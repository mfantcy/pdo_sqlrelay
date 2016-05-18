--TEST--
PDO SQLRELAY POSTGRESQL stringify and blob
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

try {
        @$db->query("SET bytea_output = 'escape'");
} catch (Exception $e) {
}
$db->query('CREATE TABLE test_one_blob (id SERIAL NOT NULL, blob1 BYTEA)');

$stmt = $db->prepare("INSERT INTO test_one_blob (blob1) VALUES (:foo)");

$data = 'foo';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);
$stmt->bindParam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$blob = '';
$stmt->bindParam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$data = '';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);

$stmt->bindParam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$blob = NULL;
$stmt->bindParam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$res = $db->query("SELECT blob1 from test_one_blob");
// Resource
var_dump($x = $res->fetch());
var_dump($x['blob1']);

// Resource
var_dump($x = $res->fetch());
var_dump($x['blob1']);

// Resource
var_dump($x = $res->fetch());
var_dump($x['blob1']);

// NULL
var_dump($res->fetch());

echo "done!";
?>
--CLEAN--
<?php 
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->query('DROP TABLE test_one_blob');
?>
--EXPECTF--
array(2) {
  ["blob1"]=>
  string(3) "foo"
  [0]=>
  string(3) "foo"
}
string(3) "foo"
array(2) {
  ["blob1"]=>
  string(0) ""
  [0]=>
  string(0) ""
}
string(0) ""
array(2) {
  ["blob1"]=>
  string(0) ""
  [0]=>
  string(0) ""
}
string(0) ""
array(2) {
  ["blob1"]=>
  NULL
  [0]=>
  NULL
}
done!