/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mb_mgr.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file for the CMsgBoardMgr class and
// related declarations.
//

#ifndef _INCLUDE_MB_MGR_H_
#define _INCLUDE_MB_MGR_H_

#include <err_man.h>
#include "msg_doc.h"
#include "mb_doc.h"
									   // Keeps SPARE_SLOT_CNT slots 
									   //  open for handling newly
									   //  create MsgBoards
#define SPARE_SLOT_CNT 5
									   // Used when doing updates to
									   //  tell views what is going on
									   // 
#define CHECK_UPDATES_HINT 	WM_USER + 5
#define DOING_REWORK_HINT 	WM_USER + 10
#define SHOW_MB_LIST_HINT 	WM_USER + 15

									   // Size of message titles
#define MSG_TITLE_SENDER_PART_LEN  19
#define MSG_TITLE_SUBJECT_PART_LEN 30
									   // Used in messages, etc.
#define MSG_BOARD_FACILITY "MsgBoardApp"

									   // For OpenDocumentFile indicates 
									   //  what the doc is being opened
									   //
#define OPEN_MSGDOC_CODE	"_OPEN_MSG_"
#define REPOST_MSGDOC_CODE	"_REPOST_MSG_"

enum EMsgBoardSubscribeType {SUBSCRIBED_MSGBOARD, SUBSCRIBABLE_MSGBOARD, 
			 				 NOT_SUBSCRIBABLE_MSGBOARD};

typedef struct Sbasic_msgboard_rec
{
    char					  msgboard_name[SIZE_MSGBOARD_NAME+1];
	int						  unread_msg_cnt;
    EMsgBoardSubscribeType    msgboard_subsc_type;

} Sbasic_msgbrd_rec;
 

class CMsgBoardMgr 
{
	friend class CMsgBoardApp;

// Construction
public:
	CMsgBoardMgr();
	~CMsgBoardMgr(); 

	BOOL Initialize();

// Attributes
protected:
	Sbasic_msgbrd_rec *m_main_msgboard_list;
    int m_msgboard_cnt;
    int	m_msgboard_spare_cnt;
    CString m_user_in_msg_board;
    CString m_user_name;
    CString m_server_name;
    
	CMultiDocTemplate *m_recv_msg_doc_tmplt;

	CMultiDocTemplate *m_send_msg_doc_tmplt;

	CMultiDocTemplate *m_msgboard_doc_tmplt;

	CImageList m_SmallImageList;

public:
	CFont *m_mb_font;

    CErrorManager      m_errman;

	CMsgBoardApp	  *m_MsgBoardApp()
		{ return ((CMsgBoardApp *)AfxGetApp()); };

	CImageList *ImageList()
		{ return(&m_SmallImageList); };

    const CString ServerName()
    	{ return(m_server_name); };

    const CString UserInMsgBoardName()
    	{ return(m_user_in_msg_board); };

	const CString UserName() 
    	{ return(m_user_name); };

// Operations
public:								
		   
	void CreateMsgBoard();

	void DeleteMsgBoard();

	const Sbasic_msgbrd_rec *MsgBoardList()
		{ return(m_main_msgboard_list); };

	int  MsgBoardListCnt()
		{ return(m_msgboard_cnt); };

	void MsgBoardSubscribe();

	CMsgDoc *OpenMsg(const char *msgboard_name, 
					 const char *title,
					 const char *post_date,
					 const char *posted_by,
					 const char *msg_key);

	void OpenMsgBoard(EMsgBoardContentsType the_type = STANDARD_MSG_BOARD,
					  const char *msgboard_name = NULL);
	
	void PostNewMsg();
	
	void PostMsgAgain(CMsgDoc *orig_msg);

	void PostMsgElseWhere(CMsgDoc *orig_msg);

	void RefreshAllMsgBoards();

	BOOL ValidateMsgBoard(const char *msgbrd_name);

// Implementation
protected:
	BOOL FindAndSetMsgType(Sbasic_msgbrd_rec *src_msgboard_list_ptr,
						   EMsgBoardSubscribeType new_msgboard_type);

	BOOL LoadMainMsgBoardList();

};
                                       // Here define/declare the one and 
                                       //   only CdllManager 
#ifdef DEFINE_THE_MSGBOARD_MAN

CMsgBoardMgr gbl_msgboard_man;

#else

extern CMsgBoardMgr gbl_msgboard_man;

#endif

#endif // _INCLUDE_MB_MGR_H_
