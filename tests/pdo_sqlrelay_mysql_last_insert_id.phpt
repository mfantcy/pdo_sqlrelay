--TEST--
PDO SQLRELAY MySQL auto_increment / last insert id
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
$db = new PDO($dsn, $username, $passwd, $options );

$db->query("DROP TABLE IF EXISTS test");

print_r($db->query("CREATE TABLE test (id int auto_increment primary key, num int)"));

print_r($db->query("INSERT INTO test (id, num) VALUES (33, 42)"));

print_r($db->query("INSERT INTO test (num) VALUES (451)"));

print_r($db->lastInsertId());

$db->query("DROP TABLE IF EXISTS test");

--EXPECT--
PDOStatement Object
(
    [queryString] => CREATE TABLE test (id int auto_increment primary key, num int)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test (id, num) VALUES (33, 42)
)
PDOStatement Object
(
    [queryString] => INSERT INTO test (num) VALUES (451)
)
34
