/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_APPLCKDLG_H__C3DA68E3_4FCB_11D1_9740_006097CA0784__INCLUDED_)
#define AFX_APPLCKDLG_H__C3DA68E3_4FCB_11D1_9740_006097CA0784__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AppLckDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppLckDlg dialog

class CAppLckDlg : public CDialog
{
// Construction
public:
	CAppLckDlg(CWnd* pParent = NULL);   // standard constructor

	int AddAppName(CString appname);
	int ClearAppList();
	CString m_LockedApps;

// Dialog Data
	//{{AFX_DATA(CAppLckDlg)
	enum { IDD = IDD_APPLOCKED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppLckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAppLckDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLCKDLG_H__C3DA68E3_4FCB_11D1_9740_006097CA0784__INCLUDED_)
