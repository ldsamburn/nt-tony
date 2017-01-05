/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\sizes.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file contains NTCSS size definitions.  Note that some
// of these definitions have different but corresponding settings in
// header file ..\comn_src\ntcssapi.h
//----------------------------------------------------------------------

#ifndef _DEFINE_SIZES_H_
#define _DEFINE_SIZES_H_

#define SIZE_ADMINLOCK_OPS          2
#define SIZE_APP_DATA               256
#define SIZE_APP_NAME               16
#define SIZE_APP_PASSWORD           32
#define SIZE_ADMIN_PID				8
#define SIZE_ACCESS_ROLE			32
#define SIZE_ALTER_MASK				6



#define SIZE_CLIENT_LOCATION        128
#define SIZE_CMD_LINE               256
#define SIZE_CMD_LINE_ARGS          128
#define SIZE_COMMON_DATA            128
#define	SIZE_COPY_FILES             2048
#define SIZE_COMMON_TAG              64
#define SIZE_CUSTOM_PROC_STAT       256
#define SIZE_DATETIME               27  
#define SIZE_DEBUG_TRACE_BUF        80
#define SIZE_PROC_STAT				128

#define SIZE_GROUP_TITLE            16
#define SIZE_HOST_NAME              16
#define SIZE_JOB_DESCRIP           256
#define SIZE_JOB_TYPE              2

#define SIZE_ICON_FILE             128
#define SIZE_IP_ADDR                16
#define SIZE_LAUNCH_TIME            13   
#define SIZE_LOGIN_NAME             8


#define SIZE_MSGBOARD_KEY           22  
#define SIZE_MSGBOARD_NAME          16  
#define SIZE_MSG_POSTDATE			 8
#define SIZE_MSG_TEXT              512  
#define SIZE_MSG_TITLE              32  

#define SIZE_MESSAGE_TYPE           20  
#define SIZE_NUM_GROUPS_PER_USER    10
#define SIZE_NUM_PROGS_PER_PROGROUP 10
#define SIZE_PRT_OUTPUT_TYPE        16
#define SIZE_ORIG_HOST              32   

#define SIZE_PASSWORD               32 //watch out this was 30 check impact
#define SIZE_PASSWORD_CHANGE_TIME   12
#define SIZE_PHONE_NUMBER           32
#define SIZE_PRINTER_NAME           16
#define SIZE_PRIORITY               2
 
#define SIZE_PROCESS_STATE          2
#define SIZE_PROCESS_ID             32
#define SIZE_PROGRAM_FILE           32
#define SIZE_PROGRAM_TITLE          64
#define SIZE_PRT_DEV_NAME           128
#define SIZE_PRT_DRIVER_NAME        32    
#define SIZE_PRT_FILE_NAME          128  
#define SIZE_PRT_FLAG               2  
#define SIZE_PRT_LOCATION           64 
#define SIZE_PRT_OUTPUT_DEVICE      32
#define SIZE_PRT_RED                64   
#define SIZE_PRT_REQ_ID             36 
#define SIZE_PRT_REQ_TIME           11    

#define SIZE_REAL_NAME              64
#define SIZE_REQUEST_PROGRAM        30
#define SIZE_RELEASE_DATE           12
#define SIZE_RESPONSE_CODE          20  
#define SIZE_SCHEDULE               128
#define SIZE_SCHEDULE_ENTRY         10

#define SIZE_SSN                    12
#define SIZE_SECURITY_CLASS          2                                        
#define SIZE_SERVER_LOCATION        128
#define SIZE_SERVER_FILE_NAME       256
#define SIZE_SIGNAL_TYPE            2    
#define SIZE_SYSTEM_PROC_STAT       64   
#define SIZE_TOKEN                  32 
#define SIZE_VERSION_NUMBER         32
#define SIZE_CHAR_TO_INT            8

                                    // Buffer size definitions 
#define SIZE_TRANSPORT_GENBUF		2048
#define SIZE_BIG_GENBUF				512
#define SIZE_STANDARD_GENBUF		256
#define SIZE_SMALL_GENBUF			128
#define SIZE_TINY_GENBUF			32
#define SIZE_OF_AN_INT              2
#define FULL_TIME_STR_LEN           13
#define DATETIME_LEN                14
/////////////////////////////////////////////
#define SIZE_WORKING_DIR			128
#define SIZE_NUM_PROGROUPS_PER_NTCSS_USER 256
#define SIZE_UNLOCKED_APPS          int((SIZE_GROUP_TITLE + 1) * SIZE_NUM_PROGROUPS_PER_NTCSS_USER)



// WSPTODO
// Eventually will not need this one:

#define SIZE_STND_DATABUF_MSGPART  256

#endif // _DEFINE_SIZES_H_




