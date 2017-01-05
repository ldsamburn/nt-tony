/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/************************************************************************************************************/
/* NtcssTimeoutAlarm.h                                                                                      */
/************************************************************************************************************/

#ifndef _NtcssTimeoutAlarm_h
#define _NtcssTimeoutAlarm_h

typedef struct {
  void (*activate)(int);
  void (*deactivate)(void);
  /********************/
  int tripped;
} NtcssTimeoutAlarm_class;

extern NtcssTimeoutAlarm_class NtcssTimeoutAlarm;

#endif /* _NtcssTimeoutAlarm_h */
