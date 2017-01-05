/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\msgs.cpp                    
//
// WSP, last update: TBD
//                                        
// This is the implmentation file containing class declarations for all
// CNtcssMEM_MSG_ALLOC_Msg derived classes used in the MsgBoard 
// application. See header for completed list of these classes.  Below
// functions for each class are listed together.  Classes should be 
// listed in alphabetical order and a double line of *'s are used to 
// separate the classes.


#include "stdafx.h"
#include "ntcssapi.h"
#include "mb_doc.h"
#include "msgs.h"


//***************************************************************************
//***************************************************************************
// CmsgADDMSGBB
// _____________


/////////////////////////////////////////////////////////////////////////////
// CmsgADDMSGBB constructor
// _________________________

CmsgADDMSGBB::CmsgADDMSGBB()
{
    // No special processing for now
}





/////////////////////////////////////////////////////////////////////////////
// CmsgADDMSGBB::Load
// _______________________

BOOL CmsgADDMSGBB::Load(const char *bb_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    Saddmsgbb_svrmsgrec *svr_msg_ptr = 
                                        new Saddmsgbb_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_ADDMSGBB_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->bb_name, bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_ADDMSGBB_MSGSTR);

    SetMsgBadMsg(BAD_ADDMSGBB_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Saddmsgbb_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_ADDMSGBB_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgADDMSGBB::ShowMsg
// ___________________________

void CmsgADDMSGBB::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgADDMSGBB message details not available";
}


#endif // _NTCSS_MSG_DEBUG_

//***************************************************************************
//***************************************************************************
// CmsgCREATEMSGFILE
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE constructor
// _________________________

CmsgCREATEMSGFILE::CmsgCREATEMSGFILE()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE destructor
// _________________________

CmsgCREATEMSGFILE::~CmsgCREATEMSGFILE()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE::Load
// ___________________

BOOL CmsgCREATEMSGFILE::Load(const char *msg_id, const char *msg_title)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Screatemsgfile_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_CREATEMSGFILE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->msg_id, msg_id,
                SIZE_MSGBOARD_KEY,' ');
                
    strncpyWpad(svr_msg_ptr->msg_title, msg_title,
                SIZE_MSG_TITLE,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_CREATEMSGFILE_MSGSTR);

    SetMsgBadMsg(BAD_CREATEMSGFILE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Screatemsgfile_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_CREATEMSGFILE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



//***************************************************************************
//***************************************************************************
// CmsgDELMSGBB
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgDELMSGBB constructor
// _________________________

CmsgDELMSGBB::CmsgDELMSGBB()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgDELMSGBB destructor
// _________________________

CmsgDELMSGBB::~CmsgDELMSGBB()
{

	delete svr_msg_ptr;

}


/////////////////////////////////////////////////////////////////////////////
// CmsgDELMSGBB::Load
// _______________________

BOOL CmsgDELMSGBB::Load(const char *bb_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sdelmsgbb_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_DELMSGBB_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->bb_name, bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_DELMSGBB_MSGSTR);

    SetMsgBadMsg(BAD_DELMSGBB_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sdelmsgbb_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_DELMSGBB_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_


/////////////////////////////////////////////////////////////////////////////
// CmsgDELMSGBB::ShowMsg
// ___________________________

void CmsgDELMSGBB::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgDELMSGBB message details not available";
}


#endif // _NTCSS_MSG_DEBUG_



//***************************************************************************
//***************************************************************************
// CmsgGETALLBBS
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS constructor
// ________________________________

CmsgGETALLBBS::CmsgGETALLBBS()
{
    m_num_msgboard_cnt = -1;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS destructor
// ________________________________

CmsgGETALLBBS::~CmsgGETALLBBS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS::Load
// ___________________

BOOL CmsgGETALLBBS::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
	svr_msg_ptr = new Sgetallbbs_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETALLBBS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETALLBBS_MSGSTR);

    SetMsgBadMsg(BAD_GETALLBBS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetallbbs_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETALLBBS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS::GetListCount
// __________________________________

BOOL CmsgGETALLBBS::GetListCount(int *msgboard_cnt_ptr)
{
    if (m_num_msgboard_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_msgboard_cnt);

        if (ok)
        {
            *msgboard_cnt_ptr = m_num_msgboard_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS::UnLoad
// _____________________

BOOL CmsgGETALLBBS::UnLoad(const int max_msgboard_cnt,
								  int *num_msgboard_ptr,
								  Sbasic_msgboard_rec *msgboard_list_ptr)
{
    BOOL ok = TRUE;

    if (m_num_msgboard_cnt == -1)
    {
        ok = UnBundle("I",&m_num_msgboard_cnt);

        if (!ok)
            return(FALSE);
    }

	if (m_num_msgboard_cnt < 1)
	{
		return(FALSE);
	}

    *num_msgboard_ptr = m_num_msgboard_cnt;

    if (ok)
    {
        if (*num_msgboard_ptr > max_msgboard_cnt)
            *num_msgboard_ptr = max_msgboard_cnt;

		Sbasic_msgboard_rec *tmp_ptr = msgboard_list_ptr;

		for (int i=0; ((i < *num_msgboard_ptr) && ok); i++)
		{
			ok = UnBundle("C", tmp_ptr->msgboard_name,
						 &(tmp_ptr->unread_msg_cnt));
			
			tmp_ptr++;
		}
    }    

    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_


/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS::ShowMsg
// ___________________________

void CmsgGETSUBSCRIBEDBBS::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETSUBSCRIBEDBBS message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




//***************************************************************************
//***************************************************************************
// CmsgGETBBMSGS
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS constructor
// _________________________

CmsgGETBBMSGS::CmsgGETBBMSGS()
{
    m_num_msgs_cnt = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS destructor
// _________________________

CmsgGETBBMSGS::~CmsgGETBBMSGS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS::Load
// ___________________

BOOL CmsgGETBBMSGS::Load(char *bb_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetbbmsgs_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETBBMSGS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->bb_name, bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETBBMSGS_MSGSTR);

    SetMsgBadMsg(BAD_GETBBMSGS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetbbmsgs_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETBBMSGS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS::GetListCount
// ___________________________

BOOL CmsgGETBBMSGS::GetListCount(int *msg_cnt_ptr)
{
    if (m_num_msgs_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_msgs_cnt);

        if (ok)
        {
            *msg_cnt_ptr = m_num_msgs_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS::UnLoad
// _____________________

BOOL CmsgGETBBMSGS::UnLoad(const int max_msg_cnt,
						   int *msg_cnt_ptr,
						   Sbasic_msglist_rec *msg_list_ptr)
{
    BOOL ok = TRUE;

    if (m_num_msgs_cnt == -1)
    {
        ok = UnBundle("I",&m_num_msgs_cnt);

        if (!ok)
            return(FALSE);
    }

	if (m_num_msgs_cnt < 1)
	{
		return(FALSE);
	}

    *msg_cnt_ptr = m_num_msgs_cnt;

    if (ok)
    {
        if (*msg_cnt_ptr > max_msg_cnt)
            *msg_cnt_ptr = max_msg_cnt;

		Sbasic_msglist_rec *tmp_ptr = msg_list_ptr;

		int is_read;
		for (int i=0; ((i < *msg_cnt_ptr) && ok); i++)
		{
			ok = UnBundle("CCI", tmp_ptr->key,
						  tmp_ptr->title,
						  &is_read);
			
			tmp_ptr->readsent_flag = (is_read == 1);
			
									   // GETBBMSGS has msg_id as
									   //  set to contain both post_date
									   //  and key, extract them here
									   //
			*(tmp_ptr->post_date)   = *(tmp_ptr->key+0);
			*(tmp_ptr->post_date+1) = *(tmp_ptr->key+1);
			*(tmp_ptr->post_date+2) = '-';
			*(tmp_ptr->post_date+3) = *(tmp_ptr->key+2);
			*(tmp_ptr->post_date+4) = *(tmp_ptr->key+3);
			*(tmp_ptr->post_date+5) = '-';
			*(tmp_ptr->post_date+6) = *(tmp_ptr->key+4);
			*(tmp_ptr->post_date+7) = *(tmp_ptr->key+5);
			*(tmp_ptr->post_date+8) = '\0';

			if (!isalpha(*(tmp_ptr->key+13)))
				strcpy(tmp_ptr->msgboard_name, "System");
			else
				strcpy(tmp_ptr->msgboard_name, (tmp_ptr->key+13));

			tmp_ptr++;
		}
    }    

    return(ok);
}



//***************************************************************************
//***************************************************************************
// CmsgGETMSGFILE
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE constructor
// _________________________

CmsgGETMSGFILE::CmsgGETMSGFILE()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE destructor
// _________________________

CmsgGETMSGFILE::~CmsgGETMSGFILE()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE::Load
// ___________________

BOOL CmsgGETMSGFILE::Load(const char *msg_id)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetmsgfile_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETMSGFILE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->msg_id, msg_id,
                SIZE_MSGBOARD_KEY,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETMSGFILE_MSGSTR);

    SetMsgBadMsg(BAD_GETMSGFILE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetmsgfile_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETMSGFILE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE::UnLoad
// _____________________

BOOL CmsgGETMSGFILE::UnLoad(char *msg_path)
{
    BOOL ok = UnBundle("C", msg_path);
			

    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE::ShowMsg
// ___________________________

void CmsgGETMSGFILE::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETMSGFILE message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




//***************************************************************************
//***************************************************************************
// CmsgGETNEWMSGFILE
// _________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE constructor
// _________________________

CmsgGETNEWMSGFILE::CmsgGETNEWMSGFILE()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE destructor
// _________________________

CmsgGETNEWMSGFILE::~CmsgGETNEWMSGFILE()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE::Load
// ___________________

BOOL CmsgGETNEWMSGFILE::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetnewmsgfile_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETNEWMSGFILE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETNEWMSGFILE_MSGSTR);

    SetMsgBadMsg(BAD_GETNEWMSGFILE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetnewmsgfile_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETNEWMSGFILE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE::UnLoad
// _____________________

BOOL CmsgGETNEWMSGFILE::UnLoad(char *msg_path)
{
    BOOL ok = UnBundle("C", msg_path);
			

    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE::ShowMsg
// ___________________________

void CmsgGETNEWMSGFILE::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETNEWMSGFILE message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




//***************************************************************************
//***************************************************************************
// CmsgGETOWNBBMSGS
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS constructor
// _________________________

CmsgGETOWNBBMSGS::CmsgGETOWNBBMSGS()
{
    m_num_msgs_cnt = -1;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS destructor
// _________________________

CmsgGETOWNBBMSGS::~CmsgGETOWNBBMSGS()
{
	delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS::Load
// ___________________

BOOL CmsgGETOWNBBMSGS::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetownbbmsgs_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_GETOWNBBMSGS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETOWNBBMSGS_MSGSTR);

    SetMsgBadMsg(BAD_GETOWNBBMSGS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetownbbmsgs_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETOWNBBMSGS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS::GetListCount
// ______________________________

BOOL CmsgGETOWNBBMSGS::GetListCount(int *msg_cnt_ptr)
{
    if (m_num_msgs_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_msgs_cnt);

        if (ok)
        {
            *msg_cnt_ptr = m_num_msgs_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS::UnLoad
// _____________________

BOOL CmsgGETOWNBBMSGS::UnLoad(const int max_msg_cnt,
						   int *msg_cnt_ptr,
						   Sbasic_msglist_rec *msg_list_ptr)
{
    BOOL ok = TRUE;

    if (m_num_msgs_cnt == -1)
    {
        ok = UnBundle("I",&m_num_msgs_cnt);

        if (!ok)
            return(FALSE);
    }

	if (m_num_msgs_cnt < 1)
	{
		return(FALSE);
	}

    *msg_cnt_ptr = m_num_msgs_cnt;

    if (ok)
    {
        if (*msg_cnt_ptr > max_msg_cnt)
            *msg_cnt_ptr = max_msg_cnt;

		Sbasic_msglist_rec *tmp_ptr = msg_list_ptr;

		int is_sent;
		char tmp_buf[SIZE_SMALL_GENBUF];

		for (int i=0; ((i < *msg_cnt_ptr) && ok); i++)
		{
			ok = UnBundle("CCI", tmp_buf,
						  tmp_ptr->title,
						  &is_sent);
			
			tmp_ptr->readsent_flag = (is_sent == 1);
			
									   // GETBBMSGS has msg_id as
									   //  set to contain both post_date
									   //  and key, extract them here
									   //
			*(tmp_ptr->post_date) = tmp_buf[0];
			*(tmp_ptr->post_date+1) = tmp_buf[1];
			*(tmp_ptr->post_date+2) = '-';
			*(tmp_ptr->post_date+3) = tmp_buf[2];
			*(tmp_ptr->post_date+4) = tmp_buf[3];
			*(tmp_ptr->post_date+5) = '-';
			*(tmp_ptr->post_date+6) = tmp_buf[4];
			*(tmp_ptr->post_date+7) = tmp_buf[5];
			*(tmp_ptr->post_date+8) = '\0';

			strcpy(tmp_ptr->key, &(tmp_buf[7]));

			if (!isalpha(*(tmp_ptr->key+13)))
				strcpy(tmp_ptr->msgboard_name, "System");
			else
				strcpy(tmp_ptr->msgboard_name, (tmp_ptr->key+13));


			tmp_ptr++;
		}
    }    

    return(ok);
}



//***************************************************************************
//***************************************************************************
// CmsgGETSUBSCRIBEDBBS
// ____________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS constructor
// ________________________________

CmsgGETSUBSCRIBEDBBS::CmsgGETSUBSCRIBEDBBS()
{
    m_num_msgboard_cnt = -1;
}




/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS destructor
// ________________________________

CmsgGETSUBSCRIBEDBBS::~CmsgGETSUBSCRIBEDBBS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS::Load
// __________________________

BOOL CmsgGETSUBSCRIBEDBBS::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetsubscribedbbs_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETSUBSCRIBEDBBS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETSUBSCRIBEDBBS_MSGSTR);

    SetMsgBadMsg(BAD_GETSUBSCRIBEDBBS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetsubscribedbbs_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETSUBSCRIBEDBBS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS::GetListCount
// __________________________________

BOOL CmsgGETSUBSCRIBEDBBS::GetListCount(int *msgboard_cnt_ptr)
{
    if (m_num_msgboard_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_msgboard_cnt);

        if (ok)
        {
            *msgboard_cnt_ptr = m_num_msgboard_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS::UnLoad
// _____________________________

BOOL CmsgGETSUBSCRIBEDBBS::UnLoad(const int max_msgboard_cnt,
								  int *num_msgboard_ptr,
								  Sbasic_msgboard_rec *msgboard_list_ptr)
{
    BOOL ok = TRUE;

    if (m_num_msgboard_cnt == -1)
    {
        ok = UnBundle("I",&m_num_msgboard_cnt);

        if (!ok)
            return(FALSE);
    }

	if (m_num_msgboard_cnt < 1)
	{
		return(FALSE);
	}

    *num_msgboard_ptr = m_num_msgboard_cnt;

    if (ok)
    {
        if (*num_msgboard_ptr > max_msgboard_cnt)
            *num_msgboard_ptr = max_msgboard_cnt;

		Sbasic_msgboard_rec *tmp_ptr = msgboard_list_ptr;

		for (int i=0; ((i < *num_msgboard_ptr) && ok); i++)
		{
			ok = UnBundle("CI", tmp_ptr->msgboard_name,
						 &(tmp_ptr->unread_msg_cnt));
			
			tmp_ptr++;
		}
    }    

    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_


/////////////////////////////////////////////////////////////////////////////
// CmsgGETSUBSCRIBEDBBS::ShowMsg
// ___________________________

void CmsgGETSUBSCRIBEDBBS::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETSUBSCRIBEDBBS message details not available";
}


#endif // _NTCSS_MSG_DEBUG_



//***************************************************************************
//***************************************************************************
// CmsgUNGETSUBSCRIBEDBBS
// ______________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS constructor
// __________________________________

CmsgGETUNSUBSCRIBEDBBS::CmsgGETUNSUBSCRIBEDBBS()
{
    m_num_msgboard_cnt = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS destructor
// __________________________________

CmsgGETUNSUBSCRIBEDBBS::~CmsgGETUNSUBSCRIBEDBBS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS::Load
// __________________________

BOOL CmsgGETUNSUBSCRIBEDBBS::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetunsubscribedbbs_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETUNSUBSCRIBEDBBS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_GETUNSUBSCRIBEDBBS_MSGSTR);

    SetMsgBadMsg(BAD_GETUNSUBSCRIBEDBBS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetunsubscribedbbs_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETUNSUBSCRIBEDBBS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS::GetListCount
// __________________________________

BOOL CmsgGETUNSUBSCRIBEDBBS::GetListCount(int *msgboard_cnt_ptr)
{
    if (m_num_msgboard_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_msgboard_cnt);

        if (ok)
        {
            *msgboard_cnt_ptr = m_num_msgboard_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS::UnLoad
// _____________________________

BOOL CmsgGETUNSUBSCRIBEDBBS::UnLoad(const int max_msgboard_cnt,
								  int *num_msgboard_ptr,
								  Sbasic_msgboard_rec *msgboard_list_ptr)
{
    BOOL ok = TRUE;

    if (m_num_msgboard_cnt == -1)
    {
        ok = UnBundle("I",&m_num_msgboard_cnt);

        if (!ok)
            return(FALSE);
    }

	if (m_num_msgboard_cnt < 1)
	{
		return(FALSE);
	}

    *num_msgboard_ptr = m_num_msgboard_cnt;

    if (ok)
    {
        if (*num_msgboard_ptr > max_msgboard_cnt)
            *num_msgboard_ptr = max_msgboard_cnt;

		Sbasic_msgboard_rec *tmp_ptr = msgboard_list_ptr;

		for (int i=0; ((i < *num_msgboard_ptr) && ok); i++)
		{
			ok = UnBundle("C", tmp_ptr->msgboard_name,
						 &(tmp_ptr->unread_msg_cnt));
			
			tmp_ptr++;
		}
    }    

    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS::ShowMsg
// ___________________________

void CmsgGETUNSUBSCRIBEDBBS::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETUNSUBSCRIBEDBBS message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


//***************************************************************************
//***************************************************************************
// CmsgMARKMSGREAD
// _________________



/////////////////////////////////////////////////////////////////////////////
// CmsgMARKMSGREAD constructor
// _________________________

CmsgMARKMSGREAD::CmsgMARKMSGREAD()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgMARKMSGREAD destructor
// _________________________

CmsgMARKMSGREAD::~CmsgMARKMSGREAD()
{
	delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgMARKMSGREAD::Load
// ___________________

BOOL CmsgMARKMSGREAD::Load(const char *msg_id)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Smarkmsgread_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_MARKMSGREAD_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->msg_id, msg_id,
                SIZE_MSGBOARD_KEY,' ');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_MARKMSGREAD_MSGSTR);

    SetMsgBadMsg(BAD_MARKMSGREAD_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Smarkmsgread_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_MARKMSGREAD_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgMARKMSGREAD::ShowMsg
// ___________________________

void CmsgMARKMSGREAD::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgMARKMSGREAD message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




//***************************************************************************
//***************************************************************************
// CmsgSENDBBMSG
// _________________



/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG constructor
// _________________________

CmsgSENDBBMSG::CmsgSENDBBMSG()
{
    // No special processing for now
}



/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG destructor
// _________________________

CmsgSENDBBMSG::~CmsgSENDBBMSG()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG::Load
// ___________________

BOOL CmsgSENDBBMSG::Load(const char *msg_id, const char *bb_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Ssendbbmsg_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_SENDBBMSG_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
//char buf[80];
//sprintf(buf,"The msg_id: <%s>",msg_id);
//AfxMessageBox(buf);
//sprintf(buf,"The bb_name: <%s>",bb_name);
//AfxMessageBox(buf);

    strncpyWpad(svr_msg_ptr->msg_id, msg_id,
                SIZE_MSGBOARD_KEY,' ');
                
    strncpyWpad(svr_msg_ptr->bb_name,bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_SENDBBMSG_MSGSTR);

    SetMsgBadMsg(BAD_SENDBBMSG_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Ssendbbmsg_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SENDBBMSG_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG::ShowMsg
// ___________________________

void CmsgSENDBBMSG::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgSENDBBMSG message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




//***************************************************************************
//***************************************************************************
// CmsgSUBSCRIBEBB
// _________________



/////////////////////////////////////////////////////////////////////////////
// CmsgSUBSCRIBEBB constructor
// _________________________

CmsgSUBSCRIBEBB::CmsgSUBSCRIBEBB()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgSUBSCRIBEBB destructor
// _________________________

CmsgSUBSCRIBEBB::~CmsgSUBSCRIBEBB()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgSUBSCRIBEBB::Load
// ___________________

BOOL CmsgSUBSCRIBEBB::Load(const char *bb_name, BOOL subscribe)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Ssubscribebb_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_SUBSCRIBEBB_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->bb_name, bb_name,
                SIZE_MSGBOARD_NAME,' ');
    
	if (subscribe)            
		svr_msg_ptr->flag = SUBSCRIBE_FLAG;
	else
		svr_msg_ptr->flag = UNSUBSCRIBE_FLAG;

    strncpyWpad(svr_msg_ptr->bb_name, bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_SUBSCRIBEBB_MSGSTR);

    SetMsgBadMsg(BAD_SUBSCRIBEBB_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Ssubscribebb_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SUBSCRIBEBB_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgSUBSCRIBEBB::ShowMsg
// ___________________________

void CmsgSUBSCRIBEBB::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgSUBSCRIBEBB message details not available";
}


#endif // _NTCSS_MSG_DEBUG_




