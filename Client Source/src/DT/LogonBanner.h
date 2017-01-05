/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_LOGONBANNER_H__A8F831CA_A2BB_4177_BF0D_78CC418D5A51__INCLUDED_)
#define AFX_LOGONBANNER_H__A8F831CA_A2BB_4177_BF0D_78CC418D5A51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogonBanner.h : header file
//
#include "NewLabel.h"
#define WM_NTCSS_WARNING_RESPONSE (WM_USER + 200)
#define WM_HIT_OK (WM_USER + 202)

/////////////////////////////////////////////////////////////////////////////
// CLogonBanner dialog

class CLogonBanner : public CDialog
{
// Construction
public:
	CLogonBanner(CWnd* pParent,const CString& strMasterServer,
				 CNtcssConfig* NtcssConfig,const BOOL bVisible);   

	CFont m_TextFont;
	CString m_strServer,m_strMasterServer;
	BOOL m_bAppAdmin;
	LoginReplyStruct* m_pLRS;
	CNtcssConfig* m_NtcssConfig;
	HWND m_Hwnd;

	~CLogonBanner();


// Dialog Data
	//{{AFX_DATA(CLogonBanner)
	enum { IDD = IDD_LOGON_BANNER };
	CButton	m_btnCancel;
	CNewLabel m_StaticPassword;
	CNewLabel m_StaticUserName;
	CNewLabel m_StaticLine3;
	CNewLabel m_StaticLine2;
	CNewLabel m_StaticLine1;
	CEdit	m_PasswordCtrl;
	CEdit	m_UserNameCtrl;
	CButton	m_btnOK;
	CString	m_strPassword;
	CString	m_strUserName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogonBanner)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogonBanner)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	afx_msg void OnChangePassword();
	//}}AFX_MSG
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	
	DECLARE_MESSAGE_MAP()
	LONG OnSecbanPing(UINT wParam,LONG lParam);
	LONG OnHitOK(UINT wParam,LONG lParam);
	BOOL startSecban();
	BOOL m_bVisible;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGONBANNER_H__A8F831CA_A2BB_4177_BF0D_78CC418D5A51__INCLUDED_)
