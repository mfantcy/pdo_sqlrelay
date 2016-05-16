--TEST--
PDO SQLRELAY (bindParam() and bindValue() do not work with MySQL MATCH () AGAINST ())
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();

try {

	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec('CREATE TABLE test(id INT, label CHAR(255)) ENGINE=MyISAM');
	$db->exec('CREATE FULLTEXT INDEX idx1 ON test(label)');

	$stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)');
	$stmt->execute(array(':placeholder' => 'row'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (:placeholder)');
	$stmt->execute(array('placeholder' => 'row'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

	$stmt = $db->prepare('SELECT id, label FROM test WHERE MATCH label AGAINST (?)');
	$stmt->execute(array('row'));
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

} catch (PDOException $e) {

	printf("[001] %s, [%s} %s\n",
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
array(0) {
}
array(0) {
}
array(0) {
}
done!
