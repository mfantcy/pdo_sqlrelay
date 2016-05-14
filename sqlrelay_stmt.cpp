/*
 *   +----------------------------------------------------------------------+
 *   | Author: MoonFantasy                                                  |
 *   +----------------------------------------------------------------------+
 */

#include "php_pdo_sqlrelay.hpp"

void freePDOSqlrelayParam(PDOSqlrelayParam *sqlrelayParam)
{
	if (sqlrelayParam) {
		if (sqlrelayParam->bindName) {
			efree(sqlrelayParam->bindName);
		}
		if (sqlrelayParam->name) {
			efree(sqlrelayParam->name);
		}
		efree(sqlrelayParam);
	}
}

static int PDOSqlrelayFillStmt(pdo_stmt_t *stmt TSRMLS_DC)
{
    PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
    sqlrcursor * cursor = sqlrelayStatement->cursor;
    int64_t rowCount = (int64_t) cursor->rowCount();
    stmt->column_count = (int) cursor->colCount();
    sqlrelayStatement->done = true;

	if (sqlrelayStatement->columnInfo) {
		efree(sqlrelayStatement->columnInfo);
		sqlrelayStatement->columnInfo = NULL;
	}

    if (stmt->column_count > 0) {
    	 sqlrelayStatement->columnInfo = (PDOSqlrelayColumn*) emalloc(stmt->column_count * sizeof(PDOSqlrelayColumn));
    }

    if (rowCount > 0) {
    	sqlrelayStatement->rowCount = stmt->row_count = rowCount;
    	sqlrelayStatement->fetched = false;
    	sqlrelayStatement->firstIndex = cursor->firstRowIndex();
    	sqlrelayStatement->fetchMode = PDO_FETCH_ORI_NEXT;
    	sqlrelayStatement->currentRow = sqlrelayStatement->firstIndex;
    } else {
        stmt->row_count = (int64_t) cursor->affectedRows();
    }

    return 1;
}

static int sqlrelayStatementDestructor(pdo_stmt_t *stmt TSRMLS_DC)
{
	PDOSqlrelayStatement * sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	if (sqlrelayStatement->errorInfo.msg) {
		pefree(sqlrelayStatement->errorInfo.msg, stmt->dbh->is_persistent);
		sqlrelayStatement->errorInfo.msg = NULL;
	}
	if (sqlrelayStatement->cursor) {
		sqlrelayStatement->cursor->closeResultSet();
		delete sqlrelayStatement->cursor;
	}
	if (sqlrelayStatement->columnInfo) {
		efree(sqlrelayStatement->columnInfo);
		sqlrelayStatement->columnInfo = NULL;
	}
    efree(sqlrelayStatement);
    return 1;
}

static int sqlrelayStatementExecute(pdo_stmt_t *stmt TSRMLS_DC)
{
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	sqlrcursor * cursor = sqlrelayStatement->cursor;

	if (sqlrelayStatement->paramsGiven < sqlrelayStatement->numOfParams) {
		setSqlrelayStatementErrorMsg(stmt, "Too few parameters bound", 2031); //"HY093"
		return 0;
	}
	if (!cursor->executeQuery()) {
    	setSqlrelayStatementError(stmt);
    	if (sqlrelayStatement->handler->debug) {
    		PDOSqlrelayDebugPrint("stmt->error_code %s", stmt->error_code);
    	}
        return 0;
    }
    PDOSqlrelayFillStmt(stmt TSRMLS_CC);
    return 1;
}

static int sqlrelayStatementFetch(pdo_stmt_t *stmt, enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	PDOSqlrelayStatement * sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	if ((uint64_t) offset >= sqlrelayStatement->rowCount)
		return 0;
	sqlrelayStatement->fetchMode = ori;
	sqlrelayStatement->currentRow = !sqlrelayStatement->currentRow ? offset : sqlrelayStatement->currentRow;

	switch (ori) {
        case PDO_FETCH_ORI_PRIOR:   /* scroll back to prior row and fetch that */
            if (sqlrelayStatement->fetched) {
            	sqlrelayStatement->currentRow--;
                if (sqlrelayStatement->currentRow  < sqlrelayStatement->firstIndex)
                    return 0;
            }
            break;

        case PDO_FETCH_ORI_ABS:     /* scroll to an absolute numbered row and fetch that */
        	sqlrelayStatement->currentRow = (uint64_t)offset + sqlrelayStatement->firstIndex;
            break;

        case PDO_FETCH_ORI_REL:     /* scroll relative to the current row, and fetch that */
            if (sqlrelayStatement->currentRow + ((uint64_t) offset) >= sqlrelayStatement->rowCount + sqlrelayStatement->firstIndex)
                return 0;
            sqlrelayStatement->currentRow += ((uint64_t) offset);
            break;

        case PDO_FETCH_ORI_FIRST:   /* scroll to the first row and fetch that */
        	sqlrelayStatement->currentRow = sqlrelayStatement->firstIndex;
            break;

        case PDO_FETCH_ORI_LAST:    /* scroll to the last row and fetch that */
        	sqlrelayStatement->currentRow = sqlrelayStatement->firstIndex + sqlrelayStatement->rowCount - 1;
            break;

        default: /* PDO_FETCH_ORI_NEXT default: fetch the next available row */
            if (sqlrelayStatement->fetched) {
            	sqlrelayStatement->currentRow++;
                if ( sqlrelayStatement->currentRow  >= (sqlrelayStatement->rowCount + sqlrelayStatement->firstIndex))
                    return 0;
            }
            break;
    }
	sqlrelayStatement->fetched = true;
    return 1;
}

static int sqlrelayStatementDescribColumn(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	PDOSqlrelayStatement * sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
    struct pdo_column_data *cols = stmt->columns;

    if (colno >= stmt->column_count) {
        return 0;
    }

    if (cols[colno].name) {
        return 1;
    }
    const char *name = sqlrelayStatement->cursor->getColumnName((uint32_t) colno);
#if PHP_MAJOR_VERSION < 7
    cols[colno].namelen = strlen(name);
    cols[colno].name = estrndup(name,cols[colno].namelen);
#else
    cols[colno].name = zend_string_init(name, strlen(name), 0);
#endif
    cols[colno].maxlen = sqlrelayStatement->cursor->getColumnLength((uint32_t) colno);

    bool isUnsigned = sqlrelayStatement->cursor->getColumnIsUnsigned((uint32_t) colno);
    const char * sqlrelayType = sqlrelayStatement->cursor->getColumnType((uint32_t) colno);

    if (isUnsigned && strcmp("BIGINT", sqlrelayType) == 0)
    	sqlrelayStatement->columnInfo[colno].phpType = SQLRELAY_PHP_TYPE_STRING;
    else
    	sqlrelayStatement->columnInfo[colno].phpType = getPHPTypeByNativeType(sqlrelayType);

    if (sqlrelayStatement->useNativeType){
		switch (sqlrelayStatement->columnInfo[colno].phpType) {
		case SQLRELAY_PHP_TYPE_BOOL:
			cols[colno].param_type = PDO_PARAM_BOOL;
			break;
		case SQLRELAY_PHP_TYPE_DOUBLE:
			cols[colno].param_type = PDO_PARAM_ZVAL;
			break;
		case SQLRELAY_PHP_TYPE_LONG:
			cols[colno].param_type = PDO_PARAM_INT;
			break;
		case SQLRELAY_PHP_TYPE_NULL:
			cols[colno].param_type = PDO_PARAM_NULL;
			break;
		default:
			cols[colno].param_type = PDO_PARAM_STR;
		}

    } else {
    	sqlrelayStatement->useNativeType = false;
    	cols[colno].param_type = PDO_PARAM_STR;
    }


    cols[colno].precision = (unsigned long) sqlrelayStatement->cursor->getColumnPrecision((uint32_t) colno);
    return 1;
}

#if PHP_MAJOR_VERSION < 7
static int sqlrelayStatementGetColumnData(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
#else
static int sqlrelayStatementGetColumnData(pdo_stmt_t *stmt, int colno, char **ptr, size_t *len, int *caller_frees TSRMLS_DC)
#endif
{
    if (colno >= stmt->column_count) {
        return 0;
    }
	struct pdo_column_data *col;
	int type, phpType;
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	uint64_t rowNum = sqlrelayStatement->currentRow;
	*caller_frees = 1;
	char * result = NULL;

	col = &stmt->columns[colno];
	type = PDO_PARAM_TYPE(col->param_type);

	result = (char *)sqlrelayStatement->cursor->getField(rowNum, (uint32_t) colno);
	if (sqlrelayStatement->useNativeType) {

		if (result == NULL) {
			*ptr = NULL;
			*len = 0;
		} else {
			switch (type) {
			case PDO_PARAM_NULL:
				*ptr = NULL;
				*len = 0;
				break;

			case PDO_PARAM_ZVAL:
				*len = sizeof(zval);
				zval *zResult;
#if PHP_MAJOR_VERSION < 7
				MAKE_STD_ZVAL(zResult);
#else
				zResult = (zval *) emalloc(sizeof(zval));
#endif
				ZVAL_DOUBLE(zResult, sqlrelayStatement->cursor->getFieldAsDouble(rowNum, (uint32_t) colno));
				*ptr = (char *)zResult;
				break;

			case PDO_PARAM_BOOL:
				zend_bool *boolResult;
				boolResult = (zend_bool *)emalloc(sizeof(zend_bool));
				*boolResult = (sqlrelayStatement->cursor->getFieldAsInteger(rowNum, (uint32_t) colno) == 0) ? 1: 0;
				*ptr = (char *)boolResult;
				*len = sizeof(zend_bool);
				break;

			case PDO_PARAM_INT:
				int64_t *intResult;
				intResult = (int64_t *) emalloc(sizeof(int64_t));
				*intResult = sqlrelayStatement->cursor->getFieldAsInteger(rowNum, (uint32_t) colno);
				*ptr = (char *)intResult;
#if PHP_MAJOR_VERSION < 7
				*len = sizeof(long);
#else
				*len = sizeof(zend_long);
#endif
				break;

			default:
				*len = (unsigned long)sqlrelayStatement->cursor->getFieldLength(rowNum, (uint32_t) colno);
				*ptr = estrndup(result, *len);
			}
		}

	} else {
		if (result == NULL) {
			*len = 0;
			*ptr = NULL;
		} else {
			*len = (unsigned long)sqlrelayStatement->cursor->getFieldLength(rowNum, (uint32_t) colno);
			*ptr = estrndup(result, *len);
		}
	}
	if (sqlrelayStatement->cursor->errorNumber()) {
		return 0;
	}
	return 1;

}

static int sqlrelayStatementHandlePostBindOut(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event eventType TSRMLS_DC)
{
	PDOSqlrelayParam * sqlrelayParam = (PDOSqlrelayParam *)param->driver_data;
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;

	if (!(param->param_type & PDO_PARAM_INPUT_OUTPUT) || !sqlrelayParam->bindOut || !SQLR_Z_ISREF(param->parameter))
		return 1;

	switch (PDO_PARAM_TYPE(param->param_type)) {
	case PDO_PARAM_STMT:
		return 0;

	case PDO_PARAM_BOOL:
	case PDO_PARAM_INT:
#if PHP_MAJOR_VERSION < 7
		ZVAL_LONG(param->parameter, sqlrelayStatement->cursor->getOutputBindInteger(sqlrelayParam->bindName));
#else
		ZVAL_LONG(&param->parameter, sqlrelayStatement->cursor->getOutputBindInteger(sqlrelayParam->bindName));
#endif
		break;

	case PDO_PARAM_NULL:
	case PDO_PARAM_LOB:
	case PDO_PARAM_STR:
	default:
		sqlrelayStatement->cursor->getOutputBindString(sqlrelayParam->bindName);
		break;

	}

	return 1;
}

static int sqlrelayStatementHandlePreBindOut(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event eventType TSRMLS_DC)
{
	PDOSqlrelayParam * sqlrelayParam = (PDOSqlrelayParam *)param->driver_data;
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;

	switch (PDO_PARAM_TYPE(param->param_type)) {
	case PDO_PARAM_STMT:
		return 0;

	case PDO_PARAM_BOOL:
	case PDO_PARAM_INT:
		sqlrelayStatement->cursor->defineOutputBindInteger(sqlrelayParam->bindName);
		break;

	case PDO_PARAM_NULL:
	case PDO_PARAM_LOB:
	case PDO_PARAM_STR:
	default:
		sqlrelayStatement->cursor->defineOutputBindString(sqlrelayParam->bindName, (uint32_t)param->max_value_len);
		break;

	}
	sqlrelayParam->bindOut = true;

	return 1;
}

static int sqlrelayStatementHandlePreBindIn(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event eventType TSRMLS_DC)
{
	PDOSqlrelayParam * sqlrelayParam = (PDOSqlrelayParam *)param->driver_data;
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	zval *parameter;
#if PHP_MAJOR_VERSION < 7
	if (Z_TYPE_P(param->parameter) == IS_NULL) {
		sqlrelayStatement->cursor->inputBind(sqlrelayParam->bindName,(const char *)NULL);
		return 1;
	}
	parameter = param->parameter;
#else
	if (!Z_ISREF(param->parameter)) {
		parameter = &param->parameter;
	} else {
		parameter = Z_REFVAL(param->parameter);
	}
	if (Z_TYPE_P(parameter) == IS_NULL) {
		sqlrelayStatement->cursor->inputBind(sqlrelayParam->bindName,(const char *)NULL);
		return 1;
	}
#endif
	switch (PDO_PARAM_TYPE(param->param_type)) {
	case PDO_PARAM_STMT:
		return 0;

	case PDO_PARAM_BOOL:
	case PDO_PARAM_INT:
		sqlrelayStatement->cursor->inputBind(sqlrelayParam->bindName, (int64_t)Z_LVAL_P(parameter));
		break;

	case PDO_PARAM_NULL:
		sqlrelayStatement->cursor->inputBind(sqlrelayParam->bindName,(const char *)NULL);
		break;

	case PDO_PARAM_LOB:
#if PHP_MAJOR_VERSION < 7
		if (Z_TYPE_P(param->parameter) == IS_RESOURCE) {
			php_stream *phpStream;
			php_stream_from_zval_no_verify(phpStream, &param->parameter);
			if (phpStream) {
				SEPARATE_ZVAL_IF_NOT_REF(&param->parameter);
				Z_TYPE_P(param->parameter) = IS_STRING;
				Z_STRLEN_P(param->parameter) = php_stream_copy_to_mem(phpStream, &Z_STRVAL_P(param->parameter), PHP_STREAM_COPY_ALL, 0);
			} else {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource" TSRMLS_CC);
				return 0;
			}
			sqlrelayStatement->cursor->inputBindBlob(sqlrelayParam->bindName, Z_STRVAL_P(param->parameter), (uint32_t)Z_STRLEN_P(param->parameter));
		}
#else
		if (Z_TYPE_P(parameter) == IS_RESOURCE) {
			php_stream *phpStream = NULL;
			php_stream_from_zval_no_verify(phpStream, parameter);
			if (phpStream) {
				zend_string *mem = php_stream_copy_to_mem(phpStream, PHP_STREAM_COPY_ALL, 0);
				zval_ptr_dtor(parameter);
				ZVAL_STR(parameter, mem ? mem : ZSTR_EMPTY_ALLOC());
			} else {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY105", "Expected a stream resource");
				return 0;
			}
			sqlrelayStatement->cursor->inputBindBlob(sqlrelayParam->bindName, Z_STRVAL_P(parameter), (uint32_t)Z_STRLEN_P(parameter));
		}
#endif
		break;

	case PDO_PARAM_STR:
	default:
		/* I dont like this ...... just mark it
		char * value = Z_STRVAL_P(parameter);
		bool useBlob = false;
		for (uint32_t i = 0; i < ((uint32_t)Z_STRLEN_P(parameter)); i++) {
			if (value[i] == '\0') {
				useBlob = true;
				break;
			}
		}
		if (useBlob)
			sqlrelayStatement->cursor->inputBindBlob(sqlrelayParam->bindName, Z_STRVAL_P(parameter), (uint32_t)Z_STRLEN_P(parameter));
		else
		*/
		sqlrelayStatement->cursor->inputBind(sqlrelayParam->bindName, Z_STRVAL_P(parameter), (uint32_t)Z_STRLEN_P(parameter));
		break;
	}
	if (sqlrelayStatement->handler->debug) {
		PDOSqlrelayDebugPrint("bind %s", sqlrelayParam->bindName);
	}
	return 1;
}

static int sqlrelayStatementParamHook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param, enum pdo_param_event eventType TSRMLS_DC)
{
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
    if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE) {
        return 1;
    }

    if (param->is_param) {
		PDOSqlrelayParam * sqlrelayParam;
		if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_STMT)
			return 0;

		switch (eventType) {
		case PDO_PARAM_EVT_ALLOC:
			if (param->paramno < 0 || param->paramno >= sqlrelayStatement->numOfParams) {
				pdo_raise_impl_error(stmt->dbh, stmt, "HY093", "Too many parameters were bound" TSRMLS_CC);
				return 0;
			}

			sqlrelayParam = (PDOSqlrelayParam*)emalloc(sizeof(PDOSqlrelayParam));
			sqlrelayParam->bindName = NULL;
			sqlrelayParam->name = NULL;
			param->driver_data = sqlrelayParam;

			if (stmt->supports_placeholders == PDO_PLACEHOLDER_NAMED && param->name) {
#if PHP_MAJOR_VERSION < 7
			sqlrelayParam->name = estrndup(param->name, param->namelen);
			if (param->name[0] == ':')
				sqlrelayParam->bindName = estrndup(param->name + 1, param->namelen - 1);
			else
				sqlrelayParam->bindName = estrndup(param->name, param->namelen);
#else
			sqlrelayParam->name = estrndup((char *)ZSTR_VAL(param->name), ZSTR_LEN(param->name));
			if (ZSTR_VAL(param->name)[0] == ':')
				sqlrelayParam->bindName = estrndup(ZSTR_VAL(param->name) + 1, ZSTR_LEN(param->name) - 1);
			else
				sqlrelayParam->bindName = estrndup(ZSTR_VAL(param->name), ZSTR_LEN(param->name));
#endif
			} else if (stmt->supports_placeholders == PDO_PLACEHOLDER_POSITIONAL && param->paramno >= 0) {
				sqlrelayParam->number = param->paramno;
				sqlrelayParam->bindName = (char *)emalloc((int)((sqlrelayParam->number + 1) / 10) + 2);
				sprintf(sqlrelayParam->bindName, "%d", sqlrelayParam->number + 1);
			}

			if (sqlrelayStatementHandlePreBindIn(stmt, param, eventType TSRMLS_CC) != 1)
				return 0;

			if (SQLR_Z_ISREF(param->parameter) && (param->param_type & PDO_PARAM_INPUT_OUTPUT)) {
				if (sqlrelayStatementHandlePreBindOut(stmt, param, eventType TSRMLS_CC) != 1)
					return 0;
			}
			sqlrelayStatement->paramsGiven++;
			break;

		case PDO_PARAM_EVT_FREE:
			if (param->driver_data)
				freePDOSqlrelayParam((PDOSqlrelayParam *)param->driver_data);
			param->driver_data = NULL;
			sqlrelayStatement->paramsGiven--;
			break;

		case PDO_PARAM_EVT_EXEC_PRE:
			break;
		case PDO_PARAM_EVT_EXEC_POST:
			if (SQLR_Z_ISREF(param->parameter) && (param->param_type & PDO_PARAM_INPUT_OUTPUT)
			) {
				if (sqlrelayStatementHandlePostBindOut(stmt, param, eventType TSRMLS_CC) != 1)
					return 0;
			}
			break;

		case PDO_PARAM_EVT_FETCH_PRE:
		case PDO_PARAM_EVT_FETCH_POST:
		case PDO_PARAM_EVT_NORMALIZE:

			break;
		}
    }
	return 1;
}

static int sqlrelayStatementSetAttribute(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	sqlrcursor * cursor = (sqlrcursor *) sqlrelayStatement->cursor;
	switch (attr) {

	case PDO_SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE:
		if (sqlrelayStatement->columnInfo != NULL) {
			pdo_raise_impl_error(stmt->dbh, stmt, "HY000", "This Statement has been executed, can not change this attribute" TSRMLS_CC);
			return -1;
		}
		if (sqlrelayStatement->useNativeType ^ (bool) Z_LVAL_P(val)) {
			sqlrelayStatement->useNativeType = (bool) Z_LVAL_P(val);
		}
		break;

	default:
		return 0;
	}
	return 1;
}
static int sqlrelayStatementGetAttribute(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{

	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
	sqlrcursor * cursor = (sqlrcursor *) sqlrelayStatement->cursor;
	switch (attr) {
	case PDO_SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE:
		ZVAL_BOOL(val, sqlrelayStatement->useNativeType);
		break;
	default:
		return 0;
	}
	return 1;
}

#if PHP_MAJOR_VERSION < 7
static int sqlrelayStatementGetColumnMeta(pdo_stmt_t *stmt, long colno, zval *returnValue TSRMLS_DC)
#else
static int sqlrelayStatementGetColumnMeta(pdo_stmt_t *stmt, zend_long colno, zval *returnValue TSRMLS_DC)
#endif
{
    PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;

    if (!sqlrelayStatement->done || colno >= stmt->column_count) {
        /* error invalid column */
        return FAILURE;
    }

    array_init(returnValue);
#if PHP_MAJOR_VERSION < 7
    zval *flags;
    MAKE_STD_ZVAL(flags);
#else
    zval _flags;
#define flags &_flags
#endif
    array_init(flags);

    if (sqlrelayStatement->cursor->getColumnIsPrimaryKey((uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "primary_key", 1);

    if (!sqlrelayStatement->cursor->getColumnIsNullable( (uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "not_null", 1);

    if (sqlrelayStatement->cursor->getColumnIsPartOfKey((uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "multiple_key", 1);

    if (sqlrelayStatement->cursor->getColumnIsUnique((uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "unique_key", 1);

    if (sqlrelayStatement->cursor->getColumnIsBinary((uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "blob", 1);

    if (sqlrelayStatement->cursor->getColumnIsAutoIncrement((uint32_t)colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "auto_increment", 1);

    if (sqlrelayStatement->cursor->getColumnIsUnsigned((uint32_t) colno))
    	SQLR_ADD_NEXT_INDEX_STRING(flags, "unsigned", 1);

    SQLR_ADD_ASSOC_STRING(returnValue, "native_type", (char *)sqlrelayStatement->cursor->getColumnType((uint32_t)colno), 1);
    add_assoc_zval(returnValue, "flags", flags);

    return SUCCESS;
}

struct pdo_stmt_methods PDOSqlrelayStatementMethods = {
        sqlrelayStatementDestructor,
        sqlrelayStatementExecute,
        sqlrelayStatementFetch,
        sqlrelayStatementDescribColumn,
        sqlrelayStatementGetColumnData,
		sqlrelayStatementParamHook,
		sqlrelayStatementSetAttribute,
		sqlrelayStatementGetAttribute,
		sqlrelayStatementGetColumnMeta,
		NULL,
		NULL
};
