
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * get_master.c
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <SockFcts.h>
#include <Ntcss.h>
#include <NetSvrInfo.h>
#include <ExtraProtos.h>


void  master_alarm(int);


static jmp_buf env_alrm;


/*****************************************************************************/

void  master_alarm(int signo)
{
    signo=signo;
    longjmp(env_alrm, 1);
}


/*****************************************************************************/

int  getMasterServer( char  *name,
                      int    length )
{
    int sock, soopts;
    struct sockaddr_in to_addr;
  
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return(ERROR_CANT_TALK_TO_SVR);
  
    soopts = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&soopts,
                                                    sizeof(soopts)) < 0) {
        close(sock);
        
        return(ERROR_CANT_TALK_TO_SVR);
    }
  
    to_addr.sin_addr.s_addr = INADDR_BROADCAST;
    to_addr.sin_port = BC_SVR_PORT;
    to_addr.sin_family = AF_INET;
    if (sendto(sock, "NTCSS_QUERY", strlen("NTCSS_QUERY"), 0,
	       (struct sockaddr *) &to_addr,
	       sizeof(struct sockaddr_in)) < 0) {
        close(sock);
        return(ERROR_CANT_TALK_TO_SVR);
    }

    if (signal(SIGALRM, master_alarm) == SIG_ERROR) {
        close(sock);
        return(ERROR_CANT_TALK_TO_SVR);
    } 
    if (setjmp(env_alrm) != 0) {
        close(sock);
        return(ERROR_CANT_TALK_TO_SVR);
    }
    (void) alarm(5);
    if (recvfrom(sock, name, length, 0, NULL, NULL) < 0) {
        (void) alarm(0);
        close(sock);
        return(ERROR_CANT_TALK_TO_SVR);
    }
    (void) alarm(0);
    close(sock);

    return(0);
}


/*****************************************************************************/
