/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  This was a small utility to read in a text file's contents and
//              then return it as a string to the caller.  Functionality
//              may be added in the future to support byte ordering on
//              different systems.

package COMMON;

import java.lang.StringBuffer;
import java.io.InputStreamReader;
import java.io.FileInputStream;

public class FileString
{
   private String filename;  // Holds the value of the "counter"

   /**
    * Constructor:
    * Takes a valid string representing the name/path of the file to be read.
    **/
   public FileString( String  target_file )
   {
     filename = target_file;
   }

   public String toString()
   {
     InputStreamReader input = null;
     try
     {
       StringBuffer temp = new StringBuffer();
       temp.setLength(0);

       input = new InputStreamReader(new FileInputStream(filename));

       int result;
       result = input.read();
       while(result > -1)
         {
           temp.append((char)result);
           result =  input.read();
         }
       return temp.toString();
     }
     catch (Exception exception)
     {
       return "";
     }
     finally
       {
         // Close out the io stream..
         try
           {
             if (input != null)
               input.close();
           }
         catch (Exception exception)
           {}
       }
   }
}