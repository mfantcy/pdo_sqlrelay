# PHP PDO SQLRelay
SQL Relay interface driver for PHP PDO

## Installation
### Requirements
#### SQL Relay And Rudiments
First it must install SQL Relay(>=0.50) and Rudiments(>=0.41) C API
You can get they at http://sqlrelay.sourceforge.net/
#### PHP PDO
PHP must installed PDO extension
###Installation on Linux, Unix or Unix-like Platforms
extracting pdo_sqlrelay and follow the command below
```shell
cd /path/to/pdo_sqlrelay
phpize
./configure --with-php-config=/path/to/php-config \
--with-pdo-sqlrelay=/path/to/sqlrelay/prefix \
--with-rudiments=/path/to/rudiments/prefix

make
make install
```
