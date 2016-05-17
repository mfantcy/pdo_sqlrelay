--TEST--
PDO SQLRELAY POSTGRESQL PDO::exec() returns 0 when the statement is a SELECT.
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$res = $db->exec('SELECT * from generate_series(1, 42);');
var_dump($res);
echo "Done\n";
?>
--EXPECTF--
int(0)
Done
