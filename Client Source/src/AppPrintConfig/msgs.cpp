/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "ntcssapi.h"
#include "msgs.h"

//***************************************************************************
//***************************************************************************
// CmsgGETOTYPEINI
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI constructor
// _________________________

CmsgGETOTYPEINI::CmsgGETOTYPEINI()
{
    // No special processing for now
}

 
/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI destructor
// _________________________

CmsgGETOTYPEINI::~CmsgGETOTYPEINI()
{
	delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI::Load
// ___________________

BOOL CmsgGETOTYPEINI::Load(CString strHostname)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetotypeini_svrmsgrec;

    //to debug dll timmer... chnage this to some erroneous message
    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETOTYPEINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

		         
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process

	SetMsgServerName((LPCTSTR)strHostname);
	SetMsgServerSelect(USE_GIVEN_SERVER);
                                       
   // SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETOTYPEINI_MSGSTR);

    SetMsgBadMsg(BAD_GETOTYPEINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetotypeini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETOTYPEINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI::UnLoad
// _____________________

BOOL CmsgGETOTYPEINI::UnLoad(char *ini_file_name_path)
{
    BOOL ok = UnBundle("C", ini_file_name_path);
    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI::ShowMsg
// ___________________________

void CmsgGETOTYPEINI::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETOTYPEINI message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


//***************************************************************************
//***************************************************************************
// CmsgPUTOTYPEINI
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI constructor
// _________________________

CmsgPUTOTYPEINI::CmsgPUTOTYPEINI()
{
    // No special processing for now
}

/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI destructor
// _________________________

CmsgPUTOTYPEINI::~CmsgPUTOTYPEINI()
{
	delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI::Load
// ___________________

BOOL CmsgPUTOTYPEINI::Load(const char *file_name,CString strHostname)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sputotypeini_svrmsgrec;

#define  DO_PUTOTYPEINI_MSGSTR    "PUTOTYPEINI"
#define  OK_PUTOTYPEINI_MSGSTR    "GOODOTYPEINI"
#define  BAD_PUTOTYPEINI_MSGSTR   "NOOTYPEINI"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_PUTOTYPEINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	 strncpyWpad(svr_msg_ptr->file_name, file_name,
                SIZE_SERVER_FILE_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerName((LPCTSTR)strHostname);
	SetMsgServerSelect(USE_GIVEN_SERVER);

	//SetMsgServerSelect(USE_CONNECTED_SERVER);

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_PUTOTYPEINI_MSGSTR);

    SetMsgBadMsg(BAD_PUTOTYPEINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sputotypeini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_PUTOTYPEINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI::UnLoad
// _____________________

BOOL CmsgPUTOTYPEINI::UnLoad(char *log_file_name_path)
{
    BOOL ok = UnBundle("C", log_file_name_path);
    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI::ShowMsg
// ___________________________

void CmsgPUTOTYPEINI::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgPUTOTYPEINI message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


