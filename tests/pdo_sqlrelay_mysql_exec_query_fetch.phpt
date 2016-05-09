--TEST--
PDO sqlreay MySQL (query() execute() and fetch())
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
$db = new PDO($dsn, $username, $passwd, $options );

$db->query("DROP TABLE IF EXISTS test");
$db->exec('CREATE TABLE test (bar INT NOT NULL)');
$db->exec('INSERT INTO test VALUES(1)');

var_dump($db->query('SELECT * from test'));
foreach ($db->query('SELECT * from test') as $row) {
	print_r($row);
}

$stmt = $db->prepare('SELECT * from test');
print_r($stmt->getColumnMeta(0));
$stmt->execute();
$tmp = $stmt->getColumnMeta(0);

// libmysql and mysqlnd will show the pdo_type entry at a different position in the hash
if (!isset($tmp['pdo_type']) || (isset($tmp['pdo_type']) && $tmp['pdo_type'] != 2))
	printf("Expecting pdo_type = 2 got %s\n", $tmp['pdo_type']);
else
	unset($tmp['pdo_type']);

print_r($tmp);
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
$db = new PDO($dsn, $username, $passwd, $options );
$db->query("DROP TABLE IF EXISTS test");
?>
--EXPECTF--
object(PDOStatement)#%d (1) {
  [%u|b%"queryString"]=>
  %unicode|string%(18) "SELECT * from test"
}
Array
(
    [bar] => 1
    [0] => 1
)
Array
(
    [native_type] => INT
    [flags] => Array
        (
            [0] => not_null
        )

    [name] => bar
    [len] => 4
    [precision] => 11
)
