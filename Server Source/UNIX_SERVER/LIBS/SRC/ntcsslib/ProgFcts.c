
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * ProgFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <syslog.h>
#include <pwd.h>
#include <unistd.h>

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <StringFcts.h>
#include <LibNtcssFcts.h>

/** LOCAL INCLUDES **/

#include "TextDBP.h"

#define PATH_SEP_CHAR_STRING " "

/** STATIC FUNCTION PROTOTYPES **/

/** STATIC VARIABLES **/

static const char *program_types[] = { "Client Program", "Server Program",
                                       "App. Admin. Server Program",
                                       "Host Admin. Server Program",
                                       "Superuser Server Program",
                                       "Unknown Value" };

/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
/* Untars the master server's database directory backup file onto the local
   host.  This code assumes the caller has already idled or shut the daemons
   down.
*/

int  unpack_master_files( void )

{
  const char  *fnct_name = "unpack_master_files():";
  char   cmd[3*MAX_FILENAME_LEN];
  char   datafile[MAX_FILENAME_LEN];
  char   host[MAX_HOST_NAME_LEN+1];
  char   tmp_file[MAX_FILENAME_LEN];
  char   version[MAX_VERSION_NUMBER_LEN+1];
  int    i;
  FILE  *fp;
  FILE  *rp;

  /* * Check if database and software versions are compatible. */
  sprintf(datafile, "%s/%s", NTCSS_HOME_DIR, NTCSS_BACKUP_INFO_FILE);
  rp = fopen(datafile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file \"%s\"!", fnct_name,
             datafile);
      return(-1);
    }

  /* Skip the first line in the file.. */
  fgets(cmd, MAX_FILENAME_LEN, rp); /* Master tar file's size. */

  /* Retreive the database version of the master's backup.. */
  if (fgets(cmd, MAX_FILENAME_LEN, rp) == NULL)
    {
      fclose(rp);
      syslog(LOG_WARNING, "%s Failed to read from file <%s>!", fnct_name,
             datafile);
      return(-2);
    }

  fclose(rp);
  stripNewline(cmd);
  strcpy(version, cmd);
  
  sprintf(datafile, "%s/%s", NTCSS_HOME_DIR, NTCSS_VERSION_FILE);
  rp = fopen(datafile, "r");
  if (rp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file <%s>!", fnct_name, datafile);
      return(-3);
    }

  if (fgets(cmd, MAX_FILENAME_LEN, rp) == NULL)
    {
      fclose(rp);
      syslog(LOG_WARNING, "%s Failed to read from file <%s>!", fnct_name,
             datafile);
      return(-4);
    }

  fclose(rp);
  stripNewline(cmd);
  if (strcmp(version, cmd) != 0)
    {
      syslog(LOG_WARNING, "%s Database and software version numbers are "
             "incompatible!", fnct_name);
      return(-5);
    }

/*
 * Unpack Master backup tar file.
 */
  sprintf(cmd, "%s xf %s/%s", TAR_PROG, NTCSS_HOME_DIR,
          NTCSS_BACKUP_TAR_FILE);
  i = system(cmd);
  if (i != 0)
    {
      syslog(LOG_WARNING, "%s Failed to untar the backup file!", fnct_name);
      return(-6);
    }

  if (gethostname(host, MAX_HOST_NAME_LEN) != 0)
    {
      syslog(LOG_WARNING, "%s Failed to determine this host's name!",
             fnct_name);
      return(-7);
    }
      
  sprintf(datafile, "%s/%s/%s", NTCSS_HOME_DIR,NTCSS_INIT_APP_DATA, NTCSS_APP);
  sprintf(tmp_file, "%s/%s/%s.tmp", NTCSS_HOME_DIR, NTCSS_INIT_APP_DATA,
          NTCSS_APP);
  fp = fopen(tmp_file, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open file <%s>!", fnct_name, tmp_file);
      return(-8);
    }

  rp = fopen(datafile, "r");
  if (rp == NULL)
    {
      fclose(fp);
      unlink(tmp_file);
      syslog(LOG_WARNING, "%s Failed to open file <%s>!", fnct_name, datafile);
      return(-9);
    }

  i = 1;
  while (fgets(cmd, 3 * MAX_FILENAME_LEN, rp) != NULL)
    {
      if (i == 2)
        fprintf(fp, "%s\n", host);
      else
        fprintf(fp, "%s", cmd);
      i++;
    }

  fclose(rp);
  fclose(fp);
  rename(tmp_file, datafile);
  chmod(datafile, S_IRUSR | S_IWUSR);

  return(0);
}


/*****************************************************************************/

const char  *getProgramType( int  value )

{
  if ( (value >= 0) && (value < NUM_PROGRAM_TYPES) )
    return(program_types[value]);
  else
    return(program_types[NUM_PROGRAM_TYPES]);
}


/*****************************************************************************/
/* Validates the program type number represented in the value string */

int  checkProgramType( char  *value,
                       char  *err_msg)

{
  const char  *fnct_name = "checkProgramType():";
  int    program_type_num;
  int    strlength;
  char  *s;

  trim(value);
  s = value;
  strlength = strlen(value);

  if (strlength < 1)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Improper length <%d> for string %s!\n",
                fnct_name, strlength, value);
      syslog(LOG_WARNING, "%s Improper length <%d> for string %s!",
             fnct_name, strlength, value);
      return(0);
    }
  
  while (*s)
    {
      if (!isdigit((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "%s Non-digit encountered in string! -> %s\n",
                    fnct_name, s);
          syslog(LOG_WARNING, "%s Non-digit encountered in string! -> %s",
                    fnct_name, s);
          return(0);
        }
      s++;
    }

  program_type_num = atoi(value);
  if ( (program_type_num < 0) || (program_type_num >= NUM_PROGRAM_TYPES) )
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Program type number (%d) isout of range!\n",
                fnct_name, program_type_num);
      syslog(LOG_WARNING, "%s Program type number (%d) isout of range!",
                fnct_name, program_type_num);
      return(0);
    }

  return(1);
}


/*****************************************************************************/

char  *getProgramAccess( int           value,
                         const char   *group,
                         AppItem     **Apps,
                         int           num_apps )
{
  char  *str;
  int  i, app_id, mask;
  int  leng = 0;

  if (value & NTCSS_BATCH_USER_BIT)
    {
      if (leng <= 0)
        leng = strlen("Batch User");
      else
        leng += strlen(",Batch User");
    }

  app_id = -1;
  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(Apps[i]->progroup_title, group) == 0)
        {
          app_id = i;
          break;
        }
    }
  if (app_id == -1)
    {
      if (leng == 0)
        {
          leng = strlen("No Access Allowed");
          str = (char *) malloc(sizeof(char)*(leng + 1));
          strcpy(str, "No Access Allowed");
        }
      return(str);
    }

  mask = 2;  /* FUTURE CHANGE to 1 upon role revisions */

  /* for each app role starting with 2'nd bit... */
  for (i = 0; i < Apps[app_id]->num_roles; i++) 
    {
      mask = mask << 1;    /* multiply by two  */
      if (value & mask)
        {
          if (leng == 0)
            leng = strlen(Apps[app_id]->roles[i]);
          else
            leng += strlen(Apps[app_id]->roles[i]) + 1;
        }
    }

  if (leng <= 0)
    {
      leng = strlen("No Access Allowed");
      str = (char *) malloc(sizeof(char)*(leng + 1));
      strcpy(str, "No Access Allowed");
      return(str);
    }

  str = (char *) malloc(sizeof(char)*(leng + 1));
  str[0] = 0;

  if (value & NTCSS_BATCH_USER_BIT)
    {
      if (str[0] == 0)
        strcpy(str, "Batch User");
      else
        strcat(str, ",Batch User");
    }

  mask = 2;
  for (i = 0; i < Apps[app_id]->num_roles; i++)
    {
      mask = mask << 1;   /* multiply by two */
      if (value & mask)
        {
          if (str[0] == 0)
            {
              strcpy(str, Apps[app_id]->roles[i]);
            }
          else
            {
              strcat(str, ",");
              strcat(str, Apps[app_id]->roles[i]);
            }
        }
    }
  str[leng] = 0;

  return(str);
}


/*****************************************************************************/

int  getAppInfo( char        *query,
                 const char  *db_dir )

{
  const char  *fnct_name = "getAppInfo():";
  AppItem  **Apps;
  int  i;
  int  leng;
  int  lock_id;
  int  num_apps;
  FILE  *fp;

  fp = fopen(query, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Failed to open query file %s!", fnct_name,
             query);
      return(ERROR_CANT_CREATE_QUERY);
    }

  fclose(fp);
  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to obtain a Read Lock!", fnct_name);
       return(ERROR_CANT_ACCESS_DB);
    }

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  unsetReadDBLock(lock_id);

  if (num_apps < 0)
    {
      freeAppData(Apps, num_apps);
      syslog(LOG_WARNING, "%s Failed to find any application info!",
             fnct_name);
      return(ERROR_NO_GROUP_INFO_FOUND);
    }
  
  leng = bundleQuery(query, "I", num_apps);

  for (i = 0; i < num_apps; i++)
    leng += bundleQuery(query, "CC", Apps[i]->progroup_title,
                        Apps[i]->hostname);
  freeAppData(Apps, num_apps);

  return(leng);   /* everthing ok, app data is returned */
}


/*****************************************************************************/
/* Checks a progroup title for illegal characters and length. */

int  checkProgroupTitle(title, err_msg)

     char  *title;
     char  *err_msg;

{
  const char  *fnct_name = "checkProgroupTitle():";
  int  leng;
  char *s;

  trim(title);
  s = title;
  leng = strlen(title);

  if ( (leng < MIN_PROGROUP_TITLE_LEN) || (leng > MAX_PROGROUP_TITLE_LEN) )
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Progroup title length (%d) is out of range!\n",
                fnct_name, leng);
      syslog(LOG_WARNING, "%s Progroup title length (%d) is out of range!",
             fnct_name, leng);
      return(0);
    }

  if (!isalpha((int)title[0]))
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Progroup title \"%s\" contains numbers!\n",
                fnct_name, title);
      syslog(LOG_WARNING, "%s Progroup title \"%s\" contains numbers!",
             fnct_name, title);
      return(0);
    }

  while (*s)
    {
      if (!(isalnum((int)*s) || (*s == '_') || (*s == '-')) || (*s == ' '))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "%s Progroup title \"%s\" contains illegal "
                    "characters!\n", fnct_name, title);
          syslog(LOG_WARNING, "%s Progroup title \"%s\" contains illegal "
                 "characters!", fnct_name, title);
          return(0);
        }
      s++;
    }

  return(1);
}

/*****************************************************************************/
/* Checks to see if the specified hostname is recognized by this server. */

int  checkHost( const char  *host,
                const char  *db_dir )

{
  HostItem  **Hosts;
  int  i;
  int  num_hosts;

  Hosts = getHostData(&num_hosts, db_dir);
  for (i = 0; i < num_hosts; i++)
    {
      if (strcmp(host, Hosts[i]->hostname) == 0)
        {
          freeHostData(Hosts, num_hosts);
          return(1);
        }
    }
  freeHostData(Hosts, num_hosts);

  return(0);
}


/*****************************************************************************/
/* Checks a Unix group title for illegal characters and length. */

int  checkUnixGroup( char  *group,
                     char  *err_msg )

{
  const char  *fnct_name = "checkUnixGroup():";
  int  leng;
  char  *s;

  trim(group);
  s = group;
  leng = strlen(group);

  if (leng == 0)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Group name length is 0!\n", fnct_name);
      syslog(LOG_WARNING, "%s Group name length is 0!", fnct_name);
      return(1);
    }
  
  if (leng > MAX_UNIX_GROUP_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Group name <%s> length (%d) is too long!\n",
                fnct_name, group, leng);
      syslog(LOG_WARNING, "%s Group name <%s> length (%d) is too long!",
             fnct_name, group, leng);
      return(0);
    }

  while (*s)
    {
      if (isspace((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "%s Progroup title <%s> contains illegal "
                    "spaces!\n", fnct_name, group);
          syslog(LOG_WARNING, "%s Progroup title <%s> contains illegal "
                 "spaces!", fnct_name, group);
          return(0);
        }
      s++;
    }

  return(1);
}


/*****************************************************************************/
/* Checks an icon file name for illegal characters and length. */

int  checkIconFile( char  *file,
                    char  *err_msg )

{
  const char  *fnct_name = "checkIconFile():";
  int    leng;
  char  *s;

  trim(file);
  s = file;
  leng = strlen(file);

  if (leng == 0)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s File name length is 0!\n", fnct_name);
      syslog(LOG_WARNING, "%s File name length is 0!", fnct_name);
      return(2);     /* Warning value picked up by the calling program */
    }
  
  if (leng > MAX_ICON_FILE_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s File name <%s> length (%d) is too long!\n",
                fnct_name, file, leng);
      syslog(LOG_WARNING, "%s File name <%s> length (%d) is too long!",
             fnct_name, file, leng);
      return(0);
    }

/*
  while (*s)
    {
      if ( isspace((int)*s) )
        {
          if (err_msg != NULL)
            sprintf(err_msg, "%s File name <%s> contains illegal spaces!\n",
                    fnct_name, file);
          syslog(LOG_WARNING, "%s File name <%s> contains illegal spaces!",
                 fnct_name, file);
          return(0);
        }
      s++;
    }
*/
  
  return(1);  /* Successful call */
}


/*****************************************************************************/
/* Checks an icon index value for illegal characters and length. */

int  checkIconIndex( char  *value,
                     char  *err_msg )

{
  const char  *fnct_name = "checkIconIndex():";
  int   leng;
  char *s;

  trim(value);
  s = value;
  leng = strlen(value);

  if (leng < 1)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "%s Icon index length is %d!\n", fnct_name, leng);
      syslog(LOG_WARNING, "%s Icon index length is %d!\n", fnct_name, leng);
      return(0);
    }

  while (*s)
    {
      if (!isdigit((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "%s Icon string <%s> contains illegal "
                    "non-digits!\n", fnct_name, value);
          syslog(LOG_WARNING, "%s Icon string <%s> contains illegal "
                    "non-digits!\n", fnct_name, value);
          return(0);
        }
      s++;
    }

  return(1);
}


/*****************************************************************************/
/* Checks a version number string for illegal length. */

int  checkVersionNumber( char  *version,
                         char  *err_msg )

{
  int  leng;

  trim(version);
  leng = strlen(version);

  if (leng > MAX_VERSION_NUMBER_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkVersionNumber(): Version number \"%s\" 's "
                "length (%d) is too long!\n", version, leng);
      syslog(LOG_WARNING, "checkVersionNumber(): Version number \"%s\" 's "
             "length (%d) is too long!", version, leng);
      return(0);
    }

  return(1);
}


/*****************************************************************************/
/* Checks a location string for illegal characters and length. */

int  checkClientLocation( char  *location,
                          char  *err_msg )

{
  int  leng;
  char  *s;

  trim(location);
  s = location;
  leng = strlen(location);

  if (leng == 0)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkClientLocation(): Location name "
                "length is 0!\n");
      syslog(LOG_WARNING, "checkClientLocation(): Location name length is 0!");
      return(1);
    }

  if (leng > MAX_CLIENT_LOCATION_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkClientLocation(): Location name \"%s\" 's"
                " length (%d) is too long!\n", location, leng);
      syslog(LOG_WARNING, "checkClientLocation(): Location name \"%s\" 's"
             " length (%d) is too long!", location, leng);
      return(0);
    }

/*
  while (*s)
    {
      if (isspace((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkClientLocation(): Location name \"%s\" "
                    "contains illegal spaces!\n", location);
          syslog(LOG_WARNING, "checkClientLocation(): Location name \"%s\" "
                    "contains illegal spaces!", location);
          return(0);
        }
      s++;
    }
*/
  
  return(1);
}


/*****************************************************************************/
/* Checks a version number string for illegal length. */

int  checkServerLocation( char  *location,
                          char  *err_msg )

{
  int  leng;
  char  *s;

  trim(location);
  s = location;
  leng = strlen(location);

  if (leng == 0)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkServerLocation(): Empty string encountered!\n");
      syslog(LOG_WARNING, "checkServerLocation(): Empty string encountered!");
      return(1);
    }
  
  if (leng > MAX_SERVER_LOCATION_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkServerLocation(): Server location \"%s\" "
                " length (%d) is too long!\n", location, leng);
      syslog(LOG_WARNING, "checkServerLocation(): Server location \"%s\" "
             " length (%d) is too long!", location, leng);
      return(0);
    }

/*
  while (*s)
    {
      if (isspace((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkServerLocation(): Server location \"%s\" "
                    "contains illegal spaces!\n", location);
          syslog(LOG_WARNING, "checkServerLocation(): Server location \"%s\" "
                 "contains illegal spaces!", location); 
          return(0);
        }
      s++;
    }
*/
  
  return(1);
}


/*****************************************************************************/
/* Checks a program title string for illegal length. */

int  checkProgramTitle( char  *title,
                        char  *err_msg )

{
  int  leng;
  char  *s;

  trim(title);
  s = title;
  leng = strlen(title);

  if (leng < MIN_PROGRAM_TITLE_LEN || leng > MAX_PROGRAM_TITLE_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkProgramTitle(): Program title \"%s\" 's length"
                " (%d) is out of range!\n", title, leng);
      syslog(LOG_WARNING, "checkProgramTitle(): Program title \"%s\" 's length"
             " (%d) is out of range!", title, leng);
      return(0);
    }
  return(1);
}


/*****************************************************************************/
/* Checks a program file name string for illegal length. */

int  checkProgramFile( char  *file,
                       char  *err_msg )

{
  int  leng;
  char  *s;

  trim(file);
  s = file;
  leng = strlen(file);

  if (leng < MIN_PROGRAM_FILE_LEN || leng > MAX_PROGRAM_FILE_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkProgramFile(): Program file \"%s\" 's length"
                " (%d) is out of range!\n", file, leng);
      syslog(LOG_WARNING, "checkProgramFile(): Program file \"%s\" 's length"
             " (%d) is out of range!", file, leng);
      return(0);
    }

/*
  while (*s)
    {
      if (isspace((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkProgramFile(): Program file name \"%s\" "
                    "contains illegal spaces!\n", file);
          syslog(LOG_WARNING, "checkProgramFile(): Program file name \"%s\" "
                    "contains illegal spaces!", file);
          return(0);
        }
      s++;
    }
*/

  return(1);
}


/*****************************************************************************/
/* Checks command line arguments for proper length. */

int  checkCmdLineArgs( char *args,
                       char  *err_msg )

{
  int  leng;
  char  *s;

  trim(args);
  s = args;
  leng = strlen(args);

/**** This code is logically useless...
  if (leng == 0)
    {
      syslog(LOG_WARNING, "checkCmdLineArgs(): Argument length is 0.");
      return(1);
    }
****/
  if (leng > MAX_CMD_LINE_ARGS_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkCmdLineArgs(): Argument \"%s\" 's length"
                " (%d) is too long!\n", args, leng);
      syslog(LOG_WARNING, "checkCmdLineArgs(): Argument \"%s\" 's length"
             " (%d) is too long!", args, leng);
      return(0);
    }

  return(1);
}


/*****************************************************************************/
/* Checks classification strings for illegal characters and proper length. */

int  checkClassification( char  *value,
                          char  *err_msg )

{
  int  leng;
  char  *s;

  trim(value);
  s = value;
  leng = strlen(value);

  if (leng < 1)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkClassification(): Empty string encountered!\n");
      syslog(LOG_WARNING, "checkClassification(): Empty string encountered!");
      return(0);
    }

  while (*s)
    {
      if (!isdigit((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkClassification(): Cassification string "
                    "<%s> contains a non-digit! <%c>\n", value, *s);
          syslog(LOG_WARNING, "checkClassification(): Cassification string "
                 "<%s> contains a non-digit! <%c>", value, *s);
          return(0);
        }
      s++;
    }

  leng = atoi(value);
  if (leng < 0 || leng >= NUM_CLASSIFICATIONS)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkClassification(): Classification length (%d)"
                " for string <%s> is out of range!\n", leng, value);
      syslog(LOG_WARNING, "checkClassification(): Classification length (%d)"
             " for string <%s> is out of range!", leng, value);
      return(0);
    }
  
  return(1);
}


/*****************************************************************************/
/* Checks to see if the access role is valid  This code will only allow for 9 access roles. This may be a FUTURE CHANGE. */

int  checkProgramAccess( int    num_roles,
                         char  *value,
                         char  *err_msg )

{
/*int  leng; */
/*int  flag;     * used for access bit mask.  Always assume access_role > 0. */
  int  role_index = -1;             /* Access role bit position in mask.*/
  int  temp_access_bitmask;         /* temporary bit mask for bitwise OR-ing */
  int  final_role_mask = 0;         /* bitmasked integer to be returned */
  char  tmp_str[2];                 /* index of role bit to set */
  char  *role_char_ptr = value;

  trim(value);

  if (value[0] == '\0')
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkProgramAccess(): Empty role access string"
                " encountered!  Assigning a zero for access role.\n");
      syslog(LOG_WARNING, "checkProgramAccess(): Empty role access string"
             " encountered!  Assigning a zero for access role.");
      return(0);
    }

  if (*role_char_ptr == '0')
    {
      return(0);    /* User/program/progroup has no access so just return. */
    }

  while (*role_char_ptr != '\0')
    {
      if (!isdigit((int)*role_char_ptr))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkProgramAccess(): Access role character"
                    " (%c) is not a digit!  Assigning a zero for "
                    "access role.\n", *role_char_ptr);
          syslog(LOG_WARNING, "checkProgramAccess(): Access role character"
                 " (%s) is not a digit!  Assigning a zero for access role.",
                 *role_char_ptr);
          return(0);      /* just assigns a '0' (NO-ACCESS) role */
        }

      tmp_str[0] = *role_char_ptr;
      tmp_str[1] = '\0';

      /* Converting the role character into its integer form... */
      role_index = atoi(tmp_str);  /* A zero return == error! */
      
      if (role_index < 0 || role_index > num_roles)
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkProgramAccess(): Access role index (%d)"
                    " from string <%s>, is out of range!\n",
                    role_index, value);
          syslog(LOG_WARNING, "checkProgramAccess(): Access role index (%d)"
                 " from string <%s>, is out of range!", role_index, value);
          return(-3);
        }

/* The following shifted the bit once to skip over the app admin bit */
/* (index 0) and once more to skip over the Ntcss Admin bit (index 1). */
      temp_access_bitmask  = (1 << (role_index + 1));
      /**NOTE if the ammount of extra shift is removed.. the similar section*/
      /* of code in ensure_database_integrity() may need updating to match */
      /* it when shifting new roles around. */

      final_role_mask |= temp_access_bitmask;

/*
 * Check for comma or end-of-string.
 */
      role_char_ptr++;
      if (*role_char_ptr == '\0')
          return(final_role_mask);                        /* Finished parsing. */
      if (*role_char_ptr != ',')
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkProgramAccess(): Bad charater in role "
                    "access string(%s)!\n", role_char_ptr);
          syslog(LOG_WARNING, "checkProgramAccess(): Bad charater in role "
                 "access string(%s)!", role_char_ptr);
          return(-4);
        }
      role_char_ptr++;
    }            /* End of parsing loop */
  return(final_role_mask);
}


/*****************************************************************************/

int  checkNoYes( char  *value,
                 char  *err_msg )

{
  int  leng;
  char *s;

  trim(value);
  s = value;
  leng = strlen(value);

  if (leng < 1)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkNoYes(): Empty string encountered!\n");
      syslog(LOG_WARNING, "checkNoYes(): Empty string encountered!");
      return(0);
    }

  while (*s)
    {
      if (!isdigit((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkNoYes(): String "
                    "<%s> contains a non-digit! <%c>\n", value, *s);
          syslog(LOG_WARNING, "checkNoYes(): String "
                 "<%s> contains a non-digit! <%c>", value, s);
          return(0);
        }
      s++;
    }

  leng = atoi(value);
  if (leng < 0 || leng >= NUM_NO_YES_VALUES)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkNoYes(): Value (%d) for string <%s>"
                " is out of range!\n", leng, value);
      syslog(LOG_WARNING, "checkNoYes(): Value (%d) for string <%s>"
             " is out of range!", leng, value);
      return(0);
    }

  return(1);
}


/*****************************************************************************/

int  checkPredefinedJobClass( char  *value,
                              char  *err_msg )

{
  int  leng;
  char  *s;

  trim(value);
  s = value;
  leng = strlen(value);

  if (leng < 1)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkPredefinedJobClass(): Empty string"
                " encountered!\n");
      syslog(LOG_WARNING, "checkPredefinedJobClass(): Empty string"
             " encountered!");
      return(0);
    }

  while (*s)
    {
      if (!isdigit((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "checkPredefinedJobClass(): String "
                    "<%s> contains a non-digit! <%c>\n", value, *s);
          syslog(LOG_WARNING, "checkPredefinedJobClass(): String "
                 "<%s> contains a non-digit! <%c>", value, s);
          return(0);
        }
      s++;
    }

  leng = atoi(value);
  if (leng < 0 || leng >= NUM_PDJ_CLASSES)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkPredefinedJobClass(): Value (%d) for"
                " string <%s> is out of range!\n", leng, value);
      syslog(LOG_WARNING, "checkPredefinedJobClass(): Value (%d) for"
             " string <%s> is out of range!", leng, value);
      return(0);
    }
  
  return(1);
}


/*****************************************************************************/

int  checkWorkingDir( char  *dir,
                      char  *err_msg )

{
  int  leng;
  char  *s;

  trim(dir);
  s = dir;
  leng = strlen(dir);

  if (leng < MIN_WORKING_DIR_LEN || leng > MAX_WORKING_DIR_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkWorkingDir(): Improper size <%d> for"
                " string <%s>!\n", leng, dir);
      syslog(LOG_WARNING, "checkWorkingDir(): Improper size <%d> for"
             " string <%s>!", leng, dir);
      return(0);
    }

/* 
  while (*s)
    {
      if (isspace((int)*s))
        {
          if (err_msg != NULL)
            sprintf(err_msg, "(): Illegal space character found"
                    " in string <%s>!\n", dir);
          syslog(LOG_WARNING, "(): Illegal space character found"
                 " in string <%s>!", dir);
          return(0);
        }
      s++;
    }
*/
  
  return(1);
}


/*****************************************************************************/

int  updateAdminProcs( const char  *program,
                       int    pid,
                       const char  *db_dir)

{
  const char  *fnct_name = "updateAdminProcs():";
  int  lock_id;
  char  datafile[MAX_FILENAME_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not obtain database lock!", fnct_name); 
      return(ERROR_CANT_ACCESS_DB);
    }

  sprintf(datafile, "%s/%s", db_dir, "ADMINPROCS");
  fp = decryptAndOpen(datafile, decryptfile, "a");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, datafile);
      unsetWriteDBLock(lock_id);
      remove(decryptfile);
      return(ERROR_CANT_OPEN_DB_FILE);
    }
  fprintf(fp, "%s %d\n", program, pid);
  closeAndEncrypt(fp, decryptfile);
  (void) rename(decryptfile, datafile);
  (void) chmod(datafile, S_IRUSR | S_IWUSR);

  unsetWriteDBLock(lock_id);

  return(0);
}


/*****************************************************************************/

int  checkAdminProc( const char  *program,
                     int          pid,
                     const char  *db_dir )

{
  const char  *fnct_name = "checkAdminProc():";
  int  lock_id;
  char  datafile[MAX_FILENAME_LEN];
  char  line[MAX_ITEM_LEN];
  char  progpid[MAX_ITEM_LEN];
  FILE  *fp;
  char  decryptfile[MAX_FILENAME_LEN];

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not obtain database lock!", fnct_name); 
      return(ERROR_CANT_ACCESS_DB);
    }

  sprintf(datafile, "%s/%s", db_dir, "ADMINPROCS");
  fp = decryptAndOpen(datafile, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file %s!", fnct_name, datafile);
      unsetReadDBLock(lock_id);
      remove(decryptfile);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  sprintf(progpid, "%s %d", program, pid);
  while (fgets(line, MAX_ITEM_LEN, fp) != NULL)
    {
      if (strncmp(line, progpid, strlen(progpid)) == 0)
        {
          fclose(fp);
          remove(decryptfile);
          unsetReadDBLock(lock_id);
          return(1);
        }
    }
  fclose(fp);
  remove(decryptfile);
  unsetReadDBLock(lock_id);

  return(0);
}


/*****************************************************************************/

int  getAppProcessLimit( const char  *app,
                         const char  *db_dir )

{
  const char  *fnct_name = "getAppProcessLimit():";
  AppItem  **Apps;
  int  app_id = -1;
  int  i;
  int  num_apps;

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(Apps[i]->progroup_title, app) == 0)
        {
          app_id = i;
          break;
        }
    }

  if (app_id == -1)
    {
      syslog(LOG_WARNING, "%s Progroup <%s> does not exist!", fnct_name, app);
      freeAppData(Apps, num_apps);
      return(ERROR_NO_SUCH_GROUP);
    }

  i = Apps[app_id]->num_processes;
  freeAppData(Apps, num_apps);

  return(i);
}


/*****************************************************************************/

int  getHostProcessLimit( const char  *host,
                          const char  *db_dir )

{
  const char  *fnct_name = "getHostProcessLimit():";
  HostItem  **Hosts;
  int  host_id = -1;
  int  i;
  int  num_hosts;

  Hosts = getHostData(&num_hosts, db_dir);

  for (i = 0; i < num_hosts; i++)
    {
      if (strcmp(Hosts[i]->hostname, host) == 0)
        {
          host_id = i;
          break;
        }
    }

  if (host_id == -1)
    {
      syslog(LOG_WARNING, "%s Host <%s> does not exist!", fnct_name, host);
      freeHostData(Hosts, num_hosts);
      return(ERROR_NO_SUCH_HOST);
    }

  i = Hosts[host_id]->num_processes;
  freeHostData(Hosts, num_hosts);

  return(i);
}


/*****************************************************************************/
/* This function has been replaced by setAppProcessLimit() &
   getHostProcessLimit().  DEPRECATED
*/

int  setProcessLimit( const char  *entity,
                      int          proc_limit,
                      int          which,     /* 0 is app, 1 is host */
                      const char  *db_dir )

{
  if (which == 0)
    return(setAppProcessLimit(entity, proc_limit, db_dir));
  else
    return(setHostProcessLimit(entity, proc_limit, db_dir));
}


/*****************************************************************************/

int  checkCommonDbTag( char *tag )

{
  const char  *fnct_name = "checkCommonDbTag():";
  int  leng;
  char  *s;

  trim(tag);
  s = tag;
  leng = strlen(tag);

  if (leng < MIN_COMDB_TAG_LEN || leng > MAX_COMDB_TAG_LEN)
    {
      syslog(LOG_WARNING, "%s Empty string encountered!", fnct_name);
      return(0);
    }

  while (*s)
    {
      if (!(isalnum((int)*s) || (*s == '_') || (*s == '-') ||
            (*s == ',') || (*s == '.') || (*s == ' ')))
        {
          syslog(LOG_WARNING, "%s String <%s> contains an illegal character "
                 "<%c>!", fnct_name, tag, s);
          return(0);
        }
      s++;
    }
  return(1);
}


/*****************************************************************************/

int  checkCommonDbItem( char  *item )

{
  const char  *fnct_name = "checkCommonDbItem():";
  int  leng;
  char  *s;
  
  trim(item);
  s = item;
  leng = strlen(item);
  
  if (leng < MIN_COMDB_DATA_LEN || leng > MAX_COMDB_DATA_LEN)
    {
      syslog(LOG_WARNING, "%s String <%s> is not the proper length <%d>!",
             fnct_name, item, leng);
      return(0);
    }

  while (*s)
    {
      if (!isprint((int)*s))
        {
          syslog(LOG_WARNING, "%s String <%s> contains an unprintable "
                 "character <%c>!", fnct_name, item, s);
          return(0);
        }
      s++;
    }
  return(1);
}


/*****************************************************************************/

int  getChildPid( int          ppid,
                  const char  *file,
                  const char  *cmdline )

{
  const char  *fnct_name = "getChildPid():";
  int    i, j, n;
  int    cpid = -1;
  char   buf[MAX_CMD_LEN];
  char   cmd[MAX_CMD_LEN];            /* system() command argument */
  char   word[MAX_ITEM_LEN];
  char  *s;
  FILE  *fp;

  (void) unlink(file);
#ifdef VSUN
  sprintf(cmd, "csh -f -c \'/bin/ps -alx > %s\'", file);
#else
  sprintf(cmd, "csh -f -c \'/bin/ps -ef > %s\'", file);
#endif

  i = system(cmd);
  if (i <0 )
    syslog(LOG_WARNING, "%s Problems executing command <%s>!", fnct_name, cmd);

  fp = fopen(file, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file <%s>!", fnct_name, file);
      i = unlink(file);
      if (i != 0)
        {
          syslog(LOG_WARNING, "%s Problems unlinking file <%s>!", fnct_name,
                 file);
          return(-2);
        }
      return(-1);
    }

  while (fgets(buf, MAX_CMD_LEN, fp) != NULL)
    {
      i = 0;
      i = stripWord(i, buf, word);
      if (i == -1)
        continue;
#ifdef VSUN
      i = stripWord(i, buf, word);
      if (i == -1)
        continue;
#endif
      i = stripWord(i, buf, word);
      if (i == -1)
        continue;

      s = &word[0];
      n = 0;
      while (*s)
        {
          if (!isdigit((int)*s))
            n = 1;
          s++;
        }
      if (n)
        continue;

      j = atoi(word);
      i = stripWord(i, buf, word);
      if (i == -1)
        continue;
      s = &word[0];
      n = 0;
      while (*s)
        {
          if (!isdigit((int)*s))
            n = 1;
          s++;
        }
      if (n)
        continue;
      if (ppid == atoi(word))
        {
#ifdef VSUN
          cpid = j;
          break;
#else
          i = stripWord(i, buf, word);
          if (i == -1)
            continue;
          i = stripWord(i, buf, word);
          if (i == -1)
            continue;
          i = stripWord(i, buf, word);
          if (i == -1)
            continue;
          i = stripWord(i, buf, word);
          if (i == -1)
            continue;
          i = stripWord(i, buf, word);
          if (i == -1)
            continue;
          i = stripWord(i, buf, word);
          if (strncmp(word, cmdline, 20) == 0)
            {
              cpid = j;
              break;
            }
#endif
        }
    }

  fclose(fp);
  i = unlink(file);
  if (i != 0)
    {
      syslog(LOG_WARNING, "%s Problems unlinking file <%s>!", fnct_name, file);
    }
  return(cpid);
}


/*****************************************************************************/

int  checkPredefinedJobTitle( char  *title,
                              char  *err_msg )
     
{
  int  leng;
  char  *s;

  trim(title);
  s = title;
  leng = strlen(title);

  if (leng < MIN_PREDEFINED_JOB_LEN )
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkPredefinedJobTitle(): PDJ title <%s> is too "
                " short <%d>!\n", title, leng);
      syslog(LOG_WARNING, "checkPredefinedJobTitle(): PDJ title <%s> is too "
             " short <%d>!", title, leng);
      return(0);
    }
  if (leng > MAX_PREDEFINED_JOB_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkPredefinedJobTitle(): PDJ title <%s> is too "
                " long <%d>!\n", title, leng);
      syslog(LOG_WARNING, "checkPredefinedJobTitle(): PDJ title <%s> is too "
             " long <%d>!", title, leng);
      return(0);
    }
  
  return(1);
}


/*****************************************************************************/

int  checkCommandLine( char  *cmd_line,
                       char  *err_msg )

{
  int  leng;
  char  *s;

  trim(cmd_line);
  s = cmd_line;
  leng = strlen(cmd_line);

  if (leng <= 0 || leng > MAX_PROC_CMD_LINE_LEN)
    {
      if (err_msg != NULL)
        sprintf(err_msg, "checkCommandLine(): String <%s> is not the "
                "proper length <%d>!\n", cmd_line, leng);
      syslog(LOG_WARNING, "checkCommandLine(): String <%s> is not the "
             "proper length <%d>!", cmd_line, leng);
      return(0);
    }
  
  return(1);
}


/*****************************************************************************/

int  getAppLinkData( const char  *app,
                     char        *link_data,
                     const char  *db_dir )

{
  const char  *fnct_name = "getAppLinkData():";
  AppItem  **Apps;
  int i;
  int  num_apps;
  int  app_index;
  int  lock_id;

  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not obtain database lock!", fnct_name); 
      return(ERROR_CANT_ACCESS_DB);
    }

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);
  app_index = -1;
  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(Apps[i]->progroup_title, app) == 0)
        {
          app_index = i;
          break;
        }
    }

  if (app_index == -1)
    {
      syslog(LOG_WARNING, "%s Application <%s> does not exist!", fnct_name,
             app);
      freeAppData(Apps, num_apps);
      unsetWriteDBLock(lock_id);
      return(ERROR_NO_SUCH_GROUP);
    }

  strcpy(link_data, Apps[app_index]->link_data);
  decryptString(app, link_data); /* should use plainCrypt for this! */
  freeAppData(Apps, num_apps);

  unsetWriteDBLock(lock_id);

  return(strlen(link_data));
}


/*****************************************************************************/

int  setAppLinkData( const char  *hostname_or_appname,
                     const char  *link_data,
                     const char  *db_dir,
                     int          server_api_flag )

{
  const char  *fnct_name = "setAppLinkData():";
  int    i; 
  int    lock_id;
  int    num_ents;
  char   datafile[MAX_FILENAME_LEN];
  char   realfile[MAX_FILENAME_LEN];
  const char  *appname;
  const char  *hostname;
  FILE  *fp;
  AppItem  **Apps;
  
  lock_id = setWriteDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Could not obtain database lock!", fnct_name); 
      return(ERROR_CANT_ACCESS_DB);
    }
  
  Apps = getAppData(&num_ents, db_dir, WITH_NTCSS_GROUP);

  /****************
    For DLL, find hostname given an appname.
    For Server API, hostname is already given.
  *****************/

  if (server_api_flag)
      hostname = hostname_or_appname; /* hostname given */
  else
    {
      appname = hostname_or_appname; /* find hostname given an appname */
      hostname = NULL;
      i = -1;
      while (!hostname && (++i < num_ents))
        if (strcmp(Apps[i]->progroup_title, appname) == 0)
          hostname = Apps[i]->hostname;
    
      if (!hostname)
        {
          syslog(LOG_WARNING, "%s Progroup <%s> does not exist!", fnct_name,
                 appname);
          freeAppData(Apps, num_ents);
          unsetWriteDBLock(lock_id);
          return(ERROR_NO_SUCH_GROUP); 
        }
    }

  sprintf(realfile, "%s/%s", db_dir, tables[PROGROUPS_ID]);
  sprintf(datafile, "%s/%s.tmp", db_dir, tables[PROGROUPS_ID]);

  fp = fopen(datafile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file <%s>!", fnct_name, datafile);
      freeAppData(Apps, num_ents);
      unsetWriteDBLock(lock_id);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  for (i = 0; i < num_ents; i++)
    {
      if (strcmp(Apps[i]->hostname, hostname) == 0)
        {
          free(Apps[i]->link_data);
          Apps[i]->link_data = allocAndCopyString(link_data);
          encryptString(Apps[i]->progroup_title, Apps[i]->link_data);
        }
      Apps[i]->marker = 1;
    }

  writeAppData(Apps,num_ents,fp);

  closeAndEncrypt(fp, datafile);

  i = rename(datafile, realfile);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems renaming file <%s> to <%s>!", fnct_name,
           datafile, realfile);  
  i = chmod(realfile, S_IRUSR | S_IWUSR);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems changing permissions on file <%s>!",
           fnct_name, realfile);  
  freeAppData(Apps, num_ents);
 
 /*** No longer used... 
  i = timestampDatabase(db_dir);
  if (i != 0)
    syslog(LOG_WARNING, "setAppLinkData(): Problems time-stamping DB!");  
 ***/

  unsetWriteDBLock(lock_id);
  
  return(0);
}


/*****************************************************************************/

int  getVersionString( char  *version )

{
  const char  *fnct_name = "getVersionString():";
  char  filespec[100];
  FILE  *fp;

  sprintf(filespec, "%s/%s", NTCSS_HOME_DIR, NTCSS_VERSION_FILE);
  fp = fopen(filespec, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file <%s>!", fnct_name, filespec);
      return(0);
    }

  if (fgets(version, MAX_VERSION_NUMBER_LEN, fp) != NULL)
    {
      stripNewline(version);
      return(1);
    }

  return(0);
}


/*****************************************************************************/
/* set the process limit number for the specified Application */

int  setAppProcessLimit( const char  *progroup,
                         int          proc_limit,
                         const char  *db_dir )

{
  const char  *fnct_name = "setAppProcessLimit():";
  AppItem  **Apps;
  int  app_index = -1;        /* For saving an index into the appData array. */
  int  i;
  int  num_apps;              /* number of all Ntcss apps */
  FILE  *fp;
  char  tempfile[MAX_FILENAME_LEN];
  char  realfile[MAX_FILENAME_LEN];

  Apps = getAppData(&num_apps, db_dir, WITH_NTCSS_GROUP);

  for (i = 0; i < num_apps; i++)
    {
      if (strcmp(Apps[i]->progroup_title, progroup) == 0)
        {
          Apps[i]->marker = 0;
          app_index = i;
        }
      else
        Apps[i]->marker = 1;
    }
  if (app_index == -1)
    {
      syslog(LOG_WARNING, "%s: Progroup <%s> does notexist!", fnct_name,
             progroup);
      freeAppData(Apps, num_apps);
      return(ERROR_NO_SUCH_GROUP);
    }

/* Write out all changed data ... */

  sprintf(realfile, "%s/%s", db_dir, tables[PROGROUPS_ID]);
  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[PROGROUPS_ID]);

  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "%s Could not open file <%s>!", fnct_name, tempfile);
      freeAppData(Apps, num_apps);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  writeAppData(Apps, num_apps, fp);
  fprintf(fp, PROGROUPS_FMT,
          Apps[app_index]->progroup_title,
          Apps[app_index]->hostname,
          Apps[app_index]->icon_file,
          Apps[app_index]->icon_index,
          Apps[app_index]->version_number,
          Apps[app_index]->release_date,
          Apps[app_index]->client_location,
          Apps[app_index]->server_location,
          Apps[app_index]->unix_group,
          proc_limit,                             /* New process limit */
          Apps[app_index]->link_data);
  closeAndEncrypt(fp, tempfile);
  freeAppData(Apps, num_apps);

  i = rename(tempfile, realfile);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems renaming file <%s> to <%s>!", fnct_name,
           tempfile, realfile);  
  i = chmod(realfile, S_IRUSR | S_IWUSR);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems changing permissionson file <%s>!",
           fnct_name, realfile);  

 /**** No longer used..
  i = timestampDatabase(db_dir);
  if (i != 0)
    syslog(LOG_WARNING, "setAppProcessLimit(): Problems time-stamping DB!");  
 ****/

  return(0);  /* this should be changed some day to reflect any errors */
}


/*****************************************************************************/
/* set process limit number for the specified host */

int  setHostProcessLimit( const char  *host,
                          int          proc_limit,
                          const char  *db_dir )

{
  const char  *fnct_name = "setHostProcessLimit():";
  HostItem  **Hosts;
  int  host_index = -1;     /* For saving an index into the Hosts array. */
  int  i;
  int  num_hosts;              /* number of all Ntcss hosts */
  FILE  *fp;
  char  tempfile[MAX_FILENAME_LEN];
  char  realfile[MAX_FILENAME_LEN];

  Hosts = getHostData(&num_hosts, db_dir);
  /*  table_id = HOSTS_ID;*/

  for (i = 0; i < num_hosts; i++)
    {
      if (strcmp(Hosts[i]->hostname, host) == 0)
        {
          Hosts[i]->marker = 0;
          host_index = i;
        }
      else
        Hosts[i]->marker = 1;
    }

  if (host_index == -1)
    {
      freeHostData(Hosts, num_hosts);
      return(ERROR_CANT_DETERMINE_HOST);
    }

  sprintf(realfile, "%s/%s", db_dir, tables[HOSTS_ID]);
  sprintf(tempfile, "%s/%s.tmp", db_dir, tables[HOSTS_ID]);

  fp = fopen(tempfile, "w");
  if (fp == NULL)
    {
      freeHostData(Hosts, num_hosts);
      return(ERROR_CANT_OPEN_DB_FILE);
    }

  writeHostData(Hosts, num_hosts, fp);
  fprintf(fp, HOSTS_FMT,
          Hosts[host_index]->hostname,
          Hosts[host_index]->ip_address,
          proc_limit,                               /* New process limit */
      Hosts[host_index]->type,
      Hosts[host_index]->repl);
  freeHostData(Hosts, num_hosts);
  closeAndEncrypt(fp, tempfile);
  if (buildAndPushNtcssNisMaps() != 0)
    {
      syslog(LOG_WARNING, "%s buildAndPushNtcssNisMaps failed!", fnct_name);
    }

  i = rename(tempfile, realfile);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems renaming file <%s> to <%s>!", fnct_name,
           tempfile, realfile);  

  i = chmod(realfile, S_IRUSR | S_IWUSR);
  if (i != 0)
    syslog(LOG_WARNING, "%s Problems changing permissionson file <%s>!",
           fnct_name, realfile);  


  return(0);  /* this should be changed some day to reflect any errors */

}


/*****************************************************************************/

/* checkFileExistenceAndStats(strFilesToCheck, strOwner, nPermMask)
   This function takes either a single file or a multiple files
   separated by spaces and checks to make sure the files are 
   owned by strOwner and have the permissions specified by nPermMask.
   nPermMask is a mask created from the nine standard permission bits:
      S_IRUSR - User read
      S_IWUSR - User write
      S_IXUSR - User execute

      S_IRGRP - Group read
      S_IWGRP - Group write
      S_IXGRP - Group execute

      S_IROTH - Other read
      S_IWOTH - Other write
      S_IXOTH - Other execute

   Function returns 0 if ALL files match owner and permissions.
   Function returns -1 if  ANY file does not match owner or permissions.

   ex.
   if (checkFileExistenceAndStats("/etc/passwd /etc/hosts", "root", S_IRUSR | S_IRGRP | S_IROTH) != 0) {
      printf("The passwd or host file is either not owned by root or not readable by everyone!\n");
   } else {
      printf("The passwd and host file permissions and ownership are OK.\n");
   } 

***/


/*****************************************************************************/

int  checkFileExistenceAndStats( const char  *strFilesToCheck,
                                 const char  *strOwner,
                                 mode_t       nPermMask )

{
  const char  *fnct_name = "checkFileExistenceAndStats():";
  int    nCharIndex;
  int    nReturn;
  uid_t    nUId;
  char   strPath[MAX_FILENAME_LEN];
  struct stat     statStruct;
  struct passwd  *pPasswdStruct;

  nReturn = 0;
  pPasswdStruct = getpwnam(strOwner);
  if (!pPasswdStruct)
    {
      syslog(LOG_WARNING, "%s Failed to get UID for %s!", fnct_name, strOwner);
      return(-1);
    }
  
  nUId = pPasswdStruct->pw_uid;
  nCharIndex = parseWord(0, strFilesToCheck, strPath, PATH_SEP_CHAR_STRING);
  while (nCharIndex >= 0)
    {
      if (stat(strPath, &statStruct) != 0)
        {
          syslog(LOG_WARNING, "%s %s DOES NOT EXIST!", fnct_name, strPath);
          nReturn = -1;
          break;
        }

      if (statStruct.st_uid != nUId)
        {
          syslog(LOG_WARNING, "%s %s IS NOT OWNED BY %s!", fnct_name, strPath,
                 strOwner);
          nReturn = -1;
          break;
        }

      if (!(statStruct.st_mode & nPermMask))
        {
          syslog(LOG_WARNING, " %s PERMISSIONS ARE INCORRECT!", fnct_name,
                 strPath);
          nReturn = -1;
          break;
        }

      nCharIndex = parseWord(nCharIndex, strFilesToCheck, strPath,
                             PATH_SEP_CHAR_STRING);
    }                                                           /* END while */
  
  return(nReturn);
}


/*****************************************************************************/
/* checkPathExistence(strPathsToCheck)
   This function takes a single path or multiple paths separated by spaces and
   checks that each of the paths exists.  

   Function returns 0 if ALL paths checked exist.
   Function returns -1 if ANY path does not exit.
 */

int  checkPathExistence( const char  *strPathsToCheck )

{
  const char  *fnct_name = "checkPathExistence():";
  int    nCharIndex;
  int    nReturn;
  char   strPath[MAX_FILENAME_LEN];
  struct stat   statStruct;

  nReturn = 0;
  nCharIndex = parseWord(0, strPathsToCheck, strPath, PATH_SEP_CHAR_STRING);
  while (nCharIndex >= 0)
    {
      if (stat(strPath, &statStruct) != 0)
        {
          syslog(LOG_WARNING, "%s %s DOES NOT EXIST!", fnct_name, strPath);
          nReturn = -1;
          break;
        }
      nCharIndex = parseWord(nCharIndex, strPathsToCheck, strPath,
                             PATH_SEP_CHAR_STRING);
    }                                                /* END while(strRecord) */
  
  return(nReturn);
}


/************************************************************/
int GetProgGroupList(char* query)
{
  const char *fnct_name = "GetProgGroupList():";
  AppItem **Apps;
  int num_apps;
  AccessRoleItem **Roles;
  int num_roles;
  FILE *fp;
  char lpszLine[156], lpszAlloc[156];
  char strApp[MAX_PROGROUP_TITLE_LEN+1];
  int leng = 0,x;
  int i=0,lock_id,j,one_role;

  lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME);
  if (lock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to obtain a Read Lock!", fnct_name);
       return(ERROR_CANT_ACCESS_DB);
    }

  Apps = getAppData(&num_apps, NTCSS_DB_DIR, WITH_NTCSS_GROUP);
  Roles=getAccessRoleData(&num_roles,NTCSS_DB_DIR);

  unsetReadDBLock(lock_id);
  fp=fopen(query,"w");
  if (fp == NULL) {
      syslog(LOG_WARNING,"%s Cannot open query file <%s>",fnct_name,query);
      return(ERROR_NO_SUCH_FILE);
  }

  memset(strApp,'\0',MAX_PROGROUP_TITLE_LEN+1);

  leng=0;
  for (i=0;i<num_apps;i++) {
      
	leng += strlen(Apps[i]->progroup_title);
	leng += strlen(Apps[i]->hostname);
	leng += strlen(Apps[i]->version_number);
	leng += strlen(Apps[i]->release_date);

        leng += 4; /* commas */


	/* no comma here, let the roles put it in  */
	if (IsAppLocked(Apps[i]->progroup_title)) {
		leng += strlen("Locked");
	} else {
		leng += strlen("N/A");
	}

        leng++;  /* comma */

	one_role=0;
	for (j=0;j<num_roles;j++) {
		if (!strcmp(Apps[i]->progroup_title,Roles[j]->progroup_title)) {
			/* add one for comma */
			leng += strlen(Roles[j]->app_role)+1;
		}
	}
    }

  memset(lpszLine,'\0',156);
  memset(lpszAlloc,'\0',156);

  leng += bundleData(lpszAlloc,"I",leng);
  x = bundleData(lpszLine,"I",leng);


  fprintf(fp,"%s%s",lpszLine,lpszAlloc);

  i=0;
  for (i=0;i<num_apps;i++) {
      
	fprintf(fp,"%s,%s,%s,%s,",Apps[i]->progroup_title,
				Apps[i]->hostname,Apps[i]->version_number,
				Apps[i]->release_date);


	/* no comma here, let the roles put it in  */
	if (IsAppLocked(Apps[i]->progroup_title)) {
		fprintf(fp,"Locked");
	} else {
		fprintf(fp,"N/A");
	}

	one_role=0;
	for (j=0;j<num_roles;j++) {
		if (!strcmp(Apps[i]->progroup_title,Roles[j]->progroup_title)) {
			fprintf(fp,",%s",Roles[j]->app_role);
			/* add one for comma */
		}
	}
	fprintf(fp,"\n");
    }

  fclose(fp);

  freeAppData(Apps,num_apps);
  freeAccessRoleData(Roles,num_roles);
  return leng;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
