/*
 *   +----------------------------------------------------------------------+
 *   | Author: MoonFantasy                                                  |
 *   +----------------------------------------------------------------------+
 *
 */

/* $Id$ */

#include "php_pdo_sqlrelay.hpp"


/* {{{ pdo_sqlrelay_functions[] */
const zend_function_entry pdo_sqlrelay_functions[] = {
    PHP_FE_END
};
/* }}} */

/* {{{ pdo_sqlrelay_module_entry */
zend_module_entry pdo_sqlrelay_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "pdo_sqlrelay",
    pdo_sqlrelay_functions,
    PHP_MINIT(pdo_sqlrelay),
    PHP_MSHUTDOWN(pdo_sqlrelay),
    NULL,
    NULL,
    PHP_MINFO(pdo_sqlrelay),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PDO_SQLRELAY_MODULE_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_SQLRELAY
ZEND_GET_MODULE(pdo_sqlrelay)
#endif

/* setup pdo class const */
/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_sqlrelay)
{
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_GET_COLUMN_INFO",         (long)PDO_SQLRELAY_ATTR_GET_COLUMN_INFO);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_TYPE",                 (long)PDO_SQLRELAY_ATTR_DB_TYPE);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_VERSION",              (long)PDO_SQLRELAY_ATTR_DB_VERSION);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_HOST_NAME",            (long)PDO_SQLRELAY_ATTR_DB_HOST_NAME);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DB_IP_ADDRESS",           (long)PDO_SQLRELAY_ATTR_DB_IP_ADDRESS);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_BIND_FORMAT",             (long)PDO_SQLRELAY_ATTR_BIND_FORMAT);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_CURRENT_DB",              (long)PDO_SQLRELAY_ATTR_CURRENT_DB);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_CONNECT_TIMEOUT",         (long)PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_RESPONSE_TOMEOUT",        (long)PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT);
    REGISTER_PDO_CLASS_CONST_LONG("SQLRELAY_ATTR_DEBUG",                   (long)PDO_SQLRELAY_ATTR_DEBUG);
    return php_pdo_register_driver(&PDOSqlrelayDriver);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo_sqlrelay)
{
    php_pdo_unregister_driver(&PDOSqlrelayDriver);
    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo_sqlrelay)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "pdo_sqlrelay support", "enabled");
    php_info_print_table_row(2, "Version", PHP_PDO_SQLRELAY_MODULE_VERSION);
    php_info_print_table_row(2, "SQL Relay API Version", SQLRELAY_VERSION);
    php_info_print_table_end();
}
/* }}} */
