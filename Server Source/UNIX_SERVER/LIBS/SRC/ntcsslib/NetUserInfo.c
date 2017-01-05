
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* NetUserInfo.c */

/*  *NOTE* This file was only intended to contain functions which retrieve user
    info from other servers, as opposed to the local server.
*/

/** SYSTEM INCLUDES **/

#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

/** INRI INCLUDES **/


#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <SockFcts.h>
#include <StringFcts.h>

/** LOCAL INCLUDES **/
/** STATIC FUNCTION PROTOTYPES **/
/** STATIC VARIABLES **/


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
/* Sends a message to the IP given to see if the user is still logged into the
   client.  It user a token validate message to do so.
   Returns negative on error, 1 if user is still logged in on client, 0 if not.
*/

int  IsUserAtIP( const char  *login_name,
                 const char  *client_ip,
                 int          name_len)

{
  const char  *fnct_name = "IsUserAtIP():";
  int    udp_sock;
  char   msg[CMD_MESSAGE_LEN + 1];
  char   user[64];


  udp_sock = sock_init(client_ip, UDP_CLT_PORT, 0, SOCK_DGRAM);

  if (udp_sock < 0)
    {
      syslog(LOG_WARNING, "%s UDP socket initialize error to IP %s!  Err %i.",
             fnct_name, client_ip, udp_sock);
      return(-1);
    }

  strcpy(msg, "VALIDATETOKEN");
  if (write(udp_sock, msg, strlen(msg) + 1) < 0)
    {
      syslog(LOG_WARNING, "%s Failed to write to UDP socket! -- %s", fnct_name,
             strerror(errno));
      close(udp_sock);
      return(-2);
    }

  if (read_sock(udp_sock, msg, CMD_LEN + name_len, USE_TIME_OUT, SOCK_DGRAM)
      < (CMD_LEN + name_len))
    {
      syslog(LOG_WARNING, "%s Failed to read from UDP socket! -- %s",
             fnct_name, strerror(errno));
      close(udp_sock);
      return(-3);
    }
  close(udp_sock);

  if (strncmp(msg, "VALIDATETOKENREPLY", strlen("VALIDATETOKENREPLY")) != 0)
    {
      syslog(LOG_WARNING, "%s Read incorrect message from UDP socket!",
             fnct_name);
      return(-4);
    }
  
  ncpy(user, &msg[CMD_LEN], name_len);
  trim(user);
  if (strcmp(login_name, user) == 0)
    return(1);

  return(0);

}

/*****************************************************************************/
