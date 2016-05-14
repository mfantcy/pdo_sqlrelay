--TEST--
PDO SQLRELAY MySQL TINYINT(1)
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
try {
	$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
	$db->exec('DROP TABLE IF EXISTS test');
	$db->exec(sprintf('CREATE TABLE test(id INT, ty TINYINT(1)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));
	$stmt = $db->query("SHOW COLUMNS FROM test");
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
	
	$stmt = $db->prepare("INSERT INTO test(id, ty) VALUES(1, ?)");
	
	$stmt->execute(array(1));
	$stmt->execute(array(16));
	$stmt->execute(array(127));
	$stmt->execute(array(255));
	

	$stmt = $db->prepare('SELECT id, ty FROM test');
	$stmt->execute();
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
	
	$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
	$stmt = $db->prepare('SELECT id, ty FROM test');
	$stmt->execute();
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

} catch (PDOException $e) {
	printf("[001] %s [%s] %s\n",
		$e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
}

print "done!";
?>
--CLEAN--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(2) {
  [0]=>
  array(6) {
    ["field"]=>
    string(2) "id"
    ["type"]=>
    string(7) "int(11)"
    ["null"]=>
    string(3) "YES"
    ["key"]=>
    string(0) ""
    ["default"]=>
    NULL
    ["extra"]=>
    string(0) ""
  }
  [1]=>
  array(6) {
    ["field"]=>
    string(2) "ty"
    ["type"]=>
    string(10) "tinyint(1)"
    ["null"]=>
    string(3) "YES"
    ["key"]=>
    string(0) ""
    ["default"]=>
    NULL
    ["extra"]=>
    string(0) ""
  }
}
array(4) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["ty"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["ty"]=>
    string(2) "16"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["ty"]=>
    string(3) "127"
  }
  [3]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["ty"]=>
    string(3) "127"
  }
}
array(4) {
  [0]=>
  array(2) {
    ["id"]=>
    int(1)
    ["ty"]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(1)
    ["ty"]=>
    int(16)
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(1)
    ["ty"]=>
    int(127)
  }
  [3]=>
  array(2) {
    ["id"]=>
    int(1)
    ["ty"]=>
    int(127)
  }
}
done!
