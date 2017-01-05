/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  Takes the supplied properties file, combines it with the
//              System properties and any properties specified on the command
//              line, and then loads all of them into the environment.


package COMMON;

import java.util.*;
import java.io.*;

public class PropertiesLoader
{
   // Loads the properties into the environment and returns success or failure
   public static boolean load(String propertiesFilename)
   {
      Properties props;
      boolean ret;

      try {
         // Creates a new Properties object with the system properties as its
         //  parent
         props = new Properties(System.getProperties());

         // Load the specified file of properties into it
         props.load(new BufferedInputStream(new FileInputStream(propertiesFilename)));

         // Set these new combined properties as the system properties
         System.setProperties(props);

         ret = true;
      }
      catch (Exception e) {
         ret = false;
      }

      return ret;
   } 

   // Dumps the current system properties
   public static boolean dumpProperties()
   {
      Properties properties;
      Enumeration keys;
      String key;

      try {
         // Get the current system properties
         properties = System.getProperties();
         keys = properties.propertyNames();

         while (keys.hasMoreElements() == true) {
            key = (String)keys.nextElement();

            System.out.println(key + " = " + System.getProperty(key));
         }
      }
      catch (Exception exception) {
         return false;
      }
 
      return true;
   }
}
