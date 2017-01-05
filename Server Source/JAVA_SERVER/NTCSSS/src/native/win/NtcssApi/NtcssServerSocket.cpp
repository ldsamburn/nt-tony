/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssServerSocket.cpp: implementation of the NtcssServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NtcssApi.h"
#include "NtcssServerSocket.h"
#include "NtcssError.h"
#include "LibFcts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

NtcssServerSocket_class NtcssServerSocket;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NtcssServerSocket_class::NtcssServerSocket_class()
{
   server_socket   = new CTimeoutSocket();
   timeout_seconds = 0;
   opened          = FALSE;
   ok              = FALSE;
   bytes_read      = 0;
   bytes_written   = 0; 
   host_name[0]     = '\0';  
}

NtcssServerSocket_class::~NtcssServerSocket_class()
{

}

/*****************************************************************************/

void NtcssServerSocket_class::open(LPCSTR host_name, int port, int timeout_seconds)
{
  
  if (strcmp(host_name, "$MASTER$") == 0)
    get_master_server(this->host_name);
  else
    if (strcmp(host_name, "$CURRENT$") == 0)
      {
        /*  Point to ourself */
        if (gethostname(this->host_name, MAX_HOST_NAME_LEN) != 0)
          NtcssError.set_error_code(ERROR_GETTING_CURRENT_HOST_NAME);
      }
    else
      memcpy(this->host_name, host_name, MAX_HOST_NAME_LEN);

  this->timeout_seconds = timeout_seconds;         

  (this->server_socket)->Create();
  if ((this->server_socket)->Connect(this->host_name, port) != 0)
    {
      this->opened = TRUE;
      this->ok     = TRUE;
    }
  else
    {
      this->opened = FALSE;
      this->ok     = FALSE;
    }

  if ((this->opened) == FALSE)
    NtcssError.set_error_code(ERROR_INITIALIZING_SERVER_SOCKET);


  return;
}

/*****************************************************************************/

void NtcssServerSocket_class::get_master_server(LPTSTR master_server)
{

  int  x;

  /* NOTE: First parameter is database dir, but get_master_server now
     uses the NTCSS custom NIS map to find out which server is master, so
     here we're just passing in an empty string for the database dir */

  x = LIB_get_master_server(master_server);
  if (x < 0)
    NtcssError.set_error_code(ERROR_GETTING_MASTER_SERVER_NAME);

  return;
}


/*****************************************************************************/

void  NtcssServerSocket_class::close()
{
  if (this->opened)
    (this->server_socket)->Close();

  this->opened = FALSE;
  this->ok = FALSE;

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_class::write(LPTSTR buffer, int n)
{
  this->ok = FALSE;

  (this->server_socket)->SetTimeoutLength(this->timeout_seconds);

  this->bytes_written = (this->server_socket)->Send(buffer, n, 0);

  if ((this->server_socket)->HadTimeout() == TRUE) 
     NtcssError.set_error_code(ERROR_SERVER_TIMED_OUT);

  if (NtcssError.occurred())
    return;

  this->ok = (this->bytes_written != SOCKET_ERROR);

 return;
}


/*****************************************************************************/

void  NtcssServerSocket_class::read(LPTSTR buffer, int n)
{
  this->ok = FALSE;

  (this->server_socket)->SetTimeoutLength(this->timeout_seconds);

  this->bytes_read = (this->server_socket)->Receive(buffer, n);

  if ((this->server_socket)->HadTimeout() == TRUE) 
     NtcssError.set_error_code(ERROR_SERVER_TIMED_OUT);

  if (NtcssError.occurred())
    return;

  this->ok = (this->bytes_read != SOCKET_ERROR);

  return;
}


/*****************************************************************************/
