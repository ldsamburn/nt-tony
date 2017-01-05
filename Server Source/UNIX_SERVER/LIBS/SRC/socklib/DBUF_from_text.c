
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* DBUF_from_text.c */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <string.h>

/** INRI INCLUDES **/

#include <NetCopy.h>


/*****************************************************************************/

int  DBUF_from_text( struct DBUF_IN  *inbuf,
                     struct DBUF     *bptr )
{

  bptr->cmd = atoi(inbuf->cmd);
  strncpy(bptr->file_name, inbuf->file_name, HOSTNAMELEN);
  strncpy(bptr->hostname, inbuf->hostname, HOSTNAMELEN);
  bptr->uid = atoi(inbuf->uid);
  bptr->gid = atoi(inbuf->gid);
  bptr->file_mode = atoi(inbuf->file_mode);
  bptr->len = atoi(inbuf->len);
  memcpy(bptr->buf, inbuf->buf, BUF_SIZE);

  return(0);
}


/*****************************************************************************/
