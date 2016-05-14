--TEST--
PDO SQLRELAY MySQL PDOStatement->bindColumn()
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();
PDOSqlrelayMysqlTestConfig::createTestTable($db);

try {

	$stmt = $db->prepare('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
	$stmt->execute();
	$id = $label = null;

	if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
		printf("[003] Cannot bind integer column, %s %s\n",
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
		printf("[004] Cannot bind string column, %s %s\n",
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	$data = array();
	while ($stmt->fetch(PDO::FETCH_BOUND)) {
		printf("id = %s (%s) / label = %s (%s)\n",
			var_export($id, true), gettype($id),
			var_export($label, true), gettype($label));
		$data[] = array('id' => $id, 'label' => $label);
	}

	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2');
	$index = 0;
	while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
		if ($row['id'] != $data[$index]['id']) {
			printf("[005] Fetch bound and fetch assoc differ - column 'id', bound: %s/%s, assoc: %s/%s\n",
				var_export($data[$index]['id'], true), gettype($data[$index]['id']),
				var_export($row['id'], true), gettype($row['id']));
		}
		if ($row['label'] != $data[$index]['label']) {
			printf("[006] Fetch bound and fetch assoc differ - column 'label', bound: %s/%s, assoc: %s/%s\n",
				var_export($data[$index]['label'], true), gettype($data[$index]['label']),
				var_export($row['label'], true), gettype($row['label']));
		}
		$index++;
	}


	$stmt = $db->prepare('SELECT id, label FROM test ORDER BY id ASC LIMIT 2, 2');
	$stmt->execute();
	$id = $label = null;

	if (!$stmt->bindColumn(1, $id, PDO::PARAM_INT))
		printf("[008] Cannot bind integer column, %s %s\n",
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	if (!$stmt->bindColumn(2, $label, PDO::PARAM_STR))
		printf("[009] Cannot bind string column, %s %s\n",
			$stmt->errorCode(), var_export($stmt->errorInfo(), true));

	$data = array();
	while ($stmt->fetch(PDO::FETCH_BOUND)) {
		printf("id = %s (%s) / label = %s (%s)\n",
			var_export($id, true), gettype($id),
			var_export($label, true), gettype($label));
		$data[] = array('id' => $id, 'label' => $label);
	}

	$stmt = $db->query('SELECT id, label FROM test ORDER BY id ASC LIMIT 2, 2');
	$index = 0;
	while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
		if ($row['id'] != $data[$index]['id']) {
			printf("[010] Fetch bound and fetch assoc differ - column 'id', bound: %s/%s, assoc: %s/%s\n",
				var_export($data[$index]['id'], true), gettype($data[$index]['id']),
				var_export($row['id'], true), gettype($row['id']));
		}
		if ($row['label'] != $data[$index]['label']) {
			printf("[011] Fetch bound and fetch assoc differ - column 'label', bound: %s/%s, assoc: %s/%s\n",
				var_export($data[$index]['label'], true), gettype($data[$index]['label']),
				var_export($row['label'], true), gettype($row['label']));
		}
		$index++;
	}

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
id = 1 (integer) / label = 'a' (string)
id = 2 (integer) / label = 'b' (string)
id = 3 (integer) / label = 'c' (string)
id = 4 (integer) / label = 'd' (string)
done!
