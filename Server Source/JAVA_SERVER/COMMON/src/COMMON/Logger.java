/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

//Description:  A logger object. NOTE: This is a singleton


package COMMON;

import java.io.*;
import java.text.*;
import java.util.*;

public class Logger
{
   private static Logger instance = new Logger();
   private SimpleDateFormat dateFormatter;
   private PrintWriter  out;
   private StringBuffer buf;

   /**
    * Constructor
    */
   protected Logger()
   {
      dateFormatter = new SimpleDateFormat("MMM d HH:mm:ss");
      out = new PrintWriter(System.err);
      buf = new StringBuffer();
   }

   /**
    * Returns the single instance of this class
    */
   public static Logger getInstance()
   {
      return instance;
   }

   /**
     * Sets the output stream.
     */
   public void setOutputStream(OutputStream out)
   {
      this.out = new PrintWriter(out);
   }

   /**
     * Sets the output stream to the OutputStream created with
     * the given file.  The subsequent log messges will be appended to the
     * file if "append" is true, otherwise, the content of the file will
     * be cleared before the subsequent log message out.
     */
   public void setOutputStream(String file, boolean append) throws IOException
   {
      setOutputStream(new FileOutputStream(file, append));
   }

   /**
    * Logs the information
    */
   public boolean out(String logEntry)
   {
      boolean ok;

      try {
         synchronized (buf) {
            buf.setLength(0);
            buf.append(getCurrentTime());
            buf.append(" ");
            buf.append(logEntry);
            out.println(buf.toString());
            out.flush();
         }

         ok = true;
      }
      catch (Exception e) {
         ok = false;
      }

      return true;
   }

   /**
    * Returns a string specifing the current time in the format:
    *  Month Data Time
    */
   private String getCurrentTime()
   {
      String date;

      try {
         date = dateFormatter.format(new Date());
      }
      catch (Exception e) {
         date = " ";
      }

      return date;
   }
}