
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * send_msg.c
 */

/** SYSTEM INCLUDES **/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/** INRI INCLUDES **/

#include <SockFcts.h>
#include <Ntcss.h>
#include <NetSvrInfo.h>

/** LOCAL PROTOS **/

static void  on_alarm( int );
static int   alarm_ind;


/*****************************************************************************/

int  send_msg( const char  *hostname,
               char  *message,
               const char  *label,
               int    label_leng )
{
  int sock;
  char totmsg[1000];

  memset(totmsg, ' ', label_leng);
  totmsg[label_leng] = 0;
  strcat(totmsg, message);
  memset(totmsg, '\0', label_leng);
  memcpy(totmsg, label, strlen(label));

  sock = sock_init(hostname, UDP_CLT_PORT, 0, SOCK_DGRAM);
  if (sock == -1)
    return(ERROR_CANT_TALK_TO_SVR);

  if ((send(sock, totmsg, label_leng + strlen(message) + 1, 0)) == -1) {
    close(sock);
    return(ERROR_CANT_TALK_TO_SVR);
  }
  close(sock);

  return(0);
}


/*****************************************************************************/

int  send_msg_with_response( const char  *hostname,
                             char  *message,
                             const char  *label,
                             char  *resp,
                             int    label_leng,
                             int    resp_len )
{
  int sock;
  char totmsg[1000];

  memset(totmsg, ' ', label_leng);
  totmsg[label_leng] = 0;
  strcat(totmsg, message);
  memset(totmsg, '\0', label_leng);
  memcpy(totmsg, label, strlen(label));

  sock = sock_init(hostname, UDP_CLT_PORT, 0, SOCK_DGRAM);
  if (sock == -1)
    return(ERROR_CANT_TALK_TO_SVR);

  if ((send(sock, totmsg, label_leng + strlen(message) + 1, 0)) == -1) {
    close(sock);
    return(ERROR_CANT_TALK_TO_SVR);
  }
    
  alarm_ind=0;
  signal(SIGALRM, on_alarm);
  alarm(15);

  if (recv(sock, resp, resp_len, 0) == -1) {
    close(sock);
    return(ERROR_CANT_TALK_TO_SVR);
  }
    
  close(sock);

  return(0);
}


/*****************************************************************************/

static void  on_alarm( int signo )

{
    signo=signo;
    alarm_ind = 1;
    return;
}


/*****************************************************************************/
