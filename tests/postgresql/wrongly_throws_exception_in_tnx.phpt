--TEST--
PDO SQLRELAY POSTGRESQL  PDO::beginTransaction() wrongly throws exception in tnx
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$pdo = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$pdo->beginTransaction();
try {
	$pdo->query("CREATE TABLE test (a int NOT NULL PRIMARY KEY DEFERRABLE INITIALLY DEFERRED)");
	$pdo->query("INSERT INTO test VALUES (1), (1)");

	var_dump($pdo->inTransaction());
	$pdo->commit(); // This should fail!
} catch (\Exception $e) {
	var_dump($pdo->inTransaction());
	$pdo->rollBack();
	var_dump(preg_replace("/\n/", "", $e->getMessage()));
}

?>
--CLEAN--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->exec('DROP TABLE test');
?>
--EXPECTF--
bool(true)
string(%d) "SQLSTATE[HY000]: General error:%s"
