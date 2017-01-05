/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\ntcssdef.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file contains common, miscellaneous NTCSS and related
// definitions used by the DLL, SecBan, etc.
//
// Put stuff in here only if it is applicable/useful to more than 
// one project - if it is specific to DLL or application only then put it 
// in a project include file.
//----------------------------------------------------------------------

#ifndef _INCLUDE_NTCSSDEF_H_CLASS_
#define _INCLUDE_NTCSSDEF_H_CLASS_

// ***
// *** Keep defines/const ints in alphabetical order and/or ordered
// ***  by sections pertaining to use
// ***

                                       // Set up our definitions to
                                       //  indicate if doing 16-bit or
                                       //  a 32-bit build and also set
                                       //  up DLL entry point mechanism
                                       //  appropriate for 16/32 bit
#undef  DllImport
#undef  DllExport



#define NTCSS_32BIT_BUILD

#define DllImport __declspec(dllimport) WINAPI

#define DllExport __declspec(dllexport) WINAPI



                                       // WSPTODO - used internally
                                       //  to document that dealing
                                       //  with a security level value
typedef int SecurityLevel;                                       


                                       // int functions usually return
                                       //   this on error
#define RETURN_INT_ERROR        -1


                                       // Min/Max definitions
                                       //
#define MAX_ERROR_MSG_LEN          256

                                       // Ports through which to pass
                                       //  msgs to the server
                                       //
#define NTCSS_PORT_CMDPORT            9152
#define NTCSS_PORT_DBPORT             9132
#define NTCSS_PORT_PQPORT			  9122
#define NTCSS_PORT_LPR                515
#define NTCSS_PORT_MSGBOARD           9172
#define NTCSS_PORT_SPQPORT            9162
#define NTCSS_PORT_TIMEPORT           13
#define NTCSS_PORT_DSKPORT            9142


                                       // Used to find DeskTop window
                                       //
const char NTCSS_DESKTOP_CLASS_NAME[] =   "WHOCARES";
const char NTCSS_DESKTOP_WINDOW_NAME[] =  "NTCSS II Login";

                                       // Used in NtcssDLLInitialize
                                       //  calls for MsgBoard, etc.
#define NTCSS_APP_NAME  "NTCSS"

                                       // Eric uses common data tags with
                                       //  this at front, can't let
                                       //  users use tags like this
                                       //
#define NTCSS_RESERVED_COMMON_TAG_LEAD  "NTCSS"

                                       // In NTCSS Windows Init file

#define NTCSSCODE_RESERVED_FOR_USE          999
#define NTCSSCODE_VALITY_CHECK_VALUE        0x777

                                       // Used as application name to
                                       //   tell NTCSS DLL that an app
                                       //   is 'special', e.g. SecBan,
                                       //   DeskTop.
                                       //  
#define NTCSS_PRIVATE_APPNAME       "NTCSS"

                                       // Name of NTCSS Windows Init 
                                       //   file
                                       //
#define NTCSS_INI_FILE_NM       "\\ntcss.ini"


                                       // Name used in registering
                                       //  SecBan app
#define SECBAN_CLASS_NAME  "SECBAN"

                                       // Messages handled by SecBan
                                       //
const int SECBAN_TOCHK_MSG           = WM_USER + 10;
const int SECBAN_KILLIT_MSG          = WM_USER + 20;
const int SECBAN_GETSESSIONSLOT_MSG  = WM_USER + 30;
const int SECBAN_DO_WARNING_MSG		 = WM_USER + 40;


                                       // Messages to use during
                                       //  development
                                       //
#define UNDER_CONSTRUCTION_USER_MSG "Sorry out of order - this area currently under construction"
#define UNDER_RENOVATION_USER_MSG   "Sorry out of order - this area currently under renovation"

                                       // These used by SecBan to tell 
                                       //   DeskTop whether uses accepts
                                       //   conditions
                                       //
#define WM_SECACCEPT        WM_USER + 400
#define WM_SECDENY          WM_USER + 401


#endif // _INCLUDE_NTCSSDEF_H_CLASS_