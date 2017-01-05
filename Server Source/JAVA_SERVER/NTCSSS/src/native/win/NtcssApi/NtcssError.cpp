/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssError.cpp: implementation of the NtcssError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NtcssApi.h"
#include "NtcssError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

NtcssError_class NtcssError;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NtcssError_class::NtcssError_class()
{
   error_code = NTCSS_NO_ERROR;
}

NtcssError_class::~NtcssError_class()
{

}

/*****************************************************************************/

bool NtcssError_class::occurred()
{
   return error_code != NTCSS_NO_ERROR;
}

/*****************************************************************************/

void NtcssError_class::reset()

{
   error_code = NTCSS_NO_ERROR;
}


/*****************************************************************************/

void NtcssError_class::set_error_code(int x)
{
   error_code = x;
} 

/*****************************************************************************/

LPCTSTR NtcssError_class::get_error_message()

{
  switch(error_code)
    {
    case NTCSS_NO_ERROR:
      return("No NTCSS Server API errors detected.");
    case ERROR_READING_SERVER:
      return("Error getting data from the NTCSS master server.");
    case ERROR_WRITING_REQUEST_TO_SERVER:
      return("Error sending data to the NTCSS master server.");
    case ERROR_READING_LABEL_FROM_SERVER:
      return("Error reading the label from the NTCSS master server.");
    case ERROR_REQUEST_DENIED:
      return("The request was denied by the NTCSS master server.");
    case ERROR_SERVER_TIMED_OUT:
      return("The NTCSS master server timed out.");
    case ERROR_SETTING_TIMEOUT_ALARM:
      return("Not able to set the timeout alarm.");
        case ERROR_GETTING_HOST_TYPE:
      return("Error getting server type for this host!");
    case ERROR_GETTING_LABEL:
      return("Error getting the label from the NTCSS master server.");
    case ERROR_GETTING_RESPONSE_SIZE:
      return("Error getting the size of the response message from the NTCSS "
             "master server.");
    case ERROR_GETTING_MASTER_SERVER_NAME:
      return("Error getting the master server's name.");
    case ERROR_GETTING_NTCSS_PROCESS_ID:
      return("Error getting the NTCSS process id.");
    case ERROR_GETTING_LOGIN_NAME:
      return("Error getting the login name.");
    case ERROR_INITIALIZING_SERVER_SOCKET:
      return("Error initializing the NTCSS master server socket.");
    case ERROR_BUFFER_TOO_SMALL:
      return("The calling application's buffer is too small.");
    case ERROR_SERVER_ERROR:
      return(server_error_message + NUMBER_HEADER_SIZE);
    case ERROR_GETTING_DATE_TIME:
      return("Error getting the date and time from the NTCSS master server.");
    case ERROR_GETTING_HOST_DATA:
      return("Error getting information about the NTCSS master server.");
    case ERROR_GETTING_HOST_NAME:
      return("Error getting the name of the host.");
    case ERROR_GETTING_SERVER_NAME:
      return("Error getting the name of the server.");
    case ERROR_INVALID_SECURITY_LEVEL:
      return("Invalid security level.");
    case ERROR_GETTING_CURRENT_HOST_NAME:
      return("Error getting the host name.");
    case ERROR_INVALID_PRIORITY_CODE:
      return("Invalid priority code.");
    case ERROR_CONVERTING_IP_ADDRESS:
      return("Error converting IP address to dot notation.");
    case ERROR_INVALID_TYPE_OF_JOB:
      return("Invalid type of job to start during initialization.");
    case ERROR_REQUEST_BUFFER_OVERFLOW:
      return("Overflow error while loading the message buffer.");
    case ERROR_INVALID_POST_MESSAGE_DATA:
      return("Invalid data received for posting message.");
    case ERROR_OPENING_TEMP_FILE:
      return("Error opening temporary file.");
    case ERROR_WRITING_TO_TEMP:
      return("Error writing to temporary file.");
    case ERROR_DURING_FTP_OF_MESSAGE_FILE:
      return("Error during FTP (net_copy) of message file.");
    case ERROR_ALLOCATING_RESPONSE:
      return("Error allocating memory for the server response.");
    case ERROR_ALLOCATING_PAD_STRING:
      return("Error allocating memory for padding the server request.");
    case ERROR_ALLOCATING_MEMORY:
      return("Error allocating memory.");
    case ERROR_INVALID_BATCH_USER:
      return("Invalid batch user.");
    case ERROR_INVALID_NUMBER_OF_COPIES:
      return("Invalid number of copies requested.");
    case ERROR_PROCESS_LIST_SIZE_TOO_SMALL:
      return("Process list size is too small.");
    case ERROR_GETTING_APPLICATION_HOST:
          return("Error loading/processing Ntcss INI file.");
    case ERROR_GETTING_AUTH_SERVER:
          return("Error retrieving user's auth server from NIS map");
    default:
      return("Unknown NTCSS Server API error.");
    }
}
