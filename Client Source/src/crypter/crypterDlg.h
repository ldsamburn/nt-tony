/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// crypterDlg.h : header file
//

#if !defined(AFX_CRYPTERDLG_H__8C9C0F66_1460_11D3_9769_00C04F4D4DC8__INCLUDED_)
#define AFX_CRYPTERDLG_H__8C9C0F66_1460_11D3_9769_00C04F4D4DC8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCrypterDlg dialog

class CCrypterDlg : public CDialog
{
// Construction
public:
	CCrypterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCrypterDlg)
	enum { IDD = IDD_CRYPTER_DIALOG };
	CString	m_Decrypted;
	CString	m_Encrypted;
	CString	m_Key;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrypterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCrypterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRYPTERDLG_H__8C9C0F66_1460_11D3_9769_00C04F4D4DC8__INCLUDED_)
