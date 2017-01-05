/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrinterUIDoc.cpp : implementation of the CPrinterUIDoc class
//

#include "stdafx.h"
#include "PrinterUI.h"

#include "PrinterUIDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIDoc

IMPLEMENT_DYNCREATE(CPrinterUIDoc, CDocument)

BEGIN_MESSAGE_MAP(CPrinterUIDoc, CDocument)
	//{{AFX_MSG_MAP(CPrinterUIDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIDoc construction/destruction

CPrinterUIDoc::CPrinterUIDoc()
{


}

CPrinterUIDoc::~CPrinterUIDoc()
{
}

BOOL CPrinterUIDoc::OnNewDocument()
{
	
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIDoc serialization

void CPrinterUIDoc::Serialize(CArchive& ar)
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
// CPrinterUIDoc diagnostics

#ifdef _DEBUG
void CPrinterUIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPrinterUIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIDoc commands
