/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_FTPDLG_H__7C9AF7F2_3DE2_11D2_A75F_006008421BB1__INCLUDED_)
#define AFX_FTPDLG_H__7C9AF7F2_3DE2_11D2_A75F_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "Zipfile.h"
// FtpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFtpDlg dialog

class CFtpDlg : public CDialog
{
// Construction
public:
	CFtpDlg(CWnd* pParent = NULL,CString InFile=_T(""),CString OutFile=_T(""),BOOL bZip=TRUE);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFtpDlg)
	enum { IDD = IDD_FTPDLG };
	CProgressCtrl	m_ProgressCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFtpDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
	LRESULT DOZIP(UINT wParam,LONG lParam);
	CString m_strInFile,m_strOutFile;
	CZipFile m_zf;
	BOOL m_bZip;
 
 

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPDLG_H__7C9AF7F2_3DE2_11D2_A75F_006008421BB1__INCLUDED_)
