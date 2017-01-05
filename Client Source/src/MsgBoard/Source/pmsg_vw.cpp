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
// This is the implementation file for the CPostMsgView.
//


#include "stdafx.h"

#include "msgboard.h"

#include "msg_doc.h"
#include "pmsg_vw.h"
#include "dialogs.h"
#include "msgboard.h"
#include "mb_mgr.h"
#include "MsgBHelp.h"
#include <sizes.h>
#include <genutils.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPostMsgView constructor/destructor
// ____________________________________

CPostMsgView::CPostMsgView()
	: CFormView(CPostMsgView::IDD)
{
	//{{AFX_DATA_INIT(CPostMsgView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
}

CPostMsgView::~CPostMsgView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPostMsgView dynamic creation and message mapping
// __________________________________________________

IMPLEMENT_DYNCREATE(CPostMsgView, CFormView)

BEGIN_MESSAGE_MAP(CPostMsgView, CFormView)
	//{{AFX_MSG_MAP(CPostMsgView)
	ON_BN_CLICKED(IDC_POST_BTN, OnPostBtn)
	ON_BN_CLICKED(IDC_SELECT_MSGBOARD_BTN, OnPostToSelectBtn)
	ON_WM_KILLFOCUS()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)

END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// DoDataExchange
// ______________

void CPostMsgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostMsgView)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// OnInitialUpdate
// _______________

void CPostMsgView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	help_object.SetWindowHelp(HT_NewMessage_WindowDescription);

									   // Set up document so it will
									   //  no what 'type' it is and
									   //  also will know its view
	GetDocument()->SetMsgView(this);

	GetDocument()->SetMsgType(CMsgDoc::TO_POST_MSG);

//  GetParentFrame()->RecalcLayout();
//	ResizeParentToFit(TRUE);
//	ResizeParentToFit(FALSE);
}	


/////////////////////////////////////////////////////////////////////////////
// OnCancel
// ________

void CPostMsgView::OnCancel()
{
	if (ViewHasChangedData())
	{
    	if (AfxMessageBox("Trash this posting?", MB_YESNO) != IDYES)
    		return;
	}

	GetDocument()->OnCloseDocument();
}



/////////////////////////////////////////////////////////////////////////////
// OnPostBtn
// _________
//
// Gets called when user is ready to post the message.

void CPostMsgView::OnPostBtn()
{
                                       // First get data from Post To field
                                       // 
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
	char msg_postto_buf[SIZE_STANDARD_GENBUF];
	int idx = ctl->GetLine(NULL, msg_postto_buf, sizeof(msg_postto_buf));
	msg_postto_buf[idx] = '\0';
	trim(msg_postto_buf);
	                                   // Error if nothing there
	if (msg_postto_buf[0] == '\0')
	{
		AfxMessageBox("Please select message board to post to", MB_OK);
		ctl->SetFocus();
		return;
	}
	                                   // Now let document transfer from
	                                   //  msg_postto_buf to its
	                                   //  m_post_to_list, if problems
									   //  then m_errman will have
									   //  them recorded, display error
									   //  msg here and return
									   //
	if (!GetDocument()->SetPostToEntries(msg_postto_buf))
	{
		gbl_msgboard_man.m_errman.HandleError(MSG_BOARD_FACILITY);

		ctl->SetFocus();
		return;
	}
		                               // Now get the subject text
		                               //
	ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
	char msg_title_buf[SIZE_MSG_TITLE+1];
	idx = ctl->GetLine(NULL,msg_title_buf,SIZE_MSG_TITLE);
	msg_title_buf[idx] = '\0';
	
                                       // Here getting all the lines of
                                       //  message_text
                                       //
	ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	char msg_text_buf[SIZE_MSG_TEXT+1];
    
    int num_chars_so_far = 0;
    int cnt;
    char *msg_text_ptr = msg_text_buf;
    
	for (int i=0; i < ctl->GetLineCount(); i++)
	{
		cnt = ctl->GetLine(i, msg_text_ptr,(SIZE_MSG_TEXT - 
									        num_chars_so_far));
        num_chars_so_far += cnt;
        
		if (num_chars_so_far >= SIZE_MSG_TEXT)
			break;
			
		msg_text_ptr += cnt;
		*msg_text_ptr = '\n';
		msg_text_ptr++;
	}
									   // End text with a '\0'
	if (msg_text_ptr > msg_text_buf)
		*(msg_text_ptr-1) = '\0';
	else
	    msg_text_buf[0] = '\0';
                                       // Set doc's msg text and title
                                       //
	GetDocument()->SetMessageTitle(msg_title_buf);
	GetDocument()->SetMessageText(msg_text_buf);

                                       // Now setup and loop for each
                                       //  item in postto_list
                                       //
	BOOL ok = TRUE;
	BOOL got_errs = FALSE;
	BOOL first_time = TRUE;
	int  done_cnt = 0;

	for (i = 0; ((ok) && 
				 (i < GetDocument()->GetPostToMsgBoardListCnt())); i++)
	{
		if (GetDocument()->PostToMsgBoard(i, first_time))
		{
			done_cnt++;
		}
		else
		{
			CString errmsg = "Error posting message to message board ";
			errmsg += GetDocument()->GetPostToMsgBoardName(i);
			errmsg += "\n\nContinue processing message?";
			
			ok = (AfxMessageBox(errmsg, MB_YESNO) == IDYES);
			
			got_errs = TRUE;
		}
									   // See PostToMsgBoard about
									   //  how first_time parm reduces
									   //  amount of necessary msging
		first_time = FALSE;
	}
    
    CString result_msg;
	if ((!got_errs) && (done_cnt > 0))
	{
		if (done_cnt > 1)
		{
			char tmpbuf[SIZE_SMALL_GENBUF];
			sprintf(tmpbuf,
					"Message successfully posted to %d message boards",
					done_cnt);
			result_msg = tmpbuf;
		}
		else
		{
			result_msg = "Message successfully posted";
		}
	    AfxMessageBox(result_msg, MB_OK);
	}
	else if (done_cnt > 0)
	{
		char tmpbuf[SIZE_SMALL_GENBUF];
		sprintf(tmpbuf,
				"Message successfully posted to %d message boards",
				done_cnt);
		result_msg = tmpbuf;
	    AfxMessageBox(result_msg, MB_OK);
	}
	else
	{
	    AfxMessageBox("Message not posted", MB_OK);
	}
									   // Close document, done now 
	GetDocument()->OnCloseDocument();
}

                                                    

/////////////////////////////////////////////////////////////////////////////
// OnPostToSelectBtn
// _________________
//
// Gets called when user wants to select one or more message boards
// to post the message to.

void CPostMsgView::OnPostToSelectBtn()
{
	CMultiSelectMsgBoardDlg dlg;
	
	if (dlg.DoModal() == IDOK)
	{
		GetDocument()->SetModifiedFlag(TRUE);
		
		CEdit *ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
		CString post_to_str = dlg.CSelectedMsgBoardList();
		ctl->SetWindowText(LPCTSTR(post_to_str));
		ctl->SetModify();
	}
}



/////////////////////////////////////////////////////////////////////////////
// OnKillFocus
// ___________
//
// Called when the view loses the focus.  Resets the document title
// using substrings from the message 'post to' field and the message
// title fields.

void CPostMsgView::OnKillFocus(CWnd* pNewWnd)
{
    int idx = 0;
    
	char title_buf[SIZE_SMALL_GENBUF];

	CEdit *ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
	if (ctl->GetModify())
	{
		idx = ctl->GetLine(NULL, title_buf, MSG_TITLE_SENDER_PART_LEN);
    	title_buf[idx] = '\0';
    }

   	ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
	if (ctl->GetModify())
    {
    	if (idx != 0)
    		title_buf[idx++] = '/';

	
		idx += ctl->GetLine(NULL, &(title_buf[idx]), 
						    MSG_TITLE_SUBJECT_PART_LEN);
    	title_buf[idx] = '\0';
	}
	
	if (idx == 0)
		strcpy(title_buf,"New Message");
	
	GetDocument()->SetTitle(title_buf);	
	GetDocument()->SetPathName(title_buf);	
}



/////////////////////////////////////////////////////////////////////////////
// ViewHasData
// ___________
//
// Returns TRUE if any changes have been made to the title, 'post to'
// or message text fields (i.e. if the user has made changes to the
// message.

BOOL CPostMsgView::ViewHasChangedData()
{
                                       // If already set then is
                                       //  automatically TRUE, otherwise
                                       //  look at the three fields
	if (GetDocument()->IsModified())
		return(TRUE);

	CEdit *subject_ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
	CEdit *send_to_ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
	CEdit *message_txt = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);

	return(subject_ctl->GetModify() ||
		   send_to_ctl->GetModify() ||
		   message_txt->GetModify());
}



/////////////////////////////////////////////////////////////////////////////
// OnUpdate
// ________
//
// Called by the document.  For now only handling two cases, indicated
// by lHint.

void CPostMsgView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint == CHECK_UPDATES_HINT)
	{	
									   // See if user has changed message
									   //
    	GetDocument()->SetModifiedFlag(ViewHasChangedData());
	}
	else if (lHint == DOING_REWORK_HINT)
	{
									   // Resets view fields using
									   //  document data
									   //
		CEdit *ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
		ctl->SetWindowText(GetDocument()->GetTitle());
	
		ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
		ctl->SetWindowText(GetDocument()->GetPostToMsgBoardList());

		ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
		ctl->SetWindowText(GetDocument()->GetMessageText());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPostMsgView printing

void CPostMsgView::OnPrint( CDC *pDC, CPrintInfo *pInfo )
{

	// First get data from Post To field
    //								  
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_POST_TO);
	CString buf;
	ctl->GetWindowText(buf);
	pDC->TextOut(0,0,buf);
	// Now get the subject text
	//
	ctl = (CEdit *)GetDlgItem(IDC_TITLE_TEXT);
	ctl->GetWindowText(buf);
	pDC->TextOut(0,50,buf);
	// Here getting all the lines of
    //  message_text displaying line
    //  at a time
	//
	ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	int x=100;
	for (int i=0; i < ctl->GetLineCount(); i++)
	{
		ctl->GetLine(i,buf.GetBufferSetLength(ctl->LineLength(i)));
		pDC->TextOut(0,x,buf.GetBuffer(ctl->LineLength(i)));
		x+=50;
	}


}


BOOL CPostMsgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPostMsgView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPostMsgView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics
// ___________

#ifdef _DEBUG
void CPostMsgView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPostMsgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}


CMsgDoc* CPostMsgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMsgDoc)));
	return (CMsgDoc*)m_pDocument;
}
#endif //_DEBUG


void CPostMsgView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	
}

// next 2 functions get the paste to work

void CPostMsgView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
	
}

void CPostMsgView::OnEditPaste() 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->Paste();
}


void CPostMsgView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	int start_text,end_text;

	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->GetSel(start_text,end_text);
	pCmdUI->Enable(start_text!=end_text);
}

void CPostMsgView::OnEditCopy() 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->Copy();
}

void CPostMsgView::OnEditCut() 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->Cut();
	
}

void CPostMsgView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	int start_text,end_text;

	CEdit *ctl = (CEdit *)GetDlgItem(IDC_MESSAGE_TEXT);
	ctl->GetSel(start_text,end_text);
	pCmdUI->Enable(start_text!=end_text);	
}


BOOL CPostMsgView::PreTranslateMessage(MSG* pMsg)
{
  AUTOMATIC_HELP(pMsg);
  return CFormView::PreTranslateMessage(pMsg);
}


void CPostMsgView::OnDestroy() 
{
	CFormView::OnDestroy();
	help_object.SetWindowHelp(0);
	
}
