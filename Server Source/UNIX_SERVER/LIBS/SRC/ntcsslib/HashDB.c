
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Hashdb.c
 */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>


/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <DbFcts.h>
#include <LibNtcssFcts.h>
#include <ExtraProtos.h>



/** LOCAL INCLUDES **/

#include "TextDBP.h"


/** STATIC FUNCTION PROTOTYPES **/

static int  includeProcess(ServerProcessItem*, int, ProcessFilterData*,
                           const char*, const char*, int);

/** STATIC VARIABLES **/
/** GLOBAL VARIABLES **/


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
int  initLocalDBFiles( const char  *db_dir )

{
  char   filename[MAX_FILENAME_LEN];
  char   file[MAX_FILENAME_LEN];
  char  *s;
  char   buf[(MAX_PRT_REQUEST_LEN > MAX_SERVER_PROCESS_LEN) ? MAX_PRT_REQUEST_LEN : MAX_SERVER_PROCESS_LEN];
  char   process_id[(MAX_REQUEST_ID_LEN > MAX_PROCESS_ID_LEN) ? MAX_REQUEST_ID_LEN + 1 : MAX_PROCESS_ID_LEN + 1];
  char   str[32];
  char   pidstr[32];
  DB    *db;
  int    fd, leng, lengd, pid, pidleng, newpidleng;
  char   str_NO_SUCH_JOB[] = "NO_SUCH_JOB";

  sprintf(filename, "%s/%s%s", db_dir, PRT_REQUESTS_TABLE, ".dat");
  sprintf(file, "%s/%s%s", db_dir, PRT_REQUESTS_TABLE, ".idx");
  if ((access(filename, R_OK) == -1) ||
      (access(file, R_OK) == -1)) {
    sprintf(filename, "%s/%s", db_dir, PRT_REQUESTS_TABLE);
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
    dbClose(db);
  } else {
    sprintf(file, "%s/%s%s", db_dir, PRT_REQUESTS_TABLE, ".bin");
    sprintf(filename, "%s/%s", db_dir, PRT_REQUESTS_TABLE);
    if ((db = dbOpen(filename, O_RDWR, 
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
    if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == -1) {
      dbClose(db);
      return(0);
    }
    dbRewind(db);
    while ((s = dbNextrec(db, NULL)) != NULL) {
      if (unbundleData(s, "C", process_id) == -1) {
        dbClose(db);
        close(fd);
        return(0);
      }
      leng = strlen(process_id);
      lengd = strlen(s);
      sprintf(str, "%5.5d%5.5d", leng, lengd);
      if (((int) write(fd, (void *) &str[0],
                       (size_t) 10)) < 10) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (((int) write(fd, (void *) &process_id[0],
                       (size_t) leng)) < leng) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (((int) write(fd, (void *) &s[0], (size_t) lengd)) < lengd) {
        dbClose(db);
        close(fd);
        return(0);
      }
    }
    dbClose(db);
    if (((int) lseek(fd, (off_t) 0, SEEK_SET)) < 0) {
      close(fd);
      return(0);
    }
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL) {
      close(fd);
      return(0);
    }
    while ((leng = (int) read(fd, str, (size_t) 10)) == 10) {
      if (leng < 0)
        break;
      lengd = atoi(&str[5]);
      str[5] = 0;
      leng = atoi(str);
      if ((pid = (int) read(fd, process_id, (size_t) leng)) != leng) {
        leng = -1;
        break;
      }
      process_id[leng] = 0;
      if ((pid = (int) read(fd, buf, (size_t) lengd)) != lengd) {
        leng = -1;
        break;
      }
      buf[lengd] = 0;
      if (dbStore(db, process_id, buf, DB_INSERT) != 0) {
        leng = -1;
        break;
      }
    }
    dbClose(db);
    close(fd);
    if (leng < 0)
      return(0);
  }
  sprintf(file, "%s/%s%s", db_dir, PRT_REQUESTS_TABLE, ".dat");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  sprintf(file, "%s/%s%s", db_dir, PRT_REQUESTS_TABLE, ".idx");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  
  sprintf(filename, "%s/%s%s", db_dir, ACTIVE_PROCS_TABLE, ".dat");
  sprintf(file, "%s/%s%s", db_dir, ACTIVE_PROCS_TABLE, ".idx");
  if ((access(filename, R_OK) == -1) ||
      (access(file, R_OK) == -1)) {
    sprintf(filename, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
  } else {
    sprintf(file, "%s/%s%s", db_dir, ACTIVE_PROCS_TABLE, ".bin");
    sprintf(filename, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
    if ((db = dbOpen(filename, O_RDWR, 
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
    if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == -1) {
      dbClose(db);
      return(0);
    }
    dbRewind(db);
    while ((s = dbNextrec(db, NULL)) != NULL) {
      if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
          (strncmp(s, "JOB", strlen("JOB")) == 0)) {
        continue;
      }
      if (unbundleData(s, "IC", &pid, process_id) == -1) {
        dbClose(db);
        close(fd);
        return(0);
      }
      leng = strlen(process_id);
      if (pid > 0) {
        sprintf(pidstr, "%d", pid);
        pidleng = strlen(pidstr);
        sprintf(str, "%d", CRASHED_STATE);
        newpidleng = strlen(str);
        lengd = strlen(s) - pidleng + newpidleng;
        sprintf(pidstr, "%d%d", newpidleng, CRASHED_STATE);
        pidleng++;
        newpidleng++;
      } else
        lengd = strlen(s);
      sprintf(str, "%5.5d%5.5d", leng, lengd);
      if (((int) write(fd, (void *) &str[0],
                       (size_t) 10)) < 10) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (((int) write(fd, (void *) &process_id[0],
                       (size_t) leng)) < leng) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (pid > 0) {
        if (((int) write(fd, (void *) &pidstr[0],
                         (size_t) newpidleng)) < newpidleng) {
          dbClose(db);
          close(fd);
          return(0);
        }
        lengd = strlen(s) - pidleng;
        if (((int) write(fd, (void *) &s[pidleng],
                         (size_t) lengd)) < lengd) {
          dbClose(db);
          close(fd);
          return(0);
        }
      } else {
        if (((int) write(fd, (void *) &s[0],
                         (size_t) lengd)) < lengd) {
          dbClose(db);
          close(fd);
          return(0);
        }
      }
    }
    dbClose(db);
    if (((int) lseek(fd, (off_t) 0, SEEK_SET)) < 0) {
      close(fd);
      return(0);
    }
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL) {
      close(fd);
      return(0);
    }
    while ((leng = (int) read(fd, str, (size_t) 10)) == 10) {
      if (leng < 0)
        break;
      lengd = atoi(&str[5]);
      str[5] = 0;
      leng = atoi(str);
      if ((pid = (int) read(fd, process_id, (size_t) leng)) != leng) {
        leng = -1;
        break;
      }
      process_id[leng] = 0;
      if ((pid = (int) read(fd, buf, (size_t) lengd)) != lengd) {
        leng = -1;
        break;
      }
      buf[lengd] = 0;
      if (dbStore(db, process_id, buf, DB_INSERT) != 0) {
        leng = -1;
        break;
      }
    }
    close(fd);
    if (leng < 0)
      return(0);
  }
/*
 * Insert "run next job" record.
 */
  if (dbStore(db, "RUN_NEXT_JOB", str_NO_SUCH_JOB, DB_INSERT) != 0) {
    dbClose(db);
    return(0);
  }
  dbClose(db);
  sprintf(file, "%s/%s%s", db_dir, ACTIVE_PROCS_TABLE, ".dat");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  sprintf(file, "%s/%s%s", db_dir, ACTIVE_PROCS_TABLE, ".idx");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  sprintf(filename, "%s/%s%s", db_dir, SCHEDULED_PROCS_TABLE, ".dat");
  sprintf(file, "%s/%s%s", db_dir, SCHEDULED_PROCS_TABLE, ".idx");
  if ((access(filename, R_OK) == -1) ||
      (access(file, R_OK) == -1)) {
    sprintf(filename, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
    dbClose(db);
  } else {
    sprintf(file, "%s/%s%s", db_dir, SCHEDULED_PROCS_TABLE, ".bin");
    sprintf(filename, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
    if ((db = dbOpen(filename, O_RDWR, 
                     S_IRUSR | S_IWUSR |
                     S_IRGRP | S_IROTH)) == NULL)
      return(0);
    if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == -1) {
      dbClose(db);
      return(0);
    }
    dbRewind(db);
    while ((s = dbNextrec(db, NULL)) != NULL) {
      if (unbundleData(s, "IC", &pid, process_id) == -1) {
        dbClose(db);
        close(fd);
        return(0);
      }
      leng = strlen(process_id);
      lengd = strlen(s);
      sprintf(str, "%5.5d%5.5d", leng, lengd);
      if (((int) write(fd, (void *) &str[0],
                       (size_t) 10)) < 10) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (((int) write(fd, (void *) &process_id[0],
                       (size_t) leng)) < leng) {
        dbClose(db);
        close(fd);
        return(0);
      }
      if (((int) write(fd, (void *) &s[0], (size_t) lengd)) < lengd) {
        dbClose(db);
        close(fd);
        return(0);
      }
    }
    dbClose(db);
    if (((int) lseek(fd, (off_t) 0, SEEK_SET)) < 0) {
      close(fd);
      return(0);
    }
    if ((db = dbOpen(filename, O_RDWR | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR)) == NULL) {
      close(fd);
      return(0);
    }
    while ((leng = (int) read(fd, str, (size_t) 10)) == 10) {
      if (leng < 0)
        break;
      lengd = atoi(&str[5]);
      str[5] = 0;
      leng = atoi(str);
      if ((pid = (int) read(fd, process_id, (size_t) leng)) != leng) {
        leng = -1;
        break;
      }
      process_id[leng] = 0;
      if ((pid = (int) read(fd, buf, (size_t) lengd)) != lengd) {
        leng = -1;
        break;
      }
      buf[lengd] = 0;
      if (dbStore(db, process_id, buf, DB_INSERT) != 0) {
        leng = -1;
        break;
      }
    }
    dbClose(db);
    close(fd);
    if (leng < 0)
      return(0);
  }
  sprintf(file, "%s/%s%s", db_dir, SCHEDULED_PROCS_TABLE, ".dat");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  sprintf(file, "%s/%s%s", db_dir, SCHEDULED_PROCS_TABLE, ".idx");
  (void) chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  
  return(1);
}


/*****************************************************************************/

int  addPrtRequest(db_dir, prt_request)

     const char  *db_dir;
     PrtRequestItem  *prt_request;
{

  char file[MAX_FILENAME_LEN], buf[MAX_PRT_REQUEST_LEN];
  DB *db;
  int retval;

  if ((retval = checkPrtRequestSize(db_dir, prt_request)) != 0)
    return(retval);
/*
 * Suspend print request.
 */
  sprintf(file, "%s/%s", db_dir, PRT_REQUESTS_TABLE);
  if ((db = dbOpen(file, O_RDWR,
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH )) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);
  
  buf[0] = 0;
  (void) bundleData(buf, "NNNNNDIINI", prt_request->request_id,
                    prt_request->progroup_title,
                    prt_request->prt_file,
                    prt_request->login_name,
                    prt_request->prt_name,
                    prt_request->time_of_request,
                    prt_request->security_class,
                    prt_request->size,
                    prt_request->orig_host,
                    prt_request->status);

  if (dbStore(db, prt_request->request_id, buf, DB_INSERT) != 0) {
    dbClose(db);
    return(ERROR_CANT_INSERT_VALUE);
  }
  dbClose(db);
  
  return(0);
}


/*****************************************************************************/

int  delPrtRequest( const char  *db_dir,
                    const char  *request_id,
                    char        *login_name,
                    char        *appname )

{

  char file[MAX_FILENAME_LEN], req_id[MAX_REQUEST_ID_LEN+1];
  char prt_file[MAX_PRT_FILE_LEN+1], *s;
  DB *db;

/*
 * Delete print request.
 */
  sprintf(file, "%s/%s", db_dir, PRT_REQUESTS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);
  
  login_name[0] = 0;
  appname[0] = 0;
  if ((s = dbFetch(db, request_id)) != NULL) {
    (void) unbundleData(s, "CCCC", req_id, appname, prt_file, login_name);
  }

  if (dbDelete(db, request_id) != 0) {
    dbClose(db);
    return(ERROR_CANT_DELETE_VALUE);
  }
  dbClose(db);
  
  return(0);
}


/*****************************************************************************/

int  checkPrtRequestSize( const char      *db_dir,
                          PrtRequestItem  *prt_request )

{
  char  host[MAX_ITEM_LEN];
  int   i, lock_id, num_prts, prt_id;
  PrtItem  **Prts;

  if (gethostname(host, MAX_ITEM_LEN) != 0)
    return(ERROR_CANT_DETERMINE_HOST);
  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    return(ERROR_CANT_ACCESS_DB);
  
  Prts = getPrtData(&num_prts, db_dir, (AppItem **) 0, 0);
  unsetReadDBLock(lock_id);

  prt_id = -1;
  for (i = 0; i < num_prts; i++) {
    if (strcmp(prt_request->prt_name, Prts[i]->prt_name) == 0) {
      prt_id = i;
      break;
    }
  }
  if (prt_id == -1) {
    freePrtData(Prts, num_prts);
    return(ERROR_NO_SUCH_PRT);
  }
/*
 * Check if queueing disabled.
 */
  if (!isQueueEnabled(Prts[prt_id]->status)) {
    freePrtData(Prts, num_prts);
    return(ERROR_QUEUE_DISABLED);
  }
/*
 * Check if print request should be suspended.
 */
  if (Prts[prt_id]->suspend_flag ||
      prt_request->size <= Prts[prt_id]->max_size) {
    freePrtData(Prts, num_prts);
    return(ERROR_NO_NEED_TO_SUSPEND);
  }
  freePrtData(Prts, num_prts);
  
  return(0);
}


/*****************************************************************************/
/* For the role, 0 is NTCSS User, 1 is App Admin, 2 is NTCSS Admin.
*/

int  getPrtRequestQueueList( const char  *prt,
                             const char  *login_name,
                             const char  *app,
                             int          role, 
                             const char  *query,
                             char        *msg,
                             const char  *db_dir)

{
  const char  *fnct_name = "getPrtRequestQueueList():";
  char   buf[4];
  char   cfile[MAX_PRT_FILE_LEN + 1];
  FILE  *cfp;
  DB    *db;
  char   file[MAX_FILENAME_LEN];
  FILE  *fp;
  int    leng;
  char   l_name[MAX_LOGIN_NAME_LEN + 1];
  int    num_prt_reqs = 0;
  char   orig_host[MAX_HOST_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   prt_file[MAX_PRT_FILE_LEN + 1];
  int    prt_len;
  char   prt_name[MAX_PRT_NAME_LEN + 1];
  int    prt_security;
  int    prt_status;
  char   req_time[DATETIME_LEN + 1];
  char   request_id[MAX_REQUEST_ID_LEN + 1];
  char   target_file[MAX_PRT_FILE_LEN + 1];
  char  *s;
  char   snew[4000];

  fp = fopen(query, "w");
  if (fp == NULL)
    return(ERROR_CANT_CREATE_QUERY);

  sprintf(file, "%s/%s", db_dir, PRT_REQUESTS_TABLE);
  db = dbOpen(file, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (db == NULL)
    {
      fclose(fp);
      syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
             fnct_name, file);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  dbRewind(db);
  leng = 0;
  s = dbNextrec(db, NULL);
  while (s != NULL)
    {
      if (unbundleData(s, "CCCCCDIICI", request_id, progroup_title,
                       prt_file, l_name, prt_name,
                       req_time, &prt_security, &prt_len,
                       orig_host, &prt_status) == -1)
        {
          dbClose(db);
          fclose(fp);
          syslog(LOG_WARNING, "%s Failed to read record from db file %s!",
                 fnct_name, file);
          return(ERROR_CANT_READ_VALUE);
        }

      if (strcmp(prt_name, prt) != 0)
        continue;

      sprintf(cfile, "%s/%s/%s/cfA%s", NTCSS_HOME_DIR,
              NTCSS_LP_SPOOL_DEFAULT_DIR, prt_name, request_id);
      cfp = fopen(cfile, "r");
      if (cfp)
        {
          while (!feof(cfp))
            {
              memset(target_file, '\0', MAX_PRT_FILE_LEN + 1);
              fgets(target_file, MAX_PRT_FILE_LEN, cfp);
              if (target_file[0] == 'N')
                {
                  break;
                }
            }
          fclose(cfp);
        }

      switch (role)
        {
        case 0:
          if ((strcmp(login_name, l_name) == 0) &&
              (strcmp(app, progroup_title) == 0))
            {
              memset(snew, '\0', 4000);
              bundleData(snew, "CCNCCDIICI", request_id, progroup_title,
                         &target_file[1], l_name, prt_name,
                         req_time, prt_security, prt_len,
                         orig_host, prt_status);
              fprintf(fp, "%s", snew);
              leng += strlen(snew);
              num_prt_reqs++;
            }
          break;

        case 1:
          if (strcmp(app, progroup_title) == 0)
            {
              memset(snew, '\0', 4000);
              bundleData(snew, "CCNCCDIICI", request_id, progroup_title,
                         &target_file[1], l_name, prt_name,
                         req_time, prt_security, prt_len,
                         orig_host, prt_status);
              fprintf(fp, "%s", snew);
              leng += strlen(snew);
              num_prt_reqs++;
            }
          break;

        case 2:
          memset(snew, '\0', 4000);
          bundleData(snew, "CCNCCDIICI", request_id, progroup_title,
                     &target_file[1], l_name, prt_name,
                     req_time, prt_security, prt_len,
                     orig_host,prt_status);
          fprintf(fp, "%s", snew);
          leng += strlen(snew);
          num_prt_reqs++;
          break;
        }
    }
  dbClose(db);
  fclose(fp);

  sprintf(file, "%d", num_prt_reqs);
  sprintf(buf, "%1d", strlen(file));
  strcat(msg, buf);
  strcat(msg, file);
    
  return(leng);   /* everthing ok, printer queue data is returned */
}


/*****************************************************************************/

int  getNextServerProcess( const char  *db_dir,
                           char        *app,
                           char        *process_id,
                           int          app_limit,
                           int          host_limit,
                           int          app_id )

{

  ServerProcessItem  server_proc;
  long   current_time;
  char   current_time_str[DATETIME_LEN];
  DB    *db;
  long   delta_time;
  long   delta1_time;
  char   file[MAX_FILENAME_LEN];
  const char  *fnct_name = "getNextServerProcess():";
  int    highest_priority = -10;
  int    lock_id;
  long   next_time;
  char   oldest_datetime[DATETIME_LEN];
  long   prev_time;
  char  *s;

  if (!okLaunchServerProcess(db_dir, app_limit, host_limit, app))
    return(0);

  sprintf(file, "%s/../logs/%s.%d", db_dir, CHECK_APP_LOCK_FILE, app_id);
  if (!setFileLock(file, &lock_id, LOCK_WAIT_TIME)) {
    syslog(LOG_WARNING, "%s Failed to obtain lock for checking the"
           " process queue of %s!", fnct_name, app);
    return(0);
  }

  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL) {
    unsetFileLock(lock_id);
    syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
           fnct_name, file);
    return(0);
  }
/*
 * Check if there is a "run next job" record.
 */
  if ((s = dbFetch(db, "RUN_NEXT_JOB")) != NULL) {
    if ((strcmp(s, "NO_SUCH_JOB") != 0) &&
        (strncmp(s, "JOB", 3) == 0)) {
      strcpy(process_id, &s[3]);
      if (((s = dbFetch(db, process_id)) == NULL) ||
          (unbundleData(s, "ICIC", &server_proc.pid,
                        server_proc.process_id,
                        &server_proc.priority,
                        server_proc.progroup_title) == -1)) {
        syslog(LOG_WARNING, "%s Failed to read record from "
               "database file %s!", fnct_name, file);
      } else if ((strcmp(server_proc.progroup_title, app) == 0) &&
                 (server_proc.pid == RUN_NEXT_STATE)) {
        dbClose(db);
        goto change_server_process;
      }
    }
  }

  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICICCICCCCCC", &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title,
                     server_proc.command_line,
                     &server_proc.proc_status,
                     server_proc.cust_proc_status,
                     server_proc.login_name,
                     server_proc.unix_owner,
                     server_proc.job_descrip,
                     server_proc.request_time,
                     server_proc.launch_time) == -1) {
      dbClose(db);
      unsetFileLock(lock_id);
      syslog(LOG_WARNING, "%s Failed to read record from database file %s!",
             fnct_name, file);
      return(0);
    }

    if ((strcmp(server_proc.progroup_title, app) != 0) ||
        (server_proc.pid != WAITING_FOR_EXEC_STATE))
      continue;

    if (highest_priority == -10) {
      strcpy(oldest_datetime, server_proc.request_time);
      highest_priority = server_proc.priority;
      strcpy(process_id, server_proc.process_id);
      if (isValidFullTimeStr(server_proc.launch_time))
        strcpy(oldest_datetime, server_proc.launch_time);
    } else {
      prev_time=FullTimeStrToSecs(oldest_datetime);
      next_time=FullTimeStrToSecs(server_proc.request_time);
      getFullTimeStr(current_time_str);
      current_time=FullTimeStrToSecs(current_time_str);
      
      delta_time = current_time - next_time;
      delta1_time = current_time - prev_time;
      
      if (delta_time > SERVER_PROCESS_MAX_WAIT) {
        if (delta_time > delta1_time) {
          highest_priority = server_proc.priority;
          strcpy(oldest_datetime, 
                 server_proc.launch_time);
          strcpy(process_id, 
                 server_proc.process_id);
        }
      }  else {
        if (server_proc.priority > highest_priority &&
            delta1_time <= SERVER_PROCESS_MAX_WAIT) { 
          highest_priority = server_proc.priority;
          strcpy(oldest_datetime, 
                 server_proc.launch_time);
          strcpy(process_id, 
                 server_proc.process_id);
        } else if (server_proc.priority == highest_priority && delta_time > delta1_time) {
          highest_priority = server_proc.priority;
          strcpy(oldest_datetime, 
                 server_proc.launch_time);
          strcpy(process_id, 
                 server_proc.process_id);
        }
      }
    }
  }
  dbClose(db);
    
  if (highest_priority == -10) {
    unsetFileLock(lock_id);
    return(0);
  }

 change_server_process:
  if (changeServerProcess(db_dir, process_id, ABOUT_TO_BE_RUN_STATE,
                          CHANGE_PID_VALUE, -1, -1, NULL, NULL, NULL,
                          NULL, app, 0, LOW_PRIORITY) < 0) {
    syslog(LOG_WARNING, "%s Failed to change server process %s!",
           fnct_name, process_id);
    unsetFileLock(lock_id);
    return(0);
  }

  unsetFileLock(lock_id);

  return(1);
}


/*****************************************************************************/

int  okLaunchServerProcess( const char  *db_dir,
                            int          app_limit,
                            int          host_limit,
                            const char  *app )
{

  DB    *db;
  char   file[MAX_FILENAME_LEN];
  const char  *fnct_name = "getNextServerProcess():";
  char  *s;
  int    num_app_procs = 0;
  int    num_host_procs = 0;
  ServerProcessItem  server_proc;

  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL) {
    syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
           fnct_name, file);
    return(0);
  }
  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICIC", &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title) == -1) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to read record from database file %s!",
             fnct_name, file);
      return(0);
    }

    if ((server_proc.pid > 0) ||
        (server_proc.pid == ABOUT_TO_BE_RUN_STATE)) {
      num_host_procs++;
      if (strcmp(server_proc.progroup_title, app) == 0)
        num_app_procs++;
    }
  }
  dbClose(db);

  if ((num_host_procs >= host_limit) || (num_app_procs >= app_limit))
    return(0);

  return(1);
}


/*****************************************************************************/

char  *getServerProcessApps( const char  *db_dir )

{

  char   appname[MAX_PROGROUP_TITLE_LEN + 1];
  DB    *db;
  char   file[MAX_FILENAME_LEN];
  int    flag;
  const char  *fnct_name = "getServerProcessApps():";
  int    i;
  int    n;
  char  *retlist;
  char  *s;
  ServerProcessItem  server_proc;

  if ((retlist = (char *) malloc(sizeof(char)*5)) == NULL) {
    syslog(LOG_WARNING, "%s Failed to allocate space for server"
           " process app list!", fnct_name);
    return(NULL);
  }
  n = 5;
  retlist[0] = 0;
  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL) {
    syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
           fnct_name, file);
    return(NULL);
  }
  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICIC", &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title) == -1) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to read record from database file %s!",
             fnct_name, file);
      return(NULL);
    }
/*
 * Do not get apps for running processes.
 */
    if (server_proc.pid > 0)
      continue;
/*
 * Check for duplication.
 */
    i = flag = 0;
    while ((i = stripWord(i, retlist, appname)) != -1) {
      if (strcmp(appname, server_proc.progroup_title) == 0)
        flag = 1;
    }
    if (flag)
      continue;
    n += strlen(server_proc.progroup_title) + 1;
    if ((retlist = (char *) reallocateString(retlist,
                                             sizeof(char)*n)) == NULL) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to allocate space for server"
             " process app list!", fnct_name);
      return(NULL);
    }
    strcat(retlist, server_proc.progroup_title);
    strcat(retlist, " ");
  }
  dbClose(db);
  
  return(retlist);
}


/*****************************************************************************/

int  getScheduledServerProcs( const char  *db_dir,
                              const char  *datetime,
                              const char  *query )
{

  int    i, j;
  int    is_excepted;
  int    is_onetime;
  int    leng;
  int    num_jobs = 0;
  int    num_sched_items;
  int    ofst;
  int    run_job;
  char   buf[MAX_SERVER_PROCESS_LEN];
  char   cust_status[MAX_CUST_PROC_STAT_LEN+1];
  char   file[MAX_FILENAME_LEN];
  const char  *fnct_name = "getScheduledServerProcs():";
  char  *s;
  char   str[MAX_FILENAME_LEN];
  FILE  *fp;
  DB    *db;
  ServerProcessItem  server_proc;

  if ((fp = fopen(query, "w")) == NULL) {
    syslog(LOG_WARNING, "%s Failed to open file %s!", fnct_name, query);
    return(ERROR_CANT_CREATE_QUERY);
  }
  fclose(fp);

  sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL) {
    syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
           fnct_name, file);
    return(ERROR_CANT_OPEN_DB_FILE);    
  }

  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if (unbundleData(s, "ICICCIC", &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title,
                     server_proc.command_line,
                     &server_proc.proc_status,
                     server_proc.cust_proc_status) == -1) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to read record from database file %s!",
             fnct_name, file);
      return(ERROR_CANT_READ_VALUE);
    }
    
    if (server_proc.pid != APPROVED_STATE)
      continue;

    leng = strlen(server_proc.cust_proc_status);
    if ((leng % MAX_SCHED_STR_LEN) != 0)
      continue;

    num_sched_items = leng/MAX_SCHED_STR_LEN;

    cust_status[0] = 0;
    is_onetime = is_excepted = run_job = 0;
    for (i = 0; i < num_sched_items; i++) {
      ofst = i*MAX_SCHED_STR_LEN;
      switch (server_proc.cust_proc_status[ofst]) {
      case 'O':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
        /*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Check date.
 */
        if (datetime[4] != server_proc.cust_proc_status[ofst + 6])
          break;
        if (datetime[5] != server_proc.cust_proc_status[ofst + 7])
          break;
/*
 * Check month.
 */
        if (datetime[2] != server_proc.cust_proc_status[ofst + 8])
          break;
        if (datetime[3] != server_proc.cust_proc_status[ofst + 9])
          break;
/*
 * Time values match.
 */
        run_job = 1;
        is_onetime = 1;
        leng = strlen(cust_status);
        for (j = (leng - 1); j >= (leng - MAX_SCHED_STR_LEN); j--)
          cust_status[j] = 'X';
        break;


      case 'E':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (is_excepted)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Check date.
 */
        if (datetime[4] != server_proc.cust_proc_status[ofst + 6])
          break;
        if (datetime[5] != server_proc.cust_proc_status[ofst + 7])
          break;
/*
 * Check month.
 */
        if (datetime[2] != server_proc.cust_proc_status[ofst + 8])
          break;
        if (datetime[3] != server_proc.cust_proc_status[ofst + 9])
          break;
/*
 * Time values match.
 */
        is_excepted = 1;
        leng = strlen(cust_status);
        for (j = (leng - 1); j >= (leng - MAX_SCHED_STR_LEN); j--)
          cust_status[j] = 'X';
        break;


      case 'H':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (run_job)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Time values match.
 */
        run_job = 1;
        break;
        
      case 'D':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (run_job)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Time values match.
 */
        run_job = 1;
        break;
        
      case 'W':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (run_job)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Check day.
 */
        leng = getWeekDayOfTimeStr(datetime);
        str[0] = server_proc.cust_proc_status[ofst + 5];
        str[1] = 0;
        if (leng != atoi(str))
          break;
/*
 * Time values match.
 */
        run_job = 1;
        break;

      case 'M':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (run_job)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Check date.
 */
        if (datetime[4] != server_proc.cust_proc_status[ofst + 6])
          break;
        if (datetime[5] != server_proc.cust_proc_status[ofst + 7])
          break;
/*
 * Time values match.
 */
        run_job = 1;
        break;

      case 'Y':
        strncat(cust_status, &server_proc.cust_proc_status[ofst],
                MAX_SCHED_STR_LEN);
        if (run_job)
          break;
/*
 * Check minute.
 */
        if (datetime[9] != server_proc.cust_proc_status[ofst + 1])
          break;
        if (datetime[10] != server_proc.cust_proc_status[ofst + 2])
          break;
/*
 * Check hour.
 */
        if (datetime[7] != server_proc.cust_proc_status[ofst + 3])
          break;
        if (datetime[8] != server_proc.cust_proc_status[ofst + 4])
          break;
/*
 * Check date.
 */
        if (datetime[4] != server_proc.cust_proc_status[ofst + 6])
          break;
        if (datetime[5] != server_proc.cust_proc_status[ofst + 7])
          break;
/*
 * Check month.
 */
        if (datetime[2] != server_proc.cust_proc_status[ofst + 8])
          break;
        if (datetime[3] != server_proc.cust_proc_status[ofst + 9])
          break;
/*
 * Time values match.
 */
        run_job = 1;
        break;
      }
    }
    
    if (run_job && !is_excepted) {
      num_jobs++;
      if ((fp = fopen(query, "a+")) == NULL) {
        dbClose(db);
        syslog(LOG_WARNING, "%s Unable to append data to file %s!",
               fnct_name, file);
        return(ERROR_CANT_CREATE_QUERY);
      }
      fprintf(fp, "%s\n", server_proc.process_id);
      fclose(fp);
    }

    if (is_onetime || is_excepted) {
      strcpy(buf, server_proc.process_id);
      if (((s = dbFetch(db, buf)) != NULL) &&
          (unbundleData(s, "ICICCICCCCCCCICCCICICCCCIIII",
                        &server_proc.pid,
                        server_proc.process_id,
                        &server_proc.priority,
                        server_proc.progroup_title,
                        server_proc.command_line,
                        &server_proc.proc_status,
                        server_proc.cust_proc_status,
                        server_proc.login_name,
                        server_proc.unix_owner,
                        server_proc.job_descrip,
                        server_proc.request_time,
                        server_proc.launch_time,
                        server_proc.end_time,
                        &server_proc.security_class,
                        server_proc.orig_host,
                        server_proc.prt_name,
                        server_proc.hostname,
                        &server_proc.prt_sec_class,
                        server_proc.prt_filename,
                        &server_proc.prt_flag,
                        server_proc.sdi_ctl_rec,
                        server_proc.sdi_data_file,
                        server_proc.sdi_tci_file,
                        server_proc.sdi_device,
                        &server_proc.orientation,
                        &server_proc.banner,
                        &server_proc.copies,
                        &server_proc.papersize) != -1)) {
        buf[0] = 0;
        (void) bundleData(buf, "ICICCICCCCCCCICCCICICCCCIIII",
                          server_proc.pid,
                          server_proc.process_id,
                          server_proc.priority,
                          server_proc.progroup_title,
                          server_proc.command_line,
                          server_proc.proc_status,
                          cust_status,
                          server_proc.login_name,
                          server_proc.unix_owner,
                          server_proc.job_descrip,
                          server_proc.request_time,
                          server_proc.launch_time,
                          server_proc.end_time,
                          server_proc.security_class,
                          server_proc.orig_host,
                          server_proc.prt_name,
                          server_proc.hostname,
                          server_proc.prt_sec_class,
                          server_proc.prt_filename,
                          server_proc.prt_flag,
                          server_proc.sdi_ctl_rec,
                          server_proc.sdi_data_file,
                          server_proc.sdi_tci_file,
                          server_proc.sdi_device,
                          server_proc.orientation,
                          server_proc.banner,
                          server_proc.copies,
                          server_proc.papersize);
        if (dbStore(db, server_proc.process_id, buf, DB_REPLACE) != 0) {
          dbClose(db);
          syslog(LOG_WARNING, "%s Failed to replace \"one time\""
                 " or \"excepted\" record in database file %s!",
                 fnct_name, file);
          return(ERROR_CANT_REPLACE_VALUE);
        }
      } else {
        dbClose(db);
        syslog(LOG_WARNING, "%s Failed to replace \"one time\""
               " or \"excepted\" record in database file %s!",
               fnct_name, file);
        return(ERROR_CANT_REPLACE_VALUE);
      }
    }
  }
  dbClose(db);

  return(num_jobs);
}


/*****************************************************************************/

int  getServerProcessData( const char         *db_dir,
                           const char         *process_id,
                           ServerProcessItem  *server_proc,
                           int                 which )

{
  char    file[MAX_FILENAME_LEN];
  char   *s;
  DB     *db;

/*
 * Get server process data.
 */
  if (which == SCHEDULED_PRIORITY)
    sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  else
    sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);
  
  if ( ((s = dbFetch(db, process_id)) == NULL) ||
       (unbundleData(s, "ICICCICCCCCCCICCCICICCCCIIII",
                     &server_proc->pid,
                     server_proc->process_id,
                     &server_proc->priority,
                     server_proc->progroup_title,
                     server_proc->command_line,
                     &server_proc->proc_status,
                     server_proc->cust_proc_status,
                     server_proc->login_name,
                     server_proc->unix_owner,
                     server_proc->job_descrip,
                     server_proc->request_time,
                     server_proc->launch_time,
                     server_proc->end_time,
                     &server_proc->security_class,
                     server_proc->orig_host,
                     server_proc->prt_name,
                     server_proc->hostname,
                     &server_proc->prt_sec_class,
                     server_proc->prt_filename,
                     &server_proc->prt_flag,
                     server_proc->sdi_ctl_rec,
                     server_proc->sdi_data_file,
                     server_proc->sdi_tci_file,
                     server_proc->sdi_device,
                     &server_proc->orientation,
                     &server_proc->banner,
                     &server_proc->copies,
                     &server_proc->papersize) == -1))
    {
      dbClose(db);
      return(ERROR_CANT_FIND_VALUE);
    }
  dbClose(db);

  return(0);
}


/*****************************************************************************/

int  addServerProcess( const char         *db_dir,
                       ServerProcessItem  *server_proc,
                       int                 sched_launch )

{

  char file[MAX_FILENAME_LEN], buf[MAX_SERVER_PROCESS_LEN];
  DB *db;
  int flag, counter = 0;
  int retval = 0;

  if ((server_proc->priority == SCHEDULED_PRIORITY) && !sched_launch)
    sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  else
    sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR,
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);

  while (1) {
    sprintf(server_proc->process_id, "%s%s%3.3d",
            server_proc->request_time,
            server_proc->login_name,
            counter);
    buf[0] = 0;
    (void) bundleData(buf, "ICICCICCCCCCCICCCICICCCCIIII", 
                      server_proc->pid,
                      server_proc->process_id,
                      server_proc->priority,
                      server_proc->progroup_title,
                      server_proc->command_line,
                      server_proc->proc_status,
                      server_proc->cust_proc_status,
                      server_proc->login_name,
                      server_proc->unix_owner,
                      server_proc->job_descrip,
                      server_proc->request_time,
                      server_proc->launch_time,
                      server_proc->end_time,
                      server_proc->security_class,
                      server_proc->orig_host,
                      server_proc->prt_name,
                      server_proc->hostname,
                      server_proc->prt_sec_class,
                      server_proc->prt_filename,
                      server_proc->prt_flag,
                      server_proc->sdi_ctl_rec,
                      server_proc->sdi_data_file,
                      server_proc->sdi_tci_file,
                      server_proc->sdi_device,
                      server_proc->orientation,
                      server_proc->banner,
                      server_proc->copies,
                      server_proc->papersize);

    flag = dbStore(db, server_proc->process_id, buf, DB_INSERT);
    if (flag == 0) {
      retval = 0;
      break;
    } else if ((flag == 1) && (counter >= 999)) {
      retval = ERROR_CANT_INSERT_VALUE;
      break;
    } else if (flag == 1) {
      counter++;
    } else {
      retval = ERROR_CANT_INSERT_VALUE;
      break;
    }
  }
  dbClose(db);
    
  return(retval);
}


/*****************************************************************************/

int  delServerProcess( const char  *db_dir,
                       const char  *process_id,
                       const char  *app_name,
                       int          is_sched,
                       int          ignore_running )

{
  const char  *fnct_name = "delServerProcess():";
  char    file[MAX_FILENAME_LEN];
  char    proc_id[MAX_PROCESS_ID_LEN];
  char    progroup_title[MAX_PROGROUP_TITLE_LEN];
  char   *s;
  char   *s1;
  DB     *db;
  int     pid;
  int     priority;
  char    str_NO_SUCH_JOB[] = "NO_SUCH_JOB";

/*
 * Delete server process.
 */
  if (is_sched)
    sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  else
    sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);
  
  if (((s = dbFetch(db, process_id)) == NULL) ||
      (unbundleData(s, "ICIC", &pid, proc_id, &priority,
                    progroup_title) == -1)) {
    dbClose(db);
    return(ERROR_NO_SUCH_PROCESS);
  }

  if (strcmp(app_name,progroup_title) != 0) {
    dbClose(db);
    return(ERROR_REMOVE_FROM_WRONG_PROGROUP);
  }

  if (pid == RUN_NEXT_STATE) {
    if ((s1 = dbFetch(db, "RUN_NEXT_JOB")) != NULL) {
      if (strncmp(s1, "JOB",3) == 0) {
        if (dbStore(db, "RUN_NEXT_JOB", 
                    str_NO_SUCH_JOB, DB_REPLACE) != 0) 
          syslog(LOG_WARNING,"%s Failed to clear run next job record!",
                 fnct_name);
      }
    }
  }

  if (!ignore_running && (priority != SCHEDULED_PRIORITY) && (pid > 0)) {
    dbClose(db);
    return(ERROR_CANT_REMOVE_ACT_PROCESS);
  }

  if (dbDelete(db, process_id) != 0) {
    dbClose(db);
    return(ERROR_CANT_DELETE_VALUE);
  }

  dbClose(db);

  return(0);
}


/*****************************************************************************/
/* The bit_flags var holds printer and SDI flag bits.
*/

int  changeServerProcess( const char  *db_dir,
                          const char  *change_proc_id,
                          int          pid,
                          int          mask,
                          int          retval,
                          int          priority,
                          const char  *cust_stat,
                          const char  *launch_time,
                          const char  *end_time,
                          const char  *devname,
                          char        *app_name,
                          int          bit_flags,
                          int          which )

{

  ServerProcessItem  server_proc;
  DB    *db;
  char   buf[MAX_SERVER_PROCESS_LEN];
  char   file[MAX_FILENAME_LEN];
  const char  *fnct_name = "changeServerProcess():";
  char  *s;
  int    i;
  int    rtnval = 0;
  char   str_NO_SUCH_JOB[] = "NO_SUCH_JOB";

/*
 * Change server process data.
 */
  if (which == SCHEDULED_PRIORITY)
    sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  else
    sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);

  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);

  if (change_proc_id == NULL)
    {
      i = 0;
      dbRewind(db);
      while ((s = dbNextrec(db, NULL)) != NULL)
        {
          if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
              (strncmp(s, "JOB", strlen("JOB")) == 0))
            {
              continue;
            }

          if ((unbundleData(s, "ICICCICCCCCCCICCCICICCCCIIII",
                            &server_proc.pid,
                            server_proc.process_id,
                            &server_proc.priority,
                            server_proc.progroup_title,
                            server_proc.command_line,
                            &server_proc.proc_status,
                            server_proc.cust_proc_status,
                            server_proc.login_name,
                            server_proc.unix_owner,
                            server_proc.job_descrip,
                            server_proc.request_time,
                            server_proc.launch_time,
                            server_proc.end_time,
                            &server_proc.security_class,
                            server_proc.orig_host,
                            server_proc.prt_name,
                            server_proc.hostname,
                            &server_proc.prt_sec_class,
                            server_proc.prt_filename,
                            &server_proc.prt_flag,
                            server_proc.sdi_ctl_rec,
                            server_proc.sdi_data_file,
                            server_proc.sdi_tci_file,
                            server_proc.sdi_device,
                            &server_proc.orientation,
                            &server_proc.banner,
                            &server_proc.copies,
                            &server_proc.papersize) != -1)
              && (pid == server_proc.pid))
            {
              i = 1;
              break;
            }
        }

      if (!i)
        {
          dbClose(db);
          return(ERROR_CANT_FIND_VALUE);
        }
    }
  else
    {                                      /* want to change the proc ID ... */
      if ( ((s = dbFetch(db, change_proc_id)) == NULL) ||
           (unbundleData(s, "ICICCICCCCCCCICCCICICCCCIIII",
                         &server_proc.pid,
                         server_proc.process_id,
                         &server_proc.priority,
                         server_proc.progroup_title,
                         server_proc.command_line,
                         &server_proc.proc_status,
                         server_proc.cust_proc_status,
                         server_proc.login_name,
                         server_proc.unix_owner,
                         server_proc.job_descrip,
                         server_proc.request_time,
                         server_proc.launch_time,
                         server_proc.end_time,
                         &server_proc.security_class,
                         server_proc.orig_host,
                         server_proc.prt_name,
                         server_proc.hostname,
                         &server_proc.prt_sec_class,
                         server_proc.prt_filename,
                         &server_proc.prt_flag,
                         server_proc.sdi_ctl_rec,
                         server_proc.sdi_data_file,
                         server_proc.sdi_tci_file,
                         server_proc.sdi_device,
                         &server_proc.orientation,
                         &server_proc.banner,
                         &server_proc.copies,
                         &server_proc.papersize) == -1)) 
        {
          dbClose(db);
          return(ERROR_CANT_READ_VALUE);
        }
    }

  strcpy(app_name, server_proc.progroup_title);

/*
 * Reassign process status if needed.
 */
  if (mask & CHANGE_PROC_STAT)
    server_proc.proc_status = retval;

/*
 * Reassign custom process status if needed.
 */
  if (mask & CHANGE_CUST_STAT) 
    strcpy(server_proc.cust_proc_status, cust_stat);

/*
* Assign Device 
*/
  if (mask & CHANGE_DEVICE)
    strcpy(server_proc.sdi_device, devname);

/*
 * Reassign PID value if needed.
 */
  if (mask & CHANGE_PID_VALUE)
    {
      if (pid == RUN_NEXT_STATE)
        {
          rtnval = ERROR_RUN_NEXT_TAKEN; /* indicates no change made to pid */
          if ((s = dbFetch(db, "RUN_NEXT_JOB")) != NULL)
            {
              if (strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)
                {
                  sprintf(buf, "JOB%s", change_proc_id);
                  if (dbStore(db, "RUN_NEXT_JOB", buf, DB_REPLACE) == 0)
                    {
                      rtnval = 0;
                      server_proc.pid = pid;
                    }
                  else
                    syslog(LOG_WARNING, "%s Failed to set \"run next"
                           " job\" record!", fnct_name);
                }
              else
                {
                  dbClose(db); 
                  return(ERROR_RUN_NEXT_TAKEN);
                }
            }
        }
      else
        if ( pid > 0 && (s = dbFetch(db,"RUN_NEXT_JOB")) != NULL &&
             (strcmp(s,"NO_SUCH_JOB") != 0) )
          {
            if (dbStore(db, "RUN_NEXT_JOB", str_NO_SUCH_JOB, DB_REPLACE) != 0)
              syslog(LOG_WARNING,"%s Failed to clear \"run"
                     " next job\" record!", fnct_name);
            server_proc.pid = pid;
          }
        else
          server_proc.pid = pid;
    }

/*
 * Reassign launch time if needed.
 */
  if (mask & CHANGE_LAUNCH_TIME)
    strcpy(server_proc.launch_time, launch_time);

/*
 * Reassign end time if needed.
 */
  if (mask & CHANGE_END_TIME)
    strcpy(server_proc.end_time, end_time);

/*
 * Reassign priority if needed.
 */
  if (mask & CHANGE_PRIORITY)
    server_proc.priority = priority;

/*
 * Reassign prt_flag if needed.
 */
  if (mask & CHANGE_PRT_FLAG)
    server_proc.prt_flag = bit_flags;

  buf[0] = 0;
  (void) bundleData(buf, "ICICCICCCCCCCICCCICICCCCIIII",
                    server_proc.pid,
                    server_proc.process_id,
                    server_proc.priority,
                    server_proc.progroup_title,
                    server_proc.command_line,
                    server_proc.proc_status,
                    server_proc.cust_proc_status,
                    server_proc.login_name,
                    server_proc.unix_owner,
                    server_proc.job_descrip,
                    server_proc.request_time,
                    server_proc.launch_time,
                    server_proc.end_time,
                    server_proc.security_class,
                    server_proc.orig_host,
                    server_proc.prt_name,
                    server_proc.hostname,
                    server_proc.prt_sec_class,
                    server_proc.prt_filename,
                    server_proc.prt_flag,
                    server_proc.sdi_ctl_rec,
                    server_proc.sdi_data_file,
                    server_proc.sdi_tci_file,
                    server_proc.sdi_device,
                    server_proc.orientation,
                    server_proc.banner,
                    server_proc.copies,
                    server_proc.papersize);
  
/*
 * Replace server process record.
 */
  if (dbStore(db, server_proc.process_id, buf, DB_REPLACE) != 0)
    {
      dbClose(db);
      return(ERROR_CANT_REPLACE_VALUE);
    }
  dbClose(db);

  return(rtnval);
}


/*****************************************************************************/

static int  includeProcess( ServerProcessItem  *server_proc,
                            int                 mask,
                            ProcessFilterData  *filter_data,
                            const char         *app,
                            const char         *login_name,
                            int                 role )
{

  if (strcmp(server_proc->progroup_title, app) != 0)
    return(FALSE);

  if (mask & SPQ_BIT_MASK_ONLY_CERTAIN_USER)
    if (strcmp(login_name, server_proc->login_name) != 0)
      return(FALSE);
  
  if (mask & SPQ_BIT_MASK_ONLY_RUNNING_PROCS)
    if (server_proc->pid <= 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_PRIORITY)
    if (server_proc->priority != filter_data->priority)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_COMMAND_LINE)
    if (strcmp(server_proc->command_line,filter_data->command_line) != 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_CUST_PROC_STATUS)
    if (strcmp(server_proc->cust_proc_status,
               filter_data->cust_proc_status) != 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_LOGIN_NAME)
    if (strcmp(server_proc->login_name,filter_data->login_name) != 0)
      return(FALSE);
  
  if (mask & SPQ_BIT_MASK_UNIX_OWNER)
    if (strcmp(server_proc->unix_owner,filter_data->unix_owner) != 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_PROGROUP_TITLE)
    if (strcmp(server_proc->progroup_title,filter_data->progroup_title) != 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_JOB_DESCRIP)
    if (strcmp(server_proc->job_descrip,filter_data->job_descrip) != 0)
      return(FALSE);

  if (mask & SPQ_BIT_MASK_ORIG_HOST)
    if (strcmp(server_proc->orig_host,filter_data->orig_host) != 0)
      return(FALSE);

  switch (role) {
  case NTCSS_USER_ROLE:
    return(strcmp(login_name, server_proc->login_name) == 0);
    break;
  case APP_ADMIN_ROLE:
  case NTCSS_ADMIN_ROLE:
    return(TRUE);
    break;
  }

  return(FALSE);
}


/*****************************************************************************/
/* For the role var;  0 is NTCSS User, 1 is App Admin, 2 is NTCSS Admin.
*/

int  getServerProcessQueueList( const char         *app,
                                const char         *login_name,
                                int                 role,
                                char               *query,
                                char               *msg,
                                const char         *db_dir,
                                int                 is_sched,
                                int                 mask,
                                ProcessFilterData  *filter_data)
{

  char file[MAX_FILENAME_LEN], *s, buf[32];
  DB *db;
  ServerProcessItem server_proc;
  int flag, leng, num_que_items = 0;
  FILE *fp;
  
  if ((fp = fopen(query, "w")) == NULL)
    return(ERROR_CANT_CREATE_QUERY);
  fclose(fp);

  if (is_sched)
    sprintf(file, "%s/%s", db_dir, SCHEDULED_PROCS_TABLE);
  else
    sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDWR, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL) {
    return(ERROR_CANT_OPEN_DB_FILE);
  }

  dbRewind(db);
  leng = 0;
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    flag = 0;
    if (unbundleData(s, "ICICCICCCCCCCICCCICICCCCIIII",
                     &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title,
                     server_proc.command_line,
                     &server_proc.proc_status,
                     server_proc.cust_proc_status,
                     server_proc.login_name,
                     server_proc.unix_owner,
                     server_proc.job_descrip,
                     server_proc.request_time,
                     server_proc.launch_time,
                     server_proc.end_time,
                     &server_proc.security_class,
                     server_proc.orig_host,
                     server_proc.prt_name,
                     server_proc.hostname,
                     &server_proc.prt_sec_class,
                     server_proc.prt_filename,
                     &server_proc.prt_flag,
                     server_proc.sdi_ctl_rec,
                     server_proc.sdi_data_file,
                     server_proc.sdi_tci_file,
                     server_proc.sdi_device,
                     &server_proc.orientation,
                     &server_proc.banner,
                     &server_proc.copies,
                     &server_proc.papersize) == -1) {
      dbClose(db);
      return(ERROR_CANT_READ_VALUE);
    }

    if (mask & SPQ_BIT_MASK_ONLY_USER_ACTIVE_PROCS) {
      if ((strcmp(login_name, server_proc.login_name) == 0) &&
          (server_proc.pid > 0)) {
        leng += bundleQuery(query, "C", server_proc.process_id);
        num_que_items++;
      }
      continue;
    }

    if (includeProcess(&server_proc, mask, filter_data, app, login_name,
                       role)){
      leng += bundleQuery(query, "ICICCI",
                          server_proc.pid,
                          server_proc.process_id,
                          server_proc.priority,
                          server_proc.progroup_title,
                          server_proc.command_line,
                          server_proc.proc_status);

      if (mask & SPQ_BIT_MASK_WITH_CUST_PROC_STATUS)
        leng += bundleQuery(query, "C", 
                            server_proc.cust_proc_status);

      leng += bundleQuery(query, "CC",
                          server_proc.login_name,
                          server_proc.unix_owner);
      
      if (mask & SPQ_BIT_MASK_WITH_JOB_DESCRIP)
        leng += bundleQuery(query, "C", 
                            server_proc.job_descrip);

      leng += bundleQuery(query, "CCCIC",
                          server_proc.request_time,
                          server_proc.launch_time,
                          server_proc.end_time,
                          server_proc.security_class,
                          server_proc.orig_host);

      if (mask & SPQ_BIT_MASK_WITH_PRT_REQ_INFO) 
        leng += bundleQuery(query, "CCICICCCCIIII",
                            server_proc.prt_name,
                            server_proc.hostname,
                            server_proc.prt_sec_class,
                            server_proc.prt_filename,
                            server_proc.prt_flag,
                            server_proc.sdi_ctl_rec,
                            server_proc.sdi_data_file,
                            server_proc.sdi_tci_file,
                            server_proc.sdi_device,
                            server_proc.orientation,
                            server_proc.banner,
                            server_proc.copies,
                            server_proc.papersize);
      num_que_items++;
    }
  }
  dbClose(db);

  sprintf(buf, "%d", num_que_items);
  sprintf(file, "%d", leng + strlen(buf) + 1);
  sprintf(buf, "%1d", strlen(file));
  strcat(msg, buf);
  strcat(msg, file);

  sprintf(file, "%d", num_que_items);
  sprintf(buf, "%1d", strlen(file));
  strcat(msg, buf);
  strcat(msg, file);
    
  return(leng);   /* everthing ok, server process queue data is returned */
}


/*****************************************************************************/

int  getServerProcessId( int          pid,
                         const char  *process_id,
                         const char  *appname,
                         const char  *db_dir )

{
  DB   *db;
  char  file[MAX_FILENAME_LEN], *s;
  int   i, j, retval = 0;

  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDONLY, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);

  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICIC", &i, process_id, &j, appname) == -1) {
      retval = ERROR_CANT_READ_VALUE;
      break;
    }
    if (i == pid) {
      retval = 1;
      break;
    }
  }
  dbClose(db);
  
  return(retval);
}


/*****************************************************************************/

int  killActiveServerProcesses(const char  *db_dir,
                               int         *num_jobs,
                               int         *siglist,
                               const char  *applist,
                               int          appcnt )
{
  const char  *fnct_name = "killActiveServerProcesses():";
  ServerProcessItem  server_proc;
  DB    *db;
  const char  *appptr;
  char   file[MAX_FILENAME_LEN];
  char  *s;
  int    child_pid;
  int    flag;
  int    i;
  int    retval = 0;
  int    sc;
  int    signal_value;
  int    skip;

  *num_jobs = 0;
    
  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDONLY, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);

  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICIC", &server_proc.pid,server_proc.process_id,
                     &server_proc.priority,server_proc.progroup_title) == -1) {
      retval = ERROR_CANT_READ_VALUE;
      continue;
    }
    if (server_proc.pid > 0) {
      skip = 0;
      flag = 1;
      i = 0;

      appptr = applist;

      for (sc=0;sc<appcnt;sc++) {
        if (!strcmp(server_proc.progroup_title,appptr))
          break;
        appptr += MAX_PROGROUP_TITLE_LEN+1; 
      }

      if (sc >= appcnt)
        signal_value = SIGKILL;
      else
        signal_value=siglist[sc];
      
      while (!skip && ((child_pid = getChildProcess(server_proc.pid)) 
                       != -1)) {
        i++;
        if (i > MAX_NUM_KILL_CHILD) {
          syslog(LOG_WARNING, "%s Attempted to kill more than %d"
                 " children for a process. Defunct processes may"
                 " have been created!", fnct_name, MAX_NUM_KILL_CHILD);
          break;
        }
        flag = 0;
        (*num_jobs)++;
        
        if (child_pid > 0) {
          if (kill((pid_t) child_pid, signal_value) < 0) {
            syslog(LOG_WARNING, "%s Failed to kill process %d!",
                 fnct_name, child_pid);
            retval = ERROR_CANT_KILL_SVR_PROC;
            skip = 1;
          }
        }
      }
      if (flag || (i > MAX_NUM_KILL_CHILD)) {
        (*num_jobs)++;
	  if (server_proc.pid > 0) {
        	if (kill((pid_t) server_proc.pid, signal_value) < 0) {
          	syslog(LOG_WARNING, "%s Failed to kill process %d!",
                 	fnct_name, server_proc.pid);
          	retval = ERROR_CANT_KILL_SVR_PROC;
          }
	}
      }
    }
  }
  dbClose(db);

  return(retval);
}

/*****************************************************************************/

int  validateServerProcessId( const char  *process_id,
                              const char  *db_dir)

{

  DB *db;
  char file[MAX_FILENAME_LEN], *s, proc_id[MAX_PROCESS_ID_LEN];
  int i, retval = ERROR_USER_NOT_VALID;

  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  if ((db = dbOpen(file, O_RDONLY, 
                   S_IRUSR | S_IWUSR |
                   S_IRGRP | S_IROTH)) == NULL)
    return(ERROR_CANT_OPEN_DB_FILE);
  
  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "IC", &i, proc_id) == -1) {
      retval = ERROR_CANT_READ_VALUE;
      break;
    }
    if (strcmp(process_id, proc_id) == 0) {
      retval = 1;
      break;
    }
  }
  dbClose(db);

  return(retval);
}


/*****************************************************************************/

char  *getRunningServerProcessApps( const char  *db_dir )

{

  ServerProcessItem server_proc;
  DB    *db;
  int    flag;
  int    i, n;
  char   appname[MAX_PROGROUP_TITLE_LEN+1];
  char   file[MAX_FILENAME_LEN];
  const char  *fnct_name = "getRunningServerProcessApps():";
  char  *retlist;
  char  *s;

  retlist = (char *) malloc(sizeof(char)*5);
  if (retlist == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to allocate space for server "
             "process app list!", fnct_name);
      return(NULL);
    }
  n = 5;
  retlist[0] = 0;
  sprintf(file, "%s/%s", db_dir, ACTIVE_PROCS_TABLE);
  db = dbOpen(file, O_RDWR, 
              S_IRUSR | S_IWUSR |
              S_IRGRP | S_IROTH);
  if (db == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open database file %s for reading!",
             fnct_name, file);
      return(NULL);
    }
  dbRewind(db);
  while ((s = dbNextrec(db, NULL)) != NULL) {
    if ((strncmp(s, "NO_SUCH_JOB", strlen("NO_SUCH_JOB")) == 0)  ||
        (strncmp(s, "JOB", strlen("JOB")) == 0)) {
      continue;
    }
    if (unbundleData(s, "ICIC", &server_proc.pid,
                     server_proc.process_id,
                     &server_proc.priority,
                     server_proc.progroup_title) == -1) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to read record from database file %s!",
             fnct_name, file);
      return(NULL);
    }
/*
 * Only get running process apps
 */
    if (server_proc.pid <= 0)
      continue;
/*
 * Check for duplication.
 */
    i = flag = 0;
    while ((i = stripWord(i, retlist, appname)) != -1) {
      if (strcmp(appname, server_proc.progroup_title) == 0)
        flag = 1;
    }
    if (flag)
      continue;
    n += strlen(server_proc.progroup_title) + 1;
    if ((retlist = (char *) reallocateString(retlist,
                                             sizeof(char)*n)) == NULL) {
      dbClose(db);
      syslog(LOG_WARNING, "%s Failed to allocate space for server"
             " process app list!", fnct_name);
      return(NULL);
    }
    strcat(retlist, server_proc.progroup_title);
    strcat(retlist, " ");
  }
  dbClose(db);
  
  return(retlist);
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
