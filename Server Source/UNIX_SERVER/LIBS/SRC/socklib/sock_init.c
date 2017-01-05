
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*********************************************************************/
/* PROGRAM: sockinit.c - Socket Handling routines.                   */
/* AUTHOR : Anthony C. Amburn                                        */
/* DESCR  : The routines contained within this file are a set of     */
/*          functions built to control TCP socket connections        */
/*          as a CLIENT or a  SERVER.                                */
/*                                                                   */
/* Modifcation History:                                              */
/* Date      Name                 Description                        */
/*                                                                   */
/* 11/19/93  Anthony C. Amburn    Original Coding                    */
/*                                                                   */
/*********************************************************************/

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

/** INRI INCLUDES **/

#include <SockFcts.h>

/** LOCAL INCLUDES **/

#include "SockLibP.h"


/** FUNCTION PROTOTYPES **/

int  sock_init(const char*, int, int, int);


/** STATIC VARIABLES **/

static char con_to;


/** FUNCTION DEFINITIONS **/

/*****************************************************************************/

int  sock_init( const char  *hostname,
                int    port_num,
                int    cl_srv,
                int    type )
{
  int rc, si_sock, ruflag;
  struct linger  slinger;

  rc = 0;
  con_to = 0;

  si_sock = socket(AF_INET, type, 0);
    
  if (si_sock < 0)
    {
      return(-1);
    }
    
  slinger.l_onoff = 1;
  slinger.l_linger = 0;
  /*  if (setsockopt(si_sock,SOL_SOCKET,SO_LINGER,&slinger,
      sizeof(slinger)) < 0) {
      close(si_sock);
      return(-1);
      }*/

  ruflag = 1;
  if (setsockopt(si_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&ruflag,
                 sizeof(ruflag)) < 0)
    {
      close(si_sock);
      return(-1);
    }

  rc = si_sock;
  if (cl_srv == CLIENT)
    {
      if(connect_client(si_sock,hostname,port_num) == -1) {
        close(si_sock);
        rc = -1;
      }
    }
  else
    {
      if (connect_server(si_sock, hostname, port_num) == -1) {
        close(si_sock);
        rc = -1;
      }
    }

  return(rc);
}


/*****************************************************************************/
