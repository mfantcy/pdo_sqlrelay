--TEST--
PDO SQLRELAY PDO::setAttribute()/getAttribute()
--SKIPIF--
<?php include "pdo_sqlrelay_mysql_skipif.inc"; ?>
--FILE--
<?php
include "PDOSqlrelayMysqlTestConfig.inc";
$db = PDOSqlrelayMysqlTestConfig::PDOFactory();

var_dump($debug = $db->getAttribute(PDO::SQLRELAY_ATTR_DEBUG));
var_dump($dbType = $db->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE));
var_dump($dbVersion = $db->getAttribute(PDO::SQLRELAY_ATTR_DB_VERSION));
var_dump($dbHost = $db->getAttribute(PDO::SQLRELAY_ATTR_DB_HOST_NAME));
var_dump($dbIpAddress = $db->getAttribute(PDO::SQLRELAY_ATTR_DB_IP_ADDRESS));
var_dump($currentDb = $db->getAttribute(PDO::SQLRELAY_ATTR_CURRENT_DB));
var_dump($connectTimeout = $db->getAttribute(PDO::SQLRELAY_ATTR_CONNECT_TIMEOUT));
var_dump($responseTimeout = $db->getAttribute(PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT));
var_dump($useNativeType = $db->getAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE));

echo "setAttribute\n";

echo "SQLRELAY_ATTR_DEBUG\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DEBUG, !$debug));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_DEBUG) == !$debug);
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DEBUG, $debug));

echo "SQLRELAY_ATTR_DB_TYPE\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DB_TYPE, "mytest"));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_DB_TYPE) == $dbType);

echo "SQLRELAY_ATTR_DB_VERSION\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DB_VERSION, "123456"));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_DB_VERSION) == $dbVersion);

echo "SQLRELAY_ATTR_DB_HOST_NAME\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DB_HOST_NAME, "testhost"));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_DB_HOST_NAME) == $dbHost);

echo "SQLRELAY_ATTR_DB_IP_ADDRESS\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_DB_IP_ADDRESS, "0.0.0.0"));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_DB_IP_ADDRESS) == $dbIpAddress);

echo "SQLRELAY_ATTR_CURRENT_DB\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_CURRENT_DB, "testdb"));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_CURRENT_DB) == $currentDb);

echo "SQLRELAY_ATTR_CONNECT_TIMEOUT\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_CONNECT_TIMEOUT, 20));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_CONNECT_TIMEOUT) == $connectTimeout);

echo "SQLRELAY_ATTR_RESPONSE_TOMEOUT\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT, $connectTimeout + 1));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_RESPONSE_TOMEOUT) == $connectTimeout + 1);

echo "SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE\n";
var_dump($db->setAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE, !$useNativeType));
assert($db->getAttribute(PDO::SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE) == !$useNativeType);

print "done!";
?>
--EXPECTF--
bool(%s)
string(5) "%s"
string(6) "%s"
string(9) "%s"
string(9) "%d.%d.%d.%d"
string(4) "%s"
int(%d)
int(%d)
bool(%s)
setAttribute
SQLRELAY_ATTR_DEBUG
bool(true)
bool(true)
SQLRELAY_ATTR_DB_TYPE
bool(false)
SQLRELAY_ATTR_DB_VERSION
bool(false)
SQLRELAY_ATTR_DB_HOST_NAME
bool(false)
SQLRELAY_ATTR_DB_IP_ADDRESS
bool(false)
SQLRELAY_ATTR_CURRENT_DB
bool(false)
SQLRELAY_ATTR_CONNECT_TIMEOUT
bool(false)
SQLRELAY_ATTR_RESPONSE_TOMEOUT
bool(true)
SQLRELAY_ATTR_RESULT_USE_NATIVE_TYPE
bool(true)
done!