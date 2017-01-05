@echo off

rem      Copyright(c)2002 Northrop Grumman Corporation
rem
rem                All Rights Reserved
rem
rem  This material may be reproduced by or for the U.S. Government pursuant to 
rem  the copyright license under the clause at Defense Federal Acquisition 
rem  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).

setlocal

set NTCSS_ROOT_DIR=%0\..\..\..

rem Source the variables file
call %NTCSS_ROOT_DIR%\bin\vars.bat %NTCSS_ROOT_DIR%

rem Uncomment the following line for debug statements to be sent to the log file
rem set DEBUG=-DDEBUG

%NTCSS_JVM_DIR%\bin\java -Xrs -Djetty.home=%WEB_DIR% %DEBUG% -DLOG_FILE=%WEB_DIR%\logs\debug_yyyy_mm_dd.log -cp %CLASS_DIR%;%NTCSSS_JAR%;%WEB_JAR% org.mortbay.jetty.Server %WEB_DIR%\etc\admin.xml %WEB_DIR%\etc\config.xml

endlocal
