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
                                       // >>> GETPDJLISTMSGFILE stuff
                                       //
#define  DO_CHANGEPASSWORD_MSGSTR    "CHANGEUSERPASSWD"
#define  OK_CHANGEPASSWORD_MSGSTR    "GOODPASSWDCHANGE"
#define  BAD_CHANGEPASSWORD_MSGSTR   "NOPASSWDCHANGE"
#define  SIZE_CHANGEPASSWORD_RESPONSE 65536       /* big buffer hey*/


////////////////////////////////////////////////////////////////////////////////
typedef struct Schangepassword_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				oldPwd[SIZE_PASSWORD*2];
	char				newPwd[SIZE_PASSWORD*2];
    char                null_tail;

} Schangepassword_svrmsgrec;


/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEPASSWORD
// ______________

class CmsgCHANGEPASSWORD : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgCHANGEPASSWORD();
    
    ~CmsgCHANGEPASSWORD();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	Load(char *strOldPasswordCrypted,char *strNewPasswordCrypted);

//	BOOL UnLoadToken(char *strToken);

protected:


};




#define  DO_LAUNCHINTERACTIVE_MSGSTR    "LAUNCHSVRPROCINTER"
#define  OK_LAUNCHINTERACTIVE_MSGSTR    "GOODSVRPROC"
#define  BAD_LAUNCHINTERACTIVE_MSGSTR   "NOSVRPROC"
#define  SIZE_LAUNCHINTERACTIVE_RESPONSE 65536      


////////////////////////////////////////////////////////////////////////////////
typedef struct Srunremote_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				strAppName[SIZE_GROUP_TITLE];
	char				strCommand[SIZE_CMD_LINE];
	char				strOwner[SIZE_LOGIN_NAME];
    char                null_tail;

} Srunremote_svrmsgrec;


/////////////////////////////////////////////////////////////////////////////
// CmsgRunRemote
// ______________

class CmsgRunRemote : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgRunRemote();
    
    ~CmsgRunRemote();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	Load(char *strAppName,char *strCommand, char *strOwner, char *strHostName);

	BOOL UnLoadPID(char *strPID);

protected:


};


/////////////////////////////////////////////////////////////////////////////
// CmsgLogout
// ______________

#define  DO_LOGOUT_MSGSTR    "USERLOGOUT"
#define  OK_LOGOUT_MSGSTR    "GOODUSERLOGOUT"
#define  BAD_LOGOUT_MSGSTR   "NOUSERLOGOUT"
#define  SIZE_LOGOUT_RESPONSE 20      
//#define  SIZE_LOGOUT_RESPONSE 0


////////////////////////////////////////////////////////////////////////////////
typedef struct Slogout_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				strUnlockedApps[SIZE_UNLOCKED_APPS];
    char                null_tail;

} Slogout_svrmsgrec;

class CmsgLogout : public CNtcssMEM_MSG_ADVALLOC_Msg
{

public:

    CmsgLogout();
    
    ~CmsgLogout();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	Load(char *strLockedApps);

protected:
	Slogout_svrmsgrec* svr_msg_ptr;


};


/*

//////////////////////////////////////////////////////////////
//////////////////  Kill Message   ///////////////////////////
//////////////////////////////////////////////////////////////

#define  DO_KILLPROCESS_MSGSTR    "KILLSVRPROC"
#define  OK_KILLPROCESS_MSGSTR    "GOODKILLSVRPROC"
#define  BAD_KILLPROCESS_MSGSTR   "NOKILLSVRPROC"
#define  SIZE_KILLPROCESS_RESPONSE 0       // big buffer hey


                                        //
typedef struct Skillprocess_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char pid[SIZE_TOKEN];
    char killtype[SIZE_SIGNAL_TYPE];
	char progroup[SIZE_APP_NAME];
    char null_tail;

} Skillprocess_svrmsgrec;


class CmsgKILLPROCESS : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgKILLPROCESS();
    
    ~CmsgKILLPROCESS();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(char *progroup, int killtype, char *pid, char *servername);

//	BOOL UnLoad(int *iPdjs);

protected:


};


//////////////////////////////////////////////////////////////
//////////////////  Remove Message   /////////////////////////
//////////////////////////////////////////////////////////////

#define  DO_REMOVEPROCESS_MSGSTR    "REMOVESPQITEM"
#define  OK_REMOVEPROCESS_MSGSTR    "GOODREMOVESPQITEM"
#define  BAD_REMOVEPROCESS_MSGSTR   "NOREMOVESPQITEM"
#define  SIZE_REMOVEPROCESS_RESPONSE 0       // big buffer hey


                                        //
typedef struct Sremoveprocess_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char pid[SIZE_TOKEN];
    char jobtype[SIZE_JOB_TYPE];
	char progroup[SIZE_APP_NAME];
    char null_tail;

} Sremoveprocess_svrmsgrec;


class CmsgREMOVEPROCESS : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgREMOVEPROCESS();
    
    ~CmsgREMOVEPROCESS();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(char *progroup, char *jobtype, char *pid, char *servername);

//	BOOL UnLoad(int *iPdjs);

protected:


};


//////////////////////////////////////////////////////////////
//////////////////  Alter Message   /////////////////////////
//////////////////////////////////////////////////////////////

#define  DO_ALTERPROCESS_MSGSTR    "ALTERSPQITEM"
#define  OK_ALTERPROCESS_MSGSTR    "GOODALTERSPQITEM"
#define  BAD_ALTERPROCESS_MSGSTR   "NOALTERSPQITEM"
#define  SIZE_ALTERPROCESS_RESPONSE 0       // big buffer hey


typedef struct Salterprocess_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char pid[SIZE_TOKEN];
    char jobtype[SIZE_JOB_TYPE];
	char altermask[SIZE_ALTER_MASK];
	char customprocstat[SIZE_PROC_STAT];
	char state[SIZE_PROCESS_STATE];
	char priority[SIZE_PRIORITY];
    char null_tail;

} Salterprocess_svrmsgrec;


class CmsgALTERPROCESS : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgALTERPROCESS();
    
    ~CmsgALTERPROCESS();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL Load(char *pid,int state, int priority, 
							char *jobtype, int operation, char *custproc, char *servername);

	BOOL UnLoad(char *message);

protected:

};


*/





