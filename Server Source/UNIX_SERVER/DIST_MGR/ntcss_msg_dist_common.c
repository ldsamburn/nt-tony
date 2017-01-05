
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


#include <stdlib.h>
#include <string.h>
#include "ntcss_msg_dist_common.h"


#define AUTHUSER_TIMEOUT             15
#define USERADMIN_TIMEOUT            15
#define PRINTADMIN_TIMEOUT           20
#define DEFAULT_TIMEOUT_VALUE        10
#define GETMASTERFILES_TIMEOUT_VALUE 10
#define NISUPDATE_TIMEOUT_VALUE      atoi(getNtcssSystemSettingTempPtr("NISUPDATE_TIMEOUT_VALUE", "15"))
#define DM_TIMEOUT_BUFFER             2

/*****************************************************************************/

int  getAlarmTimeoutValue( char *strMsg )

{
  const char *fnct_name = "getAlarmTimeoutValue():";

  if (strcmp(strMsg, MT_AUTHUSER) == 0)
    {
      return(AUTHUSER_TIMEOUT);
    }
    
  if (strcmp(strMsg, MT_USERADMIN) == 0)
    {
      return(USERADMIN_TIMEOUT);
    }

  if (strcmp(strMsg, MT_PRINTADMIN) == 0)
    {
      return(PRINTADMIN_TIMEOUT);
    }

  if (strcmp(strMsg, MT_GETMASTERFILES) == 0)
    {
      return(GETMASTERFILES_TIMEOUT_VALUE);
    }

  if (strcmp(strMsg, MT_NISMAPUPDATE) == 0)
    {
      return(NISUPDATE_TIMEOUT_VALUE);
    }

  syslog(LOG_WARNING, "%s Unknown message (%s)! Returning default timeout.",
	 fnct_name, strMsg);

  return(DEFAULT_TIMEOUT_VALUE);
    
}


/*****************************************************************************/
/* The DM adds a buffer to the timeout value to ensure the DPs have
   adequate time to timeout if necessary and perform their work before
   the DM alarm sounds. */

int  getDmAlarmTimeoutValue( char *strMsg )

{
  return(getAlarmTimeoutValue(strMsg) + DM_TIMEOUT_BUFFER);
} 


/*****************************************************************************/

int  getDpAlarmTimeoutValue( char *strMsg )

{
  return(getAlarmTimeoutValue(strMsg));
}


/*****************************************************************************/

int  removeAndCreatePipes( void )

{
  const char  *fnct_name = "removeAndCreatePipes():";
  struct stat  pipeStatus;


  /* Remove pipes if they already exist and then create the  pipes */
  /* This is done on startup to clean out any data in the pipes that may
     have been left due to problems */
  if (stat(APP_TO_DM_PIPE, &pipeStatus) == 0)
    {
      if (remove(APP_TO_DM_PIPE) != 0)
        {
          syslog(LOG_WARNING, "%s remove(APP_TO_DM_PIPE) failed!", fnct_name);
          return(-2);
        }
    }
  
  if (mkfifo(APP_TO_DM_PIPE, 0777) < 0)
    {
      syslog(LOG_WARNING, "%s mkfifo(APP_TO_DM_PIPE) failed!", fnct_name);
      return(-3);
    }
  
  if (stat(DM_TO_APP_PIPE, &pipeStatus) == 0)
    {
      if (remove(DM_TO_APP_PIPE) != 0)
        {
          syslog(LOG_WARNING, "%s remove(DM_TO_APP_PIPE) failed!", fnct_name);
          return(-4);
        }
    }

  if (mkfifo(DM_TO_APP_PIPE, 0777) < 0)
    {
      syslog(LOG_WARNING, "%s mkfifo(DM_TO_APP_PIPE) failed!", fnct_name);
      return(-5);
    }

  return(0);
}


/*****************************************************************************/

/*****************************************************************************/
