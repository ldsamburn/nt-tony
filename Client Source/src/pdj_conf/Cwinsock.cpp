// NTCSS_ROOT_DIR\source\comn_lib\cwinsock.cpp
//
// WSP, last update: 9/16/96
//                                        
// See original comments below, no changes made by me, WSP.
//
//----------------------------------------------------------------------
//  Source File:    CWINSOCK.cpp
//  Purpose:    CWINSOCK class definitions                                   
//  Date:       9 November 1993
//  Version:    Alpha                 
//  Developer:  Michael J. Baldwin, GENISYS Comm, Inc.                                                            
//      
//  The enclosed C++ definitions and software is copyrighted by GENISYS Comm, and is
//  hereby released into the public domain.  It may be freely used by others if 
//  the following notice is included in each source code and header file:
//
//  "Copywrite 1993, GENISYS Comm, Inc."
//
//  Please provide suggesting, improvements, etc. to the author (baldwin@GENISYS.com)
//  and we will modify this definition as warranted.
//
//

#define STRICT
#pragma warning (disable:4706 4355)  

#include "stdafx.h"

#include "cwinsock.hpp"


// initialize statics for class...
WSAData CWinsock::Data;
unsigned int CWinsock::Instances=0; // initialize to 0 for application

// initialize defines...
#define WM_NETEVENT WM_USER+1

// define message map for CWinsock
BEGIN_MESSAGE_MAP(CWinsock,CWnd)
    ON_MESSAGE(WM_NETEVENT, NetEvent) // for FD_READ, etc...
    ON_WM_TIMER()
END_MESSAGE_MAP()
 
CWinsock::CWinsock()
{
}

CWinsock::CWinsock(WINSOCKPROC lpWinsockProc, CWnd * pParent)
    {
    // initialize WINSOCK interface
    if (!Instances)
        WSAStartup(0x0101, &Data);
        
    // one more instance
    Instances++;
    
    // Initialize data members
    WinsockProc=lpWinsockProc;
    s=NULL;
    
    // create window for all asynchronous notification
    RECT Rect={0,0,0,0};
    Create(NULL,"CWinsockWnd",WS_CHILD,Rect,pParent,NULL);

    UINT val;
    val = SetTimer(101,(10),NULL);
    }


CWinsock::~CWinsock()
    {
    // one less instance
    Instances--;
    if (!Instances)
        WSACleanup();
    }
 
SOCKET CWinsock::Accept (struct sockaddr FAR *addr, int FAR *addrlen)
    {
    // install error checking later
    return accept (s, addr, addrlen);
    }
 
int CWinsock::Bind (const struct sockaddr FAR *addr, int namelen)
    {
    // install error checking later
    return bind (s, addr, namelen);
    }
 
int CWinsock::CloseSocket ()
    {
    // install error checking later
    return closesocket (s);
    }
 
int CWinsock::Connect (const struct sockaddr FAR *name, int namelen)
    {
    // install error checking later
    return connect (s, name, namelen);
    }
 
int CWinsock::IoctlSocket (long cmd, u_long FAR *argp)
    {
    // install error checking later
    return ioctlsocket (s, cmd, argp);
    }
 
int CWinsock::GetPeerName (struct sockaddr FAR *name,
                         int FAR * namelen)
    {
    return getpeername (s, name, namelen);
    }
 
int CWinsock::GetSockName (struct sockaddr FAR *name,
                         int FAR * namelen)
    {
    return getsockname (s, name, namelen);
    }
 
int CWinsock::GetSockOpt (int level, int optname,
                        char FAR * optval, int FAR *optlen)
    {
    return getsockopt (s, level, optname, optval, optlen);
    }
 
u_long CWinsock::Htonl (u_long hostlong)
    {
    return htonl (hostlong);
    }
 
u_short CWinsock::Htons (u_short hostshort)
    {
    return htons (hostshort);
    }
 
unsigned long CWinsock::Inet_Addr (const char FAR * cp)
    {
    return inet_addr (cp);
    }
 
char FAR * CWinsock::Inet_Ntoa (struct in_addr in)
    {
    return inet_ntoa (in);
    }
 
int CWinsock::Listen (int backlog)
    {
    return listen (s, backlog);
    }
 
u_long CWinsock::Ntohl (u_long netlong)
    {
    return ntohl (netlong);
    }
 
u_short CWinsock::Ntohs (u_short netshort)
    {
    return ntohs(netshort);
    }
 
int CWinsock::Recv (char FAR * buf, int len, int flags)
    {
    return recv (s, buf, len, flags);
    }
 
int CWinsock::RecvFrom (char FAR * buf, int len, int flags,
                         struct sockaddr FAR *from, int FAR * fromlen)
    {
    return recvfrom (s, buf, len, flags, from, fromlen);
    }
 
int CWinsock::Select (int nfds, fd_set FAR *readfds, fd_set FAR *writefds,
                       fd_set FAR *exceptfds, const struct timeval FAR *timeout)
    {
    return select (nfds, readfds, writefds, exceptfds, timeout); 
    }
 
int CWinsock::Send (const char FAR * buf, int len, int flags)
    {
    return send (s, buf, len, flags);
    }
 
int CWinsock::SendTo (const char FAR * buf, int len, int flags,
                       const struct sockaddr FAR *to, int tolen)
    {
    return sendto (s, buf, len, flags, to, tolen);
    }
 
int CWinsock::SetSockOpt (int level, int optname,
                           const char FAR * optval, int optlen)
    {
    return setsockopt (s, level, optname, optval, optlen);
    }
 
int CWinsock::Shutdown (int how)
    {
    return shutdown(s, how);
    }
 
SOCKET CWinsock::Socket (int af, int type, int protocol)
    {
    return socket (af, type, protocol);
    }
 
struct hostent FAR * CWinsock::GetHostByAddr(const char FAR * addr,
                                              int len, int type)
    {
    return gethostbyaddr(addr, len, type);
    }
 
struct hostent FAR * CWinsock::GetHostByName(const char FAR * name)
    {
    return gethostbyname(name);
    }
 
int CWinsock::GetHostName (char FAR * name, int namelen)
    {
    return gethostname (name, namelen);
    }
 
struct servent FAR * CWinsock::GetServByPort(int port, const char FAR * proto)
    {
    return getservbyport(port, proto);
    }
 
struct servent FAR * CWinsock::GetServByName(const char FAR * name,
                                              const char FAR * proto)
    { 
    return getservbyname(name, proto);
    }
 
struct protoent FAR * CWinsock::GetProtoByNumber(int proto)
    {
    return getprotobynumber(proto);
    }
 
struct protoent FAR * CWinsock::GetProtoByName(const char FAR * name)
    {
    return getprotobyname(name);
    }
 
void CWinsock::SetLastError(int iError)
    {
    WSASetLastError(iError);
    }
 
int CWinsock::GetLastError(void)
    {
    return WSAGetLastError();
    }
 
BOOL CWinsock::IsBlocking(void)
    {
    return WSAIsBlocking();
    }
 
int CWinsock::UnhookBlockingHook(void)
    {
    return WSAUnhookBlockingHook();
    }
 
FARPROC CWinsock::SetBlockingHook(FARPROC lpBlockFunc)
    {
    return WSASetBlockingHook(lpBlockFunc);
    }
 
int CWinsock::CancelBlockingCall(void)
    {
    return WSACancelBlockingCall();
    }
 
HANDLE CWinsock::AsyncGetServByName(const char FAR * name, 
                                        const char FAR * proto,
                                        char FAR * buf, int buflen)
    {
    return WSAAsyncGetServByName(m_hWnd, WM_NETEVENT, name, proto, buf, buflen);
    }
 
HANDLE CWinsock::AsyncGetServByPort(int port,
                                        const char FAR * proto, char FAR * buf,
                                        int buflen)
    {
    return WSAAsyncGetServByPort(m_hWnd, WM_NETEVENT, port, proto, buf, buflen);
    }
 
HANDLE CWinsock::AsyncGetProtoByName(const char FAR * name, char FAR * buf,
                                         int buflen)
    {
    return WSAAsyncGetProtoByName(m_hWnd, WM_NETEVENT, name, buf, buflen);
    }
 
HANDLE CWinsock::AsyncGetProtoByNumber(int number, char FAR * buf,
                                           int buflen)
    {
    return WSAAsyncGetProtoByNumber(m_hWnd, WM_NETEVENT, number, buf, buflen);
    }
 
HANDLE CWinsock::AsyncGetHostByName(const char FAR * name, char FAR * buf,
                                        int buflen)
    {
    return WSAAsyncGetHostByName(m_hWnd, WM_NETEVENT, name, buf, buflen);
    }
 
HANDLE CWinsock::AsyncGetHostByAddr(const char FAR * addr, int len, int type,
                                        char FAR * buf, int buflen)
    {
    return WSAAsyncGetHostByAddr(m_hWnd, WM_NETEVENT, addr, len, type, buf, buflen);
    }
 
int CWinsock::CancelAsyncRequest(HANDLE hAsyncTaskHandle)
    {
    return 0; // WSACancelAsyncRequest(hAsyncTaskHandle);
    }
 
int CWinsock::AsyncSelect(long lEvent)
    {
    return WSAAsyncSelect (s, m_hWnd, WM_NETEVENT, lEvent);
    }

// assume a socket accepted by another CWinsock object 
void CWinsock::Assume(SOCKET NewSocket)
    {
    if (s!=INVALID_SOCKET)
        CloseSocket();
    s=NewSocket;
    }

// message map function start here
LONG CWinsock::NetEvent (WPARAM wParam, LPARAM lParam)
    {
    WinsockProc(wParam, lParam);
    return 0l;
    } 
 
