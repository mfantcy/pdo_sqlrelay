--TEST--
SQLRELAY MySQL PDO->prepare(), native PS, clear line after error
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();

try {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));


	$db->exec("INSERT INTO test(id, label) VALUES (1, 'row1')");

	$stmt = $db->prepare('SELECT unknown_column FROM test WHERE id > :placeholder ORDER BY id ASC');
	$stmt->execute(array(':placeholder' => 0));
	if ('00000' !== $stmt->errorCode())
		printf("[003] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));

	$stmt = $db->prepare('SELECT id, label FROM test WHERE id > :placeholder ORDER BY id ASC');
	$stmt->execute(array(':placeholder' => 0));
	if ('00000' !== $stmt->errorCode())
		printf("[004] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));
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
$db = PDOSqlrelayMysqlTestConfig::PDOfactory(true);
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
Warning: PDOStatement::execute(): SQLSTATE[42S22]: Column not found: 1054 Unknown column 'unknown_column' in 'field list' in %s on line %d
[003] Execute has failed, '42S22' array (
  0 => '42S22',
  1 => 1054,
  2 => 'Unknown column \'unknown_column\' in \'field list\'',
)
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(4) "row1"
  }
}
done!