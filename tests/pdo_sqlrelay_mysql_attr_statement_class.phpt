--TEST--
PDO SQLRELAY PDO::ATTR_STATEMENT_CLASS
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory(true);
PDOSqlrelayMysqlTestConfig::createTestTable($db);
	
$default =  $db->getAttribute(PDO::ATTR_STATEMENT_CLASS);
var_dump($default);
try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, true);
} catch (PDOException $e) {
	printf("[001] %s\n", $e->getMessage());
}

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, 'foo');
} catch (PDOException $e) {
	printf("[002] %s\n", $e->getMessage());
}

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('classname'));
} catch (PDOException $e) {
	printf("[003] %s\n", $e->getMessage());
}

try {
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('classname', array()));
} catch (PDOException $e) {
	printf("[004] %s\n", $e->getMessage());
}
	
class myclass {
	function __construct() {
		printf("myclass\n");
	}
}

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('myclass', array()));
} catch (PDOException $e) {
	printf("[005] %s\n", $e->getMessage());
}
	
class mystatement extends PDOStatement {
	public function __construct() {
		printf("mystatement\n");
	}
}

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement', array()));
} catch (PDOException $e) {
	printf("[006] %s\n", $e->getMessage());
}
	
class mystatement2 extends PDOStatement {
	public function __destruct() {
		printf("mystatement\n");
	}
}

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement2', array()));
} catch (PDOException $e) {
	printf("[007] %s\n", $e->getMessage());
}
	
class mystatement3 extends PDOStatement {
	private function __construct($msg) {
		printf("mystatement3\n");
		var_dump($msg);
	}
}
try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement3', array('param1')));
} catch (PDOException $e) {
	printf("[008] %s\n", $e->getMessage());
}
	
class mystatement4 extends PDOStatement {
	private function __construct($msg) {
		printf("%s\n", get_class($this));
		var_dump($msg);
	}
}
try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement4', array('param1')));
} catch (PDOException $e) {
	printf("[009] %s\n", $e->getMessage());
}

try {
	$db->getAttribute(PDO::ATTR_STATEMENT_CLASS, "mystatement4");
	$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, true);
	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
	var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
} catch (PDOException $e) {
	printf("[010] %s\n", $e->getMessage());
}

class mystatement5 extends mystatement4 {
	public function fetchAll($fetch_style = 1, $column_index = 1, $ctor_args = array()) {
		return "no data :)";
	}
}
$db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, false);

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement5', array('mystatement5')));
	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
	var_dump($stmt->fetchAll());
} catch (PDOException $e) {
	printf("[011] %s\n", $e->getMessage());
}
try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('PDOStatement'));
	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');
	var_dump($stmt->fetchAll());
} catch (PDOException $e) {
	printf("[012] %s\n", $e->getMessage());
}

register_shutdown_function(function(){
	if ($error = error_get_last() ) {
		printf("[013] %s\n", $error['message']);
		echo "done!";
	}
	error_clear_last();
});
error_reporting(0);

abstract class mystatement6 extends mystatement5 {
}

if (version_compare(PHP_VERSION, "7.0.0", "<")) {
	class Error extends ErrorException {}
} 

try {
	$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('mystatement6', array('mystatement6')));
	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 1');
} catch (Error $e) {
	printf("[013] %s\n", $e->getMessage());
}
error_clear_last();
print "done!";
?>
--CLEAN--
<?php 
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOfactory();
$db->exec('DROP TABLE IF EXISTS test');
?>
--EXPECTF--
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
[001] SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS%s
[002] SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS%s
[003] SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS%s
[004] SQLSTATE[HY000]: General error: PDO::ATTR_STATEMENT_CLASS%s
[005] SQLSTATE[HY000]: General error: user-supplied statement class must be derived from PDOStatement
[006] SQLSTATE[HY000]: General error: user-supplied statement class cannot have a public constructor

Warning: PDO::getAttribute() expects exactly 1 parameter, 2 given in%s
mystatement4
string(6) "param1"
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    int(1)
    ["label"]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(2)
    ["label"]=>
    string(1) "b"
  }
}
mystatement5
string(12) "mystatement5"
string(10) "no data :)"
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
  }
}
[013] Cannot instantiate abstract class mystatement6
done!