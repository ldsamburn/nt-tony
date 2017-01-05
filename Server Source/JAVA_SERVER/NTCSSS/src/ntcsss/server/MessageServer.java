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

import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;

import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.libs.Constants;
import ntcsss.libs.ErrorCodes;
import ntcsss.libs.StringLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.UserLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.DataLib;

import ntcsss.util.IntegerBuffer;

/**
 * Server to handle bulletin board messages
 */
public class MessageServer extends ServerBase
{
   private boolean isGetOwnBBMsg = false;
   private boolean isAddBulletinBoard;      
   
   private static char LF_REPLACEMENT = '\07';
   
   /** 
    * Constructor
    *  This should ALWAYS call the ServerBase class constructor
    */
   public MessageServer(String messageData, Socket socket, 
                        ServerState serverState) 
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
         serverClass = MessageServer.class;
           
         MessageHandlerDescription handlers[] = 
           {  new MessageHandlerDescription("SENDSYSBBMSG", 
                  serverClass.getMethod("sendSysBBMsg", null)),              
              new MessageHandlerDescription("SENDBBMSG",
                  serverClass.getMethod("sendBulletinBoardMessage", null)),
              new MessageHandlerDescription("SUBSCRIBEBB",
                  serverClass.getMethod("subscribeToBulletinBoard", null)),              
              new MessageHandlerDescription("GETSUBSCRIBEDBBS",
                  serverClass.getMethod("getSubscribedBulletinBoards", null)),              
              new MessageHandlerDescription("GETUNSUBSCRIBEDBBS",
                  serverClass.getMethod("getUnsubscribedBulletinBoards", null)),
              new MessageHandlerDescription("GETALLBBS", 
                  serverClass.getMethod("getAllBulletinBoards", null)),              
              new MessageHandlerDescription("GETOWNBBMSGS",                                    
                  serverClass.getMethod("getOwnBulletinBoardMessages", null)),
              new MessageHandlerDescription("GETBBMSGS", 
                  serverClass.getMethod("getBulletinBoardMessages", null)),                           
              new MessageHandlerDescription("MARKMSGREAD", 
                  serverClass.getMethod("markMessageRead", null)),
              new MessageHandlerDescription("GETNEWMSGFILE", 
                  serverClass.getMethod("getNewMsgFile", null)),                                       
              new MessageHandlerDescription("CREATEMSGFILE", 
                  serverClass.getMethod("createMsgFile", null)),              
              new MessageHandlerDescription("GETMSGFILE", 
                  serverClass.getMethod("getMsgFile", null)),              
              new MessageHandlerDescription("ADDMSGBB", 
                  serverClass.getMethod("addBulletinBoard", null)),                     
              new MessageHandlerDescription("DELMSGBB", 
                  serverClass.getMethod("removeBulletinBoard", null)),
              new MessageHandlerDescription("SERVERSTATUS", 
                  serverClass.getMethod("serverStatus", null)) };

         // Return the information concerning this class
         return new ServerDescription("Message Server", serverClass, handlers, 
                                      EntityName.MSG_SVR_GRP);
      }
      catch (Exception exception) {
         Log.excp("MessageServer", "getServerDescription", exception.toString());
         return null;
      }
   }
   
   /**
    * Adds the system message to the local database and then informs subscribers
    *  that are logged into the local host that a new message is ready to be
    *  read
    */
   public void sendSysBBMsg()
   {
      String fnct_name = "sendSysBBMsg";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, appname;
      int msg_type;
      String sys_msg;
      IntegerBuffer all_users = new IntegerBuffer();
      Connection connection;
      String bb_name;
      StringBuffer extension = new StringBuffer();
      StringBuffer clientAddress = new StringBuffer();
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      String messageToSend;
      String SQLString;
      
      try {
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }                  
                    
         // Parse the data from the client         
         dataItemLengths = new int[4];
         dataItems = new String[4];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_PROGROUP_TITLE_LEN;
         dataItemLengths[2] = 3;           
         dataItemLengths[3] = Constants.MAX_SYSTEM_MSG_LEN;
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         }         
         loginname = dataItems[0];
         appname = dataItems[1];
         msg_type = StringLib.parseInt(dataItems[2], -1);
         sys_msg = dataItems[3];
                      
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            errorCode = ErrorCodes.ERROR_CANT_ACCESS_DB;
            throw new GoTo();
         }                           
         
         // Insert the bulletin board message into the local database
         errorCode = insertSystemBBMsg(loginname, appname, msg_type, 
                                       sys_msg, extension, all_users,
                                       connection);                                            
         if (errorCode < 0) {
            DatabaseConnectionManager.releaseConnection(connection);
            throw new GoTo();
         }                  
                                           
         Log.debug("MessageServer", fnct_name, "System Message posted for " + 
                   appname);
         
         // Send message to appropriate users informing them of new message.
         bb_name = appname + "." + extension.toString();
         messageToSend = formatServerMsg("NEWBBMSGCAMEIN", bb_name);
         if (all_users.getValue() == 0) {    

            if (UserLib.isUserLoggedIn(loginname, clientAddress, connection) > 0) 
               SocketLib.sendFireAndForgetDatagram(clientAddress.toString(), 
                                                   Constants.UDP_CLT_PORT, 
                                                   messageToSend);               
            
            DatabaseConnectionManager.releaseConnection(connection);
            throw new GoTo();
         }
         else {            
            
            // Find all the user's that are logged in to this host that are
            //  subscribers to this bulletin board
            statement = connection.createStatement();
            sqlItems = new Object[1];
            sqlItems[0] = bb_name;
            SQLString = StringLib.SQLformat("SELECT client_address FROM " + 
              "current_users, bb_subscribers WHERE login_time != '0' AND " +
              "current_users.login_name = bb_subscribers.login_name AND " +
              "bb_name = " + Constants.ST, sqlItems);
            resultSet = statement.executeQuery(SQLString);
            
            // Send a message to all those users
            while (resultSet.next() == true) 
               SocketLib.sendFireAndForgetDatagram(
                                          resultSet.getString("client_address"), 
                                          Constants.UDP_CLT_PORT, 
                                          messageToSend);
            
            resultSet.close();
            statement.close();
            
            
            // NOTE: In the unix code, if execution ever got to this point, a
            //  response was not sent back to the client for some reason
         }
         
         DatabaseConnectionManager.releaseConnection(connection);
      }
      catch (GoTo goTo) {
         if (errorCode < 0) 
            sendErrorMessage("NOSENDSYSBBMSG", errorCode);         
         else 
            sendFormattedServerMsg("GOODSENDSYSBBMSG", "\0");
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Returns all the bulletin board names from the local host
    */
   public void getAllBulletinBoards()
   {
      String fnct_name = "getAllBulletinBoards";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token;
      IntegerBuffer uid = new IntegerBuffer();      
      String queryFilename = null;
      StringBuffer msg = new StringBuffer();
      boolean noSendError;
      File fileDescp;
    
      try {
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[2];
         dataItems = new String[2];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            
            // If the user is logged in, get a list of all the bulletin boards 
            //  on the local host
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {               
               
               queryFilename = Constants.NTCSS_LOGS_DIR + "/" + 
                               Thread.currentThread().getName() + "." + 
                               StringLib.getFullTimeStr() + ".query";                
                       
               errorCode = getAllBulBrds(loginname, queryFilename, msg);        
               if (errorCode >= 0) 
                  Log.debug("MessageServer", fnct_name, "Getting all bbs for " + 
                            loginname);
            }
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send response to client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETALLBBS", errorCode);         
         else 
            noSendError = sendServerMsgFile("GOODGETALLBBS", 
                                            msg.toString(), queryFilename,
                                            errorCode);
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");
         
         // Remove the query file
         if (queryFilename != null) {
            fileDescp = new File(queryFilename);
            if (fileDescp.exists() == true)
               fileDescp.delete();
         }
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Returns all the bulletin boards that the given user is not subscribed to
    */
   public void getUnsubscribedBulletinBoards()
   {
      String fnct_name = "getUnsubscribedBulletinBoards";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token;
      IntegerBuffer uid = new IntegerBuffer();
      String queryFilename = null;
      StringBuffer msg = new StringBuffer();
      File fileDescp;
      boolean noSendError;
               
      try {
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[2];
         dataItems = new String[2];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {
      
               queryFilename = Constants.NTCSS_LOGS_DIR + "/" + 
                               Thread.currentThread().getName() + "." + 
                               StringLib.getFullTimeStr() + ".query";
                              
               errorCode = getUnsubscribedBulBrds(loginname, queryFilename, msg);        
               if (errorCode >= 0) 
                  Log.debug("MessageServer", fnct_name, 
                            "Getting unsubscribed bbs for " + loginname);
            }
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send response to client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETUNSUBSCRBBS", errorCode);         
         else 
            noSendError = sendServerMsgFile("GOODGETUNSUBSCRBBS", 
                                            msg.toString(), queryFilename,
                                            errorCode);
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");
         
         // Remove the query file
         if (queryFilename != null) {
            fileDescp = new File(queryFilename);
            if (fileDescp.exists() == true)
               fileDescp.delete();
         }
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Returns a list of bulletin boards the given user is subscribed to
    */
   public void getSubscribedBulletinBoards()
   {
      String fnct_name = "getSubscribedBulletinBoards";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token;
      IntegerBuffer uid = new IntegerBuffer();
      String queryFilename = null;
      StringBuffer msg = new StringBuffer();
      File fileDescp;
      boolean noSendError;
               
      try {
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[2];
         dataItems = new String[2];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {
      
               queryFilename = Constants.NTCSS_LOGS_DIR + "/" + 
                               Thread.currentThread().getName() + "." + 
                               StringLib.getFullTimeStr() + ".query";
                              
               errorCode = getSubscribedBulBrds(loginname, queryFilename, msg);        
               if (errorCode >= 0) 
                  Log.debug("MessageServer", fnct_name, 
                            "Getting subscribed bbs for " + loginname);             
            }
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send response to client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETSUBSCRBBS", errorCode);         
         else 
            noSendError = sendServerMsgFile("GOODGETSUBSCRBBS", 
                                            msg.toString(), queryFilename,
                                            errorCode);
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");
         
         // Remove the query file
         if (queryFilename != null) {
            fileDescp = new File(queryFilename);
            if (fileDescp.exists() == true)
               fileDescp.delete();
         }
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }      
   }
   
   /**
    * Returns the bulletin board messages for the given user
    */
   public void getBulletinBoardMessages()
   {
      String fnct_name = "getBulletinBoardMessages";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token, bbName;
      IntegerBuffer uid = new IntegerBuffer();
      String queryFilename = null;
      StringBuffer msg = new StringBuffer();
      File fileDescp;
      boolean noSendError;
      
      try {
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_BB_NAME_LEN;         
         if ((isGetOwnBBMsg == false) &&                 
             (parseData(dataItemLengths, dataItems) == false)) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         }
         else if ((isGetOwnBBMsg == true) &&                  
                  (parseData(2, dataItemLengths, dataItems) == false)) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         }
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            bbName = dataItems[2];
            
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {
      
               queryFilename = Constants.NTCSS_LOGS_DIR + "/" + 
                               Thread.currentThread().getName() + "." + 
                               StringLib.getFullTimeStr() + ".query";
                   
               if (isGetOwnBBMsg == true)
                  errorCode = getOwnBulBrdMsgs(loginname, queryFilename, msg);        
               else
                  errorCode = getBulBrdMsgs(loginname, bbName, queryFilename, msg);        
                  
               if (errorCode >= 0) 
                  Log.debug("MessageServer", fnct_name, 
                            "Getting bulletin board messages for " + loginname);
            }
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
                
         // Send response to client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETBBMSGS", errorCode);         
         else 
            noSendError = sendServerMsgFile("GOODGETBBMSGS", 
                                            msg.toString(), queryFilename,
                                            errorCode);
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");         

         // Remove the query file
         if (queryFilename != null) {
            fileDescp = new File(queryFilename);
            if (fileDescp.exists() == true)
               fileDescp.delete();
         }
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }  
   }
   
   public void getOwnBulletinBoardMessages()
   {
      isGetOwnBBMsg = true;
      getBulletinBoardMessages();
   }
   
   /**
    * Create a name for a local file and return the name
    */
   public void getNewMsgFile()
   {
      String fnct_name = "getNewMsgFile";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token;
      IntegerBuffer uid = new IntegerBuffer();                 
      boolean noSendError;
      String filename = null;
      
      try {
         
         // Check if server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[2];
         dataItems = new String[2];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            
            // Validate the user
            if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
               errorCode = doServerValidate(loginname, token, 
                                            getClientAddress());
            else
               errorCode = UserLib.isUserValidLocally(loginname, token, uid);
            
            if (errorCode < 0)
               throw new GoTo();
            
            // Create a name for a file
            filename = Constants.NTCSS_LOGS_DIR + "/msgbb_" + loginname + "_" +                                
                               StringLib.getFullTimeStr() + ".new";
                              
            Log.debug("MessageServer", fnct_name, "Getting new msg file " +
                         "for " + loginname);            
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send response to client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETNEWMSGFILE", errorCode);         
         else 
            noSendError = sendFormattedServerMsg("GOODGETNEWMSGFILE", 
                                        StringLib.valueOf(filename.length(), 5) 
                                        + filename + "\0");
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }           
   }
   
   /**
    * Takes the contents of a file FTPed from the client and applies its
    *  bulletin board message data to the bb_messages table
    */
   public void createMsgFile()
   {
      String fnct_name = "createMsgFile";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token, msg_id, title;
      IntegerBuffer uid = new IntegerBuffer(); 
      String message;
      
      try {
         
         // Check if server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[4];
         dataItems = new String[4];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_MSG_ID_LEN;         
         dataItemLengths[3] = Constants.MAX_MSG_TITLE_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                        
         } 
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            msg_id = dataItems[2];
            title = dataItems[3];
            
            // Validate the user
            if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
               errorCode = doServerValidate(loginname, token, 
                                            getClientAddress());
            else
               errorCode = UserLib.isUserValidLocally(loginname, token, uid);
            
            if (errorCode < 0)
               throw new GoTo();
            
            // Create a new entry in the bb_messages table
            errorCode = addBulletinBoardMessage(msg_id, title);
            
            if (errorCode > 0)
               Log.debug("MessageServer", fnct_name,  "User " + loginname + 
                         " creating new msg " + msg_id + ".");                                          
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send response to client
         if (errorCode < 0) 
            sendErrorMessage("NOCREATEMSGFILE", errorCode);         
         else {
            message = "User successfully created new message.";
            sendFormattedServerMsg("GOODCREATEMSGFILE", 
                                        StringLib.valueOf(message.length(), 5) 
                                        + message);                  
         }
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }             
   }
   
   /**
    * Inserts the a bb_message_map entry for the given msg_id and then 
    *  sends a message to users logged in that are subscribers of the 
    *  bulletin board that a new message has arrived.     
    */
   public void sendBulletinBoardMessage()
   {
      String fnct_name = "sendBulletinBoardMessage";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];      
      IntegerBuffer uid = new IntegerBuffer();
      boolean noSendError;
      String message;
      StringBuffer clientAddress = new StringBuffer();
      Connection connection = null;
      Statement statement; 
      String loginname, token, msg_id, bb_name;
      String messageToSend;
      Object sqlItems[];
      String SQLString;
      ResultSet resultSet;
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[4];
         dataItems = new String[4];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_MSG_ID_LEN;         
         dataItemLengths[3] = Constants.MAX_BB_NAME_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;            
            throw new GoTo();
         }          
         loginname = dataItems[0];
         token = dataItems[1];
         msg_id = dataItems[2];
         bb_name = dataItems[3];
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            errorCode = ErrorCodes.ERROR_CANT_ACCESS_DB;
            throw new GoTo();
         }
         
         // Validate the user
         if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
            errorCode = doServerValidate(loginname, token, getClientAddress());
         else
            errorCode = UserLib.isUserValidLocally(loginname, token, uid);

         if (errorCode < 0) 
            throw new GoTo();         
                  
         // Insert an entry in the bb_message_map table for the given msg_id
         //  and bulletin board
         errorCode = insertMsgMapEntry(msg_id, bb_name, connection);
         if (errorCode < 0)
            throw new GoTo();
         
         Log.debug("MessageServer", fnct_name, "Message " + msg_id + 
                   " posted to " + bb_name);
         
         // Send message to appropriate users informing them of new message.
         messageToSend = formatServerMsg("NEWBBMSGCAMEIN", bb_name);     
         if (getBulBrdType(bb_name) == Constants.MSG_BB_TYPE_USER) {
                    
            if (UserLib.isUserLoggedIn(bb_name, clientAddress, connection) > 0)                            
               SocketLib.sendFireAndForgetDatagram(clientAddress.toString(), 
                                                   Constants.UDP_CLT_PORT, 
                                                   messageToSend)  ;                                                                       
         }
         else {              
         
            // Find all the user's that are logged in to this host that are
            //  subscribers to this bulletin board
            statement = connection.createStatement();
            sqlItems = new Object[1];
            sqlItems[0] = bb_name;
            SQLString = StringLib.SQLformat("SELECT client_address FROM " + 
              "current_users, bb_subscribers WHERE login_time != '0' AND " +
              "current_users.login_name = bb_subscribers.login_name AND " +
              "bb_name = " + Constants.ST, sqlItems);
            resultSet = statement.executeQuery(SQLString);
            
            // Send a message to all those users
            while (resultSet.next() == true)                 
               SocketLib.sendFireAndForgetDatagram(
                                         resultSet.getString("client_address"), 
                                         Constants.UDP_CLT_PORT, 
                                         messageToSend);
            
            resultSet.close();
            statement.close();            
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         if (connection != null)
            DatabaseConnectionManager.releaseConnection(connection);
         
         // Send a response back to the client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOSENDBBMSG", errorCode);                        
         else {
            message = "Bulletin board message successfully sent.";
            noSendError = sendFormattedServerMsg("GOODSENDBBMSG", 
                              StringLib.valueOf(message.length(), 5) + message + "\0");
         }
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Dumps the contents of the given message to a file and returns the
    *  name of the file to the client.
    */
   public void getMsgFile()
   {
      String fnct_name = "getMsgFile";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];      
      IntegerBuffer uid = new IntegerBuffer();
      boolean noSendError;      
      String loginname, token, msg_id;
      String msg_path = null;
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_MSG_ID_LEN;                  
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                      
         }    
         else {
            loginname = dataItems[0];
            token = dataItems[1];
            msg_id = dataItems[2]; 
            
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {      
               
               Log.debug("MessageServer", fnct_name, "Getting msg for " + 
                         loginname);
               
               // Dump the contents of the message to a file
               msg_path = createMsgDump(msg_id);
               if (msg_path == null) {
                  Log.error("MessageServer", fnct_name, "Error creating dump");
                  errorCode = ErrorCodes.ERROR_CANT_OPEN_FILE;
               }
            }
         }         
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOGETMSGFILE", errorCode);                        
         else             
            noSendError = sendFormattedServerMsg("GOODGETMSGFILE", 
                            StringLib.valueOf(msg_path.length(), 5) + msg_path);         
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Inserts a record in the bb_read_history table for the given message and
    *  user
    */
   public void markMessageRead()
   {
      String fnct_name = "markMessageRead";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];      
      IntegerBuffer uid = new IntegerBuffer();
      boolean noSendError;      
      String loginname, token, msg_id;      
      String message;
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_MSG_ID_LEN;                  
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                      
         }    
         else {
            loginname = dataItems[0];
            token     = dataItems[1];
            msg_id    = dataItems[2]; 
            
            // Validate the user
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {      
               
               // Mark the given message as "read"
               errorCode = markMsgRead(loginname, msg_id);
               if (errorCode >= 0) 
                  Log.debug("MessageServer", fnct_name, "Message " + msg_id + 
                            " read by " + loginname);
            }
         }         
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOMARKMSGREAD", errorCode);                        
         else { 
            message = "Bulletin board message successfully marked.";
            noSendError = sendFormattedServerMsg("GOODMARKMSGREAD", 
                            StringLib.valueOf(message.length(), 5) + message);         
         }
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Subscribes or unsubscribes the given user from the given bulletin board
    */
   public void subscribeToBulletinBoard()
   {
      String fnct_name = "subscribeToBulletinBoard";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];      
      IntegerBuffer uid = new IntegerBuffer();
      boolean noSendError;      
      String loginname, token, bb_name;      
      String message;
      boolean subscribe;      
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[4];
         dataItems = new String[4];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_BB_NAME_LEN;                  
         dataItemLengths[3] = 1;                  
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                      
         }    
         else {
            loginname = dataItems[0];
            token     = dataItems[1];
            bb_name   = dataItems[2]; 
            subscribe = (StringLib.parseInt(dataItems[3], 0) == 0) ? false : true;
            
            // Validate the user
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {      
               
               // Subscribe or unsubscribe the user to/from the given bulletin board
               if ((subscribe ? DataLib.subscribeToBulletinBoard(loginname, bb_name, null) :
                          DataLib.unsubscribeFromBulletinBoard(loginname, bb_name, null)) == true) {
                             
                  if (subscribe == true)
                     Log.debug("MessageServer", fnct_name, "User " + loginname + 
                               " subscribing to " + bb_name + ".");
                  else
                     Log.debug("MessageServer", fnct_name, "User " + loginname + 
                               " unsubscribing to " + bb_name + ".");
                  
                  errorCode = 0;
               }
               else
                  errorCode = ErrorCodes.ERROR_CANT_ACCESS_DB;
            }
         }         
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOSUBSCRIBEBB", errorCode);                        
         else { 
            message = "User successfully subscribed to bulletin board.";
            noSendError = sendFormattedServerMsg("GOODSUBSCRIBEBB", 
                            StringLib.valueOf(message.length(), 5) + message);         
         }
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }      
   }
   
   /**
    * Adds the given bulletin board
    */
   public void addBulletinBoard()
   {
      isAddBulletinBoard = true;
      changeBulletinBoard();
   }
   
   /**
    * Removes the given bulletin board
    */
   public void removeBulletinBoard()
   {
      isAddBulletinBoard = false;
      changeBulletinBoard();
   }
   
   /**
    * Either adds or removes the given bulletin board
    */
   private void changeBulletinBoard()
   {
      String fnct_name = "changeBulletinBoard";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];      
      IntegerBuffer uid = new IntegerBuffer();
      boolean noSendError;      
      String loginname, token, bb_name;      
      String message;      
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
            throw new GoTo();
         }
         
         // Get the data from the client
         dataItemLengths = new int[3];
         dataItems = new String[3];
         dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;         
         dataItemLengths[2] = Constants.MAX_BB_NAME_LEN;                                            
         if (parseData(dataItemLengths, dataItems) == false) {                       
            Log.error("MessageServer", fnct_name, 
                         "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                      
         }    
         else {
            loginname = dataItems[0];
            token     = dataItems[1];
            bb_name   = dataItems[2];             
            
            // Validate the user
            if ((errorCode = UserLib.isUserValidLocally(loginname, token, 
                                                        uid)) >= 0) {      
               
               // Add or remove the given bulletin board
               errorCode = changeBulBrd(loginname, bb_name, isAddBulletinBoard);
               if (errorCode >= 0) {               
                  if (isAddBulletinBoard == true)
                     Log.debug("MessageServer", fnct_name, "Adding msg bb <" + 
                               bb_name + ">.");
                  else
                     Log.debug("MessageServer", fnct_name, "Deleting msg bb <" +
                               bb_name + ">.");
               }
            }
         }         
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOCHANGEMSGBB", errorCode);                        
         else { 
            if (isAddBulletinBoard == true)
               message = "Bulletin board successfully added.";
            else
               message = "Bulletin board successfully deleted.";
               
            noSendError = sendFormattedServerMsg("GOODCHANGEMSGBB", 
                            StringLib.valueOf(message.length(), 5) + message);         
         }
         
         if (noSendError == false)
            Log.error("MessageServer", fnct_name, 
                      "Could not send message from server.");                  
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
      }      
   }
   
   /**
    * Inserts the given system bulletin board message into the local database
    */
   private int insertSystemBBMsg(String user, String app, int sys_bb_id,
                                     String sys_msg, StringBuffer ret_extension,
                                     IntegerBuffer allUsers,
                                     Connection connection)
   {
      String fnct_name = "insertSystemBBMsg";
      String item;
      String currentTime;
      Object sqlItems[];
      String SQLString;
      String msg_id;
      Statement statement;
      String title;
      
      try {
         
         // Create a title of the message
         ret_extension.delete(0, ret_extension.capacity());
         switch (sys_bb_id) {
    
            case Constants.SYS_MSG_BB_PROCESS_TERMINATED:
               item = "Server process ended";   // Titles must be <= 24 
               ret_extension.insert(0, "all");  
               allUsers.setValue(0);
               break;
            case Constants.SYS_MSG_BB_PRT_JOB_DELETED:
               item = "Print request deleted";
               ret_extension.insert(0, "all");               
               allUsers.setValue(0);
               break;
            case Constants.SYS_MSG_BB_PRT_JOB_RELEASED:
               item = "Print request released";
               ret_extension.insert(0, "all");               
               allUsers.setValue(0);
               break;
            default:
               item = "System message";
               ret_extension.insert(0, "adm");               
               allUsers.setValue(0);
               break;
         }                  
         title = StringLib.padString(user, 
                                     Constants.MAX_LOGIN_NAME_LEN, ' ') +  item;                    

         currentTime = StringLib.getFullTimeStr();
         msg_id = currentTime + user;

         // Replace all the newline characters since the database engine
         //  doesn't like newlines in a text field
         if (sys_msg != null)
            sys_msg = sys_msg.replace('\n', LF_REPLACEMENT);
            
         // Insert the message into the database 
         sqlItems = new Object[6];
         sqlItems[0] = msg_id;
         sqlItems[1] = currentTime;
         sqlItems[2] = Constants.BASE_NTCSS_USER;
         sqlItems[3] = title;
         sqlItems[4] = sys_msg;
         sqlItems[5] = new Boolean(false);
         SQLString = StringLib.SQLformat("INSERT INTO bb_messages (msg_id, " +
              "creation_time, login_name, msg_title, contents, sent) " + 
              "VALUES(" + Constants.ST + ", " + Constants.ST + ", " + 
              Constants.ST + ", " + Constants.ST + ", " + Constants.ST + 
              ", " + Constants.ST + ")", sqlItems);
         
         statement = connection.createStatement();
         statement.executeUpdate(SQLString);                  
         statement.close();         

         return insertMsgMapEntry(msg_id, app + "." + ret_extension, connection);
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Changes the sent field in the bb_messages table for the message from
    *  false to true and adds an entry in the bb_message_map table for
    *  the message if one doesn't already exist. 1 is returned if there are
    *  no errors. Otherwise, a negative value is returned.
    */   
   private int insertMsgMapEntry(String msg_id, String bulbrd, 
                                 Connection connection)
   {
      int returnVal;
      String fnct_name = "insertMsgMapEntry";      
      String SQLString;
      Statement statement;
      Object sqlItems[];
      ResultSet resultSet;

      try {
         
         // Check to see if the given bulletin board and the bulletin board
         //  message exists
         if ((returnVal = DataLib.doesBulletinBoardExist(bulbrd, connection)) < 0)
            return returnVal;
         if ((returnVal = doesMsgExist(msg_id, connection)) < 0)
            return returnVal;               
         
         statement = connection.createStatement();
      
         // Execute SQL to update the sent field to true          
         sqlItems = new Object[2];
         sqlItems[0] = new Boolean(true);
         sqlItems[1] = msg_id;
         SQLString = StringLib.SQLformat("UPDATE bb_messages SET sent = " +
              Constants.ST + " WHERE msg_id = " + Constants.ST, sqlItems);
         statement.executeUpdate(SQLString);            
            
         // Create a entry in the bb_message_map table if one doesn't exist
         sqlItems = new Object[2];
         sqlItems[0] = msg_id;
         sqlItems[1] = bulbrd;
         SQLString = StringLib.SQLformat("SELECT * FROM bb_message_map " + 
                  "WHERE msg_id = " + Constants.ST + " AND bb_name = " + 
                  Constants.ST, sqlItems);
         resultSet = statement.executeQuery(SQLString);   
         if (resultSet.next() == false) 
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
               "bb_message_map (msg_id, bb_name) VALUES (" + Constants.ST + 
               ", " + Constants.ST + ")", sqlItems));
         else
            resultSet.close();
         
         statement.close();
         
         return 1;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());         
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
      
   /**
    * Checks to see if the given bb_messages table record exists. Returns
    *  1 if it exists. Otherwise a negative integer is returned.
    */
   private int doesMsgExist(String msg_id, Connection connection)
   {
      String fnct_name = "doesMsgExist";   
      String SQLString;   
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      int retCode;

      try {

         // See if the given record exists in the bb_messages table 
         sqlItems = new Object[1];
         sqlItems[0] = msg_id;         
         SQLString = StringLib.SQLformat("SELECT * FROM bb_messages WHERE " + 
                        "msg_id = " + Constants.ST, sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         if (resultSet.next() == false)
            retCode = ErrorCodes.ERROR_BB_MSG_DOES_NOT_EXIST;
         else
            retCode = 1;
         
         resultSet.close();
         statement.close();
         
         return retCode;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Returns in the given a file, named by the given query string, a list
    *  of all the bulletin boards that don't equal the given login_name.
    */
   private int getAllBulBrds(String login_name, String query, StringBuffer msg)
   {
      String fnct_name = "getAllBulBrds";
      Object sqlItems[];
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      Connection connection;
      FileWriter writer;      
      int numBBs = 0;
      int length;
      String tmp1, tmp2;
      Object bundleItems[];
      
      try {
                  
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         sqlItems = new Object[1];
         sqlItems[0] = login_name;
         SQLString = StringLib.SQLformat("SELECT bb_name FROM bulletin_boards "
                     + "WHERE bb_name != " + Constants.ST, sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         // Create a writer in which to send output
         writer = new FileWriter(query, false);
         
         // Write the bundled data out to the file
         length = 0;
         bundleItems = new Object[1];
         while (resultSet.next() != false) {
            bundleItems[0] = resultSet.getString("bb_name");                        
            length += BundleLib.bundleQuery(writer, "C", bundleItems);
            numBBs++;
         }         
         
         // Close the system resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);         
         writer.close();
         
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(numBBs);
         tmp2 = String.valueOf(length + tmp1.length() + 1);
         tmp1 = String.valueOf(tmp2.length());
         msg.insert(0, tmp1);
         msg.append(tmp2);

         tmp2 = String.valueOf(numBBs);
         tmp1 = String.valueOf(tmp2.length());
         msg.append(tmp1);
         msg.append(tmp2);

         return length;   // Everthing ok, bulletin board data is returned 
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   }
   
   /**
    * Returns all the non-GENERAL type bulletin boards that the given user
    *  is not a part of. The results are stored in the specified file.
    */
   private int getUnsubscribedBulBrds(String login_name, String queryFilename,  
                                      StringBuffer msg)
   {
      String fnct_name = "getUnsubscribedBulBrds";
      Object sqlItems[];
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      Connection connection;            
      int length;
      String tmp1, tmp2;
      int num_unsub_bbs = 0;
      Object bundleItems[];
      FileWriter writer;
      Hashtable bbNames;
      String bbName;
      Enumeration list;
      
      try {
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         statement = connection.createStatement();
         
         // Get a list of all the bulletin boards
         resultSet = statement.executeQuery("SELECT bb_name FROM bulletin_boards");         
         bbNames = new Hashtable();
         while (resultSet.next() == true) {
            bbName = resultSet.getString("bb_name");
            if (getBulBrdType(bbName) != Constants.MSG_BB_TYPE_GENERAL)
               continue;
            
            bbNames.put(bbName, new Integer(1));
         }
         resultSet.close();
         
         // Get a list of all the bulletin boards the user is subscribed to
         sqlItems = new Object[1];
         sqlItems[0] = login_name;
         SQLString = StringLib.SQLformat("SELECT bb_name FROM bb_subscribers "
                     + "WHERE login_name = " + Constants.ST + 
                     " AND bb_name != login_name", sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);  
         
         while (resultSet.next() == true)  {
            bbName = resultSet.getString("bb_name");
            
            if (bbNames.contains(bbName))
               bbNames.put(bbName, new Integer(0));
         }
         
         // Close the system resources         
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);          
           
         // Create a writer in which to send output
         writer = new FileWriter(queryFilename, false);
         
         // Write the bundled data out to the file
         length = 0;
         bundleItems = new Object[1]; 
         list = bbNames.keys();
         while (list.hasMoreElements()) {
            
            bundleItems[0] = (String)list.nextElement();
            if (((Integer)bbNames.get(bundleItems[0])).intValue() != 0) {
                               
               length += BundleLib.bundleQuery(writer, "C", bundleItems);
               num_unsub_bbs++;
            }            
         }         
         
         // Close the file
         writer.close();
         
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(num_unsub_bbs);
         tmp2 = String.valueOf(length + tmp1.length() + 1);
         tmp1 = String.valueOf(tmp2.length());
         msg.insert(0, tmp1);
         msg.append(tmp2);

         tmp2 = String.valueOf(num_unsub_bbs);
         tmp1 = String.valueOf(tmp2.length());
         msg.append(tmp1);
         msg.append(tmp2);

         return length;   // Everthing ok, bulletin board data is returned 
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   }
   
   /**
    * Returns the type of the given bulletin board
    */
   private int getBulBrdType(String bulbrd)
   {
      boolean flag = false;      

      for (int i = 0; i < bulbrd.length(); i++) {
         
         if (Character.isUpperCase(bulbrd.charAt(i)) == true)
            flag = true;
         
         if (bulbrd.charAt(i) == '.')
            return Constants.MSG_BB_TYPE_SYSTEM;         
      }
      
      if (flag == true)
         return Constants.MSG_BB_TYPE_GENERAL;

      return Constants.MSG_BB_TYPE_USER;
   }
   
   /**
    * Bundles into the given file a list of the bulletin boards the 
    *  given user is subscribed to along with the number of unread messages
    *  in each bulletin board.
    */
   private int getSubscribedBulBrds(String login_name, String queryFilename, 
                                    StringBuffer msg)
   {
      String fnct_name = "getSubscribedBulBrds";
      Object sqlItems[];
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      Connection connection;            
      int length;
      String tmp1, tmp2;
      int numBBs = 0;
      Object bundleItems[];
      FileWriter writer;
      Vector bbList;
      Enumeration list;
            
      try {
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         sqlItems = new Object[1];
         sqlItems[0] = login_name;
         SQLString = StringLib.SQLformat("SELECT bb_name FROM bb_subscribers " +
                         "WHERE login_name = " + Constants.ST, sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);                  
         
         bbList = new Vector();
         while (resultSet.next() == true) 
            bbList.add(resultSet.getString("bb_name"));
         resultSet.close();         
         statement.close();         
           
         // Create a writer in which to send output
         writer = new FileWriter(queryFilename, false);
         
         // Write the bundled data out to the file
         length = 0;
         bundleItems = new Object[2]; 
         list = bbList.elements();
         while (list.hasMoreElements() == true) {
            bundleItems[0] = (String)list.nextElement();
            bundleItems[1] = new Integer(getNumUnreadMsgs(login_name, 
                                                       (String)bundleItems[0],
                                                       connection));
            
            length += BundleLib.bundleQuery(writer, "CI", bundleItems);
            numBBs++;                        
         }         
         
         // Close the system resources         
         DatabaseConnectionManager.releaseConnection(connection); 
         writer.close();
         
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(numBBs);
         tmp2 = String.valueOf(length + tmp1.length() + 1);
         tmp1 = String.valueOf(tmp2.length());
         msg.insert(0, tmp1);
         msg.append(tmp2);

         tmp2 = String.valueOf(numBBs);
         tmp1 = String.valueOf(tmp2.length());
         msg.append(tmp1);
         msg.append(tmp2);

         return length;   // Everthing ok, bulletin board data is returned 
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   }
   
   /**
    * Returns the number of messages read by the given user from the given
    *  bulletin board. If an error occurs, a negative integer is returned.
    */
   private int getNumUnreadMsgs(String login_name, String bulbrd,
                                Connection connection)
   {      
      String fnct_name = "getNumUnreadMsgs";      
      String SQLString;
      Object sqlItems[];
      Statement statement;
      ResultSet resultSet;
      int numUnread;

      try {
         
         // Check to make sure the bulletin board exists locally
         if (DataLib.doesBulletinBoardExist(bulbrd, connection) < 0)
            return -1;

         // Find how many messages have been read from the given bulletin board
         //  by the given user 
         sqlItems = new Object[3];
         sqlItems[0] = bulbrd;         
         sqlItems[1] = new Boolean(false);      
         sqlItems[2] = login_name;
         SQLString = StringLib.SQLformat("SELECT count(*) as cnt FROM " +
            "bb_message_map m, bb_read_history h WHERE m.bb_name = " + 
            Constants.ST + " AND " +
            "h.isRead = " + Constants.ST + 
            " AND m.msg_id = h.msg_id AND h.login_name = " + Constants.ST, 
            sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
         resultSet.next();
            
         numUnread = resultSet.getInt("cnt");
         
         resultSet.close();
         statement.close();
         
         return numUnread;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", "getNumUnreadMsgs", exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Bundles into the specified file all the messages in bb_messages that
    *  are for the given user
    */
   private int getOwnBulBrdMsgs(String login_name, String queryFilename, 
                                StringBuffer msg)
   {
      String fnct_name = "getOwnBulBrdMsgs";
      Object sqlItems[];
      String SQLString;
      Statement statement;
      ResultSet resultSet;
      Connection connection;            
      int length;
      String tmp1, tmp2;
      int numBBmsgs = 0;
      Object bundleItems[];
      FileWriter writer;
      
      try {
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         sqlItems = new Object[1];
         sqlItems[0] = login_name;
         SQLString = StringLib.SQLformat("SELECT msg_id, msg_title, sent FROM " 
                 + "bb_messages WHERE login_name = " + Constants.ST, sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);                  
           
         // Create a writer in which to send output
         writer = new FileWriter(queryFilename, false);
         
         // Write the bundled data out to the file
         length = 0;
         bundleItems = new Object[3];         
         while (resultSet.next() != false) {
            bundleItems[0] = resultSet.getString("msg_id");                                                                    
            bundleItems[1] = resultSet.getString("msg_title");
            bundleItems[2] = new Integer(resultSet.getBoolean("sent") ? 1:0);
            
            length += BundleLib.bundleQuery(writer, "CCI", bundleItems);
            numBBmsgs++;                        
         }         
         
         // Close the system resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection); 
         writer.close();
         
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(numBBmsgs);
         tmp2 = String.valueOf(length + tmp1.length() + 1);
         tmp1 = String.valueOf(tmp2.length());
         msg.insert(0, tmp1);
         msg.append(tmp2);

         tmp2 = String.valueOf(numBBmsgs);
         tmp1 = String.valueOf(tmp2.length());
         msg.append(tmp1);
         msg.append(tmp2);

         return length;   // Everthing ok, bulletin board data is returned 
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   }
   
   /**
    * Bundle into the specified file all the messages for the given 
    *  bulletin board and whether the given user has read those messages or not.
    */
   private int getBulBrdMsgs(String login_name, String bulbrd, 
                             String queryFilename, StringBuffer msg)
   {
      String fnct_name = "getBulBrdMsgs";
      Object sqlItems[];
      String SQLString;
      Statement statement;      
      ResultSet resultSet;      
      Connection connection;            
      int length;
      String tmp1, tmp2;
      int numBBmsgs = 0;
      Object bundleItems[];
      FileWriter writer;
      int errorCode;
      boolean is_sysbb = false;
      Hashtable msgList;
      Enumeration list;
      
      try {
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         // Make sure the given bulletin board exists
         if ((errorCode = DataLib.doesBulletinBoardExist(bulbrd, connection)) < 0)
            return errorCode;
         
         // Check to see if this is a "system" type bulletin board
         if (getBulBrdType(bulbrd) == Constants.MSG_BB_TYPE_SYSTEM)
            is_sysbb = true;
         
         // Get all the messages from the given bulletin board
         sqlItems = new Object[1];
         sqlItems[0] = bulbrd;
         SQLString = StringLib.SQLformat("SELECT m.msg_id, t.msg_title, " +
              "m.bb_name FROM bb_message_map m, bb_messages t WHERE bb_name " +
              "= " + Constants.ST + " AND m.msg_id = t.msg_id", sqlItems);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);  
         
         msgList = new Hashtable();
         while (resultSet.next() == true) 
            msgList.put(resultSet.getString("msg_id"),
                        resultSet.getString("msg_title"));         
         resultSet.close();
           
         // Create a writer in which to send output
         writer = new FileWriter(queryFilename, false);
         
         // Write the bundled data out to the file
         length = 0;
         bundleItems = new Object[3];                  
         sqlItems = new Object[2];
         sqlItems[1] = login_name;
         list = msgList.keys();
         while (list.hasMoreElements() == true) {
                                    
            bundleItems[0] = (String)list.nextElement();                                                                    
            bundleItems[1] = (String)msgList.get(bundleItems[0]);
            
            // Determine if the message has been read by the given user
            sqlItems[0] = bundleItems[0];
            SQLString = StringLib.SQLformat("SELECT isRead FROM " +
                         "bb_read_history WHERE msg_id = " + Constants.ST + 
                         " AND " + "login_name = " + Constants.ST, sqlItems);
            resultSet = statement.executeQuery(SQLString);
            
            if (resultSet.next() == true) 
               bundleItems[2] = new Integer(resultSet.getBoolean("isRead") ? 1:0);
            else
               bundleItems[2] = new Integer(0);
            
            resultSet.close();            
            
            // Bundle the data
            length += BundleLib.bundleQuery(writer, "CCI", bundleItems);
            numBBmsgs++;                        
         }         
         
         // Close the system resources                  
         statement.close();         
         DatabaseConnectionManager.releaseConnection(connection); 
         writer.close();
         
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(numBBmsgs);
         tmp2 = String.valueOf(length + tmp1.length() + 1);
         tmp1 = String.valueOf(tmp2.length());
         msg.insert(0, tmp1);
         msg.append(tmp2);

         tmp2 = String.valueOf(numBBmsgs);
         tmp1 = String.valueOf(tmp2.length());
         msg.append(tmp1);
         msg.append(tmp2);

         return length;   // Everthing ok, message data is returned 
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   } 
   
   /**
    * Takes the contents of a file that had been FTPed from the client and
    *  adds them to the bb_messages table. 1 is returned if successful.
    */
   private int addBulletinBoardMessage(String msg_id, String title)
   {
      String fnct_name = "addBulletinBoardMessage";
      String user;
      String ftpFilename;
      File fileDescp;
      FileReader reader;
      String SQLString;
      Connection connection;
      Statement statement;
      Object sqlItems[];
      String creationTime;
      char contents[];
      boolean finished;
      int offset, num_read, length;
      String contentsStr;
      
      try {
         creationTime = msg_id.substring(0, Constants.DATETIME_LEN - 1);
         user = msg_id.substring(Constants.DATETIME_LEN - 1);
         
         // Construct the name of the file that was FTPed from the client
         ftpFilename = Constants.NTCSS_LOGS_DIR + "/msgbb_" + user + "_" + 
                       creationTime + ".new";
         
         // Check to see if the file exists
         fileDescp = new File(ftpFilename);
         if (fileDescp.exists() == false) {
            Log.error("MessageServer", fnct_name, "Failed to access file " + 
                      ftpFilename);
            return ErrorCodes.ERROR_CANT_OPEN_FILE;
         }
         
         // Create a reader to read the contents of the file
         reader = new FileReader(ftpFilename);
         
         // Read the file contents
         finished = false;
         contents = new char[512 + 1];
         offset = 0; 
         length = contents.length;         
         do {
            if ((num_read = reader.read(contents, offset, length)) == -1)
               finished = true;
            else {
               offset += num_read;
               length = contents.length - offset;
            }            
         } while ((length != 0) && (finished == false));                  
         reader.close();
         
         // Delete the file
         fileDescp.delete();
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         // Replace all the newline characters since the database engine
         //  doesn't like newlines in a text field
         contentsStr = new String(contents, 0, offset);
         contentsStr = contentsStr.replace('\n', LF_REPLACEMENT);
         
         // Insert the message into the database 
         sqlItems = new Object[6];
         sqlItems[0] = msg_id;
         sqlItems[1] = creationTime;
         sqlItems[2] = user;
         sqlItems[3] = title;
         sqlItems[4] = contentsStr;
         sqlItems[5] = new Boolean(false);
         SQLString = StringLib.SQLformat("INSERT INTO bb_messages (msg_id, " +
              "creation_time, login_name, msg_title, contents, sent) " +
              "VALUES(" + Constants.ST + ", " + Constants.ST + "," + 
              Constants.ST + ", " + Constants.ST + ", " + Constants.ST + 
              ", " + Constants.ST + ")", sqlItems);
         statement = connection.createStatement();
         statement.executeUpdate(SQLString);
    
         // Close the database resources
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return 1;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }      
   
   /**
    * Creates a name of a temporary file and dumps the contents of the 
    *  given message to the file.
    */
   private String createMsgDump(String msg_id)
   {
      String fnct_name = "createMsgDump";
      String path;
      File fileDescp;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      FileWriter writer;
      String contentsStr;
      
      try {
         path = Constants.NTCSS_LOGS_DIR + "/" + msg_id + ".bbrd";
         
         // If the file doesn't exist, create it
         fileDescp = new File(path);
         if (fileDescp.exists() == false) {
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               Log.error("MessageServer", fnct_name, "null connection returned");
               return null;            
            }
            
            // Get the contents of the message
            sqlItems = new Object[1];
            sqlItems[0] = msg_id;
            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
                "contents FROM bb_messages WHERE msg_id = " + Constants.ST, 
                sqlItems));
            
            if (resultSet.next() == true) {
               
               contentsStr = resultSet.getString("contents");
               
               // Dump the contents to the file
               writer = new FileWriter(path, false);
               if (contentsStr != null) 
                  // Replace all the newline characters since the database engine
                  //  doesn't like newlines in a text field
                  writer.write(contentsStr.replace(LF_REPLACEMENT, '\n'));               
               writer.close();
            }
            else
               Log.error("MessageServer", fnct_name, "Msg contents not found " +
                         "for msg_id = <" + msg_id + ">");            
            
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         
         return path;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return null;
      }
   }
   
   /**
    * Inserts an entry in the bb_read_history table if one doesn't exist
    *  for the given message and user. If no errors occur, 1 is returned.
    */
   private int markMsgRead(String login_name, String msg_id)
   {
      String fnct_name = "markMsgRead";            
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];   
      int errorCode;
      
      try {         
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         // Check to make sure the message exists first
         if ((errorCode = doesMsgExist(msg_id, connection)) < 0) {
            DatabaseConnectionManager.releaseConnection(connection);
            return errorCode;
         }
            
         // See if a bb_read_history entry exists for the message and user
         sqlItems = new Object[2];
         sqlItems[0] = msg_id;   
         sqlItems[1] = login_name;
         statement = connection.createStatement();
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT * " +
                     "FROM bb_read_history WHERE msg_id = " + Constants.ST + 
                     " AND " + "login_name = " + Constants.ST, sqlItems));
         
         // If one doesn't exist, insert one
         if (resultSet.next() == false) {
            resultSet.close();
            sqlItems = new Object[3];
            sqlItems[0] = msg_id;
            sqlItems[1] = login_name;
            sqlItems[2] = new Boolean(true);
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                 "bb_read_history (msg_id, login_name, isRead) VALUES " +
                  "(" + Constants.ST + "," + Constants.ST + ", " + 
                  Constants.ST + ")", sqlItems));
         }
         else
            resultSet.close();
                                                               
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);         
         
         return 1;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }      
   }
      
   /**
    * Add or delete the given bulletin board based on the "isAdd" flag.
    *  If no errors occur, 1 is returned.
    */
   private int changeBulBrd(String creator, String bulbrd, boolean isAdd)
   {
      String fnct_name = "changeBulBrd";            
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];       
      int flag;
                           
      try {
         
         // Make sure the bulletin board name is valid
         if (isBBNameValid(bulbrd) == false)
            return ErrorCodes.ERROR_INVALID_BB_NAME;
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("MessageServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
            
         // Check to see if the bulletin board alread exists
         sqlItems = new Object[1];         
         sqlItems[0] = bulbrd;
         statement = connection.createStatement();
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT * "+
            "FROM bulletin_boards WHERE bb_name = " + Constants.ST, sqlItems));
         
         flag = 0;
         if (resultSet.next() == true)
            flag = 1;
         resultSet.close();
         
         if (isAdd == true) {  // Add the bulletin board
            
            if (flag == 1)
               flag = ErrorCodes.ERROR_BB_ALREADY_EXISTS;
            else {
               
               // Add the bulletin board
               sqlItems = new Object[2];         
               sqlItems[0] = bulbrd;
               sqlItems[1] = creator;
               statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                      "bulletin_boards (bb_name, creator) VALUES(" + 
                      Constants.ST + "," + Constants.ST + ")", 
                      sqlItems));
               
               flag = 1;
            }
         }
         else {  // Remove the bulletin board
            
            if (flag == 0) 
               flag = ErrorCodes.ERROR_BB_DOES_NOT_EXIST;
            else {
               
               // Remove the bulletin board
               sqlItems = new Object[1];         
               sqlItems[0] = bulbrd;               
//               statement.executeUpdate(StringLib.SQLformat("DELETE FROM " +
//                 "bulletin_boards WHERE bb_name = " + Constants.ST, sqlItems));
               DataLib.cascadeDelete(statement, "bulletin_boards", StringLib.SQLformat(
                 "bb_name = " + Constants.ST, sqlItems));               
            }
         }                  
         
         if ((isAdd == true) || (flag != 1)) {
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            return flag;
         }
                  
         // Remove all subscribers to the deleted bulletin board.
         //  This step is probably unecessary because when the bulletin board 
         //  is deleted, the DB should cascade this delete on down into the 
         //  bb_subscribers table. 
         // Remove the bulletin board
         sqlItems = new Object[1];         
         sqlItems[0] = bulbrd;               
//         statement.executeUpdate(StringLib.SQLformat("DELETE FROM " +
//                 "bb_subscribers WHERE bb_name = " + Constants.ST, sqlItems));                  
         DataLib.cascadeDelete(statement, "bb_subscribers", StringLib.SQLformat(
                                      "bb_name = " + Constants.ST, sqlItems));                           

         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         return flag;
      }
      catch (Exception exception) {
         Log.excp("MessageServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }
   
   /**
    * Returns true or false based on wether the given bulletin board name
    *  meets certain criteria.
    */
   private boolean isBBNameValid(String bulbrd)
   {
      boolean isValid = false; 
      char curChar;

      for (int i = 0; i < bulbrd.length(); i++) {
      
         curChar = bulbrd.charAt(i);
         
         if (Character.isUpperCase(curChar) == true)  
            isValid = true;
         else if (!Character.isLetterOrDigit(curChar) &&
                    (curChar != '_')  && 
                    (curChar != '-')  && 
                    (curChar != ' '))
            return false;        
      }

      return isValid;
   }
   
   class GoTo extends Throwable 
   {
      public GoTo()
      {
         super();
      }
   }
}
