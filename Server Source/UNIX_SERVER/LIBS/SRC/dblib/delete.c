
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * delete.c
 */

#include    "db.h"

/* Delete the specified record */

int  dbDelete( DB          *db,
               const char  *key )

{
    int     rc;

    if (_db_find(db, key, 1) == 0) {
        rc = _db_dodelete(db);  /* record found */
        db->cnt_delok++;
    } else {
        rc = -1;                /* not found */
        db->cnt_delerr++;
    }

    if (un_lock(db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
        err_dump("un_lock error");
    return(rc);
}
