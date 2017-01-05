/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.database;

import java.net.Socket;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.FileReader;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

import java.util.StringTokenizer;
import java.util.Vector;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;

import COMMON.INI;

import ntcsss.log.Log;

import ntcsss.libs.Constants;
import ntcsss.libs.DataLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.SocketLib;
import ntcsss.libs.NDSLib;
import ntcsss.libs.CryptLib;
import ntcsss.libs.UserLib;

import ntcsss.util.FileOps;

import ntcsss.tools.SQLTool;

/**
 * Updates the host and application data in the database with the information
 *  from the progrps.ini file
 */
public class PrimeDB
{   
   private PrintWriter writer = null;
   
   /**
    * Starts the priming process
    */
   public boolean start()
   {
      String fnct_name = "start";
      INI ini;
      Vector newBatchUsersList;
      
      try {
         
         // Open the prime log file
         writer = new PrintWriter(new FileWriter(
                          Constants.NTCSS_LOGS_DIR + "/prime_db.log", false));
         
         print("Database initialization started.");

         // Make sure the servers aren't awake
         if (areServersAwake() == true) {
            print("Database initialization halted! NTCSS daemons are not idled!");
            writer.close();
            return false;
         }         
         
         // Check to see if any database upgrades need to be done
         print("Checking to see if database files need to be " + 
                       "upgraded to version " + Constants.NTCSS_DB_VERSION);
         if (upgradeDatabase() == false) {
            print("Database initialization halted! Conversion of " +
                         "database files has failed!");
            writer.close();
            return false;
         }         

         // Update the database with the hosts in the progrps.ini file
         ini = new INI();
         print("Processing host info...");
         if (updateHostData(ini) == false) {
            print("Database initialization halted! Cannot interpret host" +
                  " information!");
            writer.close();
            return false;
         }

         // Update the database with the applications in the progrps.ini file
         print("Processing application info...");
         newBatchUsersList = new Vector();
         if (updateApplicationData(ini, newBatchUsersList) == false) {
            print("Database initialization halted! Cannot interpret" +
                  " application information!");
            writer.close();
            return false;
         }

         // Remove old application bulletin boards and add new application
         //  bulletin boards
         print("Checking message bulletin boards data...");
         if (updateBulletinBoards(newBatchUsersList) == false) {    
            print("Database initialization halted! Cannot modify message" +
                  " bulletin board data correctly.");
            writer.close();
            return false;
         }
         
         // If the base NTCSS user doesn't exist, add it
         if (addBaseNtcssUser() == false) {
            print("Database initialization halted! Error adding base " + 
                  "NTCSS user.");
            writer.close();
            return false;
         }

         print("Database initialization completed successfully.");         

         // Delete the cached progroup data file
         removeProgroupCacheFile();
         
         // Copy the init_data/progrps.ini file to the database directory
         if (FileOps.copy(Constants.NTCSS_PROGRP_INIT_FILE, 
                          Constants.NTCSS_PROGRP_DB_FILE) == false)                  
            print("Error copying file");         

         writer.close();
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("PrimeDB", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Checks whether prime needs to be run. If it does, true is returned.
    *  Otherwise, false is returned.
    */
   public boolean checkRunNecessity()
   {
      String fnct_name = "checkRunNecessity";
      File initDataFile, dbFile;
      
      try {
         
         // Check to make sure the progrps.ini file in the init_data dir exists
         initDataFile = new File(Constants.NTCSS_PROGRP_INIT_FILE);
         if (initDataFile.exists() == false) {
            Log.error("PrimeDB", fnct_name, Constants.NTCSS_PROGRP_INIT_FILE + 
                      " does not exist");
            return false;
         }
                           
         // Check to make sure the progrps.ini file in the database dir exists
         dbFile = new File(Constants.NTCSS_PROGRP_DB_FILE);
         if (dbFile.exists() == false) 
            return true;         
         
         // Compare the modification times of the two files
         if ((initDataFile.lastModified() - dbFile.lastModified()) > 0) {
            Log.info("PrimeDB", fnct_name, "Found newer Progoups Ini file. " +
                      "Running prime ...");
            return true;
         }
         else
            return false;
      }
      catch (Exception exception) {
         Log.excp("PrimeDB", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Returns true if the servers on the local host are in the "awake" state
    */
   public boolean areServersAwake()
   {
      Socket socket;
      String message;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;  
      byte buffer[];
      int bytesRead;
      String responseStr;
      boolean ret = true;
      
      try {
         
         // Create a connection to the local host
         socket = new Socket("localhost", Constants.DSK_SVR_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());
         
         // Construct the message to send
         message = StringLib.formatMessage("SERVERSTATUS", 
                      String.valueOf(Constants.SERVER_STATE_CHECK) + "\0");
         
         // Send the given message
         outputStream.write(message.getBytes(), 0, message.length());
         outputStream.flush();
         
         // Read the response
         buffer = new byte[50];
         socket.setSoTimeout(Constants.SOCKET_READ_TIMEOUT);
         if ((bytesRead = SocketLib.readStream(inputStream, 
                                               buffer, buffer.length)) <= 0)
            ret = false;
         else {
         
            // Check the response
            responseStr = new String(buffer, Constants.CMD_LEN + 1, 1);   
            if (Integer.parseInt(responseStr) != Constants.SERVER_STATE_AWAKE)
               ret = false;
         }
         
         // Close the resources
         outputStream.close();
         inputStream.close();
         socket.close();
         
         return ret;
      }
      catch (Exception exception) {
         return false;
      }
   }
   
   /**
    * Sends the given string to the log file
    */
   private void print(String str)
   {
      try {
         writer.println(str);
      }
      catch (Exception exception) {
      }
   }
   
   /**
    * Performs any database upgrades. Returns true if no errors occured.
    */
   private boolean upgradeDatabase()
   {
      String currentVersion;
      boolean finished;
      String newVersion;
      
      try {
         // Get the current NTCSS database version
         if ((currentVersion = DataLib.getNtcssDatabaseVersion()) == null)
            currentVersion = "0.0";
         
         // Convert database one version at a time until it becomes 
         //  the most recent version.
         finished = false;
         do {
            
            // Check to see if the database has been upgraded to the lastest
            //  release's verion
            if (currentVersion.equals(Constants.NTCSS_DB_VERSION) == true)
               finished = true;
            else {
               
               print("Converting the databases from version " + currentVersion);
            
               if ((newVersion = convertVersion(currentVersion)) == null) {
                  print("Failed to convert database from version " + 
                            currentVersion);
                  return false;
               }
            
               currentVersion = newVersion;
            }
            
         } while (finished == false);
         
         // Update the VERSION file
         print("Setting the new database version file...");                  
         if (DataLib.writeNtcssDatabaseVersion() == false) 
            print("Cannot create new database version file!");
         
         return true;
      }
      catch (Exception exception) {      
         print("Error upgrading database <" + exception.toString() + ">");
         return false;
      }
   }
   
   /**
    * Convert the current version up to the next release version
    */
   private String convertVersion(String currentVersion)
   {
      try {
         
         if (currentVersion.equals("0.0") == true) {
            
            // Execute the script that will create the NTCSS RDBMS 
            //  database tables
            if (SQLTool.executeScript(Constants.CREATE_TABLES_SCRIPT) == false)
               return null;
            else
               return Constants.NTCSS_DB_VERSION;
         }
         // else if (currentVersion.equals(...
         // else if (currentVersion.equals(...
         else {
            print("Unrecognized database version! Cannot convert from " + 
                         currentVersion + "!");
            return null;
         }
      }
      catch (Exception exception) {
         print("Error converting version <" + exception.toString() + ">");
         return null;
      }
   }
   
   /**
    * Removes old user and application bulletin boards and adds any new
    *  application bulletin boards
    */
   private boolean updateBulletinBoards(Vector newBatchUsersList) 
   {
      Connection connection;
      Statement statement;
      StringTokenizer tokenizer;
      String batchUser, app;
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {       
            print("Error obtaining db connection");
            return false; 
         }         
         statement = connection.createStatement();
         
         // Remove from the bulletin_boards table any user bulletin boards where
         //  the user no longer exists.         
//         statement.executeUpdate("DELETE FROM bulletin_boards WHERE bb_name " +
//               "= creator AND creator NOT IN (SELECT login_name FROM users)");
         DataLib.cascadeDelete(statement, "bulletin_boards",  "bb_name " +
               "= creator AND creator NOT IN (SELECT login_name FROM users)");
         
         // Remove application bulletin boards of applications that don't 
         //  exist any more         
         statement.executeUpdate("CREATE TABLE tmp1 (bb_name TEXT NOT NULL)");
         statement.executeUpdate("INSERT INTO tmp1 (bb_name) " +
             "SELECT trim(trailing '.adm' from bb_name) FROM bulletin_boards WHERE " +
             "bb_name LIKE '%.adm'");
         statement.executeUpdate("DELETE FROM tmp1 WHERE bb_name IN " + 
                                 "(SELECT app_title FROM apps)");         
//         statement.executeUpdate("DELETE FROM bulletin_boards WHERE " +
//             "trim(trailing '.adm' from bb_name) IN (SELECT * FROM tmp1)");
         DataLib.cascadeDelete(statement, "bulletin_boards", 
             "trim(trailing '.adm' from bb_name) IN (SELECT * FROM tmp1)");         
//         statement.executeUpdate("DELETE FROM bulletin_boards WHERE " +
//             "trim(trailing '.all' from bb_name) IN (SELECT * FROM tmp1)");         
         DataLib.cascadeDelete(statement, "bulletin_boards", 
             "trim(trailing '.all' from bb_name) IN (SELECT * FROM tmp1)");                  
         statement.executeUpdate("DROP TABLE tmp1");
         
         // Insert application bulletin boards into the bulletin_boards table 
         //  that don't exist yet.  
         statement.executeUpdate("CREATE TABLE tmp2 (bb_name TEXT NOT NULL)");
         statement.executeUpdate("INSERT INTO tmp2 (bb_name) " +
             "SELECT app_title FROM apps");         
         statement.executeUpdate ("DELETE FROM tmp2 WHERE bb_name IN " +
            "(SELECT trim(trailing '.all' from bb_name) FROM " +
             "bulletin_boards WHERE bb_name LIKE '%.all')");         
         statement.executeUpdate("INSERT INTO bulletin_boards (bb_name, creator) " +
            "SELECT CONCAT(bb_name, '.adm'), 'SYSTEM' FROM tmp2");
         statement.executeUpdate("INSERT INTO bulletin_boards (bb_name, creator) " +
            "SELECT CONCAT(bb_name, '.all'), 'SYSTEM' FROM tmp2");         
         statement.executeUpdate("DROP TABLE tmp2");
         
         // Due to the nature of the RDBMS (field constraints & cascade 
         //  deletes), the bb_subscribers, bb_read_history, bb_messages, and 
         //  bb_message_map table entries should not have to be checked for 
         //  validity 
         
         // Release the database resources
         statement.close();
                  
         // Add any new batch users to their proper bulletin boards
         for (int i = 0; i < newBatchUsersList.size(); i++) {
            tokenizer = new StringTokenizer(
                                  (String)newBatchUsersList.elementAt(i), "/");
            batchUser = tokenizer.nextToken();
            app = tokenizer.nextToken();
            
            DataLib.setupStandardBBUserInfo(batchUser, false, connection);
            
            DataLib.subscribeToAppBulletinBoard(batchUser, app, connection);
         }
         
         // Release the database resources
         DatabaseConnectionManager.releaseConnection(connection);
         
         return true;
      }
      catch (Exception exception) {  
         print("Error updating bulletin boards <" + exception.toString() + ">");
         return false;
      }
   }
   
   /**
    * Removes the progroup cache file if it exists
    */
   private void removeProgroupCacheFile()
   {
      File fileDescp;
      
      try {
         fileDescp = new File(Constants.PROGROUP_CACHE_FILE);
         if (fileDescp.exists() == true)
            fileDescp.delete();
      }
      catch (Exception exception) {
         print("Error removing cache file <" + exception.toString() + ">");   
      }
   }
   
   /**
    * Updates the host data in the database with the information in the 
    *  progrps.ini file. If no errors occur, true is returned.
    */
   private boolean updateHostData(INI ini)
   {
      Connection connection;
      Statement statement;      
      String hostSections;
      StringTokenizer tokenizer, entryTokenizer;
      String hostSection;                        
      String hostListStr = new String();
      HostInfo hostInfo;
      
      try {   
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            print("Unable to get DB connection for host update");
            return false;
         }         
         
         // Get all the items in the HOSTS section         
         hostSections = ini.GetProfileSection("HOSTS", 
                                             Constants.NTCSS_PROGRP_INIT_FILE);
         tokenizer = new StringTokenizer(hostSections, INI.STRINGS_DELIMITER);
         
         // Parse through the items                  
         while (tokenizer.hasMoreElements() == true) {
            
            // Find the section that matches the first item
            entryTokenizer = new StringTokenizer(tokenizer.nextToken(), 
                                                 INI.TAG_VALUE_DELIMITER);
            hostSection = entryTokenizer.nextToken();
                        
            // Get the information about the current host
            hostInfo = new HostInfo();
            if (hostInfo.load(ini, hostSection) == false) {
               print("Error loading host info for <" + hostSection + ">");
               continue;
            }
            
            // Validate the host's fields
            if (hostInfo.verifyFields() == false) {
               print("Error validating fields for <" + hostInfo.hostname + ">");
               continue;
            }                        
            
            // Add the host to the database
            if (hostInfo.add(connection) == false) {
               print("Error adding info for <" + hostInfo.hostname + ">");
               continue;
            }
            
            // Add the host to the list of hostnames to be used later
            if (hostListStr.length() == 0)
               hostListStr += "'" + 
                              StringLib.escapeCharacters(hostInfo.hostname) + "'";
            else
               hostListStr += ",'" + 
                              StringLib.escapeCharacters(hostInfo.hostname) + "'";
         }
         
         // Remove the hosts from the database that aren't in the 
         //  progrps.ini file         
         statement = connection.createStatement();
         if (hostListStr.length() != 0)            
//            statement.executeUpdate("DELETE FROM hosts WHERE hostname NOT IN (" 
//                                    + hostListStr + ")");
            DataLib.cascadeDelete(statement, "hosts", "hostname NOT IN (" 
                                    + hostListStr + ")");            
         
         // Release the database resources
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Update the cached master server name       
         DataLib.cacheMasterName();
         
         // If this is the master server, update the NDS data
         if (DataLib.isMasterServer() == true) {
            if (NDSLib.refreshHostData() == false)
               print("Error updating NDS data");
         }
         
         return true;
      }
      catch (Exception exception) {         
         print("Error updating hosts <" + exception.toString() + ">"); 
         return false;
      }      
   }
   
   /**
    * Updates the application data in the database with the information in the 
    *  progrps.ini file. If no errors occur, true is returned.
    */
   private boolean updateApplicationData(INI ini, Vector newBatchUsersList)
   {
      Connection connection = null;      
      String appsSections;
      StringTokenizer tokenizer;      
      StringTokenizer entryTokenizer;
      String appSection;
      String appName;
      AppInfo appInfo;     
      String appListStr = new String();
      String batchUserListStr = new String();
      String groupListStr = new String();
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      
      try {      
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            print("Unable to get DB connection for host update");
            return false;
         }
                  
         // Get all the items in the APPS section         
         appsSections = ini.GetProfileSection("APPS", 
                                             Constants.NTCSS_PROGRP_INIT_FILE);
         tokenizer = new StringTokenizer(appsSections, INI.STRINGS_DELIMITER);
         
         // Parse through the items         
         while (tokenizer.hasMoreElements() == true) {
            
            // Find the section that matches the first item
            entryTokenizer = new StringTokenizer(tokenizer.nextToken(), 
                                                 INI.TAG_VALUE_DELIMITER);
            appSection = entryTokenizer.nextToken();
            
            print("-------------------------------------------------------");
            print("Verifying information on program group <" + 
                  appSection + ">...");                                            
            
            // Get the information from the progrps.ini file
            appInfo = new AppInfo();
            if (appInfo.load(ini, appSection) == false) {
                print("ERROR: Cannot read progroup record for <" + 
                      appSection + ">");
                continue;
            }
            
            // Validate all the fields
            if (appInfo.verifyFields(ini) == false) {
               print("ERROR: Cannot verify progroup record for <" + 
                      appSection + ">");
               continue;
            }
            
            // Apply the application information to the database            
            if (appInfo.add(connection) == false) {
               print("Problems modifying program group <" + appInfo.app_title + 
                     ">");  
               continue;
            }
            
            // Create the system group
            if (StringLib.isBlank(appInfo.unix_group) == false) {
                                   
               // Add the system group to the list of groups to be used later
               if (groupListStr.length() == 0)
                  groupListStr += "'" + 
                     StringLib.escapeCharacters(appInfo.unix_group) + "'";
               else
                  groupListStr += ",'" + 
                     StringLib.escapeCharacters(appInfo.unix_group) + "'"; 
               
               if (UserLib.addSystemGroup(appInfo.unix_group) == false)
                  print("WARNING: May have to manually add UNIX group <" + 
                         appInfo.unix_group + ">");
            }
            else
               print("WARNING: Need to manually add the intended UNIX group " +
                     "for progroup <" + appInfo.app_title + ">");            

            // Add given batch user
            if (StringLib.isBlank(appInfo.batch_user) == false) {    
               
               // Add the batch user to the list of batch users to be used later
               if (batchUserListStr.length() == 0)
                  batchUserListStr += "'" + 
                     StringLib.escapeCharacters(appInfo.batch_user) + "'";
               else
                  batchUserListStr += ",'" + 
                     StringLib.escapeCharacters(appInfo.batch_user) + "'";
               
               if (appInfo.addBatchUser(connection, ini, 
                                        newBatchUsersList) == false) 
                  print("Failed to add/edit user");                              
            }            
            
            // Create new app roles
            print(""); // Newline
            print("Creating access roles for program group <" + 
                  appInfo.app_title + ">...");    
            if (updateApplicationRoles(ini, appSection, connection, 
                                       appInfo.app_title) == false) 
               print("Error updating roles");            

            
            // Add new application programs
            print(""); // Newline
            print("Setting up programs for program group <" + appInfo.app_title 
                  + ">...");                        
            if (updateApplicationPrograms(ini, appSection, connection,
                                          appInfo.app_title) == false)
               print("Error updating programs");
            
            // Check predefined job data
            print(""); // Newline
            print("Checking Predefined Jobs..."); 
            if (updateApplicationPDJs(ini, appSection, connection,
                                      appInfo.app_title, 
                                      appInfo.server_location) == false)
               print("Error updating application PDJs");
            
            // Add the application to the list of applications to be used later
            if (appListStr.length() == 0)
               appListStr += "'" + 
                     StringLib.escapeCharacters(appInfo.app_title) + "'";
            else
               appListStr += ",'" + 
                     StringLib.escapeCharacters(appInfo.app_title) + "'";
         }   
         
         print("-------------------------------------------------------");
         
         // Remove orphaned batch users. If cascade delete is on in the
         //  database, the records should be removed automatically from 
         //  app_users table also.
         print("Removing any orphaned batch users...");
         statement = connection.createStatement();
         if (batchUserListStr.length() != 0) {
            
            // Get the orphaned users
            sqlItems = new Object[1];
            sqlItems[0] = new Boolean(true);                                                                                 
            resultSet = statement.executeQuery(StringLib.SQLformat(
                 "SELECT login_name, unix_group FROM app_users, apps " +
                 "WHERE batch_user = " + Constants.ST + " AND " + 
                 "login_name NOT IN ", sqlItems) + "(" + batchUserListStr + 
                 ") AND app_users.app_title = apps.app_title");
               
            // Loop through the orphaned batch users
            while (resultSet.next() == true)  {
               
               // If this is the master server remove the batch user from NDS
               if (DataLib.isMasterServer() == true) {  
                  if (NDSLib.deleteUser(resultSet.getString("login_name")) == false)
                     print("Error removing batch user <" + 
                           resultSet.getString("login_name") + "> from user NDS");
                  
                  if (NDSLib.removeUserFromGroup(resultSet.getString("login_name"),
                                          resultSet.getString("unix_group")) == false)
                     print("Error removing batch user <" + 
                           resultSet.getString("login_name") + "> from group NDS");
               }
               
               // Remove the orphaned batch user from the operating system    
               if (UserLib.removeBatchUserFromSystem(resultSet.getString("login_name")) == false)
                  print("Error removing batch user <" + 
                        resultSet.getString("login_name") + "> from the system");
            }               
            resultSet.close();            
            
            // Remove the users from the local database                       
//            statement.executeUpdate(StringLib.SQLformat("DELETE FROM users " +
//               "WHERE users.login_name IN (SELECT users.login_name " +
//               "FROM users, app_users WHERE users.login_name = app_users.login_name " +
//               "AND app_users.batch_user = " + Constants.ST + " AND " + 
//               "users.login_name NOT IN ", sqlItems) + "(" + batchUserListStr + "))");                        
            DataLib.cascadeDelete(statement, "users", StringLib.SQLformat(
               "users.login_name IN (SELECT users.login_name " +
               "FROM users, app_users WHERE users.login_name = app_users.login_name " +
               "AND app_users.batch_user = " + Constants.ST + " AND " + 
               "users.login_name NOT IN ", sqlItems) + "(" + batchUserListStr + "))");                                    
         }
         
         // Remove orphaned system groups
         print("Removing any orphaned system groups...");
         if (groupListStr.length() != 0) {
            
            // Get the orphaned system groups
            resultSet = statement.executeQuery("SELECT unix_group FROM apps " +
                 "WHERE unix_group NOT IN (" + groupListStr + ")");
               
            // Loop through the orphaned groups
            while (resultSet.next() == true)  {
               
               // Remove the orphaned system group from the operating system    
               if (UserLib.removeSystemGroup(resultSet.getString("unix_group")) == false)
                  print("Error removing group <" + 
                        resultSet.getString("unix_group") + "> from the system");
            }               
            resultSet.close();                                                            
         }
         
         // Remove orphaned applications    
         print("Removing any orphaned applications...");
         if (appListStr.length() != 0) 
//            statement.executeUpdate("DELETE FROM apps WHERE app_title NOT IN " +
//                             "(" + appListStr + ")");                          
            DataLib.cascadeDelete(statement, "apps", "app_title NOT IN " +
                             "(" + appListStr + ")");                                      
         
         statement.close(); 
         
         print("-------------------------------------------------------");
         print("Finished checking all progroups.");
         
         // Release the database resources         
         DatabaseConnectionManager.releaseConnection(connection);
         
         return true;
      }      
      catch (Exception exception) {
         print("Error updating applications <" + exception.toString() + ">");
         return false;
      }
   }
   
   /**
    * Updates the database role information for the given application
    *  with information from the progrps.ini file
    */
   private boolean updateApplicationRoles(INI ini, String appSection,                                            
                                          Connection connection,
                                          String app_title)
   {
      AppRoleInfo appRoleInfo;
      int roleNumber;
      String roleSection;
      String rolesSections;
      String rolesListStr = new String();      
      StringTokenizer rolesTokenizer, entryTokenizer;
      Statement statement;
      
      try {
         
         roleNumber = 1;
            
         // Get the roles section for the given application
         rolesSections = ini.GetProfileSection(appSection + "_ROLES", 
                                          Constants.NTCSS_PROGRP_INIT_FILE);
         if (rolesSections == null) 
            print("Could not find " + appSection + "_ROLES section");            
         else {
         
            rolesTokenizer = new StringTokenizer(rolesSections, 
                                                 INI.STRINGS_DELIMITER);            
            while (rolesTokenizer.hasMoreTokens() == true) {
               entryTokenizer = new StringTokenizer(rolesTokenizer.nextToken(),
                                                    INI.TAG_VALUE_DELIMITER);
               roleSection  = entryTokenizer.nextToken();
               
               // Load the role information
               appRoleInfo = new AppRoleInfo();
               if (appRoleInfo.load(ini, roleSection) == false) {
                  print("Could not get access roles section <" + roleSection + ">");
                  continue;
               }
               
               // Verify the role information               
               if (appRoleInfo.verifyFields() == false) {
                  print("Could not verify access roles section <" + roleSection+ ">");
                  continue;
               }
               
               // Insert or update the role information
               if (appRoleInfo.add(connection, app_title, 
                                   roleNumber++) == false) {
                  print("Could not add access role <" + roleSection + ">");
                  continue;
               }
                           
               // Add the role to the list of roles to be used later
               if (rolesListStr.length() == 0)
                  rolesListStr += "'" + 
                        StringLib.escapeCharacters(appRoleInfo.roleName) + "'";
               else
                  rolesListStr += ",'" + 
                        StringLib.escapeCharacters(appRoleInfo.roleName) + "'";
            }
         }
            
         // Remove the roles that are orphaned
         statement = connection.createStatement();
         if (rolesListStr.length() == 0)
//            statement.executeUpdate("DELETE FROM app_roles WHERE app_title = '" 
//               + StringLib.escapeCharacters(app_title) + "'");
            DataLib.cascadeDelete(statement, "app_roles", "app_title = '" 
               + StringLib.escapeCharacters(app_title) + "'");            
         else
//            statement.executeUpdate("DELETE FROM app_roles WHERE app_title = '" 
//               + StringLib.escapeCharacters(app_title) + 
//               "' AND role_name NOT IN (" + rolesListStr + ")");
            DataLib.cascadeDelete(statement, "app_roles", "app_title = '" 
               + StringLib.escapeCharacters(app_title) + 
               "' AND role_name NOT IN (" + rolesListStr + ")");
            
         statement.close();
         
         return true;
      }
      catch (Exception exception) {
         print("Error updating application roles <" + exception.toString() + ">");
         return false;
      }
   }
   
   /**
    * Updates the database application program information for the given 
    *  application with information from the progrps.ini file
    */
   private boolean updateApplicationPrograms(INI ini, String appSection,                                            
                                             Connection connection,
                                             String app_title)
   {      
      String programsSections;
      StringTokenizer programsTokenizer;      
      String programsListStr = new String();
      String programSection;
      AppProgramInfo programInfo;
      StringTokenizer entryTokenizer;
      Statement statement;
      
      try {
         
         // Get the roles section for the given application
         programsSections = ini.GetProfileSection(appSection + "_PROG", 
                                          Constants.NTCSS_PROGRP_INIT_FILE);
         if (programsSections == null) 
            print("Could not find " + appSection + "_PROG section");            
         else {
         
            programsTokenizer = new StringTokenizer(programsSections, 
                                                   INI.STRINGS_DELIMITER);            
            while (programsTokenizer.hasMoreTokens() == true) {
               entryTokenizer = new StringTokenizer(programsTokenizer.nextToken(),
                                                    INI.TAG_VALUE_DELIMITER);
               programSection  = entryTokenizer.nextToken();
            
               print(""); // Newline
               
               // Load the program information
               programInfo = new AppProgramInfo();
               if (programInfo.load(ini, programSection) == false) {
                  print("Could not get program section <" + programSection + 
                        ">");
                  continue;
               }
               
               // Verify the program information             
               print("Verifying data for program <" + programInfo.program_title 
                     + ">...");
               if (programInfo.verifyFields() == false) {
                  print("Could not verify program section <" + programSection + ">");
                  continue;
               }
               
               // Insert or update the program information
               print("Adding program <" + programInfo.program_title + ">..");
               if (programInfo.add(connection, app_title) == false) {
                  print(" Failed to modify program <" + 
                        programInfo.program_title + "> for application <" + 
                        app_title + ">");
                  continue;
               }
                        
               // Add the application program to the list of application
               //  programs to be used later
               if (programsListStr.length() == 0)
                  programsListStr += "'" + 
                    StringLib.escapeCharacters(programInfo.program_title) + "'";
               else
                  programsListStr += ",'" + 
                    StringLib.escapeCharacters(programInfo.program_title) + "'";
            }
         }
            
         // Remove the programs that are orphaned
         statement = connection.createStatement();
         if (programsListStr.length() == 0)
//            statement.executeUpdate("DELETE FROM app_programs WHERE app_title " 
//               + "= '" + StringLib.escapeCharacters(app_title) + "'"); 
            DataLib.cascadeDelete(statement, "app_programs", "app_title " 
               + "= '" + StringLib.escapeCharacters(app_title) + "'");             
         else
//            statement.executeUpdate("DELETE FROM app_programs WHERE app_title = '" 
//                + StringLib.escapeCharacters(app_title) + 
//                "' AND program_title NOT IN (" + programsListStr + ")");
            DataLib.cascadeDelete(statement, "app_programs", "app_title = '" 
                + StringLib.escapeCharacters(app_title) + 
                "' AND program_title NOT IN (" + programsListStr + ")");            
         statement.close();
         
         return true;
      }         
      catch (Exception exception) {
         print("Error updating program data <" + exception.toString() + ">");
         return false;
      }
   }    
      
   /**
    * Updates the database application PDJ information for the given 
    *  application with information from the progrps.ini file
    */
   private boolean updateApplicationPDJs(INI ini, String appSection,                                            
                                         Connection connection,
                                         String app_title, String server_location)
   {      
      String pdjsSections;
      StringTokenizer pdjsTokenizer;      
//      String pdjsListStr = new String();
      String pdjSection;
      PDJInfo pdjInfo;
      StringTokenizer entryTokenizer;
//      Statement statement;
      
      try {
         
         // Get the roles section for the given application
         pdjsSections = ini.GetProfileSection(appSection + "_PDJ", 
                                          Constants.NTCSS_PROGRP_INIT_FILE);
         if (pdjsSections == null) 
            print("Could not find " + appSection + "_PDJ section");            
         else {
         
            pdjsTokenizer = new StringTokenizer(pdjsSections, 
                                                INI.STRINGS_DELIMITER);            
            while (pdjsTokenizer.hasMoreTokens() == true) {
               entryTokenizer = new StringTokenizer(pdjsTokenizer.nextToken(),
                                                    INI.TAG_VALUE_DELIMITER);
               pdjSection  = entryTokenizer.nextToken();
            
               print(""); // Newline
               
               // Load the predefined job information
               pdjInfo = new PDJInfo();
               if (pdjInfo.load(ini, pdjSection) == false) {
                  print("Could not parse PDJ section <" + pdjSection + 
                        ">");
                  continue;
               }
               
               // Verify the PDJ information  
               if (pdjInfo.verifyFields() == false) {
                  print("Could not verify PDJ section <" + pdjSection + ">");
                  continue;
               }
               
               // Insert or update the PDJ information
               print("Adding predefined job <" + pdjInfo.pdj_title + ">..");
               if (pdjInfo.add(connection, app_title, server_location) == false) {
                  print(" Failed to modify PDJ <" + 
                        pdjInfo.pdj_title + "> for application <" + 
                        app_title + ">");
                  continue;
               }
                           
/*  This is only needed if you want to remove PDJs that are in the DB but
    not in the progrps.ini file
               
               // Add the PDJ to the list of PDJs to be used later
               if (pdjsListStr.length() == 0)
                  pdjsListStr += "'" + 
                      StringLib.escapeCharacters(pdjInfo.pdj_title) + "'";
               else
                  pdjsListStr += ",'" + 
                      StringLib.escapeCharacters(pdjInfo.pdj_title) + "'";
 */
            }
         }
            
/*  This is only needed if you want to remove PDJs that are in the DB but
    not in the progrps.ini file
         
         // Remove the PDJs that are orphaned
         statement = connection.createStatement();
         if (pdjsListStr.length() == 0) 
//            statement.executeUpdate("DELETE FROM predefined_jobs WHERE " + 
//                 "app_title = '" + StringLib.escapeCharacters(app_title) + "'");
            DataLib.cascadeDelete(statement, "predefined_jobs", 
                 "app_title = '" + StringLib.escapeCharacters(app_title) + "'");
         else
//            statement.executeUpdate("DELETE FROM predefined_jobs WHERE app_title = '" 
//                + StringLib.escapeCharacters(app_title) + 
//                "' AND pdj_title NOT IN (" + pdjsListStr + ")");
            DataLib.cascadeDelete(statement, "predefined_jobs",  "app_title = '" 
                + StringLib.escapeCharacters(app_title) + 
                "' AND pdj_title NOT IN (" + pdjsListStr + ")");
 
         statement.close();
 */
         
         return true;
      }         
      catch (Exception exception) {
         print("Error updating PDJ data <" + exception.toString() + ">");
         return false;
      }            
   }   
   
   /**
    * Adds the base NTCSS user to the database if it doesn't exist
    */
   private boolean addBaseNtcssUser()
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      Object items[];
      String keys[];
      StringBuffer masterServerName;
      StringBuffer sharedPasswordBuffer;
      StringBuffer encryptedPassword;
      StringBuffer appPasswordBuffer;
      int isMasterServer;
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {       
            print("Error obtaining db connection");
            return false; 
         }  
         statement = connection.createStatement();                                                      
                                                      
         // See if there is a record for the base NTCSS user in the users table
         items = new Object[14];
         items[0] = Constants.BASE_NTCSS_USER;
         resultSet = statement.executeQuery(StringLib.SQLformat(
                   "SELECT * FROM users WHERE login_name = " + Constants.ST, 
                   items));
            
         // Insert the user into the database
         if (resultSet.next() == false) {  
            resultSet.close();
           
            // Get the name of the master server
            masterServerName = new StringBuffer();
            isMasterServer = DataLib.getMasterServer(masterServerName);
               
            // Create a shared password
            keys = new String[3];
            sharedPasswordBuffer = new StringBuffer();
            keys[0] = StringLib.getFullTimeStr();
            keys[1] = Constants.BASE_NTCSS_USER;
            keys[2] = "Base NTCSS User";
            StringLib.getRandomString(sharedPasswordBuffer, 
                                      Constants.MAX_APP_PASSWD_LEN, keys); 
            StringLib.removePunctuation(sharedPasswordBuffer);
            
            // Create the user's password       
            encryptedPassword = new StringBuffer();
            if (CryptLib.plainEncryptString(Constants.BASE_NTCSS_USER + 
                                         Constants.BASE_NTCSS_USER, 
                                         Constants.BASE_NTCSS_USER, 
                                         encryptedPassword, false, true) != 0) {                             
               print("Error creating base NTCSS user password");
               statement.close();
               DatabaseConnectionManager.releaseConnection(connection);
               return false;
            }
                  
            // Add the user to the "users" database table
            print("Adding base NTCSS user ...");                           
            items[0]  = Constants.BASE_NTCSS_USER;       // login name
            items[1]  = Constants.BASE_NTCSS_USER;       // long login name
            items[2]  = "Base NTCSS User";               // real name
            items[3]  = encryptedPassword.toString();    // password
            items[4]  = "0";                             // user id
            items[5]  = "000-00-0000";                   // social security number
            items[6]  = "Unknown";                       // phone number
            items[7]  = String.valueOf(Constants.UNCLASSIFIED_CLEARANCE);                
                                                         // security classification
            items[8]  = "950720.0000";                   // password change time
            items[9]  = new Boolean(false);              // login lock
            items[10] = sharedPasswordBuffer.toString();
            items[11] = masterServerName.toString();     // authentication server                                    
            items[12] = new Boolean(true);               // editable user
            items[13] = new Boolean(true);               // NTCSS super user                                                                                          
            statement.executeUpdate(StringLib.SQLformat(
                    "INSERT INTO users (login_name, long_login_name, real_name,"+
                    "password, unix_id, ss_number, phone_number, security_class,"+
                    "pw_change_time, login_lock, shared_db_passwd, " + 
                    "auth_server, editable_user, ntcss_super_user) VALUES ("                      
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + "," 
                    + Constants.ST + "," + Constants.ST + ")", items));
               
                  
            // Create an app password
            keys = new String[3];
            appPasswordBuffer = new StringBuffer();
            keys[0] = StringLib.getFullTimeStr();
            keys[1] = Constants.BASE_NTCSS_USER;
            keys[2] = Constants.NTCSS_APP;
            StringLib.getRandomString(appPasswordBuffer, 
                                      Constants.MAX_APP_PASSWD_LEN, keys);
            StringLib.removePunctuation(appPasswordBuffer);
                  
            // Add the user to the "app_users" database table
            print("Adding base NTCSS user to app_users table ...");
            items[0] = Constants.NTCSS_APP;          // app title
            items[1] = Constants.BASE_NTCSS_USER;    // login name
            items[2] = null;                         // custom app data
            items[3] = appPasswordBuffer.toString(); 
            items[4] = new Boolean(false);           // registered user
            items[5] = new Boolean(false);           // batch user
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                    "app_users (app_title, login_name, custom_app_data, " +
                    "app_passwd, registered_user, batch_user) VALUES (" + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + ")", 
                    items)); 
            
            // Add the user to the "user_app_roles" database table
            print("Adding base NTCSS user to user_app_roles table ...");
            items[0] = Constants.BASE_NTCSS_USER;        
            items[1] = Constants.NTCSS_APP;    
            items[2] = "App Administrator";              
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                    "user_app_roles (login_name, app_title, role_name) " +
                    "VALUES (" + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + ")", 
                    items));     
            
            // If this is the master server, add the user to the NDS table
            if ((isMasterServer == 1) && 
                (NDSLib.addUser(Constants.BASE_NTCSS_USER, 
                                encryptedPassword.toString(), "000-00-0000", 
                                masterServerName.toString()) == false))                       
               print("Error adding base NTCSS user to NDS table");
            
            // Add the user the operating system
            if (UserLib.addBaseNtcssUserToSystem(Constants.BASE_NTCSS_USER, 
                                                 Constants.BASE_NTCSS_USER,
                                                 "Base NTCSS User") == false) 
               print("Error adding base NTCSS user to system");
         }
            
         statement.close();    
         DatabaseConnectionManager.releaseConnection(connection);                              
         
         return true;
      }         
      catch (Exception exception) {
         print("Error trying to add base NTCSS user <" + exception.toString() 
               + ">");
         return false;
      }
   }
   
   /**
    * A container for a host's data from the progrps.ini file.
    *  NOTE: Made an inner class so that it could use PrimeDB.print method
    */
   class HostInfo   
   {
      public String hostname, address, typeStr, replicationStr;
      int type, replication;
      
      /**
       * Loads a host's information from the progrps.ini file
       */
      public boolean load(INI ini, String hostSection)
      {
         final String defVal = "NONE";
         
         try {            
            hostname = ini.GetProfileString(hostSection, "NAME", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            address = ini.GetProfileString(hostSection, "IPADDR", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            typeStr = ini.GetProfileString(hostSection, "TYPE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            replicationStr = ini.GetProfileString(hostSection, "REPL", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            
            // Check the hostname
            if (hostname.equals(defVal) == true) {
               print("Error: Invalid Hostname Address for host <" + hostname 
                     + ">");
               return false;
            }
            
            // Check the IP address
            if (address.equals(defVal) == true) {
               print("Error: Invalid IP Address for host <" + hostname + ">");
               return false;
            }
            
            // Check the type
            if (typeStr.equals(defVal) == true) {
               print("Error: Invalid host type for host <" + hostname + ">");
               return false;
            }
            
            // Check the replication value
            if (replicationStr.equals(defVal) == true) {
               print("Error: Invalid replication value for host <" + hostname 
                      + ">");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error loading host information <" + exception.toString() + 
                  ">");
            return false;
         }
      }
            
      /**
       * Makes sure the fields follow certian criteria
       */
      public boolean verifyFields()
      {
         try {                        
            
            // Check hostname
            if (checkHostName(hostname) == false) {                      
               print("ERROR: Bad hostname value <" + hostname + ">");
               return false;
            }
            
            // Check IP address            
            if (checkIpAddress(address) == false) {
               print("ERROR: Bad IP address value <" + address + ">");
               return false;
            }
            
            // Check type
            if ((type = Constants.getHostIntType(typeStr)) == -1) {
               print("ERROR: Bad host type <" + typeStr + ">");
               return false;
            }
                        
            // Check replication value
            if ((replication = convertReplicationString(replicationStr)) == -1) {
               print("ERROR: Bad replication value <" + replicationStr + ">");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error validating host fields <" + exception.toString() + ">");
            return false;
         }
      }
       
      /**
       * Updates or inserts the host record
       */
      public boolean add(Connection connection)
      {
         Statement statement;
         ResultSet resultSet;
         Object hostItems[];
         int processLimit = 30;
         
         try {
            
            statement = connection.createStatement();
            
            // Check to see if this record already exists in the database
            hostItems = new Object[5];
            hostItems[0] = hostname.toUpperCase();                        
            resultSet = statement.executeQuery(StringLib.SQLformat(
                               "SELECT * FROM hosts WHERE upper(hostname) = " + 
                               Constants.ST, hostItems));
            
            // If already exists, update the record
            if (resultSet.next() == true) {
               resultSet.close();
            
               // For some reason, in the flat file code, the max_running_jobs
               //  field was always overwritten. So the logic is just carried
               //  over
               hostItems[0] = address;
               hostItems[1] = new Integer(type);
               hostItems[2] = new Boolean((replication == 1) ? true : false);
               hostItems[3] = new Integer(processLimit);
               hostItems[4] = hostname.toUpperCase();
               statement.executeUpdate(StringLib.SQLformat(
                  "UPDATE hosts SET host_ip_address = " + Constants.ST + 
                  ", type = " + Constants.ST + ", replication_ind = " + 
                  Constants.ST + ", max_running_jobs = " + 
                  Constants.ST + " WHERE upper(hostname) = " + Constants.ST, hostItems));
            }
            else { // Insert the new record 
               resultSet.close();     
               
               hostItems[0] = hostname;
               hostItems[1] = address;
               hostItems[2] = new Integer(type);
               hostItems[3] = new Boolean((replication == 1) ? true : false);  
               hostItems[4] = new Integer(processLimit);
               statement.executeUpdate(StringLib.SQLformat(
                   "INSERT INTO hosts (hostname, host_ip_address, type, " + 
                   "replication_ind, max_running_jobs) VALUES (" + Constants.ST 
                   + ", " + Constants.ST + ", " + Constants.ST + ", " + 
                   Constants.ST + ", " + Constants.ST + ")", hostItems));
            }
            
            statement.close();
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding host information <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Checks that the given hostname follows certain criteria
       */
      private boolean checkHostName(String name)
      {      
         int index = 0;
         char curChar;

         try {
            name = name.trim();
                        
            // Check the length of the string
            if (name.length() < Constants.MIN_HOST_NAME_LEN || 
                name.length() > Constants.MAX_HOST_NAME_LEN)
               return false;

            // Check each of the characters
            for (int i = 0; i < name.length(); i++) {
         
               curChar = name.charAt(i);
               if (!Character.isLetterOrDigit(curChar) &&
                    (curChar != '_')  && 
                    (curChar != '-'))         
                  return false;         
            }

            return true;
         }
         catch (Exception exception) {
            return false;
         }
      }
   
      /**
       * Checks that the given address follows IP address format
       */
      private boolean checkIpAddress(String address)
      {
         StringTokenizer tokenizer;
         int octet;

         try {
            address = address.trim();      

            // Check the length
            if (address.length() < 7 || 
                address.length() > 15)
               return false;
         
            // Make sure there are 4 octets
            tokenizer = new StringTokenizer(address, ".");
            if ((tokenizer.countTokens() != 4) || 
                (address.charAt(address.length() - 1) == '.'))
               return false;
         
            // Make sure each octet is within the correct range
            for (int i = 0; i < 4; i++) {
               octet = StringLib.parseInt(tokenizer.nextToken(), -1);
            
               if (octet < 0 || octet > 255)
                  return false;
            }    
         
            return true;
         }
         catch (Exception exception) {
            return false;
         }   
      }         
   
      /**
       * Takes the given replication string and returns the integer value
       */
      private int convertReplicationString(String repl)
      {
         if (repl.equals("YES") == true)
            return 1;
         else if (repl.equals("NO") == true) 
            return 0;
         else
            return -1;
      }
   }      
                            
   /**
    * A container for an application's data from the progrps.ini file.
    *  NOTE: Made an inner class so that it could use PrimeDB.print method
    */
   class AppInfo
   {                  
      public String app_title, hostname, app_lockStr, icon_file, icon_indexStr, 
                    version, release_date, client_location, server_location,
                    unix_group, max_jobsStr, link_data, app_env_file, 
                    process_status_flag, batch_user, termination_signalStr;      
   
      /**
       * Loads the data from the progrps file
       */
      public boolean load(INI ini, String appSection)
      {
         final String defVal = "!";
         
         try {
            app_title = ini.GetProfileString(appSection, "GROUP", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            hostname = ini.GetProfileString(appSection, "SERVER", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            unix_group = ini.GetProfileString(appSection, "UNIX_GROUP", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            icon_file = ini.GetProfileString(appSection, "ICON_FILE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            icon_indexStr = ini.GetProfileString(appSection, "ICON_INDEX", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            version = ini.GetProfileString(appSection, "VERSION", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            release_date = ini.GetProfileString(appSection, "RELEASE_DATE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            client_location = ini.GetProfileString(appSection, "CLIENT_LOCATION", 
                                   defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            server_location = ini.GetProfileString(appSection, "SERVER_LOCATION", 
                                    defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            app_env_file = ini.GetProfileString(appSection, "APP_ENV_FILE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            process_status_flag = ini.GetProfileString(appSection, 
                                            "PROCESS_STATUS_FLAG", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            batch_user = ini.GetProfileString(appSection, "BATCH_USER", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            termination_signalStr = ini.GetProfileString(appSection, 
                                            "TERMINATION_SIGNAL", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            
            if (app_title.equals(defVal) == true) {
               print("Error getting group value");
               return false;
            }
            
            if (hostname.equals(defVal) == true) {
               print("Error getting server value");
               return false;
            }
            
            if (unix_group.equals(defVal) == true) {
               print("Error getting unix group value");
               return false;
            }
            
            if (icon_file.equals(defVal) == true) {
               print("Error getting icon file value");
               return false;
            }
            
            if (icon_indexStr.equals(defVal) == true) {
               print("Error getting icon index value");
               return false;
            }
            
            if (version.equals(defVal) == true) {
               print("Error getting version value");
               return false;
            }
            
            if (release_date.equals(defVal) == true) {
               print("Error getting release date value");
               return false;
            }
            
            if (client_location.equals(defVal) == true) {
               print("Error getting client location value");
               return false;
            }
            
            if (server_location.equals(defVal) == true) {
               print("Error getting server location value");
               return false;
            }
            
            if (app_env_file.equals(defVal) == true) {
               print("Error getting app env file value");
               return false;
            }
            
            if (process_status_flag.equals(defVal) == true) {
               print("Error getting process status flag value");
               return false;
            }
            
            if (batch_user.equals(defVal) == true) {
               print("Error getting batch user value");
               return false;
            }
            
            if (termination_signalStr.equals(defVal) == true) {
               print("Error getting termination signal value");
               return false;
            }
         
            return true;
         }
         catch (Exception exception) {
            print("Error loading application data <" + appSection + "> <" + 
                  exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Makes sure all the fields follow certain criteria
       */
      public boolean verifyFields(INI ini)
      {
         try {
            
            if (checkProgroupTitle() == false) {
               print("ERROR: \tBad progroup title value! <" + app_title + ">");
               return false;
            }
            
            if (DataLib.getHostTypeFromInitData(hostname, ini) < 0) {
               print("ERROR: \tBad host type for host <" + hostname + ">");
               return false;
            }
            
            if (checkUnixGroup() == false) {
               print("ERROR: \tBad UNIX group name <" + unix_group + "> for " +
                        "progroup " + app_title);
               return false;
            }            
            
            if (checkIconFile() == false) {
               print("ERROR: \tBad icon file value for progroup <" + app_title 
                     + ">");
               return false;
            }
            
            if (checkVersionNumber() == false) {
               print("ERROR: \tBad version number value for progroup <" + 
                     app_title + ">");
               return false;
            }
            
            if (StringLib.isValidTimeStr(release_date) == false) {
               print("ERROR: \tBad release date value for progroup");
               return false;
            }
            
            if (checkClientLocation() == false) {
               print("ERROR: \tBad client location value for progroup");
               return false;
            }
            
            if (checkServerLocation() == false) {
               print("ERROR: \tBad server location value for progroup");
               return false;
            }
            
            if (checkBatchUser() == false) {
               print("ERROR: \tBad batch user value for progroup");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error verifying application data <" + app_title + "> <" + 
                  exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * If the application doesn't exist in the database, then add it. 
       *  Otherwise just update the record.
       */
      public boolean add(Connection connection)
      {
         Statement statement;
         ResultSet resultSet;
         Object items[];
         
         try {
            
            // If this is the windows platform, use the path in SERVER_LOCATION
            //  (if there is one specified) to replace the path in the 
            //  APP_ENV_FILE and PROCESS_STATUS_FLAG fields. The whole path
            //  is not replaced - the last directory component remains. 
            //  e.g. SERVER_LOCATION = "c:\Program Files\NALCOMIS"
            //       APP_ENV_FILE = "/h/NALCOMIS/bin/env.csh"
            //    then APP_ENV_FILE will become "c:\Program Files\NALCOMIS\bin\env.csh"           
            if ((Constants.IS_WINDOWS_OS == true) && (server_location != null) &&
                (server_location.length() > 0)) {
                   
                // Modify the APP_ENV_FILE field if one is specified
                if ((app_env_file != null) && (app_env_file.length() > 0)) {  
                   
                   File fileDescp = new File(app_env_file);                   
                   String file = fileDescp.getName();
                   
                   fileDescp = fileDescp.getParentFile();
                   String nextDir = fileDescp.getName();
                   
                   // Remove any trailing file separator if there is one
                   if (server_location.endsWith("\\") || server_location.endsWith("/"))
                      server_location = server_location.substring(0, server_location.length() - 1);
                   
                   if ((nextDir != null) && (nextDir.length() > 0))
                      app_env_file = server_location + "\\" + nextDir + "\\" + file;
                   else
                      app_env_file = server_location + "\\" + file;
                }
                
                // Modify the PROCESS_STATUS_FLAG field if one is specified
                if ((process_status_flag != null) && 
                    (process_status_flag.length() > 0)) { 
                       
                   File fileDescp = new File(process_status_flag);                   
                   String file = fileDescp.getName();
                   
                   fileDescp = fileDescp.getParentFile();
                   String nextDir = fileDescp.getName();
                   
                   // Remove any trailing file separator if there is one
                   if (server_location.endsWith("\\") || server_location.endsWith("/"))
                      server_location = server_location.substring(0, server_location.length() - 1);
                   
                   if ((nextDir != null) && (nextDir.length() > 0))
                      process_status_flag = server_location + "\\" + nextDir + 
                                            "\\" + file;
                   else
                      process_status_flag = server_location + "\\" + file;
                }
            }            
            
            statement = connection.createStatement();
            
            // See if the record exists
            items = new Object[15];
            items[0] = app_title;
            resultSet = statement.executeQuery(StringLib.SQLformat(
                "SELECT * FROM apps WHERE app_title = " + Constants.ST, 
                items));
            
            if (resultSet.next() == true) {  // Update the record
               resultSet.close();
               
               items[0]  = hostname;
               items[1]  = new Boolean(false);  // Clear the app lock
               items[2]  = icon_file;
               items[3]  = new Integer(icon_indexStr);
               items[4]  = version;
               items[5]  = release_date;
               items[6]  = client_location;
               items[7]  = server_location;
               items[8]  = unix_group;
               items[9]  = app_env_file;
               items[10]  = process_status_flag;
               items[12] = new Integer(termination_signalStr);
               items[12] = app_title;
               statement.executeUpdate(StringLib.SQLformat(
                 "UPDATE apps SET hostname = " + Constants.ST + ", app_lock = " + 
                 Constants.ST + ", icon_file = " + 
                 Constants.ST + ", icon_index = " + Constants.ST + ", version_number = " + 
                 Constants.ST + ", release_date = " + Constants.ST + 
                 ", client_location = " + Constants.ST + ", server_location = " + 
                 Constants.ST + ", unix_group = " + Constants.ST + 
                 ", env_filename = " + Constants.ST + ", psp_filename = " + 
                 Constants.ST + ", termination_signal = " + Constants.ST + 
                 " WHERE app_title = " + Constants.ST, items));
            }
            else {  // Insert the record
               resultSet.close();
               
               items[0]  = app_title;
               items[1]  = hostname;
               items[2]  = new Boolean(false);
               items[3]  = icon_file;
               items[4]  = new Integer(icon_indexStr);
               items[5]  = version;
               items[6]  = release_date;
               items[7]  = client_location;
               items[8]  = server_location;
               items[9]  = unix_group;
               items[10] = new Integer(10);
               items[11] = null;
               items[12] = app_env_file;
               items[13] = process_status_flag;
               items[14] = new Integer(termination_signalStr);               
               statement.executeUpdate(StringLib.SQLformat("INSERT INTO apps (" +
                  "app_title, hostname, app_lock, icon_file, icon_index, " +
                  "version_number, release_date, client_location, " +
                  "server_location, unix_group, max_running_app_jobs, " +
                  "link_data, env_filename, psp_filename, termination_signal) " +
                  "VALUES (" + Constants.ST + ", " + Constants.ST + ", " +
                  Constants.ST + ", " + Constants.ST + "," + Constants.ST + 
                  " , " + Constants.ST + ", " + Constants.ST + ", " + 
                  Constants.ST + ", " + Constants.ST + ", " + Constants.ST + 
                  ", " + Constants.ST + ", " + Constants.ST + ", " + 
                  Constants.ST + ", " + Constants.ST + ", " + Constants.ST + 
                  ")", items));
            }
            
            statement.close();
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding application record <" + exception.toString() + 
                  ">");
            return false;
         }
      }          
      
      /**
       * Adds the batch user for this application
       */
      public boolean addBatchUser(Connection connection, INI ini, 
                                  Vector newBatchUsersList) 
      {
         String localHostname;
         Statement statement;
         ResultSet resultSet;
         Object items[];
         StringBuffer sharedPasswordBuffer = new StringBuffer();
         StringBuffer appPasswordBuffer = new StringBuffer();
         String keys[];
         int hostType;
         final String userPassword = "ecafretni";
         StringBuffer encryptedUserPassword;
         boolean isAppHostOrMaster = false;
         
         try {            
            
            // If this is the host of the app, add the batch user to the database
            localHostname = StringLib.getHostname();
            hostType = DataLib.getHostTypeFromInitData(localHostname, ini);
            if ((hostname.equalsIgnoreCase(localHostname) == true) ||
                (hostType == Constants.NTCSS_MASTER)) {
                   
               isAppHostOrMaster = true;
                                                      
               statement = connection.createStatement();                                                      
                                                      
               // See if there is a record for this batch user in the users table
               items = new Object[14];
               items[0] = batch_user;
               resultSet = statement.executeQuery(StringLib.SQLformat(
                   "SELECT * FROM users WHERE login_name = " + Constants.ST, 
                   items));
            
               // Insert the record
               if (resultSet.next() == false) {  
                  resultSet.close();
               
                  // Create a shared password
                  keys = new String[3];
                  keys[0] = StringLib.getFullTimeStr();
                  keys[1] = batch_user;
                  keys[2] = "Batch User";
                  StringLib.getRandomString(sharedPasswordBuffer, 
                                            Constants.MAX_APP_PASSWD_LEN, keys);   
                  StringLib.removePunctuation(sharedPasswordBuffer);
                  
                  // Encrypt the user password             
                  encryptedUserPassword = new StringBuffer();
                  if (CryptLib.plainEncryptString(batch_user + batch_user, 
                                         userPassword, 
                                         encryptedUserPassword, false, true) != 0) {                             
                     print("Error creating batch user password");
                     statement.close();                    
                     return false;
                  }
                  
                  print("Adding NTCSS user <" + batch_user + ">..");                           
                  items[0]  = batch_user;         // login name
                  items[1]  = batch_user;         // long login name
                  items[2]  = "Batch User";       // real name
                  items[3]  = encryptedUserPassword.toString(); // password
                  items[4]  = "0";                // user id
                  items[5]  = "000-00-0000";      // social security number
                  items[6]  = "N/A";              // phone number
                  items[7]  = String.valueOf(Constants.UNCLASSIFIED_CLEARANCE);   
                                                  // security classification
                  items[8]  = "N/A";              // password change time
                  items[9]  = new Boolean(false); // login lock
                  items[10] = sharedPasswordBuffer.toString();
                  items[11] = hostname;           // authentication server                                    
                  items[12] = new Boolean(false); // editable user
                  items[13] = new Boolean(false); // NTCSS super user                                                                                          
                  statement.executeUpdate(StringLib.SQLformat(
                    "INSERT INTO users (login_name, long_login_name, real_name,"+
                    "password, unix_id, ss_number, phone_number, security_class,"+
                    "pw_change_time, login_lock, shared_db_passwd, " + 
                    "auth_server, editable_user, ntcss_super_user) VALUES ("                      
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + ","
                    + Constants.ST + "," + Constants.ST + "," 
                    + Constants.ST + "," + Constants.ST + ")", items));
                  
                  // If this is the master server, add the user to the NDS tables
                  if (hostType == Constants.NTCSS_MASTER) { 
                     if (NDSLib.addUser(batch_user, encryptedUserPassword.toString(), 
                                      "000-00-0000", hostname) == false)
                        print("Error adding batch user <" + batch_user + 
                              "> to user NDS table");   
                     
                     if (NDSLib.addUserToGroup(batch_user, unix_group) == false)
                        print("Error adding batch user <" + batch_user + 
                              "> to group NDS table");   

                  }
                  
                  // Add the batch user to the operating system
                  if (UserLib.addBatchUserToSystem(batch_user, userPassword, 
                                                   "Batch User", 
                                                   unix_group) == false) 
                     print("Error adding batch user <" + batch_user + 
                           "> to system");      
                  
                  // Add this new batch user to a running list of new batch
                  //  users. This list will be used in subscribing the batch
                  //  users to the proper bulletin boards. Can't subscribe
                  //  the user here, since there is no guarantee that the
                  //  bulletin board exists at this point. The users are subscribed
                  //  in updateBulletinBoards.
                  newBatchUsersList.add(batch_user + "/" + app_title);
               }
               
               // See if there is a record for this batch user in the 
               //  app_users table               
               items[0] = batch_user;
               items[1] = app_title;
               resultSet = statement.executeQuery(StringLib.SQLformat(
                   "SELECT * FROM app_users WHERE login_name = " + Constants.ST + 
                   " AND app_title = " + Constants.ST , items));
               
               // Insert the new record
               if (resultSet.next() == false) {
                  resultSet.close();
                  
                  // Create an app password
                  keys = new String[3];
                  keys[0] = StringLib.getFullTimeStr();
                  keys[1] = batch_user;
                  keys[2] = app_title;
                  StringLib.getRandomString(appPasswordBuffer, 
                                            Constants.MAX_APP_PASSWD_LEN, keys);
                  StringLib.removePunctuation(appPasswordBuffer);
                  
                  print("Adding app user <" + batch_user + ">..");
                  items[0] = app_title;                    // app title
                  items[1] = batch_user;                   // login name
                  items[2] = null;                         // custom app data
                  items[3] = appPasswordBuffer.toString();
                  items[4] = new Boolean(false);           // regisitered user
                  items[5] = new Boolean(true);            // batch user
                  statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                    "app_users (app_title, login_name, custom_app_data, " +
                    "app_passwd, registered_user, batch_user) VALUES (" + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + ")", 
                    items));                                    
               }
            
               statement.close();                                 
            }     
            
            // If this is being run on windows, even though this host may not
            //  be the application's host or the NTCSS master, an attempt must
            //  still be made to add the batch user to the system. (NOTE: The
            //  native call won't "fail" if the user already exists in the 
            //  system.) This is because, if this host is the PDC (the only
            //  windows host type that will actually add the users to the 
            //  system), but it is not the application host, it still needs
            //  to add the user to the system so that the real application
            //  host (if it is windows) will be able to see the system user
            //  when it needs to run a batch job as the user (since the real
            //  application host won't have the ability to add the batch user
            //  to the system itself since it is'nt the NT PDC).
            //
            //  NOTE: only those batch users that are associated with applications
            //  which will run on windows platform need to be added. Since
            //  this information can't be determined, ALL batch users are added
            //  to the windows user domain.
            if ((Constants.IS_WINDOWS_OS == true) &&
                (isAppHostOrMaster == false)) {
                                 
               // Add the batch user to the operating system
               if (UserLib.addBatchUserToSystem(batch_user, userPassword, 
                                                "Batch User", 
                                                unix_group) == false) 
                  print("Error adding batch user <" + batch_user + 
                        "> to system on host that is not the app host or master");
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding batch user <" + exception.toString() + 
                  ">");
            return false;
         }
      }
      
      /**
       * Returns true if the given application title follows certain criteria
       */
      private boolean checkProgroupTitle()
      {
         char curChar;
         
         app_title = app_title.trim();
         
         // Check the length
         if ((app_title.length() < Constants.MIN_PROGROUP_TITLE_LEN) ||
             (app_title.length() > Constants.MAX_PROGROUP_TITLE_LEN)) {
            print("Progroup title length (" + app_title.length() + 
                  ") is out of range!");
            return false;
         }
         
         // Make sure first character is a letter
         if (Character.isLetterOrDigit(app_title.charAt(0)) == false) {
            print("Progroup title <" + app_title + 
            "> doesn't start with a letter!");
            return false;
         }
         
         for (int i = 0; i < app_title.length(); i++) {
            curChar = app_title.charAt(i);
            
            // Make sure characters are alphnumeric or '_' or '-'
            if ((Character.isLetterOrDigit(curChar) == false) &&
                (curChar != '_')  && 
                (curChar != '-'))         
               return false;
         }
         
         return true;
      }
      
      /**
       * Returns true if the given unix_group follows certain criteria
       */
      private boolean checkUnixGroup()
      {
         unix_group = unix_group.trim();
         
         // Check length
         if (unix_group.length() > Constants.MAX_UNIX_GROUP_LEN) {
            print("Group name <" + unix_group + "> length (" + 
                  unix_group.length() + ") is too long!");
            return false;
         }                           
         
         return true;
      }
      
      /**
       * Returns true if the given icon file follows certain criteria
       */
      private boolean checkIconFile()
      {
         icon_file = icon_file.trim();
         
         if (icon_file.length() > Constants.MAX_ICON_FILE_LEN) {
            print("File name <" + icon_file + "> length (" + icon_file.length() 
                  + ") is too long!");
            return false;
         }                  
         
         return true;
      }
      
      /**
       * Returns true if the given icon index follows certain criteria
       */
      private boolean checkIconIndex()
      {
         icon_indexStr = icon_indexStr.trim();
         
         if (icon_indexStr.length() < 1) {
            print("Icon index length is " + icon_indexStr.length());
            return false;
         }
         
         for (int i = 0; i < icon_indexStr.length(); i++)  {
            if (Character.isDigit(icon_indexStr.charAt(i)) == false) {
               print("Icon string <" + icon_indexStr + "> contains non-digits");
               return false;
            }
         }
         
         return true;
      }
      
      /**
       * Returns true if the given version follows certain criteria
       */
      private boolean checkVersionNumber()
      {
         version = version.trim();
         
         if (version.length() > Constants.MAX_VERSION_NUMBER_LEN) {
            print("Version number <" + version + "> length (" + 
                  version.length() + ") is too long");
            return false;
         }  
         
         return true;
      }
      
      /**
       * Returns true if the given client location follows certain criteria
       */
      private boolean checkClientLocation()
      {
         client_location = client_location.trim();
         
         if (client_location.length() > Constants.MAX_CLIENT_LOCATION_LEN) {
            print("Location name <" + client_location + "> length (" + 
                  client_location.length() + ") is too long");
            return true;
         }                  
         
         return true;
      }
      
      /**
       * Returns true if the given server location follows certain criteria
       */
      private boolean checkServerLocation()
      {
         server_location = server_location.trim();
         
         if (server_location.length() > Constants.MAX_SERVER_LOCATION_LEN) {
            print("Server location name <" + server_location + "> length (" + 
                  server_location.length() + ") is too long");
            return true;
         }
         
         return true;
      }     
      
      /**
       * Returns true if this app has a batch user and it is a valid name
       */
      private boolean checkBatchUser()
      {
         char curChar;
         
         if (StringLib.isBlank(batch_user) == false) {
            
            batch_user = batch_user.trim();
         
            // Check the length
            if ((batch_user.length() < Constants.MIN_LOGIN_NAME_LEN) ||
                (batch_user.length() > Constants.MAX_LOGIN_NAME_LEN)) 
               return false;            
         
            // Make sure first character is not a digit
            if (Character.isDigit(batch_user.charAt(0)) == true) 
               return false;            
         
            for (int i = 0; i < batch_user.length(); i++) {
               curChar = batch_user.charAt(i);
            
               // Make sure characters are alphnumeric
               if (Character.isLetterOrDigit(curChar) == false)
                  return false;
               
               // Make sure the characters are lower case
               if (Character.isUpperCase(curChar) == true)
                   return false;
            }                                 
         }
         
         return true;
      }
   }
   
   /**
    * A container for an application's role info from the progrps.ini file.
    *  NOTE: Made an inner class so that it could use PrimeDB.print method
    */
   class AppRoleInfo
   {
      public String roleName, lockOverrideStr;
      
      /**
       * Loads the role's information from the progrps.ini file
       */
      public boolean load(INI ini, String appRoleSection)
      {
         final String defVal = "!";
         
         try {
            roleName = ini.GetProfileString(appRoleSection, "TITLE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            lockOverrideStr = ini.GetProfileString(appRoleSection, 
                                   "LOCK_OVERRIDE", defVal, 
                                   Constants.NTCSS_PROGRP_INIT_FILE);
            
            if (roleName.equals(defVal) == true) {
               print("Error reading role name value");
               return false;
            }
            
            if (lockOverrideStr.equals(defVal) == true) {
               print("Error reading lock override value");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error loading applicaton role  <" + appRoleSection + ">");
            return false;
         }
      }
      
      /**
       * Makes sure the fields follow certain criteria
       */
      public boolean verifyFields()
      {
         try {
            if (roleName.equals("!") == true) 
               return false;
               
            if (lockOverrideStr.equals("!") == true)
               lockOverrideStr = "0";
            
            return true;
         }
         catch (Exception exception) {
            print("Error verifying applicaton role  <" + roleName + ">");
            return false;
         }
      }
      
      /**
       * Adds or updates the role's information to the database
       */
      public boolean add(Connection connection, String app_title, int roleNumber)
      {
         Statement statement;
         Object items[];
         ResultSet resultSet;
         
         try {
            statement = connection.createStatement();
            
            // See if the record exists
            items = new Object[3];
            items[0] = roleName;
            items[1] = app_title;
            resultSet = statement.executeQuery(StringLib.SQLformat(
                "SELECT * FROM app_roles WHERE role_name = " + Constants.ST + 
                " and app_title = " + Constants.ST, items));
            
            if (resultSet.next() == true) {  // Update the record
               resultSet.close();
               
               items[0]  = new Boolean(lockOverrideStr.equals("1") ? true : false);
               items[1]  = roleName;
               items[2]  = app_title;
               statement.executeUpdate(StringLib.SQLformat(
                 "UPDATE app_roles SET app_lock_override = " + Constants.ST + 
                 ", role_number = " + roleNumber + " WHERE role_name = " + 
                 Constants.ST + " AND app_title = " + Constants.ST, items));
            }
            else {  // Insert the record
               resultSet.close();
               
               items[0]  = roleName;
               items[1]  = app_title;               
               items[2]  = new Boolean(lockOverrideStr.equals("1") ? true : false);
               statement.executeUpdate(StringLib.SQLformat(
                  "INSERT INTO app_roles (role_name, app_title, " + 
                  "app_lock_override, role_number) VALUES (" + Constants.ST + 
                  ", " + Constants.ST + "," + Constants.ST + 
                  " , " + roleNumber + ")", items));
            }                        
            
            statement.close();
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding applicaton role  <" + roleName + ">");                        
            return false;
         }
      }
   }
   
   /**
    * A container for an application's program data from the progrps.ini file.
    *  NOTE: Made an inner class so that it could use PrimeDB.print method
    */
   class AppProgramInfo
   {                  
      public String program_title, program_file, icon_file, icon_indexStr, 
             cmd_line_args, security_classStr, program_typeStr, working_dir,
             access;
      public int security_class, program_type;
   
      /**
       * Loads the data from the progrps file
       */
      public boolean load(INI ini, String programSection)
      {
         final String defVal = "!";
         
         try {
            program_title = ini.GetProfileString(programSection, "PROGRAM", 
                               defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            program_file = ini.GetProfileString(programSection, "EXEFILE",defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            icon_file = ini.GetProfileString(programSection, "ICONFILE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            icon_indexStr = ini.GetProfileString(programSection, "ICONINDEX", 
                                   defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            cmd_line_args = ini.GetProfileString(programSection, "COMMANDLINE", 
                                      defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            security_classStr = ini.GetProfileString(programSection, 
                                            "SECURITYCLASS", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            access = ini.GetProfileString(programSection, "ACCESS", 
                                      defVal, Constants.NTCSS_PROGRP_INIT_FILE);            
            program_typeStr = ini.GetProfileString(programSection, "PROGRAMTYPE", 
                                     defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            
            working_dir = ini.GetProfileString(programSection, "WORKINGDIR", 
                                    defVal, Constants.NTCSS_PROGRP_INIT_FILE);
            
            if (program_title.equals(defVal) == true) {
               print("Error reading program title value");
               return false;
            }
            
            if (program_file.equals(defVal) == true) {
               print("Error reading program file value");
               return false;
            }
            
            if (icon_file.equals(defVal) == true) {
               print("Error reading icon file value");
               return false;
            }
            
            if (icon_indexStr.equals(defVal) == true) {
               print("Error reading icon index value");
               return false;
            }
            
            if (cmd_line_args.equals(defVal) == true) {
               print("Error reading command line arguments value");
               return false;
            }
            
            if (security_classStr.equals(defVal) == true) {
               print("Error reading security class value");
               return false;
            }
            
            if (access.equals(defVal) == true) {
               print("Error reading access value");
               return false;
            }
            
            if (program_typeStr.equals(defVal) == true) {
               print("Error reading program type value");
               return false;
            }
            
            if (working_dir.equals(defVal) == true) {
               print("Error reading working dir value");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error loading application program <" + programSection + 
                  "> <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Makes sure all the fields follow certain criteria
       */
      public boolean verifyFields()
      {
         try {        
            
            if (checkProgramTitle() == false) {
               print("ERROR! \tBad program title key value <" + program_title 
                     + ">");
               return false;
            }
            
            if (checkProgramFile() == false) {
               print("ERROR! \tBad program file key value for program <" + 
                     program_title + ">");
               return false;
            }
            
            if (checkIconFile() == false) {
               print("ERROR! \tBad icon file key value for program <" + 
                     program_title + ">");
               return false;
            }
            
            if (checkIconIndex() == false) {
               print("ERROR! \tBad icon index key value for program <" + 
                     program_title + ">");
               return false;
            }
            
            if (checkCmdLineArgs() == false) {
               print("ERROR! \tBad command line arguments key value for program <" 
                     + program_title + ">");
               return false;
            }
            
            if ((security_class = 
                         Constants.getSecurityCode(security_classStr)) == -1) {
               print("ERROR! \tBad classification key value for program <" 
                     + program_title + ">");
               return false;
            }
            
            if ((program_type = Constants.getProgramType(program_typeStr)) == -1) {
               print("ERROR! \tBad program type key value for program <" 
                     + program_title + ">");
               return false;
            }
            
            if (checkWorkingDir() == false) {
               print("ERROR! \tBad working directory key value for program <" 
                     + program_title + ">");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error verifying application program <" + program_title + 
                  "> <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * If the program doesn't exist in the database, then add it. 
       *  Otherwise just update the record.
       */
      public boolean add(Connection connection, String app_title)
      {
         Statement statement;
         ResultSet resultSet;
         Object items[];
         
         try {
            statement = connection.createStatement();
                        
            // See if the record exists
            items = new Object[7];
            items[0] = program_title;
            items[1] = app_title;
            resultSet = statement.executeQuery(StringLib.SQLformat(
                "SELECT * FROM app_programs WHERE program_title = " + 
                Constants.ST + " AND app_title = " + Constants.ST, items));
            
            if (resultSet.next() == true) {  // Update the record
               resultSet.close();
               
               items[0] = program_file;
               items[1] = icon_file;
               items[2] = new Integer(StringLib.parseInt(icon_indexStr, -1));
               items[3] = cmd_line_args;
               items[4] = working_dir;
               items[5] = program_title;
               items[6] = app_title;
               statement.executeUpdate(StringLib.SQLformat(
                 "UPDATE app_programs SET program_file = " + Constants.ST + 
                 ", icon_file = " + Constants.ST + ", icon_index = " + 
                 Constants.ST + ", cmd_line_args = " + 
                 Constants.ST + ", security_class = " + security_class + 
                 ", program_type = " + program_type + ", working_dir = " + 
                 Constants.ST + " WHERE program_title = " + Constants.ST + 
                 " AND app_title = " + Constants.ST, items));
            }
            else {  // Insert the record
               resultSet.close();
               
               items[0] = program_title;
               items[1] = app_title;               
               items[2] = program_file;
               items[3] = icon_file;
               items[4] = new Integer(StringLib.parseInt(icon_indexStr, -1));
               items[5] = cmd_line_args;
               items[6] = working_dir;
               statement.executeUpdate(StringLib.SQLformat(
                  "INSERT INTO app_programs (program_title, app_title, " + 
                  "program_file, icon_file, icon_index, cmd_line_args, " + 
                  "security_class, program_type, working_dir) VALUES (" + 
                  Constants.ST + ", " + Constants.ST + ", " + 
                  Constants.ST + ", " + Constants.ST + ", " + Constants.ST +
                  ", " + Constants.ST + ", " + security_class + ", " + 
                  program_type + ", " + Constants.ST + ")", items));
            }
            
            statement.close();
            
            // Update this program's roles
            if (addProgramRoles(connection, app_title) == false)
               print("Error add application program roles");                        
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding program record <" + exception.toString() + 
                  ">");
            return false;
         }
      }  
      
      /**
       * Adds this program's roles to the database
       */
      private boolean addProgramRoles(Connection connection, String app_title)
      {
         StringTokenizer tokenizer;
         Statement statement;          
         ResultSet resultSet;
         Object items[];
         String role;
         String roleStr = new String();
         
         try {
            statement = connection.createStatement();
                                    
            items = new Object[3];
            tokenizer = new StringTokenizer(access, ",");
            while (tokenizer.hasMoreTokens() == true) {
               role = tokenizer.nextToken().trim();                              
               
               // Check the length
               if (role.length() == 0)
                  continue;
               
               // See if this role is already in the database
               items[0] = role;
               items[1] = program_title;
               items[2] = app_title;
               resultSet = statement.executeQuery(StringLib.SQLformat(
                  "SELECT * FROM app_program_roles WHERE role_name = " + 
                  Constants.ST + " AND program_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST, items));
               
               // Insert the record if it doesn't exist
               if (resultSet.next() == false) {
                  resultSet.close();
                                                      
                  // This is in its own try catch so that if there is a foreign                  
                  //  key constraint exception, only this program role won't be
                  //  inserted
                  try {
                     statement.executeUpdate(StringLib.SQLformat(
                       "INSERT INTO app_program_roles (role_name, program_title," +
                       "app_title) VALUES(" + Constants.ST + ", " + 
                       Constants.ST + ", " + Constants.ST + ")", items));
                  }
                  catch (Exception exp) {
                     print("Error adding predefined job role \"" + role + "\" <" + 
                           exp.toString() + ">");
                  }
               }
               else
                  resultSet.close();         
               
               if (roleStr.length() == 0)
                  roleStr += "'" + 
                     StringLib.escapeCharacters(role) + "'";
               else
                  roleStr += ",'" + 
                     StringLib.escapeCharacters(role) + "'";
            }
            
            // Remove orphaned application program roles
            items[0] = program_title;
            items[1] = app_title;               
            if (roleStr.length() == 0) 
//               statement.executeUpdate(StringLib.SQLformat("DELETE FROM " + 
//                  "app_program_roles WHERE program_title = " + Constants.ST + 
//                  " AND app_title = " + Constants.ST, items));                                 
               DataLib.cascadeDelete(statement, "app_program_roles", StringLib.SQLformat(
                  "program_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST, items));                                                
            else 
//               statement.executeUpdate(StringLib.SQLformat("DELETE FROM " + 
//                  "app_program_roles WHERE program_title = " + Constants.ST + 
//                  " AND app_title = " + Constants.ST + 
//                  " AND role_name NOT IN (" + roleStr + ")", items));                        
               DataLib.cascadeDelete(statement, "app_program_roles", StringLib.SQLformat(
                  "program_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST + 
                  " AND role_name NOT IN (" + roleStr + ")", items));                                       
            statement.close();
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding application program roles <" + 
                  exception.toString() + ">");
            return false;
         }
      }
      
      private boolean checkProgramTitle()
      {
         program_title = program_title.trim();
         
         if (program_title.length() < Constants.MIN_PROGRAM_TITLE_LEN ||
            program_title.length() > Constants.MAX_PROGRAM_TITLE_LEN) {
            print("Program title <" + program_title + "> length (" + 
                  program_title.length() + ") is out of range!");
            return false;
         }
         
         return true;
      }
      
      private boolean checkProgramFile()
      {
         program_file = program_file.trim();
         
         if (program_file.length() < Constants.MIN_PROGRAM_FILE_LEN ||
             program_file.length() > Constants.MAX_PROGRAM_FILE_LEN) {
            print("Program file <" + program_file + "> length (" + 
                  program_file.length() + ") is out of range!");    
            return false;
         }
         
         return true;
      }
      
      private boolean checkIconFile()
      {
         icon_file = icon_file.trim();
         
         if (icon_file.length() > Constants.MAX_ICON_FILE_LEN) {
            print("File name <" + icon_file + "> length (" + icon_file.length() 
                  + ") is too long!");
            return false;
         }
         
         return true;
      }
            
      private boolean checkIconIndex()
      {
         icon_indexStr = icon_indexStr.trim();
         
         if (icon_indexStr.length() < 1) {
            print("Icon index length is " + icon_indexStr.length());
            return false;
         }
         
         for (int i = 0; i < icon_indexStr.length(); i++)  {
            if (Character.isDigit(icon_indexStr.charAt(i)) == false) {
               print("Icon string <" + icon_indexStr + "> contains non-digits");
               return false;
            }
         }
         
         return true;
      }
      
      private boolean checkCmdLineArgs()
      {
         cmd_line_args = cmd_line_args.trim();
         
         if (cmd_line_args.length() > Constants.MAX_CMD_LINE_ARGS_LEN) {
            print("Argument <" + cmd_line_args + "> length (" + 
                  cmd_line_args.length() + ") is too long!");
            return false;
         }
         
         return true;
      }            
      
      private boolean checkWorkingDir()
      {
         working_dir = working_dir.trim();
         
         if (working_dir.length() < Constants.MIN_WORKING_DIR_LEN ||
             working_dir.length() > Constants.MAX_WORKING_DIR_LEN) {
            print("Working directory <" + working_dir + "> length (" + 
                  working_dir.length() + ") is out of range!");    
            return false;
         }
         
         return true;
      }
   }   
   
   /**
    * A container for an application's PDJ data from the progrps.ini file.
    *  NOTE: Made an inner class so that it could use PrimeDB.print method
    */
   class PDJInfo
   {
      public String pdj_title, command_line, job_typeStr, schedule_str, access;
      public int job_type;
      
      /**
       * Loads the given PDJ information from the progrps.ini file
       */
      public boolean load(INI ini, String pdjSection)
      {
         final String defVal = "!";
         
         try {
            pdj_title = ini.GetProfileString(pdjSection, "TITLE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            command_line = ini.GetProfileString(pdjSection, "COMMAND_LINE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            access = ini.GetProfileString(pdjSection, "ACCESS", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            job_typeStr = ini.GetProfileString(pdjSection, "JOBTYPE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            schedule_str = ini.GetProfileString(pdjSection, "SCHEDULE", defVal, 
                                            Constants.NTCSS_PROGRP_INIT_FILE);
            
            if (pdj_title.equals(defVal) == true) {
               print("Error reading pdj title value");
               return false;
            }
            
            if (command_line.equals(defVal) == true) {
               print("Error reading command line value");
               return false;
            }
            
            if (access.equals(defVal) == true) {
               print("Error reading access value");
               return false;
            }
            
            if (job_typeStr.equals(defVal) == true) {
               print("Error reading job type value");
               return false;
            }
            
            if (schedule_str.equals(defVal) == true) {
               print("Error reading schedule string value");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error loading PDJ data <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Verifies that the fields follow certain criteria
       */
      public boolean verifyFields()
      {
         try {
            if (checkPredefinedJobTitle() == false) {
               print("ERROR: \tBad predefined job title value for PDJ <" + 
                     pdj_title + ">");
               return false;
            }
            
            if (checkCommandLine() == false) {
               print("ERROR: \tBad predefined job command line value for PDJ " +
                     "<" + pdj_title + ">");
               return false;
            }
            
            if ((job_type = Constants.getProgramType(job_typeStr)) == -1) {
               print("ERROR: \tBad predefined job class value for PDJ <" + 
                     pdj_title + ">");               
               return false;
            }
            
            if (checkScheduleString() == false) {
               print("ERROR: \tBad predefined job schedule string value" +
                     " for PDJ <" + pdj_title + ">");
               return false;
            }
            
            return true;
         }
         catch (Exception exception) {
            print("Error verifying PDJ fields <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Adds the PDJ info to the database
       */
      public boolean add(Connection connection, String app_title,
                         String server_location)
      {
         ResultSet resultSet;
         Object items[];
         Statement statement;
         
         try {
            
            // If this is the windows platform, use the path in APP.SERVER_LOCATION
            //  (if there is one specified) to replace the path in the 
            //  COMMAND_LINE field. The whole path is not replaced -
            //  the last directory component remains. 
            //  e.g. SERVER_LOCATION = "c:\Program Files\NALCOMIS"
            //       COMMAND_LINE = "/h/NALCOMIS/bin/sample.csh"
            //    then COMMAND_LINE will become "c:\Program Files\NALCOMIS\bin\sample.csh"           
            if ((Constants.IS_WINDOWS_OS == true) && (server_location != null) &&
                (server_location.length() > 0)) {
                   
                // Modify the COMMAND_LINE field if one is specified
                if ((command_line != null) && (command_line.length() > 0)) {  
                   
                   File fileDescp = new File(command_line);                   
                   String file = fileDescp.getName();
                   
                   fileDescp = fileDescp.getParentFile();
                   String nextDir = fileDescp.getName();
                   
                   // Remove any trailing file separator if there is one
                   if (server_location.endsWith("\\") || server_location.endsWith("/"))
                      server_location = server_location.substring(0, server_location.length() - 1);
                   
                   if ((nextDir != null) && (nextDir.length() > 0))
                      command_line = server_location + "\\" + nextDir + "\\" + file;
                   else
                      command_line = server_location + "\\" + file;
                }                                
            }    
            
            statement = connection.createStatement();
            
            // Check to see if this record already exists in the database
            items = new Object[5];
            items[0] = app_title;                        
            items[1] = pdj_title;                        
            resultSet = statement.executeQuery(StringLib.SQLformat(
                               "SELECT * FROM predefined_jobs WHERE app_title = " + 
                               Constants.ST + " AND pdj_title = " + 
                               Constants.ST, items));
            
            // If already exists, update the record
            if (resultSet.next() == true) {
               resultSet.close();
            
               // NOTE: the job_type and schedule_str fields are not updated
               items[0] = command_line;
               items[1] = pdj_title;
               items[2] = app_title;
               statement.executeUpdate(StringLib.SQLformat(
                  "UPDATE predefined_jobs SET command_line = " + Constants.ST + 
                  " WHERE pdj_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST, items));
            }
            else { // Insert the new record 
               resultSet.close();     
               
               items[0] = pdj_title;
               items[1] = app_title;
               items[2] = command_line;
               items[3] = new Integer(job_type);
               items[4] = schedule_str;               
               statement.executeUpdate(StringLib.SQLformat(
                   "INSERT INTO predefined_jobs (pdj_title, app_title, " +
                   "command_line, job_type, schedule_str) VALUES (" + 
                   Constants.ST + ", " + Constants.ST + ", " + Constants.ST 
                   + ", " + Constants.ST + ", " + Constants.ST + ")", items));
            }
            
            statement.close();
            
            // Update this PDJ's roles
            if (addPDJRoles(connection, app_title) == false)
               print("Error adding predefined job roles"); 
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding predefined job <" + exception.toString() + ">");
            return false;
         }
      }
      
      /**
       * Adds this predefined job's roles to the database
       */
      private boolean addPDJRoles(Connection connection, String app_title)
      {
         StringTokenizer tokenizer;
         Statement statement;          
         ResultSet resultSet;
         Object items[];
         String role;
         String roleStr = new String();
         
         try {
            statement = connection.createStatement();
                                    
            items = new Object[3];
            tokenizer = new StringTokenizer(access, ",");
            while (tokenizer.hasMoreTokens() == true) {
               role = tokenizer.nextToken().trim();
               
               // Check the length
               if (role.length() == 0)
                  continue;
               
               // See if this role is already in the database
               items[0] = role;
               items[1] = pdj_title;
               items[2] = app_title;
               resultSet = statement.executeQuery(StringLib.SQLformat(
                  "SELECT * FROM pdj_app_roles WHERE role_name = " + 
                  Constants.ST + " AND pdj_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST, items));
               
               // Insert the record if it doesn't exist
               if (resultSet.next() == false) {
                  resultSet.close();
                  
                  // This is in its own try catch so that if there is a foreign                  
                  //  key constraint exception, only this pdj role won't be
                  //  inserted
                  try {
                     statement.executeUpdate(StringLib.SQLformat(
                       "INSERT INTO pdj_app_roles (role_name, pdj_title," +
                       "app_title) VALUES(" + Constants.ST + ", " + 
                       Constants.ST + ", " + Constants.ST + ")", items));
                  }
                  catch (Exception exp) {
                     print("Error adding predefined job role \"" + role + "\" <" + 
                           exp.toString() + ">");
                  }
               }
               else
                  resultSet.close();         
               
               if (roleStr.length() == 0)
                  roleStr += "'" + 
                     StringLib.escapeCharacters(role) + "'";
               else
                  roleStr += ",'" + 
                     StringLib.escapeCharacters(role) + "'";
            }
            
            // Remove orphaned predefined job roles
            items[0] = pdj_title;
            items[1] = app_title;
            if (roleStr.length() == 0)
//               statement.executeUpdate(StringLib.SQLformat("DELETE FROM " + 
//                  "pdj_app_roles WHERE pdj_title = " + Constants.ST + 
//                  " AND app_title = " + Constants.ST, items));            
               DataLib.cascadeDelete(statement, "pdj_app_roles", StringLib.SQLformat(
                  "pdj_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST, items));                        
            else
//               statement.executeUpdate(StringLib.SQLformat("DELETE FROM " + 
//                  "pdj_app_roles WHERE pdj_title = " + Constants.ST + 
//                  " AND app_title = " + Constants.ST + 
//                  " AND role_name NOT IN (" + roleStr + ")", items));            
               DataLib.cascadeDelete(statement, "pdj_app_roles", StringLib.SQLformat(
                  "pdj_title = " + Constants.ST + 
                  " AND app_title = " + Constants.ST + 
                  " AND role_name NOT IN (" + roleStr + ")", items));                           
            statement.close();
            
            return true;
         }
         catch (Exception exception) {
            print("Error adding predefined job roles <" + 
                  exception.toString() + ">");
            return false;
         }
      }      
      
      private boolean checkPredefinedJobTitle()            
      {
         pdj_title = pdj_title.trim();
         
         if (pdj_title.length() < Constants.MIN_PREDEFINED_JOB_LEN ||
            pdj_title.length() > Constants.MAX_PREDEFINED_JOB_LEN) {
            print("PDJ title <" + pdj_title + "> is not in range <" + 
                  pdj_title.length() + ">");   
            return false;
         }
         
         return true;
      }
      
      private boolean checkCommandLine()
      {
         command_line = command_line.trim();
         
         if (command_line.length() > Constants.MAX_PROC_CMD_LINE_LEN) {
            print("String <" + command_line + "> is not the proper length <" +
                  command_line.length() + ">!");   
            return false;
         }
         
         return true;
      }            
      
      private boolean checkScheduleString()
      {
         String sched;
         String tmp;
         int value;
         
         if ((schedule_str.length() % Constants.MAX_SCHED_STR_LEN) != 0) {
            print("Schedule string in invalid format");
            return false;
         }
         
         for (int i = 0; i < schedule_str.length(); i++) {
            sched = schedule_str.substring(i, i + Constants.MAX_SCHED_STR_LEN);
            
            switch (sched.charAt(0)) {
               case 'H':
               case 'D':
                  break;

               case 'O':
               case 'E':
               case 'Y':
                  tmp = sched.substring(8, 10);
                  value = StringLib.parseInt(tmp, -1);
                  if ((value < 1) || (value > 12))
                     return false;
               case 'M':
                  tmp = sched.substring(6, 8);
                  value = StringLib.parseInt(tmp, -1);
                  if ((value < 1) || (value > 31))
                     return false;
                  break;

               case 'W':
                  tmp = sched.substring(5, 6);
                  value = StringLib.parseInt(tmp, -1);
                  if ((value < 0) || (value > 6))
                     return false;
                  break;

               default:
                  return false;                  
            }
            
            if (sched.charAt(0) != 'H') {
               tmp = sched.substring(3, 5);
               value = StringLib.parseInt(tmp, -1);
               if ((value < 0) || (value > 23))
                  return false;
            }
            
            tmp = sched.substring(1, 3);
            value = StringLib.parseInt(tmp, -1);
            if ((value < 0) || (value > 59))
               return false;    
            
            i += Constants.MAX_SCHED_STR_LEN;
         }
         
         return true;
      }
   }
}

