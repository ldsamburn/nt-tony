
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <string.h>  /* This is the only place strerror() is defined! */
#include <ctype.h>

/** INRI INCLUDES **/
#include <StringFcts.h>
#include <SockFcts.h>
#include <ExtraProtos.h>

/* Common system include statements in ntcss_msg_dist_common.h */
#include "ntcss_msg_dist_common.h"

/* Local Defines */

/* Global Variables */
int    just_caught_debug_sig = 0;
int    debug_level = 0;  
int    nSemDistCompleteId;
int    nSemIndex;
int    nSemReleaseDistId;
char   strHostName[MAX_HOST_NAME_LEN + 1];
int    nPortProblemCount;


/* Local Function Prototypes */

static int   checkParameters ( int, char *[] );
#ifdef USE_DAEMONIZE
static void  daemonize( void );
#endif
static int   distributeAndProcessMessage( int, char*, int );
static void  DPdebugSignal( int );
static int   getSharedMemoryOffset( void );
static int   getShmMessage( char **, int * );
static int   initialize( void );
static int   notifyManagerOfCompletion( void );
static void  sig_alarm( int );
static int   storeResultInShm( void *, int );
static void  usage( void );
static int   waitForManagerToReleaseSemaphore( void );


/*****************************************************************************/

int  main ( int   argc,
            char *argv[] )

{
  int    n;
  int    nConnectToPort;
  int    n_errors = 0;
  char  *pShmMsg;

  sprintf(strHostName, "Uninitialized");
  openlog("", LOG_NDELAY | LOG_PID, LOG_LOCAL7);

  /* checkParameters will make sure the correct number of parameters
     are passed in and will parse them into the correct global variables */
  if (checkParameters(argc, argv) != 0)
    {
      usage();
      exit(1);
    }

  if (initialize() < 0)
    {
      syslog(LOG_WARNING, "DP(%s) Exiting!", strHostName);
      exit(2);
    }

#ifdef USE_DAEMONIZE
  daemonize();
#endif

  /* Allows us to dynamically switch on/off debugging. */

  if (signal(SIGUSR2, DPdebugSignal) == SIG_ERROR)
    syslog(LOG_WARNING, "DP(%s) Failed to set sig!", strHostName);

  /* DWB release 3.0.4 Changed the while() so we could catch debug signals. */
  /* while (waitForManagerToReleaseSemaphore() == 0) */

  nPortProblemCount=0;

  while(1)
    {
      n = waitForManagerToReleaseSemaphore();
      if (n != 0)  
        {
          if (n > 0)                        /* Was it the debug interrupt? */
            {
              if (debug_level >= 10)
                {
                  syslog(LOG_WARNING, "DP(%s) %s", strHostName,
                         strerror(errno));
                }
              if (just_caught_debug_sig == 1)
                {
                  just_caught_debug_sig = 0;
                  continue;                       /* Go back to waiting. */
                }
            }
          break;              /* ..Else just bail - unknown error. */
        }

      /* NOTE: getShmMessage allocates the memory for pShMsg,
         but it is our responsibility to free it */
      if (getShmMessage(&pShmMsg, &nConnectToPort) != 0)
        {
          syslog(LOG_WARNING, "DP(%s) getShmMessage failed!", strHostName);
          n_errors++;
        }
    
      if (debug_level >= 10)
        {
          syslog(LOG_WARNING, "DP(%s) Sending msg <%s> to port %d.",
                 strHostName, pShmMsg, nConnectToPort);
        }
    
      if (distributeAndProcessMessage(nConnectToPort, pShmMsg, 0) < 0)
        {
          n_errors++;
          /***
          syslog(LOG_WARNING, "DP(%s) distributeAndProcessMessage failed!",
                 strHostName);
          ***/
        }
    
      if (notifyManagerOfCompletion() < 0)
        {
          n_errors++;
          syslog(LOG_WARNING, "DP(%s) Failed to notify DM of completion!",
                 strHostName);
        }

      free(pShmMsg);

    }   /* END while */
  return(n_errors + 100);
}


/*****************************************************************************/
/* Takes the message from the DM and sends it to the specified daemon's port
   on this host.
*/

static int  distributeAndProcessMessage( int    nConnectToPort,
                                         char  *pShmMsg,
                                         int    send_and_forget )

{
#define DPM_BUFF_SIZE 2048

  const char  *fnct_name = "distributeAndProcessMessage():";
  int    n;
  int    nReturnDataSize = 0;
  int    sock;
  int    tmp_errno;
  char  *pMsgData;
  char   strReturnBuffer[DPM_BUFF_SIZE];
  char   outbuf[40];
  char   inbuf[40];
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering.", strHostName, fnct_name);

  /* if there have been two consecutive problems, send stat messages */
  if (nPortProblemCount > 1) {
	memset(outbuf,'\0',40);
	memset(inbuf,'\0',40);
	memset(outbuf, '\0', CMD_LEN + 20);
	memcpy(outbuf, "SERVERSTATUS", strlen("SERVERSTATUS"));
	sprintf(&outbuf[CMD_LEN], "%1i", SERVER_STATE_CHECK);
        n=send_msg_to_daemon(strHostName,nConnectToPort,
			outbuf,40,inbuf,27,5);
        if (n<0) {
      		syslog(LOG_WARNING, "DP(%s) %s still not responding!", 
			strHostName, fnct_name);
        } else {
      		syslog(LOG_WARNING, "DP(%s) %s Cleared!",
			strHostName, fnct_name);
		nPortProblemCount=0;
	}
	return(-1);
  }

  sock = sock_init(strHostName, nConnectToPort, 0, SOCK_STREAM);
  if (sock < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s sock_init failed!", strHostName,
             fnct_name);
      nPortProblemCount=0;
      return(-1);
    }

  n = write(sock, pShmMsg, DM_MSG_LEN);
  if (n != DM_MSG_LEN)
    {
      syslog(LOG_WARNING, "DP(%s) %s Only wrote %d of the usual %d bytes"
             "on socket %d!  Errno %d - %s", strHostName, fnct_name, n,
             DM_MSG_LEN, sock, errno, strerror(errno));
      nPortProblemCount=0;
      return(-2);
    }
  
  pMsgData = pShmMsg + DM_MSG_LEN;
  n = write(sock, pMsgData, strlen(pMsgData));
  if ((size_t)n != strlen(pMsgData))
    {
      syslog(LOG_WARNING, "DP(%s) %s Only wrote %d of the %d bytes of msg"
             " <%s> on socket!  Errno %d - %s", strHostName, fnct_name, n,
             strlen(pMsgData), pMsgData, errno, strerror(errno));
      nPortProblemCount=0;
      return(-3);
    }

  if (write(sock, "\0", 1) != 1)
    {
      syslog(LOG_WARNING,"DP(%s) %s Failed to write a NULL to the socket!",
             strHostName, fnct_name);
      nPortProblemCount=0;
      return(-4);
    }

  if (debug_level >= 10)
    syslog(LOG_WARNING, "DP(%s) %s Msg, data, and NULL terminator "
           "sent to host.", strHostName, fnct_name);

  /* Get the response from the server daemon.. */
  memset(strReturnBuffer, '\0', DPM_BUFF_SIZE);
  nReturnDataSize = 0;

  if (send_and_forget == 1)
    {
      if (debug_level > 0)
        syslog(LOG_WARNING, "DP(%s) %s Exiting with success in "
               "send_and_forget mode", strHostName, fnct_name);
      return(0);
    }

  alarm(getDpAlarmTimeoutValue(pShmMsg));

  /* Read in the whole response, ending with first null after 20 bytes.. */
  nReturnDataSize = read_sock(sock, strReturnBuffer, DPM_BUFF_SIZE,
                              NO_TIME_OUT, SOCK_STREAM);

  alarm(0);                                       /* Turn off the alarm. */
  tmp_errno = errno;
  close(sock);

  if (nReturnDataSize < DM_MSG_LEN)
    {
      if (nReturnDataSize > 0)
        {
          syslog(LOG_WARNING, "DP(%s) %s Only read %i of the daemon's "
                 "%i byte response header! --%s", strHostName, fnct_name,
                 nReturnDataSize, DM_MSG_LEN, strerror(tmp_errno));
          return(-5);
        }
      else
        {
          /* Most likely we timed out.. */
          syslog(LOG_WARNING, "DP(%s) %s Failed to read the daemon's "
                 "response header,port marked! --%s", strHostName, fnct_name,
                 strerror(tmp_errno));

	  /* found timeout, increment problem counter */
	  nPortProblemCount++;

          return(-6);
        }
    }

  /* clear the port problem counter */
  nPortProblemCount=0;

  if (debug_level >= 10)
    syslog(LOG_WARNING, "DP(%s) %s Successfully read msg reply from"
           " remote host: [%-20.20s] with data [%-.100s]", strHostName,
           fnct_name, strReturnBuffer, strReturnBuffer + CMD_LEN);
  
  storeResultInShm(strReturnBuffer, nReturnDataSize);

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting with 0 (Success)", strHostName,
           fnct_name);

  return(0);
}


/*****************************************************************************/
/* Finds the offset into shared memory which this DP is supposed to write
   responses to.
*/

static int  getSharedMemoryOffset( void )

{
  const char  *fnct_name = "getSharedMemoryOffset():";
  int    nReturn;
  int    nShmIdReturnOffset;
  void  *pShMem;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering..", strHostName, fnct_name);

  nShmIdReturnOffset = shmget(myShmReturnOffsetKey, 0, IPC_CREAT | 0777);
  
  if (nShmIdReturnOffset < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to get semaphore ID for "
             "myShmReturnOffsetKey (%d)! Errno %d - %s", strHostName,
             fnct_name, myShmReturnOffsetKey, errno, strerror(errno)); 
      return(-1);
    }
  
  pShMem = shmat(nShmIdReturnOffset, 0, 0777);
  if ((int)pShMem == -1)  
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))   */
    { 
      syslog(LOG_WARNING, "DP(%s) %s Failed to attach to shared memory!"
             " Errno %d - %s", strHostName, fnct_name, errno,
             strerror(errno)); 
      return(-2);
    }

  /* Offset array is an array of integers in shared memory that gives
     each distributor the offset into the shared memory to write
     its message reply into.
     The following pointer indirection is equivalent to int offsetArray[];
     offsetArray[nSemIndex] See ntcss_msg_dist_mngr.c for how the
     array is built.
  */
  nReturn = *((int *)pShMem + nSemIndex);
  
  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to detach from shared memory "
             "after getting the offset index for this DP! Errno %d - %s",
             strHostName, fnct_name, errno, strerror(errno)); 
      return(-3);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting with %d. (Success)",
           strHostName, fnct_name, nReturn);

  return(nReturn);
}


/*****************************************************************************/
/* Takes the response from the remote NTCSS server daemon and stores it in
   shared memory.
*/

static int  storeResultInShm( void *pData, int nSize )

{
  const char  *fnct_name = "storeResultInShm():";
  int    nShmRetId;
  int    nSharedMemoryOffset;
  char  *pShmRet;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering..", strHostName, fnct_name);


  /* Get offset for where to store result */
  nSharedMemoryOffset = getSharedMemoryOffset();
  if (nSharedMemoryOffset < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to find the offset into"
             "shared memory in which to write the daemon response!",
             strHostName, fnct_name);
      return(-1);
    }

  nShmRetId = shmget(myShmReturnKey, 0, IPC_CREAT | 0777);
  if (nShmRetId < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to get semaphore ID for"
             " myShmReturnKey (%d)! Unable to write daemon response msg!"
             " -- %s", strHostName, fnct_name, myShmReturnKey,
             strerror(errno));
      return(-2);
    }

  pShmRet = shmat(nShmRetId, 0, 0777);
  if ((int)pShmRet == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))  */
    { 
      syslog(LOG_WARNING,  "DP(%s) %s Failed to attach to shared memory!"
             " Unable to write daemon response msg! Errno %d - %s",
             strHostName, fnct_name, errno, strerror(errno));
      return(-3);
    }

  /* Now store the result */
  memcpy(pShmRet + nSharedMemoryOffset, pData, nSize);

  if (shmdt(pShmRet) < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to detach from shared memory "
             "after storing the daemon response msg! Errno %d - %s",
             strHostName, fnct_name, errno, strerror(errno));
      return(-4);
    }
 
  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting with 0. (Success)", strHostName,
           fnct_name);
  
  return(0);
}


/*****************************************************************************/
/* getShmMessage will read shared memory and get the message
   placed there by ntcss_msg_dist_mngr that we are to send
   to our host (strHostName) and which port to send it to.
   getShmMessage will allocate the space needed for the message
   and assign that pointer to *pBuf.  It is the responsibility
   of the function that called getShmMessage to free the memory
   allocated for the message */

static int  getShmMessage( char **pBuf, int *pnConnectToPort )

{
  const char  *fnct_name = "getShmMessage():";
  int    nShmMsgId;
  char  *pShmMsg;
  char   strPortNumber[5];
  char  *pMsg;
  char  *pMsgData;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering..", strHostName, fnct_name);

  /* Magic Number 5 should probably be a #define */
  memset(strPortNumber, '\0', 5);

  nShmMsgId = shmget(myShmMsgKey, 0, IPC_CREAT | 0777);
  if (nShmMsgId < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to get the semaphore ID for "
             "key %d! --%s",  strHostName, fnct_name, myShmMsgKey,
             strerror(errno));
      return(-1);
    }

  pShmMsg = shmat(nShmMsgId, 0, 0777);
  if ((int)pShmMsg == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))  */
    {
      syslog(LOG_WARNING, "DP(%s) %s  Failed to attach to shared memory and "
             "read msg to distribute! -- %s", strHostName, fnct_name,
             strerror(errno));
      return(-2);
    }

  /* First PORT_LEN chars are ascii representation of the port to send
     this message to.  After that is the DM_MSG_LEN char message
     followed by the data to send which is null terminated*/

  pMsg = pShmMsg + PORT_LEN;
  pMsgData = pShmMsg + DM_MSG_LEN + PORT_LEN;

  *pBuf = malloc(sizeof(char) * (DM_MSG_LEN + strlen(pMsgData) + 1));

  memcpy(strPortNumber, pShmMsg, PORT_LEN);
  *pnConnectToPort = atoi(strPortNumber);
  memcpy(*pBuf, pMsg, DM_MSG_LEN);
  memcpy(*pBuf + DM_MSG_LEN, pMsgData, strlen(pMsgData) + 1);

  if (shmdt(pShmMsg) < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to detach from shared memory!"
             " --%s", strHostName, fnct_name, strerror(errno));
    }

  if (debug_level > 19)
    syslog(LOG_WARNING, "DP(%s) %s Msg == [%-20.20s], Data == [%s]",
           strHostName, fnct_name, *pBuf, *pBuf + DM_MSG_LEN);

  if (debug_level > 9)
    syslog(LOG_WARNING, "DP(%s) %s Success, exiting.", strHostName, fnct_name);

  return(0);  

}


/*****************************************************************************/

static int  waitForManagerToReleaseSemaphore( void )

{
  struct sembuf  semOp;
  const char  *fnct_name = "waitForManagerToReleaseSemaphore():";
  int     nResult;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) Entering %s", strHostName, fnct_name);
  
  semOp.sem_num = nSemIndex;
  semOp.sem_flg = 0;
  
  /* When driver is ready for this program to go it will 
     release the semaphore.  Until then this call will block */
  semOp.sem_op = -1;
  nResult = semop(nSemReleaseDistId, &semOp, 1);
  if (nResult < 0)
    {
      if (errno == EINTR)                                /* Was it a signal? */
        {
          if (debug_level >= 10)
            syslog(LOG_WARNING, "DP(%s) %s Interrupted by a signal.",
                   strHostName, fnct_name);
          return(1);
        }

      /* Unknown error. */
      syslog(LOG_WARNING, "DP(%s) %s semop() Failed! --%s ", strHostName,
             fnct_name, strerror(errno));
      return(-2);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) Exiting %s with 0 (Success)", strHostName,
           fnct_name);

  return(0);
}


/*****************************************************************************/
/* Signals the DM (via a semaphore release that it blocks on) that a response
   has been obtained from the remote host.
*/

static int  notifyManagerOfCompletion( void )

{
  const char  *fnct_name = "notifyManagerOfCompletion():";
  struct sembuf  semOp;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering.", strHostName, fnct_name);

  semOp.sem_num = 0;
  semOp.sem_flg = 0;
  semOp.sem_op =  1;

  /* Increment semaphore that the Dist Manager is waiting on.. */
  if (semop(nSemDistCompleteId, &semOp, 1) < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed release the semaphore that "
             "tells the DM that this DP is finished!  --%s",
             strHostName, fnct_name, strerror(errno));
      return(-1);
    }
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting with 0 (Success)", strHostName,
           fnct_name);

  return(0);  
}


/*****************************************************************************/

static void  sig_alarm( int  signal_num )

{
  const char  *fnct_name = "sig_alarm():";

  signal_num = signal_num;         /* Just so this is not an unused var. */

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering.", strHostName, fnct_name);

  /* Turn off alarm */
  alarm(0);

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting.", strHostName, fnct_name);

  /* Set the signal handler */
  signal(SIGALRM, sig_alarm);

}


/*****************************************************************************/

static int  initialize( void )

{
  const char  *fnct_name = "initialize():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering..", strHostName, fnct_name);

  /* Get the semaphore ID */
  nSemReleaseDistId = semget(mySemReleaseDistKey, 0, IPC_CREAT | 0777);
  if (nSemReleaseDistId < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to obtain semaphore"
             " id for key nSemReleaseDistId (%d) -- %s", strHostName,
             fnct_name, nSemReleaseDistId, strerror(errno));
      return(-1);
    }

  nSemDistCompleteId = semget(mySemDistCompleteKey, 0, IPC_CREAT | 0777);
  if (nSemDistCompleteId < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Failed to obtain semaphore"
             " id for key mySemDistCompleteKey (%d)  -- %s",
             strHostName, fnct_name, mySemDistCompleteKey, strerror(errno));
      return(-2);
    }

  /* Set the signal handler */
  signal(SIGALRM, sig_alarm);

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting with 0 (Success).", strHostName,
           fnct_name);

  return(0);
}


/*****************************************************************************/
#ifdef USE_DAEMONIZE
static void  daemonize( void )

{
  const char  *fnct_name = "daemonize():";
  pid_t   pid = 0;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering..", strHostName, fnct_name);

  pid = fork();
  if (pid < 0)
    {
      syslog(LOG_WARNING, "DP(%s) %s Fork failed!", strHostName, fnct_name);
      exit(1);
    }
  else
    if(pid != 0)
      {
        /* Parent exits */
        if (debug_level > 0)
          {
            syslog(LOG_WARNING, "DP(%s) %s Parent program terminating "
                   "normally.", strHostName, fnct_name);
          }
        exit(0);
      }

  /* Child continues */
  setsid(); /* Become session leader */
  chdir("/"); 
  umask(0);

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Child successfully exiting.", strHostName,
           fnct_name);

}
#endif

/*****************************************************************************/

static int  checkParameters ( int   argc,
                              char *argv[] )

{
  const char  *fnct_name = "initialize():";
  int   hostname_flag = 0;
  int   i;

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering.", strHostName, fnct_name);

  memset(strHostName, '\0', MAX_HOST_NAME_LEN);
  nSemIndex = -1;
 
  if ( argc < 5)
    {
      syslog(LOG_WARNING, "DP invoked with too few parameters!");
      fprintf(stderr, "Insufficient args!");
      return(-1);
    }

  for (i = 1; i < argc; i++)
    {
      /* Check for the debug flag.. */
      if (strcmp(argv[i], "-debug") == 0)
        {
          debug_level = 1;

          /* Check to see if a debug level was specified next.. */
          if ( (i < (argc - 1)) && isdigit( (int)(*(argv[i + 1])) ) )
            {
              debug_level = atoi(argv[i + 1]);
              i++;                  /* Should now point at the debug number. */
              continue;             /* See what's next ont he command line.. */
            }
        }                             /* End of debug flag stuff. */

      /* Check for the host arg.. */
      if (strcmp(argv[i], "-h") == 0)
        {
          if ( (i + 1) == argc )        /* There had better be another arg! */
            {
              printf("Invalid -h arg!\n");
              syslog(LOG_WARNING, "DP invoked without companion arg of -h!");
              return(-2);
            }
          hostname_flag = 1;
          sprintf(strHostName, "%s", argv[i + 1]);
          if(debug_level >= 10)
          syslog(LOG_WARNING, "DP invoked for host %s.", strHostName);
          i++;
          continue;
        }


      /* Check for the semaphore arg.. */
      if (strcmp(argv[i], "-s") == 0)
        {
          if ( i == (argc - 1) )        /* There had better be another arg! */
            {
              printf("Invalid -s arg!\n");
              syslog(LOG_WARNING, "DP invoked without companion arg of -s!");
              return(-2);
            }
          nSemIndex = atoi(argv[i + 1]);
          if(debug_level >= 10)
            syslog(LOG_WARNING, "DP invoked with a semaphore index of %i.",
                   nSemIndex);
          i++;
          continue;
        }
    }

  if (hostname_flag != 1)
    {
      syslog(LOG_WARNING, "DP invoked with no -h argument!");
      return(-3);
    }

  if (nSemIndex < 0)
    {
      syslog(LOG_WARNING, "DP invoked with no -s argument!");
      return(-4);
    }

  if (debug_level > 0)
      syslog(LOG_WARNING, "DP(%s) %s Exiting with Success.", strHostName,
             fnct_name);

  return(0);
}


/*****************************************************************************/

static void  usage( void )

{
  const char  *fnct_name = "usage():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Entering.", strHostName, fnct_name);

  printf("Usage: ntcss_msg_dist -h <host> -s <semaphore index> "
         "-debug <debug level>\n");
  printf("\twhere <host> is an NTCSS II Replication/Authentification server.\n"
         "\t<semaphore index> is the local Dist Manager's pipe semaphore.\n"
         "\t<debug level> specifies the content/volume of syslog debug "
         "messages.\n\n");

  if (debug_level > 0)
    syslog(LOG_WARNING, "DP(%s) %s Exiting.", strHostName, fnct_name);

}


/*****************************************************************************/
/* Sets the level of debug.  This is the signal handler for SIGUSR2.
 */

static void  DPdebugSignal( int  signo )

{
  signo = signo;                      /* Evades compiler warnings. */

  /* Reset handler.. */
  signal(SIGUSR2, DPdebugSignal);

  just_caught_debug_sig = 1;

  /* Don't need debug levels*/
  /*
  if (debug_level == 0)
    {
      syslog(LOG_WARNING, "DP(%s) Turning debug on to Level 1.",
             strHostName);
      debug_level = 1;
      return;
    }

  if (debug_level < 10)
    {
      syslog(LOG_WARNING, "DP(%s) Stepping debug up to Level 10 from %i.",
             strHostName, debug_level);
      debug_level = 10;
      return;
    }

  if (debug_level < 20)
    {
      syslog(LOG_WARNING, "DP(%s) Stepping debug up to Level 20 from %i.",
             strHostName, debug_level);
      debug_level = 20;
      return;
    }

  if (debug_level >= 20)
    {
      syslog(LOG_WARNING, "DP(%s) Turning debug off.", strHostName);
      debug_level = 0;
      return;
    }
  */

  if (debug_level == 0) {
     syslog(LOG_WARNING, "DP(%s) Turning debug on.", strHostName);
     debug_level = 30;
  }
  else {
     syslog(LOG_WARNING, "DP(%s) Turning debug off.", strHostName);
     debug_level = 0;
  }

  return;
}

/*****************************************************************************/
