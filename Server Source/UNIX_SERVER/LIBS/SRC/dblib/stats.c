
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * stats.h
 */

#include	"db.h"
#include	<stdio.h>

/* Print and reset the accumulated statistics. */

void
dbStats(DB *db)
{
	fprintf(stderr, "%ld: del=%ld+%ld, fetch=%ld+%ld, next=%ld, "
					"store=%ld(%ld+%ld+%ld+%ld+%ld)\n", getpid(),
				db->cnt_delok, db->cnt_delerr,
				db->cnt_fetchok, db->cnt_fetcherr,
				db->cnt_nextrec,
				(db->cnt_stor1 + db->cnt_stor2 + db->cnt_stor3 + 
				 db->cnt_stor4 + db->cnt_storerr),
				db->cnt_stor1, db->cnt_stor2, db->cnt_stor3, 
				db->cnt_stor4, db->cnt_storerr);

	db->cnt_delok = db->cnt_delerr =
	db->cnt_fetchok = db->cnt_fetcherr = db->cnt_nextrec =
	db->cnt_stor1 = db->cnt_stor2 = db->cnt_stor3 =
	db->cnt_stor4 = db->cnt_storerr = 0;
}
