/*
 *   +----------------------------------------------------------------------+
 *   | Author: ZhiMing Zhu                                                  |
 *   +----------------------------------------------------------------------+
 *
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_sqlrelay.h"
#include "php_pdo_sqlrelay_int.h"
#include "zend_exceptions.h"


/* {{{ _pdo_sqlrelay_error */
int _pdo_sqlrelay_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, sqlrcur cur, const char *file, int line TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *)dbh->driver_data;
	pdo_error_type *pdo_err;
	pdo_sqlrelay_error_info *err_info;
	pdo_sqlrelay_stmt *S = NULL;

	if (stmt)
	{
		S = (pdo_sqlrelay_stmt*) stmt->driver_data;

		if (S->err_info.msg)
		{
			pefree(S->err_info.msg, dbh->is_persistent);
			S->err_info.msg = NULL;
		}

		pdo_err = &stmt->error_code;
		err_info   = &S->err_info;

		if ((err_info->code = (unsigned int) sqlrcur_errorNumber(S->cur)))
		{
			err_info->msg =  pestrdup(sqlrcur_errorMessage(S->cur), dbh->is_persistent);
			/* realloc sql relay cursor */
			sqlrcur_free(S->cur);
			S->cur = sqlrcur_alloc(H->conn);
		}
	}
	else
	{
		if (H->err_info.msg)
		{
			pefree(H->err_info.msg, dbh->is_persistent);
			H->err_info.msg = NULL;
		}
		pdo_err = &dbh->error_code;
		err_info   = &H->err_info;
		if(cur && (err_info->code = (unsigned int) sqlrcur_errorNumber(cur)))
		{
			err_info->msg =  pestrdup( sqlrcur_errorMessage(cur), dbh->is_persistent);
		}
		else if((err_info->code = (unsigned int) sqlrcon_errorNumber(H->conn)))
		{
			err_info->msg =  pestrdup( sqlrcon_errorMessage(H->conn), dbh->is_persistent);
		}

	}

	if (!err_info->code) { /* no error on sqlrelay */
		strcpy(*pdo_err, PDO_ERR_NONE);
		return 0;
	}
	err_info->file = file;
	err_info->line = line;
	/* so many SQLSTATE must mapping so just use HY000 */
	strcpy(*pdo_err, "HY000");
	if (!dbh->methods)
	{
		zend_throw_exception_ex(php_pdo_get_exception(), err_info->code TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, err_info->code, err_info->msg);
	}
	return err_info->code;
}
/* }}} */


/* {{{ sqlrelay_dbh_close */
static int sqlrelay_dbh_close(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	if (H)
	{
		if (H->conn)
		{
			sqlrcon_endSession(H->conn);
			sqlrcon_free(H->conn);
			H->conn = NULL;
		}
		if (H->identify)
		{
			pefree(H->identify, dbh->is_persistent);
		}

		if (H->err_info.msg)
		{
			pefree(H->err_info.msg, dbh->is_persistent);
			H->err_info.msg = NULL;
		}
		pefree (H, dbh->is_persistent);
		dbh->driver_data = NULL;
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ sqlrelay_dbh_prepare */
static int sqlrelay_dbh_prepare(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	pdo_sqlrelay_stmt *S = ecalloc(1, sizeof(pdo_sqlrelay_stmt));

	char *nsql = NULL;
	int nsql_len = 0;
	int ret;
	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlrelay_stmt_methods;
	S->cur = sqlrcur_alloc(H->conn);
	if(!S->cur)
	{
		pdo_sqlrelay_error(dbh);
		return 0;
	}
	//sqlrcur_getColumnInfo(S->cur);
	S->result_set_buffer_size = sqlrcur_getResultSetBufferSize(S->cur);
	if(H->fakebinds)
	{
		stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
	}
	else
	{
		if (strstr("oracle8|sqlite", H->identify))
		{
			stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
		}
		else if (strstr("postgresql", H->identify))
		{
			stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
			stmt->named_rewrite_template = "$%d";
		}
		else if (strstr("firebird|mysql|db2", H->identify))
		{
			stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;
		}
		else if( strstr("freetds|sybase", H->identify))
		{
			stmt->supports_placeholders = PDO_PLACEHOLDER_NAMED;
			stmt->named_rewrite_template = "@%s";
		}
		else
		{
			stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
		}
	}

	if (stmt->supports_placeholders == PDO_PLACEHOLDER_NONE)
	{
		return 1;
	}

	ret = pdo_parse_params(stmt, (char *)sql, sql_len, &nsql, &nsql_len TSRMLS_CC);

	if (ret == 1) {
		/* query was rewritten */
		S->query = estrndup(nsql, nsql_len);
		S->query[nsql_len] = '\0';
		S->query_len = nsql_len;
		efree(nsql);

	} else if (ret == -1) {
		/* failed to parse */
		strcpy(dbh->error_code, stmt->error_code);
		return 0;
	}
	else
	{
		S->query = estrndup(sql, sql_len);
		S->query[sql_len] = '\0';
		S->query_len = sql_len;
	}

	sqlrcur_prepareQueryWithLength(S->cur, S->query, S->query_len);

	if(sqlrcur_errorNumber(S->cur))
	{
		pdo_sqlrelay_error_stmt(stmt);
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ sqlrelay_dbh_do */
static long sqlrelay_dbh_do(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	int64_t c = 0;
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	sqlrcur cur;
	if((cur = sqlrcur_alloc(H->conn)))
	{
		if (!sqlrcur_sendQueryWithLength(cur, sql, (uint32_t)sql_len) ||  sqlrcur_errorNumber(cur))
		{
			pdo_sqlrelay_error_cur(dbh, cur);
			return -1;
		}

		c = sqlrcur_affectedRows(cur);
		if (c == -1)
		{
			c = sqlrcur_rowCount(cur);
		}
		sqlrcur_free(cur);
		return (long) c;
	}
	return -1;
}
/* }}} */

/* {{{ sqlrelay_dbh_begin */
static int sqlrelay_dbh_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	if (strstr("mysql", H->identify) && sqlrcon_autoCommitOff(H->conn))
	{
		return 1;
	}
	else if(sqlrcon_begin(H->conn))
	{
		return 1;
	}
	return (0 == pdo_sqlrelay_error(dbh));
}
/* }}} */

/* {{{ sqlrelay_dbh_commit */
static int sqlrelay_dbh_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	if(sqlrcon_commit(H->conn))
	{
		if(strstr("mysql", H->identify) && dbh->auto_commit){
			sqlrcon_autoCommitOn(H->conn);
		}
		return 1;
	}
	return (0 == pdo_sqlrelay_error(dbh));
}
/* }}} */

/* {{{ sqlrelay_dbh_rollback */
static int sqlrelay_dbh_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	if(sqlrcon_rollback(H->conn))
	{
		if(strstr("mysql", H->identify) && dbh->auto_commit){
			sqlrcon_autoCommitOn(H->conn);
		}
		return 1;
	}
	return (0 == pdo_sqlrelay_error(dbh));
}
/* }}} */

/* {{{ sqlrelay_dbh_set_attr */
static int sqlrelay_dbh_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	switch (attr) {

		case PDO_ATTR_AUTOCOMMIT:
			convert_to_boolean(val);
			if (dbh->auto_commit ^ Z_BVAL_P(val)) {
				dbh->auto_commit = Z_BVAL_P(val);
				if(dbh->auto_commit)
				{
					sqlrcon_autoCommitOn(H->conn);
				}
				else
				{
					sqlrcon_autoCommitOff(H->conn);
				}
			}
			return 1;
		case PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT:
			if(H->conn_timeout != ((int32_t) Z_LVAL_P(val)))
			{
				H->conn_timeout = (int32_t) Z_LVAL_P(val);
				sqlrcon_setConnectTimeout(H->conn, H->conn_timeout, 0);
			}
			return 1;

		case PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT:
			if(H->res_timeout != ((int32_t) Z_LVAL_P(val)))
			{
				H->res_timeout = (int32_t) Z_LVAL_P(val);
				sqlrcon_setResponseTimeout(H->conn, H->res_timeout, 0);
			}
			return 1;

		case PDO_SQLRELAY_ATTR_DEBUG:
			if(H->debug ^ Z_BVAL_P(val))
			{
				H->debug = Z_BVAL_P(val);
				if(	H->debug)
				{
					sqlrcon_debugOn(H->conn);
					sqlrcon_debugPrintFunction(H->conn, zend_printf);
				}
				else
				{
					sqlrcon_debugOff(H->conn);
				}
			}
			return 1;
		case PDO_SQLRELAY_ATTR_FAKEBINDS:
			H->fakebinds = Z_BVAL_P(val);
			return 1;
		default:
			return 0;
	}
	return 0;

}
/* }}} */

/* {{{ *sqlrelay_dbh_last_id */
static char *sqlrelay_dbh_last_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	uint64_t insert_id = sqlrcon_getLastInsertId(H->conn);
	if(sqlrcon_errorNumber(H->conn))
	{
		pdo_sqlrelay_error(dbh);
		return NULL;
	}

	char *id = php_pdo_int64_to_str((pdo_int64_t) insert_id TSRMLS_CC);
	*len = strlen(id);
	return id;
}
/* }}} */

/* {{{ sqlrelay_dbh_fetch_error */
static int sqlrelay_dbh_fetch_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	pdo_sqlrelay_error_info *einfo;

	if(stmt)
	{
		pdo_sqlrelay_stmt *S = (pdo_sqlrelay_stmt *)stmt->driver_data;
		einfo = &S->err_info;

	}
	else
	{
		einfo = &H->err_info;
	}

	if (einfo->code)
	{
		add_next_index_long(info, einfo->code);
		add_next_index_string(info, einfo->msg, 1);
	}
	return 1;
}
/* }}} */

/* {{{ sqlrelay_dbh_get_attr */
static int sqlrelay_dbh_get_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	char *info;
	const char *db_version;
	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
			ZVAL_STRING(val, (char *) sqlrcon_clientVersion(H->conn), 1);
			break;

		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(val, (char *) sqlrcon_serverVersion(H->conn), 1);
			break;

		case PDO_ATTR_CONNECTION_STATUS:
			ZVAL_BOOL(val, sqlrcon_ping(H->conn));
			break;

		case PDO_ATTR_SERVER_INFO:
			db_version = sqlrcon_dbVersion(H->conn);
			info = emalloc(strlen(H->identify) + strlen(db_version) + 2);
			sprintf(info, "%s %s", H->identify, db_version);
			ZVAL_STRING(val, info, 1);
			efree(info);
			break;

		case PDO_SQLRELAY_ATTR_DB_TYPE:
			ZVAL_STRING(val, H->identify, 1);
			break;

		case PDO_SQLRELAY_ATTR_DB_VERSION:
			ZVAL_STRING(val, (char *)sqlrcon_dbVersion(H->conn), 1);
			break;

		case PDO_SQLRELAY_ATTR_DB_HOST_NAME:
			ZVAL_STRING(val, (char *)sqlrcon_dbHostName(H->conn), 1);
			break;

		case PDO_SQLRELAY_ATTR_DB_IP_ADDRESS:
			ZVAL_STRING(val, (char *)sqlrcon_dbIpAddress(H->conn), 1);
			break;

		case PDO_SQLRELAY_ATTR_BIND_FORMAT:
			ZVAL_STRING(val, (char *)sqlrcon_bindFormat(H->conn), 1);
			break;

		case PDO_SQLRELAY_ATTR_CURRENT_DB:
			ZVAL_STRING(val, (char *)sqlrcon_getCurrentDatabase(H->conn) ,1);
			break;

		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(val, dbh->auto_commit);
			break;

		case PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT:
			ZVAL_LONG(val, (long)H->conn_timeout);
			break;

		case PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT:
			ZVAL_LONG(val, (long)H->res_timeout);
			break;

		case PDO_SQLRELAY_ATTR_DEBUG:
			ZVAL_BOOL(val, H->debug);
			break;

		case PDO_SQLRELAY_ATTR_FAKEBINDS:
			ZVAL_BOOL(val, H->fakebinds);
			break;

		default:
			return 0;
	}
	return 1;
}
/* }}} */

/* {{{ sqlrelay_dbh_check_liveness */
static int sqlrelay_dbh_check_liveness(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	if(sqlrcon_ping(H->conn))
		return SUCCESS;
	return FAILURE;
}
/* }}} */

/* {{{ sqlrelay_dbh_persistent_shutdown */
static void sqlrelay_dbh_persistent_shutdown(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlrelay_db_handle *H = (pdo_sqlrelay_db_handle *) dbh->driver_data;
	sqlrcon_endSession(H->conn);
}
/* }}} */


/* {{{ sqlrelay_methods */
static struct pdo_dbh_methods sqlrelay_methods = {
		sqlrelay_dbh_close,
		sqlrelay_dbh_prepare,
		sqlrelay_dbh_do,
		NULL,
		sqlrelay_dbh_begin,
		sqlrelay_dbh_commit,
		sqlrelay_dbh_rollback,
		sqlrelay_dbh_set_attr,
		sqlrelay_dbh_last_id,
		sqlrelay_dbh_fetch_error,
		sqlrelay_dbh_get_attr,
		sqlrelay_dbh_check_liveness,
		sqlrelay_dbh_persistent_shutdown,
		NULL
};
/* }}} */

/* {{{ pdo_sqlrelay_handle_factory */
static int pdo_sqlrelay_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC)
{
	int i, ret = 0;
	pdo_sqlrelay_db_handle *H;
	char *host = NULL, *socket = "";
	uint16_t port = 9000;
	char *dbname;
	int32_t retrytime = 0;
	int32_t tries = 1;

	struct pdo_data_src_parser vars[] = {
		{ "dbname",     "",	         0 },
		{ "host",       "",          0 },
		{ "port",       "0",	     0 },
		{ "socket",     "",	         0 },
		{ "retrytime",  "0",         0 },
		{ "tries",      "1",         0 },
		{ "debug",      "0",         0 },
	};

	dbh->methods = &sqlrelay_methods;
	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 7);

	H = pecalloc(1, sizeof(pdo_sqlrelay_db_handle), dbh->is_persistent);
	H->err_info.code = 0;
	H->err_info.msg = NULL;
	H->debug = 0;
	H->conn_timeout = 60;
	H->res_timeout = 30;
	dbname = vars[0].optval;
	if(vars[3].optval)
	{
		socket = vars[3].optval;
	}
	else if(vars[1].optval)
	{
		host = vars[1].optval;
		port = 9000;
		if(vars[2].optval)
		{
			port = (uint16_t) atoi(vars[2].optval);
		}

	}

	if(vars[4].optval)
	{
		retrytime = (int32_t) atoi(vars[4].optval);
	}
	if(vars[5].optval)
	{
		tries = (int32_t) atoi(vars[5].optval);
	}

	if(vars[6].optval && atoi(vars[6].optval) != 0)
	{
		H->debug = 1;
	}
	//H->conn = sqlrcon_alloc("localhost",9000,"","user1","password1",0,1);
	H->conn = sqlrcon_alloc(host, port, socket, dbh->username, dbh->password, retrytime, tries);
	if(!H->conn)
	{
		pdo_raise_impl_error(dbh, NULL, "HY000", "Connect to SQL Relay Fail" TSRMLS_CC);
		sqlrcon_free(H->conn);
		H->conn = NULL;
		goto cleanup;
	}

	dbh->driver_data = H;
	if (driver_options)
	{
		H->conn_timeout = (int32_t) pdo_attr_lval(driver_options, PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT, 60 TSRMLS_CC);
		H->res_timeout = (int32_t) pdo_attr_lval(driver_options, PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT, 30 TSRMLS_CC);
		H->fakebinds = pdo_attr_lval(driver_options, PDO_SQLRELAY_ATTR_FAKEBINDS, 0 TSRMLS_CC);
		H->debug = pdo_attr_lval(driver_options, PDO_SQLRELAY_ATTR_DEBUG, 0 TSRMLS_CC);
	}

	if (H->debug)
	{
		sqlrcon_debugOn(H->conn);
		sqlrcon_debugPrintFunction(H->conn, zend_printf);
		zend_printf("connection to host:%s port:%d socket:%s username:%s password:%s retytime:%d tries:%d debug:%s\n", host, port, socket, dbh->username, dbh->password, retrytime, tries, vars[6].optval);
		zend_printf("bind format: %s\n", sqlrcon_bindFormat(H->conn));
	}
	else
	{
		sqlrcon_debugOff(H->conn);
	}

	if(sqlrcon_ping(H->conn) == 0)
	{
		const char * errmsg = sqlrcon_errorMessage(H->conn);
		pdo_raise_impl_error(dbh, NULL, "HY000", errmsg TSRMLS_CC);
		sqlrcon_free(H->conn);
		H->conn = NULL;
		goto cleanup;
	}
	sqlrcon_setConnectTimeout(H->conn, H->conn_timeout, 0);
	sqlrcon_setResponseTimeout(H->conn, H->res_timeout, 0);
	H->can_bind_out = 0;
	H->identify = pestrdup(sqlrcon_identify(H->conn), dbh->is_persistent);
	if (strstr("oracle8|freetds|sybase|odbc|db2|firebird|router", H->identify))
	{
		H->can_bind_out = 1;
	}
	sqlrcon_setClientInfo(H->conn, "PHP PDO Sql Relay client");
	/* set auto commit */
	if (!dbh->auto_commit)
	{
		sqlrcon_autoCommitOff(H->conn);
	}
	else
	{
		sqlrcon_autoCommitOn(H->conn);
	}
	ret = 1;
	cleanup:
		for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
		{
			if (vars[i].freeme)
			{
				efree(vars[i].optval);
			}
		}
		dbh->methods = &sqlrelay_methods;
	return ret;
}
/* }}} */

pdo_driver_t pdo_sqlrelay_driver = {
	PDO_DRIVER_HEADER(sqlrelay),
	pdo_sqlrelay_handle_factory
};
