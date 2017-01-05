/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

public class Constants
{               
   private static final String EXE_EXTENSION; 
   private static final String LIB_EXTENSION; 
   public static final boolean IS_WINDOWS_OS;
   
   static {
      String OSName = System.getProperty("os.name");
      
      // Determine the operating system 
      if (OSName.toLowerCase().indexOf("windows") != -1) { // i.e. Windows OS
         EXE_EXTENSION = ".exe";
         LIB_EXTENSION = ".dll";
         IS_WINDOWS_OS = true;
      }
      else {
         EXE_EXTENSION = ""; 
         LIB_EXTENSION = ".so";
         IS_WINDOWS_OS = false;
      }
   } 
   
   // Return null for the default master name if it is not specified in the
   //  properties file
   public static final String NTCSS_DEFAULT_MASTER = System.getProperty("ntcss.default_master");
   
   
   public static final String NTCSS_DB_VERSION = "3.0.5";
   public static final String NTCSS_SOFTWARE_VERSION = "1.0.0";
   
   // Directories
   public static final String NTCSS_ROOT_DIR =
                             System.getProperty("ntcss.root_dir", "D:/Projects/NTCSS_JAVA/NTCSSS");   
   public static final String NTCSS_DB_DIR = NTCSS_ROOT_DIR + "/database";
   public static final String NTCSS_BIN_DIR = NTCSS_ROOT_DIR + "/bin";
   public static final String NTCSS_LOGS_DIR = NTCSS_ROOT_DIR + "/logs";
   public static final String NTCSS_APP_DATA_DIR = NTCSS_ROOT_DIR + "/applications/data";
   public static final String NTCSS_INIT_DATA_DIR = NTCSS_ROOT_DIR + "/init_data";
   public static final String NTCSS_APP_DATA_INSTALL_DIR = NTCSS_ROOT_DIR + 
                                   "/applications/data/install";    
   public static final String NTCSS_LIBS_DIR = NTCSS_ROOT_DIR + "/libs";
   public static final String NTCSS_NTDRIVER_DIR = NTCSS_ROOT_DIR + "/spool/ntdrivers";
   public static final String NTCSS_NDS_DIR = NTCSS_ROOT_DIR + "/ldap";
   public static final String NTCSS_TMP_DIR = NTCSS_ROOT_DIR + "/tmp";
   
   // Files
   public static final String NTCSS_PROGRP_INIT_FILE = NTCSS_INIT_DATA_DIR + 
                                                                "/progrps.ini"; 
   public static final String NTCSS_PROGRP_DB_FILE = NTCSS_DB_DIR + 
                                                                "/progrps.ini";
   public static final String PROGROUP_CACHE_FILE = NTCSS_LOGS_DIR + 
                                                                "/PG.query";
   
   public static final String PRELAUNCH_FILE = NTCSS_BIN_DIR + "/prelaunch" + EXE_EXTENSION;
   
   public static final String NTCSS_UP_TIME_FILE = NTCSS_LOGS_DIR + 
                                                               "/ntcss_up_time";
   public static final String NOLOGINS_FILE = NTCSS_LOGS_DIR + "/NOLOGINS";
   public static final String NATIVE_LIBRARY = NTCSS_LIBS_DIR + "/NtcssNative" + LIB_EXTENSION;   
   public static final String CURRENT_USERS_FILE = NTCSS_DB_DIR + "/current_users";
   public static final String CREATE_TABLES_SCRIPT = NTCSS_DB_DIR + "/create_tables.sql";
   public static final String NTCSS_LOG_FILE = NTCSS_LOGS_DIR + "/ntcsss.log";
   public static final String STOP_FILE = NTCSS_LOGS_DIR + "/STOP_FILE";
   public static final String PRT_TYPES_FILE = NTCSS_DB_DIR + "/prt_class";
   public static final String ADAPTER_TYPES_FILE = NTCSS_DB_DIR + "/ADAPTER_TYPES";
   

   // INI-related files:   
   public static final String NTCSS_OTYPE_INI_FILE = NTCSS_LOGS_DIR +
                                                                   "/otype.ini";
   public static final String NTCSS_OTYPE_CHANGE_FILE = NTCSS_LOGS_DIR +
                                                          "/otype_results.ini";
   public static final String NTCSS_PDJ_INI_FILE = NTCSS_LOGS_DIR +
                                                                 "/pdjdata.ini";
   public static final String NTCSS_PDJ_CHANGE_FILE = NTCSS_APP_DATA_DIR +
                                                        "/pdj_conf/pdjdata.sql";
   public static final String USER_CONFIG_USERS_CACHE_FILE = NTCSS_LOGS_DIR +
                                                      "/UC_users.cache";
   public static final String USR_ADMIN_APPLY_FILE = NTCSS_LOGS_DIR +
                                                              "/UserAdminApply";
   public static final String USR_ADMIN_APPLY_LOG_FILE = NTCSS_LOGS_DIR +
                                                           "/UserAdminApplyLog";
   public static final String PRT_ADMIN_APPLY_FILE = NTCSS_LOGS_DIR +
                                                         "/PrintAdminApply.INI";
   public static final String PRT_ADMIN_APPLY_LOG_FILE = NTCSS_LOGS_DIR +
                                                      "/PrintAdminApplyLog.INI";

   // Ports
   public static final int UDP_CLT_PORT = 
    StringLib.parseInt(System.getProperty("client.udp_port", "9121"), 9121);
   public static final int PRTQ_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.prtq_svr_port", "9122"), 9122);
   public static final int DB_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.db_svr_port", "9132"), 9132);
   public static final int DSK_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.dsk_svr_port", "9142"), 9142);
   public static final int CMD_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.cmd_svr_port", "9152"), 9152);
   public static final int SPQ_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.spq_svr_port", "9162"), 9162);
   public static final int MSG_SVR_PORT =
    StringLib.parseInt(System.getProperty("servers.msg_svr_port", "9172"), 9172);
   public static final int LOG_DAEMON_PORT =
    StringLib.parseInt(System.getProperty("logger.port", "9112"), 9112); 
   public static final int PRT_LISTD_PORT = 
    StringLib.parseInt(System.getProperty("plist.port", "5583"), 5583);
   
   // On Windows NT 4.0, this number has a very large effect on the size of 
   //  "non-paged pool" memory used by the process. Each increment of this 
   //  number increases "non-paged pool" by 16kbytes. If the amount of 
   //  "non-paged pool" memory used is too high, NTCSS batch jobs will not be
   //  able to run - the system reports a ERROR_NOT_ENOUGH_QUOTA Microsoft error.
   public static final int SERVER_BACKLOG_QUEUE_SIZE = 
    StringLib.parseInt(System.getProperty("server.backlog_size", "5"), 5);


   // Database constants
   public static final int DB_CONNECTION_POOL_SIZE =
     StringLib.parseInt(System.getProperty("ntcss_database.connection_pool_size", "25"), 25);
   public static final String DB_JDBC_DRIVER = "com.mckoi.JDBCDriver";     
   public static final String DB_PORT = System.getProperty("jdbc_server_port", "9157");
   public static final String DB_URL = "jdbc:mckoi://localhost:" + DB_PORT + "/";    
   public static final String DB_USER = "ntcss";
   public static final String DB_PASSWORD = "ntcss";
   public static final String ST = "~~"; // SQL delimiter
   public static final int DB_CONNECTION_ATTEMPTS =
     StringLib.parseInt(System.getProperty("ntcss_database.connection_attempts", "3"), 3);
   
   // Server state/actions constants
   public static final int SERVER_STATE_IDLE     = 0;   // action, state 
   public static final int SERVER_STATE_AWAKE    = 1;   // action, state
   public static final int SERVER_STATE_CHECK    = 2;   // action
   public static final int SERVER_STATE_DEAD     = 3;   // state 
   public static final int SERVER_STATE_ONDEBUG  = 4;   // action, state 
   public static final int SERVER_STATE_OFFDEBUG = 5;   // action, state 
   public static final int SERVER_STATE_DIE      = 7;   // action 
   public static final int SERVER_STATE_ONLOCK   = 8;   // action, state 
   public static final int SERVER_STATE_OFFLOCK  = 9;   // action, state 
   
   // Server type constants
   public static final int NTCSS_MASTER      = 0;
   public static final int NTCSS_APP_SERVER  = 1;
   public static final int NTCSS_AUTH_SERVER = 2;
   
   // Naming and Directory Services constants
   public static final String NDS_SERVER = System.getProperty("ntcss.nds_server");
   public static final String NDS_ROOT = System.getProperty("ntcss.nds_root",
                                                            "ATLASS");
   public static final String NDS_SERVER_PORT = System.getProperty(
                                                "ntcss.nds_server_port", "389");
   
   public static final int SOCKET_READ_TIMEOUT = 5000;  // In milliseconds
   
   public static final String SERVER_TOKEN_STRING = "XXXXXX";
   public static final int    PRETOKEN_SIZE = 6;
   public static final String BACKDOOR_TOKEN_STRING  = "YYYYYY";      
   
   // NTCSS time format
   public static final int TIME_STR_LEN      = 11;
   public static final int FULL_TIME_STR_LEN = 13;  
   public static final int DATETIME_LEN      = 14;

   // misc. messaging
   public static final int CMD_MESSAGE_LEN   = 4096; 
   public static final int MAX_CMD_LEN       = 2048;
   public static final int CMD_LEN           = 20; 
   public static final int MAX_MSG_LEN       = 8192;
   public static final int MAX_PATH_LEN      = 256;

   // hosts
   public static final int MAX_HOST_NAME_LEN  = 16;
   public static final int MAX_IP_ADDRESS_LEN = 32;
   public static final int MIN_HOST_NAME_LEN  = 3;
   
   // User data
   public static final int MAX_LOGIN_NAME_LEN = 8;
   public static final int MIN_LOGIN_NAME_LEN = 3;
   public static final int MAX_PASSWORD_LEN   = 32;
   public static final int MAX_SS_NUM_LEN     = 11;
   public static final int MAX_APP_DATA_LEN   = 256;
   public static final String  BASE_NTCSS_USER      = "ntcssii";
   public static final String  NTCSS_UNIX_USERS_DIR = "/h/users";

   // Apps
   public static final String NTCSS_APP            = "NTCSS";
   public static final int MAX_PROGROUP_TITLE_LEN  = 16;
   public static final int MIN_PROGROUP_TITLE_LEN  = 3;
   public static final int MAX_UNIX_GROUP_LEN      = 16;
   public static final int MAX_ICON_FILE_LEN       = 128;
   public static final int MAX_VERSION_NUMBER_LEN  = 32;
   public static final int MAX_APP_PASSWD_LEN      = 32;

   // App_programs
   public static final int MAX_WORKING_DIR_LEN     = 128;
   public static final int MIN_WORKING_DIR_LEN     = 3;
   public static final int MIN_PROGRAM_TITLE_LEN   = 3;
   public static final int MAX_PROGRAM_TITLE_LEN   = 64;
   public static final int MIN_PROGRAM_FILE_LEN    = 3;
   public static final int MAX_PROGRAM_FILE_LEN    = 32;
   public static final int MAX_CMD_LINE_ARGS_LEN   = 128;

   // App_user
   public static final int MAX_NUM_PROGROUPS_PER_NTCSS_USER = 64;
   public static final int MAX_GROUP_LIST_LEN = (MAX_PROGROUP_TITLE_LEN + 1) *
                                              MAX_NUM_PROGROUPS_PER_NTCSS_USER;
   
   // Login - current_users
   public static final int MAX_TIME_WITH_MILI_SEC_LEN = 32;
   public static final int LOGIN_ENCRYPTION_KEY_LEN = 8;
   public static final int MAX_TOKEN_LEN      = 32;

   //App roles stuff..
   public static final int MAX_NUM_APP_ROLES_PER_PROGROUP = 8;

   // predefined_jobs
   public static final int MAX_REQUEST_ID_LEN = 36;
   public static final int MIN_PREDEFINED_JOB_LEN = 3;
   public static final int MAX_PREDEFINED_JOB_LEN = 32;

   // sys_conf stuff
   public static final int MIN_COMDB_TAG_LEN   = 1;
   public static final int MAX_COMDB_TAG_LEN   = 64;
   public static final int MIN_COMDB_DATA_LEN  = 1;
   public static final int MAX_COMDB_DATA_LEN  = 128;

   // Jobs related
   public static final int MAX_PROCESS_ID_LEN      = 32;
   public static final int MAX_PROC_CMD_LINE_LEN   = 256;
   public static final int MAX_CUST_PROC_STAT_LEN  = 128;
   public static final int MAX_JOB_DESCRIP_LEN     = 256;
   public static final int MAX_ORIG_HOST_LEN       = 32;
   public static final int MAX_PROC_PID_LEN        = 8;
   public static final int MAX_SCHED_STR_LEN       = 10;
   public static final int MAX_NUM_PROC_ARGS       = 16;
   public static final int MAX_SDI_CTL_RECORD_LEN  = 8;
   public static final int MAX_SERVER_LOCATION_LEN = 128;
   public static final int MAX_CLIENT_LOCATION_LEN = 128;

   // BB stuff
   public static final int MAX_SYSTEM_MSG_LEN  = 512;
   public static final int NUM_SYSTEM_MSG_BBS  = 20;
   public static final int MIN_BB_NAME_LEN     = 3;
   public static final int MAX_BB_NAME_LEN     = 16;
   public static final int MAX_MSG_ID_LEN      = (DATETIME_LEN + MAX_LOGIN_NAME_LEN);
   public static final int MAX_MSG_TITLE_LEN   = 32;

   // Printer stuff
   public static final int MAX_PRT_DEVICE_NAME_LEN = 128;
   public static final int MAX_OUT_TYPE_TITLE_LEN  = 16;
   public static final int MAX_PRT_NAME_LEN        = 16;
   public static final int MAX_PRT_FILE_LEN        = 128;
   public static final int MAX_PRT_OPTIONS_LEN       =  255;
   public static final String COPY_FILE_PREFIX     = "COPY_FILE";   
   public static final int MAX_COPY_FILE_LENGTH     = 2048;  
   
   public static final String NT_INI_FILE = NTCSS_DB_DIR + "/ntprint.ini";
   public static final String NTCSS_PRINTER_INI_FILE = NTCSS_LOGS_DIR +
                                                             "/printcap.ini";
   public static final String NTCSS_PRINTER_CHANGE_FILE = NTCSS_APP_DATA_DIR +
                                                   "/prt_conf/printcap.changes";
   
   public static final int DRIVER_DRIVER_TYPE  = 0;
   public static final int UI_DRIVER_TYPE      = 1;
   public static final int DATA_DRIVER_TYPE    = 2;
   public static final int HELP_DRIVER_TYPE    = 3;
   public static final int MONITOR_DRIVER_TYPE = 4;
   public static final int NO_DRIVER_TYPE      = 5; //copy files 

   // DM/DP values
   public static final int MAX_NUM_ACTIVE_DPS = 
        StringLib.parseInt(System.getProperty("distmgr.max_num_dps", "10"), 10); 
   public static final int DP_CONNECTION_TIMEOUT = 
        StringLib.parseInt(System.getProperty("distproc.conn_timeout", "5"), 5);
   public static final int DP_AUTHUSER_TIMEOUT         = 20;
   public static final int DP_GETMASTERFILES_TIMEOUT   = 10;         
   public static final int DP_USERADMINCHANGES_TIMEOUT = 40;         

   // Basic job types
   public static final int REGULAR_JOB   = 0;
   public static final int BOOT_JOB      = 1;
   public static final int SCHEDULED_JOB = 2;

   // Server process queue bit mask values.
   public static final int SPQ_BIT_MASK_WITH_CUST_PROC_STATUS  = 1;
   public static final int SPQ_BIT_MASK_WITH_PRT_REQ_INFO      = 2;
   public static final int SPQ_BIT_MASK_WITH_JOB_DESCRIP       = 4;
   public static final int SPQ_BIT_MASK_ONLY_CERTAIN_USER      = 8;
   public static final int SPQ_BIT_MASK_ONLY_RUNNING_PROCS     = 16;
   public static final int SPQ_BIT_MASK_ONLY_USER_ACTIVE_PROCS = 32; // special case, overrides all
   public static final int SPQ_BIT_MASK_PRIORITY               = 64;
   public static final int SPQ_BIT_MASK_COMMAND_LINE           = 128;
   public static final int SPQ_BIT_MASK_CUST_PROC_STATUS       = 256;
   public static final int SPQ_BIT_MASK_LOGIN_NAME             = 512;
   public static final int SPQ_BIT_MASK_UNIX_OWNER             = 1024;
   public static final int SPQ_BIT_MASK_PROGROUP_TITLE         = 2048;
   public static final int SPQ_BIT_MASK_JOB_DESCRIP            = 4096;
   public static final int SPQ_BIT_MASK_ORIG_HOST              = 8192;
   
   // Access roles
   public static final int NTCSS_USER_ROLE      = 0;
   public static final int APP_ADMIN_ROLE       = 1;
   public static final int LRS_NTCSS_ADMIN_ROLE = 1;
   public static final int NTCSS_ADMIN_ROLE     = 2;
   public static final int NTCSS_BATCH_USER_BIT = 1024;
   public static final int ROLE_BIT_BASE_OFFSET = 1;

   // User security roles
   public static final int UNCLASSIFIED_CLEARANCE            = 0;
   public static final int UNCLASSIFIED_SENSITIVE_CLEARANCE  = 1;
   public static final int CONFIDENTIAL_CLEARANCE            = 2;
   public static final int SECRET_CLEARANCE                  = 3;
   public static final int TOPSECRET_CLEARANCE               = 4;

   // Server process priorities.
   public static final int BOOT_PRIORITY      = -2;
   public static final int SCHEDULED_PRIORITY = -1;
   public static final int LOW_PRIORITY       =  1;
   public static final int MEDIUM_PRIORITY    =  2;
   public static final int HIGH_PRIORITY      =  3;
   
   //Server process JOB_Q constants
   public static final int JOB_Q_AGE_WEIGHT   =  1;
   public static final int JOB_Q_PRIORITY_WEIGHT = 100;

   // Server process change bit mask values
   public static final int CHANGE_CUST_STAT   = 1;
   public static final int CHANGE_PROC_STAT   = 2;
   public static final int CHANGE_PID_VALUE   = 4;
   public static final int CHANGE_END_TIME    = 8;
   public static final int CHANGE_LAUNCH_TIME = 16;
   public static final int CHANGE_PRIORITY    = 32;
   public static final int CHANGE_DEVICE      = 64;
   public static final int CHANGE_PRT_FLAG    = 128;

   // Server process states.
   public static final int WAITING_FOR_OK_STATE   = -1;  // active procs, scheduled procs 
   public static final int DEVICE_DEPENDENT_STATE = -2;  // active procs, scheduled procs 
   public static final int CRASHED_STATE          = -3;  // active procs 
   public static final int EXITED_STATE           = -4;  // active procs 
   public static final int WAITING_FOR_EXEC_STATE = -5;  // active procs 
   public static final int ABOUT_TO_BE_RUN_STATE  = -6;  // active procs 
   public static final int APPROVED_STATE         = -7;  // scheduled procs 
   public static final int RUN_NEXT_STATE         = -8;  // active procs 
   public static final int SDI_FAILURE_STATE      = -9;  // active procs 
   public static final int SCHED_TO_RUN_STATE     = -10; // scheduled procs 

   // Server process prt_flag field mask
   // 00000001 the file-needs-to-be-printed flag 
   public static final int PRT_FLAG_MASK1 = 0x01;
   // 00000010 the delete-file-after-it-is-printed flag 
   public static final int PRT_FLAG_MASK2 = 0x02;
   // 00000100 this may be the process-is-restartable flag 
   public static final int PRT_FLAG_MASK3 = 0x04;
   // 00001000 the sdi_io_flag  - either read (1) or write (0) 
   public static final int PRT_FLAG_MASK4 = 0x08;
   // 00010000 the sdi_required_flag - have to do a tape op
   public static final int PRT_FLAG_MASK5 = 0x10;
   // 00100000 SDI failed on last execution try 
   public static final int PRT_FLAG_MASK6 = 0x20;
   // 01000000 UNUSED 
   public static final int PRT_FLAG_MASK7 = 0x40;
   // 10000000 UNUSED 
   public static final int PRT_FLAG_MASK8 = 0x80;

   public static final int YEAR_TYPE   = 0;
   public static final int MONTH_TYPE  = 1;
   public static final int WEEK_TYPE   = 2;
   public static final int DAY_TYPE    = 3;
   public static final int HOUR_TYPE   = 4;
   public static final int MINUTE_TYPE = 5;      

   // Login history states.
   public static final int GOOD_LOGIN_STATE      = 0;
   public static final int FAILED_LOGIN_STATE    = 1;
   public static final int LOGOUT_STATE          = 2;
   public static final int RECORD_LOGOUT_STATE   = 3;
   public static final int LOGIN_RESERVED_STATE  = 4;
   public static final int REMOVE_RESERVED_STATE = 5;

   // System message bulletin board values.
   public static final int SYS_MSG_BB_PROCESS_TERMINATED = 1;
   public static final int SYS_MSG_BB_PRT_JOB_DELETED    = 2;           
   public static final int SYS_MSG_BB_PRT_JOB_RELEASED   = 3;

   // Message bulletin board types.
   public static final int MSG_BB_TYPE_USER    = 0;
   public static final int MSG_BB_TYPE_GENERAL = 1;
   public static final int MSG_BB_TYPE_SYSTEM  = 2;                      
   
   // Printing methods
   public static final String W2K_PRT_DRVR_EXT = "!2K";
   public static final String UNIX_PRINTER     = "SERVER";
   public static final String NT_SHARE_PRINTER = "NTSHARE";
   public static final String REGULAR_PRINTER  = "REGULAR";
   public static final String FILE_PRINTER     = "FILE";
      
   /**
    * Given a classification string, its corresponding integer representation
    *  is returned. If a null string is given, or the given string is an
    *  unknown classification, -1 is returned.
    */
   public static int getSecurityCode(String classificationString)
   {
      if (classificationString == null)
         return -1;
      
      if (classificationString.equals("Unclassified-Sensitive") == true)
         return UNCLASSIFIED_SENSITIVE_CLEARANCE;
      else if (classificationString.equals("Unclassified") == true) 
         return UNCLASSIFIED_CLEARANCE;
      else if (classificationString.equals("Confidential") == true)
         return CONFIDENTIAL_CLEARANCE;
      else if (classificationString.equals("Secret") == true)
         return SECRET_CLEARANCE;
      else if (classificationString.equals("Top Secret") == true)
         return TOPSECRET_CLEARANCE;
      else
         return -1;
   }

    /**
    * Given a classification int, its corresponding string representation
    *  is returned. If a null string is given, or the given string is an
    *  unknown classification, an empty String is returned.
    */
   public static String getSecurityString(int classificationInt)
   {
      if (classificationInt < 0 || classificationInt > 4)
         return "";
      
      if (classificationInt == 0)
         return "Unclassified-Sensitive";
      else if (classificationInt == 1) 
         return "Unclassified";
      else if (classificationInt == 2)
         return "Confidential";
      else if (classificationInt == 3)
         return "Secret";
      else if (classificationInt == 4)
         return "Top Secret";
      else
         return "";
   }
   
   /**
    * Given a program type string, its corresponding integer representation
    *  is returned. If a null string is given, or the given string is an
    *  unknown program type, -1 is returned.
    */
   public static int getProgramType(String programType)
   {
      if (programType == null)
         return -1;
      
      if (programType.equals("Client Program") == true)
         return 0;
      else if (programType.equals("Standard Predefined Job") == true)
         return 0;
      else if (programType.equals("Boot Job") == true) 
         return 1;
      else if (programType.equals("App. Admin. Server Program") == true)
         return 2;
      else if (programType.equals("Scheduled Job") == true)
         return 2;
      else if (programType.equals("Host Admin. Server Program") == true)
         return 3;
      else if (programType.equals("Superuser Server Program") == true)
         return 4;
      else if (programType.equals("Server Program") == true)
         return 1;
      else
         return -1;
   }   
   
   /**
    * Takes the given host type string and returns the corresponding integer 
    *  constant. Returns -1 if the given string is not a host type or the 
    *  given string is null.
    */
   public static int getHostIntType(String hosttype)
   {
      if (hosttype == null)
         return -1;
      
      if (hosttype.equals("Master Server") == true)
         return Constants.NTCSS_MASTER;
      else if (hosttype.equals("Application Server") == true) 
         return Constants.NTCSS_APP_SERVER;
      else if (hosttype.equals("Authentication Server") == true) 
         return Constants.NTCSS_AUTH_SERVER;
      else
         return -1;
   }
   
   /**
    * Takes the given host type integer constant and returns the corresponding 
    *  string constant. Returns null if the given integer does not correspond
    *  to a host type
    */
   public static String getHostStrType(int hosttype)
   {
      if (hosttype == Constants.NTCSS_MASTER)
         return "Master Server";         
      else if (hosttype == Constants.NTCSS_APP_SERVER)
         return "Application Server";         
      else if (hosttype == Constants.NTCSS_AUTH_SERVER)
         return "Authentication Server";         
      else
         return null;
   }
}