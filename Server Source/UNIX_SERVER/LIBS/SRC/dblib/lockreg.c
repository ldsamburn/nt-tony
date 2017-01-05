
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * lockreg.c
 */

#include	<sys/types.h>
#include	<fcntl.h>
#include	"ourhdr.h"

int
lock_reg(fd, cmd, type, offset, whence, len)
int fd, cmd, type;
off_t offset;
int whence;
off_t len;
{
	struct flock	lock;

	lock.l_type = type;		/* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_start = offset;	/* byte offset, relative to l_whence */
	lock.l_whence = whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;		/* #bytes (0 means to EOF) */

	return( fcntl(fd, cmd, &lock) );
}
