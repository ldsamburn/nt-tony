/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.util;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.File;

import java.util.StringTokenizer;

import ntcsss.log.Log;

/**
 * Basic file operations
 */
public class FileOps
{    
   /**
    * Copies the contents of "srcFile" into "destFile". If no errors occur,
    *  true is returned.
    */
   public static boolean copy(String srcFile, String destFile)
   {
      BufferedOutputStream destWriter;
      BufferedInputStream srcReader;
      byte buffer[];
      int bytesRead;
      File fileDescp;
      long modificationTime;
      
      try {
         
         // Make sure the source file exists
         fileDescp = new File(srcFile);
         if (fileDescp.exists() == false)
            return false;     
         
         // Create the reader and writer
         destWriter = new BufferedOutputStream(new FileOutputStream(destFile, false));
         srcReader  = new BufferedInputStream(new FileInputStream(srcFile));
         
         // Copy all the data from the one file to the other
         buffer = new byte[1000];         
         do {
            bytesRead = srcReader.read(buffer, 0, buffer.length);
            if (bytesRead != -1)
               destWriter.write(buffer, 0, bytesRead);
         } while (bytesRead != -1);         
         
         // Close the reader and writer
         srcReader.close();
         destWriter.close();
         
         // Get the modification time of the source file
         fileDescp = new File(srcFile);
         modificationTime = fileDescp.lastModified();
         
         // Set the modification time of the destination file to be 
         //  the same time
         fileDescp = new File(destFile);
         fileDescp.setLastModified(modificationTime);                  
         
         return true;
      }
      catch (Exception exception) {     
         return false;
      }
   }
   
   /**
    * Recursively deletes everything under the given directory and deletes that
    *  directory itself. If this operation is successful, true is returned.
    *
    * USE WITH EXTREME CAUTION!!  This method will delete WHATEVER directory
    *  it is given INCLUDING ROOT PARTITIONS AND DRIVES!!
    */
   public static boolean recursivelyDeleteDir(String directory)
   {
      String fnct_name = "recursivelyDeleteDir";
      File dirDescp;     
      File files[];      
      
      try {
         
         // Make sure the given directory exists and is in fact a directory
         dirDescp = new File(directory);
         if ((dirDescp.exists() == false) || (dirDescp.isDirectory() == false)) {
            Log.error("FileOps", fnct_name, "Invalid directory given");
            return false;
         }
         
         // Delete all the files and directories under this directory
         files = dirDescp.listFiles();
         for (int i = 0; i < files.length; i++) {
            
            // If this entry is a file, delete it
            if (files[i].isFile() == true)
               files[i].delete();
            
            // Otherwise, this entry must be a directory, so recurively delete
            //  its files and subdirectories
            else if (recursivelyDeleteDir(files[i].getAbsolutePath()) == false)
               return false;
         }
         
         // Now delete this empty directory
         dirDescp.delete();
         
         return true;
      }
      catch (Exception exception) {     
         Log.excp("FileOps", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Returns true if the two given files are different
    */
   public static boolean diff(String file1, String file2, boolean binaryCompare)
   {
      String fnct_name = "diff";
      File fileDescp1, fileDescp2;
      BufferedInputStream stream1, stream2;
      int input1, input2;
      boolean different = false;
      BufferedReader reader1, reader2;
      String line1, line2;
      
      try {
         
         fileDescp1 = new File(file1);
         fileDescp2 = new File(file2);
         
         // See if the files exist
         if ((fileDescp1.exists() == false) ||
             (fileDescp2.exists() == false))
            return true;
         
         // If the user wants the binary version of the files to be checked
         if (binaryCompare == true) {
            
            // Check the lengths of the file
            if (fileDescp1.length() != fileDescp2.length())
               return true;
         
            // Compare each byte in the files
            stream1 = new BufferedInputStream(new FileInputStream(fileDescp1));
            stream2 = new BufferedInputStream(new FileInputStream(fileDescp2));
         
            do {
               input1 = stream1.read();
               input2 = stream2.read();
            
               if (((input1 == -1) && (input2 != -1)) ||
                   ((input1 != -1) && (input2 == -1))) {
                  different = true;
                  break;
               }
               
               if (input1 != input2) {
                  different = true;
                  break;
               }
               
            } while ((input1 != -1) && (input2 != -1));
                     
                  
            stream1.close();
            stream2.close();
            return different;
         }
         else {
            
            // Compare each line in the files
            reader1 = new BufferedReader(new FileReader(file1));
            reader2 = new BufferedReader(new FileReader(file2));
            
            do {
               line1 = reader1.readLine();
               line2 = reader2.readLine();
            
               if (((line1 == null) && (line2 != null)) ||
                   ((line1 != null) && (line2 == null))) {
                  different = true;
                  break;
               }
               
               if ((line1 != null) && (line1.equals(line2) == false)) {
                  different = true;
                  break;
               }
               
            } while ((line1 != null) && (line2 != null));
                     
                  
            reader1.close();
            reader2.close();
            return different;
         }
      }
      catch (Exception exception) {     
         Log.excp("FileOps", fnct_name, exception.toString());
         return true;
      }
   }
   
   /**
    * Stats the command line taking into acocunt WIN32 
    * Filenames with spaces
    */ 
   public static boolean Stat(String strCmdLine)
   {
        String strFilename=null;
	StringTokenizer st = new StringTokenizer(strCmdLine);
        
        Log.debug("FileOps", "Stat","Stating... <" + strCmdLine + ">");

     	while (st.hasMoreTokens()) {
            
         if(strFilename==null)
           strFilename=st.nextToken();
         else
           strFilename= strFilename + " " + st.nextToken();
         
	if(new File(strFilename).exists())
          return true;
        }
        
        Log.warn("FileOps", "Stat","Stat Failed for... " + strFilename);

        return false;
   }

}
