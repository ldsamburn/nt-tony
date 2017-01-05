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
                                       // >>> GETPDJINIMSGFILE stuff
                                       //
#define  DO_GETPDJINI_MSGSTR    "GETPDJINI"
#define  OK_GETPDJINI_MSGSTR    "GOODPDJINI"
#define  BAD_GETPDJINI_MSGSTR   "NOPDJINI"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODGETPDJINI (or
                                        //  NOGETPDJINI
                                        //
#define  SIZE_GETPDJINI_RESPONSE     256

typedef struct Sgetpdjini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetpdjini_svrmsgrec;

////////////////////////////////////////////////////////////////////////////////
                                      // ***
                                       // >>> PUTPDJINIMSGFILE stuff
                                       //
#define  DO_PUTPDJINI_MSGSTR    "PUTPDJINI"
#define  OK_PUTPDJINI_MSGSTR    "GOODPDJINI"
#define  BAD_PUTPDJINI_MSGSTR   "NOPDJINI"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODGETPDJINI (or
                                        //  NOGETPDJINI
                                        //
#define  SIZE_PUTPDJINI_RESPONSE     256

typedef struct Sputpdjini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				filename[NTCSS_SIZE_COMMAND_LINE];
    char                null_tail;

} Sputpdjini_svrmsgrec;


/////////////////////////////////////////////////////////////////////////////
// CmsgPUTPDJINI
// ______________

class CmsgPUTPDJINI : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgPUTPDJINI();
    
    ~CmsgPUTPDJINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(char *ini_file_name_path,char *servername);

    BOOL UnLoad(char *ini_file_name_path);


protected:

	 Sputpdjini_svrmsgrec *svr_msg_ptr;


};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETPDJINI
// ______________

class CmsgGETPDJINI : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgGETPDJINI();
    
    ~CmsgGETPDJINI();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(char *servername);

    BOOL UnLoad(char *ini_file_name_path);

protected:

	 Sgetpdjini_svrmsgrec *svr_msg_ptr;

};




