/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.tools;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import java.io.File;

import java.util.Vector;
import java.util.StringTokenizer;

import COMMON.PropertiesLoader;
import COMMON.FTP;
import COMMON.INI;
import COMMON.Version;

import ntcsss.log.EntityName;
import ntcsss.log.Log;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.distmgr.DistributionManager;
import ntcsss.distmgr.DPResults;

import ntcsss.libs.Constants;
import ntcsss.libs.StringLib;
import ntcsss.libs.DataLib;
import ntcsss.libs.SvrStatLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.ReplicationLib;
import ntcsss.libs.UserLib;
import ntcsss.libs.NDSLib;

import ntcsss.libs.structs.SvrStatus;

import ntcsss.util.FileOps;

/**
 * A class that implements the ntcss_cmd commandline program
 */
public class NtcssCmd
{
   private static String localHostname;
   private static boolean isMasterServer;
   
   /**
    * Starts the ntcss_cmd process
    */
   public static void main(String arguments[])
   {      
      String newArguments[];
      
      try {
         
         // Set the entity name for logging purposes
         Log.setDefaultEntityName(EntityName.NTCSS_CMD);
         
         // Load the properties file which will always be the first argument
         if ((arguments.length < 1) || 
             (PropertiesLoader.load(arguments[0])) == false) {
            print("Error loading properties file");
            System.exit(1);
         }                              

         // Process the command line arguments
         if (arguments.length == 1)
            processArguments(null);
         else {
            
            // Make a copy of the arguments (minus the properties file)
            newArguments = new String[arguments.length - 1];
            System.arraycopy(arguments, 1, newArguments, 0, arguments.length - 1);
            
            // Process the new argument list
            processArguments(newArguments);
         }
      }
      catch (Exception exception) {
         print("Error occurred < " + exception.toString() + ">");
      }
   }
   
   /**
    * Prints the given string to the proper output stream
    */
   private static void print(String str)
   {
      System.out.println(str);
   }

   /**
    * Prints out all the currently supported command line arguments
    */
   private static void printHelp()
   {
      print("Valid arguments are:");
      print("   awaken");
      print("   comData get <data tag>");
      print("   comData list");
      print("   comData set <data tag> <data item>");
      print("   debug <on|off>");
      print("   help");
      print("   idle");
      print("   logins <block|allow> [ALL]");
      print("   logins count [ALL]");      
      print("   master copy <host>");      
      print("   master listcopy");
      print("   master make");
      print("   master unpack");
      print("   master who");      
      print("   msgbb cleanup");      
      print("   msgbb listperiod");
      print("   msgbb setperiod <number of days>");            
      print("   status <host|*leave blank = all hosts*>");
      print("   version <ntcsss|common>");
   }

   /**
    * Processes the given command line arguments. Returns true if no
    *  errors occurred.
    */   
   public static boolean processArguments(String arguments[])
   {
      boolean successful = true;
      
      try {
         
         // Set the global variables
         localHostname = StringLib.getHostname();
      
         if (DataLib.getMasterServer(new StringBuffer()) == 1)
            isMasterServer = true;
         else
            isMasterServer = false;
                  
         
         // Make sure any arguments are given
         if ((arguments == null) || (arguments.length == 0)) {        
            printHelp();
            return true;
         }
                  
         // Check to see which command line request was received
         if (arguments[0].equals("awaken") == true)
            successful = handleAwaken();           
         else if (arguments[0].equals("comData") == true)           
            successful = handleComData(arguments);               
         else if (arguments[0].equals("debug") == true)
            successful = handleDebug(arguments);
         else if (arguments[0].equals("help") == true) {
            printHelp();      
            successful = true;
         }
         else if (arguments[0].equals("idle") == true)
            successful = handleIdle();      
         else if (arguments[0].equals("logins") == true)           
            successful = handleLogins(arguments);            
         else if (arguments[0].equals("master") == true)       
            successful = handleMaster(arguments);              
         else if (arguments[0].equals("msgbb") == true)            
            successful = handleMsgBB(arguments);                                 
         else if (arguments[0].equals("status") == true)
            successful = handleStatus(arguments);
         else if (arguments[0].equals("version") == true)
            successful = handleVersion(arguments);
         else {
            print("Unknown argument :" + arguments[0]);
            printHelp();
            successful = false;
         }
         
         return successful;
      }
      catch (Exception exception) {
         print("Error processing arguments <" + exception.toString() + ">");
         
         return false;
      }
   }
   
   /**
    * Handles the awaken option. Returns true if there weren't any errors.
    *  This is public so that it can be used by the control panel.
    */
   public static boolean handleAwaken()
   {      
      int failures;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int status;
      
      try {
                  
         // Check to see if the local server is already awake
         status = SvrStatLib.isServerAwake("localhost", Constants.DB_SVR_PORT);
         if (status == 1) {
            print("Servers already awake on this host");
            return true;
         }
         else if (status == -1) {
            print("Error obtaining current server status");
            return true;
         }         
         
         // Clear the current users table
         if (DataLib.clearCurrentUsersTable() == false) 
            print("Error clearing local current users table");          
         
         // Awaken the NTCSS daemons on the local host
         if ((failures = SvrStatLib.sendAwakenMessage("localhost")) != 0)         
            print("Failed to awaken " + failures + " daemons on this host!");
         else
            print("Finished awaking all daemons on this host.");
         
         // If this is the master server send the awaken message to all 
         //  application server hosts
         if (isMasterServer == true) {
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               print("Error obtaining database connection");
               return false;
            }
            statement = connection.createStatement();
               
            // Get a list of all the application server hosts               
            resultSet = statement.executeQuery("SELECT hostname FROM hosts " +
                  "WHERE type = " + Constants.NTCSS_APP_SERVER);
               
            while (resultSet.next() == true) {
                  
               // Send the message to the host
               if ((failures = SvrStatLib.sendAwakenMessage(
                                          resultSet.getString("hostname"))) != 0)
                  print("Failed to awaken " + failures + " daemons on host <" + 
                        resultSet.getString("hostname") + "> !");
               else
                  print("Finished awaking all daemons on host <" + 
                        resultSet.getString("hostname") + ">.");
            }
               
            // Close the system resources
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }  
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling awaken option <" + exception.toString() + ">");
         
         return false;
      }
   }
   
   /**
    * Handles the common data option. Returns true if no errors occurred.
    * This is public so that it can be used by the control panel.
    */
   public static boolean handleComData(String arguments[])
   {
      String tag, value;
      StringBuffer data;
      
      try {
         
         // Check the number of parameters
         if ((arguments.length < 2) || (arguments.length > 4)) {
            print("Invalid number of arguments");
            return false;
         }
                  
         // Make sure this is the NTCSS master server 
/* Commented out because this check is no longer done in the Unix C code        
         if (isMasterServer == false) {
            print("This host is not the Master Server!");
            return false;
         }
 */
         
         if (arguments[1].equals("list") == true) {  // List all key/pair values
            if (DataLib.getNtcssSysConfData(null, null) != 0)
               print("Could not list common data.");
         }
         else if (arguments[1].equals("set") == true) { // Set the value of a key
            
            // Make sure enough arguments were given
            if (arguments.length != 4) {
               print("Invalid number of arguments");
               return false;
            }
            
            tag = arguments[2].trim();
            value = arguments[3].trim();
            
            // Make sure the given tag follow certain criteria
            if (DataLib.checkCommonDbTag(tag) == false) {
               print("Could not set common data. Invalid tag.");
               return false;
            }
            
            // Make sure the given value follow certain criteria
            if (DataLib.checkCommonDbItem(value) == false) {
               print("Could not set common data. Invalid value.");
               return false;
            }
            
            // Try to set the given key and value
            if (DataLib.putNtcssSysConfData(tag, value) != 0) {
               print("Could not set common data.");
               return false;
            }
            else
               print("Common data item with tag of \"" + tag + 
                     "\" set to \"" + value + "\".");
         }
         else if (arguments[1].equals("get") == true) { // Get the value of a key
            
            // Make sure a key was given
            if (arguments.length != 3) {
               print("Invalid number of arguments");
               return false;
            }
            
            tag = arguments[2].trim();
            
            // Make sure the given tag follow certain criteria
            if (DataLib.checkCommonDbTag(tag) == false) {
               print("Could not get common data. Invalid tag.");
               return false;
            }
            
            // Get the given key's value
            data = new StringBuffer();
            if (DataLib.getNtcssSysConfData(tag, data) != 0) {
               print("Could not get common data.");
               return false;
            }
            else
               print("Common data item with tag of \"" + tag + "\" is \"" 
                     + data.toString() + "\".");
         }
         else {
            print("Unknown argument: " + arguments[1]);
            return false;
         }
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling common data option <" + exception.toString() + ">");
         
         return false;
      }            
   }
   
   /**
    * Handles the debug option. Returns true if no errors occurred.
    *  This is public so that it can be used by the control panel.
    */
   public static boolean handleDebug(String arguments[])
   {
      int newDebugState;
      int isDebugCurrentlyOn;
      String messageToSend;
      boolean sentOk = true;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      
      try {                  
         
         // Check the number of arguments given
         if (arguments.length != 2) {
            print("Invalid number of arguments");
            return false;
         }                  
         
         // Make sure received known argument
         if (arguments[1].equals("on") == true)
            newDebugState = Constants.SERVER_STATE_ONDEBUG;
         else if (arguments[1].equals("off") == true)
            newDebugState = Constants.SERVER_STATE_OFFDEBUG;
         else {
            print("Unknown debug argument: " + arguments[1]);
            return false;
         }
         
         // Get the current debug state of the local server
         isDebugCurrentlyOn = SvrStatLib.isDebugOn("localhost", 
                                                   Constants.CMD_SVR_PORT);
         if (isDebugCurrentlyOn == -1) {
            print("Error obtaining debug state of local server");
            return true;
         }
          
         // No need to turn debug on if it is already on and vice versa
         if (((newDebugState == Constants.SERVER_STATE_ONDEBUG) && 
              (isDebugCurrentlyOn == 0)) ||
             ((newDebugState == Constants.SERVER_STATE_OFFDEBUG) &&
              (isDebugCurrentlyOn == 1))) {
                 
            // Create the message to send out
            messageToSend = StringLib.formatMessage("SERVERSTATUS", 
                                                    String.valueOf(newDebugState));
                        
            // If this is the master server, update the debug state on all the
            //  hosts
            if (isMasterServer == true) {
               
               // Get a database connection
               connection = DatabaseConnectionManager.getConnection();
               if (connection == null) {
                  print("Error obtaining database connection");
                  return false;
               }
               statement = connection.createStatement();
               
               // Get a list of all the hosts               
               resultSet = statement.executeQuery("SELECT hostname FROM hosts");
               
               while (resultSet.next() == true) {
                  
                  // Send the message to the host
                  if (SocketLib.sendFireForgetMessage(resultSet.getString("hostname"), 
                                                      Constants.CMD_SVR_PORT, 
                                                      messageToSend) == false)
                     sentOk = false;
               }
               
               // Close the system resources
               resultSet.close();
               statement.close();
               DatabaseConnectionManager.releaseConnection(connection);
            }
            else
               sentOk = SocketLib.sendFireForgetMessage("localhost", 
                                                        Constants.CMD_SVR_PORT, 
                                                        messageToSend);
            
            // Check to see if there was an error
            if (sentOk == false)
               print("Failed to communicate with all NTCSS Server Daemons!");
         }
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling debug option <" + exception.toString() + ">");
         
         return false;
      }
   }
   
   /**
    * Handles the idle option. Returns true if no errors occurred.
    *  This is public so that it can be used by the control panel.
    */
   public static boolean handleIdle()
   {
      int failures;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int status;
      
      try {     
                 
         // Check to see if the local server is already idle
         status = SvrStatLib.isServerAwake("localhost", Constants.DB_SVR_PORT);
         if (status == 0) {
            print("Servers already idled on this host");
            return true;
         }
         else if (status == -1) {
            print("Error obtaining current server status");
            return true;
         }
         
         // Idle the NTCSS daemons on the local host
         if ((failures = SvrStatLib.sendIdleMessage("localhost")) != 0)     
            print("Failed to idle " + failures + " daemons on this host!");
         else
            print("Finished idling all daemons on this host.");
         
         // If this is the master server send the idle message to all 
         //  application server hosts
         if (isMasterServer == true) {
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               print("Error obtaining database connection");
               return false;
            }
            statement = connection.createStatement();
               
            // Get a list of all the application server hosts               
            resultSet = statement.executeQuery("SELECT hostname FROM hosts " +
                  "WHERE type = " + Constants.NTCSS_APP_SERVER);
               
            while (resultSet.next() == true) {
                  
               // Send the message to the host
               if ((failures = SvrStatLib.sendIdleMessage(
                                        resultSet.getString("hostname"))) != 0)
                  print("Failed to idle " + failures + " daemons on host <" + 
                        resultSet.getString("hostname") + "> !");
               else
                  print("Finished idling all daemons on host <" + 
                        resultSet.getString("hostname") + ">.");
            }
               
            // Close the system resources
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         } 
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling idle option <" + exception.toString() + ">");
         
         return false;
      }
   }
   
   /**
    * Handles the login option. Returns true if no errors occurred.
    * This is public so that it can be used by the control panel.
    */
   public static boolean handleLogins(String arguments[])
   {
      boolean all;     
      int option;
      Connection connection;
      ResultSet resultSet;
      Statement statement;
      String messageToSend;      
      File destinationFile;
      int numCurrentUsers;
      int totalNumCurrentUsers;
      final String unixPathCurrentUsers;
      
      try {
                  
         // Check the number of arguments given
         if ((arguments.length < 2) || (arguments.length > 3)) {
            print("Invalid number of arguments provided");
            return false;
         } 
         
         // Check the option given
         if (arguments[1].equals("block") == true)
            option = Constants.SERVER_STATE_ONLOCK;
         else if (arguments[1].equals("allow") == true)
            option = Constants.SERVER_STATE_OFFLOCK;
         else if (arguments[1].equals("count") == true)
            option = -1;
         else {
            print("Unknown argument: " + arguments[1]);
            return false;
         }
         
         // Check for the "all" argument
         all = false;
         if (arguments.length == 3) {
            
            if (arguments[2].equalsIgnoreCase("all") == true)
               all = true;
            else {
               print("Invalid argument");
               return false;
            }
            
            // Don't allow "block" or "allow" ALL to be executed from a server
            //  that isn't the master
            if ((option != -1) && (isMasterServer == false)) {
               print("ALL servers can only be blocked/allowed from the Master server");
               return false;
            }
         }
         
         if (option == -1) { // i.e. "count"                
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               print("Error obtaining database connection");
               return false;
            }
            
   
            if (all == true) {
                              
               destinationFile = new File(Constants.NTCSS_LOGS_DIR + "/current_users");
               
               // When getting the current users from a Windows platform,
               //  the FTP daemon there will translate this path according to
               //  its platform
               unixPathCurrentUsers = "/h/NTCSSS/database/current_users";
                                             
               // Get a list of all the non-application server hosts  
               statement = connection.createStatement();
               resultSet = statement.executeQuery("SELECT hostname FROM hosts " +
                  "WHERE type != " + Constants.NTCSS_APP_SERVER);
                                       
               totalNumCurrentUsers = 0;
               while (resultSet.next() == true) {
                  
                  // Delete the temporary file if it exists
                  if (destinationFile.exists() == true)
                     destinationFile.delete();
                  
                  // FTP the current_users file from the given host
                  if (FTP.get(resultSet.getString("hostname"), 
                              unixPathCurrentUsers, 
                              destinationFile.getAbsolutePath(), false) == true) {
                                 
                     // Count the number of current users in that file
                     numCurrentUsers = 
                      DataLib.countCurrentUsers(destinationFile.getAbsolutePath());
                     if (numCurrentUsers < 0) {
                        print("Counting of current users failed");
                        break;
                     }
                     
                     print("The current number of NTCSS users is " + 
                           numCurrentUsers + " on host " + 
                           resultSet.getString("hostname"));
                     totalNumCurrentUsers += numCurrentUsers;
                  }
                  else
                     print("Net copy from host <" + 
                           resultSet.getString("hostname") + "> failed");
               }
               
               print("The total number of NTCSS users on all hosts = " + 
                      totalNumCurrentUsers);
               if (destinationFile.exists() == true)
                  destinationFile.delete();
                         
               resultSet.close();
               statement.close();                              
            }
            else {
               
               // Get the number of users logged in to this server
               numCurrentUsers = DataLib.numCurrentUsers(connection);
               if (numCurrentUsers >= 0) 
                  print("The current number of NTCSS users is " + numCurrentUsers);                                                             
               else
                  print("Error getting the current number of NTCSS users");                                                             
            } 
                  
            // Close the system resources            
            DatabaseConnectionManager.releaseConnection(connection);
           
         }
         else {  // i.e. "block" or "allow"
            
            messageToSend = StringLib.formatMessage("SERVERSTATUS",
                                                    String.valueOf(option));
            
            if (all == true) {
               
               // Get a database connection
               connection = DatabaseConnectionManager.getConnection();
               if (connection == null) {
                  print("Error obtaining database connection");
                  return false;
               }
               statement = connection.createStatement();
               
               // Get a list of all the non-application server hosts               
               resultSet = statement.executeQuery("SELECT hostname FROM hosts " +
                  "WHERE type != " + Constants.NTCSS_APP_SERVER);
                                             
               while (resultSet.next() == true) {
                  
                  if (option == Constants. SERVER_STATE_ONLOCK)
                     print("Blocking logins on host " + 
                            resultSet.getString("hostname") + " ....");
                  else
                     print("Allowing logins on host " + 
                           resultSet.getString("hostname") + " ....");
                  
                  // Send the message to the host                  
                  if (SocketLib.sendFireForgetMessage(resultSet.getString("hostname"), 
                              Constants.DSK_SVR_PORT, messageToSend) == false)
                     print("Failed to communicate with NTCSS Server Daemons on " + 
                           resultSet.getString("hostname"));
                  else
                     print("Done"); 
                  
               }
               
               // Close the system resources
               resultSet.close();
               statement.close();
               DatabaseConnectionManager.releaseConnection(connection);
            }
            else {
               
               // Send the message to the local host
               if (SocketLib.sendFireForgetMessage("localhost",              
                              Constants.DSK_SVR_PORT, messageToSend) == false)
                  print("Failed to communicate with NTCSS Server Daemons on " + 
                        "this host");
               else
                  print("Done");
            } 
         }  
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling logins option <" + exception.toString() + ">");
         
         return false;
      }
   }
   
   /**
    * Handles "master" option. If no errors occur, true is returned.
    */
   private static boolean handleMaster(String arguments[])
   {
      StringBuffer masterHostname;
      Vector hostList;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      DPResults results;
      
      try {
         
         // Check the number of arguments given
         if (arguments.length < 2) {
            print("Invalid number of arguments");
            return false;
         }
         
         if (arguments[1].equals("make") == true) {
            
            // Check to make sure this host is not already the master
            if (DataLib.getMasterServer(new StringBuffer()) == 1) {
               print("This host is already the Master Server");
               return false;
            }
            
            return makeMaster();
         }
         else if (arguments[1].equals("unpack") == true) {
            
            // Check to see if the server daemons are idled on this host
            if (SvrStatLib.isServerAwake("localhost", Constants.DSK_SVR_PORT) == 1) {
               
               // Log all the users on this server off
               UserLib.forceLogoffAllUsers();
               
               // Idle the daemon servers on this host
               if (SvrStatLib.sendIdleMessage("localhost") != 0) {
                  print("Error idling daemons");
                  return false;
               }
            }
            
            // Unpack the contents of the master database backup bundle
            if (ReplicationLib.unpackMasterFiles() == false) {
               print("Failed to unpack master files! Originial database restored.");
               print("Please awaken the servers on this host.");
               return false;
            }
            
            // Awaken the server daemons on this host
            if (SvrStatLib.sendAwakenMessage("localhost") != 0) {
               print("Failed to awaken all servers on this host! Please do " + 
                     "this manually!");
               return false;
            }
                        
         }
         else if (arguments[1].equals("who") == true) { // Reports current master
            
            // Get the name of the NTCSS master server            
            masterHostname = new StringBuffer();
            if (DataLib.getMasterServer(masterHostname) == 1)
               print("This host is the Master Server.");
            else
               print("Host " + masterHostname.toString() + " is the Master Server.");
         }
         else if (arguments[1].equals("listcopy") == true) {
            // Lists the contents of the master.info file
            
            ReplicationLib.displayBackupInfo();
         }
         else if (arguments[1].equals("copy") == true) { 
            
            // Bundles up the database and sends a message out to either all the
            //  replication hosts or just the host specified, indicating that
            //  they need to copy the newly created database bundle.
            
            // Check the number of arguments given 
            if (arguments.length > 3) {
               print("Invalid number of parameters");
               return false;
            }
                        
            // Make sure this is the master server
            if (isMasterServer == false) {
               print("Cannot copy files from a non master!");
               return false;
            }
            
            // Build the database backup
            if (ReplicationLib.buildMasterDBBackupFiles() == false) {
               print("Failed to build the master backup files!"); 
               return false;
            }
            
            // Create a list of the hosts the message is to be sent out to
            hostList = new Vector();
            if (arguments.length == 3) 
               hostList.add(arguments[2]);
            else {
               
               // Get a database connection
               if ((connection = DatabaseConnectionManager.getConnection()) == null) {
                  print("null database connection");
                  return false;
               }
               statement = connection.createStatement();
               
               // Get a list of all the replication servers
               sqlItems = new Object[1];
               sqlItems[0] = new Boolean(true);
               resultSet = statement.executeQuery(StringLib.SQLformat(
                  "SELECT hostname FROM hosts WHERE type != " + 
                  Constants.NTCSS_MASTER +  "  AND replication_ind = " + 
                  Constants.ST, sqlItems));
               while (resultSet.next() == true)
                  hostList.add(resultSet.getString("hostname"));
               
               // Release the system resources
               resultSet.close();
               statement.close();
               DatabaseConnectionManager.releaseConnection(connection);
            }  
                        
            // Send the message to the host(s) to indicate that they need to
            //  copy the backup database bundle            
            results = DistributionManager.sendMessage(
                StringLib.padString("COPYMASTERFILES", Constants.CMD_LEN, '\0'), 
                hostList, Constants.DB_SVR_PORT, false,
                Constants.DP_GETMASTERFILES_TIMEOUT);
            
            // No need to check the results
                        
         }
         else {
            print("Unknown argument");            
            return false;
         }
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling master option <" + exception.toString() + ">");
         
         return false;
      }
   }    
         
   /**
    * Handles "msgbb" option. If no errors occur, true is returned.
    */
   private static boolean handleMsgBB(String arguments[])
   {            
      String maxBBMessageAgeTag = "NTCSS MSG BB PERIOD";
      StringBuffer value;
      int maxBBMessageAge;
      String currentTime;
      Connection connection;
      Statement statement;
      Object sqlItems[];
      
      try {
         
         // Check number of arguments given
         if (arguments.length < 2) {
            print("At least one additional argument required");
            return false;
         }
                  
         // Make sure this is the NTCSS master server
         if (isMasterServer == false) {
            print("This host is not the Master Server.");
            return false;
         }
         
         // Check to see which argument was given
         if (arguments[1].equals("cleanup") == true) {
            
            // Get the maximum age from SYS CONF
            value = new StringBuffer();
            if (DataLib.getNtcssSysConfData(maxBBMessageAgeTag, value) < 0) {
               print("Could not get time period");
               return false;
            }
            
            // Check the age value
            maxBBMessageAge = StringLib.parseInt(value.toString(), -1);            
            if (maxBBMessageAge < 0) {
               print("Invalid period value");
               return false;
            }                                            
            
            // Determine the oldest date for which messages will be kept
            if (StringLib.getNextFullTimeStr(StringLib.getFullTimeStr(), value, 
                         Constants.DAY_TYPE, -1 * maxBBMessageAge) == false) {
               print("Could not determine maximum age");
               return false;
            }
                        
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               print("Error obtaining database connection");
               return false;
            }
            statement = connection.createStatement();
            
            // This should cascade to delete cooresponding entries in the
            //  bb_message_map and bb_read_history tables
            sqlItems = new Object[2];
            sqlItems[0] = new Boolean(true);
            sqlItems[1] = value.toString().trim();
//            statement.executeUpdate(StringLib.SQLformat("DELETE FROM " + 
//                "bb_messages WHERE sent = " + Constants.ST + 
//                " AND creation_time < " + Constants.ST, sqlItems));
            DataLib.cascadeDelete(statement, "bb_messages", StringLib.SQLformat(
                "sent = " + Constants.ST + 
                " AND creation_time < " + Constants.ST, sqlItems));            
               
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);            
         }
         else if (arguments[1].equals("setperiod") == true) {
            
            // Make sure an age argument is given
            if (arguments.length != 3) {
               print("Invalid number of parameters");
               return false;
            }
            
            // Make sure the maximum bulletin board message age given is valid
            maxBBMessageAge = StringLib.parseInt(arguments[2], -1);
            if (maxBBMessageAge < 0) {
               print("Invalid age given <" + arguments[2] + ">");
               return false;
            }
            
            // Attempt to store the given value
            if (DataLib.putNtcssSysConfData(maxBBMessageAgeTag, 
                                            String.valueOf(maxBBMessageAge)) != 0) {
               print("Could not set time period.");
               return false;
            }
            else 
               print("Bulletin board messages older than " + maxBBMessageAge + 
                     " days will be deleted.");
   
         }
         else if (arguments[1].equals("listperiod") == true) {
            
            // Get the SYS_CONF tag which indicates the age (in days) that 
            //  bulletin board message will be pruned.
            value = new StringBuffer();
            if (DataLib.getNtcssSysConfData(maxBBMessageAgeTag, value) != 0) 
               print("Could not get time period.");            
            else
               print("Bulletin board messages older than <" + value.toString() + 
                     "> days will be deleted.");
         }
         else {
            print("Unknown argument");
            return false;
         }
      
         return true;
      }
      catch (Exception exception) {
         print("Error handling msgbb option <" + exception.toString() + ">");
         return false;
      } 
   } 
   
   /**
    * Handles "status" option. If no errors occur, true is returned.
    */
   private static boolean handleStatus(String arguments[])
   {      
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      
      try {
         
         if (arguments.length > 2) {
            print("Invalid number of arguments");
            return false;
         }
                  
         if (arguments.length == 1) { // Get all hosts' status
            
            // Get a database connection
            connection = DatabaseConnectionManager.getConnection();
            if (connection == null) {
               print("Error obtaining database connection");
               return false;
            }
            statement = connection.createStatement();
               
            // Get a list of all the NTCSS server hosts               
            resultSet = statement.executeQuery("SELECT hostname FROM hosts ");
                                             
            while (resultSet.next() == true) 
               getAndDisplayHostStatus(resultSet.getString("hostname"));               
               
            // Close the system resources
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         else  // Get status only for specified host
            getAndDisplayHostStatus(arguments[1]);         
              
         return true;
      }
      catch (Exception exception) {
         print("Error handling status option <" + exception.toString() + ">");
         return false;
      }            
   }
   
   /**
    * Handles "version" option. If no errors occur, true is returned.
    */
   private static boolean handleVersion(String arguments[])
   {
      try {
         
         // If no arguments are given, just report the version of NTCSSS
         //  software
         if (arguments.length < 2) {
            print("Version " + Constants.NTCSS_SOFTWARE_VERSION);
         }
         else if (arguments.length > 2) {
            print("Invalid number of parameters");
            return false;
         }
         else {
            if ((arguments[1].equalsIgnoreCase("ntcsss") == false) &&
                (arguments[1].equalsIgnoreCase("common") == false)) {
               print("Invalid parameter");
               return false;
            }
            
            if (arguments[1].equalsIgnoreCase("ntcsss") == true)
               print("Version " + Constants.NTCSS_SOFTWARE_VERSION);
            else
               print("Version " + Version.get());
         }         
         
         return true;
      }
      catch (Exception exception) {
         print("Error handling version option <" + exception.toString() + ">");
         return false;
      }
   }
      
   /**
    * Gets the NTCSS daemon status for the specified host and displays the
    *  results.
    */
   private static void getAndDisplayHostStatus(String host)
   {
      String line = null;
      SvrStatus cmdStatus = null, dbStatus = null, dskStatus = null, 
                spqStatus = null, msgStatus = null, prtStatus = null;
      SvrStatus tmpStatus = null;
      
      try {
         
         // Get the status of each of the host's server daemons
         cmdStatus = SvrStatLib.getServerStatus(host, Constants.CMD_SVR_PORT);
         
         // No need to get the rest of the daemons' status if the first one
         //  failed
         if (cmdStatus != null) {         
            dbStatus  = SvrStatLib.getServerStatus(host, Constants.DB_SVR_PORT);
            dskStatus = SvrStatLib.getServerStatus(host, Constants.DSK_SVR_PORT);
            spqStatus = SvrStatLib.getServerStatus(host, Constants.SPQ_SVR_PORT);            
            msgStatus = SvrStatLib.getServerStatus(host, Constants.MSG_SVR_PORT);
            prtStatus = SvrStatLib.getServerStatus(host, Constants.PRTQ_SVR_PORT); 
         }
         
         // Display the results
         print("=======================================================================");         
         print("   STATUS OF ALL NTCSS DAEMONS RUNNING ON HOST \"" + host + "\"");
         print("");
         print("DAEMON NAME                 STATUS     DEBUG    ACCESS");
         print("--------------------------- ---------- -------- --------------------");                                            
         
         for (int i = 0; i < 6; i++) {
            switch (i) {
               case 0:
                  line = "Command Server              ";
                  tmpStatus = cmdStatus;
                  break;
               case 1:
                  line = "Database Server             ";
                  tmpStatus = dbStatus;
                  break;
               case 2:
                  line = "Desktop Server              ";
                  tmpStatus = dskStatus;
                  break;
               case 3:
                  line = "Job Queue Server            ";
                  tmpStatus = spqStatus;
                  break;
               case 4:
                  line = "Message Server              ";
                  tmpStatus = msgStatus;
                  break;
               case 5:
                  line = "Print Queue Server          ";
                  tmpStatus = prtStatus;
                  break;
            }
            
            if (tmpStatus == null) 
               line += " UNKNOWN    UNKNOWN";
            else {
            
               // Display state
               if (tmpStatus.isServerAwake() == true)
                  line += " AWAKE     ";
               else if (tmpStatus.isServerIdled() == true)
                  line += " IDLE      ";
               else
                  line += " UNKNOWN   ";
            
               // Display debug status
               if (tmpStatus.isDebugOn() == true)
                  line += " ON      ";
               else if (tmpStatus.isDebugOff() == true)
                  line += " OFF     ";
               else
                  line += " UNKNOWN ";
            
               // Display login status
               if (tmpStatus == dskStatus) {
                  if (tmpStatus.areLoginsAllowed() == true)
                     line += " LOGINS ALLOWED ";
                  else if (tmpStatus.areLoginsBlocked() == true)
                     line += " LOGINS BLOCKED "; 
               }  
            }
            
            print(line);                 
         }
         
         print("");
         print("");     
      }
      catch (Exception exception) {
         print("Error displaying status for host <" + host + "> <" + 
                exception.toString() + ">");       
      }
   }            
   
   /**
    * Performs the necessary steps to make this host the NTCSS master server.
    *  If any errors occur, false is returned.
    *
    * The main steps are:
    *  - importing the old master's backup database
    *  - updating the local database to reflect the hosts.type change for
    *    this host and the original master
    *  - updating the progrps.ini file to reflect the host type change for
    *    this host and the original master      
    *
    * The NDS code automatically will point to the NDS master server if a
    *  host is the NTCSS master server, so no changes need to be made in that
    *  area, except getting the latest NDS configuration files from the
    *  original NTCSS master server.
    */
   private static boolean makeMaster()
   {
      SvrStatus status;
      StringBuffer originalMaster;
      Connection connection;
      Statement statement;
      String localHostname = StringLib.getHostname();
      String hostname;
      StringTokenizer hostsTokenizer, tmpTokenizer;
      String hostsList, hostSection;      
      INI iniFile;
      File file;
      
      try {
         
         // Get the name of the current NTCSS master server
         originalMaster = new StringBuffer();
         DataLib.getMasterServer(originalMaster);
                  
         // Check to make sure that if the original master is up, that 
         //  it is atleast idled or not accepting login request.
         status = SvrStatLib.getServerStatus(originalMaster.toString(), 
                                             Constants.DSK_SVR_PORT);
         if ((status != null)  &&
             ((status.isServerAwake() == true) && 
              (status.areLoginsAllowed() == true))) {
            print("The master server's daemons are still running.");
            print("Must shut master down before running master make.");
            return false;
         }
         
         // Check to make sure the master backup copy on this host is ok
         if (ReplicationLib.isMasterBundleOk() == false) {
            print("Backup file not valid! Aborting.");
            return false;
         }
         
         // If the server daemons on this host are up, idle them and force
         //  off any users logged in to this server
         status = SvrStatLib.getServerStatus("localhost", 
                                             Constants.DSK_SVR_PORT);
         if (status != null) {                                                
            
            // Try to idle the host
            if (SvrStatLib.sendIdleMessage("localhost") != 0) {
               print("Unable to idle the local host. Aborting.");
               return false;            
            }                                    
            
            // Logoff all the users on this server
            if (UserLib.forceLogoffAllUsers() == false) {
               print("Could not log off all the users. Aborting.");
               return false;
            }
         }
         
         // Import the contents of the master backup bundle
         if (ReplicationLib.unpackMasterFiles() == false) {
            print("Failed to unpack the master files! Aborting.");
            return false;
         }
         
         // Update the database to reflect the change in master server
         if ((connection = 
              DatabaseConnectionManager.getConnection()) == null) {
            print("Could not get database connection for updating. Aborting");
            return false;
         }
         statement = connection.createStatement();
         
         statement.executeUpdate("UPDATE hosts SET type = " + 
              Constants.NTCSS_MASTER + " WHERE upper(hostname) = '" + 
              localHostname.toUpperCase() + "'");
         statement.executeUpdate("UPDATE hosts SET type = " + 
              Constants.NTCSS_AUTH_SERVER + " WHERE upper(hostname) = '" + 
              originalMaster.toString().toUpperCase() + "'");
         
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Update the cached NTCSS master hostname
         DataLib.cacheMasterName();    
         
         // Update the progrps.ini file to reflect the change in master server
         iniFile = new INI();
         if ((hostsList = iniFile.GetProfileSection("HOSTS", 
                          Constants.NTCSS_PROGRP_INIT_FILE)) == null) {
            print("Error updating INI file. Aborting.");
            return false;
         }
         
         // Loop through each of the hosts listed and update this host's and
         //  the original master's host type
         hostsTokenizer = new StringTokenizer(hostsList, INI.STRINGS_DELIMITER);
         while (hostsTokenizer.hasMoreTokens() == true) {
            
            // Get the name of the current host's section
            tmpTokenizer = new StringTokenizer(hostsTokenizer.nextToken(), 
                                               INI.TAG_VALUE_DELIMITER);
            hostSection = tmpTokenizer.nextToken();
            
            // Get the hostname entry for this host section
            hostname = iniFile.GetProfileString(hostSection, "NAME", 
                       "thishostnotfound", Constants.NTCSS_PROGRP_INIT_FILE);
            if (hostname.equals(localHostname) == true) 
               iniFile.WriteProfileString(hostSection, "TYPE", 
                               Constants.getHostStrType(Constants.NTCSS_MASTER), 
                               Constants.NTCSS_PROGRP_INIT_FILE);            
            else if (hostname.equals(originalMaster.toString()) == true)
               iniFile.WriteProfileString(hostSection, "TYPE", 
                          Constants.getHostStrType(Constants.NTCSS_AUTH_SERVER), 
                          Constants.NTCSS_PROGRP_INIT_FILE);
         }
         
         // Write the changes out to the file
         iniFile.FlushINIFile(Constants.NTCSS_PROGRP_INIT_FILE);  
                  
         if (FileOps.copy(Constants.NTCSS_PROGRP_INIT_FILE, 
                          Constants.NTCSS_PROGRP_DB_FILE) == false)
            print("Failed to copy progrps.ini file to database directory");
         
         // Get the LDAP configuration files from the old master
         
         file = new File(Constants.NTCSS_NDS_DIR + "/ldapservers");
         if (file.exists() == true)
            file.delete();
         
         if (FTP.get(originalMaster.toString(), "/h/NTCSSS/ldap/ldapservers",
         file.getCanonicalPath(), false) == false) {
            print("Unable to get ldap servers list from <" + 
                  originalMaster.toString() + ">");
            return false;
         }
         
         file = new File(Constants.NTCSS_NDS_DIR + "/master.conf");
         if (file.exists() == true)
            file.delete();
         
         if (FTP.get(originalMaster.toString(), "/h/NTCSSS/ldap/master.conf",
         file.getCanonicalPath(), false) == false) {
            print("Unable to get NDS master configuration file from <" + 
                  originalMaster.toString() + ">");
            return false;
         }
         
         file = new File(Constants.NTCSS_NDS_DIR + "/slave.conf");
         if (file.exists() == true)
            file.delete();
         
         if (FTP.get(originalMaster.toString(), "/h/NTCSSS/ldap/slave.conf",
         file.getCanonicalPath(), false) == false) {
            print("Unable to get NDS slave configuration file from <" + 
                  originalMaster.toString() + ">");
            return false;
         }
         
         file = new File(Constants.NTCSS_NDS_DIR + "/save.ldif");
         if (file.exists() == true)
            file.delete();
         
         if (FTP.get(originalMaster.toString(), "/h/NTCSSS/ldap/save.ldif",
         file.getCanonicalPath(), false) == false) {
            print("WARNING: Unable to get NDS database file from <" + 
                  originalMaster.toString() + ">");
            // No need to return if we don't get this file
         }
         
         // Refresh the NDS info
         NDSLib.refreshContextInfo();
                                    
         // Update the NDS host type attribute for this host and the 
         //  originial master
         NDSLib.modifyHostType(localHostname, Constants.NTCSS_MASTER);
         NDSLib.modifyHostType(originalMaster.toString(), 
                               Constants.NTCSS_AUTH_SERVER);
         
         // Store into NDS the information in this host's SYS CONF database
         //  table
         NDSLib.refreshSysConfData();                                                               
         
         // Try to awaken the server daemons on this host
         if (SvrStatLib.sendAwakenMessage("localhost") != 0) 
            print("Unable to awaken the local host. This must be done manually.");                     
         
         return true;
      }
      catch (Exception exception) {
         print("Error making master <" + exception.toString() + ">");
         return false;
      }
   }
}