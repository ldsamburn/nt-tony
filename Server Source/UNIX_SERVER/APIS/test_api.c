/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
  test_api.c
*/

#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>

#include <ExtraProtos.h>
#include <LibNtcssFcts.h>

#include "NtcssApi.h"

FILE  *fp;
char  pid[MAX_PROCESS_ID_LEN];

void  test_NtcssGetUserInfo(void);
void  call_NtcssGetAppData(void);
void  call_NtcssSetAppData(const char*);
void  test_NtcssGetAppData(void);
void  test_NtcssGetAppPassword(void);
void  print_common_data(char*);
void  test_NtcssGetCommonData(void);
void  call_NtcssGetAppUserInfo(char*);
void  test_NtcssGetAppUserInfo(void);
void  test_NtcssGetDateTime(void);
void  test_NtcssGetLocalIP(void);
void  test_NtcssGetHostName(void);
void  test_NtcssGetServerName(void);
void  call_NtcssStartServerProcess(char*, char*);
void  test_NtcssStartServerProcess(void);
void  test_NtcssStopServerProcess(void);
void  test_NtcssPrintServerFile(void);
int   main(void);

/*****************************************************************************/

void   test_NtcssGetUserInfo()

{
  char  error_message[MAX_ERR_MSG_LEN];
  NtcssUserInfo  user_info;

  syslog(LOG_WARNING, "NtcssGetUserInfo");

  fprintf(fp, "\nTesting NtcssGetUserInfo...\n");

  if (NtcssGetUserInfo(&user_info))
    {
      fprintf(fp,"      login_name: %.8s\n", user_info.login_name);
      fprintf(fp," real_first_name: %.32s\n", user_info.real_first_name);
      fprintf(fp,"real_middle_name: %.32s\n", user_info.real_middle_name);
      fprintf(fp,"  real_last_name: %.32s\n", user_info.real_last_name);
      fprintf(fp,"             ssn: %.11s\n", user_info.ssn);
      fprintf(fp,"    phone_number: %.32s\n", user_info.phone_number);
      fprintf(fp,"  security_class: %d\n", user_info.security_class);
      fprintf(fp,"         unix_id: %d\n", user_info.unix_id);
  }
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");
  
  fflush(fp);
}


/*****************************************************************************/

void  call_NtcssGetAppData()

{
  char   app_data_buffer[MAX_APP_DATA_LEN];
  char   error_message[MAX_ERR_MSG_LEN];

  fprintf(fp, "\nTesting NtcssGetAppData...\n");

  if (NtcssGetAppData(app_data_buffer, MAX_APP_DATA_LEN)) 
    fprintf(fp, "app_data_buffer: %s\n", app_data_buffer);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  call_NtcssSetAppData(data)

     const char  *data;
{
  char    error_message[MAX_ERR_MSG_LEN];

  fprintf(fp, "\nTesting NtcssSetAppData...\n");

  if (NtcssSetAppData(data)) 
    fprintf(fp, "app_data_set\n");
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  test_NtcssGetAppData()

{

  syslog(LOG_WARNING, "NtcssSetAppData and NtcssGetAppData");

  call_NtcssSetAppData("XTESTX");
  call_NtcssGetAppData();
  call_NtcssSetAppData("GGGGOOOOODDDDDD");
  call_NtcssGetAppData();
}


/*****************************************************************************/

void  test_NtcssGetAppPassword()

{
  char   app_password_buffer[MAX_APP_PASSWD_LEN];
  char   error_message[MAX_ERR_MSG_LEN];
  int  x;

  syslog(LOG_WARNING, "NtcssGetAppPassword");

  fprintf(fp, "\nTesting NtcssGetAppPassword...\n");

  x = NtcssGetAppPassword(app_password_buffer, MAX_APP_PASSWD_LEN);
  if (x != 0)
    fprintf(fp, "app_password_buffer: %s\n", app_password_buffer);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  print_common_data(tag)

     char  *tag;
{
  char   data[MAX_COMDB_DATA_LEN];
  char   error_message[MAX_ERR_MSG_LEN];
  int  x;

  x = NtcssGetCommonData(tag, data, MAX_COMDB_DATA_LEN);
  if (x != 0)
    fprintf(fp, "%s: %s\n", tag, data);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "error on %s: %s\n", tag, error_message);
    else
      fprintf(fp, "error on %s and NTCSS error message not available\n", tag);

  fflush(fp);
}


/*****************************************************************************/

void  test_NtcssGetCommonData()

{
  char str1[] = "NTCSS HOME DIR";
  char str2[] = "BOOK";
  char str3[] = "NTCSS SYSLOG FILE";
  char str4[] = "MOVIE";
  char str5[] = "MYSHIP";
  char str6[] = "MYNAME";
  char str7[] = "SHIPS_NAME";
  char str8[] = "ERIC TAG";
  char str9[] = "COLOR";
  char str10[] = "NTCSS MSG BB PERIOD";
  char str11[] = "MYKEY";
  char str12[] = "NTCSS DB TIMESTAMP";
  char str13[] = "\t\t\t NTCSS HOME DIR ";
  char str14[] = " BOOK \t\t";
  char str15[] = "\t NTCSS SYSLOG FILE\t ";
  char str16[] = " MOVIE ";
  char str17[] = " MYSHIP ";
  char str18[] = " MYNAME ";
  char str19[] = " SHIPS_NAME ";
  char str20[] = " ERIC TAG ";
  char str21[] = " COLOR ";
  char str22[] = " NTCSS MSG BB PERIOD ";
  char str23[] = " MYKEY ";
  char str24[] = "!@#$&errorasdfa";
  char str25[] = "xxxyyyzzz";
  char str26[] = "321123";

  syslog(LOG_WARNING, "NtcssGetCommonData");

  fprintf(fp, "\nTesting NtcssGetCommonData...\n");

  print_common_data(str1);
  print_common_data(str2);
  print_common_data(str3);
  print_common_data(str4);
  print_common_data(str5);
  print_common_data(str6);
  print_common_data(str7);
  print_common_data(str8);
  print_common_data(str9);
  print_common_data(str10);
  print_common_data(str11);
  print_common_data(str12);

  print_common_data(str13);
  print_common_data(str14);
  print_common_data(str15);
  print_common_data(str16);
  print_common_data(str17);
  print_common_data(str18);
  print_common_data(str19);
  print_common_data(str20);
  print_common_data(str21);
  print_common_data(str22);
  print_common_data(str23);
  print_common_data(str24);
  print_common_data(str25);
  print_common_data(str26);

  fflush(fp);
}


/*****************************************************************************/

void  call_NtcssGetAppUserInfo(user_name)

     char  *user_name;
{
  char   error_message[MAX_ERR_MSG_LEN];
  char   prog_access[NTCSS_SIZE_APP_PROGRAM_INFO];
  NtcssUserInfo  user_info;

  fprintf(fp, "\nTesting NtcssGetAppUserInfo...\n");

  if (NtcssGetAppUserInfo(user_name, &user_info, prog_access,
                          NTCSS_SIZE_APP_PROGRAM_INFO))
    {
      fprintf(fp, "      login_name: %.8s\n", user_info.login_name);
      fprintf(fp, " real_first_name: %.32s\n", user_info.real_first_name);
      fprintf(fp, "real_middle_name: %.32s\n", user_info.real_middle_name);
      fprintf(fp, "  real_last_name: %.32s\n", user_info.real_last_name);
      fprintf(fp, "             ssn: %.11s\n", user_info.ssn);
      fprintf(fp, "    phone_number: %.32s\n", user_info.phone_number);
      fprintf(fp, "  security_class: %d\n", user_info.security_class);
      fprintf(fp, "         unix_id: %d\n", user_info.unix_id);
      fprintf(fp, "          access: %s\n", prog_access);
    }
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  test_NtcssGetAppUserInfo()

{
  char name1[] = "tamburn";
  char name2[] = "dbyers";
  char name3[] = "ckozoyed";
  char name4[] = "xcvxcvxcv";

  syslog(LOG_WARNING, "NtcssGetAppUserInfo");

  call_NtcssGetAppUserInfo(name1);
  call_NtcssGetAppUserInfo(name2);
  call_NtcssGetAppUserInfo(name3);
  call_NtcssGetAppUserInfo(name4);
}

/*****************************************************************************/

void  test_NtcssGetDateTime()

{
  char   date_time_buffer[NTCSS_SIZE_DATETIME1];
  char   error_message[MAX_ERR_MSG_LEN];
  int  x;

  syslog(LOG_WARNING, "NtcssGetDateTime");
  fprintf(fp, "Testing NtcssGetDateTime...\n");

   x = NtcssGetDateTime(date_time_buffer, NTCSS_SIZE_DATETIME1);
  if (x != 0)
    fprintf(fp, "%s\n", date_time_buffer);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}  


/*****************************************************************************/

void  test_NtcssGetLocalIP()

{
  char   error_message[MAX_ERR_MSG_LEN];
  char   ip_addr_buffer[MAX_IP_ADDRESS_LEN];
  int  x;

  syslog(LOG_WARNING, "NtcssGetLocalIP");
  fprintf(fp, "Testing NtcssGetLocalIP...\n");

  x = NtcssGetLocalIP(ip_addr_buffer, MAX_IP_ADDRESS_LEN);
  if (x != 0)
    fprintf(fp, "%s\n", ip_addr_buffer);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}  


/*****************************************************************************/

void  test_NtcssGetHostName()

{
  char   error_message[MAX_ERR_MSG_LEN];
  char   host_name[MAX_HOST_NAME_LEN];
  int  x;

  syslog(LOG_WARNING, "NtcssGetHostName");
  fprintf(fp, "Testing NtcssGetHostName...\n");

  x = NtcssGetHostName(host_name, MAX_HOST_NAME_LEN);
  if (x != 0)
    fprintf(fp, "%s\n", host_name);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  test_NtcssGetServerName()

{
  char   error_message[MAX_ERR_MSG_LEN];
  char   server_name[MAX_HOST_NAME_LEN];
  int   x;

  syslog(LOG_WARNING, "NtcssGetServerName");
  fprintf(fp, "Testing NtcssGetServerName...\n");

  x = NtcssGetServerName(server_name, MAX_HOST_NAME_LEN);
  if (x != 0)
    fprintf(fp, "%s\n", server_name);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  call_NtcssStartServerProcess(program_file, arguments)

     char  *program_file;
     char  *arguments;
{
  NtcssServerProcessDescr  process_description;
  char   error_message[MAX_ERR_MSG_LEN];
  int  x;

  strcpy(process_description.owner_str, "tamburn");
  strcpy(process_description.prog_name_str, program_file);
  strcpy(process_description.prog_args_str, arguments);
  strcpy(process_description.job_descrip_str, "ripv");
  process_description.seclvl = UNCLASSIFIED_CLEARANCE;
  process_description.priority_code = LOW_PRIORITY;
  process_description.needs_approval_flag = 1;
  process_description.needs_device_flag = 0;
  process_description.restartable_flag = 0;
  process_description.prt_output_flag = 0;
  strcpy(process_description.prt_filename_str, "");
  strcpy(process_description.output_type_str, "");
  process_description.use_default_prt_flag = 0;
  process_description.general_access_prt_flag = 0;
  process_description.save_prt_output_flag = 0;

  x = NtcssStartServerProcess(&process_description, pid, MAX_PROCESS_ID_LEN);
  if (x != 0)
    fprintf(fp, "Server Process Started: pid = %s\n", pid);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

void  test_NtcssStartServerProcess()

{
  char   exec_file_name[MAX_PATH_LEN];
  char   empty[] = "";

  syslog(LOG_WARNING, "NtcssStartServerProcess");
  fprintf(fp, "Testing NtcssStartServerProcess...\n");

  sprintf(exec_file_name, "%s/%s/writenow", NTCSS_HOME_DIR, NTCSS_BIN_DIR);
  call_NtcssStartServerProcess(exec_file_name, empty);
}


/*****************************************************************************/

void  test_NtcssStopServerProcess()

{
  char   error_message[MAX_ERR_MSG_LEN];
  char   exec_file_name[MAX_PATH_LEN];
  char   empty[] = "";

  syslog(LOG_WARNING, "NtcssStartStopServerProcess");
  fprintf(fp, "Testing NtcssStopServerProcess...\n");

  sprintf(exec_file_name, "%s/%s/ripv", NTCSS_HOME_DIR, NTCSS_BIN_DIR);
  call_NtcssStartServerProcess(exec_file_name, empty);
  sleep(5);

  if (NtcssStopServerProcess(pid, 0))
    fprintf(fp, "Server Process STOPPED: pid = %s", pid);
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");
}


/*****************************************************************************/

void  test_NtcssPrintServerFile()

{
  char  error_message[MAX_ERR_MSG_LEN];
  char  print_file_name[MAX_PATH_LEN];
  char  this_hosts_name[MAX_HOST_NAME_LEN + 1];
  int   x;
  char  empty[] = "";
  char  str_HCN[] = "HCN";

  syslog(LOG_WARNING, "NtcssPrintServerFile");
  fprintf(fp, "Testing NtcssGetServerName...\n");

  x = gethostname(this_hosts_name, (size_t) MAX_HOST_NAME_LEN);
  if (x != 0)
    {
      syslog(LOG_WARNING, "NtcssPrintServerFile: Failed to determine this "
             " host's name!");
      fprintf(fp, "Failed to determine this host's name!\n");
      return;
    }

  sprintf(print_file_name, "%s/%s/command_server.pid",
          NTCSS_HOME_DIR, NTCSS_LOGS_DIR);

  x = NtcssPrintServerFile(this_hosts_name, 0, str_HCN, print_file_name, 1, 
                           empty);
  if (x != 0)
    fprintf(fp, "PrintServerFile STARTED");
  else
    if (NtcssGetLastError(error_message, MAX_ERR_MSG_LEN))
      fprintf(fp, "%s\n", error_message);
    else
      fprintf(fp, "error and NTCSS error message not available\n");

  fflush(fp);
}


/*****************************************************************************/

int main()

{
  sleep(5);         /* give the server a chance to init this job */

  openlog("==|NTCSS API TEST|== ", LOG_PID, LOG_LOCAL7);
  syslog(LOG_WARNING, "BEGIN NTCSS SERVER API TEST");

  fp = fopen("test_api.txt", "w");

  test_NtcssGetCommonData();      
  test_NtcssGetUserInfo();        
  test_NtcssGetAppData();         
  test_NtcssGetAppPassword();     
  test_NtcssGetAppUserInfo();     
  test_NtcssGetDateTime();        
  test_NtcssGetLocalIP();         
  test_NtcssGetHostName();        
  test_NtcssGetServerName();      
  test_NtcssStartServerProcess(); 
  test_NtcssStopServerProcess();  
  test_NtcssPrintServerFile();   

  fclose(fp);

  syslog(LOG_WARNING, "END NTCSS SERVER API TEST");
  closelog();

  return(0);
}

/*****************************************************************************/
