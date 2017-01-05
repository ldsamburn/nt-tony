/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\rmsg_vw.cpp                    
//
// WSP, last update: TBD
//                                        
// This is the implementation file for the CRecvMsgView.
//

#include "stdafx.h"
#include "msgboard.h"

#include "msg_doc.h"
#include "mb_mgr.h"
#include "rmsg_vw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecvMsgView constructor/destructor
// ____________________________________

CRecvMsgView::CRecvMsgView()
	: CFormView(CRecvMsgView::IDD)
{
	//{{AFX_DATA_INIT(CRecvMsgView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
}

CRecvMsgView::~CRecvMsgView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CRecvMsgView dynamic creation and message mapping
// __________________________________________________

IMPLEMENT_DYNCREATE(CRecvMsgView, CFormView)

BEGIN_MESSAGE_MAP(CRecvMsgView, CFormView)
	//{{AFX_MSG_MAP(CRecvMsgView)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// DoDataExchange
// ______________

void CRecvMsgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecvMsgView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// OnInitialUpdate
// _________________

void CRecvMsgView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GetDocument()->SetMsgView(this);
}	



/////////////////////////////////////////////////////////////////////////////
// SetupTitle
// __________
//
// Sets document title using mix of poster / title strings.
//

void CRecvMsgView::SetupTitle()
{
    int idx = 0;											   
    
	char title_buf[SIZE_SMALL_GENBUF];

	strcpy(title_buf, GetDocument()->GetPostFromMsgBoard());

	idx = strlen(title_buf);
	if (idx > MSG_TITLE_SENDER_PART_LEN)
		idx = MSG_TITLE_SENDER_PART_LEN;

	if (idx != 0)
	{
   		title_buf[idx++] = '/';
   		title_buf[idx] = '\0';
	}

	strcat(title_buf, GetDocument()->GetTitle());

	if (strlen(title_buf) > (MSG_TITLE_SENDER_PART_LEN + 
							 MSG_TITLE_SUBJECT_PART_LEN + 1))
	{
		title_buf[(MSG_TITLE_SENDER_PART_LEN + 
				   MSG_TITLE_SUBJECT_PART_LEN + 1)] = '\0';
	}
	GetDocument()->SetTitle(title_buf);	
}



/////////////////////////////////////////////////////////////////////////////
// OnUpdate
// ________

void CRecvMsgView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
	ctl->SetWindowText(GetDocument()->GetTitle());
	
	ctl = (CEdit *)GetDlgItem(IDC_POSTED_BY);
	ctl->SetWindowText(GetDocument()->GetPostFromMsgBoard());

	ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->SetWindowText(GetDocument()->GetMessageText());

	ctl = (CEdit *)GetDlgItem(IDC_POST_DATE);
	ctl->SetWindowText(GetDocument()->GetPostDate());
}

/////////////////////////////////////////////////////////////////////////////
// CPostMsgView printing

void CRecvMsgView::OnPrint( CDC *pDC, CPrintInfo *pInfo )
{
	pDC->TextOut(0,0,GetDocument()->GetPostFromMsgBoard());
	pDC->TextOut(0,50,GetDocument()->GetTitle());
    pDC->TextOut(0,100,GetDocument()->GetPostDate());
//	pDC->TextOut(0,150,GetDocument()->GetMessageText());
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	CString buf;
	int x=150;
	for (int i=0; i < ctl->GetLineCount(); i++)
	{
		ctl->GetLine(i,buf.GetBufferSetLength(ctl->LineLength(i)));
		pDC->TextOut(0,x,buf.GetBuffer(ctl->LineLength(i)));
		x+=60;
	}
	
}

BOOL CRecvMsgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRecvMsgView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRecvMsgView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics
// ___________

#ifdef _DEBUG
void CRecvMsgView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRecvMsgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}


CMsgDoc *CRecvMsgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMsgDoc)));
	return (CMsgDoc*)m_pDocument;
}
#endif //_DEBUG



//Disable Save on the toolbar
void CRecvMsgView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	
}

//Enable Copy in Form View
void CRecvMsgView::OnEditCopy() 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->Copy();	
}

void CRecvMsgView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	int start_text,end_text;

	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->GetSel(start_text,end_text);
	pCmdUI->Enable(start_text!=end_text);
	
}
