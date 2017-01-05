
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * problem_defs.h
 */

#ifndef _problem_defs_h
#define _problem_defs_h

/** SYSTEM INCLUDES **/

#include <errno.h>

/** INRI INCLUDES **/

#include <MiscXFcts.h>


/** GLOBAL DECLARATIONS **/

extern ProblemItem problem_array[];


/** DEFINE DEFINITIONS **/

#define WRONG_NUM_ARGS_PROB					(ENOSYS + 1)
#define UNLINK_FILE_PROB					(ENOSYS + 2)
#define CANT_CREATE_FILE_PROB				(ENOSYS + 3)
#define CANT_READ_FILE_PROB					(ENOSYS + 4)
#define CANT_APPEND_FILE_PROB				(ENOSYS + 5)

#define CRONTAB_ADD_PROB					(ENOSYS + 6)
#define CRONTAB_LIST_PROB					(ENOSYS + 7)

#define CANT_START_PROGRAM_PROB				(ENOSYS + 8)
#define BACKUP_MSG_SCRIPT_PROB				(ENOSYS + 9)

#define MT_FSF_FAILED_PROB					(ENOSYS + 10)
#define MT_REW_FAILED_PROB					(ENOSYS + 11)
#define MT_EJECT_FAILED_PROB				(ENOSYS + 12)

#define TAR_EXTRACT_FAILED_PROB				(ENOSYS + 13)
#define TAR_CREATE_FAILED_PROB				(ENOSYS + 14)

#define BAD_THING_PROB						(ENOSYS + 15)
#define FILE_OR_DIR_ALREADY_IN_LIST_PROB	(ENOSYS + 16)
#define NO_THING_SELECTED_PROB				(ENOSYS + 17)
#define TOO_MANY_FILES_SELECTED_PROB		(ENOSYS + 18)
#define CHMOD_PROB							(ENOSYS + 19)

#define NO_INSTRUCTIONS_FILE_PROB			(ENOSYS + 20)
#define DONT_APPLY_PARENT_DIRS_PROB			(ENOSYS + 21)
#define CANT_DETERMINE_DEVICES_PROB			(ENOSYS + 22)
#define NO_EXIT_THRU_WM_PROB				(ENOSYS + 23)

#define CANT_OPEN_THING_PROB				(ENOSYS + 24)
#define CANT_READ_THING_PROB				(ENOSYS + 25)
#define CANT_WRITE_THING_PROB				(ENOSYS + 26)
#define CANT_COPY_THING_PROB				(ENOSYS + 27)
#define CANT_CREATE_THING_PROB				(ENOSYS + 28)
#define CANT_REMOVE_THING_PROB				(ENOSYS + 29)

#define TOO_MANY_JOBS_PER_DEV_PROB			(ENOSYS + 30)
#define NO_SUCH_THING_PROB					(ENOSYS + 31)

#define BAD_CONFIG_FILE_PROB				(ENOSYS + 32)
#define BAD_HEADER_FILE_PROB				(ENOSYS + 33)
#define BAD_FILELIST_FILE_PROB				(ENOSYS + 34)
#define BAD_WORKLIST_FILE_PROB				(ENOSYS + 35)
#define BAD_TARLIST_FILE_PROB				(ENOSYS + 36)
#define CANT_CREATE_WORKLIST_FILE_PROB		(ENOSYS + 37)
#define CANT_CREATE_HEADER_FILE_PROB		(ENOSYS + 38)
#define CANT_DETER_FILELIST_SIZE_PROB		(ENOSYS + 39)
#define CANT_CREATE_TARLIST_FILE_PROB		(ENOSYS + 40)

#define VERSION_INFO_PROB					(ENOSYS + 41)

#define THING_ALREADY_EXISTS_PROB			(ENOSYS + 42)
#define INVALID_THING_PROB					(ENOSYS + 43)
#define SEL_ONE_DAY_BACKUP_PROB				(ENOSYS + 44)
#define DEVICE_CANT_BE_THING_PROB			(ENOSYS + 45)
#define CANT_THING_TIMED_JOB_PROB			(ENOSYS + 46)
#define DISABLE_JOB_BEFORE_THING_PROB		(ENOSYS + 47)
#define CANT_GET_DEVICE_LOCK_PROB			(ENOSYS + 48)
#define INIT_APP_PROB						(ENOSYS + 49)

#endif /* _problem_defs_h */
