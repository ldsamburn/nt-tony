/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssSock.cpp
//                                         
// This is the implementation file for CNtcssSock.  
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include "0_basics.h"

#include "ntcssSock.h"
#include "session.h"
#include "dllutils.h"

										// Define/init static class data 
//WSAData CNtcssSock::m_wsa_data;

unsigned int CNtcssSock::Instances = 0;   
CMapWordToPtr CNtcssSock::m_this_list;


/////////////////////////////////////////////////////////////////////////////
// CNtcssSock constructor
// ______________________

CNtcssSock::CNtcssSock(Csession *session)
{
//    m_connected_host.Empty();
    if(!AfxSocketInit())
		AfxMessageBox("Sockinit Failed");
	TRACE1("Thread %ld has called AfxSocketInit\n",GetCurrentThreadId());
    m_session = session;
}


/////////////////////////////////////////////////////////////////////////////
// CNtcssSock destructor
// _____________________

CNtcssSock::~CNtcssSock()
{
}



/////////////////////////////////////////////////////////////////////////////
// CloseSocket
// ___________

int CNtcssSock::CloseSocket ()
{
	Close();
    return 1;
}


/////////////////////////////////////////////////////////////////////////////
// ConnectHost
// ___________

BOOL CNtcssSock::ConnectHost(LPCTSTR Hostname, int port)
{   
                                       //  Get host info
   struct hostent* hp;
   hp=::gethostbyname(Hostname);

   if(hp && Connect(hp->h_name,port))
   {   
	//   m_connected_host = Hostname;
	   return TRUE;
   }
   else
   {
	   TRACE2(_T("Ntcss Socket Connect failed to %s... Error (%d)\n"),Hostname,GetLastError());
	//   m_connected_host.Empty();
	   return FALSE;
   }

} 
    

/////////////////////////////////////////////////////////////////////////////
// handleNonSocketError
// ____________________

void CNtcssSock::handleNonSocketError(char *msg)
{
	m_session->ssnErrorMan()->RecordError(msg);
}



/////////////////////////////////////////////////////////////////////////////
// handleSocketError
// _________________

void CNtcssSock::handleSocketError(char *lead_msg = NULL)
{
    char err_msg[MAX_ERROR_MSG_LEN];

    sprintf(err_msg,
             "WinSock/%s error - %s",lead_msg,
               (char *)SockerrToString(GetLastError()));

    m_session->ssnErrorMan()->RecordError(err_msg);
}


////////////////////////////////////////////////////////////////////////////
// ReadNum
// _______

int CNtcssSock::ReadNum() 
{
    int     rval,num_chars;
    TCHAR   ch_str[SIZE_TINY_GENBUF];

try
{
    int rc=Receive(ch_str,1);
 
    if (rc == SOCKET_ERROR || rc==0)
    {
        rval = SOCKET_ERROR;
		if(HadTimeout())
			throw(_T("ReadNum(): Socket Timed out"));
		else
			throw(_T("ReadNum(): Socket Error"));
    }
  
 
	ch_str[1] = '\0';
	num_chars = _ttoi(ch_str);
	rc=Receive(ch_str,num_chars);

	if (rc != num_chars)
		throw(_T("ReadNum(): Socket Error"));
	else
	{
		ch_str[num_chars] = '\0';
		rval = atoi(ch_str);
	}
}

	catch(LPTSTR pStr)
	{
		
        handleSocketError(pStr);
        return(SOCKET_ERROR);
    }

	return(rval);
}


/////////////////////////////////////////////////////////////////////////////
// ReadLine
// ________    

int CNtcssSock::ReadLine(register char *ptr, register int maxlen) 
{
    int     rc;
    char    c;
                                      
                                       // Read until 
    for (int n = 0; n < maxlen; n++) 
    {
		rc=Receive(&c,1);

        if (rc == SOCKET_ERROR)
            break;
  
        else if (rc == 0)
              break;
        else
        {
            *ptr++ = c;
            if (c == '\n')
                break;
        }    
    }
 
    
    if (rc == SOCKET_ERROR)
    {
        handleSocketError("ReadLine");
        return(SOCKET_ERROR);
    }
    
    *ptr = '\0';

    return(n);

}  

/////////////////////////////////////////////////////////////////////////////
// Recv	
// ____

int CNtcssSock::Recv (CMemChain *chain_ptr, LPTSTR buf, 
					  int len, int flags)
{
									   // First do validity checks
									   //
	if ( ((chain_ptr == NULL) && (buf == NULL))
		 ||
		 ((chain_ptr != NULL) && (buf != NULL))	)
	{
		handleNonSocketError("Invalid parameters");
		return(RETURN_INT_ERROR);
	}

	if ( ((buf != NULL) &&
		  ( (len > NTCSSVAL_MAX_SOCKET_RECV_BLOCK_SIZE)
		    ||		  
		    (len < 1) ))
	     ||
		 ((chain_ptr != NULL) && (len < 0)) )

	{
		handleNonSocketError("Invalid Recv len");
		return(RETURN_INT_ERROR);
	}

    char    *recv_buf_ptr;
		
    int     total_bytes_read = 0;
	int		bytes_to_read;
    int     recvd_bytes;
                                       // Outer loop handles the case
									   //   when have to read more than
									   //   MAX_SOCKET_RECV_BLOCK_SIZE
									   //   bytes
	BOOL ok = TRUE;
	BOOL done = FALSE;
	while ((ok) && (!done))
	{

		if (chain_ptr == NULL)
		{
									   // Here setting up to read into
									   //  buf
									   //
			recv_buf_ptr = buf + total_bytes_read;
			bytes_to_read = len;
		}
		else
		{
									   // Here setting up to read into
									   //  CMemChain data blocks
									   //
			bytes_to_read = 
				( ((len - total_bytes_read) 
				     > MAX_SOCKET_RECV_BLOCK_SIZE)
				? MAX_SOCKET_RECV_BLOCK_SIZE
				: len - total_bytes_read);
									   // Now get a new link to receive
									   //   the data
									   //
	
			recv_buf_ptr = chain_ptr->NewLink(bytes_to_read);

			if (recv_buf_ptr == NULL)
			{
				ok = FALSE;
				handleNonSocketError("Memory allocation error");
			}
		}
									   // Inner loop keeps doing recv's
									   //   until have received
									   //   bytes_to_read bytes
									   //
		while ((ok) && (!done) && (bytes_to_read > 0))
		{
                                       // Setup timer, if don't get it
									   //  then have to quit
		
									   // Get data from socket here
									   //
		//	recvd_bytes = recv (m_socket, recv_buf_ptr, 
							//	bytes_to_read, flags);

			recvd_bytes = Receive(recv_buf_ptr,bytes_to_read);


			if (recvd_bytes == SOCKET_ERROR)
			{
	            handleSocketError("Recv");
				ok = FALSE;
		        continue;
			}

			total_bytes_read += recvd_bytes;

			bytes_to_read    -= recvd_bytes;

			recv_buf_ptr     += recvd_bytes;

			if (*(recv_buf_ptr-1) == NULL || recvd_bytes==0)
				done = TRUE;
		}
		if ( (len != 0) && (total_bytes_read >= len) )
			done = TRUE;
    }
                                       // Finally done
    return (ok ? total_bytes_read : SOCKET_ERROR);
}

/////////////////////////////////////////////////////////////////////////////
// Startup
// ________
                                   
BOOL CNtcssSock::Startup()
{
	//TODO: see if you can use AfxSockInit instead

         
	// Specify WINSOCK version needed

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2);
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
        AfxMessageBox("Couldn't find the correct winsock.dll");
		return FALSE;
	}

                                          // Have one more instance now
    Instances++;
                                       // Initialize data members
    return TRUE;
} 


