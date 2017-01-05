/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mb_doc.cpp                   
//
// WSP, last update: TBD
//                                        
// This is the header file for the CMsgBoardDoc class and
// related declarations.  There is one CMsgBoardDoc object created for
// each open MsgBoard.
//


#ifndef _INCLUDE_MB_DOC_H_
#define _INCLUDE_MB_DOC_H_

#include <sizes.h>

									   // This structure contains the
									   //  info for listing details
									   //  for a single message
typedef struct Sbasic_msglist_rec
{
    char    key[SIZE_MSGBOARD_KEY+1];
	char	post_date[SIZE_MSG_POSTDATE+1];
   	char    msgboard_name[SIZE_MSGBOARD_NAME+1];
    char    title[SIZE_MSG_TITLE+1];
   	BOOL    readsent_flag;

} Sbasic_msglist_rec;

								       // OWNER_IN_MSG_BOARD's are set
									   //  up one for each user (in-box),
									   //  all others are STANDARD_MSG_BOARDs
									   //
typedef enum {OWNER_IN_MSG_BOARD, STANDARD_MSG_BOARD}
			  EMsgBoardContentsType;

class CMsgBoardView;

									   // CMsgBoardDoc starts here
class CMsgBoardDoc : public CDocument
{
	friend class CMsgBoardMgr;
	
public:

// Construction

protected: // create from serialization only

	CMsgBoardDoc();

	DECLARE_DYNCREATE(CMsgBoardDoc)


// Attributes
public:
				   
	EMsgBoardContentsType MsgBoardType()
		{ return (m_msg_board_type); };

	CMsgBoardView *MsgBoardView()
		{ return (m_msgboard_view); };

	void SetMsgBoardView(CMsgBoardView *mb_view)
		{ m_msgboard_view = mb_view; };

// Operations
public:
	const char *MsgBoardName()
		{ return(m_msgboard_name); };

	const Sbasic_msglist_rec *MsgList()
		{ return(m_msg_list); };

	int  MsgListCnt()
		{ return(m_msg_list_cnt); };
		
	BOOL ReadMsgBoard(EMsgBoardContentsType msg_board_type);

	BOOL RefreshMsgBoard();


// Implementation

public:
	virtual ~CMsgBoardDoc();

protected:
	BOOL m_ok;
	char m_msgboard_name[SIZE_MSGBOARD_NAME+1];

	int m_msg_list_cnt;
	Sbasic_msglist_rec *m_msg_list;

    EMsgBoardContentsType m_msg_board_type;

	CMsgBoardView *m_msgboard_view;

	BOOL ReadMsgBoardNormal();			   
	BOOL ReadMsgBoardOwnerPosts();			   

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	virtual BOOL OnNewDocument();

	virtual BOOL OnOpenDocument(const char *path);

// Generated message map functions
//
protected:
	//{{AFX_MSG(CMsgBoardDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _INCLUDE_MB_DOC_H_
