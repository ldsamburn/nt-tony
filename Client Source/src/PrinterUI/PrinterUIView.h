/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrinterUIView.h : interface of the CPrinterUIView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_PRINTERUIVIEW_H_
#define _INCLUDE_PRINTERUIVIEW_H_
#define theman
#include "ini.h"
#include "PUIListCtrl.h"



class CPrinterUIView : public CFormView
{
protected: // create from serialization only
	CPrinterUIView();
	DECLARE_DYNCREATE(CPrinterUIView)
	BOOL m_GotLog,m_DoingUpdates;
	CString m_strCurrentPrinter,m_strCurrentApp;
//	static UINT the_thread(LPVOID param);
	void SetAppPrintStatuses();

public:
	//{{AFX_DATA(CPrinterUIView)
	enum { IDD = IDD_PRINTERUI_FORM };
	PUIListCtrl	m_AppListCtrl;
	PUIListCtrl	m_AdapterList;
	PUIListCtrl	m_PrinterList;
	CString	m_Location;
	int		m_MaxRequestSize;
	CString	m_PrinterClass;
	CString	m_AdapterIP;
	CString	m_AdapterType;
	CString	m_AdapterPort;
	CString	m_SuspendState;
	CString	m_RedirectPrinter;
	CString	m_Access;
	//}}AFX_DATA

// Attributes
public:
	CPrinterUIDoc* GetDocument();
	BOOL FillEditBoxes(CString as_printer_name);
	void ChangeEditBoxes(printer_node *ptr_printer,adapter_node* ptr_adapter);
	void SelectAdapterInCtrl(CString as_AdapterIP,PUIListCtrl* ctrl);
	void FillPrinterListCtrl();
	void FillAdapterListCtrl();
	CString GetAppHost(CString strApp);
	CString GetSelectedItem(CListCtrl* ctrl, int item=0);

	CString m_app;
		
	CString m_PID; //PID used for locks
	CMenu m_PrinterPopUp,m_AdapterPopUp;

	int  m_nTimer;
//	int m_nCurrentSubItemPrinterList,m_nCurrentSubItemAdapterList; //jgj
//	BOOL m_bCurrentAscendingPrinterList,m_bCurrentAscendingAdapterList;
//	void CPrinterUIView::ResetAppListBox();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrinterUIView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrinterUIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_Printer_Enabled;
	BOOL m_Queue_Enabled;
	
// Generated message map functions
	//afx_msg void OnColumnclickPrinterList(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	//{{AFX_MSG(CPrinterUIView)
	afx_msg void OnApply();
	afx_msg void OnAdapterDelete();
	afx_msg void OnUpdateAdapterDelete(CCmdUI* pCmdUI);
	afx_msg void OnPrintDelete();
	afx_msg void OnUpdatePrintDelete(CCmdUI* pCmdUI);
	afx_msg void OnStatusHaltprinter();
	afx_msg void OnUpdateStatusHaltprinter(CCmdUI* pCmdUI);
	afx_msg void OnStatusEnableprinter();
	afx_msg void OnUpdateStatusEnableprinter(CCmdUI* pCmdUI);
	afx_msg void OnStatusEnablequeueing();
	afx_msg void OnUpdateStatusEnablequeueing(CCmdUI* pCmdUI);
	afx_msg void OnStatusDisablequeueing();
	afx_msg void OnUpdateStatusDisablequeueing(CCmdUI* pCmdUI);
	afx_msg void OnPrintAdd();
	afx_msg void OnPrintEdit();
	afx_msg void OnUpdatePrintEdit(CCmdUI* pCmdUI);
	afx_msg void OnAdapterAdd();
	afx_msg void OnAdapterEdit();
	afx_msg void OnUpdateAdapterEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileMruFile1(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPUIQuit();
	afx_msg void OnClickPrinterList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedApplicationctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPrinterList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRefresh();
	afx_msg void OnDblclkAdapterList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PrinterUIView.cpp
inline CPrinterUIDoc* CPrinterUIView::GetDocument()
   { return (CPrinterUIDoc*)m_pDocument; }
#endif
#endif
/////////////////////////////////////////////////////////////////////////////
