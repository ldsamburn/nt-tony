@echo off

rem      Copyright(c)2002 Northrop Grumman Corporation
rem
rem                All Rights Reserved
rem
rem  This material may be reproduced by or for the U.S. Government pursuant to 
rem  the copyright license under the clause at Defense Federal Acquisition 
rem  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).

set NTCSS_ROOT_DIR=%1

set PROPS_FILE=%NTCSS_ROOT_DIR%\Ntcss.properties
set DB_CONFIG_FILE=%PROPS_FILE%

set WEB_DIR=%NTCSS_ROOT_DIR%\web

set NTCSSS_JAR=%NTCSS_ROOT_DIR%\libs\ntcsss.jar
set COMMON_JAR=%NTCSS_ROOT_DIR%\libs\common.jar
set CNTL_PNL_JAR=%NTCSS_ROOT_DIR%\libs\ntcss_cntrl_pnl.jar
set JDBC_FILE=%NTCSS_ROOT_DIR%\libs\mkjdbc.jar
set DB_FILE=%NTCSS_ROOT_DIR%\libs\mckoidb.jar
set TAR_JAR=%NTCSS_ROOT_DIR%\libs\tar.jar;%NTCSS_ROOT_DIR%\libs\activation.jar
set WEB_JAR=%WEB_DIR%\lib\org.mortbay.jetty.jar;%WEB_DIR%\lib\com.sun.net.ssl.jar;%WEB_DIR%\lib\javax.servlet.jar;%WEB_DIR%\lib\org.apache.jasper.jar;%WEB_DIR%\lib\javax.xml.jaxp.jar;%WEB_DIR%\lib\org.apache.crimson.jar

set NTCSS_JVM_DIR=%NTCSS_ROOT_DIR%\jre

rem Source the development environment file if it exists
if exist %NTCSS_ROOT_DIR%\..\ntcss_dev.bat call %NTCSS_ROOT_DIR%\..\ntcss_dev.bat

rem NOTE: This environment variable only makes sense in a development environment
set CLASS_DIR=%NTCSS_ROOT_DIR%\..\classes