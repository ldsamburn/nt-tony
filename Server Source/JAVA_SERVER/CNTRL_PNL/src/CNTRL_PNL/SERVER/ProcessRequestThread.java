/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.SERVER;

import java.net.*;
import java.io.*;
import java.util.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

import ntcsss.libs.DataLib;
import ntcsss.libs.NDSLib;

import ntcsss.tools.NtcssCmd;

import ntcsss.util.SystemOps;

public class ProcessRequestThread extends Thread
{
   private MessageWithData msg;
   private HandlerCounter counter;
   private KillFlag killFlag;
   private boolean msgRetrieved;

   public ProcessRequestThread(MessageWithData message, HandlerCounter cntr)
   {
      setPriority(Thread.MAX_PRIORITY);

      counter = cntr;
      counter.increment();

      msg = message;
      msgRetrieved = true;

      killFlag = null;
      
      // Set this thread to be a daemon thread so that when the server ends,
      //  this thread will automatically end
      setDaemon(true);

      start();
   }

   public ProcessRequestThread(Socket socket, HandlerCounter cntr, KillFlag flag)
   {
      setPriority(Thread.MAX_PRIORITY);

      counter = cntr;
      counter.increment();

      msg = new MessageWithData(socket);
      msgRetrieved = false;

      killFlag = flag;
      
      // Set this thread to be a daemon thread so that when the server ends,
      //  this thread will automatically end
      setDaemon(true);

      start();
   }

   public void run()
   {
      if (msgRetrieved == false)
         msg.receive();
      
      if (msg.getHeader() == null)
         Log.log(Log.SVR, Log.ERR, this.getClass().getName(), "run",
                             "Received message null header");
      else {
         
         // Log the message header
         Log.log(Log.SVR, Log.INFO, this.getClass().getName(), "run",
                             "Received message - " + msg.getHeader());

         // Figure out which message header was received and process the data
         if (msg.headerEquals(CPMessageHeaders.GET_SERVER_STATUS))
            getServerStatus();
         else if (msg.headerEquals(CPMessageHeaders.START_SERVER))
            startServer(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.HALT_SERVER))
            haltServer();
         else if (msg.headerEquals(CPMessageHeaders.GET_SERVER_STATE))
            getServerState();
         else if (msg.headerEquals(CPMessageHeaders.AWAKEN_SERVER))
            awakenServer();
         else if (msg.headerEquals(CPMessageHeaders.IDLE_SERVER))
            idleServer();
         else if (msg.headerEquals(CPMessageHeaders.GET_LOGIN_STATE))
            getLoginState();
         else if (msg.headerEquals(CPMessageHeaders.ENABLE_LOGINS))
            enableLogins();
         else if (msg.headerEquals(CPMessageHeaders.DISABLE_LOGINS))
            disableLogins();
         else if (msg.headerEquals(CPMessageHeaders.GET_SERVER_DEBUG_STATE))
            getServerDebugState();
         else if (msg.headerEquals(CPMessageHeaders.TURN_DEBUG_ON))
            turnDebugOn();
         else if (msg.headerEquals(CPMessageHeaders.TURN_DEBUG_OFF))
            turnDebugOff();
         else if (msg.headerEquals(CPMessageHeaders.GET_NUM_CURRENT_USERS))
            getNumCurrentUsers();
         else if (msg.headerEquals(CPMessageHeaders.GET_HOST_UP_TIME))
            getHostUpTime();
         else if (msg.headerEquals(CPMessageHeaders.GET_NTCSS_UP_TIME))
            getNtcssUpTime();
         else if (msg.headerEquals(CPMessageHeaders.MULTICAST_PING))
            sendMulticastPingAck(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.PING))
            sendPingAck();
         else if (msg.headerEquals(CPMessageHeaders.CREATE_STATUS_INI_FILE))
            createStatusINIFile();
         else if (msg.headerEquals(CPMessageHeaders.CREATE_LOGIN_HISTORY_INI_FILE))
            createLgnHstryINIFile(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.CREATE_SYSTEM_LOG_INI_FILE))
            createSystmLogINIFile(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.CREATE_COMMON_DATA_INI_FILE))
            createCommonDataINIFile();
         else if (msg.headerEquals(CPMessageHeaders.GET_INI_DIRECTORY))
            getINIDirectory();
         else if (msg.headerEquals(CPMessageHeaders.KILL))
            killSvr();
         else if (msg.headerEquals(CPMessageHeaders.GET_SERVER_TYPE))
            getServerType();
         else if (msg.headerEquals(CPMessageHeaders.SET_COMMON_DATA))
            setCommonData(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.MAKE_NTCSS_MASTER))
            makeNTCSSMasterSvr(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.MAKE_NTCSS_AUTH))
            makeNTCSSAuthSvr(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.MAKE_NTCSS_APP))
            makeNTCSSAppSvr(msg.getData());
         else if (msg.headerEquals(CPMessageHeaders.START_NDS_SERVER))
            startNDSServer();
         else if (msg.headerEquals(CPMessageHeaders.STOP_NDS_SERVER))
            stopNDSServer();
         else if (msg.headerEquals(CPMessageHeaders.RUN_NDS_PROGRAM))
            runNDSProgram(msg.getData());
         else
            Log.log(Log.SVR, Log.ERR, this.getClass().getName(), "run",
                                "Error: Unknown message - " + msg.getHeader());
      }

      // Closes the socket and any associated IO streams
      msg.finished();

      counter.decrement();
   }

   // Starts the daemons for the Ntcss server on this host and sends a response
   //  back to the control panel
   private void startServer(String parameters)
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (startServer_aux(parameters) == 0)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Starts the daemons for the Ntcss server on this host and returns the
   //  process exit value
   private int startServer_aux(String parameters)
   {
      Process ps;
      String[] cmdArray;
      int exitValue = 99;
      StringTokenizer strTokenizer;
      int numParams;

      try {
         // Create cmdArray
         if (parameters != null) {
            strTokenizer = new StringTokenizer(parameters);
            numParams = strTokenizer.countTokens();
            cmdArray = new String[1 + numParams];

            cmdArray[0] = Constants.SVR_SERVICE_START;
            for (int i = 0; i < numParams; i++)
               cmdArray[i + 1] = (strTokenizer.nextToken()).trim();
         }
         else {
            cmdArray = new String[1];
            cmdArray[0] = Constants.SVR_SERVICE_START;
         }

         // Execute the command to start the server
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Pause for a few seconds to make sure the server is up
         sleep(5000);
         
         return 0;
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "startServer_aux",
                 e.toString());
         
         return exitValue;
      }      
   }

   // Get whether or not the Ntcss daemons are up or down on this host and sends
   //  the proper respose back to the control panel
   private void getServerStatus()
   {
      // Send the proper response back to the control panel
      if (getServerStatus_aux() == 1)
         msg.setHeader(CPMessageHeaders.DAEMONS_UP);
      else
         msg.setHeader(CPMessageHeaders.DAEMONS_DOWN);
      msg.send();
   }

   // Get whether or not the Ntcss daemons are up or down on this host
   private int getServerStatus_aux()
   {
      //String[] cmdArray = {"/bin/sh", "-c", "ps -e | grep gen_se | grep -v grep | wc -l"};
      int[] spqSvrInfo  = getDaemonInfo(Constants.SPQ_SVR_PORT);
      if (spqSvrInfo == null)
         return 0;
      
      int[] prtqSvrInfo = getDaemonInfo(Constants.PRTQ_SVR_PORT);
      if (prtqSvrInfo == null)
         return 0;
      
      int[] dskSvrInfo  = getDaemonInfo(Constants.DSK_SVR_PORT);
      if (dskSvrInfo == null)
         return 0;
      
      int[] cmdSvrInfo  = getDaemonInfo(Constants.CMD_SVR_PORT);
      if (cmdSvrInfo == null)
         return 0;
      
      int[] msgSvrInfo  = getDaemonInfo(Constants.MSG_SVR_PORT);
      if (msgSvrInfo == null)
         return 0;
      
      int[] dbSvrInfo   = getDaemonInfo(Constants.DB_SVR_PORT);
      if (dbSvrInfo == null)
         return 0;

      return 1;
   }

   // Retrieves awake/idle state, login state, and debug state for the specified
   //  NTCSS server daemon
   private int[] getDaemonInfo(int daemonPort)
   {
      Socket socket;
      BufferedOutputStream output;
      BufferedReader input;
      byte[] buffer = new byte[22]; // elements initialized to zero by default
      String info;
      int[] ret = null;

      try {
         socket = new Socket(localHostname(), daemonPort);
         output = new BufferedOutputStream(socket.getOutputStream());
         input = new BufferedReader(new InputStreamReader(
                                                      socket.getInputStream()));

         buffer[0] = 83;  // S
         buffer[1] = 69;  // E
         buffer[2] = 82;  // R
         buffer[3] = 86;  // V
         buffer[4] = 69;  // E
         buffer[5] = 82;  // R
         buffer[6] = 83;  // S
         buffer[7] = 84;  // T
         buffer[8] = 65;  // A
         buffer[9] = 84;  // T
         buffer[10] = 85; // U
         buffer[11] = 83; // S
         buffer[20] = 50; // 2  i.e. CHECK

         // Send the message to the daemon
         output.write(buffer);
         output.flush();

         // Retreive the information from the daemon
         info = input.readLine();
         if ((info.substring(0, 16)).equals("GOODSERVERSTATUS")) {
            ret = new int[3];

            // Read the idle/awake state info
            if (info.charAt(21) == '0')
               ret[0] = 0; // Daemon idle
            else if (info.charAt(21) == '1')
               ret[0] = 1; // Daemon awake
            else
               ret[0] = -1; // Unknown

            // Read the login state info
            if (info.charAt(23) == '8')
               ret[1] = 0; // Logins blocked
            else if (info.charAt(23) == '9')
               ret[1] = 1; // Logins allowed
            else
               ret[1] = -1; // Unknown

            // Read the debug state info
            if (info.charAt(25) == '4')
               ret[2] = 1; // Debug on
            else if (info.charAt(25) == '5')
               ret[2] = 0; // Debug off
            else
               ret[2] = -1; // Unknown
         }

         // Close the IO streams and socket connection
         output.close();
         input.close();
         socket.close();
      }
      catch (ConnectException e) {
         return ret;
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "getDaemonInfo",
                 e.toString());
      }

      return ret;
   }

   // Halt the Ntcss daeomons on this host and return the proper response back
   //  to the control panel
   private void haltServer()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (haltServer_aux() == 0)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Halt the Ntcss daeomons on this host and return the exit value
   private int haltServer_aux()
   {
      Process ps;
      String[] cmdArray = {Constants.SVR_SERVICE_STOP};
      int exitValue = 99;

      try {
         // Execute the command to halt the server
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();

         exitValue = ps.exitValue();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "haltServer_aux",
                 e.toString());
      }

      return exitValue;
   }


   // Get the number of users logged on to this server and returns the number
   //  to the control panel
   private void getNumCurrentUsers()
   {
      // Send the information back to the control panel
      msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      msg.setData(getNumCurrentUsers_aux());
      msg.send();
   }

   // Get the number of users logged on to this server and returns the number
   private String getNumCurrentUsers_aux()
   {
      return String.valueOf(DataLib.numCurrentUsers(null));
   }

   // Find out if the server is idled or awake and return the state to the
   //  control panel
   private void getServerState()
   {
      // Check the idle/awake state of each daemon on the server
      if (getServerState_aux() == 1)
         msg.setHeader(CPMessageHeaders.DAEMONS_AWAKE);
      else
         msg.setHeader(CPMessageHeaders.DAEMONS_IDLE);
      msg.send();
   }

   // Find out if the server is idled or awake and return the state
   private int getServerState_aux()
   {
      int[] spqSvrInfo  = getDaemonInfo(Constants.SPQ_SVR_PORT);
      int[] prtqSvrInfo = getDaemonInfo(Constants.PRTQ_SVR_PORT);
      int[] dskSvrInfo  = getDaemonInfo(Constants.DSK_SVR_PORT);
      int[] cmdSvrInfo  = getDaemonInfo(Constants.CMD_SVR_PORT);
      int[] msgSvrInfo  = getDaemonInfo(Constants.MSG_SVR_PORT);
      int[] dbSvrInfo   = getDaemonInfo(Constants.DB_SVR_PORT);

      // Check the idle/awake state of each daemon on the server
      if (((spqSvrInfo != null) && (prtqSvrInfo != null) && (dskSvrInfo != null)
           && (cmdSvrInfo != null) && (msgSvrInfo != null) && (dbSvrInfo != null)) &&
          ((spqSvrInfo[0] == 1) && (prtqSvrInfo[0] == 1) &&
           (dskSvrInfo[0] == 1) && (cmdSvrInfo[0] == 1) &&
           (msgSvrInfo[0] == 1) && (dbSvrInfo[0] == 1)))
         return 1;
      else
         return 0;
   }

   // Awakens the server and returns success of failure
   private void awakenServer()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (awakenServer_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Attempts to awaken the Ntcss server
   private boolean awakenServer_aux()
   {
      return NtcssCmd.handleAwaken();
   }

   // Idles the server and returns success of failure
   private void idleServer()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (idleServer_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Attempts to idle the Ntcss server
   private boolean idleServer_aux()
   {
      return NtcssCmd.handleIdle();
   }

   // Checks to see if users are allowed to login to this server and return the
   //  result back to the control panel
   private void getLoginState()
   {
      // Send the proper response back to the control panel
      if (getLoginState_aux() == true)
         msg.setHeader(CPMessageHeaders.LOGINS_ALLOWED);
      else
         msg.setHeader(CPMessageHeaders.LOGINS_DISALLOWED);
      msg.send();
   }

   // Checks to see if users are allowed to login to this server
   private boolean getLoginState_aux()
   {
      boolean allowed = false;
      int[] spqSvrInfo  = getDaemonInfo(Constants.SPQ_SVR_PORT);
      int[] prtqSvrInfo = getDaemonInfo(Constants.PRTQ_SVR_PORT);
      int[] dskSvrInfo  = getDaemonInfo(Constants.DSK_SVR_PORT);
      int[] cmdSvrInfo  = getDaemonInfo(Constants.CMD_SVR_PORT);
      int[] msgSvrInfo  = getDaemonInfo(Constants.MSG_SVR_PORT);
      int[] dbSvrInfo   = getDaemonInfo(Constants.DB_SVR_PORT);

      // Check the login status of each daemon on the server
      if (((spqSvrInfo != null) && (prtqSvrInfo != null) && (dskSvrInfo != null)
           && (cmdSvrInfo != null) && (msgSvrInfo != null) && (dbSvrInfo != null)) &&
          ((spqSvrInfo[1] == 1) && (prtqSvrInfo[1] == 1) &&
           (dskSvrInfo[1] == 1) && (cmdSvrInfo[1] == 1) &&
           (msgSvrInfo[1] == 1) && (dbSvrInfo[1] == 1)))
         allowed = true;

      return allowed;
   }

   // Enables users to login to this server and returns success or failure to
   //  the control panel
   private void enableLogins()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (enableLogins_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Enables users to login to this server
   private boolean enableLogins_aux()
   {
      String[] cmdArray = {"logins", "allow"};
      
      return NtcssCmd.handleLogins(cmdArray);
   }

   // Disables users from logging in to this server and return success or
   //  failure to the control panel
   private void disableLogins()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (disableLogins_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Disables users from logging in to this server
   private boolean disableLogins_aux()
   {
      String[] cmdArray = {"logins", "block"};
      
      return NtcssCmd.handleLogins(cmdArray);
   }

   // Gets information from the local host and sends it to the specified
   //  host running the control panel
   private void sendMulticastPingAck(String data)
   {     
      String localHostname = localHostname();
      String IPaddress = getIPAddr();  
      int serverType = getServerType_aux(localHostname);         
      int serverStatus = getServerStatus_aux();
      String delimiter = MessageWithData.dataFieldDelimiter;
      String cpHost;
      int listeningPort;
      MessageWithData msg;
      StringTokenizer st;

      try {
         // Parse the data received for the hostname and port to send the
         //  response to
         st = new StringTokenizer(data, MessageWithData.dataFieldDelimiter);
         cpHost = st.nextToken();
         listeningPort = Integer.parseInt((st.nextToken()).trim());

         msg = new MessageWithData(cpHost, listeningPort);
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         msg.setData(localHostname + delimiter +
                     IPaddress     + delimiter +
                     Integer.toString(serverType)   + delimiter +
                     Integer.toString(serverStatus));      
         msg.send();         
         
         msg.finished();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "sendMulticastPingAck", e.toString());
      }
   }

   // Returns the hostname of the host this server is running on
   private String localHostname()
   {
      String hostname = null;

      try {
         hostname = InetAddress.getLocalHost().getHostName();
         hostname = hostname.substring(0, hostname.indexOf("."));
      }
      catch (UnknownHostException e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "localHostname",
                 e.toString());
      }
      finally {
         return hostname;
      }
   }

   // Returns the IP address of the local host
   private String getIPAddr()
   {
      InetAddress addr;
      String IPaddr;

      try{
         addr = InetAddress.getByName(localHostname());
         IPaddr = addr.getHostAddress();
      }
      catch(Exception e) {
         IPaddr = null;
      }

      return IPaddr;
   }

   // Retrieves the server type of this host and returns the result to the
   //  control panel.
   private void getServerType()
   {
      int serverType;

      // Get the server type
      serverType = getServerType_aux(localHostname());

      // Send the proper response back to the control panel
      if (serverType == Constants.MASTER_SVR)
         msg.setHeader(CPMessageHeaders.MASTER_SVR);
      else if (serverType == Constants.AUTH_SVR)
         msg.setHeader(CPMessageHeaders.AUTH_SVR);
      else if (serverType == Constants.APP_SVR)
         msg.setHeader(CPMessageHeaders.APP_SVR);
      else
         msg.setHeader(CPMessageHeaders.UNKNOWN_SVR_TYPE);

      msg.send();
   }

   // Returns the server type of the given host.
   private int getServerType_aux(String hostname)
   {
      int serverType = DataLib.getHostType(hostname);         
      
      if (serverType == 0)
         return Constants.MASTER_SVR;
      else if (serverType == 1)
         return Constants.APP_SVR;
      else if (serverType == 2)
         return Constants.AUTH_SVR;
      else
         return Constants.UNKNOWN_SVR_TYPE; 
   }

   // Turns debug on for the Ntcss server and returns success or failure to
   //  the control panel
   private void turnDebugOn()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (turnDebugOn_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Attempts to turn debugging on
   private boolean turnDebugOn_aux()
   {
      String[] cmdArray = {"debug", "on"};
      return NtcssCmd.handleDebug(cmdArray);
   }

   // Turns debug off for the Ntcss server and returns success or failure to
   //  the control panel
   private void turnDebugOff()
   {
      // Check the exit value and send the proper response back to the
      //  control panel
      if (turnDebugOff_aux() == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Attempts to turn debugging off
   private boolean turnDebugOff_aux()
   {
      String[] cmdArray = {"debug", "off"};
      return NtcssCmd.handleDebug(cmdArray);
   }

   // Gets the server's debug state and returns it to the control panel
   private void getServerDebugState()
   {
      // Send the proper response back to the control panel
      if (getServerDebugState_aux() == true)
         msg.setHeader(CPMessageHeaders.DEBUG_ON);
      else
         msg.setHeader(CPMessageHeaders.DEBUG_OFF);
      msg.send();
   }

   // Gets the server's debug state
   private boolean getServerDebugState_aux()
   {
      int[] spqSvrInfo  = getDaemonInfo(Constants.SPQ_SVR_PORT);
      int[] prtqSvrInfo = getDaemonInfo(Constants.PRTQ_SVR_PORT);
      int[] dskSvrInfo  = getDaemonInfo(Constants.DSK_SVR_PORT);
      int[] cmdSvrInfo  = getDaemonInfo(Constants.CMD_SVR_PORT);
      int[] msgSvrInfo  = getDaemonInfo(Constants.MSG_SVR_PORT);
      int[] dbSvrInfo   = getDaemonInfo(Constants.DB_SVR_PORT);

      // Check the debug state of each daemon on the server
      if (((spqSvrInfo != null) && (prtqSvrInfo != null) && (dskSvrInfo != null)
           && (cmdSvrInfo != null) && (msgSvrInfo != null) && (dbSvrInfo != null)) &&
          ((spqSvrInfo[2] == 1) || (prtqSvrInfo[2] == 1) ||
           (dskSvrInfo[2] == 1) || (cmdSvrInfo[2] == 1) ||
           (msgSvrInfo[2] == 1) || (dbSvrInfo[2] == 1)))
         return true;
      else
         return false;
   }

   // Obtains the date the system OS was booted up last and returns that value
   //  to the control panel
   private void getHostUpTime()
   {
      String time;
     
      time = getHostUpTime_aux();    

      if (time != null) {
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         msg.setData(time);
      }
      else {
         msg.setHeader(CPMessageHeaders.ERROR);
         msg.setData("Error");
      }

      msg.send();
   }

   // Obtains the date the system OS was booted up last
   private String getHostUpTime_aux()
   {
      return SystemOps.getUpTime();
   }

   // Read a file which contains the date which the NTCSS server was started
   //  last and returns that value to the control panel
   private void getNtcssUpTime()
   {
      String ntcssUpTimeFile = Constants.NTCSS_LOGS_DIRECTORY + "ntcss_up_time";
      BufferedReader file;
      String buffer;

      try {
         file = new BufferedReader(new FileReader(ntcssUpTimeFile));
         buffer = file.readLine();
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         file.close();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "getNtcssUpTime",
                 e.toString());
         buffer = "Error";
         msg.setHeader(CPMessageHeaders.ERROR);
      }

      msg.setData(buffer.trim());
      msg.send();
   }

   // Create the status INI file and send back its local filename
   private void createStatusINIFile()
   {
      int[] spqSvrInfo  = getDaemonInfo(Constants.SPQ_SVR_PORT);
      int[] prtqSvrInfo = getDaemonInfo(Constants.PRTQ_SVR_PORT);
      int[] dskSvrInfo  = getDaemonInfo(Constants.DSK_SVR_PORT);
      int[] cmdSvrInfo  = getDaemonInfo(Constants.CMD_SVR_PORT);
      int[] msgSvrInfo  = getDaemonInfo(Constants.MSG_SVR_PORT);
      int[] dbSvrInfo   = getDaemonInfo(Constants.DB_SVR_PORT);
      INI iniFile = new INI();
      String statusKey = "STATUS";
      String statusAwakeValue = "AWAKE";
      String statusIdleValue = "IDLE";
      String debugKey = "DEBUG";
      String debugOnValue = "ON";
      String debugOffValue = "OFF";
      String loginsKey = "LOGINS";
      String loginsAllowedValue = "ALLOWED";
      String loginsBlockedValue = "BLOCKED";
      String unknown = "UNKNOWN";
      String spqSvr = "Job Queue Server";
      String prtqSvr = "Print Queue Server";
      String dskSvr = "Desktop Server";
      String cmdSvr = "Command Server";
      String msgSvr = "Message Server";
      String dbSvr = "Database Server";
      boolean errorOccurred;
      String filename = Constants.SVR_INI_DIRECTORY + "status.ini";

      try {
         // Create an INI file from the status information

         // Write the Job Queue Server info
         if (spqSvrInfo != null) {

            if (spqSvrInfo[0] == 1)
               iniFile.WriteProfileString(spqSvr, statusKey, statusAwakeValue, filename);
            else if (spqSvrInfo[0] == 0)
               iniFile.WriteProfileString(spqSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(spqSvr, statusKey, unknown, filename);

            if (spqSvrInfo[1] == 1)
               iniFile.WriteProfileString(spqSvr, loginsKey, loginsAllowedValue, filename);
            else if (spqSvrInfo[1] == 0)
               iniFile.WriteProfileString(spqSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(spqSvr, loginsKey, unknown, filename);

            if (spqSvrInfo[2] == 1)
               iniFile.WriteProfileString(spqSvr, debugKey, debugOnValue, filename);
            else if (spqSvrInfo[2] == 0)
               iniFile.WriteProfileString(spqSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(spqSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(spqSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(spqSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(spqSvr, loginsKey, unknown, filename);
         }

         // Write the PrintQueue Server info
         if (prtqSvrInfo != null) {

            if (prtqSvrInfo[0] == 1)
               iniFile.WriteProfileString(prtqSvr, statusKey, statusAwakeValue, filename);
            else if (prtqSvrInfo[0] == 0)
               iniFile.WriteProfileString(prtqSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(prtqSvr, statusKey, unknown, filename);

            if (prtqSvrInfo[1] == 1)
               iniFile.WriteProfileString(prtqSvr, loginsKey, loginsAllowedValue, filename);
            else if (prtqSvrInfo[1] == 0)
               iniFile.WriteProfileString(prtqSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(prtqSvr, loginsKey, unknown, filename);

            if (prtqSvrInfo[2] == 1)
               iniFile.WriteProfileString(prtqSvr, debugKey, debugOnValue, filename);
            else if (prtqSvrInfo[2] == 0)
               iniFile.WriteProfileString(prtqSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(prtqSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(prtqSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(prtqSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(prtqSvr, loginsKey, unknown, filename);
         }

         // Write the Desktop Server info
         if (dskSvrInfo != null) {

            if (dskSvrInfo[0] == 1)
               iniFile.WriteProfileString(dskSvr, statusKey, statusAwakeValue, filename);
            else if (dskSvrInfo[0] == 0)
               iniFile.WriteProfileString(dskSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(dskSvr, statusKey, unknown, filename);

            if (dskSvrInfo[1] == 1)
               iniFile.WriteProfileString(dskSvr, loginsKey, loginsAllowedValue, filename);
            else if (dskSvrInfo[1] == 0)
               iniFile.WriteProfileString(dskSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(dskSvr, loginsKey, unknown, filename);

            if (dskSvrInfo[2] == 1)
               iniFile.WriteProfileString(dskSvr, debugKey, debugOnValue, filename);
            else if (dskSvrInfo[2] == 0)
               iniFile.WriteProfileString(dskSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(dskSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(dskSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(dskSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(dskSvr, loginsKey, unknown, filename);
         }

         // Write the Command Server info
         if (cmdSvrInfo != null) {

            if (cmdSvrInfo[0] == 1)
               iniFile.WriteProfileString(cmdSvr, statusKey, statusAwakeValue, filename);
            else if (cmdSvrInfo[0] == 0)
               iniFile.WriteProfileString(cmdSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(cmdSvr, statusKey, unknown, filename);

            if (cmdSvrInfo[1] == 1)
               iniFile.WriteProfileString(cmdSvr, loginsKey, loginsAllowedValue, filename);
            else if (cmdSvrInfo[1] == 0)
               iniFile.WriteProfileString(cmdSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(cmdSvr, loginsKey, unknown, filename);

            if (cmdSvrInfo[2] == 1)
               iniFile.WriteProfileString(cmdSvr, debugKey, debugOnValue, filename);
            else if (cmdSvrInfo[2] == 0)
               iniFile.WriteProfileString(cmdSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(cmdSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(cmdSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(cmdSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(cmdSvr, loginsKey, unknown, filename);
         }

         // Write the Message Server info
         if (msgSvrInfo != null) {

            if (msgSvrInfo[0] == 1)
               iniFile.WriteProfileString(msgSvr, statusKey, statusAwakeValue, filename);
            else if (msgSvrInfo[0] == 0)
               iniFile.WriteProfileString(msgSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(msgSvr, statusKey, unknown, filename);

            if (msgSvrInfo[1] == 1)
               iniFile.WriteProfileString(msgSvr, loginsKey, loginsAllowedValue, filename);
            else if (msgSvrInfo[1] == 0)
               iniFile.WriteProfileString(msgSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(msgSvr, loginsKey, unknown, filename);

            if (msgSvrInfo[2] == 1)
               iniFile.WriteProfileString(msgSvr, debugKey, debugOnValue, filename);
            else if (msgSvrInfo[2] == 0)
               iniFile.WriteProfileString(msgSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(msgSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(msgSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(msgSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(msgSvr, loginsKey, unknown, filename);
         }

         // Write the Database Server info
         if (dbSvrInfo != null) {
            if (dbSvrInfo[0] == 1)
               iniFile.WriteProfileString(dbSvr, statusKey, statusAwakeValue, filename);
            else if (dbSvrInfo[0] == 0)
               iniFile.WriteProfileString(dbSvr, statusKey, statusIdleValue, filename);
            else
               iniFile.WriteProfileString(dbSvr, statusKey, unknown, filename);

            if (dbSvrInfo[1] == 1)
               iniFile.WriteProfileString(dbSvr, loginsKey, loginsAllowedValue, filename);
            else if (dbSvrInfo[1] == 0)
               iniFile.WriteProfileString(dbSvr, loginsKey, loginsBlockedValue, filename);
            else
               iniFile.WriteProfileString(dbSvr, loginsKey, unknown, filename);

            if (dbSvrInfo[2] == 1)
               iniFile.WriteProfileString(dbSvr, debugKey, debugOnValue, filename);
            else if (dbSvrInfo[2] == 0)
               iniFile.WriteProfileString(dbSvr, debugKey, debugOffValue, filename);
            else
               iniFile.WriteProfileString(dbSvr, debugKey, unknown, filename);
         }
         else {
            iniFile.WriteProfileString(dbSvr, statusKey, unknown, filename);
            iniFile.WriteProfileString(dbSvr, debugKey, unknown, filename);
            iniFile.WriteProfileString(dbSvr, loginsKey, unknown, filename);
         }

         iniFile.FlushINIFile(filename);

         errorOccurred = false;
      }
      catch(Exception e) {
         errorOccurred = true;
      }

      try {
         // If successful, send the filename to the control panel
         if (!errorOccurred) {
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);
            msg.setData(filename);
         }
         else
            msg.setHeader(CPMessageHeaders.ERROR);

         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "createStatusINIFile", e.toString());
      }
   }

   // Create the Login History INI file
   private void createLgnHstryINIFile(String filename)
   {
      String outputFilename;
      int maxRecords;
      INI ini = new INI();      
      
      try {   
         
         outputFilename = ini.GetProfileString("NTCSS_INI_PARAMETERS", 
                                     "OUTPUT_INI_FILENAME", "ERROR", filename);
         maxRecords = ini.GetProfileInt("LOGIN_HISTORY_OPTIONS", 
                                         "MAX_RECORD_COUNT", -1, filename);
         
         if ((outputFilename.equals("ERROR") == true) ||
             (maxRecords < 0) ||
             (DataLib.createLoginHistoryINI(outputFilename, maxRecords) == false))
            msg.setHeader(CPMessageHeaders.ERROR);
         else            
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);

         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "createLgnHstryINIFile",
                 e.toString());

         msg.setHeader(CPMessageHeaders.ERROR);
         msg.send();
      }
                                                  
   }

   // Create the System Log INI file
   private void createSystmLogINIFile(String filename)
   {
      INI ini = new INI();
      int blockSize;
      int maxRecordCount;
      int dateFilterCode, entityFilterCode;
      String startDate, endDate;
      String outputFile;
      Vector entityList;
      String entities;
      StringTokenizer entitiesTokenizer, entityTokenizer;
      
      try {
         
         // Get the input parameter from the given INI file
         // "SYSLOG_FILE" parameter is ignored
         blockSize = ini.GetProfileInt("SYSLOG_FILTER", "BLOCK_SIZE", -1, 
                                       filename);
         maxRecordCount = ini.GetProfileInt("SYSLOG_FILTER", "MAX_RECORD_COUNT", 
                                            -1, filename);
         dateFilterCode = ini.GetProfileInt("SYSLOG_FILTER", "DATE_FILTER_CODE", 
                                            -1, filename);
         entityFilterCode = ini.GetProfileInt("SYSLOG_FILTER", 
                                            "ENTITY_FILTER_CODE", -1, filename);
         startDate = ini.GetProfileString("SYSLOG_FILTER", "START_DATE", 
                                          "ERROR", filename);
         endDate = ini.GetProfileString("SYSLOG_FILTER", "END_DATE", "ERROR", 
                                        filename);
         outputFile = ini.GetProfileString("NTCSS_INI_PARAMETERS", 
                                    "OUTPUT_INI_FILENAME", "ERROR", filename);
         
         // Get a list of all the entities listed in the "ENTITY_FILTERS" section
         entityList = new Vector();  
         if ((entityFilterCode == 1) &&
             (((entities = ini.GetProfileSection("ENTITY_FILTERS", 
                                                  filename)) != null))) {
            entitiesTokenizer = new StringTokenizer(entities, 
                                                    INI.STRINGS_DELIMITER);
            while (entitiesTokenizer.hasMoreTokens() == true) {
               
               entityTokenizer = new StringTokenizer(entitiesTokenizer.nextToken(), 
                                                     INI.TAG_VALUE_DELIMITER);
               entityList.add(entityTokenizer.nextToken());
            }
         }                        

         // Check the input parameters and create the output INI file
         if ((blockSize < 0) ||
             (maxRecordCount < 0) ||
             (dateFilterCode < 0) ||
             (entityFilterCode < 0) ||
             (startDate.equals("ERROR") == true) || 
             (endDate.equals("ERROR") == true) ||
             (outputFile.equals("ERROR") == true) ||
             (DataLib.createSystemLogINI(outputFile, blockSize, maxRecordCount, 
                                         dateFilterCode, 
                                         ((entityFilterCode == 1) ? true : false),
                                         startDate, endDate, entityList) == false))             
            msg.setHeader(CPMessageHeaders.ERROR);
         else
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);

         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), 
                 "createSystmLogINIFile", e.toString());

         msg.setHeader(CPMessageHeaders.ERROR);
         msg.send();
      }
   }

   // Get the Ntcss common data and send back to the control panel
   private void createCommonDataINIFile()
   {      
      String filename = Constants.SVR_INI_DIRECTORY + "comData.ini";    
      int exitValue = 1;      
      Hashtable data;
      Enumeration keys;
      String key;
      INI ini = new INI();

      try {
         
         // Get a list of the sys conf tag/value pairs
         if ((data = NDSLib.getSysConfData()) != null) {

            // Add each tag/value pair to the INI file
            keys = data.keys();
            while (keys.hasMoreElements() == true) {
               key = (String)keys.nextElement();
               
               ini.WriteProfileString("SYS_CONF", key, (String)data.get(key), 
                                      filename);
            }
            
            // Write out the contents of the INI structure
            ini.FlushINIFile(filename);
            
            exitValue = 0;
         }         
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "createCommonDataINIFile", e.toString());
      }

      // Create the proper message to send back
      if (exitValue == 0) {
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         msg.setData(filename);
      }
      else
         msg.setHeader(CPMessageHeaders.ERROR);

      msg.send();
   }

   // Sends a reponse to a PING message sent to this host
   private void sendPingAck()
   {
      msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      msg.send();
   }

   // Retrieves the directory on this host where INI files are retrieved/stored
   //  and sends back to the control panel
   private void getINIDirectory()
   {
      msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      msg.setData(Constants.SVR_INI_DIRECTORY);
      msg.send();
   }

   // Sets a flag which will indicate to the server that it needs to shut down
   private void killSvr()
   {
      if (killFlag != null)
         killFlag.set();
   }

   // Attempts to add to the Common Data set the given tag/value pair and
   //  sends a response back to the control panel
   private void setCommonData(String tagValuePair)
   {
      String tag, value;
      boolean ret;
      StringTokenizer st;

      // Get the tag/value pair out of the data given
      st =
         new StringTokenizer(tagValuePair, MessageWithData.dataFieldDelimiter);
      tag   = st.nextToken();
      value = st.nextToken();

      // Try to set the tag/value pair
      ret = setCommonData_aux(tag, value);

      // Set up the message with the proper header
      if (ret == true)
         msg.setHeader(CPMessageHeaders.SUCCESSFUL);
      else
         msg.setHeader(CPMessageHeaders.ERROR);

      // Send the response back to the control panel
      msg.send();
   }

   // Attempts to add to the Common Data set the given tag/value pair
   private boolean setCommonData_aux(String tag, String value)
   {
      String[] cmdArray = {"comData", "set", tag, value};
      
      return NtcssCmd.handleComData(cmdArray);
   }

   // Reconfigures the host to be the NTCSS master
   private void makeNTCSSMasterSvr(String data)
   {
/*      
      String newHostname, IPaddress, domainname;
      StringTokenizer st;
      int numHosts;
      String hostData[][];

      try {
         // Parse the data into the right parameters
         st = new StringTokenizer(data, MessageWithData.dataFieldDelimiter);
         newHostname = st.nextToken();
         IPaddress   = st.nextToken();
         domainname  = st.nextToken();
         numHosts = Integer.parseInt(st.nextToken());
         hostData = new String[numHosts][4];
         for (int r = 0; r < numHosts; r++) {
            for (int c = 0; c < 4; c++)
               hostData[r][c] = st.nextToken();
         }

         // Pass the data from the message to the function which will actually
         //  do the work. Then send a message back to the client indicating
         //  its return status.
         if (makeNTCSSMaster_aux(newHostname, IPaddress, domainname, hostData)
                                                            == true)
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         else
            msg.setHeader(CPMessageHeaders.ERROR);
         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSMasterSvr", e.toString());
      }
 */
      Log.log(Log.SVR, Log.ERR, this.getClass().getName(),
                 "makeNTCSSMasterSvr", "This operation currently not supported");
      msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Reconfigures the host to be the NTCSS master
   private boolean makeNTCSSMaster_aux(String newHostname, String IPaddress,
                                       String domainname, String[][] hostData)
   {
      Process ps;
      boolean ret = false;
      String iniFilename = Constants.SVR_INI_DIRECTORY + "hosts.ini";
      INI ini;
      String cmdString;
      String[] cmdArray = {"makeNtcssMstrSvr", "-h", newHostname,
                                               "-i", IPaddress,
                                               "-d", domainname,
                                               "-f", iniFilename};

      try {
         // Remove the INI file if it exists
         if ((new File(iniFilename)).exists() == true)
            (new File(iniFilename)).delete();

         // Re-create the INI file
         ini = new INI();
         for (int r = 0; r < hostData.length; r++) {
            ini.WriteProfileString("OLD_HOSTNAMES", hostData[r][0],
                                   hostData[r][0], iniFilename);
            ini.WriteProfileString(hostData[r][0], "NEW_HOSTNAME",
                                   hostData[r][1], iniFilename);
            ini.WriteProfileString(hostData[r][0], "NEW_IP_ADDRESS",
                                   hostData[r][2], iniFilename);

            if (Integer.parseInt(hostData[r][3]) == Constants.MASTER_SVR)
               ini.WriteProfileString(hostData[r][0], "NEW_SERVER_TYPE",
                                      "Master Server", iniFilename);
            else if (Integer.parseInt(hostData[r][3]) == Constants.AUTH_SVR)
               ini.WriteProfileString(hostData[r][0], "NEW_SERVER_TYPE",
                                      "Authentication Server", iniFilename);
            else
               ini.WriteProfileString(hostData[r][0], "NEW_SERVER_TYPE",
                                      "Application Server", iniFilename);
         }
         ini.FlushINIFile(iniFilename);

         // Log the command that is to be executed
         cmdString = new String();
         for (int x = 0; x < cmdArray.length; x++)
            cmdString += cmdArray[x] + " ";
         Log.log(Log.SVR, Log.INFO, this.getClass().getName(),
                 "makeNTCSSMaster_aux", cmdString);

         // Execute the command to reconfigure the host
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();

         // Check the exit value of the process
         if (ps.exitValue() == 0)
            ret = true;
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSMaster_aux", e.toString());
      }

      return ret;
   }

   // Reconfigures the host to be an NTCSS authentication server
   private void makeNTCSSAuthSvr(String data)
   {
/*      
      String oldHostname, newHostname, IPaddress, domainname;
      String masterOldName, masterNewName, masterIP;
      StringTokenizer st;

      try {
         // Parse the data into the right parameters
         st = new StringTokenizer(data, MessageWithData.dataFieldDelimiter);
         oldHostname   = st.nextToken();
         newHostname   = st.nextToken();
         IPaddress     = st.nextToken();
         domainname    = st.nextToken();
         masterOldName = st.nextToken();
         masterNewName = st.nextToken();
         masterIP      = st.nextToken();

         // Pass the data from the message to the function which will actually
         //  do the work. Then send a message back to the client indicating
         //  its return status.
         if (makeNTCSSAuth_aux(oldHostname, newHostname, IPaddress, domainname,
                               masterOldName, masterNewName, masterIP) == true)
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         else
            msg.setHeader(CPMessageHeaders.ERROR);
         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSAuthSvr", e.toString());
      }
 */
      Log.log(Log.SVR, Log.ERR, this.getClass().getName(),
                 "makeNTCSSAuthSvr", "This operation currently not supported");
      msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Reconfigures the host to be a NTCSS authentication server
   private boolean makeNTCSSAuth_aux(String oldHostname, String newHostname,
                                     String IPaddress, String domainname,
                                     String masterOldName, String masterNewName,
                                     String masterIP)
   {
      Process ps;
      boolean ret = false;
      INI ini;
      String iniFilename = Constants.SVR_INI_DIRECTORY + "hosts.ini";
      String cmdString;
      String[] cmdArray = {"makeNtcssAuthSvr", "-h", newHostname,
                                               "-i", IPaddress,
                                               "-d", domainname,
                                               "-m", masterNewName,
                                               "-f", iniFilename};

      try {
         // Remove the INI file if it exists
         if ((new File(iniFilename)).exists() == true)
            (new File(iniFilename)).delete();

         // Re-create the INI file
         ini = new INI();
         ini.WriteProfileString("OLD_HOSTNAMES", oldHostname,
                                oldHostname, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_HOSTNAME",
                                newHostname, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_IP_ADDRESS",
                                IPaddress, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_SERVER_TYPE",
                                "Authentication Server", iniFilename);

         ini.WriteProfileString("OLD_HOSTNAMES", masterOldName,
                                masterOldName, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_HOSTNAME",
                                masterNewName, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_IP_ADDRESS",
                                masterIP, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_SERVER_TYPE",
                                "Master Server", iniFilename);

         ini.FlushINIFile(iniFilename);

         // Log the command that is to be executed
         cmdString = new String();
         for (int x = 0; x < cmdArray.length; x++)
            cmdString += cmdArray[x] + " ";
         Log.log(Log.SVR, Log.INFO, this.getClass().getName(),
                 "makeNTCSSAuth_aux", cmdString);

         // Execute the command to reconfigure the host
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();

         // Check the exit value of the process
         if (ps.exitValue() == 0)
            ret = true;
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSAuth_aux", e.toString());
      }

      return ret;
   }

   // Reconfigures the host to be an NTCSS application server
   private void makeNTCSSAppSvr(String data)
   {
/*      
      String oldHostname, newHostname, IPaddress, domainname;
      String masterOldName, masterNewName, masterIP;
      StringTokenizer st;

      try {
         
         // Parse the data into the right parameters
         st = new StringTokenizer(data, MessageWithData.dataFieldDelimiter);
         oldHostname   = st.nextToken();
         newHostname   = st.nextToken();
         IPaddress     = st.nextToken();
         domainname    = st.nextToken();
         masterOldName = st.nextToken();
         masterNewName = st.nextToken();
         masterIP      = st.nextToken();

         // Pass the data from the message to the function which will actually
         //  do the work. Then send a message back to the client indicating
         //  its return status.
         if (makeNTCSSApp_aux(oldHostname, newHostname, IPaddress, domainname,
                              masterOldName, masterNewName, masterIP) == true)
            msg.setHeader(CPMessageHeaders.SUCCESSFUL);
         else
            msg.setHeader(CPMessageHeaders.ERROR);
         msg.send();
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSAppSvr", e.toString());
      }
*/      
      
      // Since "application servers" don't exist anymore, this operation
      //  is not supported
      Log.log(Log.SVR, Log.ERR, this.getClass().getName(),
                 "makeNTCSSAppSvr", "This operation no longer supported");
      msg.setHeader(CPMessageHeaders.ERROR);
      msg.send();
   }

   // Reconfigures the host to be a NTCSS application server
   private boolean makeNTCSSApp_aux(String oldHostname, String newHostname,
                                    String IPaddress, String domainname,
                                    String masterOldName, String masterNewName,
                                    String masterIP)
   {
      Process ps;
      boolean ret = false;
      INI ini;
      String iniFilename = Constants.SVR_INI_DIRECTORY + "hosts.ini";
      String cmdString;
      String[] cmdArray = {"makeNtcssAppSvr", "-h", newHostname,
                                              "-i", IPaddress,
                                              "-d", domainname,
                                              "-m", masterNewName,
                                              "-f", iniFilename};

      try {
         // Remove the INI file if it exists
         if ((new File(iniFilename)).exists() == true)
            (new File(iniFilename)).delete();

         // Re-create the INI file
         ini = new INI();
         ini.WriteProfileString("OLD_HOSTNAMES", oldHostname,
                                oldHostname, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_HOSTNAME",
                                newHostname, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_IP_ADDRESS",
                                IPaddress, iniFilename);
         ini.WriteProfileString(oldHostname, "NEW_SERVER_TYPE",
                                "Application Server", iniFilename);

         ini.WriteProfileString("OLD_HOSTNAMES", masterOldName,
                                masterOldName, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_HOSTNAME",
                                masterNewName, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_IP_ADDRESS",
                                masterIP, iniFilename);
         ini.WriteProfileString(masterOldName, "NEW_SERVER_TYPE",
                                "Master Server", iniFilename);

         ini.FlushINIFile(iniFilename);

         // Log the command that is to be executed
         cmdString = new String();
         for (int x = 0; x < cmdArray.length; x++)
            cmdString += cmdArray[x] + " ";
         Log.log(Log.SVR, Log.INFO, this.getClass().getName(),
                 "makeNTCSSApp_aux", cmdString);

         // Execute the command to reconfigure the host
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();

         // Check the exit value of the process
         if (ps.exitValue() == 0)
            ret = true;
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSApp_aux", e.toString());
      }

      return ret;
   }
   
   // Starts the NDS (ldap) server on this host
   private void startNDSServer()
   {
      Process ps;
      String[] cmdArray = {Constants.NDS_SERVICE_START};      

      try {
         // Execute the command to start the NDS server
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();                           
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "startNDSServer",
                 e.toString());
      }
   }
   
   // Stops the NDS (ldap) server on this host
   private void stopNDSServer()
   {
      Process ps;
      String[] cmdArray = {Constants.NDS_SERVICE_STOP};      

      try {
         // Execute the command to halt the NDS server
         ps = (Runtime.getRuntime()).exec(cmdArray);

         // Wait for the child process to exit
         ps.waitFor();                           
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "stopNDSServer",
                 e.toString());
      }
   }
   
   // Runs any program that is under the NTCSSS/ldap/runtime directory. The
   //  argument given is of the format: "prgram type" "program name". 
   //  i.e.  SCRIPT start_slurpd    or    BIN notepad
   private void runNDSProgram(String program)
   {
      Process ps;           
      String cmd;
      StringTokenizer tokenizer;
      StringBuffer modifiedPath;
      String token;
      String programName;
      String programType;

      try {       
         
         // Parse the needed information out of the string given
         tokenizer = new StringTokenizer(program);
         programType = tokenizer.nextToken();
         programName = tokenizer.nextToken();
         
         // If the given program is a script, append to it the platform
         //  specific extension
         if (programType.equalsIgnoreCase("SCRIPT") == true) {
            if (Constants.IS_WINDOWS_OS == true)
               programName += ".bat";
            else
               programName += ".sh";
         }
         
         // For Windows batch scripts, start the script using cmd.exe
         if (programName.endsWith(".bat") == true) {
                     
            // Quote any directories in the path that have a space in them
            modifiedPath = new StringBuffer();         
            tokenizer = new StringTokenizer(Constants.NTCSS_LDAP_RUNTIME_DIRECTORY, "\\/");
            while (tokenizer.hasMoreTokens() == true) {

               token = tokenizer.nextToken();
               if (token.indexOf(' ') != -1)
                  token = "\"" + token + "\"";

               modifiedPath.append(token);
               modifiedPath.append("/");
            }         

            // The resulting path will have a "/" at the end
            cmd = "cmd.exe /Q /C start " + modifiedPath.toString() + programName;                                 
         }
         else 
            cmd = Constants.NTCSS_LDAP_RUNTIME_DIRECTORY + "/" + programName;                 
            
         // Execute the given program 
         ps = (Runtime.getRuntime()).exec(cmd);

         // Wait for the child process to exit
         ps.waitFor();                           
      }
      catch (Exception e) {
         Log.log(Log.SVR, Log.EXCP, this.getClass().getName(), "runNDSProgram",
                 e.toString());
      }
   }
}
