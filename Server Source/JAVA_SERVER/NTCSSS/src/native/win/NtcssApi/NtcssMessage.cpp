/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssMessage.cpp: implementation of the NtcssMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NtcssApi.h"
#include "NtcssMessage.h"
#include "NtcssServerSocket.h"
#include "NtcssError.h"
#include "LibFcts.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

NtcssMessage_class NtcssMessage;

static char   request_buffer[MAX_REQUEST_SIZE + 1];
static LPTSTR request_buffer_ptr;
static int    request_size;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NtcssMessage_class::NtcssMessage_class()
{
   accept_label       = NULL;
   reject_label       = NULL;
   response_size      = 0;
   response           = NULL;
   response_ptr       = NULL;
   response_allocated = FALSE;
}

NtcssMessage_class::~NtcssMessage_class()
{

}

/*****************************************************************************/

void  NtcssMessage_class::open(LPCTSTR host_name, int port, LPCSTR accept_label, 
                               LPCSTR reject_label, LPCSTR request_label, 
                               int response_size, int timeout_seconds)
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

void  NtcssMessage_class::close()

{
  free_response();
  NtcssServerSocket.close();
}

/*****************************************************************************/


void NtcssMessage_class::write(LPCSTR data, int slot_size )
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

void  NtcssMessage_class::read()

{
  char   msg_label[CMD_LEN + 1];
  int   bytes_to_read;
  bool   server_engaged = true;

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


void NtcssMessage_class::free_response()

{
  if (this->response_allocated)
    {
      free(this->response);
      this->response_allocated = FALSE;
    }
}


/*****************************************************************************/

void NtcssMessage_class::get_server_error()

{
  NtcssServerSocket.read(NtcssError.server_error_message, MAX_ERR_MSG_LEN);

  if (NtcssServerSocket.ok)
    NtcssError.set_error_code(ERROR_SERVER_ERROR);
  else
    NtcssError.set_error_code(ERROR_REQUEST_DENIED);
}


/*****************************************************************************/

void NtcssMessage_class::figure_response_size()

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

void NtcssMessage_class::allocate_response(int x)
{
  this->response_allocated = FALSE;
  this->response_ptr = (LPTSTR)malloc(x);
  this->response = this->response_ptr;
  if (this->response == 0)
    {
      NtcssError.set_error_code(ERROR_ALLOCATING_RESPONSE);
      return;
    }

  this->response_allocated = TRUE;
}

/*****************************************************************************/

bool NtcssMessage_class::received_null()
{
  if (this->response_ptr > this->response)
    return(*(this->response_ptr - 1) == 0);
  else
    return(*(this->response) == 0);
}


/*****************************************************************************/
