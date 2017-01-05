
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

/* INRI Includes */
#include <StringFcts.h>
#include <ExtraProtos.h>

/* Common system include statements in ntcss_msg_dist_common.h */
#include "ntcss_msg_dist_common.h"

typedef struct _DMfuncPtrStruct {
        const char  *cmd;
        const char  *cmd_description;
        int   (*func_ptr)  ( int, char* );
} DMfuncPtrStruct;

typedef struct HostDistInfoStruct_ {
  char *strHostName;
  int   nHostDistributed;
  pid_t pid;
} HostDistInfoStruct;

#define HOST_LIST_SIZE_LEN  5
#define STOP_DISTRIBUTORS_MSG  "DIST_EXIT"

/* Local Function Declarations */
static int   addDpReplyToProgroupUseArray( ProgroupUseStruct *, char * );
static int   allocateMessageBuffer( int );
static int   allocateReturnBuffers( char * );
static int   checkParameters( int, char *[] );
static void  cleanup( void );
static void  daemonize( void );
       void  debugSignal( int );
static int   determineMessagePort( char * );
static int   determineMessageReturnSize( const char * );
static int   determineNumberReturnInstances( const char *, const char * );
static int   determineValidMessageType( char * );
       void  dm_reaper(int);
static void  freeMessageBuffer( void );
static void  freeReturnBuffers( void );
static int   getAuthUserReply( char *, int * );
static int   getDpReply( int, char * );
static int   getHostList( int, char ** );
static int   getNumberDistributorProcesses( void );
static int   getSharedMemoryOffset( int );
static int   getUserPrinterAdminReply( char *, char * );
static int   initialize( void );
static int   processDistributorResults( char * );
static int   processDpAuthUserReplies( int, char * );
static int   processDpCurrentAppData( int, char * );
static int   processDpUserPrinterAdminReplies( int, char * );
static int   processGetMasterFilesReplies( int, char * );
static int   processDpNisMapUpdateReplies( int, char * );
static int   releaseDistributorsAndWait( char *, char * );
static int   setDistributorsToRun( struct sembuf *, char * );
       void  sig_alarm(int);
       void  sig_usr1(int);
       void  sig_hup(int);
static int   startDistributorProcesses( void );
static int   storeMessageInSharedMemory( int, char *, int );
/* static void  usage( void ); Not currently in use. */
static DMfuncPtrStruct  *get_command_data( char *, DMfuncPtrStruct * );

/* Functions that (mostly) listen for responses from the distrinuted messages.
 */

DMfuncPtrStruct dm_commands[] = {         

  { "AUTHUSER",
    "Authenticate User through DPs.",
    processDpAuthUserReplies },

  { "DPPROCESSUSRCHANGES",
    "Perform User Admin through DPs.",
    processDpUserPrinterAdminReplies },

  { "DPPROCESSPRTCHANGES",
    "Perform Printer Admin through DPs.",
      processDpUserPrinterAdminReplies },

  { "COPYMASTERFILES",
    "Copy Master backup files",
    processGetMasterFilesReplies },

  { "DPCURRENTAPPDATA",
    "Get Current App data through DPs.",
    processDpCurrentAppData },

  { MT_NISMAPUPDATE,
    "Inform hosts to get NIS maps.",
    processDpNisMapUpdateReplies },

  { NULL, NULL, NULL }
};


/* Global Variables */
HostDistInfoStruct  *hostDistInfo;
int    debug_level = 0;
int    nContinue = 1;         /* Variable used to drive program loop in main */
int    nHowManyDistributors = 0;
int    nSemIdDistComplete = -1;
int    nShmIdMsg = -1;
int    nSemIdReleaseDist = -1;
int    nShmIdReturn = -1;
int    nShmIdReturnOffset = -1;


/*****************************************************************************/
/* Searches a list of commands and returns a pointer to the one associated
   with the one in the incoming buf variable.

   NOTE: This function is in server_lib.a and should be linked that way 
   Once that is done delete the copy of get_command_data here 
   I changed this function to take a DMfuncPtrStruct ptr instead of
   ServerDefStruct This must be changed
*/
static DMfuncPtrStruct  *get_command_data( char             *buf,
                                           DMfuncPtrStruct  *sdptr )

{
  DMfuncPtrStruct  *tptr;
  
  /*  tptr = sdptr->cstruct; */
  tptr = sdptr;
  while (tptr->cmd)
    {
      if (!strcmp(buf, tptr->cmd))
        return(tptr);
      tptr++;
    }
  return(NULL);
}


/*****************************************************************************/

int  main ( int    argc,
            char  *argv[] )

{
  
  char   ch;
  char   strCompleteMessage[20480];
  char  *strHostList;
  char   strMessage[DM_MSG_LEN + 1];
  char   strMessageToDistribute[DM_MSG_LEN + 1];
  char   strMsgData[20480];
  int    inputStream;
  int    nBytesRead;
  int    nMsgDataLength;
  int    nPort;
  int    nPId;
  int    nFatalError;
  ushort *zeroSemValues;
#ifdef METRICS
  time_t  startT;
  time_t  endT;
#endif
  
  union semun {
    int              val;
    struct semid_ds *buf;
    ushort          *array;
  } sem_arg;

  openlog("DM", LOG_NDELAY | LOG_PID, LOG_LOCAL7);
  
  if (checkParameters(argc, argv) != 0)
    {
      syslog(LOG_WARNING, "Invalid parameters!");
      exit(1);
    }

  if (initialize() != 0)
    {
      syslog(LOG_WARNING, "Exiting!");
      exit(2);
    }

  if (removeAndCreatePipes() != 0)
    {
      syslog(LOG_WARNING, "Failed to init the pipes!");
      cleanup();
      exit(3);
    }
  
  if (signal(SIGCHLD, dm_reaper) == SIG_ERROR)
    syslog(LOG_WARNING, "Failed to set the SIGCHLD signal handler!");

  if (signal(SIGHUP, sig_hup) == SIG_ERROR) 
    syslog(LOG_WARNING, "Failed to set the SIGHUP signal handler!");

  /* Allows us to dynamically switch on/off debugging. */
  if (signal(SIGUSR2, debugSignal) == SIG_ERROR) 
    syslog(LOG_WARNING, "Failed to set the SIGUSR2 signal handler!");

  if (startDistributorProcesses() != 0)
    {
      syslog(LOG_WARNING, "Failed to start all the DPs!");
      cleanup();
      exit(4);
    }

  nPId = getpid();
  if (setDmPid(nPId) != 0)
    {
      printf("setDmPid failed. Exiting.\n");
      syslog(LOG_WARNING, "Failed to set this DM's PID file!  Exiting.");
      exit(5);
    }

  zeroSemValues = (ushort *) malloc(sizeof(ushort) * nHowManyDistributors);
/*
  memset((char *) zeroSemValues, 0, nHowManyDistributors * sizeof(ushort));
*/
  memset((ushort *) zeroSemValues, (ushort)0,
         nHowManyDistributors * sizeof(ushort));
  strMessage[20] = '\0';

  /* Loop, listen, and process requests coming down the App pipe.. */
  while (nContinue)
    {
      nFatalError = 0;
      if (debug_level > 9)
        syslog(LOG_WARNING, "Before open(%s)", APP_TO_DM_PIPE);

      /*  This blocks until an app opens the pipe's other end to initiate a
          request to the DM.
       */
      inputStream = open(APP_TO_DM_PIPE, O_RDONLY);

      if (inputStream < 0)
        {
          /* Do not report error to syslog if open returned negative
             because of an interrupt.  This happens on startup when we get
             SIGCHLD signals for the DPs. */
          if (errno == EINTR)
            {
              if (debug_level > 19)
                syslog(LOG_WARNING, "Caught a signal.");
              continue;
            }

          syslog(LOG_WARNING, "Failed to open pipe to read from apps! --%s",
                 strerror(errno));
      
          nContinue = 0; /* Bail! */
          continue;
        }             /* END if */

      if (debug_level > 19)
        syslog(LOG_WARNING, "An app opened the DM pipe to initiate a "
               "request..");
   
      /* May have to check for condition errno != EPIPE */
      nBytesRead = read(inputStream, strMessage, DM_MSG_LEN);

      if (nBytesRead != DM_MSG_LEN)
        {
          if (nBytesRead == 0)
            {
              /* if nBytesRead == 0, continue the loop, once the process
                 that writes to the pipe closes it we should eventually
                 block on the open. */
              syslog(LOG_WARNING, "Some app failed to initiate its request!"
                     " Closing the pipe and continuing.");
              close(inputStream);
              continue;
            }
          if (nBytesRead < 0)
            {
              syslog(LOG_WARNING, "Error reading request type! --%s",
                     strerror(errno));
              continue;
            }

          syslog(LOG_WARNING, "Only read %d of the usual %d chars of the "
                 "app's request type! msg = <%s>",
                 nBytesRead, DM_MSG_LEN, strMessage);
          /* ACA
          nContinue = 0;
          */
          close(inputStream);
	  continue;
        }
    
      /* Check on the message type */
      if (!determineValidMessageType(strMessage))
        {
          syslog(LOG_WARNING, "Received bad Message! [%-20.20s]  Ignoring.",
                 strMessage);
          continue;
        }

      if (debug_level >= 20)
        syslog(LOG_WARNING, "Read StrMessage: [%-20.20s]", strMessage);

      /* Next read in the list of hosts to distribute the message to.. */
      if (getHostList(inputStream, &strHostList) < 0)
        {
          syslog(LOG_WARNING, "Problem with host list <%s>.  Continuing.",
                 strHostList);
          /*
          nContinue = 0;
          */
          continue;
        }

      if (debug_level > 0)
        syslog(LOG_WARNING, "strHostList == <%s>", strHostList);

      /* Read in the message header to distribute.. */
      memset(strMessageToDistribute, '\0', DM_MSG_LEN);
      nBytesRead = read(inputStream, strMessageToDistribute, DM_MSG_LEN);
      if (nBytesRead != DM_MSG_LEN)
        {
          syslog(LOG_WARNING, "Only read %d of the normal %d characters of"
                 " the app's message header to distribute!", nBytesRead,
                 DM_MSG_LEN);

          free(strHostList);
/*
          nContinue = 0;
*/
          close(inputStream);
	  continue;
        }

      if (debug_level > 19)
        syslog(LOG_WARNING, "strMessageToDistribute: [%-20.20s]",
               strMessageToDistribute);
    
      /* Get the body data to be passed with msg header.. */
      ch = '1';
      nMsgDataLength = 0;
      while (ch != '\0')
        {
          if (read(inputStream, &ch, 1) != 1)
            {
              syslog(LOG_WARNING, "Failed to read the app's msg body from the"
                     " pipe! --%s", strerror(errno));
              nFatalError=1;
              ch = '\0';
            }
          strMsgData[nMsgDataLength] = ch;
          nMsgDataLength++;

        }   /* END while */

      if (debug_level > 9)
          syslog(LOG_WARNING, "App's message body: <%s>", strMsgData);

      /* some fatal error, try to continue */
      if (nFatalError)
        {
          free(strHostList);
          close(inputStream);
	  continue;
        }

      close(inputStream);

      if (debug_level > 19)
        syslog(LOG_WARNING, "closed(APP_TO_DM_PIPE)");      

/*******PRETEND LIKE I BLEW *********
while(1)
   sleep(100);
*/


      memcpy(strCompleteMessage, strMessageToDistribute, DM_MSG_LEN);
      memcpy(strCompleteMessage + DM_MSG_LEN, strMsgData, strlen(strMsgData));
    
      /* Allocate and deallocate message and return buffers each time
         because the message and return sizes might be different each time. */
      allocateMessageBuffer(DM_MSG_LEN + nMsgDataLength + PORT_LEN);
      allocateReturnBuffers(strMessage);

      nPort = determineMessagePort(strMessage);
   
      /* Store message in shared memory for distributors to read.. */
      if (storeMessageInSharedMemory(nPort, strCompleteMessage,
                                     DM_MSG_LEN + nMsgDataLength) < 0)
        {
          nContinue = 0;
          continue;
        }

#ifdef METRICS
      time(&startT);
#endif
      if (releaseDistributorsAndWait(strHostList, strMessage) < 0)
        {
          nContinue = 0;
          continue;
        }
#ifdef METRICS
      time(&endT);
      syslog(LOG_WARNING, "Elapsed time for releaseDistributorsAndWait: %d", 
             (endT - startT));
#endif

      if (processDistributorResults(strMessage) < 0)
        {
          syslog(LOG_WARNING, "Failed to process distributor results!");
          /* nContinue = 0; */
          freeMessageBuffer();
          freeReturnBuffers();
          free(strHostList);
          continue;
        }
 
      /* Reset the semaphores */ 
      sem_arg.array = zeroSemValues;

/*
      if (semctl(nSemIdReleaseDist, 0, SETALL, zeroSemValues) < 0)
*/
      if (semctl(nSemIdReleaseDist, 0, SETALL, sem_arg) < 0)
        {
          syslog(LOG_WARNING, "semctl failed to reset semaphores! -- %s!",
                 strerror(errno));
          nContinue = 0;
          continue;
        }

      freeMessageBuffer();
      freeReturnBuffers();
      free(strHostList);
      if (debug_level > 19)
        syslog(LOG_WARNING, "Completed an iteration of while()");      

    } /* END of the main while */
  
  cleanup();
  free(zeroSemValues);

  if (debug_level > 0)
    syslog(LOG_WARNING, "Exiting main() with return value of 0.");

  exit(0);
}


/*****************************************************************************/

/*  Implemented in common code lib now.
static int  removeAndCreatePipes( void )

{
  const char  *fnct_name = "removeAndCreatePipes():";
  struct stat  pipeStatus;

  if (debug_level > 0)
      syslog(LOG_WARNING, "Entering %s", fnct_name);

  / * Remove pipes if they already exist and then create the  pipes * /
  / * This is done on startup to clean out any data in the pipes that may
     have been left due to problems * /
  if (stat(APP_TO_DM_PIPE, &pipeStatus) == 0)
    {
      if (remove(APP_TO_DM_PIPE) != 0)
        {
          syslog(LOG_WARNING, "%s remove(APP_TO_DM_PIPE) failed!", fnct_name);
          return(-2);
        }
    }
  
  if (mkfifo(APP_TO_DM_PIPE, 0777) < 0)
    {
      syslog(LOG_WARNING, "%s mkfifo(APP_TO_DM_PIPE) failed!", fnct_name);
      return(-3);
    }
  
  if (stat(DM_TO_APP_PIPE, &pipeStatus) == 0)
    {
      if (remove(DM_TO_APP_PIPE) != 0)
        {
          syslog(LOG_WARNING, "%s remove(DM_TO_APP_PIPE) failed!", fnct_name);
          return(-4);
        }
    }

  if (mkfifo(DM_TO_APP_PIPE, 0777) < 0)
    {
      syslog(LOG_WARNING, "%s mkfifo(DM_TO_APP_PIPE) failed!", fnct_name);
      return(-5);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}
***/

/*****************************************************************************/

static int  getHostList( int    inputStream,
                         char **pStrHostList )

{

  const char  *fnct_name = "getHostList():";
  char  strHostListLength[HOST_LIST_SIZE_LEN + 1];
  int   n;
  int   nHostListLength;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  memset(strHostListLength, '\0', HOST_LIST_SIZE_LEN + 1);

  /* Read in the length of the host string to follow.. */
  n = read(inputStream, strHostListLength, HOST_LIST_SIZE_LEN);
  if (n < HOST_LIST_SIZE_LEN)
    {
      syslog(LOG_WARNING, "%s Only read %d of the usual %d chars"
             " of the host list string length!", fnct_name, n,
             HOST_LIST_SIZE_LEN);
      return(-1);
    }

  nHostListLength = atoi(strHostListLength);
  *pStrHostList   = (char *) malloc(sizeof(char) * nHostListLength + 1);
  memset(*pStrHostList, '\0', nHostListLength + 1);
  
  n = read(inputStream, *pStrHostList, nHostListLength);
  if (n < nHostListLength)
    {
      syslog(LOG_WARNING, "%s Only read %d of the %d chars"
             " in the app's host list string!", fnct_name, n, nHostListLength);
      return(-2);
    }
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/

static int  getDpReply( int   nDpIndex,
                        char *strDpReply )

{
  const char  *fnct_name = "getDpReply():";
  char  *pShMemRet;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  pShMemRet = shmat(nShmIdReturn, 0, 0777);
  if ((int)pShMemRet == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE)) */
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-1);
    }

  strncpy(strDpReply, pShMemRet + getSharedMemoryOffset(nDpIndex), CMD_LEN);

  /* Finished with shared memory so detach from it */
  if (shmdt(pShMemRet) < 0)
    {
      syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-2);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "Exiting %s with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/
/* Functions to process DPCURRENTAPPDATA message */

static int  addDpReplyToProgroupUseArray(ProgroupUseStruct *aProgroupUseStruct,
                                         char              *strDpReplyData )
{

  const char  *fnct_name = "addDpReplyToProgroupUseArray():";
  char   strProgroup[MAX_PROGROUP_TITLE_LEN];
  char   strProgroupUse[50];
  int    nCharIndex;
  int    nProgroupUse;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);
  
  /* The data will be in the form progroup~nUsers~progroup~nUsers... */
  nCharIndex = parseWord(0, strDpReplyData, strProgroup,    "~");
  nCharIndex = parseWord(nCharIndex, strDpReplyData, strProgroupUse, "~");
  nProgroupUse = atoi(strProgroupUse);
  while (nCharIndex >= 0)
    {
      addProgroupUseInfo(aProgroupUseStruct, strProgroup, nProgroupUse,
                         MAX_NUM_PROGROUPS);

      nCharIndex = parseWord(nCharIndex, strDpReplyData, strProgroup,   "~");
      nCharIndex = parseWord(nCharIndex, strDpReplyData, strProgroupUse,"~");
      nProgroupUse = atoi(strProgroupUse);
    } /* END while */

  if (debug_level > 0)
    syslog(LOG_WARNING, "Exiting %s", fnct_name);

  return(0);

}


/*****************************************************************************/

static int  processDpCurrentAppData( int    outputFd,
                                     char  *strMessage )

{

  ProgroupUseStruct   aProgroupUseStruct[MAX_NUM_PROGROUPS];
  const char  *fnct_name = "processDpCurrentAppData():";
  char  *pDpReply;
  char  *pDpReplyData;
  char  *pShMemRet;
  char   strBuf[250];
  char   strDmReply[CMD_LEN];
  int    i;
  int    nBytesWritten;

  /* done only to get rid of compiler warning about unused parameters */
  strMessage=strMessage;
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  pShMemRet = shmat(nShmIdReturn, 0, 0777);
  if ((int)pShMemRet == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE)) */
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-1);
    }

  memset(strDmReply,  '\0', CMD_LEN);
  sprintf(strDmReply, "%s", MT_BADCURRENTAPPDATA);

  initProgroupUseStruct(aProgroupUseStruct, MAX_NUM_PROGROUPS);
  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      /* Skip the hosts that did not get the message */
      if (hostDistInfo[i].nHostDistributed == 0)
        continue;
    
      pDpReply     = pShMemRet + getSharedMemoryOffset(i);
      pDpReplyData = pDpReply  + CMD_LEN;

      if (debug_level > 0)
        {
          syslog(LOG_WARNING, "%s DP[%s] Reply == %s", fnct_name,
                 hostDistInfo[i].strHostName, pDpReply);
        }
    
      /* If this host did not respond without error, then
         skip it and continue on to the next host */
      if (strcmp(pDpReply, MT_GOODCURRENTAPPDATA) == 0)
        {
          addDpReplyToProgroupUseArray(aProgroupUseStruct, pDpReplyData);
          memset(strDmReply,  '\0', CMD_LEN);
          sprintf(strDmReply, "%s", MT_GOODCURRENTAPPDATA);
        }
    }      /* END for */

  if (shmdt(pShMemRet) < 0)
    {
      syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-2);
    }

  nBytesWritten = write(outputFd, strDmReply, CMD_LEN);
  if (nBytesWritten != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s: write(outputFd, CMD_LEN) returned %d",
             fnct_name, nBytesWritten);
      return(-3);
    }

  for (i = 0; i < MAX_NUM_PROGROUPS; i++)
    {
      if (strcmp(aProgroupUseStruct[i].strProgroup, "") == 0)
        break;

      if (debug_level > 0)
        {
          syslog(LOG_WARNING, "%s Total use for <%s> == <%d>", fnct_name,
                 aProgroupUseStruct[i].strProgroup,
                 aProgroupUseStruct[i].nUsersCurrentlyInProgroup);
        }
    
      sprintf(strBuf, "%s~%d~", aProgroupUseStruct[i].strProgroup,
              aProgroupUseStruct[i].nUsersCurrentlyInProgroup);

      write(outputFd, strBuf, strlen(strBuf));
    
    } /* END for */

  return(0);
}

/* END DPCURRENTDATA */


/*****************************************************************************/
/* Functions to process USERADMIN and PRINTADMIN Message */

static int  getUserPrinterAdminReply( char  *strReply,
                                      char  *strMessage )

{
  const char  *fnct_name = "getUserPrinterAdminReply():";
  int    i;
  char   strBadReply[CMD_LEN];
  char   strDpReply[CMD_LEN];
  char   strGoodReply[CMD_LEN];

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering..", fnct_name);

  if (strcmp(strMessage, MT_USERADMIN) == 0)
    {
      sprintf(strGoodReply, "%s", MT_GOODUSERADMIN);
      sprintf(strBadReply,  "%s", MT_BADUSERADMIN);
    }
  else
    {
      sprintf(strGoodReply, "%s", MT_GOODPRINTADMIN);
      sprintf(strBadReply,  "%s", MT_BADPRINTADMIN);
    }     /* END if */
  
  memset(strReply, '\0', CMD_LEN);
  strncpy(strReply, strGoodReply, strlen(strGoodReply));
    
  /* For the USERADMIN message, any NAK or non response from a DP causes a
     NAK */
  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      /* There will be no reply from hosts that did not receive this message,
         so skip */
      if (hostDistInfo[i].nHostDistributed == 0)
        continue;

      if (getDpReply(i, strDpReply) != 0)
        {
          syslog(LOG_WARNING, "%s getDpReply failed", fnct_name);
          return(-1);
        }
    
      if (debug_level > 0)
        syslog(LOG_WARNING, "%s Reply from DP[%s] == %s", fnct_name,
               hostDistInfo[i].strHostName, strDpReply);
    
      if (strcmp(strDpReply, strGoodReply) != 0)
        {
          memset(strReply, '\0', CMD_LEN);
          strncpy(strReply, strBadReply, strlen(strBadReply));
          break;
        }

    } /* END for(i) */

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success), strReply == %s",
           fnct_name, strReply);
  
  return(0);
}


/*****************************************************************************/

static int  processDpUserPrinterAdminReplies( int    outputFd,
                                              char  *strMessage )

{
  const char  *fnct_name = "processDpUserPrinterAdminReplies():";
  char  *pShMemRet;
  char  *pDpReply;
  char  *pDpReplyData;
  char   strBadReply[CMD_LEN + 1];
  char   strDmReply[CMD_LEN];
  char   strGoodReply[CMD_LEN + 1];
  char   strHostResponseFileName[500];
  char   strLogFile[MAX_FILENAME_LEN];
  char   strNoReply[CMD_LEN + 1];
  int    i;
  int    nBytesWritten;
  int    nHostResponseFd;
  
  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Entering. strMessage == %s", fnct_name,
             strMessage);
    }
  
  memset(strGoodReply, '\0', CMD_LEN + 1);
  memset(strBadReply,  '\0', CMD_LEN + 1);
  memset(strNoReply,   '\0', CMD_LEN + 1);
  if (strcmp(strMessage, MT_USERADMIN) == 0)
    {
      sprintf(strGoodReply, "%s", MT_GOODUSERADMIN);
      sprintf(strBadReply,  "%s", MT_BADUSERADMIN);
      sprintf(strNoReply,   "%s", MT_NOREPLYUSERADMIN);
      sprintf(strLogFile,   "%s", USR_ADMIN_APPLY_LOG_FILE);
    }
  else
    {
      sprintf(strGoodReply, "%s", MT_GOODPRINTADMIN);
      sprintf(strBadReply,  "%s", MT_BADPRINTADMIN);
      sprintf(strNoReply,   "%s", MT_NOREPLYPRINTADMIN);
      sprintf(strLogFile,   "%s", PRT_ADMIN_APPLY_LOG_FILE);
    } /* END if */

  memset(strDmReply, '\0', CMD_LEN);
  if (getUserPrinterAdminReply(strDmReply, strMessage) != 0)
    {
      syslog(LOG_WARNING, "%s getUserPrinterAdminReply failed!", fnct_name);
      return(-1);
    }

  nBytesWritten = write(outputFd, strDmReply, CMD_LEN);
  if (nBytesWritten != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s write(outputFd, CMD_LEN) returned %d!",
             nBytesWritten, fnct_name);
      return(-2);
    }

  pShMemRet = shmat(nShmIdReturn, 0, 0777);
  if ((int)pShMemRet == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE)) */
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s",
             fnct_name, errno, strerror(errno));
      return(-3);
    }

  /* Send host, status for each host that the message was sent to
     The host, status info will be sent in bundled format */
  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      /* Skip the hosts that did not get the message */
      if (hostDistInfo[i].nHostDistributed == 0)
        continue;

      pDpReply = pShMemRet + getSharedMemoryOffset(i);
      pDpReplyData = pDpReply + CMD_LEN;

      if (debug_level > 0)
        syslog(LOG_WARNING, "%s DP[%s] Reply == %s", fnct_name,
               hostDistInfo[i].strHostName, pDpReply);

      if (strcmp(pDpReply, strGoodReply) == 0 ||
          strcmp(pDpReply, strBadReply) == 0 )
        {
          write(outputFd, hostDistInfo[i].strHostName,
                strlen(hostDistInfo[i].strHostName));
          write(outputFd, ":", sizeof(char));
          write(outputFd, pDpReply, strlen(pDpReply));
          write(outputFd, ":", sizeof(char));
          if (strlen(pDpReplyData) == 0)
            {
              write(outputFd, "N", sizeof(char));
            }
          else
            {
              write(outputFd, "Y", sizeof(char));
              sprintf(strHostResponseFileName, "/%s/%s/%s.%s", NTCSS_HOME_DIR,
                      NTCSS_LOGS_DIR, strLogFile,
                      hostDistInfo[i].strHostName);
              nHostResponseFd = creat(strHostResponseFileName,
                                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
              if (nHostResponseFd < 0)
                syslog(LOG_WARNING, "%s open(%s) failed with error %d %s",
                       fnct_name, strHostResponseFileName, errno,
                       strerror(errno));
              else
                {
                  write(nHostResponseFd, pDpReplyData, strlen(pDpReplyData));
                  close(nHostResponseFd);
                }
            } /* END if(strlen(pDpReplyData)) */
        }
      else
        if (strcmp(pDpReply, "") != 0)
          { /* Some other reply, send reply */
            write(outputFd, hostDistInfo[i].strHostName,
                  strlen(hostDistInfo[i].strHostName));
            write(outputFd, ":", sizeof(char));
            write(outputFd, pDpReply, strlen(pDpReply));
            write(outputFd, ":", sizeof(char));
            write(outputFd, "N", sizeof(char)); 
          }
        else
          { /* No reply */
            write(outputFd, hostDistInfo[i].strHostName,
                  strlen(hostDistInfo[i].strHostName));
            write(outputFd, ":", sizeof(char));
            write(outputFd, strNoReply, strlen(strNoReply));
            write(outputFd, ":", sizeof(char));
            write(outputFd, "N", sizeof(char)); 
          } /* END if */

      write(outputFd, ":", sizeof(char));

    } /* END for */
  
  /* Detach from shared memory */
  if (shmdt(pShMemRet) < 0)
    {
      syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-4);
    }

  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Wrote out %d bytes for msg reply data.",
             fnct_name, nBytesWritten);
      syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);
    }

  return(0);

}


/*****************************************************************************/
/* Functions to process MT_AUTHUSER Message */

static int  getAuthUserReply( char  *strReply,
                              int   *pnTotalDpRecs )

{
  int    i;
  int    nDpRecs;
  char  *pShMemRet;
  char   strDpReply[CMD_LEN];
  const char  *fnct_name = "getAuthUserReply():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering", fnct_name);
  
  *pnTotalDpRecs = 0;
  memset(strReply, '\0', CMD_LEN);
  strncpy(strReply, MT_GOODAUTHUSER, strlen(MT_GOODAUTHUSER));
    
  /* For the AUTHUSER message, any NAK from a DP causes a NAK */
  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      /* Do not examine reply from hosts that did not receive this message */
      if (hostDistInfo[i].nHostDistributed == 0)
        continue;

      if (getDpReply(i, strDpReply) != 0)
        {
          syslog(LOG_WARNING, "%s getDpReply failed!", fnct_name);
          return(-1);
        }
    
      if (debug_level > 0)
        syslog(LOG_WARNING, "%s Reply from DP[%s] == %s", fnct_name,
               hostDistInfo[i].strHostName, strDpReply);

    if (strcmp(strDpReply, MT_GOODAUTHUSER) == 0)
      {
        pShMemRet = shmat(nShmIdReturn, 0, 0777);
        if ((int)pShMemRet == -1) 
        /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
            (errno == EINVAL) || (errno == EMFILE)) */
          {
            syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
                   errno, strerror(errno));
            return(-2);
          }

        unbundleData(pShMemRet + getSharedMemoryOffset(i) + CMD_LEN, "I",
                     &nDpRecs);
        *pnTotalDpRecs += nDpRecs;

        /* Finished with shared memory so detach from it */
        if (shmdt(pShMemRet) < 0)
          {
            syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
                   errno, strerror(errno));
            return(-3);
          }
      }       /* END if */
    
    if (strcmp(strDpReply, MT_BADAUTHUSER) == 0)
      {
        memset(strReply, '\0', CMD_LEN);
        strncpy(strReply, MT_BADAUTHUSER, strlen(MT_BADAUTHUSER));
        break;
      }

    }  /* END for(i) */

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success), strReply == %s,"
          " *pnTotalDpRecs == %d", fnct_name, strReply, *pnTotalDpRecs);

  return(0);

}

/*****************************************************************************/

static int  processDpAuthUserReplies( int    outputFd,
                                      char  *strMessage )

{
  char  *pCh;
  char  *pDpReply;
  char  *pDpReplyData;
  char  *pShMemRet;
  char   strDmReply[CMD_LEN];
  char   strNumRecs[50];
  int    i;
  int    nBytesWritten;
  int    nTotalDpRecs = 0;
  const char  *fnct_name = "processDpAuthUserReplies():";

  /* done only to get rid of compiler warning about unused parameters */
  strMessage=strMessage;
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  memset(strDmReply, '\0', CMD_LEN);
  if (getAuthUserReply(strDmReply, &nTotalDpRecs) != 0)
    {
      syslog(LOG_WARNING, "%s getAuthUserReply failed!", fnct_name);
      return(-1);
    }

  nBytesWritten = write(outputFd, strDmReply, CMD_LEN);
  if (nBytesWritten != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s write(outputFd, CMD_LEN) returned %d",
             fnct_name, nBytesWritten);
      return(-2);
    }

  /* Only send trailing data if MT_GOODAUTHUSER is the DM's reply */
  if (strcmp(strDmReply, MT_GOODAUTHUSER) == 0)
    {
      /* First send the number of records */
      memset(strNumRecs, '\0', 50);
      bundleData(strNumRecs, "I", nTotalDpRecs);
      strcat(strNumRecs, AUTHUSER_SEP_CHAR_STRING);
      nBytesWritten = write(outputFd, strNumRecs, strlen(strNumRecs));
      
      pShMemRet = shmat(nShmIdReturn, 0, 0777);
      if ((int)pShMemRet == -1) 
      /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
          (errno == EINVAL) || (errno == EMFILE)) */
        {
          syslog(LOG_WARNING, "%s shmat failed with error %d %s",
                 fnct_name, errno, strerror(errno));
          return(-3);
        }

      for (i = 0; i < getNumberDistributorProcesses(); i++)
        {
          pDpReply = pShMemRet + getSharedMemoryOffset(i);
          pDpReplyData = pDpReply + CMD_LEN;
          
          if (debug_level > 0)
            syslog(LOG_WARNING, "%s DP[%s] Reply == %s", fnct_name,
                   hostDistInfo[i].strHostName, pDpReply);

          /* Only write out data for DPs that returned MT_GOODAUTHUSER */
          if (strcmp(pDpReply, MT_GOODAUTHUSER) == 0)
            {
              if (debug_level > 0)
                syslog(LOG_WARNING, "%s DP[%s] data == %.150s", fnct_name,
                       hostDistInfo[i].strHostName, pDpReplyData);

              /* We don't want to write out each DPs record count, since
                 we already tallied that up above and sent it, so
                 we use pCh to go past the record count for each DP's
                 reply data. */
              pCh = pDpReplyData;
              while (*pCh != AUTHUSER_SEP_CHAR)
                pCh++;
              pCh++;
              nBytesWritten += write(outputFd, pCh, strlen(pCh));
            } /* END if */
        } /* END for */

      if (shmdt(pShMemRet) < 0)
        {
          syslog(LOG_WARNING, "%s shmdt failed with error %d %s", fnct_name,
                 errno, strerror(errno));
          return(-4);
        }

    } /* END if */
  
  if (debug_level > 0)
    { 
      syslog(LOG_WARNING, "%s Wrote out %d bytes for msg reply data",
             fnct_name, nBytesWritten);
      syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);
    }

  return(0);

}


/*****************************************************************************/

static int  processGetMasterFilesReplies( int    outputFd,
                                          char  *strMessage )

{
  char  strMsgReply[CMD_LEN];
  int   nBytesWritten;

  /* done only to get rid of compiler warning about unused parameters */
  strMessage = strMessage;

  memset(strMsgReply, '\0', CMD_LEN);
  memcpy(strMsgReply, MT_GOODGETMASTERFILES, strlen(MT_GOODGETMASTERFILES));

  nBytesWritten = write(outputFd, strMsgReply, CMD_LEN);
  if (nBytesWritten != CMD_LEN)
    {
      syslog(LOG_WARNING, "processGetMasterFilesReplies(): Only wrote %d "
             "of the expected %d bytes of the reply message!",
             nBytesWritten, CMD_LEN);
      return(-1);
    }

  return(0);
}

/*****************************************************************************/
/* The function determines and sends the correct response back across the 
   pipe. */

static int  processDpNisMapUpdateReplies( int    outputFd,
                                          char  *strMessage )

{
  char  *pDpReply;
  char  *pDpReplyData;
  char  *pShMemRet;
  char   strDmReply[CMD_LEN];
  int    i;
  int    nBytesWritten;
  const char  *fnct_name = "processDpNisMapUpdateReplies():";

  /* done only to get rid of compiler warning about unused parameters */
  strMessage=strMessage;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  memset(strDmReply, '\0', CMD_LEN);
  strncpy(strDmReply, MT_DONENISUPDATE, strlen(MT_DONENISUPDATE));

  nBytesWritten = write(outputFd, strDmReply, CMD_LEN);
  if (nBytesWritten != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s write(outputFd, CMD_LEN) returned %d",
             fnct_name, nBytesWritten);
      return(-2);
    }

  /* Check the responses the DPs gathered */
  pShMemRet = shmat(nShmIdReturn, 0, 0777);
  if ((int)pShMemRet == -1)
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
        (errno == EINVAL) || (errno == EMFILE)) */
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s",
             fnct_name, errno, strerror(errno));
      return(-3);
    }
  
  for (i = 0; i < getNumberDistributorProcesses(); i++)
     {
       /* Do not examine reply from hosts that did not receive this message */
       if (hostDistInfo[i].nHostDistributed == 0)
         continue;

       pDpReply = pShMemRet + getSharedMemoryOffset(i);
       pDpReplyData = pDpReply + CMD_LEN;

       /* Only syslog info from DPs that returned MT_BADNISUPDATE */
       if (strcmp(pDpReply, MT_BADNISUPDATE) == 0)
         {
           if (debug_level > 0)
             syslog(LOG_WARNING, "%s DP[%s] data == %.150s", fnct_name,
                    hostDistInfo[i].strHostName, pDpReplyData);
         } 
       else if (strcmp(pDpReply, MT_GOODNISUPDATE) != 0)
         {
           if (debug_level > 0)
             syslog(LOG_WARNING, "%s DP[%s] could not read response <%.150s>",
                    fnct_name, hostDistInfo[i].strHostName, pDpReply);
         }
      } /* END for */

  if (shmdt(pShMemRet) < 0)
    {
      syslog(LOG_WARNING, "%s shmdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-4);
    }


  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Wrote out %d bytes for msg reply data",
             fnct_name, nBytesWritten);
      syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);
    }

  return(0);

}

/*****************************************************************************/
/* The function determines and sends the correct response back across the pipe.

 NOTE : This function needs cleaned up!
   And I'm sure this function is not correct for writing the
   distributor results to the socket.
*/

static int  processDistributorResults( char  *strMessage )

{
  int        outputFd;
  DMfuncPtrStruct *cmd_sptr;
  const char  *fnct_name = "processDistributorResults():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s..", fnct_name);

  outputFd = open(DM_TO_APP_PIPE, O_WRONLY);
  if (outputFd < 0)
    {
      syslog(LOG_WARNING, "%s Failed to open the DM_TO_APP_PIPE!"
             "  Errno %d - %s", fnct_name, errno, strerror(errno));
      return(-1);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Successfully opened DM_TO_APP_PIPE.", fnct_name);

  /* Get the response-listening command to execute for this message type.. */
  cmd_sptr = get_command_data(strMessage, dm_commands);
  if (!cmd_sptr)
    return(-2);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s get_command_data returned success.", fnct_name);
      
  /* Send the correct response back to the distributer. */
  if (cmd_sptr->func_ptr(outputFd, strMessage) != 0)
    {
      syslog(LOG_WARNING, "%s cmd_sptr->funct_ptr(%s) failed", fnct_name,
             strMessage);
      close(outputFd);
      return(-3);
    }

  if (debug_level > 0)
      syslog(LOG_WARNING, "%s Call to cmd_sptr->funct_ptr successful.",
             fnct_name);

  /* If write fails, the listener may be gone anyway. */
  if (write(outputFd, "", 1) != 1)
    syslog(LOG_WARNING, "%s Failed to write null terminator to the pipe!",
           fnct_name);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Wrote out NULL terminator.", fnct_name);
  
  close(outputFd);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success).", fnct_name);

  return(0);
  
}


/*****************************************************************************/

static int  startDistributorProcesses( void )

{
  const char  *fnct_name = "startDistributorProcesses():";
  int    i;
  pid_t  pid = 0;
  char  *command_args[10];
  /*char   strDebuglev[20]; */
  char   strDistributorIndex[5];
  char   strDistributorProgramName[MAX_CMD_LEN];
  char   tack_h[] = "-h";
  char   tack_s[] = "-s";
  /*char   tack_debug[] = "-debug";*/
  char   strLocalHostName[500];

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  sprintf(strDistributorProgramName, "%s/%s/ntcss_msg_dist", NTCSS_HOME_DIR,
          NTCSS_BIN_DIR);
  command_args[0] = strDistributorProgramName;
  command_args[1] = tack_h;
  command_args[2] = strLocalHostName;
  command_args[3] = tack_s;
  command_args[4] = strDistributorIndex;

  /* Null out the rest of the array entries.. */
  for (i = 5; i < 11; i++)
     command_args[i] = NULL;

  /* 
  if (debug_level > 0)
    {
      sprintf(strDebuglev, "%i", debug_level);
      command_args[5] = tack_debug;
      command_args[6] = strDebuglev;
    }
  */

  for (i = 0; i < getNumberDistributorProcesses() ; i++)
    {
      /* start a distributor for each host we need to talk to */
      sprintf(strLocalHostName, "%s", hostDistInfo[i].strHostName);
      sprintf(strDistributorIndex, "%d", i);  

      if (debug_level > 0)
        {
          syslog(LOG_WARNING, "%s Starting DP for %s: %s %s %s %s %s",
                 fnct_name, hostDistInfo[i].strHostName, command_args[0],
                 command_args[1], command_args[2], command_args[3],
                 command_args[4]);
        }
    
      pid = fork();
      if (pid < 0)
        return(-1);
      else {
        if(pid == 0)
          {
            /* Child execs the ntcss_msg_dist */
            execv(strDistributorProgramName, command_args);
            /* if execv returns then an error occurred, so return with error */
            syslog(LOG_WARNING, "%s execv failed for %s with error %d - %s",
                   fnct_name, hostDistInfo[i].strHostName, errno,
                   strerror(errno));
            return(-2);
          }
        else /* parent continues */
          hostDistInfo[i].pid = pid;
      }
    } /* END for */

  /* nChildrenToReap = getNumberDistributorProcesses(); */

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
  
}


/*****************************************************************************/

static int  getSharedMemoryOffset( int  nHostIndex )

{
  const char  *fnct_name = "getSharedMemoryOffset():";
  int    nReturn;
  void  *pShMem;
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  pShMem = shmat(nShmIdReturnOffset, 0, 0777);
  if ((int)pShMem == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))*/
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
           errno, strerror(errno));
      return(-1);
    }

  nReturn = *((int *)pShMem + nHostIndex);
  
  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shmdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-2);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with %d Success", fnct_name, nReturn);

  return(nReturn);
  
}


/*****************************************************************************/

static int  storeMessageInSharedMemory( int    nPort,
                                        char  *pMessage,
                                        int    nSize )

{
  const char  *fnct_name = "storeMessageInSharedMemory():";
  char  *pShMemMsg;
  char   strFormat[50];
  char   strPortNumber[10];

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  pShMemMsg = shmat(nShmIdMsg, 0, 0777);
  if ((int)pShMemMsg == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))*/
    {
      syslog(LOG_WARNING, "%s Failed to attach to shared memory! Errno %d "
             "- %s", fnct_name, errno, strerror(errno));
      return(-1);
    }
  
  /* Dynamically create the string format based on the port length. */
  /* The string format will look like "%.<PORT_LEN>d". */
  sprintf(strFormat, "%%.%dd", PORT_LEN);
  sprintf(strPortNumber, strFormat, nPort);

  memset(pShMemMsg, '\0', nSize);
  memcpy(pShMemMsg, strPortNumber, PORT_LEN);
  memcpy(pShMemMsg + PORT_LEN, pMessage, nSize);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Message size is %d bytes.", fnct_name, nSize);

  if (shmdt(pShMemMsg) < 0)
    {
      syslog(LOG_WARNING, "%s Failed to detach from shared memory! Erno %d "
             "- %s", fnct_name, errno, strerror(errno));
      return(-2);
    }
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/

void  freeReturnBuffers( void )

{
  const char  *fnct_name = "freeReturnBuffers():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  if (nShmIdReturn > 0)
    {
      if (shmctl(nShmIdReturn, IPC_RMID, 0) < 0)
        syslog(LOG_WARNING, "%s shmctl failed with error %d %s", fnct_name,
               errno, strerror(errno));
      else
        nShmIdReturn = -1;
    }        /* END if */
  
  if (nShmIdReturnOffset > 0)
    {
      if (shmctl(nShmIdReturnOffset, IPC_RMID, 0) < 0)
        syslog(LOG_WARNING, "%s shmctl failed with error %d %s", fnct_name,
               errno, strerror(errno));
      else
        nShmIdReturnOffset = -1;
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

} 


/*****************************************************************************/

static int  determineNumberReturnInstances( const char  *strMessage,
                                            const char  *strHost )

{
  const char  *fnct_name = "determineNumberReturnInstances():";
  
  /* done only to get rid of compiler warning about unused parameters: */
  if (strMessage != strMessage)
    if(strHost == strHost)  /* This should never run. */
      return(1);
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 1 (Success)", fnct_name);

  return(1);
} 


/*****************************************************************************/

static int  allocateReturnBuffers( char *strMessage )

{
  const char  *fnct_name = "allocateReturnBuffers():";
  char  *pShMem = NULL;
  int    i;
  int    nReturnBufferSize;
  int   *offsetArray;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  /* Create the offset array */
  offsetArray = malloc(getNumberDistributorProcesses() * sizeof(int));
  offsetArray[0] = 0;

  nReturnBufferSize = determineMessageReturnSize(strMessage) * 
    determineNumberReturnInstances(strMessage, hostDistInfo[0].strHostName);

  for (i = 1; i < getNumberDistributorProcesses(); i++)
    {
      offsetArray[i] = offsetArray[i - 1] +
        (determineMessageReturnSize(strMessage) * 
         determineNumberReturnInstances(strMessage,
                                        hostDistInfo[i - 1].strHostName));

      nReturnBufferSize += (determineMessageReturnSize(strMessage) *
                            determineNumberReturnInstances(strMessage,
                                                           hostDistInfo[i].strHostName));
    } /* END for */


  /* Store the offset array in shared memory */
  nShmIdReturnOffset = shmget(myShmReturnOffsetKey,
                              getNumberDistributorProcesses() * sizeof(int),
                              IPC_CREAT | 0777);

  if (nShmIdReturnOffset < 0)
    {
      syslog(LOG_WARNING, "%s shmget failed with error %d %s", fnct_name,
             errno, strerror(errno));
      free(offsetArray);
      return(-1);
    }
  
  pShMem = shmat(nShmIdReturnOffset, 0, 0777);
  if ((int)pShMem == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE))*/
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
             errno, strerror(errno));
      free(offsetArray);
      return(-2);
    }

  memcpy(pShMem, offsetArray, getNumberDistributorProcesses() * sizeof(int));
  /* We are finished with the offsetArray variable, so free that memory */
  free(offsetArray);

  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-3);
    }

  /* Create and initialize the return buffers */  
  nShmIdReturn = shmget(myShmReturnKey, nReturnBufferSize, IPC_CREAT | 0777);
  if (nShmIdReturn < 0)
    {
      syslog(LOG_WARNING, "%s shmget failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-4);
    }

  pShMem = shmat(nShmIdReturn, 0, 0777);
  if ((int)pShMem == -1) 
  /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
      (errno == EINVAL) || (errno == EMFILE)) */
    {
      syslog(LOG_WARNING, "%s shmat failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-5);
    }

  memset(pShMem, 0, nReturnBufferSize);

  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shdt failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-6);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/

void  freeMessageBuffer( void )

{
  const char  *fnct_name = "freeMessageBuffer():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);
  
  if (nShmIdMsg > 0)
    {
      if (shmctl(nShmIdMsg, IPC_RMID, 0) < 0)
        syslog(LOG_WARNING, "%s shmctl failed with error %d %s", fnct_name,
               errno, strerror(errno) );
      else
        nShmIdMsg = -1;
    }
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

}


/*****************************************************************************/

static int  allocateMessageBuffer( int  nSize )

{
  const char  *fnct_name = "allocateMessageBuffer():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s.", fnct_name);

  nShmIdMsg = shmget(myShmMsgKey, nSize, IPC_CREAT | 0777);
  if (nShmIdMsg < 0)
    {
      syslog(LOG_WARNING, "%s shmget(%d, %d)  failed with error %d %s",
             fnct_name, myShmMsgKey, nSize, errno, strerror(errno) );
      return(-1);
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/

static int  determineMessagePort( char  *strMessage )

{
  const char  *fnct_name = "determineMessagePort():";
  int    nReturnPort = 0;

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering with arg (%s)", fnct_name, strMessage);

  if (strncmp(strMessage, MT_AUTHUSER, DM_MSG_LEN) == 0)
    {
      nReturnPort = DSK_SVR_PORT;
    }
  else if (strncmp(strMessage, MT_USERADMIN, DM_MSG_LEN) == 0)
    {
      nReturnPort = DB_SVR_PORT;
    }
  else if (strncmp(strMessage, MT_PRINTADMIN, DM_MSG_LEN) == 0)
    {
      nReturnPort = DB_SVR_PORT;
    }
  else if (strncmp(strMessage, "DPCURRENTAPPDATA", DM_MSG_LEN) == 0)
    {
      nReturnPort = DB_SVR_PORT;
    }
  else if (strncmp(strMessage, MT_GETMASTERFILES, DM_MSG_LEN) == 0)
    {
      nReturnPort = DB_SVR_PORT;
    }
  else if (strncmp(strMessage, MT_NISMAPUPDATE, DM_MSG_LEN) == 0)
    {
      nReturnPort = DB_SVR_PORT;
    }

  if (nReturnPort == 0)
    syslog(LOG_WARNING, "%s unknown message type (%s)!", fnct_name,
           strMessage);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with %d (0 == Failure, Non Zero "
           "Success) with msg arg (%s)", fnct_name, nReturnPort, strMessage);

  return(nReturnPort);
}


/*****************************************************************************/
/* Returns the number of bytes to allocate for the remote host's response.
   Thes should really be mathematically caluclated based on field size defines
   and MAX_xxxx limits defined in  Ntcss.h.
*/

static int  determineMessageReturnSize( const char  *strMessage )

{
  const char  *fnct_name = "determineMessageReturnSize():";
  int   nReturnMsgSize = -1;

  if (strncmp(strMessage, MT_AUTHUSER, DM_MSG_LEN ) == 0)
    {
      nReturnMsgSize = 500;
    }

  else if (strncmp(strMessage, MT_USERADMIN, DM_MSG_LEN ) == 0)
    {
      nReturnMsgSize = 1000;
    }

  else if (strncmp(strMessage, MT_PRINTADMIN, DM_MSG_LEN ) == 0)
    {
      nReturnMsgSize = 1000;
    }

  else if (strncmp(strMessage, "DPCURRENTAPPDATA", DM_MSG_LEN ) == 0)
    {
      nReturnMsgSize = 1000;
    }

  else if (strncmp(strMessage, MT_GETMASTERFILES, DM_MSG_LEN) == 0)
    {
      nReturnMsgSize = 1000;
    }

  else if (strncmp(strMessage, MT_NISMAPUPDATE, DM_MSG_LEN) == 0)
    {
      nReturnMsgSize = 500;
    }


  if (nReturnMsgSize == -1)
    syslog(LOG_WARNING, "%s unknown message(%s)", fnct_name, strMessage);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with %d. (-1 == Failure)", fnct_name,
           nReturnMsgSize);
  
  return(nReturnMsgSize);
}


/*****************************************************************************/

static int  releaseDistributorsAndWait( char  *strHostList,
                                        char  *strMsg )

{
  const char  *fnct_name = "releaseDistributorsAndWait():";
  int     nHowManyDistributorsToRun = 0;
  struct sembuf   semWaitOp;
  struct sembuf  *semReleaseOps;
  union semun {
     int              val;
     struct semid_ds *buf;
     ushort          *array;
  } sem_arg;

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering %s", fnct_name);

  semReleaseOps = malloc(sizeof(struct sembuf) * 
                         getNumberDistributorProcesses());
  if (semReleaseOps == NULL)
    {
      syslog(LOG_WARNING, "%s Malloc failed!", fnct_name);
      return(-1);
    }

  sem_arg.val = 0;
  if (semctl(nSemIdDistComplete, 0, SETVAL, sem_arg) < 0)
/*
  if (semctl(nSemIdDistComplete, 0, SETVAL, 0) < 0)
*/
    {
      syslog(LOG_WARNING, "%s semctl failed with error %d! - %s", fnct_name,
             errno, strerror(errno));
      return(-2);
    }

  nHowManyDistributorsToRun = setDistributorsToRun(semReleaseOps, strHostList);

  /* Release semaphores and let distributors process the current message */
  if (semop(nSemIdReleaseDist, semReleaseOps,
            getNumberDistributorProcesses()) < 0)
    {
      syslog(LOG_WARNING, "%s semop failed with errno %d! - %s", fnct_name,
             errno, strerror(errno));
      free(semReleaseOps);
      return(-3);
    }

  /* Set proper distribution timeout for this type of message. */
 /*  alarm(getDmAlarmTimeoutValue(strMsg) * 2);   wait twice as long as DPs */
  alarm(getDmAlarmTimeoutValue(strMsg)+2); /* wait twice as long as DPs */

  semWaitOp.sem_num = 0;
  semWaitOp.sem_op  = -nHowManyDistributorsToRun;
  semWaitOp.sem_flg = 0;
    
  /* Do not print error message if this failed because the alarm went off */
  if ( (semop(nSemIdDistComplete, &semWaitOp, 1) < 0) &&
       (errno != EINTR) )
    {
      syslog(LOG_WARNING, "%s semop failed with error %d! - %s", fnct_name,
             errno, strerror(errno));
      free(semReleaseOps);
      alarm(0);
      return(-4);
    }

  free(semReleaseOps);
  alarm(0);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);

  return(0);
}


/*****************************************************************************/
/* Set up semaphores and return the number of hosts in the host list. */

static int  setDistributorsToRun( struct sembuf  *pSemReleaseOps,
                                  char           *strHostList )
{
  const char  *fnct_name = "setDistributorsToRun():";
  int    i;
  int    nHowManyToRun = 0;
  char  *strHost;

  
  if (debug_level > 0)
      syslog(LOG_WARNING, "Entering %s strHostList == <%s>", fnct_name,
             strHostList);

  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      pSemReleaseOps[i].sem_flg = 0;
      pSemReleaseOps[i].sem_num = i;
      if (strcmp(strHostList, "ALLHOSTS") == 0)
        {
          hostDistInfo[i].nHostDistributed = 1;
          pSemReleaseOps[i].sem_op = 1;
          nHowManyToRun++;
        }
      else
        {
          hostDistInfo[i].nHostDistributed = 0;
          pSemReleaseOps[i].sem_op = 0;
        }
    }        /* END for */

  /* If strHostList is "ALLHOSTS" then this loop will not execute */
  strHost = strtok(strHostList, DM_HOST_SEP_CHARS_STRING);
  while (strHost)
    {
      for (i = 0; i < getNumberDistributorProcesses(); i++)
        {  
          if (strcmp(strHost, hostDistInfo[i].strHostName) == 0)
            {
              pSemReleaseOps[i].sem_op = 1;
              hostDistInfo[i].nHostDistributed = 1;
              nHowManyToRun++;
            }
        }                               /* END for */
      strHost = strtok(NULL, ", ");
    }                                   /* END while */

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0 (Success)", fnct_name);
  
  return(nHowManyToRun);
}


/*****************************************************************************/

static int  getNumberDistributorProcesses( void )

{
  return(nHowManyDistributors);
}


/*****************************************************************************/

void  dm_reaper( int  signal_num )
     
{
  const char  *fnct_name = "dm_reaper():";
  int   nPid;
  int   nStoreErrno;
  /* union wait  status; */
  int   status;

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering...", fnct_name);

  nStoreErrno = errno;

  nPid = wait3(&status, WNOHANG, /*(struct rusage *)*/ NULL);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Reaped process %d.", fnct_name, nPid);

  if (signal(SIGCHLD, dm_reaper) == SIG_ERROR)
    syslog(LOG_WARNING, "%s signal(SIGCHLD) returned SIG_ERROR\n", fnct_name);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

  signal_num = signal_num;
  errno = nStoreErrno;
  return;

}


/*****************************************************************************/

void  sig_hup( int  signo )

{
  const char  *fnct_name = "sig_hup():";
  int nStoreErrno;

  signo = signo;

  nStoreErrno = errno;

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering...", fnct_name);

  if (signal(SIGHUP, sig_hup) == SIG_ERROR)
    syslog(LOG_WARNING, "%s signal(SIGHUP) returned SIG_ERROR", fnct_name);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

  errno = nStoreErrno;
  return;
}

/*****************************************************************************/

void  sig_usr1( int  signo )

{
  const char  *fnct_name = "sig_usr1():";
  int   nStoreErrno;

  signo = signo;

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering...", fnct_name);

  nStoreErrno = errno;

  /* This will cause the loop in main to terminate */
  nContinue = 0;
  
  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

  errno = nStoreErrno;
}


/*****************************************************************************/

void  debugSignal( int  signo )

{
  int   nStoreErrno;  /* Used to preserve previous errno. */
  int   i;

  signo = signo;      /* Rids us of "unused var" compile warnings. */

  nStoreErrno = errno;

  signal(SIGUSR2, debugSignal);

  /* Commented out to get rid of "levels" */
  /*
  if (debug_level == 0)
    {
      syslog(LOG_WARNING, "Turning debug on to Level 1.");
      debug_level = 1;
      errno = nStoreErrno;
      return;
    }

  if (debug_level < 10)
    {
      syslog(LOG_WARNING, "Increasing debug from level %i to Level 10.",
             debug_level);
      debug_level = 10;
      errno = nStoreErrno;
      return;
    }

  if (debug_level < 20)
    {
      syslog(LOG_WARNING, "Increasing debug from level %i to Level 20.",
             debug_level);
      debug_level = 20;
      errno = nStoreErrno;
      return;
    }

  if (debug_level < 30)
    {
      syslog(LOG_WARNING, "Turning debug off.");
      debug_level = 0;
      errno = nStoreErrno;
      return;
    }
  */

  if (debug_level == 0) {
     syslog(LOG_WARNING, "Turning debug on.");
     debug_level = 30;
  }
  else {
     syslog(LOG_WARNING, "Turning debug off.");
     debug_level = 0;
  }

  /* Send signal to DPs to switch debug state */
/*
  for (i = 0; i < getNumberDistributorProcesses() ; i++) 
     if (kill(hostDistInfo[i].pid, SIGUSR2) != 0)
        syslog(LOG_WARNING, "Error sending debug signal to DP:%s - %s", 
               hostDistInfo[i].strHostName, strerror(errno)); 
*/
   for (i = 0; i < getNumberDistributorProcesses() ; i++) {
      if (hostDistInfo[i].pid >= 0) {
         if (kill(hostDistInfo[i].pid, SIGUSR2) != 0)
            syslog(LOG_WARNING, "Error sending debug signal to DP:%s - %s",
                   hostDistInfo[i].strHostName, strerror(errno));
      }
      else
         syslog(LOG_WARNING, "Attempted to kill DP<%s> with pid = %d",
                hostDistInfo[i].strHostName, hostDistInfo[i].pid);
   }


  errno = nStoreErrno;
  return;
}


/*****************************************************************************/

void  sig_alarm( int  signo )

{
  const char  *fnct_name = "sig_alarm():";
  ushort  *semValues;
  uint   semValue = 0;
  int  nStoreErrno;

  union semun {
     int              val;
     struct semid_ds *buf;
     ushort          *array;
  } sem_arg;

  signo = signo;                /* Rids us of "unused var" compile warnings. */

  nStoreErrno = errno;
  syslog(LOG_WARNING, "%s Entering...", fnct_name);

  /* Resest signal handler for SIGALRM */
  if (signal(SIGALRM, sig_alarm) == SIG_ERROR)
    syslog(LOG_WARNING, "%s signal(SIGALARM) returned SIG_ERR", fnct_name);

  /* Reset the semaphores */
/*
  semValues = malloc(sizeof(uint) * nHowManyDistributors);
  memset(semValues, 0, nHowManyDistributors);
*/
  semValues = malloc(sizeof(ushort) * nHowManyDistributors);
  memset((ushort *)semValues, (ushort)0, nHowManyDistributors);
  
  sem_arg.array = semValues;
  if (semctl(nSemIdReleaseDist, 0, SETALL, sem_arg) < 0)
/*
  if (semctl(nSemIdReleaseDist, 0, SETALL, semValues) < 0)
*/
    syslog(LOG_WARNING, "%s semctl failed! --%s", fnct_name, strerror(errno));
       
  sem_arg.val = semValue;
  if (semctl(nSemIdDistComplete, 0, SETVAL, sem_arg) < 0)
/*
  if (semctl(nSemIdDistComplete, 0, SETVAL, semValue) < 0)
*/
    syslog(LOG_WARNING, "%s semctl failed! --%s", fnct_name, strerror(errno));
  
  free(semValues);
  alarm(0);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting.", fnct_name);

  errno = nStoreErrno;
  return;
}


/*****************************************************************************/
/* Set up data structures, semaphores, and session  ID.
*/

static int  initialize( void )

{
  const char  *fnct_name = "initialize():";
  int   i;
  int   nIndex;
  int   nTotalHosts;
  char  strThisMachine[200];
  HostItem  **hostItems;

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering.", fnct_name);

  if (gethostname(strThisMachine, 200) != 0)
    syslog(LOG_WARNING, "%s gethostname failed!", fnct_name);

  hostItems = getHostData(&nTotalHosts, NTCSS_DB_DIR);
  if (nTotalHosts < 0)
    {
      syslog(LOG_WARNING, "%s getHostData returned failed!", fnct_name);
      return(-1);
    }

  /* We only need DPs for Ntcss Master and Authentication servers other than
     this machine */
  nHowManyDistributors = 0;
  for (i = 0; i < nTotalHosts; i++)
    {
      if ( (hostItems[i]->type == NTCSS_MASTER ||
            hostItems[i]->type == NTCSS_AUTH_SERVER) &&
           strcmp(hostItems[i]->hostname, strThisMachine) != 0 )
        {
          nHowManyDistributors++;
        }
    }       /* END for */
  
  if (nHowManyDistributors == 0)
    {
      syslog(LOG_WARNING, "%s No NTCSS Master or NTCSS Auth servers found!",
             fnct_name);
      return(-2);
    }

  hostDistInfo = malloc(sizeof(HostDistInfoStruct) *
                        getNumberDistributorProcesses());

  nIndex = 0;
  for (i = 0; i < nTotalHosts; i++)
    {
      if ( (hostItems[i]->type == NTCSS_MASTER ||
            hostItems[i]->type == NTCSS_AUTH_SERVER) &&
           strcmp(hostItems[i]->hostname, strThisMachine) != 0 )
        {
          hostDistInfo[nIndex].strHostName = malloc(sizeof(char) *
                                                    MAX_HOST_NAME_LEN + 1);
          memset(hostDistInfo[nIndex].strHostName, '\0',
                 MAX_HOST_NAME_LEN + 1);
          sprintf(hostDistInfo[nIndex].strHostName, "%s",
                  hostItems[i]->hostname);
          hostDistInfo[nIndex].nHostDistributed = 0;
          nIndex++;
          if (debug_level > 0)
            syslog(LOG_WARNING, "%s Identified host %s.", fnct_name,
                   hostItems[i]->hostname);
        }    /* END if(hostItems) */
    }     /* END for */

  freeHostData(hostItems, nTotalHosts);

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Identified %d hosts to create distributors "
           "for", fnct_name, getNumberDistributorProcesses());

  nSemIdReleaseDist = semget(mySemReleaseDistKey,
                             getNumberDistributorProcesses(),
                             IPC_CREAT | 0777);
  if (nSemIdReleaseDist < 0)
    {
      syslog(LOG_WARNING, "%s semget failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-3);
    }

  nSemIdDistComplete = semget(mySemDistCompleteKey, 1, IPC_CREAT | 0777);
  if (nSemIdDistComplete < 0)
    {
      syslog(LOG_WARNING, "%s semget failed with error %d %s", fnct_name,
             errno, strerror(errno));
      return(-4);
    }

  if (signal(SIGALRM, sig_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s signal(SIGALARM) returned SIG_ERR", fnct_name);
      return(-5);
    }

  if (signal(SIGUSR1, sig_usr1) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s signal(SIGUSR1) returned SIG_ERR", fnct_name);
      return(-6);
    }

  daemonize();

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with 0(Success)", fnct_name);

  return(0);
}


/*****************************************************************************/

static void  daemonize( void )

{

  if (debug_level > 0)
    syslog(LOG_WARNING, "Entering daemonize");
 
  setsid(); /* Become session leader */
  chdir("/"); 
  umask(0);

  if (debug_level > 0)
    syslog(LOG_WARNING, "Exiting daemonize");

}


/*****************************************************************************/

static void  cleanup( void )

{

  int    i;
  const char  *fnct_name = "cleanup():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering.", fnct_name);

  for (i = 0; i < getNumberDistributorProcesses(); i++)
    {
      free(hostDistInfo[i].strHostName);    
    }
  free(hostDistInfo);
              
  if (semctl(nSemIdReleaseDist, 0, IPC_RMID) != 0)
    {
      syslog(LOG_WARNING, "%s semctl failed to remove Distributor Start"
             " semaphore! --%s", fnct_name, strerror(errno));
    }

  if (semctl(nSemIdDistComplete, 0, IPC_RMID) != 0)
    {
      syslog(LOG_WARNING, "%s semctl failed to remove Distributor "
             "Completion semaphore! --%s", fnct_name, strerror(errno));
    }
  
  freeMessageBuffer();
  freeReturnBuffers();
  closelog();

  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Exiting successfully.", fnct_name);
    }

}


/*****************************************************************************/

static int  checkParameters ( int    argc,
                              char  *argv[] )

{
  int    nReturn = 0;
  const char  *fnct_name = "checkParameters():";

  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Entering..", fnct_name);
    }
      syslog(LOG_WARNING, "%s Entering..", fnct_name);

  switch (argc)
    {
    case 1:
syslog(LOG_WARNING, "%s returning OK..", fnct_name);
      nReturn = 0;
      break;

    case 2:
      if (strcmp(argv[1], "-d") == 0)
        {
          debug_level = 10;
          nReturn = 0;
        }
      else
        {
          nReturn = -1;
        }
      break;

    default:
      nReturn = -1;
      break;

    }   /* END switch */

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Exiting with %d(0 == Success)", fnct_name,
           nReturn);

  return(nReturn);
}


/*****************************************************************************/
/* Prints out help for this program. */

/******* Not currently used!
static void  usage( void )

{
  const char  *fnct_name[] = "usage():";

  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Entering function..", fnct_name);
    }

  printf("ntcss_msg_dist_mngr -d\n");
  printf("\t-d Debug status\n");

  if (debug_level > 0)
    {
      syslog(LOG_WARNING, "%s Exiting with success. ", fnct_name);
    }

}
*******/

/*****************************************************************************/

static int  determineValidMessageType( char *strMessage )

{
  DMfuncPtrStruct *tptr;
  const char  *fnct_name = "determineValidMessageType():";

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Entering with arg (%s)", fnct_name, strMessage);

  tptr = dm_commands;
  while (tptr->cmd)
    {
      if (!strcmp(strMessage, tptr->cmd))
        return(1);
      tptr++;
    }

  if (debug_level > 0)
    syslog(LOG_WARNING, "%s Found invalid message type(%s)", fnct_name,
           strMessage);

  return(0);
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
