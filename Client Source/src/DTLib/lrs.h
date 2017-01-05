/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include <sizes.h>

///////////////////////////////////////////////////////////////////
// Structures for the LRS that is rx'd from the authentication
//  server.
///////////////////////////////////////////////////////////////////

///////////////
// UserInfo Struct
///////////////
#ifndef _lrs_
#define _lrs_
typedef struct UserInfoStruct {
	char m_strToken[SIZE_TOKEN];
	int m_nUnixID;
	char m_strLoginName[SIZE_LOGIN_NAME];
	char m_strRealName[SIZE_REAL_NAME];
	char m_strSSN[SIZE_SSN];
	char m_strPhoneNumber[SIZE_PHONE_NUMBER];
	int m_nSecurityClass;
	char m_strPasswordChangeTime[SIZE_RELEASE_DATE];
	char m_strShardPassword[SIZE_PASSWORD];
	int m_nUserRole;
} UserInfoStruct;


//////////////
// Program Struct, is a linked list
//////////////
typedef struct ProgramStruct {
	char m_strProgramTitle[SIZE_PROGRAM_TITLE];
	char m_strProgramFile[SIZE_PROGRAM_FILE];
	char m_strIconFile[SIZE_ICON_FILE];
	int  m_nIconIndex;
	char m_strCmdLineArgs[SIZE_CMD_LINE_ARGS];
	int  m_nSecurityClass;
	int  m_nFlags;
	char m_strWorkingDir[SIZE_WORKING_DIR];
	struct ProgramStruct *m_stNextProgram;
} ProgramStruct;


////////////////
// Group structure, also a linked list
/////////////////
typedef struct GroupStruct {
	char m_strGroupTitle[SIZE_GROUP_TITLE];
	char m_strHostName[SIZE_HOST_NAME];
	int m_nHostType;
	int m_nAppRole;
	char m_strAppData[SIZE_APP_DATA];
	char m_strAppPassword[SIZE_APP_PASSWORD];
	char m_strIconFile[SIZE_ICON_FILE];
	int m_nIconIndex;
	char m_strVersionNumber[SIZE_VERSION_NUMBER];
	char m_strReleaseDate[SIZE_RELEASE_DATE];
	char m_strClientLocation[SIZE_CLIENT_LOCATION];
	char m_strServerLocation[SIZE_SERVER_LOCATION];
	int m_nNumberOfPrograms;
	struct ProgramStruct *m_stFirstProgram;
	struct GroupStruct *m_stNextGroup;
} GroupStruct;


/////////////////
// The actual LRS itself
/////////////////
typedef struct LoginReplyStruct {
	int m_nUserValidatedFlag;						// this is a valid LRS if set
	struct UserInfoStruct m_stUserInfo;					// include the User Info
	int m_nNumberOfGroups;							// number of nodes in the Groups list
	struct GroupStruct *m_stFirstGroup;				// The first group
	char m_strUnlockedString[SIZE_UNLOCKED_APPS];	// a list of group names that are unlocked
} LoginReplyStruct;

//Tony I added this 1... this seemed to be the right place for it.
typedef struct LRSTemplateStruct
{
	CString strPG;
	DWORD dwApp;
	const BOOL operator==(const LRSTemplateStruct& rhs) const{return strPG == rhs.strPG;}
	const BOOL operator!=(const LRSTemplateStruct& rhs) const{return strPG != rhs.strPG;}
	const BOOL operator<(const LRSTemplateStruct& rhs) const{return strPG < rhs.strPG;}
	const BOOL operator>(const LRSTemplateStruct& rhs) const{return strPG > rhs.strPG;}

	// default constructor
	LRSTemplateStruct()
	{};

	// regular constructor
    LRSTemplateStruct(const CString& strApp,const DWORD dwAddr)
	{strPG=strApp;dwApp=dwAddr;}

	 // copy
    LRSTemplateStruct(const LRSTemplateStruct& rhs)
        {strPG = rhs.strPG; dwApp = rhs.dwApp;}

    // assign
    const LRSTemplateStruct& operator= (const LRSTemplateStruct& rhs)
        {strPG = rhs.strPG; dwApp = rhs.dwApp; return *this;}


} LRSTemplateStruct; 

#endif


///////////////////////////////////////////////////
//
// LRS defines
//
///////////////////////////////////////////////////
//
//

#define UNABLE_TO_ACCESS_PAS		-1
#define UNABLT_TO_CONNECT			-2
#define	UNABLE_TO_SEND_MT			-3
#define	UNABLE_TO_RX_MESSAGE		-4
#define	UNABLE_TO_CHANGE_PWD		-5
#define NTCSSDIRSTRING				"$NTCSSDIR"
#define NTCSSBINDIRSTRING			"$NTCSSBINDIR"
#define NTCSSRESDIRSTRING			"$NTCSSRESDIR"
#define NTCSSHELPDIRSTRING			"$NTCSSHELPDIR"

// Structre flags for application "FLAGS" field
#define NTCSS_CLIENT_PROC			0			// simple client app
#define NTCSS_SERVER_PROC			1			// Server Process, RunRemoteProcess
#define NTCSS_SERVER_PROC_WARGS		2			// Server Process, concatenate args,RunRemoteProcess
#define NTCSS_SERVER_PROC_HOST		3			// Display a list of hosts from the cmd line args

#define PWMSGSIZE SIZE_MESSAGE_TYPE+SIZE_LOGIN_NAME+SIZE_PASSWORD*2+SIZE_PASSWORD*2
#define NTCSS_AUTO_PWD				"XYZNTCSSNAVMASSOINRIzyx"

///////////////////////////////////////////////////////
//NTCSSHELPDIRSTRING_LEN : constant := 13;			 //
//#define NTCSSDIRSTRING_LEN : constant := 9;	     //
//#define NTCSSBINDIRSTRING_LEN : constant := 12;	 //
//NTCSSRESDIRSTRING_LEN : constant := 12;			 //
///////////////////////////////////////////////////////
