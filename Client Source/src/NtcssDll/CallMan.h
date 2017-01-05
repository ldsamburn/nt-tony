/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// CallMan.h                    
//                                         
// This is the header file for the CdllCallManager.
// 
//----------------------------------------------------------------------


                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_CALL_MGR_H_CLASS_
#define _INCLUDE_CALL_MGR_H_CLASS_
        
#include <ntcssapi.h>
#include <msgcore.h>

#include <dbg_defs.h>

class Csession;


class CdllCallManager // : public CObject
{
// Constructors

public:
    CdllCallManager() {}; 

// Attributes

// Operations

// Overrides

// Implementation - destructor and internal functions

public:

    ~CdllCallManager() {};

    BOOL validateBufSize(const int actual_size, const int required_size,
                         Csession *session);

          
// Implementation - User Application API's 

public:
    void  NtcssAPI_Assert
		(const BOOL assert_expr_val,
         const char FAR *message_text);

    BOOL  NtcssAPI_CreateSchedule
		(LPTSTR schedule_buf,
         const int schedule_buf_size);

    BOOL  NtcssAPI_DeregisterSecurity
		(const HANDLE hInstance);

    BOOL  NtcssAPI_DisplaySchedule  
		(LPCTSTR schedule_str);

    BOOL  NtcssAPI_DLLInitialize
		(LPCTSTR dll_version_str,
		 LPCTSTR app_name_str,
		 const HANDLE hInstance,
	     const HANDLE hWnd);


    BOOL  NtcssAPI_EditSchedule 
		(const char FAR  *orig_schedule_str,
         char FAR  *new_schedule_buf,
         const int new_schedule_buf_size);

    void NtcssAPI_ErrorMessagingOFF ();

    void NtcssAPI_ErrorMessagingON ();

    BOOL NtcssAPI_ErrorMessagingStatus ();

    BOOL NtcssAPI_GetAppData
		(LPTSTR app_data_buf,
         const int app_data_buf_size);

    BOOL NtcssAPI_GetAppName 
		(LPTSTR app_name_buf,
		 const int   app_name_buf_size);

    BOOL NtcssAPI_GetAppPassword 
		(LPTSTR app_name_buf,
		 const int   app_name_buf_size);

    BOOL NtcssAPI_GetAppUserInfo
		(LPCTSTR user_name_str,
		 NtcssUserInfo        *user_info_rec_ptr,
         LPTSTR               prog_access_info_buf,
         const int            prog_access_info_buf_size);

    int  NtcssAPI_GetAppUserList
		(NtcssAppUserList *user_list_recs_ptr,
         const int user_list_recs_cnt);
           
	int  NtcssAPI_GetAppUserListCt ();
		
    BOOL NtcssAPI_GetCommonData 
		(LPCTSTR tag_str,
         LPTSTR common_data_buf,
         const int common_data_buf_size);

    BOOL NtcssAPI_GetDateTime
		(LPTSTR datetime_buf,
         const int datetime_buf_size);

	BOOL NtcssAPI_GetFile
		(LPCTSTR ServerName,
		 LPCTSTR ServerFileName,
		 LPCTSTR ClientFileName,
		 const BOOL Ascii);

	BOOL NtcssAPI_GetFileInfo
		(LPTSTR HostName,
		 LPTSTR ServerFileName,
		 int  *LinesRequested,
		 int  *BytesRequired);

	int NtcssAPI_GetFileSection
		(LPTSTR HostName,
		 LPTSTR ServerFileName,
		 LPTSTR buf,
		 int  FirstLine,
		 int  LastLine);

    BOOL NtcssAPI_GetHostName 
		(LPTSTR host_name_buf,
         const int host_name_buf_size);

    BOOL NtcssAPI_GetLastError
		(LPTSTR err_buf,
         const int err_buf_size);

	BOOL NtcssAPI_GetLinkData 
		(LPTSTR link_data_buf,
		 const int   link_data_buf_size);

    BOOL NtcssAPI_GetLocalIP
		(LPTSTR ip_addr_buf,
         const int ip_addr_size);

    BOOL NtcssAPI_GetServerName 
		(LPTSTR server_name_buf,
         const int server_name_buf_size);

	int NtcssAPI_GetServerProcessList
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only,
		 NtcssProcessFilter *filter,
         NtcssServerProcessDetails *list_buf,
         const int list_buf_slot_cnt);

	int NtcssAPI_GetServerProcessListCt
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only,
		 NtcssProcessFilter *filter);
         

    BOOL NtcssAPI_GetUserInfo 
		(NtcssUserInfo *user_info_ptr);

    BOOL NtcssAPI_PostAppMessage 
		(const char FAR *msg_title,
         const char FAR *msg_str);

    BOOL NtcssAPI_PostMessage 
		(LPCTSTR msg_board_str,
		 LPCTSTR msg_title,
         LPCTSTR msg_str);

    BOOL NtcssAPI_PostSystemMessage 
		(LPCTSTR msg_title,
         LPCTSTR msg_str);

    BOOL NtcssAPI_PrintClientFile
		(const CString& file_name_str,
         const int  seclvl, 
         const CString& output_type_str);

    BOOL NtcssAPI_PrintServerFile 
		(LPCTSTR server_name_str,
         const int    seclvl, 
         LPCTSTR output_type_str,
         LPCTSTR dir_name_str,
		 const int   ncopies,
		 LPCTSTR options);

	BOOL NtcssAPI_PutFile
		(LPCTSTR ServerName,
		 LPCTSTR ServerFileName,
		 LPCTSTR ClientFileName,
		 const BOOL Ascii,BOOL append=FALSE);


    BOOL NtcssAPI_RegisterAppUser 
		(LPCTSTR user_name_str,
         const BOOL  do_register);


    BOOL NtcssAPI_RegisterSecurityClass
		(const HANDLE hInstance,
         const int seclvl);

    BOOL NtcssAPI_RemoveSchedServerProcess
		(LPCTSTR pid_buf,
         const BOOL scheduled_job);

    BOOL NtcssAPI_RestorePrinter (void);

    unsigned long NtcssAPI_ScheduleServerProcess 
		(LPCTSTR schedule_str,
		 NtcssServerProcessDescr *descr_ptr,
         LPTSTR pid_buf,
         const int pid_buf_size);

    HDC  NtcssAPI_SelectPrinter 
		(const char         *output_type_str,
         const int           seclvl,
         const BOOL          default_flag,
         const BOOL          gen_access_flag,
         const BOOL          return_DC_flag,
         const NtcssPrintAttrib  *pr_attr_ptr,
         const BOOL          local_enable_flag,
         const BOOL          remote_enable_flag);

	int NtcssSelectServerFile(CString lpszServer,CString lpszDirectory,
						  int buffSize, LPTSTR lpszFileName);

    BOOL NtcssAPI_SetAppData 
		(LPCTSTR app_data_str);

	BOOL NtcssAPI_SetLinkData 
		(LPCTSTR link_data_str);

    BOOL NtcssAPI_SpoolPrintRequest(void);

    BOOL NtcssAPI_StartServerProcess
		(NtcssServerProcessDescr* descr_ptr,
         LPTSTR pid_buf,
         const int pid_buf_size);
                                     

    BOOL NtcssAPI_StopServerProcess 
		(LPCTSTR pid_str,
         const int signal_type);


	BOOL NtcssAPI_ValidateUser(char *ssn_buf,int ssn_buf_size);

	void NtcssAPI_ViewServerFile(CString ServerName,CString Directory);
	

// Implementation - NTCSS DeskTop API's

public:
    int   NtcssPRV_ClearLRS();

	BOOL  NtcssPRV_PortDelete();

	void  NtcssPRV_IsPrintProcessAlive();

    int   NtcssPRV_DTInitialize(char *version);

    int   NtcssPRV_DTReset(char *version);

    int   NtcssPRV_GetToken(char *token);

    int   NtcssPRV_SetLRS(void *ptr_from_desktop);

	void  NtcssPRV_UpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,
													 const DWORD nNewProcessID,
											         const DWORD nNewThreadID);

    BOOL NtcssPRV_GetHostNameList(LPTSTR pList, int nSize);


// Implementation - NTCSS Secban API's

public:

    int  NtcssPRV_CurrentSecurityLevel();

    int  NtcssPRV_ReserveSessionTableSpace();


    BOOL NtcssPRV_SecBanInitialize(char *version);


// Implementation - NTCSS Applicaction API's

public:

	BOOL NtcssPRV_AdminLockOptions(const char *appname,
		 const char *code,const char *action, char *pid,const char *hostname);

    BOOL NtcssPRV_GetDllVersion(char *version_num);

	BOOL NtcssPRV_GetNTDomainName(char* DomainName,LPDWORD cchDomainName);

#ifdef _NTCSS_MSG_TRACING_
    BOOL NtcssPRV_DoMessage(SDllMsgRec *msg,
    						BOOL trace_this_msg);

#else
    BOOL NtcssPRV_DoMessage(SDllMsgRec *msg);

#endif

    BOOL NtcssPRV_DoMessageCleanup(const SDllMsgRec *msg);

    BOOL NtcssPRV_GetAppHost
						(LPCTSTR app_name_str,
						 LPTSTR host_buf,
						 int host_buf_size);

    BOOL NtcssPRV_GetAppNameList (char *app_list, int size);

    BOOL NtcssPRV_GetAppRole(LPCTSTR app_name);

	BOOL NtcssPRV_GetMasterName(char* strMasterName);

	BOOL NtcssPRV_IsAuthServer(const char *lpszGroupTitle);

	BOOL NtcssPRV_IsAdmin();

	int NtcssPRV_GetNumTasks();

	BOOL NtcssPRV_GetLoginName(LPSTR lpszLoginName);

    BOOL NtcssPRV_GetUserRole (void);

	BOOL NtcssPRV_GetAuthenticationServer(const char *lpszApp,
		char *lpszHost);

	void NtcssPRV_KillProcesses();

    BOOL NtcssPRV_SetCommonData (LPTSTR lpszTag, LPTSTR lpszData);

	BOOL NtcssPRV_StartServerProcessEx
		(LPCTSTR app_name_str,
		 NtcssServerProcessDescr* descr_ptr,
         LPTSTR pid_buf,
         const int pid_buf_size);

	unsigned long NtcssPRV_ScheduleServerProcessEx
		(const char  *app_name_str,
		 const char *schedule_str,
		 NtcssServerProcessDescr FAR *descr_ptr,
         char  *pid_buf,
         const int pid_buf_size);

// Implementation - NTCSS Thunker API's

public:

    BOOL   NtcssPRV_ThunkerInitialize(char *version);
};

#endif // _INCLUDE_CALL_MGR_H_CLASS_





