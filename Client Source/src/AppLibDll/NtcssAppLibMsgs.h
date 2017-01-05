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

                                       // ***
                                       // >>> REMOTEVALIDATE stuff
                                       //
#define  DO_REMOTEVALIDATE_MSGSTR    "REMOTEVALIDATE"
#define  OK_REMOTEVALIDATE_MSGSTR    "GOODREMOTEVALIDATE"
#define  BAD_REMOTEVALIDATE_MSGSTR   "NOREMOTEVALIDATET"

                                        //
#define  SIZE_REMOTEVALIDATE_RESPONSE     8192

typedef struct SREMOTEVALIDATE_svrmsgrec 
{
//    Sbasic_svrmsgrec    bsc;
	char message[SIZE_MESSAGE_TYPE];
	char loginname[SIZE_LOGIN_NAME];
	char password[SIZE_PASSWORD];
   	char progroup_title[SIZE_GROUP_TITLE];
    char null_tail;

} SREMOTEVALIDATE_svrmsgrec;

////////////////////////////////////////////////////////////////////////////////
 

/////////////////////////////////////////////////////////////////////////////
// CmsgREMOTEVALIDATE
// ______________

class CmsgREMOTEVALIDATE : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgREMOTEVALIDATE();
    
    ~CmsgREMOTEVALIDATE();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL Load(char *logname, char *password, 
		      char *progroup, char *hostname);

	BOOL UnLoad(char *dbpassword);

	int UnLoadCount();

protected:

	 SREMOTEVALIDATE_svrmsgrec *svr_msg_ptr;


};





