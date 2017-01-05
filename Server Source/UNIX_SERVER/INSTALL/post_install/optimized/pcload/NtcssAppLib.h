
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

