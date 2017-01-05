
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* Replication.c */

/* This file should only contain code that implements or specifically supports
   replication activites.
*/

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <SockFcts.h>
#include <NetSvrInfo.h>          /* structures defined in here */
#include <NetCopy.h>
#include <LibNtcssFcts.h>
#include <ExtraProtos.h>


/** LOCAL INCLUDES **/

/** GLOBAL DECLARATIONS **/

/** FUNCTION PROTOTYPES **/


/*****************************************************************************/
/* This function makes copies of all the files to be backed up and then tars
   those copies together into a backup file.  An info file is also produced.
   The copy method is sort of kludgy and should be revised in the future.
   All of the copied tree should be dependant what is in the backup_list file,
   but if it is now modified, this code will have to be modified to make copies
   of it as well.
*/

int  BuildMasterDBbackupFiles( void )

{
  const char  *fnct_name = "BuildMasterDBbackupFiles():";
  char   sys_cmd[3 * MAX_FILENAME_LEN];
  char   rmdir_cmd[(2 * MAX_FILENAME_LEN) + 1];
  char   decryptfile[MAX_FILENAME_LEN];
  char   master_host[MAX_HOST_NAME_LEN + 1];
  char   tmp_buff[MAX_FILENAME_LEN + 1];
  char   temp_tar_filename[MAX_FILENAME_LEN + 1];
  char   temp_info_filename[MAX_FILENAME_LEN + 1];
  int    i, n;
  int    login_lock_id;
  int    main_lock_id;
  int    sysconf_lock_id;
  int    x;
  FILE  *rp;
  FILE  *fp;
  struct stat  stat_buf;               /* for getting file sizes */

syslog(LOG_WARNING, "%s DWB Beginning.", fnct_name);
  sprintf(temp_tar_filename, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_TEMP_TAR_FILE);

  /* Determine if host is Master Server.. */
  x = get_master_server(NTCSS_DB_DIR, master_host);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Failed to determine the master's name!",
             fnct_name);
      return(-1);
    }

  /*  Set the database locks to prevent changes during the copy operation.. */
  main_lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (main_lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to lock main database!", fnct_name);
      return(-2);
    }
  
  login_lock_id = setReadDBLock(LOGIN_INFO_RW_KEY, LOCK_WAIT_TIME);
  if (login_lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to lock the login info database!",
             fnct_name);
      unsetReadDBLock(main_lock_id);
      return(-3);
    }
  sysconf_lock_id = setReadDBLock(SYS_CONF_RW_KEY, LOCK_WAIT_TIME);
  if (sysconf_lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to lock system config database!",
             fnct_name);
      unsetReadDBLock(main_lock_id);
      unsetReadDBLock(login_lock_id);
      return(-4);
    }
  /* TO DO:  Should obtain msg bb locks also(?) */

  /* Gets rid of any old files/dirs that might no longer exist (esp in the
     msg dir since users/memberships could have been deleted.. */
  sprintf(rmdir_cmd, "%s %s", RM_PROG, TMP_DB_BACKUP_TAR_DIR);

  i = system(rmdir_cmd);
  if (i < 0)
    {
      /* If this fails, it might be because the directory was already removed.
         Therefor, we don't exit.*/
      syslog(LOG_WARNING, "%s Failed to remove directory %s! -- %s", fnct_name,
             TMP_DB_BACKUP_TAR_DIR, strerror(errno));
    }

  /* We create this directory so that /database is copied into it as a subdir
     and not just /databse's files, as would happen if the bakup dir did not
     pre-exist.
  */
  i = mkdir(TMP_DB_BACKUP_TAR_DIR, S_IRWXU);
  if (i < 0)
    {
      /* This would fail if it wasn't removed previously, which is still OK. */
      syslog(LOG_WARNING, "%s Failed to make directory %s! -- %s", fnct_name,
             TMP_DB_BACKUP_TAR_DIR, strerror(errno));
    }

 /* Make a snapshot of the database to tar with by copying all files over.. */

  /* We use a single iteration loop here just so we can use the "break" call.*/
  for (n = 0; n < 1; n++)
    {
      sprintf(sys_cmd, "%s -rf %s %s", CP_PROG, NTCSS_DB_DIR,
              TMP_DB_BACKUP_TAR_DIR);
      i = system(sys_cmd);
      if (i != 0)
        {
          syslog(LOG_WARNING, "%s Failed to run %s!", fnct_name, sys_cmd);
          break;
        }

      /* Make copies of the messages directory.. */
      sprintf(sys_cmd, "%s -rf %s/%s %s", CP_PROG, NTCSS_HOME_DIR,
              NTCSS_MSG_DIR, TMP_DB_BACKUP_TAR_DIR);
      i = system(sys_cmd);
      if (i != 0)
        {
          syslog(LOG_WARNING, "%s Failed to run %s!", fnct_name, sys_cmd);
          break;
        }

      /* Create repository structure for spool/.. */
      sprintf(sys_cmd, "%s/spool", TMP_DB_BACKUP_TAR_DIR);
      i = mkdir(sys_cmd, S_IRWXU);

      /* Make copies of the NT drivers directory under spool.. */
      sprintf(sys_cmd, "%s -rf %s/%s %s/spool", CP_PROG, NTCSS_HOME_DIR,
              NTDRIVER_DIR, TMP_DB_BACKUP_TAR_DIR);
      i = system(sys_cmd);
      if (i != 0)
        {
          syslog(LOG_WARNING, "%s Failed to run %s!", fnct_name, sys_cmd);
          break;
        }

      /* Create repository structure for log_items file/.. */
      sprintf(sys_cmd, "%s/applications", TMP_DB_BACKUP_TAR_DIR);
      i = mkdir(sys_cmd, S_IRWXU);
      sprintf(sys_cmd, "%s/applications/data", TMP_DB_BACKUP_TAR_DIR);
      i = mkdir(sys_cmd, S_IRWXU);
      sprintf(sys_cmd, "%s/applications/data/sys_mon", TMP_DB_BACKUP_TAR_DIR);
      i = mkdir(sys_cmd, S_IRWXU);

      sprintf(sys_cmd, "%s -rf %s/applications/data/sys_mon/log_items "
              "%s/applications/data/sys_mon/",
              CP_PROG, NTCSS_HOME_DIR, TMP_DB_BACKUP_TAR_DIR);
      i = system(sys_cmd);
      if (i != 0)
        {
          syslog(LOG_WARNING, "%s Failed to run %s!", fnct_name, sys_cmd);
          break;
        }

      /* Copy the init_data dir's contents.. */
      sprintf(sys_cmd, "%s -rf %s %s", CP_PROG, NTCSS_INIT_DATA_DIR,
              TMP_DB_BACKUP_TAR_DIR);
      i = system(sys_cmd);
      if( i != 0)
        {
          syslog(LOG_WARNING, "%s Failed to run %s!", fnct_name, sys_cmd);
          break;
        }
    }        /* End of copying loop. */


  unsetReadDBLock(main_lock_id);
  unsetReadDBLock(login_lock_id);
  unsetReadDBLock(sysconf_lock_id);

  if (i != 0)
    {
      syslog(LOG_WARNING, "%s Failed to make temp copies of the backup dirs!",
             fnct_name);
      return(-5);
    }

/**** Create a new list of the files, prepending the full path... * /
   sprintf(backup_list_filename, "%s/%s", NTCSS_HOME_DIR,
   NTCSS_BACKUP_LIST_FILE);
   rp = fopen(backup_list_filename, "r");
   if (rp == NULL)
   {
   syslog(LOG_WARNING, "%s Unable to open %s!", fnct_name,
   backup_list_filename);      
   remove(TMP_DB_BACKUP_TAR_DIR);
   return(-6);
   }

   sprintf(tmp_backup_list_filename, "%s.tmp", backup_list_filename);
   fp = fopen(tmp_backup_list_filename, "w");
   if (fp == NULL)
   {
   fclose(rp);
   syslog(LOG_WARNING, "%s Unable to create %s!", fnct_name,
   tmp_backup_list_filename);
   remove(TMP_DB_BACKUP_TAR_DIR);
   return(-7);
   }

   / * Prepend the temp path to the files listed in this file..
   while (fgets(tmp_buff, MAX_FILENAME_LEN, rp) != NULL)
   fprintf(fp, "%s/%s", TMP_DB_BACKUP_TAR_DIR, tmp_buff);
   fclose(rp);
   fclose(fp);
*****/

  /* Run tar ... */
# ifdef SYSV
  sprintf(sys_cmd, "%s cf %s/%s -C %s `cat %s` 1>&2", TAR_PROG,
          NTCSS_HOME_DIR, NTCSS_BACKUP_TEMP_TAR_FILE, TMP_DB_BACKUP_TAR_DIR,
          /* tmp_backup_list_filename);*/
          NTCSS_BACKUP_LIST_FILE);
# endif
# ifdef VSUN
  sprintf(sys_cmd, "%s cf %s/%s -C %s -I %s 1>&2", TAR_PROG,
          NTCSS_HOME_DIR, NTCSS_BACKUP_TEMP_TAR_FILE, TMP_DB_BACKUP_TAR_DIR,
          /*tmp_backup_list_filename);*/
          NTCSS_BACKUP_LIST_FILE);
# endif

  i = system(sys_cmd);

  if (i != 0)
    {
      syslog(LOG_WARNING, "%s Error tarring backup file %s!  System() "
             "returned %i", fnct_name, tmpfile, i);
      remove(TMP_DB_BACKUP_TAR_DIR);
      return(-8);
    }

  sync();             /* Ensures that cached tar file is flushed to disk. */  


/* Assemble the master backup info file. */
  sprintf(temp_info_filename, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_TEMP_INFO_FILE);
  fp = fopen(temp_info_filename, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open temp file %s!", fnct_name,
             temp_info_filename);
      return(-9);
    }

  /* Check the size of the tar file... */
  i = stat(temp_tar_filename, &stat_buf);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Unable to stat new Master tar file %s!",
             fnct_name, temp_info_filename);
      fclose(fp);
      return(-10);
    }

  fprintf(fp, "%li\n", (long)stat_buf.st_size);          /* Record the size. */

  /* Get the db vers #... */
  sprintf(tmp_buff, "%s/%s", NTCSS_DB_DIR, "VERSION");
  rp = decryptAndOpen(tmp_buff, decryptfile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to decrypt file %s!", fnct_name,
             tmp_buff);
      fclose(fp);
      remove(decryptfile);
      return(-11);
    }

  if (fgets(tmp_buff, MAX_FILENAME_LEN, rp) == NULL)
    {
      fclose(fp);
      fclose(rp);
      remove(decryptfile);
      syslog(LOG_WARNING, "%s Failed to read from file %s!", fnct_name,
             decryptfile);
      return(-12);
    }
  fclose(rp);
  remove(decryptfile);

  /* Record the db version # and host this file originated from.. */
  fprintf(fp, "%s", tmp_buff);
  fprintf(fp, "%s\n", master_host);
  fclose(fp);
  syslog(LOG_WARNING, "%s Finished.",fnct_name);
  return(0);
}

/*****************************************************************************/
/* Gets the backup files from the master via netcopy. Also calls a function to
   check the file sizes and renames the tmp files to their usual names.
*/

int  copyMasterBackupFiles( void )

{
  int     n;
  const char   *fct_nam = "copyMasterBackupFiles";
  char    strDataFile[MAX_FILENAME_LEN];
  char    strLocalStatusFile[MAX_FILENAME_LEN];
  char    strMasterHost[MAX_HOST_NAME_LEN];

  sprintf(strLocalStatusFile, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_TEMP_INFO_FILE);

  n = get_master_server(NTCSS_DB_DIR, strMasterHost);
  if (n < 0)
    {
      syslog(LOG_WARNING, "%s Failed to determine master server!", fct_nam);
      return(-1);
    }

  /* If I am master, I don't send the file to myself */
  if (n != 0)
    {
      syslog(LOG_WARNING, "%s Attempted to copy backup files to the"
             " NTCSS master server!", fct_nam);
      return(-2);
    }

  /* Get the backup tar file */  
  sprintf(strDataFile, "%s/%s", NTCSS_HOME_DIR, NTCSS_BACKUP_TEMP_TAR_FILE);
  if (net_copy("get", strMasterHost, strDataFile, strDataFile,
               XFER_BINARY) < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get file (%s) from master(%s)!",
             fct_nam, strDataFile, strMasterHost);
      return(-3);
    }

  /* Get the temp info file for the backup file */
  sprintf(strDataFile, "%s/%s", NTCSS_HOME_DIR, NTCSS_BACKUP_TEMP_INFO_FILE);
  if (net_copy("get", strMasterHost, strDataFile, strDataFile,
               XFER_ASCII) < 0)
    {
      syslog(LOG_WARNING, "%s Failed to get file (%s) from master(%s)!",
             fct_nam, strDataFile, strMasterHost);
      return(-4);
    }

  /* Check for correct file sizes and rename the files */
  if (VerifyCopiedMasterBackupFile() < 0)
    return(-4);

  return(0);
}


/*****************************************************************************/
/* Sends messages to all the replication servers to get a copy of the master's
   db backup tar file.  The host list is generated in this function unless
   it is passed in via the parameter.  This function can only be called on the
   master.
*/

int  sendMasterFilesToReplServers( const char  *strInHostList )

{
  int    num_hosts;
  int    i;
  int    nRet;
  int    pfp_in;
  int    pfp_out;
  char   ch;
  const char  *fct_nam = "sendMasterFilesToReplServers():";
  const char  *pStrHostList;
  char   strHostListLen[50];
  char   strLocalHostList[(MAX_HOST_NAME_LEN + 2) * MAX_NUM_HOSTS];
  char   strMaster[MAX_HOST_NAME_LEN + 1];
  char   str_local_host_name[MAX_HOST_NAME_LEN + 1];
  char   strMsg[CMD_LEN + 1];
  char   strReply[CMD_LEN + 1];
  HostItem   **hostItems;

  /* Make sure this is the master server.. */

  if (gethostname(str_local_host_name, MAX_HOST_NAME_LEN) != 0)
    {
      syslog(LOG_WARNING, "%s Couldn't determine this host's name!", fct_nam);
      return(-1);
    }

  if (get_master_server(NTCSS_DB_DIR, strMaster) < 0)
    {
      syslog(LOG_WARNING, "%s Couldn't determine the master server!", fct_nam);
      return(-2);
    }

  if (strcmp(strMaster, str_local_host_name) != 0)
    {
      syslog(LOG_WARNING, "%s Attempted to distribute master files from"
             " a non-master server!", fct_nam);
      return(-3);
    }
    
  /* If strInHostList is NULL then we send all replication hosts
     the message to pick up the Master files */

  if (strInHostList == NULL)
    {
      hostItems = getHostData(&num_hosts, NTCSS_DB_DIR);
      if (num_hosts < 0)
        syslog(LOG_WARNING, "%s Failed to retreive a host list!", fct_nam);

      strLocalHostList[0] = '\0';
      for (i = 0; i < num_hosts; i++)
        {
          /* Add host to list if it is a replication server and is
             not the master. */

          if ( (strcmp(hostItems[i]->hostname, strMaster) != 0) &&
               (hostItems[i]->repl == 1) )
            {
              if (strLocalHostList[0] != '\0')
                strcat(strLocalHostList, DM_HOST_SEP_CHARS_STRING);
              strcat(strLocalHostList, hostItems[i]->hostname);
            }
        }

      pStrHostList = strLocalHostList;
      freeHostData(hostItems, num_hosts);
    }
  else
    {
      pStrHostList = strInHostList;
    }

  /* If there are no hosts to send to, just exit now. */
  if (pStrHostList[0] == '\0')
    return(0);
  
  if (requestAndWaitForDmPipe() != 0)
    return(-4);

  pfp_out = open(APP_TO_DM_PIPE, O_WRONLY);
  if (pfp_out < 0)
    {
      syslog(LOG_WARNING, "%s open(APP_TO_DM_PIPE) failed!", fct_nam);
      releaseDmPipe();
      return(-5);
    }

  /* Send the msg title so DM can figure out what it's supposed to do.. */  
  memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, MT_GETMASTERFILES, strlen(MT_GETMASTERFILES));
  if (write(pfp_out, strMsg, CMD_LEN) != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to write msg to DM!", fct_nam);
      close(pfp_out);
      releaseDmPipe();
      return(-6);
    }

  /* Send the host list string's length..*/
  sprintf(strHostListLen, "%05d", strlen(pStrHostList));
  if ((size_t)write(pfp_out, strHostListLen, strlen(strHostListLen)) !=
      strlen(strHostListLen))
    {
      syslog(LOG_WARNING, "%s Failed to write DM the host name string length!",
             fct_nam);
      releaseDmPipe();
      return(-7);
    }

  /* Send the host list string.. */
  if ((size_t)write(pfp_out, pStrHostList, strlen(pStrHostList)) != 
      strlen(pStrHostList))
    {
      syslog(LOG_WARNING, "%s Failed to write the host list to DM!", fct_nam);
      releaseDmPipe();
      return(-8);
    }

  /* Send actual message header destined for all the hosts in the list.. */
  memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, MT_GETMASTERFILES, strlen(MT_GETMASTERFILES));
  if (write(pfp_out, strMsg, CMD_LEN) != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to write out msg (%s)!", fct_nam, 
             MT_GETMASTERFILES);
      releaseDmPipe();
      return(-9);
    }

  /* Write out NULL for trailing data */
  if (write(pfp_out, "", 1) != 1)
    {
      syslog(LOG_WARNING, "%s Failed to write out trailing NULL!", fct_nam);
      releaseDmPipe();
      return(-10);
    }

  /* Need to read from dm via the app pipe to catch any returned messages.. */
  pfp_in = open(DM_TO_APP_PIPE, O_RDONLY);
  if (pfp_in < 0)
    {
      syslog(LOG_WARNING,"%s Unable to open DM to APP Pipe File!", fct_nam);
      releaseDmPipe();
      return(-8);
    }
  
  /* Not sure what is done with this reply(?) */
  nRet = read(pfp_in, strReply, CMD_LEN);
  if (nRet != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to read reply from the DM!", fct_nam);
      releaseDmPipe();
      return(-11);
    }

  /* Just throw away any trailing data */
  ch = '1';
  while(ch != '\0')
    {
      nRet = read(pfp_in, &ch, 1);
      if (!nRet)
        {
          syslog(LOG_WARNING, "%s Unable to read from in pipe!", fct_nam);
          close(pfp_in);     
          releaseDmPipe();
          return(-12);
        }
    }
  releaseDmPipe();

  return(0);
}


/*****************************************************************************/
/* Checks to make sure the freshly copied master server files are the correct
   sizes and renames them to their usual names. */

int  VerifyCopiedMasterBackupFile( void )

{
  const char  *fnct_nam = "VerifyCopiedMasterBackupFile():";
  char   normal_info_filename[MAX_FILENAME_LEN];
  char   normal_tar_filename[MAX_FILENAME_LEN];
  char   temp_tar_filename[MAX_FILENAME_LEN];
  char   temp_info_filename[MAX_FILENAME_LEN];
  int    i;
  long   backup_file_size;  /* original file size */
  struct stat  stat_buf;
  FILE  *fp;


  sprintf(temp_tar_filename, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_TEMP_TAR_FILE);
  sprintf(temp_info_filename, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_TEMP_INFO_FILE);
  sprintf(normal_tar_filename, "%s/%s", NTCSS_HOME_DIR, NTCSS_BACKUP_TAR_FILE);
  sprintf(normal_info_filename, "%s/%s", NTCSS_HOME_DIR,
          NTCSS_BACKUP_INFO_FILE);

  /* Check the existance of the info file... */
  i = stat(temp_info_filename, &stat_buf);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Unable to stat new Master Info File %s!",
             fnct_nam, temp_info_filename);
      return(-1);
    }

  /* Open the info file and get the file size for the tar file.. */
  fp = fopen(temp_info_filename, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Unable to open file <%s>!", fnct_nam, 
             temp_info_filename);
      return(-2);
    }

  if(fscanf(fp, "%li", &backup_file_size) < 1)
    {
      syslog(LOG_WARNING, "%s Unable to read file <%s>! File format"
             " may be incorrect!", fnct_nam, temp_info_filename);
      fclose(fp);
      return(-3);
    }

  fclose(fp);

  /* Check the existance of the tar file... */
  i = stat(temp_tar_filename, &stat_buf);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Unable to stat new Master File <%s>!", fnct_nam,
             temp_tar_filename);
      fclose(fp);
      return(-4);
    }

  /* Check the tar file's size.. */
  if (backup_file_size != (long)stat_buf.st_size)
    {
      syslog(LOG_WARNING, "%s Received Master Backup file %s is damaged!",
             fnct_nam, temp_tar_filename);
      syslog(LOG_WARNING, "%s Original size = %d, current size = %d",
             fnct_nam, backup_file_size, stat_buf.st_size);
      return(-5);
    }

  /* Rename the temp tar file to its normal name.. */
  i = rename(temp_tar_filename, normal_tar_filename);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Unable to rename incoming Master File %s!",
             fnct_nam, temp_tar_filename);
      return(-6);
    }

  /* Rename the temp info file to its normal name.. */
  i = rename(temp_info_filename, normal_info_filename);
  if (i < 0)
    {
      syslog(LOG_WARNING, "Unable to rename incoming Master Info File %s!",
             temp_info_filename);

      /* Get rid of the good master tar file since it is useless without
         its accompanying tmp_info file. */
      unlink(normal_tar_filename);
      return(-7);
    }
  
  return(0);
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
