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
#include <cstring>
#include <map>

extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_var.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "zend.h"
#include "zend_exceptions.h"
#include "zend_modules.h"

#ifdef ZTS
#include "TSRM.h"
#endif

}
#include <sqlrelay/sqlrclient.h>

#if PHP_MAJOR_VERSION < 7
#define SQLR_ZVAL_STRING(a,b,c) ZVAL_STRING(a, b, c)
#define SQLR_ZVAL_STRINGL(a,b,l,c) ZVAL_STRINGL(a, b, l, c)
#define SQLR_ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a, b, c)
#define SQLR_ADD_ASSOC_STRING(a,b,c,d) add_assoc_string(a, b, c, d)
#define SQLR_Z_ISREF(a) Z_ISREF_P(a)
#define SQLR_PARAM_NAME_VAL(a) a
#define SQLR_PARAM_NAME_LEN(a) a##len
#define SQLR_PARAM_ZHASH_FIND(ht,n,pt) zend_hash_find(ht, n, n##len + 1, (void**)&pt)
#else
#define SQLR_ZVAL_STRING(a,b,c) ZVAL_STRING(a, b)
#define SQLR_ZVAL_STRINGL(a,b,l,c) ZVAL_STRINGL(a, b, l)
#define SQLR_ADD_NEXT_INDEX_STRING(a,b,c) add_next_index_string(a, b);
#define SQLR_ADD_ASSOC_STRING(a,b,c,d) add_assoc_string(a, b, c)
#define SQLR_Z_ISREF(a) Z_ISREF(a)
#define SQLR_PARAM_NAME_VAL(a) ZSTR_VAL(a)
#define SQLR_PARAM_NAME_LEN(a) ZSTR_LEN(a)
#define SQLR_PARAM_ZHASH_FIND(ht,n,pt) ((pt = (char *)zend_hash_find_ptr(ht, n)) == NULL ? FAILURE : SUCCESS)
#endif

#define PHP_PDO_SQLRELAY_MODULE_VERSION    "1.0.1"

#define SQLRELAY_PHP_TYPE_NULL 0
#define SQLRELAY_PHP_TYPE_BOOL 1
#define SQLRELAY_PHP_TYPE_LONG 2
#define SQLRELAY_PHP_TYPE_DOUBLE 3
#define SQLRELAY_PHP_TYPE_STRING 4

typedef struct _PDOSqlrelayErrorInfo
{
	uint64_t code;
	char *msg;
} PDOSqlrelayErrorInfo;

typedef struct _PDOSqlrelayHandler
{
	sqlrconnection * connection;
	char * identify;
    char * host;
    uint16_t port;
    char * socket;
    PDOSqlrelayErrorInfo errorInfo;
	int retryTime;
	int tries;
	int connectionTimeout;
	int responseTimeout;
	int authenticationTimeout;
	bool useNativeType;
    bool debug;
    bool inTnx;
} PDOSqlrelayHandler;

typedef struct _PDOSqlrelayColumn
{
	int phpType;
} PDOSqlrelayColumn;

typedef struct _PDOSqlrelayStatement
{
	sqlrcursor * cursor;
	PDOSqlrelayHandler * handler;
	PDOSqlrelayErrorInfo errorInfo;
	PDOSqlrelayColumn * columnInfo;
	uint64_t resultSetBufferSize;
	uint16_t resultSetId;
	int64_t rowCount;
	uint64_t currentRow;
	uint64_t firstIndex;
	enum pdo_fetch_orientation fetchMode;
	uint32_t numOfParams;
	uint32_t paramsGiven;
	char bindMark;
	bool countLessParam;
	bool useNativeType;
	bool fetched;
	bool done;
	bool cursorScroll;
} PDOSqlrelayStatement;

typedef struct _PDOSqlrelayParam
{
	char * bindName;
	long number;
	char * name;
	bool bindOut;
	bool isNull;
	bool isRef;
	bool hasBound;
} PDOSqlrelayParam;

extern pdo_driver_t PDOSqlrelayDriver;
extern struct pdo_dbh_methods PDOSqlrelayHandleMethods;
extern struct pdo_stmt_methods PDOSqlrelayStatementMethods;

/* error handle mocro */

extern int _setPDOSqlrelayHandlerError(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *message, const int code, const char * file, int line);
extern void freePDOSqlrelayParam(PDOSqlrelayParam *sqlrelayParam);
extern int sqlrelayDebugPrint(const char * tpl, ...);
extern int PDOSqlrelayDebugPrint(const char * tpl, ...);
extern const char * getPDOSqlState(const uint64_t);
extern int getPHPTypeByNativeType(const char *nativeType);


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
	PDO_SQLRELAY_ATTR_DB_TYPE,
	PDO_SQLRELAY_ATTR_DB_VERSION,
	PDO_SQLRELAY_ATTR_DB_HOST_NAME,
	PDO_SQLRELAY_ATTR_DB_IP_ADDRESS,
	PDO_SQLRELAY_ATTR_BIND_FORMAT,
	PDO_SQLRELAY_ATTR_CURRENT_DB,
	PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT,
	PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT,
	PDO_SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE,
};

extern zend_module_entry pdo_sqlrelay_module_entry;
#define phpext_pdo_sqlrelay_ptr &pdo_sqlrelay_module_entry

PHP_MINIT_FUNCTION(pdo_sqlrelay);
PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay);
PHP_MINFO_FUNCTION(pdo_sqlrelay);

#endif	/* PHP_PDO_SQLRELAY_H */
