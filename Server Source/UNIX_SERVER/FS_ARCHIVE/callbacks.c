
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * callbacks.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/Text.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <ButtonBox.h>
#include <MiscXFcts.h>
#include <Menus.h>
#include <NtcssHelp.h>
#include <ExtList.h>
#include <ComboBox.h>

/** LOCAL INCLUDES **/

#include "fs_archive.h"
#include "problem_defs.h"
#include "fs_archive_help.h"


/** FUNCTION PROTOTYPES **/

static void adjust_quantum(XtPointer, XtIntervalId*);
void arrow_activate(Widget, XtPointer, XmArrowButtonCallbackStruct*);
void backup_enable_callback(Widget, char*, XmAnyCallbackStruct*);
void show_size_callback(Widget, XtPointer, XmToggleButtonCallbackStruct*);
void backup_days_callback(Widget, int, XmToggleButtonCallbackStruct*);
void wm_close_callback(Widget, XtPointer, XmAnyCallbackStruct*);
void msgtype_sel_callback(Widget, XtPointer, XmAnyCallbackStruct*);
void system_menu_callback(Widget, int);
void job_menu_callback(Widget, int);
void device_menu_callback(Widget, int);
void changedev_bb_callback(Widget, XtPointer, XmSelectionBoxCallbackStruct*);
void main_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
void devicelist_sel_callback(Widget, XtPointer, XmListCallbackStruct*);
void restore_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
static void ok_restore_callback(Widget, XtPointer, XmAnyCallbackStruct*);
void restore_filelist_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*)
;
void restore_filelist_sel_callback(Widget, XtPointer, XmListCallbackStruct*);
void backup_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
void backup_filelist_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
void copyjob_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
void change_backup_buttons(Widget, Widget, Boolean);
static void delete_job_callback(Widget, XtPointer, XmAnyCallbackStruct*);
static void do_backup(void);
static void ok_backup_callback(Widget, XtPointer, XmAnyCallbackStruct*);
static void run_backup(void);
static void do_restore(void);
void read_header(void);
static void run_restore(void);
static void do_rewind(void);
static void do_eject(void);



/** STATIC VARIABLES **/

static XtIntervalId arrow_timer;
ExtdListItemStruct *rowinfo;


/** FUNCTION DEFINITIONS **/

static void adjust_quantum(XtPointer client, XtIntervalId *id)
{
    char *value, buf[3];
    int n;
    ArrowData_t ad = (ArrowData_t) client;
    
    value = XmTextGetString(ad->w);
    n = atoi(value) + ad->incr;
    if (n > ad->rollover)
	n = 0;
    else if (n < 0)
	n = ad->rollover;
    sprintf(buf, "%2.2d", n);
    XmTextSetString(ad->w, buf);
    XtFree(value);
    
    arrow_timer = XtAppAddTimeOut(context, (int) id == -1 ? 500 : 100,
				  adjust_quantum, client);
}



void arrow_activate(Widget w, XtPointer client, XmArrowButtonCallbackStruct *call)
{
    arcenv_t aenv;
    static ArrowData ad;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    if (call->event->type != ButtonPress &&
	call->event->type != ButtonRelease)
	return;
	
    if (call->reason == XmCR_ARM) {
	FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
	switch ((int) client) {
	case HOUR_ARROW_UP:
	    ad.w = aenv->hours;
	    ad.rollover = HIGH_HOUR;
	    ad.incr = 1;
	    break;
	case HOUR_ARROW_DOWN:
	    ad.w = aenv->hours;
	    ad.rollover = HIGH_HOUR;
	    ad.incr = -1;
	    break;
	case MINUTE_ARROW_UP:
	    ad.w = aenv->minutes;
	    ad.rollover = HIGH_MINUTE;
	    ad.incr = 1;
	    break;
	case MINUTE_ARROW_DOWN:
	    ad.w = aenv->minutes;
	    ad.rollover = HIGH_MINUTE;
	    ad.incr = -1;
	    break;
	}
	adjust_quantum((XtPointer) &ad, (XtIntervalId*)-1);
    } else
	XtRemoveTimeOut(arrow_timer);
}



void backup_enable_callback(Widget w, char *args, XmAnyCallbackStruct *call)
{
    int is_enabled;
    XmString s;
    arcenv_t aenv;

    /* done to get rid of compiler warning about unused parameters */
    args=args;
    call=call;
	
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    XtVaGetValues(w, XmNuserData, (XtPointer) &is_enabled, NULL);
    if (is_enabled) {
	aenv->is_enabled = 0;
	s = XmStringCreate("Job Disabled", XmSTRING_DEFAULT_CHARSET);
    } else {
	aenv->is_enabled = 1;
	s = XmStringCreate("Job Enabled ", XmSTRING_DEFAULT_CHARSET);
    }
    XtVaSetValues(w, XmNuserData, (XtPointer) aenv->is_enabled,
		  XmNlabelString, s,
		  NULL);
    XmStringFree(s);
    SAVECONTEXT(toplevel, env, (caddr_t)aenv);    /* FIX do you need this */
}



void show_size_callback(Widget w, XtPointer client, 
			XmToggleButtonCallbackStruct *call)
{
    arcenv_t aenv;

    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    aenv->show_size = call->set;
}



void backup_days_callback(Widget w, int which, XmToggleButtonCallbackStruct *call)
{
    arcenv_t aenv;

    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    if (call->set)
	aenv->daystr[which] = '1';
    else
	aenv->daystr[which] = '0';
    SAVECONTEXT(toplevel, env, (caddr_t)aenv); /* FIX do you need this */
}



void wm_close_callback(Widget w, XtPointer client, XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    client=client;
    call=call;
#ifndef HPUX
    if (call->reason == XmCR_PROTOCOLS)
#endif
	displayProblem(WARNING, NO_EXIT_THRU_WM_PROB,
		       problem_array, w);
}



void msgtype_sel_callback(Widget w, XtPointer client, XmAnyCallbackStruct *call)
{
    XiComboBoxCallbackStruct *cbs = (XiComboBoxCallbackStruct *) call;
    arcenv_t aenv;
    char *s;

    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;

    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    
    s = XiComboBoxGetTextString(w);

    if (s && *s) {
	if (get_msg_type_num(s) != aenv->msgtype) {
	    aenv->msgtype = get_msg_type_num(s);
	}
    }
    XtFree(s);
}



void system_menu_callback(Widget w, int selection)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    switch (selection) {
	/* BACKUP Case */
    case 0:
	do_backup();
	break;

	/* RESTORE Case */
    case 1:
	do_restore();
	break;

	/* QUIT Case */
    case 2:
	exit_archive(0);
	break;
    }
}



void job_menu_callback(Widget w, int selection)
{
    FILE *fp;
    arcenv_t aenv;
    char title[MAX_ITEM_LEN], time_[3];
    char file[MAX_FILENAME_LEN], cmd[MAX_CMD_LEN], *job;
    int i, j, count;
    static int backup_shell_status = SHELL_START;
    XmString s;

    /* done to get rid of compiler warning about unused parameters */
    count=count;
    w=w;

    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    /*
     * Check if a device is selected.
     */
    XtVaGetValues(aenv->devicelist, XmNselectedItemCount, &i, NULL);
    if (aenv->device_row_num == -1 || i <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "device");
	return;
    }
    if ((selection == 0 || selection == 2) && 
	(num_jobs >= MAX_NUM_JOBS_PER_DEV)) {
	displayProblem(WARNING, TOO_MANY_JOBS_PER_DEV_PROB,
		       problem_array, toplevel);
	return;
    }
    switch (selection) {
	/* ADD and EDIT Cases */
    case 0:
    case 1:
	waitCursor(toplevel, True, False);
	aenv->selection_type = selection;
	switch (backup_shell_status) {
	case SHELL_START:
	    (void) checkForStop(toplevel);
	    (void) checkForStop(toplevel);
	    backup_shell = create_backup_widget(toplevel, aenv);
	    backup_shell_status = SHELL_UP;
	    
	case SHELL_UP:
	    (void) checkForStop(toplevel);
	    (void) checkForStop(toplevel);
	    if (selection == 1) {
		sprintf(title, "Edit Backup Job");
		set_runtime_window_help_vector(_EditBackupJob_WindowDescription);
	    } else if (selection == 0) {
		sprintf(title, "Add Backup Job");
		set_runtime_window_help_vector(_AddBackupJob_WindowDescription);
	    }
	    XtVaSetValues(backup_shell, XmNtitle, title, NULL);
	    /*
	     * Put job data into various text fields.
	     */
	    for (i = 0; i < NUM_BACKUP_LABELS; i++)
		if (i == B_JOBNAME_FLD ||
		    i == B_ARCNAME_FLD ||
		    i == B_DEVICE_FLD ||
		    i == B_EMAIL_FLD ||
		    i == B_SIZE_FLD)
		    XmTextFieldSetString(aenv->backup_tb[i], "");
	    
	    if (selection == 1) {
		XtVaSetValues(aenv->backup_tb[B_JOBNAME_FLD],
			      XmNeditable, False,
			      XmNcursorPositionVisible, False,
			      NULL);
		i = -1;
		if (XiExtdListGetSelectedItems(aenv->joblist,
					       &rowinfo) == 1) {
		    XmStringGetLtoR(rowinfo->item[0],
				    XmSTRING_DEFAULT_CHARSET, &job);
		    i = get_job_row_num(job);
		    XtFree(job);
		}
		if (i != -1) {
		    aenv->job_index = i;
		    XmTextFieldSetString(aenv->backup_tb[B_JOBNAME_FLD],
					 job_data[i].name);
		    XmTextFieldSetString(aenv->backup_tb[B_ARCNAME_FLD],
					 job_data[i].archive);
		    XmTextFieldSetString(aenv->backup_tb[B_DEVICE_FLD],
					 dev_data[aenv->device_row_num - 1].name);
		    XmTextFieldSetString(aenv->backup_tb[B_EMAIL_FLD],
					 job_data[i].email);
		    XiComboBoxSetTextString(aenv->backup_tb[B_MSGTYPE_FLD],
					    get_msg_type(job_data[i].msg_type),
					    False);
		    aenv->msgtype = job_data[i].msg_type;
		    sprintf(time_, "%2.2d", job_data[i].hours);
		    XmTextSetString(aenv->hours, time_);
		    sprintf(time_, "%2.2d", job_data[i].minutes);
		    XmTextSetString(aenv->minutes, time_);
		    aenv->is_enabled = job_data[i].is_enabled;
		    if (aenv->is_enabled)
			s = XmStringCreate("Job Enabled ",
					   XmSTRING_DEFAULT_CHARSET);
		    else
			s = XmStringCreate("Job Disabled",
					   XmSTRING_DEFAULT_CHARSET);
		    XtVaSetValues(aenv->enable_toggle,
				  XmNset, (aenv->is_enabled?True:False),
				  XmNlabelString, s,
				  XmNuserData, (XtPointer) aenv->is_enabled,
				  NULL);
		    XmStringFree(s);
		    strcpy(aenv->daystr, job_data[i].daystr);
		    for (j = 0; j < NUM_DAYS; j++)
			XtVaSetValues(aenv->days_toggle[j],
				      XmNset, ((aenv->daystr[j] == '1')?True:False),
				      NULL);
		    /*
		     * Copy file list to temporary list
		     */
		    (void) checkForStop(toplevel);
		    sprintf(cmd, "%s %s/%s %s/%s", CP_PROG,
			    job_data[i].dir, "filelist",
			    job_data[i].dir, "templist");
		    (void) system(cmd);
		    sprintf(cmd, "%s/%s", job_data[i].dir, "templist");
		    if (access(cmd, F_OK) == -1) {
			waitCursor(toplevel, False, False);
			displayProblem(WARNING, CANT_COPY_THING_PROB,
				       problem_array, toplevel,
				       "backup job filelist");
			return;
		    }
		    /*
		     * Read in backup filelist.
		     */
		    sprintf(file, "%s/%s", job_data[i].dir, "templist");
		    if ((fp = fopen(file, "r")) != NULL) {
			XmListDeleteAllItems(aenv->backup_filelist);
			/*
			 * Pop up edit backup job shell.
			 */
			waitCursor(toplevel, False, False);
			set_default_help_info(RUNTIME_WINDOW_INDICATOR,
					      _BackupFileList);
			popupCenterDialog(toplevel, backup_shell,
					  BACKUP_WIDTH, BACKUP_HEIGHT);
			if (aenv->show_size) {
			    filelist_size = 0;
			    sprintf(title, "%d", filelist_size);
			} else
			    strcpy(title, "Not Determined");
			XmTextFieldSetString(aenv->backup_tb[B_SIZE_FLD], title);
			waitCursor(backup_shell, True, False);
			XtAppAddInput(context, fileno(fp),
				      (XtPointer)XtInputReadMask,
				      read_backup_filelist_in, fp);
		    } else {
			waitCursor(toplevel, False, False);
			displayProblem(WARNING, CANT_READ_THING_PROB,
				       problem_array, toplevel,
				       "backup job filelist");
			return;
		    }
		} else {
		    waitCursor(toplevel, False, False);
		    displayProblem(WARNING, NO_THING_SELECTED_PROB,
				   problem_array, toplevel,
				   "backup job");
		    return;
						}
	    } else if (selection == 0) {
		/*
		 * Create temp dir for new job.
		 */
		(void) checkForStop(toplevel);
		sprintf(cmd, "%s/backup/%s/tmp_tmp_tmp",
			resources.data_dir,
			dev_data[aenv->device_row_num - 1].dir);
		if ((access(cmd, F_OK) == -1) &&
		    (mkdir(cmd,
			   S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
		     != 0)) {
		    waitCursor(toplevel, False, False);
		    displayProblem(WARNING, CANT_CREATE_THING_PROB,
				   problem_array, toplevel,
				   "backup job directory");
		    return;
		}
		(void) checkForStop(toplevel);
		sprintf(job_data[num_jobs].dir,
			"%s/backup/%s/tmp_tmp_tmp",
			resources.data_dir,
			dev_data[aenv->device_row_num - 1].dir);
		sprintf(file, "%s/backup/%s/tmp_tmp_tmp/templist",
			resources.data_dir,
			dev_data[aenv->device_row_num - 1].dir);
		if ((fp = fopen(file, "w")) == NULL) {
		    waitCursor(toplevel, False, False);
		    displayProblem(WARNING, CANT_WRITE_THING_PROB,
				   problem_array, backup_shell,
				   "backup job filelist");
		    return;
		} else
		    fclose(fp);
		aenv->job_index = num_jobs;
		aenv->is_enabled = 0;
		s = XmStringCreate("Job Disabled",
				   XmSTRING_DEFAULT_CHARSET);
		XtVaSetValues(aenv->enable_toggle,
			      XmNset, False,
			      XmNlabelString, s,
			      XmNuserData, (XtPointer) aenv->is_enabled,
			      NULL);
		XmStringFree(s);
		strcpy(aenv->daystr, "0000000");
		for (j = 0; j < NUM_DAYS; j++)
		    XtVaSetValues(aenv->days_toggle[j],
				  XmNset, False,
				  NULL);
		(void) checkForStop(toplevel);
		XmTextSetString(aenv->hours, "00");
		XmTextSetString(aenv->minutes, "00");
		XtVaSetValues(aenv->backup_tb[B_JOBNAME_FLD],
			      XmNeditable, True,
			      XmNcursorPositionVisible, True,
			      NULL);
		XiComboBoxSetTextString(aenv->backup_tb[B_MSGTYPE_FLD],
					get_msg_type(0), False);
		aenv->msgtype = 0;
		XmTextFieldSetString(aenv->backup_tb[B_DEVICE_FLD],
				     dev_data[aenv->device_row_num - 1].name);
		XmListDeleteAllItems(aenv->backup_filelist);
		if (aenv->show_size) {
		    filelist_size = 0;
		    sprintf(title, "%d", filelist_size);
		} else
		    strcpy(title, "Not Determined");
		XmTextFieldSetString(aenv->backup_tb[B_SIZE_FLD],
				     title);
		/*
		 * Pop up edit backup job shell.
		 */
		waitCursor(toplevel, False, False);
		set_default_help_info(RUNTIME_WINDOW_INDICATOR,_BackupFileList);
		popupCenterDialog(toplevel, backup_shell,
				  BACKUP_WIDTH, BACKUP_HEIGHT);
	    }
	    break;
	}
	break;
	
	/* COPY Case */
    case 2:
	i = -1;
	if (XiExtdListGetSelectedItems(aenv->joblist,
				       &rowinfo) == 1) {
	    XmStringGetLtoR(rowinfo->item[0],
			    XmSTRING_DEFAULT_CHARSET, &job);
	    i = get_job_row_num(job);
	    XtFree(job);
	}
	if (i != -1) {
	    waitCursor(toplevel, True, False);
	    (void) checkForStop(toplevel);
	    (void) checkForStop(toplevel);
	    aenv->job_index = i;
	    if (job_data[i].is_enabled) {
		waitCursor(toplevel, False, False);
		displayProblem(WARNING, DISABLE_JOB_BEFORE_THING_PROB,
			       problem_array, toplevel,
			       "copying");
		return;
	    }
	    copyjob_dialog = create_copyjob_widget(toplevel, aenv);
	    XmTextFieldSetString(aenv->copyjob[0], job_data[i].name);
	    XmTextFieldSetString(aenv->copyjob[1], "");
	    waitCursor(toplevel, False, False);
	    set_default_help_info(_CopyBackupJob_WindowDescription,_OldName);
	    popupCenterDialog(toplevel, copyjob_dialog,
			      COPYJOB_WIDTH, COPYJOB_HEIGHT);
	} else {
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, toplevel,
			   "backup job");
	    return;
	}
	break;
	
	/* DELETE Case */
    case 3:
	i = -1;
	if (XiExtdListGetSelectedItems(aenv->joblist,
				       &rowinfo) == 1) {
	    XmStringGetLtoR(rowinfo->item[0],
			    XmSTRING_DEFAULT_CHARSET, &job);
	    i = get_job_row_num(job);
	    XtFree(job);
	}
	if (i != -1) {
	    aenv->job_index = i;
	    if (job_data[i].is_enabled) {
		displayProblem(WARNING, DISABLE_JOB_BEFORE_THING_PROB,
			       problem_array, toplevel,
			       "deleting");
		return;
	    }
	    popupDialogBox(QUESTION, True, DELETE_JOB_QUEST,
			   True, True, False,
			   delete_job_callback, toplevel);
	} else {
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, toplevel,
			   "backup job");
	    return;
	}
	break;
	
	/* CHANGE DEVICE Case */
    case 4:
	i = -1;
	if (XiExtdListGetSelectedItems(aenv->joblist,
				       &rowinfo) == 1) {
	    XmStringGetLtoR(rowinfo->item[0],
			    XmSTRING_DEFAULT_CHARSET, &job);
	    i = get_job_row_num(job);
	    XtFree(job);
	}
	if (i != -1) {
	    aenv->job_index = i;
	    if (job_data[i].is_enabled) {
		displayProblem(WARNING, DISABLE_JOB_BEFORE_THING_PROB,
			       problem_array, toplevel,
			       "changing device");
		return;
	    }
	    changedev_dialog = create_changedev_widget(toplevel,
						       aenv->device_row_num - 1);
	} else {
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, toplevel,
			   "backup job");
	    return;
	}
	break;
    }
}



void device_menu_callback(Widget w, int selection)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    switch (selection) {
	/* REWIND Case */
    case 0:
	do_rewind();
	break;
	
	/* EJECT Case */
    case 1:
	do_eject();
	break;
    }
}



void changedev_bb_callback(Widget w, XtPointer client, 
			   XmSelectionBoxCallbackStruct *call)
{
    char *device, file[MAX_FILENAME_LEN], cmd[MAX_CMD_LEN];
    int dev_i, n, job_i;
    JobData newjob;
    arcenv_t aenv;
    Problem prob;
    
    /* done to get rid of compiler warning about unused parameters */
    client=client;
    
    switch (call->reason) {
	/* CANCEL Case */
    case XmCR_CANCEL:
	XtDestroyWidget(XtParent(w));
	restore_help_info();
	break;
	
	/* NO MATCH Case */
    case XmCR_NO_MATCH:
	displayProblem(WARNING, NO_SUCH_THING_PROB,
		       problem_array, changedev_dialog,
		       "device");
	break;
	
	/* OK Case */
    case XmCR_OK:
	FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
	if (XmStringGetLtoR(call->value, XmSTRING_DEFAULT_CHARSET,
			    &device)) {
	    waitCursor(changedev_dialog, True, False);
	    (void) checkForStop(changedev_dialog);
	    (void) checkForStop(changedev_dialog);
	    if ((dev_i = get_device_row_num(device)) == -1) {
		XtFree(device);
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, NO_SUCH_THING_PROB,
			       problem_array, changedev_dialog,
			       "device");
		return;
	    }
	    XtFree(device);
	    sprintf(file, "%s/backup/%s", resources.data_dir,
		    dev_data[dev_i].dir);
	    if ((n = get_num_jobs(dev_i)) == -1) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, CANT_OPEN_THING_PROB,
			       problem_array, changedev_dialog,
			       "backup job directory");
		return;
	    } else if (n >= MAX_NUM_JOBS_PER_DEV) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, TOO_MANY_JOBS_PER_DEV_PROB,
			       problem_array, changedev_dialog);
		return;
	    }
	    (void) checkForStop(changedev_dialog);
	    
	    job_i = aenv->job_index;
	    strcpy(newjob.name, job_data[job_i].name);
	    convert_name_to_dir(newjob.name, cmd);
	    sprintf(newjob.dir, "%s/%s", file, cmd);
	    strcpy(newjob.archive, job_data[job_i].archive);
	    strcpy(newjob.email, job_data[job_i].email);
	    strcpy(newjob.daystr, job_data[job_i].daystr);
	    newjob.hours = job_data[job_i].hours;
	    newjob.minutes = job_data[job_i].minutes;
	    newjob.is_enabled = 0;
	    newjob.msg_type = job_data[job_i].msg_type;
	    
	    (void) checkForStop(changedev_dialog);
	    if (access(newjob.dir, F_OK) != -1) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, THING_ALREADY_EXISTS_PROB,
			       problem_array, changedev_dialog,
			       "Backup job");
		return;
	    }
	    (void) checkForStop(changedev_dialog);
	    if ((access(newjob.dir, F_OK) == -1)  &&
		(mkdir(newjob.dir,
		       S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
		 != 0 )) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, CANT_CREATE_THING_PROB,
			       problem_array, changedev_dialog,
			       "backup job directory");
		return;
	    }
	    (void) checkForStop(changedev_dialog);
	    prob = create_job_config_file(&newjob, dev_i);
	    if (prob != NO_PROB) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, CANT_CREATE_THING_PROB,
			       problem_array, changedev_dialog,
			       "backup job configuration file");
		return;
	    }
	    (void) checkForStop(changedev_dialog);
	    sprintf(cmd, "%s %s/%s %s/%s", CP_PROG,
		    job_data[job_i].dir, "filelist",
		    newjob.dir, "filelist");
	    (void) system(cmd);
	    sprintf(cmd, "%s/%s", newjob.dir, "filelist");
	    if (access(cmd, F_OK) == -1) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, CANT_COPY_THING_PROB,
			       problem_array, changedev_dialog,
			       "backup job filelist");
		return;
	    }
	    (void) checkForStop(changedev_dialog);
	    sprintf(cmd, "%s %s", RM_PROG, job_data[job_i].dir);
	    (void) system(cmd);
	    if (access(job_data[job_i].dir, F_OK) != -1) {
		waitCursor(changedev_dialog, False, False);
		displayProblem(WARNING, CANT_REMOVE_THING_PROB,
			       problem_array, changedev_dialog,
			       "old backup job directory");
		return;
	    }
	    waitCursor(changedev_dialog, False, False);
	    XtDestroyWidget(XtParent(w));
	    restore_help_info();
	    num_jobs = 0;
	    XiExtdListDeleteAllItems(aenv->joblist);
	    XmListDeselectAllItems(aenv->devicelist);
	    /*
	     * Unset device lock if necessary.
	     */
#ifdef USE_FILE_LOCKS
	    if (device_lock != -1) {
		unsetFileLock(device_lock);
		device_lock = -1;
	    }
#endif
	    aenv->device_row_num = -1;
	} else
	    displayProblem(WARNING, NO_SUCH_THING_PROB,
			   problem_array, changedev_dialog,
			   "device");
	break;
    }
}



void main_bb_callback(Widget w, XtPointer client, XiButtonBoxCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    switch (call->id) {
	/* BACKUP Case */
    case 0:
	do_backup();
	break;
	
	/* RESTORE Case */
    case 1:
	do_restore();
	break;
	
	/* QUIT Case */
    case 2:
	exit_archive(0);
	break;
    }
}



void devicelist_sel_callback(Widget w, XtPointer client, XmListCallbackStruct *call)
{
#ifdef USE_FILE_LOCKS
    char lockfile[MAX_FILENAME_LEN];
    int i, k;
#endif
    int count;
    arcenv_t aenv;
    Problem prob;
    
    /* done to get rid of compiler warning about unused parameters */
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    XtVaGetValues(w, XmNselectedItemCount, &count, NULL);
    if (count == 1) {
	waitCursor(toplevel, True, False);
	prob = get_job_data(call->item_position - 1);
	if (prob != NO_PROB) {
	    aenv->device_row_num = -1;
	    XiExtdListDeleteAllItems(aenv->joblist);
	    XmListDeselectAllItems(w);
	    waitCursor(toplevel, False, False);
	    displayProblem(WARNING, prob,
			   problem_array, toplevel,
			   "device directory");
	    return;
	}
	/*
	 * Acquire device lock if necessary.
	 */
#ifdef USE_FILE_LOCKS
	if (call->item_position != aenv->device_row_num) {
	    i = dev_data[call->item_position - 1].lock_id;
	    sprintf(lockfile, "%s/%s/%s.%d", resources.db_dir, LOCK_DIR,
		    DEVICE_LOCK_FILE, i);
	    if (!setFileLock(lockfile, &k, LOCK_WAIT_TIME)) {
		XmListDeselectAllItems(w);
		if (aenv->device_row_num != -1)
		    XmListSelectPos(w, aenv->device_row_num, False);
		waitCursor(toplevel, False, False);
		displayProblem(WARNING, CANT_GET_DEVICE_LOCK_PROB,
			       problem_array, toplevel);
		return;
	    }
	    if (device_lock != -1)
		unsetFileLock(device_lock);
	    device_lock = k;
	}
#endif
	/*
	 * Reset and refresh joblist.
	 */
	set_job_rows();
	aenv->device_row_num = call->item_position;
	waitCursor(toplevel, False, False);
    } else {
	/*
	 * Unset device lock if necessary.
	 */
#ifdef USE_FILE_LOCKS
	if (device_lock != -1) {
	    unsetFileLock(device_lock);
	    device_lock = -1;
	}
#endif
	aenv->device_row_num = -1;
	XiExtdListDeleteAllItems(aenv->joblist);
    }
}



void restore_bb_callback(Widget w, XtPointer client, XiButtonBoxCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    switch (call->id) {
	/* OK Case */
    case 0:
	popupDialogBox(QUESTION, True, RESTORE_FILES_QUEST,
		       True, True, False,
		       ok_restore_callback, restore_shell);
	break;
	
	/* CANCEL Case */
    case 1:
	XtPopdown(restore_shell);
	restore_help_info();
	break;
    }
}



static void ok_restore_callback(Widget w, XtPointer client_data, 
				XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client_data=client_data;
    
    switch (call->reason) {
	/* OK Case */
    case XmCR_OK:
	run_restore();
	break;
	
	/* CANCEL Case */
    case XmCR_CANCEL:
	break;
    }
}



void restore_filelist_bb_callback(Widget w, XtPointer client, 
				  XiButtonBoxCallbackStruct *call)
{
    arcenv_t aenv;
    int i, count;
    static int sel_all_on = 1;
    XmString s;
    XmStringTable xstr;
    
    /* done to get rid of compiler warning about unused parameters */
    xstr=xstr;
    client=client;
    
    switch (call->id) {
	/* SELECT ALL Case */
    case 0:
	FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
	if (sel_all_on) {
	    sel_all_on = 0;
	    XtVaGetValues(aenv->restore_filelist,
			  XmNitemCount, &count,
			  NULL);
	    for (i = 1; i <= count; i++) {
		XmListDeselectPos(aenv->restore_filelist, i);
		XmListSelectPos(aenv->restore_filelist, i, True);
	    }
	    s = XmStringCreate(" Unselect All ", XmSTRING_DEFAULT_CHARSET);
	    XtVaSetValues(XiButtonBoxGetButton(w, 0),
			  XmNlabelString, s,
			  NULL);
	    XmStringFree(s);
	} else {
	    sel_all_on = 1;
	    XmListDeselectAllItems(aenv->restore_filelist);
	    s = XmStringCreate("  Select All  ", XmSTRING_DEFAULT_CHARSET);
	    XtVaSetValues(XiButtonBoxGetButton(w, 0),
			  XmNlabelString, s,
			  NULL);
	    XmStringFree(s);
	}
	break;
    }
}



void restore_filelist_sel_callback(Widget w, XtPointer client, 
				   XmListCallbackStruct *call)
{
    FILE *fp;
    int i, j, count, last;
    char *file, *nextfile, *s, *str_ptr, *file_ptr;
    arcenv_t aenv;
    XmString xs;
    XmStringTable xstr;
    char dir[MAX_FILELISTING_LEN], tarlist[MAX_FILENAME_LEN];
    char str[MAX_FILELISTING_LEN], line[MAX_FILELISTING_LEN];
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    
    XtVaGetValues(aenv->restore_filelist, XmNitemCount, &count,
		  XmNitems, &xstr, NULL);
    i = call->item_position;
    XmListDeselectPos(aenv->restore_filelist, i);
    XmStringGetLtoR(xstr[i - 1], XmSTRING_DEFAULT_CHARSET, &file);
    if (file[strlen(file) - 1] != '/') {
	XtFree(file);
	return;
    }
    
    if (i < count) {
	last = i + 1;
	while (last <= count) {
			XmStringGetLtoR(xstr[last - 1],
					XmSTRING_DEFAULT_CHARSET, &nextfile);
			if (strstr(&nextfile[FILELISTING_FILE_POS],
				   &file[FILELISTING_FILE_POS]) != NULL)
			    last++;
			else
			    break;
	}
	last--;
	XtFree(nextfile);
	for (j = last; j > i; j--)
	    XmListDeletePos(aenv->restore_filelist, j);
	if (last > i) {
	    XtFree(file);
	    return;
	}
    }
    
    sprintf(tarlist, "%s/%s/%s/%s", resources.data_dir, "restore",
	    dev_data[aenv->device_row_num - 1].dir, "tarlist");
    if ((fp = fopen(tarlist, "r")) == NULL) {
	XtFree(file);
	return;
    }
    file_ptr = &file[FILELISTING_FILE_POS];
    dir[0] = 0;
    while (fgets(str, MAX_FILELISTING_LEN, fp) != NULL) {
	stripNewline(str);
	line[0] = 0;
	str_ptr = &str[FILELISTING_FILE_POS];
	if (strncmp(str_ptr, file_ptr, strlen(file_ptr)) == 0 &&
	    (s = strstr(str_ptr, file_ptr)) != NULL) {
	    s += strlen(file_ptr);
	    if (strchr(s, '/') != NULL) {
		if ((dir[0] == 0) ||
		    (strstr(str_ptr, &dir[FILELISTING_FILE_POS]) == NULL)) {
		    strcpy(dir, str);
		    strcpy(line, str);
		}
	    } else if (*s != 0)
		strcpy(line, str);
	    if (line[0] != 0) {
		xs = XmStringCreateSimple(line);
		XmListAddItem(aenv->restore_filelist, xs, i + 1);
		i++;
		XmStringFree(xs);
	    }
	}
    }
    fclose(fp);
    XtFree(file);
}



void backup_bb_callback(Widget w, XtPointer client, XiButtonBoxCallbackStruct *call)
{
    arcenv_t aenv;
    Problem prob;
    int i, job_i;
    char buf[MAX_ITEM_LEN], *s, cmd[MAX_CMD_LEN], command[MAX_CMD_LEN];
    JobData editjob;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    switch (call->id) {
	/* OK Case */
    case 0:
	job_i = aenv->job_index;
	s = XmTextGetString(aenv->hours);
	editjob.hours = atoi(s);
	XtFree(s);
	s = XmTextGetString(aenv->minutes);
	editjob.minutes = atoi(s);
	XtFree(s);
	
	strcpy(editjob.daystr, aenv->daystr);
	if (strstr(editjob.daystr, "1") == NULL) {
	    displayProblem(WARNING, SEL_ONE_DAY_BACKUP_PROB,
			   problem_array, backup_shell);
	    return;
	}
	
	if (aenv->selection_type == 0) {
	    s = XmTextFieldGetString(aenv->backup_tb[B_JOBNAME_FLD]);
	    if (!check_job_name(s)) {
		XtFree(s);
		displayProblem(WARNING, INVALID_THING_PROB,
			       problem_array, backup_shell,
			       "backup job name");
		return;
	    }
	    strcpy(editjob.name, s);
	    trim(editjob.name);
	    XtFree(s);
	    convert_name_to_dir(editjob.name, buf);
	    sprintf(editjob.dir, "%s/backup/%s/%s",
		    resources.data_dir,
		    dev_data[aenv->device_row_num - 1].dir,
		    buf);
	    if (access(editjob.dir, F_OK) != -1) {
		displayProblem(WARNING, THING_ALREADY_EXISTS_PROB,
			       problem_array, backup_shell,
			       "Backup job");
		return;
	    }
	} else {
	    strcpy(editjob.name, job_data[job_i].name);
	    strcpy(editjob.dir, job_data[job_i].dir);
	}
	
	s = XmTextFieldGetString(aenv->backup_tb[B_ARCNAME_FLD]);
	if (!(s && *s)) {
	    XtFree(s);
	    displayProblem(WARNING, INVALID_THING_PROB,
			   problem_array, backup_shell,
			   "archive name");
	    return;
	}
	ncpy(editjob.archive, s, MAX_ITEM_LEN - 1);
	trim(editjob.archive);
	XtFree(s);
	
	s = XmTextFieldGetString(aenv->backup_tb[B_EMAIL_FLD]);
	if (!(s && *s)) {
	    XtFree(s);
	    displayProblem(WARNING, INVALID_THING_PROB,
			   problem_array, backup_shell,
			   "e-mail address");
	    return;
	}
	ncpy(editjob.email, s, MAX_ITEM_LEN - 1);
	trim(editjob.email);
	XtFree(s);
	
	s = XiComboBoxGetTextString(aenv->backup_tb[B_MSGTYPE_FLD]);
	if (!(s && *s)) {
	    XtFree(s);
	    displayProblem(WARNING, INVALID_THING_PROB,
			   problem_array, backup_shell,
			   "message type");
	    return;
	}
	editjob.msg_type = get_msg_type_num(s);
	XtFree(s);
	
	if (aenv->selection_type == 0) {
	    if (rename(job_data[job_i].dir, editjob.dir) != 0) {
		displayProblem(WARNING, CANT_CREATE_THING_PROB,
			       problem_array, backup_shell,
			       "backup job directory");
		return;
	    }
	}
	prob = create_job_config_file(&editjob, aenv->device_row_num - 1);
	if (prob != NO_PROB) {
	    displayProblem(WARNING, CANT_CREATE_THING_PROB,
			   problem_array, backup_shell,
			   "backup job configuration file");
	    return;
	}
	sprintf(cmd, "%s %s/%s %s/%s", CP_PROG,
		editjob.dir, "templist",
		editjob.dir, "filelist");
	(void) system(cmd);
	sprintf(cmd, "%s/%s", editjob.dir, "filelist");
	if (access(cmd, F_OK) == -1) {
	    displayProblem(WARNING, CANT_COPY_THING_PROB,
			   problem_array, backup_shell,
			   "backup job filelist");
	    return;
	}
	/*
	 * Disable or enable timed backup job.
	 */
	sprintf(command, "%s/%s", resources.prog_dir, "backup_tape");
	if (strcmp(machine_type, "sun") == 0)
	    replacePattern("/hp/", "/sun/", command, cmd);
	else
	    if (strcmp(machine_type, "linux") == 0)
		replacePattern("/hp/", "/linux/", command, cmd);
	    else
		strcpy(cmd, command);
	i = dev_data[aenv->device_row_num - 1].lock_id;
	if (!change_cron_state(cmd, aenv->is_enabled, &editjob, i)) {
	    if (aenv->is_enabled)
		strcpy(command, "enable");
	    else
		strcpy(command, "disable");
	    displayProblem(WARNING, CANT_THING_TIMED_JOB_PROB,
			   problem_array, backup_shell,
			   command);
	    return;
	}
	XmListSelectPos(aenv->devicelist, aenv->device_row_num, True);
	break;
	
	/* CLEAR Case */
    case 1:
	if (aenv->selection_type == 0)
	    XmTextFieldSetString(aenv->backup_tb[B_JOBNAME_FLD], "");
	XmTextFieldSetString(aenv->backup_tb[B_ARCNAME_FLD], "");
	XmTextFieldSetString(aenv->backup_tb[B_EMAIL_FLD], "");
	return;
	break;
	
	/* CANCEL Case */
    case 2:
	break;
    }
    XtPopdown(backup_shell);
    restore_help_info();
}



void backup_filelist_bb_callback(Widget w, XtPointer client, 
				 XiButtonBoxCallbackStruct *call)
{
    int i, j, *pos, count, size;
    arcenv_t aenv;
    char file[MAX_FILENAME_LEN], *line;
    char buf[MAX_ITEM_LEN];
    FILE *fp;
    XmStringTable xstr;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    switch (call->id) {
	/* ADD Case */
    case 0:
	waitCursor(backup_shell, True, False);
	fsb_dialog = create_fsb_widget(backup_shell, NULL);
	waitCursor(backup_shell, False, False);
	change_backup_buttons(aenv->backup_bb,
			      aenv->backup_filelist_bb, False);
	set_default_help_info(_FileSelection_WindowDescription,_Name_Directories);
	popupCenterDialog(backup_shell, fsb_dialog,
			  FILESEL_WIDTH, FILESEL_HEIGHT);
	break;
	
	/* DELETE Case */
    case 1:
	/*
	 * Delete all selected items.
	 */
	if (XmListGetSelectedPos(aenv->backup_filelist,
				 &pos, &count) == True) {
	    waitCursor(backup_shell, True, False);
	    (void) checkForStop(backup_shell);
	    (void) checkForStop(backup_shell);
	    XtVaGetValues(aenv->backup_filelist,
			  XmNselectedItemCount, &j,
			  XmNselectedItems, &xstr,
			  NULL);
	    size = 0;
	    for (i = 0; i < j; i++) {
		(void) checkForStop(backup_shell);
		XmStringGetLtoR(xstr[i], XmSTRING_DEFAULT_CHARSET,
				&line);
		if (aenv->show_size)
		    size += get_file_size(&line[FILELISTING_FILE_POS]);
	    }
	    for (i = (count - 1); i >= 0; i--)
		XmListDeletePos(aenv->backup_filelist, pos[i]);
	    if (aenv->show_size) {
#ifdef HPUX
		size = size/2;
#endif
		filelist_size -= size;
		if (filelist_size < 0)
		    filelist_size = 0;
		sprintf(buf, "%d", filelist_size);
		XmTextFieldSetString(aenv->backup_tb[B_SIZE_FLD], buf);
	    }
	    /*
	     * Update file list.
	     */
	    i = aenv->job_index;
	    sprintf(file, "%s/%s", job_data[i].dir, "templist");
	    if ((fp = fopen(file, "w")) != NULL) {
		XtVaGetValues(aenv->backup_filelist,
			      XmNitemCount, &count,
			      XmNitems, &xstr,
			      NULL);
		for (j = 0; j < count; j++) {
		    (void) checkForStop(backup_shell);
		    XmStringGetLtoR(xstr[j], XmSTRING_DEFAULT_CHARSET,
				    &line);
		    fprintf(fp, "%s\n", &line[FILELISTING_FILE_POS]);
		}
		fclose(fp);
		XtFree(line);
		waitCursor(backup_shell, False, False);
	    } else {
		waitCursor(backup_shell, False, False);
		displayProblem(WARNING, CANT_WRITE_THING_PROB,
			       problem_array, backup_shell,
			       "backup job filelist");
	    }
	} else
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, backup_shell,
			   "file or directory");
	break;
	
	/* MOVE UP Case */
    case 2:
	if (XmListGetSelectedPos(aenv->backup_filelist,
				 &pos, &count) == True) {
	    if (count == 1) {
		if (pos[0] <= 1)
		    return;
		/*
		 * Move file up one.
		 */
		XtVaGetValues(aenv->backup_filelist,
			      XmNselectedItems, &xstr,
			      NULL);
		XmListAddItem(aenv->backup_filelist, xstr[0], pos[0] - 1);
		XmListDeletePos(aenv->backup_filelist, pos[0] + 1);
		/*
		 * Update file list.
		 */
		i = aenv->job_index;
		sprintf(file, "%s/%s", job_data[i].dir, "templist");
		if ((fp = fopen(file, "w")) != NULL) {
		    waitCursor(backup_shell, True, False);
		    XtVaGetValues(aenv->backup_filelist,
				  XmNitemCount, &count,
				  XmNitems, &xstr,
				  NULL);
		    for (j = 0; j < count; j++) {
			(void) checkForStop(backup_shell);
			XmStringGetLtoR(xstr[j], XmSTRING_DEFAULT_CHARSET,
					&line);
			fprintf(fp, "%s\n", &line[FILELISTING_FILE_POS]);
		    }
		    fclose(fp);
		    XtFree(line);
		    waitCursor(backup_shell, False, False);
		} else
		    displayProblem(WARNING, CANT_WRITE_THING_PROB,
				   problem_array, backup_shell,
				   "backup job filelist");
	    } else
		displayProblem(WARNING, TOO_MANY_FILES_SELECTED_PROB,
			       problem_array, backup_shell);
	} else
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, backup_shell,
			   "file or directory");
	break;
	
	/* REFRESH Case */
    case 3:
	i = aenv->job_index;
	sprintf(file, "%s/%s", job_data[i].dir, "templist");
	if ((fp = fopen(file, "r")) != NULL) {
	    XmListDeleteAllItems(aenv->backup_filelist);
	    filelist_size = 0;
	    waitCursor(backup_shell, True, False);
	    XtAppAddInput(context, fileno(fp),
			  (XtPointer)XtInputReadMask,
			  read_backup_filelist_in, fp);
	} else
	    displayProblem(WARNING, CANT_READ_THING_PROB,
			   problem_array, backup_shell,
			   "backup job filelist");
	break;
    }
}



void copyjob_bb_callback(Widget w, XtPointer client, XiButtonBoxCallbackStruct *call)
{
    arcenv_t aenv;
    char *s;
    int job_i, dev_i;
    char buf[MAX_FILENAME_LEN], cmd[MAX_CMD_LEN];
    Problem prob;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    switch (call->id) {
	/* OK Case */
    case 0:
	job_i = aenv->job_index;
	dev_i = aenv->device_row_num - 1;
	s = XmTextFieldGetString(aenv->copyjob[1]);
	if (!check_job_name(s)) {
	    displayProblem(WARNING, INVALID_THING_PROB,
			   problem_array, copyjob_dialog,
			   "backup job name");
	    XtFree(s);
	    return;
	}
	
	convert_name_to_dir(s, buf);
	sprintf(job_data[num_jobs].dir,
		"%s/backup/%s/%s", resources.data_dir,
		dev_data[dev_i].dir, buf);
	if (access(job_data[num_jobs].dir, F_OK) != -1) {
	    displayProblem(WARNING, THING_ALREADY_EXISTS_PROB,
			   problem_array, copyjob_dialog,
			   "Backup job");
	    XtFree(s);
	    return;
	}
	
	strcpy(job_data[num_jobs].name, s);
	XtFree(s);
	strcpy(job_data[num_jobs].archive, job_data[job_i].archive);
	strcpy(job_data[num_jobs].email, job_data[job_i].email);
	strcpy(job_data[num_jobs].daystr, job_data[job_i].daystr);
	job_data[num_jobs].hours = job_data[job_i].hours;
	job_data[num_jobs].minutes = job_data[job_i].minutes;
	job_data[num_jobs].is_enabled = 0;
	job_data[num_jobs].msg_type = job_data[job_i].msg_type;
	
	if ((access(job_data[num_jobs].dir, F_OK) == -1) &&
	    (mkdir(job_data[num_jobs].dir,
		   S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)) {
	    displayProblem(WARNING, CANT_CREATE_THING_PROB,
			   problem_array, copyjob_dialog,
			   "backup job directory");
	    return;
	}
	prob = create_job_config_file(&job_data[num_jobs], dev_i);
	if (prob != NO_PROB) {
	    displayProblem(WARNING, CANT_CREATE_THING_PROB,
			   problem_array, copyjob_dialog,
			   "backup job configuration file");
	    return;
	}
	sprintf(cmd, "%s %s/%s %s/%s", CP_PROG,
		job_data[job_i].dir, "filelist",
		job_data[num_jobs].dir, "filelist");
	(void) system(cmd);
	sprintf(cmd, "%s/%s", job_data[num_jobs].dir, "filelist");
	if (access(cmd, F_OK) == -1) {
	    displayProblem(WARNING, CANT_COPY_THING_PROB,
			   problem_array, toplevel,
			   "backup job filelist");
	    return;
	}
	XmListSelectPos(aenv->devicelist, aenv->device_row_num, True);
	break;
	
	/* CLEAR Case */
    case 1:
	XmTextFieldSetString(aenv->copyjob[1], "");
	return;
	break;
	
	/* CANCEL Case */
    case 2:
	break;
    }
    XtDestroyWidget(copyjob_dialog);
    restore_help_info();
}



void change_backup_buttons(Widget bb, Widget list_bb, Boolean state)
{
    XtVaSetValues(XiButtonBoxGetButton(bb, 0), XmNsensitive, state, NULL);
    XtVaSetValues(XiButtonBoxGetButton(bb, 2), XmNsensitive, state, NULL);
    XtVaSetValues(XiButtonBoxGetButton(list_bb, 0), XmNsensitive, state, NULL);
    XtVaSetValues(XiButtonBoxGetButton(list_bb, 1), XmNsensitive, state, NULL);
    XtVaSetValues(XiButtonBoxGetButton(list_bb, 2), XmNsensitive, state, NULL);
    XtVaSetValues(XiButtonBoxGetButton(list_bb, 3), XmNsensitive, state, NULL);
}



static void delete_job_callback(Widget w, XtPointer client_data, 
				XmAnyCallbackStruct *call)
{
    arcenv_t aenv;
    char cmd[MAX_CMD_LEN];
    int i;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client_data=client_data;
    
    switch (call->reason) {
	/* OK Case */
    case XmCR_OK:
	FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
	sprintf(cmd, "%s %s", RM_PROG, job_data[aenv->job_index].dir);
	(void) system(cmd);
	i = access(job_data[aenv->job_index].dir, F_OK);
	if (i != -1) {
	    displayProblem(WARNING, CANT_REMOVE_THING_PROB,
			   problem_array, toplevel,
			   "backup job directory");
	    return;
	}
	XmListSelectPos(aenv->devicelist, aenv->device_row_num, True);
	break;
	
	/* CANCEL Case */
    case XmCR_CANCEL:
	break;
    }
}



static void do_backup()
{
    int i;
    arcenv_t aenv;
    char *job;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    /*
     * Check if a device is selected.
     */
    XtVaGetValues(aenv->devicelist, XmNselectedItemCount, &i, NULL);
    if (aenv->device_row_num == -1 || i <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "device");
	return;
    }
    /*
     * Check if a backup job is selected.
     */
    i = -1;
    if (XiExtdListGetSelectedItems(aenv->joblist, &rowinfo) == 1) {
		XmStringGetLtoR(rowinfo->item[0], XmSTRING_DEFAULT_CHARSET, &job);
		i = get_job_row_num(job);
		XtFree(job);
    }
    if (i != -1) {
	aenv->job_index = i;
	popupDialogBox(QUESTION, True, PERFORM_BACKUP_QUEST,
		       True, True, False,
		       ok_backup_callback, toplevel);
    } else {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "backup job");
	return;
    }
}



static void ok_backup_callback(Widget w, XtPointer client_data, 
			       XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client_data=client_data;
    
    switch (call->reason) {
	/* OK Case */
    case XmCR_OK:
	run_backup();
	break;
	
	/* CANCEL Case */
    case XmCR_CANCEL:
	break;
    }
}



static void run_backup()
{
    int i, fd;
    arcenv_t aenv;
    char *cmd[4], cmdstr[4][MAX_FILENAME_LEN];
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    
    waitCursor(toplevel, True, True);
    (void) checkForStop(toplevel);
    (void) checkForStop(toplevel);
    
    for (i = 0; i < 4; i++)
	cmd[i] = &cmdstr[i][0];
    sprintf(cmd[1], "%s/%s", resources.prog_dir, "backup_tape");
    if (strcmp(machine_type, "sun") == 0)
	replacePattern("/hp/", "/sun/", cmd[1], cmd[0]);
    else
	if (strcmp(machine_type, "linux") == 0)
	    replacePattern("/hp/", "/linux/", cmd[1], cmd[0]);
	else
	    strcpy(cmd[0], cmd[1]);
    strcpy(cmd[1], job_data[aenv->job_index].dir);
    sprintf(cmd[2], "IMMEDIATE");
    cmd[3] = NULL;
    /*
     * Launch backup tape program.
     */
    if ((access(cmd[0], X_OK) == -1) ||
	((aenv->pid = launch_process(cmd, &fd)) == -1)) {
	waitCursor(toplevel, False, True);
	displayProblem(WARNING, CANT_START_PROGRAM_PROB,
		       problem_array, toplevel,
		       "backup");
	return;
    }
    input_id = XtAppAddInput(context, fd,
			     (XtPointer)XtInputReadMask, read_from_process, (XtPointer)fd);
    timer_id = XtAppAddTimeOut(context, 500, check_timer, (XtPointer)fd);
}



static void do_restore()
{
    int i, dev_i, fd;
    arcenv_t aenv;
    char *cmd[6], cmdstr[6][MAX_FILENAME_LEN], command[MAX_CMD_LEN];
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    /*
     * Check if a device is selected.
     */
    XtVaGetValues(aenv->devicelist, XmNselectedItemCount, &i, NULL);
    if (aenv->device_row_num == -1 || i <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "device");
	return;
    }
    
    waitCursor(toplevel, True, True);
    (void) checkForStop(toplevel);
    (void) checkForStop(toplevel);
    
    dev_i = aenv->device_row_num - 1;
    sprintf(command, "%s/restore/%s", resources.data_dir, dev_data[dev_i].dir);
    if ((access(command, F_OK) == -1) &&
	(mkdir(command,
	       S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)) {
	waitCursor(toplevel, False, True);
	displayProblem(WARNING, CANT_CREATE_THING_PROB,
		       problem_array, toplevel,
		       "restore directory");
	return;
    }
    
    for (i = 0; i < 6; i++)
	cmd[i] = &cmdstr[i][0];
    sprintf(cmd[1], "%s/%s", resources.prog_dir, "read_header");
    if (strcmp(machine_type, "sun") == 0)
	replacePattern("/hp/", "/sun/", cmd[1], cmd[0]);
    else
	if (strcmp(machine_type, "linux") == 0)
	    replacePattern("/hp/", "/linux/", cmd[1], cmd[0]);
	else
	    strcpy(cmd[0], cmd[1]);
    strcpy(cmd[1], dev_data[dev_i].file);
    sprintf(cmd[2], "%s/%s/%s", resources.data_dir, "restore",
	    dev_data[dev_i].dir);
    if (dev_data[dev_i].is_drive)
	strcpy(cmd[3], "DRIVE");
    else
	strcpy(cmd[3], "FILE");
    strcpy(cmd[4], machine_type);
    cmd[5] = NULL;
    /*
     * Launch read header program.
     */
    if ((access(cmd[0], X_OK) == -1) ||
	((aenv->pid = launch_process(cmd, &fd)) == -1)) {
	waitCursor(toplevel, False, True);
	displayProblem(WARNING, CANT_START_PROGRAM_PROB,
		       problem_array, toplevel,
		       "read header");
	return;
    }
    input_id = XtAppAddInput(context, fd,
			     (XtPointer)XtInputReadMask, read_from_process,
			     (XtPointer)fd);
    timer_id = XtAppAddTimeOut(context, 500, check_timer, (XtPointer)fd);
}



void read_header()
{
    char item[MAX_ITEM_LEN];
    arcenv_t aenv;
    char file[MAX_FILENAME_LEN];
    FILE *fp;
    int dev_i, flag;
    static int restore_shell_status = SHELL_START;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    
    waitCursor(toplevel, True, False);
    switch (restore_shell_status) {
    case SHELL_START:
	(void) checkForStop(toplevel);
	(void) checkForStop(toplevel);
	restore_shell = create_restore_widget(toplevel, aenv);
	restore_shell_status = SHELL_UP;
	
    case SHELL_UP:
	(void) checkForStop(toplevel);
	(void) checkForStop(toplevel);
	dev_i = aenv->device_row_num - 1;
	XmTextFieldSetString(aenv->restore_tb[R_DEVICE_FLD],
			     dev_data[dev_i].name);
	sprintf(file, "%s/%s/%s/%s", resources.data_dir, "restore",
		dev_data[dev_i].dir,
		"header");
	
	if ((fp = fopen(file, "r")) == NULL) {
	    waitCursor(toplevel, False, False);
	    displayProblem(WARNING, BAD_THING_PROB,
			   problem_array, toplevel,
			   "header file");
	    return;
	}
	
	flag = 0;
	if (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
	    stripNewline(item);
	    XmTextFieldSetString(aenv->restore_tb[R_ARCNAME_FLD], item);
	} else
	    flag = 1;
	
	if (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
	    stripNewline(item);
	    XmTextFieldSetString(aenv->restore_tb[R_TIME_FLD], item);
	} else
	    flag = 1;
	
	if (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
	    stripNewline(item);
	    XmTextFieldSetString(aenv->restore_tb[R_DATE_FLD], item);
	} else
	    flag = 1;
	
	if (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
	    stripNewline(item);
	    XmTextFieldSetString(aenv->restore_tb[R_SIZE_FLD], item);
	} else
	    flag = 1;

	fclose(fp);
	if (flag == 1) {
	    waitCursor(toplevel, False, False);
	    displayProblem(WARNING, BAD_THING_PROB,
			   problem_array, toplevel,
			   "header file");
	    return;
	}
	
	XmListDeleteAllItems(aenv->restore_filelist);
	waitCursor(toplevel, False, False);
	set_default_help_info(_HelpContents1,_HelpContents1);
	popupCenterDialog(toplevel, restore_shell,
			  RESTORE_WIDTH, RESTORE_HEIGHT);
	/*
	 * Read in filelist.
	 */
	waitCursor(restore_shell, True, False);
	sprintf(file, "%s/%s/%s/%s", resources.data_dir, "restore",
		dev_data[dev_i].dir, "filelist");
	if ((fp = fopen(file, "r")) != NULL)
	    XtAppAddInput(context, fileno(fp), (XtPointer)XtInputReadMask,
			  read_restore_filelist_in, fp);
	break;
    }

}



static void run_restore()
{
    char *cmd[6], cmdstr[6][MAX_FILENAME_LEN];
    int i, j, count, fd;
    arcenv_t aenv;
    XmStringTable xstr;
    FILE *fp;
    char file[MAX_FILENAME_LEN], line[MAX_FILENAME_LEN], *text;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    XtVaGetValues(aenv->restore_filelist, XmNselectedItemCount, &count,
		  XmNselectedItems, &xstr,
		  NULL);
    if (count <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, restore_shell,
		       "file or directory");
	return;
    }
    
    XtPopdown(restore_shell);
    restore_help_info();
    waitCursor(toplevel, True, True);
    (void) checkForStop(toplevel);
    (void) checkForStop(toplevel);
    
    sprintf(file, "%s/%s/%s/%s", resources.data_dir, "restore",
	    dev_data[aenv->device_row_num - 1].dir, "worklist");
    if ((fp = fopen(file, "w")) != NULL) {
	for (i = 0; i < count; i++) {
	    XmStringGetLtoR(xstr[i], XmSTRING_DEFAULT_CHARSET, &text);
	    strcpy(line, &text[FILELISTING_FILE_POS]);
	    j = strlen(line);
	    while (line[j] == '/' || line[j] == '\0') {
		line[j] = 0;
		j--;
	    }
	    fprintf(fp, "%s\n", line);
	}
		XtFree(text);
		fclose(fp);
    } else {
	waitCursor(toplevel, False, True);
	displayProblem(WARNING, CANT_CREATE_THING_PROB,
		       problem_array, restore_shell,
		       "restore worklist file");
	return;
    }
    
    for (i = 0; i < 6; i++)
	cmd[i] = &cmdstr[i][0];
    sprintf(cmd[1], "%s/%s", resources.prog_dir, "restore_tape");
    if (strcmp(machine_type, "sun") == 0)
	replacePattern("/hp/", "/sun/", cmd[1], cmd[0]);
    else
	if (strcmp(machine_type, "linux") == 0)
	    replacePattern("/hp/", "/linux/", cmd[1], cmd[0]);
	else
	    strcpy(cmd[0], cmd[1]);
    strcpy(cmd[1], dev_data[aenv->device_row_num - 1].file);
    sprintf(cmd[2], file);
    if (dev_data[aenv->device_row_num - 1].is_drive)
	strcpy(cmd[3], "DRIVE");
    else
	strcpy(cmd[3], "FILE");
    strcpy(cmd[4], machine_type);
    cmd[5] = NULL;
    /*
     * Launch restore tape program.
     */
    if ((access(cmd[0], X_OK) == -1) ||
	((aenv->pid = launch_process(cmd, &fd)) == -1)) {
	waitCursor(toplevel, False, True);
	displayProblem(WARNING, CANT_START_PROGRAM_PROB,
		       problem_array, toplevel,
		       "restore");
	return;
    }
    input_id = XtAppAddInput(context, fd,
			     (XtPointer)XtInputReadMask, read_from_process,
			     (XtPointer)fd);
    timer_id = XtAppAddTimeOut(context, 500, check_timer, (XtPointer)fd);
}



static void do_rewind()
{
    int i, dev_i;
    arcenv_t aenv;
    char cmd[MAX_CMD_LEN];
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    /*
     * Check if a device is selected.
     */
    XtVaGetValues(aenv->devicelist, XmNselectedItemCount, &i, NULL);
    if (aenv->device_row_num == -1 || i <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "device");
	return;
    }
    /*
     * Check if device is a drive.
     */
    dev_i = aenv->device_row_num - 1;
    if (!dev_data[dev_i].is_drive) {
	displayProblem(WARNING, DEVICE_CANT_BE_THING_PROB,
		       problem_array, toplevel,
		       "rewound");
	return;
    }
    /*
     * Rewind device.
     */
    waitCursor(toplevel, True, False);
    (void) checkForStop(toplevel);
    (void) checkForStop(toplevel);
    sprintf(cmd, "%s %s %s", MT_PROG, dev_data[dev_i].file, REWIND_CMD);
#ifdef DEBUG
    syslog(LOG_WARNING, "Running command %s.", cmd);
#endif
    if (system(cmd) != 0) {
	waitCursor(toplevel, False, False);
	displayProblem(WARNING, MT_REW_FAILED_PROB,
		       problem_array, toplevel);
	return;
    }
    
    waitCursor(toplevel, False, False);
    popupDialogBox(INFO, True, REWIND_TAPE_OK, True, False, False,
		   NULL, toplevel);
}



static void do_eject()
{
    int i, dev_i;
    arcenv_t aenv;
    char cmd[MAX_CMD_LEN];
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    /*
     * Check if a device is selected.
     */
    XtVaGetValues(aenv->devicelist, XmNselectedItemCount, &i, NULL);
    if (aenv->device_row_num == -1 || i <= 0) {
	displayProblem(WARNING, NO_THING_SELECTED_PROB,
		       problem_array, toplevel,
		       "device");
	return;
    }
    /*
     * Check if device is a drive.
     */
    dev_i = aenv->device_row_num - 1;
    if (!dev_data[dev_i].is_drive) {
	displayProblem(WARNING, DEVICE_CANT_BE_THING_PROB,
		       problem_array, toplevel,
		       "ejected");
	return;
    }
    /*
     * Eject device.
     */
    waitCursor(toplevel, True, False);
    (void) checkForStop(toplevel);
    (void) checkForStop(toplevel);
    sprintf(cmd, "%s %s %s", MT_PROG, dev_data[dev_i].file, EJECT_CMD);
#ifdef DEBUG
    syslog(LOG_WARNING, "Running command %s.", cmd);
#endif
    if (system(cmd) != 0) {
	waitCursor(toplevel, False, False);
	displayProblem(WARNING, MT_EJECT_FAILED_PROB,
		       problem_array, toplevel);
	return;
    }
    
    waitCursor(toplevel, False, False);
    popupDialogBox(INFO, True, EJECT_TAPE_OK, True, False, False,
		   NULL, toplevel);
}



void joblist_findbtn_callback(Widget w, XtPointer client, XtPointer call)
{
    char *text = XmTextFieldGetString(joblist_tb);
    XmString xms;
    int *pos, count, start, row;
    arcenv_t aenv;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    call=call;
    
    if (text == NULL)
	return;
    
    FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
    xms = XmStringCreateLocalized(text);
    count = XiExtdListGetSelectedPos(aenv->joblist, &pos);
    if (count > 0)
	start = pos[count -1] + 1;
    else
	start = 1;
    row = XiExtdListFindItem(aenv->joblist, xms, -1, start, True);
    XmStringFree(xms);
    XtFree(text);
}

