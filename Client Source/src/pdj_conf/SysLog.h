/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// SysLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSysLog dialog

class CSysLog : public CDialog
{
// Construction
public:
	CSysLog(CWnd* pParent = NULL);   // standard constructor
	void SetFile(CString logFile);
	CString m_LogFile;

// Dialog Data
	//{{AFX_DATA(CSysLog)
	enum { IDD = IDD_SYSTEM_LOG };
	CString	m_SyslogText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysLog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
