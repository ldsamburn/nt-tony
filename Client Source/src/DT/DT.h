/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DT.h : main header file for the DT application
//

#if !defined(AFX_DT_H__7B0C322D_7250_408B_A9FE_9F9AB1BE2D0F__INCLUDED_)
#define AFX_DT_H__7B0C322D_7250_408B_A9FE_9F9AB1BE2D0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "NtcssConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDTApp:
// See DT.cpp for the implementation of this class
//

class CDTApp : public CWinApp
{
public:
	CDTApp();
	~CDTApp();
	CNtcssConfig* m_NtcssConfig;
private:
	void KillSecBan();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DT_H__7B0C322D_7250_408B_A9FE_9F9AB1BE2D0F__INCLUDED_)
