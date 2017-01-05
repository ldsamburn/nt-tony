/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//#include "ntcssapi.h"
//#include "ntcsss.h"
#define DllImport __declspec(dllimport)


int DLL_ENTRY Ntcss_SetLRS (loginReply far *lrs);                                
int DLL_ENTRY Ntcss_ClearLRS();
BOOL DLL_ENTRY NtcssSetUserInfo(char *szPrivateKey, NtcssUserInfo *ntcssUserInfo);
BOOL DLL_ENTRY NtcssClearUserInfo(char *szPrivateKey); 
BOOL DLL_ENTRY NtcssGetToken(char *szTokenBuffer);


