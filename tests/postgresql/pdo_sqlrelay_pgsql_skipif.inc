<?php
if (!extension_loaded('pdo_sqlrelay')) die ('skip pdo_sqlrelay extension not available in this build');
include "PDOSqlrelayPostgresqlTestConfig.inc";
if (PDOSqlrelayPostgresqlTestConfig::isUseLcalSqlrelayDemon()) {
	if (!PDOSqlrelayPostgresqlTestConfig::isSqlrelayRunning() && ($msg = PDOSqlrelayPostgresqlTestConfig::startSqlrelayDaemon()) != "running")
		die ("skip $msg");
}
$dsn = PDOSqlrelayPostgresqlTestConfig::getPDOSqlrelayDSN();
$username = PDOSqlrelayPostgresqlTestConfig::getSqlrelayUser();
$passwd = PDOSqlrelayPostgresqlTestConfig::getSqlrelayPassword();
$options = array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION);
try {
	$pdo = new PDO($dsn, $username, $passwd, $options );
} catch (Exception $e) {
	die ("skip ".$e->getMessage());
}
$dbType = $pdo->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE);
if ($dbType != "postgresql")
	die ("skip DB type is not postgresql , '".$dbType."' got");