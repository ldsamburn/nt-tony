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

#ifndef _INCLUDE_MSGS_H_
#define _INCLUDE_MSGS_H_

       
#include "privapis.h"
#include "ntcssmsg.h"
#include "msgdefs.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CmsgGETOTYPEINI
// ______________

class CmsgGETOTYPEINI : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgGETOTYPEINI();
    
    ~CmsgGETOTYPEINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(CString strHostname);

    BOOL UnLoad(char *ini_file_name_path);

protected:

	Sgetotypeini_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);#include "stdafx.h"



#endif
};

/////////////////////////////////////////////////////////////////////////////
// CmsgPUTOTYPEINI
// ______________

class CmsgPUTOTYPEINI : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgPUTOTYPEINI();
    
    ~CmsgPUTOTYPEINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(const char *file_name,CString strHostname);

    BOOL UnLoad(char *log_file_name_path);

protected:
	Sputotypeini_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};

#endif
