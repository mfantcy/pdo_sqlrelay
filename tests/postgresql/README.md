##Testing PHP PDO SQLRelay POSTGRESQL
before you run ```make test```
sqlrelay config suggestion

*````instances/instance```` tag set attribute ````fakeinputbindvariables="no"````
*````connections/connection```` postgresql databases, the connect string ````fakebinds=no```` and ````typemangling=yes```` don't Overrides ````identity````  


You can set the following environment variables to run the tests.
For example:

```Shell
export SQRPGSQL_DSN=sqlrelay:socket=/tmp/sqlrelaypostgresqltest.socket;tries=1;retrytime=1;debug=0
export SQRPGSQL_USER=pgsqltest
export SQRPGSQL_PASS=pgsqltest
export make test
```
or

```Shell
export SQRPGSQL_DSN=
export SQRPGSQL_USE_LOCAL=y
export SQRPGSQL_LOCAL_BIN=/usr/local/firstwork/bin
export SQRPGSQL_PGSQL_HOST=localhost
export SQRPGSQL_PGSQL_PORT=5432
export SQRPGSQL_PGSQL_USER=testuser
export SQRPGSQL_PGSQL_PASS=testpass
make test
```
or

```Shell
export SQRPGSQL_DSN=
export SQRPGSQL_USE_LOCAL=n
export SQRPGSQL_HOST=127.0.0.1
export SQRPGSQL_PORT=90000
export SQRPGSQL_USER=user
export SQRPGSQL_PASS=pass
make test
```
------
####ENV description
#####SQRPGSQL_DSN
pdo_sqlrelay dsn string
For example:

```Shell
SQRPGSQL_DSN=sqlrelay:socket=/tmp/sqlrelaypostgresqltest.socket;tries=1;retrytime=1;debug=0
```
------
#####SQRPGSQL_USE_LOCAL
using local sqlrelay for testing [y|n] [default:y] ,
If 'y' it will auto startup sqlrelay daemon when testing
For example:

```Shell
SQRPGSQL_USE_LOCAL=y
```
------
#####SQRPGSQL_LOCAL_BIN
require ````SQRPGSQL_USE_LOCAL=y````,
path to local sqlrelay bin/ [default:/usr/local/firstwork/bin]
For example:

```Shell
SQRPGSQL_LOCAL_BIN=/usr/local/firstwork/bin
```
------
#####SQRPGSQL_PGSQL_HOST
require ````SQRPGSQL_USE_LOCAL=y````, postgresql database host[default:localhost] 
For example:

```Shell
SQRPGSQL_PGSQL_HOST=127.0.0.1
```
------
#####SQRPGSQL_PGSQL_PORT
require ````SQRPGSQL_USE_LOCAL=y````, postgresql database port[default:3306] 
For example:

```Shell
SQRPGSQL_PGSQL_PORT=3306
```
------
#####SQRPGSQL_PGSQL_USER
require ````SQRPGSQL_USE_LOCAL=y````,
postgresql database user [default:root]
For example:

```Shell
SQRPGSQL_PGSQL_USER=testuser
```
#####SQRPGSQL_PGSQL_PASS
require ````SQRPGSQL_USE_LOCAL=y````,
postgresql database password [default: ]
For example:

```Shell
SQRPGSQL_PGSQL_PASS=testpass
```
------
#####SQRPGSQL_SOCKET
require ````SQRPGSQL_USE_LOCAL=n````,
sqlrelay local socket path [default: ]
For example:

```Shell
SQRPGSQL_SOCKET=/tmp/sqlrelaytest.sock
```
------
#####SQRPGSQL_HOST
require ````SQRPGSQL_USE_LOCAL=n SQRPGSQL_SOCKET=````,
remote sqlrelay host [default:localhost]
For example:

```Shell
SQRPGSQL_HOST=127.0.0.1
```
------
#####SQRPGSQL_PORT
require ````SQRPGSQL_USE_LOCAL=n SQRPGSQL_SOCKET=````,
remote sqlrelay port [default:90000]
For example:

```Shell
SQRPGSQL_PORT=90001
```
------
#####SQRPGSQL_USER
require ````SQRPGSQL_USE_LOCAL=n````,
remote sqlrelay user [default:pgsqltest]
For example:

```Shell
SQRPGSQL_USER=pgsqltest
```
#####SQRPGSQL_PASS
require ````SQRPGSQL_USE_LOCAL=n````,
remote sqlrelay password [default:pgsqltest]
For example:

```Shell
SQRPGSQL_PASS=pgsqltest
```
------
#####SQRPGSQL_DB
postgresql database name or schema for testing [default:test]
For example:

```Shell
SQRPGSQL_DB=test
```