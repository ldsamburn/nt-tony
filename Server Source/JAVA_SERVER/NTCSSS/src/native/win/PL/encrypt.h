/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/** CONSTANT DEFINITIONS **/

#define NUM_OUTPUT_CHARS	92 /* # of ASCII characters to be used as
outputs */
#define NUM_ALL_CHARS	94       /* # of all possible input ASCII
chars */
#define FIRST_PRINTABLE_ASCII 32 /* ASCII value of first printable
character */


class CEncrypt
{

public:

CEncrypt();

CString plainDecryptString(CString strKey, CString strData, 
                        int use_compression=0, int use_scrambling=1);

CString plainEncryptString(CString szKey, CString szData,
                        int use_compression=0, int use_scrambling=1);

BOOL Decrypt(LPCTSTR strKey,LPCTSTR strData,LPTSTR strResult);
BOOL Encrypt(LPCTSTR strKey,LPCTSTR strData,LPTSTR strResult);

protected:

void strShift(char *ptr, int shiftNum);

int  cdiv(int dividend, int devisor);
int  createSubstChars(char* ptr);
int  findStringIndex(char* character, char* string, int strlength);
int  plainCryptInit(char* key);

int  stringIsPrintable(char* test_string);
int  strCompress(char* in);
int  strScramble (char* data);
int  strUncompress(char* inString, char* outString);
int  strUnscramble (char* data);

};