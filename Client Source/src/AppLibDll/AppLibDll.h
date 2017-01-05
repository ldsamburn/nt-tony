/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppLibDll.h : main header file for the APPLIBDLL DLL
//

#if !defined(AFX_APPLIBDLL_H__DF431BF0_E87E_11D2_9284_009027164965__INCLUDED_)
#define AFX_APPLIBDLL_H__DF431BF0_E87E_11D2_9284_009027164965__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAppLibDllApp
// See AppLibDll.cpp for the implementation of this class
//

class CAppLibDllApp : public CWinApp
{
public:
	CAppLibDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppLibDllApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAppLibDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLIBDLL_H__DF431BF0_E87E_11D2_9284_009027164965__INCLUDED_)
