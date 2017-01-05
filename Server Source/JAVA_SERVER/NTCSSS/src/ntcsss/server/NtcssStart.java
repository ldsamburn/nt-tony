/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.util.Properties;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Hashtable;
import java.util.StringTokenizer;

import java.net.ServerSocket;
import java.net.Socket;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.File;
import java.io.FilenameFilter;

import java.text.SimpleDateFormat;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import COMMON.PropertiesLoader;
import COMMON.FTP;

import ntcsss.libs.Constants;
import ntcsss.libs.DataLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.ReplicationLib;
import ntcsss.libs.NDSLib;
import ntcsss.libs.SvrProcLib;
import ntcsss.libs.PrintLib;
import ntcsss.libs.ErrorCodes;

import ntcsss.database.DatabaseConnectionManager;
import ntcsss.database.PrimeDB;

import ntcsss.log.Log;
import ntcsss.log.EntityName;

import ntcsss.util.FileOps;

/**
 * Creates and starts the servers
 */
public class NtcssStart
{   
   private static boolean start_boot_jobs = true;      
   private static boolean noPrime         = false;  
   public static final int NUM_SERVERS   = 6;
   
   /**
    * Creates and starts the servers
    */
   public static void main(String args[])
   {
      GenericServer servers[];
      int hostType;
      PrimeDB prime;
      ActiveServers activeServers;
      File stopFile;

      try {
         
         Log.setDefaultEntityName(EntityName.NTCSS_START);

         // Load the properties file
         if ((args.length < 1) || (PropertiesLoader.load(args[0]) == false)) {
            printError("Error loading properties file");
            exit(1);
         }                 
         
         // Make sure we can obtain a connection to a NDS server
         if (NDSLib.checkConnection() == false) {
            printError("Unable to connect to a NDS server");            
            exit(1);
         }
         
         // Get the master's progrps.ini file if it is different from the local
         //  copy
         getMasterProgrpsFile();
         
         // Perform checks on critical components that need to be in place
         if (prelaunchCheck() == false) {
            printError("Pre-launch check failed");            
            exit(1);
         }
         
         // Check to see if the servers are already up
         if (areServersRunning() == true) {
            printError("Servers are already running");            
            exit(1);
         }
         
         // Delete the file that NtcssStop created
         stopFile = new File(Constants.STOP_FILE);
         if (stopFile.exists() == true)
            stopFile.delete();
         
         // Parse the command line arguments
         if (parseArguments(args) == false)
            exit(1);        
         
         // Get the NTCSS host type of this host from the progrps.ini file
         if ((hostType = 
                DataLib.getHostTypeFromInitData(StringLib.getHostname())) < 0) {
            printError("Error obtaining host type");
            exit(1);
         }
         
         // A java NTCSS server cannot be an Application server type, so stop 
         //  that here
         if (hostType == Constants.NTCSS_APP_SERVER) {
            printError("This host cannot be an NTCSS application server");
            exit(1);
         }
         
         // If this is the master or an authentication server, see if the 
         //  database needs to be "primed"
         if (((hostType == Constants.NTCSS_MASTER) || 
              (hostType == Constants.NTCSS_AUTH_SERVER)) && 
             (noPrime == false)) {
                            
            prime = new PrimeDB();
            
            // Check to see if prime needs to run
            if (prime.checkRunNecessity() == true) {
               
               printInfo("Found newer Progoups Ini file. " +
                                  "Running prime ...");
               if (prime.start() == false) {
                  printError("Problems occured while runing prime!");
                  exit(1);   
               }
               else
                  printInfo("Prime complete.");
            }
         }
         
         // Import the records from the printer types database file into
         //  the database
         if (importPrinterTypes() == false)
            printWarn("Error importing printer types");
         
         // Import the records from the adapter types database file into
         //  the database
         if (importAdapterTypes() == false)
            printWarn("Error importing adapter types");
         
         // If this is the master server, update the NDS SYS_CONF data
         //  with the SYS_CONF data from the local database
         if (hostType == Constants.NTCSS_MASTER) {
            if (NDSLib.refreshSysConfData() == false)
               printWarn("Error updating SYS_CONF data");
         }
         
         // If this is an authentication server, reconcile the printers
         if (hostType == Constants.NTCSS_AUTH_SERVER) {    
            printInfo("Reconciling printers...");   
            if (PrintLib.reconcile() == false)
               printError("Error reconciling printers");   
            else
               printInfo("Printer reconcile is complete");   
         }         
         
         // Clean out the currrent_users table
         if (DataLib.clearCurrentUsersTable() == false) {
            printError("Error clearing current_users table");
            exit(1);
         }

         if (start_boot_jobs == true)
            DataLib.createNOLOGINSFile();

         // Create the servers  
         activeServers = new ActiveServers();
         servers = new GenericServer[NUM_SERVERS];             
         servers[0] = new GenericServer(Constants.SPQ_SVR_PORT,
            SPQServer.getServerDescription(), activeServers);      // Server Process Queue server
         servers[1] = new GenericServer(Constants.PRTQ_SVR_PORT,
            PrintQueServer.getServerDescription(), activeServers);  // Print Request Queue server
         servers[2] = new GenericServer(Constants.DB_SVR_PORT,
            DatabaseServer.getServerDescription(), activeServers); // Database server
         servers[3] = new GenericServer(Constants.DSK_SVR_PORT,
            DskServer.getServerDescription(), activeServers);      // Desktop server
         servers[4] = new GenericServer(Constants.CMD_SVR_PORT,
            CommandServer.getServerDescription(), activeServers);  // Command server            
         servers[5] = new GenericServer(Constants.MSG_SVR_PORT,
            MessageServer.getServerDescription(), activeServers);  // Message server

         // Start the servers
         for (int i = 0; i < NUM_SERVERS; i++)
            servers[i].start();  
         
         // Send CHECKWAITSVRPROC message to the local command server
         SocketLib.sendFireForgetMessage("localhost", 
                                         Constants.CMD_SVR_PORT, 
                                         StringLib.formatMessage("CHECKWAITSVRPROCS",                                                      
                                           StringLib.padString("all groups", 
                                            Constants.MAX_PROGROUP_TITLE_LEN, ' ')));  
         
         if (hostType == Constants.NTCSS_MASTER) {
            if (DataLib.putNtcssSysConfData("NTCSS MULTI-USER ADMIN MODE", "Disabled") < 0)
               printWarn("WARNING: Failed to ensure that multi-mode user admin mode" +
                          " is disabled.");
         }
         
         // Start any boot and scheduled predefined jobs
         if (start_boot_jobs == true) {
            
            // Send a message to command server for every local boot and 
            //  scheduled predefined job
            if (SvrProcLib.startScheduledAndBootTypePDJs() == false) {            
               DataLib.removeNOLOGINSFile();          
               exit(1);            
            }
            else
               DataLib.removeNOLOGINSFile();
         }
         
         // Check to see if the master's database should be copied
         if ((hostType != Constants.NTCSS_MASTER) &&
             (isReplicationOn() == true)) {
            printInfo("Copying the master backup files....");
            ReplicationLib.copyMasterBackupFiles();               
         }         

         createNtcssUpTimeFile();

         printInfo("Finished startup");       
         Log.info("NtcssStart", "main", "NTCSS startup completed");
         
         // When this method returns, all the servers are guaranteed to be
         //  shutdown. Therefore, any "clean up" code can be done after this
         //  statement.
         activeServers.waitForCompletion();
         
         // Clear out entries in the current users table
         if (DataLib.clearCurrentUsersTable() == false)
            printWarn("Error clearing current users table!");
         
         // Remove any temporary files that may be left
         removeTemporaryFiles();
         
         exit(0);
      }
      catch (Exception exception) {
         Log.excp("NtcssStart", "main", exception.toString());
         
         exit(1);
      }
   } 
   
   /**
    * Sends the given string to the proper log sink
    */
   private static void printError(String str)
   {
      Log.error("NtcssStart", "start", str);
      System.out.println(str);
   }      
   private static void printInfo(String str)
   {
      Log.info("NtcssStart", "start", str);
      System.out.println(str);
   }   
   private static void printExcp(String str)
   {
      Log.excp("NtcssStart", "start", str);
      System.out.println(str);
   }
   private static void printWarn(String str)
   {
      Log.warn("NtcssStart", "start", str);
      System.out.println(str);
   }
   
   private static void exit(int exitValue)
   {
      if (exitValue != 0)
        printError("Cannot start");
      
      System.exit(exitValue);
   }
   
   /**
    * Parses through the given command line arguments
    */
   private static boolean parseArguments(String arguments[]) 
   {
      if (arguments == null)
         return true;
      
      // Start at the second argument because the first will always be
      //  the name of the properties file
      for (int i = 1; i < arguments.length; i++) {
         
         Log.info("NtcssStart", "parseArguments", 
                  "Received <" + arguments[i] + "> command line parameter");
         
         if (arguments[i].equals("-noboot") == true)
            start_boot_jobs = false;
         else if (arguments[i].equals("-noprime") == true)
            noPrime = true;
         else {
            printError("USAGE: ntcss_init {-noboot} {-noprime}");
            return false;
         }
      }
      
      return true;
   }
   
   /**
    * Perform checks on critical components that need to be in place. If
    *  everything checks out ok, true is returned.
    */
   private static boolean prelaunchCheck()
   {
      try {
         
         // Make sure critical paths and file exist
         if (checkCriticalPaths() == false) {
            printError("Check of critical paths failed");
            return false;
         }
         
         if (DatabaseConnectionManager.isEngineRunning() == false) {
            printError("Database engine connection check failed");
            return false;
         }
         
         return true;
      }
      catch (Exception exception) {   
         return false;
      }
   }
   
   /**
    * Checks to make sure some specific directories and files exists. If
    *  any of the items do not exist, false is returned. Otherwise, true is
    *  returned.
    */
   private static boolean checkCriticalPaths()
   {
      File fileToCheck;
      
      try {
         fileToCheck = new File(Constants.NTCSS_ROOT_DIR);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.NTCSS_ROOT_DIR + "> doesn't exist");
            return false;
         }
         
         fileToCheck = new File(Constants.NTCSS_BIN_DIR);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.NTCSS_BIN_DIR + "> doesn't exist");
            return false;
         }
         
         fileToCheck = new File(Constants.NTCSS_DB_DIR);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.NTCSS_DB_DIR + "> doesn't exist");
            return false;
         }
         
         fileToCheck = new File(Constants.NTCSS_PROGRP_INIT_FILE);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.NTCSS_PROGRP_INIT_FILE + "> doesn't exist");
            return false;
         }    
                  
         fileToCheck = new File(Constants.PRELAUNCH_FILE);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.PRELAUNCH_FILE + "> doesn't exist");
            return false;
         } 
         
         fileToCheck = new File(Constants.NATIVE_LIBRARY);
         if (fileToCheck.exists() == false) {
            printError("<" + Constants.NATIVE_LIBRARY + "> doesn't exist");
            return false;
         }
         
         return true;
      }
      catch (Exception exception) {      
         return false;
      }
   }
   
   /**
    * Check to see if the servers are already running
    */
   private static boolean areServersRunning()
   {
      ServerSocket tmpServer;
      
      try {
         
         // Try to create a server socket connecting to one of the server ports.
         //  If the server is already running on that port, this call will
         //  throw an exception.
         tmpServer = new ServerSocket(Constants.DB_SVR_PORT);
         tmpServer.close();
         
         return false;
      }
      catch (Exception exception) {
         return true;
      }                        
   }            
   
   /**
    * Checks to see if the local host is a replication server
    */
   private static boolean isReplicationOn()
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      boolean nReturn;
      String SQLString;
      Object item[];      
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null)             
            return false;         

         // Get the information from the database 
         item = new Object[1];
         item[0] = StringLib.getHostname().toUpperCase();
         SQLString = StringLib.SQLformat("SELECT replication_ind FROM hosts " +
                                         "WHERE upper(hostname) = " + Constants.ST, item);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
  
         // Get the type from the returned result set
         if (resultSet.next() == false)
            nReturn = false;
         else
            nReturn = resultSet.getBoolean("replication_ind");
         
         // Close the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return nReturn;
      }
      catch (Exception exception) {         
         return false;
      }
   }
   
   /**    
    * Creates a file which contains the date the NTCSS server was last started  
    */   
   private static void createNtcssUpTimeFile()
   {
      SimpleDateFormat dateFormatter;    
      FileWriter writer;
      
      try {
         dateFormatter = new SimpleDateFormat("E MMM d HH:mm:ss z yyyy");
      
         writer = new FileWriter(Constants.NTCSS_UP_TIME_FILE, false);
         writer.write(dateFormatter.format(new java.util.Date()));
         writer.close();                  
      }
      catch (Exception exception) {
         printExcp("Error creating time file <" + exception.toString() 
                            + ">");
      }
   }
   
   /**
    * Removes any temporary files that may be left around
    */
   private static void removeTemporaryFiles()
   {
      File logsDirectory;
      File fileList[];
      
      try {
         logsDirectory = new File(Constants.NTCSS_LOGS_DIR);
         
         // Remove files that end in ".query"
         fileList = logsDirectory.listFiles(new FileSuffixFilter("query"));         
         for (int i = 0; i < fileList.length; i++)
            fileList[i].delete();
                  
         // Remove files that contain ".ini." in their name
         fileList = logsDirectory.listFiles(new INIFilter());         
         for (int i = 0; i < fileList.length; i++)
            fileList[i].delete();
                  
         // Remove files that start with "UserAdmin" in their name
         fileList = logsDirectory.listFiles(new UserAdminFileFilter());         
         for (int i = 0; i < fileList.length; i++)
            fileList[i].delete();
      }
      catch (Exception exception) {
         Log.excp("NtcssStart", "removeTemporaryFiles", exception.toString());
      }
   }
   
   /**
    * If this host is not the master server, an attempt is made to replace
    *  the local progrps.ini file with the one from the master if they are
    *  different.
    */
   private static void getMasterProgrpsFile()
   {
      String fnct_name = "getMasterProgrpsFile";
      StringBuffer masterName;
      String tmpFile;
      File fileDescp;
      int ret;
      
      try {
         
         // Don't need to do this operation if this host is already the
         //  master. NOTE: it is important to get the master name from NDS since
         //  it is possible that the local database is out of date.
         masterName = new StringBuffer();
         if ((ret = NDSLib.getMasterServer(masterName)) == 1) 
            return;
         
         if (ret == ErrorCodes.ERROR_CANT_DETER_MASTER) {
            Log.warn("NtcssStart", fnct_name, "Unable to determine master " + 
                     "server. Cannot retreive progrps file.");
            return;
         }                  
                
         // Get the master server's progrps.ini file
         Log.info("NtcssStart", fnct_name, "Transfering progrps file from <" + 
                                            masterName.toString() + ">");
         tmpFile = Constants.NTCSS_LOGS_DIR + "/progrps.ini";
         if (FTP.get(masterName.toString(), "/h/NTCSSS/database/progrps.ini",
                     tmpFile, false) == false) {
            Log.error("NtcssStart", fnct_name, 
                    "Unable to get master's progrps.ini file");                        
            return;
         }
        
         fileDescp = new File(tmpFile);
         
         // Compare the master's file and the local version
         if (FileOps.diff(tmpFile, Constants.NTCSS_PROGRP_INIT_FILE, false) == false) {
            Log.info("NtcssStart", fnct_name, 
                     "Master's progrps file not different from local copy");
            fileDescp.delete();      
            return;
         }
               
         // Copy the master's progrps.ini file to the init_data directory
         Log.info("NtcssStart", fnct_name, 
                  "Copying master's progrps file to local dir");
         if (FileOps.copy(tmpFile, Constants.NTCSS_PROGRP_INIT_FILE) == false) {
            Log.error("NtcssStart", fnct_name, 
                    "Error copying master's prgrps.ini file");
            fileDescp.delete();
            return;
         }
         
         // Remove the temporary file         
         fileDescp.delete();
      }
      catch (Exception exception) {
         Log.excp("NtcssStart", fnct_name, exception.toString());
      }
   }
   
   /**
    * Reads the printer types from the file in the database directory, and
    *  merges the records with the ones in the database.
    */
   private static boolean importPrinterTypes()
   {
      BufferedReader reader = null;
      String fnct_name = "importPrinterTypes";
      String type, model, driver_name;
      Hashtable typesFromFile;
      Vector record;
      Connection connection = null;
      Statement statement = null;
      ResultSet resultSet;
      Hashtable typesFromDB;
      Object sqlItems[];
      Enumeration typeEnumeration;      
      Boolean deleted;
      
      try {
         
         // Read the records out of the file
         typesFromFile = new Hashtable();
         reader = new BufferedReader(new FileReader(Constants.PRT_TYPES_FILE));         
         do {
            
            // Get the first field and make sure there is data in it
            if (((type = reader.readLine()) == null) ||
                (type.trim().length() == 0))
               break;
            
            // Read the rest of the fields
            if (((model = reader.readLine()) == null) || 
                ((driver_name = reader.readLine()) == null) ||
                (reader.readLine() == null)) { // Record separator 
               Log.error("NtcssStart", fnct_name, "Invalid file format");
               return false;
            }
            
            record = new Vector();
            record.add(model);
            record.add(driver_name);
            
            typesFromFile.put(type, record);
         } while(true);
         
         
         // Get the list of printer types from the database
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            Log.error("NtcssStart", fnct_name, "Unable to get database connection");
            return false;
         }
         statement = connection.createStatement();
         
         typesFromDB = new Hashtable();
         resultSet = statement.executeQuery("SELECT printer_type FROM printer_types");
         while (resultSet.next() == true) 
            typesFromDB.put(resultSet.getString("printer_type"), new Boolean(false)); 
         resultSet.close();
         
         
         // Delete from the database any records that aren't in the file
         sqlItems = new Object[3];
         typeEnumeration = typesFromDB.keys();
         while (typeEnumeration.hasMoreElements() == true) {
            type = (String)typeEnumeration.nextElement();
            
            if (typesFromFile.containsKey(type) == false) {
               sqlItems[0] = type;
//               statement.executeUpdate(StringLib.SQLformat(
//                  "DELETE FROM printer_types WHERE printer_type = " + Constants.ST, sqlItems));
               DataLib.cascadeDelete(statement, "printer_types", StringLib.SQLformat(
                  "printer_type = " + Constants.ST, sqlItems));               
               
               typesFromDB.put(type, new Boolean(true));
            }
         }
         
         // Insert/update the records from the file into the database
         typeEnumeration = typesFromFile.keys();
         while (typeEnumeration.hasMoreElements() == true) {
            type = (String)typeEnumeration.nextElement();   
            record = (Vector)typesFromFile.get(type);               
            
            deleted = (Boolean)typesFromDB.get(type);
            if ((deleted == null) || (deleted.booleanValue() == true)) { // Insert the record             
               sqlItems[0] = type;
               sqlItems[1] = (String)record.elementAt(0);
               sqlItems[2] = (String)record.elementAt(1);
               statement.executeUpdate(StringLib.SQLformat(
                 "INSERT INTO printer_types (printer_type, model, driver_name) " +
                 " VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + ")", sqlItems));
            }
            else { // Update the record               
               sqlItems[0] = (String)record.elementAt(0);
               sqlItems[1] = (String)record.elementAt(1);
               sqlItems[2] = type;
               statement.executeUpdate(StringLib.SQLformat(
                 "UPDATE printer_types SET model = " + Constants.ST + 
                 ", driver_name = " + Constants.ST + " WHERE printer_type = " + 
                 Constants.ST, sqlItems));
            }   
         }
         
         return true;                           
      }
      catch (Exception exception) {
         Log.excp("NtcssStart", fnct_name, exception.toString());
         
         return false;
      }
      finally {
         
         try {   
            if (reader != null)
               reader.close();
            
            if (statement != null)
               statement.close();
            
            if (connection != null)
               DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
         }
      }
   }
   
   /**
    * Reads the adapter types from the file in the database directory, and
    *  merges the records with the ones in the database.
    */
   private static boolean importAdapterTypes()
   {
      BufferedReader reader = null;
      String fnct_name = "importAdapterTypes";
      String type, ports, port, setup_program;
      Hashtable typesFromFile;
      Vector record;
      Connection connection = null;
      Statement statement = null;
      ResultSet resultSet;
      Hashtable typesFromDB;
      Object sqlItems[];
      Enumeration typeEnumeration;      
      Boolean deleted;
      StringTokenizer tokenizer;
      int numParallelPorts, numSerialPorts;
      
      try {
         
         // Read the records out of the file
         typesFromFile = new Hashtable();
         reader = new BufferedReader(new FileReader(Constants.ADAPTER_TYPES_FILE));         
         do {
            
            // Get the first field and make sure there is data in it
            if (((type = reader.readLine()) == null) ||
                (type.trim().length() == 0))
               break;
            
            // Read the rest of the fields
            if (((ports = reader.readLine()) == null) || 
                ((setup_program = reader.readLine()) == null) ||
                (reader.readLine() == null)) { // Record separator 
               Log.error("NtcssStart", fnct_name, "Invalid file format");
               return false;
            }
            
            record = new Vector();
            record.add(ports);
            record.add(setup_program);
            
            typesFromFile.put(type, record);
         } while(true);
         
         
         // Get the list of adapter types from the database
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            Log.error("NtcssStart", fnct_name, "Unable to get database connection");
            return false;
         }
         statement = connection.createStatement();
         
         typesFromDB = new Hashtable();
         resultSet = statement.executeQuery("SELECT type FROM adapter_types");
         while (resultSet.next() == true) 
            typesFromDB.put(resultSet.getString("type"), new Boolean(false)); 
         resultSet.close();
         
         
         // Delete from the database any records that aren't in the file
         sqlItems = new Object[4];
         typeEnumeration = typesFromDB.keys();
         while (typeEnumeration.hasMoreElements() == true) {
            type = (String)typeEnumeration.nextElement();
            
            if (typesFromFile.containsKey(type) == false) {
               sqlItems[0] = type;
//               statement.executeUpdate(StringLib.SQLformat(
//                  "DELETE FROM adapter_types WHERE type = " + Constants.ST, sqlItems));
               DataLib.cascadeDelete(statement, "adapter_types", StringLib.SQLformat(
                  "type = " + Constants.ST, sqlItems));               
               
               typesFromDB.put(type, new Boolean(true));
            }
         }
         
         // Insert/update the records from the file into the database
         typeEnumeration = typesFromFile.keys();
         while (typeEnumeration.hasMoreElements() == true) {
            type = (String)typeEnumeration.nextElement();   
            record = (Vector)typesFromFile.get(type);    
            
            // Determine the number of each type of port
            ports = (String)record.elementAt(0);
            numParallelPorts = 0;
            numSerialPorts = 0;
            tokenizer = new StringTokenizer(ports);
            while (tokenizer.hasMoreTokens() == true) {
               port = tokenizer.nextToken();
               
               if ((port.charAt(0) == 'P') || (port.charAt(0) == 'p'))
                  numParallelPorts++;
               else if ((port.charAt(0) == 'S') || (port.charAt(0) == 's'))
                  numSerialPorts++;
            }
            
            deleted = (Boolean)typesFromDB.get(type);
            if ((deleted == null) || (deleted.booleanValue() == true)) { // Insert the record             
               sqlItems[0] = type;
               sqlItems[1] = new Integer(numParallelPorts);
               sqlItems[2] = new Integer(numSerialPorts);
               sqlItems[3] = (String)record.elementAt(1);
               statement.executeUpdate(StringLib.SQLformat(
                 "INSERT INTO adapter_types (type, num_parallel_ports, num_serial_ports, setup_program) " +
                 " VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + ")", sqlItems));
            }
            else { // Update the record               
               sqlItems[0] = new Integer(numParallelPorts);
               sqlItems[1] = new Integer(numSerialPorts);
               sqlItems[2] = (String)record.elementAt(1);
               sqlItems[3] = type;
               statement.executeUpdate(StringLib.SQLformat(
                 "UPDATE adapter_types SET num_parallel_ports = " + Constants.ST + 
                 ", num_serial_ports = " + Constants.ST + ", setup_program = " + 
                 Constants.ST + " WHERE type = " + 
                 Constants.ST, sqlItems));
            }   
         }
         
         return true;                           
      }
      catch (Exception exception) {
         Log.excp("NtcssStart", fnct_name, exception.toString());
         
         return false;
      }
      finally {
         
         try {   
            if (reader != null)
               reader.close();
            
            if (statement != null)
               statement.close();
            
            if (connection != null)
               DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
         }
      }
   }
}

/**
 * A data structure that keeps track of the number of servers that are still 
 *  active
 */
class ActiveServers
{    
   private int activeCount;
   
   /**
    * Constructor
    */
   public ActiveServers()
   {
      activeCount = NtcssStart.NUM_SERVERS;
   }
   
   /**
    * Decrements the active worker count by one
    */
   public synchronized void decrement()
   {      
      activeCount--;
      
      // If the active count is now zero, notify anyone who may be waiting
      //  for this state
      if (activeCount == 0)
         notify();
   }
      
   /**
    * This method will cause the calling thread to wait until all the 
    *  servers have finished.        
    */
   public synchronized void waitForCompletion()
   {
      try {
         
         // Only need to wait if the active count is greater than 0
         if (activeCount > 0)                          
            wait();                                                     
      }
      catch (Exception exception) {
         Log.excp("ActiveServers", "waitForCompletion", exception.toString());                           
      }
   }      
}

/**
 * Implements a filter for files with the given suffix  (e.g. "query", "log").
 *  For the accept method to return true, the specified suffix must follow
 *  the "." character in the filename (e.g. "dbtmp.log").
 */
class FileSuffixFilter implements FilenameFilter
{
   private String suffix;
   
   public FileSuffixFilter(String suffix)
   {
      this.suffix = "." + suffix;
   }
   
   public boolean accept(File fileDirectory, String filename)
   {
      try {
         
         if (filename.length() > suffix.length()) 
            return filename.substring(filename.length() - suffix.length()).equals(suffix);
         else
            return false;
      }
      catch (Exception exception) {
         return false;
      }
   }
}

/**
 * Implements a filter for files in which the string ".ini." appear in the name.
 */
class INIFilter implements FilenameFilter
{
   public boolean accept(File fileDirectory, String filename)
   {
      try {
         
         if (filename.indexOf(".ini.") != -1) 
            return true;
         else
            return false;
      }
      catch (Exception exception) {
         return false;
      }
   }
}

/**
 * Implements a filter for filenames which begin with the string "UserAdmin".
 */
class UserAdminFileFilter implements FilenameFilter
{
   private String prefix = "UserAdmin";
   
   public boolean accept(File fileDirectory, String filename)
   {
      try {
         
         if (filename.length() >= prefix.length()) 
            return filename.substring(0, prefix.length()).equals(prefix);
         else
            return false;
      }
      catch (Exception exception) {
         return false;
      }
   }
}