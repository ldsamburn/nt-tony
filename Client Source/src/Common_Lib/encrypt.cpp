/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "encrypt.h"

/* This class will en/decrypt text strings.  The encrypted output
   characters will be restricted to the customizable output character
   array listed below.  The output from the en/decrypt functions
   require the caller to free the memory allocated for the returned result. 
   
   Output scrambling and compression can be turned on ( reccomended if
   strings longer than 200 characters are being used ) or off through
   the parameter lists. Make sure that these options match for both
   encryption and decryption!
*/


 CEncrypt::CEncrypt()
 {
 }

/***************** plainCryptInit ********************************/
/* Creates a type of seed number from the key. */
/* Requires a NULL terminted key string. */
int  CEncrypt::plainCryptInit(char *key)

{
 
  int  sum = 0; 
  int  maxSum;

	
  maxSum = INT_MAX - 127; /* limit in case of a REALLY large key sum */
  while ((*key) && ( sum < maxSum ))
    {
      sum += (int)*key;
      maxSum++;
      sum = cdiv((sum % NUM_OUTPUT_CHARS), 2);
      key++;
    }

  return(sum);
}


//************** plainEncryptString ********************************

/* This algorithm requires that the character map array contains unique
   characters numbering AT LEAST (the square root of(number all possible
   input characters)) rounded to the next highest whole number.  Input
   must be NULL terminated.  If length of data coming in is N, then
   the output size will be 2N -1 and NULL terminated. The user is
   responsible for calling free() on the output string when finished with it.
*/

CString CEncrypt::plainEncryptString(CString szKey, CString szData,
                        int use_compression/*=0*/, int use_scrambling/*=1*/)

  
     
{
  char *current; /* temporary pointer into the data string */
  char substChars[NUM_OUTPUT_CHARS];
  int  delta;  /* offset calculated from the key */
  int  inDataLen;
  int  lastChar; /* ASCII integer value from last place result */
  int  ndx = 0;
  char *outString;
  int  outDataLen;
  int  wrapAround = 0; /*# times modulus wraps around the substChars array*/
  CString strRet=_T("");
  LPTSTR outStringPtr=NULL;

 
  LPTSTR data=new char[strlen(szData)+1];
  strcpy(data,szData);

  LPTSTR key=new char[strlen(szKey)+1];
  strcpy(key,szKey);
  
  
  inDataLen = (int)strlen(data); /* original string length */
  if (inDataLen < 1) /* empty data */
     goto end;
 
  if (use_compression == 1)
    strCompress(data);  

  inDataLen = (int)strlen(data); /* new string length */

  if (use_scrambling == 1)
    strScramble(data);

  delta = plainCryptInit(key);
  createSubstChars(substChars);
  
  outDataLen = inDataLen * 2;
  outStringPtr =   new char [outDataLen + 1];
  outString = outStringPtr;
  if (outString == NULL)
 	goto end;
  
  current = data;
  ndx = 0;
  lastChar = 0;
  while (*current != NULL)
    {
      /* do the substitution */
      
      outString[ndx] = substChars[((lastChar + ((int)*current) + delta)
% NUM_OUTPUT_CHARS)];
      lastChar = (int)outString[ndx];
      
      ndx++; 
      /* record information about the previous substitution */
      
      /* the following assumes "*current" is never ASCII character zero*/
      wrapAround = cdiv(((unsigned int)*current - 1), NUM_OUTPUT_CHARS);
      
      outString[ndx] =  substChars[(wrapAround + cdiv(NUM_OUTPUT_CHARS, 7)
    + delta + lastChar) % NUM_OUTPUT_CHARS]; 
      
      lastChar = (int)outString[ndx];
      ndx++;
      current++;
    }

  outString[ndx] = '\0';    /* Should be last character. */

  if (use_scrambling == 1)
    strScramble(outString); 

	strRet=outStringPtr;

end:

  if(data)
	delete [] data;

  if(key)
	delete [] key;

  if(outStringPtr)
	  delete [] outStringPtr;

  return strRet;
}


/************ plainDecryptString ********************************/
/* caller responsible to free() output pointer */

CString  CEncrypt::plainDecryptString(CString strKey, CString strData,
                        int use_compression/*=0*/, int use_scrambling/*=1*/)


     
{


  char *decString;                     /* Decrypted result string. */
  char  substChars[NUM_OUTPUT_CHARS];  /* Allowed chars in encrypted
result. */
  int  delta;                          /* Offset calculated from the key. */
  int  i;
  int  inDataLen;
  int  modDelta;              /* Used for intermediary modulus
calculations. */
  int  ndx = 0;
  int  offSet = 0;            /* Used for temporary calculations. */
  int  outDataLen;
  int  placeCharNdx;
  int  previousChar;          /* ASCII value of previous character in
array. */
  int  temp;        /* Just to record position of second (information)
byte. */
  int  wrapArounds;         /* # Times modulus wraps around substChar
array. */

  CString strReturn=_T("");

  LPTSTR outStringPtr=NULL;

  LPTSTR data=new char[strlen(strData)+1];
  strcpy(data,strData);

  LPTSTR key=new char[strlen(strKey)+1];
  strcpy(key,strKey);

  inDataLen = (int)strlen(data);
  if (inDataLen < 1)            /* Empty data */
    return strReturn;
  
  if ((inDataLen % 2) != 0)     /* Should ALWAYS be a multiple of 2. */
    return strReturn;

  if (stringIsPrintable(key) && stringIsPrintable(data))
      return strReturn;

  if (use_scrambling == 1)
    strUnscramble(data);

  delta = plainCryptInit(key);
  
  createSubstChars(substChars);  /* Create the substitution mapping array. */
  
  outDataLen = cdiv(inDataLen, 2);
  decString = new char[outDataLen +1];

  if (decString == NULL)
    return(-1);

  ndx = outDataLen - 1;
  
  for (i = (inDataLen - 1); i > -1; i = i - 2)
    {
      /* Extract the information byte.. */
      temp = findStringIndex(&(data[i]), substChars, NUM_OUTPUT_CHARS);
      previousChar = (int)data[i - 1];   /* Should be at least one
previous. */
      offSet = cdiv(NUM_OUTPUT_CHARS , 7) + delta + previousChar;
      modDelta = temp - offSet;

      if(modDelta < 0)
{ 
  temp = modDelta % NUM_OUTPUT_CHARS;
  if(temp < 0)
    wrapArounds = NUM_OUTPUT_CHARS + temp;
  else
    wrapArounds = 0;             /* Temp should always be zero here. */
}
      else
wrapArounds = modDelta;
      
      /* Do the unsubstitution.. */
      
      temp = findStringIndex(&(data[i-1]), substChars, NUM_OUTPUT_CHARS);
      
      if (i < 2)         /* Last pair of bytes to be converted. */
previousChar = (unsigned int)0;
      else
previousChar = (unsigned int) data[i-2];   /* Must be at least one more. */
      
      modDelta = temp - (delta + previousChar);
      
      if(modDelta < 0)
{ 
  temp = modDelta % NUM_OUTPUT_CHARS;
  if(temp < 0)
    placeCharNdx = NUM_OUTPUT_CHARS + temp;
  else
    placeCharNdx = 0;                /* Temp should always be zero here. */
}
      else
placeCharNdx = modDelta;
      
      decString[ndx] = (unsigned char)(placeCharNdx 
       + (wrapArounds * NUM_OUTPUT_CHARS) );
      ndx--;
    }
  decString[outDataLen] = '\0';           /* Terminate string. */
  if (use_scrambling == 1)
    strUnscramble (decString);

  if (use_compression == 1)
     strUncompress(decString, outStringPtr);

  strReturn=decString;

  
  if(decString)
	delete [] decString;

  if(outStringPtr)
	delete [] outStringPtr;

  if(data)
	delete [] data;

  if(key)
	delete [] key;

  return strReturn;
}


/***************** findStringIndex ********************************/

int  CEncrypt::findStringIndex(char* character, char* string, int strlength)

         
{

  int  i;
  
  for (i = 0; i < strlength; i++)
    if (string[i] == *character)
      return(i);

  return (-1);      /* Does not exist in string! */
}


/***************** stringIsPrintable ********************************/

int  CEncrypt::stringIsPrintable(char* test_string)

{
  char  *current;

  current = test_string;
  while (*current) 
    {
      if (!isprint(*current))
		return(-1);          /* Non-printable character appears! */
      current++;
    }

  return (0);
}


/***************** cdiv ********************************************/
/* this function just returns the int quotient value of the div */

int CEncrypt::cdiv(int dividend, int devisor)

{
  div_t  result;
  
  result = div(dividend, devisor);
  return(result.quot);
}


/***************** createSubstChars ********************************/
/* Create the substitution mapping array -all ASCII printables but the ":" in
   this case. */
/* This array contains the list all possible characters that are allowed in
   the encrypted result. */

int CEncrypt::createSubstChars(char* ptr)

{
  /*make sure you put in exactly NUM_OUTPUT_CHARS characters into this
array */
  /* AND that they are ALL UNIQUE!! (ie NO repeated characters) */
  
ptr[0]  = 'X'; ptr[1]  = '1'; ptr[2]  = '^'; ptr[3]  = 'q'; ptr[4] = 'v';
ptr[5]  = ','; ptr[6]  = 'Q'; ptr[7]  = 'w'; ptr[8]  = 'h'; ptr[9] = '!';
ptr[10] = 'l'; ptr[11] = 'a'; ptr[12] = 'F'; ptr[13] = '2'; ptr[14] = 'S';
ptr[15] = '|'; ptr[16] = '%'; ptr[17] = 'Z'; ptr[18] = '}'; ptr[19] = 'W';
ptr[20] = '>'; ptr[21] = 'A'; ptr[22] = 'e'; ptr[23] = ')'; ptr[24] = 'c';
ptr[25] = '3'; ptr[26] = 'M'; ptr[27] = 'r'; ptr[28] = 'P'; ptr[29] = 'L';
ptr[30] = '.'; ptr[31] = '4'; ptr[32] ='\''; ptr[33] = 't'; ptr[34] = '5';
ptr[35] = '&'; ptr[36] = '_'; ptr[37] = 'm'; ptr[38] = '<'; ptr[39] = 'R';
ptr[40] = '?'; ptr[41] = 'G'; ptr[42] = 'y'; ptr[43] ='\"'; ptr[44] = 'j';
ptr[45] = '@'; ptr[46] = 'O'; ptr[47] = 'b'; ptr[48] = '$'; ptr[49] = 'H';
ptr[50] = '+'; ptr[51] = 'd'; ptr[52] = '6'; ptr[53] = 'u'; ptr[54] = 'N';
ptr[55] = '#'; ptr[56] = 'g'; ptr[57] = 'E'; ptr[58] = '7'; ptr[59] = 'i';
ptr[60] = ';'; ptr[61] = '8'; ptr[62] = 'x'; ptr[63] = 'f'; ptr[64] = '(';
ptr[65] = 'D'; ptr[66] = 'U'; ptr[67] = 'n'; ptr[68] = '{'; ptr[69] = '/';
ptr[70] = 'o'; ptr[71] = '9'; ptr[72] = 'J'; ptr[73] = '*'; ptr[74] = 'V';
ptr[75] = 'p'; ptr[76] = '0'; ptr[77] = 'I'; ptr[78] = '['; ptr[79] = 's';
ptr[80] = 'T'; ptr[81] = 'K'; ptr[82] = '-'; ptr[83] = 'z'; ptr[84] = 'C';
ptr[85] = '~'; ptr[86] = 'B'; ptr[87] = '='; ptr[88] = 'k'; ptr[89] = 'Y';
ptr[90] =' ';ptr[91] =']';
 

/* string should be
X1^qv,Qwh!laF2S|%Z}W>Ae)c3MrPL.4't5&_m<R?Gy"j@Ob$H+d6uN#g
E7i;8xf(DUn{/o9J*Vp0I[sTK-zC~B=kY\]   ***/
return 0;
}


/************************* strcompress ***********************************/
/* this algorithm stores a non-printing ASCII character in the output
   representing the number of repetitions a character appears in
   sequence at a certain position.  This information is stored as two
   bytes, the first being the character that is repeated, and the
   next byte contains the number ( between minReps and maxReps) that
   the character repeats.  The repeating characters are truncated from
   the list and the remaining ones shifted into the empty space created.
   The resulting string will be NULL-terminated */

int CEncrypt::strCompress(char* in)


{
  char *pattern;
  char *current;
  char *previous;
  int  j;
  int  maxReps = (FIRST_PRINTABLE_ASCII -1);
  int  minReps = 4;
  int  totalLen;

  if(minReps >= NUM_OUTPUT_CHARS)
    return(2);               /* OK but useless to compress. */

  totalLen = strlen(in);

  if(totalLen < minReps)
    return(3);               /* OK but useless to compress. */

  previous = &(in[0]);       /* Always set to first character. */
  current = &(in[1]);
  while(( (*previous) != NULL) && ( (*current) != NULL))
    {
      if (*current == *previous)     /* At least one repeat, check for more. */
{
  pattern = current;
  j = 0;
  while((*current == *previous) && (j <= maxReps))
    {
      j++;
      current++;
    }
  if(j > minReps)
    {
      *pattern = (char) j;          /* Store repeat value as ASCII code. */
      pattern++;
      strShift(pattern, (j - 1));   /* Truncate the repeats */
      current = pattern;            /* First uncompared char remaining. */
    }
}
      previous = current;
      current++;
    }

  return(0);
}


/************************* strUncompress ***********************************/
/* (Also see strCompress) This function takes a NULL-terminated string
   modified by strCompress and restores the repeating characters to the
   string.  The string will expand through use of malloc */

int  CEncrypt::strUncompress(char* inString, char* outString)

{
  char *current;
  char *out;    /* Just for clarity. */
  int  extraBytes = 0;
  int  i;
  int  index;
  int  infoBytes = 0;
  int  outStringSize;
  int  repetitions;
  
  current = inString;
  while((*current) != NULL)
    {
      if( ((int)(*current)) < FIRST_PRINTABLE_ASCII )
        {
          extraBytes += (int)(*current);
          infoBytes++;
        }
      current++;
    }

  outStringSize = extraBytes - infoBytes + (int)strlen(inString) + 1;
  outString = new char[outStringSize];

  out = outString;

  if(out == NULL)
    return(-1);        /* Cannot malloc! */

  current = inString;
  index = 0;
  while(*current != NULL)
    {
      out[index] = *current;
      if((int)(*current) < FIRST_PRINTABLE_ASCII)    /* Expansion needed.. */
        {
          repetitions = (int)(*current);   /* Should be at least minReps. */
          for(i = 0; i < repetitions; i++)
            out[index + i] = *(current - 1);
          index += (i - 1);         /* Should = repetitions -1 */
        }
      index ++;
      current++;
    }
  out[index] = '\0';
  return(0);
}


/************************ strShift **********************************/
/* removes shiftNum of characters from a string by left-shifting
   the remainder of the string*/

void CEncrypt::strShift(char *ptr, int shiftNum)

{
  int  i = 0;
  
  for(i = 0; (ptr[i] != NULL); i++)
    ptr[i] = ptr[i + shiftNum];
}

/************************ strScramble *******************************/
/* Scrambles a character string in a predefined way.

    Req: MUST BE GREATER THEN 6 CHARS
    Preconditions: receives data-string pointer 
    Postconditions: outputs the scambled input
*/

int CEncrypt::strScramble(char* data)

{


  int left = 0;    /* index */
  int right = 0;   /* index */
  int index = 0;   /* for loop */
  int length = 0;  /* length of incoming string */
  char templeft;  /* temp storage for left */
  char tempright; /* temp storage for right */


  length = (int) strlen(data);
  if ((length > 0) && (length <= 6)) 
    return(1);  /** will not scramble less than or equal to 6 **/

  /** to scramble **/
  /** left moves with index; right moves with index of [length-index]/2] **/
  /** starting from 0 to length-1 **/

  for (index=0; index < length-1; index++) 
    {
      left = (cdiv((length-index),2));
      right = length-index-1;
      templeft = data[left];
      tempright= data[right];
      data[left] = tempright;
      data[right] = templeft; 
    }
  return(0);
}


/************************ strUnscramble *******************************/
/** Procedure ScrambleOff:
    Req: MUST BE GREATER THEN 6 CHARS (same as scrambleOn)
    Preconditions: receives pointer to scrambled string;
    Postconditions: outputs the unscambled input **/

int  CEncrypt::strUnscramble (char *data)

  

{

  int  index = 0;   /** for loop **/
  int  left = 0;    /** index **/
  int  length = 0;  /** length of incoming string (data) **/
  int  right = 0;   /** index **/
  char templeft;   /** temp storage for left **/
  char tempright;  /** temp storage for right **/


  length = (int) strlen(data);
  if ((length > 0) && (length <= 6)) 
      return(1);  /** will not scramble less than or equal to 6 **/

  /** left moves with index; right moves with index of [length-index]/2] **/
  /** starting from length-1 to 0 (reverse order as scramble) **/

  for (index = length - 1; index >= 0 ; index--) 
    {
      left = (cdiv((length-index), 2));
      right = length - index - 1;
      templeft = data[left];
      tempright= data[right];
      data[left] = tempright;
      data[right] = templeft; 
    }
  return(0);
}

BOOL CEncrypt::Decrypt(LPCTSTR strKey,LPCTSTR strData,LPTSTR strResult)
{
	int use_compression=0;
	int use_scrambling=1;
	CString strValue=plainDecryptString(strKey,strData);
	return (_tcscpy(strResult,(LPCTSTR)strValue) !=NULL);

}

BOOL CEncrypt::Encrypt(LPCTSTR strKey,LPCTSTR strData,LPTSTR strResult)
{
	int use_compression=0;
	int use_scrambling=1;
	CString strValue=plainEncryptString(strKey,strData);
	return (_tcscpy(strResult,(LPCTSTR)strValue) !=NULL);

}