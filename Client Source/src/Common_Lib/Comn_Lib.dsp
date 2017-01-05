# Microsoft Developer Studio Project File - Name="comn_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=comn_lib - Win32 DLL_Version_release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Comn_Lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Comn_Lib.mak" CFG="comn_lib - Win32 DLL_Version_release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "comn_lib - Win32 DLL_Version_release" (based on "Win32 (x86) Static Library")
!MESSAGE "comn_lib - Win32 App_Version_release" (based on "Win32 (x86) Static Library")
!MESSAGE "comn_lib - Win32 DLL_Version_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "comn_lib - Win32 App_Version_debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "comn_lib - Win32 DLL_Version_release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\comn_li6"
# PROP BASE Intermediate_Dir ".\comn_li6"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\DLL_release"
# PROP Intermediate_Dir ".\DLL_release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\comn_lib" /I "..\ntcssdll" /I "..\..\source\include" /I "..\..\32_bit\comn_lib" /I "..\..\source\ntcssdll" /I "..\..\32_bit\ntcssdll" /I "..\..\source\comn_lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NTCSS_DLL_BUILD" /YX /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I ".\\" /I "..\includes" /I "..\NtcssDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NTCSS_DLL_BUILD" /D "SKIP_HELP" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\dll_clib.lib"
# ADD LIB32 /nologo /out:"dll_release\dll_clib.lib"

!ELSEIF  "$(CFG)" == "comn_lib - Win32 App_Version_release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\comn_li7"
# PROP BASE Intermediate_Dir ".\comn_li7"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\APP_release"
# PROP Intermediate_Dir ".\APP_release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\comn_lib" /I "..\ntcssdll" /I "..\..\source\include" /I "..\..\32_bit\comn_lib" /I "..\..\source\ntcssdll" /I "..\..\32_bit\ntcssdll" /I "..\..\source\comn_lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NTCSS_DLL_BUILD" /YX /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I ".\\" /I "..\includes" /I "..\NtcssDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\dll_clib.lib"
# ADD LIB32 /nologo /out:"APP_release\app_clib.lib"

!ELSEIF  "$(CFG)" == "comn_lib - Win32 DLL_Version_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\comn_li1"
# PROP BASE Intermediate_Dir ".\comn_li1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DLL_debug"
# PROP Intermediate_Dir ".\DLL_debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "..\..\source\include" /I "..\..\32_bit\comn_lib" /I "..\..\source\ntcssdll" /I "..\..\32_bit\ntcssdll" /I "..\..\source\comn_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /Zp1 /MTd /W3 /GX /Z7 /Od /I ".\\" /I "..\includes" /I "..\NtcssDLL" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NTCSS_DLL_BUILD" /FR /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\comn_lib.lib"
# ADD LIB32 /nologo /out:"DLL_debug\dll_clib.lib"

!ELSEIF  "$(CFG)" == "comn_lib - Win32 App_Version_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\comn_li2"
# PROP BASE Intermediate_Dir ".\comn_li2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\APP_debug"
# PROP Intermediate_Dir ".\APP_debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "..\..\source\include" /I "..\..\32_bit\comn_lib" /I "..\..\source\ntcssdll" /I "..\..\32_bit\ntcssdll" /I "..\..\source\comn_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /Zp1 /MTd /W3 /GX /Z7 /Od /I ".\\" /I "..\includes" /I "..\NtcssDLL" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\comn_lib.lib"
# ADD LIB32 /nologo /out:"APP_debug\app_clib.lib"

!ENDIF 

# Begin Target

# Name "comn_lib - Win32 DLL_Version_release"
# Name "comn_lib - Win32 App_Version_release"
# Name "comn_lib - Win32 DLL_Version_debug"
# Name "comn_lib - Win32 App_Version_debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\cregkey.cpp
# End Source File
# Begin Source File

SOURCE=.\encrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\Err_man.cpp
# End Source File
# Begin Source File

SOURCE=.\Etc.cpp
# End Source File
# Begin Source File

SOURCE=.\Genutils.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_man.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Inri_FTP.cpp
# End Source File
# Begin Source File

SOURCE=.\Masked.cpp
# End Source File
# Begin Source File

SOURCE=.\MemChain.cpp
# End Source File
# Begin Source File

SOURCE=.\MyHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\NtcssPaths.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\ScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# End Source File
# Begin Source File

SOURCE=.\StringEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeoutSocket.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\cregkey.h
# End Source File
# Begin Source File

SOURCE=.\encrypt.h
# End Source File
# Begin Source File

SOURCE=.\Err_man.h
# End Source File
# Begin Source File

SOURCE=.\Etc.h
# End Source File
# Begin Source File

SOURCE=.\FieldHelpTopics.h
# End Source File
# Begin Source File

SOURCE=.\GenUtils.h
# End Source File
# Begin Source File

SOURCE=.\Ini_man.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\Inri_FTP.h
# End Source File
# Begin Source File

SOURCE=.\kind_of.h
# End Source File
# Begin Source File

SOURCE=.\Masked.h
# End Source File
# Begin Source File

SOURCE=.\MemChain.h
# End Source File
# Begin Source File

SOURCE=.\MsgCore.h
# End Source File
# Begin Source File

SOURCE=.\Msgdlvmn.h
# End Source File
# Begin Source File

SOURCE=.\MyHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NtcssHelp.h
# End Source File
# Begin Source File

SOURCE=.\Ntcssmsg.h
# End Source File
# Begin Source File

SOURCE=.\NtcssPaths.h
# End Source File
# Begin Source File

SOURCE=.\ResizingDialog.h
# End Source File
# Begin Source File

SOURCE=.\Schedule.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StringEx.h
# End Source File
# Begin Source File

SOURCE=.\TimeoutSocket.h
# End Source File
# Begin Source File

SOURCE=.\Tttime.h
# End Source File
# Begin Source File

SOURCE=.\WindowHelpTopics.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
