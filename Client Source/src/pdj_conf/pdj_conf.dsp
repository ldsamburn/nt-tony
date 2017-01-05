# Microsoft Developer Studio Project File - Name="pdj_conf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=pdj_conf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pdj_conf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pdj_conf.mak" CFG="pdj_conf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pdj_conf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "pdj_conf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pdj_conf - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\includes" /I "..\Common_lib" /I "..\NtcssDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib ..\Common_lib\app_release\app_clib.lib ..\NtcssDLL\release\ntcssdll.lib /nologo /stack:0x40960 /subsystem:windows /machine:I386 /out:"../../bin/pdj_conf.exe"

!ELSEIF  "$(CFG)" == "pdj_conf - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\Common_lib" /I "..\NtcssDLL" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib ..\Common_lib\app_debug\app_clib.lib ..\NtcssDLL\debug\ntcssdll.lib /nologo /stack:0x40960 /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "pdj_conf - Win32 Release"
# Name "pdj_conf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\AppLckDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwinsock.cpp
# End Source File
# Begin Source File

SOURCE=.\pdj_conf.cpp
# End Source File
# Begin Source File

SOURCE=.\pdj_conf.rc
# End Source File
# Begin Source File

SOURCE=.\pdj_confDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PDJHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\PDJListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\pdjMsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\ScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SysLog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddEdit.h
# End Source File
# Begin Source File

SOURCE=.\AppLckDlg.h
# End Source File
# Begin Source File

SOURCE=.\Cwinsock.hpp
# End Source File
# Begin Source File

SOURCE=.\pdj_conf.h
# End Source File
# Begin Source File

SOURCE=.\pdj_confDlg.h
# End Source File
# Begin Source File

SOURCE=.\PDJHelp.h
# End Source File
# Begin Source File

SOURCE=.\PDJListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\pdjMsgs.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysLog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\pdj_conf.ico
# End Source File
# Begin Source File

SOURCE=.\res\pdj_conf.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Pdjconf.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
