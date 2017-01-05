
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * rewind.c
 */

#include	"db.h"

/* Rewind the index file for dbNextrec().
 * Automatically called by dbOpen().
 * Must be called before first dbNextrec().
 */

void
dbRewind(db)
DB *db;
{
	off_t	offset;

	offset = (db->nhash + 1) * PTR_SZ;		/* +1 for free list ptr */

		/* We're just setting the file offset for this process
		   to the start of the index records; no need to lock.
		   +1 below for newline at end of hash table. */

	if ( (db->idxoff = lseek(db->idxfd, offset+1, SEEK_SET)) == -1)
		err_dump("lseek error");
}
