--TEST--
PDO SQLRELAY POSTGRESQL select bool
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec("SET LC_MESSAGES='C'");
$db->exec('CREATE TABLE test (foo varchar(5) NOT NULL, bar bool NOT NULL)');
$db->exec("INSERT INTO test VALUES('false',false)");
$db->exec("INSERT INTO test VALUES('true', true)");

$res = $db->prepare('SELECT * from test where bar = ?');
echo "POSITIONAL\n";
echo "bindValue(1, false, PDO::PARAM_BOOL)\n";
$res->bindValue(1, false, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

echo "bindValue(1, true, PDO::PARAM_BOOL)\n";
$res->bindValue(1, true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));


echo "execute(array(true))\n";
if (!$res->execute(array(true)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

echo "execute(array(false))\n";
if (!$res->execute(array(false)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));


$res = $db->prepare('SELECT * from test where bar = :bar');
echo "NAMED\n";
echo "bindValue(':bar', false, PDO::PARAM_BOOL)\n";
$res->bindValue(':bar', false, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
var_dump($res->fetchAll(PDO::FETCH_ASSOC));
	
echo "bindValue(1, true, PDO::PARAM_BOOL)\n";
$res->bindValue(':bar', true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));
	
	
echo "execute(array(true))\n";
if (!$res->execute(array(':bar'=>true)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));
	
echo "execute(array(false))\n";
if (!$res->execute(array(':bar'=>false)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));
	
	
	
	
echo "setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true)\n";
$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
$res = $db->prepare('SELECT * from test where bar = :bar');

$res->bindValue(":bar", true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

$res->bindValue(":bar", false, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));
	
$db->exec('DROP TABLE test');
echo "done!"
?>
--CLEAN--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->exec('DROP TABLE test');
?>
--EXPECTF--
POSITIONAL
bindValue(1, false, PDO::PARAM_BOOL)
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(5) "false"
    ["bar"]=>
    string(1) "f"
  }
}
bindValue(1, true, PDO::PARAM_BOOL)
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(4) "true"
    ["bar"]=>
    string(1) "t"
  }
}
execute(array(true))
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(4) "true"
    ["bar"]=>
    string(1) "t"
  }
}
execute(array(false))
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(1)
  [2]=>
  string(50) "ERROR:  invalid input syntax for type boolean: ""
"
}
NAMED
bindValue(':bar', false, PDO::PARAM_BOOL)
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(5) "false"
    ["bar"]=>
    string(1) "f"
  }
}
bindValue(1, true, PDO::PARAM_BOOL)
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(4) "true"
    ["bar"]=>
    string(1) "t"
  }
}
execute(array(true))
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(4) "true"
    ["bar"]=>
    string(1) "t"
  }
}
execute(array(false))
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(1)
  [2]=>
  string(50) "ERROR:  invalid input syntax for type boolean: ""
"
}
setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true)
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(4) "true"
    ["bar"]=>
    bool(true)
  }
}
array(1) {
  [0]=>
  array(2) {
    ["foo"]=>
    string(5) "false"
    ["bar"]=>
    bool(false)
  }
}
done!