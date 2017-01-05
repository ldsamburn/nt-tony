/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgBoardView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "MyListCtrl.h"

class CMsgBoardView : public CFormView
{
protected:
	CMsgBoardView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMsgBoardView)

// Form Data
public:
	//{{AFX_DATA(CMsgBoardView)
	enum { IDD = IDD_MSGBOARD_FORM };
	MyListCtrl	m_MessageList;
	//}}AFX_DATA

virtual	void CMsgBoardView::OnPrint( CDC *pDC, CPrintInfo *pInfo );
virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBoardView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMsgBoardView();
	void UpdateListView(); 
	int m_nCurrentListViewColumn;
	BOOL m_bAscending;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMsgBoardView)
	afx_msg void OnPostMessageElsewhere();
	afx_msg void OnUpdatePostMessageElsewhere(CCmdUI* pCmdUI);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMessagesReadmessage();
	afx_msg void OnUpdateMessagesReadmessage(CCmdUI* pCmdUI);
	afx_msg void OnMsgboardsRefreshCurrent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
