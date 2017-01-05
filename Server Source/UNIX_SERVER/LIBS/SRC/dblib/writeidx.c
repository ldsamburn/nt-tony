
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * writeidx.c
 */

#include	"db.h"
#include	<sys/uio.h>		/* struct iovec */

/* Write an index record.
 * _db_writedat() is called before this function, to set the fields
 * datoff and datlen in the DB structure, which we need to write
 * the index record. */

void
_db_writeidx(db, key, offset, whence, ptrval)
DB *db;
const char *key;
off_t offset;
int whence;
off_t ptrval;
{
	struct iovec	iov[2];
	char			asciiptrlen[PTR_SZ + IDXLEN_SZ +1];
	int				len;

	if ( (db->ptrval = ptrval) < 0 || ptrval > PTR_MAX)
		err_quit("invalid ptr: %d", ptrval);

	sprintf(db->idxbuf, "%s%c%ld%c%d\n",
	 	key, SEP, (long)db->datoff, SEP,
                (int)db->datlen);
	if ( (len = strlen(db->idxbuf)) < IDXLEN_MIN || len > IDXLEN_MAX)
		err_dump("invalid length");
	sprintf(asciiptrlen, "%*ld%*d", PTR_SZ, (long)ptrval, IDXLEN_SZ, len);

		/* If we're appending, we have to lock before doing the lseek()
		   and write() to make the two an atomic operation.  If we're
		   overwriting an existing record, we don't have to lock. */
	if (whence == SEEK_END)		/* we're appending */
		if (writew_lock(db->idxfd, ((db->nhash+1)*PTR_SZ)+1,
													SEEK_SET, 0) < 0)
			err_dump("writew_lock error");

		/* Position the index file and record the offset */
	if ( (db->idxoff = lseek(db->idxfd, offset, whence)) == -1)
		err_dump("lseek error");

	iov[0].iov_base = asciiptrlen;
	iov[0].iov_len  = PTR_SZ + IDXLEN_SZ;
	iov[1].iov_base = db->idxbuf;
	iov[1].iov_len  = len;
	if (writev(db->idxfd, &iov[0], 2) != PTR_SZ + IDXLEN_SZ + len)
		err_dump("writev error of index record");

	if (whence == SEEK_END)
		if (un_lock(db->idxfd, ((db->nhash+1)*PTR_SZ)+1, SEEK_SET, 0) < 0)
			err_dump("un_lock error");
}
