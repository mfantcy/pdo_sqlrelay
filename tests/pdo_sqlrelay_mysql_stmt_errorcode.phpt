--TEST--
PDO SQLRELAY MySQL PDOStatement->errorCode();
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc";?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

$db->exec('DROP TABLE IF EXISTS ihopeitdoesnotexist');

try {
	$stmt = $db->prepare('SELECT id FROM ihopeitdoesnotexist ORDER BY id ASC');
	$stmt->execute();
	var_dump($stmt->errorCode());


} catch (PDOException $e) {
	printf("[001] %s [%s] %s\n",
		$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--EXPECTF--
Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.ihopeitdoesnotexist' doesn't exist in %s on line %d
string(5) "42S02"
done!