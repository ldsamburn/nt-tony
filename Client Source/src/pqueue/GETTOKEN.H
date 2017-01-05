/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


// Packaging prototypes

//extern "C" {
//BOOL DLL_ENTRY NtcssGetToken(char far *token_buf);
//}


// #define DllImport __declspec(dllimport)
extern "C" int WINAPI NtcssGetToken(char *token);
