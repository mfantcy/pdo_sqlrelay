--TEST--
PDO::ATTR_PREFETCH
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = null;//array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
$db = new PDO($dsn, $username, $passwd, $options );
var_dump($db->getAttribute(PDO::ATTR_PREFETCH));
	var_dump($db->setAttribute(PDO::ATTR_PREFETCH, true));
	print "done!";
--EXPECTF--
Warning: PDO::getAttribute(): SQLSTATE[IM001]: Driver does not support this function: driver does not support that attribute in %s on line %d
bool(false)
bool(false)
done!