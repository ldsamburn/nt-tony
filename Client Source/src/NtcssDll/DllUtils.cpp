/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllUtils.cpp
//                                         
// This is an implementation file containing miscellaneous functions
// used by the NTCSS DLL.
//----------------------------------------------------------------------

#include "stdafx.h"
#include "ctype.h"

#include "0_basics.h"
#include "session.h"
#include "dlluser.h"
#include "DllMan.h"
#include "ntcssapi.h"
#include "genutils.h"
#include "io.h"
#include "direct.h"

/////////////////////////////////////////////////////////////////////////////
// nameFixup
// _____________________
//
// Breaks full_name_str into three part name and puts in appropriate
// fields of user_info_ptr.

void nameFixup(NtcssUserInfo *user_info_ptr, const char *full_name_str)
{
    char tmp_src_buf[SIZE_REAL_NAME];
    strcpy(tmp_src_buf, full_name_str);

    trim(tmp_src_buf);

    char *src_ptr = tmp_src_buf;     
                                       
    char tmp_dest_buf[SIZE_REAL_NAME];
    char *dest_ptr = tmp_dest_buf;
                                       // First name
                                       //
    while ( (*src_ptr != '\0') && (!isspace(*src_ptr)) )
        *dest_ptr++ = *src_ptr++;

    *dest_ptr = '\0';
    strncpy(user_info_ptr->real_first_name,tmp_dest_buf,
            NTCSS_SIZE_USERFIRSTNAME);

    while ( (*src_ptr != '\0') && (isspace(*src_ptr)) )
        src_ptr++;
                                       // Middle name
                                       //
    if (*src_ptr == '\0')
    {
        strncpy(user_info_ptr->real_last_name,
                user_info_ptr->real_first_name,
                NTCSS_SIZE_USERLASTNAME);

        *user_info_ptr->real_first_name = '\0';
        *user_info_ptr->real_middle_name = '\0';
        return;
    }

    dest_ptr = tmp_dest_buf;
    while ( (*src_ptr != '\0') && (!isspace(*src_ptr)) )
        *dest_ptr++ = *src_ptr++;

    *dest_ptr = '\0';
    strncpy(user_info_ptr->real_middle_name,tmp_dest_buf,
            NTCSS_SIZE_USERMIDDLENAME);

    while ( (*src_ptr != '\0') && (isspace(*src_ptr)) )
        src_ptr++;

    if (*src_ptr == '\0')
    {
        strncpy(user_info_ptr->real_last_name,
                user_info_ptr->real_middle_name,
                NTCSS_SIZE_USERLASTNAME);

        *user_info_ptr->real_middle_name = '\0';
        return;
    }
                                       // Last name
                                       //
    dest_ptr = tmp_dest_buf;
    while ( (*src_ptr != '\0') && (!isspace(*src_ptr)) )
        *dest_ptr++ = *src_ptr++;

    *dest_ptr = '\0';
    strncpy(user_info_ptr->real_last_name,tmp_dest_buf,
            NTCSS_SIZE_USERLASTNAME);
}



/////////////////////////////////////////////////////////////////////////////
// makeSpoolFileName 
// _________________
//
// Generates a temporary spool file name using spool directory info
// and spool file sequence number data in the ini file.

void makeSpoolFileName(char *spool_file_name_buf)
{
   /* int seq_num = updatedNtcssIniNumValue(NTCSSINICODE_LPR_SECTION_CODE,
                                          NTCSSINICODE_SPOOLFILENO_CODE,
                                          NTCSSVAL_MAX_LPR_SPOOLFILENUM);*/

    char ntcss_ini_file[SIZE_SMALL_GENBUF];


    ::GetWindowsDirectory(ntcss_ini_file,SIZE_SMALL_GENBUF);

    strcat(ntcss_ini_file,NTCSS_INI_FILE_NM);
    
    char tmpbuf[SIZE_STANDARD_GENBUF];

    GetPrivateProfileString(NTCSSINICODE_MAIN_NTCSS_SECTION_CODE,
                            NTCSSINICODE_SPOOL_DIR_CODE,
                            NTCSSINICODE_SPOOL_DEFAULT_DIR_CODE,
                            tmpbuf,
                            sizeof(tmpbuf),
                            ntcss_ini_file);

	if( (_access( tmpbuf, 0 )) == -1 )
		_mkdir(tmpbuf);
	
    sprintf(spool_file_name_buf,"%s\\ntcss.prt",
            tmpbuf);

    return;
}



/////////////////////////////////////////////////////////////////////////////
// SockerrToString 
// _________________
//
// This was taken from old sockutil.cpp along with these comments:
//
//    SYNOPSIS:   Maps a socket error (like WSAEINTR) to a displayable
//                form (like "Interrupted system call").
//
//    ENTRY:      serr - The error to map.
//
//    RETURNS:    LPSTR - The displayable form of the error.  Will be
//                    "Unknown" for unknown errors.


extern "C" LPSTR SockerrToString( int serr )
{
    switch( serr )
    {
    case WSAENAMETOOLONG :
        return "Name too long";

    case WSANOTINITIALISED :
        return "Not initialized";

    case WSASYSNOTREADY :
        return "System not ready";

    case WSAVERNOTSUPPORTED :
        return "Version is not supported";

    case WSAESHUTDOWN :
        return "Can't send after socket shutdown";

    case WSAEINTR :
        return "Interrupted system call";

    case WSAHOST_NOT_FOUND :
        return "Host not found";

    case WSATRY_AGAIN :
        return "Try again";

    case WSANO_RECOVERY :
        return "Non-recoverable error";

    case WSANO_DATA :
        return "No data record available";

    case WSAEBADF :
        return "Bad file number";

    case WSAEWOULDBLOCK :
        return "Operation would block";

    case WSAEINPROGRESS :
        return "Operation now in progress";

    case WSAEALREADY :
        return "Operation already in progress";

    case WSAEFAULT :
        return "Bad address";

    case WSAEDESTADDRREQ :
        return "Destination address required";

    case WSAEMSGSIZE :
        return "Message too long";

    case WSAEPFNOSUPPORT :
        return "Protocol family not supported";

    case WSAENOTEMPTY :
        return "Directory not empty";

    case WSAEPROCLIM :
        return "EPROCLIM returned";

    case WSAEUSERS :
        return "EUSERS returned";

    case WSAEDQUOT :
        return "Disk quota exceeded";

    case WSAESTALE :
        return "ESTALE returned";

    case WSAEINVAL :
        return "Invalid argument";

    case WSAEMFILE :
        return "Too many open files";

    case WSAEACCES :
        return "Access denied";

    case WSAELOOP :
        return "Too many levels of symbolic links";

    case WSAEREMOTE :
        return "The object is remote";

    case WSAENOTSOCK :
        return "Socket operation on non-socket";

    case WSAEADDRNOTAVAIL :
        return "Can't assign requested address";

    case WSAEADDRINUSE :
        return "Address already in use";

    case WSAEAFNOSUPPORT :
        return "Address family not supported by protocol family";

    case WSAESOCKTNOSUPPORT :
        return "Socket type not supported";

    case WSAEPROTONOSUPPORT :
        return "Protocol not supported";

    case WSAENOBUFS :
        return "No buffer space is supported";

    case WSAETIMEDOUT :
        return "Connection timed out";

    case WSAEISCONN :
        return "Socket is already connected";

    case WSAENOTCONN :
        return "Socket is not connected";

    case WSAENOPROTOOPT :
        return "Bad protocol option";

    case WSAECONNRESET :
        return "Connection reset by peer";

    case WSAECONNABORTED :
        return "Software caused connection abort";

    case WSAENETDOWN :
        return "Network is down";

    case WSAENETRESET :
        return "Network was reset";

    case WSAECONNREFUSED :
        return "Connection refused";

    case WSAEHOSTDOWN :
        return "Host is down";

    case WSAEHOSTUNREACH :
        return "Host is unreachable";

    case WSAEPROTOTYPE :
        return "Protocol is wrong type for socket";

    case WSAEOPNOTSUPP :
        return "Operation not supported on socket";

    case WSAENETUNREACH :
        return "ICMP network unreachable";

    case WSAETOOMANYREFS :
        return "Too many references";

    default :
        return "Unknown";
    }
}



/////////////////////////////////////////////////////////////////////////////
// updatedNtcssIniNumValue 
// _______________________
//
// Uses given section and key values to locate integer value in NTCSS
// ini file - bumps the value up and writes it back to the ini file
// before returning the value to the caller.

int updatedNtcssIniNumValue(char *section_str,
                            char *key_str,
                            int  max_val)
{
    char ntcss_ini_file[SIZE_SMALL_GENBUF];

    ::GetWindowsDirectory(ntcss_ini_file,SIZE_SMALL_GENBUF);

    strcat(ntcss_ini_file,NTCSS_INI_FILE_NM);
    
    int value = ::GetPrivateProfileInt (section_str, 
                                        key_str, 
                                        1, 
                                        ntcss_ini_file);
    value++;
    if (value > max_val)
       value = 1;
    
    char num_str[SIZE_SMALL_GENBUF];   
    sprintf(num_str,"%03d",value);

    ::WritePrivateProfileString(section_str,
                                key_str, 
                                num_str,
                                ntcss_ini_file);
    return(value);
}



/////////////////////////////////////////////////////////////////////////////
// validateCommonDataTag
// _____________________
//
// Eric wanted validation done on given common data tags before they
// are sent over to the server.

BOOL validateCommonDataTag(char *tag, Csession *session)
{
    BOOL ok = TRUE;

    if (strncmp(tag,NTCSS_RESERVED_COMMON_TAG_LEAD,
                sizeof(NTCSS_RESERVED_COMMON_TAG_LEAD)) == 0)

        ok = FALSE;

    char *ptr = tag;

    while ((ok) && (*ptr))
    {
        if (!isalnum(*ptr) && (*ptr != '_')
                           && (*ptr != '-')
                           && (*ptr != ',')
                           && (*ptr != '.')
                           && (*ptr != ' '))
            ok = FALSE;
        else
            ptr++;
    }

    if (!ok)
    {
        session->ssnErrorMan()->RecordError("Invalid tag value");
    }

    return(ok);
}

NtcssProcessFilter* CreateBlankFilterRecord(const BOOL get_scheduled_jobs,
		 const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only)
{
	NtcssProcessFilter  *t=new NtcssProcessFilter;
	t->get_scheduled_jobs=get_scheduled_jobs;
	t->get_unscheduled_jobs=get_unscheduled_jobs;
	t->get_only_running_jobs=get_only_running_jobs;
	t->get_for_current_user_only=get_for_current_user_only;
	t->priority_flag=FALSE;
	t->command_line_flag=FALSE;
	t->cust_proc_status_flag=FALSE;
	t->login_name_flag=FALSE;
	t->unix_owner_flag=FALSE;
	t->progroup_title_flag=FALSE;
	t->job_descrip_flag=FALSE;
	t->orig_host_flag=FALSE;
	t->priority=0;
	t->command_line[0]=NULL;
	t->cust_proc_status[0]=NULL;
	t->login_name[0]=NULL;
	t->unix_owner[0]=NULL;
	t->progroup_title[0]=NULL;
	t->job_descrip[0]=NULL;
	t->orig_host[0]=NULL;
	return t;
}




