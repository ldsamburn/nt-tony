/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pdj_conf.h : main header file for the PDJ_CONF application
//
#define MAX_ROLE_LEN 32
#define MAX_TYPE_LEN 32
#define MAX_INI_ENTRY 512
#define MAX_INI_SECTION 65535
#define PDJ_TIMER_ID 101
#define PDJ_TIMER_VAL 180000
#define PDJ_LOCK "01"
#define LOCK_NEW "00"
#define LOCK_RENEW "01"
#define LOCK_REMOVE "02"
#define LOCK_ID_LEN 9

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPdj_confApp:
// See pdj_conf.cpp for the implementation of this class
//

class CPdj_confApp : public CWinApp
{
public:
	CPdj_confApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPdj_confApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPdj_confApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
