--TEST--
SQLRELAY MySQL PDO->prepare(), native PS, named placeholder
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

	$stmt = $db->prepare("SELECT :param FROM test ORDER BY id ASC LIMIT 1");
	$stmt->execute(array(':param' => 'id'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    ["id"]=>
    string(2) "id"
  }
}
done!