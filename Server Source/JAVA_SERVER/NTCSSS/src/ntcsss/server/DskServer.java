/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.net.Socket;

import java.util.StringTokenizer;
import java.util.Vector;
import java.util.Enumeration;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.io.File;
import java.io.FileWriter;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.distmgr.DistributionManager;
import ntcsss.distmgr.DPResults;

import ntcsss.util.IntegerBuffer;

import ntcsss.libs.ErrorCodes;
import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;
import ntcsss.libs.UserLib;
import ntcsss.libs.CryptLib;
import ntcsss.libs.DataLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.NDSLib;

/**
 * The message handlers for the Desktop server
 */
public class DskServer extends ServerBase 
{
   private final String GOODAUTHUSER = "GOODAUTHUSER";
   private final String BADAUTHUSER = "BADAUTHUSER";
   private final String NOUSRINFOAUTHUSER = "NOUSRINROAUTHUSER"; // Mispelled
   
   private final String AUTHUSER_SEP_CHAR_STRING = "\3";

   /** 
    * Constructor
    *  This should ALWAYS call the ServerBase class constructor
    */
   public DskServer(String messageData, Socket socket, ServerState serverState) 
   {
      super(messageData, socket,serverState);
   }

   /**
    * Returns an object that contains information about this server including
    *  a list of message handlers this class contains
    */
   public static ServerDescription getServerDescription()
   {
      Class serverClass;

      try {
          
         // Set the local variable to the class object of this server         
         serverClass = DskServer.class;
           
         MessageHandlerDescription handlers[] = 
           { new MessageHandlerDescription("AUTHUSER", 
                          serverClass.getMethod("authorizeUserLogin", null)),
             new MessageHandlerDescription("GETUSERINFO", 
                          serverClass.getMethod("getUserInfo", null)),
             new MessageHandlerDescription("USERLOGOUT", 
                          serverClass.getMethod("userLogout", null)),
             // VERIFYUSER     not called anymore
             // VERIFYAPP      not called anymore
             // VERIFYAPPREPLY not called anymore
             new MessageHandlerDescription("VALIDATEUSER", 
                          serverClass.getMethod("validateUser", null)),
             // CHECKVERSION   not called anymore
             // REMOTEVALIDATE not called anymore
             new MessageHandlerDescription("GETAUTHSVR", 
                          serverClass.getMethod("getAuthServer", null)),             
             new MessageHandlerDescription("SERVERSTATUS", 
                          serverClass.getMethod("serverStatus", null)) };

         // Return the information concerning this class
         return new ServerDescription("Desktop Server", serverClass, handlers, 
                                      EntityName.DSK_SVR_GRP);
      }
      catch (Exception exception) {
         Log.excp("DskServer", "getServerDescription", exception.toString());
         return null;
      }
   }
   
   /**
    * Attempts to log a user in to this server
    */
   public void getUserInfo()
   {
      int errorCode = 0;      
      String strLoginKey;
      byte buffer[];      
      int dataItemLengths[];
      String dataItems[];
      int recordStateError;
      boolean sendOK;
      String loginname = null;
      String password;
      String ntdomain;
      StringBuffer msg = new StringBuffer();
      StringBuffer token = new StringBuffer();
      StringBuffer group_list = new StringBuffer();
      StringBuffer pStrNtcssDecryptedPassword = new StringBuffer();       
      StringBuffer client_ip;
      StringBuffer uid = new StringBuffer();
      Connection connection = null;
      File nologinsFile = new File(Constants.NOLOGINS_FILE);

      try {
         
         Log.debug("DskServer", "getUserInfo", "Entering...");
         
                  
         // Check to see if this server is idled
         if ((isServerIdled() == true) || (nologinsFile.exists() == true))
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
         else if (loginsAllowed() == false)
            errorCode = ErrorCodes.ERROR_SYSTEM_LOCKED;      
                  
         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DskServer", "getUserInfo",  
                      "\"null\" connection returned");
            return;
         }
                  
         // Send filler characters
         if (sendServerMsg("FillerCharacters   ") == false)
            Log.error("DskServer", "getUserInfo", 
                      "Failed to send server message!");

         // Send a key for the desktop to use to encrypt the user's password 
         //  typed in at the desktop login screen
         strLoginKey = getLoginEncryptionKey();            
         if (sendServerMsg(strLoginKey + "\0") == false)
            Log.error("DskServer", "getUserInfo", 
                      "Could not send login initialization info!");
         
         // Read in twice the size of password because the password is passed 
         //  over encrypted, and the encryption algorithm can expand the 
         //  password up to twice its unencrypted size.           
         buffer = new byte[Constants.MAX_LOGIN_NAME_LEN     + 
                           (2 * Constants.MAX_PASSWORD_LEN) +
                           Constants.MAX_WORKING_DIR_LEN    + 1];         
         if (getServerMsg(buffer, buffer.length, true) == 0) {
            Log.error("DskServer", "getUserInfo", "Error reading client data");
            return;
         }
         
         // Check to see if should proceed
         if (errorCode < 0)
            throw new GoTo();
         
         // Parse the client data
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1]  = Constants.MAX_PASSWORD_LEN * 2;
         dataItemLengths[2]  = Constants.MAX_WORKING_DIR_LEN;  // Domain name            
         if (parseData(new String(buffer), dataItemLengths, dataItems) == false) {
            Log.error("DskServer", "getUserInfo", 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
         }
         else {
            loginname = dataItems[0];
            password  = dataItems[1];
            ntdomain  = dataItems[2];
            
            // Decrypt the given password
            password = CryptLib.trimEncryptedPasswordTerminator(password);          
            if (CryptLib.plainDecryptString(strLoginKey, password,
                                            pStrNtcssDecryptedPassword, false, 
                                            true) != 0) {        
               Log.error("DskServer", "getUserInfo", 
                         "User conversion failed for " + loginname + "!");
               errorCode = ErrorCodes.ERROR_PASSWORD_TRANSLATION_FAILED;
               throw new GoTo();
            }               
            password = pStrNtcssDecryptedPassword.toString();     
                        
            Log.debug("DskServer", "getUserInfo", 
                      "parse_data returned loginname == " + loginname + ", " +
                      "ntdomain == " + ntdomain + "");         
            
            // Check to see if the given domain is in the list of domains from
            //  the SYS_CONF data
            if (isRegisteredDomain(ntdomain) == false) {
               Log.error("DskServer", "getUserInfo", "User " + loginname + 
                         " attempted login from domain " + ntdomain + 
                         " and failed.");                 
               errorCode = ErrorCodes.ERROR_USER_NOT_VALID;
               throw new GoTo();
            }
            
            // Make sure this machine is either user's primary auth
            //  server or master 
            errorCode = thisHostIsUserAuthServerOrMaster(loginname, connection);
            if (errorCode != 0) {        
               Log.error("DskServer", "getUserInfo", 
                        "thisHostIsUserAuthServerOrMaster returned non zero for " 
                        + loginname+ "!");
               throw new GoTo();
            }
            
            // Get the login reply structure for the given user
            client_ip = new StringBuffer();
            errorCode = UserLib.isUserLoggedIn(loginname,client_ip, connection);
            if (errorCode > 0) {  // i.e. The user is logged in to the local host
               
               if ((client_ip.length() > 0) &&                              
                   (client_ip.toString().equals(getClientAddress()) == true))
                  errorCode = getUserInfoEx(loginname, password, token, msg, 
                                             uid, group_list, 
                                             getClientHostname(), connection);
               else            
                  errorCode = ErrorCodes.ERROR_USER_ALREADY_IN;            
            }
            else if (errorCode == 0) {  // i.e. They're not logged in 
               errorCode = getUserInfoEx(loginname, password, token, msg, uid,
                                         group_list, getClientHostname(), 
                                         connection);               
            }
         } 
         
         // Record the login state in the login history table
         recordStateError = 0;
         switch (errorCode) {
            case ErrorCodes.ERROR_CANT_OPEN_DB_FILE:
            case ErrorCodes.ERROR_CANT_UNLOCK_LOGIN:
            case ErrorCodes.ERROR_USER_ALREADY_IN:
            case ErrorCodes.ERROR_CANT_ACCESS_DB:
            case ErrorCodes.ERROR_NO_SUCH_USER:
            case ErrorCodes.ERROR_USER_LOGIN_DISABLED:
            case ErrorCodes.ERROR_INCORRECT_PASSWORD:
            case ErrorCodes.ERROR_NO_GROUP_INFO_FOUND:
            case ErrorCodes.ERROR_NO_PROGRAM_INFO_FOUND:
            case ErrorCodes.ERROR_APP_TO_DM_PIPE_OPEN:
            case ErrorCodes.ERROR_APP_TO_DM_PIPE_WRITE:
            case ErrorCodes.ERROR_DM_SEMAPHORE:
            case ErrorCodes.ERROR_DM_TO_APP_PIPE_OPEN:
            case ErrorCodes.ERROR_DM_TO_APP_PIPE_READ:
            case ErrorCodes.ERROR_LOCAL_PROGRAM_INFO:
            case ErrorCodes.ERROR_NOT_USER_AUTH_SERVER_OR_MASTER:
            case ErrorCodes.ERROR_CANT_DETER_MASTER:
            case ErrorCodes.ERROR_CANT_CREATE_LRS:   
               recordStateError = UserLib.recordLoginState(loginname, 
                                                   getClientHostname(), 
                                                   null, null,
                                                   Constants.FAILED_LOGIN_STATE,
                                                   msg, connection);
               break;
            case ErrorCodes.ERROR_CANT_PARSE_MSG:
               break;
            default:
               recordStateError = UserLib.recordLoginState(loginname,                
                                                   getClientHostname(), 
                                                   token.toString(), 
                                                   group_list.toString(), 
                                                   Constants.GOOD_LOGIN_STATE,
                                                   msg, connection);
               if (errorCode >= 0 && recordStateError >= 0) {
                  errorCode += recordStateError;
                  Log.debug("DskServer", "getUserInfo", "User " + loginname + 
                             " logged in.");
               }               
                  
               break;
         }
         
         if ((errorCode >= 0) && (recordStateError < 0))
            errorCode = recordStateError;
         
         throw new GoTo();
      }
      catch (GoTo goTo) {   // Send a response to the client
                           
         if (errorCode < 0) {
            Log.debug("DskServer", "getUserInfo", "INVALID login for user " + 
                      loginname + "!");
            
//            sendOK = sendErrorMessage("INVALIDLOGIN", errorCode);  
            
            // Purposely used this method to send the error code instead of
            //  the more convient sendErrorMessage method. For some reason,
            //  when logging into NTCSS using the old desktop client, the
            //  client fails to receive an "INVALIDLOGIN" message unless it
            //  is purposely terminated with a null character (which the 
            //  sendErrorMessage doesn't add).
            String errorMsg = ErrorCodes.getErrorMessage(errorCode);
            sendOK = sendFormattedServerMsg("INVALIDLOGIN", StringLib.valueOf(errorMsg.length(), 5) +
                         errorMsg + "\0");               
         }
         else {
            Log.debug("DskServer", "getUserInfo", "VALID login for user " + 
                      loginname + ".");             
            sendOK = sendFormattedServerMsg("VALIDLOGIN", msg.toString() + "\0");
         }
         
         if (sendOK == false)
            Log.error("DskServer", "getUserInfo", 
                      "Could not send message from server for " + loginname + ".");
         
         Log.info("DskServer", "getUserInfo", "Exiting successfully for " + 
                  loginname);                  
      }
      catch (Exception exception) {
         Log.excp("DskServer", "getUserInfo", exception.toString());
      }
      finally {
         
         // Close the database resources 
         if (connection != null)
            DatabaseConnectionManager.releaseConnection(connection);
      }    
   }
   
   /**
    * Called when a user logs off from this server
    */
   public void userLogout()
   {
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname;
      String token;
      String group_list;
      String fnct_name = "userLogout"; 
      IntegerBuffer uid = new IntegerBuffer();
      
      try {      
         
         // Check if server is idled. 
         if (isServerIdled() == true) 
            return;          
         
         // Parse the client data
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
         dataItemLengths[2]  = Constants.MAX_GROUP_LIST_LEN;              
         if (parseData(dataItemLengths, 
                       dataItems) == false) 
            Log.error("DskServer", fnct_name, 
                         "Could not parse incoming message");                        
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            group_list = dataItems[2];
            
            // Send a response back to the client
            if (sendFormattedServerMsg("GOODUSERLOGOUT", 
                                       StringLib.valueOf(0, 5)) == false)            
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server.");

            // Check to see if the user is in the current_users table
            if (UserLib.isUserValidLocally(loginname, token, uid) < 0)
               Log.error("DskServer", fnct_name, 
                         "Could not validate user " + loginname + 
                         " for logout!");
            else {               
               Log.debug("DskServer", fnct_name, "User " + loginname + 
                         " logging off with groups " + group_list + "."); 
               
               // Attempt to remove the users's current_users record
               UserLib.recordLoginState(loginname, getClientHostname(),
                                        null, group_list,
                                        Constants.LOGOUT_STATE, null, null);                                                                       
            }
         }
      }
      catch (Exception exception) {
         Log.excp("DskServer", "userLogout", exception.toString());
      }
   }
   
   /**
    * This is called by another authenication server when it is trying to
    *  log a user in. 
    */
   public void authorizeUserLogin()
   {
      String fnct_name = "authorizeUserLogin";            
      int dataItemLengths[];
      String dataItems[];
      String timeStamp, loginName;
      Connection connection = null;
      Vector appuserProgroupAndRoles;
      StringBuffer message = new StringBuffer();
      int nRecs;
      AppuserProgroupRoleItem item;
      Object bundleItems[];
             
      try {
         Log.debug("DskServer", fnct_name, "Entering ...");
         
         // Check if server is idled. 
         if (isServerIdled() == true)
            return;                  
         
         // Get the data from the message
         dataItemLengths = new int[2];
         dataItems = new String[2];
         dataItemLengths[0]  = Constants.MAX_TIME_WITH_MILI_SEC_LEN;
         dataItemLengths[1]  = Constants.MAX_LOGIN_NAME_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("DskServer", fnct_name, 
                         "Could not parse incoming message");   
            
            // Send a response back to the client
            if (sendFormattedServerMsg(BADAUTHUSER, "\0") == false) 
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");                 
            return;
         }         
         timeStamp = dataItems[0];
         loginName = dataItems[1];
         
         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DskServer", fnct_name,  
                      "\"null\" connection returned");
            return;
         }
         
         // Check to see if the given user is in the local "users" db table
         if (UserLib.nIsUserInDB(loginName, connection) != 1) {    

            Log.debug("DskServer", fnct_name, "User " + loginName + 
                      " is not in this host's database. User cleared for login.");

            if (sendFormattedServerMsg(NOUSRINFOAUTHUSER, "\0") == false)
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");
            
            throw new GoTo();
         }
         
         // Make sure the user hasn't tried to login to this host
         if (clearUserForLogin(loginName, timeStamp, connection) == false) {

            Log.debug("DskServer", fnct_name, "User " + loginName + 
                      " not cleared for login!");
            if (sendFormattedServerMsg(BADAUTHUSER, "\0") == false)
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");      
            
            throw new GoTo();
         }
         
         Log.debug("DskServer", fnct_name, "User " + loginName + 
                   " cleared for login.");
         
         // Get the user's local progroup information
         appuserProgroupAndRoles = getUserProgroupAndRoleInfoForLogin(
                                                                loginName, 
                                                                connection);
         if (appuserProgroupAndRoles == null) {    
            Log.error("DskServer", fnct_name, 
                      "Failed to get progroup/role info for user " + 
                      loginName + "!");
            if (sendFormattedServerMsg(BADAUTHUSER, "\0") == false)
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");                

            throw new GoTo();
         }
         
         nRecs = appuserProgroupAndRoles.size();
         if ( (nRecs == 0) && 
              (Constants.BASE_NTCSS_USER.equals(loginName) == false) )  {    

            if (sendFormattedServerMsg(NOUSRINFOAUTHUSER, "\0") == false)
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");
      
            throw new GoTo();
         }

         // Now bundle all the records into a message to send back
         
         Log.debug("DskServer", fnct_name, "Identified " + nRecs + 
                   " programs for " + loginName);
         
         // First data in the message is how many records 
         bundleItems = new Object[1];
         bundleItems[0] = new Integer(nRecs);
         BundleLib.bundleData(message, "I", bundleItems);
         message.append(AUTHUSER_SEP_CHAR_STRING);
         
         // Next send the records bundled and separated by AUTHUSER_SEP_CHAR 
         bundleItems = new Object[5];
         for (int i = 0; i < nRecs; i++) {    
            item = (AppuserProgroupRoleItem)appuserProgroupAndRoles.elementAt(i);
            
            Log.debug("DskServer", fnct_name, "program[" + i + "] == " + 
                      item.getAppTitle());        

            bundleItems[0] = item.getAppTitle();
            bundleItems[1] = item.getPassword();
            bundleItems[2] = item.getAppData();
            bundleItems[3] = new Integer(item.getAppRolesNumber());
            bundleItems[4] = item.getLinkData();
            BundleLib.bundleData(message, "CCCIC", bundleItems);

            message.append(AUTHUSER_SEP_CHAR_STRING);
         }
         
         Log.debug("DskServer", fnct_name, "Msg Reply == " + GOODAUTHUSER);
         Log.debug("DskServer", fnct_name, "Msg Reply Data[1 - 400] == " +         
                   ((message.length() < 400) ? 
                    message.toString() : message.toString().substring(0, 400)));             
    
         // Send the response back to the other authentication server
         if (sendFormattedServerMsg(GOODAUTHUSER, message.toString()) == false)
            Log.error("DskServer", fnct_name, 
                      "Could not send message from server!");

         Log.debug("DskServer", fnct_name, "Exiting with 0 (Success)");
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         if (connection != null)
            DatabaseConnectionManager.releaseConnection(connection);
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
      }
   }      
   
   /**
    * Checks to see if the given user with the given token is logged in locally
    */
   public void validateUser()
   {
      String fnct_name = "validateUser";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token;
      IntegerBuffer uid = new IntegerBuffer();
      
      try {                
         
         // Check if server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the message
         dataItemLengths = new int[2];
         dataItems = new String[2];         
         dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;         
         dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("DskServer", fnct_name, 
                         "Could not parse incoming message");   
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            
            Log.debug("DskServer", fnct_name, "Validating user " + loginname);

            // Check to see if the user is logged in locally and if they are,
            //  return their UID in the response message
            if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
               errorCode = UserLib.nIsUserInDB(loginname, null);
            else if ((errorCode = UserLib.isUserValidLocally(loginname, 
                                                      token, uid)) >= 0)
               errorCode = uid.getValue();
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
       
         // Send a response to the client
         if (sendFormattedServerMsg("RETVALIDATEUSER", 
                                    StringLib.valueOf(errorCode, 5)) == false)
            Log.error("DskServer", fnct_name, 
                      "Could not send message from server.");
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Returns the given user's auth server to the requesting client
    */
   public void getAuthServer()
   {
      String fnct_name = "getAuthSvr";
      String loginname;      
      String strAuthServer;
      int dataItemLengths[];
      String dataItems[];

      try {     
         
         // Check if server is idled
         if (isServerIdled() == true) {
            
            // Send a response back to the client
            if (sendErrorMessage("BADGETAUTHSVR", 
                                 ErrorCodes.ERROR_SERVER_IDLED) == false)                        
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");                 
            return;
         }                               
  
         // Get the data from the message
         dataItemLengths = new int[1];
         dataItems = new String[1];         
         dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("DskServer", fnct_name, 
                         "Could not parse incoming message");   
            
            // Send a response back to the client
            if (sendErrorMessage("BADGETAUTHSVR", 
                                 ErrorCodes.ERROR_CANT_PARSE_MSG) == false) 
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");                 
            return;
         }                  
         loginname = dataItems[0];      
         
         Log.debug("DskServer", fnct_name, 
                   "Retreiving user auth server for " + loginname + ".");             

         // Attempt to get the user's authentication server from NDS
         if ((strAuthServer = NDSLib.getAuthServer(loginname)) == null) {    
            Log.error("DskServer", fnct_name, 
                "Failed to get auth server from NDS for user " + loginname + "!"); 
            if (sendErrorMessage("BADGETAUTHSVR", 
                                 ErrorCodes.ERROR_CANT_READ_VALUE) == false) 
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");       
         }
         else {
            
            Log.debug("DskServer", fnct_name, 
                   "Retreived user auth server <" + strAuthServer + "> for " + 
                   loginname + "."); 
            
            if (sendFormattedServerMsg("GOODGETAUTHSVR", strAuthServer + "\0") == false)
               Log.error("DskServer", fnct_name, 
                         "Could not send message from server!");           
         }
      }      
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Creates a key to be used by the desktop to encrypt the user's password
    *  with
    */
   private String getLoginEncryptionKey()
   {
      String strTemp1;
      String strTemp2;
      String strTemp3;
      String args[] = new String[3];
      StringBuffer strKeyBuffer = 
                          new StringBuffer(Constants.LOGIN_ENCRYPTION_KEY_LEN);
      
      strTemp1 = getClientHostname();      
      strTemp2 = getClientAddress();
      strTemp3 = StringLib.getFullTimeStr();                  
      
      // Use strTemp3 + 7, because the first part of the time string
      //  is the date which will only change every day, so we use the
      //  time portion of the time string which starts at the 7th char
      //  of the full time string. 
      args[0] = strTemp1;
      args[1] = strTemp2;
      args[2] = strTemp3.substring(7);
      
      strKeyBuffer.delete(0, Constants.LOGIN_ENCRYPTION_KEY_LEN);
      
      StringLib.getRandomString(strKeyBuffer, Constants.LOGIN_ENCRYPTION_KEY_LEN, 
                                args);          
      
      return strKeyBuffer.toString();
   }
   
   /**
    * Check to see if the given domain is in the list of domains from
    *  the SYS_CONF data. If the domain is in the list of given domains, true
    *  is returned. Otherwise, false is returned. If the domain list is not
    *  in the SYS_CONF file, true is returned.
    */
   private boolean isRegisteredDomain(String domain)
   {      
      StringBuffer registeredDomains = new StringBuffer();
      StringTokenizer tokenizer;            
         
      try {
         
         // Get the list of domains specified in the "NTDOMAIN" key in the
         //  SYS_CONF file
         if (DataLib.getNtcssSysConfData("NTDOMAIN", registeredDomains) == 0) {
            
            // Loop through each of the domains looking for a match with the
            //  domain that was passed in
            tokenizer = new StringTokenizer(registeredDomains.toString(), ",");            
            while(tokenizer.hasMoreTokens() == true) {
               
               // If a match is found, return true
               if (domain.equals(tokenizer.nextToken()) == true) 
                  return true;                                 
            }
            
            return false;
         }    
         
         // If NTDOMAIN is not a tag in the SYS_CONF file, return true anyways
         return true;
      }
      catch (Exception exception) {
         Log.excp("DskServer", "isRegisteredDomain", exception.toString());
         
         return false;
      }
   }
   
   /**
    * Checks to see if the local host is the authentication server for the
    *  given user or the master server. If this is the case, 0 is returned.
    *  Otherwise a negative error code is returned.
    */
   private int thisHostIsUserAuthServerOrMaster(String loginname, 
                                                Connection connection)
   {       
      String     strAuthServer;      
      String     strThisHost;             
      Statement  statement;
      ResultSet  resultSet;
            
      // Lookup the user's authentication server in the NDS data
      if ((strAuthServer = NDSLib.getAuthServer(loginname)) == null) {  
         Log.error("DskServer", "thisHostIsUserAuthServerOrMaster", 
                   "Failed to get auth server from NDS for user " + loginname 
                   + "!");             
         return ErrorCodes.ERROR_USER_NOT_VALID;
      }                  
      
      // Compare the local hostname to the master's hostname and the user's
      //  authentication server
      strThisHost = StringLib.getHostname();     
      if ((DataLib.getMasterServer(new StringBuffer()) != 1) &&
          (strThisHost.equalsIgnoreCase(strAuthServer) == false)) {
         Log.error("DskServer", "thisHostIsUserAuthServerOrMaster",
                   "This host is not the master or PAS for user " + loginname 
                   + ".");             
         return ErrorCodes.ERROR_NOT_USER_AUTH_SERVER_OR_MASTER;
      }

      return 0;
   }
   
   /**
    * Creates the login reply structure that is sent back to the client.
    *  This data is stored in the given "strMsg" buffer.
    */
   private int getUserInfoEx(String strLoginName, String strPassword,
                             StringBuffer strToken, StringBuffer strMsg,
                             StringBuffer pnUid, StringBuffer strGroupList,
                             String strClientAddr, Connection connection)
   {
      String fnct_name = "getUserInfoEx";
      int nErrorCode;
      String strThisMachine;
      Vector strHostList;
      Vector aTotal = null;
      Vector aDm = null;
      Vector aLocal = null;
      String currentTime = StringLib.getFullTimeStrWithMiliSec();
      AppuserProgroupRoleItem item;
      
      try {
          
         Log.debug("DskServer", fnct_name, "Entering for user " + strLoginName 
                   + ".");                   
         
         // Check to make sure the user isn't locked from logging in and that
         //  the given password is correct
         nErrorCode = preCreateLRS(strLoginName, strPassword, connection);
         if (nErrorCode != 0) {    
            Log.error("DskServer", fnct_name, 
                      "Failed to pre-create LRS for user " + strLoginName + "!");             
            return nErrorCode;
         }      
                  
         // Attempt to insert a "reservation" into the local current_users 
         //  table 
         strThisMachine = StringLib.getHostname();
         nErrorCode = makeLoginReservation(strLoginName, currentTime,                                         
                                           strClientAddr, strThisMachine,
                                           connection);     
         if (nErrorCode != 0) {    
            Log.error("DskServer", fnct_name, 
                      "Failed to make a login reservation for user " + 
                      strLoginName + "!");
            return nErrorCode;
         }
                  
         Log.debug("DskServer", fnct_name, "Before creating host list for " + 
                   strLoginName + "..");
         
         // Get a list of authentication servers excluding the local host
         strHostList = getAuthServerList(strThisMachine, connection);
         if (strHostList == null) {
                         
            // Remove the login reservation
            if (removeLoginReservation(strLoginName, connection) != 0)
               Log.error("DskServer", fnct_name, 
                         "Failed to remove a login reservation for user " + 
                         strLoginName + "!");

            return ErrorCodes.ERROR_CANT_ACCESS_DB;
         }
                  
         Log.debug("DskServer", fnct_name, "Completed creating host list for " + 
                   strLoginName + " <" + strHostList.toString() + ">");
         
         // If any servers are in the list, we must use the DM to get the 
         //  user's progroups from those hosts
         if (strHostList.size() != 0) {
            
            Log.debug("DskServer", fnct_name, 
                      "Before call to sendAuthUserToDm for " + strLoginName 
                      + "..");                 
            
            // Get the user's progroup info from the other servers
            aDm = new Vector();
            nErrorCode = getUserProgroupsFromAuthServers(strHostList, 
                                                         strLoginName,
                                                         currentTime, aDm);
            if (nErrorCode != 0) {        
               Log.error("DskServer", fnct_name, 
                     "Failed to get authorization responses back from the DM for " 
                     + strLoginName + "!");
               if (removeLoginReservation(strLoginName, connection) != 0)             
                  Log.error("DskServer", fnct_name, 
                      "Failed to remove a login reservation for user " + 
                      strLoginName + "!");
            
               return nErrorCode;
            }
            
            // Output the response from the DM
            if (Log.isDebugOn() == true) {
               Log.debug("DskServer", fnct_name, 
                         "========== DM RECORDS >>>>>>>>>>>>>>> for " + 
                         strLoginName);
               
               for (int j = 0; j < aDm.size(); j++) {            
                  Log.debug("DskServer", fnct_name, "....progrp => <" + 
                      ((AppuserProgroupRoleItem)aDm.elementAt(j)).getAppTitle() 
                      + ">");
         
                  Log.debug("DskServer", fnct_name, "....appdat => <" + 
                      ((AppuserProgroupRoleItem)aDm.elementAt(j)).getAppData() 
                      + ">");
                  
                  Log.debug("DskServer", fnct_name, "....lnkdat => <" + 
                      ((AppuserProgroupRoleItem)aDm.elementAt(j)).getLinkData() 
                      + ">");
               }  
               
               Log.debug("DskServer", fnct_name, 
                         "<<<<<<<<<<<< DM RECORDS =============== for " + 
                         strLoginName);               
            }
                                    
            // Get the user's progroups from the local server
            Log.debug("DskServer", fnct_name, 
                      "Before retrieving local progroup and role info for " + 
                      strLoginName + "..");
            aLocal = getUserProgroupAndRoleInfoForLogin(strLoginName, 
                                                        connection);
            if (aLocal == null) {
               Log.error("DskServer", fnct_name, 
                         "Failed to gather progroup/role info for user " + 
                         strLoginName + "!");
               return ErrorCodes.ERROR_LOCAL_PROGRAM_INFO;
            }
            
            // Combine the records retreived from the DM and the local records                    
            Log.debug("DskServer", fnct_name, 
                      "Consolidating DM's and Local progroup/role info for " + 
                      strLoginName);
            Log.debug("DskServer", fnct_name, strLoginName + " nDmRecs == " + 
                      aDm.size() + ", nLocalRecs == " + aLocal.size());
            
            aTotal = new Vector();
            for (int i = 0; i < aDm.size(); i++) {
               item = (AppuserProgroupRoleItem)aDm.elementAt(i);               
               aTotal.add(item);               
               Log.debug("DskServer", fnct_name, strLoginName + " progroup == " 
                         + item.getAppTitle() + ", app_role == " + 
                         item.getAppRoleString());                     
            }
            
            for (int i = 0; i < aLocal.size(); i++) {
               item = (AppuserProgroupRoleItem)aLocal.elementAt(i);               
               aTotal.add(item);               
               Log.debug("DskServer", fnct_name, strLoginName + " progroup == " 
                         + item.getAppTitle() + ", app_role == " + 
                         item.getAppRoleString());                     
            }                
         }
         else{  // The only authentication server in the DB is the local one
                 
            Log.debug("DskServer", fnct_name, 
                      "Bypassed DM, getting progroup/role info locally for " + 
                      strLoginName + "..");
            
            // Get the list of progroups on this host that the user is a part of
            aTotal = getUserProgroupAndRoleInfoForLogin(strLoginName, 
                                                        connection);
            if (aTotal == null) {        
               Log.error("DskServer", fnct_name, 
                         "Failed to get progroup/role info for user " + 
                         strLoginName + "!");
               return ErrorCodes.ERROR_LOCAL_PROGRAM_INFO;
            }
         }
                                        
         Log.debug("DskServer", fnct_name, "Before call to createLRS for " + 
                   strLoginName + "..");   
         
         // Create the login reply structure from the user's progroup info,
         //  to return back to the client
         nErrorCode = ((createLRS(strLoginName, aTotal, strToken, strMsg, pnUid,
                                  strGroupList, connection)) 
                                      ? 0 : ErrorCodes.ERROR_CANT_CREATE_LRS);
         if (nErrorCode != 0) {    
            Log.error("DskServer", fnct_name, "Failed to create LRS for user " + 
                      strLoginName + "!");
             
            if (removeLoginReservation(strLoginName, connection) != 0)               
               Log.error("DskServer", fnct_name, 
                         "Failed to remove a login reservation for user " + 
                         strLoginName + "!");
        
            return nErrorCode;
         }
  
         Log.debug("DskServer", fnct_name, "LRS == [1 - 400] " +            
                   ((strMsg.length() < 400) ? 
                    strMsg.toString() : strMsg.toString().substring(0, 400)));
 
         // Remove the user's login reservation
         nErrorCode = removeLoginReservation(strLoginName, connection);
         if (nErrorCode != 0) {    
            Log.error("DskServer", fnct_name, 
                      "Failed to remove a login reservation for user " +  
                      strLoginName +"!");
            return nErrorCode;
         }
          
         Log.debug("DskServer", fnct_name, "Exiting with " + nErrorCode + 
                   " for " + strLoginName + ". (0 == Success)");

         return nErrorCode;         
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Checks to make sure the user is not locked out, and that the given 
    *  password is the user's correct password. If there are no errors,
    *  0 is returned.
    */
   private int  preCreateLRS(String loginname, String password, 
                             Connection connection)                  
   {
      String fnct_name = "preCreateLRS";
      StringBuffer strSSN;
      String SQLString;
      Statement statement;
      ResultSet resultSet;      
      Object args[];

      Log.debug("DskServer", fnct_name, "Entering for " + loginname + "...");
                     
      try {
         
         //  Execute the query
         args = new Object[1];
         args[0] = loginname;
         SQLString = StringLib.SQLformat("SELECT login_lock FROM users WHERE " +
                     "login_name=" + Constants.ST, args); 
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         if (resultSet.next() == false) {         
            Log.error("DskServer", fnct_name, "User <" + loginname + 
                      "> not in database!");
            resultSet.close();
            statement.close();            
            return ErrorCodes.ERROR_NO_SUCH_USER;
         }

         // Check to see if the user is locked out
         if (resultSet.getBoolean("login_lock") == true) {         
            Log.error("DskServer", fnct_name, "User " + loginname + 
                      " is locked out from logging in.");             
            resultSet.close();
            statement.close();            
            return ErrorCodes.ERROR_USER_LOGIN_DISABLED;
         }
         
         // Close the database resources
         resultSet.close();
         statement.close();         
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_NO_SUCH_USER;
      }
      
      // If the given password isn't the special password, make sure it is the
      //  correct one for the given user
      strSSN = new StringBuffer();
      if (password.equals("XYZNTCSSNAVMASSOINRIzyx") == false &&
          UserLib.checkNtcssUserPassword(loginname, password, strSSN) != 1) {                                 

         Log.error("DskServer", fnct_name, "Incorrect password specified for " +
                   "user " + loginname + "!");             
         return ErrorCodes.ERROR_INCORRECT_PASSWORD;
      }

      Log.debug("DskServer", fnct_name, "Successfully exiting with 0 for " + 
                loginname + ".");    

      return 0;
   }
   
   /**
    * Adds a "reservation" entry in the local current_users table and inserts
    *  an entry in the login_history table. Returns 0 if everything was ok.
    */
   private int makeLoginReservation(String strLoginName, String strReserveTime,
                                    String strClientIp, String strAuthServer,
                                    Connection connection)
   {
      String SQLString;
      Statement statement;
      Object args[];

      try {
         
         // Update the current user's record
         if (UserLib.addCurrentUsersRecord(strLoginName, "0", strClientIp,
                                           "NoToken", strReserveTime,
                                           connection) == 0) {                     
      
            // Insert a record into the login_history
            args = new Object[4];
            args[0] = strLoginName;
            args[1] = strClientIp;
            args[2] = strReserveTime;
            args[3] = strAuthServer;
            SQLString = StringLib.SQLformat(
                 "INSERT INTO login_history (login_name, client_address,"
                 + "login_time, status, auth_server) VALUES(" + Constants.ST
                 + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.LOGIN_RESERVED_STATE + "," + Constants.ST + 
                 ")", args);               

            // Execute the insert         
            statement = connection.createStatement();
            statement.executeUpdate(SQLString);
            statement.close();     
            
            return 0;
         }         
      }
      catch (Exception exception) {
         Log.excp("DskServer", "makeLoginReservation", exception.toString());
      }
      
      return ErrorCodes.ERROR_CANT_ACCESS_DB;
   }
   
   /**
    * Removes the user's entry out of the local current_users table and inserts
    *  an entry in the login_history table. Returns 0 if everything was ok.
    */
   private int removeLoginReservation(String strLoginName, Connection connection)
   {      
      String SQLString;
      Statement statement;
      Object args[];
      
      try {
         
         // Update the current user's record
         if (UserLib.removeCurrentUsersRecord(strLoginName, connection) == 0) {
            
            // Insert a record into the login_history
            args = new Object[4];
            args[0] = strLoginName;
            args[1] = "N/A";        // client address
            args[2] = StringLib.getFullTimeStrWithMiliSec();
            args[3] = "N/A";        // authenitcation server
            SQLString = StringLib.SQLformat(
                 "INSERT INTO login_history (login_name, client_address,"
                 + "login_time, status, auth_server) VALUES(" + Constants.ST
                 + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.REMOVE_RESERVED_STATE + "," + Constants.ST + 
                 ")", args);               

            // Execute the insert         
            statement = connection.createStatement();
            statement.executeUpdate(SQLString);
            statement.close();     
            
            return 0;
         }
      }
      catch (Exception exception) {
         Log.excp("DskServer", "removeLoginReservation", exception.toString());
      }
      
      return ErrorCodes.ERROR_CANT_ACCESS_DB;
   }
   
   /**
    * Return from the database the list if authentication servers (including
    *  the master) excluding the given host.
    */
   private Vector getAuthServerList(String hostname, Connection connection)
   {
      Vector list;
      Statement statement;
      ResultSet resultSet;
      String SQLString;
      Object args[];
      
      try {
         args = new Object[1];
         args[0] = hostname.toUpperCase();
         SQLString = StringLib.SQLformat("SELECT hostname FROM hosts WHERE " +
                   "upper(hostname) != " + Constants.ST + " AND type != " 
                   + Constants.NTCSS_APP_SERVER, args);
         
         // Execute query
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         list = new Vector();
         while (resultSet.next() == true) 
            list.add(resultSet.getString("hostname"));         
         
         resultSet.close();
         statement.close();     
         
         return list;
      }
      catch (Exception exception) {
         Log.excp("DskServer", "getAuthServerList", exception.toString());
         return null;
      }
   }  
   
   /**
    * Get a list of the unlocked progroups that are on the local machine that
    *  the given user is a part of.
    */
   private Vector getUserProgroupAndRoleInfoForLogin(String login_name, 
                                                     Connection connection)
   {
      String fnct_name = "getUserProgroupAndRoleInfoForLogin";            
      Vector AppuserProgroupRoleItems;
      String strRoles;
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      AppuserProgroupRoleItem item;     
      boolean done;
      String currentApp;
      Object args[];

      try {
                  
         AppuserProgroupRoleItems = new Vector();
         
         // Check to see if the user is even in the local database
         if (UserLib.nIsUserInDB(login_name, connection) != 1) {    
            Log.info("DskServer", fnct_name, 
                     "User (" + login_name + ") has no information on this host.");
            return AppuserProgroupRoleItems;
         }
         
         // Execute the query
         args = new Object[5];
         args[0] = StringLib.getHostname().toUpperCase();
         args[1] = login_name;
         args[2] = new Boolean(false);
         args[3] = new Boolean(true);
         args[4] = new Boolean(true);
/*         
         SQLString = StringLib.SQLformat(
            "SELECT DISTINCT a.app_title, au.app_passwd, au.custom_app_data," +
              "a.link_data,a.hostname, h.type, uar.role_name " +
            "FROM apps a, users u,app_users au, hosts h, user_app_roles uar, " +
              "app_roles ar " +
            "WHERE  upper(a.hostname) = " + Constants.ST + " AND " +
                "a.hostname=h.hostname AND " +
                "u.login_name = " + Constants.ST + " AND " +
                "u.login_name = au.login_name AND " +
                "a.app_title = au.app_title AND " +              
                "(au.login_name=uar.login_name AND au.app_title=uar.app_title)  AND "+
                "(uar.app_title=ar.app_title AND uar.role_name=ar.role_name)  AND " +
                "(a.app_lock = " + Constants.ST + " OR (a.app_lock = " + Constants.ST + 
                   " AND ar.app_lock_override = " + Constants.ST + ")) " +
            "ORDER BY a.app_title, uar.role_name", args);
 */
         
         // This SQL statement will not only return apps on the local host, 
         //  but if this is a master server, it will also return apps that 
         //  reside on application servers.
         SQLString = StringLib.SQLformat(
            "SELECT DISTINCT a.app_title, au.app_passwd, au.custom_app_data," +
              "a.link_data, uar.role_name " +
            "FROM apps a, app_users au, hosts h, user_app_roles uar, app_roles ar " +
            "WHERE  " + 
                (DataLib.isMasterServer() ?                  
                   ("((upper(a.hostname) = " + Constants.ST + ") OR (a.hostname = h.hostname AND h.type = 1))") : 
                   ("upper(a.hostname) = " + Constants.ST )) + " AND " +                
                
                "au.login_name = " + Constants.ST + " AND " +                
                "a.app_title = au.app_title AND " +              
                "(au.login_name=uar.login_name AND au.app_title=uar.app_title)  AND "+
                "(uar.app_title=ar.app_title AND uar.role_name=ar.role_name)  AND " +
                "(a.app_lock = " + Constants.ST + " OR (a.app_lock = " + Constants.ST + " AND ar.app_lock_override = " + Constants.ST + ")) " +
            "ORDER BY a.app_title, uar.role_name", args);


         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);                              
         
         // Store the information in the list
         done = !resultSet.next();
         currentApp = null;
         while(done == false) {                                          
            
            item = new AppuserProgroupRoleItem( );            
            if (currentApp == null)
               currentApp = resultSet.getString("app_title");
            item.setAppTitle(currentApp);
            item.setPassword(resultSet.getString("app_passwd"));
            item.setAppData(resultSet.getString("custom_app_data"));
            item.setLinkData(resultSet.getString("link_data"));

            // Get the roles            
            while (true) {                      

               item.addAppRole(resultSet.getString("role_name"));
               
               if (resultSet.next() == false) {
                  done = true;
                  break;               
               }
               
               currentApp = resultSet.getString("app_title");
               if (currentApp.equals(item.getAppTitle()) == false)
                  break;
            }            
                        
            AppuserProgroupRoleItems.add(item);
         }
         
         // Close database resources
         resultSet.close();
         statement.close();
         
         return AppuserProgroupRoleItems;
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         return null;
      }
   }
   
   /**
    * Creates a login reply structure (LRS) from the given app roles and data.
    *  The result is stored in the given "msgLRS" parameter.
    */
   private boolean createLRS(String loginname, Vector approles, 
                             StringBuffer token, StringBuffer msgLRS, 
                             StringBuffer uid, StringBuffer group_list,
                             Connection connection)
   {     
      String fnct_name = "createLRS";
      String server;      
      int    leng;
//      int    tmp_app_role;
      String buf;      
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      String vals[];
      Object bundleItems[];
      AppuserProgroupRoleItem item;
      boolean firstRecordForApp;
      String real_nameT, passwordT, ss_numberT, phone_numberT, pw_change_timeT, 
             shared_db_passwdT;
//      boolean ntcss_super_userT;
      int unix_idT, security_classT;                   
      String hostnameT, icon_fileT, version_numberT, release_dateT, 
             client_locationT, server_locationT, program_titleT, program_fileT,
             pg_icon_fileT, cmd_line_argsT, working_dirT;
      int typeT, icon_indexT, cntT, pg_icon_indexT, program_typeT;       
      Object args[];
      String tmpTableName;
      int programCount;
      int appRolesNumber;      
      String tmpTokenStr;
      int oldUserRoleField;
      
      try {
         
//         tmp_app_role = Constants.NTCSS_USER_ROLE;
      
         group_list.delete(0, group_list.capacity());
      
         Log.debug("DskServer", fnct_name, "Entering for user " + loginname + 
                   "..");
         
         // Get the user's "user" record
         args = new Object[1];
         args[0] = loginname;
         SQLString = StringLib.SQLformat("SELECT * FROM users WHERE " +
                      "login_name=" + Constants.ST, args);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         resultSet.next();
         
         // The columns have to be extracted in the order that they appear
         //  in the database due to a SQL Server ODBC error
         real_nameT = resultSet.getString("real_name");
         passwordT = resultSet.getString("password");
         unix_idT = resultSet.getInt("unix_id");                 
         ss_numberT = resultSet.getString("ss_number");
         phone_numberT = resultSet.getString("phone_number");
         security_classT = resultSet.getInt("security_class");
         pw_change_timeT = resultSet.getString("pw_change_time");
         shared_db_passwdT = resultSet.getString("shared_db_passwd");
//         ntcss_super_userT = resultSet.getBoolean("ntcss_super_user");                  
         
         // Set the given uid field
         uid.insert(0, StringLib.valueOf(unix_idT, -1));
         
         Log.debug("DskServer", fnct_name, "unix id for " + loginname + " == " + 
                   uid.toString()); 
         
         // Create a login token. getRandomString potientially could return
         //  a string that ends in spaces. Make sure these spaces are trimmed
         //  off the end. Since information that is sent from the client
         //  is trimmed when it is parsed apart, spaces at the end of the token
         //  string will obviously cause a problem.
         vals = new String[3];
         do {
            // Get a random string
            token.setLength(0);
            buf = StringLib.getFullTimeStr();         
            vals[0] = buf.substring(5);
            vals[1] = passwordT;         
            vals[2] = loginname;         
            StringLib.getRandomString(token, Constants.MAX_TOKEN_LEN, vals);
            
            // Trim the spaces off the end of the string
            tmpTokenStr = token.toString().trim();            
            
            // If the length of the trimmed string is now zero, do the process
            //  again
         } while (tmpTokenStr.length() == 0);  
         token.setLength(0);  // Clear the token buffer
         token.insert(0, tmpTokenStr);
         
         // Need to check if user is app admin for NTCSS
         oldUserRoleField = 0;
         for (int e = 0; e < approles.size(); e++) {
            item = (AppuserProgroupRoleItem)approles.elementAt(e);
            if (item.getAppTitle().equals(Constants.NTCSS_APP) == true) {
               if ((item.getAppRolesNumber() & 0x04) != 0)
                 oldUserRoleField = 1;
               break;
            }
         }
                  
         Log.debug("DskServer", fnct_name, "Before bundleData for " + loginname 
                   + "..");
                         
         // Bundle the user data to the message
         bundleItems = new Object[11];
         bundleItems[0] = token.toString();
         bundleItems[1] = new Integer(uid.toString());
         bundleItems[2] = loginname;
         bundleItems[3] = real_nameT;
         bundleItems[4] = ss_numberT;
         bundleItems[5] = phone_numberT;
         bundleItems[6] = new Integer(security_classT);
         bundleItems[7] = pw_change_timeT;
         bundleItems[8] = shared_db_passwdT;                    
//         bundleItems[9] = new Integer(ntcss_super_userT ? 1:0);
         bundleItems[9] = new Integer(oldUserRoleField);
         bundleItems[10] = new Integer(approles.size());
         leng = BundleLib.bundleData(msgLRS, "CICCCCIDCII", bundleItems);
                    
         resultSet.close();
         
         Log.debug("DskServer", fnct_name, 
                   "Before collecting progroup info, ngroups == " + 
                   approles.size() + " for " + loginname);
         
         // Create a table which contains LRS information for the user
         tmpTableName = loginname + System.currentTimeMillis();
         if(makeLRSTable(connection, approles, tmpTableName) == false) {
            statement.close();
            return false;
         }
         
         // Bundle the user's progroup data to the message
         args = new Object[1];
         for (int i = 0; i < approles.size(); i++) {
            item = (AppuserProgroupRoleItem)approles.elementAt(i);
            
            Log.debug("DskServer", fnct_name, 
                      "In for loop collecting progroup info, i == " + i + 
                      " -->" + item.getAppTitle() + " for " + loginname); 
            
            // Get the number of programs for this application
            args[0] = item.getAppTitle();            
            SQLString = StringLib.SQLformat("select count(*) as cnt from LRS"
                  + tmpTableName + " WHERE app_title = " + Constants.ST, args);
            resultSet = statement.executeQuery(SQLString);
            resultSet.next();
            programCount = resultSet.getInt("cnt");
            resultSet.close();
                                    
            SQLString = StringLib.SQLformat("SELECT * FROM LRS" + tmpTableName + 
                     " WHERE app_title=" + Constants.ST, args);
            resultSet = statement.executeQuery(SQLString);
            
            firstRecordForApp = true;
            while (resultSet.next() == true) {               
                                                                                                      
               if (firstRecordForApp == true) {
                  group_list.append(item.getAppTitle() + " ");
                  
/*                  
                  // Adjust role for desktop and DLL
                  if (isAppAdmin(item.getAppRoles()) == true)
                     tmp_app_role = Constants.APP_ADMIN_ROLE;
 */
                  
                  // The columns have to be extracted in the order that they 
                  //  appear in the database due to a SQL Server ODBC error
                  cntT = programCount; 
                  hostnameT = resultSet.getString("hostname");               
                  typeT = resultSet.getInt("type");                          
                  icon_fileT = resultSet.getString("icon_file");               
                  icon_indexT = resultSet.getInt("icon_index");               
                  version_numberT = resultSet.getString("version_number");               
                  release_dateT = resultSet.getString("release_date");               
                  client_locationT = resultSet.getString("client_location");               
                  server_locationT = resultSet.getString("server_location");
                  
                  // This kluge is done in the old code for some reason,
                  //  so it is duplicated here
                  if ((appRolesNumber = item.getAppRolesNumber()) == 0)
                     appRolesNumber = 8;
                  
                  // Adjust role for desktop and DLL
                  if ((appRolesNumber & 0x04) != 0)            
                     appRolesNumber |= Constants.APP_ADMIN_ROLE;            

                  // Bundle app specific data up
                  bundleItems = new Object[13];
                  bundleItems[0] = item.getAppTitle();
                  bundleItems[1] = hostnameT;
                  bundleItems[2] = new Integer(typeT);
//                  bundleItems[3] = new Integer(tmp_app_role);
                  bundleItems[3] = new Integer(appRolesNumber);
                  bundleItems[4] = item.getAppData();
                  bundleItems[5] = item.getPassword();
                  bundleItems[6] = icon_fileT;
                  bundleItems[7] = new Integer(icon_indexT);
                  bundleItems[8] = version_numberT;
                  bundleItems[9] = release_dateT;
                  bundleItems[10] = client_locationT;
                  bundleItems[11] = server_locationT;
                  bundleItems[12] = new Integer(cntT);
                  leng += BundleLib.bundleData(msgLRS, "CCIICCCICDCCI", bundleItems);                             
                  
                  firstRecordForApp = false;
               }
               
               // The columns have to be extracted in the order that they 
               //  appear in the database due to a SQL Server ODBC error
               program_titleT = resultSet.getString("program_title");
               program_fileT = resultSet.getString("program_file");
               pg_icon_fileT = resultSet.getString("pg_icon_file");
               pg_icon_indexT = resultSet.getInt("pg_icon_index");               
               cmd_line_argsT = resultSet.getString("cmd_line_args");
               security_classT = resultSet.getInt("security_class");
               working_dirT = resultSet.getString("working_dir");
               program_typeT = resultSet.getInt("program_type");
               
               // Bundle the programs for this app               
               bundleItems = new Object[8];
               bundleItems[0] = program_titleT;
               bundleItems[1] = program_fileT;
               bundleItems[2] = pg_icon_fileT;
               bundleItems[3] = new Integer(pg_icon_indexT);
               bundleItems[4] = cmd_line_argsT;
               bundleItems[5] = new Integer(security_classT);
               bundleItems[6] = new Integer(program_typeT);
               bundleItems[7] = working_dirT;
               leng += BundleLib.bundleData(msgLRS, "CCCICIIC", bundleItems);                         
            }
            
            resultSet.close();
         }
         
         // Remove the temporary table
         statement.executeUpdate("DROP TABLE LRS" + tmpTableName);
         
         statement.close();
                  
         Log.debug("DskServer", fnct_name, "After collecting progroup info for " 
                   + loginname + "..");         
         Log.debug("DskServer", fnct_name, "Exiting with success for user " + 
                   loginname);
         Log.debug("DskServer", fnct_name, "Exiting with group list <" + 
                   group_list.toString() + "> for " + loginname);

         return true;
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Returns true if "App Administrator" is one of the roles
    */
   private boolean isAppAdmin(Enumeration roles)
   {     
      String role;
      
      while (roles.hasMoreElements() == true) {
         role = (String)roles.nextElement();
         if (role.equals("App Administrator") == true)
            return true;
      }
      
      return false;
   }      
   
   /**
    * Creates a temp table for the efficient creation of the LRS
    * Called in createLRS... 
    */
   private boolean makeLRSTable(Connection connection, Vector approles,
                                String tmpTableName)                         
   {
      String fnct_name = "makeLRSTable";
      String SQLString;
      Statement statement;          
      AppuserProgroupRoleItem item;
      Enumeration roles;
      Object sqlItems[];

      try {
         Log.debug("DskServer", fnct_name, 
                   "Preparing to make the 1st temp table " + tmpTableName);
         
         statement = connection.createStatement();

         // Make 1st temp table.. really just used to make the 2nd    
         SQLString = "CREATE TABLE " + tmpTableName + " (app_title TEXT NOT NULL," +
                     "role_name TEXT NOT NULL, app_data TEXT, " + 
                     "shared_pw TEXT)";         
         statement.executeUpdate(SQLString);                  
         
         // Go through the combined AppuserProgroupRoleItems 
         sqlItems = new Object[4];
         for (int i = 0; i < approles.size(); i++) { 
            
            item = (AppuserProgroupRoleItem)approles.elementAt(i);
            
            roles = item.getAppRoles();
            while (roles.hasMoreElements() == true) {               
                                
               sqlItems[0] = item.getAppTitle();
               sqlItems[1] = (String)roles.nextElement();
               sqlItems[2] = item.getAppData();
               sqlItems[3] = item.getLinkData();
               statement.executeUpdate("INSERT INTO " + tmpTableName + 
                       " (app_title, role_name, app_data, shared_pw) values (" + 
                       StringLib.SQLformat(Constants.ST + "," + 
                                           Constants.ST + "," +
                                           Constants.ST + "," +
                                           Constants.ST, sqlItems) + ")");                                       
            }
         }
         
         Log.debug("DskServer", fnct_name, 
                   "Preparing to make the 2nd temp table LRS" + tmpTableName);

         // Now make second temp table from 1st table plus the other stuff we need
         statement.executeUpdate("CREATE TABLE LRS" + tmpTableName + 
           " (app_title TEXT, hostname TEXT, type INTEGER, app_data TEXT,"+
           "shared_pw TEXT, icon_file TEXT, icon_index INTEGER, " +
           "version_number TEXT, release_date TEXT, client_location TEXT, " +
           "server_location TEXT, program_title TEXT, program_file TEXT, " +
           "pg_icon_file TEXT, pg_icon_index INTEGER, cmd_line_args TEXT, " +
           "security_class INTEGER, working_dir TEXT, program_type INTEGER)");
           
         SQLString = "INSERT INTO LRS" + tmpTableName + 
         " (app_title, hostname, type, app_data,"+
           "shared_pw, icon_file, icon_index, " +
           "version_number, release_date, client_location, " +
           "server_location, program_title, program_file, " +
           "pg_icon_file, pg_icon_index, cmd_line_args, " +
           "security_class, working_dir, program_type) " +         
          "SELECT DISTINCT a.app_title,b.hostname,type,a.app_data,a.shared_pw," +
           "b.icon_file,b.icon_index,version_number,release_date,client_location,server_location," +
           "e.program_title,e.program_file,e.icon_file,e.icon_index," +
           "cmd_line_args,security_class,working_dir,program_type FROM " + tmpTableName + " a,apps b," +
           "hosts c, app_program_roles d, app_programs e WHERE a.app_title=b.app_title AND " +
           "b.hostname=c.hostname AND a.app_title=d.app_title AND a.role_name=d.role_name AND  " +
           "a.app_title=d.app_title AND d.app_title=e.app_title AND d.program_title=e.program_title";
         statement.executeUpdate(SQLString);                             
         
         // Remove the first temporary table
         statement.executeUpdate("DROP TABLE " + tmpTableName);         
  
         statement.close();

         return true;  
      }
      catch (Exception exception) {   
         Log.excp("DskServer", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Send the "AUTHUSER" message to the given list of hosts and process
    *  their replies. The results are stored in the given "aDm" parameter.
    */
   private int getUserProgroupsFromAuthServers(Vector strHostList, 
                                               String strLoginName,
                                               String currentTime, 
                                               Vector aDm)
   {      
      String fnct_name = "getUserProgroupsFromAuthServers";
      DPResults results;
      String messageToDistribute;
      Enumeration returnedHosts;
      String hostResult;      
      String currentHost;        
      int returnCode;
      
      try {
         
         // Create the message that will be sent to all the hosts in the
         //  given list
         messageToDistribute = formatServerMsg("AUTHUSER", 
           StringLib.padString(currentTime, 
                               Constants.MAX_TIME_WITH_MILI_SEC_LEN, ' ') +                    
           StringLib.padString(strLoginName, 
                               Constants.MAX_LOGIN_NAME_LEN, ' '));
         
         Log.debug("DskServer", fnct_name, "Sending message to auth servers for " 
                   + strLoginName);
         
         // Send the message to the hosts and wait for their response
         results = DistributionManager.sendMessage(messageToDistribute, 
                                                   strHostList, 
                                                   Constants.DSK_SVR_PORT, 
                                                   false,
                                                   Constants.DP_AUTHUSER_TIMEOUT);   
         
         Log.debug("DskServer", fnct_name, 
                   "Processing results from auth servers for " + strLoginName);
         
         // Get a list of all the hosts that returned results
         returnedHosts = results.getHosts();
         
         // Process the hosts' responses         
         while (returnedHosts.hasMoreElements() == true) {
            currentHost = (String)returnedHosts.nextElement();
            hostResult = (String)results.getResult(currentHost);                                         
            
            // Store the hosts response in the "aDm" data structure
            if ((returnCode = getAuthUserReply(currentHost, hostResult, aDm)) 
                                                                          != 0)            
               return returnCode;            
         }
         
         return 0; 
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         
         return -1;
      }                 
   }
   
   /**
    * Parses the given reply from the given authentication server and stores
    *  the results in the given reply structure
    */
   private int getAuthUserReply(String host, String reply, Vector replyStruct)
   {
      String fnct_name = "getAuthUserReply";      
      Object numItems[];      
      Object dataItems[];
      StringTokenizer tokenizer;    
      int numRecords;
      String data;
      String header, messageData;
      
      try {
         numItems = new Object[1];
         dataItems = new Object[5];
         
         if ((reply != null) &&
             (reply.length() >= Constants.CMD_LEN)) {     
                       
            header = reply.substring(0, Constants.CMD_LEN).trim();
            messageData = reply.substring(Constants.CMD_LEN);
                   
            // Check to see if the host's response is equal to GOODAUTHUSER
            if (header.equals(GOODAUTHUSER) == true) {                
                    
               tokenizer = new StringTokenizer(messageData, 
                                               AUTHUSER_SEP_CHAR_STRING);
               
               // Get the number of records
               if (BundleLib.unbundleData(tokenizer.nextToken(), "I", numItems) 
                                                                    == false) {
                  Log.error("DskServer", fnct_name, "Error unbundling count " +
                            "for " + host);
                  return 0;
               }
               numRecords = ((Integer)numItems[0]).intValue();
                                   
               // Unbundle the host's data
               for (int i = 0; (i < numRecords) &&
                    ((data = tokenizer.nextToken()).trim().length() > 0); i++) {                     
                  if (BundleLib.unbundleData(data, "CCCIC", dataItems) == true) {

                     // Store the data in the return object
                     replyStruct.add (new AppuserProgroupRoleItem(
                                                        (String)dataItems[0],
                                                        (String)dataItems[1],
                                                        (String)dataItems[2],
                                                        (Integer)dataItems[3],
                                                        (String)dataItems[4]));                                                            
                  }
                  else {
                     Log.error("DskServer", fnct_name, 
                               "Error unbundling data from " + host);
                     return 0;
                  }
               }
            }
            else if (header.equals(BADAUTHUSER) == true) {  
                           
               // If didn't receive ACK (i.e. GOODAUTHUSER), return
               //  immediately
                           
               replyStruct.clear();                  
               return ErrorCodes.ERROR_USER_ALREADY_IN;
            }
         }
         else 
            Log.debug("DskServer", fnct_name, "Received bad reply from " + 
                      host);
         
         return 0;
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         
         return -1;
      }
   }
   
   /**
    * Makes sure user isn't logged into this host. If they are, checks the
    *  reservation time. If the user can log in, true is returned.
    */
   private boolean clearUserForLogin(String strLoginName, String strTimeStamp,
                                     Connection connection)
   {
      String fnct_name = "clearUserForLogin";
      StringBuffer strClientIp = new StringBuffer();
      Statement statement;
      ResultSet resultSet;
      
      try {
         Log.debug("DskServer", fnct_name, "Entering (" + strLoginName + ", " + 
                   strTimeStamp + ").");
      
         // Check to see if the user is logged in to the local host
         if (UserLib.isUserLoggedIn(strLoginName, strClientIp, connection) > 0) {    
            Log.error("DskServer", fnct_name, "User " + strLoginName + 
                      " already logged in.");
            return false;
         }
         
         // Check the current_users table for the user's reservation time
         statement = connection.createStatement();
         resultSet = statement.executeQuery("SELECT reserver_time FROM " +
                    "current_users WHERE login_name = '" + strLoginName + "'");
         
         // User not logged in or reserved on this host 
         if (resultSet.next() == false) {             
            
            Log.debug("DskServer", fnct_name, "(" + strLoginName + ", " + 
                      strTimeStamp + "): User cleared for login.");
            
            resultSet.close();
            statement.close();
            return true;
         }
         
         // Check to see if this host has a newer reservation time for the user
         if (strTimeStamp.compareTo(resultSet.getString("reserver_time")) < 0) {    
            
            // Timestamp is less than our reserved time, so we 
            //  clearUserForLogin and remove the reservation on this host 
      
            Log.debug("DskServer", fnct_name, "(" + strLoginName + ", " + 
                      strTimeStamp + "): User cleared for login. Reservation" +
                    " newer than ours, so removing user reservation from our " +
                    "database.");

            if (removeLoginReservation(strLoginName, connection) != 0) {        
               Log.error("DskServer", fnct_name, 
                         "Failed to remove login reservation for user " + 
                         strLoginName);
               return false;
            }
            
            return true;
         }
         
         // Our reservation is more recent, so we do not clear the user to log 
         //  in on the other host 
         Log.debug("DskServer", fnct_name, "(" + strLoginName + ", " + 
                   strTimeStamp + "): User NOT cleared for login. Our " +
               "Reservation is newer, so we NAK authorize user login request");
         
         resultSet.close();
         statement.close();
         
         return false;
      }
      catch (Exception exception) {
         Log.excp("DskServer", fnct_name, exception.toString());
         return false;
      }
   }
}

class GoTo extends Throwable
{
   public GoTo()
   {
      super();
   }
}

/**
 * A structure that contains progroup information for a user
 */
class AppuserProgroupRoleItem 
{
   private String progroup_title;
   private String shared_passwd;
   private String app_data;   
   private String link_data;   
   private Vector appRoles;
   
   public AppuserProgroupRoleItem()
   {
      appRoles = new Vector();                   
      
      setAppTitle(null);
      setPassword(null);
      setAppData(null);      
      setLinkData(null);
   }
   
   public AppuserProgroupRoleItem(String appTitle, String passwd, 
                                  String appData, Integer appRoleNumber,
                                  String linkData)                  
   {
      appRoles = new Vector();            
      
      setAppTitle(appTitle);
      setPassword(passwd);
      setAppData(appData);
      setAppRoles(appRoleNumber.intValue());
      setLinkData(linkData);
   }
   
   public void setAppTitle(String app_title)
   {
      progroup_title = app_title;
   }
   
   public String getAppTitle()
   {
      return progroup_title;
   }
   
   public void setPassword(String app_passwd)
   {
      shared_passwd = app_passwd;
   }
   
   public String getPassword()
   {
      return shared_passwd;
   }
   
   public void setAppData(String custom_app_data)
   {    
      app_data = custom_app_data;
   }
   
   public String getAppData()
   {
      return app_data;
   }
      
   public void setLinkData(String lnk_data)
   {
      link_data = lnk_data;
   }
   
   public String getLinkData()
   {
      return link_data;
   }
   
   /**
    * Adds the given comma delimited string of app roles
    */
   public void setAppRoles(String appRoleString)
   {
      StringTokenizer tokenizer;
      
      if (appRoleString != null) {
         tokenizer = new StringTokenizer(appRoleString, ",");
         
         while (tokenizer.hasMoreTokens() == true)
            addAppRole(tokenizer.nextToken());
      }
   }
   
   /**
    * Determines the which app roles to add based on the given mask
    */
   public void setAppRoles(int rolesNumber)
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int mask = 4;
      int bitPosition = 1; // The third bit is "bit position" 1, i.e. the
                           //  lowest two bits are ignored
      
      try {
         if (progroup_title != null) {
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               Log.error("AppuserProgroupRoleItem", "setAppRoles",  
                         "\"null\" connection returned");
               return;
            }
            
            statement = connection.createStatement();
   
            for (int i = 0; i < Constants.MAX_NUM_APP_ROLES_PER_PROGROUP; i++) {
               
               if ((rolesNumber & mask) != 0) {
                  resultSet = statement.executeQuery("SELECT role_name FROM " +
                                   "app_roles WHERE app_title = '" + 
                                   progroup_title + "' AND role_number = " + 
                                   bitPosition);
                  
                  if (resultSet.next() == false)
                     Log.warn("AppuserProgroupRoleItem", "setAppRoles", 
                          "No role for " + progroup_title + " " + bitPosition);
                  else
                     addAppRole(resultSet.getString("role_name"));
                  
                  resultSet.close();
               }
               
               mask = mask << 1;
               bitPosition++;
            }
            
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
      }
      catch (Exception exception) {
         Log.excp("AppuserProgroupRoleItem", "setAppRoles", exception.toString());
      }
   }
   
   /**
    * Adds the given app role to the list of app roles
    */
   public void addAppRole(String role)
   {
      appRoles.add(role);
   }
   
   /**
    * Returns a list of app roles
    */
   public Enumeration getAppRoles()
   {
      return appRoles.elements();
   }
   
   /**
    * Returns all the app roles as a comma delimited string
    */
   public String getAppRoleString()
   {
      StringBuffer rolesString = new StringBuffer();
      
      for (int i = 0; i < appRoles.size(); i++)
         rolesString.append((String)appRoles.elementAt(i) + ", ");
      
      return rolesString.toString();
   }   
   
   /**
    * Returns a number representing a bit mask for all the app roles
    */
   public int getAppRolesNumber()
   {
      int rolesNumber = 0;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Enumeration appRoles;      
      String SQLString;
      Object items[];
      
      try {
         if (progroup_title != null) {
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               Log.error("AppuserProgroupRoleItem", "getAppRolesNumber",  
                         "\"null\" connection returned");
               return 0;
            }
            
            statement = connection.createStatement();
            
            appRoles = getAppRoles();
            items = new Object[2];
            items[1] = progroup_title;
            while (appRoles.hasMoreElements() == true) {               
               
               items[0] = (String)appRoles.nextElement();               
               SQLString = StringLib.SQLformat("SELECT role_number FROM " + 
                "app_roles WHERE role_name = " + Constants.ST + 
                " AND app_title = " + Constants.ST, items);
               resultSet = statement.executeQuery(SQLString);
               
               if (resultSet.next() == true)                              
                  rolesNumber |= ( 1 << (resultSet.getInt("role_number") + 1));
               
               resultSet.close();
            }
            
            // Close the database resources
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         
         return rolesNumber;
      }
      catch (Exception exception) {
         Log.excp("DskServer", "getAppRolesNumber", exception.toString());
         return 0;
      }
   }      
} 
