--TEST--
PDO SQLRELAY MySQL PDO->prepare(), anonymous placeholder
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
try {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec(sprintf('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));
	$db->exec("INSERT INTO test(id, label) VALUES (1, 'row1')");

	$stmt = $db->prepare('SELECT ?, id, label FROM test WHERE ? = ? ORDER BY id ASC');
	$stmt->execute(array('id', 'label', 'label'));
	if ('00000' !== $stmt->errorCode())
		printf("[003] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));


	$stmt = $db->prepare('SELECT ?, id, label FROM test WHERE ? = ? ORDER BY id ASC');
	$stmt->execute(array('id', 'label', 'label'));
	if ('00000' !== $stmt->errorCode())
		printf("[005] Execute has failed, %s %s\n",
			var_export($stmt->errorCode(), true),
			var_export($stmt->errorInfo(), true));

	$tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
	if (isset($tmp[0]['id'])) {
		if (gettype($tmp[0]['id']) == 'string')
			settype($tmp[0]['id'], 'integer');
	}
	var_dump($tmp);

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
array(1) {
  [0]=>
  array(3) {
    ["?"]=>
    string(2) "id"
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(4) "row1"
  }
}
array(1) {
  [0]=>
  array(3) {
    ["?"]=>
    string(2) "id"
    ["id"]=>
    int(1)
    ["label"]=>
    string(4) "row1"
  }
}
done!