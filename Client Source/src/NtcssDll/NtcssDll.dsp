# Microsoft Developer Studio Project File - Name="NtcssDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NtcssDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NtcssDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NtcssDll.mak" CFG="NtcssDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NtcssDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NtcssDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NtcssDll - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "..\includes" /I "..\Common_lib" /I "..\Zipfile" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /D "SKIP_HELP" /D "_MBCS" /D "NTCSS_DLL_BUILD" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\Common_lib\DLL_release\dll_clib.lib ..\ZipFile\Release\Zipfile.lib netapi32.lib ..\ZipFile\Implode.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"msvcrtd.lib" /out:"..\..\bin\NtcssDll.dll"
# SUBTRACT LINK32 /verbose /nodefaultlib

!ELSEIF  "$(CFG)" == "NtcssDll - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\Common_lib" /I "..\Zipfile" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /D "SKIP_HELP" /D "STATIC_BUILD" /D "NTCSS_DLL_BUILD" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\Common_lib\DLL_debug\dll_clib.lib ..\ZipFile\DEBUG\Zipfile.lib winmm.lib ..\ZipFile\Implode.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "NtcssDll - Win32 Release"
# Name "NtcssDll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AtlassFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CallMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Dll_msgs.cpp
# End Source File
# Begin Source File

SOURCE=.\DllMan.cpp
# End Source File
# Begin Source File

SOURCE=.\dlluser.cpp
# End Source File
# Begin Source File

SOURCE=.\DllUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\entries.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GetServerFilename.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgDlvMan.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssDll.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssDll.def
# End Source File
# Begin Source File

SOURCE=.\NtcssDll.rc
# End Source File
# Begin Source File

SOURCE=.\NtcssSock.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PrtCFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrtDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\PrtSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrtSFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrtSpoolMan.cpp
# End Source File
# Begin Source File

SOURCE=.\SESSION.CPP
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ValidateUserDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewServerFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\viewsfiledlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AtlassFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\BuildNum.h
# End Source File
# Begin Source File

SOURCE=.\CallMan.h
# End Source File
# Begin Source File

SOURCE=.\Dll_msgs.h
# End Source File
# Begin Source File

SOURCE=.\DllMan.h
# End Source File
# Begin Source File

SOURCE=.\DllUser.h
# End Source File
# Begin Source File

SOURCE=.\DllUtils.h
# End Source File
# Begin Source File

SOURCE=.\FtpDlg.h
# End Source File
# Begin Source File

SOURCE=.\GetServerFilename.h
# End Source File
# Begin Source File

SOURCE=.\MsgDlvMan.h
# End Source File
# Begin Source File

SOURCE=.\NtcssDll.h
# End Source File
# Begin Source File

SOURCE=.\NtcssSock.h
# End Source File
# Begin Source File

SOURCE=.\PrintUtil.h
# End Source File
# Begin Source File

SOURCE=.\PrtCFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrtDriver.h
# End Source File
# Begin Source File

SOURCE=.\PrtSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrtSFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrtSpoolMan.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SESSION.H
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ValidateUserDialog.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# Begin Source File

SOURCE=.\ViewServerFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\viewsfiledlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\NtcssDll.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
