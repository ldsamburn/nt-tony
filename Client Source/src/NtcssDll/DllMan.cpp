/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllMan.cpp                    
//                                         
//----------------------------------------------------------------------

#include "stdafx.h"                   

#define DEFINE_THE_DLL_MAN             // Define the only CdllManager

#include "0_basics.h"

#include "msgcore.h"
#include "msgdefs.h"
#include "PrintUtil.h"
#include "privapis.h"



//***************************************************************************
// After constructor/destructor functions listed in alphabetical order.
//***************************************************************************


/////////////////////////////////////////////////////////////////////////////
// CdllManager constructor, destructor
// ___________________________________

CdllManager::CdllManager()
{
    m_pShmemDllManData = NULL;

    m_eThisAppType     = NORMAL_APP;
    m_bInitOk          = FALSE;
}



CdllManager::~CdllManager()
{
	if ((m_bInitOk) && (dllSpoolPrintMan() != NULL))
	{
	    delete m_pShmemDllManData->pPrintSpoolMan;

		m_pShmemDllManData->pPrintSpoolMan = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// APPcall_dismissThisDllInstance
// ______________________________
//
// This function must be called by the CNtcssDLL when an application
// is exiting in order that the session table can be cleared of any
// session belonging to the CNtcssDLL.  Also the Secban application
// is notified of the changes in order that it can change the banner
// settings if necessary as a result of this application exiting.
//
// NEEDTODO - need lock here

BOOL CdllManager::APPcall_dismissThisDllInstance()
{
    DWORD current_id;
	
    current_id = GetCurrentProcessId();

    
									   // No need to process these
									   //
	if ((current_id == NULL) || (!initializedOK()))
        return(FALSE);
									   // Unlock print spooling if need be
    if (sessionHasSpoolPrintLock())
        sessionSpoolPrintUnLock();
        
    SntcssSessionTableEntry *entry_ptr;

									   // Look at each entry in session
									   //  table to see if the session
									   //  belongs to this process.  If
									   //  so then free it
									   //

    for (int i=0; i < SESSIONS_MAX_ACTIVE_CNT; i++) 
    {
        entry_ptr = &(m_pShmemDllManData->pSessionTable[i]);
	
        if (entry_ptr->nProcessID == current_id)

        {
           
			delete entry_ptr->pSession;

            entry_ptr->nProcessID = 0;
            entry_ptr->nThreadID = 0;

            entry_ptr->eCurSecLvl = NTCSS_UNCLASSIFIED;
            entry_ptr->pSession = NULL;
            entry_ptr->hwnd = NULL;
        }
    }
									   // Now notify the Secban app
									   //  that there has been a change
									   //  in the session table
									   //
    return (pingOnSecBan(SECBAN_TOCHK_MSG) != RETURN_INT_ERROR);
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_activeProcessCnt
// _______________________

int CdllManager::CMcall_activeProcessCnt()
{
	//called by DT only
    cleanUpSessionTable(); 

    int cnt = 0;

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
        if (m_pShmemDllManData->pSessionTable[idx].nProcessID
            != NULL)

        {
            cnt++;
        }
    }
    return(cnt);
}

/////////////////////////////////////////////////////////////////////////////
// CMcall_KillActiveProcesses
// _______________________

void CdllManager::CMcall_KillActiveProcesses()
{

	HANDLE hProc,ThreadHandle;
    cleanUpSessionTable();

    for (int idx=2; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
        if (m_pShmemDllManData->pSessionTable[idx].nProcessID
            != NULL)

		{
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 
				m_pShmemDllManData->pSessionTable[idx].nProcessID);
			if (hProc != NULL)
			{
				TRACE1(_T("KillActiveProcesses is trying to kill process # %d\n"),
				m_pShmemDllManData->pSessionTable[idx].nProcessID);
				ThreadHandle=::CreateRemoteThread(hProc,NULL,0,
						     (LPTHREAD_START_ROUTINE)(GetProcAddress(GetModuleHandle
							 ("KERNEL32.DLL"),"ExitProcess")),
							 NULL,0,NULL);


				if (ThreadHandle!=NULL)
				   WaitForSingleObject(ThreadHandle,INFINITE);
				//PostMessage(m_pShmemDllManData->pSessionTable[idx].hwnd,WM_QUIT,0L,0L);

			}
		}
}
/////////////////////////////////////////////////////////////////////////////
// CMcall_deskTopInit
// __________________
//
// This function does special processing required in order for DLL
// to come up properly.  It is called by the DeskTop application via
// an NTCSS DLL entry, NtcssDTInitialize.

BOOL CdllManager::CMcall_deskTopInit(char *lpszVersion)
{
                                       // Make sure haven't already
                                       //
    if (m_pShmemDllManData != NULL)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Improper DeskTop re-initialization");

        return(FALSE);
    }

   
                                       // Validate version
                                       //
   
    if (!gbl_dll_man.rawInitializations(TRUE))
    {
        // if bad then RecordError already done
        return FALSE;
    }
       
									   // This is now the DESKTOP_APP
    m_eThisAppType = DESKTOP_APP;

									   // Init session table entry
									   //
    SntcssSessionTableEntry  *desktop_session_entry =
        &(m_pShmemDllManData->pSessionTable[SESSION_IDX_DESKTOP]);

    desktop_session_entry->hwnd = (HWND) NTCSSVAL_IN_PROGRESS_CODE;
    desktop_session_entry->pSession = NULL;

    desktop_session_entry->hwnd = NULL;

                                       //   Put lpszDllVersionNum so
                                       //   no other dll versions come
                                       //   on line
                                       //
    strcpy(m_pShmemDllManData->lpszDllVersionNum,
           NTCSS_DLL_VERSION);

	/****** 6/18 *****/
	CPrintUtil PU;
	PU.DelTempPrinter();
    //try to delete port & printer if it exists
	//from power outage or some other disaster
	/*****************/
            
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_deskTopReset
// ___________________

BOOL CdllManager::CMcall_deskTopReset()
{
    m_pShmemDllManData->bDeskTopInitFlag = FALSE;

    return(TRUE);
};



/////////////////////////////////////////////////////////////////////////////
// CMcall_sessionGetSpoolPrintLock
// _______________________________
//
// If nobody currently has a CdllSpoolPrintManager active then one
// is created, m_pShmemDllManData is set to reflect this and TRUE is
// returned, otherwise FALSE, another app is already is doing print
// spooling.
//  
// NEEDTODO - need real semaphore here

BOOL CdllManager::CMcall_sessionGetSpoolPrintLock(Csession *session)
{

    if (m_pShmemDllManData->nPrintLockerID == NULL)
    {
        m_pShmemDllManData->nPrintLockerID = GetCurrentProcessId();

        m_pShmemDllManData->pPrintSpoolMan = 
                        new CdllSpoolPrintManager(session);

        if (m_pShmemDllManData->pPrintSpoolMan != NULL)
            return(TRUE);

        else
            m_pShmemDllManData->nPrintLockerID = NULL;
    }
                                       // If another session has lock
                                       //  or if can't create the
                                       //  print mgr then not ok

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CdllManager::cleanUpSessionTable
// ________________________________
//
// Runs through session table and checking that each active process
// has a valid hwnd.  If one is found that doesn't then its session
// table entry data is cleared.

void CdllManager::cleanUpSessionTable()
{
    int cnt = 0;

    SntcssSessionTableEntry *entry_ptr;

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
        entry_ptr = &(m_pShmemDllManData->pSessionTable[idx]);

        if (entry_ptr->nProcessID != NULL)

        {
            if (!IsWindow(entry_ptr->hwnd))
            {
                entry_ptr->nProcessID = 0;
                entry_ptr->nThreadID = 0;
                entry_ptr->eCurSecLvl = NTCSS_UNCLASSIFIED;
                entry_ptr->pSession = NULL;
                entry_ptr->hwnd = NULL;
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// CdllManager::IsPrintProcessAlive
// ________________________________
//
// Called by SECBAN via a timer to check if a process with print lock was 
// killed in a manner that bypasses exit instance (eg. task manager) if so
// table entry data is cleared & printlock is released & temp printer & port
// are deleted

void CdllManager::IsPrintProcessAlive()
{
    int cnt = 0;

    SntcssSessionTableEntry *entry_ptr;

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
        entry_ptr = &(m_pShmemDllManData->pSessionTable[idx]);

        if (entry_ptr->nProcessID != NULL)

        {
            if (!IsWindow(entry_ptr->hwnd)&&m_pShmemDllManData->nPrintLockerID ==
            entry_ptr->nProcessID)
            {     
			
                entry_ptr->nProcessID = 0;
                entry_ptr->nThreadID = 0;
                entry_ptr->eCurSecLvl = NTCSS_UNCLASSIFIED;
                entry_ptr->pSession = NULL;
                entry_ptr->hwnd = NULL;
				CPrintUtil PU;
				PU.DelTempPrinter();
				PU.DelNTCSSPort();
				if(m_pShmemDllManData->pPrintSpoolMan)
				{
				//	delete m_pShmemDllManData->pPrintSpoolMan;
					m_pShmemDllManData->pPrintSpoolMan=NULL;
					m_pShmemDllManData->nPrintLockerID = NULL;
				}
				break;

            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////
// CMcall_getSessionReadyForApiAction
// __________________________________
//
// This function called before every API call.  Does validation and
// gets Csession ready for action.
//
// NEEDTODO - may want to cut down on some of the validation for
//           'special' apps (e.g. DeskTop).

Csession *CdllManager::CMcall_getSessionReadyForApiAction
											(BOOL bResetError) 
{
    if (bResetError)
        gbl_dll_man.dllErrorMan()->ClearError();

                                       // First make sure that 
                                       //  m_pShmemDllManData
                                       //  is valid
                                       //
    if (!validateShmemCoreData())
            return(NULL);
                                       // Check user to make sure is ok
    if (!validateUser() )
        return(NULL);
                                        // Now make sure DeskTop is
                                        //  initialized ok
                                        //
    if (!validateDeskTopInitialized())
        return(NULL);
                                       // Now check that this 
                                       //  application has done
                                       //  required initializations
    if (!validateAppInitialized())
        return(NULL);
                                       // Everything checked out ok,
                                       //  now get session - set it up
                                       //  if need be
                                       //
    Csession *session = findOrMakeSession();

    if (session == NULL)
        return(NULL);

    if (!session->DMcall_reserve())
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Session is busy");
        return(NULL);
    }

                                       // Everything okay, now setup
                                       //  the session
    if (bResetError)
        session->ssnErrorMan()->ClearError();

  
                                       // Everything okay
    return(session);
}


/////////////////////////////////////////////////////////////////////////////
// CMcall_overallSecurityLevel
// ___________________________
//
// Runs through session table and finds entry with highest security
// level setting.

SecurityLevel CdllManager::CMcall_overallSecurityLevel()
{
	SecurityLevel highest_so_far = NTCSS_UNCLASSIFIED;
    SntcssSessionTableEntry *entry_ptr;

    for (int i=0; i < SESSIONS_MAX_ACTIVE_CNT; i++) 
    {
        entry_ptr = &(m_pShmemDllManData->pSessionTable[i]);

        if ( (entry_ptr->nProcessID != NULL) &&
             (entry_ptr->eCurSecLvl > highest_so_far) )
        {
            highest_so_far = entry_ptr->eCurSecLvl;
        }
    }

    return(highest_so_far);
}


/////////////////////////////////////////////////////////////////////////////
// CMcall_secbanInit
// _________________
//

BOOL CdllManager::CMcall_secbanInit(char *lpszVersion)
{
                                       // Validate version
                                       //
    if (strcmp(lpszVersion,THE_SECBAN_VERSION) != 0)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Incompatible NTCSS DLL/Security Banner versions");

        return(FALSE);
    }

    m_eThisAppType = SECBAN_APP;

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_sessionCanPrint
// ______________________
//
// This function used to keep the current application from doing
// to do NTCSS DLL dialog printing if the current application is in the
// middle of doing print spooling.

BOOL CdllManager::CMcall_appCanDoPrintDlg()
{
    if (m_pShmemDllManData->nPrintLockerID
        != GetCurrentProcessId())

        return(TRUE);

    else
        return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_setInitializedOk
// _______________________

void CdllManager::CMcall_setInitializedOk()
{
    m_bInitOk = TRUE;

    if (thisIsDeskTopDll())

        m_pShmemDllManData->bDeskTopInitFlag = TRUE;

    else if (thisIsSecbanDll())

        m_pShmemDllManData->bSecbanInitFlag = TRUE;  
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_setSessionSecLvl
// _______________________
//
// Sets security level in session table entry and then notifies the
// Security Banner of the change.

BOOL CdllManager::CMcall_setSessionSecLvl(Csession *session, 
                                          SecurityLevel seclvl)
{
    SntcssSessionTableEntry *entry_ptr =
        &(m_pShmemDllManData->pSessionTable[session->ID()]);

    entry_ptr->eCurSecLvl = seclvl;

    return (pingOnSecBan(SECBAN_TOCHK_MSG) != RETURN_INT_ERROR);
}



/////////////////////////////////////////////////////////////////////////////
// CMcall_thunkerInit
// __________________

BOOL CdllManager::CMcall_thunkerInit(char *lpszVersion)
{
                                       // Validate version
                                       //
    if (strcmp(lpszVersion,THE_THUNKER_VERSION) != 0)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Incompatible NTCSS DLL/Thunker versions");

        return(FALSE);
    }

    m_eThisAppType = THUNKER_APP;

    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// dllFacilityName
// _______________

CString CdllManager::dllFacilityName()
{
    CString tmp = NTCSS_DLL_NAME + '(';
    
    tmp += NTCSS_DLL_VERSION;
    tmp += " : ";
    tmp += THE_BUILD_NUM;
    tmp += ')';

    return(tmp);
}



/////////////////////////////////////////////////////////////////////////////
// dllSpoolPrintMan
// ________________
//
// Returns pointer to a CdllSpoolPrintManager but only if the current
// process has the lock on it.

CdllSpoolPrintManager *CdllManager::dllSpoolPrintMan()
{
    if (initializedOK())
    {
	    DWORD current_id;

		current_id = (DWORD) GetCurrentProcessId();

									   // If nPrintLockerID set for
									   //  current process then okay
									   //
        if (m_pShmemDllManData->nPrintLockerID ==
            current_id)

        {
            return(m_pShmemDllManData->pPrintSpoolMan);
        }
    }
    return(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CdllManager::findOrMakeSession
// ______________________________

Csession *CdllManager::findOrMakeSession() 
{
    SecurityLevel highest_level = NTCSS_UNCLASSIFIED;
    int  first_process_match_idx = -1;

    DWORD  current_thread_id = GetCurrentThreadId();
    DWORD  current_process_id = GetCurrentProcessId();

                                       // This loop runs through the
                                       //   pSessionTable in shared
                                       //   memory looking for a 
                                       //   matching process/thread, or
                                       //   for an empty slot 
                                       //
	TRACE2(_T("In CdllManager::findOrMakeSession ThreadID-> %d PID->%d\n"),
		      current_thread_id,current_process_id);

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
                                       // Process ID match?
                                       //
        if (m_pShmemDllManData-> //...
                                pSessionTable[idx].nProcessID
            == current_process_id)
        {
			
                                       // Thread ID match??
                                       //
            if (m_pShmemDllManData-> //...
                                pSessionTable[idx].nThreadID
                == current_thread_id)
            {
                                       // Found a match, return ptr
                                       //  to the session
                                       //
			

                return(m_pShmemDllManData-> //...
                                pSessionTable[idx].pSession);
            }
                                       // Else this is a thread of
                                       //  the current process, will
                                       //  save info in case have to
                                       //  make a new session
            else
            {
				TRACE2(_T("Thread %d is a thread of existing process %d\n"),
					current_thread_id,current_process_id);
                                       // 
                if (first_process_match_idx == -1)
                    first_process_match_idx = idx;

                if (m_pShmemDllManData-> //...
                                pSessionTable[idx].eCurSecLvl
                    > highest_level)
                {
                    highest_level = m_pShmemDllManData-> //...
                                      pSessionTable[idx].eCurSecLvl;
                }
            }
        }
    }
 									   // Should always be at least
									   //  one session of this process,
									   //  otherwise an error
    if (first_process_match_idx == -1)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Unable to locate session table entry");
        return(NULL);
    }
                                       // Didn't find the session, must be
									   //  a new thread so will
                                       //  have to make a session

    Csession *existing_session = 
                m_pShmemDllManData-> //...
                    pSessionTable[first_process_match_idx].pSession;

    const char *app_name = (LPCTSTR)existing_session->ssnAppName();

    Csession *new_session = makeNewSession
                            (app_name, existing_session->ssnhInstance(),
                             existing_session->ssnHWnd(), highest_level);
    return(new_session);
}






/////////////////////////////////////////////////////////////////////////////
// CdllManager::getCurrentSession
// ______________________________

Csession *CdllManager::getCurrentSession()
{ 
    DWORD  current_thread_id = GetCurrentThreadId();
    DWORD  current_process_id = GetCurrentProcessId();

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
        if ( (m_pShmemDllManData-> //...
                            pSessionTable[idx].nProcessID
              == current_process_id)
            &&
             (m_pShmemDllManData-> //...
                            pSessionTable[idx].nThreadID
               == current_thread_id) )
        {
            return(m_pShmemDllManData-> //...
                                pSessionTable[idx].pSession);
        }
    }
    return(NULL);
}




/////////////////////////////////////////////////////////////////////////////
// makeNewSession
// ______________
//
// Called when for first and every thread using this instance of the
// NTCSS DLL.  Allocates space for, sets up and returns a pointer to
// a Csession.

Csession *CdllManager::makeNewSession(const char *lpszApplication, 
                                      const HANDLE hInstance,
                                      const HANDLE hWnd, 
                                      const SecurityLevel startup_seclvl)
{
                                       // Now make sure that 
                                       //  m_pShmemDllManData
                                       //  is valid
                                       //
    if (!validateShmemCoreData())
            return(NULL);
    
    int idx;                                   
    if (thisIsDeskTopDll())

        idx = SESSION_IDX_DESKTOP;

    else if (thisIsSecbanDll())

        idx = SESSION_IDX_SECBAN;

									   // DeskTop and Secban are special
									   //  cases, have session table
									   //  slots reserved in advance
	else
    {
        if (!validateUser())
            return(NULL);

        idx = pingOnSecBan(SECBAN_GETSESSIONSLOT_MSG);
	

        //idx = reserveSessionTableSpace(); 4/8/97 this was causing session space problem (jgj)

	    if (idx == RETURN_INT_ERROR)
            return(NULL);
    }

                                       // All sessions except secban's
                                       //  need sockets
    BOOL need_socket = !thisIsSecbanDll();

                                       // Everything okay, now 'make'
                                       //  the session
    Csession *session =
                        new Csession((HINSTANCE)hInstance, (HWND)hWnd,
                                     lpszApplication, idx, need_socket);

    if (session == NULL)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Unable to allocate memory for this session");
        return(NULL);
    }
                                        // Setup pointer to new session
                                        //  table entry and then set
                                        //  table data
    SntcssSessionTableEntry *entry_ptr = 
            &(m_pShmemDllManData->pSessionTable[idx]);

    entry_ptr->nProcessID     = GetCurrentProcessId();
    entry_ptr->nThreadID      = GetCurrentThreadId();


    entry_ptr->eCurSecLvl = startup_seclvl;
    entry_ptr->pSession        = session;

    entry_ptr->hwnd           = (HWND) hWnd;

    return(session);
 }



/////////////////////////////////////////////////////////////////////////////
// pingOnSecBan
// ____________

LRESULT CdllManager::pingOnSecBan(UINT nMsgVal)
{
    CWnd *secbanwin = CWnd::FindWindow(SECBAN_CLASS_NAME,NULL);

    if (secbanwin != NULL)
    {
        return(::SendMessage(secbanwin->GetSafeHwnd(),nMsgVal,0,0));

    }                                  
                                       // Else no SECBAN to be found?
    else
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Cannot communicate with Security Banner");

        return(RETURN_INT_ERROR);
    }
}


/////////////////////////////////////////////////////////////////////////////
// CdllManager::rawInitializations
// _______________________________
//
// Does system file mapping necessary to map to a SshmemCoreDllDataRec
// block of shared memory.  If this is the first initialation of the
// shared memory then the data is setup accordingly.

BOOL CdllManager::rawInitializations(BOOL bIsDesktop)
{
    BOOL   ok,initial_setup;
    HANDLE map_object_handle;

                                       // Try to link to core dll memory 
                                       //   map file
                                       //
    int size_shmem = sizeof(SshmemCoreDllDataRec);

    map_object_handle = 
                CreateFileMapping
                        ( (HANDLE) 0xFFFFFFFF,       // Use paging file
                            NULL,                    // No security attr.
                            PAGE_READWRITE,          // Read/Write
                            0,                       // size: high 32-bits
                            size_shmem,              // size: low 32-bits
                            SHMEM_MAINDLL);          // Map name

    ok = (map_object_handle != NULL);
                                       // If get ERROR_ALREADY_EXISTS then
                                       //   another instance of dll has
                                       //   already setup shmem area
                                       //
    initial_setup = (GetLastError() != ERROR_ALREADY_EXISTS);

	
    if (ok)
    {
                                       // Now get pointer to it
        m_pShmemDllManData = 

                     (SshmemCoreDllDataRec *)
                      MapViewOfFile
                            (map_object_handle,     // Map to here
                             FILE_MAP_WRITE,        // Read/Write
                             0,                     // high offset
                             0,                     // low offset
                             0);                    // map all of file

        ok = (m_pShmemDllManData != NULL);
    }

                                       // If shared memory area was not
                                       //  already active then must
                                       //  initialize it.

	
    if ((ok) && (initial_setup))
    {
		
        memset(m_pShmemDllManData,0,
               sizeof(SshmemCoreDllDataRec));

        m_pShmemDllManData->nDllUserCnt = 0;

        m_pShmemDllManData->eOverallCurSecLvl = NTCSS_UNCLASSIFIED;

        m_pShmemDllManData->bDeskTopInitFlag = FALSE;

        m_pShmemDllManData->bSecbanInitFlag = FALSE;

        m_pShmemDllManData->nShmemLrsSize = 0;

        m_pShmemDllManData->nPrintLockerID = 0;

        m_pShmemDllManData->pPrintSpoolMan = NULL;

        memset(m_pShmemDllManData->lpszServerName, 0,SIZE_HOST_NAME);

        memset(m_pShmemDllManData->lpszDllVersionNum, 0,
               SIZE_VERSION_NUMBER);

        m_pShmemDllManData->nValidityChkVal = 
                                            NTCSSCODE_VALITY_CHECK_VALUE;
    }

    								   // If this is DeskTop then set
									   //  bDeskTopInitFlag
									   //
	else if ((ok) && (!initial_setup) && (bIsDesktop))
	{
        m_pShmemDllManData->bDeskTopInitFlag = TRUE;
    }

    								   // If this is not DeskTop but DeskTop
									   //  has already initialized then set
									   //  link to the LRS data
									   //
	else if ((ok) && (!initial_setup) && (!bIsDesktop) 
			 && (m_pShmemDllManData->bDeskTopInitFlag == TRUE))
    {
        ok = m_pUser.DMcall_linkToShmemLrsData
                            (m_pShmemDllManData->nShmemLrsSize);
    } 
	
    if (ok)

        return(TRUE);

    else
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Error in DLL shared memory initialization");
        return(FALSE);
    }
}




/////////////////////////////////////////////////////////////////////////////
// CdllManager::reserveSessionTableSpace
// _____________________________________
//
// Finds first unused entry in the session table, marks it as reserved
// and returns index to it.

int CdllManager::reserveSessionTableSpace ()
{
									   // Loop until find an unused
									   //  session table entry
									   //

    for (int idx = SESSION_IDX_USER_START; 
         idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
		

        if (m_pShmemDllManData->pSessionTable[idx].nProcessID
            == NULL)
			
        {
            break;
        }
    }
		 

    if (idx == SESSIONS_MAX_ACTIVE_CNT)
    {
        dllErrorMan()->RecordError("No session space available ");
        return(RETURN_INT_ERROR);
    }

    SntcssSessionTableEntry *entry_ptr = 
            &(m_pShmemDllManData->pSessionTable[idx]);

    entry_ptr->nProcessID       = (DWORD) NTCSSCODE_RESERVED_FOR_USE;
    entry_ptr->nThreadID        = (DWORD) NTCSSCODE_RESERVED_FOR_USE;

    entry_ptr->eCurSecLvl   = NTCSS_UNCLASSIFIED;
    entry_ptr->pSession     = NULL;

    return(idx);
} 



//////////////////////////////////////////////////////////////////////////////
// sessionSpoolPrintUnLock
// _______________________
//
// NEEDTODO - need lock here

BOOL CdllManager::sessionSpoolPrintUnLock()
{
    if (!sessionHasSpoolPrintLock())
        return(FALSE);

    delete m_pShmemDllManData->pPrintSpoolMan;

    m_pShmemDllManData->pPrintSpoolMan=NULL;

    m_pShmemDllManData->nPrintLockerID = NULL;

    return(TRUE);
}



    
    

/////////////////////////////////////////////////////////////////////////////
// UScall_setSizeOfUserLrsShmem
// ____________________________
//
// Called by the CdllUser to tell the CdllManager the size of the
// LRS data.

BOOL CdllManager::UScall_setSizeOfUserLrsShmem(int nSize)
{
    VERIFY(m_pShmemDllManData->nValidityChkVal ==
           NTCSSCODE_VALITY_CHECK_VALUE);

    m_pShmemDllManData->nShmemLrsSize = nSize;

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CdllManager::validateAppInitialized
// ___________________________________

BOOL CdllManager::validateAppInitialized()
{
    if (!m_bInitOk)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Application not properly initialized");
        return(NULL);
    }
                                       // OK
    return(TRUE);
}


////////////////////////////////////////////////////////////////////////////
// validateApplication
// ___________________

BOOL CdllManager::validateApplication(const char *lpszApplication)
{
                                       // Private apps okayed here
                                       //
    if (strcmp(lpszApplication, NTCSS_PRIVATE_APPNAME) == 0) 
    {
        if ( (!thisIsDeskTopDll()) &&
             (!thisIsSecbanDll()) )
        {
            m_eThisAppType = PRIVATE_APP;

        }
    }
                                       // If not private then must 
                                       //   validate
    else
    {
        if (m_pUser.appGroup(lpszApplication) == NULL)
        {
            gbl_dll_man.dllErrorMan()->RecordError
                                    ("Invalid application");
            return(FALSE);
        }
    }
                                       // OK
    return(TRUE);
}




////////////////////////////////////////////////////////////////////////////
// validateAppOkToInit
// ___________________

BOOL CdllManager::validateAppOkToInit(const char *lpszDllVersion, 
                                      const char *lpszApplication,
                                      const HANDLE   hInstance, 
                                      const HANDLE   hWnd)
{
    if (!validateShmemCoreData())
        return(FALSE);

    if (!validateDeskTopInitialized())
        return(FALSE);

    if ( (thisIsDeskTopDll()) || (thisIsSecbanDll()) )
    {
        return(TRUE);
    }

    if (!validateUser())
        return(FALSE);

    if (!validateApplication(lpszApplication))
        return(FALSE);

	HANDLE lcl_handle;
	

		lcl_handle = hWnd;


    if (!IsWindow((HWND)lcl_handle))
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Window handle is not valid");
        return(FALSE);
    }
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateDeskTopInitialized
// __________________________

BOOL CdllManager::validateDeskTopInitialized()
{
    if (!thisIsDeskTopDll() &&
        !m_pShmemDllManData->bDeskTopInitFlag )
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Desktop is not initialized");
        return(FALSE);
    }                                  
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateDllVersion
// __________________

BOOL CdllManager::validateDllVersion(const char *dll_version)
{
    VERIFY(m_pShmemDllManData->nValidityChkVal ==
           NTCSSCODE_VALITY_CHECK_VALUE);

    if ( strcmp(dll_version,
                 m_pShmemDllManData->lpszDllVersionNum)
          != 0)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Incompatible DLL version already loaded");
        return(FALSE);
    }
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateSecBanInitialized        
// _________________________
                                   
BOOL CdllManager::validateSecBanInitialized()
{
    if ( !m_pShmemDllManData->bSecbanInitFlag )
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Security Banner is not initialized");
        return(FALSE);
    }                                  
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateShmemCoreData
// _____________________

BOOL CdllManager::validateShmemCoreData()
{
    if ( (m_pShmemDllManData == NULL) ||
         (m_pShmemDllManData->nValidityChkVal !=
                                        NTCSSCODE_VALITY_CHECK_VALUE))
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("NTCSS DLL has not been properly initialized");
        return(FALSE);
    }
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateThisIsDeskTop
// _____________________

BOOL CdllManager::validateThisIsDeskTop()
{
    if ( !thisIsDeskTopDll() )
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Error: This is not DeskTop");
        return(FALSE);
    }
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateThisIsSecBan
// ____________________

BOOL CdllManager::validateThisIsSecBan()
{
    if ( !thisIsSecbanDll() )
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Error: This is not Secban");
        return(FALSE);
    }
                                       // OK
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// validateUser
// ____________

BOOL CdllManager::validateUser()
{
    if ( !m_pUser.isValid() )
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("NTCSS DLL login reply structure not initialized properly");
        return(NULL);
    }
                                       // OK
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CMcall_activeProcessCnt
// _______________________

void CdllManager::CMcall_UpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,
													 const DWORD nNewProcessID,
											         const DWORD nNewThreadID)
{
	//hack needed to compensate for having to initialize with Login Dialog instead of DT
	//try to swap the Handles with the new 1's
   

    SntcssSessionTableEntry *entry_ptr;

    for (int idx=0; idx < SESSIONS_MAX_ACTIVE_CNT; idx++) 
    {
        entry_ptr = &(m_pShmemDllManData->pSessionTable[idx]);

        if (entry_ptr->hwnd == OldhWnd)

        {
            if (IsWindow(NewhWnd))
            {
                entry_ptr->nProcessID = nNewProcessID;
                entry_ptr->nThreadID = nNewThreadID;
                entry_ptr->hwnd = NewhWnd;
				break;
            }
        }
    }
}

