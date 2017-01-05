/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif



#include "ntcssmsg.h"       
#include "privapis.h"
#include "msgdefs.h"


typedef struct  {
    	char prt_name[SIZE_PRINTER_NAME];
    	int prt_namelen;
    	char hostname[SIZE_HOST_NAME];
    	char location[SIZE_PRT_LOCATION];
    	char device_name[SIZE_PRT_DEV_NAME];
    	int que_enabled;
    	int printer_enabled;
    	char redirect_printer[SIZE_PRT_RED];
		char driver_file[SIZE_PRT_FILE_NAME];
		char ui_file[SIZE_PRT_FILE_NAME];
		char data_file[SIZE_PRT_FILE_NAME];
		char help_file[SIZE_PRT_FILE_NAME];
		char monitor_file[SIZE_PRT_FILE_NAME];
		char monitor_name[NTCSS_SIZE_SERVER_LOCATION];
		char copy_files[SIZE_TRANSPORT_GENBUF];
		char filename[SIZE_PRT_FILE_NAME];
} NtcssPrinterList;
 


                                       // ***
                                       // >>> GETPRTLIST stuff
                                       //
#define  DO_GETPRTLIST_MSGSTR    "GETPRTLIST"
#define  OK_GETPRTLIST_MSGSTR    "GOODPRTLIST"
#define  BAD_GETPRTLIST_MSGSTR   "NOPRTLIST"

                                        //
#define  SIZE_GETPRTLIST_RESPONSE     8192

typedef struct Sgetprtlist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
   	char progroup_title[SIZE_GROUP_TITLE];
   	char printer_access;
    char null_tail;

} Sgetprtlist_svrmsgrec;

////////////////////////////////////////////////////////////////////////////////
 

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTLIST
// ______________

class CmsgGETPRTLIST : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgGETPRTLIST();
    
    ~CmsgGETPRTLIST();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL Load(char *progroup, char access, char *servername);

	BOOL UnLoad(int *num_printers,
				NtcssPrinterList *printer_list);

	int UnLoadCount();

protected:

	 Sgetprtlist_svrmsgrec *svr_msg_ptr;

	 int m_ListCount;

};




/*
Release message 
*/


#define  DO_RELREQUEST_MSGSTR    "RELPRTREQ"
#define  OK_RELREQUEST_MSGSTR    "GOODRELPRTREQ"
#define  BAD_RELREQUEST_MSGSTR   "NORELPRTREQ"

                                        //
#define  SIZE_RELREQUEST_RESPONSE     200

typedef struct Srelrequest_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
   	char prt_name[SIZE_PRINTER_NAME];
    char request_id[SIZE_PRT_REQ_ID];
    char null_tail;

} Srelrequest_svrmsgrec;


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTLIST
// ______________

class CmsgRelRequest : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgRelRequest();
    
    ~CmsgRelRequest();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL Load(char *prname, char *request, char *servername);

	BOOL UnLoad();

protected:

	 Srelrequest_svrmsgrec *svr_msg_ptr;

};
