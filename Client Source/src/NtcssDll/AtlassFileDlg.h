/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_ATLASSFILEDLG_H__3AF30682_FC88_11D1_A752_006008421BB1__INCLUDED_)
#define AFX_ATLASSFILEDLG_H__3AF30682_FC88_11D1_A752_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "resource.h"
// AtlassFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAtlassFileDlg dialog

class CAtlassFileDlg : public CDialog
{
// Construction
public:
	CAtlassFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAtlassFileDlg)
	enum { IDD = IDD_ATLASSFILE };
	CString	m_strAtlassFname;
	int		m_nAppend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtlassFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAtlassFileDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATLASSFILEDLG_H__3AF30682_FC88_11D1_A752_006008421BB1__INCLUDED_)
