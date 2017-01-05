/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#ifndef _Ntcss_h
#define _Ntcss_h

#define CMD_LEN             20

#define NTCSS_NULLCH            '\0'

#define MAX_ERR_MSG_LEN             (80 * 4)
#define MAX_APP_DATA_LEN            256
#define MAX_LOGIN_NAME_LEN          8
#define MAX_PROGROUP_TITLE_LEN      16
#define MAX_HOST_NAME_LEN           16
#define MAX_PHONE_NUMBER_LEN        32
#define MAX_APP_PASSWD_LEN          32
#define MAX_TOKEN_LEN               32
#define MAX_PASSWORD_LEN            32
#define MAX_SERVER_LOCATION_LEN     128
#define MAX_CMD_LINE_ARGS_LEN       12
#define MAX_JOB_DESCRIP_LEN         256
#define MAX_SDI_CTL_RECORD_LEN      8
#define MAX_PROCESS_ID_LEN          32
#define MAX_PROC_CMD_LINE_LEN       256
#define MAX_CUST_PROC_STAT_LEN      128
#define MAX_ORIG_HOST_LEN           32
#define MAX_PRT_NAME_LEN            16
#define MAX_CLIENT_LOCATION_LEN     128
#define MAX_COMDB_DATA_LEN          128
#define MAX_COMDB_TAG_LEN           64
#define MAX_IP_ADDRESS_LEN          32
#define MAX_BB_NAME_LEN             16
#define MAX_REQUEST_ID_LEN          36
#define MAX_MSG_TITLE_LEN           32
#define MAX_MSG_LEN                 8192
#define MAX_PRT_OPTIONS_LEN         255
#define MAX_PATH_LEN                256


#define NTCSS_MASTER_SERVER_TYPE         0
#define NTCSS_APP_SERVER_TYPE            1
#define NTCSS_AUTH_SERVER_TYPE           2

#define SPQ_BIT_MASK_WITH_CUST_PROC_STATUS   1
#define SPQ_BIT_MASK_WITH_PRT_REQ_INFO       2
#define SPQ_BIT_MASK_WITH_JOB_DESCRIP        4

#define SPQ_BIT_MASK_ONLY_CERTAIN_USER       8
#define SPQ_BIT_MASK_ONLY_RUNNING_PROCS     16
#define SPQ_BIT_MASK_PRIORITY               64
#define SPQ_BIT_MASK_COMMAND_LINE              128
#define SPQ_BIT_MASK_CUST_PROC_STATUS          256
#define SPQ_BIT_MASK_LOGIN_NAME                512
#define SPQ_BIT_MASK_UNIX_OWNER               1024
#define SPQ_BIT_MASK_PROGROUP_TITLE           2048
#define SPQ_BIT_MASK_JOB_DESCRIP              4096
#define SPQ_BIT_MASK_ORIG_HOST                8192

#define DB_SVR_PORT     9132
#define SPQ_SVR_PORT    9162
#define CMD_SVR_PORT    9152
#define MSG_SVR_PORT    9172

#define UNCLASSIFIED_CLEARANCE             0
#define UNCLASSIFIED_SENSITIVE_CLEARANCE   1
#define CONFIDENTIAL_CLEARANCE             2
#define SECRET_CLEARANCE                   3
#define TOPSECRET_CLEARANCE                4

#define LOW_PRIORITY                  1
#define MEDIUM_PRIORITY               2
#define HIGH_PRIORITY                 3


#define MAX_FILENAME_LEN        1024

#define ERROR_CANT_DETER_MASTER                         -44

// From NetCopy.h
#define         XFER_ASCII              0

#endif /* _Ntcss_h */