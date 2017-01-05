/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/************************************************************************************************************
  NtcssGetAppData.c
*************************************************************************************************************/

#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include "NtcssApi.h"

FILE *fp;

void call_NtcssGetAppData(void);
int main(void);

/************************************************************************************************************/

void call_NtcssGetAppData()
{
  char error_message[NTCSS_MAX_ERROR_MSG_LEN];
  char app_data_buffer[NTCSS_SIZE_APP_DATA];

  if (NtcssGetAppData(app_data_buffer,NTCSS_SIZE_APP_DATA)) 
    fprintf(fp,"app_data_buffer: %s\n",app_data_buffer);
  else if (NtcssGetLastError(error_message,NTCSS_MAX_ERROR_MSG_LEN))
    fprintf(fp,"%s\n",error_message);
  else
    fprintf(fp,"error and NTCSS error message not available\n");

  fflush(fp);
}


/************************************************************************************************************/

int main()
{
  sleep(5); /* give the server a chance to init this job */

  openlog("NtcssGetAppData.c", LOG_PID, LOG_LOCAL7);
  syslog(LOG_WARNING,"Starting test");

  fp = fopen("NtcssGetAppData.out","w");

  call_NtcssGetAppData();         

  fclose(fp);

  syslog(LOG_WARNING,"End of test");
  closelog();

  return(0);
}
