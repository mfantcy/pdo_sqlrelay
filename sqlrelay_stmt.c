/*
 *   +----------------------------------------------------------------------+
 *   | Author: ZhiMing Zhu                                                  |
 *   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_sqlrelay.h"
#include "php_pdo_sqlrelay_int.h"
#include "zend_exceptions.h"

static int pdo_sqlrelay_fill_stmt(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	int64_t row_count = (int64_t) sqlrcur_rowCount(S->cur);
	if(row_count > 0)
	{
		S->row_count = stmt->row_count = row_count;
		stmt->column_count = (int) sqlrcur_colCount(S->cur);
		S->current_row = 0;
		S->fetched = 0;
		S->first_index = 0;
		S->fetch_mode = PDO_FETCH_ORI_NEXT;
		S->first_index = sqlrcur_firstRowIndex(S->cur);
		S->current_row += S->first_index;
	}
	else
	{
		stmt->row_count = (int64_t) sqlrcur_affectedRows(S->cur);
	}
	return 1;
}

/* {{{ sqlrelay_stmt_dtor */
static int sqlrelay_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	if(S->err_info.msg)
	{
		pefree(S->err_info.msg, stmt->dbh->is_persistent);
		S->err_info.msg = NULL;
	}
	if(S->query)
	{
		efree(S->query);
		S->query = NULL;
		S->query_len = 0;
	}
	if(S->cur)
	{

		sqlrcur_clearBinds(S->cur);
		sqlrcur_closeResultSet(S->cur);
		sqlrcur_free(S->cur);
	}
	efree(S);
	return 1;
}
/* }}} */

/* {{{ sqlrelay_stmt_execute */
static int sqlrelay_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE)
	{
		char *query;
		uint32_t query_len;
		if (stmt->active_query_string)
		{
			query = stmt->active_query_string;
			query_len = (uint32_t) stmt->active_query_stringlen;
		}
		else
		{
			query = stmt->query_string;
			query_len = stmt->query_stringlen;
		}
		if (!sqlrcur_sendQueryWithLength(S->cur, query, query_len) || sqlrcur_errorNumber(S->cur))
		{
			pdo_sqlrelay_error_stmt(stmt);
			return 0;
		}
		pdo_sqlrelay_fill_stmt(stmt TSRMLS_CC);
		return 1;
	}
	if(!sqlrcur_executeQuery(S->cur) || sqlrcur_errorNumber(S->cur))
	{
		pdo_sqlrelay_error_stmt(stmt);
		return 0;
	}

	pdo_sqlrelay_fill_stmt(stmt TSRMLS_CC);
	return 1;

}
/* }}} */

/* {{{ sqlrelay_stmt_fetch */
static int sqlrelay_stmt_fetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	if ((uint64_t) offset + S->first_index >= S->row_count)
	{
		return 0;
	}
	S->fetch_mode = ori;

	if (!S->fetched && offset > 0)
	{
		S->current_row = (uint64_t)offset;
	}
	switch(ori)
	{
		case PDO_FETCH_ORI_PRIOR:   /* scroll back to prior row and fetch that */
			if(S->fetched)
			{
				S->current_row--;
				if (S->current_row  < S->first_index)
				{
					return 0;
				}
			}
			break;
		case PDO_FETCH_ORI_ABS:     /* scroll to an absolute numbered row and fetch that */
			S->current_row = (uint64_t)offset + S->first_index;
			break;
		case PDO_FETCH_ORI_REL:     /* scroll relative to the current row, and fetch that */
			if(S->current_row + ((uint64_t) offset) >= S->row_count + S->first_index)
			{
				return 0;
			}
			S->current_row += ((uint64_t) offset);
			break;
		case PDO_FETCH_ORI_FIRST:   /* scroll to the first row and fetch that */
			break;
		case PDO_FETCH_ORI_LAST:    /* scroll to the last row and fetch that */
			break;
		default: /* PDO_FETCH_ORI_NEXT default: fetch the next available row */
			if(S->fetched){
				S->current_row++;
				if ( S->current_row  >= (S->row_count + S->first_index))
				{
					return 0;
				}
			}
			break;
	}

	S->fetched = 1;
	return 1;
}
/* }}} */

/* {{{ sqlrelay_stmt_describe_col */
static int sqlrelay_stmt_describe_col(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;
	if(colno >= stmt->column_count)
	{
		return 0;
	}
	if(cols[colno].name)
	{
		return 1;
	}
	char const *name = sqlrcur_getColumnName(S->cur, (uint32_t) colno);
	cols[colno].namelen = strlen(name);
	cols[colno].name = estrndup(name,cols[colno].namelen);
	cols[colno].maxlen = sqlrcur_getColumnLengthByIndex(S->cur, (uint32_t) colno);
	cols[colno].param_type = PDO_PARAM_STR;
	cols[colno].precision = (unsigned long) sqlrcur_getColumnPrecisionByIndex(S->cur, (uint32_t) colno);
	return 1;

}
/* }}} */

/* {{{ sqlrelay_stmt_get_col_data */
static int sqlrelay_stmt_get_col_data(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	uint64_t row_num = 0;
	switch(S->fetch_mode)
	{
		case PDO_FETCH_ORI_FIRST:
			row_num = S->first_index;
			break;
		case PDO_FETCH_ORI_LAST:
			row_num = S->first_index + S->row_count - 1;
			break;
		default:
			row_num = S->current_row;
			break;
	}
	*caller_frees = 1;
	*len = (unsigned long) sqlrcur_getFieldLengthByIndex(S->cur, row_num, (uint32_t) colno);
	*ptr = estrndup(sqlrcur_getFieldByIndex(S->cur, row_num, (uint32_t) colno), *len);
	if(S->H->debug)
	{
		zend_printf("col_data %s", *ptr);
	}
	if (sqlrcur_errorNumber(S->cur))
	{
		pdo_sqlrelay_error_stmt(stmt);
		return 0;
	}
	return 1;
}
/* }}} */

/* {{{ sqlrelay_stmt_param_hook */
static int sqlrelay_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE)
	{
		return 1;
	}
	pdo_sqlrelay_param *P;
	uint32_t outlen = 0;
	if(param->is_param)
	{
		char * param_name = NULL;
		switch(event_type)
		{
			case PDO_PARAM_EVT_ALLOC:
				P =  ecalloc(1, sizeof(pdo_sqlrelay_param));
				P->name = estrndup(param->name, param->namelen);
				P->number = param->paramno;
				if (stmt->supports_placeholders == PDO_PLACEHOLDER_POSITIONAL)
				{
					P->bind_name = emalloc((int)((P->number+1)/10) + 2);
					sprintf(P->bind_name, "%d", P->number+1);
				}
				else
				{
					if (param->name[0] == ':')
					{
						P->bind_name = estrndup(param->name + 1, param->namelen - 1);
					}
					else
					{
						P->bind_name = estrndup(param->name, param->namelen);
					}
				}
				param->driver_data = P;


				switch (PDO_PARAM_TYPE(param->param_type))
				{
					case PDO_PARAM_STMT:
						return 0;
					case PDO_PARAM_BOOL:
					case PDO_PARAM_INT:
						convert_to_long(param->parameter);
						sqlrcur_inputBindLong(S->cur, P->bind_name, (int64_t)Z_LVAL_P(param->parameter));
						if (S->H->can_bind_out && Z_ISREF_P(param->parameter) && (param->param_type & PDO_PARAM_INPUT_OUTPUT))
						{
							//sqlrcur_defineOutputBindBlob(S->cur, P->bind_name);
							sqlrcur_defineOutputBindInteger(S->cur, P->bind_name);
							P->bind_out = 1;
							S->has_bindout = 1;
						}
						break;

					case PDO_PARAM_NULL:
						convert_to_null(param->parameter);

						break;
					case PDO_PARAM_LOB:

						if (S->H->can_bind_out && Z_ISREF_P(param->parameter) && (param->param_type & PDO_PARAM_INPUT_OUTPUT))
						{
							sqlrcur_defineOutputBindBlob(S->cur, P->bind_name);
							P->bind_out = 1;
							S->has_bindout = 1;
						}
						if (Z_TYPE_P(param->parameter) == IS_RESOURCE)
						{
							php_stream *stm;
							php_stream_from_zval_no_verify(stm, &param->parameter);
							if (stm)
							{
								SEPARATE_ZVAL_IF_NOT_REF(&param->parameter);
								Z_TYPE_P(param->parameter) = IS_STRING;
								Z_STRLEN_P(param->parameter) = php_stream_copy_to_mem(stm, &Z_STRVAL_P(param->parameter), PHP_STREAM_COPY_ALL, 0);
							}
							else
							{
								pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource" TSRMLS_CC);
								return 0;
							}
						}
						convert_to_string(param->parameter);
						sqlrcur_inputBindBlob(S->cur, P->bind_name, Z_STRVAL_P(param->parameter) ,Z_STRLEN_P(param->parameter));
						break;

					case PDO_PARAM_STR:
					default:
						if (S->H->can_bind_out && Z_ISREF_P(param->parameter) && (param->param_type & PDO_PARAM_INPUT_OUTPUT))
						{
							sqlrcur_defineOutputBindString(S->cur, P->bind_name, (uint32_t)param->max_value_len);
							P->bind_out = 1;
							S->has_bindout = 1;
						}
						convert_to_string(param->parameter);
						sqlrcur_inputBindStringWithLength(S->cur, P->bind_name, Z_STRVAL_P(param->parameter) ,Z_STRLEN_P(param->parameter));
						break;
				}

				S->has_bind = 1;
				break;

			case PDO_PARAM_EVT_FREE:
				P = (pdo_sqlrelay_param *) param->driver_data;
				efree(P->bind_name);
				efree(P->name);
				efree(P);
				break;

			case PDO_PARAM_EVT_EXEC_POST:
				P = (pdo_sqlrelay_param *) param->driver_data;
				if (P->bind_out && Z_ISREF_P(param->parameter))
				{
					outlen = sqlrcur_getOutputBindLength(S->cur, P->bind_name);
					if(outlen)
					{
						Z_STRVAL_P(param->parameter) = estrndup(sqlrcur_getOutputBindBlob(S->cur, P->bind_name), outlen);
						Z_STRLEN_P(param->parameter) = outlen;
						Z_TYPE_P(param->parameter) = IS_STRING;
					}
				}
				break;
		}
	}
	return 1;
}
/* }}} */

/* {{{ sqlrelay_stmt_set_attr */
static int sqlrelay_stmt_set_attr(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	switch(attr)
	{
		case PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE:
			if (S->result_set_buffer_size != ((uint64_t) Z_LVAL_P(val)))
			{
				S->result_set_buffer_size = ((uint64_t) Z_LVAL_P(val));
				sqlrcur_setResultSetBufferSize(S->cur, S->result_set_buffer_size);
			}
			return 1;
		case PDO_SQLRELAY_ATTR_GET_COLUMN_INFO:
			convert_to_boolean(val);
			if (Z_BVAL_P(val)==TRUE)
			{
				sqlrcur_getColumnInfo(S->cur);
			}
			else
			{
				sqlrcur_dontGetColumnInfo(S->cur);
			}
			return 1;
	}
	return 0;
}
/* }}} */

/* {{{ sqlrelay_stmt_get_attr */
static int sqlrelay_stmt_get_attr(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	switch(attr)
	{
		case PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE:
			S->result_set_buffer_size = sqlrcur_getResultSetBufferSize(S->cur );
			ZVAL_LONG(val, (long)S->result_set_buffer_size);
			break;
	}
	return 0;
}
/* }}} */

/* {{{ sqlrelay_stmt_get_column_meta */
static int sqlrelay_stmt_get_column_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC)
{
	if (colno >= stmt->column_count) {
		/* error invalid column */
		return FAILURE;
	}

	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	zval *flags;
	array_init(return_value);
	MAKE_STD_ZVAL(flags);
	array_init(flags);

	add_assoc_string(return_value, "native_type", (char *)sqlrcur_getColumnTypeByIndex(S->cur, (uint32_t)colno), 1);

	if (sqlrcur_getColumnIsPrimaryKeyByIndex(S->cur, (uint32_t)colno))
	{
		add_next_index_string(flags, "primary_key", 1);
	}
	if (!sqlrcur_getColumnIsNullableByIndex(S->cur, (uint32_t)colno))
	{
		add_next_index_string(flags, "not_null", 1);
	}
	if (!sqlrcur_getColumnIsPartOfKeyByIndex(S->cur, (uint32_t)colno))
	{
		add_next_index_string(flags, "multiple_key", 1);
	}
	if (sqlrcur_getColumnIsUniqueByIndex(S->cur, (uint32_t)colno))
	{
		add_next_index_string(flags, "unique_key", 1);
	}
	if (sqlrcur_getColumnIsBinaryByIndex(S->cur, (uint32_t)colno))
	{
		add_next_index_string(flags, "blob", 1);
	}
	if (sqlrcur_getColumnIsAutoIncrementByIndex(S->cur, (uint32_t)colno)) {
		add_next_index_string(flags, "auto_increment", 1);
	}

	add_assoc_zval(return_value, "flags", flags);

	return SUCCESS;
}
/* }}} */



/* {{{ sqlrelay_stmt_cursor_closer */
static int sqlrelay_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *) stmt->driver_data;
	if(S->cur)
	{
		sqlrcur_closeResultSet(S->cur);
	}
	return 1;
}
/* }}} */

/* {{{ sqlrelay_stmt_methods */
struct pdo_stmt_methods sqlrelay_stmt_methods = {
		sqlrelay_stmt_dtor,
		sqlrelay_stmt_execute,
		sqlrelay_stmt_fetch,
		sqlrelay_stmt_describe_col,
		sqlrelay_stmt_get_col_data,
		sqlrelay_stmt_param_hook,
		sqlrelay_stmt_set_attr,
		sqlrelay_stmt_get_attr,
		sqlrelay_stmt_get_column_meta,
		NULL,
		sqlrelay_stmt_cursor_closer
};
/* }}} */
