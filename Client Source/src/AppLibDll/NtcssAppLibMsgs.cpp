/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
//#include "ntcssapi.h"
#include "NtcssAppLibMsgs.h"

//#include "Dll_defs.h"
//***************************************************************************
//***************************************************************************
// CmsgREMOTEVALIDATE
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgGETPDJINI constructor
// _______________________________

CmsgREMOTEVALIDATE::CmsgREMOTEVALIDATE()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI destructor
// _______________________________

CmsgREMOTEVALIDATE::~CmsgREMOTEVALIDATE()
{
	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgREMOTEVALIDATE::Load
// _________________________

BOOL CmsgREMOTEVALIDATE::Load(char *logname, char *password, char *progroup, char *hostname)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

//	m_ListCount=-1;

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new SREMOTEVALIDATE_svrmsgrec;

    strncpyWpad(svr_msg_ptr->message,DO_REMOTEVALIDATE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->loginname,logname,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->password,password,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->progroup_title,progroup,
                SIZE_GROUP_TITLE,' ');
               
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_DSKPORT);

	SetMsgServerName(hostname);

    SetMsgOkayMsg(OK_REMOTEVALIDATE_MSGSTR);
    SetMsgBadMsg(BAD_REMOTEVALIDATE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(SREMOTEVALIDATE_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_REMOTEVALIDATE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgREMOTEVALIDATE::UnLoad
// _______________________

BOOL CmsgREMOTEVALIDATE::UnLoad(char *dbpassword)
{
	BOOL ok = TRUE;

	ok = UnBundle("C",dbpassword);    

	return(ok);

}


