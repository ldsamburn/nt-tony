
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * ExtraProtos.h
 */

#ifndef _ExtraProtos_h
#define _ExtraProtos_h

/* Defining signal prototypes since sys/signal.h incorrect for using protos. */
#define SIG_ERROR ((void (*) (int))-1)
#define SIG_IGNORE ((void (*) (int))1)

#ifndef linux
#ifdef SOLARIS
#include <sys/resource.h>

pid_t wait3( int *, int, struct rusage * );

int gethostname( char *, int );
#endif
#endif
#endif /* _ExtraProtos_h */

