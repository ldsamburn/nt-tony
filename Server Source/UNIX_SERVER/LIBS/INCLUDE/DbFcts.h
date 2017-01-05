
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * DbFcts.h
 */

#ifndef _DbFcts_h
#define _DbFcts_h

/** SYSTEM INCLUDES **/

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <stddef.h>



/** INRI INCLUDES **/




/** DEFINE DEFINITIONS **/

#define DB_INSERT       1               /* insert new record only */
#define DB_REPLACE      2               /* replace existing record */

        /* default file access permissions for new files */
#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


/** TYPEDEF DEFINITIONS **/

typedef struct _DB {    /* our internal structure */
  int   idxfd;          /* fd for index file */
  int   datfd;          /* fd for data file */
  int   oflag;          /* flags for open()/dbOpen(): O_xxx */
  char  *idxbuf;        /* malloc'ed buffer for index record */
  char  *datbuf;        /* malloc'ed buffer for data record*/
  char  *name;          /* name db was opened under */
  off_t idxoff;         /* offset in index file of index record */
			   /* actual key is at (idxoff + PTR_SZ + IDXLEN_SZ) */
  size_t idxlen;        /* length of index record */
                        /* excludes IDXLEN_SZ bytes at front of index record */
                        /* includes newline at end of index record */
  off_t datoff;         /* offset in data file of data record */
  size_t datlen;        /* length of data record, includes newline at end */
  off_t ptrval;         /* contents of chain ptr in index record */
  off_t ptroff;      /* offset of chain ptr that points to this index record */
  off_t chainoff;       /* offset of hash chain for this index record */
  off_t hashoff;        /* offset in index file of hash table */
  int   nhash;          /* current hash table size */
  long  cnt_delok;      /* delete OK */
  long  cnt_delerr;     /* delete error */
  long  cnt_fetchok;    /* fetch OK */
  long  cnt_fetcherr;   /* fetch error */
  long  cnt_nextrec;    /* nextrec */
  long  cnt_stor1;      /* store: DB_INSERT, no empty, appended */
  long  cnt_stor2;      /* store: DB_INSERT, found empty, reused */
  long  cnt_stor3;      /* store: DB_REPLACE, diff data len, appended */
  long  cnt_stor4;      /* store: DB_REPLACE, same data len, overwrote */
  long  cnt_storerr;    /* store error */
} DB;




/** GLOBAL DECLARATIONS **/

/* open.c */
DB *dbOpen(const char*, int, int);

/* close.c */
void dbClose(DB*);

/* fetch.c */
char *dbFetch(DB*, const char*);

/* store.c */
int dbStore(DB*, const char*, char*, int);

/* delete.c */
int dbDelete(DB*, const char*);

/* rewind.c */
void dbRewind(DB*);

/* nextrec.c */
char *dbNextrec(DB*, char*);

/* stats.c */
void dbStats(DB*);


#endif /* _DbFcts_h */
