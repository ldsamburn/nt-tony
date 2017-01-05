/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs;

/**
 * A list of error codes
 */
public class ErrorCodes
{
   // Error codes
   public static final int ERROR_CANT_PARSE_MSG                         = -1;
   public static final int ERROR_CANT_ACCESS_DB                         = -2;
   public static final int ERROR_CANT_OPEN_DB_FILE                      = -3;
   public static final int ERROR_NO_SUCH_USER                           = -4;
   public static final int ERROR_INCORRECT_PASSWORD                     = -5;
   public static final int ERROR_NO_GROUP_INFO_FOUND                    = -6;
   public static final int ERROR_NO_PROGRAM_INFO_FOUND                  = -7;
   public static final int ERROR_NO_SUCH_APP_USER                       = -8;
   public static final int ERROR_NO_SUCH_GROUP                          = -9;
   public static final int ERROR_NO_PRT_INFO_FOUND                      = -10;
   public static final int ERROR_NO_PRT_CLS_INFO_FOUND                  = -11;
   public static final int ERROR_NO_OTYPE_INFO_FOUND                    = -12;
   public static final int ERROR_NO_SUCH_OTYPE                          = -13;
   public static final int ERROR_CANT_INSERT_VALUE                      = -14;
   public static final int ERROR_INCORRECT_OLD_PASSWD                   = -15;
   public static final int ERROR_INCORRECT_NEW_PASSWD                   = -16;
   public static final int ERROR_CANT_PARSE_ARGS                        = -17;
   public static final int ERROR_NO_SUCH_PRT                            = -18;
   public static final int ERROR_CANT_DETERMINE_HOST                    = -19;
   public static final int ERROR_NO_NEED_TO_SUSPEND                     = -20;
   public static final int ERROR_QUEUE_DISABLED                         = -21;
   public static final int ERROR_CANT_READ_VALUE                        = -22;
   public static final int ERROR_CANT_CREATE_QUERY                      = -23;
   public static final int ERROR_USER_ALREADY_IN                        = -24;
   public static final int ERROR_CANT_UNLOCK_LOGIN                      = -25;
   public static final int ERROR_CANT_ACCESS_LOGIN                      = -26;
   public static final int ERROR_CANT_GET_PID_DATA                      = -27;
   public static final int ERROR_CANT_DELETE_VALUE                      = -28;
   public static final int ERROR_USER_NOT_VALID                         = -29;
   public static final int ERROR_CANT_REL_REQUEST                       = -30;
   public static final int ERROR_CANT_KILL_SVR_PROC                     = -31;
   public static final int ERROR_CANT_REPLACE_VALUE                     = -32;
   public static final int ERROR_INVALID_NUM_ARGS                       = -33;
   public static final int ERROR_CANT_TALK_TO_SVR                       = -34;
   public static final int ERROR_NO_SUCH_FILE                           = -35;
   public static final int ERROR_CANT_REMOVE_ACT_PROCESS                = -36;
   public static final int ERROR_CANT_ACCESS_DIR                        = -37;
   public static final int ERROR_NTCSS_LP_FAILED                        = -38;
   public static final int ERROR_BB_MSG_DOES_NOT_EXIST                  = -39;
   public static final int ERROR_BB_DOES_NOT_EXIST                      = -40;
   public static final int ERROR_USER_LOGIN_DISABLED                    = -41;
   public static final int ERROR_CANT_OPEN_FILE                         = -42;
   public static final int ERROR_NOT_UNIX_USER                          = -43;
   public static final int ERROR_CANT_DETER_MASTER                      = -44;
   public static final int ERROR_CANT_COMM_WITH_MASTER                  = -45;
   public static final int ERROR_CANT_TRANSFER_FILE                     = -46;
   public static final int ERROR_CANT_CREATE_MASTER_BACKUP              = -47;
   public static final int ERROR_NO_SUCH_ITEM_REGISTERED                = -48;
   public static final int ERROR_SERVER_IDLED                           = -49;
   public static final int ERROR_SYSTEM_LOCKED                          = -50;
   public static final int ERROR_INVALID_COMDB_TAG_VALUE                = -51;
   public static final int ERROR_INVALID_COMDB_ITEM_VALUE               = -52;
   public static final int ERROR_CANT_FIND_VALUE                        = -53;
   public static final int ERROR_CANT_KILL_NON_ACTIVE_PROC              = -54;
   public static final int ERROR_NO_SUCH_HOST                           = -55;
   public static final int ERROR_NO_SUCH_PROCESS                        = -56;
   public static final int ERROR_CANT_SET_MASTER                        = -57;
   public static final int ERROR_CANT_UNPACK_MASTER_BACKUP              = -58;
   public static final int ERROR_DB_SOFTWARE_VERSION_MISMATCH           = -59;
   public static final int ERROR_DAEMON_IS_AWAKE                        = -60;
   public static final int ERROR_CANT_CHANGE_NTCSS_APP_FILE             = -61;
   public static final int ERROR_HOST_IS_NOT_MASTER                     = -62;
   public static final int ERROR_CANT_RENAME_FILE                       = -63;
   public static final int ERROR_INVALID_BB_NAME                        = -64;
   public static final int ERROR_BB_ALREADY_EXISTS                      = -65;
   public static final int ERROR_CHANGE_PASSWD_FAILED                   = -66;
   public static final int ERROR_CANT_SET_ADMIN_LOCK                    = -67;
   public static final int ERROR_CANT_SIGNAL_PROCESS                    = -68;
   public static final int ERROR_CANT_CREATE_INI_FILE                   = -69;
   public static final int ERROR_CANT_EXTRACT_INI_FILE                  = -70;
   public static final int ERROR_CANT_GET_FILE_INFO                     = -71;
   public static final int ERROR_PROCESS_NOT_IN_VALID_STATE_FOR_REMOVAL = -72;
   public static final int ERROR_RUN_NEXT_TAKEN                         = -73;
   public static final int ERROR_REMOVE_FROM_WRONG_PROGROUP             = -74;
   public static final int ERROR_KILL_FROM_WRONG_PROGROUP               = -75;
   public static final int ERROR_SDI_NO_TARGET_FILE                     = -76;
   public static final int ERROR_SDI_NO_TCI_FILE                        = -77;
   public static final int ERROR_SDI_NO_CNTRL_REC                       = -78;
   public static final int ERROR_CANT_LOCK_APPLICATION                  = -79;
   public static final int ERROR_CANT_UNLOCK_APPLICATION                = -80;
   public static final int ERROR_USER_LOGGED_IN                         = -81;
   public static final int ERROR_APP_TO_DM_PIPE_OPEN                    = -82;
   public static final int ERROR_APP_TO_DM_PIPE_WRITE                   = -83;
   public static final int ERROR_DM_SEMAPHORE                           = -84;
   public static final int ERROR_DM_TO_APP_PIPE_OPEN                    = -85;
   public static final int ERROR_DM_TO_APP_PIPE_READ                    = -86;
   public static final int ERROR_LOCAL_PROGRAM_INFO                     = -87;
   public static final int ERROR_NOT_USER_AUTH_SERVER_OR_MASTER         = -88;
   public static final int ERROR_NTCSS_BUILD_AND_PUSH_NIS_ERROR         = -89;
   public static final int ERROR_PASSWORD_TRANSLATION_FAILED            = -90;
   public static final int ERROR_SYSTEM_CMD_FAILED                      = -91;
   public static final int ERROR_CANT_CREATE_LRS                        = -92;
   public static final int ERROR_DECRYPTION_PROBLEM                     = -93;
   public static final int ERROR_DB_UPDATE_FAILED                       = -94;
   public static final int ERROR_DB_DELETE_FAILED                       = -95;
   public static final int ERROR_DB_QUERY_FAILED                        = -96;
   public static final int ERROR_PASSWORD_CHANGE_FAILED                 = -97;
   public static final int ERROR_WRONG_HOST                             = -98;
   public static final int ERROR_BAD_METHOD_ARGUMENT                    = -99;
   public static final int ERROR_BAD_DATA_FOUND                         = -100;
   public static final int ERROR_SERVER_NOT_RESPONDING                  = -101;
   public static final int ERROR_CAUGHT_EXCEPTION                       = -102;

   /**
    * Returns a string based on the given error code
    */
   public static String getErrorMessage(int errorCode)
   {
      switch (errorCode) {
         case ERROR_CANT_PARSE_MSG:
            return("Could not parse message.");
         case ERROR_CANT_ACCESS_DB:
            return("Could not access database.");
         case ERROR_CANT_OPEN_DB_FILE:
            return("Could not open database file.");
         case ERROR_NO_SUCH_USER:
            return("No such NTCSS user.");
         case ERROR_INCORRECT_PASSWORD:
            return("Incorrect password.");
         case ERROR_NO_GROUP_INFO_FOUND:
            return("Could not find application information.");
         case ERROR_NO_PROGRAM_INFO_FOUND:
            return("Could not find program information.");
         case ERROR_NO_SUCH_APP_USER:
            return("No such application user.");
         case ERROR_NO_SUCH_GROUP:
            return("No such application.");
         case ERROR_NO_PRT_INFO_FOUND:
            return("Could not find printer information.");
         case ERROR_NO_PRT_CLS_INFO_FOUND:
            return("Could not find printer class information.");
         case ERROR_NO_OTYPE_INFO_FOUND:
            return("Could not find output type information.");
         case ERROR_NO_SUCH_OTYPE:
            return("No such output type.");
         case ERROR_CANT_INSERT_VALUE:
            return("Could not insert data record.");
         case ERROR_INCORRECT_OLD_PASSWD:
            return("Incorrect old password.");
         case ERROR_INCORRECT_NEW_PASSWD:
            return("Invalid new password.");
         case ERROR_CANT_PARSE_ARGS:
            return("Could not parse command line arguments.");
         case ERROR_NO_SUCH_PRT:
            return("No such printer.");
         case ERROR_CANT_DETERMINE_HOST:
            return("Failed to determine hostname.");
         case ERROR_NO_NEED_TO_SUSPEND:
            return("No need to suspend print request.");
         case ERROR_QUEUE_DISABLED:
            return("Printer queue is disabled.");
         case ERROR_CANT_READ_VALUE:
            return("Could not read data record.");
         case ERROR_CANT_CREATE_QUERY:
            return("Could not create query file.");
         case ERROR_USER_ALREADY_IN:
            return("User already logged in.");
         case ERROR_CANT_UNLOCK_LOGIN:
            return("Could not unlock login files.");
         case ERROR_CANT_ACCESS_LOGIN:
            return("Could not access login files.");
         case ERROR_CANT_GET_PID_DATA:
            return("Could not determine PID data.");
         case ERROR_CANT_DELETE_VALUE:
            return("Could not delete data record.");
         case ERROR_USER_NOT_VALID:
            return("User cannot be validated.");
         case ERROR_CANT_REL_REQUEST:
            return("Could not release request.");
         case ERROR_CANT_KILL_SVR_PROC:
            return("Could not kill server process.");
         case ERROR_CANT_REPLACE_VALUE:
            return("Could not replace data record.");
         case ERROR_INVALID_NUM_ARGS:
            return("Invalid number of arguments.");
         case ERROR_CANT_TALK_TO_SVR:
            return("Could not communicate with daemon.");
         case ERROR_NO_SUCH_FILE:
            return("Failed to locate file.");
         case ERROR_CANT_REMOVE_ACT_PROCESS:
            return("Cannot remove active process.");
         case ERROR_CANT_ACCESS_DIR:
            return("Cannot access directory.");
         case ERROR_NTCSS_LP_FAILED:
            return("Server print command failed.");
         case ERROR_BB_MSG_DOES_NOT_EXIST:
            return("Bulletin board message does not exist.");
         case ERROR_BB_DOES_NOT_EXIST:
            return("Bulletin board does not exist.");
         case ERROR_USER_LOGIN_DISABLED:
            return("User login is disabled.");
         case ERROR_CANT_OPEN_FILE:
            return("Could not open file.");
         case ERROR_NOT_UNIX_USER:
            return("User has no UNIX account.");
         case ERROR_CANT_DETER_MASTER:
            return("Could not determine Master Server.");
         case ERROR_CANT_COMM_WITH_MASTER:
            return("Could not communicate with Master Server.");
         case ERROR_CANT_TRANSFER_FILE:
            return("Could not transfer file across network.");
         case ERROR_CANT_CREATE_MASTER_BACKUP:
            return("Could not create Master Server backup.");
         case ERROR_NO_SUCH_ITEM_REGISTERED:
            return("No such database item is registered.");
         case ERROR_SERVER_IDLED:
            return("System is idled and not accepting any requests.");
         case ERROR_SYSTEM_LOCKED:
            return("System is locked and not allowing login requests.");
         case ERROR_INVALID_COMDB_TAG_VALUE:
            return("Invalid common data tag value.");
         case ERROR_INVALID_COMDB_ITEM_VALUE:
            return("Invalid common data item value.");
         case ERROR_CANT_FIND_VALUE:
            return("Could not find database record.");
         case ERROR_CANT_KILL_NON_ACTIVE_PROC:
            return("Process is currently not running.");
         case ERROR_NO_SUCH_HOST:
            return("No such NTCSS Server.");
         case ERROR_NO_SUCH_PROCESS:
            return("No such server process.");
         case ERROR_CANT_SET_MASTER:
            return("Could not set value of Master Server.");
         case ERROR_CANT_UNPACK_MASTER_BACKUP:
            return("Could not unpack Master Server backup.");
         case ERROR_DB_SOFTWARE_VERSION_MISMATCH:
            return("Database and software version numbers are incompatible.");
         case ERROR_DAEMON_IS_AWAKE:
            return("Daemon is awake.");
         case ERROR_CANT_CHANGE_NTCSS_APP_FILE:
            return("Could not change NTCSS application initialization file.");
         case ERROR_HOST_IS_NOT_MASTER:
            return("This host is not a Master Server.");
         case ERROR_CANT_RENAME_FILE:
            return("Could not rename file.");
         case ERROR_INVALID_BB_NAME:
            return("Invalid bulletin board name.");
         case ERROR_BB_ALREADY_EXISTS:
            return("Bulletin board already exists.");
         case ERROR_CHANGE_PASSWD_FAILED:
            return("Change password script failed.");
         case ERROR_CANT_SET_ADMIN_LOCK:
            return("Could not set admin lock.");
         case ERROR_CANT_SIGNAL_PROCESS:
            return("Could not signal process.");
         case ERROR_CANT_CREATE_INI_FILE:
            return("Could not create INI file.");
         case ERROR_CANT_EXTRACT_INI_FILE:
            return("Could not extract INI file.");
         case ERROR_CANT_GET_FILE_INFO:
            return("Could not get file information.");
         case ERROR_PROCESS_NOT_IN_VALID_STATE_FOR_REMOVAL:
            return("Process is not in a valid state for removal.");
         case ERROR_RUN_NEXT_TAKEN:
            return("Run Next Process Exists!");
         case ERROR_REMOVE_FROM_WRONG_PROGROUP:
            return("Not allowed to remove a process queue item in a" +
                   " different program group.");
         case ERROR_KILL_FROM_WRONG_PROGROUP:
            return("Not allowed to stop a process in a different" +
                   " program group.");
         case ERROR_CANT_LOCK_APPLICATION:
            return("Failed to lock the application.");
         case ERROR_CANT_UNLOCK_APPLICATION:
            return("Failed to unlock the application.");
         case ERROR_USER_LOGGED_IN:
            return("User logged in.");
         case ERROR_APP_TO_DM_PIPE_OPEN:
            return("Error opening pipe from App to DM");
         case ERROR_APP_TO_DM_PIPE_WRITE:
            return("Error writing to pipe from App to DM");
         case ERROR_DM_SEMAPHORE:
            return("Error on DM Pipe semaphore");
         case ERROR_DM_TO_APP_PIPE_OPEN:
            return("Error opening pipe from DM to App");
         case ERROR_DM_TO_APP_PIPE_READ:
            return("Error reading pipe from DM to App");
         case ERROR_LOCAL_PROGRAM_INFO:
            return("Error with local program info");
         case ERROR_NOT_USER_AUTH_SERVER_OR_MASTER:
            return("Error.  Server is not the Master or user's PAS");
         case ERROR_NTCSS_BUILD_AND_PUSH_NIS_ERROR:
            return("Error building NTCSS NIS Maps");
         case ERROR_PASSWORD_TRANSLATION_FAILED:
            return("Error password translation failed");
         case ERROR_CANT_CREATE_LRS:
            return("Error creating LRS");
         case ERROR_DECRYPTION_PROBLEM:
            return("Failed to decrypt a string");
         case ERROR_DB_UPDATE_FAILED:
            return("Failed to update database record(s)");
         case ERROR_DB_DELETE_FAILED:
            return("Failed to remove database record(s)");
         case ERROR_PASSWORD_CHANGE_FAILED:
            return("Failed to change user's password");
         case ERROR_WRONG_HOST:
            return("Incorrect host for this operation");
         case ERROR_BAD_METHOD_ARGUMENT:
            return("Argument is unexpectedly null");
         case ERROR_BAD_DATA_FOUND:
            return("Expected data either not found or corrupt");
         case ERROR_SERVER_NOT_RESPONDING:
            return("No response from server/service");
         case ERROR_CAUGHT_EXCEPTION:
            return("An exception was caught");
         default:
            return("INVALID ERROR NUMBER!");
      }
   }
}
