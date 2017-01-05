/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// LogFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogFileDlg dialog

class CLogFileDlg : public CDialog
{
// Construction
public:
	CLogFileDlg(CWnd* pParent = NULL,CString strFileName="");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogFileDlg)
	enum { IDD = IDD_LOGFILE };
	CEdit	m_Edit_Ctrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_LogFileName;
	// Generated message map functions
	//{{AFX_MSG(CLogFileDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
