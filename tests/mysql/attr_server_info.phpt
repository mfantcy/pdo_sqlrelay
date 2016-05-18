--TEST--
PDO SQLRELAY PDO::ATTR_SERVER_INFO
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

assert(('' == $db->errorCode()) || ('00000' == $db->errorCode()));

$info = $db->getAttribute(PDO::ATTR_SERVER_INFO);
if ('' == $info)
	printf("[001] Server info must not be empty\n");

if (false !== $db->setAttribute(PDO::ATTR_SERVER_INFO, 'new uptime: 0s'))
	printf("[002] Wonderful, I can change the client version!\n");

$new_info = $db->getAttribute(PDO::ATTR_SERVER_INFO);
if (soundex($new_info) != soundex($info))
	printf("[003] Did we change it from '%s' to '%s'?\n", $info, $info);

if (!$stmt = $db->query("SHOW STATUS LIKE '%uptime%'"))
	printf("[004] Cannot run SHOW STATUS, [%s]\n", $db->errorCode());
else {
	if (!$row = $stmt->fetch(PDO::FETCH_NUM))
		printf("[005] Unable to fetch uptime, [%s]\n", $db->errorCode());
	else
		$uptime = $row[1];
	$stmt->closeCursor();
}

print "done!";
--EXPECTF--
done!