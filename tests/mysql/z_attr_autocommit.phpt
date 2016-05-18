--TEST--
PDO SQLRELAY MYSQL PDO::ATTR_AUTOCOMMIT
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

if (1 !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
	printf("[001] Expecting int/1 got %s\n", var_export($tmp, true));

$row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);

if (!$row['_autocommit'])
	printf("[002] Server autocommit mode should be on, got '%s'\n", var_export($row['_autocommit']));

if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0))
	printf("[003] Cannot turn off autocommit\n");

$row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);
if ($row['_autocommit'])
	printf("[004] Server autocommit mode should be off, got '%s'\n", var_export($row['_autocommit']));

if (!$db->query('SET autocommit = 1'))
	printf("[005] Cannot turn on server autocommit mode, %s\n", var_export($db->errorInfo(), true));

if (0 !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
	printf("[006] Expecting int/0 got %s\n", var_export($tmp, true));

if (!$db->query('SET autocommit = 0'))
	printf("[007] Cannot turn off server autocommit mode, %s\n", var_export($db->errorInfo(), true));

if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1))
	printf("[008] Cannot turn on autocommit\n");

$row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);
if (!$row['_autocommit'])
	printf("[009] Server autocommit mode should be on, got '%s'\n", var_export($row['_autocommit']));

if (1 !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
	printf("[010] Expecting int/1 got %s\n", var_export($tmp, true));


$db->exec('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . PDOSqlrelayMysqlTestConfig::getStorageEngin());
$db->exec("INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')");
	
	
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
$num = $row['_num'];

$db->query("INSERT INTO test(id, label) VALUES (100, 'z')");
$num++;
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[011] Insert has failed, test will fail\n");

$db->query('ROLLBACK');
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[012] ROLLBACK should not have undone anything\n");

if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0))
	printf("[013] Cannot turn off autocommit\n");

$db->query('DELETE FROM test WHERE id = 100');
$db->query('ROLLBACK');
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[014] ROLLBACK should have undone the DELETE\n");

$db->query('DELETE FROM test WHERE id = 100');
$db->query('COMMIT');
$num--;
$row = $db->query('SELECT COUNT(*) AS _num FROM test')->fetch(PDO::FETCH_ASSOC);
if ($row['_num'] != $num)
	printf("[015] DELETE should have been committed\n");

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
done!