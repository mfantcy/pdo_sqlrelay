<?php
if (!extension_loaded('pdo_sqlrelay')) die ('skip pdo_sqlrelay extension not available in this build');
include "PDOSqlrelayMysqlTestConfig.inc";
if (PDOSqlrelayMysqlTestConfig::isUseLcalSqlrelayDemon()) {
	if (!PDOSqlrelayMysqlTestConfig::isSqlrelayRunning() && ($msg = PDOSqlrelayMysqlTestConfig::startSqlrelayDaemon()) != "running")
		die ("skip $msg");
}
$dsn = PDOSqlrelayMysqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayMysqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayMysqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
try {
	$pdo = new PDO($dsn, $username, $passwd, $options );
} catch (Exception $e) {
	die ("skip ".$e->getMessage());
}
$dbType = $pdo->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE);
if ($dbType != "mysql")
	die ("skip DB type is not mysql ,'".$dbType."' got");