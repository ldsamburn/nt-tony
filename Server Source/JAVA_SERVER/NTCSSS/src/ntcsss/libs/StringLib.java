/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.text.SimpleDateFormat;
import java.text.NumberFormat;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import java.util.Date;
import java.util.Random;
import java.util.Arrays;
import java.util.StringTokenizer;

import java.io.File;


import java.net.InetAddress;

import ntcsss.log.Log;

/**
 * A library containing methods for manipulating strings
 */
public class StringLib
{
   /**
    * The days in each month
    */
   public static final int month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30,
                                            31, 30, 31 };
   public static final String months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul","Aug", "Sep", "Oct", "Nov", "Dec" };
                                
   /**
    * Returns the current time in the given buffer
    */
   public static String getFullTimeStr()
   {
      SimpleDateFormat dateFormatter;      
      
      dateFormatter = new SimpleDateFormat("yyMMdd.HHmmss");
      
      return dateFormatter.format(new Date());
   }
   
   /**
    * Returns the current time in the given buffer
    */
   public static String getTimeStr()
   {
      SimpleDateFormat dateFormatter;      
      
      dateFormatter = new SimpleDateFormat("yyMMdd.HHmm");
      
      return dateFormatter.format(new Date());
   }
    
    /**
     * Same as Integer.parseInt except this method takes a default value
     *  incase there is an error
     */
    public static int parseInt(String stringValue, int defaultInteger)
    {
       try {
          return Integer.parseInt(stringValue);
       }
       catch (Exception exception) {          
          return defaultInteger;
       }
    }
        
   /**
    * Returns the name of the local host
    */
   public static String getHostname()
   {
      try {
         InetAddress address = InetAddress.getLocalHost();
       
         return address.getHostName();
      }
      catch (Exception exception) {
         return null;
      }       
   }

   /**
    * Returns the IP address of the local host
    */
   public static String getHostAddress()
   {
      try {
         InetAddress address = InetAddress.getLocalHost();
         return address.getHostAddress();
      }
      catch (Exception exception) {
         return null;
      }
   }
   
   /**
    * See below
    */
   public static boolean  getNextFullTimeStr(String str, StringBuffer new_str, 
                                             int type, int num)
   {
      char tmpArray[];
      boolean ret;
      
      try {
         if ((str == null) || (new_str == null))
            return false;
         
         tmpArray = new char[100];
         
         ret = getNextFullTimeStr(str.toCharArray(), tmpArray, type, num);
         if (ret == true) {
            new_str.delete(0, new_str.capacity());
            new_str.insert(0, tmpArray);
         }         
                                             
         return ret;                                            
      }
      catch (Exception exception) {
         Log.excp("StringLib", "getNextFullTimeStr", exception.toString());
         return false;
      }
   }

    
   /**
    * Increments (or decrements) the given time string ("str") by "num" base on the type.
    *  The result is returned in "new_str". If there is an error, "false"
    *  is returned. Otherwise, "true" is returned
    */
   public static boolean  getNextFullTimeStr(char str[], char new_str[], 
                                             int type, int num)

   {       
      int    year;
      int    month;
      int    day;
      int    extra;      
      long   tme;
      long   amt;
      String tmp = new String(str);   
      String tmpStr;
      String yearStr, monthStr, dateStr;

      if (!isValidFullTimeStr(str)) {       
         Log.error("StringLib", "getNextFullTimeStr", 
                   "Corrupt time string: <" + str + ">");
         return false;
      }

      amt = (long)num;
      switch (type) {
      
         case Constants.YEAR_TYPE: 
            year = Integer.parseInt(tmp.substring(0,2));
            month = Integer.parseInt(tmp.substring(2,4));
            day = Integer.parseInt(tmp.substring(4,6));
         
            year += num;
         
            while (day == 29 && month == 2 && (year % 4) != 0)
               year += 1;
         
            while (year >= 100) 
               year -= 100;                     
            
            if (year < 10)
               yearStr = "0" + String.valueOf(year);
            else
               yearStr = String.valueOf(year);
         
            if (month < 10)
               monthStr = "0" + String.valueOf(month);
            else
               monthStr = String.valueOf(month);
         
            if (day < 10)
               dateStr = "0" + String.valueOf(day);
            else
               dateStr = String.valueOf(day);
            
            tmpStr = yearStr + monthStr + dateStr;                     
            System.arraycopy(tmpStr.toCharArray(), 0, new_str, 0, tmpStr.length());
            System.arraycopy(str, 6, new_str, 6, tmp.length() - 6);
            
            return true;         
      
         case Constants.MONTH_TYPE:
            year = Integer.parseInt(tmp.substring(0,2));
            month = Integer.parseInt(tmp.substring(2,4));
            day = Integer.parseInt(tmp.substring(4,6));
            
            month += num;
            
            while (month >= 13) {
               month -= 12;
               year += 1;
            }
            
            extra = 0;
            if (month == 2 && (year % 4) == 0)
               extra = 1;
            
            while ((month_days[month - 1] + extra) < day)
               month += 1;
            
            while (year >= 100)
               year -= 100;                                                                              
         
            if (year < 10)
               yearStr = "0" + String.valueOf(year);
            else
               yearStr = String.valueOf(year);
         
            if (month < 10)
               monthStr = "0" + String.valueOf(month);
            else
               monthStr = String.valueOf(month);
         
            if (day < 10)
               dateStr = "0" + String.valueOf(day);
            else
               dateStr = String.valueOf(day);
            
            tmpStr = yearStr + monthStr + dateStr;
            System.arraycopy(tmpStr.toCharArray(), 0, new_str, 0, tmpStr.length());
            System.arraycopy(str, 6, new_str, 6, tmp.length() - 6);
            
            return true;      

         case Constants.WEEK_TYPE:
            amt *= (long)7;
         case Constants.DAY_TYPE:
            amt *= (long)24;
         case Constants.HOUR_TYPE:
            amt *= (long)60;
         case Constants.MINUTE_TYPE:
            amt *= (long)60;
            break;

         default:
            return false;      
      }
    
      // Get the number of seconds since Jan 1, 1970 and the given time string
      tme = FullTimeStrToSecs(str);
      if (tme < (long)0) {    
         Log.error("StringLib", "getNextFullTimeStr", 
                   "Could not convert string <" + tmp + "> to seconds!");             
         return false;
      }
      
      // Add these seconds to the increment amount
      tme += amt;
      
      // Create a new time string from the given number of seconds since
      //  Jan 1, 1970. The result is stored in "new_str".
      if (SecsToFullTimeStr(tme, new_str) == false) {    
         Log.error("StringLib", "getNextFullTimeStr", 
                   "Unable to create a time string from seconds!");
         return false;
      }

      return true;
   }
    
   /**
    * Makes sure the given time string is in the correct format. If it isn't,
    *  "false" is returned. Otherwise, "true" is returned.
    */
   public static boolean isValidFullTimeStr(char str[])

   {               
      int   t;      
      String tmpStr = (new String(str)).trim();        
      
      // Make sure it is the correct length
      if (tmpStr.length() != Constants.FULL_TIME_STR_LEN) {
    
         Log.warn("StringLib", "isValidFullTimeStr", "Input <"  + tmpStr + 
                   "> is not the required " + Constants.FULL_TIME_STR_LEN + 
                   " characters!");
         return false;
      }

      // Make sure the . is in the correct place
      if (str[6] != '.') {
         Log.warn("StringLib", "isValidFullTimeStr", "Input <" + tmpStr +
                  "> delimeter not in correct position!");             
         return false;
      }

      // Make sure all the characters (except the .) are numbers
      for (int i = 0; i < Constants.FULL_TIME_STR_LEN; i++) {
                  
         if (Character.isDigit(str[i]) == false) {
        
            if (i == 6) /* is just the period delimeter. */
               continue;
            
            Log.warn("StringLib", "isValidFullTimeStr", "Input <" + tmpStr +                     
                     "> has a non-numeric character in position " + i + "!");
            return false;
         }
      }

      // Make sure the year is in the correct range
      t = Integer.parseInt(tmpStr.substring(0, 2));
      if (t < 00 || t > 99) {
    
         Log.warn("StringLib", "isValidFullTimeStr", 
                  "Input <" + tmpStr + "> year <" + t + "> is out of range!");                  
         return false;
      }

      // Make sure the month is in the correct range
      t = Integer.parseInt(tmpStr.substring(2, 4));
      if (t < 01 || t > 12) {    
         Log.warn("StringLib", "isValidFullTimeStr", 
                  "Input <" + tmpStr + "> month <" + t + "> is out of range!");             
         return false;
      }

      // Make sure the day is in the correct range
      t = Integer.parseInt(tmpStr.substring(4, 6));
      if (t < 01 || t > 31) {    
          Log.warn("StringLib", "isValidFullTimeStr", 
                   "Input <" + tmpStr + "> day <" + t + "> is out of range!");                
          return false;
      }

      // Make sure the hour is in the correct range
      t = Integer.parseInt(tmpStr.substring(7, 9));
      if (t < 00 || t > 23) {    
         Log.warn("StringLib", "isValidFullTimeStr", 
                  "Input <" + tmpStr + "> hour <" + t + "> is out of range!");                
         return false;
      }

      // Make sure the minute is in the correct range
      t = Integer.parseInt(tmpStr.substring(9, 11));
      if (t < 00 || t > 59) {    
         Log.warn("StringLib", "isValidFullTimeStr", 
                  "Input <" + tmpStr + "> minute <" + t + "> is out of range!");             
         return false;
      }

      // Make sure the seconds is in the correct range
      t = Integer.parseInt(tmpStr.substring(11, 13));
      if (t < 00 || t > 59) {             
         Log.warn("StringLib", "isValidFullTimeStr",
                  "Input <" + tmpStr + "> second <" + t + "> is out of range!");             
         return false;
      } 

      return true;
   }
   
   /**
    * Determines the number of seconds between January 1, 1970 and the
    *  given time string (YYMMDD.HHMMSS)
    */
   public static long FullTimeStrToSecs(char str[])
   {
      int year, month, date, hour, min, sec;
      int iyr, imn, idt;
      long tme = (long)0;
      String tmpStr = new String(str);

      if (!isValidFullTimeStr(str))
         return (long)-1;

      year = Integer.parseInt(tmpStr.substring(0, 2));      
      if (year >= 0 && year <= 59)
         year += 100;
      
      month = Integer.parseInt(tmpStr.substring(2, 4));
      date = Integer.parseInt(tmpStr.substring(4, 6));
      hour = Integer.parseInt(tmpStr.substring(7, 9));
      min = Integer.parseInt(tmpStr.substring(9, 11));
      sec = Integer.parseInt(tmpStr.substring(11, 13));      

      for (iyr = 70; iyr <= year; iyr++) {
         
         if (iyr == year) {
            
            for (imn = 1; imn <= month; imn++) {
               if (imn == month) {
                  for (idt = 1; idt <= date; idt++) {
                     if (idt == date)
                        tme += ((long) hour)*(long)3600 + 
                               ((long) min)*(long)60 +
                               ((long) sec);
                      else
                         tme += (long)86400;
                  }
               } 
               else {
                  tme += ((long) month_days[imn - 1])*(long)86400;
                  if (((iyr % 4) == 0) && (imn == 2))
                     tme += (long)86400;
               }
            }
         } 
         else {
            if ((iyr % 4) == 0)
               tme += (long)31622400;
            else
               tme += (long)31536000;
         }
      }

      return(tme);
   }
   
   /**
    * Takes the number of seconds since January 1, 1970 stored in "tme", 
    *  and constructs a time string to represent that date and stores
    *  the result in "str". If an error occurs, "false" is returned. Otherwise,
    *  "true" is returned.
    */
   public static boolean  SecsToFullTimeStr( long  tme, char str[] )
   {
      int i, j, year, month, date, hour, min, sec;
      long nsecs = (long)0;
      long amt = (long)0;
      String tmpStr;
      String yearStr, monthStr, dateStr, hourStr, minStr, secStr;
      
      month = date = 1;
      year = hour = min = sec = 0;
      
      try {
   
         // Make sure the given number of seconds isn't negative
         if (tme < (long)0)
            return false;
                           
         // Determine the year
         for (i = 70; i <= 159; i++)
         {
            year = i;
            
            if (tme == nsecs)
               throw new GoTo();
            
            if ((year % 4) == 0)
               amt = (long)31622400;
            else
               amt = (long)31536000;
            
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
         
         // Determine the month
         for (i = 1; i <= 12; i++)
         {
            month = i;
            
            if (tme == nsecs)
               throw new GoTo();
            
            amt = ((long) month_days[month - 1])*(long)86400;
            if (((year % 4) == 0) && (month == 2))
               amt += (long)86400;
            
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
                  
         j = month_days[month - 1];
         if (((year % 4) == 0) && (month == 2))
            j++;
         
         // Determine the day of the month
         for (i = 1; i <= j; i++)
         {
            date = i;
            
            if (tme == nsecs)
               throw new GoTo();
            
            amt = (long)86400;
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
         
         // Determine the number of hours
         for (i = 0; i < 24; i++)
         {
            hour = i;
            
            if (tme == nsecs)
               throw new GoTo();
               
            amt = (long)3600;
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
         
         // Determine the number of minutes
         for (i = 0; i < 60; i++)
         {
            min = i;
            
            if (tme == nsecs)
               throw new GoTo();
            
            amt = (long)60;
            
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
         
         // Determine the number of seconds
         for (i = 0; i < 60; i++)
         {
            sec = i;
            
            if (tme == nsecs)
               throw new GoTo();
            
            amt = (long)1;
            
            if ((nsecs + amt) > tme)
               break;
            
            nsecs += amt;
         }
         
         // Should not get here
         return false;
      }         
      catch (GoTo goTo) {
         
         // Assemble the time string
         
         if (year > 99)
            year -= 100;
  
         if (year < 10)
            yearStr = "0" + String.valueOf(year);
         else
            yearStr = String.valueOf(year);
         
         if (month < 10)
            monthStr = "0" + String.valueOf(month);
         else
            monthStr = String.valueOf(month);
         
         if (date < 10)
            dateStr = "0" + String.valueOf(date);
         else
            dateStr = String.valueOf(date);
         
         if (hour < 10)
            hourStr = "0" + String.valueOf(hour);
         else
            hourStr = String.valueOf(hour);
         
         if (min < 10)
            minStr = "0" + String.valueOf(min);
         else
            minStr = String.valueOf(min);
         
         if (sec < 10)
            secStr = "0" + String.valueOf(sec);
         else
            secStr = String.valueOf(sec);
         
         tmpStr = yearStr + monthStr + dateStr + "." + 
                  hourStr + minStr + secStr;                  
         
         System.arraycopy(tmpStr.toCharArray(), 0, str, 0, tmpStr.length());                  
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("StringLib", "SecsToFullTimeStr", exception.toString());
         return false;
      }            
   }  
   
   /**
    * Returns a string representing the given integer which is padded on the
    *  left with zeros so that its minimum length is equal to "length". If
    *  the integer length is greater than "length", no padding is done.
    *
    *  e.g.  If integer = 12 and length = 5    result  00012
    *  e.g.  If integer = 123456 and length = 5    result  123456
    *  e.g.  If integer = -7 and length = 2   result = -7
    *  e.g.  If integer = -7 and length = 3  result = -07
    */
   public static String valueOf(int integer, int length)
   {
      NumberFormat format = NumberFormat.getInstance();
      
      // Turns off printing of "," characters in the number
      format.setGroupingUsed(false);
      
      // If the given number is negative, decrement the count by one to 
      //  account for the negative sign.
      if (integer < 0)
         length--;
      
      if (length > 0)
         format.setMinimumIntegerDigits(length);
      
      
      return format.format(integer);
   }
   
   /**
    * Provides a one-way encryption of the given string. The encrypted string
    *  is returned as an array of bytes
    */
   public static byte[] crypt(String string)
   {
      MessageDigest algorithm;
      
      try {
         algorithm = MessageDigest.getInstance("SHA-1");
         algorithm.update(string.getBytes());
      
         return algorithm.digest();
      }
      catch (Exception exception) {
         Log.excp("StringLib", "crypt", exception.toString());
         return null;
      }                  
   }
   
   /**
    * Stores in the given string buffer a random string. The string will be no
    *  longer than the given length. The "args" parameters are used in 
    *  developing the random string. The random string will be made up of
    *  only printable characters.
    */
   public static void  getRandomString(StringBuffer str, int max_leng, 
                                       String args[])
   {      
      String salt = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      char salt_val[] = new char[3];
      char key[] = new char[8];
      int i, j, leng, offset = 0;      
      String currentArg;
      Random randomNumber;
      String randomString;
      
      Arrays.fill(key, 'z');
      
      for (j = 0; j < args.length; j++) {
         if (args[j] == null)
            break;
         
         currentArg = args[j];
         
         leng = currentArg.length();
         if (leng > 8)
            leng = 8;
         
         for (i = 0; i < leng; i++) {
            offset = Math.abs(((int) currentArg.charAt(i) - (int)key[i])/2);
            if ((int) currentArg.charAt(i) < (int) key[i])
                key[i] = (char) ((int) currentArg.charAt(i) + offset);
            else
                key[i] = (char) ((int) key[i] + offset);
         }
      }
      
      randomNumber = new Random();      
      i = randomNumber.nextInt() % 62;
      j = randomNumber.nextInt() % 62;
      salt_val[0] = salt.charAt(Math.abs(i));
      salt_val[1] = salt.charAt(Math.abs(j));
      salt_val[2] = '\0';
     
      randomString = new String(crypt(new String(salt_val) + new String(key)));
      
      // Clear the given buffer
      str.delete(0, str.capacity());      
      
      if (randomString.length() >= max_leng)         
         str.insert(0, randomString.substring(0, max_leng));            
      else         
         str.insert(0, randomString);    
      
      // Replace any non-printable characters with printable characters
      for (int k = 0; k < str.length(); k++) {         
         if ((str.charAt(k) < 32) || (str.charAt(k) > 126))
            str.setCharAt(k, (char)(randomNumber.nextInt(95) + 32));
      }
   }
   
   /**
    * Returns a time string that includes milliseconds
    */
   public static String getFullTimeStrWithMiliSec()
   {
      SimpleDateFormat dateFormatter;      
      
      dateFormatter = new SimpleDateFormat("yyMMdd.HHmmss:SSS");
      
      return dateFormatter.format(new Date());
   }
   
   /**
    * Takes the given string and escapes any suspect characters so that the
    *  result can be used in a SQL statement. The new string is returned.
    */
   public static String escapeCharacters(String string) 
   {
      StringBuffer newString = new StringBuffer();
      
      if (string != null) {
         for (int i = 0; i < string.length(); i++) {
            if ((string.charAt(i) == '\\') ||
                (string.charAt(i) == '\''))
               newString.append('\\');
            
            newString.append(string.charAt(i));
         }
      }
      
      return newString.toString();
   }
   
   /**
    * Takes a given SQL statment that contains the special delimiter and places 
    *  the given arguments in where it occurs. 
    */
   public static String SQLformat(String SQLstatement, Object args[])
   {      
      StringBuffer newStatement;
      Object argument;
      int currentPosition;
      int delimiterIndex;
      int argumentNumber;

      try {
         
         // Make sure null arguments weren't given
         if ((SQLstatement == null) ||
             (args == null)) {
            Log.error("StringLib", "SQLformat", "null parameter given");
            return SQLstatement;
         }
                  
         // Create a buffer into which will be placed the formated string
         newStatement = new StringBuffer();

         currentPosition = 0;
         argumentNumber = 0;
         
         // Find the next occurrence of the delimiter
         while ((currentPosition < SQLstatement.length()) && 
                (delimiterIndex = SQLstatement.indexOf(Constants.ST, 
                                                      currentPosition)) != -1) {

            // Append all the data up to this point to the new string
            newStatement.append(SQLstatement.substring(currentPosition, 
                                                       delimiterIndex));

            // Make sure there is an argument 
            if (argumentNumber >= args.length) {
               Log.error("StringLib", "SQLformat", "Not enough arguments supplied");
               return SQLstatement;
            }

            // Insert the next argument
            argument = args[argumentNumber++];               
            if (argument != null) {
                  
               // If the argument is a String, "escape" any special characters
               if (argument instanceof String) 
                  argument = "'" + escapeCharacters(argument.toString()) + "'";                  
               else if (argument instanceof Boolean)                  
                  argument = ((Boolean)argument).booleanValue() ? "true":"false";
           
               newStatement.append(argument.toString());
            }
            else 
               newStatement.append("null");

            // Move the cursor past the delimiter
            currentPosition = delimiterIndex + Constants.ST.length();
         }

         // Append the rest of the string to the buffer
         if (currentPosition < SQLstatement.length())
            newStatement.append(SQLstatement.substring(currentPosition));
         
         // Return the new string
         return newStatement.toString();
      }
      catch (Exception exception) {
         Log.excp("StringLib", "SQLformat", exception.toString());
         return SQLstatement;
      }
   }   
   
   /**
    * Adds padding to the end of the given string so that the total length is
    *  equal to the given length. The stricter. If the length of the string is
    *  greater than or equal to the given length parameter, then no padding
    *  is added to the end of the string. The resulting string is returned.
    */
   public static String padString(String string, int length, char padCharacter)
   {
      char padding[];
      
      try {
         
         // Check to see if the given string even needs padding
         if ((string == null) || (string.length() >= length))
            return string;
         
         // Create a padding string
         padding = new char[length - string.length()];
         Arrays.fill(padding, padCharacter);
         
         return string + new String(padding);
      }
      catch (Exception exception) {
         Log.excp("StringLib", "padString", exception.toString());
         
         return string;
      }
   }
   
   /**
    * Returns true if the given string is 0 length or contains only spaces
    */
   public static boolean isBlank(String str) 
   {
      if ((str == null) || (str.length() == 0))
         return true;
      
      for (int i = 0; i < str.length(); i++)
         if (str.charAt(i) != ' ')
            return false;
      
      return true;
   }
   
   /**
    * Verifies the given string follows the following pattern YYMMDD.HHMM
    */
   public static boolean isValidTimeStr(String str)
   {
      int val;
      
      if ((str == null) || (str.length() != Constants.TIME_STR_LEN))
        return false;

      // Make sure only digits are given
      for (int i = 0; i < Constants.TIME_STR_LEN; i++) {
         if (i == 6) {
            if (str.charAt(i) != '.')
               return false;
         } else {
            if (Character.isDigit(str.charAt(i)) == false)
               return false;
         }
      }
      
      // Make sure part falls with in its specified range
      val = parseInt(str.substring(0, 2), -1);   // Year
      if (val < 00 || val > 99)
         return false;
      
      val = parseInt(str.substring(2, 4), -1);   // Month
      if (val < 01 || val > 12)
         return false;
      
      val = parseInt(str.substring(4, 6), -1);   // Date   
      if (val < 01 || val > 31)
         return false;
      
      val = parseInt(str.substring(7, 9), -1);   // Hour   
      if (val < 00 || val > 23)
         return false;
      
      val = parseInt(str.substring(9, 11), -1);  // Minute  
      if (val < 00 || val > 59)
         return false;

      return true;
   }
   
   /**
    * Pads the given header out to CMD_LEN and appends the given message
    *  to the end of it
    */
   public static String formatMessage(String header, String message)
   {      
      char paddingCharacter = '\0';
      
      try {
         
         // Make sure the heading is not greater than the max size allowed
         if (header.length() > Constants.CMD_LEN) {
            Log.error("StringLib", "formatMessage", "Header <" +
                      header + "> is too long");
            return null;            
         }
                  
         // Return the formatted string
         return (padString(header, Constants.CMD_LEN, paddingCharacter)  + message);
      }
      catch (Exception exception) {
         Log.excp("StringLib", "formatMessage", exception.toString());                            
      }
      
      return null;
   }
   
   /**
    * Replaces any punctuation characters in the given string with 
    *  alphanumeric characters. The given string buffer is modified.    
    */
   public static void removePunctuation(StringBuffer string)   
   {
      char currentChar;
      Random randomIntGenerator;
      String replacements = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      
      if (string == null)
         return;

      // Create a random number generator
      randomIntGenerator = new Random();
      
      // Loop through each of the characters in the given string
      for (int i = 0; i < string.length(); i++) {

         // See if the current character in the string is a punctuation character
         currentChar = string.charAt(i);
         if (((currentChar > 32) && (currentChar < 48)) ||
             ((currentChar > 57) && (currentChar < 65)) ||
             ((currentChar > 90) && (currentChar < 97)) ||
             ((currentChar > 122) && (currentChar < 127))) {
                
            // Replace the character with an alphanumeric character
            string.setCharAt(i, 
               replacements.charAt(randomIntGenerator.nextInt(replacements.length())));
         } 
      }
   } 
   
   public static String timeStrToReadable(String str)                        
   {      
      int month, day, year;

      if (!isValidTimeStr(str))
         return null;

      year = Integer.parseInt(str.substring(0, 2));
      month = Integer.parseInt(str.substring(2, 4)) - 1;
      day = Integer.parseInt(str.substring(4, 6));      
      
      return valueOf(day, 2) + " " + 
             months[month] + " " +
             valueOf(year, 2) + " " +  
             str.substring(7, 9) + ":" + str.substring(9, 11);
   }
}

class GoTo extends Throwable
{
   public GoTo()
   {
      super();
   }
}