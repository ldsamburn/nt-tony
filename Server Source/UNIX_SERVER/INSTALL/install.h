
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * install.h
 */

#ifndef _install_h
#define _install_h

/** SYSTEM INCLUDES **/

#include <errno.h>

/** INRI INCLUDES **/
#include <Ntcss.h>     /* a few #defs needed in here */
#include <LibNtcssFcts.h>     /* a few #defs needed in here */

/** DEFINE DEFINITIONS **/

#define MAX_INSTALL_ITEM_LEN    512  /* Similar (smaller) def is in Ntcss.h */
/*#define MAX_CMD_LEN             2048  <- now def'd in Ntcss.h */
/*#define MAX_FILENAME_LEN        1024  <- now def'd in Ntcss.h */
#define MAX_APPLINE_LEN         512

#define TOP_LEVEL_DIR           NTCSS_HOME_DIR

#ifndef VSUN
#define OWNER_GROUP             "sys"
#else
#define OWNER_GROUP             "staff"
#endif


#define NEW_FILES               "Release_Notes applications logs notes init_data database message env spool"

#define REMOVE_FILES            "Release_Notes applications/help applications/app-defaults* applications/data/install applications/data/prt_conf applications/data/pdj_conf applications/data/users_conf applications/data/unix_uga logs notes init_data/app_info/NTCSS init_data/prt_info database/ADAPTER_TYPES"

#ifdef SOLARIS
#define UPDATE_FILES            "Release_Notes applications/help applications/app-defaults.solaris applications/data/install applications/data/prt_conf applications/data/pdj_conf applications/data/users_conf logs notes init_data/app_info/NTCSS init_data/prt_info database/ADAPTER_TYPES spool/ntdrivers"
#elif VSUN
#define UPDATE_FILES            "Release_Notes applications/help applications/app-defaults.sun applications/data/install applications/data/prt_conf applications/data/pdj_conf applications/data/users_conf logs notes init_data/app_info/NTCSS init_data/prt_info database/ADAPTER_TYPES spool/ntdrivers"
#else
#define UPDATE_FILES            "Release_Notes applications/help applications/app-defaults applications/data/install applications/data/prt_conf applications/data/pdj_conf applications/data/users_conf logs notes init_data/app_info/NTCSS init_data/prt_info database/ADAPTER_TYPES spool/ntdrivers"
#endif

#define ZERO_LENG_FILES         "database/access_roles database/adapters database/current_apps database/login_history database/output_prt database/output_type database/predefined_jobs database/printer_access database/printers"

#define ZERO_LENG_BBMSG_FILES   "message/bb_msgs_09 message/bb_msgs_AE message/bb_msgs_FJ message/bb_msgs_KO message/bb_msgs_PT message/bb_msgs_UZ message/bb_msgs_ae message/bb_msgs_fj message/bb_msgs_ko message/bb_msgs_pt message/bb_msgs_uz message/bul_brds message/markers_09 message/markers_ae message/markers_fj message/markers_ko message/markers_pt message/markers_uz message/subscribers"

#define APP_INFO                0
#define HOST_INFO               1
#define PRTCLASS_INFO           2

/** TYPEDEF DEFINITIONS **/


/** GLOBAL DECLARATIONS **/

extern FILE *logp;
extern int add_unix_batch_users;


/** misc_fcts.c **/

extern void input_item(const char*, char*);
extern int copy_app_default_files(char*);
extern int is_host_master(char*);
extern int are_any_servers_running(void);

/** database.c **/

extern int init_database(char*, char*, char*, int, char*);
extern int convert_database_files(char*, char*); 
extern int are_there_newly_registered_apps(char*);
extern int process_newly_registered_apps(char*, char*, 
                                         const char*, const char*);
extern int SaveAdminsFor300Convert(char*);
extern int RestoreAdminsFor300Conversion(char*);

/** grp_fcts.c **/

extern int nis_running_check(void);
extern int add_unix_group(char*);

#endif /* _install_h */
