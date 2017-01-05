/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ntcsss.h: Defines for private NTCSS applications
// These functions can be called from C or C++ 
//
/***************************** NTCSSII *****************************/
/**                                                               **/
/**  Developed by Inter-National Research Institute, Inc. under   **/
/**  contract to the Department of Defense.  All U.S. Government  **/
/**  restrictions on software distributions apply.                **/
/**                                                               **/
/**       Copyright 1994, Inter-National Research Institute       **/
/**                      All Rights Reserved.                     **/ 
/**                                                               **/
/*******************************************************************/


#ifndef _DEFINE_NTCSSS_H_
#define _DEFINE_NTCSSS_H_

#define SIZE_LOGIN_NAME 8
#define SIZE_REAL_NAME 64
#define SIZE_PHONE_NUMBER 32
//#define SIZE_PASSWORD 30 Defined in Sizes.h
#define SIZE_PROGROUP_TITLE 16
#define SIZE_APP_DATA 256
#define SIZE_APP_PASSWORD 32
#define SIZE_ICON_FILE 128
#define SIZE_VERSION_NUMBER 32
//#define SIZE_RELEASE_DATE 14  Defined in Sizes.h
#define SIZE_CLIENT_LOCATION 128
#define SIZE_SERVER_LOCATION 128
#define SIZE_PROGRAM_TITLE 64
#define SIZE_PROGRAM_FILE 32
#define SIZE_CMD_LINE_ARGS 128
#define SIZE_NUM_GROUPS_PER_USER 10
#define SIZE_NUM_PROGRAMS_PER_PROGROUP 10
#define SIZE_REQUEST_PROGRAM 30
#define SIZE_TOKEN 32 
#define SIZE_MESSAGE_TYPE 20  
#define SIZE_HOST_NAME 16
#define SIZE_PRT_FILE_NAME 128  
#define SIZE_PRT_NAME 16  
#define SIZE_PRT_LOCATION 64 
#define SIZE_PRT_DEV_NAME 128    
#define SIZE_PRT_RED 64   
#define SIZE_PRT_REQ_ID 36 
#define SIZE_PRT_REQ_TIME 11    
#define SIZE_ORIG_HOST 32

#define NTCSS_PRIVATE_APPNAME "NTCSS"

typedef unsigned int	u_int;

enum securityClassEnum {
	_UNCLASSIFIED = 0,
	_UNCLASSIFIED_SENSITIVE = 1,
	_CONFIDENTIAL = 2,
	_SECRET = 3,
	_TOP_SECRET = 4,
};
typedef enum securityClassEnum securityClassEnum;



struct RunProcessArgs {
	char far *LoginName;
	char far *Password;
	char far *Program;
	char far *Arguments;
	char far *RequestingProgram;
};
typedef struct RunProcessArgs RunProcessArgs;



enum userRoleEnum {
	_NTCSS_USER = 0,
	_NTCSS_ADMINISTRATOR = 1,
};
typedef enum userRoleEnum userRoleEnum;



enum appRoleEnum {
	_APP_USER = 0,
	_APP_ADMINISTRATOR = 1,
};
typedef enum appRoleEnum appRoleEnum;


struct userInfoStruct {
    char far *Token;
	int UnixId;
	char far *LoginName;
	char far *RealName;
	char far *PhoneNumber;
	securityClassEnum SecurityClassification;
	userRoleEnum UserRole;
};
typedef struct userInfoStruct userInfoStruct;


struct loginUserArgs {
	char far *LoginName;
	char far *Password;
};
typedef struct loginUserArgs loginUserArgs;


struct validUserLoginReplyStruct {
	struct userInfoStruct userInfo;
	struct {
		u_int appGroups_len;
		struct appGroupStruct far *appGroups_val;
	} appGroups;
};
typedef struct validUserLoginReplyStruct validUserLoginReplyStruct;


struct loginReply {
	u_int userValidatedFlag;
	union {
		validUserLoginReplyStruct validUserLoginReply;
	} loginReply_u;
};
typedef struct loginReply loginReply;


struct appProgramStruct {
	char far *ProgramTitle;
	char far *ProgramFile;
	char far *IconFile;
	int IconIndex;
	char far *CommandLine;
	securityClassEnum SecurityClass;
	int Flags;
	char far *WorkingDir;
};
typedef struct appProgramStruct appProgramStruct;


struct appGroupStruct {
	char far *ProgroupTitle;
	char far *Hostname;
	appRoleEnum AppRole;
	char far *AppData;
	char far *AppPassword;
	char far *IconFile;
	int IconIndex;
	char far *VersionNumber;
	char far *ReleaseDate;
	char far *ClientLocation;
	char far *ServerLocation;
	struct {
		u_int appPrograms_len;
		struct appProgramStruct far *appPrograms_val;
	} appPrograms;
};
typedef struct appGroupStruct appGroupStruct;


#define NTCSSSERVER ((u_long)783728)

#define NTCSSVERS ((u_long)1)
#define RUNREMOTEPROCESS ((u_long)1)

/*
#define NTCSSDESKTOP ((u_long)783727)

#define NTCSSVERS ((u_long)1)
#define VALIDATEUSER ((u_long)1)
*/

#endif // _DEFINE_NTCSSS_H_
        
