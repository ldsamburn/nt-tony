
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* NOTE: Clean up duplicate defines and includes */

#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>

/* INRI Includes */
#include <Ntcss.h>
#include <ExtraProtos.h>
#include "ntcss_msg_dist_common.h"


/* Common system include statements in ntcss_msg_dist_common.h */
/* #include "ntcss_msg_dist_common.h" */

/* Local Function Declarations */
int  main(int, char*[]);
int  initialize(void);
int  resetDmResources(void);
int  checkParameters(int, char*[]);
void usage(void);

/* Signal Handlers */
static void  sig_usr1 (int);
static void  sig_usr2 (int);
static void  sig_alarm(int);


#define STATE_IDLE   1
#define STATE_TIMING 2
#define STATE_RESET  3


/* Global Variables */
int nState, nDmResourceWaitInterval;


/*****************************************************************************/

int  main ( int    argc,
	    char  *argv[] )

{
  const char  *prog_name = "DM Resource Monitor:";
  pid_t pid;
  int nContinue = 1;

  openlog("DMResMon", LOG_NDELAY | LOG_PID, LOG_LOCAL7);

  setsid();

  pid = getpid();
  if (setDmResourceMonitorPid(pid) != 0) {
    printf("%s: setDmResourceMonitorPid failed. Exiting.\n", prog_name);
    syslog(LOG_WARNING, "setDmResourceMonitorPid failed. Exiting.");
    exit(1);
  } /* END if */ 

  if (checkParameters(argc, argv) != 0) {
    usage();
    syslog(LOG_WARNING, "checkParameters failed.  Exiting.");
    exit(2);
  } /* END if */
  
  initialize();

  while (nContinue) {
    pause(); /* Suspends until a signal is received */

    switch(nState) {
    case STATE_IDLE:
      break;

    case STATE_TIMING: /* Alarm is set in signal handler */
      break;

    case STATE_RESET: 
      syslog(LOG_WARNING, "Resetting DM Resources");
      if (resetDmResources() != 0) {
	syslog(LOG_WARNING, "resetDmResources failed");
      } /* END if */
      nState = STATE_IDLE;
      break;

    default:
      printf("DM Resource Monitor: ERROR: Unknown state. Exiting.\n");
      syslog(LOG_WARNING, "Fata Error: Unknown state. nState == <%d>.", 
	     nState);
      nContinue = 0; /* Cause loop to terminate */
      break;
    } /* END switch */
  } /* END while */
 
  syslog(LOG_WARNING, "%s terminating normally.", prog_name);
  return(0);
} 


/*****************************************************************************/

int  resetDmResources( void )

{
  struct sembuf  semOp;
  int  nSemId, nDmPid;
  int  nResult; 

  /* Reset semaphore values */
  nSemId = semget(DM_TO_APP_PIPE_SEM_ID, 1, IPC_CREAT | 0777);
  if (nSemId < 0) {
    syslog(LOG_WARNING, "resetDmResources: Failed to get semaphore!");
    return(-1);
  } else {
    semOp.sem_num = 0;
    semOp.sem_op  = 1;
    semOp.sem_flg = 0;
    nResult = semop(nSemId, &semOp, 1);
    if (nResult < 0) {
      syslog(LOG_WARNING, "resetDmResources: semop failed! - %s ",
	     sys_errlist[errno]);
      return(-1);
    } /* END if */
  } /* END if */
  
  removeAndCreatePipes();

  nDmPid = getDmPid();
  if (nDmPid < 0) {
    syslog(LOG_WARNING,
	   "resetDmResource: Error: getDmPid returned %d. Unable to send SIGHUP to DM.",
	   nDmPid);
  } else {
    syslog(LOG_WARNING, "resetDmResource: Sending SIGHUP to DM(Pid: %d)", nDmPid);
    kill(nDmPid, SIGHUP);
  } /* END if */

  return(0);

}


/*****************************************************************************/


int  initialize( void )

{
  const char  *fnct_name = "initialize():";
  nState = STATE_IDLE;
  
  if (signal(SIGUSR1, sig_usr1) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGUSR1) returned SIG_ERR", fnct_name);
  } /* END if */
  
  if (signal(SIGUSR2, sig_usr2) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGUSR1) returned SIG_ERR", fnct_name);
  } /* END if */

  if (signal(SIGALRM, sig_alarm) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGUSR1) returned SIG_ERR", fnct_name);
  } /* END if */
  
  return(0);

}


/*****************************************************************************/


static void  sig_usr1( int  signo )

{
  const char  *fnct_name = "sig_usr1():";
  int  nStoreErrno;

  nStoreErrno = errno;

  if (signal(SIGUSR1, sig_usr1) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGUSR1) returned SIG_ERR!", fnct_name);
  } /* END if */

  nState = STATE_TIMING;
  alarm(nDmResourceWaitInterval);

  signo = signo;
  errno = nStoreErrno;
  return;
}


/*****************************************************************/


static void  sig_usr2( int  signo )

{
  const char  *fnct_name = "sig_usr2():";
  int nStoreErrno;

  nStoreErrno = errno;

  if (signal(SIGUSR2, sig_usr2) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGUSR2) returned SIG_ERR!", fnct_name);
  } /* END if */

  nState = STATE_IDLE;
 
  alarm(0);

  signo = signo;
  errno = nStoreErrno;
  return;
}



/*****************************************************************************/

void  sig_alarm( int  signo )

{
  const char  *fnct_name = "sig_alarm():";
  int  nStoreErrno;

  nStoreErrno = errno;

  /* Resest signal handler for SIGALRM */
  if (signal(SIGALRM, sig_alarm) == SIG_ERROR) {
    syslog(LOG_WARNING, "%s signal(SIGALARM) returned SIG_ERR!", fnct_name);
  } /* END if */

  nState = STATE_RESET;
  
  alarm(0);

  signo = signo;
  errno = nStoreErrno;
  return;

}


/*****************************************************************************/

void  usage(void)

{
  printf("dm_resource_monitor -t TimeOut\n");
  printf("\tTimeOut is in seconds\n");
}


int  checkParameters (int    argc,
		      char  *argv[])

{
  const char *fnct_name = "checkParameters():";
 
  if (argc != 3) {
    return(-1);
  } /* END if */

  if (strcmp(argv[1], "-t") != 0) {
    return(-1);
  } /* END if */

  nDmResourceWaitInterval = atoi(argv[2]);
  if (nDmResourceWaitInterval <= 0) {
    printf("TimeOut set to %d.  TimeOut must be a positive integer\n",
	   nDmResourceWaitInterval);
    syslog(LOG_WARNING, "%s TimeOut set to %d.  TimeOut must be a positive "
	   "integer!", fnct_name, nDmResourceWaitInterval);
    return(-1);
  } /* END if */
  
  syslog(LOG_WARNING, "%s nDmResourceWaitInterval: <%d>", fnct_name,
	 nDmResourceWaitInterval);

  return(0);
}



/*****************************************************************************/

