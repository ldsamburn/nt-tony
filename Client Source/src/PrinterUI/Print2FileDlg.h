/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_PRINT2FILEDLG_H__9EFD7322_D539_11D1_A748_006008421BB1__INCLUDED_)
#define AFX_PRINT2FILEDLG_H__9EFD7322_D539_11D1_A748_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Print2FileDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPrint2FileDlg dialog


class CPrint2FileDlg : public CDialog
{
// Construction
public:
	CPrint2FileDlg(CWnd* pParent = NULL,CString strFileName=_T(""),int nAppend=-1);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrint2FileDlg)
	enum { IDD = IDD_PRINT2FILE };
	CString	m_PrintFileName;
	int		m_nAppend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrint2FileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrint2FileDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINT2FILEDLG_H__9EFD7322_D539_11D1_A748_006008421BB1__INCLUDED_)
