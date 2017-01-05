/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
 * NtcssApi.h
 */

#include <Ntcss.h>
#ifndef _NtcssApi_h
#define _NtcssApi_h

/** DEFINE SIZES **/

/* Attempted to replace some of these with the standard Ntcss.h #defines.
   The unmapped ones may cause problems when values are adjusted in Ntcss.h and
   not here!  -DWB 12/98
   */

#define NTCSS_SIZE_APP_DATA            MAX_APP_DATA_LEN 
#define NTCSS_SIZE_APP_FLAG            2    /* ? */
#define NTCSS_SIZE_APP_PROGRAM_INFO    320  /* MAX_APP_INFO_LEN ? */
#define NTCSS_SIZE_APP_GROUP           MAX_PROGROUP_TITLE_LEN
#define NTCSS_SIZE_APP_PASSWORD        MAX_APP_PASSWD_LEN
#define NTCSS_SIZE_CLIENT_LOCATION     MAX_CLIENT_LOCATION_LEN
#define NTCSS_SIZE_CLIENT_WS           16  /* ? */
#define NTCSS_SIZE_COMMON_DATA         MAX_COMDB_DATA_LEN
#define NTCSS_SIZE_COMMON_TAG          MAX_COMDB_TAG_LEN
#define NTCSS_SIZE_COMMAND_LINE        MAX_PROC_CMD_LINE_LEN
#define NTCSS_SIZE_CUSTOM_PROC_STAT    MAX_CUST_PROC_STAT_LEN
#define NTCSS_SIZE_DATETIME1           27  /* ? */
#define NTCSS_SIZE_DATETIME2           13  /* ? */
#define NTCSS_SIZE_FILENAME            32  /* ? */
#define NTCSS_SIZE_GROUP_TITLE         MAX_PROGROUP_TITLE_LEN
#define NTCSS_SIZE_HOSTNAME            MAX_HOST_NAME_LEN
#define NTCSS_SIZE_JOB_DESCRIP         MAX_JOB_DESCRIP_LEN
#define NTCSS_SIZE_IP_ADDR             MAX_IP_ADDRESS_LEN
#define NTCSS_SIZE_LOGINNAME           MAX_LOGIN_NAME_LEN
#define NTCSS_SIZE_MSG_BOARD_NAME      MAX_BB_NAME_LEN
#define NTCSS_SIZE_ORIG_HOST           MAX_ORIG_HOST_LEN
#define NTCSS_SIZE_PASSWORD            MAX_PASSWORD_LEN
#define NTCSS_SIZE_PHONENUMBER         MAX_PHONE_NUMBER_LEN
#define NTCSS_SIZE_PRINTERNAME         MAX_PRT_NAME_LEN
#define NTCSS_SIZE_PROCESS_ARGS        MAX_CMD_LINE_ARGS_LEN
#define NTCSS_SIZE_PROCESS_ID          MAX_PROCESS_ID_LEN
#define NTCSS_SIZE_PRT_OUTPUT_TYPE     16
#define NTCSS_SIZE_REQUEST_ID          MAX_REQUEST_ID_LEN /* 36? was 4 */
#define NTCSS_SIZE_SDI_CONTROL_REC_ID  MAX_SDI_CTL_RECORD_LEN /* 8? was 5 */
#define NTCSS_SIZE_SERVER_LOCATION     MAX_SERVER_LOCATION_LEN
#define NTCSS_SIZE_SSN                 11  /* Probably OK */
#define NTCSS_SIZE_USERFIRSTNAME       32
#define NTCSS_SIZE_USERLASTNAME        32
#define NTCSS_SIZE_USERMIDDLENAME      32
#define NTCSS_MAX_ERROR_MSG_LEN        MAX_ERR_MSG_LEN
#define NTCSS_MAX_MESSAGE_TITLE_LEN    MAX_MSG_TITLE_LEN
#define NTCSS_MAX_MESSAGE_LEN          MAX_MSG_LEN
#define NTCSS_MAX_SCHEDULE_LEN         128 /* MAX_SCHED_STR_LEN == 8 */
#define NTCSS_MAX_PRT_OPTIONS_LEN      MAX_PRT_OPTIONS_LEN

/*** DEFINE CONSTANTS **/

#define True                           1
#define False                          0
#define NTCSS_UNCLASSIFIED             UNCLASSIFIED_CLEARANCE 
#define NTCSS_UNCLASSIFIED_SENSITIVE   UNCLASSIFIED_SENSITIVE_CLEARANCE
#define NTCSS_CONFIDENTIAL             CONFIDENTIAL_CLEARANCE
#define NTCSS_SECRET                   SECRET_CLEARANCE
#define NTCSS_TOPSECRET                TOPSECRET_CLEARANCE
#define NTCSS_LOW_PRIORITY             LOW_PRIORITY
#define NTCSS_MED_PRIORITY             MEDIUM_PRIORITY
#define NTCSS_HIGH_PRIORITY            HIGH_PRIORITY
#define NTCSS_STOP_PROCESS_SOFT        0
#define NTCSS_STOP_PROCESS_HARD        1
#define NTCSS_NON_SCHEDULED_JOB        0
#define NTCSS_SCHEDULED_JOB            1
#define NTCSS_PRINT_NOT                0
#define NTCSS_PRINT_AND_REMOVE         1
#define NTCSS_PRINT_AND_SAVE           2

/** DEFINE TYPES AND STRUCTURES **/

typedef int BOOL;

typedef struct NtcssUserInfo{
  char          login_name[MAX_LOGIN_NAME_LEN];
  char          real_first_name[NTCSS_SIZE_USERFIRSTNAME];
  char          real_middle_name[NTCSS_SIZE_USERMIDDLENAME];
  char          real_last_name[NTCSS_SIZE_USERLASTNAME];
  char          ssn[NTCSS_SIZE_SSN];
  char          phone_number[MAX_PHONE_NUMBER_LEN];
  int           security_class;
  int           unix_id;
  char          app_passwd[MAX_PASSWORD_LEN];
} NtcssUserInfo;


   /* Use the other structure for clarity - it's names are identical to the
      database fileds.   */
typedef struct NtcssServerProcessDescr{
  char owner_str[MAX_LOGIN_NAME_LEN];
  char prog_name_str[MAX_SERVER_LOCATION_LEN];
  char prog_args_str[MAX_CMD_LINE_ARGS_LEN];
  char job_descrip_str[MAX_JOB_DESCRIP_LEN];
  int  seclvl;
  int  priority_code;
  BOOL needs_approval_flag;
  BOOL needs_device_flag;
  BOOL restartable_flag;
  BOOL prt_output_flag;
  char prt_filename_str[MAX_SERVER_LOCATION_LEN];
  char output_type_str[NTCSS_SIZE_PRT_OUTPUT_TYPE];
  BOOL use_default_prt_flag;
  BOOL general_access_prt_flag;
  BOOL save_prt_output_flag;
  BOOL sdi_required_flag;
  BOOL sdi_input_flag;
  char sdi_control_id_str[MAX_SDI_CTL_RECORD_LEN + 1];
  char sdi_data_file_str[MAX_SERVER_LOCATION_LEN];
  char sdi_tci_file_str[MAX_SERVER_LOCATION_LEN];
  int copies;
  int banner;
  int orientation;
  int papersize;
} NtcssServerProcessDescr; 


typedef struct NtcssServerProcessDetails{
    int  pid;                               
    char process_id_str[MAX_PROCESS_ID_LEN + 1]; 
    int  seclvl;                            
    int  priority_code;                     
    int  status_code;                       
    int  job_type_code;                     
    BOOL restartable_flag;
    char login_name_str[MAX_LOGIN_NAME_LEN + 1];
    char unix_owner_str[MAX_LOGIN_NAME_LEN + 1];
    char app_name_str[MAX_PROGROUP_TITLE_LEN + 1];
    char command_line_str[MAX_PROC_CMD_LINE_LEN + 1];
    char custom_process_stat_str[MAX_CUST_PROC_STAT_LEN + 1];
    char job_descrip_str[MAX_JOB_DESCRIP_LEN + 1];
    char request_time[NTCSS_SIZE_DATETIME2 + 1];
    char launch_time[NTCSS_SIZE_DATETIME2 + 1];
    char end_time[NTCSS_SIZE_DATETIME2 + 1];
    char orig_host_str[MAX_ORIG_HOST_LEN + 1];
    char printer_name_str[MAX_PRT_NAME_LEN + 1];
    char printer_host_name_str[MAX_HOST_NAME_LEN + 1];
    int  print_seclvl;
    char prt_filename_str[MAX_CLIENT_LOCATION_LEN + 1];
    int  print_code;
    char sdi_control_record_id_str[MAX_SDI_CTL_RECORD_LEN + 2];
    char sdi_data_file_str[MAX_SERVER_LOCATION_LEN + 1];
    char sdi_tci_file_str[MAX_SERVER_LOCATION_LEN + 1];
    char sdi_device_data_file_str[MAX_SERVER_LOCATION_LEN + 1];
    int copies;
    int banner;
    int orientation;
    int papersize;
} NtcssServerProcessDetails; 

typedef struct NtcssProcessFilter {
  BOOL get_scheduled_jobs;
  BOOL get_unscheduled_jobs;
  BOOL get_only_running_jobs;
  BOOL get_for_current_user_only;
  BOOL priority_flag;
  BOOL command_line_flag;
  BOOL cust_proc_status_flag;
  BOOL login_name_flag;
  BOOL unix_owner_flag;
  BOOL progroup_title_flag;
  BOOL job_descrip_flag;
  BOOL orig_host_flag;
  int priority;
  char command_line[MAX_PROC_CMD_LINE_LEN + 1];
  char cust_proc_status[MAX_CUST_PROC_STAT_LEN + 1];
  char login_name[MAX_LOGIN_NAME_LEN + 1];
  char unix_owner[MAX_LOGIN_NAME_LEN + 1];
  char progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char job_descrip[MAX_JOB_DESCRIP_LEN + 1];
  char orig_host[MAX_ORIG_HOST_LEN + 1];
} NtcssProcessFilter;



/** API FUNCTION PROTOTYPES **/

extern BOOL NtcssGetLastError(char*, int);
/* 
 * char *error_msg;      - (OUTPUT) The error message buffer.
 * int   error_msg_size; - (INPUT)  The size of the error message
                                    buffer (NTCSS_MAX_ERROR_MSG_LEN).
 *
 * Returns True when successful.
 * Returns False when there was an error.
 */

extern BOOL NtcssGetAppData(char*, int);       
/* 
 * char *app_data_buffer;      - (OUTPUT) Application data buffer.
 * int   app_data_buffer_size; - (INPUT)  Size of the application buffer
                                          (MAX_APP_DATA_LEN).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
   message).
 */

extern BOOL NtcssGetAppPassword(char*, int);   
/* 
 * char *password_buffer;      - (OUTPUT) Password buffer.
 * int   password_buffer_size; - (INPUT)  Size of the password buffer
                                           (NTCSS_SIZE_APP_PASSWORD).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get
    error message).
 */ 

extern BOOL NtcssGetAppUserInfo(char*, NtcssUserInfo*, char*, int);   
/* 
 * char *user_name;              - (INPUT)  The user name.
 * NtcssUserInfo *user_info_ptr; - (OUTPUT) User info for the given user name.
 * char *prog_access_buf;        - (OUTPUT) Pogram access buffer for the given
                                             user name.
 * int prog_access_buf_size      - (INPUT)  Size of the program access buffer
                                             (NTCSS_SIZE_APP_PROGRAM_INFO).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
     message).
 */

extern BOOL NtcssGetCommonData(char*, char*, int);
/*
 * char *tag;            - (INPUT)  The tag to retrieve.
 * char *data_buffer;    - (OUTPUT) The data buffer.
 * int data_buffer_size; - (INPUT)  The size of the data buffer
                                     (NTCSS_SIZE_COMMON_DATA).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetDateTime(char*, int);
/*
 * char *date_time_buffer;    - (OUTPUT) The date/time buffer.
 * int date_time_buffer_size; - (INPUT)  The size of the date/time buffer
                                          (NTCSS_SIZE_DATETIME1).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern int NtcssGetFilteredServerProcessList(NtcssProcessFilter*, 
                                             NtcssServerProcessDetails*, int);
/*
   ProcessFilter *process_filter;            - (INPUT)  A record of filters.
   NtcssServerProcessDetails **process_list  - (OUTPUT) A list of process
                                                         details.
   int list_buffer_count                     - (INPUT)  The number of processes
                                                         process_list can hold.
  
   Returns the number of processes in process_list.
   Returns -1 when there was an error (use NtcssGetLastError() to get error
     message).
 */

extern int NtcssGetFilteredServerProcessListCt(NtcssProcessFilter*);
/*
 * ProcessFilter *process_filter;            - (INPUT)  A record of filters.
 *
 * Returns the number of processes found.
 * Returns -1 when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetHostName(char*, int);
/*
 * char *host_name;    - (OUTPUT) The host name buffer.
 * int host_name_size; - (INPUT)  The size of the host name buffer
                                   (NTCSS_SIZE_HOSTNAME).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetLocalIP(char*, int);
/*
 * char *ip_address_buffer;    - (OUTPUT) The ip address buffer.
 * int ip_address_buffer_size; - (INPUT)  The size of the ip address buffer
                                           (IP_ADDRESS_SIZE).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetServerName(char*, int);
/*
 * char *server_name;    - (OUTPUT) The server name buffer.
 * int server_name_size; - (INPUT)  The size of the server name buffer
                                     (NTCSS_SIZE_HOSTNAME).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetUserInfo(NtcssUserInfo*);
/*
 * NtcssUserInfo *user_info_ptr; - (OUTPUT) The user information record.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
     message).
 */

extern BOOL NtcssPrintServerFile(char*, int, char*, char*, int, char*);
/*
 * char *server_name;    - (INPUT) The name of the server to handle the print
                                    job.
 * int security_class;   - (INPUT) The security class of the print job .
 * char *output_type;    - (INPUT) The output type of the print job.
 * char *print_filename; - (INPUT) The filename to print.
 * int number_of_copies; - (INPUT) The number of copies to print.
 * char *options;        - (INPUT) The -o options string for lp .
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssSetAppData(const char*);
/*
 * const char *app_data; - (INPUT) The application data buffer.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssStartServerProcess(NtcssServerProcessDescr*, char*, int);
/*
 * NtcssServerProcessDescr *process_description; - (INPUT)  The server process
                                                             record.
 * char *pid;                                    - (OUTPUT) The process id
                                                             buffer.
 * int pid_size;                                 - (INPUT)  Size of process id
                                                             buffer
                                                        (NTCSS_SIZE_PROCESS_ID)
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssStartServerProcessEx(char*, NtcssServerProcessDescr*, char*,
                                      int);
/*
 * char *hostname;  The host where this process will execute.
 * NtcssServerProcessDescr *process_description; - (INPUT)  The server process
                                                             record.
 * char *pid;                                    - (OUTPUT) The process id
                                                             buffer.
 * int pid_size;                                 - (INPUT)  Size of process id
                                                             buffer
                                                       (NTCSS_SIZE_PROCESS_ID).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssStopServerProcess(char*, int);
/* 
 * char *pid;        - (INPUT) The process id .
 * int signal_type;  - (INPUT) The signal type.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssRemoveServerProcess(char*, int);
/*
 * char *pid;                    - (INPUT) The process id.
 * int   scheduled_process_flag  - ()
 *
 * Returns True when successful
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetAppName(char*, int);
/* 
 * char *application_name;    - (OUTPUT) The application name buffer.
 * int application_name_size; - (INPUT) The size of the application name
                                          buffer.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetLinkData(char*, int);
/* 
 * char *link_data;    - (OUTPUT) The link data buffer.
 * int link_data_size; - (INPUT)  The size of the link data buffer.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssSetLinkData(char*);
/* 
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssPostAppMessage(char*, char*);
/* 
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssPostMessage(const char*, char*, char*);
/* 
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssPostSystemMessage(char*, char*);
/* 
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetAppHost(char*, char*, int);
/*
 * char *app_name_str;   - (INPUT) The name of the application.
 * char *host_buf;       - (OUTPUT) The name of the application's host.
 * int host_buf_size;    - (INPUT) The size of the host name buffer
                                        (NTCSS_SIZE_HOSTNAME).
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetAuthServer(char*);
/*
 * char *pas;   - (OUTPUT) The name of the authentication server.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssGetAppDataServer(char*, char*, int);
/*
 * char *app_name;      - (INPUT) The name of the application.
 * char *host_name;     - (OUTPUT) The name of the applicaiton's server.
 * int  host_name_size; - (INPUT) The size of the host name buffer.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */

extern BOOL NtcssValidateUser(char*, char*, char *,int);
/*
 * char *app_name;      - (INPUT) The name of the application.
 * char *host_name;     - (OUTPUT) The name of the applicaiton's server.
 * int  host_name_size; - (INPUT) The size of the host name buffer.
 *
 * Returns True when successful.
 * Returns False when there was an error (use NtcssGetLastError() to get error
    message).
 */



#endif /* _NtcssApi_h */

