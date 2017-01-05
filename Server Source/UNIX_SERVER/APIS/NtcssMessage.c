/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  NtcssMessage.c
*/


/* System Headers */

#include <syslog.h>
#include <stdlib.h>
# include <string.h>


/* Local Headers */
#include <LibNtcssFcts.h>

#include "NtcssMacros.h"
#include "NtcssError.h"
#include "NtcssTimeoutAlarm.h"
#include "NtcssServerSocket.h"
#include "NtcssMessage.h"
#include "NtcssApiP.h"

/* Public Function Prototypes */

void  NtcssMessage_associate(NtcssMessage_class*);
void  NtcssMessage_open(const char*, int, const char*, const char*, 
                        const char*, int, int);
void  NtcssMessage_write(const char*, int);
void  NtcssMessage_read(void);
void  NtcssMessage_close(void);
int   NtcssMessage_timed_out(void);

/* Public Data */

NtcssMessage_class NtcssMessage = {
  (void (*)(void*))NtcssMessage_associate,
  NtcssMessage_open,
  NtcssMessage_write,
  NtcssMessage_read,
  NtcssMessage_close,
  NtcssMessage_timed_out,
  0,0,0,0,0,0};


/* Private Function Prototypes */

static void  figure_response_size(void);
static int   received_null(void);
static void  get_server_error(void);
/* static void  allocate_request(); */
static void  allocate_response(int);
/* static void  free_request(); */
static void  free_response(void);


/* Private Data */

static NtcssMessage_class *this = &NtcssMessage;
static char request_buffer[MAX_REQUEST_SIZE + 1];
static char *request_buffer_ptr;
static int request_size;

/* Public Function Definitions */

/*****************************************************************************/
void  NtcssMessage_associate(new_this)

     NtcssMessage_class *new_this;  
{
  this = new_this;
}


/*****************************************************************************/
void  NtcssMessage_open(host_name, port, accept_label, reject_label,
                        request_label, response_size, timeout_seconds)

     const char  *host_name;
     int   port;
     const char  *accept_label;
     const char  *reject_label;
     const char  *request_label;
     int   response_size;
     int   timeout_seconds;

{
  this->accept_label = accept_label;
  this->reject_label = reject_label;
  this->response_size = response_size;

  NtcssServerSocket.open(host_name, port, timeout_seconds);
  if (NtcssError.occurred())
    return;

  memset(request_buffer, '\0', REQUEST_LABEL_SIZE);
  strcpy(request_buffer, request_label);
  request_buffer_ptr = request_buffer + REQUEST_LABEL_SIZE;
  request_size = REQUEST_LABEL_SIZE;
}


/*****************************************************************************/

void NtcssMessage_write( data, slot_size )

     const char  *data;
     int   slot_size;
{

  /* space padded write */

  int   data_length = strlen(data);
  int   bytes_to_write = slot_size;
  int   bytes_to_pad = 0;

  if (NtcssError.occurred())
    return;

  if (data_length < slot_size)
    {
      bytes_to_write = data_length;
      bytes_to_pad = slot_size - data_length;
    }

  if (bytes_to_write)
    {
      if ( (request_size + bytes_to_write) >= MAX_REQUEST_SIZE )
        {
          NtcssError.set_error_code(ERROR_REQUEST_BUFFER_OVERFLOW);
          return;
        }

      memcpy(request_buffer_ptr, data, bytes_to_write);
      request_buffer_ptr += bytes_to_write;
      request_size += bytes_to_write;
    }

  if (bytes_to_pad)
    {
      if ((request_size + bytes_to_pad) >= MAX_REQUEST_SIZE)
        {
          NtcssError.set_error_code(ERROR_REQUEST_BUFFER_OVERFLOW);
          return;
        }

      memset(request_buffer_ptr, SPACE, bytes_to_pad);
      request_buffer_ptr += bytes_to_pad;
      request_size += bytes_to_pad;
    }
}


/*****************************************************************************/

void  NtcssMessage_read()

{
  char   msg_label[CMD_LEN + 1];
  int   bytes_to_read;
  int   server_engaged = True;

  if (NtcssError.occurred())
    return;

  *request_buffer_ptr = 0;
  request_size++;

  /* Write the request */

  NtcssServerSocket.write(request_buffer, request_size);
  if (NtcssServerSocket.bytes_written != request_size)
    {
      NtcssError.set_error_code(ERROR_WRITING_REQUEST_TO_SERVER);
      return;
    }

  /* Read the response */

  NtcssServerSocket.read(msg_label, CMD_LEN);
  if (NtcssServerSocket.bytes_read != CMD_LEN)
    {
      NtcssError.set_error_code(ERROR_READING_LABEL_FROM_SERVER);
      return;
    }

  msg_label[CMD_LEN] = '\0';

  if (strcmp(msg_label, this->accept_label) != 0)
    { 
      if (strcmp(msg_label, this->reject_label) == 0) 
        get_server_error();
      else 
        NtcssError.set_error_code(ERROR_GETTING_LABEL);
      return;
    }

  figure_response_size();
  if (NtcssError.occurred())
    return;

  bytes_to_read = this->response_size;

  if (bytes_to_read == 0)
    {
      allocate_response(1);
      *this->response = 0;
      return;
    }

  allocate_response(this->response_size);

  while (server_engaged)
    {
      NtcssServerSocket.read(this->response_ptr, bytes_to_read);
      if (! NtcssServerSocket.ok)
        {
          NtcssError.set_error_code(ERROR_READING_SERVER);
          return;
        }

      bytes_to_read -= NtcssServerSocket.bytes_read;
      this->response_ptr += NtcssServerSocket.bytes_read;
      server_engaged = (!received_null()) && (bytes_to_read > 0);
    }
}


/*****************************************************************************/

void  NtcssMessage_close()

{
  free_response();
  NtcssServerSocket.close();
}


/*****************************************************************************/

int  NtcssMessage_timed_out()

{
  return(NtcssTimeoutAlarm.tripped);
}


/*****************************************************************************/

/* Private Function Definitions */

/*****************************************************************************/

static void  figure_response_size()

{
  int   bytes_to_read;
  int   x;
  char   buffer[512];

  /* Use either the predetermined response size or get response size
     from the server. */

  if (this->response_size > 0)
    return; 

  NtcssServerSocket.read(buffer, 1);
  if (!NtcssServerSocket.ok)
    {
      NtcssError.set_error_code(ERROR_GETTING_RESPONSE_SIZE);
      return;
    }

  buffer[1] = '\0';
  bytes_to_read = atoi(buffer);

  if (bytes_to_read < 1)
    {
      this->response_size = 0;
      return;
    }
    
  NtcssServerSocket.read(&buffer[1], bytes_to_read);
  if (! NtcssServerSocket.ok)
    {
      NtcssError.set_error_code(ERROR_GETTING_RESPONSE_SIZE);
      return;
    }

  x = unbundleData(buffer, "I", &(this->response_size));
  if (x == -1)
    NtcssError.set_error_code(ERROR_GETTING_RESPONSE_SIZE);
}


/*****************************************************************************/

static int  received_null()

{
  if (this->response_ptr > this->response)
    return(*(this->response_ptr - 1) == 0);
  else
    return(*(this->response) == 0);
}


/*****************************************************************************/

static void  get_server_error()

{
  NtcssServerSocket.read(NtcssError.server_error_message, MAX_ERR_MSG_LEN);

  if (NtcssServerSocket.ok)
    NtcssError.set_error_code(ERROR_SERVER_ERROR);
  else
    NtcssError.set_error_code(ERROR_REQUEST_DENIED);
}


/*****************************************************************************/

static void  allocate_response(x)

     int   x;
{
  this->response_allocated = False;
  this->response_ptr = malloc(x);
  this->response = this->response_ptr;
  if (this->response == 0)
    {
      NtcssError.set_error_code(ERROR_ALLOCATING_RESPONSE);
      return;
    }

  this->response_allocated = True;
}


/*****************************************************************************/

static void  free_response()

{
  if (this->response_allocated)
    {
      free(this->response);
      this->response_allocated = False;
    }
}


/*****************************************************************************/
