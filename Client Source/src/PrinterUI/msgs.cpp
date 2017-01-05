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
// CmsgGETPRINTERINI
// _____________


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI constructor
// _________________________

CmsgGETPRINTERINI::CmsgGETPRINTERINI()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI destructor
// _________________________


CmsgGETPRINTERINI::~CmsgGETPRINTERINI()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI::Load
// ___________________

BOOL CmsgGETPRINTERINI::Load()
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
	svr_msg_ptr = new Sgetprinterini_svrmsgrec;

#define  DO_GETPRINTERINI_MSGSTR    "GETPRINTERINI"
#define  OK_GETPRINTERINI_MSGSTR    "GOODGETPRINTERINI"
#define  BAD_GETPRINTERINI_MSGSTR   "NOGETPRINTERINI"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_GETPRINTERINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

		         
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_MASTER_SERVER);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETPRINTERINI_MSGSTR);

    SetMsgBadMsg(BAD_GETPRINTERINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetprinterini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETPRINTERINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI::UnLoad
// _____________________

BOOL CmsgGETPRINTERINI::UnLoad(LPTSTR ini_file_name_path)
{
    return(UnBundle("C", ini_file_name_path));
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI::ShowMsg
// ___________________________

void CmsgGETPRINTERINI::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETPRINTERINI message details not available";
}


#endif // _NTCSS_MSG_DEBUG_







//***************************************************************************
//***************************************************************************
// CmsgGETHOSTINFO
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO constructor
// _________________________

CmsgGETHOSTINFO::CmsgGETHOSTINFO()
{
    // No special processing for now
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO destructor
// _________________________

CmsgGETHOSTINFO::~CmsgGETHOSTINFO()
{

	delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO::Load
// ___________________

BOOL CmsgGETHOSTINFO::Load(LPTSTR host_name,LPTSTR ip_address)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgethostinfo_svrmsgrec;

	#define  DO_GETHOSTINFO_MSGSTR    "GETHOSTINFO"
	#define  OK_GETHOSTINFO_MSGSTR    "GOODGETHOSTINFO"
	#define  BAD_GETHOSTINFO_MSGSTR   "NOGETHOSTINFO"


    strncpyWpad(svr_msg_ptr->bsc.header, DO_GETHOSTINFO_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->host_name, host_name,
                SIZE_HOST_NAME,' ');

	strncpyWpad(svr_msg_ptr->ip_address, ip_address,
                SIZE_IP_ADDR*2,' ');

	svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
	                                   //
    SetMsgServerSelect(USE_MASTER_SERVER);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETHOSTINFO_MSGSTR);

    SetMsgBadMsg(BAD_GETHOSTINFO_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgethostinfo_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETHOSTINFO_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO::UnLoad
// _____________________

BOOL CmsgGETHOSTINFO::UnLoad(LPTSTR host_name,LPTSTR ip_address)
{
    return(UnBundle("CC", host_name,ip_address));
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO::ShowMsg
// ___________________________

void CmsgGETHOSTINFO::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETHOSTINFO message details not available";
}

#endif // _NTCSS_MSG_DEBUG_



//***************************************************************************
// CmsgGETPRTSTATUS
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS constructor
// _________________________

CmsgGETPRTSTATUS::CmsgGETPRTSTATUS()
{
    // No special processing for now
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS destructor
// _________________________

CmsgGETPRTSTATUS::~CmsgGETPRTSTATUS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS::Load
// ___________________

BOOL CmsgGETPRTSTATUS::Load(LPCTSTR printer_name,LPCTSTR host_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
    svr_msg_ptr = new Sgetprtstatus_svrmsgrec;

#define  DO_GETPRTSTATUS_MSGSTR    "GETPRTSTATUS"
#define  OK_GETPRTSTATUS_MSGSTR    "GOODGETPRTSTATUS"
#define  BAD_GETPRTSTATUS_MSGSTR   "NOGETPRTSTATUS"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_GETPRTSTATUS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->printer_name, printer_name,
                SIZE_PRINTER_NAME,' ');
	
	svr_msg_ptr->null_tail = '\0';
                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_GIVEN_SERVER);

//	AfxMessageBox(host_name);

	SetMsgServerName(host_name);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_GETPRTSTATUS_MSGSTR);

    SetMsgBadMsg(BAD_GETPRTSTATUS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sgetprtstatus_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_GETPRTSTATUS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS::UnLoad
// _____________________

BOOL CmsgGETPRTSTATUS::UnLoad(int *printer_status,int *queue_status)
{

    BOOL ok = UnBundle("II", printer_status, queue_status);
	return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS::ShowMsg
// ___________________________

void CmsgGETPRTSTATUS::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgGETPRTSTATUS message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


//***************************************************************************
//***************************************************************************
// CmsgNEWNTPRINTER
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgNEWNTPRINTER constructor
// _________________________

CmsgNEWNTPRINTER::CmsgNEWNTPRINTER()
{
    // No special processing for now
}



/////////////////////////////////////////////////////////////////////////////
// CmsgNEWNTPRINTER destructor
// _________________________

CmsgNEWNTPRINTER::~CmsgNEWNTPRINTER()
{

	delete svr_msg_ptr;

}





/////////////////////////////////////////////////////////////////////////////
// CmsgNEWNTPRINTER::Load
// ___________________

BOOL CmsgNEWNTPRINTER::Load(LPCTSTR drivername,LPCTSTR driverfile,LPCTSTR uifile,
		LPCTSTR datafile,LPCTSTR helpfile,LPCTSTR monitorfile,LPCTSTR monitorstring,
		LPCTSTR copyfiles)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg
   svr_msg_ptr = new Snewntprinter_svrmsgrec;

#define  DO_NEWNTPRINTER_MSGSTR    "NEWNTPRINTER"
#define  OK_NEWNTPRINTER_MSGSTR    "GOODNEWNTPRINTER"
#define  BAD_NEWNTPRINTER_MSGSTR   "NONEWNTPRINTER"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_NEWNTPRINTER_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->drivername,drivername,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->driverfile,driverfile,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->uifile,uifile,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->datafile,datafile,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->helpfile,helpfile,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->monitorfile,monitorfile,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->monitorstring,monitorstring,
                SIZE_PRT_DEV_NAME ,' ');

	strncpyWpad(svr_msg_ptr->copyfiles,copyfiles,
                SIZE_COPY_FILES ,' ');

	         
    svr_msg_ptr->null_tail = '\0';

/*
#ifdef _DEBUG
	AfxMessageBox(svr_msg_ptr->drivername); //debug 1/15/02
	AfxMessageBox(svr_msg_ptr->copyfiles);  //debug 1/15/02
#endif
*/

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_MASTER_SERVER);

    SetMsgPortNum(NTCSS_PORT_DBPORT);

    SetMsgOkayMsg(OK_NEWNTPRINTER_MSGSTR);

    SetMsgBadMsg(BAD_NEWNTPRINTER_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Snewntprinter_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_NEWNTPRINTER_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}




#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgNEWNTPRINTER::ShowMsg
// ___________________________

void CmsgNEWNTPRINTER::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgNEWNTPRINTER message details not available";
}


#endif // _NTCSS_MSG_DEBUG_






//***************************************************************************
//***************************************************************************
// CmsgPUTPRINTERINI
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPRINTERINI constructor
// _________________________

CmsgPUTPRINTERINI::CmsgPUTPRINTERINI()
{
    // No special processing for now
}

 CmsgPUTPRINTERINI::~CmsgPUTPRINTERINI()
{

	delete svr_msg_ptr;

}

/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPRINTERINI::Load
// ___________________

BOOL CmsgPUTPRINTERINI::Load(LPCTSTR file_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Sputprinterini_svrmsgrec;

#define  DO_PUTPRINTERINI_MSGSTR    "PUTPRINTERINI"
#define  OK_PUTPRINTERINI_MSGSTR    "GOODPUTPRINTERINI"
#define  BAD_PUTPRINTERINI_MSGSTR   "NOPUTPRINTERINI"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_PUTPRINTERINI_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->file_name, file_name,
                SIZE_SERVER_FILE_NAME,' ');
                
    svr_msg_ptr->null_tail = '\0';

                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_MASTER_SERVER);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

    SetMsgOkayMsg(OK_PUTPRINTERINI_MSGSTR);

    SetMsgBadMsg(BAD_PUTPRINTERINI_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Sputprinterini_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_PUTPRINTERINI_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPRINTERINI::UnLoad
// _____________________

BOOL CmsgPUTPRINTERINI::UnLoad(char *log_file_name_path)
{
//AfxMessageBox("getting ready to UnBundle");
    BOOL ok = UnBundle("C", log_file_name_path);
//AfxMessageBox("back from UnBundle");
    return(ok);
}



#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPRINTERINI::ShowMsg
// ___________________________

void CmsgPUTPRINTERINI::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgPUTPRINTERINI message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


//***************************************************************************
//***************************************************************************
// CmsgSETPRTSTATUS
// _____________



/////////////////////////////////////////////////////////////////////////////
// CmsgSETPRTSTATUS constructor
// _________________________

CmsgSETPRTSTATUS::CmsgSETPRTSTATUS()
{
    // No special processing for now
}

CmsgSETPRTSTATUS::~CmsgSETPRTSTATUS()
{
	delete svr_msg_ptr;
}

/////////////////////////////////////////////////////////////////////////////
// CmsgSETPRTSTATUS::Load
// ___________________

BOOL CmsgSETPRTSTATUS::Load(LPCTSTR printer_name,LPCTSTR printer_enable_status,LPCTSTR queue_enable_status,LPCTSTR host_name)
{
                                       // Verify is ok to load
    if (MsgState() != RAW)

        EmptyMsg();

                                       // First setup record going to
                                       //   the server and fill in
                                       //   info specific to this msg

    svr_msg_ptr = new Ssetprtstatus_svrmsgrec;

#define DO_SETPRTSTATUS_MSGSTR "SETPRTSTATUS"
#define OK_SETPRTSTATUS_MSGSTR "GOODSETPRTSTATUS"
#define BAD_SETPRTSTATUS_MSGSTR "NOSETPRTSTATUS"

    strncpyWpad(svr_msg_ptr->bsc.header, DO_SETPRTSTATUS_MSGSTR,
                SIZE_MESSAGE_TYPE,'\0');

	strncpyWpad(svr_msg_ptr->printer_name, printer_name,
                SIZE_PRINTER_NAME,' ');

	strncpyWpad(svr_msg_ptr->printer_enable_status, printer_enable_status,
                SIZE_PRT_FLAG-1,' ');

	strncpyWpad(svr_msg_ptr->queue_enable_status, queue_enable_status,
                SIZE_PRT_FLAG-1,' ');

         
    svr_msg_ptr->null_tail = '\0';


                                       // Now fixup msg so dll will
                                       //  be able to process
                                       //
    SetMsgServerSelect(USE_GIVEN_SERVER);

    SetMsgPortNum(NTCSS_PORT_CMDPORT);

	SetMsgServerName(host_name);

    SetMsgOkayMsg(OK_SETPRTSTATUS_MSGSTR);

    SetMsgBadMsg(BAD_SETPRTSTATUS_MSGSTR);

    SetMsgSendDataPtr((char *)svr_msg_ptr);

    SetMsgSendDataSize(sizeof(Ssetprtstatus_svrmsgrec));

    SetMsgRecvDataChainSize(SIZE_SETPRTSTATUS_RESPONSE);

    SetMsgState(READY_TO_GO);
    
    return(TRUE);
}




#ifdef _NTCSS_MSG_DEBUG_

/////////////////////////////////////////////////////////////////////////////
// CmsgSETPRTSTATUS::ShowMsg
// ___________________________

void CmsgSETPRTSTATUS::ShowMsg(CString *show_buf) 
{
    *show_buf = "   CmsgSETPRTSTATUS message details not available";
}


#endif // _NTCSS_MSG_DEBUG_


//***************************************************************************
//***************************************************************************
// CmsgGETDIRLISTING
// ___________________



/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING constructor
// _______________________________

CmsgGETDIRLISTING::CmsgGETDIRLISTING()
{
   // m_num_files_cnt = -1;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING destructor
// _______________________________

CmsgGETDIRLISTING::~CmsgGETDIRLISTING()
{

	delete svr_msg_ptr;
}


/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING::Load
// _________________________

BOOL CmsgGETDIRLISTING::Load(LPCTSTR server_name,LPCTSTR dir_name)
{

//	if(strlen(server_name)==0 || strlen(dir_name)==0)
//		return FALSE:

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
		SetMsgServerSelect(USE_APP_SERVER); //probably bad

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



