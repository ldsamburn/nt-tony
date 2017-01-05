
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Ntcss.h
 */

#ifndef _Ntcss_h
#define _Ntcss_h

/** MOTIF INCLUDES **/

/* NOTE: */
/* The function getNtcssSystemSettingTempPtr takes a default value as its 
   second parameter.  This parameter will be used if the Tag, first parameter,
   cannot be successfully looked up in shared memory.  This could occur for
   two reasons, the tag was not found in shared memory, or there is a problem
   accessing shared memory.  The only defaults that have been supplied so far are
   the ones needed to run fs_archive and unix_uga stand alone.
   */

/** SYSTEM INCLUDES **/

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/sem.h>


/** INRI INCLUDES **/
/*
  WARNING!!!  This file is delivered with our releases. So avoid adding any
  "includes" here that include any of our header files, because those files
  will also need to be delivered with our releases.  
*/

/** DEFINE DEFINITIONS **/

/*
 * Special copyright strings.
 */
#define NTCSS_TITLE      "Naval Tactical Command Support System"
#define NTCSS_COPYRIGHT1 "(c) Copyright Inter-National Research Institute, Inc. (1997)"
#define NTCSS_COPYRIGHT2 "All Rights Reserved"
#define NTCSS_COPYRIGHT3 "This material may be reproduced by or for the U.S. Government pursuant to"
#define NTCSS_COPYRIGHT4 "the copyright license under the clause at DFARS 252.227-7013 (OCT 1988)."


/* NOTE: These values are also defined in 
         StringFcts.c */
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

/* System Settings Defines */
/* These must be #defines and not macros using getNtcssSystemSettingTempPtr
   because these defines are used by the function loadSystemSettings to
   initialize the shared memory the getNtcssSystemSettingTempPtr function uses */
#define NTCSS_SS_MAX_KEYS        180
#define NTCSS_SS_MAX_KEY_LEN      50
#define NTCSS_SS_MAX_VALUE_LEN   100
#define NTCSS_SS_SHMEM_KEY       500
#define NTCSS_SS_NUM_TMP_STRINGS 250
#define NTCSS_SS_SECTION_HEADER "NTCSS_SYSTEM_SETTINGS"
#define NTCSS_SS_INI_FILE       "/etc/ntcss_system_settings.ini"
#define NTCSS_SS_END_STRING     "END_NTCSS_SS_LIST"
#define NTCSS_SS_DEFAULT_STR    "NoSystemSetting"
#define NTCSS_LEGACY_DEFAULT    "/usr/local/NTCSSII"
#define NTCSS_NULLCH		'\0' 
/* END System Settings Defines */

/*
 * Directories.
 */
#define NTCSS_INIT_DATA_DIR        getNtcssSystemSettingTempPtr("NTCSS_INIT_DATA_DIR", "/h/NTCSSS/init_data")
#define NTCSS_BIN_DIR              getNtcssSystemSettingTempPtr("NTCSS_BIN_DIR", "bin")
#define NTCSS_DB_DIR               getNtcssSystemSettingTempPtr("NTCSS_DB_DIR","/h/NTCSSS/database")
#define NTCSS_HELP_INI_DIR         getNtcssSystemSettingTempPtr("NTCSS_HELP_INI_DIR", "/h/NTCSSS/applications/data/help_ini")
#define NTCSS_HOME_DIR             getNtcssSystemSettingTempPtr("NTCSS_HOME_DIR", "/h/NTCSSS")
#define NTCSS_LOCK_FILES_DIR       getNtcssSystemSettingTempPtr("NTCSS_LOCK_FILES_DIR", NTCSS_SS_DEFAULT_STR)
#define NTCSS_LOGS_DIR             getNtcssSystemSettingTempPtr("NTCSS_LOGS_DIR", "logs")
#define NTCSS_LP_SPOOL_DEFAULT_DIR getNtcssSystemSettingTempPtr("NTCSS_LP_SPOOL_DEFAULT_DIR", NTCSS_SS_DEFAULT_STR)
#define NTCSS_MSG_DIR              getNtcssSystemSettingTempPtr("NTCSS_MSG_DIR", NTCSS_SS_DEFAULT_STR)
#define NTDRIVER_DIR               getNtcssSystemSettingTempPtr("NTDRIVER_DIR", NTCSS_SS_DEFAULT_STR)
#define NTCSS_UNIX_USERS_DIR       getNtcssSystemSettingTempPtr("NTCSS_UNIX_USERS_DIR", NTCSS_SS_DEFAULT_STR)
#define SDI_DATA_FILES_DIR         getNtcssSystemSettingTempPtr("SDI_DATA_FILES_DIR", NTCSS_SS_DEFAULT_STR)
#define SDI_DIR                    getNtcssSystemSettingTempPtr("SDI_DIR", NTCSS_SS_DEFAULT_STR)
#define SDI_TCI_FILES_DIR          getNtcssSystemSettingTempPtr("SDI_TCI_FILES_DIR", NTCSS_SS_DEFAULT_STR)
#define USER_CONFIG_DIR            getNtcssSystemSettingTempPtr("USER_CONFIG_DIR", NTCSS_SS_DEFAULT_STR)
#define APP_DEFAULTS_DIR           getNtcssSystemSettingTempPtr("APP_DEFAULTS_DIR", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SPOOL_DIR            getNtcssSystemSettingTempPtr("NTCSS_SPOOL_DIR", NTCSS_SS_DEFAULT_STR)
#define HOSTS_INFO_DIR_PATH getNtcssSystemSettingTempPtr("HOSTS_INFO_DIR_PATH", NTCSS_SS_DEFAULT_STR)
#define NTCSS_LEGACY_DIR getNtcssSystemSettingTempPtr("NTCSS_LEGACY_DIR", NTCSS_LEGACY_DEFAULT)
#define TMP_DB_BACKUP_TAR_DIR  getNtcssSystemSettingTempPtr("TMP_DB_BACKUP_TAR_DIR", NTCSS_SS_DEFAULT_STR)

/*
 *  Files.
 */
         /* misc files */
/* need to make this file random name, use pid or something */
#define TMP_MAP_FILE               getNtcssSystemSettingTempPtr("TMP_MAP_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_INIT_APP_DATA        getNtcssSystemSettingTempPtr("NTCSS_INIT_APP_DATA", NTCSS_SS_DEFAULT_STR)
#define NTCSS_INIT_HOSTS_FILE      getNtcssSystemSettingTempPtr("NTCSS_INIT_HOSTS_FILE", NTCSS_SS_DEFAULT_STR)

#define NTCSS_OTYPE_CHANGE_FILE    getNtcssSystemSettingTempPtr("NTCSS_OTYPE_CHANGE_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_OTYPE_INI_FILE       getNtcssSystemSettingTempPtr("NTCSS_OTYPE_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_OTYPE_SQL_FILE       getNtcssSystemSettingTempPtr("NTCSS_OTYPE_SQL_FILE", NTCSS_SS_DEFAULT_STR)

#define NTCSS_PDJ_CHANGE_FILE      getNtcssSystemSettingTempPtr("NTCSS_PDJ_CHANGE_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PDJ_INI_FILE         getNtcssSystemSettingTempPtr("NTCSS_PDJ_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PDJ_SQL_FILE         getNtcssSystemSettingTempPtr("NTCSS_PDJ_SQL_FILE", NTCSS_SS_DEFAULT_STR)

#define DB_NTCSS_USERS_TABLE_PATHNAME     getNtcssSystemSettingTempPtr("DB_NTCSS_USERS_TABLE_PATHNAME", NTCSS_SS_DEFAULT_STR)
#define DB_TMP_NTCSS_USERS_TABLE_PATHNAME getNtcssSystemSettingTempPtr("DB_TMP_NTCSS_USERS_TABLE_PATHNAME", NTCSS_SS_DEFAULT_STR)
#define DB_NTCSS_APPUSERS_TABLE_PATHNAME  getNtcssSystemSettingTempPtr("DB_NTCSS_APPUSERS_TABLE_PATHNAME", NTCSS_SS_DEFAULT_STR)
#define DB_TMP_NTCSS_APPUSERS_TABLE_PATHNAME getNtcssSystemSettingTempPtr("DB_TMP_NTCSS_APPUSERS_TABLE_PATHNAME", NTCSS_SS_DEFAULT_STR)
#define HOSTS_SQL_FILEPATH              getNtcssSystemSettingTempPtr("HOSTS_SQL_FILEPATH", NTCSS_SS_DEFAULT_STR)
#define PRIME_DB_LOG_FILEPATH           getNtcssSystemSettingTempPtr("PRIME_DB_LOG_FILEPATH", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PROGRP_INI                getNtcssSystemSettingTempPtr("NTCSS_PROGRP_INI", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PROGRP_INI_FILE           getNtcssSystemSettingTempPtr("NTCSS_PROGRP_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PROGRP_DB_FILE            getNtcssSystemSettingTempPtr("NTCSS_PROGRP_DB_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PROGRP_ID_FILE            getNtcssSystemSettingTempPtr("NTCSS_PROGRP_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_VERSION_FILE              getNtcssSystemSettingTempPtr("NTCSS_VERSION_FILE","applications/data/install/VERSION")
#define GROUP_FILE                      getNtcssSystemSettingTempPtr("GROUP_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSLOG_FILE               getNtcssSystemSettingTempPtr("NTCSS_SYSLOG_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_DB_CONVERT_30_USERS_FILE  getNtcssSystemSettingTempPtr("NTCSS_DB_CONVERT_30_USERS_FILE", NTCSS_SS_DEFAULT_STR)


/* Resource Admin related files */
	/* User Admin */
#define USR_ADMIN_INFO_FILE             getNtcssSystemSettingTempPtr("USR_ADMIN_INFO_FILE", NTCSS_SS_DEFAULT_STR)
#define USR_ADMIN_APPLY_FILE            getNtcssSystemSettingTempPtr("USR_ADMIN_APPLY_FILE", NTCSS_SS_DEFAULT_STR)
#define USR_ADMIN_APPLY_UNDO_PATHFILE   getNtcssSystemSettingTempPtr("USR_ADMIN_APPLY_UNDO_PATHFILE", NTCSS_SS_DEFAULT_STR)
#define USR_ADMIN_APPLY_LOG_FILE        getNtcssSystemSettingTempPtr("USR_ADMIN_APPLY_LOG_FILE", NTCSS_SS_DEFAULT_STR)
#define USR_ADMIN_APPLY_LOG_FILE_PATH   getNtcssSystemSettingTempPtr("USR_ADMIN_APPLY_LOG_FILE_PATH", NTCSS_SS_DEFAULT_STR)
	/* Printer Admin */
#define PRT_ADMIN_INFO_FILE             getNtcssSystemSettingTempPtr("PRT_ADMIN_INFO_FILE", NTCSS_SS_DEFAULT_STR)
#define PRT_ADMIN_APPLY_FILE            getNtcssSystemSettingTempPtr("PRT_ADMIN_APPLY_FILE", NTCSS_SS_DEFAULT_STR)
#define PRT_ADMIN_APPLY_UNDO_PATHFILE   getNtcssSystemSettingTempPtr("PRT_ADMIN_APPLY_UNDO_PATHFILE", NTCSS_SS_DEFAULT_STR)
#define PRT_ADMIN_APPLY_LOG_FILE        getNtcssSystemSettingTempPtr("PRT_ADMIN_APPLY_LOG_FILE", NTCSS_SS_DEFAULT_STR)
#define PRT_ADMIN_APPLY_LOG_FILE_PATH   getNtcssSystemSettingTempPtr("PRT_ADMIN_APPLY_LOG_FILE_PATH", NTCSS_SS_DEFAULT_STR)


         /* backup related files */
#define NTCSS_BACKUP_INFO_FILE      getNtcssSystemSettingTempPtr("NTCSS_BACKUP_INFO_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_BACKUP_LIST_FILE      getNtcssSystemSettingTempPtr("NTCSS_BACKUP_LIST_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_BACKUP_TAR_FILE       getNtcssSystemSettingTempPtr("NTCSS_BACKUP_TAR_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_BACKUP_TEMP_TAR_FILE  getNtcssSystemSettingTempPtr("NTCSS_BACKUP_TEMP_TAR_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_BACKUP_TEMP_INFO_FILE getNtcssSystemSettingTempPtr("NTCSS_BACKUP_TEMP_INFO_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_MASTER_BACKUP_STATUS_FILE    getNtcssSystemSettingTempPtr("NTCSS_MASTER_BACKUP_STATUS_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_DATA_INSTALL_DIR    getNtcssSystemSettingTempPtr("NTCSS_DATA_INSTALL_DIR", NTCSS_SS_DEFAULT_STR)

        /* FS Archive Files */
#define FS_ARCH_LOCK_DIR           getNtcssSystemSettingTempPtr("FS_ARCH_LOCK_DIR", NTCSS_SS_DEFAULT_STR)


        /* SRV_DEV Files */
#define SRV_DEV_LOCK_DIR            getNtcssSystemSettingTempPtr("SRV_DEV_LOCK_DIR", NTCSS_SS_DEFAULT_STR)

        /* lock related files */
#define APP_PDJ_CONF_LOCK_FILE     getNtcssSystemSettingTempPtr("APP_PDJ_CONF_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define APP_PRT_CONF_LOCK_FILE     getNtcssSystemSettingTempPtr("APP_PRT_CONF_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define CHECK_APP_LOCK_FILE        getNtcssSystemSettingTempPtr("CHECK_APP_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define DEVICE_LOCK_FILE           getNtcssSystemSettingTempPtr("DEVICE_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define OTYPE_ADMIN_LOCK_FILE      getNtcssSystemSettingTempPtr("OTYPE_ADMIN_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define LOG_HISTORY_LOCK_ID_FILE   getNtcssSystemSettingTempPtr("LOG_HISTORY_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define MAIN_DB_LOCK_ID_FILE       getNtcssSystemSettingTempPtr("MAIN_DB_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define MSG_BB_LOCK_ID_FILE        getNtcssSystemSettingTempPtr("MSG_BB_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define PDJ_ADMIN_LOCK_FILE        getNtcssSystemSettingTempPtr("PDJ_ADMIN_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define PRINTER_ADMIN_LOCK_FILE    getNtcssSystemSettingTempPtr("PRINTER_ADMIN_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define PRINT_CONFIG_LOCK_ID_FILE  getNtcssSystemSettingTempPtr("PRINT_CONFIG_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define SYSTEM_CONFIG_LOCK_ID_FILE getNtcssSystemSettingTempPtr("SYSTEM_CONFIG_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define SYSMON_LOCK_ID_FILE        getNtcssSystemSettingTempPtr("SYSMON_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define SYSTEM_MONITOR_LOCK_FILE   getNtcssSystemSettingTempPtr("SYSTEM_MONITOR_LOCK_FILE", NTCSS_SS_DEFAULT_STR)
#define USER_ADMIN_LOCK_ID_FILE    getNtcssSystemSettingTempPtr("USER_ADMIN_LOCK_ID_FILE", NTCSS_SS_DEFAULT_STR)
#define USER_ADMIN_LOCK_FILE       getNtcssSystemSettingTempPtr("USER_ADMIN_LOCK_FILE", NTCSS_SS_DEFAULT_STR)

        /* printing related files */
#define ADAPTER_TYPES_FILE         getNtcssSystemSettingTempPtr("ADAPTER_TYPES_FILE", NTCSS_SS_DEFAULT_STR)
#define NT_INI_FILE                getNtcssSystemSettingTempPtr("NT_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRINTER_CHANGE_FILE getNtcssSystemSettingTempPtr("NTCSS_PRINTER_CHANGE_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRINTER_INI_FILE     getNtcssSystemSettingTempPtr("NTCSS_PRINTER_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRINTER_SQL_FILE     getNtcssSystemSettingTempPtr("NTCSS_PRINTER_SQL_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRNT_SEQUENCE_FILE   getNtcssSystemSettingTempPtr("NTCSS_PRNT_SEQUENCE_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRT_CLASS_TABLE      getNtcssSystemSettingTempPtr("NTCSS_PRT_CLASS_TABLE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRT_CONF_DIR         getNtcssSystemSettingTempPtr("NTCSS_PRT_CONF_DIR", NTCSS_SS_DEFAULT_STR)
#define PRINTER_RECON_INI_FILE     getNtcssSystemSettingTempPtr("PRINTER_RECON_INI_FILE", NTCSS_SS_DEFAULT_STR)

        /* sdi related files */
#define NTCSS_DEVICES_FILE         getNtcssSystemSettingTempPtr("NTCSS_DEVICES_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_TAPE_CTL_FILE        getNtcssSystemSettingTempPtr("NTCSS_TAPE_CTL_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_TAPE_SPECS_FILE      getNtcssSystemSettingTempPtr("NTCSS_TAPE_SPECS_FILE", NTCSS_SS_DEFAULT_STR)
        /* sysmon related files */
#define NTCSS_SYSMON_HOSTS_INI_FILE            getNtcssSystemSettingTempPtr("NTCSS_SYSMON_HOSTS_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSMON_LOGIN_HISTORY_INI_FILE    getNtcssSystemSettingTempPtr("NTCSS_SYSMON_LOGIN_HISTORY_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSMON_PRINTERS_INI_FILE         getNtcssSystemSettingTempPtr("NTCSS_SYSMON_PRINTERS_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSMON_PROGROUPS_INI_FILE        getNtcssSystemSettingTempPtr("NTCSS_SYSMON_PROGROUPS_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSMON_SYSTEM_LOG_INI_FILE       getNtcssSystemSettingTempPtr("NTCSS_SYSMON_SYSTEM_LOG_INI_FILE", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SYSMON_USERS_INI_FILE            getNtcssSystemSettingTempPtr("NTCSS_SYSMON_USERS_INI_FILE", NTCSS_SS_DEFAULT_STR)

/* HELP FILE NAMES */

#define HELP_TOPICS_FILE_NAME       getNtcssSystemSettingTempPtr("HELP_TOPICS_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define GETTING_HELP_FILE_NAME      getNtcssSystemSettingTempPtr("GETTING_HELP_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define WINDOW_HELP_FILE_NAME       getNtcssSystemSettingTempPtr("WINDOW_HELP_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define FIELD_HELP_FILE_NAME        getNtcssSystemSettingTempPtr("FIELD_HELP_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define BOOKS_FILE_NAME             getNtcssSystemSettingTempPtr("BOOKS_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define USER_GUIDE_FILE_NAME        getNtcssSystemSettingTempPtr("USER_GUIDE_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define DATA_ELEMENTS_FILE_NAME     getNtcssSystemSettingTempPtr("DATA_ELEMENTS_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define DATABASE_SPEC_FILE_NAME     getNtcssSystemSettingTempPtr("DATABASE_SPEC_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define INTERFACE_DESIGN_FILE_NAME  getNtcssSystemSettingTempPtr("INTERFACE_DESIGN_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define SYSTEM_ADMIN_FILE_NAME      getNtcssSystemSettingTempPtr("SYSTEM_ADMIN_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define SECURITY_PLAN_FILE_NAME     getNtcssSystemSettingTempPtr("SECURITY_PLAN_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define SECURITY_CONCEPT_FILE_NAME  getNtcssSystemSettingTempPtr("SECURITY_CONCEPT_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define SITE_SECURITY_FILE_NAME     getNtcssSystemSettingTempPtr("SITE_SECURITY_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define TECHNICAL_SUPPORT_FILE_NAME getNtcssSystemSettingTempPtr("TECHNICAL_SUPPORT_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define VERSION_FILE_NAME           getNtcssSystemSettingTempPtr("VERSION_FILE_NAME", NTCSS_SS_DEFAULT_STR)
#define NTCSS_TOP_LEVEL_DIRECTORY   getNtcssSystemSettingTempPtr("NTCSS_TOP_LEVEL_DIRECTORY", NTCSS_SS_DEFAULT_STR)
#define NTCSS_HELP_INI_DIR_TAG      getNtcssSystemSettingTempPtr("NTCSS_HELP_INI_DIR", NTCSS_SS_DEFAULT_STR)

/*
 * Programs.
 */
#define NTCSS_CHANGE_USER_PROGRAM  getNtcssSystemSettingTempPtr("NTCSS_CHANGE_USER_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_CMD_PROG             getNtcssSystemSettingTempPtr("NTCSS_CMD_PROG", NTCSS_SS_DEFAULT_STR)
#define NTCSS_INI_PROGRAM          getNtcssSystemSettingTempPtr("NTCSS_INI_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_LP_PROG              getNtcssSystemSettingTempPtr("NTCSS_LP_PROG", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PRELAUNCH_PROGRAM    getNtcssSystemSettingTempPtr("NTCSS_PRELAUNCH_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_PREPSP_PROGRAM       getNtcssSystemSettingTempPtr("NTCSS_PREPSP_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SET_PID_PROGRAM      getNtcssSystemSettingTempPtr("NTCSS_SET_PID_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_SET_STATUS_PROGRAM   getNtcssSystemSettingTempPtr("NTCSS_SET_STATUS_PROGRAM", NTCSS_SS_DEFAULT_STR)
#define NTCSS_START_BOOT_JOBS_PROG getNtcssSystemSettingTempPtr("NTCSS_START_BOOT_JOBS_PROG", NTCSS_SS_DEFAULT_STR)
#define SDI_IN_PROG                getNtcssSystemSettingTempPtr("SDI_IN_PROG", NTCSS_SS_DEFAULT_STR)
#define SDI_OUT_PROG               getNtcssSystemSettingTempPtr("SDI_OUT_PROG", NTCSS_SS_DEFAULT_STR)
#define TAR_PROG                   getNtcssSystemSettingTempPtr("TAR_PROG","/bin/tar")
#define GTAR_PROG                  getNtcssSystemSettingTempPtr("GTAR_PROG","/usr/local/bin/gtar")
#define DU_PROG                    getNtcssSystemSettingTempPtr("DU_PROG","/bin/du -s")
#define PS_PROG		           getNtcssSystemSettingTempPtr("PS_PROG", NTCSS_SS_DEFAULT_STR)
#define RM_PROG                    getNtcssSystemSettingTempPtr("RM_PROG","/bin/rm -rf")
#define CAT_PROG                   getNtcssSystemSettingTempPtr("CAT_PROG","/bin/cat")
#define MKDIR_PROG                 getNtcssSystemSettingTempPtr("MKDIR_PROG", NTCSS_SS_DEFAULT_STR)
#define MV_PROG                    getNtcssSystemSettingTempPtr("MV_PROG","/bin/mv")
#define CHMOD_PROG                 getNtcssSystemSettingTempPtr("CHMOD_PROG", NTCSS_SS_DEFAULT_STR)
#define CHGRP_PROG                 getNtcssSystemSettingTempPtr("CHGRP_PROG", NTCSS_SS_DEFAULT_STR)
#define ECHO_PROG                  getNtcssSystemSettingTempPtr("ECHO_PROG", NTCSS_SS_DEFAULT_STR)
#define TOUCH_PROG                 getNtcssSystemSettingTempPtr("TOUCH_PROG", NTCSS_SS_DEFAULT_STR)
#define CP_DIR_PROG                getNtcssSystemSettingTempPtr("CP_DIR_PROG", NTCSS_SS_DEFAULT_STR)
#define CP_PROG                    getNtcssSystemSettingTempPtr("CP_PROG","/bin/cp")
#define PR_PROG                    getNtcssSystemSettingTempPtr("PR_PROG", NTCSS_SS_DEFAULT_STR)
#define LP_PROG                    getNtcssSystemSettingTempPtr("LP_PROG", NTCSS_SS_DEFAULT_STR)
#define LP_CMD                     getNtcssSystemSettingTempPtr("LP_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_BASE_SPOOL_DIR          getNtcssSystemSettingTempPtr("LP_BASE_SPOOL_DIR", NTCSS_SS_DEFAULT_STR)
#define LP_INTERFACE_DIR          getNtcssSystemSettingTempPtr("LP_INTERFACL_DIR", NTCSS_SS_DEFAULT_STR)
#define MORE_PROG                  getNtcssSystemSettingTempPtr("MORE_PROG", NTCSS_SS_DEFAULT_STR)
#define DATE_PROG                  getNtcssSystemSettingTempPtr("DATE_PROG", "/bin/date")
#define PR_FMT_FILE                getNtcssSystemSettingTempPtr("PR_FMT_FILE", NTCSS_SS_DEFAULT_STR)
#define TMP_NTCSS_USERS            getNtcssSystemSettingTempPtr("TMP_NTCSS_USERS", NTCSS_SS_DEFAULT_STR)
#define TMP_APP_USERS              getNtcssSystemSettingTempPtr("TMP_APP_USERS", NTCSS_SS_DEFAULT_STR)
#define TMP_INI                    getNtcssSystemSettingTempPtr("TMP_INI", NTCSS_SS_DEFAULT_STR)
#define TMP_SPOOL                  getNtcssSystemSettingTempPtr("TMP_SPOOL", NTCSS_SS_DEFAULT_STR)
#define MT_PROG                    getNtcssSystemSettingTempPtr("MT_PROG","/bin/mt -t")
#define MAIL_PROG                  getNtcssSystemSettingTempPtr("MAIL_PROG","/usr/bin/mailx")
#define CRON_LIST_PROG             getNtcssSystemSettingTempPtr("CRON_LIST_PROG","/usr/bin/crontab -l")
#define CRON_ADD_PROG              getNtcssSystemSettingTempPtr("CRON_ADD_PROG","/usr/bin/crontab")
#define REWIND_CMD                 getNtcssSystemSettingTempPtr("REWIND_CMD","rew")
#define EJECT_CMD                  getNtcssSystemSettingTempPtr("EJECT_CMD","offl")
#define PUSH_NTCSS_NIS_MAPS_SCRIPT getNtcssSystemSettingTempPtr("PUSH_NTCSS_NIS_MAPS_SCRIPT",  NTCSS_SS_DEFAULT_STR)
#define MAKE_MAP_LIST_SCRIPT getNtcssSystemSettingTempPtr("MAKE_MAP_LIST_SCRIPT",  NTCSS_SS_DEFAULT_STR)
#define YPXFR                      getNtcssSystemSettingTempPtr("YPXFR",  NTCSS_SS_DEFAULT_STR)
#define NIS_DIR                    getNtcssSystemSettingTempPtr("NIS_DIR",  NTCSS_SS_DEFAULT_STR)
#define NISMAP_DIR                 getNtcssSystemSettingTempPtr("NISMAP_DIR",  NTCSS_SS_DEFAULT_STR)

/* LP_OPS Defines */
#define LP_SHUT_CMD                getNtcssSystemSettingTempPtr("LP_SHUT_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_START_CMD               getNtcssSystemSettingTempPtr("LP_START_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_ENABLE_CMD              getNtcssSystemSettingTempPtr("LP_ENABLE_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_DISABLE_CMD             getNtcssSystemSettingTempPtr("LP_DISABLE_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_ACCEPT_CMD              getNtcssSystemSettingTempPtr("LP_ACCEPT_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_REJECT_CMD              getNtcssSystemSettingTempPtr("LP_REJECT_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_ISACCEPT_STATUS_CMD     getNtcssSystemSettingTempPtr("LP_ISACCEPT_STATUS_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_ISENABLE_STATUS_CMD     getNtcssSystemSettingTempPtr("LP_ISENABLE_STATUS_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_ISSCHED_CMD             getNtcssSystemSettingTempPtr("LP_ISSCHED_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_EXISTS_INTERFACE_CMD    getNtcssSystemSettingTempPtr("LP_EXISTS_INTERFACE_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_EXISTS_REQUEST_CMD      getNtcssSystemSettingTempPtr("LP_EXISTS_REQUEST_CMD", NTCSS_SS_DEFAULT_STR)
#define LP_EXISTS_MEMBER_CMD       getNtcssSystemSettingTempPtr("LP_EXISTS_MEMBER_CMD", NTCSS_SS_DEFAULT_STR)
#define NTCSS_LPNODATA_MSG         getNtcssSystemSettingTempPtr("NTCSS_LPNODATA_MSG", "LPNODATA_MSG")
#define NTCSS_LPNOFILE_MSG         getNtcssSystemSettingTempPtr("NTCSS_LPNOFILE_MSG", "LPNOFILE_MSG")
#define NTCSS_SPOOL_ETC            getNtcssSystemSettingTempPtr("NTCSS_SPOOL_ETC", "/h/NTCSSS/spool/etc")

/* Encryption */
/* NOTE: DO NOT use the getNtcssSystemSettingTempPtr macro for the define NTCSS_ENCRYPTION_CHECK_FILE
   because this define is used when loading the shared memory in NtcssSystemSettingsFcts.c.
   Trying to access the shared memory while loading it caused a core dump. */
#define NTCSS_ENCRYPTION_CHECK_FILE     "/h/NTCSSS/database/ntcss_users"
#define NTCSS_ENCRYPTION_TAG            "NTCSS_ENCRYPTION_TAG"
#define NTCSS_ENCRYPTION_ON_STRING      "NTCSS_ENCRYPTION_ON"
#define NTCSS_ENCRYPTION_OFF_STRING     "NTCSS_ENCRYPTION_OFF"
#define NTCSS_ENCRYPTION_UNKNOWN_STRING "NTCSS_ENCRPTION_UNKNOWN"
#define NTCSS_ENCRYPTION_MODE      getNtcssSystemSettingTempPtr(NTCSS_ENCRYPTION_TAG, NTCSS_ENCRYPTION_OFF_STRING)

typedef enum _NtcssEncryptionType {
  NTCSS_ENCRYPTION_ON          = 0,
  NTCSS_ENCRYPTION_OFF         = 1,
  NTCSS_ENCRYPTION_UNKNOWN     = 2
} NtcssEncryptionType;


/* Programs added to support sysmon files */ 
#ifdef VSUN
#define WC_PROG                "/usr/ucb/wc "
#elif linux
#define WC_PROG                "/usr/bin/wc "
#else
#define WC_PROG                "/bin/wc "
#endif

#define FILE_PROG              "/usr/bin/file"

#ifdef linux
#define TAIL_PROG              "/usr/bin/tail"
#else
#define TAIL_PROG              "/bin/tail"
#endif

/*
 * Maximum values. 
 */
#define MAX_NUM_PROGROUPS_PER_NTCSS_USER   256
#define MAX_NUM_APP_ROLES_PER_PROGROUP      8

#define MAX_NUM_NTCSS_USERS     16384
#define MAX_NUM_PROGROUPS       256
#define MAX_NUM_PROGRAMS        1024
#define MAX_NUM_APPUSERS        (MAX_NUM_NTCSS_USERS*MAX_NUM_PROGROUPS)
#define MAX_NUM_ACCESS_AUTH     (MAX_NUM_NTCSS_USERS*MAX_NUM_PROGRAMS)
#define MAX_NUM_NTCSS_ROLES     (MAX_NUM_APP_ROLES_PER_PROGROUP*MAX_NUM_PROGROUPS)
#define MAX_NUM_PRINTERS        500
#define MAX_NUM_ADAPTERS        500
#define MAX_NUM_PRT_CLASS       32
#define MAX_NUM_ADAPTER_TYPES   16
#define MAX_NUM_HOSTS           MAX_NUM_PROGROUPS /* one host per progroup */ 
#define MAX_NUM_OUTPUT_TYPE     512
#define MAX_NUM_PREDEFINED_JOB  2048
#define MAX_NUM_DEVICES         8
#define MAX_MASTER_BACKUP_STATUS_LEN   50

#define MAX_NUM_PROGRAMS_PER_PROGROUP       32
#define MAX_NUM_OUTPUT_TYPE_PER_PROGROUP    16   /*(MAX #OTYPE)/(MAX #PRGRPS)*/
#define MAX_NUM_PREDEFINED_JOB_PER_PROGROUP 64   /*(MAX #PDJ)/(MAX #PRGRPS) */

#define NUM_CLASSIFICATIONS     5
#define NUM_PRT_ACCESSES        2
#define NUM_PRT_SUSPEND_STATES  2
#define NUM_PRT_PORTS           4
#define NUM_NO_YES_VALUES       2
#define NUM_PROGRAM_TYPES       5
#define NUM_UNIX_STATUSES       4
#define NUM_PDJ_CLASSES         3


/*
 * Database field lengths.
 */
#define DATETIME_LEN            14
/*  98-08-07 GWY Added DATETIME_MILISEC_LEN for ATLASS reservation */
#define DATETIME_MILISEC_LEN    32

#define MIN_LOGIN_NAME_LEN      3
#define MAX_LOGIN_NAME_LEN      8
#define MIN_REAL_NAME_LEN       3
#define MAX_REAL_NAME_LEN       64
#define MIN_PASSWORD_LEN        6
#define MAX_PASSWORD_LEN        32
#define MIN_PHONE_NUMBER_LEN    3
#define MAX_PHONE_NUMBER_LEN    32
#define MAX_TOKEN_LEN           32

#define MIN_PROGRAM_TITLE_LEN       3
#define MAX_PROGRAM_TITLE_LEN       64
#define MIN_PROGRAM_FILE_LEN        3
#define MAX_PROGRAM_FILE_LEN        32
#define MAX_ICON_FILE_LEN           128
#define MAX_CMD_LINE_ARGS_LEN       128
#define MIN_WORKING_DIR_LEN         3
#define MAX_WORKING_DIR_LEN         128

#define MAX_APP_DATA_LEN            256
#define MAX_APP_PASSWD_LEN          32

#define MIN_PROGROUP_TITLE_LEN      3
#define MAX_PROGROUP_TITLE_LEN      16
#define MAX_VERSION_NUMBER_LEN      32
#define MAX_CLIENT_LOCATION_LEN     128
#define MAX_SERVER_LOCATION_LEN     128
#define MAX_UNIX_GROUP_LEN          16

#define MAX_ACCESS_ROLE_LEN         32  /* DB ERD chart mistakenly says 64 */

#define MIN_PRT_NAME_LEN            3
#define MAX_PRT_NAME_LEN            16
#define MIN_HOST_NAME_LEN           3
#define MAX_HOST_NAME_LEN           16
#define MIN_PRT_LOCATION_LEN        5
#define MAX_PRT_LOCATION_LEN        64
#define MIN_ADAPTER_LOC_LEN         5
#define MAX_ADAPTER_LOC_LEN         64
#define MIN_OUT_TYPE_TITLE_LEN      3
#define MAX_OUT_TYPE_TITLE_LEN      16
#define MIN_DESCRIPTION_LEN         3
#define MAX_DESCRIPTION_LEN         64
#define MIN_PRT_CLASS_TITLE_LEN     3
#define MAX_PRT_CLASS_TITLE_LEN     32
#define MIN_PRT_DEVICE_NAME_LEN     3
#define MAX_PRT_DEVICE_NAME_LEN     128
#define MIN_PRT_DRIVER_NAME_LEN     3
#define MAX_PRT_DRIVER_NAME_LEN     32

#define MAX_REQUEST_ID_LEN          36
#define MAX_PRT_FILE_LEN            128
#define MAX_SDI_CTL_RECORD_LEN      8
#define MAX_ORIG_HOST_LEN           32

#define MAX_PROC_PID_LEN            8
#define MAX_PROCESS_ID_LEN          32
#define MAX_PROC_CMD_LINE_LEN       256
#define MAX_CUST_PROC_STAT_LEN      128
#define MAX_JOB_DESCRIP_LEN         256
#define MAX_SCHED_STR_LEN           10
#define MAX_NUM_PROC_ARGS           16

#define MIN_APP_ROLE_LEN            3
#define MAX_APP_ROLE_LEN            32
#define MAX_IP_ADDRESS_LEN          32

#define MIN_BB_NAME_LEN             3
#define MAX_BB_NAME_LEN             16
#define MAX_MSG_ID_LEN              (DATETIME_LEN+MAX_LOGIN_NAME_LEN)
#define MAX_MSG_TITLE_LEN           32

#define MIN_COMDB_DATA_LEN          1
#define MAX_COMDB_DATA_LEN          128
#define MIN_COMDB_TAG_LEN           1
#define MAX_COMDB_TAG_LEN           64

#define MIN_PREDEFINED_JOB_LEN      3
#define MAX_PREDEFINED_JOB_LEN      32
#define MAX_DEVICE_LEN              128

#define MAX_PATH_LEN                256
#define MAX_PRT_OPTIONS_LEN         255

#define MAX_GROUP_LIST_LEN         ((MAX_PROGROUP_TITLE_LEN+1)*MAX_NUM_PROGROUPS_PER_NTCSS_USER)

#define MAX_ERR_MSG_LEN             (80 * 4)
#define MAX_DES_CRYPTED_PASSWD_LEN  16
/*
 * Special length values.
 */
/* 6 char strings x 5 + 3 integers x 10 = 60 */
#define MAX_PRT_REQUEST_LEN     (MAX_REQUEST_ID_LEN+MAX_PROGROUP_TITLE_LEN+MAX_PRT_FILE_LEN+MAX_LOGIN_NAME_LEN+MAX_PRT_NAME_LEN+DATETIME_LEN+MAX_ORIG_HOST_LEN+60)

/* 14 char strings x 5 + 6 integers x 10 = 130 */
#define MAX_SERVER_PROCESS_LEN  (MAX_PROCESS_ID_LEN+MAX_PROC_CMD_LINE_LEN+MAX_CUST_PROC_STAT_LEN+MAX_LOGIN_NAME_LEN+MAX_LOGIN_NAME_LEN+MAX_PROGROUP_TITLE_LEN+DATETIME_LEN+DATETIME_LEN+DATETIME_LEN+MAX_ORIG_HOST_LEN+MAX_PRT_NAME_LEN+MAX_HOST_NAME_LEN+MAX_PRT_FILE_LEN+MAX_JOB_DESCRIP_LEN+130)

/* 4 char strings x 5 + 4 integers x 10 = 60 */
#define MAX_USER_INFO_LEN       (MAX_TOKEN_LEN+MAX_LOGIN_NAME_LEN+MAX_REAL_NAME_LEN+11+MAX_PHONE_NUMBER_LEN+DATETIME_LEN+60)

/* 8 char strings x 5 + 3 integers x 10 = 70 */
#define MAX_APP_INFO_LEN        (MAX_PROGROUP_TITLE_LEN+MAX_HOST_NAME_LEN+MAX_APP_DATA_LEN+MAX_APP_PASSWD_LEN+MAX_ICON_FILE_LEN+MAX_VERSION_NUMBER_LEN+DATETIME_LEN+MAX_CLIENT_LOCATION_LEN+MAX_SERVER_LOCATION_LEN+70)

/* 5 char strings x 5 + 3 integers x 10 = 55 */
#define MAX_PROGRAM_INFO_LEN    (MAX_PROGRAM_TITLE_LEN+MAX_PROGRAM_FILE_LEN+MAX_ICON_FILE_LEN+MAX_CMD_LINE_ARGS_LEN+MAX_WORKING_DIR_LEN+55)

/* 3 char strings x 5 + 2 integers x 10 = 35 */
#define MAX_PROGRAM_ACCESS_LEN  ((MAX_NUM_APP_ROLES_PER_PROGROUP + 2)*MAX_ACCESS_ROLE_LEN)
#define MAX_APP_USER_INFO_LEN   (MAX_REAL_NAME_LEN+MAX_PHONE_NUMBER_LEN+MAX_PROGRAM_ACCESS_LEN+35)

/* String lengths defined for servers.h and NetSvrInfo.c */

/*#define MAX_FILENAME_LEN    512 <- changed because of install.h */
#define MAX_FILENAME_LEN   	1024
/*#define MAX_CMD_LEN         1024  <- changed because of install.h */
#define MAX_CMD_LEN         2048
#define MAX_MSG_LEN         8192
#define CMD_LEN             20
#define CMD_MESSAGE_LEN     8192


/*
 * Error values.
 */
#define ERROR_CANT_PARSE_MSG                            -1
#define ERROR_CANT_ACCESS_DB                            -2
#define ERROR_CANT_OPEN_DB_FILE                         -3
#define ERROR_NO_SUCH_USER                              -4
#define ERROR_INCORRECT_PASSWORD                        -5
#define ERROR_NO_GROUP_INFO_FOUND                       -6
#define ERROR_NO_PROGRAM_INFO_FOUND                     -7
#define ERROR_NO_SUCH_APP_USER                          -8
#define ERROR_NO_SUCH_GROUP                             -9
#define ERROR_NO_PRT_INFO_FOUND                         -10
#define ERROR_NO_PRT_CLS_INFO_FOUND                     -11
#define ERROR_NO_OTYPE_INFO_FOUND                       -12
#define ERROR_NO_SUCH_OTYPE                             -13
#define ERROR_CANT_INSERT_VALUE                         -14
#define ERROR_INCORRECT_OLD_PASSWD                      -15
#define ERROR_INCORRECT_NEW_PASSWD                      -16
#define ERROR_CANT_PARSE_ARGS                           -17
#define ERROR_NO_SUCH_PRT                               -18
#define ERROR_CANT_DETERMINE_HOST                       -19
#define ERROR_NO_NEED_TO_SUSPEND                        -20
#define ERROR_QUEUE_DISABLED                            -21
#define ERROR_CANT_READ_VALUE                           -22
#define ERROR_CANT_CREATE_QUERY                         -23
#define ERROR_USER_ALREADY_IN                           -24
#define ERROR_CANT_UNLOCK_LOGIN                         -25
#define ERROR_CANT_ACCESS_LOGIN                         -26
#define ERROR_CANT_GET_PID_DATA                         -27
#define ERROR_CANT_DELETE_VALUE                         -28
#define ERROR_USER_NOT_VALID                            -29
#define ERROR_CANT_REL_REQUEST                          -30
#define ERROR_CANT_KILL_SVR_PROC                        -31
#define ERROR_CANT_REPLACE_VALUE                        -32
#define ERROR_INVALID_NUM_ARGS                          -33
#define ERROR_CANT_TALK_TO_SVR                          -34
#define ERROR_NO_SUCH_FILE                              -35
#define ERROR_CANT_REMOVE_ACT_PROCESS                   -36
#define ERROR_CANT_ACCESS_DIR                           -37
#define ERROR_NTCSS_LP_FAILED                           -38
#define ERROR_BB_MSG_DOES_NOT_EXIST                     -39
#define ERROR_BB_DOES_NOT_EXIST                         -40
#define ERROR_USER_LOGIN_DISABLED                       -41
#define ERROR_CANT_OPEN_FILE                            -42
#define ERROR_NOT_UNIX_USER                             -43
#define ERROR_CANT_DETER_MASTER                         -44
#define ERROR_CANT_COMM_WITH_MASTER                     -45
#define ERROR_CANT_TRANSFER_FILE                        -46
#define ERROR_CANT_CREATE_MASTER_BACKUP                 -47
#define ERROR_NO_SUCH_ITEM_REGISTERED                   -48
#define ERROR_SERVER_IDLED                              -49
#define ERROR_SYSTEM_LOCKED                             -50
#define ERROR_INVALID_COMDB_TAG_VALUE                   -51
#define ERROR_INVALID_COMDB_ITEM_VALUE                  -52
#define ERROR_CANT_FIND_VALUE                           -53
#define ERROR_CANT_KILL_NON_ACTIVE_PROC                 -54
#define ERROR_NO_SUCH_HOST                              -55
#define ERROR_NO_SUCH_PROCESS                           -56
#define ERROR_CANT_SET_MASTER                           -57
#define ERROR_CANT_UNPACK_MASTER_BACKUP                 -58
#define ERROR_DB_SOFTWARE_VERSION_MISMATCH              -59
#define ERROR_DAEMON_IS_AWAKE                           -60
#define ERROR_CANT_CHANGE_NTCSS_APP_FILE                -61
#define ERROR_HOST_IS_NOT_MASTER                        -62
#define ERROR_CANT_RENAME_FILE                          -63
#define ERROR_INVALID_BB_NAME                           -64
#define ERROR_BB_ALREADY_EXISTS                         -65
#define ERROR_CHANGE_PASSWD_FAILED                      -66
#define ERROR_CANT_SET_ADMIN_LOCK                       -67
#define ERROR_CANT_SIGNAL_PROCESS                       -68
#define ERROR_CANT_CREATE_INI_FILE                      -69
#define ERROR_CANT_EXTRACT_INI_FILE                     -70
#define ERROR_CANT_GET_FILE_INFO                        -71
#define ERROR_PROCESS_NOT_IN_VALID_STATE_FOR_REMOVAL    -72
#define ERROR_RUN_NEXT_TAKEN                            -73
#define ERROR_REMOVE_FROM_WRONG_PROGROUP                -74
#define ERROR_KILL_FROM_WRONG_PROGROUP                  -75
#define ERROR_SDI_NO_TARGET_FILE                        -76
#define ERROR_SDI_NO_TCI_FILE                           -77
#define ERROR_SDI_NO_CNTRL_REC                          -78
#define ERROR_CANT_LOCK_APPLICATION                     -79
#define ERROR_CANT_UNLOCK_APPLICATION                   -80
#define ERROR_USER_LOGGED_IN                            -81
#define ERROR_APP_TO_DM_PIPE_OPEN                       -82         
#define ERROR_APP_TO_DM_PIPE_WRITE                      -83
#define ERROR_DM_SEMAPHORE                              -84
#define ERROR_DM_TO_APP_PIPE_OPEN                       -85
#define ERROR_DM_TO_APP_PIPE_READ                       -86
#define ERROR_LOCAL_PROGRAM_INFO                        -87
#define ERROR_NOT_USER_AUTH_SERVER_OR_MASTER            -88
#define ERROR_NTCSS_BUILD_AND_PUSH_NIS_ERROR            -89
#define ERROR_PASSWORD_TRANSLATION_FAILED               -90
#define ERROR_SYSTEM_CMD_FAILED                         -91

/*
 * SQL operation values.
 */
#define SQL_MOD_NTCSS_USERS         0
#define SQL_DEL_NTCSS_USERS         1
#define SQL_MOD_APPUSERS_NOAPPDATA  2
#define SQL_DEL_APPUSERS            3
#define SQL_MOD_PROGROUPS           4
#define SQL_MOD_PROGRAMS            5
#define SQL_MOD_PRINTERS            6
#define SQL_CHANGE_PRINTERS         7
#define SQL_LIMMOD_PRINTERS         8
#define SQL_DEL_PRINTERS            9
#define SQL_ADD_PRINTER_ACCESS      10
#define SQL_DEL_PRINTER_ACCESS      11
#define SQL_MOD_ADAPTERS            12
#define SQL_CHANGE_ADAPTERS         13
#define SQL_DEL_ADAPTERS            14
#define SQL_MOD_OUTPUT_TYPE         15
#define SQL_DEL_OUTPUT_TYPE         16
#define SQL_ADD_OUTPUT_PRT          17
#define SQL_DEL_OUTPUT_PRT          18
#define SQL_MOD_HOSTS               19
#define SQL_MOD_PRT_CLASS           20
#define SQL_REDO_DB                 21
#define SQL_REDO_PRINTERS           22
#define SQL_REDO_BB                 23
#define SQL_MOD_PREDEFINED_JOB      24
#define SQL_DEL_PREDEFINED_JOB      25
#define SQL_MOD_BATCH_NTCSS_USERS   26
#define SQL_MOD_BATCH_APPUSERS      27

/*
 * Database initialization values.
 */
#define DB_CHANGE_REGULAR   0
#define DB_CHANGE_INIT      1
#define DB_CHANGE_REDO      2
#define DB_CHANGE_BB_REDO   3
#define DB_REMOVE_OLD       4

/*
 * Lock key and wait time values.
 */
#define LOCK_WAIT_TIME      55
#define MAIN_DB_RW_KEY      12450
#define USER_ADMIN_KEY      12451
#define PRT_CONFIG_KEY      12452
#define LOGIN_INFO_RW_KEY   12453
#define SYS_CONF_RW_KEY     12454
#define SYS_MON_RW_KEY      12455
#define NUM_MSG_BB_LOCKS    19
#define MSG_BUL_BRD_KEY     12100   /* NUM_MSG_BB_LOCKS locks */

/*
 * Login history states.
 */
#define GOOD_LOGIN_STATE        0
#define FAILED_LOGIN_STATE      1
#define LOGOUT_STATE            2
#define RECORD_LOGOUT_STATE     3
#define LOGIN_RESERVED_STATE	4
#define REMOVE_RESERVED_STATE  5

/*
 * Server port numbers.
 */
#define DEF_UDP_CLT_PORT        9121
#define DEF_PRTQ_SVR_PORT       9122
#define DEF_DB_SVR_PORT         9132
#define DEF_DSK_SVR_PORT        9142
#define DEF_CMD_SVR_PORT        9152
#define DEF_SPQ_SVR_PORT        9162
#define DEF_MSG_SVR_PORT        9172
#define DEF_PRINT_PROXY         5581
#define DEF_BC_SVR_PORT         5582
#define DEF_PLIST_SVR_PORT      5583

/*  NOTE:first arg in function call must match servers_data.c name entry */
#define PRINT_PROXY	getNtcssPort("printer_org",DEF_PRINT_PROXY)   
#define BC_SVR_PORT     getNtcssPort("bcserver",DEF_BC_SVR_PORT)
#define PLIST_SVR_PORT  getNtcssPort("plist",DEF_PLIST_SVR_PORT)
#define UDP_CLT_PORT	getNtcssPort("clt_port",DEF_UDP_CLT_PORT)
#define PRTQ_SVR_PORT	getNtcssPort("prtque_server",DEF_PRTQ_SVR_PORT)
#define DB_SVR_PORT	getNtcssPort("database_server",DEF_DB_SVR_PORT)
#define DSK_SVR_PORT	getNtcssPort("desktop_server",DEF_DSK_SVR_PORT)
#define CMD_SVR_PORT	getNtcssPort("command_server",DEF_CMD_SVR_PORT)
#define SPQ_SVR_PORT	getNtcssPort("spque_server",DEF_SPQ_SVR_PORT)
#define MSG_SVR_PORT	getNtcssPort("message_server",DEF_MSG_SVR_PORT)


/*
 * Access roles
 */
#define NTCSS_USER_ROLE        0
#define APP_ADMIN_ROLE         1
#define LRS_NTCSS_ADMIN_ROLE   1
#define NTCSS_ADMIN_ROLE       2
#define NTCSS_BATCH_USER_BIT   1024
#define ROLE_BIT_BASE_OFFSET   1

/*
 * Server process states.
 */
#define WAITING_FOR_OK_STATE        -1  /* active procs, scheduled procs */
#define DEVICE_DEPENDENT_STATE      -2  /* active procs, scheduled procs */
#define CRASHED_STATE               -3  /* active procs */
#define EXITED_STATE                -4  /* active procs */
#define WAITING_FOR_EXEC_STATE      -5  /* active procs */
#define ABOUT_TO_BE_RUN_STATE       -6  /* active procs */
#define APPROVED_STATE              -7  /* scheduled procs */
#define RUN_NEXT_STATE              -8  /* active procs */
#define SDI_FAILURE_STATE           -9  /* active procs */

/*
 * Server process priorities.
 */
#define BOOT_PRIORITY                -2
#define SCHEDULED_PRIORITY           -1
#define LOW_PRIORITY                  1
#define MEDIUM_PRIORITY               2
#define HIGH_PRIORITY                 3

/*
 * Server process change bit mask values.
 */
#define CHANGE_CUST_STAT                1
#define CHANGE_PROC_STAT                2
#define CHANGE_PID_VALUE                4
#define CHANGE_END_TIME                 8
#define CHANGE_LAUNCH_TIME             16
#define CHANGE_PRIORITY                32
#define CHANGE_DEVICE                  64
#define CHANGE_PRT_FLAG               128
/*
 * Server process queue bit mask values.
 */
#define SPQ_BIT_MASK_WITH_CUST_PROC_STATUS   1
#define SPQ_BIT_MASK_WITH_PRT_REQ_INFO       2
#define SPQ_BIT_MASK_WITH_JOB_DESCRIP        4
#define SPQ_BIT_MASK_ONLY_CERTAIN_USER       8
#define SPQ_BIT_MASK_ONLY_RUNNING_PROCS     16
#define SPQ_BIT_MASK_ONLY_USER_ACTIVE_PROCS 32 /* special case, overrides all*/
#define SPQ_BIT_MASK_PRIORITY                   64
#define SPQ_BIT_MASK_COMMAND_LINE              128
#define SPQ_BIT_MASK_CUST_PROC_STATUS          256
#define SPQ_BIT_MASK_LOGIN_NAME                512
#define SPQ_BIT_MASK_UNIX_OWNER               1024
#define SPQ_BIT_MASK_PROGROUP_TITLE           2048
#define SPQ_BIT_MASK_JOB_DESCRIP              4096
#define SPQ_BIT_MASK_ORIG_HOST                8192

/*
 * NTCSS group specification values.
 */
#define WITHOUT_NTCSS_GROUP             0
#define WITH_NTCSS_GROUP                1
#define WITHOUT_NTCSS_AND_WITH_ROLES    2
#define WITH_NTCSS_AND_WITH_ROLES       3
#define WITH_NTCSS_AND_WITH_OLD_ROLES   4

/*
 * Security Roles for all Ntcss Users
 */
#define UNCLASSIFIED_CLEARANCE             0
#define UNCLASSIFIED_SENSITIVE_CLEARANCE   1
#define CONFIDENTIAL_CLEARANCE             2
#define SECRET_CLEARANCE                   3
#define TOPSECRET_CLEARANCE                4

/*
 * Server daemon actions and states.
 */
#define SERVER_STATE_IDLE       0   /* action, state */
#define SERVER_STATE_AWAKE      1   /* action, state */
#define SERVER_STATE_CHECK      2   /* action */
#define SERVER_STATE_DEAD       3   /* state */
#define SERVER_STATE_ONDEBUG    4   /* action, state */
#define SERVER_STATE_OFFDEBUG   5   /* action, state */
#define SERVER_STATE_DIE        7   /* action */
#define SERVER_STATE_ONLOCK     8   /* action, state */
#define SERVER_STATE_OFFLOCK    9   /* action, state */

/*  Max Wait time for a process in seconds*/

#define SERVER_PROCESS_MAX_WAIT      1800   /* 30 minutes */

/*
 * Configuration file bit mask identifiers.
 */
#define CONFIG_FILE_MASK_LENGTH         3
#define CONFIG_FILE_PROGROUPS           1
#define CONFIG_FILE_HOSTS               2
#define CONFIG_FILE_DEVICES             4

/*
 * Message bulletin board types.
 */
#define MSG_BB_TYPE_USER        0
#define MSG_BB_TYPE_GENERAL     1
#define MSG_BB_TYPE_SYSTEM      2

#define MAX_SYSTEM_MSG_LEN      512
#define NUM_SYSTEM_MSG_BBS      20

/*
 * System message bulletin board values.
 */
#define SYS_MSG_BB_PROCESS_TERMINATED     1
#define SYS_MSG_BB_PRT_JOB_DELETED        2
#define SYS_MSG_BB_PRT_JOB_RELEASED       3

/* misc. SYSMON defines */
#define NTCSS_SYSMON_CREATE_USERS_INI           1
#define NTCSS_SYSMON_CREATE_PROGROUPS_INI       2
#define NTCSS_SYSMON_CREATE_HOSTS_INI           3
#define NTCSS_SYSMON_CREATE_PRINTERS_INI        4
#define NTCSS_SYSMON_CREATE_LOGIN_HISTORY_INI   5
#define NTCSS_SYSMON_CREATE_SYSTEM_LOG_INI      6

#define NTCSS_SYSMON_PROGROUPS_INI_TAG         "SYSMON_PROGROUPS"
#define NTCSS_SYSMON_USERS_INI_TAG             "SYSMON_USERS"
#define NTCSS_SYSMON_HOSTS_INI_TAG             "SYSMON_HOSTS"
#define NTCSS_SYSMON_PRINTERS_INI_TAG          "SYSMON_PRINTERS"
#define NTCSS_SYSMON_LOGIN_HISTORY_INI_TAG     "SYSMON_LOGIN_HISTORY"
#define NTCSS_SYSMON_SYSTEM_LOG_INI_TAG        "SYSMON_SYSTEM_LOG"

/* db server lock values */
#define PRINT_ADMIN_LOCK_CLIENT     0
#define PDJ_ADMIN_LOCK_CLIENT       1
#define OTYPE_ADMIN_LOCK_CLIENT     2
#define APP_OTYPE_ADMIN_LOCK_CLIENT 3
#define SYSMON_LOCK_CLIENT          4
#define USER_ADMIN_LOCK_CLIENT      5

#define CREATE_ADMIN_LOCK           0
#define RENEW_ADMIN_LOCK            1
#define REMOVE_ADMIN_LOCK           2

/*
 * Miscellaneous values.
 */

#define MAX_NUM_COLS            16
#define MAX_COL_LEN             256

#define EMPTY_STR               "EMPTY EMPTY EMPTY"
#define NTCSS_APP               "NTCSS"
#define NTCSS_UNIX_GROUP        "ntcss"
#define END_PGRP_MARKER         "*+*+*+*+*+*+*+*"
#define START_PDJ_MARKER        "*-*-*-*-*-*-*-*"
#define SERVER_TOKEN_STRING     "XXXXXX"
#define BACKDOOR_TOKEN_STRING   "YYYYYY"
#define DB_TO_MSG_BB_DIR        "../message"
#define BASE_NTCSS_USER         "ntcssii"
#define BAD_NTCSS_MASTER_BACKUP  "BAD_BACKUPFILE_STATUS"
#define GOOD_NTCSS_MASTER_BACKUP "GOOD_BACKUPFILE_STATUS"

#define NUM_PROCESS_SEARCH_ATTEMPTS     20
#define NUM_SECS_ADMIN_LOCK_TIMER       180
#define MAX_NUM_KILL_CHILD              10
#define MASTER_FILES_BACKUP_INTERVAL    5

/* encrypt/decryption key used for NtcssCrypt.h functions */
#define ENCRYPT_DECRYPT_KEY "\x1b\x12\x08\x0b\x10\x07\x08\x15\x05\x08\x04\x09\x0f\x1f\x08"


/** ATLASS Enhancements **/
#define APP_TO_DM_PIPE        getNtcssSystemSettingTempPtr("APP_TO_DM_PIPE",    NTCSS_SS_DEFAULT_STR)
#define DM_TO_APP_PIPE        getNtcssSystemSettingTempPtr("DM_TO_APP_PIPE",    NTCSS_SS_DEFAULT_STR)
#define NTCSS_LP_PIPE_DIR     getNtcssSystemSettingTempPtr("NTCSS_LP_PIPE_DIR", NTCSS_SS_DEFAULT_STR)
#define NTCSS_LP_PIPE_DAEMON   "nt_lppd"
#define AUTHUSER_SEP_CHAR      '\3'
#define AUTHUSER_SEP_CHAR_STRING         "\3"
#define DM_RESOURCE_MONITOR_PID_FILE "DmResourceMonitor.pid"
#define DM_PID_FILE                  "DM.pid"
#define DM_HOST_SEP_CHARS_STRING         ", "
#define DM_TO_APP_PIPE_SEM_ID            424
#define NTCSS_MASTER_SERVER_TYPE         0
#define NTCSS_APP_SERVER_TYPE            1
#define NTCSS_AUTH_SERVER_TYPE           2
#define MAX_DM_AUTH_SERVER_REPLY_LEN     32768
#define MAX_DM_PIPE_WAIT_TIME		 55

#define ALLHOSTS "ALLHOSTS"

/* authentication defines */ 
#define MT_AUTHUSER            "AUTHUSER"
#define MT_GOODAUTHUSER        "GOODAUTHUSER"
#define MT_BADAUTHUSER         "BADAUTHUSER"
#define MT_NOUSRINFOAUTHUSER   "NOUSRINROAUTHUSER"

/* Current App Data defines */
#define MT_CURRENTAPPDATA      "CURRENTAPPDATA"
#define MT_GOODCURRENTAPPDATA  "GOODCURRENTAPPDATA"
#define MT_BADCURRENTAPPDATA   "BADCURRENTAPPDATA"

/* Distributed User admin */
#define MT_USERADMIN           "DPPROCESSUSRCHANGES"
#define MT_GOODUSERADMIN       "GOODDPPROCUSRADMIN"
#define MT_BADUSERADMIN        "BADDPPROCUSRADMIN"     
#define MT_NOREPLYUSERADMIN    "NORPLYDPPROCUSRADMN"

/* Distributed print admin */
#define MT_PRINTADMIN          "DPPROCESSPRTCHANGES"
#define MT_GOODPRINTADMIN      "GOODDPPROCPRTADMIN"
#define MT_BADPRINTADMIN       "BADDPPROCPRTADMIN"     
#define MT_NOREPLYPRINTADMIN   "NORPLYDPPROCPRTADMN"

/* Distributed copy master files */
#define MT_GETMASTERFILES      "COPYMASTERFILES"
#define MT_GOODGETMASTERFILES  "GOODCOPYMASTERFILES"
#define MT_BADGETMASTERFILES   "BADCOPYMASTERFILES"

/* user admin defined */
#define MT_MODUSER             "MODUSER"
#define MT_GOODMODUSER         "GOODMODUSER"
#define MT_BADMODUSER          "BADMODUSER"

/* prt admin defined */
#define MT_MODPRT              "MODPRT"
#define MT_GOODMODPRT          "GOODMODPRT"
#define MT_BADMODPRT           "BADMODPRT"
#define COPY_FILE_PREFIX	"COPY_FILE"
#define MAX_COPY_FILES		99
#define NO_MORE_COPY_FILES	"NOMOREFILES"

/* NIS message defines */
#define MT_NISMAPUPDATE		"GETNISUPDATE"
#define MT_DONENISUPDATE	"DONENISMAPGET"
#define MT_GOODNISUPDATE	"GOODNISUPDATE"
#define MT_BADNISUPDATE		"BADNISUPDATE"

/** END ATLASS Enhancements **/

/** TYPEDEF DEFINITIONS **/

typedef struct  _UserItem {
    int  marker;
#ifdef linux
    uid_t  unix_id;
#else
    int  unix_id;
#endif
    char  *login_name;
    char  *real_name;
    char  *password;
    char  *shared_passwd;
    char  *ss_number;
    char  *phone_number;
    int  security_class;
    char  *pw_change_time;
    int  user_role;
    int  user_lock;
    char *auth_server;
} UserItem;

/* GWY 98-08-08 Added CurrentUserItem for ATLAS */
typedef struct _CurrentUsersItem {
  int  marker;
  char *login_name;
  char *login_time;
  char *client_address;
  char *token;
  char *reserve_time;
  int  uid;
} CurrentUsersItem;



typedef struct _userLoadStruct
{
  char hostname[MAX_HOST_NAME_LEN + 1];
  int  num_users;
} userLoadStruct;


typedef struct  _AppItem {
    int  marker;
    char  *progroup_title;
    char  *hostname;
    char  *icon_file;
    int  icon_index;
    char  *version_number;
    char  *release_date;
    char  *client_location;
    char  *server_location;
    char  *unix_group;
    int  num_processes;
    char  *link_data;
    int  num_roles;
    char  *roles[MAX_NUM_APP_ROLES_PER_PROGROUP];
} AppItem;

typedef struct  _ProgramItem {
    int  marker;
    char  *program_title;
    char  *program_file;
    char  *icon_file;
    int  icon_index;
    char  *cmd_line_args;
    char  *progroup_title;
    int  security_class;
    int  program_access;
    int  flag;
    char  *working_dir;
} ProgramItem;

typedef struct  _AppuserItem {
    int  marker;
    char  *progroup_title;
    char  *login_name;
    char  *real_name;
    char  *app_data;
    char  *app_passwd;
    int  app_role;
    int  registered_user;
} AppuserItem;

/* GWY 98-07-17 BEGIN Added for ATLAS (DM/DP) */
typedef struct _AppuserProgroupRoleItem {
    char *progroup_title;
    char *shared_passwd;
    char *app_data;
    int  app_role;
    char *link_data;
} AppuserProgroupRoleItem;
/* GWY 98-07-17 END Added for ATLAS (DM/DP) */

/* Convenience functions for use with ProgroupUseStruct
   can be found in ManipData.c
   Both these functions expect memory to already be allocated for
   the array of ProgroupUseStruct that is passed in.
   initProgroupUseStruct(pProgroupUseStruct, nItems)
   addProgroupUseInfo(aProgroupUseStruct, strProgroup, nProgroupUse, nMaxArrayElements)
*/
typedef struct ProgroupUseStruct_ {
    char strProgroup[MAX_PROGROUP_TITLE_LEN];
    int  nUsersCurrentlyInProgroup;
} ProgroupUseStruct;


typedef struct  _ProguserItem {
    int  marker;
    char  *login_name;
    char  *real_name;
    char  *program_title;
    char  *progroup_title;
} ProguserItem;

typedef struct  _PrtItem {
    int  marker;
    char  *prt_name;
    char  *hostname;
    char  *prt_location;
    int  max_size;
    char  *prt_class_title;  /* link to prt_class table */
    char  *ip_address;       /* link to adapters table */
    int  security_class;
    int  status;
    char  *port_name;
    int  suspend_flag;
    char  *redirect_prt;
    char  *FileName;         /*Atlas*/ 
    int  prt_access[MAX_NUM_PROGROUPS];  /* equal 1 if app has access */
} PrtItem;

typedef struct  _AdapterItem {
    int  marker;
    char  *adapter_type;
    char  *machine_address;
    char  *ip_address;
    char  *location;
} AdapterItem;

typedef struct  _PrtclassItem {
    int  marker;
    char  *prt_class_title;
    char  *prt_device_name;
    char  *prt_driver_name;
} PrtclassItem;

typedef struct  _AdaptypeItem {
    char  *type;
    int  num_ports;
    char  *port[NUM_PRT_PORTS];
    char  *setup_script;
} AdaptypeItem;

typedef struct  _OutputTypeItem {
    int  marker;
    char  *out_type_title;
    char  *progroup_title;
    char  *description;
    char  *default_prt;
    int  batch_flag;
    int  output_prt[MAX_NUM_PRINTERS];   /* equal 1 if printer connected */
} OutputTypeItem;                        /* to output type */

typedef struct  _AccessRoleItem {
    int  marker;
    char  *app_role;
    char  *progroup_title;
    int   role_number;
	int   locked_role_override;
} AccessRoleItem;

typedef struct  _HostItem {
    int  marker;
    char  *hostname;
    char  *ip_address;
    int   num_processes;
    /*98-08-06: GWY Added type to HostItem for ATLAS:  
      0 - Master, 1 - App Server, 2 - Authentication Server */
    int  type;
   /*98-09-02: ACA added replication indicator for ATLASS
      0 = NO Replication, 1 = Replicatino */
    int repl;
} HostItem;

typedef struct  _PrtRequestItem {
    char  *request_id;
    char  *progroup_title;
    char  *prt_file;
    char  *login_name;
    char  *prt_name;
    char  *time_of_request;
    int  security_class;
    int  size;
    char  *orig_host;
    int  status;
} PrtRequestItem;

typedef struct  _PredefinedJobItem {
    int  marker;
    char  *job_title;
    char  *progroup_title;
    char  *command_line;
    int  role_access;
    char  *schedule_str;
    int  flag;
} PredefinedJobItem;

typedef struct  _ServerProcessItem {
    char  process_id[MAX_PROCESS_ID_LEN];
    int  pid;
    int  priority;
    char  command_line[MAX_PROC_CMD_LINE_LEN+1];
    int  proc_status;
    char  cust_proc_status[MAX_CUST_PROC_STAT_LEN+1];
    char  login_name[MAX_LOGIN_NAME_LEN+1];
    char  unix_owner[MAX_LOGIN_NAME_LEN+1];
    char  progroup_title[MAX_PROGROUP_TITLE_LEN+1];
    char  job_descrip[MAX_JOB_DESCRIP_LEN+1];
    char  request_time[DATETIME_LEN];
    char  launch_time[DATETIME_LEN];
    char  end_time[DATETIME_LEN];
    int  security_class;
    char  orig_host[MAX_ORIG_HOST_LEN+1];
    char  prt_name[MAX_PRT_NAME_LEN+1];
    char  hostname[MAX_HOST_NAME_LEN+1];
    int  prt_sec_class;
    char  prt_filename[MAX_PRT_FILE_LEN+1];
    int  prt_flag;                                            /* bitmasked */
    char  sdi_ctl_rec[MAX_SDI_CTL_RECORD_LEN+1];
    char  sdi_data_file[MAX_PRT_FILE_LEN+1];
    char  sdi_tci_file[MAX_PRT_FILE_LEN+1];
    char  sdi_device[MAX_PRT_FILE_LEN+1];
    int  copies;
    int  banner;
    int  orientation;
    int  papersize;
} ServerProcessItem;

typedef struct  _DeviceDataItem {
    char  name[MAX_DEVICE_LEN+1];
    char  dir[MAX_DEVICE_LEN+1];
    char  file[MAX_PRT_FILE_LEN+1];
    int  is_drive;
} DeviceDataItem;

typedef struct  _DBField {
    char  data[MAX_COL_LEN];
} DBField;

typedef struct  _DBRecord {
    DBField  field[MAX_NUM_COLS];
    int  sql_change;
    int  num_items;
} DBRecord;

/* ATLAS Enhancements 98-08-06 GWY  Added NtcssHostType */
typedef enum _NtcssHostType {
  NTCSS_MASTER      = 0,
  NTCSS_APP_SERVER  = 1,
  NTCSS_AUTH_SERVER = 2
} NtcssHostType;

typedef enum  security 
{
    UNCLASSIFIED = UNCLASSIFIED_CLEARANCE,
    UNCLASSIFIED_SENSITIVE = UNCLASSIFIED_SENSITIVE_CLEARANCE,
    CONFIDENTIAL = CONFIDENTIAL_CLEARANCE,
    SECRET = SECRET_CLEARANCE,
    TOPSECRET = TOPSECRET_CLEARANCE 
} SecurityLevel;


typedef struct  _ProcessFilterData {
  int  priority;
  char  command_line[MAX_PROC_CMD_LINE_LEN+1];
  char  cust_proc_status[MAX_CUST_PROC_STAT_LEN+1];
  char  login_name[MAX_LOGIN_NAME_LEN+1];
  char  unix_owner[MAX_LOGIN_NAME_LEN+1];
  char  progroup_title[MAX_PROGROUP_TITLE_LEN+1];
  char  job_descrip[MAX_JOB_DESCRIP_LEN+1];
  char  orig_host[MAX_ORIG_HOST_LEN+1];
} ProcessFilterData;

#endif /* _Ntcss_h */
