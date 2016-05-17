--TEST--
PDO SQLRELAY POSTGRESQL Incorrect binding numeric array with gaps
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$pdo = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute (\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_ASSOC);

$test = function () use ($pdo) {
	$arr = [
		0 => "a",
		2 => "b",
	];

	$stmt = $pdo->prepare("SELECT (?)::text AS a, (?)::text AS b");

	try {
		$stmt->execute($arr);
		var_dump($stmt->fetch());
	} catch (\Exception $e) {
		echo preg_replace("/\n/", "", $e->getMessage())."\n";
	}
};

$test();

?>
--EXPECTF--
SQLSTATE[%s]: %s

