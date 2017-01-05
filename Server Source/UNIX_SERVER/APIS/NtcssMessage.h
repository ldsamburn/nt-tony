/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/************************************************************************************************************/
/* NtcssMessage.h                                                                                       */
/************************************************************************************************************/

#ifndef _NtcssMessage_h
#define _NtcssMessage_h

typedef struct {
  void (*associate)(void*);
  void (*open)(const char*, int, const char*, const char*, 
               const char*, int, int);
  void (*write)(const char*, int);
  void (*read)(void);
  void (*close)(void);
  int (*timeout)(void);
  /******************/
  const char *accept_label;
  const char *reject_label;
  int response_size;
  char *response;
  char *response_ptr;
  int response_allocated;
} NtcssMessage_class;

extern NtcssMessage_class NtcssMessage;

#endif /* _NtcssMessage_h */
