/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.io.FileOutputStream;
import java.io.FileWriter;

import java.util.Arrays;
import java.util.Vector;

import ntcsss.log.Log;

/**
 * A library containing methods for bundling/unbundling data
 */
public class BundleLib
{
   /**
    * Bundles the given data on to the end of the given file and
    *  returns the number of bytes added.
    */
   public static int bundleQuery(String filename, String key, Object items[])
   {
      FileWriter writer;
      int fileLength = 0;      
      
      try {
         
         // Open an output stream to the file
         writer = new FileWriter(filename, true);
         
         fileLength = bundleQuery(writer, key, items);
         
         writer.close();
         
         return fileLength;
      }
      catch (Exception exception) {
         Log.excp("BundleLib", "bundleQuery", exception.toString());
         return 0;
      }
   } 
   
   /**
    * Bundles the given data on to the end of the given file stream and
    *  returns the number of bytes added.
    */
   public static int bundleQuery(FileWriter writer, String key, Object items[])
   {
      int fileLength = 0;
      String bundle;
      
      try {
         
         if (key.length() > items.length) {
            Log.error("BundleLib", "bundleQuery", "Key too long");
            return 0;           
         }
         
         // Loop through each of the data items and bundle them on to the
         //  end of the file
         for (int i = 0; i < key.length(); i++) {
            
            // Get the bundled format of the current data item
            bundle = createBundle(key.charAt(i), items[i]);
            
            // Append the formatted data to the end of the file
            writer.write(bundle);
         
            // Increase the length by the number of bytes just added
            fileLength += bundle.length();                         
         }
         
         return fileLength;
      }
      catch (Exception exception) {
         Log.excp("BundleLib", "bundleQuery", exception.toString());
         return 0;
      }
   }
   
   /**
    * Bundles the given data on to the end of the given string buffer and
    *  returns the number of bytes added.
    */
   public static int bundleData(StringBuffer str, String key, Object items[])                 
   {                     
      int leng = 0;       
      String bundle;
      
      if (key.length() > items.length) {
         Log.error("BundleLib", "bundleData", "Key too long");
         return 0;           
      }
                     
      // Loop through each of the data items and bundle them on to the
      //  end of the string buffer
      for (int i = 0; i < key.length(); i++) {   
         
         // Get the bundled format of the current data item
         bundle = createBundle(key.charAt(i), items[i]);
                    
         // Append the formatted data to the end of the string buffer
         str.append(bundle);
         
         // Increase the length by the number of bytes just added
         leng += bundle.length();                                                 
      }      

      return leng;
   }   
   
   /////////////////////////////////////////////////////////////////////////////
   /**
    * This method returns a bundled string suitable for a client
    *  reponse message format. 
    * items[0] should always be the usual bundle format key.
    *
    * The resulting string will have an extra "IC" bundle prepended to the
    * usual bundled data, where the "I" is the total size of the bundled message
    * (excluding the "I") and the "C" is the format key itself bundled in with
    * the rest of the data.
    */

   public static String smartBundle(Object  items[])
   {                     
      StringBuffer bundle_buff = new StringBuffer();
      bundleData(bundle_buff, "C" + items[0].toString(), items);

      // Bundle in the total length integer
      String bundle = createBundle('I', new Integer(bundle_buff.length()));
      return bundle + bundle_buff.toString();
   }


   /**
    * Parses through the given string storing the results in the given 
    *  "items" array
    */
   public static boolean unbundleData(String str, String key, Object items[])
   {
      int currentIndex = 0;
      int strLength;
      
      if (key.length() > items.length) {
         Log.error("BundleLib", "unbundleData", "Key too long");
         return false;           
      }
                     
      for (int i = 0; i < key.length(); i++) {   
                  
         switch (key.charAt(i)) {
            
            // Unbundle character string.
            case 'C':
               if ((currentIndex + 5) > str.length())
                  return false;
               
               strLength = Integer.parseInt(str.substring(currentIndex, currentIndex + 5));
               
               currentIndex += 5;
               if ((currentIndex + strLength) > str.length())
                  return false;
               
               if (currentIndex == currentIndex + strLength)
                  items[i] = null;
               else
                  items[i] = str.substring(currentIndex, currentIndex + strLength);
               
               currentIndex += strLength;
               break;
            
            // Unbundle integer string.
            case 'I':
               if ((currentIndex + 1) > str.length())
                  return false;
               
               strLength = Integer.parseInt(str.substring(currentIndex, currentIndex + 1));
               
               currentIndex += 1;
               if ((currentIndex + strLength) > str.length())
                  return false;
               
               if (currentIndex == currentIndex + strLength)
                  items[i] = null;
               else
                  items[i] = new Integer(str.substring(currentIndex, currentIndex + strLength));
               
               currentIndex += strLength;
               break;
               
            // Unbundle date/time string.
            case 'D':
               if ((currentIndex + 11) > str.length())
                  return false;
               
               items[i] = str.substring(currentIndex, currentIndex + 11);
               
               currentIndex += 11;
               break;            
         }         
      }      

      return true;
   }
   
   /**
    * Parses through the given string storing the results in the given 
    *  "items" vector
    */
   public static boolean unbundleData(String str, String key, Vector items)
   {
      Object array[];
      boolean successful;
                            
      try {
         
         // Check to make sure a vector object was passed in
         if (items == null) {
            Log.error("BundleLib", "unbundleData", "null structure given");
            return false;           
         }
         
         // Create a temporary array
         array = new Object[key.length()];
         
         // Call the unbundle method with the temporary array
         if ((successful = unbundleData(str, key, array)) == true) {
         
            // Copy the elements from the temporary array to the vector
            for (int i = 0; i < array.length; i++)
               items.add(array[i]);
         }    
         
         return successful;
      }
      catch (Exception exception) {
         Log.excp("BundleLib", "unbundleData", exception.toString());
         return false;
      }            
   }
   
   /////////////////////////////////////////////////////////////////////////////
   /**
    * This is the mate to smartBundle() and parses the bundled_string created
    * by smartBundle() into a Vector class.
    * True is returned if all went well, false if not.
    */
   public static boolean smartUnbundle(String bundled_string, Vector items)

   {
     // We first call unbundle to get at the key string in the second position
     if (!unbundleData(bundled_string, "IC", items))
       return false;

     // unbundle the entire array (with the now-redundant preceding "IC")
     if (!unbundleData(bundled_string, "IC" + items.get(1).toString(), items))
        return false;

     items.removeElementAt(0);  // get rid of the msg string size integer.
     items.removeElementAt(0);  // get rid of the key string.
     return true;
   }

   
   /////////////////////////////////////////////////////////////////////////////
   /**
    * Returns the given data in its bundled format based on the given type
    */
   private static String createBundle(char type, Object data)
   {
      String charData;       
      Integer intData;      
      int newlineIndex;
       
      switch (type) {
             
         // Bundle character string.
         case 'C':
            if (data != null) {
               charData = (String)data;                                      
               return StringLib.valueOf(charData.length(), 5) + charData;                                 
            }
            else 
               return "00000";                                                          

         // Bundle character string without newlines.
         case 'N':
            if (data != null) {
               charData = (String)data;
                  
               newlineIndex = charData.lastIndexOf('\n');                  
               if (newlineIndex != -1)
                  charData = charData.substring(0, newlineIndex);
                   
               return createBundle('C', charData);                                 
            }
            else 
               return "00000";                                                     

         // Bundle integer string.
         case 'I':
            if (data != null) {
               intData = (Integer)data;
               charData = intData.toString();                  
               return charData.length() + charData;               
            }
            else 
               return "0";                                                             

         // Bundle date/time string.
         case 'D':
            if (data != null) {
               charData = (String)data;
               return charData.substring(0,11);               
            }
            else 
               return "00000000000";                           
      }   
      
      return null;
   }
}
