--TEST--
SQLRELAY MySQL PDO->prepare(), native PS, named placeholder
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

try {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));


	$stmt = $db->prepare("INSERT INTO test(id, label) VALUES (100, ':placeholder')");

	$stmt->execute(array(':placeholder' => 'row1'));
	if ('00000' !== $stmt->errorCode())
		printf("[003] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));

	$stmt = $db->prepare('SELECT id, label FROM test');
	$stmt->execute();
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));


	$stmt = $db->prepare("INSERT INTO test(id, label) VALUES(101, ':placeholder')");
	$stmt->execute();
	if ('00000' !== $stmt->errorCode())
		printf("[005] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));

	$stmt = $db->prepare('SELECT id, label FROM test ORDER BY id');
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
[003] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => NULL,
  2 => NULL,
)
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(3) "101"
    ["label"]=>
    string(12) ":placeholder"
  }
}
done!
