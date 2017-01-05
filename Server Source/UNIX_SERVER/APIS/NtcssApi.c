/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  NtcssApi.c
*/

/******************************************************************************
  System Headers
******************************************************************************/
#include <stdio.h>
#include <pwd.h>
#include <netdb.h>
#include <syslog.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/******************************************************************************
  Local Headers
******************************************************************************/
#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <NetCopy.h>
#include <SockFcts.h>
#include <StringFcts.h>
#include <ini_Fcts.h>
#include <ExtraProtos.h>
#include <inri_version.h>

#include "NtcssApiP.h" 
#include "NtcssApi.h"
#include "NtcssError.h"
#include "NtcssMacros.h"
#include "NtcssServerSocket.h"
#include "NtcssMessage.h"

/******************************************************************************
  Private Function Prototypes
******************************************************************************/
static void  check_buffer_size(int, int);
static void  check_priority_code(int);
static void  check_security_level(int);
static void  fill_login_name(char*, char*);
static void  get_login_name(char*);
static const char *get_api_function_name(void);
static int   getAppHostName(char*, char*, char*);
static void  get_command_line(char*, NtcssServerProcessDescr*);
static void  get_common_user_data(const char*);
static void  get_default_printer_info(char*, char*, char*, char*);
static void  get_filter_mask_buffer(NtcssProcessFilter*, char*);
static int   GetFilteredServerProcessList(NtcssProcessFilter*, 
                                          NtcssServerProcessDetails*, int, int,
                                          int, int);
static void  get_ntcss_info(char*, char*);
/* static void  get_ntcss_user_data(); */
static void  get_printer_flag_string(char*, NtcssServerProcessDescr*);
static void  get_start_process_flag_string(char*, NtcssServerProcessDescr*);
static void  get_users_access_role(char*, char*, char*, int*);
static void  reset_api(int);
static void  set_api_code(int);
static void  split_real_name(char*, char*, char*, char*);
static int   StartServerBootProcess(NtcssServerProcessDescr*, char*, int, 
                                    char*, char*, int);
static void  get_application_hostname(char*, char*, int);
static void  get_authentication_server(char*);
static void  get_application_data_server(char*, char*, int);
static void  get_batch_user(char*, char*, char*, char*, char*);

/******************************************************************************
  Private Data
******************************************************************************/
static int  api_code;
static common_user_data_str  common_user_data;
/* static int  interactive_test_process_id = 0; */

/******************************************************************************
  Public Function Prototypes
******************************************************************************/
BOOL NtcssGetAppData(char*, int);
BOOL NtcssGetAppName(char*, int);
BOOL  NtcssGetAppPassword(char*, int);
BOOL  NtcssGetAppUserInfo(char*, NtcssUserInfo*, char*, int);
BOOL  NtcssGetCommonData(char*, char*, int);
BOOL  NtcssGetDateTime(char*, int);
int   NtcssGetFilteredServerProcessList(NtcssProcessFilter*, 
                                        NtcssServerProcessDetails*, int);
int   NtcssGetFilteredServerProcessListCt(NtcssProcessFilter*);
BOOL  NtcssGetHostName(char*, int);
BOOL  NtcssGetLastError(char*, int);
BOOL  NtcssGetLinkData(char*, int);
BOOL  NtcssGetLocalIP(char*, int);
BOOL  NtcssGetServerName(char*, int);
BOOL  NtcssGetAppHost(char*, char*, int);
BOOL  NtcssGetAuthServer(char*);
BOOL  NtcssGetAppDataServer(char*, char*, int);
BOOL  NtcssGetUserInfo(NtcssUserInfo*);
BOOL  NtcssPostAppMessage(char*, char*);
BOOL  NtcssPostMessage(const char*, char*, char*);
BOOL  NtcssPostSystemMessage(char*, char*);
BOOL  NtcssPrintServerFile(char*, int, char*, char*, int, char*);
BOOL  NtcssRemoveServerProcess(char*, int);
BOOL  NtcssSetAppData(const char*);
BOOL  NtcssSetLinkData(char*);
BOOL  NtcssStartBootJobs(void);
BOOL  NtcssStartServerProcess(NtcssServerProcessDescr*, char*, int);
BOOL  NtcssStartServerProcessEx(char*, NtcssServerProcessDescr*, char*, int);
BOOL  NtcssStopServerProcess(char*, int);
BOOL  NtcssValidateUser(char*,char *, char *, int);

/******************************************************************************
  Public Function Definitions
******************************************************************************/

BOOL  NtcssGetLastError(error_msg, error_msg_size)

     char  *error_msg;
     int  error_msg_size;
{
  if (error_msg_size < MAX_ERR_MSG_LEN)
    return(False);

  sprintf(error_msg, "%s: %s", get_api_function_name(),
          NtcssError.get_error_message());

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetAppData(app_data_buffer, app_data_buffer_size)

     char  *app_data_buffer;
     int  app_data_buffer_size;
{
  reset_api( API_CODE_NtcssGetAppData );

  check_buffer_size(app_data_buffer_size, MAX_APP_DATA_LEN);
  if (NtcssError.occurred())
    return(False);

  get_common_user_data("");
  if (NtcssError.occurred())
    return(False);

  memcpy(app_data_buffer, common_user_data.app_data, MAX_APP_DATA_LEN);
  return(True);
}

/*****************************************************************************/

BOOL  NtcssGetAppPassword(password_buffer, password_buffer_size)

     char  *password_buffer;
     int  password_buffer_size;
{
  reset_api( API_CODE_NtcssGetAppPassword );

  check_buffer_size(password_buffer_size, MAX_APP_PASSWD_LEN);
  if (NtcssError.occurred())
    return(False);

  get_common_user_data("");
  if (NtcssError.occurred())
    return(False);

  memcpy(password_buffer, common_user_data.app_passwd, MAX_APP_PASSWD_LEN);

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetAppUserInfo(user_name, user_info_ptr, prog_access_buf,
                          prog_access_buf_size)

     char *user_name;
     NtcssUserInfo  *user_info_ptr;
     char *prog_access_buf;
     int   prog_access_buf_size;
{
  reset_api( API_CODE_NtcssGetAppUserInfo );

  check_buffer_size(prog_access_buf_size, NTCSS_SIZE_APP_PROGRAM_INFO);
  if (NtcssError.occurred())
    return(False);

  get_common_user_data(user_name);
  if (NtcssError.occurred())
    return(False);

  memcpy(user_info_ptr->login_name, common_user_data.login_name,
         MAX_LOGIN_NAME_LEN);
  memcpy(user_info_ptr->real_first_name, common_user_data.first_name,
         NTCSS_SIZE_USERFIRSTNAME);
  memcpy(user_info_ptr->real_middle_name, common_user_data.middle_name,
         NTCSS_SIZE_USERMIDDLENAME);
  memcpy(user_info_ptr->real_last_name, common_user_data.last_name,
         NTCSS_SIZE_USERLASTNAME);
  memcpy(user_info_ptr->ssn, common_user_data.ssn, NTCSS_SIZE_SSN);
  memcpy(user_info_ptr->phone_number, common_user_data.phone_number,
         MAX_PHONE_NUMBER_LEN);
  memcpy(user_info_ptr->app_passwd, common_user_data.app_passwd,
         MAX_APP_PASSWD_LEN);

  user_info_ptr->security_class = common_user_data.security_class;
  user_info_ptr->unix_id        = common_user_data.unix_id;

  memcpy(prog_access_buf, common_user_data.program_access,
         NTCSS_SIZE_APP_PROGRAM_INFO);

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetCommonData(tag, data_buffer, data_buffer_size)

     char  *tag;
     char  *data_buffer;
     int  data_buffer_size;
{
  char   app_name[MAX_PROGROUP_TITLE_LEN + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char  *strSysConfData;
  char   token[MAX_TOKEN_LEN + 1];

  reset_api( API_CODE_NtcssGetCommonData );

  check_buffer_size(data_buffer_size, MAX_COMDB_DATA_LEN);
  if (NtcssError.occurred())
    return(False);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, app_name);
  if (NtcssError.occurred())
    return(False);

  if (getNtcssSysConfValueFromNis(&strSysConfData, tag) == 0)
    {
      strncpy(data_buffer, strSysConfData, strlen(strSysConfData));
      free(strSysConfData);
      return(True);
    }
  else
    syslog(LOG_WARNING, "NtcssGetCommonData API: Failed to get NIS tag (%s)!",
           tag);    

  NtcssMessage.open("$MASTER$",
                    DATABASE_PORT,
                    GET_COMMON_DATA_ACCEPT_LABEL,
                    GET_COMMON_DATA_REJECT_LABEL,
                    GET_COMMON_DATA_REQUEST_LABEL,
                    MAX_COMDB_DATA_LEN,
                    GET_COMMON_DATA_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name, MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token, MAX_TOKEN_LEN);
  NtcssMessage.write( tag, MAX_COMDB_TAG_LEN);

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "C", data_buffer);
  NtcssMessage.close();
  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetDateTime(date_time_buffer, date_time_buffer_size)

     char  *date_time_buffer;
     int   date_time_buffer_size;
{
  reset_api( API_CODE_NtcssGetDateTime );

  check_buffer_size(date_time_buffer_size, NTCSS_SIZE_DATETIME1);
  if(NtcssError.occurred())
    return(False);

/* This function was modified so that the time returned is from the
   current server.  It was decided for v3.xx to let the installers assume
   responsibility for keeping the non-master Ntcss servers time synced with
   the master.
*/

  NtcssServerSocket.open("$CURRENT$", TIME_PORT,
                         GET_DATE_TIME_TIMEOUT_VALUE);
  if(NtcssError.occurred())
    return(False);

  NtcssServerSocket.read(date_time_buffer, NTCSS_SIZE_DATETIME1);
  NtcssServerSocket.close();
  if(NtcssError.occurred())
    return(False);

  if (NtcssServerSocket.bytes_read < NTCSS_SIZE_DATETIME1)
    date_time_buffer[NtcssServerSocket.bytes_read] = 0;

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetHostName(host_name, host_name_size)

     char  *host_name;
     int  host_name_size;
{
  int  x;

  reset_api(API_CODE_NtcssGetHostName);

  check_buffer_size(host_name_size, MAX_HOST_NAME_LEN);
  if (NtcssError.occurred())
    return(False);

  x = gethostname(host_name, (size_t)(MAX_HOST_NAME_LEN + 1));
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetLocalIP(ip_addr_buffer, ip_addr_buffer_size)

     char  *ip_addr_buffer;
     int  ip_addr_buffer_size;
{
  char  *ip_address;
  char   local_host_name[MAX_HOST_NAME_LEN + 1];
  struct hostent  *host_info;
  struct in_addr tmp;

  reset_api(API_CODE_NtcssGetLocalIP);

  check_buffer_size(ip_addr_buffer_size, MAX_IP_ADDRESS_LEN);
  if (NtcssError.occurred())
    return(False);

  NtcssGetHostName(local_host_name, MAX_HOST_NAME_LEN); 
  if (NtcssError.occurred())
    return(False);

  host_info = gethostbyname(local_host_name);
  if (host_info == NULL)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_DATA);
      return(False);
    }

  /*ip_address = inet_ntoa(*((struct in_addr *) *(host_info->h_addr_list)));*/
  tmp.s_addr = strtoul(host_info->h_addr, (char**)NULL, 10);
  ip_address = inet_ntoa(tmp);
  if (ip_address == NULL)
    {
      NtcssError.set_error_code(ERROR_CONVERTING_IP_ADDRESS);
      return(False);
    }
  
  memcpy(ip_addr_buffer, ip_address, MAX_IP_ADDRESS_LEN);
  return(True);

}


/*****************************************************************************/

BOOL  NtcssGetServerName(host_name, host_name_size)

     char  *host_name;
     int  host_name_size;
{
  reset_api(API_CODE_NtcssGetServerName);

  check_buffer_size(host_name_size, MAX_HOST_NAME_LEN);
  if (NtcssError.occurred())
    return(False);

  NtcssServerSocket.get_master_server(host_name); 

  return(! NtcssError.occurred());
}


/*****************************************************************************/

static void  get_filter_mask_buffer(filter, filter_mask_buffer)

     NtcssProcessFilter  *filter;
     char  *filter_mask_buffer;
{
  unsigned int  filter_mask;

  filter_mask = (SPQ_BIT_MASK_WITH_CUST_PROC_STATUS | 
                  SPQ_BIT_MASK_WITH_PRT_REQ_INFO | 
                  SPQ_BIT_MASK_WITH_JOB_DESCRIP);

  if (filter->get_only_running_jobs)
    filter_mask |= SPQ_BIT_MASK_ONLY_RUNNING_PROCS;
  if (filter->get_for_current_user_only)
    filter_mask |= SPQ_BIT_MASK_ONLY_CERTAIN_USER;
  if (filter->priority_flag)
    filter_mask |= SPQ_BIT_MASK_PRIORITY;
  if (filter->command_line_flag)
    filter_mask |= SPQ_BIT_MASK_COMMAND_LINE;
  if (filter->cust_proc_status_flag)
    filter_mask |= SPQ_BIT_MASK_CUST_PROC_STATUS;
  if (filter->login_name_flag)
    filter_mask |= SPQ_BIT_MASK_LOGIN_NAME;
  if (filter->unix_owner_flag)
    filter_mask |= SPQ_BIT_MASK_UNIX_OWNER;
  if (filter->progroup_title_flag)
    filter_mask |= SPQ_BIT_MASK_PROGROUP_TITLE;
  if (filter->job_descrip_flag)
    filter_mask |= SPQ_BIT_MASK_JOB_DESCRIP;
  if (filter->orig_host_flag)
    filter_mask |= SPQ_BIT_MASK_ORIG_HOST;

  sprintf(filter_mask_buffer, "%0*d", FILTER_MASK_SIZE, filter_mask);
}


/*****************************************************************************/

static void  get_users_access_role(token, progroup_title, login_name,
                                   access_role)

     char  *token;
     char  *login_name;
     char  *progroup_title;
     int  *access_role;
{
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  int   this_hosts_type;
  int   x;                       /* General purpose */

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return;
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return;
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT,
                      GET_USERS_ACCESS_ROLE_ACCEPT_LABEL,
                      GET_USERS_ACCESS_ROLE_REJECT_LABEL,
                      GET_USERS_ACCESS_ROLE_REQUEST_LABEL,
                      GET_USERS_ACCESS_ROLE_RESPONSE_SIZE,
                      GET_USERS_ACCESS_ROLE_TIMEOUT_VALUE);
  else
     NtcssMessage.open(this_hosts_name,
                       DATABASE_PORT,
                       GET_USERS_ACCESS_ROLE_ACCEPT_LABEL,
                       GET_USERS_ACCESS_ROLE_REJECT_LABEL,
                       GET_USERS_ACCESS_ROLE_REQUEST_LABEL,
                       GET_USERS_ACCESS_ROLE_RESPONSE_SIZE,
                       GET_USERS_ACCESS_ROLE_TIMEOUT_VALUE);

  if  (NtcssError.occurred())
    return;

  NtcssMessage.write( login_name,     MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( token,          MAX_TOKEN_LEN          );
  NtcssMessage.write( progroup_title, MAX_PROGROUP_TITLE_LEN );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return;
    }

  *access_role = atoi(NtcssMessage.response + 1);
  
  NtcssMessage.close();

}


/*****************************************************************************/

static int  GetFilteredServerProcessList(process_filter,
                                         process_list,
                                         process_list_size,
                                         get_process_list_flag,
                                         get_scheduled_jobs,
                                         starting_process_index)

     NtcssProcessFilter         *process_filter;
     NtcssServerProcessDetails  *process_list;
     int   process_list_size;
     int   get_process_list_flag;
     int   get_scheduled_jobs;
     int   starting_process_index;

/* 
   GetFilteredServerProcessList handles the functionality for both
   NtcssGetFilteredServerProcessList and NtcssGetFilteredServerProcessListCt.
   GetFilteredServerProcessList is designed to be called twice - once
   with get_scheduled_jobs true, and once with get_scheduled_jobs false.
   This makes it possible for the api user to get both scheduled and
   unscheduled jobs in one call because the server only supports sending 
   either scheduled or unscheduled jobs.

   in  - process_filter - Describes fields and data in which to filter
                           the processes.
   out - process_list   - List of process details returned to caller.
   in  - get_process_list_flag - When get_process_list_flag is true,
                                  GetFilteredServerProcessList fills
                                  process_list and returns the process count.
                                 When get_process_list_flag is false, 
                                 GetFilteredProcessList only returns the
                                 process count.
   in  - get_scheduled_jobs -  This flag indicates whether to go after
                                scheduled jobs or unscheduled jobs.
   in  - starting_process_index - Indicates the starting index in which
                                   to start filling in process details.

*/

{
  char   access_role_buffer[ACCESS_ROLE_BUFFER_SIZE + 1];
  char   filter_mask_buffer[FILTER_MASK_SIZE + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   priority_buffer[PRIORITY_BUFFER_SIZE + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char  *response;
  char  *response_ptr;
  char   schedule_flag_buffer[FLAG_BUFFER_SIZE + 1];
  char   token[MAX_TOKEN_LEN + 1];

  int   access_role;
  int   i;
  int   print_code_mask;
  int   process_count;

  if (get_scheduled_jobs)
    {
      if (!process_filter->get_scheduled_jobs)
        return(0);
      else
        strcpy(schedule_flag_buffer,"1");
    }
  else
    {
      if (!process_filter->get_unscheduled_jobs)
        return(0);
      else
        strcpy(schedule_flag_buffer,"0");
    }

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(-1);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(-1);

  get_users_access_role(token, progroup_title, login_name, &access_role);
  if (NtcssError.occurred())
    return(-1);

  get_filter_mask_buffer(process_filter, filter_mask_buffer);

  sprintf(access_role_buffer, "%0*d", ACCESS_ROLE_BUFFER_SIZE, access_role);
  sprintf(priority_buffer,    "%0*d", PRIORITY_BUFFER_SIZE,
          process_filter->priority);


  NtcssMessage.open("$CURRENT$",
                    SPQ_SVR_PORT,
                    GET_FILTERED_SERVER_PROCESS_LIST_ACCEPT_LABEL,
                    GET_FILTERED_SERVER_PROCESS_LIST_REJECT_LABEL,
                    GET_FILTERED_SERVER_PROCESS_LIST_REQUEST_LABEL,
                    GET_FILTERED_SERVER_PROCESS_LIST_RESPONSE_SIZE,
                    GET_FILTERED_SERVER_PROCESS_LIST_TIMEOUT_VALUE);
  if (NtcssError.occurred())
    return(-1);

  NtcssMessage.write( login_name,                     MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token,                          MAX_TOKEN_LEN);
  NtcssMessage.write( progroup_title,                 MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.write( access_role_buffer,             ACCESS_ROLE_BUFFER_SIZE);
  NtcssMessage.write( schedule_flag_buffer,           FLAG_BUFFER_SIZE);
  NtcssMessage.write( filter_mask_buffer,             FILTER_MASK_SIZE);
  NtcssMessage.write( priority_buffer,                PRIORITY_BUFFER_SIZE);
  NtcssMessage.write( process_filter->command_line,   MAX_PROC_CMD_LINE_LEN);
  NtcssMessage.write( process_filter->cust_proc_status,
                      MAX_CUST_PROC_STAT_LEN );
  NtcssMessage.write( process_filter->login_name,     MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( process_filter->unix_owner,     MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( process_filter->progroup_title, MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.write( process_filter->job_descrip,    MAX_JOB_DESCRIP_LEN);
  NtcssMessage.write( process_filter->orig_host,      MAX_ORIG_HOST_LEN);

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(-1);
    }

   if (!(NtcssMessage.response))
    {
      NtcssError.set_error_code(ERROR_READING_SERVER);
      NtcssMessage.close();
      return(-1);
    }

  if (!(*NtcssMessage.response))
    {
      NtcssError.set_error_code(ERROR_READING_SERVER);
      NtcssMessage.close();
      return(-1);
    }

  unbundleData(NtcssMessage.response, "I", &process_count);

  if (!get_process_list_flag)
    {
      NtcssMessage.close();
      return process_count;
    }

  response_ptr = malloc(strlen(NtcssMessage.response));
  response = response_ptr;
  if (!(response_ptr))
    {
      NtcssError.set_error_code(ERROR_ALLOCATING_MEMORY);
      return(-1);
    }

  memcpy(response, NtcssMessage.response, strlen(NtcssMessage.response));
  NtcssMessage.close();
  response_ptr += getOverallLength("I", process_count);

  if ( (starting_process_index + process_count) > process_list_size)
    {
      NtcssError.set_error_code(ERROR_PROCESS_LIST_SIZE_TOO_SMALL);
      free(response); 
      return(-1);
    }


  for (i = starting_process_index;
       i < (starting_process_index + process_count); i++)
    {
      unbundleData(response_ptr,
                   "ICICCICCCCCCCICCCICICCCCIIII",
                   &process_list[i].pid,
                   process_list[i].process_id_str,
                   &process_list[i].priority_code,
                   process_list[i].app_name_str,
                   process_list[i].command_line_str,
                   &process_list[i].status_code,
                   process_list[i].custom_process_stat_str,
                   process_list[i].login_name_str,
                   process_list[i].unix_owner_str,
                   process_list[i].job_descrip_str,
                   process_list[i].request_time,
                   process_list[i].launch_time,
                   process_list[i].end_time,
                   &process_list[i].seclvl,
                   process_list[i].orig_host_str,
                   process_list[i].printer_name_str,
                   process_list[i].printer_host_name_str,
                   &process_list[i].print_seclvl,
                   process_list[i].prt_filename_str,
                   &print_code_mask,
                   process_list[i].sdi_control_record_id_str,
                   process_list[i].sdi_data_file_str,
                   process_list[i].sdi_tci_file_str,
                   process_list[i].sdi_device_data_file_str,
                   &process_list[i].orientation,
                   &process_list[i].banner,
                   &process_list[i].copies,
                   &process_list[i].papersize);

      response_ptr += getOverallLength("ICICCICCCCCCCICCCICICCCCIIII",
                                       process_list[i].pid,
                                       process_list[i].process_id_str,
                                       process_list[i].priority_code,
                                       process_list[i].app_name_str,
                                       process_list[i].command_line_str,
                                       process_list[i].status_code,
                                       process_list[i].custom_process_stat_str,
                                       process_list[i].login_name_str,
                                       process_list[i].unix_owner_str,
                                       process_list[i].job_descrip_str,
                                       process_list[i].request_time,
                                       process_list[i].launch_time,
                                       process_list[i].end_time,
                                       process_list[i].seclvl,
                                       process_list[i].orig_host_str,
                                       process_list[i].printer_name_str,
                                       process_list[i].printer_host_name_str,
                                       process_list[i].print_seclvl,
                                       process_list[i].prt_filename_str,
                                       print_code_mask,
                                     process_list[i].sdi_control_record_id_str,
                                       process_list[i].sdi_data_file_str,
                                       process_list[i].sdi_tci_file_str,
                                      process_list[i].sdi_device_data_file_str,
                                       process_list[i].orientation,
                                       process_list[i].banner,
                                       process_list[i].copies,
                                       process_list[i].papersize);

      if (print_code_mask & PRINT_OUTPUT_FLAG)
        {
          if (print_code_mask & DEL_AFTER_PRINT_FLAG) 
            process_list[i].print_code = NTCSS_PRINT_AND_REMOVE;
          else
            process_list[i].print_code = NTCSS_PRINT_AND_SAVE;
        }
      else
        process_list[i].print_code = NTCSS_PRINT_NOT;
      
      process_list[i].restartable_flag = ((print_code_mask & RESTART_FLAG)
                                          == RESTART_FLAG);
    }

  free(response);

  return process_count;
}


/*****************************************************************************/

int NtcssGetFilteredServerProcessList(process_filter,
                                      process_list,
                                      process_list_size)

     NtcssProcessFilter  *process_filter;
     NtcssServerProcessDetails  *process_list;
     int  process_list_size;
{
  int  process_count = 0;

  reset_api(API_CODE_NtcssFilteredServerProcessList);

  process_count = GetFilteredServerProcessList(process_filter, 
                                               process_list, 
                                               process_list_size, 
                                               TRUE,  /* get process list */
                                               FALSE,
                                               /* get unscheduled jobs
                                                  (if applicable) */ 
                                               process_count);
  if (NtcssError.occurred())
    return(-1);

  process_count += GetFilteredServerProcessList(process_filter, 
                                                process_list, 
                                                process_list_size, 
                                                TRUE, /* get process list */
                                                TRUE,
                                                /* get scheduled jobs
                                                   (if applicable) */
                                                process_count);
  if (NtcssError.occurred())
    return(-1);

  return process_count;
}


/*****************************************************************************/

int  NtcssGetFilteredServerProcessListCt(process_filter)

     NtcssProcessFilter  *process_filter;
{
  NtcssServerProcessDetails  *process_list;
  int  process_count = 0;

  reset_api(API_CODE_NtcssFilteredServerProcessList);

  process_count = GetFilteredServerProcessList(process_filter, 
                                               process_list, 
                                               0,     /* process_list_size */
                                               FALSE, /* get process count only */
                                               FALSE,
                                               /* count unscheduled jobs
                                                  (if applicable) */ 
                                               process_count);
  if (NtcssError.occurred())
    return(-1);

  process_count += GetFilteredServerProcessList(process_filter, 
                                                process_list, 
                                                0,    /* process_list_size */
                                                FALSE,
                                                /* get process count only */
                                                TRUE,
                                                /* count scheduled jobs
                                                   (if applicable) */
                                                process_count);
  if (NtcssError.occurred())
    return(-1);

  return process_count;
}


/*****************************************************************************/

BOOL  NtcssGetUserInfo(user_info_ptr)

     NtcssUserInfo  *user_info_ptr;
{
  reset_api( API_CODE_NtcssGetUserInfo );

  get_common_user_data("");
  if (NtcssError.occurred())
    return(False);

  memcpy(user_info_ptr->login_name, common_user_data.login_name,
         MAX_LOGIN_NAME_LEN);
  memcpy(user_info_ptr->real_first_name, common_user_data.first_name,
         NTCSS_SIZE_USERFIRSTNAME);
  memcpy(user_info_ptr->real_middle_name, common_user_data.middle_name,
         NTCSS_SIZE_USERMIDDLENAME);
  memcpy(user_info_ptr->real_last_name, common_user_data.last_name,
         NTCSS_SIZE_USERLASTNAME);
  memcpy(user_info_ptr->ssn, common_user_data.ssn, NTCSS_SIZE_SSN);
  memcpy(user_info_ptr->phone_number, common_user_data.phone_number,
         MAX_PHONE_NUMBER_LEN);

  user_info_ptr->security_class = common_user_data.security_class;
  user_info_ptr->unix_id        = common_user_data.unix_id;

  return(True);
}


/*****************************************************************************/

BOOL  NtcssSetAppData(app_data)

     const char  *app_data;
{
  int   this_hosts_type;
  int   x;                      /* General purpose */
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssSetAppData);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return(False);
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT, 
                      SET_APP_DATA_ACCEPT_LABEL,
                      SET_APP_DATA_REJECT_LABEL,
                      SET_APP_DATA_REQUEST_LABEL,
                      SET_APP_DATA_RESPONSE_SIZE,
                      SET_APP_DATA_TIMEOUT_VALUE);
  else
    NtcssMessage.open(this_hosts_name,
                      DATABASE_PORT, 
                      SET_APP_DATA_ACCEPT_LABEL,
                      SET_APP_DATA_REJECT_LABEL,
                      SET_APP_DATA_REQUEST_LABEL,
                      SET_APP_DATA_RESPONSE_SIZE,
                      SET_APP_DATA_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,      MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,           MAX_TOKEN_LEN );
  NtcssMessage.write( progroup_title,  MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.write( app_data,        MAX_APP_DATA_LEN );

  NtcssMessage.read();
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/
/* ***NOTE*** If changed, see StartServerBootProcess for possible changes
   that need to be made.
*/

BOOL  NtcssStartServerProcess(process_description, pid, pid_size)

     NtcssServerProcessDescr  *process_description;
     char  *pid;
     int  pid_size;

{
  char   banner[PRINTER_BANNER_SIZE + 1];
  char   command_line[MAX_PROC_CMD_LINE_LEN + 1];
  char   copies[PRINTER_COPIES_SIZE + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   orientation[PRINTER_ORIENTATION_SIZE + 1];
  char   papersize[PRINTER_PAPERSIZE_SIZE + 1];
  char   printer_flag_string[PRINTER_FLAG_STRING_SIZE + 1];
  char   printer_name[MAX_PRT_NAME_LEN + 1];
  char   printer_host_name[MAX_HOST_NAME_LEN + 1];
  char   priority_code[PRIORITY_CODE_SIZE + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   sdi_control_record[MAX_SDI_CTL_RECORD_LEN + 2];
  char   sdi_data_file[MAX_SERVER_LOCATION_LEN + 1];
  char   sdi_input_flag[SDI_INPUT_FLAG_SIZE + 1];
  char   sdi_required_flag[SDI_REQUIRED_FLAG_SIZE + 1];
  char   sdi_tci_file[MAX_SERVER_LOCATION_LEN + 1];
  char   security_class[SECURITY_CLASS_SIZE + 1];
  char   start_process_flag_string[START_PROCESS_FLAG_SIZE + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssStartServerProcess);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  check_buffer_size(pid_size, MAX_PROCESS_ID_LEN);
  if (NtcssError.occurred())
    return(False);

  check_security_level(process_description->seclvl); 
  if (NtcssError.occurred())
    return(False);

  check_priority_code(process_description->priority_code); 
  if (NtcssError.occurred())
    return(False);

  fill_login_name(login_name, process_description->owner_str);
  if (NtcssError.occurred())
    return(False);

  if (process_description->prt_output_flag)
    {
      get_default_printer_info(process_description->output_type_str,
                               progroup_title,
                               printer_name,
                               printer_host_name);
      if (NtcssError.occurred())
        return(False);
    }
  else
    {
      strcpy(printer_name,"");
      strcpy(printer_host_name,"");
    }

  get_start_process_flag_string(start_process_flag_string,
                                process_description);
  get_command_line(command_line, process_description);
  get_printer_flag_string(printer_flag_string, process_description);
 
  /* this is also used for the printer security level*/
  sprintf(security_class, "%0*d", SECURITY_CLASS_SIZE,
          process_description->seclvl);
  sprintf(priority_code, "%0*d", PRIORITY_CODE_SIZE,
          process_description->priority_code);
  sprintf(sdi_required_flag,"%d", process_description->sdi_required_flag == 1);
  sprintf(sdi_input_flag, "%d", process_description->sdi_input_flag == 1);
  memcpy(sdi_control_record, process_description->sdi_control_id_str,
         MAX_SDI_CTL_RECORD_LEN + 1);
  memcpy(sdi_data_file, process_description->sdi_data_file_str,
         MAX_SERVER_LOCATION_LEN);
  memcpy(sdi_tci_file, process_description->sdi_tci_file_str,
         MAX_SERVER_LOCATION_LEN);

  sprintf(copies, "%0*d", PRINTER_COPIES_SIZE, process_description->copies);
  sprintf(banner, "%*d", PRINTER_BANNER_SIZE, process_description->banner);
  sprintf(orientation, "%0*d", PRINTER_ORIENTATION_SIZE,
          process_description->orientation);
  sprintf(papersize, "%0*d", PRINTER_PAPERSIZE_SIZE,
          process_description->papersize);

  NtcssMessage.open("$CURRENT$",
                    COMMAND_PORT, 
                    START_SERVER_PROCESS_ACCEPT_LABEL,
                    START_SERVER_PROCESS_REJECT_LABEL,
                    START_SERVER_PROCESS_REQUEST_LABEL,
                    START_SERVER_PROCESS_RESPONSE_SIZE,
                    START_SERVER_PROCESS_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,                MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token,                     MAX_TOKEN_LEN);
  NtcssMessage.write( progroup_title,            MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.write( start_process_flag_string, START_PROCESS_FLAG_SIZE);
  NtcssMessage.write( priority_code,             PRIORITY_CODE_SIZE);
  NtcssMessage.write( command_line,              MAX_PROC_CMD_LINE_LEN);
  NtcssMessage.write( login_name,                MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( process_description->job_descrip_str,
                      MAX_JOB_DESCRIP_LEN);
  NtcssMessage.write( security_class,            SECURITY_CLASS_SIZE);
  NtcssMessage.write( printer_name,              MAX_PRT_NAME_LEN);
  NtcssMessage.write( printer_host_name,         MAX_HOST_NAME_LEN);
  NtcssMessage.write( security_class,            SECURITY_CLASS_SIZE);
  NtcssMessage.write( process_description->prt_filename_str,
                      PRINT_FILE_NAME_SIZE);
  NtcssMessage.write( printer_flag_string,       PRINTER_FLAG_STRING_SIZE);
  NtcssMessage.write( sdi_required_flag,         SDI_REQUIRED_FLAG_SIZE);
  NtcssMessage.write( sdi_input_flag,            SDI_INPUT_FLAG_SIZE);
  NtcssMessage.write( sdi_control_record,        MAX_SDI_CTL_RECORD_LEN + 1);
  NtcssMessage.write( sdi_data_file,             MAX_SERVER_LOCATION_LEN);
  NtcssMessage.write( sdi_tci_file,              MAX_SERVER_LOCATION_LEN);
  NtcssMessage.write( orientation,               PRINTER_ORIENTATION_SIZE);
  NtcssMessage.write( banner,                    PRINTER_BANNER_SIZE);
  NtcssMessage.write( copies,                    PRINTER_COPIES_SIZE);
  NtcssMessage.write( papersize,                 PRINTER_PAPERSIZE_SIZE);

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "C", pid);

  NtcssMessage.close();

  return(True);
}


/*****************************************************************************/
/* If changed, see StartServerBootProcess for possible changes that
   need to be made.
*/
BOOL  NtcssStartServerProcessEx(appname, process_description, pid, pid_size)

     char  *appname;
     NtcssServerProcessDescr  *process_description;
     char  *pid;
     int  pid_size;
{
  char   banner[PRINTER_BANNER_SIZE + 1];
  char   command_line[MAX_PROC_CMD_LINE_LEN + 1];
  char   copies[PRINTER_COPIES_SIZE + 1];
  char   hostname[MAX_HOST_NAME_LEN + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   orientation[PRINTER_ORIENTATION_SIZE + 1];
  char   papersize[PRINTER_PAPERSIZE_SIZE + 1];
  char   printer_flag_string[PRINTER_FLAG_STRING_SIZE + 1];
  char   printer_name[MAX_PRT_NAME_LEN + 1];
  char   printer_host_name[MAX_HOST_NAME_LEN + 1];
  char   priority_code[PRIORITY_CODE_SIZE + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   sdi_control_record[MAX_SDI_CTL_RECORD_LEN + 1 + 1];
  char   sdi_data_file[MAX_SERVER_LOCATION_LEN + 1];
  char   sdi_input_flag[SDI_INPUT_FLAG_SIZE + 1];
  char   sdi_required_flag[SDI_REQUIRED_FLAG_SIZE + 1];
  char   sdi_tci_file[MAX_SERVER_LOCATION_LEN + 1];
  char   security_class[SECURITY_CLASS_SIZE + 1];
  char   start_process_flag_string[START_PROCESS_FLAG_SIZE + 1];
  char   token[MAX_TOKEN_LEN + 1];


  reset_api(API_CODE_NtcssStartServerProcess);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  check_buffer_size(pid_size, MAX_PROCESS_ID_LEN);
  if (NtcssError.occurred())
    return(False);

  check_security_level(process_description->seclvl); 
  if (NtcssError.occurred())
    return(False);

  check_priority_code(process_description->priority_code); 
  if (NtcssError.occurred())
    return(False);

  fill_login_name(login_name, process_description->owner_str);
  if (NtcssError.occurred())
    return(False);

  getAppHostName(login_name, appname, hostname);
  if (NtcssError.occurred())
    return(False);

  if (process_description->prt_output_flag)
    {
      get_default_printer_info(process_description->output_type_str,
                               appname,
                               printer_name,
                               printer_host_name);
      if (NtcssError.occurred())
        return(False);
    }
  else
    {
      strcpy(printer_name,"");
      strcpy(printer_host_name,"");
    }

  get_start_process_flag_string(start_process_flag_string,
                                process_description);
  get_command_line(command_line, process_description);
  get_printer_flag_string(printer_flag_string, process_description);
 
  /* this is also used for the printer security level*/
  sprintf(security_class, "%0*d", SECURITY_CLASS_SIZE,
          process_description->seclvl);
  sprintf(priority_code,"%0*d",PRIORITY_CODE_SIZE,
          process_description->priority_code);
  sprintf(sdi_required_flag,"%d", process_description->sdi_required_flag == 1);
  sprintf(sdi_input_flag,"%d", process_description->sdi_input_flag == 1);
  memcpy(sdi_control_record, process_description->sdi_control_id_str,
         MAX_SDI_CTL_RECORD_LEN + 1);
  memcpy(sdi_data_file, process_description->sdi_data_file_str,
         MAX_SERVER_LOCATION_LEN);
  memcpy(sdi_tci_file, process_description->sdi_tci_file_str,
         MAX_SERVER_LOCATION_LEN);

  sprintf(copies, "%0*d", PRINTER_COPIES_SIZE, process_description->copies);
  sprintf(banner, "%*d", PRINTER_BANNER_SIZE, process_description->banner);
  sprintf(orientation, "%0*d", PRINTER_ORIENTATION_SIZE,
          process_description->orientation);
  sprintf(papersize, "%0*d", PRINTER_PAPERSIZE_SIZE,
          process_description->papersize);

  NtcssMessage.open(hostname,
                    COMMAND_PORT, 
                    START_SERVER_PROCESS_ACCEPT_LABEL,
                    START_SERVER_PROCESS_REJECT_LABEL,
                    START_SERVER_PROCESS_REQUEST_LABEL,
                    START_SERVER_PROCESS_RESPONSE_SIZE,
                    START_SERVER_PROCESS_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,                   MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token,                        MAX_TOKEN_LEN);
  NtcssMessage.write( appname,                      MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.write( start_process_flag_string,    START_PROCESS_FLAG_SIZE);
  NtcssMessage.write( priority_code,                PRIORITY_CODE_SIZE);
  NtcssMessage.write( command_line,                 MAX_PROC_CMD_LINE_LEN);
  NtcssMessage.write( login_name,                   MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( process_description->job_descrip_str,
                      MAX_JOB_DESCRIP_LEN);
  NtcssMessage.write( security_class,               SECURITY_CLASS_SIZE);
  NtcssMessage.write( printer_name,                 MAX_PRT_NAME_LEN);
  NtcssMessage.write( printer_host_name,            MAX_HOST_NAME_LEN);
  NtcssMessage.write( security_class,               SECURITY_CLASS_SIZE);
  NtcssMessage.write( process_description->prt_filename_str,
                      PRINT_FILE_NAME_SIZE);
  NtcssMessage.write( printer_flag_string,          PRINTER_FLAG_STRING_SIZE);
  NtcssMessage.write( sdi_required_flag,            SDI_REQUIRED_FLAG_SIZE);
  NtcssMessage.write( sdi_input_flag,               SDI_INPUT_FLAG_SIZE);
  NtcssMessage.write( sdi_control_record,           MAX_SDI_CTL_RECORD_LEN + 1);
  NtcssMessage.write( sdi_data_file,                MAX_SERVER_LOCATION_LEN);
  NtcssMessage.write( sdi_tci_file,                 MAX_SERVER_LOCATION_LEN);
  NtcssMessage.write( orientation,                  PRINTER_ORIENTATION_SIZE);
  NtcssMessage.write( banner,                       PRINTER_BANNER_SIZE);
  NtcssMessage.write( copies,                       PRINTER_COPIES_SIZE);
  NtcssMessage.write( papersize,                    PRINTER_PAPERSIZE_SIZE);

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "C", pid);

  NtcssMessage.close();

  return(True);
}


/*****************************************************************************/

BOOL  NtcssStopServerProcess(pid, signal_type)

     char  *pid;
     int  signal_type;
{
  char  login_name[MAX_LOGIN_NAME_LEN + 1];
  char  progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char  signal_type_string[SIGNAL_TYPE_SIZE + 1];
  char  token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssStopServerProcess);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  sprintf(signal_type_string, "%0*d", SIGNAL_TYPE_SIZE, signal_type);

  NtcssMessage.open("$CURRENT$",
                    COMMAND_PORT,
                    STOP_SERVER_PROCESS_ACCEPT_LABEL,
                    STOP_SERVER_PROCESS_REJECT_LABEL,
                    STOP_SERVER_PROCESS_REQUEST_LABEL,
                    STOP_SERVER_PROCESS_RESPONSE_SIZE,
                    STOP_SERVER_PROCESS_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,         MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( token,              MAX_TOKEN_LEN          );
  NtcssMessage.write( pid,                MAX_PROCESS_ID_LEN            );
  NtcssMessage.write( signal_type_string, SIGNAL_TYPE_SIZE    );
  NtcssMessage.write( progroup_title,     MAX_PROGROUP_TITLE_LEN );

  NtcssMessage.read();
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssPrintServerFile(server_name, security_class, output_type,
                           print_filename, number_of_copies, options)

     char  *server_name; /* not used */
     int    security_class;
     char  *output_type;
     char  *print_filename;
     int    number_of_copies;
     char  *options;
{
  char  login_name[MAX_LOGIN_NAME_LEN + 1];
  char  number_of_copies_string[NUMBER_OF_COPIES_SIZE + 1];
  char  printer_host_name[MAX_HOST_NAME_LEN + 1];
  char  printer_name[MAX_PRT_NAME_LEN + 1];
  char  progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char  security_class_string[SECURITY_CLASS_SIZE + 1];
  char  token[MAX_TOKEN_LEN + 1];

  /* done only to get rid of compiler warning about unused parameters */
  server_name=server_name;

  reset_api(API_CODE_NtcssPrintServerFile);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  if ( (number_of_copies < 1) || (number_of_copies > 99) )
    {
      NtcssError.set_error_code(ERROR_INVALID_NUMBER_OF_COPIES);
      return(False);
    }

  get_default_printer_info( output_type, progroup_title, printer_name,
                            printer_host_name );
  if (NtcssError.occurred())
    return(False);

  sprintf(security_class_string, "%0*d", SECURITY_CLASS_SIZE, security_class);
  sprintf(number_of_copies_string, "%0*d", NUMBER_OF_COPIES_SIZE,
          number_of_copies);

  NtcssMessage.open("$CURRENT$",
                    COMMAND_PORT,
                    PRINT_SERVER_FILE_ACCEPT_LABEL,
                    PRINT_SERVER_FILE_REJECT_LABEL,
                    PRINT_SERVER_FILE_REQUEST_LABEL,
                    PRINT_SERVER_FILE_RESPONSE_SIZE,
                    PRINT_SERVER_FILE_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,              MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,                   MAX_TOKEN_LEN );
  NtcssMessage.write( print_filename,          PRINT_FILE_NAME_SIZE );
  NtcssMessage.write( printer_name,            MAX_PRT_NAME_LEN );
  NtcssMessage.write( printer_host_name,       MAX_HOST_NAME_LEN );
  NtcssMessage.write( progroup_title,          MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.write( security_class_string,   SECURITY_CLASS_SIZE );
  NtcssMessage.write( number_of_copies_string, NUMBER_OF_COPIES_SIZE );
  NtcssMessage.write( options,                 MAX_PRT_OPTIONS_LEN );

  NtcssMessage.read();
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssStartBootJobs()

{
  char  *response;
  char  *response_ptr;
  NtcssServerProcessDescr  process_description;
  int   number_of_boot_jobs;
  int   this_hosts_type;
  int   type_of_job;
  int   i, x;

  char   command_line[MAX_PROC_CMD_LINE_LEN + 1];
  char   error_msg[MAX_ERR_MSG_LEN + 1];
  char   host_name[MAX_HOST_NAME_LEN + 1];
  char   job_title[MAX_JOB_DESCRIP_LEN + 1];
  char   pdj_progroup[MAX_PROGROUP_TITLE_LEN + 1];
  char   pid[MAX_PROCESS_ID_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   schedule_string[NTCSS_MAX_SCHEDULE_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api( API_CODE_NtcssStartBootJobs );

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  x = gethostname(host_name, MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  this_hosts_type = getHostType(host_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return(False);
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT,
                      GET_BOOT_JOBS_ACCEPT_LABEL,
                      GET_BOOT_JOBS_REJECT_LABEL,
                      GET_BOOT_JOBS_REQUEST_LABEL,
                      GET_BOOT_JOBS_RESPONSE_SIZE,
                      GET_BOOT_JOBS_TIMEOUT_VALUE);
  else
    NtcssMessage.open("$CURRENT$",
                      DATABASE_PORT,
                      GET_BOOT_JOBS_ACCEPT_LABEL,
                      GET_BOOT_JOBS_REJECT_LABEL,
                      GET_BOOT_JOBS_REQUEST_LABEL,
                      GET_BOOT_JOBS_RESPONSE_SIZE,
                      GET_BOOT_JOBS_TIMEOUT_VALUE);
  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( BASE_NTCSS_USER, MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,           MAX_TOKEN_LEN      );
  NtcssMessage.write( host_name,       MAX_HOST_NAME_LEN  );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  if (!NtcssMessage.response)
    {
      NtcssMessage.close();
      return(True);
    }

  if (!*(NtcssMessage.response))
    {
      NtcssMessage.close();
      return(True);
    }

  response_ptr = malloc(strlen(NtcssMessage.response));
  response = response_ptr;
  if (response == NULL)
    {
      NtcssError.set_error_code(ERROR_ALLOCATING_MEMORY);
      return(False);
    }
    
  memcpy(response,NtcssMessage.response,strlen(NtcssMessage.response));
  NtcssMessage.close();

  unbundleData(response, "I", &number_of_boot_jobs);
  response += getOverallLength("I", number_of_boot_jobs);
  for (i = 1; i <= number_of_boot_jobs; i++)
    {
      unbundleData(response, "CCCIC", pdj_progroup, job_title, command_line,
                   &type_of_job, schedule_string);
      memcpy(process_description.prog_name_str, command_line,
             MAX_PROC_CMD_LINE_LEN);
      memcpy(process_description.job_descrip_str, job_title,
             MAX_JOB_DESCRIP_LEN);
      strcpy(process_description.prog_args_str,    "");
      strcpy(process_description.prt_filename_str, "");
      strcpy(process_description.output_type_str,  "");
      process_description.seclvl                  = UNCLASSIFIED_CLEARANCE;
      process_description.priority_code           = LOW_PRIORITY;
      process_description.needs_approval_flag     = 0;
      process_description.needs_device_flag       = 0;
      process_description.restartable_flag        = 1;
      process_description.prt_output_flag         = 0;
      process_description.use_default_prt_flag    = 0;
      process_description.general_access_prt_flag = 0;
      process_description.save_prt_output_flag    = 0;
      memcpy(process_description.owner_str, BASE_NTCSS_USER,
             MAX_LOGIN_NAME_LEN);
      process_description.sdi_required_flag       = 0;
      process_description.sdi_input_flag          = 0;
    
      strcpy(process_description.sdi_control_id_str, "");
      strcpy(process_description.sdi_data_file_str,  "");
      strcpy(process_description.sdi_tci_file_str,   "");
      process_description.banner      = 0;
      process_description.copies      = 0;
      process_description.orientation = 0;
      process_description.papersize   = 0;
      
      if (!StartServerBootProcess(&process_description, pdj_progroup,
                                  type_of_job, schedule_string, pid,
                                  MAX_PROCESS_ID_LEN))
        {
          if (NtcssGetLastError(error_msg, MAX_ERR_MSG_LEN))
            syslog(LOG_WARNING, "Error starting boot job! PROGRAM GROUP: %s "
                   "JOB TITLE: %s, COMMAND LINE: %s ERROR MESSAGE %s",
                   pdj_progroup, job_title, command_line, error_msg);
          else
            syslog(LOG_WARNING, "Error starting boot job! PROGRAM GROUP: %s "
                   "JOB TITLE: %s, COMMAND LINE: %s", pdj_progroup,
                   job_title, command_line);
          NtcssError.set_error_code(NO_ERROR);
        }
      response += getOverallLength("CCCIC", pdj_progroup, job_title,
                                 command_line, type_of_job, schedule_string);
      sleep(5);  /* Gives the forked process time to register itself. */
    }

  free(response_ptr);

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetAppName(application_name, application_name_size)

     char  *application_name;
     int  application_name_size;
{
  char  token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssGetAppName);

  check_buffer_size(application_name_size, MAX_PROGROUP_TITLE_LEN);
  if (NtcssError.occurred())
    return(False);
  
  get_ntcss_info(token, application_name);
  if (NtcssError.occurred())
    return(False);

  return(True);
}


/*****************************************************************************/

BOOL  NtcssGetLinkData(link_data,link_data_size)

     char  *link_data;
     int  link_data_size;
{
  int   this_hosts_type;
  int   x;                       /* General purpose */
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];

  reset_api(API_CODE_NtcssGetLinkData);

  check_buffer_size(link_data_size, MAX_PASSWORD_LEN);
  if (NtcssError.occurred())
    return(False);

  if (strcmp(login_name,"root") == 0 || 
      strcmp(login_name,"sybase") == 0)
    strcpy(token, PRETOKEN);
  else {
    get_ntcss_info(token, progroup_title);
    if (NtcssError.occurred())
       return(False);
  }

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return(False);
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT,
                      GET_LINK_DATA_ACCEPT_LABEL,
                      GET_LINK_DATA_REJECT_LABEL,
                      GET_LINK_DATA_REQUEST_LABEL,
                      GET_LINK_DATA_RESPONSE_SIZE,
                      GET_LINK_DATA_TIMEOUT_VALUE);
  else
    NtcssMessage.open(this_hosts_name,
                      DATABASE_PORT,
                      GET_LINK_DATA_ACCEPT_LABEL,
                      GET_LINK_DATA_REJECT_LABEL,
                      GET_LINK_DATA_REQUEST_LABEL,
                      GET_LINK_DATA_RESPONSE_SIZE,
                      GET_LINK_DATA_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,     MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( token,          MAX_TOKEN_LEN          );
  NtcssMessage.write( progroup_title, MAX_PROGROUP_TITLE_LEN );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "C", link_data);
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssPostAppMessage(message_title, message)

     char  *message_title;
     char  *message;
{
  (void) NtcssPostMessage(USE_APP_NAME_CODE, message_title, message);

  set_api_code( API_CODE_NtcssPostAppMessage );

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssPostMessage(message_board, message_title, message)

     const char  *message_board;
     char  *message_title;
     char  *message;
{
  int   i, x;
  int   message_length;
  FILE  *fp;
  char  *local_file_name;
  char   temp_dir[256];
  char   local_message_board[MAX_PROGROUP_TITLE_LEN + 6];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   message_id[MESSAGE_ID_SIZE + 1];
  char   master_server[MAX_HOST_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   server_file_name[MAX_SERVER_LOCATION_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssPostMessage);

  get_login_name(login_name);            
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, progroup_title); 
  if (NtcssError.occurred())
    return(False);

  if (! (message_board && message_title && message) )
    {
      NtcssError.set_error_code(ERROR_INVALID_POST_MESSAGE_DATA);
      return(False);
    }

  if (! (*message_board && *message_title && *message) )
    {
      NtcssError.set_error_code(ERROR_INVALID_POST_MESSAGE_DATA);
      return(False);
    }

  if (strcmp(message_board, USE_APP_NAME_CODE) == 0)
    {
      strcpy(local_message_board, progroup_title);
      for (i = 0; (size_t)i < strlen(local_message_board); i++)
        local_message_board[i] = (char) toupper((int) local_message_board[i]);
      strcat(local_message_board, ".all");
    }
  else
    strcpy(local_message_board, message_board);

  NtcssMessage.open("$MASTER$",
                    MESSAGE_PORT,
                    GET_NEW_MESSAGE_FILE_ACCEPT_LABEL,
                    GET_NEW_MESSAGE_FILE_REJECT_LABEL,
                    GET_NEW_MESSAGE_FILE_REQUEST_LABEL,
                    GET_NEW_MESSAGE_FILE_RESPONSE_SIZE,
                    GET_NEW_MESSAGE_FILE_TIMEOUT_VALUE); 
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  NtcssMessage.write( login_name, MAX_LOGIN_NAME_LEN ); 
  NtcssMessage.write( token,      MAX_TOKEN_LEN      ); 

  NtcssMessage.read();

  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response,"C",server_file_name);
  NtcssMessage.close();

  strcpy(message_id,
         &server_file_name[strlen(server_file_name) - MSG_ID_START_POS]);
  message_id[MSG_ID_LEN] = ZERO;
  strcat(message_id, login_name);

  sprintf(temp_dir, "%s/%s", NTCSS_HOME_DIR, NTCSS_LOGS_DIR);
  local_file_name = tempnam(temp_dir, "mbb");

  fp = fopen(local_file_name, "w");
  if (fp == NULL)
    {
      NtcssError.set_error_code(ERROR_OPENING_TEMP_FILE);
      return(False);
    }

  message_length = strlen(message);

  x = fwrite(message, sizeof(char), message_length, fp);
  fclose(fp);
  if (x != message_length)
    {
      NtcssError.set_error_code(ERROR_WRITING_TO_TEMP);
      unlink(local_file_name);
      free(local_file_name);
      return(False);
    }

  NtcssServerSocket.get_master_server(master_server); 
  if (NtcssError.occurred())
    {
      unlink(local_file_name);
      free(local_file_name);
      return(False);
    }

  x = net_copy("put", master_server, local_file_name, server_file_name,
               XFER_ASCII);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_DURING_FTP_OF_MESSAGE_FILE);
      unlink(local_file_name);
      free(local_file_name); 
      return(False);
    }

  NtcssMessage.open("$MASTER$",
                    MESSAGE_PORT,
                    CREATE_MESSAGE_FILE_ACCEPT_LABEL,
                    CREATE_MESSAGE_FILE_REJECT_LABEL,
                    CREATE_MESSAGE_FILE_REQUEST_LABEL,
                    CREATE_MESSAGE_FILE_RESPONSE_SIZE,
                    CREATE_MESSAGE_FILE_TIMEOUT_VALUE);

  NtcssMessage.write( login_name,      MAX_LOGIN_NAME_LEN    );
  NtcssMessage.write( token,           MAX_TOKEN_LEN         );
  NtcssMessage.write( message_id,      MESSAGE_ID_SIZE    );
  NtcssMessage.write( message_title,   MESSAGE_TITLE_SIZE );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  NtcssMessage.close();

  NtcssMessage.open("$MASTER$",
                    MESSAGE_PORT,
                    SEND_BB_MESSAGE_ACCEPT_LABEL,
                    SEND_BB_MESSAGE_REJECT_LABEL,
                    SEND_BB_MESSAGE_REQUEST_LABEL,
                    SEND_BB_MESSAGE_RESPONSE_SIZE,
                    SEND_BB_MESSAGE_TIMEOUT_VALUE);
  
  NtcssMessage.write( login_name,           MAX_LOGIN_NAME_LEN       );
  NtcssMessage.write( token,                MAX_TOKEN_LEN            );
  NtcssMessage.write( message_id,           MESSAGE_ID_SIZE       );
  NtcssMessage.write( local_message_board,  MAX_PROGROUP_TITLE_LEN   );

  NtcssMessage.read();

  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssPostSystemMessage(message_title, message)

     char  *message_title;
     char  *message;
{
  (void) NtcssPostMessage("NTCSS.all", message_title, message);
  set_api_code(API_CODE_NtcssPostSystemMessage); 
  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssSetLinkData(link_data)

     char  *link_data;
{
  int   this_hosts_type;
  int   x;                     /* General prupose */
  char  hostname[MAX_HOST_NAME_LEN + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssSetLinkData);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  if (strcmp(login_name,"root") == 0 || 
      strcmp(login_name,"sybase") == 0)
    strcpy(token, PRETOKEN);
  else
    get_ntcss_info(token, progroup_title);

  x = gethostname(hostname, MAX_PROGROUP_TITLE_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  this_hosts_type = getHostType(hostname, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return(False);
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT,
                      SET_LINK_DATA_ACCEPT_LABEL,
                      SET_LINK_DATA_REJECT_LABEL,
                      SET_LINK_DATA_REQUEST_LABEL,
                      SET_LINK_DATA_RESPONSE_SIZE,
                      SET_LINK_DATA_TIMEOUT_VALUE);
  else
    NtcssMessage.open(hostname,
                      DATABASE_PORT,
                      SET_LINK_DATA_ACCEPT_LABEL,
                      SET_LINK_DATA_REJECT_LABEL,
                      SET_LINK_DATA_REQUEST_LABEL,
                      SET_LINK_DATA_RESPONSE_SIZE,
                      SET_LINK_DATA_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name, MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,      MAX_TOKEN_LEN );
  NtcssMessage.write( hostname,   MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.write( link_data,  MAX_PASSWORD_LEN );

  NtcssMessage.read();
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

BOOL  NtcssGetAppHost(app_name_str, host_buf, host_buf_size)

     char *app_name_str;
     char *host_buf;
     int  host_buf_size;
{
  reset_api(API_CODE_NtcssGetAppHost);

  check_buffer_size(host_buf_size, MAX_HOST_NAME_LEN);
  if (NtcssError.occurred())
    return(False);

  get_application_hostname(app_name_str, host_buf, host_buf_size);
  if (NtcssError.occurred())
    return(False);

  return(True);
}

/*****************************************************************************/

BOOL  NtcssGetAuthServer(pas)

     char *pas;
{
  reset_api(API_CODE_NtcssGetAuthServer);

  get_authentication_server(pas);
  if (NtcssError.occurred())
    return(False);

  return(True);
}

/*****************************************************************************/

BOOL  NtcssGetAppDataServer(app_name, host_name, host_name_size)

     char *app_name;
     char *host_name;
     int   host_name_size;
{
  reset_api(API_CODE_NtcssGetAppDataServer);

  check_buffer_size(host_name_size, MAX_HOST_NAME_LEN);
  if (NtcssError.occurred())
    return(False);

  get_application_data_server(app_name, host_name, host_name_size);
  if (NtcssError.occurred())
    return(False);

  return(True);
}


/******************************************************************************
  Private Function Definitions
******************************************************************************/


static void  set_api_code( _api_code )

     int  _api_code;
{
  api_code = _api_code;
}


/*****************************************************************************/

static void  reset_api( _api_code )

     int  _api_code;
{
  set_api_code( _api_code );
  NtcssError.reset();
}


/*****************************************************************************/

static void  check_buffer_size(callers_buffer_size, needed_buffer_size)

     int  callers_buffer_size;
     int  needed_buffer_size;
{
  if (callers_buffer_size < needed_buffer_size)
    NtcssError.set_error_code(ERROR_BUFFER_TOO_SMALL);

  return;
}


/*****************************************************************************/

static void  get_login_name(login_name)
     
     char  *login_name;
{
  struct passwd  *pw;

  pw = getpwuid(getuid());
  if (pw == NULL)
    {
      NtcssError.set_error_code(ERROR_GETTING_LOGIN_NAME);
      return;
    }

  memcpy(login_name, pw->pw_name, MAX_LOGIN_NAME_LEN);
  trim(login_name);
  login_name[MAX_LOGIN_NAME_LEN] = '\0';
}


/*****************************************************************************/

static void  get_ntcss_info(token, progroup_title)

     char  *token;
     char  *progroup_title;
{
  char  *ntcss_pid;
  char  *ntcss_program_group_title;

  ntcss_pid = getenv(PID_ENVIRONMENT_VARIABLE);

  ntcss_program_group_title = getenv(PROGRAM_GROUP_ENVIRONMENT_VARIABLE);

#ifdef DEBUG
  syslog(LOG_WARNING, "get_ntcss_info() ntcss_pid: %s progroup_title: %s",
         ntcss_pid, ntcss_program_group_title);
#endif

  if ( (!ntcss_pid) || (!ntcss_program_group_title) )
    {
      NtcssError.set_error_code(ERROR_GETTING_NTCSS_PROCESS_ID);
      return;
    }

  if (!(*ntcss_pid && *ntcss_program_group_title))
    {
      NtcssError.set_error_code(ERROR_GETTING_NTCSS_PROCESS_ID);
      return;
    }

  memcpy(token, PRETOKEN, PRETOKEN_SIZE);
  memcpy(token + PRETOKEN_SIZE, ntcss_pid, MAX_TOKEN_LEN - PRETOKEN_SIZE);
  memcpy(progroup_title, ntcss_program_group_title, MAX_PROGROUP_TITLE_LEN);
}


/*****************************************************************************/

static const char  *get_api_function_name()

{
  switch(api_code)
    {
    case API_CODE_NtcssFilteredServerProcessList:
      return("NtcssFilteredServerProcessList");
    case API_CODE_NtcssFilteredServerProcessListCt:
      return("NtcssFilteredServerProcessListCt");
    case API_CODE_NtcssGetAppData:          return("NtcssGetAppData");
    case API_CODE_NtcssGetAppName:          return("NtcssGetAppName");
    case API_CODE_NtcssGetAppPassword:      return("NtcssGetAppPassword");
    case API_CODE_NtcssGetAppRole:          return("NtcssGetAppRole");
    case API_CODE_NtcssGetAppUserInfo:      return("NtcssGetAppUserInfo");
    case API_CODE_NtcssGetCommonData:       return("NtcssGetCommonData");
    case API_CODE_NtcssGetDateTime:         return("NtcssGetDateTime");
    case API_CODE_NtcssGetHostName:         return("NtcssGetHostName");
    case API_CODE_NtcssGetLinkData:         return("NtcssGetLinkData");
    case API_CODE_NtcssGetLocalIP:          return("NtcssGetLocalIP");
    case API_CODE_NtcssGetServerName:       return("NtcssGetServerName");
    case API_CODE_NtcssGetUserInfo:         return("NtcssGetUserInfo");
    case API_CODE_NtcssGetUserRole:         return("NtcssGetUserRole");
    case API_CODE_NtcssGetAppHost:          return("NtcssGetAppHost");
    case API_CODE_NtcssGetAuthServer:       return("NtcssGetAuthServer");
    case API_CODE_NtcssGetAppDataServer:    return("NtcssGetAppDataServer");
    case API_CODE_NtcssPostAppMessage:      return("NtcssPostAppMessage");
    case API_CODE_NtcssPostMessage:         return("NtcssPostMessage");
    case API_CODE_NtcssPostSystemMessage:   return("NtcssPostSystemMessage");
    case API_CODE_NtcssPrintServerFile:     return("NtcssPrintServerFile");
    case API_CODE_NtcssSetAppData:          return("NtcssSetAppData");
    case API_CODE_NtcssSetLinkData:         return("NtcssSetLinkData");
    case API_CODE_NtcssStartBootJobs:       return("NtcssStartBootJobs");
    case API_CODE_NtcssStartServerProcess:  return("NtcssStartServerProcess");
    case API_CODE_NtcssStopServerProcess:   return("NtcssStopServerProcess");
    case API_CODE_NtcssRemoveServerProcess: return("NtcssRemoveServerProcess");
    default:                                return("UnknownAPIFunction");
  }
}


/*****************************************************************************/

static void  split_real_name( real_name, first_name, middle_name, last_name )

     char  *real_name;
     char  *first_name;
     char  *middle_name;
     char  *last_name;
{
  int  real_name_pos = 0;

  first_name[0]  = '\0';
  middle_name[0] = '\0';
  last_name[0]   = '\0';

  real_name_pos = stripWord(real_name_pos, real_name, first_name);
  if (real_name_pos > 0)
    {
      real_name_pos = stripWord(real_name_pos, real_name, middle_name);
      if (real_name_pos < 1)
        {
          memcpy(last_name, first_name, NTCSS_SIZE_USERFIRSTNAME);
          first_name[0] = 0;
        }
      else
        {
          real_name_pos = stripWord(real_name_pos, real_name, last_name);
          if (real_name_pos < 1)
            { 
              memcpy(last_name, middle_name, NTCSS_SIZE_USERMIDDLENAME);
              middle_name[0] = 0;
            }
        }
    }
}


/*****************************************************************************/
/* Fills the global common_user_data structure.
 */

static void  get_common_user_data(callers_user_name)

     const char  *callers_user_name;
{
  int   this_hosts_type;
  int   x;   /* General return value */
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   real_name[REAL_NAME_SIZE + 1];
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];
  char   user_name[MAX_LOGIN_NAME_LEN + 1];

  get_login_name(login_name);
  if (NtcssError.occurred())
    return;

  if (strlen(callers_user_name) == 0)
    memcpy(user_name, login_name, MAX_LOGIN_NAME_LEN);
  else
    memcpy(user_name, callers_user_name, MAX_LOGIN_NAME_LEN);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return;

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return;
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return;
    }

  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT,
                      GET_USER_DATA_ACCEPT_LABEL,
                      GET_USER_DATA_REJECT_LABEL,
                      GET_USER_DATA_REQUEST_LABEL,
                      GET_USER_DATA_RESPONSE_SIZE,
                      GET_USER_DATA_TIMEOUT_VALUE);
  else
    NtcssMessage.open(this_hosts_name,
                      DATABASE_PORT,
                      GET_USER_DATA_ACCEPT_LABEL,
                      GET_USER_DATA_REJECT_LABEL,
                      GET_USER_DATA_REQUEST_LABEL,
                      GET_USER_DATA_RESPONSE_SIZE,
                      GET_USER_DATA_TIMEOUT_VALUE);
  if (NtcssError.occurred())
    return;

  NtcssMessage.write( login_name,     MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( token,          MAX_TOKEN_LEN          );
  NtcssMessage.write( user_name,      MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( progroup_title, MAX_PROGROUP_TITLE_LEN );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return;
    }

  unbundleData(NtcssMessage.response,"CCCIIICCIC",
               real_name,
               common_user_data.ssn,
               common_user_data.phone_number,
               &(common_user_data.security_class),
               &(common_user_data.unix_id),
               &(common_user_data.user_role),
               common_user_data.app_data,
               common_user_data.app_passwd,
               &(common_user_data.registered_user),
               common_user_data.program_access);

  split_real_name(real_name, 
                  common_user_data.first_name, 
                  common_user_data.middle_name, 
                  common_user_data.last_name );
  
  memcpy(common_user_data.login_name, user_name, MAX_LOGIN_NAME_LEN);

  NtcssMessage.close();
}


/*****************************************************************************/

static void  check_security_level( security_level )

     int  security_level;
{
  if ( (security_level < UNCLASSIFIED_CLEARANCE) ||
       (security_level > TOPSECRET_CLEARANCE) )
    NtcssError.set_error_code(ERROR_INVALID_SECURITY_LEVEL);
  return;
}


/*****************************************************************************/

static void  check_priority_code( priority_code )

     int  priority_code;
{
  if ( (priority_code < LOW_PRIORITY) ||
       (priority_code > HIGH_PRIORITY) )
    NtcssError.set_error_code(ERROR_INVALID_PRIORITY_CODE);

  return;
}


/*****************************************************************************/

static void  fill_login_name( login_name, callers_login_name )

     char  *login_name;
     char  *callers_login_name;
{
  if (strlen(callers_login_name) == 0)
    get_login_name(login_name);
  else
    memcpy(login_name, callers_login_name, MAX_LOGIN_NAME_LEN);
}


/*****************************************************************************/

static void  get_default_printer_info( output_type, progroup_title,
                                       printer_name, printer_host_name )

     char  *output_type;
     char  *progroup_title;
     char  *printer_name;
     char  *printer_host_name;
{
  int   number_of_printers;
  int   this_hosts_type;
  int   x;                       /* General purpose */
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return;
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return;
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT, 
                      GET_DEFAULT_PRINTER_INFO_ACCEPT_LABEL,
                      GET_DEFAULT_PRINTER_INFO_REJECT_LABEL,
                      GET_DEFAULT_PRINTER_INFO_REQUEST_LABEL,
                      GET_DEFAULT_PRINTER_INFO_RESPONSE_SIZE,
                      GET_DEFAULT_PRINTER_INFO_TIMEOUT_VALUE);
  else
    NtcssMessage.open(this_hosts_name,
                      DATABASE_PORT, 
                      GET_DEFAULT_PRINTER_INFO_ACCEPT_LABEL,
                      GET_DEFAULT_PRINTER_INFO_REJECT_LABEL,
                      GET_DEFAULT_PRINTER_INFO_REQUEST_LABEL,
                      GET_DEFAULT_PRINTER_INFO_RESPONSE_SIZE,
                      GET_DEFAULT_PRINTER_INFO_TIMEOUT_VALUE);
  if (NtcssError.occurred())
    return;

  NtcssMessage.write( progroup_title, MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.write( output_type, NTCSS_SIZE_PRT_OUTPUT_TYPE );
  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return;
    }

  unbundleData(NtcssMessage.response, "ICC",
               &number_of_printers,
               printer_name,
               printer_host_name);

  NtcssMessage.close();
}


/*****************************************************************************/

static void  get_start_process_flag_string(start_process_flag_string,
                                           process_description)

     char  *start_process_flag_string;
     NtcssServerProcessDescr  *process_description;
{
  if (process_description->needs_device_flag)
    memcpy(start_process_flag_string, START_SERVER_PROCESS_NEED_DEVICE,
           START_PROCESS_FLAG_SIZE);
  else
    if (process_description->needs_approval_flag)
      memcpy(start_process_flag_string, START_SERVER_PROCESS_NEED_APPROVAL,
             START_PROCESS_FLAG_SIZE);
    else
      memcpy(start_process_flag_string, START_SERVER_PROCESS_READY_TO_START,
             START_PROCESS_FLAG_SIZE);
}


/*****************************************************************************/

static void  get_printer_flag_string(printer_flag_string, process_description)
     
     char  *printer_flag_string;
     NtcssServerProcessDescr  *process_description;
{
  int  printer_flag;

  printer_flag = ((process_description->prt_output_flag & 1)
                       << PRINTER_OUTPUT_FLAG_BIT_POSITION) +
                 ((!(process_description->save_prt_output_flag & 1))
                       << SAVE_OUTPUT_FLAG_BIT_POSITION) +
                 ((process_description->restartable_flag & 1)
                       << RESTART_FLAG_BIT_POSITION);

  sprintf(printer_flag_string, "%0*d", PRINTER_FLAG_STRING_SIZE, printer_flag);
}


/*****************************************************************************/

static void  get_command_line(command_line, process_description)

     char  *command_line;
     NtcssServerProcessDescr  *process_description;
{
  sprintf(command_line, "%s %s", process_description->prog_name_str,
          process_description->prog_args_str);
}


/*****************************************************************************/

static void  get_batch_user(login_name, token, progroup_title, pdj_progroup,
                            batch_user)

     char  *login_name;
     char  *token;
     char  *progroup_title;
     char  *pdj_progroup;
     char  *batch_user;
{
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  int   this_hosts_type;
  int   x;                       /* General purpose */

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return;
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return;
    }

  /* If this is a slave server, we have to get this info from the master. */
  if (this_hosts_type == NTCSS_APP_SERVER_TYPE)
    NtcssMessage.open("$MASTER$",
                      DATABASE_PORT, 
                      GET_BATCH_USER_ACCEPT_LABEL,
                      GET_BATCH_USER_REJECT_LABEL,
                      GET_BATCH_USER_REQUEST_LABEL,
                      GET_BATCH_USER_RESPONSE_SIZE,
                      GET_BATCH_USER_TIMEOUT_VALUE);
  else
    NtcssMessage.open(this_hosts_name,
                      DATABASE_PORT, 
                      GET_BATCH_USER_ACCEPT_LABEL,
                      GET_BATCH_USER_REJECT_LABEL,
                      GET_BATCH_USER_REQUEST_LABEL,
                      GET_BATCH_USER_RESPONSE_SIZE,
                      GET_BATCH_USER_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return;

  NtcssMessage.write( login_name,     MAX_LOGIN_NAME_LEN     );
  NtcssMessage.write( token,          MAX_TOKEN_LEN          );
  NtcssMessage.write( progroup_title, MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.write( pdj_progroup,   MAX_PROGROUP_TITLE_LEN );
  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return;
    }

  unbundleData(NtcssMessage.response, "C", batch_user);
  NtcssMessage.close();

  if (!(*batch_user))
    {
      NtcssError.set_error_code(ERROR_INVALID_BATCH_USER);
      return;
    }

}


/*****************************************************************************/
/* Handles both starting boot jobs and scheduling jobs. */
/* If changed, see NtcssStartServerProcess for possible changes that need to be made there. */

static int  StartServerBootProcess(process_description, pdj_progroup,
                                   type_of_job, schedule_string, pid,pid_size)

     NtcssServerProcessDescr  *process_description;
     char  *pdj_progroup;
     int  type_of_job;
     char  *schedule_string;
     char  *pid;
     int  pid_size;

{
  int   priority_or_schedule_size;
  int   response_size;
  int   timeout_value;
  const char  *accept_label;
  char   banner[PRINTER_BANNER_SIZE + 1];
  char   batch_user[MAX_LOGIN_NAME_LEN + 1];
  char   command_line[MAX_PROC_CMD_LINE_LEN + 1];
  char   copies[PRINTER_COPIES_SIZE + 1];
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   orientation[PRINTER_ORIENTATION_SIZE + 1];
  char   papersize[PRINTER_PAPERSIZE_SIZE + 1];
  char   printer_flag_string[PRINTER_FLAG_STRING_SIZE + 1];
  char   printer_name[MAX_PRT_NAME_LEN + 1];
  char   printer_host_name[MAX_HOST_NAME_LEN + 1];
  char   priority_code[PRIORITY_CODE_SIZE + 1];
  char  *priority_or_schedule;
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  const char  *reject_label;
  const char  *request_label;
  char   sdi_control_record[MAX_SDI_CTL_RECORD_LEN + 2];
  char   sdi_data_file[MAX_SERVER_LOCATION_LEN + 1];
  char   sdi_input_flag[SDI_INPUT_FLAG_SIZE + 1];
  char   sdi_required_flag[SDI_REQUIRED_FLAG_SIZE + 1];
  char   sdi_tci_file[MAX_SERVER_LOCATION_LEN + 1];
  char   security_class[SECURITY_CLASS_SIZE + 1];
  char   start_process_flag_string[START_PROCESS_FLAG_SIZE + 1];
  char   token[MAX_TOKEN_LEN + 1];

  /* done only to get rid of compiler warning about unused parameters */
  pid_size=pid_size;

  if ( (type_of_job != BOOT_JOB) && (type_of_job != SCHEDULED_JOB) )
    {
      NtcssError.set_error_code(ERROR_INVALID_TYPE_OF_JOB);
      return(False);
    }

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  fill_login_name(login_name, process_description->owner_str);
  if (NtcssError.occurred())
    return(False);

  if (process_description->prt_output_flag)
    {
      get_default_printer_info(process_description->output_type_str,
                               progroup_title,
                               printer_name,
                               printer_host_name);
      if (NtcssError.occurred())
        return(False);
    }
  else
    {
      strcpy(printer_name,"");
      strcpy(printer_host_name,"");
    }

  get_batch_user(login_name, token, progroup_title, pdj_progroup, batch_user);
  if (NtcssError.occurred())
    return(False);

  if (type_of_job == BOOT_JOB)
    {
      get_start_process_flag_string(start_process_flag_string,
                                    process_description);
    }
  else
    { 
      /* scheduled job */
      memcpy(start_process_flag_string, SCHEDULE_SERVER_PROCESS_APPROVED,
             START_PROCESS_FLAG_SIZE);
    }

  get_command_line(command_line, process_description);
  get_printer_flag_string(printer_flag_string, process_description);
  sprintf(security_class, "%0*d", SECURITY_CLASS_SIZE,
          process_description->seclvl);
  sprintf(priority_code,"%0*d",PRIORITY_CODE_SIZE,
          process_description->priority_code);

  strcpy(sdi_required_flag, "0");
  strcpy(sdi_input_flag, "0");
  strcpy(sdi_control_record, "");
  strcpy(sdi_data_file, "");
  strcpy(sdi_tci_file, "");
  strcpy(orientation, "00");
  strcpy(banner, "0");
  strcpy(copies, "00");
  strcpy(papersize, "00");

  if (type_of_job == BOOT_JOB)
    {
      request_label              = START_SERVER_PROCESS_REQUEST_LABEL;
      accept_label               = START_SERVER_PROCESS_ACCEPT_LABEL;
      reject_label               = START_SERVER_PROCESS_REJECT_LABEL;
      response_size              = START_SERVER_PROCESS_RESPONSE_SIZE;
      timeout_value              = START_SERVER_PROCESS_TIMEOUT_VALUE;
      priority_or_schedule       = priority_code;
      priority_or_schedule_size  = PRIORITY_CODE_SIZE;
    }
  else
    { 
      /* scheduled job */
      request_label              = SCHEDULE_SERVER_PROCESS_REQUEST_LABEL;
      accept_label               = SCHEDULE_SERVER_PROCESS_ACCEPT_LABEL;
      reject_label               = SCHEDULE_SERVER_PROCESS_REJECT_LABEL;
      response_size              = SCHEDULE_SERVER_PROCESS_RESPONSE_SIZE;
      timeout_value              = SCHEDULE_SERVER_PROCESS_TIMEOUT_VALUE;
      priority_or_schedule       = schedule_string;
      priority_or_schedule_size  = NTCSS_MAX_SCHEDULE_LEN;
    }

  NtcssMessage.open("$CURRENT$",
                    COMMAND_PORT, 
                    accept_label,
                    reject_label,
                    request_label,
                    response_size,
                    timeout_value);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,                   MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token,                        MAX_TOKEN_LEN);
  NtcssMessage.write( pdj_progroup,                 MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.write( start_process_flag_string,    START_PROCESS_FLAG_SIZE);
  NtcssMessage.write( priority_or_schedule,         priority_or_schedule_size);
  NtcssMessage.write( command_line,                 MAX_PROC_CMD_LINE_LEN);
  NtcssMessage.write( batch_user,                   MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( process_description->job_descrip_str,
                      MAX_JOB_DESCRIP_LEN);
  NtcssMessage.write( security_class,               SECURITY_CLASS_SIZE);
  NtcssMessage.write( printer_name,                 MAX_PRT_NAME_LEN);
  NtcssMessage.write( printer_host_name,            MAX_HOST_NAME_LEN);
  NtcssMessage.write( security_class,               SECURITY_CLASS_SIZE);
  NtcssMessage.write( process_description->prt_filename_str,
                      PRINT_FILE_NAME_SIZE);
  NtcssMessage.write( printer_flag_string,          PRINTER_FLAG_STRING_SIZE);

  if (type_of_job == BOOT_JOB)
    {
      NtcssMessage.write( sdi_required_flag,   SDI_REQUIRED_FLAG_SIZE );
      NtcssMessage.write( sdi_input_flag,      SDI_INPUT_FLAG_SIZE );
      NtcssMessage.write( sdi_control_record,  MAX_SDI_CTL_RECORD_LEN + 1 );
      NtcssMessage.write( sdi_data_file,       MAX_SERVER_LOCATION_LEN );
      NtcssMessage.write( sdi_tci_file,        MAX_SERVER_LOCATION_LEN );
      NtcssMessage.write( orientation,         PRINTER_ORIENTATION_SIZE );
      NtcssMessage.write( banner,              PRINTER_BANNER_SIZE );
      NtcssMessage.write( copies,              PRINTER_COPIES_SIZE );
      NtcssMessage.write( papersize,           PRINTER_PAPERSIZE_SIZE );
    }

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "C", pid);
  NtcssMessage.close();
  return(True);
}


/*****************************************************************************/

BOOL  NtcssRemoveServerProcess(pid,scheduled_process_flag)
     
     char  *pid;
     int  scheduled_process_flag;
{
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   scheduled_process_string[SCHEDULED_FLAG_SIZE + 1];
  char   token[MAX_TOKEN_LEN + 1];

  reset_api(API_CODE_NtcssRemoveServerProcess);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  if (scheduled_process_flag)
    strcpy(scheduled_process_string, " 1");
  else
    strcpy(scheduled_process_string, " 0");

  NtcssMessage.open("$CURRENT$",
                    SERVER_PROCESS_QUEUE_PORT,
                    REMOVE_SERVER_PROCESS_ACCEPT_LABEL,
                    REMOVE_SERVER_PROCESS_REJECT_LABEL,
                    REMOVE_SERVER_PROCESS_REQUEST_LABEL,
                    REMOVE_SERVER_PROCESS_RESPONSE_SIZE,
                    REMOVE_SERVER_PROCESS_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,               MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,                    MAX_TOKEN_LEN );
  NtcssMessage.write( pid,                      MAX_PROCESS_ID_LEN );
  NtcssMessage.write( scheduled_process_string, SCHEDULED_FLAG_SIZE );
  NtcssMessage.write( progroup_title,           MAX_PROGROUP_TITLE_LEN );

  NtcssMessage.read();
  NtcssMessage.close();

  return(! NtcssError.occurred());
}


/*****************************************************************************/

static int  getAppHostName(username, appname, hostname)
     
     char  *username;
     char  *appname;
     char  *hostname;
{
  int   i, x;
  int   number_of_apps;
  int   this_hosts_type;
  char   apphost[MAX_HOST_NAME_LEN + 1];
  char   progroup[MAX_PROGROUP_TITLE_LEN + 1];
  char  *response;
  char  *response_ptr;
  char   this_hosts_name[MAX_HOST_NAME_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];

  get_ntcss_info(token, progroup);
  if (NtcssError.occurred())
    return(False);

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
      return(False);
    }

  this_hosts_type = getHostType(this_hosts_name, NTCSS_DB_DIR);
  if (this_hosts_type < 0)
    {
      NtcssError.set_error_code(ERROR_GETTING_HOST_TYPE);
      return(False);
    }


  /* On master and auth servers, this info is in local database.. */
  if (this_hosts_type != NTCSS_APP_SERVER_TYPE)
    {
      (void) GetProfileString(appname, "SERVER", "UNKNOWN", hostname, 
                              MAX_PROGROUP_TITLE_LEN, NTCSS_PROGRP_DB_FILE);
      x = strcmp(hostname, "UNKNOWN");
      if (x == 0)
        {
          NtcssError.set_error_code(ERROR_GETTING_HOST_NAME);
          return(False);
        }
      else
        return(True);
    }

  /* If this is a slave, we have to get this info from the master.. */
  NtcssMessage.open("$MASTER$",
                    DATABASE_PORT, 
                    GET_APP_LIST_ACCEPT_LABEL,
                    GET_APP_LIST_REJECT_LABEL,
                    GET_APP_LIST_REQUEST_LABEL,
                    GET_APP_LIST_RESPONSE_SIZE,
                    GET_APP_LIST_TIMEOUT_VALUE);
  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( username, MAX_LOGIN_NAME_LEN );
  NtcssMessage.write( token,    MAX_TOKEN_LEN );

  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  if (!NtcssMessage.response)
    {
      NtcssMessage.close();
      return(True);
    }
  
  response_ptr = malloc(strlen(NtcssMessage.response));
  response = response_ptr;
  if (response == NULL)
    {
      NtcssError.set_error_code(ERROR_ALLOCATING_MEMORY);
      return(False);
    }

  memcpy(response, NtcssMessage.response, strlen(NtcssMessage.response));

  unbundleData(response, "I", &number_of_apps);
    
  response += getOverallLength("I", number_of_apps);

  /* Find the right host/app combo and put the result in the hostname var. */
  for (i = 1; i <= number_of_apps; i++)
    {
      memset(apphost, NTCSS_NULLCH, MAX_HOST_NAME_LEN + 1);
      memset(progroup, NTCSS_NULLCH, MAX_PROGROUP_TITLE_LEN + 1);
      
      unbundleData(response, "CC", progroup, apphost);

      if (!strcmp(appname, progroup) && (strlen(appname) == strlen(progroup)))
        {
          strcpy(hostname, apphost);
          break;
        }

      response += getOverallLength("CC", progroup, apphost);
    }

  free(response_ptr);

  NtcssMessage.close();

  return(True);
}


/*****************************************************************************/

static void get_application_hostname(app_name_str, host_buf, host_buf_size)

     char *app_name_str;
     char *host_buf;
     int  host_buf_size;
{
  char progrps_file[MAX_FILENAME_LEN];

  /* get application's host server from the progrps ini file */
  sprintf(progrps_file,"%s/%s", NTCSS_DB_DIR, NTCSS_PROGRP_INI_FILE);
  if (GetProfileString(app_name_str, "SERVER", "NONE", host_buf,
                                         host_buf_size, progrps_file) == -1)
     NtcssError.set_error_code(ERROR_GETTING_APPLICATION_HOST);

  return;
}

/*****************************************************************************/

static void get_authentication_server(pas)

     char* pas;
{
  char user[MAX_LOGIN_NAME_LEN + 1];

  /* get user's login */
  get_login_name(user);
  if(!NtcssError.occurred())

    /* lookup the user's auth server in the NIS map */
    if(getNtcssAuthServerByLoginNameFromNis(user, pas) != 0)
      NtcssError.set_error_code(ERROR_GETTING_AUTH_SERVER);

  return;
}  


/*****************************************************************************/

static void get_application_data_server(app_name, host_name, host_name_length)

     char *app_name;
     char *host_name;
     int  host_name_length;
{

  /* get the host for the named application */
  if (NtcssGetAppHost(app_name, host_name, host_name_length) == False) 
    NtcssError.set_error_code(ERROR_GETTING_APPLICATION_HOST);
  else {
    /* get the type for the retrieved host. if the host type is Application
       server, lookup the MASTER host and return that value */
    if (getHostType(host_name, NTCSS_DB_DIR) == NTCSS_APP_SERVER_TYPE)
      if (NtcssGetServerName(host_name, host_name_length) == False)
         NtcssError.set_error_code(ERROR_GETTING_SERVER_NAME);
  }
  
  return;
} 

/*****************************************************************************/
BOOL  NtcssValidateUser(username, password, ssn, ssn_size)

     char  *username;
     char *password;
     char  *ssn;
     int  ssn_size;
{
  char   login_name[MAX_LOGIN_NAME_LEN + 1];
  char   token[MAX_TOKEN_LEN + 1];
  char   progroup_title[MAX_PROGROUP_TITLE_LEN + 1];
  char   crypt_key[MAX_APP_PASSWD_LEN * 2];
  char   *crypt_password;
  int    len;


  reset_api(API_CODE_NtcssValidateUser);

  get_login_name(login_name);
  if (NtcssError.occurred())
    return(False);

  get_ntcss_info(token, progroup_title);
  if (NtcssError.occurred())
    return(False);

  check_buffer_size(ssn_size, MAX_APP_PASSWD_LEN);
  if (NtcssError.occurred())
    return(False);

  sprintf(crypt_key,"%s%s",username,username);
 
  plainEncryptString(crypt_key,password,&crypt_password,0,1);
  strcat(crypt_password,":");

  NtcssMessage.open("$CURRENT$",
                    DATABASE_PORT, 
                    VALIDATE_USER_ACCEPT_LABEL,
                    VALIDATE_USER_REJECT_LABEL,
                    VALIDATE_USER_REQUEST_LABEL,
                    VALIDATE_USER_RESPONSE_SIZE,
                    VALIDATE_USER_TIMEOUT_VALUE);

  if (NtcssError.occurred())
    return(False);

  NtcssMessage.write( login_name,                   MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( token,                        MAX_TOKEN_LEN);
  NtcssMessage.write( username,                     MAX_LOGIN_NAME_LEN);
  NtcssMessage.write( crypt_password,               MAX_APP_PASSWD_LEN * 2);
  NtcssMessage.write( progroup_title,               MAX_PROGROUP_TITLE_LEN);
  NtcssMessage.read();
  if (NtcssError.occurred())
    {
      NtcssMessage.close();
      return(False);
    }

  unbundleData(NtcssMessage.response, "IC",&len, ssn);

  NtcssMessage.close();

  free(crypt_password);

  return(True);
}

