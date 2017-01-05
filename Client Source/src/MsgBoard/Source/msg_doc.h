/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\msg_doc.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file for the CMsgDoc class.  There will be one
// CMsgDoc for each open message.
//

#ifndef _INCLUDE_MSG_DOC_H_
#define _INCLUDE_MSG_DOC_H_

#include <sizes.h>

class CMsgDoc : public CDocument
{
	friend class CMsgBoardMgr;
	
public:
	enum EMsgType {INITIAL_MSG, TO_POST_MSG, DONE_POSTED_MSG, 
				   RECEIVE_MSG};

// Construction
protected: // create from serialization only

	CMsgDoc();

	DECLARE_DYNCREATE(CMsgDoc)
public:
									   // m_msg_type is fundamental to
									   //  class operation, SetMsgType()
									   //  needs to be called right away,
									   //  hence putting under
									   //  'Construction' heading

	void SetMsgType(const EMsgType msg_type)
		{ m_msg_type = msg_type; };
	
// Attributes
public:
									   // This is for a linked list
									   //  of message boards to post
									   //  to, idx_num indexes into
									   //  gbl_msgboard_man's
									   //  MsgBoardList
	typedef struct SPostToMsgBoardEntry
	{
		char msgboard_name[SIZE_MSGBOARD_NAME+1];
		int  idx_num;
		SPostToMsgBoardEntry *next;
	}
	SPostToMsgBoardEntry;

protected:							   
									   // Depending on m_msg_type,
									   //  m_msg_tofrom_ptr may point
									   //  to a SPostToMsgBoardEntry or
									   //  to a character string, see
									   //  implemetation for details	

    VOID *m_msg_tofrom_ptr;			   // Other class member variables

	CFormView *m_msg_view;
	BOOL m_ok;

    SPostToMsgBoardEntry *m_current_msgboard_ptr;
	int  m_post_to_msgboard_cnt;

	EMsgType m_msg_type;
	
	char m_post_date[SIZE_MSG_POSTDATE+1];
	char m_title[SIZE_MSG_TITLE+1];
	char m_message_text[SIZE_MSG_TEXT+1];

public:
	const char *GetMessageText()
		{ return(m_message_text); };
		
	const char *GetPostDate()
		{ return(m_post_date); };
		
	const char *GetPostFromMsgBoard()
		{ 
			return ( (m_msg_type == RECEIVE_MSG) 
					  ?  (char *)m_msg_tofrom_ptr
					  : NULL); 
		};
		
	int  GetPostToMsgBoardListCnt()
		{ return(m_post_to_msgboard_cnt); };
	
	const char *GetPostToMsgBoardList()
		{ 
return ((char *)m_msg_tofrom_ptr); 
		};
		
	const char *GetPostToMsgBoardName(int idx)
		{ return(FindPostToMsgBoard(idx)->msgboard_name); };
		
	const char *GetTitle()
		{ return(m_title); };
		
	void SetMsgView(CFormView *msg_view)
		{ m_msg_view = msg_view; };

	void SetMessageText(const char *message_text)
		{ strcpy(m_message_text, message_text); };
	
	void SetMessageTitle(const char *message_title)
		{ strcpy(m_title, message_title); };
	
	BOOL SetPostFromMsgBoard(const char *msgboard_name);

	BOOL SetPostToMsgBoardList(const char *msgboard_list);

	BOOL SetPostToEntries(const char *msgboard_list);


	
// Operations

public:

	BOOL PostToMsgBoard(int msgboard_entry_num,
						BOOL first_posting_for_msg);

	BOOL ReadMsg(const char *msgboard_name,
				 const char *title,
				 const char *post_date,
				 const char *posted_by, 
				 const char *msg_key);

    void ReworkMsg(CMsgDoc *orig_msg, BOOL same_post_setting);


// Implementation - general
public:
	virtual ~CMsgDoc();

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(const char *path);

// Implementation - internal functions
protected:

	SPostToMsgBoardEntry *FindPostToMsgBoard(int msgboard_entry_num);

	void FixupMsgLines(const char *dest_text_buf,
					   const int max_dest_line_len,	
					   char *src_text_buf);

    void TrashToFromData();

// Implementation -  Generated message map functions
protected:
	//{{AFX_MSG(CMsgDoc)
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	afx_msg void OnPostMsgElsewhere();
	afx_msg void OnUpdatePostMsgElsewhere(CCmdUI* pCmdUI);
	afx_msg void OnMessagesPostmessageagain();
	afx_msg void OnUpdateMessagesPostmessageagain(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation - debug
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif // _INCLUDE_MSG_DOC_H_
