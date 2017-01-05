/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\dialogs.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file containing class declarations for all
// CDialog derived classes used in the MsgBoard application. 
//


//***************************************************************************
//***************************************************************************
// CMultiSelectMsgBoardDlg dialog
// ______________________________
//
// Allows user to select a number of message boards from a list of 
// all the message boards.

class CMultiSelectMsgBoardDlg : public CDialog
{
// Construction

public:
	CMultiSelectMsgBoardDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CMultiSelectMsgBoardDlg)
	enum { IDD = IDD_SELECT_MULTI_MSGBOARD };
	CListBox	m_msgboard_list_ctl;
	//}}AFX_DATA

// Attributes

private:
    BOOL  m_ok;
    int  *m_msgboard_idxs;        
    int   m_single_select_idx;
    
// Operations
public:
	CString CSelectedMsgBoardList();

// Implementation

public:
	~CMultiSelectMsgBoardDlg();	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMultiSelectMsgBoardDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//***************************************************************************
//***************************************************************************
// CNewMsgBoard dialog
// ____________________
//
// Dialog for collecting name of new message board to be created.

class CNewMsgBoard : public CDialog
{
// Construction
public:
	CNewMsgBoard(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNewMsgBoard)
	enum { IDD = IDD_NEW_MSGBOARD };
	CString	m_msgboard_name;
	BOOL	m_subscribe;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNewMsgBoard)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};



//***************************************************************************
//***************************************************************************
// CSingleSelectMsgBoardDlg dialog
// _______________________________
//
// Dialog for allowing selection of a single message board (e.g. the
// name of a message board to be opened).

class CSingleSelectMsgBoardDlg : public CDialog
{
// Construction
public:
	CSingleSelectMsgBoardDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSingleSelectMsgBoardDlg)
	enum { IDD = IDD_SELECT_SINGLE_MSGBOARD };
	CString	m_listbox_val;
	//}}AFX_DATA

// Attributes

	CString m_selection;
	BOOL	m_ok;

// Operations

	CString CSelectedMsgBoard()
		{ return (m_ok ? m_selection : ""); };


// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CSingleSelectMsgBoardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkMsgboardSingleList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//***************************************************************************
//***************************************************************************
// CSubscribeDlg dialog
// ____________________
//
// Dialog used to subscribe/unsubscribe from message boards.
// If successful then GetLists can be called to get selection
// results.  See implementation for details.

class CSubscribeDlg : public CDialog
{
// Construction
public:
	CSubscribeDlg(CWnd* pParent = NULL);	// standard constructor

	//{{AFX_DATA(CSubscribeDlg)
	enum { IDD = IDD_SUBSCRIBE };
	CListBox	m_subscribe_list_ctl;
	CListBox	m_avail_list_ctl;
	//}}AFX_DATA

// Attributes

private:
    BOOL m_ok;
    int  *m_subscribed_idxs;
    int  *m_not_subscribed_idxs;
     
// Operations

public:

	void GetLists(int *subscribe_list, int *not_subscribe_list);

// Implementation
public:
	~CSubscribeDlg();	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CSubscribeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnRemoveAll();
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




