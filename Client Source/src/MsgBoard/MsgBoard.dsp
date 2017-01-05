# Microsoft Developer Studio Project File - Name="MsgBoard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MsgBoard - Win32 Debug NT
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MsgBoard.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MsgBoard.mak" CFG="MsgBoard - Win32 Debug NT"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MsgBoard - Win32 Debug NT" (based on "Win32 (x86) Application")
!MESSAGE "MsgBoard - Win32 Release NT" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MsgBoard - Win32 Debug NT"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\MsgBoard"
# PROP BASE Intermediate_Dir ".\MsgBoard"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DebugNT"
# PROP Intermediate_Dir ".\DebugNT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\msgboard\source" /I "..\msgboard" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\includes" /I ".\source" /I "." /I "..\Common_Lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "SKIP_HELP" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib ..\lib\reg_clib.lib ..\lib\ntcssdll.lib /nologo /stack:0x17700 /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\bin\MsgBoard.exe"
# ADD LINK32 wsock32.lib ..\Common_Lib\app_debug\app_clib.lib ..\NtcssDLL\debug\ntcssdll.lib /nologo /stack:0x17700 /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"Debug\MsgBoard.exe"

!ELSEIF  "$(CFG)" == "MsgBoard - Win32 Release NT"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\MsgBoar2"
# PROP BASE Intermediate_Dir ".\MsgBoar2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ReleaseNT"
# PROP Intermediate_Dir ".\ReleaseNT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\include" /I "..\msgboard\source" /I "..\msgboard" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\includes" /I ".\source" /I "." /I "..\Common_Lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SKIP_HELP" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib ..\lib\reg_clib.lib ..\lib\ntcssdll.lib /nologo /stack:0x17700 /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\bin\MsgBoard.exe"
# ADD LINK32 wsock32.lib wsock32.lib ..\Common_Lib\app_release\app_clib.lib ..\NtcssDLL\release\ntcssdll.lib /nologo /stack:0x17700 /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\bin\MsgBoard.exe"

!ENDIF 

# Begin Target

# Name "MsgBoard - Win32 Debug NT"
# Name "MsgBoard - Win32 Release NT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\Cwinsock.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Mb_doc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mb_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mb_noteDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mb_noteView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Mb_vw.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\msg_doc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MsgBHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Msgboard.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgBoard.rc
# End Source File
# Begin Source File

SOURCE=.\Source\msgs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\pmsg_vw.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\rmsg_vw.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\source\cwinsock.hpp
# End Source File
# Begin Source File

SOURCE=.\Source\DIALOGS.H
# End Source File
# Begin Source File

SOURCE=.\Source\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\Source\Mb_doc.h
# End Source File
# Begin Source File

SOURCE=.\Source\mb_mgr.h
# End Source File
# Begin Source File

SOURCE=.\Source\mb_noteDoc.h
# End Source File
# Begin Source File

SOURCE=.\Source\mb_noteView.h
# End Source File
# Begin Source File

SOURCE=.\Source\Mb_vw.h
# End Source File
# Begin Source File

SOURCE=.\Source\msg_doc.h
# End Source File
# Begin Source File

SOURCE=.\Source\MsgBHelp.h
# End Source File
# Begin Source File

SOURCE=.\source\msgboard.h
# End Source File
# Begin Source File

SOURCE=.\Source\msgs.h
# End Source File
# Begin Source File

SOURCE=.\Source\pmsg_vw.h
# End Source File
# Begin Source File

SOURCE=.\Source\rmsg_vw.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\BITMAP1.BMP
# End Source File
# Begin Source File

SOURCE=.\res\BITMAP2.BMP
# End Source File
# Begin Source File

SOURCE=.\res\CHECK.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\IDR_MSG_.BMP
# End Source File
# Begin Source File

SOURCE=.\res\idr_msgb.ico
# End Source File
# Begin Source File

SOURCE=.\res\MB_DOC.ICO
# End Source File
# Begin Source File

SOURCE=.\res\MsgBoard.ico
# End Source File
# Begin Source File

SOURCE=.\res\MsgBoard.rc2
# End Source File
# Begin Source File

SOURCE=.\res\newmail.bmp
# End Source File
# Begin Source File

SOURCE=.\res\oldmail.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
