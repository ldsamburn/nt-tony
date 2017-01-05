/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  NtcssApiP.h
*/

/* Most of the size defs were replaced with the proper Ntcss.h equivalents,
   but some may be unresolved and could become cause for concern if related
   values are changed in Ntcss.h.  DWB 12/98
*/

#ifndef _NtcssApiP_h
#define _NtcssApiP_h

#include "NtcssApi.h"


/* DEFINITIONS FOR TESTING */

#define TEST_PROCESS_ID 0
#define INTERACTIVE_TESTS 1

/* API CODE DEFINITIONS */

#define API_CODE_NtcssStartBootJobs               0
#define API_CODE_NtcssFilteredServerProcessList   1
#define API_CODE_NtcssFilteredServerProcessListCt 2
#define API_CODE_NtcssGetAppData                  3
#define API_CODE_NtcssGetAppName                  4
#define API_CODE_NtcssGetAppPassword              5
#define API_CODE_NtcssGetAppRole                  6
#define API_CODE_NtcssGetAppUserInfo              7
#define API_CODE_NtcssGetCommonData               8
#define API_CODE_NtcssGetDateTime                 9
#define API_CODE_NtcssGetHostName                 10 
#define API_CODE_NtcssGetLinkData                 11
#define API_CODE_NtcssGetLocalIP                  12
#define API_CODE_NtcssGetServerName               13
#define API_CODE_NtcssGetUserInfo                 14
#define API_CODE_NtcssGetUserRole                 15
#define API_CODE_NtcssPostAppMessage              16
#define API_CODE_NtcssPostMessage                 17
#define API_CODE_NtcssPostSystemMessage           18
#define API_CODE_NtcssPrintServerFile             19
#define API_CODE_NtcssSetAppData                  20
#define API_CODE_NtcssSetLinkData                 21
#define API_CODE_NtcssStartServerProcess          22
#define API_CODE_NtcssStopServerProcess           23
#define API_CODE_NtcssRemoveServerProcess         24
#define API_CODE_NtcssGetAppHost                  25
#define API_CODE_NtcssGetAuthServer               26
#define API_CODE_NtcssGetAppDataServer            27
#define API_CODE_NtcssValidateUser                28


/* PORT DEFINITIONS */

#define COMMAND_PORT               CMD_SVR_PORT
#define DATABASE_PORT              DB_SVR_PORT
#define MESSAGE_PORT               MSG_SVR_PORT
#define PRINTER_PORT               PRINT_PROXY_PORT
#define SERVER_PROCESS_QUEUE_PORT  SPQ_SVR_PORT
#define TIME_PORT                  13


/* GENERAL DEFINITIONS */

#define DB_DIR                             NTCSS_DB_DIR
#define MASTER                             "MASTER"
#define MIN_TIMEOUT_VALUE                  40
#define MAX_TIMEOUT_VALUE                  55
#define I_AM_MASTER                        "I AM MASTER"
#define MASTER_SERVER                      "$MASTER$"
#define CURRENT_SERVER                     "$CURRENT$"
#define PRETOKEN                           "XXXXXX"
#define PROGROUP_TITLE                     "NTCSS"
#define PID_ENVIRONMENT_VARIABLE           "CURRENT_NTCSS_PID"
#define PROGRAM_GROUP_ENVIRONMENT_VARIABLE "CURRENT_NTCSS_GROUP"
#define SPACE                              ' '
#define ZERO    /* So, what is "0" ?? */   '\0'
#define USE_APP_NAME_CODE                  "USE_APP_NAME_CODE"
#define MSG_ID_START_POS                   17
#define MSG_ID_LEN                         13
#define MAX_REQUEST_SIZE                   2000
#define PRINT_OUTPUT_FLAG                  1
#define DEL_AFTER_PRINT_FLAG               2
#define RESTART_FLAG                       4



/* TYPES OF PREDEFINED JOBS */

#define PREDEFINED_JOB 0
#define BOOT_JOB       1
#define SCHEDULED_JOB  2

/* SIZE DEFINITIONS */

/*#define ACCESS_INFO_SIZE                NTCSS_SIZE_APP_PROGRAM_INFO*/
#define ACCESS_ROLE_BUFFER_SIZE         1
/* #define APP_DATA_SIZE                   MAX_APP_DATA_LEN */
/* #define APP_PASSWORD_SIZE               MAX_APP_PASSWD_LEN */
/* #define COMMAND_LINE_SIZE               MAX_PROC_CMD_LINE_LEN */
/* #define COMMON_RESPONSE_SIZE            MAX_COMDB_DATA_LEN */
/* #define COMMON_TAG_SIZE                 MAX_COMDB_TAG_LEN */
/* #define DATE_TIME_SIZE                  NTCSS_SIZE_DATETIME1 */
#define DEFAULT_PRINTER_ONLY_SIZE       1
#define DYNAMIC_RESPONSE_SIZE           0
/* #define ERROR_MESSAGE_SIZE              MAX_ERR_MSG_LEN */
#define FILE_NAME_SIZE                  512
#define FILTER_MASK_SIZE                6
/* #define FIRST_NAME_SIZE                 NTCSS_SIZE_USERFIRSTNAME */
#define FLAG_BUFFER_SIZE                1
#define GENERAL_ACCESS_SIZE             1
/* #define HOST_NAME_SIZE                  MAX_HOST_NAME_LEN */
/* #define IP_ADDRESS_SIZE                 MAX_IP_ADDRESS_LEN */
/* #define JOB_DESCRIPTION_SIZE            MAX_JOB_DESCRIP_LEN */
/* #define LAST_NAME_SIZE                  NTCSS_SIZE_USERLASTNAME */
/* #define LOGIN_NAME_SIZE                 MAX_LOGIN_NAME_LEN */
/* #define MESSAGE_LABEL_SIZE              20 CMD_LEN*/ 
#define MESSAGE_ID_SIZE                 22
#define MESSAGE_TITLE_SIZE              32
/* #define MIDDLE_NAME_SIZE                NTCSS_SIZE_USERMIDDLENAME */
#define NUMBER_HEADER_SIZE              5
#define NUMBER_OF_COPIES_SIZE           2
/* #define OPTIONS_SIZE                    NTCSS_MAX_PRT_OPTIONS_LEN */
/* #define OUTPUT_TYPE_SIZE                NTCSS_SIZE_PRT_OUTPUT_TYPE */
/* #define PASSWORD_SIZE                   MAX_PASSWORD_LEN */
/* #define PHONE_NUMBER_SIZE               MAX_PHONE_NUMBER_LEN */
/* #define PID_SIZE                        MAX_PROCESS_ID_LEN */
#define PRETOKEN_SIZE                   6
#define PRINTER_FILENAME_SIZE           128
#define PRINTER_FLAG_STRING_SIZE        2
/* #define PRINTER_NAME_SIZE               MAX_PRT_NAME_LEN */
#define PRINT_FILE_NAME_SIZE            128
#define PRIORITY_BUFFER_SIZE            2
#define PRIORITY_CODE_SIZE              2         
/* #define PROGROUP_TITLE_SIZE             MAX_PROGROUP_TITLE_LEN */
#define REAL_NAME_SIZE                  64
#define REQUEST_LABEL_SIZE              20                
/* #define SCHEDULE_STRING_SIZE            NTCSS_MAX_SCHEDULE_LEN */
#define SCHEDULED_FLAG_SIZE             2
/* #define SDI_CONTROL_RECORD_SIZE         MAX_SDI_CTL_RECORD_LEN + 1 */
/* #define SDI_DATA_FILE_SIZE              MAX_SERVER_LOCATION_LEN */
/* #define SDI_DEVICE_FILE_SIZE            MAX_SERVER_LOCATION_LEN */
#define SDI_INPUT_FLAG_SIZE             1
#define SDI_REQUIRED_FLAG_SIZE          1
/* #define SDI_TCI_FILE_SIZE               MAX_SERVER_LOCATION_LEN */
#define SECURITY_CLASS_SIZE             2
#define SECURITY_LEVEL_SIZE             1
#define SIGNAL_TYPE_SIZE                2
/* #define SERVER_LOCATION_SIZE            MAX_SERVER_LOCATION_LEN */
/* #define SSN_SIZE                        NTCSS_SIZE_SSN */
#define START_PROCESS_FLAG_SIZE         2
#define TINY_GENBUF_SIZE                32
/* #define TOKEN_SIZE                      MAX_TOKEN_LEN */
#define PRINTER_COPIES_SIZE             2
#define PRINTER_BANNER_SIZE             1
#define PRINTER_ORIENTATION_SIZE        2
#define PRINTER_PAPERSIZE_SIZE          2

#define GET_COMMON_DATA_ACCEPT_LABEL   "GOODGETCOMMONDB"
#define GET_COMMON_DATA_REJECT_LABEL   "NOGETCOMMONDB"
#define GET_COMMON_DATA_REQUEST_LABEL  "GETCOMMONDB"
#define GET_COMMON_DATA_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_USER_DATA_REQUEST_LABEL  "GETUSERDATA"
#define GET_USER_DATA_ACCEPT_LABEL   "GOODGETUSERDATA"
#define GET_USER_DATA_REJECT_LABEL   "NOGETUSERDATA"
#define GET_USER_DATA_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define GET_USER_DATA_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

typedef struct{
  char login_name[MAX_LOGIN_NAME_LEN + 1];
  char first_name[NTCSS_SIZE_USERFIRSTNAME + 1];
  char middle_name[NTCSS_SIZE_USERMIDDLENAME + 1];
  char last_name[NTCSS_SIZE_USERLASTNAME + 1];
  char ssn[NTCSS_SIZE_SSN + 1];
  char phone_number[MAX_PHONE_NUMBER_LEN + 1];
  int security_class;
  int unix_id;
  int user_role;
  char app_data[MAX_APP_DATA_LEN + 1];
  char app_passwd[MAX_APP_PASSWD_LEN + 1];
  int registered_user;
  char program_access[NTCSS_SIZE_APP_PROGRAM_INFO + 1];
} common_user_data_str;


#define GET_DATE_TIME_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_DEFAULT_PRINTER_INFO_REQUEST_LABEL   "GETDEFPRTOTYPE"
#define GET_DEFAULT_PRINTER_INFO_ACCEPT_LABEL    "GOODDEFPRTOTYPE"
#define GET_DEFAULT_PRINTER_INFO_REJECT_LABEL    "NODEFPRTOTYPE"
#define GET_DEFAULT_PRINTER_INFO_RESPONSE_SIZE   60
#define GET_DEFAULT_PRINTER_INFO_TIMEOUT_VALUE   MIN_TIMEOUT_VALUE

#define DEFAULT_PRINTER_ONLY              "1"
#define PRINTER_OUTPUT_FLAG_BIT_POSITION  0
#define SAVE_OUTPUT_FLAG_BIT_POSITION     1
#define RESTART_FLAG_BIT_POSITION         2

#define START_SERVER_PROCESS_REQUEST_LABEL   "LAUNCHSVRPROCBG"
#define START_SERVER_PROCESS_ACCEPT_LABEL    "GOODSVRPROC"
#define START_SERVER_PROCESS_REJECT_LABEL    "NOSVRPROC"
#define START_SERVER_PROCESS_RESPONSE_SIZE   60
#define START_SERVER_PROCESS_TIMEOUT_VALUE   MAX_TIMEOUT_VALUE

#define START_SERVER_PROCESS_NEED_DEVICE     "-2"
#define START_SERVER_PROCESS_NEED_APPROVAL   "-1"
#define START_SERVER_PROCESS_READY_TO_START  "-5"

#define SCHEDULE_SERVER_PROCESS_REQUEST_LABEL   "SCHEDSVRPROCBG"
#define SCHEDULE_SERVER_PROCESS_ACCEPT_LABEL    "GOODSVRPROC"
#define SCHEDULE_SERVER_PROCESS_REJECT_LABEL    "NOSVRPROC"
#define SCHEDULE_SERVER_PROCESS_RESPONSE_SIZE   60
#define SCHEDULE_SERVER_PROCESS_TIMEOUT_VALUE   MIN_TIMEOUT_VALUE

#define SCHEDULE_SERVER_PROCESS_WAITING_FOR_APPROVAL     "-1"
#define SCHEDULE_SERVER_PROCESS_DEVICE_DEPENDENT         "-2"
#define SCHEDULE_SERVER_PROCESS_APPROVED                 "-7"

#define STOP_SERVER_PROCESS_REQUEST_LABEL     "KILLSVRPROC"
#define STOP_SERVER_PROCESS_ACCEPT_LABEL      "GOODKILLSVRPROC"
#define STOP_SERVER_PROCESS_REJECT_LABEL      "NOKILLSVRPROC"
#define STOP_SERVER_PROCESS_RESPONSE_SIZE     DYNAMIC_RESPONSE_SIZE
#define STOP_SERVER_PROCESS_TIMEOUT_VALUE     MIN_TIMEOUT_VALUE

#define SET_APP_DATA_REQUEST_LABEL "CHANGEAPPDATA"
#define SET_APP_DATA_ACCEPT_LABEL  "GOODAPPDATACHANGE"
#define SET_APP_DATA_REJECT_LABEL  "NOAPPDATACHANGE"
#define SET_APP_DATA_RESPONSE_SIZE DYNAMIC_RESPONSE_SIZE
#define SET_APP_DATA_TIMEOUT_VALUE MIN_TIMEOUT_VALUE

#define PRINT_SERVER_FILE_REQUEST_LABEL  "PRINTSVRFILE"
#define PRINT_SERVER_FILE_ACCEPT_LABEL   "GOODPRINTSVRFILE"
#define PRINT_SERVER_FILE_REJECT_LABEL   "NOPRINTSVRFILE"
#define PRINT_SERVER_FILE_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define PRINT_SERVER_FILE_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_BOOT_JOBS_REQUEST_LABEL  "GETBOOTJOBLIST"
#define GET_BOOT_JOBS_ACCEPT_LABEL   "GOODGETBOOTJOBLIST"
#define GET_BOOT_JOBS_REJECT_LABEL   "NOGETBOOTJOBLIST"
#define GET_BOOT_JOBS_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define GET_BOOT_JOBS_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define SET_LINK_DATA_REQUEST_LABEL  "SETLINKDATA"
#define SET_LINK_DATA_ACCEPT_LABEL   "GOODSETLINKDATA"
#define SET_LINK_DATA_REJECT_LABEL   "NOSETLINKDATA"
#define SET_LINK_DATA_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define SET_LINK_DATA_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_LINK_DATA_REQUEST_LABEL  "GETLINKDATA"
#define GET_LINK_DATA_ACCEPT_LABEL   "GOODGETLINKDATA"
#define GET_LINK_DATA_REJECT_LABEL   "NOGETLINKDATA"
#define GET_LINK_DATA_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define GET_LINK_DATA_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_BATCH_USER_REQUEST_LABEL  "GETBATCHUSER"
#define GET_BATCH_USER_ACCEPT_LABEL   "GOODGETBATCHUSER"
#define GET_BATCH_USER_REJECT_LABEL   "NOGETBATCHUSER"
#define GET_BATCH_USER_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define GET_BATCH_USER_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_NEW_MESSAGE_FILE_REQUEST_LABEL  "GETNEWMSGFILE"
#define GET_NEW_MESSAGE_FILE_ACCEPT_LABEL   "GOODGETNEWMSGFILE"
#define GET_NEW_MESSAGE_FILE_REJECT_LABEL   "NOGETNEWMSGFILE"
#define GET_NEW_MESSAGE_FILE_RESPONSE_SIZE  600
#define GET_NEW_MESSAGE_FILE_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define CREATE_MESSAGE_FILE_REQUEST_LABEL  "CREATEMSGFILE"
#define CREATE_MESSAGE_FILE_ACCEPT_LABEL   "GOODCREATEMSGFILE"
#define CREATE_MESSAGE_FILE_REJECT_LABEL   "NOCREATEMSGFILE"
#define CREATE_MESSAGE_FILE_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define CREATE_MESSAGE_FILE_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define SEND_BB_MESSAGE_REQUEST_LABEL  "SENDBBMSG"
#define SEND_BB_MESSAGE_ACCEPT_LABEL   "GOODSENDBBMSG"
#define SEND_BB_MESSAGE_REJECT_LABEL   "NOSENDBBMSG"
#define SEND_BB_MESSAGE_RESPONSE_SIZE  100
#define SEND_BB_MESSAGE_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_USERS_ACCESS_ROLE_REQUEST_LABEL  "GETACCESSROLE"
#define GET_USERS_ACCESS_ROLE_ACCEPT_LABEL   "GOODGETACCESSROLE"
#define GET_USERS_ACCESS_ROLE_REJECT_LABEL   "NOGETACCESSROLE"
#define GET_USERS_ACCESS_ROLE_RESPONSE_SIZE  100
#define GET_USERS_ACCESS_ROLE_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define GET_FILTERED_SERVER_PROCESS_LIST_REQUEST_LABEL  "SVRPROCLIST"
#define GET_FILTERED_SERVER_PROCESS_LIST_ACCEPT_LABEL   "GOODSPQUE"
#define GET_FILTERED_SERVER_PROCESS_LIST_REJECT_LABEL   "NOSPQUE"
#define GET_FILTERED_SERVER_PROCESS_LIST_RESPONSE_SIZE  DYNAMIC_RESPONSE_SIZE
#define GET_FILTERED_SERVER_PROCESS_LIST_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define REMOVE_SERVER_PROCESS_REQUEST_LABEL     "REMOVESPQITEM"
#define REMOVE_SERVER_PROCESS_ACCEPT_LABEL      "GOODREMOVESPQITEM"
#define REMOVE_SERVER_PROCESS_REJECT_LABEL      "NOREMOVESPQITEM"
#define REMOVE_SERVER_PROCESS_RESPONSE_SIZE     DYNAMIC_RESPONSE_SIZE
#define REMOVE_SERVER_PROCESS_TIMEOUT_VALUE     MIN_TIMEOUT_VALUE

#define GET_APP_LIST_REQUEST_LABEL  "GETAPPLIST"
#define GET_APP_LIST_ACCEPT_LABEL   "GOODAPPLIST"
#define GET_APP_LIST_REJECT_LABEL   "NOAPPLIST"
#define GET_APP_LIST_RESPONSE_SIZE  2048
#define GET_APP_LIST_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE

#define VALIDATE_USER_REQUEST_LABEL  "VALIDATEUSER"
#define VALIDATE_USER_ACCEPT_LABEL   "GOODVALIDATEUSER"
#define VALIDATE_USER_REJECT_LABEL   "NOVALIDATEUSER"
#define VALIDATE_USER_RESPONSE_SIZE  2048
#define VALIDATE_USER_TIMEOUT_VALUE  MIN_TIMEOUT_VALUE



#endif /* _NtcssApiP_h */
