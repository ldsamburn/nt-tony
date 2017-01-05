/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// StartRemoteProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\NtcssApi.h"

#include "afxwin.h"
#include <Lm.h>

#include "atlbase.h"  // For ATL conversion macros
#include "atlconv.h"

void DisplayUsage(void);
int  get_login_name(LPTSTR login_name);

int main(int argc, char* argv[])
{
	
int i;
NtcssServerProcessDescr spd;
char ntcss_pid[NTCSS_SIZE_PROCESS_ID+1];
int ok;


        if (argc < 4 || argc > 15) {
                DisplayUsage();
                exit(1);
        }

        if (get_login_name(spd.owner_str) != 0) {
                printf("Error validating user!\n");
                exit(1);
        }
        //strcpy(spd.owner_str,pw->pw_name);
        strcpy(spd.prog_name_str,argv[3]);

        memset(spd.prog_args_str,NTCSS_NULLCH,NTCSS_SIZE_PROCESS_ARGS);
        for (i=4;i<argc;i++) {
                strcat(spd.prog_args_str,argv[i]);
                if (strlen(spd.prog_args_str))
                        strcat(spd.prog_args_str," ");
        }
        strcpy(spd.job_descrip_str,argv[2]);
        spd.seclvl=0;
        spd.priority_code=1;
        spd.needs_approval_flag=0;
        spd.restartable_flag=0;
        spd.prt_output_flag=0;
        spd.needs_device_flag=0;
        spd.sdi_required_flag=0;
        spd.copies=0;
        spd.banner=0;
        spd.orientation=0;
        spd.papersize=1;

        memset(ntcss_pid,NTCSS_NULLCH,NTCSS_SIZE_PROCESS_ID+1);
        ok=NtcssStartServerProcessEx(argv[1],&spd,ntcss_pid,
                                NTCSS_SIZE_PROCESS_ID);

        if (ok) {
                printf("%s\n",ntcss_pid);
                exit(0);
        } else {
                printf("Unable to start process for %s\n",argv[1]);
                exit(1);
        }

        return 0;
}

void DisplayUsage()
{
        printf("\n");
        printf("Usage: \n");
        printf("      NtcssStartRemoteProcess <App> <Desc> <Cmd> <Parms>\n");
        printf("\n");
        printf("      Where:\n");
        printf("         App    -> The Application Name.\n");
        printf("         Desc   -> A brief description of the process\n");
        printf("         Cmd    -> The complete path and command.\n");
        printf("         Parms  -> The parameters for the command.\n");
        printf("\n");
}

int get_login_name(LPTSTR login_name)     
{
  DWORD dwLevel = 0;
  LPWKSTA_USER_INFO_0 pBuf = NULL;
  NET_API_STATUS nStatus; 
  int ret = 0; 

  USES_CONVERSION;

  //
  // Call the NetWkstaUserGetInfo function;
  //  specify level 1.
  //
  nStatus = NetWkstaUserGetInfo(NULL,
                                dwLevel,
                                (LPBYTE *)&pBuf);

  if ((nStatus != NERR_Success) || (pBuf == NULL))   
      ret = 1;         
  else {

     WCHAR buf[50];
     swprintf(buf, L"%s", pBuf->wkui0_username);     
     memcpy(login_name, W2T(buf), MAX_LOGIN_NAME_LEN);
     login_name[MAX_LOGIN_NAME_LEN] = '\0';

  }

  //
  // Free the allocated memory.
  //
  if (pBuf != NULL)
     NetApiBufferFree(pBuf);

  return ret;
} 

