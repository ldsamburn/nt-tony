/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  NtcssServerSocket.h
*/

#ifndef _NtcssServerSocket_h
#define _NtcssServerSocket_h

#include "NtcssApiP.h"
#include "NtcssMacros.h"

typedef struct NtcssServerSocket {
  void (*open)(const char*, int, int);
  void (*read)(char*, int);
  void (*write)(char*, int);
  void (*close)(void);
  void (*get_master_server)(char*);
  /**********************/
  int   server_socket;
  int   timeout_seconds;
  int   opened;
  int   ok;
  int   bytes_read;
  int   bytes_written;
  char   host_name[MAX_HOST_NAME_LEN];
} NtcssServerSocket_class;

extern NtcssServerSocket_class  NtcssServerSocket;

#endif /* _NtcssServerSocket_h */









