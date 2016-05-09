--TEST--
SQLRELAY MySQL PDO->prepare(), native PS, mixed, wired style
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);


	$stmt = $db->query('DELETE FROM test');
	$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (1, ?), (2, ?)');
	$stmt->execute(array('a', 'b'));
	$stmt = $db->prepare("SELECT id, label FROM test WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)");
	$stmt->execute(array(1, 1));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number: mixed named and positional parameters in %s on line %d

Warning: PDO::prepare(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d

Fatal error: Uncaught Error: Call to a member function execute() on boolean in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
