/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ValidateUserDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CValidateUserDialog dialog

                                       // Make sure have stdafx.h


#include "resource.h"

class CValidateUserDialog : public CDialog
{
// Construction
public:
	CValidateUserDialog(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CValidateUserDialog)
	enum { IDD = IDC_VALIDATEUSER };
	CString	m_UserName;
	CString	m_Password;
	//}}AFX_DATA
//	CString m_Password, m_UserName;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValidateUserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CValidateUserDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
