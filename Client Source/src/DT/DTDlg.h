/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DTDlg.h : header file
//

#if !defined(AFX_DTDLG_H__6ECA4C83_2E7D_41EC_9281_8961FC3CF584__INCLUDED_)
#define AFX_DTDLG_H__6ECA4C83_2E7D_41EC_9281_8961FC3CF584__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GfxOutBarCtrl.h"
#include "Resource.h"
#include "DTSocket.h"
#include "NtcssConfig.h"
#include "lrs.h"
#include "BtnST.h"
#include "list"
#include "afxpriv.h"

//An STL list implies some sort of order, so the
//template needs to be able to use the relational operators
//to arrange the list items. Also, local types
//can't be used to instatiate a template so that's why is here 

typedef std::list<LRSTemplateStruct> CLRSList;

//LRSTemplateStruct

/////////////////////////////////////////////////////////////////////////////
// CDTDlg dialog

class CDTDlg : public CDialog
{

// Construction
friend class CDTSocket;

enum {PageSize=5}; //TODO dynamically size

public:
	CDTDlg(CWnd* pParent,CNtcssConfig* cfg,
		   CString& strServer,LoginReplyStruct* pLRS,const CString& strPassword,
		   const BOOL m_bAppAdmin,HWND hLoginWnd);	// standard constructor
	~CDTDlg();

	CString OldPassword(){return m_strPassword;}

// Dialog Data
	//{{AFX_DATA(CDTDlg)
	enum { IDD = IDD_DT_DIALOG };
	CButtonST	m_btnMenu;
	CButtonST	m_btnHelp;
	CButtonST	m_btnSearch;
	CButtonST	m_btnBack;
	CButtonST	m_btnForward;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGfxOutBarCtrl	m_wndBar;
	CImageList m_imgLarge,m_imgSmall;
	HICON m_hIcon,m_hIconDefault;
	int m_nPages,m_nPage;
	HWND m_hLoginWnd;

	// Generated message map functions
	//{{AFX_MSG(CDTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnApc();
	afx_msg void OnUpdateApc(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );
	afx_msg long OnOutbarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBack();
	afx_msg void OnForward();
	afx_msg void OnSearch();
	afx_msg void OnBjq();
	afx_msg void OnUpdateBjq(CCmdUI* pCmdUI);
	afx_msg void OnHelpbutton();
	afx_msg void OnSh();
	afx_msg void OnUpdateSh(CCmdUI* pCmdUI);
	afx_msg void OnFh();
	afx_msg void OnAbout();
	afx_msg void OnRn();
	afx_msg void OnMsgbrds();
	afx_msg void OnUpdateMsgbrds(CCmdUI* pCmdUI);
	afx_msg void OnPdj();
	afx_msg void OnUpdatePdj(CCmdUI* pCmdUI);
	afx_msg void OnPrq();
	afx_msg void OnUpdatePrq(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnPasschg();
	afx_msg void OnUpdatePasschg(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKickIdle();
	afx_msg void OnMenubutton();
	afx_msg void OnEndSession(BOOL bEnding);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

//  virtual void OnOK() {}; 
//	virtual void OnCancel() {}; 



private:
	
	BOOL startProcess(CString& sCmdLine,CString& sRunningDir,
		DWORD& dwRetValue);
	BOOL startProcessAndWait(CString& sCmdLine,CString& sRunningDir,
		DWORD& dwRetValue);
	BOOL STAT(const CString& strFile);
	CWinThread*	m_thread_handle;
	CDTSocket*	m_pDTSocket;
	void UDPSelect();
	CString m_strRoot,m_strNtcssBin,m_strHelp,m_strServer,m_strVCProgram;
	CString m_strIniAppNameTag;
	CString m_strUserName;
	CString m_strXEmulator,m_strVersion,m_strDefaultServer,m_strNtcssIniFile;
	BOOL m_bRunOnce,m_bUDP,m_bAppAdmin,m_bShowMessage,m_bQueryKnownHosts;
	void Advance(const int nAdvance,CLRSList::iterator& x);
	BOOL isVCProgram(){return STAT(m_strVCProgram);}
	BOOL isXEmulator(){return STAT(m_strXEmulator);}
	void SetNoMessage(){m_bShowMessage=FALSE;}
	BOOL DisplayMessage() {return m_bShowMessage;}
	CString GetDir(const CString& strPath);
	BOOL GetApp(const CString& strApp);
	void display_error(DWORD error_code);
	void CheckCmdLine(CString& strCmdline);
	void CreateKnownHosts();

	void SetUpTemplateList(const LoginReplyStruct* lrs);

	BOOL DoInitializations(LoginReplyStruct* lrs);
	void NewPage(const LoginReplyStruct* lrs,const int nPage,const CString& strItem=_T(""));
	UserInfoStruct* m_pUserInfoStruct;
	LoginReplyStruct* m_pLRS;
	CLRSList m_listlrs;  //Container List with Sorted LRS
	CStringEx m_strExPGs;
	CStatusBarCtrl m_StatBar; //statusbar
	int m_Widths[1];
	void LookupPage(const CString& strApp);
	BOOL KillSecBan();
	BOOL FindApp(const CString& strApp);
	CString m_strPassword;
};


/////////////////////////////////////////////////////////////////////////////
// CProgPropPage dialog


/////////////////////////////////////////////////////////////////////////////
// CLookupDlg dialog

class CLookupDlg : public CDialog
{
// Construction
public:
	CLookupDlg(CWnd* pParent,CStringEx strApps);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLookupDlg)
	enum { IDD = IDD_SEARCH };
	CButton	m_btnOK;
	CEdit	m_FindCtrl;
	CListBox	m_lbControl;
	CString	m_strApp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLookupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLookupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePgeditbox();
	afx_msg void OnDblclkPglistbox();
	afx_msg void OnSelchangePglistbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CStringEx m_strApps;
};
/////////////////////////////////////////////////////////////////////////////
// CPassChangeDlg dialog

class CPassChangeDlg : public CDialog
{
// Construction
public:
	CPassChangeDlg(CWnd* pParent,const CString& strUserName,
		           const CString& strOldPassword);  

// Dialog Data
	//{{AFX_DATA(CPassChangeDlg)
	enum { IDD = IDD_CHGPASSDLG };
	CEdit	m_OldPassCtrl;
	CEdit	m_VerifyCtrl;
	CEdit	m_PasswordCtrl;
	CButton	m_btnOk;
	CString	m_strUserName;
	CString	m_strPassword;
	CString	m_strVerifyPassword;
	CString	m_strOldPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassChangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassChangeDlg)
	afx_msg void OnSetfocusNewpasswrd();
	afx_msg void OnSetfocusVerifypass();
	afx_msg void OnUpdateVerifypass();
	virtual void OnOK();
	//}}AFX_MSG
	//
	LONG OnCheckPass(UINT wParam,LONG lParam);
	//afx_msg void OnKillfocusOldpass();

	DECLARE_MESSAGE_MAP()
	CString m_strOldPassword2;
};
/////////////////////////////////////////////////////////////////////////////
// CSelHostDlg dialog

class CSelHostDlg : public CDialog
{
// Construction
public:
	CSelHostDlg(CWnd* pParent,LPCTSTR strCmdLine);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelHostDlg)
	enum { IDD = IDD_SELECTHOST };
	CComboBox	m_SelHostCtrl;
	CButton	m_btnOk;
	CString	m_strHost;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelHostDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelHostDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CStringEx m_strExCommandLine;

};
/////////////////////////////////////////////////////////////////////////////
// CPropDlg dialog

class CPropDlg : public CDialog
{
// Construction
public:
	CPropDlg(CWnd* pParent,LPCTSTR strTitle,LPCTSTR strFile,
		     LPCTSTR strArgs,LPCTSTR strWorkingDir);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropDlg)
	enum { IDD = IDD_PROGPROP };
	CString	m_strArgs;
	CString	m_strFile;
	CString	m_strTitle;
	CString	m_strWorkingDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
#endif // !defined(AFX_DTDLG_H__6ECA4C83_2E7D_41EC_9281_8961FC3CF584__INCLUDED_)
