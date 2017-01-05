
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * grp_fcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <syslog.h> /*For debugging only! */
#include <time.h>
#include <rpcsvc/ypclnt.h>
#include <sys/types.h>
#include <rpc/rpc.h>
#include <rpcsvc/yp_prot.h>
#include <unistd.h>
#include <stdlib.h>

/** LOCAL INCLUDES **/

#include <ExtraProtos.h>
#include "install.h"


/** FUNCTION PROTOTYPES **/

int nis_running_check(void);
int add_unix_group(char*);


/** STATIC VARIABLES **/

static char  *gbl_nis_master;


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
/* Checks to see if the NIS is running. */

int  nis_running_check()

{
  char *gbl_domain;
  int  x;
  char str_hosts_byname[] = "hosts.byname";

  x = yp_get_default_domain(&gbl_domain);
  if (x != 0)
    {
      syslog(LOG_WARNING, "nis_running_check: Failed to determine the "
             "default domain! Error %i", x);
      return(0);
    }

  x = yp_master(gbl_domain, str_hosts_byname, &gbl_nis_master);
  if (x != 0)
    return(0);  /* Error ? */

  return(1);  /* NIS is running */
}


/*****************************************************************************/

int  add_unix_group(char *group)
{

#ifdef linux
  gid_t  largest_gr_gid;
#else
  int  largest_gr_gid;
#endif
  int  nis_running;
  int  x;                    /* generic use */
  char  cmd[MAX_CMD_LEN];
  char  host_name[MAX_INSTALL_ITEM_LEN];
  char  tmpfile_[MAX_FILENAME_LEN];
  FILE *fp;
  struct group  *my_group;

  nis_running = nis_running_check();

  if (gethostname(host_name, MAX_INSTALL_ITEM_LEN) != 0)
    {
      syslog(LOG_WARNING, "add_unix_group: Could not determine "
             "this host's name!");
      return(0);
    }

  if (nis_running && (strcmp(host_name, gbl_nis_master) != 0))
    {
      syslog(LOG_WARNING, "add_unix_group: An attempt was made to add a group"
             " <%s> to this host (wich is not a NIS master)!", group);
      return(1);
    }

  if (getgrnam(group) != NULL)
    {
      syslog(LOG_WARNING, "add_unix_group: Group <%s> already exists!",
             group);
      return(1);
    }

  largest_gr_gid = 0;
  while ((my_group = getgrent()) != NULL)
    if (largest_gr_gid < my_group->gr_gid)
      largest_gr_gid = my_group->gr_gid;

  sprintf(tmpfile_, "%s.tmp", GROUP_FILE);
  sprintf(cmd, "%s %s %s", CP_PROG, GROUP_FILE, tmpfile_);
  if (system(cmd) != 0)
    {
      syslog(LOG_WARNING, "add_unix_group: Could not run <%s>!", cmd);
      return(0);
    }

  x = chmod(GROUP_FILE, 0644);
  if (x != 0)
    {
      syslog(LOG_WARNING, "add_unix_group: chmod returned error %i!", x);
    return(0);
    }

  fp = fopen(tmpfile_, "a");
  if (fp == NULL)
    return(0);
    
  fprintf(fp, "%s:*:%d:\n", group, largest_gr_gid + 1);
  fflush(fp);
  fclose(fp);

  x = rename(tmpfile_, GROUP_FILE);
  if (x == -1)
    {
      syslog(LOG_WARNING, "add_unix_group: rename returned error %i!", x);
      return(0);
    }

  if (getgrnam(group) == NULL)
    {
      syslog(LOG_WARNING, "add_unix_group: Modification didn't take for group"
             " <%s>!", group);
      return(0);
    }

  x = chmod(GROUP_FILE, 0444);
  if (x != 0)
    {
      syslog(LOG_WARNING, "add_unix_group: chmod returned error %i!", x);
      return(0);
    }

  return(1);
}
