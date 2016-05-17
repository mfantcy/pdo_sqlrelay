--TEST--
PDO SQLRELAY POSTGRESQL Numeric data type
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$db->query("
CREATE TABLE test
(
   id   integer NOT NULL PRIMARY KEY
 , i    integer
 , bi   bigint
 , dc   decimal
 , r    real
 , d    double precision
);");

$stmt = $db->prepare('INSERT INTO test (id, i, bi, dc, r, d) VALUES (:id, :i, :bi, :dc, :r, :d);');
$stmt->bindValue(":id", 1);
$stmt->bindValue(":i", 2147483647);
$stmt->bindValue(":bi", 9223372036854775807);
$stmt->bindValue(":dc", -9223372036854775808);
$stmt->bindValue(":r", 123456.123456);
$stmt->bindValue(":d", 654321.654321);
$stmt->execute();

$stmt = $db->prepare('SELECT * FROM test ');
$stmt->execute();
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
$stmt = $db->prepare('SELECT * FROM test ');
$stmt->execute();
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

echo "done!";
?>
--CLEAN--
<?php 
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->query('DROP TABLE test');
?>
--EXPECTF--
array(6) {
  ["id"]=>
  string(1) "1"
  ["i"]=>
  string(10) "2147483647"
  ["bi"]=>
  string(19) "9223372036854775807"
  ["dc"]=>
  string(20) "-9223372036854800000"
  ["r"]=>
  string(6) "123456"
  ["d"]=>
  string(13) "654321.654321"
}
array(6) {
  ["id"]=>
  int(1)
  ["i"]=>
  int(2147483647)
  ["bi"]=>
  int(9223372036854775807)
  ["dc"]=>
  string(20) "-9223372036854800000"
  ["r"]=>
  float(123456)
  ["d"]=>
  float(654321.654321)
}
done!