/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.net.Socket;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.io.File;

import COMMON.PropertiesLoader;

import ntcsss.libs.Constants;
import ntcsss.libs.SvrStatLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.SocketLib;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.log.EntityName;
import ntcsss.log.Log;


/**
 * Sends a message to the servers to indicate to them
 *  that they need to shutdown
 */
public class NtcssStop
{
   /**
    * Stops the NTCSS servers on the local host
    */
   public static void main(String args[])
   {           
      try {
         
         Log.setDefaultEntityName(EntityName.NTCSS_STOP);

         // Load the properties file
         if ((args.length < 1) || (PropertiesLoader.load(args[0]) == false)) {
            printError("Error loading properties file");
            exit(1);
         }
         
         // Check to see if any of the local NTCSS servers are up
         if (areServersRunning() == false) {
            printError("Sorry, NTCSSII is not currently running");
            exit(1);
         }
         
         // Check to see if anybody is still logged on to the local host
         if (areUsersLoggedIn() == true) {
            printError("At least one user still logged into this server");
            exit(1);
         }
         
         // In the legacy code, it was at this point that the daemons were
         //  idled. This was done so that do one could log in while the
         //  server processes where killed, but allow possible messaging
         //  needed to kill these processes. Since that messaging isn't needed
         //  in the this implementation, just the "die" message is sent which
         //  will prevent anyone from logging in and cause command server
         //  to kill any running server processes.
         
         // Send the stop message to the servers. NOTE: When the stop
         //  message is sent to the command server, it will stop the
         //  running jobs.
         sendStopMessage(Constants.CMD_SVR_PORT);         
         sendStopMessage(Constants.MSG_SVR_PORT);
         sendStopMessage(Constants.SPQ_SVR_PORT);
         sendStopMessage(Constants.DB_SVR_PORT);
         sendStopMessage(Constants.DSK_SVR_PORT);
         sendStopMessage(Constants.PRTQ_SVR_PORT);
         
         
         // !!!WARNING!!!: Note any code placed below this line must be carefully
         //  considered. Although the "stop" message has been sent to the servers,
         //  they are not guaranteed to be completly shutdown when execution 
         //  reaches this point. For instance, code that cleans up temporary
         //  files should not be placed here since these files may still be in 
         //  use by the servers during their shutdown process. Code of this
         //  nature should be placed at the end of NtcssStart.                  
         
         exit(0);
      }
      catch (Exception exception) {
         Log.excp("NtcssStop", "main", exception.toString());
         
         exit(1);
      }
   }   
   
   /**
    * Sends the given string to the proper log sink
    */
   private static void printError(String str)
   {
      Log.error("NtcssStop", "stop", str);
      System.out.println(str);
   }   
   private static void printInfo(String str)
   {
      Log.info("NtcssStop", "stop", str);
      System.out.println(str);
   }
   private static void printExcp(String str)
   {
      Log.excp("NtcssStop", "stop", str);
      System.out.println(str);
   }
   
   private static void exit(int exitValue)
   {
      if (exitValue != 0) 
        printError("Cannot stop");      
      else {
         
         // Create the file that indicates the stop message was successful.
         //  This file's existence is checked for in the Windows 
         //  "NTCSS Server" service to see if it should stop or not.
         try {            
            File stopFile = new File(Constants.STOP_FILE);
            if (stopFile.createNewFile() == false)
               Log.error("NtcssStop", "exit", "Could not create stop file");
         }
         catch (Exception exception) {
            Log.excp("NtcssStop", "exit", exception.toString());
         }
      }
      
      System.exit(exitValue);
   }
   
   /**
    * Returns true if atleast one NTCSS server on the local host is alive 
    *  regardless of its state (i.e. awake or idle).
    */
   private static boolean areServersRunning()
   {
      try {
         
         if (SvrStatLib.isServerRunning("localhost", Constants.DSK_SVR_PORT) == 1)
            return true;
         else if (SvrStatLib.isServerRunning("localhost", Constants.DB_SVR_PORT) == 1)
            return true;
         else if (SvrStatLib.isServerRunning("localhost", Constants.SPQ_SVR_PORT) == 1) 
            return true;
         else if (SvrStatLib.isServerRunning("localhost", Constants.PRTQ_SVR_PORT) == 1) 
            return true;
         else if (SvrStatLib.isServerRunning("localhost", Constants.MSG_SVR_PORT) == 1) 
            return true;
         else if (SvrStatLib.isServerRunning("localhost", Constants.CMD_SVR_PORT) == 1)  
            return true;
         else
            return false;
      }
      catch (Exception exception) {
         Log.excp("NtcssStop", "areServersRunning", exception.toString());
         return false;
      }
   }
   
   /**
    * Checks to see if any NTCSS users are logged into this host
    */
   private static boolean areUsersLoggedIn()
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      boolean ret;
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null)             
            return false;         

         // Get the information from the database 
         statement = connection.createStatement();
         resultSet = statement.executeQuery("SELECT * FROM current_users");
  
         // Get the type from the returned result set
         if (resultSet.next() == false)
            ret = false;
         else
            ret = true;            
         
         // Close the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return ret;   
      }
      catch (Exception exception) {
         Log.excp("NtcssStop", "areUsersLoggedIn", exception.toString());
         return false;
      }
   }      

   /**
    * Sends the "shutdown" message to the given port on the local machine
    */
   private static void sendStopMessage(int port)
   {                
      String message;

      try {         
         message = StringLib.formatMessage("SERVERSTATUS", 
                        String.valueOf(Constants.SERVER_STATE_DIE));  
         SocketLib.sendFireForgetMessage("localhost", port, message);                                               
      }
      catch (Exception exception) {
         Log.excp("NtcssStop", "sendMessage", exception.toString());                            
      }
   }      
}