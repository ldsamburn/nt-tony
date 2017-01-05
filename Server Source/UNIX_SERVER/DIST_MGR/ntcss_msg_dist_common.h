
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* System Includes */
#include <syslog.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>


/* INRI Includes */
#include <Ntcss.h>
#include <LibNtcssFcts.h>


#define mySemReleaseDistKey    425
#define mySemDistCompleteKey   426
#define myShmMsgKey            425
#define myShmReturnKey         426
#define myShmReturnOffsetKey   427
#define DM_MSG_LEN              20
#define PORT_LEN                 4

#if defined SOLARIS || defined HPUX
extern char *sys_errlist[];
#endif

int  getAlarmTimeoutValue( char * );
int  getDmAlarmTimeoutValue( char * );
int  getDpAlarmTimeoutValue( char * );
int  removeAndCreatePipes( void );
