:: This will make a clean build of *ALL* the NTCSS client-side binaries, .DLLs, and .libs
:: A local file is expected (named msdev_path) that contains the full path name of msdev.exe

@echo off
set CLEAN=false
set build_type=Release
set clean_only=false

if [%1]==[] goto usage
if [%1]==[sub] goto %2
if [%1]==[debug] set build_type=Debug
if [%1]==[release] set build_type=Release
if [%1]==[all] set build_type= - ALL"
if [%2]==[clean] set clean=true
if [%2]==[clean_only] set clean_only=true

set ntcss_msdev=
set temp_path=

:: Check to see if the local set_msdev.bat file exists...
if not exist .\set_msdev.bat goto search_for_msdev

:: This sets the temp_path environment variable for us
call .\set_msdev.bat

if "%temp_path%"=="" goto search_for_msdev

if exist "%temp_path%" goto start_building

echo   Invalid path listed for msdev.exe!
del .\set_msdev.bat


:search_for_msdev

echo   Searching usual places for the location of msdev.exe...

::Check the usual installation places..
set temp_path=c:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\msdev.exe
if not exist "%temp_path%" goto second_guess
echo   Found %temp_path%
echo set temp_path=%temp_path% > set_msdev.bat
goto start_building


:second_guess

set temp_path=d:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\msdev.exe
if not exist "%temp_path%" goto global_search
echo   Found %temp_path%
echo set temp_path=%temp_path% > set_msdev.bat

goto start_building

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:global_search
:: We'll have to do a complete search on all this machine's drives for it...
echo    Did not find msdev.exe in the usual places.
echo  Searching all drives (This may take a while!)

FOR %%D IN (C D E F G H I J K L M N O P Q R S T U V W X Y Z) DO call %0 sub search_drive %%D

if exist set_msdev.bat goto start_building

echo Could not find msdev.exe!  Exiting.
goto end


:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::: This subroutine does a directory search for msdev on the specified drive and loads the result into the %temp_path% environment variable
::::: Syntax: %0 sub search_drive <drive letter>

:search_drive

if "%3"=="" (
   echo No drive specified!
   goto end
)

::::: If set_msdev.bat exists at this point, we assume it is valid and exit immediately...
if exist set_msdev.bat goto end

::::: Make sure the specified drive exists...
if not exist %3: goto end

::::: Try finding msdev.exe on the specified drive
echo    Searching drive %3...

::::: Unfortunately, this will find every instance of msdev.exe on the listed drive
:::::  - we only want one, and we can only hope it's the right one.
::::: The 2>&1 redirects stderr (which will get a "File Not Found") to stdout.
::::: The find commmand just filters out any "File Not Found" string.
DIR %3:\msdev.exe /S /B 2>&1 |find /V "File Not Found" > dirout.txt
call %0 sub set_env_var .\dirout.txt

del .\dirout.txt 2>&1 >nul

::::: Double check the path...
if not "%temp_path%"=="" (
   if exist "%temp_path%" (
      echo   Found %temp_path%
      goto end
   )
)

set temp_path=
del set_msdev.bat 2>&1 >nul
goto end


:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::: Subroutine: gets the results of a one-line file into %msdev%
::::: syntax for calling:  %0 sub set_env_var <file>
:::::The "oe" under "rcx" defines the HEX length (14) of the set line, alter if modifying the length of the variable name. 

:set_env_var

::::: name of the file {(tmp).bah} we're going to work on...
> (tmp).bac echo n (tmp).bah

:::: Enter the string just after the Program Segment Prefix...
>>(tmp).bac echo e 100 "set temp_path="

::::modify register cx 
>>(tmp).bac echo rcx
>>(tmp).bac echo 0e

:::: write...
>>(tmp).bac echo w
:::: quit...
>>(tmp).bac echo q
debug < (tmp).bac > nul

::::::Concatenate the two files...
copy /B (tmp).bah + /A %3 set_msdev.bat /A> nul

del (tmp).ba?

::::: Just in case something went wrong...
if not exist set_msdev.bat (
   set temp_path=
   goto end
)

call .\set_msdev.bat

goto end

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


:start_building

:: Call the bat file to set the environment variable...
call set_msdev.bat

::::: We have to enclose the path in quotes so the command lines following don't mistake paths with spaces in them as separate arguments...
set ntcss_msdev="%temp_path%"

if %CLEAN_ONLY%==true goto Begin_clean
if not %CLEAN%==true goto skip_clean

:Begin_clean
::::::: Get rid of any pre-existing log file for the clean process..
echo Cleaning all projects...
if exist .\clean_results.txt del .\clean_results.txt > nul


%ntcss_msdev% .\ACLLibs\ACLLibs.dsp /MAKE "ACLLibs - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
::::::: Concatenate the last clean operation's (temporary) output logfile to a master log file..
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\AppLibDll\AppLibDll.dsp /MAKE "AppLibDll - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\AppLibTest\AppLibTest.dsp /MAKE "AppLibTest - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\AppPrintConfig\AppPrintConfig.dsp /MAKE "AppPrintConfig - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\Common_Lib\Comn_Lib.dsp /MAKE "Comn_Lib - Win32 DLL_Version_%build_type%" "Comn_Lib - Win32 App_Version_%build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\crypter\crypter.dsp /MAKE "crypter - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\DT\DT.dsp /MAKE "DT - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\DTLib\DTLib.dsp /MAKE "DTLib - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\InriZip\InriZip.dsp /MAKE "InriZip - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\MsgBoard\MsgBoard.dsp /MAKE "MsgBoard - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\NtcssDll\NtcssDll.dsp /MAKE "NtcssDll - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\pdj_conf\pdj_conf.dsp /MAKE "pdj_conf - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\pqueue\pqueue.dsp /MAKE "pqueue - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\PrinterUI\PrinterUI.dsp /MAKE "PrinterUI - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\SecBan\SecBan.dsp /MAKE "SecBan - Win32 Windows NT %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\ServerTest\ServerTest.dsp /MAKE "ServerTest - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\Setup\Setup2.dsp /MAKE "Setup2 - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\SPQ\Spqprop.dsp /MAKE "Spqprop - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\SystemMonitor.v2.0.0\SystemMonitor.dsp /MAKE "SystemMonitor - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\UC\UserConfig.dsp /MAKE "UserConfig - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt
%ntcss_msdev% .\ZipFile\ZipFile.dsp /MAKE "ZipFile - Win32 %build_type%" /CLEAN /OUT .\tmp_logfile
type .\tmp_logfile >> .\clean_results.txt

:::::: Get rid of the temp file..
if exist .\tmp_logfile del .\tmp_logfile > nul

if %CLEAN_ONLY%==true goto end


:skip_clean

:::::: Get rid of any pre-existing log file for the clean process..
if exist .\build_results.txt del .\build_results.txt > nul

::================ FIRST TIER ============================================================

echo Building the ACL libs...
%ntcss_msdev% .\ACLLibs\ACLLibs.dsp /MAKE "ACLLibs - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_ACLLibs
goto handle_output
:done_ACLLibs

echo Building the Common libs for all projects...
%ntcss_msdev% .\Common_Lib\Comn_Lib.dsp /MAKE "Comn_Lib - Win32 DLL_Version_%build_type%" "Comn_Lib - Win32 App_Version_%build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_Common_Lib
goto handle_output
:done_Common_Lib

echo Building the Zip library...
%ntcss_msdev% .\ZipFile\ZipFile.dsp /MAKE "ZipFile - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_ZipFile
goto handle_output
:done_ZipFile

echo Building the NTCSS Zip DLL...
%ntcss_msdev% .\InriZip\InriZip.dsp /MAKE "InriZip - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_InriZip
goto handle_output
:done_InriZip

echo Building the server testing program...
%ntcss_msdev% .\ServerTest\ServerTest.dsp /MAKE "ServerTest - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_ServerTest
goto handle_output
:done_ServerTest

::================ SECOND TIER ===========================================================

echo Building the NTCSS DLL...
%ntcss_msdev% .\NtcssDll\NtcssDll.dsp /MAKE "NtcssDll - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_NtcssDll
goto handle_output
:done_NtcssDll

echo Building the NTCSS progroups.ini setup program...
%ntcss_msdev% .\Setup\Setup2.dsp /MAKE "Setup2 - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_Setup
goto handle_output
:done_Setup


::================ THIRD TIER ===========================================================


echo Building the App DLL...
%ntcss_msdev% .\AppLibDll\AppLibDll.dsp /MAKE "AppLibDll - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_AppLibDll
goto handle_output
:done_AppLibDll

echo Building the App printer config program...
%ntcss_msdev% .\AppPrintConfig\AppPrintConfig.dsp /MAKE "AppPrintConfig - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_AppPrintConfig
goto handle_output
:done_AppPrintConfig

:crypter
echo Building the encryption/decryption program...
%ntcss_msdev% .\crypter\crypter.dsp /MAKE "crypter - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_crypter
goto handle_output
:done_crypter

:DTLib
echo Building the desktop DLL...
%ntcss_msdev% .\DTLib\DTLib.dsp /MAKE "DTLib - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_DTLib
goto handle_output
:done_DTLib

:MsgBoard
echo Building the Message Bulletin Boards program...
%ntcss_msdev% .\MsgBoard\MsgBoard.dsp /MAKE "MsgBoard - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_MsgBoard
goto handle_output
:done_MsgBoard

:pdj_conf
echo Building the Predefined JOb Configuration program...
%ntcss_msdev% .\pdj_conf\pdj_conf.dsp /MAKE "pdj_conf - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_pdj_conf
goto handle_output
:done_pdj_conf

:pqueue
echo Building the Printer Queue program...
%ntcss_msdev% .\pqueue\pqueue.dsp /MAKE "pqueue - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_pqueue
goto handle_output
:done_pqueue

:PrinterUI
echo Building the Printer Configuration program...
%ntcss_msdev% .\PrinterUI\PrinterUI.dsp /MAKE "PrinterUI - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_PrinterUI
goto handle_output
:done_PrinterUI

:SecBan
echo Building the security banner program...
%ntcss_msdev% .\SecBan\SecBan.dsp /MAKE "SecBan - Win32 Windows NT %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_SecBan
goto handle_output
:done_SecBan

:SPQ
echo Building the Server Process Queue program...
%ntcss_msdev% .\SPQ\Spqprop.dsp /MAKE "Spqprop - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_SPQ
goto handle_output
:done_SPQ

:SystemMonitor.v2.0.0
echo Building the System Monitor program...
%ntcss_msdev% .\SystemMonitor.v2.0.0\SystemMonitor.dsp /MAKE "SystemMonitor - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_SystemMonitor.v2.0.0
goto handle_output
:done_SystemMonitor.v2.0.0

:UC
echo Building the User Configuration program...
%ntcss_msdev% .\UC\UserConfig.dsp /MAKE "UserConfig - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_UC
goto handle_output
:done_UC

::================ FOURTH TIER ===========================================================

:DT
echo Building the desktop program...
%ntcss_msdev% .\DT\DT.dsp /MAKE "DT - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_DT
goto handle_output
:done_DT

:AppLibTest
echo Building the App Library tester...
%ntcss_msdev% .\AppLibTest\AppLibTest.dsp /MAKE "AppLibTest - Win32 %build_type%" /REBUILD /OUT .\tmp_logfile
set return_to=done_AppLibTest
goto handle_output
:done_AppLibTest


echo Done.  See the build_results.txt file for any warnings/errors generated during the build.



:::::: Get rid of the temp file..
if exist .\tmp_logfile del .\tmp_logfile > nul
goto end



::====================== subroutine ==========================
:handle_output

::::: Append the last compiles' output file to the master file....
type .\tmp_logfile >> .\build_results.txt

::::: Check for any source errors during the last compile...
find ") : error " .\tmp_logfile >nul
if not errorlevel 1 (
   echo.
   echo   COMPILER ERROR!! See file .\tmp_logfile for details...
   echo.
   echo TERMINATING BUILD.
   echo.
   goto end
   )

::::: Check for any Link errors during the last compile...
find "Error executing link.exe." .\tmp_logfile >nul
if not errorlevel 1 (
   echo.
   echo   LINKER ERROR!! See file .\tmp_logfile for details...
   echo.
   echo TERMINATING BUILD.
   echo.
   goto end
   )

goto %return_to%

::====================== subroutine ==========================
:usage
   echo "Usage: %0 release|debug|all [clean | clean_only]

:end
