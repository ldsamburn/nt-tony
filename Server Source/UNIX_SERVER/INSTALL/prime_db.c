
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * prime_db.c
 */

/** SYSTEM INCLUDES **/

#include <errno.h>
#include <search.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

/** INRI INCLUDES **/

#include <TextDBP.h>
#include <Ntcss.h>
#include <StringFcts.h>
#include <ini_Fcts.h>
#include <ExtraProtos.h>


/** LOCAL INCLUDES **/

#include "install.h"
#include "inri_version.h"


/** FUNCTION PROTOTYPES **/

void  print_out(const char*);
int  main(int, char**);
int   getHostTypeFromInitData(char*);
int  getMasterNameFromInitData(char*);
void  cleanupSystemSettings(void);


/** STATIC VARIABLES **/

static int  net_install = 0;
static int  nResponsibleForSystemSettings = 0;


/** GLOBAL VARIABLES **/

FILE  *logp;
int  add_unix_batch_users;


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/

void  print_out(msg)

const char *msg;

{
  syslog(LOG_WARNING, msg);
  if (logp != NULL)
    {
      fprintf(logp, msg);
      fprintf(logp, "\n");
    }
  if (!net_install)
    {
      printf(msg);
      printf("\n");
    }
}


/*****************************************************************************/

void  cleanupSystemSettings()

{
  if (!nResponsibleForSystemSettings)
    {
      return;
    }

  freeNtcssSystemSettingsShm();

}

/*****************************************************************************/

int  getHostTypeFromInitData(strHostName)

     char  *strHostName;
{
  char strHostEntrySectionHeaders[MAX_NUM_HOSTS][MAX_HOST_NAME_LEN + 1];
  char strEntryHostType[200], strEntryHostName[200], *pCh;
  char strHostIniEntries[MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + 2)];
  char strIniFileName[MAX_FILENAME_LEN];
  int  i, nContinue, nHostEntries, nHostType;

  sprintf(strIniFileName, "%s/%s", NTCSS_INIT_DATA_DIR, NTCSS_PROGRP_INI_FILE);

  /* Get the hosts section headers, HOST_1=HOST_1<NULL>HOST_2=HOST_2...<NULL><NULL> */
  if (GetProfileSection("HOSTS",  strHostIniEntries,
                        MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + 2),
                        strIniFileName) < 0)
    {
      syslog(LOG_WARNING, "prime_db: GetProfileSection failed, could not "
             "determine host type");
      return(-1);
    }
  
  pCh = strHostIniEntries;
  nHostEntries = 0;

  /* If no host entries returned do not go through loop */
  if (*pCh == '\0')
    {
      nContinue = 0;
    }
  else
    {
      nContinue = 1;
    }
  
  /* Count the number of HOST_ section entries and host entry
     title sections to strHostEntrySectionHeaders array */
  while (nContinue)
    {
    /* Each entry is in the form HOST_1=HOST_1, so we parse out
       each section header based on the =. */
      parseWord(0, pCh, strHostEntrySectionHeaders[nHostEntries], "=");
      nHostEntries++;
      pCh += strlen(pCh) + 1;

      /* Stop on consecutive null condition */
      if (*pCh == '\0')
        {
          nContinue = 0;
        }
    } /* END while */
  
  nHostType = -1;
  for (i = 0; i < nHostEntries; i++)
    {
      if (GetProfileString(strHostEntrySectionHeaders[i], "NAME", "noname",
                           strEntryHostName, MAX_HOST_NAME_LEN,
                           strIniFileName) < 0)
        {
          syslog(LOG_WARNING, "prime_db: GetProfileString failed, could "
                 "not determine host type");
          break;
        }
    
      if (strcmp(strEntryHostName, strHostName) != 0)
        {
          continue;
        }
    
      if (GetProfileString(strHostEntrySectionHeaders[i], "TYPE", "notype",
                           strEntryHostType, 200, strIniFileName) < 0)
        {
          syslog(LOG_WARNING, "prime_db: GetProfileString failed, could "
                 "not determine host type");
          break;
        }
    
      if (strcmp(strEntryHostType, "Master Server") == 0)
        {
          nHostType = NTCSS_MASTER;
        }
    
      if (strcmp(strEntryHostType, "Authentication Server") == 0)
        {
          nHostType = NTCSS_AUTH_SERVER;
        }
    
      if (strcmp(strEntryHostType, "Application Server") == 0)
        {
          nHostType = NTCSS_APP_SERVER;
        }

    /* If we reach this point then we have found the server entry 
       we were looking for, and we don't need to continue looping
       through the remainder of the entries */
      break;
    } /* END for */
  
  return(nHostType);

}

/*
  getMasterTypeFromInitData....
*/
int  getMasterNameFromInitData(strHostName)
     char  *strHostName;
{
  char strEntryHostType[200], strEntryHostName[200];
  char strHostIniEntries[MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + 2)];
  char strIniFileName[MAX_FILENAME_LEN];
  char *pch;

  sprintf(strIniFileName, "%s/%s", NTCSS_INIT_DATA_DIR, NTCSS_PROGRP_INI_FILE);

  /* Get the hosts section headers, HOST_1=HOST_1<NULL>HOST_2=HOST_2...<NULL><NULL> */
  if (GetSectionValues("HOSTS",  strHostIniEntries,
                        MAX_NUM_HOSTS * (MAX_HOST_NAME_LEN + 2),
                        strIniFileName) < 0)
    {
      syslog(LOG_WARNING, "prime_db: GetSectionValues failed, could not "
             "determine host type");
      return(-1);
    }
  
  pch=strHostIniEntries;

  while (strlen(pch)) {
      GetProfileString(pch, "TYPE", "noname", 
		strEntryHostType, MAX_HOST_NAME_LEN, strIniFileName); 
    
      /* if this is the master, grab the name */
      if (!strcmp(strEntryHostType, "Master Server")) {
    
      		GetProfileString(pch, "NAME", "NOHOST",
                           strEntryHostName, 200, strIniFileName);
		if (!strcmp(strEntryHostName,"NOHOST")) {
          		syslog(LOG_WARNING, "getMasterFromInit: GetProfileString failed, could not determine host name");
          		return(-1);
        	}
		strcpy(strHostName,strEntryHostName);
        }
	pch +=  strlen(pch) + 1;
  } /* end while */
  
  return(0);

}

/*****************************************************************************/

int  main(argc, argv)

     int   argc;
     char  **argv;

{
  char *cwd;                       /* current working directory buffer */
  char  datafile[MAX_FILENAME_LEN];
  char  query[MAX_FILENAME_LEN];
  char  db_dir[MAX_FILENAME_LEN];
  char  dir[MAX_FILENAME_LEN], strHostName[MAX_HOST_NAME_LEN];
  char  msg[MAX_ERR_MSG_LEN];          /* four lines worth of error message */
  char  path_name[MAX_PATH_LEN + 1];   /* current working dir name */
  char  sql_file[MAX_FILENAME_LEN];
  int  i, nHostType;
  int  group_file_index = -1;
  int  x;                              /* returned values of function calls */
  FILE  *fp;
  int nCanRun=0;
  int no_ensure=0,retcode;

  if (SystemSettingsAreInitialized() == FALSE)
    {
      nResponsibleForSystemSettings = 1;
      printf("Loading System Settings\n");
      if (loadNtcssSystemSettingsIntoShm() != 0)
        {
          syslog(LOG_WARNING, "prime_db: Failed to load NTCSS shared memory!");
          printf("loadNtcssSystemSettingsIntoShm failed.  NTCSS II prime "
                 "stopped\n");
          exit(1);
        }
    }

  /* sprintf(db_dir, "%s/database", NTCSS_HOME_DIR); */
  sprintf(db_dir, "%s", NTCSS_DB_DIR);

/* Check to make sure we're not running prime_db from the database directory */
  cwd = getcwd(path_name, MAX_PATH_LEN + 1);
  if (cwd == NULL)
    {
      print_out("Could not determine the current working directory!\n"
                "  EXITING.");
      cleanupSystemSettings();
      exit(2);
    }
  /* if(strstr(path_name, "NTCSSII/database") != NULL) */
  if(strstr(path_name, NTCSS_DB_DIR) != NULL)
    {
      print_out("You cannot run prime_db from the database"
                " directory!\n  EXITING.");
      cleanupSystemSettings();
      exit(3);
    }

  openlog("NTCSS_INITDB", LOG_PID, LOG_LOCAL7);
  
  add_unix_batch_users = 1;
  
  i = 1;
  while (i < argc)
    {

/*
 * Determine command-line action.
 */
      if (strcmp(argv[i], "-network") == 0)
          net_install = 1;
      else
        if (strcmp(argv[i], "-g") == 0) 
          {
            i++;
            if (i < argc)
              group_file_index = i;
          }
        else
          if (strcmp(argv[i], "-nounixbatch") == 0) 
              add_unix_batch_users = 0;
          else
            {
		if (strcmp(argv[i], "-SSCCinriPRIME")) {
              		printf("USAGE: prime_db {-network} {-g <group file>} "
                     		"{-nounixbatch}\n");
	      		cleanupSystemSettings();
              		exit(4);
		} else
 			nCanRun=1;
            }
      i++;
    }

    if (!nCanRun) {
	printf("prime_db: Cannot run prime_db from the command line!\n");
        fflush(stdout);
	cleanupSystemSettings();
        exit(5);
    }
/*
 * Open local logging file.
 */
  sprintf(dir, "%s/logs/prime_db.log", NTCSS_HOME_DIR);
  logp = fopen(dir, "w");
  if (logp == NULL)
    {
      sprintf(sql_file, "Database initialization halted! Cannot not"
              "open log file %s!", dir);
      print_out(sql_file);
      cleanupSystemSettings();
      exit(5);
    }

#ifdef VSUN
  print_out("Database initialization halted! A Sun host cannot be"
            " the Master Server!");
  fclose(logp);
  cleanupSystemSettings();
  exit(6);
#endif

/*
 * Check if host is the Master Server or an Auth Server.
 */

  memset(strHostName, '\0', sizeof(strHostName));
  gethostname(strHostName, MAX_HOST_NAME_LEN);
  
  /* Jan 04 1999: GWY: Commented out so that prime never tries to find host
     type anywhere except ini file */
  /* nHostType = getHostType(strHostName, db_dir); */
  /* If host type if unknown, this might be a fresh install
     on master, so check host type from init_data/progrps.ini */
  /* if (nHostType < 0) {
     nHostType = getHostTypeFromInitData(strHostName); 
  } */
  
  nHostType = getHostTypeFromInitData(strHostName); 
  if (nHostType < 0) {
    printf("prime_db: getHostType failed\n");
    syslog(LOG_WARNING, "getHostType failed");
    fclose(logp);
    cleanupSystemSettings();
    exit(7);
  }
  
  if ((nHostType != NTCSS_MASTER) && (nHostType != NTCSS_AUTH_SERVER)) {
    print_out("Database initialization halted! This host is"
                " not the Master Server!");
      fclose(logp);
      cleanupSystemSettings();
      exit(8);
  }

/*
 * Check if any servers are still running.
 */
  if (are_any_servers_running() == 2)
    {
      print_out("Database initialization halted! NTCSS daemons"
                " are not idled!");
      fclose(logp);
      cleanupSystemSettings();
      exit(9);
    }
  print_out("Database initialization started.");

/*
 * Convert database files.
 */
  sprintf(msg, "Checking to see if database files need to be upgraded to"
          " version %s", version_id);
  print_out(msg);
  retcode=convert_database_files(db_dir, version_id);
  if (retcode == 0)
    /* the current version_id is hard-coded in inri_version.h */
    {
      print_out("Database initialization halted! Conversion of "
                "database files has failed!");
      fclose(logp);
      cleanupSystemSettings();
      exit(10);
    };
  if (retcode == 2)
     no_ensure = 1;

/*
 * Register new apps (if neccessary).
 */
  sprintf(sql_file, "%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR);
  sprintf(dir, "%s/progrps.ini", NTCSS_INIT_DATA_DIR);
  i = are_there_newly_registered_apps(sql_file);
  if (i < 0)
    {
      print_out("Database initialization halted! Cannot determine if"
                " there are new applications awaiting registration.");
      fclose(logp);
      cleanupSystemSettings();
      exit(11);
    } 
  if (i > 0)
    {
      sprintf(dir, "%s/progrps.ini", NTCSS_INIT_DATA_DIR);
      if (!process_newly_registered_apps(sql_file, dir,
                                         "host_info", "app_info"))
        {
          print_out("Database initialization halted! Cannot register "
                    "new applications!");
          fclose(logp);
	  cleanupSystemSettings();
          exit(12);
        }
    }

/*
 * Make host changes.
 */
  sprintf(sql_file, "%s/host.sql", NTCSS_INIT_DATA_DIR);
  sprintf(dir, "%s/host_info", NTCSS_INIT_DATA_DIR);
  
  i = init_database(dir, sql_file, db_dir, HOST_INFO, NULL);
  if (!i)
    {
      print_out("Database initialization halted! Cannot interpret host"
                " information!");
      fclose(logp);
      cleanupSystemSettings();
      exit(13);
    }

    /* initialize the database copy files */
     if (!copy_database_files(db_dir, INIT_TO_TMP)) {
	printf("Unable to make .tmp database file copies! HALTING.\n");
        fclose(logp);
        cleanupSystemSettings();
	exit(14);
     }

/* * Make printer class changes.
 3/31/99
 Prime should no longer be independently making these changes...

  sprintf(sql_file, "%s/prtclass.sql", NTCSS_INIT_DATA_DIR);
  sprintf(dir, "%s/prt_info", NTCSS_INIT_DATA_DIR);
  if (!init_database(dir, sql_file, db_dir, PRTCLASS_INFO, NULL))
    {
      print_out("Database initialization halted! Cannot interpret"
                " printer class information!");
      fclose(logp);
      cleanupSystemSettings();
      exit(15);
    }
  if (!applyDatabaseChanges(sql_file, db_dir, DB_CHANGE_REGULAR))
    {
      sprintf(dir, "Database initialization halted! Cannot apply data"
              " in file %s to database!", sql_file);
      print_out(dir);
      fclose(logp);
      cleanupSystemSettings();
      exit(16);
    }
  (void) unlink(sql_file);

*****/

/*
 * Make application changes.
 */
  sprintf(sql_file, "%s/app.sql", NTCSS_INIT_DATA_DIR);
  sprintf(dir, "%s/app_info", NTCSS_INIT_DATA_DIR);
  if (group_file_index == -1)
    {
      sprintf(datafile, "%s/progrps.ini", NTCSS_INIT_DATA_DIR);
      if (access(datafile, R_OK) == -1)
        i = init_database(dir, sql_file, db_dir, APP_INFO, NULL);
      else
        i = init_database(dir, sql_file, db_dir, APP_INFO, datafile);
    }
  else
      i = init_database(dir, sql_file, db_dir, APP_INFO,
                        argv[group_file_index]);
  if (i == 0)
    {
      print_out("Database initialization halted! Cannot interpret"
                " application information!");
      fclose(logp);
      cleanupSystemSettings();
      exit(17);
    }
  if (applyDatabaseChanges(sql_file, db_dir, DB_CHANGE_REGULAR) == 0)
    {
      sprintf(dir, "Database initialization halted! Cannot apply data in "
              "file %s to database!", sql_file);
      print_out(dir);
      fclose(logp);
      cleanupSystemSettings();
      exit(18);
    }
  (void) unlink(sql_file);

/*
 * Merge UNIX user changes with NTCSS database.
 */
  sprintf(sql_file, "SPECIAL NO LOCK BACKDOOR%s", db_dir);
  if (getNtcssSysConfData("NTCSS UNIX PASSWD FILE", datafile, sql_file) < 0)
    strcpy(datafile, "/etc/passwd");
  (void) verifyUnixUserInfo(db_dir, datafile);

  if (nHostType == NTCSS_MASTER) {
	/* Save app admins if databases were just converted to version 3.0.0 */
  	sprintf(datafile, "%s/convertDB_to_300_flag", NTCSS_HOME_DIR);
  	fp = fopen(datafile, "r");
  	if (fp != NULL)
    	{
      		fclose(fp);
      		print_out("Saving the App Admins for the version 3.0.0 update..");
      		x = SaveAdminsFor300Convert(db_dir);
      		if (x < 0)
        	{
          	print_out("Could not save the old App Admins!  They"
                    	" will have to be updated manually.");
          	remove(datafile); /* kill this so Restore..() doesn't run.*/
        	}
    	}
  }
  
/* Run integrity checker */
  print_out("Checking Database Integrity.\n");
  if (no_ensure == 0 && applyDatabaseChanges(NULL, db_dir, DB_CHANGE_REDO) == 0)
    {
      print_out("Database initialization halted!  Failed to check "
                "database integrity!");
      fclose(logp);
      cleanupSystemSettings();
      exit(19);
    }

   /* Make appuser role changes if converting to 3.0.0 */
   sprintf(datafile, "%s/convertDB_to_300_flag", NTCSS_HOME_DIR);
   fp = fopen(datafile, "r");
   if (fp != NULL)
   {
  	fclose(fp);
  	if (nHostType == NTCSS_MASTER) {
      		print_out("Restoring App Admins for the version 3.0.0 update..");
      		x = RestoreAdminsFor300Conversion(db_dir);
      		if (x < 0)
        		print_out("Could not restore the NTCSS admins correctly!  They will have to be updated manually.");
      		else {
        		unlink(datafile);
		}
    	}
  }

/*
 * Remove the *.old database files.
 */
  if (applyDatabaseChanges(NULL, db_dir, DB_REMOVE_OLD) == 0)
    {
      print_out("Database initialization halted! Cannot remove *.old database"
                " files.");
      fclose(logp);
      cleanupSystemSettings();
      exit(20);
    }

/*
 * Check integrity of message bulletin board data.
 */
  print_out("Checking message bulletin boards data...");
  sprintf(sql_file, "%s/message", NTCSS_HOME_DIR);
  if (applyDatabaseChanges(sql_file, db_dir, DB_CHANGE_BB_REDO) == 0)
    {
      print_out("Database initialization halted! Cannot modify message"
                " bulletin board data correctly.");
      fclose(logp);
      cleanupSystemSettings();
      exit(21);
    }

  print_out("Database initialization completed successfully.");

  sprintf(query, "%s/%s/PG.query", NTCSS_HOME_DIR, NTCSS_LOGS_DIR);
  GetProgGroupList(query);

/* Flush the INI file */
  if (group_file_index == -1)
    {
      /*      sprintf(datafile, "%s/progrps.ini", NTCSS_INIT_DATA_DIR);*/
      LoadINIFile(NTCSS_PROGRP_ID_FILE);
      FlushINIFile(NTCSS_PROGRP_ID_FILE);
    }
  else
    {
      LoadINIFile(argv[group_file_index]);
      FlushINIFile(argv[group_file_index]);
    }
  
  /* Copy the progrps.ini file to the DB directory on success */
  sprintf(path_name,"%s %s %s",CP_PROG, NTCSS_PROGRP_ID_FILE,
          NTCSS_PROGRP_DB_FILE);
  system(path_name);

  fclose(logp);
  cleanupSystemSettings();
  
  exit(0);
}
