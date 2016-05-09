--TEST--
PDO::ATTR_STATEMENT_CLASS
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = null;
$db = new PDO($dsn, $username, $passwd, $options );
$db->exec('DROP TABLE IF EXISTS test');
$db->exec('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . PDOSqlrelayMysqlTestConfig::getStorageEngin());
$db->exec("INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')");
	
$default =  $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
var_dump($default);
if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, true)))
	printf("[001] Expecting boolean/false got %s\n", var_export($tmp, true));

if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, 'foo')))
	printf("[002] Expecting boolean/false got %s\n", var_export($tmp, true));

if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('classname'))))
	printf("[003] Expecting boolean/false got %s\n", var_export($tmp, true));

if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('classname', array()))))
	printf("[004] Expecting boolean/false got %s\n", var_export($tmp, true));

class myclass {
	function __construct() {
		printf("myclass\n");
	}
}
if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('myclass', array()))))
	printf("[005] Expecting boolean/false got %s\n", var_export($tmp, true));

class mystatement extends PDOStatement {
	public function __construct() {
		printf("mystatement\n");
	}
}

if (false !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement', array()))))
	printf("[006] Expecting boolean/false got %s\n", var_export($tmp, true));

class mystatement2 extends PDOStatement {
	public function __destruct() {
		printf("mystatement\n");
	}
}

if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement2', array()))))
	printf("[007] Expecting boolean/true got %s\n", var_export($tmp, true));

class mystatement3 extends PDOStatement {
	private function __construct($msg) {
		printf("mystatement3\n");
		var_dump($msg);
	}
}
if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement3', array('param1')))))
	printf("[008] Expecting boolean/true got %s\n", var_export($tmp, true));

class mystatement4 extends PDOStatement {
	private function __construct($msg) {
		printf("%s\n", get_class($this));
		var_dump($msg);
	}
}
if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement4', array('param1')))))
	printf("[008] Expecting boolean/true got %s\n", var_export($tmp, true));

var_dump($db->getAttribute(PDO::ATTR_STATEMENT_CLASS));
$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');

class mystatement5 extends mystatement4 {
	public function fetchAll($fetch_style = 1, $column_index = 1, $ctor_args = array()) {
		return "no data :)";
	}
}

if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement5', array('mystatement5')))))
	printf("[009] Expecting boolean/true got %s\n", var_export($tmp, true));
$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
var_dump($stmt->fetchAll());

if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('PDOStatement'))))
	printf("[010] Expecting boolean/true got %s\n", var_export($tmp, true));

$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');
var_dump($stmt->fetchAll());

abstract class mystatement6 extends mystatement5 {
}
if (true !== ($tmp = $db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement6', array('mystatement6')))))
	printf("[011] Expecting boolean/true got %s\n", var_export($tmp, true));
$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');

print "done!";
?>
--CLEAN--
<?php 
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
$db = new PDO($dsn, $username, $passwd, $options );
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(1) {
  [0]=>
  %unicode|string%(12) "PDOStatement"
}

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS requires format array(classname, array(ctor_args)); the classname must be a string specifying an existing class in %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: user-supplied statement class must be derived from PDOStatement in %s on line %d

Warning: PDO::setAttribute(): SQLSTATE[HY000]: General error: user-supplied statement class cannot have a public constructor in %s on line %d
array(2) {
  [0]=>
  %unicode|string%(12) "mystatement4"
  [1]=>
  array(1) {
    [0]=>
    %unicode|string%(6) "param1"
  }
}
mystatement4
%unicode|string%(6) "param1"
mystatement5
%unicode|string%(12) "mystatement5"
%unicode|string%(10) "no data :)"
array(1) {
  [0]=>
  array(4) {
    [%u|b%"id"]=>
    %unicode|string%(1) "1"
    [0]=>
    %unicode|string%(1) "1"
    [%u|b%"label"]=>
    %unicode|string%(1) "a"
    [1]=>
    %unicode|string%(1) "a"
  }
}

Fatal error: Uncaught Error: Cannot instantiate abstract class mystatement6 in %s:%d
Stack trace:
#0 %s(%d): PDO->query('SELECT id, labe...')
#1 {main}
  thrown in %s on line %d