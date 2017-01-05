/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqueuvw.h : interface of the CPqueueView class
//
/////////////////////////////////////////////////////////////////////////////

class CPqueueView : public CView
{
protected: // create from serialization only
	CPqueueView();
	DECLARE_DYNCREATE(CPqueueView)

// Attributes
public:
	CPqueueDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CPqueueView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPqueueView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in pqueuvw.cpp
inline CPqueueDoc* CPqueueView::GetDocument()
   { return (CPqueueDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
