/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_LOCKEDAPPWARNDLG_H__65E1ACC2_4FE2_11D1_9D47_0060972BFCF1__INCLUDED_)
#define AFX_LOCKEDAPPWARNDLG_H__65E1ACC2_4FE2_11D1_9D47_0060972BFCF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LockedAppWarnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLockedAppWarnDlg dialog

class CLockedAppWarnDlg : public CDialog
{
// Construction
public:
	CLockedAppWarnDlg(CWnd* pParent = NULL,CString strAppList=_T(""));   // standard constructor
	CString m_strAppList;
// Dialog Data
	//{{AFX_DATA(CLockedAppWarnDlg)
	enum { IDD = IDD_LOCKEDAPPWARNDLG };
	CListBox	m_AppListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLockedAppWarnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLockedAppWarnDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCKEDAPPWARNDLG_H__65E1ACC2_4FE2_11D1_9D47_0060972BFCF1__INCLUDED_)
