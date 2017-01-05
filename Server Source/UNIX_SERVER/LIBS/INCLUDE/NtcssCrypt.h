
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*** NtcssCrypt.h
 Binary file encryption and decryption using multiblock, single/double/triple
 length keys (Refer to d3des.h for more information)
***/

/******** CRITICAL ERRORS **********/

#define _FILE_OPEN_FAILED          -1
#define _MALLOC_FAILED             -2
#define _FILE_READ_FAILED          -3
#define _FILE_WRITE_FAILED         -4
#define _NON_16_MULTIPLE           -5
#define _NO_PLAIN_DATA             -6
#define _BAD_POINTER               -7
#define _DECRYPTION_ERROR          -8
#define _STAT_FAILED               -9

/******** Non-critical errors ********/
#define _EMPTY_FILE                 1
#define _EMPTY_STRING               2
#define _NON_ENCRYPTED_FILE         3

/****** CUSTOM FILE IDS  *************/
/* These two defines allow the specified ID_BYTES byte string to be
   prepended to every encrypted file.  If this feature is not wanted, 
   NUM_ID_BYTES can be set to zero.  NUM_ID_BYTES is the number of
   the ID_BYTES that will be prepended.  A file encrypted with ID_BYTES
   must be decrypted with the exact same ID_BYTES.  After decryption, these
   bytes are removed fomr the result.
*/
#define  NUM_ID_BYTES  4     
#define  ID_BYTES  "\x09\x0E\x12\x09"


/***** FUNCTION PROTOTYPES ************/

int  ntcssEncryptFileToMemory(const char*, unsigned char**, unsigned long*,
                              const char*);
int  ntcssEncryptFileToFile(const char*, const char*, const char*);
int  ntcssEncryptMemory(char*, unsigned long, char**, unsigned long*,
                         const char*);
int  ntcssDecryptFileToFile(const char*, const char*, const char*);
int  ntcssDecryptFileToMemory(const char*, const char*, unsigned char**, int*);
int  ntcssDecryptMemory(char*, unsigned long, const char*, int*);
int  ntcssDecryptFileToStdout(const char*, const char*);


/*****************************************************************************/
/*FUNCTIONAL DESCRIPTIONS


int  ntcssEncryptFileToFile(char *source_file,
                            char *dest_filename,
                            const char *user_key)

The function uses double DES to ecrypt a file.

 "sourceFile" should point to a null terminated string containing
 the legal path (including the file name) of the file to be encrypted.

 "destinationFile" should either be NULL or
 point to a null terminated string containing the legal path
 (including the file name) of the file to which the encrypted data
 is to be written.  If this parameter is NULL then the
 destination file automatically becomes the source file, overwriting
 the source file with the output.  If "destinationFile" does not
 exist, this function will create it.

"userKey" should point to null-terminated string of characters
 to be used as a key by which the file is scrambled.  The key
 string can be of any length, but longer lengths may not necessarily
 provide more security.  This is because the keystring is hashed
 into a standard 16-byte DES double-length key.  In addition to this,
 every 8'th bit is discarded (counted as a parity bit).

The return value should be 0 if everything went ok, non zero if not.
 The #defined codes in the header file list each error condition.
 Appropriate messages are also written to the system log in the
 event of an error.

If sourceFile is an empty file, destinationFile will be created as an
 empty file.

*/

/************************************
int  ntcssDecryptFileToFile(char *source_file,
                            char *destination_file,
                            const char *user_key)

This function unencrypts a file encrypted by ntcssEncryptFile().
 The entire file (contained in "sourceFile") is loaded into
 memory, decrypted, and written out to a file.

 "sourceFile" should point to a null terminated string containing
 the legal path (including the target filename) to the file to
 be decrypted.

 "destinationFile" should either be NULL or
 point to a null terminated string containing the legal path
 (including the file name) of the file to which the decrypted data
 is to be written.  If this parameter is NULL then the
 destination file automatically becomes the source file, overwriting
 the source file with the output.  If "destinationFile" does not
 exist, this function will create it.

 userKey must point to the same null-terminated string of characters used
 as the key that the file was originally encrypted with.

The return value should be 0 if everything went ok, non zero if not.
 The #defined codes in the header file list each error condition.
 Appropriate messages are also written to the system log in the
 event of an error.

*/

/************************************
int  ntcssEncryptMemory(char           *plain_data,
                        unsigned long   plain_data_size,
                        char           **encrypted_data,
                        unsigned long  *encrypted_data_size,
                        char           *encryption_key)

This function takes a block of characters from memory and encrypts it to
 malloc'd memory.  Nulls can be part of the block, but are not required for
 encryption.
 The function returns zero if no errors occured during the operation, and a
 negative integer if one did occur.
The encrypted_data paramaeter should be passed in as the address of the pointer
 to the encrypted result so that it can be set.
The plain_data_size integer represents the true length of the plain_data
 string ,including the terminating NULL if desired.
 This is so that a string with mulitple NULLs in it can be passed in and
 encrypted as a whole instead of its pieces.
The encrypted_data_size pointer may be passed in as NULL.  If this pointer is 
 non-NULL, then the function assumes that space has been allocated for
 the integer, and sets the integer to the number of bytes of valid data
 returned.  This parameter may just be a local variable and passed
 in as &encrypted_data_size.
Appropriate messages are also written to the system log in the
 event of an error.
 NOTE****  The calling function MUST free() the memory pointed to by the 
 encrypted_data pointer.
*/


/************************************
int  ntcssDecryptMemory(char          *crypt_data,
                        unsigned long  crypt_data_size,
                        char          *user_key,
                        int           *num_pad_bytes);

This function takes a buffer in memory and decrypts it.
 The function returns zero if no errors occured during the operation, and a
 negative integer if one did occur.
After the operation, the decrypted data has its padding bytes NULLed out. 
The num_pad_bytes is an optional input.  If the address of an int is passed
 in, the integer is set to represent how many null bytes follow
 the original data.
Appropriate messages are also written to the system log in the
 event of an error.

*/


/************************************
int  ntcssDecryptFileToMemory( char            *source_file,
                               char            *user_key,
                               unsigned char  **plain_data,
                               int             *plain_data_size);

This function unencrypts contents of a file (encrypted by ntcssEncryptFile())
 placing the result in a malloc'd memory buffer and pointing the plain_data
 input parameter to it.

"sourceFile" should point to a null terminated string containing
 the legal path (including the target filename) to the file to
 be decrypted.

The plain_data_size parameter is set to indicate the exact size
 of the original unencrypted data, including the ending NULL if it was counted
 when encrypting the orginal data.  This parameter is optional and may be
 passed in as NULL.

There will always be at least one extra NULL byte following
 the plain_data_size "index".  This is a leftover
 from the pad bytes number included in every encrypted file and serves as a
 terminator if the data was a character string.
Appropriate messages are also written to the system log in the
 event of an error, as well as negative return values.

 NOTE****  The calling function MUST free() the memory pointed to by
 plain_data unless an error occured.

 NOTE****  If the source file is empty, a positive integer is returned and the 
  plain_data pointer will remain NULL ~ trying to free this would cause
  an error!

*/

/************************************

int  ntcssEncryptFileToMemory(char            *source_file;
                              unsigned char  **result_buff;
                              unsigned long   *result_size;
                              char            *user_key;

This function creates a buffer to read the contnts of the source_file into
 and then encrypts the buffer.  The input parameter result_buff is set to point
 at this buffer.

If the optional result_size pointer parameter is not NULL, it is set to
 the total size of the result_buff.  It should always be a multiple of 16
 after subtracting any ID_BYTES specified from the sum.

***NOTE The calling function is rsponsible to call free() on the result_buff
  parameter when it is finished using it, except for when the return value is
  _EMPTY_FILE.

If sourceFile is an empty file, The value of result_buff will be NULL and
 result_size will be 0, and the function's return value will be _EMPTY_FILE.

*/

/************************************

int  ntcssDecryptFileToStdout(char  *source_file,
                              char  *user_key);

This function prints the unencrypted contents of source_file to the STDOUT.
The data is delimited by comments specifying the beginning and end of the data.








*****/
