/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mb_mgr.cpp                    
//
// WSP, last update: TBD
//                                        
// This is the implementation file for the CMsgBoardMgr class.
//

#include "stdafx.h"

#define DEFINE_THE_MSGBOARD_MAN

#include "ntcssapi.h"
#include "ntcssdef.h"

#include "msg_doc.h"
#include "mb_doc.h"
#include "msgboard.h"
#include "mb_mgr.h"
#include "mb_vw.h"
#include "mainfrm.h"
#include "msgs.h"

#include "dialogs.h"


/////////////////////////////////////////////////////////////////////////////
// CMsgBoardMgr constructor/destructor
// ___________________________________

CMsgBoardMgr::CMsgBoardMgr()
{
	m_main_msgboard_list = NULL;
	m_msgboard_cnt  = 0;
	m_msgboard_spare_cnt = 0;

    m_user_in_msg_board   = "";
	m_user_name           = "";
	m_server_name         = "";
}


CMsgBoardMgr::~CMsgBoardMgr()
{
	if (m_main_msgboard_list != NULL)
		delete [] m_main_msgboard_list;
}                            



/////////////////////////////////////////////////////////////////////////////
// CreateMsgBoard
// ______________

void CMsgBoardMgr::CreateMsgBoard()
{
	CNewMsgBoard dlg;
									   // Return now if dlg not ok'd
	if (dlg.DoModal() != IDOK)
		return;

	CmsgADDMSGBB addmsgbb_msg;
									   // First need to ADDMSGBB
									   //
	BOOL ok = addmsgbb_msg.Load((LPCSTR)dlg.m_msgboard_name);

	if (ok)
		ok = addmsgbb_msg.DoItNow();

	if (!ok)
	{
		m_errman.RecordError("Error in creating message board(ADDMSGBB)");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return;
	}
									   // Next do SUBSCRIBEBB if
									   //  necessary
	if (dlg.m_subscribe)
	{
		CmsgSUBSCRIBEBB subscribe_msg;

		ok = subscribe_msg.Load((LPCSTR)dlg.m_msgboard_name, 
								 TRUE);
		if (ok)
			ok = subscribe_msg.DoItNow();

		if (!ok)
		{
			m_errman.RecordError("Message board added but unable to subscribe (SUBSCRIBEBB)");
			m_errman.HandleError(MSG_BOARD_FACILITY);
			return;
		}
	}
									   // If have a spare slot in
									   //  m_main_msgboard_list then
									   //  add new message board to
									   //  it
	CString msg;
	if (m_msgboard_spare_cnt > 0)
	{
		Sbasic_msgbrd_rec *msgboard_spare_ptr;
		msgboard_spare_ptr = 
					(m_main_msgboard_list+m_msgboard_cnt);

		strcpy(msgboard_spare_ptr->msgboard_name,
			   (LPCSTR)dlg.m_msgboard_name);
		msgboard_spare_ptr->unread_msg_cnt = 0;

		if (dlg.m_subscribe)
		{
			msgboard_spare_ptr->msgboard_subsc_type = SUBSCRIBED_MSGBOARD;
			msg = "Message board added and subscribed to";
		}
		else
		{
			msgboard_spare_ptr->msgboard_subsc_type = SUBSCRIBABLE_MSGBOARD;
			msg = "Message board added";
		}
		m_msgboard_cnt++;
		m_msgboard_spare_cnt--;
	}
									   // If no spare slots then for
									   //  now give msg
	else
	{
		msg = "Message board added, you must restart application before the new message board is available";
	}
	AfxMessageBox(msg);
}




/////////////////////////////////////////////////////////////////////////////
// DeleteMsgBoard
// ______________

void CMsgBoardMgr::DeleteMsgBoard()
{
	CSingleSelectMsgBoardDlg dlg;
									   // Return now if dlg not ok'd
	if (dlg.DoModal() != IDOK)
		return;

	CmsgDELMSGBB delmsgbb_msg;
									   // First need to ADDMSGBB
									   //
	BOOL ok = delmsgbb_msg.Load((LPCSTR)dlg.m_selection);

	if (ok)
		ok = delmsgbb_msg.DoItNow();

	if (!ok)
	{
		m_errman.RecordError("Error in deleting message board(DELMSGBB)");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return;
	}

AfxMessageBox("Need to remove from view list, etc. now");

}




/////////////////////////////////////////////////////////////////////////////
// FindAndSetMsgType
// _________________
//
// Goes through m_main_msgboard_list Sbasic_msgboard_rec's until
// one is found whose msgboard_name matches src_msgboard_list_ptr's
// msgboard_name.  Then the located record has its msgboard_subsc_type
// set to new_msgboard_subsc_type.
 
BOOL CMsgBoardMgr::FindAndSetMsgType
					(Sbasic_msgbrd_rec *src_msgboard_list_ptr,
					 EMsgBoardSubscribeType new_msgboard_subsc_type)
{
	ASSERT (m_main_msgboard_list != NULL);

	Sbasic_msgboard_rec *temp_msgboard_list_ptr = m_main_msgboard_list;

	for (int i=0; i < m_msgboard_cnt; i++)
	{
		if (strcmp(src_msgboard_list_ptr->msgboard_name,
				   temp_msgboard_list_ptr->msgboard_name)
			== 0)
		{
			temp_msgboard_list_ptr->msgboard_subsc_type =	new_msgboard_subsc_type;

			return(TRUE);
		}
		temp_msgboard_list_ptr++;
	}

	return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// Initialize
// ________________
//
// MsgBoard application cannot proceed unless Initialize is
// successful.

BOOL CMsgBoardMgr::Initialize()
{
								       // Init the DLL
	if (!NtcssDLLInitialize
            (NTCSS_DLL_VERSION,
			 NTCSS_APP_NAME,
             AfxGetInstanceHandle(),
             AfxGetMainWnd()->m_hWnd))
	{
		return(FALSE);
	}
									   // From here out MsgBoard is
									   //  handling its own errors
	NtcssErrorMessagingOFF();

	NtcssUserInfo user_info;

	BOOL ok = NtcssGetUserInfo(&user_info);

	if (ok)
	{
		
		m_user_name = user_info.login_name;
		//TR10147  login_name is only 8 why isn't everything else screwing up
		m_user_name.GetBufferSetLength(NTCSS_SIZE_LOGINNAME);
		m_user_name.ReleaseBuffer();
		m_user_name.TrimRight();
		//TR10147
		m_user_in_msg_board  = m_user_name;
	    m_user_in_msg_board += " - MsgBoard";
	
		ok = LoadMainMsgBoardList();
	}

	if (ok)
	{
		char server_name[NTCSS_SIZE_HOSTNAME+1];
		ok = NtcssGetServerName(server_name, sizeof(server_name));
		if (ok)
			m_server_name = server_name;
	}

/*	if (ok)
		ok = m_SmallImageList.Create(IDR_MSG_ICONS,16,1,RGB(255,255,255));

	CDC *pDC;

	if (ok)
	{
		m_mb_font = new CFont;

		if (m_mb_font != NULL)
		{
			pDC = AfxGetMainWnd()->GetDC();

			BOOL ok = (pDC != NULL);
		}
		else
			ok = FALSE;
	}
	if (ok)
    	ok = m_mb_font->CreateFont(MulDiv(6,(-pDC->GetDeviceCaps(LOGPIXELSY)-30),72),
        	                  0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
                              OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,
                              DRAFT_QUALITY,FIXED_PITCH | FF_MODERN,
                              "CourierNew"); // Courier");

	if (pDC != NULL)
     	AfxGetMainWnd()->ReleaseDC(pDC);*/

	if (ok)
		return(TRUE);
	else
	{		 
		m_errman.RecordError("MsgBoard initialization error");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return(FALSE);
	}
}



/////////////////////////////////////////////////////////////////////////////
// LoadMainMsgBoardList
// ____________________

BOOL CMsgBoardMgr::LoadMainMsgBoardList()
{
	BOOL ok;
	Sbasic_msgboard_rec *msgboard_list_ptr = NULL;
	Sbasic_msgboard_rec *temp_msgboard_list_ptr = NULL;
	int list_cnt;


	{ // start block for CmsgGETALLBBS

									   // First get all the MsgBoards
		CmsgGETALLBBS getall_msg;


		ok = getall_msg.Load();
		if (ok)
			ok = getall_msg.DoItNow();
		
//		if (ok) Sleep(2000);//jgj


		if (ok)
			ok = getall_msg.GetListCount(&list_cnt);

		if ((ok) && (list_cnt == 0))
		{
			ok = FALSE;
		}

		if (ok) 
		{
			msgboard_list_ptr = new Sbasic_msgboard_rec
									[list_cnt + SPARE_SLOT_CNT];

			if (msgboard_list_ptr == NULL)
				ok = FALSE;
		}

		if (ok)
		{
			ok = getall_msg.UnLoad(list_cnt, 
								   &list_cnt,
								   msgboard_list_ptr);
		}
	
	} // end block for CmsgGETALLBBS


									   // if ok then now set member 
									   //  values and initialize all
									   //  msgboards as not being able 
									   //  subscribed to
	if (ok)
	{
		m_main_msgboard_list = msgboard_list_ptr;
		m_msgboard_cnt = list_cnt;
		m_msgboard_spare_cnt = SPARE_SLOT_CNT;
		msgboard_list_ptr = NULL;

		temp_msgboard_list_ptr = m_main_msgboard_list;
		for (int i=0; i < list_cnt; i++)
		{
			temp_msgboard_list_ptr->msgboard_subsc_type = NOT_SUBSCRIBABLE_MSGBOARD; 
			temp_msgboard_list_ptr++;
		}
	}


	{ // start block for CmsgGETUNSUBSCRIBEDBBS

									   // Now get the MsgBoard's
									   //  that are not subscribed to
									   //  but could be subscribed to
									   //
		CmsgGETUNSUBSCRIBEDBBS get_unsubscribe_msg;
		if (ok)
		{
			ok = get_unsubscribe_msg.Load();
		}

		if (ok)
			ok = get_unsubscribe_msg.DoItNow();

//		if (ok)Sleep(2000);//jgj

		if (ok)
			ok = get_unsubscribe_msg.GetListCount(&list_cnt);

		msgboard_list_ptr = NULL;

		if ((ok) && (list_cnt != 0))
		{
			msgboard_list_ptr = new Sbasic_msgboard_rec[list_cnt];

			if (msgboard_list_ptr == NULL)
				ok = FALSE;
		}
		if ((ok) && (list_cnt != 0))
		{
			ok = get_unsubscribe_msg.UnLoad(list_cnt, 
									    &list_cnt,
									    msgboard_list_ptr);
		}
	
	} // end block for CmsgGETUNSUBSCRIBEDBBS
									   
									   // Now go thru the list and mark
									   //  these message boards as being
									   //  able to be subscribed to
									   //
	if ((ok) && (msgboard_list_ptr != NULL))
	{
		temp_msgboard_list_ptr = msgboard_list_ptr;

		for (int i=0; ((ok) && (i < list_cnt)); i++)
		{
			ok = FindAndSetMsgType(temp_msgboard_list_ptr,
								   SUBSCRIBABLE_MSGBOARD);
			temp_msgboard_list_ptr++;
		}
	}

	if (msgboard_list_ptr != NULL)
		delete [] msgboard_list_ptr;


	{ // start block for CmsgSUBSCRIBEBB

									   // Now get the MsgBoard's
									   //  that are subscribed to
									   //
		CmsgGETSUBSCRIBEDBBS get_subscribe_msg;
		if (ok)
		{
			ok = get_subscribe_msg.Load();
		}

		if (ok)
			ok = get_subscribe_msg.DoItNow();

//		if (ok)Sleep(2000);//jgj

		if (ok)
			ok = get_subscribe_msg.GetListCount(&list_cnt);

		msgboard_list_ptr = NULL;

		if ((ok) && (list_cnt != 0))
		{
			msgboard_list_ptr = new Sbasic_msgboard_rec[list_cnt];

			if (msgboard_list_ptr == NULL)
				ok = FALSE;
		}
		if (ok)
		{
			ok = get_subscribe_msg.UnLoad(list_cnt, 
									  &list_cnt,
									  msgboard_list_ptr);
		}
	
	} // end block for CmsgSUBSCRIBEBB

									   // Now go thru the list and mark
									   //  these message boards as being
									   //  able to be subscribed to
									   //
	if ((ok) && (msgboard_list_ptr != NULL))
	{
		temp_msgboard_list_ptr = msgboard_list_ptr;

		for (int i=0; ((ok) && (i < list_cnt)); i++)
		{
			ok = FindAndSetMsgType(temp_msgboard_list_ptr,
								   SUBSCRIBED_MSGBOARD);
			temp_msgboard_list_ptr++;
		}
	}

	if (msgboard_list_ptr != NULL)
		delete [] msgboard_list_ptr;

	return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// MsgBoardSubscribe
// ______________
//
// This function uses CSubscribeDlg to allow the user to select 
// message boards to subscribe to/from, then sends the server 
// messages to do the actual subscribing/unsubscribing.
// 
// WSPTODO - for some reason can't reuse a msg, so until this is
// fixed am creating a new one every time and then deleting it - need
// to come back to this problem

void CMsgBoardMgr::MsgBoardSubscribe()
{						 
	CSubscribeDlg dlg;

	if (dlg.DoModal() != IDOK)
		return;
	
	int *subscribe_list     = new int[m_msgboard_cnt];	
	int *not_subscribe_list = new int[m_msgboard_cnt];
	if ((subscribe_list == NULL) || (not_subscribe_list == NULL))
	{
		m_errman.RecordError("Memory allocation error");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return;
	}
	memset(subscribe_list, 0, sizeof(subscribe_list));
	memset(subscribe_list, 0, sizeof(not_subscribe_list));

	dlg.GetLists(subscribe_list,not_subscribe_list);

	int *ptr = subscribe_list;

	CmsgSUBSCRIBEBB *subscribe_msg;
	BOOL ok = TRUE;
								       // First have to subscribe
									   //  to the msgboard's in
									   //  subscribe_list that are not 
									   //  already type 
									   //  SUBSCRIBED_MSGBOARD
	int subscribed_cnt = 0;
	while ((ok) && (*ptr != -1))
	{
		if (m_main_msgboard_list[*ptr].msgboard_subsc_type 
			!= SUBSCRIBED_MSGBOARD)
		{
			subscribe_msg = new CmsgSUBSCRIBEBB;

			ok = subscribe_msg->Load(m_main_msgboard_list[*ptr].msgboard_name, 
								     TRUE);
			if (ok)
				ok = subscribe_msg->DoItNow();

			delete subscribe_msg;

									   // Also change msgboard_subsc_type
									   //
			m_main_msgboard_list[*ptr].msgboard_subsc_type =
												SUBSCRIBED_MSGBOARD;
			if (ok)
				subscribed_cnt++;
		}
		ptr++;

	}
								       // Now have to unsubscribe
									   //  from the msgboard's in
									   //  subscribe_list are already
									   //  type SUBSCRIBED_MSGBOARD
	ptr = not_subscribe_list;

	int unsubscribed_cnt = 0;
	while ((ok) && (*ptr != -1))
	{

		if (m_main_msgboard_list[*ptr].msgboard_subsc_type 
			== SUBSCRIBED_MSGBOARD)
		{
			subscribe_msg = new CmsgSUBSCRIBEBB;

			ok = subscribe_msg->Load(m_main_msgboard_list[*ptr].msgboard_name, 
								   FALSE);
			if (ok)
				ok = subscribe_msg->DoItNow();

									   // Also change msgboard_subsc_type
									   //
			delete subscribe_msg;					
												  
			m_main_msgboard_list[*ptr].msgboard_subsc_type =
											SUBSCRIBABLE_MSGBOARD;

			if (ok)
				unsubscribed_cnt++;
		}
		ptr++;

	}

	if ((ok) && ((subscribed_cnt != 0) || (unsubscribed_cnt != 0)))
	{
		char msgbuf1[SIZE_SMALL_GENBUF];
		char msgbuf2[SIZE_SMALL_GENBUF];
		if (subscribed_cnt != 0)
			sprintf(msgbuf1,
				    "  subscribed to %d message board(s)\n",
					subscribed_cnt);
		else
			msgbuf1[0] = '\0';
		if (unsubscribed_cnt != 0)
			sprintf(msgbuf2,
					"  unsubscribed from %d message board(s)\n",
					unsubscribed_cnt);
		else
			msgbuf2[0] = '\0';
		
		CString msgstr = "Successfully...\n";
		msgstr += msgbuf1;
		msgstr += msgbuf2;

		AfxMessageBox(msgstr);
	}
	else if (!ok)
	{
		m_errman.RecordError("Error occurred in subscribing/unsubscribing");
		m_errman.HandleError(MSG_BOARD_FACILITY);
	}

 	delete [] subscribe_list;
	delete [] not_subscribe_list;
}



/////////////////////////////////////////////////////////////////////////////
// OpenMsg
// _______
//
// Called to open an existing message - new MDI child.

CMsgDoc *CMsgBoardMgr::OpenMsg(const char *msgboard_name,
							   const char *title,
							   const char *post_date,
							   const char *posted_by,
						       const char *msg_key)
{
	CMsgDoc *msg_doc = 
			(CMsgDoc *)
	    	(gbl_msgboard_man.m_recv_msg_doc_tmplt->OpenDocumentFile
 	    										 (OPEN_MSGDOC_CODE));
	if (msg_doc == NULL)
    		return(NULL);
      		
	if (msg_doc->ReadMsg(msgboard_name, title, post_date, 
					     posted_by, msg_key))
	{
	    return(msg_doc);
	}
	else
	{
		m_errman.HandleError(MSG_BOARD_FACILITY);

		msg_doc->OnCloseDocument();
    	return(NULL);
	}
    	
}




/////////////////////////////////////////////////////////////////////////////
// OpenMsgBoard
// ____________
//
// Called to open a message board - new MDI child.

void CMsgBoardMgr::OpenMsgBoard(EMsgBoardContentsType the_type,
								const char *msgboard_name)

{

                                       // Make sure have gotten basic
                                       //  msgboard list setup ok
	if (m_main_msgboard_list == NULL)
	{
		m_errman.RecordError("No message boards currently available");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return;
    }
    
    char lcl_msgboard_name[SIZE_MSGBOARD_NAME+1];
    
                                       // If msgboard_name is NULL then use
                                       //  CSingleSelectMsgBoardDlg to allow
                                       //  msgboard selection
    if (msgboard_name == NULL)
    {
	
		ASSERT (the_type == STANDARD_MSG_BOARD);

		CSingleSelectMsgBoardDlg open_msgboard_dlg(FALSE);
	
		if (open_msgboard_dlg.DoModal() == IDOK)
		{
                                       // Copy selection to lcl_msgboard_name
		    strncpy(lcl_msgboard_name,
		    		(LPCTSTR)open_msgboard_dlg.CSelectedMsgBoard(),
		    		SIZE_MSGBOARD_NAME);
		}
                                       // Else canceled, done
		else
			return;
	}
                                       // Else use given msgboard_name
	else
		strncpy(lcl_msgboard_name, msgboard_name, SIZE_MSGBOARD_NAME);
	    
                                       // Now setup to look to see if
                                       //  the msg board is already open
                                       //
	
	BOOL found = FALSE;

	POSITION pos = m_msgboard_doc_tmplt->GetFirstDocPosition();
	CMsgBoardDoc *doc_ptr;

	while ((!found) && (pos != NULL))
	{
		doc_ptr = (CMsgBoardDoc *)m_msgboard_doc_tmplt->GetNextDoc(pos);

		if ((the_type == OWNER_IN_MSG_BOARD) &&
			(doc_ptr->MsgBoardType() == OWNER_IN_MSG_BOARD))

			found = TRUE;

		else if ((the_type == STANDARD_MSG_BOARD) &&
				 (_stricmp((LPCSTR)(doc_ptr->GetTitle()), 
						   lcl_msgboard_name) == 0))
			found = TRUE;
	}
	                                   // If msg board is already open then 
                                       //  set focus to it and be done

	if (found)
	{
		((CMainFrame *)
		m_MsgBoardApp()->m_pMainWnd)->MDIActivate
					(doc_ptr->m_msgboard_view->GetParentFrame());
		return;
	}
                                       // Now OpenDocumentFile
	doc_ptr = (CMsgBoardDoc *)
				m_msgboard_doc_tmplt ->OpenDocumentFile
	  											(lcl_msgboard_name);
	if (doc_ptr == NULL)
	{
		m_errman.RecordError("Unable to create message board window");
		m_errman.HandleError(MSG_BOARD_FACILITY);
		return;
	}
                                 	   // If opened okay then read msg
                                 	   //  board data, is a standard
									   //  msg board unless lcl_msgboard_name
									   //  is same as UserName()


	BOOL ok = doc_ptr->ReadMsgBoard(the_type);

	if (!ok)
	{											   
		char msgbuf[SIZE_SMALL_GENBUF];
		sprintf(msgbuf,
			    "Error opening message board %s",
				lcl_msgboard_name);
		m_errman.RecordError(msgbuf);
		m_errman.HandleError(MSG_BOARD_FACILITY);

		doc_ptr->OnCloseDocument();

		return;
	}
}



/////////////////////////////////////////////////////////////////////////////
// PostMsgAgain
// ____________
//
// Called to post orig_msg again.  Creates a new MDI child for
// this.

void CMsgBoardMgr::PostMsgAgain(CMsgDoc *orig_msg)
{
	CMsgDoc *msg_doc = (CMsgDoc *)
	    				m_send_msg_doc_tmplt->OpenDocumentFile
	    												(REPOST_MSGDOC_CODE);
    if (msg_doc == NULL)
		return;
		  
    msg_doc->ReworkMsg(orig_msg, TRUE);

	orig_msg->OnCloseDocument();
}



/////////////////////////////////////////////////////////////////////////////
// PostMsgElseWhere
// ________________
//
// Called to post orig_msg elsewhere.  Creates a new MDI child for
// this.

void CMsgBoardMgr::PostMsgElseWhere(CMsgDoc *orig_msg)
{
	CMsgDoc *msg_doc = (CMsgDoc *)
	    				m_send_msg_doc_tmplt->OpenDocumentFile
	    												(REPOST_MSGDOC_CODE);
    if (msg_doc == NULL)
		return;
		  
    msg_doc->ReworkMsg(orig_msg, FALSE);

	orig_msg->OnCloseDocument();
}



/////////////////////////////////////////////////////////////////////////////
// PostNewMsg
// __________                               

void CMsgBoardMgr::PostNewMsg()
{
	CMsgDoc *msg_doc = (CMsgDoc *)
	    				m_send_msg_doc_tmplt->OpenDocumentFile
	    											(OPEN_MSGDOC_CODE);
      
	if (msg_doc == NULL)
    	return;
      		
    msg_doc->SetMsgType(CMsgDoc::TO_POST_MSG);

	((CMainFrame *)m_MsgBoardApp()->m_pMainWnd)->MDIMaximize
					(msg_doc->m_msg_view->GetParentFrame());
}

  


/////////////////////////////////////////////////////////////////////////////
// RefreshAllMsgBoards
// ___________________

void CMsgBoardMgr::RefreshAllMsgBoards()
{
	POSITION pos = m_msgboard_doc_tmplt->GetFirstDocPosition();
	CMsgBoardDoc *doc_ptr;

	while (pos != NULL)
	{
		doc_ptr = (CMsgBoardDoc *)m_msgboard_doc_tmplt->GetNextDoc(pos);

		doc_ptr->RefreshMsgBoard();
	}
}



/////////////////////////////////////////////////////////////////////////////
// ValidateMsgBoard
// ________________

BOOL CMsgBoardMgr::ValidateMsgBoard(const char *msgboard_name)
{
	Sbasic_msgbrd_rec *msgboard_ptr = m_main_msgboard_list;
	
    for (int i=0; i < m_msgboard_cnt; i++)
    {
    	if (strcmp(msgboard_ptr->msgboard_name,msgboard_name) == 0)
    		return(TRUE);
		    		
		msgboard_ptr++;
	}
	return(FALSE);
}



