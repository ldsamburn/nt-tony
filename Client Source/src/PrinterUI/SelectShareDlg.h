/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_SELECTSHAREDLG_H__41B18971_8EEA_11D2_A76A_006008421BB1__INCLUDED_)
#define AFX_SELECTSHAREDLG_H__41B18971_8EEA_11D2_A76A_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StringEx.h"


// SelectShareDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectShareDlg dialog

class CSelectShareDlg : public CDialog
{
// Construction
public:
	CSelectShareDlg(CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CSelectShareDlg)
	enum { IDD = IDD_SELECTSHARE };
	CTreeCtrl	m_TreeCtrl;
	CString	m_strPrinter;
	//}}AFX_DATA
    

// Overrides
	CString m_strHost;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectShareDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectShareDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	afx_msg void OnSelchangedTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	
	HACCEL m_hAccel;
	CImageList m_ImageList;
	void EnumPrintServer(HTREEITEM hParent,CString strName);
public:
	CStringEx strResult;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSHAREDLG_H__41B18971_8EEA_11D2_A76A_006008421BB1__INCLUDED_)
