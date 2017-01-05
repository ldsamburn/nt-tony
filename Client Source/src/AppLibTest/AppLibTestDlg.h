/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppLibTestDlg.h : header file
//

#if !defined(AFX_APPLIBTESTDLG_H__19C547C8_CA66_11D2_927C_009027164965__INCLUDED_)
#define AFX_APPLIBTESTDLG_H__19C547C8_CA66_11D2_927C_009027164965__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAppLibTestDlg dialog

class CAppLibTestDlg : public CDialog
{
// Construction
public:
	CAppLibTestDlg(CWnd* pParent = NULL);	// standard constructor
/*	int ConnectServer(CAsyncSocket *cps, int port, char *host);
	BOOL NtcssGetDBPassword(char *loginname,
						char *password,
						char *appname,
						char *hostname,
						char *dbpassword);
*/

// Dialog Data
	//{{AFX_DATA(CAppLibTestDlg)
	enum { IDD = IDD_APPLIBTEST_DIALOG };
	CString	m_LoginName;
	CString	m_Password;
	CString	m_AppName;
	CString	m_HostName;
	CString	m_AppPassword;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppLibTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAppLibTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLIBTESTDLG_H__19C547C8_CA66_11D2_927C_009027164965__INCLUDED_)
