/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_GETSERVERFILENAME_H__DA70A5AF_5EE7_11D3_94B5_0050041C95DD__INCLUDED_)
#define AFX_GETSERVERFILENAME_H__DA70A5AF_5EE7_11D3_94B5_0050041C95DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetServerFilename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetServerFilename dialog
#include "ViewSFileDlg.h"

class CGetServerFilename : public CViewSFileDlg
{
// Construction
public:
	CGetServerFilename(CWnd* pParent = NULL,Csession *session=NULL,
		          CString ServerName=_T(""), CString Directory=_T(""));   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetServerFilename)
	//enum { IDD = IDD_VIEWFILEDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetServerFilename)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetServerFilename)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETSERVERFILENAME_H__DA70A5AF_5EE7_11D3_94B5_0050041C95DD__INCLUDED_)
