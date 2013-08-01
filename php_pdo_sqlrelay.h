/*
 *   +----------------------------------------------------------------------+
 *   | Created on: 2013/8/1                                                 |
 *   | Author: ZhiMing Zhu                                                  |
 *   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_PDO_SQLRELAY_H
#define PHP_PDO_SQLRELAY_H

extern zend_module_entry pdo_sqlrelay_module_entry;
#define phpext_pdo_sqlrelay_ptr &pdo_sqlrelay_module_entry

#ifdef PHP_WIN32
#	define PHP_PDO_SQLRELAY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PDO_SQLRELAY_API __attribute__ ((visibility("default")))
#else
#	define PHP_PDO_SQLRELAY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_sqlrelay);
PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay);
PHP_MINFO_FUNCTION(pdo_sqlrelay);

#ifdef ZTS
#define PDO_SQLRELAY_G(v) TSRMG(pdo_sqlrelay_globals_id, zend_pdo_sqlrelay_globals *, v)
#else
#define PDO_SQLRELAY_G(v) (pdo_sqlrelay_globals.v)
#endif

#endif	/* PHP_PDO_SQLRELAY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
