
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* DBUF_to_text.c */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** INRI INCLUDES **/

#include <NetCopy.h>

/*****************************************************************************/

int  DBUF_to_text( struct DBUF    *addr,
                   struct DBUF_IN *dout )

{
  strncpy(dout->file_name, addr->file_name, HOSTNAMELEN);
  strncpy(dout->hostname, addr->hostname, HOSTNAMELEN);
  sprintf(dout->cmd, "%-d", addr->cmd);
  sprintf(dout->uid, "%-ld", (long)addr->uid);
  sprintf(dout->gid, "%-ld", (long)addr->gid);
#ifdef SOLARIS
  sprintf(dout->file_mode, "%-ld", addr->file_mode);
#else
  sprintf(dout->file_mode, "%-d", addr->file_mode);
#endif
  sprintf(dout->len, "%-d", addr->len);
  memcpy(dout->buf, addr->buf, BUF_SIZE);
  return(0);
}


/*****************************************************************************/
