/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Dll_defs.h                    
//                                         
// This file contains declarations specific to the NTCSS DLL.
// are needed by all DLL source files.
//
// Also see include\ntcssdef.h for more general declarations.
//               
//----------------------------------------------------------------------

#ifndef _INCLUDE_DLLDEFS_H_CLASS_
#define _INCLUDE_DLLDEFS_H_CLASS_

#include <sizes.h>


// ***
// *** TEMPORARY AREA - Put things here until I find
// ***                  somewhere better or get rid of

                             

#define UNSURE_ABOUT_VAL1   5582



#define HELP_COMING_SOON_MSG "Help information for this dialog is planned."


// ***
// *** END OF TEMPORARY AREA 
// ***                  



// ***
// *** Keep defines/const ints in alphabetical order
// ***

                                       // Used in some dialogs to make
                                       //  error sound
                                       //
const unsigned int BEEP_SOUND  = 0xFFFF;

                                       // This enum used to identify the
                                       //   type of application that is
                                       //   using the dll
typedef enum EtypeOfApp
{
    NORMAL_APP      = 0,
    PRIVATE_APP     = 1,
    SECBAN_APP      = 2,
    DESKTOP_APP     = 3,
    THUNKER_APP     = 4
}                                 
EtypeOfApp;
                                       // To Desktop on int returns 0
                                       //   means ok, -1 means error
#define DESKTOP_INT_OK           0
#define DESKTOP_INT_ERROR       -1

                                       // Used to validate Login Reply
                                       //  Structure data received from
                                       //  the DeskTop
#define DESKTOP_LRS_VALIDTY_VALUE 1
                                       // This is default spool dir
                                       //  used unless specified
                                       //  differently in INI file
                                       //
#define NTCSS_DEFAULT_SPOOL_DIR "c:\\"

                                       // Name for dll to use in msg
                                       //  boxes, etc.
                                       //
const CString NTCSS_DLL_NAME = "NTCSS DLL";


                                       // Strings used in call to 
                                       //   get windows profile info
                                       //
#define NTCSSINICODE_MAIN_NTCSS_SECTION_CODE  "NTCSS"

                                       // For server name info
                                       //
#define NTCSSINICODE_SERVER_SECTION_CODE      "SERVER"
#define NTCSSINICODE_BROADCAST_CODE           "BROADCAST"
#define NTCSSINICODE_NAME_KEY_CODE            "NAME"

                                       // For LPR infor
                                       //
#define NTCSSINICODE_LPR_SECTION_CODE         "LPR"
#define NTCSSINICODE_SEQNO_CODE               "SEQNO"
#define NTCSSINICODE_SPOOLFILENO_CODE         "SPOOLNO"

                                       // For spool directory
                                       //
#define NTCSSINICODE_SPOOL_DIR_CODE           "NTCSS_SPOOL_DIR"
#define NTCSSINICODE_SPOOL_DEFAULT_DIR_CODE   "c:\\"


                                       // Misc. NTCSS constants
                                       //
#define NTCSSVAL_AVG_INI_DEV_PROFILE_LEN    80

#define NTCSSVAL_AVG_UNIX_FILE_NAME_LEN     128

#define NTCSSVAL_MAX_INI_SECTION            64280

#define NTCSSVAL_UNIX_FILE_DATE_LEN			14

#define NTCSSVAL_UNIX_FILE_OWNER_LEN		13

#define NTCSSVAL_MAX_LOCAL_PRINTERS         12

#define NTCSSVAL_MAX_LPR_SEQ_NUM            999  

#define NTCSSVAL_MAX_LPR_SPOOLFILENUM       999  

#define NTCSSVAL_MAX_INI_DEV_PROFILE_LEN    256

//#define NTCSSVAL_MAX_REMOTE_PRINTERS        12

#define NTCSSVAL_MAX_SOCKET_RECV_BLOCK_SIZE 8192

#define NTCSSVAL_MAX_SOCKET_TIMEOUT_SECS    40 //changed from 10 5/5/99 camp legeune

#define NTCSSVAL_IN_PROGRESS_CODE           99

#define NTCSSVAL_PRINTER_REDIRECT_CODE      "***"

#define NTCSSVAL_LPR_ACK                    '\000'

#define NTCSSVAL_LPR_CMDFILE_NUM_LINES      512

#define NTCSSVAL_LPR_CMDFILE_LINE_LEN       80

#define NTCSSVAL_LPR_PRINT_DOCNAME          "ntcss_pt"

#define NTCSSVAL_LPR_PRINT_PREFIX_STR       "NTCSS_PRINTREQ:"

#define NTCSSVAL_LPR_LOCALPRT_DOC_NAME      "NTCSS_LOCAL_LPR"
#define NTCSSVAL_LPR_SPOOL_DOC_NAME         "NTCSS_REMOTE_SPOOL_LPR"

#define NTCSSVAL_ROLE_USER                  0
#define NTCSSVAL_ROLE_APP_ADMINISTRATOR     1
#define NTCSSVAL_ROLE_NTCSS_ADMINISTRATOR   2

                                      // DeskTop always at session slot
                                      //  0, SecBan at 1, users at 2+
#define SESSION_IDX_DESKTOP     0
#define SESSION_IDX_SECBAN      1
#define SESSION_IDX_USER_START  2

                                       // This many concurrent sessions
                                       //   max for the NTCSS DLL
#define SESSIONS_MAX_ACTIVE_CNT  25

                                       // These names used in call to
                                       //  CreateFileMapping to setup
                                       //  shared memory areas
                                       //   
#define SHMEM_LOGINREP  "LRSHMEM4"

#define SHMEM_MAINDLL    "MAINDLL_SHMEM"


#define TEXT_MSG_BAD_REQ_PARM	"Required parameter NULL or empty"

                                       // Strings used in call to 
                                       //   get/put info in windows
                                       //   ini file
                                       //
#define WININICODE_WINDOWS_SECTION_CODE       "windows"
#define WININICODE_A_DEVICE_KEY_CODE          "device"
#define WININICODE_DEVICES_SECTION_CODE       "devices"

                                       // Printer info structures
typedef struct SlocalPrinterInfo  
{
    char    printer_device_name_buf[SIZE_PRT_DEV_NAME];
    char    printer_driver_name_buf[SIZE_PRT_DRIVER_NAME];
    char    output_device_buf[SIZE_PRT_OUTPUT_DEVICE];
}
SlocalPrinterInfo;

                                       // Dir info structures
typedef struct SUnixDirInfo  
{
    char    FileName[NTCSSVAL_AVG_UNIX_FILE_NAME_LEN];
	char	FileDate[NTCSSVAL_UNIX_FILE_DATE_LEN];
	int     FileSize;
	char    FileOwner[NTCSSVAL_UNIX_FILE_OWNER_LEN];
}
SUnixDirInfo;


typedef struct SremotePrinterInfo  
{
    char    printer_name_buf[SIZE_PRINTER_NAME];
    char    host_name_buf[SIZE_HOST_NAME];
    char    printer_loc_buf[SIZE_PRT_LOCATION];
    char    printer_device_name_buf[SIZE_PRT_DEV_NAME];
    char    printer_driver_name_buf[SIZE_PRT_DRIVER_NAME];
    char    redirect_device_buf[SIZE_PRT_DEV_NAME];
    char    redirect_host_buf[SIZE_HOST_NAME];
    char    redirect_printer_buf[SIZE_PRT_RED];
    BOOL    default_printer_flag;
    BOOL    queue_enabled_flag;
    BOOL    printer_enabled_flag;
    char    output_spool_file[SIZE_CLIENT_LOCATION];
	char    printer_NTdriver_name_buf[SIZE_PRT_DRIVER_NAME];   //added 5/1
	char    printer_datafile_name_buf[SIZE_PRT_DRIVER_NAME];   //added 5/1
	char    printer_uifile_name_buf[SIZE_PRT_DRIVER_NAME];     //added 5/1
	char    printer_helpfile_name_buf[SIZE_PRT_DRIVER_NAME];   //added 5/1
	char    printer_monitorfile_name_buf[SIZE_PRT_DRIVER_NAME];//added 6/12
	char    printer_monitorstring_name_buf[SIZE_PRT_DRIVER_NAME];//added 6/12
	char    CopyFiles[SIZE_COPY_FILES];
	int     nCopies;                                             //added 12/3
	BOOL    bBanner;                                             //added 12/3
	char    AtlassFileName[_MAX_PATH];	                         //Atlass

}
SremotePrinterInfo;
#endif  // _INCLUDE_DLLDEFS_H_CLASS_

