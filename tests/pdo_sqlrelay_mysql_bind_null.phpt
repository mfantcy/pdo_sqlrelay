--TEST--
PDO SQLRELAY MySQL bindParam/bindValue null
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

$db->exec('create table test ( bar char(3) NULL )');
$stmt = $db->prepare('insert into test (bar) values(:bar)') or var_dump($db->errorInfo());

$bar = 'foo';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = null;
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = 'qaz';
$stmt->bindParam(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$stmt2 = $db->prepare('select * from test') or var_dump($db->errorInfo());

if($stmt2) $stmt2->execute();
if($stmt2) var_dump($stmt2->fetchAll(PDO::FETCH_ASSOC));

$db->exec('delete from test');

$bar = 'foo';
$stmt->bindValue(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = null;
$stmt->bindValue(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$bar = 'qaz';
$stmt->bindValue(':bar', $bar);
$stmt->execute() or var_dump($stmt->errorInfo());

$stmt2 = $db->prepare('select * from test') or var_dump($db->errorInfo());

if($stmt2) $stmt2->execute();
if($stmt2) var_dump($stmt2->fetchAll(PDO::FETCH_ASSOC));


print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(3) {
  [0]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "foo"
  }
  [1]=>
  array(1) {
    [%u|b%"bar"]=>
    NULL
  }
  [2]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "qaz"
  }
}
array(3) {
  [0]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "foo"
  }
  [1]=>
  array(1) {
    [%u|b%"bar"]=>
    NULL
  }
  [2]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "qaz"
  }
}
done!