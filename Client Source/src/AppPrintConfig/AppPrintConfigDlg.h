/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppPrintConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppPrintConfigDlg dialog

#ifndef _INCLUDE_AppPrintConfigDlg_H_
#define _INCLUDE_AppPrintConfigDlg_H_

#include "afxtempl.h"
#include "APCListCtrl.h"

struct change_node
{
	CString group;
	CString operation;
	CString title;
	CString description;
	CString Default;
	CString batch;
	CString printers;
}; 



const int MAX_INI_SECTION=65535;
const int PID_SIZE=8;
const int APP_SIZE=16;
const int MAX_OT_LEN=16;
const int MIN_OT_LEN=3;
const int MAX_DESCRIP_LEN=64;
const int MIN_DESCRIP_LEN=3;


class CAppPrintConfigDlg : public CDialog
{

friend class CModifyOtDlg;

private:

	CString m_PutFileName,m_GetFileName,m_HostName,m_ServerFileName,m_LogFileName;
	BOOL m_bIsLogFile;
//	CString m_PID;

	change_node *new_node; 

    CList <change_node*,change_node*> ChangeList;
//	CList <lock_node*,lock_node*> PID_List;
//	CList <lock_node*,lock_node*> LockedGroup; //define list for locked program groups
	void FillAppListCtrl();


// Construction
public:
	CAppPrintConfigDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CAppPrintConfigDlg)
	enum { IDD = IDD_APPPRINTCONFIG_DIALOG };
	APCListCtrl	m_PrinterCtrl;
	APCListCtrl	m_OutPutCtrl;
	CListBox	m_AppListCtrl;
	CString	m_AppList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppPrintConfigDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAppPrintConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeApplist();
	afx_msg void OnApply();
	afx_msg void OnOtadd();
	afx_msg void OnOtedit();
	afx_msg void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );
	afx_msg void OnViewlog();
	afx_msg void OnUpdateOtedit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOtdelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOtadd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewlog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApply(CCmdUI* pCmdUI);
	afx_msg void OnOtdelete();
	afx_msg void OnItemchangedOutputlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickPrinterlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDesktophelp();
	afx_msg void OnGettinghelp();
	afx_msg void OnWindowhelp();
	afx_msg void OnFieldhelp();
	afx_msg void OnUserguide();
	afx_msg void OnIdd();
	virtual void OnCancel();
	afx_msg void OnSysadm();
	afx_msg void OnTechsupport();
	afx_msg void OnVersion();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRefresh();
	afx_msg void OnQuit();
	//}}AFX_MSG

	//afx_msg void OnQuit();
	
	//change_node* MakeNode(int nIndex,CString operation);
	void UpdatePrinterList(CString app);
	void ExtractPrinters(CString title);
	void UpdateOTList(CString app);
	void UnSelectPrintCtrl();
	void AddNodeToCtrl(int nIndex=-1);
	CString FindPrinterStringInChangeList(CString otype);
	CString NumericPrintStringToCString(CString printers);
	CString GetName(int nIndex=-1);
	CString GetDescription(int nIndex=-1);
	CString GetDefaultPrinter(int nIndex=-1);
	CString GetBatch(int nIndex=-1);
	CString GetPrinterString();
	BOOL IsInOtypeChangeList(CString otype);
	BOOL OutputChangeList();
	BOOL DeleteCList();
	BOOL OTInit();
	BOOL PutIniFile();
	BOOL GetIniFile();
	//void SetLockOptions();
	//BOOL RemoveRefreshLockOptions(BOOL refresh);
	int m_nTimer;
	CMenu m_Popup;
	CImageList* m_pimagelist;
	CString GetAuthServer(CString strApp);
	BOOL m_bRDMS;

	DECLARE_MESSAGE_MAP()

};

#endif 
