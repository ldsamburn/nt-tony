/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


// NTCSS Remote access to an application DB password.  
// NtcssGetDBPassword-
//		Input:   loginname - A valid application users name.
//				 password  - The valid password for the loginname
//               appname   - The name of the application.
//               hostname  - The host this request is directed towards
//      Output:  dbpassword - The dbpassword retrieved.
//
//


extern "C" BOOL WINAPI NtcssGetDBPassword(char *loginname,
										  char *password,
										  char *appname,
										  char *hostname,
										  char *dbpassword);

