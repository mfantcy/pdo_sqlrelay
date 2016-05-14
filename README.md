# PHP PDO SQLRelay
SQL Relay interface driver for PHP PDO

## Installation
### Requirements
#### SQL Relay And Rudiments
First it must install SQL Relay(>=0.50) and Rudiments(>=0.41) C API<br/>
You can download from http://sqlrelay.sourceforge.net/
#### PHP PDO
PHP PDO extension must be installed 
###Installation on Linux, Unix or Unix-like Platforms
extracting pdo_sqlrelay and follow the command below

```Shell
cd /path/to/pdo_sqlrelay
phpize
./configure --with-php-config=/path/to/php-config \
--with-pdo-sqlrelay=/path/to/sqlrelay/prefix \
--with-rudiments=/path/to/rudiments/prefix

make
make install
```
####php.ini
open php.ini add

```Shell
extension=pdo_sqlrelay.so
```
###How to use
#####Create PDO instance

```php
/* Using host and port */
$pdo = new PDO("sqlrelay:host=localhost;port=9000;tries=1;retrytime=1;debug=0","user","password");
/* Using socket file */
$pdo = new PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");
```

#####Execute an SQL statement
```php
$pdo = new PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");

/* Delete all rows from the FRUIT table */
$count = $pdo->exec("DELETE FROM fruit WHERE colour = 'red'");

/* Return number of rows that were deleted */
print("Deleted $count rows.\n");
```

#####Prepares a statement for execution
```php
$dbh = new PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");

/* Execute a prepared statement by passing an array of values */
$sql = 'SELECT name, colour, calories
    FROM fruit
    WHERE calories < :calories AND colour = :colour';
$sth = $dbh->prepare($sql);
$sth->execute(array(':calories' => 150, ':colour' => 'red'));
$red = $sth->fetchAll();
$sth->execute(array(':calories' => 175, ':colour' => 'yellow'));
$yellow = $sth->fetchAll();
```

#####Retrieve a connection attribute
```php
$dbh = new PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");
/* get database type it will return 'mysql or oracle ......' */
echo $dbh->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE);
```
#####PDO Sqlrelay constants 
```
	PDO::SQLRELAY_ATTR_DEBUG
    PDO::SQLRELAY_ATTR_DB_TYPE
    PDO::SQLRELAY_ATTR_DB_VERSION
    PDO::SQLRELAY_ATTR_DB_HOST_NAME
    PDO::SQLRELAY_ATTR_DB_IP_ADDRESS
    PDO::SQLRELAY_ATTR_BIND_FORMAT
    PDO::SQLRELAY_ATTR_CURRENT_DB
    PDO::SQLRELAY_ATTR_CONNECT_TIMEOUT
    PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT
    PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE
```

#####Set result set buffer size
```php
$dbh = new PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");
$stm = $dbh->prepare($sql);
$stmt->setAttribute(PDO::SQLRELAY_ATTR_RESULT_SET_BUFFER_SIZE,6);
......
.... and so on
```
[mkdnlink]:tests/README.md