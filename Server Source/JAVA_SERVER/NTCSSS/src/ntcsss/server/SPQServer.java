/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

/*
 * This class contains all the message handlers for the Server Process Queue
 *  server
 */

package ntcsss.server;

import java.net.Socket;

import java.util.Vector;
import java.util.Enumeration;

import java.io.File;
import java.io.BufferedOutputStream;

import java.util.Arrays;

import ntcsss.log.EntityName;
import ntcsss.log.Log;

import ntcsss.util.IntegerBuffer;

import ntcsss.libs.Constants;
import ntcsss.libs.ErrorCodes;
import ntcsss.libs.SvrProcLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.structs.SvrProcessFilter;

public class SPQServer extends ServerBase
{
   private boolean newQue = true;
   private boolean refreshSchedJobQueue = false;
   
   /**
    * This should ALWAYS call the ServerBase class constructor
    */
   public SPQServer(String messageData, Socket socket, ServerState serverState)
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
         serverClass = SPQServer.class;
  
         MessageHandlerDescription handlers[] = 
           { new MessageHandlerDescription("NEWSPQ", 
                          serverClass.getMethod("startNewSPQue", null)),
             new MessageHandlerDescription("SVRPROCLIST", 
                          serverClass.getMethod("getServerProcessList", null)),             
             new MessageHandlerDescription("REMOVESPQITEM", 
                          serverClass.getMethod("removeSPQueItem", null)),                                   
             new MessageHandlerDescription("ALTERSPQITEM", 
                          serverClass.getMethod("alterSPQueItem", null)), 
             new MessageHandlerDescription("CHANGECUSTPROCSTAT", 
                          serverClass.getMethod("changeCustomStatus", null)),
             new MessageHandlerDescription("REFRESHSPQ", 
                          serverClass.getMethod("refreshSPQue", null)),
             new MessageHandlerDescription("REFRESHSCHEDJOBQ", 
                          serverClass.getMethod("refreshSchedJobQue", null)), 
             new MessageHandlerDescription("SERVERSTATUS", 
                          serverClass.getMethod("serverStatus", null))  };

         // Return the information concerning this class
         return new ServerDescription("Server Process Queue Server", serverClass,
                                      handlers, EntityName.SPQ_SVR_GRP);
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "getServerDescription", exception.toString());
         return null;
      }
   }
   
   /**
    * Starts a new queue
    */
   public void startNewSPQue()
   {
      int errorCode = 0;
      boolean quit;
      int dataItemLengths[];
      String dataItems[];
      Thread currentThread = Thread.currentThread();
      QueueInfo queueInfo;
      String loginName = null;
      String token;
      String progroupTitle = null;      
      int role = 0;
      int isScheduledJob = 0;
      int filterMask = 0;
      int priority = 0;
      String queryFilename = null;
      StringBuffer messageData= new StringBuffer();
      SvrProcessFilter processFilter= null;
      boolean interrupted;
      boolean sendError;
      boolean cont;
      int fileSize= 0;
      String timeString = null;
      byte readBuffer[];
      File fileDesc;
      byte heartbeatMsg[];
      String heartbeatStr = "NTCSSHB";
      IntegerBuffer uid = new IntegerBuffer();
      
      try {               

         // Check to see if this server is idled
         if (isServerIdled() == true)
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
         
         if (errorCode == 0) {
        
            // Get the client data
            dataItemLengths = new int[14];
            dataItems = new String[14];

            dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
            dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN;
            dataItemLengths[3]  = 1; // access role
            dataItemLengths[4]  = 1; // schedule server process flag
            dataItemLengths[5]  = 6; // filter mask
            dataItemLengths[6]  = 2; // prioritiy
            dataItemLengths[7]  = Constants.MAX_PROC_CMD_LINE_LEN;
            dataItemLengths[8]  = Constants.MAX_CUST_PROC_STAT_LEN;
            dataItemLengths[9]  = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[10] = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[11] = Constants.MAX_PROGROUP_TITLE_LEN;
            dataItemLengths[12] = Constants.MAX_JOB_DESCRIP_LEN;
            dataItemLengths[13] = Constants.MAX_ORIG_HOST_LEN;
            
            if (parseData(dataItemLengths, dataItems) == false) {
               Log.error("SPQServer", "startNewSPQue", 
                         "Could not parse incoming message");
               errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;
            }
            else {
                              
               loginName      = dataItems[0];
               token          = dataItems[1];               
               progroupTitle  = dataItems[2];                    
               role           = StringLib.parseInt(dataItems[3], role);               
               isScheduledJob = StringLib.parseInt(dataItems[4], isScheduledJob);               
               filterMask     = StringLib.parseInt(dataItems[5], filterMask);               
               priority       = StringLib.parseInt(dataItems[6], priority);               
               processFilter  = new SvrProcessFilter(dataItems[7], dataItems[8], 
                                                    dataItems[9], dataItems[10], 
                                                   dataItems[11], dataItems[12], 
                                                   dataItems[13], priority);               
               
               if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
                  errorCode = 
                        doServerValidate(loginName, token, getClientAddress());
               else
                  errorCode = isUserValid(loginName, token, uid);
               
               if (errorCode >= 0) {
                  
                  // Create a filename
                  timeString = StringLib.getFullTimeStr();
                  queryFilename = Constants.NTCSS_LOGS_DIR + "/" +
                                  currentThread.getName() + "." + timeString + 
                                   ".query";                   
                  
                  // Get a list of processes based on the filter data and write
                  //  the data out to the file
                  errorCode = SvrProcLib.getServerProcessQueueList(progroupTitle, 
                                                       loginName, 
                                                       role, queryFilename, 
                                                       messageData, 
                                                       isScheduledJob, 
                                                       filterMask, 
                                                       processFilter); 
                  
                  if (errorCode >= 0)
                     Log.debug("SPQServer", "startNewSPQue", 
                               "Sending server process info for " + 
                               progroupTitle + " role " + role);                                      
               }   
            }
         } 
         
         // Check no errors have occurred, send the query data         
         if (errorCode >= 0) {
            quit = !sendServerMsgFile("GOODSPQUE", messageData.toString(), 
                                      queryFilename, errorCode);
            
            // Delete the query file
            fileDesc = new File(queryFilename);
            if (fileDesc.exists() == true)
               fileDesc.delete();            
         }
         else
            quit = !sendErrorMessage("NOSPQUE", errorCode);         
         
         // If this is not a NEWSPQ message, exit
         if (newQue == false)
            return;
        
         if (quit) {
            Log.debug("SPQServer", "startNewSPQue", 
                      "Process exiting because client is not responding!");
            return;
         }         

         // Store off information about this message handler instance
         queueInfo = new QueueInfo(currentThread, progroupTitle, 
                                   isScheduledJob); 
         QueueList.add(queueInfo);

         // Create the heartbeat message
         heartbeatMsg = new byte[Constants.CMD_LEN];
         Arrays.fill(heartbeatMsg, (byte)0);  // Initialize to null
         System.arraycopy(heartbeatStr.getBytes(), 0, heartbeatMsg, 0,
                          heartbeatStr.length());

         // Continue in this loop, either sending heartbeats or process lists,
         //  until the client goes away
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
            
            // If this thread was interrupted, send an updated process list.
            //  Otherwise, send a heartbeat message
            if (interrupted == true) {
               Log.info("SPQServer", "startNewSPQue", currentThread.getName() + 
                        ": Sending process list");
               
               // Try to send re-queried server process data
               if (resendSPQList(progroupTitle, loginName, role, isScheduledJob,
                                 filterMask, processFilter) == false)
                  break;
            }
            else {

               Log.debug("SPQServer", "startNewSPQue", "Send hb for " + 
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
         
         Log.debug("SPQServer", "startNewSPQue", 
                   "Process exiting because client is not responding");
         
         // Remove the information from the queue list
         QueueList.remove(queueInfo);         
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "startNewSPQue", exception.toString());
      }
   } 
   
   public void getServerProcessList()
   {              
      newQue = false;
      startNewSPQue();
   }

   /**
    * Attempts to delete the given process information from the database
    */
   public void removeSPQueItem()
   {
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginName = null;
      String token;      
      int isScheduledJob;
      String progroupTitle = null;
      String processId = null;
      boolean sendError;
      IntegerBuffer uid = new IntegerBuffer();

      try {
               
         // Check to see if the server is idled
         if (isServerIdled() == true)
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;

         if (errorCode >= 0) {

            // Get the client data
            dataItemLengths = new int[5];
            dataItems = new String[5];

            dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
            dataItemLengths[2]  = Constants.MAX_PROCESS_ID_LEN;
            dataItemLengths[3]  = 2; 
            dataItemLengths[4]  = Constants.MAX_PROGROUP_TITLE_LEN;                                                

            if (parseData(dataItemLengths, dataItems) == false) {
               Log.error("SPQServer", "removeSPQueItem", 
                         "Could not parse incoming message");
               errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;
            }
            else {

               loginName = dataItems[0];
               token = dataItems[1];
               processId = dataItems[2];
               isScheduledJob = StringLib.parseInt(dataItems[3], 0);
               progroupTitle = dataItems[4];

               if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
                  errorCode = 
                        doServerValidate(loginName, token, getClientAddress());
               else
                  errorCode = isUserValid(loginName, token, uid);

               if (errorCode >= 0) {

                  // Try to delete the given process from the database
                  errorCode = SvrProcLib.deleteServerProcess(processId,
                                                             progroupTitle,
                                                             isScheduledJob,
                                                             false);

                  if (errorCode >= 0) {
                     updateClients(progroupTitle, isScheduledJob);
                     Log.debug("SPQServer", "removeSPQueItem", 
                               "Removing process " + processId);
                  }
               }
            }
         }

         // Send a message back to the client
         if (errorCode < 0)
            sendError = !sendErrorMessage("NOREMOVESPQITEM", errorCode);
         else
            sendError = !sendFormattedServerMsg("GOODREMOVESPQITEM",
                                        StringLib.valueOf(29, 5) + 
                                        "Process successfully removed.");

         if (sendError == true)
            Log.error("SPQServer", "removeSPQueItem",
                      "Could not send message from server.");

         if (errorCode >= 0)
            SvrProcLib.notifyAppCallback(progroupTitle, "NOTRUN", 0, processId, 
                                         loginName);
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "removeSPQueItem", exception.toString());
      }
   }

   /**
    * Alters the information in the database for a particular server process
    */
   public void alterSPQueItem()
   {
      int errorCode = 0;
      int dataItemLengths[];
      String dataItems[];
      String loginName;
      String token;      
      int isScheduledJob;
      int mask;
      int pid;
      int priority;
      String processId;
      String custProcStatus;
      int whichPriority;
      String launchTime = null;              
      boolean noError;
      StringBuffer progroupTitle = new StringBuffer();
      IntegerBuffer uid = new IntegerBuffer();
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true)
            errorCode = ErrorCodes.ERROR_SERVER_IDLED;
         
         if (errorCode >= 0) {
            
            // Get the client data
            dataItemLengths = new int[8];
            dataItems = new String[8];

            dataItemLengths[0] = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[1] = Constants.MAX_TOKEN_LEN;
            dataItemLengths[2] = Constants.MAX_PROCESS_ID_LEN;
            dataItemLengths[3] = 2;
            dataItemLengths[4] = 6;
            dataItemLengths[5] = Constants.MAX_CUST_PROC_STAT_LEN;
            dataItemLengths[6] = 2;
            dataItemLengths[7] = 2;

            if (parseData(dataItemLengths, dataItems) == false) {
               Log.error("SPQServer", "alterSPQueItem", 
                         "Could not parse incoming message");
               errorCode = ErrorCodes.ERROR_CANT_PARSE_MSG;
            }
            else {
               loginName = dataItems[0];
               token = dataItems[1];
               processId = dataItems[2];
               isScheduledJob = StringLib.parseInt(dataItems[3], 0);
               mask = StringLib.parseInt(dataItems[4], 0);
               custProcStatus = dataItems[5];
               pid = StringLib.parseInt(dataItems[6], 0);
               priority = StringLib.parseInt(dataItems[7], 0);
               
               Log.debug("SPQServer", "alterSPQueItem", "Received process id <" + 
                 processId + "> scheduled job <" + isScheduledJob + "> mask <" + 
                 mask + "> custom status <" + custProcStatus + "> pid <" +
                 pid + "> priority <" + priority + ">");
               
               errorCode = isUserValid(loginName, token, uid);
               if (errorCode >= 0) {
                  
                  if (isScheduledJob != 0)
                     whichPriority = Constants.SCHEDULED_PRIORITY;
                  else
                     whichPriority = Constants.LOW_PRIORITY;
                  
                  if ((mask & Constants.CHANGE_PRIORITY) != 0) {
                     mask |= Constants.CHANGE_LAUNCH_TIME;
                     launchTime = StringLib.getFullTimeStr();
                  }
                  
                  Log.info("SPQServer", "alterSPQueItem", 
                           "Changing process <" + processId + ">");
                  
                  errorCode = SvrProcLib.changeServerProcess(processId, pid, 
                                                             mask, -1, priority,
                                                             custProcStatus, 
                                                             launchTime, null, 
                                                             null, 
                                                             progroupTitle, 0, 
                                                             whichPriority);
                  if (errorCode < 0) 
                     Log.error("SPQServer", "alterSPQueItem", 
                               "Failed to alter job <" + processId + ">");
                  else {
                     
                     updateClients(progroupTitle.toString(), isScheduledJob);
                     Log.debug("SPQServer", "alterSPQueItem", 
                               "Altering process " + processId);
                     
                     // Signal command server to check waiting processes
                     if (isScheduledJob == 0) {
                                                  
                        if (SocketLib.sendFireForgetMessage(
                                                     StringLib.getHostname(), 
                                                     Constants.CMD_SVR_PORT,
                                                     StringLib.formatMessage("CHECKWAITSVRPROCS", 
                                                     StringLib.padString(progroupTitle.toString(), 
                                                      Constants.MAX_PROGROUP_TITLE_LEN, ' '))) == false)                        
                           Log.error("SPQServer", "alterSPQueItem", 
                                   "Failed to send message to command server");                        
                     }
                  }
               }
            }
         }
         
         // Send response 
         if (errorCode < 0)
            noError = sendErrorMessage("NOALTERSPQITEM", errorCode);                                       
         else if (errorCode == 1) // Indicates "run next job" record already taken 
            noError = sendFormattedServerMsg("NONEXTALTERSPQITEM", 
                                           StringLib.valueOf(29, 5) + 
                                           "Process successfully altered.");
         else
            noError = sendFormattedServerMsg("GOODALTERSPQITEM", 
                                            StringLib.valueOf(29, 5) + 
                                            "Process successfully altered.");         
         
         if (noError == false)
            Log.error("SPQServer", "alterSPQueItem", 
                       "Could not send message from server!");
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "alterSPQueItem", exception.toString());
      }
   }
   
   /**
    * This function pretty much just changes the custom_job_status field for a
    *  scheduled job, which amounts to a schedule string modification. But
    * there is some fuzzy logic going on underneath that should be cleaned up.
    */
   public void changeCustomStatus()
   {      
      int dataItemLengths[];
      String dataItems[];
      int pidValue;
      int isScheduledJobs;
      int error;
      String status;
      StringBuffer progroupTitle = new StringBuffer();
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true) {
            
            Log.error("SPQServer", "changeCustomStatus", 
                      "Received message while daemon idled.");  
            
            if (sendFormattedServerMsg("RETCHANGESTATUS", 
                  StringLib.valueOf(ErrorCodes.ERROR_SERVER_IDLED, 4)) == false)
               Log.error("SPQServer", "changeCustomStatus", 
                         "Failed to send message");
            
            return;                        
         }
         
         // Get the client data
         dataItemLengths = new int[2];
         dataItems = new String[2];

         dataItemLengths[0] = Constants.MAX_PROCESS_ID_LEN;
         dataItemLengths[1] = Constants.MAX_CUST_PROC_STAT_LEN;         
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("SPQServer", "changeCustomStatus", 
                      "Could not parse incoming message");
            
            if (sendFormattedServerMsg("RETCHANGESTATUS", 
                StringLib.valueOf(ErrorCodes.ERROR_CANT_PARSE_MSG, 4)) == false)
               Log.error("SPQServer", "changeCustomStatus", 
                         "Failed to send message");
            
            return;            
         }
         
         pidValue = StringLib.parseInt(dataItems[0], 0);
         status = dataItems[1];
         isScheduledJobs = 0;
         
         if (dataItems[0].length() >= 8) 
            // referenced by ntcss_job_id
            error = SvrProcLib.changeServerProcess(dataItems[0], -1, 
                                        Constants.CHANGE_CUST_STAT, -1, -1, 
                                        status, null, null, null, progroupTitle,
                                        0, Constants.LOW_PRIORITY);         
         else 
            // only referenced by unix ID.. must be running..
            error = SvrProcLib.changeServerProcess(null, pidValue, 
                                        Constants.CHANGE_CUST_STAT, -1, -1, 
                                        status, null, null, null, progroupTitle,
                                        0, Constants.LOW_PRIORITY);    
         
         if (error < 0) {    
            
            if (sendFormattedServerMsg("RETCHANGESTATUS", 
                                         StringLib.valueOf(error, 4)) == false)
               Log.error("SPQServer", "changeCustomStatus", 
                         "Failed to send message");
            
            return;            
         }
         
         updateClients(progroupTitle.toString(), isScheduledJobs);
         
         if (isScheduledJobs != 0)
            Log.debug("SQPServer", "changeCustomStatus", 
                      "Changing schedule string for " + dataItems[0] + ".");
         else
            Log.debug("SPQServer", "changeCustomStatus", 
                      "Changing custom status for " + dataItems[0] + ".");
         
         
         if (sendFormattedServerMsg("RETCHANGESTATUS", 
                                         StringLib.valueOf(error, 5)) == false)
            Log.error("SPQServer", "changeCustomStatus", 
                      "Failed to send message");                                 
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "changeCustomStatus", exception.toString());
      }
   }
   
   public void refreshSchedJobQue()
   {           
      refreshSchedJobQueue = true;
      refreshSPQue();
   }
   
   /**
    * Sends an updated server process list to all listening clients
    */
   public void refreshSPQue()
   {      
      int dataItemLengths[];
      String dataItems[];
      int isScheduledJob;
      
      try {
         
         // Check to see if the server is idled
         if (isServerIdled() == true)
            return;
         
         // Read the client data
         dataItemLengths = new int[1];
         dataItems = new String[1];
         dataItemLengths[0]  = Constants.MAX_PROGROUP_TITLE_LEN; // progroup title
         if (parseData(dataItemLengths, dataItems) == false) {
            Log.error("SPQServer", "startNewSPQue", 
                      "Could not parse incoming message");
            return;
         }
         
         // Determine which message was received
         if (refreshSchedJobQueue == true)
            isScheduledJob = 1;
         else
            isScheduledJob = 0;
         
         // Send a message to all the threads to indicate that they need to 
         //  send an updated server process list to their listening clients
         updateClients(dataItems[0], isScheduledJob);
         Log.debug("SPQServer", "refreshSPQue", "Refreshing " + dataItems[0] + 
                   " process queues");
         
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "refreshSPQue", exception.toString());
      }
   }      
   
   /**
    * Re-queries the database for server process data and sends the new
    *  information to the client. True is returned if there were no errors.
    */
   private boolean resendSPQList(String progroupTitle, String loginName, 
                                 int role, int isScheduledJob, int filterMask, 
                                 SvrProcessFilter filterData)
   {
      String timeString = null;
      String queryFilename;
      int fileSize;
      boolean quit;
      StringBuffer messageData = new StringBuffer();
      File fileDesc;
      
      try {
         // Check to see if the server is idled
         if (isServerIdled() == true) 
            return false;
         
         // Create a filename
         timeString = StringLib.getFullTimeStr();
         queryFilename = Constants.NTCSS_LOGS_DIR + "/" +
                         Thread.currentThread().getName() + "." + timeString + 
                         ".query";
         
         // Re-query the database for server process data
         fileSize = SvrProcLib.getServerProcessQueueList(progroupTitle, 
                                                         loginName, role, 
                                                         queryFilename, 
                                                         messageData,
                                                         isScheduledJob, 
                                                         filterMask,
                                                         filterData);
         if (fileSize > 0)
            Log.debug("SPQServer", "resendSPQList", "Resending server process " 
                      + "info for " + progroupTitle + " role " + role);
         
         // Send a response back to the client         
         if (fileSize >= 0) 
            quit = !sendServerMsgFile("GOODSPQUE", messageData.toString(), 
                                      queryFilename, fileSize);                        
         else 
            quit = !sendErrorMessage("NOSPQUE", fileSize);               
         
         // Delete the query file
         fileDesc = new File(queryFilename);
         if (fileDesc.exists() == true)
            fileDesc.delete();
         
         // If there was an error sending the response, this thread should die
         if (quit == true) {
            Log.debug("SPQServer", "resendSQPList", "Process exiting because " +
                      "client is not responding");
            
            return false;
         }
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "resendSPQList", exception.toString()); 
         return false;
      }
      
      return true;
   }
   
   /**
    * Interrupts all the threads that have server process clients listening
    *  to them. This interruption will force them to resend server process
    *  data to their corresponding client.
    */
   private void updateClients(String progroupTitle, int isScheduledJob)
   {
      Enumeration list;
      QueueInfo queueInfo;
      
      try {
   
         // Get the list of threads which have server process clients listening
         //  to them
         list = QueueList.getList();
         
         while (list.hasMoreElements() == true) {
            queueInfo = (QueueInfo)list.nextElement();
            
            if (((queueInfo.getProgroupTitle()).equals(progroupTitle)) &&
                (queueInfo.getScheduledJobFlag() == isScheduledJob))
               (queueInfo.getThreadInstance()).interrupt();
         }
      }
      catch (Exception exception) {
         Log.excp("SPQServer", "updateClients", exception.toString());
      }
   }
}

/**
 * Contains information on each process queue created
 */
class QueueList
{
   private static Vector list = new Vector();
   
   /**
    * Adds the given information to the list
    */
   public static synchronized void add(QueueInfo info)
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
   public static synchronized void remove(QueueInfo info)
   {
      list.remove(info);
   }

   /**
    * Dumps the information in the list
    */
   public static void dump()
   {
      Enumeration currentList;
      QueueInfo info;

      try {
         currentList = getList();

         while(currentList.hasMoreElements()) {
            info = (QueueInfo)currentList.nextElement();
            Log.info("QueueList", "dump", 
                     (info.getThreadInstance()).getName());
         }
      }
      catch (Exception exception) {
         Log.excp("QueueList", "dump", exception.toString());
      }
   }
}

/**
 * This represents information obtained when a NEWSPQ message is received
 */
class QueueInfo
{
   private Thread threadInstance;
   private String progroupTitle;
   private int isSchedJob;
   
   public QueueInfo(Thread threadInstance, String progroupTitle, 
                    int isSchedJob)
   {
      try {
         this.threadInstance = threadInstance;
         this.progroupTitle = progroupTitle;
         this.isSchedJob = isSchedJob;
      }
      catch (Exception exception) {
         Log.excp("QueueInfo", "QueueInfo", exception.toString());
      }
   }
   
   public String getProgroupTitle()
   {
      return progroupTitle;
   }
   
   public int getScheduledJobFlag()
   {
      return isSchedJob;
   }
   
   public Thread getThreadInstance()
   {
      return threadInstance;
   }
}