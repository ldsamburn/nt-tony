/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import java.net.Socket;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;

import COMMON.PropertiesLoader;
import COMMON.INI;

import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.SocketLib;

import ntcsss.util.SystemOps;

/**
 * Start a given process on an application's host machine
 */
public class StartRemoteProcess
{
   private static boolean startAsUser;
   
   public static void main(String[] arguments)
   {    
      StringBuffer ntcss_pid;
      String application, user, description, command;
      String parameters = null;
      int parameterStartIndex;
      
      try {
         
         // The first argument should always be the properties file.
         //  Load the properties file
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0]) == false)) {
            System.out.println("Error loading properties file");
            System.exit(1);
         }
         
         // The next argument indicates whether a user name was given for
         //  starting the process with
         if (arguments.length < 2) {
            System.out.println("Invalid number of arguments");
            System.exit(1);
         }
         else
            startAsUser = arguments[1].equals("ASUSER");
            
         // Make sure the right number of arguments are given
         if (((startAsUser == true) && (arguments.length < 6)) ||
             ((startAsUser == false) && (arguments.length < 5))) {   
            displayUsage();
            System.exit(1);
         }                   
         
         // Get the arguments
         if (startAsUser == false) {
            application = arguments[2];
            user = SystemOps.getCurrentSystemUser();           
            description = arguments[3];
            command = arguments[4]; 
            parameterStartIndex = 5;
         }
         else {
            application = arguments[2];
            user = arguments[3];
            description = arguments[4];
            command = arguments[5];
            parameterStartIndex = 6;
         }
                  
         for (int i = parameterStartIndex; i < arguments.length; i++) {
            if (parameters == null)
               parameters = arguments[i];
            else
               parameters += " " + arguments[i];
         }
         
         // Start the requested process
         ntcss_pid = new StringBuffer();
         if (startProcess(application, user, description, command, parameters,
                          ntcss_pid) == true) {
            System.out.println(ntcss_pid.toString());
            System.exit(0);
         }
         else {
            System.out.println("Unable to start process for " + arguments[2]);
            System.exit(1);
         }
      }
      catch (Exception exception) {
      }
   }
   
   private static void displayUsage()
   {
      if (startAsUser == false) {
         System.out.println("");
         System.out.println("Usage: ");
         System.out.println("      NtcssStartRemoteProcess <App> <Desc> <Cmd> <Parms>");
         System.out.println("");
         System.out.println("      Where:");
         System.out.println("         App    -> The Application Name.");
         System.out.println("         Desc   -> A brief description of the process");
         System.out.println("         Cmd    -> The complete path and command.");
         System.out.println("         Parms  -> The parameters for the command.");
         System.out.println("");
      }
      else {
         System.out.println("");
         System.out.println("Usage: ");
         System.out.println("      NtcssStartRemoteProcessAsUser <App> <User> <Desc> <Cmd> <Parms>");
         System.out.println("");
         System.out.println("      Where:");
         System.out.println("         App    -> The Application Name.");
         System.out.println("         User   -> The User Name.");
         System.out.println("         Desc   -> A brief description of the process");
         System.out.println("         Cmd    -> The complete path and command.");
         System.out.println("         Parms  -> The parameters for the command.");
         System.out.println("");
      }
   }
   
   /**
    * Sends the start message to the command server on the application's host.
    *  If no errors occur, true is returned and the job's ntcss job id is 
    *  stored in the given ntcss_pid buffer. Otherwise false is returned.
    */
   private static boolean startProcess(String application, String user, 
                                       String description, String command, 
                                       String parameters, StringBuffer ntcss_pid)
   {
     /*
        NtcssServerProcessDescr spd;
      
        spd.owner_str = user;      
        spd.prog_name_str = command;
        if (parameters != null)
           spd.prog_args_str = parameters;
        else
           spd.prog_args_str = "";
        spd.job_descrip_str = description;
        spd.seclvl = 0;
        spd.priority_code = 1;
        spd.needs_approval_flag = 0;
        spd.restartable_flag = 0;
        spd.prt_output_flag = 0;
        spd.needs_device_flag = 0;
        spd.sdi_required_flag = 0;
        spd.copies = 0;
        spd.banner = 0;
        spd.orientation = 0;
        spd.papersize = 1;
        
        return NtcssStartServerProcessEx(application, spd, ntcss_pid);
       */   
      
      String token;      
      INI ini;
      String hostname;      
      String message;
      Socket socket;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;
      byte buffer[];
      final int bufferSize = 100;
      StringBuffer responseBuffer;
      int bytesRead;
      Object bundleItems[];
      String commandLine;
      
      try {         
         
         // Create a token string
         if ((token = SystemOps.getSystemEnvironmentVariable("CURRENT_NTCSS_PID")) == null)
            return false;
         else
            token = "XXXXXX" + token;                  
         
         // Get the server the application resides on
         ini = new INI();         
         if ((hostname = ini.GetProfileString(application, "SERVER", "UNKNOWN", 
                                         Constants.NTCSS_PROGRP_DB_FILE)) == null)
            return false;   
                  
         commandLine = (parameters != null) ? (command + " " + parameters) : command;         
         
         // Create the message to send
         message = StringLib.padString("LAUNCHSVRPROCBG", Constants.CMD_LEN, '\0') + 
                   StringLib.padString(user, Constants.MAX_LOGIN_NAME_LEN, ' ') +
                   StringLib.padString(token, Constants.MAX_TOKEN_LEN, ' ') +
                   StringLib.padString(application, Constants.MAX_PROGROUP_TITLE_LEN, ' ') +
                   StringLib.valueOf(Constants.WAITING_FOR_EXEC_STATE, 2) +
                   "01" +  // priority                   
                   StringLib.padString(commandLine, Constants.MAX_PROC_CMD_LINE_LEN, ' ') +                   
                   StringLib.padString(user, Constants.MAX_LOGIN_NAME_LEN, ' ') +
                   StringLib.padString(description, Constants.MAX_JOB_DESCRIP_LEN, ' ') +
                   "00" + // security class
                   StringLib.padString("", Constants.MAX_PRT_NAME_LEN, ' ') + // printer name
                   StringLib.padString("", Constants.MAX_HOST_NAME_LEN, ' ') + // printer host name
                   "00" + // printer security class
                   StringLib.padString("", Constants.MAX_PRT_FILE_LEN, ' ') + // prt filename                      
                   "00" + // printer flag
                   "0" + // sdi required
                   "0" + // sdi input
                   StringLib.padString("", Constants.MAX_SDI_CTL_RECORD_LEN + 1, ' ') + // sdi control record
                   StringLib.padString("", Constants.MAX_SERVER_LOCATION_LEN, ' ') +  // sdi data file
                   StringLib.padString("", Constants.MAX_SERVER_LOCATION_LEN, ' ') +  // sdi tci file
                   "00" + // orientation
                   "0" + // banner
                   "00" + // copies
                   "01" + // papersize                   
                   "\0";        

         // Create a socket to the app host
         socket = new Socket(hostname, Constants.CMD_SVR_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());

         // Send the message
         outputStream.write(message.getBytes(), 0, message.length());
         outputStream.flush();

         // Read response
         buffer = new byte[bufferSize];
         responseBuffer = new StringBuffer();
         do {
            bytesRead = SocketLib.readStream(inputStream, buffer, bufferSize);
            
            if (bytesRead > 0) 
               responseBuffer.append(new String(buffer, 0, bytesRead));                           
         }
         while (bytesRead > 0);

         inputStream.close();
         outputStream.close();
         socket.close();         

         // Check the response
         if (responseBuffer.substring(0, Constants.CMD_LEN).trim().equals("GOODSVRPROC") == false)
            return false;
         
         // Unbundle the returned data
         bundleItems = new Object[1];
         BundleLib.unbundleData(responseBuffer.substring(Constants.CMD_LEN), 
                                "C", bundleItems);      

         // Save off the returned pid
         ntcss_pid.insert(0, ((String)bundleItems[0]).trim());
         
         return true;
      }      
      catch (Exception exception) {
         System.out.println("An exception occurred <" + exception.toString() + ">");
         return false;
      }      
   }
}
