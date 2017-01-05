
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * ManipData.c
 */


/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

#if defined SOLARIS || defined linux
#  include <dirent.h>
#include <crypt.h> 
#else
#  include <sys/dir.h>
#endif

/* Added definition for linux only. VSD */
#ifdef linux
#define _XOPEN_SOURCE
#endif



/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <NtcssCrypt.h>
#include <LibNtcssFcts.h>
#include <ExtraProtos.h>
#include <ini_Fcts.h>


/** LOCAL INCLUDES **/

#include "TextDBP.h"

#define LIST_SEP_CHARACTER   " " 


/** STATIC FUNCTION PROTOTYPES **/
static int   apply_sql_change(DBRecord*, const char*, const char*);
static int   appropriate_boot_job(char**, int, const char*, int);
static int   changeOutputPrtData(int, const char*, const char*, const char*,
                                 const char*, const char*);
static int   changePrinterAccessData(int, const char*, const char*,
                                     const char*, const char*);
static int   ensure_msgbb_integrity(const char*, const char*);
static void  get_progroup_titles_for_host_name(const char*, const char*,
                                               char**, int*);
static int   getSqlChange(DBRecord*, FILE*);
static int   mod_msgbb_user(int, const char*, const char*, int);          
static int   mod_msgbb_appuser(int, const char*, const char*, const char*,
                               int);       
static void  writeAdapterData(AdapterItem**, int, FILE*);
static void  writeOutputTypeData(OutputTypeItem**, int, FILE*);
static void  writePredefinedJobData(PredefinedJobItem**, int, FILE*);
static void  writeProgramData(ProgramItem**, int, FILE*);
static void  writePrtclassData(PrtclassItem**, int, FILE*);


/** STATIC VARIABLES **/

static int  using_tmp = 0;


/** GLOBAL VARIABLES **/
#if defined SOLARIS || defined HPUX
extern char *sys_errlist[];
#endif

/* Each entitity's index is defined in Ntcss.h */

const char *tables[] = { "ntcss_users", "progroups", "programs", "appusers",
                         "printers", "adapters", "prt_class",
                         "printer_access", "output_type", "output_prt",
                         "hosts", "current_users", "login_history",
                         "access_roles", "current_apps", "predefined_jobs" };

/** FUNCTION DEFINITIONS **/

/*****************************************************************************/

CurrentUsersItem  **getCurrentUsersData( const char *strDbDir,
                                         int        *pnCurrentUsers )
{
  int  nRecords, nIndex;
  char  strDatafile[MAX_FILENAME_LEN];
  FILE  *fp;
  DBRecord  record;
  CurrentUsersItem **currentUsersItems;
  char decryptfile[MAX_FILENAME_LEN];
  
  *pnCurrentUsers = -1;

  /*
  nLockId = setReadDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (nLockId < 0) {
    syslog(LOG_WARNING, "getCurrentUsersData: setReadDBLock failed");
    return((CurrentUsersItem **) 0);
  } */

  sprintf(strDatafile, "%s/%s", strDbDir, tables[CURRENT_USERS_ID]);
  fp = decryptAndOpen(strDatafile, decryptfile, "r");
  if (fp == NULL) {
    /* unsetReadDBLock(nLockId); */
    syslog(LOG_WARNING, "getCurrentUsersData: fopen(%s) failed" ,strDatafile); 
    if (decryptfile[0] != '\0')
      remove(decryptfile);
    return((CurrentUsersItem **) 0);
  }
  
  /* Calculate how many entries in the current users table */
  nRecords = 0;
  while (getRecord(&record, fp, CURRENT_USERS_NUM_RECS)) {
    nRecords++;
  }


  /* Now malloc the array to hold these entries and populate
     the array from the current users table
   */
  /* Rewind file pointer to the beginning of file */
  if (fseek(fp, 0L, SEEK_SET) != 0) {
    syslog(LOG_WARNING, "getCurrentUsersData: fseek(fp, 0L, SEEK_SET) failed");
    return((CurrentUsersItem **) 0);
  }

  *pnCurrentUsers = nRecords;
  currentUsersItems = (CurrentUsersItem **) malloc(nRecords * sizeof(char *));
 
  nIndex = 0;
  while (getRecord(&record, fp, CURRENT_USERS_NUM_RECS)) {
    currentUsersItems[nIndex] = malloc(sizeof(CurrentUsersItem));
    currentUsersItems[nIndex]->login_name =
      allocAndCopyString(record.field[0].data);
    currentUsersItems[nIndex]->login_time =
      allocAndCopyString(record.field[1].data);
    currentUsersItems[nIndex]->client_address =
      allocAndCopyString(record.field[2].data);
    currentUsersItems[nIndex]->token =
      allocAndCopyString(record.field[3].data);
    currentUsersItems[nIndex]->uid = atoi(record.field[4].data);
    currentUsersItems[nIndex]->reserve_time =
      allocAndCopyString(record.field[5].data);
    nIndex++;
  } /* END while */

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  /* unsetReadDBLock(nLockId); */
  
  return(currentUsersItems);

}


/*****************************************************************************/

void  writeCurrentUsersData( CurrentUsersItem **CurrentUsers,
                             int                num_users,
                             FILE              *fp )
{
  int  i;

  for (i = 0; i < num_users; i++)
    if (CurrentUsers[i]->marker == 1)
      fprintf(fp, CURRENT_USERS_FMT,
              CurrentUsers[i]->login_name,
              CurrentUsers[i]->login_time,
              CurrentUsers[i]->client_address,
              CurrentUsers[i]->token,
              CurrentUsers[i]->uid,
              CurrentUsers[i]->reserve_time);
}


/*****************************************************************************/

void  freeCurrentUsersData( CurrentUsersItem **pCurrentUsersItems,
                            int                nCurrentUsers )
{
  int i;

  for (i = 0; i < nCurrentUsers; i++) {
    freeCurrentUsersRecord(pCurrentUsersItems[i]);
    /*
      free(pCurrentUsersItems[i]->login_name);
      free(pCurrentUsersItems[i]->login_time);
      free(pCurrentUsersItems[i]->client_address);
      free(pCurrentUsersItems[i]->token);
      free(pCurrentUsersItems[i]->reserve_time);
    */
    free((char*) pCurrentUsersItems[i]);
  } /* END for */
  
  free((char*) pCurrentUsersItems);
}


/****************************************************************************
   Encryption and decryption of database files can be turned off. When it is
   turned off, the database files are not decrypted before they are opened and 
   they are not encrypted after they have been closed. "encryptDecryptTurnedOn"
   indicates whether the encryption/decryption functionality is turned on.

   Return:
   1 is returned if encryption/decryption is turned on. Otherwise, 0 is
   returned.
*/

int  encryptDecryptTurnedOn( void )
{
  /* check the current encrytion mode and return the proper value */

  if (strcmp(NTCSS_ENCRYPTION_MODE, NTCSS_ENCRYPTION_ON_STRING) == 0)
    return 1;
  else if (strcmp(NTCSS_ENCRYPTION_MODE, NTCSS_ENCRYPTION_OFF_STRING) == 0)
    return 0;
  else { /* NTCSS_ENCRYPTION_MODE == NTCSS_ENCRYPTION_UNKNOWN_STRING */ 
    syslog(LOG_WARNING, "encryptDecryptTurnedOn: Encryption state unknown");
    return 0;
  } 
}


/*****************************************************************************
   Takes an encrypted file and decrypts the contents to a seperate file. The
   decrypted file is then opened with mode  access specified. Note: the key
   used for decryption must be the same key that is used for encryption.

   Return:
   The function returns a NULL if an error occured while decrypting or there
   was an error opening the decrypted file. Otherwise the file pointer 
   returned from the fopen call of the decrypted file is returned.

   Parameter(s);
   "pathname" is the name of the file to be decrypted.  "decryptFile" is a
   buffer into which the name of the decrypted file is stored. "mode" is the
   access the file will be opened with.

   Note: Encryption and decryption can be turned off. The function 
   "encryptDecryptTurnedOn" returns whether this function is activated or not.
   If this functionality is not turned on, then "decryptAndOpen" simply does
   an fopen on the file passed in.
*/

FILE*  decryptAndOpen( const char  *pathname,
                       char        *decryptFile,
                       const char  *mode )
{
  if (!encryptDecryptTurnedOn())
    {
      decryptFile[0] = '\0';      /* Initialize this in case caller uses it. */
      return fopen(pathname, mode);
    }

  /* create the name of the file where the decryption result will be placed */
  strcpy(decryptFile, pathname);
  strcat(decryptFile, ".dcp");

  /* attempt to decrypt the file to the file "decryptFile" */
  if (ntcssDecryptFileToFile(pathname, decryptFile, 
                             ENCRYPT_DECRYPT_KEY) == 0)
    return fopen(decryptFile, mode);       /* try to open the decrypted file */
  else
    return NULL;         /* an error occured while decrypting the given file */
}

/*****************************************************************************
   Takes a file pointer and closes the file and then encrypts the file. Note:
   the encryption must use the same key as the decryption function.

   Return:
   The function returns the result of the file encryptor. This result indicates
   wheter the encryption was successful or not.

   Parameter(s):
   "filePointer" is the pointer to the FILE structure to close.  "pathname" is
   the name of the file pointed to by the file pointer.

   Note: Encryption and decryption can be turned off. The function 
   "encryptDecryptTurnedOn" returns whether this function is activated or not.
   If this functionality is not turned on, then "closeAndEncrypt" simply does
   an fclose on the file pointer passed in.
*/

int  closeAndEncrypt( FILE        *filePointer,
                      const char  *pathname )
{
   int  ret;
 
   /* close the file */
   ret = fclose(filePointer);

   if (encryptDecryptTurnedOn())
     /* encrypt the file and return the resulting code */
     return ntcssEncryptFileToFile(pathname, NULL, ENCRYPT_DECRYPT_KEY);
   else
     return ret;
}


/*****************************************************************************/

int  isBitSet( int  value,
               int  mask )
{
    return(value & mask);
}


/*****************************************************************************/

void  doSetBit( int  *value,
                int   mask )
{
  if (!(*value & mask))
    *value |= mask;
}


/*****************************************************************************/

void  doClearBit( int *value,
                  int  mask )
{
  if (*value & mask)
    *value ^= mask;
}


/*****************************************************************************/

int  getRecord( DBRecord *record,
                FILE     *fp,
                int       num_cols )
{
  int  i;
  char  item[MAX_COL_LEN];   /* junk storage */

  for (i = 0; i < num_cols; i++)
    {
      if (fgets(record->field[i].data, MAX_COL_LEN, fp) == NULL)
        return(0);
      stripNewline(record->field[i].data);
    }
  (void) fgets(item, MAX_COL_LEN, fp);  /* skip the record separator line */

  return(1);
}


/*****************************************************************************/

void  convToSql( DBRecord *record,
                 int       num_cols )
{
  int  i;

  for (i = 0; i < num_cols; i++)
    if (isBlank(record->field[i].data))
      strcpy(record->field[i].data, "***");
}


/*****************************************************************************/

void  convFromSql( DBRecord *record,
                   int       num_cols )
{
  int  i;

  for (i = 0; i < num_cols; i++)
    if (strcmp(record->field[i].data, "***") == 0)
      strcpy(record->field[i].data, "");
}


/*****************************************************************************/

int  timestampDatabase( const char  *db_dir )

{
  int  i;
  char  date[MAX_ITEM_LEN];

  i = getTimeStr(date);
  if (i < 0)
    {
      syslog(LOG_WARNING, "timestampDatabase: Error getting system time!");
      return(-1);
    }

  i = putNtcssSysConfData("NTCSS DB TIMESTAMP", date, db_dir);
  if (i != 0)
    {
      syslog(LOG_WARNING, "timestampDatabase: Error (%i) adding db timestamp"
             " to SYS_CONF!", i);
      return(-2);
    }
  return(0);
}


/*****************************************************************************/

int  getNtcssSysConfData( const char  *tag,
                          char        *data,
                          const char  *db_dir )
{
  const char  *fnct_name = "getNtcssSysConfData():";
  char  datafile[MAX_FILENAME_LEN];
  char  item[MAX_ITEM_LEN], *strSysConfData;
  int  lock_id;
  int  use_lock = 1;
  int  flag;
  FILE  *fp;
  char decryptfile[MAX_FILENAME_LEN];

  if (tag != NULL) {
     if (getNtcssSysConfValueFromNis(&strSysConfData, tag) == 0)
       {
         strncpy(data, strSysConfData,
                 strlen(strSysConfData) + 1 /* + 1 for null term */); 
         free(strSysConfData);
         return(0);
       }
     else
       {
         syslog(LOG_WARNING, "%s getNtcssSysConfValueFromNis failed for tag (%s)",
                fnct_name, tag);    
       }
  }

  flag = strlen("SPECIAL NO LOCK BACKDOOR");
  if (strncmp(db_dir, "SPECIAL NO LOCK BACKDOOR", (size_t)flag) != 0)
    {
      lock_id = setReadDBLock(SYS_CONF_RW_KEY, LOCK_WAIT_TIME);
      if (lock_id < 0)
        return(ERROR_CANT_ACCESS_DB);
      flag = 0;
    }
  else
    use_lock = 0;

  sprintf(datafile, "%s/%s", &db_dir[flag], "SYS_CONF");
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp != NULL)
    {
      while (fgets(item, MAX_ITEM_LEN, fp) != NULL)
        {
          if (tag == NULL)
            {
              printf("%s", item);
              continue;
            }
          stripNewline(item);
          trim(item);
          if ((strncmp(item, tag, strlen(tag)) == 0) &&
              (item[strlen(tag)] == ':'))
            {
              fclose(fp);
              if (decryptfile[0] != '\0')
                remove(decryptfile);
              if (use_lock)
                unsetReadDBLock(lock_id);
              strcpy(data, trim(&item[strlen(tag) + 1]));
              return(0);
            }
        }
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
    }
  else
    {
      if (use_lock)
        unsetReadDBLock(lock_id);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  if (use_lock)
    unsetReadDBLock(lock_id);

  if (tag == NULL)
    return(0);

  return(ERROR_NO_SUCH_ITEM_REGISTERED);
}


/*****************************************************************************/

int  putNtcssSysConfData( const char  *tag,
                          char        *data,
                          const char  *db_dir )
{
  const char  *fnct_name = "putNtcssSysConfData():";
  char  datafile[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];
  char  item[MAX_ITEM_LEN];
  char  realfile[MAX_FILENAME_LEN];
  FILE  *rp;
  FILE  *fp;
  int  lock_error = 0;  /* =1 if could not obtain a lock */
  int  lock_id;
  int  locks_available;  /* =0 if no daemons running (locks unavailable) */

  lock_id = setWriteDBLock(SYS_CONF_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not get a db lock!", fnct_name);
      lock_error = 1;
      /* We don't return here because the daemons might not be running,
         and so the locks are not active as in the case of the prime_db
         during a fresh install. */
    }
  else
    locks_available = 1;

  if (lock_error)
    {
      if (NumDaemonsRunningOnThisHost() == 0)  /* daemons down */
        {
          locks_available = 0;
          syslog(LOG_WARNING, "%s Daemons are down, continuing to record "
                 "sysconfdata without locks...", fnct_name);
        }
      else
        return(ERROR_CANT_ACCESS_DB);/* couldn't get a lock for some reason. */
    }

  sprintf(realfile, "%s/%s", db_dir, "SYS_CONF");
  rp = decryptAndOpen(realfile, decryptfile, "r");
  if (rp == NULL)
    {
      if (locks_available)
        unsetWriteDBLock(lock_id);
      syslog(LOG_WARNING, "%s Could not read file %s!", fnct_name, realfile);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, "SYS_CONF");
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      if (locks_available)
        unsetWriteDBLock(lock_id);
      syslog(LOG_WARNING, "%s Could not create file %s!", fnct_name, datafile);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
    {
      stripNewline(item);
      trim(item);
      if (!((strncmp(item, tag, strlen(tag)) == 0) &&
            (item[strlen(tag)] == ':')))
        fprintf(fp, "%s\n", item);
    }
  fprintf(fp, "%s: %s\n", tag, trim(data));
  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  closeAndEncrypt(fp, datafile);
  (void) rename(datafile, realfile);
  (void) chmod(realfile, (mode_t)(S_IRUSR | S_IWUSR));
  if (locks_available)
    unsetWriteDBLock(lock_id);

/****  ACA 2/9/00  Commented out, takes too much time
  
  if (buildAndPushNtcssNisMaps() != 0)
    {
      syslog(LOG_WARNING, "%s buildAndPushNtcssNisMaps failed!", fnct_name);
    }
*******************************/

  return(0);
}


/*****************************************************************************/

const char  *getErrorMessage( int num )

{
  switch (num)
    {
    case ERROR_CANT_PARSE_MSG:
      return("Could not parse message.");
    case ERROR_CANT_ACCESS_DB:
      return("Could not access database.");
    case ERROR_CANT_OPEN_DB_FILE:
      return("Could not open database file.");
    case ERROR_NO_SUCH_USER:
      return("No such NTCSS user.");
    case ERROR_INCORRECT_PASSWORD:
      return("Incorrect password.");
    case ERROR_NO_GROUP_INFO_FOUND:
      return("Could not find application information.");
    case ERROR_NO_PROGRAM_INFO_FOUND:
      return("Could not find program information.");
    case ERROR_NO_SUCH_APP_USER:
      return("No such application user.");
    case ERROR_NO_SUCH_GROUP:
      return("No such application.");
    case ERROR_NO_PRT_INFO_FOUND:
      return("Could not find printer information.");
    case ERROR_NO_PRT_CLS_INFO_FOUND:
      return("Could not find printer class information.");
    case ERROR_NO_OTYPE_INFO_FOUND:
      return("Could not find output type information.");
    case ERROR_NO_SUCH_OTYPE:
      return("No such output type.");
    case ERROR_CANT_INSERT_VALUE:
      return("Could not insert data record.");
    case ERROR_INCORRECT_OLD_PASSWD:
      return("Incorrect old password.");
    case ERROR_INCORRECT_NEW_PASSWD:
      return("Invalid new password.");
    case ERROR_CANT_PARSE_ARGS:
      return("Could not parse command line arguments.");
    case ERROR_NO_SUCH_PRT:
      return("No such printer.");
    case ERROR_CANT_DETERMINE_HOST:
      return("Failed to determine hostname.");
    case ERROR_NO_NEED_TO_SUSPEND:
      return("No need to suspend print request.");
    case ERROR_QUEUE_DISABLED:
      return("Printer queue is disabled.");
    case ERROR_CANT_READ_VALUE:
      return("Could not read data record.");
    case ERROR_CANT_CREATE_QUERY:
      return("Could not create query file.");
    case ERROR_USER_ALREADY_IN:
      return("User already logged in.");
    case ERROR_CANT_UNLOCK_LOGIN:
      return("Could not unlock login files.");
    case ERROR_CANT_ACCESS_LOGIN:
      return("Could not access login files.");
    case ERROR_CANT_GET_PID_DATA:
      return("Could not determine PID data.");
    case ERROR_CANT_DELETE_VALUE:
      return("Could not delete data record.");
    case ERROR_USER_NOT_VALID:
      return("User cannot be validated.");
    case ERROR_CANT_REL_REQUEST:
      return("Could not release request.");
    case ERROR_CANT_KILL_SVR_PROC:
      return("Could not kill server process.");
    case ERROR_CANT_REPLACE_VALUE:
      return("Could not replace data record.");
    case ERROR_INVALID_NUM_ARGS:
      return("Invalid number of arguments.");
    case ERROR_CANT_TALK_TO_SVR:
      return("Could not communicate with daemon.");
    case ERROR_NO_SUCH_FILE:
      return("Failed to locate file.");
    case ERROR_CANT_REMOVE_ACT_PROCESS:
      return("Cannot remove active process.");
    case ERROR_CANT_ACCESS_DIR:
      return("Cannot access directory.");
    case ERROR_NTCSS_LP_FAILED:
      return("Server print command failed.");
    case ERROR_BB_MSG_DOES_NOT_EXIST:
      return("Bulletin board message does not exist.");
    case ERROR_BB_DOES_NOT_EXIST:
      return("Bulletin board does not exist.");
    case ERROR_USER_LOGIN_DISABLED:
      return("User login is disabled.");
    case ERROR_CANT_OPEN_FILE:
      return("Could not open file.");
    case ERROR_NOT_UNIX_USER:
      return("User has no UNIX account.");
    case ERROR_CANT_DETER_MASTER:
      return("Could not determine Master Server.");
    case ERROR_CANT_COMM_WITH_MASTER:
      return("Could not communicate with Master Server.");
    case ERROR_CANT_TRANSFER_FILE:
      return("Could not transfer file across network.");
    case ERROR_CANT_CREATE_MASTER_BACKUP:
      return("Could not create Master Server backup.");
    case ERROR_NO_SUCH_ITEM_REGISTERED:
      return("No such database item is registered.");
    case ERROR_SERVER_IDLED:
      return("System is idled and not accepting any requests.");
    case ERROR_SYSTEM_LOCKED:
      return("System is locked and not allowing login requests.");
    case ERROR_INVALID_COMDB_TAG_VALUE:
      return("Invalid common data tag value.");
    case ERROR_INVALID_COMDB_ITEM_VALUE:
      return("Invalid common data item value.");
    case ERROR_CANT_FIND_VALUE:
      return("Could not find database record.");
    case ERROR_CANT_KILL_NON_ACTIVE_PROC:
      return("Process is currently not running.");
    case ERROR_NO_SUCH_HOST:
      return("No such NTCSS Server.");
    case ERROR_NO_SUCH_PROCESS:
      return("No such server process.");
    case ERROR_CANT_SET_MASTER:
      return("Could not set value of Master Server.");
    case ERROR_CANT_UNPACK_MASTER_BACKUP:
      return("Could not unpack Master Server backup.");
    case ERROR_DB_SOFTWARE_VERSION_MISMATCH:
      return("Database and software version numbers are incompatible.");
    case ERROR_DAEMON_IS_AWAKE:
      return("Daemon is awake.");
    case ERROR_CANT_CHANGE_NTCSS_APP_FILE:
      return("Could not change NTCSS application initialization file.");
    case ERROR_HOST_IS_NOT_MASTER:
      return("This host is not a Master Server.");
    case ERROR_CANT_RENAME_FILE:
      return("Could not rename file.");
    case ERROR_INVALID_BB_NAME:
      return("Invalid bulletin board name.");
    case ERROR_BB_ALREADY_EXISTS:
      return("Bulletin board already exists.");
    case ERROR_CHANGE_PASSWD_FAILED:
      return("Change password script failed.");
    case ERROR_CANT_SET_ADMIN_LOCK:
      return("Could not set admin lock.");
    case ERROR_CANT_SIGNAL_PROCESS:
      return("Could not signal process.");
    case ERROR_CANT_CREATE_INI_FILE:
      return("Could not create INI file.");
    case ERROR_CANT_EXTRACT_INI_FILE:
      return("Could not extract INI file.");
    case ERROR_CANT_GET_FILE_INFO:
      return("Could not get file information.");
    case ERROR_PROCESS_NOT_IN_VALID_STATE_FOR_REMOVAL:
      return("Process is not in a valid state for removal.");
    case ERROR_RUN_NEXT_TAKEN:
      return("Run Next Process Exists!");
    case ERROR_REMOVE_FROM_WRONG_PROGROUP:
      return("Not allowed to remove a process queue item in a"
             " different program group.");
    case ERROR_KILL_FROM_WRONG_PROGROUP:
      return("Not allowed to stop a process in a different"
             " program group.");
    case ERROR_CANT_LOCK_APPLICATION:
      return("Failed to lock the application.");
    case ERROR_CANT_UNLOCK_APPLICATION:
      return("Failed to unlock the application.");
    case ERROR_USER_LOGGED_IN:
      return("User logged in.");
    case ERROR_APP_TO_DM_PIPE_OPEN:
      return("Error opening pipe from App to DM");
    case ERROR_APP_TO_DM_PIPE_WRITE:
      return("Error writing to pipe from App to DM");
    case ERROR_DM_SEMAPHORE:
      return("Error on DM Pipe semaphore");
    case ERROR_DM_TO_APP_PIPE_OPEN:
      return("Error opening pipe from DM to App");
    case ERROR_DM_TO_APP_PIPE_READ:
      return("Error reading pipe from DM to App");
    case ERROR_LOCAL_PROGRAM_INFO:
      return("Error with local program info");
    case ERROR_NOT_USER_AUTH_SERVER_OR_MASTER:
      return("Error.  Server is not the Master or user's PAS");
    case ERROR_NTCSS_BUILD_AND_PUSH_NIS_ERROR:
      return("Error building NTCSS NIS Maps");
    case ERROR_PASSWORD_TRANSLATION_FAILED:
      return("Error password translation failed");

    default:
        return("getErrorMessage(): INVALID ERROR NUMBER!");
    }
}





/*****************************************************************************/

const char  *getAccessRolesTableName( void )

{
  return(tables[ACCESS_ROLES_ID]);
}


/*****************************************************************************/

const char  *getCurrentUsersTableName( void )

{
  return(tables[CURRENT_USERS_ID]);
}


/*****************************************************************************/

const char  *getCurrentAppsTableName( void )

{
  return(tables[CURRENT_APPS_ID]);
}


/*****************************************************************************/

int  getCurrentUsersNumRecs( void )

{
  return(CURRENT_USERS_NUM_RECS);
}


/*****************************************************************************/

const char  *getLoginHistoryTableName( void )

{
  return(tables[LOGIN_HISTORY_ID]);
}


/*****************************************************************************/

int  getLoginHistoryNumRecs( void )

{
  return(LOGIN_HISTORY_NUM_RECS);
}


/*****************************************************************************/

static int  getSqlChange( DBRecord *record,
                          FILE     *fp )
{
  int  i;
  char item[MAX_ITEM_LEN];

  if (fgets(item, MAX_ITEM_LEN, fp) == NULL)
    return(0);
  stripNewline(item);
  record->sql_change = atoi(item);

  i = 0;
  while (fgets(item, MAX_ITEM_LEN, fp) != NULL)
    {
      stripNewline(item);
      if (isBlank(item))
        break;
      strcpy(record->field[i].data, item);
      i++;
    }
  record->num_items = i;
  
  return(1);
}


/*****************************************************************************/

int  copy_database_files( const char *db_dir,
                          int         mode )
{
  char  cmd[MAX_CMD_LEN];
  char  strFileName[MAX_FILENAME_LEN + 1];
  int  error = 0;                  /* ==1 if an error occured */
  int  i, x;
  struct stat  fileStatus;

  for (i = 0; i < NUM_TABLES; i++)
    {
      switch (mode)
        {
        case ONLINE_TO_TMP:  /* 0 */
          /* copy current db files to their .tmp equivalents */
          sprintf(cmd, "/bin/cp %s/%s %s/%s.tmp", db_dir, tables[i],
                  db_dir, tables[i]);
          x = system(cmd);
          if (x != 0)
            {
              syslog(LOG_WARNING, "copy_database_files: Problems copying"
                     " file %s/%s to %s.tmp!", db_dir, tables[i], tables[i]);
              error = 1;
            }
          break;

          /* MOVE_EXISTING_TMP_FILES_ONLINE is more or less a flag that
             prevents syslog messages about moving files in the case where
             they don't exist. */

          /* **NOTE***  DO NOT SEPARATE THESE TWO!! */
        case MOVE_EXISTING_TMP_FILES_ONLINE: /* 5 */
        case TMP_TO_ONLINE:  /* 1 */
          sprintf(strFileName, "%s/%s.tmp", db_dir, tables[i]);
          x = stat(strFileName, &fileStatus);
          if (x != 0)
            {
              x = errno;
              if (x == ENOENT)  /* File does not exist! */
                {
                  if (mode == TMP_TO_ONLINE)
                    syslog(LOG_WARNING, "copy_database_files: <%s> does not"
                           " exist!", strFileName);
                  continue;
                }
              else
                syslog(LOG_WARNING, "copy_database_files: stat() on file <%s>"
                       " returned error %i! (%s)", strFileName, x,
                       sys_errlist[x]);
            }
          /* replace current db files with their .tmp equivalents */
          sprintf(cmd, "/bin/mv %s/%s.tmp %s/%s", db_dir, tables[i],
                  db_dir, tables[i]);
          x = system(cmd);  /* FIX if fails we have corrupted DB */
          if (x != 0)
            syslog(LOG_WARNING, "copy_database_files: mv returned error %i"
                   " on file %s.tmp!", x, tables[i]);
          sprintf(cmd, "%s/%s", db_dir, tables[i]);
          x = chmod(cmd, (mode_t)(S_IRUSR | S_IWUSR));
          if (x != 0)
            syslog(LOG_WARNING, "copy_database_files: chmod returned error "
                   " %i for file %s!", x, tables[i]);
          break;

        case REMOVE_TMP:  /* 2 */
          /* remove all the .tmp db files */
          sprintf(cmd, "%s/%s.tmp", db_dir, tables[i]);
          x = unlink(cmd);
          if (x != 0)
            syslog(LOG_WARNING, "copy_database_files: unlink() returned "
                   "error %i on file %s!", x, cmd);
          break;
          
        case REMOVE_OLD:  /* 4 */
          /* Remove all the .old db files */
          sprintf(cmd, "%s/%s.old", db_dir, tables[i]);
          x = unlink(cmd);
          if (x != 0)
            syslog(LOG_WARNING, "copy_database_files: unlink() returned "
                   "error %i on file %s!", x, cmd);
          break;

        case INIT_TO_TMP:  /* 3 */
          /* Copy the current db files to .tmp/.old files. */
          switch (i)
            {
            case PROGROUPS_ID:   /* 1 */
            case PROGRAMS_ID:    /* 2 */
/* DWB      case ACCESS_AUTH_ID:    4 */
            case PRT_CLASS_ID:   /* 6*/
            case HOSTS_ID:       /* 10*/
              sprintf(cmd, "/bin/cp %s/%s %s/%s.old", db_dir, tables[i],
                      db_dir, tables[i]);
              x = system(cmd);
              if (x != 0)
                {
                  syslog(LOG_WARNING, "copy_database_files: cp returned error "
                         " %i copying file %s to %s.old!", x, tables[i],
                         tables[i]);
                  error = 1;
                }
              sprintf(cmd, "%s/%s.tmp", db_dir, tables[i]);
              if (access(cmd, F_OK) != -1)
                {
                  x = unlink(cmd);
                  if (x != 0)
                    {
                      syslog(LOG_WARNING, "copy_database_files: unlink() "
                             " returned error %i on file %s!", x, cmd);
                      error = 1;
                    }
                }
              sprintf(cmd, "/bin/touch %s/%s.tmp", db_dir, tables[i]);
              break;

            case ACCESS_ROLES_ID:
              sprintf(cmd, "/bin/cp %s/%s %s/%s.old", db_dir, tables[i],
                      db_dir, tables[i]);
              x = system(cmd);
              if (x != 0)
                {
                  syslog(LOG_WARNING, "copy_database_files: cp %s %s.old "
                         "returned error %i!", tables[i], tables[i], x);
                  error = 1;
                }
              sprintf(cmd, "/bin/cp %s/%s %s/%s.tmp", db_dir, tables[i],
                      db_dir, tables[i]);
              break;

            default:
              sprintf(cmd, "/bin/cp %s/%s %s/%s.tmp", db_dir, tables[i],
                      db_dir, tables[i]);
              break;
            }
          x = system(cmd);
          if (x != 0)
            {
              syslog(LOG_WARNING, "copy_database_files: Problems executing"
                     " command <%s>!  Error %i", cmd, x);
              error = 1;
            }
          break;
        }  /* end of first case */
    }      /* end of for loop */

  if (error)
    return(0);
  else
    return(1);
}


/*****************************************************************************/

/*
static int  has_program_access(user_role, app_role, program_access)

     int  user_role;
     int  app_role;
     int  program_access;

{
  if (app_role & program_access)
    return(1);
  else
    return(0);
}
*/


/*****************************************************************************/
/*  Mallocs memory containing the ntcss_users db record for the specified
    login name.  The caller is responsible for freeing the pointer parameter..
*/
int  getUserDataByLoginName( UserItem    *ppUserItem[],
                             const char  *strLoginName,
                             const char  *strDbDir )
{

  UserItem **userData;
  int    i;
  int    nUsers;
  int    nFound = 0;

  userData = getUserData(&nUsers, strDbDir);
  if (userData == NULL)
    {
      *ppUserItem = (UserItem*) NULL;
      syslog(LOG_WARNING, "getUserDataByLoginName: getUserData failed");
      return(-1);
    }

  for (i = 0; i < nUsers; i++)
    {
      if (strcmp(userData[i]->login_name, strLoginName) != 0)
        continue;

      *ppUserItem = (UserItem *) malloc(sizeof(UserItem));
      (*ppUserItem)->marker = 1;
      (*ppUserItem)->unix_id = userData[i]->unix_id;
      (*ppUserItem)->login_name = allocAndCopyString(userData[i]->login_name); 
      (*ppUserItem)->real_name = allocAndCopyString(userData[i]->real_name);
      (*ppUserItem)->password = allocAndCopyString(userData[i]->password);
      (*ppUserItem)->ss_number = allocAndCopyString(userData[i]->ss_number);
      (*ppUserItem)->phone_number
        = allocAndCopyString(userData[i]->phone_number);
      (*ppUserItem)->security_class = userData[i]->security_class;
      (*ppUserItem)->pw_change_time
        = allocAndCopyString(userData[i]->pw_change_time);
      (*ppUserItem)->user_role = userData[i]->user_role;
      (*ppUserItem)->user_lock = userData[i]->user_lock;
      (*ppUserItem)->shared_passwd
        = allocAndCopyString(userData[i]->shared_passwd);
      (*ppUserItem)->auth_server
        = allocAndCopyString(userData[i]->auth_server);
      nFound = 1;
      break;

    }          /* END for */

  freeUserData(userData, nUsers);

  if (nFound)
    {
      return(0);
    }

  return(-1);
}

/*****************************************************************************/

UserItem  **getUserData( int        *num_users,
                         const char *dir )
{
  UserItem  **Users;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];
  char  key[(MAX_LOGIN_NAME_LEN * 2) + 1];
  /* char  *decryptedSSN; */

  *num_users = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[NTCSS_USERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[NTCSS_USERS_ID]);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "getUserData: Could not open file %s!", datafile);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((UserItem **) 0);
    }

  *num_users = 0;

  Users = (UserItem **) malloc(sizeof(char *)*MAX_NUM_NTCSS_USERS);

  while (getRecord(&record, fp, NTCSS_USERS_NUM_RECS) &&
         *num_users < MAX_NUM_NTCSS_USERS)
    {
      Users[*num_users] = (UserItem *) malloc(sizeof(UserItem));
      Users[*num_users]->marker = 1;
      Users[*num_users]->unix_id = atoi(record.field[0].data);
      Users[*num_users]->login_name = allocAndCopyString(record.field[1].data);
      Users[*num_users]->real_name = allocAndCopyString(record.field[2].data);
      Users[*num_users]->password = allocAndCopyString(record.field[3].data);

      sprintf(key, "%s%s", Users[*num_users]->login_name, 
                           Users[*num_users]->login_name);
      /*
      decryptedSSN = 0;

      ret = plainDecryptString(key, record.field[4].data, &decryptedSSN, 0, 0);
      if (ret == 0) {
         Users[*num_users]->ss_number = allocAndCopyString(decryptedSSN); 
         free(decryptedSSN);
      }
      else 
         syslog(LOG_WARNING, "getUserData: Could not convert SSN "
                "for user <%s>",record.field[1].data);
      */

      Users[*num_users]->ss_number = 
                                allocAndCopyString(record.field[4].data); 

      Users[*num_users]->phone_number = allocAndCopyString(record.field[5].data);
      Users[*num_users]->security_class = atoi(record.field[6].data);
      Users[*num_users]->pw_change_time = allocAndCopyString(record.field[7].data);
      Users[*num_users]->user_role = atoi(record.field[8].data);
      Users[*num_users]->user_lock = atoi(record.field[9].data);
      Users[*num_users]->shared_passwd = allocAndCopyString(record.field[10].data);
      Users[*num_users]->auth_server = allocAndCopyString(record.field[11].data);

      /* this is here as opposed to next to where the error occurred, so that
         the rest of the current Users stucture could be "alloc"ed to make
         sure the freeUserData function wouldn't fail */
  /*
      if (ret != 0) {
         fclose(fp);
         if (decryptfile[0] != '\0')
         remove(decryptfile);
         freeUserData(Users, num_users);
         return((UserItem **) 0);
      } */

      (*num_users)++;
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  return(Users);
}

UserItem  **getUserDataByName(const char *user_name,
			int *num_users,
                         const char *dir )
{
  UserItem  **Users;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];
  char  key[(MAX_LOGIN_NAME_LEN * 2) + 1];
  /* char  *decryptedSSN; */

  *num_users = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[NTCSS_USERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[NTCSS_USERS_ID]);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "getUserData: Could not open file %s!", datafile);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((UserItem **) 0);
    }

  *num_users = 0;

  Users = (UserItem **) malloc(sizeof(char *)*MAX_NUM_NTCSS_USERS);

  while (getRecord(&record, fp, NTCSS_USERS_NUM_RECS) &&
         *num_users < MAX_NUM_NTCSS_USERS)
    {
      if (!strcmp(user_name,record.field[1].data)) {
      	Users[*num_users] = (UserItem *) malloc(sizeof(UserItem));
      	Users[*num_users]->marker = 1;
      	Users[*num_users]->unix_id = atoi(record.field[0].data);
      	Users[*num_users]->login_name = allocAndCopyString(record.field[1].data);
      	Users[*num_users]->real_name = allocAndCopyString(record.field[2].data);
      	Users[*num_users]->password = allocAndCopyString(record.field[3].data);
	
      	sprintf(key, "%s%s", Users[*num_users]->login_name, 
                           	Users[*num_users]->login_name);
      	/*
      	decryptedSSN = 0;
	
      	ret = plainDecryptString(key, record.field[4].data, &decryptedSSN, 0, 0);
      	if (ret == 0) {
         	Users[*num_users]->ss_number = allocAndCopyString(decryptedSSN); 
         	free(decryptedSSN);
      	}
      	else 
         	syslog(LOG_WARNING, "getUserData: Could not convert SSN "
                	"for user <%s>",record.field[1].data);
      	*/
	
      	Users[*num_users]->ss_number = 
                                allocAndCopyString(record.field[4].data); 
	
      	Users[*num_users]->phone_number = allocAndCopyString(record.field[5].data);
      	Users[*num_users]->security_class = atoi(record.field[6].data);
      	Users[*num_users]->pw_change_time = allocAndCopyString(record.field[7].data);
      	Users[*num_users]->user_role = atoi(record.field[8].data);
      	Users[*num_users]->user_lock = atoi(record.field[9].data);
      	Users[*num_users]->shared_passwd = allocAndCopyString(record.field[10].data);
      	Users[*num_users]->auth_server = allocAndCopyString(record.field[11].data);

      	(*num_users)++;
	break;
      }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  return(Users);
}

/*****************************************************************************/

void  writeUserData( UserItem  **Users,
                     int         num_users,
                     FILE       *fp )
{
  int  i;
  /*char key[(MAX_LOGIN_NAME_LEN * 2) + 1];*/
  /* char *encryptedSSN; */

  for (i = 0; i < num_users; i++)
    if (Users[i]->marker == 1) {
      /* re-encrypt the SSN */
      /*
      sprintf(key, "%s%s", Users[i]->login_name, Users[i]->login_name);
      if (plainEncryptString(key, Users[i]->ss_number, 
                             &encryptedSSN, 0, 0) != 0) {
         syslog(LOG_WARNING, "writeUserData: Could not convert SSN for <%s>!",
                Users[i]->login_name);
      }
      */

      fprintf(fp, NTCSS_USERS_FMT,
              Users[i]->unix_id,
              Users[i]->login_name,
              Users[i]->real_name,
              Users[i]->password,
              Users[i]->ss_number, 
              /* encryptedSSN, */
              Users[i]->phone_number,
              Users[i]->security_class,
              Users[i]->pw_change_time,
              Users[i]->user_role,
              Users[i]->user_lock,
              Users[i]->shared_passwd,
              Users[i]->auth_server);

      /* free(encryptedSSN); */
    }
}


/*****************************************************************************/

void  freeUserData( UserItem  **Users,
                    int         num_users )
{
  int  i;

  for (i = num_users - 1; i >= 0; i--)
    {
      free(Users[i]->login_name);
      free(Users[i]->real_name);
      free(Users[i]->password);
      free(Users[i]->shared_passwd);
      free(Users[i]->ss_number);
      free(Users[i]->phone_number);
      free(Users[i]->pw_change_time);
      free(Users[i]->auth_server);
      free((char *) Users[i]);
    }
  free((char *) Users);
}


/*****************************************************************************/


int  doesProgramGroupExist( const char  *app,
                            const char  *dir )
{
  DBRecord  record;
  FILE  *fp;
  char  datafile[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PROGROUPS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PROGROUPS_ID]);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(0);
    }

  while (getRecord(&record, fp, PROGROUPS_NUM_RECS))
    {
      if (strcmp(app, record.field[0].data) == 0)
        {
          fclose(fp);
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(1);
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  
  return(0);
}


/*****************************************************************************/
/* Reads in records from the progroups database and creates an array of them
   filtered according to mode */

AppItem  **getAppData( int        *num_apps,
                       const char *dir,
                       int         mode )
{
  AppItem  **Apps;
  DBRecord  record;
  char   buf[MAX_ITEM_LEN];
  char   datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "getAppData():";
  char   item[MAX_ITEM_LEN];
  int    j;
  FILE  *fp;
  char   decryptfile[MAX_FILENAME_LEN];
  
  *num_apps = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PROGROUPS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PROGROUPS_ID]);

  Apps = (AppItem **) malloc(sizeof(AppItem *) * MAX_NUM_PROGROUPS);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, datafile);
      freeAppData(Apps, *num_apps);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AppItem **) 0);
    }

  *num_apps = 0;
  while (getRecord(&record, fp, PROGROUPS_NUM_RECS) &&
         *num_apps < MAX_NUM_PROGROUPS)
    {
      /* NOTE, since role changes for v 3.0.0, the NTCSS progroup should not
         be singled out from the other progroups like the following... */
      if ((strcmp(NTCSS_APP, record.field[0].data) != 0) ||
          (mode == WITH_NTCSS_GROUP) ||
          (mode == WITH_NTCSS_AND_WITH_ROLES) ||
           (mode == WITH_NTCSS_AND_WITH_OLD_ROLES))
        {
          Apps[(*num_apps)] = (AppItem *) malloc(sizeof(AppItem));
          Apps[*num_apps]->marker = 1;
          Apps[*num_apps]->progroup_title
            = allocAndCopyString(record.field[0].data);
          Apps[*num_apps]->hostname
            = allocAndCopyString(record.field[1].data);
          Apps[*num_apps]->icon_file
            = allocAndCopyString(record.field[2].data);
          Apps[*num_apps]->icon_index = atoi(record.field[3].data);
          Apps[*num_apps]->version_number
            = allocAndCopyString(record.field[4].data);
          Apps[*num_apps]->release_date
            = allocAndCopyString(record.field[5].data);
          Apps[*num_apps]->client_location
            = allocAndCopyString(record.field[6].data);
          Apps[*num_apps]->server_location
            = allocAndCopyString(record.field[7].data);
          Apps[*num_apps]->unix_group
            = allocAndCopyString(record.field[8].data);
          Apps[*num_apps]->num_processes = atoi(record.field[9].data);
          Apps[*num_apps]->link_data
            = allocAndCopyString(record.field[10].data);
          Apps[*num_apps]->num_roles = 0;  /* not stored in database */
          (*num_apps)++;
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  /*  WITHOUT_NTCSS_GROUP should never be requested for all new versions! */
  if ((mode == WITHOUT_NTCSS_GROUP) || (mode == WITH_NTCSS_GROUP))
    return(Apps);

  if (mode == WITH_NTCSS_AND_WITH_OLD_ROLES)
    sprintf(datafile, "%s/%s.old", dir, tables[ACCESS_ROLES_ID]);
  else
    if (using_tmp)
      sprintf(datafile, "%s/%s.tmp", dir, tables[ACCESS_ROLES_ID]);
    else
      sprintf(datafile, "%s/%s", dir, tables[ACCESS_ROLES_ID]);
  
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, datafile);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(Apps);
    }

  if ((fgets(item, MAX_ITEM_LEN, fp) != NULL) &&
      (mode == WITH_NTCSS_AND_WITH_OLD_ROLES) &&
      (strncmp(item, EMPTY_STR, strlen(EMPTY_STR)) == 0))
    goto empty_file;  /* db must have just been installed or primed */

  /* for every record in the progroups list just obtained...*/  
  for (j = 0; j < *num_apps; j++) 
    {
      rewind(fp);  /* go back to the beginning of the .old/.tmp file.. */

      /* look in access_rolls.old/.tmp for matching progroup titles.. */
      while (fgets(item, MAX_ITEM_LEN, fp) != NULL)
        {
          stripNewline(item);
          if (strcmp(Apps[j]->progroup_title, item) != 0)
            continue;         /* ..not the progroup title we're looking for. */

          if ((fgets(item, MAX_ITEM_LEN, fp) != NULL) && /* access_role */
              (fgets(buf, MAX_ITEM_LEN, fp) != NULL))    /* role_number */
            {                 /* found an access_role for this progroup.... */
              stripNewline(item);  
              stripNewline(buf);
              /* store the name of this role in the index representing which
                 bit this role is associated with in app_role entries. */
              Apps[j]->roles[atoi(buf) - 1] = allocAndCopyString(item); /* role */
              Apps[j]->num_roles++;
            }
          else
            goto empty_file;

          /* skip over the record separating line.. */
          if (fgets(item, MAX_ITEM_LEN, fp) == NULL)
            goto empty_file;
        }
    }
  
 empty_file:
  
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  
  if (mode == WITH_NTCSS_AND_WITH_OLD_ROLES)
    (void) unlink(datafile);
  
  return(Apps);
}


/*****************************************************************************/

void  writeAppData( AppItem  **Apps,
                    int        num_apps,
                    FILE      *fp )
{
  int i;

  for (i = 0; i < num_apps; i++)
    if (Apps[i]->marker == 1)
      fprintf(fp, PROGROUPS_FMT,
              Apps[i]->progroup_title,
              Apps[i]->hostname,
              Apps[i]->icon_file,
              Apps[i]->icon_index,
              Apps[i]->version_number,
              Apps[i]->release_date,
              Apps[i]->client_location,
              Apps[i]->server_location,
              Apps[i]->unix_group,
              Apps[i]->num_processes,
              Apps[i]->link_data);
}



/*****************************************************************************/
/* Frees memory created by getAppData.
 */

void  freeAppData( AppItem  **Apps,
                   int        num_apps)
{

  int   i, j;

  for (i = num_apps - 1; i >= 0; i--)
    {
      free(Apps[i]->progroup_title);
      free(Apps[i]->hostname);
      free(Apps[i]->icon_file);
      free(Apps[i]->version_number);
      free(Apps[i]->release_date);
      free(Apps[i]->client_location);
      free(Apps[i]->server_location);
      free(Apps[i]->unix_group);
      free(Apps[i]->link_data);
      for (j = Apps[i]->num_roles - 1; j >= 0; j--)
        free(Apps[i]->roles[j]);
      free(Apps[i]);
    }
  free(Apps);

  return;
}


/*****************************************************************************/

ProgramItem  **getProgramData( int         *num_programs,
                               const char  *dir,
                               int          with_ntcss ) /* No longer used. */
{
  ProgramItem  **Programs;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  /* this is here to satifsy compiler warning about the parameter not being
     used. if the parameter is ever used in the future, the following will
     no longer be needed. */
  with_ntcss=with_ntcss;

  *num_programs = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PROGRAMS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PROGRAMS_ID]);

  Programs = (ProgramItem **) malloc(sizeof(char *)*MAX_NUM_PROGRAMS);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeProgramData(Programs, *num_programs);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((ProgramItem **) 0);
    }
  
  *num_programs = 0;
  while (getRecord(&record, fp, PROGRAMS_NUM_RECS) &&
         *num_programs < MAX_NUM_PROGRAMS)
    {
      Programs[*num_programs] = (ProgramItem *) malloc(sizeof(ProgramItem));
      Programs[*num_programs]->marker = 1;
      Programs[*num_programs]->program_title = allocAndCopyString(record.field[0].data);
      Programs[*num_programs]->program_file = allocAndCopyString(record.field[1].data);
      Programs[*num_programs]->icon_file = allocAndCopyString(record.field[2].data);
      Programs[*num_programs]->icon_index = atoi(record.field[3].data);
      Programs[*num_programs]->cmd_line_args = allocAndCopyString(record.field[4].data);
      Programs[*num_programs]->progroup_title = allocAndCopyString(record.field[5].data);
      Programs[*num_programs]->security_class = atoi(record.field[6].data);
      Programs[*num_programs]->program_access = atoi(record.field[7].data);
      Programs[*num_programs]->flag = atoi(record.field[8].data);
      Programs[*num_programs]->working_dir = allocAndCopyString(record.field[9].data);
      (*num_programs)++;
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Programs);
}


/*****************************************************************************/

static void  writeProgramData(Programs, num_programs, fp)

     ProgramItem  **Programs;
     int  num_programs;
     FILE  *fp;
{
  int  i;

  for (i = 0; i < num_programs; i++)
    if (Programs[i]->marker == 1)
      fprintf(fp, PROGRAMS_FMT,
              Programs[i]->program_title,
              Programs[i]->program_file,
              Programs[i]->icon_file,
              Programs[i]->icon_index,
              Programs[i]->cmd_line_args,
              Programs[i]->progroup_title,
              Programs[i]->security_class,
              Programs[i]->program_access,
              Programs[i]->flag,
              Programs[i]->working_dir);
}


/*****************************************************************************/

void freeProgramData(Programs, num_programs)
ProgramItem **Programs;
int num_programs;
{
    int i;

    for (i = num_programs - 1; i >= 0; i--) {
        free(Programs[i]->program_title);
        free(Programs[i]->program_file);
        free(Programs[i]->icon_file);
        free(Programs[i]->cmd_line_args);
        free(Programs[i]->progroup_title);
        free(Programs[i]->working_dir);
        free((char *) Programs[i]);
    }
    free((char *) Programs);
}

/*****************************************************************************/

AppuserItem  **getAppuserData(num_appusers, dir, with_ntcss)

     int  *num_appusers;
     const char  *dir;
     int  with_ntcss;

{
  AppuserItem  **Appusers;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_appusers = -1;

  if (using_tmp)                                          /* using temp file */
    sprintf(datafile, "%s/%s.tmp", dir, tables[APPUSERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[APPUSERS_ID]);
  
  Appusers = (AppuserItem **) malloc(sizeof(char *)*MAX_NUM_APPUSERS);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppuserData(Appusers, *num_appusers);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AppuserItem **) 0);
    }

  *num_appusers = 0;
  while (getRecord(&record, fp, APPUSERS_NUM_RECS) &&
         *num_appusers < MAX_NUM_APPUSERS)
    {
      if ((strcmp(NTCSS_APP, record.field[0].data) != 0) || with_ntcss)
        {   /* (not ntcss admin app) or (get all listings) */
          Appusers[*num_appusers]
            = (AppuserItem *) malloc(sizeof(AppuserItem));
          Appusers[*num_appusers]->marker = 1;
          Appusers[*num_appusers]->progroup_title
            = allocAndCopyString(record.field[0].data);
          Appusers[*num_appusers]->login_name
            = allocAndCopyString(record.field[1].data);
          Appusers[*num_appusers]->real_name
            = allocAndCopyString(record.field[2].data);
          Appusers[*num_appusers]->app_data
            = allocAndCopyString(record.field[3].data);
          Appusers[*num_appusers]->app_passwd
            = allocAndCopyString(record.field[4].data);
          Appusers[*num_appusers]->app_role
            = atoi(record.field[5].data);
          Appusers[*num_appusers]->registered_user
            = atoi(record.field[6].data);
          (*num_appusers)++;
        }
    }

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Appusers);
}

/*** NEW FOR ACA/JJ *****/
AppuserItem  **getAppuserDataByName(user_name,num_appusers, dir, with_ntcss)
     const char *user_name;
     int  *num_appusers;
     const char  *dir;
     int  with_ntcss;

{
  AppuserItem  **Appusers;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];
  int msize;

  *num_appusers = -1;

  if (using_tmp)                                          /* using temp file */
    sprintf(datafile, "%s/%s.tmp", dir, tables[APPUSERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[APPUSERS_ID]);
  
/*
  Appusers = (AppuserItem **) malloc(sizeof(char *)*MAX_NUM_APPUSERS);
*/
  Appusers = NULL;

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppuserData(Appusers, *num_appusers);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AppuserItem **) 0);
    }

  *num_appusers = 0;
  while (getRecord(&record, fp, APPUSERS_NUM_RECS) &&
         *num_appusers < MAX_NUM_APPUSERS)
    {
      if ((strcmp(NTCSS_APP, record.field[0].data) != 0) || with_ntcss)
        {   /* (not ntcss admin app) or (get all listings) */
          if (!strcmp(record.field[1].data,user_name)) {
                msize=(sizeof(char *) * (*num_appusers+1));
  		Appusers = (AppuserItem **) realloc(Appusers,msize);
          	Appusers[*num_appusers]
            		= (AppuserItem *) malloc(sizeof(AppuserItem));
          	Appusers[*num_appusers]->marker = 1;
          	Appusers[*num_appusers]->progroup_title
            		= allocAndCopyString(record.field[0].data);
          	Appusers[*num_appusers]->login_name
            		= allocAndCopyString(record.field[1].data);
          	Appusers[*num_appusers]->real_name
            		= allocAndCopyString(record.field[2].data);
          	Appusers[*num_appusers]->app_data
            		= allocAndCopyString(record.field[3].data);
          	Appusers[*num_appusers]->app_passwd
            		= allocAndCopyString(record.field[4].data);
          	Appusers[*num_appusers]->app_role
            		= atoi(record.field[5].data);
          	Appusers[*num_appusers]->registered_user
            		= atoi(record.field[6].data);
          	(*num_appusers)++;
	    }
        }
    }

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Appusers);
}

/*** NEW FOR ACA/JJ *****/
AppuserItem  **getAppuserDataByApp(app_name,num_appusers, dir, with_ntcss)
     const char *app_name;
     int  *num_appusers;
     const char  *dir;
     int  with_ntcss;

{
  AppuserItem  **Appusers;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];
  int msize;

  *num_appusers = -1;

  if (using_tmp)                                          /* using temp file */
    sprintf(datafile, "%s/%s.tmp", dir, tables[APPUSERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[APPUSERS_ID]);
  
/*
  Appusers = (AppuserItem **) malloc(sizeof(char *)*MAX_NUM_APPUSERS);
*/
  Appusers = NULL;

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppuserData(Appusers, *num_appusers);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AppuserItem **) 0);
    }

  *num_appusers = 0;
  while (getRecord(&record, fp, APPUSERS_NUM_RECS) &&
         *num_appusers < MAX_NUM_APPUSERS)
    {
      if ((strcmp(NTCSS_APP, record.field[0].data) != 0) || with_ntcss)
        {   /* (not ntcss admin app) or (get all listings) */
          if (!strcmp(record.field[0].data,app_name)) {
                msize=(sizeof(char *) * (*num_appusers+1));
  		Appusers = (AppuserItem **)realloc(Appusers,msize);
          	Appusers[*num_appusers]
            		= (AppuserItem *) malloc(sizeof(AppuserItem));
          	Appusers[*num_appusers]->marker = 1;
          	Appusers[*num_appusers]->progroup_title
            		= allocAndCopyString(record.field[0].data);
          	Appusers[*num_appusers]->login_name
            		= allocAndCopyString(record.field[1].data);
          	Appusers[*num_appusers]->real_name
            		= allocAndCopyString(record.field[2].data);
          	Appusers[*num_appusers]->app_data
            		= allocAndCopyString(record.field[3].data);
          	Appusers[*num_appusers]->app_passwd
            		= allocAndCopyString(record.field[4].data);
          	Appusers[*num_appusers]->app_role
            		= atoi(record.field[5].data);
          	Appusers[*num_appusers]->registered_user
            		= atoi(record.field[6].data);
          	(*num_appusers)++;
	    }
        }
    }

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Appusers);
}
/*****************************************************************************/
/* Writes all the non-changed database entries to a (temp) file. */
/* These entries are denoted by Appusers[i]->marker == 1 */

void  writeAppuserData(Appusers, num_appusers, fp)

     AppuserItem  **Appusers;
     int  num_appusers;
     FILE  *fp;

{
  int i;

  for (i = 0; i < num_appusers; i++)
    if (Appusers[i]->marker == 1)             /* ...is a no-change entry? */
      fprintf(fp, APPUSERS_FMT,
              Appusers[i]->progroup_title,
              Appusers[i]->login_name,
              Appusers[i]->real_name,
              Appusers[i]->app_data,
              Appusers[i]->app_passwd,
              Appusers[i]->app_role,
              Appusers[i]->registered_user);
}


/*****************************************************************************/

void  freeAppuserData(Appusers, num_appusers)

     AppuserItem  **Appusers;
     int  num_appusers;

{
  int i;
  
  for (i = num_appusers - 1; i >= 0; i--)
    {
      free(Appusers[i]->progroup_title);
      free(Appusers[i]->login_name);
      free(Appusers[i]->real_name);
      free(Appusers[i]->app_data);
      free(Appusers[i]->app_passwd);
      free((char *) Appusers[i]);
    }
  free((char *) Appusers);
}


/*****************************************************************************/

int  getBatchUser( const char  *progroup,
                   char        *batch_user,
                   const char  *dir )

{
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  int  found = 0;
  char decryptfile[MAX_FILENAME_LEN];
  
  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[APPUSERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[APPUSERS_ID]);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(0);
    }
  
  while (getRecord(&record, fp, APPUSERS_NUM_RECS) && !found)
    {
      found = ( (strcmp(progroup,record.field[0].data)==0) &&
                (atoi(record.field[5].data) & NTCSS_BATCH_USER_BIT) );
      if (found)
        strcpy(batch_user, record.field[1].data);
    }

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(found);
}


/*****************************************************************************/



PrtItem **getPrtData(num_prts, dir, Apps, num_apps)
int  *num_prts;
const char *dir;
AppItem **Apps;
int   num_apps;
{

  PrtItem **printerItems;
  int  nPrinters;

  /* Try to get printer info from nis and if not successful,
     then try to get the info from the data base */

  /* no nis for now 
  printerItems = getPrtDataFromNis(&nPrinters, Apps, num_apps);
  if (nPrinters >= 0) {
    *num_prts = nPrinters;
    return(printerItems);
  } 
  */

  printerItems = getPrtDataFromDb(&nPrinters, dir, Apps, num_apps);
  *num_prts = nPrinters;
  return(printerItems);

} 


/******************************************************************************/


PrtItem  **getPrtDataFromDb(num_prts, dir, Apps, num_apps)

     int  *num_prts;
     const char  *dir;
     AppItem  **Apps;
     int  num_apps;

{
  PrtItem  **Prts;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  int  i, j;
  char decryptfile[MAX_FILENAME_LEN];

  *num_prts = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PRINTERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PRINTERS_ID]);

  Prts = (PrtItem **) malloc(sizeof(char *)*MAX_NUM_PRINTERS);
  
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freePrtData(Prts, *num_prts);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PrtItem **) 0);
    }

  *num_prts = 0;
  while (getRecord(&record, fp, PRINTERS_NUM_RECS) &&
         *num_prts < MAX_NUM_PRINTERS)
    {
      Prts[*num_prts] = (PrtItem *) malloc(sizeof(PrtItem));
      Prts[*num_prts]->marker = 1;
      Prts[*num_prts]->prt_name = allocAndCopyString(record.field[0].data);
      Prts[*num_prts]->hostname = allocAndCopyString(record.field[1].data);
      Prts[*num_prts]->prt_location = allocAndCopyString(record.field[2].data);
      Prts[*num_prts]->max_size = atoi(record.field[3].data);
      Prts[*num_prts]->prt_class_title = allocAndCopyString(record.field[4].data);
      Prts[*num_prts]->ip_address = allocAndCopyString(record.field[5].data);
      Prts[*num_prts]->security_class = atoi(record.field[6].data);
      Prts[*num_prts]->status = atoi(record.field[7].data);
      Prts[*num_prts]->port_name = allocAndCopyString(record.field[8].data);
      Prts[*num_prts]->suspend_flag = atoi(record.field[9].data);
      Prts[*num_prts]->redirect_prt = allocAndCopyString(record.field[10].data);
      Prts[*num_prts]->FileName = allocAndCopyString(record.field[11].data); /*Atlas*/
      for (i = 0; i < MAX_NUM_PROGROUPS; i++)
        Prts[*num_prts]->prt_access[i] = 0;
      (*num_prts)++;
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  if (num_apps <= 0)
    return(Prts);

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PRINTER_ACCESS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PRINTER_ACCESS_ID]);
  
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freePrtData(Prts, *num_prts);
      *num_prts = -1;
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PrtItem **) 0);
    }

  while (getRecord(&record, fp, PRINTER_ACCESS_NUM_RECS))
    {
      for (i = 0; i < *num_prts; i++)
        {
/*
          if ((strcmp(record.field[0].data, Prts[i]->prt_name) == 0) &&
              (strcmp(record.field[1].data, Prts[i]->hostname) == 0))
*/
          if ((strcmp(record.field[0].data, Prts[i]->prt_name) == 0))
            {
              for (j = 0; j < num_apps; j++)
                {
                  if (strcmp(record.field[2].data,
                             Apps[j]->progroup_title) == 0)
                    {
                      Prts[i]->prt_access[j] = 1;
                      break;
                    }
                }
              break;
            }
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Prts);
}


/*****************************************************************************/

void  writePrtData(Prts, num_prts, fp)

     PrtItem  **Prts;
     int  num_prts;
     FILE  *fp;

{
  int  i;

  for (i = 0; i < num_prts; i++)
    if (Prts[i]->marker == 1)
      fprintf(fp, PRINTERS_FMT,
              Prts[i]->prt_name,
              Prts[i]->hostname,
              Prts[i]->prt_location,
              Prts[i]->max_size,
              Prts[i]->prt_class_title,
              Prts[i]->ip_address,
              Prts[i]->security_class,
              Prts[i]->status,
              Prts[i]->port_name,
              Prts[i]->suspend_flag,
              Prts[i]->redirect_prt,
              Prts[i]->FileName);       /* for Atlas */
}


/*****************************************************************************/

void  freePrtData(Prts, num_prts)

     PrtItem  **Prts;
     int  num_prts;

{
  int  i;

  for (i = num_prts - 1; i >= 0; i--)
    {
      free(Prts[i]->prt_name);
      free(Prts[i]->hostname);
      free(Prts[i]->prt_location);
      free(Prts[i]->prt_class_title);
      free(Prts[i]->ip_address);
      free(Prts[i]->port_name);
      free(Prts[i]->redirect_prt);
      /*Atlas*/
      free(Prts[i]->FileName);
      free((char *) Prts[i]);
    }
  free((char *) Prts);
}


/*****************************************************************************/

PrtItem  **getAppPrtData( int         *num_prts,
                          const char  *dir,
                          const char  *app )

{
  PrtItem  **AllPrts, **Prts;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  int  i, num_allprts;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_prts = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PRINTERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PRINTERS_ID]);

  num_allprts = 0;
  AllPrts = (PrtItem **) malloc(sizeof(char *)*MAX_NUM_PRINTERS);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppPrtData(AllPrts, num_allprts);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PrtItem **) 0);
    }

  while (getRecord(&record, fp, PRINTERS_NUM_RECS) &&
         num_allprts < MAX_NUM_PRINTERS)
    {
      AllPrts[num_allprts] = (PrtItem *) malloc(sizeof(PrtItem));
      AllPrts[num_allprts]->prt_name = allocAndCopyString(record.field[0].data);
      AllPrts[num_allprts]->hostname = allocAndCopyString(record.field[1].data);
      AllPrts[num_allprts]->prt_location = allocAndCopyString(record.field[2].data);
      AllPrts[num_allprts]->max_size = atoi(record.field[3].data);
      AllPrts[num_allprts]->prt_class_title = allocAndCopyString(record.field[4].data);
      AllPrts[num_allprts]->security_class = atoi(record.field[6].data);
      AllPrts[num_allprts]->status = atoi(record.field[7].data);
      AllPrts[num_allprts]->port_name =  allocAndCopyString(record.field[8].data);/*Atlas need port*/
      AllPrts[num_allprts]->suspend_flag = atoi(record.field[9].data);
      AllPrts[num_allprts]->redirect_prt = allocAndCopyString(record.field[10].data);
      AllPrts[num_allprts]->FileName = allocAndCopyString(record.field[11].data); /*Atlas*/
      num_allprts++;
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PRINTER_ACCESS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PRINTER_ACCESS_ID]);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppPrtData(AllPrts, num_allprts);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PrtItem **) 0);
    }

  Prts = (PrtItem **) malloc(sizeof(char *)*MAX_NUM_PRINTERS);

  *num_prts = 0;
  while (getRecord(&record, fp, PRINTER_ACCESS_NUM_RECS))
    {
      for (i = 0; i < num_allprts; i++)
        {
/*
          if ((strcmp(record.field[0].data, AllPrts[i]->prt_name) == 0) &&
              (strcmp(record.field[1].data, AllPrts[i]->hostname) == 0) &&
              (strcmp(record.field[2].data, app) == 0))
*/
          if ((strcmp(record.field[0].data, AllPrts[i]->prt_name) == 0) &&
              (strcmp(record.field[2].data, app) == 0))
            {
              Prts[*num_prts] = (PrtItem *) malloc(sizeof(PrtItem));
              Prts[*num_prts]->prt_name = allocAndCopyString(AllPrts[i]->prt_name);
              Prts[*num_prts]->hostname = allocAndCopyString(AllPrts[i]->hostname);
              Prts[*num_prts]->prt_location = allocAndCopyString(AllPrts[i]->prt_location);
              Prts[*num_prts]->max_size = AllPrts[i]->max_size;
              Prts[*num_prts]->prt_class_title = allocAndCopyString(AllPrts[i]->prt_class_title);
              Prts[*num_prts]->security_class = AllPrts[i]->security_class;
              Prts[*num_prts]->status = AllPrts[i]->status;
              Prts[*num_prts]->port_name = allocAndCopyString(AllPrts[i]->port_name);/*Atlas*/
              Prts[*num_prts]->suspend_flag = AllPrts[i]->suspend_flag;
              Prts[*num_prts]->redirect_prt = allocAndCopyString(AllPrts[i]->redirect_prt);
              Prts[*num_prts]->FileName  = allocAndCopyString(AllPrts[i]->FileName);/*Atlas*/
              (*num_prts)++;
              break;
            }
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  freeAppPrtData(AllPrts, num_allprts);

  return(Prts);
}


/*****************************************************************************/

void  freeAppPrtData(Prts, num_prts)

     PrtItem  **Prts;
     int  num_prts;

{
  int  i;

  for (i = num_prts - 1; i >= 0; i--)
    {
      free(Prts[i]->prt_name);
      free(Prts[i]->hostname);
      free(Prts[i]->prt_location);
      free(Prts[i]->prt_class_title);
      free(Prts[i]->redirect_prt);
      free(Prts[i]->port_name);      /*Atlas*/
      free(Prts[i]->FileName);      /*Atlas*/
      free((char *) Prts[i]);
    }
  free((char *) Prts);
}


/*****************************************************************************/

static int  changePrinterAccessData( int          mode,
                                     const char  *db_dir,
                                     const char  *prt_name,
                                     const char  *hostname,
                                     const char  *group_title )
{
    DBRecord   record;
    char   tmp_file[MAX_FILENAME_LEN];
    char   tempfile[MAX_FILENAME_LEN];
    FILE  *rp, *wp;
    char   decryptfile[MAX_FILENAME_LEN];

    if (mode == SQL_DEL_PRINTER_ACCESS) {
        if (using_tmp)
            sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[PRINTER_ACCESS_ID]);
        else
            sprintf(tmp_file, "%s/%s", db_dir, tables[PRINTER_ACCESS_ID]);
        rp = decryptAndOpen(tmp_file, decryptfile, "r");
        if (rp == NULL) {
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(0);
        }

        sprintf(tempfile, "%s/%s.temp", db_dir, tables[PRINTER_ACCESS_ID]);

        wp = fopen(tempfile, "w");
        if (wp == NULL) {
            fclose(rp);
            if (decryptfile[0] != '\0')
              remove(decryptfile);
            return(0);
        }

        while (getRecord(&record, rp, PRINTER_ACCESS_NUM_RECS)) {
            if (strcmp(record.field[0].data, prt_name)) {
                fprintf(wp, PRINTER_ACCESS_FMT,
                        record.field[0].data,
                        record.field[1].data,
                        record.field[2].data);
            }
        }
        closeAndEncrypt(wp, tempfile);
        fclose(rp);
        if (decryptfile[0] != '\0')
          remove(decryptfile);

        if (rename(tempfile, tmp_file) == -1)
            return(0);
    } else if (mode == SQL_ADD_PRINTER_ACCESS) {
        if (using_tmp)
            sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[PRINTER_ACCESS_ID]);
        else
            sprintf(tmp_file, "%s/%s", db_dir, tables[PRINTER_ACCESS_ID]);

        sprintf(tempfile, "%s/%s.temp", db_dir, tables[PRINTER_ACCESS_ID]);

        if ((rp = decryptAndOpen(tmp_file, decryptfile, "r")) == NULL) {
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(0);
        }

        if ((wp = fopen(tempfile, "w")) == NULL) {
            fclose(rp);
            if (decryptfile[0] != '\0')
              remove(decryptfile);
            return(0);
        }

        while (getRecord(&record, rp, PRINTER_ACCESS_NUM_RECS)) {
             fprintf(wp, PRINTER_ACCESS_FMT,
                        record.field[0].data,
                        record.field[1].data,
                        record.field[2].data);
        }
        fprintf(wp, PRINTER_ACCESS_FMT, prt_name, hostname, group_title);
        closeAndEncrypt(wp, tempfile);
        fclose(rp);
        if (decryptfile[0] != '\0')
          remove(decryptfile);

        if (rename(tempfile, tmp_file) == -1)
            return(0);
    }

    return(1);
}


/*****************************************************************************/

AdapterItem  **getAdapterDataFromDb( int         *num_adapters,
                                     const char  *dir )

{

  AdapterItem  **Adapters;
  DBRecord       record;
  char   datafile[MAX_FILENAME_LEN];
  char   decryptfile[MAX_FILENAME_LEN]; 
  FILE  *fp;

  *num_adapters = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[ADAPTERS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[ADAPTERS_ID]);

  Adapters = (AdapterItem **) malloc(sizeof(char *)*MAX_NUM_ADAPTERS);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAdapterData(Adapters, *num_adapters);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AdapterItem **) 0);
    }

  *num_adapters = 0;
  while (getRecord(&record, fp, ADAPTERS_NUM_RECS) &&
         *num_adapters < MAX_NUM_ADAPTERS) {
    Adapters[*num_adapters] = (AdapterItem *) malloc(sizeof(AdapterItem));
    Adapters[*num_adapters]->marker = 1;
    Adapters[*num_adapters]->adapter_type = allocAndCopyString(record.field[0].data);
    Adapters[*num_adapters]->machine_address = allocAndCopyString(record.field[1].data);
    Adapters[*num_adapters]->ip_address = allocAndCopyString(record.field[2].data);
    Adapters[*num_adapters]->location = allocAndCopyString(record.field[3].data);
    (*num_adapters)++;
  }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Adapters);
}


/*****************************************************************************/

AdapterItem  **getAdapterData( int         *num_adapters,
                               const char  *dir )

{
  AdapterItem  **Adapters;
  int   nAdapters;

  /* this is here to satifsy compiler warning about the parameter not being
     used. if the parameter is ever used in the future, the following will
     no longer be needed. */
  dir=dir;

  /* First try to get the data from NIS and if that fails
     then get the data from the local db file */

/*
  Adapters = getAdapterDataFromNis(&nAdapters);
  if (nAdapters < 0) {
    Adapters = getAdapterDataFromDb(&nAdapters);
  }
*/
  Adapters = getAdapterDataFromDb(&nAdapters, dir);

  *num_adapters = nAdapters;
  return(Adapters);

}


/*****************************************************************************/

static void writeAdapterData(Adapters, num_adapters, fp)
AdapterItem **Adapters;
int num_adapters;
FILE *fp;
{
  int i;

  for (i = 0; i < num_adapters; i++)
    if (Adapters[i]->marker == 1)
      fprintf(fp, ADAPTERS_FMT,
              Adapters[i]->adapter_type,
              Adapters[i]->machine_address,
              Adapters[i]->ip_address,
              Adapters[i]->location);
}


/*****************************************************************************/

void freeAdapterData(Adapters, num_adapters)
AdapterItem **Adapters;
int num_adapters;
{
    int i;

    for (i = num_adapters - 1; i >= 0; i--) {
        free(Adapters[i]->adapter_type);
        free(Adapters[i]->machine_address);
        free(Adapters[i]->ip_address);
        free(Adapters[i]->location);
        free((char *) Adapters[i]);
    }
    free((char *) Adapters);
}


/*****************************************************************************/

PrtclassItem  **getPrtclassData( int         *num_prtclasses,
                                 const char  *dir )

{
  PrtclassItem  **Prtclasses;
  DBRecord  record;
  char   datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char   decryptfile[MAX_FILENAME_LEN];
  
  *num_prtclasses = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PRT_CLASS_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PRT_CLASS_ID]);

    Prtclasses = (PrtclassItem **) malloc(sizeof(char *)*MAX_NUM_PRT_CLASS);
    fp = decryptAndOpen(datafile, decryptfile, "r");
    if (fp == NULL) {
        freePrtclassData(Prtclasses, *num_prtclasses);
        if (decryptfile[0] != '\0')
          remove(decryptfile);
        return((PrtclassItem **) 0);
    }

    *num_prtclasses = 0;
    while (getRecord(&record, fp, PRT_CLASS_NUM_RECS) &&
                                    *num_prtclasses < MAX_NUM_PRT_CLASS) {
        Prtclasses[*num_prtclasses] = (PrtclassItem *) malloc(sizeof(PrtclassItem));
        Prtclasses[*num_prtclasses]->marker = 1;
        Prtclasses[*num_prtclasses]->prt_class_title = allocAndCopyString(record.field[0].data);
        Prtclasses[*num_prtclasses]->prt_device_name = allocAndCopyString(record.field[1].data);
        Prtclasses[*num_prtclasses]->prt_driver_name = allocAndCopyString(record.field[2].data);
        (*num_prtclasses)++;
    }
    fclose(fp);
    if (decryptfile[0] != '\0')
      remove(decryptfile);

    return(Prtclasses);
}


/*****************************************************************************/

static void writePrtclassData(Prtclasses, num_prtclasses, fp)
PrtclassItem **Prtclasses;
int num_prtclasses;
FILE *fp;
{
  int i;
    
  for (i = 0; i < num_prtclasses; i++)
    if (Prtclasses[i]->marker == 1)
      fprintf(fp, PRT_CLASS_FMT,
              Prtclasses[i]->prt_class_title,
              Prtclasses[i]->prt_device_name,
              Prtclasses[i]->prt_driver_name);
}


/*****************************************************************************/

void freePrtclassData(Prtclasses, num_prtclasses)
PrtclassItem **Prtclasses;
int num_prtclasses;
{
  int i;

  for (i = num_prtclasses - 1; i >= 0; i--) {
    free(Prtclasses[i]->prt_class_title);
    free(Prtclasses[i]->prt_device_name);
    free(Prtclasses[i]->prt_driver_name);
    free((char *) Prtclasses[i]);
  }
  free((char *) Prtclasses);
}


/*****************************************************************************/

AdaptypeItem  **getAdaptypeData( int        *num_adaptypes,
                                 const char *file )

{
  AdaptypeItem **Adaptypes;
  DBRecord       record;
  char   decryptfile[MAX_FILENAME_LEN];
  FILE  *fp;
  int    i;
  char   str[MAX_ITEM_LEN];

  *num_adaptypes = -1;

  Adaptypes = (AdaptypeItem **) malloc(sizeof(char *)*MAX_NUM_ADAPTER_TYPES);
  fp = decryptAndOpen(file, decryptfile, "r");
  if (fp == NULL)
    {
      freeAdaptypeData(Adaptypes, *num_adaptypes);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AdaptypeItem **) 0);
    }
  
  *num_adaptypes = 0;
  while (getRecord(&record, fp, 3) && *num_adaptypes < MAX_NUM_ADAPTER_TYPES)
    {
      Adaptypes[*num_adaptypes] = (AdaptypeItem *) malloc(sizeof(AdaptypeItem));
      Adaptypes[*num_adaptypes]->type = allocAndCopyString(record.field[0].data);
      Adaptypes[*num_adaptypes]->num_ports = 0;
      i = 0;
      while (((i = stripWord(i, record.field[1].data, str)) != -1) &&
             Adaptypes[*num_adaptypes]->num_ports < NUM_PRT_PORTS)
        Adaptypes[*num_adaptypes]->port[Adaptypes[*num_adaptypes]->num_ports++]
          = allocAndCopyString(str);
      Adaptypes[*num_adaptypes]->setup_script
        = allocAndCopyString(record.field[2].data);
      (*num_adaptypes)++;
    }

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Adaptypes);
}


/*****************************************************************************/

void  freeAdaptypeData( AdaptypeItem **Adaptypes,
                        int            num_adaptypes )

{
    int i, j;

    for (i = num_adaptypes - 1; i >= 0; i--) {
        free(Adaptypes[i]->type);
        for (j = Adaptypes[i]->num_ports - 1; j >= 0; j--)
            free(Adaptypes[i]->port[j]);
        free(Adaptypes[i]->setup_script);
        free((char *) Adaptypes[i]);
    }
    free((char *) Adaptypes);
}


/*****************************************************************************/

OutputTypeItem  **getOutputTypeData( int        *num_otypes,
                                     const char *dir )
{
    OutputTypeItem **Otypes;
    DBRecord record;
    char datafile[MAX_FILENAME_LEN];
    FILE *fp;
    char decryptfile[MAX_FILENAME_LEN];

    *num_otypes = -1;

    if (using_tmp)
        sprintf(datafile, "%s/%s.tmp", dir, tables[OUTPUT_TYPE_ID]);
    else
        sprintf(datafile, "%s/%s", dir, tables[OUTPUT_TYPE_ID]);

    Otypes = (OutputTypeItem **) malloc(sizeof(char *)*MAX_NUM_OUTPUT_TYPE);
    fp = decryptAndOpen(datafile, decryptfile, "r");
    if (fp == NULL) {
        freeOutputTypeData(Otypes, *num_otypes);
        if (decryptfile[0] != '\0')
          remove(decryptfile);
        return((OutputTypeItem **) 0);
    }

    *num_otypes = 0;
    while (getRecord(&record, fp, OUTPUT_TYPE_NUM_RECS) &&
                                    *num_otypes < MAX_NUM_OUTPUT_TYPE) {
        Otypes[*num_otypes] = (OutputTypeItem *) malloc(sizeof(OutputTypeItem));
        Otypes[*num_otypes]->marker = 1;
        Otypes[*num_otypes]->out_type_title = allocAndCopyString(record.field[0].data);
        Otypes[*num_otypes]->progroup_title = allocAndCopyString(record.field[1].data);
        Otypes[*num_otypes]->description = allocAndCopyString(record.field[2].data);
        Otypes[*num_otypes]->default_prt = allocAndCopyString(record.field[3].data);
        Otypes[*num_otypes]->batch_flag = atoi(record.field[4].data);
        (*num_otypes)++;
    }
    fclose(fp);
    if (decryptfile[0] != '\0')
      remove(decryptfile);

    return(Otypes);
}


/*****************************************************************************/

static void writeOutputTypeData(Otypes, num_otypes, fp)
OutputTypeItem **Otypes;
int num_otypes;
FILE *fp;
{
    int i;

    for (i = 0; i < num_otypes; i++)
        if (Otypes[i]->marker == 1)
            fprintf(fp, OUTPUT_TYPE_FMT,
                        Otypes[i]->out_type_title,
                        Otypes[i]->progroup_title,
                        Otypes[i]->description,
                        Otypes[i]->default_prt,
                        Otypes[i]->batch_flag);
}


/*****************************************************************************/

void freeOutputTypeData(Otypes, num_otypes)
OutputTypeItem **Otypes;
int num_otypes;
{
    int i;

    for (i = num_otypes - 1; i >= 0; i--) {
        free(Otypes[i]->out_type_title);
        free(Otypes[i]->progroup_title);
        free(Otypes[i]->description);
        free(Otypes[i]->default_prt);
        free((char *) Otypes[i]);
    }
    free((char *) Otypes);
}


/*****************************************************************************/

OutputTypeItem  **getAppOutputTypeData( int        *num_otypes,
                                        const char *dir,
                                        const char *app,
                                        PrtItem    *Prts[],
                                        int         num_prts )

{
    OutputTypeItem **Otypes;
    DBRecord record;
    char datafile[MAX_FILENAME_LEN];
    FILE *fp;
    int i, j;
    char decryptfile[MAX_FILENAME_LEN];

    *num_otypes = -1;

    if (using_tmp)
        sprintf(datafile, "%s/%s.tmp", dir, tables[OUTPUT_TYPE_ID]);
    else
        sprintf(datafile, "%s/%s", dir, tables[OUTPUT_TYPE_ID]);

    Otypes = (OutputTypeItem **) malloc(sizeof(char *)*MAX_NUM_OUTPUT_TYPE);
    fp = decryptAndOpen(datafile, decryptfile, "r");
    if (fp == NULL) {
        freeAppOutputTypeData(Otypes, *num_otypes);
        if (decryptfile[0] != '\0')
          remove(decryptfile);
        return((OutputTypeItem **) 0);
    }

    *num_otypes = 0;
    while (getRecord(&record, fp, OUTPUT_TYPE_NUM_RECS) &&
                                    *num_otypes < MAX_NUM_OUTPUT_TYPE) {
        if (strcmp(app, record.field[1].data) == 0) {
            Otypes[*num_otypes] = (OutputTypeItem *) malloc(sizeof(OutputTypeItem));
            Otypes[*num_otypes]->marker = 1;
            Otypes[*num_otypes]->out_type_title = allocAndCopyString(record.field[0].data);
            Otypes[*num_otypes]->description = allocAndCopyString(record.field[2].data);
            Otypes[*num_otypes]->default_prt = allocAndCopyString(record.field[3].data);
            Otypes[*num_otypes]->batch_flag = atoi(record.field[4].data);
            for (i = 0; i < MAX_NUM_PRT_CLASS; i++)
                Otypes[*num_otypes]->output_prt[i] = 0;
            (*num_otypes)++;
        }
    }
    fclose(fp);
    if (decryptfile[0] != '\0')
      remove(decryptfile);

    if (num_prts <= 0)
        return(Otypes);

    if (using_tmp)
        sprintf(datafile, "%s/%s.tmp", dir, tables[OUTPUT_PRT_ID]);
    else
        sprintf(datafile, "%s/%s", dir, tables[OUTPUT_PRT_ID]);

    fp = decryptAndOpen(datafile, decryptfile, "r");
    if (fp == NULL) {
        freeAppOutputTypeData(Otypes, *num_otypes);
        if (decryptfile[0] != '\0')
          remove(decryptfile);
        return((OutputTypeItem **) 0);
    }

    while (getRecord(&record, fp, OUTPUT_PRT_NUM_RECS)) {
        for (i = 0; i < *num_otypes; i++) {
            if ((strcmp(record.field[0].data,
                                Otypes[i]->out_type_title) == 0) &&
                (strcmp(record.field[1].data, app) == 0)) {
                for (j = 0; j < num_prts; j++) {
                    if ((strcmp(record.field[2].data,
                                    Prts[j]->prt_name) == 0)) {
                        Otypes[i]->output_prt[j] = 1;
                        break;
                    }
                }
                break;
            }
        }
    }
    fclose(fp);
    if (decryptfile[0] != '\0')
      remove(decryptfile);

    return(Otypes);
}


/*****************************************************************************/

void  freeAppOutputTypeData( OutputTypeItem **Otypes,
                             int              num_otypes )
{
  int  i;

  for (i = num_otypes - 1; i >= 0; i--)
    {
      free(Otypes[i]->out_type_title);
      free(Otypes[i]->description);
      free(Otypes[i]->default_prt);
      free((char *) Otypes[i]);
    }
  free((char *) Otypes);
}


/*****************************************************************************/

static int  changeOutputPrtData( int          mode,
                                 const char  *db_dir,
                                 const char  *prtname,
                                 const char  *prthost,
                                 const char  *otype,
                                 const char  *group )
{
  char   decryptfile[MAX_FILENAME_LEN];
  char   tmp_file[MAX_FILENAME_LEN];
  char   tempfile[MAX_FILENAME_LEN];
  FILE  *rp;
  FILE  *wp;
  DBRecord   record;

  if (mode == SQL_DEL_OUTPUT_PRT)
    {
      if (using_tmp)
        sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[OUTPUT_PRT_ID]);
      else
        sprintf(tmp_file, "%s/%s", db_dir, tables[OUTPUT_PRT_ID]);
      
      rp = decryptAndOpen(tmp_file, decryptfile, "r");
      if (rp == NULL)
        {
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(0);
        }

      sprintf(tempfile, "%s/%s.temp", db_dir, tables[OUTPUT_PRT_ID]);
      wp = fopen(tempfile, "w");
      if (wp == NULL)
        {
          fclose(rp);
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(0);
        }

      while (getRecord(&record, rp, OUTPUT_PRT_NUM_RECS))
        {
          if (!((strcmp(record.field[0].data, otype) == 0) &&
                (strcmp(record.field[1].data, group) == 0))) {
            fprintf(wp, OUTPUT_PRT_FMT,
                    record.field[0].data,
                    record.field[1].data,
                    record.field[2].data,
                    record.field[3].data);
          }
        }
      closeAndEncrypt(wp, tempfile);
      fclose(rp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);

      if (rename(tempfile, tmp_file) == -1)
        return(0);
    }
  else
    if (mode == SQL_ADD_OUTPUT_PRT)
      {
        if (using_tmp)
          sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[OUTPUT_PRT_ID]);
        else
          sprintf(tmp_file, "%s/%s", db_dir, tables[OUTPUT_PRT_ID]);
        rp = decryptAndOpen(tmp_file, decryptfile, "r");
        if (rp == NULL)
          {
            if (decryptfile[0] != '\0')
              remove(decryptfile);
            return(0);
          }

        sprintf(tempfile, "%s/%s.temp", db_dir, tables[OUTPUT_PRT_ID]);
        wp = fopen(tempfile, "w");
        if (wp == NULL)
          {
            fclose(rp);
            if (decryptfile[0] != '\0')
              remove(decryptfile);
            return(0);
          }

        while (getRecord(&record, rp, OUTPUT_PRT_NUM_RECS))
          {
            fprintf(wp, OUTPUT_PRT_FMT,
                    record.field[0].data,
                    record.field[1].data,
                    record.field[2].data,
                    record.field[3].data);
          }
        fprintf(wp, OUTPUT_PRT_FMT, otype, group, prtname, prthost);
        closeAndEncrypt(wp, tempfile);
        fclose(rp);
        if (decryptfile[0] != '\0')
          remove(decryptfile);

        if (rename(tempfile, tmp_file) == -1)
          return(0);
        
      }

  return(1);
}


/*****************************************************************************/

PredefinedJobItem  **getPredefinedJobData( int         *num_pdjs,
                                           const char  *dir )

{
  PredefinedJobItem  **Pdjs;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_pdjs = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PREDEFINED_JOB_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PREDEFINED_JOB_ID]);

  Pdjs = (PredefinedJobItem **) malloc(sizeof(char *)*MAX_NUM_PREDEFINED_JOB);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freePredefinedJobData(Pdjs, *num_pdjs);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PredefinedJobItem **) 0);
    }

  *num_pdjs = 0;
  while (getRecord(&record, fp, PREDEFINED_JOB_NUM_RECS) &&
         *num_pdjs < MAX_NUM_PREDEFINED_JOB)
    {
      Pdjs[*num_pdjs] = (PredefinedJobItem *) malloc(sizeof(PredefinedJobItem));
      Pdjs[*num_pdjs]->marker = 1;
      Pdjs[*num_pdjs]->job_title = allocAndCopyString(record.field[0].data);
      Pdjs[*num_pdjs]->progroup_title = allocAndCopyString(record.field[1].data);
      Pdjs[*num_pdjs]->command_line = allocAndCopyString(record.field[2].data);
      Pdjs[*num_pdjs]->role_access = atoi(record.field[3].data);
      Pdjs[*num_pdjs]->flag = atoi(record.field[4].data);
      Pdjs[*num_pdjs]->schedule_str = allocAndCopyString(record.field[5].data);
      (*num_pdjs)++;
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Pdjs);
}


/*****************************************************************************/

static void writePredefinedJobData(Pdjs, num_pdjs, fp)
PredefinedJobItem **Pdjs;
int num_pdjs;
FILE *fp;
{
    int i;

    for (i = 0; i < num_pdjs; i++)
        if (Pdjs[i]->marker == 1)
            fprintf(fp, PREDEFINED_JOB_FMT,
                        Pdjs[i]->job_title,
                        Pdjs[i]->progroup_title,
                        Pdjs[i]->command_line,
                        Pdjs[i]->role_access,
                        Pdjs[i]->flag,
                        Pdjs[i]->schedule_str);
}


/*****************************************************************************/

void freePredefinedJobData(Pdjs, num_pdjs)
PredefinedJobItem **Pdjs;
int num_pdjs;
{
    int i;

    for (i = num_pdjs - 1; i >= 0; i--) {
        free(Pdjs[i]->job_title);
        free(Pdjs[i]->progroup_title);
        free(Pdjs[i]->command_line);
        free(Pdjs[i]->schedule_str);
        free((char *) Pdjs[i]);
    }
    free((char *) Pdjs);
}


/*****************************************************************************/

PredefinedJobItem  **getAppPredefinedJobDataByRole( int         *num_pdjs,
                                                    const char  *dir,
                                                    const char  *app,
                                                    int          role )

{
  PredefinedJobItem  **Pdjs;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_pdjs = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PREDEFINED_JOB_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PREDEFINED_JOB_ID]);

  Pdjs = (PredefinedJobItem **) malloc(sizeof(char *)*MAX_NUM_PREDEFINED_JOB);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppPredefinedJobDataByRole(Pdjs, *num_pdjs);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PredefinedJobItem **) 0);
    }

  *num_pdjs = 0;

  while (getRecord(&record, fp, PREDEFINED_JOB_NUM_RECS) &&
         *num_pdjs < MAX_NUM_PREDEFINED_JOB)
    {

      /* Only want regular PDJ's, no boot jobs or scheduled jobs. */
      if (strcmp(app, record.field[1].data) == 0 &&  /* progroup title field */
          role & atoi(record.field[3].data) &&      /* role access field */
          atoi(record.field[4].data) == 0)         /* PDJ type bitmask field */
        {
          Pdjs[*num_pdjs] = (PredefinedJobItem *)
            malloc(sizeof(PredefinedJobItem));
          Pdjs[*num_pdjs]->marker = 1;
          Pdjs[*num_pdjs]->job_title = allocAndCopyString(record.field[0].data);
          Pdjs[*num_pdjs]->progroup_title = allocAndCopyString(record.field[1].data);
          Pdjs[*num_pdjs]->command_line = allocAndCopyString(record.field[2].data);
          Pdjs[*num_pdjs]->role_access = atoi(record.field[3].data);
          Pdjs[*num_pdjs]->flag = atoi(record.field[4].data);
          Pdjs[*num_pdjs]->schedule_str = allocAndCopyString(record.field[5].data);
          (*num_pdjs)++;
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Pdjs);
}


/*****************************************************************************/

void freeAppPredefinedJobDataByRole(Pdjs, num_pdjs)
PredefinedJobItem **Pdjs;
int num_pdjs;
{
    int i;

    for (i = num_pdjs - 1; i >= 0; i--) {
        free(Pdjs[i]->job_title);
        free(Pdjs[i]->progroup_title);
        free(Pdjs[i]->command_line);
        free(Pdjs[i]->schedule_str);
        free((char *) Pdjs[i]);
    }
    free((char *) Pdjs);
}


/*****************************************************************************/

PredefinedJobItem  **getAppPredefinedJobData( int         *num_pdjs,
                                              const char  *dir,
                                              const char  *app )

{
  PredefinedJobItem  **Pdjs;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_pdjs = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", dir, tables[PREDEFINED_JOB_ID]);
  else
    sprintf(datafile, "%s/%s", dir, tables[PREDEFINED_JOB_ID]);

  Pdjs = (PredefinedJobItem **) malloc(sizeof(char *)*MAX_NUM_PREDEFINED_JOB);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeAppPredefinedJobData(Pdjs, *num_pdjs);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PredefinedJobItem **) 0);
    }
  
  *num_pdjs = 0;
  while (getRecord(&record, fp, PREDEFINED_JOB_NUM_RECS) &&
         *num_pdjs < MAX_NUM_PREDEFINED_JOB)
    {
      if (strcmp(app, record.field[1].data) == 0)
        {
          Pdjs[*num_pdjs] = (PredefinedJobItem *)
            malloc(sizeof(PredefinedJobItem));
          Pdjs[*num_pdjs]->marker = 1;
          Pdjs[*num_pdjs]->job_title = allocAndCopyString(record.field[0].data);
          Pdjs[*num_pdjs]->progroup_title = allocAndCopyString(record.field[1].data);
          Pdjs[*num_pdjs]->command_line = allocAndCopyString(record.field[2].data);
          Pdjs[*num_pdjs]->role_access = atoi(record.field[3].data);
          Pdjs[*num_pdjs]->flag = atoi(record.field[4].data);
          Pdjs[*num_pdjs]->schedule_str = allocAndCopyString(record.field[5].data);
          (*num_pdjs)++;
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(Pdjs);
}


/*****************************************************************************/

void freeAppPredefinedJobData(Pdjs, num_pdjs)
PredefinedJobItem **Pdjs;
int num_pdjs;
{
    int i;

    for (i = num_pdjs - 1; i >= 0; i--) {
        free(Pdjs[i]->job_title);
        free(Pdjs[i]->progroup_title);
        free(Pdjs[i]->command_line);
        free(Pdjs[i]->schedule_str);
        free((char *) Pdjs[i]);
    }
    free((char *) Pdjs);
}


/*****************************************************************************/

void  get_progroup_titles_for_local_host_and_app_servers(
                                           const char  *db_dir,
                                           char       **progroup_titles, 
                                           int         *progroup_title_count )
{
  AppItem  **Apps;
  const char *fnct_name = 
                    "get_progroup_titles_for_local_host_and_app_servers():";
  char   strThisHostName[MAX_HOST_NAME_LEN + 1];
  int    num_apps;
  int    i;
  int    nHostType;

  *progroup_title_count = -1;

  memset(strThisHostName,  NTCSS_NULLCH, sizeof(strThisHostName));
  if (gethostname(strThisHostName, MAX_HOST_NAME_LEN) != 0)
    {
      syslog(LOG_WARNING, " %s gethostname failed!", fnct_name);
    }

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  *progroup_title_count = 0;

  for (i = 0; i < num_apps; i++)
    {
      nHostType = getHostType(Apps[i]->hostname, NTCSS_DB_DIR);
      if (nHostType < 0)
        {
          syslog(LOG_WARNING, "%s getHostType failed for %s!", fnct_name,
                 Apps[i]->hostname);
        }

      if ((strcmp(Apps[i]->hostname, strThisHostName) == 0) ||
          (nHostType == NTCSS_APP_SERVER))
        {
          progroup_titles[*progroup_title_count] =
            allocAndCopyString(Apps[i]->progroup_title);
          syslog(LOG_WARNING, "%s Progroup_titles[%i] == %s", fnct_name,
                 *progroup_title_count,
                 progroup_titles[*progroup_title_count]);
          (*progroup_title_count)++;
        }
    }         /* END for */
  
  freeAppData(Apps,num_apps);
}

/*****************************************************************************/

static void  get_progroup_titles_for_host_name( const char  *db_dir,
                                                const char  *host_name,
                                                char       **progroup_titles, 
                                                int     *progroup_title_count )
{
  AppItem  **Apps;
  int  num_apps;
  int  i;

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  *progroup_title_count = 0;

  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(Apps[i]->hostname, host_name) == 0)
        {
          progroup_titles[*progroup_title_count] =
            allocAndCopyString(Apps[i]->progroup_title);
          (*progroup_title_count)++;
        }
    }                    
  
  freeAppData(Apps, num_apps);
}


/*****************************************************************************/

static int appropriate_boot_job( char        *progroup_titles[],
                                 int          progroup_title_count, 
                                 const char  *progroup_title, 
                                 int          boot_job)
{
  int  i;

  if (!boot_job) return(0);

  for (i = 0; i < progroup_title_count; i++)
    if (strcmp(progroup_title, progroup_titles[i]) == 0)
      return(1);

  return(0);
}


/*****************************************************************************/

PredefinedJobItem  **getServerBootJobs( int         *num_pdjs,
                                        const char  *db_dir,
                                        const char  *host_name )

{

  PredefinedJobItem  **Pdjs;
  PredefinedJobItem  **PdjsNew;
  DBRecord  record;
  FILE  *fp;
  char  datafile[MAX_FILENAME_LEN];
  char  *progroup_titles[MAX_NUM_PROGROUPS];
  int  progroup_title_count;
  int  i;
  int  num_new_pdjs;
  char decryptfile[MAX_FILENAME_LEN];

  *num_pdjs = -1;

  if (using_tmp)
    sprintf(datafile, "%s/%s.tmp", db_dir, tables[PREDEFINED_JOB_ID]);
  else
    sprintf(datafile, "%s/%s", db_dir, tables[PREDEFINED_JOB_ID]);

  get_progroup_titles_for_host_name(db_dir, host_name, progroup_titles,
                                    &progroup_title_count );

  Pdjs = (PredefinedJobItem **) malloc(sizeof(char *)*MAX_NUM_PREDEFINED_JOB);
  PdjsNew = (PredefinedJobItem **) malloc(sizeof(char *)*MAX_NUM_PREDEFINED_JOB);

  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      freeServerBootJobs(Pdjs, *num_pdjs);
      freeServerBootJobs(PdjsNew, *num_pdjs);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((PredefinedJobItem **) 0);
    }

  *num_pdjs = 0;
  num_new_pdjs = 0;

  while (getRecord(&record, fp, PREDEFINED_JOB_NUM_RECS) &&
         *num_pdjs < MAX_NUM_PREDEFINED_JOB)
    {
      PdjsNew[num_new_pdjs] = (PredefinedJobItem *) malloc(sizeof(PredefinedJobItem));
      PdjsNew[num_new_pdjs]->marker = 1;
      PdjsNew[num_new_pdjs]->job_title = allocAndCopyString(record.field[0].data);
      PdjsNew[num_new_pdjs]->progroup_title = allocAndCopyString(record.field[1].data);
      PdjsNew[num_new_pdjs]->command_line = allocAndCopyString(record.field[2].data);
      PdjsNew[num_new_pdjs]->role_access = atoi(record.field[3].data);
      PdjsNew[num_new_pdjs]->flag = atoi(record.field[4].data);
      PdjsNew[num_new_pdjs]->schedule_str = allocAndCopyString(record.field[5].data);

      if (appropriate_boot_job(progroup_titles, progroup_title_count, 
                               record.field[1].data,
                               atoi(record.field[4].data)))
        {
          Pdjs[*num_pdjs] = (PredefinedJobItem *) malloc(sizeof(PredefinedJobItem));
          Pdjs[*num_pdjs]->marker = 1;
          Pdjs[*num_pdjs]->job_title = allocAndCopyString(record.field[0].data);
          Pdjs[*num_pdjs]->progroup_title = allocAndCopyString(record.field[1].data);
          Pdjs[*num_pdjs]->command_line = allocAndCopyString(record.field[2].data);
          Pdjs[*num_pdjs]->role_access = atoi(record.field[3].data);
          Pdjs[*num_pdjs]->flag = atoi(record.field[4].data);
          Pdjs[*num_pdjs]->schedule_str = allocAndCopyString(record.field[5].data);

          if (strlen(Pdjs[*num_pdjs]->schedule_str))
            PdjsNew[num_new_pdjs]->flag = 0;

          (*num_pdjs)++;
        }
      num_new_pdjs++;
    }

  for (i = 0; i < progroup_title_count; i++)
    free(progroup_titles[i]);

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      freeServerBootJobs(PdjsNew, num_new_pdjs);
    }
  else
    {
      writePredefinedJobData(PdjsNew, num_new_pdjs, fp);
      freeServerBootJobs(PdjsNew, num_new_pdjs);
      closeAndEncrypt(fp, datafile);
    }

  return(Pdjs);
}


/*****************************************************************************/

void freeServerBootJobs(Pdjs, num_pdjs)
PredefinedJobItem **Pdjs;
int num_pdjs;
{
  int i;

  for (i = num_pdjs - 1; i >= 0; i--) {
    free(Pdjs[i]->job_title);
    free(Pdjs[i]->progroup_title);
    free(Pdjs[i]->command_line);
    free(Pdjs[i]->schedule_str);
    free((char *) Pdjs[i]);
  }
  free((char *) Pdjs);
}


/*****************************************************************************/

int  getHostType( char        *strHostName,
                  const char  *strDbDir )

{
  HostItem  **hosts;
  HostItem hostItem;
  int  nHosts;
  int  i;
  int  nReturn;
  
  /* If getNtcssHostItemByNameFromNis failed, then call getHostData
     which will first try to get the data from NIS and then go to
     the hosts file in the database for the information */

  if (getNtcssHostItemByNameFromNis(&hostItem, strHostName) == 0) {
    nReturn = hostItem.type;

    free(hostItem.hostname);
    free(hostItem.ip_address);
    
    return(nReturn);
  } /* END if */

  hosts = getHostData(&nHosts, strDbDir);
  if (hosts == NULL)
    {
      syslog(LOG_WARNING, "getHostType: getHostData failed");
      return(-1);    
    } /* END if */
  
  nReturn = -2;

  for (i = 0; i < nHosts; i++)
    {
      if (strcmp(strHostName, hosts[i]->hostname) == 0)
        {
          nReturn = hosts[i]->type;
          break;
        } /* END if */
  } /* END for */

  freeHostData(hosts, nHosts);

  return(nReturn);
}


/*****************************************************************************/
HostItem **getHostData(num_hosts, dir)
int *num_hosts;
const char *dir;
{
    HostItem **Hosts;
    DBRecord record;
    char datafile[MAX_FILENAME_LEN];
    FILE *fp;
    char decryptfile[MAX_FILENAME_LEN];

    Hosts = getHostDataFromNis(num_hosts);
    /* If the NIS function was successful, return it's results 
       otherwise read host data from the file */

    /* TONY, changed from >= 0 ,12/12/00 */
    if (*num_hosts > 0) {
      return(Hosts);
    } /* END if */
    
    *num_hosts = -1;

    if (using_tmp)
        sprintf(datafile, "%s/%s.tmp", dir, tables[HOSTS_ID]);
    else
        sprintf(datafile, "%s/%s", dir, tables[HOSTS_ID]);

    Hosts = (HostItem **) malloc(sizeof(char *)*MAX_NUM_HOSTS);
    fp = decryptAndOpen(datafile, decryptfile, "r");
    if (fp == NULL) {
        freeHostData(Hosts, *num_hosts);
        if (decryptfile[0] != '\0')
          remove(decryptfile);
        return((HostItem **) 0);
    }

    *num_hosts = 0;
    while (getRecord(&record, fp, HOSTS_NUM_RECS) &&
                                        *num_hosts < MAX_NUM_HOSTS) {
printf("Hostname = <%s>\n",record.field[0].data);
fflush(stdout);
        Hosts[*num_hosts] = (HostItem *) malloc(sizeof(HostItem));
        Hosts[*num_hosts]->marker = 1;
        Hosts[*num_hosts]->hostname = allocAndCopyString(record.field[0].data);
        Hosts[*num_hosts]->ip_address = allocAndCopyString(record.field[1].data);
        Hosts[*num_hosts]->num_processes = atoi(record.field[2].data);
    Hosts[*num_hosts]->type = atoi(record.field[3].data);
    Hosts[*num_hosts]->repl = atoi(record.field[4].data);
        (*num_hosts)++;
    }
    fclose(fp);
    if (decryptfile[0] != '\0')
      remove(decryptfile);

    return(Hosts);
}


/*****************************************************************************/

void writeHostData(Hosts, num_hosts, fp)
HostItem **Hosts;
int num_hosts;
FILE *fp;
{
    int i;

    for (i = 0; i < num_hosts; i++)
        if (Hosts[i]->marker == 1)
            fprintf(fp, HOSTS_FMT,
                        Hosts[i]->hostname,
                        Hosts[i]->ip_address,
                        Hosts[i]->num_processes,
                Hosts[i]->type,Hosts[i]->repl);

    /* if (buildAndPushNtcssNisMaps() != 0) {
       syslog(LOG_WARNING, "writeHostData: buildAndPushNtcssNisMaps failed");
    } */ /* END if */
}


/*****************************************************************************/

void freeHostData(Hosts, num_hosts)
HostItem **Hosts;
int num_hosts;
{
    int i;

    for (i = num_hosts - 1; i >= 0; i--) {
        free(Hosts[i]->hostname);
        free(Hosts[i]->ip_address);
        free((char *) Hosts[i]);
    }
    free((char *) Hosts);
}


/*****************************************************************************/

AppItem  **getCurrentAppData(num_apps, dir)

     int  *num_apps;
     const char  *dir;
{
  AppItem  **Apps;
  DBRecord  record;
  char  datafile[MAX_FILENAME_LEN];
  int   nNtcssGroupIndex;
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  *num_apps = -1;

  if (using_tmp) {
    sprintf(datafile, "%s/%s.tmp", dir, tables[CURRENT_APPS_ID]);
  } else {
    sprintf(datafile, "%s/%s", dir, tables[CURRENT_APPS_ID]);
  } /* END if */

  Apps = (AppItem **)malloc(sizeof(char *) * MAX_NUM_PROGROUPS);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL) {
    freeAppData(Apps, *num_apps);
    if (decryptfile[0] != '\0')
      remove(decryptfile);
    return((AppItem **) 0);
  } /* END if */
  
  nNtcssGroupIndex = -1;
  *num_apps = 0;
  while (getRecord(&record, fp, CURRENT_APPS_NUM_RECS) &&
         *num_apps < MAX_NUM_PROGROUPS) {
    Apps[*num_apps] = (AppItem *) malloc(sizeof(AppItem));
    Apps[*num_apps]->marker         = 1;
    Apps[*num_apps]->progroup_title = allocAndCopyString(record.field[0].data);
    Apps[*num_apps]->hostname       = allocAndCopyString(record.field[1].data);
    Apps[*num_apps]->num_roles      = atoi(record.field[2].data);
    Apps[*num_apps]->icon_index     = atoi(record.field[3].data);
    (*num_apps)++;
  } /* END while */

  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  
  return(Apps);
}

/*****************************************************************************/

void writeCurrentAppData(CurrentAppData, num_current_app_data, fp)
AppItem **CurrentAppData;
int num_current_app_data;
FILE *fp;
{
   int i;

   for (i = 0; i < num_current_app_data; i++)
      if (CurrentAppData[i]->marker == 1)
         fprintf(fp, CURRENT_APPS_FMT,
                 CurrentAppData[i]->progroup_title,
                 CurrentAppData[i]->hostname,
                 CurrentAppData[i]->num_roles,
                 CurrentAppData[i]->icon_index);
}

/*****************************************************************************/

void  freeCurrentAppData(Apps, num_apps)

     AppItem  **Apps;
     int  num_apps;

{
  int  i;

  for (i = num_apps - 1; i >= 0; i--)
    {
      free(Apps[i]->progroup_title);
      free(Apps[i]->hostname);
      free((char *) Apps[i]);
    }
  free((char *) Apps);
}


/**************

void  incrementCurrentAppDataCount( const char *strApp )

{
   AppItem     **appItems;
   FILE *fp;
   int nAppItems = 0;
   int i;
   char fileName[MAX_PATH_LEN];
   char realFileName[MAX_PATH_LEN];
   int nWriteFile = 0;

   appItems = getCurrentAppData(&nAppItems, NTCSS_DB_DIR);
   for (i = 0; i < nAppItems; i++)
   {
      appItems[i]->marker = 1;

      if (strcmp(appItems[i]->progroup_title, strApp) == 0)
      {
         appItems[i]->num_roles += 1;
         nWriteFile = 1;
         break;
      }
   }

   if (nWriteFile == 1)
   {
      sprintf(fileName,"%s/%s.tmp", NTCSS_DB_DIR, tables[CURRENT_APPS_ID]);
      sprintf(realFileName,"%s/%s", NTCSS_DB_DIR, tables[CURRENT_APPS_ID]);

      fp = fopen(fileName, "w");
      if (fp == NULL)
      {
         freeCurrentAppData(appItems, nAppItems);
         return;
      }
      writeCurrentAppData(appItems, nAppItems, fp);
      closeAndEncrypt(fp, fileName);
      (void) rename(fileName, realFileName);
      (void) chmod(realFileName, S_IRUSR | S_IWUSR);

   }

   freeCurrentAppData(appItems, nAppItems);

}


void  decrementCurrentAppDataCount( const char *strApp )

{
AppItem     **appItems;
FILE *fp;
int nAppItems = 0;
int i;
char fileName[MAX_PATH_LEN];
char realFileName[MAX_PATH_LEN];
int nWriteFile = 0;

   appItems = getCurrentAppData(&nAppItems, NTCSS_DB_DIR);
   for (i = 0; i < nAppItems; i++)
   {
      appItems[i]->marker = 1;
                                 
      if (strcmp(appItems[i]->progroup_title, strApp) == 0)
      {
         if (appItems[i]->num_roles > 0)
         {
            appItems[i]->num_roles -= 1;
            nWriteFile = 1;
         }
      }
   }

   if (nWriteFile == 1)
   {
      sprintf(fileName,"%s/%s.tmp", NTCSS_DB_DIR, tables[CURRENT_APPS_ID]);
      sprintf(realFileName,"%s/%s", NTCSS_DB_DIR, tables[CURRENT_APPS_ID]);

      fp = fopen(fileName, "w");
      if (fp == NULL)
      {
         freeCurrentAppData(appItems, nAppItems);
         return;
      }
      writeCurrentAppData(appItems, nAppItems, fp);
      closeAndEncrypt(fp, fileName);
      (void) rename(fileName, realFileName);
      (void) chmod(realFileName, S_IRUSR | S_IWUSR);

   }

   freeCurrentAppData(appItems, nAppItems);
}

*************************************************/

void  incrementCurrentAppsFromList( const char  *strAppList, const char *dbdir)

{
AppItem     **appItems;
FILE *fp;
int nAppItems = 0;
int i;
char fileName[MAX_PATH_LEN];
char realFileName[MAX_PATH_LEN];
int nWriteFile = 0;

int nCharIndex;
char strOneApp[MAX_PROGRAM_TITLE_LEN+1];

   appItems = getCurrentAppData(&nAppItems,dbdir);

   nCharIndex = 0;
   nCharIndex = parseWord(0, strAppList, strOneApp, LIST_SEP_CHARACTER);

   /* Loop through each access role */
   while (nCharIndex >= 0) 
   {
      WriteProfileString("APPS",strOneApp,strOneApp,"/tmp/apps");

      nCharIndex = parseWord(nCharIndex, strAppList, strOneApp,
                             LIST_SEP_CHARACTER);
   }

   /* update the lists */
   for (i=0;i<nAppItems;i++) {
      memset(strOneApp,NULL,MAX_PROGRAM_TITLE_LEN+1);
      GetProfileString("APPS",appItems[i]->progroup_title,"NoWayImHere",
		strOneApp,MAX_PROGRAM_TITLE_LEN,"/tmp/apps");

      /* if app here */
      if (strcmp(strOneApp,"NoWayImHere")) {
            appItems[i]->num_roles += 1;
            nWriteFile=1;
      }
   }

   if (nWriteFile) {
      sprintf(fileName,"%s/%s.tmp", dbdir, tables[CURRENT_APPS_ID]);
      sprintf(realFileName,"%s/%s", dbdir, tables[CURRENT_APPS_ID]);

      fp = fopen(fileName, "w");
      if (fp == NULL)
      {
         freeCurrentAppData(appItems, nAppItems);
         return;
      }
      writeCurrentAppData(appItems, nAppItems, fp);
      closeAndEncrypt(fp, fileName);
      (void) rename(fileName, realFileName);
   }

   if (nAppItems > 0)
   	freeCurrentAppData(appItems, nAppItems);
}

void  decrementCurrentAppsFromList( const char  *strAppList, const char *dbdir)

{
AppItem     **appItems;
FILE *fp;
int nAppItems = 0;
int i;
char fileName[MAX_PATH_LEN];
char realFileName[MAX_PATH_LEN];
int nWriteFile = 0;

int nCharIndex;
char strOneApp[MAX_PROGRAM_TITLE_LEN+1];

   appItems = getCurrentAppData(&nAppItems,dbdir);

   nCharIndex = 0;
   nCharIndex = parseWord(0, strAppList, strOneApp, LIST_SEP_CHARACTER);

   /* Loop through each access role */
   while (nCharIndex >= 0) 
   {
      WriteProfileString("APPS",strOneApp,strOneApp,"/tmp/apps");

      nCharIndex = parseWord(nCharIndex, strAppList, strOneApp,
                             LIST_SEP_CHARACTER);
   }

   /* update the lists */
   for (i=0;i<nAppItems;i++) {
      memset(strOneApp,NULL,MAX_PROGRAM_TITLE_LEN+1);
      GetProfileString("APPS",appItems[i]->progroup_title,"NoWayImHere",
		strOneApp,MAX_PROGRAM_TITLE_LEN,"/tmp/apps");

      /* if app here */
      if (strcmp(strOneApp,"NoWayImHere")) {
            appItems[i]->num_roles -= 1;
            if (appItems[i]->num_roles < 0)
               appItems[i]->num_roles=0;
            nWriteFile=1;
      }
   }

   if (nWriteFile) {
      sprintf(fileName,"%s/%s.tmp", dbdir, tables[CURRENT_APPS_ID]);
      sprintf(realFileName,"%s/%s", dbdir, tables[CURRENT_APPS_ID]);

      fp = fopen(fileName, "w");
      if (fp == NULL)
      {
         freeCurrentAppData(appItems, nAppItems);
         return;
      }
      writeCurrentAppData(appItems, nAppItems, fp);
      closeAndEncrypt(fp, fileName);
      (void) rename(fileName, realFileName);
   }

   if (nAppItems > 0)
   	freeCurrentAppData(appItems, nAppItems);
}
/***

void  decrementCurrentAppsFromList( const char  *strAppList )

{
   int nCharIndex;
   char strOneApp[MAX_PROGRAM_TITLE_LEN+1];

   nCharIndex = 0;
   nCharIndex = parseWord(0, strAppList, strOneApp, LIST_SEP_CHARACTER);
   while (nCharIndex >= 0) 
   {
      decrementCurrentAppDataCount(strOneApp);
      nCharIndex = parseWord(nCharIndex, strAppList, strOneApp,
                             LIST_SEP_CHARACTER);
   }
}

**********/

int  clearCurrentAppsTable( const char *db_dir )

{
  char datafile[MAX_FILENAME_LEN];
  char tmp_file[MAX_ITEM_LEN];
  FILE *fp, *rp;
  DBRecord record;
  char decryptfile[MAX_FILENAME_LEN];

  sprintf(tmp_file, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "clearCurrentAppsTable(): Could not open file <%s>!",
             tmp_file);
      return(-1);
    }

  sprintf(datafile, "%s/%s", db_dir, tables[CURRENT_APPS_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "clearCurrentAppsTable(): Could not open file <%s>!",
             datafile);
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(-2);
    }


  while (getRecord(&record, rp, CURRENT_APPS_NUM_RECS))
    fprintf(fp, CURRENT_APPS_FMT,
            record.field[0].data,
            record.field[1].data,
            0,
            atoi(record.field[3].data));

  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);
  closeAndEncrypt(fp, tmp_file);

  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));

  return(0);
}


/*****************************************************************************/

int  applyDatabaseChanges( const char  *sql_file,
                           const char  *db_dir,
                           int          mode )
{
  DBRecord  record;
  FILE  *fp;
  int  flag;
  int  i;
  int  x;


  /* Determine what to do with the involved db files for this operation. */
  switch (mode)
    {
    case DB_REMOVE_OLD:  /* 4 */
      x = copy_database_files(db_dir, REMOVE_OLD);
      return(x);  /* 4 */
      break;

    case DB_CHANGE_INIT:  /* 1  */
      x = copy_database_files(db_dir, INIT_TO_TMP);
      if (x == 0) /* 3 */
        {
          syslog(LOG_WARNING, "applyDatabaseChanges: Failed to make .tmp"
                 " copies of the database files!");
          return(0); /* error occured*/
        }
      break;

    case DB_CHANGE_BB_REDO: /* 3 */
      break;

    default:  /* make a copy of all database files with a .tmp ext. */
      x = copy_database_files(db_dir, ONLINE_TO_TMP);
      if (x == 0)  /* 0 */ 
        {
          syslog(LOG_WARNING, "applyDatabaseChanges: Failed to restore .tmp"
                 " copies to their original database files!");
          return(0); /* error occured*/
        }
      break;
    }

  /* Start modifying the target DB files.. */
  switch (mode)
    {
    case DB_CHANGE_REDO:  /* 2 */
      using_tmp = 1;                           /* global flag */
      record.sql_change = SQL_REDO_DB;
      record.num_items = 0;
      if (apply_sql_change(&record, db_dir, NULL))
        flag = 1;
      else
        flag = 0;
      using_tmp = 0;
      break;

    case DB_CHANGE_BB_REDO:  /* 3 */
      record.sql_change = SQL_REDO_BB;
      record.num_items = 0;
      if (apply_sql_change(&record, db_dir, sql_file))
        return(1);
      else
        return(0);
      break;

    default:
      using_tmp = 1;  /* Tells apply_sql_change to use the .tmp copies. */
      fp = fopen(sql_file, "r");
      if (fp == NULL)
        {
          using_tmp = 0;
          syslog(LOG_WARNING, "applyDatabaseChanges: Unable to open SQL"
                 " file %s!", sql_file);
          return(0);
        }

      flag = 1;

      x = getSqlChange(&record, fp);  /* check for empty SQL file first */
      if (x != 1)
        {
          syslog(LOG_WARNING, "applyDatabaseChanges: SQL file"
                 " <%s> has no records!", sql_file);
          flag = 0;
          using_tmp = 0;
          break;
        }

      do                  /* Process any remaining SQL records in the file.. */
        {
          x = apply_sql_change(&record, db_dir, NULL);
          if (x == 0)                             /* an error in this case.. */
            {
              syslog(LOG_WARNING, "applyDatabaseChanges: Problems processing"
                     " the following record from file %s..", sql_file);
              syslog(LOG_WARNING, "SQL change no: <%i>, Number of columns: "
                     "<%i> ", record.sql_change, record.num_items);
              for (i = 0; i < record.num_items; i++)
                syslog(LOG_WARNING, "Record.field[%i]: %s", i,
                       record.field[i].data);
              flag = 0;
              break; /* from the do-while loop. */
            }
        }
      while (getSqlChange(&record, fp));

      using_tmp = 0;
      fclose(fp);  /* All finished with the SQL file. */
      break;
    }                          /* End of the apply_sql cases. */

  


  if (flag)
    {
      flag = copy_database_files(db_dir, TMP_TO_ONLINE);
      if (flag != 1)
        syslog(LOG_WARNING, "applyDatabaseChanges: Problems swapping in"
               " the new database files!");
    }
  else
    {
      x = copy_database_files(db_dir, REMOVE_TMP); /* Error */
      /*** This is really a benign error..
        if (x != 1)
          syslog(LOG_WARNING, "applyDatabaseChanges: Problems removing "
                 " the database .tmp files!");
      ***/
    }

  if (flag)
    x = timestampDatabase(db_dir);

  return(flag);
}


/*****************************************************************************/

static int  apply_sql_change( DBRecord    *record,
                              const char  *db_dir,
                              const char  *misc_str )
{
  const char  *fnct_name = "apply_sql_change():";
  AppItem   **Apps;
  HostItem  **Hosts;
  PrtItem       **Prts;
  AdapterItem   **Adapters;
  PrtclassItem  **Prtclasses;
  OutputTypeItem     **Otypes;
  PredefinedJobItem  **Pdjs;
  int  num_adapters;
  int  num_apps;
  int  num_hosts;
  int  num_otypes;
  int  num_pdjs;
  int  num_prts;
  int  num_prtclasses;
  int  i, j, k, l, x;
  int  flag = 1;
  char  date[MAX_ITEM_LEN];
  char  datafile[MAX_FILENAME_LEN];
  char  tempfile[MAX_FILENAME_LEN];
  DBRecord  recrd;
  FILE  *fp, *rp;
  char  decryptfile[MAX_FILENAME_LEN];

  switch (record->sql_change)
    {
    case SQL_MOD_NTCSS_USERS:                             /* 0 as of 5/98 */
    case SQL_MOD_BATCH_NTCSS_USERS:                        /* 26 as of 5/98 */
      if (record->num_items != 10)
        {
          syslog(LOG_WARNING, "apply_sql_change: Only %i fields were given "
                 "for the add/edit NTCSS user operation! 10 args are required!",
                 record->num_items);
          return(0);                    /* error */
        }
      syslog(LOG_WARNING, "%s Adding or editing NTCSS user <%s>..", fnct_name,
             record->field[1].data);
      i = editNtcssUser(record, db_dir, misc_str);
      if (i < 0)
        {
          syslog(LOG_WARNING, "%s Failed to add/edit user <%s>!", fnct_name,
                 record->field[1].data);
          return(0);                    /* error */
        }
      else
        return(1);

    case SQL_DEL_NTCSS_USERS:                               /* 1 as of 5/98 */
      if (record->num_items != 1)
        {
          syslog(LOG_WARNING, "%s Only %i fields were given for the NTCSS "
                 "delete user operation! 1 arg is required!", fnct_name,
                 record->num_items);
          return(0);                    /* error */
        }
      syslog(LOG_WARNING, "%s Deleting NTCSS user <%s>..", fnct_name,
             record->field[0].data);
      i = delNtcssUser(record, db_dir);
      if (i < 0)
        {
          syslog(LOG_WARNING, "%s Failed to delete NTCSS user <%s>!",
                 fnct_name, record->field[0].data);
          return(0);                    /* error */
        }
      else
        return(1);

    case SQL_MOD_APPUSERS_NOAPPDATA:                         /* 2 as of 5/98 */
    case SQL_MOD_BATCH_APPUSERS:                            /* 27 as of 5/98 */
      if (record->num_items != 4)
        {
          syslog(LOG_WARNING, "%s Only %i fields were given for the Appuser "
                 "add/edit operation!  4 args are required!", fnct_name,
                 record->num_items);
          return(0);                  /* error */
        }
      syslog(LOG_WARNING, "%s Modifying app user <%s>..", fnct_name,
             record->field[1].data);
      i = modAppUsers(record, db_dir);
      if (i < 0)
        {
          syslog(LOG_WARNING, "%s Failed to add user <%s> to group <%s>!",
                 fnct_name, record->field[1].data, record->field[0].data);
          return(0);                    /* error */
        }
      else
        return(1);

    case SQL_DEL_APPUSERS:                                  /* 3 as of 5/98 */
      if (record->num_items != 2)
        {
          syslog(LOG_WARNING, "%s Only %i fields were given for the App user "
                 "delete operation!  2 args are required!", fnct_name,
                 record->num_items);
          return(0);                    /* error */
        }
      syslog(LOG_WARNING, "%s Deleting app user <%s> from progroup <%s>..",
             fnct_name, record->field[1].data, record->field[0].data);
      i = delAppUser(record, db_dir);
      if (i < 0)
        {
          syslog(LOG_WARNING, "%s Failed to delete appuser <%s> from progroup "
                 "<%s>!", fnct_name, record->field[1].data,
                 record->field[0].data);
          return(0);                    /* error */
        }
      return(1);

    case SQL_MOD_PROGROUPS:                                 /* 4 as of 5/98 */
      if (record->num_items != 9)
        {
          syslog(LOG_WARNING, "%s Only %i fields were given for a progroup "
                 "edit operation!  9 args are required!", fnct_name,
                 record->num_items);
          return(0);  /* error */
        }
      syslog(LOG_WARNING, "%s Modifying a program group <%s>..", fnct_name,
             record->field[0].data);
      i = modProgramGroup(record, db_dir);
      if (i < 0)
        {
          syslog(LOG_WARNING, "apply_sql_change: Problems modifying"
                 " program group <%s>!", record->field[0].data);
          return(0);                    /* error */
        }
      else
        return(1);

    case SQL_MOD_PROGRAMS:                                  /* 5 as of 5/98 */
      if (record->num_items != 10)
        {
          syslog(LOG_WARNING, "%s Only %i fields were given for the SQL_"
                 "MOD_PROGRAMS operation!  10 args are required!", fnct_name,
                 record->num_items);
          return(0);  /* error */
        }
      syslog(LOG_WARNING, "%s Modifying program <%s>..", fnct_name,
             record->field[0].data);

      x = editNtcssProgram(record, db_dir);
      if (x < 0)
        {
          flag = 0;
          syslog(LOG_WARNING, "%s Failed to modify program <%s> for progroup "
                 "<%s>!", fnct_name, record->field[0].data,
                 record->field[5].data);
        }
      break;

    case SQL_MOD_PRINTERS:
    case SQL_CHANGE_PRINTERS:
      if (record->num_items != 14)
        return(0);  /* error */
      syslog(LOG_WARNING, "%s Modifying/changing printer <%s>", fnct_name,
             record->field[0].data);

      /* * Add/mod printer to DB. */
      Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
      for (i = 0; i < num_prts; i++)
/*
  if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0) &&
  (strcmp(record->field[2].data, Prts[i]->hostname) == 0))
*/
        /* Erase the previous record of this printer if it exists. */
        if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0))
          Prts[i]->marker = 0;
        else
          Prts[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writePrtData(Prts, num_prts, fp);
          convFromSql(record, 14);
          fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
                  record->field[0].data,
                  record->field[1].data,
                  record->field[5].data,
                  record->field[6].data,
                  record->field[7].data,
                  record->field[9].data,
                  record->field[3].data,
                  record->field[4].data,
                  record->field[10].data,
                  record->field[8].data,
                  record->field[12].data,
                  record->field[13].data);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePrtData(Prts, num_prts);
      break;


    case SQL_LIMMOD_PRINTERS:
      if (record->num_items != 7)
        return(0);  /* error */

/*
 * Add printer to DB.
 */
      j = -1;
      Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
      for (i = 0; i < num_prts; i++)
        {
/*
          if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0) &&
              (strcmp(record->field[1].data, Prts[i]->hostname) == 0))
*/
          if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0))
            {
              Prts[i]->marker = 0;
              j = i;
            }
          else
            Prts[i]->marker = 1;
        }
      
      if (j == -1)
        {
          freePrtData(Prts, num_prts);
          return(0);  /* error */
        }
      
      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writePrtData(Prts, num_prts, fp);
          convFromSql(record, 7);
          fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%i\n%s\n%s\n%s\n%s\n\n",
                  record->field[0].data,
                  record->field[1].data,
                  record->field[3].data,
                  record->field[4].data,
                  Prts[j]->prt_class_title,
                  Prts[j]->ip_address,
                  Prts[j]->security_class,
                  record->field[2].data,
                  Prts[j]->port_name,
                  record->field[5].data,
                  record->field[6].data);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePrtData(Prts, num_prts);
      break;


    case SQL_DEL_PRINTERS:
      if (record->num_items != 2)
     return(0);  /* error */
      syslog(LOG_WARNING, "%s Deleting printer <%s>", fnct_name,
             record->field[0].data);

      sprintf(date, "%s@%s", record->field[0].data, record->field[1].data);
/*
 * Delete printer from DB.
 */
      Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
      for (i = 0; i < num_prts; i++)
/*
        if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0) &&
            (strcmp(record->field[1].data, Prts[i]->hostname) == 0))
*/
        if ((strcmp(record->field[0].data, Prts[i]->prt_name) == 0))
          Prts[i]->marker = 0;
        else
          Prts[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writePrtData(Prts, num_prts, fp);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePrtData(Prts, num_prts);
      break;


    case SQL_REDO_PRINTERS:
      if (record->num_items != 0)
        return(0);  /* error */
      syslog(LOG_WARNING, "%s Checking printer DB integrity...", fnct_name);

/*
 * Check integrity of output type default printers in DB.
 */

/*    Apps = getAppData(&num_apps, db_dir, WITHOUT_NTCSS_GROUP); ==> */
      Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP); /* <== */
      Prts = getPrtData(&num_prts, db_dir, Apps, num_apps);
      Otypes = getOutputTypeData(&num_otypes, db_dir);

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_TYPE_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          for (i = 0; i < num_otypes; i++)
            {
              l = 0;
              if (!isBlank(Otypes[i]->default_prt))
                {
                  for (j = 0; j < num_prts; j++)
                    {
/*****
      sprintf(date, "%s@%s", Prts[j]->prt_name,
      Prts[j]->hostname);
      if (strcmp(date, Otypes[i]->default_prt) != 0)
****/
                      if (strcmp(Otypes[i]->default_prt,
                                 Prts[j]->prt_name) != 0)
                        continue;

                      for (k = 0; k < num_apps; k++)
                        {
                          if (strcmp(Otypes[i]->progroup_title,
                                     Apps[k]->progroup_title) != 0)
                            continue;
                          if (Prts[j]->prt_access[k])
                            {
                              l = 1;
                              break;
                            }
                        }
                      if (l == 1)
                        break;
                    }               /* end of for(j = 0; j < num_prts; j++) */
                }
              if (l == 0)
                fprintf(fp, OUTPUT_TYPE_FMT,
                        Otypes[i]->out_type_title,
                        Otypes[i]->progroup_title,
                        Otypes[i]->description,
                        "",
                        Otypes[i]->batch_flag);
              else
                fprintf(fp, OUTPUT_TYPE_FMT,
                        Otypes[i]->out_type_title,
                        Otypes[i]->progroup_title,
                        Otypes[i]->description,
                        Otypes[i]->default_prt,
                        Otypes[i]->batch_flag);
            }
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeOutputTypeData(Otypes, num_otypes);
      if (flag == 0)  /* couldn't open the .tmp file so.. */
        {
          freeAppData(Apps, num_apps);
          freePrtData(Prts, num_prts);
          break;
        }

/*
 * Check integrity of links between output types and printers in DB.
 */
      sprintf(tempfile, "%s/%s.temp", db_dir, tables[OUTPUT_PRT_ID]);
      fp = fopen(tempfile, "w");
      if (fp == NULL)
        {
          freeAppData(Apps, num_apps);
          freePrtData(Prts, num_prts);
          flag = 0;
          break;
        }
      sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_PRT_ID]);
      rp = decryptAndOpen(datafile, decryptfile, "r");
      if (rp == NULL)
        {
          fclose(fp);
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          freeAppData(Apps, num_apps);
          freePrtData(Prts, num_prts);
          flag = 0;
          break;
        }
      while (getRecord(&recrd, rp, OUTPUT_PRT_NUM_RECS))
        {
          l = 0;
          for (j = 0; j < num_prts; j++)
            {
/*
              if ((strcmp(recrd.field[2].data, Prts[j]->prt_name) != 0) ||
                  (strcmp(recrd.field[3].data, Prts[j]->hostname) != 0))
*/
              if ((strcmp(recrd.field[2].data, Prts[j]->prt_name) != 0))
                continue;
              for (k = 0; k < num_apps; k++)
                {
                  if (strcmp(recrd.field[1].data,
                             Apps[k]->progroup_title) != 0)
                    continue;
                  if (Prts[j]->prt_access[k])
                    {
                      fprintf(fp, OUTPUT_PRT_FMT,
                              recrd.field[0].data,
                              recrd.field[1].data,
                              recrd.field[2].data,
                              recrd.field[3].data);
                      l = 1;
                      break;
                    }
                }
              if (l == 1)
                break;
            }
        }
      closeAndEncrypt(fp, tempfile);
      fclose(rp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);

      freeAppData(Apps, num_apps);
      freePrtData(Prts, num_prts);
      x = rename(tempfile, datafile);
      if (x < 0)
        flag = 0;
      break;
      

    case SQL_ADD_PRINTER_ACCESS:
      if (record->num_items != 3)
        return(0);                                 /* error */
      syslog(LOG_WARNING, "%s Adding access for printer <%s>", fnct_name,
             record->field[0].data);
/*
 * Add printer access to DB.
 */
      x = changePrinterAccessData(record->sql_change, db_dir,
                                  record->field[0].data,
                                  record->field[1].data,
                                  record->field[2].data);
      if (x == 0)
        flag = 0;
      break;
      
      
/*
 * Delete printer access from DB.
 */
    case SQL_DEL_PRINTER_ACCESS:
      if (record->num_items != 2)
        return(0);                         /* error */
      syslog(LOG_WARNING, "%s Deleting access for printer <%s>..", fnct_name,
             record->field[0].data);
      x = changePrinterAccessData(record->sql_change, db_dir,
                                   record->field[0].data,
                                   record->field[1].data, NULL);
      if (x == 0)
        flag = 0;
      break;


/*
 * Add adapter to DB.
 */
    case SQL_MOD_ADAPTERS:
    case SQL_CHANGE_ADAPTERS:
      if (record->num_items != 4)
        return(0);                  /* error */
      syslog(LOG_WARNING, "%s Modifying/Changing printer adapters...",
             fnct_name);
      Adapters = getAdapterData(&num_adapters, db_dir);
      for (i = 0; i < num_adapters; i++)
        if (strcmp(record->field[0].data, Adapters[i]->ip_address) == 0)
          Adapters[i]->marker = 0;
        else
          Adapters[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[ADAPTERS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writeAdapterData(Adapters, num_adapters, fp);
          fprintf(fp, ADAPTERS_FMT,
                  record->field[3].data,
                  record->field[1].data,
                  record->field[0].data,
                  record->field[2].data);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeAdapterData(Adapters, num_adapters);
      break;

/*
 * Delete adapter from DB.
 */
    case SQL_DEL_ADAPTERS:
      if (record->num_items != 1)
        return(0);                  /* error */
      syslog(LOG_WARNING, "%s Deleting a printer adapter.", fnct_name);
      Adapters = getAdapterData(&num_adapters, db_dir);
      for (i = 0; i < num_adapters; i++)
        if (strcmp(record->field[0].data, Adapters[i]->ip_address) == 0)
          Adapters[i]->marker = 0;
        else
          Adapters[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[ADAPTERS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writeAdapterData(Adapters, num_adapters, fp);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeAdapterData(Adapters, num_adapters);
      break;

/*
 * Add output type to DB.
 */
    case SQL_MOD_OUTPUT_TYPE:
      if (record->num_items != 5)
        return(0);  /* error */
      syslog(LOG_WARNING, "\5s Modifying an output type.", fnct_name);
      Otypes = getOutputTypeData(&num_otypes, db_dir);
      for (i = 0; i < num_otypes; i++)
        if ((strcmp(record->field[0].data, Otypes[i]->out_type_title) == 0) &&
            (strcmp(record->field[1].data, Otypes[i]->progroup_title) == 0))
          Otypes[i]->marker = 0;
        else
          Otypes[i]->marker = 1;
      
      sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_TYPE_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          convFromSql(record, 5);
          writeOutputTypeData(Otypes, num_otypes, fp);
          fprintf(fp, "%s\n%s\n%s\n%s\n%s\n\n",
                  record->field[0].data,
                  record->field[1].data,
                  record->field[2].data,
                  record->field[3].data,
                  record->field[4].data);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeOutputTypeData(Otypes, num_otypes);
      break;

/*
 * Delete output type from DB.
 */
    case SQL_DEL_OUTPUT_TYPE:
      if (record->num_items != 2)
     return(0);
      syslog(LOG_WARNING, "%s Deleting an output type..", fnct_name);
      Otypes = getOutputTypeData(&num_otypes, db_dir);
      for (i = 0; i < num_otypes; i++)
        if ((strcmp(record->field[0].data, Otypes[i]->out_type_title) == 0) &&
            (strcmp(record->field[1].data, Otypes[i]->progroup_title) == 0))
          Otypes[i]->marker = 0;
        else
          Otypes[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_TYPE_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writeOutputTypeData(Otypes, num_otypes, fp);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeOutputTypeData(Otypes, num_otypes);
      break;

/*
 * Add predefined job to DB.
 */
    case SQL_MOD_PREDEFINED_JOB:
      if (record->num_items != 6)
        return(0);
      syslog(LOG_WARNING, "%s Modifying predefined job <%s> in progroup %s...",
             fnct_name, record->field[0].data, record->field[1].data);
      Pdjs = getPredefinedJobData(&num_pdjs, db_dir);
      for (i = 0; i < num_pdjs; i++)
        if ((strcmp(record->field[0].data, Pdjs[i]->job_title) == 0) &&
            (strcmp(record->field[1].data, Pdjs[i]->progroup_title) == 0))
          Pdjs[i]->marker = 0;
        else
          Pdjs[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PREDEFINED_JOB_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          convFromSql(record, 6);
          writePredefinedJobData(Pdjs, num_pdjs, fp); /* all non-related */
          fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n\n",
                  record->field[0].data,    /* job title */
                  record->field[1].data,    /* progroup title */
                  record->field[2].data,    /* command line */
                  record->field[3].data,    /* access role */
                  record->field[4].data,    /* bitmasked job type flag */
                  record->field[5].data);   /* time schedule */
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePredefinedJobData(Pdjs, num_pdjs);
      break;

/*
 * Delete output type from DB.
 */
    case SQL_DEL_PREDEFINED_JOB:
      if (record->num_items != 2)
        return(0);
      syslog(LOG_WARNING, "%s Deleting predefined job <%s> in progroup %s..",
             fnct_name, record->field[0].data, record->field[1].data);
      Pdjs = getPredefinedJobData(&num_pdjs, db_dir);
      for (i = 0; i < num_pdjs; i++)
        if ((strcmp(record->field[0].data, Pdjs[i]->job_title) == 0) &&
            (strcmp(record->field[1].data, Pdjs[i]->progroup_title) == 0))
          Pdjs[i]->marker = 0;
        else
          Pdjs[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PREDEFINED_JOB_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writePredefinedJobData(Pdjs, num_pdjs, fp);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePredefinedJobData(Pdjs, num_pdjs);
      break;

/*
 * Add output printer to DB.
 */
    case SQL_ADD_OUTPUT_PRT:
      if (record->num_items != 4)
        return(0);
      syslog(LOG_WARNING, "%s Adding an output type..", fnct_name);
      if (!changeOutputPrtData(record->sql_change, db_dir,
                               record->field[0].data,
                               record->field[1].data,
                               record->field[2].data,
                               record->field[3].data))
        flag = 0;
      break;

/*
 * Delete output printer from DB.
 */
    case SQL_DEL_OUTPUT_PRT:
      if (record->num_items != 2)
        {
          flag = 0;
          break;
        }
      syslog(LOG_WARNING, "%s Deleting output printer from group <%s>",
             fnct_name, record->field[1].data);
      if (!changeOutputPrtData(record->sql_change, db_dir, NULL, NULL,
                               record->field[0].data,
                               record->field[1].data))
        flag = 0;
      break;

/*
 * Add/Update hosts to DB.
 */
    case SQL_MOD_HOSTS:
      if (record->num_items != HOSTS_NUM_RECS-1)
        return(0);
      syslog(LOG_WARNING, "%s Modifying host <%s>..", fnct_name,
             record->field[0].data);
      j = 10;
      Hosts = getHostData(&num_hosts, db_dir);
      for (i = 0; i < num_hosts; i++)
        if (strcmp(record->field[0].data, Hosts[i]->hostname) == 0)
          {
            j = Hosts[i]->num_processes;
                     /* 98-08-06 GWY Added nHostType */
         /* nHostType = Hosts[i]->type;*/
         /* nRepl = Hosts[i]->repl;*/
            Hosts[i]->marker = 0;
          } 
        else
          Hosts[i]->marker = 1;

      /* Get old process limit value.*/
      sprintf(datafile, "%s/%s.old", db_dir, tables[HOSTS_ID]);
      fp = decryptAndOpen(datafile, decryptfile, "r");
      if (fp != NULL)
        {
          while (getRecord(&recrd, fp, HOSTS_NUM_RECS))
            {
              if (strcmp(recrd.field[0].data, record->field[0].data) == 0) 
                j = atoi(recrd.field[2].data); 
            }
          fclose(fp);
          if (decryptfile[0] != '\0')
            remove(decryptfile);
        }
            
      sprintf(datafile, "%s/%s.tmp", db_dir, tables[HOSTS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writeHostData(Hosts, num_hosts, fp);
          fprintf(fp, HOSTS_FMT,
                  record->field[0].data,
                  record->field[1].data,
                  j,
                  atoi(record->field[2].data),
                  atoi(record->field[3].data));
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freeHostData(Hosts, num_hosts);
      break;

/*
 * Add/modify printer classes to DB.
 */
    case SQL_MOD_PRT_CLASS:
      if (record->num_items != 3)
        return(0);
      syslog(LOG_WARNING, "%s Modifying printer class <%s>", fnct_name,
             record->field[0].data);
      Prtclasses = getPrtclassData(&num_prtclasses, db_dir);
      for (i = 0; i < num_prtclasses; i++)
        if (strcmp(record->field[0].data, Prtclasses[i]->prt_class_title) == 0)
          Prtclasses[i]->marker = 0;
        else
          Prtclasses[i]->marker = 1;

      sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRT_CLASS_ID]);
      fp = fopen(datafile, "w");
      if (fp != NULL)
        {
          writePrtclassData(Prtclasses, num_prtclasses, fp);
          fprintf(fp, PRT_CLASS_FMT,
                  record->field[0].data,
                  record->field[1].data,
                  record->field[2].data);
          closeAndEncrypt(fp, datafile);
        }
      else
        flag = 0;

      freePrtclassData(Prtclasses, num_prtclasses);
      break;

/*
 * Run the database integrity checker
 */
    case SQL_REDO_DB:
      if (record->num_items != 0)
        {
          syslog(LOG_WARNING, "%s Args were specified for the DB integrity "
                 "checker!", fnct_name);
          return(0);
        }
      syslog(LOG_WARNING, "%s Running database integrity checker..",
             fnct_name);
      flag = ensure_database_integrity(db_dir);
      break;

/*
 * Run the message bulletin boards integrity checker
 */
    case SQL_REDO_BB:
      if (record->num_items != 0)
        return(0);
      syslog(LOG_WARNING, "%s Running Bulletin Board database integrity "
             "checker.", fnct_name);
      flag = ensure_msgbb_integrity(misc_str, db_dir);
      break;

/*
 * Default
 */
    default:
      syslog(LOG_WARNING, "%s Unknown SQL change <%i> encountered!",
             fnct_name, record->sql_change);    
      return(0);   /* error */
    }

  return(flag);    /* 0 means error occured */
}


/*****************************************************************************/

int  ensure_database_integrity( const char  *db_dir )

{
  const char    *fnct_name = "ensure_database_integrity():";
  AppItem      **Apps;
  AppuserItem  **Appusers;
  PrtItem      **Printers;
  int   role_map[MAX_NUM_PROGROUPS][2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3];
  int   num_apps;
  int   num_appusers;
  int   num_printers;
  int   x;                              /* general purpose */

/****              
  x = ModifyNTCSSAdminProgArgs(db_dir);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to Modify the NTCSS programs' "
      "command lines!  Error %d", fnct_name, x);
    }
*****/

  /*  Determine access role change operations. */
 
  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_AND_WITH_ROLES);
  if (num_apps < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get app data!", fnct_name);
      return(0);
    }

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  if (num_appusers < 0)
    {
      freeAppData(Apps, num_apps);
      syslog(LOG_WARNING, "%s Failed to get appuser data!", fnct_name);
      return(0);
    }

  x = RemoveApplessAppusers(db_dir, ".tmp", Apps, num_apps, Appusers,
                            num_appusers, 0);
  if (x < 0)
    {
      freeAppuserData(Appusers, num_appusers);
      freeAppData(Apps, num_apps);
      syslog(LOG_WARNING, "%s Unable to update the appusers database!  "
             "Error %d", fnct_name, x);
      return(0);   /* error */
    }


  x = InitializeRoleMap(role_map, Apps, num_apps, db_dir); 


/*   x = ResolveAppUserAccessRoleNumShifts(role_map, Apps, num_apps, db_dir);*/


  x = ResolveAppUserAccessRoleNumShifts(db_dir, ".tmp", role_map, Apps,
                                        num_apps, Appusers, num_appusers, 1);


  /* TONY one more run to clean up orphaned appusers 
  x = RemoveApplessAppusers(db_dir, ".tmp", Apps, num_apps, Appusers,
                            num_appusers, 1);
*/

  freeAppuserData(Appusers, num_appusers); /* No longer need Apps.. */

  if (x < 0)
    {
      freeAppData(Apps, num_apps);
      syslog(LOG_WARNING, "%s Unable to reconcile role number changes for"
             " the Appusers DB!  Error %d", fnct_name, x);
      return(0);   /* error */
    }



/****************
  x = UpdatePDJs(role_map, Apps, num_apps, db_dir);


  if (x < 0)
    {
      freeAppData(Apps, num_apps);
      syslog(LOG_WARNING, "%s Unable to verify/remove PDJs associated with "
             "valid/invalid apps!  Error %d", fnct_name, x);
      return(0); 
    }
********************/


  x = RemoveApplessPrtAccessRecs(Apps, num_apps, db_dir);

/*
  freeAppData(Apps, num_apps);
*/
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to verify & remove Printer access "
             "records associated with invalid apps!  Error %d", fnct_name, x);
      return(0);   /* error */
    }


  Printers = getPrtData(&num_printers, db_dir, (AppItem **) 0, 0);


  x = RemoveClasslessPrinters(Printers, num_printers, db_dir);

  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to verify & remove printer records not "
             "associated with valid printer classes!  Error %d", fnct_name, x);
      freePrtData(Printers, num_printers);
      return(0);   /* error */
    }


  x = UpdateOutputTypesDB(Printers, num_printers, db_dir);

  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to update the output_type database!  "
             "Error %d", fnct_name, x);
      freePrtData(Printers, num_printers);
      return(0);   /* error */
    }

  x = RemovePrinterlessPrtAccessRecs(Printers, num_printers, db_dir);

  freePrtData(Printers, num_printers);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to update the prt_access database!  "
             "Error %d", fnct_name, x);
      return(0);   /* error */
    }


  x = InitializeCurrentAppsDB(db_dir);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to update the current_apps database!  "
             "Error %d", fnct_name, x);
      return(0);   /* error */
    }

  return(1);
}


/*****************************************************************************/

static int  ensure_msgbb_integrity( const char  *msg_dir,
                                    const char  *db_dir )

{
  AppItem      **Apps;
  AppuserItem  **Appusers;
  UserItem     **Users;
  int   flag;
  int   i, j, k, x;
  int   max_bbs = MAX_ARRAY_SIZE;
  int   num_apps;
  int   num_appusers;
  int   num_bbs;
  int   num_users;
  char   bb_name[MAX_BB_NAME_LEN + 1];
  char **bb_names;
  char   datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "ensure_msgbb_integrity():";
  char   item[MAX_ITEM_LEN];
  char   msgid[MAX_MSG_ID_LEN + 1];
  char   tempfile[MAX_FILENAME_LEN];
  char   user[MAX_LOGIN_NAME_LEN + 1];
  FILE  *fp;
  FILE  *rp;
  DIR   *dp;
#ifdef HPUX
  struct direct  *direc;
#else
  struct dirent  *direc;
#endif
  struct stat  sbuf;

  Users = getUserData(&num_users, db_dir);
  if (num_users <= 0)
    return(0);

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);
  if (num_apps <= 0)
    {
      freeUserData(Users, num_users);
      return(0);
    }

/*
 * Check integrity of all general bulletin boards.
 */
    sprintf(datafile, "%s/%s", msg_dir, BUL_BRDS_TABLE);
    rp = fopen(datafile, "r");
    if (rp == NULL)
      {
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }
    sprintf(tempfile, "%s/%s.tmp", msg_dir, BUL_BRDS_TABLE);
    fp = fopen(tempfile, "w");
    if (fp == NULL)
      {
        fclose(rp);
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }

    num_bbs = 0;
    bb_names = (char **) malloc(sizeof (char *)*max_bbs);

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
      {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (getBulBrdType(bb_name) == MSG_BB_TYPE_SYSTEM)
          continue;
        if (strcmp(user, bb_name) == 0)
          continue;
        for (i = 0; i < num_users; i++)
          {
            if (strcmp(Users[i]->login_name, user) == 0)
              {
                fprintf(fp, "%s\n", item);
                bb_names[num_bbs] = allocAndCopyString(bb_name);
                num_bbs++;
                break;
              }
          }
        if (num_bbs >= max_bbs)
          {
            max_bbs += MAX_ARRAY_SIZE;
            bb_names = (char **) realloc(bb_names, sizeof(char *)*max_bbs);
          }
      }
    fclose(rp);

/*
 * Add in all system bulletin boards.
 */
    sprintf(item, "%%-%ds %%s.%%s", MAX_LOGIN_NAME_LEN);
    for (i = 0; i < num_apps; i++)
      {
        fprintf(fp, item, "SYSTEM", Apps[i]->progroup_title, "adm");
        fprintf(fp, "\n");
        fprintf(fp, item, "SYSTEM", Apps[i]->progroup_title, "all");
        fprintf(fp, "\n");
      }
/*
 * Add in all user bulletin boards.
 */
    sprintf(item, "%%-%ds %%s", MAX_LOGIN_NAME_LEN);
    for (i = 0; i < num_users; i++)
      {
        fprintf(fp, item, Users[i]->login_name, Users[i]->login_name);
        fprintf(fp, "\n");
      }
    fclose(fp);

    x = rename(tempfile, datafile);
    if (x < 0)
      {
        syslog(LOG_WARNING, "%s Rename returns error  %i trying to rename %s "
               "to %s!", fnct_name, x, tempfile, datafile);
        for (i = num_bbs - 1; i >= 0; i--)
          free((char *) bb_names[i]);
        free((char *) bb_names);
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }
    (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
/*
 * Check integrity of subscribers table.
 */
    sprintf(datafile, "%s/%s", msg_dir, SUBSCRIBERS_TABLE);
    rp = fopen(datafile, "r");
    if (rp == NULL)
      {
        syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, datafile);
        for (i = num_bbs - 1; i >= 0; i--)
          free((char *) bb_names[i]);
        free((char *) bb_names);
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }

    sprintf(tempfile, "%s/%s.tmp", msg_dir, SUBSCRIBERS_TABLE);
    fp = fopen(tempfile, "w");
    if (fp == NULL)
      {
        syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, tempfile);
        fclose(rp);
        for (i = num_bbs - 1; i >= 0; i--)
          free((char *) bb_names[i]);
        free((char *) bb_names);
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
      {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (getBulBrdType(bb_name) == MSG_BB_TYPE_SYSTEM)
          continue;
        if (strcmp(user, bb_name) == 0)
          continue;
        for (i = 0; i < num_users; i++) {
          if (strcmp(Users[i]->login_name, user) == 0)
            {
              for (j = 0; j < num_bbs; j++)
                {
                  if (strcmp(bb_names[j], bb_name) == 0)
                    {
                      fprintf(fp, "%s\n", item);
                      break;
                    }
                }
              break;
            }
        }
      } /* End while get item loop */
    fclose(rp);

/*
 * Add subscriptions to system bulletin boards.
 */
/*  Appusers = getAppuserData(&num_appusers, db_dir, WITHOUT_NTCSS_GROUP);*/
    Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
    sprintf(item, "%%-%ds %%s.%%s", MAX_LOGIN_NAME_LEN);
    for (i = 0; i < num_appusers; i++)
      {
        fprintf(fp, item, Appusers[i]->login_name,
                Appusers[i]->progroup_title, "all");
        fprintf(fp, "\n");
        if (Appusers[i]->app_role & 1)
          {
            fprintf(fp, item, Appusers[i]->login_name,
                    Appusers[i]->progroup_title, "adm");
            fprintf(fp, "\n");
          }
      }

    freeAppuserData(Appusers, num_appusers);

    for (i = 0; i < num_users; i++)
      {
        fprintf(fp, item, Users[i]->login_name, NTCSS_APP, "all");
        fprintf(fp, "\n");
        if (Users[i]->user_role == 1)
          {
            fprintf(fp, item, Users[i]->login_name, NTCSS_APP, "adm");
            fprintf(fp, "\n");
          }
      }
    fclose(fp);
    x = rename(tempfile, datafile);
    if (x < 0)
      {
        syslog(LOG_WARNING, "%s Rename returns error  %i trying to rename "
               "%s to %s!", fnct_name, x, tempfile, datafile);
        for (i = num_bbs - 1; i >= 0; i--)
          free((char *) bb_names[i]);
        free((char *) bb_names);
        freeUserData(Users, num_users);
        freeAppData(Apps, num_apps);
        return(0);
      }
    (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
/*
 * Check integrity of bulletin board messages tables.
 */
    for (k = 0; k < 11; k++)
      {
        switch (k)
          {
          case 0:
            sprintf(datafile, "%s/%s09", msg_dir, BB_MSGS_TABLE);
            break;
          case 1:
            sprintf(datafile, "%s/%sAE", msg_dir, BB_MSGS_TABLE);
            break;
          case 2:
            sprintf(datafile, "%s/%sFJ", msg_dir, BB_MSGS_TABLE);
            break;
          case 3:
            sprintf(datafile, "%s/%sKO", msg_dir, BB_MSGS_TABLE);
            break;
          case 4:
            sprintf(datafile, "%s/%sPT", msg_dir, BB_MSGS_TABLE);
            break;
          case 5:
            sprintf(datafile, "%s/%sUZ", msg_dir, BB_MSGS_TABLE);
            break;
          case 6:
            sprintf(datafile, "%s/%sae", msg_dir, BB_MSGS_TABLE);
            break;
          case 7:
            sprintf(datafile, "%s/%sfj", msg_dir, BB_MSGS_TABLE);
            break;
          case 8:
            sprintf(datafile, "%s/%sko", msg_dir, BB_MSGS_TABLE);
            break;
          case 9:
            sprintf(datafile, "%s/%spt", msg_dir, BB_MSGS_TABLE);
            break;
          case 10:
            sprintf(datafile, "%s/%suz", msg_dir, BB_MSGS_TABLE);
            break;
          }
        sprintf(tempfile, "%s.tmp", datafile);

        rp = fopen(datafile, "r");
        if (rp == NULL)
          {
            syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                   datafile);
            for (i = num_bbs - 1; i >= 0; i--)
              free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            freeAppData(Apps, num_apps);
            return(0);
          }
        fp = fopen(tempfile, "w");
        if (fp == NULL)
          {
            syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                   tempfile);
            fclose(rp);
            for (i = num_bbs - 1; i >= 0; i--)
              free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            freeAppData(Apps, num_apps);
            return(0);
          }
        while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
          {
            stripNewline(item);
            ncpy(bb_name, item, MAX_BB_NAME_LEN);
            trim(bb_name);
            ncpy(msgid, &item[MAX_BB_NAME_LEN + 1], MAX_MSG_ID_LEN);
            trim(msgid);
/*
 * Check integrity of bulletin board.
 */
            flag = 1;
            switch (getBulBrdType(bb_name))
              {
              case MSG_BB_TYPE_SYSTEM:
                for (i = 0; i < num_apps; i++)
                  {
                    if (strncmp(bb_name, Apps[i]->progroup_title,
                                strlen(Apps[i]->progroup_title)) == 0)
                      {
                        flag = 0;
                        break;
                      }
                  }
                break;

              case MSG_BB_TYPE_GENERAL:
                for (j = 0; j < num_bbs; j++)
                  {
                    if (strcmp(bb_names[j], bb_name) == 0)
                      {
                        flag = 0;
                        break;
                      }
                  }
                break;

              case MSG_BB_TYPE_USER:
                for (j = 0; j < num_users; j++)
                  {
                    if (strcmp(Users[j]->login_name, bb_name) == 0)
                      {
                        flag = 0;
                        break;
                      }
                  }
                break;
              }
            if (flag)
              continue;
/*
 * Check integrity of message.
 */
            if (isdigit((int)msgid[DATETIME_LEN - 1]) != 0)
              {                                     /* Indicates that    */
                fprintf(fp, "%s\n", item);          /* message source is */
                continue;                           /* system not user.  */
              }
            for (i = 0; i < num_users; i++)
              {
                if (strcmp(Users[i]->login_name,
                           &msgid[DATETIME_LEN - 1]) == 0)
                  {
                    fprintf(fp, "%s\n", item);
                    break;
                  }
              }
          } /* End of the while(there are items) loop */

        fclose(rp);
        fclose(fp);
        x = rename(tempfile, datafile);
        if (x < 0)
          {
            syslog(LOG_WARNING, "%s Rename returns error  %i trying to rename"
                   " %s to %s!", fnct_name, x, tempfile, datafile);
            for (i = num_bbs - 1; i >= 0; i--)
              free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            freeAppData(Apps, num_apps);
            return(0);
          }
        (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
      }
    freeAppData(Apps, num_apps);
/*
 * Check integrity of markers tables.
 */
    for (k = 0; k < 6; k++)
      {
        switch (k)
          {
          case 0:
            sprintf(datafile, "%s/%s09", msg_dir, MARKERS_TABLE);
            break;
          case 1:
            sprintf(datafile, "%s/%sae", msg_dir, MARKERS_TABLE);
            break;
          case 2:
            sprintf(datafile, "%s/%sfj", msg_dir, MARKERS_TABLE);
            break;
          case 3:
            sprintf(datafile, "%s/%sko", msg_dir, MARKERS_TABLE);
            break;
          case 4:
            sprintf(datafile, "%s/%spt", msg_dir, MARKERS_TABLE);
            break;
          case 5:
            sprintf(datafile, "%s/%suz", msg_dir, MARKERS_TABLE);
            break;
          }

        sprintf(tempfile, "%s.tmp", datafile);
        rp = fopen(datafile, "r");
        if (rp == NULL)
          {
            syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                   datafile);
            for (i = num_bbs - 1; i >= 0; i--)
                free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            return(0);
          }
        fp = fopen(tempfile, "w");
        if (fp == NULL)
          {
            syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                   tempfile);
            fclose(rp);
            for (i = num_bbs - 1; i >= 0; i--)
                free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            return(0);
          }
        while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
          {
            stripNewline(item);
            ncpy(user, item, MAX_LOGIN_NAME_LEN);
            trim(user);
            strcpy(msgid, &item[MAX_LOGIN_NAME_LEN + 1]);
            trim(msgid);
/*
 * Check integrity of user.
 */
            flag = 1;
            for (j = 0; j < num_users; j++)
              {
                if (strcmp(Users[j]->login_name, user) == 0)
                  {
                    flag = 0;
                    break;
                  }
              }
            if (flag)
              continue;
/*
 * Check integrity of message.
 */
            if (isdigit((int)msgid[DATETIME_LEN - 1]))
              {                                     /* Indicates that    */
                fprintf(fp, "%s\n", item);          /* message source is */
                continue;                           /* system not user.  */
              }
            for (i = 0; i < num_users; i++)
              {
                x = strcmp(Users[i]->login_name, &msgid[DATETIME_LEN - 1]);
                if (x == 0)
                  {
                    fprintf(fp, "%s\n", item);
                    break;
                  }
              }
          }
        fclose(rp);
        fclose(fp);

        x = rename(tempfile, datafile);
        if (x < 0)
          {
            syslog(LOG_WARNING, "%s Rename returns error  %i trying to "
                   "rename %s to %s!", fnct_name, x, tempfile, datafile);
            for (i = num_bbs - 1; i >= 0; i--)
                free((char *) bb_names[i]);
            free((char *) bb_names);
            freeUserData(Users, num_users);
            return(0);
          }
        (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
      }

/*
 * Check integrity of user directories.
 */
    dp = opendir(msg_dir);
    if (dp == NULL)
      {
        syslog(LOG_WARNING, "%s Could not open directory %s!", fnct_name,
               msg_dir);
        for (i = num_bbs - 1; i >= 0; i--)
          free((char *) bb_names[i]);
        free((char *) bb_names);
        freeUserData(Users, num_users);
        return(0);
      }
    while ((direc = readdir(dp)) != NULL)
      {
        if (direc->d_ino == 0)
          continue;
        if (strcmp(direc->d_name, ".") == 0 ||
            strcmp(direc->d_name, "..") == 0)
          continue;
        sprintf(datafile, "%s/%s", msg_dir, direc->d_name);
        if (stat(datafile, &sbuf) == -1)
          continue;
        if ((sbuf.st_mode & S_IFMT) != S_IFDIR)
          continue;
/*
 * Determine if directory is a system directory.
 */
        flag = 1;
        if ((strlen(direc->d_name) == 3) &&
            isdigit((int)direc->d_name[0]) &&
            isdigit((int)direc->d_name[1]) &&
            isdigit((int)direc->d_name[2]))
          {
            i = atoi(direc->d_name);
            if (i >= 1 && i <= NUM_SYSTEM_MSG_BBS)
              flag = 0;
          }
        else
          {
            for (i = 0; i < num_users; i++)
              {
                if (strcmp(Users[i]->login_name, direc->d_name) == 0)
                  {
                    flag = 0;
                    Users[i]->marker = 0;
                    break;
                  }
              }
          }
        if (flag)
          {
            sprintf(tempfile, "/bin/rm -rf \"%s\"", datafile);
            (void) system(tempfile);
          }
      }
    closedir(dp);

    flag = 1;
    for (i = 0; i < num_users; i++)
      {
        if (Users[i]->marker)
          {
            sprintf(datafile, "%s/%s", msg_dir, Users[i]->login_name);
            x = mkdir(datafile, (mode_t)(S_IRUSR|S_IWUSR|S_IXUSR));

            /* if mkdir had an error other that dir exists, error */
            if (x != 0 && errno != EEXIST)
              {
                syslog(LOG_WARNING, "%s Could not create directory %s!",
                       fnct_name, datafile);
                flag = 0;
                continue;
              }
          }
        sprintf(datafile, "%s/%s/lock_file", msg_dir, Users[i]->login_name);
        k = open(datafile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (k < 0)
          {
             syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                    datafile);
            flag = 0;
          }
        else
          {
            close(k);
            (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
          }
      }
/*
 * Check integrity of system directories.
 */
    if (flag)
      {
        for (i = 1; i <= NUM_SYSTEM_MSG_BBS; i++)
          {
            sprintf(datafile, "%s/%3.3d", msg_dir, i);
            if (access(datafile, F_OK) == -1)
              {
                x = mkdir(datafile, (mode_t)(S_IRUSR|S_IWUSR|S_IXUSR));
                if (x != 0)
                  {
                    syslog(LOG_WARNING, "%s Could not create directory %s!",
                           fnct_name, datafile);
                    flag = 0;
                  }
                else
                  {
                    sprintf(datafile, "%s/%3.3d/lock_file", msg_dir, i);
                    k = open(datafile, O_RDWR | O_CREAT | O_TRUNC,
                             S_IRUSR | S_IWUSR);
                    if (k < 0)
                      {
                        syslog(LOG_WARNING, "%s Could not open file %s!",
                               fnct_name, datafile);
                        flag = 0;
                      }
                    else
                      {
                        close(k);
                        (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
                      }
                  }
                continue;
              }
            if (stat(datafile, &sbuf) == -1)
              {
                flag = 0;
                continue;
              }
            if ((sbuf.st_mode & S_IFMT) != S_IFDIR)
              {
                flag = 0;
                continue;
              }
            sprintf(datafile, "%s/%3.3d/lock_file", msg_dir, i);
            k = open(datafile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (k < 0)
              {
                syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name,
                       datafile);
                flag = 0;
              }
            else
              {
                close(k);
                (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
              }
          }
      }

    for (i = num_bbs - 1; i >= 0; i--)
      free((char *) bb_names[i]);
    free((char *) bb_names);
    freeUserData(Users, num_users);

    return(flag);
}


/*****************************************************************************/
/* Used to return 0 on error, 1 if success.)
 */

static int  mod_msgbb_user( int          is_add,
                            const char  *msg_dir,
                            const char  *mod_user,
                            int          user_role )

{
  const char  *fnct_name = "mod_msgbb_user():";
  int    i, j;
  int    lock_id;
  int    table_id;
  char   bb_name[MAX_BB_NAME_LEN + 1];
  char   datafile[MAX_FILENAME_LEN];
  char   item[MAX_ITEM_LEN];
  char   tmp_file[MAX_FILENAME_LEN];
  char   user[MAX_LOGIN_NAME_LEN + 1];
  FILE  *fp;
  FILE  *rp;

  for (j = 0; j < 2; j++)
    {
      if (j == 0)
        table_id = BUL_BRDS_TABLE_ID;
      else
        table_id = SUBSCRIBERS_TABLE_ID;

      i = returnLockId(0, table_id);
      if (i < 0)
        {
          syslog(LOG_WARNING, "%s Failed to get lock ID!", fnct_name);
          return(-1);
        }

      lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME);
      if (lock_id < 0)
        {
          syslog(LOG_WARNING, "%s Failed to set DB write lock!", fnct_name);
          return(-2);
        }

      returnDbFilename(0, table_id, datafile, msg_dir);
      sprintf(tmp_file, "%s.tmp", datafile);
    
      fp = fopen(tmp_file, "w");
      if (fp == NULL)
        {
          syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name,
                 tmp_file);
          unsetWriteDBLock(lock_id);
          return(-3);
        }

      rp = fopen(datafile, "r");
      if (rp == NULL)
        {
          syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name,
                 datafile);
          unsetWriteDBLock(lock_id);
          fclose(fp);
          return(-4);
        }

      while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
        {
          stripNewline(item);
          ncpy(user, item, MAX_LOGIN_NAME_LEN);
          trim(user);
          strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
          trim(bb_name);
          if ((strcmp(user, mod_user) != 0) ||
              ((table_id == BUL_BRDS_TABLE_ID) &&
               (getBulBrdType(bb_name) == MSG_BB_TYPE_GENERAL)))
            fprintf(fp, "%s\n", item);
        }
      
      if (is_add)
        {
          switch (table_id)
            {
            case BUL_BRDS_TABLE_ID:
              sprintf(item, "%%-%ds %%s", MAX_LOGIN_NAME_LEN);
              fprintf(fp, item, mod_user, mod_user);
            fprintf(fp, "\n");
            break;
            
            case SUBSCRIBERS_TABLE_ID:
              sprintf(item, "%%-%ds %%s.%%s", MAX_LOGIN_NAME_LEN);
              fprintf(fp, item, mod_user, NTCSS_APP, "all");
              fprintf(fp, "\n");
              if (user_role == 1)
                {
                  fprintf(fp, item, mod_user, NTCSS_APP, "adm");
                  fprintf(fp, "\n");
                }
              break;
            }       /* End switch. */
        }      /* End if. */

      fclose(rp);
      fclose(fp);
      (void) rename(tmp_file, datafile);
      (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
      unsetWriteDBLock(lock_id);
    }                                    /* end of for loop */

/*
 * Add user directory and lock file (if needed).
 */

  if (is_add == 0)
    return(0);

  sprintf(datafile, "%s/%s", msg_dir, mod_user);
  sprintf(tmp_file, "%s/lock_file", datafile);

  /* If user does not have own directory, create it.. */
  if (access(datafile, F_OK) == -1)
    {
      if (mkdir(datafile, (mode_t)(S_IRUSR|S_IWUSR|S_IXUSR)) != 0)
        {
          syslog(LOG_WARNING, "%s Failed to create directory %s!", fnct_name,
                 datafile);
          return(-5);
        }

      /* Create a lock file for this user.. */
      j = open(tmp_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (j < 0)
        {
          syslog(LOG_WARNING, "%s Failed to create file %s! --%s", fnct_name,
                 tmp_file, strerror(errno));
          return(-6);
        }
      close(j);
      (void) chmod(tmp_file, (mode_t)(S_IRUSR | S_IWUSR));
      return(0);
    }

  /* If user does not have own lock_file in their directory, create it.. */
  if (access(tmp_file, F_OK) == -1)
    {
      j = open(tmp_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (j < 0)
        {
          syslog(LOG_WARNING, "%s Failed to create file %s! --%s", fnct_name,
                 tmp_file, strerror(errno));
          return(-7);
        }
      close(j);
      (void) chmod(tmp_file, (mode_t)(S_IRUSR | S_IWUSR));
    }

  return(0);
}


/*****************************************************************************/

static int  mod_msgbb_appuser( int          is_add,
                               const char  *msg_dir,
                               const char  *app,
                               const char  *mod_user,
                               int          is_admin )

{
  const char  *fnct_name = "mod_msgbb_appuser():";
  int    i;
  int    lock_id;
  char   bb_name[MAX_BB_NAME_LEN + 1];
  char   datafile[MAX_FILENAME_LEN];
  char   item[MAX_ITEM_LEN];
  char   tmp_file[MAX_FILENAME_LEN];
  char   user[MAX_LOGIN_NAME_LEN + 1];
  FILE  *fp;
  FILE  *rp;

  i = returnLockId(0, SUBSCRIBERS_TABLE_ID);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get lock ID!", fnct_name);
      return(-1);
    }

  lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to set DB write lock!", fnct_name);
      return(-2);
    }

  returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, msg_dir);
  sprintf(tmp_file, "%s.tmp", datafile);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name, tmp_file);
      unsetWriteDBLock(lock_id);
      return(-3);
    }

  rp = fopen(datafile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name, datafile);
      unsetWriteDBLock(lock_id);
      fclose(fp);
      return(-4);
    }

  while (fgets(item, MAX_ITEM_LEN, rp) != NULL)
    {
      stripNewline(item);
      ncpy(user, item, MAX_LOGIN_NAME_LEN);
      trim(user);
      strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
      trim(bb_name);
      if (!((strcmp(user, mod_user) == 0) &&
            (getBulBrdType(bb_name) == MSG_BB_TYPE_SYSTEM) &&
            (strncmp(bb_name, app, strlen(app)) == 0)))
        fprintf(fp, "%s\n", item);
    }

  if (is_add)
    {
      sprintf(item, "%%-%ds %%s.%%s", MAX_LOGIN_NAME_LEN);
      fprintf(fp, item, mod_user, app, "all");
      fprintf(fp, "\n");
      if (is_admin)
        {
          fprintf(fp, item, mod_user, app, "adm");
          fprintf(fp, "\n");
        }
    }

  fclose(rp);
  fclose(fp);
  (void) rename(tmp_file, datafile);
  (void) chmod(datafile, (mode_t)(S_IRUSR | S_IWUSR));
  unsetWriteDBLock(lock_id);

  return(0);
}


/*****************************************************************************/

static const char *alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";


/*****************************************************************************/
/* Replaces punctuation characters in a string with a random character. */
 
int  RemovePunct( char *str )

{
  int  i;
  int  seed;
  double  d;
  int  newidx;

  seed = 0;
  for (i = 0; (size_t)i < strlen(str); i++)
    seed += (int)(str[i]);

  srand48((long int)seed);

  for (i = 0; (size_t)i < strlen(str); i++)
    {
      if (ispunct((int)str[i]))
        {
          d = drand48();
          newidx = (int)((double)d*(double)strlen(alpha));
          if ((size_t)newidx >= strlen(alpha))
            newidx--;
          if (newidx < 0)
            newidx = 0;
          str[i] = alpha[newidx];
        }
    }

  return(0);
}


/*****************************************************************************/

DeviceDataItem  **getDeviceData( int         *num_devices,
                                 const char  *dir )

{

  /*  char  devfile[MAX_PRT_FILE_LEN];*/
  char  inbuf[MAX_PRT_FILE_LEN];
  /*  char  *ptr;*/
  int  n;
  FILE  *fp;
  DeviceDataItem  **Devices;


  Devices = (DeviceDataItem **) malloc(sizeof(char *)*MAX_NUM_DEVICES);

  fp = fopen(dir, "r");
  if (fp == NULL)
    return(NULL);

  *num_devices = 0;
  while (fgets(inbuf, MAX_PRT_FILE_LEN, fp))
    {
      if (inbuf[0] == '#')
        continue;

      inbuf[strlen(inbuf) - 1] = NTCSS_NULLCH;

      Devices[*num_devices] = (DeviceDataItem *)malloc(sizeof(DeviceDataItem));
      n = stripWord(0, inbuf, Devices[*num_devices]->name);
      n = stripWord(n, inbuf, Devices[*num_devices]->dir);

      strcpy(Devices[*num_devices]->file, &inbuf[n]);

      if (!strcmp(Devices[*num_devices]->name, "DRIVE"))
        Devices[*num_devices]->is_drive = 1;
      else
        Devices[*num_devices]->is_drive = 0;
      (*num_devices)++;
    }
  fclose(fp);
  return(Devices);
    
}


/*****************************************************************************/

void  freeDeviceData(Devices, num_devices)

     DeviceDataItem  **Devices;
     int  num_devices;

{
  int  i;

  for (i = 0; i < num_devices; i++)
    free((char *)Devices[i]);
  free((char *)Devices);
}


/*****************************************************************************/

int  IAmMaster( void )

{
  char strMasterHostName[MAX_HOST_NAME_LEN];
  int  nReturnValue;

  nReturnValue = get_master_server(NTCSS_DB_DIR, strMasterHostName);

  switch(nReturnValue) {
  case 0:
    return(FALSE);
    break;

  case 1:
    return(TRUE);
    break;

  default:
    return(ERROR_CANT_DETER_MASTER);
    break;
  } /* END switch */
    
}


/*****************************************************************************/

int  getUnixGroupForProgroup( const char  *db_dir,
                              const char  *progroup,
                              char        *unix_group)

{
  AppItem **Apps;
  int app_count;
  int app_index;
  int found = 0;
  int return_code = ERROR_NO_SUCH_GROUP;

  Apps = getAppData(&app_count, db_dir, WITH_NTCSS_GROUP);

  app_index = app_count;

  while (!found && (app_index >= 0))
    if (strcmp(Apps[--app_index]->progroup_title, progroup) == 0) 
      found = 1;

  if (found) {
    strcpy(unix_group, Apps[app_index]->unix_group);
    return_code = 0;
  }

  freeAppData(Apps, app_count);

  return(return_code);
}


/*****************************************************************************/
/* This procedure replaces the fields for the specified user in the
   ntcss_users database.  If the user doesn't already exist, they are added.
*/
/** We should add an additional parameter here as to the ntcss_users table's
    name, so that we can specify whether or not to use .tmp tables or not. **/

int  editNtcssUser( DBRecord    *record,   /* record containing user info */
                    const char  *db_dir,   /* path of the desired DB */
                    const char  *misc_str )

{
  int  users_index = -1;   /* marks index of preexisting record to update */
  int  i;
  int  num_users;
  int  x;   /* general purpose var */
  UserItem  **Users;
  char  app_passwd[MAX_APP_PASSWD_LEN+1];
  char  date[MAX_ITEM_LEN];
  char  unix_password[MAX_PASSWORD_LEN + 1];
  char  tempfile[MAX_FILENAME_LEN];
  FILE  *fp;

  /* Removes compiler warnings about unused var.. */
  misc_str = misc_str;

/* First, add user to database if they don't exist. */
  Users = getUserData(&num_users, db_dir);
  for (i = 0; i < num_users; i++) {
    if (strcmp(record->field[1].data, Users[i]->login_name) == 0)
      {
        Users[i]->marker = 0;                   /* User already exists */
        users_index = i;
      }
    else
      Users[i]->marker = 1;
  }

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[NTCSS_USERS_ID]); /*users*/
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "editNtcssUser: Unable to open file %s!",
             tempfile);
      freeUserData(Users, num_users);
      return(-1);  
    }

  writeUserData(Users, num_users, fp);     /* write all records marked 1. */

  /* Create the password change time field if none was specified */
  if (strcmp(record->field[7].data, "N/A") == 0)
    getTimeStr(date);
  else
    strcpy(date, record->field[7].data);

  /* Preserve the users password unless a new one is specified.. */
  if ((strcmp(record->field[3].data, "***************") == 0) &&
      (users_index >= 0) )                /* User exists */
    strcpy(unix_password, Users[users_index]->password);
  else                                       /* Preserve user's old password */
    strcpy(unix_password, record->field[3].data);

  if (users_index > -1)                     /* User exists.. */
    {
      if (record->sql_change == SQL_MOD_BATCH_NTCSS_USERS)
        {      
          /* re-encrypt the SSN field */    
          /*
            sprintf(key, "%s%s", Users[users_index]->login_name, 
            Users[users_index]->login_name);
            if (plainEncryptString(key, Users[users_index]->ss_number, 
            &encryptedSSN, 0, 0) != 0) {
            syslog(LOG_WARNING, "editNtcssUsers: Couldn't convert SSN!");
            return(-1);
            }
          */
     
          /* most of the info isn't set for batch users */
          fprintf(fp, NTCSS_USERS_FMT,
                  Users[users_index]->unix_id,
                  Users[users_index]->login_name,
                  record->field[2].data,                /* real name */
                  unix_password,
                  /* encryptedSSN, */
                  Users[users_index]->ss_number,
                  Users[users_index]->phone_number,
                  Users[users_index]->security_class,
                  Users[users_index]->pw_change_time,
                  0,                     /* pads the unused user role field */
                  Users[users_index]->user_lock,
                  Users[users_index]->shared_passwd,
                  Users[users_index]->auth_server);

          /* free(encryptedSSN); */
        }
      else    /* Exists, but is not a batch user */
        {
          /*
            sprintf(key, "%s%s", record->field[1].data, record->field[1].data);
            if (plainEncryptString(key, record->field[4].data, &encryptedSSN, 0, 0)
            != 0) {
            syslog(LOG_WARNING, "editNtcssUser: SSN conversion failed!");
            return (-1);
            }
          */

          fprintf(fp, NTCSS_USERS_FMT,
                  atoi(record->field[0].data), /* unix ID */
                  record->field[1].data,       /* login name */
                  record->field[2].data,       /* real name */
                  unix_password,
                  record->field[4].data,       /* ss # */
                  /* encryptedSSN, */                /* ss # */
                  record->field[5].data,       /*   phone # */
                  atoi(record->field[6].data), /* secutiy class */ 
                  date,                        /* last time pw changed */
                  0,                      /* pads the unused user role field */
                  Users[users_index]->user_lock,
                  Users[users_index]->shared_passwd,
                  record->field[9].data);     /* auth_server */

          /* free(encryptedSSN); */
        }
    }

  if (users_index == -1)   /* User not found, so add them.. */
    {
      getFullTimeStr(tempfile);
      getRandomString(app_passwd, MAX_APP_PASSWD_LEN, 3, &tempfile[5],
                      record->field[1].data, record->field[2].data);
      x = RemovePunct(app_passwd);

      /*
      sprintf(key, "%s%s", record->field[1].data, record->field[1].data);
      if (plainEncryptString(key, record->field[4].data, &encryptedSSN, 0, 0)
          != 0) {
         syslog(LOG_WARNING, "editNtcssUser: SSN conversion failed!");
         return (-1);
      }
      */

      fprintf(fp, NTCSS_USERS_FMT,
              atoi(record->field[0].data),  /* unix_id */
              record->field[1].data,        /* login_name */
              record->field[2].data,        /* real_name */
              unix_password,
              record->field[4].data,        /* ssn # */
              /* encryptedSSN, */                 /* ssn# */
              record->field[5].data,        /* phone number */
              atoi(record->field[6].data),  /* security class */
              date,                         /* passwd change time */
              0,                          /* just to pad the user role field */
              0,                            /* user_lock */
              app_passwd,
              record->field[9].data);       /* auth_server */
      /* free(encryptedSSN); */
    }

  freeUserData(Users, num_users);
  closeAndEncrypt(fp, tempfile);

/* * Add/modify user in message bulletin boards databases. */
  sprintf(tempfile, "%s/%s", db_dir, DB_TO_MSG_BB_DIR);
  x = mod_msgbb_user(1, tempfile, record->field[1].data,
                     atoi(record->field[8].data));
  if (x < 0)
    {
      syslog(LOG_WARNING, "editNtcssUser: Failed to modify user %s in the"
             " msg bbs! (%i)", record->field[1].data, x);
      return(1);
    }

  return(0);
}


/*****************************************************************************/
/* Finds the user's login name and updates his/her real name in a temp copy of the app_users database. */

int  updateAppUserName( const char  *login_name,
                        const char  *real_name,
                        const char  *db_dir )

{
  char  tempfile[MAX_FILENAME_LEN];
  int  i;
  int  num_appusers;
  AppuserItem  **Appusers;
  FILE  *fp;

  /* New for version 3.0.0: This should not be used any longer. */
  syslog(LOG_WARNING, "updateAppUserName: This function was called and "
         "should not be used any longer!");
  return(-1);

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  for (i = 0; i < num_appusers; i++)
    if (strcmp(login_name, Appusers[i]->login_name) == 0)
      Appusers[i]->marker = 0;          /* update this entry */
    else
      Appusers[i]->marker = 1;                 /* don't change this entry */

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "updateAppUserName: Unable to open file %s!",
             tempfile);
      freeAppuserData(Appusers, num_appusers);
      return(-1);  
    }

           /* Write all the non-changed user entries to the temp file.. */
  writeAppuserData(Appusers, num_appusers, fp);

          /* Write out all entries for the specified user.. */
  for (i = 0; i < num_appusers; i++)
    {
      if (Appusers[i]->marker == 0)
        fprintf(fp, APPUSERS_FMT,
                Appusers[i]->progroup_title,
                login_name,
                real_name,
                Appusers[i]->app_data,
                Appusers[i]->app_passwd,
                Appusers[i]->app_role,
                Appusers[i]->registered_user);
    }

   closeAndEncrypt(fp, tempfile);
   freeAppuserData(Appusers, num_appusers);
   return(0);
}


/*****************************************************************************/

int  delNtcssUserByLoginName( const char  *strLoginName,
                              int          nMoveTmpFilesOnLine )

{
  DBRecord dbRec;
  int      nReturn = -1;
  
  /* NOTE: The function delNtcssUser expects a DBRecord with ONLY
     the login name filled in the 1ST field.  The DBRecord is NOT
     a full record as read from the ntcss_users data file. */
  sprintf(dbRec.field[0].data, "%s", strLoginName);
  nReturn = delNtcssUser(&dbRec, NTCSS_DB_DIR);
  if (nReturn != 0) {
    syslog(LOG_WARNING, "authorize_user_login: delNtcssUser failed for user %s",
       strLoginName);
  } /* END if(nError) */

  if (nMoveTmpFilesOnLine) {
    if (copy_database_files(NTCSS_DB_DIR, MOVE_EXISTING_TMP_FILES_ONLINE) != 1) {
      nReturn = -1;
    } /* END if(copy_database_files) */
  } /* END if */

  return(nReturn);
}


/*****************************************************************************/
/* Deletes a user from the ntcss_users, app_users, and bulletin board 
   databases.
*/

int  delNtcssUser( DBRecord   *record,      /* Record containing user info. */
                   const char *db_dir )

{
  UserItem  **Users;
  char  tempfile[MAX_FILENAME_LEN];
  int  i;
  int  num_users;                /* # records in the ntcss_users database */
  FILE  *fp;
  int  num_appusers;
  AppuserItem  **Appusers;

  Users = getUserData(&num_users, db_dir);
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  /* first delete user from ntcss_users database .... */
  for (i = 0; i < num_users; i++)
    {
      if (strcmp(record->field[0].data, Users[i]->login_name) == 0)
        Users[i]->marker = 0;
      else
        Users[i]->marker = 1;
    }

  /* next delete user from appusers database .... */
  for (i = 0; i < num_appusers; i++)
    {
      if (strcmp(record->field[0].data, Appusers[i]->login_name) == 0)
        Appusers[i]->marker = 0;
      else
        Appusers[i]->marker = 1;
    }

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[NTCSS_USERS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "delNtcssUser: Unable to open temp db file %s!",
             tempfile);
      freeUserData(Users, num_users);
      return(-1);
    }

  writeUserData(Users, num_users, fp); /* skips over this user's records.. */
  closeAndEncrypt(fp, tempfile);
  freeUserData(Users, num_users);

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "delNtcssUser: Unable to open temp db file %s!",
             tempfile);
      freeAppuserData(Appusers, num_appusers);
      return(-1);
    }

  writeAppuserData(Appusers, num_appusers, fp);
  closeAndEncrypt(fp, tempfile);
  freeAppuserData(Appusers, num_appusers);

  /** Delete all records for this user from message bulletin boards. */
  sprintf(tempfile, "%s/%s", db_dir, DB_TO_MSG_BB_DIR);
  i = mod_msgbb_user(0, tempfile, record->field[0].data, 0);
  if (i < 0)
    {
      syslog(LOG_WARNING, "delNtcssUser: Failed to delete user %s from"
             " msg bbs! (%i)", record->field[0].data, i);
      return(-4);
    }

  return(0);
}


/*****************************************************************************/
/* Modifies App user records in l temp
 */

int  modAppUsers( DBRecord    *record,
                  const char  *db_dir )

{
  const char  *fnct_name = "modAppUsers():";
  AppuserItem  **Appusers;
  FILE  *fp;
  char  app_passwd[MAX_APP_PASSWD_LEN+1];
  char  date[MAX_ITEM_LEN];
  char  tempfile[MAX_FILENAME_LEN];
  int  app_role = 0;            /* new app_role to be written to database */
  int  i, j;
  int  num_appusers = 0;


/*
 * Modify fields in the appusers database for requested user/app.
 */

  j = -1;  /* index of progroup entry to change */

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  for (i = 0; i < num_appusers; i++)
    {
      if (strcmp(record->field[1].data, Appusers[i]->login_name) == 0)
        {
          if (strcmp(record->field[0].data,
                     Appusers[i]->progroup_title) == 0)
            {
              j = i;
              Appusers[i]->marker = 0;  /* this is the entry to update. */
            }
          else  /* wrong user, mark this record for no-changes. */
            Appusers[i]->marker = 1;
        }
    }

   /* app_role is considered to contain all the desired roles at this point. */

  app_role = atoi(record->field[3].data);  /*store any new role(s) assigned. */

  /* if this batchuser is already a member of this progroup...
     if ((j > -1) && (record->sql_change == SQL_MOD_BATCH_APPUSERS))
     app_role |= Appusers[j]->app_role;        Add pre-existing roles too.
  */

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             tempfile);
      freeAppuserData(Appusers, num_appusers);
      return(-1);
    }
  /* Write out all the non-changed entries to temp file..*/
  writeAppuserData(Appusers, num_appusers, fp);

  /* Now write out any changed or new entries... */
  if (j == -1)      /* appuser record not foundfor this user, add a new one..*/
    {
      getFullTimeStr(date);  /* have to create new app_password.. */
      getRandomString(app_passwd, MAX_APP_PASSWD_LEN, 3, &date[5],
                      record->field[1].data, record->field[0].data);
      (void) RemovePunct(app_passwd);
      fprintf(fp, APPUSERS_FMT,
              record->field[0].data,  /* progroup title */
              record->field[1].data,  /* login_name */
              record->field[2].data,  /* real_name */
              "",                     /* app_data */
              app_passwd,
              app_role,
              0);                     /* registered_user */
    }
  else
    fprintf(fp, APPUSERS_FMT,     /* Pre-existing record for this user */
            record->field[0].data,  /* progroup title */
            record->field[1].data,  /* login_name */
            record->field[2].data,  /* real_name */
            Appusers[j]->app_data,
            Appusers[j]->app_passwd,
            app_role,
            Appusers[j]->registered_user);

  closeAndEncrypt(fp,tempfile);
  freeAppuserData(Appusers, num_appusers);

  /* Add user to application message bulletin board. Non-crucial. */
  sprintf(tempfile, "%s/%s", db_dir, DB_TO_MSG_BB_DIR);
  i = mod_msgbb_appuser(1, tempfile, record->field[0].data,
                        record->field[1].data,
                        atoi(record->field[3].data) & 1);
  if (i != 0)
    syslog(LOG_WARNING, "%s Failed to add user %s to %s msg BB! (%i)",
           fnct_name, record->field[1].data, record->field[0].data, i);

  return(0);  /* all ok */
}


/*****************************************************************************/
/* Deletes a user's membership from the specified progroup/app.  This is done
   by removing the entry in the appusers db, and all user_auth records under
   the specified progroup for this user.  If the user is registered, the
   operation is aborted.
*/

int  delAppUser( DBRecord    *record,
                 const char  *db_dir )

{
  const char  *fnct_name = "delAppUser():";
  AppuserItem  **Appusers;
  FILE  *fp;
  int  i;
  int  num_appusers;
  char  tempfile[MAX_FILENAME_LEN];
  int   f_registered_user;

/* Delete the appuser DB entry for this user. */
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  f_registered_user = 0;
  for (i = 0; i < num_appusers; i++)
    {
      if (strcmp(record->field[1].data, Appusers[i]->login_name) == 0)
        {
          if (strcmp(record->field[0].data, Appusers[i]->progroup_title) == 0)
	    {
	      if (Appusers[i]->registered_user != 0)
		{
		  f_registered_user = 1; /* Signify this illegal action.. */
		  break;                 /* ..stop what we're doing.. */
		}
	      else
		Appusers[i]->marker = 0;         /* Mark entry to delete */
	    }
          else
            Appusers[i]->marker = 1;               /* No change */
        }
    }

  /* Since trying to delete a registered user is bad...... */
  if (f_registered_user != 0)
    {
      syslog(LOG_WARNING, "%s Attempt made to delete registered user %s!",
	     fnct_name, record->field[0].data);
      freeAppuserData(Appusers, num_appusers);
      /* DO NOT change/duplicate this return value - it is looked for in
         in the userAdminFcts.c code in unix_ini */
      return(-2);
    }

  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, tempfile);
      freeAppuserData(Appusers, num_appusers);
      return(-1);
    }

  /* By writing out all apps sans the specified one, we virtualy delete the
     appuser record for this user...
  */
  writeAppuserData(Appusers, num_appusers, fp);
  freeAppuserData(Appusers, num_appusers);

  closeAndEncrypt(fp, tempfile);


  /* Delete user from application message bulletin board. */
  sprintf(tempfile, "%s/%s", db_dir, DB_TO_MSG_BB_DIR);
  i = mod_msgbb_appuser(0, tempfile, record->field[0].data,
                        record->field[1].data, 0);
  if (i < 0)
    syslog(LOG_WARNING, "%s Failed to delete user %s from %s msg BB!",
           fnct_name, record->field[1].data, record->field[0].data);

  return(0);
}


/*****************************************************************************/

/* Takes the given program group and adds it if it doesn't exist, or updates
 the current entry if the progroup already exists.  */
/* Link data & process limits are preserved if the progroup already exists.
  If progroup also exists in the progroups.old file, it's link_data and
 process limits are used instead of the other. */

int  modProgramGroup( DBRecord   *record,
                      const char *db_dir )

{
  AppItem   **Apps;
  DBRecord  tmp_record;
  FILE  *fp;
  int  i;
  int  num_apps;
  int  old_proc_limit = 10;
  char  datafile[MAX_FILENAME_LEN];
  char  tmp_link_data[MAX_APP_PASSWD_LEN+1];
  char decryptfile[MAX_FILENAME_LEN];

/* if progroup already exists, preserve process limit # and link_data */
  tmp_link_data[0] = '\0';
  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(record->field[0].data, Apps[i]->progroup_title) == 0)
        {
          old_proc_limit = Apps[i]->num_processes;
          strcpy(tmp_link_data, Apps[i]->link_data);
          Apps[i]->marker = 0;                          /* update this entry */
        }
      else
        Apps[i]->marker = 1;
    }

/* look in the .old file and get old link_data and process limit #.
   These values over-ride the ones in the current db. */
  sprintf(datafile, "%s/%s.old", db_dir, tables[PROGROUPS_ID]);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "modProgramGroup: Unable to open file %s!",
             datafile);
      freeAppData(Apps, num_apps);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(-1);
    }

  while (getRecord(&tmp_record, fp, PROGROUPS_NUM_RECS))
    {
      if (strcmp(tmp_record.field[0].data, record->field[0].data) == 0)
        {
          old_proc_limit = atoi(tmp_record.field[9].data); /* num_processes */
          strcpy(tmp_link_data, tmp_record.field[10].data);
          break;
        }
    }
  fclose(fp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PROGROUPS_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "modProgramGroup: Unable to open file %s!",
             datafile);
      freeAppData(Apps, num_apps);
      return(-2);
    }

  writeAppData(Apps, num_apps, fp);
  convFromSql(record, 9);
  fprintf(fp, PROGROUPS_FMT,
          record->field[0].data, record->field[1].data, record->field[2].data,
          atoi(record->field[3].data), record->field[4].data,
          record->field[5].data, record->field[6].data, record->field[7].data,
          record->field[8].data,
          old_proc_limit,
          tmp_link_data);

  closeAndEncrypt(fp, datafile);
  freeAppData(Apps, num_apps);
  return(0);
}


/*****************************************************************************/

/* Creates an array of all the records in the access_roles table. */

AccessRoleItem  **getAccessRoleData(num_roles, dir)

     int  *num_roles;
     const char  *dir;

{
  AccessRoleItem  **AccessRoles;
  DBRecord          record;
  char   decryptfile[MAX_FILENAME_LEN];
  char   filename[MAX_FILENAME_LEN];
  const char  *fnct_name = "getAccessRoleData():";
  FILE  *rp;

  *num_roles = -1;

  if (using_tmp)                          /* using temp file for source data */
    sprintf(filename, "%s/access_roles.tmp", dir);
  else
    sprintf(filename, "%s/access_roles", dir);

  rp = decryptAndOpen(filename, decryptfile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, filename);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return((AccessRoleItem **) 0);
    }

  AccessRoles = (AccessRoleItem **)
                   malloc(sizeof(char *)*MAX_NUM_NTCSS_ROLES);

  *num_roles = 0;
  while (getRecord(&record, rp, ACCESS_ROLES_NUM_RECS) &&
         *num_roles < MAX_NUM_NTCSS_ROLES)
    {
      AccessRoles[*num_roles] = (AccessRoleItem *) malloc(sizeof(AccessRoleItem));
      AccessRoles[*num_roles]->marker = 1;
      AccessRoles[*num_roles]->progroup_title = allocAndCopyString(record.field[0].data);
      AccessRoles[*num_roles]->app_role = allocAndCopyString(record.field[1].data);
      AccessRoles[*num_roles]->role_number = atoi(record.field[2].data);
      AccessRoles[*num_roles]->locked_role_override = atoi(record.field[3].data);
      (*num_roles)++;
    }

  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  return(AccessRoles);
}


/*****************************************************************************/
/* Frees AccessRoleItem arrays. */

void  freeAccessRoleData(AccessRoles, num_roles)

     AccessRoleItem  **AccessRoles;
     int  num_roles;

{
  int i;
  
  for (i = num_roles - 1; i >= 0; i--)
    {
      free(AccessRoles[i]->app_role);
      free(AccessRoles[i]->progroup_title);
      free((char *) AccessRoles[i]);
    }
  free((char *) AccessRoles);
}


/*****************************************************************************/
/* This function was mainly written for ConvertUsersTo3_0_0(), but is generic
   enough to be used elsewhere. */
/* Given a login name, progroup, and role index, it masks in the new role. */

int  changeAppUserArrayRole( const char  *login_name,
                             const char  *progroup_title,
                             int          offset,
                             AppuserItem *Appusers[],
                             int          num_appusers)
{
  int  i;
  int  mask;

  mask = ( 1 << (offset + 1) );  /* create new access role bit mask */

  for (i = 0; i < num_appusers; i++)
    if ( (strcmp(Appusers[i]->login_name, login_name) == 0) &&
         (strcmp(Appusers[i]->progroup_title, progroup_title) == 0) )
      {
        Appusers[i]->app_role |= mask;
        return(0);
      }

  return(1); /* didn't find the appuser */
}


/*****************************************************************************/
/* This function was written to duplicate the stripNewLine() function in
   strlibs library.  If available, the strlibs version should be used. */
/* This one should only be used by other linked files if linking the strlib
   library causes problems. */
/*
 * Parameters:  str - input string
 * Description: Removes one newline character from the end of 'str' if it
 *              has one.
 */

void  manipStripNewLine(str)

     char *str;
{
  int leng;

  leng = strlen(str);
  if (str[leng - 1] == '\n')
    str[leng - 1] = '\0';
}


/*****************************************************************************/
/* Calls a Unix script to create a new Unix account for a user.
   The password passed in should just be the plain text of the password.
 */

int  CreateUnixAccount( const char  *login_name,
                        const char  *plain_password,
                        const char  *home_dir_prefix, 
                        const char  *full_name,
                        const char  *auth_server )
{
  const char  *fnct_name = "CreateUnixAccount():";
  char  *crypted_password;
  char   error_msg[MAX_ERR_MSG_LEN];
  char system_cmd[MAX_PATH_LEN +         /* path to scripts program */
                 10 +                    /* command line switch */
                 MAX_LOGIN_NAME_LEN +    /* login name */
                 MAX_DES_CRYPTED_PASSWD_LEN +   /* 3Des encrypted passwd */
                 MAX_PATH_LEN +          /* user's home directory path */
                 MAX_REAL_NAME_LEN +     /* user's real name */
                 MAX_HOST_NAME_LEN +     /* auth server */
                 10];                    /* spaces between fields */
  int    script_return;
  int    x;
  FILE  *cmd_pipe;

  /* Encrypt the plain text in Unix style for the script call.. */
  crypted_password = crypt(plain_password, plain_password);

  /* The plain password is included for adding the user to any NT machines.
     Since the command line is so long, it is assumed that no one will see the
     plain text password at the end due to screen widths when doing a ps- ef */
  sprintf(system_cmd, "%s ADDUSER %s \"%s\" %s \"%s\" %s %s 2>&1",
          NTCSS_CHANGE_USER_PROGRAM, login_name, crypted_password,
          home_dir_prefix, full_name, "DUMMY_ARG" /*plain_password*/,
          auth_server);

/*syslog(LOG_WARNING, "%s DEBUG cmd_line = %s", fnct_name, system_cmd);*/
  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM);
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))           /* Process terminated abnormally. */
    {
      syslog(LOG_WARNING, "%s Unknown Error occured!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);            /* Get script's return value. */
  if (x != 0)                                /* Should be a postive integer. */
    {
      syslog(LOG_WARNING, "%s Script could not create an account for <%s>! "
             "Script returned error %i", fnct_name, login_name, x);
    }

  return(x);
}


/*****************************************************************************/
/* Calls a script to change the user's Unix password.
   Note that the password is expected to be encrypted via NtcssEncryptMemory()
   using the key in the decryption call below. */

int ChangeUnixPassword( const char  *login_name,
                        const char  *plain_password )

{
  const char  *fnct_name = "ChangeUnixPassword():";
  char system_cmd[MAX_PATH_LEN +         /* path to scripts program */
                 10 +                    /* command line switch */
                 MAX_LOGIN_NAME_LEN +    /* login name */
                 MAX_DES_CRYPTED_PASSWD_LEN + /* oversized subst for crypted */
                 MAX_PASSWORD_LEN +      /* plain text passwrod */
                 10];                     /* for spaces between fields */
  char  *crypted_password;
  /*char   error_msg[MAX_ERR_MSG_LEN];*/
  int    script_return;
  int    x;
  /*FILE  *cmd_pipe;*/

  crypted_password = crypt(plain_password, plain_password);

  /* The plain password is included for adding the user to any NT machines.
     This is a security breach if someone does a ps- ef while the script is
     running, unless the script command line params are modified.  */

  sprintf(system_cmd, "%s CHANGEPASSWD %s %s %s"       /* 2>&1",*/,
          NTCSS_CHANGE_USER_PROGRAM, login_name,
          "PLACE_HOLDER" /* <-was plain_password */, crypted_password);

  /* 
     05-12-99 GWY
     Commented out popen implementation and used a system call to execute the
     change password script.  The pclose was not getting the correct script
     return value occasionally.
     08-06-99 DWB
     Restored the pipe features and tried various intentional script failures.
     No unusual behavious discovered.  Previous problem might have occured
     before the user config script's return values were modified.
  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM);
      return(-1);
    }


  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);
  */

  script_return = system(system_cmd);

/*
  if (!WIFEXITED(script_return)) 
    {
      syslog(LOG_WARNING, "%s Unknown Error occured! (%i)", fnct_name,
             script_return);
      return(-2);
    }
*/

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not change password for <%s>!"
             "  Script returned %i.", fnct_name, login_name, x);
      if (x == 127)         /* Specific to pclose. */
        syslog(LOG_WARNING, "%s Could not run the script!", fnct_name);
    }

  return(x);
}


/*****************************************************************************/
/* Removes a user's Unix acount. Mail account *may* be left behind. */

int  RemoveUnixAccount( const char  *login_name )

{
  const char  *fnct_name = "RemoveUnixAccount():";
  char   error_msg[MAX_ERR_MSG_LEN];
  char   system_cmd[MAX_PATH_LEN +          /* path to scripts program */
                    10 +                    /* command line switch */
                    MAX_LOGIN_NAME_LEN +    /* login name */
                    6];                     /* spaces between fields */
  int  script_return;
  int  x;
  FILE  *cmd_pipe;

  sprintf(system_cmd, "%s DELUSER %s 2>&1", NTCSS_CHANGE_USER_PROGRAM,
          login_name);
  /*syslog(LOG_WARNING, "%s DEBUG cmd_line = %s", fnct_name, system_cmd);*/

  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM );
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))            /* Process terminated abnormally */
    {
      syslog(LOG_WARNING, "%s Unknown error!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not delete  account for <%s>! "
             "Script returned %i", fnct_name, login_name, x);
    }

  return(x);
}


/*****************************************************************************/
/* Adds a user to Unix Group */

int  AddUserToUnixGroup( const char  *login_name,
                         const char  *group_name )

{
  const char  *fnct_name = "AddUserToUnixGroup():";
  char  error_msg[MAX_ERR_MSG_LEN];
  char  system_cmd[MAX_PATH_LEN +           /* path to scripts program */
                   10 +                     /* command line switch */
                   MAX_LOGIN_NAME_LEN  +    /* login name */
                   MAX_PROGROUP_TITLE_LEN + /* (guessed) max group name len */
                   6];                      /* spaces between fields */
  int  script_return;
  int  x;
  FILE  *cmd_pipe;

  sprintf(system_cmd, "%s ADDGROUP %s %s 2>&1", NTCSS_CHANGE_USER_PROGRAM,
          login_name, group_name);

  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM);
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))            /* Process terminated abnormally */
    {
      syslog(LOG_WARNING, "%s Unknown Error!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not add  user %s to group %s! "
             "Script returned %i.", fnct_name, login_name, group_name, x);
    }

  return(x);
}


/*****************************************************************************/

int  RemoveUserFromUnixGroup( const char  *login_name,
                              const char  *group_name )

{
  const char  *fnct_name = "RemoveUserFromUnixGroup():";
  char  error_msg[MAX_ERR_MSG_LEN];
  char  system_cmd[MAX_PATH_LEN +           /* path to scripts program */
                   10 +                     /* command line switch */
                   MAX_LOGIN_NAME_LEN +     /* login name */
                   MAX_PROGROUP_TITLE_LEN + /* (guessed) max group name len */
                   6];                      /* spaces between fields */
  int   script_return;
  int   x;
  FILE  *cmd_pipe;

  sprintf(system_cmd, "%s DELGROUP %s %s 2>&1", NTCSS_CHANGE_USER_PROGRAM,
          login_name, group_name);
/*syslog(LOG_WARNING, "RemoveUserFromUnixGroup: DEBUG cmd_line = %s",system_cmd);*/

  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM );
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))         /* Did process terminated normally? */
    {
      syslog(LOG_WARNING, "%s Unknown Error!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not remove %s from group %s! "
             "Script returned %i", fnct_name, group_name, login_name, x);
    }

  return(x);
}


/*****************************************************************************/
/* Calls a Unix script to create a new Unix account for an Ntcss batch user.
 * The password parameter is not really needed, but only to see if the batch
    user has one or not.
   The script that is called here automatically adds the batch user to its
    proper group, so a separate call is not needed to establish that.
 */

int  CreateUnixBatchUserAccount( const char  *login_name,
                                 const char  *plain_password,
                                 const char  *home_dir_prefix,
                                 const char  *full_name,
                                 const char  *group_name )
{
  const char  *fnct_name = "CreateUnixBatchUserAccount():";
  char   error_msg[MAX_ERR_MSG_LEN];
  char   system_cmd[MAX_PATH_LEN +          /* path to scripts program */
                    10 +                     /* command line switch */
                    MAX_LOGIN_NAME_LEN +     /* login name */
                  MAX_DES_CRYPTED_PASSWD_LEN + /*oversized subst for crypted */
                    MAX_PATH_LEN +           /* user's home directory path */
                    MAX_REAL_NAME_LEN +      /* user's real name */
                    MAX_PROGROUP_TITLE_LEN + /* (guessed) max group name len */
                    7];                      /* spaces between fields */
  int    script_return;
  int    x;
  FILE  *cmd_pipe;

  /* The plain password is included for adding the user to any NT machines.
     Since the command line is so long, it is assumed that no one will see the
     plain text password at the end due to screen widths when doing a ps- ef */

  sprintf(system_cmd, "%s ADDBATCHUSER %s %s %s \"%s\" %s 2>&1",
          NTCSS_CHANGE_USER_PROGRAM, login_name, plain_password,
          home_dir_prefix, full_name, group_name);
/*syslog(LOG_WARNING, "CreateUnixBatchUserAccount: DEBUG cmd_line = %s", system_cmd);*/
  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM );
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))            /* Process terminated abnormally */
    {
      syslog(LOG_WARNING, "%s System call failed!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not create an account for <%s>! "
             "Script returned %i", fnct_name, login_name, x);
    }

  return(x);
}

/*****************************************************************************/
/* This function updates/adds the gecos field for a UNIX user's password entry.
   This text field is usually just the user's real name and other misc. info.
*/

int  EditUnixGecosField( const char  *login_name,
                         const char  *gecos_field )

{
  const char  *fnct_name = "EditUnixGecosField():";
  char system_cmd[MAX_PATH_LEN +          /* path to scripts program */
                 10 +                     /* command line switch */
                 MAX_LOGIN_NAME_LEN +     /* login name */
                 MAX_REAL_NAME_LEN +      /* user's real name */
                 7];                      /* spaces between fields */
  char   error_msg[MAX_ERR_MSG_LEN];
  int    script_return;
  int    x;
  FILE  *cmd_pipe;

  sprintf(system_cmd, "%s ADDGECOS %s \"%s\" 2>&1", NTCSS_CHANGE_USER_PROGRAM,
          login_name, gecos_field);

  cmd_pipe = popen(system_cmd, "r");
  if (cmd_pipe == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to run %s!", fnct_name,
             NTCSS_CHANGE_USER_PROGRAM );
      return(-1);
    }

  /* Report any output from the script.. */
  while (fgets(error_msg, MAX_ERR_MSG_LEN, cmd_pipe) != NULL)
    syslog(LOG_WARNING, "%s Script reports: %s", fnct_name, error_msg);

  script_return = pclose(cmd_pipe);

  if (!WIFEXITED(script_return))            /* Process terminated abnormally */
    {
      syslog(LOG_WARNING, "%s System call failed!", fnct_name);
      return(-2);
    }

  x = WEXITSTATUS(script_return);  /* get script's return value */
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Script could not edit the UNIX GECOS field for "
             "<%s>! Script returned %i", fnct_name, login_name, x);
    }

  return(x);
}

/*****************************************************************************/
/*  Replaces an App program's information, or adds it if it doesn't pre-exist.
    This function expects a .sql record for its data.
*/

int  editNtcssProgram(  DBRecord    *record,  /* record containing user info */
                        const char  *db_dir ) /* path of the desired DB */

{
  int    i;
  int    num_programs;
  char   datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  ProgramItem  **Programs;

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PROGRAMS_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "editNtcssProgram: Unable to open SQL"
             " file %s!", datafile);
      return(-1);
    }

  Programs = getProgramData(&num_programs, db_dir, WITH_NTCSS_GROUP);
  for (i = 0; i < num_programs; i++)
    if ( (strcmp(record->field[0].data, Programs[i]->program_title) == 0) &&
         (strcmp(record->field[5].data, Programs[i]->progroup_title) == 0) )
      Programs[i]->marker = 0;       /* Prog exists, replace this entry. */
    else
      Programs[i]->marker = 1;

  writeProgramData(Programs, num_programs, fp);
  freeProgramData(Programs, num_programs);

  convFromSql(record, 10);
  fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
              record->field[0].data,  /* program title */
              record->field[1].data,  /* progroup title */
              record->field[2].data,  /* program file name */
              record->field[3].data,  /* icon file */
              record->field[4].data,  /* icon file index */
              record->field[5].data,  /* command line arguments */
              record->field[6].data,  /* security class */
              record->field[7].data,  /* program access bitmask */
              record->field[8].data,  /* program type */
              record->field[9].data); /* program working directory */
  closeAndEncrypt(fp, datafile);

  return(0);
}


/*****************************************************************************/
/* This function fills in a provided array with the names of all the auth
   servers and the number of users on each one.
*/

int  CalculateServerUserLoads( UserItem  **Users,                  /* Input */
                               int         num_users,              /* Input */
                               userLoadStruct  *user_loads,        /* Output */
                               int        *num_auth_servers)       /* Output */

{
  int  i = 0;
  int  j = 0; /* intialized in case below loop doesn't run */
  int  x;
  int  nLocalUsers;
  UserItem **pLocalUsers;

  if (Users == NULL)
    {
      pLocalUsers = getUserData(&nLocalUsers, NTCSS_DB_DIR);
      if (nLocalUsers < 0)
        {
          syslog(LOG_WARNING, "CalculateServerUserLoads: getUserData failed");
          return(-1);
        }
    }
  else
    {
      pLocalUsers = Users;
      nLocalUsers = num_users;  
    }
  
  if (nLocalUsers > 0)
    {
      /* Initialize the first auth server entry.. */
      sprintf(user_loads[0].hostname, "%s", pLocalUsers[0]->auth_server);
      user_loads[0].num_users = 1;   /* Init new value. */
      *num_auth_servers = 1;
    }
  else
    {
      *num_auth_servers = 0;        /* Extremely rare case. */
      if (Users == NULL)
        freeUserData(pLocalUsers, nLocalUsers);
      return(-2);
    }

  /* We start at one for index i because we already acounted for user 0 when
     we initialized the array above. */
  for (i = 1; i < nLocalUsers; i++)  /* For every user.. */
    {
      /* look for this user's auth server in the server array.. */
      for (j = 0; j < *num_auth_servers; j++)
        {
          x = strcmp(pLocalUsers[i]->auth_server, user_loads[j].hostname);
          if (x == 0)
            {                           /* Found a user on this auth server. */
              user_loads[j].num_users++;
              break;
            }
        }

      /* Check for invalid auth server field.. */
      if (pLocalUsers[i]->auth_server[0] == '\0')
        {
          syslog(LOG_WARNING, "CalculateServerUserLoads: No auth server "
                 " specified for user %s!", pLocalUsers[i]->login_name);
          continue;
        }

      if (j == *num_auth_servers)  /* This auth server was not found.. */
        {
          /* ..so we'll add it to the list. */
          sprintf(user_loads[*num_auth_servers].hostname, "%s",
                  pLocalUsers[i]->auth_server);
          user_loads[*num_auth_servers].num_users = 1;   /* Init new value. */
          (*num_auth_servers)++;
        }
    }

  if (Users == NULL)
    freeUserData(pLocalUsers, nLocalUsers);

  return(0);
}


/*****************************************************************************/

int  initProgroupUseStruct( ProgroupUseStruct *pProgroupUseStruct,
                            int                nItems)
{
  int i;
  
  for (i = 0; i < nItems; i++) {
    memset(pProgroupUseStruct[i].strProgroup, '\0', MAX_PROGROUP_TITLE_LEN);
    pProgroupUseStruct[i].nUsersCurrentlyInProgroup  = 0;
  } /* END for */

  return(0);
}


/*****************************************************************************/

int addProgroupUseInfo(ProgroupUseStruct  *aProgroupUseStruct,
                       const char         *strInProgroup,
                       int                 nInProgroupUse,
                       int                 nMaxArrayElements )

{
  int i, nIndex, nReturn;

  /* Loop through the progroup use array.  If there isn't an entry for
     strInProgroup then append it to the end of the array.  If there is 
     an entry add the nInProgroupUse to it. 
  */
  nIndex = -1;
  for (i = 0; i < nMaxArrayElements; i++) {
    if (strcmp(aProgroupUseStruct[i].strProgroup, strInProgroup) == 0) {
      nIndex = i;
      break;
    } /* END if */
    
    if (strcmp(aProgroupUseStruct[i].strProgroup, "") == 0) {
      nIndex = i;
      break;
    } /* END if */
  } /* END for */

  if (nIndex < 0) {
    syslog(LOG_WARNING, "addProgroupUseInfo: Failed to find entry or empty space for <%s>",
       strInProgroup);
    nReturn = -1;
  } else {
    sprintf(aProgroupUseStruct[nIndex].strProgroup, strInProgroup);
    aProgroupUseStruct[nIndex].nUsersCurrentlyInProgroup += nInProgroupUse;
    nReturn = 0;
  } /* END if */
 
  return(nReturn);
 
}



/*****************************************************************************/
/* This function was stolen from the integrity checker.  When roles #s are
   changed around, this function searches for the new role number
   and corresponding old role number so that they can be referenced when
   updating other db's.
*/
      
int  InitializeRoleMap( int       role_map[MAX_NUM_PROGROUPS]
                                       [2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3],
                        AppItem     *Apps[],
                        int          num_apps,
                        const char  *db_dir)

{     
  AppItem    **OldApps;
  const char  *fnct_name = "InitializeRoleMap():";
  int    role_cnt;
  int    new_app_ndx;
  int    new_role_ndx;
  int    num_old_apps; 
  int    old_app_ndx;
  int    old_role_ndx;

  /* role_map: first index is for each progroup, 2'nd is for old db and new
     db, 3'rd is role_cnt bit positions in the old, the new db's,
     and a flag */

   /* Grab role data from .tmp/.old db files and store it in Apps structure */

  OldApps = getAppData(&num_old_apps, db_dir, WITH_NTCSS_AND_WITH_OLD_ROLES);

   if (OldApps == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to retrieve old application data!",
             fnct_name);
      return(-1);
    }


  for (new_app_ndx = 0; new_app_ndx < num_apps; new_app_ndx++)
    {


      /* First, clear all the entries.. */
      for (old_app_ndx = 0;
           old_app_ndx < (2 * MAX_NUM_APP_ROLES_PER_PROGROUP);
           old_app_ndx++)
        {
          role_map[new_app_ndx][old_app_ndx][0] = LAST_ACCESS_OP;
          role_map[new_app_ndx][old_app_ndx][1] = 0;
          role_map[new_app_ndx][old_app_ndx][2] = 0;
        }


      for (old_app_ndx = 0; old_app_ndx < num_old_apps; old_app_ndx++)
        {
          if (strcmp(Apps[new_app_ndx]->progroup_title,
                     OldApps[old_app_ndx]->progroup_title) != 0)
            continue;

syslog(LOG_WARNING,"InitializeRoleMap:processing <%s>",Apps[new_app_ndx]->progroup_title);

          /* For each old role, find the matching one in the current db..*/
          /* l & k represent the bit position of the role in the mask. */

      	  role_cnt = 0;
          for (old_role_ndx = 0;
               old_role_ndx < OldApps[old_app_ndx]->num_roles; old_role_ndx++)
            {
              for (new_role_ndx = 0;
                   new_role_ndx < Apps[new_app_ndx]->num_roles; new_role_ndx++)
                {
                  if (strcmp(OldApps[old_app_ndx]->roles[old_role_ndx],
                             Apps[new_app_ndx]->roles[new_role_ndx]) != 0)
                    continue;

syslog(LOG_WARNING,"InitializeRoleMap:processing rolename <%s>", Apps[new_app_ndx]->roles[new_role_ndx]);
syslog(LOG_WARNING,"InitializeRoleMap:processing role values old=<%d>,new=<%d>", old_role_ndx + 1,new_role_ndx + 1);
                  /* Record what the role num shifted to for this role */
                  role_map[new_app_ndx][role_cnt][0] = MOVE_ACCESS_OP;
                  role_map[new_app_ndx][role_cnt][1] = old_role_ndx + 1;
                  role_map[new_app_ndx][role_cnt][2] = new_role_ndx + 1;
                  role_cnt++;
                  break;  /* ..to next app role. */
                }
            }
          break;  /*.. to next old app */
        }
    }
  freeAppData(OldApps, num_old_apps);

  return(0);
}


/*****************************************************************************/
/*  Removes users from non-existant progroups and update app_role bits
    for them.
 * Bits representing roles may have gotten shifted or changed, so the
 * corresponding app_role bits in the appusers table needs to be updated.
 * This functions overwrites the original appusers table.
 */

int  ResolveAppUserAccessRoleNumShifts( const char   *db_dir,
                                        const char   *file_ext,
                                        int  role_map[MAX_NUM_PROGROUPS]
                                       [2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3],
                                        AppItem      *Apps[],
                                        int           num_apps,
                                        AppuserItem  *Appusers[],
                                        int           num_appusers,
                                        int           write_results )

{
  const char  *fnct_name = "ResolveAppUserAccessRoleNumShifts():";
  int   app_role;
  int   i, j, k;
  int   mask1, mask2;
  int   op;
  int   responsible_for_apps;
  int   responsible_for_appusers;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;


  /* If we're not writing results locally, what's the sense in getting the
     data locally?  */
  if ((write_results == 0) && (Appusers == NULL))
    return(-1);

  /* If caller did not pass in the Apps array, get it ourselves. */
  if (Apps == NULL)
    {
      Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);
      if (num_apps < 0)
        {
          syslog(LOG_WARNING, "%s Unable retrieve Apps data!", fnct_name);
          return(-2);
        }
      responsible_for_apps = 1;
    }
  else                            /* Apps data was passed in for us to use.. */
    {
      if (num_apps < 0)                           /* Bad data was passed in! */
        return(-3);
      responsible_for_apps = 0;
    }

  /* If caller did not pass in the Appusers array, get it ourselves. */
  if (Appusers == NULL)
    {
      Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
      if (num_appusers < 0)
        {
          syslog(LOG_WARNING, "%s Unable retrieve Appusers data!", fnct_name);
          if (responsible_for_apps == 1)
            freeAppData(Apps, num_apps);
          return(-2);
        }
      responsible_for_appusers = 1;
    }
  else                        /* Appusers data was passed in for us to use.. */
    {
      if (num_appusers < 0)                           /* Bad data passed in! */
        {
          if (responsible_for_apps == 1)
            freeAppData(Apps, num_apps);
          return(-3);
        }
      responsible_for_appusers = 0;
    }


  for (i = 0; i < num_appusers; i++)
    {
      /* If something else already deleted this appuser, skip over them.. */
      if (Appusers[i]->marker == 0)
        continue;

      /* DWB 8/9/99 Don't know why this was here.. perhaps to remove
         users that did not match apps? */
      Appusers[i]->marker = 0;                    /* default is delete */

      for (j = 0; j < num_apps; j++)
        {
          /* If something else already deleted this app, skip over it.. */
          if (Apps[j]->marker == 0)
            continue;

          if (strcmp(Appusers[i]->progroup_title,
                     Apps[j]->progroup_title) != 0)
            continue;

          /* * Apply access_role change operations. */
          app_role = 0;
          if (Appusers[i]->app_role & NTCSS_BATCH_USER_BIT)
            app_role |= NTCSS_BATCH_USER_BIT;
          k = 0;
          while ((op = (role_map[j][k][0]) != LAST_ACCESS_OP))
            {           /* this bit could have changed position... */
              switch (op)           /* 0 means no change, 1 means change*/
                {
                case MOVE_ACCESS_OP:          /* may need to change. */
                  mask1 = 1 << (role_map[j][k][1] + 1); /*old approle bit */
                  mask2 = 1 << (role_map[j][k][2] + 1); /*new approle bit */
                  
              /* If old and new role bits match for this progroup, all's ok. */
                  if (isBitSet(Appusers[i]->app_role, mask1)) /* current */
                    doSetBit(&app_role, mask2);        /* temp role bits */
                  else
                    doClearBit(&app_role, mask2);

syslog(LOG_WARNING,"App <%s>, user <%s>,old <%d>, new <%d>,approle<%d>",Apps[j]->progroup_title,Appusers[i]->login_name,role_map[j][k][1],role_map[j][k][2],app_role);

                  break;
                }
              k++;
            }
          Appusers[i]->app_role = app_role; /* corrected app_role bitmask*/
          Appusers[i]->marker = 1;      /* needs to be written to the DB */
          break;
        }                   /* end for (each app) */
    }                  /* end for(each appuser) */

  if (responsible_for_apps == 1)
    freeAppData(Apps, num_apps);

  if (!write_results)  /* Determine if we are supposed to write out results. */
    return(0);

  sprintf(datafile, "%s/appusers%s", db_dir, file_ext);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      if (responsible_for_appusers == 1)
        freeAppuserData(Appusers, num_appusers);
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, datafile);
      return(-4);
    }

  writeAppuserData(Appusers, num_appusers, fp);
  closeAndEncrypt(fp, datafile);

  if (responsible_for_appusers == 1)
    freeAppuserData(Appusers, num_appusers);

  return(0);
}


/*****************************************************************************/
/* Remove all predefined jobs in pdj DB associated with any removed progroups,
 * and update any role bit-shifting that might have occured when access_roles
 * bit positions may have changed.
 */

int  UpdatePDJs( int          role_map[MAX_NUM_PROGROUPS]
                                       [2 * MAX_NUM_APP_ROLES_PER_PROGROUP][3],
                 AppItem     *Apps[],
                 int          num_apps,
                 const char  *db_dir)
{
  int   app_role;
  int   i, j, k;
  int   mask1, mask2;
  int   num_pdjs;
  int   op;
  char  datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "UpdatePDJs():";
  FILE  *fp;
  PredefinedJobItem **Pdjs;

  Pdjs = getPredefinedJobData(&num_pdjs, db_dir);
  for (i = 0; i < num_pdjs; i++)
    {
      Pdjs[i]->marker = 0;   /* default is no_change */
      for (j = 0; j < num_apps; j++)
        {
          if (strcmp(Pdjs[i]->progroup_title, Apps[j]->progroup_title) != 0)
            continue;

          /* * Apply access role change operations. */
          app_role = 0;
/****
          user_role = Pdjs[i]->role_access;
          if (Pdjs[i]->role_access & 1)   currently allows app_admins
            app_role = 1;
          if (Pdjs[i]->role_access & 2)   currently allows batch users
            app_role |= 2;
*****/
          k = 0;
          while ((op = role_map[j][k][0]) != LAST_ACCESS_OP)
            {            /* this bit could have changed position... */
              switch (op)         /* 0 means no change, 1 means change*/
                {
                case MOVE_ACCESS_OP:
                  mask1 = 1 << (role_map[j][k][1] + 1);/*old approle bit*/
                  mask2 = 1 << (role_map[j][k][2] + 1);/*new approle bit*/
    /* If old and new role bits match for this progroup, all's ok. */
                  if (isBitSet(Pdjs[i]->role_access, mask1))
                    doSetBit(&app_role, mask2);
                  else
                    doClearBit(&app_role, mask2);
                  break;
                }
              k++;
            }
          Pdjs[i]->role_access = app_role;
          Pdjs[i]->marker = 1;  /* this one needs updating */
          break;
        }
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PREDEFINED_JOB_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      freePredefinedJobData(Pdjs, num_pdjs);
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, datafile);
      return(-1);
    }

  writePredefinedJobData(Pdjs, num_pdjs, fp);
  closeAndEncrypt(fp, datafile);

  freePredefinedJobData(Pdjs, num_pdjs);
  return(0);
}


/*****************************************************************************/
/* Remove access links between printers and non-existent apps in DB.
*/

int  RemoveApplessPrtAccessRecs( AppItem       *Apps[],
                                 int            num_apps,
                                 const char    *db_dir)
{
  int   j;
  int   x;
  char  datafile[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];
  const char  *fnct_name = "RemoveApplessPrtAccessRecs():";
  char  tempfile[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord   record;

  sprintf(tempfile, "%s/%s.temp", db_dir, tables[PRINTER_ACCESS_ID]);
  fp = fopen(tempfile, "w");  /* just want to clear this out */
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, tempfile);
      return(-1);   /* error */
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTER_ACCESS_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, datafile);
      return(-2);
    }

  /* write out only the printer_access records that have matching progroup
     names in the progroups DB... */
  while (getRecord(&record, rp, PRINTER_ACCESS_NUM_RECS))
    {
      for (j = 0; j < num_apps; j++)
        {
          if (strcmp(record.field[2].data, Apps[j]->progroup_title) == 0)
            {
              fprintf(fp, PRINTER_ACCESS_FMT,
                      record.field[0].data,              /* printer's name */
                      record.field[1].data,              /* hostname */
                      record.field[2].data);             /* progroup title */
              break;
            }
        }
    }
  closeAndEncrypt(fp, tempfile);
  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  x = rename(tempfile, datafile);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Rename returns error %i trying to rename %s "
             "to %s!", fnct_name, x, tempfile, datafile);
      return(-3);
    }

  return(0);
}


/*****************************************************************************/
/* Remove printers no longer associated with valid print classes. The
   printers.tmp file is created here with the updated printer information.
 */

int  RemoveClasslessPrinters( PrtItem     *Printers[],
                              int          num_printers,
                              const char  *db_dir )
{
  int   i, j, k;
  int   num_printer_classes;
  char  datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "RemoveClasslessPrinters():";
  FILE  *fp;
  PrtclassItem **Printerclasses;

  Printerclasses = getPrtclassData(&num_printer_classes, db_dir);
  for (i = 0; i < num_printers; i++)
    {
      for (j = 0; j < num_printer_classes; j++)
        {
          if (strcmp(Printers[i]->prt_class_title,
                     Printerclasses[j]->prt_class_title) == 0)
            {
              Printers[i]->marker = 1;      /* Mark this one to keep. */
              break;
            }
        }
    }
  freePrtclassData(Printerclasses, num_printer_classes);

/* Remove references to non-existent redirect printers in DB. */

  for (i = 0; i < num_printers; i++)                   /* For each printer.. */
    {
      if (Printers[i]->marker != 0) /* Printer has been marked for removal.. */
        continue;                   /* ..skip to next printer in the list.. */

      /* Ignor the printers that don't have redirects.. */
      if (isBlank(Printers[i]->redirect_prt))
        continue;

      k = 1;   /* Default is to clear the redirect field for this record. */

      /* Check the printer list to see if this redirect printer exists... */
      for (j = 0; j < num_printers; j++)
        {
          if (Printers[j]->marker == 0)           /* Skip deleted printers.. */
            continue;                                          /* Inner loop */

          if (strcmp(Printers[i]->redirect_prt, Printers[j]->prt_name) == 0)
            {
              k = 0;                     /* Means redirect printer is valid. */
              break;                                          /* Inner loop. */
            }
        }

      if (k)
        Printers[i]->marker = -1; /* -1 clears the redirect printer field. */
    }

  /* Recreate the printers.tmp database.. */
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING,"%s Unable to open file %s!", fnct_name, datafile);
      return(-1);
    }

  writePrtData(Printers, num_printers, fp);
  for (i = 0; i < num_printers; i++)
    if (Printers[i]->marker == -1)
      fprintf(fp, PRINTERS_FMT,
              Printers[i]->prt_name,
              Printers[i]->hostname,
              Printers[i]->prt_location,
              Printers[i]->max_size,
              Printers[i]->prt_class_title,
              Printers[i]->ip_address,
              Printers[i]->security_class,
              Printers[i]->status,
              Printers[i]->port_name,
              Printers[i]->suspend_flag,
              "", "");

  closeAndEncrypt(fp, datafile);

  return(0);
}


/*****************************************************************************/
/* Validate output type references to default printers
   and remove output type records associated with apps no longer in
   progrps file.  This is called from the db integrity checker.
*/

int  UpdateOutputTypesDB( PrtItem      *Printers[],
                          int           num_printers,
                          const char  *db_dir )
{
  int    i, j, k;
  int    num_apps;
  int    num_otypes;
  int    x;
  char   datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "UpdateOutputTypesDB():";
  FILE  *fp;
  AppItem    **Apps;
  OutputTypeItem    **Otypes;

  /* These functions access .tmp tables here before they get overwritten..*/
  Otypes = getOutputTypeData(&num_otypes, db_dir);
  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_TYPE_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      freeAppData(Apps, num_apps);
      freeOutputTypeData(Otypes, num_otypes);
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, datafile);
      return(-1);
    }

    /* For each output type record.. */
  for (i = 0; i < num_otypes; i++)
    {
      k = 0;

      /* Skip the records with no default printers   */
      if (!isBlank(Otypes[i]->default_prt)) {
      	/* If a default printer is assoc. with this o_type, check validity..*/
      	/* For each printer... */
      	for (j = 0; j < num_printers; j++)
        {

          /* Skip printer entries that are marked for deletion.. */
          if (Printers[j]->marker == 0)
            continue;

          if (strcmp(Printers[j]->prt_name, Otypes[i]->default_prt) == 0)
            {             /* Found the default printer. */
              k++;        /* Set flag to record the default printer. */
              break;                                    /* ..from inner loop */
            }

        }  /* End of loop to find default printer for this o_type. */
      }

      /* Verify that app associated with this o_type record is valid..*/
      for (j = 0; j < num_apps; j++)
        {
          if (strcmp(Apps[j]->progroup_title, Otypes[i]->progroup_title) == 0)
            {
              k += 2;  /* Set flag for valid app */
              break;
            }
        }

      /* Write out the record only if the asscossociated app is valid.. */
      if (k == 2)             /* No default printer found, but app is valid. */
        fprintf(fp, OUTPUT_TYPE_FMT,
                Otypes[i]->out_type_title,
                Otypes[i]->progroup_title,
                Otypes[i]->description,
                "",                    /* Default printer field stays empty. */
                Otypes[i]->batch_flag);
      else
        if (k == 3)           /* Default printer AND app is valid. */
          fprintf(fp, OUTPUT_TYPE_FMT,
                  Otypes[i]->out_type_title,
                  Otypes[i]->progroup_title,
                  Otypes[i]->description,
                  Otypes[i]->default_prt,
                  Otypes[i]->batch_flag);
    }

  closeAndEncrypt(fp, datafile);

  /* Since output types may have changed, update the dependent output_prt DB */
  x = UpdateOutputPrintersDB(Printers, num_printers, Otypes, num_otypes,
                             Apps, num_apps, db_dir);

  freeOutputTypeData(Otypes, num_otypes);
  freeAppData(Apps, num_apps);

  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Unable to update the output_prt database!  "
             "Error %d", fnct_name, x);
      return(-2);   /* error */
    }

  return(0);
}


/*****************************************************************************/
/* Remove links between invalid output type/app combos as well as
   non-existent printers.  This is called from UpdateOutputTypesDB.
*/

int  UpdateOutputPrintersDB( PrtItem        *Printers[],
                             int             num_printers,
                             OutputTypeItem *Otypes[],
                             int             num_otypes,
                             AppItem        *Apps[],
                             int             num_apps,
                             const char     *db_dir )
{
  int   good_so_far;   /* flag */
  int   i;
  int   x;
  char  datafile[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];
  const char  *fnct_name = "UpdateOutputPrintersDB():";
  char  tempfile[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord   record;

  /* this is here to satifsy compiler warning about the parameter not being
     used. if the parameter is ever used in the future, the following will
     no longer be needed. */
  Otypes=Otypes;
  num_otypes=num_otypes;

  sprintf(tempfile, "%s/%s.temp", db_dir, tables[OUTPUT_PRT_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             tempfile);
      return(-1);
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[OUTPUT_PRT_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             datafile);
      return(-2);
    }

  /* Get rid of any output_prt recs without a valid progroup.. */
  while (getRecord(&record, rp, OUTPUT_PRT_NUM_RECS))
    {
      good_so_far = 0;
      /* check to see if associated progroup exists.. */
      for (i = 0; i < num_apps; i++)
        {
          if (strcmp(record.field[1].data, Apps[i]->progroup_title) == 0)
            {
              good_so_far++;
              break;
            }
        }

      if (!good_so_far)
        continue;              /* Skip to next record */
      else
        good_so_far = 0;     /* Re-init */

      /* Check to make sure printer is valid.. */
      for (i = 0; i < num_printers; i++)
        {
          if (Printers[i]->marker != 0)          /* Skip deleted printers. */
            {
              if (strcmp(record.field[2].data, Printers[i]->prt_name) == 0)

/** Since printer names are now unique, hosts are ignored.
              && (strcmp(record.field[3].data, Printers[i]->hostname) == 0) )
*****/
                {
                  good_so_far++;
                  break;
                }
            }
        }

      if (!good_so_far)
        continue;              /* Skip to next record */

      /* Only recs with valid printers & progroups get written. */
      fprintf(fp, OUTPUT_PRT_FMT,
              record.field[0].data,
              record.field[1].data,
              record.field[2].data,
              record.field[3].data);
    }

  closeAndEncrypt(fp, tempfile);
  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  x = rename(tempfile, datafile);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s rename() returns error %i renaming file %s "
             "to %s!", fnct_name, x, tempfile, datafile);
      return(-3);
    }
  return(0);
}


/*****************************************************************************/
/* Remove printer_access table records with invalid printers. Called from the
   integrity checker.
*/

int  RemovePrinterlessPrtAccessRecs(  PrtItem     *Printers[],
                                      int          num_printers,
                                      const char  *db_dir)
{
  int   i;
  int   x;
  char  datafile[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];
  const char  *fnct_name = "RemovePrinterlessPrtAccessRecs():";
  char  tempfile[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord   record;

  sprintf(tempfile, "%s/%s.temp", db_dir, tables[PRINTER_ACCESS_ID]);
  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             tempfile);
      return(-1);
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTER_ACCESS_ID]);
  rp = decryptAndOpen(datafile, decryptfile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             datafile);
      fclose(fp);
      if (decryptfile[0] != '\0')
        remove(decryptfile);
      return(-2);
    }

  while (getRecord(&record, rp, PRINTER_ACCESS_NUM_RECS))
    {
      for (i = 0; i < num_printers; i++)
        {
          if (Printers[i]->marker == 0)   /* Deleted printer ? */
            continue;

          if ((strcmp(record.field[0].data, Printers[i]->prt_name) == 0) &&
              (strcmp(record.field[1].data, Printers[i]->hostname) == 0))
            {
              fprintf(fp, PRINTER_ACCESS_FMT,
                      record.field[0].data,
                      record.field[1].data,
                      record.field[2].data);
              break;
            }
        }
    }
  closeAndEncrypt(fp, tempfile);
  fclose(rp);
  if (decryptfile[0] != '\0')
    remove(decryptfile);

  x = rename(tempfile, datafile);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s rename() returns error %i renaming file %s to "
             "%s!", fnct_name, x, tempfile, datafile);
        return(-3);
    }

  return(0);
}


/*****************************************************************************/
/* Add all the hosts' names to the arguments line of all NTCSS progroup
   admin programs.
*/

int  ModifyNTCSSAdminProgArgs( const char *db_dir )

{
  int   flag;
  int   i, j;
  int   num_hosts;
  int   num_programs;
  char  cmd_args[MAX_ITEM_LEN];
  char  datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "ModifyNTCSSAdminProgArgs():";
  FILE  *fp;
  HostItem   **Hosts;
  ProgramItem  **Programs;

  Hosts = getHostData(&num_hosts, db_dir);
  if (Hosts == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to retrieve Host data!", fnct_name);
      return(-1);
    }

  Programs = getProgramData(&num_programs, db_dir, WITH_NTCSS_GROUP);
  if (Programs == NULL)
    {
      freeHostData(Hosts, num_hosts);
      syslog(LOG_WARNING, "%s Unable to retrieve program data!", fnct_name);
      return(-2);
    }

  for (i = 0; i < num_programs; i++)
    if ((strcmp(NTCSS_APP, Programs[i]->progroup_title) == 0) &&
        (Programs[i]->flag == 3))
      Programs[i]->marker = 0;  /* is a NTCSS progroup client & server app. */
    else
      Programs[i]->marker = 1; /* is NOT a ^^^^^^^^^ */

  cmd_args[0] = '\0';
  for (j = 0; j < num_hosts; j++)    /* Create string of all hosts' names. */
    {
      strcat(cmd_args, Hosts[j]->hostname);
      strcat(cmd_args, " ");
    }

  trim(cmd_args);
  if (strlen(cmd_args) > MAX_CMD_LINE_ARGS_LEN)
    {
     syslog(LOG_WARNING, "%s Too many host names to store!", fnct_name);
     flag = 0;
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PROGRAMS_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             datafile);
      freeHostData(Hosts, num_hosts);
      freeProgramData(Programs, num_programs);
      return(-3);
    }

  /* Write out all the non-NTCSS progroup programs.. */
  writeProgramData(Programs, num_programs, fp);

  /* Write out the remaining (NTCSS progroup) programs with new args.. */
  for (i = 0; i < num_programs; i++)
    {
      if (Programs[i]->marker == 0)
        {
          fprintf(fp, PROGRAMS_FMT,
                  Programs[i]->program_title,
                  Programs[i]->program_file,
                  Programs[i]->icon_file,
                  Programs[i]->icon_index,
                  cmd_args,
                  Programs[i]->progroup_title,
                  Programs[i]->security_class,
                  Programs[i]->program_access,
                  Programs[i]->flag,
                  Programs[i]->working_dir);
        }
    }
  closeAndEncrypt(fp, datafile);

  freeHostData(Hosts, num_hosts);
  freeProgramData(Programs, num_programs);
  if (flag == 0)
    return(-4);  /* error */
  else
    return(0);
}

/*****************************************************************************/
/* Modify links between all users and apps in DB.
*/

int  RemoveUserlessAppuserRecs( const char *db_dir )

{
  int   i, j;
  int   num_appusers;
  int   num_users;
  char  datafile[MAX_FILENAME_LEN];
  const char  *fnct_name = "RemoveUserlessAppuserRecs():";
  FILE  *fp;
  UserItem   **Users;
  AppuserItem  **Appusers;

  Users = getUserData(&num_users, db_dir);
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  for (j = 0; j < num_users; j++)
    {
      Users[j]->security_class = 4;                         /* default value */
      Users[j]->marker = -1;                                /* default value */

      for (i = 0; i < num_appusers; i++)
        {
          if (strcmp(Users[j]->login_name, Appusers[i]->login_name) == 0)
              Appusers[i]->marker = 0;                 /* Mark for later. */
        }
    }

  /* At ths point, if app->marker == 0, then user is valid. else is 1. */

  for (i = 0; i < num_appusers; i++)
    {
      if (Appusers[i]->marker == 1)     /* If no user found for this entry.. */
        Appusers[i]->marker = 0;                 /* ..mark this for removal. */
      else
        Appusers[i]->marker = 1;     /* user found, write this out to the DB */
    }

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[APPUSERS_ID]);
  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open temp db file %s!", fnct_name,
             datafile);
      freeAppuserData(Appusers, num_appusers);
      freeUserData(Users, num_users);
      return(-1);
    }

  writeAppuserData(Appusers, num_appusers, fp);
  closeAndEncrypt(fp, datafile);

  freeAppuserData(Appusers, num_appusers);
  freeUserData(Users, num_users);  /* added this in for next call..*/

  return(0);
}


/*****************************************************************************/
/* Clears the num_users fields in the current_apps table and makes sure the
   associated apps are valid.
*/

int  InitializeCurrentAppsDB( const char *db_dir )

{
  const char  *fnct_name = "InitializeCurrentAppsDB():";
  int    i, j;
  int    num_apps;
  int    num_current_apps;
  char   writefile[MAX_FILENAME_LEN];
  FILE  *file_ptr;
  AppItem    **TempApps;
  AppItem    **CurrentApps;

  TempApps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);
  CurrentApps = getCurrentAppData(&num_current_apps, db_dir);

  sprintf(writefile, "%s/%s.tmp", db_dir, tables[CURRENT_APPS_ID]);
  file_ptr = fopen(writefile, "w");
  if (file_ptr == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name, writefile);
      freeAppData(TempApps, num_apps);
      freeCurrentAppData(CurrentApps, num_current_apps);
      return(-1);
    }

  /* Basically we overwrite the current_apps table, but refer to the old
     current_apps entries to preserve the app_lock field. */
  for(i = 0; i < num_apps; i++)
    {
      TempApps[i]->num_roles = 0;              /* Clear the num_users field. */
      TempApps[i]->icon_index = 0;              /* Clear the app_lock field. */

      /* Restore the app_lock field from the current_users record (if exists)*/
      for(j = 0; j < num_current_apps; j++)
        if (strcmp(TempApps[i]->progroup_title,
                   CurrentApps[j]->progroup_title) == 0)
          TempApps[i]->icon_index = CurrentApps[j]->icon_index;
    }

  freeCurrentAppData(CurrentApps, num_current_apps);

  writeCurrentAppData(TempApps, num_apps, file_ptr);
  closeAndEncrypt(file_ptr, writefile);

  freeAppData(TempApps, num_apps);

  return(0);
}


/*****************************************************************************/
/****** REMOVED BY TON

int ReduceAppList(strOrigList,strAppsList)
char *strAppsList;
char *strOrigList;
{
int i,j;
int nfound;
char  strApp[MAX_PROGROUP_TITLE_LEN+1];
char  strAppNew[MAX_PROGROUP_TITLE_LEN+1];
char  strSaveAppsList[MAX_GROUP_LIST_LEN + 1];

  strcpy(strSaveAppsList, strAppsList);
  memset(strAppsList, NULL, MAX_GROUP_LIST_LEN + 1);

    strcpy(strSaveAppsList,strAppsList); 
    memset(strAppsList,NULL,MAX_GROUP_LIST_LEN+1);

    i=0;
    while ((i=stripWord(i,strOrigList,strApp)) != -1) {
        nfound=1;
        j=0;
        while ((j=stripWord(j,strSaveAppsList,strAppNew)) != -1) {
            if (!strcmp(strApp,strAppNew)) {
                 nfound=0;
                 break;
            }
            }
            if (nfound==1)
            {
                if (strlen(strAppsList))
                        strcat(strAppsList," ");
                    strcat(strAppsList,strApp);
            }
    }
    return(0);
}

*******************/


/*****************************************************************************/

int  IsAppLocked( const char *appname )

{
  AppItem  **currentApps;
  int   num_apps;
  int   lockval;
  int   i;

  lockval = 0;  /* default is not locked */
  currentApps = getCurrentAppData(&num_apps, NTCSS_DB_DIR);

  for (i = 0; i < num_apps; i++)
    {
      if (!strcmp(appname, currentApps[i]->progroup_title))
        {
          lockval = currentApps[i]->icon_index;
          break;
        }
    }
  freeCurrentAppData(currentApps, num_apps);
  return(lockval);
}


/*****************************************************************************/

int  IsAppRoleLockOverride( const char      *appname,
                            int              role,
                            AccessRoleItem **AccessRoles,
                            int              num_roles)
{

  int  i;
  int  local_data_flag;
  int  overridestatus;
  int  adjrole;
  
  /* this is here to satifsy compiler warning about the parameter not beiing
     used. if the parameter is ever used in the future, the following will
     no longer be needed. */
  appname = appname;

  overridestatus = 0;
  local_data_flag = 0;

  if (AccessRoles == NULL)
    {
      AccessRoles = getAccessRoleData(&num_roles, NTCSS_DB_DIR);
      local_data_flag = 1;
    }

  for (i = 0; i < num_roles; i++)
    {
      if (!strcmp(AccessRoles[i]->progroup_title, appname))
        {
          adjrole = (1 << (AccessRoles[i]->role_number +
                            ROLE_BIT_BASE_OFFSET));
          if ((role & adjrole) && AccessRoles[i]->locked_role_override)
            {
              overridestatus = 1;
              break;
            }
        }
    }

  if (local_data_flag == 1)
    freeAccessRoleData(AccessRoles,num_roles);

  return(overridestatus);
}


/*****************************************************************************/

int isAppOnThisHost( const char  *appname,
                     AppItem    **Apps,
                     int          num_apps)
{
  int   i;
  int  appstatus;
  char myhost[MAX_HOST_NAME_LEN+1];

  /* this is here to satisfy compiler warning about the parameter not being
     used. if the parameter is ever used in the future, the following will
     no longer be needed. */
  appname = appname;

  appstatus = 0;

  gethostname(myhost, MAX_HOST_NAME_LEN + 1);

  for (i = 0; i < num_apps; i++)
    {
      /* if this server matches the one in the App list
         or this host is the master AND the app server is a slave 
      */
      if ( !strcmp(Apps[i]->hostname, myhost) ||
           ( (getHostType(Apps[i]->hostname, NTCSS_DB_DIR) == 
                                                   NTCSS_APP_SERVER_TYPE ) &&
             (getHostType(myhost, NTCSS_DB_DIR) == NTCSS_MASTER_SERVER_TYPE)) )
        {
          appstatus = 1;
          break;
        }
    }

  return(appstatus);
}


/*****************************************************************************/
/* Removes appuser records if the app they were associated with has been
    removed.
   Var "file_ext" should be the extension of the appusers file created
    including the separator period.  It can be NULL.  If the write_to_file
    flag is 0, then no extension is needed anyway.
   Var "write_results" tells us whether to write the results out to file or
    not.  This should only be zero if the caller only wants to clean up the
    records in the Appusers array passed in
   NOTE** Apps and/or Appusers can optionally be NULL.
*/

int  RemoveApplessAppusers( const char    *db_dir,
                            const char    *file_ext,
                            AppItem       *Apps[],
                            int            num_apps,
                            AppuserItem   *Appusers[],
                            int            num_appusers,
                            int            write_results )

{
  const char  *fnct_name = "RemoveApplessAppusers():";
  FILE   *fp;
  char    results_file[MAX_FILENAME_LEN];
  int     i, j;
  int     responsible_for_apps;
  int     responsible_for_appusers;

  /* If we're not writing results locally, what's the sense in getting the
     data locally?  */
  if ((write_results == 0) && (Appusers == NULL))
    return(-1);

  /* If caller did not pass in the Apps array, get it ourselves. */
  if (Apps == NULL)
    {
      Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);
      if (num_apps < 0)
        {
          syslog(LOG_WARNING, "%s Unable retrieve Apps data!", fnct_name);
          return(-2);
        }
      responsible_for_apps = 1;
    }
  else                            /* Apps data was passed in for us to use.. */
    {
      if (num_apps < 0)                           /* Bad data was passed in! */
        return(-3);
      responsible_for_apps = 0;
    }

  /* If caller did not pass in the Appusers array, get it ourselves. */
  if (Appusers == NULL)
    {
      Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
      if (num_appusers < 0)
        {
          syslog(LOG_WARNING, "%s Unable retrieve Appusers data!", fnct_name);
          if (responsible_for_apps == 1)
            freeAppData(Apps, num_apps);
          return(-2);
        }
      responsible_for_appusers = 1;
    }
  else                        /* Appusers data was passed in for us to use.. */
    {
      if (num_appusers < 0)                           /* Bad data passed in! */
        {
          if (responsible_for_apps == 1)
            freeAppData(Apps, num_apps);
          return(-4);
        }
      responsible_for_appusers = 0;
    }


  /* for each appuser, make sure their associated app is still valid.. */
  for (i = 0; i < num_appusers; i++)
    {
      /* If something else already deleted this appuser, skip over them.. */
      if (Appusers[i]->marker == 0)
        continue;

      for (j = 0; j < num_apps; j++)
        {
          /* If something else already deleted this app, skip over it.. */
          if (Apps[j]->marker == 0)
            continue;

	/* TONY changed to include 0 approles  10/11/00 */
          if(strcmp(Appusers[i]->progroup_title, Apps[j]->progroup_title) == 0 
		&& Appusers[i]->app_role != 0)
            {
              Appusers[i]->marker = 1;     /* App found, preserve this rec. */
              break;                       /* Skip to next appuser rec. */
            }
        }

      if (j == num_apps) /* At end of app list, so app was not found.. */
        {
          syslog(LOG_WARNING, "%s Removing appuser %s from app %s.", fnct_name,
                 Appusers[i]->login_name, Appusers[i]->progroup_title);
          Appusers[i]->marker = 0;
        }
    }

  if (responsible_for_apps == 1)
    freeAppData(Apps, num_apps);

  if (!write_results)  /* Determine if we are supposed to write out results. */
    return(0);

  sprintf(results_file, "%s/appusers%s", db_dir, file_ext);
  fp = fopen(results_file, "w");
  if (fp == NULL)
    {
      if (responsible_for_appusers == 1)
        freeAppuserData(Appusers, num_appusers);
      syslog(LOG_WARNING, "%s Unable to open file %s!", fnct_name,
             results_file);
      return(-5);
    }
  writeAppuserData(Appusers, num_appusers, fp);
  closeAndEncrypt(fp, results_file);

  if (responsible_for_appusers == 1)
    freeAppuserData(Appusers, num_appusers);

  return(0);
}


/*****************************************************************************/
