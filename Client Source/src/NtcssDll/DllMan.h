/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllMan.h                    
//                                         
// This is the implementation file for CdllManager.  Also contained
// is the global declaration of the one and only CdllManager -
// gbl_dll_man.
//
//----------------------------------------------------------------------

#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_DLLUSER_H_
    #error include "dlluser.h" before including this file
#endif

#ifndef _INCLUDE_DLL_MAN_H_
#define _INCLUDE_DLL_MAN_H_

#include "ntcssdll.h"
#include "CallMan.h"
#include "dll_defs.h"
#include "err_man.h"
#include "session.h"
#include "lrs_defs.h"
#include "msgcore.h"
#include "PrtSpoolMan.h"
#include "dllutils.h"
#include "BuildNum.h"
#include "ntcssmsg.h"
#include "MsgDlvMan.h"

DWORD WINAPI ThreadFunc();//jj
                                       // These records maintained in
                                       //   shared memory to keep track
                                       //   of Csession's
typedef struct SntcssSessionTableEntry
{
    DWORD            nProcessID;
    DWORD            nThreadID;
    HWND             hwnd;
    SecurityLevel    eCurSecLvl;
    Csession         *pSession;                 
}
SntcssSessionTableEntry;
                                       // This is the structure for the
                                       //  main block of shared memory 
                                       //  used by the dll
typedef struct SshmemCoreDllDataRec
{
    unsigned int                nValidityChkVal;
    unsigned int                nDllUserCnt;
    SecurityLevel               eOverallCurSecLvl;
    BOOL                        bDeskTopInitFlag;
    BOOL                        bSecbanInitFlag;
    DWORD                       nPrintLockerID;
    CdllSpoolPrintManager      *pPrintSpoolMan;
    unsigned int                nShmemLrsSize;
    char                        lpszDllVersionNum[SIZE_VERSION_NUMBER+1];
    char                        lpszServerName[SIZE_HOST_NAME+1];
    SntcssSessionTableEntry     pSessionTable[SESSIONS_MAX_ACTIVE_CNT];
} 
SshmemCoreDllDataRec;


class CdllUser;


/////////////////////////////////////////////////////////////////////////////
// The CdllManager
// _______________

class CdllManager 
{
// Constructors

public:
    CdllManager();

// Attributes

public:
									   // Access to private member data
    CdllCallManager *dllCallMan()
        { return(&m_pCallMan); };

    CErrorManager   *dllErrorMan()
        { return(&m_pErrorMan); };

    CString dllFacilityName();

    const char *dllServerName()
        { return(m_pShmemDllManData->lpszServerName); };

    CdllSpoolPrintManager *dllSpoolPrintMan();

    CdllUser *dllUser()
        { return(&m_pUser); };

    								   // Identity checks for special
									   //  DLL instances
	BOOL thisIsDeskTopDll()
		{ return(m_eThisAppType == DESKTOP_APP); };

	BOOL setServer(LPCTSTR lpszServer) //DT Sets Auth server with this
	{return strlen((strcpy(m_pShmemDllManData->lpszServerName,lpszServer)))!=0;};

    BOOL thisIsSecbanDll()
		{ return(m_eThisAppType == SECBAN_APP); };

    BOOL thisIsThunkerDll()
		{ return(m_eThisAppType == THUNKER_APP); };

// Operations

public:
									   // Validation functions
    BOOL validateAppInitialized();

    BOOL validateApplication(const char *lpszApplication);

    BOOL validateAppOkToInit(const char *lpszDllVersion, 
                             const char *lpszApplication,
                             const HANDLE   hInstance, 
                             const HANDLE   hWnd);

    BOOL validateDeskTopInitialized();

    BOOL validateDllVersion(const char *lpszDllVersion);

    BOOL validateSecBanInitialized();

    BOOL validateShmemCoreData();

    BOOL validateUser();

    BOOL validateThisIsDeskTop();

    BOOL validateThisIsSecBan();

// Overrides

// Implementation - private data
private:
									   // Helper classes
    CdllCallManager        m_pCallMan;
    CErrorManager          m_pErrorMan;
    CdllUser               m_pUser;
                                       // This points to main block of
                                       //   shared memory used by dll
                                       //
    SshmemCoreDllDataRec *m_pShmemDllManData;

                                       // This tells what kind of app 
									   //  is using this instance of 
									   //  the dll 
    EtypeOfApp  m_eThisAppType;

                                       // Indicates whether properly
                                       //  initialized
    BOOL m_bInitOk;

public:
									   // Destructor
    ~CdllManager();

// Implementation - private functions
                                       // These functions
                                       //  intended to only be called
                                       //  internally from other 
                                       //  CdllManager functions
private:
    void cleanUpSessionTable();

    BOOL initializedOK()
		{ return(m_bInitOk); };

    Csession *findOrMakeSession();
    
    Csession *makeNewSession(const char *lpszApplication,
                             const HANDLE   hInstance,
                             const HANDLE hWnd, 
                             const SecurityLevel startup_seclvl);

    LRESULT pingOnSecBan(UINT nMsgVal);

    BOOL rawInitializations(BOOL bIsDesktop);

    BOOL sessionHasSpoolPrintLock()
        { return (dllSpoolPrintMan() != NULL); };

    BOOL sessionSpoolPrintUnLock();


// Implementation - internally called by public access functions
//					(need to be declared public)
public:

	Csession *getCurrentSession();
    int reserveSessionTableSpace();  

									   // Used in debugging
//private:
//    void DBGshowShmemCoreData();


// Implementation - CNtcssDLLApp called functions
public:

                                       // To be called by CNtcssDLLApp
                                       //  when an app is exitting
                                       //
    BOOL APPcall_dismissThisDllInstance();

// Implementation - CdllCallManager called functions
public:
                                       // These functions
                                       //  intended to only be called
                                       //  from CdllCallManager
                                       //
    int CMcall_activeProcessCnt();      

    BOOL CMcall_appCanDoPrintDlg();

    BOOL CMcall_deskTopInit(char *lpszVersion);

    BOOL CMcall_deskTopReset();

	void CMcall_KillActiveProcesses();

    Csession *CMcall_getSessionReadyForApiAction
									(BOOL reset_error = TRUE); 

    BOOL CMcall_initializedOK()
        { return(initializedOK()); };

	void IsPrintProcessAlive();

    Csession *CMcall_makeNewSession(const char *lpszAppNamee,
                                    const HANDLE   hInstance,
                                    const HANDLE hWnd, 
                                    const SecurityLevel startup_seclvl)
        { 
            return(makeNewSession(lpszAppNamee, hInstance, 
                                  hWnd, startup_seclvl));
        };

    SecurityLevel CMcall_overallSecurityLevel();

    BOOL CMcall_rawInitializations()
        { return(rawInitializations(FALSE)); };

    void CMcall_releaseSession(Csession *session)
        { session->DMcall_release(); };

    int CMcall_reserveSessionTableSpace()
		{ return(reserveSessionTableSpace()); };

    BOOL CMcall_secbanInit(char *lpszVersion);

    BOOL CMcall_sessionGetSpoolPrintLock(Csession *session);

    BOOL CMcall_sessionHasSpoolPrintLock()
        { return (sessionHasSpoolPrintLock()); };

    BOOL CMcall_sessionSpoolPrintUnLock()
        { return(sessionSpoolPrintUnLock()); };

    void CMcall_setInitializedOk();

    BOOL CMcall_setSessionSecLvl(Csession *session, SecurityLevel seclvl);


    BOOL CMcall_thunkerInit(char *lpszVersion);

	void CMcall_UpdateDTSessionValues(const HWND OldhWnd,const HWND NewhWnd,
									  const DWORD nNewProcessID,
									  const DWORD nNewThreadID);

// Implementation - CdllMsgDeliveryMan called functions
public:
                                       // This function intended
                                       //  to be used only by 
									   //  CdllMsgDeliveryMan
    Csession *MMcall_CurrentSession()
		{ return(getCurrentSession()); };

// Implementation - CdllUser called functions
public:
                                       // This function intended
                                       //  to be used only by CdllUser
    BOOL UScall_setSizeOfUserLrsShmem(int nSize);
};

                                       // Here define/declare the one and 
                                       //   only CdllManager 
#ifdef DEFINE_THE_DLL_MAN

CdllManager gbl_dll_man;

#else

extern CdllManager gbl_dll_man;

#endif

#endif //_INCLUDE_DLL_MAN_H_					 

