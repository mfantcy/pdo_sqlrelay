--TEST--
PDO::ATTR_FETCH_TABLE_NAMES
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

$db->exec('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . PDOSqlrelayMysqlTestConfig::getStorageEngin());
$db->exec("INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')");

var_dump($db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, 1));
$stmt = $db->query('SELECT label FROM test LIMIT 1');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();

var_dump($db->setAttribute(PDO::ATTR_FETCH_TABLE_NAMES, 0));
$stmt = $db->query('SELECT label FROM test LIMIT 1');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();
$db->query("DROP TABLE IF EXISTS test");
print "done!";

?>
--EXPECTF--
bool(false)
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
  }
}
bool(false)
array(1) {
  [0]=>
  array(1) {
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
  }
}
done!
