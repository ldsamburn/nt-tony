/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssSock.h        
//                                         
// This is the header file for CNtcssSock, a CWinsock derived class.
// 
//----------------------------------------------------------------------

#ifndef _INCLUDE_NTCSSOCK_H_CLASS_
#define _INCLUDE_NTCSSOCK_H_CLASS_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "memchain.h"

class CNtcssSock;

#include "TimeoutSocket.h"
#define MAX_SOCKET_RECV_BLOCK_SIZE      8192


									   // Sock Header File must be 
									   //   included as extern "C"

/////////////////////////////////////////////////////////////////////////////
// CNtcssSock window

class CNtcssSock : public CTimeoutSocket
{
private:
    Csession           *m_session;
//    CString             m_connected_host; 
  
    void handleNonSocketError(char *);

    void handleSocketError(char *);
    
    int Recv (CMemChain *chain_ptr, LPTSTR buf, int len, int flags);

    static CMapWordToPtr m_this_list;

    static unsigned int Instances; 

	char m_HostBuf[MAXGETHOSTSTRUCT];

public:
    CNtcssSock(Csession *session);
	~CNtcssSock();
    
    int CloseSocket();

    BOOL ConnectHost (LPCTSTR Hostname, int port);

 //   int	GetHostName (LPTSTR name, int name_len);

  // int GetHostIpAddr(LPTSTR name, int name_len);

    int ReadLine(register char *ptr, register int maxlen);

	int ReadNum();

    int Recv (LPTSTR buf, int len, int flags)
		{ return (Recv (NULL, buf, len, flags)); };

    int Recv (CMemChain *chain_ptr, int len, int flags) 
		{ return (Recv (chain_ptr, NULL, len, flags)); };

    BOOL Startup();


};

#endif //_INCLUDE_NTCSSOCK_H_CLASS_
