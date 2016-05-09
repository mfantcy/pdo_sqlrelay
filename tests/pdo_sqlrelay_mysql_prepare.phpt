--TEST--
SQLRELAY MySQL PDO->prepare(),  PS
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
try {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));

	$stmt = $db->prepare("INSERT INTO test(id, label) VALUES(1, '?')");

	$stmt->execute(array('first row'));
	if ('00000' !== $stmt->errorCode())
		printf("[001] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));

	$stmt = $db->prepare('SELECT id, label FROM test');
	$stmt->execute();
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

} catch (PDOException $e) {
	printf("[001] %s [%s] %s\n",
		$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
[001] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
done!
