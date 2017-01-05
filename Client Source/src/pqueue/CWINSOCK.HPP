//
//                                                              
//  Source File:    CWINSOCK.hpp
//  Purpose:    Class declaration for CWINSOCK                                   
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
        
#ifndef _WINSOCKAPI_
extern "C"
{
    #include "winsock.h" 
}
#endif

typedef void (CALLBACK* WINSOCKPROC)(WPARAM wParam, LPARAM lParam);

// Declare CGcpWnd, a CFrameWnd descendant
class CWinsock : public CWnd
{
private:
    SOCKET s;
    WINSOCKPROC WinsockProc;
	HWND m_Hwnd;

public:
    CWinsock(WINSOCKPROC WinsockProc, CWnd * pParent);
    CWinsock();
    ~CWinsock();
    
    SOCKET Accept (struct sockaddr FAR *addr, int FAR *addrlen);
    int Bind (const struct sockaddr FAR *addr, int namelen);
    int CloseSocket ();
    int Connect (const struct sockaddr FAR *name, int namelen);
    int IoctlSocket (long cmd, u_long FAR *argp);
    int GetPeerName (struct sockaddr FAR *name,
                            int FAR * namelen);
    int GetSockName (struct sockaddr FAR *name,
                            int FAR * namelen);
    int GetSockOpt (int level, int optname,
                           char FAR * optval, int FAR *optlen);
    u_long Htonl (u_long hostlong);
    u_short Htons (u_short hostshort);
    unsigned long Inet_Addr (const char FAR * cp);
    char FAR * Inet_Ntoa (struct in_addr in);
    int Listen (int backlog);
    u_long Ntohl (u_long netlong);
    u_short Ntohs (u_short netshort);
    int Recv (char FAR * buf, int len, int flags);
    int RecvFrom (char FAR * buf, int len, int flags,
                             struct sockaddr FAR *from, int FAR * fromlen);
    int Select (int nfds, fd_set FAR *readfds, fd_set FAR *writefds,
                           fd_set FAR *exceptfds, const struct timeval FAR *timeout);
    int Send (const char FAR * buf, int len, int flags);
    int SendTo (const char FAR * buf, int len, int flags,
                           const struct sockaddr FAR *to, int tolen);
    int SetSockOpt (int level, int optname,
                               const char FAR * optval, int optlen);
    int Shutdown (int how);
    SOCKET Socket (int af, int type, int protocol);
    struct hostent FAR * GetHostByAddr(const char FAR * addr,
                                                  int len, int type);
    struct hostent FAR * GetHostByName(const char FAR * name);
    int GetHostName (char FAR * name, int namelen);
    struct servent FAR * GetServByPort(int port, const char FAR * proto);
    struct servent FAR * GetServByName(const char FAR * name,
                                                  const char FAR * proto);
    struct protoent FAR * GetProtoByNumber(int proto);
    struct protoent FAR * GetProtoByName(const char FAR * name);
    void SetLastError(int iError);
    int GetLastError(void);
    BOOL IsBlocking(void);
    int UnhookBlockingHook(void);
    FARPROC SetBlockingHook(FARPROC lpBlockFunc);
    int CancelBlockingCall(void);
    HANDLE AsyncGetServByName(const char FAR * name, 
                                            const char FAR * proto,
                                            char FAR * buf, int buflen);
    HANDLE AsyncGetServByPort(int port,
                                            const char FAR * proto, char FAR * buf,
                                            int buflen);
    HANDLE AsyncGetProtoByName(const char FAR * name, char FAR * buf,
                                             int buflen);
    HANDLE AsyncGetProtoByNumber(int number, char FAR * buf,
                                               int buflen);
    HANDLE AsyncGetHostByName(const char FAR * name, char FAR * buf,
                                            int buflen);
    HANDLE AsyncGetHostByAddr(const char FAR * addr, int len, int type,
                                            char FAR * buf, int buflen);
    int CancelAsyncRequest(HANDLE hAsyncTaskHandle);
    int AsyncSelect(long lEvent);
                                   
    void Assume(SOCKET s);
    
    // statics for class
    static unsigned int Instances; // must be initialized to 0 in a code segment
    static WSAData Data;
    
    // message map functions
    LONG NetEvent (WPARAM wParam, LPARAM lParam); 
    
    DECLARE_MESSAGE_MAP();
};
