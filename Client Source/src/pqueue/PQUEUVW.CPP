/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqueuvw.cpp : implementation of the CPqueueView class
//

#include "stdafx.h"
#include "pqueue.h"

#include "pqueudoc.h"
#include "pqueuvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPqueueView

IMPLEMENT_DYNCREATE(CPqueueView, CView)

BEGIN_MESSAGE_MAP(CPqueueView, CView)
	//{{AFX_MSG_MAP(CPqueueView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPqueueView construction/destruction

CPqueueView::CPqueueView()
{
	// TODO: add construction code here
}

CPqueueView::~CPqueueView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPqueueView drawing

void CPqueueView::OnDraw(CDC* pDC)
{
	CPqueueDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPqueueView diagnostics

#ifdef _DEBUG
void CPqueueView::AssertValid() const
{
	CView::AssertValid();
}

void CPqueueView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPqueueDoc* CPqueueView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPqueueDoc)));
	return (CPqueueDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPqueueView message handlers
