/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// msgboard.h : main header file for the MSGBOARD application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _INCLUDE_MSGBOARD_H_
#define _INCLUDE_MSGBOARD_H_

#include "resource.h"       // main symbols

class CMsgBoardMgr;

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardApp:
// See msgboard.cpp for the implementation of this class
//

class CMsgBoardApp : public CWinApp
{
	friend CMsgBoardMgr;
	
// Construction

public:
	CMsgBoardApp();

// Overrides
	virtual BOOL InitInstance();


// Implementation

private: 
	
public:
	//{{AFX_MSG(CMsgBoardApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // _INCLUDE_MSGBOARD_H_

/////////////////////////////////////////////////////////////////////////////
