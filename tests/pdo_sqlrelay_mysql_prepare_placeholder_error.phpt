--TEST--
SQLRELAY MySQL PDO->prepare(),  PS, placeholder error
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory(true);
$db->exec('DROP TABLE IF EXISTS test');
$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));
$db->exec("INSERT INTO test(id, label) VALUES (1, 'row1')");

try {
	$stmt = $db->prepare('SELECT ? FROM test WHERE ? > ?');
	$stmt->execute(array('test'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
} catch (PDOException $e) {
	printf("[001] %s\n",
		$e->getMessage());
}

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
[001] SQLSTATE[HY093]: Invalid parameter number: %s
done!
