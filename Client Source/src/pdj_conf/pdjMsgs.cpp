/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


#include "stdafx.h"
#include "ntcssapi.h"
#include "pdjMsgs.h"

//#include "Dll_defs.h"
//***************************************************************************
//***************************************************************************
// CmsgGETPDJINI
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgGETPDJINI constructor
// _______________________________

CmsgGETPDJINI::CmsgGETPDJINI()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI destructor
// _______________________________

CmsgGETPDJINI::~CmsgGETPDJINI()
{
	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI::Load
// _________________________

BOOL CmsgGETPDJINI::Load(char *servername)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
   svr_msg_ptr = new Sgetpdjini_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETPDJINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);
	SetMsgServerName(servername);

    SetMsgOkayMsg(OK_GETPDJINI_MSGSTR);
    SetMsgBadMsg(BAD_GETPDJINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetpdjini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETPDJINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI::UnLoad
// _______________________

BOOL CmsgGETPDJINI::UnLoad(char* filename)
{
   return UnBundle("C", filename);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgPUTPDJINI constructor
// _______________________________

CmsgPUTPDJINI::CmsgPUTPDJINI()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI destructor
// _______________________________

CmsgPUTPDJINI::~CmsgPUTPDJINI()
{
	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPDJINI::Load
// _________________________

BOOL CmsgPUTPDJINI::Load(char* filename,char *servername)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
   svr_msg_ptr = new Sputpdjini_svrmsgrec;

   strncpyWpad(svr_msg_ptr->bsc.header,DO_PUTPDJINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->filename,filename,
              NTCSS_SIZE_COMMAND_LINE,' ');

                
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

	SetMsgServerName(servername);

    SetMsgOkayMsg(OK_PUTPDJINI_MSGSTR);
    SetMsgBadMsg(BAD_PUTPDJINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sputpdjini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_PUTPDJINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPDJINI::UnLoad
// _______________________

BOOL CmsgPUTPDJINI::UnLoad(char* filename)
{
   return UnBundle("C", filename);
}









