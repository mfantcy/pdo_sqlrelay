--TEST--
PDO SQLRELAY PDO::ATTR_SERVER_VERSION
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

assert(('' == $db->errorCode()) || ('00000' == $db->errorCode()));

$version = $db->getAttribute(PDO::SQLRELAY_ATTR_DB_VERSION);
if ('' == $version)
	printf("[001] Server version must not be empty\n");

if (is_string($version)) {
	if (!preg_match('/(\d+)\.(\d+)\.(\d+)(.*)/', $version, $matches))
		printf("[002] Client version string seems wrong, got '%s'\n", $version);
	else {
		if ($matches[1] < 3)
			printf("[003] Strange major version: '%s'. Should be more than 3\n", $matches[1]);
		if ($matches[2] < 0)
			printf("[004] Minor version should be at least 0, got '%s'\n", $matches[2]);
		if ($matches[3] < 0)
			printf("[005] Sub version should be at least 0, got '%s'\n", $matches[2]);
	}
} else if (is_int($version)) {
	$major = floor($version / 10000);
	$minor = floor(($version - ($main * 10000)) / 100);
	$sub = $version - ($main * 10000) - ($minor * 100);
	if ($major < 3)
		printf("[006] Strange major version: '%s'. Should be more than 3\n", $major);
	if ($minor < 0)
		printf("[007] Minor version should be at least 0, got '%s'\n", $minor);
	if ($sub < 0)
		printf("[008] Sub version should be at least 0, got '%s'\n", $sub);
}

if (false !== $db->setAttribute(PDO::ATTR_CLIENT_VERSION, '1.0'))
	printf("[009] Wonderful, I can change the client version!\n");

print "done!";
?>
--EXPECTF--
done!