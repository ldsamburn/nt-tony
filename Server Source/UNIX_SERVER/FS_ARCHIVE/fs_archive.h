
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * fs_archive.h
 */

#ifndef _fs_archive_h
#define _fs_archive_h

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>


/** INRI INCLUDES **/

#include <Ntcss.h>
#include <ButtonBox.h>
#include <ExtList.h>

/** DEFINE DEFINITIONS **/

#define DEBUG                   1

#define MAX_ITEM_LEN            512
#define MAX_ARRAY_SIZE          32

#define MAX_FILELINE_LEN        512
#define MAX_FILELISTING_LEN     (MAX_FILENAME_LEN + 100)  /* used to be 1124 */
#define FILELISTING_FILE_POS    57
#define MAX_DEVNAME_LEN         128
#define MAX_NUM_JOBS_PER_DEV    24
#define NUM_DAYS                7
#define MAX_USER_LEN            8
#define MAX_GROUP_LEN           8
#define DEVNAME_LEN             30
#define MAX_JOBNAME_LEN         64

#define OFFSET                  3
#define OFFSET_MED              5
#define OFFSET_LARGE            10
#define OFFSET_RT_EXT           6
#define OFFSET_BT_EXT           5
#define BASESIZE                100
#define TEXT_LABEL_END          35
#define TEXT_FIELD_START        38

#define BACKUP_WIDTH            680
#define BACKUP_HEIGHT           420
#define RESTORE_WIDTH           660
#define RESTORE_HEIGHT          360
#define COPYJOB_WIDTH           340
#define COPYJOB_HEIGHT          160
#define FILESEL_WIDTH           400
#define FILESEL_HEIGHT          400

#define NUM_JOB_TITLES          4

#define NUM_BACKUP_LABELS       6
#define NUM_RESTORE_LABELS      5
#define NUM_COPYJOB_LABELS      2

#define HIGH_HOUR               23
#define HIGH_MINUTE             59

#define HOUR_ARROW_UP           0
#define HOUR_ARROW_DOWN         1
#define MINUTE_ARROW_UP         2
#define MINUTE_ARROW_DOWN       3

#define NUM_MSG_TYPES           3

#define SHELL_START             0
#define SHELL_UP                1

#ifdef VSUN
#define MT_PROG                 "/bin/mt -f"
#define MAIL_PROG               "/usr/ucb/mail"
#define DU_PROG                 "/usr/bin/du -s"
#define CRON_LIST_PROG          "/bin/crontab -l"
#define CRON_ADD_PROG           "/bin/crontab"
#define REWIND_CMD              "rewind"
#define EJECT_CMD               "offline"
#endif

#define APPLICATION_TITLE       "NTCSS File System Backup/Restore"
#define DEVICE_LIST_TITLE       "DEVICE LIST                      "
#define RESTORE_FILELIST_TITLE  "RESTORE FILE LIST                "
#define BACKUP_FILELIST_TITLE   "BACKUP FILE LIST                 "
#define JOBLIST_TITLE           "BACKUP JOB LIST                  "
#define COPYJOB_TITLE           "Copy Backup Job"
#define CHANGEDEV_TITLE         "Select New Device"

#define LOCK_DIR                "../logs"

#define XtNdataDir              "dataDir"
#define XtCDataDir              "DataDir"
#define XtNprogDir              "progDir"
#define XtCProgDir              "ProgDir"
#define XtNhelpFile             "helpFile"
#define XtCHelpFile             "HelpFile"
#define XtNdevFile              "devFile"
#define XtCDevFile              "DevFile"
#define XtNshowText             "showText"
#define XtCShowText             "ShowText"
#define XtNdbDir                "dbDir"
#define XtCDbDir                "DbDir"

#define EJECT_TAPE_OK           "Media in device was ejected."
#define REWIND_TAPE_OK          "Media in device was rewound."
#define DELETE_JOB_QUEST        "Are you sure you want to delete this job?"
#define PERFORM_BACKUP_QUEST    "Perform an immediate backup\nof files in backup job?"
#define RESTORE_FILES_QUEST     "Restore selected files?"


/** TYPEDEF DEFINITIONS **/

typedef struct _AppResources {
    char *data_dir,
         *prog_dir,
         *help_file,
         *dev_file,
         *show_text,
         *db_dir;
} AppResources;

typedef struct _arcenv {
    Widget hours,
            minutes,
            devicelist,
            joblist,
            backup_filelist,
            restore_filelist,
            backup_tb[NUM_BACKUP_LABELS],
            restore_tb[NUM_RESTORE_LABELS],
            copyjob[NUM_COPYJOB_LABELS],
            enable_toggle,
            days_toggle[NUM_DAYS],
            backup_bb,
            backup_filelist_bb,
            main_bb,
            menubar,
            restore_bb,
            restore_filelist_bb;
    char daystr[NUM_DAYS+1];
    int is_enabled,
        selection_type,
        device_row_num,
        job_index,
        msgtype,
        show_size,
        pid;
} arcenv, *arcenv_t;

typedef struct _ArrowData {
    Widget w;
    int incr,
        rollover;
} ArrowData, *ArrowData_t;

typedef struct _DeviceData {
    char name[MAX_DEVNAME_LEN],
            dir[MAX_DEVNAME_LEN],
            file[MAX_FILENAME_LEN];
    int lock_id,
        is_drive;
} DeviceData;

typedef struct _JobData {
    char name[MAX_JOBNAME_LEN+1],
            archive[MAX_ITEM_LEN],
            email[MAX_ITEM_LEN],
            dir[MAX_FILENAME_LEN],
            daystr[NUM_DAYS+1];
    int hours,
        minutes,
        is_enabled,
        msg_type;
} JobData;

typedef enum _BackupField { B_JOBNAME_FLD, B_ARCNAME_FLD, B_DEVICE_FLD,
                                B_EMAIL_FLD, B_MSGTYPE_FLD,
                                B_SIZE_FLD } BackupField;

typedef enum _RestoreField { R_ARCNAME_FLD, R_DEVICE_FLD, R_TIME_FLD,
                                R_DATE_FLD, R_SIZE_FLD } RestoreField;



/** GLOBAL DECLARATIONS **/

/** fs_archive.c **/

extern Widget toplevel, restore_shell, backup_shell, fsb_dialog;
extern Widget changedev_dialog, copyjob_dialog, joblist_tb;
extern XtAppContext context;
extern XContext env;
extern AppResources resources;
extern Display *display;
extern char machine_type[];
extern int filelist_size;
extern int device_lock;
extern XtIntervalId timer_id;
extern XtInputId input_id;

/** widgets.c **/

extern void exit_archive(int);
extern void create_main_widget(Widget, arcenv_t);
extern Widget create_restore_widget(Widget, arcenv_t);
extern Widget create_backup_widget(Widget, arcenv_t);
extern Widget create_changedev_widget(Widget, int);
extern Widget create_copyjob_widget(Widget, arcenv_t);

/** callbacks.c **/

extern void arrow_activate(Widget, XtPointer, XmArrowButtonCallbackStruct*);
extern void backup_enable_callback(Widget, char*, XmAnyCallbackStruct*);
extern void show_size_callback(Widget, XtPointer, XmToggleButtonCallbackStruct*);
extern void backup_days_callback(Widget, int, XmToggleButtonCallbackStruct*);
extern void wm_close_callback(Widget, XtPointer, XmAnyCallbackStruct*);
extern void msgtype_sel_callback(Widget, XtPointer, XmAnyCallbackStruct*);
extern void system_menu_callback(Widget, int);
extern void job_menu_callback(Widget, int);
extern void device_menu_callback(Widget, int);
extern void changedev_bb_callback(Widget, XtPointer, XmSelectionBoxCallbackStruct*);
extern void main_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void devicelist_sel_callback(Widget, XtPointer, XmListCallbackStruct*);
extern void restore_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void restore_filelist_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void restore_filelist_sel_callback(Widget, XtPointer, XmListCallbackStruct*);
extern void backup_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void backup_filelist_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void copyjob_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
extern void change_backup_buttons(Widget, Widget, Boolean);
extern void read_header(void);
extern void joblist_findbtn_callback(Widget, XtPointer, XtPointer);

/** misc_fcts.c **/

extern DeviceData dev_data[];
extern int num_devices;
extern JobData job_data[];
extern int num_jobs;
extern int num_job_rows;

extern int get_item(int, int, char*, char*);
extern int determine_machine_type(void);
extern void read_devicelist_in(XtPointer, int*, XtInputId*);
extern void read_backup_filelist_in(XtPointer, int*, XtInputId*);
extern void read_restore_filelist_in(XtPointer, int*, XtInputId*);
extern Problem prepare_archive_dirs(char*, char*);
extern int get_num_jobs(int);
extern Problem get_job_data(int);
extern int get_job_row_num(char*);
extern int get_device_row_num(char*);
extern void set_job_rows(void);
extern char *get_msg_type(int);
extern int get_msg_type_num(char*);
extern void convert_name_to_dir(char*, char*);
extern int get_file_size(char*);
extern int check_job_name(char*);

/** cron_tar_fcts.c **/

extern int change_cron_state(char*, int, JobData*, int);
extern int return_cron_state(char*);
extern int convert_daystr_to_days(char*, char*);
extern int launch_process(char**, int*);
extern void reaper(int);
extern void read_from_process(XtPointer, int*, XtInputId*);
extern void check_timer(XtPointer, XtInputId*);

/** file_sel_box.c **/

extern Widget create_fsb_widget(Widget, char*);

/** tarlist_fcts.c **/

extern Problem create_tarlist(char*, char*);
extern Problem determine_filelist_size(char*, int*);
extern void build_file_listing(char*, char*);
extern void get_owner(char*, struct stat*);


#endif /* _fs_archive_h */
