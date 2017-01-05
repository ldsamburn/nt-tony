/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DTLib.h : main header file for the DTLIB DLL
//

#if !defined(AFX_DTLIB_H__56E989CE_6DD7_47C8_A6D1_3E01C4020E01__INCLUDED_)
#define AFX_DTLIB_H__56E989CE_6DD7_47C8_A6D1_3E01C4020E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "lrs.h"
#include "afxtempl.h"
#include "StringEx.h"
#include "timeoutsocket.h"
/////////////////////////////////////////////////////////////////////////////
// CDTLibApp
// See DTLib.cpp for the implementation of this class
//

class CDTLibApp : public CWinApp
{
public:
	CDTLibApp();

	BOOL IsDomainLocal(char *strDCName);
	int GetPAS(char *strPAS,const char *strUserName, const char *strDefaultPAS);
	int ConnectServer(CTimeoutSocket *cps, int port, char *strHostname);
	int ReadMessage(CTimeoutSocket *cps, char *lrsBuffer,int maxMessage);
	int FillLRS(char *lrsBuffer);
	char *unbundleData(char *str, char *key,... );
	int ExpandPath(char *strPath, int nPathLen);
	int DoChangePassword(const char *strUserName, char *strCryptOldPwd,
		char *strCryptNewPwd);
	CString getpas(const CString& UserName, const CString& strMaster);

	LoginReplyStruct *m_stLRS;
	int m_TotalPrograms;
	int m_TotalGroups;
	CString m_NtcssBinDir;
	CString m_NtcssDir;
	CString m_NtcssResourceDir;
	CString m_NtcssHelpDir;
	CString m_strPAS;
	BOOL m_AppAdminFlag;
	char m_UnlockedApps[SIZE_UNLOCKED_APPS];
	char m_strErrMsg[100];
	bool m_bIgnoreNT;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTLibApp)
	public:
	virtual BOOL InitInstance();


	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDTLibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DTLIB_H__56E989CE_6DD7_47C8_A6D1_3E01C4020E01__INCLUDED_)
