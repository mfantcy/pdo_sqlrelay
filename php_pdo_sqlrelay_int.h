/*
 *   +----------------------------------------------------------------------+
 *   | Created on: 2013/8/1                                                 |
 *   | Author: ZhiMing Zhu                                                  |
 *   +----------------------------------------------------------------------+
 *    
 */

/* $Id$ */

#include <sqlrelay/sqlrclientwrapper.h>


typedef struct {
	const char *file;
	int line;
	unsigned int code;
	char *msg;
} pdo_sqlrelay_error_info;

typedef struct {
	sqlrcon conn;
	char *identify;
	int32_t conn_timeout;
	int32_t res_timeout;
    unsigned debug:1;
    unsigned fakebinds:1;
	pdo_sqlrelay_error_info err_info;
	unsigned can_bind_out:1;
} pdo_sqlrelay_db_handle;


typedef struct {
	pdo_sqlrelay_db_handle 	*H;
	sqlrcur cur;
	uint64_t result_set_buffer_size;
	int64_t row_count;
	uint64_t current_row;
	unsigned fetched:1;
	unsigned done:1;
	uint64_t first_index;
	enum pdo_fetch_orientation fetch_mode;
	pdo_sqlrelay_error_info err_info;
	unsigned has_bind:1;
	unsigned has_bindout:1;
	char *query;
	uint32_t query_len;
} pdo_sqlrelay_stmt;

typedef struct {
	char * bind_name;
	long number;
	char * name;
	unsigned bind_out:1;
}pdo_sqlrelay_param;

extern pdo_driver_t pdo_sqlrelay_driver;

extern struct pdo_stmt_methods sqlrelay_stmt_methods;
/* error handle mocro */
extern int _pdo_sqlrelay_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, sqlrcur cur, const char *file, int line TSRMLS_DC);
#define pdo_sqlrelay_error(dbh) _pdo_sqlrelay_error(dbh, NULL, NULL, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_sqlrelay_error_stmt(stmt) _pdo_sqlrelay_error(stmt->dbh, stmt, NULL, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_sqlrelay_error_cur(dbh, cur) _pdo_sqlrelay_error(dbh, NULL, cur, __FILE__, __LINE__ TSRMLS_CC)

enum {
	PDO_SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_SQLRELAY_ATTR_GET_COLUMN_INFO,
	PDO_SQLRELAY_ATTR_DB_TYPE,
	PDO_SQLRELAY_ATTR_DB_VERSION,
	PDO_SQLRELAY_ATTR_DB_HOST_NAME,
	PDO_SQLRELAY_ATTR_DB_IP_ADDRESS,
	PDO_SQLRELAY_ATTR_BIND_FORMAT,
	PDO_SQLRELAY_ATTR_CURRENT_DB,
	PDO_SQLRELAY_ATTR_CONNECT_TIMEOUT,
	PDO_SQLRELAY_ATTR_RESPONSE_TOMEOUT,
	PDO_SQLRELAY_ATTR_DEBUG,
	PDO_SQLRELAY_ATTR_FAKEBINDS,
};

