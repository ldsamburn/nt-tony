/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqueudoc.cpp : implementation of the CPqueueDoc class
//

#include "stdafx.h"
#include "pqueue.h"

#include "pqueudoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPqueueDoc

IMPLEMENT_DYNCREATE(CPqueueDoc, CDocument)

BEGIN_MESSAGE_MAP(CPqueueDoc, CDocument)
	//{{AFX_MSG_MAP(CPqueueDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPqueueDoc construction/destruction

CPqueueDoc::CPqueueDoc()
{
	// TODO: add one-time construction code here
}

CPqueueDoc::~CPqueueDoc()
{
}

BOOL CPqueueDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPqueueDoc serialization

void CPqueueDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPqueueDoc diagnostics

#ifdef _DEBUG
void CPqueueDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPqueueDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPqueueDoc commands
