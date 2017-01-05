
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>


/** PROTOS **/

void  recv_to_alarm( int );
int  ReadGenericPacket( int, char *, int, int );
int  WriteGenericPacket(int, char *, int );


/*****************************************************************************/
int  ReadGenericPacket( int   sptr,
                        char *bptr,
                        int   size,
                        int   bsize )
{
  char *ptr;
  int tsize,ttotal;

    ptr=bptr;
    ttotal=0;
    do {
        if (size - ttotal >= bsize)
            tsize = bsize;
        else
            tsize = size - ttotal;

        signal(SIGALRM,recv_to_alarm);
        alarm(30);
        tsize = read(sptr, ptr, tsize);
        if (tsize <= 0) {
            return(-1);
        }
        alarm(0);
        ttotal += tsize;
        ptr += tsize;
    } while (ttotal < size);

    return(ttotal);
}


/*****************************************************************************/

int  WriteGenericPacket( int   sptr,
                         char *addr,
                         int   size )
{
int tsize;
char *tptr;
int ttotal;


    tptr=addr;
    ttotal=0;
    tsize=0;
    do {
        tsize=size - ttotal;
        if ((tsize=write(sptr,tptr,tsize)) < 0) {
            break;
        }
        tptr++;
        ttotal += tsize;
    } while (ttotal < size);

    return(ttotal);
}


/*****************************************************************************/

void recv_to_alarm( int signo )

{
    signo=signo;
    return;
}


/*****************************************************************************/
