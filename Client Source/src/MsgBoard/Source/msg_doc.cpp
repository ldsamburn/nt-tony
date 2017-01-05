/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\msg_doc.cpp                    
//
// WSP, last update: TBD
//                                        
// This is the implementation file for the CMsgDoc class.
//

#include "stdafx.h"

#include <ctype.h>
#include <sizes.h>
#include <inri_ftp.h>

#include "msgboard.h"

#include "msg_doc.h"
#include "mb_mgr.h"
#include "rmsg_vw.h"

#include "msgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMsgDoc constructor
// ___________________

CMsgDoc::CMsgDoc()
{
	m_msg_type = INITIAL_MSG;

    m_msg_tofrom_ptr = NULL;
    m_current_msgboard_ptr = NULL;
    
	m_post_to_msgboard_cnt = 0;
	
	m_ok = TRUE;
	
	m_title[0] = '\0';
	m_message_text[0] = '\0';
	m_post_date[0] = '\0';
}



/////////////////////////////////////////////////////////////////////////////
// CMsgDoc destructor
// ______________________
//

CMsgDoc::~CMsgDoc()
{
	TrashToFromData();
}



/////////////////////////////////////////////////////////////////////////////
// CMsgDoc dynamic creation and message mapping
// ________________________________________________

IMPLEMENT_DYNCREATE(CMsgDoc, CDocument)

BEGIN_MESSAGE_MAP(CMsgDoc, CDocument)
	//{{AFX_MSG_MAP(CMsgDoc)
	ON_COMMAND(ID_POST_MESSAGE_ELSEWHERE, OnPostMsgElsewhere)
	ON_UPDATE_COMMAND_UI(ID_POST_MESSAGE_ELSEWHERE, OnUpdatePostMsgElsewhere)
	ON_COMMAND(ID_MESSAGES_POSTMESSAGEAGAIN, OnMessagesPostmessageagain)
	ON_UPDATE_COMMAND_UI(ID_MESSAGES_POSTMESSAGEAGAIN, OnUpdateMessagesPostmessageagain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CanCloseFrame
// _____________
//
// For messages that are being posted, this lets the view have a 
// chance at setting the modified flag and then if the message has in 
// fact changed but not been posted yet a message will be displayed 
// and user given a chance to back out of the close.

BOOL CMsgDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	ASSERT(m_ok);

	if (m_msg_type == TO_POST_MSG)
	{
		UpdateAllViews(NULL, CHECK_UPDATES_HINT, NULL);

		if (IsModified())
		{
			if (AfxMessageBox("An open message has yet to be posted, quit anyway? ",
					      MB_YESNO) == IDYES)
				return(TRUE);
			else
				return(FALSE);
		}
	}
   	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// FindPostToMsgBoard
// __________________
//
// Locates the msgboard_entry_num'th entry in the SPostToMsgBoardEntry list.
// Uses and sets m_current_msgboard_ptr to try to reduce the search time. 

CMsgDoc::SPostToMsgBoardEntry *CMsgDoc::FindPostToMsgBoard
										(int msgboard_entry_num)
{
	ASSERT(m_ok);
	ASSERT(m_msg_type == TO_POST_MSG);

                                       // If current_entry ptr isn't set
                                       //  or if it is set somewhere in
                                       //  the list past the index that
                                       //  we need, then start at front
                                       //
	if ( (m_current_msgboard_ptr == NULL) ||
		 ((m_current_msgboard_ptr != NULL) &&
		 (msgboard_entry_num < m_current_msgboard_ptr->idx_num) ))
	{
		m_current_msgboard_ptr = (SPostToMsgBoardEntry *)m_msg_tofrom_ptr;
	}
		
	while ( (m_current_msgboard_ptr != NULL) &&
			(m_current_msgboard_ptr->idx_num < msgboard_entry_num) )
	{
		m_current_msgboard_ptr = m_current_msgboard_ptr->next;
	}
                      				   // If have found the right
                      				   //  entry then ok, return ptr,
                      				   //  otherwise NULL
                      				   //
    if ((m_current_msgboard_ptr != NULL) &&
    	(m_current_msgboard_ptr->idx_num == msgboard_entry_num))
    
    	return(m_current_msgboard_ptr);
    else
    	return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// FixupMsgLines
// _____________
//
// Copies text in src_text_buf to dest_text_buf, for any line that is
// longer than max_dest_line_len.

void CMsgDoc::FixupMsgLines(const char *src_text_buf, 
			   			    const int max_dest_line_len,
							char *dest_text_buf)
{
//char buf[256];
	if ((src_text_buf == NULL) || (src_text_buf == NULL) ||
		(max_dest_line_len <= 0))
		return;

	int src_idx = 0;
	int dest_idx = 0;
	int src_last_space_idx = -1;
	int dest_last_space_idx = -1;
	int crnt_line_len = 0;
	int i;
	
									   // Loop until hit ending NULL
									   //
	while (*(src_text_buf + src_idx) != NULL)
	{
									   // If this char is '/n' then
									   //  need to end line
									   //
		if (*(src_text_buf + src_idx) == '\n')
		{
			crnt_line_len = 0;
			src_last_space_idx = -1;
			dest_last_space_idx = -1;
			src_idx++;
									   // These chars needed to end line
									   //  in list box
									   //
			*(dest_text_buf + dest_idx) = '\r';
			dest_idx++;
			*(dest_text_buf + dest_idx) = '\r';
			dest_idx++;
			*(dest_text_buf + dest_idx) = '\n';
			dest_idx++;
		}
									   // Else if still less than a full
									   //  line then just bump up cnt
									   //  and copy over,
									   //  also if on a space then save
									   //  its position
									   //
		else if (crnt_line_len < max_dest_line_len)
		{
			if (isspace(*(src_text_buf + src_idx)))
			{
				src_last_space_idx = src_idx;
				dest_last_space_idx = dest_idx;
			}
			*(dest_text_buf + dest_idx) = 
								*(src_text_buf + src_idx);

			src_idx++;
			dest_idx++;
			crnt_line_len++;
		}
									   // Else have gone past the
									   //  max_dest_line_len, have to deal with
		else
		{
//AfxMessageBox("Hit line too long");
									   // If had a previous space on the
									   //  current line then end line
									   //  there
			if (src_last_space_idx != -1)
			{
//AfxMessageBox("Had a previous space");
//sprintf(buf,"dest_idx was %d, setting to %d",dest_idx,dest_last_space_idx );
//AfxMessageBox(buf);
									   // Go back to dest_last_space_idx
									   //  and end line for list box
									   //
				dest_idx = dest_last_space_idx;

				*(dest_text_buf + dest_idx) = '\r';
				dest_idx++;
				*(dest_text_buf + dest_idx) = '\r';
				dest_idx++;
				*(dest_text_buf + dest_idx) = '\n';
				dest_idx++;
//*(dest_text_buf + dest_idx) = '\0';
//sprintf(buf,"Just terminated dest line is: <%s>",dest_text_buf);
//AfxMessageBox(buf);
//sprintf(buf,"Now src_idx is %d and src_last_space_idx is %d",
//src_idx, src_last_space_idx);
//AfxMessageBox(buf);
										// Now have to copy over chars
										//  from src_last_space_idx to
										//  src_idx
										//
				for (i = 1; i < (src_idx - src_last_space_idx); i++)
				{
					*(dest_text_buf + dest_idx) =
						*(src_text_buf + src_last_space_idx + i);

					dest_idx++;
				}
//*(dest_text_buf + dest_idx) = '\0';
//sprintf(buf,"After pulling over rest of stuff: <%s>", dest_text_buf);
//AfxMessageBox(buf);

				crnt_line_len = 0;
			}
									   // Else if did not have a previous
									   //  space then too bad, just have
									   //  to look for next space, will
									   //  end up with line longer than
									   //  max_dest_line_len 
			else
			{
//AfxMessageBox("Did not have a previous space");
//*(dest_text_buf + dest_idx) = '\0';
//sprintf(buf,"Now looks like stuff: <%s>", dest_text_buf);
//AfxMessageBox(buf);
				while ((*(src_text_buf + src_idx) != NULL) &&
					   (!isspace(*(src_text_buf + src_idx))))
				{
					*(dest_text_buf + dest_idx) =
						*(src_text_buf + src_idx);

					src_idx++;
					crnt_line_len++;
					dest_idx++;
				}
//*(dest_text_buf + dest_idx) = '\0';
//sprintf(buf,"After the loop: <%s>", dest_text_buf);
//AfxMessageBox(buf);
				if (isspace(*(src_text_buf + src_idx)))
				{
					*(dest_text_buf + src_last_space_idx) = '\r';
					dest_idx++;
					*(dest_text_buf + src_last_space_idx+1) = '\r';
					dest_idx++;
					*(dest_text_buf + src_last_space_idx+2) = '\n';
					dest_idx++;
					crnt_line_len = 0;
				}
									   // Else have hit end of buffer,
									   //  just continue to quit
				else
					continue;
			}

									   // Signifies no space yet found
									   //  on current line
			src_last_space_idx = -1;
			dest_last_space_idx = -1;
		}
	}
	*(dest_text_buf + dest_idx)=NULL; //jj buffer overrun problem
}



/////////////////////////////////////////////////////////////////////////////
// OnNewDocument
// _____________

BOOL CMsgDoc::OnNewDocument()
{
								       // Nothing to do
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// OnOpenDocument
// ______________
//
// Makes sure that is a valid call by requiring path to point to a
// string containing OPEN_MSGDOC_CODE or REPORT_MSGDOC_CODE.
//

BOOL CMsgDoc::OnOpenDocument(const char *path)
{
	if ((strcmp(path,OPEN_MSGDOC_CODE) == 0) ||
	    (strcmp(path,REPOST_MSGDOC_CODE) == 0))

		return(TRUE);

	else
		return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// OnPostMsgElsewhere
// __________________

void CMsgDoc::OnMessagesPostmessageagain()
{
	ASSERT(m_ok);

	gbl_msgboard_man.PostMsgAgain(this);
}



/////////////////////////////////////////////////////////////////////////////
// OnPostMsgElsewhere
// __________________

void CMsgDoc::OnPostMsgElsewhere()
{
	ASSERT(m_ok);

	gbl_msgboard_man.PostMsgElseWhere(this);
}



/////////////////////////////////////////////////////////////////////////////
// OnUpdatePostMsgElsewhere
// ________________________
//
// Only RECEIVE_MSG's can be posted elsewhere

void CMsgDoc::OnUpdatePostMsgElsewhere(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_msg_type == RECEIVE_MSG);
}



/////////////////////////////////////////////////////////////////////////////
// OnUpdatePostMsgElsewhere
// ________________________
//
// Only RECEIVE_MSG's can be posted again

void CMsgDoc::OnUpdateMessagesPostmessageagain(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_msg_type == RECEIVE_MSG);
}



/////////////////////////////////////////////////////////////////////////////
// PostToMsgBoard
// ______________
//
// This function posts the current CMsgDoc to the indicated message
// board.  This is accomplished in five steps - first a
// GETNEWMSGFILE is done to tell the server about the new message
// and to get a server file name that is to receive the message text.
// Next a local file is created and the message text is written
// to the file.  After that CInriFtpMan is used to copy the file
// to the server. Then a CREATEMSGFILE is done to tell the server
// about the transferred file.  Finally a SENDBBMSG is done to actually
// post the message to the given board.
//
// Note for messages going to multiple recipients, only the last
// step, SENDBBMSG, needs to be done for the second and subsequents
// postings.  Static variables message_id and file_already_sent are
// used to control this.  Parm first_posting_for_msg indicates when
// a brand new message is being sent.  As soon as the first four
// steps are completed file_already_sent is set to flag that these
// steps don't have to be repeated if called again and
// first_posting_for_msg is FALSE.
//
// Also Note that parm msgboard_entry_num is used with function 
// FindPostToMsgBoard to find the particular message board to which
// the message is being sent.

#define	MSG_ID_START_POS	17
#define MSG_ID_LEN			13

BOOL CMsgDoc::PostToMsgBoard(int msgboard_entry_num,
							 BOOL first_posting_for_msg)
{
	ASSERT(m_ok);
	ASSERT(m_msg_type == TO_POST_MSG);

	char server_file[SIZE_SERVER_LOCATION];

									   // These retain value between
									   //  calls so can post a msg to
									   //  many boards with only doing
									   //  a single FTP
									   //
	static char message_id[SIZE_MSGBOARD_KEY];
	static BOOL file_already_sent;
									   
	if (first_posting_for_msg)
	{
		file_already_sent = FALSE;
	}
									   // Validate
	ASSERT (m_msg_tofrom_ptr != NULL);
									   // Find message board to post to
									   //
	SPostToMsgBoardEntry *current_entry_ptr =  
						FindPostToMsgBoard(msgboard_entry_num);
						
	ASSERT (current_entry_ptr == NULL);

	BOOL ok = TRUE;					   
									   // If haven't sent file yet...
	if (!file_already_sent)
	{
									   // Get name of server location
									   //  to receive msg file
		CmsgGETNEWMSGFILE getnewmsgfile_msg;

		ok = getnewmsgfile_msg.Load();

		if (ok)
			ok = getnewmsgfile_msg.DoItNow();

		if (ok)
			ok = getnewmsgfile_msg.UnLoad(server_file);

		if (!ok)
		{
			gbl_msgboard_man.m_errman.RecordError
					("Message posting error (GETNEWMSGFILE)");
			return(FALSE);
		}

	//	strcpy(message_id, &server_file[strlen(server_file)-MSG_ID_START_POS]);
	//	message_id[MSG_ID_LEN] = '\0';

	//	strcat(message_id, gbl_msgboard_man.UserName());

    //jgj fix for 8 char user id 6/14/97
        strcpy(message_id, &server_file[strlen(server_file)-MSG_ID_START_POS]);

		strncpy(&message_id[MSG_ID_LEN], (LPCTSTR)gbl_msgboard_man.UserName(), SIZE_LOGIN_NAME);
        message_id[SIZE_MSGBOARD_KEY - 1] = '\0';


									   // Open temp local disk file
		FILE *fp = NULL;
		char lcl_file_name[SIZE_TINY_GENBUF];

		getTempFileName(lcl_file_name, SIZE_TINY_GENBUF);

	
		ok = ((fp = fopen(lcl_file_name, "w")) != NULL);

									   // Write msg to local disk file
		if (ok)
		{
			UINT len = strlen(m_message_text);
			ok = (fwrite(m_message_text, 1, len, fp) == len);
			fclose(fp);
		}

		if (!ok)
		{
			gbl_msgboard_man.m_errman.RecordError
					("Message posting error (file create)");
			return(FALSE);
		}
									   // Now FTP the file to the
									   //  server
		CInriFtpMan ftp_man;

		ok = ftp_man.Initialize();

		if (ok)
			ok = ftp_man.PutFile(gbl_msgboard_man.ServerName(),
								 server_file,
								 lcl_file_name,
								 TRUE,
								 0666);  // WSPTODO - better protection?

		if (!ok)
		{
			gbl_msgboard_man.m_errman.RecordError
					("Message posting error (FTP)");
			return(FALSE);
		}
									   // Now do CREATEMSGFILE to tell
									   //  the server about the file
									   //
		_unlink(lcl_file_name);        //TR ##### 9-6-97

		CmsgCREATEMSGFILE createmsgfile_msg;

		ok = createmsgfile_msg.Load(message_id, m_title);

		if (ok)
			ok = createmsgfile_msg.DoItNow();

									   // If okay then won't have to
									   //  do file send again if have
									   //  multiple recipitents
		if (ok)
			file_already_sent = TRUE;
		else
		{
			gbl_msgboard_man.m_errman.RecordError
					("Message posting error (CREATEMSGFILE)");
			return(FALSE);
		}

// WSPTODO - need to set error message here	if !ok

	} // endif for (!file_already_sent)

									   // Whether file_already_sent
									   //  or not need to do a
									   //  SENDBBMSG
	CmsgSENDBBMSG sendbbmsg_msg;

	ok = sendbbmsg_msg.Load(message_id,
								current_entry_ptr->msgboard_name);

		ok = sendbbmsg_msg.DoItNow();

	if (ok)
		return(TRUE);

	else
	{
		gbl_msgboard_man.m_errman.RecordError
				("Message posting error (SENDBBMSG)");
		return(FALSE);
	}
}



/////////////////////////////////////////////////////////////////////////////
// ReadMsg
// _______
//
// Reads a message from the server into this CMsgDoc.  
// Three major steps are involved - first CmsgGETMSGFILE 
// is used to tell the server that a file is to be read 
// (using msg_key to indicate the particular msg). CmsgGETMSGFILE
// gets the name of a server file containing the message 
// text and	CInriFtpMan is used to pull the file down from
// the server.  Finally the file is fopen'd locally and read.  Note
// that the other data for the message (title, post_data, message
// board name, etc.), is taken from the parameters to this function.

#define MAX_MSG_LINE_LEN 72

BOOL CMsgDoc::ReadMsg(const char *msgboard_name,
					  const char *title,
 					  const char *post_date,
					  const char *posted_by, 
					  const char *msg_key)
{
	ASSERT(m_ok);

	m_msg_type = RECEIVE_MSG;
									   // Do GETMSGFILE msg
	CmsgGETMSGFILE msgfile_msg;

	BOOL ok = msgfile_msg.Load(msg_key);

	if (ok)
		ok = msgfile_msg.DoItNow();

	char svr_file_name[SIZE_SERVER_LOCATION];

	if (ok)
		ok = msgfile_msg.UnLoad(svr_file_name);

	if (!ok)
	{
		m_ok = FALSE;
		gbl_msgboard_man.m_errman.RecordError
				("Message reading error (GETMSGFILE)");
		return(FALSE);
	}
									   // Now FTP the file over
									   //
	char lcl_file_name[SIZE_TINY_GENBUF];

	getTempFileName(lcl_file_name, SIZE_TINY_GENBUF);

	
	CInriFtpMan ftp_man;

	ok = ftp_man.Initialize();

	if (ok)
		ok = ftp_man.GetFile(gbl_msgboard_man.ServerName(),
							 svr_file_name,
							 lcl_file_name,
							 TRUE,
							 0666);  // WSPTODO - better protection

	if (!ok)
	{
		m_ok = FALSE;
		gbl_msgboard_man.m_errman.RecordError
				("Message reading error (FTP)");
		return(FALSE);
	}
									   // Read the msg file now
	FILE *in_file;

	if ((in_file = fopen(lcl_file_name, "r")) == NULL)
	{
			ok = FALSE;
	}

	char temp_message_text[SIZE_MSG_TEXT+1];

	if (ok)
	{
		int total_len = 0;
		int num_read;

		do
		{
			if (fgets(&(temp_message_text[total_len]), 
					  (SIZE_MSG_TEXT - total_len - 3), 
					  in_file) != NULL)
			{
				num_read = strlen(&(temp_message_text[total_len]));
				total_len += num_read;
			}
			else
				num_read = 0;

		} while (num_read != 0);

		temp_message_text[total_len] = '\0';

		fclose(in_file);
	}

	if (!ok)
	{
		m_ok = FALSE;
		gbl_msgboard_man.m_errman.RecordError
				("Message reading error (file read)");
		return(FALSE);
	}
									   // Fixup lines that are too
									   //  long
									   //
   

	FixupMsgLines(temp_message_text, MAX_MSG_LINE_LEN,  
				  m_message_text);

	strcpy(m_title, title);
	strcpy(m_post_date, post_date);

	SetPostFromMsgBoard(posted_by);

	UpdateAllViews(NULL);

	((CRecvMsgView *)m_msg_view)->SetupTitle();

	_unlink(lcl_file_name);        //TR ##### 9-6-97

									   // Mark msg as have been read
	CmsgMARKMSGREAD	markmsgread_msg;

	markmsgread_msg.Load(msg_key);

	markmsgread_msg.DoItNow();

	return(TRUE);
}                             


	

/////////////////////////////////////////////////////////////////////////////
// ReworkMsg
// _________
//
// Called when posting the current CMsgDoc to a different message
// board.  Data for this CMsgDoc is taken from the given orig_msg.

void CMsgDoc::ReworkMsg(CMsgDoc *orig_msg, BOOL same_post_setting)
{
	ASSERT(m_ok);

	m_msg_type = TO_POST_MSG;
	
	strcpy(m_title, orig_msg->m_title);
	strcpy(m_message_text, orig_msg->m_message_text);
    
	if (same_post_setting)
		SetPostToEntries((char *)orig_msg->m_msg_tofrom_ptr);
	else
		SetPostToEntries("");

    int idx = 0;
    
	char title_buf[SIZE_SMALL_GENBUF];
    
    strcpy(title_buf, "Other Message Board");
    idx = strlen(title_buf);
    	
    title_buf[idx++] = '/';
    title_buf[idx] = '\0';
    
    int len = strlen(m_title);
    
    if (len > MSG_TITLE_SUBJECT_PART_LEN)
    	len = MSG_TITLE_SUBJECT_PART_LEN;

    strncat(title_buf, m_title, len);
    
	UpdateAllViews(NULL, DOING_REWORK_HINT, NULL);
}



/////////////////////////////////////////////////////////////////////////////
// SetPostFromMsgBoard
// ___________________
//
// Allocates space for msgboard_name and points m_msg_tofrom_ptr to it.

BOOL CMsgDoc::SetPostFromMsgBoard(const char *msgboard_name)
{
	ASSERT (m_ok);
	ASSERT (m_msg_type == RECEIVE_MSG);
	ASSERT (msgboard_name != NULL);

	int len = strlen(msgboard_name);

	char *ptr = new char[len+1];
	if (ptr == NULL)
	{
		gbl_msgboard_man.m_errman.RecordError
				("Memory allocation error");
		return(FALSE);
	}

	if (ptr == NULL)
	{
		m_ok = FALSE;
		return(FALSE); // WSPTODO - set err msg here?
	}

	strcpy(ptr, msgboard_name);

	m_msg_tofrom_ptr = (VOID *) ptr;

	return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// SetPostToEntries
// ________________
//
// Expects raw_entry_str to point to a list of comma or space separated
// message board names.  If successful TRUE is returned and 
// m_msg_tofrom_ptr will point to a list of SPostToMsgBoardEntry's
// containing valid message board names.  Also m_post_to_msgboard_cnt
// will be set to the number of message boards.  If an error occurs then
// FALSE is returned and the CString pointed to by err_msg will contain
// reasons for the error.

BOOL CMsgDoc::SetPostToEntries(const char *raw_entry_str)
{
	ASSERT (m_ok);
	ASSERT (m_msg_type == TO_POST_MSG);
	ASSERT (raw_entry_str != NULL);
	
	const char *ch_ptr = raw_entry_str;
	const char *word_ptr = raw_entry_str;
	int len;

	SPostToMsgBoardEntry *current_entry_ptr;
	SPostToMsgBoardEntry *prev_entry_ptr = NULL;
	
    m_post_to_msgboard_cnt = 0;
	BOOL done = FALSE;
    BOOL ok = TRUE;
    
                                       // Loop until all of raw_entry_str 
                                       //  has been processed
	while (!done)
	{
                                       // Skip white space
                                       //
		while ((*ch_ptr != '\0') && (isspace(*ch_ptr)))
			ch_ptr++;
			
                                       // If on NULL then we're done
		if (*ch_ptr == '\0')
		{
			done = TRUE;
			continue;
		}
		
		word_ptr = ch_ptr;
                                       // Now skip until hit a blank, comma
                                       //  or a NULL
                                       //
		while ((*ch_ptr != '\0') && (!isspace(*ch_ptr))
								 && (*ch_ptr != ','))
			ch_ptr++;
			
                                       // If hit NULL will be done but still
                                       //  process this one
		if (*ch_ptr == '\0')
			done = TRUE;
                                       // Compute length
        len = ch_ptr - word_ptr;
                                       // If this one is longer than allowed
                                       //  then have to cut if off!
        if (len > SIZE_MSGBOARD_NAME)
        	len = SIZE_MSGBOARD_NAME;
                                       // Now setup to add this one to
                                       //  the list
                                       //
		current_entry_ptr = new SPostToMsgBoardEntry;
		
                                       // If can't allocate then bail
		if (current_entry_ptr == NULL)
		{
			gbl_msgboard_man.m_errman.RecordError
								("Memory allocation error");
			m_ok = FALSE;
			return(FALSE);
		}
		                               // Copy to msgboard_name and NULL
		                               //  terminate
		                               //
        strncpy(current_entry_ptr->msgboard_name,
        	    word_ptr, len);
        *(current_entry_ptr->msgboard_name+len) = '\0';


                                       // Setup ptrs and also indexes
                                       //  for the SPostToMsgBoardEntry list
		current_entry_ptr->next = NULL;
		
		if (prev_entry_ptr == NULL)
		{
			m_msg_tofrom_ptr = (VOID *)current_entry_ptr;
			current_entry_ptr->idx_num = 0;
		}
		else
		{
			prev_entry_ptr->next = current_entry_ptr;
			current_entry_ptr->idx_num = (prev_entry_ptr->idx_num+1);
		}
		prev_entry_ptr = current_entry_ptr;

        	                           // Bump up counter
        	                           //
        m_post_to_msgboard_cnt++;

                                       // If not done setup for next word
		if (!done)
        	ch_ptr++;
	}
    
    CString err_msg = "Invalid message board(s) specified:\n";

    if (m_post_to_msgboard_cnt == 0)
	{    
    	err_msg += raw_entry_str;
    	ok = FALSE;
    }
    else
    {
    	current_entry_ptr = (SPostToMsgBoardEntry *)m_msg_tofrom_ptr;

    	while (current_entry_ptr != NULL)
    	{
			if (!gbl_msgboard_man.ValidateMsgBoard
								(current_entry_ptr->msgboard_name))
			{
				err_msg += current_entry_ptr->msgboard_name;

				ok = FALSE;
			}
			current_entry_ptr = current_entry_ptr->next;
		}
    }
    
	if (!ok)
    {
		gbl_msgboard_man.m_errman.RecordError((LPCTSTR)err_msg);
		return(FALSE);    
	}
    
	return(ok);
}
					   



/////////////////////////////////////////////////////////////////////////////
// TrashToFromData
// ________________
//
// If m_msg_tofrom_ptr is not null then have to delete allocated memory.
// The type of allocation depends on the setting of m_msg_type.
// If is TO_POST_MSG then have to delete a linked list of 
// SPostToMsgBoardEntry's, if FALSE then is just a simple character 
// string that must be deleted.
//

void CMsgDoc::TrashToFromData()
{
	if (!m_ok)
		return;

	if (m_msg_tofrom_ptr != NULL)
	{
		if ((m_msg_type == TO_POST_MSG))
		{
			SPostToMsgBoardEntry *current_ptr =
								(SPostToMsgBoardEntry *)m_msg_tofrom_ptr;
								
			SPostToMsgBoardEntry *next_ptr;
			
			while (current_ptr != NULL)
			{
				next_ptr =  current_ptr->next;
				delete [] current_ptr;
				current_ptr = next_ptr;
			}
		}
		else
			delete [] m_msg_tofrom_ptr;
	}

	m_post_to_msgboard_cnt = 0;
	m_current_msgboard_ptr = NULL;
	m_msg_tofrom_ptr = NULL;
}

 

/////////////////////////////////////////////////////////////////////////////
// diagnostics
// ___________

#ifdef _DEBUG
void CMsgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMsgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG




