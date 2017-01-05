
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * db.h
 */

#include	<DbFcts.h>
#include	"ourhdr.h"

		/* magic numbers */
#define	IDXLEN_SZ	   4	/* #ascii chars for length of index record */
#define	IDXLEN_MIN	   6	/* key, sep, start, sep, length, newline */
#define	IDXLEN_MAX	1024	/* arbitrary */
#define	SEP			 ':'	/* separator character in index record */
#define	DATLEN_MIN	   2	/* data byte, newline */
#define	DATLEN_MAX	1024	/* arbitrary */

		/* following definitions are for hash chains and free list chain
		   in index file */
#define	PTR_SZ		   6	/* size of ptr field in hash chain */
#define	PTR_MAX   999999	/* max offset (file size) = 10**PTR_SZ - 1 */
#define	NHASH_DEF	 137	/* default hash table size */
#define	FREE_OFF	   0	/* offset of ptr to free list in index file */
#define	HASH_OFF  PTR_SZ	/* offset of hash table in index file */

typedef	unsigned long	hash_t;	/* hash values */


/* internal functions */

DB		*_db_alloc(int );
int		 _db_checkfree(DB * );
int		 _db_dodelete( DB * );
int		 _db_emptykey(char * );
int		 _db_find( DB *, const char *, int );
int		 _db_findfree( DB *, int, int );
int		 _db_free( DB * );
hash_t	 _db_hash( DB *, const char * );
char	*_db_nextkey( DB * );
char	*_db_readdat( DB * );
off_t	 _db_readidx( DB *, off_t );
off_t	 _db_readptr( DB *, off_t );
void	 _db_writedat( DB *, char *, off_t, int );
void	 _db_writeidx( DB *, const char *, off_t, int, off_t );
void	 _db_writeptr( DB *, off_t, off_t );
