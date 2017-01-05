# Microsoft Developer Studio Project File - Name="PrinterUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PrinterUI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PrinterUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PrinterUI.mak" CFG="PrinterUI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PrinterUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PrinterUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PrinterUI - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\Common_Lib\app_release\app_clib.lib ..\NtcssDLL\release\ntcssdll.lib wsock32.lib MPR.lib lz32.lib winspool.lib Setupapi.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin\prtui.exe"

!ELSEIF  "$(CFG)" == "PrinterUI - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\Common_Lib\app_debug\app_clib.lib ..\NtcssDLL\debug\ntcssdll.lib wsock32.lib MPR.lib lz32.lib winspool.lib Setupapi.lib /nologo /stack:0x57000 /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\bin\prtui.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "PrinterUI - Win32 Release"
# Name "PrinterUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AddNTPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\EditAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\EditPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\ini.cpp
# End Source File
# Begin Source File

SOURCE=.\ini2.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallFromDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\ipaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\msgs.cpp
# End Source File
# Begin Source File

SOURCE=.\Print2FileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrinterUI.cpp
# End Source File
# Begin Source File

SOURCE=.\PrinterUI.rc
# End Source File
# Begin Source File

SOURCE=.\PrinterUIDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PrinterUIView.cpp
# End Source File
# Begin Source File

SOURCE=.\PUIHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\PUIListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\RedirectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectShareDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AddNTPrinter.h
# End Source File
# Begin Source File

SOURCE=.\EditAdapter.h
# End Source File
# Begin Source File

SOURCE=.\EditPrinter.h
# End Source File
# Begin Source File

SOURCE=.\ini.h
# End Source File
# Begin Source File

SOURCE=.\ini2.h
# End Source File
# Begin Source File

SOURCE=.\InstallFromDisk.h
# End Source File
# Begin Source File

SOURCE=.\ipaddr.h
# End Source File
# Begin Source File

SOURCE=.\LogFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\msgs.h
# End Source File
# Begin Source File

SOURCE=.\Print2FileDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrinterUI.h
# End Source File
# Begin Source File

SOURCE=.\PrinterUIDoc.h
# End Source File
# Begin Source File

SOURCE=.\PrinterUIView.h
# End Source File
# Begin Source File

SOURCE=.\PUIHelp.h
# End Source File
# Begin Source File

SOURCE=.\PUIListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RedirectDialog.h
# End Source File
# Begin Source File

SOURCE=.\SelectShareDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\adapter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\grp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\netprinter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port.bmp
# End Source File
# Begin Source File

SOURCE=.\res\printer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\printer.ico
# End Source File
# Begin Source File

SOURCE=.\res\PrinterUI.ico
# End Source File
# Begin Source File

SOURCE=.\res\PrinterUI.rc2
# End Source File
# Begin Source File

SOURCE=.\res\PrinterUIDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# End Group
# End Target
# End Project
# Section PrinterUI : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:102
# 	2:21:SplashScreenInsertKey:4.0
# End Section
