/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/***************************************************************************************************
  NtcssTimeoutAlarm.c
***************************************************************************************************/

/***************************************************************************************************
  System Headers
****************************************************************************************************/
#include <signal.h>
#include <unistd.h>

/****************************************************************************************************
  Local Headers
****************************************************************************************************/
#include <ExtraProtos.h>

#include "NtcssMacros.h"
#include "NtcssError.h"
#include "NtcssTimeoutAlarm.h"

/***************************************************************************************************
  Public Function Prototypes
***************************************************************************************************/
void activate(int);
void deactivate(void);
void signal_timeout(int);

/***************************************************************************************************
  Public Data
***************************************************************************************************/
NtcssTimeoutAlarm_class NtcssTimeoutAlarm = {activate,deactivate,0};

/***************************************************************************************************
  Private Data
***************************************************************************************************/
static NtcssTimeoutAlarm_class *this = &NtcssTimeoutAlarm;

/***************************************************************************************************
  Public Function Definitions
****************************************************************************************************/

void signal_timeout(int signo)
{
    signo=signo;
    NtcssError.set_error_code(ERROR_SERVER_TIMED_OUT);
    this->tripped = True;
}

/*************************************************************************************************/

void activate(int number_of_seconds)
{
  if (signal(SIGALRM, signal_timeout) == SIG_ERROR) 
    NtcssError.set_error_code(ERROR_SETTING_TIMEOUT_ALARM);
  else {
    alarm(number_of_seconds);
    this->tripped = False;
  }
}

/**************************************************************************************************/

void deactivate()
{
  alarm(0);
}



