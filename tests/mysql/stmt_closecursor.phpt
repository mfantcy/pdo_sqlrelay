--TEST--
PDO SQLRELAY MySQL PDOStatement->closeCursor() 
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

function pdo_mysql_stmt_closecursor($db) {
	$stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
	$stmt2 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
 	$stmt1->closeCursor();

	unset($stmt1);
	$stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
	$row1 = $stmt1->fetch(PDO::FETCH_ASSOC);
 	$stmt1->closeCursor();

	$stmt2 = $db->prepare('UPDATE test SET label = ? WHERE id = ?');
	$stmt2->bindValue(1, "z");

	$stmt2->bindValue(2, $row1['id']);
	$stmt2->execute();
 	$stmt2->closeCursor();

	$stmt1->execute();
	$row2 = $stmt1->fetch(PDO::FETCH_ASSOC);
 	$stmt1->closeCursor();
	if (!isset($row2['label']) || ('z' !== $row2['label']))
		printf("Expecting array(id => 1, label => z) got %s\n", var_export($row2, true));
	unset($stmt1);

	$stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
	// should work
	$stmt2 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
 	$stmt1->closeCursor();

	$stmt1 = $db->query('SELECT id, label FROM test ORDER BY id ASC');
	// fetch only the first rows and let closeCursor() clean up
	$row3 = $stmt1->fetch(PDO::FETCH_ASSOC);
 	$stmt1->closeCursor();
	assert($row3 == $row2);

	$stmt2 = $db->prepare('UPDATE test SET label = ? WHERE id = ?');
	$stmt2->bindValue(1, "a");
	$stmt2->bindValue(2, $row1['id']);
	$stmt2->execute();
 	$stmt2->closeCursor();

	$stmt1->execute();
	$row4 = $stmt1->fetch(PDO::FETCH_ASSOC);
 	$stmt1->closeCursor();
	assert($row4 == $row1);

	$offset = 0;
	$stmt = $db->prepare('SELECT id, label FROM test WHERE id > ? ORDER BY id ASC LIMIT 2');
	$in = 0;
	if (!$stmt->bindParam(1, $in))
		printf("[%03d + 1] Cannot bind parameter, %s %s\n", $offset,
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	$stmt->execute();
	$id = $label = null;

	if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
		printf("[%03d + 2] Cannot bind integer column, %s %s\n", $offset,
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
		printf("[%03d + 3] Cannot bind string column, %s %s\n", $offset,
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	while ($stmt->fetch(PDO::FETCH_BOUND))
		printf("in = %d -> id = %s (%s) / label = %s (%s)\n",
			$in,
			var_export($id, true), gettype($id),
			var_export($label, true), gettype($label));

 	$stmt->closeCursor();
	$stmt->execute();

}


try {

	PDOSqlrelayMysqlTestConfig::createTestTable($db);
	pdo_mysql_stmt_closecursor($db);

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
in = 0 -> id = 1 (integer) / label = 'a' (string)
in = 0 -> id = 2 (integer) / label = 'b' (string)
done!
