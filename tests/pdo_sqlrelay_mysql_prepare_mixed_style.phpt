--TEST--
PDO SQLRELAY MySQL PDO->prepare(),  mixed, wired style
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory(true);
PDOSqlrelayMysqlTestConfig::createTestTable($db);
$stmt = $db->query('DELETE FROM test');
try {
	$stmt = $db->prepare('INSERT INTO test(id, label) VALUES (1, ?), (2, ?)');
	$stmt->execute(array('a', 'b'));
} catch (PDOException $e) {
	echo $e;
}
try {
	$stmt = $db->prepare("SELECT id, label FROM test WHERE id = :placeholder AND label = (SELECT label AS 'SELECT' FROM test WHERE id = ?)");
	$stmt->execute(array(1, 1));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
} catch (PDOException $e) {
	echo $e;
}
print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
%sInvalid parameter number:%s
Stack trace:
#0 %s: PDO->prepare('SELECT id, labe...')
#1 {main}%s