/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\comn_src\include\privapis.h                    
//
// WSP, last update: 9/10/96
//                                        
// This header file contains prototypes for private APIs provided
// by the NTCSS DLL.
//
// WSPTODO - Need to go through here (and corresponding entries.cpp and
//           call_mgr.cpp calls) and 'upgrade' as was done with the public
//           calls.
//----------------------------------------------------------------------
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INCLUDE_PRIVAPIS_H_CLASS_
#define _INCLUDE_PRIVAPIS_H_CLASS_

#include "ntcssapi.h"
#include "ntcssdef.h"
#include "msgcore.h"

typedef struct NtcssAppList
{
    char app_name[SIZE_GROUP_TITLE];
    char host_name[SIZE_HOST_NAME];
} NtcssAppList;

//application codes for locks
const CString PUI_Lock=_T("00");
const CString PDJ_Lock=_T("01");
const CString APC_Lock=_T("02");

const CString APCAPP_Lock=_T("03");
const CString SYSMON_Lock=_T("04");
const CString USRADM_Lock=_T("05");

//action codes for locks
const CString NewLock=_T("00");
const CString RenewLock=_T("01");
const CString RemoveLock=_T("02");


extern "C" int DllImport  Ntcss_SetLRS (void  *desktop_lrs_data);                                

extern "C" int DllImport  NtcssCurrentSecurityLevel();

#ifdef _NTCSS_MSG_TRACING_


extern "C" int DllImport  NtcssDoMessage(SDllMsgRec *msg,
										 BOOL trace_this_msg);

#else

extern "C" int DllImport  NtcssDoMessage(SDllMsgRec *msg);

#endif // _NTCSS_MSG_TRACING_


extern "C" BOOL DllImport NtcssAdminLockOptions
				   (const char  *app_name_str,
					const char  *code,
					const char  *action,
					char  *pid,const char *hostname);

extern "C" BOOL DllImport  NtcssDoMessageCleanup(const SDllMsgRec *msg);

extern "C" BOOL DllImport NtcssFreeSessionTableSpace(int index);

extern "C" int DllImport  NtcssDTInitialize(char *version);

extern "C" int DllImport Ntcss_ClearLRS();




extern "C" BOOL DllImport NtcssGetAppHost
									(const char  *app_name_str,
									 char  *host_buf,
									 int host_buf_size);

extern "C" BOOL DllImport NtcssGetAppNameList(char   *app_list_buf,
                                               int    app_list_buf_size);

extern "C" int DllImport  NtcssGetAppRole(LPCTSTR lpszAppName);

extern "C" BOOL DllExport NtcssGetAuthServer(char* lpszPAS);

extern "C" BOOL DllExport NtcssGetAuthenticationServer(const char* lpszApp, 
													   char* lpszHost);

extern "C" BOOL DllImport NtcssGetNTDomainName(char* DomainName,LPDWORD cchDomainName);

extern "C" int DllImport  NtcssGetDllVersion(char  *szversion);

extern "C" BOOL  DllImport NtcssLoginName(LPSTR lpszLoginName);

extern "C" int DllImport  NtcssGetNumTasks(void);

extern "C" int DllImport  NtcssGetToken(char  *token);

extern "C" int DllImport  NtcssGetUserRole(void);

extern "C" void DllImport NtcssIsPrintProcessAlive();

extern "C" void DllExport NtcssKillProcesses();

extern "C" BOOL DllImport NtcssPortDelete();

extern "C" int DllImport  NtcssRegisterSession(HANDLE hInstance,
                                               SecurityLevel security);

extern "C" int DllImport  NtcssReserveSessionTableSpace();

extern "C" BOOL DllImport NtcssSecBanInitialize(const char  *version);

extern "C" BOOL DllExport NtcssSetAuthServer(const char *lpszAuthServer);

extern "C" BOOL DllImport NtcssSetCommonData
                                      (char  *lpszTag, char  *lpszData);


extern "C" BOOL DllImport NtcssScheduleServerProcessEx
				   (const char *app_name_str,
				    const char *schedule_str,
					NtcssServerProcessDescr  *descr_ptr,
					char  *pid_buf,
					const int pid_buf_size);

extern "C" BOOL DllImport NtcssStartServerProcessEx
				   (const char *app_name_str,
					NtcssServerProcessDescr *descr_ptr,
					char  *pid_buf,
					const int pid_buf_size);

extern "C" BOOL DllImport NtcssGetMasterName(LPTSTR strMasterName);

extern "C" BOOL DllExport NtcssIsAuthServer(LPCTSTR lpszGroupTitle);

extern "C" BOOL DllExport NtcssIsAdmin();

extern "C" BOOL DllExport NtcssGetAppPath(LPCTSTR strAppName,LPTSTR strPath,DWORD dwLen);

extern "C" BOOL DllExport NtcssEncrypt(LPCTSTR lpcszKey,LPCTSTR lpcszString, LPTSTR lpszResult);

extern "C" BOOL DllExport NtcssDecrypt(LPCTSTR lpcszKey,LPCTSTR lpcszString, LPTSTR lpszResult);

extern "C" void DllExport NtcssUpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,
													 const DWORD nNewProcessID,const DWORD nNewThreadID);

extern "C" BOOL DllExport NtcssGetHostNameList(LPTSTR pList, int nSize);

#endif // _INCLUDE_PRIVAPIS_H_CLASS_




