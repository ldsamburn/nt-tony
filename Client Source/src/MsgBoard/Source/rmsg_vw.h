/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\rmsg_vw.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file for the CRecvMsgView class and
// related declarations.  This is the view for CMsgDoc when messages
// are being received (read).
//

#ifndef _INCLUDE_RMSG_VW_H_
#define _INCLUDE_RMSG_VW_H_

#include "mb_doc.h"

class CRecvMsgView : public CFormView
{
// Construction
protected: // create from serialization only
	CRecvMsgView();
	DECLARE_DYNCREATE(CRecvMsgView)

public:
	//{{AFX_DATA(CRecvMsgView)
	enum{ IDD = IDD_READ_MESSAGE_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
protected:
	CFont *m_the_font;

public:
	CMsgDoc* GetDocument();

// Operations
public:


// Implementation - general functions
public:
	virtual ~CRecvMsgView();

	void SetupTitle();

protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Overrides
public:
virtual	void CRecvMsgView::OnPrint( CDC *pDC, CPrintInfo *pInfo );
virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation - Generated message map functions
protected:
	//{{AFX_MSG(CRecvMsgView)
	virtual void OnInitialUpdate();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation - debug stuff
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

#ifndef _DEBUG  // debug version in rmsg_vw.cpp
inline CMsgDoc* CRecvMsgView::GetDocument()
   { return (CMsgDoc*)m_pDocument; }
#endif

#endif  // _INCLUDE_RMSG_VW_H_
