/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgDlvMan.h                    
//                                         
// This is the header file for CdllMsgDeliveryMan.
//----------------------------------------------------------------------


                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_MSGDLVMN_H_CLASS_
#define _INCLUDE_MSGDLVMN_H_CLASS_
		
#include "msgcore.h"
#include "session.h"
#include "msgdefs.h"


class CdllMsgDeliveryMan
{
// Constructors

public:

#ifdef _NTCSS_MSG_TRACING_

    CdllMsgDeliveryMan(BOOL trace_is_on = FALSE);

#else

    CdllMsgDeliveryMan();


#endif // _NTCSS_MSG_TRACING_

// Attributes

// Operations

// Overrides

// Implementation - protected data

protected:
	BOOL     m_bIsPartOfDll;

    Csession *m_session;

#ifdef _NTCSS_MSG_TRACING_

	BOOL m_bTraceIsOn;
	FILE *m_pTraceFile;

#endif // _NTCSS_MSG_TRACING_

// Implementation - internal functions

protected:
	void    FormatBasicSvrMsgRec(Sbasic_svrmsgrec *the_msg_buf);

	void    DealWithError(Csession *session, SDllMsgRec *msg,
						  char *err_msg);
									   
// Implementation - Messaging  

public:
    ~CdllMsgDeliveryMan();
                          
#ifdef NTCSS_DLL_BUILD

	BOOL DllDoMessage(SDllMsgRec *msg,
					  CNtcssBaseMsg *caller_msg);
#endif  // NTCSS_DLL_BUILD

	BOOL DoMessage(Csession *session,
				   SDllMsgRec *msg, 
				   CNtcssBaseMsg *caller_msg = NULL);



// Implementation - Message tracing routines 

protected:

#ifdef _NTCSS_MSG_TRACING_

	void TraceMsgStart();
	void TraceMsgStop();
	void TraceOutput(char *buf);
	void TraceShowMessage(SDllMsgRec *msg, 
						  CNtcssBaseMsg *caller);

#endif // _NTCSS_MSG_TRACING_
};

#endif // _INCLUDE_MSGDLVMN_H_CLASS_





