
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * problem_defs.c
 */

/** INRI INCLUDES **/

#include <MiscXFcts.h>

/** LOCAL INCLUDES **/

#include "problem_defs.h"
#include "inri_version.h"


/** GLOBAL VARIABLES **/

ProblemItem problem_array[] =
{
	{ WRONG_NUM_ARGS_PROB,
	  "Wrong number of arguments." },

	{ UNLINK_FILE_PROB,
	  "Failed to remove file." },

	{ CANT_CREATE_FILE_PROB,
	  "Failed to create file." },

	{ CANT_READ_FILE_PROB,
	  "Failed to read file." },

	{ CANT_APPEND_FILE_PROB,
	  "Failed to append file." },

	{ CRONTAB_ADD_PROB,
	  "Failed to register timed backup job." },

	{ CRONTAB_LIST_PROB,
	  "Failed to list timed backup jobs." },

	{ CANT_START_PROGRAM_PROB,
	  "Failed to start %s process." },

	{ BACKUP_MSG_SCRIPT_PROB,
	  "Failed to create message or run scripts." },

	{ MT_FSF_FAILED_PROB,
	  "Failed to fast forward media in device." },

	{ MT_REW_FAILED_PROB,
	  "Failed to rewind media in device." },

	{ MT_EJECT_FAILED_PROB,
	  "Failed to eject media from device." },

	{ TAR_EXTRACT_FAILED_PROB,
	  "Failed to extract from device." },

	{ TAR_CREATE_FAILED_PROB,
	  "Failed to write to device." },

	{ BAD_THING_PROB,
	  "Missing or invalid %s." },

	{ FILE_OR_DIR_ALREADY_IN_LIST_PROB,
	  "File or directory already in list." },

	{ NO_THING_SELECTED_PROB,
	  "No %s selected." },

	{ TOO_MANY_FILES_SELECTED_PROB,
	  "Only one file or directory can be selected." },

	{ CHMOD_PROB,
	  "Failed to change file permissions." },

	{ NO_INSTRUCTIONS_FILE_PROB,
	  "No instruction text available." },

	{ DONT_APPLY_PARENT_DIRS_PROB,
	  "Do not add parent directories." },

	{ CANT_DETERMINE_DEVICES_PROB,
	  "No devices found." },

	{ NO_EXIT_THRU_WM_PROB,
	  "Window cannot be closed by this method." },

	{ CANT_OPEN_THING_PROB,
	  "Failed to open %s." },

	{ CANT_READ_THING_PROB,
	  "Failed to read from %s."},

	{ CANT_WRITE_THING_PROB,
	  "Failed to write to %s."},

	{ CANT_COPY_THING_PROB,
	  "Failed to copy %s."},

	{ CANT_CREATE_THING_PROB,
	  "Failed to create %s."},

	{ CANT_REMOVE_THING_PROB,
	  "Failed to remove %s."},

	{ TOO_MANY_JOBS_PER_DEV_PROB,
	  "Too many backup jobs for device." },

	{ NO_SUCH_THING_PROB,
	  "No such %s." },

	{ BAD_CONFIG_FILE_PROB,
	  "Backup job configuration file is\nmissing or contains improper data." },

	{ BAD_HEADER_FILE_PROB,
	  "Backup job header file is\nmissing or contains improper data." },

	{ BAD_FILELIST_FILE_PROB,
	  "Backup job filelist file is\nmissing or contains improper data." },

	{ BAD_WORKLIST_FILE_PROB,
	  "Backup job worklist file is\nmissing or contains improper data." },

	{ BAD_TARLIST_FILE_PROB,
	  "Backup job tarlist file is\nmissing or contains improper data." },

	{ CANT_CREATE_WORKLIST_FILE_PROB,
	  "Failed to create backup job worklist file." },

	{ CANT_CREATE_HEADER_FILE_PROB,
	  "Failed to create backup job header file." },

	{ CANT_DETER_FILELIST_SIZE_PROB,
	  "Failed to determine the size\nof all the backup job files." },

	{ CANT_CREATE_TARLIST_FILE_PROB,
	  "Failed to create backup job tarlist file." },

	{ VERSION_INFO_PROB,
	  version_id },

	{ THING_ALREADY_EXISTS_PROB,
	  "%s already exists." },

	{ INVALID_THING_PROB,
	  "Invalid %s." },

	{ SEL_ONE_DAY_BACKUP_PROB,
	  "Select at least one day for backup." },

	{ DEVICE_CANT_BE_THING_PROB,
	  "Device cannot be %s." },

	{ CANT_THING_TIMED_JOB_PROB,
	  "Failed to %s timed backup job." },

	{ DISABLE_JOB_BEFORE_THING_PROB,
	  "Disable backup job before %s." },

	{ CANT_GET_DEVICE_LOCK_PROB,
	  "Failed to acquire lock for device." },

	{ INIT_APP_PROB,
	  "Cannot start the program.\n%s." }
};
