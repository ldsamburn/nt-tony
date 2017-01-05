/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\ntcssmsg.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the following classes:
//                  CNtcssBaseMsg
//                  CNtcssMEM_MSG_ALLOC_Msg
//                  CNtcssMEM_MSG_ADVALLOC_Msg
//
// CNtcssBaseMsg provides the base functionality for an NTCSS socket
// message.  It was thought that we may have some messages which buffer
// to memory (CNtcssMEM_MSG_ALLOC_Msg) and others to disk (haven't had
// to do this one yet).  
// 
// CNtcssBaseMsg contains a SDllMsgRec and provides access functions 
// to read/write to the structure.  Also provided is function DoItNow
// which makes the message happen (using calls provided by the NTCSS DLL).
// 
// CNtcssMEM_MSG_ALLOC_Msg is derived from CNtcssBaseMsg and uses
// a the CMemChain class to buffer message data received from the server.
// 
// CntcssMEM_MSG_ADVALLOC_Msg is derived from CNtcssMEM_MSG_ALLOC_Msg
// and is designed for messages that indicate in advance the number of bytes
// that are to be received from the server.
// 
// See implementation file for details.  Also further details are
// provided below with the class declarations.
//
// Also see closely coupled classes and declarations in files:
//		...\comn_src\include\msgdefs.h
//		...\comn_src\include\msgcore.h   
//		...\comn_src\ntcssdll\dll_msgs.h
//		...\comn_src\ntcssdll\msgdlvmn.h
//----------------------------------------------------------------------

#ifndef _INCLUDE_NTCSSMSG_H_CLASS_
#define _INCLUDE_NTCSSMSG_H_CLASS_

#include <sizes.h>
#include <ntcssdef.h>
#include <ntcssapi.h>
#include <msgcore.h>
#include <memchain.h>
#include <genutils.h>
#include <err_man.h>
#include <dbg_defs.h>


/////////////////////////////////////////////////////////////////////////////
// CNtcssBaseMsg
// _____________
/*
This is the base class for NTCSS messaging.  It is an abstract class
to be built on by other classes.  The primary responsibility of this 
class is in implementation of DoItNow which calls the NTCSS DLL
NtcssDoMessage function. 
*/

class CNtcssBaseMsg 
{
private:
    EmsgState         m_the_state;
    SDllMsgRec        m_the_msg_rec;


	CErrorManager	  *m_errman;


protected:

    virtual BOOL EmptyMsg() = 0;

    virtual BOOL LogMsg()   = 0;

    TMemChainLink *MsgRecvDataChainPtr()
        { return((TMemChainLink *)
                  m_the_msg_rec.recv_data_chain_ptr); };

    CErrorManager *MsgErrorMan()
    	{ return(m_errman); };

    EmsgState MsgState()
        { return(m_the_state); };

    const SDllMsgRec *MsgRecordPtr()   
            { return(&m_the_msg_rec); };

    void Reset();

    void SetMsgBadMsg(const char *val)
        { strcpy(m_the_msg_rec.bad_msg,val); };

    void SetMsgCategory(EmsgCategory val)
        { m_the_msg_rec.msg_category = val; };

    void SetMsgOkayMsg(const char *val)
        { strcpy(m_the_msg_rec.ok_msg,val); };

    void SetMsgPortNum(int val)
        { m_the_msg_rec.port_num = val; };

    void SetMsgRecvDataChainPtr(TMemChainLink *val)
        { m_the_msg_rec.recv_data_chain_ptr = (void *)val; };

    void SetMsgRecvDataChainSize(int val)
        { m_the_msg_rec.recv_data_size = val; };

    void SetMsgSendDataPtr(char *val)
        { m_the_msg_rec.send_data_ptr = val; };

    void SetMsgSendDataSize(int val)
        { m_the_msg_rec.send_data_size = val; };

    void SetMsgServerName(const char *val)
        { strcpy(m_the_msg_rec.server_name,val); };

    void SetMsgServerSelect(EmsgServerChoices val)
        { m_the_msg_rec.server_select = val; };

    void SetMsgState(EmsgState given_state)
        { m_the_state = ( (m_the_state == DONE_SENT) 
                          ? (EmsgState)NULL 
                          : given_state); };
    
    void SetMsgValidityCheckValue(int val)
        { m_the_msg_rec.validity_check_value = val; };

#ifdef _NTCSS_MSG_TRACING_
    const char *ShowFieldStrData(const char *field_buf, 
    							 const int size_field_buf);
#endif

public:
    
    CNtcssBaseMsg();
        
    virtual ~CNtcssBaseMsg();
    
    virtual BOOL DoItNow();

	int GetMessageSize() {return m_the_msg_rec.recv_data_size;} //1/26/01

    virtual const char *GetLastErrorMsgText()
    	{ return(m_errman->GetLastErrorMsgText()); };

									   // For tracing support have
									   // flag to indicate if tracing
									   // currently on a a function
									   // that derived classes can use
									   // to set the flag.  Also
									   // functions for writing to
									   // trace file and a pure virtual
									   // ShowMsg function that must
									   // be implemented
#ifdef	_NTCSS_MSG_TRACING_
private:
    BOOL m_trace_this_msg;
	FILE *m_trace_file;

protected:
	void SetNtcssMsgTracing(BOOL tracing_on)
		{ m_trace_this_msg = tracing_on; };

	void TraceMsgStart();
	void TraceMsgStop();
	void TraceOutput(char *buf);

public:
    virtual void ShowMsg(CString *output_str)  = 0;

#endif // _NTCSS_MSG_TRACING_
};



/////////////////////////////////////////////////////////////////////////////
// CNtcssMEM_MSG_ALLOC_Msg
// _______________________

/*
This class builds onto CNtcssBaseMsg to implement message handling
using allocated memory buffers to store data received from the
server. Function emptyMessage is implemented to free up message buffers
when the message is deleted (emptyMessage called by CNtcssBaseMsg
destructor).  Also unBundle is provided to allow derived classes to
easily convert received message data into the appropriate types.

This is not an abstract class, however, it still needs derived classes
to deal with the details of setting up for and handling the completion
of messages.
*/

class CNtcssMEM_MSG_ALLOC_Msg : public CNtcssBaseMsg
{
private:
    CMemChain  *m_memchain_ptr;

protected:

    virtual BOOL EmptyMsg();

    virtual BOOL LogMsg();

    virtual void ShowMsg(CString *output_str);

    BOOL TakeNextBytes(char *dest, int num_bytes);

    BOOL UnBundle(char *key, ...);


public:

    CNtcssMEM_MSG_ALLOC_Msg(); 

	const CString GetServerResponse();


        
    ~CNtcssMEM_MSG_ALLOC_Msg()
    {
       if (MsgState() != RAW)
            EmptyMsg(); 
    };

    virtual BOOL DoItNow();
};


   
// CNtcssMEM_MSG_ADVALLOC_Msg
// __________________________

/*
This class builds onto CNtcssMEM_MSG_ALLOC_Msg message to handle
communication with the server in which the data to be received from
the server is variable in length and potentially large.  The only
real changes added by the CntcssMEM_MSG_ADVALLOC_Msg is in the setting
of the msgRecordWritablePtr()->msg_category to the MEM_MSG_ADVALLOC
value.  This should be done in the contructor and the load functions of
the derived classes.  Processing by the CdllMsgDeliveryMan will cause
all CntcssMEM_MSG_ADVALLOC_Msg messages to, after receiving initial
ok server response, first read an integer value telling how many
bytes are expected to be received.  Having this information ahead of
time helps in allocated the buffers and determining when the receive is
complete.
*/

class CNtcssMEM_MSG_ADVALLOC_Msg : public CNtcssMEM_MSG_ALLOC_Msg
{
private:



public:

    CNtcssMEM_MSG_ADVALLOC_Msg()
        {   SetMsgCategory(MEM_MSG_ADVALLOC); };
};


#endif  // _INCLUDE_NTCSSMSG_H_CLASS_+



