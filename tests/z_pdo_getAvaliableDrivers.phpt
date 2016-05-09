--TEST--
public static array PDO::getAvailableDrivers(); support sqlrelay
--FILE--
<?php
echo in_array("sqlrelay", PDO::getAvailableDrivers()) ? "yes" : "no";
?>
--EXPECT--
yes
