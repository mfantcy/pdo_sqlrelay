--TEST--
SQLRELAY MySQL PDO->prepare(), native PS, named placeholder 2
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

	try {

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, label1 CHAR(255), label2 CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));


		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES (1, :placeholder, :placeholder)');

		$stmt->execute(array(':placeholder' => 'row1'));
		if ('00000' !== $stmt->errorCode())
			printf("[001] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE id = 1');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		$sql = "SELECT id, label1 FROM test WHERE id = :placeholder AND label1 = (SELECT label1 AS 'SELECT' FROM test WHERE id = :placeholder)";
		
		$stmt = $db->prepare($sql);
		$stmt->execute(array(':placeholder' => 1));
		if ('00000' !== $stmt->errorCode())
			printf("[002] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES (1, :placeholder, :placeholder2)');
		
		$stmt->execute(array(':placeholder' => 'row1', ':placeholder2' => 'row2'));
		if ('00000' !== $stmt->errorCode())
			printf("[003] Execute has failed, %s %s\n",
					var_export($stmt->errorCode(), true),
					var_export($stmt->errorInfo(), true));
		
			$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE id = 1');
			$stmt->execute();
			var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

			$sql = "SELECT id, label1 FROM test WHERE id = :placeholder AND label1 = (SELECT label1 AS 'SELECT' FROM test WHERE id = :placeholder2)";
			
			$stmt = $db->prepare($sql);
			$stmt->execute(array(':placeholder' => 1, ':placeholder2' => 1));
			if ('00000' !== $stmt->errorCode())
				printf("[002] Execute has failed, %s %s\n",
						var_export($stmt->errorCode(), true),
						var_export($stmt->errorInfo(), true));
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
Warning: %s: SQLSTATE[HY093]: Invalid parameter number: 2031 No data supplied for parameters in prepared statement in %s
[001] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => 2031,
  2 => 'No data supplied for parameters in prepared statement',
)
array(0) {
}

Warning: %s: SQLSTATE[HY093]: Invalid parameter number: 2031 No data supplied for parameters in prepared statement in %s
[002] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => 2031,
  2 => 'No data supplied for parameters in prepared statement',
)
array(0) {
}
array(1) {
  [0]=>
  array(3) {
    ["id"]=>
    string(1) "1"
    ["label1"]=>
    string(4) "row1"
    ["label2"]=>
    string(4) "row2"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label1"]=>
    string(4) "row1"
  }
}
done!
