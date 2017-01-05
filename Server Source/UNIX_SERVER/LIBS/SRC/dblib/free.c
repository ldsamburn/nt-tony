
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * free.c
 */

#include	"db.h"

/* Free up a DB structure, and all the malloc'ed buffers it
 * may point to.  Also close the file descriptors if still open. */

int
_db_free(db)
DB *db;
{
	if (db->idxfd >= 0 && close(db->idxfd) < 0)
		err_dump("index close error");
	if (db->datfd >= 0 && close(db->datfd) < 0)
		err_dump("data close error");
	db->idxfd = db->datfd = -1;

	if (db->idxbuf != NULL)
		free(db->idxbuf);
	if (db->datbuf != NULL)
		free(db->datbuf);
	if (db->name != NULL)
		free(db->name);
	free(db);
	return(0);
}
