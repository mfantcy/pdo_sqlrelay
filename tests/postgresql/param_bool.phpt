--TEST--
PDO SQLRELAY POSTGRESQL PARAM_BOOL
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);

$errors = array();

$value = true;
$query = $db->prepare('SELECT :foo IS FALSE as val_is_false');
$query->bindValue(':foo', $value, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();
var_dump($value);

$query->bindValue(':foo', 0, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();

// Verify bindParam maintains reference and only passes when execute is called
$value = true;
$query->bindParam(':foo', $value, PDO::PARAM_BOOL);
$value = false;
$query->execute();
$errors[] = $query->errorInfo();
var_dump($value);

// Try with strings - Bug #68351
$value = 0;
$query->bindParam(':foo', $value, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();
var_dump($query->fetchColumn());

$value = "abc";
$query->bindParam(':foo', $value, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();
var_dump($query->fetchColumn());

$expect = 'No errors found';

foreach ($errors as $error)
{
  if (strpos('Invalid text representation', $error[2]) !== false)
  {
    $expect = 'Invalid boolean found';
  }
}
echo $expect;
?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
bool(false)
No errors found
