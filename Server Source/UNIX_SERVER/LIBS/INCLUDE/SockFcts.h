
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * SockFcts.h
 */

#ifndef	_SockFcts_h
#define	_SockFcts_h

/** DEFINE DEFINITIONS **/

#define NO_TIME_OUT		0
#define USE_TIME_OUT		1


/** GLOBAL DECLARATIONS **/
#ifdef __cplusplus
 extern "C"{
#endif

int  connect_client(int, const char*, int);
int  connect_server(int, const char*, int);
int  send_msg_with_response( const char *, char *, const char *, char *, int, int );
int  read_sock(int, char*, int, char, int);
int  read_stream(int, char*, int, char);
int  read_dgram(int, char*, int, char);
int  sock_init(const char*, int, int, int);
int  send_msg(const char*, char*, const char*, int);
int  getMasterServer(char*, int);
int  net_copy(const char *, const char *, const char *, const char *, int );
/*
       intnet_copy( operation,hostname,srcfile,destfile,ascii_flag )
	   where:
		operation :  get or put
		hostname  :  hostname of destination or source host
		srcfile   :  source file, pathname should be in native form
		destfile  :  destinaion file, pathname should be in native form
		ascii_flag:  determines if the transfer is ascii or binary. 
					 Values = XFER_ASCII or XFER_BINARY
*/

#ifdef __cplusplus
 };
#endif



#endif /* _SockFcts_h */
