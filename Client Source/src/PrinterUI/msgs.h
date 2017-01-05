/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// 
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_MSGS_H_
#define _INCLUDE_MSGS_H_

#include "ntcssmsg.h"       
#include "privapis.h"
#include "msgdefs.h"


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRINTERINI
// ______________

class CmsgGETPRINTERINI : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgGETPRINTERINI();
    
    ~CmsgGETPRINTERINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load();

    BOOL UnLoad(LPTSTR ini_file_name_path);

protected:

	Sgetprinterini_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETHOSTINFO
// ______________

class CmsgGETHOSTINFO : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgGETHOSTINFO();
    
    ~CmsgGETHOSTINFO();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPTSTR host_name,LPTSTR ip_address);

    BOOL UnLoad(LPTSTR host_name,LPTSTR ip_address);

protected:

	Sgethostinfo_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CmsgGETPRTSTATUS
// ______________

class CmsgGETPRTSTATUS : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgGETPRTSTATUS();
    
    ~CmsgGETPRTSTATUS();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR printer_name,LPCTSTR host_name);

    BOOL UnLoad(int *printer_status,int *queue_status);

protected:

	Sgetprtstatus_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgNEWNTPRINTER
// ______________

class CmsgNEWNTPRINTER : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgNEWNTPRINTER();
    
    ~CmsgNEWNTPRINTER();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR drivername,LPCTSTR driverfile,LPCTSTR uifile,
			LPCTSTR datafile,LPCTSTR helpfile,LPCTSTR monitorfile,
			LPCTSTR monitorstring,LPCTSTR copyfiles);

protected:

	Snewntprinter_svrmsgrec *svr_msg_ptr;
 
#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};
/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPRINTERINI
// ______________

class CmsgPUTPRINTERINI : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgPUTPRINTERINI();
    
    ~CmsgPUTPRINTERINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR file_name);

    BOOL UnLoad(char *log_file_name_path);

protected:
	 Sputprinterini_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CmsgSETPRTSTATUS
// ______________

class CmsgSETPRTSTATUS : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgSETPRTSTATUS();
    
    ~CmsgSETPRTSTATUS();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR printer_name,LPCTSTR printer_enable_status,LPCTSTR queue_enable_status,
		      LPCTSTR host_name);

protected:
	Ssetprtstatus_svrmsgrec *svr_msg_ptr;

    
#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING
// ___________________

class CmsgGETDIRLISTING : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgGETDIRLISTING();
    
    ~CmsgGETDIRLISTING();
    
    BOOL Load(LPCTSTR server_name,
              LPCTSTR dir_name);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

protected:
	 Sgetdirlisting_svrmsgrec *svr_msg_ptr;

  
};

#endif  


