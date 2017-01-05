
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*********************************************************************/
/* Program: Read Socket, read_sock.c, Anthony C. Amburn (INRI)       */
/* Descr:   This program reads a given socket until data has been    */
/*          received or a timeout has occurred (If TO indicator is   */
/*          set).                                                    */
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>


/** INRI INCLUDES **/

#include <SockFcts.h>
#include <Ntcss.h>

/** FUNCTION PROTOTYPES **/

static void  on_alarm( int );
int  read_sock( int, char*, int, char, int );
int  read_stream( int, char*, int, char );
int  read_dgram( int, char*, int, char );


/** STATIC VARIABLES **/

static int  alarm_ind;


/** FUNCTION DEFINITIONS **/


/*****************************************************************************/
static void  on_alarm( int signo )

{
    signo=signo;
    alarm_ind = 1;
    return;
}


/*****************************************************************************/

int  read_sock(int   sock,
               char *buffer,
               int   size,
               char  to_ind,
               int   type)
{
  if (type == SOCK_STREAM)
    return(read_stream(sock, buffer, size, to_ind));
  else
    return(read_dgram(sock, buffer, size, to_ind));
}


/*****************************************************************************/

int  read_stream(int   sock,
                 char *buffer,
                 int   size,
                 char  to_ind)
{
  char *tptr;
  int   rc;
  int   blen;
  int   totlen;
  int   icnt;

  rc = 0;
  blen = 0;
  totlen = 0;
  tptr = buffer;

  /*
    read_size = size-20;


    if ((blen=read(sock,tptr,20)) < 20) {
        syslog(LOG_WARNING,"READ_STREAM:Got Error reading MT");
        return (-1);
    } else {
        tptr += 20;
        totlen=20;
    }
    */

  rc = totlen;
  while (totlen < size)
    {
      icnt = (size - totlen);

      alarm_ind = 0;
      if (to_ind)
        {
          signal(SIGALRM, on_alarm);
          alarm(20);
        }

      if ((blen = read(sock, tptr, icnt)) < 0)
        {
          if (alarm_ind == 1)
            syslog(LOG_WARNING, "READ_STREAM:Received alarm on read.");
          else
            syslog(LOG_WARNING, "READ_STREAM:Received unknown "
                   "interrupt - <%d>", errno);
          alarm_ind = 0;
          rc = -1;
          break;
        }
      else
        {
          if (to_ind)
            alarm(0);
          if (blen != 0)
            {
              totlen += blen;
              rc = totlen;
              if (tptr[blen-1] == NTCSS_NULLCH && totlen > 20)
                {
                  break;
                }
              tptr += blen;
            }
          else
            {
              break;
            }
        }
    } 
/*
     } while (blen != 0 && rc != -1 && totlen <= size && *(tptr-1) != NULL);

    close(msgsock);
    syslog(LOG_WARNING,"READ_STREAM:Returning : %d",rc);
*/
  return(rc);
}


/*****************************************************************************/

int  read_dgram( int   sock,
                 char *buffer,
                 int   size,
                 char  to_ind)
{
  char *tptr;
  int   rc;
  int   blen;
  int   totlen;

  rc = 0;
  blen = totlen = 0;
  tptr = buffer;

  alarm_ind = 0;
  if (to_ind)
    {
      signal(SIGALRM, on_alarm);
      alarm(10);
    }

  blen = recv(sock, tptr, size, 0);
  if (blen < 0)
    rc = errno*(-1);
  else
    rc = blen;

  alarm(0);
  close(sock);

  return(rc);
}


/*****************************************************************************/
