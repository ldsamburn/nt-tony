/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  NtcssServerSocket.c
  */

/*  System Headers */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>

/* Local Headers */

#include <LibNtcssFcts.h>
#include <SockFcts.h>
#include <ExtraProtos.h>

#include "NtcssMacros.h"
#include "NtcssError.h"
#include "NtcssTimeoutAlarm.h"
#include "NtcssServerSocket.h"

/* PUBLIC FUNCTION PROTOTYPES */

void NtcssServerSocket_open(const char*, int, int);
void NtcssServerSocket_read(char*, int);
void NtcssServerSocket_write(char*, int);
void NtcssServerSocket_close(void);
/* void NtcssServerSocket_open_master(); */
void NtcssServerSocket_get_master_server(char*);

/* PUBLIC DATA */

NtcssServerSocket_class NtcssServerSocket = {
  NtcssServerSocket_open,
  NtcssServerSocket_read,
  NtcssServerSocket_write,
  NtcssServerSocket_close,
  NtcssServerSocket_get_master_server,
  0, 0, 0, 0, 0, 0 ,{'\0'}};

/* LOCAL FUNCTIONS */

/* static void strip_newline(); */
static void set_status(void);

/* Globals */

static NtcssServerSocket_class *this = &NtcssServerSocket;

/* PUBLIC FUNCTIONS DEFINITIONS */

/*****************************************************************************/

void  NtcssServerSocket_open(host_name, port, timeout_seconds)

     const char  *host_name;
     int  port;
     int  timeout_seconds;

{
  if (strcmp(host_name, "$MASTER$") == 0) 
    NtcssServerSocket_get_master_server(this->host_name);
  else
    if (strcmp(host_name, "$CURRENT$") == 0)
      {
        /* Point to ourself */
        if (gethostname(this->host_name, MAX_HOST_NAME_LEN) != 0)
          NtcssError.set_error_code(ERROR_GETTING_CURRENT_HOST_NAME);
      }
    else
      memcpy(this->host_name, host_name, MAX_HOST_NAME_LEN);

  this->timeout_seconds = timeout_seconds;
  this->server_socket = sock_init(this->host_name, port, 0, SOCK_STREAM); 

  set_status();

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_read(buffer, n)

     char  *buffer;
     int   n;
{
  this->ok = False;

  NtcssTimeoutAlarm.activate(this->timeout_seconds);
  if (NtcssError.occurred())
    return;

  this->bytes_read = read(this->server_socket, buffer, n);

  NtcssTimeoutAlarm.deactivate();
  if (NtcssError.occurred())
    return;

  this->ok = (this->bytes_read > 0);

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_write(buffer, n)

     char  *buffer;
     int  n;
{
  this->ok = False;

  NtcssTimeoutAlarm.activate(this->timeout_seconds);
  if (NtcssError.occurred())
    return;

  this->bytes_written = write(this->server_socket, buffer, n);

  NtcssTimeoutAlarm.deactivate();
  if (NtcssError.occurred())
    return;

  this->ok = (this->bytes_written > 0);

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_close()

{
  if (this->opened)
    close(this->server_socket);
  this->opened = False;
  this->ok = False;

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_get_master_server(master_server)
     
     char  *master_server;
{
  int  x; 
  /* NOTE: First parameter is database dir, but get_master_server now
     uses the NTCSS custom NIS map to find out which server is master, so
     here we're just passing in an empty string for the database dir */
 
  x = get_master_server("", master_server); 
 if (x < 0)
   NtcssError.set_error_code(ERROR_GETTING_MASTER_SERVER_NAME);

 return;
}


/*****************************************************************************/

/*
static void  strip_newline(line)

     char  *line;
{
  int  last_character_position;

  last_character_position = (strlen(line) - 1);
  if (last_character_position >= 0)
    if (line[last_character_position] == '\n')
      line[last_character_position] = 0;

 return;
}
*/


/*****************************************************************************/

static void  set_status()

{
  if (this->server_socket >= 0)
    {
      this->opened = 1;
      this->ok = 1;
    }
  else
    {
      this->opened = 0;
      this->ok = 0;
    }  
  if ((this->opened) == 0)
    NtcssError.set_error_code(ERROR_INITIALIZING_SERVER_SOCKET);

 return;
}


/*****************************************************************************/







