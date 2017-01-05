/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.net.Socket;

import java.io.FileWriter;
import java.io.File;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.util.Enumeration;
import java.util.Vector;
import java.util.Arrays;

import ntcsss.libs.Constants;
import ntcsss.libs.ErrorCodes;
import ntcsss.libs.StringLib;
import ntcsss.libs.BundleLib;
import ntcsss.libs.DataLib;
import ntcsss.libs.SocketLib;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.util.IntegerBuffer;

/**
 *
 */
public class PrintQueServer extends ServerBase 
{
   private static boolean resendLists;
   
   /** 
    * Constructor
    *  This should ALWAYS call the ServerBase class constructor
    */
   public PrintQueServer(String messageData, Socket socket, 
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
         serverClass = PrintQueServer.class;
           
         MessageHandlerDescription handlers[] =          
           { new MessageHandlerDescription("NEWPRTQ", 
                  serverClass.getMethod("startNewQue", null)),                               
             new MessageHandlerDescription("RELPRTREQ", 
                  serverClass.getMethod("releaseQueItem", null)),                                        
             new MessageHandlerDescription("DELPRTREQ", 
                  serverClass.getMethod("deletePrtQueItem", null)),
             new MessageHandlerDescription("REFRESHPRTQ", 
                  serverClass.getMethod("refreshQue", null)),              
             new MessageHandlerDescription("PRTCHANGE", 
                  serverClass.getMethod("printerChanged", null)),
             new MessageHandlerDescription("SERVERSTATUS", 
                  serverClass.getMethod("serverStatus", null)) };

         // Return the information concerning this class
         return new ServerDescription("Print Que Server", serverClass, handlers, 
                                      EntityName.PRTQ_SVR_GRP);
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", "getServerDescription", exception.toString());
         return null;
      }
   }
      
   /**
    * Creates a new print queue monitor for a client
    */
   public void startNewQue()
   {
      String fnct_name = "startNewQue";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String prt_name = null;
      String loginname = null;
      String token = null;
      String appname = null;
      int role = 0;
      IntegerBuffer uid = new IntegerBuffer();
      String queryFilename = null;
      StringBuffer msg = new StringBuffer();  
      boolean noSendError;
      File fileDescp;
      PrintQueueInfo queueInfo;
      byte heartbeatMsg[];
      String heartbeatStr = "NTCSSHB";
      byte readBuffer[];
      boolean cont;
      boolean interrupted;
      Thread currentThread = Thread.currentThread();
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) 
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;            
         else {
         
            // Parse the data from the client         
            dataItemLengths = new int[5];
            dataItems = new String[5];
            dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[1] = Constants.MAX_TOKEN_LEN;
            dataItemLengths[2] = Constants.MAX_PRT_NAME_LEN;
            dataItemLengths[3] = Constants.MAX_PROGROUP_TITLE_LEN;
            dataItemLengths[4] = 1;
            if (parseData(dataItemLengths, dataItems) == false) {
               Log.error("PrintQueServer", fnct_name, 
                         "Could not parse incoming message");
               errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;                           
            } 
            else {
               loginname = dataItems[0];
               token     = dataItems[1];
               prt_name  = dataItems[2];
               appname   = dataItems[3];
               role      = StringLib.parseInt(dataItems[4], 0);
               
               // Validate the user
               if ((errorCode = isUserValid(loginname, token, uid)) >= 0) {              
                  
                  // Get a list of current print requests
                  queryFilename = Constants.NTCSS_LOGS_DIR + "/" +
                                  currentThread.getName() + "." + 
                                  StringLib.getFullTimeStr() + ".query";
                  
                  errorCode = getPrtRequestQueueList(prt_name, loginname,
                                                     appname, role,
                                                     queryFilename, msg);                  
                  if (errorCode >= 0)                  
                     Log.debug("PrintQueServer", fnct_name, "Sending prt " +
                             "request info for " + prt_name +  " role " + role);
               }
            }
         }
         
         // Send response to the client 
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOPRTREQS", errorCode);
         else
            noSendError = sendServerMsgFile("GOODPRTREQS", msg.toString(), 
                                            queryFilename, errorCode);
         
         // Delete the query file
         if (queryFilename != null) {
            fileDescp = new File(queryFilename);
            if (fileDescp.exists() == true)
               fileDescp.delete();
         }
         
         // Check to see if there was an error send the client response
         if (noSendError == false) {    
            Log.debug("PrintQueServer", fnct_name, "Process exiting because " +
                      "client is not responding.");
            return;     
         }
         
         // Store off information about this message handler instance
         queueInfo = new PrintQueueInfo(currentThread, prt_name); 
         PrintQueueList.add(queueInfo);
         
         
         // Create the heartbeat message
         heartbeatMsg = new byte[Constants.CMD_LEN];
         Arrays.fill(heartbeatMsg, (byte)0);  // Initialize to null
         System.arraycopy(heartbeatStr.getBytes(), 0, heartbeatMsg, 0,
                          heartbeatStr.length());

         // Continue in this loop, either sending heartbeats or print request
         //  lists, until the client goes away
         cont = true;
         readBuffer = new byte[Constants.CMD_LEN];
         while(cont) {
            
            interrupted = false;
            
            // Sleep for a certain period of time
            try {
               currentThread.sleep(10000);  // Milliseconds
            }
            catch (InterruptedException inter) {
               interrupted = true;
            }           
            
            // If this thread was interrupted, send an updated print request 
            //  list. Otherwise, send a heartbeat message
            if (interrupted == true) {
               Log.info("PrintQueServer", fnct_name, currentThread.getName() + 
                        ": Sending process list");
               
               // Process the interruption
               if (isResendList() == true) {
                  
                  if (resendPrtQList(prt_name, loginname, appname, role) == false)
                     break;
               }
               else                   
                  sendPrinterChangeMessage(prt_name);               
            }
            else {

               Log.debug("PrintQueServer", fnct_name, "Send hb for " + 
                         currentThread.getName());
                              
               // Send a heartbeat message 
               if (sendServerMsg(heartbeatMsg, Constants.CMD_LEN) == false)
                  break;
               
               // Make sure we get a response back from the client               
               if (getServerMsg(readBuffer, Constants.CMD_LEN, true) < 
                                                            Constants.CMD_LEN)
                  break;
            }
         } 
         
         Log.debug("PrintQueServer", fnct_name, 
                   "Process exiting because client is not responding");
         
         // Remove the information from the queue list
         PrintQueueList.remove(queueInfo);
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Causes all the queue monitor threads to resend their print request list
    */
   public void refreshQue()
   {
      String fnct_name = "refreshQue";
      int dataItemLengths[];
      String dataItems[];
   
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true)
            return;
           
         // Get the data from the client
         dataItemLengths = new int[1];
         dataItems = new String[1];
         dataItemLengths[0] = Constants.MAX_PRT_NAME_LEN;
         if (parseData(dataItemLengths, dataItems) == false) 
            Log.error("PrintQueServer", fnct_name, 
                         "Could not parse incoming message");            
         else {  
   
            // Force all the threads to resend their print queue lists
            forceListResend(dataItems[0]);
            
            Log.debug("PrintQueueServer", fnct_name, "Refreshing prt queue for " 
                      + dataItems[0] + ".");     
         }         
      }
      catch (Exception exception) {
         Log.excp("PrintQueueServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Causes all the queue monitor threads to send the "printer changed" message
    */
   public void printerChanged()
   {
      String fnct_name = "printerChanged";
      int dataItemLengths[];
      String dataItems[];
   
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true)
            return;
           
         // Get the data from the client
         dataItemLengths = new int[1];
         dataItems = new String[1];
         dataItemLengths[0] = Constants.MAX_PRT_NAME_LEN;
         if (parseData(dataItemLengths, dataItems) == false) 
            Log.error("PrintQueServer", fnct_name, 
                         "Could not parse incoming message");            
         else {  
   
            // Force all the threads to send the "printer changed" message
            forcePrtChangeSend(dataItems[0]);
            
            Log.debug("PrintQueueServer", fnct_name, "Prt " + dataItems[0] + 
                      " changed.");     
         }         
      }
      catch (Exception exception) {
         Log.excp("PrintQueueServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Removes from the print_requests table the record with the given request ID
    *  and sends a bulletin board message
    */
   public void releaseQueItem()
   {
      String fnct_name = "releaseQueItem";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token, printerName, requestID;  
      StringBuffer username = new StringBuffer();
      StringBuffer appname = new StringBuffer();
      StringBuffer masterHostname = new StringBuffer();
      boolean noSendError;
      String message;
      
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
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;
         dataItemLengths[2] = Constants.MAX_PRT_NAME_LEN;
         dataItemLengths[3] = Constants.MAX_REQUEST_ID_LEN;         
         if (parseData(dataItemLengths, 
                      dataItems) == false) {
            Log.error("PrintQueServer", fnct_name, 
                      "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;    
            throw new GoTo();
         } 
         loginname   = dataItems[0];
         token       = dataItems[1];
         printerName = dataItems[2];
         requestID   = dataItems[3];         
         
         /*
         run /NTCSSS/bin/release_req prt_name request_id
          
         if (bad exit code)
            errorCode = ERROR_CANT_REL_REQUEST;
         else { */
         
            // Delete the print request from the database        
            errorCode = deletePrtRequest(requestID, username, appname); 
            
            // Force all the print request queues to resend an updated print
            //  request list
            forceListResend(printerName);
            
            Log.debug("PrintQueServer", fnct_name,  "Releasing prt request " + 
                      requestID);
            
            // Send out appropriate bulletin board message.
            if (DataLib.getMasterServer(masterHostname) < 0)
               throw new GoTo();
            
            if (postSystemMessage(masterHostname.toString(), 
                                  username.toString(), appname.toString(), 
                                  "Print request " + requestID + 
                                  " released by " + loginname + ".",
                                  Constants.SYS_MSG_BB_PRT_JOB_RELEASED) < 0)
               Log.error("PrintQueServer", fnct_name, "Failed to send system " +
                         "message to " + username.toString()); 
         
         // }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0)
            noSendError = sendErrorMessage("NORELPRTREQ", errorCode);
         else {
            message = "Print request successfully released.\0";
            noSendError = sendFormattedServerMsg("GOODRELPRTREQ", 
                                        StringLib.valueOf(message.length(), 5) 
                                        + message);        
         }
         
          if (noSendError == false)
             Log.error("PrintQueServer", fnct_name, 
                       "Could not send message from server.");
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
      }
   }
   
   /**
    * Removes from the print_requests table the record with the given request ID
    *  and sends a bulletin board message
    */
   public void deletePrtQueItem()
   {
      String fnct_name = "deletePrtQueItem";
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginname, token, printerName, requestID;  
      StringBuffer username = new StringBuffer();
      StringBuffer appname = new StringBuffer();
      StringBuffer masterHostname = new StringBuffer();
      boolean noSendError;
      String message;
      IntegerBuffer uid = new IntegerBuffer();
      
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
         dataItemLengths[1] = Constants.MAX_TOKEN_LEN;
         dataItemLengths[2] = Constants.MAX_PRT_NAME_LEN;
         dataItemLengths[3] = Constants.MAX_REQUEST_ID_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("PrintQueServer", fnct_name, 
                      "Could not parse incoming message");
            errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;    
            throw new GoTo();
         } 
         loginname   = dataItems[0];
         token       = dataItems[1];
         printerName = dataItems[2];
         requestID   = dataItems[3];
         
         // Validate the user
         if ((errorCode = isUserValid(loginname, token, uid)) >= 0) {
                        
            // Delete the print request from the database        
            errorCode = deletePrtRequest(requestID, username, appname); 
            
            // Force all the print request queues to resend an updated print
            //  request list
            forceListResend(printerName);
            
            if (errorCode >= 0) {
               
               // delete the NTCSSS/spool/lp/"printerName"/cfA"requestID" file
               // delete the NTCSSS/spool/lp/"printerName"/dfA"requestID" file
               
               Log.debug("PrintQueServer", fnct_name, "Deleting prt request " + 
                         requestID);
               
               // Send out appropriate bulletin board message.
               if (DataLib.getMasterServer(masterHostname) < 0)
                  throw new GoTo();
            
               if (postSystemMessage(masterHostname.toString(), 
                                     username.toString(), appname.toString(),                                      
                                     "Print request " + requestID +                                      
                                     " deleted by " + loginname + ".",
                                     Constants.SYS_MSG_BB_PRT_JOB_DELETED) < 0)
                  Log.error("PrintQueServer", fnct_name, "Failed to send " + 
                            "system message to " + username.toString()); 
            }
         }
         
         throw new GoTo();
      }
      catch (GoTo goTo) {
         
         // Send a response back to the client
         if (errorCode < 0)
            noSendError = sendErrorMessage("NODELPRTREQ", errorCode);
         else {
            message = "Print request successfully deleted.\0";
            noSendError = sendFormattedServerMsg("GOODDELPRTREQ", 
                                        StringLib.valueOf(message.length(), 5) 
                                        + message);        
         }
         
          if (noSendError == false)
             Log.error("PrintQueServer", fnct_name, 
                       "Could not send message from server.");
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
      }      
   }
   
   /**
    * Forces all the print queues on this host to resend an updated print
    *  request list to their clients
    */
   private void forceListResend(String printerName)
   {
      // Set the "resend" flag
      setResendList();
      
      // Force the queues to wake up and respond to the flag
      updateClients(printerName);
   }
   
   /**
    * Forces all the print queues on this host to send a "printer changed"
    *  message to their clients 
    */
   private void forcePrtChangeSend(String printerName)
   {
      // Set the "send change message" flag
      setSendPrtChange();
      
      // Force the queues to wake up and respond to the flag
      updateClients(printerName);
   }
   
   /**
    * A set of wrapper functions to manage the state of a flag which is set
    *  before all the threads are interrupted
    */
   private synchronized void setResendList()
   {
      resendLists = true;
   }

   private synchronized void setSendPrtChange()
   {
      resendLists = false;
   }
   
   private synchronized boolean isResendList()
   {
      return resendLists;
   }
   
   /**
    * Bundles into the specified file the current print_request table 
    *  information. If no errors occured, the size of the file is returned.
    */
   private int getPrtRequestQueueList(String prt_name, String login_name,
                                      String app, int role, 
                                      String queryFilename, StringBuffer msg)
   {
      String fnct_name = "getPrtRequestQueueList";
      FileWriter writer;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];      
      int length = 0;
      int num_prt_reqs = 0;
      String tmp1, tmp2;
      Object bundleItems[];
           
      try {
         
         // Create a writer to the output file
         writer = new FileWriter(queryFilename, false);
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("PrintQueServer", fnct_name, "null connection returned");
            return ErrorCodes.ERROR_CANT_ACCESS_DB;            
         }
         
         // Get a list of the print requests from the database
         sqlItems = new Object[1];
         sqlItems[0] = prt_name;
         statement = connection.createStatement();
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
             "request_id, app_title, filename, login_name, time_of_request,  " +
             "security_class, size, orig_host, status FROM print_requests " +
             "WHERE printer_name=" + Constants.ST, sqlItems));
         
         // Bundle the print requests into the file
         bundleItems = new Object[10];         
         while (resultSet.next() == true) {            
            
            // Due to a SQL Server ODBC error, these fields must be pulled off
            //  in order
            bundleItems[0] = resultSet.getString("request_id");
            bundleItems[1] = resultSet.getString("app_title");
            bundleItems[2] = resultSet.getString("filename");
            bundleItems[3] = resultSet.getString("login_name");
            bundleItems[4] = prt_name; 
            bundleItems[5] = resultSet.getString("time_of_request");
            bundleItems[6] = new Integer(resultSet.getInt("security_class"));
            bundleItems[7] = new Integer(resultSet.getInt("size"));
            bundleItems[8] = resultSet.getString("orig_host");
            bundleItems[9] = new Integer(resultSet.getInt("status"));                        
            
            switch (role) {
               case Constants.NTCSS_USER_ROLE:
                  if ((login_name.equals((String)bundleItems[3]) == true) &&
                      (app.equals((String)bundleItems[1]) == true)) {
                     length += BundleLib.bundleQuery(writer, "CCNCCDIICI", 
                                                     bundleItems);  
                     num_prt_reqs++;
                  }

                  break;
                  
               case Constants.APP_ADMIN_ROLE:
                  if (app.equals((String)bundleItems[1]) == true) {
                     length += BundleLib.bundleQuery(writer, "CCNCCDIICI", 
                                                     bundleItems);  
                     num_prt_reqs++;
                  }
                  
                  break;
                  
               case Constants.NTCSS_ADMIN_ROLE:
                  length += BundleLib.bundleQuery(writer, "CCNCCDIICI", 
                                                  bundleItems);  
                  num_prt_reqs++;
                     
                  break;
            }
         }
         
         // Close the system resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         writer.close();
         
         // Store the count information in the given message buffer
         msg.delete(0, msg.capacity());
         tmp1 = String.valueOf(num_prt_reqs);
         tmp2 = String.valueOf(tmp1.length());
         msg.insert(0, tmp2);
         msg.append(tmp1);

         return length;   // Everthing ok, printer queue data is returned.
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      } 
   }
   
   /**
    * Bundles all the current print request list and sends the information
    *  to the client
    */
   private boolean resendPrtQList(String prt_name, String loginname, 
                                  String appname, int role)
   {
      String fnct_name = "resendPrtQList";
      String queryFilename = null;
      int errorCode = 0;
      StringBuffer msg = new StringBuffer();
      boolean noSendError;
      File fileDescp;
      
      try {
         queryFilename = Constants.NTCSS_LOGS_DIR + "/" +  
                             Thread.currentThread().getName() + "." + 
                             StringLib.getFullTimeStr() + ".query"; 
         
         errorCode = getPrtRequestQueueList(prt_name, loginname, appname, role,
                                            queryFilename, msg);
         if (errorCode >= 0)
            Log.debug("PrintQueServer", fnct_name, "Resending prt request " + 
                      "info for " + prt_name + " with role " + role + " " + 
                      msg.toString());
         
         if (errorCode < 0) 
            noSendError = sendErrorMessage("NOPRTREQS", errorCode);
         else 
            noSendError = sendServerMsgFile("GOODPRTREQS", msg.toString(), 
                                            queryFilename, errorCode);
                  
         // Delete the file
         fileDescp = new File(queryFilename);
         if (fileDescp.exists() == true)
            fileDescp.delete();
         
         if (noSendError == false) {
            Log.debug("PrintQueServer", fnct_name, 
                      "Process exiting because client is not responding.");
            return false;
         }
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Sends a message to the connected client
    */
   private void sendPrinterChangeMessage(String prt_name)
   {
      String fnct_name = "sendPrinterChange";
      String messageToSend;
      
      try {
         Log.debug("PrintQueServer", fnct_name, "Sending prt change message.");                           
         
         // Send the message
         messageToSend = "Printer " + prt_name + " has been reconfigured.";                     
         if (sendFormattedServerMsg("PRTCHANGE", 
                                    StringLib.valueOf(messageToSend.length(), 5) 
                                    + messageToSend) == false)
            Log.error("PrintQueServer", fnct_name,  
                      "Could not send message from server.");
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());         
      }
   }
   
   /**
    * Interrupts all the threads that have print queue clients listening
    *  to them. This interruption will force them to resend print request
    *  data to their corresponding client.
    */
   private void updateClients(String printerName)
   {
      Enumeration list;
      PrintQueueInfo queueInfo;
      
      try {
   
         // Get the list of threads which have print queue clients listening
         //  to them
         list = PrintQueueList.getList();
         
         while (list.hasMoreElements() == true) {
            queueInfo = (PrintQueueInfo)list.nextElement();
                      
            if ((queueInfo.getPrinterName()).equals(printerName))                
               (queueInfo.getThreadInstance()).interrupt();
         }
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", "updateClients", exception.toString());
      }
   }
   
   /**
    * Deletes the print_request table record that has the given requestID.
    *  The "login_name" and "app_title" fields of the records are stored in
    *  the given buffers.
    */
   private int deletePrtRequest(String requestID, StringBuffer username, 
                                StringBuffer appname)
   {
      String fnct_name = "deletePrtRequest";
      Connection connection;
      Statement statement;
      ResultSet resultSet;            
      Object sqlItems[];
      
      try {
                  
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("PrintQueServer", fnct_name,  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
         }

         // Check to see if the print request exists
         sqlItems = new Object[1];
         sqlItems[0] = requestID;         
         statement = connection.createStatement();
         resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
           "app_title, login_name FROM print_requests WHERE request_id = " + 
           Constants.ST, sqlItems));
           
         if (resultSet.next() == true) {
            
            // Clear the given buffers
            username.delete(0, username.capacity());
            appname.delete(0, appname.capacity());
            
            // Store the login name and app name in the given buffers
            appname.insert(0, resultSet.getString("app_title"));
            username.insert(0, resultSet.getString("login_name"));
            
            resultSet.next();
            
            // Delete the record      
            statement.executeUpdate(StringLib.SQLformat("DELETE FROM " +
                 "print_requests WHERE request_id=" + Constants.ST, sqlItems));
            
         }
         else {
            resultSet.close();
            Log.error("PrintQueServer", fnct_name, "Print request " + 
                      requestID + " does not exist");
         }
         
         // Close the database resources         
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return 0;
      }
      catch (Exception exception) {
         Log.excp("PrintQueServer", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_DELETE_VALUE;
      }
   }
   
   class GoTo extends Throwable
   {
      public GoTo()
      { 
         super();
      }
   }
}


/**
 * Contains information on each print queue created
 */
class PrintQueueList
{
   private static Vector list = new Vector();
   
   /**
    * Adds the given information to the list
    */
   public static synchronized void add(PrintQueueInfo info)
   {
      list.add(info);
   }
   
   /**
    * Returns a list of the information
    */
   public static synchronized Enumeration getList()
   {
      return list.elements();
   }
   
   /**
    * Removes the given information from the list
    */
   public static synchronized void remove(PrintQueueInfo info)
   {
      list.remove(info);
   }

   /**
    * Dumps the information in the list
    */
   public static void dump()
   {
      Enumeration currentList;
      PrintQueueInfo info;

      try {
         currentList = getList();

         while(currentList.hasMoreElements()) {
            info = (PrintQueueInfo)currentList.nextElement();
            Log.info("PrintQueueList", "dump", 
                     (info.getThreadInstance()).getName());
         }
      }
      catch (Exception exception) {
         Log.excp("PrintQueueList", "dump", exception.toString());
      }
   }
}

/**
 * This represents information obtained when a NEWPRTQ message is received
 */
class PrintQueueInfo
{
   private Thread threadInstance;
   private String printer_name;   
   
   public PrintQueueInfo(Thread threadInstance, String printer_name)
   {
      try {
         this.threadInstance = threadInstance;
         this.printer_name = printer_name;         
      }
      catch (Exception exception) {
         Log.excp("PrintQueueInfo", "PrintQueueInfo", exception.toString());
      }
   }
   
   public String getPrinterName()
   {
      return printer_name;
   }
   
   public Thread getThreadInstance()
   {
      return threadInstance;
   }
}