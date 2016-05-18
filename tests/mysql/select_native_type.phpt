--TEST--
PDO SQLRELAY MySQL PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

$db->query('DROP TABLE IF EXISTS test');
$db->exec("CREATE TABLE test(
		id INT,
		ti TINYINT, 
		uti TINYINT UNSIGNED, 
		i INT,
		ui INT UNSIGNED,
		bi BIGINT, 
		ubi BIGINT UNSIGNED,
		r REAL,
		ur REAL UNSIGNED,
		d DOUBLE, 
		ud DOUBLE UNSIGNED,
		f FLOAT,
		uf FLOAT UNSIGNED,
		vc VARCHAR(10),
		PRIMARY KEY(id)) ENGINE=InnoDB");

$stmt = $db->prepare("INSERT INTO test(id,ti,uti,i,ui,bi,ubi,r,ur,d,ud,f,uf,vc) 
		VALUES(:id,:ti,:uti,:i,:ui,:bi,:ubi,:r,:ur,:d,:ud,:f,:uf,:vc)");


$stmt->bindValue(":id", 2);
$stmt->bindValue(":ti", -127);
$stmt->bindValue(":uti", 255);
$stmt->bindValue(":i", -2147483648);
$stmt->bindValue(":ui", 4294967295);
$stmt->bindValue(":bi", -9223372036854775808);
$stmt->bindValue(":ubi", '18446744073709551615');
$stmt->bindValue(":r", -10000.01111);
$stmt->bindValue(":ur", 20000.02222);
$stmt->bindValue(":d", -30000.03333);
$stmt->bindValue(":ud", 40000.04444);
$stmt->bindValue(":f", "-50000.05555");
$stmt->bindValue(":uf", "60000.06666");
$stmt->bindValue(":vc", "test test");

$stmt->execute();

$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, false);
$stmt = $db->query("SELECT * FROM test");
var_dump($result = $stmt->fetch(PDO::FETCH_ASSOC));

$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
$stmt = $db->query("SELECT * FROM test");
var_dump($result = $stmt->fetch(PDO::FETCH_ASSOC));

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(14) {
  ["id"]=>
  string(1) "2"
  ["ti"]=>
  string(4) "-127"
  ["uti"]=>
  string(3) "255"
  ["i"]=>
  string(11) "-2147483648"
  ["ui"]=>
  string(10) "4294967295"
  ["bi"]=>
  string(20) "-9223372036854775808"
  ["ubi"]=>
  string(20) "18446744073709551615"
  ["r"]=>
  string(12) "-10000.01111"
  ["ur"]=>
  string(11) "20000.02222"
  ["d"]=>
  string(12) "-30000.03333"
  ["ud"]=>
  string(11) "40000.04444"
  ["f"]=>
  string(8) "-50000.1"
  ["uf"]=>
  string(7) "60000.1"
  ["vc"]=>
  string(9) "test test"
}
array(14) {
  ["id"]=>
  int(2)
  ["ti"]=>
  int(-127)
  ["uti"]=>
  int(255)
  ["i"]=>
  int(-2147483648)
  ["ui"]=>
  int(4294967295)
  ["bi"]=>
  int(-9223372036854775808)
  ["ubi"]=>
  string(20) "18446744073709551615"
  ["r"]=>
  float(-10000.01111)
  ["ur"]=>
  float(20000.02222)
  ["d"]=>
  float(-30000.03333)
  ["ud"]=>
  float(40000.04444)
  ["f"]=>
  float(-50000.1)
  ["uf"]=>
  float(60000.1)
  ["vc"]=>
  string(9) "test test"
}
done!