/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////


class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppExit();
	afx_msg void OnDesktoptopics();
	afx_msg void OnGettinghelp();
	afx_msg void OnWindowhelp();
	afx_msg void OnFieldhelp();
	afx_msg void OnUsersguide();
	afx_msg void OnIdd();
	afx_msg void OnSysadm();
	afx_msg void OnTechsupport();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//afx_msg void OnRefresh();
};

/////////////////////////////////////////////////////////////////////////////
