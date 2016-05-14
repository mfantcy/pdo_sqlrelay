--TEST--
PDO SQLRELAY MySQL PDO->prepare(),  named placeholder 2
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

	try {

		$db->exec('DROP TABLE IF EXISTS test');
		$db->exec(sprintf('CREATE TABLE test(id INT, label1 CHAR(255), label2 CHAR(255)) ENGINE=%s', PDOSqlrelayMysqlTestConfig::getStorageEngin()));

		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES(:id, :label1, :label2)');
		// No replacement shall be made
		$stmt->execute(array(':label2' => 'row2', ':id' => 1, ':label1' => 'row1'));
		if ('00000' !== $stmt->errorCode())
			printf("[003] Execute has failed, %s %s\n",
					var_export($stmt->errorCode(), true),
					var_export($stmt->errorInfo(), true));
		
		$label1 = "row3";
		$label2 = "row4";
		$id = 2;
		$stmt->bindParam(':label2', $label2);
		$stmt->bindParam(':id', $id);
		$stmt->bindParam(':label1', $label1);
		$stmt->execute();
		
		$stmt->bindValue(':label2', 33);
		$stmt->bindValue(':id', 3);
		$stmt->bindParam(':label1', $label1);
		$stmt->execute();
			// Now, what do we have in the DB?
		$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE 1');
		$stmt->bindColumn(1, $id);
		$stmt->bindColumn(3, $lable2);
		//$stmt->bindParam(":placeholder", $id);
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
		var_dump($id, $label2);	
		$db->exec('DELETE FROM test');
		
		
		$stmt = $db->prepare('INSERT INTO test(id, label1, label2) VALUES(2, :placeholder, :placeholder)');
		// No replacement shall be made
		$stmt->execute(array(':placeholder' => 'row2'));
		if ('00000' !== $stmt->errorCode())
			printf("[005] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));

		// Now, what do we have in the DB?
		$stmt = $db->prepare('SELECT id, label1, label2 FROM test WHERE id = 2');
		$stmt->execute();
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		//
		// Another variation of the theme
		//

		$db->exec('DELETE FROM test');
		$db->exec("INSERT INTO test (id, label1, label2) VALUES (1, 'row1', 'row2')");
		$sql = "SELECT id, label1 FROM test WHERE id = :placeholder AND label1 = (SELECT label1 AS 'SELECT' FROM test WHERE id = :placeholder)";

		// emulated...
		$stmt = $db->prepare($sql);
		$stmt->execute(array(':placeholder' => 1));
		if ('00000' !== $stmt->errorCode())
			printf("[006] Execute has failed, %s %s\n",
				var_export($stmt->errorCode(), true),
				var_export($stmt->errorInfo(), true));
		var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

		$stmt = $db->prepare($sql);
		$stmt->execute(array(':placeholder' => 1));
		if ('00000' !== $stmt->errorCode())
			printf("[008] Execute has failed, %s %s\n",
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
array(3) {
  [0]=>
  array(3) {
    ["id"]=>
    string(1) "1"
    ["label1"]=>
    string(4) "row1"
    ["label2"]=>
    string(4) "row2"
  }
  [1]=>
  array(3) {
    ["id"]=>
    string(1) "2"
    ["label1"]=>
    string(4) "row3"
    ["label2"]=>
    string(4) "row4"
  }
  [2]=>
  array(3) {
    ["id"]=>
    string(1) "3"
    ["label1"]=>
    string(4) "row3"
    ["label2"]=>
    string(2) "33"
  }
}
string(1) "3"
string(4) "row4"

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number%s
[005] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => %s,
  2 => %s,
)
array(0) {
}

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number%s
[006] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => %s,
  2 => %s,
)
array(0) {
}

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number%s
[008] Execute has failed, 'HY093' array (
  0 => 'HY093',
  1 => %s,
  2 => %s,
)
array(0) {
}
done!
