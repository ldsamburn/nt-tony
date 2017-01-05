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
import java.io.FileOutputStream;
import java.io.FileInputStream;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;
import java.util.StringTokenizer;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import COMMON.FTP;
import COMMON.INI;

import ntcsss.log.Log;

import ntcsss.libs.Constants;
import ntcsss.libs.DataLib;
import ntcsss.libs.StringLib;
import ntcsss.libs.PrintLib;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.util.FileOps;

import com.ice.tar.TarArchive;
import com.ice.tar.TarEntry;

/**
 * Functions dealing with the replication of the NTCSS database
 */
public class ReplicationLib
{  
   private static final String NTCSS_BACKUP_TEMP_TAR_FILE = 
                  Constants.NTCSS_APP_DATA_INSTALL_DIR + "/master.temp_tar";
   private static final String NTCSS_BACKUP_TAR_FILE = 
                  Constants.NTCSS_APP_DATA_INSTALL_DIR + "/master.tar";
   private static final String NTCSS_BACKUP_TEMP_INFO_FILE =
                  Constants.NTCSS_APP_DATA_INSTALL_DIR + "/master.temp_info";
   private static final String NTCSS_BACKUP_INFO_FILE =
                  Constants.NTCSS_APP_DATA_INSTALL_DIR + "/master.info";
   
   private static final String UNIX_PATH_NTCSS_BACKUP_TEMP_TAR_FILE = 
                  "/h/NTCSSS/applications/data/install/master.temp_tar";
   private static final String UNIX_PATH_NTCSS_BACKUP_TEMP_INFO_FILE =
                  "/h/NTCSSS/applications/data/install/master.temp_info";
      
   public static final String FIELD_SEPARATOR = "\n";
   
   public static final String ADAPTER_TYPES_FILE = "database/ADAPTER_TYPES";
   public static final String SYS_CONF_FILE = "database/SYS_CONF";
   public static final String VERSION_FILE = "database/VERSION";
   public static final String APP_ROLES_FILE = "database/access_roles";
   public static final String PRINTER_ADAPTERS_FILE = "database/adapters";
   public static final String APP_USERS_FILE = "database/appusers";
   public static final String HOSTS_FILE = "database/hosts";
   public static final String LOGIN_HISTORY_FILE = "database/login_history";
   public static final String USERS_FILE = "database/ntcss_users";
   public static final String OUTPUT_PRINTERS_FILE = "database/output_prt";
   public static final String OUTPUT_TYPES_FILE = "database/output_type";
   public static final String APP_PRINTERS_FILE = "database/printer_access";
   public static final String PRINTER_TYPES_FILE = "database/prt_class";
   public static final String PRINTERS_FILE = "database/printers";
   public static final String PRINTER_DRIVERS_FILE = "database/ntprint.ini";
   public static final String APP_PROGRAMS_FILE = "database/programs";
   public static final String APPS_FILE = "database/progroups";
   public static final String CURRENT_APPS_FILE = "database/current_apps";
   public static final String PREDEFINED_JOBS_FILE = "database/predefined_jobs";
   public static final String BULLETIN_BOARDS_FILE = "database/bulletinBoards";   
   public static final String BULLETIN_BOARD_SUBSCRIBERS_FILE = "database/bbSubscribers";
   public static final String BULLETIN_BOARD_MESSAGES_FILE = "database/bbMessages";
   public static final String BULLETIN_BOARD_MESSAGE_MAP_FILE = "database/bbMessageMap";
   public static final String BULLETIN_BOARD_READ_HISTORY_FILE = "database/bbReadHistory";
   public static final String NTDRIVER_DIR = "spool/ntdrivers";   
   
   /**
    * Gets the backup files from the master. Also calls a function to
    *  check the file sizes and renames the tmp files to their usual names. If
    *  no errors occur, 0 is returned. Otherwise, a negative number is returned.
    */
   public static int copyMasterBackupFiles()
   {
      int     ret;
      String fnct_name = "copyMasterBackupFiles";
      StringBuffer strMasterHost;      
      String filename;

      try {
         
         // Get the name of the master server
         strMasterHost = new StringBuffer();
         ret = DataLib.getMasterServer(strMasterHost);
         if (ret < 0) {         
            Log.error("ReplicationLib", fnct_name, 
                      "Failed to determine master server!");
            return -1;
         }

         // If I am master, I don't send the file to myself 
         if (ret != 0) {         
            Log.info("ReplicationLib", fnct_name, 
                   "Attempted to copy backup files to the NTCSS master server!");
            return -2;
         } 

         // Get the backup DB file                   
         if (FTP.get(strMasterHost.toString(), 
                          UNIX_PATH_NTCSS_BACKUP_TEMP_TAR_FILE,
                          NTCSS_BACKUP_TEMP_TAR_FILE, false) == false) {
            Log.error("ReplicationLib", fnct_name, "Failed to get file (" + 
                      UNIX_PATH_NTCSS_BACKUP_TEMP_TAR_FILE + ") from master (" + 
                      strMasterHost.toString() + ")!");
            return -3;
         }

         // Get the temp info file for the backup file      
         if (FTP.get(strMasterHost.toString(), 
                          UNIX_PATH_NTCSS_BACKUP_TEMP_INFO_FILE,
                          NTCSS_BACKUP_TEMP_INFO_FILE, true) == false) {                                      
            Log.error("ReplicationLib", fnct_name, "Failed to get file (" + 
                      UNIX_PATH_NTCSS_BACKUP_TEMP_INFO_FILE + ") from master(" + 
                      strMasterHost.toString() + ")!");
            return -4;
         }

         // Check for correct file sizes and rename the files 
         if (verifyCopiedMasterBackupFile() == false)
            return -4;

         return 0;      
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", fnct_name, exception.toString());
         return -5;
      }
   }
   
   /**
    * Displays the contents of the master backup info file
    */
   public static void displayBackupInfo()
   {
      String fnct_name = "displayBackupInfo";
      BufferedReader reader;
      String size, version, masterServer;
      File file;
      
      try {
         file = new File(NTCSS_BACKUP_INFO_FILE);
         
         if (file.exists() == false) {
            System.out.println(NTCSS_BACKUP_INFO_FILE + " file doesn't exist");
            return;
         }
         
         reader = new BufferedReader(new FileReader(file));
         
         size = reader.readLine();
         if (size == null) {
            System.out.println("Unable to get size information");
            return;
         }
         
         version = reader.readLine();
         if (version == null) {
            System.out.println("Unable to get version information");
            return;
         }
         
         masterServer = reader.readLine();
         if (size == null) {
            System.out.println("Unable to get master hostname information");
            return;
         }
         
         System.out.println("Information on Master Server Copy");
         System.out.println("Backup size:        " + size);
         System.out.println("Version Number:     " + version);
         System.out.println("Master Server Host: " + masterServer);
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", fnct_name, exception.toString()); 
         System.out.println("Failed to read information");
      }
   }
   
   /**
    * Creates a bundle of the database for replication servers to pick up.
    *  Returns false if there are any errors.
    */
   public static boolean buildMasterDBBackupFiles()
   {      
      String fnct_name = "buildMasterDBBackupFiles";
      Connection connection = null;
      Statement statement = null;      
      File fileDescp;
      Vector archiveEntries;
      TarArchive archive;     
      FileWriter writer;
      StringBuffer masterHostname;      
      String exportDir = Constants.NTCSS_ROOT_DIR + "/tmp_db";         
      
      try {
         
         // Delete the temporary export directory if it exists
         fileDescp = new File(exportDir);
         if ((fileDescp.exists() == true) &&
             (FileOps.recursivelyDeleteDir(exportDir) == false)) {
            Log.error("ReplicationLib", fnct_name, 
                      "Error removing temporary directory <" + exportDir + ">");
            return false;
         }                  
         
         // Create the database directory under the temporary export directory
         fileDescp = new File(exportDir + "/database");
         fileDescp.mkdirs();
         
         // Get a database connection
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("ReplicationLib", fnct_name, "null connection returned");
            return false;
         }
         statement = connection.createStatement();                  
         
         // Create a list into which all the entries for the tar archive will
         //  be placed
         archiveEntries = new Vector();
         
         // Export the database tables
         archiveEntries.add(AdapterTypes.exportTable(exportDir, statement));         
         archiveEntries.add(SysConf.exportTable(exportDir, statement));
         archiveEntries.add(Version.exportTable(exportDir));     
         archiveEntries.add(AppRoles.exportTable(exportDir, statement));
         archiveEntries.add(PrinterAdapters.exportTable(exportDir, statement));
         archiveEntries.add(AppUsers.exportTable(exportDir, statement));
         archiveEntries.add(Hosts.exportTable(exportDir, statement));
         archiveEntries.add(LoginHistory.exportTable(exportDir, statement));
         archiveEntries.add(Users.exportTable(exportDir, statement));
         archiveEntries.add(OutputPrinters.exportTable(exportDir, statement));
         archiveEntries.add(OutputTypes.exportTable(exportDir, statement));
         archiveEntries.add(AppPrinters.exportTable(exportDir, statement));
         archiveEntries.add(PrinterTypes.exportTable(exportDir, statement));
         archiveEntries.add(Printers.exportTable(exportDir, statement));        
         PrinterDrivers.exportTable(archiveEntries, exportDir, statement);         
         archiveEntries.add(AppPrograms.exportTable(exportDir, statement));
         Apps.exportTable(archiveEntries, exportDir, statement);
         archiveEntries.add(PredefinedJobs.exportTable(exportDir, statement));
         
         // Export the nt driver files
         DriverFiles.exportFiles(archiveEntries);
         
         // NOTE: The following tables are not exported:
         // - any bulletin board related table
         // - current_users
         // - active_jobs, scheduled_jobs, and job_schedules
         // - print_requests
         
         // Release the system resources
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         // If the archive and the info file exist, delete them
         fileDescp = new File(NTCSS_BACKUP_TEMP_TAR_FILE);
         if (fileDescp.exists() == true);
            fileDescp.delete();         
         fileDescp = new File(NTCSS_BACKUP_TEMP_INFO_FILE);
         if (fileDescp.exists() == true);
            fileDescp.delete();
                 
         // Create the archive
         archive = new TarArchive(new FileOutputStream(NTCSS_BACKUP_TEMP_TAR_FILE));
         for (int i = 0; i < archiveEntries.size(); i++)
            archive.writeEntry((TarEntry)archiveEntries.elementAt(i), false);          
         archive.closeArchive();         
         
         // Create the info file
         writer = new FileWriter(NTCSS_BACKUP_TEMP_INFO_FILE);
         
         // Write out the archive size
         fileDescp = new File(NTCSS_BACKUP_TEMP_TAR_FILE);  
         writer.write(Long.toString(fileDescp.length()) + FIELD_SEPARATOR);
         
         // Write out the NTCSS database version
         writer.write(DataLib.getNtcssDatabaseVersion() + FIELD_SEPARATOR);
         
         // Write out the name of the master server
         masterHostname = new StringBuffer();
         DataLib.getMasterServer(masterHostname);
         writer.write(masterHostname.toString() + FIELD_SEPARATOR);
         writer.close();
         
         return true;
      }
      catch (ExportException exportException) {
         
         Log.error("ReplicationLib", fnct_name, exportException.getMessage());
         
         // Release the system resources
         try {
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
            Log.excp("ReplicationLib", fnct_name, exp.toString());             
         }
         
         return false;
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", fnct_name, exception.toString()); 
         return false;
      }
   }
   
   /** 
    * Unbundles the master server's database backup file onto the local
    * host.  This code assumes the caller has already idled or shut the daemons
    * down. If any errors occur, false is returned.
    */
   public static boolean unpackMasterFiles()
   {
      String fnct_name = "unpackMasterFiles";
      BufferedReader reader;
      String version;
      File fileDescp;
      TarArchive archive;
      Connection connection = null;
      Statement statement = null;
      String importDir = Constants.NTCSS_ROOT_DIR + "/tmp_db";
      INI iniFile;
      
      try {
         
         // Make sure the NTCSS database version specified in the info file 
         //  matches the local NTCSS database version 
         reader = new BufferedReader( new FileReader(NTCSS_BACKUP_INFO_FILE));
         reader.readLine(); // Skip the bundle size
         version = reader.readLine();
         reader.close();
         
         if (version.equals(DataLib.getNtcssDatabaseVersion()) == false) {
            Log.error("ReplicationLib", fnct_name, "Database versions are " +
                      "incompatible!");
            return false;
         }
                  
         // Delete the temporary import directory if it exists
         fileDescp = new File(importDir);
         if ((fileDescp.exists() == true) &&
             (FileOps.recursivelyDeleteDir(importDir) == false)) {
            Log.error("ReplicationLib", fnct_name, 
                      "Error removing temporary directory <" + importDir + ">");
            return false;
         }  
         
         // Unpack the database bundle to the temporary import directory
         archive = new TarArchive(new FileInputStream(NTCSS_BACKUP_TAR_FILE));
         archive.extractContents(fileDescp);
         archive.closeArchive();
                           
         // Get a database connection
         if ((connection = DatabaseConnectionManager.getConnection()) == null) {
            Log.error("ReplicationLib", fnct_name, "Could not get database connection");
            return false;
         }
         connection.setAutoCommit(false);
         statement = connection.createStatement();         
         
         // Preserve the bulletin board information
         BulletinBoards.exportTable(importDir, statement);
         BulletinBoardSubscribers.exportTable(importDir, statement);
         BulletinBoardMessages.exportTable(importDir, statement);
         BulletinBoardMessageMap.exportTable(importDir, statement);
         BulletinBoardReadHistory.exportTable(importDir, statement);

         // Remove the records from all the database tables
         clearDatabase(statement);       
         
         // Create an instance of the INI class that can be share between
         //  import methods so that the progrps.ini file doesn't have to keep
         //  being re-loaded.
         iniFile = new INI();
                                  
         // Import the database table records. The order in which these
         //  tables are imported is important. "Top level" tables should
         //  be imported first                 
         SysConf.importTable(importDir, statement); 
         Version.importTable(importDir);     
         LoginHistory.importTable(importDir, statement);         
         Hosts.importTable(importDir, statement);         
         AdapterTypes.importTable(importDir, statement);        
         PrinterDrivers.importTable(importDir, statement);                   
         PrinterTypes.importTable(importDir, statement);         
         PrinterAdapters.importTable(importDir, statement);       
         Users.importTable(importDir, statement);           
         Apps.importTable(importDir, statement, iniFile);        
         Printers.importTable(importDir, statement);                                     
         AppRoles.importTable(importDir, statement);         
         AppUsers.importTable(importDir, statement);      
         AppPrinters.importTable(importDir, statement);            
         PredefinedJobs.importTable(importDir, statement);       
         AppPrograms.importTable(importDir, statement, iniFile);           
         OutputTypes.importTable(importDir, statement);       
         OutputPrinters.importTable(importDir, statement);  
         DriverFiles.importFiles(importDir);
         
         // Restore the preserved bulletin board information
         BulletinBoards.importTable(importDir, statement); 
         BulletinBoardSubscribers.importTable(importDir, statement);
         BulletinBoardMessages.importTable(importDir, statement);
         BulletinBoardMessageMap.importTable(importDir, statement); 
         BulletinBoardReadHistory.importTable(importDir, statement);                 
                                
         // NOTE: The following tables are not imported:         
         // - current_users
         // - active_jobs, scheduled_jobs, and job_schedules
         // - print_requests            
         // - any bulletin board related table (this information is preserved
         //     from the local database; it is not part of the master bundle)

         // Close the system resources
         connection.commit();  // Commit all the changes
         statement.close();         
         DatabaseConnectionManager.releaseConnection(connection);         
         
         return true;                    
      }          
      catch (ImportException importException) {
         
         Log.error("ReplicationLib", fnct_name, importException.getMessage());
         
         // Release the system resources
         try {
            Log.info("ReplicationLib", fnct_name, "Restoring original database");
            connection.rollback();  // Rollback the changes made
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
            Log.excp("ReplicationLib", fnct_name, exp.toString());             
         }
         
         return false;
      }
      catch (ExportException exportException) {
         
         Log.error("ReplicationLib", fnct_name, exportException.getMessage());
         
         // Release the system resources
         try {
            Log.info("ReplicationLib", fnct_name, "Restoring original database");
            connection.rollback();  // Rollback the changes made
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
         }
         catch (Exception exp) {
            Log.excp("ReplicationLib", fnct_name, exp.toString());             
         }
         
         return false;
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", fnct_name, exception.toString()); 
         return false;
      }
   }
   
   /**
    * When the master backup bundle is copied over to this host, only when
    *  it has been "verified" (see verifyCopiedMasterBackupFile) correctly
    *  will be renamed to its final name (probably "master.tar"). So a check
    *  for the existence of this file is a sufficient to make sure the
    *  bundle is "ok". If it is ok (i.e. the file "master.tar" exists), 
    *  true is returned.
    */
   public static boolean isMasterBundleOk()
   {      
      File fileDescp;
      
      try {
         fileDescp = new File(NTCSS_BACKUP_TAR_FILE);
         return fileDescp.exists();         
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", "isMasterBundleOk", exception.toString()); 
         return false;
      }
   }
   
   /**
    * Makes sure that the size of DB backup matches what is specified in 
    *  the first line of the info file. If there are no errors, true is returned.    
    */
   private static boolean verifyCopiedMasterBackupFile()
   {
      String fnct_name = "verifyCopiedMasterBackupFile";
      File file, destFile;
      BufferedReader reader;
      String backup_file_sizeStr;
  
      try {
         
         // Check the existance of the info file... 
         file = new File(NTCSS_BACKUP_TEMP_INFO_FILE);
         if (file.exists() == false) {         
            Log.error("ReplicationLib", fnct_name, 
                      "Unable to stat new Master Info File " + 
                      NTCSS_BACKUP_TEMP_INFO_FILE+ "!");
            return false;
         }

         // Open the info file and get the file size for the tar file..          
         reader = new BufferedReader(new FileReader(NTCSS_BACKUP_TEMP_INFO_FILE));                           
         if((backup_file_sizeStr = reader.readLine()) == null) {         
            Log.error("ReplicationLib", fnct_name, "Unable to read file <" + 
                      NTCSS_BACKUP_TEMP_INFO_FILE + ">! File format may be " + 
                      "incorrect!");
            reader.close();
            return false;
         }

         reader.close();

         // Check the existance of the tar file... 
         file = new File(NTCSS_BACKUP_TEMP_TAR_FILE);
         if (file.exists() == false) {
            Log.error("ReplicationLib", fnct_name, "Unable to stat new " + 
                      "Master File <" + NTCSS_BACKUP_TEMP_TAR_FILE + ">!");            
            return false;
         }

         // Check the tar file's size.. 
         if (Long.parseLong(backup_file_sizeStr) != file.length())
         {
            Log.error("ReplicationLib", fnct_name, "Received Master Backup file " 
                       + NTCSS_BACKUP_TEMP_TAR_FILE + " is damaged!");
            Log.error("ReplicationLib", fnct_name, "Original size = " + 
                      backup_file_sizeStr.trim() + ", current size = " + 
                      file.length());
            return false;
         }
         
         // If the master.tar file exists, delete it
         destFile = new File(NTCSS_BACKUP_TAR_FILE);
         if (destFile.exists() == true)
            destFile.delete();

         // Rename the temp tar file to its normal name.. 
         if (file.renameTo(destFile) == false) {
            Log.error("ReplicationLib", fnct_name, "Unable to rename " + 
             "incoming Master File " + NTCSS_BACKUP_TEMP_TAR_FILE + "!");
            return false;
         }
         
         // If the master.tar file exists, delete it
         destFile = new File(NTCSS_BACKUP_INFO_FILE);
         if (destFile.exists() == true)
            destFile.delete();

         // Rename the temp info file to its normal name.. 
         file = new File(NTCSS_BACKUP_TEMP_INFO_FILE);
         if (file.renameTo(destFile) == false) {
            Log.error("ReplicationLib", fnct_name, "Unable to rename " + 
              "incoming Master Info File " +  NTCSS_BACKUP_TEMP_INFO_FILE + "!");             

            // Get rid of the good master tar file since it is useless without
            //  its accompanying tmp_info file. 
            file = new File(NTCSS_BACKUP_TAR_FILE);
            file.delete();
            return false;
         }

         return true;
      }
      catch (Exception exception) {
         Log.excp("ReplicationLib", fnct_name, exception.toString());
         return false;
      }
   }
   
   /**
    * Removes the records out of all the database tables. If any errors occur,
    *  false is returned.    
    *
    * WARNING!!! This deletion is permanent!!
    */
   private static void clearDatabase(Statement statement) throws ImportException
   {
      String fnct_name = "clearDatabase";      
      
      try {                  
                  
         // Delete the records from all the database tables. If cascade delete
         //  is on, then only the first few "top level" tables need to be 
         //  explicitly removed. Otherwise, the ordering of the table clearing
         //  is important (kind of a "bottom up" deletion approach)
         statement.executeUpdate("DELETE FROM bb_message_map");
         statement.executeUpdate("DELETE FROM bb_read_history"); 
         statement.executeUpdate("DELETE FROM user_app_roles");
         statement.executeUpdate("DELETE FROM job_schedules");
         statement.executeUpdate("DELETE FROM pdj_app_roles");
         statement.executeUpdate("DELETE FROM output_printers");
         statement.executeUpdate("DELETE FROM app_program_roles");
         statement.executeUpdate("DELETE FROM output_types");
         statement.executeUpdate("DELETE FROM bb_subscribers");
         statement.executeUpdate("DELETE FROM bb_messages");
         statement.executeUpdate("DELETE FROM current_users");
         statement.executeUpdate("DELETE FROM app_users");
         statement.executeUpdate("DELETE FROM active_jobs");
         statement.executeUpdate("DELETE FROM scheduled_jobs");
         statement.executeUpdate("DELETE FROM print_requests");
         statement.executeUpdate("DELETE FROM app_roles");
         statement.executeUpdate("DELETE FROM app_printers");
         statement.executeUpdate("DELETE FROM predefined_jobs");
         statement.executeUpdate("DELETE FROM app_programs");
         statement.executeUpdate("DELETE FROM users");
         statement.executeUpdate("DELETE FROM apps");
         statement.executeUpdate("DELETE FROM printers");
         statement.executeUpdate("DELETE FROM printer_types");
         statement.executeUpdate("DELETE FROM print_driver_files");
         statement.executeUpdate("DELETE FROM printer_adapters");
         statement.executeUpdate("DELETE FROM bulletin_boards");
         statement.executeUpdate("DELETE FROM hosts");
         statement.executeUpdate("DELETE FROM adapter_types");
         statement.executeUpdate("DELETE FROM print_drivers");
         statement.executeUpdate("DELETE FROM sys_conf");
         statement.executeUpdate("DELETE FROM login_history");                                                  
      }
      catch (Exception exception) {
         throw new ImportException("Error clearing database <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * An exception that is thrown if there was an error exporting one of the 
 *  database tables to a file
 */
class ExportException extends Exception
{
   public ExportException(String str)
   {
      super(str);
   }
}

/**
 * An exception that is thrown if there was an error importing one of the 
 *  database tables from a file
 */
class ImportException extends Exception
{
   public ImportException(String str)
   {
      super(str);
   }
}

/**
 * Containts methods that are common to tables
 */
abstract class BasicTable
{
   /**
    * Reads the SQL insert statements from the given import file and applies
    *  them to the database.
    */
   protected static void sqlImport(String importFile, 
                                   Statement statement) throws Exception
   {
      BufferedReader reader;  
      String line;
      
      // Open the "import" file for reading from it         
      reader = new BufferedReader(new FileReader(importFile));
                           
      // Read the records from the file and insert into cooresponding 
      //  database table              
      while ((line = reader.readLine()) != null)            
         statement.executeUpdate(line.trim());                
      
      // Close the import file
      reader.close(); 
   } 
   
   /**
    * Takes in a field from a record in a flat file and if there is nothing,
    *  in this field, "null" is returned.
    */ 
   protected static String checkNullableField(String field)
   {
      if ((field == null) ||
          (field.trim().length() == 0))
         return null;
      else
         return field.trim();
   }
   
   /**
    * Creates a quick lookup table of all the applications' roles
    */
   protected static Hashtable produceQuickAppRolesHash(Statement statement) 
                                               throws ImportException
   {
      Hashtable table;
      ResultSet resultSet;
      String appTitle;
      int roleNumber, roleMask;
      Vector infoHolder;
      Object info[];
      
      try {
         
         
         resultSet = statement.executeQuery("SELECT role_name, app_title, " +
                                            "role_number FROM app_roles");
         table = new Hashtable();
         while (resultSet.next() == true) {
            appTitle = resultSet.getString("app_title");
            
            if (table.containsKey(appTitle) == false) {
               
               roleNumber = resultSet.getInt("role_number");               
               roleMask = 1 << ((roleNumber - 1) + 2);
               
               info = new Object[2];
               info[0] = new Integer(roleMask);
               info[1] = resultSet.getString("role_name");
               
               infoHolder = new Vector();
               infoHolder.add(info);
               
               table.put(appTitle, infoHolder);
            }
            else {
               roleNumber = resultSet.getInt("role_number");               
               roleMask = 1 << ((roleNumber - 1) + 2);
               
               info = new Object[2];
               info[0] = new Integer(roleMask);
               info[1] = resultSet.getString("role_name");
               
               infoHolder = (Vector)table.get(appTitle);
               infoHolder.add(info);
            }
         }
         
         resultSet.close();
         
         return table;
      }
      catch (Exception exception) {
         throw new ImportException("Error creating app roles hash <" + 
                                   exception.toString() + ">");         
      }
   }   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class AdapterTypes
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir, 
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;          
      int numParallelPorts;
      int numSerialPorts;
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.ADAPTER_TYPES_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM adapter_types");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("type") + ReplicationLib.FIELD_SEPARATOR);
            
            numParallelPorts = resultSet.getInt("num_parallel_ports");
            for (int i = 1; i <= numParallelPorts; i++)
               if (i == 1)
                  writer.write("P" + i);            
               else
                  writer.write(" P" + i);
            
            numSerialPorts = resultSet.getInt("num_serial_ports");
            for (int i = 1; i <= numSerialPorts; i++)
               if ((i == 1) && (numParallelPorts == 0))
                  writer.write("S" + i);
               else
                  writer.write(" S" + i);
                  
            writer.write(ReplicationLib.FIELD_SEPARATOR);               
               
            writer.write(resultSet.getString("setup_program") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.ADAPTER_TYPES_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting adapter types <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      int numParallelPorts;
      int numSerialPorts;
      String numPortsStr;
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                           "/" + ReplicationLib.ADAPTER_TYPES_FILE));
         
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[4];
         while ((sqlItems[0] = reader.readLine()) != null) {      
            sqlItems[0] = ((String)sqlItems[0]).trim(); // type        
            numPortsStr = reader.readLine().trim();     // num ports
            sqlItems[3] = reader.readLine().trim();     // setup porgram            
            reader.readLine(); // Record terminator
            
            numParallelPorts = 0;
            numSerialPorts   = 0;
            for (int i = 0; i < numPortsStr.length(); i++) {
               if (numPortsStr.charAt(i) == 'P')
                  numParallelPorts++;
               else if (numPortsStr.charAt(i) == 'S') 
                  numSerialPorts++;
            }
            sqlItems[1] = new Integer(numParallelPorts);
            sqlItems[2] = new Integer(numSerialPorts);
                                    
            // Write record to the database table            
            statement.executeUpdate("INSERT INTO adapter_types (type, " + 
                        "num_parallel_ports, " + 
                        "num_serial_ports, setup_program) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
          }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing adapter types <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class SysConf
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir, 
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.SYS_CONF_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM sys_conf");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("tag") + ":");
            if (resultSet.getString("value") != null)  
               writer.write(" " + resultSet.getString("value"));
            writer.write(ReplicationLib.FIELD_SEPARATOR);                        
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.SYS_CONF_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting sys conf data <" + 
                                 exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];  
      String line;
      int separatorIndex;
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                           "/" + ReplicationLib.SYS_CONF_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[2];
         while ((line = reader.readLine()) != null) {        
            separatorIndex = line.indexOf(':');
            if (separatorIndex == -1)
               continue;
            
            // Get the tag and value
            sqlItems[0] = line.substring(0, separatorIndex).trim();     // tag
            if (separatorIndex + 1 >= line.length())
               sqlItems[1] = null;
            else
               sqlItems[1] = line.substring(separatorIndex + 1).trim(); // value            
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO sys_conf (tag, value) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing sys conf <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class Version
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir) throws ExportException
   {                  
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {      

         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.VERSION_FILE;
         writer = new FileWriter(exportFile);                  
         writer.write(DataLib.getNtcssDatabaseVersion());                                         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.VERSION_FILE);                
         
         return tarEntry; 
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting database version <" + 
                                 exception.toString() + ">");
      }
   }
   
   /**
    * Writes the NTCSS database version out to the flat file.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir) throws ImportException
   {                                 
      try {                                    
         
         // Really no need to read VERSION file since at this point, the database 
         //  version specified in that file has to be the same as the database
         //  version for this server or else we wouldn't be able import. So
         //  just write out the database version of this server out.
         if (DataLib.writeNtcssDatabaseVersion() == false)
            throw new Exception("Error writing version");
      }
      catch (Exception exception) {
         throw new ImportException("Error importing database version <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class AppRoles
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir, 
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.APP_ROLES_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM app_roles");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);                                                                     
            writer.write(resultSet.getString("role_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("role_number") + ReplicationLib.FIELD_SEPARATOR);
            writer.write((resultSet.getBoolean("app_lock_override") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);             
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.APP_ROLES_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting app roles <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                    "/" + ReplicationLib.APP_ROLES_FILE));
         
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[4];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // app_title
            sqlItems[1] = reader.readLine().trim();               // role_name
            sqlItems[2] = new Integer(reader.readLine().trim());  // role_number
            sqlItems[3] = new Boolean((Integer.parseInt(
                reader.readLine().trim()) == 0) ? false : true);  // app_lock_override            
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO app_roles (app_title, " + 
                "role_name, role_number, app_lock_override) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing app roles <" + 
                                   exception.toString() + ">");         
      }   
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class PrinterAdapters
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir, 
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.PRINTER_ADAPTERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM printer_adapters");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("type") + ReplicationLib.FIELD_SEPARATOR);                                                                     
            writer.write(resultSet.getString("ethernet_address") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("adapter_ip_address") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("location") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.PRINTER_ADAPTERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting printer adapters <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                          "/" + ReplicationLib.PRINTER_ADAPTERS_FILE));                  
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[4];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // type
            sqlItems[1] = reader.readLine().trim();               // ethernet_address
            sqlItems[2] = reader.readLine().trim();  // adapter_ip_address
            sqlItems[3] = reader.readLine().trim();  // location            
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO printer_adapters (type, ethernet_address, " + 
                        "adapter_ip_address, location) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing printer adapters <" + 
                                   exception.toString() + ">");         
      }
   }
   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class AppUsers extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      Hashtable appUsers;
      String key, appData;
      AppUserInfo info;
      int role_number;
      Enumeration appUsersList;      
      Object sqlItems[];
      String exportFile;
      
      try {                                                      
         
         // Get all the app users and the role_number for each role they are
         //  associated with
         resultSet = statement.executeQuery("SELECT au.login_name, " + 
            " au.app_title, custom_app_data, app_passwd, registered_user, " + 
            "role_number FROM app_users au, app_roles ar, user_app_roles " + 
            "uar WHERE au.login_name =  uar.login_name AND uar.app_title = " + 
            "ar.app_title AND uar.role_name = ar.role_name");
         appUsers = new Hashtable();
         while (resultSet.next() == true) {
            
            // Store the information in a hash table where the key is made
            //  up of the login and app title
            key = resultSet.getString("login_name") + '\3' + resultSet.getString("app_title");
            
            // If an entry doesn't already exist in the hash table for this
            //  key, insert it
            if (appUsers.containsKey(key) == false)  {
               info = new AppUserInfo();
               info.login_name = resultSet.getString("login_name");
               info.app_title = resultSet.getString("app_title");
               info.custom_app_data = resultSet.getString("custom_app_data");
               info.app_passwd = resultSet.getString("app_passwd");
               info.registered_user = resultSet.getBoolean("registered_user");
               role_number = resultSet.getInt("role_number");
               
               // Determine the corresponding bit mask for the given role_number
               //  e.g. for a role number of 1, the bit mask is 4
               //       for a role number of 2, the bit mask is 8
               //       for a role number of 3, the bit mask is 16, etc...
               if (role_number > 0)
                  info.app_roles_mask = 1 << (role_number + 1) ;
               else
                  info.app_roles_mask = 0;
               
               // Store the new information in the hash table
               appUsers.put(key, info);
            } 
            else { // Record already exists
               
               // Update the app roles mask with role number from this record
               info = (AppUserInfo)appUsers.get(key);
               info.app_roles_mask |= 1 << (resultSet.getInt("role_number") + 1);
            }
         }
         resultSet.close();         
         
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.APP_USERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out all the records stored in the hash table
         appUsersList = appUsers.elements();
         while (appUsersList.hasMoreElements() == true) {            
            info = (AppUserInfo)appUsersList.nextElement();
                                                
            writer.write(info.app_title + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.login_name + ReplicationLib.FIELD_SEPARATOR);
            writer.write("?" + ReplicationLib.FIELD_SEPARATOR);            
            if (info.custom_app_data == null)
               writer.write(ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write(info.custom_app_data + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.app_passwd + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.app_roles_mask + ReplicationLib.FIELD_SEPARATOR);
            writer.write((info.registered_user ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);
           
         }
         
         // Get all the batch user appusers and write them to the file
         sqlItems = new Object[1];
         sqlItems[0] = new Boolean(true);
         resultSet = statement.executeQuery(StringLib.SQLformat(
                        "SELECT * FROM app_users WHERE batch_user = " + 
                        Constants.ST, sqlItems));
         while (resultSet.next() == true) {
            
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("login_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write("?" + ReplicationLib.FIELD_SEPARATOR);
            appData = resultSet.getString("custom_app_data");
            if (appData == null)
               writer.write(ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write(appData + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("app_passwd") + ReplicationLib.FIELD_SEPARATOR);
            writer.write("1024" + ReplicationLib.FIELD_SEPARATOR);
            writer.write((resultSet.getBoolean("registered_user") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);
         }
         resultSet.close(); 
         
         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.APP_USERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting app users <" + 
                                   exception.toString() + ">");         
      }
   }   
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];  
      int rolesMask;
      int mask;
      Vector appRoles;
      Object info[];
      Hashtable appRoleTable;
            
      try {
             
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                  "/" + ReplicationLib.APP_USERS_FILE));  
         
         // Create a quick lookup table of all the applications' roles
         appRoleTable = produceQuickAppRolesHash(statement); 
            
         // Read the records from the file and insert into cooresponding 
         //  database table          
         sqlItems = new Object[6];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // app_title
            sqlItems[1] = reader.readLine().trim();               // login_name
            reader.readLine();                                    // old "real_name" field
            sqlItems[2] = checkNullableField(reader.readLine());  // custom_app_data
            sqlItems[3] = checkNullableField(reader.readLine());  // app_passwd
            rolesMask = Integer.parseInt(reader.readLine().trim()); // old "app_role" field
            sqlItems[4] = new Boolean((Integer.parseInt(
                reader.readLine().trim()) == 0) ? false : true);  // registered_user
            reader.readLine(); // Record terminator
            
            if ((rolesMask & 1024) == 1024)                       // batch_user
               sqlItems[5] = new Boolean(true); 
            else
               sqlItems[5] = new Boolean(false);                         
                                    
            // Write record to the database table
            statement.executeUpdate("INSERT INTO app_users (app_title, " + 
              "login_name, custom_app_data, app_passwd, registered_user, " + 
              "batch_user) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
            
            // Insert the app roles for this app user
            appRoles = (Vector)appRoleTable.get(sqlItems[0]);
            for (int i = 0; i < appRoles.size(); i++) {
               info = (Object[])appRoles.elementAt(i);
                              
               mask = ((Integer)info[0]).intValue();
               if ((rolesMask & mask) == mask) {                         
                  sqlItems[2] = (String)info[1];
                  statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                    "user_app_roles (app_title, login_name, role_name) values(" +
                    Constants.ST +"," + Constants.ST + "," + Constants.ST + ")",
                    sqlItems));
               }
            }            
         }         
         reader.close();      
         
         // Set all the batch user's users.editable_user field to false
         sqlItems[0] = new Boolean(false);
         sqlItems[1] = new Boolean(true);
         statement.executeUpdate(StringLib.SQLformat("UPDATE users SET " +
           "editable_user = " + Constants.ST + 
           " WHERE users.login_name IN (SELECT app_users.login_name FROM " +
           "app_users WHERE app_users.batch_user = " + Constants.ST + ")", sqlItems)); 
      }
      catch (Exception exception) {
         throw new ImportException("Error importing app users <" + 
                                   exception.toString() + ">");         
      }
   }     
   
   /**
    * A data structure for holding information about an app user
    */
   static class AppUserInfo
   {      
      public String login_name;
      public String app_title;
      public String custom_app_data;
      public String app_passwd;
      public boolean registered_user;
      public int app_roles_mask;                 
   }   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class Hosts
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.HOSTS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM hosts");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("hostname") + ReplicationLib.FIELD_SEPARATOR);                                                                     
            writer.write(resultSet.getString("host_ip_address") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("max_running_jobs") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("type") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write((resultSet.getBoolean("replication_ind") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.HOSTS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting hosts <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                           "/" + ReplicationLib.HOSTS_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[5];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // hostname
            sqlItems[1] = reader.readLine().trim();               // IP
            sqlItems[2] = new Integer(reader.readLine().trim());  // max num jobs
            sqlItems[3] = new Integer(reader.readLine().trim());  // type
            sqlItems[4] = reader.readLine().trim();               // replication
            reader.readLine(); // Record terminator
            
            if (((String)sqlItems[4]).equals("1") == true)
               sqlItems[4] = new Boolean(true);
            else
               sqlItems[4] = new Boolean(false);
                                    
            // Write record to the database table
            statement.executeUpdate("INSERT INTO hosts (hostname, host_ip_address, " + 
                        "max_running_jobs, type, replication_ind) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing hosts <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class LoginHistory
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.LOGIN_HISTORY_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM login_history");
         while (resultSet.next() == true) {
            writer.write(resultSet.getString("login_time") + ReplicationLib.FIELD_SEPARATOR);                                                                     
            writer.write(resultSet.getInt("status") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("client_address") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("login_name") + ReplicationLib.FIELD_SEPARATOR);                        
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.LOGIN_HISTORY_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting login history <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                     "/" + ReplicationLib.LOGIN_HISTORY_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[5];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // login_time
            sqlItems[1] = new Integer(reader.readLine().trim());  // status
            sqlItems[2] = reader.readLine().trim();               // client_address
            sqlItems[3] = reader.readLine().trim();               // login_name
            sqlItems[4] = null;                                   // auth_server
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO login_history (login_time, status, " + 
                        "client_address, login_name, auth_server) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing login history <" + 
                                   exception.toString() + ">");         
      }
   }

}

/**
 * Encapsulates operations on the corresponding database table.
 */
class Users
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.USERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM users");
         while (resultSet.next() == true) {
            writer.write(resultSet.getInt("unix_id") + ReplicationLib.FIELD_SEPARATOR);          
            writer.write(resultSet.getString("login_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("real_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("password") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("ss_number") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("phone_number") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("security_class") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("pw_change_time") + ReplicationLib.FIELD_SEPARATOR);
            writer.write((resultSet.getBoolean("ntcss_super_user") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write((resultSet.getBoolean("login_lock") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write(resultSet.getString("shared_db_passwd") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("auth_server") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.USERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting users <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];  
      Integer loginLock;
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                           "/" + ReplicationLib.USERS_FILE));
         
        // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[14];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = new Integer(((String)sqlItems[0]).trim()); // unix_id
            sqlItems[1] = reader.readLine().trim();                  // login_name
            sqlItems[2] = reader.readLine().trim();                  // real_name
            sqlItems[3] = reader.readLine().trim();                  // password
            sqlItems[4] = reader.readLine().trim();                  // ss_number
            sqlItems[5] = reader.readLine();                         // phone_number
            if ((sqlItems[5] == null) || 
                (((String)sqlItems[5]).trim().length() == 0))
               sqlItems[5] = null;
            else
               sqlItems[5] = ((String)sqlItems[5]).trim();
            sqlItems[6] = new Integer(reader.readLine().trim());     // security_class
            sqlItems[7] = reader.readLine().trim();                  // pw_change_time  
            reader.readLine(); // The information in this field isn't used
            sqlItems[8] = new Boolean(
              Constants.BASE_NTCSS_USER.equals((String)sqlItems[1])); // ntcss_super_user            
            loginLock = new Integer(reader.readLine().trim());        // login_lock
            if (loginLock.intValue() == 0)
               sqlItems[9] = new Boolean(false);
            else
               sqlItems[9] = new Boolean(true);
            sqlItems[10] = reader.readLine();
            if ((sqlItems[10] == null) ||                            // shared_db_passwd
                (((String)sqlItems[10]).trim().length() == 0))
               sqlItems[10] = null;
            else
               sqlItems[10] = ((String)sqlItems[10]).trim();
            sqlItems[11] = reader.readLine().trim();               // auth_server
            sqlItems[12] = sqlItems[1];                            // long_login_name
            sqlItems[13] = new Boolean(true);                      // editable_user            
            reader.readLine(); // Record terminator
            
            // NOTE: When the app_users table is imported, the users.editable_user
            //       field will get updated for batch users
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO users (unix_id, login_name, " + 
                        "real_name, password, ss_number, phone_number, " + 
                        "security_class, pw_change_time, ntcss_super_user, " +
                        "login_lock, shared_db_passwd, auth_server, " +
                        "long_login_name, editable_user) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing users <" + 
                                   exception.toString() + ">");         
      }
   }
   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class OutputPrinters extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.OUTPUT_PRINTERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM output_printers");
         while (resultSet.next() == true) {                    
            writer.write(resultSet.getString("otype_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("printer_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("printer_name") + ReplicationLib.FIELD_SEPARATOR);  // This field isn't used any more
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.OUTPUT_PRINTERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting output printers <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                 "/" + ReplicationLib.OUTPUT_PRINTERS_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[5];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim(); // otype_title
            sqlItems[1] = reader.readLine().trim();     // app_title
            sqlItems[2] = reader.readLine().trim();     // printer_name
            reader.readLine();                          // old "hostname" field
            reader.readLine(); // Record terminator
            sqlItems[3] = new Boolean(false);
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO output_printers (otype_title, " +
               "app_title, printer_name, default_printer) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," +                    
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                       
         
         
         // Get the default printer info
         reader = new BufferedReader(new FileReader(importDir + 
                                 "/" + ReplicationLib.OUTPUT_TYPES_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table          
         while ((sqlItems[0] = reader.readLine()) != null) {                          
            sqlItems[2] = ((String)sqlItems[0]).trim();           // otype_title            
            
            sqlItems[1] = new Boolean(true);                  
            sqlItems[3] = reader.readLine().trim();               // app_title
            reader.readLine(); // Skip the description            
            sqlItems[4] = checkNullableField(reader.readLine());  // default_printer            
            reader.readLine(); // Skip the batch_flag
            reader.readLine(); // Record terminator
            sqlItems[0] = sqlItems[4];

            // Update record in the database table
            // NOTE: because of a parsing error in Mckoi engine where it doesn't like
            //  a field name starting with "default" directly after the SET,
            //  the printer_name field was added just to prevent this error.
            if (sqlItems[4] != null) {
               statement.executeUpdate(StringLib.SQLformat("UPDATE " +
                  "output_printers SET printer_name = " + Constants.ST + 
                  ", default_printer = " + Constants.ST + 
                  " WHERE otype_title = " + Constants.ST + " AND app_title = " + 
                  Constants.ST + " AND printer_name = " + Constants.ST, sqlItems));
            }
         }         
         reader.close();                             

      }
      catch (Exception exception) {
         throw new ImportException("Error importing output printers <" + 
                                   exception.toString() + ">");         
      }
   }      
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class OutputTypes extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Statement statement2;
      ResultSet resultSet2;
      Object sqlItems[] = new Object[3];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.OUTPUT_TYPES_FILE;
         writer = new FileWriter(exportFile);
         
         statement2 = (statement.getConnection()).createStatement();
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM output_types");
         while (resultSet.next() == true) {                    
            writer.write(resultSet.getString("otype_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("description") + ReplicationLib.FIELD_SEPARATOR);
            
            // Get the output_type's default printer
            sqlItems[0] = resultSet.getString("otype_title");
            sqlItems[1] = resultSet.getString("app_title");
            sqlItems[2] = new Boolean(true);
            resultSet2 = statement2.executeQuery(StringLib.SQLformat("SELECT " +
                  "printer_name FROM output_printers WHERE otype_title = " + 
                  Constants.ST + " AND app_title = " + Constants.ST + 
                  " AND default_printer = " + Constants.ST, sqlItems));
            if (resultSet2.next() == false)
               writer.write("" + ReplicationLib.FIELD_SEPARATOR);
            else            
               writer.write(resultSet2.getString("printer_name") + ReplicationLib.FIELD_SEPARATOR);
            resultSet2.close();
            
            writer.write((resultSet.getBoolean("batch_flag") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();           
         statement2.close();
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.OUTPUT_TYPES_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting output types <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                 "/" + ReplicationLib.OUTPUT_TYPES_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[4];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // otype_title
            sqlItems[1] = reader.readLine().trim();               // app_title
            sqlItems[2] = checkNullableField(reader.readLine());  // description
//            sqlItems[3] = checkNullableField(reader.readLine());  // default_printer
            reader.readLine(); // Skip the default_printer field
            sqlItems[3] = new Boolean(
               (Integer.parseInt(reader.readLine().trim()) == 0) ? false : true);  // batch_flag
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO output_types (otype_title, " +
               "app_title, description, batch_flag) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing output types <" + 
                                   exception.toString() + ">");         
      }
   }       
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class AppPrinters
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.APP_PRINTERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM app_printers");
         while (resultSet.next() == true) {                    
            writer.write(resultSet.getString("printer_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write("NODATA" + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.APP_PRINTERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting app printers <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                             "/" + ReplicationLib.APP_PRINTERS_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[2];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim(); // printer_name
            reader.readLine();                          // old "hostname" field
            sqlItems[1] = reader.readLine().trim();     // app_title
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO app_printers (printer_name, " +
              "app_title) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing app printers <" + 
                                   exception.toString() + ">");         
      }
   }   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class PrinterTypes
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.PRINTER_TYPES_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out to this file the printer types (these are probably
         //  unix printer types)
         resultSet = statement.executeQuery("SELECT * FROM printer_types");
         while (resultSet.next() == true) {                    
            writer.write(resultSet.getString("printer_type") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("model") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("driver_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.PRINTER_TYPES_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting printer types <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                     "/" + ReplicationLib.PRINTER_TYPES_FILE));
                                    
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[3];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim(); // printer_type
            sqlItems[1] = reader.readLine().trim();     // model
            sqlItems[2] = reader.readLine().trim();     // driver_name
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO printer_types (printer_type, model, " + 
                        "driver_name) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");
         }         
         reader.close();                             
      }
      catch (Exception exception) {
         throw new ImportException("Error importing printer_types <" + 
                                   exception.toString() + ">");         
      }
   }
}
   
/**
 * Encapsulates operations on the corresponding database table.
 */
class Printers extends BasicTable
{   
   private static final String NO_PRINTER_TYPE = "NOCLASS";
   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String printMethod;
      String ipAddress, portName, filename, redirectionPrinter;
      int status;    
      String exportFile;
      String printerType;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.PRINTERS_FILE;
         writer = new FileWriter(exportFile);         
   
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM printers");
         while (resultSet.next() == true) {                    
            
            // Get the printing method
            printMethod = resultSet.getString("printing_method");
            if (printMethod.equals(Constants.UNIX_PRINTER) == true) {
               ipAddress = resultSet.getString("host_ip_address"); 
               portName  = resultSet.getString("port_name"); 
               filename  = "None"; 
            }
            else if (printMethod.equals(Constants.NT_SHARE_PRINTER) == true) { 
               ipAddress = resultSet.getString("nt_host_name"); 
               portName  = resultSet.getString("nt_share_name"); 
               filename  = "None";
            }
            else if (printMethod.equals(Constants.REGULAR_PRINTER) == true) { 
               ipAddress = resultSet.getString("adapter_ip_address"); 
               portName  = resultSet.getString("port_name"); 
               filename  = "None";
            }
            else if (printMethod.equals(Constants.FILE_PRINTER) == true) { 
               ipAddress = "None";
               portName  = "FILE";
               filename  = resultSet.getString("file_name"); 
            }
            else {
               Log.error("Printers", "export", "Unknown printing method <" + 
                         printMethod + ">");
               continue;
            }
            
            status = 0;
            if (resultSet.getBoolean("queue_enabled") == true)
               status |= 0x01;
            if (resultSet.getBoolean("printer_enabled") == true)
               status |= 0x02;
            if (resultSet.getBoolean("general_access") == true)
               status |= 0x04;            
            if (resultSet.getBoolean("file_append_flag")== true) // NOTE: If this field is null in the DB,
               status |= 0x08;                                   //   false is returned.
            
            // Check to see if the printer_type field is null
            if ((printerType = resultSet.getString("printer_type")) == null)
               printerType = NO_PRINTER_TYPE;
            
            writer.write(resultSet.getString("printer_name") + ReplicationLib.FIELD_SEPARATOR);
            writer.write("NODATA" + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("location") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("job_max_bytes") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(printerType + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ipAddress + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("security_class") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write(status + ReplicationLib.FIELD_SEPARATOR);
            writer.write(portName + ReplicationLib.FIELD_SEPARATOR);
            writer.write((resultSet.getBoolean("suspend_requests") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);
            redirectionPrinter = resultSet.getString("redirection_ptr");
            if (redirectionPrinter != null)
               writer.write(redirectionPrinter + ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write("NONE" + ReplicationLib.FIELD_SEPARATOR);
            writer.write(filename + ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.PRINTERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting printers <" + 
                                   exception.toString() + ">");         
      }
   }  
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      String oldIPaddress, oldPortName, oldFilename;
      int oldStatus;
      ResultSet resultSet;
      String iniFilename;
      INI iniFile;
      String printers;
      StringTokenizer printersTokenizer, tmpTokenizer;
      
      try {
                           
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                               "/" + ReplicationLib.PRINTERS_FILE));
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[19];
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();           // printer_name                        
            reader.readLine();                                    // old "hostname" field
            sqlItems[1] = reader.readLine().trim();               // location
            sqlItems[2] = new Integer(reader.readLine().trim());  // job_max_bytes
            sqlItems[3] = reader.readLine().trim();               // printer_type
            oldIPaddress = reader.readLine().trim();              // old "ip_address" field
            sqlItems[4] = new Integer(reader.readLine().trim());  // security_class
            oldStatus = Integer.parseInt(reader.readLine().trim()); // old "status" field
            oldPortName = reader.readLine().trim();               // old "port_name" field
            sqlItems[5] = new Boolean((Integer.parseInt(
                 reader.readLine().trim()) == 0) ? false : true); // suspend_flag
            sqlItems[6] = checkNullableField(reader.readLine());  // redirection_ptr
            oldFilename = reader.readLine().trim();               // old "filename" field
            reader.readLine(); // Record terminator                        
            
            if ((oldStatus & 0x01) == 0x01) // queue_enabled
               sqlItems[7] = new Boolean(true);
            else
               sqlItems[7] = new Boolean(false);
               
            if ((oldStatus & 0x02) == 0x02) // printer_enabled
               sqlItems[8] = new Boolean(true);
            else
               sqlItems[8] = new Boolean(false);
               
            if ((oldStatus & 0x04) == 0x04) // general_access
               sqlItems[9] = new Boolean(true);
            else
               sqlItems[9] = new Boolean(false);
               
            if ((oldStatus & 0x08) == 0x08) // file_append_flag
               sqlItems[10] = new Boolean(true);
            else
               sqlItems[10] = new Boolean(false);                           
            
            // Check to see what type of printing method is associated with
            //  this printer
            if ((oldIPaddress.equalsIgnoreCase("None") == true) &&
                (oldPortName.equalsIgnoreCase("FILE") == true)) {               
               sqlItems[11] = Constants.FILE_PRINTER; // printing_method
               sqlItems[12] = null;                   // host_ip_address                                    
               sqlItems[13] = null;                   // port_name
               sqlItems[14] = oldFilename;            // file_name
               sqlItems[15] = null;                   // adapter_ip_address            
               sqlItems[16] = null;                   // nt_host_name
               sqlItems[17] = null;                   // nt_share_name
            }
            else {
               resultSet = statement.executeQuery("SELECT * FROM hosts " + 
                               "WHERE host_ip_address = '" + oldIPaddress + "'");
               if (resultSet.next() == true) {
                  resultSet.close();
                  sqlItems[11] = Constants.UNIX_PRINTER; // printing_method
                  sqlItems[12] = oldIPaddress;           // host_ip_address                                    
                  sqlItems[13] = oldPortName;            // port_name
                  sqlItems[14] = null;                   // file_name
                  sqlItems[15] = null;                   // adapter_ip_address            
                  sqlItems[16] = null;                   // nt_host_name
                  sqlItems[17] = null;                   // nt_share_name                  
               }
               else {
                  resultSet.close();
                  resultSet = statement.executeQuery("SELECT * FROM printer_adapters " + 
                               "WHERE adapter_ip_address = '" + oldIPaddress + "'");
                  if (resultSet.next() == true) {
                     resultSet.close();
                     sqlItems[11] = Constants.REGULAR_PRINTER; // printing_method
                     sqlItems[12] = null;                      // host_ip_address                                    
                     sqlItems[13] = oldPortName;               // port_name
                     sqlItems[14] = null;                      // file_name
                     sqlItems[15] = oldIPaddress;              // adapter_ip_address            
                     sqlItems[16] = null;                      // nt_host_name
                     sqlItems[17] = null;                      // nt_share_name                     
                  }
                  else {
                     resultSet.close();
                     sqlItems[11] = Constants.NT_SHARE_PRINTER; // printing_method
                     sqlItems[12] = null;                       // host_ip_address                                    
                     sqlItems[13] = null;                       // port_name
                     sqlItems[14] = null;                       // file_name
                     sqlItems[15] = null;                       // adapter_ip_address            
                     sqlItems[16] = oldIPaddress;               // nt_host_name
                     sqlItems[17] = oldPortName;                // nt_share_name
                  }
               }
            }
            
            // Check the printer_type field
            if (((String)sqlItems[3]).equals(NO_PRINTER_TYPE) == true)
               sqlItems[3] = null;
            
            sqlItems[18] = null; // driver_name
                        
            // Write record to the database table
            statement.executeUpdate("INSERT INTO printers (printer_name, location, "
                + "job_max_bytes, printer_type, security_class, suspend_requests, "
                + "redirection_ptr, queue_enabled, printer_enabled, general_access, "
                + "file_append_flag, printing_method, host_ip_address, port_name, "
                + "file_name, adapter_ip_address, nt_host_name, nt_share_name, " 
                + "driver_name) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," +
                   Constants.ST, sqlItems) + ")");
         }         
         reader.close(); 
         
         // Loop through the "DRIVERLINK" section of the ntprint.ini file
         //  to fill in the printers.driver_name fields                 
         
         // Open the ntprint.ini file for reading from it         
         iniFilename = importDir + "/" + ReplicationLib.PRINTER_DRIVERS_FILE;
         iniFile = new INI(); 
         
         if ((printers = iniFile.GetProfileSection("DRIVERLINK", 
                                                       iniFilename)) != null) {
            printersTokenizer = new StringTokenizer(printers, 
                                                   INI.STRINGS_DELIMITER);
            
            // Loop through each of the entries in this section 
            while (printersTokenizer.hasMoreTokens() == true) {
               tmpTokenizer = new StringTokenizer(printersTokenizer.nextToken(), 
                                                  INI.TAG_VALUE_DELIMITER);
               
               // Insert the entry into the printers table
               sqlItems[1] = tmpTokenizer.nextToken();  // printer name
               sqlItems[0] = tmpTokenizer.nextToken();  // driver name  
               
               // Put in its own try catch because don't want hole operation
               //  to fail just because there was an error in the ntprint.ini
               //  file
               try {
                  statement.executeUpdate(StringLib.SQLformat("UPDATE printers " + 
                    "SET driver_name = " + Constants.ST + " WHERE printer_name = " 
                    + Constants.ST, sqlItems));                         
               }
               catch (Exception ex) {}
            }
         }
      }
      catch (Exception exception) {
         throw new ImportException("Error importing printers <" + 
                                   exception.toString() + ">");         
      }
   }   
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class PrinterDrivers
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static void exportTable(Vector archiveEntries, String exportDir,
                                  Statement statement) throws ExportException
   {                     
      TarEntry tarEntry;
      String iniFilename;
      StringBuffer exceptionBuffer;
      File file;
      
      // Create the "export" file for writing to it         
      iniFilename = exportDir + "/" + 
                    ReplicationLib.PRINTER_DRIVERS_FILE;
      
      // Attempt to export the data to the file
      exceptionBuffer = new StringBuffer();
      if (PrintLib.exportPrtDrivers(iniFilename, exceptionBuffer) == false)
         throw new ExportException("Error exporting printer drivers <" + 
                                   exceptionBuffer.toString() + ">");
      
      // Check to see if the INI file was created. If it wasn't then there
      //  probably wasn't any driver information in the database. 
      file = new File(iniFilename);
      if (file.exists() == true) {
      
         // Create a tar entry for this file and add it to the list of entries
         try {
         
            // Create the tar entry
            tarEntry = new TarEntry(new File(iniFilename));
            // The "setName" method insures that the archive entry will have
            //  a relative path not an absolute path specified which is 
            //  necessary for proper extraction from the tar archive.
            tarEntry.setName(ReplicationLib.PRINTER_DRIVERS_FILE);   
         
            archiveEntries.add(tarEntry);                  
         }
         catch (Exception exception) {
            throw new ExportException("Error exporting printer drivers <" + 
                                      exception.toString() + ">");
         }     
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                           
      Object sqlItems[];      
      INI iniFile;
      String iniFilename;
      String drivers;
      StringTokenizer driversTokenizer, sectionTokenizer, printerTypesTokenizer;
      StringTokenizer driverNameTokenizer, tmpTokenizer;
      String driverName;
      String info;
      String error = "NOFTFOUNDHERE";
      String printerTypes;
      
      try {
                           
         // Open the "import" file for reading from it         
         iniFilename = importDir + "/" + ReplicationLib.PRINTER_DRIVERS_FILE;
         iniFile = new INI();         
                     
         sqlItems = new Object[3];
         
         // Loop through all the entries in the "PRINTERS" section
         if ((drivers = iniFile.GetProfileSection("PRINTERS", 
                                                  iniFilename)) != null) {
            
            driversTokenizer = new StringTokenizer(drivers, INI.STRINGS_DELIMITER);
            while (driversTokenizer.hasMoreTokens() == true) {
               driverNameTokenizer = new StringTokenizer(driversTokenizer.nextToken(), 
                                                      INI.TAG_VALUE_DELIMITER);
               driverName = driverNameTokenizer.nextToken();
            
               // Insert the record in the print_drivers table
               info = iniFile.GetProfileString(driverName, "MONITORNAME", 
                                               error, iniFilename);            
               sqlItems[0] = driverName;
               if (info.equals(error) == true)
                  sqlItems[1] = null;
               else
                  sqlItems[1] = info;
               statement.executeUpdate("INSERT INTO print_drivers (driver_name, " + 
                        "nt_monitor_name) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
            
               // Get the "DRIVER" driver filename associated with this driver
               info = iniFile.GetProfileString(driverName, "DRIVER", 
                                            error, iniFilename);            
               if (info.equals(error) == false) {
                  sqlItems[0] = driverName;
                  sqlItems[1] = new Integer(Constants.DRIVER_DRIVER_TYPE);
                  sqlItems[2] = info;
                  statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                        "type, driver_filename) values(" + 
                     StringLib.SQLformat(
                      Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
               }
               
               // Get the "UI" driver filename associated with this driver
               info = iniFile.GetProfileString(driverName, "UI", 
                                            error, iniFilename);            
               if (info.equals(error) == false) {
                  sqlItems[0] = driverName;
                  sqlItems[1] = new Integer(Constants.UI_DRIVER_TYPE);
                  sqlItems[2] = info;
                  statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                        "type, driver_filename) values(" + 
                     StringLib.SQLformat(
                     Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
               }
            
               // Get the "DATA" driver filename associated with this driver
               info = iniFile.GetProfileString(driverName, "DATA", 
                                               error, iniFilename);            
               if (info.equals(error) == false) {
                  sqlItems[0] = driverName;
                  sqlItems[1] = new Integer(Constants.DATA_DRIVER_TYPE);
                  sqlItems[2] = info;
                  statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                        "type, driver_filename) values(" + 
                     StringLib.SQLformat(
                     Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
               }
            
               // Get the "HELP" driver filename associated with this driver
               info = iniFile.GetProfileString(driverName, "HELP", 
                                               error, iniFilename);            
               if (info.equals(error) == false) {
                  sqlItems[0] = driverName;
                  sqlItems[1] = new Integer(Constants.HELP_DRIVER_TYPE);
                  sqlItems[2] = info;
                  statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                        "type, driver_filename) values(" + 
                    StringLib.SQLformat(
                     Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
               }
            
               // Get the "MONITOR" driver filename associated with this driver
               info = iniFile.GetProfileString(driverName, "MONITOR", 
                                               error, iniFilename);            
               if (info.equals(error) == false) {
                  sqlItems[0] = driverName;
                  sqlItems[1] = new Integer(Constants.MONITOR_DRIVER_TYPE);
                  sqlItems[2] = info;
                  statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                        "type, driver_filename) values(" + 
                    StringLib.SQLformat(
                    Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
               }
            
               // Get any "copy" driver filenames associated with this driver
               for (int i = 1; ; i++) {
                  info = iniFile.GetProfileString(driverName, 
                                            Constants.COPY_FILE_PREFIX + "_" + i, 
                                            error, iniFilename);            
                  if (info.equals(error) == false) {                          
                     sqlItems[0] = driverName;
                     sqlItems[1] = new Integer(Constants.NO_DRIVER_TYPE);
                     sqlItems[2] = info;
                     statement.executeUpdate("INSERT INTO print_driver_files (driver_name, " + 
                           "type, driver_filename) values(" + 
                       StringLib.SQLformat(
                       Constants.ST + "," + Constants.ST + "," + Constants.ST, sqlItems) + ")"); 
                  }
                  else
                     break;
               }
            } // End while loop
         }
         
         // The "DRIVERLINK" section will be read when the printers information
         //  is imported
      }
      catch (Exception exception) {
         throw new ImportException("Error importing print drivers <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class AppPrograms extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String field;
      String exportFile;
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.APP_PROGRAMS_FILE;
         writer = new FileWriter(exportFile);
         
/*       Since the programs table is not currently used in the flat file
         database (version 3.0.5), no need to export the table contents
 
      
         // Write out the table's contents to the file
         resultSet = statement.executeQuery("SELECT * FROM app_programs");
         while (resultSet.next() == true) {                    
            writer.write(resultSet.getString("program_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getString("program_file") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("icon_file");
            if (field != null)
               writer.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write(ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("icon_index") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("cmd_line_args");
            if (field != null)
               writer.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write(ReplicationLib.FIELD_SEPARATOR);
            writer.write(resultSet.getInt("security_class") + ReplicationLib.FIELD_SEPARATOR);            
            writer.write("0" + ReplicationLib.FIELD_SEPARATOR);  // "program_access" in the old table
            writer.write(resultSet.getInt("program_type") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("working_dir");
            if (field != null)
               writer.write(field + ReplicationLib.FIELD_SEPARATOR);            
            else
               writer.write(ReplicationLib.FIELD_SEPARATOR);            
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();   
 */      
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.APP_PROGRAMS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting application programs <" + 
                                   exception.toString() + ">");         
      }
   }
   
   // maybe need to read from ini
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement,
                                  INI iniFile) throws ImportException
   {                                  
      Object sqlItems[];      
      String appsList, programsList;
      StringTokenizer appsTokenizer, programsTokenizer, 
                      tmpTokenizer, rolesTokenizer;
      String appTitle, programSection;
      String programTitle, programFile, iconFile, iconIndex, cmdLineArgs,
             securityClass, programType, workingDir, access;
      String error = "NOTFOUNDHERE";
      
      try {
         
         // Get a list of all the applications from the progrps.ini file
         if ((appsList = iniFile.GetProfileSection("APPS", 
                                  Constants.NTCSS_PROGRP_INIT_FILE)) == null)
            throw new ImportException("Could not find APPS section in " + 
                                       Constants.NTCSS_PROGRP_INIT_FILE);
         
         // Loop through each of the applications adding their programs
         sqlItems = new Object[9];
         appsTokenizer = new StringTokenizer(appsList, INI.STRINGS_DELIMITER);
         while (appsTokenizer.hasMoreTokens() == true) {
            tmpTokenizer = new StringTokenizer(appsTokenizer.nextToken(), 
                                               INI.TAG_VALUE_DELIMITER);
            appTitle = tmpTokenizer.nextToken();
            
            // Get a list of this application's programs
            if ((programsList = iniFile.GetProfileSection(appTitle + "_PROG",
                                    Constants.NTCSS_PROGRP_INIT_FILE)) == null)
               continue;
            
            // Loop through each of this application's programs
            programsTokenizer = new StringTokenizer(programsList, 
                                                    INI.STRINGS_DELIMITER);
            while (programsTokenizer.hasMoreTokens() == true) {               
               tmpTokenizer = new StringTokenizer(programsTokenizer.nextToken(), 
                                                  INI.TAG_VALUE_DELIMITER);
               programSection = tmpTokenizer.nextToken();
               
               // Get the information about this program
               programTitle = iniFile.GetProfileString(programSection, 
                           "PROGRAM", error, Constants.NTCSS_PROGRP_INIT_FILE);
               // This program may be list under the app's program list, but
               //  it may not have a program section for this program
               if (programTitle.equals(error) == true) 
                  continue;
               
               programFile = iniFile.GetProfileString(programSection, 
                           "EXEFILE", error, Constants.NTCSS_PROGRP_INIT_FILE);               
               iconFile = iniFile.GetProfileString(programSection, 
                           "ICONFILE", error, Constants.NTCSS_PROGRP_INIT_FILE);
               iconIndex = iniFile.GetProfileString(programSection, 
                           "ICONINDEX", error, Constants.NTCSS_PROGRP_INIT_FILE);
               cmdLineArgs = iniFile.GetProfileString(programSection, 
                           "COMMANDLINE", error, Constants.NTCSS_PROGRP_INIT_FILE);
               securityClass = iniFile.GetProfileString(programSection, 
                           "SECURITYCLASS", error, Constants.NTCSS_PROGRP_INIT_FILE);               
               access = iniFile.GetProfileString(programSection, 
                           "ACCESS", error, Constants.NTCSS_PROGRP_INIT_FILE);
               programType = iniFile.GetProfileString(programSection, 
                           "PROGRAMTYPE", error, Constants.NTCSS_PROGRP_INIT_FILE);
               workingDir = iniFile.GetProfileString(programSection, 
                           "WORKINGDIR", error, Constants.NTCSS_PROGRP_INIT_FILE);
                              
               // Write record to the database table   
               sqlItems[0] = programTitle.trim();
               sqlItems[1] = appTitle.trim();
               sqlItems[2] = programFile.trim();
               sqlItems[3] = checkNullableField(iconFile);
               sqlItems[4] = new Integer(iconIndex.trim());
               sqlItems[5] = checkNullableField(cmdLineArgs);
               sqlItems[6] = new Integer(
                             Constants.getSecurityCode(securityClass.trim()));
               if ((programType != null) && (programType.trim().length() > 0))
                  sqlItems[7] = new Integer(
                                  Constants.getProgramType(programType.trim()));
               else
                  sqlItems[7] = null;
               sqlItems[8] = checkNullableField(workingDir);                             
               statement.executeUpdate("INSERT INTO app_programs " + 
                    "(program_title, app_title, program_file, icon_file, " + 
                    "icon_index, cmd_line_args, security_class, program_type, " +
                    "working_dir) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");        
           
               // Add the program's roles
               rolesTokenizer = new StringTokenizer(access, ",");
               while (rolesTokenizer.hasMoreTokens() == true) {
                  
                  // Catch SQL exceptions here just incase the progrps.ini
                  //  file errantly has referential integrity violations
                  try {
                     sqlItems[2] = rolesTokenizer.nextToken().trim();
                     statement.executeUpdate("INSERT INTO app_program_roles " + 
                          "(program_title, app_title, role_name) values(" + 
                        StringLib.SQLformat(
                          Constants.ST + "," + Constants.ST + "," + 
                          Constants.ST, sqlItems) + ")");   
                  }
                  catch (Exception exp) {
                     Log.error("AppPrograms", "importTable", exp.toString());
                  }
               }
               
            }  // End app's programs list loop
         }  // End app list loop                                                                                                    
      }
      catch (Exception exception) {
         throw new ImportException("Error importing application programs <" + 
                                   exception.toString() + ">");         
      }
   }           
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class Apps extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise tar 
    *  entry objects are added to the given list of tar archive entries.
    */
   public static void exportTable(Vector archiveEntries, String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter appsWriter;    
      FileWriter currentAppsWriter;
      TarEntry tarEntry;
      String field;
      String appsExportFile;
      String currentAppsExportFile;
      
      try {
                           
         // Open the "export" files for writing to them
         appsExportFile = exportDir + "/" + ReplicationLib.APPS_FILE;         
         appsWriter = new FileWriter(appsExportFile);
         currentAppsExportFile = exportDir + "/" + ReplicationLib.CURRENT_APPS_FILE;
         currentAppsWriter = new FileWriter(currentAppsExportFile);
         
         // Write out the table's contents to the files
         resultSet = statement.executeQuery("SELECT * FROM apps");
         while (resultSet.next() == true) {                    
            appsWriter.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            appsWriter.write(resultSet.getString("hostname") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("icon_file");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);
            appsWriter.write(resultSet.getInt("icon_index") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("version_number");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("release_date");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("client_location");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("server_location");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);
            appsWriter.write(resultSet.getString("unix_group") + ReplicationLib.FIELD_SEPARATOR);
            appsWriter.write(resultSet.getInt("max_running_app_jobs") + ReplicationLib.FIELD_SEPARATOR);
            field = resultSet.getString("link_data");
            if (field != null)
               appsWriter.write(field + ReplicationLib.FIELD_SEPARATOR);                        
            else
               appsWriter.write(ReplicationLib.FIELD_SEPARATOR);                        
            appsWriter.write(ReplicationLib.FIELD_SEPARATOR);   
            
            currentAppsWriter.write(resultSet.getString("app_title") + ReplicationLib.FIELD_SEPARATOR);
            currentAppsWriter.write(resultSet.getString("hostname") + ReplicationLib.FIELD_SEPARATOR);
            currentAppsWriter.write("0" + ReplicationLib.FIELD_SEPARATOR); // number users
            currentAppsWriter.write((resultSet.getBoolean("app_lock") ? "1" : "0") + ReplicationLib.FIELD_SEPARATOR);
            currentAppsWriter.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         appsWriter.close();
         currentAppsWriter.close();
         
         // Create the tar entry for the applications data
         tarEntry = new TarEntry(new File(appsExportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.APPS_FILE);                
         
         archiveEntries.add(tarEntry);
         
         // Create the tar entry for the current applications data
         tarEntry = new TarEntry(new File(currentAppsExportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.CURRENT_APPS_FILE);                
         
         archiveEntries.add(tarEntry);
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting applications <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement,
                                  INI iniFile) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];            
      String error = "NOTFOUNDHERE";
      String field;
      
      try {
         
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                                           "/" + ReplicationLib.APPS_FILE));                                                      
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[15];
         while ((sqlItems[0] = reader.readLine()) != null) {   
            
            sqlItems[0] = ((String)sqlItems[0]).trim();           // app_title
            sqlItems[1] = reader.readLine().trim();               // hostname
            sqlItems[2] = checkNullableField(reader.readLine());  // icon_file
            sqlItems[3] = new Integer(reader.readLine().trim());  // icon_index
            sqlItems[4] = checkNullableField(reader.readLine());  // version_number
            sqlItems[5] = checkNullableField(reader.readLine());  // release_date
            sqlItems[6] = checkNullableField(reader.readLine());  // client_location
            sqlItems[7] = checkNullableField(reader.readLine());  // server_location
            sqlItems[8] = reader.readLine().trim();               // unix_group
            sqlItems[9] = new Integer(reader.readLine().trim());  // max_running_app_jobs
            sqlItems[10] = checkNullableField(reader.readLine()); // link_data                        
            sqlItems[11] = new Boolean(false);                    // app_lock
            
            // Get the env filename field from the progrps.ini file
            field = iniFile.GetProfileString((String)sqlItems[0], "APP_ENV_FILE", 
                                      error, Constants.NTCSS_PROGRP_INIT_FILE);
            if (error.equals(field) == false)                     // env_filename
               sqlItems[12] = checkNullableField(field);
            else
               sqlItems[12] = null;
            
            // Get the psp filename field from the progrps.ini file
            field = iniFile.GetProfileString((String)sqlItems[0], 
                                         "PROCESS_STATUS_FLAG", error, 
                                         Constants.NTCSS_PROGRP_INIT_FILE);
            if (error.equals(field) == false)                     // psp_filename
               sqlItems[13] = checkNullableField(field);
            else
               sqlItems[13] = null;
            
            // Get the termination signal field from the progrps.ini file
            field = iniFile.GetProfileString((String)sqlItems[0], 
                         "TERMINATION_SIGNAL", error, 
                         Constants.NTCSS_PROGRP_INIT_FILE);    
            if (error.equals(field) == false) {                    // termination signal
               field = checkNullableField(field);
               if (field == null)
                  sqlItems[14] = null;
               else
                  sqlItems[14] = new Integer(field);
            }
            else
               sqlItems[14] = null;
            
            reader.readLine(); // Record terminator
                                  
            // Write record to the database table
            statement.executeUpdate("INSERT INTO apps (app_title, hostname, " + 
                        "icon_file, icon_index, version_number, release_date, " +
                        "client_location, server_location, unix_group, " + 
                        "max_running_app_jobs, link_data, app_lock, " + 
                        "env_filename, psp_filename, termination_signal) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");
         }         
         reader.close();                  
         
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                              "/" + ReplicationLib.CURRENT_APPS_FILE));                                                      
                           
         // Read the records from the file and insert into cooresponding 
         //  database table          
         while ((sqlItems[1] = reader.readLine()) != null) {               
            sqlItems[1] = ((String)sqlItems[1]).trim();           // app_title
            reader.readLine();                                    // hostname
            reader.readLine();                                    // num_users
            sqlItems[0] = new Boolean((Integer.parseInt(
                reader.readLine().trim()) == 1) ? true : false);  // app_lock            
            reader.readLine(); // Record terminator
                                               
            // Write record to the database table
            statement.executeUpdate(StringLib.SQLformat("UPDATE apps " + 
                "SET app_lock = " + Constants.ST + " WHERE app_title = " + 
                Constants.ST, sqlItems));
         }         
         reader.close(); 
      } 
      catch (Exception exception) {
         throw new ImportException("Error importing applications <" + 
                                   exception.toString() + ">");         
      }
   }      
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class PredefinedJobs extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String field;
      String key;
      Hashtable pdj_info;
      PdjInfo info;
      int role_number;
      Enumeration pdjs;
      String exportFile;
      
      try {
                                             
         // Get all the information concerning predefined jobs
         resultSet = statement.executeQuery("SELECT pdj.pdj_title, " + 
                  "pdjar.app_title, role_number, command_line, job_type, " + 
                  "schedule_str FROM predefined_jobs pdj, pdj_app_roles pdjar, " + 
                  "app_roles ar WHERE pdj.pdj_title = pdjar.pdj_title AND " + 
                  "pdj.app_title = pdjar.app_title AND pdjar.app_title = " + 
                  "ar.app_title AND pdjar.role_name = ar.role_name");
         pdj_info = new Hashtable();
         while (resultSet.next() == true) { 
            
            // Create a unique for this entry in the hash table
            key = resultSet.getString("pdj_title") + '\3' + 
                  resultSet.getString("app_title");
            
            // If an entry doesn't already exist in the hash table for this
            //  key, insert it
            if (pdj_info.containsKey(key) == false) {                   
               info = new PdjInfo();
               info.pdj_title = resultSet.getString("pdj_title");
               info.app_title = resultSet.getString("app_title");
               info.command_line = resultSet.getString("command_line");
               info.job_type = resultSet.getInt("job_type");
               info.schedule_str = resultSet.getString("schedule_str");
               role_number = resultSet.getInt("role_number");
                              
               // Determine the corresponding bit mask for the given role_number
               //  e.g. for a role number of 1, the bit mask is 4
               //       for a role number of 2, the bit mask is 8
               //       for a role number of 3, the bit mask is 16, etc...
               if (role_number > 0)
                  info.roles_mask = 1 << (role_number + 1) ;
               else
                  info.roles_mask = 0;
               
               // Add the record to the hash table
               pdj_info.put(key, info);
            }
            else {   // Record already exists
               
               // Update the roles mask with role number from this record
               info = (PdjInfo)pdj_info.get(key);
               info.roles_mask |= 1 << (resultSet.getInt("role_number") + 1);
            }
         }
         resultSet.close(); 
         
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.PREDEFINED_JOBS_FILE;
         writer = new FileWriter(exportFile);
         
         pdjs = pdj_info.elements();
         while (pdjs.hasMoreElements() == true) {
            info = (PdjInfo)pdjs.nextElement();
            
            writer.write(info.pdj_title + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.app_title + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.command_line + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.roles_mask + ReplicationLib.FIELD_SEPARATOR);
            writer.write(info.job_type + ReplicationLib.FIELD_SEPARATOR);
            if (info.schedule_str != null)
               writer.write(info.schedule_str + ReplicationLib.FIELD_SEPARATOR);
            else
               writer.write(ReplicationLib.FIELD_SEPARATOR);
            writer.write(ReplicationLib.FIELD_SEPARATOR);
         }
         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.PREDEFINED_JOBS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting predefined jobs <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      BufferedReader reader;       
      Object sqlItems[];      
      int rolesMask;      
      int mask;
      Vector appRoles;
      Object info[];
      Hashtable appRoleTable;
      
      try {
                                                      
         // Open the "import" file for reading from it         
         reader = new BufferedReader(new FileReader(importDir + 
                           "/" + ReplicationLib.PREDEFINED_JOBS_FILE));
         
         // Create a quick lookup table of all the applications' roles
         appRoleTable = produceQuickAppRolesHash(statement); 
                           
         // Read the records from the file and insert into cooresponding 
         //  database table 
         sqlItems = new Object[5];         
         while ((sqlItems[0] = reader.readLine()) != null) {        
            sqlItems[0] = ((String)sqlItems[0]).trim();             // pdj_title
            sqlItems[1] = reader.readLine().trim();                 // app_title            
            sqlItems[2] = reader.readLine().trim();                 // command_line
            rolesMask = Integer.parseInt(reader.readLine().trim()); // old "role_access" field
            sqlItems[3] = new Integer(reader.readLine().trim());    // job_type
            sqlItems[4] = checkNullableField(reader.readLine());    // schedule_str            
            reader.readLine(); // Record terminator
            
            // Write record to the database table
            statement.executeUpdate("INSERT INTO predefined_jobs (pdj_title, " +
              "app_title, command_line, job_type, schedule_str) values(" + 
                  StringLib.SQLformat(
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST + "," + Constants.ST + "," + 
                   Constants.ST, sqlItems) + ")");
            
            // Insert the app roles for this predefined job
            appRoles = (Vector)appRoleTable.get(sqlItems[1]);
            for (int i = 0; i < appRoles.size(); i++) {
               info = (Object[])appRoles.elementAt(i);
                              
               mask = ((Integer)info[0]).intValue();
               if ((rolesMask & mask) == mask) {                         
                  sqlItems[2] = (String)info[1];
                  statement.executeUpdate(StringLib.SQLformat("INSERT INTO " +
                    "pdj_app_roles (pdj_title, app_title, role_name) values(" +
                    Constants.ST +"," + Constants.ST + "," + Constants.ST + ")",
                    sqlItems));
               }
            }
         }         
         reader.close();                        
      }      
      catch (Exception exception) {
         throw new ImportException("Error importing predefined jobs <" + 
                                   exception.toString() + ">");         
      }
   }      
   
   /**
    * A data structure for holding predefined information
    */
   static class PdjInfo
   {
      String pdj_title;
      String app_title;
      String command_line;
      int job_type;
      String schedule_str;
      int roles_mask;
   }
}

/**
 * Encapsulates operations on the set of printer drivers.
 */
class DriverFiles
{   
   /**
    * Exports the printer driver files.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static void exportFiles(Vector archiveList) throws ExportException
   {                  
      File fileDescp;      
      
      try {       
         
         // Check to make sure the driver directory exists
         fileDescp = new File(Constants.NTCSS_NTDRIVER_DIR);
         if (fileDescp.exists() == false)
            return;     
                  
         getTarEntries(archiveList, Constants.NTCSS_NTDRIVER_DIR, 
                                 "spool/ntdrivers");                                          
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting print driver files <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the printer driver files. If there are any errors, a special 
    *  exception is thrown. 
    */
   public static void importFiles(String importDir) throws ImportException
   {
      File fileDescp;
      
      try {
         
         // Check to make sure the import driver directory exists
         fileDescp = new File(importDir + "/spool/ntdrivers");
         if (fileDescp.exists() == false)
            return;
         
         copyDrivers(importDir + "/spool/ntdrivers", 
                               Constants.NTCSS_NTDRIVER_DIR);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing print driver files <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Recurively adds file tar entries that are in "dirName" to the list "list".
    */
   private static void getTarEntries(Vector list, String dirName, 
                               String tarEntryRoot) throws ExportException
   {
      File dirDescp;
      TarEntry tarEntry;
      File files[];
      
      try {
                                   
         // List all the files and directories under this directory
         dirDescp = new File(dirName);
         files = dirDescp.listFiles();
         for (int i = 0; i < files.length; i++) {         
            
            // If this entry is a file, add it to the list
            if (files[i].isFile() == true) {
               
               // Create the tar entry
               tarEntry = new TarEntry(files[i]);
               // The "setName" method insures that the archive entry will have
               //  a relative path not an absolute path specified which is 
               //  necessary for proper extraction from the tar archive.
               tarEntry.setName(tarEntryRoot + "/" + files[i].getName()); 
            
               // Add the tar entry to the list
               list.add(tarEntry);                           
            }
            // Otherwise, this entry must be a directory, so recurively list
            //  its files and subdirectories
            else 
               getTarEntries(list, files[i].getAbsolutePath(), 
                             tarEntryRoot + "/" + files[i].getName());
               
         }
                              
         // Now add this directory to the list         
         tarEntry = new TarEntry(dirDescp);
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.         
         tarEntry.setName(tarEntryRoot);          
            
         // Add the tar entry to the list
         list.add(tarEntry);                                                       
         
      }
      catch (ExportException exportException) {  // This must have been thrown 
         throw exportException;                 //  from the recurse method         
      }
      catch (Exception exception) {
         throw new ExportException("Error listing print driver files <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Copies the print drivers from the given source directory to the given
    *  destination directory.
    *
    *  NOTE: Empty directories don't get "copied" into the spool/ntdrivers dir
    */
   private static void copyDrivers(String sourceDir, 
                                   String destDir) throws ImportException
   {
      File dirDescp, tmpDirDescp;
      File files[];
      
      try {
         
         // List all the files and directories under the source directory
         dirDescp = new File(sourceDir);
         files = dirDescp.listFiles();
         for (int i = 0; i < files.length; i++) {
            
            // If this entry is a file, copy it to the destination directory
            if (files[i].isFile() == true) {
               
               // Check to make sure the directory that this file is to be 
               //  copied into exists
               tmpDirDescp = new File(destDir);             
               if (tmpDirDescp.exists() == false)
                  tmpDirDescp.mkdirs();                
               
               // Copy the driver
               if (FileOps.copy(files[i].getAbsolutePath(), 
                                destDir + "/" + files[i].getName()) == false)
                  throw new ImportException("Error copying <" + 
                      files[i].getAbsolutePath() + "> to <" + destDir + "/" + 
                      files[i].getName() + ">");                                
            }
            // Otherwise, this entry must be a directory, so recurively list
            //  its files and subdirectories
            else 
               copyDrivers(files[i].getAbsolutePath(), 
                           destDir + "/" + files[i].getName());
               
         }                   
      }
      catch (ImportException importException) {  // This must have been thrown 
         throw importException;                  //  from the recurse method         
      }
      catch (Exception exception) {
         throw new ImportException("Error copying print driver files <" + 
                                   exception.toString() + ">");         
      }
   }
}

/**
 * Encapsulates operations on the corresponding database table.
 */
class BulletinBoards extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Object sqlItems[];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + ReplicationLib.BULLETIN_BOARDS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         sqlItems = new Object[2];
         resultSet = statement.executeQuery("SELECT * FROM bulletin_boards");
         while (resultSet.next() == true) {
            sqlItems[0] = resultSet.getString("bb_name");
            sqlItems[1] = resultSet.getString("creator");
            writer.write("INSERT INTO bulletin_boards (bb_name, creator) values " +
             StringLib.SQLformat("(" + Constants.ST + "," + Constants.ST + ")", sqlItems));
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.BULLETIN_BOARDS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting bulletin boards <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      try {        
         sqlImport(importDir + "/" + ReplicationLib.BULLETIN_BOARDS_FILE,
                     statement);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing bulletin boards <" + 
                                   exception.toString() + ">");         
      }
   }      
}   

/**
 * Encapsulates operations on the corresponding database table.
 */
class BulletinBoardSubscribers extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Object sqlItems[];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + 
                      ReplicationLib.BULLETIN_BOARD_SUBSCRIBERS_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         sqlItems = new Object[2];
         resultSet = statement.executeQuery("SELECT * FROM bb_subscribers");
         while (resultSet.next() == true) {
            sqlItems[0] = resultSet.getString("login_name");
            sqlItems[1] = resultSet.getString("bb_name");
            writer.write("INSERT INTO bb_subscribers (login_name, bb_name) values " +
             StringLib.SQLformat("(" + Constants.ST + "," + Constants.ST + ")", 
             sqlItems));
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.BULLETIN_BOARD_SUBSCRIBERS_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting bulletin board subscribers <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      try {        
         sqlImport(importDir + "/" +
                   ReplicationLib.BULLETIN_BOARD_SUBSCRIBERS_FILE, statement);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing bulletin board subscribers <" + 
                                   exception.toString() + ">");         
      }
   }      
}   

/**
 * Encapsulates operations on the corresponding database table.
 */
class BulletinBoardMessages extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Object sqlItems[];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + 
                      ReplicationLib.BULLETIN_BOARD_MESSAGES_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         sqlItems = new Object[6];
         resultSet = statement.executeQuery("SELECT * FROM bb_messages");
         while (resultSet.next() == true) {
            sqlItems[0] = resultSet.getString("msg_id");
            sqlItems[1] = resultSet.getString("creation_time");
            sqlItems[2] = resultSet.getString("login_name");
            sqlItems[3] = resultSet.getString("msg_title");
            sqlItems[4] = resultSet.getString("contents");
            sqlItems[5] = new Boolean(resultSet.getBoolean("sent"));
            writer.write("INSERT INTO bb_messages (msg_id, creation_time, " +
             "login_name, msg_title, contents, sent) values " +
             StringLib.SQLformat("(" + Constants.ST + "," + Constants.ST + "," +
              Constants.ST + "," + Constants.ST + "," + 
              Constants.ST + "," + Constants.ST + "," + ")", 
             sqlItems));
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.BULLETIN_BOARD_MESSAGES_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting bulletin board messages <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      try {        
         sqlImport(importDir + "/" +
                   ReplicationLib.BULLETIN_BOARD_MESSAGES_FILE, statement);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing bulletin board messages <" + 
                                   exception.toString() + ">");         
      }
   }      
}   

/**
 * Encapsulates operations on the corresponding database table.
 */
class BulletinBoardMessageMap extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Object sqlItems[];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + 
                      ReplicationLib.BULLETIN_BOARD_MESSAGE_MAP_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         sqlItems = new Object[2];
         resultSet = statement.executeQuery("SELECT * FROM bb_message_map");
         while (resultSet.next() == true) {
            sqlItems[0] = resultSet.getString("bb_name");
            sqlItems[1] = resultSet.getString("msg_id");            
            writer.write("INSERT INTO bb_message_map (bb_name, msg_id) values " +
             StringLib.SQLformat("(" + Constants.ST + "," + Constants.ST + ")", 
             sqlItems));
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.BULLETIN_BOARD_MESSAGE_MAP_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting bulletin board message map <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      try {        
         sqlImport(importDir + "/" +
                   ReplicationLib.BULLETIN_BOARD_MESSAGE_MAP_FILE, statement);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing bulletin board message map <" + 
                                   exception.toString() + ">");         
      }
   }      
}   

/**
 * Encapsulates operations on the corresponding database table.
 */
class BulletinBoardReadHistory extends BasicTable
{   
   /**
    * Exports the contents of the corresponding database table to a flat file.
    *  If there are any errors, a special exception is thrown. Otherwise a tar 
    *  entry object is returned so that it can be added to a tar archive.
    */
   public static TarEntry exportTable(String exportDir,
                                      Statement statement) throws ExportException
   {
      ResultSet resultSet;                
      FileWriter writer;    
      TarEntry tarEntry;
      String exportFile;
      Object sqlItems[];
      
      try {
                           
         // Open the "export" file for writing to it
         exportFile = exportDir + "/" + 
                      ReplicationLib.BULLETIN_BOARD_READ_HISTORY_FILE;
         writer = new FileWriter(exportFile);
         
         // Write out the table's contents to the file
         sqlItems = new Object[3];
         resultSet = statement.executeQuery("SELECT * FROM bb_read_history");
         while (resultSet.next() == true) {
            sqlItems[0] = resultSet.getString("msg_id");
            sqlItems[1] = resultSet.getString("login_name");            
            sqlItems[2] = resultSet.getString("isRead");            
            writer.write("INSERT INTO bb_read_history (msg_id, login_name, " +
             "isRead) values " +
             StringLib.SQLformat("(" + Constants.ST + "," + Constants.ST + 
                "," + Constants.ST + ")", sqlItems));
            writer.write(ReplicationLib.FIELD_SEPARATOR);            
         }
         resultSet.close();         
         writer.close();
         
         // Create the tar entry
         tarEntry = new TarEntry(new File(exportFile));
         // The "setName" method insures that the archive entry will have
         //  a relative path not an absolute path specified which is 
         //  necessary for proper extraction from the tar archive.
         tarEntry.setName(ReplicationLib.BULLETIN_BOARD_READ_HISTORY_FILE);                
         
         return tarEntry;
      }
      catch (Exception exception) {
         throw new ExportException("Error exporting bulletin board read history <" + 
                                   exception.toString() + ">");         
      }
   }
   
   /**
    * Imports the contents of the flat file to the corresponding database table.
    *  If there are any errors, a special exception is thrown. 
    */
   public static void importTable(String importDir, 
                                  Statement statement) throws ImportException
   {                     
      try {        
         sqlImport(importDir + "/" +
                   ReplicationLib.BULLETIN_BOARD_READ_HISTORY_FILE, statement);
      }
      catch (Exception exception) {
         throw new ImportException("Error importing bulletin board read history <" + 
                                   exception.toString() + ">");         
      }
   }      
}   