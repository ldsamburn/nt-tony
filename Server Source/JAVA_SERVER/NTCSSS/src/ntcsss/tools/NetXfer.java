/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import COMMON.FTP;
import COMMON.PropertiesLoader;

/**
 * A command line interface to the NTCSS FTP library
 */
public class NetXfer
{
   public static void main(String arguments[])
   {
      String usage = "Usage: net_xfer put|get hostname src_file dest_file";
      boolean successful = true;
      boolean isText = false;
      boolean append = false;
      
      try {
         
         // Check the number of arguments
         if (arguments.length != 5) {
            print(usage);
            System.exit(1);
         }
         
         // Load the properties file
         if (PropertiesLoader.load(arguments[0]) == false) {
            print("Error loading properties file");
            System.exit(1);
         } 
         
         // Perform the requested transfer
         if (arguments[1].equalsIgnoreCase("get") == true)
            successful =  FTP.get(arguments[2], arguments[3],
                             arguments[4], isText);
         else if (arguments[1].equalsIgnoreCase("put") == true)
            successful =  FTP.put(arguments[2], arguments[4],
                             arguments[3], isText, append);
         else {
            print(usage);
            System.exit(1);
         }
         
         // Check to see if the transfer was successful or not
         if (successful == false) {
            print("The transfer was unsuccessful");
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
