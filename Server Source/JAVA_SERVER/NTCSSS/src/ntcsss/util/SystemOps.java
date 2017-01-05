/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.util;

import ntcsss.libs.Constants;

import ntcsss.log.Log;

/**
 * An interface for specific system dependent operations
 */
public class SystemOps
{
   static {  
      
      // Load the native library
      try {      
         System.load(Constants.NATIVE_LIBRARY);
      }
      catch (Exception exception) {
         Log.excp("SystemOps", "static initializer", exception.toString());
      }
   } 
   
   /**
    * Returns a string specifying the length of time the system platform 
    *  has been up
    */
   public static native String getUpTime();
   
   /**
    * Returns the name of the user who is currently logged in to the system
    */
   public static native String getCurrentSystemUser();   
   
   /**
    * Returns the value of the given system environment variable
    */
   public static native String getSystemEnvironmentVariable(String variable);
}
