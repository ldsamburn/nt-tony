/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssError.h: interface for the NtcssError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTCSSERROR_H__89649C31_0932_11D6_B4ED_00C04F4D4DED__INCLUDED_)
#define AFX_NTCSSERROR_H__89649C31_0932_11D6_B4ED_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NtcssApiP.h"

/******************************************************************************
ERROR CODE DEFINITIONS
******************************************************************************/

#define NTCSS_NO_ERROR                     0
#define API_ERRORS                         0
#define ERROR_READING_SERVER               (API_ERRORS + 1)
#define ERROR_WRITING_REQUEST_TO_SERVER    (API_ERRORS + 2)
#define ERROR_READING_LABEL_FROM_SERVER    (API_ERRORS + 3)
#define ERROR_REQUEST_DENIED               (API_ERRORS + 4)
#define ERROR_SERVER_TIMED_OUT             (API_ERRORS + 5)
#define ERROR_SETTING_TIMEOUT_ALARM        (API_ERRORS + 6)
#define ERROR_GETTING_LABEL                (API_ERRORS + 7)
#define ERROR_GETTING_RESPONSE_SIZE        (API_ERRORS + 8)
#define ERROR_GETTING_MASTER_SERVER_NAME   (API_ERRORS + 9)
#define ERROR_GETTING_NTCSS_PROCESS_ID     (API_ERRORS + 10)
#define ERROR_GETTING_LOGIN_NAME           (API_ERRORS + 11)
#define ERROR_INITIALIZING_SERVER_SOCKET   (API_ERRORS + 12)
#define ERROR_BUFFER_TOO_SMALL             (API_ERRORS + 13)
#define ERROR_SERVER_ERROR                 (API_ERRORS + 14)
#define ERROR_GETTING_DATE_TIME            (API_ERRORS + 15)
#define ERROR_GETTING_HOST_DATA            (API_ERRORS + 16)
#define ERROR_GETTING_HOST_NAME            (API_ERRORS + 17)
#define ERROR_GETTING_SERVER_NAME          (API_ERRORS + 18)
#define ERROR_INVALID_SECURITY_LEVEL       (API_ERRORS + 19)
#define ERROR_INVALID_PRIORITY_CODE        (API_ERRORS + 20)
#define ERROR_GETTING_CURRENT_HOST_NAME    (API_ERRORS + 21)
#define ERROR_CONVERTING_IP_ADDRESS        (API_ERRORS + 22)
#define ERROR_INVALID_TYPE_OF_JOB          (API_ERRORS + 23)
#define ERROR_REQUEST_BUFFER_OVERFLOW      (API_ERRORS + 24)
#define ERROR_INVALID_POST_MESSAGE_DATA    (API_ERRORS + 25)
#define ERROR_OPENING_TEMP_FILE            (API_ERRORS + 26)
#define ERROR_WRITING_TO_TEMP              (API_ERRORS + 27)
#define ERROR_DURING_FTP_OF_MESSAGE_FILE   (API_ERRORS + 28)
#define ERROR_ALLOCATING_RESPONSE          (API_ERRORS + 29)
#define ERROR_ALLOCATING_PAD_STRING        (API_ERRORS + 30)
#define ERROR_ALLOCATING_MEMORY            (API_ERRORS + 31)
#define ERROR_INVALID_BATCH_USER           (API_ERRORS + 32)
#define ERROR_INVALID_NUMBER_OF_COPIES     (API_ERRORS + 33)
#define ERROR_PROCESS_LIST_SIZE_TOO_SMALL  (API_ERRORS + 34)
#define ERROR_GETTING_HOST_TYPE            (API_ERRORS + 35)
#define ERROR_GETTING_APPLICATION_HOST     (API_ERRORS + 36)
#define ERROR_GETTING_AUTH_SERVER          (API_ERRORS + 37)

class NtcssError_class  
{
public:	
	NtcssError_class();
	virtual ~NtcssError_class();

    bool    occurred();
    void    reset();
    void    set_error_code(int x);
    LPCTSTR get_error_message();

    char server_error_message[MAX_ERR_MSG_LEN + 1];

private:
	int  error_code;    
};

extern NtcssError_class NtcssError;

#endif // !defined(AFX_NTCSSERROR_H__89649C31_0932_11D6_B4ED_00C04F4D4DED__INCLUDED_)
