# Microsoft Developer Studio Project File - Name="Pqueue" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Pqueue - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Pqueue.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pqueue.mak" CFG="Pqueue - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Pqueue - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Pqueue - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Pqueue - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\release"
# PROP Intermediate_Dir ".\release"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O1 /Ob0 /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"PQUEUE.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Pqueue.bsc"
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winsock.lib ntcss.lib /nologo /stack:0x40960 /subsystem:windows /machine:IX86
# ADD LINK32 nafxcw.lib libcmt.lib wsock32.lib ..\NtcssDLL\release\ntcssdll.lib ..\Common_Lib\app_release\app_clib.lib /nologo /stack:0x40960 /subsystem:windows /machine:IX86 /out:"..\..\bin\pqueue.exe"

!ELSEIF  "$(CFG)" == "Pqueue - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir ".\debug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\Common_Lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"PQUEUE.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib winsock.lib ntcss.lib /nologo /stack:0x40960 /subsystem:windows /debug /machine:IX86
# ADD LINK32 oldnames.lib wsock32.lib ..\NtcssDLL\debug\ntcssdll.lib ..\Common_Lib\app_debug\app_clib.lib  ctl3d32.lib /nologo /stack:0x40960 /subsystem:windows /debug /machine:IX86 /nodefaultlib:"libcd.lib"

!ENDIF 

# Begin Target

# Name "Pqueue - Win32 Release"
# Name "Pqueue - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\BUNDLEDA.C
# End Source File
# Begin Source File

SOURCE=.\CWINSOCK.CPP
# End Source File
# Begin Source File

SOURCE=.\DMCDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\PQDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\pqhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\pqlistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\pqmsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\PQUEUDOC.CPP
# End Source File
# Begin Source File

SOURCE=.\PQUEUE.CPP
# End Source File
# Begin Source File

SOURCE=.\PQUEUE.DEF
# End Source File
# Begin Source File

SOURCE=.\PQUEUE.RC
# End Source File
# Begin Source File

SOURCE=.\PQUEUVW.CPP
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BUNDLEDA.H
# End Source File
# Begin Source File

SOURCE=.\CWINSOCK.HPP
# End Source File
# Begin Source File

SOURCE=.\DMCDLG.H
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\NTCSSS.H
# End Source File
# Begin Source File

SOURCE=.\PQDLG.H
# End Source File
# Begin Source File

SOURCE=.\PQHelp.h
# End Source File
# Begin Source File

SOURCE=.\pqlistctrl.h
# End Source File
# Begin Source File

SOURCE=.\pqmsgs.h
# End Source File
# Begin Source File

SOURCE=.\PQUEUDOC.H
# End Source File
# Begin Source File

SOURCE=.\PQUEUE.H
# End Source File
# Begin Source File

SOURCE=.\PQUEUVW.H
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\MLW.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\PQUEUE.RC2
# End Source File
# Begin Source File

SOURCE=.\RES\PRT.ICO
# End Source File
# End Group
# End Target
# End Project
