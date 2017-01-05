/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  start_boot_jobs.c
  */

#include <syslog.h>
#include <unistd.h>

#include "NtcssApi.h"
#include "NtcssApiP.h"
#include "NtcssMacros.h"

extern BOOL  NtcssStartBootJobs(void);
int main(void);

int  main(/* argc, argv */)
/*
     int   argc;
     char   *argv[]; */
{
  char  error_message[MAX_ERR_MSG_LEN + 1];

  openlog("NTCSS START BOOT JOBS", LOG_PID, LOG_LOCAL7);

  syslog(LOG_WARNING, "Start of the NTCSS start boot jobs process.");

  if (!NtcssStartBootJobs())
    {
      if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
        syslog(LOG_WARNING,"%s",error_message);
      else
        syslog(LOG_WARNING, "Error starting the boot jobs and the "
               "NtcssGetLastError message is not available!");
  }

  syslog(LOG_WARNING, "End of the NTCSS start boot jobs process.");
  unlink("/h/NTCSSS/logs/NOLOGINS");

  return(0);
}





