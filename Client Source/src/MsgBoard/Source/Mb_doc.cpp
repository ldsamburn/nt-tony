/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mb_doc.h                    
//
// WSP, last update: TBD
//                                        
// This is the implementation file for the CMsgBoardDoc class
//

#include "stdafx.h"
#include "msgboard.h"

#include "mb_mgr.h"
#include "msgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMsgBoardDoc constructor/destructor and message mapping
// _______________________________________________________

CMsgBoardDoc::CMsgBoardDoc()
{
	m_ok = TRUE;

	m_msg_list = NULL;
    m_msg_list_cnt = 0;
    m_msgboard_name[0] = '\0';

	m_msgboard_view = NULL;
}



CMsgBoardDoc::~CMsgBoardDoc()
{
	if (m_msg_list != NULL)
		delete [] m_msg_list;
}


IMPLEMENT_DYNCREATE(CMsgBoardDoc, CDocument)

BEGIN_MESSAGE_MAP(CMsgBoardDoc, CDocument)
	//{{AFX_MSG_MAP(CMsgBoardDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// OnNewDocument
// _____________

BOOL CMsgBoardDoc::OnNewDocument()
{
									   // Nothing to do yet
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// OnOpenDocument
// ______________

BOOL CMsgBoardDoc::OnOpenDocument(const char *path)
{
	strcpy(m_msgboard_name, path);
	
//	SetTitle(path);	

	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// ReadMsgBoard
// ____________
//
// At one point we were implementing owner 'out' MsgBoards and
// the msg_board_type was used to determine processing.  Now just
// hand off to ReadMsgBoardNormal for all calls.

BOOL CMsgBoardDoc::ReadMsgBoard(EMsgBoardContentsType msg_board_type)
{
	ASSERT (m_ok);
	m_msg_board_type = msg_board_type;

	return(ReadMsgBoardNormal());
}



/////////////////////////////////////////////////////////////////////////////
// ReadMsgBoardNormal
// __________________

BOOL CMsgBoardDoc::ReadMsgBoardNormal()
{
									   // GETBBMSGS - get list of
									   //  messages
	CmsgGETBBMSGS getbbmsgs_msg;

	BOOL ok = getbbmsgs_msg.Load(m_msgboard_name);

	if (ok)
		ok = getbbmsgs_msg.DoItNow();

	int getbbmsgs_list_cnt;

	if (ok)
		ok = getbbmsgs_msg.GetListCount(&getbbmsgs_list_cnt);

	Sbasic_msglist_rec *temp_msgboard_list_ptr = NULL;

									   // Now allocate space to store
									   //  the list of messages
									   //
	if ((ok) && (getbbmsgs_list_cnt > 0))
	{
		temp_msgboard_list_ptr = new Sbasic_msglist_rec[getbbmsgs_list_cnt];

		if (temp_msgboard_list_ptr == NULL)
		{
			gbl_msgboard_man.m_errman.RecordError
								("Memory allocation error");
			m_ok = FALSE;
			return(FALSE);
		}
	}
									   // Now actually unload the
									   //  data to the allocated space
									   //
	if ((ok) && (getbbmsgs_list_cnt > 0))
	{
		ok = getbbmsgs_msg.UnLoad(getbbmsgs_list_cnt, 
								  &getbbmsgs_list_cnt,
								  temp_msgboard_list_ptr);

	}
									   // Here set class members to
									   //  track the message list
									   //
	if ((ok) && (getbbmsgs_list_cnt > 0))
	{
		m_msg_list_cnt = getbbmsgs_list_cnt;
		m_msg_list = temp_msgboard_list_ptr;	
	}
									   // Here is ok but no messages
	else if (ok)
	{
		m_msg_list_cnt = 0;
	}								   
									   // Otherwise error, free space
									   //  if allocated
	else
	{
		if (temp_msgboard_list_ptr != NULL)
			delete [] temp_msgboard_list_ptr;

		gbl_msgboard_man.m_errman.RecordError
							("Message Board read error(GETBBMSGS)");
		m_ok = FALSE;
		return(FALSE);
	}
									   // Now let view take care of
									   //  displaying the list
	UpdateAllViews(NULL, SHOW_MB_LIST_HINT);

	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// ReadMsgBoardOwnerPosts
// ______________________
//
// No longer supporting MsgBoards containing owner posted messages
// (out boxes).

//BOOL CMsgBoardDoc::ReadMsgBoardOwnerPosts()
//{
//									   // GETBBMSGS - get list of
//									   //  messages
//	CmsgGETOWNBBMSGS getownbbmsgs_msg;
//		
//	BOOL ok = getownbbmsgs_msg.Load();
//
//	if (ok)
//		ok = getownbbmsgs_msg.DoItNow();
//
//	int getownbbmsgs_list_cnt;
//
//	if (ok)
//		ok = getownbbmsgs_msg.GetListCount(&getownbbmsgs_list_cnt);
//
//	Sbasic_msglist_rec *temp_msgboard_list_ptr = NULL;
//
//									   // Now allocate space to store
//									   //  the list of messages
//									   //
//	if ((ok) && (getownbbmsgs_list_cnt > 0))
//	{
//		temp_msgboard_list_ptr = new Sbasic_msglist_rec[getownbbmsgs_list_cnt];
//
//		if (temp_msgboard_list_ptr == NULL)
//		{
//			gbl_msgboard_man.m_errman.RecordError
//								("Memory allocation error");
//			m_ok = FALSE;
//			return(FALSE);
//		}
//	}
//									   // Now actually unload the
//									   //  data to the allocated space
//									   //
//	if ((ok) && (getownbbmsgs_list_cnt > 0))
//	{
//		ok = getownbbmsgs_msg.UnLoad(getownbbmsgs_list_cnt, 
//								     &getownbbmsgs_list_cnt,
//								     temp_msgboard_list_ptr);
//
//	}
//									   // Here set class members to
//									   //  track the message list
//									   //
//	if ((ok) && (getownbbmsgs_list_cnt > 0))
//	{
//		m_msg_list_cnt = getownbbmsgs_list_cnt;
//		m_msg_list = temp_msgboard_list_ptr;	
//	}
//									   // Here is ok but no messages
//	else if (ok)
//	{
//		m_msg_list_cnt = 0;
//	}								   
//									   // Otherwise error, free space
//									   //  if allocated
//	else
//	{
//		if (temp_msgboard_list_ptr != NULL)
//			delete [] temp_msgboard_list_ptr;
//
//		gbl_msgboard_man.m_errman.RecordError
//							("Message Board read error(GETOWNBBMSGS)");
//		m_ok = FALSE;
//		return(FALSE);
//	}
//									   // Now let view take care of
//									   //  displaying the list
//	UpdateAllViews(NULL, SHOW_MB_LIST_HINT);
//
//	return(TRUE);
//}



/////////////////////////////////////////////////////////////////////////////
// ReadMsgBoard
// ____________
//
// No longer supporting ReadMsgBoardOwnerPosts, so just pass thru
// to  ReadMsgBoardNormal

BOOL CMsgBoardDoc::RefreshMsgBoard()
{
	ASSERT (m_ok);

	if (m_msg_list != NULL)
	{
		delete [] m_msg_list;
		m_msg_list = NULL;
	}

	return(ReadMsgBoardNormal());
}



/////////////////////////////////////////////////////////////////////////////
// CMsgBoardDoc diagnostics
// ________________________

#ifdef _DEBUG
void CMsgBoardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMsgBoardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


