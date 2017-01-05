/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\ntcssmsg.cpp
//
// WSP, last update: 9/16/96
//                                         
// This is the implementation file for the following classes:
//                  CNtcssBaseMsg
//                  CNtcssMEM_MSG_ALLOC_Msg
//
// See routine comments below and also the header file for details on 
// processing.
//
//----------------------------------------------------------------------

#include <stdarg.h>

#include "stdafx.h"

#include <ntcssmsg.h>
#include <privapis.h>
#include <ntcssapi.h>
#include <genutils.h>
#include <dbg_defs.h>
#include <kind_of.h>       



#ifdef NTCSS_DLL_BUILD               
                                       // If this built for the DLL
                                       //   then need to be able to
                                       //   make direct calls
#include <0_basics.h>
#include <msgdlvmn.h>

#endif // NTCSS_DLL_BUILD



//***************************************************************************
// CNtcssBaseMsg                                              
// _____________

///////////////////////////////////////////////////////////////////////////////
// CNtcssBaseMsg constructor and destructor
// ________________________________________

CNtcssBaseMsg::CNtcssBaseMsg()
{
                                       // Initialize message and tell
                                       //  error mgr not to do error
                                       //  message displays.
                                       //   
 memset(&m_the_msg_rec,0,sizeof(m_the_msg_rec));

 m_the_state = RAW;

#ifdef	NTCSS_DLL_BUILD
	m_errman = gbl_dll_man.getCurrentSession()->ssnErrorMan();
#else
	m_errman = new CErrorManager;
#endif

#ifdef	_NTCSS_MSG_TRACING_
     m_trace_this_msg = FALSE;
#endif 

}


CNtcssBaseMsg::~CNtcssBaseMsg()
{
#ifndef	NTCSS_DLL_BUILD
	if (m_errman != NULL)
		delete m_errman;
#endif
}



/////////////////////////////////////////////////////////////////////////////
// CNtcssBaseMsg::DoItNow
// ______________________
// 
// Compilation depends on setting of NTCSS_DLL_BUILD define.  If set then
// this is being compiled for use with the NTCSS DLL and so we can make
// direct calls, otherwise have to go through the NTCSS DLL external
// interface.


BOOL CNtcssBaseMsg::DoItNow()
{
                                       // First make sure state is ok
    if (m_the_state != READY_TO_GO)
    {
        m_errman->RecordError
                    ("Attempt to process message before it is ready");
        return(FALSE);
    }

    BOOL ok;					       
                                       // If this is a part of the NTCSS
                                       //   DLL then can just use
                                       //   a CdllMsgDeliveryMan
                                       //   directly
#ifdef NTCSS_DLL_BUILD

									   // CdllMsgDeliveryMan constructor 
									   //  depends on whether or not
									   //  _NTCSS_MSG_TRACING_
#ifdef _NTCSS_MSG_TRACING_

	CdllMsgDeliveryMan msg_dlv_man(m_trace_this_msg);

#else

	CdllMsgDeliveryMan msg_dlv_man;

#endif



    ok = msg_dlv_man.DllDoMessage(&m_the_msg_rec, this);
							

                                       // Otherwise use NtcssDoMessage API
                                       //
#else  // NTCSS_DLL_BUILD

									   // API is dependent on setting
									   //  of _NTCSS_MSG_TRACING_

#ifdef _NTCSS_MSG_TRACING_

    ok = NtcssDoMessage(&m_the_msg_rec, m_trace_this_msg);

#else

    ok = NtcssDoMessage(&m_the_msg_rec);

#endif // _NTCSS_MSG_TRACING_

#endif // NTCSS_DLL_BUILD
                                       // If ok then set state, log
                                       //  and tell caller ok
    if (ok)
    {
        SetMsgState(DONE_SENT);

        LogMsg();

        return(TRUE);
    }
    else
    {							      
                                       // If this is not a DLL message
                                       // then have to use API to get
                                       // error message and set m_errman
#ifndef NTCSS_DLL_BUILD

	char err_msg[MAX_ERROR_MSG_LEN];

	NtcssGetLastError(err_msg, MAX_ERROR_MSG_LEN);

	m_errman->RecordError(err_msg);


#endif
        return(FALSE);
    }
}



/////////////////////////////////////////////////////////////////////////////
// CNtcssBaseMsg::Reset
// ____________________
//
// Note that any memory allocated and pointed to by
// m_the_msg_rec.send_data_ptr or m_the_msg_rec.recv_data_chain_ptr
// should have been freed before this function is called.
//

void CNtcssBaseMsg::Reset()
{
    m_the_state =     RAW;

    m_the_msg_rec.server_select = NO_SERVER_CONNECTION;

    memset(m_the_msg_rec.server_name,' ',SIZE_HOST_NAME);
    m_the_msg_rec.port_num = -1;
    memset(m_the_msg_rec.ok_msg,' ',SIZE_RESPONSE_CODE);
    memset(m_the_msg_rec.bad_msg,' ',SIZE_RESPONSE_CODE);
    m_the_msg_rec.send_data_ptr = NULL;
    m_the_msg_rec.send_data_size = 0;
    m_the_msg_rec.recv_data_chain_ptr = NULL;
    m_the_msg_rec.recv_data_size = 0;

#ifdef	_NTCSS_MSG_TRACING_
    if (m_errman != NULL)
	{
		delete m_errman;
		m_errman = NULL;
	}
#endif
}



#ifdef _NTCSS_MSG_TRACING_ 




/////////////////////////////////////////////////////////////////////////////
// CNtcssBaseMsg::ShowFieldStrData
// ________________________________
//
// This function, used for debugging only, takes a pointer to a
// character buf and its length.  It does not alter either parameter
// and will return a pointer to a 'formatted' version of the field_buf,
// that is NULL-terminated.  This function used mostly for displaying
// structure char[] fields that are not necessarily null-terminated.
// Note that a static buffer defined in this function is used as the
// formatting area.
//

const char *CNtcssBaseMsg::ShowFieldStrData(const char *field_buf, 
                                            const int size_field_buf)
{
    static char formatted_buf[SIZE_BIG_GENBUF];

    formatted_buf[0] = '<';

    for (int i=1; i <= size_field_buf; i++)
        formatted_buf[i] = field_buf[i-1];

    formatted_buf[i++] = '>';
    formatted_buf[i] = '\0';

    return(formatted_buf);
}



/////////////////////////////////////////////////////////////////////////////
// TraceMsgStart
// _____________
//
// Opens msg tracing file for appending.

void CNtcssBaseMsg::TraceMsgStart()
{
	if (m_trace_this_msg)
		m_trace_file = fopen(NTCSS_MSG_TRACE_FILE, "a");

	if (m_trace_file != NULL)
	{
		; // Nothing for now
// WSPTODO - need to output date at start maybe put date
	}

	else
		m_trace_this_msg = FALSE;

}



/////////////////////////////////////////////////////////////////////////////
// TraceMsgStop
// ____________

void CNtcssBaseMsg::TraceMsgStop()
{
	if (m_trace_file != NULL)
	{
	    fclose(m_trace_file);
	}
	m_trace_file = NULL;
}



/////////////////////////////////////////////////////////////////////////////
// TraceOutput
// ___________
//
// Outputs to trace msg file.  Minor formatting done to try to make
// sure that lines are not longer than 80 chars

void CNtcssBaseMsg::TraceOutput(char *buf)
{
									   // If not tracing now then do
									   // nothing
	if (!m_trace_this_msg)
		return;

	int chars_to_print = strlen(buf);

									   // Easy if less than 80 chars
	if (chars_to_print < 80)
	{
		fprintf(m_trace_file, buf);
		return;
	}

	int idx_on = 0;
	BOOL done = FALSE;
	char tmp_ch1;
	char tmp_ch2;
									   // Otherwise output 80 chars
									   //  per line until all chars
									   //  from buf are written

	while (!done)
	{
		if (chars_to_print >= 80)
		{
			tmp_ch1 = *(buf + idx_on + 80);
			tmp_ch2 = *(buf + idx_on + 81);

			*(buf + idx_on + 80) = '\n';
			*(buf + idx_on + 81) = '\0';

			fprintf(m_trace_file, (buf+idx_on));

			*(buf + idx_on + 80) = tmp_ch1;
			*(buf + idx_on + 81) = tmp_ch2;

			idx_on += 80;
			chars_to_print -= 80;

			if (chars_to_print <= 0)
				done = TRUE;
		}
		else
		{
			*(buf + idx_on + chars_to_print)= '\n';
			*(buf + idx_on + chars_to_print + 1) = '\0';

			fprintf(m_trace_file, (buf+idx_on));

			done = TRUE;
		}
	}
}


#endif // _NTCSS_MSG_TRACING_



//***************************************************************************
//***************************************************************************
// CNtcssMEM_MSG_ALLOC_Msg
// _______________________

/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::CNtcssMEM_MSG_ALLOC_Msg constructor
// ________________________________________________

CNtcssMEM_MSG_ALLOC_Msg::CNtcssMEM_MSG_ALLOC_Msg()
{
    SetMsgCategory(MEM_MSG_ALLOC);
    
    m_memchain_ptr = new CMemChain(MsgRecvDataChainPtr());

                                       // Shouldn't be any links, here
                                       //  make sure of that
    m_memchain_ptr->TrashChainLinks(); 
}



/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::DoItNow
// ________________________________


BOOL CNtcssMEM_MSG_ALLOC_Msg::DoItNow()
{
    if (CNtcssBaseMsg::DoItNow())
    {
                                       // Reset our memory chain as
                                       //   may  have one or more new
                                       //   links after DoItNow is done

        m_memchain_ptr->Reset(MsgRecvDataChainPtr());

        m_memchain_ptr->PrepareToByteChain();
    
        return(TRUE);
    }
    else
        return(FALSE);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
const CString CNtcssMEM_MSG_ALLOC_Msg::GetServerResponse() //added 11/11/99 to get response on good message
{
	//Gets a raw (unbundled) response
	CString strResponse=_T("");
	if(!m_memchain_ptr->TakeChainBytes(strResponse.GetBuffer(512),512))
		return _T("");
	strResponse.ReleaseBuffer();
	return strResponse;
}

/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::EmptyMsg
// _____________________________________

BOOL CNtcssMEM_MSG_ALLOC_Msg::EmptyMsg()
{

    BOOL ok = TRUE;


#ifdef NTCSS_DLL_BUILD               
    delete m_memchain_ptr;
#else
	NtcssDoMessageCleanup(MsgRecordPtr());
#endif

    m_memchain_ptr = NULL;

    Reset();

    return(ok);
}
           


/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::LogMsg
// _________________________________
//
// WSPTODO - nothing for now

BOOL CNtcssMEM_MSG_ALLOC_Msg::LogMsg()
{
	return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::ShowMsg
// _________________________________
//
// WSPTODO - nothing for now
//

void CNtcssMEM_MSG_ALLOC_Msg::ShowMsg(CString *output_str)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::TakeNextBytes
// ______________________________________

BOOL CNtcssMEM_MSG_ALLOC_Msg::TakeNextBytes(char *dest, int num_bytes)
{
    if (MsgState() != DONE_SENT)
        return(FALSE);

    BOOL ok = (m_memchain_ptr->TakeChainBytes(dest,num_bytes)
               == num_bytes);

    return(ok);
}


/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg::UnBundle
// _________________________________

BOOL CNtcssMEM_MSG_ALLOC_Msg::UnBundle(char  *key, ... )
{


    if (MsgState() != DONE_SENT)
    {
		TRACE0(_T("UnBundle failing, MsgState() != DONE_SENT\n"));
        return FALSE;
    }

    BOOL ok = TRUE;
    char  *key_item_on = key;
    int   *int_data;
    char  *char_data;
    char  *date_data;
    char  char_tmpbuf[SIZE_SMALL_GENBUF];
    
    int  number,ret=0;
    
    va_list(marker);
    va_start(marker,key);

	TRACE1(_T("UnBundle: key is_> <%s>\n"),key);

    while ((ok) && (*key_item_on != NULL))
    {

       switch (*key_item_on)
        {
			case POINTER_KEY:
		//this is a new type it's needed because the size could exced the "c"
		//method of dealing with characters - it will ONLY work if it's the 
		//the last value in a message - we then unload up to INT_MAX  characters
		//since we don't how many characters are unloaded, we use the return value
		//of TakeChainBytes which is num_bytes_to_get (INT_MAX) - bytes_still_needed
		//to null terminate the buffer. INT_MAX -> 2147483647

			
			char_data = va_arg(marker, char *);
			ret=m_memchain_ptr->TakeChainBytes
                                        (char_data,INT_MAX);
					
                   
             *(char_data+ret) = '\0';

                break;

            case CHAR_STR_KEY:

				TRACE0(_T("Unbundling character string value"));

                char_data = va_arg(marker, char *);

                ok = (m_memchain_ptr->TakeChainBytes
                                        (char_tmpbuf,CHAR_FMTSTR_LEN)
                      == CHAR_FMTSTR_LEN);
                
                if (ok)
                {
                    char_tmpbuf[CHAR_FMTSTR_LEN] = '\0';
                    number = atoi(char_tmpbuf);


					TRACE1(_T("Character string being UnBundled has length "
							  "of %d\n"),number);

                    ok = (m_memchain_ptr->TakeChainBytes
                                        (char_data,number)
                          == number);

                    if (ok)
                    {
                        *(char_data+number) = '\0';
						TRACE1(_T("UnBundled string is <%s>\n"),char_data);
                    }

                }
                break;
                
            case INT_KEY:

				TRACE0(_T("Unbundling integer value\n"));

                int_data = va_arg(marker, int *);
                ok = (m_memchain_ptr->TakeChainBytes
                                    (char_tmpbuf,INT_FMTSTR_LEN)
                      == INT_FMTSTR_LEN);

                if (ok)
                {
                    char_tmpbuf[INT_FMTSTR_LEN] = '\0';
                    number = atoi(char_tmpbuf);

					TRACE1(_T("Integer string being UnBundled has "
						      "length of %d\n"),number);


					ok = (m_memchain_ptr->TakeChainBytes
                                                (char_tmpbuf,number)
                          == number);

                                   
                    char_tmpbuf[number] = '\0';

					TRACE1(_T("Integer string is <%s>\n"),char_tmpbuf);

                    *int_data = atoi(char_tmpbuf);

					TRACE1(_T("Converted integer value is %d\n"),*int_data);
                }

                break;
                
            case DATE_KEY:

				TRACE0(_T("Unbundling date value"));

                date_data = va_arg(marker, char *);
                ok = (m_memchain_ptr->TakeChainBytes
                                    (date_data,DATE_FMTSTR_LEN)
                      == DATE_FMTSTR_LEN);

                if (ok)
                    *(date_data+DATE_FMTSTR_LEN) = '\0';


				TRACE1(_T("UnBundled date string is <%s>\n"),date_data);


                break;

            default:

				TRACE1(_T("Error, bad key value: %c\n"),*key_item_on);

                 ok = FALSE;

                break;
        }
        key_item_on++;
    }
    va_end(marker);
    

    if (!ok)
    {

		TRACE0(_T("Error encountered in UnBundle\n"));
		MsgErrorMan()->RecordError("Data unbundling error");

    }
	else

		TRACE0(_T("UnBundle terminating successfully\n"));
    
    return(ok);
}
