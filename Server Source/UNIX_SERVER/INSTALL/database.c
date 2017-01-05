
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * database.c
 */

/* Some functions here return 0 as an error condition, against usual UNIX conventions.  Any new functions should use 0 only for successful conditions. */

#define RECSIZE (size_t) 128
#define GROUP_SIZE (size_t) 32

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <search.h>
#include <errno.h>
#include <netdb.h>
#include <sys/signal.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <ini_Fcts.h>
#include <TextDBP.h>
#include <ExtraProtos.h>
#include <NetCopy.h>
#include <SockFcts.h>


/** LOCAL INCLUDES **/

#include "install.h"


/** FUNCTION PROTOTYPES **/
extern int   getHostTypeFromInitData(char*); /* Found in prime_db.c */
extern int   getMasterNameFromInitData(char*); /* Found in prime_db.c */
extern void  print_out(const char*);

int  are_there_newly_registered_apps(char*);
int  convert_database_files(char*, char*);
int  getIniRecord(DBRecord*, char*, char*, char*);
int  init_database(char*, char*, char*, int, char*);
int  process_newly_registered_apps(char*, char*, const char*, const char*);
int  ConvertUsersFor3_0_0(char*);
int  DBconvert0_1_1to0_2_0(char*);
int  DBconvert0_2_0to0_3_0(char*);
int  DBconvert0_3_0to0_4_0(char*);
int  DBconvert0_4_0to0_5_0(char*);
int  DBconvert0_5_0to1_0_0(char*);
int  DBconvert1_0_0to1_0_1(char*);
int  DBconvert1_0_1to1_0_2(char*);
int  DBconvert1_0_7to3_0_0(char*);
int  DBconvert3_0_0to3_0_1(char*);
int  DBconvert3_0_3to3_0_4(char*);
int  GetEncryptedBatchPassword(char *,char *);
int MergeData107to300(char *);
int  Copy_Legacy_DB_Files(char *);
int FixSysConfigDataTags(char *);
int TrimOutputTypes(char *db_dir);
int prep_check_users_in_unix(void);
int call_check_users(char*);
int remove_brackets(char*,char*);
int split_left(char*,char*);
int split_right(char*,char*);
int read_ntcss_system_settings(const char*,char*);
void settings_status(int,const char*);

char  getJobCode(char*);
char  getSecurityCode(char*);
char*  makeAccessString(char*, char*, char*, char*);
char*  getRoleNumber(char*, char*, char*, char*);
void  CopyProfileSection(char*, char*, char*);
static int  add_env_dir(char*, char*);
static int  add_env_file(char*, char*, char*);
static int  add_PSP_file(char*, char*, char*);
static int  add_SIG_file(char*, int, char*);
static int  convert_versions(char*, char*);

int  SaveAdminsFor300Convert(char*);
int  RestoreAdminsFor300Conversion(char*);
int ConvertHostType(char*);
int ConvertRepl(char*);

static int GetAdapterType(const char*,char*,char*);
void transfer_printers(char *);


/** STATIC VARIABLES **/
int nHostType;

/** GLOBAL VARIABLES **/

/** FUNCTION DEFINITIONS **/


/*****************************************************************************/

int  init_database(dir, sql, db_dir, which, argfile)

     char  *dir;
     char  *sql;
     char  *db_dir;
     int  which;
     char  *argfile;

{
  int  i, j, k, nCounter;
  int  retval = 1;
  int  nis_running;
  int  signum;
  int  x;            /* general purpose var */
  FILE  *rp;
  FILE  *wp;
  FILE  *ap;
  FILE  *aip;
  FILE  *cap, *hostFp;
  struct stat  sbuf;
  DBRecord  record;
  HostItem **hostItems;

  char  access_[MAX_FILENAME_LEN];
  char  strHostFileName[MAX_FILENAME_LEN];
  char  buf[MAX_FILENAME_LEN];
  char  cmd[MAX_CMD_LEN];
  char  err_msg[MAX_ERR_MSG_LEN];      /* 320 chars as of 6/98 */
  char  group[MAX_INSTALL_ITEM_LEN];
  char  homedir[MAX_FILENAME_LEN];
  char  host[MAX_INSTALL_ITEM_LEN];
  char  hosttype[MAX_INI_ENTRY];  
  char  strLocalHost[MAX_HOST_NAME_LEN+1];
  char  ipaddr[MAX_INI_ENTRY];
  char  repl[MAX_INI_ENTRY];
  char  sched[MAX_SCHED_STR_LEN+1];
  char  *sptr;
  char  strNewAccessString[MAX_INI_ENTRY];
  char  tbuf[MAX_INI_ENTRY * 100];
  char  tbuf2[MAX_INI_ENTRY * 100];
  char  tbuf3[MAX_INI_ENTRY * 100];
  char  *tptr,Sbuf[MAX_INSTALL_ITEM_LEN];
  char  *tptr2;
  char  lpszLockOverrideValue[MAX_ACCESS_ROLE_LEN+1];
  char  lpszAccRole[MAX_ACCESS_ROLE_LEN+1];
  char  lpszRolesSections[MAX_INI_ENTRY * 100];
  char  *pCh;
  char  lpszAccessRoleSectionHeaders[MAX_ACCESS_ROLE_LEN+1];
  int   nContinue;
  char  strEncryptedPassword[MAX_PASSWORD_LEN + MAX_PASSWORD_LEN + 1];
  char  uc_path[12];
  char  ntcss_home_dir[RECSIZE];

/*
 * Get UNIX home directory (if needed).
 */
  if (add_unix_batch_users)
    {
      sprintf(cmd, "SPECIAL NO LOCK BACKDOOR%s", db_dir);
      x = getNtcssSysConfData("NTCSS HOME DIR", homedir, cmd);
      if (x < 0)
        {
          fprintf(logp, "Init_database(): Cannot determine UNIX home"
                  " directory for adding batch users!\n");
          return(0);
        }
    }

/*
* Set path
*/
    x=read_ntcss_system_settings("HOME_DIR",ntcss_home_dir);
    ntcss_home_dir[ strlen(ntcss_home_dir) - 1 ] = 0;
    if (x < 0)
    {
        fprintf(logp, "WARNING: (Init_Database) Cannot determine NTCSS home"
                " directory for reconciling users.\n");
        return(-1);
    }
    strcpy(uc_path,"PATH=$PATH:");
    strcat(uc_path,ntcss_home_dir);
    strcat(uc_path,"/bin;export PATH");
    system(uc_path);
    /*printf ("PATH: %s\n",uc_path);*/

/*
 * Open the target SQL file.
 */
  wp = fopen(sql, "w");
  if (wp == NULL)
    {
      fprintf(logp, "Init_database(): Cannot open file %s!\n", sql);
      return(0);
    }

  switch (which)
    {
    case APP_INFO:

/*
 * clean out the progroups file
 */
      memset(strLocalHost,NTCSS_NULLCH,MAX_HOST_NAME_LEN+1);
      gethostname(strLocalHost,MAX_HOST_NAME_LEN);

      nHostType=getHostTypeFromInitData(strLocalHost);
      if (nHostType < 0) {
      	fprintf(logp, "Unable to determine host type for <%s>\n",strLocalHost);
      	printf("Unable to determine host type for <%s>, Halting\n",
			strLocalHost);
	return(0);
      }

      fprintf(logp, "Init_database(): Resetting the prgroups DB...\n");
      sprintf(tbuf,"%s/progroups",db_dir);
      ap=fopen(tbuf,"w");
      if (ap)
	fclose(ap);


/*
 * Create new access roles file.
 */
      fprintf(logp, "Init_database(): Creating access_roles DB...\n");
     
      sprintf(access_, "%s/%s", db_dir, getAccessRolesTableName());
      ap = fopen(access_, "w");
      if (ap == NULL)
        {
          fprintf(logp, "Init_database(): Cannot open file %s!\n",
                  access_);
          fclose(wp);
          return(0);
        }
/*
 * Create new application id file.
 */
      fprintf(logp, "Init_database(): Creating APP_ID file...\n");
      sprintf(buf, "%s/%s", db_dir, "APP_ID");
      aip = fopen(buf, "w");
      if (aip == NULL)
        {
          fprintf(logp, "Init_database(): Cannot open file %s!\n", buf);
          fclose(wp);
          fclose(ap);
          return(0);
        }
/*
 * Create new current apps file.
 */
      fprintf(logp, "Init_database(): Creating current apps table...\n");
      sprintf(buf, "%s/%s", db_dir, getCurrentAppsTableName());
      cap = fopen(buf, "w");
      if (cap == NULL)
        {
          fprintf(logp, "Init_database(): Cannot open file %s!\n", buf);
          fclose(ap);
          fclose(aip);
          fclose(wp);
          return(0);
        }
/*
 * Check if NIS is running.
 */
      fprintf(logp, "Init_database(): Checking NIS status ...\n");
      nis_running = nis_running_check();
      if (nis_running == 0)
        fprintf(logp, "Init_database() WARNING: Assuming NIS is not"
                " active!\n");
    
/*
 * Check program group data.
 */
      if (argfile == NULL) 
        sprintf(buf, "%s", NTCSS_PROGRP_ID_FILE);
      else
        strcpy(buf, argfile);
        
      if (stat(buf, &sbuf) == -1)
        {
          fprintf(logp, "Init_database(): Cannot get status"
                  " of file %s!\n", buf);
          fclose(ap);
          fclose(aip);
          fclose(cap);
          fclose(wp);
          return(0);
        }

      fprintf(logp, "Init_database(): Checking progroups in the INI file..\n");
      fprintf(logp, "\n************************************************\n");

      memset(tbuf, NTCSS_NULLCH, MAX_INI_ENTRY * 100);
      GetSectionValues("APPS", tbuf, MAX_INI_ENTRY * 100, buf);
      tptr = tbuf;
      if(strlen(tptr) > 0) 
        do
          {
            if (!getIniRecord(&record, tptr, tbuf2, buf))
              {
                fprintf(logp, "Init_database() ERROR: Cannot read progroup"
                        " record in file %s!\n", buf);
                retval = 0;
                continue;
              }

            fprintf(logp, "\n-------------------------------------"
                    "-------------------------\n");
            fprintf(logp, "\nInit_database(): Verifying information "
                    "on program group <%s>...\n", record.field[0].data);
            x = checkProgroupTitle(record.field[0].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database(): ERROR: \tBad progroup"
                        "title value! <%s>\n", record.field[0].data);
                retval = 0;
                continue;
              }

         /* x = checkHost(record.field[1].data, db_dir); */
            x = getHostTypeFromInitData(record.field[1].data); 
            if (x < 0)
              {
                fprintf(logp, "Init_database() ERROR: \tBad host type "
                        " for host <%s> in progroup %s!\n",
                        record.field[1].data, record.field[0].data);
                retval = 0;
                return(0);
              }

            x = checkUnixGroup(record.field[2].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERROR: \tBad UNIX group"
                        " name <%s> for progroup %s!\n", record.field[2].data,
                        record.field[0].data);
                retval = 0;
                return(0);
              }

            i = isBlank(record.field[2].data);
            if (i == 0)
                {
                  x = add_unix_group(record.field[2].data);
                  if (x == 0)
                    fprintf(logp, "Init_database() WARNING: May have to"
                            " manually add UNIX group %s!  Error %i\n",
                            record.field[2].data, x);
                }
            else
              fprintf(logp, "Init_database() WARNING: Need to"
                      " manually add the intended UNIX group for progroup "
                      "%s!\n", record.field[0].data);

            x = checkIconFile(record.field[3].data, err_msg);
            if ((x == 0) || (x == 2))     /* 2 is non-fatal */
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERR/WARNING: Bad icon file"
                        " value for progroup %s!\n", record.field[0].data);
                if (x == 0)     /* Bad error */
                  {
                    retval = 0;
                    continue;
                  }
              }

            x = checkIconIndex(record.field[4].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERROR: Bad icon index"
                        " value for progroup %s!\n", record.field[0].data);
                retval = 0;
                continue;
              }

            x = checkVersionNumber(record.field[5].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERROR: Bad version number"
                        " value for progroup %s!\n", record.field[0].data);
                retval = 0;
                return(0);
              }

            x = isValidTimeStr(record.field[6].data);
            if (x == 0)
              {
                fprintf(logp, "Init_database() ERROR: Bad release date"
                        " value for progroup %s!\n", record.field[0].data);
                retval = 0;
                return(0);
              }

            x = checkClientLocation(record.field[7].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERROR: Bad client location"
                        " value for progroup %s!\n", record.field[0].data);
                retval = 0;
                continue;
              }

            x = checkServerLocation(record.field[8].data, err_msg);
            if (x == 0)
              {
                fprintf(logp, err_msg);
                fprintf(logp, "Init_database() ERROR: Bad server location"
                        " value for progroup %s!\n", record.field[0].data);
                retval = 0;
                continue;
              }

            strcpy(group, record.field[0].data);
            strcpy(host, record.field[1].data);
 /*
 * Add environment and PSP files for each program group.
 */
            fprintf(logp, "\nInit_database(): Setting up environment"
                    " files and PSP files for program group <%s>...\n",
                    record.field[0].data);
            x = add_env_dir(record.field[0].data, db_dir);
            if (x == 0)
              {
                fprintf(logp, "Init_database() ERROR: \tUnable to create"
                        " app dir %s!\n", record.field[0].data);
                retval = 0;
                return(0);
              }

            x = add_env_file(record.field[0].data, record.field[9].data,
                             db_dir);
            if (x == 0)
              {
                fprintf(logp, "Init_database() ERROR: \tUnable to create"
                        " app env file %s!\n", record.field[9].data);
                retval = 0;
                return(0);
              }

            x = add_PSP_file(record.field[0].data, record.field[10].data,
                             db_dir);
            if (x == 0)
              {
                fprintf(logp, "Init_database() ERROR: \tUnable to create"
                        " app PSP file %s!\n", record.field[9].data);
                retval = 0;
                return(0);
              }

            signum = atoi(record.field[12].data);
                
            if (signum == SIGCHLD || signum == SIGABRT ||
                signum == SIGSEGV || signum == SIGIO )
              {
                fprintf(logp, "Init_database() ERROR: \tIllegal App Signal"
                        " for %s! Setting to default (SIGUSR1)!\n",
                        record.field[0].data);
                signum = SIGUSR1;
              }

            x = add_SIG_file(record.field[0].data, signum, db_dir);
            if (x == 0)
              {
                fprintf(logp, "Init_database() ERROR: \tUnable to create"
                        " app SIG file for %s!\n", record.field[0].data);
                retval = 0;
                return(0);
              }
/*
 * Add batch user (if necessary).
 */
            if (!isBlank(record.field[11].data))
              {
                x = checkLoginName(record.field[11].data);
                if (x == 0)
                  {
                    fprintf(logp, "Init_database() ERROR: \tBad batch user"
                            " value for progroup %s!\n",
                            record.field[0].data);
                    retval = 0;
                    return(0);
                  }
                fprintf(logp, "\nInit_database(): Adding batchuser for"
                        " program group <%s>...\n", record.field[0].data);


		/* set batch password */
		memset(strEncryptedPassword,NULL,
			(MAX_PASSWORD_LEN * 2) + 1);
		retval=GetEncryptedBatchPassword(record.field[11].data,
			strEncryptedPassword);
		if (retval == 0) {
                    	fprintf(logp, "Init_database() ERROR: \tBad batch user"
                            " password encryption,setting to default!\n");
			sprintf(strEncryptedPassword,"*");
		}

                /* if this is the host of the app, add the batch user */
                if (!strcmp(strLocalHost,record.field[1].data) ||
                    nHostType == NTCSS_MASTER)
                  {
                    fprintf(wp,
                            "%d\n%d\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n%d\n%s\n\n",
                            SQL_MOD_BATCH_NTCSS_USERS,
                            0,
                            record.field[11].data,
                            "Batch User",
                            strEncryptedPassword,
                            "000-00-0000", 
                            "N/A",
                            0,
                            "N/A",
                            0, record.field[1].data);

                    fprintf(wp, "%d\n%s\n%s\n%s\n%d\n\n",
                            SQL_MOD_BATCH_APPUSERS,
                            record.field[0].data,
                            record.field[11].data,
                            "Batch User",
                            1024);
                  }

                if (add_unix_batch_users) {  
		    if (nHostType == NTCSS_MASTER)
                    {
                       sprintf(cmd, "%s ADDBATCHUSER %s %s %s \"%s\" %s",
                            NTCSS_CHANGE_USER_PROGRAM,
                            record.field[11].data, "passwd", homedir,
                            "Batch User", record.field[2].data);
                       j = system(cmd);
                       if (j < 0)
                         j = errno;
                       else
                         j = WEXITSTATUS(j);
                       if (j <= 0)
                         fprintf(logp, "Init_database() WARNING: \tFailed to"
                              " give batch user %s a UNIX account!\n",
                              record.field[11].data);
                     } else if (nHostType == NTCSS_AUTH_SERVER) {

                       sprintf(cmd,"/h/NTCSSS/applications/data/users_conf/Create_Auth_Account ADDUSER %s/%s %s 2>&1 > /dev/null",  homedir, record.field[11].data, record.field[11].data);
                       j = system(cmd);
                       if (j < 0)
                         j = errno;
                       else
                         j = WEXITSTATUS(j);
                       if (j <= 0)
                         fprintf(logp, "Init_database() WARNING: \tFailed to"
                              " give batch user %s a UNIX home directory!\n",
                              record.field[11].data);
                     }
                  }
              }      /* end of if (!blank[11]) */

            convToSql(&record, 9);
            fprintf(wp, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
                    SQL_MOD_PROGROUPS,
                    record.field[0].data,
                    record.field[1].data,
                    record.field[3].data,
                    record.field[4].data,
                    record.field[5].data,
                    record.field[6].data,
                    record.field[7].data,
                    record.field[8].data,
                    record.field[2].data);

/*
 * Create new access roles file.
 */

            /* Read in access roles. */
            fprintf(logp, "\nInit_database(): Creating access roles for"
                    " program group <%s>...\n", record.field[0].data);
            j = 1;
            sprintf(Sbuf, "%s_%s", tptr, "ROLES");

/* Melody test stuff */
            if (GetProfileSection(Sbuf,lpszRolesSections, MAX_INI_ENTRY * 100,
                                  buf ) < 0)
            {
              fprintf(logp, "init_database(): GetProfileSection failed, "
                      "could not get access roles section!\n");
               return(0);
            }

            pCh = lpszRolesSections;

            /* If no host entries returned do not go through loop */
            if (*pCh == '\0') 
              {
                nContinue = 0;
              } 
            else 
              {
                nContinue = 1;
              }
                                             
            /* Loop through each app role and get the value for that role
               and then get the lockoverride value for that role.
               Witheach role add it to the access_roles database file. */
            while (nContinue) 
              {
                /* Each entry is in the form NTCSS_ROLE1=App Administrator,
                   so we parse out each section header based on the =.
                   We use the NTCSS_ROLE1 to get the value App
                   Administrator and from the section [NTCSS_ROLE_1]
                   LOCK_OVERRIDE=0.*/
                parseWord(0, pCh, lpszAccessRoleSectionHeaders, "=");

                GetProfileString(Sbuf, lpszAccessRoleSectionHeaders, "!",
                                 lpszAccRole, MAX_ACCESS_ROLE_LEN, buf);
                if(strchr(lpszAccRole,'!'))
                  {
                    fprintf(logp, "init_database(): GetProfileString failed,"
                            " could not get access role!\n");
                    return(0);
                  }

                GetProfileString(lpszAccessRoleSectionHeaders, "LOCK_OVERRIDE",
                                 "!", lpszLockOverrideValue,
                                 MAX_ACCESS_ROLE_LEN, buf);
                if(strchr(lpszLockOverrideValue,'!'))
                  {
                    strcpy(lpszLockOverrideValue, "0");
                    WriteProfileString(lpszAccessRoleSectionHeaders,
                                       "LOCK_OVERRIDE", lpszLockOverrideValue,
                                       buf);
                  }
                fprintf(ap, "%s\n%s\n%d\n%s\n\n", record.field[0].data,
                        lpszAccRole, j++, lpszLockOverrideValue);

                pCh += strlen(pCh) + 1;
                /* Stop on consecutive null condition */
                if (*pCh == '\0') 
                  {
                    nContinue = 0;
                  }
              } /* END while */


/*
 *Read in Programs and check their stats...
 */     
            fprintf(logp, "\nInit_database(): Setting up programs for"
                    " program group <%s>...\n", record.field[0].data);
            memset(tbuf2, NTCSS_NULLCH, MAX_INI_ENTRY * 100);
            sprintf(Sbuf, "%s_%s", tptr, "PROG");
            GetSectionValues(Sbuf, tbuf2, MAX_INI_ENTRY * 100, buf);
            tptr2 = tbuf2;
            if(strlen(tptr2) > 0)
              do
                {   
                  if(getIniRecord(&record, tptr2, tbuf3, buf))
                    {  
                      fprintf(logp, "Init_database(): Verifying data for "
                              "program <%s>...\n", record.field[0].data);
                      x = checkProgramTitle(record.field[0].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad program"
                                  " title key value <%s> in %s!\n",
                                  record.field[0].data, buf);
                          retval = 0;
                          return(0);
                        }
                      
                      x = checkProgramFile(record.field[1].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad program"
                                  " file key value for program <%s> in %s"
                                  "!\n", record.field[0].data, buf);
                          retval = 0;
                          return(0);
                        }

/* For some unknown reason, x (below) is not getting the correct return
   value from the call when errors occur - eg: no file spec'd */
                      x = checkIconFile(record.field[2].data, err_msg);
                      if ((x == 0) || (x == 2)) /* 2 is non-fatal */
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad icon"
                                  " file key value for program <%s> in %s!\n",
                                  record.field[0].data, buf);
                          if (x == 0)  /* Bad error */
                            {
                              retval = 0;
                              continue;
                            }
                        }

                      x = checkIconIndex(record.field[3].data, err_msg);
                      if (x == 0)
                         {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad icon"
                                  " index key value for program <%s> in %s!\n",
                                  record.field[0].data, buf);
                          retval = 0;
                          continue;
                        }

                      x = checkCmdLineArgs(record.field[4].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad command"
                                  " line arguments key value for program <%s>"
                                  " in %s!\n", record.field[0].data, buf);
                          retval = 0;
                          continue;
                        }
                      
                      record.field[5].data[0] = getSecurityCode(record.field[5].data);
                      record.field[5].data[1] = '\0';
                    
                      x = checkClassification(record.field[5].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp,"Init_database(): ERROR! \tBad"
                                  " classification key value (%s) for"
                                  " program <%s> in %s!\n",
                                  record.field[5].data, record.field[0].data,
                                  buf);
                          retval = 0;
                          return(0);
                        }
                      strcpy(record.field[6].data,
                             makeAccessString(record.field[6].data, tptr,
                                              strNewAccessString, buf));
                      k = checkProgramAccess(j, record.field[6].data, err_msg);
                      if (k == 0)
                        fprintf(logp, "Init_database(): \tProgram %s does not"
                                "have any access roles assigned!\n",
                                record.field[0].data);
                      if (k < 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad "
                                  "program access key value <%s> for"
                                  " program <%s> in %s!\n",
                                  record.field[6].data, record.field[0].data,
                                  buf);
                          retval = 0;
                          return(0);
                        }

                      sprintf(record.field[7].data, "%c", 
                              getJobCode(record.field[7].data));
                      x = checkProgramType(record.field[7].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \tBad program"
                                  " type key value for program <%s> in %s!\n",
                                  record.field[0].data, buf);
                          retval = 0;
                          return(0);
                        }

                      x = checkWorkingDir(record.field[8].data, err_msg);
                      if (x == 0)
                        {
                          fprintf(logp, err_msg);
                          fprintf(logp, "Init_database(): ERROR! \t Bad"
                                  " working directory key value for"
                                  " program <%s> in %s!\n",
                                  record.field[0].data, buf);
                        retval = 0;
                        return(0);
                        }
                      convToSql(&record, 9);
                      fprintf(wp,
                              "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n%s\n\n",
                              SQL_MOD_PROGRAMS,
                              record.field[0].data,
                              record.field[1].data,
                              record.field[2].data,
                              record.field[3].data,
                              record.field[4].data,
                              group,
                              record.field[5].data,
                              k,                       /* access role # */
                              record.field[7].data,
                              record.field[8].data);
                    } /*if */

                  tptr2 = strchr(tptr2, '\0') + 1;
                }
              while(*tptr2 != NTCSS_NULLCH);

/*
 * Check predefined job data.           
 */

            fprintf(logp, "\nInit_database(): Checking Predefined Jobs"
                    "...\n");
            sprintf(Sbuf, "%s_%s", tptr, "PDJ");
            memset(tbuf2, NTCSS_NULLCH, MAX_INI_ENTRY * 100);

            GetSectionValues(Sbuf, tbuf2, MAX_INI_ENTRY * 100, buf);
            tptr2 = tbuf2;
            if(strlen(tptr2) > 0)
              do
                {
                  if(!getIniRecord(&record, tptr2, tbuf3, buf))
                    {
                      fprintf(logp, "Init_database() ERROR: Could Not Parse"
                              "PDJ section in %s!\n", buf);
                      retval = 0;
                      return(0);
                    }

                  fprintf(logp, "Init_database(): Checking PDJ <%s>..\n",
                          record.field[0].data);
                  x = checkPredefinedJobTitle(record.field[0].data,
                                              err_msg);
                  if (x == 0)
                    {
                      fprintf(logp, err_msg);
                      fprintf(logp, "Init_database() ERROR: \tBad "
                              "predefined job title value for PDJ %s!\n",
                              record.field[0].data);
                      retval = 0;
                      return(0);
                    }
                  x = checkCommandLine(record.field[2].data, err_msg);
                  if (x == 0)
                    {
                      fprintf(logp, err_msg);
                      fprintf(logp, "Init_database() ERROR: \tBad "
                              "predefined job command line value"
                              " for PDJ %s!\n", record.field[0].data);
                      retval = 0;
                      return(0);
                    }

                  strcpy(record.field[3].data,    /* access role field */
                         makeAccessString(record.field[3].data, tptr,
                                          strNewAccessString, buf));
                  k = checkProgramAccess(j, record.field[3].data, err_msg);
                  if (k == 0)
                    fprintf(logp, "Init_database(): \tPDJ %s does not"
                            "have any access roles assigned!\n",
                            record.field[0].data);
 
                  if (k < 0)
                    {
                      fprintf(logp, err_msg);
                      fprintf(logp, "Init_database() ERROR: \tBad "
                              "predefined job program access value"
                              " for PDJ %s!\n", record.field[0].data);
                      retval = 0;
                      return(0);
                    }
            
                  record.field[4].data[0] = getJobCode(record.field[4].data);
                  record.field[4].data[1] = '\0';

                  x = checkPredefinedJobClass(record.field[4].data,
                                              err_msg);
                  if (x == 0)
                    {
                      fprintf(logp, err_msg);
                      fprintf(logp, "Init_database() ERROR: \tBad "
                              "predefined job class value for PDJ %s!\n",
                              record.field[0].data);
                      retval = 0;
                      return(0);
                    }
                  sptr = record.field[5].data;

                  while (strlen(sptr) >= MAX_SCHED_STR_LEN)
                    {
                      ncpy(sched, sptr, MAX_SCHED_STR_LEN);
                      x = checkScheduleString(sched);
                      if (x == 0)
                        {
                          fprintf(logp, "Init_database() ERROR: \tBad"
                                 " predefined job schedule string value"
                                 " for PDJ %s!\n",
                                 record.field[0].data);
                          retval = 0;
                          return(0);
                        }
                      sptr += MAX_SCHED_STR_LEN;
                    }

                  convToSql(&record, 6);
                  fprintf(wp, "%d\n%s\n%s\n%s\n%d\n%s\n%s\n\n",
                          SQL_MOD_PREDEFINED_JOB,
                          record.field[0].data,    /* job's title */
                          group,                   /* progroup title */
                          record.field[2].data,    /* command line */
                          k,                       /* access role */
                          record.field[4].data,    /* PDJ type bitmask */
                          record.field[5].data);   /* scehdule */
                  
                  tptr2 = strchr(tptr2, '\0') + 1;
                }
              while(*tptr2!= NTCSS_NULLCH); /*PDJ dowhile*/

/*
 * Write group name to application id file.
 */
            fprintf(aip, "%s\n", group);
            fprintf(cap, "%s\n%s\n0\n0\n\n", group, host);

            tptr = strchr(tptr, '\0') + 1;
          }
        while(*tptr != NTCSS_NULLCH); /*while for progroups*/

      fprintf(logp, "\nInit_database(): Finished checking all progroups.\n\n");

      sprintf(buf, "%s/%s", db_dir, getAccessRolesTableName());
      closeAndEncrypt(ap, buf);
      (void) chmod(buf, S_IRUSR | S_IWUSR);

      sprintf(buf, "%s/%s", db_dir, getCurrentAppsTableName());
      closeAndEncrypt(cap,buf);
      (void) chmod(buf, S_IRUSR | S_IWUSR);

      sprintf(buf, "%s/%s", db_dir, "APP_ID");
      closeAndEncrypt(aip, buf);
      (void) chmod(buf, S_IRUSR | S_IWUSR);
      break;


    case HOST_INFO:

/*
 * Create new host id file.
 * In /database just list of names
 */
      fprintf(logp, "Init_database(): Creating HOST_ID file....\n");
      sprintf(buf, "%s/%s", db_dir, "HOST_ID");
      aip = fopen(buf, "w");
      if (aip == NULL)
        {
          fclose(wp);
          fprintf(logp, "Init_database() ERROR: Cannot open file %s!\n", buf);
          return(0);
        }

      if (argfile == NULL)
        sprintf(buf, "%s", NTCSS_PROGRP_ID_FILE);
      else
        strcpy(buf, argfile);


     
      memset(tbuf, NTCSS_NULLCH, MAX_INI_ENTRY * 100);
      GetSectionValues("HOSTS", tbuf, MAX_INI_ENTRY * 100, buf);

      hostItems = (HostItem **) malloc(sizeof(char *)*MAX_NUM_HOSTS);
      nCounter = 0;
      tptr = tbuf;
      while (strlen(tptr))
        {
          GetProfileString(tptr, "NAME", "NONE", host, MAX_INI_ENTRY, buf);
          GetProfileString(tptr, "IPADDR", "NONE", ipaddr, MAX_INI_ENTRY, buf);
          GetProfileString(tptr, "TYPE", "NONE", hosttype, MAX_INI_ENTRY, buf);
          GetProfileString(tptr, "REPL", "NO", repl, MAX_INI_ENTRY, buf);

          if (!strcmp(host, "NONE"))
            {
              syslog(LOG_WARNING,"Init_Database() Error: Invalid Hostname "
                     "Address for host <%s>", host);
              retval = 0;
              tptr += strlen(tptr) + 1;   /* Point to next host. */
              continue;
            }

          if (!strcmp(ipaddr, "NONE"))
            {
              syslog(LOG_WARNING,"Init_Database() Error: Invalid IP "
                     "Address for host <%s>", host);
              retval = 0;
              tptr += strlen(tptr) + 1;   /* Point to next host. */
              continue;
            }

          if (!strcmp(hosttype, "NONE"))
            {
              syslog(LOG_WARNING,"Init_Database() Error: Invalid host "
                     "type for host <%s>",host);
              retval = 0;
              tptr += strlen(tptr) + 1;   /* Point to next host. */
              continue;
            }

          x = checkHostName(host);
          if (x == 0)
            {
              fprintf(logp, "Init_database() ERROR: Bad hostname value in"
                      " file %s!\n", buf);
              retval = 0;
     /* Added this line in because of suspected logic error DB*/
              tptr += strlen(tptr) + 1;   /* Point to next host. */
              continue;
            }

          x = checkIpAddress(ipaddr);
          if (x == 0)
            {
              fprintf(logp, "Init_database() ERROR: Bad IP address value in"
                      " file %s.\n", buf);
              retval = 0;
     /* Added this line in because of suspected logic error DB*/
              tptr += strlen(tptr) + 1;   /* Point to next host. */
              continue;
            }

          fprintf(wp,
                  "%d\n%s\n%s\n%d\n%d\n\n",
                  SQL_MOD_HOSTS,
                  host,
                  ipaddr, ConvertHostType(hosttype),ConvertRepl(repl));
          fprintf(aip, "%s\n", host);

          tptr += strlen(tptr) + 1;
          hostItems[nCounter] = (HostItem *) malloc(sizeof(HostItem));
          hostItems[nCounter]->marker        = 1;
          hostItems[nCounter]->hostname      = allocAndCopyString(host);
          hostItems[nCounter]->ip_address    = allocAndCopyString(ipaddr);
          hostItems[nCounter]->num_processes = 30; /* Hard coded limit */
          hostItems[nCounter]->type          = ConvertHostType(hosttype);
          hostItems[nCounter]->repl          = ConvertRepl(repl);
          nCounter++;
        }  /* End of host processing loop. */

/*
 * Write hostname to host id file.
 */
      
      sprintf(buf, "%s/%s", db_dir, "HOST_ID");
      closeAndEncrypt(aip, buf);
      (void) chmod(buf, S_IRUSR | S_IWUSR);

      sprintf(strHostFileName, "%s/hosts", NTCSS_DB_DIR);
      hostFp = fopen(strHostFileName, "w");
      if (hostFp == NULL) {
        fprintf(logp, "Init_database() ERROR: open hosts database file failed.\n");
        return(0);
      } /* END if(hostFp) */

      writeHostData(hostItems, nCounter, hostFp);
      freeHostData(hostItems, nCounter);
      closeAndEncrypt(hostFp, strHostFileName);
      break;

    case PRTCLASS_INFO:
      sprintf(buf, "%s/classes", dir);
      rp = fopen(buf, "r");
       if (rp == NULL)
         {
           fclose(wp);
           fprintf(logp, "Init_database() ERROR: Cannot open file %s!\n", buf);
           return(0);
         }
/*
 * Check printer class data.
 */
       while (getRecord(&record, rp, 3))
         {
           fprintf(logp, "Init_database(): Checking Print Class <%s>...\n",
                   record.field[0].data);

           x = checkPrtClassTitle(record.field[0].data);
           if (x == 0)
             {
               fprintf(logp, "Init_database() ERROR: \tBad printer class title"
                       " value for class %s!\n", record.field[0].data);
               retval = 0;
               continue;
             }

           x = checkPrtDeviceName(record.field[1].data);
           if (x == 0)
             {
               fprintf(logp, "ERROR: \tBad printer device name value for class"
                       " %s!\n", record.field[0].data);
               retval = 0;
               continue;
             }
           x = checkPrtDriverName(record.field[2].data);
           if (x == 0)
             {
               fprintf(logp, "Init_database() ERROR: \tBad printer driver name"
                       " value for class %s!\n", record.field[0].data);
               retval = 0;
               continue;
             }
           fprintf(wp,
                   "%d\n%s\n%s\n%s\n\n",
                   SQL_MOD_PRT_CLASS,
                   record.field[0].data,
                   record.field[1].data,
                   record.field[2].data);
         }
       fclose(rp);
       break;
  }  				/* switch */
  fflush(logp); 
  fclose(wp);
  return(retval);
}


/*****************************************************************************/

int  convert_database_files(db_dir, new_version)

     char  *db_dir;
     char  *new_version;

{
  int  x;                                                 /* general purpose */
  char  filename[MAX_FILENAME_LEN];
  char  temp_filename[MAX_FILENAME_LEN];
  char  cmd[MAX_CMD_LEN];
  char  temp_dir[MAX_FILENAME_LEN];
  char  old_version[MAX_INSTALL_ITEM_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];
  FILE  *tmp;
  char  file[MAX_FILENAME_LEN];
  struct stat st;
  int oldNTCSSflag;
  char ans[100];
  int no_ensure;
  int nMyHostType;
  char strLocalHost[MAX_HOST_NAME_LEN+1];

/*
 * Determine old version number.
 */
  memset(strLocalHost,NTCSS_NULLCH,MAX_HOST_NAME_LEN+1);
  gethostname(strLocalHost,MAX_HOST_NAME_LEN);

  nMyHostType=getHostTypeFromInitData(strLocalHost);

  no_ensure=0;
  oldNTCSSflag=0;
  if (!stat("/usr/local/NTCSSII",&st)) {
        
	/* if auth server, dont ask questions, just doit */
        if (nMyHostType == NTCSS_AUTH_SERVER) {
		oldNTCSSflag=1;
  		sprintf(filename, "/usr/local/NTCSSII/database/VERSION");
	} else {
		while (1) {
			memset(ans,'\0',100);
			printf("\nPrevious Version of NTCSS detected in /usr/local/NTCSSII!!\n");
			printf("Use this data for conversion (Y/N)?");
			gets(ans);
			if (!strcmp(ans,"y") || !strcmp(ans,"Y")) {
  				sprintf(filename, "/usr/local/NTCSSII/database/VERSION");
				printf("\n***** NOTE: ***** \n");
				printf("   The directory /usr/local/NTCSSII will be renamed to /usr/local/NTCSSII.leg\n");
				printf("***** NOTE: ***** \n");
				printf("\nContinuing Prime....\n");
  				oldNTCSSflag=1;
				break;
			} else if (!strcmp(ans,"n") || !strcmp(ans,"N")) {
  				sprintf(filename, "%s/VERSION", db_dir);
				break;
			} else
				continue;
		}
	}
  } else {
  	sprintf(filename, "%s/VERSION", db_dir);
  }
	
  fp = decryptAndOpen(filename, decryptfile, "r");
  if (fp != NULL)
    {
      if (fgets(old_version, MAX_INSTALL_ITEM_LEN, fp) == NULL)
        {
          fprintf(logp, "ERROR: Cannot read the old version file %s!\n",
                  filename);
          fclose(fp);
          remove(decryptfile);
          return(0);
        }
      fclose(fp);
      remove(decryptfile);
      stripNewline(old_version);
      trim(old_version);
    }
  else        /* try to read the alternate version file for info */
    { 
      sprintf(temp_filename, "%s/version", db_dir);
      fp = fopen(temp_filename, "r");
      if (fp != NULL)
        {
          if (fgets(old_version, MAX_INSTALL_ITEM_LEN, fp) == NULL)
            {
              fprintf(logp, "ERROR: Cannot read old version file %s!\n",
                      temp_filename);
              fclose(fp);
              return(0);
            }
          fclose(fp);
          stripNewline(old_version);
          trim(old_version);
          if (rename(temp_filename, filename) != 0)
            {
              fprintf(logp, "ERROR: Cannot create old version file!\n");
              return(0);
            }
        }
      else  /* Couldn't open either of the two version files, so create one. */
        {
          sprintf(cmd, "%s 0.1.1 > %s", ECHO_PROG, filename);
          x = system(cmd);
          if (x != 0)
            {
              fprintf(logp, "ERROR: Cannot create an 'old' version file %s!\n",
                      filename);
              return(0);
            }
          strcpy(old_version, "0.1.1"); /* start with the first version */
        }
    }


  sprintf(cmd, "%s 600 %s", CHMOD_PROG, filename);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: Cannot change permissions on version file %s!\n",
            filename);

/*
 * Convert database to latest version.
 */

  /* Remove leftover database.tmp directory from previous attempts if exists.*/
  sprintf(temp_dir, "%s.tmp", db_dir);
  sprintf(cmd, "%s %s", RM_PROG, temp_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: Cannot remove previous temporary database"
              " directory %s.\n", temp_dir);
      return(0);
    }

  /* Make a copy of the current database directory with a .tmp extension. */
  /* This .tmp directory is what the conversion functions work on. */
  sprintf(cmd, "%s %s %s", CP_DIR_PROG, db_dir, temp_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: Cannot copy old database directory %s to %s!\n",
              db_dir, temp_dir);
      return(0);
    }

  /* Convert the old (.tmp) database one version at a time until it becomes */
  /* the current version. */
  while (1)
    {
      if (strcmp(old_version, new_version) == 0)
        break;
      fprintf(logp, "Converting the databases from version %s to %s..\n",
              old_version, new_version);

      x = convert_versions(old_version, temp_dir);
      if (x == 0)   /* file = database.tmp */
        {
          /* In case of unsuccessful conversion(s) */
          sprintf(cmd, "%s %s", RM_PROG, temp_dir); /* remove the .tmp dir */
          x = system(cmd);
          if (x != 0)
            fprintf(logp, "ERROR: Cannot remove temporary database directory"
                    "%s!\n", temp_dir);
          fprintf(logp, "ERROR: Failed to convert database from %s to %s!\n",
                  old_version, new_version);
          return(0);
        }
       if (x == 2)   /* don't run ensure for prime */
          no_ensure=1;
    }

/*
 * Create new version file.
 */

  /* remove the version file in the db.tmp dir first */
  fprintf(logp, "Setting the new version file...\n");
  sprintf(cmd, "%s %s/VERSION", RM_PROG, temp_dir);
  x = system(cmd);
  if (x != 0)
    {
      /* If we can't remove the version file, trash the whole db.tmp dir. */
      fprintf(logp, "ERROR: Cannot remove old version file %s!\n", temp_dir);
      sprintf(cmd, "%s %s", RM_PROG, temp_dir);
      x = system(cmd);
      if (x != 0)
        fprintf(logp, "ERROR: Cannot remove temporary database"
                " directory %s!\n", temp_dir);
      return(0);
    }

  /* set the new version # for the db.tmp dir */
  sprintf(cmd, "%s %s > %s/VERSION", ECHO_PROG, new_version, temp_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: Cannot create new version file!\n");
      sprintf(cmd, "%s %s", RM_PROG, temp_dir);
      x = system(cmd);
      if (x != 0)
        fprintf(logp, "ERROR: Cannot remove temporary database"
                " directory %s!\n", temp_dir);
      return(0);
    }
  sprintf(cmd, "%s 600 %s/VERSION", CHMOD_PROG, temp_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: Cannot change permissions on version file %s!\n",
            temp_dir);

  /* encrpypt the VERSION file */
  sprintf(file, "%s/VERSION", temp_dir); 
  tmp = fopen(file, "r");
  closeAndEncrypt(tmp, file);

/*
 * Replace the original DB directory with the database.tmp directory.
 */
  fprintf(logp, "Replacing the old database with the new one..\n");
  sprintf(cmd, "%s %s", RM_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: Cannot remove database directory %s!\n",
              db_dir);
      return(0);
    }
  if (rename(temp_dir, db_dir) != 0)
    {
      fprintf(logp, "ERROR: Cannot rename the temporary database directory"
              " %s!\n", db_dir);
      return(0);
    }
  sprintf(cmd, "%s 755 %s", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: Cannot change permissions on upgraded"
            " database directory %s!\n", db_dir);

  if (oldNTCSSflag) 
	rename("/usr/local/NTCSSII","/usr/local/NTCSSII.leg");
  if (no_ensure == 0)
  	return(1);
  else 
        return(2);
}


/*****************************************************************************/

int  are_there_newly_registered_apps(tmpdir)

     char  *tmpdir;

{
  DIR *dp;
  int  x;
  char  buf[MAX_FILENAME_LEN];
  struct stat  sbuf;

#ifdef SOLARIS
  struct dirent  *direc;
#else
  struct direct  *direc;
#endif

  dp = opendir(tmpdir);
  if (dp == NULL)
    {
      fprintf(logp, "ERROR: Cannot open directory %s!\n", tmpdir);
      return(-1);
    }
  while ((direc = readdir(dp)) != NULL)
    {
      if (direc->d_ino == 0)
        continue;

      x = strncmp(direc->d_name, "regapp.", strlen("regapp."));
      if (x != 0)
        continue;

      sprintf(buf, "%s/%s", tmpdir, direc->d_name);
      x = stat(buf, &sbuf);
      if (x == -1)
        {
          fprintf(logp, "ERROR: Cannot get status of file %s!\n", buf);
          closedir(dp);
          return(-1);
        }

      if (((sbuf.st_mode & S_IFMT) == S_IFREG) &&
          (access(buf, R_OK) != -1))
        {
          closedir(dp);
          return(1);
        }
    }
  closedir(dp);

  return(0);
}


/*****************************************************************************/

int process_newly_registered_apps(tmpdir, initdir, hostdir, appdir)

     char  *tmpdir;
     char  *initdir;
     const char  *hostdir;
     const char  *appdir;
{

#ifdef SOLARIS
  struct dirent  *direc;
#else
  struct direct  *direc;
#endif

  char  app[MAX_INSTALL_ITEM_LEN];
  char  buf[MAX_FILENAME_LEN];
  char  cmd[MAX_CMD_LEN];
  char  item[MAX_INSTALL_ITEM_LEN];
  char  line[MAX_INSTALL_ITEM_LEN];
  char  Section[MAX_INI_ENTRY * 100];
  char  Section2[MAX_INI_ENTRY * 100];
  char  *p,*q;
  DIR  *dp;
  struct stat  sbuf;
  struct hostent  *hp;
  struct sockaddr_in  addr;

  /* done only to get rid of compiler warning about unused parameters */
  hostdir=hostdir;
  appdir=appdir;
      
    
  dp = opendir(tmpdir);
  if (dp == NULL)
    {
      fprintf(logp, "ERROR: Cannot open directory %s!\n", tmpdir);
      return(0);
    }

  while ((direc = readdir(dp)) != NULL)
    {
      if (direc->d_ino == 0)
        continue;
      if (strncmp(direc->d_name, "regapp.", strlen("regapp.")) != 0)
        continue;
      sprintf(buf, "%s/%s", tmpdir, direc->d_name);
      if (stat(buf, &sbuf) == -1)
        {
          fprintf(logp, "ERROR: Cannot get status of file %s.\n", buf);
          continue;
        }
        
      GetSectionValues("APPS",app,MAX_INSTALL_ITEM_LEN,buf);
      GetProfileString(app,"SERVER","!",line,MAX_INSTALL_ITEM_LEN,buf);
      WriteProfileString("APPS",app,app,initdir);

      hp = (struct hostent *) gethostbyname(line);
      if (hp == NULL)
        {
          fprintf(logp, "ERROR: Cannot determine IP address of host %s!\n",
                  item);
          continue;
        }
#ifndef VSUN
      memmove((caddr_t)&addr.sin_addr, hp->h_addr_list[0], hp->h_length);
#endif
      strcpy(cmd, inet_ntoa(addr.sin_addr));

/*
 * Add hostname and IP address  to "init data" hosts file.
 */
       
      WriteProfileString("HOSTS", line, cmd, initdir);
               
      GetProfileSection(app, Section, MAX_INI_ENTRY * 100, buf);
      WriteProfileSection(app, Section, initdir);

      sprintf(line, "%s_PDJ", app);
      CopyProfileSection(line, initdir, buf);
      
      sprintf(line, "%s_PROG", app);
      CopyProfileSection(line, initdir, buf);

      sprintf(line, "%s_ROLES", app);
      CopyProfileSection(line, initdir, buf);

      memset(Section, NTCSS_NULLCH, MAX_INI_ENTRY * 100);               
      GetProfileSection(line, Section, MAX_INI_ENTRY * 100, buf);
      GetSectionValues(line, Section2, MAX_INI_ENTRY * 100, buf);
      p = Section;
      q = Section2;
      while(strlen(p))
        {
          memset(line, NTCSS_NULLCH, MAX_INSTALL_ITEM_LEN);
          memset(item, NTCSS_NULLCH, MAX_INSTALL_ITEM_LEN);
          strcpy(line, p);
          *(strchr(line, '=')) = '\0';
          sprintf(item, "TITLE=%s", q);    
          WriteProfileSection(line, item, initdir);
          q += strlen(q) + 1;
          p += strlen(p) + 1;
        }
    }
  closedir(dp);

  sprintf(buf, "rm -f %s/regapp.*", tmpdir);
  system(buf);

  return(1);
}


/*****************************************************************************/
/* Basically calls functions that convert a database from it's (current) "old" version to the next highest version. */

static int  convert_versions(old_version, db_dir)

     char  *old_version;
     char  *db_dir;  /* probably the copied database.tmp directory */

{
  int  x;              /* for function return values */

  if (strcmp(old_version, "0.1.1") == 0)
    {
      x = DBconvert0_1_1to0_2_0(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "0.2.0");       /* prepare it for the next step.. */
      return(1);
    }

  if (strcmp(old_version, "0.2.0") == 0)
    {
      x = DBconvert0_2_0to0_3_0(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "0.3.0");
      return(1);
    }

  if (strcmp(old_version, "0.3.0") == 0)
    {
      x = DBconvert0_3_0to0_4_0(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "0.4.0");
      return(1);
    }

  if (strcmp(old_version, "0.4.0") == 0)
    {
      x = DBconvert0_4_0to0_5_0(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "0.5.0");
      return(1);
    }

  if (strcmp(old_version, "0.5.0") == 0)
    {
      x = DBconvert0_5_0to1_0_0(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "1.0.0");
      return(1);
    }

  if (strcmp(old_version, "1.0.0") == 0)
    {
      x = DBconvert1_0_0to1_0_1(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "1.0.1");
      return(1);
    }
  
  if (strcmp(old_version, "1.0.1") == 0)
    {
      x = DBconvert1_0_1to1_0_2(db_dir);
      if (x < 0)
        return(0);                    /* error occured */
      strcpy(old_version, "1.0.2");
      return(1);
    }

 if (strcmp(old_version, "1.0.2") == 0)
   {
     /** NO CONVERSION FROM 1.0.2 TO 1.0.3 **/
     strcpy(old_version, "1.0.3");
     return(1);
    }
 if (strcmp(old_version, "1.0.3") == 0)
   {
     /** NO CONVERSION FROM 1.0.3 TO 1.0.4 **/
     strcpy(old_version, "1.0.4");
     return(1);
   } 
 if (strcmp(old_version, "1.0.4") == 0)
   {
     /** NO CONVERSION FROM 1.0.4 TO 1.0.5 **/
       strcpy(old_version, "1.0.5");
       return(1);
    }
 if (strcmp(old_version, "1.0.5") == 0)
   {
     /** NO CONVERSION FROM 1.0.5 TO 1.0.6 **/
     strcpy(old_version, "1.0.6");
     return(1);
    }
 if (strcmp(old_version, "1.0.6") == 0)
   {
     /** NO CONVERSION FROM 1.0.6 TO 1.0.7 **/
     strcpy(old_version, "1.0.7");
     return(1);
   }

 if (strcmp(old_version, "1.0.7") == 0)
   {
     /** CONVERSION FROM 1.0.7 TO 3.0.0 **/
     x = DBconvert1_0_7to3_0_0(db_dir);
     if (x != 0 && x != 2)
       return(0);                    /* error occured */
     strcpy(old_version, "3.0.0");
     if (x == 2)
        return(2);
     return(1);
   }

 if (strcmp(old_version, "3.0.0") == 0) 
   {
     /** CONVERSION FROM 3.0.0 to 3.0.1 **/
     x = DBconvert3_0_0to3_0_1(db_dir);
     if (x != 0)
       return(0);                    /* error occured */
     strcpy(old_version, "3.0.1");
     return(1);
   } 
 if (strcmp(old_version, "3.0.1") == 0)
   {
     /** NO CONVERSION FROM 3.0.1 TO 3.0.2 **/
     strcpy(old_version, "3.0.2");
     return(1);
    }

 if (strcmp(old_version, "3.0.2") == 0)
   {
     /** NO CONVERSION FROM 3.0.2 TO 3.0.3 **/
     strcpy(old_version, "3.0.3");
     return(1);
   }

  if (strcmp(old_version, "3.0.3") == 0) 
   {
     /** CONVERSION FROM 3.0.3 to 3.0.4 **/
     /*
     x = DBconvert3_0_3to3_0_4(db_dir);
     if (x != 0)
       return(0);                   
     */
     strcpy(old_version, "3.0.4");
     return(1);
   }

  if (strcmp(old_version, "3.0.4") == 0) 
   {
     /** CONVERSION FROM 3.0.4 to 3.0.5 **/
     strcpy(old_version, "3.0.5");
     return(1);
   }

     /** UNKNOWN CONVERSION **/
 fprintf(logp, "ERROR: Unrecognized database version!"
         "Cannot convert from %s!\n", old_version);
 return(0);                            /* error occured */

}


/*****************************************************************************/

static int  add_env_dir(appGroup, dbDir)

     char  *appGroup;
     char  *dbDir;

{
  int  retCode;
  char  dn[MAX_FILENAME_LEN];
  char  fn[MAX_FILENAME_LEN];
  char *slashptr;
  mode_t  envMode;

  strcpy(dn, dbDir);
  if ((slashptr = strrchr(dn, (int)'/')))
    *slashptr = NTCSS_NULLCH;

  sprintf(fn, "%s/env/%s", dn, appGroup);
  envMode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
            S_IXOTH;

  retCode = mkdir(fn, envMode);

  if (retCode && errno != EEXIST)
    return(0);

  return(1);
}


/*****************************************************************************/

static int  add_env_file(appGroup, envFile, dbDir)

     char  *appGroup;
     char  *envFile;
     char  *dbDir;
{
  int   retCode;
  char  dirname[MAX_FILENAME_LEN];
  char  filename[MAX_FILENAME_LEN];
  char *slashptr;
  FILE  *fp;

  strcpy(dirname, dbDir);
  if ((slashptr = strrchr(dirname, (int)'/')))
    *slashptr = NTCSS_NULLCH;                 /* Get rid of the preceding slash */

  sprintf(filename, "%s/env/%s/ENV", dirname, appGroup);
  fp = fopen(filename, "w");
  if (fp == NULL)
    {
      printf("Unable to open ENV file <%s>!", filename);
      return(0);
    }

  retCode = fputs(envFile, fp);
  fclose(fp);

  if (retCode >= 0)
    return (1);

  return (0);
}


/*****************************************************************************/

static int  add_PSP_file(appGroup, PSPFile, dbDir)

     char  *appGroup;
     char  *PSPFile;
     char  *dbDir;
{
  char  fn[MAX_FILENAME_LEN];
  FILE *fp;
  int  retCode;
  char dn[MAX_FILENAME_LEN];
  char *slashptr;

  strcpy(dn, dbDir);
  if ((slashptr = strrchr(dn, (int)'/')))
    *slashptr = NTCSS_NULLCH;

  sprintf(fn, "%s/env/%s/PSP", dn, appGroup);
  fp = fopen(fn, "w");
  if (fp == NULL)
    {
      printf("Unable to PSP open file <%s>!", fn);
      return(0);
    }

  retCode = fputs(PSPFile, fp);
  fclose(fp);

  if (retCode >= 0)
    return (1);

  return (0);
}


/*****************************************************************************/

static int  add_SIG_file(appGroup, signum, dbDir)

     char  *appGroup;
     char  *dbDir;
     int  signum;
{
  int  retCode;
  char  dn[MAX_FILENAME_LEN];
  char  fn[MAX_FILENAME_LEN];
  char *slashptr;
  FILE *fp;

  strcpy(dn, dbDir);
  if ((slashptr = strrchr(dn, (int)'/')))
    *slashptr = NTCSS_NULLCH;

  sprintf(fn, "%s/env/%s/SIG", dn, appGroup);
  fp = fopen(fn, "w");
  if (fp == NULL)
    {
      printf("Unable to SIG open file <%s>!", fn);
      return(0);
    }

  retCode = fprintf(fp, "%d", signum);
  fclose(fp);
  
  if (retCode >= 0)
    return (1);

  return (0);
}


/*****************************************************************************/

int  getIniRecord(record, section, buf, fn)

     DBRecord  *record;
     char  *section;
     char  *fn;
     char  *buf;

{
  char  *q;
  char  KeyBuffer[MAX_INI_ENTRY * 100];
  char  sSingleKey[MAX_INI_ENTRY];
  int  i = 0;

  memset(record, NTCSS_NULLCH, sizeof(DBRecord));
  GetProfileSection(section, KeyBuffer, MAX_INI_ENTRY * 100, fn);
  q = KeyBuffer;                /*get each key & do seperate GPS for each key*/
  while (strlen(q))
    {
      memset(sSingleKey, NTCSS_NULLCH, MAX_INI_ENTRY);
      strcpy(sSingleKey, q);
      *(strchr(sSingleKey, '=')) = '\0';
      q += strlen(q) + 1;
      GetProfileString(section, sSingleKey, "!", buf, MAX_INI_ENTRY * 100, fn);
      strcpy(record->field[i++].data, buf);
    }

  return(i);
}


/*****************************************************************************/

char  getSecurityCode(strCode)

     char*  strCode;

{
  if(strstr("Unclassified-Sensitive",strCode))
    return('1');
  if(strstr(strCode,"Unclassified"))
    return('0');   
  if(strstr(strCode,"Confidential"))
    return('2');
  if(strstr(strCode,"Secret"))
    return('3');
  if(strstr(strCode,"Top Secret"))
    return('4');

  return(' ');
}


/*****************************************************************************/

char getJobCode(strCode)
char* strCode;
{

  if(strstr(strCode,"Client Program"))
    return '0';
  if(strstr(strCode,"Standard Predefined Job"))
    return '0';
  if(strstr(strCode,"Boot Job"))
    return '1';
  if(strstr(strCode,"App. Admin. Server Program"))
    return '2';
  if(strstr(strCode,"Scheduled Job"))
    return '2';
  if(strstr(strCode,"Host Admin. Server Program"))
    return '3';
  if(strstr(strCode,"Superuser Server Program"))
    return '4';
  if(strstr(strCode,"Server Program"))
    return '1';
  fprintf(logp,
      "ERROR: Unknown job class");
  return ' ';

/***
  char job_code;

  if(strstr(strCode,"Client Program"))
    job_code = '0';
  else if(strstr(strCode,"Standard Predefined Job"))
    job_code = '0';
  else if(strstr(strCode,"Boot Job"))
    job_code = '1';
  else if(strstr(strCode,"App. Admin. Server Program"))
    job_code = '2';
  else if(strstr(strCode,"Scheduled Job"))
    job_code = '2';
  else if(strstr(strCode,"Host Admin. Server Program"))
    job_code = '3';
  else if(strstr(strCode,"Superuser Server Program"))
    job_code = '4';
  else if(strstr(strCode,"Server Program"))
    job_code = '1';

  fprintf(logp,"getJobCode str=%s code=%c\n",strCode,job_code);

  return job_code;
***/
}


/*****************************************************************************/

char*  makeAccessString(strOldString, strApp, strNewAccessString, fn)

     char *strOldString;   /* data to work on */
     char *strApp;         /* ?? pointer */
     char *strNewAccessString;   /*  */
     char *fn;
{
  char  buf1[MAX_INI_ENTRY];
  char  seps[2];
  char *token;

  memset(strNewAccessString, NTCSS_NULLCH, MAX_INI_ENTRY);
  strcpy(seps, ",");
  token = strtok(strOldString, seps);
  while(token != NULL)
    {

/*******
      if (strstr(token, "NTCSS Admin"))
        strcat(strNewAccessString, "N,");
      else
        if (strstr(token, "App Admin"))
          strcat(strNewAccessString, "A,");
        else
*******/
          strcat(strNewAccessString, getRoleNumber(token, strApp, buf1, fn));
      token = strtok(NULL, seps);
    }
  if (strlen(strNewAccessString) > 0)
    /* take out last comma*/
     strNewAccessString[strlen(strNewAccessString) - 1] = '\0';

  return(strNewAccessString);
}


/*****************************************************************************/

char* getRoleNumber(strRole, strApp, buf1, fn)

     char  *strRole;
     char  *strApp;
     char  *buf1;
     char  *fn;

{
 
  char  buf2[MAX_INI_ENTRY*100];
  char  *p;
  int i = 1;

  sprintf(buf1, "%s_ROLES", strApp);
  GetSectionValues(buf1, buf2, MAX_INI_ENTRY * 100, fn);
  p = buf2;
  memset(buf1, NTCSS_NULLCH, MAX_INI_ENTRY);

  while(strlen(p) > 0)
    {
      if(strstr(p, strRole))
        {
          sprintf(buf1, "%d,", i);
          break;
        }
      i++;
      p += strlen(p) + 1;
    }
  return(buf1);
}
 

/*****************************************************************************/

void  CopyProfileSection(strSection, strProgrpsIni, strRegAppIni)

     char  *strSection;
     char  *strProgrpsIni;
     char  *strRegAppIni;

{
  char  Section[MAX_INI_ENTRY * 100];
  char  Section2[MAX_INI_ENTRY * 100];
  char  *p;
 
  memset(Section, NTCSS_NULLCH, MAX_INI_ENTRY * 100);
  memset(Section2, NTCSS_NULLCH, MAX_INI_ENTRY * 100);
  GetProfileSection(strSection, Section, MAX_INI_ENTRY * 100, strRegAppIni);
  WriteProfileSection(strSection, Section, strProgrpsIni);

  if(strstr(strSection, "ROLES"))
    return;
  GetSectionValues(strSection, Section2, MAX_INI_ENTRY * 100, strRegAppIni);
  p = Section2;
  while (strlen(p))
    {
      memset(Section, NTCSS_NULLCH,MAX_INI_ENTRY * 100);
      GetProfileSection(p, Section, MAX_INI_ENTRY * 100, strRegAppIni);
      WriteProfileSection(p, Section, strProgrpsIni);
      p += strlen(p) + 1;
    }
}


/*****************************************************************************/
/*  Converts database version 0.1.1 to version 0.2.0 */

int  DBconvert0_1_1to0_2_0(db_dir)

     char  *db_dir;

{
  char  app_passwd[MAX_APP_PASSWD_LEN+1];
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  str[DATETIME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;
  int  x;

/* * Remove init files. */
  sprintf(cmd, "%s %s/*.init", RM_PROG, db_dir);
  x = system(cmd);

/* * Create access roles table. */
  sprintf(cmd, "%s %s > %s/access_roles", ECHO_PROG, EMPTY_STR, db_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: (011->020) Problems executing %s!\n", cmd);
      return(-1);
    }

/* * Convert appusers table to accomodate application password. */
  sprintf(read_filename, "%s/appusers", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (011->020) Cannot open file %s!\n", read_filename);
      return(-2);
    }

  sprintf(write_filename, "%s/appusers.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, " ERROR: (011->020) Cannot open file %s!\n",
              write_filename);
      return(-3);
    }

  while (getRecord(&record, rp, 5))
    {
      getFullTimeStr(str);
      getRandomString(app_passwd, MAX_APP_PASSWD_LEN, 3, &str[5],
                      record.field[1].data, record.field[0].data);
      if (strcmp(NTCSS_APP, record.field[0].data) == 0)
        fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%d\n\n",
                record.field[0].data,
                record.field[1].data,
                record.field[2].data,
                record.field[3].data,
                "***",
                record.field[4].data,
                0);
      else
        fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%d\n\n",
                record.field[0].data,
                record.field[1].data,
                record.field[2].data,
                record.field[3].data,
                app_passwd,
                record.field[4].data,
                0);
    }
  fclose(rp);
  fclose(fp);
  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (010->020) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-4);
    }
  sprintf(cmd, "%s 600 %s/appusers", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: Cannot change permissions on appusers table!\n");

  return(0);  
}


/*****************************************************************************/
/*  Converts database version 0.2.0 to version 0.3.0 */

int  DBconvert0_2_0to0_3_0(db_dir)

     char  *db_dir;

{
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  int  x;
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;

/* * Remove output class table. */
  sprintf(cmd, "%s %s/output_class", RM_PROG, db_dir);
  x = system(cmd);

/* * Create output printers table. */
  sprintf(cmd, "%s %s/output_prt", TOUCH_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    {
      fprintf(logp, "ERROR: (020->030) Cannot create output printers"
              " table!\n");
      return(-1);
    }
  sprintf(cmd, "%s 600 %s/output_prt", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: (020->030) Cannot change permissions on output"
            " printers table.\n");

/* * Remove all references to default printers for output types. */
  sprintf(read_filename, "%s/output_type", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (020->030) Cannot open file %s!\n", read_filename);
      return(-2);
    }
  sprintf(write_filename, "%s/output_type.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR: (020->030) Cannot  Cannot open file %s!\n",
              write_filename);
      return(-3);
    }
  while (getRecord(&record, rp, 5))
    fprintf(fp, "%s\n%s\n%s\n\n%s\n\n",
            record.field[0].data,
            record.field[1].data,
            record.field[2].data,
            record.field[4].data);
  fclose(rp);
  fclose(fp);

  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (020->030)  Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-4);
    }

  sprintf(cmd, "%s 600 %s/output_type", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: (020->030) Cannot change permissions on"
            " output type table!\n");
  return(0);

}


/*****************************************************************************/
/*  Converts database version 0.3.0 to version 0.4.0 */

int  DBconvert0_3_0to0_4_0(db_dir)

     char  *db_dir;

{
  char  cmd[MAX_CMD_LEN];
  int  x;

/* * Remove adapter base table. */

  sprintf(cmd, "%s %s/ADAPTER_BASE", RM_PROG, db_dir);
  x = system(cmd);
  if ( x != 0)
    fprintf(logp, "WARNING: (030->040) Problems removing Adapter"
            " Base file!\n");
  return(0);
}

/*****************************************************************************/
/*  Converts database version 0.4.0 to version 0.5.0 */

int  DBconvert0_4_0to0_5_0(db_dir)

     char  *db_dir;

{
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;

/* * Convert ntcss users table to accomodate lock field. */
  sprintf(read_filename, "%s/ntcss_users", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (040->050) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/ntcss_users.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR: (040->050) Cannot open file %s!\n",
              write_filename);
      return(-2);
    }

  while (getRecord(&record, rp, 8))
    fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n\n",
            record.field[0].data,
            record.field[1].data,
            record.field[2].data,
            record.field[3].data,
            record.field[4].data,
            record.field[5].data,
            record.field[6].data,
            record.field[7].data,
            0);
  fclose(rp);
  fclose(fp);

  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (040->050) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }

  sprintf(cmd, "%s 600 %s/ntcss_users", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: (040->050) Cannot change permissions on"
            " ntcss_users table!\n");

/* * Create current applications table. */
  sprintf(write_filename, "%s/current_apps", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ERROR: (040->050) Cannot create file %s!\n",
              write_filename);
      return(-4);
    }
  fclose(fp);
  sprintf(cmd, "%s 600 %s/current_apps", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: (040->050) Cannot change permissions on"
            " the current_apps file!\n");

  return(0);
}


/*****************************************************************************/
/*  Converts database version 0.5.0 to version 1.0.0 */

int  DBconvert0_5_0to1_0_0(db_dir)

     char  *db_dir;

{
  char  buf[MAX_INSTALL_ITEM_LEN];
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  int  x;
  int  flag;
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;
  char  decryptfile[MAX_FILENAME_LEN];

/* * Convert ntcss users table to accomodate social security number field. */
  sprintf(read_filename, "%s/ntcss_users", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (050->100) Cannot open %s!\n", read_filename);   
      return(-1);
    }

  sprintf(write_filename, "%s/ntcss_users.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR: (050->100) Cannot open %s!\n", write_filename );
      return(-2);
    }

  while (getRecord(&record, rp, 9))
    fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
            record.field[0].data,
            record.field[1].data,
            record.field[2].data,
            record.field[3].data,
            "000-00-0000",
            record.field[4].data,
            record.field[5].data,
            record.field[6].data,
            record.field[7].data,
            record.field[8].data);
  fclose(rp);
  fclose(fp);
  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (050->100) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }

  sprintf(cmd, "%s 600 %s/ntcss_users", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: (050->100) Cannot change permissions on"
            " ntcss_users table!\n");
/* * Remove old ntcss app info file. */
  sprintf(cmd, "%s %s/../init_data/app_info/ntcss", RM_PROG, db_dir);
  x = system(cmd);

/* * Remove time templates file. */
  sprintf(cmd, "%s %s/TIME_TEMPS", RM_PROG, db_dir);
  x = system(cmd);

/* * Replace and/or add records in system configuration file. */
  sprintf(read_filename, "%s/SYS_CONF", db_dir);
  rp = decryptAndOpen(read_filename, decryptfile, "r");
  if (rp == NULL)
    {
      remove(decryptfile);
      fprintf(logp, "ERROR: (050->100) Cannot open %s!\n", read_filename);
      return(-4);
    }

  sprintf(write_filename, "%s/SYS_CONF.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      remove(decryptfile);
      fprintf(logp, "ERROR: (050->100) Cannot open %s!\n", write_filename);
      return(-5);
    }

  flag = 0;
  while (fgets(buf, MAX_FILENAME_LEN, rp) != NULL)
    {
      trim(buf);
      if (strncmp(buf, "HOME DIRECTORY:", strlen("HOME DIRECTORY:")) == 0)
        {
          fprintf(fp, "NTCSS HOME DIR:%s\n", &buf[strlen("HOME DIRECTORY:")]);
          flag |= 1;
        }
      else
        if (strncmp(buf, "NTCSS SYSLOG FILE:",
                    strlen("NTCSS SYSLOG FILE:")) == 0)
          flag |= 2;
        else
          fprintf(fp, "%s\n", buf);
    }

  if (!(flag & 1))
    fprintf(fp, "NTCSS HOME DIR: /users\n");
  if (!(flag & 2))
    fprintf(fp, "NTCSS SYSLOG FILE: %s\n", NTCSS_SYSLOG_FILE);
  fclose(rp);
  remove(decryptfile);
  closeAndEncrypt(fp, write_filename);

  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (050->100) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-6);
    }

  sprintf(cmd, "%s 600 %s/SYS_CONF", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: (050->100) Cannot change permissions on system"
            " configuration file!\n");
  return(0);
}


/*****************************************************************************/
/*  Converts database version 1.0.0 to version 1.0.1 */

int  DBconvert1_0_0to1_0_1(db_dir)

     char  *db_dir;

{
  char  app_passwd[MAX_APP_PASSWD_LEN+1];
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  str[DATETIME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;

/* * Convert ntcss users table to accomodate shared password field. */
  sprintf(read_filename, "%s/ntcss_users", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp,  "ERROR: (100->101) Cannot open %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/ntcss_users.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR:  (100->101) Cannot open %s!\n", write_filename);
      return(-2);
    }

  while (getRecord(&record, rp, 10))
    {
      getFullTimeStr(str);
      getRandomString(app_passwd, MAX_APP_PASSWD_LEN, 3, &str[5],
                      record.field[1].data, record.field[2].data);
      fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
              record.field[0].data,
              record.field[1].data,
              record.field[2].data,
              record.field[3].data,
              record.field[4].data,
              record.field[5].data,
              record.field[6].data,
              record.field[7].data,
              record.field[8].data,
              record.field[9].data,
              app_passwd);
    }
  fclose(rp);
  fclose(fp);
  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (100->101) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }
  sprintf(cmd, "%s 600 %s/ntcss_users", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: (100->101) Cannot change permissions on"
            " ntcss_users table!\n");

/* * Convert program groups table to accomodate link data field. */
  sprintf(read_filename, "%s/progroups", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (100->101) Cannot open file %s!\n",
              read_filename);
      return(-4);
    }
  
  sprintf(write_filename, "%s/progroups.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR: (100->101) Cannot open file %s!\n",
              write_filename);
      return(-5);
    }

  while (getRecord(&record, rp, 10))
    {
      fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
              record.field[0].data,
              record.field[1].data,
              record.field[2].data,
              record.field[3].data,
              record.field[4].data,
              record.field[5].data,
              record.field[6].data,
              record.field[7].data,
              record.field[8].data,
              record.field[9].data,
              "");
    }
  fclose(rp);
  fclose(fp);
  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (100->101) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-6);
    }
  sprintf(cmd, "%s 600 %s/progroups", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: (100->101) Cannot change permissions"
            "on progroups table!\n");

  return(0);
}

/*****************************************************************************/
/*  Converts database version 1.0.1 to version 1.0.2 */

int  DBconvert1_0_1to1_0_2(db_dir)

     char  *db_dir;

{
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  int  x;
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;

/* * Convert predefined jobs table to accomodate scheduled str field. */
  sprintf(read_filename, "%s/predefined_jobs", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (101->102) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/predefined_jobs.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      fprintf(logp, "ERROR: (101->102) Cannot open file %s!\n",
              write_filename);
      return(-2);
    }

  while (getRecord(&record, rp, 5))
    {
      fprintf(fp, "%s\n%s\n%s\n%s\n%s\n\n\n",
              record.field[0].data,
              record.field[1].data,
              record.field[2].data,
              record.field[3].data,
              record.field[4].data);
    }
  fclose(rp);
  fclose(fp);
  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (101->102) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }

  sprintf(cmd, "%s 600 %s/predefined_jobs", CHMOD_PROG, db_dir);
  x = system(cmd);
  if (x != 0)
    fprintf(logp, "WARNING: (101->102) Cannot change permissions on"
            " predefined jobs table!\n");
  
  return(0);
}

/*****************************************************************************/
/*  Converts database version 1.0.7 to version 3.0.0   6/98 */

/* This program assumes that the "Operator" and "Security" roles already
   exist in the progroups.ini file for progroup NTCSS. */
/* Also assumes that every user has access to progroup NTCSS. */

int  DBconvert1_0_7to3_0_0(db_dir)

     char  *db_dir;

{
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  int  x,y;                                    /* function call results */
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;
  /*char *default_ptr;*/
  char strLocalHost[200];
  /*int old_role,new_role;*/
  /* char  homedir[MAX_FILENAME_LEN]; */
  char calling_routine[RECSIZE];

  /* char  cmd[MAX_CMD_LEN];  */

  gethostname(strLocalHost,200);
  /* get current host type */
  x = getHostTypeFromInitData(strLocalHost);
  if (x < 0) {
           fprintf(logp, "DBconvert1_0_7to3_0_0: Error finding host type for <%s>", strLocalHost);
           return(-1);
  }

  /* copy old files to new location */
  Copy_Legacy_DB_Files(db_dir);

  /* if this is now an auth server */
  if (x == NTCSS_AUTH_SERVER_TYPE) {
      y=MergeData107to300(db_dir);
      if (y == 0)
	return(2);
      return(y);
  } else {
      /* add new printers and stuff from old files */
  }

  /* do this for either type of server */
  TrimOutputTypes(db_dir);

/*  Add in the filename field for the printers database */
  sprintf(read_filename, "%s/printers", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    { 
      fprintf(logp, "ERROR: (107->300) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/printers.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ERROR: (107->300) Cannot open file %s!\n",
              write_filename);
      fclose(rp);
      return(-2);
    }

  fprintf(logp, "Upgrading the printers database with the new filename"
          " field..\n");
  while (getRecord(&record, rp, 11))
    {
      fprintf(fp, PRINTERS_FMT,
              record.field[0].data,        /* prt_name */
              "NOHOST",
              record.field[2].data,        /* prt_location */
              atoi(record.field[3].data),  /* max_size */
              record.field[4].data,        /* prt_class_title */
              record.field[5].data,        /* ip_address */
              atoi(record.field[6].data),  /* security_class */
              atoi(record.field[7].data),  /* status */
              record.field[8].data,        /* port_name */
              atoi(record.field[9].data),  /* suspend_flag */
              record.field[10].data,       /* redirect_prt */
              "None");      /* FileName */
    }
  fclose(rp);
  fclose(fp);
  x = rename(write_filename, read_filename);
  if (x != 0)
    {
      fprintf(logp, "ERROR: (107->300) Cannot rename file %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }
  unlink(write_filename);

/**ACA***ACA*************** USE TRIMOUTPUTTYPE FUNCTION
  sprintf(read_filename, "%s/output_type", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (107->300) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/output_type.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ERROR: (107->300) Cannot open file %s!\n",
              write_filename);
      fclose(rp);
      return(-2);
    }

  fprintf(logp, 
     "Upgrading the output type database with the new printer name field..\n");

  while (getRecord(&record, rp, OUTPUT_TYPE_NUM_RECS)) {
	 default_ptr=strchr(record.field[3].data,'@');
	 if (default_ptr) 
		*default_ptr='\0';

 	fprintf(logp, "Changing printer name from to <%s> for otype <%s>\n", record.field[3].data,record.field[0].data);
       	fprintf(fp, OUTPUT_TYPE_FMT,
       		record.field[0].data,   
       		record.field[1].data,  
       		record.field[2].data, 
       		record.field[3].data, 
       		atoi(record.field[4].data)); 
    }
  fclose(rp);
  fclose(fp);

  x = rename(write_filename, read_filename);
  if (x != 0)
    {
      fprintf(logp, "ERROR: (107->300) Cannot rename file %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }
  unlink(write_filename);
******************************************ACA***ACA**/

  /* Leave a flag file to upgrade old user roles later in the prime process. */
  sprintf(write_filename, "%s/convertDB_to_300_flag", TOP_LEVEL_DIR);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "Couldn't create the 3.0.0 Conversion flag file!"
              "Information regarding old Ntcss Admins is lost!"
              " They will have to be added manually to the"
              " Security admin and Operator roles for progroup NTCSS.\n");
      return(-2);
    }
  fprintf(fp, "If this file exists, clear all user_role fields in"
          " ntcss_users, and assign"
          " all Ntcss Admins the new Ntcss Security and Operator roles.\n");
  fclose(fp);
 
  /* fix SYS_CONF tags if needed */
  FixSysConfigDataTags(db_dir);



  /* appusers update */
/*************************
  sprintf(write_filename, "%s/appusers.tmp", db_dir);
  sprintf(read_filename, "%s/appusers", db_dir);
  rp = fopen(read_filename, "r");
  if (!rp) {
    fprintf(logp,"Unable to open appusers for role conversion!");
    return(-2);
  }
  fp = fopen(write_filename, "w");
  if (!fp) {
    fprintf(logp,"Unable to open appusers.tmp for role conversion!");
    return(-2);
  }

  while (getRecord(&record, rp, 7)) {
    old_role=atoi(record.field[5].data);  
    new_role=old_role & 0xfffc;		 
    new_role *= 2;			
    old_role &= 0x0003;		
    new_role |= old_role;
    fprintf(fp,"%s\n%s\n%s\n%s\n%s\n%d\n%s\n\n",
	record.field[0].data,
	record.field[1].data,
	record.field[2].data,
	record.field[3].data,
	record.field[4].data,
	new_role,
	record.field[6].data);
  }
  fclose(rp);
  fclose(fp);

  rename(write_filename,read_filename);
*************************************************/
  /* add the printers to LP here, let printers reconcile on auths */
  if (x == NTCSS_MASTER_SERVER_TYPE) /*jj*/
    transfer_printers(db_dir);

/*********************fgr*********************/
  strcpy(calling_routine,"107->300");
  x=call_check_users(calling_routine);
  return(x);
/*********************fgr*********************/
}

/*********************fgr*********************/
  int call_check_users(char* calling_routine)
{
  int x;
  char home_dir[RECSIZE];

  /* Execute script "user_integ" to insure users have a home directory */
  /* Get UNIX home directory for formulating system script command */
  x=read_ntcss_system_settings("HOME_DIR",home_dir);
  home_dir[ strlen(home_dir) - 1 ] = 0;
  if (x < 0)
  {
      fprintf(logp, "WARNING: (%s): Cannot determine UNIX home"
              " directory for reconciling users.\n",calling_routine);
      return(-1);
  }
   /*  Execute script  */
   /* Providing an arg to user_integ causes it to check passwd & group*/
   /* strcat(home_dir,"/applications/data/install/user_integ PASS");*/
   strcat(home_dir,"/applications/data/install/user_integ"); /*No args*/
   if (system(home_dir) != 0)
   {
     fprintf(logp, "WARNING: (%s) 'user_integ' "
                   "script did not fully execute!\n",calling_routine);
     return(-1);
   }
   return(0);
/*********************fgr***********************/
}

int remove_brackets( char* buffer, char* tptr )
{
    int i,j,len,appfound;
    char wbuf[RECSIZE],*newtptr;

    len=strlen(buffer);
    if(strlen(buffer)<2)
        return(0);
    if(buffer[0]!='[')
        return(0);
    j=1;
    for(i=0;i<len;i++)
    {
       wbuf[i]=buffer[j++]; 
    } 
    for(i=len;i>0;i--)
    {
       if(wbuf[i]==']')
       {
           wbuf[i]=(char)0;
           break;
       }
    }
    wbuf[RECSIZE]=(char)0;
    for(i=0;i<len;i++)
    {
        buffer[i]=wbuf[i];
        if(buffer[i]==0)
            break;
    }
    buffer[i+1]=(char)0;

    if(strcmp(tptr,"IGNORE")==0)
    {
        return(0);
    }

    newtptr=tptr;
    appfound=0;
    while (strlen(newtptr))
    {
        if(strcmp(buffer,newtptr) == 0)
        {
            appfound=1;
            break;
        }
        newtptr += strlen(newtptr) + 1;
    }

    if(appfound==1)
        return(1);

    return(0);
}

int split_left(char *inbuf, char *retbuf)
{
/*
   Return left half of equation contained in inbuf back in retbuf
   Returns 1 if brackets removed
   Returns 0 if no brackets removed
*/

    int i,len;
    len=strlen(inbuf);
    if(len<2)
        return(0);
    for(i=0;i<len;i++)
    {
        retbuf[i]=inbuf[i];
        if(inbuf[i]=='=')
        {
            retbuf[i]=(char)0;
            return (1);
        }
    }
    return (0);
}


int split_right(char *inbuf, char *retbuf)
{
/*
   Return right half of equation contained in inbuf back in retbuf
   Returns 1 if something extracted
   Returns 0 if nothing extracted
*/

    int i,j,rptr,len;
    len=strlen(inbuf);
    if(len<2)
        return (0);
    memset(retbuf,0,RECSIZE);
    for(i=0;i<len;i++)
    {
        if(inbuf[i]=='=')
            break;
    }
    if(i==len)
    {
        retbuf[RECSIZE]=(char)0;
        return (0);
    }
    rptr=0;
    for(j=i+1;j<len;j++)
    {
        retbuf[rptr++]=inbuf[j];
        retbuf[RECSIZE]=(char)0;
    }
    return (1);
}

int read_ntcss_system_settings(const char *search_pattern,char *results_buf)
{
/* Look up search_pattern, return answer in results, like
   'GetProfileString' except just for rapid scan for UNIX_GROUPS entry.

   Returns 1 if pattern not found
   Returns 0 if pattern found
*/
    int j,len;
    char inbuf[RECSIZE],local_results[RECSIZE],*fr,dummy[7];
    FILE *settingsfp;

    strcpy(dummy,"IGNORE");
    settingsfp=fopen("/etc/ntcss_system_settings.ini","r");
    if(settingsfp==NULL)
    {
        printf("READ_NTCSS_SYSTEM_SETTINGS: ");
        printf("Bad open of /etc/ntcss_system_settings.ini file\n");
        exit (-1);
    }
    while (1)
    {
        memset(inbuf,0,RECSIZE);
        fr=fgets(inbuf,(int)RECSIZE,settingsfp);
        if(fr == NULL)
        {
            return(1);
        }
        j=remove_brackets(inbuf,dummy); /* Ignore j */
        len=strlen(search_pattern);
        j=split_left(inbuf,local_results);
        if(strcmp(local_results,search_pattern)==0)        
        {
            j=split_right(inbuf,local_results);
            break;
        }
    }
    strcpy(results_buf,local_results);
    fclose(settingsfp);
    return(0);
}

void settings_status(int i, const char *value)
{
/*
   Check results of read from /etc/ntcss_system_settings.ini file
*/
    if(i!=0)
    {
        printf("READ_NTCSS_SYSTEM_SETTINGS: ");
        printf("Unable to read %s from /etc/ntcss_system_settings.ini ",value);
        printf("file\n");
        exit(-1);
    }
}

/***********************************************************************/
/* Converts the old Ntcss 'superuser' role into an ordinary App Admin.  Also
   converts the old App Admin access bit into a normal App Administror role
   for each user. */
/* This function should only be called when converting from version 1.0.7 to
   version 3.0.0, preferably after  the ensure_database_integrity() function has run. */
/* The global FILE* variable, "logp" is assumed to be available for this function. */

int ConvertUsersFor3_0_0(db_dir)

     char *db_dir;

{
  char  regular_filename[MAX_FILENAME_LEN];
  char  temp_filename[MAX_FILENAME_LEN];
  char  progroup_title[MAX_PROGROUP_TITLE_LEN + 1];           
  char  login_name[MAX_LOGIN_NAME_LEN + 1];
  int  app_admin_role_offset = -1; /* role number for the App Admin role */
  int  i;                          /* loop controller */
  int  new_app_roles = 0;          /* holds the bitmask for the new roles */
  int  num_appusers;
  int  num_roles;
  int  num_users;
  int  op_role_offset = -1;        /* role number for the Operator role */
  int  sec_admin_role_offset = -1; /* role number for Security Administrator */
  int  x;                          /* function call results */
  UserItem  **Users;               /* all users */
  AppuserItem  **Appusers;         /* all appusers table entries */
  AccessRoleItem **AccessRoles;    /* all access_roles */
  FILE  *fp;

  fprintf(logp, "ConvertUsersFor3_0_0(): Converting users for"
          " version 3.0.0...\n");

  /* Record all the previous app Admins for each progroup before integrity
     checker wipes them out.*/
  sprintf(temp_filename, "%s/convert_app_admin_temp.CUF300", NTCSS_HOME_DIR);
  fp = fopen(temp_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      return(-1);
    }

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  /* record all app admins & assoc. progroups. */
  for (i = 0; i < num_appusers; i++) {
    if (Appusers[i]->app_role & 1 || Appusers[i]->app_role & 2)
    WriteProfileString( Appusers[i]->progroup_title,
                Appusers[i]->login_name, 
                Appusers[i]->login_name, 
                NTCSS_DB_CONVERT_30_USERS_FILE);
  }

  FlushINIFile(NTCSS_DB_CONVERT_30_USERS_FILE);
  freeAppuserData(Appusers, num_appusers);

  /* Get the role bitmask shifting settled first via integrity checker..*/
  x = ensure_database_integrity(db_dir); 
  if (x != 1)
    {
      print_out("ConvertUsersFor3_0_0(): Database integrity checker failed!");
      return(-2);
    }

  /* Read in the old app admins and assign them the new bits */
  fp = fopen(temp_filename, "r");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      return(-3);
    }

  /* Get each app admin entry and apply the new bitmask to each user */
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);
  AccessRoles = getAccessRoleData(&num_roles, db_dir);
  while (fgets(progroup_title, MAX_PROGROUP_TITLE_LEN, fp) != NULL)
    {
      manipStripNewLine(progroup_title);
      fgets(login_name, MAX_LOGIN_NAME_LEN, fp);
      manipStripNewLine(login_name);
      app_admin_role_offset = -1;  /* Init value in case it isn't found */
      for (i = 0; i < num_roles; i++)
        {
          if ( (strcmp(AccessRoles[i]->app_role, "App Administrator") == 0) &&
               (strcmp(AccessRoles[i]->progroup_title, progroup_title) == 0) )
            app_admin_role_offset = AccessRoles[i]->role_number; 
        }
      if (app_admin_role_offset == -1)    /* didn't find app admin!! */
        {
          fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Could not determine"
                  "App Admin role for progroup %s! User %s will have to be"
                  " assigned as an App Admin manually.",
                  progroup_title, login_name);
          continue;  /* just continue on to next user loop iteration */
        }
      changeAppUserArrayRole(login_name, progroup_title,
                             app_admin_role_offset, Appusers,
                             num_appusers);
      fgets(login_name, 3, fp);  /* skip the next blank line */
    }

  /* Find out bitmask offsets for 
        Security Administrator & Operator roles. */

  for (i = 0; i < num_roles; i++)
    {
      if (strncmp(AccessRoles[i]->progroup_title, "NTCSS", 5) == 0)
        {
         if (strncmp(AccessRoles[i]->app_role, "Operator", 8) == 0)
            {
              op_role_offset = AccessRoles[i]->role_number;
              continue;
            }
          if (strncmp(AccessRoles[i]->app_role,
                      "Security Administrator", 22) == 0)
            {
              sec_admin_role_offset = AccessRoles[i]->role_number;
              continue;
            }
        }
    }
  freeAccessRoleData(AccessRoles, num_roles);

  if ((op_role_offset < 0) || (sec_admin_role_offset < 0) ||
      (app_admin_role_offset < 0))
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Unable to determine the"
              " Ntcss Operator, Security Administrator, role numbers!\n");
      freeAppuserData(Appusers, num_appusers);
      return(1);                                           /* non-fatal */
    }

  new_app_roles |= (1 << (op_role_offset + 1));
  new_app_roles |= (1 << (sec_admin_role_offset + 1));

  fprintf(logp, "ConvertUsersFor3_0_0(): Assigning the base Ntcss"
          " user the Operator and Security Admin roles..\n");
  x = changeAppUserArrayRole("ntcssii", "NTCSS", new_app_roles,
                             Appusers, num_appusers);
  if (x == 1)
    fprintf(logp, "ConvertUsersFor3_0_0(): WARNING!  Base User not found!!\n");

  sprintf(temp_filename, "%s/appusers.tmp", db_dir);
  sprintf(regular_filename, "%s/access_auth", db_dir);

  /* Write out all the updated appuser entries */
  fp = fopen(temp_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      freeAppuserData(Appusers, num_appusers);
      return(-4);
    }

  for (i = 0; i < num_appusers; i++)
    fprintf(fp, APPUSERS_FMT,
            Appusers[i]->progroup_title,
            Appusers[i]->login_name,
            Appusers[i]->real_name,
            Appusers[i]->app_data,
            Appusers[i]->app_passwd,
            Appusers[i]->app_role,
            Appusers[i]->registered_user);

  fclose(fp);
  freeAppuserData(Appusers, num_appusers);

  x = rename(temp_filename, regular_filename);
  if (x!= 0)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() WARNING: Could not"
             " rename %s to %s!\n\tThe access_auth table will have"
             " to be renamed manually!\n", temp_filename, regular_filename);
      return(2);
    }

  sprintf(temp_filename, "%s/ntcss_users.tmp", db_dir);
  sprintf(regular_filename, "%s/ntcss_users", db_dir);

  /* Clear all user_role fields in the ntcss_users database... */
  fp = fopen(temp_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      return(-5);
    }

  Users = getUserData(&num_users, db_dir);

  fprintf(logp, "ConvertUsersFor3_0_0() Clearing the user_role fields"
          " in the ntcss_users table..\n");
  for (i = 0; i < num_users; i++)
    fprintf(fp, NTCSS_USERS_FMT,
            Users[i]->unix_id,
            Users[i]->login_name,
            Users[i]->real_name,
            Users[i]->password,
            Users[i]->ss_number,
            Users[i]->phone_number,
            Users[i]->security_class,
            Users[i]->pw_change_time,
            0,                            /* cleared user_role */
            Users[i]->user_lock,
            Users[i]->shared_passwd, "");
  freeUserData(Users, num_users);
  fclose(fp);

  x = rename(temp_filename, regular_filename);
  if (x!= 0)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() WARNING: Couldn't rename %s"
              " to %s!\n\tThe user_role fields in the ntcss_users table"
              "will have to be cleared manually.\n",
              temp_filename, regular_filename);
      return(3);                                         /* non-fatal */
    }

  fprintf(logp, "ConvertUsersFor3_0_0(): All users converted for"
          " version 3.0.0.\n");

  return(0);
}

/*****************************************************************************/
/* This function records the old app admins from the appusers table.  This 
function was needed since ensure_database_integrity clears the app admin &
super user bits during the prime process for the conversion to 3.0.0. */

/* Old Ntcss 'superusers' are converted into an ordinary App Admin for progroup NTCSS. */

int   SaveAdminsFor300Convert(db_dir)

     char *db_dir;

{
  int  i;                          /* loop controller */
  int  num_appusers;               /* number of appuser records */
  char  ini_file[MAX_FILENAME_LEN];
  AppuserItem  **Appusers;         /* all appusers table entries */

  /*  COLLECT ALL APP/NTCSS ADMIN USERS */

  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  sprintf(ini_file, "%s/%s/cvt300users.ini", NTCSS_HOME_DIR, NTCSS_LOGS_DIR);

  /* record all app admins & assoc. progroups. */
  for (i = 0; i < num_appusers; i++)
    {
      if (Appusers[i]->app_role & 1)  /* Regular App Admin */
        {
          WriteProfileString( "APPS",
                              Appusers[i]->progroup_title,
                              Appusers[i]->progroup_title,
                              ini_file);
          WriteProfileString( Appusers[i]->progroup_title,
                              Appusers[i]->login_name, 
                              Appusers[i]->login_name, 
                              ini_file);
        }
      if (Appusers[i]->app_role & 2)    /* Superuser */
        {
          WriteProfileString( "APPS",
                              "NTCSS",
                              "NTCSS",
                              ini_file);
          WriteProfileString( "NTCSS",
                              Appusers[i]->login_name, 
                              Appusers[i]->login_name, 
                              ini_file);
        }
    }

  FlushINIFile(NTCSS_DB_CONVERT_30_USERS_FILE);
  freeAppuserData(Appusers, num_appusers);

  return(0);
}

/*****************************************************************************/
/* Assigns the old App Admins to a normal App Administror role for each 
   progroup. This function was only intended for use during the db conversion
   to 3.0.0 */
/* The global FILE* variable, "logp" is assumed to be available for this
   function. (via prime_db.c) */

int  RestoreAdminsFor300Conversion(db_dir)

     char *db_dir;

{
  char  temp[MAX_NUM_PROGROUPS * MAX_LOGIN_NAME_LEN + 1];
  char  regular_filename[MAX_FILENAME_LEN];
  char  temp_filename[MAX_FILENAME_LEN];
  int  i;                          /* loop controller */
  int  num_appusers;
  int  x;                          /* function call results */
  AppuserItem  **Appusers;         /* all appusers table entries */
  FILE *fp;

  /* Get each app admin entry and apply the new bitmask to each user */
  Appusers = getAppuserData(&num_appusers, db_dir, WITH_NTCSS_GROUP);

  sprintf(regular_filename, "%s/%s/cvt300users.ini", NTCSS_HOME_DIR,
          NTCSS_LOGS_DIR);
  for (i = 0; i < num_appusers; i++)
    {
      memset(temp, NTCSS_NULLCH, 10);
      GetProfileString(Appusers[i]->progroup_title,
                       Appusers[i]->login_name,
                       "NotHere", temp, 10,
                       regular_filename);
      
      if (!strcmp(temp, "NotHere"))
        continue;  /* just continue on to next appuser loop iteration */
      
      /* It is assumed that the new progroups.ini file for this conversion
         has been applied and assigned all App Admin roles to the 3'rd bit!  */
      Appusers[i]->app_role |= 4;
    }

  sprintf(temp_filename, "%s/appusers.tmp", db_dir);
  sprintf(regular_filename, "%s/appusers", db_dir);

  /* Write out all the updated appuser entries */
  fp = fopen(temp_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      freeAppuserData(Appusers, num_appusers);
      return(-4);
    }

  for (i = 0; i < num_appusers; i++)
    {
      if (Appusers[i]->app_role != 0) 
      	fprintf(fp, APPUSERS_FMT,
              Appusers[i]->progroup_title,
              Appusers[i]->login_name,
              Appusers[i]->real_name,
              Appusers[i]->app_data,
              Appusers[i]->app_passwd,
              Appusers[i]->app_role,
              Appusers[i]->registered_user);
    }


  fclose(fp);
  freeAppuserData(Appusers, num_appusers);

  x = rename(temp_filename, regular_filename);

/*
  sprintf(temp_filename, "%s/ntcss_users.tmp", db_dir);
  sprintf(regular_filename, "%s/ntcss_users", db_dir);

  fp = fopen(temp_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              temp_filename);
      return(-5);
    }
  rp = fopen(regular_filename, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() ERROR: Cannot open file %s!\n",
              regular_filename);
      return(-5);
    }

  gethostname(myHostName,100);

  fprintf(logp, "ConvertUsersFor3_0_0() Clearing the user_role fields"
          " in the ntcss_users table..\n");

  while (getRecord(&record, rp, 11)) {
    	fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
    		record.field[0].data,
       		record.field[1].data,
       		record.field[2].data,
       		record.field[3].data,
       		record.field[4].data,
       		record.field[5].data,
       		record.field[6].data,
       		record.field[7].data,
       		0,
       		record.field[9].data,
       		record.field[10].data,
       		myHostName);
 	fflush(fp);

  }
  Users = getUserData(&num_users, db_dir);

  for (i = 0; i < num_users; i++)
    fprintf(fp, NTCSS_USERS_FMT,
            Users[i]->unix_id,
            Users[i]->login_name,
            Users[i]->real_name,
            Users[i]->password,
            Users[i]->ss_number,
            Users[i]->phone_number,
            Users[i]->security_class,
            Users[i]->pw_change_time,
            0, 
            Users[i]->user_lock,
            Users[i]->shared_passwd, myHostName);
  freeUserData(Users, num_users);
  fclose(fp);
  fclose(rp);

  x = rename(temp_filename, regular_filename);
  if (x!= 0)
    {
      fprintf(logp, "ConvertUsersFor3_0_0() WARNING: Couldn't rename %s"
              " to %s!\n\tThe user_role fields in the ntcss_users table"
              "will have to be cleared manually.\n",
              temp_filename, regular_filename);
      return(3);
    }
*/

  x = RemoveApplessAppusers(db_dir, ".tmp", NULL, 0, NULL,
                            0, 1);

  fprintf(logp, "ConvertUsersFor3_0_0(): All users converted for"
          " version 3.0.0.\n");
 
  /* added for upgrading 107 app servers to 300 auth servers */
  /* this will cause init to recreate all the printers and   */
  /* associate them to the correct apps */

  return(0);
}


int ConvertHostType(hosttype)
char *hosttype;
{
  if (!strcmp(hosttype,"Master Server"))
    return(NTCSS_MASTER);
  if (!strcmp(hosttype,"Application Server"))
    return(NTCSS_APP_SERVER);
  if (!strcmp(hosttype,"Authentication Server"))
    return(NTCSS_AUTH_SERVER);
  return(NTCSS_APP_SERVER);   /* sort of a default */
}

int ConvertRepl(repl)
char *repl;
{
  if (!strcmp(repl, "YES"))
    return(1);
  if (!strcmp(repl,"NO"))
    return(0);

  return(0);
}


int  DBconvert3_0_0to3_0_1(db_dir)

     char  *db_dir;

{
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  int  x;                                    /* function call results */
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;
  char tmp_hostname[MAX_FILENAME_LEN], strMaster[MAX_HOST_NAME_LEN];

/*  Add in the filename field for the printers database */
  sprintf(read_filename, "%s/hosts", db_dir);
  rp = fopen(read_filename, "r");
  if (rp == NULL)
    { 
      fprintf(logp, "ERROR: (300->301) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/hosts.temp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fprintf(logp, "ERROR: (300->301) Cannot open file %s!\n",
              write_filename);
      fclose(rp);
      return(-2);
    }

  gethostname(tmp_hostname,MAX_FILENAME_LEN);

  fprintf(logp, "Upgrading Host File with new host types\n");

  /* if the hostname in the DB is the current hostname, this will be the
     master otherwise we'll make it an app server.  Also clear the repl bit */
  while (getRecord(&record, rp, HOSTS_NUM_RECS))  {
      fprintf(fp, "%s\n%s\n%s\n%d\n%d\n\n",
              record.field[0].data,        /* hostname  */
              record.field[1].data,        /* ip addr   */
              /*atoi(*/record.field[2].data/*)*/,  /* num_procs */
              (!strcmp(tmp_hostname,record.field[0].data) ? NTCSS_MASTER : NTCSS_APP_SERVER),
              0);
  }
  fclose(rp);
  fclose(fp);
  x = rename(write_filename, read_filename);
  if (x !=0)
    {
      fprintf(logp, "ERROR: (300->301) Cannot rename file %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }
  unlink(write_filename);


  /* UPDATE USER STRUCTURE WITH AUTH SERVER */

  if (getMasterNameFromInitData(strMaster) < 0) {
    fprintf(logp, "ERROR: getMasterMasterNameFromInit failed");
    return(-1);
  } /* END if */

  /* do this only on the master */
  if (!strcmp(tmp_hostname,strMaster)) {
  	sprintf(read_filename, "%s/ntcss_users", db_dir);
  	rp = fopen(read_filename, "r");
  	if (rp == NULL){ 
    		fprintf(logp, "ERROR: (300->301) Cannot open file %s!\n", read_filename);
    		return(-1);
  	} /* END if */

  	sprintf(write_filename, "%s/ntcss_users.temp", db_dir);
  	fp = fopen(write_filename, "w");
  	if (fp == NULL) {
    		fprintf(logp, "ERROR: (300->301) Cannot open file %s!\n",
        	write_filename);
    		fclose(rp);
    		return(-2);
  	} /* END if */

  	fprintf(logp, "Upgrading the data with auth server"
       	   " field..\n");

  	/* if the hostname in the DB is the current hostname, this will be the
     	master otherwise we'll make it an app server.  
				Also clear the repl bit */

  	while (getRecord(&record, rp, 11))  {
    		fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
        	record.field[0].data,  /*   unix_id,       */
        	record.field[1].data,  /*  login_name,     */
        	record.field[2].data,  /*  real_name,      */
        	record.field[3].data,  /*  password,       */
        	record.field[4].data,  /*  ss_number,      */
        	record.field[5].data,  /*  phone_number,   */
        	record.field[6].data,  /*  security_class, */
        	record.field[7].data,  /*  pw_change_time, */
        	record.field[8].data,  /*  user_role,      */
        	record.field[9].data,  /*  user_lock,      */
        	record.field[10].data, /*  shared_passwd,  */
        	strMaster);
  	} /* END while */
  	fclose(rp);
  	fclose(fp);
  	x = rename(write_filename, read_filename);
  	if (x !=0)
    	{
      		fprintf(logp, "ERROR: (300->301) Cannot rename file %s to %s!\n",
              	write_filename, read_filename);
      		return(-3);
    	}
  	unlink(write_filename);
  }  /* if master server */

  return(0);
}

/*****************************************************************************/
/*  Converts database version 3.0.3 to version 3.0.4   6/99 */

/*  Encrypts the social security number fields in the users records in the
    ntcss_users database file. 
*/
int  DBconvert3_0_3to3_0_4(db_dir)

     char  *db_dir;

{
  char  cmd[MAX_CMD_LEN];
  char  read_filename[MAX_FILENAME_LEN];
  char  write_filename[MAX_FILENAME_LEN];
  char  decryptfile[MAX_FILENAME_LEN];
  FILE  *fp;
  FILE  *rp;
  DBRecord  record;
  char  key[(MAX_LOGIN_NAME_LEN * 2) + 1];
  char  *encryptedSSN; 


  sprintf(read_filename, "%s/ntcss_users", db_dir);
  rp = decryptAndOpen(read_filename, decryptfile, "r");
  if (rp == NULL)
    {
      fprintf(logp, "ERROR: (303->304) Cannot open file %s!\n", read_filename);
      return(-1);
    }

  sprintf(write_filename, "%s/ntcss_users.tmp", db_dir);
  fp = fopen(write_filename, "w");
  if (fp == NULL)
    {
      fclose(rp);
      remove(decryptfile);
      fprintf(logp, "ERROR: (303->304) Cannot open file %s!\n",
              write_filename);
      return(-2);
    }


  while (getRecord(&record, rp, 12)) {

    /* encrypt the social security number */
    sprintf(key, "%s%s", record.field[1].data, record.field[1].data);
    if (plainEncryptString(key, record.field[4].data, 
                           &encryptedSSN, 0, 0) != 0)  {
       fprintf(logp, "ERROR: (303->304) Cannot convert SSN!\n");
       return(-4);
    }


    fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
            record.field[0].data,
            record.field[1].data,
            record.field[2].data,
            record.field[3].data,
            encryptedSSN,
            record.field[5].data,
            record.field[6].data,
            record.field[7].data,
            record.field[8].data,
            record.field[9].data,
            record.field[10].data,
            record.field[11].data);
    fflush(fp);

    free(encryptedSSN);
  }

  fclose(rp);
  remove(decryptfile);
  closeAndEncrypt(fp, write_filename);

  if (rename(write_filename, read_filename) == -1)
    {
      fprintf(logp, "ERROR: (303->304) Cannot rename %s to %s!\n",
              write_filename, read_filename);
      return(-3);
    }

  sprintf(cmd, "%s 600 %s/ntcss_users", CHMOD_PROG, db_dir);
  if (system(cmd) != 0)
    fprintf(logp, "WARNING: (303->304) Cannot change permissions on"
            " ntcss_users table!\n");

  return (0);
}

int GetEncryptedBatchPassword(char *strLoginName, char *strEncryptedPassword)
{
char strKey[150];
char *pStrEncryptedPassword;

	sprintf(strKey,"%s%s",strLoginName,strLoginName);

	if (plainEncryptString(strKey, "ecafretni", &pStrEncryptedPassword,
                	         /* Use compression */ 0,
                       		 /* Use scrambling */ 1) != 0) {
		strEncryptedPassword[0]='\0';
		return(0);
	}
	strcpy(strEncryptedPassword,pStrEncryptedPassword);
	free(pStrEncryptedPassword);
	return(1);
}


int MergeData107to300(char *db_dir)
{
char strLocalHost[100];
char strMasterHost[100];
char nu_read_filename[MAX_FILENAME_LEN];
char nu_write_filename[MAX_FILENAME_LEN];
char au_read_filename[MAX_FILENAME_LEN];
char au_write_filename[MAX_FILENAME_LEN];
char ot_read_filename[MAX_FILENAME_LEN];
char ot_write_filename[MAX_FILENAME_LEN];
char op_read_filename[MAX_FILENAME_LEN];
char op_write_filename[MAX_FILENAME_LEN];
char appbuf[MAX_INI_ENTRY * 100];
char appserver[MAX_INI_ENTRY];
char appname[MAX_INI_ENTRY];
char datafile[MAX_FILENAME_LEN];
char cmd[MAX_FILENAME_LEN];
char homedir[MAX_FILENAME_LEN];
char wholedir[MAX_FILENAME_LEN];
char dest_datafile[MAX_FILENAME_LEN];
FILE *fp,*rp;
DBRecord  record;
int retval,j,x;
const char *memini="mergeusers.ini";
char *tptr,*aptr;
int cnt;
struct passwd *pptr;
struct group *gr;
char calling_routine[RECSIZE];

	printf("\n*** Migrating Data From Optimized Installation...\n");
	fflush(stdout);

	/* clear up app buffer */
	memset(appbuf,NULL,MAX_INI_ENTRY * 100);

	/* get my host name */
	memset(strLocalHost,NULL,100);
	gethostname(strLocalHost,100);

	/* get master name */
	getMasterNameFromInitData(strMasterHost);

	printf("Collecting files from Master Server <%s>\n",strMasterHost);
	fflush(stdout);
        /*************************************************************
         We have to get the master files and trim up the data to only
         include this hosts apps.  The appusers and ntcss_users files
         will be read in and truncated.  We have to save the apps
         on this host first.
        *************************************************************/
	/* Copy files from the master,,which should be running */
	
	printf("....Transferring: Users");
	fflush(stdout);

	sprintf(dest_datafile,"%s/ntcss_users",db_dir);
	retval=net_copy("get",strMasterHost,
		DB_NTCSS_USERS_TABLE_PATHNAME,
		dest_datafile,
		XFER_ASCII);
       	if (retval < 0) {
		fprintf(logp, "MergeUsers107to300: Error getting ntcss_users file from master!");
		return(-1);
	}
	printf(" AppUsers,");
	fflush(stdout);
        /* get appusers file */
	sprintf(dest_datafile,"%s/appusers",db_dir);
	retval=net_copy("get",strMasterHost,
		DB_NTCSS_APPUSERS_TABLE_PATHNAME,
		dest_datafile,
		XFER_ASCII);
       	if (retval < 0) {
		fprintf(logp, "MergeUsers107to300: Error getting appusers file from master!");
		return(-1);
	}
	printf(" Output Types,");
	fflush(stdout);

        /* output type file */
	strcpy(datafile,"/usr/local/NTCSSII.leg/database/output_type");
	sprintf(dest_datafile,"%s/output_type",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting output type file from master!");
                return(-1);
        }

        /* output type file */
        strcpy(datafile,"/usr/local/NTCSSII.leg/database/output_prt");
	sprintf(dest_datafile,"%s/output_prt",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting output prt file from master!");
                return(-1);
        }

	printf("Printers.");
	fflush(stdout);

        /***** RECONCILE SHOULD WORK  printer access file  printers file
        sprintf(datafile,"%s/printers",NTCSS_DB_DIR);
	sprintf(dest_datafile,"%s/printers",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting printers file from master!");
                return(-1);
        }

        sprintf(datafile,"%s/printer_access",NTCSS_DB_DIR);
	sprintf(dest_datafile,"%s/printer_access",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting printer access file from master!");
                return(-1);
        }
        **********************************/

        /* printer ini file */
        sprintf(datafile,"%s/ntprint.ini",NTCSS_DB_DIR);
	sprintf(dest_datafile,"%s/ntprint.ini",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting printer access file from master!");
                return(-1);
        }
        /* printer class file */
        sprintf(datafile,"%s/prt_class",NTCSS_DB_DIR);
	sprintf(dest_datafile,"%s/prt_class",db_dir);
        retval=net_copy("get",strMasterHost,
                datafile, dest_datafile,
                XFER_ASCII);
        if (retval < 0) {
                fprintf(logp, "MergeUsers107to300: Error getting printer access file from master!");
                return(-1);
        }


	/* collect apps on this host */
 	 GetSectionValues("APPS", appbuf, MAX_INI_ENTRY * 100, NTCSS_PROGRP_ID_FILE);
      	tptr = appbuf;

	printf("..Done.\n");
	fflush(stdout);

	printf("....Collecting Apps: ");
	fflush(stdout);
	cnt=0;

	while (strlen(tptr)) {

		/* get next apps server name */
       		GetProfileString(tptr,"SERVER","NOHOST",appserver,
			MAX_INI_ENTRY, NTCSS_PROGRP_ID_FILE);

		/* if host names match...save to memory ini file */
		if (!strcmp(appserver,strLocalHost)) {
       			GetProfileString(tptr,"GROUP","NOHOST",appname,
				MAX_INI_ENTRY, NTCSS_PROGRP_ID_FILE);
			if (cnt)
				printf(",<%s>",tptr);
			else
				printf("<%s>",tptr);
			cnt++;
			fflush(stdout);
			WriteProfileString("APPS",appname,appname,memini);
		}
		tptr += strlen(tptr)+1;

	}
	printf("\n");
	fflush(stdout);

		
	/****************************************************
	 PROCESS THE APPUSERS TABLE HERE.  ONLY KEEP APPUSER
	 ENTRIES FOR USERS RELATED TO APPS ON THIS HOST, 
	 DISCARD ALL OTHERS.
	****************************************************/
	printf("....Collecting App Users: ");
	fflush(stdout);

	cnt=0;

  	sprintf(au_read_filename, "%s/appusers", db_dir);
  	rp = fopen(au_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open appusers file");
      		return(-1);
    	}

  	sprintf(au_write_filename, "%s/appusers.tmp", db_dir);
  	fp = fopen(au_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open appusers.tmp file");
      		fclose(rp);
      		return(-1);
    	}

  	while (getRecord(&record, rp, APPUSERS_NUM_RECS)) {
       		GetProfileString("APPS",(const char *)record.field[0].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP") && strcmp("ntcssii",record.field[1].data)) 
			continue;
		cnt++;
       		fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
               		record.field[0].data,
               		record.field[1].data,
               		record.field[2].data,
               		record.field[3].data,
               		record.field[4].data,
               		record.field[5].data,
               		record.field[6].data);
		fflush(fp);
		WriteProfileString("USERS", record.field[1].data,
			 record.field[1].data,memini);
	}
	fclose(fp);
	fclose(rp);

	printf(" %d entries transferred.\n",cnt);
	fflush(stdout);

	/****************************************************
	 PROCESS THE USERS TABLE HERE.  ONLY KEEP  USER
	 ENTRIES FOR USERS THAT WERE FOUND IN THE APPUSER 
	 PHASE, DISCARD ALL OTHERS.
	****************************************************/

	printf("....Collecting Ntcss Users: ");
	fflush(stdout);

	cnt=0;


        /* prepare to add the users home directory */
        sprintf(cmd, "SPECIAL NO LOCK BACKDOOR%s", db_dir);
        j = getNtcssSysConfData("NTCSS HOME DIR", homedir, cmd);
        if (j < 0)
        {
          fprintf(logp, "Init_database(): Cannot determine UNIX home"
                  " directory for adding batch users!\n");
          return(0);
        }

  	sprintf(nu_read_filename, "%s/ntcss_users", db_dir);
  	rp = fopen(nu_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open ntcss_users file");
      		return(-1);
    	}

  	sprintf(nu_write_filename, "%s/ntcss_users.tmp", db_dir);
  	fp = fopen(nu_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open ntcss_users.tmp file");
      		fclose(rp);
      		return(-1);
    	}


	gr=getgrnam("ntcss");
	
        /* for the auth server, let's trim the users file and
           add the users home dir since we need to source the env here */
  	while (getRecord(&record, rp, 12)) {
       		GetProfileString("USERS",(const char *)record.field[1].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP")) 
			continue;

		cnt++;
    		fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", 
			record.field[0].data, 
			record.field[1].data, 
			record.field[2].data, 
			record.field[3].data, 
			record.field[4].data, 
			record.field[5].data, 
			record.field[6].data, 
			record.field[7].data, 
			record.field[8].data, 
			record.field[9].data, 
			record.field[10].data, 
			record.field[11].data);
    		fflush(fp);

		sprintf(wholedir,"%s/%s",homedir,record.field[1].data);
		mkdir(wholedir,(mode_t)S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
		pptr=getpwnam(record.field[1].data);
		if (pptr && gr) 
			chown(wholedir,pptr->pw_uid,gr->gr_gid);
	}

	/* close all files */
	fclose(fp);
	fclose(rp);

	printf("\n %d Users Transferred.\n",cnt);
	fflush(stdout);

	/****************************************************
	 PROCESS THE OUTPUTTYPE TABLE HERE.  ONLY KEEP OTYPE
	 ENTRIES FOR OTYPES THAT WERE FOUND IN THE APPUSER 
	 PHASE, DISCARD ALL OTHERS.
	****************************************************/
	printf("....Collecting Output Types: ");
	fflush(stdout);
	cnt=0;

  	sprintf(ot_read_filename, "%s/output_type", db_dir);
  	rp = fopen(ot_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_type file");
      		return(-1);
    	}

  	sprintf(ot_write_filename, "%s/output_type.tmp", db_dir);
  	fp = fopen(ot_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_type.tmp file");
      		fclose(rp);
      		return(-1);
    	}

	cnt=0;
  	while (getRecord(&record, rp, 5)) {
       		GetProfileString("APPS",(const char *)record.field[1].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP")) 
			continue;

/*******Diagnostic commented out by fgr ********
		if (cnt)
			printf(",<%s/%s>",record.field[0].data,record.field[1].data);
		else
			printf("<%s/%s>",record.field[0].data,record.field[1].data);
******************fgr***************************/
		cnt++;
		fflush(stdout);

		aptr=strchr(record.field[3].data,'@');
		if (aptr)
			*aptr=NULL;

    		fprintf(fp, "%s\n%s\n%s\n%s\n%s\n\n",
       			record.field[0].data,
       			record.field[1].data,
       			record.field[2].data,
       			record.field[3].data,
       			record.field[4].data);
    		fflush(fp);

	}
	fclose(fp);
	fclose(rp);

	if (cnt)
		printf("\n");
	else
		printf("None.\n");
	fflush(stdout);

	/****************************************************
	 PROCESS THE OUTPUTPRTS TABLE HERE.  ONLY KEEP OTYPE
	 ENTRIES FOR OTYPES THAT WERE FOUND IN THE APPUSER 
	 PHASE, DISCARD ALL OTHERS.
	****************************************************/
	printf("....Collecting Output Printers: ");
	fflush(stdout);
	cnt=0;

  	sprintf(op_read_filename, "%s/output_prt", db_dir);
  	rp = fopen(op_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_prt file");
      		return(-1);
    	}

  	sprintf(op_write_filename, "%s/output_prt.tmp", db_dir);
  	fp = fopen(op_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_prt.tmp file");
      		fclose(rp);
      		return(-1);
    	}

  	while (getRecord(&record, rp, 4)) {
       		GetProfileString("APPS",(const char *)record.field[1].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP")) 
			continue;

/**** Diagnostic commented out by fgr ****
                if (cnt)
                   printf(",<%s>",record.field[0].data);
                else
		   printf("<%s>",record.field[0].data);
*******************fgr*******************/
                cnt++;
		fflush(stdout);

    		fprintf(fp, "%s\n%s\n%s\n%s\n\n",
       			record.field[0].data,
       			record.field[1].data,
       			record.field[2].data,
       			record.field[3].data);
    		fflush(fp);

	}
	fclose(fp);
	fclose(rp);

	if (cnt)
		printf("\n");
	else
		printf("None.\n");
	fflush(stdout);
  
	rename(au_write_filename,au_read_filename);
	rename(nu_write_filename,nu_read_filename);
	rename(ot_write_filename,ot_read_filename);
	rename(op_write_filename,op_read_filename);

        /* clearing printer and adapter files, reconcile wil populate them */
        sprintf(dest_datafile,"%s -f /dev/null %s/printers", CP_PROG,  db_dir);
        system(dest_datafile);

        sprintf(dest_datafile,"%s -f /dev/null %s/adapters", CP_PROG,  db_dir);
        system(dest_datafile);
	if (FixSysConfigDataTags(db_dir)) 
		printf("....WARNING:Unable to update SYS_CONF data, perform manually!\n");

/************fgr*******************/
        strcpy(calling_routine,"DB Convert 107->300");
        x=call_check_users(calling_routine);
/************fgr*******************/

        printf("Optimized Data Migration Complete.\n\n");
        fflush(stdout);

        return(x);
}


/* This function will copy the database files, message files, spool files, fs_archive files and srv_dev files from the old directory location to the new directory location */
int Copy_Legacy_DB_Files(char *db_dir)
{
  char cmd[MAX_CMD_LEN];
  struct stat  sbuf;
  char legdir[MAX_CMD_LEN];

     sprintf(legdir,"%s/database",NTCSS_LEGACY_DIR);
     sprintf(cmd,"%s -f %s/database/* %s 2>&1 > /dev/null", CP_PROG,
		NTCSS_LEGACY_DIR, db_dir);
     if (!stat(legdir,&sbuf)) 
     	system(cmd);


     sprintf(cmd,"%s -f /dev/null %s/current_users", CP_PROG,  db_dir);
     system(cmd);

     sprintf(cmd,"%s -rf %s/spool/* %s 2>&1 > /dev/null", CP_PROG,
			NTCSS_LEGACY_DIR, NTCSS_SPOOL_DIR);
     sprintf(legdir,"%s/spool",NTCSS_LEGACY_DIR);
     if (!stat(legdir,&sbuf)) 
     	system(cmd);

/**** got rid of this to avoid confusion on old backups and devices
     sprintf(cmd,"%s -rf %s/applications/data/fs_archive/* %s/applications/data/fs_archive 2>&1 > /dev/null",CP_PROG,NTCSS_LEGACY_DIR, NTCSS_HOME_DIR);
     sprintf(legdir,"%s/applications/data/fs_archive",NTCSS_LEGACY_DIR);
     if (!stat(legdir,&sbuf)) 
     	system(cmd);
*******/

     sprintf(cmd,"%s -rf %s/applications/data/srv_dev/* %s/applications/data/srv_dev 2>&1 > /dev/null",CP_PROG,NTCSS_LEGACY_DIR, NTCSS_HOME_DIR);
     sprintf(legdir,"%s/applications/data/srv_dev",NTCSS_LEGACY_DIR);
     if (!stat(legdir,&sbuf)) 
     	system(cmd);

     /* remove old .bin, .dat, and .idx files */
     sprintf(cmd,"%s %s/database/*.idx %s/database/*.dat %s/database/*.bin", CP_PROG, NTCSS_DB_DIR,NTCSS_DB_DIR,NTCSS_DB_DIR);

     return(0);
}

int FixSysConfigDataTags(char *db_dir)
{

	FILE *fp,*rp;
	char read_filename[512];
	char write_filename[512];
	char line_data[512];
	char *tptr;
	int found_home_dir,found_prof_dir;
	
	found_home_dir=found_prof_dir=0;

	sprintf(read_filename,"%s/SYS_CONF",db_dir);
	sprintf(write_filename,"%s/SYS_CONF.tmp",db_dir);

	rp=fopen(read_filename,"r");
	if (!rp) {
		return(-1);
	}

	fp=fopen(write_filename,"w");
	if (!fp) {
		fclose(rp);
		return(-1);
	}

	while (!feof(rp)) {
		memset(line_data,'\0',512);
		fgets(line_data,512,rp);
		if (!strncmp(line_data,"NTUSERDIR:",strlen("NTUSERDIR:"))) {
			found_home_dir=1;
			trim(line_data);
			tptr=strchr(line_data,'\\');
                        /* if no back slash, look for fwd slash */
			if (!tptr)
				tptr=strchr(line_data,'/');
			/* if neither fwd or back,, assume OK */
			if (tptr) {
				printf("....Changing NT Home dir from <%s> to <%s>.\n",line_data+strlen("NTUSERDIR: "),tptr+1);
				fprintf(fp,"NTUSERDIR: %s\n",tptr+1);
			} else {
				printf("....NT Home dir appears valid <%s>.\n",line_data+strlen("NTHOMEDIR: "));
				fputs(line_data,fp);
			}
		} else if (!strncmp(line_data,"NTPROFDIR:",strlen("NTPROFDIR:"))) {
			found_prof_dir=1;
			trim(line_data);
			tptr=strchr(line_data,'\\');
                        /* if no back slash, look for fwd slash */
			if (!tptr)
				tptr=strchr(line_data,'/');
			/* if neither fwd or back,, assume OK */
			if (tptr) {
				printf("....Changing NT Profile dir from <%s> to <%s>.\n",line_data+strlen("NTPROFDIR: "),tptr+1);
				fprintf(fp,"NTPROFDIR: %s\n",tptr+1);
			} else {
				printf("......NT Profile dir appears valid <%s>.\n",line_data+strlen("NTPROFDIR: "));
				fputs(line_data,fp);
			}
		} else 
			fputs(line_data,fp);
		fflush(fp);
		
	}
	if (!found_home_dir)
		fprintf(fp,"NTUSERDIR: NTUSERS\n");
	if (!found_prof_dir)
		fprintf(fp,"NTPROFDIR: PROFILES\n");
	fclose(rp);
	fclose(fp);
	if (rename(write_filename,read_filename))
		printf("Unable to rename file <%s> to <%s>, %d\n",write_filename,read_filename, errno);
	return(0);
}


/********************
This function will trim the output types from the master datbase.  It
is duplicatd from the Merge107to300 fucntion only to help expedite the 
ASPEN release.  Notice this does not touch ntcss_users or appusers.  These
files need only be touched on the auth_servers.  The master is already
correct.
********************/
int TrimOutputTypes(char *db_dir)
{
char strLocalHost[100];
char strMasterHost[100];
char ot_read_filename[MAX_FILENAME_LEN];
char ot_write_filename[MAX_FILENAME_LEN];
char op_read_filename[MAX_FILENAME_LEN];
char op_write_filename[MAX_FILENAME_LEN];
char appbuf[MAX_INI_ENTRY * 100];
char appserver[MAX_INI_ENTRY];
char appname[MAX_INI_ENTRY];
/*fgr char datafile[MAX_FILENAME_LEN];*/
/*fgr char dest_datafile[MAX_FILENAME_LEN];*/
FILE *fp,*rp;
DBRecord  record;
/*fgr int retval;*/
const char *memini="mergeusers.ini";
char *tptr,*aptr;
int cnt;


	/* clear up app buffer */
	memset(appbuf,NULL,MAX_INI_ENTRY * 100);

	/* get my host name */
	memset(strLocalHost,NULL,100);
	gethostname(strLocalHost,100);

	/* get master name */
	getMasterNameFromInitData(strMasterHost);

	/* collect apps on this host */
 	 GetSectionValues("APPS", appbuf, MAX_INI_ENTRY * 100, NTCSS_PROGRP_ID_FILE);
      	tptr = appbuf;

	printf("....Collecting Apps: ");
	fflush(stdout);
	cnt=0;

	while (strlen(tptr)) {

		/* get next apps server name */
       		GetProfileString(tptr,"SERVER","NOHOST",appserver,
			MAX_INI_ENTRY, NTCSS_PROGRP_ID_FILE);

		/* if host names match...save to memory ini file */
		if (!strcmp(appserver,strLocalHost)) {
       			GetProfileString(tptr,"GROUP","NOHOST",appname,
				MAX_INI_ENTRY, NTCSS_PROGRP_ID_FILE);
/*******diagnostic commented out by fgr*************
			if (cnt)
				printf(",<%s>",tptr);
			else
				printf("<%s>",tptr);
**********************fgr**************************/
			cnt++;
			fflush(stdout);
			WriteProfileString("APPS",appname,appname,memini);
		}
		tptr += strlen(tptr)+1;

	}
	printf("..Done.\n");
	fflush(stdout);

		
	/****************************************************
	 PROCESS THE OUTPUTTYPE TABLE HERE.  ONLY KEEP OTYPE
	 ENTRIES FOR OTYPES THAT WERE FOUND IN THE APPUSER 
	 PHASE, DISCARD ALL OTHERS.
	****************************************************/
	printf("....Collecting Output Types: ");
	fflush(stdout);
	cnt=0;

  	sprintf(ot_read_filename, "%s/output_type", db_dir);
  	rp = fopen(ot_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_type file");
      		return(-1);
    	}

  	sprintf(ot_write_filename, "%s/output_type.tmp", db_dir);
  	fp = fopen(ot_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_type.tmp file");
      		fclose(rp);
      		return(-1);
    	}

	cnt=0;
  	while (getRecord(&record, rp, 5)) {
       		GetProfileString("APPS",(const char *)record.field[1].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP")) 
			continue;

/**********diagnostic commented out by fgr **********
		if (cnt)
			printf(",<%s/%s>",record.field[0].data,record.field[1].data);
		else
			printf("<%s/%s>",record.field[0].data,record.field[1].data);
*******************fgr*******************************/
		cnt++;
		fflush(stdout);

fprintf(logp,"Checking ot <%s> for at\n",record.field[3].data);
		aptr=strchr(record.field[3].data,'@');
		if (aptr) {
			*aptr=NULL;
fprintf(logp,"found ot with at new is <%s>\n",record.field[3].data);
		}

    		fprintf(fp, "%s\n%s\n%s\n%s\n%s\n\n",
       			record.field[0].data,
       			record.field[1].data,
       			record.field[2].data,
       			record.field[3].data,
       			record.field[4].data);
    		fflush(fp);

	}
	fclose(fp);
	fclose(rp);

	if (cnt)
		printf("\n");
	else
		printf("None.\n");
	fflush(stdout);

	/****************************************************
	 PROCESS THE OUTPUTPRTS TABLE HERE.  ONLY KEEP OTYPE
	 ENTRIES FOR OTYPES THAT WERE FOUND IN THE APPUSER 
	 PHASE, DISCARD ALL OTHERS.
	****************************************************/
	printf("....Collecting Output Printers: ");
	fflush(stdout);
	cnt=0;

  	sprintf(op_read_filename, "%s/output_prt", db_dir);
  	rp = fopen(op_read_filename, "r");
  	if (rp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_prt file");
      		return(-1);
    	}

  	sprintf(op_write_filename, "%s/output_prt.tmp", db_dir);
  	fp = fopen(op_write_filename, "w");
  	if (fp == NULL) {
      		fprintf(logp, "MergeUsers107to300: cannot open output_prt.tmp file");
      		fclose(rp);
      		return(-1);
    	}

  	while (getRecord(&record, rp, 4)) {
       		GetProfileString("APPS",(const char *)record.field[1].data,
			"NOAPP",appserver,
			MAX_INI_ENTRY,memini);
		if (!strcmp(appserver,"NOAPP")) 
			continue;

                if (cnt)
                   printf(",<%s>",record.field[0].data);
                else
		   printf("<%s>",record.field[0].data);
                cnt++;
		fflush(stdout);

    		fprintf(fp, "%s\n%s\n%s\n%s\n\n",
       			record.field[0].data,
       			record.field[1].data,
       			record.field[2].data,
       			record.field[3].data);
    		fflush(fp);

	}
	fclose(fp);
	fclose(rp);

	if (cnt)
		printf("\n");
	else
		printf("None.\n");
	fflush(stdout);

	rename(ot_write_filename,ot_read_filename);
	rename(op_write_filename,op_read_filename);

        printf("Optimized Data Migration Complete.\n\n");
        fflush(stdout);

	return(0);
}

/****************************************************************/
/* this function will add all printers to the system from a 1.07*/
/* to a 3.05 conversion*/

void transfer_printers(char *db_dir)
{

  PrtItem **Printers;
  AppItem **Apps;
  int num_printers,num_apps,i;
  char cmd[MAX_FILENAME_LEN], print_dir[MAX_FILENAME_LEN];
  char machine_addr[MAX_INI_ENTRY];
  char adapter_type[MAX_INI_ENTRY];

  Apps = getAppData(&num_apps, db_dir, WITHOUT_NTCSS_GROUP);
  Printers = getPrtData(&num_printers, db_dir, Apps, num_apps);

  sprintf(print_dir, "%s/%s", NTCSS_HOME_DIR, NTCSS_PRT_CONF_DIR);

  printf("....Transferring Printers: \n\n");
  for (i = 0; i < num_printers; i++)
  {

    printf("....Printer/AdapterIP <%s/%s>\n",Printers[i]->prt_name,
	Printers[i]->ip_address);

    if(!GetAdapterType(Printers[i]->ip_address,adapter_type, machine_addr))
        continue;

    sprintf(cmd, "%s/main_print ADD %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
    print_dir, 
    Printers[i]->prt_name,
    Printers[i]->port_name, 
    Printers[i]->ip_address,
    machine_addr,
    adapter_type,
    Printers[i]->prt_class_title);

    system(cmd);  /*we dont care about the return value*/

  }

  printf("....Printer Transfer Complete \n\n");

  freePrtData(Printers, num_printers); 
  freeAppData(Apps,num_apps);
}


/*********************************************************/

static int  GetAdapterType (const char  *ip,
                            char  *type,
                            char  *ether)
{
  AdapterItem  **Adapters;
  int   num_adapters;
  int   i;
  int found=0;

  Adapters = getAdapterData(&num_adapters, NTCSS_DB_DIR);
  memset(type, '\0', MAX_INI_ENTRY);

  for (i = 0; i < num_adapters; i++)
    if (!strcmp(ip, Adapters[i]->ip_address))
    {
      strcpy(type, Adapters[i]->adapter_type);
      strcpy(ether, Adapters[i]->machine_address);
      found=1;
      break;
    }

  freeAdapterData(Adapters, num_adapters); 
  return found;
}
