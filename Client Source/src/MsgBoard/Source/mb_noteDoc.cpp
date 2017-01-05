/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// mb_noteDoc.cpp : implementation of the CMb_noteDoc class
//

#include "stdafx.h"
#include "MsgBoard.h"

#include "mb_noteDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMb_noteDoc

IMPLEMENT_DYNCREATE(CMb_noteDoc, CDocument)

BEGIN_MESSAGE_MAP(CMb_noteDoc, CDocument)
	//{{AFX_MSG_MAP(CMb_noteDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMb_noteDoc construction/destruction

CMb_noteDoc::CMb_noteDoc()
{
	// TODO: add one-time construction code here

}

CMb_noteDoc::~CMb_noteDoc()
{
}

BOOL CMb_noteDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMb_noteDoc serialization

void CMb_noteDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CMb_noteDoc diagnostics

#ifdef _DEBUG
void CMb_noteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMb_noteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMb_noteDoc commands
