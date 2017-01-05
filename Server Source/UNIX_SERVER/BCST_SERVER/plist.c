/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/* plist.c */

/** SYSTEM INCLUDES **/

#include <stdio.h>
/* #include <fcntl.h> */
/* #include <sys/types.h> */
/* #include <sys/dir.h> */
#include <dirent.h>
#include <stdlib.h>
/* #include <Ntcss.h> */

int  main( int    argc,
           char **argv )

{
struct dirent /* *dp, */ *direc;
DIR   *dp;
int    oneflag;
char  *dummy;

  /* Assign these just to quiet the compiler about unused vars.. */   
  oneflag = argc;
  dummy = argv[1];
	oneflag=0;
	/* dp=opendir("/usr/spool/lp/request"); */
	dp=opendir("/usr/spool/lp/request");

	if (dp) {
		while ((direc=readdir(dp))) {
			if (direc->d_name[0] == '.')
				continue;

			if (oneflag)
				printf(",");
			printf("%s",direc->d_name);
			oneflag=1;
		}
		/* close(dp); */
		closedir(dp);
	}
	

	exit(0);
}
