/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"

BOOL AddAccessRights (char *pFileName, DWORD dwAcessMask, BYTE Flags, char *lpszUserName);

VOID DumpACL (LPTSTR lpFileName);

int SetFileAccess(char *FileName,char *Operation,char *Trustee);



/////////////////////////////////
// NtcssAddUserToNT : Adds the give user to NT
//    Input:  loginame  - users name (jsmith)
//            password  - users password (unencrypted)
//			  realname  - long name of the user (John A. Smith)
//			  homedir   - The home dir,usally a share (\\concord\\h\\USERS)
//			  profilepath -
//						  The profile path, usually a share
//            temp_user - The template users to inhereit groups from
//            domain_flag -
//                        1 = add to domain, 0 = add to local
//			  NtcssNTDelay -
//                        A delay to accoutn for lazy writes (usyally 0)
//            adminFlag   - 0 if not adminstrator, else administrator
////////////////////////////////
extern "C" BOOL WINAPI NtcssAddUserToNT(char *loginname,
	    			  char *password,
					  char *realname,
					  char *homedir,
					  char *profilepath,
					  char *temp_user, 
					  BOOL domain_flag, 
					  char *pas,
					  int NtcssNTDelay,
					  int adminFlag);

extern "C" BOOL WINAPI NtcssChangeNTPassword(char *loginname,
					char *oldPassword,char *newPassword,
					int adm_flag,
                    int domain_flag);

extern "C" BOOL WINAPI NtcssDeleteNTUser(char *loginname,int domain_flag, BOOL bRmDir);

extern "C" BOOL WINAPI NtcssChangeNTName(char *loginname,char *realname,BOOL domain_flag);

extern "C" BOOL WINAPI NtcssMakeNTAdmin(char *loginname,
									    char *temp_user, 
										BOOL domain_flag);
extern "C" BOOL WINAPI NtcssMakeAdminNormal(char *loginname,
					  char *temp_user,BOOL domain_flag);

extern "C" BOOL WINAPI NtcssSavePas(const char *strHomeDir,
				  const char *strPAS);

extern "C"	BOOL WINAPI NtcssGetUsersDC(LPCTSTR loginname,
	 							 BOOL domain_flag, 
								 LPTSTR UserDirDomain,LPTSTR ProfileDirDomain);

extern "C" BOOL WINAPI NtcssSetUsersDC(char *loginname,char *homepath, 
								char *profilepath,
								BOOL domain_flag);

