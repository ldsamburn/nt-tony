/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "PrintUtil.h"
#include "0_basics.h"
#include "CallMan.h"
#include "msgcore.h"
#include "cregkey.h"
#include "encrypt.h"

//***************************************************************************
//***************************************************************************
// (1) User Application API's (public)
//***************************************************************************
//***************************************************************************


/////////////////////////////////////////////////////////////////////////////
// NtcssAssert
// ___________

extern "C" void DllExport  NtcssAssert
		(const BOOL assert_expr_val,
         const char  *message_text)
{
    try
    {
            gbl_dll_man.dllCallMan()->NtcssAPI_Assert
                                (assert_expr_val, message_text );
    }
    catch(...)
    {
        return;
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssCreateSchedule
// ___________________

extern "C" BOOL DllExport NtcssCreateSchedule
		(char   *schedule_buf,
         const int schedule_buf_size)
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssAPI_CreateSchedule
                                (schedule_buf, schedule_buf_size ));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssDeregisterSecurity
// _______________________

extern "C" BOOL DllExport  NtcssDeregisterSecurity
		(const HANDLE hInstance)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_DeregisterSecurity
                                                        (hInstance));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssDisplaySchedule
// ____________________

extern "C" BOOL DllExport NtcssDisplaySchedule
		(const char   *schedule_str)
{                                 
    try
    {
        return (gbl_dll_man.dllCallMan()->NtcssAPI_DisplaySchedule
                                                        (schedule_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssDLLInitialize
// __________________

extern "C" BOOL DllExport  NtcssDLLInitialize
		(const char   *dll_version_str,
		 const char   *app_name_str,
		 const HANDLE hInstance,
	     const HANDLE hWnd)
{    
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_DLLInitialize
                                            (dll_version_str,
                                             app_name_str,
                                             hInstance,
                                             hWnd));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssEditSchedule
// _________________

extern "C" BOOL DllExport NtcssEditSchedule
		(const char   *orig_schedule_str,
         char   *new_schedule_buf,
         const int new_schedule_buf_size)
{                                 
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssAPI_EditSchedule
                                (orig_schedule_str, 
                                 new_schedule_buf,
                                 new_schedule_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssErrorMessagingOFF
// ______________________

extern "C" void DllExport  NtcssErrorMessagingOFF()
{    
    try
    {
        gbl_dll_man.dllCallMan()->NtcssAPI_ErrorMessagingOFF();
    }
    catch(...)
    {
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssErrorMessagingON
// _____________________

extern "C" void DllExport  NtcssErrorMessagingON()
{    
    try
    {
        gbl_dll_man.dllCallMan()->NtcssAPI_ErrorMessagingON();
    }
    catch(...)
    {
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssErrorMessagingStatus
// _________________________

extern "C" BOOL DllExport  NtcssErrorMessagingStatus()
{    
    try
    {
        return(gbl_dll_man.dllCallMan()-> //...
                    NtcssAPI_ErrorMessagingStatus());
    }
    catch(...)
    {
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppData
// ___________________

extern "C" BOOL DllExport  NtcssGetAppData
		(char   *app_data_buf,
         const int app_data_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppData
                                                (app_data_buf,
                                                 app_data_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppName
// _______________

extern "C" BOOL DllExport  NtcssGetAppName
		(char  *app_name_buf,
		 const int   app_name_buf_size)

{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppName 
                                              (app_name_buf,
                                               app_name_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppPassword
// ___________________

extern "C" BOOL DllExport NtcssGetAppPassword
		(char  *password_buf,
         const int password_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppPassword
                                                (password_buf,
                                                 password_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppUserInfo
// ___________________


extern "C" BOOL DllExport  NtcssGetAppUserInfo
		(const char            *user_name_str,
		 NtcssUserInfo         *user_info_rec_ptr,
         char                  *prog_access_info_buf,
         const int            prog_access_info_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppUserInfo
                                        (user_name_str,
                                         user_info_rec_ptr,
                                         prog_access_info_buf,
                                         prog_access_info_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppUserList
// ___________________

extern "C" int DllExport  NtcssGetAppUserList
		(NtcssAppUserList  *user_list_recs_ptr,
         const int user_list_recs_cnt)

{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppUserList
                                            (user_list_recs_ptr,
                                             user_list_recs_cnt));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           


/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppUserListCt
// ___________________

extern "C" int DllExport  NtcssGetAppUserListCt ()
	

{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetAppUserListCt ());
                                            
    }
    catch(...)
    {
        return(FALSE);
    }
    
}  
/////////////////////////////////////////////////////////////////////////////
// NtcssGetCommonData
// ___________________

extern "C" BOOL DllExport NtcssGetCommonData
		(LPCTSTR tag_str,
         LPTSTR common_data_buf,
         const int common_data_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetCommonData
                                           (tag_str, common_data_buf,
                                            common_data_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetDateTime
// __________________

extern "C" BOOL DllExport  NtcssGetDateTime
		(char   *datetime_buf,
         const int datetime_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetDateTime
                                                (datetime_buf,
                                                 datetime_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           


/////////////////////////////////////////////////////////////////////////////
// NtcssGetFile
// ___________________

extern "C" BOOL DllExport  NtcssGetFile
		(LPCTSTR ServerName,
		 LPCTSTR ServerFileName,
		 LPCTSTR ClientFileName,
		 const BOOL Ascii)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetFile
                                                (ServerName,
												 ServerFileName,
												 ClientFileName,
                                                 Ascii));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetFileInfo
// ___________________

extern "C" BOOL DllExport  NtcssGetFileInfo
		(char *ServerName,
		 char *ServerFileName,
		 int *LinesRequested,
		 int *BytesRequired)
	
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetFileInfo
                                                (ServerName,
												 ServerFileName,
												 LinesRequested,
                                                 BytesRequired));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssGetFileSection
// ___________________

extern "C" int DllExport  NtcssGetFileSection
		(char *ServerName,
		 char *ServerFileName,
		 char  *buf,
		 int  FirstLine,
		 int  LastLine)
	
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetFileSection
                                                (ServerName,
												 ServerFileName,
												 buf,
												 FirstLine,
                                                 LastLine));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetFilteredServerProcessList
// _________________________

extern "C" int DllExport  NtcssGetFilteredServerProcessList
		(NtcssProcessFilter *filter,
         NtcssServerProcessDetails *list_buf,
         const int list_buf_slot_cnt)

{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetServerProcessList
                             (filter->get_scheduled_jobs,
                              filter->get_unscheduled_jobs,
                              filter->get_only_running_jobs,
                              filter->get_for_current_user_only,
							  filter,
                              list_buf,
                              list_buf_slot_cnt));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           


/////////////////////////////////////////////////////////////////////////////
// NtcssGetFilteredServerProcessListCt
// _________________________

extern "C" int DllExport  NtcssGetFilteredServerProcessListCt
		(NtcssProcessFilter *filter)
         
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetServerProcessListCt
                             (filter->get_scheduled_jobs,
                              filter->get_unscheduled_jobs,
                              filter->get_only_running_jobs,
                              filter->get_for_current_user_only,
							  filter));
                              
    }
    catch(...)
    {
        return(FALSE);
    }
    
} 
 
/////////////////////////////////////////////////////////////////////////////
// NtcssGetHostName
// ________________

extern "C" int DllExport NtcssGetHostName
		(char   *host_name_buf,
         const int host_name_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetHostName
                                                (host_name_buf,
                                                 host_name_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetLastError
// _________________

extern "C" BOOL DllExport  NtcssGetLastError
		(char   *err_buf,
         const int err_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetLastError
                                                    (err_buf,
                                                     err_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           


/////////////////////////////////////////////////////////////////////////////
// NtcssGetLinkData
// ___________________

extern "C" BOOL DllExport NtcssGetLinkData
		(char  *password_buf,
         const int password_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetLinkData
                                                (password_buf,
                                                 password_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           


/////////////////////////////////////////////////////////////////////////////
// NtcssGetLocalIP
// __________________

extern "C" BOOL DllExport  NtcssGetLocalIP
		(char   *ip_addr_buf,
         const int ip_addr_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetLocalIP
                                            (ip_addr_buf,
                                             ip_addr_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetServerName
// __________________

extern "C" BOOL DllExport  NtcssGetServerName
		(char  *server_name_buf,
         const int server_name_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetServerName
                                           (server_name_buf,
                                            server_name_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           




/////////////////////////////////////////////////////////////////////////////
// NtcssGetServerProcessList
// _________________________

extern "C" int DllExport  NtcssGetServerProcessList
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only,
         NtcssServerProcessDetails *list_buf,
         const list_buf_slot_cnt)

{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetServerProcessList
                             (get_scheduled_jobs,
                              get_unscheduled_jobs,
                              get_only_running_jobs,
                              get_for_current_user_only,
							  NULL,
                              list_buf,
                              list_buf_slot_cnt));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetServerProcessListCt
// _________________________

extern "C" int DllExport  NtcssGetServerProcessListCt
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only)
         
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetServerProcessListCt
                             (get_scheduled_jobs,
                              get_unscheduled_jobs,
                              get_only_running_jobs,
                              get_for_current_user_only,
							  NULL));
                              
    }
    catch(...)
    {
        return(FALSE);
    }
    
}           

/////////////////////////////////////////////////////////////////////////////
// NtcssGetUserInfo
// ________________		   

extern "C" BOOL DllExport  NtcssGetUserInfo
		(NtcssUserInfo  *user_info_ptr)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_GetUserInfo
                                                        (user_info_ptr));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           




/////////////////////////////////////////////////////////////////////////////
// NtcssPostAppMessage
// _______________________

extern "C" BOOL DllExport  NtcssPostAppMessage
		(const char  *msg_title,
         const char  *msg_str)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PostAppMessage
                                            (msg_title,
                                             msg_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssPostMessage
// _______________________

extern "C" BOOL DllExport  NtcssPostMessage
		(const char  *msg_board_str,
		 const char  *msg_title,
         const char  *msg_str)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PostMessage
                                            (msg_board_str,
											 msg_title,
                                             msg_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssPostSystemMessage
// _______________________

extern "C" BOOL DllExport  NtcssPostSystemMessage
		(const char  *msg_title,
         const char  *msg_str)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PostSystemMessage
                                            (msg_title,
                                             msg_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssPrintClientFile
// _______________________

extern "C" BOOL DllExport  NtcssPrintClientFile
		(const char  *file_name_str,
         const int  seclvl, 
         const char  *output_type_str)
{
    try
    {

	        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PrintClientFile
                                            (file_name_str,
                                             seclvl,
                                             output_type_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssPrintServerFile
// _______________________

extern "C" BOOL DllExport  NtcssPrintServerFile
		(const char   *server_name_str,
         const int    seclvl, 
         const char   *output_type_str,
         const char   *dir_name_str,
		 const int   copies,
		 const char *options)

{
	

    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PrintServerFile
                                            (server_name_str,
                                             seclvl,
                                             output_type_str,
                                             dir_name_str,
											 copies,
											 options));
    }

    catch(...)
    {
        return(FALSE);
    }
    
}                           

//!!!!!!!!!!!!!!!!!!!!
/////////////////////////////////////////////////////////////////////////////
// NtcssPutFile
// ___________________

extern "C" BOOL DllExport  NtcssPutFile
		(const char  *ServerName,
		 const char  *ServerFileName,
		 const char  *ClientFileName,
		 const BOOL Ascii)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PutFile
                                                (ServerName,
												 ServerFileName,
												 ClientFileName,
                                                 Ascii)); 
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPutFileAppend
// ___________________

extern "C" BOOL DllExport  NtcssPutFileAppend
		(LPCSTR ServerName,
		 LPCSTR ServerFileName,
		 LPCSTR ClientFileName,
		 const BOOL Ascii)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_PutFile
                                                (ServerName,
												 ServerFileName,
												 ClientFileName,
                                                 Ascii,TRUE));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}

//!!!!!!!!!!!!!!!!!!

/////////////////////////////////////////////////////////////////////////////
// NtcssRegisterAppUser
// ____________________

extern "C" BOOL DllExport  NtcssRegisterAppUser
		(const char   *user_name_str,
         const BOOL  do_register)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_RegisterAppUser
                                                (user_name_str,
                                                 do_register));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssRegisterSecurityClass
// __________________________

extern "C" BOOL DllExport  NtcssRegisterSecurityClass
		(const HANDLE hInstance,
         const int seclvl)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_RegisterSecurityClass
                                                        (hInstance, 
                                                         seclvl));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssRemoveSchedServerProcess
// _____________________________

extern "C" BOOL DllExport NtcssRemoveServerProcess
		(const char  *pid_buf,
         const BOOL scheduled_job)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_RemoveSchedServerProcess
                                               (pid_buf,
                                                scheduled_job)); 
    }
    catch(...)
    {
        return(FALSE);
    }
    
}




/////////////////////////////////////////////////////////////////////////////
// NtcssRestorePrinter
// ___________________

extern "C" BOOL DllExport  NtcssRestorePrinter()
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_RestorePrinter());
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssScheduleServerProcess
// __________________________

extern "C" BOOL DllExport NtcssScheduleServerProcess 
		(const char  *schedule_str,
		 NtcssServerProcessDescr  *descr_ptr,
         char  *pid_buf,
         const int pid_buf_size)
{
    try
    {
		
        return(gbl_dll_man.dllCallMan()->NtcssAPI_ScheduleServerProcess
                                    (schedule_str,
                                     descr_ptr, 
                                     pid_buf, 
                                     pid_buf_size));
    }
    catch(...)
    {
		AfxMessageBox("Caught exception in Entries");
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssSelectPrinter
// _______________________

extern "C" HDC DllExport  NtcssSelectPrinter
		(const char          *output_type_str,
         const int           seclvl,
         const BOOL          default_flag,
         const BOOL          gen_access_flag,
         const BOOL          return_DC_flag,
         const NtcssPrintAttrib   *pr_attr_ptr,
         const BOOL          local_enable_flag,
         const BOOL          remote_enable_flag)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_SelectPrinter
                                        (output_type_str,
                                         seclvl,
                                         default_flag,
                                         gen_access_flag,
                                         return_DC_flag,
                                         pr_attr_ptr,
                                         local_enable_flag,
                                         remote_enable_flag));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}       


extern "C" int DllExport NtcssSelectServerFile(LPCTSTR lpszServer,LPCTSTR lpszDirectory,
												int buffSize, LPTSTR lpszFileName)
{
	try
	{
		 return gbl_dll_man.dllCallMan()->NtcssSelectServerFile(lpszServer,lpszDirectory,
			                              buffSize,lpszFileName);
		 
	}

	 catch(...)
    {
        return(-1);
    }
}





/////////////////////////////////////////////////////////////////////////////
// NtcssSetAppData
// _______________

extern "C" BOOL DllExport  NtcssSetAppData
		(const char  *app_data_str)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_SetAppData(app_data_str));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssSetLinkData
// _______________

extern "C" BOOL DllExport  NtcssSetLinkData
		(const char  *link_data)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_SetLinkData(link_data));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}
/////////////////////////////////////////////////////////////////////////////
// NtcssSpoolPrintRequest
// _______________________

extern "C" BOOL DllExport  NtcssSpoolPrintRequest()
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_SpoolPrintRequest());
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           




/////////////////////////////////////////////////////////////////////////////
// NtcssStartServerProcess
// _______________________

extern "C" BOOL DllExport NtcssStartServerProcess 
		(NtcssServerProcessDescr  *descr_ptr,
         char  *pid_buf,
         const int pid_buf_size)
{
    try
    {

  

        return(gbl_dll_man.dllCallMan()->NtcssAPI_StartServerProcess
                                    (descr_ptr, 
                                     pid_buf, 
                                     pid_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssStopServerProcess
// ______________________

extern "C" BOOL DllExport NtcssStopServerProcess
		(const char  *pid_str,
         const int signal_type)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_StopServerProcess
                                               (pid_str,
                                                signal_type)); 
    }
    catch(...)
    {
        return(FALSE);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssValidateUser
// ______________________

extern "C" BOOL DllExport NtcssValidateUser(char *ssn,int ssn_name_buf_size)
	
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssAPI_ValidateUser(ssn,ssn_name_buf_size));
                                                
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssViewServerFile
// ___________________

extern "C" void DllExport  NtcssViewServerFile
		(LPCTSTR ServerName, LPCTSTR Directory)


	
{
    try
    {
                                
          gbl_dll_man.dllCallMan()->NtcssAPI_ViewServerFile
                                                ((CString)ServerName,
												 (CString)Directory);
											
    }
    catch(...)
    {
        ;
    }
    
}


//***************************************************************************
//***************************************************************************
// (2) NTCSS DeskTop API's (private)
//***************************************************************************
//***************************************************************************

/////////////////////////////////////////////////////////////////////////////
// Ntcss_ClearLRS
// ______________

extern "C" int DllExport  Ntcss_ClearLRS ()                                
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssPRV_ClearLRS());
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssIsPrintProcessAlive
// _________________          

extern "C" void DllExport  NtcssIsPrintProcessAlive()
{
    try
    {
		gbl_dll_man.dllCallMan()->NtcssPRV_IsPrintProcessAlive();
        return;
    }
    catch(...)
    {
        return;
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssPortDelete
// _________________          

extern "C" BOOL DllExport  NtcssPortDelete()
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssPRV_PortDelete());
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssDTInitialize
// _________________          

extern "C" int DllExport  NtcssDTInitialize(char *version)
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssPRV_DTInitialize(version));
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    //
}



/////////////////////////////////////////////////////////////////////////////
// NtcssDTReset
// ____________

extern "C" int DllExport  NtcssDTReset(char *version)
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssPRV_DTReset(version));
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    
}



/////////////////////////////////////////////////////////////////////////////
// NtcssGetToken
// _____________

extern "C" int DllExport  NtcssGetToken(char  *token)
{
    try
    {
        return(gbl_dll_man.dllCallMan()->NtcssPRV_GetToken(token));
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// Ntcss_SetLRS
// ____________

extern "C" int DllExport  Ntcss_SetLRS (void  *desktop_lrs_data)                                
{

   
    SshmemLoginReplyStruct *desk_top_data = 
                                    (SshmemLoginReplyStruct *)desktop_lrs_data;
    try
    {
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_SetLRS(desktop_lrs_data));
    }
    catch(...)
    {
        return(DESKTOP_INT_ERROR);
    }
    
}




//***************************************************************************
//***************************************************************************
// (3) NTCSS Secban API's (private)
//***************************************************************************
//***************************************************************************



/////////////////////////////////////////////////////////////////////////////
// NtcssCurrentSecurityLevel
// _________________________

extern "C" int DllExport  NtcssCurrentSecurityLevel()
{
    try
    {
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_CurrentSecurityLevel());
    }
    catch(...)
    {
        return(0);
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssReserveSessionTableSpace
// _____________________________

extern "C" int DllExport  NtcssReserveSessionTableSpace()
{
    try
    {
        return
            (gbl_dll_man.dllCallMan()-> //...
                            NtcssPRV_ReserveSessionTableSpace());
    }
    catch(...)
    {
        return(0);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssSecBanInitialize
// _____________________

extern "C" BOOL DllExport  NtcssSecBanInitialize(char  *version)
{
    try
    {
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_SecBanInitialize
                                                        (version));
    }
    catch(...)
    {
        return(TRUE);
    }
    
} 



//***************************************************************************
//***************************************************************************
// (4) NTCSS Applicaction API's (private)
//***************************************************************************
//***************************************************************************


/////////////////////////////////////////////////////////////////////////////
// NtcssAdminLockOptions
// _____________________
//

extern "C" BOOL DllExport  NtcssAdminLockOptions(const char *appname,
					       const char *code,const char *action,char *pid,
						   const char *hostname)

{
    try
    {
	
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_AdminLockOptions
									   (appname,code,action,pid,hostname));
		
		
    }
    catch(...)
    {
        return(0);
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssDoMessage
// ________________
//
// NtcssDoMessage parameters and NtcssPRV_DoMessage invocation is
// dependent on setting of _NTCSS_MSG_TRACING_, i.e. two different
// versions of this API

#ifdef _NTCSS_MSG_TRACING_

extern "C" BOOL DllExport  NtcssDoMessage(SDllMsgRec  *msg,
										  BOOL trace_this_msg)
#else

extern "C" BOOL DllExport  NtcssDoMessage(SDllMsgRec  *msg)

#endif // _NTCSS_MSG_TRACING_


{
    try
    {
#ifdef _NTCSS_MSG_TRACING_
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_DoMessage
											   (msg,
												trace_this_msg));
#else
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_DoMessage(msg));
#endif

    }
    catch(...)
    {
        return(0);
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssDoMessageCleanup
// _____________________
//

extern "C" BOOL DllExport  NtcssDoMessageCleanup(const SDllMsgRec *msg)

{
    try
    {
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_DoMessageCleanup
															(msg));
    }
    catch(...)
    {
        return(0);
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppHost
// _______________

extern "C" BOOL DllExport  NtcssGetAppHost
									(const char  *app_name_str,
									 char  *host_buf,
									 int host_buf_size)
{
  
		char env_app_name_str[SIZE_GROUP_TITLE+1];

		int env_ok=GetEnvironmentVariable("APPNAME",env_app_name_str,SIZE_GROUP_TITLE+1);

		if (env_ok == 0 || !strcmp(env_app_name_str,"NTCSS"))
			strcpy(env_app_name_str,app_name_str);


        return    
			
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetAppHost
                                            (env_app_name_str, 
											 host_buf,
											 host_buf_size));
 
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppNameList
// ___________________

extern "C" BOOL DllExport  NtcssGetAppNameList(char   *app_list,
                                               int    size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetAppNameList
                                            (app_list, size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppRole
// ___________________

extern "C" int DllExport  NtcssGetAppRole(char  *lpszAppName)
{
    try                    
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetAppRole(lpszAppName));
    }
    catch(...)
    {
        return(RETURN_INT_ERROR);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppPath
// ___________________

extern "C" BOOL DllExport NtcssGetAppPath(LPCTSTR strAppname,LPTSTR strPath,DWORD dwLen)
{

	CRegKey reg;
	BOOL bRet=FALSE;

	CString csPath=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\APP PATHS\\");
	csPath +=strAppname;

	::ZeroMemory(strPath,dwLen);

    try                    
    {
		
		if((reg.Open(HKEY_LOCAL_MACHINE,(LPCTSTR)csPath))!=ERROR_SUCCESS)
			throw(_T("Couldn't open registry key"));

		if((reg.QueryValue(strPath,_T("Path"),&dwLen))!=ERROR_SUCCESS)
		{
			::ZeroMemory(strPath,dwLen);
			bRet=FALSE;
		}
		else
			bRet=TRUE;
	
		reg.Close();
	
        return bRet;                    
 
    }

	catch(LPTSTR pLpr)
	{   
		reg.Close();
		TRACE (pLpr);
		return FALSE;
	}

    catch(...)
    {
        return FALSE;
    }
    
}





/////////////////////////////////////////////////////////////////////////////
// NtcssGetNTDomainName
// ___________________

extern "C" int DllExport  NtcssGetNTDomainName(char* DomainName,LPDWORD cchDomainName)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetNTDomainName(DomainName,
			                                               cchDomainName));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetDllVersion
// ___________________

extern "C" int DllExport  NtcssGetDllVersion(char  *szversion)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetDllVersion
                                                        (szversion));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}                           



/////////////////////////////////////////////////////////////////////////////
// NtcssGetNumTasks
// ________________

extern "C" int DllExport  NtcssGetNumTasks(void)
{
    try
    {
        return
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetNumTasks());
    }
    catch(...)
    {
        return(0);
    }
    
} 

/////////////////////////////////////////////////////////////////////////////
// UpdateDTSessionValues
// _____________________
									
extern "C" void DllExport NtcssUpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,const DWORD nNewProcessID,
											   const DWORD nNewThreadID)
{
    try
    {
            gbl_dll_man.dllCallMan()->NtcssPRV_UpdateDTSessionValues(OldhWnd,NewhWnd,
				                                                     nNewProcessID,nNewThreadID);
    }
    catch(...)
    {
        
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssGetUserRole
// ________________

extern "C" int DllExport  NtcssGetUserRole(void)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetUserRole());
    }
    catch(...)
    {
        return(RETURN_INT_ERROR);
    }
    
}                           

/////////////////////////////////////////////////////////////////////////////
// NtcssKillProcess
// _____________________
//

extern "C" void DllExport  NtcssKillProcesses()

{
    try
    {
        
       gbl_dll_man.dllCallMan()->NtcssPRV_KillProcesses();
									  
    }
    catch(...)
    {
        return;
    }
    
} 



/////////////////////////////////////////////////////////////////////////////
// NtcssSetCommonData
// __________________

extern "C" BOOL DllExport  NtcssSetCommonData
                                    (char  *lpszTag, char  *lpszData)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_SetCommonData
                                             (lpszTag, lpszData));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssScheduleServerProcessEX
// __________________________

extern "C" BOOL DllExport NtcssScheduleServerProcessEx 
		(const char  *app_name_str,
		 const char  *schedule_str,
		 NtcssServerProcessDescr  *descr_ptr,
         char  *pid_buf,
         const int pid_buf_size)
{
    try
    {

	
        return(gbl_dll_man.dllCallMan()->NtcssPRV_ScheduleServerProcessEx
                                    (app_name_str,
									 schedule_str,
                                     descr_ptr, 
                                     pid_buf, 
                                     pid_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssSetAuthServer
// ___________________

extern "C" BOOL DllExport  NtcssSetAuthServer(LPCTSTR lpszAuthServer)
{
	CRegKey reg;

    try                    
    {
		if(!gbl_dll_man.setServer(lpszAuthServer))
			throw(_T("Error setting Authentication Server"));

	//	if(reg.Create(HKEY_CURRENT_USER,"Software\\SPAWARSYSCEN\\Server")!=ERROR_SUCCESS)
		//	throw(_T("Couldn't create registry key"));

	//if(reg.SetValue(lpszAuthServer,"ServerName")!=ERROR_SUCCESS)
		//	throw(_T("Couldn't set registry value"));
//
//		reg.Close();
	
        return TRUE;                    
 
    }

	catch(LPTSTR pLpr)
	{   
		reg.Close();
		AfxMessageBox(pLpr);
		return FALSE;
	}

    catch(...)
    {
        return FALSE;
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssStartServerProcessEx
// _________________________

extern "C" BOOL DllExport NtcssStartServerProcessEx
				   (const char  *app_name_str,
					NtcssServerProcessDescr  *descr_ptr,
					char   *pid_buf,
					const int pid_buf_size)
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_StartServerProcessEx
				   (app_name_str,
					descr_ptr,
					pid_buf,
					pid_buf_size));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetMasterName
// _________________________

extern "C" BOOL DllExport NtcssGetMasterName
				   (char   *strMasterName)
{
    try
    {
		// ::DebugBreak(); sdk breakpoint
	    return   
	(gbl_dll_man.dllCallMan()->NtcssPRV_GetMasterName(strMasterName));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// NtcssIsAuthServer
// _________________________

extern "C" BOOL DllExport NtcssIsAuthServer
				   (const char *lpszGroupTitle)
{
    try
    {
	
        return                        
            gbl_dll_man.dllCallMan()->NtcssPRV_IsAuthServer(lpszGroupTitle);
    }
    catch(...)
    {
        return FALSE;
    }
    
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetAuthenticationServer
// _________________________

extern "C" BOOL DllExport NtcssGetAuthenticationServer(const char* lpszApp, char* lpszHost)
{
	try 
	{
		  return                        
            gbl_dll_man.dllCallMan()->NtcssPRV_GetAuthenticationServer(lpszApp,lpszHost);
	}
	catch(...)
    {
        return FALSE;
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssIsAdmin
// _________________________

extern "C" BOOL DllExport NtcssIsAdmin()
{
	try 
	{
		  return                        
            gbl_dll_man.dllCallMan()->NtcssPRV_IsAdmin();
	}
	catch(...)
    {
        return FALSE;
    }
}


/////////////////////////////////////////////////////////////////////////////
// NtcssLoginName
// _________________________

extern "C" BOOL DllExport NtcssLoginName(LPSTR lpszLoginName)
{
	try
	{
		return gbl_dll_man.dllCallMan()->NtcssPRV_GetLoginName(lpszLoginName);
	}
	catch(...)
	{
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssEncrypt
// _________________________

extern "C" BOOL DllExport NtcssEncrypt(LPCTSTR lpcszKey,LPCTSTR lpcszString, LPTSTR lpszResult)
{
	//mainly used for desktop right now

	try
	{
		CString strResult;
		CEncrypt crypt;
		strResult=crypt.plainEncryptString(lpcszKey,lpcszString);
		strcpy(lpszResult,strResult);
		return !strResult.IsEmpty();
				
	}
	catch(...)
	{
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// NtcssDecrypt
// _________________________

extern "C" BOOL DllExport NtcssDecrypt(LPCTSTR lpcszKey,LPCTSTR lpcszString, LPTSTR lpszResult)
{
	//mainly used for desktop right now

	try
	{
		CString strResult;
		CEncrypt crypt;
		strResult=crypt.plainDecryptString(lpcszKey,lpcszString);
		strcpy(lpszResult,strResult);
		return !strResult.IsEmpty();
				
	}
	catch(...)
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// NtcssGetAppNameList
// ___________________

extern "C" BOOL DllExport  NtcssGetHostNameList(LPTSTR pList, int nSize)
                                         
{
    try
    {
        return                        
            (gbl_dll_man.dllCallMan()->NtcssPRV_GetHostNameList(pList, nSize));
    }
    catch(...)
    {
        return(FALSE);
    }
    
}