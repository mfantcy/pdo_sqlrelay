--TEST--
PDO SQLRELAY MySQL PDOStatement->columnCount()
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc";?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);
try {
	$stmt = $db->prepare("SELECT id, label, '?' as foo FROM test");
	$stmt->execute();
	var_dump($stmt->columnCount());

	$stmt = $db->query('SELECT * FROM test');
	var_dump($stmt->columnCount());

} catch (PDOException $e) {
	printf("[001] %s [%s] %s\n",
		$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
}

try {

	$stmt = $db->prepare("SELECT id, label, '?' as foo, 'TODO - Stored Procedure' as bar FROM test");
	$stmt->execute();
	var_dump($stmt->columnCount());

	$stmt = $db->query('SELECT * FROM test');
	var_dump($stmt->columnCount());

} catch (PDOException $e) {
	printf("[003] %s [%s] %s\n",
		$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
int(3)
int(2)
int(4)
int(2)
done!