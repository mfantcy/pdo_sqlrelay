##Testing PHP PDO SQLRelay
before you run ```make test```
You can set the following environment variables to run the tests.
For example:

```Shell
SQRMYSQL_DSN=sqlrelay:socket=/tmp/sqlrelaymysqltest.socket;tries=1;retrytime=1;debug=0
SQRMYSQL_USER=mysqltest
SQRMYSQL_PASS=mysqltest
make test
```
or

```Shell
SQRMYSQL_DSN=
SQRMYSQL_USE_LOCAL=y
SQRMYSQL_LOCAL_BIN=/usr/local/firstwork/bin
SQRMYSQL_MYSQL_HOST=127.0.0.1
SQRMYSQL_MYSQL_PORT=3306
SQRMYSQL_MYSQL_USER=testuser
SQRMYSQL_MYSQL_PASS=testpass
make test
```
or

```Shell
SQRMYSQL_DSN=
SQRMYSQL_USE_LOCAL=n
SQRMYSQL_HOST=127.0.0.1
SQRMYSQL_PORT=90000
SQRMYSQL_USER=user
SQRMYSQL_PASS=pass
make test
```
------
####ENV description
#####SQRMYSQL_DSN
pdo_sqlrelay dsn string
For example:

```Shell
SQRMYSQL_DSN=sqlrelay:socket=/tmp/sqlrelaymysqltest.socket;tries=1;retrytime=1;debug=0
```
------
#####SQRMYSQL_USE_LOCAL
using local sqlrelay for testing [y|n] [default:y] ,
If 'y' it will auto startup sqlrelay daemon when testing
For example:

```Shell
SQRMYSQL_USE_LOCAL=y
```
------
#####SQRMYSQL_LOCAL_BIN
require ````SQRMYSQL_USE_LOCAL=y````,
path to local sqlrelay bin/ [default:/usr/local/firstwork/bin]
For example:

```Shell
SQRMYSQL_LOCAL_BIN=/usr/local/firstwork/bin
```
------
#####SQRMYSQL_MYSQL_SOCKET
require ````SQRMYSQL_USE_LOCAL=y````,
mysql database server socket path [default: ]
For example:

```Shell
SQRMYSQL_MYSQL_SOCKET=/tmp/mysqld.sock
```
------
#####SQRMYSQL_MYSQL_HOST
require ````SQRMYSQL_USE_LOCAL=y SQRMYSQL_MYSQL_SOCKET=````, mysql database host[default:localhost] 
For example:

```Shell
SQRMYSQL_MYSQL_HOST=127.0.0.1
```
------
#####SQRMYSQL_MYSQL_PORT
require ````SQRMYSQL_USE_LOCAL=y SQRMYSQL_MYSQL_SOCKET=````, mysql database port[default:3306] 
For example:

```Shell
SQRMYSQL_MYSQL_PORT=3306
```
------
#####SQRMYSQL_MYSQL_USER
require ````SQRMYSQL_USE_LOCAL=y````,
mysql database user [default:root]
For example:

```Shell
SQRMYSQL_MYSQL_USER=testuser
```
#####SQRMYSQL_MYSQL_PASS
require ````SQRMYSQL_USE_LOCAL=y````,
mysql database password [default: ]
For example:

```Shell
SQRMYSQL_MYSQL_PASS=testpass
```
------
#####SQRMYSQL_SOCKET
require ````SQRMYSQL_USE_LOCAL=n````,
sqlrelay local socket path [default: ]
For example:

```Shell
SQRMYSQL_SOCKET=/tmp/sqlrelaytest.sock
```
------
#####SQRMYSQL_HOST
require ````SQRMYSQL_USE_LOCAL=n SQRMYSQL_SOCKET=````,
remote sqlrelay host [default:localhost]
For example:

```Shell
SQRMYSQL_HOST=127.0.0.1
```
------
#####SQRMYSQL_PORT
require ````SQRMYSQL_USE_LOCAL=n SQRMYSQL_SOCKET=````,
remote sqlrelay port [default:90000]
For example:

```Shell
SQRMYSQL_PORT=90001
```
------
#####SQRMYSQL_USER
require ````SQRMYSQL_USE_LOCAL=n````,
remote sqlrelay user [default:mysqltest]
For example:

```Shell
SQRMYSQL_USER=mysqltest
```
#####SQRMYSQL_PASS
require ````SQRMYSQL_USE_LOCAL=n````,
remote sqlrelay password [default:mysqltest]
For example:

```Shell
SQRMYSQL_PASS=mysqltest
```
------
#####SQRMYSQL_DB
mysql database name or schema for testing [default:test]
For example:

```Shell
SQRMYSQL_DB=test
```
------
#####SQRMYSQL_STORAGE_ENGIN
mysql table storage engin will be created when testing [default:innodb]
For example:

```Shell
SQRMYSQL_DB=innodb
```
------