/*
 *   +----------------------------------------------------------------------+
 *   | Author: MoonFantasy                                                  |
 *   +----------------------------------------------------------------------+
 */

#ifndef PHP_PDO_SQLRELAY_H
#define PHP_PDO_SQLRELAY_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <iostream>
#include <string>
#include <cstdlib>

extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stdio.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "pdo/php_pdo_error.h"
#include "zend_exceptions.h"
#include "zend_modules.h"

#ifdef ZTS
#include "TSRM.h"
#endif

}
#include <sqlrelay/sqlrclient.h>


#if PHP_MAJOR_VERSION < 7
#define SQLR_ZVAL_STRING(a,b,c) ZVAL_STRING(a, b, c)
#define SQLR_ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a, b, c)
#define SQLR_ADD_ASSOC_STRING(a,b,c,d) add_assoc_string(a, b, c, d)
#define SQLR_Z_ISREF(a) Z_ISREF_P(a)
#define SQLR_ZVAL_LONG(a, b) ZVAL_LONG(a, b);
#else
#define SQLR_ZVAL_STRING(a,b,c) ZVAL_STRING(a, b)
#define SQLR_ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a, b);
#define SQLR_ADD_ASSOC_STRING(a,b,c,d) add_assoc_string(a, b, c)
#define SQLR_Z_ISREF(a) Z_ISREF(a)
#define SQLR_ZVAL_LONG(a, b) ZVAL_LONG(a, b);
#endif

#define PHP_PDO_SQLRELAY_MODULE_VERSION    "1.0.0"

typedef struct PDOSqlrelayErrorInfo
{
	char *file;
	int line;
	int code;
	char *msg;
};

typedef struct PDOSqlrelayHandler
{
	sqlrconnection * connection;
	char * identify;
    char * host;
    uint16_t port;
    char * socket;
    PDOSqlrelayErrorInfo * errorInfo;
	int retryTime;
	int tries;
	int connectionTimeout;
	int responseTimeout;
	int authenticationTimeout;
    bool debug;
    bool inTnx;
};


typedef struct PDOSqlrelayStatement
{
	sqlrcursor * cursor;
	PDOSqlrelayHandler * handler;
	PDOSqlrelayErrorInfo * errorInfo;
	uint64_t resultSetBufferSize;
	uint16_t resultSetId;
	int64_t rowCount;
	uint64_t currentRow;
	uint64_t firstIndex;
	enum pdo_fetch_orientation fetchMode;
	const char * query;
	long queryLen;
	bool hasBind;
	bool hasBindout;
	bool fetched;
	bool done;
	bool cursorScroll;
};

typedef struct PDOSqlrelayParam
{
	char * bindName;
	long number;
	char * name;
	bool bindOut;
};

extern pdo_driver_t PDOSqlrelayDriver;
extern struct pdo_dbh_methods PDOSqlrelayHandleMethods;
extern struct pdo_stmt_methods PDOSqlrelayStatementMethods;

/* error handle mocro */

extern int _setPDOSqlrelayHandlerError(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *message, const int code, const char * file, int line);
extern void freePDOSqlrelayErrorInfo(PDOSqlrelayErrorInfo * errorInfo);
extern void freePDOSqlrelayParam(PDOSqlrelayParam *sqlrelayParam);
extern int sqlrelayDebugPrint(const char * tpl, ...);
extern int PDOSqlrelayDebugPrint(const char * tpl, ...);



#ifndef __FUNCTION_NAME__
    #ifdef __PRETTY_FUNCTION__
        #define __FUNCTION_NAME__   __PRETTY_FUNCTION__
	#elif defined  __FUNCTION__
		#define __FUNCTION_NAME__   __FUNCTION__
    #elif defined __func__
        #define __FUNCTION_NAME__   __func__
	#else
		#define __FUNCTION_NAME__   "unknowfunc"
    #endif
#endif


#define setSqlrelayHandlerError(h) _setPDOSqlrelayHandlerError(h, NULL, NULL, 0, __FILE__, __LINE__)
#define setSqlrelayHandlerErrorMsg(h, m, c) _setPDOSqlrelayHandlerError(h, NULL, m, c, __FILE__, __LINE__)
#define setSqlrelayStatementError(s) _setPDOSqlrelayHandlerError(s->dbh, s, NULL, 0, __FILE__, __LINE__)
#define setSqlrelayStatementErrorMsg(s, m, c) _setPDOSqlrelayHandlerError(s->dbh, s, m, c, __FILE__, __LINE__)
#define PDO_SQLRELAY_DEBUG_FORMAT(a, b) PDOSqlrelayDebugPrint("file:%s ,line:%d, func:%s ," \
		a, __FILE__, __LINE__, __FUNCTION_NAME__, b)
#define PDO_SQLRELAY_DEBUG() PDOSqlrelayDebugPrint("file:%s ,line:%d, func:%s", __FILE__, __LINE__, __FUNCTION_NAME__)

enum {
	PDO_SQLRELAY_ATTR_DEBUG = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_SQLRELAY_ATTR_GET_COLUMN_INFO,
	PDO_SQLRELAY_ATTR_DB_TYPE,
	PDO_SQLRELAY_ATTR_DB_VERSION,
	PDO_SQLRELAY_ATTR_DB_HOST_NAME,
	PDO_SQLRELAY_ATTR_DB_IP_ADDRESS,
	PDO_SQLRELAY_ATTR_BIND_FORMAT,
	PDO_SQLRELAY_ATTR_CURRENT_DB,
	PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT,
	PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT,
};


extern zend_module_entry pdo_sqlrelay_module_entry;
#define phpext_pdo_sqlrelay_ptr &pdo_sqlrelay_module_entry

PHP_MINIT_FUNCTION(pdo_sqlrelay);
PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay);
PHP_MINFO_FUNCTION(pdo_sqlrelay);



#endif	/* PHP_PDO_SQLRELAY_H */
