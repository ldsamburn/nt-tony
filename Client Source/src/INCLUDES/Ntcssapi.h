/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// CHANGES LOGGED HERE - started with 9/12/96 baseline
//
// DATE
// ----
// 9/12/96 NtcssPrintServerFile, changed comments
// 9/16/96 NtcssGetCommonData, added FAR to common_data_buf parm
//		   NtcssGetDateTime,  added FAR to date_time_buf parm
//		   NtcssGetHostName,  added FAR to host_name_buf parm
//		   NtcssGetLastError,  added FAR to err_buf parm
//		   NtcssGetLocalIP,  added FAR to ip_addr_buf parm
//		   NtcssGetServerName,  added FAR to server_name_buf parm
//		   NtcssGetServerName,  added FAR to server_name_buf parm
//		   NtcssPrintClientFile,  added FAR to file_name_str and to
//									output_type_str parms
//		   NtcssPrintServerFile,  added FAR to server_name_str and to
//									output_type_str, dir_name_str parms
//		   NtcssRegisterAppUser,  added FAR to user_name_str parm
//		   NtcssScheduleServerProcess,  added FAR to schedule_str parm
//		   NtcssSelectPrinter,  added FAR to pr_attr_ptr parm
//		   NtcssSetAppData,  added FAR to app_data_str parm
//		   NtcssStartServerProcess,  added FAR to descr_ptr and to
//									pid_buf parms
// 
// 12/17/96 NtcssStartServerProcess, adding SDI stuff to 
//									NtcssServerProcessDescr
// 03/27/97 NtcssGetLinkData,       added as new API
// 03/28/97 NtcssSetLinkData,       added as new API
// 03/28/97 NtcssValidateUser,      added as new API 
// 06/05/97 NtcssGetFile            added as new API
// 06/05/97 NtcssPutFile            added as new API
// 07/07/97 NtcssGetFileInfo        added as new API
// 07/10/97 NtcssGetFileSection     added as new API
// 08/05/97 NtcssViewServerFile     added as new API
// 08/07/97 NtcssPrintServerFile    added 2 new parameters.#of copies 
//                                  & options
// 08/18/97 NtcssGetFilteredServerProcessList    added as new API
// 08/18/97 NtcssGetFilteredServerProcessListCt  added as new API
// 05/14/98 NtcssPutFileAppend		  added as new API
// 08/31/99 NtcssSelectServerFile     added as new API

// Real top of file, next line:
// ntcssapi.h  - Defines APIs for the NTCSS DLL
//
/***************************** NTCSSII *****************************/
/**                                                               **/
/**  Developed by Inter-National Research Institute, Inc. under   **/
/**  contract to the Department of Defense.  All U.S. Government  **/
/**  restrictions on software distributions apply.                **/
/**                                                               **/
/**       Copyright 1994, Inter-National Research Institute       **/
/**                      All Rights Reserved.                     **/ 
/**                                                               **/
/*******************************************************************/


#ifndef _DEFINE_NTCSSAPI_H_
#define _DEFINE_NTCSSAPI_H_  
                                       // Identifies current version
                                       //  of NTCSS DLL
#define NTCSS_DLL_VERSION       "1.0.1"

                                       // If DllImport not already setup
#ifndef DllImport
                                       // Win32 specific declarations
#ifdef WIN32

#define DllImport __declspec(dllimport) WINAPI

                                       // Else is for 16-bit
#else

#define DllImport FAR PASCAL

#endif // WIN32
#endif // DllImport


                                       // These constants are used in NTCSS DLL
                                       //  API function calls requiring
                                       //  security level settings
                                       //
#define NTCSS_UNCLASSIFIED             0
#define NTCSS_UNCLASSIFIED_SENSITIVE   1
#define NTCSS_CONFIDENTIAL             2
#define NTCSS_SECRET                   3
#define NTCSS_TOPSECRET                4



                                       // These defines represent the
									   //  three levels of server process
									   //  priority supported by NTCSS
                                       
#define NTCSS_LOW_PRIORITY             1
#define NTCSS_MED_PRIORITY             2
#define NTCSS_HIGH_PRIORITY            3

                                       // These defines are used in stopping
									   //  a server process.  A hard stop
									   //  will use a signal to abrubtly
									   //  stop the process, a soft stop
									   //  uses a signal that allows the
									   //  process to terminate in a more
									   //  controlled fashion
                                       
#define NTCSS_STOP_PROCESS_SOFT             0
#define NTCSS_STOP_PROCESS_HARD             1

                                       // These defines are used for the
									   //  ServerProcessDetails's job_type_code
									   //  (see below) to indicate
									   //  whether a server process is
									   //  scheduled or not
                                       
#define NTCSS_NON_SCHEDULED_JOB                 0
#define NTCSS_SCHEDULED_JOB                     1

                                       // These defines are used for the
									   //  ServerProcessDetails's print_code
									   //  (see below) to indicate
									   //  how process printing is to be
									   //  handled
                                       
#define NTCSS_PRINT_NOT			    0
#define NTCSS_PRINT_AND_REMOVE		1
#define NTCSS_PRINT_AND_SAVE        2

                                       //These defines are used for the Get & Put
                                       //
#define NTCSS_ASCII_FTP             TRUE
#define NTCSS_BINARY_FTP            FALSE

                                       // These constants are used as sizes
                                       //  for parameter buffers and/or
                                       //  structure fields used in NTCSS DLL
                                       //  API function calls
#define NTCSS_SIZE_APP_DATA				256
#define NTCSS_SIZE_APP_FLAG				2
#define NTCSS_SIZE_APP_PROGRAM_INFO		320         
#define NTCSS_SIZE_APP_GROUP			16
#define NTCSS_SIZE_APP_PASSWORD			32
#define NTCSS_SIZE_CLIENT_LOCATION		128
#define NTCSS_SIZE_CLIENT_WS			16
#define NTCSS_SIZE_COMMON_DATA			128
#define NTCSS_SIZE_COMMON_TAG			64
#define NTCSS_SIZE_COMMAND_LINE			256
#define NTCSS_SIZE_CUSTOM_PROC_STAT		128
#define NTCSS_SIZE_DATETIME1			27  
#define NTCSS_SIZE_DATETIME2			13  
#define NTCSS_SIZE_FILENAME				32
#define NTCSS_SIZE_GROUP_TITLE			16
#define NTCSS_SIZE_HOSTNAME				16
#define NTCSS_SIZE_JOB_DESCRIP			256        
#define NTCSS_SIZE_IP_ADDR				16
#define NTCSS_SIZE_LOGINNAME			8                 
#define NTCSS_SIZE_MSG_BOARD_NAME		16
#define NTCSS_SIZE_ORIG_HOST			32                 
#define NTCSS_SIZE_PASSWORD				32
#define NTCSS_SIZE_PHONENUMBER			32
#define NTCSS_SIZE_PRINTERNAME			16
#define NTCSS_SIZE_PROCESS_ARGS			128
#define NTCSS_SIZE_PROCESS_ID			32
#define NTCSS_SIZE_PRT_OUTPUT_TYPE		16
#define NTCSS_SIZE_REQUEST_ID			4
#define NTCSS_SIZE_SDI_CONTROL_REC_ID	5
#define NTCSS_SIZE_SERVER_LOCATION		128
#define NTCSS_SIZE_SSN					11
#define NTCSS_SIZE_USERFIRSTNAME		32
#define NTCSS_SIZE_USERLASTNAME			32
#define NTCSS_SIZE_USERMIDDLENAME		32


									   // Max length constants
#define NTCSS_MAX_ERROR_MSG_LEN     256
#define NTCSS_MAX_MESSAGE_LEN       512
#define NTCSS_MAX_MESSAGE_TITLE_LEN  32
#define NTCSS_MAX_SCHEDULE_LEN 128
#define NTCSS_MAX_PRT_COPIES_LEN 2
#define NTCSS_MAX_PRT_OPTIONS_LEN 255

                                       // Maximum number of users
                                       //  allowed for by NTCSS
#define NTCSS_MAX_NUMBER_USERS      256





/*
NtcssAppUserList structure
--------------------------

This structure is used in the NTCSS DLL NtcssGetAppUserList()
function to return a list of users for the current NTCSS Application. 

This structure contains a single user name (appuser_name) and a
flag field to indicate whether the user is registered with the
application.  A setting of TRUE for appuser_reg_flag indicates that
the user specified in the appuser_name is registered for the application,
FALSE indicates the user is not registered.  Note that a successful call 
to NtcssGetAppUserList() will result in one or more of these structures 
written successively into a user supplied buffer.
*/

typedef struct NtcssAppUserList
{
    char appuser_name[NTCSS_SIZE_LOGINNAME];
    BOOL appuser_reg_flag;
} NtcssAppUserList;



/*
NtcssUserInfo structure
-----------------------

This structure is used in the NTCSS DLL NtcssGetAppUserInfo and
NtcssGetUserInfo API function calls to return NtcssGetUserInfo API,
the app_passwd fill does not apply and will be filled with spaces.  
When returned from the Ntcss Note that character string buffers for 
this structure will be NULL terminated UNLESS values for fields 
completely fill the field (e.g. if the login_name is a full 
NTCSS_SIZE_LOGINNAME characters in length then it will not be
NULL terminated).
*/                                         

typedef struct NtcssUserInfo
{
   char          login_name[NTCSS_SIZE_LOGINNAME];
   char          real_first_name[NTCSS_SIZE_USERFIRSTNAME];
   char          real_middle_name[NTCSS_SIZE_USERMIDDLENAME];
   char          real_last_name[NTCSS_SIZE_USERLASTNAME];
   char          ssn[NTCSS_SIZE_SSN];
   char          phone_number[NTCSS_SIZE_PHONENUMBER];
   char          app_passwd[NTCSS_SIZE_APP_PASSWORD];
   int           security_class;
   int           unix_id;
} NtcssUserInfo;



/*
NtcssPrintAttrib structure
--------------------------

The NtcssPrintAttrib structure is used in the NTCSS DLL SelectPrinter() 
function to specify printer setup information. 
*/

								       // These Microsoft Windows 
									   //  GDI constant definitions 
									   //  are repeated here as they
									   //  are required to assign
									   //  PrintAttrib field values
#ifndef _WINGDI_

#define DMORIENT_PORTRAIT   1
#define DMORIENT_LANDSCAPE  2

#define DMPAPER_LETTER      1  // Letter 8 1/2 x 11 in               
#define DMPAPER_LEGAL       5  // Legal 8 1/2 x 14 in          

#endif


typedef struct NtcssPrintAttrib
{
   int  orient;        // DMORIENT_PORTRAIT OR DMORIENT_LANDSCAPE
   BOOL banner;        // TRUE if banner should be printed
   int  copies;        // Number of copies
   int  paper_size;    // DMPAPER_LETTER or DMPAPER_LEGAL

} NtcssPrintAttrib;



/*
NtcssServerProcessDescr structure
---------------------------------

This structure is used in passing server process information to the
NtcssStartServerProcess and NtcssScheduleServerProcess APIs.  The fields
of this structure should be set as follows when making calls to these
APIs:

owner_str -
	A NULL terminated string giving the NTCSS login name of the 
	individual responsible for starting the server process.  This field
	is used when process approval is being performed and the user
	approving (and thereby submitting the job via NtcssStartServerProcess()
	or NtcssScheduleServerProcess()) is different from the user who
	should be assigned as owner of the process when it executes on the
	server.  If the owner_str is NULL then the current user (as 
	determined by the current NTCSS login name) will be responsible for
	the process recorded as its owner.  

prog_name_str - 
	A NULL terminated string giving the path for the server process that is 
	to be started; the prog_name_str may not be NULL

prog_args_str - 
	A NULL terminated string giving the arguments for the server process that 
	is to be started; prog_args_str may be NULL when the server process 
	does not requireany arguments

job_descrip_str - 
	A NULL terminated string containing text used to describe the server 
	process that is to be started; this information will be made available 
	for NTCSS and application process approval and scheduling 
	functions; the job_descrip_str may be NULL

seclvl - 
	security level for the server process to be started; range is
	NTCSS_UNCLASSIFIED ... NTCSS_TOPSECRET

priority_code -
	the priority level for the server process to be started; 
	NTCSS supports three levels of priority for server processes:
	NTCSS_LOW_PRIORITY, NTCSS_MED_PRIORITY and NTCSS_HIGH_PRIORITY 

needs_approval_flag - 
	TRUE if the server process to be started must be approved before it 
	can begin, FALSE otherwise; an approval requires operator/
	administrator intervention and serves as a mechanism to allow 
	system load and process monitoring

needs_device_flag - 
	TRUE if the server process to be started requires special device 
	capabilities, FALSE otherwise;  similarly to the needs_approval_flag, 
	a TRUE setting for needs_device_flag will require operator/
	administrator intervention to verify device availability before the 
	server process may begin

restartable_flag -
	TRUE if the server process to be started should be allowed to
	be restarted should it terminate before successful completion.  Any
	such restarting will require operator/administrator intervention.
	If restartable_flag is FALSE then such restarting will not be allowed
	for the server process.

prt_output_flag - 
	Set to TRUE to indicate that the server process to be started does 
	require printing of output.  If set to FALSE no output printing is 
	done and all following fields in this structure are ignored

prt_filename_str -
	A NULL terminated string giving the name of file that is to receive 
	output from the server process

output_type_str -  
	A NULL terminated string used to associate applications with 
	printers.  The setting of output_type_str determine printers that 
	will be available for printing the server process output; 
	output_type_str is only valid if prt_output_flag is TRUE 

use_default_prt_flag - 
	Set to TRUE to indicate that output from the server process should 
	be printed on the output type's default printer; if 
	use_default_prt_flag is FALSE then a printer selection dialog is 
	displayed to allow specification of the printer to receive the 
	server process output; use_default_prt_flag is only valid if 
	prt_output_flag is TRUE 

general_access_prt_flag - 
	Set to TRUE to indicate that only general access printers will be 
	eligible to receive output from the server process; 
	general_access_prt_flag is only valid if prt_output_flag is
	TRUE and use_default_prt_flag is FALSE; when general_access_prt_flag
	is TRUE a printer selection dialog is displayed to allow 
	specification of a general access printer to receive the server
	process output

save_prt_output_flag - 
	Set to TRUE to indicate that output from the server process is NOT 
	to be deleted when printing is completed; save_prt_output_flag is 
	valid only when prt_output_flag is TRUE; if prt_output_flag is TRUE 
	and save_prt_output_flag is FALSE then server process output is 
	deleted after it has been printed.

sdi_required_flag -
	Set to TRUE to indicate that SDI input or output Is to be 
	performed for the server process.

sdi_input_flag -
	Valid only if sdi_required_flag is set to TRUE.  If sdi_input_flag
	is TRUE then the SDI is used to read tape data which will be 
	provided as input to the server process.  If sdi_input_flag is set 
	to FALSE then the SDI is used to write data output by the server 
	process to tape.

sdi_control_record_id_str -
	Valid only if sdi_required_flag is set to TRUE.  Identifies
	the particular SDI Tape Control Record File entry which 
	describes the tape data format to use for the server process SDI 
	input or output that is to be performed.

sdi_data_file_str -
	Valid only if sdi_required_flag is set to TRUE.  
	The sdi_data_file_str will specify a complete server path and file
	name.  If sdi_input_flag is TRUE then prior to starting the server 
	process the SDI is used to read data, in the 
	format indicated by	sdi_control_record_id_str, into the file 
	indicated by sdi_data_file_str.  If sdi_input_flag is FALSE then 
	once the server process successfully completes, the file created by
	the	process, whose name is indicated by sdi_data_file_str, will be
	written to tape using the SDI.
	
sdi_tci_file_str
	Valid only if sdi_required_flag is set to TRUE and sdi_input_flag
	is set to FALSE.  Indicates	the name of the SDI tape control
	intermediate file to use in creating the tape.


nOrient
    Valid only if prt_output_flag is TRUE. PORTRAIT=1 LANDSCAPE=2.

bBanner
	Valid only if prt_output_flag is TRUE. BOOL set to control if a
	banner is attached to print job.

nCopies
	Valid only if prt_output_flag is TRUE. Int set to control number
	of copies printed.

nPaperSize
	Valid only if prt_output_flag is TRUE. Determines paper size
	LETTER=1 LEGAL=5

*/

typedef struct NtcssServerProcessDescr			  
{
    char owner_str[NTCSS_SIZE_LOGINNAME];
    char prog_name_str[NTCSS_SIZE_SERVER_LOCATION];
    char prog_args_str[NTCSS_SIZE_PROCESS_ARGS];
    char job_descrip_str[NTCSS_SIZE_JOB_DESCRIP];
	int  seclvl;
	int  priority_code;
	BOOL needs_approval_flag;
	BOOL needs_device_flag;
	BOOL restartable_flag;
	BOOL prt_output_flag;
	char prt_filename_str[NTCSS_SIZE_SERVER_LOCATION];
    char output_type_str[NTCSS_SIZE_PRT_OUTPUT_TYPE];
    BOOL use_default_prt_flag;
    BOOL general_access_prt_flag;
    BOOL save_prt_output_flag;
    BOOL sdi_required_flag;
    BOOL sdi_input_flag;
    char sdi_control_record_id_str[NTCSS_SIZE_SDI_CONTROL_REC_ID+1];
    char sdi_data_file_str[NTCSS_SIZE_SERVER_LOCATION];
    char sdi_tci_file_str[NTCSS_SIZE_SERVER_LOCATION];
	int  nOrient;        // DMORIENT_PORTRAIT 1 OR DMORIENT_LANDSCAPE 2
    BOOL bBanner;        // TRUE if banner should be printed
	int  nCopies;
    int  nPaperSize;    // DMPAPER_LETTER 1 or DMPAPER_LEGAL 5
} NtcssServerProcessDescr; 


/*
NtcssServerProcessDetails structure
-----------------------------------
                
This structure is used in to return information from the
NtcssGetServerProcessList API.  Upon successful completion of
NtcssGetServerProcessList, data is returned to the caller in
ServerProcessDetails records.  The fields of the ServerProcessDetails
are set by NtcssGetServerProcessList to descibe a server process as 
follows:

pid -
	Unique UNIX process id

process_id_str -
	Unique NTCSS process id

seclvl - 
	security level for the server process; range is
	NTCSS_UNCLASSIFIED ... NTCSS_TOPSECRET

priority_code -
	the priority level for the server process; 
	NTCSS supports three levels of priority for server processes:
	NTCSS_LOW_PRIORITY, NTCSS_MED_PRIORITY and NTCSS_HIGH_PRIORITY 

status_code -
	the integer exit status for the server process;  this value is
	undefined until after ther server process has completed execution.

job_type_code -
	set to NTCSS_SCHEDULED_JOB for a scheduled process or to 
	NTCSS_NON_SCHEDULED_JOB if no schedule is associated with the
	process

restartable_flag -
	set to TRUE if the process can be restarted if it fails to complete
	successfully, otherwise FALSE to indicate that no efforts will be
	made to restart the process if it fails to successfully complete

login_name_str -
	the NTCSS login name of the individual responsible for starting
	the server process;  the login_name_str will be NULL terminated
	unless the name is a full NTCSS_SIZE_LOGINNAME characters in length

unix_owner_str -
	Unix login name of user who started the process; the unix_owner_str
	will be NULL terminated unless the name is a full 
	NTCSS_SIZE_LOGINNAME characters in length

app_name_str -
	String containing application name for the process; the app_name_str
	will be NULL terminated unless it is a full NTCSS_SIZE_GROUP_TITLE
	bytes in length

char command_line_str -
	NULL terminated string containing the command line used for starting
	the server process

custom_process_stat_str -
	A buffer used by the server process in an application defined
	manner (i.e. not set by NTCSS).

job_descrip_str -
	A NULL terminated string containing text used to describe the server 
	process that is to be started; this information is used 
	for NTCSS and application process approval and scheduling 
	functions; the job_descrip_str may be NULL

request_time -
	Null terminated string giving time that server process execution
	was first requested.  Time format is YYMMDD.hhmmss where YY is
	hours, MM is month, DD is date, hh is hour, mm is minutes and ss is
	seconds.

launch_time -
	Null terminated string giving time that server process actually
	began execution.  Time format is same as for request_time.

end_time -
	NULL terminated string giving time that server process 
	completed execution.  Time format is same as for request_time.

orig_host_str -
	A NULL terminated string specifying the IP address for the host
	which originated the server process.

printer_name_str -
	NULL terminated string giving name of the printer on which output from
	the server process will be printed.

printer_host_name_str -
	NULL terminated string giving name of the host which has the printer 
	(given by printer_name_str)  on which output from the server 
	process will be printed.

print_seclvl -
	Same as seclvl above.

prt_filename_str -
	NULL terminated string giving name of file that is to receive output
	from the server process

print_code -
	Indicates how printing and the print output file will be handled
	for the server process.  Valid values are NTCSS_PRINT_NOT,
	NTCSS_PRINT_AND_REMOVE and NTCSS_PRINT_AND_SAVE.


sdi_control_record_id_str -
	Identifies	the particular SDI Tape Control Record File entry which 
	describes the tape data format to use for the server process SDI 
	input or output that is to be performed.

sdi_data_file_str -
	The sdi_data_file_str will specify a complete server path and file
	name.  If sdi_input_flag is TRUE then prior to starting the server 
	process the SDI is used to read data, in the 
	format indicated by	sdi_control_record_id_str, into the file 
	indicated by sdi_data_file_str.  If sdi_input_flag is FALSE then 
	once the server process successfully completes, the file created by
	the	process, whose name is indicated by sdi_data_file_str, will be
	written to tape using the SDI.
	
sdi_tci_file_str -
	Indicates the name of the SDI tape control
	intermediate file to use in creating the tape.


sdi_device_data_file_str - 
    The sdi_data_file_str will specify a complete server path and device
	name.


	
*/

typedef struct NtcssServerProcessDetails
{
    int  pid;								
    char process_id_str[NTCSS_SIZE_PROCESS_ID];	
    int  seclvl;							
    int  priority_code;						
	                                        
    int  status_code;						
    int  job_type_code;						
	BOOL restartable_flag;
											
    char login_name_str[NTCSS_SIZE_LOGINNAME];
										    
    char unix_owner_str[NTCSS_SIZE_LOGINNAME];
											
    char app_name_str[NTCSS_SIZE_GROUP_TITLE];
											 
    char command_line_str[NTCSS_SIZE_COMMAND_LINE];
    char custom_process_stat_str[NTCSS_SIZE_CUSTOM_PROC_STAT];
    char job_descrip_str[NTCSS_SIZE_JOB_DESCRIP];

    char request_time[NTCSS_SIZE_DATETIME2];
    char launch_time[NTCSS_SIZE_DATETIME2];
    char end_time[NTCSS_SIZE_DATETIME2];

    char orig_host_str[NTCSS_SIZE_ORIG_HOST];

    char printer_name_str[NTCSS_SIZE_PRINTERNAME];
    char printer_host_name_str[NTCSS_SIZE_HOSTNAME];
    int  print_seclvl;
    char prt_filename_str[NTCSS_SIZE_CLIENT_LOCATION];
    int  print_code;
	char sdi_control_record_id_str[NTCSS_SIZE_SDI_CONTROL_REC_ID+1];
    char sdi_data_file_str[NTCSS_SIZE_SERVER_LOCATION];
    char sdi_tci_file_str[NTCSS_SIZE_SERVER_LOCATION];
	char sdi_device_data_file_str[NTCSS_SIZE_SERVER_LOCATION];
	int  orientation;
	int  banner;
	int  copies;
	int  PaperSize;
	

} NtcssServerProcessDetails; 


/*this structure is used to precisley filter the return information from 
the NtcssGetFilteredServerProcessList API. Upon successful completion of
NtcssGetFilteredServerProcessList, data is returned to the caller in
ServerProcessDetails records. The NtcssProcessFilter structure is defined as
follows:

get_scheduled_jobs -
	BOOL value indicates if scheduled jobs should be counted in query.

get_unscheduled_jobs -
	BOOL value indicates if unscheduled jobs should be counted in query.

get_only_running_jobs - 
	BOOL value indicates if only running jobs should be counted in query.

get_for_current_user_only - 
	BOOL value indicates if jobs for current user only should be counted in query.

priority - 
	int value indicates if only jobs this priority should be counted in query.

command_line - 
	const char value indicates if jobs with this command line only should be counted in query.

cust_proc_status - 
	const char value indicates if jobs with this costume proc status only should be counted in query.

login_name  - 
	const char value indicates if jobs with this login name only should be counted in query.

unix_owner  - 
	const char value indicates if jobs with this unix owner only should be counted in query.

progroup_title - 
	const char value indicates if jobs with this progroup title only should be counted in query.

job_descrip - 
	const char value indicates if jobs with this job descrip title only should be counted in query.

orig_host - 
	const char value indicates if jobs with this originating host only should be counted in query.



*/
typedef struct NtcssProcessFilter 
{

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
  char command_line[NTCSS_SIZE_COMMAND_LINE];
  char cust_proc_status[NTCSS_SIZE_CUSTOM_PROC_STAT];
  char login_name[NTCSS_SIZE_LOGINNAME];
  char unix_owner[NTCSS_SIZE_LOGINNAME];
  char progroup_title[NTCSS_SIZE_APP_GROUP];
  char job_descrip[NTCSS_SIZE_JOB_DESCRIP];
  char orig_host[NTCSS_SIZE_ORIG_HOST];

} NtcssProcessFilter;

#ifndef NTCSS_DLL_BUILD



#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */



/* ***********
 * NtcssAssert
 * ___________
 *
 *PARAMETERS:   assert_expr_val : Passed as an BOOL but is usually the
 *                    result of an expression (see description).
 *
 *              message_str : A pointer to a NULL-terminated character
 *                     string that is to be dislayed in the NtcssAssert message box.
 *
 *DESCRIPTION:  If assert_expr_val is TRUE then function does nothing.
 *    If assert_expr_val is FALSE then this function displays a message box
 *    containing message_text and gives the user the option of aborting the
 *    program or continuing.  If the user selects to abort then this function
 *    will terminate the current application.  If the user elects to continue
 *    then control returns to the caller.  This function is usually called
 *    with the assert_expr_val being the result of an expression, for 
 *    example:
 *              NtcssAssert( (0 <= idx) && (idx < MAX_SIZE),
 *                          "Attempt to index out of bounds");
 */

void DllImport NtcssAssert
		(const BOOL assert_expr_val,
         const char *message_str);


/* *******************
 * NtcssCreateSchedule
 * ___________________
 *
 *PARAMETERS:   schedule_buf : A pointer to a buffer to which will be
 *                   stored NTCSS schedule data.  Buffer should be 
 *                   large enough to contain NTCSS_MAX_SCHEDULE_LEN 
 *                   bytes.
 *
 *              schedule_buf_size: size of schedule_buf buffer
 *
 *DESCRIPTION:  This function will display the NTCSS Schedule Selection
 *              dialog to allow the user to specify a schedule.  Refer 
 *              to TBD for details on the operation and use of the 
 *              NTCSS Schedule Selection dialog.  If successful TRUE 
 *              will be returned and data representing the selected
 *              NTCSS schedule will be written to the schedule_buf.
 *
 *              This function uses message boxes to display error 
 *              information to the user regardless of the current 
 *              NtcssErrorMessagingStatus() setting.  
 * 
 *              Note that when FALSE is returned the calling program 
 *              can determine if the reason that this function failed 
 *              to create a scedhule was due to error by checking 
 *              NtcssGetLastError().  The string written to by 
 *              NtcssGetLastError() will be NULL if the user cancelled 
 *              the create schedule dialog.
 */

BOOL DllImport NtcssCreateSchedule
		(char FAR  *schedule_buf,
         const int schedule_buf_size);



/* ***********************
 * NtcssDeregisterSecurity
 * _______________________
 *
 *PARAMETERS:   hInstance: Handle to the calling application
 *
 *RETURNS:      TRUE:      If successful
 *              FALSE:     Otherwise
 *
 *DESCRIPTION:  Notifies NTCSS Desktop Environment that the current
 *              application is exitting and to clear any previous security
 *              requests.  The NTCSS Security Banner will be reset to
 *              display the highest security level for the 
 *              remaining registered applications or "Unclassified" 
 *              if the NTCSS Desktop is the only registered application.
 */

BOOL DllImport NtcssDeregisterSecurity
		(const HANDLE hInstance);



/* ********************
 * NtcssDisplaySchedule
 * ____________________
 *
 *PARAMETERS:   schedule_str : A pointer to a string containing
 *                  data representing an existing NTCSS schedule.  
 *                  This data should have been generated by prior 
 *                  calls to NtcssCreateSchedule or NtcssEditSchedule.
 *
 *DESCRIPTION:  This function will display the NTCSS Schedule 
 *              Selection dialog to show the given schedule to the user.
 *              Refer to TBD for details on the operation and use of 
 *              the NTCSS Schedule Selection dialog.  Note, however, 
 *              that the dialog will operate in read-only mode and 
 *              will not allow changes to the given schedule.  TRUE 
 *              will be returned by this function unless the given
 *              schedule is invalid.
 */


BOOL DllImport NtcssDisplaySchedule
		(const char FAR  *schedule_str);



/* ******************
 * NtcssDLLInitialize
 * __________________
 *
 *
 *PARAMETERS:   dll_version_str: DLL Version string (use defined NTCSS_DLL_VERSION).
 *
 *              app_name_str:    NTCSS Application Name.
 *
 *              hInstance:   The instance of the Windows application.
 *
 *              hWnd:        The Main Window of the calling application.
 *
 *
 *RETURNS:      TRUE:  If sucessful
 *              FALSE: Otherwise.
 *
 *DESCRIPTION: This function verifies that the correct version of the
 *    NTCSS DLL is being called and does NTCSS DLL dependent initialization.
 *    NtcssDLLInitialize() MUST be SUCCESSFULLY called before any other
 *    NTCSS DLL API function is called.  Before NtcssDLLInitialize may be
 *    successfully called the NTCSS Desktop must be currently running.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssDLLInitialize
		(const char FAR  *dll_version_str,
		 const char FAR  *app_name_str,
		 const HANDLE hInstance,
	     const HANDLE hWnd);



/* *****************
 * NtcssEditSchedule
 * _________________
 *
 *PARAMETERS:   orig_schedule_str : A pointer to a string containing
 *                  data representing an existing NTCSS schedule.  
 *                  This data should have been generated by prior 
 *                  calls to NtcssCreateSchedule or NtcssEditSchedule.
 *              new_schedule_buf : A pointer to a buffer to which will be
 *                   stored NTCSS schedule data.  Buffer should be 
 *                   large enough to contain NTCSS_MAX_SCHEDULE_LEN 
 *                   bytes.
 *              new_schedule_buf_size: size of schedule_buf buffer
 *
 *DESCRIPTION:  This function will display the NTCSS Schedule Selection
 *              dialog to allow the user to modify a schedule.  Refer 
 *              to TBD for details on the operation and use of the 
 *              NTCSS Schedule Selection dialog.  If successful TRUE 
 *              will be returned and data representing the selected
 *              NTCSS schedule will be written to the schedule_buf.
 *
 *              This function uses message boxes to display error 
 *              information to the user regardless of the current 
 *              NtcssErrorMessagingStatus() setting.  
 * 
 *              Note that when FALSE is returned the calling program 
 *              can determine if the reason that this function failed 
 *              to create a scedhule was due to error by checking 
 *              NtcssGetLastError().  The string written to by 
 *              NtcssGetLastError() will be NULL if the user cancelled 
 *              the create schedule dialog.
 */

BOOL DllImport NtcssEditSchedule
		(const char FAR  *orig_schedule_str,
         char FAR  *new_schedule_buf,
         const int new_schedule_buf_size);


/* **********************
 * NtcssErrorMessagingOFF
 * ______________________
 *
 *PARAMETERS:   none.
 *
 *RETURNS:      none.
 *
 *DESCRIPTION: This function will cause the NTCSS DLL not to display
 *  application level error messages when a subsequent error is
 *  encountered in a DLL API function call by the current application.
 *  The startup NtcssDLLInitialize() call for an application calls the
 *  NtcssErrorMessagingON() function and this setting is in effect
 *  until a  NtcssErrorMessagingOFF() call is made to the NTCSS DLL.
 *  Each application using  the NTCSS DLL retains its own 
 *  error message setting (NtcssErrorMessagingOFF() / NtcssErrorMessagingON()).
 */

void DllImport NtcssErrorMessagingOFF();



/* *********************
 * NtcssErrorMessagingON
 * _____________________
 *
 *PARAMETERS:   none.
 *
 *RETURNS:      none.
 *
 *DESCRIPTION: This function will cause the NTCSS DLL to resume displaying
 *  application level error messages when a subsequent error is
 *  encountered in a DLL API function call by the current application.
 *  The startup NtcssDLLInitialize call for an application calls the
 *  NtcssErrorMessagingON() function and this setting is in effect
 *  until a  NtcssErrorMessagingOFF() call is made to the NTCSS DLL.
 *  Each application using the NTCSS DLL retains its own 
 *  error message setting (NtcssErrorMessagingOFF() / NtcssErrorMessagingON()).
 */

void DllImport NtcssErrorMessagingON();



/* *************************
 * NtcssErrorMessagingStatus
 * _________________________
 *
 *PARAMETERS:   none.
 *
 *RETURNS:      TRUE   if NtcssErrorMessagingON() is in effect
 *              FALSE  if NtcssErrorMessagingOFF() is in effect
 *
 *DESCRIPTION: This function allows the calling application to determine
 *  the current setting of application level error handling of the NTCSS DLL.
 *  A return of  TRUE indicates that NtcssErrorMessagingON() is currently in
 *  effect and that the NTCSS DLL will display application level error
 *  messages when a error is encountered in a DLL API function call by
 *  the current application.  A return of FALSE indicates that
 *  NtcssErrorMessagingOFF() is currently in effect and that the NTCSS DLL
 *  will NOT display application error message when an error is encountered.
 */

BOOL DllImport NtcssErrorMessagingStatus();



/* ***************
 * NtcssGetAppData
 * _______________
 *
 *PARAMETERS:   app_data_buf: Destination address to store application data.
 *                   Buffer should be large enough to contain
 *                   NTCSS_SIZE_APP_DATA bytes.  The buffer will be NULL
 *                   terminated unless the current setting of the
 *                   Application data is a full NTCSS_SIZE_APP_DATA bytes.
 *
 *              app_data_buf_size: size of add_data_buf buffer
 *
 *RETURNS:      TRUE:        If successful.
 *              FALSE:       Otherwise.
 *
 *DESCRIPTION:  NTCSS maintains Application Data for each application 
 *			used by each user. This function will write the current 
 *			setting for the current user and application to the  
 *			destination address pointed to by parameter app_data_buf.
 *			Note that the current application is specified in the
 *			NtcssDLLInitialize call and the current user is identified
 *			at login to the NTCSS Desktop.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetAppData
		(char FAR  *app_data_buf,
         const int app_data_buf_size);
                                                                      

/* ***************
 * NtcssGetAppName
 * _______________
 *
 *PARAMETERS:   app_name_buf: A pointer to a buffer to store application name.
 *                   Buffer should be large enough to contain
 *                   NTCSS_SIZE_APP_GROUP bytes.   The buffer will be NULL
 *                   terminated unless the application name is a full
 *                   NTCSS_SIZE_APP_GROUP characters long
 *
 *              app_name_buf_size: size of app_name_buf buffer
 *
 *RETURNS:      TRUE:            If successful.
 *              FALSE:           Otherwise.
 *
 *DESCRIPTION:  This function will put the name of the current
 *              application, as registered in the NtcssDLLInitialize() call,
 *              into the buffer pointed to by app_name_buf.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetAppName
		(char FAR *app_name_buf,
		 const int   app_name_buf_size);



/* *******************
 * NtcssGetAppPassword
 * ___________________
 *
 *PARAMETERS:   password_buf: A pointer to a buffer to store password.
 *                   Buffer should be large enough to contain
 *                   NTCSS_SIZE_PASSWORD bytes.  The buffer will be
 *                   NULL terminated unless the password is a full
 *                   NTCSS_SIZE_PASSWORD characters long.

 *              password_buf_size: size of app_name_buf buffer
 *
 *RETURNS:      TRUE:         If successful.
 *              FALSE:        Otherwise.
 *
 *DESCRIPTION:  This function will copy the NTCSS Application Password into the
 *    buffer pointed to by password_buf.  
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetAppPassword
		(char *password_buf,
		 const int   password_buf_size);



/* *******************
 * NtcssGetAppUserInfo
 * ___________________
 *
 *PARAMETERS:   user_name_str: A pointer to a NULL terminated string
 *                              giving the name of the NTCSS user for
 *                              whom information is being requested
 *
 *              user_info_rec_ptr : A pointer to a NtcssUserInfoStruct
 *                              structure that will receive information
 *                              about the requested user
 *
 *              prog_access_info_buf : A pointer to a buffer to receive
 *                              NTCSS Application program access 
 *                              information (see description below). This
 *                              buffer should have length of at least 
 *                              NTCSS_SIZE_APP_PROGRAM_INFO bytes
 *
 *              prog_access_info_buf_size : Indicates size of
 *                              NTCSS_SIZE_APP_PROGRAM_INFO 
 *
 *RETURNS:      TRUE:            If successful.
 *              FALSE:           Otherwise.
 *
 *DESCRIPTION:  This function will retreive information concerning
 *    the NTCSS user whose name is indicated by paramter user_name_str.
 *    If successful then TRUE is returned, the NtcssUserInfoStruct
 *    pointed to by user_info_rec_ptr is filled with data and the
 *    buffer pointed to by prog_access_info_buf will be filled by up
 *    to NTCSS_SIZE_APP_PROGRAM_INFO bytes of NTCSS Application program
 *    access information.  The NTCSS Application program access 
 *    information is a comma separated list of programs which the
 *    user is allowed to access.  The data written to the 
 *    prog_access_info_buf will be NULL terminated.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetAppUserInfo
		(const char           *user_name_str,
		 NtcssUserInfo        *user_info_rec_ptr,
         char                 *prog_access_info_buf,
         const int            prog_access_info_buf_size);


/* *******************
 * NtcssGetAppUserList
 * ___________________
 *
 *PARAMETERS:   user_list_recs_ptr: A pointer to a buffer large enough to
 *                   store user_list_recs_size AppUserList records.
 *
 *              user_list_recs_cnt: number of NtcssAppUserList
 *					 structures that the buffer pointed to by 
 *                   user_list_recs_ptr can accomodate.  Note
 *                   that user_list_recs_cnt indicates size of buffer in full
 *                   NtcssAppUserList structures, NOT the number of bytes.
 *
 *DESCRIPTION:  This function will retrieve information on the users of 
 *				the current application and store the information as a 
 *				list of NtcssAppUserList structures which will be 
 *				written to the 
 *				buffer pointed to by user_list_recs_ptr.
 *
 *RETURNS:   An integer value indicating the number of valid NtcssAppUserList
 *           records written to the buffer pointed to by user_list_recs_ptr.
 *           Note that a successful call to NtcssGetAppUserList() 
 *           will result in one or more (up to user_list_recs_cnt)
 *           of these structures written successively into the buffer
 *           pointed to by user_list_recs_ptr.
 *
 *           A return of 0 by this function indicates that an error has
 *           occured and and if this is the case then NtcssGetLastError()
 *           can be called to find out the reason for the error
 */
                                                                              
int DllImport NtcssGetAppUserList
		(NtcssAppUserList FAR *user_list_recs_ptr,
         const int user_list_recs_cnt);


/* *******************
 * NtcssGetAppUserListCt
 * ___________________
 *
 *PARAMETERS:   None.
 *                   
 *
 *
 *DESCRIPTION:  This function will return the number of users for the current 
 *				application. This API should be used BEFORE a call to 
 *              NtcssGetAppUserList to properly allocate a buffer. The buffer 
 *              would be this return value * the size of a GetAppUser structure.
 *
 *RETURNS:   An integer value indicating the number of users for the current 
 *		     application.
 *
 *           A return of 0 by this function indicates that an error has
 *           occured and and if this is the case then NtcssGetLastError()
 *           can be called to find out the reason for the error
 */
                                                                              
int DllImport NtcssGetAppUserListCt();
		
/* *******************
 * NtcssGetCommonData
 * ___________________
 *
 *PARAMETERS:  tag_str: A pointer to a NULL terminated string which is the
 *                   Key for accessing a NTCSS Common Data Value.  The
 *                   string pointed to by tag_str may be up to 
 *                   NTCSS_SIZE_COMMON_TAG bytes in length.
 *
 *             common_data_buf : A pointer to a buffer to store retrieved
 *                   common data.  Buffer should be large enough to contain
 *                   NTCSS_SIZE_COMMON_DATA bytes.
 *
 *              common_data_buf_size: size of common_data_buf buffer
 *
 *RETURNS:      TRUE:        If successful.
 *              FALSE:       Otherwise.
 *
 *DESCRIPTION:  This function returns will use the string pointed to by tag_str
 *       to perform a lookup in the NTCSS Common Data Dictionary.  If a
 *       match is found then TRUE is returned and the current setting for
 *       the matched  Common Data Value is written to the buffer pointed
 *       to by common_data_buf.  The buffer will be NULL terminated
 *       unless the Common Data Value is a full NTCSS_SIZE_COMMON_DATA
 *       bytes long.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
*/

BOOL DllImport NtcssGetCommonData
		(const char FAR *tag_str,
         char FAR *common_data_buf,
         const int common_data_buf_size);


                               
/* ****************
 * NtcssGetDateTime
 * ________________
 *
 *PARAMETERS:   date_time_buf: A pointer to a buffer to receive the current
 *                date and time.  Buffer should be large enough to contain
 *                NTCSS_SIZE_DATETIME1 bytes.
 *
 *              date_time_buf_size    : The size of the date_time_buf
 *                   buffer.
 *
 *RETURNS:      TRUE:         If successful.
 *              FALSE:        Otherwise.
 *
 *DESCRIPTION:  This function will obtain current NTCSS Time (server based) and
 *       write it to the buffer pointed to by datetime_buf.  The time will
 *       be given in format returned from the Standard C Library
 *       asctime() function and will contain the '\n' and '\0' as the
 *       last two characters written to the datetime_buf.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetDateTime
		(char  *datetime_buf,
         const int datetime_buf_size);


/* ****************
 * NtcssGetFile
 * ________________
 *
 *PARAMETERS: ServerName:     A Pointer to a buffer containing the name of the Server to FTP to.
 *            
 *            
 *            ServerFileName: A Pointer to a buffer containing the name of the Server File to
 *                            be retrieved by FTP.
 *
 *			  ClientFileName: A Pointer to a buffer containing the name of the Client File to
 *                            be created by FTP.
 *
 *            Ascii:          A Boolean value used to to set the type of transfer for this FTP.
 *                            A value of NTCSS_ASCII_FTP denotes an ascii file transfer, A 
 *                            value of NTCSS_BINARY_FTP denotes a binary file transfer.
 *
 *
 *RETURNS:    TRUE:           If successful
 *            FALSE:          Otherwise
 *
 *
 *DESCRIPTION: This function takes the file specified by ServerFileName 
 *             and sends it via the FTP protocol from the Server specified 
 *             by ServerFileName. The new client file will be named as 
 *             specified by ClientFileName. The transfer can take place 
 *             in either Ascii (NTCSS_ASCII_FTP) mode or Binary mode
 *             NTCSS_BINARY_FTP.
 *
 */

BOOL DllImport NtcssGetFile
		(const char  *ServerName,
		 const char  *ServerFileName,
		 const char  *ClientFileName,
		 const BOOL Ascii);

/* ****************
 * NtcssGetFileInfo
 * ________________
 *
 *PARAMETERS: ServerName:     A Pointer to a buffer containing the name of the Server to FTP to.
 *            
 *            
 *            ServerFileName: A Pointer to a buffer containing the name of the Server File to
 *                            extract the number of lines and the number of bytes.
 *
 *			  Lines:          The number of lines in the Server File specified by ServerFileName.
 *                            
 *
 *            Bytes:		  The number of bytes in the Server File specified by ServerFileName.
 *
 *
 *RETURNS:    TRUE:           If successful
 *            FALSE:          Otherwise
 *
 *
 *DESCRIPTION: This function takes the file specified by ServerFileName 
 *             and retieves the number of lines it contains and the number  
 *             of bytes. The client should take these values and calculate a
 *             buffer for a subsequent to NtcssGetFileSection to actually retrieve
 *             the data. .
 *
 */

BOOL DllImport NtcssGetFileInfo
		(char *ServerName,
		 char *ServerFileName,
		 int *Lines,
		 int *Bytes);
	
/* ****************
 * NtcssGetFileSection
 * ________________
 *
 *PARAMETERS: ServerName:     A Pointer to a buffer containing the name of the Server to FTP to.
 *            
 *            
 *            ServerFileName: A Pointer to a buffer containing the name of the Server File to
 *                            be retrieved by FTP.
 *
 *            buf:            A Pointer to buffer allocated by the client by doing a 
 *                            calculation on the values from a previous call to NtcssGetFileInfo
 *
 *			  FirstLine:      The starting line the client wishes to retrieve from the Server
 *                            File.
 *                            .
 *
 *            LastLine:       The last line to be retrieved from the Server File. 
 *
 *
 *RETURNS:    Number of lines read:           If successful
 *            0:                              Otherwise
 *
 *
 *DESCRIPTION: This function takes the file specified by ServerFileName 
 *             and retieves the number of lines from it specified by  
 *             by the parameters FirstLine - Lastline. If Lastline is -1, 
 *             the function will return the FirstLine to the end of the file.
 *             The client buffer buf will receive the data and the function will 
 *             return the number of lines actually copied to the client buffer. 
 *
 */

int DllImport NtcssGetFileSection
		(char *ServerName,
		 char *ServerFileName,
		 char *buf,
		 int  FirstLine,
		 int  LastLine);

/* ****************
 * NtcssGetHostName
 * ________________
 *
 *PARAMETERS:   host_name_buf: A pointer to a buffer to receive the host name
 *                of the current client machine. Buffer should be large enough
 *                to contain NTCSS_SIZE_HOSTNAME bytes.
 *
 *              host_name_buf_size    : The size of the host_name_buf
 *                   buffer.
 *
 *RETURNS:      TRUE:         If successful.
 *              FALSE:        Otherwise.
 *
 *DESCRIPTION:  This function copies the host name of the current client
 *       machine to the buffer pointed to by host_name_buf.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */



BOOL DllImport NtcssGetHostName
		(char  FAR *host_name_buf,
         const int host_name_buf_size);

/* *************************
 * NtcssGetFilteredServerProcessList
 * _________________________
 *
 *
 *PARAMETERS:   filter: A NtcssProcessFilter structure (defined in this file)
 *					    that is filled by the caller and is used to control 
 *                      what processes are to be retrieved.
 *              				
 *              list_buf : A buffer of ServerProcessDetails records to 
                           receive the requested information 
 *            			   on server processes 
 *
 *              list_buf_slot_cnt : total number of ServerProcessDetails records
 *              					that can be held in the buffer pointed to
 *              					by list_buf. Should be determined by a call 
 *                                  to NtcssGetFilteredServerProcessListCt
 *
 *RETURNS:      An integer value indicating the number of ServerProcessDetails
 *				records written to the list_buf.  
 *
 *DESCRIPTION:  This function obtains information describing the current
 *  condition of server processes (filtered by the values in the NtcssProcessFilter
 *  structure) in the NTCSS system and writes this information in the form of 
 *  ServerProcessDetails records to the buffer area pointed to by list_buf.
 *  If a value less than 0 is returned then NtcssGetLastError() can be called 
 *  to find out why.
 * 
 */

int DllImport NtcssGetFilteredServerProcessList
		(NtcssProcessFilter *filter,
         NtcssServerProcessDetails *list_buf,
         const int list_buf_slot_cnt);

/* *************************
 * NtcssGetFilteredServerProcessListCt
 * _________________________
 *
 *
 *PARAMETERS:   filter: A NtcssProcessFilter structure (defined in this file)
 *					    that is filled by the caller and is used to control 
 *                      what processes are to be counted.
 *              					
 *
 *
 *RETURNS:      An integer value indicating the number of ServerProcessDetails
 *				records the meet the selection criteria. Can be used to allocate
 *              an appropriate buffer.
 *
 *DESCRIPTION:  This function returns a count of ServerProcessDetails records
 *  that would be returned (describing the current condition of server processes
 *  in the NTCSS system) filtered by the controlling parameters in the NtcssProcessFilter
 *  record. This function would typically be called BEFORE a call to
 *  NtcssGetFilteredServerProcessList using the SAME NtcssProcessFilter record to 
 *  Allocate a buffer of the appropraite size. If a value less than 0 is returned then
 *  NtcssGetLastError() can be called to find out why.
 */

int DllImport NtcssGetFilteredServerProcessListCt
		(NtcssProcessFilter *filter);

/* *****************
 * NtcssGetLastError
 * _________________
 *
 *PARAMETERS:   err_buf : A pointer to a buffer to receive the error message.
 *                Buffer should be large enough to contain
 *                NTCSS_MAX_ERROR_MSG_LEN bytes.
 *
 *              err_buf_size    : The size of the err_buf buffer
 *
 *RETURNS:      TRUE  : If successful.
 *              FALSE : Otherwise.
 *
 *DESCRIPTION: After NtcssErrorMessagingOFF() has been called and has not been
 *      reset by a subsequent call to NtcssErrorMessagingOFF(), then
 *      NtcssGetLastError() can be used to obtain a text string
 *      describing the most recent application level DLL error.
 *      The text string will be NULL terminated.  A FALSE return
 *      means that the NtcssGetLastError() function encountered an
 *      error.  A TRUE return with an empty err_buf (err_buf[0]==NULL)
 *      indicates that there is no most recent application level DLL
 *      error recorded.  It is important to note that most NTCSS DLL
 *      function calls clear the error indicators before beginning
 *      processing. In order to obtain the correct error indicator text
 *      NtcssGetLastError() should be used immediately following a 
 *      NTCSS DLL function call returning an error status.
 *      
 */

BOOL DllImport NtcssGetLastError
		(char  FAR *err_buf,
         const int err_buf_size);


/* *******************
 * NtcssGetLinkData
 * ___________________
 *
 *PARAMETERS:   link_data_buf: A pointer to a buffer to store link data.
 *                   Buffer should be large enough to contain
 *                   NTCSS_SIZE_PASSWORD bytes.  The buffer will be
 *                   NULL terminated unless the link data is a full
 *                   NTCSS_SIZE_PASSWORD characters long.

 *              link_data_buf_size: size of link_data_buf buffer
 *
 *RETURNS:      TRUE:         If successful.
 *              FALSE:        Otherwise.
 *
 *DESCRIPTION:  This function will copy the NTCSS Link Data into the
 *              buffer pointed to by link_data_buf.For the current user and application.
 *              Note that the current application is specified in the NtcssDLLInitialize
 *              call and the current user is identified at login to the	NTCSS Desktop.
 *
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetLinkData
		(char FAR    *link_data_buf,
		 const int    link_data_buf_size);

/* ***************
 * NtcssGetLocalIP
 * _______________
 *
 *PARAMETERS:   ip_addr_buf : A pointer to a buffer to receive the IP
 *                address string.
 *                Buffer should be large enough to contain
 *                NTCSS_SIZE_IP_ADDR bytes.
 *
 *              ip_addr_size    : The size of the ip_addr_buf buffer
 *
 *RETURNS:      TRUE  : If successful.
 *              FALSE : Otherwise.
 *
 *DESCRIPTION: This function returns the IP address of the current clieht
 *      machine as a NULL terminate string written to ip_addr_buf in the
 *      format "NUM.NUM.NUM.NUM" where NUM is the ASCII representation
 *      of of a number in the range 0 <= NUM <= 255, for example
 *      "198.202.184.114".
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetLocalIP
		(char  *ip_addr_buf,
         const int ip_addr_size);



/* ******************
 * NtcssGetServerName
 * __________________
 *
 *PARAMETERS:   server_name_buf: A pointer to a buffer to receive the name
 *                of the NTCSS server for the current client machine.
 *                Buffer should be large enough to contain
 *                NTCSS_SIZE_HOSTNAME bytes.
 *
 *              server_name_buf_size    : The size of the server_name_buf
 *                   buffer.
 *
 *RETURNS:      TRUE:         If successful.
 *              FALSE:        Otherwise.
 *
 *DESCRIPTION:  This function copies the name of the NTCSS server for
 *       the current client machine to the buffer pointed to by server_name_buf.
 *       The data written to the buffer pointed to by server_name_buf will
 *       be NULL terminated unless the name of the NTCSS server takes
 *       up a full NTCSS_SIZE_HOSTNAME bytes.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssGetServerName
		(char  *server_name_buf,
         const int server_name_buf_size);



/* *************************
 * NtcssGetServerProcessList
 * _________________________
 *
 *
 *PARAMETERS:   get_scheduled_jobs: Set to TRUE if information on scheduled
 *								jobs should be obtained
 *              get_unscheduled_jobs : Set to TRUE if information on 
 *              					unscheduled
 *              get_only_running_jobs : Set to TRUE if information should 
 *              					be obtained only on currently running 
 *              					jobs
 *              get_for_current_user_only : Set to TRUE if information on
 *              					server processes should be obtained 
 *              					only for processes initiated jobs should 
 *              					be obtained
 *              list_buf : a buffer area to receive the requested information 
 *            					on server processes
 *              list_buf_slot_cnt : total number of ServerProcessDetails records
 *              					that can be held in the buffer pointed to
 *              					by list_buf
 *
 *RETURNS:      An integer value indicating the number of ServerProcessDetails
 *				records written to the list_buf.  
 *
 *DESCRIPTION:  This function obtains information describing the current
 * condition of server processes in the NTCSS system and writes this
 * information in the form of ServerProcessDetails records to the buffer
 * area pointed to by list_buf.  If a value less than 0 is returned then 
 * NtcssGetLastError() can be called to find out why.
 * can be used to determine if an error occurred 
 */

int DllImport NtcssGetServerProcessList
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only,
         NtcssServerProcessDetails FAR *list_buf,
         const list_buf_slot_cnt);

/* *************************
 * NtcssGetServerProcessListCt
 * _________________________
 *
 *
 *PARAMETERS:   get_scheduled_jobs:    Set to TRUE if information on scheduled
 *								       jobs should be obtained
 *              get_unscheduled_jobs : Set to TRUE if information on 
 *              					   unscheduled
 *              get_only_running_jobs : Set to TRUE if information should 
 *              					    be obtained only on currently running 
 *              					    jobs
 *              get_for_current_user_only : Set to TRUE if information on
 *              					        server processes should be obtained 
 *              					        only for processes initiated jobs should 
 *              					        be obtained
 *
 *RETURNS:      An integer value indicating the number of ServerProcessDetails
 *				records that meet the criteria set by the input parameters.  
 *
 *DESCRIPTION:  This function returns a count of the number of ServerProcessDetails records
 * that WOULD be returned based on the criteria set by the input parameters. This function
 * would typically called BEFORE a call to NtcssGetServerProcessList to enable the client to
 * allocate the correct size buffer. If a value less than 0 is returned then 
 * NtcssGetLastError() can be called to find out why.
 */

int DllImport NtcssGetServerProcessListCt
		(const BOOL get_scheduled_jobs,
         const BOOL get_unscheduled_jobs,
         const BOOL get_only_running_jobs,
         const BOOL get_for_current_user_only);
         
/* *****************
 * NtcssGetUserInfo
 * _________________
 *
 *PARAMETERS:   user_info_ptr:  A pointer to a structure of type NtcssUserInfo.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:   Returns information about the current user in the record
 *       pointed to by user_info_ptr.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
*/

BOOL DllImport NtcssGetUserInfo
		(NtcssUserInfo *user_info_ptr);



/* *******************
 * NtcssPostAppMessage
 * ___________________
 *
 *PARAMETERS:   msg_title: A pointer to a NULL terminated string which 
 *						 is the title for the message that will be 
 *						 posted to the application Message Board. The  
 *						 title may be up to NTCSS_MAX_MESSAGE_TITLE_LEN
 *						 characters in length
 *
 *				msg_str: A pointer to a NULL terminated string which is
 *						 the message that will be posted to the 
 *						 application Message Board. The message may 
 *						 be up to NTCSS_MAX_MESSAGE_LEN characters in 
 *						 length
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will post the given message to the 
 *				Message Board for the current application.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssPostAppMessage
		(const char  *msg_title,
         const char  *msg_str);



/* ****************
 * NtcssPostMessage
 * ________________
 *
 *PARAMETERS:   msg_board_str: A pointer to a NULL terminated string 
 *						 indicating the name of the Message Board 
 *						 to which the given message will be posted.	
 *						 The Message Board name may be up to 
 *						 NTCSS_SIZE_MSG_BOARD_NAME characters in length
 *
 *				msg_title: A pointer to a NULL terminated string which 
 *						 is the title for the message that will be 
 *						 posted to the given Message Board. The message 
 *						 title may be up to NTCSS_MAX_MESSAGE_TITLE_LEN
 *						 characters in length
 *
 *				msg_str: A pointer to a NULL terminated string which is
 *						 the message that will be posted to the given
 *						 Message Board. The message may be up to
 *						 NTCSS_MAX_MESSAGE_LEN characters in length
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will post the given message to the 
 *				indicated Message Board.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssPostMessage
		(const char  *msg_board_str,
		 const char  *msg_title,
         const char  *msg_str);



/* **********************
 * NtcssPostSystemMessage
 * ______________________
 *
 *PARAMETERS:   msg_title: A pointer to a NULL terminated string which 
 *						 is the title for the message that will be 
 *						 posted to the system Message Board. The message 
 *						 title may be up to NTCSS_MAX_MESSAGE_TITLE_LEN
 *						 characters in length
 *
 *				msg_str: A pointer to a NULL terminated string which is
 *						 the message that will be posted to the 
 *						 System Message Board. The message may 
 *						 be up to NTCSS_MAX_MESSAGE_LEN characters in 
 *						 length
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will post the given message to the 
 *				NTCSS System Message Board.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssPostSystemMessage
		(const char *msg_title,
         const char *msg_str);



/* ********************
 * NtcssPrintClientFile
 * ____________________
 *
 *PARAMETERS:   file_name_str: A pointer to a NULL terminated string
 *                indicating the name of the file on the client 
 *                 machine that is to be printed.  The string may be 
 *                 NULL to indicate that a dialog box is to be used to 
 *                 allow the user to select the client file to be 
 *                 printed.  
 *
 *              seclvl: A value in the range range NTCSS_UNCLASSIFIED ..
 *                   NTCSS_TOPSECRET which indicates the security
 *                   level of the print operation to be performed.
 *
 *              output_type_str: A NULL terminated string used to
 *                  associate applications with printers.  Determines
 *                  printers that will be available in the printer
 *                  selection for this function.
 *
 *RETURNS:      TRUE:           If a file was successfully printed.
 *              FALSE:          FALSE if, for any reason, a file was 
 *                              not printed.
 *                              
 *DESCRIPTION:   This function operates in an interactive mode.
 *           Dialog boxes are provided to allow user selection 
 *           of a client file to print (only when file_name_str is NULL) 
 *           and user selection of an NTCSS remote or local printer. 
 *           (selection based on seclvl and output_type_str parameters).  
 *           If this function successfully prints a file then TRUE is 
 *           returned.  FALSE is returned if no file is printed (due 
 *           to user cancellation or error).  
 *
 *           This function uses message boxes to display error 
 *           information to the user regardless of the current 
 *           NtcssErrorMessagingStatus() setting.  
 * 
 *           Note that when FALSE is returned the calling program 
 *           can determine if the reason that this function failed 
 *           to print was due to error by checking NtcssGetLastError().  
 *           The string written to by NtcssGetLastError() will be NULL 
 *           if the user cancelled the print operation.
 */

 BOOL DllImport NtcssPrintClientFile
		(const char  *file_name_str,
         const int  seclvl, 
         const char  *output_type_str);


/* ********************
 * NtcssPrintServerFile
 * ____________________
 *
 *PARAMETERS:   server_name_str: A pointer to a NULL terminated string
 *                indicating the name of the server machine that is
 *                 to be used for printing. 
 *
 *              seclvl: A value in the range range NTCSS_UNCLASSIFIED ..
 *                   NTCSS_TOPSECRET which indicates the security
 *                   level of the print operation to be performed.
 *
 *              output_type_str: A NULL terminated string used to
 *                  associate applications with printers.  Determines
 *                  printers that will be available in the printer
 *                  selection for this function.
 *
 *              dir_name_str: A pointer to a NULL terminated string
 *                  specifying the Server directory from which the
 *                  user will be allowed to select a file for printing.
 *                  The string pointed to by dir_name_str MUST specify a valid
 *                  server directory accessible by the current user.
 *
 *              copies: An Integer in the range of 01..99. Used to control
 *                      the number of printed copies.
 *
 *              options: A pointer to a NULL terminated string of up to 255 characters
 *                       used to pass printer options to the Server
 *
 *RETURNS:      TRUE:           If a file was successfully printed.
 *              FALSE:          FALSE if, for any reason, a file was 
 *                              not printed.
 *                              
 *DESCRIPTION:   This function operates in an interactive mode.
 *           Dialog boxes are provided to allow user selection 
 *           of a server file to print and user selection of an NTCSS 
 *           remote printer (selection based on server_name_str, 
 *           seclvl and output_type_str parameters).  If this function 
 *           successfully prints a file then TRUE is returned.  FALSE 
 *           is returned if no file is printed (due to user 
 *           cancellation or error).
 *
 *           Note that the user is not allowed to change directories
 *           on the server - i.e. the user is restricted to printing 
 *           a file that resides in the directory given by dir_name_str.  
 *
 *           This function uses message boxes to display error 
 *           information to the user regardless of the current 
 *           NtcssErrorMessagingStatus() setting.  
 * 
 *           Note that when FALSE is returned the calling program 
 *           can determine if the reason that this function failed 
 *           to print was due to error by checking NtcssGetLastError().  
 *           The string written to by NtcssGetLastError() will be NULL 
 *           if the user cancelled the print operation.
 */

BOOL DllImport NtcssPrintServerFile
		(const char  *server_name_str,
         const int    seclvl, 
         const char  *output_type_str,
         const char  *dir_name_str,
		 const int   copies,
		 const char  *options);

/* ********************
 * NtcssPutFile
 * ____________________
 *
 *PARAMETERS: ServerName:     A Pointer to a buffer containing the name of the Server to FTP to.
 *            
 *            
 *            ServerFileName: A Pointer to a buffer containing the name of the Server File to
 *                            be created by FTP.
 *
 *			  ClientFileName: A Pointer to a buffer containing the name of the Client File to
 *                            be sent by FTP.
 *
 *            Ascii:          A Boolean value used to to set the type of transfer for this FTP.
 *                            A value of NTCSS_ASCII_FTP  denotes an ascii file transfer, A value of NTCSS_BINARY_FTP 
 *                            denotes a binary file transfer.
 *
 *
 *RETURNS:    TRUE:           If successful
 *            FALSE:          Otherwise
 *
 *
 *DESCRIPTION: This function takes the file specified by ClientFileName 
 *             and sends it via the FTP protocol to the Server specified 
 *             by ServerFileName. The new server file will be named as 
 *             specified by ServerFileName. The transfer can take place 
 *             in either Ascii (NTCSS_ASCII_FTP) mode or Binary mode
 *             NTCSS_BINARY_FTP.
 *
 */

BOOL DllImport NtcssPutFile
		(const char  *ServerName,
		 const char  *ServerFileName,
		 const char  *ClientFileName,
		 const BOOL Ascii);

/* ********************
 * NtcssPutFileAppend
 * ____________________
 *
 *PARAMETERS: ServerName:     A Pointer to a buffer containing the name of the Server to FTP to.
 *            
 *            
 *            ServerFileName: A Pointer to a buffer containing the name of the Server File to
 *                            be apppended to  by FTP.
 *
 *			  ClientFileName: A Pointer to a buffer containing the name of the Client File to
 *                            be sent by FTP.
 *
 *            Ascii:          A Boolean value used to to set the type of transfer for this FTP.
 *                            A value of NTCSS_ASCII_FTP  denotes an ascii file transfer, A value of NTCSS_BINARY_FTP 
 *                            denotes a binary file transfer.
 *
 *
 *RETURNS:    TRUE:           If successful
 *            FALSE:          Otherwise
 *
 *
 *DESCRIPTION: This function takes the file specified by ClientFileName 
 *             and sends it via the FTP protocol to the Server specified 
 *             by ServerFileName. The data will be appended to the server file
 *             if it exists, a new file will be created if the sever file doesn't
 *             exist as specified by ServerFileName. The transfer can take place 
 *             in either Ascii (NTCSS_ASCII_FTP) mode or Binary mode
 *             NTCSS_BINARY_FTP.
 *
 */

BOOL DllImport NtcssPutFileAppend
		(LPCSTR ServerName,
		 LPCSTR ServerFileName,
		 LPCSTR ClientFileName,
		 const BOOL Ascii);

/* ********************
 * NtcssRegisterAppUser
 * ____________________
 *
 *PARAMETERS:   user_name_str:  A pointer to a NULL terminated string
 *                               indicating the NTCSS user to be affected
 *                               by this call.
 *
 *              do_register_flag: If this if value is TRUE the function will
 *                               Register the user, if FALSE then will
 *                               DeRegister the user.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION: The NTCSS provides facilities for applications 
 *             to allow registering of users.  This mechanism simply 
 *             records for each user of each application whether or 
 *             not the user is 'registered'.  The NtcssRegisterAppUser()
 *             function is the mechanism provided to allow registering 
 *             and deregistering to take place and the NtcssGetAppUserList() 
 *             function can be used to access the application registration 
 *             status of users.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssRegisterAppUser
		(const char  *user_name_str,
         const BOOL  do_register_flag);



/* **************************
 * NtcssRegisterSecurityClass
 * __________________________
 *
 *PARAMETERS:   hInstance:   The instance of the windows application.
 *
 *              seclvl:      A value in the range NTCSS_UNCLASSIFIED ..
 *                           NTCSS_TOPSECRET.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION: This function sets the NTCSS Security Level of the current
 *              application to the value specified by parameter seclvl.
 *              The NTCSS Security Banner will be reset to
 *              display the highest security level of all 
 *              registered applications or "Unclassified" 
 *              if the NTCSS Desktop is the only registered application.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 *
 */

BOOL DllImport NtcssRegisterSecurityClass
		(const HANDLE hInstance,
         const int seclvl);


/* *****************************
 * NtcssRemoveServerProcess
 * _____________________________
 *
 *
 *PARAMETERS:   pid_buf:	   The uniquie NTCSS process identifier for
 *							   the process to be removed
 *
 *              scheduled_job: TRUE if the process to be removed is 
 *							   a scheduled process, FALSE if it
 *							   is an unscheduled process.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION: This function will attempt to remove the server process
 *    identified uniquely by the pid_buf from the NTCSS server process
 *    control queues.  The scheduled_job parameter unscheduled process 
 *    is required to indicate whether it is a scheduled process or an
 *    unscheduled process that is being removed.  Note that 
 *    NtcssRemoveServerProcess is used to remove scheduled 
 *    processes and processes that are currently in a waiting 
 *    state (waiting for resources and/or approvals).  
 *    NtcssRemoveServerProcess cannot be used to terminate 
 *    currently active processes - use NtcssStopServerProcess instead.
 */

BOOL DllImport NtcssRemoveServerProcess 
		(const char *pid_buf,
         const BOOL scheduled_job);



/* *******************
 * NtcssRestorePrinter
 * ___________________
 *
 *PARAMETERS:   none
 *
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION: This function is called after a NtcssSelectPrinter()
 *       has been successfully performed and the calling application 
 *       is now done with the printer selected and the spool file 
 *       set up by that function.  This function will release NTCSS 
 *       DLL printer resources and return to the printer settings as 
 *       they were prior to the NtcssSelectPrinter() call.  
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssRestorePrinter(void);



/* **************************
 * NtcssScheduleServerProcess
 * __________________________
 *
 *PARAMETERS:  schedule_str : A pointer to a buffer which contains NTCSS schedule
 *							data.  The data pointed to by schedule_str 
 *							should have been set using the 
 *							NtcssCreateSchedule() or NtcssEditSchedule() API.
 *							Alternately, schedule_str may be NULL to 
 *							indicate that the NTCSS Schedule Selection
 *							dialog to should be used to allow the user 
 *							to specify a schedule.
 *
 *              descr_ptr: A pointer to a ServerProcessDescr that is 
 *						   to contain data to describe the server 
 *						   process that is to be started.  Refer to the 
 *						   descripton of the ServerProcessDescr structure 
 *						   above for details on the field contents of 
 *						   the structure.
 *
 *				pid_buf  : destination address to receive, if the 
 *						   function is successful, the process id for 
 *						   the server process which is started
 *
 *				pid_buf_size : size of the pid_buf
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will attempt to schedule the server process
 *				that is described in the ServerProcessDescr structure
 *				pointed to by descr_ptr. 
 *
 *				The schedule indicated by parameter schedule_str will be
 *				used to specify the schedule for the process unless 
 *				schedule_str is NULL, in which case the NTCSS Schedule 
 *				Selection dialog will be used to create a schedule for 
 *				the process.
 *
 *				Note that if parameter schedule_str is NULL and the
 *				NTCSS Schedule Selection dialog used to collect the 
 *				schedule information is cancelled then no attempt to 
 *				schedule the server process will be made.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.  
 */

BOOL DllImport NtcssScheduleServerProcess
		(const char  *schedule_str,
		 NtcssServerProcessDescr *descr_ptr,
         char   *pid_buf,
         const int pid_buf_size);



/* ******************
 * NtcssSelectPrinter
 * __________________
 *
 *PARAMETERS:   output_type_str: A NULL terminated string used to
 *                  associate applications with printers.  Determines
 *                  the printers to be available in the
 *                  printer selection.
 *
 *              seclvl: A value in the range NTCSS_UNCLASSIFIED ..
 *                  NTCSS_TOPSECRET which indicates the security
 *                  level requirements for the printer selection.
 *
 *              use_default_prt_flag: If TRUE then this function does not
 *                  allow user interaction in printer selection but 
 *                  instead the selected printer will be the current 
 *                  NTCSS default remote printer of the specified
 *					output type.  If FALSE then a printer selection
 *                   dialog will be used to 
 *                  select a printer. 
 *
 *              general_access_prt_flag: If TRUE then general access printers
 *                   will be included in the printer selection 
 *                   choices, otherwise they are not.
 *
 *              return_DC_flag: If TRUE then this routine will return
 *                   a handle to the device context for the selected 
 *                   printer, if FALSE then return from this function 
 *                   is either TRUE to indicate success or FALSE to 
 *                   indicate failure.  See description below for 
 *                   more details.
 *
 *              pr_attr_ptr: A pointer to a PrintAttrib structure used
 *                  to alter the device context of the selected printer.
 *
 *              local_enable_flag: A BOOL variable which should be set  
 *                  to TRUE if local printers are to be included in the 
 *                  available printer selection or FALSE if not.
 *
 *              remote_enable_flag: A BOOL variable which should be set  
 *                  to TRUE if remote printers are to be included in  
 *                  the available printer selection or FALSE if not.
 *
 *RETURNS:      When (return_DC_flag == TRUE)
 *                  a handle to a device context for the selected
 *                  printer if successful or NULL if not successful
 *
 *              When (return_DC_flag == FALSE)
 *                  TRUE if successful or FALSE if not
 *
 *DESCRIPTION: This function is to be used by the calling program to
 *       select a printer to receive output by the program.  This
 *       function operates in a variety of modes depending
 *       on the settings of calling parameters.  
 *
 *       The method of printer selection is determined by parameter 
 *       return_DC_flag.  When return_DC_flag is TRUE then the return
 *       from this function is a handle to a Device Context that 
 *       can be used to write directly to a spool file setup for 
 *       the selected printer. When return_DC_flag is FALSE then 
 *       this routine updates the Windows win.ini "device" profile 
 *       to indicate the printer selection to the calling program.
 *
 *       If default_flag is TRUE then this routine does not 
 *       display a dialog box to allow printer selection but instead 
 *       
 *       uses the default NTCSS remote printer as the 'selected' printer.
 *       If default_flag is FALSE then a printer selection dialog is
 *       used to allow interactive user selection of a printer.
 *
 *       The BOOL flags local_enable_flag and remote_enable flags 
 *       are only valid if the default_flag is FALSE.  If the 
 *       local_enable_flag is TRUE then local printers are 
 *       displayed in the selection dialog and if remote_enable_flag 
 *       is TRUE then remote printers are displayed in the selection 
 *       dialog.  When default_flag is FALSE either local_enable_flag 
 *       or remote_enable_flag or both must be TRUE
 *       
 *       The PrintAttrib structure pointed to by pr_attr_ptr can 
 *       be used to specify print options.  However, note that 
 *       during interactive printer selection (when default_flag == 
 *       FALSE) that the user is may, through a printer setup dialog, 
 *       change these specifications.
 *
 *       After a successful call to NtcssSelectPrinter(), the calling 
 *       program can use the returned device context handle 
 *       (return_DC_flag==TRUE) or the information in the "device"
 *       file (return_DC_flag==FALSE) to output information to a 
 *       temporary spool file.  When output is complete then a call 
 *       to NtcssSpoolPrintRequest() should be made to have the spool 
 *       file printed on the specified printer and then a call to 
 *       NtcssRestorePrinter() made to allow the NTCSS DLL to  
 *       release printer resources and return to original print 
 *       settings.  Note that the call to NtcssSpoolPrintRequest() 
 *       can be omitted in case of print cancellation but that a 
 *       call to NtcssRestorePrinter() should always be made after
 *       a successful call to NtcssSelectPrinter().
 *
 *       Note that when FALSE is returned the calling program 
 *       can determine if the reason that this function failed 
 *       to print was due to error by checking NtcssGetLastError().  
 *       The string written to by NtcssGetLastError() will be NULL 
 *       if the user cancelled the printer selection dialog.
 */

HDC DllImport NtcssSelectPrinter
		(const char     *output_type_str,
         const int           seclvl,
         const BOOL          use_default_prt_flag,
         const BOOL          general_access_prt_flag,
         const BOOL          return_DC_flag,
         const NtcssPrintAttrib *pr_attr_ptr,
         const BOOL          local_enable_flag,
         const BOOL          remote_enable_flag);

/* ***************
 * NtcssSelectServerFile
 * ________________
 *
 *PARAMETERS:   lpszServer:    A pointer to a NULL terminated string identifying
 *                             the server.
 *
 *              lpszDirectory: A pointer to a NULL terminated string identifying
 *                             the directory.
 *                        
 *              BuffSize:      An int indicating the length of the users buffer.
 *
 *
 *              lpszFileName:  A pointer to a NULL terminated string (BuffSize)
 *                             used to return FileName to user.
 *
 *RETURNS:      -1:           If user cancels or a runtime exception occurs.
 *               0:           If user supplied buffer is to small.
 *              >0:           Indicates success & the length of FileName
 *
 *DESCRIPTION:  This function will present a dialog with a List of files
 *              in the user specified directory. On Selecting OK or double
 *              clicking, the API will return the FileName to the caller.
 *
 */


int DllImport NtcssSelectServerFile(LPCTSTR lpszServer,LPCTSTR lpszDirectory,
												int buffSize, LPTSTR lpszFileName);


/* ***************
 * NtcssSetAppData
 * ________________
 *
 *PARAMETERS:   app_data_str:   A pointer to a NULL terminated string to use
 *                        in setting the NTCSS Applidation data for the current
 *                        NTCSS application.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will set the NTCSS Application Data for
 *              the current application (as determined in the startup
 *              NtcssDLLInitialize call).
 *
 *DESCRIPTION:  NTCSS maintains Application Data for each application 
 *				used by each user. This function will set the 
 *				application data for the current user and application 
 *				using data supplied in the address pointed to by 
 *				parameter app_data_buf.  Note that the current 
 *				application is specified in the	NtcssDLLInitialize call 
 *				and the current user is identified at login to the 
 *				NTCSS Desktop.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssSetAppData
		(const char *app_data_str);

/* ***************
 * NtcssSetLinkData
 * ________________
 *
 *PARAMETERS:   link_data_str: A pointer to a NULL terminated string to use
 *                         in setting the NTCSS Link data for the current
 *                         user and application. Note that the current 
 *				           application is specified in the	NtcssDLLInitialize call 
 *				           and the current user is identified at login to the 
 *				           NTCSS Desktop. 
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will set the NTCSS Link Data (password) for
 *              the current user (as determined in the startup
 *              NtcssDLLInitialize call).
 *
 *
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssSetLinkData
		(const char *link_data_str);

/************************
 * NtcssSpoolPrintRequest
 * ______________________
 *
 *PARAMETERS:   none.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION: This function is called to send to a printer data 
 *       that the calling appliction has written to a spool file.  
 *       The spool file is set up with the NtcssSelectPrinter() 
 *       function call and comments for that API should be referred 
 *       to for further details.  A successful NtcssSelectPrinter() 
 *       must take place before this function is called. 
 *       The NtcssSpoolPrintRequest() may be be called any number of 
 *       times to send spool file data to the printer.  After each 
 *       call, the the data	in the spool file is sent to the 
 *       printer and the spool file is cleared.
 *       
 *       After print file spooling is complete, the NtcssRestorePrinter() 
 *       function must be called.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssSpoolPrintRequest();



/* ***********************
 * NtcssStartServerProcess
 * _______________________
 *
 *
 *PARAMETERS:   descr_ptr: A pointer to a ServerProcessDescr that is 
 *						   to contain data to describe the server 
 *						   process that is to be started.  Refer to the 
 *						   descripton of the ServerProcessDescr structure 
 *						   above for details on the field contents of 
 *						   the structure.
 *
 *				pid_buf  : destination address to receive, if the 
 *						   function is successful, the process id for 
 *						   the server process which is started
 *
 *				pid_buf_size : size of the pid_buf
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will attempt to start the server process
 *				that is described in the ServerProcessDescr pointed to 
 *				by descr_ptr. 
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssStartServerProcess
		(NtcssServerProcessDescr *descr_ptr,
         char  *pid_buf,
         const int pid_buf_size);



/* ***********************
 * NtcssStopServerProcess
 * _______________________
 *
 *PARAMETERS:   pid_str: A pointer to a NULL terminated string 
 *						 indicating the NTCSS process id for the server 
 *						 process to be stopped.
 *
 *				signal_type: Must be set to either NTCSS_STOP_PROCESS_SOFT
 *							 or NTCSS_STOP_PROCESS_HARD	to indicate how
 *							 the given process is to be terminated.
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:  This function will attempt to stop the server process
 *				identified by the given pid_str.  Note that 
 *				NtcssStopServerProcess can only stop currently executing
 *				processes.  Use	NtcssRemoveServerProcess to remove
 *				scheduled processes and processes that are currently in 
 *				a waiting state (waiting for resources and/or approvals).
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssStopServerProcess 
		(const char *pid_str,
         const int signal_type);





/* ***********************
 * NtcssValidateUser
 * _______________________
 *
 *PARAMETERS:   ssn_buf:        A pointer to a buffer of NTCSS_SIZE_SSN bytes
 *				                that will be filled with the users SSN 
 *                              if this function is successful. 
 *
 *  		ssn_buf_size:       An integer number that is the number of bytes in the             
 *				                ssn_buf. This should be equal to NTCSS_SIZE_SSN.
 *				
 *						 
 *					
 *
 *RETURNS:      TRUE:           If successful.
 *              FALSE:          Otherwise
 *
 *DESCRIPTION:                  This function will present the user with a dialog to
 *                              enter their user name and password. If this information
 *                              describes a valid user, the ssn_buf will return the SSN and
 *                              the function will return TRUE.
 *
 *    If this function is not successful then NtcssGetLastError() can be called
 *    to find out why.
 */

BOOL DllImport NtcssValidateUser (char *ssn_buf, int ssn_buf_size);


/* ***********************
 * NtcssViewServerFile
 * _______________________
 *
 *PARAMETERS:   Server :        A pointer to a string containing the desired server name.
 *				                that will be filled with the users SSN 
 *                              if this function is successful. 
 *
 *  		    Directory:      A pointer to a string containing the desired server directory             
 *				                to retrieve files. 
 *				
 *						 
 *					
 *
 *RETURNS:      Nothing:
 *              
 *
 *DESCRIPTION:                  This function will present the user with a dialog with a file 
 *                              list composed of the files retrieved using the Server & Directory
 *                              parameters. The user selects files from this list & another Dialog
 *                              will display them.
 *
 *   
 *    
 */

void DllImport NtcssViewServerFile (LPCTSTR Server, LPCTSTR Directory);
		

#ifdef __cplusplus
}
#endif  // __cplusplus 

#endif  //  NTCSS_DLL_BUILD 

#endif  // _DEFINE_NTCSSAPI_H_