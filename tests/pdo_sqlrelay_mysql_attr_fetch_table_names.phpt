--TEST--
PDO SQLRELAY PDO::ATTR_FETCH_TABLE_NAMES
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

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
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
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
