/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

 // NTCSS_ROOT_DIR\source\comn_lib\inri_ftp.cpp
//
// WSP, last update: 9/16/96
//                                         
// This is the implementation file for the CInriFtpMan routines.  
//
// See routine comments below and also the header file for details on 
// processing.
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include "stdio.h"
#include "inri_ftp.h"
#include <sys/types.h>   
#include <sys/stat.h>
 


/////////////////////////////////////////////////////////////////////////////
// LocalFileGetFromRemote
// ______________________
//
// Reads a file from the server on the other end of m_win_sock and
// writes it to a local disk.

CInriFtpMan::~CInriFtpMan()
{
	if(m_win_sock)
		delete m_win_sock;
}


/////////////////////////////////////////////////////////////////////////////
// LocalFileGetFromRemote
// ______________________
//
// Reads a file from the server on the other end of m_win_sock and
// writes it to a local disk.

int CInriFtpMan::LocalFileGetFromRemote
						 (const char *remote_file_name_str,
                          const char *local_file_name_str,
                          BOOL is_text)
{
                                       // Static so have to bother
                                       //  with allocating space only when
                                       //  class is first constructed


    static SfixedDataBuffer fixed_data;
    
    FILE  *file_ptr = NULL;
    int  state = XFER_INIT_READ;
    BOOL done_fopen = FALSE;

    BOOL done = FALSE;
    BOOL ok   = TRUE;
	int totalbytes=0;
	int tcnt=0;
	CString t;

    
    while(!done) 
    {
        switch(state) 
        {
        	                           // Receieve ACK from server after
        	                           //  XFER_INIT_READ 
        	                           //  and after XFER_WRITE_LOCAL
									   //  (XFER_OK_PACK case), here
									   //  just read data, actually
									   //  check contents in 
									   //  XFER_OPEN_LOCAL and
									   //  WRITE_LOCAL
			case XFER_GET_RESP:
			{   
				TRACE0("XFER_GET_RESP\n");
				tcnt=PacketRead(&fixed_data);

				if (tcnt  != XFER_ERROR_CODE) 
			    {
					
				    if (!done_fopen)
					    state = XFER_OPEN_LOCAL;
	                else {
		                state = XFER_WRITE_LOCAL;
						totalbytes += fixed_data.len_val;
					}
			    }
				else
				{
			        m_errman.RecordError("Error reading socket response");
				    state = XFER_TRANSFER_ERROR;
	            } 
			}
            break;

                                       // Open remote file
			case XFER_INIT_READ:
			{
				TRACE0("XFER_INIT_READ\n");
				if (SendCommand(XFER_OPEN_READ_NC, 
								remote_file_name_str))
				{
					state = XFER_GET_RESP;
					strcpy(fixed_data.file_name_buf,
						   remote_file_name_str);
				}
				else
				{ 
					char err_msg[MAX_ERROR_MSG_LEN];
					sprintf(err_msg,"Error opening remote file\n\t%s",
	                        remote_file_name_str);
		                
			        m_errman.RecordError(err_msg);
				    state = XFER_TRANSFER_ERROR;
				} 
			}
            break; 
									   // After have done 
									   //  XFER_GET_RESP for the
									   //  XFER_INIT_READ then get to
									   //  XFER_OPEN_LOCAL to verify
									   //  status and to open local
									   //  file for write
			case XFER_OPEN_LOCAL:	   
			{
				TRACE0("XFER_OPEN_LOCAL\n");
				
									   // First make sure is ok
									   //
				if (fixed_data.cmd_val == XFER_OK_PACK) 
				{
									   // If local file exists try to
									   //  delete it
									   //
    			    struct stat stat_val;
	                if (!stat(local_file_name_str, &stat_val)) 
		            {
			            if ((stat_val.st_mode & _S_IFREG) &&  
				            (stat_val.st_uid == fixed_data.uid_val)) 
					    {
						    remove(local_file_name_str);
						} 
						else 
						{
							char err_msg[MAX_ERROR_MSG_LEN];
							sprintf(err_msg,"Unable to create file (permission or type error)\n\t%s",
								    local_file_name_str);
                    
							m_errman.RecordError(err_msg);
							state = XFER_TRANSFER_ERROR;
						}
					}


									   // Now open local file for writing
									   //  and then start reading file
									   //  from server
									   //
					if ((file_ptr = fopen(local_file_name_str, is_text ? "w" : "w+b")) != NULL) 
					 
					{
		                done_fopen = TRUE;
			            SendCommand(XFER_READ_NC, remote_file_name_str);
				        state = XFER_GET_RESP; 
					}
					else
					{
						char err_msg[MAX_ERROR_MSG_LEN];
						sprintf(err_msg,"Unable to open file for writing\n\t%s",
							    local_file_name_str);
                    
						m_errman.RecordError(err_msg);
						state = XFER_TRANSFER_ERROR;
					} 
				}
									   // Else did't get expected ack
				else
	            {
					char err_msg[MAX_ERROR_MSG_LEN];
	                sprintf(err_msg,"Error opening remote file\n\t%s",
						    remote_file_name_str);
                    
					m_errman.RecordError(err_msg);
					state = XFER_TRANSFER_ERROR;
				} 
			}
            break;

									   // Just got data from server, 
									   //  now verify ok and write
									   //  data to local file
			case XFER_WRITE_LOCAL:
			{
				TRACE0("XFER_WRITE_LOCAL\n");

		        switch(fixed_data.cmd_val) 
			    {
									   // Standard case, received ok
									   //  write data to local file
									   //  and ask server for more
					case XFER_OK_PACK:
					{
						TRACE("XFER_OK_PACK\n");

	                    if ((fwrite(fixed_data.data_buf, 1, 
									(size_t)fixed_data.len_val, 
									file_ptr)) 
                             == (size_t)fixed_data.len_val) 
	                    {
							fflush(file_ptr);
		                    state = XFER_GET_RESP;
			                SendCommand(XFER_READ_NC, remote_file_name_str);             
				        }
						else 
		                {
							char err_msg[MAX_ERROR_MSG_LEN];
							sprintf(err_msg,"Local file write error error\n\t%s",
							        local_file_name_str);
							m_errman.RecordError(err_msg);

			                SendCommand(XFER_CLOSE_READ_NC, remote_file_name_str);
				            state = XFER_TRANSFER_ERROR;
					    } 
                    }
					break;
									   // EOF case, if any data at all
									   //  came over then write it to
									   //  file and then do 
									   //  XFER_CLOSE_LOCAL
					case XFER_EOF_PACK:
					{
						TRACE("XFER_EOF_PACK\n");
		                state = XFER_CLOSE_LOCAL;

	                    if (fixed_data.len_val != 0)
						{
							if ((fwrite(fixed_data.data_buf, 1,
		                                (size_t)fixed_data.len_val,
								        file_ptr)) 
			                    != (size_t)fixed_data.len_val) 
							{
								m_errman.RecordError("Unexpected end of file");
								state = XFER_TRANSFER_ERROR;
							} 
							fflush(file_ptr);

						}
					}
				    break;
                    				   // Error case, didn't get good
									   //  ack from server
	                case XFER_NOK_PACK:
					{
						TRACE("XFER_NOK_PACK\n");
		                m_errman.RecordError("File transfer error (invalid server acknowledgement)");
			            state = XFER_TRANSFER_ERROR;
					}
				    break;
                    				   // Weirdo case, error also
					default:
					{
						m_errman.RecordError("Unexpected server response");
	                    state = XFER_TRANSFER_ERROR;
					}
                    break;
                }
            }
            break;
    								   // So far so good, close file
									   //  and socket then done ok
			case XFER_CLOSE_LOCAL:
			{
				TRACE("XFER_CLOSE_LOCAL\n");

	            fclose(file_ptr);    
		        m_win_sock->Close();
			    done = TRUE;
				ok = TRUE;
				//t.Format("DEBUG:Total Recv = %d",totalbytes);
				//AfxMessageBox(t); 
			}
            break;					   //  Errors go through here
            
	        case XFER_TRANSFER_ERROR:    
			{
				TRACE("XFER_TRANSFER_ERROR\n");
			    if (file_ptr)
				    fclose(file_ptr);
				m_win_sock->Close();
				done = TRUE;
				ok = FALSE;
			}
            break;
            						   // Wierdo case
			default:
			{
				m_errman.RecordError("File transfer state error");
	            state = XFER_TRANSFER_ERROR;
			}
            break;

        }  // end of switch(state) 
    }  // end of while(!done) 


    return(ok);
}


/////////////////////////////////////////////////////////////////////////////
// FileWrite
// _________
//
//
// Reads a local file and sends it to the server on the other end of 
// the m_win_sock.
                 
BOOL CInriFtpMan::LocalFilePutToRemote
						 (const char *remote_file_name_str,
                          const char *local_file_name_str,
                          BOOL is_text,BOOL append/*FALSE*/)
{
                                       // Static so have to bother
                                       //  with allocating space only when
                                       //  class is first constructed
    static SfixedDataBuffer fixed_data;


    int    read_cnt;
    FILE  *file_ptr = NULL;
    int  state = XFER_INIT_WRITE;
    BOOL done_fclose = FALSE;

    BOOL done = FALSE;
    BOOL ok   = TRUE;
    
    while(!done) 
    {
        switch(state) 
        {
        	                           // Receieve ACK from server after
        	                           //  OPEN_REMOTE, after WRITE_REMOTE
        	                           //  and after CLOSE.
        	case XFER_GET_RESP:
        	{
				TRACE0("XFER_GET_RESP\n");
    	        if (PacketRead(&fixed_data)  != XFER_ERROR_CODE) 
        	    {
            	    if (fixed_data.cmd_val == XFER_OK_PACK) 
                	{
                	                   // For GET_RESP state next state is
                	                   //  always READ_LOCAL until the flag
                	                   //  done_fclose becomes TRUE
                	                   //
	                    if (done_fclose)
    	                    state = XFER_DONE;
        	            else
            	            state = XFER_READ_LOCAL;
	                } 
    	            else 
        	        {
            	        char err_msg[MAX_ERROR_MSG_LEN];
                	    sprintf(err_msg,"Remote file open/read error\n\t%s",
                    	        remote_file_name_str);
                    
	                    m_errman.RecordError(err_msg);
    	                state = XFER_TRANSFER_ERROR;
        	        }
				}
    	        else
	            {
            	    m_errman.RecordError("Error reading socket data response");
        	        state = XFER_TRANSFER_ERROR;
	            }
	        }
   	        break;
                                       // Make sure local file exists
        	case XFER_INIT_WRITE:
        	{
				TRACE0("XFER_INIT_WRITE\n");

    			struct stat stat_val;
            	if (!stat(local_file_name_str, &stat_val))
            	{
                	state = XFER_OPEN_LOCAL;
            	} 
            	else 
            	{
                	state = XFER_TRANSFER_ERROR;
                	char err_msg[MAX_ERROR_MSG_LEN];
                	sprintf(err_msg,"File not found or invalid file type\n\t%s",
                            local_file_name_str);
                	m_errman.RecordError(err_msg);
            	}
        	}
        	break;                     
                                       // Open local file
        	case XFER_OPEN_LOCAL:
			{
				TRACE0("XFER_OPEN_LOCAL\n");
            	if ((file_ptr = fopen(local_file_name_str, is_text ? "r" : "rb")) != NULL) 
            	{ 
                	state = XFER_OPEN_REMOTE;
            	}
            	else
            	{
                	char err_msg[MAX_ERROR_MSG_LEN];
                	sprintf(err_msg,"No such file or directory\n\t%s",
                    	    local_file_name_str);
                	m_errman.RecordError(err_msg);
                	state = XFER_TRANSFER_ERROR;
            	} 
            }
           	break;                     // Have server open remote file
            
        	case XFER_OPEN_REMOTE:
        	{
				TRACE0("XFER_OPEN_REMOTE\n");

				if (!(SendCommand(append ? XFER_OPEN_NC_APPEND :XFER_OPEN_NC, remote_file_name_str)))     
    	        {
        	        char err_msg[MAX_ERROR_MSG_LEN];
            	    sprintf(err_msg,"Unable to open remote file\n\t%s",
                	        remote_file_name_str);
	                m_errman.RecordError(err_msg);
    	            state = XFER_TRANSFER_ERROR;
        	    } 
            	else 
	                state = XFER_GET_RESP;
        	}
   	        break;          
                                       // Read another block from the the
                                       //  local file unless hit eof
        	case XFER_READ_LOCAL:
			{
				TRACE0("XFER_READ_LOCAL\n");
            	if (feof(file_ptr))
            	{
	                state = XFER_CLOSE;
	            }
	            else 
    	        {
// Here is where would be handling cr/lf stuff, WSPTODO, old code
//              if (ascii_flag == TRUE) {
//                  if ((fgets(hdblock.dbuf.buf,BUF_SIZE,file_ptr)) == NULL) {
//                      MessageBox("Error reading local file","",MB_OK);
//                      state=CLOSE;
//                  } else {
                        // we assume the order is CR/LF at the end so change strlen-1 to LF
//                      hdblock.dbuf.buf[strlen(hdblock.dbuf.buf)-1]
//                  }
                    
        	        if ((read_cnt = fread(fixed_data.data_buf, 1, 
        	        					  XFER_DATABUF_SIZE, file_ptr)) == 0) 
            	    {
                	    char err_msg[MAX_ERROR_MSG_LEN];
                    	sprintf(err_msg,"Error in local file read\n\t%s",
	                            local_file_name_str);
                    
    	                m_errman.RecordError(err_msg);
        	            state = XFER_TRANSFER_ERROR;
            	    } 
                	else 
	                    state = XFER_WRITE_REMOTE;
    	        }
    	    }
        	break;
                                       // Send another block to server
        	case XFER_WRITE_REMOTE:
			{
				TRACE0("XFER_WRITE_REMOTE\n");
				
            	fixed_data.cmd_val = XFER_WRITE_NC;
            	fixed_data.len_val = read_cnt;
            
// WSPTODO, is strcpy necessary everytime or can get by doing just once?

            	strcpy(fixed_data.hostname_buf, m_lcl_hostname_str);
            
            	if (!(PacketWrite(&fixed_data))) 
	            {
                	state = XFER_TRANSFER_ERROR;
	                m_errman.RecordError("Socket transfer error.");
    	        }
        	    else
            	    state=XFER_GET_RESP;
            }
            break;
                                       // Done, close local file and send
                                       //  close file cmd to server
        	case XFER_CLOSE:
        	{
				TRACE0("XFER_CLOSE\n");
            	fclose(file_ptr);
            	file_ptr = NULL;              
            	SendCommand(XFER_CLOSE_NC, remote_file_name_str);
            	state = XFER_GET_RESP;
            	done_fclose = TRUE;
            }
            break;                     
                                       // Finished transfer, everything ok
        	case XFER_DONE:  
            {
				TRACE0("XFER_DONE\n");
	            m_win_sock->Close();
    	        done = TRUE;
        	    ok = TRUE;
        	}
            break;
                                       // Ran into a problem, m_errman
                                       //  should already have record of 
                                       //  error
        	case XFER_TRANSFER_ERROR:
        	{    
				TRACE0("XFER_TRANSFER_ERROR\n");
			   	if (file_ptr)
                	fclose(file_ptr);
            	done = TRUE;
            	ok = FALSE;
            }
            break;
                                       // Just in case
        	default:
        	{
            	m_errman.RecordError("File transfer state error");
	            state = XFER_TRANSFER_ERROR;
        	}
            break;

    	} // end of switch(state) 
        
    }  // end of while(!done) 
    
    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// GetFile
// _______
// 
// Does initializations and housekeeping for LocalFileGetFromRemote.

BOOL CInriFtpMan::GetFile
				(LPCTSTR remote_hostname_str,
                 LPCTSTR remote_file_name_str,
                 LPCTSTR local_file_name_str,
                 BOOL is_text,
                 int protection_mask)  
{



	TRACE3("ENTERING FTPMAN->GetFile to transfer remote file %s\n\t from host %s to local file %s\n",
	       remote_file_name_str, remote_hostname_str, local_file_name_str);
	       
    m_errman.ClearError();
    BOOL ok = TRUE;
    
	if (!m_is_ok)
	{
    	m_errman.RecordError("Not properly initialized");
    	ok = FALSE;
	}
	
	if ( (ok) &&
		 (strlen(remote_hostname_str) > (XFER_NAMES_BUF_LEN-1)) ||
		 (strlen(remote_file_name_str) > (XFER_NAMES_BUF_LEN-1)) ||
		 (strlen(local_file_name_str) > (XFER_NAMES_BUF_LEN-1)) )
	{
  		m_errman.RecordError("Host and/or file name length error");
    	return(FALSE);
	}
    
    char tmp_host_name_str[XFER_NAMES_BUF_LEN];
    strcpy(tmp_host_name_str, remote_hostname_str);
    
    if ( (ok) &&
    	 (!SockInit(tmp_host_name_str)) )
    {   
       	// m_errman.RecordError done in SockInit
       	ok = FALSE;
    }


// WSPTODO - note that for now not using mode_val

    if ( (ok) &&
		 (!LocalFileGetFromRemote(remote_file_name_str, 
    				 			  local_file_name_str, 
    				 			  is_text)) )
    {   
       	// m_errman.RecordError done in SockInit
       	ok = FALSE;
    }
         
    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// Initialize
// ____________
//
// Does the UNnecessary initializations to prepare for file transfer.
// Really could be done in the constructor
//

BOOL CInriFtpMan::Initialize()
{
	

    struct servent *servent_ptr;
                                       // Get protocol info & set m_port_num
                                       //
    if ((servent_ptr = (struct servent *)
						::getservbyname(XFER_SERVER_NAME,_T("tcp"))) 
        == NULL) 
        
        m_port_num = XFER_DEFAULT_PORT;

    else
        m_port_num = (int)(servent_ptr->s_port);
    
    if (m_port_num < 0)

        m_port_num = XFER_DEFAULT_PORT;
    
                                        // Save host name
                                        //
    ::gethostname(m_lcl_hostname_str, XFER_NAMES_BUF_LEN);
    
    char *dot_ptr;   
    if ((dot_ptr = strchr(m_lcl_hostname_str, (int)'.')) != NULL)
        (*dot_ptr) = NULL;

	AfxSocketInit();

	m_win_sock=new CTimeoutSocket();
	if(!m_win_sock->Create())
	{
		m_errman.RecordError(_T("Ftpman: Unable to create socket."));
		TRACE0(_T("Ftpman: Unable to create socket\n"));
		return FALSE;
	}


    m_errman.ClearError();
                                       // Okay to continue now
	m_is_ok = TRUE;

    return TRUE;
}    

/////////////////////////////////////////////////////////////////////////////
// PacketRead
// __________
//
// Reads a SrawDataBuffer from m_win_sock into a SrawDataBuffer and
// then convert that structure's values to the fixed_data's
// SfixedDataBuffer values.

int CInriFtpMan::PacketRead (SfixedDataBuffer *fixed_data)
{
    static SrawDataBuffer raw_data;
    char *data_ptr = (char *)&raw_data;     

    BOOL done = FALSE;
    BOOL ok = TRUE;
    int  total_bytes_recvd = 0;
    int  recv_cnt;
        
    memset(&raw_data, NULL, sizeof(SrawDataBuffer));

	recv_cnt = ReadGenericPacket((char*)&raw_data,XFER_HEADER_SIZE,XFER_MAX_SEGMENT);

	if ((recv_cnt <= 0) || (atoi(raw_data.len_buf) < 0))
        return(XFER_ERROR_CODE);

	fixed_data->cmd_val = atoi(raw_data.cmd_buf);
    strcpy(fixed_data->file_name_buf,raw_data.file_name_buf);
    fixed_data->gid_val = atoi(raw_data.gid_buf);
    fixed_data->uid_val = atoi(raw_data.uid_buf);
    fixed_data->file_mode_val = atoi(raw_data.file_mode_buf);
    strcpy(fixed_data->hostname_buf, raw_data.hostname_buf);
    fixed_data->len_val = atoi(raw_data.len_buf);    
    

	if (fixed_data->len_val > 0) {
		recv_cnt = ReadGenericPacket(raw_data.data_buf,fixed_data->len_val,XFER_MAX_SEGMENT);
		if (recv_cnt <= 0) 
	        return(XFER_ERROR_CODE);

	}
    memcpy(fixed_data->data_buf, raw_data.data_buf, fixed_data->len_val);



	
    return(total_bytes_recvd);
  
        
}



/////////////////////////////////////////////////////////////////////////////
// PacketWrite
// ___________
//
// Converts fixed_data's SfixedDataBuffer values into raw_data's
// a SrawDataBuffer and writes raw_data to m_win_sock. 

BOOL CInriFtpMan::PacketWrite (SfixedDataBuffer *fixed_data)
{
                                       // Static so have to bother
                                       //  with allocating space only when
                                       //  class is first constructed
    static SrawDataBuffer raw_data;
    int xfer_cnt;
                                       // Translate data from fixed_data to
                                       //  raw_data structures
                                       //

    memset((void *)&raw_data, NULL, sizeof(SrawDataBuffer));

    strcpy(raw_data.hostname_buf, m_lcl_hostname_str);
    
    sprintf(raw_data.gid_buf, "%-d", fixed_data->gid_val);
    sprintf(raw_data.uid_buf, "%-d", fixed_data->uid_val);
    sprintf(raw_data.len_buf, "%-d", fixed_data->len_val);
    sprintf(raw_data.file_mode_buf, "%-d", fixed_data->file_mode_val);
    
    strcpy(raw_data.file_name_buf, fixed_data->file_name_buf);

    sprintf(raw_data.cmd_buf, "%-d", fixed_data->cmd_val); 
    
    memcpy(&raw_data.data_buf, fixed_data->data_buf, XFER_DATABUF_SIZE);

                                       // Now send raw_data to server 
    BOOL ok = TRUE;

    
	xfer_cnt = WriteGenericPacket((char *)&raw_data,
								   250);
    
    if (xfer_cnt != 250) 
    { 
            ok = FALSE;
    }

	if (ok && fixed_data->len_val > 0) {

		xfer_cnt = WriteGenericPacket((char *)raw_data.data_buf,
								   fixed_data->len_val);
		    if (xfer_cnt != fixed_data->len_val) 
			{ 
				ok = FALSE;
			}
	}

    return(ok);
}

 
 
/////////////////////////////////////////////////////////////////////////////
// PutFile
// _______
// 
// Does initializations and housekeeping for LocalFilePutToRemote.
//

int CInriFtpMan::PutFile
					  (LPCTSTR remote_hostname_str,
                       LPCTSTR remote_file_name_str,
                       LPCTSTR local_file_name_str,
                       BOOL is_text,
					   int protection_mask,BOOL append/*FALSE*/)  
{                                                                

	
    m_errman.ClearError();
    BOOL ok = TRUE;
    
	if (!m_is_ok)
	{
    	m_errman.RecordError("Not properly initialized");
    	ok = FALSE;
	}
	
	if ( (ok) &&
		 (strlen(remote_hostname_str) > (XFER_NAMES_BUF_LEN-1)) ||
		 (strlen(remote_file_name_str) > (XFER_NAMES_BUF_LEN-1)) ||
		 (strlen(local_file_name_str) > (XFER_NAMES_BUF_LEN-1)) )
	{
  		m_errman.RecordError("Host and/or file name length error");
    	return(FALSE);
	}
    
    char tmp_host_name_str[XFER_NAMES_BUF_LEN];
    strcpy(tmp_host_name_str, remote_hostname_str);
    
    if ( (ok) &&
    	 (!SockInit(tmp_host_name_str)) )
    {   
       	// m_errman.RecordError done in SockInit
       	ok = FALSE;
    }

// WSPTODO - note that for now not using mode_val

    if ( (ok) &&

		 (!LocalFilePutToRemote(remote_file_name_str, 
    				 			local_file_name_str, 
    				 			is_text,append)) )
    {   
       	// m_errman.RecordError done in SockInit
       	ok = FALSE;
    }
         
// WSPTODO - do something better here?, had been having problems with
//			 timing on server side and this seems to correct 10/96
// WSPTODO - Need to get equivilent for Sleep under 16 bit

#ifdef NTCSS_32BIT_BUILD
	Sleep(1000);
#endif
    return(ok);
}  
    


/////////////////////////////////////////////////////////////////////////////
// SendCommand
// ___________
//
// Puts the given cmd and filename_str into a SfixedDataBuffer and
// then does a PacketWrite of it
//
BOOL CInriFtpMan::SendCommand(int cmd, const char *filename_str)
{ 
                                       // Static so have to bother
                                       //  with allocating space only when
                                       //  class is first constructed
    static SfixedDataBuffer fixed_data;

   
    memset(&fixed_data, NULL, sizeof(SfixedDataBuffer));
                      
    strcpy(fixed_data.hostname_buf, m_lcl_hostname_str);
    fixed_data.gid_val 		= 0;
    //fixed_data.uid_val 		= 0;
	fixed_data.uid_val = m_nUnixID;  //jgj 071597
    fixed_data.len_val 		= 0;

// WSPTODO - for now mode is set here?

    fixed_data.file_mode_val  = 0666;

    strcpy(fixed_data.file_name_buf, filename_str);
    fixed_data.cmd_val = cmd;

    BOOL ok = TRUE;

  
    if (!PacketWrite(&fixed_data))
    {
        ok = FALSE;
    }


    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// SockInit
// ________
//
// Sets	up m_win_sock with a connection to the host given by
// hostname_str.

BOOL CInriFtpMan::SockInit(LPCTSTR hostname_str)
{

   TRACE0("In CInriFtpMan->SockInit()\n");
	    
   struct hostent* hp;
   hp=::gethostbyname(hostname_str);

   if(hp && m_win_sock->connect(hp->h_name,m_port_num,5))
	   return TRUE;
   else
   {
	   TRACE2(_T("Ntcss Socket Connect failed to %s... Error (%d)\n"),hostname_str,GetLastError());
	   return FALSE;
   }
  
}

//---------------------------------------------------------------
//---------------------------------------------------------------
int CInriFtpMan::ReadGenericPacket(char *bptr,int size,int bsize)
{
char *ptr;
int tsize,ttotal;

   	ptr=bptr;
	ttotal=0;
	do {
		if (size - ttotal >= bsize)
			tsize = bsize;
		else
			tsize = size - ttotal;

        tsize = m_win_sock->Receive(ptr, tsize, 0);  
        
        if (tsize == SOCKET_ERROR) 
			return(-1);
		ttotal += tsize;
		ptr += tsize;
	} while (ttotal < size);

	return(ttotal);
}

//------------------------------------------------------
//------------------------------------------------------
int CInriFtpMan::WriteGenericPacket(char *addr,int size)
{
int tsize;
char *tptr;
int ttotal;

	tptr=addr;
	ttotal=0;
	tsize=0;
	do {
		tsize=size - ttotal;

		tsize = m_win_sock->Send((char *)tptr,tsize,0);

		if (tsize == SOCKET_ERROR) 
		{ 
			if (m_win_sock->GetLastError() != WSAEWOULDBLOCK) 
			{
	            m_win_sock->Close();
				break;
		    }
		}

		tptr++;
		ttotal += tsize;
	} while (ttotal < size);

	return(ttotal);
}





                                   
