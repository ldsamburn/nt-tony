/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssDll.h : main header file for the NTCSSDLL DLL
//

#if !defined(AFX_NTCSSDLL_H__5ACE7173_1FF6_11D2_A759_006008421BB1__INCLUDED_)
#define AFX_NTCSSDLL_H__5ACE7173_1FF6_11D2_A759_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNtcssDll
// See NtcssDll.cpp for the implementation of this class
//

class CNtcssDll : public CWinApp
{
public:
	CNtcssDll();
	const CString GetLocalHostIP(){return m_csLocalHostIP;};
	const CString GetLocalHostName(){return m_csLocalHostName;};

/*	 CNtcssDll(const char* pszAppName)
        : CWinApp(pszAppName)
        {}*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNtcssDll)
	public:
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNtcssDll)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_csLocalHostName,m_csLocalHostIP;
	void GetLocalHostANDIP();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTCSSDLL_H__5ACE7173_1FF6_11D2_A759_006008421BB1__INCLUDED_)
