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

%NTCSS_JVM_DIR%\bin\java -cp %CLASS_DIR%;%NTCSSS_JAR%;%COMMON_JAR%;%JDBC_FILE% ntcsss.tools.ForceLogoff %PROPS_FILE% %*

endlocal