/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssServerSocket.h: interface for the NtcssServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTCSSSERVERSOCKET_H__9F250044_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_)
#define AFX_NTCSSSERVERSOCKET_H__9F250044_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimeoutSocket.h"

class NtcssServerSocket_class  
{
public:
	NtcssServerSocket_class();
	virtual ~NtcssServerSocket_class();

    void open(LPCSTR, int, int);
    void read(LPTSTR, int);
    void write(LPTSTR, int);
    void close();
    void get_master_server(LPTSTR);

    int      bytes_written;
    int      bytes_read;
    bool     ok;

private:
    void set_status();

    CTimeoutSocket* server_socket;
    int      timeout_seconds;
    bool     opened;            
    char     host_name[MAX_HOST_NAME_LEN];

};

extern NtcssServerSocket_class  NtcssServerSocket;

#endif // !defined(AFX_NTCSSSERVERSOCKET_H__9F250044_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_)
