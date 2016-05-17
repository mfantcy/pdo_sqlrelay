--TEST--
PDO SQLRELAY POSTGRESQL Invalid parameter number
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$stmt = $db->prepare('SELECT * FROM child');
var_dump($stmt->bindValue(':test', 1, PDO::PARAM_INT));

$stmt = $db->prepare('SELECT * FROM child where :bb');
$stmt->execute();

echo "Done\n";
?>
--EXPECTF--
Warning: PDOStatement::bindValue(): SQLSTATE[HY093]: Invalid parameter number%s
bool(false)

Warning: PDOStatement::execute(): SQLSTATE[HY000]: General error:%s does not exist
%s
Done