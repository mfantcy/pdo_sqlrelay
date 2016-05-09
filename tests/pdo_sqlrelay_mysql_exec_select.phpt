--TEST--
PDO SQLRELAY MySQL PDO->exec(), SELECT
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
function exec_and_count($offset, &$db, $sql, $exp) {

	try {

		$ret = $db->exec($sql);
		if ($ret !== $exp) {
			printf("[%03d] Expecting '%s'/%s got '%s'/%s when running '%s', [%s] %s\n",
				$offset, $exp, gettype($exp), $ret, gettype($ret), $sql,
				$db->errorCode(), implode(' ', $db->errorInfo()));
			return false;
		}

	} catch (PDOException $e) {
		printf("[%03d] '%s' has failed, [%s] %s\n",
			$offset, $sql, $db->errorCode(), implode(' ', $db->errorInfo()));
		return false;
	}

	return true;
}

include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);
try {

	exec_and_count(2, $db, 'DROP TABLE IF EXISTS test', 0);
	exec_and_count(3, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()), 0);
	exec_and_count(4, $db, "INSERT INTO test(id, col1) VALUES (1, 'a')", 1);
	exec_and_count(5, $db, "INSERT INTO test(id, col1) VALUES (2, 'a')", 1);
	exec_and_count(6, $db, "INSERT INTO test(id, col1) VALUES (3, 'a')", 1);
	
	exec_and_count(7, $db, 'SELECT id FROM test', 3);
	exec_and_count(8, $db, "INSERT INTO test(id, col1) VALUES (2, 'b')", 1);

} catch (PDOException $e) {
	printf("[001] %s, [%s] %s\n",
		$e->getMessage(),
		$db->errorCode(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
Warning: %s SQLSTATE[23000]: %sDuplicate entry%s
[008] Expecting %s, [23000] 23000 1062 Duplicate entry %s for key 'PRIMARY'
done!