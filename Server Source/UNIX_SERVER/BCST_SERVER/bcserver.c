/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/**/ 
/**/ 
/*
 * bcserver.c
 */

/** SYSTEM INCLUDES **/
#include <syslog.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#ifndef HPUX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <termios.h>
#endif
#ifdef linux
#include <sys/ioctl.h>
#endif
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/** INRI INCLUDES **/

#include <ExtraProtos.h>

#include "bcserver.h"


static void  get_connection( void );
/* static int   send_alarm( void ); */
/* static void  zapped( void ); */

/** GLOBAL VARS **/
int   con_sock;
FILE *con_stream;
int   debug_flag = FALSE;
FILE *fp;
char  remote_host[100];


/*****************************************************************************/

int  main( int    argc,
           char **argv )

{

  /* very simple parse down of command line args */
  if(argc > 1)				/* do we have some args? */
  {
    if(strcmp(argv[1],"-d") == 0)	/* did we get the debug_flag flag? */
      debug_flag=TRUE;
    if(!debug_flag || argc > 2)		/* no debug or to many args, BARF! */
    {
      fprintf(stderr,"Usage: %s [-d]\n",argv[0]);
      exit(1);
    }
  }

  /* get the connection */
  get_connection();

  syslog(LOG_WARNING,"Done processing connection!");
  exit(0);

}


/*****************************************************************************/
/** Not currently used..
static int  get_line( char *line )

{
  char *ptr;
  int   size;

  ptr=line;
  do {
	if ((size = fread(ptr,1,1,con_stream)) != 1)  {
		return(NULL);
	}
  } while (*ptr++ != '\n');

  size=(ptr-line);
  return(size);
}
****/

/*****************************************************************************/
/* Establishes connections for inetd mode */

static void  get_connection( void )

{
#ifdef linux
  socklen_t client_len;
#else
  int client_len;
#endif
  struct hostent *hp,*myhp;
  struct sockaddr_in from_addr,to_addr,temp_addr;
  int incnt;
  char inbuf[100];
  char pbuf[400];
  char myhostname[100];
  char myhostip[100];
  FILE *pfp;
  char addr[MAX_IP_ADDRESS_LEN+1];
#ifndef HPUX
    int n;
#endif

  /* Not all systems pass fd's 1 and 2 from inetd */
  OPENLOG("BCSERVER",LOG_PID,LOG_LOCAL7);

  (void) dup(0);
  (void) dup(0);

  con_sock = 0;
  con_stream = stdin;

#if defined(HPUX)
    /*
     * setsid will give me a new session w/o any tty associated at all
     */
    setsid();
#else
    /*
     * TIOCNOTTY will get rid of my tty & set my pgrp to 0
     */
    if((n=open("/dev/tty",O_RDWR)) > 0)
    {
      ioctl(n, TIOCNOTTY, 0) ;
      close(n) ;
    }
#endif 

  client_len=sizeof(from_addr);
  if ((incnt=recvfrom(con_sock,inbuf,100,0,
      (struct sockaddr *)&from_addr,&client_len)) < 0) {
	syslog(LOG_WARNING,"Received no data.");
	exit(1);
  }

/*
  if ((pfp=popen("ps -ef | grep database_server | egrep -v grep","r")) < 0) {
*/
   memset(pbuf,NTCSS_NULLCH,400);
  /*if ((pfp=popen("cat /usr/local/NTCSSII/database/MASTER","r")) < 0) { */
  if ((pfp=popen("cat /usr/local/NTCSSII/database/MASTER","r")) == NULL) {
     syslog(LOG_WARNING,"Unable to Open PIPE %d",errno);
     exit(1);
  }
  
  if (strcmp(pbuf,"I AM MASTER") || fgets(pbuf,400,pfp)) {
#ifdef DEBUG
        syslog(LOG_WARNING,"Request found on Non-Master.");
#endif
	pclose(pfp);
	exit(0);
  }
  pclose(pfp);

  if ((gethostname(myhostname,100)) < 0) {
     syslog(LOG_WARNING,"Unable to get hostname %d",errno);
     exit(1);
  }

  if ((myhp=gethostbyname(myhostname)) == NULL) {
     syslog(LOG_WARNING,"Unable to get hostent %d",errno);
     exit(1);
  }
  bcopy(myhp->h_addr, (char *)&temp_addr.sin_addr,myhp->h_length);
  strcpy(myhostip, inet_ntoa(temp_addr.sin_addr));

#ifdef DEBUG
   syslog(LOG_WARNING,"My host IP is %s",inet_ntoa(temp_addr.sin_addr));
   syslog(LOG_WARNING,"My hostname is %s",myhostname);
#endif

  memset(addr, NTCSS_NULLCH, MAX_IP_ADDRESS_LEN+1);
#ifdef _IN_ADDR_T
  sprintf(addr, "%u", (unsigned int)from_addr.sin_addr.s_addr);
#else
  sprintf(addr, "%lu", from_addr.sin_addr.s_addr);
#endif
  /* if((hp=gethostbyaddr(&from_addr.sin_addr,sizeof(struct sockaddr_in),
   AF_INET)) == NULL) */
  if((hp=gethostbyaddr(addr,sizeof(addr), AF_INET)) == NULL) 
    strcpy(remote_host, inet_ntoa(from_addr.sin_addr));
  else
    strcpy(remote_host,hp->h_name);

#ifdef DEBUG 
     syslog(LOG_WARNING,"connection from %x - %s",from_addr.sin_addr,remote_host);
     syslog(LOG_WARNING,"to port %d",to_addr.sin_port);
#endif

#ifdef DEBUG
   syslog(LOG_WARNING,"data received -%s-",inbuf);
#endif

  bcopy((char *)&from_addr,(char *)&to_addr,sizeof(struct sockaddr_in));
  to_addr.sin_addr.s_addr=inet_addr(remote_host);

  if ((sendto(con_sock,myhostname,strlen(myhostname),0,
	      (struct sockaddr*)&to_addr,client_len)) < 0) {
     syslog(LOG_WARNING,"Cannot sendto -%d-",errno);
     exit(1);
  }

}


/*****************************************************************************/
/** Not currently used..
static void  zapped( void )
{
  syslog(LOG_WARNING,"Terminated by unexpect Signal!\n");
  if (con_stream)
  	fclose(con_stream);
  close(con_sock);
  exit(1);
}
*****/

/*****************************************************************************/
/** Not currently used..
static int  send_alarm( void )

{
#ifdef DEBUG
	syslog(LOG_WARNING,"WARNING:Timeout Alarm Writing to PRINTQ Server");
#endif
	return 0;
}
***/

/*****************************************************************************/
/*****************************************************************************/
