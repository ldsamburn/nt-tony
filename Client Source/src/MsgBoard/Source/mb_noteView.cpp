/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// mb_noteView.cpp : implementation of the CMb_noteView class
//

#include "stdafx.h"
#include "MsgBoard.h"

#include "mb_noteDoc.h"
#include "mb_noteView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView

IMPLEMENT_DYNCREATE(CMb_noteView, CEditView)

BEGIN_MESSAGE_MAP(CMb_noteView, CEditView)
	//{{AFX_MSG_MAP(CMb_noteView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView construction/destruction

CMb_noteView::CMb_noteView()
{
	// TODO: add construction code here

}

CMb_noteView::~CMb_noteView()
{
}

BOOL CMb_noteView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView drawing

void CMb_noteView::OnDraw(CDC* pDC)
{
	CMb_noteDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView printing

BOOL CMb_noteView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CMb_noteView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing.
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CMb_noteView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView diagnostics

#ifdef _DEBUG
void CMb_noteView::AssertValid() const
{
	CEditView::AssertValid();
}

void CMb_noteView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CMb_noteDoc* CMb_noteView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMb_noteDoc)));
	return (CMb_noteDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMb_noteView message handlers
