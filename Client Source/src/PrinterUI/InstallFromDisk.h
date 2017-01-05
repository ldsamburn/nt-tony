/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// InstallFromDisk.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInstallFromDisk dialog

class CInstallFromDisk : public CDialog
{
// Construction
public:
	CInstallFromDisk(CWnd* pParent = NULL,CString DriverFile=_T(""),
		BOOL SysInf=FALSE, CMapStringToString* list=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallFromDisk)
	enum { IDD = IDD_INSTALLFROMDISK };
	CComboBox	m_DriveCtrl;
	CString	m_Drive;
	//}}AFX_DATA

	CString m_Path,m_strINF;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstallFromDisk)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

CString OnPickfolder(CString driver);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInstallFromDisk)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString m_DriverFile;
	BOOL m_SysInf;
	CMapStringToString* m_list;

};
