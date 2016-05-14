--TEST--
PDO SQLRELAY MySQL PDOStatement->rowCount() @ SELECT
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

try {

	if (0 !== ($tmp = $db->query('SELECT id FROM test WHERE 1 = 0')->rowCount()))
		printf("[002] Expecting 0 got %s", var_export($tmp, true));

	if (1 !== ($tmp = $db->query('SELECT id FROM test WHERE id = 1')->rowCount()))
		printf("[003] Expecting 1 got %s", var_export($tmp, true));

} catch (PDOException $e) {
	printf("[001] %s [%s] %s\n",
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
done!