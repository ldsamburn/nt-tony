/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "ntcssapi.h"
#include "pqmsgs.h"

//#include "Dll_defs.h"
//***************************************************************************
//***************************************************************************
// CmsgGETPRTLIST
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgCmsgGETPDJINI constructor
// _______________________________

CmsgGETPRTLIST::CmsgGETPRTLIST()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI destructor
// _______________________________

CmsgGETPRTLIST::~CmsgGETPRTLIST()
{
	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTLIST::Load
// _________________________

BOOL CmsgGETPRTLIST::Load(char *progroup, char access, char *servername)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

	m_ListCount=-1;

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
   svr_msg_ptr = new Sgetprtlist_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_GETPRTLIST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->progroup_title,progroup,
                SIZE_GROUP_TITLE,' ');
	svr_msg_ptr->printer_access=access;
                
  
    svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
	
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_DBPORT);

	SetMsgServerName(servername);

    SetMsgOkayMsg(OK_GETPRTLIST_MSGSTR);
    SetMsgBadMsg(BAD_GETPRTLIST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Sgetprtlist_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETPRTLIST_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTLIST::UnLoad
// _______________________

BOOL CmsgGETPRTLIST::UnLoad(int *num_printers,
							NtcssPrinterList *printer_list)
{
	BOOL ok = TRUE;

	if (m_ListCount == -1) {
		m_ListCount = UnLoadCount();
		if (m_ListCount == -1) {
			*num_printers = 0;
			return(FALSE);
		}
	}

	for (int i=0;i<m_ListCount;i++) {
		ok = UnBundle("CCCCIICCCCCCCCC",
		      printer_list[i].prt_name,
		      printer_list[i].hostname,
		      printer_list[i].location,
		      printer_list[i].device_name,
		      &printer_list[i].que_enabled,
		      &printer_list[i].printer_enabled,
		      printer_list[i].redirect_printer,
		      printer_list[i].driver_file,
		      printer_list[i].ui_file,
		      printer_list[i].data_file,
		      printer_list[i].help_file, 
			  printer_list[i].monitor_file,
			  printer_list[i].monitor_name,
			  printer_list[i].copy_files,
			  printer_list[i].filename);

		printer_list[i].prt_namelen=strlen(printer_list[i].prt_name);

		*num_printers++;

	}

	return(ok);

}


int CmsgGETPRTLIST::UnLoadCount()
{
	BOOL ok=TRUE;

	int num_printers;

	ok=UnBundle("I", &num_printers);

	if (!ok)
		return(-1);

	m_ListCount=num_printers;

	return(num_printers);

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Release Message

/////////////////////////////////////////////////////////////////////////////
// CmsgRelRequest constructor
// _______________________________

CmsgRelRequest::CmsgRelRequest()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgRelRequest destructor
// _______________________________

CmsgRelRequest::~CmsgRelRequest()
{
	delete svr_msg_ptr;
}
/////////////////////////////////////////////////////////////////////////////
// CmsgRelRequest::Load
// _________________________

BOOL CmsgRelRequest::Load(char *prname, char *request, char *servername)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)
        EmptyMsg();

    svr_msg_ptr = new Srelrequest_svrmsgrec;

    strncpyWpad(svr_msg_ptr->bsc.header,DO_RELREQUEST_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');
    strncpyWpad(svr_msg_ptr->prt_name,prname,
                SIZE_PRINTER_NAME,' ');
    strncpyWpad(svr_msg_ptr->request_id,request,
                SIZE_PRT_REQ_ID,' ');
        
  
    svr_msg_ptr->null_tail = '\0';
 								
    SetMsgServerSelect(USE_GIVEN_SERVER);
    SetMsgPortNum(NTCSS_PORT_PQPORT);

    SetMsgOkayMsg(OK_RELREQUEST_MSGSTR);
    SetMsgBadMsg(BAD_RELREQUEST_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);
    SetMsgSendDataSize(sizeof(Srelrequest_svrmsgrec));
	SetMsgServerName(servername);


    SetMsgRecvDataChainSize(SIZE_RELREQUEST_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgRelRequest::UnLoad
// _______________________

BOOL CmsgRelRequest::UnLoad()
{
	BOOL ok = TRUE;

	return(ok);

}








