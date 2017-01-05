# Microsoft Developer Studio Project File - Name="DT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DT.mak" CFG="DT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DT - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DT - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DT - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\includes" /I "..\DTLib" /I "..\Common_Lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Netapi32.lib ..\Common_Lib\APP_release\app_clib.lib ..\DTLIB\release\dtlib.lib ..\NtcssDLL\release\ntcssdll.lib Winmm.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\bin\frame.exe"

!ELSEIF  "$(CFG)" == "DT - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I "..\DTLib" /I "..\Common_Lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Netapi32.lib ..\Common_Lib\APP_debug\app_clib.lib ..\DTLIB\debug\dtlib.lib ..\NtcssDLL\debug\ntcssdll.lib Winmm.lib /nologo /stack:0x57000 /subsystem:windows /debug /machine:I386 /out:"..\..\bin\framed.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DT - Win32 Release"
# Name "DT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\DT.cpp
# End Source File
# Begin Source File

SOURCE=.\DT.rc
# End Source File
# Begin Source File

SOURCE=.\DTDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DTSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxGroupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxOutBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\LogonBanner.cpp
# End Source File
# Begin Source File

SOURCE=.\NewLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\ControlToolTip.h
# End Source File
# Begin Source File

SOURCE=.\DT.h
# End Source File
# Begin Source File

SOURCE=.\DTDlg.h
# End Source File
# Begin Source File

SOURCE=.\DTSocket.h
# End Source File
# Begin Source File

SOURCE=.\GfxGroupEdit.h
# End Source File
# Begin Source File

SOURCE=.\GfxOutBarCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GfxPopupMenu.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\LogonBanner.h
# End Source File
# Begin Source File

SOURCE=.\NewLabel.h
# End Source File
# Begin Source File

SOURCE=.\NtcssConfig.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\32x32x16_Left.ico
# End Source File
# Begin Source File

SOURCE=.\res\32x32x16_Right.ico
# End Source File
# Begin Source File

SOURCE=.\res\ANCHOR.ICO
# End Source File
# Begin Source File

SOURCE=.\res\back.ico
# End Source File
# Begin Source File

SOURCE=.\res\back1.ico
# End Source File
# Begin Source File

SOURCE=.\res\back256.ico
# End Source File
# Begin Source File

SOURCE=.\res\back257.ico
# End Source File
# Begin Source File

SOURCE=.\res\back258.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DESKTOP.ICO
# End Source File
# Begin Source File

SOURCE=.\res\dragging.cur
# End Source File
# Begin Source File

SOURCE=.\res\DT.ico
# End Source File
# Begin Source File

SOURCE=.\res\DT.rc2
# End Source File
# Begin Source File

SOURCE=.\res\EXPLODE_.ICO
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icr_hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\imageList.bmp
# End Source File
# Begin Source File

SOURCE=.\res\last.ico
# End Source File
# Begin Source File

SOURCE=.\res\last1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Menu.ico
# End Source File
# Begin Source File

SOURCE=.\res\next256.ico
# End Source File
# Begin Source File

SOURCE=.\res\next257.ico
# End Source File
# Begin Source File

SOURCE=.\res\next258.ico
# End Source File
# Begin Source File

SOURCE=.\res\nodraggi.cur
# End Source File
# Begin Source File

SOURCE=.\res\OutBarDemo.ico
# End Source File
# Begin Source File

SOURCE=.\res\question256.ico
# End Source File
# Begin Source File

SOURCE=.\res\Search.ico
# End Source File
# Begin Source File

SOURCE=.\res\search256.ico
# End Source File
# Begin Source File

SOURCE=.\res\User256.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
