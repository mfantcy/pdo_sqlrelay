/*
 *   +----------------------------------------------------------------------+
 *   | Author: MoonFantasy                                                  |
 *   +----------------------------------------------------------------------+
 *
 */

/* $Id$ */

#include "php_pdo_sqlrelay.hpp"

int sqlrelayDebugPrint(const char * tpl, ...)
{
	int ret;
	va_list args;
	va_start (args, tpl);
	ret = vprintf (tpl, args);
	va_end (args);
	return ret;
}

int PDOSqlrelayDebugPrint(const char * tpl, ...)
{
	int ret;
	char buff[500];
	va_list args;
	va_start (args, tpl);
	sprintf(buff, "PDO_SQLRELAY_DEBUG: %s\n", tpl);
	ret = vprintf (buff, args);
	va_end (args);
	return ret;
}

void freePDOSqlrelayErrorInfo(PDOSqlrelayErrorInfo * errorInfo)
{
	if (errorInfo) {
		if (errorInfo->file) {
			efree(errorInfo->file);
		}
		if (errorInfo->msg) {
			efree(errorInfo->msg);
		}
		efree(errorInfo);
	}

}

int _setPDOSqlrelayHandlerError(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *message, const int code, const char * file, int line)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	pdo_error_type *pdoError;
	PDOSqlrelayErrorInfo *errorInfo;
	sqlrconnection *sqlrelayConnection;
	sqlrcursor *sqlrelayCursor = NULL;

	sqlrelayConnection = sqlrelayHandler->connection;

	if (stmt) {
		PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
		sqlrelayCursor = sqlrelayStatement->cursor;
		pdoError = &stmt->error_code;
		if (sqlrelayStatement->errorInfo)
			freePDOSqlrelayErrorInfo(sqlrelayStatement->errorInfo);

		sqlrelayStatement->errorInfo = (PDOSqlrelayErrorInfo*) emalloc(sizeof(PDOSqlrelayErrorInfo));
		errorInfo = sqlrelayStatement->errorInfo;
	} else {
		pdoError = &dbh->error_code;
		if (sqlrelayHandler->errorInfo)
			freePDOSqlrelayErrorInfo(sqlrelayHandler->errorInfo);

		sqlrelayHandler->errorInfo = (PDOSqlrelayErrorInfo*) emalloc(sizeof(PDOSqlrelayErrorInfo));
		errorInfo = sqlrelayHandler->errorInfo;
	}

	if (message) {
		errorInfo->msg = estrndup(message, strlen(message));
		errorInfo->code = code;
	} else {
		if (sqlrelayCursor->errorNumber()) {
			errorInfo->msg = estrdup(sqlrelayCursor->errorMessage());
			errorInfo->code = sqlrelayCursor->errorNumber();
		} else if (sqlrelayConnection->errorNumber()) {
			errorInfo->msg = estrdup(sqlrelayConnection->errorMessage());
			errorInfo->code = sqlrelayConnection->errorNumber();
		} else {
			strcpy(*pdoError, PDO_ERR_NONE);
			return 0;
		}
	}
	strcpy(*pdoError, "SQLRe");
	PDOSqlrelayDebugPrint(errorInfo->msg);
	errorInfo->line =  line;
	errorInfo->file = estrndup(file, strlen(file));
	return 1;
}



static int sqlrealyHandlerClose(pdo_dbh_t *dbh TSRMLS_DC)
{

	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	if (!sqlrelayHandler)
		return 0;

	if (sqlrelayHandler->connection) {
		delete (sqlrconnection *)sqlrelayHandler->connection;
		sqlrelayHandler->connection = NULL;
	}

	if (sqlrelayHandler->identify)
		pefree(sqlrelayHandler->identify, dbh->is_persistent);

	if (sqlrelayHandler->host)
		pefree(sqlrelayHandler->host, dbh->is_persistent);

	if (sqlrelayHandler->socket)
		pefree(sqlrelayHandler->socket, dbh->is_persistent);

	pefree(sqlrelayHandler, dbh->is_persistent);

	return 1;

}

static int sqlrelayHandlerPrepare(
		pdo_dbh_t *dbh,
		const char *sql,
#if PHP_MAJOR_VERSION < 7
		long sqlLen,
#else
		size_t sqlLen,
#endif
		pdo_stmt_t *stmt,
		zval *driverOptions TSRMLS_DC)
{

	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	PDOSqlrelayStatement *sqlrelayStatement = (PDOSqlrelayStatement*) ecalloc(1, sizeof(PDOSqlrelayStatement));

	sqlrcursor * sqlrelsyCursor = new sqlrcursor(sqlrelayHandler->connection, true);
	sqlrelsyCursor->setResultSetBufferSize(0);
	sqlrelsyCursor->getColumnInfo();
	sqlrelsyCursor->prepareQuery(sql, sqlLen);

	sqlrelayStatement->cursor = sqlrelsyCursor;
	sqlrelayStatement->handler = sqlrelayHandler;
	sqlrelayStatement->query = sql;
	sqlrelayStatement->queryLen = sqlLen;
	sqlrelayStatement->done = false;
	sqlrelayStatement->cursorScroll = pdo_attr_lval(driverOptions, PDO_ATTR_CURSOR, PDO_CURSOR_SCROLL TSRMLS_CC) == PDO_CURSOR_SCROLL;

	stmt->driver_data = sqlrelayStatement;
	stmt->methods = &PDOSqlrelayStatementMethods;
	stmt->column_count = 0;
	stmt->columns = NULL;
	stmt->row_count = 0;
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;

	return 1;
}
#if PHP_MAJOR_VERSION < 7
static long sqlrelayHandlerExec(pdo_dbh_t *dbh, const char *sql, long sqlLen TSRMLS_DC)
#else
static zend_long sqlrelayHandlerExec(pdo_dbh_t *dbh, const char *sql, size_t sqlLen)
#endif
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	sqlrcursor sqlrelayCursor(sqlrelayHandler->connection);
	if (!sqlrelayCursor.sendQuery(sql, sqlLen)) {
		setSqlrelayHandlerErrorMsg(dbh, sqlrelayCursor.errorMessage(), (const int)sqlrelayCursor.errorNumber());
		return -1;
	}

#if PHP_MAJOR_VERSION < 7
	return sqlrelayCursor.affectedRows();
#else
	return (zend_long)sqlrelayCursor.affectedRows();
#endif
}

static int sqlrealyHandlerBegin(pdo_dbh_t *dbh TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;

	if (!sqlrelayHandler->connection->begin())
		return 0;

	sqlrelayHandler->inTnx = true;

	return 1;
}

static int sqlrelayHandlerCommit(pdo_dbh_t *dbh TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	if (!sqlrelayHandler->connection->commit())
		return 0;

	sqlrelayHandler->inTnx = false;


	return 1;
}

static int sqlrelayHandlerRollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	if (!sqlrelayHandler->connection->rollback())
		return 0;

	sqlrelayHandler->inTnx = false;

	return 1;
}

static int sqlrelayHandlerSetAttr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	switch (attr) {
	case PDO_ATTR_AUTOCOMMIT:
		convert_to_boolean(val);
		if (dbh->auto_commit ^ ((bool)Z_LVAL_P(val))) {
			dbh->auto_commit = (bool)Z_LVAL_P(val);
			if(dbh->auto_commit) {
				sqlrelayHandler->connection->autoCommitOn();
			} else {
				sqlrelayHandler->connection->autoCommitOff();
			}
			break;
		}
		return 1;
	case PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT:
		if (sqlrelayHandler->connectionTimeout != ((int) Z_LVAL_P(val))) {
			sqlrelayHandler->connectionTimeout = (int) Z_LVAL_P(val);
			sqlrelayHandler->connection->setConnectTimeout(sqlrelayHandler->connectionTimeout, 0);
			break;
		}
		return 1;

	case PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT:
		if (sqlrelayHandler->responseTimeout != ((int) Z_LVAL_P(val))) {
			sqlrelayHandler->responseTimeout = (int32_t) Z_LVAL_P(val);
			sqlrelayHandler->connection->setResponseTimeout(sqlrelayHandler->responseTimeout, 0);
			break;
		}
		return 1;

	case PDO_SQLRELAY_ATTR_DEBUG:
		if (sqlrelayHandler->debug ^ ((bool)Z_LVAL_P(val))) {
			sqlrelayHandler->debug = (bool)Z_LVAL_P(val);
			if (sqlrelayHandler->debug) {
				sqlrelayHandler->connection->debugOn();
				sqlrelayHandler->connection->debugPrintFunction(sqlrelayDebugPrint);
			} else {
				sqlrelayHandler->connection->debugOff();;
			}
			break;
		}
		return 1;
	default:
		return 0;
	}

	if (sqlrelayHandler->connection->errorNumber())
		return -1;

	return 0;
}


static char *sqlrealyHandlerLastId(
		pdo_dbh_t *dbh,
		const char *name,
#if PHP_MAJOR_VERSION < 7
		unsigned int *len
#else
		size_t*len
#endif
		TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	pdo_int64_t lastInsertId = (pdo_int64_t) sqlrelayHandler->connection->getLastInsertId();
	if (sqlrelayHandler->connection->errorNumber()) {
		return NULL;
	}
	char * id = php_pdo_int64_to_str(lastInsertId TSRMLS_CC);
	*len = strlen(id);

	return id;
}

static int sqlrealyHandlerFetchError(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	PDOSqlrelayStatement *sqlrelayStatement;
	long errorNumber = 0;
	const char * errorMessage;

	if (stmt) {
		sqlrelayStatement = (PDOSqlrelayStatement *) stmt->driver_data;
		if (sqlrelayStatement->errorInfo) {
			errorNumber = sqlrelayStatement->errorInfo->code;
			errorMessage = sqlrelayStatement->errorInfo->msg;
		} else {
			errorNumber = (long)sqlrelayStatement->cursor->errorNumber();
			errorMessage = sqlrelayStatement->cursor->errorMessage();
		}
	}

	if (!errorMessage) {
		if (sqlrelayHandler->errorInfo) {
			errorNumber = sqlrelayHandler->errorInfo->code;
			errorMessage = sqlrelayHandler->errorInfo->msg;
		} else {
			errorNumber = (long)sqlrelayHandler->connection->errorNumber();
			errorMessage = sqlrelayHandler->connection->errorMessage();
		}
	}

	if (!errorMessage)
		return 0;

	add_next_index_long(info, errorNumber);
	SQLR_ADD_NEXT_INDEX_STRING(info, errorMessage, 1);

	return 1;
}

static int sqlrelayHandlerGetAttr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	switch (attr) {
	case PDO_ATTR_CLIENT_VERSION:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->clientVersion(), 1);
		break;

	case PDO_ATTR_SERVER_VERSION:
		SQLR_ZVAL_STRING(val, (char *) sqlrelayHandler->connection->serverVersion(), 1);
		break;

	case PDO_ATTR_CONNECTION_STATUS:
		ZVAL_BOOL(val, sqlrelayHandler->connection->ping());
		return 1;
		break;

	case PDO_ATTR_SERVER_INFO:
		char info[50];
		sprintf(info, "%s %s", sqlrelayHandler->identify, sqlrelayHandler->connection->serverVersion());
		SQLR_ZVAL_STRING(val, info, 1);
		break;

	case PDO_SQLRELAY_ATTR_DB_TYPE:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->identify, 1);
		break;

	case PDO_SQLRELAY_ATTR_DB_VERSION:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->dbVersion(), 1);
		break;

	case PDO_SQLRELAY_ATTR_DB_HOST_NAME:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->dbHostName(), 1);
		break;

	case PDO_SQLRELAY_ATTR_DB_IP_ADDRESS:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->dbIpAddress(), 1);
		break;

	case PDO_SQLRELAY_ATTR_BIND_FORMAT:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->bindFormat(), 1);
		break;

	case PDO_SQLRELAY_ATTR_CURRENT_DB:
		SQLR_ZVAL_STRING(val, sqlrelayHandler->connection->getCurrentDatabase() ,1);
		break;

	case PDO_ATTR_AUTOCOMMIT:
		ZVAL_LONG(val, dbh->auto_commit);
		return 1;
		break;

	case PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT:
		ZVAL_LONG(val, (long)sqlrelayHandler->connectionTimeout);
		return 1;
		break;

	case PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT:
		ZVAL_LONG(val, (long)sqlrelayHandler->responseTimeout);
		return 1;
		break;

	case PDO_SQLRELAY_ATTR_DEBUG:
		ZVAL_BOOL(val, sqlrelayHandler->debug);
		return 1;
		break;

	default:
		return 0;
	}

	if (!sqlrelayHandler->connection->errorNumber())
			return -1;
	return 1;
}

static int sqlrelayHandlerCheckLiveness(pdo_dbh_t *dbh TSRMLS_DC)
{

	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	if (sqlrelayHandler->connection->ping())
		return SUCCESS;

	return FAILURE;

}


static int sqlrelayHandlerInTransaction(pdo_dbh_t *dbh TSRMLS_DC)
{

	PDOSqlrelayHandler *sqlrelayHandler = (PDOSqlrelayHandler *) dbh->driver_data;
	return (int) sqlrelayHandler->inTnx;

}

static struct pdo_dbh_methods sqlrelay_methods = {
        sqlrealyHandlerClose,
		sqlrelayHandlerPrepare,
		sqlrelayHandlerExec,
		NULL,
		sqlrealyHandlerBegin,
		sqlrelayHandlerCommit,
		sqlrelayHandlerRollback,
		sqlrelayHandlerSetAttr,
		sqlrealyHandlerLastId,
		sqlrealyHandlerFetchError,
		sqlrelayHandlerGetAttr,
        sqlrelayHandlerCheckLiveness,
		NULL,
        NULL,
		sqlrelayHandlerInTransaction
};

static int parseDataSourceToHandler(pdo_dbh_t *dbh, PDOSqlrelayHandler *sqlrelayHandler)
{
	int atoiTmp;
    sqlrelayHandler->connection = NULL;
    sqlrelayHandler->identify = NULL;
    sqlrelayHandler->port = 9000;
    sqlrelayHandler->socket = NULL;
    sqlrelayHandler->authenticationTimeout = 5;
    sqlrelayHandler->inTnx = false;

	struct pdo_data_src_parser vars[] = {
        { "host",        NULL,        0 },
        { "port",        NULL,        0 },
        { "socket",      NULL,        0 },
		{ "conntimeout", "5",        0 },
		{ "resptimout",  "30",        0 },
        { "retrytime",   "2",         0 },
        { "tries",       "1",         0 },
        { "debug",       "0",         0 },
    };

    php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 8);

    if (vars[2].optval) {
    	sqlrelayHandler->socket = pestrdup(vars[2].optval, dbh->is_persistent);
    	sqlrelayHandler->port = 0;
    } else {
        if (vars[0].optval)
        	sqlrelayHandler->host = pestrdup(vars[0].optval, dbh->is_persistent);
        else
        	sqlrelayHandler->host = pestrdup("localhost", dbh->is_persistent);

        if (vars[1].optval)
        	sqlrelayHandler->port = (uint16_t)((atoiTmp = atoi(vars[1].optval)) > 0 ? atoiTmp : sqlrelayHandler->port);
    }

    sqlrelayHandler->connectionTimeout = (atoiTmp = atoi(vars[3].optval)) > 0 ? atoiTmp : 5;
    sqlrelayHandler->responseTimeout = (atoiTmp = atoi(vars[4].optval)) > 0 ? atoiTmp : 30;
    sqlrelayHandler->retryTime = (atoiTmp = atoi(vars[5].optval)) > 0 ? atoiTmp : 2;
    sqlrelayHandler->tries = (atoiTmp = atoi(vars[6].optval)) > 0 ? atoiTmp : 1;
    sqlrelayHandler->debug = (bool)((atoiTmp = atoi(vars[7].optval)) >= 0 ? atoiTmp : 0);

	for (int i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}
	return 1;
}

static int setInitPDOOptions(zval *driver_options, PDOSqlrelayHandler *sqlrelayHandler TSRMLS_DC)
{
    if (driver_options) {
    	sqlrelayHandler->connectionTimeout = (int) pdo_attr_lval(driver_options, (enum pdo_attribute_type)PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT, 60 TSRMLS_CC);
    	sqlrelayHandler->responseTimeout = (int) pdo_attr_lval(driver_options, (enum pdo_attribute_type)PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT, 30 TSRMLS_CC);
    	sqlrelayHandler->debug = (bool) pdo_attr_lval(driver_options, (enum pdo_attribute_type)PDO_SQLRELAY_ATTR_DEBUG, 0 TSRMLS_CC);
    }
	return 1;
}

static int PDOSqlrelayHandlerFactory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC)
{

    int i, ret = 0;
    PDOSqlrelayHandler *sqlrelayHandler;
    sqlrconnection *sqlrelayConnection;

    do {
		sqlrelayHandler = (PDOSqlrelayHandler *)pecalloc(1, sizeof(PDOSqlrelayHandler), dbh->is_persistent);

		if (!parseDataSourceToHandler(dbh, sqlrelayHandler))
			break;

		if (!setInitPDOOptions(driver_options, sqlrelayHandler TSRMLS_CC))
			break;

		sqlrelayHandler->connection = new sqlrconnection(
				sqlrelayHandler->host,
				sqlrelayHandler->port,
				sqlrelayHandler->socket,
				dbh->username,
				dbh->password,
				sqlrelayHandler->retryTime,
				sqlrelayHandler->tries,
				true
		);

		if (!sqlrelayHandler->connection) {
			zend_throw_exception(php_pdo_get_exception(), "Connect to SQL Relay Fail", 0 TSRMLS_CC);
			break;
		}

		sqlrelayConnection = sqlrelayHandler->connection;
		sqlrelayConnection->setConnectTimeout(sqlrelayHandler->connectionTimeout, 0);
		sqlrelayConnection->setResponseTimeout(sqlrelayHandler->responseTimeout, 0);
		sqlrelayConnection->setAuthenticationTimeout(sqlrelayHandler->authenticationTimeout, 0);
		dbh->driver_data = sqlrelayHandler;
		PDOSqlrelayDebugPrint("set debug:::: %d", sqlrelayHandler->debug);
		if (sqlrelayHandler->debug) {
			sqlrelayConnection->debugOn();
			sqlrelayConnection->debugPrintFunction(sqlrelayDebugPrint);
			PDOSqlrelayDebugPrint("connection to host:%s port:%d socket:%s username:%s password:%s retytime:%d tries:%d debug:%d\n",
					sqlrelayHandler->host,
					sqlrelayHandler->port,
					sqlrelayHandler->socket,
					dbh->username,
					dbh->password,
					sqlrelayHandler->retryTime,
					sqlrelayHandler->tries,
					(int)sqlrelayHandler->debug
			);
			PDOSqlrelayDebugPrint("bind format: %s\n", sqlrelayConnection->bindFormat());
		} else {
			sqlrelayConnection->debugOff();
		}


		if (!sqlrelayConnection->ping()) {
			zend_throw_exception(php_pdo_get_exception(), sqlrelayConnection->errorMessage(), (long)sqlrelayConnection->errorNumber() TSRMLS_CC);
			break;
		}

		sqlrelayHandler->identify = pestrdup(sqlrelayConnection->identify(), dbh->is_persistent);

		sqlrelayConnection->setClientInfo("PHP PDO Sql Relay client");

		if (dbh->auto_commit) {
			sqlrelayConnection->autoCommitOn();
		} else {
			sqlrelayConnection->autoCommitOff();
		}

		if (sqlrelayConnection->errorMessage()) {
			zend_throw_exception(php_pdo_get_exception(), sqlrelayConnection->errorMessage(), (long)sqlrelayConnection->errorNumber() TSRMLS_CC);
			break;
		}


		ret = 1;
    } while(0);

	dbh->methods = &sqlrelay_methods;
    return ret;
}

pdo_driver_t PDOSqlrelayDriver = {
    PDO_DRIVER_HEADER(sqlrelay),
	PDOSqlrelayHandlerFactory
};
