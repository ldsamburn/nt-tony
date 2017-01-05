/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ModifyOtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyOtDlg dialog

#include "AppPrintConfigDlg.h"

class CModifyOtDlg : public CDialog
{
// Construction
public:
	CModifyOtDlg(CWnd* pParent = NULL,CString Mode="EDIT",CAppPrintConfigDlg* dad=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyOtDlg)
	enum { IDD = IDD_MODIFYOT };
	CEdit	m_DesripCtrl;
	CButton	m_BatchCtrl;
	CComboBox	m_DefaultPrinterComboBox;
	CEdit	m_NameCEdit;
	CListBox	m_PrinterList;
	BOOL	m_Batch;
	CString	m_Default;
	CString	m_Description;
	CString	m_App;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyOtDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString mMode,m_strNumericPrintList,m_Host;
	CAppPrintConfigDlg *m_dad;
	BOOL CheckChanges();
	BOOL CheckForChanges();
	void GetNewPrinterString();
	void UpdateParent();
	void UpdateDisplay();
	// Generated message map functions
	//{{AFX_MSG(CModifyOtDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangePrinterlist();
	afx_msg void OnSelchangeDefaultprinter();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
