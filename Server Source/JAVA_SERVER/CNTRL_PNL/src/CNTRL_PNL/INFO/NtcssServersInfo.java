/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.INFO;

import java.net.*;
import java.io.*;
import java.util.*;
import java.lang.reflect.*;

import COMMON.*;
import CNTRL_PNL.COMMON.*;

// NOTE: This class is a singleton
public class NtcssServersInfo
{
   private static NtcssServersInfo info = new NtcssServersInfo();

   // Some constants
   public static final int NTCSS_SVR_UP     = 1;
   public static final int NTCSS_SVR_DOWN   = 0;
   public static final int SVR_AWAKE        = 1;
   public static final int SVR_IDLE         = 0;
   public static final int DEBUG_ON         = 1;
   public static final int DEBUG_OFF        = 0;
   public static final int LOGINS_ALLOWED   = 1;
   public static final int LOGINS_BLOCKED   = 0;
   public static final int CP_SVR_UP        = 1;
   public static final int CP_SVR_DOWN      = 0;

   // Class Constructor !NOTE! not to be made "public" since this class is a
   //  singleton
   private NtcssServersInfo()
   {
   }

   // Returns the only instance of this class
   public static NtcssServersInfo getHandle()
   {
      return info;
   }

   // Collects a list of hosts running control panel servers
   public HostInfoList collectHostsInfo()
   {
      HostInfoList ntcssHostList = new HostInfoList();

      try {
         // Create a server to receive connections from the hosts
         ServerSocket svrSocket = new ServerSocket(0, 
                                          Constants.SERVER_BACKLOG_QUEUE_SIZE);

         // If no connections are made in the specified amount of time, the
         //  server socket throws an InterruptedIOException exception
         svrSocket.setSoTimeout(Constants.MULTICAST_PING_WAIT);

         // Send multicast PING to control panel servers
         pingNCPServers(svrSocket.getLocalPort());

         // Collect the list of hosts running control panel servers
         //  that respond back
         Socket socket;
         while (true) {
            try {
               socket = svrSocket.accept();

               // Get the hostname of the responding host and add to list
               new CollectHostnameThread(ntcssHostList, socket);
            }
            catch (InterruptedIOException e) {   // Timeout occurred
               svrSocket.close();
               break;
            }
         }
      }
      catch (IOException e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "collectHostsInfo", e.toString());
      }

      return ntcssHostList;
   }

   // Sends a ping message to the multicast group
   private void pingNCPServers(int listeningPort)
   {
      try {
         // Setup communication to the multicast group
         MessageWithData msg = new MessageWithData(Constants.MULTICAST_PORT,
                                                   Constants.MULTICAST_IP);
         String delimiter = MessageWithData.dataFieldDelimiter;
         String data;

         // Send the process's hostname to the multicast group
         msg.setHeader(CPMessageHeaders.MULTICAST_PING);
         InetAddress addr = InetAddress.getLocalHost();
         data = addr.getHostAddress() + delimiter + String.valueOf(listeningPort);
         msg.setData(data);
         msg.send();

         // Leave the multicast group
         msg.finished();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "pingNCPServers",
                 e.toString());
      }
   }

   // Returns true if the Ntcss server is up on the specified host
   public boolean isNtcssServerUp(String host)
   {
      if (getServerStatus(host) == NTCSS_SVR_UP)
         return true;
      else
         return false;
   }

   // Finds out if the Ntcss server on the specified host is up or down
   private int getServerStatus(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      int ret = NTCSS_SVR_DOWN;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_SERVER_STATUS);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.DAEMONS_UP))
               ret = NTCSS_SVR_UP;
            else if (msg.headerEquals(CPMessageHeaders.DAEMONS_DOWN))
               ret = NTCSS_SVR_DOWN;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Starts up the Ntcss server on the specified host
   public boolean startSingleServer(String host, String parameters)
   {
      boolean ret = false;
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);

      // Set up the message
      msg.setHeader(CPMessageHeaders.START_SERVER);
      msg.setData(parameters);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Sends the given message to the selected hosts running control panel servers
   //  and waits for them to respond back
   private boolean distributeToMultipleHosts(String[] selectedHosts,
                                             String message, String data)
   {
      Counter counter = new Counter(0);

      // Start up a thread for each host that will send the message and wait
      //  for the response
      for (int i = 0; i < selectedHosts.length; i++)
         new InfoThread(selectedHosts[i], message,
                        data, counter);

      // Wait for all the threads to finish
      while (counter.getValue() > 0) {}

      return true;
   }

   // Start the Ntcss server on selected hosts
   public boolean startServers(String[] selectedHosts, String parameters)
   {
      if (selectedHosts.length == 1)
         return startSingleServer(selectedHosts[0], parameters);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.START_SERVER,
                                          parameters);
   }

   // Halts the Ntcss server on the specified host
   public boolean haltSingleServer(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set the message header
      msg.setHeader(CPMessageHeaders.HALT_SERVER);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Halt the Ntcss server on selected hosts
   public boolean haltServers(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return haltSingleServer(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.HALT_SERVER, null);
   }

   // Returns true if the specified host is in debug mode
   public boolean isDebugOn(String host)
   {
      if (getServerDebugState(host) == DEBUG_ON)
         return true;
      else
         return false;
   }

   // Checks whether the Ntcss server on the specified host is in debug
   //  mode or not
   public int getServerDebugState(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      int ret = DEBUG_OFF;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_SERVER_DEBUG_STATE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.DEBUG_ON))
               ret = DEBUG_ON;
            else if (msg.headerEquals(CPMessageHeaders.DEBUG_OFF))
               ret = DEBUG_OFF;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Turns debugging on for the specified host
   public boolean turnSingleDebuggingOn(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message information
      msg.setHeader(CPMessageHeaders.TURN_DEBUG_ON);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Turn debugging on for the Ntcss server on selected hosts
   public boolean turnDebuggingOn(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return turnSingleDebuggingOn(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.TURN_DEBUG_ON, null);
   }

   // Turns debugging off for the specified host
   public boolean turnSingleDebuggingOff(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message information
      msg.setHeader(CPMessageHeaders.TURN_DEBUG_OFF);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Turn debugging off for the Ntcss server on selected hosts
   public boolean turnDebuggingOff(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return turnSingleDebuggingOff(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.TURN_DEBUG_OFF, null);
   }

   // Returns a string which signifies the amount of time the host has been up
   public String getHostUpTime(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      String upTime = "Error";

      // Set up the message information
      msg.setHeader(CPMessageHeaders.GET_HOST_UP_TIME);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               upTime = msg.getData();
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return upTime;
   }

   // Returns a string which signifies the amount of time the Ntcss server has
   //  been up on the specified host
   public String getNtcssUpTime(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      String upTime = "Error";

      // Set up the message information
      msg.setHeader(CPMessageHeaders.GET_NTCSS_UP_TIME);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               upTime = msg.getData();
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return upTime;
   }

   // Returs true if the Ntcss server on the specified host is awake
   public boolean isServerAwake(String host)
   {
      if (getServerState(host) == SVR_AWAKE)
         return true;
      else
         return false;
   }

   // Finds out if the Ntcss server on the specified host is idled or awake
   private int getServerState(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      int ret = SVR_IDLE;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_SERVER_STATE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.DAEMONS_AWAKE))
               ret = SVR_AWAKE;
            else if (msg.headerEquals(CPMessageHeaders.DAEMONS_IDLE))
               ret = SVR_IDLE;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Retrieves the type of NTCSS server on the specified host
   public int getServerType(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      int ret = Constants.UNKNOWN_SVR_TYPE;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_SERVER_TYPE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.MASTER_SVR))
               ret = Constants.MASTER_SVR;
            else if (msg.headerEquals(CPMessageHeaders.AUTH_SVR))
               ret = Constants.AUTH_SVR;
            else if (msg.headerEquals(CPMessageHeaders.APP_SVR))
               ret = Constants.APP_SVR;
            else if (msg.headerEquals(CPMessageHeaders.UNKNOWN_SVR_TYPE))
               ret = Constants.UNKNOWN_SVR_TYPE;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Awakens the Ntcss server on the specified host
   public boolean awakenSingleServer(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message information
      msg.setHeader(CPMessageHeaders.AWAKEN_SERVER);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Awaken the Ntcss server on selected hosts
   public boolean awakenServers(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return awakenSingleServer(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.AWAKEN_SERVER, null);
   }

   // Idles the Ntcss server on the specified host
   public boolean idleSingleServer(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message information
      msg.setHeader(CPMessageHeaders.IDLE_SERVER);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Idle the Ntcss server on the selected hosts
   public boolean idleServers(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return idleSingleServer(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.IDLE_SERVER, null);
   }

   // Returns true if Ntcss logins are allowed on the specified host
   public boolean areLoginsAllowed(String host)
   {
      if (getLoginState(host) == LOGINS_ALLOWED)
         return true;
      else
         return false;
   }

   // Checks to see if logins are allowed or not on the server
   private int getLoginState(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      int ret = LOGINS_BLOCKED;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_LOGIN_STATE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.LOGINS_ALLOWED))
               ret = LOGINS_ALLOWED;
            else if (msg.headerEquals(CPMessageHeaders.LOGINS_DISALLOWED))
               ret = LOGINS_BLOCKED;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Enables Ntcss logins through the specified host
   public boolean enableSingleServerLogins(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.ENABLE_LOGINS);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Enables Ntcss logins on selected hosts running Ntcss servers
   public boolean enableServerLogins(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return enableSingleServerLogins(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.ENABLE_LOGINS, null);
   }

   // Disables Ntcss logins through the specified host
   public boolean disableSingleServerLogins(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ret = false;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.DISABLE_LOGINS);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               ret = true;
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ret;
   }

   // Disable Ntcss logins on selected hosts
   public boolean disableServerLogins(String[] selectedHosts)
   {
      if (selectedHosts.length == 1)
         return disableSingleServerLogins(selectedHosts[0]);
      else
         return distributeToMultipleHosts(selectedHosts,
                                          CPMessageHeaders.DISABLE_LOGINS, null);
   }

   // Returns the number of Ntcss users logged in on the specified host
   public String getNumCurrentUsers(String host)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      String number = "Error";

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_NUM_CURRENT_USERS);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
               number = msg.getData();
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return number;
   }

   // Tells the control panel server running on the given host to create the
   //  status INI file. Then the file is FTPed to the control panel host.
   public boolean getStatusINIFile(String host, String localFilename)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ok = false;
      FTP ftp;
      String remoteFilename;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.CREATE_STATUS_INI_FILE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL)) {
               remoteFilename = msg.getData();

               // Attempt to get the newly created INI file
               try {
                  ftp = new FTP();
                  if (ftp.GetFile(host, remoteFilename, localFilename, true) == true)
                     ok = true;
               }
               catch (Exception e) {
                  Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                          "getStatusINIFile", e.toString());
               }
            }
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ok;
   }

   // Gets the control panel server running on the given host to create the
   //  login history INI file. Then the file is FTPed to the control panel host.
   public boolean getLoginHistoryINIFile(String host,
                                         String sendFilename,
                                         String returnFilename)
   {
      return getINIFile(host, sendFilename, returnFilename,
                        CPMessageHeaders.CREATE_LOGIN_HISTORY_INI_FILE);
   }

   // Gets the control panel server running on the given host to create the
   //  system log INI file. Then the file is FTPed to the control panel host.
   public boolean getSystemLogINIFile(String host,
                                      String sendFilename,
                                      String returnFilename)
   {
      return getINIFile(host, sendFilename, returnFilename,
                        CPMessageHeaders.CREATE_SYSTEM_LOG_INI_FILE);
   }

   // Gets the control panel server running on the given host to create the
   //  specified INI file. Then the file is FTPed to the control panel host.
   private boolean getINIFile(String host, String sendFilename,
                              String returnFilename, String headerInfo)
   {
      MessageWithData msg;
      boolean ok = false;
      FTP ftp;
      String remoteDirectoryname = getINIDirectory(host);
      String sendRemoteFilename;
      String returnRemoteFilename;

      sendRemoteFilename = remoteDirectoryname + sendFilename;
      returnRemoteFilename = remoteDirectoryname + returnFilename;

      // Attempt to ftp the param file for unix_ini
      try {
         ftp = new FTP();
         if (ftp.PutFile(host, sendRemoteFilename, sendFilename, true, false) == true)
            ok = true;
         else {
            Log.log(Log.CLT, Log.ERR, this.getClass().getName(), "getINIFile",
                    "PutFile failed");
            return false;
         }
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "getINIFile",
                 e.toString());
         return false;
      }

      // Set up the message
      msg = new MessageWithData(host, Constants.CP_SERVER_PORT);
      msg.setHeader(headerInfo);
      msg.setData(sendRemoteFilename);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL)) {

               // Attempt to get the newly created INI file
               try {
                  ftp = new FTP();
                  if (ftp.GetFile(host, returnRemoteFilename, returnFilename, true) == true)
                     ok = true;
                  else
                     Log.log(Log.CLT, Log.ERR, this.getClass().getName(),
                             "getINIFile", "GetFile failed");
               }
               catch (Exception e) {
                  Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                          "getINIFile", e.toString());
               }
            }
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ok;
   }

   // Tells the control panel server running on the given host to create the
   //  common INI file. Then the file is FTPed to the control panel host.
   public boolean getCommonDataINIFile(String host, String localFilename)
   {
      MessageWithData msg = new MessageWithData(host,
                                                Constants.CP_SERVER_PORT);
      boolean ok = false;
      FTP ftp;
      String remoteFilename;

      // Set up the message header
      msg.setHeader(CPMessageHeaders.CREATE_COMMON_DATA_INI_FILE);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true) {

            // Check the response
            if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL)) {
               remoteFilename = msg.getData();

               // Attempt to get the newly created INI file
               try {
                  ftp = new FTP();
                  if (ftp.GetFile(host, remoteFilename, localFilename, true) == true)
                     ok = true;
               }
               catch (Exception e) {
                  Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                          "getCommonDataINIFile", e.toString());
               }
            }
         }
      }

      // Clean up the message mechanism
      msg.finished();

      return ok;
   }

   // Attempts to add/modify the Common Data values with the given
   //  tag/value pair on the specified host.
   public boolean setCommonData(String host, String tag, String value)
   {
      MessageWithData msg;
      boolean result = false;

      try {
         // Set up the message to send
         msg = new MessageWithData(host, Constants.CP_SERVER_PORT);
         msg.setHeader(CPMessageHeaders.SET_COMMON_DATA);
         msg.setData(tag + MessageWithData.dataFieldDelimiter + value);

         // Try to send the message which contains the tag/value pair
         if (msg.send() == true) {

            // Wait for the response
            if (msg.receive() == true)
               result = true;
         }

         msg.finished();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "setCommonData",
                 e.toString());
      }

      return result;
   }

   // Checks to see if a connection can be made to the control panel server on
   //  the given host
   public boolean isCPServerUp(String host)
   {
      MessageWithData msg;
      boolean result = false;

      try {
         msg = new MessageWithData(host, Constants.CP_SERVER_PORT);
         msg.setHeader(CPMessageHeaders.PING);

         // Try to send a ping and wait for a response
         if (msg.send() == true)
            if (msg.receive() == true)
               if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
                  result = true;

         msg.finished();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(), "isCPServerUp",
                 e.toString());
      }

      return result;
   }

   // Retrieves, from the specified host, the directory where INI files reside
   //  on that particular host. This directory is what is returned from this
   //  function.
   public String getINIDirectory(String host)
   {
      MessageWithData msg;
      String directory = "tmp";

      msg = new MessageWithData(host, Constants.CP_SERVER_PORT);

      // Set up the message header
      msg.setHeader(CPMessageHeaders.GET_INI_DIRECTORY);

      // Send the message
      if (msg.send() == true) {

         // Wait for the response
         if (msg.receive() == true)
            directory = msg.getData();
      }

      // Clean up the message mechanism
      msg.finished();

      return directory;
   }

   // Sends necessary information to the desired host so that it can
   //  reconfigure itself to be the NTCSS master server
   public boolean makeNTCSSMasterSvr(String oldHostname, String newHostname,
                                     String IPaddr, String domainname,
                                     String[][] hostData)
   {
      MessageWithData msg;
      String data;
      String delimiter = MessageWithData.dataFieldDelimiter;
      boolean successful = false;

      try {
         msg = new MessageWithData(oldHostname,
                                   Constants.CP_SERVER_PORT);

         // Set up the message header
         msg.setHeader(CPMessageHeaders.MAKE_NTCSS_MASTER);

         // Set up the data part of the messsage.
         data = newHostname + delimiter +
                IPaddr      + delimiter +
                domainname  + delimiter +
                String.valueOf(hostData.length) + delimiter;
         for (int i = 0; i < hostData.length; i++) {
            data += hostData[i][0] + delimiter +    // Old hostname
                    hostData[i][1] + delimiter +    // New hostname
                    hostData[i][2] + delimiter +    // IP address
                    hostData[i][3] + delimiter;     // NTCSS server type
         }
         msg.setData(data);

         // Send the message
         if (msg.send() == true) {

            // Wait for the response
            if (msg.receive() == true) {

               // Check the response
               if (msg.headerEquals(CPMessageHeaders.SUCCESSFUL))
                  successful = true;
            }
         }

         // Clean up the message mechanism
         msg.finished();
      }
      catch (Exception e) {
         Log.log(Log.CLT, Log.EXCP, this.getClass().getName(),
                 "makeNTCSSMasterSvr", e.toString());
      }

      return successful;
   }

   // Reconfigures each of host in the list (except the master server) to be
   //  either an authentication or application server
   public boolean reconfigureSlaves(String[][] hostData, String domainname,
                                    int masterIndex)
   {
      Counter counter = new Counter(0);
      String delimiter = MessageWithData.dataFieldDelimiter;
      String data;

      // Start up a thread for each host that will send the message and wait
      //  for the response
      for (int i = 0; i < hostData.length; i++) {
         if (i != masterIndex) {
            // Host's info
            data = hostData[i][0] + delimiter +    // Old hostname
                   hostData[i][1] + delimiter +    // New hostname
                   hostData[i][2] + delimiter +    // IP address
                   domainname     + delimiter;     // Domain name

            // Master's info
            data += hostData[masterIndex][0] + delimiter +  // Old hostname
                    hostData[masterIndex][1] + delimiter +  // New hostname
                    hostData[masterIndex][2] + delimiter;   // IP address

            if (Integer.parseInt(hostData[i][3]) == Constants.AUTH_SVR)
               new InfoThread(hostData[i][0], CPMessageHeaders.MAKE_NTCSS_AUTH,
                              data, counter);
            else
               new InfoThread(hostData[i][0], CPMessageHeaders.MAKE_NTCSS_APP,
                              data, counter);
         }
      }

      // Wait for all the threads to finish
      while (counter.getValue() > 0) {}

      return true;
   }

   // Stops and then restarts the NTCSS daemons on the specified host
   public void recycleNtcssDaemons(String host)
   {
      // Halt the daemons
      if (haltSingleServer(host) == false) {
         Log.log(Log.CLT, Log.ERR, this.getClass().getName(),
                 "recycleNtcssDaemons", "Error stopping daemons on " + host);
         return;
      }

      // Restart the daemons
      if (startSingleServer(host, null) == false)
         Log.log(Log.CLT, Log.ERR, this.getClass().getName(),
                 "recycleNtcssDaemons", "Error re-starting daemons on " + host);
   }
}

// Used to read a response from a host which indicates it's info and then adds
//  it to a running list of hostnames
class CollectHostnameThread extends Thread
{
   private MessageWithData msg;
   private HostInfoList hostInfoList;

   public CollectHostnameThread(HostInfoList list, Socket socket)
   {
      msg = new MessageWithData(socket);
      hostInfoList = list;

      // Calls run()
      start();
   }

   public void run()
   {
      String data;

      // Read the data from the socket
      if (msg.receive() == true) {

         // Get the data from the message
         data = msg.getData();

         // Add the data to the host info list
         addToList(data);
      }

      // Clean up message mechanism
      msg.finished();
   }

   // Parses through the data retrieved from the control panel server and
   //  and the information to the host info list
   private void addToList(String data)
   {
      String hostname;
      String IPaddress;
      int serverType;
      String status;
      int serverStatus;
      StringTokenizer st;

      st = new StringTokenizer(data, MessageWithData.dataFieldDelimiter);

      // Get the host's name
      hostname = (st.nextToken()).trim();

      // Get the host's IP address
      IPaddress = (st.nextToken()).trim();

      // Get the host's NTCSS server type
      serverType = Integer.parseInt(st.nextToken());

      // Get the status of the NTCSS server
      status = st.nextToken();
      if (status.indexOf("0") != -1)
         serverStatus = NtcssServersInfo.NTCSS_SVR_DOWN;
      else
         serverStatus = NtcssServersInfo.NTCSS_SVR_UP;

      // Add the host's info to the list of hosts
      hostInfoList.addEntry(hostname, IPaddress, serverType, serverStatus);
   }
}

// Sends the given message to the specified host and waits for a response
class InfoThread extends Thread
{
   private String host;
   private String message;
   private String data;
   private Counter counter;

   public InfoThread(String hst, String msg, String dt, Counter cntr)
   {
      host = hst;
      message = msg;
      data = dt;
      counter = cntr;

      counter.increment();

      setPriority(MAX_PRIORITY);

      start();
   }

   public void run()
   {
      MessageWithData msg = new MessageWithData(host,
                                             Constants.CP_SERVER_PORT);

      // Set up the message
      msg.setHeader(message);
      msg.setData(data);

      // Send the message
      if (msg.send() == true)
         // Wait for the response
         msg.receive();

      // Clean up the message mechanism
      msg.finished();

      counter.decrement();
   }
}


