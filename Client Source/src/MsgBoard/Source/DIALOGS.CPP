/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\dialogs.cpp                    
//
// WSP, last update: TBD
//                                        
// This is the implementation file containing class declarations for 
// all CDialog derived classes used in the MsgBoard application. 
//

#include "stdafx.h"
#include "msgboard.h"
#include "dialogs.h"
#include "mb_mgr.h"
#include "MsgBHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//***************************************************************************
//***************************************************************************
// CMultiSelectMsgBoardDlg dialog
// ______________________________
//
// Allows user to select a number of message boards from a list of 
// all the message boards.


/////////////////////////////////////////////////////////////////////////////
// CMultiSelectMsgBoardDlg constructor/destructor
// _________________________________________

CMultiSelectMsgBoardDlg::CMultiSelectMsgBoardDlg(CWnd* pParent)
	: CDialog(CMultiSelectMsgBoardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMultiSelectMsgBoardDlg)
	//}}AFX_DATA_INIT

	m_msgboard_idxs = NULL;
	m_ok = FALSE;
	m_single_select_idx = -1;
}


CMultiSelectMsgBoardDlg::~CMultiSelectMsgBoardDlg()
{
	if (m_msgboard_idxs != NULL)
		delete [] m_msgboard_idxs;
}


/////////////////////////////////////////////////////////////////////////////
// CMultiSelectMsgBoardDlg::DoDataExchange and message mapping
// ______________________________________________________

void CMultiSelectMsgBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiSelectMsgBoardDlg)
	DDX_Control(pDX, IDC_MSGBOARD_MULTI_LIST, m_msgboard_list_ctl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMultiSelectMsgBoardDlg, CDialog)
	//{{AFX_MSG_MAP(CMultiSelectMsgBoardDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMultiSelectMsgBoardDlg::OnInitDialog
// _____________________________________
//
// Populate the list box with each of the message boards in 
// gbl_msgboard_man.MsgBoardList().
//

BOOL CMultiSelectMsgBoardDlg::OnInitDialog()
{
	if (!CDialog::OnInitDialog())
		return(FALSE);
		
	int idx;
	
    const Sbasic_msgboard_rec *msgboards = 
									gbl_msgboard_man.MsgBoardList();
    
	for (int i=0; i < gbl_msgboard_man.MsgBoardListCnt(); i++)
	{
		idx = m_msgboard_list_ctl.AddString(msgboards[i].msgboard_name);

		m_msgboard_list_ctl.SetItemData(idx, i);
	}


	
	
	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CMultiSelectMsgBoardDlg::OnOK
// _____________________________
//
// Allocates space for list of indexes and puts the indexes for the
// selected message boards into the list so that CSelectedMsgBoardList
// can later access.
//

void CMultiSelectMsgBoardDlg::OnOK()
{
	                                   // Get cnt of selected msgboards
	                                   //
	int cnt = m_msgboard_list_ctl.GetSelCount();
	                                 
	if (cnt == 0)
	{
		AfxMessageBox("Select Message Board or press cancel", MB_OK);
		return;
	}
									   // Allocate space
	int *listbox_idxs = new int[cnt];
	m_msgboard_idxs = new int[cnt+1];
	
	if ((listbox_idxs == NULL) || (m_msgboard_idxs == NULL))
	{
// WSPTODO - handle error better?
		AfxMessageBox("Memory allocation error", MB_OK);

		if (listbox_idxs != NULL)
			delete listbox_idxs;
		
		else if (m_msgboard_idxs != NULL)
			delete m_msgboard_idxs;

		return;
	}
                                       // Get selected items' indexes
                                       //
    cnt = m_msgboard_list_ctl.GetSelItems(cnt, listbox_idxs);

    DWORD data;
    int *tmpptr_listbox_idxs  = listbox_idxs;
    int *tmpptr_msgboard_idxs = m_msgboard_idxs;
    
    for (int i=0; i < cnt; i++)
    {
                                       // Now put indexes of message boards
                                       //  of message boards selected into
                                       //  m_msgboard_idxs list
                                       //                                       //  list 
		data = m_msgboard_list_ctl.GetItemData(*tmpptr_listbox_idxs);
        

        *tmpptr_msgboard_idxs = (int)data;

		tmpptr_listbox_idxs++;
		tmpptr_msgboard_idxs++;
    }
    *tmpptr_msgboard_idxs = -1;
    
	m_ok = TRUE;

	delete [] listbox_idxs;
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CMultiSelectMsgBoardDlg::CSelectedMsgBoardList
// _________________________________________
//
// Will return a CString containing a comma separating list of the message
// boards selected.  Returns NULL if there have been problems with the
// dialog, e.g. user did not select OK.
//

CString CMultiSelectMsgBoardDlg::CSelectedMsgBoardList()
{
	if (!m_ok)
		return("");
		
	CString msg_board_list_str = "";
	      
	const Sbasic_msgboard_rec *msgboard_list = gbl_msgboard_man.MsgBoardList();
	const Sbasic_msgboard_rec *tmpptr_msgboard_list;
	
	int *tmpptr_msgboard_idxs = m_msgboard_idxs;
	
	while (*tmpptr_msgboard_idxs != -1)
	{
		if (msg_board_list_str != "")
			msg_board_list_str += ", ";
			
		tmpptr_msgboard_list = msgboard_list + *tmpptr_msgboard_idxs;
		
		msg_board_list_str += tmpptr_msgboard_list->msgboard_name;
        tmpptr_msgboard_idxs++;
	}

	return(msg_board_list_str);
}



//***************************************************************************
//***************************************************************************
// CNewMsgBoard dialog
// ___________________
//
// Gets name of message board that is to be created.

CNewMsgBoard::CNewMsgBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CNewMsgBoard::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewMsgBoard)
	m_msgboard_name = "";
	m_subscribe = TRUE;
	//}}AFX_DATA_INIT
}

void CNewMsgBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewMsgBoard)
	DDX_Text(pDX, IDC_MSGBOARD_NAME, m_msgboard_name);
	DDX_Check(pDX, IDC_SUBSCRIBE_CHK, m_subscribe);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewMsgBoard, CDialog)
	//{{AFX_MSG_MAP(CNewMsgBoard)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//***************************************************************************
//***************************************************************************
// CSubscribeDlg dialog
// ____________________
//
// Allows user to select a number of message boards to subscribe to.  All
// available message boards from gbl_msgboard_man.MsgBoardList() will be
// displayed - either in the already subscribed to list or in the available
// list.  If successful (no errors and no user cancelation), then 
// function GetLists() will provide the caller with the the two lists
// (subscribed to and still available) selected by the user

/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg constructor/destructor
// ____________________________________

CSubscribeDlg::CSubscribeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubscribeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSubscribeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ok = FALSE;
    m_subscribed_idxs = NULL;
    m_not_subscribed_idxs = NULL;

}


CSubscribeDlg::~CSubscribeDlg()
{
	if (m_subscribed_idxs != NULL)
		delete [] m_subscribed_idxs;

	if (m_not_subscribed_idxs != NULL)
		delete [] m_not_subscribed_idxs;
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::DoDataExchange
// _____________________________

void CSubscribeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubscribeDlg)
	DDX_Control(pDX, IDC_SUBSCRIBE_LIST, m_subscribe_list_ctl);
	DDX_Control(pDX, IDC_AVAIL_LIST, m_avail_list_ctl);
	//}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg message map
// _________________________

BEGIN_MESSAGE_MAP(CSubscribeDlg, CDialog)
	//{{AFX_MSG_MAP(CSubscribeDlg)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::GetLists
// _______________________
//
// If m_ok then this function will return in the array pointed to by
// subscribe_list, a list of indexes into gbl_msgboard_man.MsgBoardList()
// for message boards that the user wants (or already has) subscribed to.
// A -1 indicates the end of the list.  Similarly, the array pointed to by
// not_subscribe_list is set with the indexes for the message boards that the
// user has selected not to subscribe to.
//

void CSubscribeDlg::GetLists(int *subscribe_list, int *not_subscribe_list)
{
	ASSERT(m_ok);
		
	int *src_tmp_ptr = m_subscribed_idxs;
	int *dst_tmp_ptr = subscribe_list;
	
	while (*src_tmp_ptr != -1)
		*dst_tmp_ptr++ = *src_tmp_ptr++;

	*dst_tmp_ptr = -1;


	src_tmp_ptr = m_not_subscribed_idxs;
	dst_tmp_ptr = not_subscribe_list;
	
	while (*src_tmp_ptr != -1)
		*dst_tmp_ptr++ = *src_tmp_ptr++;

	*dst_tmp_ptr = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnAdd
// ____________________
//
// Moves the currently selected message boards from the available
// list to the subscribed list.

void CSubscribeDlg::OnAdd()
{
	int cnt = m_avail_list_ctl.GetSelCount();
	
	if (cnt == 0)
	{
		AfxMessageBox("Select subscribed Message Board to add", MB_OK);
		return;
	}
	
	int *sel_idxs = new int[cnt];
	
	if (sel_idxs == 0)
	{
// WSPTODO - handle error better

		AfxMessageBox("Memory allocation error", MB_OK);
		return;
	}

    cnt = m_avail_list_ctl.GetSelItems(cnt, sel_idxs);
    
	char temp_buf[SIZE_MSGBOARD_NAME+1];
    int idx;
    DWORD data;
									   // Move selected items from
									   //  available list to the
									   //  subscribed list
    for (int i=0; i < cnt; i++)
    {
		m_avail_list_ctl.GetText(*(sel_idxs+i), temp_buf);
		data = m_avail_list_ctl.GetItemData(*(sel_idxs+i));
		
		idx = m_subscribe_list_ctl.AddString(temp_buf);
		m_subscribe_list_ctl.SetItemData(idx, data);
    }
									   // Then remove from the 
									   //  available list
    for (i=cnt-1; i >= 0; i--)
    {
        m_avail_list_ctl.DeleteString(*(sel_idxs+i));
    }

	delete []sel_idxs;
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnCancel
// _______________________

void CSubscribeDlg::OnCancel()
{
	CDialog::OnCancel();
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnInitDialog
// ___________________________
//
// For each SUBSCRIBED_MSGBOARD and SUBSCRIBABLE_MSGBOARD message 
// board in gbl_msgboard_man.MsgBoardList(), this function
// will add the name to either the subscribe to list (m_subscribe_list_ctl)
// or the available list (m_avail_list_ctl) and also associate the 
// gbl_msgboard_man.MsgBoardList() index for the message board
// as the list box item data.
//

BOOL CSubscribeDlg::OnInitDialog()
{
	if (!CDialog::OnInitDialog())
		return(FALSE);
		
	int idx;
	
    const Sbasic_msgboard_rec *msgboards = gbl_msgboard_man.MsgBoardList();
    
	for (int i=0; i < gbl_msgboard_man.MsgBoardListCnt(); i++)
	{
		if (msgboards[i].msgboard_subsc_type == SUBSCRIBED_MSGBOARD)
        {
			if (strchr(msgboards[i].msgboard_name, '.') == NULL)
			{
				idx = m_subscribe_list_ctl.AddString
									(msgboards[i].msgboard_name);
                                                  
				m_subscribe_list_ctl.SetItemData(idx, i);
			}

		} 
		else if (msgboards[i].msgboard_subsc_type == SUBSCRIBABLE_MSGBOARD)
        {
			idx = m_avail_list_ctl.AddString
									(msgboards[i].msgboard_name);

			m_avail_list_ctl.SetItemData(idx, i);
		} 
	}
	
	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnOK
// ___________________
//
// This function will save the state of the currently selection of
// message boards into m_subscribed_idxs and m_not_subscribed_idxs.
// Arrays of integers containing indexes into gbl_msgboard_man.MsgBoardList()
// for selected and unselected message boards.
//

void CSubscribeDlg::OnOK()
{
	m_subscribed_idxs = new int[(m_subscribe_list_ctl.GetCount()+1)];
	m_not_subscribed_idxs = new int[(m_avail_list_ctl.GetCount()+1)];
	
	if (( m_subscribed_idxs == NULL) ||
	    ( m_not_subscribed_idxs == NULL) )
	{
// WSPTODO - handle error better
		AfxMessageBox("Memory allocation error", MB_OK);
		return;
	}

	int *temp_ptr = m_subscribed_idxs;
    for (int i = 0; i < m_subscribe_list_ctl.GetCount(); i++)
    {
		*temp_ptr = (int) m_subscribe_list_ctl.GetItemData(i);

		temp_ptr++;
    }
    *temp_ptr = -1;
    
	temp_ptr = m_not_subscribed_idxs;
    for (i = 0; i < m_avail_list_ctl.GetCount(); i++)
    {
		*temp_ptr = (int) m_avail_list_ctl.GetItemData(i);

		temp_ptr++;
    }
    *temp_ptr = -1;
    
	m_ok = TRUE;

	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnRemove
// _______________________
//
// Moves the currently selected message board from the subscribed list
// to the available list.

void CSubscribeDlg::OnRemove()
{
	int cnt = m_subscribe_list_ctl.GetSelCount();
	
	if (cnt == 0)
	{
		AfxMessageBox("Select subcribed Message Board to remove", MB_OK);
		return;
	}
	
	int *sel_idxs = new int[cnt];
	
	if (sel_idxs == 0)
	{
		AfxMessageBox("Memory allocation error", MB_OK);
		return;
	}

    cnt = m_subscribe_list_ctl.GetSelItems(cnt, sel_idxs);
    
	char temp_buf[SIZE_MSGBOARD_NAME+1];
    int idx;
    DWORD data;
									   // Move from subscribed list
									   //  to available list
    for (int i=0; i < cnt; i++)
    {
		m_subscribe_list_ctl.GetText(*(sel_idxs+i), temp_buf);
		data = m_subscribe_list_ctl.GetItemData(*(sel_idxs+i));
		
		idx = m_avail_list_ctl.AddString(temp_buf);
		m_avail_list_ctl.SetItemData(idx, data);
    }
									   // Remove from subscribed list
    for (i=cnt-1; i >= 0; i--)
    {
        m_subscribe_list_ctl.DeleteString(*(sel_idxs+i));
    }

	delete []sel_idxs;
}



/////////////////////////////////////////////////////////////////////////////
// CSubscribeDlg::OnRemoveAll
// __________________________
//
// Moves all the message boards from the subscribed list to the 
// available list.

void CSubscribeDlg::OnRemoveAll()
{
	char temp_buf[SIZE_MSGBOARD_NAME+1];
    int idx;
    DWORD data;
    
    for (int i=0; i < m_subscribe_list_ctl.GetCount(); i++)
    {
		m_subscribe_list_ctl.GetText(i, temp_buf);
		data = m_subscribe_list_ctl.GetItemData(i);
		
		idx = m_avail_list_ctl.AddString(temp_buf);
		m_avail_list_ctl.SetItemData(idx, data);
    }
    
    m_subscribe_list_ctl.ResetContent();
}



//***************************************************************************
//***************************************************************************
// CSingleSelectMsgBoardDlg dialog
// _______________________________
//
// Allows users to select a single message board from 
// SUBSCRIBED_MSGBOARD in gbl_msgboard_man.MsgBoardList()


/////////////////////////////////////////////////////////////////////////////
// CSingleSelectMsgBoardDlg constructor, DoDataExchange and msg maps
// _________________________________________________________________

CSingleSelectMsgBoardDlg::CSingleSelectMsgBoardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleSelectMsgBoardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSingleSelectMsgBoardDlg)
	m_listbox_val = "";
	//}}AFX_DATA_INIT

	m_ok = FALSE;
}



void CSingleSelectMsgBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSingleSelectMsgBoardDlg)
	DDX_LBString(pDX, IDC_MSGBOARD_SINGLE_LIST, m_listbox_val);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CSingleSelectMsgBoardDlg, CDialog)
	//{{AFX_MSG_MAP(CSingleSelectMsgBoardDlg)
	ON_LBN_DBLCLK(IDC_MSGBOARD_SINGLE_LIST, OnDblclkMsgboardSingleList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// OnDblclkMsgboardSingleList
// __________________________

void CSingleSelectMsgBoardDlg::OnDblclkMsgboardSingleList()
{
	m_selection = m_listbox_val;
	OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
// ____________
//
// Populate the list box with each of the message boards in 
// gbl_msgboard_man.MsgBoardList() that are currently subscribed to.

BOOL CSingleSelectMsgBoardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ok = FALSE;
	
	CListBox *list_box = (CListBox *)GetDlgItem(IDC_MSGBOARD_SINGLE_LIST);
	
	int idx;
	
    const Sbasic_msgboard_rec *msgboards = gbl_msgboard_man.MsgBoardList();
    
	for (int i=0; i < gbl_msgboard_man.MsgBoardListCnt(); i++)
	{
		if (msgboards[i].msgboard_subsc_type == SUBSCRIBED_MSGBOARD)
	
			idx = list_box->AddString(msgboards[i].msgboard_name);
	}

	help_object.SetWindowHelp(HT_NewMessageBoard_WindowDescription);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}



/////////////////////////////////////////////////////////////////////////////
// OnOK
// ____

void CSingleSelectMsgBoardDlg::OnOK()
{
	m_ok = TRUE;
	
	UpdateData(TRUE);

	if (m_listbox_val == "")
	{
		AfxMessageBox("Select Message Board or press cancel", MB_OK);
		return;
	}
	
	m_selection = m_listbox_val;

	CDialog::OnOK();
}


void CNewMsgBoard::OnDestroy() 
{
	CDialog::OnDestroy();
	help_object.SetWindowHelp(0);
}

BOOL CNewMsgBoard::PreTranslateMessage(MSG* pMsg)
{
  AUTOMATIC_HELP(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}

BOOL CNewMsgBoard::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	help_object.SetWindowHelp(HT_NewMessageBoard_WindowDescription);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
