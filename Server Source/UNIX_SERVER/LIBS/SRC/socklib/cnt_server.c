
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*********************************************************************/
/* Program: Connect Server socket, Anthony C. Amburn                 */
/* Descrip: This module connects the calling program to a comms      */
/*          socket as a server.                                      */
/*                                                                   */
/* Modification History:                                             */
/*                                                                   */
/* Date       Name                Description                        */
/*                                                                   */
/* 11/19/93   Anthony C. Amburn   Original Coding                    */
/*                                                                   */
/*********************************************************************/

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>

/** INRI INCLUDES **/

#include <SockFcts.h>


/** FUNCTION PROTOTYPES **/

int  connect_server(int, const char*, int);


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/
/* Binds a socket to a port on the local host, usually for a server/daemon. 
 */

int  connect_server( int          sock,
                     const char  *hostname,   /* Not used. */
                     int          port_num )

{
  int                 rc;
#ifdef linux
    socklen_t         length;
#else
  int                 length;
#endif
  struct sockaddr_in  name;

  /* done only to get rid of compiler warning about unused parameters */
  hostname = hostname;

  rc = 0;

  name.sin_family = AF_INET;
  name.sin_addr.s_addr = INADDR_ANY;
  name.sin_port = port_num;

  if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
      /*printf("ERROR: unable to bind socket %d\n",errno); */
      rc = -1;
    }
  length = sizeof(name);
  if (getsockname(sock, (struct sockaddr *)&name, &length) < 0)
    {
      /* printf("ERROR: unable to get socket name \n"); */
      return(-2);
    }
  else
    listen(sock, 15);

  return(rc);
}


/*****************************************************************************/
