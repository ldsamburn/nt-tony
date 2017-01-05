
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
#include <string.h>     /* strcmp */
#include <sys/types.h>  /* for time_t */
#include <sys/time.h>
#include <syslog.h> 
#include <stdlib.h>     /* for div_t */
#include <dirent.h>     /* opendir() */

/** INRI INCLUDES **/

#include <LibNtcssFcts.h>

/** LOCAL INCLUDES **/
/** STATIC FUNCTION PROTOTYPES **/
/** STATIC VARIABLES **/
/** GLOBAL VARIABLES **/


/** FUNCTION DEFINITIONS **/


/*****************************************************************************/
/* Determines if the specified file is older than the specified time.
   NOTE! This function is not very accurate when using the month or year
    units, but is close enough for all pratical purposes. Use days or shorter
    units when in doubt.
   NOTE! Leap years and daylight savings adjustments are not made here.
   Unit names are sec, hr, day, mo, and yr.
*/

int  is_file_older_than( long  elapsed_time,/* units of time to compare with */
                         const char  *time_units,
                         const char  *filename )  /* target to find age for */
{
  const char  *fnct_name = "is_file_older_than():";
  struct stat  stat_buf;                   /* file statistics */
  struct tm  *tmp_tm_ptr;
  time_t  current_time_secs;  /* current time in seconds since epoch */
  long  file_age_secs;     /* seconds elapsed since last modification */
  int  current_time_mos;
  int  file_age_mos;
  int  file_age_years;
  int  i;
  int  last_mod_time_mos;    /* months since last file change */

  i = stat(filename, &stat_buf);
  if (i < 0)
    {
      syslog(LOG_WARNING, "%s Could not stat file %s!", fnct_name, filename);
      return(-1);  
    }

  current_time_secs = time(NULL);
  file_age_secs = (double) (current_time_secs - stat_buf.st_mtime);

  if (strcmp(time_units, "sec") == 0)
    {
      if (file_age_secs > elapsed_time)
        return(1);
      else
        return(0);
    }

  if (strcmp(time_units, "min") == 0)
    {
      if ( ldiv(file_age_secs, 60).quot > elapsed_time )
        return(1);
      else
        return(0);
    }

  if (strcmp(time_units, "hr") == 0)
    {
      if ( ldiv(file_age_secs, 3600).quot > elapsed_time )
        return(1);
      else
        return(0);
    }

  if (strcmp(time_units, "day") == 0)
    {
      if ( ldiv(file_age_secs, 86400).quot > elapsed_time )
        return(1);
      else
        return(0);
    }

/* Use months since year 1900 for base unit in month and year calcs.
   Will not work as expected when elapsed time falls over a month boundary!
     EG: March 31 to April 1 == 1 month!
*/

  tmp_tm_ptr = gmtime(&current_time_secs);
  current_time_mos = tmp_tm_ptr->tm_mon + (12 * tmp_tm_ptr->tm_year);

  tmp_tm_ptr = gmtime(&stat_buf.st_mtime);
  last_mod_time_mos = tmp_tm_ptr->tm_mon + (12 * tmp_tm_ptr->tm_year);

  file_age_mos = current_time_mos - last_mod_time_mos ;

  if (strcmp(time_units, "mo") == 0)
    {
      if (file_age_mos > elapsed_time)
        return(1);
      else
        return(0);
    }

  if (strcmp(time_units, "yr") == 0)
    {
      file_age_years = div(file_age_mos, 12).quot;
      if ( file_age_years >= elapsed_time )
        return(1);
      else
        return(0);
    }
  
  return(-2);  /* unknown units specified */
}


/*****************************************************************************/
/* Retrieves file information about the specified directory and puts it in an
   .INI file format.

  THIS FUNCTION IS NOT FINISHED YET!


*//*
   pathname - Name of the file/directory/link of which to search for sub-level
              components.
   target_buffer - (OPTIONAL) If not NULL, the resulting information is placed
                   in this buffer.
   buffer_size - Exact size of the target_buffer.  Ignored if target_buffer is
                 NULL.
   target_filename - (OPTIONAL) If not NULL, the resulting information is
                     written this file.
   sort_flag - (0 or 1 for now) If 1, the entries of pathname are sorted in
               alphabetical order.
   xtra_info_flag - (0 or 1 for now) If 1, a stat is done on each entry to
                    determine what sort of file it is (eg: directory, soft
                    link, hard link, or just a file.
*//*

int  GetDirEntries( const char  *pathname,
                    char        *target_buffer,
                    int          buffer_size,
                    const char  *target_filename,
                    int          sort_flag,        / * unimplemented yet * /
                    int          xtra_info_flag )

{
  const char  *fnct_name = "GetDirEntries():";
  const char  *info_str;
  DIR     *dir_ptr;
  dirent  *dir_member;
  int      first_flag;  

  if (target_buffer != NULL)
    target_buffer[0] = '\0';

  dir_ptr = opendir(pathname);
  if (dir_ptr == NULL)
    {
      syslog(LOG_WARNING, "%s Dir %s does not exist!", fnct_name, pathname);
      return(-1);
    }


/ *  TO DO:  Want to have something like this for the ini file...

    Maybe we should build a buffer string first and then spit it out to both
    places?


    [/h/NTCSSS/init_data]  <- section header is the path for this ini file..
    progrps.ini = file      <- for each directory entry..
    host_info = direc
    filename3 = link

    [progrps.ini]  <- AddFileDetailsToINI puts all this extra info here.
    size = 103984
    perms = -rwxr-x--x
    owner_uid = root
    owner_gid = sys
    created = 01/30/99:13:59
    modified = 01/30/99:01:05

* /

  WriteProfileSection(pathname, "", target_filename);

  dir_member = readdir(dir_ptr);
  first_flag = 1;
  while (dir_member != NULL)
    {

All this following logic should be redone.

      if (xtra_info_flag > 0)
	AddFileDetailsToINI(pathname, target_buffer, target_filename);
      else
        info_str = dir_member.d_name;     Just put in filename for now.

      if (target_filename != NULL)
        writeProfileString(pathname, dir_member.d_name, info_str,
                           target_filename);

      if (target_buffer != NULL)        
        {
          if (first_flag == 0)                    not the first filename..
            strcat(target_buffer, ",");           Add in delimeter.
          strcat(target_buffer, dir_member.d_name);
        }

      dir_member = readdir(dir_ptr);
      first_flag = 0;
    }

  closedir(dir_ptr);
  return(0);
}

****/

/*****************************************************************************/
/* Does a stat on a directory entry and records useful information about that
   entry to the provided buffer and/or ini filename.

static int  AddFileDetailsToINI(pathname, target_buffer, target_filename);


[progrps.ini]  <- AddFileDetailsToINI puts all this extra info here.
size = 103984
perms = -rwxr-x--x
owner_uid = root
owner_gid = sys
created = 01/30/99:13:59
modified = 

**/
