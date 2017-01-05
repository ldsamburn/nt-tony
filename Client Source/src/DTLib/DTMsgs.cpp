/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


#include "stdafx.h"
#include "ntcssapi.h"
#include "DTMsgs.h"

//#include "Dll_defs.h"
//***************************************************************************
//***************************************************************************
// CmsgGETPDJINI
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEPASSWORD constructor
// _______________________________

CmsgCHANGEPASSWORD::CmsgCHANGEPASSWORD()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEPASSWORD destructor
// _______________________________

CmsgCHANGEPASSWORD::~CmsgCHANGEPASSWORD()
{
	//delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJLIST::Load
// _________________________

BOOL CmsgCHANGEPASSWORD::Load(char *strOldPasswordCrypted,char *strNewPasswordCrypted)
{
Schangepassword_svrmsgrec *svr_msg_ptr;
                                     // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

	svr_msg_ptr = new Schangepassword_svrmsgrec;

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    strncpyWpad(svr_msg_ptr->bsc.header,DO_CHANGEPASSWORD_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                                       // Now fixup msg so dll will
                                       //  be able to process
 								
    SetMsgServerSelect(USE_MASTER_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);
//	SetMsgServerName(servername);

    strncpyWpad(svr_msg_ptr->oldPwd,strOldPasswordCrypted,
                SIZE_PASSWORD*2,' ');
    strncpyWpad(svr_msg_ptr->newPwd,strNewPasswordCrypted,
                SIZE_PASSWORD*2,' ');
    svr_msg_ptr->null_tail = '\0';

    SetMsgOkayMsg(OK_CHANGEPASSWORD_MSGSTR);
    SetMsgBadMsg(BAD_CHANGEPASSWORD_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Schangepassword_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_CHANGEPASSWORD_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}

///////////////////////////////////////////////////////////
// CmsgRunRemote
///////////////////////////////////////////////////////////

CmsgRunRemote::CmsgRunRemote()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgRunRemote destructor
// _______________________________

CmsgRunRemote::~CmsgRunRemote()
{
	//delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgRunRemote::Load
// _________________________

BOOL CmsgRunRemote::Load(char *strAppName,char *strCommand, char *strOwner, char *strHostName)
{
	Srunremote_svrmsgrec *svr_msg_ptr;
                                     // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

	svr_msg_ptr = new Srunremote_svrmsgrec;

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    strncpyWpad(svr_msg_ptr->bsc.header,DO_LAUNCHINTERACTIVE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                                       // Now fixup msg so dll will
                                       //  be able to process
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_CMDPORT);
//	SetMsgServerName(servername);

    strncpyWpad(svr_msg_ptr->strAppName,strAppName,
                SIZE_GROUP_TITLE,' ');
    strncpyWpad(svr_msg_ptr->strCommand,strCommand,
                SIZE_CMD_LINE,' ');
    strncpyWpad(svr_msg_ptr->strOwner,strOwner,
                SIZE_LOGIN_NAME,' ');
    svr_msg_ptr->null_tail = '\0';

    SetMsgOkayMsg(OK_LAUNCHINTERACTIVE_MSGSTR);
    SetMsgBadMsg(BAD_LAUNCHINTERACTIVE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Srunremote_svrmsgrec));
	SetMsgServerName(strHostName);
    SetMsgRecvDataChainSize(SIZE_LAUNCHINTERACTIVE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);

}

/////////////////////////////////////////////////////////////////////////////
// CmsgRunRemote::UnLoad
// _______________________

BOOL CmsgRunRemote::UnLoadPID(char *strPID)
{
 
   if (UnBundle("C", strPID))
		return(TRUE);
   else
	   return(FALSE);

}



///////////////////////////////////////////////////////////
// CmsgLogout
///////////////////////////////////////////////////////////

CmsgLogout::CmsgLogout()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgLogout destructor
// _______________________________

CmsgLogout::~CmsgLogout()
{  
	if(svr_msg_ptr)
	 delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgLogout::Load
// _________________________

BOOL CmsgLogout::Load(char *strUnlockedApps)
{

                                     // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

	svr_msg_ptr = new Slogout_svrmsgrec;

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    strncpyWpad(svr_msg_ptr->bsc.header,DO_LOGOUT_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                                       // Now fixup msg so dll will
                                       //  be able to process
 								
    SetMsgServerSelect(USE_CONNECTED_SERVER);
    SetMsgPortNum(NTCSS_PORT_DSKPORT);

    strncpyWpad(svr_msg_ptr->strUnlockedApps,strUnlockedApps,
                SIZE_UNLOCKED_APPS,' ');
    svr_msg_ptr->null_tail = '\0';

    SetMsgOkayMsg(OK_LOGOUT_MSGSTR);
    SetMsgBadMsg(BAD_LOGOUT_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Slogout_svrmsgrec));
    SetMsgRecvDataChainSize(SIZE_LOGOUT_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);

}

/*
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// CmsgKILLPROCESS
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgKILLPROCESS constructor
// _______________________________

CmsgKILLPROCESS::CmsgKILLPROCESS()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgKILLPROCESS destructor
// _______________________________

CmsgKILLPROCESS::~CmsgKILLPROCESS()
{
//	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgKILLPROCESS::Load
// _________________________

BOOL CmsgKILLPROCESS::Load(char *progroup, int killtype,char *pid, char *servername)
{
Skillprocess_svrmsgrec *svr_msg_ptr;
                                      // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Skillprocess_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_KILLPROCESS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->pid,pid,
                SIZE_TOKEN,' ');
	wsprintf((char far *)&svr_msg_ptr->killtype,"%02d",killtype);

    strncpyWpad(svr_msg_ptr->progroup,progroup,
                SIZE_GROUP_TITLE,' ');
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_KILLPROCESS_MSGSTR);
    SetMsgBadMsg(BAD_KILLPROCESS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Skillprocess_svrmsgrec));
	SetMsgServerName(servername);

    SetMsgRecvDataChainSize(0);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// CmsgREMOVEPROCESS
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgREMOVEPROCESS constructor
// _______________________________

CmsgREMOVEPROCESS::CmsgREMOVEPROCESS()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgKILLPROCESS destructor
// _______________________________

CmsgREMOVEPROCESS::~CmsgREMOVEPROCESS()
{
//	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgREMOVEPROCESS::Load
// _________________________

BOOL CmsgREMOVEPROCESS::Load(char *progroup, char *jobtype,char *pid, char *servername)
{
Sremoveprocess_svrmsgrec *svr_msg_ptr;

                                     // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sremoveprocess_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_REMOVEPROCESS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->pid,pid,
                SIZE_TOKEN,' ');
    strncpyWpad(svr_msg_ptr->jobtype,jobtype,
                SIZE_JOB_TYPE,' ');

    strncpyWpad(svr_msg_ptr->progroup,progroup,
                SIZE_GROUP_TITLE,' ');
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_SPQPORT);

    SetMsgOkayMsg(OK_REMOVEPROCESS_MSGSTR);
    SetMsgBadMsg(BAD_REMOVEPROCESS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sremoveprocess_svrmsgrec));
	SetMsgServerName(servername);

    SetMsgRecvDataChainSize(0);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// CmsgALTERPROCESS
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgALTERPROCESS constructor
// _______________________________

CmsgALTERPROCESS::CmsgALTERPROCESS()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgALTERPROCESS destructor
// _______________________________

CmsgALTERPROCESS::~CmsgALTERPROCESS()
{
//	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgALTERPROCESS::Load
// _________________________

BOOL CmsgALTERPROCESS::Load(char *pid,int state, int priority, 
							char *jobtype, int operation, char *custproc, char *servername)
{
Salterprocess_svrmsgrec *svr_msg_ptr;


    if (MsgState() != RAW)
        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Salterprocess_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_ALTERPROCESS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->pid,pid,
                SIZE_TOKEN,' ');
    strncpyWpad(svr_msg_ptr->jobtype,jobtype,
                SIZE_JOB_TYPE,' ');
	
	wsprintf(svr_msg_ptr->altermask,"%06d",operation);

	if (custproc == NULL)
		strncpyWpad(svr_msg_ptr->customprocstat,"",
			        SIZE_PROC_STAT,' ');
	else
		strncpyWpad(svr_msg_ptr->customprocstat,custproc,
			        SIZE_PROC_STAT,' ');

	wsprintf(svr_msg_ptr->state,"%1.1d",state);
	wsprintf(svr_msg_ptr->priority,"%02d",priority);
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_SPQPORT);

    SetMsgOkayMsg(OK_ALTERPROCESS_MSGSTR);
    SetMsgBadMsg(BAD_ALTERPROCESS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Salterprocess_svrmsgrec));
	SetMsgServerName(servername);

    SetMsgRecvDataChainSize(0);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}

BOOL CmsgALTERPROCESS::UnLoad(char *message)
{

		UnBundle("C", message);
  
	return(TRUE);

}


*/






