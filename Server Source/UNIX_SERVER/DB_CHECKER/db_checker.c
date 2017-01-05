
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* 

   File Name   : db_checker.c
   Program     : NTCSS DATABASE VALIDATION PROGRAM
   Originator  : Chuck Kozoyed
   Date        : April 19, 1999

   Remarks 

   1. Database Validation Philosophy

      The main idea behind validating the NTCSS database in this
      program is to examine a table's relationship with other tables
      and to use predefined (and some newly defined) NTCSS functions
      to validate each of the fields in each record of a table.  Doing
      this for every table provides an overlapping (redundant) check
      of the database.  

   2. Program Architecture

      This program uses many static variables to store both database
      data and program control information.  If you know C++, it will
      be helpful to think of this program as a class (although if the
      C++ option would have been available, the architecture would
      have been considerably different).  For example, the static
      variables are like member variables and the static functions are
      like member functions.

      This program relies heavily on several macros which define
      functionality for most of the program.  An alternative would
      have been to use a struct to store control information such as
      primary table, related table, primary key, which print function
      to call, etc., and some "data stuffing routines" that put the
      data in the struct(s).  Finally, there would be functions which
      performs a generic validation (what the macros are doing now).

      I started down the road of the macro approach and got the bulk
      of everything working when I thought of this other cleaner
      approach.  I didn't think switching to the cleaner approach was
      justified since I had a short amount of time to do this and more
      functionality was still needed. The down side of the cleaner
      approach is that you would have to define a function for each
      primary key comparison.  If you want more information on this
      mailto:ckozoyed@ch.inri.com, or stop by and ask.

   3. Coding Conventions

      In general, I like my coding conventions, and I might not
      neccesarily like yours.  So don't change code just for the hell
      of it or because you like to add a lot of unneccessary space
      which makes the program difficult to understand because you can
      only see a little bit of it.  If you want to make a change then
      change only the part you are working on, but not all of the code
      to suit your tastes (unless you have been tasked to make code
      more readable or it is -so- repulsive that you can't understand
      it).  This comment is in response to this happening in the past
      by personnel on the NTCSS team.

   4. Command Line Arguments

      There are many command line options that are available to the
      operator (24 at the time of this writing).  These arguments turn
      on/off variables in the program which control particular
      behaviors.  Run the program with no arguments to get the usage
      output and an explanation of these arguments.

   5. Reading the database

      The database is read using functions defined in ManipData.c
      except for two which are defined in this program.  These
      functions are getPrinterAccessData() and getOutputPrinterData()
      which were not available elsewhere but are coded very similarly
      (in this program) as the ones in ManipData.c.  The database is
      read all at one time at the beginning of the program into static
      (member) variables.  The program accesses the database from this
      data that is in memory.  The memory is freed after the database
      is validated.  See the function ReadDatabase() to see how the
      database is read.

   6. Checking a table

      This program reports problems in fields, n to 1 relationships,
      and duplicate primary keys.  It allows the operator to visually
      inspect the data for additional validation.  Whenever a record
      is printed, the fields are checked by the field validation
      routines for that table.  So whenever a field is displayed, any
      problems are always reported (unless the field check switch is
      turned off -- see command line arguments).

      NOTE: As of this writing, some of the field validation routines
      defined in this program don't do any validation.  That is
      because the validation to perform on those fields have not been
      communicated to me yet.  Once the validation to be performed on
      these fields is determined, it is simple to add the code into
      the corresponding field validation routine.

      There are two types of table relationships (as seen in this program):

      a. N to 1 - one or more records relate to exactly one record.
      b. 1 to N - one record relates to N records (N is between zero 
                  and some maximum determined by system limitations).

      NOTE: Obtain the NTCSS Entity Relationship diagram to see the
      relationships in the NTCSS database.

      In the following diagram,

      +---------+ N    1 +---------+
      | TABLE A |--------| TABLE B |
      |---------|---+    |---------|
      | FIELD 1 | 1 |    | FIELD 1 |
      | FIELD 2 |   |    | FIELD 2 |
      | FIELD 3 |   |    | FIELD 3 |
      | FIELD 4 |   |    | FIELD 4 |
      +---------+   |    +---------+
                    |
                    |    +---------+ N    1 +---------+
                    +----| TABLE C |--------| TABLE D |
                       N |---------|        |---------|
                         | FIELD 1 |        | FIELD 1 |
                         | FIELD 2 |        | FIELD 2 |
                         | FIELD 3 |        | FIELD 3 |
                         | FIELD 4 |        | FIELD 4 |
                         +---------+        +---------+


      table A is related to B and C.  The validation routine would do
      the following:

      For each record in A
        - Print current record in A.
        - Print each related record in B (should be exactly one).
        - Print each related record in C (zero or more).
          
      Find any duplicate records in A.

      Table B is related to A.  The validation routine for table B
      does the following:

      For each record in B
        - Print current record in B.
        - Print each related record in A (zero or more).

      Find any duplicate records in B.

      Table C is related to A and D.  The validation routine does the
      following:

      For each record in C
        - Print current record in C.
        - Print each related record in A (should be exactly one).
        - Print each related record in D (zero or more).

      and so on for each table.  See the code for more details.

*/

#include <stdio.h>
#include <unistd.h>
#ifdef SOLARIS
#include "/usr/include/limits.h"
#else
#include <limits.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fnmatch.h>

#include <Ntcss.h>
#include <LibNtcssFcts.h>
#include <StringFcts.h>
#include <TextDBP.h>

/* Record Printing Functions */

static void PrintPrinterAccessRecord(int, int, int);
static void PrintOutputPrinterRecord(int, int, int);
static void PrintPrinterClass(int, int, int);
static void PrintPrinterAdapter(int, int, int);
static void PrintCurrentApp(int, int, int);
static void PrintHost(int, int, int);
static void PrintPdj(int, int, int);
static void PrintOutputType(int, int, int);
static void PrintAppuser(int, int, int);
static void PrintAccessRole(int, int, int);
static void PrintProgram(int, int, int);
static void PrintProgroup(int, int, int);
static void PrintPrinterAccessRecord(int, int, int);
static void PrintUser(int, int, int);
static void PrintPrinter(int, int, int);

/* Table Checking Functions */

static void CheckHosts(void);
static void CheckProgroups(void);
static void CheckPrinters(void);
static void CheckOutputTypes(void);
static void CheckPrinterAdapters(void);
static void CheckUsers(void);
static void CheckOutputPrinterRecords(void);
static void CheckAppUsers(void);
static void CheckAccessRoles(void);
static void CheckCurrentApps(void);
static void CheckPrograms(void);
static void CheckPdjs(void);
static void CheckPrinterAccessRecords(void);
static void CheckPrinterClasses(void);

/* Table Reading and Freeing Functions */

static void getPrinterAccessData(char*);
static void getOutputPrinterData(char*);
static void freeOutputPrinterData(void);
static void freePrinterAccessData(void);
static void ReadDatabase(void);
static void FreeMemory(void);

/* Additional Functions */

static void Quit(int);
static void Message(const char*);
static void Tab(void);
static void NoTab(void);
static void PutInt(int, int);
static void PutString(const char*, int);
static void PutTab(void);
static void Newline(void);
static void Underline(int);
static void SetPrimaryKey(char*);
static void AddToPrimaryKey(char*);
static void SetCheckedPrimaryKey(char*);
static void AddToCheckedPrimaryKey(char*);
static void SetCheckedTable(const char*);
static void Usage(char*);
int main (int, char**);



/* Field Checking Functions for fields that didn't already have a function defined in NTCSS */

static int CheckLoginName(char*, char*);
static int CheckNumUsers(int, char*);
static int CheckAppLock(int, char*);
static int CheckAdapterType(char*, char*);
static int CheckNumProcesses(int, char*);
static int CheckHostType(int, char*);
static int CheckHostReplication(int, char*);
static int CheckAccessRoleNumber(int, char*);
static int CheckScheduleString(char*, char*);
static int CheckPdjFlag(int, char*);
static int CheckBatchFlag(int, char*);
static int CheckAppData(char*, char*);
static int CheckAppPasswd(char*, char*);
static int CheckAppRole(int, char*);
static int CheckRegisteredUser(int, char*);
static int CheckAccessRole(char*, char*);
static int CheckSecurityClass(int, char*);
static int CheckProgramAccess(int, char*);
static int CheckIconIndex(int, char*);
static int CheckProgramFlag(int, char*);
static int CheckReleaseDate(char*, char*);
static int CheckLinkData(char*, char*);
static int CheckPwChangeTime(char*, char*);
static int CheckUserRole(int, char*);
static int CheckUserLock(int, char*);
static int CheckSharedPasswd(char*, char*);
static int CheckAuthServer(char*, char*);
static int CheckMaxsize(int, char*);
static int CheckStatus(int, char*);
static int CheckPortName(char*, char*);
static int CheckSuspendFlag(int, char*);
static int CheckFileName(char*, char*);

/* The following structs are not defined in Ntcss.h */

typedef struct  _PrinterAccessItem {
  char *progroup_title;
  char *prt_name;
  char *hostname;
} PrinterAccessRecord;

typedef struct _OutputPrinterItem {
  char *out_type_title;
  char *progroup_title;
  char *prt_name;
  char *hostname;
} OutputPrinterItem;

static char db_dir[256];                    /* directory of the NTCSS database  */
/*static FILE *fp = stdout;                output sent to stdout */

static char tab[] = "";                  /* controls how far from left side data is printed */
static char error_message[200];         /* what do you think this might be for ? */
static char buffer[300];                /* generic buffer */
static int primary_index;               /* index for table being validated */
static int related_index;               /* index for related table */ 
static int duplicate_count = 0;         /* count of duplicate records */
static int related_count = 0;           /* count of related records */
static char primary_key[300];           /* stores the primary key in ready to print format (not for comparisons) */
static char checked_primary_key[300];   /* stores the primary key of the table being field checked in ready to print format */
static char checked_table[50];          /* the title of the table whose fields are to be checked for display  */
static int opened_system_settings = 0;  /* determines whether shared memory was opened by this program */
static unsigned short int port = 13;    /* port to use when testing connecting to hosts */

static int print_data = 1;              /* determines whether database data is to be printed */
static int print_errors = 1;            /* determines whether errors are is to be printed */
static int check_many = 1;              /* determines whether 1 to N relationships are to be looked at */
static int check_many_overide = 0;      /* used by the macro which checks N to 1 relationships to force checking for related records */
static int check_one = 1;               /* determines whether N to 1 relations are to be checked */
static int check_duplicates = 1;        /* determines whether duplicates are to be checked */
static int check_fields = 1;            /* determines whether field values are to be checked */
static int check_connection = 1;        /* determines whether to test that a tcp connection is able to be established to a host */
static int check_something = 0;         /* determines whether at least one table was turned on by command line arg */
static int check_all = 0;               /* determines whether all tables are to be checked */
static int check_progroups = 0;         /* determines whether the progroups table is to be checked */
static int check_users = 0;             /* determines whether the users table is to be checked */
static int check_appusers = 0;          /* determines whether the appusers table is to be checked */
static int check_access_roles = 0;      /* determines whether the access_roles table is to be checked */
static int check_current_apps = 0;      /* determines whether the current_apps table is to be checked */ 
static int check_programs = 0;          /* determines whether the programs table is to be checked */
static int check_pdjs = 0;              /* you get the idea */
static int check_hosts = 0;
static int check_printer_access_records = 0;
static int check_printer_adapters = 0;
static int check_printer_classes = 0;
static int check_output_printer_records = 0;
static int check_printers = 0;
static int check_output_types = 0;

/* NTCSS Database Data */

static AppItem              **progroups;
static PredefinedJobItem    **pdjs;
static HostItem             **hosts;
static PrinterAccessRecord  **printer_access_records;
static OutputTypeItem       **output_types;
static AppuserItem          **appusers;
static ProgramItem          **programs;
static AccessRoleItem       **access_roles;
static UserItem             **users;
static AppItem              **current_apps;
static PrtItem              **printers;
static AdapterItem          **adapters;
static PrtclassItem         **printer_classes;
static OutputPrinterItem    **output_printer_records;

/* Total Table Record Counts */

static int progroup_count               = 0;
static int pdj_count                    = 0;
static int host_count                   = 0;
static int printer_access_record_count  = 0;
static int output_type_count            = 0;
static int appuser_count                = 0;
static int program_count                = 0;
static int access_role_count            = 0;
static int user_count                   = 0;
static int current_app_count            = 0;
static int printer_count                = 0;
static int adapter_count                = 0;
static int printer_class_count          = 0;
static int output_printer_record_count  = 0;

/* Two types of output formats */

#define FULL_FORMAT 1  /* one field per line */
#define LIST_FORMAT 0  /* one record per line */

/* syntatic sugar for checking command line args */
#define CHECK_ARG(arg,stmt) if (strstr(argv[i],arg) != NULL) {stmt;}

/* syntatic sugar for calling table checking functions based on command line args */
#define CHECK_TABLE(check_flag, checkfunction) if ((check_flag) || (check_all)) { checkfunction;} 

/* generic duplicate primary key check */
#define CHECK_FOR_DUPLICATES(table, record_count, duplicate_expression) { \
  if (check_duplicates) { \
    duplicate_count = 0; \
    if (print_data) fprintf(stdout,"\nCHECKING FOR DUPLICATE PRIMARY KEYS IN %s\n",table); \
    for (primary_index = 0; primary_index < (record_count - 1); primary_index++) { \
      for (related_index = primary_index + 1; related_index < record_count; related_index++) { \
        if (duplicate_expression) { \
          if (print_errors) fprintf(stdout,"*** ERROR IN THE %s TABLE *** DUPLICATE PRIMARY KEY IN RECORD NUMBER %d AND %d.\n",table,primary_index+1,related_index+1); \
          duplicate_count++; \
        } \
      } \
    } \
    if (print_data) fprintf(stdout,"TOTAL NUMBER OF DUPLICATES: %d\n",duplicate_count); \
  } \
}

/* generic related record check */
#define CHECK_RELATED_RECORDS(title, record_count, related_expression, print_function, format) { \
  if (check_many || check_many_overide) { \
    related_count = 0; \
    for (related_index = 0; related_index < record_count; related_index++) { \
      if (related_expression) { \
        related_count++; \
        if (related_count == 1) { \
          sprintf(buffer, "%s RELATED TO %s", title, primary_key); \
          if (print_data) fprintf(stdout,"\n%s%s\n",tab,buffer); \
          memset(buffer,'-',strlen(buffer)); \
          if (print_data) fprintf(stdout,"%s%s\n",tab,buffer); \
        } \
        print_function(related_index, format, (related_count == 1)); \
      } \
    } \
    if (related_count == 0) { \
      sprintf(buffer, "NO %s RELATED TO %s", title, primary_key); \
      if (print_data) fprintf(stdout, "\n%s%s\n", tab, buffer); \
    } else { \
      sprintf(buffer, "TOTAL NUMBER OF %s RELATED TO %s: %d", title, primary_key, related_count); \
      if (print_data) fprintf(stdout, "\n%s%s\n", tab, buffer); \
    } \
  } \
}

/* generic check for one related record */
#define CHECK_FOR_ONE_RELATED_RECORD(table1, table2, title, record_count, related, print_function) { \
  if (check_one) { \
    check_many_overide = 1; \
    CHECK_RELATED_RECORDS(title, record_count, related, print_function, FULL_FORMAT); \
    check_many_overide = 0; \
    if (related_count != 1) { \
      if (print_errors) fprintf(stdout,"%s*** RELATION ERROR *** EACH %s SHOULD HAVE -EXACTLY ONE- RELATED %s RECORD (%s)\n", tab, table1, table2, primary_key); \
    } \
  } \
}

/* check a field with format 1 of check_field_function */
#define CHECK_FIELD(check_field_function, field_data) { \
  if (check_fields) { \
    if (!check_field_function(field_data, error_message)) { \
      if (print_errors) { \
        fprintf(stdout,"%s*** FIELD ERROR *** (TABLE: %s, KEY: %s) %s\n",tab, checked_table, checked_primary_key, error_message); \
      } \
    } \
  } \
}

/* check a field with format 2 of check_field function */
#define CHECK_FLD(check_field_function, data, name) { \
  if (check_fields) { \
    if (!check_field_function(data)) { \
      if (print_errors) { \
        fprintf(stdout,"%s*** FIELD ERROR *** (TABLE: %s, KEY: %s) ERROR IN %s\n", tab, checked_table, checked_primary_key, name); \
      } \
    } \
  } \
}

static int CheckLoginName(login_name, msg)
     char *login_name;
     char *msg;
{
  if (getpwnam(login_name) == NULL) {
    sprintf(msg,"%s is not a valid login name.",login_name);
    return 0;
  }

  return 1;
}

static int CheckNumUsers(num_users, msg)
     int num_users;
     char *msg;
{
  if ((num_users < 0) || (num_users > MAX_NUM_NTCSS_USERS)) {
    sprintf(msg,"invalid number of users (%d).",num_users);
    return 0;
  }

  return 1;
}

static int CheckAppLock(app_lock,msg)
     int app_lock;
     char *msg;
{
  if ((app_lock != 0) && (app_lock != 1)) {
    sprintf(msg,"invalid app lock (%d).",app_lock);
    return 0;
  }

  return 1;
}

static int CheckAdapterType(adapter_type,msg)
     char *adapter_type;
     char *msg;
{  
  /* done to get rid of compiler warning about unused parameters */
  adapter_type=adapter_type;
  msg=msg;

  return 1;
}

static int CheckNumProcesses(num_processes, msg)
     int num_processes;
     char *msg;
{
  if ((num_processes < 0) || (num_processes > INT_MAX)) {
    sprintf(msg,"invalid number of processes (%d).",num_processes);  
    return 0;
  }

  return 1;
}

static int CheckHostType(type, msg)
     int type;
     char *msg;
{
  if ((type != NTCSS_MASTER_SERVER_TYPE) && (type != NTCSS_APP_SERVER_TYPE) && (type != NTCSS_AUTH_SERVER_TYPE)) {
    sprintf(msg,"invalid host type (%d).",type);
    return 0;
  }

  return 1;
}

static int CheckHostReplication(replication, msg)
     int replication;
     char *msg;
{
  if ((replication != 0) && (replication != 1)) {
    sprintf(msg,"invalid replication (%d).",replication);
    return 0;
  }

  return 1;
}

static int CheckAccessRoleNumber(access_role_number, msg)
     int access_role_number;
     char* msg;
{
  /* done to get rid of compiler warning about unused parameters */
  access_role_number=access_role_number;
  msg=msg;

  /* can't check without progroup - checked in the table's Check function */
  return 1;
}

static int CheckScheduleString(schedule_string, msg)
     char *schedule_string;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  schedule_string=schedule_string;
  msg=msg;

  return 1;
}

static int CheckPdjFlag(flag, msg)
     int flag;
     char *msg;
{
  if ((flag != 0) && (flag != 1) && (flag != 2)) {
    sprintf(msg,"invalid pdj flag (%d).",flag);
    return 0;
  }

  return 1;
}

static int CheckBatchFlag(batch_flag, msg)
     int batch_flag;
     char *msg;
{
  if ((batch_flag != 0) && (batch_flag != 1)) {
    sprintf(msg,"*** FIELD ERROR *** invalid output type batch flag (%d).",batch_flag);
    return 0;
  }

  return 1;
}

static int CheckAppData(app_data, msg)
     char *app_data;
     char *msg;
{
  if (strlen(app_data) > MAX_APP_DATA_LEN) {
    sprintf(msg,"*** FIELD ERROR *** application data too long.");
    return 0;
  }

  return 1;
}

static int CheckAppPasswd(app_passwd, msg)
     char *app_passwd;
     char *msg;
{
  if (strlen(app_passwd) > MAX_APP_PASSWD_LEN) {
    sprintf(msg,"*** FIELD ERROR *** application password too long.");
    return 0;
  }

  return 1;
}

static int CheckAppRole(app_role, msg)
     int app_role;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  app_role=app_role;
  msg=msg;

  /* can't check without progroup - checked in the table's Check function */
  return 1;
}

static int CheckRegisteredUser(registered_user, msg)
     int registered_user;
     char *msg;
{
  if ((registered_user != 0) && (registered_user != 1)) {
    sprintf(msg,"invalid appuser registered user value (%d).",registered_user);
    return 0;
  }

  return 1;
}

static int CheckAccessRole(access_role, msg)
     char *access_role;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  access_role=access_role;
  msg=msg;

  /* access_roles.access_role : can bounce it against the progrps.ini
    file.  It should be in sequence with the role definitions. 
    Maybe add role number to bounce both off progrps.ini */

  return 1;
}

static int CheckSecurityClass(security_class, msg)
     int security_class;
     char *msg;
{
  if ((security_class != UNCLASSIFIED_CLEARANCE) && (security_class != UNCLASSIFIED_SENSITIVE_CLEARANCE) && (security_class != CONFIDENTIAL_CLEARANCE) && (security_class != SECRET_CLEARANCE) && (security_class != TOPSECRET_CLEARANCE)) {
    sprintf(msg,"invalid security class (%d).",security_class);
    return 0;
  }

  return 1;
}

static int CheckProgramAccess(program_access, msg)
     int program_access;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  program_access=program_access;
  msg=msg;

  /* should be validated with program group */
  return 1;
}

static int CheckIconIndex(icon_index, msg)
     int icon_index;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  icon_index=icon_index;
  msg=msg;

  return 1;
}

static int CheckProgramFlag(program_flag, msg)
     int program_flag;
     char *msg;
{
  if ((program_flag < 1) || (program_flag > 5)) {
    sprintf(msg,"invalid program flag (%d).",program_flag);
    return 0;
  }

  return 1;
}

static int CheckReleaseDate(release_date, msg)
     char *release_date;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  release_date=release_date;
  msg=msg;

  return 1;
}

static int CheckLinkData(link_data, msg)
     char *link_data;
     char *msg;
{
  if (strlen(link_data) > MAX_APP_PASSWD_LEN) {
    sprintf(msg,"invalid link data.");
    return 0;
  }
  
  return 1;
}

static int CheckPwChangeTime(pw_change_time, msg)
     char *pw_change_time;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  pw_change_time=pw_change_time;
  msg=msg;

  return 1;
}

static int CheckUserRole(user_role, msg)
     int user_role;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  user_role=user_role;
  msg=msg;

  /* can't check without progroup - checked in the table's Check function */
  return 1;
}

static int CheckUserLock(user_lock, msg)
     int user_lock;
     char *msg;
{

  if ((user_lock < 0) || (user_lock > 1)) {
    sprintf(msg,"invalid user lock value (%d).",user_lock);
    return 0;
  }

  return 1;
}

static int CheckSharedPasswd(shared_passwd, msg)
     char *shared_passwd;
     char *msg;
{
  if (strlen(shared_passwd) > MAX_APP_PASSWD_LEN) {
    sprintf(msg,"invalid shared passwd.");
    return 0;
  }
  
  return 1;
}

static int CheckAuthServer(auth_server, msg)
     char *auth_server;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  auth_server=auth_server;
  msg=msg;

  /* Checked in the CheckUser table function */

  return 1;
}

static int CheckMaxsize(max_size, msg)
     int max_size;
     char *msg;
{
  if (max_size < 1) {
    sprintf(msg,"invalid maxsize (%d).",max_size);
    return 0;
  }

  return 1;
}

static int CheckStatus(status, msg)
     int status;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  status=status;
  msg=msg;

  return 1;
}

static int CheckPortName(port_name, msg)
     char *port_name;
     char *msg;
{
  /* done to get rid of compiler warning about unused parameters */
  port_name=port_name;
  msg=msg;

  return 1;
}

static int CheckSuspendFlag(suspend_flag, msg)
     int suspend_flag;
     char *msg;
{
  if ((suspend_flag != 0) && (suspend_flag != 1)) {
    sprintf(msg,"invalid suspend flag (%d).",suspend_flag);
    return 0;
  }

  return 1;
}

static int CheckFileName(FileName, msg)
     char *FileName;
     char *msg;
{
  if (fnmatch("*", FileName, 0) != 0) {
    sprintf(msg,"invalid filename (%s).", FileName);
    return 0;
  }

  return 1;
}


static void Quit(exit_code) 
  int exit_code;
{
  if (opened_system_settings) {
    if (freeNtcssSystemSettingsShm() != 0) {
      printf("Error closing system settings.\n");
      exit(1);
    }
  }

  exit(exit_code);
}


static void getPrinterAccessData(dir)
     char *dir;
{
  int max_printer_access_records = MAX_NUM_PRINTERS * MAX_NUM_PROGROUPS;
  char filename[MAX_FILENAME_LEN];
  char decrypt_file[MAX_FILENAME_LEN];
  DBRecord record;
  FILE *printer_access_fp;

  printer_access_record_count = -1;
  printer_access_records = (PrinterAccessRecord **) malloc(max_printer_access_records * sizeof(char *));

  if (printer_access_records == NULL) {
    return;
  }

  sprintf(filename, "%s/%s", dir, "printer_access");
  printer_access_fp = decryptAndOpen(filename, decrypt_file, "r");

  if (printer_access_fp == NULL) {
    free(printer_access_records);
    printer_access_records = NULL;
    remove(decrypt_file);
    return;
  }

  printer_access_record_count = 0;
  while (getRecord(&record, printer_access_fp, PRINTER_ACCESS_NUM_RECS) && (printer_access_record_count < max_printer_access_records)) {
    printer_access_records[printer_access_record_count] = (PrinterAccessRecord *) malloc(sizeof(PrinterAccessRecord));
    printer_access_records[printer_access_record_count]->prt_name = allocAndCopyString(record.field[0].data);
    printer_access_records[printer_access_record_count]->hostname = allocAndCopyString(record.field[1].data); 
    printer_access_records[printer_access_record_count]->progroup_title = allocAndCopyString(record.field[2].data);
    printer_access_record_count++;
  }

  fclose(printer_access_fp);
  remove(decrypt_file);
}

static void freePrinterAccessData()
{
  int  k;

  for (k=0; k < printer_access_record_count; k++) {
      free(printer_access_records[k]->progroup_title);
      free(printer_access_records[k]->hostname);
      free(printer_access_records[k]->prt_name);
      free(printer_access_records[k]);
  }
  free(printer_access_records);
}

static void getOutputPrinterData(dir)
     char *dir;
{
  int max_output_printer_records = MAX_NUM_PRINTERS * MAX_NUM_OUTPUT_TYPE;
  char filename[MAX_FILENAME_LEN];
  char decrypt_file[MAX_FILENAME_LEN];
  DBRecord record;
  FILE *output_printer_fp;

  output_printer_record_count = -1;
  output_printer_records = (OutputPrinterItem **) malloc(max_output_printer_records * sizeof(char *));

  if (printer_access_records == NULL) {
    return;
  }

  sprintf(filename, "%s/%s", dir, "output_prt");
  output_printer_fp = decryptAndOpen(filename, decrypt_file, "r");

  if (printer_access_records == NULL) {
    remove(decrypt_file);
    free(output_printer_records);
    output_printer_records = NULL;
    return;
  }

  output_printer_record_count = 0;
  while (getRecord(&record, output_printer_fp, OUTPUT_PRT_NUM_RECS) && (output_printer_record_count < max_output_printer_records)) {
    output_printer_records[output_printer_record_count] = (OutputPrinterItem *) malloc(sizeof(OutputPrinterItem));
    output_printer_records[output_printer_record_count]->out_type_title = allocAndCopyString(record.field[0].data);
    output_printer_records[output_printer_record_count]->progroup_title = allocAndCopyString(record.field[1].data); 
    output_printer_records[output_printer_record_count]->prt_name = allocAndCopyString(record.field[2].data);
    output_printer_records[output_printer_record_count]->hostname = allocAndCopyString(record.field[3].data);
    output_printer_record_count++;
  }

  fclose(output_printer_fp);
}

static void freeOutputPrinterData()
{
  int  k;

  for (k=0; k < output_printer_record_count; k++) {
      free(output_printer_records[k]->out_type_title);
      free(output_printer_records[k]->progroup_title);
      free(output_printer_records[k]->prt_name);
      free(output_printer_records[k]->hostname);
      free(output_printer_records[k]);
  }
  free(output_printer_records);
}

static void Tab()
{
  if (print_data) strcpy(tab, "  ");
}

static void NoTab()
{
  strcpy(tab,"");
}

static void Message(message)
     const char *message;
{
  if (print_data) fprintf(stdout,"%s%s\n",tab,message);
}

static void PutInt(n, size)
     int n;
     int size;
{
  fprintf(stdout,"%*d ",size,n);
}

static void PutString(string, size)
     const char *string;
     int size;
{
  fprintf(stdout,"%-*.*s ",size,size,string);
}

static void PutTab()
{
  fprintf(stdout,tab);
}

static void Newline()
{
  fprintf(stdout,"\n");
}

static void Underline(size)
     int size;
{
  char underline_buffer[100];
  memset(underline_buffer,'-',size);
  underline_buffer[size] = '\0';
  PutString(underline_buffer,size);
}

static void PrintOutputPrinterRecord(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {
      fprintf( stdout,
               "\n"
               "%sOUTPUT TYPE/PRINTER LINK RECORD\n"
               "%s-------------------------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sOUTPUT TYPE TITLE   (PK) : %s\n"
               "%sPROGRAM GROUP TITLE (PK) : %s\n"
               "%sPRINTER NAME        (PK) : %s\n"
               "%sHOST NAME           (PK) : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,output_printer_records[index_]->out_type_title,
               tab,output_printer_records[index_]->progroup_title,
               tab,output_printer_records[index_]->prt_name,
               tab,output_printer_records[index_]->hostname );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("OUTPUT_TYPE",16);
        PutString("PROGROUP_TITLE",16);
        PutString("PRINTER NAME",16);
        PutString("HOST NAME",16);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(16);
        Underline(16);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(output_printer_records[index_]->out_type_title,16);
      PutString(output_printer_records[index_]->progroup_title,16);
      PutString(output_printer_records[index_]->prt_name,16);
      PutString(output_printer_records[index_]->hostname,16);
      Newline();
    }
  }

  SetCheckedTable("OUTPUT TYPE/PRINTER LINK RECORD");
  SetCheckedPrimaryKey(output_printer_records[index_]->out_type_title);
  AddToCheckedPrimaryKey(output_printer_records[index_]->progroup_title);
  AddToCheckedPrimaryKey(output_printer_records[index_]->prt_name);
  AddToCheckedPrimaryKey(output_printer_records[index_]->hostname);

  CHECK_FLD( checkOutputTypeTitle, output_printer_records[index_]->out_type_title, "OUTPUT TYPE TITLE" );
  CHECK_FIELD( checkProgroupTitle,  output_printer_records[index_]->progroup_title );
  CHECK_FLD( checkPrtName, output_printer_records[index_]->prt_name, "PRINTER NAME" );
  CHECK_FLD( checkHostName, output_printer_records[index_]->hostname, "HOST NAME" );

}

static void PrintPrinterClass(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  /* done to get rid of compiler warning about unused parameters */
  print_full=print_full;

  if (print_data) {
    if (1) {  /* always show printer classes in full view */
      fprintf( stdout,
               "\n"
               "%sPRINTER CLASS\n"
               "%s-------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sPRINTER CLASS TITLE (PK) : %s\n"
               "%sPRINTER DEVICE NAME      : %s\n"
               "%sPRINTER DRIVER NAME      : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,printer_classes[index_]->prt_class_title,
               tab,printer_classes[index_]->prt_device_name,
               tab,printer_classes[index_]->prt_driver_name );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PRINTER CLASS",32);
        PutString("DEVICE NAME",128);
        PutString("DRIVER NAME",32);
        Newline();
        PutTab();
        Underline(7);
        Underline(32);
        Underline(128);
        Underline(32);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(printer_classes[index_]->prt_class_title,32);
      PutString(printer_classes[index_]->prt_device_name,128);
      PutString(printer_classes[index_]->prt_driver_name,32);
      Newline();
    }
  }

  SetCheckedTable("PRINTER CLASS");
  SetCheckedPrimaryKey(printer_classes[index_]->prt_class_title);

  CHECK_FLD( checkPrtClassTitle, printer_classes[index_]->prt_class_title, "PRINTER CLASS TITLE" );
  CHECK_FLD( checkPrtDeviceName, printer_classes[index_]->prt_device_name, "PRINTER DEVICE NAME" );
  CHECK_FLD( checkPrtDriverName, printer_classes[index_]->prt_driver_name, "PRINTER DRIVER NAME" );
}

static void PrintPrinterAdapter(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  /* done to get rid of compiler warning about unused parameters */
  print_full=print_full;

  if (print_data) {
    if (1) { /* always print in full view */
      fprintf( stdout,
               "\n"
               "%sPRINTER ADAPTER\n"
               "%s---------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sADAPTER TYPE             : %s\n"
               "%sMACHINE ADDRESS (UNIQUE) : %s\n"
               "%sIP ADDRESS      (PK)     : %s\n"
               "%sLOCATION                 : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,adapters[index_]->adapter_type,
               tab,adapters[index_]->machine_address,
               tab,adapters[index_]->ip_address,
               tab,adapters[index_]->location );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("ADAPTER TYPE",32);
        PutString("MACHINE ADDRESS",32);
        PutString("IP ADDRESS",32);
        PutString("LOCATION",64);
        Newline();
        PutTab();
        Underline(7);
        Underline(32);
        Underline(32);
        Underline(32);
        Underline(64);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(adapters[index_]->adapter_type,32);
      PutString(adapters[index_]->machine_address,32);
      PutString(adapters[index_]->ip_address,32);
      PutString(adapters[index_]->location,64);
      Newline();
    }

  }

  SetCheckedTable("PRINTER ADAPTER");
  SetCheckedPrimaryKey(adapters[index_]->ip_address);

  CHECK_FIELD( CheckAdapterType, adapters[index_]->adapter_type );
  CHECK_FLD( checkEthernetAddress, adapters[index_]->machine_address, "MACHINE ADDRESS" );
  CHECK_FLD( checkIpAddress, adapters[index_]->ip_address, "IP ADDRESS" );
  CHECK_FLD( checkAdapterLocation, adapters[index_]->location, "LOCATION" );

}

static void PrintCurrentApp(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {

      fprintf( stdout,
               "\n"
               "%sCURRENT APPLICATION\n"
               "%s-------------------\n"
               "%sRECORD NUMBER             : %d\n"
               "%sPROGRAM GROUP TITLE (PK)  : %s\n"
               "%sHOST NAME                 : %s\n"
               "%sUSER COUNT                : %d\n"
               "%sAPPLICATION LOCK          : %d\n",
               tab,tab,
               tab,index_ + 1,
               tab,current_apps[index_]->progroup_title,
               tab,current_apps[index_]->hostname,
               tab,current_apps[index_]->num_roles,
               tab,current_apps[index_]->icon_index );
      
    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGRAM GROUP",16);
        PutString("HOST NAME",16);
        PutString("USER COUNT",7);
        PutString("LOCK",7);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(7);
        Underline(7);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(current_apps[index_]->progroup_title,16);
      PutString(current_apps[index_]->hostname,16);
      PutInt(current_apps[index_]->num_roles,7);
      PutInt(current_apps[index_]->icon_index,7);
      Newline();
    }

  }

  SetCheckedTable("CURRENT APPLICATION");
  SetCheckedPrimaryKey(current_apps[index_]->progroup_title);

  CHECK_FIELD( CheckNumUsers, current_apps[index_]->num_roles );
  CHECK_FIELD( CheckAppLock, current_apps[index_]->icon_index ); 
  CHECK_FIELD( checkProgroupTitle,  current_apps[index_]->progroup_title ); 
  CHECK_FLD( checkHostName, current_apps[index_]->hostname, "HOST NAME" );

}

static void PrintHost(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  struct sockaddr_in addr;
  struct hostent *hostent_ptr;
  int fd;

  /* done to get rid of compiler warning about unused parameters */
  print_full=print_full;
  first_in_series=first_in_series;

  if (print_data) {
    fprintf( stdout,
             "\n"
             "%sHOST RECORD\n"
             "%s-----------\n"
             "%sRECORD NUMBER          : %d\n"
             "%sHOST NAME     (PK)     : %s\n"
             "%sIP ADDRESS    (UNIQUE) : %s\n"
             "%sPROCESS COUNT          : %d\n"
             "%sTYPE                   : %d\n"
             "%sREPLICATION            : %d\n",
             tab,tab,
             tab,index_ + 1,
             tab,hosts[index_]->hostname,
             tab,hosts[index_]->ip_address,
             tab,hosts[index_]->num_processes,
             tab,hosts[index_]->type,
             tab,hosts[index_]->repl );
  }

  SetCheckedTable("HOST");
  SetCheckedPrimaryKey(hosts[index_]->hostname);

  if (hosts[index_]->num_processes > 100) {
    fprintf(stdout,"%s*** WARNING *** %s has more than 100 processes.\n",tab,hosts[index_]->hostname);
  }

  CHECK_FIELD( CheckNumProcesses, hosts[index_]->num_processes );
  CHECK_FIELD( CheckHostType, hosts[index_]->type );
  CHECK_FIELD( CheckHostReplication, hosts[index_]->repl );
  CHECK_FLD( checkHostName, hosts[index_]->hostname, "HOST NAME" );
  CHECK_FLD( checkIpAddress, hosts[index_]->ip_address, "IP ADDRESS");

  if (check_connection) {
    hostent_ptr = gethostbyname(hosts[index_]->hostname);
    /*bzero(&addr, sizeof(addr));*/
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    /*addr.sin_addr.s_addr = ((struct in_addr *) hostent_ptr->h_addr_list[0])->s_addr; */
    addr.sin_addr.s_addr = strtoul(hostent_ptr->h_addr, (char**)NULL, 10);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      fprintf(stdout,"%s*** ERROR *** error creating socket to test connecting to host.\n",tab);
    } else {
      if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stdout,"%s*** HOST ERROR *** error connecting to host: %s port: %d.\n",tab,hosts[index_]->hostname,port);
      } 
      close(fd);
    }
  }
}

static void PrintPdj(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {
      fprintf( stdout,
               "\n"
               "%sPREDEFINED JOB RECORD\n"
               "%s---------------------\n"
               "%sRECORD NUMBER      : %d\n"
               "%sPROGRAM GROUP (PK) : %s\n"
               "%sJOB TITLE     (PK) : %s\n"
               "%sCOMMAND LINE       : %s\n"
               "%sACCESS ROLE        : %d\n"
               "%sFLAG               : %d\n"
               "%sSCHEDULE           : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,pdjs[index_]->progroup_title,
               tab,pdjs[index_]->job_title,
               tab,pdjs[index_]->command_line,
               tab,pdjs[index_]->role_access,
               tab,pdjs[index_]->flag,
               tab,pdjs[index_]->schedule_str );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGRAM GROUP",32);
        PutString("JOB TITLE",32);
        PutString("COMMAND LINE",50);
        PutString("ACCESS ROLE",7);
        PutString("FLAG",7);
        PutString("SCHEDULE",30);
        Newline();
        PutTab();
        Underline(7);
        Underline(32);
        Underline(32);
        Underline(50);
        Underline(7);
        Underline(7);
        Underline(30);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(pdjs[index_]->progroup_title,32);
      PutString(pdjs[index_]->job_title,32);
      PutString(pdjs[index_]->command_line,50);
      PutInt(pdjs[index_]->role_access,7);
      PutInt(pdjs[index_]->flag,7);
      PutString(pdjs[index_]->schedule_str,30);
      Newline();
    }
  }

  SetCheckedTable("PREDEFINED JOB");
  SetCheckedPrimaryKey(pdjs[index_]->progroup_title);
  AddToCheckedPrimaryKey(pdjs[index_]->job_title);

  CHECK_FIELD( CheckAccessRoleNumber, pdjs[index_]->role_access);
  CHECK_FIELD( CheckScheduleString, pdjs[index_]->schedule_str);
  CHECK_FIELD( CheckPdjFlag, pdjs[index_]->flag);
  CHECK_FIELD( checkProgroupTitle,  pdjs[index_]->progroup_title );
  CHECK_FIELD( checkPredefinedJobTitle, pdjs[index_]->job_title );
  CHECK_FIELD( checkCommandLine, pdjs[index_]->command_line );
}
 
static void PrintOutputType(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {

      fprintf( stdout,
               "\n"
               "%sOUTPUT TYPE\n"
               "%s-----------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sOUTPUT TYPE TITLE   (PK) : %s\n"
               "%sPROGRAM GROUP TITLE (PK) : %s\n"
               "%sDESCRIPTION              : %s\n"
               "%sDEFAULT PRINTER          : %s\n"
               "%sBATCH FLAG               : %d\n",
               tab,tab,
               tab,index_ + 1,
               tab,output_types[index_]->out_type_title,
               tab,output_types[index_]->progroup_title,
               tab,output_types[index_]->description,
               tab,output_types[index_]->default_prt,
               tab,output_types[index_]->batch_flag );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("OUTPUT TYPE",16);
        PutString("PROGRAM GROUP",16);
        PutString("DESCRIPTION",64);
        PutString("DEFAULT PRINTER",64);
        PutString("BATCH FLAG",7);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(64);
        Underline(64);
        Underline(7);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(output_types[index_]->out_type_title,16);
      PutString(output_types[index_]->progroup_title,16);
      PutString(output_types[index_]->description,64);
      PutString(output_types[index_]->default_prt,64);
      PutInt(output_types[index_]->batch_flag,7);
      Newline();
    }
  }

  SetCheckedTable("OUTPUT TYPE");
  SetCheckedPrimaryKey(output_types[index_]->progroup_title);
  AddToCheckedPrimaryKey(output_types[index_]->out_type_title);

  CHECK_FIELD( CheckBatchFlag, output_types[index_]->batch_flag );
  /* Replaced, found with ANSI compile */
/*  CHECK_FIELD( checkOutputTypeTitle, output_types[index_]->out_type_title ); */
  CHECK_FLD( checkOutputTypeTitle, output_types[index_]->out_type_title , "OUTPUT TYPE TITLE"); 

  CHECK_FIELD( checkProgroupTitle,  output_types[index_]->progroup_title );
  /* Replaced, found with ANSI compile */
  /* CHECK_FIELD( checkOutputTypeDescription, output_types[index_]->description ); */
  CHECK_FLD( checkOutputTypeDescription, output_types[index_]->description, "OUTPUT TYPE DESCRIPTION" );
  CHECK_FLD( checkPrtName, output_types[index_]->default_prt, "DEFAULT PRINTER" );

}
         
static void PrintAppuser(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {
      fprintf( stdout,
               "\n"
               "%sAPPLICATION/USER LINK RECORD\n"
               "%s----------------------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sPROGRAM GROUP TITLE (PK) : %s\n"
               "%sLOGIN NAME          (PK) : %s\n"
               "%sREAL NAME                : %s\n"
               "%sAPPLICATION DATA         : %s\n"
               "%sAPPLICATION PASSWORD     : %s\n"
               "%sAPPLICATION ROLE         : %d\n"
               "%sREGISTERED USER          : %d\n",
               tab,tab,
               tab,index_ + 1,
               tab,appusers[index_]->progroup_title,
               tab,appusers[index_]->login_name,
               tab,appusers[index_]->real_name,
               tab,appusers[index_]->app_data,
               tab,appusers[index_]->app_passwd,
               tab,appusers[index_]->app_role,
               tab,appusers[index_]->registered_user );
      
    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGRAM GROUP",16);
        PutString("LOGIN",8);
        PutString("REAL NAME",30);
        PutString("APPLICATION DATA",50);
        PutString("APPLICATION PASSWD",32);
        PutString("APPROLE",7);
        PutString("REGUSER",7);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(8);
        Underline(30);
        Underline(50);
        Underline(32);
        Underline(7);
        Underline(7);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(appusers[index_]->progroup_title,16);
      PutString(appusers[index_]->login_name,8);
      PutString(appusers[index_]->real_name,30);
      PutString(appusers[index_]->app_data,50);
      PutString(appusers[index_]->app_passwd,32);
      PutInt(appusers[index_]->app_role,7);
      PutInt(appusers[index_]->registered_user,7);
      Newline();
    }
  }

  SetCheckedTable("APPLICATION/USER LINK RECORD");
  SetCheckedPrimaryKey( appusers[index_]->progroup_title );
  AddToCheckedPrimaryKey( appusers[index_]->login_name );

  CHECK_FIELD( CheckAppData, appusers[index_]->app_data);
  CHECK_FIELD( CheckAppPasswd, appusers[index_]->app_passwd);
  CHECK_FIELD( CheckAppRole, appusers[index_]->app_role);
  CHECK_FIELD( CheckRegisteredUser, appusers[index_]->registered_user);
  CHECK_FIELD( CheckLoginName, appusers[index_]->login_name);
  CHECK_FIELD( checkProgroupTitle,  appusers[index_]->progroup_title );
  CHECK_FLD( checkLoginName, appusers[index_]->login_name, "LOGIN NAME" );
  /* Commented out with ANSI compile.  checkRealName is a function in 
  UserFcts and it takes 4 parameters.  This is only passing 1 parameter.  
  The complete name.*/
  /* CHECK_FLD( checkRealName, appusers[index_]->real_name, "REAL NAME" ); */

}

static void PrintAccessRole(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {
      fprintf( stdout,
               "\n"
               "%sACCESS ROLE\n"
               "%s-----------\n"
               "%sRECORD NUMBER      : %d\n"
               "%sPROGRAM GROUP (PK) : %s\n"
               "%sACCESS ROLE   (PK) : %s\n"
               "%sROLE NUMBER        : %d\n",
               tab,tab,
               tab,index_ + 1,
               tab,access_roles[index_]->progroup_title,
               tab,access_roles[index_]->app_role,
               tab,access_roles[index_]->role_number );
      
    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGRAM GROUP",16);
        PutString("ROLE NUMBER",7);
        PutString("ACCESS ROLE",64);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(7);
        Underline(64);
        Newline();
      }
      PutTab();
      PutInt(index_ + 1,7);
      PutString(access_roles[index_]->progroup_title,16);
      PutInt(access_roles[index_]->role_number,7);
      PutString(access_roles[index_]->app_role,64);
      Newline();
    }
  }

  SetCheckedTable("ACCESS ROLE");
  SetCheckedPrimaryKey(access_roles[index_]->progroup_title);
  AddToCheckedPrimaryKey(access_roles[index_]->app_role);

  CHECK_FIELD( CheckAccessRole, access_roles[index_]->app_role );
  CHECK_FIELD( CheckAccessRoleNumber, access_roles[index_]->role_number );
  CHECK_FIELD( checkProgroupTitle,  access_roles[index_]->progroup_title );
}


static void PrintProgram(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  /* done to get rid of compiler warning about unused parameters */
  print_full=print_full;

  if (print_data) {
    if (1) { /* always print programs in full format */
      fprintf( stdout,
               "\n"
               "%sPROGRAM\n"
               "%s-------\n"
               "%sRECORD NUMBER              : %d\n"
               "%sPROGRAM GROUP         (PK) : %s\n"
               "%sPROGRAM TITLE         (PK) : %s\n"
               "%sPROGRAM FILE               : %s\n"
               "%sICON FILE                  : %s\n"
               "%sICON INDEX                 : %d\n"
               "%sCOMMAND LINE ARGUMENTS     : %s\n"
               "%sSECURITY CLASS             : %d\n"
               "%sPROGRAM ACCESS             : %d\n"
               "%sFLAG                       : %d\n"
               "%sWORKING DIRECTORY          : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,programs[index_]->progroup_title,
               tab,programs[index_]->program_title,
               tab,programs[index_]->program_file,
               tab,programs[index_]->icon_file,
               tab,programs[index_]->icon_index,
               tab,programs[index_]->cmd_line_args,
               tab,programs[index_]->security_class,
               tab,programs[index_]->program_access,
               tab,programs[index_]->flag,
               tab,programs[index_]->working_dir);
      
    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGRAM GROUP",16);
        PutString("PROGRAM TITLE",30);
        PutString("PROGRAM FILE",32);
        PutString("ICON FILE",30);
        PutString("ICON INDEX",7);
        PutString("COMMAND LINE ARGS",60);
        PutString("SECURITY CLASS",7);
        PutString("ACCESS",7);
        PutString("FLAG",7);
        PutString("WORKING DIRECTORY",40);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(30);
        Underline(32);
        Underline(30);
        Underline(7);
        Underline(60);
        Underline(7);
        Underline(7);
        Underline(7);
        Underline(40);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(programs[index_]->progroup_title,16);
      PutString(programs[index_]->program_title,30);
      PutString(programs[index_]->program_file,32);
      PutString(programs[index_]->icon_file,30);
      PutString((char *)programs[index_]->icon_index,7);
      PutString(programs[index_]->cmd_line_args,60);
      PutInt(programs[index_]->security_class,7);
      PutInt(programs[index_]->program_access,7);
      PutInt(programs[index_]->flag, 7);
      PutString(programs[index_]->working_dir, 40);
      Newline();
    }
  }

  SetCheckedTable("PROGRAM");
  SetCheckedPrimaryKey(programs[index_]->progroup_title);
  AddToCheckedPrimaryKey(programs[index_]->program_title);

  CHECK_FIELD( CheckSecurityClass, programs[index_]->security_class);
  CHECK_FIELD( CheckProgramAccess, programs[index_]->program_access);
  CHECK_FIELD( CheckIconIndex, programs[index_]->icon_index);
  CHECK_FIELD( CheckProgramFlag, programs[index_]->flag);
  CHECK_FIELD( checkProgroupTitle, programs[index_]->progroup_title );
  CHECK_FIELD( checkProgramTitle, programs[index_]->program_title );
  CHECK_FIELD( checkProgramFile, programs[index_]->program_file );
  CHECK_FIELD( checkIconFile, programs[index_]->icon_file );
  CHECK_FIELD( checkCmdLineArgs, programs[index_]->cmd_line_args );
  CHECK_FIELD( checkWorkingDir, programs[index_]->working_dir );
}


static void PrintProgroup(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {

      fprintf( stdout,
               "\n"
               "%sPROGRAM GROUP RECORD\n"
               "%s--------------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sPROGRAM GROUP TITLE (PK) : %s\n"
               "%sHOST NAME                : %s\n"
               "%sICON FILE                : %s\n"
               "%sVERSION NUMBER           : %s\n"
               "%sRELEASE DATE             : %s\n"
               "%sCLIENT LOCATION          : %s\n"
               "%sSERVER LOCATION          : %s\n"
               "%sUNIX GROUP               : %s\n"
               "%sPROCESS COUNT            : %d\n"
               "%sLINK DATA                : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,progroups[index_]->progroup_title,
               tab,progroups[index_]->hostname,
               tab,progroups[index_]->icon_file,
               tab,progroups[index_]->version_number,
               tab,progroups[index_]->release_date,
               tab,progroups[index_]->client_location,
               tab,progroups[index_]->server_location,
               tab,progroups[index_]->unix_group,
               tab,progroups[index_]->num_processes,
               tab,progroups[index_]->link_data );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGROUP",16);
        PutString("HOST NAME",16);
        PutString("ICON FILE",30);
        PutString("VERSION NUMBER",15);
        PutString("RELEASE DATE",15);
        PutString("CLIENT LOCATION",20);
        PutString("SERVER LOCATION",20);
        PutString("UNIX GROUP",16);
        PutString("PROCESS COUNT",7);
        PutString("LINK DATA",10);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(30);
        Underline(15);
        Underline(15);
        Underline(20);
        Underline(20);
        Underline(16);
        Underline(7);
        Underline(10);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(progroups[index_]->progroup_title,16);
      PutString(progroups[index_]->hostname,16);
      PutString(progroups[index_]->icon_file,30);
      PutString(progroups[index_]->version_number,15);
      PutString(progroups[index_]->release_date,15);
      PutString(progroups[index_]->client_location,20);
      PutString(progroups[index_]->server_location,20);
      PutString(progroups[index_]->unix_group,16);
      PutInt(progroups[index_]->num_processes,7);
      PutString(progroups[index_]->link_data,10);
      Newline();
    }

  }

  SetCheckedTable("PROGRAM GROUP");
  SetCheckedPrimaryKey(progroups[index_]->progroup_title);

  CHECK_FIELD( CheckReleaseDate, progroups[index_]->release_date);
  CHECK_FIELD( CheckNumProcesses, progroups[index_]->num_processes);
  CHECK_FIELD( CheckLinkData, progroups[index_]->link_data);
  CHECK_FIELD( checkProgroupTitle,  progroups[index_]->progroup_title );
  CHECK_FLD( checkHostName, progroups[index_]->hostname, "HOST NAME" );
  CHECK_FIELD( checkIconFile,       progroups[index_]->icon_file );
  CHECK_FIELD( checkVersionNumber,  progroups[index_]->version_number );
  CHECK_FIELD( checkClientLocation, progroups[index_]->client_location );
  CHECK_FIELD( checkServerLocation, progroups[index_]->server_location );
  CHECK_FIELD( checkUnixGroup,      progroups[index_]->unix_group );
}

static void PrintPrinterAccessRecord(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {
      fprintf( stdout,
               "\n"
               "%sPRINTER ACCESS RECORD\n"
               "%s---------------------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sPROGRAM GROUP TITLE (PK) : %s\n"
               "%sHOST NAME           (PK) : %s\n"
               "%sPRINTER NAME        (PK) : %s\n",
               tab,tab,
               tab,index_ + 1,
               tab,printer_access_records[index_]->progroup_title,
               tab,printer_access_records[index_]->hostname,
               tab,printer_access_records[index_]->prt_name );
      
    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PROGROUP",16);
        PutString("HOST NAME",16);
        PutString("PRINTER NAME",16);
        Newline();
        PutTab();
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(16);
        Newline();
      }
      PutTab();
      PutInt(index_ + 1,7);
      PutString(printer_access_records[index_]->progroup_title,16);
      PutString(printer_access_records[index_]->hostname,16);
      PutString(printer_access_records[index_]->prt_name,16);
      Newline();
    }
  }

  SetCheckedTable("PRINTER ACCESS RECORD");
  SetCheckedPrimaryKey(printer_access_records[index_]->progroup_title);
  AddToCheckedPrimaryKey(printer_access_records[index_]->hostname);
  AddToCheckedPrimaryKey(printer_access_records[index_]->prt_name);

  CHECK_FIELD( checkProgroupTitle,  printer_access_records[index_]->progroup_title );
  CHECK_FLD( checkHostName, printer_access_records[index_]->hostname, "HOST NAME" );
  CHECK_FLD( checkPrtName, printer_access_records[index_]->prt_name, "PRINTER NAME" );
}


static void PrintUser(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  /* done to get rid of compiler warning about unused parameters */
  print_full=print_full;
  first_in_series=first_in_series;
  if (print_data) {
    fprintf( stdout,
             "\n"
             "%sUSER\n"
             "%s----\n"
             "%sRECORD NUMBER              : %d\n"
             "%sUNIX ID                    : %d\n"
             "%sLOGIN NAME            (PK) : %s\n"
             "%sREAL NAME                  : %s\n"
             "%sPASSWORD                   : %s\n"
             "%sSSN                        : %s\n"
             "%sPHONE NUMBER               : %s\n"
             "%sSECURITY CLASS             : %d\n"
             "%sPASSWORD CHANGE TIME       : %s\n"
             "%sUSER ROLE                  : %d\n"
             "%sUSER LOCK                  : %d\n"
             "%sSHARED PASSWORD            : %s\n"
             "%sAUTHENTICATION SERVER      : %s\n",
             tab,tab,
             tab,index_ + 1,
             tab,users[index_]->unix_id,
             tab,users[index_]->login_name,
             tab,users[index_]->real_name,
             tab,users[index_]->password,
             tab,users[index_]->ss_number,
             tab,users[index_]->phone_number,
             tab,users[index_]->security_class,
             tab,users[index_]->pw_change_time,
             tab,users[index_]->user_role,
             tab,users[index_]->user_lock,
             tab,users[index_]->shared_passwd,
             tab,users[index_]->auth_server );

  }

  SetCheckedTable("USER");
  SetCheckedPrimaryKey(users[index_]->login_name);

  CHECK_FIELD( CheckSecurityClass, users[index_]->security_class );
  CHECK_FIELD( CheckPwChangeTime, users[index_]->pw_change_time );
  CHECK_FIELD( CheckUserRole, users[index_]->user_role );
  CHECK_FIELD( CheckUserLock, users[index_]->user_lock );
  CHECK_FIELD( CheckSharedPasswd, users[index_]->shared_passwd );
  CHECK_FIELD( CheckAuthServer, users[index_]->auth_server );
  CHECK_FIELD( CheckLoginName, users[index_]->login_name );
  CHECK_FLD( checkLoginName, users[index_]->login_name, "LOGIN NAME" );
  /* Commented out with ANSI compile.  checkRealName is a function in 
  UserFcts and it takes 4 parameters.  This is only passing 1 parameter.  
  The complete name.*/
  /* CHECK_FLD( checkRealName, users[index_]->real_name, "REAL NAME" ); */
  CHECK_FLD( checkUserPassword, users[index_]->password, "PASSWORD" );
  CHECK_FLD( checkSSNumber, users[index_]->ss_number, "SOCIAL SECURITY NUMBER" );
  CHECK_FLD( checkPhoneNumber, users[index_]->phone_number, "PHONE NUMBER" );

}

static void PrintPrinter(index_, print_full, first_in_series)
     int index_;
     int print_full;
     int first_in_series;
{
  if (print_data) {
    if (print_full) {

      fprintf( stdout,
               "\n"
               "%sPRINTER\n"
               "%s-------\n"
               "%sRECORD NUMBER            : %d\n"
               "%sPRINTER NAME        (PK) : %s\n"
               "%sHOST NAME           (PK) : %s\n"
               "%sPRINTER LOCATION         : %s\n"
               "%sMAX SIZE                 : %d\n"
               "%sPRINTER CLASS TITLE      : %s\n"
               "%sIP ADDRESS               : %s\n"
               "%sSECURITY CLASS           : %d\n"
               "%sSTATUS                   : %d\n"
               "%sPORT NAME                : %s\n"
               "%sSUSPEND FLAG             : %d\n"
               "%sREDIRECT PRINTER         : %s\n"
               "%sFILE NAME                : %s\n",
               tab,tab,
               tab,index_+ 1,
               tab,printers[index_]->prt_name,
               tab,printers[index_]->hostname,
               tab,printers[index_]->prt_location,
               tab,printers[index_]->max_size,
               tab,printers[index_]->prt_class_title,
               tab,printers[index_]->ip_address,
               tab,printers[index_]->security_class,
               tab,printers[index_]->status,
               tab,printers[index_]->port_name,
               tab,printers[index_]->suspend_flag,
               tab,printers[index_]->redirect_prt,
               tab,printers[index_]->FileName );

    } else {

      if (first_in_series) {
        Newline();
        PutTab();
        PutString("RECORD",7);
        PutString("PRINTER",12);
        PutString("HOST",12);
        PutString("LOCATION",15);
        PutString("MAX SIZE",7);
        PutString("PRINTER CLASS",16);
        PutString("IP ADDRESS",16);
        PutString("SECURITY CLASS",7);
        PutString("STATUS",7);
        PutString("PORT",16);
        PutString("SUSPEND FLAG",7);
        PutString("REDIRECT PRINTER",15);
        PutString("FILENAME",12);
        Newline();
        PutTab();
        Underline(7);
        Underline(12);
        Underline(12);
        Underline(15);
        Underline(7);
        Underline(16);
        Underline(16);
        Underline(7);
        Underline(7);
        Underline(16);
        Underline(7);
        Underline(15);
        Underline(12);
        Newline();
      }

      PutTab();
      PutInt(index_ + 1,7);
      PutString(printers[index_]->prt_name,12);
      PutString(printers[index_]->hostname,12);
      PutString(printers[index_]->prt_location,15);
      PutInt(printers[index_]->max_size,7);
      PutString(printers[index_]->prt_class_title,16);
      PutString(printers[index_]->ip_address,16);
      PutInt(printers[index_]->security_class,7);
      PutInt(printers[index_]->status,7);
      PutString(printers[index_]->port_name,16);
      PutInt(printers[index_]->suspend_flag,7);
      PutString(printers[index_]->redirect_prt,15);
      PutString(printers[index_]->FileName,12);
      Newline();
    }
  }

  SetCheckedTable("PRINTER");
  SetCheckedPrimaryKey(printers[index_]->prt_name);
  SetCheckedPrimaryKey(printers[index_]->hostname);

  CHECK_FIELD( CheckMaxsize, printers[index_]->max_size);
  CHECK_FIELD( CheckSecurityClass, printers[index_]->security_class);
  CHECK_FIELD( CheckStatus, printers[index_]->status);
  CHECK_FIELD( CheckPortName, printers[index_]->port_name);
  CHECK_FIELD( CheckSuspendFlag, printers[index_]->suspend_flag);
  CHECK_FIELD( CheckFileName, printers[index_]->FileName);
  CHECK_FLD( checkPrtName, printers[index_]->prt_name, "PRINTER NAME" );
  CHECK_FLD( checkHostName, printers[index_]->hostname, "HOST NAME" );
  CHECK_FLD( checkPrtLocation, printers[index_]->prt_location, "PRINTER LOCATION" );
  CHECK_FLD( checkPrtClassTitle, printers[index_]->prt_class_title, "PRINTER CLASS" );
  CHECK_FLD( checkIpAddress, printers[index_]->ip_address, "IP ADDRESS" );
  CHECK_FLD( checkPrtName, printers[index_]->redirect_prt, "REDIRECT PRINTER" );
}

/* set the primary key for display purposes */

static void SetPrimaryKey(field)
     char *field;
{
  strcpy(primary_key, field);
}

static void AddToPrimaryKey(field)
     char *field;
{
  strcat(primary_key, ", ");
  strcat(primary_key, field);
}

/* set the related table's primary key for display purposes */

static void SetCheckedPrimaryKey(field)
     char *field;
{
  strcpy(checked_primary_key, field);
}

static void AddToCheckedPrimaryKey(field)
     char *field;
{
  strcat(checked_primary_key, ", ");
  strcat(checked_primary_key, field);
}

static void SetCheckedTable(table)
     const char *table;
{
  strcpy(checked_table, table);
}


static void CheckHosts()
{
  NoTab();

  Message("\n");
  Message("CHECKING HOSTS");
  Message("--------------");

  for (primary_index = 0; primary_index < host_count; primary_index++) {

    SetPrimaryKey(hosts[primary_index]->hostname);

    PrintHost(primary_index,FULL_FORMAT,0);

    Tab();

    CHECK_RELATED_RECORDS( "PROGROUPS",
                           progroup_count,
                           (strcmp(hosts[primary_index]->hostname, progroups[related_index]->hostname) == 0),
                           PrintProgroup,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "CURRENT APPLICATIONS",
                           current_app_count,
                           (strcmp(hosts[primary_index]->hostname, current_apps[related_index]->hostname) == 0),
                           PrintCurrentApp,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "PRINTERS",
                           printer_count,
                           (strcmp(hosts[primary_index]->hostname, printers[related_index]->hostname) == 0),
                           PrintPrinter,
                           LIST_FORMAT );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("HOSTS",
                       host_count,
                       (strcmp(hosts[primary_index]->hostname, hosts[related_index]->hostname) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF HOSTS: %d\n",host_count);

}

static void CheckProgroups()
{
  NoTab();

  Message("\n");
  Message("CHECKING PROGRAM GROUPS");
  Message("-----------------------");

  for (primary_index = 0; primary_index < progroup_count; primary_index++) {

    SetPrimaryKey(progroups[primary_index]->progroup_title);
    
    PrintProgroup(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "PROGRAMS",
                           program_count,
                           (strcmp(progroups[primary_index]->progroup_title, programs[related_index]->progroup_title) == 0),
                           PrintProgram,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "ACCESS ROLES",
                           access_role_count,
                           (strcmp(progroups[primary_index]->progroup_title, access_roles[related_index]->progroup_title) == 0),
                           PrintAccessRole,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "OUTPUT TYPES",
                           output_type_count,
                           (strcmp(progroups[primary_index]->progroup_title, output_types[related_index]->progroup_title) == 0),
                           PrintOutputType,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "PRINTER ACCESS RECORDS",
                           printer_access_record_count,
                           (strcmp(progroups[primary_index]->progroup_title, printer_access_records[related_index]->progroup_title) == 0),
                           PrintPrinterAccessRecord,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "APPLICATION/USER RECORDS",
                           appuser_count,
                           (strcmp(progroups[primary_index]->progroup_title, appusers[related_index]->progroup_title) == 0),
                           PrintAppuser,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "PREDEFINED JOBS",
                           pdj_count, 
                           (strcmp(progroups[primary_index]->progroup_title, pdjs[related_index]->progroup_title) == 0),
                           PrintPdj,
                           LIST_FORMAT );

    CHECK_FOR_ONE_RELATED_RECORD( "PROGRAM GROUP",
                                  "HOST",
                                  "HOSTS",
                                  host_count,
                                  (strcmp(progroups[primary_index]->hostname, hosts[related_index]->hostname) == 0),
                                  PrintHost );

    CHECK_RELATED_RECORDS( "CURRENT APPLICATIONS",
                           current_app_count,
                           (strcmp(progroups[primary_index]->progroup_title, current_apps[related_index]->progroup_title) == 0),
                           PrintCurrentApp,
                           LIST_FORMAT );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PROGROUPS",
                       progroup_count,
                       (strcmp(progroups[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PROGRAM GROUPS: %d\n",progroup_count);

}

static void CheckPrinters()
{
  NoTab();

  Message("\n");
  Message("CHECKING PRINTERS");
  Message("-----------------");

  for (primary_index = 0; primary_index < printer_count; primary_index++) {

    SetPrimaryKey(printers[primary_index]->prt_name);
    AddToPrimaryKey(printers[primary_index]->hostname);

    PrintPrinter(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "PRINTER ACCESS RECORDS",
                           printer_access_record_count,
                           ((strcmp(printers[primary_index]->prt_name, printer_access_records[related_index]->prt_name) == 0) &&
                            (strcmp(printers[primary_index]->hostname, printer_access_records[related_index]->hostname) == 0)),
                           PrintPrinterAccessRecord,
                           LIST_FORMAT );

    CHECK_RELATED_RECORDS( "OUTPUT TYPE/PRINTER RECORDS",
                           output_printer_record_count,
                           ((strcmp(printers[primary_index]->prt_name, output_printer_records[related_index]->prt_name) == 0) &&
                            (strcmp(printers[primary_index]->hostname, output_printer_records[related_index]->hostname) == 0)),
                           PrintOutputPrinterRecord,
                           LIST_FORMAT );

    CHECK_FOR_ONE_RELATED_RECORD( "PRINTER",
                                  "PRINTER CLASS",
                                  "PRINTER CLASSES",
                                  printer_class_count,
                                  (strcmp(printers[primary_index]->prt_class_title, printer_classes[related_index]->prt_class_title) == 0),
                                  PrintPrinterClass );

    CHECK_FOR_ONE_RELATED_RECORD( "PRINTER",
                                  "PRINTER ADAPTER",
                                  "PRINTER ADAPTERS",
                                  adapter_count,
                                  (strcmp(printers[primary_index]->ip_address, adapters[related_index]->ip_address) == 0),
                                  PrintPrinterAdapter );

    CHECK_FOR_ONE_RELATED_RECORD( "PRINTER",
                                  "HOST",
                                  "HOSTS",
                                  host_count,
                                  (strcmp(printers[primary_index]->hostname, hosts[related_index]->hostname) == 0),
                                  PrintHost );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PRINTERS",
                       printer_count,
                       ((strcmp(printers[primary_index]->prt_name, printers[related_index]->prt_name) == 0) &&
                        (strcmp(printers[primary_index]->hostname, printers[related_index]->hostname) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PRINTERS: %d\n",printer_count);

}

static void CheckOutputTypes()
{
  NoTab();

  Message("\n");
  Message("CHECKING OUTPUT TYPES");
  Message("---------------------");

  for (primary_index = 0; primary_index < output_type_count; primary_index++) {

    SetPrimaryKey(output_types[primary_index]->out_type_title);
    AddToPrimaryKey(output_types[primary_index]->progroup_title);

    PrintOutputType(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "OUTPUT TYPE/PRINTER RECORDS",
                           output_printer_record_count,
                           ((strcmp(output_types[primary_index]->out_type_title, output_printer_records[related_index]->out_type_title) == 0) &&
                            (strcmp(output_types[primary_index]->progroup_title, output_printer_records[related_index]->progroup_title) == 0)),
                           PrintOutputPrinterRecord,
                           LIST_FORMAT );

    CHECK_FOR_ONE_RELATED_RECORD( "OUTPUT TYPE",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(output_types[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("OUTPUT TYPES",
                       output_type_count,
                       ((strcmp(output_types[primary_index]->out_type_title, output_types[related_index]->out_type_title) == 0) &&
                        (strcmp(output_types[primary_index]->progroup_title, output_types[related_index]->progroup_title) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF OUTPUT TYPES: %d\n",output_type_count);

}


static void CheckPrinterAdapters()
{
  NoTab();

  Message("\n");
  Message("CHECKING PRINTER ADAPTERS");
  Message("-------------------------");

  for (primary_index = 0; primary_index < adapter_count; primary_index++) {

    SetPrimaryKey(adapters[primary_index]->ip_address);

    PrintPrinterAdapter(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "PRINTERS",
                           printer_count,
                           (strcmp(adapters[primary_index]->ip_address, printers[related_index]->ip_address) == 0),
                           PrintPrinter,
                           LIST_FORMAT );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PRINTER ADAPTERS",
                       adapter_count,
                       (strcmp(adapters[primary_index]->ip_address, adapters[related_index]->ip_address) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PRINTER ADAPTERS: %d\n",adapter_count);

}

static void CheckUsers()
{
  NoTab();

  Message("\n");
  Message("CHECKING USERS");
  Message("--------------");

  for (primary_index = 0; primary_index < user_count; primary_index++) {

    SetPrimaryKey(users[primary_index]->login_name);

    PrintUser(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "APPLICATION/USER RECORDS",
                           appuser_count,
                           (strcmp(users[primary_index]->login_name, appusers[related_index]->login_name) == 0),
                           PrintAppuser,
                           LIST_FORMAT );

    CHECK_FOR_ONE_RELATED_RECORD( "USER",
                                  "HOST",
                                  "HOSTS",
                                  host_count,
                                  (strcmp(users[primary_index]->auth_server, hosts[related_index]->hostname) == 0),
                                  PrintHost );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("USERS",
                       user_count,
                       (strcmp(users[primary_index]->login_name, users[related_index]->login_name) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF USERS: %d\n",user_count);

}


static void CheckOutputPrinterRecords()
{
  NoTab();

  Message("\n");
  Message("CHECKING OUTPUT TYPE/PRINTER RECORDS");
  Message("--------------------------------------");

  for (primary_index = 0; primary_index < output_printer_record_count; primary_index++) {

    SetPrimaryKey(output_printer_records[primary_index]->out_type_title);
    AddToPrimaryKey(output_printer_records[primary_index]->progroup_title);
    AddToPrimaryKey(output_printer_records[primary_index]->prt_name);
    AddToPrimaryKey(output_printer_records[primary_index]->hostname);

    PrintOutputPrinterRecord(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "OUTPUT TYPE/PRINTER RECORD",
                                  "OUTPUT TYPE",
                                  "OUTPUT TYPES",
                                  output_type_count,
                                  ((strcmp(output_printer_records[primary_index]->out_type_title, output_types[related_index]->out_type_title) == 0) &&
                                   (strcmp(output_printer_records[primary_index]->progroup_title, output_types[related_index]->progroup_title) == 0)),
                                  PrintOutputType );

    CHECK_FOR_ONE_RELATED_RECORD( "OUTPUT TYPE/PRINTER RECORD",
                                  "PRINTER",
                                  "PRINTERS",
                                  printer_count,
                                  ((strcmp(output_printer_records[primary_index]->prt_name, printers[related_index]->prt_name) == 0) &&
                                   (strcmp(output_printer_records[primary_index]->hostname, printers[related_index]->hostname) == 0)),
                                  PrintPrinter );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("OUTPUT TYPE/PRINTER RECORDS",
                       output_printer_record_count,
                       ((strcmp(output_printer_records[primary_index]->out_type_title, output_printer_records[related_index]->out_type_title) == 0) &&
                        (strcmp(output_printer_records[primary_index]->progroup_title, output_printer_records[related_index]->progroup_title) == 0) &&
                        (strcmp(output_printer_records[primary_index]->prt_name, output_printer_records[related_index]->prt_name) == 0) &&
                        (strcmp(output_printer_records[primary_index]->hostname, output_printer_records[related_index]->hostname) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF OUTPUT TYPE/PRINTER RECORDS: %d\n",output_printer_record_count);

}

static void CheckAppUsers()
{
  int access_role_number;
  int access_role_number_mask;
  char access_role_string[20];

  NoTab();

  Message("\n");
  Message("CHECKING APPLICATION/USER RECORDS");
  Message("---------------------------------");

  for (primary_index = 0; primary_index < appuser_count; primary_index++) {

    SetPrimaryKey(appusers[primary_index]->login_name);
    AddToPrimaryKey(appusers[primary_index]->progroup_title);

    PrintAppuser(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "APPLICATION/USER RECORD",
                                  "USER",
                                  "USERS",
                                  user_count,
                                  (strcmp(appusers[primary_index]->login_name, users[related_index]->login_name) == 0),
                                  PrintUser );

    CHECK_FOR_ONE_RELATED_RECORD( "APPLICATION/USER RECORD",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(appusers[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    /* access role number mask starts at bit 2 (0,1,2) because first two bits are now ignored */
    for (access_role_number = 1, access_role_number_mask = 4; access_role_number <= MAX_NUM_APP_ROLES_PER_PROGROUP; access_role_number++) {
      SetPrimaryKey(appusers[primary_index]->login_name);
      AddToPrimaryKey(appusers[primary_index]->progroup_title);
      sprintf(access_role_string,"ACCESS ROLE NUMBER %d",access_role_number);
      AddToPrimaryKey(access_role_string);

      if ((appusers[primary_index]->app_role & access_role_number_mask) == access_role_number_mask) {
        CHECK_FOR_ONE_RELATED_RECORD( "APPLICATION/USER RECORD",
                                      "ACCESS ROLE",
                                      "ACCESS ROLES",
                                      access_role_count,
                                      ((strcmp(appusers[primary_index]->progroup_title, access_roles[related_index]->progroup_title) == 0) &&
                                       (access_role_number == access_roles[related_index]->role_number)),
                                      PrintAccessRole );
      }

      access_role_number_mask <<= 1;

    }

    NoTab();

  }

  CHECK_FOR_DUPLICATES("APPLICATION/USER RECORDS",
                       user_count,
                       ((strcmp(appusers[primary_index]->progroup_title, appusers[related_index]->progroup_title) == 0) &&
                        (strcmp(appusers[primary_index]->login_name, appusers[related_index]->login_name) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF APPLICATION/USER RECORDS: %d\n",appuser_count);

}

static void CheckAccessRoles()
{
  NoTab();

  Message("\n");
  Message("CHECKING ACCESS ROLES");
  Message("---------------------");

  for (primary_index = 0; primary_index < access_role_count; primary_index++) {

    SetPrimaryKey(access_roles[primary_index]->progroup_title);
    AddToPrimaryKey(access_roles[primary_index]->app_role);

    PrintAccessRole(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "ACCESS ROLE",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(access_roles[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("ACCESS ROLES",
                       access_role_count,
                       ((strcmp(access_roles[primary_index]->progroup_title, access_roles[related_index]->progroup_title) == 0) &&
                        (strcmp(access_roles[primary_index]->app_role, access_roles[related_index]->app_role) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF ACCESS ROLES: %d\n",access_role_count);

}

static void CheckCurrentApps()
{
  NoTab();

  Message("\n");
  Message("CHECKING CURRENT APPLICATIONS");
  Message("-----------------------------");

  for (primary_index = 0; primary_index < current_app_count; primary_index++) {

    SetPrimaryKey(current_apps[primary_index]->progroup_title);

    PrintCurrentApp(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "CURRENT APPLICATION",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(current_apps[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    CHECK_FOR_ONE_RELATED_RECORD( "CURRENT APPLICATION",
                                  "HOST",
                                  "HOSTS",
                                  host_count,
                                  (strcmp(current_apps[primary_index]->hostname, hosts[related_index]->hostname) == 0),
                                  PrintHost );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("CURRENT APPLICATIONS",
                       current_app_count,
                       (strcmp(current_apps[primary_index]->progroup_title, current_apps[related_index]->progroup_title) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF CURRENT APPLICATIONS: %d\n",access_role_count);

}

static void CheckPrograms()
{
  NoTab();

  Message("\n");
  Message("CHECKING PROGRAMS");
  Message("-----------------");

  for (primary_index = 0; primary_index < program_count; primary_index++) {

    SetPrimaryKey(programs[primary_index]->progroup_title);
    AddToPrimaryKey(programs[primary_index]->program_title);

    PrintProgram(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "PROGRAM",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(programs[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PROGRAMS",
                       program_count,
                       ((strcmp(programs[primary_index]->progroup_title, programs[related_index]->progroup_title) == 0) &&
                        (strcmp(programs[primary_index]->program_title, programs[related_index]->program_title) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PROGRAMS: %d\n", program_count);

}

static void CheckPdjs()
{
  int access_role_number_mask;
  int access_role_number;
  char access_role_string[20];

  NoTab();

  Message("\n");
  Message("CHECKING PREDEFINED JOBS");
  Message("------------------------");

  for (primary_index = 0; primary_index < pdj_count; primary_index++) {

    SetPrimaryKey(pdjs[primary_index]->progroup_title);
    AddToPrimaryKey(pdjs[primary_index]->job_title);

    PrintPdj(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "PREDEFINED JOB",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  pdj_count,
                                  (strcmp(pdjs[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    /* access role number mask starts at bit 2 (0,1,2) because first two bits are now ignored */
    for (access_role_number = 1, access_role_number_mask = 4; access_role_number <= MAX_NUM_APP_ROLES_PER_PROGROUP; access_role_number++) {
      SetPrimaryKey(pdjs[primary_index]->progroup_title);
      AddToPrimaryKey(pdjs[primary_index]->job_title);
      sprintf(access_role_string,"ACCESS ROLE NUMBER %d",access_role_number);
      AddToPrimaryKey(access_role_string);

      if ((pdjs[primary_index]->role_access & access_role_number_mask) == access_role_number_mask) {
        CHECK_FOR_ONE_RELATED_RECORD( "PREDEFINED JOB",
                                      "ACCESS ROLE",
                                      "ACCESS ROLES",
                                      access_role_count,
                                      ((strcmp(pdjs[primary_index]->progroup_title, access_roles[related_index]->progroup_title) == 0) &&
                                       (access_role_number == access_roles[related_index]->role_number)),
                                      PrintAccessRole );
      }
      access_role_number_mask <<= 1;

    }

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PREDEFINED JOBS",
                       pdj_count,
                       ((strcmp(pdjs[primary_index]->progroup_title, pdjs[related_index]->progroup_title) == 0) &&
                        (strcmp(pdjs[primary_index]->job_title, pdjs[related_index]->job_title) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PREDEFINED JOBS: %d\n", pdj_count);

}

static void CheckPrinterAccessRecords()
{
  NoTab();

  Message("\n");
  Message("CHECKING PRINTER ACCESS RECORDS");
  Message("-------------------------------");

  for (primary_index = 0; primary_index < printer_access_record_count; primary_index++) {

    SetPrimaryKey(printer_access_records[primary_index]->prt_name);
    AddToPrimaryKey(printer_access_records[primary_index]->hostname);

    PrintPrinterAccessRecord(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_FOR_ONE_RELATED_RECORD( "PRINTER ACCESS RECORD",
                                  "PROGRAM GROUP",
                                  "PROGRAM GROUPS",
                                  progroup_count,
                                  (strcmp(printer_access_records[primary_index]->progroup_title, progroups[related_index]->progroup_title) == 0),
                                  PrintProgroup );

    CHECK_FOR_ONE_RELATED_RECORD( "PRINTER ACCESS RECORD",
                                  "PRINTER",
                                  "PRINTERS",
                                  printer_count,
                                  ((strcmp(printer_access_records[primary_index]->prt_name, printers[related_index]->prt_name) == 0) &&
                                   (strcmp(printer_access_records[primary_index]->hostname, printers[related_index]->hostname) == 0)),
                                  PrintPrinter );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PRINTER ACCESS RECORDS",
                       printer_access_record_count,
                       ((strcmp(printer_access_records[primary_index]->progroup_title, printer_access_records[related_index]->progroup_title) == 0) &&
                        (strcmp(printer_access_records[primary_index]->prt_name, printer_access_records[related_index]->prt_name) == 0) &&
                        (strcmp(printer_access_records[primary_index]->hostname, printer_access_records[related_index]->hostname) == 0)));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PRINTER ACCESS RECORDS: %d\n",printer_access_record_count);

}


static void CheckPrinterClasses()
{
  NoTab();

  Message("\n");
  Message("CHECKING PRINTER CLASSES");
  Message("------------------------");

  for (primary_index = 0; primary_index < printer_class_count; primary_index++) {

    SetPrimaryKey(printer_classes[primary_index]->prt_class_title);

    PrintPrinterClass(primary_index, FULL_FORMAT, 0);

    Tab();

    CHECK_RELATED_RECORDS( "PRINTERS",
                           printer_count,
                           (strcmp(printer_classes[primary_index]->prt_class_title, printers[related_index]->prt_class_title) == 0),
                           PrintPrinter,
                           LIST_FORMAT );

    NoTab();

  }

  CHECK_FOR_DUPLICATES("PRINTER CLASSES",
                       printer_class_count,
                       (strcmp(printer_classes[primary_index]->prt_class_title, printer_classes[related_index]->prt_class_title) == 0));

  if (print_data) fprintf(stdout,"TOTAL NUMBER OF PRINTER CLASSES: %d\n",printer_class_count);

}


static void ReadDatabase()
{
  progroups       = getAppData( &progroup_count, db_dir, WITH_NTCSS_GROUP );
  pdjs            = getPredefinedJobData( &pdj_count, db_dir );
  hosts           = getHostData( &host_count, db_dir );
  output_types    = getOutputTypeData( &output_type_count, db_dir );
  appusers        = getAppuserData( &appuser_count, db_dir, WITH_NTCSS_GROUP );
  programs        = getProgramData( &program_count, db_dir, WITH_NTCSS_GROUP );
  access_roles    = getAccessRoleData( &access_role_count, db_dir );
  users           = getUserData( &user_count, db_dir );
  current_apps    = getCurrentAppData( &current_app_count, db_dir );
  printers        = getPrtDataFromDb( &printer_count, db_dir, NULL, 0);
  adapters        = getAdapterDataFromDb( &adapter_count, db_dir );
  printer_classes = getPrtclassData( &printer_class_count, db_dir );
  getPrinterAccessData( db_dir ); 
  getOutputPrinterData( db_dir );
}

static void FreeMemory()
{
  freeAppData( progroups, progroup_count );
  freePredefinedJobData( pdjs, pdj_count );
  freeHostData( hosts, host_count );
  freeOutputTypeData( output_types, output_type_count );
  freeAppuserData( appusers, appuser_count );
  freeProgramData( programs, program_count );
  freeAccessRoleData( access_roles, access_role_count );
  freeUserData( users, user_count );
  freeCurrentAppData( current_apps, current_app_count );
  freePrtData( printers, printer_count );
  freeAdapterData( adapters, adapter_count );
  freePrinterAccessData();
  freeOutputPrinterData();
}

static void Usage(program_name)
     char *program_name;
{
  printf("USAGE\n"
         "%s [-nodata | -noerror] [-noone] [-nomany] [-nodup] [-nofield] [-db_dir=<path>] <table specfication> \n"
         "<table specification> is one or more of the following:\n"
         "  -all            -pdjs          -printers \n"
         "  -progroups      -access_roles  -prt_class\n"
         "  -current_apps   -appusers      -output_prt\n"
         "  -programs       -hosts         -adapters\n"
         "  -output_types   -users         -printer_access\n",program_name);
  
  printf("\nWHERE\n"
         "  -nodata                  - Turns off printing out the database (default on).\n"
         "  -noerror                 - Turns off printing error messages (default on).\n"
         "  -noone                   - Turns off checking n to one relationsships (default on) (n>=1).\n");
  printf("  -nomany                  - Turns off checking one to many relationships (default on).\n"
         "  -nofield                 - Turns off field checking for all fields in database (default on).\n"
         "  -nodup                   - Turns off checking for duplicates in the same table (default on).\n");
  printf("  -noconnect               - Turns off checking if host exists by connecting a socket to that host (default on).\n"
         "  -port=<portnum>          - Sets the port number for checking if host exists (default is port 13).\n"
         "  -db_dir=<path>           - Sets the path to the NTCSS database (default is the live database).\n"
         "  -all                     - Checks all tables.\n"
         "  -other table specifiers  - Check the respective table.\n");

  printf("\nNOTES\n"
         "  - Options may appear in any order.\n"
         "  - There must be at least one table specifier.\n"
         "  - When specifying -db_dir=<path>, do not put spaces around = (see examples).\n"
         "  - You might have to be logged in as root to successfully run this program.\n"
         "  - Record numbers are not neccessarily persistent from one run of this program to the next.\n");
  
  printf("\nEXAMPLES\n");
  printf("%s -all\n",program_name);
  printf("%s -all -db_dir=mycopy/database\n",program_name);
  printf("%s -nodata -all\n",program_name);
  printf("%s -noerror -all\n",program_name);
  printf("%s -progroups -access_roles -current_apps -programs -appusers -pdjs -hosts -output_types\n",program_name);
  printf("%s -users -appusers\n",program_name);
  printf("%s -printers -adapters -hosts -printer_access -prt_class -output_prt\n",program_name);
  printf("%s -nofield -nodup -printers -adapters -hosts -printer_access -prt_class -output_prt\n",program_name);
  printf("\n");
  printf("Hint: Use these examples in scripts to process groups of tables (e.g., all tables, \n"
         "      progroup related tables, user related tables, printer related tables, etc.).\n");

  Quit(1);
}

int main (argc, argv)
     int argc;
     char **argv;
{
  int i;

  if (SystemSettingsAreInitialized() == FALSE) {
    if (loadNtcssSystemSettingsIntoShm() == 0) {
      opened_system_settings = 1;
    } else {
      printf("Error loading system settings.\n");
      exit(1);
    }
  }

  strcpy(db_dir, NTCSS_DB_DIR);

  fprintf(stdout,"NTCSS DATABASE INTEGRITY CHECK\n\n");

  fprintf(stdout,"command line: ");

  for (i=0; i < argc; i++) {

    fprintf(stdout,"%s ",argv[i]);

    CHECK_ARG("-nodata",           print_data = 0);
    CHECK_ARG("-noerror",          print_errors = 0);
    CHECK_ARG("-nomany",           check_many = 0);
    CHECK_ARG("-nodup",            check_duplicates = 0);
    CHECK_ARG("-noone",            check_one = 0);
    CHECK_ARG("-nofield",          check_fields = 0);
    CHECK_ARG("-noconnect",        check_connection = 0);
    CHECK_ARG("-port=",            port = atoi(argv[i]+strlen("-port=")));
    CHECK_ARG("-db_dir=",          strcpy(db_dir, argv[i]+strlen("-db_dir=")));
    CHECK_ARG("-all",              check_something = check_all = 1);
    CHECK_ARG("-progroups",        check_something = check_progroups = 1);
    CHECK_ARG("-appusers",         check_something = check_appusers = 1);
    CHECK_ARG("-users",            check_something = check_users = 1);
    CHECK_ARG("-access_roles",     check_something = check_access_roles = 1);
    CHECK_ARG("-current_apps",     check_something = check_current_apps = 1);
    CHECK_ARG("-programs",         check_something = check_programs = 1);
    CHECK_ARG("-pdjs",             check_something = check_pdjs = 1);
    CHECK_ARG("-hosts",            check_something = check_hosts = 1);
    CHECK_ARG("-printer_access",   check_something = check_printer_access_records = 1);
    CHECK_ARG("-printer_adapters", check_something = check_printer_adapters = 1);
    CHECK_ARG("-printer_classes",  check_something = check_printer_classes = 1);
    CHECK_ARG("-output_prt",       check_something = check_output_printer_records = 1);
    CHECK_ARG("-printers",         check_something = check_printers = 1);
    CHECK_ARG("-output_types",     check_something = check_output_types = 1);
  }

  fprintf(stdout,"\n\n");

  if ((!print_data && !print_errors)) {
    fprintf(stdout,"Error in command line - both -nodata and -noerror were specified.\n\n");
    Usage(argv[0]);
  }

  if (!check_something) {
    fprintf(stdout,"Error in command line - either invalid or no table specification.\n\n");
    Usage(argv[0]);
  }

  ReadDatabase();

  CHECK_TABLE( check_progroups,              CheckProgroups());
  CHECK_TABLE( check_users,                  CheckUsers());
  CHECK_TABLE( check_appusers,               CheckAppUsers());
  CHECK_TABLE( check_access_roles,           CheckAccessRoles());
  CHECK_TABLE( check_current_apps,           CheckCurrentApps());
  CHECK_TABLE( check_programs,               CheckPrograms());
  CHECK_TABLE( check_pdjs,                   CheckPdjs());
  CHECK_TABLE( check_hosts,                  CheckHosts());
  CHECK_TABLE( check_printer_access_records, CheckPrinterAccessRecords());
  CHECK_TABLE( check_printer_adapters,       CheckPrinterAdapters());
  CHECK_TABLE( check_printer_classes,        CheckPrinterClasses());
  CHECK_TABLE( check_output_printer_records, CheckOutputPrinterRecords());
  CHECK_TABLE( check_printers,               CheckPrinters());
  CHECK_TABLE( check_output_types,           CheckOutputTypes());

  FreeMemory();

  Quit(0);
  
  return(0);
}
