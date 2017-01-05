/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import COMMON.PropertiesLoader;

import ntcsss.libs.UserLib;

/**
 * A command line program for logging off a user from the local NTCSS server
 */
public class ForceLogoff
{
   public static void main(String arguments[])
   {
      try {
         
         // Check the number of arguments
         if (arguments.length != 2) {
            print("USAGE: force_logoff <username|ALLUSERS>");
            System.exit(1);
         }
         
         // Load the properties file
         if (PropertiesLoader.load(arguments[0]) == false) {
            print("Error loading properties file");
            System.exit(1);
         } 
         
         if (UserLib.forceLogoff(arguments[1]) == false) {
            print("Error logging off user(s)");
            System.exit(1);
         }
      }
      catch (Exception exception) {
         print("Exception occurred <" + exception.toString() + ">");
      }
   }
   
   private static void print(String str)
   {
      System.out.println(str);
   }
}
