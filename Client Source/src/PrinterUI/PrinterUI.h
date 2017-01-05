/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrinterUI.h : main header file for the PRINTERUI application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CPrinterUIApp:
// See PrinterUI.cpp for the implementation of this class
//

class CPrinterUIApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CPrinterUIApp();
	CString m_strShares;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrinterUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrinterUIApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
