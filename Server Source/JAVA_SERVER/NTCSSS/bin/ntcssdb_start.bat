@echo off

rem      Copyright(c)2002 Northrop Grumman Corporation
rem
rem                All Rights Reserved
rem
rem  This material may be reproduced by or for the U.S. Government pursuant to 
rem  the copyright license under the clause at Defense Federal Acquisition 
rem  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).

setlocal

set NTCSS_ROOT_DIR=%0\..\..

rem Source the variables file
call %NTCSS_ROOT_DIR%\bin\vars.bat %NTCSS_ROOT_DIR%

rem Check to see if the initial database files exist
if not exist %NTCSS_ROOT_DIR%\database\data goto create
if exist %NTCSS_ROOT_DIR%\database\data goto start

:create

rem Create the initial database files
%NTCSS_JVM_DIR%\bin\java -jar %DB_FILE% -conf %DB_CONFIG_FILE% -create "ntcss" "ntcss"

rem Delete the progrps.ini file to force prime to occur
if exist %NTCSS_ROOT_DIR%\database\progrps.ini del %NTCSS_ROOT_DIR%\database\progrps.ini

rem Delete the VERSION file to force prime to create the NTCSS RDBMS tables
if exist %NTCSS_ROOT_DIR%\database\VERSION del %NTCSS_ROOT_DIR%\database\VERSION


:start

rem Start the database engine
%NTCSS_JVM_DIR%\bin\java -Xrs -jar %DB_FILE% -conf %DB_CONFIG_FILE%

endlocal