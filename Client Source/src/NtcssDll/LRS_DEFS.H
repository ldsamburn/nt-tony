/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Lrs_Defs.h                    
//                                         
// This file Contains Login Reply Structure declarations.  The Login
//  Reply Structure is data supplied to the NTCSS DLL by the DeskTop.
//
//----------------------------------------------------------------------

#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_LSR_DEFS_H_CLASS_
#define _INCLUDE_LSR_DEFS_H_CLASS_

#include <ntcssdef.h>
#include <ntcssapi.h>


typedef struct SuserInfoRec_wFullData 
{
    char             token_buf[SIZE_TOKEN];
    long             unix_id;
    char             login_name_buf[SIZE_LOGIN_NAME];
    char             real_name_buf[SIZE_REAL_NAME];
    char             ssn_buf[SIZE_SSN];
    char             phone_number_buf[SIZE_PHONE_NUMBER];
    SecurityLevel    security_class;
    char             password_change_time[SIZE_PASSWORD_CHANGE_TIME];
    char             shared_passwd_buf[SIZE_APP_PASSWORD];
    int              user_role;
} 
SuserInfoRec_wFullData;


typedef struct SappProgramRec_wFullData  
{
    char                program_title_buf[SIZE_PROGRAM_TITLE];
    char                program_file_name_buf[SIZE_PROGRAM_FILE];
    char                icon_file_name_buf[SIZE_ICON_FILE];
    int                 icon_index;
    char                command_line_buf[SIZE_CMD_LINE_ARGS];
    SecurityLevel       security_class;
    int                 flags;
    char                working_dir_buf[SIZE_SERVER_LOCATION];

    SappProgramRec_wFullData *xxx_next_program_ptr;
} 
SappProgramRec_wFullData ;

typedef struct SappGroupRec_wFullData
{
    char            progroup_title_buf[SIZE_GROUP_TITLE];
    char            host_name_buf[SIZE_HOST_NAME];
	int             server_type;  /*10/22*/
    int             app_role;
    char            app_data_buf[SIZE_APP_DATA];
    char            app_password_buf[SIZE_APP_PASSWORD];
    char            icon_file_buf[SIZE_ICON_FILE];
    int             icon_index;
    char            version_number_buf[SIZE_VERSION_NUMBER];
    char            release_date_buf[SIZE_RELEASE_DATE];
    char            client_location_buf[SIZE_CLIENT_LOCATION];
    char            server_location_buf[SIZE_SERVER_LOCATION];

    unsigned int                 program_cnt;
    SappProgramRec_wFullData    *xxx_program_list;

    SappGroupRec_wFullData      *xxx_next_group_ptr;
} 
SappGroupRec_wFullData ;

                                       // The dll has to copy the Login
                                       //  Reply Structure from the Desktop
                                       //  to shared dll memory.  The
                                       //  CdllUser class is responsible
                                       //  for this and uses this structure
                                       //  to access the data
typedef struct SshmemLoginReplyStruct
{
    int                           validity_check_value;
    SuserInfoRec_wFullData        user_rec;

    unsigned int                   app_group_cnt;
    SappGroupRec_wFullData        *xxx_app_group_list;

} SshmemLoginReplyStruct;


#endif // _INCLUDE_LSR_DEFS_H_CLASS_


