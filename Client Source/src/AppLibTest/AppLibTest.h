/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppLibTest.h : main header file for the APPLIBTEST application
//

#if !defined(AFX_APPLIBTEST_H__19C547C6_CA66_11D2_927C_009027164965__INCLUDED_)
#define AFX_APPLIBTEST_H__19C547C6_CA66_11D2_927C_009027164965__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAppLibTestApp:
// See AppLibTest.cpp for the implementation of this class
//

class CAppLibTestApp : public CWinApp
{
public:
	CAppLibTestApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppLibTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAppLibTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLIBTEST_H__19C547C6_CA66_11D2_927C_009027164965__INCLUDED_)
