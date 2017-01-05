/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
 * interactive_test_api.c
 */

/** INCLUDES **/
#include <errno.h>
#include <termio.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "NtcssApi.h"
#include <ExtraProtos.h>
#include <LibNtcssFcts.h>

#define BACKSPACE 8
#define ENTER     10
#define SPACE     32
#define MAX_NUMBER_OF_APIs 50
#define MAX_PROCESS_LIST   100
#define SILENT  1
#define NOT_SILENT  0

typedef struct _api_test_structure{
    const char *name;
    void (*test)(void);
    void (*get_parameters)(void);
    void (*print_parameters)(void);
    void (*print_results)(void);
} api_test_structure;

/*** GLOBALS ***/
int   api_number = 1;
int   let_the_rest_be_default = False;
int   number_of_copies;
int   process_count;
int   returned_process_count;
int   scheduled_flag;
int   security_class;
int   signal_type;
int   success;
int   test_data_count = -1;

api_test_structure          api_tests[MAX_NUMBER_OF_APIs + 1];
NtcssServerProcessDescr     process_description;
NtcssProcessFilter          process_filter;
NtcssServerProcessDetails   process_list[MAX_PROCESS_LIST];
NtcssUserInfo               user_info;

char    app_data[MAX_APP_DATA_LEN];
char    app_data_buffer[MAX_APP_DATA_LEN];
char    app_password_buffer[MAX_APP_PASSWD_LEN];
char    appname[MAX_PROGROUP_TITLE_LEN];
char    application_name[MAX_PROGROUP_TITLE_LEN];
char    common_data[MAX_COMDB_DATA_LEN];
char    common_tag[MAX_COMDB_TAG_LEN];
char    date_time_buffer[NTCSS_SIZE_DATETIME1];
char    error_message[MAX_ERR_MSG_LEN];
char    host_name[MAX_HOST_NAME_LEN];
char    link_data[MAX_APP_PASSWD_LEN];
char    local_pid[MAX_PROCESS_ID_LEN];
char    message[MAX_MSG_LEN];
char    message_board[MAX_BB_NAME_LEN];
char    message_title[MAX_MSG_TITLE_LEN];
char    ip_addr_buffer[MAX_IP_ADDRESS_LEN];
char    options[MAX_PRT_OPTIONS_LEN];
char    output_type[NTCSS_SIZE_PRT_OUTPUT_TYPE];
char    pid[MAX_PROCESS_ID_LEN];
char    print_filename[NTCSS_SIZE_FILENAME];
char    prog_access[NTCSS_SIZE_APP_PROGRAM_INFO];
char    remhost[MAX_HOST_NAME_LEN + 1];
char    server_name[MAX_HOST_NAME_LEN];
char    user_name[MAX_LOGIN_NAME_LEN];
char	password[MAX_APP_PASSWD_LEN * 2];
char	ssn[MAX_APP_PASSWD_LEN + 1];

void  nada(void);
char  read_character(void);
void  read_string(char*,int);
void  get_number(const char*, int*);
void  get_string(const char*, char*,int);
void  waitkey(void);
void  print_header(void);
void  print_footer(void);
void  print_error_message(void);
static void  clear_string(char*);
static void  NtcssGetCommonData_test(void);
static void  NtcssGetCommonData_get_parameters(void);
static void  NtcssGetCommonData_print_parameters(void);
static void  NtcssGetCommonData_print_results(void);
static void  NtcssGetFilteredServerProcessList_test(void);
static void  NtcssGetFilteredServerProcessList_get_parameters(void);
static void  NtcssGetFilteredServerProcessList_print_parameters(void);
static void  NtcssGetFilteredServerProcessList_print_results(void);
static void  NtcssGetFilteredServerProcessListCt_test(void);
static void  NtcssGetFilteredServerProcessListCt_get_parameters(void);
static void  NtcssGetFilteredServerProcessListCt_print_parameters(void);
static void  NtcssGetFilteredServerProcessListCt_print_results(void);
static void NtcssGetUserInfo_test(void);
static void  NtcssGetUserInfo_get_parameters(void);
static void  NtcssGetUserInfo_print_parameters(void);
static void  NtcssGetUserInfo_print_results(void);
static void  NtcssGetAppData_test(void);
static void  NtcssGetAppData_get_parameters(void);
static void  NtcssGetAppData_print_parameters(void);
static void  NtcssGetAppData_print_results(void);
static void  NtcssGetAppPassword_test(void);
static void  NtcssGetAppPassword_get_parameters(void);
static void  NtcssGetAppPassword_print_parameters(void);
static void  NtcssGetAppPassword_print_results(void);
static void  NtcssGetAppUserInfo_test(void);
static void  NtcssGetAppUserInfo_get_parameters(void);
static void  NtcssGetAppUserInfo_print_parameters(void);
static void  NtcssGetAppUserInfo_print_results(void);
static void  NtcssGetDateTime_test(void);
static void  NtcssGetDateTime_get_parameters(void);
static void  NtcssGetDateTime_print_parameters(void);
static void  NtcssGetDateTime_print_results(void);
static void  NtcssGetLocalIP_test(void);
static void  NtcssGetLocalIP_get_parameters(void);
static void  NtcssGetLocalIP_print_parameters(void);
static void  NtcssGetLocalIP_print_results(void);
static void  NtcssGetHostName_test(void);
static void  NtcssGetHostName_get_parameters(void);
static void  NtcssGetHostName_print_parameters(void);
static void  NtcssGetHostName_print_results(void);
static void  NtcssGetServerName_test(void);
static void  NtcssGetServerName_get_parameters(void);
static void  NtcssGetServerName_print_parameters(void);
static void  NtcssGetServerName_print_results(void);
static void  NtcssStartServerProcessEx_test(void);
static void  NtcssStartServerProcessEx_get_parameters(void);
static void  NtcssStartServerProcessEx_print_parameters(void);
static void  NtcssStartServerProcessEx_print_results(void);
static void  NtcssStartServerProcess_test(void);
static void  NtcssStartServerProcess_get_parameters(void);
static void  NtcssStartServerProcess_print_parameters(void);
static void  NtcssStartServerProcess_print_results(void);
static void  NtcssStopServerProcess_test(void);
static void  NtcssStopServerProcess_get_parameters(void);
static void  NtcssStopServerProcess_print_parameters(void);
static void  NtcssStopServerProcess_print_results(void);
static void  NtcssRemoveServerProcess_test(void);
static void  NtcssRemoveServerProcess_get_parameters(void);
static void  NtcssRemoveServerProcess_print_parameters(void);
static void  NtcssRemoveServerProcess_print_results(void);
static void  NtcssPrintServerFile_test(void);
static void  NtcssPrintServerFile_get_parameters(void);
static void  NtcssPrintServerFile_print_parameters(void);
static void  NtcssPrintServerFile_print_results(void);
static void  NtcssSetAppData_test(void);
static void  NtcssSetAppData_get_parameters(void);
static void  NtcssSetAppData_print_parameters(void);
static void  NtcssSetAppData_print_results(void);
/*****
static void  NtcssGetAppRole_test(void);
static void  NtcssGetAppRole_get_parameters(void);
static void  NtcssGetAppRole_print_parameters(void);
static void  NtcssGetAppRole_print_results(void);

static void  NtcssGetUserRole_test(void);
static void  NtcssGetUserRole_get_parameters(void);
static void  NtcssGetUserRole_print_parameters(void);
static void  NtcssGetUserRole_print_results(void);

static void  NtcssGetAppName_test(void);
static void  NtcssGetAppName_get_parameters(void);
static void  NtcssGetAppName_print_parameters(void);
****/
static void  NtcssGetAppName_print_results(void);
/****
static void  NtcssSetCommonData_test(void);
static void  NtcssSetCommonData_get_parameters(void);
static void  NtcssSetCommonData_print_parameters(void);
static void  NtcssSetCommonData_print_results(void);
*****/
static void  NtcssSetLinkData_test(void);
static void  NtcssSetLinkData_get_parameters(void);
static void  NtcssSetLinkData_print_parameters(void);
static void  NtcssSetLinkData_print_results(void);
static void  NtcssGetLinkData_test(void);
static void  NtcssGetLinkData_get_parameters(void);
static void  NtcssGetLinkData_print_parameters(void);
static void  NtcssGetLinkData_print_results(void);
static void  NtcssPostMessage_test(void);
static void  NtcssPostMessage_get_parameters(void);
static void  NtcssPostMessage_print_parameters(void);
static void NtcssPostMessage_print_results(void);
static void  NtcssPostSystemMessage_test(void);
static void  NtcssPostSystemMessage_get_parameters(void);
static void  NtcssPostSystemMessage_print_parameters(void);
static void  NtcssPostSystemMessage_print_results(void);
static void  NtcssPostAppMessage_test(void);
static void  NtcssPostAppMessage_get_parameters(void);
static void  NtcssPostAppMessage_print_parameters(void);
static void  NtcssPostAppMessage_print_results(void);
static void  NtcssGetAppHost_test(void);
static void  NtcssGetAppHost_get_parameters(void);
static void  NtcssGetAppHost_print_parameters(void);
static void  NtcssGetAppHost_print_results(void);
static void  NtcssGetAuthServer_test(void);
static void  NtcssGetAuthServer_get_parameters(void);
static void  NtcssGetAuthServer_print_parameters(void);
static void  NtcssGetAuthServer_print_results(void);
static void  NtcssGetAppDataServer_test(void);
static void  NtcssGetAppDataServer_get_parameters(void);
static void  NtcssGetAppDataServer_print_parameters(void);
static void  NtcssGetAppDataServer_print_results(void);
static void  NtcssGetAppName_test(void);
static void  NtcssGetAppName_get_parameters(void);
static void  NtcssGetAppName_print_parameters(void);

static void  NtcssValidateUser_test(void);
static void  NtcssValidateUser_get_parameters(void);
static void  NtcssValidateUser_print_parameters(void);
static void  NtcssValidateUser_print_results(void);

void  load_test( const char*, void (*)(void), void (*)(void), void (*)(void), 
                 void(*)(void));
void  load_functions(void);
void  get_parameters(void);
void  print_parameters(void);
void  call_api(void);
void  print_results(void);
void  print_menu(void);
void  main_menu(void);
int  main(int, char*[]);

/*****************************************************************************/

void  nada()

{
  return;
}


/*****************************************************************************/

char  read_character()

{
  struct termio   save;
  struct termio   term;
  char   ch = '\0';
  
  if (ioctl(0, TCGETA, &term) == -1)
 {
    printf("standard input not a tty\n");
    return('\0');
  }

  save = term;

  term.c_lflag &= ~ICANON;
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;

  ioctl(0, TCSETA, &term);
  read(0, &ch, 1);

  ioctl(0, TCSETA, &save);

  return(ch);
}


/*****************************************************************************/

void  read_string(buffer,silent)

     char  *buffer;
     int   silent;
{
  char  *start;

  start = buffer;

  fflush(stdout);

  while((*buffer = getchar()) != ENTER)
    {
      if (*buffer == BACKSPACE)
        {
          if (buffer > start)
            {
              printf("%c%c", SPACE, BACKSPACE);
              buffer--;
            }
        }
      else {
        if (silent) {
              printf("%c%c", BACKSPACE,'*');
	}
        buffer++;
      }
    }
  *buffer = '\0';
}


/*****************************************************************************/

void  get_number(label, number)

     const char  *label;
     int  *number;
{
  char local_string[100];

  printf(label);
  read_string(local_string,NOT_SILENT);

  if (local_string[0] == '*')
    let_the_rest_be_default = True;
  else
    if (local_string[0] != '=')
      *number = atoi(local_string);
}


/*****************************************************************************/

void  get_string(label, string_data,silent)

     const char  *label;
     char        *string_data;
     int         silent;
{
  char   local_string[100];

  printf(label);
  read_string(local_string,silent);

  if (local_string[0] == '*')
    let_the_rest_be_default = True;
  else
    if (local_string[0] != '=')
      strcpy(string_data, local_string);
}


/*****************************************************************************/

void  waitkey()

{
  (void) read_character();
}


/*****************************************************************************/

void  print_header()
     
{
  char   header_line[50];

  memset(header_line, '-', 50);
  header_line[strlen(api_tests[api_number].name)] = 0;

  printf("\n");
  printf("Testing %s... ", api_tests[api_number].name);

  syslog(LOG_WARNING, "START TEST OF %s", api_tests[api_number].name);
}


/*****************************************************************************/

void  print_footer()

{
  syslog(LOG_WARNING, "END TEST OF %s", api_tests[api_number].name);

  printf("\n\nPress any key to continue...");
  waitkey();
  printf("\n\n");
}


/*****************************************************************************/

void  print_error_message()

{
  printf("Failure.\n\n");

  if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
    printf("%s\n", error_message);
  else
    printf("NTCSS error message is not available.\n");
}


/*****************************************************************************/

static void  clear_string(string)

     char  *string;
{
  strcpy(string, "!set");
}


/*****************************************************************************/

static void  NtcssGetCommonData_test() 

{ 
  clear_string(common_data);
  success = NtcssGetCommonData(common_tag, common_data,
                               MAX_COMDB_DATA_LEN); 
}


/*****************************************************************************/

static void  NtcssGetCommonData_get_parameters() 

{ 
  clear_string(common_tag);
  get_string("tag: ", common_tag,SILENT); 
}


/*****************************************************************************/

static void  NtcssGetCommonData_print_parameters() 

{ 
  printf("\nParameters...\n");
  printf("tag: %s", common_tag); 
}


/*****************************************************************************/

static void  NtcssGetCommonData_print_results() 

{
  printf("%s: %s\n", common_tag,common_data); 
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessList_test() 

{ 
  returned_process_count = NtcssGetFilteredServerProcessList(&process_filter,
                                                             process_list,
                                                             process_count);
  if (returned_process_count != -1)
    success = 1;
  else
    success = 0;
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessList_get_parameters()

{
  process_count = MAX_PROCESS_LIST;
  process_filter.get_scheduled_jobs = 0;
  process_filter.get_unscheduled_jobs = 0;
  process_filter.get_only_running_jobs = 0;
  process_filter.get_for_current_user_only = 0;
  process_filter.priority_flag = 0;
  process_filter.command_line_flag = 0;
  process_filter.cust_proc_status_flag = 0;
  process_filter.login_name_flag = 0;
  process_filter.unix_owner_flag = 0;
  process_filter.progroup_title_flag = 0;
  process_filter.job_descrip_flag = 0;
  process_filter.orig_host_flag = 0;
  process_filter.priority = 0;
  strcpy(process_filter.command_line, "");
  strcpy(process_filter.cust_proc_status, "");
  strcpy(process_filter.login_name, "");
  strcpy(process_filter.unix_owner, "");
  strcpy(process_filter.progroup_title, "");
  strcpy(process_filter.job_descrip, "");
  strcpy(process_filter.orig_host, "");

  get_number("                  process_count: ",
             &process_count);
  if (let_the_rest_be_default)
    return;

  get_number("       get_scheduled_jobs (0,1): ",
             &process_filter.get_scheduled_jobs);
  if (let_the_rest_be_default)
    return;

  get_number("     get_unscheduled_jobs (0,1): ",
             &process_filter.get_unscheduled_jobs);
  if (let_the_rest_be_default)
    return;

  get_number("    get_only_running_jobs (0,1): ",
             &process_filter.get_only_running_jobs);
  if (let_the_rest_be_default)
    return;

  get_number("get_for_current_user_only (0,1): ",
             &process_filter.get_for_current_user_only);
  if (let_the_rest_be_default)
    return;

  get_number("            priority_flag (0,1): ",
             &process_filter.priority_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.priority_flag)
    get_number("                       priority: ",
               &process_filter.priority);
  if (let_the_rest_be_default)
    return;

  get_number("        command_line_flag (0,1): ",
             &process_filter.command_line_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.command_line_flag)
    get_string("                   command_line: ",
               process_filter.command_line,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("  custom_proc_status_flag (0,1): ",
             &process_filter.cust_proc_status_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.cust_proc_status_flag)
    get_string("               cust_proc_status: ",
               process_filter.cust_proc_status,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("          login_name_flag (0,1): ",
             &process_filter.login_name_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.login_name_flag)
    get_string("                     login_name: ",
               process_filter.login_name,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("          unix_owner_flag (0,1): ",
             &process_filter.unix_owner_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.unix_owner_flag)
    get_string("                     unix_owner: ",
               process_filter.unix_owner,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("      progroup_title_flag (0,1): ",
             &process_filter.progroup_title_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.progroup_title_flag)
   get_string("                 progroup_title: ",
               process_filter.progroup_title,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("     job_description_flag (0,1): ",
             &process_filter.job_descrip_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.job_descrip_flag)
   get_string("                    job_descrip: ",
               process_filter.job_descrip,NOT_SILENT);
  if (let_the_rest_be_default)
    return;

  get_number("       original_host_flag (0,1): ",
             &process_filter.orig_host_flag);
  if (let_the_rest_be_default)
    return;

  if (process_filter.orig_host_flag)
   get_string("                      orig_host: ", process_filter.orig_host,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessList_print_parameters()

{
  printf("\nParameters...\n");
  printf("                  process_count: %d\n", process_count);
  printf("       get_scheduled_jobs (0,1): %d\n",
         process_filter.get_scheduled_jobs);        
  printf("     get_unscheduled_jobs (0,1): %d\n",
         process_filter.get_unscheduled_jobs);      
  printf("    get_only_running_jobs (0,1): %d\n",
         process_filter.get_only_running_jobs);     
  printf("get_for_current_user_only (0,1): %d\n",
         process_filter.get_for_current_user_only); 
  printf("            priority_flag (0,1): %d\n",
         process_filter.priority_flag);             
  printf("        command_line_flag (0,1): %d\n",
         process_filter.command_line_flag);         
  printf("  custom_proc_status_flag (0,1): %d\n",
         process_filter.cust_proc_status_flag);     
  printf("          login_name_flag (0,1): %d\n",
         process_filter.login_name_flag);           
  printf("          unix_owner_flag (0,1): %d\n",
         process_filter.unix_owner_flag);           
  printf("      progroup_title_flag (0,1): %d\n",
         process_filter.progroup_title_flag);       
  printf("     job_description_flag (0,1): %d\n",
         process_filter.job_descrip_flag);          
  printf("       original_host_flag (0,1): %d\n",
         process_filter.orig_host_flag);            
  printf("                       priority: %d\n",
         process_filter.priority);        
  printf("                   command_line: %s\n",
         process_filter.command_line);    
  printf("               cust_proc_status: %s\n",
         process_filter.cust_proc_status);
  printf("                     login_name: %s\n",
         process_filter.login_name);      
  printf("                     unix_owner: %s\n",
         process_filter.unix_owner);      
  printf("                 progroup_title: %s\n",
         process_filter.progroup_title);  
  printf("                    job_descrip: %s\n",
         process_filter.job_descrip);     
  printf("                      orig_host: %s\n", process_filter.orig_host);
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessList_print_results()

{
  int   i;

  printf("process count returned: %d\n", returned_process_count);
  for (i = 0; i < returned_process_count; i++)
    {
      printf("\nProcess %d\n", i + 1);
      printf("====================================================\n");
      printf("                  pid: <%d>\n", process_list[i].pid);
      printf("NTCSS long process_id: <%s>\n", process_list[i].process_id_str);
      printf("       (job) priority: <%d>\n", process_list[i].priority_code);
      printf("       progroup_title: <%s>\n", process_list[i].app_name_str);
      printf("         command_line: <%s>\n", process_list[i].command_line_str);
      printf("          proc_status: <%d>\n", process_list[i].status_code);
      printf("       cust_proc_stat: <%s>\n",
             process_list[i].custom_process_stat_str);
      printf("           login_name: <%s>\n", process_list[i].login_name_str);
      printf("           unix_owner: <%s>\n", process_list[i].unix_owner_str);
      printf("          job_descrip: <%s>\n", process_list[i].job_descrip_str);
      printf("         request_time: <%s>\n", process_list[i].request_time);
      printf("          launch_time: <%s>\n", process_list[i].launch_time);
      printf("             end_time: <%s>\n", process_list[i].end_time);
      printf("       security_class: <%d>\n", process_list[i].seclvl);
      printf("            orig_host: <%s>\n", process_list[i].orig_host_str);
      printf("             prt_name: <%s>\n", process_list[i].printer_name_str);
      printf("   printer's hostname: <%s>\n",
             process_list[i].printer_host_name_str);
      printf("        prt_sec_class: <%d>\n", process_list[i].print_seclvl);
      printf("         prt_filename: <%s>\n", process_list[i].prt_filename_str);
      printf(" bitmasked print_flag: <%d>\n", process_list[i].print_code);
      printf("          restartable: <%d>\n", process_list[i].restartable_flag);
      printf("         sdi_ctrl_rec: <%s>\n",
             process_list[i].sdi_control_record_id_str);
      printf("        sdi_data_file: <%s>\n", process_list[i].sdi_data_file_str);
      printf("         sdi_tci_file: <%s>\n", process_list[i].sdi_tci_file_str);
      printf("           sdi_device: <%s>\n",
             process_list[i].sdi_device_data_file_str);
      printf("ptr paper orientation: <%d>\n", process_list[i].orientation);
      printf("(on/off) print banner: <%d>\n", process_list[i].banner);
      printf("   num printed copies: <%d>\n", process_list[i].copies);
      printf("    printer papersize: <%d>\n", process_list[i].papersize);
    printf("\n");
  }
}

/*****************************************************************************/

static void  NtcssGetFilteredServerProcessListCt_test() 
{ 
 returned_process_count = NtcssGetFilteredServerProcessListCt(&process_filter);
 success = (returned_process_count != -1);
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessListCt_get_parameters() 
{
  NtcssGetFilteredServerProcessList_get_parameters();
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessListCt_print_parameters() 
{
  NtcssGetFilteredServerProcessList_print_parameters();
}


/*****************************************************************************/

static void  NtcssGetFilteredServerProcessListCt_print_results() 
{
  printf("process count returned: %d\n", returned_process_count);
}

/*****************************************************************************/

static void NtcssGetUserInfo_test() 
{ 
  clear_string(user_info.login_name);
  clear_string(user_info.real_first_name);
  clear_string(user_info.real_middle_name);
  clear_string(user_info.real_last_name);
  clear_string(user_info.ssn);
  clear_string(user_info.phone_number);
  user_info.security_class = 0;
  user_info.unix_id = 0;

  success = NtcssGetUserInfo(&user_info); 
}


/*****************************************************************************/

static void  NtcssGetUserInfo_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetUserInfo_print_parameters()
{
  return;
}


/*****************************************************************************/

static void  NtcssGetUserInfo_print_results()
{
  printf("      login_name: %.8s\n", user_info.login_name);
  printf(" real_first_name: %.32s\n", user_info.real_first_name);
  printf("real_middle_name: %.32s\n", user_info.real_middle_name);
  printf("  real_last_name: %.32s\n", user_info.real_last_name);
  printf("             ssn: %.11s\n", user_info.ssn);
  printf("    phone_number: %.32s\n", user_info.phone_number);
  printf("  security_class: %d\n", user_info.security_class);
  printf("         unix_id: %d\n", user_info.unix_id);
}


/*****************************************************************************/

static void  NtcssGetAppData_test() 

{ 
  clear_string(app_data_buffer);
  success = NtcssGetAppData(app_data_buffer, MAX_APP_DATA_LEN);
}


/*****************************************************************************/

static void  NtcssGetAppData_get_parameters()

{
  return;
}

/*****************************************************************************/

static void  NtcssGetAppData_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetAppData_print_results()

{ 
  printf("Application Data: %s\n", app_data_buffer); 
}


/*****************************************************************************/

static void  NtcssGetAppPassword_test()

{ 
  clear_string(app_password_buffer);
  success = NtcssGetAppPassword(app_password_buffer, MAX_APP_PASSWD_LEN); 
}


/*****************************************************************************/

static void  NtcssGetAppPassword_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetAppPassword_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetAppPassword_print_results() 

{ 
  printf("Password: %s\n", app_password_buffer); 
}


/*****************************************************************************/

static void  NtcssGetAppUserInfo_test()

{ 
  clear_string(user_info.login_name);
  clear_string(user_info.real_first_name);
  clear_string(user_info.real_middle_name);
  clear_string(user_info.real_last_name);
  clear_string(user_info.ssn);
  clear_string(user_info.phone_number);
  clear_string(prog_access);
  user_info.security_class = 0;
  user_info.unix_id = 0;

  success = NtcssGetAppUserInfo(user_name, &user_info, prog_access,
                                NTCSS_SIZE_APP_PROGRAM_INFO); 
}


/*****************************************************************************/

static void  NtcssGetAppUserInfo_get_parameters()

{
  strcpy(user_name, "tamburn");  /* Default */
 get_string("user name: ",user_name,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssGetAppUserInfo_print_parameters()

{
  printf("\nParameters...\n");
  printf("user name: %s\n", user_name);
}


/*****************************************************************************/

static void  NtcssGetAppUserInfo_print_results()

{
  printf("      login_name: %.8s\n", user_info.login_name);
  printf(" real_first_name: %.32s\n", user_info.real_first_name);
  printf("real_middle_name: %.32s\n", user_info.real_middle_name);
  printf("  real_last_name: %.32s\n", user_info.real_last_name);
  printf("             ssn: %.11s\n", user_info.ssn);
  printf("    phone_number: %.32s\n", user_info.phone_number);
  printf("  security_class: %d\n", user_info.security_class);
  printf("         unix_id: %d\n", user_info.unix_id);
  printf("          access: %s\n", prog_access);
  printf("      app_passwd: %s\n", user_info.app_passwd);
}


/*****************************************************************************/

static void  NtcssGetDateTime_test()

{ 
  clear_string(date_time_buffer);
  success = NtcssGetDateTime(date_time_buffer, NTCSS_SIZE_DATETIME1); 
}


/*****************************************************************************/

static void  NtcssGetDateTime_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetDateTime_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetDateTime_print_results()

{ 
  printf("%s\n", date_time_buffer); 
}


/*****************************************************************************/

static void  NtcssGetLocalIP_test()

{ 
  clear_string(ip_addr_buffer);
  success = NtcssGetLocalIP(ip_addr_buffer, MAX_IP_ADDRESS_LEN);
}


/*****************************************************************************/

static void  NtcssGetLocalIP_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetLocalIP_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetLocalIP_print_results()

{ 
  printf("IP: %s\n", ip_addr_buffer); 
}


/*****************************************************************************/

static void  NtcssGetHostName_test()

{ 
  clear_string(host_name);
  success = NtcssGetHostName(host_name, MAX_HOST_NAME_LEN); 
}

/*****************************************************************************/

static void  NtcssGetHostName_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetHostName_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetHostName_print_results()

{ 
  printf("Host Name: %s\n", host_name); 
}


/*****************************************************************************/

static void  NtcssGetServerName_test()

{ 
  clear_string(server_name);
  success = NtcssGetServerName(server_name, MAX_HOST_NAME_LEN); 
}


/*****************************************************************************/

static void  NtcssGetServerName_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetServerName_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetServerName_print_results()

{ 
  printf("Server Name: %s\n", server_name); 
}


/*****************************************************************************/

static void  NtcssStartServerProcessEx_test() 

{ 
  clear_string(pid);
  success = NtcssStartServerProcessEx(appname, &process_description,
                                      pid, MAX_PROCESS_ID_LEN); 
}


/*****************************************************************************/

static void  NtcssStartServerProcessEx_get_parameters()

{
  char   exec_file_name[MAX_PATH_LEN];

  sprintf(exec_file_name, "%s/%s/writenow", NTCSS_HOME_DIR, NTCSS_BIN_DIR);

  strcpy(process_description.owner_str, "tamburn");
  strcpy(process_description.prog_name_str, exec_file_name);
  process_description.prog_args_str[0]        = 0; 
  process_description.job_descrip_str[0]      = 0;
  process_description.seclvl                  = UNCLASSIFIED_CLEARANCE;
  process_description.priority_code           = LOW_PRIORITY;
  process_description.needs_approval_flag     = 0;
  process_description.needs_device_flag       = 0;
  process_description.restartable_flag        = 0;
  process_description.prt_output_flag         = 0;
  process_description.prt_filename_str[0]     = 0;
  process_description.output_type_str[0]      = 0;
  process_description.use_default_prt_flag    = 0;
  process_description.general_access_prt_flag = 0;
  process_description.save_prt_output_flag    = 0;
  process_description.sdi_required_flag       = 0;
  process_description.sdi_input_flag          = 0;
  strcpy(process_description.sdi_control_id_str, "");
  strcpy(process_description.sdi_data_file_str, "");
  strcpy(process_description.sdi_tci_file_str, "");
  process_description.banner                  = 0;
  process_description.orientation             = 0;
  process_description.copies                  = 0;
  process_description.papersize               = 0;

 get_string("                  app name: ", appname,NOT_SILENT);
 get_string("                     owner: ", process_description.owner_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("                   program: ",
             process_description.prog_name_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("                 arguments: ",
             process_description.prog_args_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("               description: ",
             process_description.job_descrip_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("security level (0,1,2,3,4): ", &(process_description.seclvl));
  if (let_the_rest_be_default)
    return;
  get_number("          priority (1,2,3): ",
             &(process_description.priority_code));
  if (let_the_rest_be_default)
    return;
  get_number("      needs approval (0,1): ",
             &(process_description.needs_approval_flag));
  if (let_the_rest_be_default)
    return;
  get_number("        needs device (0,1): ",
             &(process_description.needs_device_flag));
  if (let_the_rest_be_default)
    return;
  get_number("         restartable (0,1): ",
             &(process_description.restartable_flag));
  if (let_the_rest_be_default)
    return;
  get_number("      printer output (0,1): ",
             &(process_description.prt_output_flag));
  if (let_the_rest_be_default)
    return;

  if (process_description.prt_output_flag)
    {
     get_string("          printer filename: ",
                 process_description.prt_filename_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
     get_string("               output type: ",
                 process_description.output_type_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
      get_number(" use default printer (0,1): ",
                 &(process_description.use_default_prt_flag));
      if (let_the_rest_be_default)
        return;
      get_number("      general access (0,1): ",
                 &(process_description.general_access_prt_flag));
      if (let_the_rest_be_default)
        return;
      get_number(" save printer output (0,1): ",
                 &(process_description.save_prt_output_flag));
      if (let_the_rest_be_default)
        return;
    }

  get_number("   sdi_required_flag (0,1): ",
             &(process_description.sdi_required_flag));
  if (let_the_rest_be_default)
    return;
  if (process_description.sdi_required_flag)
    {
      get_number("      sdi_input_flag (0,1): ",
                 &(process_description.sdi_input_flag));
      if (let_the_rest_be_default)
        return;
     get_string("        sdi_control_id_str: ",
                 process_description.sdi_control_id_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
     get_string("         sdi_data_file_str: ",
                 process_description.sdi_data_file_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
     get_string("          sdi_tci_file_str: ",
                 process_description.prog_args_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
    }
}


/*****************************************************************************/

static void  NtcssStartServerProcessEx_print_parameters()

{
  printf("\nParameters...\n");
  printf("                  app name: %s\n", appname);
  printf("                     owner: %s\n", process_description.owner_str);
  printf("                   program: %s\n",
         process_description.prog_name_str);         
  printf("                 arguments: %s\n",
         process_description.prog_args_str);         
  printf("               description: %s\n",
         process_description.job_descrip_str);       
  printf("security level (0,1,2,3,4): %d\n", process_description.seclvl);
  printf("          priority (1,2,3): %d\n",
         process_description.priority_code);        
  printf("      needs approval (0,1): %d\n",
         process_description.needs_approval_flag);  
  printf("        needs device (0,1): %d\n",
         process_description.needs_device_flag);    
  printf("         restartable (0,1): %d\n",
         process_description.restartable_flag);     
  printf("      printer output (0,1): %d\n",
         process_description.prt_output_flag);      
  printf("          printer filename: %s\n",
         process_description.prt_filename_str);      
  printf("               output type: %s\n",
         process_description.output_type_str);       
  printf(" use default printer (0,1): %d\n",
         process_description.use_default_prt_flag);   
  printf("      general access (0,1): %d\n",
         process_description.general_access_prt_flag);
  printf(" save printer output (0,1): %d\n",
         process_description.save_prt_output_flag);   
  printf("         sdi_required_flag: %d\n",
         process_description.save_prt_output_flag);   
  printf("            sdi_input_flag: %d\n",
         process_description.save_prt_output_flag);   
  printf("        sdi_control_id_str: %s\n",
         process_description.owner_str);
  printf("         sdi_data_file_str: %s\n",
         process_description.owner_str);
  printf("          sdi_tci_file_str: %s\n",
         process_description.owner_str);
  printf("\n");
}


/*****************************************************************************/

static void  NtcssStartServerProcessEx_print_results()

{ 
  printf("pid: %s\n", pid); 
}


/*****************************************************************************/

static void  NtcssStartServerProcess_test()

{ 
  clear_string(pid);
  success = NtcssStartServerProcess(&process_description, pid,
                                    MAX_PROCESS_ID_LEN); 
}


/*****************************************************************************/

static void  NtcssStartServerProcess_get_parameters()

{
  char   exec_file_name[MAX_PATH_LEN];

  sprintf(exec_file_name, "%s/%s/writenow", NTCSS_HOME_DIR, NTCSS_BIN_DIR);

  strcpy(process_description.owner_str, "tamburn");
  strcpy(process_description.prog_name_str, exec_file_name);
  process_description.prog_args_str[0]        = 0; 
  process_description.job_descrip_str[0]      = 0;
  process_description.seclvl                  = UNCLASSIFIED_CLEARANCE;
  process_description.priority_code           = LOW_PRIORITY;
  process_description.needs_approval_flag     = 0;
  process_description.needs_device_flag       = 0;
  process_description.restartable_flag        = 0;
  process_description.prt_output_flag         = 0;
  process_description.prt_filename_str[0]     = 0;
  process_description.output_type_str[0]      = 0;
  process_description.use_default_prt_flag    = 0;
  process_description.general_access_prt_flag = 0;
  process_description.save_prt_output_flag    = 0;
  process_description.sdi_required_flag       = 0;
  process_description.sdi_input_flag          = 0;
  strcpy(process_description.sdi_control_id_str, "");
  strcpy(process_description.sdi_data_file_str, "");
  strcpy(process_description.sdi_tci_file_str, "");
  process_description.banner                  = 0;
  process_description.orientation             = 0;
  process_description.copies                  = 0;
  process_description.papersize               = 0;


 get_string("                     owner: ", process_description.owner_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("                   program: ",
             process_description.prog_name_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("                 arguments: ",
             process_description.prog_args_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("               description: ",
             process_description.job_descrip_str,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("security level (0,1,2,3,4): ",
             &(process_description.seclvl));
  if (let_the_rest_be_default)
    return;
  get_number("          priority (1,2,3): ",
             &(process_description.priority_code));
  if (let_the_rest_be_default)
    return;
  get_number("      needs approval (0,1): ",
             &(process_description.needs_approval_flag));
  if (let_the_rest_be_default)
    return;
  get_number("        needs device (0,1): ",
             &(process_description.needs_device_flag));
  if (let_the_rest_be_default)
    return;
  get_number("         restartable (0,1): ",
             &(process_description.restartable_flag));
  if (let_the_rest_be_default)
    return;
  get_number("      printer output (0,1): ",
             &(process_description.prt_output_flag));
  if (let_the_rest_be_default)
    return;

  if (process_description.prt_output_flag)
    {
     get_string("          printer filename: ",
                 process_description.prt_filename_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
     get_string("               output type: ",
                 process_description.output_type_str,NOT_SILENT);
      if (let_the_rest_be_default)
        return;
      get_number(" use default printer (0,1): ",
                 &(process_description.use_default_prt_flag));
      if (let_the_rest_be_default)
        return;
      get_number("      general access (0,1): ",
                 &(process_description.general_access_prt_flag));
      if (let_the_rest_be_default)
        return;
      get_number(" save printer output (0,1): ",
                 &(process_description.save_prt_output_flag));
      if (let_the_rest_be_default)
        return;
    }

  get_number("   sdi_required_flag (0,1): ",
             &(process_description.sdi_required_flag));
  if (let_the_rest_be_default)
    return;
  if (process_description.sdi_required_flag) {
    get_number("      sdi_input_flag (0,1): ",
               &(process_description.sdi_input_flag));
    if (let_the_rest_be_default)
      return;
   get_string("        sdi_control_id_str: ",
               process_description.sdi_control_id_str,NOT_SILENT);
    if (let_the_rest_be_default)
      return;
   get_string("         sdi_data_file_str: ",
               process_description.sdi_data_file_str,NOT_SILENT);
    if (let_the_rest_be_default)
      return;
   get_string("          sdi_tci_file_str: ",
               process_description.prog_args_str,NOT_SILENT);
    if (let_the_rest_be_default)
      return;
  }
}


/*****************************************************************************/

static void  NtcssStartServerProcess_print_parameters()

{
  printf("\nParameters...\n");
  printf("                     owner: %s\n", process_description.owner_str);
  printf("                   program: %s\n",
         process_description.prog_name_str);
  printf("                 arguments: %s\n",
         process_description.prog_args_str);
  printf("               description: %s\n",
         process_description.job_descrip_str);
  printf("security level (0,1,2,3,4): %d\n", process_description.seclvl);
  printf("          priority (1,2,3): %d\n",
         process_description.priority_code);        
  printf("      needs approval (0,1): %d\n",
         process_description.needs_approval_flag);  
  printf("        needs device (0,1): %d\n",
         process_description.needs_device_flag);    
  printf("         restartable (0,1): %d\n",
         process_description.restartable_flag);     
  printf("      printer output (0,1): %d\n",
         process_description.prt_output_flag);      
  printf("          printer filename: %s\n",
         process_description.prt_filename_str);      
  printf("               output type: %s\n",
         process_description.output_type_str);       
  printf(" use default printer (0,1): %d\n",
         process_description.use_default_prt_flag);   
  printf("      general access (0,1): %d\n",
         process_description.general_access_prt_flag);
  printf(" save printer output (0,1): %d\n",
         process_description.save_prt_output_flag);   
  printf("         sdi_required_flag: %d\n",
         process_description.save_prt_output_flag);   
  printf("            sdi_input_flag: %d\n",
         process_description.save_prt_output_flag);   
  printf("        sdi_control_id_str: %s\n", process_description.owner_str);
  printf("         sdi_data_file_str: %s\n", process_description.owner_str);
  printf("          sdi_tci_file_str: %s\n", process_description.owner_str);
  printf("\n");
}


/*****************************************************************************/

static void  NtcssStartServerProcess_print_results()

{ 
  printf("pid: %s\n", pid); 
}


/*****************************************************************************/

static void  NtcssStopServerProcess_test()

{ 
  success = NtcssStopServerProcess(local_pid, signal_type); 
}


/*****************************************************************************/

static void  NtcssStopServerProcess_get_parameters()

{
  strcpy(local_pid, pid);
  signal_type = 0;

 get_string("        pid: ",local_pid,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("signal type: ",&signal_type);
  if (let_the_rest_be_default)
    return;
}


/*****************************************************************************/

static void  NtcssStopServerProcess_print_parameters() 

{
  printf("\nParameters...\n");
  printf("        pid: %s\n", local_pid);
  printf("signal type: %d\n", signal_type);
}


/*****************************************************************************/

static void  NtcssStopServerProcess_print_results()

{
  return;
}


/*****************************************************************************/

static void  NtcssRemoveServerProcess_test()

{ 
  success = NtcssRemoveServerProcess(local_pid, scheduled_flag); 
}


/*****************************************************************************/

static void  NtcssRemoveServerProcess_get_parameters()

{
  strcpy(local_pid, pid);
  scheduled_flag = 0;
 get_string("           pid: ", local_pid,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("scheduled_flag: ", &scheduled_flag);
  if (let_the_rest_be_default)
    return;
}


/*****************************************************************************/

static void  NtcssRemoveServerProcess_print_parameters()

{
  printf("\nParameters...\n");
  printf("           pid: %s\n", local_pid);
  printf("scheduled_flag: %d\n", scheduled_flag);
}


/*****************************************************************************/

static void  NtcssRemoveServerProcess_print_results()

{
  return;
}


/*****************************************************************************/

static void  NtcssPrintServerFile_test()

{ 
  success = NtcssPrintServerFile(server_name, security_class, output_type,
                                 print_filename, number_of_copies, options);
}


/*****************************************************************************/

static void  NtcssPrintServerFile_get_parameters()

{
  int   x;

  x = gethostname(server_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      syslog(LOG_WARNING, "NtcssPrintServerFile_get_parameters: Failed to "
             "determine this  host's name!");
      return;
    }

  security_class = 0;
  strcpy(output_type, "HCN");
  sprintf(print_filename, "%s/%s/command_server.pid", NTCSS_HOME_DIR,
          NTCSS_LOGS_DIR);
  number_of_copies = 1;
  strcpy(options, "");

 get_string("     server name: ", server_name,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("  security class: ",&security_class);
  if (let_the_rest_be_default)
    return;
 get_string("     output type: ", output_type,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
 get_string("  print filename: ", print_filename,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
  get_number("number of copies: ",&number_of_copies);
  if (let_the_rest_be_default)
    return;
 get_string("         options: ", options,NOT_SILENT);
  if (let_the_rest_be_default)
    return;
}


/*****************************************************************************/

static void  NtcssPrintServerFile_print_parameters()

{
  printf("\nParameters...\n");
  printf("     server name: %s\n", server_name);   
  printf("  security class: %d\n", security_class);
  printf("     output type: %s\n", output_type);   
  printf("  print filename: %s\n", print_filename);
  printf("number of copies: %d\n", number_of_copies);
  printf("         options: %s\n", options);
}


/*****************************************************************************/

static void  NtcssPrintServerFile_print_results()

{
  return;
}


/*****************************************************************************/

static void  NtcssSetAppData_test()

{ 
  success = NtcssSetAppData(app_data); 
}


/*****************************************************************************/

static void  NtcssSetAppData_get_parameters()

{ 
  strcpy(app_data, "test");
 get_string("app data: ", app_data,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssSetAppData_print_parameters()

{
  printf("\nParameters...\n");
  printf("app data: %s", app_data);
}


/*****************************************************************************/

static void  NtcssSetAppData_print_results()

{
  return;
}


/*****************************************************************************/

/*
static void NtcssGetAppRole_test()

{
  return;
}
*/

/*****************************************************************************/

/*
static void NtcssGetAppRole_get_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetAppRole_print_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetAppRole_print_results()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetUserRole_test()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetUserRole_get_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetUserRole_print_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssGetUserRole_print_results()

{
  return;
}
*/


/*****************************************************************************/

static void  NtcssGetAppName_test()

{ 
  clear_string(application_name);
  success = NtcssGetAppName(application_name, MAX_PROGROUP_TITLE_LEN);
}


/*****************************************************************************/

static void  NtcssGetAppName_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetAppName_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetAppName_print_results() 

{ 
  printf("application name: %s", application_name); 
}


/*****************************************************************************/

/*
static void  NtcssSetCommonData_test()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssSetCommonData_get_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssSetCommonData_print_parameters()

{
  return;
}
*/


/*****************************************************************************/

/*
static void  NtcssSetCommonData_print_results()

{
  return;
}
*/


/*****************************************************************************/

static void  NtcssSetLinkData_test()

{ 
  success = NtcssSetLinkData(link_data); 
}


/*****************************************************************************/

static void  NtcssSetLinkData_get_parameters()

{
  strcpy(link_data, "link_data");
 get_string("link data: ", link_data,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssSetLinkData_print_parameters()

{ 
  printf("\nParameters...\n");
  printf("link data: %s", link_data);
}


/*****************************************************************************/

static void  NtcssSetLinkData_print_results()

{
  return;
}

/*****************************************************************************/

static void  NtcssGetLinkData_test()

{ 
  clear_string(link_data);
  success = NtcssGetLinkData(link_data,MAX_APP_PASSWD_LEN); 
}


/*****************************************************************************/

static void  NtcssGetLinkData_get_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetLinkData_print_parameters()

{
  return;
}


/*****************************************************************************/

static void  NtcssGetLinkData_print_results()

{ 
  printf("link data: %s", link_data); 
}


/*****************************************************************************/

static void  NtcssPostMessage_test() 

{ 
  success = NtcssPostMessage(message_board, message_title, message); 
}


/*****************************************************************************/

static void  NtcssPostMessage_get_parameters() 

{
 get_string("message board: ", message_board,NOT_SILENT);
 get_string("message title: ", message_title,NOT_SILENT);
 get_string("      message: ", message,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssPostMessage_print_parameters()

{
  printf("\nParameters...\n");
  printf("message board: %s\n", message_board);
  printf("message title: %s\n", message_title);
  printf("      message: %s\n", message);
}


/*****************************************************************************/

static void NtcssPostMessage_print_results()

{
  return;
}


/*****************************************************************************/

static void  NtcssPostSystemMessage_test()

{ 
  success = NtcssPostSystemMessage(message_title, message); 
}


/*****************************************************************************/

static void  NtcssPostSystemMessage_get_parameters()

{
 get_string("message title: ", message_title,NOT_SILENT);
 get_string("      message: ", message,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssPostSystemMessage_print_parameters()

{
  printf("\nParameters...\n");
  printf("message title: %s\n", message_title);
  printf("      message: %s\n", message);
}


/*****************************************************************************/

static void  NtcssPostSystemMessage_print_results()

{
  return;
}

/*****************************************************************************/

static void  NtcssPostAppMessage_test()

{ 
  success = NtcssPostAppMessage(message_title, message); 
}


/*****************************************************************************/

static void  NtcssPostAppMessage_get_parameters()

{
 get_string("message title: ", message_title,NOT_SILENT);
 get_string("      message: ", message,NOT_SILENT);
}


/*****************************************************************************/

static void  NtcssPostAppMessage_print_parameters()

{
  printf("\nParameters...\n");
  printf("message title: %s\n", message_title);
  printf("      message: %s\n", message);
}


/*****************************************************************************/

static void  NtcssPostAppMessage_print_results()

{
  return;
}

/*****************************************************************************/

static void  NtcssGetAppHost_test() 

{ 
  clear_string(host_name);
  success = NtcssGetAppHost(application_name, host_name,
                               MAX_HOST_NAME_LEN);
}


/*****************************************************************************/

static void  NtcssGetAppHost_get_parameters() 

{ 
 get_string("application: ", application_name,NOT_SILENT); 
}


/*****************************************************************************/

static void  NtcssGetAppHost_print_parameters() 

{ 
  printf("\nParameters...\n");
  printf("application: %s\n", application_name); 
}


/*****************************************************************************/

static void  NtcssGetAppHost_print_results() 

{
  printf("%s's host: %s\n", application_name,host_name); 
}

/*****************************************************************************/

static void  NtcssGetAuthServer_test() 

{ 
  clear_string(host_name);
  success = NtcssGetAuthServer(host_name);
}


/*****************************************************************************/

static void  NtcssGetAuthServer_get_parameters() 

{ 
  return;
}


/*****************************************************************************/

static void  NtcssGetAuthServer_print_parameters() 

{ 
  return;
}


/*****************************************************************************/

static void  NtcssGetAuthServer_print_results() 

{
  printf("Authentication Server: %s\n", host_name); 
}

/*****************************************************************************/

static void  NtcssGetAppDataServer_test() 

{ 
  clear_string(host_name);
  success = NtcssGetAppDataServer(application_name, host_name, 
                                                    MAX_HOST_NAME_LEN);
}


/*****************************************************************************/

static void  NtcssGetAppDataServer_get_parameters() 

{ 
 get_string("application: ", application_name,NOT_SILENT); 
}


/*****************************************************************************/

static void  NtcssGetAppDataServer_print_parameters() 

{ 
  printf("\nParameters...\n");
  printf("application: %s\n", application_name); 
}


/*****************************************************************************/

static void  NtcssGetAppDataServer_print_results() 

{
  printf("%s's Authentication host: %s\n", application_name,host_name); 
}

/*****************************************************************************/
static void  NtcssValidateUser_test()

{
  clear_string(ssn);
  success = NtcssValidateUser(user_name,password, ssn,
                                                    MAX_APP_PASSWD_LEN);
}
/*****************************************************************************/

static void  NtcssValidateUser_get_parameters()

{
 get_string("UserName: ", user_name,NOT_SILENT);
 get_string("Password: ", password,SILENT);
}

/*****************************************************************************/

static void  NtcssValidateUser_print_results() 

{
  printf("%s's SSN: %s\n", user_name,ssn); 
}

/*****************************************************************************/
static void  NtcssValidateUser_print_parameters() 

{ 
  printf("\nParameters...\n");
  printf("UserName: %s\n", user_name); 
  printf("Password: %s\n", password); 
}

/*****************************************************************************/

void  load_test( name, test, get_params, prt_params, prt_results )

     const char  *name;
     void  (*test)(void);
     void  (*get_params)(void);
     void  (*prt_params)(void);
     void  (*prt_results)(void);
{

  if ((test_data_count + 1) > MAX_NUMBER_OF_APIs)
    {
      printf("Error loading main test array\n");
      exit(1);
    }

  test_data_count++;
  api_tests[test_data_count].name             = name;
  api_tests[test_data_count].test             = test;
  api_tests[test_data_count].get_parameters   = get_params;
  api_tests[test_data_count].print_parameters = prt_params;
  api_tests[test_data_count].print_results    = prt_results;
}


/*****************************************************************************/

void  load_functions()

{
  load_test("Exit", nada, nada, nada, nada);

  load_test("NtcssGetAppData",
            NtcssGetAppData_test,
            NtcssGetAppData_get_parameters,
            NtcssGetAppData_print_parameters,
            NtcssGetAppData_print_results);
  
  load_test("NtcssGetAppName",
            NtcssGetAppName_test,
            NtcssGetAppName_get_parameters,
            NtcssGetAppName_print_parameters,
            NtcssGetAppName_print_results);
  
  load_test("NtcssGetAppPassword",
            NtcssGetAppPassword_test,
            NtcssGetAppPassword_get_parameters,
            NtcssGetAppPassword_print_parameters,
            NtcssGetAppPassword_print_results);

/*****************************************************
  load_test("NtcssGetAppRole",
        NtcssGetAppRole_test,
        NtcssGetAppRole_get_parameters,
        NtcssGetAppRole_print_parameters,
        NtcssGetAppRole_print_results);
*****************************************************/

  load_test("NtcssGetAppUserInfo",
            NtcssGetAppUserInfo_test,
            NtcssGetAppUserInfo_get_parameters,
            NtcssGetAppUserInfo_print_parameters,
            NtcssGetAppUserInfo_print_results);

  load_test("NtcssGetCommonData",
            NtcssGetCommonData_test,
            NtcssGetCommonData_get_parameters,
            NtcssGetCommonData_print_parameters,
            NtcssGetCommonData_print_results);

  load_test("NtcssGetDateTime",
            NtcssGetDateTime_test,
            NtcssGetDateTime_get_parameters,
            NtcssGetDateTime_print_parameters,
            NtcssGetDateTime_print_results);

  load_test("NtcssGetHostName",
            NtcssGetHostName_test,
            NtcssGetHostName_get_parameters,
            NtcssGetHostName_print_parameters,
            NtcssGetHostName_print_results);

  load_test("NtcssGetLinkData",
            NtcssGetLinkData_test,
            NtcssGetLinkData_get_parameters,
            NtcssGetLinkData_print_parameters,
            NtcssGetLinkData_print_results);

  load_test("NtcssGetLocalIP",
            NtcssGetLocalIP_test,
            NtcssGetLocalIP_get_parameters,
            NtcssGetLocalIP_print_parameters,
            NtcssGetLocalIP_print_results);

  load_test("NtcssGetServerName",
            NtcssGetServerName_test,
            NtcssGetServerName_get_parameters,
            NtcssGetServerName_print_parameters,
            NtcssGetServerName_print_results);

  load_test("NtcssGetUserInfo",
            NtcssGetUserInfo_test,
            NtcssGetUserInfo_get_parameters,
            NtcssGetUserInfo_print_parameters,
            NtcssGetUserInfo_print_results);

/************************************************
  load_test("NtcssGetUserRole",
        NtcssGetUserRole_test,
        NtcssGetUserRole_get_parameters,
        NtcssGetUserRole_print_parameters,
        NtcssGetUserRole_print_results);
************************************************/

  load_test("NtcssPostAppMessage",
            NtcssPostAppMessage_test,
            NtcssPostAppMessage_get_parameters,
            NtcssPostAppMessage_print_parameters,
            NtcssPostAppMessage_print_results);

  load_test("NtcssPostMessage",
            NtcssPostMessage_test,
            NtcssPostMessage_get_parameters,
            NtcssPostMessage_print_parameters,
            NtcssPostMessage_print_results);

  load_test("NtcssPostSystemMessage",
            NtcssPostSystemMessage_test,
            NtcssPostSystemMessage_get_parameters,
            NtcssPostSystemMessage_print_parameters,
            NtcssPostSystemMessage_print_results);

  load_test("NtcssPrintServerFile",
            NtcssPrintServerFile_test,
            NtcssPrintServerFile_get_parameters,
            NtcssPrintServerFile_print_parameters,
            NtcssPrintServerFile_print_results);

  load_test("NtcssSetAppData",
            NtcssSetAppData_test,
            NtcssSetAppData_get_parameters,
            NtcssSetAppData_print_parameters,
            NtcssSetAppData_print_results);

/********************************************************
  load_test("NtcssSetCommonData",
        NtcssSetCommonData_test,
        NtcssSetCommonData_get_parameters,
        NtcssSetCommonData_print_parameters,
        NtcssSetCommonData_print_results);
************************************************************/

  load_test("NtcssSetLinkData",
            NtcssSetLinkData_test,
            NtcssSetLinkData_get_parameters,
            NtcssSetLinkData_print_parameters,
            NtcssSetLinkData_print_results);
  
  load_test("NtcssStartServerProcess",
            NtcssStartServerProcess_test,
            NtcssStartServerProcess_get_parameters,
            NtcssStartServerProcess_print_parameters,
            NtcssStartServerProcess_print_results);

  load_test("NtcssStartServerProcessEx",
            NtcssStartServerProcessEx_test,
            NtcssStartServerProcessEx_get_parameters,
            NtcssStartServerProcessEx_print_parameters,
            NtcssStartServerProcessEx_print_results);

  load_test("NtcssStopServerProcess",
            NtcssStopServerProcess_test,
            NtcssStopServerProcess_get_parameters,
            NtcssStopServerProcess_print_parameters,
            NtcssStopServerProcess_print_results);

  load_test("NtcssGetFilteredServerProcessList",
            NtcssGetFilteredServerProcessList_test,
            NtcssGetFilteredServerProcessList_get_parameters,
            NtcssGetFilteredServerProcessList_print_parameters,
            NtcssGetFilteredServerProcessList_print_results);

  load_test("NtcssGetFilteredServerProcessListCt",
            NtcssGetFilteredServerProcessListCt_test,
            NtcssGetFilteredServerProcessListCt_get_parameters,
            NtcssGetFilteredServerProcessListCt_print_parameters,
            NtcssGetFilteredServerProcessListCt_print_results);

  load_test("NtcssRemoveServerProcess",
            NtcssRemoveServerProcess_test,
            NtcssRemoveServerProcess_get_parameters,
            NtcssRemoveServerProcess_print_parameters,
            NtcssRemoveServerProcess_print_results);

  load_test("NtcssGetAppHost",
            NtcssGetAppHost_test,
            NtcssGetAppHost_get_parameters,
            NtcssGetAppHost_print_parameters,
            NtcssGetAppHost_print_results);

  load_test("NtcssGetAuthServer",
            NtcssGetAuthServer_test,
            NtcssGetAuthServer_get_parameters,
            NtcssGetAuthServer_print_parameters,
            NtcssGetAuthServer_print_results); 

  load_test("NtcssGetAppDataServer",
            NtcssGetAppDataServer_test,
            NtcssGetAppDataServer_get_parameters,
            NtcssGetAppDataServer_print_parameters,
            NtcssGetAppDataServer_print_results);

  load_test("NtcssValidateUser",
            NtcssValidateUser_test,
            NtcssValidateUser_get_parameters,
            NtcssValidateUser_print_parameters,
            NtcssValidateUser_print_results);
}


/*****************************************************************************/

void  get_parameters()

{ 
  let_the_rest_be_default = False; 
  printf("%s\n", api_tests[api_number].name); 
  api_tests[api_number].get_parameters(); 
}


/*****************************************************************************/

void  print_parameters()

{ 
  api_tests[api_number].print_parameters(); 
  printf("\n");
}


/*****************************************************************************/

void  call_api()

{ 
  api_tests[api_number].test(); 
}


/*****************************************************************************/

void  print_results()

{ 
  if (success == 1)
    {
      printf("Success.\n");
      api_tests[api_number].print_results(); 
    }
  else
    print_error_message();
}


/*****************************************************************************/

void  print_menu()

{
  int   i;

  printf("\nInteractive Tests for the Ntcss Server APIs\n");
  printf("-------------------------------------------\n");
  
  for (i = 0; i <= test_data_count; i++) 
    printf("%2d. %s\n", i, api_tests[i].name);
}


/*****************************************************************************/

void  main_menu()

{
  int   testing = True;

  load_functions();

  while(testing)
    {
      print_menu();

      get_number("\nEnter Selection Number: ", &api_number);

      if (api_number == 0) 
        testing = 0;
      else
        if ((api_number >= 1) && (api_number <= test_data_count))
          {
            get_parameters();
            print_parameters();
            print_header();
            call_api();
            print_results();
            print_footer();
          }
        else
          {
            printf("\n\nNot a valid selection.  Press any key to continue...");
            waitkey();
            printf("\n\n");
          }
    }
}


/*****************************************************************************/

int  main(argc, argv)

     int   argc;
     char   *argv[];
{
  static char   env_string1[100];
  static char   env_string2[100];

  if (argc == 3)
    {
      sprintf(env_string1, "%s=%s", "CURRENT_NTCSS_PID", argv[1]);
      putenv(env_string1);
      sprintf(env_string2, "%s=%s", "CURRENT_NTCSS_GROUP", argv[2]);
      putenv(env_string2);
    }

  openlog("interactive_test_api", LOG_PID, LOG_LOCAL7);
  syslog(LOG_WARNING, "STARTING INTERACTIVE TEST PROGRAM");

  main_menu();

  syslog(LOG_WARNING, "ENDING INTERACTIVE TEST PROGRAM");

  return(0);
}


/*****************************************************************************/
