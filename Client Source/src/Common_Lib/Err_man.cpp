/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\err_man.cpp
//                                         
// WSP, last update: 9/16/96
//
// This is the implementation file for the CErrorManager routines.  
//
// See routine comments below and also the header file for details on 
// processing.
//
//----------------------------------------------------------------------

#include <stdafx.h>
#include <err_man.h>

/////////////////////////////////////////////////////////////////////////////
// CErrorManager constructors
// _____________

CErrorManager::CErrorManager() 
{
    m_showmsgs_on = TRUE; 
    m_last_err_buf[0] = '\0';
}



CErrorManager::CErrorManager(BOOL put_show_on)
{
    m_showmsgs_on = put_show_on;
    m_last_err_buf[0] = '\0';
}

    

/////////////////////////////////////////////////////////////////////////////
// RecordError
// ___________

void CErrorManager::RecordError(const char *err_str, BOOL append)
{
	int err_str_len = strlen(err_str);
									   // If not appending or appending
									   //  to an empty string
									   //
    if ((!append) || (m_last_err_buf[0] == '\0'))
    {
									   // If err_str will fit into 
									   //  m_last_err_buf ok then
									   //
		if (err_str_len <= (sizeof(m_last_err_buf)-1))
		{
			strcpy(m_last_err_buf,err_str);
		}
									   // Else fit in as much as possible
		else
		{	
			strncpy(m_last_err_buf, err_str, 
				    (sizeof(m_last_err_buf)-1));
			m_last_err_buf[(sizeof(m_last_err_buf)-1)] = '\0';
		}
	}
									   // If appending...
    else
    {
        int len = strlen(m_last_err_buf);

									   // Make sure m_last_err_buf is
									   //  not full, then stick on '\n'
									   //  and cat the string onto
									   //  m_last_err_buf
		if (len < sizeof(m_last_err_buf))
		{
			m_last_err_buf[len++] = '\n';
			m_last_err_buf[len] = '\0';
            strncat(m_last_err_buf,err_str,
			        (sizeof(m_last_err_buf)-len));
		}
									   // Else m_last_err_buf is full
		else
		{
			m_last_err_buf[sizeof(m_last_err_buf)-1] = '\0';
			m_last_err_buf[len] = '\0';
		}
    }
}




/////////////////////////////////////////////////////////////////////////////
// GetLastErrorMsgText 
// ___________________

BOOL CErrorManager::GetLastErrorMsgText(char *recv_buf_ptr, 
                                        int recv_buf_size)
{
    strncpy(recv_buf_ptr,m_last_err_buf, recv_buf_size);

    return(TRUE);
}




/////////////////////////////////////////////////////////////////////////////
// HandleError 
// ___________
//
// For now am handling error error by fancying it up with the given
// facility1, facility2 and msg_prefix strings and then using
// AfxMessageBox to display.  Note that ClearError called to reset when
// done.
//
//WSPTODO	- may want to log this? 


void CErrorManager::HandleError(CString facility1, CString facility2, 
                                CString msg_prefix)

{
    if (m_showmsgs_on)
    {
        CString msg(facility1);

        if (facility2.GetLength() > 0)
        {
            msg += CString(" / ");
            msg += CString(facility2);
        }
        msg += CString(" Error -->\n");

        if (msg_prefix.GetLength() > 0)
        {
            msg += CString(msg_prefix);
            msg += CString("\n");
        }

        msg += CString(m_last_err_buf);
               
        AfxMessageBox(msg, (MB_OK | MB_SYSTEMMODAL));

        ClearError();
    }
}