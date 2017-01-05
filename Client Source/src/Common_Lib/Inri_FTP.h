/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\inri_ftp.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the CInriFtpMan class and related
// declarations.  The CInriFtpMan implements client/server file transfer
// services using a protocol that is NOT 'well known'.  
//
// Credit goes to Tony Abmrose for work which led to the creation of
// this class.
//
// See implementation file for details.
//----------------------------------------------------------------------

#ifndef _INCLUDE_INRI_FTP_CLASS_
#define _INCLUDE_INRI_FTP_CLASS_

#include "TimeoutSocket.h"

#include "err_man.h"

									   // Name of server
									   //
#define		XFER_SERVER_NAME	_T("netxfer")

									   // Transfers 4K + packet info
									   //   for each read/write
#define     XFER_DATABUF_SIZE    262144

#define     XFER_MAX_SEGMENT	 8192
									   // File & host names are this long
#define     XFER_NAMES_BUF_LEN    100
									   // Int values are 10 chars long
#define     XFER_INT_BUF_LEN       10
									   // 5 seconds timeout
#define     XFER_TIMER_VAL       5000   

#define     XFER_TIMER_ID         217

#define     XFER_DEFAULT_PORT    1926

                                       // Port numbers
                                       //
#define     XFER_HOST_PORT_NUM       1925
#define     XFER_RESP_PORT_NUM       1926
#define     XFER_PWD_PORT_NUM        1927
#define     XFER_PWD_RESP_PORT_NUM   1928

                                       // State defs 
#define     XFER_INIT_WRITE      0
#define     XFER_OPEN_LOCAL      1
#define     XFER_OPEN_REMOTE     2
#define     XFER_GET_RESP        3
#define     XFER_READ_LOCAL      4
#define     XFER_WRITE_REMOTE    5
#define     XFER_CLOSE           6
#define     XFER_DONE            7
#define     XFER_INIT_READ       8

#define     XFER_READ_REMOTE     10
#define     XFER_WRITE_LOCAL     11
#define     XFER_CLOSE_LOCAL     12 
#define     XFER_INIT_PWD        13
#define     XFER_SEND_PWENT      14

#define     XFER_TRANSFER_ERROR  99

                                       // File transfer Primitive codes 
#define     XFER_OPEN_NC             0
#define     XFER_CLOSE_NC            1 
#define     XFER_OPEN_READ_NC        2
#define     XFER_WRITE_NC            3
#define     XFER_READ_NC             4
#define     XFER_CLOSE_READ_NC       5
#define     XFER_UPDATE_FILE         6
#define     XFER_SET_PWENT           7
#define     XFER_OPEN_NC_APPEND      8


                                       // Response codes
#define     XFER_OK_PACK         0
#define     XFER_NOK_PACK        1
#define     XFER_EOF_PACK        2

#define		XFER_ERROR_CODE		-1

                                       // SfixedDataBuffer is same as
									   //  SrawDataBuffer below but
									   //  here integers have been
									   //  converted 
typedef struct SfixedDataBuffer 
{
    int    cmd_val;
    char   file_name_buf[XFER_NAMES_BUF_LEN];
    int    gid_val;
    int    uid_val;
    int    file_mode_val;
    char   hostname_buf[XFER_NAMES_BUF_LEN];
    long   len_val;
    char   data_buf[XFER_DATABUF_SIZE];
}
SfixedDataBuffer;
									   
									   // SrawDataBuffer is the
									   //  structure that goes across
									   //  the socket.
typedef struct SrawDataBuffer 
{
    char  cmd_buf[XFER_INT_BUF_LEN];
    char  file_name_buf[XFER_NAMES_BUF_LEN];
    char  gid_buf[XFER_INT_BUF_LEN];
    char  uid_buf[XFER_INT_BUF_LEN];
    char  file_mode_buf[XFER_INT_BUF_LEN];
    char  hostname_buf[XFER_NAMES_BUF_LEN];
    char  len_buf[XFER_INT_BUF_LEN];
    char  data_buf[XFER_DATABUF_SIZE];
}
SrawDataBuffer;

#define     XFER_HEADER_SIZE     sizeof(SrawDataBuffer)-XFER_DATABUF_SIZE

/////////////////////////////////////////////////////////////////////////////
// CInriFtpMan
// ___________

class CInriFtpMan : public CWnd
{
private:
    
    BOOL m_is_ok;
    
    CTimeoutSocket* m_win_sock;

    CErrorManager     m_errman;

    int  m_port_num;

	int m_nUnixID;

	char m_lcl_hostname_str[XFER_NAMES_BUF_LEN];
    

    BOOL LocalFileGetFromRemote(const char *remote_file_name_str,
                      			const char *local_file_name_str,
                      			BOOL is_text);

    BOOL LocalFilePutToRemote(const char *remote_file_name_str,
                      		  const char *local_file_name_str,
                      		  BOOL is_text,BOOL append=FALSE);
                 
    int  PacketRead(SfixedDataBuffer *fixed_data);

    BOOL PacketWrite(SfixedDataBuffer *fixed_data);

	BOOL SendCommand(int cmd, const char *filename_str); 

    BOOL SockInit(LPCTSTR hostname_str);

	int ReadGenericPacket(char *bptr,int size,int bsize);

	int WriteGenericPacket(char *bptr,int size);

public:

    CInriFtpMan() 
    	{ m_nUnixID=0;};

    virtual ~CInriFtpMan();

    const char *GetLastErrorStr()
        { return (m_errman.GetLastErrorMsgText()); }; 

    BOOL GetFile(LPCTSTR remote_hostname_str,
                 LPCTSTR remote_file_name_str,
                 LPCTSTR local_file_name_str,
                 BOOL is_text,
				 int  protection_mask);
  
    BOOL Initialize(); 

    BOOL PutFile(LPCTSTR remote_hostname_str,
                 LPCTSTR remote_file_name_str,
                 LPCTSTR local_file_name_str,
                 BOOL is_text,
                 int protection_mask,BOOL append=FALSE);

	void SetUnixID(const int UnixID)
	{m_nUnixID=UnixID;}; //jgj 051597

	

									   // Functions implemented if
									   // doing _NTCSS_FTP_MAN_TRACING_



};

#endif // _INCLUDE_INRI_FTP_CLASS_