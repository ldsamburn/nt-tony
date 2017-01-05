/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// CallMan.cpp
//                                         
// This is the implementation file for CdllCallManager.
//
//----------------------------------------------------------------------


#include "stdafx.h"
#include "0_basics.h"

#include "CallMan.h"
#include "dllutils.h"
#include "PrtSelectDlg.h"
#include "MsgDlvMan.h"
#include "dll_msgs.h"
#include "PrtSFileDlg.h"
#include "PrtCFileDlg.h"
#include "PrtSpoolMan.h"
#include "PrtDriver.h"   //TR10092
#include "schedule.h"
#include "ScheduleDlg.h"
#include "ntcssapi.h"
#include "inri_ftp.h"
#include "PrintUtil.h"
#include "ValidateUserDialog.h"
#include "ViewSFileDlg.h"
#include "AtlassFileDlg.h"
#include "io.h"
#include "ftpdlg.h"
#include "lmaccess.h"
#include "lm.h"
#include "afxconv.h"
#include "encrypt.h"
#include "GetServerFilename.h"
#include "ERRNO.H"

//***************************************************************************
// CdllCallManager Internal functions 
//***************************************************************************

BOOL  CdllCallManager::validateBufSize
                    (const int actual_size, const int required_size,
                     Csession *session)
{
    if (actual_size >= required_size)
        return(TRUE);
                                                                         
    else
    {
        session->ssnErrorMan()->RecordError
                        ("Inadequate buffer size parameter");
        return(FALSE);
    }
}


//***************************************************************************
// CdllCallManager User Application API's 
//***************************************************************************




/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_Assert
// _______________

void CdllCallManager::NtcssAPI_Assert 
		(const BOOL assert_expr_val,
         const char FAR *message_text)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return;
    }

    if (!assert_expr_val)
    {
        CString msg = gbl_dll_man.dllFacilityName() +
                      session->ssnAppName() + 
                      " Application Assert Failure -->\n    " +
                      message_text +
                      "\n\nContinue processing anyway?";
        
        int answer = AfxMessageBox (msg, MB_YESNO);

        if (answer == IDNO)
        {
            AfxMessageBox("Application shutting down...",MB_OK);
            abort();    
        }
    }

    gbl_dll_man.CMcall_releaseSession(session);
    return;
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_CreateSchedule
// _______________________

BOOL CdllCallManager::NtcssAPI_CreateSchedule 
(LPTSTR schedule_buf,
 const int schedule_buf_size)
{
	
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(schedule_buf_size, 
			NTCSS_MAX_SCHEDULE_LEN, session);
		if (ok)
		{
			CScheduleDlg sched_dlg;
			
			ok = (sched_dlg.DoModal() == IDOK);
			
			if (ok)
				
				sched_dlg.TheSchedule().GetRawSchedule(schedule_buf);
			
			else
				schedule_buf[0] = '\0';
		}
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
	
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_DeregisterSecurity
// ___________________________

BOOL CdllCallManager::NtcssAPI_DeregisterSecurity
		(const HANDLE hInstance)
{
// NEEDTODO - 
//      for now just ignoring if is from Desktop, do better?

    if (gbl_dll_man.thisIsDeskTopDll())
        return(TRUE);

    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL ok = gbl_dll_man.CMcall_setSessionSecLvl(session,NTCSS_UNCLASSIFIED);

    if (!ok)
        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);
    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_DisplaySchedule
// ________________________

BOOL CdllCallManager::NtcssAPI_DisplaySchedule (LPCTSTR schedule_str)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		CSchedule the_sched(schedule_str);
		
		BOOL ok = TRUE;
		if (the_sched.IsValid())
		{
			CScheduleDlg sched_dlg(&the_sched, TRUE);
			
			sched_dlg.DoModal();
		}
		else
		{
			ok = FALSE;
			char err_buf[MAX_ERROR_MSG_LEN];
			
			the_sched.GetErrorMsg(err_buf);
			session->ssnErrorMan()->RecordError(err_buf);
			
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
				session->ssnAppName());
			
		}
		
		gbl_dll_man.CMcall_releaseSession(session);
		return(ok);
		
	}
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
        return(FALSE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_DLLInitialize
// ______________________
//
// Left AfxMessageBox's in but commented as this routine is a frequent 
// starting point for debugging.
//

BOOL CdllCallManager::NtcssAPI_DLLInitialize
(LPCTSTR dll_version_str,
 LPCTSTR app_name_str,
 const HANDLE hInstance,
 const HANDLE hWnd)
{
	Csession *session;
	try
	{
		
		gbl_dll_man.dllErrorMan()->ClearError();
		
		BOOL ok = TRUE;
		int env_ok;
		
		char env_app_name_str[SIZE_GROUP_TITLE+1];
		
		if (!gbl_dll_man.thisIsDeskTopDll())
			ok = gbl_dll_man.CMcall_rawInitializations();
		
		
		// First make sure ready for app
		//  init and that this app is ok
		//
		
		env_ok=GetEnvironmentVariable("APPNAME",env_app_name_str,SIZE_GROUP_TITLE+1);
		
		if (env_ok == 0)
			strcpy(env_app_name_str,app_name_str);
		
		if (ok)
			ok = gbl_dll_man.validateAppOkToInit(dll_version_str,
			//  ACA: ATLASS CHANGE 9/21/98           app_name_str,
											 env_app_name_str,
                                             hInstance,hWnd);
		
		
		
		if (ok)
		{
			TRACE3(_T("Adding %s to Session table InstanceHandle=%d Hwnd=%d\n"),
				env_app_name_str,hInstance,hWnd);
			session = gbl_dll_man.CMcall_makeNewSession(env_app_name_str, //app_name_str,
				hInstance,hWnd,
				NTCSS_UNCLASSIFIED);
			ok = (session != NULL);
		}
		
		
		// If any problems encountered
		//  so far then handle error
		//  with gbl_dll_man, below
		//  errors will be handled on
		//  session level
		if (!ok)
		{
			//AfxMessageBox("Trying to handle error",MB_OK);
			
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		// If this is the desktop DLL and
		// if serverName indicates need
		// to broadcast then do so,
		// otherwise server name already
		// set in shared memory
		//
		
		if (ok)
			gbl_dll_man.CMcall_setInitializedOk();
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_EditSchedule
// _____________________

BOOL CdllCallManager::NtcssAPI_EditSchedule 
		(const char FAR  *orig_schedule_str,
         char FAR  *new_schedule_buf,
         const int new_schedule_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL did_it = FALSE;
    BOOL ok = validateBufSize(new_schedule_buf_size, 
                              NTCSS_MAX_SCHEDULE_LEN, session);
    if (ok)
    {
        CSchedule the_sched(orig_schedule_str);

        if (!the_sched.IsValid())
        {
            ok = FALSE;

            char err_buf[MAX_ERROR_MSG_LEN];

            the_sched.GetErrorMsg(err_buf);
            session->ssnErrorMan()->RecordError(err_buf);
        }

        new_schedule_buf[0] = '\0';

        if (ok)
        {
            CScheduleDlg sched_dlg(&the_sched);

            if (sched_dlg.DoModal() == IDOK)
            {
                did_it = TRUE;
                sched_dlg.TheSchedule().GetRawSchedule(new_schedule_buf);
            }
        }

    }

    if (!ok)
        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);

    return(did_it);
	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ErrorMessagingOFF
// __________________________

void CdllCallManager::NtcssAPI_ErrorMessagingOFF (void)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction(FALSE);
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return;
		}
		
		session->ssnErrorMan()->SetErrorHandling(FALSE);
		
		if (gbl_dll_man.thisIsDeskTopDll())
			gbl_dll_man.dllErrorMan()->SetErrorHandling(FALSE);
	}
	
	catch(...)
	{}
	
    gbl_dll_man.CMcall_releaseSession(session);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ErrorMessagingON
// _________________________

void CdllCallManager::NtcssAPI_ErrorMessagingON (void)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction(FALSE);
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return;
		}
		
		session->ssnErrorMan()->SetErrorHandling(TRUE);
		
		if (gbl_dll_man.thisIsDeskTopDll())
		{
			gbl_dll_man.dllErrorMan()->SetErrorHandling(TRUE);
		}
		
	}
	
	catch(...)
	{}
	
    gbl_dll_man.CMcall_releaseSession(session);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ErrorMessagingStatus
// _____________________________

BOOL CdllCallManager::NtcssAPI_ErrorMessagingStatus (void)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction(FALSE);
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError
				(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL errors_on = session->ssnErrorMan()->ErrorHandlingStatus();
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(errors_on);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetAppData
// ___________________

BOOL CdllCallManager::NtcssAPI_GetAppData 
(LPTSTR app_data_buf,
 const int app_data_buf_size)
 
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		
		if ( validateBufSize(app_data_buf_size, SIZE_APP_DATA, session)
			&& (app_data_buf == NULL))
			
			throw(TEXT_MSG_BAD_REQ_PARM);
		
		
		CmsgGETAPPDATA msg;
		
		if(!msg.Load(session->ssnAppName()))
			throw(_T("GETAPPDATA load failed"));
		
		if(!msg.DoItNow())
			throw(_T("GETAPPDATA message failed"));
		
		if(!msg.UnLoad(app_data_buf))
			throw(_T("GETAPPDATA unload failed"));
		
    }
	
	
	catch(LPTSTR pStr)
	{
		session->ssnErrorMan()->RecordError(pStr, MB_OK);
		session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
	
    gbl_dll_man.CMcall_releaseSession(session);
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppName
// _______________________

BOOL CdllCallManager::NtcssAPI_GetAppName 
(LPTSTR app_name_buf,
 const int app_name_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(app_name_buf_size, SIZE_GROUP_TITLE, 
			session);
		
		if ( (ok) && (app_name_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
			strncpy(app_name_buf, session->ssnAppName(), SIZE_GROUP_TITLE);
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		return(TRUE);
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetAppPassword
// _______________________

BOOL CdllCallManager::NtcssAPI_GetAppPassword 
		(LPTSTR password_buf,
         const int password_buf_size)

{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL ok = validateBufSize(password_buf_size,SIZE_APP_PASSWORD, session);

	if ( (ok) && (password_buf == NULL) )
	{
        session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
		ok = FALSE;
	}

    if (ok)									
    {
        const SappGroupRec_wFullData *group_rec_ptr;

        group_rec_ptr = gbl_dll_man.dllUser() //...
                                    ->appGroup(session->ssnAppName());
        strncpy(password_buf, 
                group_rec_ptr->app_password_buf,
                SIZE_PASSWORD);
//  As of ATLASS, app passwords will contain the shared password
//  dsk_svr will take care of making sure the data is placed in the LRS correctly
//        strncpy(password_buf, 
//                gbl_dll_man.dllUser()->userInfo()->shared_passwd_buf,
//                SIZE_APP_PASSWORD);
    }
    else
        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);
    return(TRUE);

	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetAppUserInfo
// _______________________

BOOL CdllCallManager::NtcssAPI_GetAppUserInfo
(LPCTSTR user_name_str,
 NtcssUserInfo*       user_info_rec_ptr,
 LPTSTR prog_access_info_buf,
 const int            prog_access_info_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	try {
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(prog_access_info_buf_size,
			SIZE_APP_DATA, session);
		
		if ( ((user_name_str == NULL) || (*user_name_str == '\0')) ||
			(user_info_rec_ptr == NULL) ||
			(prog_access_info_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}			
		
		CmsgGETAPPUSERINFO msg;
		
		if (ok)
			ok = msg.Load((LPCTSTR)session->ssnAppName(),
			user_name_str);
		
		if (ok)
			ok = msg.DoItNow();
		
		BOOL unused_reg_ptr;                  
		if (ok)
		{
			
			ok = msg.UnLoad(&unused_reg_ptr, user_info_rec_ptr, 
				prog_access_info_buf,(LPCTSTR)session->ssnAppName());
			
			
		}
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
        
		gbl_dll_man.CMcall_releaseSession(session);
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetAppUserList
// _______________________

int CdllCallManager::NtcssAPI_GetAppUserList 
(NtcssAppUserList *user_list_recs_ptr,
 const int user_list_recs_cnt)
{ 
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		if (user_list_recs_ptr == NULL)
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
		}			
		
		BOOL ok = TRUE;
		
		CmsgGETAPPUSERLIST msg;
		
		ok = msg.Load(session->ssnAppName());
		
		if (ok)
			ok = msg.DoItNow();
		
		int num_users = 0;
		if (ok)
			
			
			ok = msg.UnLoad(user_list_recs_cnt, &num_users, 
			user_list_recs_ptr);
		
		if (!ok)
		{
			num_users = 0;
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
				session->ssnAppName());
		}   
		
		gbl_dll_man.CMcall_releaseSession(session);
		return (num_users);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetAppUserListCt
// _______________________

int CdllCallManager::NtcssAPI_GetAppUserListCt ()

{ 
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(0);
		}
		
		
		BOOL ok = TRUE;
		
		CmsgGETAPPUSERLIST msg;
		
		ok = msg.Load(session->ssnAppName());
		
		if (ok) 
		{
			ok = msg.DoItNow();
			int num_users;
			if(!ok)
			{
				
				AfxMessageBox("GETAPPUSERLIST Message Failed");
				return 0;
			}
			
			msg.getUserCount(&num_users);
			CString temp;
			gbl_dll_man.CMcall_releaseSession(session);
			return(num_users > 0 ? num_users : 0);
		}
		else
		{
			gbl_dll_man.CMcall_releaseSession(session);
			return (0);
		}
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
        return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetCommonData
// ______________________

BOOL CdllCallManager::NtcssAPI_GetCommonData 
(LPCTSTR tag_str,
 LPTSTR common_data_buf,
 const int common_data_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(common_data_buf_size,SIZE_COMMON_DATA,
			session);
		
		if ( (ok) && (tag_str == NULL) || (*tag_str == NULL)
			|| (common_data_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		char lcl_tag_str[SIZE_COMMON_TAG+1];
		strncpy(lcl_tag_str,tag_str,SIZE_COMMON_TAG);
		
		if (ok)
		{
			trim(lcl_tag_str);
			
			if (!validateCommonDataTag(lcl_tag_str, session))
				ok = FALSE;
		}
		
		CmsgGETCOMMONDB msg;
		
		if (ok)
			ok = msg.Load(lcl_tag_str);
		
		if (ok)
			ok = msg.DoItNow();
		
		if (ok)
			ok = msg.UnLoad(common_data_buf);
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetDateTime
// ____________________

BOOL CdllCallManager::NtcssAPI_GetDateTime
(LPTSTR datetime_buf,
 const int datetime_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		
		BOOL ok = validateBufSize(datetime_buf_size,
			SIZE_DATETIME, session);
		
		if ( (ok) && (datetime_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		COleDateTime m_Time = COleDateTime::GetCurrentTime();
		CString strLine = m_Time.Format("%b %d %y %a %X");
		strcpy(datetime_buf,strLine);
		
		if (!ok)
            session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetFile
// ____________________


BOOL CdllCallManager::NtcssAPI_GetFile
(LPCTSTR ServerName,
 LPCTSTR ServerFileName,
 LPCTSTR ClientFileName,
 const BOOL Ascii)

{
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
    CString hack;
	TCHAR lpszTempFile[_MAX_PATH];
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		//change MS slashes to UNIX
		//BOOL ok=TRUE;
		TCHAR szServerFileName[_MAX_PATH];
		strcpy(szServerFileName,ServerFileName);
		char *p=szServerFileName;
		
		while (*p)
		{
			if (*p=='\\')
				*p='/';
			p++;
		}
		
		
		TCHAR strTempFileName[_MAX_PATH+1];
		
		
		
		//////////////////////////////////////////////////////////////
		
		TCHAR path_buffer[_MAX_PATH]; 
		TCHAR drive[_MAX_DRIVE];   
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		
		_splitpath(ClientFileName, drive, dir, fname, ext);
		
		
		if(::GetDriveType(drive)==DRIVE_REMOVABLE)
		{
			CWaitCursor csr;
			
			
			//TODO later need to get CmsgGETFILEINFO to do binary byte count
			//for now we copy the file & size
			
			
			::GetPrivateProfileString("NTCSS","NTCSS_SPOOL_DIR","!",path_buffer,_MAX_PATH,"ntcss.ini");
			if (strstr(path_buffer,"!"))
				throw("There is no NTCSS ini file!");
			
			
			if(!::GetTempFileName(path_buffer,"GEN",0,strTempFileName))
				throw("Temp file creation failed");
			
		
			CInriFtpMan ftp_man;
			
			if(!ftp_man.Initialize())
				throw("ftp initialize failed");
			
			if(!ftp_man.GetFile(ServerName,
				szServerFileName,
				strTempFileName,
				Ascii,0666))        //TRUE->Ascii
				throw(_T("temp ftp failed"));
			
			hack=szServerFileName;
			
			hack=hack.Right((hack.GetLength()-hack.ReverseFind('/'))-1);
			
			hack=(CString)path_buffer+"\\"+hack;
			
			
			MoveFile(strTempFileName,hack);
			
			
			if(hack.GetLength()==0)
				throw("Couldn't parse Unix FileName");
			
			
			CFtpDlg dlg(NULL,hack.GetBufferSetLength(hack.GetLength()),ClientFileName);
			if(dlg.DoModal()==IDCANCEL)
				throw("Zip Aborted");
			
			_unlink(hack);
			gbl_dll_man.CMcall_releaseSession(session);
			
			return TRUE;
			
		}   //if floppy   
		
		
		//////////////////////////////////////////////////////////////
		
		//make sure we can write to the directory

		/////////////
		

		TRACE1(_T("Checking FTP permissions on Directory <%s> \n"),dir);

		CString strDir;
		strDir.Format(_T("%s%s"),drive,dir);

		
		if(errno==EACCES || !::GetTempFileName(strDir,_T("NTCSS"),0,lpszTempFile))
		{
			_tunlink(lpszTempFile);
			throw(_T("Can't write to Target Dir... Check Permissions"));
		}

		TRACE1(_T("Checking FTP permissions on File<%s> \n"),lpszTempFile);
		_tunlink(lpszTempFile); 

		/////////////
		
		CInriFtpMan ftp_man;
		
		if(!ftp_man.Initialize())
			throw("ftp initialize failed");
		
		if(!ftp_man.GetFile(ServerName,
			szServerFileName,
			ClientFileName,
			Ascii,0666))        //TRUE->Ascii
			throw(_T("ftp failed"));
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return TRUE;
		
   	}  
	
	catch(LPTSTR pStr)
	{
		//ok=FALSE;
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(pStr,MB_ICONEXCLAMATION);
		_tunlink(hack);
		_tunlink(lpszTempFile);
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
	catch(...)
	{
		//ok=FALSE;
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Caught unknown exception"),MB_ICONEXCLAMATION);
		_tunlink(hack);
		_tunlink(lpszTempFile);
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	} 
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetFileInfo
// ____________________


BOOL CdllCallManager::NtcssAPI_GetFileInfo
		(LPTSTR HostName,
		 LPTSTR ServerFileName,
		 int  *LinesRequested,
		 int  *BytesRequired)
{
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

	
    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }
	CmsgGETFILEINFO msg;

    BOOL ok = msg.Load(HostName,ServerFileName);

    if (ok)
	   ok = msg.DoItNow();

	if (ok)
	   ok = msg.UnLoad(LinesRequested,BytesRequired);
	
   	gbl_dll_man.CMcall_releaseSession(session);

	return ok;

	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}

}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetFileSection
// ____________________


int CdllCallManager::NtcssAPI_GetFileSection
(LPTSTR HostName,
 LPTSTR ServerFileName,
 LPTSTR buf,
 int  FirstLine,
 int  LastLine)
{
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return 0;
		}
		
		int lines;
		
		CmsgGETFILESECTION msg;
		
		
		CString FL,LL;
		FL.Format("%d",FirstLine);
		LL.Format("%d",LastLine);
		
		
		BOOL ok = msg.Load(HostName,ServerFileName,FL.GetBufferSetLength(SIZE_CHAR_TO_INT)
			,LL.GetBufferSetLength(SIZE_CHAR_TO_INT));
		
		if (ok)
			ok = msg.DoItNow();
		
		
		if (ok)
			ok = msg.UnLoad(&lines,buf);
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok?lines:0);
	} 
	
	catch (...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetHostName
// ____________________

BOOL CdllCallManager::NtcssAPI_GetHostName
(LPTSTR host_name_buf,
 const int host_name_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(host_name_buf_size, SIZE_HOST_NAME,
			session);
		
		if ( (ok) && (host_name_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		if (ok)
		{
			strncpy(host_name_buf,((CNtcssDll*)AfxGetApp())->GetLocalHostName(),SIZE_HOST_NAME);
			ok=strlen(host_name_buf)>0;
		}
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
	}
	
	catch (...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetLastError
// ______________________

BOOL CdllCallManager::NtcssAPI_GetLastError
		(LPTSTR err_buf,
         const int err_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction(FALSE);

	try
	{

    if (session != NULL)
    {
		if (!validateBufSize(err_buf_size, NTCSS_MAX_ERROR_MSG_LEN,
                              session))
			return(FALSE);

		if (err_buf_size == NULL)
			return(FALSE);

        BOOL ok = session->ssnErrorMan()->GetLastErrorMsgText
                                            (err_buf, err_buf_size);

        gbl_dll_man.CMcall_releaseSession(session);

        return(ok);
    }

    return(gbl_dll_man.dllErrorMan()-> //...
                        GetLastErrorMsgText(err_buf, err_buf_size));
	}

	catch (...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetLinkData
// _______________________

BOOL CdllCallManager::NtcssAPI_GetLinkData 
(LPTSTR password_buf,
 const int password_buf_size)
 
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(password_buf_size,SIZE_APP_PASSWORD, session);
		
		if ( (ok) && (password_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)									
		{
			
			CmsgGETLINKDATA msg;
			
			BOOL ok=msg.Load((LPCTSTR)session->ssnAppName());
			
			if (ok)
				ok=msg.DoItNow();
			
			if(ok)
				ok=msg.UnLoad(password_buf);
			
		}
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetLocalIP
// ______________________

BOOL CdllCallManager::NtcssAPI_GetLocalIP
(LPTSTR ip_addr_buf,
 const int ip_addr_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(ip_addr_size, SIZE_IP_ADDR, session);
		
		if ( (ok) && (ip_addr_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
			
			strncpy(ip_addr_buf,((CNtcssDll*)AfxGetApp())->GetLocalHostIP(),ip_addr_size);
		ok=strlen(ip_addr_buf)>0;
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetServerName
// ______________________

BOOL CdllCallManager::NtcssAPI_GetServerName
(LPTSTR server_name_buf,
 const int server_name_buf_size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(server_name_buf_size, SIZE_HOST_NAME,
			session);
		
		if ( (ok) && (server_name_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
			strncpy(server_name_buf, gbl_dll_man.dllServerName(), 
			SIZE_HOST_NAME);
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
		
	}
	
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
	
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetServerProcessList
// _____________________________

int CdllCallManager::NtcssAPI_GetServerProcessList
(const BOOL get_scheduled_jobs,
 const BOOL get_unscheduled_jobs,
 const BOOL get_only_running_jobs,
 const BOOL get_for_current_user_only,
 NtcssProcessFilter *filter,
 NtcssServerProcessDetails *list_buf,
 const int list_buf_slot_cnt)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try 
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(0);
		}
		
		BOOL ok = TRUE;
		BOOL MadeNode=FALSE;
		
		
		int slots_filled = 0;
		NtcssServerProcessDetails *list_slot_ptr = list_buf;
		int list_buf_slots_filled = 0;
		
		
		if ( (ok) && (list_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if(filter==NULL)  //will do this even if they did filter version & passed in NULL
		{
			filter=CreateBlankFilterRecord(get_scheduled_jobs,get_unscheduled_jobs,
				get_only_running_jobs,get_for_current_user_only);
			MadeNode=TRUE;
		}
		
		if ((ok) && (get_scheduled_jobs))
		{
			CmsgSVRPROCLIST msg1;
			
			
			
			ok = msg1.Load(session->ssnAppName(),
				gbl_dll_man.dllUser()->userInfo()->user_role,
				TRUE,filter); // do_scheduled_jobs
			
			if (ok)
				ok = msg1.DoItNow();
			
			if (ok)
				
				
				ok = msg1.UnLoad(list_slot_ptr, 
				list_buf_slot_cnt,
				&slots_filled);
			
			
			if (ok)
			{
				list_buf_slots_filled = slots_filled;
				list_slot_ptr = list_buf + slots_filled;    
			}
		}
		
		if (ok && get_unscheduled_jobs)
		{
			CmsgSVRPROCLIST msg2;
			
			
			ok = msg2.Load(session->ssnAppName(),
				gbl_dll_man.dllUser()->userInfo()->user_role,
				FALSE,filter); // do_unscheduled_jobs
			
			if (ok)
				ok = msg2.DoItNow();
			
			if (ok)
				
				
				ok = msg2.UnLoad(list_slot_ptr, 
				(list_buf_slot_cnt - 
				list_buf_slots_filled),
				&slots_filled);
			
			if (ok)
			{
				list_buf_slots_filled += slots_filled;
			}
		}
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		if(MadeNode) delete filter;
		
		if (ok)
			return (list_buf_slots_filled);
		else
			return(0);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetServerProcessListCt
// _____________________________

int CdllCallManager::NtcssAPI_GetServerProcessListCt
(const BOOL get_scheduled_jobs,
 const BOOL get_unscheduled_jobs,
 const BOOL get_only_running_jobs,
 const BOOL get_for_current_user_only,
 NtcssProcessFilter *filter)
 
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(0);
		}
		
		BOOL ok = TRUE,MakeNode=FALSE;
		
		int total_cnt=0,count=0;
		
		if(filter==NULL)  //will do this even if they did filter version & passed in NULL
		{
			filter=CreateBlankFilterRecord(get_scheduled_jobs,get_unscheduled_jobs,
				get_only_running_jobs,get_for_current_user_only);
			MakeNode=TRUE;
		}
		
		
		if ((ok) && (get_scheduled_jobs))
		{
			CmsgSVRPROCLIST msg1;
			
			
			
			ok = msg1.Load(session->ssnAppName(),
				gbl_dll_man.dllUser()->userInfo()->user_role,
				TRUE,filter); // do_scheduled_jobs
			
			
			
			
			if (ok)
				ok = msg1.DoItNow();
			
			if (ok)
				
				if(ok = msg1.getProcessCnt(&count))
					total_cnt=count;
				
				
				
		}
		
		if (ok && get_unscheduled_jobs)
		{
			CmsgSVRPROCLIST msg2;
			
			
			
			ok = msg2.Load(session->ssnAppName(),
				gbl_dll_man.dllUser()->userInfo()->user_role,
				FALSE,filter); // do_scheduled_jobs
			
			
			if (ok)
				ok = msg2.DoItNow();
			
			if (ok)
				
				if(ok = msg2.getProcessCnt(&count))
					total_cnt+=count;
				
				
				
		}
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		
		if(MakeNode) delete filter;
		
		return(total_cnt);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_GetUserInfo
// ____________________

 BOOL CdllCallManager::NtcssAPI_GetUserInfo 
	 (NtcssUserInfo *user_info_ptr)
 {
	 Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	 
	 try
	 {
		 
		 if (session == NULL)
		 {
			 gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			 return(FALSE);
		 }
		 
		 if (user_info_ptr == NULL)
		 {
			 session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			 return(FALSE);
		 }
		 
		 const SuserInfoRec_wFullData *user = gbl_dll_man.dllUser()->userInfo();
		 
		 strncpy(user_info_ptr->login_name, user->login_name_buf,
			 SIZE_LOGIN_NAME);
		 
		 strncpy(user_info_ptr->ssn, user->ssn_buf, SIZE_SSN);
		 
		 strncpy(user_info_ptr->phone_number, user->phone_number_buf,
			 SIZE_PHONE_NUMBER);
		 
		 memset(user_info_ptr->app_passwd,NULL,sizeof(user_info_ptr->app_passwd));
		 strncpy(user_info_ptr->app_passwd, user->shared_passwd_buf,
			 SIZE_APP_PASSWORD);
		 
		 
		 user_info_ptr->security_class = (int)user->security_class;
		 
		 user_info_ptr->unix_id = (int)user->unix_id;
		 
		 nameFixup(user_info_ptr, user->real_name_buf);
		 
		 gbl_dll_man.CMcall_releaseSession(session);
		 
		 return(TRUE);
		 
	 }
	 
	 catch(...)
	 {
		 gbl_dll_man.CMcall_releaseSession(session);
		 return FALSE;
	 }
 }                                 



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PostAppMessage
// _______________________

#define USE_APP_NAME_CODE "USE_APP_NAME_CODE"

BOOL CdllCallManager::NtcssAPI_PostAppMessage 
		(const char FAR *msg_title,
         const char FAR *msg_str)
{
	return(NtcssAPI_PostMessage(USE_APP_NAME_CODE,
								msg_title,
								msg_str));
}                                 



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PostMessage
// _____________________

#define	MSG_ID_START_POS	17
#define MSG_ID_LEN			13

BOOL CdllCallManager::NtcssAPI_PostMessage 
(LPCTSTR msg_board_str,
 LPCTSTR msg_title,
 LPCTSTR msg_str)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if ( (ok) && (msg_board_str == NULL)  || (*msg_board_str == '\0') ||
			(msg_title == NULL)  || (*msg_title == '\0') ||
			(msg_str == NULL)  || (*msg_str == '\0') )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		
		char lcl_msg_board_buf[SIZE_GROUP_TITLE+5];
		
		if (strcmp(msg_board_str, USE_APP_NAME_CODE) == 0)
		{
			strcpy(lcl_msg_board_buf, session->ssnAppName());
			_strupr(lcl_msg_board_buf);
			
			strcat(lcl_msg_board_buf, ".all");
		}
		else
			strcpy(lcl_msg_board_buf, msg_board_str);
		
		char server_file[SIZE_SERVER_LOCATION];
		
		char message_id[SIZE_MSGBOARD_KEY];
		
		// Get name of server location
		//  to receive msg file
		if (ok)
		{
			CmsgGETNEWMSGFILE getnewmsgfile_msg;
			
			ok = getnewmsgfile_msg.Load();
			
			if (ok)
				ok = getnewmsgfile_msg.DoItNow();
			
			if (ok)
				ok = getnewmsgfile_msg.UnLoad(server_file);
			
			if (!ok) 
				session->ssnErrorMan()->RecordError
				("Message posting error (GETNEWMSGFILE)");
		}
		
		
		char lcl_file_name[SIZE_TINY_GENBUF];
		if (ok)
		{
			strcpy(message_id, &server_file[strlen(server_file)-MSG_ID_START_POS]);
			message_id[MSG_ID_LEN] = '\0';
			
			strncpy(&message_id[MSG_ID_LEN], (LPCTSTR)gbl_dll_man.dllUser()->loginName(), SIZE_LOGIN_NAME);
			message_id[SIZE_MSGBOARD_KEY - 1] = '\0';
			
			
			message_id[SIZE_MSGBOARD_KEY - 1] = '\0';	
			
			
									   // Open temp local disk file
			FILE *fp = NULL;
			
			getTempFileName(lcl_file_name, SIZE_TINY_GENBUF);
			
			
			ok = ((fp = fopen(lcl_file_name, "w")) != NULL);
			
									   // Write msg to local disk file
			if (ok)
			{
				UINT len = strlen(msg_str);
				ok = (fwrite(msg_str, 1, len, fp) == len);
				fclose(fp);
			}
			
			if (!ok)
				session->ssnErrorMan()->RecordError
				("Message posting error (file create)");
		}
		// Now FTP the file to the
		//  server
		
		if (ok)
		{
			CInriFtpMan ftp_man;
			
			ok = ftp_man.Initialize();
			
			if (ok)
				ok = ftp_man.PutFile(gbl_dll_man.dllServerName(),
				server_file,
				lcl_file_name,
				TRUE,
				0666);  // NEEDTODO - better protection?
			
			if (!ok)
			{
				session->ssnErrorMan()->RecordError
					("Message posting error (FTP)");
			}
		}
		// Now do CREATEMSGFILE to tell
		//  the server about the file
		//
		if (ok)
		{
			CmsgCREATEMSGFILE createmsgfile_msg;
			
			_unlink(lcl_file_name); //jgj 5_21
			
			ok = createmsgfile_msg.Load(message_id, msg_title);
			
			
			if (ok)
				ok = createmsgfile_msg.DoItNow();
			
			if (!ok)
			{
				session->ssnErrorMan()->RecordError
					("Message posting error (CREATEMSGFILE)");
			}
			
		} // endif for (!file_already_sent)
		
		if (ok)
		{
			CmsgSENDBBMSG sendbbmsg_msg;
			
			ok = sendbbmsg_msg.Load(message_id,
				lcl_msg_board_buf);
			
			if (ok)
				ok = sendbbmsg_msg.DoItNow();
			
			
			if (!ok)
			{
				session->ssnErrorMan()->RecordError
					("Message posting error (SENDBBMSG)");
			}
		}
		
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
		
	}
}                                 



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PostSystemMessage
// __________________________

BOOL CdllCallManager::NtcssAPI_PostSystemMessage 
		(LPCTSTR msg_title,
         LPCTSTR msg_str)
{
	return(NtcssAPI_PostMessage("NTCSS.all",
								msg_title,
								msg_str));
}                                 



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PrintClientFile
// ______________________

BOOL CdllCallManager::NtcssAPI_PrintClientFile (const CString& file_name_str,
												const int  seclvl, 
												const CString& output_type_str)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}                                
		
		//TODO this IF is stoping the old code from working 
		if ((file_name_str.IsEmpty() || output_type_str.IsEmpty() || 
			_access(file_name_str, 0 ) == -1 )) 
			throw(TEXT_MSG_BAD_REQ_PARM);
		
		if(!gbl_dll_man.CMcall_appCanDoPrintDlg())
			throw(_T("Printing resources are currently locked"));
		
		//////////////////////////////////////////////////////////////////
		if(!file_name_str.IsEmpty())    //TR10092 from here down
		{
			const SremotePrinterInfo *the_printer;
			//	const SlocalPrinterInfo *the_printer;//4/29/99 added local printers
			
			CprintDriver print_driver_man(session);
			CSelPrtDialog sel_prn_dlg(CWnd::FromHandle(session->ssnHWnd()),
				session);
			
			sel_prn_dlg.enableRemoteSelection
				(output_type_str,
				seclvl,
				FALSE,  // all printers, not just default
				FALSE); // but not general access printers
			
			//	sel_prn_dlg.enableLocalSelection(); //4/29/99 added local printers
			
			//	if((the_printer = sel_prn_dlg.getDefaultRemotePrinter()))//TR10124
			if(sel_prn_dlg.DoModal()==IDOK)
			{
				CString strAtlassFileName;
				
				//	if(sel_prn_dlg.selectedRemotePrinter()) //4/29/99 added local printers
				
				the_printer = sel_prn_dlg.getRemotePrinterSelection(); //only orig line
				
				//	else //4/29/99 added local printers
				
				//the_printer = sel_prn_dlg.getLocalPrinterSelection();
				
				//ATLASS STUFF
				if(!strcmp(the_printer->AtlassFileName,_T("***")))  
				{
					BOOL ok=TRUE;
					CAtlassFileDlg dlg;
					if(dlg.DoModal()==IDOK)
					{
						
						CInriFtpMan ftp_man;
						if (ftp_man.Initialize())
						{
							ftp_man.SetUnixID(0);
							if(ftp_man.PutFile(gbl_dll_man.dllServerName(),
								(LPCTSTR)dlg.m_strAtlassFname,
								file_name_str,
								FALSE,0666,dlg.m_nAppend)) //defaulting to binary
							{
								AfxMessageBox (_T("File successfully spooled to remote printer"),MB_OK);
								gbl_dll_man.CMcall_releaseSession(session);
								return TRUE;
							}
							else
								throw(_T("Cannot Ftp Client File"));
						}
					}
					else
						throw(_T("No Atlass File Name specified"));
					
				} //ATLASS STUFF
				//12/18/98
				//set hostname for print job=to the apps host
				//////////////////////////////////////////////////////
				
				CString strServer=gbl_dll_man.dllUser()->GetAppServer(session->ssnAppName());
				
				if(!strServer.IsEmpty())
					strcpy((char*)the_printer->host_name_buf,(LPCTSTR)strServer);
				else
					throw(_T("No Host File Specified... file not printed"));
				
				if(!print_driver_man.sendRawRemoteLPRjob(the_printer,
					file_name_str,
					seclvl)) 
					
					throw(_T("sendRawRemoteLPRjob... file not printed"));
				
				
			}     //IDOK
			
			else
				throw(_T("No Default Printer... file not printed"));
		}
		
		else  //TR10092 down to here all this code is added
		{     //all old code is in this ELSE block
			CPrtClientFileDlg prt_client_file_dlg(CWnd::FromHandle(session->ssnHWnd()), 
				session,
				file_name_str,
				seclvl, output_type_str);//1st original statement
			
			// Note that CPrtClientFileDlg does
			//  its own error handling
			//  
			return prt_client_file_dlg.DoModal() == IDOK; //2nd original statement
		}
		
		
		}
		
		catch (LPTSTR pStr)
		{
			session->ssnErrorMan()->RecordError(pStr, MB_OK);
		}
		
		catch(...)
		{
			session->ssnErrorMan()->RecordError(_T("Caught Unknown Exception"), MB_OK);
		}
		
		session->ssnErrorMan()->HandleError
			(gbl_dll_man.dllFacilityName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PrintServerFile
// ______________________

BOOL CdllCallManager::NtcssAPI_PrintServerFile 
(LPCTSTR server_name_str,
 const int    seclvl, 
 LPCTSTR output_type_str,
 LPCTSTR dir_name_str,
 const int    ncopies,
 LPCTSTR options)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}                                
		
		BOOL ok = TRUE;
		if ( ((server_name_str == NULL) || (*server_name_str == '\0')) ||
			((output_type_str == NULL) || (*output_type_str == '\0')) ||
			((dir_name_str == NULL) || (*dir_name_str == '\0')) ||
			((ncopies<=0) || (ncopies>99)))
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
		{
			ok = gbl_dll_man.CMcall_appCanDoPrintDlg();
			
			if (!ok)
				session->ssnErrorMan()->RecordError
				("Printing resources are currently locked",MB_OK);
		}
		
		if (ok)
		{
			CString copies;
			copies.Format("%d",ncopies);
			
			CPrtSvrFile prt_svr_file_dlg(CWnd::FromHandle(session->ssnHWnd()), 
				session,
				server_name_str, dir_name_str,   
				output_type_str,copies.GetBufferSetLength
				(NTCSS_MAX_PRT_COPIES_LEN),options);
			
			// Note that CPrtSvrFile does
			//  its own error handling
			//  
			ok = prt_svr_file_dlg.DoModal();                                 
			
		}
		
		else
			session->ssnErrorMan()->HandleError
			(gbl_dll_man.dllFacilityName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return(ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_RegisterAppUser
// ________________________

BOOL CdllCallManager::NtcssAPI_RegisterAppUser 
(LPCTSTR user_name_str,
 const BOOL  do_register)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if ( (ok) && (user_name_str == NULL) || 
			(*user_name_str == '\0') )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
		{
			CmsgREGISTERAPPUSER msg;
			
			ok = msg.Load(user_name_str, session->ssnAppName(), do_register);
			
			if (ok)
				ok = msg.DoItNow();
			
		}
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		return (ok);
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_RegisterSecurityClass
// ______________________________

BOOL CdllCallManager::NtcssAPI_RegisterSecurityClass
(const HANDLE hInstance,
 const int seclvl)
{
	// NEEDTODO for now just ignoring if is from Desktop, do better?
	
    if ((!gbl_dll_man.CMcall_initializedOK()) || 
        (gbl_dll_man.thisIsDeskTopDll()))
		
        return(FALSE);
	
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = gbl_dll_man.CMcall_setSessionSecLvl(session,seclvl);
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		
		return(ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_PutFile
// ____________________


BOOL CdllCallManager::NtcssAPI_PutFile
(LPCTSTR ServerName,
 LPCTSTR ServerFileName,
 LPCTSTR ClientFileName,
 const BOOL Ascii,BOOL append/*FALSE*/)
{
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	TCHAR strTempFileName[_MAX_PATH+1];
	TCHAR strTempServerFileName[_MAX_PATH+1];
	TCHAR strTempClientFileName[_MAX_PATH+1];
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		CInriFtpMan ftp_man;
		
		
		
		strcpy(strTempServerFileName,ServerFileName);
		strcpy(strTempClientFileName,ClientFileName);
		
		if(!ftp_man.Initialize())
			throw(_T("FTP initialize error"));
		
		//Get Unix ID from LRS & set in FTP struct so user can get set
		const SuserInfoRec_wFullData *user = gbl_dll_man.dllUser()->userInfo();
		char temp[_MAX_PATH];
		
		::GetWindowText(session->ssnHWnd(),temp,_MAX_PATH);
		
		//change MS slashes to UNIX
		char *p=strTempServerFileName;
		
		while (*p)
		{
			if (*p=='\\')
				*p='/';
			p++;
		}
		
		//if it's the Printer UI program leave owner as ROOT
		if(!strstr(temp,"PRINTER CONFIGURATION"))
			ftp_man.SetUnixID((int)user->unix_id);
		else
			ftp_man.SetUnixID(0);
		
		TCHAR path_buffer[_MAX_PATH]; 
		TCHAR drive[_MAX_DRIVE];   
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		
		_splitpath(strTempClientFileName, drive, dir, fname, ext);
		//test if client file is on removable media
		
		if(::GetDriveType(drive)==DRIVE_REMOVABLE)
		{
			CWaitCursor csr;
			
			CZipFile Zf;
			
			//then test if it's zipped
			if(Zf.IsZipFile(strTempClientFileName))
			{
				//if it is unzip to temp before ftp
				
				
				::GetPrivateProfileString("NTCSS","NTCSS_SPOOL_DIR","!",path_buffer,_MAX_PATH,"ntcss.ini");
				
				if (strstr(path_buffer,"!"))
					throw(_T("There is no NTCSS ini file!"));
				
				
				if(!::GetTempFileName(path_buffer,"GEN",0,strTempFileName))
					throw(_T("Temp file name creation failed"));
				
				//strcpy(strTempFileName,"C:/_ntcss/spool/");
				
				CFtpDlg dlg(NULL,strTempClientFileName,strTempFileName,FALSE);
				if(dlg.DoModal()==IDCANCEL)
					throw(_T("UnZip Aborted"));
				
				
				if(!ftp_man.PutFile(ServerName,
					strTempServerFileName,
					strTempFileName,
					Ascii,0666,append))  //TRUE->Ascii
					throw(_T("FTP Failed"));
				
				_unlink(strTempFileName);
				
				gbl_dll_man.CMcall_releaseSession(session);
				
				return TRUE;
			}
		}
		
		if(!ftp_man.PutFile(ServerName,
			ServerFileName,
			strTempClientFileName,
			Ascii,0666,append))  //TRUE->Ascii
			throw(_T("FTP Failed"));
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return TRUE;
	}
	
	catch(LPTSTR pStr)
	{
		//ok=FALSE;
		AfxMessageBox(pStr);
		_unlink(strTempFileName);
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
	catch(...)
	{
		//ok=FALSE;
		AfxMessageBox("Caught unknown exception");
		_unlink(strTempFileName);
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	} 
	
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_RemoveSchedServerProcess
// _________________________________

BOOL CdllCallManager::NtcssAPI_RemoveSchedServerProcess 
		(LPCTSTR pid_buf,
         const BOOL scheduled_job)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL ok = TRUE;

	if ( (pid_buf == NULL) || (*pid_buf == '\0') )
	{
        session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
		ok = FALSE;
	}

	if (ok)
	{
		CmsgREMOVESPQITEM msg;

	    ok = msg.Load(pid_buf, scheduled_job,session->ssnAppName());

		if (ok)
			if(!(ok = msg.DoItNow())) //TR10162
				session->ssnErrorMan()->RecordError(session->ssnErrorMan()
				->GetLastErrorMsgText(), MB_OK);
		
	}

    if (!ok)
        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);

    return (ok);
}




                                                 
/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_RestorePrinter
// ______________________

BOOL CdllCallManager::NtcssAPI_RestorePrinter ()
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL ok = TRUE;

    if (!gbl_dll_man.CMcall_sessionHasSpoolPrintLock())
    {
        ok = FALSE;

        session->ssnErrorMan()->RecordError
            ("Print spooling not initialized by current application");
    }

    if (ok)
    {
        CdllSpoolPrintManager* prt_man = gbl_dll_man.dllSpoolPrintMan();

        ok = prt_man->restorePrinter();

        gbl_dll_man.CMcall_sessionSpoolPrintUnLock();
    }

    if (!ok)
        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);

    return (ok);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ScheduleServerProcess
// ______________________________

unsigned long CdllCallManager::NtcssAPI_ScheduleServerProcess
		(LPCTSTR schedule_str,
		 NtcssServerProcessDescr *descr_ptr,
         LPTSTR pid_buf,
         const int pid_buf_size)

{
	

	return(NtcssPRV_ScheduleServerProcessEx
					(NULL,
					 schedule_str,
					 descr_ptr,
					 pid_buf,
					 pid_buf_size));
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_SelectPrinter
// ______________________

HDC  CdllCallManager::NtcssAPI_SelectPrinter 
(const char         *output_type_str,
 const int           seclvl,
 const BOOL          default_flag,
 const BOOL          gen_access_flag,
 const BOOL          return_DC_flag,
 const NtcssPrintAttrib  *pr_attr_ptr,
 const BOOL          local_enable_flag,
 const BOOL          remote_enable_flag)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if ( (*output_type_str == '\0') && remote_enable_flag || 
			(pr_attr_ptr == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		if ((ok) && (!gbl_dll_man.CMcall_sessionGetSpoolPrintLock(session)))
		{
			ok = FALSE;
			
			session->ssnErrorMan()->RecordError
				("Another application currently has print spooling in progress");
		}
		
		HDC the_return_hdc = NULL;
		
		CdllSpoolPrintManager* prt_man;
		
		if (ok)
		{
			prt_man = gbl_dll_man.dllSpoolPrintMan();
			
			ok = prt_man->selectPrinter(output_type_str, 
				seclvl,
				default_flag,
				gen_access_flag,
				return_DC_flag,
				pr_attr_ptr,
				local_enable_flag,
				remote_enable_flag);
			
			if ( (!ok) || prt_man->userCancelled() ) 
			{
				gbl_dll_man.CMcall_sessionSpoolPrintUnLock();
			}
		}
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		// NEEDTODO
		//  need to have optional mechanism so that HandleError doesn't clear
		//  error as I need it retained here so that caller will know whether
		//  error happened or user cancelled
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		if ( (ok) && (!prt_man->userCancelled()) )
		{
			if (return_DC_flag)
				return (prt_man->theHDC());
			else
				return((HDC)TRUE);
		}
		else if (prt_man->userCancelled())
		{
			session->ssnErrorMan()->ClearError();
			return(NULL);
		}
		else
			return(FALSE);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_sessionSpoolPrintUnLock();
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
}


int CdllCallManager::NtcssSelectServerFile(CString lpszServer,CString lpszDirectory,
						  int buffSize, LPTSTR lpszFileName)
{
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	int ret=-1;
	
	try
	{

		if (session == NULL)
		{
			ret = -1;
			throw(_T("Couldn't obtain a session"));
		}

		
		CGetServerFilename dlg(NULL,session,lpszServer,lpszDirectory);

		if(dlg.DoModal()==IDOK)
			if(buffSize<dlg.m_SelectedFileName.GetLength())
			{
				ret= 0;
				throw(_T("Buffer Passed to DLL is to small"));
			}

			else
			{
				strcpy(lpszFileName,dlg.m_SelectedFileName);
				ret= dlg.m_SelectedFileName.GetLength();
			}
		else
			ret=0;
	}

	catch(LPTSTR pStr)
	{
		session->ssnErrorMan()->RecordError(pStr, MB_OK);
		session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());
		
	}
 

	gbl_dll_man.CMcall_releaseSession(session);
	return ret;
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_SetAppData
// ___________________

BOOL CdllCallManager::NtcssAPI_SetAppData
(LPCTSTR app_data_str)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if (app_data_str == NULL) 
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (ok)
		{
			CmsgCHANGEAPPDATA msg;
			
			ok = msg.Load((LPCTSTR)session->ssnAppName(),
		              app_data_str);
			if (ok)
				ok = msg.DoItNow();
			
			if (ok)
				gbl_dll_man.dllUser()->setAppData(session->ssnAppName(),
				app_data_str);
		}

		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok);
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_SetLinkData
// ___________________

BOOL CdllCallManager::NtcssAPI_SetLinkData
(LPCTSTR link_data)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if (link_data == NULL ) 
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		
		if (ok)
		{
			
			CmsgSETLINKDATA msg;
			
			ok = msg.Load((LPCTSTR)session->ssnAppName(),
		              link_data);
			
			if (ok)
				ok = msg.DoItNow();
			
		}
		
		else
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_SpoolPrintRequest
// ___________________________

BOOL CdllCallManager::NtcssAPI_SpoolPrintRequest(void)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    BOOL ok = TRUE;

    if (!gbl_dll_man.CMcall_sessionHasSpoolPrintLock())
    {
        ok = FALSE;

        session->ssnErrorMan()->RecordError
            ("Print spooling not initialized by current application");
    }

    if (ok)
    {
        CdllSpoolPrintManager* prt_man = gbl_dll_man.dllSpoolPrintMan();

        ok = prt_man->spoolPrintRequest();

        //if (!ok)
          //  gbl_dll_man.CMcall_sessionSpoolPrintUnLock(); //jgj 6/16
		if (!ok)
			AfxMessageBox("Warning: No data has been spooled to the printer!",
							MB_ICONEXCLAMATION);
    } else 
		if (!ok)
	        session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            session->ssnAppName());

    gbl_dll_man.CMcall_releaseSession(session);

    return (ok);

	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}




/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_StartServerProcess
// ___________________________

BOOL CdllCallManager::NtcssAPI_StartServerProcess
		(NtcssServerProcessDescr* descr_ptr,
         LPTSTR pid_buf,
         const int pid_buf_size)
{
	

	return(NtcssPRV_StartServerProcessEx
					(NULL,
					 descr_ptr,
					 pid_buf,
					 pid_buf_size));
}



/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_StopServerProcess
// __________________________

BOOL CdllCallManager::NtcssAPI_StopServerProcess 
(LPCTSTR pid_str,
 const int signal_type)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		if (pid_str == NULL)
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if ( (signal_type != NTCSS_STOP_PROCESS_SOFT) && 
			(signal_type != NTCSS_STOP_PROCESS_HARD) )
		{
			session->ssnErrorMan()->RecordError("Bad signal type parameter");
			ok = FALSE;
		}                                       
		
		CmsgKILLSVRPROC msg;
		
		ok = msg.Load(pid_str, signal_type, session->ssnAppName()); 
		
		if (ok)
			if(!(ok = msg.DoItNow())) //TR10162
				session->ssnErrorMan()->RecordError(session->ssnErrorMan()
				->GetLastErrorMsgText(), MB_OK);
			
			if (!ok)
				session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
				session->ssnAppName());
			
			gbl_dll_man.CMcall_releaseSession(session);
			
			return (ok);
			
	}
	
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
	
}

                                                 

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ValidateUser
// ___________________

BOOL CdllCallManager::NtcssAPI_ValidateUser (LPTSTR ssn, int ssn_name_buf_size)

{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = validateBufSize(ssn_name_buf_size, SIZE_SSN,
			session);
		
		if ( (ok) && (ssn == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		if(ok)
		{
			
			
			CValidateUserDialog validate_dlg;
			
			ok = (validate_dlg.DoModal() == IDOK);
			
			
			if(ok)
			{
				CEncrypt crypt;
				CString strEncryptPassword=crypt.plainEncryptString(validate_dlg.m_UserName+validate_dlg.m_UserName,
					validate_dlg.m_Password);
				
				CmsgVALIDATEUSER msg;
				
				ok = msg.Load(validate_dlg.m_UserName.GetBufferSetLength(SIZE_LOGIN_NAME),
					strEncryptPassword.GetBufferSetLength(SIZE_APP_PASSWORD*2),
					session->ssnAppName());
				
				if (ok)
					ok = msg.DoItNow();
				
				if (ok)
					ok = msg.UnLoad(ssn);
			}
		}
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok);
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssAPI_ViewServerFile
// ___________________

void CdllCallManager::NtcssAPI_ViewServerFile
(CString ServerName,CString Directory)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return;
		}
		
		CViewSFileDlg dlg(NULL,session,ServerName,Directory);
		
		dlg.DoModal();
	}
	
	catch(...)
	{
		
	}
	
	gbl_dll_man.CMcall_releaseSession(session);
    
}


//***************************************************************************
// CdllCallManager NTCSS DeskTop API's 
//***************************************************************************

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_ClearLRS
// _________________

int CdllCallManager::NtcssPRV_ClearLRS()
{
    gbl_dll_man.dllErrorMan()->ClearError();

    BOOL ok = gbl_dll_man.validateThisIsDeskTop();

    if (ok)
        gbl_dll_man.dllUser()->CMcall_invalidateShmemLrsData();

	if (ok)
		gbl_dll_man.CMcall_deskTopReset();

    if (ok)

        return(DESKTOP_INT_OK);

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(DESKTOP_INT_ERROR);
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_PortDelete
// _________________

BOOL CdllCallManager::NtcssPRV_PortDelete()
{
    //gbl_dll_man.dllErrorMan()->ClearError();

    CPrintUtil PU;
	if(PU.IsNtcssPort())
		return 	PU.DelNTCSSPort();
	else
		return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_IsPrintProcessAlive
// _________________

void CdllCallManager::NtcssPRV_IsPrintProcessAlive()
{
   
	gbl_dll_man.IsPrintProcessAlive();
	return;
 
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_DTInitialize
// _____________________

int CdllCallManager::NtcssPRV_DTInitialize(char *version)
{
// NEEDTODO
// Need to get Tony to stop calling this twice

if (gbl_dll_man.thisIsDeskTopDll())
{
//AfxMessageBox("Second call to NtcssPRV_DTInitialize??", MB_OK);
return(DESKTOP_INT_OK);
}

    gbl_dll_man.dllErrorMan()->ClearError();


    if (gbl_dll_man.CMcall_deskTopInit(version))
	{
//AfxMessageBox("Leaving NtcssPRV_DTInitialize - did okay",MB_OK);

        return(DESKTOP_INT_OK);
	}
    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

        return(DESKTOP_INT_OK);
    }

}




/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_DTReset
// ________________

int CdllCallManager::NtcssPRV_DTReset(char *version)
{
    gbl_dll_man.dllErrorMan()->ClearError();

    if ( (!gbl_dll_man.validateDeskTopInitialized()) ||
         (!gbl_dll_man.validateThisIsDeskTop()) )
    {
        return(FALSE);
    }

    BOOL ok = gbl_dll_man.CMcall_deskTopReset();

    if (ok)

        return(DESKTOP_INT_OK);

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(DESKTOP_INT_ERROR);
    }
}


    
/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetToken
// _________________

int CdllCallManager::NtcssPRV_GetToken(char *token)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if ( (!gbl_dll_man.validateDeskTopInitialized()) ||
			(!gbl_dll_man.validateUser()) )
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		strcpy(token, gbl_dll_man.dllUser()->userInfo()->token_buf);
		
		gbl_dll_man.CMcall_releaseSession(session);
		return(TRUE);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_SetLRS
// _______________

int CdllCallManager::NtcssPRV_SetLRS(void *ptr_from_desktop)
{
	
	try
	{
		gbl_dll_man.dllErrorMan()->ClearError();
		
		BOOL ok = gbl_dll_man.validateThisIsDeskTop();
		
		
		
		if (ok)
			
			ok = gbl_dll_man.dllUser()->CMcall_initializeShmemLrsData
			( (SshmemLoginReplyStruct *)
			ptr_from_desktop);
		
		
		gbl_dll_man.CMcall_setInitializedOk();
		
		if (ok)
			return(DESKTOP_INT_OK);
		
		else
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			
			return(DESKTOP_INT_ERROR);
		}
		
	}
	
	catch(...)
	{
		return(DESKTOP_INT_ERROR);
	}
}


//***************************************************************************
// CdllCallManager NTCSS Secban API's 
//***************************************************************************



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_CurrentSecurityLevel
// ________________________

SecurityLevel CdllCallManager::NtcssPRV_CurrentSecurityLevel()
{
    SecurityLevel seclvl_val;
    BOOL ok = gbl_dll_man.validateSecBanInitialized();

    if (ok)
    {
        seclvl_val = gbl_dll_man.CMcall_overallSecurityLevel();
        return(seclvl_val);
    }

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(NTCSS_UNCLASSIFIED);
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_ReserveSessionTableSpace
// _________________________________

int CdllCallManager::NtcssPRV_ReserveSessionTableSpace()
{
// NEEDTODO
// Really need to lock onto the SECBAN's session here with a semaphore

    BOOL ok = gbl_dll_man.validateThisIsSecBan();
    int idx;

    if (ok)
    {
        idx = gbl_dll_man.CMcall_reserveSessionTableSpace();

        ok = (idx != RETURN_INT_ERROR);
    }

    if (ok)
        return(idx);

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

        return(RETURN_INT_ERROR);
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_NtcssPRV_SecBanInitialize
// __________________________________

BOOL CdllCallManager::NtcssPRV_SecBanInitialize(char *version)
{
// NEEDTODO
// Really need to lock onto the SECBAN's session with a semaphore

    gbl_dll_man.dllErrorMan()->ClearError();

    if (gbl_dll_man.CMcall_secbanInit(version))

        return(TRUE);

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

        return(FALSE);
    }
}



//***************************************************************************
// CdllCallManager NTCSS Applicaction API's (private) 
//***************************************************************************

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_AdminLockOptions
// _________________________
//
BOOL CdllCallManager::NtcssPRV_AdminLockOptions(const char *appname,
					  const char *code,const char *action,char *pid,
					  const char *hostname)

{                               
 
  CmsgADMINLOCKOPS msg;


  
    BOOL ok = msg.Load(appname,code,action,pid,hostname);

   //BOOL ok = msg.Load(APPNAME,CODE,ACTION,PID);

 
    if (ok)
        ok = msg.DoItNow();

		
	if (ok)
	{
		int i;
		ok =msg.UnLoad(&i);
		if(ok)
		
		  itoa(i,pid,10); 

	} 




    return(ok);
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetDomainName
// ______________________

BOOL CdllCallManager::NtcssPRV_GetNTDomainName(char* DomainName,LPDWORD cchDomainName)
{ 
    HANDLE hToken;

	try
	{

    #define MY_BUFSIZE 512
    UCHAR InfoBuffer[ MY_BUFSIZE ];
    DWORD cbInfoBuffer = MY_BUFSIZE;
	DWORD cchUserName= MY_BUFSIZE;
    SID_NAME_USE snu;
    char UserName[MY_BUFSIZE];
	
    BOOL bSuccess;

    if(!OpenThreadToken(
        GetCurrentThread(),
        TOKEN_QUERY,
        TRUE,
        &hToken
        )) {

        if(GetLastError() == ERROR_NO_TOKEN) {
            if(!OpenProcessToken(
                GetCurrentProcess(),
                TOKEN_QUERY,
                &hToken
                )) return FALSE;

        } else {
            return FALSE;
        }
    }

    bSuccess = GetTokenInformation(
        hToken,
        TokenUser,
        InfoBuffer,
        cbInfoBuffer,
        &cbInfoBuffer
        );

    if(!bSuccess) {
        if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            CloseHandle(hToken);
            return FALSE;

        } else {
            CloseHandle(hToken);
            return FALSE;
        }
    }

    CloseHandle(hToken);

    return LookupAccountSid(
        NULL,
        ((PTOKEN_USER)InfoBuffer)->User.Sid,
        (LPTSTR)UserName,
        &cchUserName,
        DomainName,
        cchDomainName,
        &snu
        );
	}

	catch(...)
	{
		return FALSE;
	}

} 

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetDllVersion
// ______________________

BOOL CdllCallManager::NtcssPRV_GetDllVersion(char *version_num)
{
    sprintf(version_num, "NTCSS DLL Version %s (%s)",
            NTCSS_DLL_VERSION, 
            THE_BUILD_NUM);

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_DoMessage
// ____________________
//


BOOL CdllCallManager::NtcssPRV_DoMessage(SDllMsgRec *msg)



{                               
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }
								       // Two different versions of
									   // CdllMsgDeliveryMan constructor

    CdllMsgDeliveryMan dlv_man;


	BOOL ok = dlv_man.DoMessage(session, msg, NULL);

									   // Don't handle error for this
									   // API

    gbl_dll_man.CMcall_releaseSession(session);

    return(ok);
}


 
/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_DoMessageCleanup
// _________________________
//
BOOL CdllCallManager::NtcssPRV_DoMessageCleanup(const SDllMsgRec *msg)

{                               
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    CMemChain memchain_ptr((TMemChainLink *)msg->recv_data_chain_ptr);

    memchain_ptr.TrashChainLinks();
									   // Don't handle error for this
									   // API
									   //
    gbl_dll_man.CMcall_releaseSession(session);

    return(TRUE);
}


 
/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetAppHost
// _______________________

BOOL CdllCallManager::NtcssPRV_GetAppHost 
						(LPCTSTR app_name_str,
						 LPTSTR host_buf,
						 int host_buf_size)
{
	BOOL OK=TRUE;
	NtcssAppList *tmp_ptr=NULL,*app_list_ptr=NULL;

	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

	if (session == NULL)
	{
		gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
		return(FALSE);
	}



		
		if(!validateBufSize(host_buf_size, 
								  SIZE_HOST_NAME, session))
			throw(_T("NtcssPRV_GetAppHost: Invalid host_buf_size"));


		CmsgGETAPPLIST msg;

  
		if(!msg.Load())
			throw(_T("NtcssPRV_GetAppHost: CmsgGETAPPLIST msgload failed"));

		if(!msg.DoItNow())
			throw(_T("NtcssPRV_GetAppHost: CmsgGETAPPLIST message failed"));

		NtcssAppList *app_list_ptr = NULL;
		int app_cnt;

		if(!msg.getAppCount(&app_cnt))
			throw(_T("NtcssPRV_GetAppHost: CmsgGETAPPLIST getAppCount failed"));


		app_list_ptr = new NtcssAppList[app_cnt];
		if (app_list_ptr == NULL)
			throw(_T("NtcssPRV_GetAppHost: CmsgGETAPPLIST getAppCount failed"));


		if(!msg.UnLoad(app_cnt,&app_cnt,app_list_ptr))
			throw(_T("NtcssPRV_GetAppHost: CmsgGETAPPLIST getAppCount failed"));


		tmp_ptr = app_list_ptr;

		BOOL found = FALSE;
		for (int i = 0; ((!found) && (i < app_cnt)); i++)
		{
			if (strncmp(app_name_str,tmp_ptr->app_name,SIZE_GROUP_TITLE) == 0)
			{
				found = TRUE;
				strncpyWpad(host_buf,tmp_ptr->host_name,
							SIZE_HOST_NAME, '\0');
			}
			tmp_ptr++;
		}

		if (!found)
			throw(_T("NtcssPRV_GetAppHost: Application not found"));
	}

	catch(LPSTR pstr)
	{
		gbl_dll_man.dllErrorMan()->RecordError
                    (pstr);
		OK=FALSE;
	}

	catch (...)
	{
		gbl_dll_man.dllErrorMan()->RecordError
                    (_T("UnKnown exception caught"));
		OK=FALSE;
	}


	if (app_list_ptr != NULL)
		delete [] app_list_ptr;

    gbl_dll_man.CMcall_releaseSession(session);

	return(OK);
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetAppNameList
// _______________________

BOOL CdllCallManager::NtcssPRV_GetAppNameList 
                                        (char *app_list, int size)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}

		BOOL ok = gbl_dll_man.dllUser()->getAppNameList(app_list, size, session);

		gbl_dll_man.CMcall_releaseSession(session);

		return(ok);
	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetAppRole
// ___________________

int CdllCallManager::NtcssPRV_GetAppRole(LPCTSTR app_name)
{


    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    const SappGroupRec_wFullData *group_ptr;

	char env_app_name_str[SIZE_GROUP_TITLE+1];

	int env_ok=GetEnvironmentVariable("APPNAME",env_app_name_str,SIZE_GROUP_TITLE+1);

	if (env_ok == 0)
		strcpy(env_app_name_str,app_name);


    group_ptr = gbl_dll_man.dllUser()->appGroup(env_app_name_str);

    if (group_ptr != NULL)
    {
        gbl_dll_man.CMcall_releaseSession(session);
        return(group_ptr->app_role);
    }
    else
    {
        //jgj 7/25/97 changed to just ret 0 on null pointer
		//session->ssnErrorMan()->RecordError("Invalid application");
        //session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
                                            //session->ssnAppName());
        gbl_dll_man.CMcall_releaseSession(session);
        return(/*RETURN_INT_ERROR*/0);
    }
	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return 0;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetNumTasks
// ____________________

int CdllCallManager::NtcssPRV_GetNumTasks()
{
    BOOL ok = gbl_dll_man.validateDeskTopInitialized();

    if (ok)
    {
        return( gbl_dll_man.CMcall_activeProcessCnt() );
    }
    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

        return(RETURN_INT_ERROR);
    }
}

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetNumTasks
// ____________________

void CdllCallManager::NtcssPRV_UpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,
													 const DWORD nNewProcessID,
											         const DWORD nNewThreadID)
{
    BOOL ok = gbl_dll_man.validateDeskTopInitialized();

    if (ok)
		gbl_dll_man.CMcall_UpdateDTSessionValues(OldhWnd,NewhWnd,nNewProcessID,nNewThreadID);
   
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetMasterName
// ____________________
 
BOOL CdllCallManager::NtcssPRV_GetMasterName(char* strMasterName)
{
	try
	{
		if(!gbl_dll_man.validateDeskTopInitialized())
			throw;
	
        CmsgGETMASTERNAME msg;

		if(!msg.Load())
			throw;
		if(!msg.DoItNow())
			throw;
		if(!msg.UnLoad(strMasterName))
			throw;

		return TRUE;
	}
    catch(...)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return FALSE;
    }
}

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetUserRole
// ____________________

int CdllCallManager::NtcssPRV_GetUserRole (void)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
        return(FALSE);
    }

    gbl_dll_man.CMcall_releaseSession(session);

    return(gbl_dll_man.dllUser()->userInfo()->user_role);
	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_KillProcesses
// ____________________

void CdllCallManager::NtcssPRV_KillProcesses()
{
   gbl_dll_man.CMcall_KillActiveProcesses();

}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_SetCommonData
// ______________________

BOOL CdllCallManager::NtcssPRV_SetCommonData 
(LPTSTR tag, LPTSTR data)

{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		char lcl_tag_str[SIZE_COMMON_TAG+1];
		strncpy(lcl_tag_str,tag,SIZE_COMMON_TAG);
		
		trim(lcl_tag_str);
		
		if (!validateCommonDataTag(lcl_tag_str, session))
			ok = FALSE;
		
		CmsgSETCOMMONDB msg;
		
		if (ok)
			ok = msg.Load(lcl_tag_str, data);
		
		if (ok)
			ok = msg.DoItNow();
		
		if (ok)
		{
			gbl_dll_man.CMcall_releaseSession(session);
			return (TRUE);
		}
		else
		{
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
				session->ssnAppName());
			gbl_dll_man.CMcall_releaseSession(session);
			return(FALSE);
		}
		
	}
	
	catch(...)
	{
		session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
        gbl_dll_man.CMcall_releaseSession(session);
        return(FALSE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_ScheduleServerProcessEx
// ______________________________

unsigned long CdllCallManager::NtcssPRV_ScheduleServerProcessEx
(LPCTSTR app_name_str,
 LPCTSTR schedule_str,
 NtcssServerProcessDescr *descr_ptr,
 LPTSTR pid_buf,
 const int pid_buf_size)
{
	
	
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		BOOL ok = TRUE;
		
		
		if ( (schedule_str == NULL) || (descr_ptr == NULL) || 
			(pid_buf == NULL))
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		if(descr_ptr->needs_device_flag || descr_ptr->sdi_required_flag)
		{
			session->ssnErrorMan()->RecordError
				("Scheduled job cannot require a device");
			ok = FALSE;
		}
		
		//9-29-97  TR10129
		const SuserInfoRec_wFullData *user = gbl_dll_man.dllUser()->userInfo();
		//9-29-97  TR10129
		
		if ( (ok) && (descr_ptr->seclvl < NTCSS_UNCLASSIFIED) || 
			(NTCSS_TOPSECRET < descr_ptr->seclvl)/*9-29-97  TR10129*/||
			descr_ptr->seclvl > user->security_class /*9-29-97  TR10129*/ )
		{
			session->ssnErrorMan()->RecordError
				("Bad security level parameter");
			ok = FALSE;
		}
		
		if ((ok) && (descr_ptr->priority_code < NTCSS_LOW_PRIORITY) || 
			(NTCSS_HIGH_PRIORITY < descr_ptr->priority_code) )
		{
			session->ssnErrorMan()->RecordError
				("Bad priority parameter");
			ok = FALSE;
		}
		
		if (ok)
			ok = validateBufSize(pid_buf_size, SIZE_PROCESS_ID, session);
		
		char lcl_raw_sched[SIZE_SCHEDULE];
		
		if ( (ok) && ((schedule_str == NULL) || (*schedule_str == NULL)) )
		{
			CScheduleDlg sched_dlg;
			
			if (sched_dlg.DoModal() == IDOK)
			{
				sched_dlg.TheSchedule().GetRawSchedule(lcl_raw_sched);
			}
			else
			{
				session->ssnErrorMan()->RecordError
                    ("Schedule selection cancelled, process not started");
				ok = FALSE;
			}
		}
		else if (ok)
		{
			
			CSchedule given_sched(schedule_str);
			
			if (given_sched.IsValid())
			{
				given_sched.GetRawSchedule(lcl_raw_sched);
				
			}
			else
			{
				session->ssnErrorMan()->RecordError
                    ("Given schedule is invalid, process not started");
				ok = FALSE;
			}
		}
		
		
		char lcl_printer_buf[SIZE_PRINTER_NAME];
		char lcl_host_buf[SIZE_HOST_NAME];
		
		if ( (ok) && (descr_ptr->prt_output_flag) )
		{
			const SremotePrinterInfo *the_printer;
			
			//jj added 9-24-97 to stop gpf when gen_access & default are both true
			//
			if(descr_ptr->use_default_prt_flag) descr_ptr->general_access_prt_flag=FALSE;
			/////////////////////////////////////////////////////////////////
			
			CSelPrtDialog sel_prn_dlg(CWnd::FromHandle(session->ssnHWnd()),
				session);
			
			sel_prn_dlg.enableRemoteSelection(descr_ptr->output_type_str,
				descr_ptr->seclvl,
				FALSE,
				descr_ptr->general_access_prt_flag); 
			
			if (!descr_ptr->use_default_prt_flag)
			{
				
				if (sel_prn_dlg.DoModal() == IDOK)
				{
					the_printer = sel_prn_dlg.getRemotePrinterSelection();
					strcpy(lcl_printer_buf,the_printer->printer_name_buf);
					strcpy(lcl_host_buf,the_printer->host_name_buf);
					
					descr_ptr->nCopies= the_printer->nCopies;  //12/08
					descr_ptr->bBanner= the_printer->bBanner;  //this should pass it through
				}
				else
				{
					session->ssnErrorMan()->RecordError
                        ("Printer selection cancelled, process not started");
					ok = FALSE;
				}
			}
			else 
			{
				
				//the_printer = sel_prn_dlg.getDefaultRemotePrinter();  TR10124
				// no check for NULL when there is no default printer=GPF on strcpy
				
				if((the_printer = sel_prn_dlg.getDefaultRemotePrinter()))   //TR10124
				{
					strcpy(lcl_printer_buf,the_printer->printer_name_buf);
					strcpy(lcl_host_buf,the_printer->host_name_buf);
				}
				else //TR10124 added else 
				{
					session->ssnErrorMan()->RecordError
                        ("No Default Printer , process not started");
					ok = FALSE;
				}
				
			}
		}
		
		char lcl_unix_owner_str[SIZE_TINY_GENBUF];
		
		if ((descr_ptr->owner_str[0] == '\0') || (isBlank((char*)descr_ptr->owner_str)))
			strcpy(lcl_unix_owner_str, 
			(LPCTSTR)gbl_dll_man.dllUser()->loginName());
        
		else
			strcpy(lcl_unix_owner_str, descr_ptr->owner_str);
		
		CmsgSCHEDSVRPROCBG msg;
		
		
		if (ok)
			ok = msg.Load(lcl_raw_sched,
			descr_ptr,		 
			app_name_str==NULL ? (LPCTSTR)session->ssnAppName() : app_name_str,
			lcl_unix_owner_str,
			lcl_printer_buf, 
			lcl_host_buf);
		
		
		if (ok)
			ok = msg.DoItNow();
		
		
		
		if (ok)
			ok = msg.UnLoad(pid_buf);
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_StartServerProcessEx
// _____________________________

BOOL CdllCallManager::NtcssPRV_StartServerProcessEx
(LPCTSTR app_name_str,
 NtcssServerProcessDescr* descr_ptr,
 LPTSTR pid_buf,
 const int pid_buf_size)
{
	
	Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();
	
	try
	{
		
		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return(FALSE);
		}
		
		
		
		BOOL ok = TRUE;
		
		if ( (descr_ptr == NULL) || (pid_buf == NULL) )
		{
			session->ssnErrorMan()->RecordError(TEXT_MSG_BAD_REQ_PARM, MB_OK);
			ok = FALSE;
		}
		
		
		//9-29-97  TR10129
		const SuserInfoRec_wFullData *user = gbl_dll_man.dllUser()->userInfo();
		//9-29-97  TR10129
		
		
		if ((ok) &&
			(descr_ptr->seclvl < NTCSS_UNCLASSIFIED) || 
			(NTCSS_TOPSECRET < descr_ptr->seclvl) /*9-29-97  TR10129*/||
			descr_ptr->seclvl > user->security_class /*9-29-97  TR10129*/)
			
		{
			session->ssnErrorMan()->RecordError
				("Bad security level parameter");
			ok = FALSE;
		}
		
		
		if ((ok) && (descr_ptr->priority_code < NTCSS_LOW_PRIORITY) || 
			(NTCSS_HIGH_PRIORITY < descr_ptr->priority_code) )
		{
			session->ssnErrorMan()->RecordError
				("Bad priority parameter");
			ok = FALSE;
		}
		
		if (ok)
			ok = validateBufSize(pid_buf_size, SIZE_PROCESS_ID, session);
		

		CString lcl_printer_buf,lcl_host_buf;
		

		if ( (ok) && (descr_ptr->prt_output_flag) )
		{
			const SremotePrinterInfo *the_printer;

			//jj added 9-24-97 to stop gpf when gen_access & default are both true
			//10124
			if(descr_ptr->use_default_prt_flag) descr_ptr->general_access_prt_flag=FALSE;
			/////////////////////////////////////////////////////////////////
			
			
			CSelPrtDialog sel_prn_dlg(CWnd::FromHandle(session->ssnHWnd()),
				session,TRUE,descr_ptr->nCopies,descr_ptr->bBanner
				/*descr_ptr->nOrient,descr_ptr->nPaperSize*/);//12/04
				
				//sel_prn_dlg.disableSetup(); //4/29/99
				
				
				sel_prn_dlg.enableRemoteSelection(descr_ptr->output_type_str,
				descr_ptr->seclvl,
				FALSE, //hardcoded because server message
				//doesn't implement this correctly
				descr_ptr->general_access_prt_flag);
			
			
			if (!descr_ptr->use_default_prt_flag)
			{
				
				
				if (sel_prn_dlg.DoModal() == IDOK)
				{
					
					
					the_printer = sel_prn_dlg.getRemotePrinterSelection();

					TRACE1(_T("StartServerProcess() printername is <%s> \n"),
					the_printer->printer_name_buf);
					
					descr_ptr->nCopies= the_printer->nCopies;  //12/08
					descr_ptr->bBanner= the_printer->bBanner;  //this should pass it through
					
					lcl_printer_buf=the_printer->printer_name_buf;
					lcl_host_buf=the_printer->host_name_buf;

				}
				else
				{
					session->ssnErrorMan()->RecordError
                        ("Printer selection cancelled, process not started");
					ok = FALSE;
				}
				
				
			}
			else 
			{
				
				
				//the_printer = sel_prn_dlg.getDefaultRemotePrinter(); //no chk for NULL=GPF
				//TR10124 was just doing strcpy without checking
				
				
				if((the_printer = sel_prn_dlg.getDefaultRemotePrinter()))//TR10124
				{

					lcl_printer_buf=the_printer->printer_name_buf;
					lcl_host_buf=the_printer->host_name_buf;

				}
				else//TR10124  added this else for when there is no default
				{
					
					session->ssnErrorMan()->RecordError
						("No Default Printer , process not started");
					ok = FALSE;
					
				}
			}
		}
		
		CString lcl_unix_owner_str;
		
		if (descr_ptr->owner_str[0] == '\0')
			lcl_unix_owner_str=(LPCTSTR)gbl_dll_man.dllUser()->loginName();

		else
			lcl_unix_owner_str=descr_ptr->owner_str;

		
		CString lcl_app_name_ptr;
		
		if (app_name_str != NULL)
			lcl_app_name_ptr = app_name_str;
		else
			
			lcl_app_name_ptr = (LPCTSTR)session->ssnAppName();
		
		CmsgLAUNCHSVRPROCBG msg;
		
		if (ok)
			ok = msg.Load(descr_ptr, 
			lcl_app_name_ptr,
			lcl_unix_owner_str,
			lcl_printer_buf, 
			lcl_host_buf);
		
		
		if (ok)
			ok = msg.DoItNow();
		
		
		if (ok)
			ok = msg.UnLoad(pid_buf);
		
		
		if (!ok)
			session->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName(),
			session->ssnAppName());
		
		gbl_dll_man.CMcall_releaseSession(session);
		
		
		return (ok);
		
	}
	
	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_ThunkerInitialize
// __________________________

BOOL CdllCallManager::NtcssPRV_ThunkerInitialize(char *version)
{
AfxMessageBox("Doing NtcssPRV_ThunkerInitialize", MB_OK);

    if (gbl_dll_man.CMcall_thunkerInit(version))
        return(TRUE);

    else
    {
        gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

        return(FALSE);
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_IsAuthServer
// _______________

BOOL CdllCallManager::NtcssPRV_IsAuthServer(const char *lpszGroupTitle)
{

  gbl_dll_man.dllErrorMan()->ClearError();
  return gbl_dll_man.dllUser()->IsAuthServer(lpszGroupTitle);

}





/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetAuthenticationServer
// _______________

BOOL CdllCallManager::NtcssPRV_GetAuthenticationServer(const char *lpszApp,char *lpszHost)
{
	gbl_dll_man.dllErrorMan()->ClearError();
	CString temp;
	if((temp=gbl_dll_man.dllUser()->GetAuthServer(lpszApp))!=_T(""))
	{
		strncpy(lpszHost,(LPCTSTR)temp,SIZE_HOST_NAME);
		return TRUE;
	}
	else
		return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_IsAdmin
// _______________

BOOL CdllCallManager::NtcssPRV_IsAdmin()
{
	//checks if the user is an admin either locally or globally
/*	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	TCHAR szLogonServerName[MAX_COMPUTERNAME_LENGTH + 1];
	TCHAR szUserName[MAX_COMPUTERNAME_LENGTH + 1];
	USES_CONVERSION;
	DWORD rc, pref, got, total;
	LOCALGROUP_USERS_INFO_0 *buf;
	GROUP_USERS_INFO_0 *buf2;
	BOOL bIsAdmin=FALSE;
	
	DWORD dwComputerNameLength = sizeof (szComputerName);
	DWORD dwUserNameLength = sizeof (szUserName);
	wchar_t *server=NULL;
	wchar_t *user;

	if(!::GetUserName(szUserName, &dwUserNameLength))
	{
	   AfxMessageBox("Couldn't get UserName");
	   return FALSE;
	}

	user=T2W(szUserName);

	if(!::GetComputerName(szComputerName, &dwComputerNameLength))
	{
	   AfxMessageBox("Couldn't get UserName");
	   return FALSE;
	}

		//test for non-local domain

	if(::GetEnvironmentVariable("LOGONSERVER",szLogonServerName,MAX_COMPUTERNAME_LENGTH + 1)&&
		lstrcmpi(szComputerName, &szLogonServerName[2]) != 0) 
			
		server=T2W(szLogonServerName);

//now do the API's looking for Admin entry
//do local version first first

	pref = 16; // start low, get more later
	buf = NULL; // important!

	do
	{
		pref *= 2; // bump buffer size
	
		if ( buf != NULL )
		{
			NetApiBufferFree( buf );
			buf = NULL; // important!
		}

		rc = NetUserGetLocalGroups(server, user, 0, LG_INCLUDE_INDIRECT,
			(LPBYTE *) &buf, pref, &got, &total );
	} while ( rc == NERR_BufTooSmall || rc == ERROR_MORE_DATA );

	if ( rc == 0 )
		for ( rc = 0; buf != NULL && rc < got; rc ++ )
		{
			if(wcscmp(buf[rc].lgrui0_name,L"Administrators") == 0)
			{
				bIsAdmin=TRUE;
				break;
			}
			
		}

	if ( buf != NULL )
		NetApiBufferFree( buf );

	if(bIsAdmin)
	
		return TRUE;


//////////if not found yet try global groups

	pref = 16; // start low, get more later
	buf2 = NULL; // important!

	do
	{
		pref *= 2; // bump buffer size
	
		if ( buf2 != NULL )
		{
			NetApiBufferFree( buf2 );
			buf2 = NULL; // important!
		}

		rc = NetUserGetGroups(server, user, 0, (LPBYTE *) &buf2, pref, &got, &total );
	} while ( rc == NERR_BufTooSmall || rc == ERROR_MORE_DATA );

	if ( rc == 0 )
		for ( rc = 0; buf2 != NULL && rc < got; rc ++ )
		{
			if(wcscmp(buf2[rc].grui0_name,L"Domain Admins") == 0)
			{
				bIsAdmin=TRUE;
				break;
			}
		
		}

	if ( buf2 != NULL )
		NetApiBufferFree( buf2 );

	if(bIsAdmin)
		return TRUE;


	return FALSE;*/

/////////////////////////////////////////////////////////////////////////////
// felix's way

	int found;
	DWORD i, l;
	HANDLE hTok;
	PSID pAdminSid;
	SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
	CString strError;

	byte rawGroupList[4096];
	TOKEN_GROUPS& groupList = *( (TOKEN_GROUPS *) rawGroupList );

/*	if ( ! OpenThreadToken( GetCurrentThread(), TOKEN_QUERY, FALSE, &hTok ))
	{
		strError.Format("Cannot open thread token, trying process token [%lu]",
			GetLastError());

		AfxMessageBox(strError);*/
		
		if ( ! OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hTok ) )
		{
			strError.Format("Cannot open process token, quitting [%lu]",
				GetLastError());

			AfxMessageBox(strError);
			
			return FALSE;
		}
//	}

	// normally, I should get the size of the group list first, but ...
	l = sizeof rawGroupList;
	if ( ! GetTokenInformation( hTok, TokenGroups, &groupList, l, &l ) )
	{
		strError.Format("Cannot get group list from token [%lu]",
			GetLastError());

		AfxMessageBox(strError);
		
		return FALSE;
	}

	// here, we cobble up a SID for the Administrators group, to compare to.
	if ( ! AllocateAndInitializeSid( &ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSid ) )
	{
		strError.Format("Cannot create SID for Administrators [%lu]",
			GetLastError());

		AfxMessageBox(strError);
	
		return FALSE;
	}

	// now, loop through groups in token and compare
	found = FALSE;
	for ( i = 0; i < groupList.GroupCount; ++ i )
	{
		if ( EqualSid( pAdminSid, groupList.Groups[i].Sid ) )
		{
			found = TRUE;
			break;
		}
	}

	FreeSid( pAdminSid );
	CloseHandle( hTok );
	return found;
}





/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetLoginName
// _______________
// used by multi instance stuff like lock files & apply files to the server

BOOL CdllCallManager::NtcssPRV_GetLoginName(LPSTR lpszLoginName)
{

	try
	{
		strncpy(lpszLoginName,gbl_dll_man.dllUser()->loginName(),SIZE_LOGIN_NAME+1);
		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}

}


/////////////////////////////////////////////////////////////////////////////
// NtcssPRV_GetHostNameList
// _______________________
// Used to populate the known hosts section of the NTCSS.INI from the desktop
// With all the UNIQUE hosts for current user in the LRS
BOOL CdllCallManager::NtcssPRV_GetHostNameList(LPTSTR pList, int nSize)
{
    Csession *session = gbl_dll_man.CMcall_getSessionReadyForApiAction();

	try
	{

		if (session == NULL)
		{
			gbl_dll_man.dllErrorMan()->HandleError(gbl_dll_man.dllFacilityName());
			return FALSE;
		}

	
		CStringEx appList;
		CString hostList,strHost,strApp;
		CMapStringToString HostLIST;
		if(!gbl_dll_man.dllUser()->getAppNameList(appList.GetBuffer(nSize),nSize,session))
		{
			TRACE0(_T("NtcssPRV_GetHostNameList()-> dllUser()->getAppNameList() Failed!!\n"));
			return FALSE;
		}

		appList.ReleaseBuffer(); //Now Look up each Apps Host && Build Host String!!!

		for(int i=0;;i++)
		{
			if((strApp=appList.GetField(',',i))==_T(""))
				break;
			if((strHost=gbl_dll_man.dllUser()->GetAppServer(strApp))!=_T(""))
				HostLIST.SetAt(strHost,strHost);
		}


	    POSITION pos;
		CString strVal;
		for( pos = HostLIST.GetStartPosition(); pos != NULL; )
		{

			HostLIST.GetNextAssoc( pos, strHost, strVal);

			if(hostList.IsEmpty())
				hostList=strHost;
			else
				hostList+= _T(",") + strHost;

		}

		gbl_dll_man.CMcall_releaseSession(session);

		return _tcscpy(pList,hostList)!=NULL;

	}

	catch(...)
	{
		gbl_dll_man.CMcall_releaseSession(session);
		return FALSE;
	}
}



