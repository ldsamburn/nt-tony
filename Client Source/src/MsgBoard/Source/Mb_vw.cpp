/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgBoardView.cpp : implementation file
//

#include "stdafx.h"
#include "sizes.h"
#include "genutils.h"

#include "genutils.h"
#include "msgboard.h"
#include "mb_doc.h"
#include "Mb_vw.h"
#include "mb_mgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LV_COLUMN listColumn;
LV_ITEM listItem;

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardView

IMPLEMENT_DYNCREATE(CMsgBoardView, CFormView)

CMsgBoardView::CMsgBoardView()
	: CFormView(CMsgBoardView::IDD)
{
	//{{AFX_DATA_INIT(CMsgBoardView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCurrentListViewColumn=-1;
}

CMsgBoardView::~CMsgBoardView()
{
}

void CMsgBoardView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBoardView)
	DDX_Control(pDX, IDC_LIST1, m_MessageList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgBoardView, CFormView)
	//{{AFX_MSG_MAP(CMsgBoardView)
	ON_COMMAND(ID_POST_MESSAGE_ELSEWHERE, OnPostMessageElsewhere)
	ON_UPDATE_COMMAND_UI(ID_POST_MESSAGE_ELSEWHERE, OnUpdatePostMessageElsewhere)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_COMMAND(ID_MESSAGES_READMESSAGE, OnMessagesReadmessage)
	ON_UPDATE_COMMAND_UI(ID_MESSAGES_READMESSAGE, OnUpdateMessagesReadmessage)
	ON_COMMAND(ID_MSGBOARDS_REFRESH_CURRENT, OnMsgboardsRefreshCurrent)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()
	

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardView diagnostics

#ifdef _DEBUG
void CMsgBoardView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMsgBoardView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardView message handlers


void CMsgBoardView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	CMsgBoardDoc* pDoc=(CMsgBoardDoc*)GetDocument();
	pDoc->SetMsgBoardView(this);  

	SetWindowText(pDoc->MsgBoardName()); 


	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
	
	CImageList  *pimagelist;   
	CBitmap    bitmap;
	CBitmap    bitmap2;


   // Generate the imagelist and associate with the list control

	pimagelist = new CImageList(); 
	pimagelist->Create(16, 16, TRUE, 2, 3);
	
	bitmap.LoadBitmap(IDB_NEWMAIL);  
	pimagelist->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap2.LoadBitmap(IDB_OLDMAIL);  
	pimagelist->Add(&bitmap2, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap2.DeleteObject();
		
	m_MessageList.SetImageList(pimagelist, LVSIL_SMALL);
	listItem.iImage   = 0;

	
	CString str_Columns="POSTING DATE,RECEIVED FROM,MESSAGE TITLE";


	m_MessageList.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\MSGBRD\\MESSAGELIST"
					 ,NULL,TRUE);

}


void CMsgBoardView::UpdateListView() 
{

    CMsgBoardDoc* pDoc=(CMsgBoardDoc*)GetDocument();

	m_MessageList.DeleteAllItems();

	if (pDoc->MsgListCnt() != 0)
	{
	

	const Sbasic_msglist_rec *msg_list_ptr = 
    									pDoc->MsgList();
	int nItem;
	
		for (int i = 0; i < pDoc->MsgListCnt(); i++)
		{

		

			nItem=m_MessageList.Write(msg_list_ptr->post_date);
			m_MessageList.Write(msg_list_ptr->msgboard_name);
			m_MessageList.Write(msg_list_ptr->title);
			if(msg_list_ptr->readsent_flag)
				m_MessageList.SetItem( nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
			m_MessageList.SetItemData(nItem, (DWORD) msg_list_ptr);
			msg_list_ptr++;

		}
	}
	else
	{
			listItem.iItem=0;
			listItem.pszText="No Messages";
			listItem.iSubItem=0;
			listItem.iImage=0;
			listItem.lParam=0;
			m_MessageList.InsertItem(&listItem);
	}

	m_MessageList.AutoSizeColumns();

	
}

void CMsgBoardView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (lHint != SHOW_MB_LIST_HINT)
		return;
	UpdateListView();
	
}



/////////////////////////////////////////////////////////////////////////////
// OnPostMessageElsewhere
// ______________________

void CMsgBoardView::OnPostMessageElsewhere() 
{
	CMsgBoardDoc* pDoc=(CMsgBoardDoc*)GetDocument();

	if (pDoc->MsgListCnt() == 0)
		{
			AfxMessageBox("No messages to post elsewhere for this message board", MB_OK);
			return;
		}


	const Sbasic_msglist_rec *msg_list_ptr = pDoc->MsgList();

	int idx = -1;

	if ((idx=m_MessageList.GetNextItem( idx, LVNI_SELECTED))==-1)
	{
		AfxMessageBox("Select message to read", MB_OK);
		return;
	}
		
	Sbasic_msglist_rec *msglist_ptr;
	msglist_ptr = (Sbasic_msglist_rec *)
    				  m_MessageList.GetItemData(idx);
    
										  // Found message, now open a
										  //  window for it
										  //
	CMsgDoc *orig_doc = gbl_msgboard_man.OpenMsg
    									(pDoc->MsgBoardName(),
										  msglist_ptr->title,
		 								  msglist_ptr->post_date,
										  msglist_ptr->msgboard_name,
										  msglist_ptr->key);

										   // And then have it posted
										   //  elsewhere
	if (orig_doc)
		gbl_msgboard_man.PostMsgElseWhere(orig_doc);
	
}



/////////////////////////////////////////////////////////////////////////////
// OnUpdatePostMessageElsewhere
// ____________________________
//
// Enable post messages elsewhere option only when a message is
// currently selected

void CMsgBoardView::OnUpdatePostMessageElsewhere(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MessageList.GetNextItem( -1, LVNI_SELECTED)>=0);
}


/////////////////////////////////////////////////////////////////////////////
// OnDblclkMessageList
// ___________________
//

void CMsgBoardView::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMsgBoardDoc* pDoc=(CMsgBoardDoc*)GetDocument();

//9-29 Denton's TR #10146
	if (pDoc->MsgListCnt() == 0)
	{
		AfxMessageBox("No messages to read for this message board", MB_OK);
		return;
	}
//9-29  


	int nIndex;
	if((nIndex=m_MessageList.GetNextItem( -1, LVNI_SELECTED))==-1)
		return;

	const Sbasic_msglist_rec *msg_list_ptr = pDoc->MsgList();

	Sbasic_msglist_rec *msglist_ptr;
    msglist_ptr = (Sbasic_msglist_rec *)
    			   m_MessageList.GetItemData(nIndex);

									   // Found message, now open a
									   //  window for it
									   //

	gbl_msgboard_man.OpenMsg(pDoc->MsgBoardName(),
							 msglist_ptr->title,
							 msglist_ptr->post_date,
							 msglist_ptr->msgboard_name,
							 msglist_ptr->key);

									   // Need to refresh since current
									   //  msg may have changed from
									   //  unread to read
	
	pDoc->RefreshMsgBoard();

	//m_MessageList.SetItemState(nIndex,LVIS_SELECTED,LVIS_SELECTED);
	//m_MessageList.EnsureVisible( nIndex, FALSE );

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// OnMessagesReadmessage
// _____________________
//
// Handles menu option for reading a message.

void CMsgBoardView::OnMessagesReadmessage()
{
	CMsgBoardDoc* pDoc=(CMsgBoardDoc*)GetDocument();

	if (pDoc->MsgListCnt() == 0)
	{
		AfxMessageBox("No messages to read for this message board", MB_OK);
		return;
	}

    const Sbasic_msglist_rec *msg_list_ptr = pDoc->MsgList();

	int idx = m_MessageList.GetNextItem( -1, LVNI_SELECTED);
	if (idx == -1)
	{
		AfxMessageBox("Select message to read", MB_OK);
		return;
	}
	
    Sbasic_msglist_rec *msglist_ptr;
    msglist_ptr = (Sbasic_msglist_rec *)
    			  m_MessageList.GetItemData(idx);

									   // Found message, now open a
									   //  window for it
									   //
	gbl_msgboard_man.OpenMsg(pDoc->MsgBoardName(),
							 msglist_ptr->title,
							 msglist_ptr->post_date,
							 msglist_ptr->msgboard_name,
							 msglist_ptr->key);

									   // Need to refresh since current
									   //  msg may have changed from
									   //  unread to read
	pDoc->RefreshMsgBoard();
}


/////////////////////////////////////////////////////////////////////////////
// OnUpdateMessagesReadmessage
// ____________________________
//
// Enable post messages elsewhere option only when a message is
// currently selected

void CMsgBoardView::OnUpdateMessagesReadmessage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_MessageList.GetNextItem( -1, LVNI_SELECTED)>=0);	
}



/////////////////////////////////////////////////////////////////////////////
// OnMessagesRefreshCurrent
// _____________________

void CMsgBoardView::OnMsgboardsRefreshCurrent()
{
	m_MessageList.DeleteAllItems();
	CMsgBoardDoc* pDoc = (CMsgBoardDoc*) GetDocument();
	BOOL ok = pDoc->ReadMsgBoard(pDoc->MsgBoardType());

	if (ok)
		return;

	else
	{
		AfxMessageBox("OnMessagesRefreshCurrent/error occurred");
	}
}



/////////////////////////////////////////////////////////////////////////////
// CPostMsgView printing

void CMsgBoardView::OnPrint( CDC *pDC, CPrintInfo *pInfo )
{
	    

	    CString blanks="         ";
		int x=0;
		int li_cnt=m_MessageList.GetItemCount();
		for (int i=0;i<li_cnt;i++){
			CString result=m_MessageList.GetItemText(i,0)+blanks;
			result+=m_MessageList.GetItemText(i,1)+blanks;
			result+=m_MessageList.GetItemText(i,2);
			pDC->TextOut(0,x,result);
			x+=60;
		}
}

BOOL CMsgBoardView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMsgBoardView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMsgBoardView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


