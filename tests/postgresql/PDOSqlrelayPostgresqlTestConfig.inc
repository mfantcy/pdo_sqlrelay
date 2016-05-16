<?php

class PDOSqlrelayPostgresqlTestConfig
{
	static private $testTableName = "test";
	static private $configArray = array(	
		'PDO_SQLRELAY_USE_LOCAL' => 'y',
		// if PDO_SQLRELAY_USE_LOCAL == "y"
		'PDO_SQLRELAY_LOCAL_BIN_PATH' => '/usr/local/firstworks/bin',
		'PDO_SQLRELAY_DB_SOCKET'   => '',
		'PDO_SQLRELAY_DB_HOST'   => 'localhost',
		'PDO_SQLRELAY_DB_PORT'   => '5432',
		'PDO_SQLRELAY_DB_USER'   => 'postgres',
		'PDO_SQLRELAY_DB_PW'     => '12345',
		// if PDO_SQLRELAY_USE_LOCAL != "n"
		'PDO_SQLRELAY_DSN'       => '',
		'PDO_SQLRELAY_SOCKET'    => '/tmp/sqlrelaypgsqltest.socket',
		'PDO_SQLRELAY_HOST'      => 'localhost',
		'PDO_SQLRELAY_PORT'      => '90001',
		'PDO_SQLRELAY_USER'      => 'pgsqltest',
		'PDO_SQLRELAY_PW'        => 'pgsqltest',
		//database name or schema
		'PDO_SQLRELAY_DB_DBNAME' => 'test',
	); 
	
	static private $sqlrelayConfig =<<<FEOF
<instances>
<instance id="sqlrelaypostgrestest" socket="/tmp/sqlrelaypgsqltest.socket" dbase="postgresql" connections="5" cursors="5" maxcursors="20" maxstringbindvaluelength="1048576" maxlobbindvaluelength="20971520">
<users>
<user user="pgsqltest" password="pgsqltest"/>
</users>
<connections>
<connection string="user=--PDO_SQLRELAY_DB_USER--;password=--PDO_SQLRELAY_DB_PW--;host=--PDO_SQLRELAY_DB_HOST--;port=--PDO_SQLRELAY_DB_PORT--;db=--PDO_SQLRELAY_DB_DBNAME--;debug=no"/>
</connections>
</instance>
</instances>
FEOF;
	static private $flagEnvToConf =  false;
	static private function envToConf()
	{
		if (self::$flagEnvToConf)
			return;
		self::$configArray['PDO_SQLRELAY_DSN'] 
			= getenv('SQRPGSQL_DSN') ? getenv('SQRPGSQL_DSN') : self::$configArray['PDO_SQLRELAY_DSN'];
		
		self::$configArray['PDO_SQLRELAY_USE_LOCAL'] 
			= getenv('SQRPGSQL_USE_LOCAL')? (strtolower(getenv('SQRPGSQL_USE_LOCAL'))=='y'?'y':'n') 
			: self::$configArray['PDO_SQLRELAY_USE_LOCAL'];

		self::$configArray['PDO_SQLRELAY_LOCAL_BIN_PATH'] 
			= getenv('SQRPGSQL_LOCAL_BIN') ? getenv('SQRPGSQL_LOCAL_BIN') : self::$configArray['PDO_SQLRELAY_LOCAL_BIN_PATH'];
		
		self::$configArray['PDO_SQLRELAY_DB_SOCKET'] 
			= getenv('PDO_SQLRELAY_DB_SOCKET') ? getenv('PDO_SQLRELAY_DB_SOCKET') : self::$configArray['PDO_SQLRELAY_DB_SOCKET'];

		self::$configArray['PDO_SQLRELAY_DB_HOST'] 
			= getenv('SQRPGSQL_MYSQL_HOST') ? getenv('SQRPGSQL_MYSQL_HOST') : self::$configArray['PDO_SQLRELAY_DB_HOST'];

		self::$configArray['PDO_SQLRELAY_DB_PORT'] 
			= getenv('SQRPGSQL_MYSQL_PORT') ? getenv('SQRPGSQL_MYSQL_PORT') : self::$configArray['PDO_SQLRELAY_DB_PORT'];
	
		self::$configArray['PDO_SQLRELAY_DB_USER'] 
			= getenv('SQRPGSQL_MYSQL_USER') ? getenv('SQRPGSQL_MYSQL_USER') : self::$configArray['PDO_SQLRELAY_DB_USER'];
		
		self::$configArray['PDO_SQLRELAY_DB_PW'] 
			= getenv('SQRPGSQL_MYSQL_PASS') ? getenv('SQRPGSQL_MYSQL_PASS') : self::$configArray['PDO_SQLRELAY_DB_PW'];
		
		
		self::$configArray['PDO_SQLRELAY_SOCKET'] 
			= getenv('SQRPGSQL_SOCKET') ? getenv('SQRPGSQL_SOCKET') : self::$configArray['PDO_SQLRELAY_SOCKET'];
		
		self::$configArray['PDO_SQLRELAY_HOST'] 
			= getenv('SQRPGSQL_HOST') ? getenv('SQRPGSQL_HOST') : self::$configArray['PDO_SQLRELAY_HOST'];
		
		self::$configArray['PDO_SQLRELAY_PORT'] 
			= getenv('SQRPGSQL_PORT') ? getenv('SQRPGSQL_PORT') : self::$configArray['PDO_SQLRELAY_PORT'];
		
		self::$configArray['PDO_SQLRELAY_USER'] 
			= getenv('SQRPGSQL_USER') ? getenv('SQRPGSQL_USER') : self::$configArray['PDO_SQLRELAY_USER'];
		
		self::$configArray['PDO_SQLRELAY_PW'] 
			= getenv('SQRPGSQL_PASS') ? getenv('SQRPGSQL_PASS') : self::$configArray['PDO_SQLRELAY_PW'];
		
		
		self::$configArray['PDO_SQLRELAY_DB_DBNAME'] 
			= getenv('SQRPGSQL_DB') ? getenv('SQRPGSQL_DB') : self::$configArray['PDO_SQLRELAY_DB_DBNAME'];
		
		
		self::$flagEnvToConf = true;
	}
	static public function getConfigPath()
	{
		return sys_get_temp_dir()."/sqlrelaypgtest.conf";
	}
	static public function getConfigContent()
	{
		self::envToConf();
		$ret = self::$sqlrelayConfig;
		$ret = preg_replace("/--PDO_SQLRELAY_DB_SOCKET--/", self::$configArray["PDO_SQLRELAY_DB_SOCKET"], $ret);
		$ret = preg_replace("/--PDO_SQLRELAY_DB_HOST--/",   self::$configArray["PDO_SQLRELAY_DB_HOST"],   $ret);
		$ret = preg_replace("/--PDO_SQLRELAY_DB_PORT--/",   self::$configArray["PDO_SQLRELAY_DB_PORT"],   $ret);
		$ret = preg_replace("/--PDO_SQLRELAY_DB_USER--/",   self::$configArray["PDO_SQLRELAY_DB_USER"],   $ret);
		$ret = preg_replace("/--PDO_SQLRELAY_DB_PW--/",     self::$configArray["PDO_SQLRELAY_DB_PW"],     $ret);
		$ret = preg_replace("/--PDO_SQLRELAY_DB_DBNAME--/", self::$configArray["PDO_SQLRELAY_DB_DBNAME"], $ret);
		return $ret;
	}
	
	static public function isSqlrelayRunning()
	{
		$confFile = self::getConfigPath();
		exec("ps -ef |grep -v grep|grep sqlrelaypostgrestest|grep sqlr-connection", $out, $outVar);
		if (count($out) == 0)
			return false;
		return true;
	}
	
	static public function startSqlrelayDaemon()
	{
		self::envToConf();
		$confFile = self::getConfigPath();
		file_put_contents($confFile, self::getConfigContent());
		
		if (!self::isSqlrelayRunning()) {
			$startCommand = "PATH=".self::$configArray["PDO_SQLRELAY_LOCAL_BIN_PATH"]." sqlr-start -id sqlrelaypostgrestest -config {$confFile} > /dev/null 2>&1";
			exec($startCommand, $out, $outVar);
			sleep(1);
			if ($outVar == 127) {
				return "Can't found sqlr-start";
			}
			if (!self::isSqlrelayRunning()) {
				self::stopSqlrelayDaemon();
				return "Starup sqlrelay fail";
			}
		}
		return "running";
	}
	
	static public function stopSqlrelayDaemon()
	{
		self::envToConf();
		$confFile = self::getConfigPath();
		$stopCommand = "PATH=".self::$configArray["PDO_SQLRELAY_LOCAL_BIN_PATH"]." sqlr-stop -id sqlrelaypostgrestest -config {$confFile} > /dev/null 2>&1 &";
		exec($stopCommand, $out, $outVar);
	}
	
	static public function isUseLcalSqlrelayDemon()
	{
		self::envToConf();
		return trim(self::$configArray['PDO_SQLRELAY_DSN']) == '' && strtolower(self::$configArray['PDO_SQLRELAY_USE_LOCAL']) == 'y';
	}
	
	static public function getPDOSqlrelayDSN()
	{
		self::envToConf();
		if (self::isUseLcalSqlrelayDemon())
		{
			return "sqlrelay:socket=/tmp/sqlrelaypgsqltest.socket;tries=1;retrytime=1;debug=0";
		} else if (trim(self::$configArray['PDO_SQLRELAY_DSN']) != ''){
			return trim(self::$configArray['PDO_SQLRELAY_DSN']);
		} else {
			if (trim(self::$configArray['PDO_SQLRELAY_SOCKET']) != '') {
				return "sqlrelay:socket=".trim(self::$configArray['PDO_SQLRELAY_SOCKET']).";tries=1;retrytime=1;debug=0";
			}
			return "sqlrelay:host=".trim(self::$configArray['PDO_SQLRELAY_HOST']).";port=".trim(self::$configArray['PDO_SQLRELAY_PORT']).";tries=1;retrytime=1;debug=0";
		}
	}
	
	static public function getSqlrelayUser()
	{
		self::envToConf();
		if (self::isUseLcalSqlrelayDemon()) {
			return 	"pgsqltest";
		}
		return self::$configArray['PDO_SQLRELAY_USER'];
	}
	
	static public function getSqlrelayPassword()
	{
		self::envToConf();
		if (self::isUseLcalSqlrelayDemon()) {
			return 	"pgsqltest";
		}
		return self::$configArray['PDO_SQLRELAY_PW'];
	}

	/**
	 * 
	 * @param bool $throwExecption
	 * @return PDO
	 */
	static public function PDOFactory($throwExecption = false)
	{
		self::envToConf();
		$options = $throwExecption ? array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION) : array(PDO::ATTR_ERRMODE=>PDO::ERRMODE_WARNING);
		return new PDO(self::getPDOSqlrelayDSN(), self::getSqlrelayUser(), self::getSqlrelayPassword(), $options);		
	}
	
	/**
	 * 
	 * @param PDO $db
	 */
	static public function createTestTable(PDO $db)
	{
		self::envToConf();

		$db->exec("DROP TABLE IF EXISTS ". self::$testTableName);
		$db->exec("CREATE TABLE ".self::$testTableName."(id INT, label CHAR(1), PRIMARY KEY(id))");
		$db->exec("INSERT INTO ".self::$testTableName."(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')");			
	}
	
	/**
	 * @param PDO $db
	 */
	static public function dropTestTable(PDO $db = null)
	{
		self::envToConf();
		if ($db == null)
			$db = self::PDOFactory();
		
		$db->exec('DROP TABLE IF EXISTS '.self::$testTableName);
	}
	
}


