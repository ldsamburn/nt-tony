/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include <Ntcss.h>
#include "NtcssApi.h"

void DisplayUsage(void);
int  main(int, char**);

int main(argc,argv)
int argc;
char **argv;
{
/* AppItem **Apps; */
int /*num_apps,*/i;
/* char remhost[NTCSS_SIZE_HOSTNAME+1]; */
NtcssServerProcessDescr spd;
struct passwd *pw;
char ntcss_pid[NTCSS_SIZE_PROCESS_ID+1];
int ok;


	if (argc < 4 || argc > 15) {
		DisplayUsage();
		exit(1);
	}

/*
	memset(remhost,NULL,NTCSS_SIZE_HOSTNAME+1);
	Apps=getAppData(&num_apps,NTCSS_DB_DIR,WITHOUT_NTCSS_GROUP);

	for (i=0;i<num_apps;i++) {
		if (!strcmp(Apps[i]->progroup_title,argv[1])) {
			strcpy(remhost,Apps[i]->hostname);
			break;
		}
	}
        freeAppData(Apps,num_apps);
*/

	pw=getpwuid(getuid());
	if (!pw) {
		printf("Error validating user!\n");
		exit(1);
	}
	strcpy(spd.owner_str,pw->pw_name);
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
