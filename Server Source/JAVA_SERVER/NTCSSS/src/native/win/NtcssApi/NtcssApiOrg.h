/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssApiOrg.h : main header file for the NTCSSAPI DLL
//

#if !defined(AFX_NTCSSAPI_H__7A0AC305_091B_11D6_B4EC_00C04F4D4DED__INCLUDED_)
#define AFX_NTCSSAPI_H__7A0AC305_091B_11D6_B4EC_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNtcssApiApp
// See NtcssApi.cpp for the implementation of this class
//

class CNtcssApiApp : public CWinApp
{
public:
	CNtcssApiApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNtcssApiApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNtcssApiApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTCSSAPI_H__7A0AC305_091B_11D6_B4EC_00C04F4D4DED__INCLUDED_)
