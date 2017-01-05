/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Dll_msgs
//                                         
// This is the header file fo*r several NTCSS DLL message
// classes derived from CntcssBaseMsg.
//----------------------------------------------------------------------

#include "stdafx.h"
#include "0_basics.h"

#include "dll_msgs.h"
#include "msgdefs.h"
#include "genutils.h"
#include "stdlib.h"

//***************************************************************************
// CmsgADMINLOCKOPS
// ___________________

/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgADMINLOCKOPS destructor
// _______________________________

CmsgADMINLOCKOPS::~CmsgADMINLOCKOPS()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgADMINLOCKOPS constructor
// _______________________________

CmsgADMINLOCKOPS::CmsgADMINLOCKOPS()
{
    // No special processing for now
}



/////////////////////////////////////////////////////////////////////////////
// CmsgADMINLOCKOPS::Load
// _________________________

BOOL CmsgADMINLOCKOPS::Load(LPCTSTR appname,LPCTSTR code,
							LPCTSTR action,LPCTSTR pid,
							LPCTSTR hostname)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr =  new Sadminlockops_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_ADMINLOCKOPS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_APP_NAME,' ');

    strncpyWpad(svr_msg_ptr->code,code,
                SIZE_ADMINLOCK_OPS,' ');

	strncpyWpad(svr_msg_ptr->action,action,
                SIZE_ADMINLOCK_OPS,' ');

	strncpyWpad(svr_msg_ptr->pid,pid,
                SIZE_ADMIN_PID,' ');

    svr_msg_ptr ->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 	CString temp=hostname;
	temp.TrimLeft();
	//I guess we had problems telling when this was NULL?
	if(!hostname||*hostname==NULL||temp.IsEmpty())						
		SetMsgServerSelect(USE_MASTER_SERVER);
	else
	{
		SetMsgServerName(hostname);
		SetMsgServerSelect(USE_GIVEN_SERVER);
	}

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_ADMINLOCKOPS_MSGSTR);
    SetMsgBadMsg(BAD_ADMINLOCKOPS_MSGSTR);

    SetMsgSendDataPtr((LPTSTR)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sadminlockops_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_ADMINLOCKOPS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgADMINLOCKOPS::UnLoad
// _______________________

BOOL CmsgADMINLOCKOPS::UnLoad(int *pidvalue)
{
    if (UnBundle("I",pidvalue))
    {
        return(TRUE);
    }
    else
        return(FALSE);
}


//***************************************************************************
// CmsgCHANGEAPPDATA
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEAPPDATA constructor
// _______________________________

CmsgCHANGEAPPDATA::CmsgCHANGEAPPDATA()
{
    // No special processing for now
}


CmsgCHANGEAPPDATA::~CmsgCHANGEAPPDATA()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEAPPDATA::Load
// _________________________

BOOL CmsgCHANGEAPPDATA::Load(LPCTSTR appname,
                             LPCTSTR appdata)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Schangeappdata_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_CHANGEAPPDATA_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_GROUP_TITLE,' ');

    strncpyWpad(svr_msg_ptr->appdata,appdata,
                SIZE_APP_DATA,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
 								
    SetMsgServerSelect(USE_AUTH_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_CHANGEAPPDATA_MSGSTR);
    SetMsgBadMsg(BAD_CHANGEAPPDATA_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Schangeappdata_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_CHANGEAPPDATA_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



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


CmsgCREATEMSGFILE::~CmsgCREATEMSGFILE()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE::Load
// ___________________

BOOL CmsgCREATEMSGFILE::Load(LPCTSTR msg_id, LPCTSTR msg_title)
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


    //svr_msg_ptr->msg_id[SIZE_MSGBOARD_KEY]=' '; //a hack
                
    strncpyWpad(svr_msg_ptr->msg_title, msg_title,
                SIZE_MSG_TITLE,' ');

               
    svr_msg_ptr->null_tail = '\0';

	                                    // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_AUTH_SERVER);

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
// CmsgDELETESVRFILE
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgDELETESVRFILE constructor
// _______________________________

CmsgDELETESVRFILE::CmsgDELETESVRFILE()
{
    // No special processing for now
}

CmsgDELETESVRFILE::~CmsgDELETESVRFILE()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgDELETESVRFILE::Load
// _________________________

BOOL CmsgDELETESVRFILE::Load(LPCTSTR SvrName,LPCTSTR AppName,LPCTSTR FileName)
                             
{
                                       // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sdeletesvrfile_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_DELETESVRFILE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

    strncpyWpad(svr_msg_ptr->AppName,AppName,
                SIZE_GROUP_TITLE,' '); 

            
   	strncpyWpad(svr_msg_ptr->FileName,FileName,
                SIZE_SERVER_FILE_NAME,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //

	SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgServerName(SvrName);
 
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_DELETESVRFILE_MSGSTR);
    SetMsgBadMsg(BAD_DELETESVRFILE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sdeletesvrfile_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_DELETESVRFILE_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


//***************************************************************************
//***************************************************************************
// CmsgGETAPPDATA
// ___________________


/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPDATA constructor
// _______________________________

CmsgGETAPPDATA::CmsgGETAPPDATA()
{
    // No special processing for now
}

CmsgGETAPPDATA::~CmsgGETAPPDATA()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPDATA::Load
// _________________________


BOOL CmsgGETAPPDATA::Load(LPCTSTR strAppName)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr =  new Sgetappdata_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETAPPDATA_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	                
    strncpyWpad(svr_msg_ptr->appname,strAppName,
                SIZE_GROUP_TITLE,' ');

    

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
 								
    SetMsgServerSelect(USE_AUTH_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETAPPDATA_MSGSTR);
    SetMsgBadMsg(BAD_GETAPPDATA_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetappdata_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETAPPDATA_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPDATA::UnLoad
// __________________________

BOOL CmsgGETAPPDATA::UnLoad(LPTSTR strAppData)
{
	return UnBundle("C",strAppData);
}


//***************************************************************************
// CmsgGETAPPLIST
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPLIST constructor
// _______________________________

CmsgGETAPPLIST::CmsgGETAPPLIST()
{
    m_num_apps_cnt = -1;
}

CmsgGETAPPLIST::~CmsgGETAPPLIST()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPLIST::getAppCount
// ________________________________

BOOL CmsgGETAPPLIST::getAppCount(int *num_apps_ptr)
{
    if (m_num_apps_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_apps_cnt);

        if (ok)
        {
            *num_apps_ptr = m_num_apps_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPLIST::Load
// _________________________

BOOL CmsgGETAPPLIST::Load()
{
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetapplist_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETAPPLIST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETAPPLIST_MSGSTR);
    SetMsgBadMsg(BAD_GETAPPLIST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetapplist_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETAPPLIST_RESPONSE);

    SetMsgState(READY_TO_GO);
      
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPLIST::UnLoad
// ___________________________

BOOL CmsgGETAPPLIST::UnLoad(const int max_apps_cnt,
                            int *num_apps_ptr,
                            NtcssAppList *app_list_ptr)
{
    NtcssAppList *lcl_app_list_ptr = app_list_ptr;

    BOOL ok = TRUE;
    if (m_num_apps_cnt == -1)
    {
        ok = UnBundle("I",&m_num_apps_cnt);

        if (!ok)
            return(FALSE);
    }
    *num_apps_ptr = m_num_apps_cnt;

    if (*num_apps_ptr > max_apps_cnt)
        *num_apps_ptr = max_apps_cnt;

	for (int i=0; i < *num_apps_ptr; i++)
	{
		ok = UnBundle("CC", lcl_app_list_ptr->app_name,
							lcl_app_list_ptr->host_name);
		lcl_app_list_ptr++;
    }    

    return(ok);
}


//***************************************************************************
// CmsgGETAPPUSERINFO
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERINFO constructor
// _______________________________

CmsgGETAPPUSERINFO::CmsgGETAPPUSERINFO()
{
    // No special processing for now
}

CmsgGETAPPUSERINFO::~CmsgGETAPPUSERINFO()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERINFO::Load
// _________________________

BOOL CmsgGETAPPUSERINFO::Load(LPCTSTR app_name, LPCTSTR username)

{
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetappuserinfo_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETAPPUSERINFO_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

    strncpyWpad(svr_msg_ptr->username,username,
                SIZE_LOGIN_NAME,' ');

    strncpyWpad(svr_msg_ptr->appname,app_name,
                SIZE_GROUP_TITLE,' ');

    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //

  //  SetMsgServerSelect(USE_CONNECTED_SERVER);
  //  SetMsgServerSelect(USE_APP_SERVER);

	SetMsgServerSelect(USE_AUTH_SERVER); //4/2/99

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETAPPUSERINFO_MSGSTR);
    SetMsgBadMsg(BAD_GETAPPUSERINFO_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetappuserinfo_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETAPPUSERINFO_RESPONSE);

    SetMsgState(READY_TO_GO);
                                      // Save this, will need when
                                      //  unload
    strcpy(m_username, username);

    return(TRUE);
}






/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERINFO::UnLoad
// __________________________

BOOL CmsgGETAPPUSERINFO::UnLoad(BOOL *registered_ptr, 
                                NtcssUserInfo  *user_info_ptr,
                                char *access_info_ptr, const char* appname)
{
    int tmp_registered;

    char tmp_name[SIZE_REAL_NAME];

    int tmp_seclvl;

    if (UnBundle("CCCICIIC",
                 tmp_name,
                 user_info_ptr->ssn,
                 user_info_ptr->phone_number,
                 &tmp_seclvl,
                 access_info_ptr,
                 &tmp_registered,
                 &user_info_ptr->unix_id,
				 user_info_ptr->app_passwd))
    {
		
		
        strcpy(user_info_ptr->login_name, m_username);

        nameFixup(user_info_ptr, tmp_name);

        *registered_ptr = (BOOL) tmp_registered;

        user_info_ptr->security_class = tmp_seclvl;

	
        return(TRUE);
    }

    else
        return(FALSE);
}



//***************************************************************************
// CmsgGETAPPUSERLIST
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERLIST constructor
// _______________________________

CmsgGETAPPUSERLIST::CmsgGETAPPUSERLIST()
{
    m_num_users_cnt = -1;
}


CmsgGETAPPUSERLIST::~CmsgGETAPPUSERLIST()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}




/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERLIST::getUserCount
// ________________________________

BOOL CmsgGETAPPUSERLIST::getUserCount(int *num_users_ptr)
{
    if (m_num_users_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_users_cnt);

        if (ok)
        {
            *num_users_ptr = m_num_users_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERLIST::Load
// _________________________

BOOL CmsgGETAPPUSERLIST::Load(LPCTSTR app_name)
{
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetappuserlist_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETAPPUSERLIST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,app_name,SIZE_GROUP_TITLE,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    //SetMsgServerSelect(USE_CONNECTED_SERVER);
	SetMsgServerSelect(USE_AUTH_SERVER);

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETAPPUSERLIST_MSGSTR);
    SetMsgBadMsg(BAD_GETAPPUSERLIST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetappuserlist_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETAPPUSERLIST_RESPONSE);

    SetMsgState(READY_TO_GO);
      
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERLIST::UnLoad
// ___________________________

BOOL CmsgGETAPPUSERLIST::UnLoad(const int max_user_cnt,
                                int *num_users,
                                NtcssAppUserList *user_list_ptr)
{
    BOOL ok = TRUE;
    if (m_num_users_cnt == -1)
    {
        ok = UnBundle("I",&m_num_users_cnt);

        if (!ok)
            return(FALSE);
    }
	if (m_num_users_cnt < 1)
	{
        MsgErrorMan()->RecordError("No application users");
		return(FALSE);
	}
    *num_users = m_num_users_cnt;
	
    if (ok)
    {
        if (*num_users > max_user_cnt)
            *num_users = max_user_cnt;

		NtcssAppUserList *tmp_ptr = user_list_ptr;

		char flag[SIZE_REGISTERED_FLAG];
		for (int i=0; ((i < *num_users) && ok); i++)
		{
			ok = TakeNextBytes(tmp_ptr->appuser_name,
								NTCSS_SIZE_LOGINNAME);

			if (!ok)
				continue;
									   // Here let appuser_reg_flag nullify
									   //  appuser_name so can easily
									   //  trip off extra blanks
			tmp_ptr->appuser_reg_flag = 0;
			trim(tmp_ptr->appuser_name);
		//	AfxMessageBox(tmp_ptr->appuser_name);

			ok = TakeNextBytes(flag,
								SIZE_REGISTERED_FLAG);

									   // First char put into flag is
									   //  always '0', second is '1' if
									   //  register (see msg descrip)
									   //
			tmp_ptr->appuser_reg_flag = (flag[1] == '1');
				
			tmp_ptr++;
		}	
    }    

    return(ok);
}


//***************************************************************************
// CmsgGETCOMMONDB
// _______________


/////////////////////////////////////////////////////////////////////////////
// CmsgGETCOMMONDB constructor
// _______________________________

CmsgGETCOMMONDB::CmsgGETCOMMONDB()
{
    // No special processing for now
}

CmsgGETCOMMONDB::~CmsgGETCOMMONDB()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETCOMMONDB::Load
// _________________________

BOOL CmsgGETCOMMONDB::Load(LPCTSTR tag)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetcommondb_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETCOMMONDB_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->tag,tag,
                SIZE_COMMON_TAG,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETCOMMONDB_MSGSTR);
    SetMsgBadMsg(BAD_GETCOMMONDB_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetcommondb_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETCOMMONDB_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}




/////////////////////////////////////////////////////////////////////////////
// CmsgGETCOMMONDB::UnLoad
// _______________________

BOOL CmsgGETCOMMONDB::UnLoad(char *data)
{
    if (UnBundle("C",data))
    {
        return(TRUE);
    }
    else
        return(FALSE);
}




//***************************************************************************
// CmsgGETDIRLISTING
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING constructor
// _______________________________

CmsgGETDIRLISTING::CmsgGETDIRLISTING()
{
    m_num_files_cnt = -1;
}


CmsgGETDIRLISTING::~CmsgGETDIRLISTING()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING::getUserCount
// ________________________________

BOOL CmsgGETDIRLISTING::getFileCount(int *num_files_ptr)

{
    if (m_num_files_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_files_cnt);

        if (ok)
        {
            *num_files_ptr = m_num_files_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING::Load
// _________________________

BOOL CmsgGETDIRLISTING::Load(LPCTSTR server_name,
                             LPCTSTR dir_name)
{
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetdirlisting_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETDIRLISTING_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->directory,dir_name,SIZE_SERVER_LOCATION,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	CString temp=server_name;
	temp.TrimLeft();
	if(!server_name||*server_name==NULL||temp.IsEmpty())
		SetMsgServerSelect(USE_APP_SERVER);

	else
	{
		SetMsgServerName(server_name);
		SetMsgServerSelect(USE_GIVEN_SERVER);
	}

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETDIRLISTING_MSGSTR);
    SetMsgBadMsg(BAD_GETDIRLISTING_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetdirlisting_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETDIRLISTING_RESPONSE);

    SetMsgState(READY_TO_GO);
      
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING::UnLoad
// ___________________________

BOOL CmsgGETDIRLISTING::UnLoad(SUnixDirInfo *dir_filelist_ptr,
                               int   size_of_dir_filelist,
                               int  *num_put_in_dir_filelist)
{
	
    BOOL ok;

    if (m_num_files_cnt == -1)
    {
        ok = UnBundle("I",&m_num_files_cnt);

        if (!ok)
            return(FALSE);
    }

	

    if (ok)
    {
       // char *last_byte_ptr = dir_filelist_ptr + size_of_dir_filelist - 1;
        *num_put_in_dir_filelist = 0;

        for (int i = 0; i < m_num_files_cnt; i++)
        {
			

            ok = UnBundle("CDIC",&(dir_filelist_ptr->FileName),
				                 &(dir_filelist_ptr->FileDate),
								 &(dir_filelist_ptr->FileSize),
								 &(dir_filelist_ptr->FileOwner));
				                 
            if (!ok)
                break;

            (*num_put_in_dir_filelist)++;
			dir_filelist_ptr++;
			
         
        }
			
    }

    return(ok);
}



//***************************************************************************
// CmsgGETFILEINFO
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILEINFO constructor
// _______________________________

CmsgGETFILEINFO::CmsgGETFILEINFO()
{
    // No special processing for now
}

CmsgGETFILEINFO::~CmsgGETFILEINFO()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILEINFO::Load
// _________________________

BOOL CmsgGETFILEINFO::Load(LPCTSTR server,LPCTSTR fname)
                             
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sgetfileinfo_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETFILEINFO_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
   	strncpyWpad(svr_msg_ptr->fname,fname,
                SIZE_SERVER_LOCATION,' ');

	//todo got to get size & order again from eric & change to char in msgdefs.h

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgServerName(server);
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETFILEINFO_MSGSTR);
    SetMsgBadMsg(BAD_GETFILEINFO_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetfileinfo_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETFILEINFO_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILEINFO::UnLoad
// __________________________

BOOL CmsgGETFILEINFO::UnLoad(int *lines,int *bytes)
{
	return UnBundle("II",lines,bytes);
}


//***************************************************************************
// CmsgGETFILESECTION
// ___________________


/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILESECTION constructor
// _______________________________

CmsgGETFILESECTION::CmsgGETFILESECTION()
{
    // No special processing for now
}

CmsgGETFILESECTION::~CmsgGETFILESECTION()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILESECTION::Load
// _________________________

BOOL CmsgGETFILESECTION::Load(LPCTSTR server,LPCTSTR fname,LPCTSTR FirstLine,
							  LPCTSTR LastLine)
                             
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sgetfilesection_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETFILESECTION_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
   	strncpyWpad(svr_msg_ptr->fname,fname,
                SIZE_SERVER_LOCATION,' ');

	strncpyWpad(svr_msg_ptr->FirstLine,FirstLine,
                SIZE_CHAR_TO_INT,' ');

	strncpyWpad(svr_msg_ptr->LastLine,LastLine,
                SIZE_CHAR_TO_INT,' ');


    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
    if(server)
	{                            
 		SetMsgServerSelect(USE_GIVEN_SERVER);
		SetMsgServerName(server);
	}
	else
		SetMsgServerSelect(USE_APP_SERVER);

		
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETFILESECTION_MSGSTR);
    SetMsgBadMsg(BAD_GETFILESECTION_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetfilesection_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETFILESECTION_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILESECTION::UnLoad
// __________________________

BOOL CmsgGETFILESECTION::UnLoad(int *lines,char *buf)
{

	return UnBundle("IP",lines,buf);
	
}



//***************************************************************************
// CmsgGETMASTERNAME
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETMASTERNAME constructor
// _______________________________

CmsgGETMASTERNAME::CmsgGETMASTERNAME()
{
    // No special processing for now
}

CmsgGETMASTERNAME::~CmsgGETMASTERNAME()
{

	TRACE("IN DESTRUCTOR\n");

	if(svr_msg_ptr) 
	{
		delete svr_msg_ptr;
		svr_msg_ptr=NULL;

	}

	TRACE("OUT DESTRUCTOR\n");
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETMASTERNAME::Load
// _________________________

BOOL CmsgGETMASTERNAME::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sgetmastername_svrmsgrec ;


    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETMASTERNAME_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_CONNECTED_SERVER);
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETMASTERNAME_MSGSTR);
    SetMsgBadMsg(BAD_GETMASTERNAME_MSGSTR);

	SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetmastername_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETMASTERNAME_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETMASTERNAME::UnLoad
// _______________________

BOOL CmsgGETMASTERNAME::UnLoad(char *strMasterName)
{
    return UnBundle("C",strMasterName);
    
}



//***************************************************************************
// CmsgGETLINKDATA
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETLINKDATA constructor
// _______________________________

CmsgGETLINKDATA::CmsgGETLINKDATA()
{
    // No special processing for now
}

CmsgGETLINKDATA::~CmsgGETLINKDATA()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETLINKDATA::Load
// _________________________

BOOL CmsgGETLINKDATA::Load(LPCTSTR appname)
                             
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetlinkdata_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETLINKDATA_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_GROUP_TITLE,' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
 								
    //SetMsgServerSelect(USE_CONNECTED_SERVER);
	SetMsgServerSelect(USE_AUTH_SERVER); //4/23/99
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETLINKDATA_MSGSTR);
    SetMsgBadMsg(BAD_GETLINKDATA_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetlinkdata_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETLINKDATA_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETLINKDATA::UnLoad
// __________________________

BOOL CmsgGETLINKDATA::UnLoad(char *LinkData)
{
	return UnBundle("C",LinkData);
}



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

CmsgGETNEWMSGFILE::~CmsgGETNEWMSGFILE()
{
    if(svr_msg_ptr)
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


//***************************************************************************
// CmsgGETUSERPRTLIST
// ___________________


/////////////////////////////////////////////////////////////////////////////
// CmsgGETUSERPRTLIST constructor
// _______________________________

CmsgGETUSERPRTLIST::CmsgGETUSERPRTLIST()
{
    m_num_printers = -1;
}

CmsgGETUSERPRTLIST::~CmsgGETUSERPRTLIST()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgGETUSERPRTLIST::Load
// _________________________

BOOL CmsgGETUSERPRTLIST::Load(LPCTSTR the_server_name,
                              LPCTSTR the_app_name, 
                              LPCTSTR the_output_type,
                              const SecurityLevel the_security_class,
                              const BOOL the_default_only_flag,
                              const BOOL the_access_flag)

{
    if ( (the_security_class < NTCSS_UNCLASSIFIED) ||
         (NTCSS_TOPSECRET < the_security_class) )
    {
        MsgErrorMan()->RecordError("Invalid Parameters");
        return(FALSE);
    }

                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();
                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetuserprtlist_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETUSERPRTLIST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,the_app_name,
                SIZE_GROUP_TITLE,' ');

    strncpyWpad(svr_msg_ptr->output_type,the_output_type,
                SIZE_PRT_OUTPUT_TYPE,' ');

                                       // Note that unlike for other
                                       //   messages, here security
                                       //   class is just one byte
                                       //
    sprintf(&svr_msg_ptr->security_class,"%d",(int)the_security_class);

    if (the_default_only_flag)
    
        svr_msg_ptr->default_only_flag = DEFAULT_ONLY_FLAGVAL;

    else
        svr_msg_ptr->default_only_flag = ALL_PRINTERS_FLAGVAL;
    
    if (the_access_flag)
    
        svr_msg_ptr->access_flag = ACCESS_GEN_FLAGVAL;

    else
        svr_msg_ptr->access_flag = ACCESS_BYAPP_FLAGVAL;
    
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgServerName(the_server_name);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_GETUSERPRTLIST_MSGSTR);
    SetMsgBadMsg(BAD_GETUSERPRTLIST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetuserprtlist_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETUSERPRTLIST_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUSERPRTLIST::getPrinterCount
// ___________________________________

BOOL CmsgGETUSERPRTLIST::getPrinterCount(int *num_printers_ptr)
{
    if (m_num_printers == -1)
    {
        if(!UnBundle("I",&m_num_printers))
			return FALSE;
        *num_printers_ptr = m_num_printers;
  	}

	 return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUSERPRTLIST::UnLoad
// ___________________________

BOOL CmsgGETUSERPRTLIST::UnLoad(SremotePrinterInfo *printer_list_ptr,
                                int max_printers)
{
    BOOL ok = TRUE;
	
    if (m_num_printers == -1)
	   if(!UnBundle("I",&m_num_printers))
			return FALSE;

    int num_printers = m_num_printers;

    if (num_printers > max_printers)
        num_printers = max_printers;

    if (ok)
    {
        int lcl_default_printer_flag;
        int lcl_queue_enabled_flag;
        int lcl_printer_enabled_flag;
        SremotePrinterInfo *lcl_printer_list_ptr = printer_list_ptr;

        for (int i = 0; i < num_printers; i++)
        {
            ok = UnBundle("CCCICCIICCCCCCCCC",
		
                          &(lcl_printer_list_ptr->printer_name_buf),
                          &(lcl_printer_list_ptr->host_name_buf),
                          &(lcl_printer_list_ptr->printer_loc_buf),
                          &lcl_default_printer_flag,
                          &(lcl_printer_list_ptr->printer_device_name_buf),
                          &(lcl_printer_list_ptr->printer_driver_name_buf),
                          &lcl_queue_enabled_flag,
                          &lcl_printer_enabled_flag,
                          &(lcl_printer_list_ptr->redirect_printer_buf),
						  &(lcl_printer_list_ptr->printer_NTdriver_name_buf),
						  &(lcl_printer_list_ptr->printer_datafile_name_buf),
						  &(lcl_printer_list_ptr->printer_uifile_name_buf),
	                      &(lcl_printer_list_ptr->printer_helpfile_name_buf),
						  &(lcl_printer_list_ptr->printer_monitorfile_name_buf),
						  &(lcl_printer_list_ptr->printer_monitorstring_name_buf),
						  &(lcl_printer_list_ptr->CopyFiles),
						  &(lcl_printer_list_ptr->AtlassFileName)); //Atlass
	

            if (ok)
            {
				
                lcl_printer_list_ptr->default_printer_flag =
                                               lcl_default_printer_flag;
                lcl_printer_list_ptr->queue_enabled_flag =
                                               lcl_queue_enabled_flag;
                lcl_printer_list_ptr->printer_enabled_flag =
                                               lcl_printer_enabled_flag;

                                       // Don't have value for these but
                                       //  init to NULL strings
                                       //
                lcl_printer_list_ptr->output_spool_file[0] = '\0';
                
                lcl_printer_list_ptr++;

			}
            else
                break;
        }
    }


    return(ok);
}




//***************************************************************************
// CmsgKILLSVRPROC
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgKILLSVRPROC constructor
// _______________________________

CmsgKILLSVRPROC::CmsgKILLSVRPROC()
{
    // No special processing for now
}


CmsgKILLSVRPROC::~CmsgKILLSVRPROC()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgKILLSVRPROC::Load
// _________________________

BOOL CmsgKILLSVRPROC::Load(LPCTSTR pid_buf,
                           const int signal_type,
						   LPCTSTR program_group)

{
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Skillsvrproc_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_KILLSVRPROC_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->pid,pid_buf,
                SIZE_PROCESS_ID,' ');
                
    sprintf(svr_msg_ptr->signal_type,"%-*d",SIZE_SIGNAL_TYPE, signal_type);


	strncpyWpad(svr_msg_ptr->program_group,program_group,
                SIZE_GROUP_TITLE,' ');
    
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_APP_SERVER);
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_KILLSVRPROC_MSGSTR);
    SetMsgBadMsg(BAD_KILLSVRPROC_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Skillsvrproc_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_KILLSVRPROC_RESPONSE);

    SetMsgState(READY_TO_GO);

    return(TRUE);
}

//***************************************************************************
// CmsgLAUNCHSVRPROCBG
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgLAUNCHSVRPROCBG constructor
// _______________________________

CmsgLAUNCHSVRPROCBG::CmsgLAUNCHSVRPROCBG()
{
    // No special processing for now
}


CmsgLAUNCHSVRPROCBG::~CmsgLAUNCHSVRPROCBG()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgLAUNCHSVRPROCBG::Load
// _________________________

BOOL CmsgLAUNCHSVRPROCBG::Load
		(const NtcssServerProcessDescr *descr_ptr,
         LPCTSTR app_name_str, 
         LPCTSTR unix_owner_str,
         LPCTSTR prt_name_str, 
         LPCTSTR prt_hostname_str)

{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();
                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
                                       //
	//AfxMessageBox("In load passed in-> " + (CString)app_name_str);

    svr_msg_ptr = new Slaunchsvrprocbg_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_LAUNCHSVRPROCBG_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,app_name_str,SIZE_GROUP_TITLE,' ');

    if (descr_ptr->needs_device_flag)
        strcpy(svr_msg_ptr->state, LAUNCHSVRPROCBG_NEED_DEVICE_STR);

    else if (descr_ptr->needs_approval_flag)
        strcpy(svr_msg_ptr->state, LAUNCHSVRPROCBG_NEED_APPROVAL_STR);

    else 
        strcpy(svr_msg_ptr->state, LAUNCHSVRPROCBG_READY_TO_EXECUTE_STR);

    sprintf(svr_msg_ptr->priority,"%0*d",
            SIZE_PRIORITY, descr_ptr->priority_code);
            
    strcpy((char *)svr_msg_ptr->cmd_line, descr_ptr->prog_name_str);
    
    strcat(svr_msg_ptr->cmd_line," ");
    
    strncatWpad(svr_msg_ptr->cmd_line, descr_ptr->prog_args_str,
                SIZE_CMD_LINE, ' ');
                
    strncpyWpad(svr_msg_ptr->unix_owner,unix_owner_str,
                SIZE_LOGIN_NAME, ' ');
                
    strncpyWpad(svr_msg_ptr->job_descrip,descr_ptr->job_descrip_str,
                SIZE_JOB_DESCRIP, ' ');
                
    sprintf(svr_msg_ptr->seclvl, "%0*d",
            SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);
            
    sprintf(svr_msg_ptr->printer_seclvl,"%0*d",
            SIZE_SECURITY_CLASS, descr_ptr->seclvl);

	int lcl_prt_flag = 0;

    if (descr_ptr->prt_output_flag)
    {
        strncpyWpad(svr_msg_ptr->printer_name,prt_name_str,
                    SIZE_PRINTER_NAME, ' ');
                
        strncpyWpad(svr_msg_ptr->printer_host,prt_hostname_str,
                    SIZE_HOST_NAME, ' ');

        sprintf(svr_msg_ptr->printer_seclvl, "%0*d",
                SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);
            
        strncpyWpad(svr_msg_ptr->printer_file,
					descr_ptr->prt_filename_str,
                    NTCSS_SIZE_CLIENT_LOCATION, ' ');

		//////////////////////////////////////////////////////

		sprintf(svr_msg_ptr->orientation,"%0*d",
            SIZE_OF_AN_INT, descr_ptr->nOrient);

		svr_msg_ptr->banner=descr_ptr->bBanner?'1':'0';

		sprintf(svr_msg_ptr->copies,"%0*d",
            SIZE_OF_AN_INT, descr_ptr->nCopies);


		sprintf(svr_msg_ptr->papersize,"%0*d",
            SIZE_OF_AN_INT, descr_ptr->nPaperSize);

		//////////////////////////////////////////////////////

		lcl_prt_flag = PRINT_OUTPUT_FLAG;

		if (!descr_ptr->save_prt_output_flag)
			lcl_prt_flag |= DEL_AFTER_PRINT_FLAG;
    }
    else
    {
        memset(svr_msg_ptr->printer_name, ' ', SIZE_PRINTER_NAME);
        memset(svr_msg_ptr->printer_host, ' ', SIZE_HOST_NAME);
		memset(svr_msg_ptr->orientation, '0', SIZE_OF_AN_INT);
		memset(svr_msg_ptr->copies, '0', SIZE_OF_AN_INT);
		memset(svr_msg_ptr->papersize, '0', SIZE_OF_AN_INT);
		svr_msg_ptr->banner='0';
		

        sprintf(svr_msg_ptr->printer_seclvl, "%0*d",
                SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);

        memset(svr_msg_ptr->printer_file, ' ', SIZE_SERVER_LOCATION);
                
    }



									   // Note that prt_flag in message
									   //  definition deals with 
									   //  printing and restartability
	if (descr_ptr->restartable_flag)
		lcl_prt_flag |= RESTART_FLAG;

    sprintf(svr_msg_ptr->print_flag, "%0*d",
            SIZE_PRT_FLAG, lcl_prt_flag);
//start of sdi changes
	if(descr_ptr->sdi_required_flag)
	{
		svr_msg_ptr->sdi_required_flag='1';
		svr_msg_ptr->sdi_input_flag=descr_ptr->sdi_input_flag ? '1' : '0';
		strncpyWpad(svr_msg_ptr->sdi_control_record_id_str,
					descr_ptr->sdi_control_record_id_str,
                    NTCSS_SIZE_SDI_CONTROL_REC_ID+1, ' ');
		strncpyWpad(svr_msg_ptr->sdi_data_file_str,
					descr_ptr->sdi_data_file_str,
                    NTCSS_SIZE_SERVER_LOCATION, ' ');
		CString temp;
	
		strncpyWpad(svr_msg_ptr->sdi_tci_file_str,
					descr_ptr->sdi_tci_file_str,
                    NTCSS_SIZE_SERVER_LOCATION, ' ');
		temp.Format("TCI LENGTH IS ->%d",strlen(svr_msg_ptr->sdi_tci_file_str));
		

	}
	else
	{
		svr_msg_ptr->sdi_required_flag='0';
		svr_msg_ptr->sdi_input_flag=' ';
		memset(svr_msg_ptr->sdi_control_record_id_str, ' ', NTCSS_SIZE_SDI_CONTROL_REC_ID+1);
		memset(svr_msg_ptr->sdi_data_file_str, ' ', NTCSS_SIZE_SERVER_LOCATION);
		memset(svr_msg_ptr->sdi_tci_file_str, ' ', NTCSS_SIZE_SERVER_LOCATION);
	}
//end of sdi changes


    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    //SetMsgServerSelect(USE_APP_SERVER);			//jgj 9-2-97
	SetMsgServerSelect(USE_GIVEN_SERVER);  

	const SappGroupRec_wFullData *app_group_ptr = gbl_dll_man.dllUser()->appGroup
								(app_name_str);

	
	SetMsgServerName(app_group_ptr->host_name_buf); //jgj 9-2-97
    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_LAUNCHSVRPROCBG_MSGSTR);
    SetMsgBadMsg(BAD_LAUNCHSVRPROCBG_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Slaunchsvrprocbg_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_LAUNCHSVRPROCBG_RESPONSE);

    SetMsgState(READY_TO_GO);

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgLAUNCHSVRPROCBG::UnLoad
// ___________________________

BOOL CmsgLAUNCHSVRPROCBG::UnLoad(char *pid_buf)
{
	
    if (UnBundle("C",pid_buf))

        return(TRUE);
    else
        return(FALSE);
}



//***************************************************************************
// CmsgPRINTSVRFILE
// ___________________


/////////////////////////////////////////////////////////////////////////////
// CmsgPRINTSVRFILE constructor
// _______________________________

CmsgPRINTSVRFILE::CmsgPRINTSVRFILE()
{
    // No special processing for now
}

CmsgPRINTSVRFILE::~CmsgPRINTSVRFILE()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgPRINTSVRFILE::Load
// _________________________

BOOL CmsgPRINTSVRFILE::Load(LPCTSTR the_server,
                            LPCTSTR the_prt_file,
                            LPCTSTR the_prt_name,
                            LPCTSTR the_progroup_title, 
							const SecurityLevel the_security_class,
							LPCTSTR copies,
							LPCTSTR options)
{
    if ( (the_security_class < NTCSS_UNCLASSIFIED) ||
         (NTCSS_TOPSECRET < the_security_class) )
    {
        MsgErrorMan()->RecordError("Invalid Parameters");
        return(FALSE);
    }

                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sprintsvrfile_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_PRINTSVRFILE_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->prt_file,the_prt_file,
                SIZE_SERVER_LOCATION,' ');

    strncpyWpad(svr_msg_ptr->prt_name,the_prt_name,
                SIZE_PRINTER_NAME,' ');

    strncpyWpad(svr_msg_ptr->host_name,the_server,
                SIZE_HOST_NAME,' ');

    strncpyWpad(svr_msg_ptr->progroup_title,the_progroup_title,
                SIZE_GROUP_TITLE,' ');

    sprintf(svr_msg_ptr->security_class,"%0*d",
            SIZE_SECURITY_CLASS,(int)the_security_class);

	strncpyWpad(svr_msg_ptr->copies,copies,
                NTCSS_MAX_PRT_COPIES_LEN,' ');

	strncpyWpad(svr_msg_ptr->options,options,
                NTCSS_MAX_PRT_OPTIONS_LEN,' ');

    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgServerName(the_server);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_PRINTSVRFILE_MSGSTR);
    SetMsgBadMsg(BAD_PRINTSVRFILE_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sprintsvrfile_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_PRINTSVRFILE_RESPONSE);

    SetMsgState(READY_TO_GO);

    return(TRUE);
}


//***************************************************************************
// CmsgREGISTERAPPUSER
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgREGISTERAPPUSER constructor
// _______________________________

CmsgREGISTERAPPUSER::CmsgREGISTERAPPUSER()
{
    // No special processing for now
}


CmsgREGISTERAPPUSER::~CmsgREGISTERAPPUSER()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgREGISTERAPPUSER::Load
// _________________________

BOOL CmsgREGISTERAPPUSER::Load(LPCTSTR the_user_name, 
                               LPCTSTR the_app_name, 
                               const char the_flag)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sregisterappuser_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_REGISTERAPPUSER_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->username,the_user_name,
                SIZE_LOGIN_NAME,' ');

    strncpyWpad(svr_msg_ptr->appname,the_app_name,
                SIZE_GROUP_TITLE,' ');

    if (the_flag)
		svr_msg_ptr->flag = '1';
	else
		svr_msg_ptr->flag = '0';
            
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_AUTH_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_REGISTERAPPUSER_MSGSTR);
    SetMsgBadMsg(BAD_REGISTERAPPUSER_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sregisterappuser_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_REGISTERAPPUSER_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgREGISTERAPPUSER::showMsg
// ___________________________

void CmsgREGISTERAPPUSER::showMsg(CString *show_buf) 
{
    *show_buf = "   REGISTERAPPUSER message details not available";
}


#endif // _NTCSS_MSG_DEBUG_



//***************************************************************************
//***************************************************************************
// CmsgREMOVESPQITEM
// _________________



/////////////////////////////////////////////////////////////////////////////
// CmsgREMOVESPQITEM constructor
// _______________________________

CmsgREMOVESPQITEM::CmsgREMOVESPQITEM()
{
    // No special processing for now
}


CmsgREMOVESPQITEM::~CmsgREMOVESPQITEM()
{
	if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgREMOVESPQITEM::Load
// _________________________

BOOL CmsgREMOVESPQITEM::Load(LPCTSTR pid_buf,
                             const BOOL scheduled_job,
							 LPCTSTR program_group)

{
    if (MsgState() != RAW)

        EmptyMsg();
                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sremovespqitem_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header, DO_REMOVESPQITEM_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->pid,pid_buf,
                SIZE_PROCESS_ID,' ');

	
	strncpyWpad(svr_msg_ptr->program_group,program_group,
                SIZE_GROUP_TITLE,' ');
    
    if (scheduled_job)
        strncpyWpad(svr_msg_ptr->job_type, SCHEDULED_JOB_STR,
                    SIZE_JOB_TYPE, ' ');            
    else
        strncpyWpad(svr_msg_ptr->job_type, PRIORITY_JOB_STR,
                    SIZE_JOB_TYPE, ' ');            

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_APP_SERVER);
    SetMsgPortNum(NTCSS_PORT_SPQPORT);

    SetMsgOkayMsg(OK_REMOVESPQITEM_MSGSTR);
    SetMsgBadMsg(BAD_REMOVESPQITEM_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sremovespqitem_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_REMOVESPQITEM_RESPONSE);

    SetMsgState(READY_TO_GO);

    return(TRUE);
}



//***************************************************************************
// CmsgSCHEDSVRPROCBG
// ___________________


/////////////////////////////////////////////////////////////////////////////
// CmsgSCHEDSVRPROCBG constructor
// _______________________________

CmsgSCHEDSVRPROCBG::CmsgSCHEDSVRPROCBG()
{
    // No special processing for now
}


CmsgSCHEDSVRPROCBG::~CmsgSCHEDSVRPROCBG()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgSCHEDSVRPROCBG::Load
// _________________________

BOOL CmsgSCHEDSVRPROCBG::Load
             (LPCTSTR schedule,
  	  		  const NtcssServerProcessDescr *descr_ptr,
			  LPCTSTR app_name_str, 
			  LPCTSTR unix_owner_str,
			  LPCTSTR prt_name_str, 
			  LPCTSTR prt_hostname_str)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();
                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
                                       //
    svr_msg_ptr = new Sschedsvrprocbg_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_SCHEDSVRPROCBG_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,app_name_str,SIZE_GROUP_TITLE,' ');

    if (descr_ptr->needs_device_flag)
        strcpy(svr_msg_ptr->state, SCHEDSVRPROCBG_NEED_DEVICE_STR);

    else if (descr_ptr->needs_approval_flag)
        strcpy(svr_msg_ptr->state, SCHEDSVRPROCBG_NEED_APPROVAL_STR);

    else 
        strcpy(svr_msg_ptr->state, SCHEDSVRPROCBG_READY_TO_EXECUTE_STR);

    strncpyWpad(svr_msg_ptr->schedule,schedule,
                    SIZE_SCHEDULE, ' ');

    strcpy((char *)svr_msg_ptr->cmd_line,descr_ptr->prog_name_str);
    
    strcat(svr_msg_ptr->cmd_line," ");
    
    strncatWpad(svr_msg_ptr->cmd_line, descr_ptr->prog_args_str,
                SIZE_CMD_LINE, ' ');
                
    strncpyWpad(svr_msg_ptr->unix_owner,unix_owner_str,
                SIZE_LOGIN_NAME, ' ');
                
    strncpyWpad(svr_msg_ptr->job_descrip, descr_ptr->job_descrip_str,
                SIZE_JOB_DESCRIP, ' ');
                
    sprintf(svr_msg_ptr->seclvl, "%0*d",
            SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);
            
    sprintf(svr_msg_ptr->printer_seclvl,"%0*d",
            SIZE_SECURITY_CLASS, descr_ptr->seclvl);

	int lcl_prt_flag = 0;

    if (descr_ptr->prt_output_flag)
    {
		lcl_prt_flag = PRINT_OUTPUT_FLAG;

        strncpyWpad(svr_msg_ptr->printer_name,prt_name_str,
                    SIZE_PRINTER_NAME, ' ');
                
        strncpyWpad(svr_msg_ptr->printer_host,prt_hostname_str,
                    SIZE_HOST_NAME, ' ');

        sprintf(svr_msg_ptr->printer_seclvl, "%0*d",
                SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);

		sprintf(svr_msg_ptr->print_flag, "%0*d",
           SIZE_PRT_FLAG, lcl_prt_flag);

		////////////////////////////////////////////////////// 2/5/01

		sprintf(svr_msg_ptr->orientation,"%0*d",
            SIZE_OF_AN_INT, descr_ptr->nOrient);

		svr_msg_ptr->banner=descr_ptr->bBanner?'1':'0';

		sprintf(svr_msg_ptr->copies,"%0*d",
            SIZE_OF_AN_INT,descr_ptr->nCopies);

		sprintf(svr_msg_ptr->papersize,"%0*d",
            SIZE_OF_AN_INT,descr_ptr->nPaperSize);

		////////////////////////////////////////////////////// 2/5/01
            
        strncpyWpad(svr_msg_ptr->printer_file,
					descr_ptr->prt_filename_str,
                    NTCSS_SIZE_CLIENT_LOCATION, ' ');
		
		if (!descr_ptr->save_prt_output_flag)
			lcl_prt_flag |= DEL_AFTER_PRINT_FLAG;

    }
    else
    {
		
	/*	memset(svr_msg_ptr->printer_name, ' ', SIZE_PRINTER_NAME);
	memset(svr_msg_ptr->printer_host, ' ', SIZE_HOST_NAME);
	memset(svr_msg_ptr->orientation, '0', SIZE_OF_AN_INT);
	memset(svr_msg_ptr->copies, '0', SIZE_OF_AN_INT);
	memset(svr_msg_ptr->papersize, '0', SIZE_OF_AN_INT);
		svr_msg_ptr->banner='0'; */
		
		strncpyWpad(svr_msg_ptr->printer_name,"",
			SIZE_PRINTER_NAME, ' ');
		
        strncpyWpad(svr_msg_ptr->printer_host,"",
			SIZE_HOST_NAME, ' ');
		
        sprintf(svr_msg_ptr->printer_seclvl, "%0*d",
			SIZE_SECURITY_CLASS, (int)0);
		
		sprintf(svr_msg_ptr->print_flag, "%0*d",
			SIZE_PRT_FLAG, 0);
		
		//////////////////////////////////////////////////////
		//2/5/01
			
			sprintf(svr_msg_ptr->orientation,"%0*d",
            SIZE_OF_AN_INT, descr_ptr->nOrient);
		
		svr_msg_ptr->banner='0';
		
		sprintf(svr_msg_ptr->copies,"%0*d",
            SIZE_OF_AN_INT,0);
		
		sprintf(svr_msg_ptr->papersize,"%0*d",
            SIZE_OF_AN_INT,0);
		
		//////////////////////////////////////////////////////
		//2/5/01
            
			strncpyWpad(svr_msg_ptr->printer_file,"",
			SIZE_SERVER_LOCATION, ' ');
		
		
        sprintf(svr_msg_ptr->printer_seclvl, "%0*d",
			SIZE_SECURITY_CLASS, (int)descr_ptr->seclvl);
		
        memset(svr_msg_ptr->printer_file, ' ', SIZE_SERVER_LOCATION);
		
        
    }

  
   								       // Note that prt_flag in message
									   //  definition deals with 
									   //  printing and restartability
	if (descr_ptr->restartable_flag)
		lcl_prt_flag |= RESTART_FLAG;

  
	svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
  
    SetMsgServerSelect(USE_GIVEN_SERVER);
	SetMsgServerName(gbl_dll_man.dllUser()->GetAppServer(app_name_str));

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_SCHEDSVRPROCBG_MSGSTR);
    SetMsgBadMsg(BAD_SCHEDSVRPROCBG_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sschedsvrprocbg_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SCHEDSVRPROCBG_RESPONSE);

    SetMsgState(READY_TO_GO);
	
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgSCHEDSVRPROCBG::UnLoad
// ___________________________

BOOL CmsgSCHEDSVRPROCBG::UnLoad(char *pid_buf)
{
    if (UnBundle("C",pid_buf))

        return(TRUE);
    else
        return(FALSE);
}


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

CmsgSENDBBMSG::~CmsgSENDBBMSG()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG::Load
// ___________________

BOOL CmsgSENDBBMSG::Load(LPCTSTR msg_id, LPCTSTR bb_name)
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
                
    strncpyWpad(svr_msg_ptr->msg_id, msg_id,
                SIZE_MSGBOARD_KEY,' ');
                
    strncpyWpad(svr_msg_ptr->bb_name,bb_name,
                SIZE_MSGBOARD_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

		
										// Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_AUTH_SERVER);

    SetMsgPortNum(NTCSS_PORT_MSGBOARD);

    SetMsgOkayMsg(OK_SENDBBMSG_MSGSTR);

    SetMsgBadMsg(BAD_SENDBBMSG_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Ssendbbmsg_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SENDBBMSG_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



//***************************************************************************
// CmsgSETCOMMONDB
// _______________


/////////////////////////////////////////////////////////////////////////////
// CmsgSETCOMMONDB constructor
// _______________________________

CmsgSETCOMMONDB::CmsgSETCOMMONDB()
{
    // No special processing for now
}

CmsgSETCOMMONDB::~CmsgSETCOMMONDB()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}



/////////////////////////////////////////////////////////////////////////////
// CmsgSETCOMMONDB::Load
// _________________________

BOOL CmsgSETCOMMONDB::Load(LPCTSTR tag, LPCTSTR data)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Ssetcommondb_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_SETCOMMONDB_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->tag,tag,
                SIZE_COMMON_TAG,' ');

    strncpyWpad(svr_msg_ptr->data,data,
                SIZE_COMMON_DATA, ' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_CONNECTED_SERVER);  
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_SETCOMMONDB_MSGSTR);
    SetMsgBadMsg(BAD_SETCOMMONDB_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Ssetcommondb_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SETCOMMONDB_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


//***************************************************************************
// CmsgSETLINKDATA
// _______________


/////////////////////////////////////////////////////////////////////////////
// CmsgSETLINKDATA constructor
// _______________________________

CmsgSETLINKDATA::CmsgSETLINKDATA()
{
    // No special processing for now
}


CmsgSETLINKDATA::~CmsgSETLINKDATA()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgSETLINKDATA::Load
// _________________________

BOOL CmsgSETLINKDATA::Load(LPCTSTR appname, LPCTSTR link_data)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Ssetlinkdata_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_SETLINKDATA_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_APP_NAME,' ');

    strncpyWpad(svr_msg_ptr->link_data,link_data,
                SIZE_APP_PASSWORD, ' ');

    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    //SetMsgServerSelect(USE_CONNECTED_SERVER);  
	SetMsgServerSelect(USE_AUTH_SERVER); //4/23/99
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_SETLINKDATA_MSGSTR);
    SetMsgBadMsg(BAD_SETLINKDATA_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Ssetlinkdata_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SETLINKDATA_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


//***************************************************************************
// CmsgSVRPROCLIST
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgSVRPROCLIST constructor
// _______________________________

CmsgSVRPROCLIST::CmsgSVRPROCLIST()
{
    m_num_processes_cnt = -1;
}


CmsgSVRPROCLIST::~CmsgSVRPROCLIST()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgSVRPROCLIST::getProcessCnt
// ________________________________

BOOL CmsgSVRPROCLIST::getProcessCnt(int *num_processes_ptr)

{
    if (m_num_processes_cnt == -1)
    {
        BOOL ok = UnBundle("I",&m_num_processes_cnt);

        if (ok)
        {
            *num_processes_ptr = m_num_processes_cnt;
            return(TRUE);
        }
    }

    return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CmsgSVRPROCLIST::Load
// _________________________

BOOL CmsgSVRPROCLIST::Load (LPCTSTR appname,
                            const int  role,
                            const BOOL do_scheduled_jobs,
                            NtcssProcessFilter *filter)
{
    m_doing_scheduled_jobs = do_scheduled_jobs;

    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Ssvrproclist_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_SVRPROCLIST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_GROUP_TITLE,' ');
                                       // Note conversion from int to
                                       //  ASCII for role code
    svr_msg_ptr->role = (role + '0');            

    if (do_scheduled_jobs)
        svr_msg_ptr->is_schedjobs = '1';
    else
        svr_msg_ptr->is_schedjobs = '0';

    int mask_val = 0;


	mask_val |= SVRPROCLIST_FILTER_VAL_CUST_PROC_STAT;		//1
	mask_val |= SVRPROCLIST_FILTER_VAL_PRINT_INFO;			//2
	mask_val |= SVRPROCLIST_FILTER_VAL_JOB_DESCR_INFO;		//4

    if (filter->get_for_current_user_only)
        mask_val |= SVRPROCLIST_FILTER_VAL_USER_ONLY_INFO;	//8

    if (filter->get_only_running_jobs)
        mask_val |= SVRPROCLIST_FILTER_VAL_RUN_PROCS_INFO;	//16

	if (filter->priority_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_PRIORITY;		//64

	if (filter->command_line_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_COMMAND_LINE;	//128

	if (filter->cust_proc_status_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_CUST_PROC_STATUS;//256

	if (filter->login_name_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_LOGIN_NAME;		//512

	if (filter->unix_owner_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_UNIX_OWNER;		//1024

	if (filter->progroup_title_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_PROGROUP_TITLE;	//2048

	if (filter->job_descrip_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_JOB_DESCRIPTION;	//4096

	if (filter->orig_host_flag)
        mask_val |= SVRPROCLIST_FILTER_VAL_ORIG_HOST;		//8192


    sprintf(svr_msg_ptr->filter_mask,"%0*d",
            SIZE_SVRPROCLIST_FILTER_MASK, mask_val);

//	AfxMessageBox("Mask-> " +(CString)svr_msg_ptr->filter_mask);


	//svr_msg_ptr->priority = (filter->priority + '0'); 
	CString temp;
	temp.Format("%d",filter->priority);

	strncpyWpad(svr_msg_ptr->priority,temp.GetBufferSetLength(SIZE_PRIORITY),
                SIZE_PRIORITY,' ');

	             
    strncpyWpad(svr_msg_ptr->command_line,filter->command_line,
                NTCSS_SIZE_COMMAND_LINE,' ');

	strncpyWpad(svr_msg_ptr->cust_proc_status,filter->cust_proc_status,
                NTCSS_SIZE_CUSTOM_PROC_STAT,' ');

	strncpyWpad(svr_msg_ptr->login_name,filter->login_name,
                NTCSS_SIZE_LOGINNAME,' ');

	strncpyWpad(svr_msg_ptr->unix_owner,filter->unix_owner,
                NTCSS_SIZE_LOGINNAME,' ');

	strncpyWpad(svr_msg_ptr->progroup_title,filter->progroup_title,
                NTCSS_SIZE_APP_GROUP,' ');

	strncpyWpad(svr_msg_ptr->job_descrip,filter->job_descrip,
                NTCSS_SIZE_JOB_DESCRIP,' ');

	strncpyWpad(svr_msg_ptr->orig_host,filter->orig_host,
                NTCSS_SIZE_ORIG_HOST,' ');

	

    svr_msg_ptr->null_tail = '\0';

	
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_APP_SERVER);
    SetMsgPortNum(NTCSS_PORT_SPQPORT);

    SetMsgOkayMsg(OK_SVRPROCLIST_MSGSTR);
    SetMsgBadMsg(BAD_SVRPROCLIST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Ssvrproclist_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SVRPROCLIST_RESPONSE);

    SetMsgState(READY_TO_GO);
      
    return(TRUE);
}                          



/////////////////////////////////////////////////////////////////////////////
// CmsgSVRPROCLIST::UnLoad
// _______________________

BOOL CmsgSVRPROCLIST::UnLoad(NtcssServerProcessDetails *svr_proc_list_ptr,
                             int   svr_proc_list_slots,
                             int  *num_put_in_svr_proc_list)
{
    BOOL ok = TRUE;

    if (m_num_processes_cnt == -1)
    {
        ok = UnBundle("I",&m_num_processes_cnt);

        if (!ok)
            return(FALSE);
    }

    if (ok)
    {
        *num_put_in_svr_proc_list = 0;

        for (int i = 0; ((i < m_num_processes_cnt) &&
                         (i < svr_proc_list_slots)); i++)
        {
            if (m_doing_scheduled_jobs)
                svr_proc_list_ptr->job_type_code = NTCSS_SCHEDULED_JOB;
            else
                svr_proc_list_ptr->job_type_code = NTCSS_NON_SCHEDULED_JOB;
            
			int prt_flag;                
            ok = UnBundle("ICICCICCCCCCCICCCICICCCCIIII",
                          &(svr_proc_list_ptr->pid),
                          svr_proc_list_ptr->process_id_str,
                          &(svr_proc_list_ptr->priority_code),
                          svr_proc_list_ptr->app_name_str,
                          svr_proc_list_ptr->command_line_str,
                          &(svr_proc_list_ptr->status_code),
                          svr_proc_list_ptr->custom_process_stat_str,
                          svr_proc_list_ptr->login_name_str,
                          svr_proc_list_ptr->unix_owner_str,
                          svr_proc_list_ptr->job_descrip_str,
                          svr_proc_list_ptr->request_time,
                          svr_proc_list_ptr->launch_time,
                          svr_proc_list_ptr->end_time,
                          &(svr_proc_list_ptr->seclvl),
                          svr_proc_list_ptr->orig_host_str,
                          svr_proc_list_ptr->printer_name_str,
                          svr_proc_list_ptr->printer_host_name_str,
                          &(svr_proc_list_ptr->print_seclvl),
                          svr_proc_list_ptr->prt_filename_str,
                          &(prt_flag),
						  svr_proc_list_ptr->sdi_control_record_id_str,
						  svr_proc_list_ptr->sdi_data_file_str,
						  svr_proc_list_ptr->sdi_tci_file_str,
						  svr_proc_list_ptr->sdi_device_data_file_str,
						  &(svr_proc_list_ptr->orientation),
						  &(svr_proc_list_ptr->banner),
						  &(svr_proc_list_ptr->copies),
	                      &(svr_proc_list_ptr->PaperSize));
      
		
			
			if (!ok)
                break;
									   // The print_code is performing
									   //  double duty and also the
									   //  print_code values differ
									   //  from previous versions of
									   //  the SVRPROCLIST.  Here am
									   //  trying to hide this from
									   //  the callers:

			if (prt_flag & PRINT_OUTPUT_FLAG)
			{
				if (prt_flag & DEL_AFTER_PRINT_FLAG) 

					svr_proc_list_ptr->print_code = NTCSS_PRINT_AND_REMOVE;

				else

					svr_proc_list_ptr->print_code = NTCSS_PRINT_AND_SAVE;
			}
			else
				svr_proc_list_ptr->print_code = NTCSS_PRINT_NOT;


			if (prt_flag & RESTART_FLAG)

				svr_proc_list_ptr->restartable_flag = TRUE;

			else

				svr_proc_list_ptr->restartable_flag = FALSE;

									   // Now bump up to next list item
            svr_proc_list_ptr++;
            (*num_put_in_svr_proc_list)++;
        }
    }

    return(ok);
}

//***************************************************************************
// CmsgVALIDATEUSER
// _______________


/////////////////////////////////////////////////////////////////////////////
// CmsgVALIDATEUSER constructor
// _______________________________

CmsgVALIDATEUSER::CmsgVALIDATEUSER()
{
    // No special processing for now
}

CmsgVALIDATEUSER::~CmsgVALIDATEUSER()
{
    if(svr_msg_ptr)
		delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgVALIDATEUSER::Load
// _________________________

BOOL CmsgVALIDATEUSER::Load(LPCTSTR username, LPCTSTR password, LPCTSTR appname)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Svalidateuser_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_VALIDATEUSER_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
                
    strncpyWpad(svr_msg_ptr->username,username,
                SIZE_LOGIN_NAME,' ');

    strncpyWpad(svr_msg_ptr->password,password,
                SIZE_APP_PASSWORD*2, ' ');

	svr_msg_ptr->password[strlen(password)]=':';


	strncpyWpad(svr_msg_ptr->appname,appname,
                SIZE_APP_NAME,' '); 

    


    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
									   //
	

    SetMsgServerSelect(USE_AUTH_SERVER);  
    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_VALIDATEUSER_MSGSTR);
    SetMsgBadMsg(BAD_VALIDATEUSER_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Svalidateuser_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_VALIDATEUSER_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CmsgVALIDATEUSER::UnLoad
// ___________________________

BOOL CmsgVALIDATEUSER::UnLoad(char *ssn)
{
    if (UnBundle("C",ssn))
		return(TRUE);

    else
	   return(FALSE);
}









