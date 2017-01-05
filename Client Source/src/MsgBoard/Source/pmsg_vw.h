/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\pmsg_vw.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file for the CPostMsgView class and
// related declarations.  This is the view for CMsgDoc when messages
// are being posted.
//


#ifndef _INCLUDE_PMSG_VW_H_
#define _INCLUDE_PMSG_VW_H_

#include "mb_doc.h"

class CPostMsgView : public CFormView
{
// Construction

protected: // create from serialization only
	CPostMsgView();
	DECLARE_DYNCREATE(CPostMsgView)

public:
	//{{AFX_DATA(CPostMsgView)
	enum{ IDD = IDD_POST_MESSAGE_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
private:
	CFont *m_the_font;

public:
	CMsgDoc* GetDocument();

// Operations
public:

// Implementation - general functions
public:
	virtual ~CPostMsgView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);


// Implementation - internal functions
protected:
    BOOL BeenModified();
	BOOL ViewHasChangedData();
	
// Overrides
public:
virtual	void CPostMsgView::OnPrint( CDC *pDC, CPrintInfo *pInfo );
virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation - Generated message map functions
protected:
	//{{AFX_MSG(CPostMsgView)
	virtual void OnInitialUpdate();
	afx_msg void OnCancel();
	afx_msg void OnPostBtn();
	afx_msg void OnPostToSelectBtn();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		// Implementation - debug stuff
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};



#ifndef _DEBUG  // debug version in pmsg_vw.cpp
inline CMsgDoc* CPostMsgView::GetDocument()
   { return (CMsgDoc*)m_pDocument; }
#endif

#endif //  _INCLUDE_PMSG_VW_H_
