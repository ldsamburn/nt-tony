@echo off

rem      Copyright(c)2002 Northrop Grumman Corporation
rem
rem                All Rights Reserved
rem
rem  This material may be reproduced by or for the U.S. Government pursuant to 
rem  the copyright license under the clause at Defense Federal Acquisition 
rem  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).

setlocal

rem Write any given command line parameters to the registry so the service can read them

set TFILE=%TEMP%\startparams.reg

> %TFILE% echo REGEDIT4
>>%TFILE% echo
>>%TFILE% echo [HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\NTCSS NDS Server\Parameters]
>>%TFILE% echo "AppParameters"="%*"

START /WAIT REGEDIT /S %TFILE%

DEL %TFILE%


rem Start the NTCSS NDS service
net start "NTCSS NDS Server"

endlocal