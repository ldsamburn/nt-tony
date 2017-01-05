/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ...\comn_src\ntcssdll\msgdlvmn.h                    
//                                         
// 32-bit and 16-bit common header file
//
// This is the header file for CdllMsgDeliveryMan.
//----------------------------------------------------------------------

// msgdlvmn.h
//

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_MSGDLVMN_H_CLASS_
#define _INCLUDE_MSGDLVMN_H_CLASS_
		
#include "msgcore.h"
#include "session.h"
#include "msgdefs.h"


class CdllMsgDeliveryMan // : public CObject
{
private:
	BOOL     m_part_of_ntcss_dll;

    Csession *m_session;

#ifdef _NTCSS_MSG_TRACING_

	BOOL m_trace_is_on;
	FILE *m_trace_file;

#endif // _NTCSS_MSG_TRACING_

	void    FormatBasicSvrMsgRec(Sbasic_svrmsgrec *the_msg_buf);

	void    DealWithError(Csession *session, SDllMsgRec *msg,
						  char *err_msg);
									   
public:
                          
#ifdef _NTCSS_MSG_TRACING_


    CdllMsgDeliveryMan(BOOL trace_is_on = FALSE);

#else

    CdllMsgDeliveryMan();


#endif // _NTCSS_MSG_TRACING_
          
    ~CdllMsgDeliveryMan();

	BOOL DoMessage(Csession *session,
				   SDllMsgRec *msg, 
				   CNtcssBaseMsg *caller_msg = NULL);

#ifdef NTCSS_DLL_BUILD
	BOOL DllDoMessage(SDllMsgRec *msg,
					  CNtcssBaseMsg *caller_msg);
#endif  // NTCSS_DLL_BUILD

									   // _NTCSS_MSG_TRACING_ debugging
									   // routines

#ifdef _NTCSS_MSG_TRACING_

	void TraceMsgStart();
	void TraceMsgStop();
	void TraceOutput(char *buf);
	void TraceShowMessage(SDllMsgRec *msg, 
						  CNtcssBaseMsg *caller);

#endif // _NTCSS_MSG_TRACING_
};

#endif // _INCLUDE_MSGDLVMN_H_CLASS_





