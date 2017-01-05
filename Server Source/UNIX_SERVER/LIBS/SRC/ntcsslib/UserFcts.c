
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * UserFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <netdb.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pwd.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#if defined SOLARIS || defined linux
#include <crypt.h>
#endif

#if defined SOLARIS || defined HPUX
extern char *sys_errlist[];
#endif

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <ExtraProtos.h>

/** LOCAL INCLUDES **/

#include "TextDBP.h"
#include "ini_Fcts.h"


/** FUNCTION PROTOTYPES **/
void  sig_dm_alarm(int);

/** STATIC VARIABLES **/

/* Used by getClassification().  Any future classification value changes
   will have to be reflected in this string array. */
static const char *classifications[] = { "Unclassified", 
                                   "Unclassified-Sensitive",
                                   "Confidential", "Secret", "Top Secret",
                                   "Unknown Value" };

/*static const char *user_roles[] = { "NTCSS User", "NTCSS Administrator",
                              "Unknown Value" }; */
/*static const char *login_states[] = { "Successful Login", 
                                "Failed Login Attempt",
                                "Logged Out", "Unknown Value" };*/
static const char *unix_statuses[] = { "Keep existing UNIX account",
                                 "Delete existing UNIX account",
                                 "Do not add UNIX account",
                                 "Add new UNIX account",
                                 "Unknown Value" };

int  usort_compare( const void  *name1,
                    const void  *name2 );

/*****************************************************************************/
/** FUNCTION DEFINITIONS **/

int signalDmResourceMonitorStartWatch(void)
{
  int nPId;

  nPId = getDmResourceMonitorPid();

  if (nPId < 0) {
    syslog(LOG_WARNING, "signalDmResourceMonitorStartWatch: getDmResourceMonitorPid failed");
    return(-1);
  } /* END if */

  if (kill(nPId, SIGUSR1) != 0) {
    syslog(LOG_WARNING, "signalDmResourceMonitorStartWatch: kill(%d) failed with errno %d",
	   nPId, errno);
    return(-1);
  } else {
    return(0);
  } /* END if */

}

int signalDmResourceMonitorStopWatch(void) 
{
  int nPId;

  nPId = getDmResourceMonitorPid();

  if (nPId < 0) {
    syslog(LOG_WARNING, "signalDmResourceMonitorStopWatch: getDmResourceMonitorPid failed");
    return(-1);
  } /* END if */

  if (kill(nPId, SIGUSR2) != 0) {
    syslog(LOG_WARNING, "signalDmResourceMonitorStopWatch: kill(%d) failed with errno %d",
	   nPId, errno);
    return(-1);
  } else {
    return(0);
  } /* END if */

}

int getDmResourceMonitorPid(void) 
{

  FILE  *fp;
  char   filename[MAX_PATH_LEN];
  int    pid; /* change to type pid_t ?? */

  sprintf(filename, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR, 
	  DM_RESOURCE_MONITOR_PID_FILE);
  fp = fopen(filename, "r");
  if (fp == NULL) {
    syslog(LOG_WARNING, "getDmResourceMonitorPid: Could not open file <%s>!", filename);
    return(-1);
  } /* END if */

  fscanf(fp, "%d\n", &pid);
  fclose(fp);

  return(pid);
} 

int setDmResourceMonitorPid(int nPId) 
{
  FILE  *fp;
  char   filename[MAX_PATH_LEN];
  
  sprintf(filename, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR, 
	  DM_RESOURCE_MONITOR_PID_FILE);
  fp = fopen(filename, "w");
  if (fp == NULL) {
    syslog(LOG_WARNING, "setDmResourceMonitorPid: Could not open file <%s>!", filename);
    return(-1);
  } else {
     fprintf(fp, "%d\n", nPId);
     fclose(fp);
     return(0);
  } /* END if */
  
} 

int getDmPid(void) 
{

  FILE  *fp;
  char   filename[MAX_PATH_LEN];
  int    pid; /* change to type pid_t ?? */

  sprintf(filename, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR, DM_PID_FILE);
  fp = fopen(filename, "r");
  if (fp == NULL) {
    syslog(LOG_WARNING, "getDmResourceMonitorPid: Could not open file <%s>!", filename);
    return(-1);
  } /* END if */

  fscanf(fp, "%d\n", &pid);
  fclose(fp);

  return(pid);
} 

int setDmPid(int nPId)
{

  FILE  *fp;
  char   filename[MAX_PATH_LEN];
  
  sprintf(filename, "%s/%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR, DM_PID_FILE);
  fp = fopen(filename, "w");
  if (fp == NULL) {
    syslog(LOG_WARNING, "setDmPid: Could not open file <%s>!", filename);
    return(-1);
  } else {
     fprintf(fp, "%d\n", nPId);
     fclose(fp);
     return(0);
  } /* END if */

  return(0);
} 


int  getUserSharedPassword( const char  *strLoginName,
                            char        *strBuffer )

{
  const char  *fnct_name = "getUserSharedPassword():";
  UserItem  **Users;
  int  i, nFound = 0, nUsers;

  /* Initialize the return buffer */
  /*sprintf(strBuffer, "");*/
  strBuffer[0] = 0;

  /* Get the shared password for this user */
  Users = getUserDataByName(strLoginName,&nUsers, NTCSS_DB_DIR);
  for (i = 0; i < nUsers; i++)
    {
      if (strcmp(Users[i]->login_name, strLoginName) == 0)
        {
          nFound = 1;
          sprintf(strBuffer, "%s", Users[i]->shared_passwd);
        }
    }
  
  freeUserData(Users, nUsers);
  
  if (nFound != 0)
    {
      return(0);
    }

  syslog(LOG_WARNING, "%s Found no UserItem entry for (%s)", fnct_name,
         strLoginName);
  return(-1);
} 

/*****************************************************************************/
/* function added for ATLAS */
AppuserProgroupRoleItem  **getUserProgroupAndRoleInfoForLogin(
                                                     const char  *login_name,
                                                     int         *nRecords,
                                                     const char  *dbDir )
{
  const char *fnct_name = "getUserProgroupAndRoleInfoForLogin():";
  char *pStrAppHost;
  char *pStrLinkData;
  char  strThisHostName[MAX_HOST_NAME_LEN + 1];
  char  strUserSharedPassword[MAX_APP_PASSWD_LEN];
  int   i, j;
  int   nAppusers;
  int   nApps;
  int   nAppHostType;
  int   nLocalRecords;
  int   nReportProgroup;
  int   nThisHostType;
  int   num_roles;
  AppItem                  **appItems;
  AppuserItem              **Appusers;
  AppuserProgroupRoleItem  **AppuserProgroupRoleItems;
  AccessRoleItem           **AccessRoles;

  *nRecords = 0; /* 3/5/99 GWY: Changed default from -1 to 0 */
  nLocalRecords = -1;

  if (isValidNtcssUser(login_name, NTCSS_DB_DIR) != 1)
    {
      syslog(LOG_WARNING, "%s User(%s) has no information on this host."
             " Returning.", fnct_name, login_name);
      return((AppuserProgroupRoleItem **) 0);
    }

  gethostname(strThisHostName, MAX_HOST_NAME_LEN);
  nThisHostType = getHostType(strThisHostName,NTCSS_DB_DIR);
  
  if (nThisHostType < 0)
    {
      syslog(LOG_WARNING,"%s getHostType(%s) failed!", fnct_name,
             strThisHostName);
      return((AppuserProgroupRoleItem **) 0);
    }

/* ACA/JJ
  Appusers = getAppuserData(&nAppusers, dbDir, WITH_NTCSS_GROUP);
*/
  Appusers = getAppuserDataByName(login_name,&nAppusers, dbDir, 
		WITH_NTCSS_GROUP);

  if (nAppusers < 0 )
    {
      syslog(LOG_WARNING, "%s getAppuserData failed!", fnct_name);
      return((AppuserProgroupRoleItem **) 0);
    }

  if (getUserSharedPassword(login_name, strUserSharedPassword) != 0)
    {
      syslog(LOG_WARNING, "%s getUserSharedPassword(%s) failed", fnct_name,
             login_name);
      freeAppuserData(Appusers,nAppusers);
      return((AppuserProgroupRoleItem **) 0);
    }

  nLocalRecords = 0;
  AppuserProgroupRoleItems = (AppuserProgroupRoleItem **)
    malloc(MAX_NUM_PROGROUPS_PER_NTCSS_USER * sizeof(char *));

  AccessRoles=getAccessRoleData(&num_roles, dbDir);
  appItems = getAppData(&nApps, dbDir, WITH_NTCSS_GROUP);

  for (i = 0; i < nAppusers; i++)
    {
      if (strcmp(Appusers[i]->login_name, login_name) == 0)
        {
          /* Get the link_data and host type */  
          if (nApps < 0)
            {
              syslog(LOG_WARNING, "%s getAppData failed!", fnct_name);
              pStrLinkData = NULL;
              pStrAppHost  = NULL;
              /* If we can't get app data on this app, move to the next one */
              continue;
            } 
          else
            {
              for (j = 0; j < nApps; j++)
                {
                  if (strcmp(appItems[j]->progroup_title,
                             Appusers[i]->progroup_title) == 0)
                    {
                      pStrLinkData = 
                        allocAndCopyString(appItems[j]->link_data);
                      pStrAppHost  = allocAndCopyString(appItems[j]->hostname);
                      nAppHostType = getHostType(appItems[j]->hostname,
                                                 NTCSS_DB_DIR);
                      break;
                    }
                }                    /* END for(j) */
            }                 /* END if */

          nReportProgroup = 0;
      
          switch(nThisHostType)
            {
            case NTCSS_MASTER:
              /* Master Server reports all App Server Apps and all Apps that 
                 are assigned to the Master */
              if (nAppHostType == NTCSS_APP_SERVER)
                nReportProgroup = 1;
    
              if (strcmp(strThisHostName, pStrAppHost) == 0)
                nReportProgroup = 1;
    
              break;
    
            case NTCSS_AUTH_SERVER:
              /* Auth Servers only report Apps assigned to it */
              if (strcmp(strThisHostName, pStrAppHost) == 0)
                nReportProgroup = 1;
              break;
              
            case NTCSS_APP_SERVER:
              /* App server should never get here anyway */
              break;
    
            default:
              break;
            }          /* END switch */

          if (pStrAppHost)
            free(pStrAppHost);

          if (pStrLinkData)
            free(pStrLinkData);
      
          if (!nReportProgroup)
            continue;

          if (IsAppLocked(Appusers[i]->progroup_title) && 
              !IsAppRoleLockOverride(Appusers[i]->progroup_title,
                                     Appusers[i]->app_role, AccessRoles,
                                     num_roles)) 
            continue;
      
          AppuserProgroupRoleItems[nLocalRecords] = (AppuserProgroupRoleItem *)
            malloc(sizeof(AppuserProgroupRoleItem));
      
          AppuserProgroupRoleItems[nLocalRecords]->progroup_title = 
            allocAndCopyString(Appusers[i]->progroup_title);
          AppuserProgroupRoleItems[nLocalRecords]->shared_passwd =
            allocAndCopyString(strUserSharedPassword);
          AppuserProgroupRoleItems[nLocalRecords]->app_data =
            allocAndCopyString(Appusers[i]->app_data);
          AppuserProgroupRoleItems[nLocalRecords]->app_role =
            Appusers[i]->app_role;
/*
      AppuserProgroupRoleItems[nLocalRecords]->link_data = pStrLinkData;
*/
          AppuserProgroupRoleItems[nLocalRecords]->link_data =
            allocAndCopyString("NOLINKDATA");
      
          nLocalRecords++;
        }        /* END if */
    }     /* END for(i) */

  freeAccessRoleData(AccessRoles, num_roles);
  freeAppData(appItems, nApps);
  freeAppuserData(Appusers, nAppusers);

  *nRecords = nLocalRecords;
  return(AppuserProgroupRoleItems);
}


/*****************************************************************************/

void  freeUserProgroupAndRoleInfo( AppuserProgroupRoleItem
                                                 **AppuserProgroupAndRoleItems,
                                   int  nRecords)
{
  int  i;

  for (i = nRecords - 1; i >= 0; i--)
    {
      free(AppuserProgroupAndRoleItems[i]->progroup_title);
      free(AppuserProgroupAndRoleItems[i]->shared_passwd);
      free(AppuserProgroupAndRoleItems[i]->app_data);

      free((char *) AppuserProgroupAndRoleItems[i]);
    }
  free((char *) AppuserProgroupAndRoleItems );
}


/*****************************************************************************/
/* Returns a pointer to a static string with the ASCII name of the security
   class value given.
   See also MakeUserClearanceString() <- Will not be affected by any future
   changes
*/

const char  *getClassification(value)

     int  value;
{
  if (value >= 0 && value < NUM_CLASSIFICATIONS)
    return(classifications[value]);
  else
    return(classifications[NUM_CLASSIFICATIONS]);
}


/*****************************************************************************/

int  getClassificationNum( const char *str )

{
  int  i;

  for (i = 0; i < NUM_CLASSIFICATIONS; i++)
    if (strcmp(str, classifications[i]) == 0)
      return(i);

  return(0);
}


/*****************************************************************************/
/* This function lost its original functionality when the User role was done away with. *//* Apparently, this function is only called by the code in USERS_CONF. *//* The return(0) was made for the switch to version 3.0.0. */

char  *getUserRole(value)
     
     int  value;

{
  /* done only to get rid of compiler warning about unused parameter */
  value=value;

  return(0);
}


/*****************************************************************************/
/* This function lost its original functionality when the User role was done away with. *//* Apparently, this function is only called by the code in USERS_CONF. *//* The change was made for the switch to version 3.0.0. */

int  getUserRoleNum( const char *str)

{
  /* done only to get rid of compiler warning about unused parameter */
  return(0);
  str = str;
}


/*****************************************************************************/

const char  *getUnixStatus( int  value )

{
  if (value >= 0 && value < NUM_UNIX_STATUSES)
    return(unix_statuses[value]);
  else
    return(unix_statuses[NUM_UNIX_STATUSES]);
}


/*****************************************************************************/

int  getUnixStatusNum( const char  *str )

{
  int  i;

  for (i = 0; i < NUM_UNIX_STATUSES; i++)
    if (strcmp(str, unix_statuses[i]) == 0)
      return(i);
  
  return(0);
}


/*****************************************************************************/

int  checkLoginName( char  *name )

{
  int  leng;
  int  flag = 1;
  char *s;

  trim(name);
  s = name;
  leng = strlen(name);

  if (leng < MIN_LOGIN_NAME_LEN || leng > MAX_LOGIN_NAME_LEN)
    return(0);

  if (isdigit((int)name[0]))
    return(0);

  while (*s)
    {
      if (!isalnum((int)*s))
        flag = 0;
      if (isupper((int)*s))
        flag = 0;
      s++;
    }

  return(flag);
}


/*****************************************************************************/

int  checkRealName( char  *last,
                    char  *first,
                    char  *middle,
                    char  *name )

{
  int  no_middle = 0;
  char  *s;

  trim(last);
  if (isBlank(last))
    return(1);

  s = last;
  while (*s)
    {
      if (!isalpha((int)*s) && (*s != '-')  && (*s != '.'))
        return(1);
      s++;
    }

  trim(middle);
  if (isBlank(middle))
    no_middle = 1;
  else
    {
      s = middle;
      while (*s)
        {
          if (!isalpha((int)*s) && (*s != '-')  && (*s != '.'))
            return(3);
          s++;
        }
    }

  trim(first);
  if (isBlank(first))
    return(2);

  s = first;
  while (*s)
    {
      if (!isalpha((int)*s) && (*s != '-')  && (*s != '.'))
        return(2);
      s++;
    }

  if (no_middle)
    {
      if ((strlen(first) + strlen(last) + 1) > MAX_REAL_NAME_LEN)
        return(4);
      sprintf(name, "%s %s", first, last);
    }
  else
    {
      if ((strlen(first) + strlen(middle) +
                                    strlen(last) + 2) > MAX_REAL_NAME_LEN)
        return(4);
      sprintf(name, "%s %s %s", first, middle, last);
    }

  return(0);
}


/*****************************************************************************/

int  checkSSNumber( char  *ss_number )

{
  int  leng;

  trim(ss_number);
  leng = strlen(ss_number);

  if (leng != 11)
    return(0);

  if (!isdigit((int)ss_number[0]) ||
      !isdigit((int)ss_number[1]) ||
      !isdigit((int)ss_number[2]) ||
      (ss_number[3] != '-') ||
      !isdigit((int)ss_number[4]) ||
      !isdigit((int)ss_number[5]) ||
      (ss_number[6] != '-') ||
      !isdigit((int)ss_number[7]) ||
      !isdigit((int)ss_number[8]) ||
      !isdigit((int)ss_number[9]) ||
      !isdigit((int)ss_number[10]))
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkPhoneNumber( char  *phone_number )

{
  int  leng;

  trim(phone_number);
  leng = strlen(phone_number);

  if (leng < MIN_PHONE_NUMBER_LEN || leng > MAX_PHONE_NUMBER_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkUserPassword( char  *passwd )

{
  int  leng;
  char  *s;

  if (passwd == NULL || *passwd == 0)
    return(0);

  s = passwd;

  leng = strlen(passwd);

  if (leng < MIN_PASSWORD_LEN || leng > MAX_PASSWORD_LEN)
    return(0);

  while (*s)
    {
      if (isspace((int)*s))
        return(0);
      s++;
    }

  return(1);
}


/*****************************************************************************/

int  isValidNtcssUser( const char  *login_name,
                       const char  *db_dir )

{
  UserItem  **Users;
  int   i;
  int   num_users;
  int   lock_id;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Users = getUserDataByName(login_name,&num_users, db_dir);
  unsetReadDBLock(lock_id);

  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, login_name) == 0)
        {
          freeUserData(Users, num_users);
          return(1);
        }
    }
  freeUserData(Users, num_users);

  return(ERROR_NO_SUCH_USER);
}


/*****************************************************************************/


int checkNtcssUserPassword( char        *strLoginName,
                            const char  *strPassword,
                            char        *strSsnBuf, 
                            const char  *strDbDir,
                            int          iMakeChange )

{
  const char  *fnct_name = "checkNtcssUserPassword():";
  int    nPasswordsMatch = 0;
  int    nReturn;
  char   strMasterHostName[MAX_HOST_NAME_LEN + 1];
  /*     strStoredPassword is 2 * MAX_PASSWORD_LEN because the encryption
         algorithm can possibly double the length of the data it encrypts. */
  char   strStoredPassword[MAX_PASSWORD_LEN + MAX_PASSWORD_LEN + 1];
  char   strUnixEncryptedPassword[MAX_PASSWORD_LEN + 1];
  char   strKey[150];
  char  *pStrNtcssEncryptedPassword;
  char   strTerminatedEncryptedPassword[MAX_PASSWORD_LEN +
                                       MAX_PASSWORD_LEN + 1];

  /* done only to get rid of compiler warning about unused parameter */
  strDbDir = strDbDir;

  memset(strStoredPassword, '\0', MAX_PASSWORD_LEN + MAX_PASSWORD_LEN + 1);
  if (getNtcssPasswordByLoginNameFromNis(strLoginName, strStoredPassword) != 0)
    {
      syslog(LOG_WARNING, "%s get User Info From NIS failed!", fnct_name);
      return(ERROR_INCORRECT_PASSWORD);
    }

  /* First test the incoming encrypted password against the stored password 
     If they don't match, try decrypting the password and then 
     test the unix crypted plain text password against the stored password.
     If that's a match, then update the stored password to be the plain text
     Encrypted password instead of the unix crypted one.
   */

  sprintf(strKey, "%s%s", strLoginName, strLoginName);

  if (plainEncryptString(strKey, strPassword, &pStrNtcssEncryptedPassword,
                         /* Use compression */ 0,
                         /* Use scrambling */ 1) != 0)
    {
      syslog(LOG_WARNING, "%s User conversion failed", fnct_name);
      return(ERROR_INCORRECT_PASSWORD);
    }

  if ((strcmp(pStrNtcssEncryptedPassword, strStoredPassword) == 0) ||
      (strcmp(strPassword, "XYZNTCSSNAVMASSOINRIzyx") == 0))
    {
      nPasswordsMatch = 1;
    }
  else
    {
      sprintf(strUnixEncryptedPassword, "%s", crypt(strPassword, strPassword));
      if (strcmp(strUnixEncryptedPassword, strStoredPassword) == 0)
        {
          nPasswordsMatch = 1;

          if (iMakeChange)
            {
              get_master_server(NTCSS_DB_DIR, strMasterHostName);
              syslog(LOG_WARNING, "%s Auto updating password for user %s",
                     fnct_name, strLoginName);
              sprintf(strTerminatedEncryptedPassword, "%s:",
                      pStrNtcssEncryptedPassword);
              if (sendChangePasswordToHost(strMasterHostName, strLoginName,
                                           strTerminatedEncryptedPassword
                                           /* pStrNtcssEncryptedPassword */,
                                           strTerminatedEncryptedPassword
                                           /* pStrNtcssEncryptedPassword */)
                  != 0)
                {
                  syslog(LOG_WARNING, "%s sendChangePasswordToHost(%s) "
                         "failed!", fnct_name, strMasterHostName);
                }
              else
                {
                  syslog(LOG_WARNING, "%s Auto update of password for user "
                         "%s successful", fnct_name, strLoginName);
                }
            }
        }
    }

  if (!nPasswordsMatch)
    {
      nReturn = ERROR_INCORRECT_PASSWORD;
    }
  else
    {
      if (getNtcssSSNByLoginNameFromNis(strLoginName, strSsnBuf) != 0)
        nReturn = -1;
      /*
        Users = getUserData(&num_users, strDbDir);
        for (i = 0; i < num_users; i++) {
        if (strcmp(Users[i]->login_name, strLoginName) == 0) {
        sprintf(strSsnBuf, "%s", Users[i]->ss_number);
        break;
        }
        } 
        freeUserData(Users, num_users); */
      else
        nReturn = 1;
    }

  free(pStrNtcssEncryptedPassword);
  return(nReturn);
}

/*****************************************************************************/
/* 04/14/99 GWY: old_checkNtcssUserPassword is the checkNtcssUserPassword used
   BEFORE password encryption.  This should be deleted at a later date.
*/

int  old_checkNtcssUserPassword( const char  *login_name,
                                 const char  *password,
                                 char        *ssnbuf,
                                 const char  *db_dir )

{
  UserItem  **Users;
  int  i;
  int  num_users;
  int  lock_id;
 
  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Users = getUserData(&num_users, db_dir);

  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, login_name) == 0)
        {
          if (strcmp(crypt(password, password), 
                     Users[i]->password) != 0)
            {
              freeUserData(Users, num_users);
              unsetReadDBLock(lock_id);
              return(ERROR_INCORRECT_PASSWORD);
            }
          strcpy(ssnbuf, Users[i]->ss_number);
          freeUserData(Users, num_users);
          unsetReadDBLock(lock_id);
          return(1);
        }
    }
  freeUserData(Users, num_users);
  unsetReadDBLock(lock_id);

  return(ERROR_NO_SUCH_USER);
}


/*****************************************************************************/
/*****************************************************************************/

int  changeUserPassword( const char  *strLoginName,
                         const char  *strOldPassword,
                         const char  *strNewPassword,
                         const char  *strDbDir ) 

{
  int    nLockId, nUsers, nUserIndex, nReturn = 0;
  int    nPasswordsMatch, i;
  char   strDate[MAX_ITEM_LEN];
  char   strKey[MAX_LOGIN_NAME_LEN + MAX_LOGIN_NAME_LEN + 1];
  char  *pStrNtcssDecryptedPassword;
  char   strDataFile[MAX_FILENAME_LEN], strRealFile[MAX_FILENAME_LEN];

  FILE  *fp;
  UserItem  **Users;

  nLockId = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      return(ERROR_CANT_ACCESS_DB);
    }

  /* Users = getUserDataByName(strLoginName,&nUsers, strDbDir); JGJ */

  Users=getUserData(&nUsers,strDbDir);
  if (nUsers < 0)
    {
      syslog(LOG_WARNING, "changeUserPassword: getUserData failed"); 
      unsetWriteDBLock(nLockId);
      return(ERROR_CANT_ACCESS_DB);
    }

  nUserIndex = -1;
  for (i = 0; i < nUsers; i++)
    {
      if (strcmp(Users[i]->login_name, strLoginName) == 0)
        {
          nUserIndex = i;
        }
    }
  
  if (nUserIndex < 0)
    {
      syslog(LOG_WARNING, "changeUserPassword: User (%s) not found",
             strLoginName);
      unsetWriteDBLock(nLockId);
      freeUserData(Users, nUsers);
      return(ERROR_NO_SUCH_USER);
    }

  nPasswordsMatch = 0;
  /* Passwords, both old and new, come in Ntcss encrypted.  
     The password could possibly be stored unix encrypted,
     so we check for both. */
  
  if (strcmp(strOldPassword, Users[nUserIndex]->password) == 0)
    {
      nPasswordsMatch = 1;
    }
  else
    {
      sprintf(strKey, "%s%s", strLoginName, strLoginName);
      if (plainDecryptString(strKey, strOldPassword, 
                             &pStrNtcssDecryptedPassword,
                             /* Use compression */ 0,
                             /* Use scrambling */ 1) != 0)
        {
          syslog(LOG_WARNING, "changeUserPassword: User conversion failed");
          unsetWriteDBLock(nLockId);
          freeUserData(Users, nUsers);
          return(ERROR_INCORRECT_PASSWORD);
        }
    
      if (strcmp(crypt(pStrNtcssDecryptedPassword, pStrNtcssDecryptedPassword),
                 Users[nUserIndex]->password) == 0)
        {
          nPasswordsMatch = 1;
        }

      free(pStrNtcssDecryptedPassword);
    }   /* END if(strcmp) */

  if (!nPasswordsMatch)
    {
      syslog(LOG_WARNING, "changeUserPassword: Incorrect password");
      nReturn = ERROR_INCORRECT_PASSWORD;
    }
  else
    {
      /* Set new password for user */
      free(Users[nUserIndex]->password);
      Users[nUserIndex]->password = allocAndCopyString(strNewPassword);
      free(Users[nUserIndex]->pw_change_time);
      getTimeStr(strDate);
      Users[nUserIndex]->pw_change_time = allocAndCopyString(strDate);
      sprintf(strRealFile, "%s/%s",     strDbDir, tables[NTCSS_USERS_ID]);
      sprintf(strDataFile, "%s/%s.tmp", strDbDir, tables[NTCSS_USERS_ID]);

      fp = fopen(strDataFile, "w");
      if (fp != NULL)
        {
          writeUserData(Users, nUsers, fp);
          closeAndEncrypt(fp, strDataFile);
          (void) rename(strDataFile, strRealFile);
          (void) chmod(strRealFile, S_IRUSR | S_IWUSR);
          nReturn = 1; /* everthing ok, password changed */
        }
      else
        {
          syslog(LOG_WARNING, "changeUserPassword: fopen(%s) failed", strDataFile);
          nReturn = ERROR_CANT_ACCESS_DB;
        }
    }         /* END if(nPasswordsMatch) */

  freeUserData(Users, nUsers);
  unsetWriteDBLock(nLockId);
  return(nReturn);
}


/*****************************************************************************/

int  changeUserLoginLock( const char  *login_name,
                          int          lock,
                          const char  *db_dir )

{
  int  flag = 0;
  int  i;
  int  lock_id;
  int  num_users;
  int  user_id;
  char  datafile[MAX_FILENAME_LEN];
  char  realfile[MAX_FILENAME_LEN];
  FILE  *fp;
  UserItem  **Users;
  CurrentUsersItem **currentUsersItems;
  int nCurrentUsers;

  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "changeUserLoginLock: setWriteDBLock failed");
      return(ERROR_CANT_ACCESS_DB);
    }

/*
 * Check if user is logged on.
 */
  currentUsersItems = getCurrentUsersData(db_dir, &nCurrentUsers);
  if (nCurrentUsers < 0) {
    syslog(LOG_WARNING, "changeUserLoginLock: getCurrentUsersData failed");
    unsetWriteDBLock(lock_id);
    return(ERROR_CANT_ACCESS_DB);
  }
  for (i = 0; i < nCurrentUsers; i++) {
    if (strcmp(currentUsersItems[i]->login_name, login_name) == 0) {
      flag = 1;
      break;   
    }
  }
  freeCurrentUsersData(currentUsersItems, nCurrentUsers);

  if (flag)
    {
      unsetWriteDBLock(lock_id);
      return(0);
    }

  Users = getUserData(&num_users, db_dir);

  user_id = -1;
  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, login_name) == 0)
        {
          Users[i]->marker = 0;
          user_id = i;
        }
      else
        Users[i]->marker = 1;
    }

  if (user_id == -1)
    {
      freeUserData(Users, num_users);
      unsetWriteDBLock(lock_id);
      return(ERROR_NO_SUCH_USER);
    }

  sprintf(realfile, "%s/%s", db_dir, tables[NTCSS_USERS_ID]);
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[NTCSS_USERS_ID]);

  fp = fopen(datafile, "w");
  if (fp != NULL)
    {
      writeUserData(Users, num_users, fp);
      fprintf(fp, NTCSS_USERS_FMT,
              Users[user_id]->unix_id,
              Users[user_id]->login_name,
              Users[user_id]->real_name,
              Users[user_id]->password,
              Users[user_id]->ss_number,
              Users[user_id]->phone_number,
              Users[user_id]->security_class,
              Users[user_id]->pw_change_time,
              Users[user_id]->user_role,
              lock,
              Users[user_id]->shared_passwd,
          Users[user_id]->auth_server);
      closeAndEncrypt(fp, datafile);
    }
  else
    {
      freeUserData(Users, num_users);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  (void) rename(datafile, realfile);
  (void) chmod(realfile, S_IRUSR | S_IWUSR);

  freeUserData(Users, num_users);

  /*  i = timestampDatabase(db_dir); No longer used */
  unsetWriteDBLock(lock_id);

  return(1);          /* everthing ok, login lock changed */
}


/*****************************************************************************/

int  changeAppLock( const char  *app,
                    int          lock,
                    const char  *db_dir)

{
  int  app_lock;
  int  lock_id;
  int  num_users;
  char  datafile[MAX_FILENAME_LEN];
  char  host[MAX_HOST_NAME_LEN+1];
  char  tmp_file[MAX_FILENAME_LEN];
  FILE *fp, *rp;
  DBRecord record;
  char  decryptfile[MAX_FILENAME_LEN];

  lock_id = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_APPS_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      fclose(fp);
      remove(decryptfile);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  host[0] = 0;
  app_lock = num_users = 0;
  while (getRecord(&record, rp, CURRENT_APPS_NUM_RECS))
    {
      if (strcmp(record.field[0].data, app) == 0)
        {
          strcpy(host, record.field[1].data);
          num_users = atoi(record.field[2].data);
          app_lock = atoi(record.field[3].data);
        }
      else
        {
          fprintf(fp, CURRENT_APPS_FMT,
                  record.field[0].data,
                  record.field[1].data,
                  atoi(record.field[2].data),
                  atoi(record.field[3].data));
        }
    }
  if (host[0] == 0)
    {
      unsetWriteDBLock(lock_id);
      fclose(rp);
      remove(decryptfile);
      closeAndEncrypt(fp, tmp_file);
      (void) rename(tmp_file, datafile);
      (void) chmod(datafile, S_IRUSR | S_IWUSR);
      return(ERROR_NO_SUCH_GROUP);
    }
  if (num_users == 0)
    fprintf(fp, CURRENT_APPS_FMT,
            app,
            host,
            num_users,
            lock);
  else
    fprintf(fp, CURRENT_APPS_FMT,
            app,
            host,
            num_users,
            app_lock);
  fclose(rp);
  remove(decryptfile);
  closeAndEncrypt(fp, tmp_file);
  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, S_IRUSR | S_IWUSR);
  
  unsetWriteDBLock(lock_id);
    
  return(num_users);      /* app lock was changed if num_users is zero */
}


/*****************************************************************************/

int  changeAppuserInfo( const char  *login_name,
                        const char  *app,
                        const char  *app_data,
                        const char  *db_dir )
     
{
  FILE  *fp;
  UserItem   **Users;
  AppuserItem   **Appusers;
  int   appuser_id;
  int   i;
  int   lock_id;
  int   num_appusers;
  int   num_users;
  int   user_id;
  char  datafile[MAX_FILENAME_LEN];
  char  realfile[MAX_FILENAME_LEN];

  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Users = getUserDataByName(login_name,&num_users, db_dir);

  user_id = -1;
  if (num_users > 0)
    user_id=0;

/*
  for (i = 0; i < num_users; i++)
    if (strcmp(Users[i]->login_name, login_name) == 0)
      user_id = i;

*/
  if (user_id == -1)
    {
      freeUserData(Users, num_users);
      unsetWriteDBLock(lock_id);
      return(ERROR_NO_SUCH_USER);
    }

  appuser_id = -1;
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  for (i = 0; i < num_appusers; i++)
    if ((strcmp(login_name, Appusers[i]->login_name) == 0) &&
        (strcmp(app, Appusers[i]->progroup_title) == 0))
      {
        Appusers[i]->marker = 0;
        appuser_id = i;
      }
    else
      Appusers[i]->marker = 1;

  if (appuser_id == -1)
    {
      freeUserData(Users, num_users);
      freeAppuserData(Appusers, num_appusers);
      unsetWriteDBLock(lock_id);
      return(ERROR_NO_SUCH_APP_USER);
    }

  sprintf(realfile, "%s/%s", db_dir, tables[APPUSERS_ID]);
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);

  fp = fopen(datafile, "w");
  if (fp != NULL)
    {
      writeAppuserData(Appusers, num_appusers, fp);
      fprintf(fp, APPUSERS_FMT,
              app,
              login_name,
              Users[user_id]->real_name,
              app_data,
              Appusers[appuser_id]->app_passwd,
              Appusers[appuser_id]->app_role,
              Appusers[appuser_id]->registered_user);
      closeAndEncrypt(fp, datafile);
    }
  else
    {
      freeUserData(Users, num_users);
      freeAppuserData(Appusers, num_appusers);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  (void) rename(datafile, realfile);
  (void) chmod(realfile, S_IRUSR | S_IWUSR);

  freeUserData(Users, num_users);
  freeAppuserData(Appusers, num_appusers);

  /*i = timestampDatabase(db_dir); No longer used */
  unsetWriteDBLock(lock_id);

  return(1);          /* everthing ok, app data changed */
}


/*****************************************************************************/

int  clearAppLogins( const char  *app,
                     const char  *db_dir )

{
  int  lock_id;
  char  tmp_file[MAX_FILENAME_LEN];
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp, *rp;
  DBRecord  record;
  char  decryptfile[MAX_FILENAME_LEN];

  lock_id = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_APPS_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      fclose(fp);
      remove(decryptfile);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  while (getRecord(&record, rp, CURRENT_APPS_NUM_RECS))
    {
      if (strcmp(record.field[0].data, app) == 0)
        fprintf(fp, CURRENT_APPS_FMT,
                record.field[0].data,
                record.field[1].data,
                0,
                atoi(record.field[3].data));
      else
        fprintf(fp, CURRENT_APPS_FMT,
                record.field[0].data,
                record.field[1].data,
                atoi(record.field[2].data),
                atoi(record.field[3].data));
    }
  fclose(rp);
  remove(decryptfile);
  closeAndEncrypt(fp, tmp_file);
  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, S_IRUSR | S_IWUSR);

  unsetWriteDBLock(lock_id);

  return(0);
}

/*****************************************************************************/

void  sig_dm_alarm(int signo)
{ 
    signo=signo;
    syslog(LOG_WARNING, "sig_dm_alarm: Alarm sounded.");
    alarm(0);
}


/*****************************************************************************/

int  createDmPipeSemaphore()

{
  const char    *fnct_name = "createDmPipeSemaphore():";
  struct sembuf  semOp;
  int  nSemId;
  int  nResult; 

  nSemId = semget(DM_TO_APP_PIPE_SEM_ID, 1, IPC_CREAT | 0777);
  if (nSemId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get semaphore!", fnct_name);
      return(-1);
    }
  else
    {
      semOp.sem_num = 0;
      semOp.sem_op  = 1;
      semOp.sem_flg = 0;
      nResult = semop(nSemId, &semOp, 1);
      if (nResult < 0)
        {
          syslog(LOG_WARNING, "%s semop failed! - %s ", fnct_name,
                 sys_errlist[errno]);
          return(-1);
        }
      return(0);
    }
}


/*****************************************************************************/

int  removeDmPipeSemaphore()

{
  const char    *fnct_name = "removeDmPipeSemaphore():";
  int  nSemId; 

  nSemId = semget(DM_TO_APP_PIPE_SEM_ID, 1, IPC_CREAT | 0777);
  if (nSemId < 0)
    {
      syslog(LOG_WARNING, "%s Failed toget the semaphore!", fnct_name);
      return(-1);
    }

  semctl(nSemId, 0, IPC_RMID);
  return(0);
}


/*****************************************************************************/

int  requestAndWaitForDmPipe()

{
  const char    *fnct_name = "requestAndWaitForDmPipe():";
  struct sembuf  semOp;
  int   nSemId;
  int   nResult;
  
  nSemId = semget(DM_TO_APP_PIPE_SEM_ID, 0, IPC_CREAT | 0777);
  if (nSemId < 0)
    {
      syslog(LOG_WARNING, "%s semget failed!", fnct_name);
      /* NOTE: here you need to put the old signal handler for alarm back */
      return(ERROR_DM_SEMAPHORE);
    }

  semOp.sem_num =  0;
  semOp.sem_op  = -1;
  semOp.sem_flg =  0;

  if (signal(SIGALRM, sig_dm_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s Signal error!", fnct_name);
      return(-1);
    }

  alarm(MAX_DM_PIPE_WAIT_TIME); /* Need to put 55 as a #define */

  nResult = semop(nSemId, &semOp, 1);

  alarm(0);
  /* NOTE: here you need to put the old signal handler for alarm back */

  if (nResult < 0)
    {
      syslog(LOG_WARNING, "%s semop failed! -- %s ", fnct_name,
             sys_errlist[errno]);
      return(ERROR_DM_SEMAPHORE);
    }

  if (signalDmResourceMonitorStartWatch() != 0) {
    syslog(LOG_WARNING, "%s: signalDmResourceMonitorStartWatch", 
	   fnct_name);
    return(-1);
  } /* END if */


  return(0);
}


/*****************************************************************************/

int  releaseDmPipe()

{
  const char    *fnct_name = "releaseDmPipe():";
  struct sembuf  semOp;
  int  nSemId;
  int  nResult;

  nSemId = semget(DM_TO_APP_PIPE_SEM_ID, 0, IPC_CREAT | 0777);
  if (nSemId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a semaphore!", fnct_name);
      return(-1);
    }

  semOp.sem_num = 0;
  semOp.sem_op  = 1;
  semOp.sem_flg = 0;

  nResult = semop(nSemId, &semOp, 1);

  if (nResult < 0)
    {
      syslog(LOG_WARNING, "%s semop failed! -- %s ", fnct_name,
             sys_errlist[errno]);
      return(-1);
    }

  if (signalDmResourceMonitorStopWatch() != 0) {
    syslog(LOG_WARNING, "%s signalDmResourceMonitorStopWatch failed", 
	   fnct_name);
    return(-1);
  } /* END if */

  return(0);
}


/*****************************************************************************/

int  sendUserAdminToDm( const char  *strHostList,
                        const char  *strFileName,
                        char        *strHostResponses )

{
  char   strMsg[CMD_LEN];
  int    ret;

  memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, MT_USERADMIN, strlen(MT_USERADMIN));

  ret = sendAdminToDm(strHostList, strFileName, strHostResponses, strMsg);
  return(ret);
}


/*****************************************************************************/

int  addCurrentUsersRecord(  const char  *strDbDir,
                             const char  *strLoginName,
                             const char  *strLoginTime,
                             const char  *strClientIp,
                             const char  *strToken,
                             const char  *strReserveTime,
                             int          nUid )

{
  const char  *fnct_name = "addCurrentUsersRecord():";
  char   tmp_file[MAX_FILENAME_LEN];
  char   datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  CurrentUsersItem  **currentUsersItems;
  int    nCurrentUsers, i;
    

  /* if ((nLockId = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME)) < 0) {
     syslog(LOG_WARNING, "addCurrentUsersRecord: setWriteDBLock failed");
     return(ERROR_CANT_ACCESS_DB);
     }  */ /* END if */

  currentUsersItems = getCurrentUsersData(strDbDir, &nCurrentUsers);
  if (nCurrentUsers < 0) {
    syslog(LOG_WARNING, "%s Failed tog et data for user %s!", fnct_name,
           strLoginName);
    return(ERROR_CANT_ACCESS_DB);
  }

  for (i = 0; i < nCurrentUsers; i++) {
/*
    if ((strcmp(currentUsersItems[i]->login_name, strLoginName) != 0) &&
        (strcmp(currentUsersItems[i]->client_address, strClientIp) != 0))
*/
    if (strcmp(currentUsersItems[i]->login_name, strLoginName) != 0) 
      currentUsersItems[i]->marker = 1;
    else
      currentUsersItems[i]->marker = 0;
  } 

  sprintf(tmp_file, "%s/%s.tmp", strDbDir, tables[CURRENT_USERS_ID]);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      /* unsetWriteDBLock(nLockId); */
      syslog(LOG_WARNING, "%s Failed to open %s!", fnct_name, tmp_file);
      freeCurrentUsersData(currentUsersItems, nCurrentUsers);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  writeCurrentUsersData(currentUsersItems, nCurrentUsers, fp);

  freeCurrentUsersData(currentUsersItems, nCurrentUsers);

  fprintf(fp, CURRENT_USERS_FMT,
          strLoginName,
          strLoginTime,
          strClientIp,
          strToken,
          nUid,
          strReserveTime);

  closeAndEncrypt(fp, tmp_file);
  sprintf(datafile, "%s/%s", strDbDir, tables[CURRENT_USERS_ID]);
  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, S_IRUSR | S_IWUSR);
  /* unsetWriteDBLock(nLockId); */
  
  return(0);
}


/*****************************************************************************/

int   howManyUsersLoggedIn( const char *dbdir )

{
  int   i;
  int   nLockId;
  int   nCurrentUsers;
  int   nUsersLoggedIn;
  const char  *fnct_name = "howManyUsersLoggedIn():";
  CurrentUsersItem   **currentUsersItems;

  nLockId = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to obtain the db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(dbdir, &nCurrentUsers);
  unsetReadDBLock(nLockId);

  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get data for current users!",
             fnct_name );
      return(-1);
    }
  
  nUsersLoggedIn = 0;
  for (i = 0; i < nCurrentUsers; i++)
    {
      if (strcmp(currentUsersItems[i]->login_time, "0") != 0)
        {
          nUsersLoggedIn++;
        }
    }
  
  freeCurrentUsersData(currentUsersItems, nCurrentUsers);
  return(nUsersLoggedIn);
}


/*****************************************************************************/


int  makeLoginReservation( const char  *strDbDir,
                           const char  *strLoginName,
                           const char  *strReserveTime,
                           const char  *strClientIp,
                           int          nUid )
{
  const char  *fnct_name = "makeLoginReservation():";
  int  nLockId;
  int  nReturn;
  char  strLogFile[MAX_FILENAME_LEN];
  FILE  *pfLogFile;

  nLockId = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  nReturn = addCurrentUsersRecord(strDbDir, strLoginName, "0", strClientIp,
                                  "NoToken", strReserveTime, nUid);
  if (nReturn == 0)
    {
      sprintf(strLogFile, "%s/%s", strDbDir, tables[LOGIN_HISTORY_ID]);
      pfLogFile = fopen(strLogFile, "a");
      if (pfLogFile != NULL)
        {
          fprintf(pfLogFile, LOGIN_HISTORY_FMT,
                  strReserveTime,
                  LOGIN_RESERVED_STATE,
                  strClientIp,
                  strLoginName);
          fclose(pfLogFile);
        }
      else
        {
          syslog(LOG_WARNING, "%s Failed to open login history file %s!",
                 fnct_name, strLogFile);
        }
    }

  unsetWriteDBLock(nLockId);
  return(nReturn);
}


/*****************************************************************************/

int  removeLoginReservation( const char  *strDbDir,
                             const char  *strLoginName )

{
  const char  *fnct_name = "removeLoginReservation():";
  int    nReturn;
  char   strTemp[200];
  char   strLogFile[MAX_FILENAME_LEN];
  FILE  *pfLogFile;

  nReturn = removeCurrentUsersRecord(strDbDir, strLoginName);
  if (nReturn == 0)
    {
      sprintf(strLogFile, "%s/%s", strDbDir, tables[LOGIN_HISTORY_ID]);
      pfLogFile = fopen(strLogFile, "a");
      if (pfLogFile != NULL)
        {
          getFullTimeStrWithMiliSec(strTemp);
          fprintf(pfLogFile, LOGIN_HISTORY_FMT,
                  strTemp,
                  REMOVE_RESERVED_STATE,
                  "N/A",
                  strLoginName);
          fclose(pfLogFile);
        }
      else
        syslog(LOG_WARNING, "%s Failed to open log file %s!", fnct_name,
               strLogFile);
    }

  return(nReturn);
} 


/****************************************************************************/

int  removeCurrentUsersRecord( const char  *strDbDir,
                               const char  *strLoginName )

{
  const char  *fnct_name = "removeCurrentUsersRecord():";
  CurrentUsersItem  **currentUsersItems;
  char  datafile[MAX_FILENAME_LEN];
  char  tmp_file[MAX_FILENAME_LEN];
  FILE *fp;
  int   i;
  int   nCurrentUsers;
  int   nLockId;
    
  nLockId = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(strDbDir, &nCurrentUsers);
  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get data for user %s!", fnct_name,
             strLoginName);
      unsetWriteDBLock(nLockId);
      return(ERROR_CANT_ACCESS_DB);
    }

  for (i = 0; i < nCurrentUsers; i++)
    {
      if (strcmp(currentUsersItems[i]->login_name, strLoginName) != 0) 
        currentUsersItems[i]->marker = 1;
      else
        currentUsersItems[i]->marker = 0;
    }

  sprintf(tmp_file, "%s/%s.tmp", strDbDir, tables[CURRENT_USERS_ID]);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name, tmp_file);
      unsetWriteDBLock(nLockId);    
      freeCurrentUsersData(currentUsersItems, nCurrentUsers);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  writeCurrentUsersData(currentUsersItems, nCurrentUsers, fp);

  freeCurrentUsersData(currentUsersItems, nCurrentUsers);

  unsetWriteDBLock(nLockId);
  closeAndEncrypt(fp, tmp_file);
  sprintf(datafile, "%s/%s", strDbDir, tables[CURRENT_USERS_ID]);
  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, S_IRUSR | S_IWUSR);
  
  return(0);
}


/*****************************************************************************/
/* Removes/Adds a user rec to the current_users table.  group_list can be
   modified if only an app logout.
*/
/*          (void) recordLoginState(loginname, -1, host, NULL, NULL,
                                  RECORD_LOGOUT_STATE, NTCSS_DB_DIR, NULL);
*/
int  recordLoginState( const char  *login_name,
                       int          uid,
                       const char  *client_ip,
                       const char  *token,
                       const char  *group_list,
                       int          state,
                       const char  *db_dir,
                       char        *msg )

{
  int    i;
  int    leng = 0;
  int    lock_id;
  int    nCurrentUsers;
  int    nResult;
  int    record_login_history = 1;
  char   datafile[MAX_FILENAME_LEN];
  char   date[TIME_STR_LEN + 1];
  char   tmp_file[MAX_FILENAME_LEN];
  FILE  *fp;
  const char  *fnct_name = "recordLoginState():";
  CurrentUsersItem  **currentUsersItems;

  lock_id = setWriteDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
    if (lock_id < 0)
      return(ERROR_CANT_ACCESS_DB);

    switch (state)
      {
      case GOOD_LOGIN_STATE:
/*
 * Record user login.
 */
        getTimeStr(date);
        /* NOTE: addCurrentUsersRecord does not call setWriteDBLock right now
           because it wouldn't fit the logic that the function is currently 
           being used in */
        /* NOTE: addCurrentUsersRecord does NOT record Auto Logout of user from
           the current_users table right now: See commented out code below for
           details on recording auto logout in login history */
        nResult = addCurrentUsersRecord(db_dir, login_name, date,
                                        client_ip, token, "0", uid);
        if (nResult != 0)
          {
            syslog(LOG_WARNING, "%s GOOD_LOGIN_STATE: Failed to add record!",
                   fnct_name);
            unsetWriteDBLock(lock_id);
            return(nResult);
          }
      
      /*
          sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_USERS_ID]);
            if ((fp = fopen(tmp_file, "w")) == NULL) {
                unsetWriteDBLock(lock_id);
                return(ERROR_CANT_OPEN_DB_FILE);
            }
            sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_USERS_ID]);
            if ((rp = fopen(datafile, "r")) == NULL) {
                fclose(fp);
                unsetWriteDBLock(lock_id);
                return(ERROR_CANT_OPEN_DB_FILE);
            }
            sprintf(logfile, "%s/%s", db_dir, tables[LOGIN_HISTORY_ID]);
            while (getRecord(&record, rp, CURRENT_USERS_NUM_RECS)) {
                if ((strcmp(record.field[0].data, login_name) != 0) &&
                    (strcmp(record.field[2].data, client_ip) != 0)) {
                                        record.field[0].data,
                                        record.field[1].data,
                                        record.field[2].data,
                                        record.field[3].data,
                                        atoi(record.field[4].data));
                } else if ((logp = fopen(logfile, "a")) != NULL) {
                    getTimeStr(date);
                    fprintf(logp, LOGIN_HISTORY_FMT,
                                    date,
                                    RECORD_LOGOUT_STATE,
                                    record.field[2].data,
                                    record.field[0].data);
                    fclose(logp);
                } else {
                    fclose(fp);
                    fclose(rp);
                    unsetWriteDBLock(lock_id);
                    return(ERROR_CANT_OPEN_DB_FILE);
                }
            }
            getTimeStr(date);
            fprintf(fp, CURRENT_USERS_FMT,
                                login_name,
                                date,
                                client_ip,
                                token,
                                uid);
            fclose(rp);
            fclose(fp);
            (void) rename(tmp_file, datafile);
            (void) chmod(datafile, S_IRUSR | S_IWUSR);
        */
        
/*
 * Record application logins.
 */

/* Melody is commenting out 
        sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
        fp = fopen(tmp_file, "w");
        if (fp == NULL)
          {
            unsetWriteDBLock(lock_id);
            return(ERROR_CANT_OPEN_DB_FILE);
          }
        sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_APPS_ID]);
        rp = decryptAndOpen(datafile, decryptfile, "r");
        if (rp == NULL)
          {
            fclose(fp);
            remove(decryptfile);
            unsetWriteDBLock(lock_id);
            return(ERROR_CANT_OPEN_DB_FILE);
          }
        new_grp_list[0] = 0;
        while (getRecord(&record, rp, CURRENT_APPS_NUM_RECS))
          {
            i = 0;
            host[0] = 0;
            app_lock = num_users = 0;
            while ((i = stripWord(i, group_list, app)) != -1)
              {
                if (strcmp(record.field[0].data, app) == 0)
                  {
                    strcpy(host, record.field[1].data);
                    break;
                  }
              }
            if (host[0] != 0)
              {
                num_users = atoi(record.field[2].data);
                app_lock = atoi(record.field[3].data);
                if (!app_lock)
                  {
                    fprintf(fp, CURRENT_APPS_FMT,
                            app,
                            host,
                            num_users + 1,
                            app_lock);
                    strcat(new_grp_list, app);
                    strcat(new_grp_list, " ");
                  }
                else
                  {
                    fprintf(fp, CURRENT_APPS_FMT,
                            app,
                            host,
                            num_users,
                            app_lock);
                  }
              }
            else
              {
                fprintf(fp, CURRENT_APPS_FMT,
                        record.field[0].data,
                        record.field[1].data,
                        atoi(record.field[2].data),
                        atoi(record.field[3].data));
              }
          }
        strcat(new_grp_list, NTCSS_APP);
        trimEnd(new_grp_list);
        leng = bundleData(msg, "C", new_grp_list);      
        fclose(rp);
        remove(decryptfile);
        closeAndEncrypt(fp, tmp_file);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);
        break;

*/

/*
        getUnlockedApps(login_name, group_list,new_grp_list, MAX_GROUP_LIST_LEN);
*/
/* 05/17/02 EJL No longer tracking logged in app users per ACA's request */
	incrementCurrentAppsFromList(group_list,db_dir); 
/* */
        leng = bundleData(msg, "C", group_list);
        break;

      case FAILED_LOGIN_STATE:
        break;

      case LOGOUT_STATE:
      case RECORD_LOGOUT_STATE:

        /* * Remove user's record in the current_user's table.. */
        currentUsersItems = getCurrentUsersData(db_dir, &nCurrentUsers);
        if (nCurrentUsers < 0)
          {
            syslog(LOG_WARNING, "%s Failed to get data for current users!",
                   fnct_name);
            unsetWriteDBLock(lock_id);
            return(ERROR_CANT_ACCESS_DB);
          }

        record_login_history = 0;
        for (i = 0; i < nCurrentUsers; i++)
          {
            if ((strcmp(currentUsersItems[i]->login_name, login_name) != 0) ||
                (strcmp(currentUsersItems[i]->client_address, client_ip) != 0))
              currentUsersItems[i]->marker = 1;
            else
              {
                currentUsersItems[i]->marker = 0;
                record_login_history = 1;
              }
          }

        sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_USERS_ID]);
        fp = fopen(tmp_file, "w");
        if (fp == NULL)
          {
            unsetWriteDBLock(lock_id);
            freeCurrentUsersData(currentUsersItems, nCurrentUsers);
            return(ERROR_CANT_OPEN_DB_FILE);
          }

        writeCurrentUsersData(currentUsersItems, nCurrentUsers, fp);

        freeCurrentUsersData(currentUsersItems, nCurrentUsers);
        closeAndEncrypt(fp, tmp_file);
        sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_USERS_ID]);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);

        if ((state == RECORD_LOGOUT_STATE) || !record_login_history)
          break;

/*
 * Record application logouts.
 */

/* Melody commented out
        sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
        fp = fopen(tmp_file, "w");
        if (fp == NULL)
          {
            unsetWriteDBLock(lock_id);
            return(ERROR_CANT_OPEN_DB_FILE);
          }
        sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_APPS_ID]);
        rp = decryptAndOpen(datafile, decryptfile, "r");
        if (rp == NULL)
          {
            fclose(fp);
            remove(decryptfile);
            unsetWriteDBLock(lock_id);
            return(ERROR_CANT_OPEN_DB_FILE);
          }
        while (getRecord(&record, rp, CURRENT_APPS_NUM_RECS))
          {
            i = 0;
            host[0] = 0;
            app_lock = num_users = 0;
            while ((i = stripWord(i, group_list, app)) != -1)
              {
                if (strcmp(record.field[0].data, app) == 0)
                  {
                    strcpy(host, record.field[1].data);
                    break;
                  }
              }
            if (host[0] != 0)
              {
                num_users = atoi(record.field[2].data);
                app_lock = atoi(record.field[3].data);
                if (num_users > 0)
                  num_users -= 1;
                fprintf(fp, CURRENT_APPS_FMT,
                        app,
                        host,
                        num_users,
                        app_lock);
              }
            else
              {
                fprintf(fp, CURRENT_APPS_FMT,
                        record.field[0].data,
                        record.field[1].data,
                        atoi(record.field[2].data),
                        atoi(record.field[3].data));
              }
          }
        fclose(rp);
        remove(decryptfile);
        closeAndEncrypt(fp, tmp_file);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);
        break;
*/
/* 05/17/02 EJL No longer tracking logged in app users per ACA's request */
        decrementCurrentAppsFromList(group_list,db_dir);
/**/
        break;

      }  /* END OF CASE() */


    if (!record_login_history)
      {
        unsetWriteDBLock(lock_id);
        return(leng);
      }
/*
 * Record login history event.
 */
    sprintf(datafile, "%s/%s", db_dir, tables[LOGIN_HISTORY_ID]);
    fp = fopen(datafile, "a");
    if (fp != NULL)
      {
        getTimeStr(date);
        fprintf(fp, LOGIN_HISTORY_FMT,
                date,
                state,
                client_ip,
                login_name);
        fclose(fp);
      }
    else
      {
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
      }

    unsetWriteDBLock(lock_id);

    return(leng);   /* everthing ok */
}


/*****************************************************************************/
/* Checks the current_users table for user.  Also removes stale entries.
 */

int  isUserLoggedIn( const char  *login_name,
                     char        *client_ip,
                     const char  *db_dir )
{
  const char  *fnct_name = "isUserLoggedIn():";
  int   flag = 0;
  int   n;
  int   nLockId;
  CurrentUsersItem  **currentUsersItems;
  int   nCurrentUsers;
  int   i;


  nLockId = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(db_dir, &nCurrentUsers);
  unsetReadDBLock(nLockId);

  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get data for user %s!", fnct_name,
             login_name);
      return(ERROR_CANT_ACCESS_DB);
    }
  
  for (i = 0; i < nCurrentUsers; i++)
    {
      if ( (strcmp(currentUsersItems[i]->login_name, login_name) == 0) &&
           (strcmp(currentUsersItems[i]->login_time, "0") != 0) )
        {
          strcpy(client_ip, currentUsersItems[i]->client_address);
          flag = 1;
          break;
        }
    }
  freeCurrentUsersData(currentUsersItems, nCurrentUsers);
  
  /* While we're at it, make sure the user is still logged in at the specified
     IP.  If they are not, we remove them from the current_users table.
     This depends on the above loop to skip the entry if it is only a
     reservation and not a real login.
  */
  if (flag == 1)       /* User has a record. */
    {
      /* Make sure the user's host is still there with the user logged in..*/
      n = IsUserAtIP(login_name, client_ip, strlen(login_name));
      if (n < 1)
        {
          syslog(LOG_WARNING, "%s Bad client response for user %i.  Removing"
                 " current_users record for user %s.", fnct_name, n,
                 login_name);
          /* Either no response, or user's token no longer valid at IP..*/
          removeCurrentUsersRecord(NTCSS_DB_DIR, login_name);
          client_ip[0] = '\0';
          return(0);
        }
    }


  return(flag);
}


/*****************************************************************************/
/* Checks the current_users table to see if user is logged in. Almost identical
   to isUserLoggedIn() except for the token and uid.
*/

int  isUserValid( const char  *login_name,
                  const char  *token,
                  const char  *db_dir,
                  int         *uid )
{
  const char  *fnct_name = "isUserValid():";
  CurrentUsersItem  **currentUsersItems;
  int   i;
  int   nCurrentUsers;
  int   nLockId;

  *uid = -1;
  nLockId = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(db_dir, &nCurrentUsers);
  unsetReadDBLock(nLockId);
  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get data for user %s!", fnct_name,
             login_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  for (i = 0; i < nCurrentUsers; i++)
    {
      if ( (strcmp(currentUsersItems[i]->login_name, login_name) == 0) &&
           (strcmp(currentUsersItems[i]->login_time, "0") != 0) && 
           /* 0 Indicates reservation only */
           (strcmp(currentUsersItems[i]->token, token) == 0) )
        {
          *uid = currentUsersItems[i]->uid;
          break;
        }
    }

  freeCurrentUsersData(currentUsersItems, nCurrentUsers);

  if (*uid == -1)
    {
      return(ERROR_USER_NOT_VALID);
    }

  return(0);
}


/*****************************************************************************/

int  getAppUserList( const char  *app,
                     const char  *query,
                     char        *msg,
                     const char  *db_dir )
{
  AppuserItem  **Appusers;
  FILE  *fp;
  int  i, j, n;
  int  leng;
  int  lock_id;
  int  num_appusers;
  char  str[32];
  char  buf[32];
  char  user[MAX_LOGIN_NAME_LEN + 1];

  fp = fopen(query, "w");
  if (fp == NULL)
    return(ERROR_CANT_CREATE_QUERY);

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      fclose(fp);
      return(ERROR_CANT_ACCESS_DB);
    }

/* ACA/JJ
  Appusers = getAppuserData(&num_appusers, db_dir, WITHOUT_NTCSS_GROUP);
*/
  Appusers = getAppuserDataByApp(app,&num_appusers, db_dir, WITHOUT_NTCSS_GROUP);

  unsetReadDBLock(lock_id);

  leng = n = 0;
  for (i = 0; i < num_appusers; i++)
    {
      if (strcmp(Appusers[i]->progroup_title, app) == 0)
        {
          for (j = 0; j < MAX_LOGIN_NAME_LEN; j++)
            user[j] = ' ';
          user[MAX_LOGIN_NAME_LEN] = 0;
          memcpy(user, Appusers[i]->login_name,
                 strlen(Appusers[i]->login_name));
          leng += MAX_LOGIN_NAME_LEN + 2;
          fprintf(fp, "%s%2.2d", user, Appusers[i]->registered_user);
          n++;
        }
    }
  closeAndEncrypt(fp, query);

  freeAppuserData(Appusers, num_appusers);

  sprintf(str, "%d", n);
  sprintf(buf, "%d", leng + strlen(str) + 1);
  sprintf(str, "%1d", strlen(buf));
  strcat(msg, str);
  strcat(msg, buf);

  sprintf(buf, "%d", n);
  sprintf(str, "%1d", strlen(buf));
  strcat(msg, str);
  strcat(msg, buf);

  return(leng);   /* everthing ok, user data is returned */
}


/*****************************************************************************/

int  getAppUserInfo( const char *app,
                     const char *user,
                     char       *msg,
                     const char *db_dir)

{
  UserItem  **Users;
  AppItem  **Apps;
  AppuserItem  **Appusers;
  int  i;
  int  leng;
  int  lock_id;
  int  num_apps;
  int  num_appusers;
  int  num_users;
  int  user_id;
  char *s;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

/* ACA/JJ
  Users = getUserData(&num_users, db_dir);
*/
  Users = getUserDataByName(user,&num_users, db_dir);
/* ACA/JJ
  Appusers = getAppuserData(&num_appusers, db_dir, WITHOUT_NTCSS_GROUP);
*/
  Appusers = getAppuserDataByName(user,&num_appusers, db_dir, WITHOUT_NTCSS_GROUP);
  Apps = getAppData(&num_apps, db_dir, WITHOUT_NTCSS_AND_WITH_ROLES);
  unsetReadDBLock(lock_id);

  user_id = -1;
  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, user) == 0)
        {
          user_id = i;
          break;
        }
    }
  if (user_id == -1)
    {
      freeUserData(Users, num_users);
      return(ERROR_NO_SUCH_USER);
    }


  leng = 0;
  for (i = 0; i < num_appusers; i++)
    {
      if ((strcmp(Appusers[i]->login_name, user) == 0) &&
          (strcmp(Appusers[i]->progroup_title, app) == 0))
        {
          s = getProgramAccess(Appusers[i]->app_role, app, Apps, num_apps);
          leng = bundleData(msg, "CCCICIIC",
                            Users[user_id]->real_name,
                            Users[user_id]->ss_number,
                            Users[user_id]->phone_number,
                            Users[user_id]->security_class,
                            s,
                            Appusers[i]->registered_user,
                            Users[user_id]->unix_id,
                            /* Next field used to be Appusers[i]->app_passwd */
                            Users[user_id]->shared_passwd);
          free((char *) s);
          break;
        }
    }

  freeUserData(Users, num_users);
  freeAppData(Apps, num_apps);
  freeAppuserData(Appusers, num_appusers);

  if (leng <= 0)
    return(ERROR_NO_SUCH_APP_USER);

  return(leng);   /* everthing ok, user data is returned */
}


/*****************************************************************************/

int  getAppUserData( const char  *app,
                     const char  *user,
                     char        *msg,
                     const char  *db_dir)
{
  UserItem     **Users;
  AppItem      **Apps;
  AppuserItem  **Appusers;
  int    i;
  int    leng;
  int    lock_id;
  int    num_apps;
  int    num_appusers;
  int    num_users;
  int    user_id;
  char  *s;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Users = getUserDataByName(user,&num_users, db_dir);
  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_AND_WITH_ROLES);
/* ACA/JJ
  Users = getUserData(&num_users, db_dir);
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
*/
  Appusers = getAppuserDataByName(user,&num_appusers, db_dir, WITH_NTCSS_GROUP);

  unsetReadDBLock(lock_id);

  user_id = -1;
  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, user) == 0)
        {
          user_id = i;
          break;
        }
    }
  if (user_id == -1)
    {
      freeUserData(Users, num_users);
      return(ERROR_NO_SUCH_USER);
    }

  leng = 0;
  for (i = 0; i < num_appusers; i++)
    {
      if ((strcmp(Appusers[i]->login_name, user) == 0) &&
          (strcmp(Appusers[i]->progroup_title, app) == 0))
        {
          s = getProgramAccess(Appusers[i]->app_role, app, Apps, num_apps);
          leng = getOverallLength("CCCIIICCIC",
                                  Users[user_id]->real_name,
                                  Users[user_id]->ss_number,
                                  Users[user_id]->phone_number,
                                  Users[user_id]->security_class,
                                  Users[user_id]->unix_id,
                                  Users[user_id]->user_role,
                                  Appusers[i]->app_data,
                                  Appusers[i]->app_passwd,
                                  Appusers[i]->registered_user,
                                  s);
          leng = bundleData(msg, "ICCCIIICCIC",
                            leng,
                            Users[user_id]->real_name,
                            Users[user_id]->ss_number,
                            Users[user_id]->phone_number,
                            Users[user_id]->security_class,
                            Users[user_id]->unix_id,
                            Users[user_id]->user_role,
                            Appusers[i]->app_data,
                            /* Next field used to be Appusers[i]->app_passwd */
                            Users[user_id]->shared_passwd,
                            Appusers[i]->registered_user,
                            s);
          free((char *) s);
          break;
        }
    }

  freeUserData(Users, num_users);
  freeAppData(Apps, num_apps);
  freeAppuserData(Appusers, num_appusers);

  if (leng <= 0)
    return(ERROR_NO_SUCH_APP_USER);

  return(leng);   /* everthing ok, user data is returned */
}


/*****************************************************************************/

int  registerAppUser( const char  *app,
                      const char  *user,
                      int          state,
                      const char  *db_dir )

{
  AppuserItem  **Appusers;
  int  i;
  int  num_appusers;
  int  lock_id;
  int  appuser_id;
  FILE  *fp;
  char  datafile[MAX_FILENAME_LEN];
  char  realfile[MAX_FILENAME_LEN];

  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  appuser_id = -1;
  for (i = 0; i < num_appusers; i++)
    {
      if ((strcmp(Appusers[i]->login_name, user) == 0) &&
          (strcmp(Appusers[i]->progroup_title, app) == 0))
        {
          Appusers[i]->marker = 0;
          appuser_id = i;
        }
      else
        Appusers[i]->marker = 1;
    }
  if (appuser_id == -1)
    {
      freeAppuserData(Appusers, num_appusers);
      unsetWriteDBLock(lock_id);
      return(ERROR_NO_SUCH_APP_USER);
    }

  sprintf(realfile, "%s/%s", db_dir, tables[APPUSERS_ID]);
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(datafile, "w");
  if (fp != NULL)
    {
      writeAppuserData(Appusers, num_appusers, fp);
      fprintf(fp, APPUSERS_FMT,
              Appusers[appuser_id]->progroup_title,
              Appusers[appuser_id]->login_name,
              Appusers[appuser_id]->real_name,
              Appusers[appuser_id]->app_data,
              Appusers[appuser_id]->app_passwd,
              Appusers[appuser_id]->app_role,
              state);
      closeAndEncrypt(fp, datafile);
    }
  else
    {
      freeAppuserData(Appusers, num_appusers);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  (void) rename(datafile, realfile);
  (void) chmod(realfile, S_IRUSR | S_IWUSR);

  freeAppuserData(Appusers, num_appusers);

  /*i = timestampDatabase(db_dir); No longer used */
  unsetWriteDBLock(lock_id);

  return(1);  /* everthing ok, appuser registered/deregistered */
}


/*****************************************************************************/

int  verifyUnixUserInfo( const char  *db_dir,
                         const char  *passwd_file )

{
  int  i;
  int  num_users;
  char datafile[MAX_FILENAME_LEN];
  char realfile[MAX_FILENAME_LEN];
  FILE  *fp;
  UserItem **Users;
  struct passwd  *temp;

  Users = getUserData(&num_users, db_dir);

  if (num_users < 0)
    return(ERROR_CANT_ACCESS_DB);

  fp = fopen(passwd_file, "r");
  if (fp == NULL)
    return(ERROR_CANT_OPEN_FILE);

  temp = (struct passwd *) fgetpwent(fp);

  while (temp != NULL)
    {
      if (strcmp(temp->pw_name, "+") != 0)
        {
          for (i = 0; i < num_users; i++)
            {
              if (Users[i]->marker &&
                  (strcmp(temp->pw_name, Users[i]->login_name) == 0))
                {
                  if (temp->pw_uid != Users[i]->unix_id)
                    Users[i]->unix_id = temp->pw_uid;
                  Users[i]->marker = 0;
                  break;
                }
            }
        }
      temp = (struct passwd *) fgetpwent(fp);
    }
  fclose(fp);

  for (i = 0; i < num_users; i++)
    {
      Users[i]->marker = 1;
      /* This check goes away since value is unsigned  VSD */
      /*      if (Users[i]->unix_id < 0)
	      Users[i]->unix_id = 0; */
    }

  sprintf(realfile, "%s/%s", db_dir, tables[NTCSS_USERS_ID]);
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[NTCSS_USERS_ID]);
  fp = fopen(datafile, "w");
  if (fp != NULL)
    {
      writeUserData(Users, num_users, fp);
      closeAndEncrypt(fp, datafile);
    }
  else
    {
      freeUserData(Users, num_users);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  (void) rename(datafile, realfile);
  (void) chmod(realfile, S_IRUSR | S_IWUSR);

  freeUserData(Users, num_users);

  return(1);          /* everthing ok, UNIX info verified */
}


/*****************************************************************************/

int  getCurrentUserList( const char  *query,
                         const char  *db_dir )
{
  const char  *fnct_name = "getCurrentUserList():";
  CurrentUsersItem  **currentUsersItems;
  FILE   *fp;
  int     i;
  int     nCurrentUsers;
  int     nLockId;

  fp = fopen(query, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name, query);
      return(ERROR_CANT_CREATE_QUERY);
    }

  nLockId = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s setReadDBLock failed!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(db_dir, &nCurrentUsers);
  unsetReadDBLock(nLockId);

  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s getCurrentUsersData failed!", fnct_name);
      fclose(fp);
      return(ERROR_CANT_ACCESS_DB);
    }

  for (i = 0; i < nCurrentUsers; i++)
    {
      /* 98-08-07 Check to make sure user is actually logged in and
         not just reserved login_time is "0" for reservation*/
      if (strcmp(currentUsersItems[i]->login_time, "0") != 0)
        fprintf(fp, "%s\n%s\n", currentUsersItems[i]->login_name, 
                currentUsersItems[i]->client_address);
    }

  fclose(fp);
  freeCurrentUsersData(currentUsersItems, nCurrentUsers);
  return(1);          /* everthing ok, current user list returned */
}


/*****************************************************************************/

/* This function is useless since the version 3.0.0 role changes. */

int  getUsersAccessRole( const char  *login_name,
                         const char  *db_dir )

{
  /* done only to get rid of compiler warning about unused parameter */
  login_name = login_name;
  db_dir = db_dir;

  return(0);  /* Just return here in case it's ever called */
}


/*****************************************************************************/
/*
int  getNextLoggedIn(login_name, client_ip, db_dir) 

     char  *login_name;
     char  *client_ip;
     char  *db_dir;

{
  int  lock_id; 
  int  flag = 0;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  DBRecord  record; 

  lock_id = setReadDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_UNLOCK_LOGIN);

  sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_USERS_ID]);
  fp = fopen(datafile, "r");
  if (fp == NULL)
    {
      unsetReadDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  
  while (getRecord(&record, fp, CURRENT_USERS_NUM_RECS))
    {
      strcpy(client_ip, record.field[2].data);
      strcpy(login_name, record.field[0].data);
      flag = 1;
    }
  fclose(fp);
  unsetReadDBLock(lock_id);
  
  return(flag);
} */


/*****************************************************************************/

int  getCurrentUsersRecord( const char        *strDbDir,
                            CurrentUsersItem  *pCU,
                            int               *pnFound,
                            const char        *strLoginName )

{
  int   i;
  int   nCurrentUsers;
  int   nLockId;
  const char  *fnct_name = "getCurrentUsersRecord():";
  CurrentUsersItem  **currentUsersItems;
  
  *pnFound = 0;
  
  nLockId = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get a db lock!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  currentUsersItems = getCurrentUsersData(strDbDir, &nCurrentUsers);
  unsetReadDBLock(nLockId);

  if (nCurrentUsers < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get user data!", fnct_name);
      return(ERROR_CANT_ACCESS_DB);
    }

  for (i = 0; i < nCurrentUsers; i++)
    {
      if (strcmp(currentUsersItems[i]->login_name, strLoginName) == 0)
        {
          *pnFound = 1;
          pCU->login_name =
            allocAndCopyString(currentUsersItems[i]->login_name);
          pCU->login_time =
            allocAndCopyString(currentUsersItems[i]->login_time);
          pCU->client_address =
            allocAndCopyString(currentUsersItems[i]->client_address);
          pCU->token  = allocAndCopyString(currentUsersItems[i]->token);
          pCU->uid    = currentUsersItems[i]->uid;
        }
    }     /* END for */

  return(0);
}



/*****************************************************************************/

void  freeCurrentUsersRecord( CurrentUsersItem  *pCU )

{
  free(pCU->login_name);
  free(pCU->login_time);
  free(pCU->client_address);
  free(pCU->token);
  free(pCU->reserve_time);
}


/*****************************************************************************/
/* Designed for use during message request from the User Administration
   program. (Called from GetUserAminUserInfoString) The data string is
   expected to be of the following size:
*//*
 [ (8 * 10)+  <- 8 fields by 10 chars for bundle_data's lengths info
   10 +                <- alows for a 32-bit UID_MAX for this system
   MAX_LOGIN_NAME_LEN + MAX_REAL_NAME_LEN + MAX_PASSWORD_LEN + 
   11 +                <- for SSN number
   MAX_PHONE_NUMBER_LEN +
   10 +                <- 10 place # (32-bits) for security class
   DATETIME_LEN +      <- password change time (int)
   200                 <- extra buffering in case we're missing something here
 */

int  MakeUserAdminDataString( const char  *db_dir,
                              char        *login_name,
                              char        *data_string,
                              int         *bundle_len )
{
  const char  *fnct_name = "MakeUserAdminDataString():";
  UserItem  **Users;
  char temp_buf[100];  /* mainly for the security string */
  int  num_users;
  int  lock_id;
  int  i;
  int  x;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not obtain a db lock!", fnct_name);
      return(-1);
    }

  Users = getUserDataByName(login_name,&num_users, db_dir);
  unsetReadDBLock(lock_id);
  if (num_users < 0)
    {
      syslog(LOG_WARNING, "%s Failed to obtain the Users list!", fnct_name);
      return(-2);
    }
   i=0;

  /* Get rid of any spaces in the login_name string */
  (void) trim(login_name);

/*
  for (i = 0; i < num_users; i++)
    {
      if (strcmp(Users[i]->login_name, login_name) == 0)
        break;
    }
*/

  if (i >= num_users)  /* user wasn't found */
    {
      syslog(LOG_WARNING, "%s No such user! <%s>", fnct_name, login_name);
      freeUserData(Users, num_users);
      return (1);
    }

  /* Convert this int to a string */
  x = MakeUserClearanceString(temp_buf, Users[i]->security_class);

  *bundle_len = bundleData(data_string, "ICCCCCCCC",
                           Users[i]->unix_id,
                           Users[i]->login_name,
                           Users[i]->real_name,
                           Users[i]->password,
                           Users[i]->ss_number,
                           Users[i]->phone_number,
                           temp_buf,                     /* sec class */
                           Users[i]->pw_change_time,
                           Users[i]->auth_server); /* testing value only */
  freeUserData(Users, num_users);
  
  if (*bundle_len <= 0)
    {
      syslog(LOG_WARNING, "%s Failed to bundle data!", fnct_name);
      return(-3);
    }

  return(0);
}


/*****************************************************************************/
/* Creates a comma delimited string of all the role names to which a user
   having the provided access_bitmask has access to.  Since this routine is
   called frequently in succession from CreateUserAdminINIfile(), AccessRoles
   is passed in to speed up the operations here and let the caller decide
   whether to use the db locks or not..
*/

int  MakeUserAccessRoleString( AccessRoleItem  **AccessRoles,
                               int               num_roles,
                               const char       *progroup,
                               int               access_bitmask,
                               char             *role_string )
{
  int  i;
  int  temp_mask;

  role_string[0] = '\0';

  /* Check if this bitmask belongs to a batch user */
  if (access_bitmask & NTCSS_BATCH_USER_BIT)
      strcat(role_string, "Batch User");


  for (i = 0; i < num_roles; i++) /* search for progroup's access roles */
    {
      if (strcmp(AccessRoles[i]->progroup_title, progroup) ==0)
          {
            /* 2 bit offset -> FUTURE CHANGE ?? */
            temp_mask = (1 << (AccessRoles[i]->role_number +
                              ROLE_BIT_BASE_OFFSET) );
            if (temp_mask & access_bitmask)    /* user has this role.. */
              {
                if (role_string[0] != '\0')  /* already a role listed.. */
                  strcat(role_string, ",");
                strcat(role_string, AccessRoles[i]->app_role);
              }
          }
    }
  if (role_string[0] == '\0')          /* check for empty role_string */
    strcat(role_string, "No Access Allowed");

  return(0);
}


/*****************************************************************************/
/* Given the Ntcss clearance number, this function writes the appropriate
   name to the provided buffer.
*/

int  MakeUserClearanceString( char  *buffer,
                              int    clearance )

{
  const char  *fnct_name = "MakeUserClearanceString():";

  switch(clearance)
    {
    case UNCLASSIFIED_CLEARANCE:
      strcpy(buffer, "UNCLASSIFIED");
      return(0);

    case UNCLASSIFIED_SENSITIVE_CLEARANCE:
      strcpy(buffer, "UNCLASSIFIED SENSITIVE");
      return(0);

    case CONFIDENTIAL_CLEARANCE:
      strcpy(buffer, "CONFIDENTIAL");
      return(0);

    case SECRET_CLEARANCE:
      strcpy(buffer, "SECRET");
      return(0);

    case TOPSECRET_CLEARANCE:
      strcpy(buffer, "TOP SECRET");
      return(0);

    default:
      strcpy(buffer, "UNKNOWN");
      syslog(LOG_WARNING, "%s Unknown security access level (%d) encountered!",
             fnct_name, clearance);
    }
  return(-1);
}


/*****************************************************************************/
/* Given a comma delimited string of the role names, this function looks up the
   associated role numbers and creates a bitmask with those numbers.  The
   bitmask produced is sutable for OR'ing.
   If any roles are not found, the quantity of them is the returned value.
*/

int  MakeAppRoleMask( AccessRoleItem  **AccessRoles,
                      int               num_roles,
                      char             *role_string,
                      const char       *progroup_title,
                      int              *mask )
{
  int    i;
  int    num_roles_not_found = 0;   /* records non-existant roles requsted */
  char  *current_role;

  if ((num_roles < 0) || (AccessRoles == NULL) || (role_string == NULL))
    return(-1);

  current_role = strtok(role_string, ",");

  *mask = 0;                                     /* initialize the value */
  while(current_role != NULL)
    {
      for (i = 0; i < num_roles; i++)
        {
          if (strcmp(AccessRoles[i]->progroup_title, progroup_title) == 0)
            if (strcmp(AccessRoles[i]->app_role, current_role) == 0)
              {
                *mask |= (1 << (AccessRoles[i]->role_number +
                                ROLE_BIT_BASE_OFFSET));
                break;
              }
        }
      if (i >= num_roles)
        num_roles_not_found++;

      /* search for the next role in the string.. */
      current_role = strtok(NULL, ",");
    }
  return(num_roles_not_found);
}


/*****************************************************************************/
/* Creates an integer mask for the specified login name from the appusers
   table. This function expects the caller to lock the databases if necessary
   and get the AppUsers data structure.
*/

int  getAppUserRoleMask( AppuserItem  **AppUsers,
                         const char    *progroup_title,
                         int            num_appusers,
                         const char    *login_name,
                         unsigned int  *mask,
                         int           *index )
{
  int   i;

  *mask = 0;  /* initialize this value */
  for(i = 0; i < num_appusers; i++)
    {
      if (strcmp(AppUsers[i]->login_name, login_name) == 0)
        if (strcmp(AppUsers[i]->progroup_title, progroup_title) == 0)
          {
            *mask = AppUsers[i]->app_role;
            break;
          }
    }

  *index = i;
  if (i == num_appusers)
    return(1);             /* App User's name was not found! */
  else
    return(0);
}


/*****************************************************************************/

int  getAppPassword( char        *strlogin_name,
                     const char  *strpassword,
                     const char  *strgroup,
                     char        *strapppwd,
                     const char  *strdb_dir )

{
  int    lock_id;
  int    nRetVal;
  char   strSSN[MAX_TOKEN_LEN + 1];


  if (!strlogin_name || (strlogin_name != NULL && !strlen(strlogin_name)))
    return(ERROR_USER_NOT_VALID);

  if (!strpassword || (strpassword != NULL && !strlen(strpassword)))
    return(ERROR_INCORRECT_PASSWORD);

  if (!strgroup || (strgroup != NULL && !strlen(strgroup)))
    return(ERROR_NO_PROGRAM_INFO_FOUND);

  if (strapppwd == NULL)
    return(ERROR_NO_PROGRAM_INFO_FOUND);

  nRetVal = -1;

  /* validate the user first */
  nRetVal=checkNtcssUserPassword(strlogin_name, strpassword, strSSN, strdb_dir,
                             0 /* 0 - Auto update password encryption */);
  if (nRetVal > 0)
    {
      lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
      if (lock_id < 0)
        return(ERROR_CANT_ACCESS_DB);
      
	nRetVal=getUserSharedPassword(strlogin_name, strapppwd);
        unsetReadDBLock(lock_id);
    }

/* ACA/JJ
      AppUsers = getAppuserData(&num_appusers, strdb_dir, WITH_NTCSS_GROUP);
      AppUsers = getAppuserDataByName(strlogin_name,
		&num_appusers, strdb_dir, WITH_NTCSS_GROUP);
      unsetReadDBLock(lock_id);
        
      for (i = 0; i < num_appusers; i++)
        {
          if (!strcmp(strlogin_name, AppUsers[i]->login_name) &&
              !strcmp(strgroup, AppUsers[i]->progroup_title))
            {
              strcpy(strapppwd, AppUsers[i]->app_passwd);
              nRetVal = 1;
              break;
            }
        }
      
      freeAppuserData(AppUsers, num_appusers);
    }
*/
  return(nRetVal);
}


/*****************************************************************************/

int  isUserPartOfProgroup( const char  *strUser,
                           const char  *strProgroup )

{
  int   nAppusers;
  int   nFound;
  int   i;
  AppuserItem  **appuserItems;
  
/* ACA/JJ
  appuserItems = getAppuserData(&nAppusers, NTCSS_DB_DIR, WITH_NTCSS_GROUP);
*/
  appuserItems = getAppuserDataByName(strUser,&nAppusers, 
		NTCSS_DB_DIR, WITH_NTCSS_GROUP);
  if (nAppusers < 0)
    {
      return(-1);
    }

  nFound = 0;
  for (i = 0; i < nAppusers; i++)
    {
      if ( (strcmp(appuserItems[i]->login_name,     strUser)     == 0) &&
           (strcmp(appuserItems[i]->progroup_title, strProgroup) == 0) )
        {
          nFound = 1;
          break;
        }
    }
  
  freeAppuserData(appuserItems, nAppusers);
  return(nFound);
}


/*****************************************************************************/
/* Called in db_svr for GETAPPDATA message (client) to get the users APP data.
*/

int  getappdata( const char  *app,
                 const char  *user,
                 char        *appdata,
                 const char  *db_dir )

{
  AppuserItem  **Appusers;
 
  int  i;
  int  lock_id;
  int  num_appusers;
  int ret = ERROR_NO_SUCH_APP_USER;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);
 
/* ACA/JJ
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
*/
  Appusers = getAppuserDataByName(user,&num_appusers, db_dir, WITH_NTCSS_GROUP);

  unsetReadDBLock(lock_id);

 
  for (i = 0; i < num_appusers; i++)  
      if ((strcmp(Appusers[i]->login_name, user) == 0) &&
          (strcmp(Appusers[i]->progroup_title, app) == 0))
        {
      strcpy(appdata, Appusers[i]->app_data);
      ret = 1;
          break;
        }
 
  freeAppuserData(Appusers, num_appusers);
  return(ret);   
}



int GetUserInfoAndRoles(const char* query,const char* user)
{
  const char *fnct_name = "GetUserInfoAndRoles():";
  AppuserItem  **Appusers;
  UserItem *UserInfo;
  AccessRoleItem **Roles;
  /*char lpszString[4096]; JGJ*/
  char* pString; /*JGJ*/
  int nLength;
  char lpszData[512],lpszMessage[512];
  char strAppRoles[512];
  int lock_id,i,strleng,msgleng;
  int  num_appusers,one_app;
  int num_roles;
  FILE *fp;

  fp=fopen(query,"w");
  if (fp == NULL) {
	syslog(LOG_WARNING,"%s cannot open query file <%s>.",fnct_name,query);
  	return(ERROR_NO_SUCH_FILE);
  }

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0) {
    	syslog(LOG_WARNING,"%s unable to get db lock",fnct_name);
	fclose(fp);
	return(ERROR_CANT_ACCESS_DB);
  }


  if (getUserDataByLoginName(&UserInfo,user,NTCSS_DB_DIR) < 0) {
	syslog(LOG_WARNING,"%s unable to find user %s",fnct_name,user);
        if (lock_id > 0)
  		unsetReadDBLock(lock_id);
	fclose(fp);
  	return(ERROR_NO_SUCH_USER);
  }

  pString=(char*) malloc(200); /*JGJ swag @ base user size*/

  /*JGJ*/
  /*  Appusers = getAppuserData(&num_appusers, NTCSS_DB_DIR, WITH_NTCSS_GROUP);*/
 
  Appusers = getAppuserDataByName(user,&num_appusers,NTCSS_DB_DIR, WITH_NTCSS_GROUP);
  nLength=(num_appusers*MAX_NUM_APP_ROLES_PER_PROGROUP*MAX_ACCESS_ROLE_LEN)+
           MAX_NUM_APP_ROLES_PER_PROGROUP; /*JGJ*/

  pString = (char*) realloc(pString,nLength+200); 

  /* syslog(LOG_WARNING,"%s past realloc",fnct_name);*/

      if (!pString)
	{
	  syslog(LOG_WARNING,"%s Realloc Failed",fnct_name);
	  return -1;
	}
    

  Roles=getAccessRoleData(&num_roles,NTCSS_DB_DIR);

  if (lock_id > 0) 
  	unsetReadDBLock(lock_id);


  memset(pString,'\0',nLength); /*JGJ*/

  sprintf(pString,"%s\001%s\001%s\001%s\001%s\001%d\001%d\001%s",
		UserInfo->login_name,UserInfo->real_name,
		UserInfo->ss_number,UserInfo->phone_number,
		UserInfo->auth_server,UserInfo->unix_id,
		UserInfo->security_class,UserInfo->password);

  one_app=0;
  strcat(pString,"\002");

  for (i=0;i<num_appusers;i++) {
	if (!strcmp(user,Appusers[i]->login_name)) {
  		memset(strAppRoles,'\0',512);
		MakeUserAccessRoleString( Roles,num_roles,
				Appusers[i]->progroup_title,
				Appusers[i]->app_role,
                                strAppRoles);
		if (strlen(strAppRoles)) {
			if (one_app == 0) 
				one_app = 1;
			else
			  strcat(pString,"\002"); /*jgj*/
			strcat(pString,Appusers[i]->progroup_title);
			strcat(pString,",");
			strcat(pString,strAppRoles);
		}
	}
  }
  /* strcat(lpszString,"\n");*/

  strleng=strlen(pString);

  freeAppuserData(Appusers,num_appusers);
  freeAccessRoleData(Roles,num_roles);
  freeUserData(&UserInfo,1);

  /*bundle the total length adv alloc */
  memset(lpszData,'\0',sizeof(lpszData));
  memset(lpszMessage,'\0',sizeof(lpszMessage));
 
  msgleng=bundleData(lpszData,"I",strleng)+strleng;

  msgleng+=bundleData(lpszMessage,"I",msgleng);

  fprintf(fp,"%s",lpszMessage);
  fprintf(fp,"%s",lpszData);
  fprintf(fp,"%s",pString);
  fclose(fp);

  free((char*)pString); /*JGJ*/

  return msgleng;

}
int CheckSSN(const char* ssn)
{
  const char *fnct_name = "CheckSSN():";
  UserItem  **Users;
  int lock_id,i,status;
  int  num_users;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0) {
    syslog(LOG_WARNING,"%s unable to set lock.",fnct_name);
    return(ERROR_CANT_ACCESS_DB);
  }

  Users = getUserData(&num_users, NTCSS_DB_DIR);

  unsetReadDBLock(lock_id);

  status=0;
  for (i=0;i<num_users;i++) {
	if (!strcmp(ssn,Users[i]->ss_number)) {
		status=1;
		break; 
	}
  }
  freeUserData(Users,num_users);
  return status;
}

struct _UserSortList {
	char uname[MAX_LOGIN_NAME_LEN+1];
	int index;
};


int GetUserList(char* query)
{
  const char *fnct_name = "GetUserList():";
  const char *uctmp="/h/NTCSSS/logs/UCLOGS.tmp";
  UserItem  **Users;
  AppuserItem **Appusers;
  HostItem **Hosts;
  int lock_id,i,leng,nLockId;
  int  num_users,num_appusers,num_hosts;
  int nTmpLoads,one_host;
  char strLoadVal[4096],strLoadTmp[156];
  char lpszLine[156],strNumUsers[156],lpszSizeForNew[156];
  char   ini_buffer[MAX_INI_ENTRY * 5]; /* for profile section additions */
  char   ini_new_buffer[MAX_INI_ENTRY * 5];
  char   ini_out_buffer[MAX_INI_ENTRY * 5];
  char strBatchUsers[MAX_NUM_PROGROUPS*MAX_LOGIN_NAME_LEN+MAX_NUM_PROGROUPS+1];
  FILE *fp;
  struct _UserSortList *SortedUserList;
  int sort_flag,sort_i;


  nLockId=0;
  sort_flag=0;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0) {
    syslog(LOG_WARNING,"%s Unable to get lock",fnct_name);
    return(ERROR_CANT_ACCESS_DB);
  }

  Users = getUserData(&num_users, NTCSS_DB_DIR);
  Appusers = getAppuserData(&num_appusers, NTCSS_DB_DIR, WITH_NTCSS_GROUP);
  Hosts=getHostData(&num_hosts,NTCSS_DB_DIR);

  unsetReadDBLock(lock_id);

  nLockId=0;
  nLockId = setWriteDBLock(USER_ADMIN_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0)
    {
      syslog(LOG_WARNING, "%s Could not get a DBWrite Lock",fnct_name);
      return ERROR_CANT_SET_ADMIN_LOCK;
    }

  SortedUserList = (struct _UserSortList *)
                              malloc(sizeof(struct _UserSortList) * num_users);
  if (SortedUserList == NULL) {
      syslog(LOG_WARNING, "%s Could not get Sorted Info",fnct_name);
  } else
     sort_flag=1; 

  fp = fopen(query,"w");
  if (fp == NULL) {
  	if (nLockId > 0)
		unsetWriteDBLock(nLockId);
	syslog(LOG_WARNING,"%s unable to open file <%s>",fnct_name,query);
	return(ERROR_NO_SUCH_FILE);
  }


  memset(strBatchUsers, '\0', MAX_NUM_PROGROUPS*MAX_LOGIN_NAME_LEN
	                       + MAX_NUM_PROGROUPS + 1);

  leng=0;
  
  /* collect host stuff, and save it for the future */
  for (i=0;i<num_hosts;i++) {
        if (Hosts[i]->type == NTCSS_AUTH_SERVER || 
	    Hosts[i]->type == NTCSS_MASTER) {
		sprintf(strLoadVal,"%d",0);
		WriteProfileString("LOADS",Hosts[i]->hostname,strLoadVal,
				   uctmp);
	}
  }

  /* find and save the batch (non-editable) users */
  strcpy(strBatchUsers,BASE_NTCSS_USER);
  for (i=0;i<num_appusers;i++) {
  	if (Appusers[i]->app_role & NTCSS_BATCH_USER_BIT) {
		if (strlen(strBatchUsers))
			strcat(strBatchUsers,"%");
		strcat(strBatchUsers,Appusers[i]->login_name);
	}
  }
  leng += strlen(strBatchUsers) +1;  /* add one for cr */

  /*get sysconf stuff has to be there  */
  memset(ini_buffer, '\0', MAX_INI_ENTRY + 1);
  memset(ini_new_buffer, '\0', MAX_INI_ENTRY + 1);
  memset(ini_out_buffer, '\0', MAX_INI_ENTRY + 1);
  getNtcssSysConfData("NTUSERDIR", ini_buffer, NTCSS_DB_DIR);
  replacePattern("^", "\\", ini_buffer, ini_new_buffer);
  strcat(ini_out_buffer,ini_new_buffer);
  strcat(ini_out_buffer,"%");

  memset(ini_buffer, '\0', MAX_INI_ENTRY + 1);
  getNtcssSysConfData("NTTMPUSER", ini_buffer, NTCSS_DB_DIR);
  strcat(ini_out_buffer,ini_buffer);
  strcat(ini_out_buffer,"%");

  memset(ini_buffer, '\0', MAX_INI_ENTRY + 1);
  memset(ini_new_buffer, '\0', MAX_INI_ENTRY + 1);
  getNtcssSysConfData("NTPROFDIR", ini_buffer, NTCSS_DB_DIR);
  replacePattern("^", "\\", ini_buffer, ini_new_buffer);
  strcat(ini_out_buffer,ini_new_buffer);

  leng += strlen(ini_out_buffer);

  /* calculate */
  for (i=0;i<num_users;i++) {
	leng += strlen(Users[i]->login_name);
	leng += strlen(Users[i]->real_name);
	leng += strlen("Idle");
	leng += strlen(Users[i]->ss_number);
	leng += strlen(Users[i]->phone_number);
	leng += strlen(Users[i]->auth_server);
        /* add delimiters to count as well as the cr */
	leng += 6;

        /* might as well do something useful here */
	nTmpLoads=GetProfileInt("LOADS",Users[i]->auth_server,-1,uctmp);
        if (nTmpLoads >= 0) {
		sprintf(strLoadVal,"%d",++nTmpLoads);
		WriteProfileString("LOADS",Users[i]->auth_server,strLoadVal,
					uctmp);
	}
  }

  memset(strLoadVal,'\0',4096);
  one_host=0;
  /* calc the host load stuff */
  for (i=0;i<num_hosts;i++) {
        if (Hosts[i]->type == NTCSS_AUTH_SERVER || 
	    Hosts[i]->type == NTCSS_MASTER) {
		if (one_host == 0)
			one_host=1;
		else
			strcat(strLoadVal,"%");

		sprintf(strLoadTmp,"%s=%d",Hosts[i]->hostname,
			GetProfileInt("LOADS",Hosts[i]->hostname,0,uctmp));
		strcat(strLoadVal,strLoadTmp);
	}
  }
  leng += strlen(strLoadVal) + 1; /* add one for the cr */ 

  memset(strNumUsers,'\0',156);
  memset(lpszSizeForNew,'\0',156);
  memset(lpszLine,'\0',156);

  leng+= bundleData(strNumUsers,"I",num_users);
  leng+= bundleData(lpszSizeForNew,"I",leng);
  leng+= bundleData(lpszLine,"I",leng);

  fprintf(fp,"%s",lpszLine);
  fprintf(fp,"%s",lpszSizeForNew);
  fprintf(fp,"%s",strNumUsers);

  if (sort_flag) {
  	for (i=0;i<num_users;i++) {
		strcpy(SortedUserList[i].uname,Users[i]->login_name);
		SortedUserList[i].index=i;
  	}
	qsort(SortedUserList, num_users, sizeof(struct _UserSortList),
        	usort_compare);
  	for (i=0;i<num_users;i++) {
		sort_i=SortedUserList[i].index;
		fprintf(fp,"%s\001%s\001%s\001%s\001%s\001%s\n",
			Users[sort_i]->login_name,
			Users[sort_i]->real_name,"Idle",
			Users[sort_i]->ss_number,Users[sort_i]->phone_number,
			Users[sort_i]->auth_server);
  	}

  } else {
  	for (i=0;i<num_users;i++) {
		fprintf(fp,"%s\001%s\001%s\001%s\001%s\001%s\n",
			Users[i]->login_name,Users[i]->real_name,"Idle",
			Users[i]->ss_number,Users[i]->phone_number,
			Users[i]->auth_server);
  	}
 }
	
  fprintf(fp,"%s\n",ini_out_buffer);
  fprintf(fp,"%s\n",strBatchUsers);
  fprintf(fp,"%s\n",strLoadVal);

  fclose(fp);

  if (nLockId > 0)
	unsetWriteDBLock(nLockId);

  freeUserData(Users,num_users);
  freeAppuserData(Appusers,num_appusers);
  freeHostData(Hosts,num_hosts);
  if (sort_flag)
  	free(SortedUserList);

  return leng;
}


/*****************************************************************************/

int  usort_compare( const void  *name1,
                    const void  *name2 )

{
  /*
    printf("Comparing %s and %s\n",name1,name2);
  */
  const struct _UserSortList  *s1, *s2;
  const char                  *p1, *p2;

  s1 = (const struct _UserSortList *)name1;
  s2 = (const struct _UserSortList *)name2;

  p1 = s1->uname;
  p2 = s2->uname;

  return(strcmp(p1, p2));
}


/*****************************************************************************/
