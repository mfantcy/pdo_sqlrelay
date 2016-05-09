--TEST--
SQLRELAY MySQL PDO::rollBack()
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

$db->beginTransaction();

$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
$num = $row['_num'];

$db->query("INSERT INTO test(id, label) VALUES (100, 'z')");
$num++;
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[001] INSERT has failed, test will fail\n");

$db->rollBack();
$num--;
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[002] ROLLBACK has failed\n");

$db->beginTransaction();
$db->query("INSERT INTO test(id, label) VALUES (100, 'z')");
$db->query('DROP TABLE IF EXISTS test2');
$db->query('CREATE TABLE test2(id INT)');
$num++;
$db->rollBack();
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[002] ROLLBACK should have no effect because of the implicit COMMIT
		triggered by DROP/CREATE TABLE\n");


$db->query('DROP TABLE IF EXISTS test2');
$db->query('CREATE TABLE test2(id INT) ENGINE=MyISAM');
$db->beginTransaction();
$db->query('INSERT INTO test2(id) VALUES (1)');
$db->rollBack();
$row = $db->query('SELECT COUNT(*) AS _num FROM test2')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != 1)
	printf("[003] ROLLBACK should have no effect\n");

$db->query('DROP TABLE IF EXISTS test2');

$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
$db->beginTransaction();
$db->query('DELETE FROM test');
$db->rollBack();
var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0);
$db->beginTransaction();
$db->query('DELETE FROM test');
$db->rollBack();
var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
$db->beginTransaction();
$db->query('DELETE FROM test');
$db->commit();
var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0);
$db->beginTransaction();
$db->query('DELETE FROM test');
$db->commit();
var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
$db->exec('DROP TABLE IF EXISTS test2');
?>
--EXPECTF--
int(1)
int(0)
int(1)
int(0)
done!