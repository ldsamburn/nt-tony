# Microsoft Developer Studio Project File - Name="AppPrintConfig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AppPrintConfig - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AppPrintConfig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AppPrintConfig.mak" CFG="AppPrintConfig - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AppPrintConfig - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AppPrintConfig - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppPrintConfig - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /Oa /Ow /Og /Os /Op /Oy /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib ..\Common_Lib\app_release\app_clib.lib ..\NtcssDLL\release\ntcssdll.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin\app_prt.exe"

!ELSEIF  "$(CFG)" == "AppPrintConfig - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib ..\Common_Lib\app_debug\app_clib.lib ..\NtcssDLL\debug\ntcssdll.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCD.LIB" /out:"Debug\app_prt.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "AppPrintConfig - Win32 Release"
# Name "AppPrintConfig - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\APCHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\APCListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AppPrintConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\AppPrintConfig.rc
# End Source File
# Begin Source File

SOURCE=.\AppPrintConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LockedAppWarnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyOtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msgs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AFXIMPL.H
# End Source File
# Begin Source File

SOURCE=.\APCHelp.h
# End Source File
# Begin Source File

SOURCE=.\APCListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AppPrintConfig.h
# End Source File
# Begin Source File

SOURCE=.\AppPrintConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\LockedAppWarnDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyOtDlg.h
# End Source File
# Begin Source File

SOURCE=.\msgs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AppPrintConfig.ico
# End Source File
# Begin Source File

SOURCE=.\res\AppPrintConfig.rc2
# End Source File
# Begin Source File

SOURCE=.\res\otype.bmp
# End Source File
# Begin Source File

SOURCE=.\res\printer.bmp
# End Source File
# End Group
# End Target
# End Project
