/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllUtils.h                    
//                                         
// This header file contains prototypes for miscellaneous routines
// used by the NTCSS DLL.  Note that miscellaneous routines of a more
// general nature are prototyped in include\genutils.h
// routines.  See function headers for details.
//----------------------------------------------------------------------

#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_DLUTILS_H_
#define _INCLUDE_DLUTILS_H_

#include "ntcssapi.h"
#include "session.h"


void nameFixup(NtcssUserInfo *user_info_ptr, const char *full_name_str);

void makeSpoolFileName(char *spool_file_name_buf);

extern "C" LPSTR SockerrToString( int serr );

int updatedNtcssIniNumValue(char *section_str,
                            char *key_str,
                            int  max_val);

NtcssProcessFilter* CreateBlankFilterRecord(const BOOL get_scheduled_jobs,
		 const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only);

BOOL validateCommonDataTag(char *tag, Csession *session);

#endif // _INCLUDE_DLUTILS_H_
