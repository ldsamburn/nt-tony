/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mainfrm.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file for the MsgBoard CMainFrame class.
//

class CMainFrame : public CMDIFrameWnd
{
// Construction

	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();

// Attributes

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

public:

// Operations

public:

// Implementation


public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMsgboardSubcribe();
	afx_msg void OnMessagesPostnewmessage();
	afx_msg void OnUpdateMsgboardMainIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMsgboardMainOut(CCmdUI* pCmdUI);
	afx_msg void OnMsgboardOpen();
	afx_msg void OnMsgboardMainIn();
	afx_msg void OnFilePrint();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnMsgboardsRefreshAll();
	afx_msg void OnMsgboardNew();
	afx_msg void OnMsgboardDelete();
	afx_msg void OnDesktoptopics();
	afx_msg void OnGettinghelp();
	afx_msg void OnWindowhelp();
	afx_msg void OnFieldhelp();
	afx_msg void OnUserguide();
	afx_msg void OnIdd();
	afx_msg void OnSysadm();
	afx_msg void OnTechsupport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
