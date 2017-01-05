
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * TextDBP.h
 */

#ifndef _TextDBP_h
#define _TextDBP_h

/** DEFINE DEFINITIONS **/

#define MAX_ARRAY_SIZE          64
#define MAX_ITEM_LEN            256

/* #define MAX_FILENAME_LEN     512  <- Redef'd in Ntcss.h   */
/* #define MAX_CMD_LEN          1024  <- Redef'd in Ntcss.h  */

#define ONLINE_TO_TMP           0
#define TMP_TO_ONLINE           1
#define REMOVE_TMP              2
#define INIT_TO_TMP             3
#define REMOVE_OLD              4
#define MOVE_EXISTING_TMP_FILES_ONLINE 5

#define NUM_TABLES              16

/*
 * Table parameters.
 */
#define NTCSS_USERS_ID          0
#define NTCSS_USERS_NUM_RECS    12
#define NTCSS_USERS_FMT         "%d\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n%d\n%d\n%s\n%s\n\n"

#define PROGROUPS_ID            1
#define PROGROUPS_NUM_RECS      11
#define PROGROUPS_FMT           "%s\n%s\n%s\n%d\n%s\n%s\n%s\n%s\n%s\n%d\n%s\n\n"

#define PROGRAMS_ID             2
#define PROGRAMS_NUM_RECS       10
#define PROGRAMS_FMT            "%s\n%s\n%s\n%d\n%s\n%s\n%d\n%d\n%d\n%s\n\n"

#define APPUSERS_ID             3
#define APPUSERS_NUM_RECS       7
#define APPUSERS_FMT            "%s\n%s\n%s\n%s\n%s\n%d\n%d\n\n"

/***  --------------------- REMOVED -------------------------
  #define ACCESS_AUTH_ID          4
  #define ACCESS_AUTH_NUM_RECS    4
  #define ACCESS_AUTH_FMT         "%s\n%s\n%s\n%s\n\n"
     --------------------------------------------------------
***/

#define PRINTERS_ID             4
#define PRINTERS_NUM_RECS       12 /*ATLAS 11*/
#define PRINTERS_FMT            "%s\n%s\n%s\n%d\n%s\n%s\n%d\n%d\n%s\n%d\n%s\n%s\n\n" /*ATLAS*/

#define ADAPTERS_ID             5
#define ADAPTERS_NUM_RECS       4
#define ADAPTERS_FMT            "%s\n%s\n%s\n%s\n\n"

#define PRT_CLASS_ID            6
#define PRT_CLASS_NUM_RECS      3
#define PRT_CLASS_FMT           "%s\n%s\n%s\n\n"

#define PRINTER_ACCESS_ID       7
#define PRINTER_ACCESS_NUM_RECS 3
#define PRINTER_ACCESS_FMT      "%s\n%s\n%s\n\n"

#define OUTPUT_TYPE_ID          8
#define OUTPUT_TYPE_NUM_RECS    5
#define OUTPUT_TYPE_FMT         "%s\n%s\n%s\n%s\n%d\n\n"

#define OUTPUT_PRT_ID           9
#define OUTPUT_PRT_NUM_RECS     4
#define OUTPUT_PRT_FMT          "%s\n%s\n%s\n%s\n\n"

#define HOSTS_ID                10
#define HOSTS_NUM_RECS          5 /* 98-08-06 GWY Changed from 3 to 4 for type */
/* 98-08-06 GWY Added \n%d to end of HOSTS_FMT for server type, see HostItem in Ntcss.h */
#define HOSTS_FMT               "%s\n%s\n%d\n%d\n%d\n\n"

#define CURRENT_USERS_ID        11
#define CURRENT_USERS_NUM_RECS  6 /* 98-08-07 GWY Changed from 5 to 6 for reserve_time */
/* 98-08-07 GWY Added \n%s to CURRENT_USERS_FMT for reserve_time */
#define CURRENT_USERS_FMT       "%s\n%s\n%s\n%s\n%d\n%s\n\n"

#define LOGIN_HISTORY_ID        12
#define LOGIN_HISTORY_NUM_RECS  4
#define LOGIN_HISTORY_FMT       "%s\n%d\n%s\n%s\n\n"

#define ACCESS_ROLES_ID         13
#define ACCESS_ROLES_NUM_RECS   4
#define ACCESS_ROLES_FMT        "%s\n%s\n%d\n%d\n\n"


#define CURRENT_APPS_ID         14
#define CURRENT_APPS_NUM_RECS   4
#define CURRENT_APPS_FMT        "%s\n%s\n%d\n%d\n\n"

#define PREDEFINED_JOB_ID       15
#define PREDEFINED_JOB_NUM_RECS 6
#define PREDEFINED_JOB_FMT      "%s\n%s\n%s\n%d\n%d\n%s\n\n"

#define PRT_REQUESTS_TABLE      "prt_requests"
#define ACTIVE_PROCS_TABLE      "active_procs"
#define SCHEDULED_PROCS_TABLE   "scheduled_procs"

#define BB_MSGS_TABLE           "bb_msgs_"
#define BUL_BRDS_TABLE          "bul_brds"
#define MARKERS_TABLE           "markers_"
#define SUBSCRIBERS_TABLE       "subscribers"

/*
 * Access role operations.
 */
#define LAST_ACCESS_OP          0
#define MOVE_ACCESS_OP          1

/*
 * Message Bulletin Board Table IDs.
 */
#define BUL_BRDS_TABLE_ID       0
#define SUBSCRIBERS_TABLE_ID    1
#define BB_MSGS_TABLE_ID        2
#define MARKERS_TABLE_ID        3

/** TYPEDEF DEFINITIONS **/


/** GLOBAL DECLARATIONS **/

extern const char *tables[];



#endif /* _TextDBP_h */
