dnl $Id$
dnl config.m4 for extension pdo_sqlrelay

if test -z "$AWK"; then
  AC_PROG_AWK()
fi
REQUIRE_VERSION=0.48
PHP_ARG_WITH(pdo-sqlrelay, for PDO SQL Relay support,
[  --with-pdo-sqlrelay[=DIR]  PDO: SQL Relay support. DIR is the SQL Relay prefix
                                   default [/usr/local]])


PHP_ARG_WITH(rudiments, for location of rudiments,
[  --with-rudiments[=DIR]     PDO SQL Relay: Set the path to rudiments install prefix 
                                   default [/usr/local]])

if test "$PHP_PDO_SQLRELAY" != "no"; then  
  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
      AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi
  PHP_REQUIRE_CXX()

  # --with-pdo-sqlrelay -> check with-path
  SEARCH_PATH="/usr/local/firstworks /usr/local /usr"
  SEARCH_FOR="/include/sqlrelay/sqlrclientwrapper.h" 
  if test -r $PHP_PDO_SQLRELAY/$SEARCH_FOR; then # path given as parameter
    PDO_SQLRELAY_DIR=$PHP_PDO_SQLRELAY
  else # search default path list
    AC_MSG_CHECKING([for sqlrelay files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        PDO_SQLRELAY_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$PDO_SQLRELAY_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Unable to find your SQL Relay installation])
  fi

  # --with-pdo-sqlrelay -> add include path
  PHP_ADD_INCLUDE($PDO_SQLRELAY_DIR/include)



  # --with-pdo-sqlrelay -> check for lib and symbol presence
  LIBNAME=sqlrclientwrapper
  LIBSYMBOL=sqlrcon_alloc

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDO_SQLRELAY_DIR/lib, PDO_SQLRELAY_SHARED_LIBADD)
	PHP_ADD_LIBRARY([sqlrclient],, PDO_SQLRELAY_SHARED_LIBADD)
    AC_DEFINE(HAVE_SQLRELAYLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong sqlrelay lib or lib not found])
  ],[
    -L$PDO_SQLRELAY_DIR/lib -lm 
  ])

  AC_MSG_CHECKING([for sqlrelay version])
  if test -x $PDO_SQLRELAY_DIR/bin/sqlrclientwrapper-config; then
    SQLRELAY_VERSION=`$PDO_SQLRELAY_DIR/bin/sqlrclientwrapper-config --version`
	res=$($AWK 'BEGIN{if("'$SQLRELAY_VERSION'">="'$REQUIRE_VERSION'") print 1; else print 0}')
	if test $res -eq 1;then
	  AC_MSG_RESULT($SQLRELAY_VERSION)
	  AC_DEFINE_UNQUOTED([SQLRELAY_VERSION], ["$SQLRELAY_VERSION"], [SQL Relay Version])
	else
	  AC_MSG_ERROR([$SQLRELAY_VERSION ,SQL Relay version $REQUIRE_VERSION or greater required.])
	fi
  else
    AC_MSG_RESULT([unknow])
	AC_MSG_ERROR([Unable to find your SQL Relay version ,version must >= $REQUIRE_VERSION])
  fi

# --with-rudiments -> check with-path
  SEARCH_PATH="/usr/local/firstworks /usr/local /usr"
  SEARCH_FOR="/include/rudiments/private/inttypes.h"
  if test -r $PHP_RUDIMENTS/$SEARCH_FOR; then # path given as parameter
     RUDIMENTS_DIR=$PHP_RUDIMENTS
  else # search default path list
     AC_MSG_CHECKING([for rudiments files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         RUDIMENTS_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
  fi

  if test -z "$RUDIMENTS_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Unable to find your rudiments installation])
  else
     PHP_ADD_INCLUDE($RUDIMENTS_DIR/include)
     PHP_ADD_LIBRARY_WITH_PATH([rudiments], $RUDIMENTS_DIR/lib, PDO_SQLRELAY_SHARED_LIBADD)
 	 AC_DEFINE(HAVE_RUDIMENTSLIB,1,[ ])
  fi
	
  PHP_ADD_LIBRARY([stdc++],, PDO_SQLRELAY_SHARED_LIBADD)
  PHP_ADD_LIBRARY([sqlrclient],, PDO_SQLRELAY_SHARED_LIBADD)
  
  PHP_SUBST(PDO_SQLRELAY_SHARED_LIBADD)
  PHP_NEW_EXTENSION(pdo_sqlrelay, pdo_sqlrelay.cpp sqlrelay_dbh.cpp sqlrelay_stmt.cpp, $ext_shared,,-I$pdo_inc_path -I)
fi
