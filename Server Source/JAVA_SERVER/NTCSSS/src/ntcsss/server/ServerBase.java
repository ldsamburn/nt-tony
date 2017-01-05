/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.net.Socket;
import java.net.InetAddress;

import java.io.BufferedOutputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.InterruptedIOException;
import java.io.File;

import java.util.Arrays;

import ntcsss.libs.Constants;
import ntcsss.libs.ErrorCodes;
import ntcsss.libs.StringLib;
import ntcsss.libs.DataLib;
import ntcsss.libs.UserLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.NDSLib;

import ntcsss.log.Log;

import ntcsss.util.IntegerBuffer;

public class ServerBase
{
   private Socket socket;
   private String messageData;
   private BufferedOutputStream outputStream;
   private BufferedInputStream inputStream;
   private ServerState serverState;

   public ServerBase(String messageData, Socket socket, ServerState serverState)
   {
      try {
         this.socket = socket;
         this.messageData = messageData;
         this.serverState = serverState;

         // Create a read/write stream from/to the socket
         outputStream = new BufferedOutputStream(this.socket.getOutputStream());
         inputStream = new BufferedInputStream(this.socket.getInputStream());
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "ServerBase", exception.toString());                            
      }
   }
   
   /**
    * Closes any resources that are members of this class 
    */
   public void cleanup()
   {
      try {
         outputStream.close();
         inputStream.close();
         
         // NOTE: the socket instance will be closed in GenericServer
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "cleanup", exception.toString());
      }
   }

   /**
    * This method must be overridden by any class that derives from this class
    */
   public static ServerDescription getServerDescription()
   {
      return null;
   }
   
   /**
    * Pads the given header out to the proper size, appends the message
    *  to the end of it, and sends the string
    */
   protected boolean sendFormattedServerMsg(String header, String message)
   {
      String newMessage;

      try {

         // Get the formatted version of this message
         newMessage = formatServerMsg(header, message);
         if (newMessage == null)
            return false;

         // Send the string
         return sendServerMsg(newMessage);
      }
      catch (Exception exception) {
         Log.debug("ServerBase", "sendFormattedServerMsg", exception.toString());
         return false;
      }
   }

   /**
    * Pads the given header out to CMD_LEN and appends the given message
    *  to the end of it
    */
   protected String formatServerMsg(String header, String message)
   {      
      return StringLib.formatMessage(header, message);      
   }

   /**
    * Sends the given message to the listener on the
    *  other end of the socket connection
    */
   protected boolean sendServerMsg(byte message[], int length)
   {
      try {
         // Send given message
         outputStream.write(message, 0, length);
// The trailing zero can't be generically sent for all messages because,
//  in particular, it messes up the loggin process
//         outputStream.write(0);  // Send a terminating null character
         outputStream.flush();

         return true;
      }
      catch (Exception exception) {
         Log.debug("ServerBase", "sendServerMsg", exception.toString());
         return false;
      }
   }

   /**
    * A convienice method for sending the given message to the listener on the
    *  other end of the socket connection
    */
   protected boolean sendServerMsg(String message)
   {
      try {
         return sendServerMsg(message.getBytes(), message.length());
      }
      catch (Exception exception) {
         Log.debug("ServerBase", "sendServerMsg", exception.toString());
         return false;
      }
   }

   /**
    * The message handler for the SERVERSTATUS message
    */
   public void serverStatus()
   {
      int dataItemLengths[];
      String dataItems[];
      int bundleLength;
      StringBuffer bundleString;
      Object items[];
      boolean noSendError;
                    
      try {
         
         // Get the desired action
         dataItemLengths = new int[1];
         dataItems = new String[1];
         dataItemLengths[0] = 1;
         if (parseData(1, dataItemLengths, dataItems) == false) {
            Log.error("ServerBase", "serverStatus", 
                      "Could not parse incoming message");
            return;
         } 
         
         // Determine the course of action to take
         switch (StringLib.parseInt(dataItems[0], -1)) {
            
            case Constants.SERVER_STATE_AWAKE:
               Log.info("ServerBase", "serverStatus", "Awaking server");
               serverState.awaken();
               break;
               
            case Constants.SERVER_STATE_IDLE:
               Log.info("ServerBase", "serverStatus", "Idling server");
               serverState.idle();
               break;
               
            case Constants.SERVER_STATE_DIE:
               Log.info("ServerBase", "serverStatus", "Server ordered to halt");
               serverState.die();
               break;
               
            case Constants.SERVER_STATE_ONDEBUG:
               Log.turnDebugOn();
               break;
               
            case Constants.SERVER_STATE_OFFDEBUG:
               Log.turnDebugOff();
               break;
               
            case Constants.SERVER_STATE_ONLOCK:
               Log.info("ServerBase", "serverStatus", "Blocking NTCSS logins");
               serverState.turnOnLoginLock();
               break;
               
            case Constants.SERVER_STATE_OFFLOCK:
               Log.info("ServerBase", "serverStatus", "Allowing NTCSS logins");
               serverState.turnOffLoginLock();
               break;
               
            case Constants.SERVER_STATE_CHECK:
                      
               // Bundle the information about the server's state
               bundleString = new StringBuffer();
               items = new Object[3];
               items[0] = new Integer(serverState.getState());
               items[1] = new Integer(serverState.isLoginLockOn() ? 
                                      Constants.SERVER_STATE_ONLOCK : 
                                      Constants.SERVER_STATE_OFFLOCK);
               items[2] = new Integer(Log.isDebugOn() ? 
                                      Constants.SERVER_STATE_ONDEBUG : 
                                      Constants.SERVER_STATE_OFFDEBUG);
               bundleLength = BundleLib.bundleData(bundleString, "III", items);
             
               // Send the reply
               if (bundleLength < 0) 
                  noSendError = sendErrorMessage("NOSERVERSTATUS", bundleLength);
               else
                  noSendError = sendFormattedServerMsg("GOODSERVERSTATUS", 
                                                     bundleString.toString());
               
               if (noSendError == false)
                  Log.error("ServerBase", "serverStatus", 
                            "Could not send message!");
                  
               break;
               
            default:
               Log.error("ServerBase", "serverStatus", "Unknown request");                            
               break;
         }
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "serverStatus", exception.toString());                            
      }
   }   
   
   /**
    * Returns the entire payload off the given message.
    */
   protected String GetMessagePayloadString()
   {
      return messageData;
   }

   /**
    * Reads the items off the message data and places them in the given array
    */
   protected boolean parseData(int itemLengths[], String items[])
   {      
      return parseData(messageData, items.length, itemLengths, items);
   }
   
   /**
    * Reads the items off the message data and places them in the given array.
    *  The "numItems" parameter allows for the use of less items in the
    *  arrays than there actually are.
    */
   protected boolean parseData(int numItems, int itemLengths[], String items[])
   {      
      return parseData(messageData, numItems, itemLengths, items);
   }
   
   /**
    * Reads the items off the given message and places them in the given array
    */
   protected boolean parseData(String msg, int itemLengths[], String items[])
   {
      return parseData(msg, items.length, itemLengths, items);
   }
   
   /**
    * Reads the items off the given message and places them in the given array
    *  The "numItems" parameter allows for the use of less items in the
    *  arrays than there actually are.
    */
   protected boolean parseData(String msg, int numItems, int itemLengths[], 
                               String items[])
   {
      int currentPosition;
      int assumedTotalLength;

      try {

         // Make sure the data fields are the right size
         if ((itemLengths.length != items.length) ||
             (numItems > itemLengths.length)) {
            Log.error("ServerBase", "parseData", "Field sizes do not match");
            return false;
         }

         // Determine the assumed total length
         assumedTotalLength = 0;
         for (int i = 0; i < numItems; i++)
            assumedTotalLength += itemLengths[i];

         // Check to make sure the message data is at least the assumed length
         if (msg.length() < assumedTotalLength) {
            Log.error("ServerBase", "parseData", "Length of data is too small");
            return false;
         }

         // Loop through the message data and store off into the array the
         //  different fields
         currentPosition = 0;
         for (int i = 0; i < numItems; i++) {
            items[i] = (msg.substring(currentPosition,
                                   currentPosition + itemLengths[i])).trim();
            currentPosition += itemLengths[i];
         }

         return true;
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "parseData", exception.toString());                            
         return false;
      }
   }
   
   /**
    * Returns true if this server is idled
    */
   protected boolean isServerIdled()
   {
      return serverState.isIdle();
   }
   
   /**
    * Returns true if logins are allowed
    */
   protected boolean loginsAllowed()
   {
      return !serverState.isLoginLockOn();
   }
   
   /**
    * Returns the IP address of the client
    */
   protected String getClientAddress()
   {
      InetAddress address;
      
      try {
         address = socket.getInetAddress();
         
         return address.getHostAddress();
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "getClientAddress", exception.toString());
      }
      
      return null;
   }
   
   /**
    * Returns the hostname of the client
    */
   protected String getClientHostname()
   {
      InetAddress address;
      
      try {
         address = socket.getInetAddress();
         
         return address.getHostName();
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "getClientHostname", exception.toString());
      }
      
      return null;
   }
   
   /**
    * If the local host is the master or an authentication server, makes sure
    *  the given user is in the local database and that the job id (hidden at
    *  the end of the token string) is in the active_jobs table. Otherwise,
    *  it just calls isUserValid.
    */
   protected int doServerValidate(String loginName, String token, 
                                  String clientAddress)
   {      
      int nValidUser;
      String jobId;      
      int nValidProcess, ret;
      IntegerBuffer uid = new IntegerBuffer();
                  
      try {
         
         // Determine the server type of the local host
         switch (DataLib.getHostType(StringLib.getHostname())) {
    
            case Constants.NTCSS_MASTER:
            case Constants.NTCSS_AUTH_SERVER:
            
               // Check to see if the given user is in the local "users" table
               nValidUser = UserLib.nIsUserInDB(loginName, null);

               // Check to see if the job ID is in the active_jobs table
               jobId = (token.length() > Constants.PRETOKEN_SIZE) ?
                         token.substring(Constants.PRETOKEN_SIZE):null;                      
               // For now we will not validate the process, so commented out call 
               //  to validateServerProcessId and manually set nValidProcess to 1. 
               //  nValidProcess = validateServerProcessId(jobId);            
               nValidProcess = 1;            
            
               if ((nValidUser == 1) && (nValidProcess == 1))        
                  return 0;        
               else {        
                  Log.error("ServerBase", "doServerValidate", 
                            "Valid user(" + loginName + ") == " + nValidUser + 
                            ", valid process(" + jobId + ") == " + nValidProcess);
                  return ErrorCodes.ERROR_USER_NOT_VALID;
               }             
      
            default:
               ret = isUserValid(loginName, token, uid);
               if (ret < 0)        
                  Log.error("ServerBase", "doServerValidate", 
                            "Unable to validate user <" + loginName + ">!");
         
               return ret;            

         } // END switch                   
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "doServerValidate", exception.toString());
         return ErrorCodes.ERROR_USER_NOT_VALID;
      }
   }
   
   /**
    * Checks to see if the given user is in the local current_users table.
    *  If they are not, then a message is sent to either the master or 
    *  the user's authentication server to see if they are in the current
    *  users table on that host. "0" is returned if they are logged in
    *  somewhere (i.e. they're "valid")
    */
   protected int isUserValid(String loginname, String token, IntegerBuffer uid)
   {
      String fnct_name = "isUserValid";
      int nReturn;
      String strHostNameToCheckUserValid = null;
      String strThisHostName;
      StringBuffer hostName = new StringBuffer();
      StringBuffer strHostNameMaster = new StringBuffer();
      int ret, i;
      
      try {
         Log.debug("ServerBase", fnct_name, "Entering for user (" + 
                   loginname + ")");
         
         strThisHostName = StringLib.getHostname();
         switch (DataLib.getHostType(strThisHostName)) {
            
            case Constants.NTCSS_MASTER:                                 
               
               Log.debug("ServerBase", fnct_name, 
                         "Determined this is a Master server.");               

               // Check to see if the given user with the given token is in the 
               //  local current_users table
               nReturn = UserLib.isUserValidLocally(loginname, token, uid);               
               if (nReturn == 0) {
        
                  Log.debug("ServerBase", fnct_name, 
                           "User(" + loginname + ") is logged in and valid" +
                           " on this machine");
                  return nReturn;
               }
               
               // If user is not valid on this machine, then let this
               //  function continue and check the user's auth server
               //  to see if user is logged in there   
               
               // Get the given user's authentication server               
               if ((strHostNameToCheckUserValid = 
                                  NDSLib.getAuthServer(loginname)) == null) {
                  Log.error("ServerBase", fnct_name, "getAuthServer(" + 
                            loginname + ") failed!");
                  return ErrorCodes.ERROR_CANT_ACCESS_LOGIN;
               } 
 
               Log.debug("ServerBase", fnct_name, "Determined auth server (" + 
                         strHostNameToCheckUserValid + ") for user (" + 
                         loginname + ").");
 
               // If this user's auth server is also the master, we don't want
               //  to send a message to ourselves to check for this user again                             
               if (strHostNameToCheckUserValid.equals(strThisHostName) == true)
                  return nReturn;
               
               break;             
               
            case Constants.NTCSS_AUTH_SERVER:
               
               Log.debug("ServerBase", fnct_name, 
                         "Determined this is an Auth server.");
               
               // If user is not logged in on this machine then check
               //  user's auth server 
      
               Log.info("ServerBase", fnct_name, 
                        "Auth srv: before call to isUserValidLocally(" + loginname + 
                        ")..");
               nReturn = UserLib.isUserValidLocally(loginname, token, uid);
               if (nReturn == 0) {
        
                  Log.debug("ServerBase", fnct_name, "User(" + loginname + 
                            ") is logged in and valid on this machine");
                  return nReturn;
               }
                              
               if ((strHostNameToCheckUserValid = 
                                   NDSLib.getAuthServer(loginname)) == null) { 
                  Log.error("ServerBase", fnct_name, "getAuthServer(" + 
                            loginname + ") failed!");
                  return ErrorCodes.ERROR_CANT_ACCESS_LOGIN;
               }

               Log.debug("ServerBase", fnct_name, "Determined auth server (" + 
                         strHostNameToCheckUserValid + ") for user (" + 
                         loginname + ").");               

               // If this machine is the auth server, then check
               //   to see if the user is logged into the Master. 
               if (strHostNameToCheckUserValid.equals(strThisHostName) == true) {        
                  DataLib.getMasterServer(hostName); 
                  strHostNameToCheckUserValid = hostName.toString(); 
               }
             
               break;
               
            case Constants.NTCSS_APP_SERVER:
               i = DataLib.getMasterServer(hostName);
               if (i < 0)
                  return i;
               
               strHostNameToCheckUserValid = hostName.toString();
               break;
      
            default:
                return ErrorCodes.ERROR_CANT_ACCESS_LOGIN;
         }            
         
         Log.debug("ServerBase", fnct_name, 
                   "Before send VALIDATEUSER message to " + 
                   strHostNameToCheckUserValid);      
         
         // Try to validate on the determined host
         ret = sendUserValid(strHostNameToCheckUserValid, loginname, token);
         if (ret < 0) {     
            
            // Try to validate on the master
            DataLib.getMasterServer(strHostNameMaster);            
            Log.info("ServerBase", fnct_name, "Unable to validate user through "
                     + strHostNameToCheckUserValid + " trying master " + 
                     strHostNameMaster);
            ret = sendUserValid(strHostNameMaster.toString(), loginname, token);
            if (ret < 0)         
               Log.error("ServerBase", fnct_name, 
                         "Unable to validate user through the master, " + 
                         strHostNameMaster.toString() + "!");         
         }

         Log.debug("ServerBase", fnct_name, 
                   "After read response from VALIDATEUSER message.");
         Log.debug("ServerBase", fnct_name, "Returning " + ret);

         return ret;
      }
      catch (Exception exception) {      
         Log.excp("ServerBase", fnct_name, exception.toString());
         return ErrorCodes.ERROR_USER_NOT_VALID;
      }
   }
   
   /**
    * Sends the given file
    */
   protected boolean sendServerMsgFile(String header, String messageData,
                                       String filename, int fileSize)
   {            
      byte sendBuffer[];
      FileInputStream fileStream;
      int numBytes;
      String newMessage;
      File fileDesc;
      
      try {         
         
         // Get the formatted version of the message
         newMessage = formatServerMsg(header, messageData);
         if (newMessage == null) {
            Log.error("ServerBase", "sendServerMsgFile", 
                      "Error creating opening message!");
            return false;
         }
         
         // Send the opening message
         outputStream.write(newMessage.getBytes(), 0, newMessage.length());             
      
         // Check to make sure the given file exists
         fileDesc = new File(filename);
         if (fileDesc.exists() == true) {   

            // Open the given file and send the contents
            fileStream = new FileInputStream(filename);
            sendBuffer = new byte[Constants.MAX_MSG_LEN];
         
            do {
               // Read the data out of the file
               numBytes = fileStream.read(sendBuffer);
            
               // If any data was read, send it out over the socket
               if (numBytes != -1)
                  outputStream.write(sendBuffer, 0, numBytes);
            
            } while (numBytes != -1);

            // Close the file
            fileStream.close();
         } 
         
         // Send null byte
         outputStream.write(0);  
         outputStream.flush();
         
         return true;
      }
      catch (Exception exception) {
         Log.debug("ServerBase", "sendServerMsgFile", exception.toString());
         
         return false;
      }
   }
   
   /**
    * Listens on the socket for another message from the client
    */
   protected int getServerMsg(byte buffer[], int bufferLength, boolean useTimer)
   {      
      int bytesRead;
      
      try {
         
         // Set the timeout for the socket if the "user timer" flag is true
         if (useTimer == true)
            socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);
         
         // Read the data from the socket
         bytesRead = SocketLib.readStream(inputStream, buffer, bufferLength);
         
         if (bytesRead == -2) {
            Log.error("ServerBase", "getServerMsg", "Received timeout during read");
            return 0;
         }         

         return bytesRead;
      }
      catch (Exception exception) {
         Log.debug("ServerBase", "getServerMsg", exception.toString());         
         return 0;
      }
   }
   
   /**
    * Determins the text of the given message code, combines this
    *  with the header, and sends the string 
    */
   protected boolean sendErrorMessage(String header, int messageCode)
   {     
      String message;
      String msgLengthStr;
      
      try {
         message = ErrorCodes.getErrorMessage(messageCode);  
         msgLengthStr = StringLib.valueOf(message.length(), 5);
         return sendFormattedServerMsg(header, msgLengthStr + message + "\0");
      }
      catch (Exception exception) {
         Log.excp("ServerBase", "sendErrorMessage", exception.toString()); 
         return false;
      }
   }      
   
   /**
    * Sends the given bulletin board message to MSG_SVR port on the given
    *  host
    */
   protected int postSystemMessage(String hostname, String loginname,
                                   String appname, String sys_msg,
                                   int type)
   {
      String fnct_name = "postSystemMessage";
      String message;
      
      try {
         // Create the message to send
         message = formatServerMsg("SENDSYSBBMSG",
           StringLib.padString(loginname, Constants.MAX_LOGIN_NAME_LEN, ' ') +
           StringLib.padString(appname, Constants.MAX_PROGROUP_TITLE_LEN, ' ') +
           StringLib.valueOf(type, 3) +
           StringLib.padString(sys_msg, Constants.MAX_SYSTEM_MSG_LEN, ' '));
                                    
         // Send the message
         if (SocketLib.sendFireForgetMessage(hostname, Constants.MSG_SVR_PORT, 
                                             message) == false) {
            Log.error("ServerBase", fnct_name, "Error sending message");
            return ErrorCodes.ERROR_CANT_COMM_WITH_MASTER;
         }
                
         return 0;
      }
      catch (Exception exception) {
         Log.excp("ServerBase", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_COMM_WITH_MASTER;
      }
   }
   
   /**
    * This method is called by GenericServer when the server is shutdown
    *  and all of its worker threads have finished processing. This method
    *  should be overriden by any subclasses that want this capability.
    */
   public static void runFinalization()
   {
   }
   
   /**
    * Sends a VALIDATEUSER message to the given host to see if the given
    *  user is its in the current_users table
    */
   private int sendUserValid(String strHostNameToCheckUserValid,
                             String strLoginName,
                             String strToken)
   {
      int    msg_size;
      Socket socket;
      int    x;
      String fnct_name = "sendUserValid";
      String msg;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;
      int ret;
      byte buffer[];
      int bytesRead;
      StringBuffer response = new StringBuffer();
      String resultCode;
      
      try {
         
         // Create a socket to the host
         socket = new Socket(strHostNameToCheckUserValid, 
                             Constants.DSK_SVR_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());
  
         // Create the message to send to the host
         msg = formatServerMsg("VALIDATEUSER", 
                               StringLib.padString(strLoginName, 
                                Constants.MAX_LOGIN_NAME_LEN, ' ') +
                               StringLib.padString(strToken, 
                                Constants.MAX_TOKEN_LEN, ' ') + "\0");
         
         // Send the message
         outputStream.write(msg.getBytes(), 0, msg.length());
         outputStream.flush();
            
         // Get the host's response
         buffer = new byte[50];
         socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);         
         do {
            bytesRead = SocketLib.readStream(inputStream,buffer, 50);            
            if (bytesRead > 0) 
               response.append( new String(buffer, 0, bytesRead));                           
         }
         while (bytesRead > 0);                  
         
         outputStream.close();
         inputStream.close();
         socket.close();
         
         // Check to see if a timeout occurred
         if (bytesRead == -2) {
            Log.error("ServerBase", fnct_name, "Received read timeout");            
            return ErrorCodes.ERROR_CANT_TALK_TO_SVR;
         }                 

         Log.debug("ServerBase", fnct_name, 
                   "After read response from VALIDATEUSER message.");

         resultCode = (response.toString()).substring(Constants.CMD_LEN).trim();
         ret = StringLib.parseInt(resultCode, ErrorCodes.ERROR_USER_NOT_VALID);
         if (ret < 0) {
    
           Log.debug("ServerBase", fnct_name, "ret == " + ret);
           return ret;
        }

         return 0;  
      } 
      catch (Exception exception) {
         Log.excp("ServerBase", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_TALK_TO_SVR;
      }
   }
}