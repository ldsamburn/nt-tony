/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import ntcsss.log.Log;

/**
 * This program/functions will en/decrypt text strings.  The encrypted output
 *  characters will be restricted to the customizable output character
 *  array listed below. 
 *
 * Output scrambling and compression can be turned on ( reccomended if
 *  strings longer than 200 characters are being used ) or off through
 *  the parameter lists. Make sure that these options match for both
 *  encryption and decryption!
 */
public class CryptLib
{
   private static final int NUM_OUTPUT_CHARS; // # of ASCII characters to be used as outputs 
   private static final char substChars[];
   private static final int FIRST_PRINTABLE_ASCII = 32; // ASCII value of first printable character 
   
   /**
    * Create the substitution mapping array -all ASCII printables but the ":" 
    *  in this case. 
    * This array contains the list all possible characters that are allowed in
    *  the encrypted result. 
    */
   static
   {
      // Make sure you put in exactly NUM_OUTPUT_CHARS characters into this 
      //  array AND that they are ALL UNIQUE!! (ie NO repeated characters) 
      // If you remove/add characters to this array, you MUST decrement/increment
      //  the NUM_OUTPUT_CHARS constant respectively! 
      
      // characters (in qoutes) currently omitted: ":", "\"  
      
      // string should be   X1^qv,Qwh!laF2S|%Z}W>Ae)c3MrPL.4't5&_m<R?Gy"j@Ob$H+
      //   d6uN#gE7i;8xf(DUn{/o9J*Vp0I[sTK-zC~B=kY ] 
      
      NUM_OUTPUT_CHARS = 92;
      substChars = new char[NUM_OUTPUT_CHARS];

      substChars[0]  = 'X'; substChars[1]  = '1'; substChars[2]  = '^'; 
      substChars[3]  = 'q'; substChars[4]  = 'v'; substChars[5]  = ','; 
      substChars[6]  = 'Q'; substChars[7]  = 'w'; substChars[8]  = 'h'; 
      substChars[9]  = '!'; substChars[10] = 'l'; substChars[11] = 'a'; 
      substChars[12] = 'F'; substChars[13] = '2'; substChars[14] = 'S';
      substChars[15] = '|'; substChars[16] = '%'; substChars[17] = 'Z'; 
      substChars[18] = '}'; substChars[19] = 'W'; substChars[20] = '>'; 
      substChars[21] = 'A'; substChars[22] = 'e'; substChars[23] = ')'; 
      substChars[24] = 'c'; substChars[25] = '3'; substChars[26] = 'M'; 
      substChars[27] = 'r'; substChars[28] = 'P'; substChars[29] = 'L';
      substChars[30] = '.'; substChars[31] = '4'; substChars[32] ='\''; 
      substChars[33] = 't'; substChars[34] = '5'; substChars[35] = '&'; 
      substChars[36] = '_'; substChars[37] = 'm'; substChars[38] = '<'; 
      substChars[39] = 'R'; substChars[40] = '?'; substChars[41] = 'G'; 
      substChars[42] = 'y'; substChars[43] ='\"'; substChars[44] = 'j';
      substChars[45] = '@'; substChars[46] = 'O'; substChars[47] = 'b'; 
      substChars[48] = '$'; substChars[49] = 'H'; substChars[50] = '+'; 
      substChars[51] = 'd'; substChars[52] = '6'; substChars[53] = 'u'; 
      substChars[54] = 'N'; substChars[55] = '#'; substChars[56] = 'g'; 
      substChars[57] = 'E'; substChars[58] = '7'; substChars[59] = 'i';
      substChars[60] = ';'; substChars[61] = '8'; substChars[62] = 'x'; 
      substChars[63] = 'f'; substChars[64] = '('; substChars[65] = 'D'; 
      substChars[66] = 'U'; substChars[67] = 'n'; substChars[68] = '{'; 
      substChars[69] = '/'; substChars[70] = 'o'; substChars[71] = '9'; 
      substChars[72] = 'J'; substChars[73] = '*'; substChars[74] = 'V';
      substChars[75] = 'p'; substChars[76] = '0'; substChars[77] = 'I'; 
      substChars[78] = '['; substChars[79] = 's'; substChars[80] = 'T'; 
      substChars[81] = 'K'; substChars[82] = '-'; substChars[83] = 'z'; 
      substChars[84] = 'C'; substChars[85] = '~'; substChars[86] = 'B'; 
      substChars[87] = '='; substChars[88] = 'k'; substChars[89] = 'Y';
      substChars[90] = ' '; substChars[91] = ']';       
   }
   
   /**
    * key             Key used to calculate an offset for encryption.
    * data            Data to be decrypted.
    * outStringPtr    Address of encrypted ouput pointer.
    */
   public static int plainDecryptString(String key, String data,
                                        StringBuffer outStringPtr,
                                        boolean use_compression, 
                                        boolean use_scrambling)
   {      
      return plainDecryptStringWithCharSet(key, data, outStringPtr, 
                                           use_compression, use_scrambling,
                                           substChars, NUM_OUTPUT_CHARS); 
   }     
   
   /**
    * key             Key used to calculate an offset for encryption.
    * data            Data to be decrypted.
    * outStringPtr    Address of encrypted ouput pointer.    
    */
   public static int plainDecryptStringWithCharSet(String key, String data,
                                                   StringBuffer outStringPtr,
                                                   boolean use_compression,
                                                   boolean use_scrambling,
                                                   char chars[],
                                                   int numOutputChars )
   {
      char decString[];   // Decrypted result string. 
      String strTemp;     // Temporary storage used for scrambling and encryption 
      int  delta;         // Offset calculated from the key. 
      int  i;
      int  inDataLen;
      int  modDelta;      // Used for intermediary modulus calculations. 
      int  ndx = 0;
      int  offSet = 0;    // Used for temporary calculations. 
      int  outDataLen;
      int  placeCharNdx;
      int  previousChar;  // ASCII value of previous character in array. 
      int  temp;          // Just to record position of second (information) byte. 
      int  wrapArounds;   // # Times modulus wraps around substChar array. 

      // Initialize the return buffer
      outStringPtr.delete(0, outStringPtr.length());
         
      inDataLen = data.length();
      if (inDataLen < 1)   // Empty data. 
         return 1;
      
      strTemp = new String(data);

      if ((inDataLen % 2) != 0) {    // Should ALWAYS be a multiple of 2.     
         Log.error("CryptLib", "plainDecryptStringWithCharSet", 
                   "Encrypted string is corrupt!");
         return -2;
      }
      if (testStringForPrintables(key) == false) {           
         Log.error("CryptLib", "plainDecryptStringWithCharSet", 
                   "Key contains invalid characters!");
         return -3;
      }
      if (testStringForPrintables(strTemp) == false) {    
         Log.error("CryptLib", "plainDecryptStringWithCharSet", 
                   "Encrypted string contains invalid characters!");
         return -3;
      }

      if (use_scrambling == true)    
         strTemp = strUnscramble(strTemp);              

      delta = plainCryptInit(key, numOutputChars);

      outDataLen = cdiv(inDataLen, 2);      
      ndx = outDataLen - 1;

      decString = new char[outDataLen];      
      for (i = (inDataLen - 1); i > -1; i = i - 2) {       

         // Extract the information byte.. 
         temp = findStringIndex(strTemp.charAt(i), chars, numOutputChars);
         previousChar = (int)strTemp.charAt(i - 1); // Should be at least one previous. 
         offSet = cdiv(numOutputChars , 7) + delta + previousChar;
         modDelta = temp - offSet;

         if(modDelta < 0) {        
            temp = modDelta % numOutputChars;
            if(temp < 0)            
               wrapArounds = numOutputChars + temp;            
            else            
               wrapArounds = 0;          // Temp should always be zero here.             
         }
         else        
            wrapArounds = modDelta;        

         // Do the unsubstitution.. 
         temp = findStringIndex(strTemp.charAt(i - 1), chars, numOutputChars);
         if (i < 2)                     // Last pair of bytes to be converted.         
            previousChar = 0;        
         else                                     // Must be at least one more.         
            previousChar = (int) strTemp.charAt(i - 2);        

         modDelta = temp - (delta + previousChar);

         if(modDelta < 0) {        
            temp = modDelta % numOutputChars;
            if(temp < 0)            
               placeCharNdx = numOutputChars + temp;            
            else                           // Temp should always be zero here.             
               placeCharNdx = 0;            
         }
         else        
            placeCharNdx = modDelta;       
        
         decString[ndx] = (char)(placeCharNdx
                                       + (wrapArounds * numOutputChars) );                                                
         
         ndx--;
      }  // END for             
           
      if (use_scrambling == true)    
         decString = (strUnscramble(new String(decString))).toCharArray();    

      if (use_compression == true)    
         strUncompress(new String(decString), outStringPtr);   
      else    
         outStringPtr.insert(0, decString);                    

      return 0;
   }
   
   /** 
    * This algorithm requires that the character map array contains unique
    *  characters numbering AT LEAST (the square root of(number all possible
    *  input characters)) rounded to the next highest whole number.  
    *  If length of data coming in is N, then the output size will be 2N -1. 
    *
    * key           Key used to calculate an offset for encryption.
    * data          Data to be encrypted.
    * outStringPtr  Encrypted ouput buffer.
    */
   public static int plainEncryptString(String key, String data,
                                        StringBuffer outStringPtr,
                                        boolean use_compression,
                                        boolean use_scrambling)
   {
      StringBuffer compressTmp = new StringBuffer();         
      int   current;          // Temporary pointer into the data string       
      String strTemp;          // Temporary storage used for scrambling and encryption       
      int  delta;             // Offset calculated from the key. 
      int  inDataLen;
      int  lastChar;          // ASCII integer value from last place result. 
      int  ndx = 0;      
      int  wrapAround = 0;    // # times modulus wraps around the substChars array. 
      char out[];
            
      if (data.length() < 1)       
         return 1;    
      
      strTemp = new String(data);
      
      if (use_compression == true) {
         strCompress(strTemp, compressTmp);
         strTemp = compressTmp.toString();
      }

      inDataLen = strTemp.length();   // New string length. 

      if (use_scrambling == true)    
         strTemp = strScramble(strTemp); 
      
      delta = plainCryptInit(key, NUM_OUTPUT_CHARS);            
      
      out = new char[inDataLen * 2];
 
      current = 0;
      ndx = 0;
      lastChar = 0;
      while (current < strTemp.length()) {    
         
         // Do the substitution.. 
         out[ndx] = substChars[(lastChar + (int)strTemp.charAt(current) + 
                                                delta) % NUM_OUTPUT_CHARS];
         lastChar = (int)out[ndx];

         ndx++;
         // Record information about the previous substitution.. 
         // The following assumes "*current" is never ASCII character zero. 
         wrapAround = cdiv(((int)strTemp.charAt(current) - 1), NUM_OUTPUT_CHARS);

         out[ndx] = substChars[(wrapAround + cdiv(NUM_OUTPUT_CHARS, 7)
                                       + delta + lastChar) % NUM_OUTPUT_CHARS];
         lastChar = (int)out[ndx];
         
         ndx++;
         current++;
      }   // END while       
      
      strTemp = new String(out);

      if (use_scrambling == true)             
         strTemp = strScramble(strTemp);
      
      outStringPtr.delete(0, outStringPtr.capacity());   
      outStringPtr.insert(0, strTemp);
          
      return(0);   
   }
   
   /**
    * If the last character in the given string is equal to ':', it is dropped
    *  off. Nothing is done if this isn't the case. The resulting string is 
    *  returned.
    */
   public static String trimEncryptedPasswordTerminator(String strEncryptedPassword)
   {  
      int    nLastCharIndex;

      // To avoid loosing spaces at the end of the encrypted password,
      //  we are ending the encrypted password with a ":".  We must now remove
      //  that end delimeter 
      nLastCharIndex  = strEncryptedPassword.length() - 1;
      if (strEncryptedPassword.charAt(nLastCharIndex) == ':')             
         return strEncryptedPassword.substring(0, nLastCharIndex);
      else {
         Log.info("CryptLib", "trimEncryptedPasswordTerminator", 
                  "Did not find expected terminator, NO TRIM PERFORMED!");    
         return strEncryptedPassword;
      }
   }

   /**
    * Returns false if any characters in the string are "non-printable". 
    *  Otherwise, true is returned.
    */
   private static boolean testStringForPrintables(String test_string)
   {
      for (int i = 0; i < test_string.length(); i++) {
         
         // Check to see if it character is non-printable         
         if ((test_string.charAt(i) < 32) || (test_string.charAt(i) > 126))
            return false;          
      }

      return true;
   }
   
   /** 
    * Req: MUST BE GREATER THEN 6 CHARS (same as scrambleOn)
    * Preconditions: receives pointer to scrambled string;
    * Postconditions: outputs the unscambled input 
    *
    * The unscrambled string is returned.
    */
   private static String strUnscramble(String data)
   {
      int  index = 0;   // for loop 
      int  left = 0;    // index 
      int  length = 0;  // length of incoming string (data) 
      int  right = 0;   // index 
      char templeft;    // temp storage for left 
      char tempright;   // temp storage for right 
      StringBuffer newData;

      length = data.length();
      
      // Make sure the string is longer than 6 characters
      if (length <= 6)
          return data;          // Will not scramble less than or equal to 6 

      // Left moves with index; right moves with index of [length-index]/2] 
      newData = new StringBuffer(data);
      for (index = length - 1; index >= 0 ; index--) {    
         left = cdiv((length-index), 2);
         right = length - index - 1;
         templeft = newData.charAt(left);
         tempright = newData.charAt(right);
         newData.setCharAt(left, tempright);
         newData.setCharAt(right, templeft);
      }
      
      return newData.toString();
   }
   
   /**
    * Returns the int quotient value of the div 
    */
   private static int cdiv(int dividend, int devisor)
   {      
      return dividend/devisor;
   }
   
   /**
    * Creates a type of seed number from the key.   
    */
   private static int plainCryptInit(String key, int numOutputChars )
   {
      int  sum = 0;
      int  maxSum;

      // Limit in case of a REALLY large key sum. 
      maxSum = Integer.MAX_VALUE - 127;      
      
      for (int i = 0; (i < key.length()) && (sum < maxSum); i++) {      
         sum += (int)key.charAt(i);
         maxSum++;
         sum = cdiv((sum % numOutputChars), 2);         
      }

      return sum;
   }
   
   /**
    * Returns the first occurrence of the given character in the given
    *  string array within the first "strlength" characters of the array.
    *  If the character is found, it's index is returned. Otherwise, a -1
    *  is returned.
    */
   private static int findStringIndex(char character, char string[],
                                      int strlength )
   {      
      int  i;

      for (i = 0; i < strlength; i++)
         if (string[i] == character)
            return i;

      return -1;      // Does not exist in string! 
   }
   
   /**
    * (Also see strCompress) This function takes a NULL-terminated string
    *  modified by strCompress and restores the repeating characters to the
    *  string.  The result is stored in the given string buffer.
    */
   private static int strUncompress(String inString, StringBuffer outString)                           
   {
      String current;      
      int  extraBytes = 0;
      int  i;
      int  index;
      int  infoBytes = 0;
      int  outStringSize;
      int  repetitions;

      current = new String(inString);
      for (int j = 0; j < current.length(); j++) {      
    
         if((int)current.charAt(j) < FIRST_PRINTABLE_ASCII ) {        
            extraBytes += (int)current.charAt(j);
            infoBytes++;
         }         
      }

      outStringSize = extraBytes - infoBytes + inString.length();
      outString.setLength(outStringSize);
      
      index = 0;
      for (int j = 0; j < current.length(); j++) {

         outString.setCharAt(index, current.charAt(j));   
         if((int)current.charAt(j) < FIRST_PRINTABLE_ASCII) { // Expansion needed..          
            repetitions = (int)current.charAt(j);  // Should be at least minReps. 
            for(i = 0; i < repetitions; i++)
               outString.setCharAt(index + i, current.charAt(j - 1));
            index += (i - 1);            // Should equal (repetitions - 1) 
         }
         index ++;      
      }      
      
      return 0;
   }      
   
   /** 
    * This algorithm stores a non-printing ASCII character in the output
    *  representing the number of repetitions a character appears in
    *  sequence at a certain position.  This information is stored as two
    *  bytes, the first being the character that is repeated, and the
    *  next byte contains the number ( between minReps and maxReps) that
    *  the character repeats.  The repeating characters are truncated from
    *  the list and the remaining ones shifted into the empty space created.    
    */   
   private static int strCompress(String in, StringBuffer out)
   {
      int pattern;
      int current;
      int previous;
      int  j;
      int  maxReps = (FIRST_PRINTABLE_ASCII -1);
      int  minReps = 4;
      int  totalLen;
      
      out.delete(0, out.capacity());      
      out.insert(0, in);       

      if(minReps >= NUM_OUTPUT_CHARS)
         return 2;   // OK but useless to compress. 

      totalLen = in.length();

      if(totalLen < minReps)
         return 3;   // OK but useless to compress. 

      previous = 0;  // Always set to first character. 
      current = 1;
      while ((previous < out.length()) && 
             (current < out.length())) {    
                              
         if (out.charAt(current) == out.charAt(previous)) {  // At least one repeat, check for more. 
            pattern = current;
            j = 0;
             
            while((previous < out.length()) && (current < out.length()) &&
                  (out.charAt(current) == out.charAt(previous)) && 
                  (j <= maxReps)) {            
               j++;
               current++;
            }                          
             
            if (j > minReps) {            
               out.setCharAt(pattern, (char)j);      // Store repeat value as ASCII code. 
               pattern++;
               out.delete(pattern, pattern + (j - 1));                
               current = pattern;           // First uncompared char remaining. 
            }
         }
          
         previous = current;
         current++;
      }

      return 0;
   }   
   
   /**
    * Scrambles a character string in a predefined way.
    *
    * Req: MUST BE GREATER THEN 6 CHARS
    * Preconditions: receives data-string pointer
    * Postconditions: outputs the scambled input
    *
    * The scrambled string is returned.
    */
   private static String strScramble(String data) 
   {
      int left = 0;    // index 
      int right = 0;   // index 
      int index = 0;   // for loop 
      int length = 0;  // length of incoming string 
      char templeft;   // temp storage for left 
      char tempright;  // temp storage for right 
      StringBuffer out;

      length = data.length();
      if ((length > 0) && (length <= 6))
         return data;  // Will not scramble less than or equal to 6 
  
      // left moves with index; right moves with index of [length-index]/2] 
      // starting from 0 to length-1 
      out = new StringBuffer(data);
      for (index=0; index < length-1; index++) {    
         left = (cdiv((length-index),2));
         right = length-index-1;
         templeft = out.charAt(left);
         tempright= out.charAt(right);
         out.setCharAt(left, tempright);
         out.setCharAt(right, templeft);
      }
      
      return out.toString();
   }
}
