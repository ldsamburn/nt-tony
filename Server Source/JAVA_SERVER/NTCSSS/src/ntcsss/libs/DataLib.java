/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;

import java.util.StringTokenizer;
import java.util.Vector;
import java.util.Map;
import java.util.HashMap;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;

import COMMON.INI;

import ntcsss.log.Log;

import ntcsss.database.DatabaseConnectionManager;

/**
 * Library routines that access RDBMS or flat file data
 */
public class DataLib
{      
   private static final String VERSION_FILE = Constants.NTCSS_DB_DIR + "/VERSION";
    
   private static final Object CURRENT_USERS_LOCK = new Object();
   private static final Object VERSION_LOCK = new Object();
   private static final Object MASTER_NAME_CACHE_LOCK = new Object();
   
   private static String masterName;       
   
   /**
    * Obtain and stash the name of the master server
    */
   static 
   {
      cacheMasterName();                  
   }      
   
   /**
    * Makes sure the given common data tag follows certain criteria. Returns
    *  true if the string is ok.
    */
   public static boolean checkCommonDbTag(String tag)
   {      
      int  leng;    
      char currentChar;            

      leng = tag.trim().length();

      if (leng < Constants.MIN_COMDB_TAG_LEN || leng > Constants.MAX_COMDB_TAG_LEN)      
         return false;      

      for (int i = 0; i < leng; i++) {
         
         currentChar = tag.charAt(i);
         
         if (!(Character.isLetterOrDigit(currentChar) || (currentChar == '_') || 
            (currentChar == '-') || (currentChar == ',') || 
            (currentChar == '.') || (currentChar == ' ')))
            return false;        
      }
      
      return true;
   }
   
   /**
    * Makes sure the given common data value follows certain criteria. Returns
    *  true if the string is ok.
    */
   public static boolean checkCommonDbItem(String item)
   {  
      int  leng;  

      leng = item.trim().length();
      if (leng < Constants.MIN_COMDB_DATA_LEN || leng > Constants.MAX_COMDB_DATA_LEN)
         return false;    

      for (int i = 0; i < leng; i++) {
    
         if ((item.charAt(i) < 32) || (item.charAt(i) > 126))
            return false;            
      }
      
      return true;
   }
   
   /**
    * If "tag" is null, all the key/value pairs from the SYS_CONF table are 
    *  printed to the standard output stream.
    *  If no errors occur, 0 is returned.
    */
   public static int getNtcssSysConfData(String tag, StringBuffer data)
   {               
      String fnct_name = "getNtcssSysConfData";
      BufferedReader file;
      String item;
      int index;
      String value;
      int ret = ErrorCodes.ERROR_NO_SUCH_ITEM_REGISTERED;      
      Connection connection;
      Statement statement;
      ResultSet resultSet;                  
      
      // First look in NDS for the data
      if (tag != null) {
         
         if ((value = NDSLib.getSysConfValue(tag)) != null) {            
            data.delete(0, data.capacity());
            data.insert(0, value);
            return 0;
         }                               
         else
            Log.warn("DataLib", fnct_name, 
                     "NDS lookup failed for tag (" + tag + ")");          
      }
      
      // If control gets here, that means the tag wasn't found in NDS, so
      //  look in the SYS_CONF table for the tag
                                    
      // If not found in NIS, look up in the SYS_CONF file
      try {
            
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DataLib", fnct_name, "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
         }            
         statement = connection.createStatement();
            
         if (tag == null) {
            
            // Print the tag/value pairs to the screen
            resultSet = statement.executeQuery("SELECT tag, value " + 
                                               "FROM sys_conf");
            while (resultSet.next() == true) 
               System.out.println(resultSet.getString("tag") + ": " + 
                                  resultSet.getString("value"));                           
         }
         else {            
            
            resultSet = statement.executeQuery("SELECT value FROM sys_conf " + 
                     "WHERE tag = '" + StringLib.escapeCharacters(tag) + "'");
            if (resultSet.next() == true) {
               data.delete(0, data.capacity());
               data.insert(0, resultSet.getString("value"));
               ret = 0;
            }
         }
            
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);                  
      }
      catch (Exception exception) {
         return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
      }
           
      if (tag == null)
         return 0;

      return ret;      
   }
   
   /**
    * Puts the given tag/value pair in the SYS_CONF table. If the record 
    *  already exists, the value field is updated. If this is the
    *  master server, the data is also written out the NDS
    */
   public static int putNtcssSysConfData(String tag, String data)
   {               
      String fnct_name = "putNtcssSysConfData";
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      boolean exists;
      Object args[];
                  
      try {
            
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DataLib", fnct_name, "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
         }            
         statement = connection.createStatement();
            
         // Check to see if the tag exists
         resultSet = statement.executeQuery("SELECT value FROM sys_conf " + 
                   "WHERE tag = '" + StringLib.escapeCharacters(tag) + "'");
         exists = resultSet.next();
         resultSet.close();
         
         // If the record exists, update the record. Otherwise insert a new
         //  record
         args = new Object[2];
         if (exists == true) { 
            
            args[0] = data;
            args[1] = tag;
            statement.executeUpdate(StringLib.SQLformat("UPDATE sys_conf SET value = " + 
              Constants.ST + " WHERE tag = " + Constants.ST, args));              
         }
         else  {
            args[0] = tag;
            args[1] = data;
            statement.executeUpdate(StringLib.SQLformat("INSERT INTO sys_conf (tag, value) " + 
                 "VALUES (" + Constants.ST + ", " + Constants.ST + ")", args));
         }
         
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection); 
         
         // If this is the master server, update the NDS data
         if (isMasterServer() == true) {
            if (NDSLib.putSysConfValue(tag, data) == false)
               Log.error("DataLib", "putNtcssSysConfData", 
                         "Error updating NDS data");
         }
         
         return 0;
      }
      catch (Exception exception) {
         return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
      }                       
   }
   
   /**
    * Dumps the data in the current_users database table to a flat file. 
    *  This is done for backwards compatibility with "ntcss_cmd logins count"
    */
   public static boolean createFlatCurrUsersTable()
   {
      String str_fnct_name = "createFlatCurrUsersTable";
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      BufferedWriter file;
      String data;
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DataLib", str_fnct_name,  
                      "\"null\" connection returned");
            return false;
         }
         
         // Execute the query to get all the current user data
         statement = connection.createStatement();
         resultSet = statement.executeQuery(
                 "SELECT login_name, login_time, client_address, token, " +
                 "reserver_time FROM current_users");
                  
         // "Lock" access to the current_users file
         synchronized (CURRENT_USERS_LOCK) {
            
            // Open the file to write the data out to
            file = new BufferedWriter(new FileWriter(Constants.CURRENT_USERS_FILE,
                                                     false));            

            // Write the data to the file
            while (resultSet.next() == true) {
     
               data = resultSet.getString("login_name");
               file.write(data, 0, data.length());
               file.newLine();
               
               data = resultSet.getString("login_time");
               file.write(data, 0, data.length());
               file.newLine();
               
               data = resultSet.getString("client_address");
               file.write(data, 0, data.length());
               file.newLine();
               
               data = resultSet.getString("token");
               file.write(data, 0, data.length());
               file.newLine();
               
               file.write('0');   // Deprecated UID field
               file.newLine();
               
               data = resultSet.getString("reserver_time");
               file.write(data, 0, data.length());
               file.newLine();               
               
               file.newLine();
            }

            file.close();
         }
         
         // Close the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return true;
      }
      catch (Exception exception) {
         Log.excp("DataLib", str_fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Returns the type of the given NTCSS host. If an error occurs, a negative
    *  number is returned. Otherwise, a positive value indicating the type
    *  is returned.
    */
   public static int getHostType(String hostname)
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int nReturn;
      String SQLString;
      Object item[];
      int type;
      
      try {
         
         // If getNtcssHostType fails, then look the information
         //  up in the local database         
         if ((type = NDSLib.getNtcssHostType(hostname)) != -1)             
            return type;         
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DataLib", "getHostType",  
                      "\"null\" connection returned");
            return -1;
         }

         // Get the information from the database 
         item = new Object[1];
         item[0] = hostname.toUpperCase();
         SQLString = StringLib.SQLformat("SELECT type FROM hosts WHERE " +
                           "upper(hostname) = " + Constants.ST, item);
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);
  
         // Get the type from the returned result set
         if (resultSet.next() == false)
            nReturn = -2;
         else
            nReturn = resultSet.getInt("type");
         
         // Close the database resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return nReturn;
      }
      catch (Exception exception) {
         Log.excp("DataLib", "getHostType", exception.toString());
         return -1;
      }
   }   
   
   /**
    * Returns true if this host is the NTCSS master server
    */
   public static boolean isMasterServer()
   {
      if (getMasterServer(new StringBuffer()) == 1)
         return true;
      else
         return false;
   }
   
   /**
    * Retrieves the NTCSS master hostname that is stored in the local database
    *  and stores the result in the given buffer.
    * Returns 1 if current host is the master, 0 if not, negative value if error.    
    */
   public static int getMasterServer(StringBuffer masterHostName)
   {
      synchronized(MASTER_NAME_CACHE_LOCK) {
         
         // If the cached master name variable is null, attempt to cache
         //  the name.
         if (masterName == null) {
            cacheMasterName();
            
            if (masterName == null)
               return ErrorCodes.ERROR_CANT_DETER_MASTER;
         }
      
         // Clear out the given buffer
         masterHostName.delete(0, masterHostName.capacity());
      
         // Insert the master name into the given buffer
         masterHostName.insert(0, masterName);
      
         // Determine if this host is the master server or not
         if (masterName.equalsIgnoreCase(StringLib.getHostname()) == true)
            return 1;
         else
            return 0;
      }
   }
   
   /**
    * Gets the name of the master server and caches it to be retrieved by the
    *  getMasterServer method
    */
   public static void cacheMasterName()
   {
      String fnct_name = "cacheMasterName";
      Connection connection = null;
      Statement statement = null;
      ResultSet resultSet = null;
               
      synchronized(MASTER_NAME_CACHE_LOCK) {
         
         try {
                           
            // Clear the variable that the master name is cached in
            masterName = null;
         
            connection = DatabaseConnectionManager.getConnection();
            if (connection != null) {
               statement = connection.createStatement();
               resultSet = statement.executeQuery(
                                  "SELECT hostname FROM hosts WHERE type = " + 
                                  Constants.NTCSS_MASTER);
            
               if (resultSet.next() == true) 
                  masterName = resultSet.getString("hostname");            
               else 
                  Log.error("DataLib", fnct_name, "No master found!");
            
               resultSet.close();
               statement.close();
               DatabaseConnectionManager.releaseConnection(connection);
            }
            else 
               Log.error("DataLib", fnct_name,  "\"null\" connection returned");                                                     
         }
         catch (Exception exception) {
            Log.excp("DataLib", fnct_name, exception.toString());
            
            try {
               if (resultSet != null)
                  resultSet.close();
               if (statement != null)
                  statement.close();
               if (connection != null)
                  DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exp) {
            }
            
            masterName = null;
         }      
      }
   }
   
   /**
    * Returns the NTCSS host type of the given host from the progrps.ini file.
    *  If an error occurs or the host is not found, -1 is returned.
    */
   public static int getHostTypeFromInitData(String hostname)
   {
      return getHostTypeFromInitData(hostname, null);
   }
   
   public static int getHostTypeFromInitData(String hostname, INI ini)
   {
      String fnct_name = "getHostTypeFromInitData";      
      String hostSections;
      String progrpsFile = Constants.NTCSS_PROGRP_INIT_FILE;
      StringTokenizer hostTokenizer, entryTokenizer;    
      String hostSection, hostName, hostType;
            
      try {      
         if (ini == null)
            ini = new INI();
         
         // Get the HOSTS section from the INI file
         if ((hostSections = 
                ini.GetProfileSection("HOSTS", progrpsFile)) == null) {
            Log.error("DataLib", fnct_name, "GetProfileSection failed");
            return -1;
         }
         
         // Loop through the names of the host sections
         hostTokenizer = new StringTokenizer(hostSections, INI.STRINGS_DELIMITER);
         while (hostTokenizer.hasMoreElements() == true) {
            entryTokenizer = new StringTokenizer(hostTokenizer.nextToken(), 
                                                 INI.TAG_VALUE_DELIMITER);
            
            hostSection = entryTokenizer.nextToken();
            
            // Get the name of the host for this section
            if ((hostName = ini.GetProfileString(hostSection, "NAME", "default", 
                                                 progrpsFile)) == null) {
               Log.error("DataLib", fnct_name, "GetProfileString failed");
               return -1;
            }
            
            // Check to see if this is the host we're looking for
            if (hostName.equalsIgnoreCase(hostname) == false)
               continue;
            
            // Get the type of host
            if ((hostType = ini.GetProfileString(hostSection, "TYPE", "default", 
                                                 progrpsFile)) == null) {
               Log.error("DataLib", fnct_name, "GetProfileString failed");
               return -1;
            }
            
            // Check to see what host type this is
            if (hostType.equals("Master Server") == true)        
               return Constants.NTCSS_MASTER;        
            else if (hostType.equals("Authentication Server") == true)         
               return Constants.NTCSS_AUTH_SERVER;        
            else if (hostType.equals("Application Server") == true)         
               return Constants.NTCSS_APP_SERVER;        
            else
               return -1;
         }                      
      }
      catch (Exception exception) {
         Log.excp("DataLib", fnct_name, exception.toString());         
      }
      
      return -1;
   }
   
   /**
    * Removes all records from the current_users table. If an error occurs,
    *  "false" is returned.
    */
   public static boolean clearCurrentUsersTable()
   {
      Connection connection;
      Statement statement;      
      
      try {
         
         // Get a connection to the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("DataLib", "clearCurrentUsersTable",  
                      "\"null\" connection returned");
            return false;
         }

         // Get the information from the database 
         statement = connection.createStatement();
         statement.executeUpdate("DELETE FROM current_users");           
         
         // Close the database resources         
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);

         return createFlatCurrUsersTable();
      }
      catch (Exception exception) {
         Log.excp("DataLib", "clearCurrentUsersTable", exception.toString());
         return false;
      }
   } 
   
   /**
    * Gets the NTCSS database version out of the VERSION file
    */
   public static String getNtcssDatabaseVersion()
   {
      File fileDescp;
      FileReader reader;
      char buffer[];
      int bytesRead;
      
      try {
         
         // Check to see if the VERSION file exists
         fileDescp = new File(VERSION_FILE);
         if (fileDescp.exists() == false)
            return null;
         
         // "Lock" access to the VERSION file
         synchronized (VERSION_LOCK) {
            
            // Read version out of the file
            reader = new FileReader(VERSION_FILE);            
            buffer = new char[20];
            bytesRead = reader.read(buffer);            
            reader.close();
         }
         
         if (bytesRead == -1)
            return null;
         else
            return (new String(buffer, 0, bytesRead)).trim();
      }
      catch (Exception exception) {
         Log.excp("DataLib", "getNtcssDatabaseVersion", exception.toString());
         return null;
      }
   }
   
   /**
    * Writes this release's NTCSS database version out to the VERSION file
    */
   public static boolean writeNtcssDatabaseVersion()
   {
      File fileDescp;
      FileWriter writer;
      
      try {
         
         // "Lock" access to the VERSION file
         synchronized (VERSION_LOCK) {
            
            // Write version out to the file
            writer = new FileWriter(VERSION_FILE, false);            
            writer.write(Constants.NTCSS_DB_VERSION);
            writer.close();
         }
         
         return true;
      }
      catch (Exception exception) {
         Log.excp("DataLib", "writeNtcssDatabaseVersion", exception.toString());
         return false;
      }
   }
   
   /**
    * Returns the number of current users specified in the given file
    */
   public static int countCurrentUsers(String file)
   {
      BufferedReader reader;      
      String loginTime;
      int count;
      
      try {
         // Create reader to read from the given file
         reader = new BufferedReader(new FileReader(file));
         
         count = 0;
         while (reader.readLine() != null) {
            loginTime = reader.readLine();
            reader.readLine();
            reader.readLine();
            reader.readLine();
            reader.readLine();
            reader.readLine();
            
            if (loginTime.trim().equals("0") == false)
               count++;
         }
         
         reader.close();         
         return count;
      }
      catch (Exception exception) {
         Log.excp("DataLib", "countCurrentUsers", exception.toString());
         return -1;
      }
   }
   
   /**
    * Creates the NOLOGINS file
    */
   public static void createNOLOGINSFile()
   {
      File file;
      
      try {
         file = new File(Constants.NOLOGINS_FILE);         
         file.createNewFile();
      }
      catch (Exception exception) {
      }
   }
   
   /**
    * Remove the NOLOGINS file
    */
   public static void removeNOLOGINSFile()
   {
      File file;
      
      try {
         file = new File(Constants.NOLOGINS_FILE);         
         if (file.exists() == true)
            file.delete();
      }
      catch (Exception exception) {
      }
   }     
   
   /**
    * Returns the current number of users logged into this host. If an
    *  error occurs, -1 is returned.
    */
   public static int numCurrentUsers(Connection connection)
   {
      Connection con;
      Statement statement;
      ResultSet resultSet;
      int numCurrentUsers;
      
      try {
         
         // Get a database connection if one is not given
         if (connection != null)
            con = connection;
         else {
            if ((con = DatabaseConnectionManager.getConnection()) == null)            
               return -1;
         }         
         statement = con.createStatement();
         
         // Get the number of users logged in to this server
         resultSet = statement.executeQuery("SELECT count(*) AS " + 
                             "cnt FROM current_users WHERE login_time != '0'");                   
         resultSet.next();         
         numCurrentUsers = resultSet.getInt("cnt");                                              
         
         // Close the database resources
         resultSet.close();         
         statement.close();
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(con);
         
         return numCurrentUsers;         
      }
      catch (Exception exception) {
         return -1;
      }
   }
   
   /**
    * Creates an INI file with login history data records to be used by
    *  the control panel. If no errors occur, true is returned.
    */
   public static boolean createLoginHistoryINI(String INIfilename, 
                                               int maxRecords)
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int numINIRecords = 0;
      INI iniFile = new INI();
      String sectionName;
      int status;
      String date, formattedDate;
      boolean moreRecordsFound = true;
      File fileDescp;
      
      try {
         
         // Remove the INI file if it exists
         fileDescp = new File(INIfilename);
         if (fileDescp.exists() == true)
            fileDescp.delete();
         
         // Get a database connection
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {            
            Log.error("DataLib", "createLoginHistoryINI", 
                      "Could not get database connection");
            return false;
         }                  
         statement = connection.createStatement();
         
         // Get the login history records
         resultSet = statement.executeQuery("SELECT * FROM login_history " + 
                                            "ORDER by login_time ASC");       
         
         // Move the cursor to the bottom "maxRecords" number of records         
         if (resultSet.absolute(-1 * maxRecords) == false) 
            moreRecordsFound = resultSet.first();            

         // Write out an INI section for each login history record
         while (moreRecordsFound == true) {                   
            
            sectionName = "LOGIN_HISTORY " + ++numINIRecords;
            iniFile.WriteProfileString(sectionName, "ENTRY_NUMBER", 
                                       String.valueOf(numINIRecords), INIfilename);
            
            // Format the login time field
            date = resultSet.getString("login_time");
            formattedDate = date.substring(2, 4) + "/" + 
                            date.substring(4, 6) + "/" +
                            date.substring(0, 2) + " " +
                            date.substring(7, 9) + ":" + 
                            date.substring(9, 11);
            iniFile.WriteProfileString(sectionName, "EVENT_TIME", formattedDate, 
                                       INIfilename);
            
            
            // Write out the status field
            status = resultSet.getInt("status");
            switch (status) {
               
               case Constants.GOOD_LOGIN_STATE:
                  iniFile.WriteProfileString(sectionName, "EVENT", "Logged In",
                                             INIfilename);
                  break;

               case Constants.FAILED_LOGIN_STATE:
                 iniFile.WriteProfileString(sectionName, "EVENT", "Failed Login",
                                            INIfilename);
                  break;

               case Constants.LOGOUT_STATE:
                  iniFile.WriteProfileString(sectionName, "EVENT", "Logged Out",
                                             INIfilename);
                  break;

               case Constants.RECORD_LOGOUT_STATE:
                  iniFile.WriteProfileString(sectionName, "STATUS",
                                             "Recorded Log Out", INIfilename);
                  break;

               case Constants.LOGIN_RESERVED_STATE:
                  iniFile.WriteProfileString(sectionName, "EVENT",
                                             "Recorded Login Reservation",
                                             INIfilename);
                  break;

               case Constants.REMOVE_RESERVED_STATE:
                  iniFile.WriteProfileString(sectionName, "EVENT",
                                              "Removed Login Reservation",
                                              INIfilename);
                  break;
            }
            
            // Write out the client address
            iniFile.WriteProfileString(sectionName, "CLIENT_ADDRESS",
                                       resultSet.getString("client_address"), 
                                       INIfilename);
            
            // Write out the login name
            iniFile.WriteProfileString(sectionName, "LOGIN_NAME", 
                                       resultSet.getString("login_name"),
                                       INIfilename);
            
            // See if there are any more records to process
            moreRecordsFound = resultSet.next();
         }
         
         // Close the database resources
         resultSet.close();         
         statement.close();         
         DatabaseConnectionManager.releaseConnection(connection);
         
         // Write out the total number of records that were written to the
         //  INI file
         iniFile.WriteProfileString("LOGIN_HISTORY", "RECORD_COUNT", 
                                    String.valueOf(numINIRecords), INIfilename);
         iniFile.FlushINIFile(INIfilename);
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "createLoginHistoryINI", exception.toString());
         return false;
      }
   }
   
   /**
    * Informs the log daemon to create an INI file containing log file entries.
    *  If any errors occur, false is returned.
    */
   public static boolean createSystemLogINI(String outputFile, int blockSize, 
                                            int maxRecordCount, 
                                            int dateFilterCode, 
                                            boolean filterByEntity,
                                            String startDate, String endDate, 
                                            Vector entityList) 
   {   
      try {
         return Log.createSystemLogINI(outputFile, blockSize, 
                                        maxRecordCount, dateFilterCode, 
                                        filterByEntity, startDate, endDate, 
                                        entityList);
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "createSystemLogINI", exception.toString());
         return false;
      }
   }
   
   /**
    * Creates a bulletin board for the user and subscribes them to the
    *  proper NTCSS bulletin boards. If the parameter "connection" is null,
    *  a temporary database connection is made.
    */
   public static boolean setupStandardBBUserInfo(String user, 
                                                 boolean isNtcssAppAdmin, 
                                                 Connection connection)
   {  
      Connection conn;
      Object sqlItems[];
      Statement statement;
      String SQLString;
      
      try {
         
         // Check to see if a temporary database connection is needed
         if (connection != null)
            conn = connection;
         else
            conn = DatabaseConnectionManager.getConnection();
         
         statement = conn.createStatement();
         
         // Create a bulletin board that has the same name as the user
         sqlItems = new Object[2];
         sqlItems[0] = user;
         sqlItems[1] = user;
         SQLString = StringLib.SQLformat("INSERT INTO bulletin_boards " + 
                        "(bb_name, creator) VALUES(" + Constants.ST + "," + 
                        Constants.ST + ")", sqlItems);
         statement.executeUpdate(SQLString);
         statement.close();
         
         // NOTE: for some reason, in the C code version of the server, the 
         //  user is not automatically subscribed to their own bulletin board.
         //  So, it is not done here for consistency.
         
         // Now subscribe the user to the NTCSS bulletin boards
         subscribeToAppBulletinBoard(user, Constants.NTCSS_APP, conn);
         if (isNtcssAppAdmin)
            subscribeToAppAdminBulletinBoard(user, Constants.NTCSS_APP, conn);
         
         // If a temporary database connection was used, close it now
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(conn);
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "setupStandardBBUserInfo", exception.toString());
         return false;
      }
   }
   
   /**
    * Removes the user's bulletin board and unsubscribes them from the
    *  proper NTCSS bulletin boards. If the parameter "connection" is null,
    *  a temporary database connection is made.
    */
   public static boolean clearStandardBBUserInfo(String user, 
                                                 boolean isNtcssAppAdmin, 
                                                 Connection connection)
   {  
      Connection conn;
      Object sqlItems[];
      Statement statement;
      String SQLString;
      
      try {
         
         // Check to see if a temporary database connection is needed
         if (connection != null)
            conn = connection;
         else
            conn = DatabaseConnectionManager.getConnection();
         
         statement = conn.createStatement();
         
         // Remove the user's bulletin board that has the same name as the user
         sqlItems = new Object[2];
         sqlItems[0] = user;
         sqlItems[1] = user;
         SQLString = StringLib.SQLformat("DELETE FROM bulletin_boards " + 
                        "WHERE bb_name = " + Constants.ST + " AND creator = " 
                        + Constants.ST, sqlItems);
         statement.executeUpdate(SQLString);
         statement.close();
         
         // NOTE: for some reason, in the C code version of the server, the 
         //  user is not automatically subscribed to their own bulletin board.
         //  So, the unsubscribe is not done here for consistency.
         
         // Now unsubscribe the user from the NTCSS bulletin boards
         unsubscribeFromAppBulletinBoard(user, Constants.NTCSS_APP, conn);
         if (isNtcssAppAdmin)
            unsubscribeFromAppAdminBulletinBoard(user, Constants.NTCSS_APP, conn);
         
         // If a temporary database connection was used, close it now
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(conn);
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "clearStandardBBUserInfo", exception.toString());
         return false;
      }
   }   
   
   /**
    * Subscribes the given user to the "app".all bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean subscribeToAppBulletinBoard(String user, String app,                                                        
                                                      Connection connection)
   {  
      return subscribeToBulletinBoard(user, app + ".all", connection);
   }     
   
   /**
    * Subscribes the given user to the "app".adm bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean subscribeToAppAdminBulletinBoard(String user, String app,                                                        
                                                      Connection connection)
   {  
      return subscribeToBulletinBoard(user, app + ".adm", connection);
   }     
       
   /**
    * Unsubscribes the given user from the "app".all bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean unsubscribeFromAppBulletinBoard(String user, String app,                                                        
                                                      Connection connection)
   {        
      return unsubscribeFromBulletinBoard(user, app + ".all", connection);
   }
   
   /**
    * Unsubscribes the given user from the "app".adm bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean unsubscribeFromAppAdminBulletinBoard(String user, String app,                                                        
                                                      Connection connection)
   {        
      return unsubscribeFromBulletinBoard(user, app + ".adm", connection);
   }
   
   /**
    * Subscribes the given user to the given bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean subscribeToBulletinBoard(String user, String bb_name,                                                        
                                                      Connection connection)
   {  
      Connection conn;
      Object sqlItems[];
      Statement statement;
      String SQLString;
      ResultSet resultSet;
      
      try {
         
         // Check to see if a temporary database connection is needed
         if (connection != null)
            conn = connection;
         else
            conn = DatabaseConnectionManager.getConnection();
         
         // Check to make sure the message exists first
         if (doesBulletinBoardExist(bb_name, conn) < 0) {
            Log.error("DataLib", "subscribeToBulletinBoard", "Bulletin board <" 
                      + bb_name + "> does not exist");
            return false;
         }
         
         statement = conn.createStatement();
         
         // See if the user is already subscribed to the bulletin board
         sqlItems = new Object[2];
         sqlItems[0] = user;         
         sqlItems[1] = bb_name;
         SQLString = StringLib.SQLformat("SELECT * FROM bb_subscribers " + 
                        "WHERE login_name = " + Constants.ST + " AND bb_name = " 
                        + Constants.ST, sqlItems);     
         resultSet = statement.executeQuery(SQLString);        
         
         if (resultSet.next() == false) { // i.e. not subscribed
            resultSet.close();
            
            // Subscribe the user to the given bulletin board  
            SQLString = StringLib.SQLformat("INSERT INTO bb_subscribers " + 
                        "(login_name, bb_name) VALUES(" + Constants.ST + "," + 
                        Constants.ST + ")", sqlItems);     
            statement.executeUpdate(SQLString);                  
         }
         else
            resultSet.close();
         
         statement.close();
         // If a temporary database connection was used, close it now
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(conn);
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "subscribeToBulletinBoard", exception.toString());
         return false;
      }
   }     
   
   /**
    * Unsubscribes the given user from the given bulletin board.  If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static boolean unsubscribeFromBulletinBoard(String user, String bb_name,                                                        
                                                      Connection connection)
   {  
      Connection conn;
      Object sqlItems[];
      Statement statement;
      String SQLString;     
      
      try {
         
         // Check to see if a temporary database connection is needed
         if (connection != null)
            conn = connection;
         else
            conn = DatabaseConnectionManager.getConnection();
         
         statement = conn.createStatement();
         
         // Remove the record if it exists
         sqlItems = new Object[2];
         sqlItems[0] = user;         
         sqlItems[1] = bb_name;
         SQLString = StringLib.SQLformat("DELETE FROM bb_subscribers " + 
                        "WHERE login_name = " + Constants.ST + " AND bb_name = " 
                        + Constants.ST, sqlItems);     
         statement.executeUpdate(SQLString);                          
         
         statement.close();
         // If a temporary database connection was used, close it now
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(conn);
         
         return true;
      }
      catch (Exception exception) {
         
         Log.excp("DataLib", "unsubscribeFromBulletinBoard", exception.toString());
         return false;
      }
   }     
   
   /**
    * Checks to see if the given bulletin board exists. Returns
    *  1 if it exists. Otherwise a negative integer is returned. If the 
    *  parameter "connection" is null, a temporary database connection is made.
    */
   public static int doesBulletinBoardExist(String bulletinBoard, 
                                      Connection connection)
   {
      String fnct_name = "doesBulletinBoardExist";   
      String SQLString;   
      Statement statement;
      ResultSet resultSet;
      Object sqlItems[];
      int retCode;
      Connection conn;

      try {
         
         // Check to see if a temporary database connection is needed
         if (connection != null)
            conn = connection;
         else
            conn = DatabaseConnectionManager.getConnection();

         // See if the given bulletin exists in the bulletin_boards table 
         sqlItems = new Object[1];
         sqlItems[0] = bulletinBoard;         
         SQLString = StringLib.SQLformat("SELECT * FROM bulletin_boards " +
                     "WHERE bb_name = " + Constants.ST, sqlItems);
         statement = conn.createStatement();
         resultSet = statement.executeQuery(SQLString);
         
         if (resultSet.next() == false)
            retCode = ErrorCodes.ERROR_BB_DOES_NOT_EXIST;
         else
            retCode = 1;
         
         resultSet.close();
         statement.close();
         
         // If a temporary database connection was used, close it now
         if (connection == null)
            DatabaseConnectionManager.releaseConnection(conn);
         
         return retCode;
      }
      catch (Exception exception) {
         Log.excp("DataLib", fnct_name, exception.toString());
         return ErrorCodes.ERROR_CANT_ACCESS_DB;
      }
   }

   /**
    * Deletes records from the given table based on the "where" clause given.
    *  Also, any tables that have foreign keys to this table will have their
    *  cooresponding dependent records deleted.  NOTE: The where clause 
    *  parameter is to NOT include the word "WHERE" at the beginning. This 
    *  will automatically be prepended to the given string. If whereCluase is
    *  null, all records will deleted from the table and its dependent tables. 
    *  The return value is whatever Statement.executeUpdate returns.
    *  If "connection" is null, a temporary connection is created.   
    */
   public static int cascadeDelete(String tableName, String whereClause, 
                                   Connection connection) throws SQLException
   {
      Connection con;
      int ret = 0;
      Statement statement;      
      SQLException sqlException = null;
      
      // Create a temporary connection if given null connection
      if (connection == null)
         con = DatabaseConnectionManager.getConnection();
      else
         con = connection;
      
      statement = con.createStatement();
      
      // Catch any exception this may throw so that the database resources
      //  can be closed properly. Then, if an exception was thrown, re-throw it.
      try {
         ret = cascadeDelete(statement, tableName, whereClause);
      }
      catch (SQLException exception) {
         sqlException = exception;
      }
      
      statement.close();
      
      // If a temporary connection was created, close it
      if (connection == null)
         DatabaseConnectionManager.releaseConnection(con);
      
      if (sqlException != null)
         throw sqlException;
      
      return ret;
   }
   
   /**
    * Deletes records from the given table based on the "where" clause given.
    *  Also, any tables that have foreign keys to this table will have their
    *  cooresponding dependent records deleted.  NOTE: The where clause 
    *  parameter is to NOT include the word "WHERE" at the beginning. This 
    *  will automatically be prepended to the given string. If whereCluase is
    *  null, all records will deleted from the table and its dependent tables. 
    *  The return value is whatever Statement.executeUpdate returns.
    */
   public static int cascadeDelete(Statement statement, String tableName, 
                                   String whereClause) throws SQLException                          
   {                            
      String info;      
      ResultSet resultSet;
      StringTokenizer dependentTablesTokenizer, tokenizer;
      String localFields, dependentTable, dependentTableFields;                  
      String newWhereClause;
      Object items[];
      String dependentFieldNames[];
      String tmpString;
      int ret;     
      String SQLstring;
            
      // First, delete records from tables which contain foreign keys to 
      //  this table      
      if ((info = Tables.getInfo(tableName)) != null) {
         
         // Parse table information. See the Tables class for a description
         //  of the information and its format.
         dependentTablesTokenizer = new StringTokenizer(info, Tables.sep1);         
         while (dependentTablesTokenizer.hasMoreTokens() == true) {
            
            //  Parse information on dependent table
            tokenizer = new StringTokenizer(dependentTablesTokenizer.nextToken(), 
                                            Tables.sep2);
            localFields = tokenizer.nextToken();
            dependentTable = tokenizer.nextToken();
            dependentTableFields = tokenizer.nextToken();
                        
            // Get rows from the given table which will be affected by the
            //  requested delete
            if (whereClause != null)
               resultSet = statement.executeQuery("SELECT " + 
                   localFields.replace(Tables.sep3.charAt(0), ',') + " FROM " + 
                                               tableName + " WHERE " + whereClause);
            else
               resultSet = statement.executeQuery("SELECT " + 
                   localFields.replace(Tables.sep3.charAt(0), ',') + " FROM " + tableName);
            
            // Determine if this is a composite foreign key
            tokenizer = new StringTokenizer(dependentTableFields, Tables.sep3);
            items = new Object[tokenizer.countTokens()];                  
            dependentFieldNames = new String[items.length];               
            int i = 0;
            while (tokenizer.hasMoreElements() == true) 
               dependentFieldNames[i++] = tokenizer.nextToken();            

                   
            // Create a where clause for the dependent table which will delete
            //  the records in that table that rely on the records returned
            //  from the above "SELECT" 
            //
            // ASSUMPTION: assuming all foreign keys are of string db type
            newWhereClause = null;
            while (resultSet.next() == true) {               
               
               tmpString = null;                  
               for (i = 0; i < items.length; i++) {
                  items[i] = resultSet.getString(i + 1);
                     
                  if (tmpString == null)
                     tmpString = "((" + dependentFieldNames[i] + " = " + Constants.ST + ")";
                  else
                     tmpString += " AND (" + dependentFieldNames[i] + " = " + Constants.ST + ")";
               }
               tmpString += ")";
                                                      
                  
               if (newWhereClause == null)       
                  newWhereClause = StringLib.SQLformat(tmpString, items);
               else
                  newWhereClause += StringLib.SQLformat(" OR " + tmpString, items);                                 
            }
            
            resultSet.close();
            
            // Delete records from the dependent table. Stop immediatly if 
            //  there is an error
            if ((newWhereClause != null) &&
                ((ret = cascadeDelete(statement, dependentTable, newWhereClause)) < 0))
               return ret;
            
         } // End "while" loop
      }
      
      // Now, delete requested records from requested table        
      if (whereClause != null)
         SQLstring = "DELETE FROM " + tableName + " WHERE " + whereClause;         
      else
         SQLstring = "DELETE FROM " + tableName;  
      
      Log.debug("DataLib", "cascadeDelete", SQLstring);
      return statement.executeUpdate(SQLstring);
   }      
}

/**
 * Contains information about the database tables. 
 *
 * NOTE: !!!When tables modified, the data here must be updated also!!!
 */
class Tables
{
   public static final String sep1 = "?";
   public static final String sep2 = "+";
   public static final String sep3 = "*";  
   
   private static Map tables;    
   
   // Each table has a string of information. That data indicates what 
   //  tables have foreign keys to that table and what the field names are
   //  in both tables.
   //
   // e.g.  If  Table2.fieldA is a foreign key to Table1.fieldY, then
   //       Table1 will have the following information:
   //       "fieldY/Table2/fieldA"
   //       If Table2.fieldA and Table2.fieldB make up a composite foriegn key
   //       to Table1.fieldY and Table1.fieldZ, then Table1 will have the 
   //       following information:
   //       "fieldY,fieldZ/Table2/fieldA,fieldB"
   
   static 
   {
      tables = new HashMap();      
      
      tables.put("HOSTS", 
        "hostname" + Tables.sep2 + "USERS" + Tables.sep2 + "auth_server" + Tables.sep1 +
        "hostname" + Tables.sep2 + "APPS" + Tables.sep2 + "hostname" + Tables.sep1 +
        "host_ip_address" + Tables.sep2 + "PRINTERS" + Tables.sep2 + "host_ip_address");
      
      // The active_jobs/scheduled_jobs.job_initator & job_requestor fields are
      //  not foreign keyed to this table. See create_table.sql file for details.
      tables.put("USERS", 
        "login_name" + Tables.sep2 + "CURRENT_USERS" + Tables.sep2 + "login_name" + Tables.sep1 +
        "login_name" + Tables.sep2 + "APP_USERS" + Tables.sep2 + "login_name" + Tables.sep1 +
        "login_name" + Tables.sep2 + "PRINT_REQUESTS" + Tables.sep2 + "login_name" + Tables.sep1 +
        "login_name" + Tables.sep2 + "BB_SUBSCRIBERS" + Tables.sep2 + "login_name" + Tables.sep1 +
        "login_name" + Tables.sep2 + "BB_MESSAGES" + Tables.sep2 + "login_name" + Tables.sep1 +
        "login_name" + Tables.sep2 + "BB_READ_HISTORY" + Tables.sep2 + "login_name");      
      
      tables.put("APPS", 
        "app_title" + Tables.sep2 + "APP_ROLES" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "APP_USERS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "APP_PROGRAMS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "APP_PRINTERS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "PREDEFINED_JOBS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "PRINT_REQUESTS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "ACTIVE_JOBS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "SCHEDULED_JOBS" + Tables.sep2 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep2 + "OUTPUT_TYPES" + Tables.sep2 + "app_title");
      
      tables.put("APP_ROLES", 
        "role_name" + Tables.sep3 + "app_title" + Tables.sep2 + "USER_APP_ROLES" + Tables.sep2 + "role_name" + Tables.sep3 + "app_title" + Tables.sep1 +
        "app_title" + Tables.sep3 + "role_name" + Tables.sep2 + "APP_PROGRAM_ROLES" + Tables.sep2 + "app_title" + Tables.sep3 + "role_name" + Tables.sep1 +
        "role_name" + Tables.sep3 + "app_title" + Tables.sep2 + "PDJ_APP_ROLES" + Tables.sep2 + "role_name" + Tables.sep3 + "app_title");
      
      tables.put("PRINT_DRIVERS", 
        "driver_name" + Tables.sep2 + "PRINT_DRIVER_FILES" + Tables.sep2 + "driver_name" + Tables.sep1 +
        "driver_name" + Tables.sep2 + "PRINTERS" + Tables.sep2 + "driver_name");
      
      // The active_jobs/scheduled_jobs.job_initator & job_requestor fields are
      //  not foreign keyed to this table. See create_table.sql file for details.
      tables.put("PRINTERS", 
        "printer_name" + Tables.sep2 + "APP_PRINTERS" + Tables.sep2 + "printer_name" + Tables.sep1 +
        "printer_name" + Tables.sep2 + "PRINT_REQUESTS" + Tables.sep2 + "printer_name");
      
      tables.put("APP_PRINTERS",                         
        "app_title" + Tables.sep3 + "printer_name" + Tables.sep2 + "OUTPUT_PRINTERS" + Tables.sep2 + "app_title" + Tables.sep3 + "printer_name");
      
      tables.put("BULLETIN_BOARDS", 
        "bb_name" + Tables.sep2 + "BB_SUBSCRIBERS" + Tables.sep2 + "bb_name" + Tables.sep1 +
        "bb_name" + Tables.sep2 + "BB_MESSAGE_MAP" + Tables.sep2 + "bb_name");
      
      tables.put("BB_MESSAGES", 
        "msg_id" + Tables.sep2 + "BB_READ_HISTORY" + Tables.sep2 + "msg_id" + Tables.sep1 +
        "msg_id" + Tables.sep2 + "BB_MESSAGE_MAP" + Tables.sep2 + "msg_id");
      
      tables.put("APP_USERS", 
        "app_title" + Tables.sep3 + "login_name" + Tables.sep2 + "USER_APP_ROLES" + Tables.sep2 + "app_title" + Tables.sep3 + "login_name");
      
      tables.put("APP_PROGRAMS", 
        "program_title" + Tables.sep3 + "app_title" + Tables.sep2 + "APP_PROGRAM_ROLES" + Tables.sep2 + "program_title" + Tables.sep3 + "app_title");            
      
      tables.put("PRINTER_TYPES", 
        "printer_type" + Tables.sep2 + "PRINTERS" + Tables.sep2 + "printer_type");
      
      tables.put("PRINTER_ADAPTERS", 
        "adapter_ip_address" + Tables.sep2 + "PRINTERS" + Tables.sep2 + "adapter_ip_address"); 
        
      tables.put("OUTPUT_TYPES", 
        "otype_title" + Tables.sep3 + "app_title" + Tables.sep2 + "OUTPUT_PRINTERS" + Tables.sep2 + "otype_title" + Tables.sep3 + "app_title");
      
      tables.put("PREDEFINED_JOBS", 
        "pdj_title" + Tables.sep3 + "app_title" + Tables.sep2 + "PDJ_APP_ROLES" + Tables.sep2 + "pdj_title" + Tables.sep3 + "app_title");
      
      tables.put("SCHEDULED_JOBS", 
        "ntcss_job_id" + Tables.sep2 + "JOB_SCHEDULES" + Tables.sep2 + "ntcss_job_id");
            
      // It is desired that remove a ADAPTER_TYPE does not cascade down the
      //  database table structure. The way this should be accomplished is
      //  to disassociate any printers from the adapter of this type, then
      //  remove the adapter, and then remove the adapter_type
      tables.put("ADAPTER_TYPES", null);  
      
      tables.put("LOGIN_HISTORY", null);
      tables.put("CURRENT_USERS", null);
      tables.put("USER_APP_ROLES", null);
      tables.put("APP_PROGRAM_ROLES", null);
      tables.put("PRINT_DRIVER_FILES", null);      
      tables.put("OUTPUT_PRINTERS", null);
      tables.put("PDJ_APP_ROLES", null);
      tables.put("PRINT_REQUESTS", null);
      tables.put("ACTIVE_JOBS", null);
      tables.put("JOB_SCHEDULES", null);
      tables.put("BB_SUBSCRIBERS", null);
      tables.put("BB_READ_HISTORY", null);
      tables.put("BB_MESSAGE_MAP", null);
      tables.put("SYS_CONF", null);      
   }
   
   public static String getInfo(String tableName)
   {
      return (String)tables.get(tableName.toUpperCase());
   }
}