--TEST--
PDO SQLRELAY MySQL PDOStatement->errorInfo();
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc";?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

	try {

		$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
		var_dump($stmt->errorInfo());
		$stmt->execute();
		var_dump($stmt->errorInfo());

		PDOSqlrelayMysqlTestConfig::createTestTable($db);
		$stmt = $db->prepare('SELECT label FROM test ORDER BY id ASC LIMIT 1');
		$db->exec('DROP TABLE test');
		var_dump($stmt->execute());
		var_dump($stmt->errorInfo());
		var_dump($db->errorInfo());

	} catch (PDOException $e) {
		printf("[001] %s [%s] %s\n",
			$e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
	}

	print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
PDOSqlrelayMysqlTestConfig::dropTestTable();
?>
--EXPECTF--
array(3) {
  [0]=>
  %unicode|string%(0) ""
  [1]=>
  NULL
  [2]=>
  NULL
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d
array(3) {
  [0]=>
  %unicode|string%(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.ihopeitdoesnotexist' doesn't exist"
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.test' doesn't exist in %s on line %d
bool(false)
array(3) {
  [0]=>
  %unicode|string%(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  %unicode|string%(%d) "Table '%s.test' doesn't exist"
}
array(3) {
  [0]=>
  %unicode|string%(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
done!
