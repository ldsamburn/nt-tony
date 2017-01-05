/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\err_man.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the CErrorManager class which provides
// a buffer and primitives for working with a 'last error string'.
//
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef _INCLUDE_ERR_MGR_H_CLASS_
#define _INCLUDE_ERR_MGR_H_CLASS_


#include <ntcssdef.h>


class CErrorManager 
{
private:
    char m_last_err_buf[MAX_ERROR_MSG_LEN];
    BOOL m_showmsgs_on;
    
public:
                          
    CErrorManager();   

    CErrorManager(BOOL put_show_on); 
    
    ~CErrorManager() {};

    void ClearError()
        { m_last_err_buf[0] = '\0'; };

    BOOL ErrorHandlingStatus()
        { return(m_showmsgs_on); };

    BOOL GetLastErrorMsgText(char *recv_buf_ptr, int recv_buf_size);

    const char *GetLastErrorMsgText()
        { return(m_last_err_buf); };
  
    virtual void HandleError(CString facility1, CString facility2 = "",
                             CString msg_prefix = "");

    void RecordError(const char *err_str, BOOL append = FALSE); 

    void SetErrorHandling(BOOL turn_on_msgs)
        { m_showmsgs_on = turn_on_msgs; };
}; 

#endif  // _INCLUDE_ERR_MGR_H_CLASS_


