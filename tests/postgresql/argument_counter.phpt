--TEST--
PDO SQLRELAY POSTGRESQL argument counter
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();

$db->query('
CREATE TABLE nodes
(
   id   integer NOT NULL PRIMARY KEY
 , root integer NOT NULL
 , lft  integer NOT NULL
 , rgt  integer NOT NULL
);');

$db->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (1, 1, 1, 6);');
$db->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (2, 1, 2, 3);');
$db->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (3, 1, 4, 5);');


$stmt = $db->prepare('
	SELECT * 
	FROM nodes 
	WHERE (:rootId > 0 OR lft > :left OR rgt > :left) 
		AND (root = :rootId OR root  = :left)
		AND (1 > :left OR 1 < :left OR 1 = :left) 
		AND (:x > 0 OR :x < 10 OR :x > 100) 
		OR :y = 1 OR :left = 1
');

$stmt->bindValue('left',   1, PDO::PARAM_INT);
$stmt->bindValue('rootId', 3, PDO::PARAM_INT);
$stmt->bindValue('x', 5, PDO::PARAM_INT);
$stmt->bindValue('y', 50, PDO::PARAM_INT);

$stmt->execute();

foreach ($stmt->fetchAll() as $row) {
	print implode(' - ', $row);
	print "\n";
}


?>
--CLEAN--
<?php 
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->query('DROP TABLE nodes');
?>
--EXPECTF--
1 - 1 - 1 - 1 - 1 - 1 - 6 - 6
2 - 2 - 1 - 1 - 2 - 2 - 3 - 3
3 - 3 - 1 - 1 - 4 - 4 - 5 - 5
