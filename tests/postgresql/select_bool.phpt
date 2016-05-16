--TEST--
PDO PgSQL Bug #33876 (PDO misquotes/miscasts bool(false))
--SKIPIF--
<?php include "pdo_sqlrelay_pgsql_skipif.inc"?>
--FILE--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec("SET LC_MESSAGES='C'");
$db->exec('CREATE TABLE test (foo varchar(5) NOT NULL, bar bool NOT NULL)');
$db->exec("INSERT INTO test VALUES('false',false)");
$db->exec("INSERT INTO test VALUES('true', true)");

$res = $db->prepare('SELECT * from test where bar = ?');

# this is the portable approach to binding a bool
$res->bindValue(1, false, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

# this is the portable approach to binding a bool
$res->bindValue(1, true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));


# true gets cast to string (because the implied default is string)
# true-as-string is 1, so this "works"
if (!$res->execute(array(true)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

# Expected to fail; unless told otherwise, PDO assumes string inputs
# false -> "" as string, which pgsql doesn't like
if (!$res->execute(array(false)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

# And now using emulator prepares
echo "EMUL\n";


$res = $db->prepare('SELECT * from test where bar = ?', array(
	PDO::ATTR_EMULATE_PREPARES => true));

# this is the portable approach to binding a bool
$res->bindValue(1, false, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

# this is the portable approach to binding a bool
$res->bindValue(1, true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));


# true gets cast to string (because the implied default is string)
# true-as-string is 1, so this "works"
if (!$res->execute(array(true)))
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

# Expected to fail; unless told otherwise, PDO assumes string inputs
# false -> "" as string, which pgsql doesn't like
if (!$res->execute(array(false))) {
	$err = $res->errorInfo();
	// Strip additional lines outputted by recent PgSQL versions
	$err[2] = trim(current(explode("\n", $err[2])));
	var_dump($err);
} else {
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));
}

$res = $db->prepare('SELECT * from test where bar = :bar');

$res->bindValue(":bar", true, PDO::PARAM_BOOL);
if (!$res->execute())
	var_dump($res->errorInfo());
else
	var_dump($res->fetchAll(PDO::FETCH_ASSOC));

$db->exec('DROP TABLE test');
?>
--CLEAN--
<?php
include "PDOSqlrelayPostgresqlTestConfig.inc";
$db = PDOSqlrelayPostgresqlTestConfig::PDOFactory();
$db->exec('DROP TABLE test');
?>
--EXPECT--
Array
(
    [0] => Array
        (
            [foo] => false
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )
ß
)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => 22P02
    [1] => 7
    [2] => ERROR:  invalid input syntax for type boolean: ""
)
EMUL
Array
(
    [0] => Array
        (
            [foo] => false
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => 22P02
    [1] => 7
    [2] => ERROR:  invalid input syntax for type boolean: ""
)
