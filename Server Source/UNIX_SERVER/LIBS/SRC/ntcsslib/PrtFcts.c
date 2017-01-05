
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * PrtFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#ifdef SOLARIS
#  include <dirent.h>
#else
#  include <sys/dir.h>
#endif

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <StringFcts.h>
#include <ini_Fcts.h>
#include <LibNtcssFcts.h>

/** LOCAL INCLUDES **/

#include "TextDBP.h"


/** LOCAL FUNCTION PROTOTYPES **/


/** STATIC VARIABLES **/

static const char  *prt_statuses[] = { "Halted", "Working" };
static const char  *queue_statuses[] = { "Disabled", "Enabled" };
static const char  *prt_accesses[] = { "By Application", "General" };
static const char  *prt_suspend_states[] = { "Suspend Requests", "Permit All "
                                       "Requests", "Unknown Value" };
static const char  *no_yes_values[] = { "No", "Yes", "Unknown Value" };



/*****************************************************************************/

/** FUNCTION DEFINITIONS **/

int  isPrtEnabled( int status )

{
  return(status & 2);
}


/*****************************************************************************/

void enablePrtStatus( int *status )

{
    *status |= 2;
}


/*****************************************************************************/

void  disablePrtStatus( int *status )

{
  if (*status & 2)
    *status ^= 2;
}


/*****************************************************************************/

const char  *getPrtStatus( int status )

{
  if (isPrtEnabled(status))
    return(prt_statuses[1]);
  else
    return(prt_statuses[0]);
}


/*****************************************************************************/

int  isQueueEnabled( int status )

{
  return(status & 1);
}


/*****************************************************************************/

void  enableQueueStatus( int *status )

{
  *status |= 1;
}


/*****************************************************************************/

void  disableQueueStatus( int *status )

{
  if (*status & 1)
    *status ^= 1;
}


/*****************************************************************************/

const char  *getQueueStatus( int status )

{
  if (isQueueEnabled(status))
    return(queue_statuses[1]);
  else
    return(queue_statuses[0]);
}


/*****************************************************************************/

int  isPrtAccessGeneral( int status )

{
  return(status & 4);
}


/*****************************************************************************/

void  makePrtAccessGeneral( int *status )

{
  *status |= 4;
}


/*****************************************************************************/

void  makePrtAccessByapp( int *status )

{
  if (*status & 4)
    *status ^= 4;
}


/*****************************************************************************/

const char  *getPrtAccess( int status )

{
  if (isPrtAccessGeneral(status))
    return(prt_accesses[1]);
  else
    return(prt_accesses[0]);
}


/*****************************************************************************/

int  getPrtAccessNum( const char *str )

{
  int i;

  for (i = 0; i < NUM_PRT_ACCESSES; i++)
    if (strcmp(str, prt_accesses[i]) == 0)
      return(i);

  return(0);
}


/*****************************************************************************/

const char  *getPrtSuspendState( int value )

{
  if (value >= 0 && value < NUM_PRT_SUSPEND_STATES)
    return(prt_suspend_states[value]);
  else
    return(prt_suspend_states[NUM_PRT_SUSPEND_STATES]);
}


/*****************************************************************************/

int  getPrtSuspendStateNum( const char  *str )

{
  int  i;

  for (i = 0; i < NUM_PRT_SUSPEND_STATES; i++)
    if (strcmp(str, prt_suspend_states[i]) == 0)
      return(i);

  return(0);
}


/*****************************************************************************/

const char  *getNoYesValue( int value )

{
  if (value >= 0 && value < NUM_NO_YES_VALUES)
    return(no_yes_values[value]);
  else
    return(no_yes_values[NUM_NO_YES_VALUES]);
}


/*****************************************************************************/

int  getNoYesValueNum( const char *str )

{
  int  i;

  for (i = 0; i < NUM_NO_YES_VALUES; i++)
    if (strcmp(str, no_yes_values[i]) == 0)
      return(i);

  return(0);
}


/*****************************************************************************/

int  checkPrtName( char *name )

{
  int   leng;
  char *s;

  trim(name);
  s = name;
  leng = strlen(name);

  if (leng < MIN_PRT_NAME_LEN || leng > MAX_PRT_NAME_LEN)
    return(0);

  while (*s) {
    if (!(isalnum((int)*s) || (*s == '_')))
      return(0);
    s++;
  }

  return(1);
}


/*****************************************************************************/

int  checkHostName( char *name )

{
  int leng, flag = 1;
  char *s;

  trim(name);
  s = name;
  leng = strlen(name);

  if (leng < MIN_HOST_NAME_LEN || leng > MAX_HOST_NAME_LEN)
    return(0);

  while (*s) {
    if (!(isalnum((int)*s) || (*s == '_') || (*s == '-')))
      flag = 0;
    s++;
  }
  
  return(flag);
}


/*****************************************************************************/

int  checkPrtLocation( char *location )

{
  int leng;

  trim(location);
  leng = strlen(location);

  if (leng < MIN_PRT_LOCATION_LEN || leng > MAX_PRT_LOCATION_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkMaxSize( char *size )

{
  int leng, flag = 1;
  char *s;

  trim(size);
  s = size;
  leng = strlen(size);

  if (leng < 1 || leng > 9)
    return(0);

  while (*s) {
    if (!isdigit((int)*s))
      flag = 0;
    s++;
  }

  return(flag);
}


/*****************************************************************************/

int  checkIpAddress( char *address )

{
  int leng, i, j, n;
  char str[20], number[20], *s;

  trim(address);
  leng = strlen(address);

  if (leng < 7 || leng > 15)
    return(0);

  replacePattern(".", " ", address, str);

  n = 0;
  for (i = 0; i < 4; i++) {
    n = stripWord(n, str, number);
    if (n == -1)
      return(0);
    s = &number[0];
    while (*s) {
      if (!isdigit((int)*s))
        return(0);
      s++;
    }
    j = atoi(number);
    if (j < 0 || j > 255)
      return(0);
  }

  return(1);
}


/*****************************************************************************/

int  checkEthernetAddress( char *address )

{
  int leng, i, n;
  char str[20], number[20], *s;

  trim(address);
  leng = strlen(address);
  
  if (leng < 11 || leng > 17)
    return(0);

  replacePattern(":", " ", address, str);

  n = 0;
  for (i = 0; i < 6; i++) {
    n = stripWord(n, str, number);
    if (n == -1)
      return(0);
    s = &number[0];
    if (strlen(s) > 2)
      return(0);
    while (*s) {
      if (!(isdigit((int)*s) || *s == 'a' || *s == 'b' ||
            *s == 'c' || *s == 'd' || *s == 'e' || *s == 'f'))
        return(0);
      s++;
    }
  }

  return(1);
}


/*****************************************************************************/

int  checkAdapterLocation( char *location )

{
  int  leng;

  trim(location);
  leng = strlen(location);

  if (leng < MIN_ADAPTER_LOC_LEN || leng > MAX_ADAPTER_LOC_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkOutputTypeTitle( char *title )

{
  int leng;

  trim(title);
  leng = strlen(title);

  if (leng < MIN_OUT_TYPE_TITLE_LEN || leng > MAX_OUT_TYPE_TITLE_LEN)
    return(0);
  
  return(1);
}


/*****************************************************************************/

int  checkOutputTypeDescription( char *description )

{
  int leng;

  trim(description);
  leng = strlen(description);

  if (leng < MIN_DESCRIPTION_LEN || leng > MAX_DESCRIPTION_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkPrtClassTitle( char *title )

{
  int leng;

  trim(title);
  leng = strlen(title);
    
  if (leng < MIN_PRT_CLASS_TITLE_LEN || leng > MAX_PRT_CLASS_TITLE_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkPrtDeviceName( char *name )

{
  int leng;

  trim(name);
  leng = strlen(name);

  if (leng < MIN_PRT_DEVICE_NAME_LEN || leng > MAX_PRT_DEVICE_NAME_LEN)
    return(0);

  return(1);
}


/*****************************************************************************/

int  checkPrtDriverName( char *name )

{
  int leng;
  char *s;

  trim(name);
  leng = strlen(name);
  s = name;

  if (leng < MIN_PRT_DRIVER_NAME_LEN || leng > MAX_PRT_DRIVER_NAME_LEN)
    return(0);

  while (*s) {
    if (isspace((int)*s))
      return(0);
    s++;
  }

  return(1);
}


/*****************************************************************************/

int  changePrtStatus( const char  *prt_name,
                      const char  *hostname,
                      const char  *db_dir,
                      int    mask,
                      int    status)

{
  PrtItem  **Prts;
  AppItem  **Apps;
  char   cmd[MAX_CMD_LEN];
  char   datafile[MAX_FILENAME_LEN];
  int    i, j;
  int    num_prts;
  int    num_apps;
  int    flag = 1;
  FILE  *fp;

  /* done so compiler wouldn't warn about unused parameter */
  hostname=hostname;

  sprintf(cmd, "/bin/cp %s/%s %s/%s.tmp", db_dir, tables[PRINTERS_ID],
          db_dir, tables[PRINTERS_ID]);
  if (system(cmd) != 0) {
    return(0);
  }

  j = -1;
  Apps = getAppData(&num_apps, db_dir, WITHOUT_NTCSS_GROUP);
  Prts = getPrtData(&num_prts, db_dir, Apps, num_apps);
  for (i = 0; i < num_prts; i++)
    if ((strcmp(prt_name, Prts[i]->prt_name) == 0)) {
      j = i;
      Prts[i]->marker = 0;
    } else
      Prts[i]->marker = 1;
  
  if (j == -1) {
    freeAppData(Apps, num_apps);
    freePrtData(Prts, num_prts);
    sprintf(cmd, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
    (void) unlink(cmd);
    return(ERROR_NO_SUCH_PRT);
  }

  if (status)
    Prts[j]->status |= mask;
  else if (Prts[j]->status & mask)
    Prts[j]->status ^= mask;

  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PRINTERS_ID]);
  if ((fp = fopen(datafile, "w")) != NULL) {
    writePrtData(Prts, num_prts, fp);
    fprintf(fp, PRINTERS_FMT,
            Prts[j]->prt_name,
            Prts[j]->hostname,
            Prts[j]->prt_location,
            Prts[j]->max_size,
            Prts[j]->prt_class_title,
            Prts[j]->ip_address,
            Prts[j]->security_class,
            Prts[j]->status,
            Prts[j]->port_name,
            Prts[j]->suspend_flag,
            Prts[j]->redirect_prt,
            Prts[j]->FileName);
    closeAndEncrypt(fp, datafile);
  } else
    flag = 0;

  freeAppData(Apps, num_apps);
  freePrtData(Prts, num_prts);

  if (flag) {
    sprintf(cmd, "/bin/mv %s/%s.tmp %s/%s", db_dir, tables[PRINTERS_ID],
            db_dir, tables[PRINTERS_ID]);
    (void) system(cmd); /* FIX if fails we have corrupted DB */
    sprintf(cmd, "%s/%s", db_dir, tables[PRINTERS_ID]);
    (void) chmod(cmd, S_IRUSR | S_IWUSR);
  }

  /* i = timestampDatabase(db_dir); No longer used */

  return(flag);
}


/*****************************************************************************/

int  returnPrtStatus( const char  *prt_name,
                      const char  *hostname,
                      const char  *db_dir )

{
  PrtItem **Prts;
  int num_prts;
  int i, retval = -1;

  /* done so compiler wouldn't warn about unused parameter */
  hostname=hostname;

  Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
  for (i = 0; i < num_prts; i++) {
    if ((strcmp(prt_name, Prts[i]->prt_name) == 0)) {
      retval = Prts[i]->status;
      break;
    }
  }
  freePrtData(Prts, num_prts);

  return(retval);
}


/*****************************************************************************/

int  getPrtList( const char  *app,
                 int          prt_access, /* == 1 if want general access */
                 char        *query,
                 const char  *db_dir )

{
  PrtItem **Prts;
  PrtclassItem **Prtclasses;
  int num_prts, num_prtclasses, lock_id, nprts, i, j;
  int que_enabled, prt_enabled, leng;
  FILE *fp;
  char ntdevice[MAX_FILENAME_LEN];
  char driver_file[MAX_FILENAME_LEN];
  char ui_file[MAX_FILENAME_LEN];
  char data_file[MAX_FILENAME_LEN];
  char help_file[MAX_FILENAME_LEN];
  char monitor_file[MAX_FILENAME_LEN];
  char monitor_name[MAX_CMD_LINE_ARGS_LEN];
  char ntfn[MAX_FILENAME_LEN];
  char cf[MAX_FILENAME_LEN], cf_list[MAX_CMD_LEN],cf_tag[MAX_FILENAME_LEN];
  int cf_cnt;
  char apphostname[MAX_HOST_NAME_LEN + 1];

  fp = fopen(query, "w");
  if (fp == NULL)
    return(ERROR_CANT_CREATE_QUERY);
  fclose(fp);

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  sprintf(ntfn, "%s/%s", NTCSS_DB_DIR, NT_INI_FILE);
  
  switch (prt_access) {
    /* By Application Case */
  case 0:
    if (!doesProgramGroupExist(app, db_dir)) {
      unsetReadDBLock(lock_id);
      return(ERROR_NO_SUCH_GROUP);
    }
    
    Prts = getAppPrtData(&num_prts, db_dir, app);
    if (num_prts < 0) {
      unsetReadDBLock(lock_id);
      freeAppPrtData(Prts, num_prts);
      return(ERROR_NO_PRT_INFO_FOUND);
    }

    Prtclasses = getPrtclassData(&num_prtclasses, db_dir);
    if (num_prtclasses < 0) {
      unsetReadDBLock(lock_id);
      freeAppPrtData(Prts, num_prts);
      freePrtclassData(Prtclasses, num_prtclasses);
      return(ERROR_NO_PRT_CLS_INFO_FOUND);
    }
    break;

    /* General Access Case */
  case 1:
    Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
    if (num_prts < 0) {
      unsetReadDBLock(lock_id);
      freePrtData(Prts, num_prts);
      return(ERROR_NO_PRT_INFO_FOUND);
    }

    Prtclasses = getPrtclassData(&num_prtclasses, db_dir);
    if (num_prtclasses < 0) {
      unsetReadDBLock(lock_id);
      freePrtData(Prts, num_prts);
      freePrtclassData(Prtclasses, num_prtclasses);
      return(ERROR_NO_PRT_CLS_INFO_FOUND);
    }
    break;
  } /* end switch */

  unsetReadDBLock(lock_id);

/*
 * Determine which printers fit the criteria.
 */
  nprts = 0;
  for (i = 0; i < num_prts; i++) {
    Prts[i]->marker = 0;

    /* check to see if printer has a valid ntlink */
    memset(ntdevice, '\0', MAX_FILENAME_LEN);
    GetProfileString("DRIVERLINK", Prts[i]->prt_name,
                     "NONE", ntdevice, MAX_FILENAME_LEN, ntfn);
    if (!strcmp(ntdevice, "NONE"))
    continue;

    if ((access && isPrtAccessGeneral(Prts[i]->status)) ||
        (!access && !isPrtAccessGeneral(Prts[i]->status))) {
/*
 * Determine index for the printer class.
 */
      for (j = 0; j < num_prtclasses; j++) {
        if (strcmp(Prts[i]->prt_class_title,
                   Prtclasses[j]->prt_class_title) == 0) {


          Prts[i]->marker = 1;
          Prts[i]->suspend_flag = j;
          nprts++;
          break;
        }
      }
    }
  }

  leng = bundleQuery(query, "I", nprts);

  for (i = 0; i < num_prts; i++) {
    if (Prts[i]->marker) {
      if (isQueueEnabled(Prts[i]->status))
        que_enabled = 1;
      else
        que_enabled = 0;
      if (isPrtEnabled(Prts[i]->status))
        prt_enabled = 1;
      else
        prt_enabled = 0;
      j = Prts[i]->suspend_flag;

      memset(ntdevice, '\0', MAX_FILENAME_LEN);
      GetProfileString("DRIVERLINK", Prts[i]->prt_name,
                       "NONE", ntdevice, MAX_FILENAME_LEN, ntfn);
      
      memset(apphostname, '\0', MAX_HOST_NAME_LEN + 1);
      getAppHostname(app, apphostname, NTCSS_DB_DIR);

      leng += bundleQuery(query, "CCCCII", Prts[i]->prt_name,
                          apphostname,
                          Prts[i]->prt_location,
                          /*
                            Prtclasses[j]->prt_device_name,
                          */
                          ntdevice,
                          que_enabled,
                          prt_enabled);

      if (isBlank(Prts[i]->redirect_prt))
        leng += bundleQuery(query, "C", "***");
      else
        leng += bundleQuery(query, "C", Prts[i]->redirect_prt);

      if (!strcmp(ntdevice,"NONE")) {
        leng += bundleQuery(query,"C","NONE");
        leng += bundleQuery(query,"C","NONE");
        leng += bundleQuery(query,"C","NONE");
        leng += bundleQuery(query,"C","NONE");
        leng += bundleQuery(query,"C","NONE");
        leng += bundleQuery(query,"C","NONE");
      } else {
        GetProfileString(ntdevice, "DRIVER", "NONE",
                         driver_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", driver_file);
        
        GetProfileString(ntdevice, "DATA", "NONE",
                         data_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", data_file);

        GetProfileString(ntdevice, "UI", "NONE",
                         ui_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", ui_file);

        GetProfileString(ntdevice, "HELP", "NONE",
                         help_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", help_file);

        GetProfileString(ntdevice, "MONITOR", "NONE",
                         monitor_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", monitor_file);

        GetProfileString(ntdevice, "MONITORNAME", "NONE",
                         monitor_name, MAX_CMD_LINE_ARGS_LEN, ntfn);
        leng += bundleQuery(query, "C", monitor_name);
      }
    /* loop for all the copy files */
    memset(cf_list,'\0',MAX_CMD_LEN);

    for (cf_cnt = 1; cf_cnt < MAX_COPY_FILES; cf_cnt++) {
        sprintf(cf_tag, "%s_%.02d", COPY_FILE_PREFIX, cf_cnt);
        GetProfileString(ntdevice, cf_tag, NO_MORE_COPY_FILES, cf,
                         MAX_FILENAME_LEN, ntfn);

        if (!strcmp(cf, NO_MORE_COPY_FILES))
            break;
        if (strlen(cf_list))
            strcat(cf_list, ",");
        strcat(cf_list, cf);
    }
        if (strlen(cf_list) == 0)
                strcpy(cf_list, "NONE");

        leng += bundleQuery(query, "C", cf_list);

        leng += bundleQuery(query, "C", Prts[i]->FileName);
    }
  }

  freePrtclassData(Prtclasses, num_prtclasses);
  if (prt_access == 1)
    freePrtData(Prts, num_prts);
  else
    freeAppPrtData(Prts, num_prts);

  return(leng);   /* everthing ok, printer data is returned */
}


/*****************************************************************************/

int  getUserPrtList( const char  *login_name,
                     const char  *app,
                     const char  *otype,
                     int   sec_class,
                     int   def_only,/*def_only = 1 if want default prt only*/
                     int   prt_access,  /* == 1 if want general access */
                     char        *query,
                     const char  *db_dir )
{
  UserItem        **Users;
  PrtItem         **Prts;
  PrtclassItem    **Prtclasses;
  OutputTypeItem  **Otypes;
  int    num_users, num_prts, lock_id, nprts;
  int    i, j, k;
  int    num_prtclasses, num_otypes;
  int    user_sec_class, que_enabled, prt_enabled;
  char   printer[MAX_PRT_NAME_LEN + MAX_HOST_NAME_LEN + 8];
  int    leng;
  FILE  *fp;
  char   apphostname[MAX_HOST_NAME_LEN+1];
  char   driver_file[MAX_FILENAME_LEN];
  char   data_file[MAX_FILENAME_LEN];
  char   help_file[MAX_FILENAME_LEN];
  char   monitor_file[MAX_FILENAME_LEN];
  char   monitor_name[MAX_CMD_LINE_ARGS_LEN];
  char   ntfn[MAX_FILENAME_LEN];
  char   ntdevice[MAX_FILENAME_LEN];
  char   cf[MAX_FILENAME_LEN];
  int    cf_cnt;
  char   cf_list[MAX_CMD_LEN];
  char   cf_tag[MAX_FILENAME_LEN];
  char   ui_file[MAX_FILENAME_LEN];


  fp = fopen(query, "w");
  if (fp == NULL)
    return(ERROR_CANT_CREATE_QUERY);
  fclose(fp);

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  sprintf(ntfn, "%s/%s", NTCSS_DB_DIR, NT_INI_FILE);

  Users = getUserData(&num_users, db_dir);

  if (prt_access)   /* general access */
    Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
  else      /* app related */
    Prts = getAppPrtData(&num_prts, db_dir, app);

  if (num_prts < 0) {
      freeUserData(Users, num_users);
      unsetReadDBLock(lock_id);
      return(ERROR_NO_PRT_INFO_FOUND);
  }

  Prtclasses = getPrtclassData(&num_prtclasses, db_dir);
    if (num_prtclasses < 0) {
      freeUserData(Users, num_users);
      if (prt_access == 1)
        freePrtData(Prts, num_prts);
      else
        freeAppPrtData(Prts, num_prts);
      unsetReadDBLock(lock_id);
      return(ERROR_NO_PRT_CLS_INFO_FOUND);
  }

  Otypes = getAppOutputTypeData(&num_otypes, db_dir, app,
                                  Prts, num_prts);
  if (num_otypes < 0) {
      freeUserData(Users, num_users);
      if (prt_access == 1)
        freePrtData(Prts, num_prts);
      else
        freeAppPrtData(Prts, num_prts);
      freePrtclassData(Prtclasses, num_prtclasses);
      unsetReadDBLock(lock_id);
      return(ERROR_NO_OTYPE_INFO_FOUND);
  }
  unsetReadDBLock(lock_id);

  user_sec_class = -1;
  for (i = 0; i < num_users; i++) {
    if (strcmp(Users[i]->login_name, login_name) == 0) {
      user_sec_class = Users[i]->security_class;
      break;
    }
  }

  if (user_sec_class == -1) {
    freeUserData(Users, num_users);
    if (prt_access == 1)
        freePrtData(Prts, num_prts);
    else
        freeAppPrtData(Prts, num_prts);
    freePrtclassData(Prtclasses, num_prtclasses);
    freeAppOutputTypeData(Otypes, num_otypes);
    return(ERROR_NO_SUCH_USER);
  }

  switch (prt_access) {
    /* General Access Case */
  case 1:
/*
 * Determine which printers fit the criteria.
 */
    nprts = 0;
    for (i = 0; i < num_prts; i++) {
      Prts[i]->marker = 0;
      Prts[i]->max_size = 0;
      Prts[i]->suspend_flag = -1;

      /* check to see if printer has a valid ntlink */
      memset(ntdevice, '\0', MAX_FILENAME_LEN);
      GetProfileString("DRIVERLINK", Prts[i]->prt_name,
                       "NONE", ntdevice, MAX_FILENAME_LEN, ntfn);
      if (!strcmp(ntdevice, "NONE"))
                continue;

      if ((Prts[i]->security_class <= user_sec_class) &&
          (Prts[i]->security_class >= sec_class) &&
          isPrtAccessGeneral(Prts[i]->status)) {
/*
 * Determine index for the printer class.
 */
        for (j = 0; j < num_prtclasses; j++) {
          if (strcmp(Prts[i]->prt_class_title,
                     Prtclasses[j]->prt_class_title) == 0) {

            Prts[i]->marker = 1;
            Prts[i]->suspend_flag = j;
            nprts++;
            break;
          }
        }
      }
    }
    break;

    /* By Application Case */
  case 0:


    if (!doesProgramGroupExist(app, db_dir)) {
      freeUserData(Users, num_users);
      if (prt_access == 1)
        freePrtData(Prts, num_prts);
      else
        freeAppPrtData(Prts, num_prts);
      freePrtclassData(Prtclasses, num_prtclasses);
      freeAppOutputTypeData(Otypes, num_otypes);
      return(ERROR_NO_SUCH_GROUP);
    }
/*
 * Find the index of the output type.
 */

    j = -1;
    for (i = 0; i < num_otypes; i++) {
      if (strcmp(Otypes[i]->out_type_title, otype) == 0) {
        j = i;
        break;
      }
    }
    if (j == -1) {
      freeUserData(Users, num_users);
      if (prt_access == 1)
        freePrtData(Prts, num_prts);
      else
        freeAppPrtData(Prts, num_prts);
      freePrtclassData(Prtclasses, num_prtclasses);
      freeAppOutputTypeData(Otypes, num_otypes);
      return(ERROR_NO_SUCH_OTYPE);
    }
/*
 * Determine which printers fit the basic criteria.
 */
    nprts = 0;
    for (i = 0; i < num_prts; i++) {
      Prts[i]->marker = 0;
      Prts[i]->max_size = 0;
      Prts[i]->suspend_flag = -1;

      /* check to see if printer has a valid ntlink */
      memset(ntdevice, '\0', MAX_FILENAME_LEN);
      GetProfileString("DRIVERLINK", Prts[i]->prt_name,
                     "NONE", ntdevice, MAX_FILENAME_LEN, ntfn);
      if (!strcmp(ntdevice, "NONE"))
        continue;

      if ((Prts[i]->security_class > user_sec_class) ||
          (Prts[i]->security_class < sec_class) ||
          isPrtAccessGeneral(Prts[i]->status))
        continue;
/*
 * Determine if printer is the default.
 */

      strcpy(printer,Prts[i]->prt_name);

      if (strcmp(printer, Otypes[j]->default_prt) == 0)
        Prts[i]->max_size = 1;
/*
 * Determine if printer is associated with the output type.
 */
      for (k = 0; k < num_prts; k++) {
        if (!Otypes[j]->output_prt[k])
          continue;
        if ((strcmp(Prts[i]->prt_name, Prts[k]->prt_name) == 0)) {
          Prts[i]->suspend_flag = -2;
          break;
        }
      }
/*
 * Determine index for the printer class.
 */
      if (Prts[i]->suspend_flag == -2) {
        for (k = 0; k < num_prtclasses; k++) {
          if (strcmp(Prts[i]->prt_class_title,
                     Prtclasses[k]->prt_class_title) == 0) {
            Prts[i]->marker = 1;
            Prts[i]->suspend_flag = k;
            nprts++;
            break;
          }
        }
      }
    }
/*
 * Keep only the default printer, if desired.
 */
    if (def_only) {
      nprts = 0;
      for (i = 0; i < num_prts; i++)
        if (Prts[i]->marker && Prts[i]->max_size)
          nprts++;
        else
          Prts[i]->marker = 0;
    }
    break;
  } /* end switch */

  leng = bundleQuery(query, "I", nprts);
  
  for (i = 0; i < num_prts; i++) {
    if (Prts[i]->marker) {
      if (isQueueEnabled(Prts[i]->status))
        que_enabled = 1;
      else
        que_enabled = 0;
      if (isPrtEnabled(Prts[i]->status))
        prt_enabled = 1;
      else
        prt_enabled = 0;
      j = Prts[i]->suspend_flag;
      
      memset(ntdevice,'\0',MAX_FILENAME_LEN);
      GetProfileString("DRIVERLINK", Prts[i]->prt_name,
                       "NONE", ntdevice, MAX_FILENAME_LEN, ntfn);

      memset(apphostname, '\0', MAX_HOST_NAME_LEN + 1);
      getAppHostname(app, apphostname, NTCSS_DB_DIR);

      leng += bundleQuery(query, "CCCICCII", 
                          Prts[i]->prt_name,
                          apphostname,
                          Prts[i]->prt_location,
                          /* max size really indicates default printer */
                          Prts[i]->max_size,
                          ntdevice,
                          Prtclasses[j]->prt_driver_name,
                          que_enabled,
                          prt_enabled);
      
      if (isBlank(Prts[i]->redirect_prt))
        leng += bundleQuery(query, "C", "***");
      else
        leng += bundleQuery(query, "C", Prts[i]->redirect_prt);

      if (!strcmp(ntdevice, "NONE")) {
        leng += bundleQuery(query, "C", "NONE");
        leng += bundleQuery(query, "C", "NONE");
        leng += bundleQuery(query, "C", "NONE");
        leng += bundleQuery(query, "C", "NONE");
        leng += bundleQuery(query, "C", "NONE");
        leng += bundleQuery(query, "C", "NONE");
      } else {
        memset(driver_file, '\0', MAX_FILENAME_LEN);
        GetProfileString(ntdevice, "DRIVER", "NONE",
                         driver_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query,"C",driver_file);
        
        memset(data_file, '\0', MAX_FILENAME_LEN);
        GetProfileString(ntdevice, "DATA", "NONE",
                         data_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", data_file);

        memset(ui_file, '\0', MAX_FILENAME_LEN);
        GetProfileString(ntdevice, "UI", "NONE",
                         ui_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", ui_file);

        memset(help_file, '\0', MAX_FILENAME_LEN);
        GetProfileString(ntdevice, "HELP", "NONE",
                         help_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query,"C",help_file);

        memset(monitor_file, '\0', MAX_FILENAME_LEN);
        GetProfileString(ntdevice, "MONITOR", "NONE",
                         monitor_file, MAX_FILENAME_LEN, ntfn);
        leng += bundleQuery(query, "C", monitor_file);

        /*memset(monitor_name, '\0', MAX_FILENAME_LEN);*/
        memset(monitor_name, '\0', MAX_CMD_LINE_ARGS_LEN);
        GetProfileString(ntdevice, "MONITORNAME", "NONE",
                         monitor_name, MAX_CMD_LINE_ARGS_LEN,
                         ntfn);
        leng += bundleQuery(query, "C", monitor_name);

    /* loop for all the copy files */
    memset(cf_list, '\0', MAX_CMD_LEN);

    for (cf_cnt = 1; cf_cnt < MAX_COPY_FILES; cf_cnt++) {
        sprintf(cf_tag, "%s_%.02d", COPY_FILE_PREFIX, cf_cnt);
        GetProfileString(ntdevice, cf_tag, NO_MORE_COPY_FILES, cf,
                         MAX_FILENAME_LEN, ntfn);

        if (!strcmp(cf, NO_MORE_COPY_FILES))
            break;
        if (strlen(cf_list))
            strcat(cf_list, ",");
        strcat(cf_list, cf);
    }
    if (strlen(cf_list) == 0)
        strcpy(cf_list, "NONE");
    leng += bundleQuery(query, "C", cf_list);

      }
      if(! strcmp("None", Prts[i]->FileName) &&
         ! strcmp("FILE", Prts[i]->port_name))
        leng += bundleQuery(query, "C", "***");
      else
        leng += bundleQuery(query, "C", Prts[i]->FileName);
    }
  }

  freeUserData(Users, num_users);
  freeAppOutputTypeData(Otypes, num_otypes);
  freePrtclassData(Prtclasses, num_prtclasses);
  if (prt_access == 1)
    freePrtData(Prts, num_prts);
  else
    freeAppPrtData(Prts, num_prts);

  return(leng);   /* everthing ok, user printer data is returned */
}


/*****************************************************************************/

int  validatePrt( const char  *prt,
                  const char  *host,
                  const char  *db_dir )
{
  int    i;
  int    lock_id;
  int    num_prts;
  PrtItem  **Prts;

  /* done so compiler wouldn't warn about unused parameter */
  host = host;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      return(ERROR_CANT_ACCESS_DB);
    }

  Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
  unsetReadDBLock(lock_id);

  if (num_prts < 0)
    {
      freePrtData(Prts, num_prts);
      return(ERROR_NO_PRT_INFO_FOUND);
    }
/*
 * Determine if printer is valid.
 */
  for (i = 0; i < num_prts; i++)
    {
      if ((strcmp(Prts[i]->prt_name, prt) == 0))
        {
          freePrtData(Prts, num_prts);
          return(1);
        }
    }
  freePrtData(Prts, num_prts);

  return(0);
}


/*****************************************************************************/

int  getDefPrtOtype( const char  *app,
                     const char  *otype,
                     char        *prt,
                     char        *host,
                     const char  *db_dir )

{
  PrtItem         **Prts;
  OutputTypeItem  **Otypes;
  int   num_prts;
  int   lock_id;
  int   num_otypes;
  int   i, j;
  char  printer[MAX_PRT_NAME_LEN + MAX_HOST_NAME_LEN + 8];

  if (!doesProgramGroupExist(app, db_dir))
    {
      return(ERROR_NO_SUCH_GROUP);
    }

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);

  Prts = getAppPrtData(&num_prts, db_dir, app);
  Otypes = getAppOutputTypeData(&num_otypes, db_dir, app,
                                (PrtItem **) 0, 0);
  unsetReadDBLock(lock_id);

  if (num_prts < 0) {
    freeAppPrtData(Prts, num_prts);
    return(ERROR_NO_PRT_INFO_FOUND);
  }

  if (num_otypes < 0) {
    freeAppPrtData(Prts, num_prts);
    freeAppOutputTypeData(Otypes, num_otypes);
    return(ERROR_NO_OTYPE_INFO_FOUND);
  }
/*
 * Find the index of the output type.
 */
  j = -1;
  for (i = 0; i < num_otypes; i++) {
    if (strcmp(Otypes[i]->out_type_title, otype) == 0) {
      j = i;
      break;
    }
  }
  if (j == -1) {
    freeAppPrtData(Prts, num_prts);
    freeAppOutputTypeData(Otypes, num_otypes);
    return(ERROR_NO_SUCH_OTYPE);
  }
/*
 * Find the appropriate printer.
 */
  for (i = 0; i < num_prts; i++) {
    if (isPrtAccessGeneral(Prts[i]->status))
      continue;
/*
 * Determine if printer is the default.
 */
    strcpy(printer,Prts[i]->prt_name);
    if (strcmp(printer, Otypes[j]->default_prt) == 0) {
      strcpy(prt, Prts[i]->prt_name);
      strcpy(host, Prts[i]->hostname);
      freeAppPrtData(Prts, num_prts);
      freeAppOutputTypeData(Otypes, num_otypes);
      return(1);
    }
  }
  freeAppPrtData(Prts, num_prts);
  freeAppOutputTypeData(Otypes, num_otypes);
  strcpy(prt, "***");
  strcpy(host, "***");

  return(0);
}


/*****************************************************************************/

int  getAppPrtLockId( const char  *app,
                      const char  *db_dir )

{
  char   datafile[MAX_FILENAME_LEN];
  char   decryptfile[MAX_FILENAME_LEN];
  char   file_app[MAX_ITEM_LEN];
  FILE  *fp;
  int    id;

  sprintf(datafile, "%s/APP_ID", db_dir);
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      remove(decryptfile);
      return(-1);
    }

  id = 0;
  while (fgets(file_app, MAX_ITEM_LEN, fp) != NULL)
    {
      if (id >= MAX_NUM_PROGROUPS)
        {
          fclose(fp);
          return(-1);
        }
      if (app && (strncmp(app, file_app, strlen(app)) == 0))
        {
          id++;
          break;
        }
      id++;
    }
  fclose(fp);
  remove(decryptfile);

  return(id);
}


/*****************************************************************************/

int  getDirListing( const char  *dir,
                    char        *query,
                    char        *msg,
                    int          AsciiOnly )
{
  char   buf[MAX_FILENAME_LEN];
  char   str[32];
  struct passwd  *p;
  char   timebuf[DATETIME_LEN + 1];
  char   cmdbuf[MAX_FILENAME_LEN];
  FILE  *pfp;
  
/*
#ifdef SOLARIS
  struct dirent *direc;
#else
  struct direct *direc;
#endif
*/
  struct stat  sbuf;
  int   leng = 0;
  int   num_files = 0;
  FILE *fp;

  if (stat(dir,&sbuf) == -1 || ! S_ISDIR(sbuf.st_mode)) 
    return(-1);

  fp = fopen(query, "w");
  if (fp == NULL)
    return(ERROR_CANT_CREATE_QUERY);
  fclose(fp);

  memset(cmdbuf, '\0', MAX_FILENAME_LEN);
        
  if(AsciiOnly)
    sprintf(cmdbuf, "cd %s;file * | grep text | cut -d: -f1", dir);
  else
    sprintf(cmdbuf," cd %s;file * | cut -d: -f1", dir);

  pfp = popen(cmdbuf, "r");

  while (!feof(pfp))
    {
      memset(cmdbuf, '\0', MAX_FILENAME_LEN);
      if (fgets(cmdbuf, MAX_FILENAME_LEN, pfp) != NULL)
        {
          cmdbuf[strlen(cmdbuf)-1] = '\0';
          memset(buf, '\0', MAX_FILENAME_LEN);
          sprintf(buf, "%s/%s", dir, cmdbuf);
          if (stat(buf, &sbuf) == -1)
            continue;

          if ((sbuf.st_mode & S_IFMT) == S_IFREG)
            {
              p = getpwuid(sbuf.st_uid);
              (void) strftime(timebuf, TIME_STR_LEN + 1, "%y%m%d.%H%M", 
                              localtime(&sbuf.st_mtime));
              leng += bundleQuery(query, "CDIC", cmdbuf, timebuf, sbuf.st_size,
                                  p->pw_name);
              num_files++;
            }
        }
    }

  pclose(pfp);

/*
    if ((dp = opendir(dir)) == NULL)
        return(ERROR_CANT_ACCESS_DIR);
        
    while ((direc = readdir(dp)) != NULL) {
        if (direc->d_ino == 0)
            continue;
        if (strcmp(dir, "/") == 0)
            sprintf(buf, "/%s", direc->d_name);
        else
            sprintf(buf, "%s/%s", dir, direc->d_name);
        if (stat(buf, &sbuf) == -1)
            continue;
        if(AsciiOnly && IsNotTextFile(buf))  
           continue;
        if ((sbuf.st_mode & S_IFMT) == S_IFREG) {
            p=getpwuid(sbuf.st_uid);
                (void) strftime(timebuf, TIME_STR_LEN + 1, 
                "%y%m%d.%H%M", localtime(&sbuf.st_mtime));
            leng += bundleQuery(query, "CDIC", 
                        direc->d_name,
                        timebuf,
                        sbuf.st_size,
                        p->pw_name);
            num_files++;
        }
    }
    closedir(dp);
*/

  sprintf(str, "%d", num_files);
  sprintf(buf, "%d", leng + strlen(str) + 1);
  sprintf(str, "%1d", strlen(buf));
  strcat(msg, str);
  strcat(msg, buf);

  sprintf(buf, "%d", num_files);
  sprintf(str, "%1d", strlen(buf));
  strcat(msg, str);
  strcat(msg, buf);

  return(leng);   /* everthing ok, directory listing is returned */
}


/*****************************************************************************/

int  IsNotTextFile( const char *filename )

{
  char  buf[2 * MAX_PRT_FILE_LEN];
  char  line[2 * MAX_PRT_FILE_LEN];
  FILE *pp;

  sprintf(buf, "%s %s", FILE_PROG, filename);
  pp = popen(buf, "r");
  if (pp == NULL)
    return(ERROR_CANT_OPEN_FILE);
 
  if (fgets(line, 2 * MAX_PRT_FILE_LEN, pp) == NULL)
    {
      pclose(pp);
      return(ERROR_CANT_READ_VALUE);
    }

  pclose(pp);
  return (strstr(line, "text") != NULL ? 0 : 1);
}
   
  
/*****************************************************************************/
/* Printer admin control */

int  sendPrinterAdminToDm( const char  *strHostList,
                           const char  *strFileName,
                           char        *strHostResponses )
{
  char  strMsg[CMD_LEN];
  int  ret;

  memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, MT_PRINTADMIN, strlen(MT_PRINTADMIN));

  ret = sendAdminToDm(strHostList, strFileName, strHostResponses, strMsg);

  return(ret);
}


/*****************************************************************************/
int  getAppHostname( const char  *appname,
                     char        *apphost,
                     const char  *db_dir )
{
  AppItem  **Apps;
  int    i, num_apps;

  Apps = getAppData(&num_apps, db_dir, WITHOUT_NTCSS_GROUP);
  for (i = 0; i < num_apps; i++)
    {
      if ((strcmp(appname, Apps[i]->progroup_title) == 0))
        {
          strcpy(apphost, Apps[i]->hostname);
          freeAppData(Apps, num_apps);
          return(0);
        }
    }
  
  /* did not find app host*/
  freeAppData(Apps, num_apps);
  return(1);
}


/*****************************************************************************/
/* 
   Determine the printer redirection status and return the new printer 
   name if needed
*/

int   getPrtRedirectionStatus( const char  *strSourcePrinter,
                               char        *strNewPrinter )
{
  PrtItem **Prts;
  int num_prts;
  int i,retval;

  retval=0;     /* this means no redirection */
  Prts = getPrtData(&num_prts, NTCSS_DB_DIR, (AppItem **) 0, 0);

  for (i = 0; i < num_prts; i++)
    {
      
      /* if the printer name matches and there is a red printer other than
         NONE, it is a keeper */
      if (strcmp(strSourcePrinter, Prts[i]->prt_name) == 0)
        {
          if (strlen(Prts[i]->redirect_prt) > 0 && 
              strcmp(Prts[i]->redirect_prt, "NONE") && 
              strcmp(Prts[i]->redirect_prt, "None"))
            {
              strcpy(strNewPrinter, Prts[i]->redirect_prt); 
              retval = 1;
            }
          break;
        }
    }
  freePrtData(Prts, num_prts);

  return(retval);
}


/*****************************************************************************/
