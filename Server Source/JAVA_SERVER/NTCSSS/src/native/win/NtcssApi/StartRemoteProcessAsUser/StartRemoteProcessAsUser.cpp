/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// StartRemoteProcessAsUser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "..\NtcssApi.h"

int DisplayUsage(void);

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

/*
        pw=getpwuid(getuid());
        if (!pw) {
                printf("Error validating user!\n");
                exit(1);
        }
*/
        strcpy(spd.owner_str,argv[2]);
        strcpy(spd.prog_name_str,argv[4]);

        memset(spd.prog_args_str,NULL,NTCSS_SIZE_PROCESS_ARGS);
        for (i=5;i<argc;i++) {
                strcat(spd.prog_args_str,argv[i]);
                if (strlen(spd.prog_args_str))
                        strcat(spd.prog_args_str," ");
        }
        strcpy(spd.job_descrip_str,argv[3]);
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

        memset(ntcss_pid,NULL,NTCSS_SIZE_PROCESS_ID+1);
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

int DisplayUsage(void)
{
        printf("\n");
        printf("Usage: \n");
        printf("      NtcssStartRemoteProcess <App> <User> <Desc> <Cmd> <Parms>\n");
        printf("\n");
        printf("      Where:\n");
        printf("         App    -> The Application Name.\n");
        printf("         User   -> The User Name.\n");
        printf("         Desc   -> A brief description of the process\n");
        printf("         Cmd    -> The complete path and command.\n");
        printf("         Parms  -> The parameters for the command.\n");
        printf("\n");

        return 0;
}
