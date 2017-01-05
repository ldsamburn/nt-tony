/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

/////////////////////////////////////////////////////////
// Desktop DLL import include file
//
// ACA 6/11/01...............Original Coding
/////////////////////////////////////////////////////////

#include "lrs.h"
#ifndef __DTIMPORT
#define __DTIMPORT
#ifdef __DTEXPORT
#define __DECL dllexport
#else
#define __DECL dllimport
#endif

	__declspec(__DECL) BOOL NtcssChangePassword(char *strUserName,
												char *strOldPwd,char *strNewPwd);
	__declspec(__DECL) int ValidateUser(const char *strUserName,const char *strPassword,
											const char *strDefaultPAS, char *strPASName, 
											BOOL &bAppAdmin);
	__declspec(__DECL) int GetUnlockedAppList(char *strUnlockedApps);
	__declspec(__DECL) int GetUnlockedAppCount(void);
	__declspec(__DECL) int LogoutUser(void);
	__declspec(__DECL) int RunRemote(char *strCmdLineArgs,char *strAppName,char *strHostName=NULL);
	__declspec(__DECL) BOOL ClearLRS(void);
	__declspec(__DECL) BOOL NTCSS_FindServer(void);
	__declspec(__DECL) int GetUserPAS(char *strPAS);
	__declspec(__DECL) LoginReplyStruct* __stdcall GetLRS(void);
	__declspec(__DECL) int GetErrorString(char *strErrMsg);

#endif