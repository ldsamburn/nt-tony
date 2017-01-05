
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* NtcssNisFcts.c */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <pwd.h>
#include <errno.h>
#include <time.h>
#include <rpcsvc/ypclnt.h>
#include <sys/types.h>
#include <rpc/rpc.h>
#include <rpcsvc/yp_prot.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h> /* HPUX 90 system() */
#include <string.h>
#include <sys/wait.h>
#include <sys/utsname.h>

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <StringFcts.h>
#include <ini_Fcts.h>

#define NTCSS_HOSTS_BY_IP_MAP       "ntcss_hosts.byaddr"
#define NTCSS_HOSTS_BY_NAME_MAP     "ntcss_hosts.byname"
#define NTCSS_SYS_CONFIG_BY_TAG_MAP "ntcss_sys_conf.bytag"
#define NTCSS_ADAPTERS_BY_ETHER_MAP "ntcss_adapters.byip"
#define NTCSS_USER_AUTH_SERVER_BY_LOGIN_NAME_MAP "ntcss_user_auth_server.byname"
#define NTCSS_PRINTERS_BY_NAME_MAP             "ntcss_printers.byname"
#define HOST_FIELD_SEP_CHAR_STRING             ":"
#define HOST_RECORD_SEP_CHAR_STRING            "~"
#define ADAPTERS_FIELD_SEP_CHAR_STRING         "~"
#define ADAPTERS_RECORD_SEP_CHAR_STRING        "!"
#define PRINTERS_FIELD_SEP_CHAR_STRING         "~"
#define PRINTERS_RECORD_SEP_CHAR_STRING        "!"
#define NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING ":"
#define PRINTERS_NULL_STRING                   "^"
#define IP_ADDRESS_LEN              14
#define PROCESS_STR_LEN             5
#define HOST_TYPE_STR_LEN           1
#define HOST_REPL_STR_LEN           1
#define NUM_HOST_FIELD_SEPS         4

/* VERY IMPORTANT NOTE: 98-09-04 GWY
   DO NOT free the pointer returned by yp_get_default_domain_name.
   Even though the man pages state that output char ** parameters can
   be freed when the code no longer needs them.
   The output char ** parameters for the other nis client libraries
   (such as char **outval for yp_match)  can and should be freed when the
   code is finished with them.

   This observation was verified using purify. 
*/   

int ntcss_yp_match(char*, char*,
                   char*, int, char*,
                   int*);
#ifdef linux
static int   ntcss_printers_nis_callback(int, char*, int, char*, int, char*);
static int   ntcss_adapters_nis_callback(int, char*, int, char*, int, char*);
static int   ntcss_hosts_nis_callback(int, char*, int, char*, int, char*);
static int   ntcss_yp_match_callback(int, char*, int, char*, int, char*);
#else
static int   ntcss_printers_nis_callback(int, const char*, int, const char*, 
					 int, char*); 
static int   ntcss_adapters_nis_callback(int, const char*, int, const char*,
                                         int, char*);
static int   ntcss_hosts_nis_callback(int, const char*, int, const char*,
				      int, char*);
static int   ntcss_yp_match_callback(int, const char*, int, const char*, 
					int, char*);
#endif
static char *replacePrinterNullString(char*);


/*****************************************************************************/

int  buildAndPushSystemNisMaps( void )

{
  int          pid;
  const char  *fnct_name = "buildAndPushSystemNisMaps():";
  char         strCommand[500];

   return(0);

  if (IAmMaster() == FALSE)
    {
      return(0);
    }

  if (strcmp(NIS_DIR, "") == 0)
    {
      syslog(LOG_WARNING, "%s NIS_DIR not defined!", fnct_name);
      return(-1);
    }

#ifdef HPUX
  sprintf(strCommand, "%s/ypmake", NIS_DIR);
#else
  sprintf(strCommand, "cd %s;make", NIS_DIR);
#endif

  pid = fork();
  if (pid < 0)
    {
      return(-1);
    }
  else
    if(pid == 0)
      {
        /* Child does work calling system to run the push script */
        if (system(strCommand) != 0)
          {
            /* if system returns then an error occurred, so record the error */
            syslog(LOG_WARNING, "%s system(%s) failed!", fnct_name,
                   strCommand);
          }
        exit(0);
      }

  /* parent continues */
  return(0);
}


/*****************************************************************************/

int  buildAndPushNtcssNisMaps( void )

{
  /*int    f_use_db_locks;*/
  int    x,dmpid;
  /*int    i_lock_id;*/
  const char  *fnct_name = "buildAndPushNtcssNisMaps():";
  char   strCommand[500]; /*,strFileName[MAX_FILENAME_LEN];*/
  char  strMsg[CMD_LEN];
  char  strDMResp[MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + CMD_LEN + 4)];
  char  strHostResp[MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + CMD_LEN /*OK/FAIL*/ +
                                    1 /* Responded? Y/N */ +
                                    3 /*delimiters */)];
  char  *strNISTempName; 
  char  strLogsDir[MAX_FILENAME_LEN];

  if (IAmMaster() == FALSE) {
      return(0);
  }

  if (strcmp(PUSH_NTCSS_NIS_MAPS_SCRIPT, "") == 0) {
    syslog(LOG_WARNING, "%s PUSH_NTCSS_NIS_MAPS_SCRIPT not defined!",
           fnct_name);
    return(-1);
  }

  if (strcmp(NIS_DIR, "") == 0) {
    syslog(LOG_WARNING, "%s NIS_DIR not defined!", fnct_name);
    return(-2);
  }

  if (strcmp(NISMAP_DIR, "") == 0) {
    syslog(LOG_WARNING, "%s NISMAP_DIR not defined!", fnct_name);
    return(-3);
  }
 
  
/* ACA --- Jeff, no need to fork anymore 
  if ((pid = fork()) < 0) {
    return(-1);
  } else if(pid == 0) {
*********************/
   

  sprintf(strLogsDir,"%s/%s",NTCSS_HOME_DIR,NTCSS_LOGS_DIR);

  strNISTempName = tempnam(strLogsDir, "NISLIST"); 
  if (strNISTempName == NULL)
    { 
      syslog(LOG_WARNING,"Could not create temporary map list file - %d.",
	     errno);
      return(-4);
    }

  /* Get list of newer NIS maps */
  sprintf(strCommand, "%s/%s/%s > %s", NTCSS_HOME_DIR, NTCSS_BIN_DIR,
	  MAKE_MAP_LIST_SCRIPT, strNISTempName);

  x = system(strCommand);

  if (x != 0)
    {
      remove(strNISTempName);
      return (0);
    }

  sprintf(strCommand, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_BIN_DIR,
          PUSH_NTCSS_NIS_MAPS_SCRIPT);

  /* Build the new NIS maps.. */
  /* Child does work calling system to run the push script */
  x = system(strCommand);

/******
  if (x != 0)
    {
      /* if system returns then an error occurred, so record the error * /
      syslog(LOG_WARNING, "%s system(%s) failed!", fnct_name, strCommand);
      remove(strNISTempName);
      return (-7);
    }
******/
   
  /* no dm running, return */
  dmpid=getDmPid();
  if (dmpid < 0)
     return(0);

  /* Send the NIS List to the DM. */
   memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, MT_NISMAPUPDATE, strlen(MT_NISMAPUPDATE));
       
  /* Clean out resp list */
  memset(strDMResp, '\0',
	 (size_t) (MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + CMD_LEN + 4))); 

  x = sendAdminToDm("ALLHOSTS", strNISTempName, strHostResp, strMsg);

  remove(strNISTempName);

  return(0);


/* ACA --- END FORK 
  } 
  return(0);
****************/
}


/*****************************************************************************/

/* NOTE: The memory allocated by this function for the 
   hostname and ip_address should be freed by the calling 
   code.  */
int  getNtcssHostItemByIpFromNis( HostItem  *pHostItem,
                                  char      *strIp )

{
  const char  *fnct_name = "buildAndPushNtcssNisMaps():";
  char   ntcss_hosts_by_IP_map[] = NTCSS_HOSTS_BY_IP_MAP;
  char   strBuffer[500];
  char  *strDomainName;
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;

  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s!",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s", fnct_name,
           strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }
  
/*
  nErr = yp_match(strDomainName, ntcss_hosts_by_IP_map, strIp,
                  strlen(strIp), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, ntcss_hosts_by_IP_map, strIp,
                  strlen(strIp), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s Ip %s not found", fnct_name, strIp);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s", fnct_name,
             strIp, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
    yp_unbind(strDomainName);
    return(-1);
  } else {
    /* NIS Map entry is in the form hostname:IP:Num Processes:Host 
       Type:Replication Code*/
    nCharIndex = parseWord(0, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->hostname = allocAndCopyString(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->ip_address = allocAndCopyString(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->num_processes = atoi(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->type = atoi(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->repl = atoi(strBuffer);
    
    yp_unbind(strDomainName);
    return(0);
  }
}


/*****************************************************************************/
/* NOTE: The memory allocated by this function for the 
   hostname and ip_address should be freed by the calling 
   code.  */

int  getNtcssHostItemByNameFromNis( HostItem  *pHostItem,
                                    char      *strHostName )

{
  const char  *fnct_name = "getNtcssHostItemByNameFromNis():";
  char   ntcss_hosts_byname_map[] = NTCSS_HOSTS_BY_NAME_MAP;
  char   strBuffer[500];
  char  *strDomainName;
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;

  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s!",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }

  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s!", fnct_name,
           strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }

/*
  nErr = yp_match(strDomainName, ntcss_hosts_byname_map, strHostName,
                  strlen(strHostName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, ntcss_hosts_byname_map, strHostName,
                  strlen(strHostName), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s Name %s not found!", fnct_name, strHostName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s!",
             fnct_name, strHostName, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
    yp_unbind(strDomainName);
    return(-1);
  } else {
    /* NIS Map entry is in the form hostname:IP:Num Processes:
       Host Type:Replication Code*/
    nCharIndex = parseWord(0, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->hostname = allocAndCopyString(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->ip_address = allocAndCopyString(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->num_processes = atoi(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->type = atoi(strBuffer);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           HOST_FIELD_SEP_CHAR_STRING);
    pHostItem->repl = atoi(strBuffer);
    
    yp_unbind(strDomainName);
    return(0);
  }
}


/*****************************************************************************/
#ifdef linux
static int  ntcss_hosts_nis_callback( int          inStatus,
                                      char  *inKey,
                                      int          inKeyLen,
                                      char  *inVal,
                                      int          inValLen,
                                      char        *inData )
#else
static int  ntcss_hosts_nis_callback( int          inStatus,
                                      const char  *inKey,
                                      int          inKeyLen,
                                      const char  *inVal,
                                      int          inValLen,
                                      char        *inData )
#endif
{
  const char  *fnct_name = "ntcss_hosts_nis_callback():";

  /* done to get rid of compiler warning about unused variables */
  inKey = inKey;
  inKeyLen = inKeyLen;

  if (inStatus == YP_TRUE)
    {
      strcat(inData, HOST_RECORD_SEP_CHAR_STRING);
      strncat(inData, inVal, inValLen);
      return(0); /* Return 0 means get next key/value pair */
    }
  else
    {
      /* Log error if it is not YPERR_NOMORE (or YPERR_RPC for now) */
      /* NOTE: We never get YPERR_NOMORE like we expect.  Instead we
         we get YPERR_RPC.  This seems to be a bug in yp_all.  
         A socket is left open each time yp_all is called.
         If the cause of this YPERR_RPC error is found, it will likely
         correct the open socket problem */
      if (inStatus != YPERR_NOMORE  && inStatus != YPERR_RPC)
        {
          syslog(LOG_WARNING, "%s Failed with inStatus of %d - %s", fnct_name,
                 inStatus,  yperr_string(inStatus));
        }
      return(1); /* Return non-zero means don't get the next key/value pair */
    }
}


/*****************************************************************************/

HostItem  **getHostDataFromNis( int  *pnHosts )

{
  HostItem **Hosts;
  const char  *fnct_name = "getHostDataFromNis():";
  char   ntcss_hosts_byname_map[] = NTCSS_HOSTS_BY_NAME_MAP;
  char  *strDomainName;
  char   strFieldData[500];
  char   strHostRecordArray[MAX_NUM_HOSTS][MAX_HOST_NAME_LEN +
                                          IP_ADDRESS_LEN + PROCESS_STR_LEN +
                                          HOST_TYPE_STR_LEN +
                                          HOST_REPL_STR_LEN +
                                          NUM_HOST_FIELD_SEPS + 1];
  char   strHostRecords[MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + IP_ADDRESS_LEN +
                                         PROCESS_STR_LEN + HOST_TYPE_STR_LEN +
                                         HOST_REPL_STR_LEN +
                                         NUM_HOST_FIELD_SEPS) + 1];
  char   strRecord[MAX_HOST_NAME_LEN + IP_ADDRESS_LEN + PROCESS_STR_LEN +
                  HOST_TYPE_STR_LEN + HOST_REPL_STR_LEN +
                  NUM_HOST_FIELD_SEPS + 1];
  int    i;
  int    nCharIndex;
  int    nErr;
  int    nHosts;
  struct ypall_callback  callback;
  
  *pnHosts = -1;
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(NULL);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s", fnct_name,
           strDomainName, nErr, yperr_string(nErr));
    return(NULL);
  }
  
  callback.foreach  = ntcss_hosts_nis_callback;
  strHostRecords[0] = '\0';
  callback.data     = strHostRecords;
  
  /* On yp_all see comment in function ntcss_hosts_nis_callback in this file */
  if (yp_all(strDomainName, ntcss_hosts_byname_map, &callback) != 0) {
    syslog(LOG_WARNING, "%s yp_all failed!", fnct_name);
    *pnHosts = -1;
    yp_unbind(strDomainName);
    return((HostItem **) NULL);
  } else {
    /* Need to move data from strHostRecords to Hosts and set num_hosts */
    nHosts = 0;
    nCharIndex = parseWord(0, strHostRecords, strRecord,
                           HOST_RECORD_SEP_CHAR_STRING);
    while (nCharIndex >= 0) {
      sprintf(strHostRecordArray[nHosts++], "%s", strRecord);
      nCharIndex = parseWord(nCharIndex, strHostRecords,
                             strRecord, HOST_RECORD_SEP_CHAR_STRING);
    } /* END while(strRecord) */
    *pnHosts = nHosts;
    
    /* Now fill in the HostItem array */
    Hosts = (HostItem **) malloc(sizeof(HostItem *) * MAX_NUM_HOSTS);
    
    for (i = 0; i < nHosts; i++) {
      Hosts[i] = (HostItem *) malloc(sizeof(HostItem));
      
      Hosts[i]->marker = 1;
      
      /* NIS Map entry is in the form hostname:IP:Num Processes:
         Host Type:Replication Code*/
      nCharIndex = parseWord(0, strHostRecordArray[i], strFieldData,
                             HOST_FIELD_SEP_CHAR_STRING);
      Hosts[i]->hostname = allocAndCopyString(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strHostRecordArray[i],
                             strFieldData, HOST_FIELD_SEP_CHAR_STRING);
      Hosts[i]->ip_address = allocAndCopyString(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strHostRecordArray[i],
                             strFieldData, HOST_FIELD_SEP_CHAR_STRING);
      Hosts[i]->num_processes = atoi(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strHostRecordArray[i],
                             strFieldData, HOST_FIELD_SEP_CHAR_STRING);
      Hosts[i]->type = atoi(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strHostRecordArray[i],
                             strFieldData, HOST_FIELD_SEP_CHAR_STRING);
      Hosts[i]->repl = atoi(strFieldData);
    }        
    
    yp_unbind(strDomainName);
    return(Hosts);
  }  
}


/*****************************************************************************/

int  getNtcssSysConfValueFromNis( char       **strBuffer,
                                  const char  *strTag )

{
  const char   *fnct_name = "getNtcssSysConfValueFromNis():";
  char    ntcss_sys_config_by_tag_map[] = NTCSS_SYS_CONFIG_BY_TAG_MAP;
  char   *strDomainName;
/*
  char   *strMapEntry;
*/
  char   strMapEntry[1000];
  char   *strTagWithoutSpaces;
  int     i;
  int     nEntryLen;
  int     nErr;
  
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s", fnct_name,
           strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }
  
  strTagWithoutSpaces = allocAndCopyString(strTag);
  for (i = 0; (size_t)i < strlen(strTagWithoutSpaces); i++) {
    if (strTagWithoutSpaces[i] == ' ') {
      strTagWithoutSpaces[i] = '_';
    }
  }
  
/*
  nErr = yp_match(strDomainName, ntcss_sys_config_by_tag_map,
                  strTagWithoutSpaces,
          strlen(strTagWithoutSpaces), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, ntcss_sys_config_by_tag_map,
                  strTagWithoutSpaces,
          strlen(strTagWithoutSpaces), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s Tag %s not found", fnct_name,
             strTagWithoutSpaces);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s",
             fnct_name, strTagWithoutSpaces, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
    free(strTagWithoutSpaces);
    yp_unbind(strDomainName);
    return(-1);
  } else {
    *strBuffer = (char*)malloc(sizeof(char) * (nEntryLen + 1));
    memset(*strBuffer, '\0', (nEntryLen + 1));
    strncpy(*strBuffer, strMapEntry, nEntryLen);
    free(strTagWithoutSpaces);
    yp_unbind(strDomainName);
    return(0);
  }
}


/*****************************************************************************/

int  getNtcssPasswordByLoginNameFromNis( char  *strLoginName,
                                         char  *strPassword )

{

  const char   *fnct_name = "getNtcssPasswordByLoginNameFromNis():";
  char    auth_login_name_map[] = NTCSS_USER_AUTH_SERVER_BY_LOGIN_NAME_MAP;
  char    strBuffer[250];
  char   *strDomainName;
/*
  char   *strMapEntry;
*/
  char   strMapEntry[1000];
  int     nCharIndex;
  int     nEntryLen;
  int     nErr;
  int     nPasswordLen;
  int     nReturn;
  
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }
  
/*
  nErr = yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), strMapEntry, &nEntryLen);

  if (nErr != 0) {
    nReturn = -1;
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s User %s not found!", fnct_name, strLoginName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s", fnct_name,
             strLoginName, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
  } else {
    /* Map is in the form "login_name:auth_server:encrypted_password" */
    nCharIndex = parseWord(0, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);

    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);
    nPasswordLen = strlen(strBuffer);
    if (nPasswordLen != 0) {
      stripNewline(strBuffer);
    }
    sprintf(strPassword, "%s", strBuffer);
    nReturn = 0;   
  }

  yp_unbind(strDomainName);
  return(nReturn);
}


/*****************************************************************************/

int  getNtcssSSNByLoginNameFromNis( char  *strLoginName,
                                    char  *strSSN )

{
  const char  *fnct_name = "getNtcssSSNByLoginNameFromNis():";
  char   auth_login_name_map[] = NTCSS_USER_AUTH_SERVER_BY_LOGIN_NAME_MAP;
  char   strBuffer[250];
  char  *strDomainName;
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;
  int    nReturn;
  int    nSSNLen;
  /* char *decryptedSSN; */
  
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  } 
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }
  
/*
  nErr = yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    nReturn = -1;
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s User %s not found!", fnct_name, strLoginName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s",
             fnct_name, strLoginName, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
  } else {
    /* Map is in the form "login_name:auth_server:encrypted_password:
                                           encrypted_social_security_number" */
    nCharIndex = parseWord(0, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);
    
    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);

    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);

    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);
    nSSNLen = strlen(strBuffer);
    if (nSSNLen != 0) {
      stripNewline(strBuffer);
    }

    /* 
    sprintf(key, "%s%s", strLoginName, strLoginName);
    if (plainDecryptString(key, strBuffer, &decryptedSSN, 0 , 0) != 0) {
       syslog(LOG_WARNING, "getNtcssSSNByLoginNameFromNis: Couldn't convert"
              " SSN!");
       free(strMapEntry);
       return (-1);
    } 
    */

    sprintf(strSSN, "%s", strBuffer);
    /* 
    sprintf(strSSN, "%s", decryptedSSN);
    free(decryptedSSN);
    */
    nReturn = 0;   
  }

  yp_unbind(strDomainName);
  return(nReturn);
}


/*****************************************************************************/

int  getNtcssAuthServerByLoginNameFromNis( char  *strLoginName,
                                           char  *strAuthServer )

{

  const char  *fnct_name = "getNtcssAuthServerByLoginNameFromNis():";
  char   auth_login_name_map[] = NTCSS_USER_AUTH_SERVER_BY_LOGIN_NAME_MAP;
  char   strBuffer[250];
  char  *strDomainName;
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;
  int    nReturn;
  
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  } 
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
       fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }
  
/*
  nErr = ntcss_yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, auth_login_name_map, strLoginName,
                  strlen(strLoginName), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    nReturn = -1;
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s User %s not found", fnct_name, strLoginName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s",
             fnct_name, strLoginName, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
  } else {
    /* Map is in the form "login_name:auth_server:encrypted_password" */
    nCharIndex = parseWord(0, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);

    nCharIndex = parseWord(nCharIndex, strMapEntry, strBuffer,
                           NTCSS_USER_AUTH_SERVER_SEP_CHAR_STRING);
    if (strlen(strBuffer) >= MAX_HOST_NAME_LEN) {
      syslog(LOG_WARNING, "%s Auth server for %s is longer "
             "than MAX_HOST_NAME_LEN", fnct_name, strLoginName);
      /*sprintf(strAuthServer, "");*/
      strAuthServer[0]=0;
      nReturn = -1;
    } else {
      memset(strAuthServer,  '\0', strlen(strBuffer) + 1);
      sprintf(strAuthServer, "%s", strBuffer);
      nReturn = 0;
    }
    
  }

  yp_unbind(strDomainName);
  return(nReturn);
}


/*****************************************************************************/
/******* Adapters functions *******/
/*****************************************************************************/

#ifdef linux
static int  ntcss_adapters_nis_callback( int          inStatus,
                                         char  *inKey,
                                         int          inKeyLen,
                                         char  *inVal,
                                         int          inValLen,
                                         char        *inData )
#else
static int  ntcss_adapters_nis_callback( int          inStatus,
                                         const char  *inKey,
                                         int          inKeyLen,
                                         const char  *inVal,
                                         int          inValLen,
                                         char        *inData )
#endif
{
  const char  *fnct_name = "ntcss_adapters_nis_callback():";

  /* done to get rid of compiler warning about unused variables */
  inKey = inKey;
  inKeyLen = inKeyLen;

  if (inStatus == YP_TRUE)
    {
      strcat(inData, ADAPTERS_RECORD_SEP_CHAR_STRING);
      strncat(inData, inVal, inValLen);
      return(0); /* Return 0 means get next key/value pair */
    }
  else
    {
      /* Log error if it is not YPERR_NOMORE (or YPERR_RPC for now) */
      /* NOTE: We never get YPERR_NOMORE like we expect.  Instead we
         we get YPERR_RPC.  This seems to be a bug in yp_all.  
         A socket is left open each time yp_all is called.
         If the cause of this YPERR_RPC error is found, it will likely
         correct the open socket problem */
      if (inStatus != YPERR_NOMORE  && inStatus != YPERR_RPC)
        {
          syslog(LOG_WARNING, "%s Failed with inStatus of %d - %s",
                 fnct_name, inStatus, yperr_string(inStatus));
        }
      return(1); /* Return non-zero means don't get the next key/value pair */
    }
}


/*****************************************************************************/

AdapterItem  **getAdapterDataFromNis( int  *pnAdapters )

{

  AdapterItem **Adapters;
  const char  *fnct_name = "getAdapterDataFromNis():";
  char   adapters_by_ether_map[] = NTCSS_ADAPTERS_BY_ETHER_MAP;
  char   strAdapterRecordArray[MAX_NUM_ADAPTERS][500];
  char   strAdapterRecords[MAX_NUM_ADAPTERS * (500) + 1];
  char  *strDomainName;
  char   strFieldData[500];
  char   strRecord[500];
  int    i;
  int    nCharIndex;
  int    nAdapters;
  int    nErr;
  struct ypall_callback  callback;
  
  *pnAdapters = -1;
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(NULL);
  } 
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(NULL);
  }  
  
  callback.foreach     = ntcss_adapters_nis_callback;
  strAdapterRecords[0] = '\0';
  callback.data        = strAdapterRecords;
  
  /* On yp_all see comment in function ntcss_adapters_nis_callback
     in this file */
  if (yp_all(strDomainName, adapters_by_ether_map, &callback) != 0) {
    syslog(LOG_WARNING, "%s yp_all failed!", fnct_name);
    *pnAdapters = -1;
    yp_unbind(strDomainName);
    return((AdapterItem **) NULL);
  } else {
    /* Need to move data from strAdapterRecords to Adapters and
       set num_hosts */
    nAdapters  = 0;   
    nCharIndex = parseWord(0, strAdapterRecords, strRecord,
                           ADAPTERS_RECORD_SEP_CHAR_STRING);
    while (nCharIndex >= 0) {
      sprintf(strAdapterRecordArray[nAdapters++], "%s", strRecord);
      nCharIndex = parseWord(nCharIndex, strAdapterRecords,
                 strRecord, ADAPTERS_RECORD_SEP_CHAR_STRING);
    } /* END while(strRecord) */
    *pnAdapters = nAdapters;
    
    /* Now fill in the AdapterItem array */
    Adapters = (AdapterItem **) malloc(sizeof(AdapterItem *) *
                                       MAX_NUM_ADAPTERS);
    
    for (i = 0; i < nAdapters; i++) {
      Adapters[i] = (AdapterItem *) malloc(sizeof(AdapterItem));
      
      Adapters[i]->marker = 1;
      
      /* NIS Map entry is in the form Adapter Type~Machine Address~IP
         Address~Location */
      nCharIndex = parseWord(0, strAdapterRecordArray[i],
                             strFieldData, ADAPTERS_FIELD_SEP_CHAR_STRING);
      Adapters[i]->adapter_type = allocAndCopyString(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strAdapterRecordArray[i],
                             strFieldData, ADAPTERS_FIELD_SEP_CHAR_STRING);
      Adapters[i]->machine_address = allocAndCopyString(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strAdapterRecordArray[i],
                             strFieldData, ADAPTERS_FIELD_SEP_CHAR_STRING);
      Adapters[i]->ip_address = allocAndCopyString(strFieldData);
      
      nCharIndex = parseWord(nCharIndex, strAdapterRecordArray[i],
                             strFieldData, ADAPTERS_FIELD_SEP_CHAR_STRING);
      Adapters[i]->location = allocAndCopyString(strFieldData);
      
    }        
    
    yp_unbind(strDomainName);
    return(Adapters);
  }  
}


/*****************************************************************************/
/**** END Adapters functions  *****/

/**** Printer functions ****/
/*****************************************************************************/

#ifdef linux
static int  ntcss_printers_nis_callback( int inStatus, char  *inKey, 
					 int inKeyLen,
					 char  *inVal, int inValLen,
					 char *inData ) 
#else
static int  ntcss_printers_nis_callback( int          inStatus,
                                         const char  *inKey,
                                         int          inKeyLen,
                                         const char  *inVal,
                                         int          inValLen,
                                         char        *inData )
#endif
{
  const char  *fnct_name = "ntcss_printers_nis_callback():";

  /* done to get rid of compiler warning about unused variables */
  inKey = inKey;
  inKeyLen = inKeyLen;

  if (inStatus == YP_TRUE)
    {
      strcat(inData, ADAPTERS_RECORD_SEP_CHAR_STRING);
      strncat(inData, inVal, inValLen);
      return(0); /* Return 0 means get next key/value pair */
    }
  else
    {
      /* Log error if it is not YPERR_NOMORE (or YPERR_RPC for now) */
      /* NOTE: We never get YPERR_NOMORE like we expect.  Instead we
         we get YPERR_RPC.  This seems to be a bug in yp_all.  
         A socket is left open each time yp_all is called.
         If the cause of this YPERR_RPC error is found, it will likely
         correct the open socket problem */
      if (inStatus != YPERR_NOMORE  && inStatus != YPERR_RPC)
        {
          syslog(LOG_WARNING, "%s Failed with inStatus of %d - %s",
                 fnct_name, inStatus,  yperr_string(inStatus));
        }
      return(1); /* Return non-zero means don't get the next key/value pair */
    }
}


/*****************************************************************************/

static char  *replacePrinterNullString( char *strFieldData )

{

  if (strcmp(strFieldData, PRINTERS_NULL_STRING) == 0)
    {
      strFieldData[0]='\0';
      return(strFieldData);
    }
  else
    {
      return(strFieldData);
    }
}


/*****************************************************************************/

PrtItem  **getPrtDataFromNis( int       *pnPrinters,
                              AppItem  **Apps,
                              int        nApps )

{
  PrtItem **printerItems;
  const char  *fnct_name = "getPrtDataFromNis():";
  char  *pCh;
  char   printers_by_name_map[] = NTCSS_PRINTERS_BY_NAME_MAP;
  char   strCh[5];
  char  *strDomainName;
  char   strFieldData[1000];
  char   strPrinterProgroups[MAX_NUM_PROGROUPS][MAX_PROGROUP_TITLE_LEN+1];
  char   strPrinterRecordArray[MAX_NUM_PRINTERS][1000];
  char   strPrinterRecords[MAX_NUM_PRINTERS * (1000) + 1];
  char   strRecord[1000];
  int    i, j, k;
  int    nCharIndex;
  int    nErr;
  int    nPrinterApps;
  int    nPrinters;
  struct ypall_callback  callback;
  
  *pnPrinters = -1;
  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(NULL);
  } 
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(NULL);
  }  

  callback.foreach     = ntcss_printers_nis_callback;
  strPrinterRecords[0] = '\0';
  callback.data        = strPrinterRecords;
  
  /* On yp_all see comment in function ntcss_adapters_nis_callback in
     this file */
  if (yp_all(strDomainName, printers_by_name_map, &callback) != 0) {
    syslog(LOG_WARNING, "%s yp_all failed!", fnct_name);
    *pnPrinters = -1;
    yp_unbind(strDomainName);
    return((PrtItem **) NULL);
  } else {
    /* Need to move data from strPrinterRecords to printerItems and
       set num_hosts */
    nPrinters = 0;
    nCharIndex = parseWord(0, strPrinterRecords, strRecord,
                           PRINTERS_RECORD_SEP_CHAR_STRING);
    while (nCharIndex >= 0) {
      sprintf(strPrinterRecordArray[nPrinters++], "%s", strRecord);
      nCharIndex = parseWord(nCharIndex, strPrinterRecords,
                             strRecord, PRINTERS_RECORD_SEP_CHAR_STRING);
    } /* END while(strRecord) */
    *pnPrinters = nPrinters;
    
    /* Now fill in the PrtItem array */
    printerItems = (PrtItem **) malloc(sizeof(PrtItem *) * MAX_NUM_PRINTERS);
    
    for (i = 0; i < nPrinters; i++) {
      printerItems[i] = (PrtItem *) malloc(sizeof(PrtItem));
      
      printerItems[i]->marker = 1;
      
      /* NIS Map entry is in the form  */
      /* Printer Name~host~Location~MaxSize~PrtClassTitle~
         IP~SecurityClass~Status~PortName~SuspendFlag~RedirectPrt~
         FileName~AppNameList  */
      nCharIndex = parseWord(0, strPrinterRecordArray[i], strFieldData,
                             PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->prt_name = allocAndCopyString(replacePrinterNullString(strFieldData));
      
      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->hostname = allocAndCopyString(replacePrinterNullString(strFieldData));
      
      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->prt_location = allocAndCopyString(replacePrinterNullString(strFieldData));
      
      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->max_size = atoi(strFieldData);

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->prt_class_title = allocAndCopyString(replacePrinterNullString(strFieldData));

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->ip_address = allocAndCopyString(replacePrinterNullString(strFieldData));

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->security_class = atoi(strFieldData);

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->status = atoi(strFieldData);

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->port_name = allocAndCopyString(replacePrinterNullString(strFieldData));

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->suspend_flag = atoi(strFieldData);

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->redirect_prt = allocAndCopyString(replacePrinterNullString(strFieldData));

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      printerItems[i]->FileName = allocAndCopyString(replacePrinterNullString(strFieldData));

      nCharIndex = parseWord(nCharIndex, strPrinterRecordArray[i],
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
      for (j = 0; j < MAX_NUM_PROGROUPS; j++) {
        printerItems[i]->prt_access[j] = 0;
        memset(strPrinterProgroups[j], '\0', MAX_PROGROUP_TITLE_LEN + 1);
      } /* END for(j) */

      nPrinterApps = 0;
      pCh = strFieldData;
      strCh[0] = '\0'; 
      strCh[1] = '\0';
      if (*pCh != '\0') {
        nPrinterApps = 1;
      }
      while (*pCh != '\0') {
        if (*pCh != ',') {
          strCh[0] = *pCh;
          strcat(strPrinterProgroups[nPrinterApps - 1], strCh);
        } else {
          nPrinterApps++;
        }
        pCh++;
      } /* END while */

      /* Update the order dependent app list for this printer
         If the App[j] is in the printer access list for this printer
         then make prt_access[j] a 1 for this printer */
      for (j = 0; j < nApps; j++) {
        for (k = 0; k < nPrinterApps; k++) {
          if (strcmp(Apps[j]->progroup_title, strPrinterProgroups[k]) == 0) {
            printerItems[i]->prt_access[j] = 1;
          }
        } /* END for(k) */
      } /* END for(j) */      
    } /* END for(i) */ 
    
    yp_unbind(strDomainName);
    return(printerItems);
  }  
}


/*****************************************************************************/

int  getPrinterAccessListStringFromNis( char  *strPrinterName,
                                        char  *strBuffer )

{
  const char  *fnct_name = "getPrinterAccessListStringFromNis():";
  char   printers_by_name_map[] = NTCSS_PRINTERS_BY_NAME_MAP;
  char  *strDomainName;
  char   strFieldData[500];
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    i;
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;

  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
       fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }

/*
  nErr = yp_match(strDomainName, printers_by_name_map, strPrinterName,
                  strlen(strPrinterName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, printers_by_name_map, strPrinterName,
                  strlen(strPrinterName), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s Priner (%s) not found", fnct_name,
             strPrinterName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s",
             fnct_name, strPrinterName, nErr, yperr_string(nErr));
    }
    yp_unbind(strDomainName);
    return(-1);
  } else {
    nCharIndex = 0;
    for (i = 0; i < 13; i++) {
      nCharIndex = parseWord(nCharIndex, strMapEntry,
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
    }
  }

  sprintf(strBuffer, "%s", strFieldData);
  
  yp_unbind(strDomainName);
  return(0);
}


/*****************************************************************************/

int  getNtPrinterDriverNameFromNis( char  *strPrinterName,
                                    char  *strBuffer )

{
  const char  *fnct_name = "getNtPrinterDriverNameFromNis():";
  char   printers_by_name_map[] = NTCSS_PRINTERS_BY_NAME_MAP;
  char   strFieldData[500];
  char  *strDomainName;
/*
  char  *strMapEntry;
*/
  char   strMapEntry[1000];
  int    i;
  int    nCharIndex;
  int    nEntryLen;
  int    nErr;

  nErr = yp_get_default_domain(&strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_get_default_domain failed with error %d - %s",
           fnct_name, nErr, yperr_string(nErr));
    return(-1);
  }
  
  nErr = yp_bind(strDomainName);
  if (nErr != 0) {
    syslog(LOG_WARNING, "%s yp_bind(%s) failed with err %d - %s",
           fnct_name, strDomainName, nErr, yperr_string(nErr));
    return(-1);
  }

/*
  nErr = yp_match(strDomainName, printers_by_name_map, strPrinterName,
          strlen(strPrinterName), &strMapEntry, &nEntryLen);
*/
  nErr = ntcss_yp_match(strDomainName, printers_by_name_map, strPrinterName,
          strlen(strPrinterName), strMapEntry, &nEntryLen);
  
  if (nErr != 0) {
    if (nErr == YPERR_KEY) {
      syslog(LOG_WARNING, "%s Priner (%s) not found", fnct_name,
             strPrinterName);
    } else {
      syslog(LOG_WARNING, "%s yp_match(%s) failed with err %d - %s",
             fnct_name, strPrinterName, nErr, yperr_string(nErr));
    } /* END if(nErr == YPERR_KEY) */
    yp_unbind(strDomainName);
    return(-1);
  } else {
    nCharIndex = 0;
    for (i = 0; i < 14; i++) {
      nCharIndex = parseWord(nCharIndex, strMapEntry,
                             strFieldData, PRINTERS_FIELD_SEP_CHAR_STRING);
    }
  }

  /* Check for trailing new line, and overwrite it if it is there */
  i = strlen(strFieldData);
  /* Check for strlen 0 because the maps have been corrupted before and
     gave back a null string for this one */
  if (i != 0) {
    stripNewline(strFieldData);
  }

  sprintf(strBuffer, "%s", strFieldData);
  
  yp_unbind(strDomainName);
  return(0);
}


/*****************************************************************************/
/**** END Printer functions ****/

/*****************************************************************************/
/* This function removes all the .time files in the database directory.
 */

int  NISremoveTimeFiles()

{
  const char *fnct_name = "NISremoveTimeFiles():";
  char        system_cmd[MAX_CMD_LEN];
  int         x;

  sprintf(system_cmd, "%s %s/*.time 2>&1", RM_PROG, NTCSS_DB_DIR);

  x = system(system_cmd);

  if (x != 0)
    {
      syslog(LOG_WARNING, "%s rm program failed!  Errno %i - %s",
             fnct_name, errno, strerror(errno));
      return(-1);
    }

  return(0);
}


/*****************************************************************************/
/* This function contacts the NIS master for the domain specified and tries
   to get the specified map from it.
   The str_domain_name is not used for now, but should be added into the
   pipe command string with it's appropriate flag.
*/

int  GetNISmap( const char  *str_map_name,
		const char  *str_domain_name )

{
  int          i_script_return;
  int          x;
  /*char         str_err_msg_buf[MAX_ERR_MSG_LEN];*/
  const char  *str_fnct_name = "GetNISmap():";
  char         str_pipe_cmd[MAX_PATH_LEN +        /* path to scripts program */
			   ((MAX_HOST_NAME_LEN + 1) * MAX_NUM_HOSTS)];

  sprintf(str_pipe_cmd, "%s -f %s", YPXFR, str_map_name);

 i_script_return = system(str_pipe_cmd);

  if (!WIFEXITED(i_script_return))         /* Process terminated abnormally. */
    {
      syslog(LOG_WARNING, "%s Unknown Error occured! exit staus = %i",
	     str_fnct_name, i_script_return);
      return(-2);
    }

  x = WEXITSTATUS(i_script_return);            /* Get script's return value. */
  if (x != 0)                                /* Should be a postive integer. */
    {
      syslog(LOG_WARNING, "%s Failed to get NIS map %s!  Script returned "
	     "error %i", str_fnct_name, str_map_name, x);
    }

  if( str_domain_name != NULL)  /* Avoid unused var warnings */
    x = x;

  return(x);
}


/*****************************************************************************/
/* Reads the LOCK_DB_FOR_NIS_MAKE key from the system settings ini file
   and returns it's value
*/
int  lockDBForNISMake( void )

{
  char  section[] = NTCSS_SS_SECTION_HEADER;
  char  retBuf[5];
  const char *fnct_name = "lockDBForNISMake():";

  /* Get the value from the system settings ini file */
  if (GetProfileString(section, "LOCK_DB_FOR_NIS_MAKE", "1", retBuf, 5,
                       NTCSS_SS_INI_FILE) < 0)
    {
       syslog(LOG_WARNING,"%s failed to find key <%s> in system settings "
                          "ini file!", fnct_name,"LOCK_DB_FOR_NIS_MAKE");
       return 1;
    }
  else
    return atoi(retBuf);
}


/*****************************************************************************/

int  buildNtcssNisMaps( void )

{
  /*int    f_use_db_locks;*/
  int    x;
  /*int    i_lock_id;*/
  const char  *fnct_name = "buildAndPushNtcssNisMaps():";
  char   strCommand[500]; /*,strFileName[MAX_FILENAME_LEN];*/
  char  *strNISTempName; 
  char  strLogsDir[MAX_FILENAME_LEN];

  if (IAmMaster() == FALSE) {
      return(0);
  }

  if (strcmp(PUSH_NTCSS_NIS_MAPS_SCRIPT, "") == 0) {
    syslog(LOG_WARNING, "%s PUSH_NTCSS_NIS_MAPS_SCRIPT not defined!",
           fnct_name);
    return(-1);
  }

  if (strcmp(NIS_DIR, "") == 0) {
    syslog(LOG_WARNING, "%s NIS_DIR not defined!", fnct_name);
    return(-2);
  }

  if (strcmp(NISMAP_DIR, "") == 0) {
    syslog(LOG_WARNING, "%s NISMAP_DIR not defined!", fnct_name);
    return(-3);
  }
 
  
  sprintf(strLogsDir,"%s/%s",NTCSS_HOME_DIR,NTCSS_LOGS_DIR);

  strNISTempName = tempnam(strLogsDir, "NISLIST"); 
  if (strNISTempName == NULL)
    { 
      syslog(LOG_WARNING,"Could not create temporary map list file - %d.",
	     errno);
      return(-4);
    }

  /* Get list of newer NIS maps */
  sprintf(strCommand, "%s/%s/%s > %s", NTCSS_HOME_DIR, NTCSS_BIN_DIR,
	  MAKE_MAP_LIST_SCRIPT, strNISTempName);

  x = system(strCommand);

  if (x != 0)
    {
      remove(strNISTempName);
      return (0);
    }

  sprintf(strCommand, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_BIN_DIR,
          PUSH_NTCSS_NIS_MAPS_SCRIPT);

  /* Build the new NIS maps.. */
  /* Child does work calling system to run the push script */
  x = system(strCommand);

  return(0);

}

/*****************************************************************************/
/*****************************************************************************/

static char strKeyMatch[1000];
int ntcss_yp_match(char *strDomainName, char *strMapName,
                   char *strKey, int nKeyLen, char *strMapEntry,
                   int *nEntryLen)
{
struct ypall_callback ypcb;
const char  *fnct_name = "ntcss_yp_match():";
int nStatus;

	nStatus=0;
	nKeyLen=nKeyLen;   /* shut the compiler up */
        yp_bind(strDomainName);

	ypcb.foreach=ntcss_yp_match_callback;
	ypcb.data=strMapEntry;

	memset(strKeyMatch,NULL,1000);
	strcpy(strKeyMatch,strKey);
	strMapEntry[0]='\0';

	if (yp_all(strDomainName,strMapName,&ypcb) != 0) {
		syslog(LOG_WARNING,"%s Failed yp_all call",fnct_name);
		nStatus = YPERR_YPERR;
	} else {
		if (strlen(strMapEntry) == 0) {
			syslog(LOG_WARNING,"%s Key <%s> not found in map <%s>",
				fnct_name,strKey, strMapName);
			nStatus=YPERR_KEY;
		} else {
			*nEntryLen=strlen(strMapEntry);
		}
	}

        yp_unbind(strDomainName);
	return(nStatus);
}

#ifdef linux
int ntcss_yp_match_callback(int nStatus, char *strInKey, int nInKeyLen,
                            char *strInVal, int nInValLen, char *strMapBuf)
#else
int ntcss_yp_match_callback(int nStatus, const char *strInKey, int nInKeyLen,
                            const char *strInVal, int nInValLen, 
			    char *strMapBuf)
#endif
{
const char  *fnct_name = "ntcss_yp_match_callback():";

	if (nStatus == YP_TRUE) {
        	if ((strlen(strKeyMatch) == (unsigned int)nInKeyLen) &&
                	strncmp(strKeyMatch,strInKey,nInKeyLen) == 0) {
                	sprintf (strMapBuf, "%.*s", nInValLen, strInVal);
                	return(1);
        	}
		return(0);
	} else {
		if (nStatus != YPERR_NOMORE  && nStatus != YPERR_RPC) {
			syslog(LOG_WARNING, 
			   "%s Failed with nStatus of %d - %s",
			   fnct_name, nStatus,  yperr_string(nStatus));
		}
		return(1);
        }
}

