/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.Date;
import java.sql.Time;

import java.text.SimpleDateFormat;

import java.net.Socket;

import java.util.Date;
import java.util.GregorianCalendar;

import java.io.File;
import java.io.FileWriter;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

import ntcsss.log.Log;

import ntcsss.database.DatabaseConnectionManager;

import ntcsss.libs.BundleLib;
import ntcsss.libs.structs.SvrProcessFilter;
import ntcsss.libs.structs.ServerProcess;

import ntcsss.util.FileOps;

/**
 * A set of methods to handle server processes
 */
public class SvrProcLib
{
   /**
    * Returns size of file
    */
   public static int getServerProcessQueueList(String progroupTitle,
                                               String loginName, int role, 
                                               String queryFilename, 
                                               StringBuffer messageData, 
                                               int isScheduledJob, 
                                               int filterMask, 
                                               SvrProcessFilter filterData)
   {
      File fileDescp;
      String SQLString;
      Connection connection;
      PreparedStatement statement;
      ResultSet resultSet;
      int fileSize;
      ServerProcess server_proc;
      int numQueItems = 0;
      String buf, buf2;
      Object parameters[];
      FileWriter writer;
      String scheduleString;
      
      // Remove the given file if it exists
      try {         
         fileDescp = new File(queryFilename);
         if (fileDescp.exists() == true)
            fileDescp.delete();
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", "getServerProcessQueueList", 
                  exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
      
      // If the "isScheduleJob" flag is true, pull records from the 
      //  scheduled_jobs table. Otherwise, pull from the active_jobs table.
      if (isScheduledJob != 0)
         SQLString = "SELECT * FROM scheduled_jobs WHERE ntcss_job_id IN " +
                     "(SELECT ntcss_job_id FROM job_schedules)";
      else
         SQLString = "SELECT * FROM active_jobs";
      
      try {
         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("SvrProcLib", "getServerProcessQueueList",  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_OPEN_DB_FILE;
         }   
         
         // Create a writer to the output file
         writer = new FileWriter(queryFilename, false);
                   
         //  Execute the query
         statement = connection.prepareStatement(SQLString);
         resultSet = statement.executeQuery();

         // Read through the returned data
         fileSize = 0;
         server_proc = new ServerProcess();
         parameters = new Object[14];
         while (resultSet.next()) {
            
            // Read the info off the current row
            // Due to a SQL Server ODBC error, the fields must be read off
            //  in the order they appear in the table
            server_proc.prt_flag = 0;                       
            server_proc.process_id = resultSet.getString("ntcss_job_id");
            server_proc.command_line = resultSet.getString("command_line");            
            server_proc.priority = resultSet.getInt("priority");
            server_proc.security_class = resultSet.getInt("security_class");
            scheduleString = resultSet.getString("schedule_string");
            server_proc.login_name = resultSet.getString("job_initiator");
            server_proc.unix_owner = resultSet.getString("job_requestor");            
            server_proc.progroup_title = resultSet.getString("app_title");
            server_proc.job_descrip = resultSet.getString("job_description");
            server_proc.request_time = resultSet.getString("request_time");
            server_proc.launch_time = resultSet.getString("launch_time");
            server_proc.end_time = resultSet.getString("end_time");
            server_proc.pid = resultSet.getInt("pid");             
            server_proc.proc_status = resultSet.getInt("exit_status");
            server_proc.orig_host = resultSet.getString("originating_host");
            server_proc.prt_flag |=
                           resultSet.getBoolean("restartable") ? 0x04:0;            
            server_proc.cust_proc_status = 
                                   resultSet.getString("custom_job_status"); 
            server_proc.prt_flag |= 
                           resultSet.getBoolean("print_file_switch") ? 0x01:0;            
            server_proc.prt_name = resultSet.getString("printer_name");            
            server_proc.prt_sec_class = resultSet.getInt("printer_sec_class");            
            server_proc.prt_filename = resultSet.getString("print_filename");
            server_proc.prt_flag |=
                           resultSet.getBoolean("remove_print_file") ? 0x02:0;            
            server_proc.copies = resultSet.getInt("print_num_copies");
            server_proc.banner = resultSet.getBoolean("print_banner_switch") ? 1:0;                
            server_proc.papersize = resultSet.getInt("print_page_size");
            server_proc.orientation = resultSet.getBoolean("orient_portrait") ? 1:0;                
            server_proc.prt_flag |=
                           resultSet.getBoolean("sdi_required") ? 0x10:0;            
            server_proc.sdi_ctl_rec = resultSet.getString("sdi_ctrl_rec");            
            server_proc.sdi_data_file = resultSet.getString("sdi_data_file");            
            server_proc.sdi_tci_file = resultSet.getString("sdi_tci_file");            
            server_proc.sdi_device = resultSet.getString("sdi_device");                                                                                                
            server_proc.prt_flag |=
                           resultSet.getBoolean("sdi_read") ? 0x08:0;
            server_proc.prt_flag |=
                           resultSet.getBoolean("sdi_failed") ? 0x20:0;
                           
            if ((filterMask & 
                 Constants.SPQ_BIT_MASK_ONLY_USER_ACTIVE_PROCS) != 0) {
               if ((loginName.equals(server_proc.login_name) == true) &&
                   (server_proc.pid > 0)) {
                                        
                  parameters[0] = server_proc.process_id;
                  fileSize += BundleLib.bundleQuery(writer, "C", parameters);
                  numQueItems++;
               }
               
               continue;
            }
                           
            if (includeProcess(server_proc, filterMask, filterData, 
                               progroupTitle, loginName, role) == true) {
                                                 
               parameters[0] = new Integer(server_proc.pid);
               parameters[1] = server_proc.process_id;
               parameters[2] = new Integer(server_proc.priority);
               parameters[3] = server_proc.progroup_title;
               parameters[4] = server_proc.command_line;
               parameters[5] = new Integer(server_proc.proc_status);
               fileSize += BundleLib.bundleQuery(writer, "ICICCI", parameters);                                   

               if ((filterMask & Constants.SPQ_BIT_MASK_WITH_CUST_PROC_STATUS) != 0) {   
                  if (isScheduledJob != 0)
                     parameters[0] = scheduleString;
                  else
                     parameters[0] = server_proc.cust_proc_status;               
                  fileSize += BundleLib.bundleQuery(writer, "C", parameters);
               }
                                     
               parameters[0] = server_proc.login_name;
               parameters[1] = server_proc.unix_owner;
               fileSize += BundleLib.bundleQuery(writer, "CC", parameters);
                                                  
               if ((filterMask & Constants.SPQ_BIT_MASK_WITH_JOB_DESCRIP) != 0) {                  
                  parameters[0] = server_proc.job_descrip;                              
                  fileSize += BundleLib.bundleQuery(writer, "C", parameters);
               }
                            
               parameters[0] = server_proc.request_time;
               parameters[1] = server_proc.launch_time;               
               parameters[2] = server_proc.end_time;
               parameters[3] = new Integer(server_proc.security_class);
               parameters[4] = server_proc.orig_host;
               fileSize += BundleLib.bundleQuery(writer, "CCCIC", parameters);
                                   
               if ((filterMask & Constants.SPQ_BIT_MASK_WITH_PRT_REQ_INFO) != 0) {                                 
                  parameters[0] =  server_proc.prt_name;
                  parameters[1] =  server_proc.hostname;
                  parameters[2] =  new Integer(server_proc.prt_sec_class);
                  parameters[3] =  server_proc.prt_filename;
                  parameters[4] =  new Integer(server_proc.prt_flag);
                  parameters[5] =  server_proc.sdi_ctl_rec;
                  parameters[6] =  server_proc.sdi_data_file;
                  parameters[7] =  server_proc.sdi_tci_file;
                  parameters[8] =  server_proc.sdi_device;
                  parameters[9] =  new Integer(server_proc.orientation);
                  parameters[10] = new Integer(server_proc.banner);
                  parameters[11] = new Integer(server_proc.copies);
                  parameters[12] = new Integer(server_proc.papersize);
                  fileSize += BundleLib.bundleQuery(writer, "CCICICCCCIIII", 
                                      parameters);                                      
               }
               
               numQueItems++;
            }       
         }  // End of while loop
         
         // Release the system resources
         resultSet.close();
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         writer.close();

         // Bundle up the total file size into the message..                  
         buf = String.valueOf(numQueItems);
         buf2 = String.valueOf(fileSize + buf.length() + 1);
         buf = String.valueOf(buf2.length());
         messageData.delete(0, messageData.length());  // Clear out the buffer
         messageData.insert(0, buf);
         messageData.append(buf2);

         buf2 = String.valueOf(numQueItems);
         buf = String.valueOf(buf2.length());
         messageData.append(buf);
         messageData.append(buf2);

         return(fileSize);   // Everthing ok
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", "getServerProcessQueueList", 
                  exception.toString());
         return ErrorCodes.ERROR_CANT_CREATE_QUERY;
      }
   }     
         
   /**
    * Removes the given process from the database tables
    */
   public static int deleteServerProcess(String processId, String progroupTitle,
                                         int isScheduledJob,
                                         boolean ignoreRunning)
   {
      String SQLString;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      int pid, priority;
      String scheduleString;
      Object args[];

      args = new Object[2];
      args[0] = processId; 
      args[1] = progroupTitle;      
      SQLString = StringLib.SQLformat("SELECT pid, priority FROM " +
                     ((isScheduledJob == 1) ? "scheduled_jobs":"active_jobs") +
                     " WHERE ntcss_job_id = " + Constants.ST + 
                     " AND app_title = " + Constants.ST, args);
                     
      try {

         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("SvrProcLib", "deleteServerProcess",  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_NO_SUCH_PROCESS;
         }         
                   
         //  Execute the query
         statement = connection.createStatement();
         resultSet = statement.executeQuery(SQLString);

         // Read the returned data
         if (resultSet.next() == false) {
            Log.error("SvrProcLib", "deleteServerProcess",
              "Could not find job ID <" + processId + "> in the " +
              ((isScheduledJob == 1) ? "scheduled_jobs":"active_jobs") +
              " table!");

            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            return ErrorCodes.ERROR_NO_SUCH_PROCESS;
         }

         pid = resultSet.getInt("pid");
         priority = resultSet.getInt("priority");

         // Close the database resources
         resultSet.close();         

         // Check the constraints
         if (!ignoreRunning && (priority != Constants.SCHEDULED_PRIORITY)
              && (pid > 0)) {

            Log.error("SvrProcLib", "deleteServerProcess", "Modification " +
                      "constraints violated for job " + processId + "!");

            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            return ErrorCodes.ERROR_CANT_REMOVE_ACT_PROCESS;
         }

         // Delete the job from the database
         args = new Object[1];
         args[0] = processId;
//         SQLString = StringLib.SQLformat("DELETE FROM " +
//                    ((isScheduledJob == 1) ? "scheduled_jobs":"active_jobs") +
//                    " WHERE ntcss_job_id = " + Constants.ST, args);
         SQLString = StringLib.SQLformat("ntcss_job_id = " + Constants.ST, args);
         
                    
         //  Execute the update         
//         statement.executeUpdate(SQLString);
         DataLib.cascadeDelete(statement, (isScheduledJob == 1) ? "scheduled_jobs":"active_jobs",
                               SQLString);

         // Close the database resources
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", "deleteServerProcess", exception.toString());

         return ErrorCodes.ERROR_CANT_DELETE_VALUE;
      }

      return 0;
   }
   
   /**
    * Update the server process record for the given "processId" based on
    *  the value of the "mask" and the other values passed in. The 
    *  progroupTitle parameter is set with the "app_title" field of the 
    *  process's record.  If no errors occur, 0 is returned.
    */
   public static int changeServerProcess(String processId, int pid, int mask, 
                                         int retval, int priority,
                                         String custProcStatus, 
                                         String launchTime, String endTime, 
                                         String devname, 
                                         StringBuffer progroupTitle, 
                                         int bitFlags, int whichPriority)
   {
      int isScheduledJob;
      String tableName;
      String SQLString;
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      ServerProcess sJob_data;
      int schedStrChanged;
      String tmp_string1, tmp_string2;
      int rtnval = 0;
      String scheduleString;
      Object args[];     
      
      try {
         if (whichPriority == Constants.SCHEDULED_PRIORITY) {
            isScheduledJob = 1;
            tableName = "scheduled_jobs";
         }
         else {
            isScheduledJob = 0;
            tableName = "active_jobs";
         }
         
         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("SvrProcLib", "changeServerProcess",  
                      "\"null\" connection returned");
            return ErrorCodes.ERROR_CANT_READ_VALUE;
         } 
         
         statement = connection.createStatement();
         
         // Check if we were given the ntcss job ID we can use to search by..
         if (processId == null) {
            SQLString = "SELECT * FROM " + tableName + " WHERE pid = " + pid;
                          
            //  Execute the query            
            resultSet = statement.executeQuery(SQLString);
            
            if (resultSet.next() == false) {
               Log.error("SvrProcLib", "changeServerProcess", 
                         "Job pid <" + pid + "> does not exist in the " + 
                         tableName + " table!");
               return ErrorCodes.ERROR_CANT_FIND_VALUE;
            }                        
         }
         else {  // We were given an ntcss_job_id to go on..     
            args = new Object[1];
            args[0] = processId;
            SQLString = StringLib.SQLformat("SELECT * FROM " + tableName + 
                                 " WHERE ntcss_job_id = " + Constants.ST, args); 
                
            //  Execute the query            
            resultSet = statement.executeQuery(SQLString);
            
            if (resultSet.next() == false) {
               Log.error("SvrProcLib", "changeServerProcess", 
                         "Job ID <" + processId + "> does not exist in the " + 
                         tableName + " table!");
               return ErrorCodes.ERROR_CANT_FIND_VALUE;
            }
         }               
         
         // Get the information about the server process
         // Due to a SQL Server ODBC error, the fields have to be pulled off
         //  in the order that they appear in the DB table
         sJob_data = new ServerProcess();      
         sJob_data.prt_flag = 0;
         sJob_data.process_id = resultSet.getString("ntcss_job_id");
         sJob_data.command_line = resultSet.getString("command_line");
         sJob_data.priority = resultSet.getInt("priority");
         sJob_data.security_class = resultSet.getInt("security_class");
         scheduleString = resultSet.getString("schedule_string");
         sJob_data.login_name = resultSet.getString("job_initiator");
         sJob_data.unix_owner = resultSet.getString("job_requestor");
         sJob_data.progroup_title = resultSet.getString("app_title");
         sJob_data.job_descrip = resultSet.getString("job_description");
         sJob_data.request_time = resultSet.getString("request_time");
         sJob_data.launch_time = resultSet.getString("launch_time");
         sJob_data.end_time = resultSet.getString("end_time");         
         sJob_data.pid = resultSet.getInt("pid");                                    
         sJob_data.proc_status = resultSet.getInt("exit_status");
         sJob_data.orig_host = resultSet.getString("originating_host");
         sJob_data.prt_flag |=
            (resultSet.getBoolean("restartable") ? 0x04:0);         
         sJob_data.cust_proc_status = resultSet.getString("custom_job_status");                           
         sJob_data.prt_flag |=
            (resultSet.getBoolean("print_file_switch") ? 0x01:0);            
         sJob_data.prt_name = resultSet.getString("printer_name");         
         sJob_data.prt_sec_class = resultSet.getInt("printer_sec_class");         
         sJob_data.prt_filename = resultSet.getString("print_filename");
         sJob_data.prt_flag |=
            (resultSet.getBoolean("remove_print_file") ? 0x02:0);
         sJob_data.copies = resultSet.getInt("print_num_copies");
         sJob_data.banner = resultSet.getBoolean("print_banner_switch")?1:0;
         sJob_data.papersize = resultSet.getInt("print_page_size");         
         sJob_data.orientation = resultSet.getBoolean("orient_portrait")?1:0;
         sJob_data.prt_flag |=
            (resultSet.getBoolean("sdi_required") ? 0x10:0);            
         sJob_data.sdi_ctl_rec = resultSet.getString("sdi_ctrl_rec");         
         sJob_data.sdi_data_file = resultSet.getString("sdi_data_file");
         sJob_data.sdi_tci_file = resultSet.getString("sdi_tci_file");
         sJob_data.sdi_device = resultSet.getString("sdi_device");                  
         sJob_data.prt_flag |=
            (resultSet.getBoolean("sdi_read") ? 0x08:0);         
         sJob_data.prt_flag |=
            (resultSet.getBoolean("sdi_failed") ? 0x20:0); 
            
         // Close the result set
         resultSet.close();
                              
         progroupTitle.delete(0, progroupTitle.length());
         progroupTitle.insert(0, sJob_data.progroup_title);                           
         
         if ((mask & Constants.CHANGE_PROC_STAT) != 0)
            sJob_data.proc_status = retval;
             
         schedStrChanged = 0;
         if ((mask & Constants.CHANGE_CUST_STAT) != 0) {

            if (isScheduledJob == 0) 
               sJob_data.cust_proc_status = custProcStatus;                           
            else {
            
               // This is fuzzy logic but if the job has not run yet, and we get 
               //  this flag, then it usually means we need to change the job 
               //  schedule. Otherwise, it must be a request to change the 
               //  custom status.
               if ((sJob_data.pid == Constants.WAITING_FOR_OK_STATE) ||
                   (sJob_data.pid == Constants.WAITING_FOR_EXEC_STATE) ||
                   (sJob_data.pid == Constants.ABOUT_TO_BE_RUN_STATE) ||
                   (sJob_data.pid == Constants.APPROVED_STATE) ||
                   (sJob_data.pid == Constants.SCHED_TO_RUN_STATE)) {        
                  scheduleString = custProcStatus;
                  schedStrChanged = 1;
               }
               else
                  sJob_data.cust_proc_status = custProcStatus;
            }
         }
         
         // Assign Device
         if ((mask & Constants.CHANGE_DEVICE) != 0)
            sJob_data.sdi_device = devname;

         // Reassign PID value if needed.
         if ((mask & Constants.CHANGE_PID_VALUE) != 0)
            sJob_data.pid = pid;

         // Reassign launch time if needed.
         if ((mask & Constants.CHANGE_LAUNCH_TIME) != 0)
            sJob_data.launch_time = launchTime;

         // Reassign end time if needed.
         if ((mask & Constants.CHANGE_END_TIME) != 0)
            sJob_data.end_time = endTime;

         // Reassign priority if needed.
         if ((mask & Constants.CHANGE_PRIORITY) != 0)
            sJob_data.priority = priority;

         // Reassign prt_flag if needed.
         if ((mask & Constants.CHANGE_PRT_FLAG) != 0)
            sJob_data.prt_flag = bitFlags;
         
         if (schedStrChanged != 0) {
            tmp_string1 = "schedule_string";
            tmp_string2 = scheduleString;
         }
         else {    
            tmp_string1 = "custom_job_status";
            tmp_string2 = sJob_data.cust_proc_status;
         }
         
         // Replace the server process record..
         
         // NOTE: If any more field changes are allowed, then they need to be 
         //    added to this SQL update statement!!!                 
         args = new Object[15];
         args[0] = new Integer(sJob_data.proc_status);         
         args[1] = tmp_string2;         
         args[2] = sJob_data.sdi_device;
         args[3] = new Integer(sJob_data.pid);
         args[4] = sJob_data.launch_time;
         args[5] = sJob_data.end_time;
         args[6] = new Integer(sJob_data.priority);
         args[7] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK1) != 0);
         args[8] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK2) != 0);
         args[9] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK3) != 0);
         args[10] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK4) != 0);
         args[11] = new Boolean(!((sJob_data.prt_flag & Constants.PRT_FLAG_MASK4) != 0));
         args[12] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK5) != 0);
         args[13] = new Boolean((sJob_data.prt_flag & Constants.PRT_FLAG_MASK6) != 0);
         args[14] = sJob_data.process_id;
         SQLString = StringLib.SQLformat("UPDATE " + ((isScheduledJob == 1) ? "scheduled_jobs":"active_jobs") + " SET exit_status = " + Constants.ST + "," +
            tmp_string1 + " = " + Constants.ST + ", sdi_device = " + Constants.ST + ", pid = " + Constants.ST + ", launch_time = " + Constants.ST + 
            ", end_time = " + Constants.ST + ", priority = " + Constants.ST + ", " +
            "print_file_switch = " +  Constants.ST + ", remove_print_file = " + Constants.ST + "," + 
            "restartable = " + Constants.ST + ", sdi_read = " + Constants.ST + ", sdi_write = " + Constants.ST + ", " +
            "sdi_required = " + Constants.ST + ", sdi_failed = " + Constants.ST + " WHERE ntcss_job_id = " + Constants.ST,
            args);
                              
         //  Execute the update         
         statement.executeUpdate(SQLString);                     
         
         statement.close();         
         DatabaseConnectionManager.releaseConnection(connection);
         
         if ((schedStrChanged != 0) && (isScheduledJob == 1)) {
            rtnval = nChangeJobSchedules(processId, scheduleString);
            if (rtnval >= 0)
               rtnval = 0;               
         }
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", "changeServerProcess", exception.toString());
      }   
      
      return rtnval;
   }
   
   /**
    * Returns true if the given server process meets the criteria
    */
   private static boolean includeProcess(ServerProcess server_proc, int mask,
                                         SvrProcessFilter filterData, 
                                         String progroupTitle, String loginName,
                                         int role)
   {
      if ((server_proc.progroup_title).equals(progroupTitle) == false)
         return false;

      if ((mask & Constants.SPQ_BIT_MASK_ONLY_CERTAIN_USER) != 0)
         if (loginName.equals(server_proc.login_name) == false)
            return false;
 
      if ((mask & Constants.SPQ_BIT_MASK_ONLY_RUNNING_PROCS) != 0)
         if (server_proc.pid <= 0)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_PRIORITY) != 0)
         if (server_proc.priority != filterData.priority)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_COMMAND_LINE) != 0)
         if ((server_proc.command_line).equals(filterData.commandLine) == false)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_CUST_PROC_STATUS) != 0)
         if ((server_proc.cust_proc_status).equals(filterData.custProcStatus) == false)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_LOGIN_NAME) != 0)
         if ((server_proc.login_name).equals(filterData.loginName) == false)
            return false ;
 
      if ((mask & Constants.SPQ_BIT_MASK_UNIX_OWNER) != 0)
         if ((server_proc.unix_owner).equals(filterData.unixOwner) == false)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_PROGROUP_TITLE) != 0)
         if ((server_proc.progroup_title).equals(filterData.progroupTitle) == false)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_JOB_DESCRIP) != 0)
         if ((server_proc.job_descrip).equals(filterData.jobDescription) == false)
            return false;

      if ((mask & Constants.SPQ_BIT_MASK_ORIG_HOST) != 0)
         if ((server_proc.orig_host).equals(filterData.originatingHost) == false)
            return false;

      switch (role) {    
         case Constants.NTCSS_USER_ROLE:
            return loginName.equals(server_proc.login_name);            
         case Constants.APP_ADMIN_ROLE:
         case Constants.NTCSS_ADMIN_ROLE:
            return true;            
      }

      return true;
   }
   
   /**
    * This function parses an old ntcss schedule string into separate
    *  job_schedule table records, replacing all pre-existing schedule records
    *  for this processId. If no errors occur, 0 is returned. Otherwise, a
    *  negative integer is returned to indicate the error.
    */
   public static int nChangeJobSchedules(String processId, String strSchedules)
   {
      int length;
      int numSchedules;
      String SQLString;
      Connection connection;
      Statement statement;
      int currentSchedStartIndex = 0;
      String currentSchedule, strYear;
      SimpleDateFormat dateFormatter;
      char strNtcssDate[] = new char[16];
      char strExpirationTime[] = new char[16];      
      Object args[];
      long nowMilliseconds;
      String nowDate;
      String nowTime;
      String validUntilDate = null;
      String validUntilTime = null;  
      String tmpStr;            
      GregorianCalendar exprDate;
      java.util.Date tmpDate;
      
      try {
                  
         length = strSchedules.length();
         numSchedules = length/Constants.MAX_SCHED_STR_LEN;
         
         // Make sure there isn't garbage here..
         if ((length % Constants.MAX_SCHED_STR_LEN) != 0) {    
            Log.error("SvrProcLib", "nChangeJobSchedules", 
                      "Schedule string is corrupt! <" + strSchedules + ">");             
            return -2;
         }                  
         
         // Get a connection from the database
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("SvrProcLib", "nChangeJobSchedules",  
                      "\"null\" connection returned");
            return -3;
         }
         
         // First delete any existing schedules for the given process Id
         args = new Object[1];
         args[0] = processId;
         SQLString = 
            StringLib.SQLformat("DELETE FROM job_schedules WHERE ntcss_job_id = " 
                                + Constants.ST, args);                  
         statement = connection.createStatement();
         statement.executeUpdate(SQLString);   
         
         // Create SQL fields that represent the current time
         nowMilliseconds = System.currentTimeMillis();
         nowDate = (new java.sql.Date(nowMilliseconds)).toString();
         nowTime = (new Time(nowMilliseconds)).toString();
                  
         // Read off each schedule in the given strSchedules
         strYear = null;
         for (int i = 0; i < numSchedules; i++) {
            
            // Get the next schedule
            currentSchedule = strSchedules.substring(currentSchedStartIndex, 
                         currentSchedStartIndex + Constants.MAX_SCHED_STR_LEN);
            
            // The valid_until fields are incremented by one minute after the
            // scheduled launch time so the schedule expires right after it is
            // to run.
            if ((currentSchedule.charAt(0) == 'E') ||   // If "exception" or 
                (currentSchedule.charAt(0) == 'O')) {   //  or "one shot"
                   
               // We ned to get the current year for incrementation purposes.
               //  If year is non-empty, we must have already set it
               //  in a previous loop.                                 
               if (strYear == null) {                                                     
                  dateFormatter = new SimpleDateFormat("yy");      
                  strYear = dateFormatter.format(new java.util.Date());                                    
               }
               
               // Build a ntcss_date string compatible with getNextFullTimeStr()
               //  YYMMDD.HHMMSS               
               strNtcssDate[0]  =  strYear.charAt(0);         // year 1/2
               strNtcssDate[1]  =  strYear.charAt(1);         // year 2/2
               strNtcssDate[2]  =  currentSchedule.charAt(8); // month 1/2
               strNtcssDate[3]  =  currentSchedule.charAt(9); // month 1/2
               strNtcssDate[4]  =  currentSchedule.charAt(6); // day 1/2
               strNtcssDate[5]  =  currentSchedule.charAt(7); //  day 1/2
               strNtcssDate[6]  =  '.';                       // delimeter
               strNtcssDate[7]  =  currentSchedule.charAt(3); // hour 1/2
               strNtcssDate[8]  =  currentSchedule.charAt(4); // hour 2/2
               strNtcssDate[9]  =  currentSchedule.charAt(1); // minute 1/2
               strNtcssDate[10] =  currentSchedule.charAt(2); // minute 2/2
               strNtcssDate[11] =  '0';                       // seconds 1/2
               strNtcssDate[12] =  '0';                       // seconds 2/2
               strNtcssDate[13] =  '\0';
               
               // We only increment by one minute the expiration time since
               // cron will only run once a minute.
               //TODO: INCREMENT TO 5 for now for debug purposes
               if (StringLib.getNextFullTimeStr(strNtcssDate, strExpirationTime,
                                           Constants.MINUTE_TYPE, 5) == false) {
            
                  Log.error("SvrProcLib", "nChangeJobSchedules", 
                            "Date incrementing function failed!" +
                        " The following schedules were not activated for job <" 
                        + processId + ">:  <" + strNtcssDate + ">");                     
                  return -5;
               }

               // This is an effort to get the strExpirationTime char array
               //  broken into a java.sql.Time string and java.sql.Date string
               tmpStr = new String(strExpirationTime);             
               exprDate = new GregorianCalendar();
               exprDate.set(2000 + Integer.parseInt(tmpStr.substring(0,2)), // year
                            Integer.parseInt(tmpStr.substring(2,4)) - 1,    // month
                            Integer.parseInt(tmpStr.substring(4,6)),        // day
                            Integer.parseInt(tmpStr.substring(7,9)),        // hour
                            Integer.parseInt(tmpStr.substring(9,11)),       // minute
                            Integer.parseInt(tmpStr.substring(11).trim())); // second
                                         
               tmpDate = exprDate.getTime();               
               validUntilDate = (new java.sql.Date(tmpDate.getTime())).toString();
               validUntilTime = (new Time(tmpDate.getTime())).toString();
            }
            
            
            // The normal schedule string has format:
            // fmmhhwmmdd
            switch (currentSchedule.charAt(0)) {
               
               case 'E':  //Exception          
                  // For now, exceptions are treated as one-shot schedules.
                  // Here, we don't care about day_of_week,
                  // or year since they are not handled by the scheduler GUI yet.
                  args = new Object[10];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = validUntilDate; //new String(strExpirationTime, 0, 6);
                  args[4] = validUntilTime; //new String(strExpirationTime, 7, 4) + "00";
                  args[5] = new Boolean(true);
                  args[6] = new Integer(currentSchedule.substring(8, 10));
                  args[7] = new Integer(currentSchedule.substring(6, 8));
                  args[8] = new Integer(currentSchedule.substring(3,5));
                  args[9] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_date,valid_until_time,exclusion,frequency," +
                    "year, month,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + ", " + Constants.ST + ", " + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + "," +
                    Constants.ST + ",'E',NULL, " + Constants.ST + "," + Constants.ST + 
                    ",NULL, " + Constants.ST + ", " + Constants.ST + ")", args); 
                  
                  
                  break;
                  
               case 'O':                   
                  // One shot so we don't care about day_of_week,
                  // or year since they are not handled distinctively by the
                  // scheduler GUI yet.
                  args = new Object[10];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = validUntilDate; //new String(strExpirationTime, 0, 6);
                  args[4] = validUntilTime; //new String(strExpirationTime, 7, 4) + "00";
                  args[5] = new Boolean(false);
                  args[6] = new Integer(currentSchedule.substring(8,10));
                  args[7] = new Integer(currentSchedule.substring(6,8));
                  args[8] = new Integer(currentSchedule.substring(3,5));
                  args[9] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_date,valid_until_time,exclusion,frequency," +
                    "year,month, day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + ", " + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + "," +
                    Constants.ST + ",'O',NULL," + Constants.ST + "," + 
                    Constants.ST + ",NULL," + Constants.ST + "," + 
                    Constants.ST + ")", args);                                        
                    
                  break;
          
               case 'M':  //Monthly          
                  // Here we don't care about year. day_of_week is ignored because
                  // it is not handled distinctively by the scheduler GUI yet.
                  args = new Object[7];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = new Integer(currentSchedule.substring(6,8));                  
                  args[5] = new Integer(currentSchedule.substring(3,5));
                  args[6] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + Constants.ST + 
                    ",NULL,NULL," + Constants.ST + ",'M',NULL," + 
                    Constants.ST + "," + "NULL," + Constants.ST + "," + 
                    Constants.ST + ")", args);                    
                  break;          
                  
               case 'W':  //Weekly          
                  // Here we don't care about year, month, or day_of_month.
                  args = new Object[7];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = new Integer(currentSchedule.substring(5,6));
                  args[5] = new Integer(currentSchedule.substring(3,5));
                  args[6] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + ",'W',NULL,NULL," + 
                    Constants.ST + "," + Constants.ST + "," + Constants.ST + 
                    ")", args);                                        
                  break;
                  
               case 'H':  //Hourly          
                  // Here we don't care about year, month, day_of_month,
                  // day_of_week, or hour.
                  args = new Object[5];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + ",'H',NULL,NULL," +
                    "NULL,NULL," + Constants.ST + ")", args);                                      
                  break;
                  
               case 'N':  //Every Minute          
                  // Here we don't care about anything!
                  args = new Object[4];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + 
                    ",'N',NULL,NULL,NULL,NULL,NULL)", args);                    
                  break;
               
               case 'Y': //Yearly          
                  // Here we don't care about day_of_week,
                  // or year since they are not handled distinctively by the
                  //  scheduler GUI yet.
                  args = new Object[8];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = new Integer(currentSchedule.substring(8,10));
                  args[5] = new Integer(currentSchedule.substring(6,8));
                  args[6] = new Integer(currentSchedule.substring(3,5));
                  args[7] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,month, day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + ",'Y',NULL," + 
                    Constants.ST + "," + 
                    Constants.ST + "," + "NULL," + Constants.ST + 
                    "," + Constants.ST + ")", args);                                        
                  break;
                  
               case 'D': //Daily
                  //Here we don't care about year since it is not handled
                  //distinctively by the scheduler GUI yet.  Don't care about
                  //month, date, or dow
                  args = new Object[6];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = new Integer(currentSchedule.substring(3,5));
                  args[5] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + "," + 
                    "'D',NULL,NULL,NULL," + Constants.ST + "," + Constants.ST + 
                    ")", args);                                        
                  break;                     
            
               default: //Something new so we just chuck in what we're given.          
                  //Here we don't care about year since it is not handled
                  //distinctively by the scheduler GUI yet.  We skip dow since the
                  // default action would be to treat it as a "one-shot job".
                  args = new Object[8];
                  args[0] = processId;
                  args[1] = nowDate;
                  args[2] = nowTime;
                  args[3] = new Boolean(false);
                  args[4] = currentSchedule.substring(0,1);
                  args[5] = new Integer(currentSchedule.substring(6,8));
                  args[6] = new Integer(currentSchedule.substring(3,5));
                  args[7] = new Integer(currentSchedule.substring(1,3));
                  SQLString = StringLib.SQLformat(
                    "INSERT INTO job_schedules (ntcss_job_id," +
                    "valid_after_date,valid_after_time," +
                    "valid_until_time,valid_until_date,exclusion,frequency," +
                    "year,day_of_month,day_of_week,hour,minute) " +
                    "VALUES (" + Constants.ST + "," + Constants.ST + "," + 
                    Constants.ST + ",NULL,NULL," + Constants.ST + "," + 
                    Constants.ST + ",NULL," + Constants.ST + "," + 
                    "NULL," + Constants.ST + "," + Constants.ST + ")", args);                                        
                  break;   
            }                
            
            //  Execute the insertion            
            statement.executeUpdate(SQLString);
            
            currentSchedStartIndex += Constants.MAX_SCHED_STR_LEN;
         } // End of "for" loop
         
         // Close the database resources
         statement.close();
         DatabaseConnectionManager.releaseConnection(connection);
         
         return 0;
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", "nChangeJobSchedules", exception.toString());
         return -1;
      }                 
   }              
   
   /**
    * Starts all the "boot" and "scheduled" type predefined jobs that are
    *  for applications that are on this host. If there are no errors, true
    *  is returned.
    */
   public static boolean startScheduledAndBootTypePDJs()
   {
      Connection connection;
      Statement statement;
      ResultSet resultSet;
      String fnct_name = "startScheduledAndBootTypePDJs";
      Object sqlItems[];      
      String token;                                   
      String message;
      String priority_or_schedule;
      int priority_or_schedule_size;
      String start_process_flag_string;
      String app_title;
      int type_of_job;
      String schedule_string;
      String command_line;
      String pdj_title;
      String batch_user;      
      boolean errorOccurred;
      
      try {                                
         
         // Get a DB connection.
         connection = DatabaseConnectionManager.getConnection();
         if (connection == null) {
            Log.error("SvrProcLib", fnct_name, "Null DB connection returned!");            
            return false;
         }
         statement = connection.createStatement();               
      
         // Get all "boot" and "scheduled" type predefined jobs of applications
         //  that are on this host. NOTE: If there isn't a batch user 
         //  associated with a predefined job's application, that record WON'T
         //  be returned here - which is desirable.
         sqlItems = new Object[2];
         sqlItems[0] = StringLib.getHostname().toUpperCase();
         sqlItems[1] = new Boolean(true);        
         resultSet = statement.executeQuery(StringLib.SQLformat(
                              "SELECT pdj_title, p.app_title, command_line, " +
                              "job_type, schedule_str, login_name FROM apps a, " +
                              "predefined_jobs p, app_users au WHERE upper(hostname) = " +
                              Constants.ST + " AND p.app_title = a.app_title" +
                              " AND (p.job_type = " + Constants.BOOT_JOB + 
                              " OR p.job_type = " + Constants.SCHEDULED_JOB + 
                              ") AND p.app_title = au.app_title " +
                              " AND au.batch_user = " + Constants.ST, sqlItems));                                                                                                                       
         
         // Loop through the predefined jobs returned sending each to the
         //  local command server to be run
         token = Constants.SERVER_TOKEN_STRING;                      
         while (resultSet.next() != false) {    
                        
            app_title = resultSet.getString("app_title");
            type_of_job = resultSet.getInt("job_type");
            schedule_string = resultSet.getString("schedule_str");
            command_line = resultSet.getString("command_line");
            pdj_title = resultSet.getString("pdj_title");
            batch_user = resultSet.getString("login_name");                                                   
           
            if (type_of_job == Constants.BOOT_JOB)  {
               priority_or_schedule       = String.valueOf(Constants.LOW_PRIORITY);
               priority_or_schedule_size  = 2;               
               
               // Pre-format these fields
               start_process_flag_string  = StringLib.valueOf(Constants.WAITING_FOR_EXEC_STATE, 2); 
               priority_or_schedule = StringLib.valueOf(Constants.LOW_PRIORITY, 
                                                        priority_or_schedule_size);
            }
            else  { // scheduled job            
               priority_or_schedule       = schedule_string;
               priority_or_schedule_size  = Constants.MAX_CUST_PROC_STAT_LEN;               
               
               // Pre-format these fields
               start_process_flag_string  = StringLib.valueOf(Constants.APPROVED_STATE, 2);
               priority_or_schedule = StringLib.padString(priority_or_schedule, 
                                               priority_or_schedule_size, ' ');
            }                             
                           
            // Create the message to send
            message = StringLib.padString(Constants.BASE_NTCSS_USER, Constants.MAX_LOGIN_NAME_LEN, ' ') +    
                StringLib.padString(token, Constants.MAX_TOKEN_LEN, ' ') +
                StringLib.padString(app_title, Constants.MAX_PROGROUP_TITLE_LEN, ' ') +
                start_process_flag_string + 
                priority_or_schedule +                 
                StringLib.padString(command_line, Constants.MAX_PROC_CMD_LINE_LEN, ' ') +
                StringLib.padString(batch_user, Constants.MAX_LOGIN_NAME_LEN, ' ') +
                StringLib.padString(pdj_title, Constants.MAX_JOB_DESCRIP_LEN, ' ') +
                StringLib.valueOf(Constants.UNCLASSIFIED_CLEARANCE, 2) +
                StringLib.padString("", Constants.MAX_PRT_NAME_LEN, ' ') +    // printer name
                StringLib.padString("", Constants.MAX_HOST_NAME_LEN, ' ') +   // printer host name
                StringLib.valueOf(Constants.UNCLASSIFIED_CLEARANCE, 2) + // prt classification
                StringLib.padString("", Constants.MAX_PRT_FILE_LEN, ' ') + // prt filename                       
                StringLib.valueOf(6, 2);   // Printer flag  bit 3 - restartable 
                                           //               bit 2 - save prt output
                                           //               bit1 - printer output
            
            if (type_of_job == Constants.BOOT_JOB) {
               message += "0" + // SDI required
                          "0" + // SDI i/o direction                                                  
                          StringLib.padString("", Constants.MAX_SDI_CTL_RECORD_LEN - 2, ' ') +  // SDI control record
                          StringLib.padString("", Constants.MAX_SERVER_LOCATION_LEN, ' ') + // SDI data file
                          StringLib.padString("", Constants.MAX_SERVER_LOCATION_LEN, ' ');  // SDI tci file        
            }            
            
            // This seemed to be an error in the StartServerBootProcess 
            //  function in the old code. This data was only sent if the type 
            //  was "boot", but this information is expected also by the 
            //  SCHEDSVRPROCBG handler.
            message += "00" + // printer orientation
                       "0"  + // banner size
                       "00" + // num copies
                       "00" + // papersize
                       "\0";
          
            // Send the message
            if (type_of_job == Constants.BOOT_JOB)  
               errorOccurred = sendStartPDJMessge(StringLib.formatMessage("LAUNCHSVRPROCBG", message), 30);
            else
               errorOccurred = sendStartPDJMessge(StringLib.formatMessage("SCHEDSVRPROCBG", message), 15);                        
           
            // Log error if one occurred
            if (errorOccurred == true)
               Log.error("SvrProcLib", fnct_name, "Error starting predefined job!" +
                         " PROGRAM GROUP: " + app_title + 
                         ", JOB TITLE: " + pdj_title + 
                         ", COMMAND LINE: " + command_line);  
                           
         } // End of while loop
         resultSet.close ();
         
         // Set any boot and scheduled type predefined jobs (for applications on
         //  this host) that have a schedule string, to "regular" pdj type in 
         //  the database.      
         statement.executeUpdate(StringLib.SQLformat(
             "UPDATE predefined_jobs SET job_type = " + Constants.REGULAR_JOB + 
             " WHERE predefined_jobs.app_title IN (SELECT predefined_jobs.app_title " +
             "FROM predefined_jobs, apps WHERE predefined_jobs.app_title= apps.app_title " + 
             "AND apps.hostname = " + Constants.ST + " AND " +
             "(job_type = " + Constants.BOOT_JOB + " OR job_type = " + 
             Constants.SCHEDULED_JOB + ") AND length(schedule_str) != 0)", 
             sqlItems));
         
         // Close the database resources
         statement.close ();
         DatabaseConnectionManager.releaseConnection(connection);                                                                                                           
                  
         return true;
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", fnct_name, exception.toString());
         return false;
      }
   }   
   
   /**
    *  Sends the given message to the local command server and waits for
    *   a response. This will only wait for "timeout" seconds for a response.
    *   If an error occurs or a "NAK" message is sent back, true is returned.
    */
   private static boolean sendStartPDJMessge(String messageToSend, int timeout)
   {      
      String fnct_name = "sendStartPDJMessge";
      byte buffer[] = new byte[50];
      int bytesRead;
      boolean errorOccurred;
      String response;
      Socket socket;
      BufferedOutputStream outputStream;
      BufferedInputStream inputStream;
      
      try {
                  
         // Create streams to send the message to local CMD_SVR            
         socket = new Socket("localhost", Constants.CMD_SVR_PORT);
         outputStream = new BufferedOutputStream(socket.getOutputStream());
         inputStream = new BufferedInputStream(socket.getInputStream());
            
         // Send given message         
         outputStream.write(messageToSend.getBytes(), 0, messageToSend.length());
         outputStream.flush();
                                 
         // Read the response
         socket.setSoTimeout(timeout * 1000);
         bytesRead = SocketLib.readStream(inputStream, buffer, buffer.length);         
            
         // Check the response
         errorOccurred = false; 
         if (bytesRead == -2) {
            Log.error("SvrProcLib", fnct_name, "Received timeout during read");
            errorOccurred = true;
         }
         else if (bytesRead >= Constants.CMD_LEN) {
               
            response = new String(buffer, 0, bytesRead);
            response = response.substring(0, Constants.CMD_LEN).trim();
               
            if (response.equals("GOODSVRPROC") == true)
               errorOccurred = false;
            else
               errorOccurred = true;
         }
         else
            errorOccurred = true;
         
         // Close the system resources
         socket.close();
         inputStream.close();
         outputStream.close();
            
         return errorOccurred;
      }
      catch (Exception exception) {
         Log.excp("SvrProcLib", fnct_name, exception.toString());
         return true;
      }
   }
   
   public static boolean notifyAppCallback(String appname, String exitstr, int status,
   String processID, String username)
   {
       
       Connection connection;
       Statement statement;
       ResultSet resultSet;
       Object sqlItems[];
       
       try {
           
           // Get a DB connection.
           connection = DatabaseConnectionManager.getConnection();
           if (connection == null) {
               Log.error("SvrProcLib", "notifyAppCallback", "Null DB connection returned!");
               return false;
           }
           statement = connection.createStatement();
           
           sqlItems = new Object[1];
           sqlItems[0] = appname;
           resultSet = statement.executeQuery(StringLib.SQLformat("SELECT psp_filename"
           + ",env_filename FROM apps WHERE "
           + "app_title = " + Constants.ST ,
           sqlItems));
           
           resultSet.next();
           
           String envFile = resultSet.getString(2);
           String pspFile = resultSet.getString(1);
           
           statement.close ();
           DatabaseConnectionManager.releaseConnection(connection);
           
           if(!FileOps.Stat(pspFile)){
               Log.warn("SvrProcLib", "notifyAppCallback",
               "Warning, Cleanup program <" + pspFile + "> does not" +
               " exist for progroup " + appname + " !");
               return false;
           }
           
           Runtime r = Runtime.getRuntime();
           String []cmdarray={Constants.PRELAUNCH_FILE,pspFile,username,
           NDSLib.getNtcssPassword(username),appname,StringLib.getHostname(),
           new Integer(status).toString(),exitstr};
           Process p = r.exec(cmdarray);
           return true;
       }
       
       catch(Exception exception) {
           Log.excp("CommandServer", "notifyAppCallback", exception.toString());
           return false;
       }
       
  }
}