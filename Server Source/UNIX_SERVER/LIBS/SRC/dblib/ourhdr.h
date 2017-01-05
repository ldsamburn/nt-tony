
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * ourhdr.h
 */

#ifndef	__ourhdr_h
#define	__ourhdr_h

#include	<sys/types.h>	/* required for some of our prototypes */
#include	<stdio.h>		/* for convenience */
#include	<stdlib.h>		/* for convenience */
#include	<string.h>		/* for convenience */
#include	<unistd.h>		/* for convenience */

#define	MAXLINE	4096			/* max line length */



int		lock_reg( int, int, int, off_t, int, off_t );

#define	read_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define	readw_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)
#define	write_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define	writew_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)
#define	un_lock(fd, offset, whence, len) \
			lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)

void	err_dump(const char *, ...);
void	err_msg(char *, ...);
void	err_quit(const char *, ...);
void	err_ret(char *, ...);
void	err_sys(const char *, ...);

#endif	/* __ourhdr_h */
