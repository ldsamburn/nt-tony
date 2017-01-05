
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * writeptr.c
 */

#include	"db.h"

/* Write a chain ptr field somewhere in the index file:
   the free list, the hash table, or in an index record. */

void  _db_writeptr(DB *db, off_t offset, off_t ptrval)

{
  char	asciiptr[PTR_SZ + 1];

  if (ptrval < 0 || ptrval > PTR_MAX)
    err_quit("invalid ptr: %ld", (long)ptrval);

  sprintf(asciiptr, "%*ld", PTR_SZ, (long)ptrval);

  if (lseek(db->idxfd, offset, SEEK_SET) == -1)
    err_dump("lseek error to ptr field");
  if (write(db->idxfd, asciiptr, PTR_SZ) != PTR_SZ)
    err_dump("write error of ptr field");
}
