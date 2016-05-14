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
####make test
before````make install```` you can run ````make test```` for test this extension.
see [Test PDO SQLRELAY](./tests/README.md)
####php.ini
open php.ini add

```Shell
extension=pdo_sqlrelay.so
```
###PDO SQLRELAY DSN
````
sqlrelay:host=;port=;socket=;conntimeout=;resptimout=;retrytime=;tries=;debug=

host -- sqlrelay host
port -- sqlrelay port
conntimeout -- Sets the sqlrelay server connect timeout in seconds
resptimout  -- Sets the response timeout in seconds
retrytime  -- retry wait time in seconds
tries  -- retry tries times
debug  -- turn on/off debug mode [0|1]
````
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

#####PDO Sqlrelay constants for PDO::setAttribute()
```
PDO::SQLRELAY_ATTR_DEBUG                  -- turn on/off sqlrelay debug [default:off]
PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT       -- set response timeout sec [default: 30]
PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE -- qurey result useing [default:off]
```
```php
echo "setAttribute\n";
$db = PDO("sqlrelay:socket=/tmp/test.socket;tries=1;retrytime=1;debug=0","user","password");

echo "SQLRELAY_ATTR_DEBUG\n";
$db->setAttribute(PDO::SQLRELAY_ATTR_DEBUG, true);

echo "SQLRELAY_ATTR_RESPONSE_TOMEOUT\n";
$db->setAttribute(PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT, 20);

echo "SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE\n";
$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);

```

#####PDO Sqlrelay constants for PDO::getAttribute()
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

####PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE
query result convert to php scalar types

```php
$pdo = new PDO("sqlrelay:socket=/tmp/sqlrelaymysqltest.socket;tries=1;retrytime=1;debug=0", "mysqltest", "mysqltest");

$pdo->query('DROP TABLE IF EXISTS test');
$pdo->exec("CREATE TABLE test(id INT, i INT, ui INT UNSIGNED,
		bi BIGINT, ubi BIGINT UNSIGNED,
		d DOUBLE, ud DOUBLE UNSIGNED,
		f FLOAT, uf FLOAT UNSIGNED,
		vc VARCHAR(10), PRIMARY KEY(id)) ENGINE=InnoDB");

$stmt = $pdo->prepare("INSERT INTO test(id,i,ui,bi,ubi,d,ud,f,uf,vc) 
		VALUES(:id,:i,:ui,:bi,:ubi,:d,:ud,:f,:uf,:vc)");

$stmt->bindValue(":id", 2);
$stmt->bindValue(":i", -2147483648);
$stmt->bindValue(":ui", 4294967295);
$stmt->bindValue(":bi", -9223372036854775808);
$stmt->bindValue(":ubi", '18446744073709551615');
$stmt->bindValue(":d", -30000.03333);
$stmt->bindValue(":ud", 40000.04444);
$stmt->bindValue(":f", "-50000.05555");
$stmt->bindValue(":uf", "60000.06666");
$stmt->bindValue(":vc", "test test");
$stmt->execute();

$pdo->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, false);
$stmt = $pdo->query("SELECT * FROM test");
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
/* output  SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE off
array(10) {
  ["id"]=>
  string(1) "2"
  ["i"]=>
  string(11) "-2147483648"
  ["ui"]=>
  string(10) "4294967295"
  ["bi"]=>
  string(20) "-9223372036854775808"
  ["ubi"]=>
  string(20) "18446744073709551615"
  ["d"]=>
  string(12) "-30000.03333"
  ["ud"]=>
  string(11) "40000.04444"
  ["f"]=>
  string(8) "-50000.1"
  ["uf"]=>
  string(7) "60000.1"
  ["vc"]=>
  string(9) "test test"
}
*/

$pdo->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
$stmt = $pdo->query("SELECT * FROM test");
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
/* output SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE on
array(10) {
  ["id"]=>
  int(2)
  ["i"]=>
  int(-2147483648)
  ["ui"]=>
  int(4294967295)
  ["bi"]=>
  int(-9223372036854775808)
  ["ubi"]=>
  string(20) "18446744073709551615"
  ["d"]=>
  float(-30000.03333)
  ["ud"]=>
  float(40000.04444)
  ["f"]=>
  float(-50000.1)
  ["uf"]=>
  float(60000.1)
  ["vc"]=>
  string(9) "test test"
}
*/

```
