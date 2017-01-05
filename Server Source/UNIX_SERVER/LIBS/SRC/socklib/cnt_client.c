
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*********************************************************************/
/* Program: Connect Client socket, Anthony C. Amburn(INRI)           */
/* Descrip: This module connects the calling program to a comms      */
/*          socket as a client.                                      */
/*                                                                   */
/* Modification History:                                             */
/*                                                                   */
/* Date      Name                Description                         */
/*                                                                   */
/* 11/19/93  Anthony C. Amburn   Original Coding                     */
/*                                                                   */
/*********************************************************************/

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

/** INRI INCLUDES **/

#include <SockFcts.h>


#include "SockLibP.h"


/** FUNCTION PROTOTYPES **/

int connect_client(int, const char*, int);
static void cc_alarm(int);


/** STATIC VARIABLES **/

static int con_to;


/** FUNCTION DEFINITIONS **/

int connect_client(sock, hostname, port_num)
int sock, port_num;
const char *hostname;
{
  int rc;
  struct hostent *hp;
  struct sockaddr_in name;
  int soopts, bcflag;

    con_to=0;
    bcflag=0;
    if (!strcmp(hostname,"BROADCAST")) {
        bcflag=1;
	soopts=1; 
	if (setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&soopts,sizeof(soopts)) < 0)
        {
/*          printf("Error, Unable to set BROADCAST",errno); */
                        fflush(stdout);
                        return(-1);
        }
        name.sin_addr.s_addr = INADDR_BROADCAST;
    }

    hp = gethostbyname(hostname);
    if (!bcflag && hp == NULL) {
        hp=gethostbyaddr(hostname,strlen(hostname),AF_INET);
        if (hp == NULL) 
            /* rc=-1; */
                        return -1;
    }
    if (!bcflag)
        bcopy((char*)hp->h_addr,(char*)&name.sin_addr,
                      hp->h_length);
    name.sin_family = AF_INET;
#ifdef linux
  name.sin_port = ntohs(port_num);
#else
    name.sin_port = port_num;
#endif
    signal(SIGALRM,cc_alarm);
    alarm(5);
    if ((connect(sock,(struct sockaddr *)&name,sizeof(name)) < 0) || 
        (con_to)) {
/*      printf("Error Connecting to host %s\n",hostname); */
        rc=-1;
    } else
        rc=0;
    alarm(0);
    return(rc);
}



static void cc_alarm(int signo)
{
  signo=signo;
  con_to++;
}
