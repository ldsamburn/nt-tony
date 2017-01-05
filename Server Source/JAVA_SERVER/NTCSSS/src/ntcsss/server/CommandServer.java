/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.server;

import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.Date;
import java.sql.Time;
import java.util.Date;
import java.text.*;
import java.io.*;
import java.net.InetAddress;
import java.net.Socket;
import java.lang.Runtime;
import java.lang.Process;
import java.util.*;
import COMMON.INI;
import ntcsss.log.EntityName;
import ntcsss.log.Log;
import ntcsss.libs.structs.ServerProcess;
import ntcsss.libs.*;
import ntcsss.tools.NtcssLp;
import ntcsss.database.DatabaseConnectionManager;
import ntcsss.util.FileOps;

public class CommandServer extends ServerBase
{
    protected String m_strMasterServer=null;
    private static Map RunningJobs = Collections.synchronizedMap(new HashMap());
    private static Map Apps = new HashMap();
    private static Timer timer = new Timer();

    private static Object JOB_INSERT_LOCK = new Object();
    
    public CommandServer(String messageData, Socket socket, ServerState serverState)
    {
        super(messageData, socket, serverState);
    }
    
    static 
    //static initializer
    {
        
        Connection connection;
        Statement statement;
        ResultSet resultSet;
        
        try {
            
            //annonymous innner class
            TimerTask checkSSP = new TimerTask() {
                public void run()
                {
                    SocketLib.sendFireForgetMessage(StringLib.getHostname(),
                    Constants.CMD_SVR_PORT,StringLib.padString("CHECKSCHEDSVRPROCS",
                    Constants.CMD_LEN,'\0'));
                }
            };
            
            timer.schedule(checkSSP, 60000, 60000); 
            
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "Static Initializer", exception.toString());
        }
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
            serverClass = CommandServer.class;
            
            // List the messages that this server expects along with the
            //  cooresponding message handler. Each message handler maps to
            //  a method in this class
            
            // All message handler methods must be public void methods that take
            //  no parameters
            
            // Each server should include the message handler for the SERVERSTATUS
            
            //  message. The handler should be the cooresponding method in the
            //  ServerBase class not a locally declared method.
            
            MessageHandlerDescription handlers[] =
            { new MessageHandlerDescription("KILLSVRPROC",
            serverClass.getMethod("KillServerProcess", null)),
            new MessageHandlerDescription("CHECKSCHEDSVRPROCS",
            serverClass.getMethod("CheckSchedSVRProcs", null)),
            new MessageHandlerDescription("CHECKWAITSVRPROCS",
            serverClass.getMethod("CheckAndStartWaitingJobs", null)),
            new MessageHandlerDescription("SCHEDSVRPROCBG",
            serverClass.getMethod("StoreSchedJob", null)),
            new MessageHandlerDescription("LAUNCHSVRPROCBG",
            serverClass.getMethod("LaunchBGProcess", null)),
            new MessageHandlerDescription("REAPER",
            serverClass.getMethod("Reaper", null)),
            new MessageHandlerDescription("LOCKAPP",
            serverClass.getMethod("LockApp", null)),
            new MessageHandlerDescription("PUSHNIS",
            serverClass.getMethod("PushNis", null)),
            new MessageHandlerDescription("GETDEVICES",
            serverClass.getMethod("GetDevices", null)),
            new MessageHandlerDescription("CHANGELOGIN",
            serverClass.getMethod("ChangeUserLogon", null)),
             new MessageHandlerDescription("GETDIRLISTING",
            serverClass.getMethod("GetDirListing", null)),
            new MessageHandlerDescription("GETFILEINFO",
            serverClass.getMethod("GetFileInfo", null)),
            new MessageHandlerDescription("GETFILESECTION",
            serverClass.getMethod("GetFileSection", null)),
            new MessageHandlerDescription("DELETESVRFILE",
            serverClass.getMethod("DeleteSvrFile", null)),
            new MessageHandlerDescription("GETMASTERNAME",
            serverClass.getMethod("GetMasterName", null)),
            new MessageHandlerDescription("GETHOSTINFO",
            serverClass.getMethod("GetHostInfo", null)),
            new MessageHandlerDescription("SETPROCDEV",
            serverClass.getMethod("SetProcDev", null)),
            new MessageHandlerDescription("SETPRTSTATUS",
            serverClass.getMethod("SetPrinterStatus", null)),
            new MessageHandlerDescription("GETPRTSTATUS",
            serverClass.getMethod("GetPrinterStatus", null)),
            new MessageHandlerDescription("SERVERSTATUS",
            serverClass.getMethod("serverStatus", null)),
            new MessageHandlerDescription("PRINTSVRFILE",
            serverClass.getMethod("PrintServerFile", null)),
            new MessageHandlerDescription("GETPRINTERINI",
            serverClass.getMethod("getPrinterIni", null)),
            new MessageHandlerDescription("PUTPRINTERINI",
            serverClass.getMethod("putPrinterIni", null)),
            new MessageHandlerDescription("VERIFYUSER",
            serverClass.getMethod("VerifyUserLogon", null)),
            new MessageHandlerDescription("GETSERVERTYPE",
            serverClass.getMethod("GetServerType", null)),
            new MessageHandlerDescription("GETNTAPPLIST",
            serverClass.getMethod("GetNTAppList", null)),
            new MessageHandlerDescription("LAUNCHSVRPROCINTER",
            serverClass.getMethod("LaunchInteractiveProc", null)), 
            new MessageHandlerDescription("SETPROCLIMIT",
            serverClass.getMethod("setProcessLimits", null)) };
            // Return the information concerning this class
            return new ServerDescription("Command Server", serverClass, handlers,
            EntityName.CMD_SVR_GRP);
        }
        catch (Exception exception) {
            Log.excp("CommandServer", "getServerDescription", exception.toString());
            return null;
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    //     KILLSVRPROC MSG HANDLER
    
    public void KillServerProcess()throws MyException
    {
        
        String SQLString;
        Connection connection = null;
        Statement statement = null;
        ResultSet resultSet = null;
        int errorCode = 0;
        int dataItemLengths[];
        int pid=0;
        String dataItems[];
        String loginName = null;
        String token=null;
        String SignalStr=null;
        String ProcessID=null;
        String progroupTitle = null;
        Integer uid = null;
        int appSignal;
        int signal;
        
        try{
            
            // Check to see if this server is idled
            if (isServerIdled() == true)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.ERROR_SERVER_IDLED));
            
            // Get the client data
            dataItemLengths = new int[5];
            dataItems = new String[5];
            
            // Get login name, token, process id, and signal type.
            
            dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
            dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
            dataItemLengths[2]  = Constants.MAX_PROCESS_ID_LEN;
            dataItemLengths[3]  = 2; // signal type
            dataItemLengths[4]  = Constants.MAX_PROGROUP_TITLE_LEN;
            
            if (parseData(5, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_PARSE_MSG));
            
            loginName      = dataItems[0];
            token          = dataItems[1];
            ProcessID      = dataItems[2];
            SignalStr      = dataItems[3];
            progroupTitle  = dataItems[4];
            
            Log.debug("CommandServer", "KillServerProcess",
            "past parse data process_id-> " + ProcessID + " App-> " +
            progroupTitle);
            
            if (token.regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()) == true)
                errorCode=doServerValidate(loginName, token,
                getClientAddress());
            else
                errorCode = isUserValid(loginName, token, new ntcsss.util.IntegerBuffer());
            
            if (errorCode <0)
                throw new MyException(ErrorCodes.getErrorMessage(errorCode));
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes
                .ERROR_CANT_OPEN_DB_FILE));
            
            Object items[]=new Object[2];
            items[0] = ProcessID;
            items[1] = progroupTitle;
/*            
            SQLString = StringLib.SQLformat("SELECT pid FROM active_jobs WHERE " +
            "ntcss_job_id = " + Constants.ST + " AND app_title = " +
            Constants.ST,items);
 */
            SQLString = StringLib.SQLformat("SELECT pid, termination_signal " +
            "FROM active_jobs, apps WHERE ntcss_job_id = " + Constants.ST + 
            " AND active_jobs.app_title = " + Constants.ST + 
            " AND active_jobs.app_title = apps.app_title", items);
            
            Log.debug("CommandServer", "KillServerProcess",
            SQLString);
            
            //  Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(SQLString);
            
            if(!resultSet.next())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_FIND_VALUE));
            
            // Get the data
            pid=resultSet.getInt("pid");
            
            // NOTE: If this value is null in the DB, 0 is returned           
            if ((appSignal = resultSet.getInt("termination_signal")) == 0)
               appSignal = 16;  // SIGUSR1 on unix
            
            resultSet.close();  resultSet = null;
            statement.close();  statement = null;
            DatabaseConnectionManager.releaseConnection(connection);
              connection = null;
            
            if (StringLib.parseInt(SignalStr, 0) != 0)  // i.e. hard kill
               signal = 9; // SIGKILL on unix
            else // i.e. soft kill
               signal = appSignal;
            
            if(pid>0){ //good PID
                Process p = (Process) RunningJobs.get(ProcessID);
                OutputStream out = p.getOutputStream();
                Log.debug("CommandServer", "KillServerProcess",
                          "Killing proess-> " + ProcessID); 
                
                BufferedWriter w = new BufferedWriter(new OutputStreamWriter(out));
                w.write("DIE");
                w.newLine();
                w.close();
                RunningJobs.remove(ProcessID);
                sendServerMsg(StringLib.padString("GOODKILLSVRPROC", 
                Constants.CMD_LEN,'\0'));
                //TODO: make sure wacking is the thing to do
//                RemovePIDs(ProcessID,true);
                return;
            }
            else
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_KILL_SVR_PROC));
        }
        
        catch(MyException exception){
            Log.excp("CommandServer", "KillServerProcess",exception.toString());
            
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
            
            SendErrorResponse("NOKILLSVRPROC", exception.toString());
            return;
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "KillServerProcess",exception.toString());
            SendErrorResponse("NOKILLSVRPROC", exception.toString());
            return;
        }
    }
   
    /////////////////////////////////////////////////////////////////////////
    //                     CHECKSCHEDSVRPROCS MSG HANDLER
    //
    // This function determines whether it is time for a scheduled job to be
    // launched.  If it is, a copy of the job is put in the active_jobs table
    // and it is launched.
    
    public void CheckSchedSVRProcs()
    {
        String strSQL=null;
        int    i;
        int    num_errors = 0;
        Connection connection;
        Statement statement;
        ResultSet resultSet;
        
        try  {
           
            // Check if server is idled. 
            if (isServerIdled() == true) 
               return; 
            
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "CheckSchedSVRProcs",
                "Failed to Get a database connection");
                return;
            }
            
            SimpleDateFormat sdf = new SimpleDateFormat();
            java.util.Date now = new java.util.Date();
            sdf.applyPattern("dd");
            String DayofMonth=sdf.format(now);
            sdf.applyPattern("yy");
            String Year=sdf.format(now);
            sdf.applyPattern("MM");
            String Month=sdf.format(now);
            sdf.applyPattern("HH");
            String Hour=sdf.format(now);
            sdf.applyPattern("mm");
            String Min=sdf.format(now);
            
            Calendar calendar = new GregorianCalendar();
            calendar.setTime(now);
            
            // Subtract one because we store day of weeks with SUNDAY = 0,
            //  but Calendar returns day of week with SUNDAY = 1
            String DOW = StringLib.valueOf(calendar.get(Calendar.DAY_OF_WEEK) - 1,2);
            
             // Create SQL fields that represent the current time
             long nowMilliseconds = now.getTime();
             String nowDate = (new java.sql.Date(nowMilliseconds)).toString();
             String nowTime = (new Time(nowMilliseconds)).toString();
             
            //unfortunatly need 2 queries to make sure NONE of these 
            //guys are in there once with an exclusion... so, Check all exclusions
            //and stash these for a NOT IN in the next query
             
            strSQL = "SELECT DISTINCT a.ntcss_job_id FROM job_schedules a, " +
            "scheduled_jobs b WHERE (a.ntcss_job_id = b.ntcss_job_id) " +
            " AND pid = " + Constants.APPROVED_STATE + " AND " +
            " (exclusion = true)" + " AND ((valid_until_date IS NULL)" +       
            " OR ((date'" + nowDate + "') < (valid_until_date))" +
            " OR (((date'" + nowDate + "') = (valid_until_date))" +
            " AND ((time'" + nowTime + "') < valid_until_time)))" +
            " AND (((date'" + nowDate + "') > (valid_after_date))" +// check sched expires.
            " OR (((date'" + nowDate + "') = (valid_after_date))" +
            " AND((time'" + nowTime + "') >= (valid_after_time))))" +
            " AND ((year IS NULL) OR (year = " + Year + "))" +
            " AND ((month IS NULL) OR (month = " + Month  + "))" +
            " AND ((day_of_month IS NULL) OR (day_of_month = " +
            DayofMonth + ")) AND ((day_of_week IS NULL) OR "
            + "(day_of_week = " + DOW + ")) AND ((hour IS NULL)"
            + " OR (hour = " + Hour + ")) AND ((minute IS NULL) OR " +
            "(minute = " + Min + "))";
            
            //  Execute the 1ST query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(strSQL);
            String sql2=null;
            
            while(resultSet.next()) 
               if(sql2 == null)
                   sql2 = "'" + resultSet.getString("ntcss_job_id") + "'";
               else
                    sql2 += ",'" + resultSet.getString("ntcss_job_id") + "'";
   
            if(sql2!=null)
                sql2 += "))";
            
            // Retrieve the list of scheduled jobs that should run this minute..
            //Excluding the guys in the first query
            
            strSQL="SELECT DISTINCT a.ntcss_job_id FROM job_schedules a, " +
            "scheduled_jobs b WHERE (a.ntcss_job_id = b.ntcss_job_id) " +
            " AND pid = " + Constants.APPROVED_STATE + " AND ((exclusion = false)" +
            " AND ((valid_until_date IS NULL)" +
            " OR ((date'" + nowDate + "') < (valid_until_date))" +
            " OR (((date'" + nowDate + "') = (valid_until_date))" +
            " AND ((time'" + nowTime + "') < valid_until_time)))" +
            " AND (((date'" + nowDate + "') > (valid_after_date))" + // check sched expires.
            " OR (((date'" + nowDate + "') = (valid_after_date))" +
            " AND((time'" + nowTime + "') >= (valid_after_time))))" +
            " AND ((year IS NULL) OR (year = " + Year   +
            " )) AND ((month IS NULL) OR (month = " + Month  +
            " )) AND ((day_of_month IS NULL) OR (day_of_month = " +
            DayofMonth + " )) AND ((day_of_week IS NULL) OR " +
            "(day_of_week = " + DOW + " )) AND ((hour IS NULL) "+
            "OR (hour = " + Hour  + ")) AND ((minute IS NULL) OR " +
            "(minute = " + Min  + "))";
            
            if(sql2!=null)
//                strSQL += " AND ntcss_job_id NOT IN (" + sql2;
                strSQL += " AND a.ntcss_job_id NOT IN (" + sql2;
            else
                strSQL += ")";
            
            Log.debug ("CommandServer", "CheckSchedSVRProcs", strSQL);
            
            resultSet = statement.executeQuery(strSQL);
         
            if(!resultSet.first()) {
                
                resultSet.close();
                statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
                Log.debug ("CommandServer", "CheckSchedSVRProcs",
                "Found no Jobs to run ");
                return;
                
            }
            
            Log.debug ("CommandServer", "CheckSchedSVRProcs",
                "Found Job to run-> " + resultSet.getString(1));
            
            //put the result set into active jobs table
            TransferScheduledJobs(resultSet);
            
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
        } //try
        
        catch (Exception exception){
            Log.excp("CommandServer", "CheckSchedSVRProcs",
            exception.toString());
            Log.excp("CommandServer", "CheckSchedSVRProcs",
            strSQL);
            
            return;
        }
        
        // Clean out any old schedules that have expired.
        RemoveExpiredJobSchedules();
        
        return;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // This function takes a pointer to a hot database connection with the
    // ntcss_job_ids of scheduled jobs already queried
    // to be launched.  The id's are incremented if necessary and copies of the
    // record(s) in the scheduled_jobs table are placed in the active_jobs table.
    // I changed this to put them all in as WAITING_FOR_EXEC_STATE & then just post
    // CHECKWAITSERVERPROCS (all groups) message so that the process limits can be
    // applied old code didn't look like that was the case
    
    protected void TransferScheduledJobs(ResultSet db_jobs_to_launch)
    {
        
        Connection connection;
        Statement statement;
        ResultSet resultSet;
        String strBaseID;
        String strNtcssDateTime;
        String strNewID;
        Object Items[];
        
        
        try {
            
            Items = new Object[34];
            
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "TransferScheduledJobs",
                "Failed to Get a database connection");
                return;
            }
            
            statement = connection.createStatement();
            db_jobs_to_launch.beforeFirst();
            
            while(db_jobs_to_launch.next()) {
                
                strBaseID=db_jobs_to_launch.getString(1);
                // First, we need to increment the last three digits if there are other
                // copies of this scheduled job that have run.
                
                //011102.154419rockh000
                
                // Need to break out the UserName && the time before we call getNewJobID()
                strNewID=getNewJobID(strBaseID.substring(0,13),strBaseID.substring(13,
                strBaseID.length()-3),"active_jobs",connection);
                
                if(strNewID==null) {
                    Log.warn("CommandServer", "TransferScheduledJobs",
                    "Failed to Get a new Ntcss Process ID");
                    
                    PostJobFailureMsg(db_jobs_to_launch.getString(1),
                    "Failed to Get a new Ntcss Process ID","scheduled_jobs",
                    connection);
                    continue;
                }
                
//                Items[0] = strNewID;
                Items[0] = strBaseID;
                
                String strSQL = StringLib.SQLformat("SELECT * FROM scheduled_jobs"
                + " WHERE ntcss_job_id = " + Constants.ST,Items);
                
                resultSet = statement.executeQuery(strSQL);
                if(!resultSet.first())
                    return;
                
                Items[0] = strNewID;
                Items[1] = resultSet.getString("command_line");
                Items[2] = new Integer(Constants.MEDIUM_PRIORITY);
                Items[3] = new Integer(resultSet.getInt("security_class"));
                Items[4] = resultSet.getString("schedule_string");
                Items[5] = resultSet.getString("job_initiator");
                Items[6] = resultSet.getString("job_requestor");
                Items[7] = resultSet.getString("app_title");
                Items[8] = resultSet.getString("job_description");
                Items[9] = resultSet.getString("request_time");
                Items[10] = resultSet.getString("launch_time");
                Items[11] = resultSet.getString("end_time");
                Items[12] = new Integer(Constants.WAITING_FOR_EXEC_STATE);
                Items[13] = new Integer(0); //exit state
                Items[14] = resultSet.getString("originating_host");
                Items[15] = new Boolean(resultSet.getBoolean("restartable"));
                Items[16] = resultSet.getString("custom_job_status");
                Items[17] = new Boolean(resultSet.getBoolean("print_file_switch"));
                Items[18] = resultSet.getString("printer_name");
                Items[19] = new Integer(resultSet.getInt("printer_sec_class"));
                Items[20] = resultSet.getString("print_filename");
                Items[21] = new Boolean(resultSet.getBoolean("remove_print_file"));
                Items[22] = new Integer(resultSet.getInt("print_num_copies"));
                Items[23] = new Boolean(resultSet.getBoolean("print_banner_switch"));
                Items[24] = new Integer(resultSet.getInt("print_page_size"));
                Items[25] = new Boolean(resultSet.getBoolean("orient_portrait"));
                Items[26] = new Boolean(resultSet.getBoolean("sdi_required"));
                Items[27] = resultSet.getString("sdi_ctrl_rec");
                Items[28] = resultSet.getString("sdi_data_file");
                Items[29] = resultSet.getString("sdi_tci_file");
                Items[30] = resultSet.getString("sdi_device");
                Items[31] = new Boolean(resultSet.getBoolean("sdi_read"));
                Items[32] = new Boolean(resultSet.getBoolean("sdi_write"));
                Items[33] = new Boolean(resultSet.getBoolean("sdi_failed"));
                
                strSQL = StringLib.SQLformat("INSERT INTO ACTIVE_JOBS " + 
                "VALUES(" + Constants.ST + "," + Constants.ST + "," + Constants.ST + 
                 "," + Constants.ST + "," + Constants.ST + "," + Constants.ST + 
                 "," + Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," +
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," +
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," +  
                 Constants.ST + "," + Constants.ST + "," + Constants.ST + "," + 
                 Constants.ST + ")", 
                 Items);
                
                //34

                // Copy the scheduled job into the active_jobs table with the new ID and
                // exit_status fields..
                /*String strSQL = StringLib.SQLformat("INSERT INTO ACTIVE_JOBS (SELECT "
                + Constants.ST + " AS " + "ntcss_job_id,command_line," +
                Constants.ST + " AS priority,security_class,schedule_string," +
                "job_initiator,job_requestor,app_title,job_description,request_time" +
                ",launch_time,end_time, " + Constants.ST + " AS pid, " + 
                Constants.ST + " AS exit_status," + "originating_host, " + 
                "restartable,custom_job_status,print_file_switch," +
                "printer_name,printer_sec_class,print_filename,remove_print_file," +
                "print_num_copies,print_banner_switch,print_page_size," +
                "orient_portrait,sdi_required,sdi_ctrl_rec,sdi_data_file," +
                "sdi_tci_file,sdi_device,sdi_read,sdi_write,sdi_failed " +
                "FROM scheduled_jobs WHERE ntcss_job_id = " + Constants.ST + ")",
                Items); McKoi can't handle this type of statements */ 
                
                //  Execute the query
                statement.executeUpdate(strSQL);
              
            } //while
           
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            //post checkwaitserverproc here to run as many as possible
            SocketLib.sendFireForgetMessage(StringLib.getHostname(),
                    Constants.CMD_SVR_PORT,StringLib.padString("CHECKWAITSVRPROCS",
                    Constants.CMD_LEN,'\0') + StringLib.padString("all groups", 
                                          Constants.MAX_PROGROUP_TITLE_LEN, ' '));
            
        }   //try
        
        catch (Exception exception){
            Log.excp("CommandServer", "TransferScheduledJobs",
            exception.toString());
            return;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // This goes through the job_schedules table and removes any schedules
    // that have expired.  For now, this function should be called AFTER all the
    // schduled jobs haverun since we haven't fully implemented the proper
    //  incrementation of the valid_until_... fields to represent expiration times.
    
    protected void RemoveExpiredJobSchedules()
    {
        
        Connection connection;
        Statement statement;
        
        try {
            
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "RemoveExpiredJobSchedules",
                "Failed to Get a database connection");
                return;
            }
            
             long nowMilliseconds = System.currentTimeMillis();
             String nowDate = (new java.sql.Date(nowMilliseconds)).toString();
             String nowTime = (new Time(nowMilliseconds)).toString();
            
            // For one-shot jobs we never did figure out how to do a 1-day increment on
            // dates so we left the valid_until fields NULL. For one shots,
            statement = connection.createStatement();
            statement.executeUpdate("DELETE FROM job_schedules WHERE frequency = 'O'" +
            " AND (valid_until_date IS NOT NULL)" +
            " AND ((date'" + nowDate + "') >= (valid_until_date))" +
            " AND (valid_until_time IS NOT NULL)" +
            " AND ((time'" + nowTime + "') >= valid_until_time)");
            
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
        }
        catch (Exception exception){
            Log.excp("CommandServer", "RemoveExpiredJobSchedules",
            exception.toString());
            return;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // This function posts a message to the relevant application to let the
    // administrator know that a job failed to run.  The table_name supplied
    // should contain a record of the job in question so pertinient information
    // about it can be displayed with the error message for identification.
    
    protected void PostJobFailureMsg(String strJobID,String strCustomMsg,
    String strTableName,Connection connection)
    {
        
        Statement statement;
        ResultSet resultSet;
        Object items[]=new Object[1];
        items[0]=strJobID;
       
        try
        {
            
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "PostJobFailureMsg",
                "Failed to Get a database connection");
                return;
            }
            
            //  Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT * FROM "
            + strTableName + " WHERE ntcss_job_id = " + Constants.ST,
            items));
            
            if(!resultSet.first()){
                Log.warn("CommandServer", "PostJobFailureMsg",
                "Unable to post a message regarding job failure!");
                resultSet.close();
                statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
                return;
            }
            
            String strMsg= strCustomMsg + "\nJob Details:\n\t ID = " + strJobID +
            "\n\t Description = <" + resultSet.getString("job_description") + ">"
            + "\n\t Application Group = " + resultSet.getString("app_title") +
            "\n\t Unix Job Owner = " + resultSet.getString("job_requestor");
            
            postSystemMessage(StringLib.getHostname(), resultSet.getString
            ("job_requestor"),resultSet.getString("app_title"),
            strMsg,Constants.SYS_MSG_BB_PROCESS_TERMINATED);
            
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "RemoveExpiredJobSchedules",
            exception.toString());
            return;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Called in NtcssStart with "all groups", SPQSERVER by APP, Internally by
    // TarnsferJobs (from CheckScheduled Procs) && Reaper by APP
    //                       CHECKWAITSVRPROCS MSG HANDLER
    
    public void CheckAndStartWaitingJobs() throws MyException
    {
        
        int dataItemLengths[];
        String dataItems[];
        Connection connection;
        Statement statement;
        ResultSet resultSet;
        String ID;
        String strRet;
        
        try{
            
            // Check to see if this server is idled
            if (isServerIdled() == true)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            // Get the client data
            dataItemLengths = new int[1];
            dataItems = new String[1];
            
            // Get Apptitle
            dataItemLengths[0]  = Constants.MAX_PROGROUP_TITLE_LEN;
            
            if (parseData(1, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            String AppName = dataItems[0];
            String strThisHost = StringLib.getHostname();
            
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_OPEN_DB_FILE));
           
            if(AppName.equals("all groups")){
                //doing this in 2 parts so that RUN_NEXT accross apps 

                
                Object items[]=new Object[1];
                items[0]=new Integer(Constants.WAITING_FOR_EXEC_STATE);
                
                statement = connection.createStatement();
               
                resultSet = statement.executeQuery(StringLib.SQLformat("SELECT" 
                + " DISTINCT app_title FROM active_jobs WHERE pid = " + 
                Constants.ST,items));
                
                 while(resultSet.next()) 
                    while((ID=getNextServerProcess(resultSet.getString(1),
                                                   connection))!=null) 
                        if((strRet=launchJob(ID,connection))!=null) 
                           RemovePIDs(ID,false); //sets it to crashed
              
          
                resultSet.close();
                statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
                
                return;
                
            }
            
            //else it's by app
           
            while((ID=getNextServerProcess(AppName, connection))!=null)
                  if((strRet=launchJob(ID,connection))!=null) 
                           RemovePIDs(ID,false); //sets it to crashed
          
            DatabaseConnectionManager.releaseConnection(connection);

        } //try
        
        catch (MyException exception){
            Log.warn("CommandServer", "CheckAndStartWaitingJobs",
            exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "CheckAndStartWaitingJobs",
            exception.toString());
        }
        
        return;
    }
    
 
      
    protected String getNextServerProcess(String strApp,Connection connection)
    {
        
        Statement statement;
        ResultSet resultSet;
        String strProcID=null;
        int nCurrentPriority;
        long nCurrentQueuePriority;
        
        try{
            
            if(!CheckProcessLimits(strApp,connection))
                return null;
            
            Object items[]=new Object[2];
            
            items[1]=new Integer(Constants.WAITING_FOR_EXEC_STATE);
            items[0]=strApp;
            
            // Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT "
            + "ntcss_job_id, priority, request_time FROM active_jobs WHERE "
            + "app_title = " +  Constants.ST + " AND pid = " + Constants.ST
            ,items));
            
            if(!resultSet.next()) {
                resultSet.close();
                statement.close();
                return null;
            }
            
            resultSet.beforeFirst();
            
            String strNtcssTime=StringLib.getFullTimeStr();
            
            // Blank out the decimal in the string so the buffer looks like an int..
            strNtcssTime=strNtcssTime.replace('.','0');
            
            Long nTimeNow = new Long(strNtcssTime);
            
            long nHighestQueuePriority = -10;                       // Default value.
            
            // Seek the highest priority job in the queue and record its ID..
            while(resultSet.next()) {
                strNtcssTime = resultSet.getString(3);
                nCurrentPriority = resultSet.getInt(2);
                strProcID=null;
                
                // Overwrite the decimal so the time string looks like an int..
                strNtcssTime=strNtcssTime.replace('.','0');
                
                Long nCurrentRequestTime = new Long(strNtcssTime);
                
 
                // This equation considers 2 factors: job priority and age.
                // Each of these is multiplied by a configurable weight coefficient and
                // the sum is used to determine the highest priority job in the queue.
                //  Currently, the Age factor is 1 and that means that we're really
                // thinking in terms of age.
                // The Priority weight essentially just adds seconds or minutes or
                // hours or days or months or years to the age.
                // So if (JOB_Q_PRIORITY_WEIGHT * priority) totals less than 100,
                //  it really
                // only adds seconds to the age. Less than 10,000 essentially adds
                //  minutes.  Less than 1,000,000 essentially adds hours and so on.
                // just adds
                nCurrentQueuePriority = (nCurrentPriority *
                Constants.JOB_Q_PRIORITY_WEIGHT) +
                ((nTimeNow.longValue() - nCurrentRequestTime.longValue()) *
                Constants.JOB_Q_AGE_WEIGHT);
                
                if (nHighestQueuePriority < nCurrentQueuePriority) {
                    nHighestQueuePriority = nCurrentQueuePriority;
                    strProcID=resultSet.getString(1);
                }
            }
            
            resultSet.close();
            statement.close();
            
            return nHighestQueuePriority == -10 ? null : strProcID;
          
        } //try
        
        catch (Exception exception){
            Log.excp("CommandServer", "getNextServerProcess",
            exception.toString());
            return null;
        }
        
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    //    only called in checkprocesslimits
    //
    protected int JobSlotsOpen(int nAppLimit, int nHostLimit, String strApp,
    Connection connection)
    {
        int num_app_procs = 0;
        int num_host_procs = 0;
        String strSQL;
        Statement statement;
        ResultSet resultSet;
        
        try {
           
            // First find out how many jobs are running on this host..
            strSQL="SELECT count(ntcss_job_id) FROM active_jobs WHERE " +
            "pid > 0 OR pid = " + Constants.ABOUT_TO_BE_RUN_STATE;
           
            //  Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(strSQL);
           
             if(resultSet.first())
                num_host_procs = resultSet.getInt(1);
            
            // If too many jobs are running on this host already, we can't run more..
             if (num_host_procs >= nHostLimit){
                resultSet.close();
                statement.close();
                return 0;
             }
             
            // Next, find out how many jobs are already running/about to run
            // under this app..
            
            Object items[]=new Object[2];
            items[0]=new Integer(Constants.ABOUT_TO_BE_RUN_STATE);
            items[1]=strApp;
            
            strSQL=StringLib.SQLformat("SELECT COUNT (ntcss_job_id) FROM " + 
            "active_jobs WHERE (pid > 0 OR pid = " + Constants.ST + 
            ") AND (app_title = " + Constants.ST + ")",items);
           
            //  Execute the query
            resultSet = statement.executeQuery(strSQL);
            
            if(resultSet.first())
                num_app_procs = resultSet.getInt(1);
            
            resultSet.close();
            statement.close();
         
            // If too many jobs are running under this app already, we can't run more..
            if (num_app_procs >= nAppLimit)
                return 0;
            
            // Return the smaller of the application slots & host slots still open..
            if ((nHostLimit - num_host_procs) < (nAppLimit - num_app_procs))
                return (nHostLimit - num_host_procs);
            else
                return (nAppLimit - num_app_procs);
        } //try
        
        catch (Exception exception){
            Log.excp("CommandServer", "JobSlotsOpen",
            exception.toString());
            return 0;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////
    // This just adds a job to the table specified and properly inserts NULL
    // for the empty fields.
    //
    public String AddJob(String strTable, ServerProcess SP)
    
    {
        
        Connection connection;
        Statement statement;
        String strNtcssProcID;
        boolean bScheduledJob;
        
        try {
           
           // Getting a unique job id and adding it to the database needs to
           //  be an atomic action, so this block is synchronized
                       
           // Synchronizing this method will not provide the safety we need 
           //  since this method is just an
           //  instance method. We need to lock across all instances of the
           //  CommandServer class (JOB_INSERT_LOCK is appropriatly a class 
           //  member, not an instance member).
           synchronized (JOB_INSERT_LOCK) {
              
            if((connection = DatabaseConnectionManager.getConnection())==null)
                return "Can't access the Database";            
            
            bScheduledJob=(strTable.equals("scheduled_jobs"));                        
            
            if((SP.process_id=getNewJobID(SP.request_time,SP.login_name,strTable,
            connection))==null){
                Log.warn("CommandServer", "AddJob",
                "Error creating a unique job ID for job" + SP.job_descrip);
                return null;
            }            
            
            Object items[]=new Object[34];
            
            items[0] = SP.process_id;
            items[1] = SP.command_line;
            items[2] = new Integer(SP.priority);
            items[3] = new Integer(SP.security_class);
            items[4] = SP.cust_proc_status;
//            items[5] = SP.login_name;
//            items[6] = SP.unix_owner;
            items[5] = SP.unix_owner;
            items[6] = SP.login_name;            
            items[7] = SP.progroup_title;
            items[8] = SP.job_descrip;
            items[9] = SP.request_time;
            items[10] = SP.launch_time;
            items[11] = SP.end_time;
            items[12] = new Integer(SP.pid);
            items[13] = new Integer(SP.proc_status);
            items[14] = SP.orig_host;
            items[15] = new Boolean((SP.prt_flag & Constants.PRT_FLAG_MASK3)!=0);
            items[16] = SP.cust_proc_status;
            items[17] = new Boolean((SP.prt_flag & Constants.PRT_FLAG_MASK1)!=0);
            items[18] = SP.prt_name.length() == 0 ? null : SP.prt_name;
            items[19] = new Integer(SP.prt_sec_class);
            items[20] = SP.prt_filename.length() == 0 ? null : SP.prt_filename;
            items[21] = new Boolean((SP.prt_flag & Constants.PRT_FLAG_MASK2)!=0);
            items[22] = new Integer(SP.copies);
            items[23] = SP.banner == 1 ? new Boolean(true) : new Boolean(false);
            items[24] = new Integer(SP.papersize);
            items[25] = SP.orientation == 1 ? new Boolean(true) : new Boolean(false);
            items[26] = new Boolean((SP.prt_flag & Constants.PRT_FLAG_MASK5)!=0);
            items[27] = SP.sdi_ctl_rec;
            items[28] = SP.sdi_data_file;
            items[29] = SP.sdi_tci_file;
            items[30] = SP.sdi_device;
            if((SP.prt_flag & Constants.PRT_FLAG_MASK4)!=0) {
                items[31]=new Boolean(true);
                items[32]=new Boolean(false);
            }
            else {
                items[31]=new Boolean(false);
                items[32]=new Boolean(true);
            }
            
            items[33] = new Boolean((SP.prt_flag & Constants.PRT_FLAG_MASK6)!=0);
            
            //Execute Update
            statement = connection.createStatement();
            String strSQL=StringLib.SQLformat("INSERT INTO " + strTable +
            " VALUES ("
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + "," + Constants.ST + "," + Constants.ST + ","
            + Constants.ST + ")",items);
 
            statement.executeUpdate(strSQL);
            
            statement.close();
            
            DatabaseConnectionManager.releaseConnection(connection);
            
           } // End of synchronized block
            
            if (bScheduledJob)
                // add and replace all schedules for this job..
                SvrProcLib.nChangeJobSchedules(SP.process_id,SP.cust_proc_status);        
            
        } //try
        
        catch (Exception exception){
            Log.excp("CommandServer", "AddJob",
            exception.toString());
            return null;
        }
        
        return SP.process_id;
    } 
    
    ///////////////////////////////////////////////////////////////////////
    //                   getNewJobID pass table to make it usable for
    //                   for boff tables
    //                   called in TransferScheduledJobs && addjob
    
    public String getNewJobID(String strRequestTime,String strLoginName,
    String strTable, Connection connection)
    {
        Statement statement=null;
        ResultSet resultSet=null;
        boolean bFound=false;
        String strNewJobID=null,strOldJobID=null;
        int nCnt=0;
        
        try {
/*  A database connection is always passed in          
            if((connection = DatabaseConnectionManager.getConnection())==null) {
                Log.warn("CommandServer", "getNewJobID","Could not access " +
                "Database");
                return null;
            }
 */
            
            Object items[]=new Object[1];
            items[0] = strRequestTime + strLoginName + "%";
            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat("SELECT " +
            "ntcss_job_id FROM " + strTable + " WHERE ntcss_job_id LIKE " +
            Constants.ST + " ORDER BY ntcss_job_id",items));
            
            if(!resultSet.first()) 
                return strRequestTime + strLoginName + "000";
            
            if(resultSet.last()) {
                String ID = resultSet.getString(1);
//                Integer oldID=new Integer(ID.substring(18,ID.length()));
                Integer oldID=new Integer(ID.substring(ID.length() - 3,ID.length()));
                return strRequestTime + strLoginName + StringLib.valueOf(
                                                       (oldID.intValue()+1),3);
            }
            
            else {
                Log.warn("CommandServer", "getNewJobID","Could not generate new " +
                "NTCSS JobID");
                return null;
            }
            
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "getNewJobID",
            exception.toString());
            return null;
        }
        
        finally {
            try {
                
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
            }
            catch (Exception exception){
                Log.excp("CommandServer", "getNewJobID",
                exception.toString());
            }
        }
        
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    //                   "SCHEDSVRPROCBG" MSG HANDLER
    //
    // Just add record to Scheduled Job Table
    // This function was originally meant to process a schedule_job request message
    //  sent from the the Batch Job GUI.  It parses a message into the old
    //  server process data structure and does verification since it has the token
    //  info locally available.  The scheduled_jobs table then receives the record.
    public void StoreSchedJob() throws MyException
    {
        String strHostname=null;
        String dataItems[];
        int i=0;
        Integer uid = null;
        int dataItemLengths[];
        ServerProcess SP = null;
        
        // Get the client data
        dataItemLengths = new int[18];
        dataItems = new String[18];
        
        dataItemLengths[i++] = Constants.MAX_LOGIN_NAME_LEN;      //0 login name
        dataItemLengths[i++] = Constants.MAX_TOKEN_LEN;           //1 token
        dataItemLengths[i++] = Constants.MAX_PROGROUP_TITLE_LEN;  //2 progroup title
        dataItemLengths[i++] = 2;                                 //3 job_status
        dataItemLengths[i++] = Constants.MAX_CUST_PROC_STAT_LEN;  //4 schedule string
        dataItemLengths[i++] = Constants.MAX_PROC_CMD_LINE_LEN;   //5 command line
        dataItemLengths[i++] = Constants.MAX_LOGIN_NAME_LEN;      //6 unix owner
        dataItemLengths[i++] = Constants.MAX_JOB_DESCRIP_LEN;     //7 job descrip
        dataItemLengths[i++] = 2;                                 //8 security class
        dataItemLengths[i++] = Constants.MAX_PRT_NAME_LEN;        //9 printer name
        dataItemLengths[i++] = Constants.MAX_HOST_NAME_LEN;       //10 hostname
        dataItemLengths[i++] = 2;                                 //11 prt sec class
        dataItemLengths[i++] = Constants.MAX_PRT_FILE_LEN;        //12 prt file name
        dataItemLengths[i++] = 2;                                 //13 prt_flag
        dataItemLengths[i++] = 2;               //14 printed page orientation
        dataItemLengths[i++] = 1;               //15 printed security banner style
        dataItemLengths[i++] = 2;               //16 # copies to print
        dataItemLengths[i++] = 2;               //17 printer paper size
        
        try {
           
            // Check if server is idled. 
            if (isServerIdled() == true) 
               throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));

            if (parseData(18, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
           
            SP = new ServerProcess(null,StringLib.parseInt(dataItems[3],0),
            Constants.SCHEDULED_PRIORITY,dataItems[5],0,dataItems[4],
            dataItems[0], dataItems[6],dataItems[2],dataItems[7],
            StringLib.getFullTimeStr(),"***","***",
            StringLib.parseInt(dataItems[8],0),getClientHostname(),
            dataItems[9],dataItems[10],
            StringLib.parseInt(dataItems[11],0),dataItems[12],
            StringLib.parseInt(dataItems[13],0),null,null,null,
            null,StringLib.parseInt(dataItems[16],0),
            StringLib.parseInt(dataItems[15],0),
            StringLib.parseInt(dataItems[14],0),
            StringLib.parseInt(dataItems[17],0));
            
           
            
            int n=0;
            
            if(dataItems[1].regionMatches(0, Constants.BACKDOOR_TOKEN_STRING, 0, 
                             Constants.BACKDOOR_TOKEN_STRING.length()))
                n=0;
            else
                if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                    n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
                else
                    n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            boolean bNtcss=(SP.progroup_title.equals("NTCSS"));
            
            if(!FileOps.Stat(SP.command_line))
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_NO_SUCH_PROCESS));
           
            strHostname=StringLib.getHostname();
            
            if(AddJob("scheduled_jobs",SP)==null)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_INSERT_VALUE));
        } //try
        
        catch(MyException exception){
            //All negative responses are sent back through here
            //TODO make this a function called in @ least 4 places
            Log.warn("CommandServer", "StoreSchedJob",exception.toString());
            SendErrorResponse("NOSVRPROC",exception.toString());
            return;
        }
        
        
        catch (Exception exception){
            Log.excp("CommandServer", "StoreSchedJob", "Failed to add scheduled " +
            "server process record " + SP.process_id + "! -> " +
            exception.toString());
            SendErrorResponse("NOSVRPROC",exception.toString());
            return;
        }
        
        //Success-> send good reply
        SendErrorResponse("GOODSVRPROC",SP.process_id);
        
        // Signal server process queue server about process queue change.
        SendJobQueueRefreshMsg(strHostname,"REFRESHSCHEDJOBQ",SP.progroup_title);
        
        return;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // This function signals the server process queue server to update all
    //  clients with the latest spq list for a designated progroup/app.
    //  This was written because these lines of code appeared so often
    // elsewhere.
    
    // str_host:         The current host where the sqp server is running.
    // str_which_queue:  "REFRESHSPQ", "REFRESHSCHEDJOBQ", etc.
    // str_app_name:      Name of the app for which job has changed status.
    
    protected void SendJobQueueRefreshMsg(String strHost,String strWhichQueue,
    String strAppname)
    {
        SocketLib.sendFireForgetMessage(strHost, Constants.SPQ_SVR_PORT,
        StringLib.padString(strWhichQueue,Constants.CMD_LEN,'\0') +        
        StringLib.padString(strAppname,Constants.MAX_PROGROUP_TITLE_LEN,' '));
    }
    
    
    ///////////////////////////////////////////////////////////////////////////////
    //                     LAUNCHSVRPROCBG message handler
    // This adds a new (background) job to the active_jobs table and then attempts
    // to run it.  Most normal jobs submitted from client machines use this.
    // it bypasses the scheduling (getNextServerProcess) IF IT CAN RUN NOW
    
    public void LaunchBGProcess()throws MyException
    {       
        String dataItems[];
        String newID = null;
        String strRet = null;
        int i=0;
        Integer uid = null;
        int dataItemLengths[];
        ServerProcess SP = null;
        boolean runNow=false;
        
        // Get the client data
        dataItemLengths = new int[23];
        dataItems = new String[23];
        
        dataItemLengths[i++] = Constants.MAX_LOGIN_NAME_LEN;      //0 login name
        dataItemLengths[i++] = Constants.MAX_TOKEN_LEN;           //1 token
        dataItemLengths[i++] = Constants.MAX_PROGROUP_TITLE_LEN;  //2 progroup title
        dataItemLengths[i++] = 2;                      //3 State
        dataItemLengths[i++] = 2;                      //4 priority_code
        dataItemLengths[i++] = Constants.MAX_PROC_CMD_LINE_LEN;   //5 command line
        dataItemLengths[i++] = Constants.MAX_LOGIN_NAME_LEN;      //6 login_name
        dataItemLengths[i++] = Constants.MAX_JOB_DESCRIP_LEN;     //7 job descrip
        dataItemLengths[i++] = 2;                                 //8 security class
        dataItemLengths[i++] = Constants.MAX_PRT_NAME_LEN;        //9 printer name
        dataItemLengths[i++] = Constants.MAX_HOST_NAME_LEN;       //10 hostname
        dataItemLengths[i++] = 2;                                 //11 prt sec class
        dataItemLengths[i++] = Constants.MAX_PRT_FILE_LEN;        //12 prt file name
        dataItemLengths[i++] = 2;                                 //13 prt_flag
        dataItemLengths[i++] = 1;                      //14 sdi required flag
        dataItemLengths[i++] = 1;                      //15 sdi i/o direction flag
        dataItemLengths[i++] = Constants.MAX_SDI_CTL_RECORD_LEN - 2;//16 tape control record index
        dataItemLengths[i++] = Constants.MAX_SERVER_LOCATION_LEN;//17 SDI target data file
        dataItemLengths[i++] = Constants.MAX_SERVER_LOCATION_LEN;//18 tape control intermediate file
        dataItemLengths[i++] = 2;               //19 printed page orientation
        dataItemLengths[i++] = 1;               //20 printed security banner style
        dataItemLengths[i++] = 2;               //21 # copies to print
        dataItemLengths[i++] = 2;               //22 printer paper size
        
        try {
            
            // Check if server is idled. 
            if (isServerIdled() == true) 
               throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));
            
            if (parseData(23, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
 
            int n=0;
            
            if(dataItems[1].regionMatches(0, Constants.BACKDOOR_TOKEN_STRING, 0, 
                             Constants.BACKDOOR_TOKEN_STRING.length()))
                n=0;
            else
                if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                    n = doServerValidate(dataItems[0],dataItems[1],getClientAddress());
                else
                    n = isUserValid(dataItems[0],dataItems[1],new ntcsss.util.IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
                        
           if(UserLib.nIsUserInDB(dataItems[6], null)!=1)
               throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));                                          
            
            SP = new ServerProcess(null,           //process_id
            StringLib.parseInt(dataItems[3],0),   //pid
            StringLib.parseInt(dataItems[4],0),   //priority
            dataItems[5],                           //command_line
            0,null,                                 //proc_status
            dataItems[0],                           //login name
            dataItems[6],                           //unix owner
            dataItems[2],                           //app name
            dataItems[7],                           //job descrip
            StringLib.getFullTimeStr(),             //request time
            null,                                   //launch time
            null,                                   //end time
            StringLib.parseInt(dataItems[8],0),
            getClientHostname(),                     //originating host
            dataItems[9],                           //printer name
            dataItems[10],                          //hostname
            StringLib.parseInt(dataItems[11],0),
            dataItems[12],                          //prt filename
            StringLib.parseInt(dataItems[13],0),
            null,                                   //sdi Ctrl rec
            null,                                   //sdi Data File
            null,                                   //sdi TCI File
            null,                                   //sdi device
            StringLib.parseInt(dataItems[21],0), //# of copies
            StringLib.parseInt(dataItems[20],0),  //banner
            StringLib.parseInt(dataItems[19],0),  // orientation
            StringLib.parseInt(dataItems[22],0)); //papersize
            
//            if ( dataItemLengths[14] == '1') { // tape operations have been requested
            if ( dataItems[14].charAt(0) == '1') { // tape operations have been requested
                
                // clear bit #6 which represents the sdi failure flag
                SP.prt_flag &= ~Constants.PRT_FLAG_MASK6;
                
                // set bit #5 which represents the sdi_required_flag
                SP.prt_flag |= Constants.PRT_FLAG_MASK5;
                
                // set or clear bit #4 which represents the sdi_io_flag
//                if ( dataItemLengths[15] == '1')                  // reading from the device
                if ( dataItems[15].charAt(0) == '1')                  // reading from the device                   
                    SP.prt_flag |= Constants.PRT_FLAG_MASK4;
                else                                        // writing to device
                    SP.prt_flag &= ~Constants.PRT_FLAG_MASK4;
                
                SP.sdi_ctl_rec = dataItems[16];
                SP.sdi_data_file = dataItems[17];
                SP.sdi_tci_file = dataItems[18];
                SP.sdi_device = null;
                SP.pid = Constants.DEVICE_DEPENDENT_STATE;
                
            }
            else              // SDI is not required
                SP.prt_flag &= ~Constants.PRT_FLAG_MASK5; // clear sdi_required bit
            
            
            if(!FileOps.Stat(SP.command_line))
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_NO_SUCH_PROCESS));
            
//            SP.pid=Constants.WAITING_FOR_EXEC_STATE;
                        
            if((SP.pid == Constants.WAITING_FOR_EXEC_STATE) && 
                CheckProcessLimits(SP.progroup_title,null)) {
                // SP.launch_time=StringLib.getFullTimeStr();
                Log.debug("CommandServer", "launch_bg_process",
                "User " + SP.login_name + " in group " + SP.progroup_title +
                "is launching background program " + SP.job_descrip);
                runNow=true;
            }            
            
            // store the new process in the database with the correct state..
            // add job will change the ID
            if((newID=AddJob("active_jobs",SP))==null)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_INSERT_VALUE));
            
            if(runNow)
                if((strRet=launchJob(newID,null))!=null) {
                    SendErrorResponse("NOSVRPROC",strRet + "\0");
                    RemovePIDs(newID,false); //sets it to crashed
                    
                    // Refresh of SPQ server done inside launchJob
//                    SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ",
//                    SP.progroup_title);
                    
                    return;
                }                        
        } //try
        
        catch (MyException exception){
            
            if (SP != null)            
               Log.excp("CommandServer", "launch_bg_process", "Failed to Launch "
               + " Back ground process " + SP.process_id + "! -> " +
               exception.toString());
            else
               Log.excp("CommandServer", "launch_bg_process", "Failed to Launch "
               + " Back ground process! -> " +
               exception.toString());

            SendErrorResponse("NOSVRPROC", exception.toString());
                        
            return;
        }
        
        catch (Exception exception){
            
            Log.excp("CommandServer", "launch_bg_process", "Failed to Launch "
            + " Back ground process " + SP.process_id + "! -> " +
            exception.toString());
                  
            SendErrorResponse("NOSVRPROC", "Failed to Launch "
            + " Back ground process " + SP.process_id );
                    
            return;
        }
             
        //Success-> send good reply
        SendErrorResponse("GOODSVRPROC", SP.process_id + "\0");        
        
           
        // Signal server process queue server about process queue change.
        SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ",
        SP.progroup_title);        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //   called by: getNextServerProcess() & launch_bg_process()
    //
    protected boolean CheckProcessLimits(String strApp,Connection connection)
    {
        Statement statement=null;
        ResultSet resultSet=null;
        boolean bLocalConnect=false;
        int nMaxAppJobs=0;
        int nMaxServerJobs=0;
        int nRet=0;
        
        //Get Server Process Limits
        try {
            if(connection==null){
                if((connection = DatabaseConnectionManager.getConnection())==null){
                    Log.warn("CommandServer", "CheckProcessLimits",
                    "Cannot get Database Connection");
                    return false;
                }
                bLocalConnect=true;
            }
            
            Object items[]=new Object[1];
            items[0]=strApp;
            
            String strSQL=StringLib.SQLformat("SELECT max_running_app_jobs," +
            "max_running_jobs FROM apps a, hosts b WHERE a.app_title=" +
            Constants.ST + " AND upper(a.hostname) = upper(b.hostname)",items);
            
            
            //  Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(strSQL);
           
            if(resultSet.next()) {
                nMaxAppJobs=resultSet.getInt(1);
                nMaxServerJobs=resultSet.getInt(2);
            }
    
            // See if there's enough room to launch a new job..
            nRet=JobSlotsOpen(nMaxAppJobs,nMaxServerJobs,strApp,connection);

        }
        catch (Exception exception){
            Log.excp("CommandServer", "CheckProcessLimits",
            exception.toString());
            return false;
        }
        
        finally {
            try {
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                
                if (bLocalConnect == true)
                   DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "CheckProcessLimits",
                exception.toString());
            }
        }
        
        return nRet>0;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    //                       launchJob()
    //       Called in CheckAndStartWaitingJobs && LaunchBGProcess
    //
    protected String launchJob(String strNtcssID,Connection connection)throws MyException
    {
         
        Log.debug("CommandServer", "launchJob", "Entering!!!!"); //debug
        
        Statement statement=null;
        ResultSet resultSet=null;
        boolean bLocalConnect = (connection==null);
        boolean updateStatus = false;        
        String strCommandLine;
        String strUser = null;
        String strApp = null;
        
        try {
            
            if(bLocalConnect)
                if((connection = DatabaseConnectionManager.getConnection())==null){
                    Log.warn("CommandServer", "launchJob",
                    "Can't get Database Connection!!!!");
                    return "Could not access Database";
                }                        
            
            Object items[]=new Object[4];
            items[0]=strNtcssID;
            
            String strSQL=StringLib.SQLformat("SELECT command_line,job_initiator," +
            "app_title FROM active_jobs WHERE ntcss_job_id = " + Constants.ST,items);
            
            //Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(strSQL);
            
            if(!resultSet.next())
                throw new MyException("Launch failed... Could not retieve DB info " +
                "for Job-> " + strNtcssID);
            
            
            strCommandLine = resultSet.getString(1);
            strUser = resultSet.getString(2);
            strApp = resultSet.getString(3);
            
            resultSet.close();
            
            String strPassword=NDSLib.getNtcssPassword(strUser);
            
            if(strCommandLine==null || strUser==null || strPassword==null ||
            strApp==null) {
               updateStatus = true;                          
                throw new MyException("Could not launch Job (missing arguments)"
                + " for -> " + strNtcssID);
            }
            
            Runtime r = Runtime.getRuntime();
            //need to send username,password,IP,command line,&& app
            String []cmdarray={Constants.PRELAUNCH_FILE,strCommandLine,strUser,
            strPassword,strApp,StringLib.getHostname(),strNtcssID};
            
            Process p = r.exec(cmdarray);
            InputStream in  = p.getInputStream(); //used for getting PID
            BufferedReader input= new BufferedReader(new InputStreamReader(in));
            String strPID=input.readLine();
            input.close();
            
            if(strPID==null) {
               updateStatus = true;               
                throw new MyException("Could not get PID for launched Job-> "
                + strNtcssID);
            }
            
            //Prelaunch puts '!' in 1st char here
            if(strPID.charAt(0)=='!') {
               updateStatus = true;               
               throw new MyException(strPID.substring(1));
            }
            
            //stick process in map for KillServerProcess here
            RunningJobs.put(strNtcssID,p);
            
            Log.debug("CommandServer", "launchjob",
            "Got Launched PID-> " + strPID);
            
            //Update active Jobs with the real PID
            items[0]=strPID;
            items[3]=strNtcssID;
            items[1]=StringLib.getFullTimeStr();
            items[2] ="Process started";

            strSQL=StringLib.SQLformat("UPDATE active_jobs SET pid = "
            + Constants.ST + ",launch_time = " + Constants.ST + 
            ",custom_job_status = " + Constants.ST + 
            " WHERE ntcss_job_id = " + Constants.ST,items);
            
            statement.executeUpdate(strSQL);
            
            SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ", strApp);
           
            return null;
        }
        
        catch (MyException exception){
                StringBuffer masterName;
           
                if (updateStatus == true) {
                   masterName = new StringBuffer();
                   DataLib.getMasterServer(masterName);
                   
                   if (postSystemMessage(masterName.toString(), strUser, 
                                  strApp, exception.toString(),
                                  Constants.SYS_MSG_BB_PROCESS_TERMINATED) != 0)
                      Log.error("CommandServer", "launchJob", "Failed to send " +
                                 "system message to " + strUser);
                   
                   if (SvrProcLib.changeServerProcess(strNtcssID,
                          Constants.CRASHED_STATE,
                          Constants.CHANGE_CUST_STAT|Constants.CHANGE_PID_VALUE,
                          -1, -1, exception.toString(),
                          null, null, null, new StringBuffer(strApp),
                          0, Constants.LOW_PRIORITY) != 0) 
                      Log.error("CommandServer", "launchJob", 
                       "Failed to change PID for server process " + 
                       strNtcssID + "!");
                       
                   SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ", 
                                          strApp);
                }
           
                Log.warn("CommandServer", "launchJob",
                exception.toString());
                return exception.toString();
            }

        
        catch (Exception exception) {
            Log.excp("CommandServer", "launchJob", exception.toString());            
            
            if (strApp != null) {
               if (SvrProcLib.changeServerProcess(strNtcssID,
                          Constants.CRASHED_STATE,
                          Constants.CHANGE_CUST_STAT|Constants.CHANGE_PID_VALUE,
                          -1, -1, exception.toString(),
                          null, null, null, new StringBuffer(strApp),
                          0, Constants.LOW_PRIORITY) != 0) 
                      Log.error("CommandServer", "launchJob", 
                       "Failed to change PID for server process " + 
                       strNtcssID + "!");
                       
                SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ", 
                                          strApp);
            }
            
            return exception.toString();
        }
        
         finally {
            try {                              
                
                if(statement!=null)
                    statement.close();
                if(bLocalConnect)
                    DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "launchJob",
                exception.toString());
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                        REAPER HANDLER
    //
    public void Reaper()
    {
        
        String SQLString;
        Connection connection=null;
        Statement statement=null;
        int dataItemLengths[];
        String dataItems[];
        String appTitle,loginName;
        boolean prtFlag=false;
        ResultSet resultSet=null;
        String customJobStatus;
        int pidVal;
        File printFile;
        String bulletinBoardMsg;
        String endTime, unixOwner;
        StringBuffer masterName;
        
        // Get the client data
        dataItemLengths = new int[2];
        dataItems = new String[2];
        
        // Get process id
        dataItemLengths[0]  = Constants.MAX_PROCESS_ID_LEN;
        // Get Exit Code
        dataItemLengths[1]  = 10;
        
        try {                        
            
            if (parseData(2, dataItemLengths, dataItems) == false) {
                Log.warn("CommandServer", "Reaper", "Could not parse " +
                "incoming message!!!");
                return;
            }
            
            Log.debug("CommandServer","Reaper","Reaping -> " + dataItems[0]); 
            Log.debug("CommandServer","Reaper","Exit Code is -> " + dataItems[1]); 
            
            int exitCode = StringLib.parseInt(dataItems[1],0);
            
            Log.debug("CommandServer","Reaper","Exit Code is -> " + exitCode); 
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "Reaper", "Could not get " +
                "database connection!!!");
                return;
            }
            
            if (exitCode == 0) {
               customJobStatus = "Successful";
               pidVal = Constants.EXITED_STATE;
            }
            else {
               customJobStatus = "Job exit status: " + exitCode;
               pidVal = Constants.CRASHED_STATE;
            }
            
            endTime = StringLib.getFullTimeStr();
            
            Object items[]=new Object[5];
            items[0]=new Integer(pidVal);
            items[1]=new Integer(exitCode);
            items[2]=endTime;
            items[3]=customJobStatus;
            items[4]=dataItems[0];
            
            SQLString =StringLib.SQLformat("UPDATE active_jobs SET pid = " +
            Constants.ST + ",exit_status = " + Constants.ST + 
            ", end_time = " + Constants.ST + ", custom_job_status = " + 
            Constants.ST + " WHERE ntcss_job_id = " + Constants.ST,items);
            
            Log.debug("CommandServer","Reaper", SQLString);
            
            //  Execute the update
            statement = connection.createStatement();
            statement.executeUpdate(SQLString);                        
            
            // Get the job's associated info
            items[0]=dataItems[0];
            
            SQLString = StringLib.SQLformat("SELECT printer_name, print_num_copies, " +
            "app_title, job_requestor, job_initiator, print_file_switch, " +
            "printer_sec_class, " + 
            "print_filename, remove_print_file, job_description " + 
            "FROM active_jobs WHERE ntcss_job_id = " + Constants.ST, items);
            
            Log.debug("CommandServer","Reaper",SQLString); //debug                        
            
            resultSet = statement.executeQuery(SQLString);
            resultSet.next();                        
            
            appTitle = resultSet.getString("app_title");
            loginName = resultSet.getString("job_requestor");
            prtFlag = resultSet.getBoolean("print_file_switch");                                                
            
            //Do NotifyAppCallback
            SvrProcLib.notifyAppCallback(appTitle, (exitCode == 0 ? "SUCCESSFUL"
                           : "UNSUCCESSFUL") , 0, dataItems[0], loginName);
            
            Log.debug("CommandServer","Reaper","Checking Prt Flag"); //debug
            
            if(prtFlag) {
                
                /* NtcssLp(String printerName, String outputType, int numCopies, 
                  String userName, String appName, int securityLevel, 
                  Vector opList, String file) */

                // For some reason in the old code, these printing values in
                //  the database table are ignored. So just carrying over this
                //  logic
                boolean printBanner = false;
                boolean portraitOrientation = true;
                int paperSize = 1;
                
                NtcssLp lp = new NtcssLp(resultSet.getString("printer_name"),
                             null, resultSet.getInt("print_num_copies"), 
                             loginName, appTitle, 
                             resultSet.getInt("printer_sec_class"),
                             null,resultSet.getString("print_filename"), 
                             printBanner, portraitOrientation, paperSize);
                
                lp.print();
                
                // Remove the print file if desired
                if (resultSet.getBoolean("remove_print_file") == true) {
                   printFile = new File(resultSet.getString("print_filename"));
                   if (printFile.exists() == true)
                      printFile.delete();
                }
            }
            
            //Refresh the spq server queues
            SendJobQueueRefreshMsg(StringLib.getHostname(),"REFRESHSPQ",
            appTitle);
                
            //Remove process from map
            RunningJobs.remove(dataItems[0]);
            
             //post checkwaitserverproc here to run as many as possible
            SocketLib.sendFireForgetMessage(StringLib.getHostname(),
                    Constants.CMD_SVR_PORT,StringLib.padString("CHECKWAITSVRPROCS",
                    Constants.CMD_LEN,'\0') + StringLib.padString("all groups", 
                    Constants.MAX_PROGROUP_TITLE_LEN, ' '));
            
            // Send bulletin board message
            bulletinBoardMsg = "Server process " + dataItems[0] + 
                  " with job description \"" + 
                  resultSet.getString("job_description") + "\" terminated at " 
                  + endTime + " with return value " + exitCode + ".";
            
            unixOwner = resultSet.getString("job_initiator");
            masterName = new StringBuffer();
            DataLib.getMasterServer(masterName);
                         
            if (postSystemMessage(masterName.toString(), loginName, appTitle,
                                bulletinBoardMsg, 
                                Constants.SYS_MSG_BB_PROCESS_TERMINATED) != 0)
               Log.error("CommandServer", "reaper", "Failed to send system " +
                         "message to " + loginName + "!");

            if (unixOwner.equals(loginName) == false)
               if (postSystemMessage(masterName.toString(), unixOwner, appTitle, 
                                   bulletinBoardMsg, 
                                   Constants.SYS_MSG_BB_PROCESS_TERMINATED) != 0)
                  Log.error("CommandServer", "reaper", "Failed to send system " +
                         "message to " + unixOwner + "!");
            
        }
        
        catch (Exception exception) {
            Log.excp("CommandServer", "Reaper", exception.toString());
            return;
        }
        
         finally {
            try {
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "Reaper",
                exception.toString());
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                     LAUNCHSVRPROCINTER Handler
    //
    
    public void LaunchInteractiveProc() throws MyException
    {
        //FOR XTERMS ONLY
        //TODO: DECIDE IF WE ARE GOING TO IMPLEMENT
        
        String SQLString;
        Connection connection;
        Statement statement;
        ResultSet resultSet;
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[5];
        dataItems = new String[5];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN;
        dataItemLengths[3]  = Constants.MAX_PROC_CMD_LINE_LEN;
        dataItemLengths[4]  = Constants.MAX_LOGIN_NAME_LEN;
        
        try {
           
            // Check if server is idled. 
            if (isServerIdled() == true) 
               throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(5, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if(dataItems[1].regionMatches(0, Constants.BACKDOOR_TOKEN_STRING, 0, 
                             Constants.BACKDOOR_TOKEN_STRING.length()))
                n=0;
            else
                if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                    n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
                else
                    n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                    IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            if(!FileOps.Stat(dataItems[3]))
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_NO_SUCH_PROCESS));
            
            //TODO CHECK FOR AN NT ACCOUNT WHEN WE GET THE JNI STUFF HERE
            //if(!ValidNTAccount())
            // throw new MyException(ErrorCodes.getErrorMessage
            // (ErrorCodes.ERROR_NO_SUCH_USER));
            
            boolean bNtcss=(dataItems[2].equals("NTCSS"));
            
            //just runs Xterm AS USER ... Stats the process
            //checks for an account
            //stick xterm stuff to the front of it
            //and run it... directly. Need a new parameter in PL so it will just
            //launch these guys put INTERACTIVE in NtcssProcessID Field FIELD
            
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_OPEN_DB_FILE));
            
            //Get password
            SQLString ="SELECT password FROM users WHERE " +
            "login_name = '" + dataItems[4] + "'";
            
            //Execute the query
            statement = connection.createStatement();
            // statement = connection.prepareStatement(SQLString);
            resultSet = statement.executeQuery(SQLString);
            String strPassword=null;
            if(resultSet.next())
                strPassword=resultSet.getString(1);
            
            resultSet.close();
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            if(strPassword==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_INCORRECT_PASSWORD));
            
            String strCmdLine="-display " +  StringLib.getHostAddress() + ":0.0";
            
            //Launch it
            Runtime r = Runtime.getRuntime();
            //need to send username,password,IP & command line
            String []cmdarray={Constants.PRELAUNCH_FILE,
            strCmdLine,dataItems[4],strPassword,StringLib.getHostname(),
            "INTERACTIVE"};
            Process p = r.exec(cmdarray);
            InputStream in  = p.getInputStream(); //used for getting PID
            BufferedReader input= new BufferedReader(new InputStreamReader(in));
            String strPID=input.readLine();
            input.close();
            
         /* if (bNtcss) {
      //Writes program & PID in DB/ADMINPROCS flat file
      //it only happens here ...
     //TODO find out if we need to do this..  fsarchive & serdevinter
     //both of these are X, so it should be deprecated
     //do in line if we need to do it
     if (updateAdminProcs(stripPath(args[0]),(int) proc_pid,
                           NTCSS_DB_DIR) < 0)
        {
          syslog(LOG_WARNING, "%s Failed to update administration processes "
                 "file!", str_fnct_name);
        }
      } */
            
            Log.debug("CommandServer","LaunchInteractiveProc"
            ,"User " + dataItems[4] + "in group " + dataItems[2] +
            "launching interactive program " + strCmdLine + "with PID " + strPID);
            
            //Success-> send good reply
            StringBuffer message=new StringBuffer();
            String msgLengthStr;
            Object items[];
            items = new Object[3];
            items[0] = new Integer(strPID);
            items[1] = StringLib.getTimeStr();
            items[2] = StringLib.getHostname();
            
            BundleLib.bundleData(message, "ICC", items);
            msgLengthStr = String.valueOf(message.length());
            sendFormattedServerMsg("GOODSVRPROC", msgLengthStr + message);
            
        } //try
        
        catch(MyException exception){
            //All negative responses are sent back through here
            //TODO: make this a function called in @ least 4 places
            
            Log.excp("CommandServer", "LaunchInterActiveProc", "Failed to Launch " +
            "Interactive Job " +
            exception.toString());
            
            SendErrorResponse("NOSVRPROC",exception.toString());
            return;
        }
        
        catch (Exception exception){
            
            Log.excp("CommandServer", "LaunchInterActiveProc", "Failed to Launch " +
            "Interactive Job " +
            exception.toString());
            
            SendErrorResponse("NOSVRPROC",exception.toString());
            return;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    //                      GETMASTERNAME HANDLER
    //
    public void GetMasterName()
    {
        //not a preallocated message
        StringBuffer sb=new StringBuffer();
        if(DataLib.getMasterServer(sb)>=0)  //Success-> send good reply
            SendErrorResponse("GOODMASTERNAME",sb.toString());
        
        else {
            Log.warn("CommandServer", "GetMasterName", "Could not get Master Server");
            SendErrorResponse("NOMASTERNAME","Could not get Master Server");
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //           Remove PID's
    //           Called in various palces to remove a list of space 
    //           delimited PIDs or set them to crashed, if string is null
    //           wacks all with a positive PID
    
    protected void RemovePIDs(String strPIDs,boolean bRemove)
    {
        Connection connection;
        Statement statement;
        try {
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "RemovePIDs", "Could not get " +
                "database connection!!!");
                return;
            }
            
            String strSQL=null,strSQL2=null;
            
            if(strPIDs == null)
                strSQL="UPDATE active_jobs SET pid = " + Constants.CRASHED_STATE 
                + " WHERE pid > 0";
            else {
                if(bRemove == true)
//                    strSQL="DELETE FROM active_jobs WHERE ntcss_job_id IN (";
                    strSQL="UPDATE active_jobs SET pid = " + Constants.EXITED_STATE 
                    + " WHERE ntcss_job_id IN (";
                else
                    strSQL="UPDATE active_jobs SET pid = " + Constants.CRASHED_STATE 
                    + " WHERE ntcss_job_id IN (";

                StringTokenizer st = new StringTokenizer(strPIDs);
                while (st.hasMoreTokens()) {
                    if(strSQL2 == null)
                        strSQL2 = "'" + st.nextToken() + "'";
                    else
                        strSQL2 += ",'" + st.nextToken() + "'";
                }
                
                strSQL += strSQL2 + ")";
            }
            
            statement = connection.createStatement();
            statement.executeUpdate(strSQL);
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);

        }
        catch (Exception exception){
            Log.excp("CommandServer", "RemovePIDs",exception.toString());
            SendErrorResponse("NOSVRPROC",exception.toString());
            return;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //                      VERIFYUSER HANDLER
    //
    public void VerifyUserLogon()
    {
        //TODO: This message has the shortcoming of previous baselines in that
        //      it doesn't get ditributed to the other servers
        
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[3];
        dataItems = new String[3];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_LOGIN_NAME_LEN;
        
        // Check if server is idled. 
        if (isServerIdled() == true)
           return;
        
        if (parseData(3, dataItemLengths, dataItems) == false) {
            Log.warn("CommandServer", "VerifyUserLogon", "Could not Parse Data");
        }
        
        Log.debug("CommandServer", "VerifyUserLogon", "User " + dataItems[2] +
        " is being verified..");
        
        StringBuffer client_ip=new StringBuffer();
        
        if(UserLib.isUserLoggedIn(dataItems[2],client_ip,null)==1) {
                Object items[];
                StringBuffer message = new StringBuffer();
                items = new Object[2];
                items[0] = new Integer(client_ip.length() + 5);
                items[1] = client_ip.toString();
                
                BundleLib.bundleData(message, "IC", items);
                sendFormattedServerMsg("GOODVERIFYUSER", message.toString());
        }
        
        else
            SendErrorResponse("NOVERIFYUSER","Failure - " + dataItems[2] +
            " is not logged on.");
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //                       CHANGELOGIN HANDLER
    //
    //                     not a preallocated message
    public void ChangeUserLogon() throws MyException
    {
        
        Connection connection=null;
        Statement statement=null;
        ResultSet resultSet=null;
        int dataItemLengths[];
        String dataItems[];
        String goodResponse = "GOODCHANGELOGIN";
        int type;
        String clientHostname;
        
        // Get the client data
        dataItemLengths = new int[4];
        dataItems = new String[4];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[3]  = 2;
        
        try {
            
            // Check if server is idled. 
            if (isServerIdled() == true) 
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));
            
            if (parseData(4, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
             /* Only need to share this msg if this server is NOT both
            the master and user's auth server and the reqest came from
           the client.  The host type of the client will be unknown (negative).
           We do not want to redistribute the msg if it came from another server,
           if we do then we will have an infinite loop of forwarding the enable
           login msg. */
            
            
            String strAuthServer = NDSLib.getAuthServer(dataItems[2]);            
            String strSendtoHost=null;
            
            StringBuffer sbMaster=new StringBuffer();
            int ret=DataLib.getMasterServer(sbMaster);
            
            if(ret<0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_DETER_MASTER));

            clientHostname = getClientHostname();
            if(!strAuthServer.equals(sbMaster.toString()) &&
                (type = DataLib.getHostType(clientHostname)) < 0) {
                
                Log.debug("CommandServer", "ChangeUserLogon", "clientHostType (" +
                           clientHostname + ") = " + type);
                
                if(ret == 1) 
                    strSendtoHost=strAuthServer;
                else
                    strSendtoHost=sbMaster.toString();
                
                //Just re-create the message string & send it
                //Inputed messages aren't sent bundled
                
                String strResponse=SocketLib.SendServerMessage(
                   StringLib.formatMessage("CHANGELOGIN", GetMessagePayloadString() + "\0"), 
                   strSendtoHost, Constants.CMD_SVR_PORT,null);
                                
                if(strResponse.regionMatches(0, goodResponse, 0, goodResponse.length()))
                    Log.debug("CommandServer", "ChangeUserLogon", "Send to deamon "
                    + "Succeded for User " + dataItems[2]);
                else
                    Log.debug("CommandServer", "ChangeUserLogon", "Send to deamon "
                    + "Failed for User " + dataItems[2]);
                
            }
            
            //now hit local database
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_OPEN_DB_FILE));
            
            Object items[]=new Object[2];       
            items[0]=dataItems[2];                    
            
            //Execute the query
            statement = connection.createStatement();
            resultSet = statement.executeQuery(StringLib.SQLformat(
            "SELECT COUNT(*) FROM current_users WHERE " +
            "login_name = " + Constants.ST, items));
                                   
            resultSet.next();
            
            if(resultSet.getInt(1)>0) {
                resultSet.close();
                statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_USER_LOGGED_IN));
            }
            
            resultSet.close();
            
            items[0] = (dataItems[3].charAt(1)=='1' ? new Boolean(false) : new Boolean(true));
            items[1] = dataItems[2];
            
            int nRet=statement.executeUpdate(StringLib.SQLformat(
            "UPDATE users SET login_lock = " +
            Constants.ST + " WHERE login_name = " + Constants.ST, items));
            
            if(nRet <= 0)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_ACCESS_DB));
            
            //send good response
            Log.debug("CommandServer", "ChangeUserLogon","CHANGELOGIN OK");
            sendFormattedServerMsg("GOODCHANGELOGIN",null);
            return;
            
        }
        
        catch (MyException exception){
            Log.excp("CommandServer", "ChangeUserLogon",exception.toString());
            SendErrorResponse("NOCHANGELOGIN", exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "ChangeUserLogon",exception.toString());
            SendErrorResponse("NOCHANGELOGIN", exception.toString());
        }
        
         finally {
            try {
                
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "ChangeUserLogon",
                exception.toString());
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       LOCKAPP HANDLER
    //
    //                       not a preallocated message
    
    public void LockApp()throws MyException
    {
        
        Connection connection;
        Statement statement;
        int dataItemLengths[];
        String dataItems[];
        String goodResponse = "GOODLOCKAPP";
        boolean loggedInAppUsers;
        int nRet;
        
        // Get the client data
        dataItemLengths = new int[4];
        dataItems = new String[4];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN;
        dataItemLengths[3]  = 2;
        
        try {
           
            if (isServerIdled() == true)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(4, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            //see if this box is master...
            StringBuffer sbMaster=new StringBuffer();
            //if not send this message to master
            if(DataLib.getMasterServer(sbMaster)!=1) {
                                
//                String strResponse=SocketLib.SendServerMessage(dataItems[0] + dataItems[1] +
//                dataItems[2] + dataItems[0],sbMaster.toString(),
//                Constants.CMD_SVR_PORT,null);
                String strResponse=SocketLib.SendServerMessage(
                   StringLib.formatMessage("LOCKAPP", GetMessagePayloadString() + "\0"),
                   sbMaster.toString(), Constants.CMD_SVR_PORT,null);
                
                Log.debug("CommandServer", "LockApp", "Received response from master <" 
                          + strResponse + ">");
                
                if(strResponse.regionMatches(0, goodResponse, 0, goodResponse.length()))
                    Log.debug("CommandServer", "LockApp", "Send to deamon "
                    + "Succeded for App " + dataItems[2]);
                else
                    Log.debug("CommandServer", "LockApp", "Send to deamon "
                    + "Failed for App " + dataItems[2]);
            }
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_OPEN_DB_FILE));
                        
            statement = connection.createStatement();
            
            Object items[] = new Object[2];
            items[0] = dataItems[2]; 
            ResultSet resultSet = statement.executeQuery(StringLib.SQLformat(
                  "SELECT current_users.login_name FROM current_users,app_users WHERE " +
                  "app_users.app_title = " + Constants.ST + 
                  " AND app_users.login_name = current_users.login_name", items));
            
            nRet = 1;
            if (resultSet.next() == false) {
               loggedInAppUsers = false;
               
               items[0] = (dataItems[3].charAt(1) == '1' ? new Boolean(true) : new Boolean(false));
               items[1] = dataItems[2];                        
            
               nRet=statement.executeUpdate(StringLib.SQLformat(
                  "UPDATE apps SET app_lock = " + Constants.ST +
                  " WHERE app_title = " + Constants.ST,items));
            }
            else
               loggedInAppUsers = true;
            
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            if(nRet<=0)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_ACCESS_DB));
            
            if (loggedInAppUsers == true) {
               Log.error("CommandServer", "LockApp", "App users logged in");
               sendErrorMessage("NOLOCKAPP", ErrorCodes.ERROR_USER_LOGGED_IN);
               return;
            }
            
            //send good response
            Log.debug("CommandServer", "LockApp","OK");
            sendFormattedServerMsg(goodResponse,null);
            return;
            
        }
        
        catch (MyException exception){
            Log.debug("CommandServer", "LockApp",exception.toString());
            SendErrorResponse("NOLOCKAPP", exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "LockApp",exception.toString());
            SendErrorResponse("NOLOCKAPP", exception.toString());
        }        
    }
    
   
    
    ////////////////////////////////////////////////////////////////////////////////
    //                        SendErrorResponse
    //
    //  TODO might want to rename this... used for non errors too
    protected boolean SendErrorResponse(String strResponse, String strData)
    //used for the majority of messages that send a response & another BUNDLED string
    {   
        String msgLengthStr=StringLib.valueOf(strData.length(), 5);
        return sendFormattedServerMsg(strResponse,msgLengthStr + strData);
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //                      PUSHNIS HANDLER
    //                      //eats message... we use LDAP from servers now
    public void PushNis()
    {
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    //
    //     Ntcss Stop calls this override to kill active server Procs
    
    public static void runFinalization()
    {
        
        try {
            
            //whack everyone... 
            Log.info("CommandServer", "runFinalization", "Server ordered " + 
            "Killing ALL processes!!!");
            
            timer.cancel();                        
            
            Iterator i = RunningJobs.keySet().iterator();
            
            while (i.hasNext()) {
                        
                String pid = (String)i.next();
                Process p = (Process)RunningJobs.get(pid);
//                Process p = (Process)i.next();
                OutputStream out = p.getOutputStream();        
                BufferedWriter w = new BufferedWriter(new OutputStreamWriter(out));                      
                w.write("DIE");
                w.newLine();               
                w.close();
                i.remove();
            }            
            
            Connection connection;            
            
              // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null){
                Log.warn("CommandServer", "runFinalization", "Could not get " +
                "database connection!!!");
                return;
            }
            
            Statement statement = connection.createStatement();
            statement.executeUpdate("UPDATE active_jobs SET pid = " + 
            Constants.CRASHED_STATE + " WHERE pid > 0");
            statement.close();      
            DatabaseConnectionManager.releaseConnection(connection);           
        }
        
        catch (Exception exception) {
            Log.excp("CommandServer", "runFinalization", exception.toString());
        }
    } 
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       GETHOSTINFO HANDLER
    //
    //                       a preallocated message
    
    public void GetHostInfo()throws MyException
    //used by Print Config
    {
        
        String strReturnVal=null;
        InetAddress Inet=null;
        boolean bGotName=false;
        
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[4];
        dataItems = new String[4];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_HOST_NAME_LEN;
        dataItemLengths[3]  = Constants.MAX_IP_ADDRESS_LEN;
        
        try {
            
            // Check if server is idled. 
            if (isServerIdled() == true) {
               String errorStr = "Host Entry Not Found.";
               Object items[] = new Object[2];
               StringBuffer msg = new StringBuffer();
          
               items[0] = new Integer(errorStr.length());
               items[1] = errorStr;
               BundleLib.bundleData(msg, "IC", items);
               sendFormattedServerMsg("NOGETHOSTINFO", msg.toString() + "\0");
            }
            
            if (parseData(4, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            Log.debug("CommandServer", "GetHostInfo","Checking host/ip <"
            + dataItems[2] + "> and <" + dataItems[3] + ">..");
            
            if(dataItems[2].length()!=0) { //get name
                Inet=InetAddress.getByName(dataItems[2]);
                strReturnVal=Inet.getHostAddress();
            }
            else {                         //get ip
                
                Inet=InetAddress.getByName(dataItems[3]);
                strReturnVal=Inet.getHostName();
                bGotName=strReturnVal.length()>0;
            }
            
            if(strReturnVal.length()==0)
                throw new MyException("Host Entry Not Found!!!");
            
            else {
                
                Object items[];
                StringBuffer message = new StringBuffer();
                items = new Object[2];
                items[0] = bGotName==true?strReturnVal:dataItems[2];
                items[1] = bGotName==false?strReturnVal:dataItems[3];
                
                BundleLib.bundleData(message, "CC", items);
                sendFormattedServerMsg("GOODHOSTINFO", 
                                       preAllocMsg(message.toString()));
                return;
            }
        }
        
        catch (MyException exception){
            Log.excp("CommandServer", "GetHostInfo",exception.toString());
            SendErrorResponse("NOSVRPROC", exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "GetHostInfo",exception.toString());
            SendErrorResponse("NOSVRPROC", exception.toString());
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                      SETPROCDEV HANDLER
    //                      called by SPQ client
    
    public void SetProcDev()
    {
        int dataItemLengths[];
        String dataItems[];
        int n=0;
        
        // Get the client data
        dataItemLengths = new int[5];
        dataItems = new String[5];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN;
        dataItemLengths[3]  = Constants.MAX_PROCESS_ID_LEN;
        dataItemLengths[4]  = Constants.MAX_SERVER_LOCATION_LEN;
        
        // Check if server is idled. 
        if (isServerIdled() == true) 
           n = ErrorCodes.ERROR_SERVER_IDLED;
        
        if (parseData(5, dataItemLengths, dataItems) == false)
            n=ErrorCodes.ERROR_CANT_PARSE_MSG;
        
        if(n>=0)
            n = SvrProcLib.changeServerProcess(dataItems[3] , -1,
            Constants.CHANGE_DEVICE, -1, -1, null,null,null,dataItems[4],
            new StringBuffer(dataItems[2]),0,Constants.LOW_PRIORITY);
        
        if(n<0)
            SendErrorResponse("NOSETPROCDEV", ErrorCodes.getErrorMessage(n));
        else
            sendFormattedServerMsg("GOODSETPROCDEV",null);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                      SETPROCDEV HANDLER
    //                      called by SPQ client
    
    public void GetDevices()
    {
        // Check if server is idled. 
       if (isServerIdled() == true) {
           SendErrorResponse("NOGETDEVICES", 
                   ErrorCodes.getErrorMessage(ErrorCodes.ERROR_SERVER_IDLED));
           return;
       }
        
        Log.warn("CommandServer", "GetDevices","Not implemented");
        //    if(isServerIdled())
        //old code is stranded in Manip even under RDMS & basically reads from the
        //flat file SERVER DEVICES && send them back in a devices structure
        //TODO: get wif TOny on this one
        
/*SDI related files
NTCSS_DEVICES_FILE=%APP_DATA%/fs_archive/devices
NTCSS_TAPE_CTL_FILE=%APP_DATA%/srv_dev/ctlrec
NTCSS_TAPE_SPECS_FILE=%APP_DATA%/srv_dev/tape_specs*/
        
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       DELETESVRFILE MESSAGE HANDLER
    //
    //
    
    public void DeleteSvrFile()throws MyException
    {
        
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[4];
        dataItems = new String[4];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN;
        dataItemLengths[3]  = Constants.MAX_PRT_FILE_LEN;
        
        try {
            
            //Log.turnDebugOn(); //debug
            
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(4, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            //throws a secuity exception if access is denied
            SecurityManager security = System.getSecurityManager();
            if (security != null)
                security.checkDelete(dataItems[3]);
            
            File f=new File(dataItems[3]);
            
            Log.debug("CommandServer", "DeleteSvrFile","File-> " + 
            dataItems[3]); //debug
            
            if(!f.isFile())
                throw new MyException("Problem with File type!!!");
            
            if(!f.delete())
                throw new MyException("File deletion Failed!!!");
            
            // TODO: later see if we can do some of the magic stuff with
            // FilenameFilter to limit this to text fils
            
            //TODO: is this going to be by app on NT??? More JNI to group
            //IN Unix it won't delete unless a member of that group
        }
        
        catch (MyException exception){
            Log.warn("CommandServer", "GetHostInfo",exception.toString());
            SendErrorResponse("NODELETESVRFILE", exception.toString());
        }
        
        catch (SecurityException exception){
            Log.excp("CommandServer","DeleteSvrFile",exception.toString());
            SendErrorResponse("NODELETESVRFILE", "User Doesn't have " +
            "permissions to delete File " + dataItems[3]);
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "DeleteSvrFile",exception.toString());
            SendErrorResponse("NODELETESVRFILE", exception.toString());
        }
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       GETFILEINFO MESSAGE HANDLER
    //
    //                       THIS IS AN ADVANCED ALLOCATED MSG
    
    public void GetFileInfo()throws MyException
    {
        
        int dataItemLengths[];
        long lNumOfBytes=0;
        int nNumOfLines=0;
        
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[3];
        dataItems = new String[3];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PRT_FILE_LEN;
        
        try {
            
            
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(3, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            File f=new File (dataItems[2]);
            
            if(!f.isFile())
                throw new MyException("Problem with file Access!!!");
            
            if((lNumOfBytes=f.length())==0)
                throw new MyException("Could not determine the # of lines");
            
            //try to get #of lines
            
            FileReader in=new FileReader(f);
            LineNumberReader lnr=new LineNumberReader(in,(int)lNumOfBytes);
            lnr.skip(lNumOfBytes-1);
            nNumOfLines=lnr.getLineNumber();
            
            //Success-> send good reply bundle up the results
            
            StringBuffer message=new StringBuffer();
            Object items[];
            items = new Object[2];
            items[0] = new Integer(nNumOfLines);
            items[1] = new Integer((int)lNumOfBytes);
           
            BundleLib.bundleData(message, "II", items);
            
            sendFormattedServerMsg("GOODGETFILEINFO",
                                   preAllocMsg(message.toString()));

        }
        
        catch (MyException exception){
            Log.warn("CommandServer", "GetFileInfo",exception.toString());
            SendErrorResponse("NOGETFILEINFO", exception.toString());
        }
        
        catch (SecurityException exception){
            Log.excp("CommandServer","GetFileInfo",exception.toString());
            SendErrorResponse("NOGETFILEINFO", "User Doesn't have " +
            "permissions to delete File " + dataItems[3]);
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "GetFileInfo",exception.toString());
            SendErrorResponse("NOGETFILEINFO", exception.toString());
        }
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       GETFILESECTION MESSAGE HANDLER
    //
    //                       THIS IS AN ADVANCED ALLOCATED MSG
    
    public void GetFileSection()throws MyException
    {
        
        int dataItemLengths[];
        String dataItems[];
        String line;
        
        // Get the client data
        dataItemLengths = new int[5];
        dataItems = new String[5];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PRT_FILE_LEN;
        dataItemLengths[3] = 8; //First Line
        dataItemLengths[4] = 8; //Last Line
        
        try {
            
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(5, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));

            int nFirstLine=StringLib.parseInt(dataItems[3],0);
            int nLastLine=StringLib.parseInt(dataItems[4],0);
           
            //open input && msg files
            File infile=new File (dataItems[2]);
            File outfile=infile.createTempFile("ntcss","tmp");
            
            if(!infile.isFile())
                throw new MyException("Problem with file Access!!!");
            
            //write the selected lines to the message file
            BufferedWriter w = new BufferedWriter(new FileWriter(outfile));
            LineNumberReader lnr=new LineNumberReader(new FileReader(infile),
            (int)infile.length());
            
            int i;
            
            for(i=1;i<nFirstLine;i++) //read down to 1st line
                lnr.readLine();
            
            int nLines=0;
            
            for(;i<nLastLine;i++) {
               if ((line = lnr.readLine()) != null) {
                  nLines++;
                  w.write(line);
                  w.newLine();
               }
               else 
                 break;
            }
            
            if ((line = lnr.readLine()) != null) { //get last guy in 
               w.write(line); 
               w.newLine();
            }
            
            nLines++;
 
            w.flush();
            w.close();
            lnr.close();
            long lFileLength=outfile.length();
            String path = outfile.getAbsolutePath();

            StringBuffer message=new StringBuffer();
            
            Object items[];
            items = new Object[2];
            items[0] = new Integer(nLines); 
           
            BundleLib.bundleData(message, "I", items);
            
//            int total = (int)lFileLength + message.toString().length();
            
            items[0] = new Integer((int)lFileLength + message.toString().length());
            items[1] = new Integer(nLines);
            message.setLength(0);
            
            BundleLib.bundleData(message, "II", items);
            
            if(!sendServerMsgFile("GOODGETFILESECTION", 
                                  message.toString(),
                                  path,(int)lFileLength)) {
                outfile.delete();
                throw new MyException("Could not send message from server!");
            }
            
            outfile.delete();  
        }
        
        catch (MyException exception){
            Log.warn("CommandServer", "GetFileSection",exception.toString());
            SendErrorResponse("NOGETFILESECTION", exception.toString());
        }
        
        catch (SecurityException exception){
            Log.excp("CommandServer","GetFileSection",exception.toString());
            SendErrorResponse("NOGETFILESECTION", "User Doesn't have " +
            "permissions to access File " + dataItems[3]);
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "GetFileSection",exception.toString());
            SendErrorResponse("NOGETFILESECTION", exception.toString());
        }
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       GETDIRLISTING MESSAGE HANDLER
    //
    //                       THIS IS AN ADVANCED ALLOCATED MSG
    
    public void GetDirListing()throws MyException
    {
        
        int dataItemLengths[];
        String dataItems[];
        Object bundleItems[];
        
        // Get the client data
        dataItemLengths = new int[3];
        dataItems = new String[3];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_WORKING_DIR_LEN;
        
        try {
            
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(3, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            //test if path is good
            
            //Get the filenames & sort them
            File path = new File(dataItems[2]);
            
            if(!path.exists()) 
               throw new MyException("bad directory specified");
           
            String[] list=path.list();
            String fullPathname = path.getCanonicalPath();
            
            if(path.getParent()!=null)
	      fullPathname += path.separator; 
            
            Arrays.sort(list);
            
            Log.debug("CommandServer", "GetDirListing", "Past Array Sort ");
            
            //Success-> create bundled query file
            
            File query = File.createTempFile("ntcss","tmp");
            FileWriter writer = new FileWriter(query);
            
            // Write the bundled data out to the file
            int length = 0;
            int nFiles = 0;
            bundleItems = new Object[4];
            
            //Date needs to be in NTCSS format WITHOUT THE SECONDS
            
            SimpleDateFormat dateFormatter = new SimpleDateFormat("yyMMdd.HHmm00");
            
            for(int i = 0; i < list.length; i++) {
                File info = new File(fullPathname + list[i]);
                if(info.isFile()) {
                    bundleItems[0] = list[i] ;     //fn
                    bundleItems[1] = dateFormatter.format(new java.util.Date(info.lastModified()));
                    bundleItems[2] = new Integer((int)info.length());
                    bundleItems[3] = "TODO"; //TODO: Can we get "owner" like data from NT
                    length += BundleLib.bundleQuery(writer,"CDIC", bundleItems);
                    nFiles++;
                }
            }
            
            writer.flush();
            writer.close();
           
            Log.debug("CommandServer", "GetDirListing", "Past Loop ");
            
            StringBuffer message=new StringBuffer();
            Object items[];
            items = new Object[2];
            items[0] = new Integer(nFiles);
            
            BundleLib.bundleData(message, "I", items);
            
            items[0] = new Integer(length + message.toString().length());
            items[1] = new Integer(nFiles);
            message.setLength(0);
            
            BundleLib.bundleData(message, "II", items);
            
            
            if(!sendServerMsgFile("GOODDIRLISTING", message.toString(),
                query.getAbsolutePath(),length)) {
                query.delete();
                throw new MyException("Could not send message from server!");
            }
            
            query.delete();
        }
        
        catch (MyException exception){
            Log.warn("CommandServer", "GetDirListing",exception.toString());
            SendErrorResponse("NODIRLISTING", exception.toString());
        }
        
        catch (SecurityException exception){
            Log.excp("CommandServer","GetDirListing",exception.toString());
            SendErrorResponse("NODIRLISTING", "User Doesn't have " +
            "permissions to delete File " + dataItems[3]);
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "GetDirListing",exception.toString());
            SendErrorResponse("NODIRLISTING", exception.toString());
        }
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //                       SETPRTSTATUS MESSAGE HANDLER
    //

    public void SetPrinterStatus()throws MyException
    {
        //We don't write these states to the DB now
        //Just runs a script to enable... need to talk to Tony
        
        int dataItemLengths[];
        String dataItems[];
        Object bundleItems[];
        String SQLString;
        Connection connection=null;
        Statement statement=null;
        
        // Get the client data
        dataItemLengths = new int[4];
        dataItems = new String[4];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PRT_NAME_LEN;
        dataItemLengths[3]  = 2; //prt status[1] queue status[2]
        
        
        try {
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(4, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_OPEN_DB_FILE));
            
            Object items[] = new Object[3];
            items[0] = (dataItems[3].charAt(0) =='1' ? new Boolean(true) : new Boolean(false));
            items[1] = (dataItems[3].charAt(1) =='1' ? new Boolean(true) : new Boolean(false));
            items[2] = dataItems[2];
            
            SQLString =StringLib.SQLformat("UPDATE printers SET " +
            "printer_enabled = " + Constants.ST + " ,queue_enabled = " +
            Constants.ST + " WHERE printer_name = " + Constants.ST,items);
            
            Log.excp("CommandServer", "SetPrinterStatus",
            "Doing this SQL statement-> " + SQLString);  //debug
            
            //  Execute the update
            statement = connection.createStatement();
            int nRet=statement.executeUpdate(SQLString);
            statement.close();
            DatabaseConnectionManager.releaseConnection(connection);
            
            if(nRet>0)
                SendErrorResponse("GOODSETPRTSTATUS",null);
            else
                throw new MyException("The SQL Update failed-> " + SQLString);
        }
        
        catch (MyException exception){
            Log.excp("CommandServer", "SetPrinterStatus",exception.toString());
            SendErrorResponse("NOSETPRTSTATUS", exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "SetPrinterStatus",exception.toString());
            SendErrorResponse("NOSETPRTSTATUS", exception.toString());
        }
        
         finally {
            try {

                if(statement!=null)
                    statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "SetPrinterStatus",
                exception.toString());
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    //                     GETPRTSTATUS HANDLER
    //                    //eats message... until I can talk to Tony
    public void GetPrinterStatus()
    {
        //TODO: 
       
         // Check if server is idled. 
       if (isServerIdled() == true) {
          String errorStr = "Error getting printer status.";
          Object items[] = new Object[2];
          StringBuffer msg = new StringBuffer();
          
          items[0] = new Integer(errorStr.length());
          items[1] = errorStr;
          BundleLib.bundleData(msg, "IC", items);
          sendFormattedServerMsg("NOGETPRTSTATUS", msg.toString() + "\0");
       }
    }
    
    protected String preAllocMsg(String Msg)
    //Used for preallocated Msg .. size bundled as Int to front of the Msg
    {
            Integer len = new Integer(Msg.length()); 
            return (new Integer((len.toString()).length()).toString() +
                   len.toString() + Msg);
    }
    
    
    /////////////////////////////////////////////////////////////////////////
    //     Used in DLL to figure out (QUICKLY) if we're talking to NT
    //                   
    public void GetServerType()
    {
         sendServerMsg(StringLib.padString("GOODGETSERVERTYPE", 
                Constants.CMD_LEN,'\0'));
    }
    
    /////////////////////////////////////////////////////////////////////////
    //     Used in PUI to figure out (QUICKLY) what the NT based APPS are
    //     PUI will send a list of hosts & we need to send a list APPS back
    //     Send it in with the single quotes on so we can drop it right in
    
     public void GetNTAppList()
    {
        Connection connection=null;
        Statement statement=null;
        ResultSet resultSet=null;
        
        try {
            
            String msg=GetMessagePayloadString();
            msg=msg.substring(0,msg.length()-1);
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null) {
                Log.warn("CommandServer", "GetNTAppList",
                ErrorCodes.getErrorMessage(ErrorCodes.ERROR_CANT_OPEN_DB_FILE));
                sendServerMsg(StringLib.padString("NOGETNTAPPLIST", 
                Constants.CMD_LEN,'\0'));
                return;
            } 
            
            statement = connection.createStatement();
            String apps="";
            
            resultSet = statement.executeQuery("SELECT app_title FROM " + 
            "apps WHERE hostname IN (" + msg + ") ORDER BY app_title");
            
            while(resultSet.next()) {
                if(apps.length()==0)
                    apps=resultSet.getString("app_title");
                else
                    apps += "," + resultSet.getString("app_title");
            }
            
             String msgLengthStr=StringLib.valueOf(apps.length(), 5);
            
             sendServerMsg(StringLib.padString("GOODGETNTAPPLIST", 
                    Constants.CMD_LEN,' ') + msgLengthStr + apps + "\0");
         
        }
        
            catch (Exception exception){
            Log.excp("CommandServer", "GetNTAppList",exception.toString());
            SendErrorResponse("NOGETNTAPPLIST", exception.toString());
        }
        
         finally {
            try {
                
                if(resultSet!=null)
                    resultSet.close();
                if(statement!=null)
                    statement.close();
                DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "GetNTAppList",
                exception.toString());
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    //                     PRINTSVRFILE HANDLER
    //                   
    public void PrintServerFile() throws MyException {
        int dataItemLengths[];
        String dataItems[];
        Object bundleItems[];
        
        // Get the client data
        dataItemLengths = new int[9];
        dataItems = new String[9];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2] =  Constants.MAX_PRT_FILE_LEN;  //prt_file
        dataItemLengths[3] =  Constants.MAX_PRT_NAME_LEN;  //prt_name
        dataItemLengths[4] =  Constants.MAX_HOST_NAME_LEN; //host_name
        dataItemLengths[5] = Constants.MAX_PROGROUP_TITLE_LEN; //progroup_title
        dataItemLengths[6] = 2; //sec class
        dataItemLengths[7] = 2; //copies
        dataItemLengths[8] = Constants.MAX_PRT_OPTIONS_LEN; //options
        
        try {
            
            if(isServerIdled())
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_SERVER_IDLED));
            
            if (parseData(9, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage(ErrorCodes.
                ERROR_CANT_PARSE_MSG));
            
            int n=0;
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n=doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n=isUserValid(dataItems[0],dataItems[1],new ntcsss.util.
                IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            Vector optionList=new Vector();
            optionList.add(dataItems[8]);
            
            boolean printBanner = false;
            boolean portraitOrientation = true;
            int paperSize = 1;
            NtcssLp lp = new NtcssLp(dataItems[3], null, 
                                     StringLib.parseInt(dataItems[7], 1),
                                     dataItems[0],dataItems[5],
                                     StringLib.parseInt(dataItems[6], 
                                     Constants.UNCLASSIFIED_CLEARANCE),
                                     optionList, dataItems[2], printBanner, 
                                     portraitOrientation, paperSize);
            
            if(!lp.print())
                throw new MyException("Error printing File");
            
            Log.info("CommandServer", "PrintServerFile","called NTCSSLP");
           
            SendErrorResponse("GOODPRINTSVRFILE", "Server file successfully printed.");
            
        }
        
        catch (MyException exception){
            Log.warn("CommandServer", "PrintServerFile",exception.toString());
            SendErrorResponse("NOPRINTSVRFILE", exception.toString());
        }
        
        catch (Exception exception){
            Log.excp("CommandServer", "PrintServerFile",exception.toString());
            SendErrorResponse("NOPRINTSVRFILE", exception.toString());
        }
        
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    //                      GETPRINTERINI MSG HANDLER
    
    public void putPrinterIni()
    {
        
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[3];
        dataItems = new String[3];
        
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PATH_LEN;
        
        try {
           
            // Check if server is idled. 
            if (isServerIdled() == true) {
                SendErrorResponse("NOPUTPRINTERINI", ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));
                return;
            }
            
            if (parseData(3, dataItemLengths, dataItems) == false) {
                SendErrorResponse("NOPUTPRINTERINI", ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
                return;
            }
            
            String logFile = Constants.NTCSS_PRINTER_CHANGE_FILE + "." + dataItems[0];
            
            if(PrintLib.extractPrinterIni(dataItems[2],logFile)) {
                
                Object items[];
                StringBuffer message = new StringBuffer();
                items = new Object[1];
                items[0] = logFile;
                
                BundleLib.bundleData(message, "C", items);
                sendFormattedServerMsg("GOODPUTPRINTERINI", 
                                       preAllocMsg(message.toString()));
            }
            
            else
                SendErrorResponse("NOPUTPRINTERINI", "Can't create Printer INI file");
        }
        
        catch(Exception exception){
            Log.excp("CommandServer", "getPrinterIni",exception.toString());
            SendErrorResponse("NOGETPRINTERINI", exception.toString());
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    //                      PUTPRINTERINI MSG HANDLER
    
    public void getPrinterIni()
    {
        
        int dataItemLengths[];
        String dataItems[];
        
        // Get the client data
        dataItemLengths = new int[2];
        dataItems = new String[2];
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;        
   
        try {
           
            // Check if server is idled. 
            if (isServerIdled() == true) {
                SendErrorResponse("NOGETPRINTERINI", ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));
                return;
            }
            
            if (parseData(2, dataItemLengths, dataItems) == false) {
                SendErrorResponse("NOGETPRINTERINI", ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
                return;
            }
            
            String getFileName = Constants.NTCSS_PRINTER_INI_FILE + "." +
                                 dataItems[0];
            
            if(PrintLib.createPrinterIni(getFileName)) {

                Object items[];
                StringBuffer message = new StringBuffer();
                items = new Object[1];
                items[0] = getFileName;
                
                BundleLib.bundleData(message, "C", items);
                sendFormattedServerMsg("GOODGETPRINTERINI", 
                                       preAllocMsg(message.toString()));
            }
            else
                SendErrorResponse("NOGETPRINTERINI", "Can't create Printer INI file");
        }
        
        catch(Exception exception){
            Log.excp("CommandServer", "getPrinterIni",exception.toString());
            SendErrorResponse("NOGETPRINTERINI", exception.toString());
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    //                      SETPROCLIMIT MSG HANDLER
    
    public void setProcessLimits()
    {
        
        int dataItemLengths[];
        String dataItems[];
        Connection connection=null;
        Statement statement=null;
        int type = 0,limit;
        String errorMsg = "NOSETPROCLIMIT";
        
        // Get the client data
        dataItemLengths = new int[5];
        dataItems = new String[5];
        dataItemLengths[0]  = Constants.MAX_LOGIN_NAME_LEN;
        dataItemLengths[1]  = Constants.MAX_TOKEN_LEN;
        dataItemLengths[2]  = Constants.MAX_PROGROUP_TITLE_LEN; //0 is app, 1 is host
        dataItemLengths[3]  = 2; //flag
        dataItemLengths[4]  = 10;
        int n=0;
        int nRet=0;
        
        try {
           
            // Check if server is idled. 
            if (isServerIdled() == true)
               throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_SERVER_IDLED));                        
            
            if (parseData(5, dataItemLengths, dataItems) == false)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
            
            type = StringLib.parseInt(dataItems[3],0);
            limit = StringLib.parseInt(dataItems[4],0); //TODO Range chk this??
            
            if(type != 0 && type != 1)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_PARSE_MSG));
            
            if (dataItems[1].regionMatches(0, Constants.SERVER_TOKEN_STRING, 0, 
                             Constants.SERVER_TOKEN_STRING.length()))
                n = doServerValidate(dataItems[0],dataItems[1],getClientAddress());
            else
                n = isUserValid(dataItems[0],dataItems[1],new ntcsss.util.IntegerBuffer());
            
            if (n < 0)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_USER_NOT_VALID));
            
            // Get a connection from the database
            if((connection = DatabaseConnectionManager.getConnection())==null)
                throw new MyException(ErrorCodes.getErrorMessage
                (ErrorCodes.ERROR_CANT_OPEN_DB_FILE));
            
            Object items[] = new Object[2];
            
            items[0] = new Integer(limit);
            items[1] = dataItems[2];
            
            //Execute the Update
            statement = connection.createStatement();
            
            if(type==0) //app
                nRet=statement.executeUpdate(StringLib.SQLformat(
                "UPDATE apps SET max_running_app_jobs = " + Constants.ST +
                " WHERE app_title = " + Constants.ST,items));
            else { //host
               
                nRet=statement.executeUpdate(StringLib.SQLformat(
                "UPDATE hosts SET max_running_jobs = " + Constants.ST +
                " WHERE hostname = " + Constants.ST,items));
                
                // Update NDS
                if ((DataLib.isMasterServer() == true) &&
                    (NDSLib.modifyHostProcessLimit(dataItems[2], limit) == false))
                   Log.error("CommandServer", "setProcessLimits", "Unable " +
                             "to set process limit in NDS for " + dataItems[2]);
            }
            
            if(nRet>0)
                sendServerMsg(StringLib.padString("GOODSETPROCLIMIT",
                Constants.CMD_LEN,'\0'));
            else
                SendErrorResponse(errorMsg, "Setting Status Failed");
        }
        
        catch (MyException exception){
            Log.excp("CommandServer", "setProcessLimits",exception.toString());
            SendErrorResponse(errorMsg, exception.toString());
        }
        
        catch(Exception exception) {
            Log.excp("CommandServer", "setProcessLimits",exception.toString());
            SendErrorResponse(errorMsg, exception.toString());
        }
        finally {
            try {
                
                if(statement!=null)
                    statement.close();
                if (connection != null)
                   DatabaseConnectionManager.releaseConnection(connection);
            }
            catch (Exception exception){
                Log.excp("CommandServer", "setProcessLimits",
                exception.toString());
            }
        }       
        
        // Send the CHECKWAITSVRPROCS message
        if ((dataItems[2] != null) && (dataItems[2].length() > 0))  {           
           String host, app;
           
           if (type != 0) {// Host type
              host = dataItems[2];
              app = "all groups";              
           }
           else {  // APP type
              host = "localhost";
              app = dataItems[2];                                          
           }
           
           if (SocketLib.sendFireForgetMessage(host, Constants.CMD_SVR_PORT, 
                      StringLib.formatMessage("CHECKWAITSVRPROCS", 
                                              StringLib.padString(app, 
                                                                  Constants.MAX_PROGROUP_TITLE_LEN, ' ') + "\0")) == false)                                                                 
              Log.error("CommandServer", "setProcessLimits",  
                        "Failed to signal command server" +
                        " to check waiting processes on " + host + " for " + 
                        app + " after changing limit!");
        }
    }
} ///:~



///////////////////////////////////////////////////////////////////////////////
//                       class MyException
//
class MyException extends Exception
{
    public MyException() {}
    public MyException(String msg)
    {
        super(msg);
    }
}




