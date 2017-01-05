/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllUser.h                    
//                                         
// This is the header file for the CdllUser.
// 
//----------------------------------------------------------------------


#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_DLLUSER_H_
#define _INCLUDE_DLLUSER_H_

#include "ntcssapi.h"
#include "lrs_defs.h"
#include "session.h"

class CdllManager;

class CdllUser 
{
// Constructors
public:
    CdllUser();

// Attributes

    const   SappGroupRec_wFullData *appGroup 
									(const char *lpszGroupTitle);

    int     appGroupsCount () const;

    const   SappProgramRec_wFullData  *appProgram
                              (const char far *lpszGroupTitle,
                               const char far *lpszProgramTitle) const;

    BOOL isValid ()	const
        { return (m_pShmemDllUserData != NULL); };

    const   CString loginName();

    BOOL    setAppData(const char *lpszAppName, 
					   const char *lpszAppData);

    const   SuserInfoRec_wFullData *userInfo() const;

   	CString GetMasterName(); 

	void WalkLRS(const SshmemLoginReplyStruct *pDesktopLrsData);

	BOOL IsAuthServer(LPCTSTR lpszGroupTitle);

	CString GetAuthServer(LPCTSTR lpszGroupTitle);

	CString GetAppServer(LPCTSTR lpszGroupTitle);

	BOOL SetMasterName();


// Operations
public:
    BOOL    getAppNameList (char *lpszAppList, 
							const int nSize, 
							Csession *session);

// Overrides

// Implementation - destructor
public:
    ~CdllUser()
        {};					  

// Implementation - data
protected:
                                       // Points to LRS data
                                       //
    SshmemLoginReplyStruct *m_pShmemDllUserData;

	HANDLE m_hMapObject;

	CString m_strMasterServer;


// Implementation - private functions
                                       // These functions
                                       //  intended to only be called
                                       //  internally from other 
                                       //  CdllUser functions
protected:
    BOOL  copyLrsFromDtToDllShmem
                    (const SshmemLoginReplyStruct *pDesktopLrsData,
                     SshmemLoginReplyStruct *pDllLrsData);

    int   countShmemLrsDataBytesNeeded
							(const SshmemLoginReplyStruct *lrs_ptr);

// Implementation - private functions
                                       // These functions
                                       //  intended to only be called
                                       //  from other helper classes
                                       //
public:
    BOOL  CMcall_initializeShmemLrsData(void *the_ptr);

    BOOL  CMcall_invalidateShmemLrsData(); 

    BOOL  DMcall_linkToShmemLrsData(unsigned int nSize);
 };

#endif //_INCLUDE_DLLUSER_H_


