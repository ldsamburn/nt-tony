/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_VIEWSFILEDLG_H__7E78FFB1_09BF_11D1_9D1A_0060972BFCF1__INCLUDED_)
#define AFX_VIEWSFILEDLG_H__7E78FFB1_09BF_11D1_9D1A_0060972BFCF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewSFileDlg.h : header file
//
                                       // Make sure have stdafx.h

#include "resource.h"
#include "session.h"
#include "MyListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CViewSFileDlg dialog

class CViewSFileDlg : public CDialog
{
// Construction
public:
	CViewSFileDlg(CWnd* pParent = NULL,Csession *session=NULL,
		          CString ServerName=_T(""), CString Directory=_T(""));   // standard constructor

	LV_COLUMN listColumn;
	LV_ITEM listItem;

// Dialog Data
	//{{AFX_DATA(CViewSFileDlg)
	enum { IDD = IDD_VIEWFILEDLG };
	MyListCtrl	m_ListCtrl;
	CString	m_SelectedFileName;
	CString	m_DirName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewSFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewSFileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	afx_msg void OnItemchangedListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString m_strDirectory,m_strServerName;

private:

	BOOL getUnixFiles(MyListCtrl* list_box_ptr);
	BOOL CheckServerFile(CString HostName,CString ServerFileName);
	Csession* m_pSession;
    
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSFILEDLG_H__7E78FFB1_09BF_11D1_9D1A_0060972BFCF1__INCLUDED_)
