
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* NTCSScrypt.c
   Binary file encryption and decryption using multiblock, single/double/triple
   length keys (Refer to d3des.h for more information)
   The following code uses triple length keys (24 bytes) and runs
   each 16-byte block through the DES algorithm three times.
****/

#include "d3des.h"
#include "NtcssCrypt.h"

#include <stdio.h>
#include <stdlib.h>   /* for system() calls */
#include <fcntl.h>    /* for read() & write() calls */
#include <sys/stat.h> /* for stat() */ 
#include <syslog.h>   /* for syslog() */
#include <unistd.h>
#include <string.h>


/*** For Later Modifications:

*  If you have reason to doubt that the en/de-cryption algorithms are 
*  functioning properly, test using the data vectors commented out at
*  the end of the d3des.c file.  For convenience sake,this can be
*  accomplished by uncommenting the appropriate Plaintext/Key combinations
*  already set up just below the variable declarations, and then
*  uncommenting the corresponding test code just below them to print
*  out the results.
*//*
*  For more information, refer to the d3des.h file
*  where there are comments from the author as to the code's use.
*//*
* To convert the following code to other modes DES (various
*  plaintext lengths)
*  look in the d3des.h file for the functions corresponding to MAKEKEY(),
*  DESKEY(), and DES() for the mode you want, then replace the function
*  calls below with their similarly named couterparts. Then test!
* Read the header file for brief function descriptions.  This code was
*  built for 16-byte block encryption (or double-length plaintext) and
*  would require a lot of modification to change it.  Changing key 
*  length only requires changing the correct key buffer size and using 
*  the correct makekey()/deskey() functions.

* Another pitfall will be the use of other makekey()-like functions: they
*  were originally designed to overwrite the userKey string for some reason.
*  The current make2key was modified to prevent this; all others used in the
*  future should be modified also.
*//*

*** IMPORTANT NOTES: ****

* int, long, and size_t are 4 bytes or 4,294,967,296(unsigned) on the testbed:
*  an HP series 700 running HPUX 9.07.
* Ints and longs are interchangeable in this code.
* Make sure that no values exceed the maximum values
*  of 4 byte ints, or two byte ints if that's how the host architecture
*  represents ints.
* IFF your system's "size_t" is only two bytes , you are going to run into
*  major problems when file size exceeds 65,535 bytes.  This is a small
*  file in most respects, except for ASCII files.
* The syslog facility was used here to log errors, whereas a printf might
    suffice for some general purposes, except where the decrypt to stdout
    function is concerned.
************/


/*****************************************************************************/
/**** FUNCTIONS *******/
/*****************************************************************************/
/* Creates and encrypts a buffer with the source_file's contents.  Caller must
   free the buffer returned.  The result_size param is an optional input.  For
   zero length files, the returned result will be 0 bytes.
*/

int  ntcssEncryptFileToMemory( const char      *source_file,
                               unsigned char  **result_buff,
                               unsigned long   *result_size,
                               const char      *user_key)

{
  int  file_dscrptr;                /* File descriptor for file operations */
  int  i;                           /* Generic increment or temporary value */
  int  pad_bytes = 0;               /* To pad lastBytes out to a full block */
  unsigned long  buffSize;          /* Size of the memBuff array */
  unsigned long  bytes_read;        /* Bytes read from source_file */
  unsigned char  d3key[24];         /* Triple-length DES key */
  unsigned long  lastDataBytes = 0; /* Bytes that wouldn't fill in a block */ 
  unsigned char *memBuff = NULL;    /* Buffer to hold file contents */
  unsigned long  numDataBlocks;     /* Number of 16-byte blocks for cryption */
  unsigned long  processedBytes;    /* Number of bytes to be written to file */
  struct   stat  statBuff;          /* For holding file information */
  

  /* do some parameter checking.. */
  if ((source_file == NULL) || (result_buff == NULL))
    {
      syslog(LOG_WARNING, "ntcssEncryptFileToMemory(): Bad input!");
      return(_BAD_POINTER);
    }

  i = stat(source_file, &statBuff);
  if (i < 0)
    {
      syslog(LOG_WARNING, "ntcssEncryptFileToMemory(): Failed to stat file"
             " <%s>!", source_file);
      return(_STAT_FAILED);
    }

  buffSize = (unsigned long) statBuff.st_size;

  if (buffSize == 0)
    {
      *result_buff = NULL;    /* In case the caller uses free() on this ptr. */
      if (result_size != NULL)         /* Set this optional param.. */
	*result_size = 0;
      return(_EMPTY_FILE);
    }

  buffSize++;   /* +1 for the pad num byte */

  /* First see if we can open the file before we do anything else.. */
  file_dscrptr = open(source_file, O_RDONLY, 0);
  if (file_dscrptr < 0)
    {
      syslog(LOG_WARNING,"ntcssEncryptFileToMemory(): Failed to open"
             " file <%s>!", source_file);
      return(_FILE_OPEN_FAILED);
    }

  /* Break up the file's data into the required 16-byte blocks.. */
  numDataBlocks = (buffSize / 16);  /* # complete 16-byte blocks we have. */
  lastDataBytes = (buffSize % 16);  /* # bytes shy of even 16-byte multiple.*/
  if (lastDataBytes > 0)
    {
      numDataBlocks++;
      pad_bytes = 16 - lastDataBytes;       /* Calculate the padding needed. */
      buffSize += pad_bytes;      /* Should be an even 16-byte mulitple now. */
    }

  /* Make room for any extra ID_BYTES at the beginning of the buffer..*/
  buffSize += NUM_ID_BYTES;

  memBuff = (unsigned char *) malloc((size_t)buffSize);
  if (memBuff == NULL)
    {
      close(file_dscrptr);
      syslog(LOG_WARNING, "ntcssEncryptFileToMemory(): Failed to  malloc"
             " enough memory for file <%s>!", source_file);
      return(_MALLOC_FAILED);
    }

  bytes_read = read(file_dscrptr, &memBuff[NUM_ID_BYTES], statBuff.st_size);
  close(file_dscrptr);
  
  if(bytes_read != (unsigned long)statBuff.st_size)
    {
      free(memBuff);
       syslog(LOG_WARNING,"ntcssEncryptFileToMemory(): unsuccessfully read"
              " file <%s>", source_file);
      return(_FILE_READ_FAILED);
    }

    /* Add some random stuff in for the padding  */
  for (i = 0; i < pad_bytes; i++)
    memBuff[NUM_ID_BYTES + bytes_read + i] = (unsigned char) i;

    /* Record the total number of extra bytes added (including pad num byte)*/
  memBuff[buffSize - 1] = (unsigned char) (pad_bytes + 1);
  
  make3key(user_key, d3key);
  des3key(d3key, EN0);

  if (NUM_ID_BYTES > 0)
    /* Prepend the ID_BYTES to the final result.. */
    memcpy(memBuff, ID_BYTES, (size_t) NUM_ID_BYTES);

  processedBytes = NUM_ID_BYTES; /* Start encrypting AFTER the ID_BYTES */
  for (i = 0; (unsigned long)i < numDataBlocks; i++)
    {
      D2des(&memBuff[processedBytes],
	    &memBuff[processedBytes]);
      processedBytes += 16;
    }

  if (result_size != NULL)      /* Set the optional param.. */
    *result_size = buffSize;

  *result_buff = memBuff;
  return(0);
}


/*****************************************************************************/
/* Takes the contents of the sourceFile and writes its encrypted counterpart
   to the dest_filename.
   DestinationFile is created (or overwritten) with permissions 0600.
 */

int  ntcssEncryptFileToFile( const char  *source_file,
                             const char  *dest_filename,
                             const char  *user_key)

{
  int  file_dscrptr;                /* File descriptor for file operations */
  int  i;                           /* Generic increment or temporary value */
  unsigned char *crypted_data;      /* Buffer containing encrypted data. */
  unsigned long bytes_to_write;     /* Number of bytes to be written to file */
  
  /* First, get the file data encryted into memory.. */
  i = ntcssEncryptFileToMemory(source_file, &crypted_data, &bytes_to_write,
                               user_key);
  if (i < 0)
    return(i);

  if (i == _EMPTY_FILE)
    {
      /* If destfile == source file, then we don't do anything in this case. */
      if (dest_filename == NULL)
	return(0);

      /* For output, all we want to do is create an empty file. */
      file_dscrptr = open(dest_filename, O_WRONLY|O_CREAT|O_TRUNC, 0600);
      if (file_dscrptr < 0)
        {
          syslog(LOG_WARNING,"ntcssEncryptFileToFile() could not create file "
                 "<%s>!", dest_filename);
          return(_FILE_OPEN_FAILED);
        }
      close(file_dscrptr);
      return(0);
    }

  /* Next, store the memory buffer's contents to the specified file.. */
  if (dest_filename == NULL)
    dest_filename = source_file; /* Implies overwriting the original file. */

  file_dscrptr = open(dest_filename, O_WRONLY|O_CREAT|O_TRUNC, 0600);
  if (file_dscrptr < 0)
    {
      free(crypted_data);
      syslog(LOG_WARNING,"ntcssEncryptFileToFile() could not open file <%s>"
             " for writing!", dest_filename);
      return(_FILE_OPEN_FAILED);
    }

  i = write(file_dscrptr, crypted_data, bytes_to_write);

  free(crypted_data);
  close(file_dscrptr);  

  if ((unsigned long)i != bytes_to_write)
    {
      syslog(LOG_WARNING, "ntcssEncryptFileToFile() unsuccessfully"
             " wrote file <%s>", dest_filename);
      remove(dest_filename);                     /* Clean up just in case. */
      return (_FILE_WRITE_FAILED);
    }

  return (0);
}


/*****************************************************************************/
/* Takes plain_data pointer contents and sets the encrypted_data pointer to a
   malloc'd memory buffer containing the encrypted version.
 */

int  ntcssEncryptMemory( char           *plain_data,
                         unsigned long   plain_data_size,
                         char          **encrypted_data,
                         unsigned long  *encrypted_data_size,
                         const char     *encryption_key)

{
  int   i;                          /* Generic increment or temporary value */
  int   pad_bytes = 0;              /* To pad lastBytes out to a full block */
  unsigned char  *buff_ptr;         /* Triple-length DES key */
  unsigned char  d3key[24];         /* Triple-length DES key */
  unsigned long  buffSize;          /* Size of the memBuff array */
  unsigned long  lastDataBytes = 0; /* Bytes that wouldn't fill in a block */ 
  unsigned long  numDataBlocks;     /* Number of 16-byte blocks for cryption */
  unsigned long  processedBytes;    /* Number of bytes already encrypted */
  
  if (plain_data_size <= 0)
    {
      syslog(LOG_WARNING, "ntcssEncryptMemory(): Empty plain_data string "
             "encountered!");
      return(_NO_PLAIN_DATA);
    }

  buffSize = plain_data_size;  
  buffSize ++;      /* add 1 extra space to record the padding length byte */

  numDataBlocks = (buffSize / 16);   /* # of 16-byte blocks to encrypt */
  lastDataBytes = (buffSize % 16);   /* # of extra bytes needed for padding */
  if (lastDataBytes > 0)
    {
      numDataBlocks++;
      pad_bytes = 16 - lastDataBytes;
      buffSize += pad_bytes;
    }

  /* Make room for any extra ID_BYTES at the beginning of the buffer..*/
  buffSize += NUM_ID_BYTES;

  /* Allocate mem for the encrypted string */
  *encrypted_data = (char *) malloc((size_t)buffSize);
  if (encrypted_data == NULL)
    {
      syslog(LOG_WARNING, "ntcssEncryptMemory(): Failed to malloc enough"
             " memory for encrypted string!");
      return(_MALLOC_FAILED);
    }

  buff_ptr = (unsigned char*)*encrypted_data;   /* just an alias assignment*/

  /* create the padding bytes first */
  for (i = 0; i < pad_bytes; i++)
    buff_ptr[NUM_ID_BYTES + plain_data_size + i] = (unsigned char) i;

  /* store # pad bytes including the number byte.. */
  buff_ptr[buffSize - 1] = (unsigned char) (pad_bytes + 1);

  if (NUM_ID_BYTES > 0)
    /* Prepend the ID_BYTES to the final result.. */
    memcpy(buff_ptr, ID_BYTES, (size_t) NUM_ID_BYTES);

  make3key(encryption_key, d3key);
  des3key(d3key, EN0);

  processedBytes = NUM_ID_BYTES; /* Start encrypting AFTER the ID_BYTES */
  for (i = 0; (unsigned long)i < numDataBlocks; i++)
    {
      D2des((unsigned char*)&(plain_data[processedBytes]), 
            &(buff_ptr[processedBytes]));
      processedBytes += 16;
    }
  if (encrypted_data_size != NULL)
    *encrypted_data_size = buffSize;

  return (0);
}


/*****************************************************************************/
/* Takes the contents of the source_file and writes its unencrypted counterpart
   to the destination_file.
   Destination_file is created (or overwritten) with permissions 0600.
   Source_file and destination_file may be the same name which causes the
   source file to be overwritten with the result.
 */

int  ntcssDecryptFileToFile( const char  *source_file,
                             const char  *destination_file,
                             const char  *user_key )

{
  int  file_dscrptr;                /* File descriptor for file operations */
  unsigned char *plain_data;        /* Pointer to the decrypted data. */
  unsigned long  plain_data_size;   /* Size of the result. */
  unsigned long  i;                 /* Generic increment or temporary value */
  
  /* First we call this function to get a pointer to the decrypted text.. */
  i =  ntcssDecryptFileToMemory(source_file, user_key, &plain_data,
                                (int*)&plain_data_size);
  if ((int)i < 0)
    return(i);

  /* If there was nothing to decrypt, we just create an empty file. */
  if (i == _EMPTY_FILE)
    {
         /* Figure out which file to create.. */
      if (destination_file == NULL)
        source_file = destination_file;

      file_dscrptr = open(destination_file, O_WRONLY|O_CREAT|O_TRUNC, 0600);
      if  (file_dscrptr < 0)
        {
          syslog(LOG_WARNING,"ntcssDecryptFileToFile(): Could not open"
                 " file <%s> for writing empty file!", destination_file);
          return(_FILE_WRITE_FAILED);
        }
      else
        {
          close(file_dscrptr);
          return(0);
        }
    }

  if (destination_file == NULL) /* No separate destination file specified... */
    destination_file = source_file;   /* ... just overwrite the source file. */

  file_dscrptr = open(destination_file, O_WRONLY|O_CREAT|O_TRUNC, 0600);
  if  (file_dscrptr < 0)
    {
      free(plain_data);
      close(file_dscrptr);	  
      syslog(LOG_WARNING,"ntcssDecryptFileToFile(): Could not open file <%s>"
             " for writing!", destination_file );
      return(_FILE_OPEN_FAILED);
    }
  
  i = write(file_dscrptr, plain_data, plain_data_size);
  close(file_dscrptr);	  
  free(plain_data);

  if  (i != plain_data_size)
    {
      syslog(LOG_WARNING, "ntcssDecryptFileToFile(): Unsuccessfully wrote"
             " file <%s>!", destination_file);
      remove(destination_file);
      return (_FILE_WRITE_FAILED);
    } 

  return (0);
}


/*****************************************************************************/
/* Takes the contents of the source_file and writes its decrypted result to a
   malloc'd memory buffer.
 */

int  ntcssDecryptFileToMemory( const char      *source_file,  /* To decrypt. */
       /* Decryption key. */   const char      *user_key,
/* PTR set to result buffer.*/ unsigned char  **plain_data,
/* Size of result returned. */ int             *plain_data_size)

{
  int   file_dscrptr;               /* File descriptor for file operations */
  int   padding_bytes;              /* Excess bytes at end of data buffer */
  unsigned char *memBuff;           /* Buffer to hold file contents */
  unsigned long  buffSize;          /* Size of the memBuff array */
  unsigned long  bytes_read;         /* Bytes read from a file */
  int            i;                 /* Generic increment or temporary value */
  struct stat  statBuff;            /* For holding file information */

  if (plain_data == NULL)
    {
      syslog(LOG_WARNING, "ntcssDecryptFileToMemory(): Bad destination"
             " pointer!");
      return(_BAD_POINTER);
    }

  /* Get the file size.. */
  i = stat(source_file, &statBuff);
  if (i != 0)
    {
      syslog(LOG_WARNING, "ntcssDecryptFileToMemory(): Could not stat"
             " file <%s>!", source_file);
      return(_STAT_FAILED);
    }

  buffSize = (unsigned long) statBuff.st_size;

  /* If the file is empty, there's nothing to do. */
  if (buffSize == 0)
    return(_EMPTY_FILE);  /* non-negative return; caller should handle this. */

  /* Make sure the file is a valid encrypted file size.. */
  if ( ((buffSize % 16) - NUM_ID_BYTES) > 0)
    {
      syslog(LOG_WARNING, "ntcssDecryptFileToMemory(): File <%s> (size: %d)"
             " is not the right size!", source_file,
             statBuff.st_size);
      return(_NON_16_MULTIPLE);  
    }

  file_dscrptr = open(source_file, O_RDONLY, 0);
  if (file_dscrptr < 0)
    {
      syslog(LOG_WARNING, "ntcssDecryptFileToMemory(): Could not open file"
             " <%s>!", source_file);
      return(_FILE_OPEN_FAILED);
    }

  /* Allocate a buffer to read in the data to decrypt. */
  memBuff = (unsigned char *) malloc((size_t) buffSize);
  if (memBuff == NULL)
    {
      close(file_dscrptr);
      syslog(LOG_WARNING, "ntcssDecryptFileToMemory(): Could not malloc %d "
             "bytes of memory for file <%s>!", buffSize, source_file);
      return (_MALLOC_FAILED);
    }

  memset(memBuff, 0, (size_t) buffSize);               /* init the buffer */

  bytes_read = read(file_dscrptr, memBuff, statBuff.st_size);
  close(file_dscrptr);

  if (bytes_read != (unsigned long)statBuff.st_size)
    {
      free(memBuff);
      syslog(LOG_WARNING,"ntcssDecryptFileToMemory(): unsuccessfully"
             " read file <%s>!", source_file);
      return (_FILE_READ_FAILED);
     }

  /* Do the decrypting on the buffer. We neglect the shifted ID_BYTES. */
  i = ntcssDecryptMemory((char*)memBuff, buffSize, user_key, &padding_bytes);
  if (i < 0)
    {
      free(memBuff);
      syslog(LOG_WARNING,"ntcssDecryptFileToMemory(): Problems decrypting!"
             " Error (%i)", i);
      return (_DECRYPTION_ERROR);
    }

  /* Subtract the padding bytes from the total size of the original data.. */
  if(plain_data_size != NULL)
    *plain_data_size = bytes_read - padding_bytes;

  *plain_data = memBuff;        /* Set the pointer to our result */

  return (0);
}


/*****************************************************************************/
/* This is the core decryption routine for all the APIs in this file.  It
   expects a pointer to memory (crypt_data) containing the data to be
   decrypted.  The num_pad_bytes pointer can optionally be null if this data
   is not wanted.

   crypt_data;  Buffer containing encrypted data.
   crypt_data_size;  Size of crypt_data buffer.
   user_key;    Decryption key.
   excess_bytes;  The data len minus pad bytes.
 */

int  ntcssDecryptMemory( char          *crypt_data,
                         unsigned long  crypt_data_size,
                         const char    *user_key,
                         int           *excess_bytes)

{
  unsigned char  d3key[24];       /* Triple-length DES key */
  unsigned long  i;               /* Generic increment or temporary value */
  unsigned long  numDataBlocks;   /* # of 16-byte blocks for decryption */
  unsigned long  processedBytes;  /* # of bytes that have been decrypted */
  int   pad_bytes;                /* # of pad bytes @ end of last block */

  /* Some parameter checking.. */
  if(crypt_data == NULL)
    return(_BAD_POINTER);

  if ( ((crypt_data_size % 16) - NUM_ID_BYTES) > 0)
    return(_NON_16_MULTIPLE);  /* Must be an even multiple of 16 bytes! */

  /* Verify that the ID_BYTES in the file match what we expect..*/
  if (NUM_ID_BYTES > 0)
    if (memcmp(crypt_data, ID_BYTES, (size_t) NUM_ID_BYTES) != 0)
      return(_NON_ENCRYPTED_FILE);

  make3key(user_key, d3key); /* Hashes the key into a DES key. */
  des3key(d3key, DE1);       /* Initializes DES with the hashed key. */

  processedBytes = NUM_ID_BYTES; /* Start decrypting AFTER the ID_BYTES. */
  numDataBlocks = ( (crypt_data_size - NUM_ID_BYTES) / 16);
  for (i = 0; i < numDataBlocks; i++)
    {
      D2des((unsigned char*)&crypt_data[processedBytes], 
            (unsigned char*)&crypt_data[processedBytes]);
      processedBytes += 16;
    }

  /* Figure out how much padding there was in the original data.. */
  pad_bytes = crypt_data[crypt_data_size - 1];

  if (NUM_ID_BYTES > 0)
    {
      /* Shift the whole buffer, overwriting the ID_BYTES.. */
      for (i = NUM_ID_BYTES; i < crypt_data_size; i++)
        crypt_data[i - NUM_ID_BYTES] = crypt_data[i];
    }

    /* Null out all the padding bytes (includes the pad byte number now) */
    /* Pad bytes should ALWAYS be at least 1. */
  memset( &(crypt_data[ crypt_data_size - (pad_bytes + NUM_ID_BYTES) ]),
          0, (pad_bytes + NUM_ID_BYTES));

  if (excess_bytes != NULL)
    *excess_bytes = pad_bytes + NUM_ID_BYTES;

   return(0);
}

/*****************************************************************************/
/* This function reads a decrypted file and prints the output to the stdout.
   Since this function could potentially
*/

int  ntcssDecryptFileToStdout( const char *source_file,
                               const char *user_key )

{
  int   i;
  char *plain_data;

  i = ntcssDecryptFileToMemory(source_file, user_key, 
                               (unsigned char**)&plain_data, NULL);

  /* We only check fo negative return values here.. positive values are only
     considered to be FYI and not critical errors. */
  if (i < 0)
    return(i);

  if (i != _EMPTY_FILE) {
    printf("%s", plain_data);
    free(plain_data);
  }

  return(0);
}

