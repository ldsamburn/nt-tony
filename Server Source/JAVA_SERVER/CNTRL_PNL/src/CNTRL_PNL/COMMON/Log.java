/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.COMMON;

import java.io.*;
import java.text.*;
import java.util.*;

import COMMON.Logger;

public class Log extends Logger
{
   // Log entry types
   public static final String INFO  = "INFO";
   public static final String ERR   = "ERROR";
   public static final String EXCP  = "EXCEPTION";

   // Entity types
   public static final String CLT = "CLIENT";
   public static final String SVR = "SERVER";

   // Logs the given data
   public static boolean log(String sourceEntity, String entryType,
                             String className, String methodName,
                             String entryInfo)
   {
      String logEntry; 

      if (entryType.equals(Log.EXCP))
         logEntry = sourceEntity  + " " +
                    entryType     + " " +
                    className     + "." +
                    methodName    + " " +
                    entryInfo;
      else
         logEntry = sourceEntity + " " +
                    entryType    + " " + 
                    entryInfo;

      if (Constants.LOG_TO_SCREEN == true)
         System.out.println(logEntry);

      return getInstance().out(logEntry);
   }

   // Sets the log file. If "append" is true, log entries will be appended to
   //  the end of the log file.
   public static void setLogFile(String file)
   {
      try {
         getInstance().setOutputStream(file, false);
      }
      catch (Exception e) {
         getInstance().setOutputStream(System.err);
      }
   }
}
