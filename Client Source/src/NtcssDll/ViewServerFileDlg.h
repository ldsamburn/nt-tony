/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_VIEWSERVERFILEDLG_H__43F31FA1_0D0F_11D1_9D1C_0060972BFCF1__INCLUDED_)
#define AFX_VIEWSERVERFILEDLG_H__43F31FA1_0D0F_11D1_9D1C_0060972BFCF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewServerFileDlg.h : header file
//

#include "resource.h"
#include "session.h"
#include "ResizingDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CViewServerFileDlg dialog

class CViewServerFileDlg : public CResizingDialog
{
// Construction
public:
	CViewServerFileDlg(CWnd* pParent = NULL,CString Server="",
		               CString FileName="");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewServerFileDlg)
	enum { IDD = IDD_VIEWSERVERFILE };
	CEdit	m_EditCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewServerFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewServerFileDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_Server,m_FileName,m_LocalFileName;
	CString FindAndReplace(CString OrgString);

	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSERVERFILEDLG_H__43F31FA1_0D0F_11D1_9D1C_0060972BFCF1__INCLUDED_)
