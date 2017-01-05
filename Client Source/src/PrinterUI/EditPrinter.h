/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// EditPrinter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditPrinter dialog
#include "stdafx.h"
#include "PUIListCtrl.h"


class CEditPrinter : public CDialog
{
// Construction
public:
	CEditPrinter(CWnd* pParent = NULL,printer_node *saved_printer=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditPrinter)
	enum { IDD = IDD_EDITPRINTER };
	CEdit	m_FileNameCtrl;
	CEdit	m_LocationCtrl;
	CEdit	m_CEditCtrlName;
	CButton	m_NTBUTTON;
	CComboBox	m_SuspendStateCtrl;
	CComboBox	m_AdapterPortCtrl;
	CComboBox	m_PrinterClassCtrl;
	CComboBox	m_PrinterAccessCtrl;
	CComboBox	m_ClassNameCtrl;
	CListBox	m_AppList;
	PUIListCtrl	m_AdapterList;
	CString	m_Name;
	CString	m_Classification;
	CString	m_Location;
	CString	m_PrinterClass;
	CString m_HostName;
	CString	m_PrinterAccess;
	CString m_adapterIP;
	CString	m_AdapterPort;
	CString	m_RedirectPrinter;
	CString	m_SuspendState;
	int		m_MaxRequestedSize;
	CString	m_NTDriver;
	CString	m_FileName;
	int		m_nAppend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPrinter)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	void FillListBox(CListBox* pBox,CStringList* list);
	
	void FillListBox(CComboBox* pBox,CStringList* List);
	BOOL PrinterChanged();
	void SwapPrinterData(printer_node *ptr_printer);
	void SetUpForEdit();
	void SetUpForAdd();
	void SetPorts(CString adapter_IP,CString adapter_name);
	BOOL CheckChanges();
	printer_node* m_saved_printer;
	int m_nCurrentListViewColumn;
	BOOL CheckOverloadChanges();

protected:

	// Generated message map functions
	//{{AFX_MSG(CEditPrinter)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRedirect();
	afx_msg void OnItemchangedPrinterAdapterList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNtbutton();
	afx_msg void OnSelchangePrinterAccess();
	afx_msg void OnDestroy();
	afx_msg void OnClickPrinterAdapterList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//CBitmap m_Bitmap;  
	void ShowFileInfo(BOOL bShow=TRUE);
	BOOL m_bAtlass;


public:

	BOOL IsEdit() {return m_saved_printer!=NULL;}
    

};
