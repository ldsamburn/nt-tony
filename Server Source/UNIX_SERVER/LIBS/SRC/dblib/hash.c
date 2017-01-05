
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * hash.c
 */

#include    "db.h"

/*****************************************************************************/
/* Calculate the hash value for a key. */

hash_t  _db_hash( DB          *db,
                  const char  *key )

{
  char         c;
  hash_t       hval;
  int          i;
  const char  *ptr;

  hval = 0;
  /*****
        for ( ptr = key, i = 1; c = *ptr++; i++)
        hval += c * i;      ascii char times its 1-based index
  GCC complains about complexity so we expanded this a little... *****/
  ptr = key;
  c = *ptr;

  for (i = 1; c != '\0'; i++)
    {
      hval += c * i;      /* ascii char times its 1-based index */
      ptr++;
      c = *ptr;
    }

  return(hval % db->nhash);
}
