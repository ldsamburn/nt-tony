
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*  FILEFCTS.C */

/** SYSTEM INCLUDES **/

#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <syslog.h> 
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/shm.h>


/** INRI INCLUDES **/

#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <ini_Fcts.h>
#include <StringFcts.h>

/** LOCAL INCLUDES **/

/** FUNCTION PROTOTYPES **/
int getNtcssSystemSettingsShmId(void);
int getNtcssSystemSettingFromShm( const char*, char*, int );

/** STATIC VARIABLES **/
/** GLOBAL VARIABLES **/

#if defined SOLARIS || defined HPUX
extern char *sys_errlist[];
#endif

/* The global array strTempStrings is used to allow the function
   getNtcssSystemSettingTempPtr to return a char *. The pointer returned
   should be considered volatile as this array is cycled.
   Eventually that address will be reused.  The amount of time before
   it is reused depends on how many times the function
   getNtcssSystemSettingTempPtr is called.  nTempStrIndex is used to
   rotate through the array. */

int  nTempStrIndex = 0;
char strTempStrings[NTCSS_SS_NUM_TMP_STRINGS][NTCSS_SS_MAX_VALUE_LEN];

typedef struct NtcssSystemSetting_ {
  char strKey  [NTCSS_SS_MAX_KEY_LEN];
  char strValue[NTCSS_SS_MAX_VALUE_LEN];
} NtcssSystemSetting;



/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
int  getNtcssSystemSettingsShmId( void )

{
  const char  *fnct_name = "getNtcssSystemSettingsShmId():";
  int          nShmId;
  
  nShmId = shmget(NTCSS_SS_SHMEM_KEY, 0, IPC_CREAT | 0777);
  
/*  if (nShmId < 0) { */
  if (nShmId == -1)
    {
      syslog(LOG_WARNING, "%s shmget failed with error --%s", fnct_name,
             strerror(errno));
      return(-1);
    }

  return(nShmId);
}


/*****************************************************************************/

int  freeNtcssSystemSettingsShm( void )
{
  const char  *fnct_name = "freeNtcssSystemSettingsShm():";
  int  nShmId;

  nShmId = getNtcssSystemSettingsShmId();

  if (nShmId < 0)
    {
      syslog(LOG_WARNING, "%s shmtget failed", fnct_name);
      return(-1);
    }

  if (shmctl(nShmId, IPC_RMID, 0) < 0)
    {
      syslog(LOG_WARNING, "%s shmctl failed", fnct_name);
      return(-1);
    }

  return(0);
}


/*****************************************************************************/

int  loadNtcssSystemSettingsIntoShm( void )
{
  const char *fnct_name = "loadNtcssSystemSettingsIntoShm():";
  char  ss_sect_header[] = NTCSS_SS_SECTION_HEADER;
  char  ss_ini_file[] = NTCSS_SS_INI_FILE;
  int   i;
  int   nBufLen;
  int   nContinue;
  int   nError = 0;
  int   nKeyValuePairs;
  int   nOsKeyValuePairs;
  int   nShmId;
  int   nShmSize;
  char *pCh;
  char *pShMem;
  char *strKey;
  char *strKeyValuePairs;
  char  strOsHeader[150];
  char *strOsKeyValuePairs;
  char  strOsVersion[50];
  char  strTemp[NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2];
  char *strValue;

  struct utsname      unameStruct;
  struct stat         stat_buf;
  NtcssSystemSetting *pNS;

  /* PF_MAX_KEY_LEN + 1    - The two is for sepchar between the key
     value pairs and the =  NTCSS_SS_MAX_KEYS + 1 - The one is for
     the extra tag that will be loaded to determine if the NTCSS data files
     are encrtyped or not */
  nBufLen = (NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2) *
    (NTCSS_SS_MAX_KEYS + 1);
  strKeyValuePairs   = malloc(sizeof(char) * nBufLen);
  strOsKeyValuePairs = malloc(sizeof(char) * nBufLen);
  memset(strKeyValuePairs,   '\0', nBufLen);
  memset(strOsKeyValuePairs, '\0', nBufLen);

  if (GetProfileSection(ss_sect_header, strKeyValuePairs,
                        nBufLen, ss_ini_file) < 0)
    {
      syslog(LOG_WARNING, "%s GetProfileSection(%s) failed!", fnct_name,
             ss_sect_header);
      free(strKeyValuePairs);
      free(strOsKeyValuePairs);
      return(-1);
    }

  /* Determine which OS defines to load */
  if (uname(&unameStruct) < 0)
    {
      syslog(LOG_WARNING,"%s uname failed!,%d", fnct_name,errno);
      return(-2);
    }

  sprintf(strOsHeader, "%s", unameStruct.sysname);
  /* We are not concerned with version granularity on non HP-UX systems. */
  if (strcmp(unameStruct.sysname, "HP-UX") == 0)
    {
      memset(strOsVersion, '\0', 50);
      /* For HP-UX release is in form A.XX.YY (A.09.01 or B.10.10) 
         and we only need the 3rd and 4th chars of the release */
      memcpy(strOsVersion, unameStruct.release + 2, 2);
      strcat(strOsHeader, strOsVersion);
    }

  if (GetProfileSection(strOsHeader, strOsKeyValuePairs, nBufLen,
                        ss_ini_file) < 0)
    {
      syslog(LOG_WARNING, "%s GetProfileSection(%s) failed", fnct_name,
             strOsHeader);
      free(strKeyValuePairs);
      free(strOsKeyValuePairs);
      return(-3);
    }

  /* Loop through and determine how many key=value pairs we got back */
  /* We have reached the end when we find two consecutive NULLs */
  pCh = strKeyValuePairs;
  nKeyValuePairs = 0;
  /* If no key=value pairs returned do not go through loop */
  if (*pCh == '\0')
    {
      nContinue = 0;
    }
  else
    {
      nContinue = 1;
    }

  while (nContinue)
    {
      nKeyValuePairs++;
      /* Move to the end of this key=value pair */
      while (*pCh != '\0')
        {
          pCh++;
        }

      /* Move ch up one and check for consecutive null condition */
      pCh++;
      if (*pCh == '\0')
        {
          nContinue = 0;
        }
    }           /* END while */

  /* Now add the number of OS Key/Value Pairs */
  pCh = strOsKeyValuePairs;
  nOsKeyValuePairs = 0;
  if (*pCh == '\0')
    {
      nContinue = 0;
    }
  else
    {
      nContinue = 1;
    }

  while (nContinue)
    {
      nOsKeyValuePairs++;
      /* Move to the end of this key=value pair */
      while (*pCh != '\0')
        {
          pCh++;
        }

      /* Move ch up one and check for consecutive null condition */
      pCh++;
      if (*pCh == '\0')
        {
          nContinue = 0;
        }
    }                 /* END while */
  
  /* Add 1 to nKeyValuePairs for an ending structure
     Add 1 to nKeyValuePairs for the crypt/decrypt tag
  */
  nShmSize =
    (nKeyValuePairs + nOsKeyValuePairs + 1/*End struct*/ + 1/*Crypt tag*/) *
    sizeof(NtcssSystemSetting);
  nShmId = shmget(NTCSS_SS_SHMEM_KEY, nShmSize, IPC_CREAT | 0777);
  if (nShmId < 0)
    {
      syslog(LOG_WARNING, "%s shmget of size %d failed with error %d %s",
             fnct_name, nShmSize, errno, sys_errlist[errno]);
      free(strKeyValuePairs);
      free(strOsKeyValuePairs);
      return(-4);
    }

  pShMem = shmat(nShmId, 0, 0777);
  if ((int)pShMem == -1)
    {
      /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
        (errno == EINVAL) || (errno == EMFILE)) { */
      syslog(LOG_WARNING, "%s shmat failed!", fnct_name);
      free(strKeyValuePairs);
      free(strOsKeyValuePairs);
      return(-5);
    }

  /* Move System Settings into shared memory */
  pCh = strKeyValuePairs;
  i = 0;
  while (i < nKeyValuePairs)
    {
      memset(strTemp, '\0', NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2);
      strncpy(strTemp, pCh, NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2);
      pCh += strlen(strTemp) + 1;
      pNS = (NtcssSystemSetting *)pShMem + i;
      memset(pNS->strKey,    '\0', NTCSS_SS_MAX_KEY_LEN);
      memset(pNS->strValue,  '\0', NTCSS_SS_MAX_VALUE_LEN);
      strKey = strtok(strTemp, "=");
      strncpy(pNS->strKey, strKey, NTCSS_SS_MAX_KEY_LEN);
      strValue = strtok(NULL, "=");
      if (strValue != NULL)
	  strncpy(pNS->strValue, strValue, NTCSS_SS_MAX_VALUE_LEN);
    
      i++;
    }

  /* Move OS System Settings into shared memory */
  pCh = strOsKeyValuePairs;
  i = 0;
  while (i < nOsKeyValuePairs)
    {
      memset(strTemp, '\0', NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2);
      strncpy(strTemp, pCh, NTCSS_SS_MAX_KEY_LEN + NTCSS_SS_MAX_VALUE_LEN + 2);
      pCh += strlen(strTemp) + 1;
      pNS = (NtcssSystemSetting *)pShMem + (i + nKeyValuePairs);
      memset(pNS->strKey,    '\0', NTCSS_SS_MAX_KEY_LEN);
      memset(pNS->strValue,  '\0', NTCSS_SS_MAX_VALUE_LEN);
      strKey = strtok(strTemp, "=");
      strncpy(pNS->strKey, strKey, NTCSS_SS_MAX_KEY_LEN);
      strValue = strtok(NULL, "=");
      strncpy(pNS->strValue, strValue, NTCSS_SS_MAX_VALUE_LEN);
    
      i++;
    }

  /* Add crypt tag */
  pNS = ((NtcssSystemSetting *) pShMem) + (nKeyValuePairs + nOsKeyValuePairs);
  sprintf(pNS->strKey, NTCSS_ENCRYPTION_TAG);

  /* If this is an install, the CHECK_FILE may not exist.. */
  if (stat(NTCSS_ENCRYPTION_CHECK_FILE, &stat_buf) < 0)
    {
      sprintf(pNS->strValue, "%s", NTCSS_ENCRYPTION_OFF_STRING);
    } 
  else                                                /* CHECK_FILE exists.. */
    if (IsNotTextFile(NTCSS_ENCRYPTION_CHECK_FILE) == 1)
      {
        sprintf(pNS->strValue, "%s", NTCSS_ENCRYPTION_ON_STRING);
      }
    else
      {
        sprintf(pNS->strValue, "%s", NTCSS_ENCRYPTION_OFF_STRING);
      }


  /* Add trailing structure */
  pNS = ((NtcssSystemSetting *) pShMem) +
    (nKeyValuePairs + nOsKeyValuePairs + 1/*Crypt tag*/);
  sprintf(pNS->strKey,   NTCSS_SS_END_STRING);
  /*sprintf(pNS->strValue, "");*/
  pNS->strValue[0]=0;

  free(strKeyValuePairs);
  free(strOsKeyValuePairs);

  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shmdt failed!", fnct_name); 
      nError = -6;
    }
  
  DeleteINILinks(ss_ini_file);
  return(nError);  

}


/*****************************************************************************/

int  printNtcssSystemSettingsFromShm( int nValueOnly )

{
  const char *fnct_name = "printNtcssSystemSettingsFromShm():";
  int  nShmId, nContinue;
  char *pShMem;
  NtcssSystemSetting *pNS;

  nShmId = getNtcssSystemSettingsShmId();

  if (nShmId < 0)
    {
      syslog(LOG_WARNING, "%s getNtcssSystemSettingsShmId failed", fnct_name);
      return(-1);
    }
  
  pShMem = shmat(nShmId, 0, 0444);
  pNS = (NtcssSystemSetting *) pShMem;
  /*if (pNS < 0) { */
  if ((int)pShMem == -1)
    {
      /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
        (errno == EINVAL) || (errno == EMFILE)) {*/
      syslog(LOG_WARNING, "%s shmat failed!", fnct_name);
      return(-1);
    }

  nContinue = 1;
  while (nContinue)
    {
      if (strcmp(pNS->strKey, NTCSS_SS_END_STRING) == 0)
        {
          nContinue = 0;
          continue;
        }
      else
        {
          if (nValueOnly)
            {
              printf("%s\n", pNS->strValue);
            }
          else
            {
              printf("%s=%s\n", pNS->strKey, pNS->strValue);
            }
        }

      pNS += 1;
    }             /* END while */

  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shmdt failed!", fnct_name);
    }
  
  return(0);
}


/*****************************************************************************/

int  getNtcssSystemSettingFromShm( const char *strKey,
                                   char       *strBuffer,
                                   int         nBufSize )
{
  const char *fnct_name = "getNtcssSystemSettingFromShm():";
  int   nContinue;
  int   nFound;
  int   nShmId;
  char *pShMem;
  NtcssSystemSetting *pNS;

  memset(strBuffer, '\0', nBufSize);

  nShmId = getNtcssSystemSettingsShmId();

  if (nShmId < 0)
    {
      syslog(LOG_WARNING, "%s getNtcssSystemSettingsShmId failed! "
             "strKey == %s", fnct_name, strKey);
      return(-1);
    }
  
  pShMem = shmat(nShmId, 0, 0444);
  pNS = (NtcssSystemSetting *) pShMem;
  /* if (pNS < 0) { */
  if ((int)pShMem == -1 )
    {
      /*if ((errno == EINVAL) || (errno == EACCES) || (errno == ENOMEM) ||
        (errno == EINVAL) || (errno == EMFILE)) {*/
      syslog(LOG_WARNING, "%s shmat failed!", fnct_name);
      return(-1);
    }

  nFound = 0;
  nContinue = 1;
  while (nContinue)
    {
      if (strcmp(pNS->strKey, strKey) == 0)
        {
          strncpy(strBuffer, pNS->strValue, nBufSize);
          nContinue = 0;
          nFound = 1;
          continue;
        }

      if (strcmp(pNS->strKey, NTCSS_SS_END_STRING) == 0)
        {
          nFound    = 0;
          nContinue = 0;
          continue;
        }

      pNS += 1;
    }               /* END while */

  if (shmdt(pShMem) < 0)
    {
      syslog(LOG_WARNING, "%s shmdt failed!", fnct_name);
    }
  
  if (nFound == 0)
    {
      syslog(LOG_WARNING, "%s failed to find value for key <%s>!", fnct_name,
             strKey);
      return(-1);
    }
  else
    {
      return(0);
    }
}


/*****************************************************************************/
/* NOTE: The pointer returned by this function is volatile and should be
   copied to another buffer if the information is to be used after the
   immediate function call */

const char *getNtcssSystemSettingTempPtr( const char *strKey,
                                          const char *strDefault )

{
  /* Use mod(%) function to cycle through our temp string array */
  nTempStrIndex = (nTempStrIndex + 1) % NTCSS_SS_NUM_TMP_STRINGS;

  if (getNtcssSystemSettingFromShm(strKey, strTempStrings[nTempStrIndex],
                                   NTCSS_SS_MAX_VALUE_LEN) != 0)
    {
      return(strDefault);
    }
  else
    {
      return((const char*)strTempStrings[nTempStrIndex]);
    }
}


/*****************************************************************************/

int  SystemSettingsAreInitialized( void )

{
  if (getNtcssSystemSettingsShmId() < 0)
    {
      return(FALSE);
    }
  else
    {
      return(TRUE);
    }
}


/*****************************************************************************/
