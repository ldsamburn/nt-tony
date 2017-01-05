/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.util;

/**
 * Similar to java.lang.StringBuffer but designed specifically for integer
 *  values
 */
public class IntegerBuffer
{
   private int integerValue;
   
   public void setValue(int newValue)
   {
      integerValue = newValue;
   }   
   
   public int getValue()
   {
      return integerValue;
   }
}
