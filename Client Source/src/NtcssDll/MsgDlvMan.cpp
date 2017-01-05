/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgDlvMan.cpp
//                                         
// This is the implementation file for CdllMsgDeliveryMan.
//
//----------------------------------------------------------------------


#include "stdafx.h"
#include "0_basics.h"

#include "MsgDlvMan.h"
#include "msgcore.h"
#include "genutils.h"
#include "session.h"
#include "memchain.h"


/////////////////////////////////////////////////////////////////////////////
// CdllMsgDeliveryMan constructor
// ______________________________
//
// One version for when _NTCSS_MSG_TRACING_ is turned on, other for
// when not tracing.


CdllMsgDeliveryMan::CdllMsgDeliveryMan()
{
	m_bIsPartOfDll = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CdllMsgDeliveryMan destructor
// _______________________________
//
// If tracing then destructor closes file if still left open.

CdllMsgDeliveryMan::~CdllMsgDeliveryMan()
{

}



#ifdef NTCSS_DLL_BUILD

/////////////////////////////////////////////////////////////////////////////
// DllDoMessage
// _________
//
// This function used when using CdllMsgDeliveryMan from within the
// NTCSS DLL - just calls DoMessage.

BOOL CdllMsgDeliveryMan::DllDoMessage(SDllMsgRec *msg,
					  				  CNtcssBaseMsg *caller_msg)
{
	return(DoMessage(gbl_dll_man.MMcall_CurrentSession(),
			         msg, caller_msg));
}
#endif  // NTCSS_DLL_BUILD



/////////////////////////////////////////////////////////////////////////////
// DoMessage
// _________

BOOL CdllMsgDeliveryMan::DoMessage(Csession *session,
								   SDllMsgRec *msg,
								   CNtcssBaseMsg *caller_msg)
{
	// Use caller_msg value to
	// determine if msg is being
	// handled from within
	// DLL or via API

	CNtcssSock* p=new CNtcssSock(session);

	try
	{

	if(!p->Create())
	{
		CString temp;
		temp.Format("Socket Creation failed-> %d",p->GetLastError());
		AfxMessageBox(temp);
	}
	else
		p->SetTimeoutLength(72000); //was 5800 //72000 //this is seconds
	
	if (caller_msg == NULL)
		m_bIsPartOfDll = TRUE;
	else
		m_bIsPartOfDll = FALSE	;
	
	Sbasic_svrmsgrec *bsc = (Sbasic_svrmsgrec *)msg->send_data_ptr;

	// Basic formatting 
	FormatBasicSvrMsgRec(bsc);

	TRACE1(_T("Sending Message <%s>\n"),msg->send_data_ptr);
	
	int rc;	
	// First check if IsBlocking
	//
    if (p->IsBlocking()) 
		throw(_T("Socket is blocking"));

	TRACE0("Past socket IsBlocking() call\n");
		
	// Determine destination server

	CString host_to_use;
	switch (msg->server_select)
	{

	case USE_APP_SERVER :
		const SappGroupRec_wFullData *app_group_ptr;
		
		app_group_ptr = gbl_dll_man.dllUser()->appGroup
			(session->ssnAppName());
		
		host_to_use = app_group_ptr->host_name_buf;
		
		break;
		
	case USE_CONNECTED_SERVER :
		host_to_use = gbl_dll_man.dllServerName();
		//AfxMessageBox("using server " + (CString)host_to_use); //debug
		break;//TODO this should go eventually 
		
	case USE_GIVEN_SERVER :
		host_to_use = msg->server_name;
		break;
		
	case USE_MASTER_SERVER :
		host_to_use = gbl_dll_man.dllUser()->GetMasterName();
		break;
		
	case USE_AUTH_SERVER :
		
		host_to_use = gbl_dll_man.dllUser()->GetAuthServer
			(session->ssnAppName());
		
		break;
		
	default:
		break;
	}
	
	// Connect to host

	TRACE("Going to connect to host <%s> on port %d.\n",
		host_to_use,msg->port_num);

	//if (!p->ConnectHost(host_to_use, msg->port_num)) 
		//throw(_T("Unable to connect to host"));

	if (!p->connect(host_to_use, msg->port_num, 10)) //10 SECOND TIMEOUT HERE!!!!
		throw(_T("Unable to connect to host"));
	
	TRACE0("Past socket ConnectHost() call\n");

	TRACE1("Sending this msg to server %s\n",(const char *) msg->send_data_ptr);
	
    
	// Send data to server
	//

	rc = p->Send((const char *) msg->send_data_ptr,
		msg->send_data_size,0);
	
	// Make sure okay (Send should
	//  have recorded err)
	if (rc != msg->send_data_size)
		throw(_T("XYZ"));

	if(msg->recv_data_size==-1)
	{
		delete p;
		return TRUE;
	} 

	TRACE0("Past send() call, msg passed to server\n"); 
	
	
	// Now just RECV initial response
	//  from server
	//
	TCHAR server_response[SIZE_RESPONSE_CODE+1];
	::ZeroMemory(server_response,sizeof(server_response));
	
	TRACE0(_T("Getting ready to receive server response\n"));
	
	rc = p->Recv(server_response,SIZE_RESPONSE_CODE,0);

	TRACE2(_T("Socket Recv() done, got %d bytes (asked for %d bytes)\n"),
		rc, SIZE_RESPONSE_CODE);

	server_response[SIZE_RESPONSE_CODE + 1] = '\0'; //debug
	TRACE1(_T("Server response was: <%s>\n"),server_response);

	
	// Make sure recv'd ok (Recv
	//  should have recorded err)

	if (rc == SOCKET_ERROR)
		throw(_T("XYZ")); 

	// Recv handles SOCKET_ERROR,

	if (rc != SIZE_RESPONSE_CODE)
		throw(_T("NTCSS Message error - bad Recv"));


	// Was it the expected resp?
	//
	if (strcmp(server_response,msg->ok_msg) != 0)
	{
		
		TRACE0(_T("Got bad response from server.\n"));
				
		// Get in here if was not ok
		//
		if (strcmp(server_response,msg->bad_msg) == 0)
		{

			TRACE0(_T("Now trying to get error msg.\n"));

			
									   // Get in here if at least got
									   //  the expected 'bad_msg', now
									   //  try to get further info from
									   //  server
									   //
			
			
			char err_buf_raw[MAX_ERROR_MSG_LEN];
			
			rc = p->Recv(err_buf_raw,
				MAX_ERROR_MSG_LEN, 0);
			
		
			
			if (rc != SOCKET_ERROR)
			{
				// If get in here then got more
				//  error info from server, will
				//  format it and record error
				//
				char err_buf_fixed[MAX_ERROR_MSG_LEN];
				
				sprintf(err_buf_fixed,"NTCSS Message error - %s",
					&(err_buf_raw[CHAR_FMTSTR_LEN]));

				TRACE1(_T("Error message is %s\n"),err_buf_fixed);
				throw(err_buf_fixed);
					
			}
							   // Else couldn't get more info
							   //  from server
			else
			{
				TRACE(_T("Error details are unavailable\n"));
				throw(_T("NTCSS Message error - error details are unavailable"));
			}
		}
		// Here else is because we got
		//  wierd, unexpected info
		//  from server
		else
			throw(_T("NTCSS Message error - invalid data received"));
	}
	
	// For MEM_MSG_ADVALLOC messages
	//  we get the receive size from
	//  the socket
	
	if (msg->msg_category == MEM_MSG_ADVALLOC)
    {

		TRACE("Was a MEM_MSG_ADVALLOC msg, going to get byte cnt now\n",msg->recv_data_size);
	    msg->recv_data_size = p->ReadNum();
		
		
		TRACE("For this MEM_MSG_ADVALLOC msg, size is %d.\n",msg->recv_data_size);
		
		if (msg->recv_data_size == SOCKET_ERROR)
			throw(_T("XYZ"));
		
	}
	// If get here and ok then so far
	//  so good, now get have setup
	//  a CMemChain for the receive
	//  data
	
	{ // RECEIVE-BLOCK start
		if (msg->recv_data_size != 0)
		{


			TRACE("Before making new CMemChain, msg: %d\n", msg);
			CMemChain *chain_ptr = new CMemChain((TMemChainLink *)
				msg->recv_data_chain_ptr);
			
			TRACE("Just made new CMemChain, now SetExternalHead\n");
			chain_ptr->SetExternalHead((TMemChainLink **)
				&(msg->recv_data_chain_ptr));
			
									   // Should not be any data there,
									   //  but if so trash it now

			TRACE0("Just did SetExternalHead, now TrashChainLinks\n");
			chain_ptr->TrashChainLinks();
			
		
			TRACE("Now going to Recv data, looking for %d bytes.\n",
				msg->recv_data_size);

			rc = p->Recv(chain_ptr, 
				msg->recv_data_size,0);
			
			
			TRACE("Socket Recv() is done, Recv'd %d bytes, chain head pointer is now at: %d\n",
				rc,msg->recv_data_chain_ptr);
			
            //SHOW THE FIRST 450 bytes of recieved data
			//TRACE limits lines to around 450 characters!!!!


			TRACE("The received data:\n <%.450s>\n",chain_ptr->LinkData((TMemChainLink *)
		          msg->recv_data_chain_ptr));

			
		}
	} // RECEIVE-BLOCK end
	// Close socket and see if had
	// socket error
	
	BOOL ok = (rc != SOCKET_ERROR);
	
	TRACE("CdllMsgDeliveryMan::DoMessage completing %s now.\n",
		(ok ? "successfully" : "with errors"));
	
	// Finally done

    delete p;
	
    return(ok);
	} //try

	catch(LPTSTR pStr)
	{
		if(pStr!=_T("XYZ"))
			session->ssnErrorMan()->RecordError(pStr);
		delete p;
		return FALSE;
	}

    catch(...)
    {
		DWORD ret=p->GetLastError();
		CString temp;
		temp.Format("Socket Error was-> %ld",ret);
   		delete p;
        return FALSE;
    }
}


/////////////////////////////////////////////////////////////////////////////
// FormatBasicSvrMsgRec
// ______________________

void CdllMsgDeliveryMan::FormatBasicSvrMsgRec
								(Sbasic_svrmsgrec *the_msg_buf)
{
    strncpyWpad(the_msg_buf->loginname,
                gbl_dll_man.dllUser()->userInfo()->login_name_buf,
				SIZE_LOGIN_NAME,' ');
                
    strncpyWpad(the_msg_buf->token,
				gbl_dll_man.dllUser()->userInfo()->token_buf,
                SIZE_TOKEN, ' ');
}



                                       // Routines below here are compiled
                                       // only when doing _NTCSS_MSG_TRACING_

