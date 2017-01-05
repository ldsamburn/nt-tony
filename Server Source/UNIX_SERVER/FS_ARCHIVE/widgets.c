
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * widgets.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ArrowB.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/MainW.h>
#include <Xm/CascadeBG.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <X11/StringDefs.h>

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
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
#include "fs_archive_help.h"



/** FUNCTION PROTOTYPES **/

void exit_archive(int);
void create_main_widget(Widget, arcenv_t);
static Widget create_main_bb_widget(Widget, arcenv_t);
static Widget create_main_menubar_widget(Widget, arcenv_t);
static Widget create_devicelist_widget(Widget, arcenv_t);
static Widget create_joblist_widget(Widget, arcenv_t);
Widget create_restore_widget(Widget, arcenv_t);
static Widget create_restore_bb_widget(Widget, arcenv_t);
static Widget create_restore_textgroup_widget(Widget, arcenv_t);
static Widget create_restore_filelist_widget(Widget, arcenv_t);
Widget create_backup_widget(Widget, arcenv_t);
static Widget create_backup_bb_widget(Widget, arcenv_t);
static Widget create_backup_textgroup_widget(Widget, arcenv_t);
static Widget create_backup_filelist_widget(Widget, arcenv_t);
static Widget create_backup_clock_widget(Widget, arcenv_t);
static Widget create_backup_time_widget(Widget, arcenv_t);
static Widget create_backup_enable_widget(Widget, arcenv_t);
static Widget create_backup_days_widget(Widget, arcenv_t);
Widget create_changedev_widget(Widget, int);
Widget create_copyjob_widget(Widget, arcenv_t);
static Widget create_copyjob_bb_widget(Widget, arcenv_t);
static Widget create_copyjob_textgroup_widget(Widget, arcenv_t);


/** STATIC VARIABLES **/

static char *job_titles[] = { "NAME", "TIME", "DAYS", "ENABLED" };

static char *backup_labels[] = { "JOB NAME:", "ARCHIVE NAME:", "DEVICE:",
				 "E-MAIL ADDRESS:", "MESSAGE TYPE:","SIZE (KBYTES):" };

static char *restore_labels[] = { "ARCHIVE NAME:", "DEVICE:","TIME CREATED:", 
				  "DATE CREATED:","SIZE (KBYTES):" };

static unsigned backup_vectors[] = {
	_JobName,
	_ArchiveName,
	_Device,
	_E_MailAddress,
	_MessageType,
	_Size };

static MenuItem system_menu_item[] =
{
	{ "Backup", &xmPushButtonGadgetClass, 'B', NULL, NULL, 0,
	  system_menu_callback, (XtPointer) 0, (MenuItem *) NULL },
	{ "Restore", &xmPushButtonGadgetClass, 'R', NULL, NULL, 0,
	  system_menu_callback, (XtPointer) 1, (MenuItem *) NULL },
	{ "Quit", &xmPushButtonGadgetClass, 'Q', NULL, NULL, 1,
	  system_menu_callback, (XtPointer) 2, (MenuItem *) NULL },
	{ '\0', NULL, '\0', NULL, NULL, 0, NULL, (XtPointer) 0, (MenuItem *) NULL}
};

static MenuItem job_menu_item[] =
{
	{ "Add", &xmPushButtonGadgetClass, 'A', NULL, NULL, 0,
	  job_menu_callback, (XtPointer) 0, (MenuItem *) NULL },
	{ "Edit", &xmPushButtonGadgetClass, 'E', NULL, NULL, 0,
	  job_menu_callback, (XtPointer) 1, (MenuItem *) NULL },
	{ "Copy", &xmPushButtonGadgetClass, 'C', NULL, NULL, 0,
	  job_menu_callback, (XtPointer) 2, (MenuItem *) NULL },
	{ "Delete", &xmPushButtonGadgetClass, 'D', NULL, NULL, 0,
	  job_menu_callback, (XtPointer) 3, (MenuItem *) NULL },
	{ "Change Device", &xmPushButtonGadgetClass, 'h', NULL, NULL, 0,
	  job_menu_callback, (XtPointer) 4, (MenuItem *) NULL },
	{ '\0', NULL, '\0', NULL, NULL, 0, NULL, (XtPointer) 0, (MenuItem *) NULL}

};

static MenuItem device_menu_item[] =
{
	{ "Rewind", &xmPushButtonGadgetClass, 'R', NULL, NULL, 0,
	  device_menu_callback, (XtPointer) 0, (MenuItem *) NULL },
	{ "Eject", &xmPushButtonGadgetClass, 'E', NULL, NULL, 0,
	  device_menu_callback, (XtPointer) 1, (MenuItem *) NULL },
	{ '\0', NULL, '\0', NULL, NULL, 0, NULL, (XtPointer) 0, (MenuItem *) NULL}

};



/** FUNCTION DEFINITIONS **/

void exit_archive(int value)
{
    XtDestroyWidget(toplevel);
    syslog(LOG_WARNING, "Utility terminated.");
    /*	free_help(); Called from exitApp*/
    exitApp(value);
}



void create_main_widget(Widget parent, arcenv_t aenv)
{
    Widget mainwindow, devicelist, joblist, buttonbox;
    Widget container, menubar;
    
    aenv->show_size = 0;
    
    mainwindow = XtVaCreateManagedWidget("main_window",xmMainWindowWidgetClass, 
					 parent,NULL);
    
    menubar = create_main_menubar_widget(mainwindow, aenv);
    
    container = XtVaCreateManagedWidget("main_window_form",xmFormWidgetClass, 
					mainwindow,XmNfractionBase, 10,NULL);
    
    buttonbox = create_main_bb_widget(container, aenv);
    XtVaSetValues(buttonbox, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    devicelist = create_devicelist_widget(container, aenv);
    XtVaSetValues(devicelist, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment,XmATTACH_POSITION,
		  XmNrightPosition, 4,
		  NULL);
    
    joblist = create_joblist_widget(container, aenv);
    XtVaSetValues(joblist, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, devicelist,
		  XmNleftOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox,
		  XmNbottomOffset, OFFSET,
		  NULL);
    
    SET_HELP_CALLBACK(mainwindow);
}


static Widget create_main_bb_widget(Widget parent, arcenv_t aenv)
{
    Widget frame;
    
    frame = XtVaCreateManagedWidget("main_bb_frame",xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,NULL);
    
    aenv->main_bb = XiCreateButtonBox(frame, "main_bb", 
				      (XtCallbackProc)main_bb_callback, NULL,
				      "Backup", " Restore ", "Quit", NULL);
    
    return(frame);
}



static Widget create_main_menubar_widget(Widget parent, arcenv_t aenv)
{
    Widget helppull;
    
    aenv->menubar = XmCreateMenuBar(parent, "main_menubar", NULL, 0);
    
    (void) BuildPulldownMenu(aenv->menubar, "System", 'S', system_menu_item);
    (void) BuildPulldownMenu(aenv->menubar, "Backup Job", 'B', job_menu_item);
    (void) BuildPulldownMenu(aenv->menubar, "Device", 'D', device_menu_item);
    helppull = build_help_menu(aenv->menubar);
    
    XtVaSetValues(aenv->menubar, XmNmenuHelpWidget, helppull, NULL);
    XtManageChild(aenv->menubar);
    
    return(aenv->menubar);
}



static Widget create_devicelist_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, label, container;
    Arg arg[15];
    XmString s;
    int j;
    
    aenv->device_row_num = -1;
    
    frame = XtVaCreateManagedWidget("devicelist_frame",xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,NULL);
    
    container = XtVaCreateManagedWidget("devicelist_container",xmFormWidgetClass,
					frame,NULL);
    
    s = XmStringCreate(DEVICE_LIST_TITLE, XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("devicelist_label",xmLabelWidgetClass, container,
				    XmNlabelString, s,
				    XmNalignment, XmALIGNMENT_BEGINNING,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,	NULL);
    XmStringFree(s);
    
    j = 0;
    XtSetArg(arg[j], XmNselectionPolicy, XmSINGLE_SELECT); j++;
    XtSetArg(arg[j], XmNlistSizePolicy, XmCONSTANT); j++;
    XtSetArg(arg[j], XmNrightAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNrightOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNleftAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNleftOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNtopAttachment, XmATTACH_WIDGET); j++;
    XtSetArg(arg[j], XmNtopWidget, label); j++;
    XtSetArg(arg[j], XmNtopOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNbottomAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNbottomOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNscrollingPolicy, XmAUTOMATIC); j++;
    aenv->devicelist = XmCreateScrolledList(container,"devicelist_list", arg, j);
    XtManageChild(aenv->devicelist);
    XtAddCallback(aenv->devicelist, XmNsingleSelectionCallback,
		  (XtCallbackProc)devicelist_sel_callback, NULL);
    
    SET_SCROLLED_LIST_HELP(aenv->devicelist,_FileSystemBackup_RestoreOverview,
			   _DeviceList);
    
    return(frame);
}



static Widget create_joblist_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, container, label, findbtn, form;
    Widget list;
    int i;
    XmString *header, s;
    Dimension lheight, pheight;
    
    frame = XtVaCreateManagedWidget("joblist_frame",xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,NULL);
    
    container = XtVaCreateManagedWidget("joblist_container",xmFormWidgetClass, frame,
					NULL);
    
    s = XmStringCreate(JOBLIST_TITLE, XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("fsb_filelist_label",xmLabelWidgetClass, 
				    container,XmNlabelString, s,
				    XmNalignment, XmALIGNMENT_BEGINNING,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,NULL);
    XmStringFree(s);
    
    /*header = (XmString *) XtMalloc(sizeof(XmString)*NUM_JOB_TITLES);*/
    header = (XmString *) malloc(sizeof(XmString)*NUM_JOB_TITLES);
    for (i = 0; i < NUM_JOB_TITLES; i++)
	header[i] = XmStringCreateLocalized(job_titles[i]);
    
    aenv->joblist = XtVaCreateManagedWidget("joblist_list",
					    xiExtdListWidgetClass, container,
					    XiNitemType, 
					    XiEXTLIST_ITEM_TYPE_NON_STRUCTURED,
					    XiNcolumnCount, XtNumber(job_titles),
					    XiNrowCount, 1,
					    XiNitems, header,
					    XiNselectionType, 
					    XiEXTLIST_SINGLE_SELECT,
					    XmNtopAttachment, XmATTACH_WIDGET,
					    XmNtopWidget, label,
					    XmNtopOffset, OFFSET,
					    XmNrightAttachment, XmATTACH_FORM,
					    XmNrightOffset, OFFSET_RT_EXT,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNleftOffset, OFFSET, NULL);
    
    SET_EXTENDED_LIST_HELP(aenv->joblist,list,
			   _FileSystemBackup_RestoreOverview,_ICS_BackupJobList);
    
    for (i = 0; (unsigned int)i < XtNumber(job_titles); i++)
	XmStringFree(header[i]);
    XtFree((char*)header);
    
    form = XtVaCreateWidget("form",	xmFormWidgetClass, container,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, OFFSET,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNleftOffset, OFFSET,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, OFFSET, NULL);
    
    findbtn = XtVaCreateManagedWidget("Find",xmPushButtonWidgetClass, form,
				      XmNleftAttachment, XmATTACH_FORM,NULL);
    
    XtAddCallback(findbtn, XmNactivateCallback,joblist_findbtn_callback, NULL);
    
    joblist_tb = XtVaCreateManagedWidget("textfield",xmTextFieldWidgetClass, form,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNleftWidget, findbtn,
					 XmNleftOffset, OFFSET, NULL);
    /*
     * Size the push button.
     */
    XtVaGetValues(joblist_tb, XmNheight, &lheight, NULL);
    XtVaGetValues(findbtn, XmNheight, &pheight, NULL);
    XtVaSetValues(findbtn, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, (lheight - pheight)/2,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, (lheight - pheight)/2,NULL);
    XtManageChild(form);
    
    XtVaSetValues(aenv->joblist, XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, form,
		  XmNbottomOffset, OFFSET_BT_EXT,
		  NULL);
    
    return(frame);
}



Widget create_restore_widget(Widget parent, arcenv_t aenv)
{
    Widget container, filelist, buttonbox, textgroup, shell;
    Atom dw;
    
    shell = XtVaCreateWidget("restore_shell",
#ifdef VSUN
			     transientShellWidgetClass, parent,
			     XmNwidth, RESTORE_WIDTH,
			     XmNheight, RESTORE_HEIGHT,
#else
			     xmDialogShellWidgetClass, parent,
#endif
			     XtNmappedWhenManaged, False,
			     XmNdeleteResponse, XmDO_NOTHING,
			     XmNtitle, "File System Restore",
			     NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(shell, dw, (XtCallbackProc)wm_close_callback, NULL);
    
    container = XtVaCreateWidget("restore_container",
				 xmFormWidgetClass, shell,
#ifndef VSUN
				 XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
#endif
				 NULL);
    
    buttonbox = create_restore_bb_widget(container, aenv);
    XtVaSetValues(buttonbox, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    textgroup = create_restore_textgroup_widget(container, aenv);
    XtVaSetValues(textgroup, XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  NULL);
    
    filelist = create_restore_filelist_widget(container, aenv);
    XtVaSetValues(filelist, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, textgroup,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    (void) XtVaCreateManagedWidget("empty_frame",
				   xmFrameWidgetClass, container,
				   XmNshadowType, XmSHADOW_ETCHED_IN,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNrightOffset, OFFSET,
				   XmNbottomAttachment, XmATTACH_WIDGET,
				   XmNbottomWidget, buttonbox,
				   XmNbottomOffset, OFFSET,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, textgroup,
				   XmNtopOffset, OFFSET,
				   XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
				   XmNleftWidget, textgroup,
				   NULL);
    
    XtManageChild(container);
    
    return(shell);
}



static Widget create_restore_bb_widget(Widget parent, arcenv_t aenv)
{
    Widget frame;
    
    frame = XtVaCreateManagedWidget("restore_bb_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    aenv->restore_bb = XiCreateButtonBox(frame, "restore_bb",
					 (XtCallbackProc)restore_bb_callback,
					 NULL,
					 "OK", " Cancel ",
					 NULL);
    
    return(frame);
}



static Widget create_restore_textgroup_widget(Widget parent, arcenv_t aenv)
{
    int i;
    Widget frame, colform;
    Widget form = 0;
    
    frame = XtVaCreateManagedWidget("restore_textgroup_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    colform = XtVaCreateWidget("restore_textgroup_rowcol",
			       xmFormWidgetClass, frame,NULL);
    
    for (i = 0; i < NUM_RESTORE_LABELS; i++) {
	if (i == 0) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,NULL);
	} else if (i == (NUM_RESTORE_LABELS - 1)) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNbottomOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,NULL);
	} else {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,NULL);
	}
	(void) XtVaCreateManagedWidget(restore_labels[i],
				       xmLabelGadgetClass, form,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNrightAttachment, XmATTACH_POSITION,
				       XmNrightPosition, TEXT_LABEL_END,
				       XmNalignment, XmALIGNMENT_END,NULL);
	switch (i) {
	    /* ARCHIVE NAME, DEVICE, TIME CREATED,
	       DATE CREATED, AND SIZE Cases */
	case R_ARCNAME_FLD:
	case R_DEVICE_FLD:
	case R_TIME_FLD:
	case R_DATE_FLD:
	case R_SIZE_FLD:
	    aenv->restore_tb[i] = 
		XtVaCreateManagedWidget("textfield",
					xmTextFieldWidgetClass, form,
					XmNtraversalOn, True,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_POSITION,
					XmNleftPosition, TEXT_FIELD_START,
					XmNeditable, False,
					XmNcursorPositionVisible, False,
					NULL);
	    XtAddCallback(aenv->restore_tb[i], XmNactivateCallback,
			  (XtCallbackProc)XmProcessTraversal,
			  (XtPointer)XmTRAVERSE_NEXT_TAB_GROUP);
	    break;
	}
	XtManageChild(form);
    }
    XtManageChild(colform);
    
    return(frame);
}



static Widget create_restore_filelist_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, label, container, buttonbox_frame;
    Arg arg[15];
    XmString s;
    int j;
    
    frame = XtVaCreateManagedWidget("restore_filelist_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,NULL);
    
    container = XtVaCreateManagedWidget("restore_filelist_container",
					xmFormWidgetClass, frame,NULL);
    
    s = XmStringCreate(RESTORE_FILELIST_TITLE, XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("restore_filelist_label",
				    xmLabelWidgetClass, container,
				    XmNlabelString, s,
				    XmNalignment, XmALIGNMENT_BEGINNING,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,NULL);
    XmStringFree(s);
    
    j = 0;
    XtSetArg(arg[j], XmNselectionPolicy, XmMULTIPLE_SELECT); j++;
    XtSetArg(arg[j], XmNlistSizePolicy, XmCONSTANT); j++;
    XtSetArg(arg[j], XmNrightAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNrightOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNleftAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNleftOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNtopAttachment, XmATTACH_WIDGET); j++;
    XtSetArg(arg[j], XmNtopWidget, label); j++;
    XtSetArg(arg[j], XmNtopOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNscrollingPolicy, XmAUTOMATIC); j++;
    aenv->restore_filelist = XmCreateScrolledList(container,
						  "restore_filelist_list", arg, j);
    XtManageChild(aenv->restore_filelist);
    XtAddCallback(aenv->restore_filelist, XmNdefaultActionCallback,
		  (XtCallbackProc)restore_filelist_sel_callback, NULL);
    
    buttonbox_frame = XtVaCreateManagedWidget("restore_filelist_bb_frame",
					      xmFrameWidgetClass, container,
					      XmNshadowType, XmSHADOW_ETCHED_IN,
					      NULL);
    
    aenv->restore_filelist_bb = XiCreateButtonBox(buttonbox_frame,
						  "restore_filelist_bb",
						  (XtCallbackProc)restore_filelist_bb_callback,
						  NULL,"  Select All  ", NULL);
    
    XtVaSetValues(XtParent(aenv->restore_filelist),
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox_frame,
		  XmNbottomOffset, OFFSET,
		  NULL);
    
    XtVaSetValues(buttonbox_frame, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    return(frame);
}



Widget create_backup_widget(Widget parent, arcenv_t aenv)
{
    Widget container, days, filelist, buttonbox, textgroup;
    Widget enable, time_, shell;
    Atom dw;
    
    shell = XtVaCreateWidget("backup_shell",
#ifdef VSUN
			     transientShellWidgetClass, parent,
			     XmNwidth, BACKUP_WIDTH,
			     XmNheight, BACKUP_HEIGHT,
#else
			     xmDialogShellWidgetClass, parent,
#endif
			     XtNmappedWhenManaged, False,
			     XmNdeleteResponse, XmDO_NOTHING,
			     NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(shell, dw, (XtCallbackProc)wm_close_callback, NULL);
    
    container = XtVaCreateWidget("backup_container",
				 xmFormWidgetClass, shell,
#ifndef VSUN
				 XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
#endif
				 NULL);
    
    buttonbox = create_backup_bb_widget(container, aenv);
    XtVaSetValues(buttonbox, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    time_ = create_backup_time_widget(container, aenv);
    XtVaSetValues(time_, XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  NULL);
    
    days = create_backup_days_widget(container, aenv);
    XtVaSetValues(days, XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, time_,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  NULL);
    
    enable = create_backup_enable_widget(container, aenv);
    XtVaSetValues(enable, XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, time_,
		  XmNtopOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNleftWidget, time_,
		  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget, days,
		  NULL);
    
    textgroup = create_backup_textgroup_widget(container, aenv);
    XtVaSetValues(textgroup, XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, enable,
		  XmNtopOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNleftWidget, days,
		  NULL);
    
    filelist = create_backup_filelist_widget(container, aenv);
    XtVaSetValues(filelist, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, days,
		  XmNrightOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox,
		      XmNbottomOffset, OFFSET,
		  NULL);
    
    (void) XtVaCreateManagedWidget("empty_frame",
				   xmFrameWidgetClass, container,
				   XmNshadowType, XmSHADOW_ETCHED_IN,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNrightOffset, OFFSET,
				   XmNbottomAttachment, XmATTACH_WIDGET,
				   XmNbottomWidget, buttonbox,
				   XmNbottomOffset, OFFSET,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, textgroup,
				   XmNtopOffset, OFFSET,
				   XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
				   XmNleftWidget, textgroup,
				   NULL);
    
    XtManageChild(container);
    
    SET_HELP_CALLBACK(container);
    
    return(shell);
}



static Widget create_backup_bb_widget(Widget parent, arcenv_t aenv)
{
    Widget frame;
    
    frame = XtVaCreateManagedWidget("backup_bb_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    aenv->backup_bb = XiCreateButtonBox(frame, "backup_bb",
					(XtCallbackProc)backup_bb_callback, NULL,
					"OK", "Clear", " Cancel ",
					NULL);
    
    return(frame);
}



static Widget create_backup_textgroup_widget(Widget parent, arcenv_t aenv)
{
    int i, j;
    Widget frame, colform;
    Widget form = 0;
    XmString *boxitem;
    
    frame = XtVaCreateManagedWidget("backup_textgroup_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    colform = XtVaCreateWidget("backup_textgroup_rowcol",
			       xmFormWidgetClass, frame,
			       NULL);
    
    for (i = 0; i < NUM_BACKUP_LABELS; i++) {
	if (i == 0) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	} else if (i == (NUM_BACKUP_LABELS - 1)) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNbottomOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	} else {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	}
	(void) XtVaCreateManagedWidget(backup_labels[i],
				       xmLabelGadgetClass, form,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNrightAttachment, XmATTACH_POSITION,
				       XmNrightPosition, TEXT_LABEL_END,
				       XmNalignment, XmALIGNMENT_END,
				       NULL);
	switch (i) {
	    /* JOB NAME, ARCHIVE NAME, AND E-MAIL ADDRESS Cases */
	case B_JOBNAME_FLD:
	case B_ARCNAME_FLD:
	case B_EMAIL_FLD:
	    aenv->backup_tb[i] = 
		XtVaCreateManagedWidget("textfield",
					xmTextFieldWidgetClass, form,
					XmNtraversalOn, True,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_POSITION,
					XmNleftPosition, TEXT_FIELD_START,
					XmNeditable, True,
					XmNcursorPositionVisible,
					True,NULL);
	    XtAddCallback(aenv->backup_tb[i], XmNactivateCallback,
			  (XtCallbackProc)XmProcessTraversal,
			  (XtPointer)XmTRAVERSE_NEXT_TAB_GROUP);
	    
	    SET_TEXT_FIELD_HELP_WITH_RUNTIME_WINDOW(aenv->backup_tb[i], 
						    backup_vectors[i]);
	    break;
	    
	    /* DEVICE AND SIZE Cases */
	case B_DEVICE_FLD:
	case B_SIZE_FLD:
	    aenv->backup_tb[i] = 
		XtVaCreateManagedWidget("textfield",
					xmTextFieldWidgetClass, form,
					XmNtraversalOn, True,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_POSITION,
					XmNleftPosition, TEXT_FIELD_START,
					XmNeditable, False,
					XmNcursorPositionVisible,
					False,NULL);
	    XtAddCallback(aenv->backup_tb[i], XmNactivateCallback,
			  (XtCallbackProc)XmProcessTraversal,
			  (XtPointer)XmTRAVERSE_NEXT_TAB_GROUP);
	    
	    SET_TEXT_FIELD_HELP_WITH_RUNTIME_WINDOW(aenv->backup_tb[i], 
						    backup_vectors[i]);
	    break;
	    
	    /* MESSAGE TYPE Case */
	case B_MSGTYPE_FLD:
	/* boxitem = (XmString *) XtMalloc(sizeof(XmString)*NUM_MSG_TYPES); */
	    boxitem =(XmString *) malloc(sizeof(XmString)*NUM_MSG_TYPES);
	    for (j = 0; j < NUM_MSG_TYPES; j++)
		boxitem[j] = 
		    XmStringCreateLocalized(get_msg_type(j));
	    aenv->backup_tb[i] = 
		XtVaCreateManagedWidget(backup_labels[i],
					xiComboBoxWidgetClass, form,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_POSITION,
					XmNleftPosition, TEXT_FIELD_START,
					XiNeditable, False,
					XiNvisibleCount, NUM_MSG_TYPES,
					XiNitemCount, NUM_MSG_TYPES,
					XiNitems, boxitem,NULL);
	    XtAddCallback(aenv->backup_tb[i], XiNselectCallback,
			  (XtCallbackProc)msgtype_sel_callback,
			  NULL);
	    for (j = 0; j < NUM_MSG_TYPES; j++)
		XmStringFree(boxitem[j]);
	    XtFree((char*)boxitem);
	    break;
	    
	}
	XtManageChild(form);
    }
    XtManageChild(colform);
    
    return(frame);
}



static Widget create_backup_filelist_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, label, container, buttonbox_frame;
    Arg arg[15];
    XmString s;
    int j;
    
    frame = XtVaCreateManagedWidget("backup_filelist_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    container = XtVaCreateManagedWidget("backup_filelist_container",
					xmFormWidgetClass, frame,
					NULL);
    
    SET_HELP_CALLBACK(container);
    
    s = XmStringCreate(BACKUP_FILELIST_TITLE, XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("backup_filelist_label",
				    xmLabelWidgetClass, container,
				    XmNlabelString, s,
				    XmNalignment, XmALIGNMENT_BEGINNING,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    NULL);
    XmStringFree(s);
    
    j = 0;
    XtSetArg(arg[j], XmNselectionPolicy, XmMULTIPLE_SELECT); j++;
    XtSetArg(arg[j], XmNlistSizePolicy, XmCONSTANT); j++;
    XtSetArg(arg[j], XmNrightAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNrightOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNleftAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNleftOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNtopAttachment, XmATTACH_WIDGET); j++;
    XtSetArg(arg[j], XmNtopWidget, label); j++;
    XtSetArg(arg[j], XmNtopOffset, OFFSET); j++;
    XtSetArg(arg[j], XmNscrollingPolicy, XmAUTOMATIC); j++;
    aenv->backup_filelist = XmCreateScrolledList(container,
						 "backup_filelist_list", arg, j);
    XtManageChild(aenv->backup_filelist);
    
    SET_SCROLLED_LIST_HELP_WITH_RUNTIME_WINDOW(aenv->backup_filelist,
					       _BackupFileList);
    
    buttonbox_frame = XtVaCreateManagedWidget("backup_filelist_bb_frame",
					      xmFrameWidgetClass, container,
					      XmNshadowType, XmSHADOW_ETCHED_IN,
					      NULL);
    
    aenv->backup_filelist_bb = XiCreateButtonBox(buttonbox_frame,
						 "backup_filelist_bb",
						 (XtCallbackProc)backup_filelist_bb_callback, 
						 NULL,
						 "Add", "Delete", " Move Up ",
						 "Refresh", NULL);
    
    XtVaSetValues(XtParent(aenv->backup_filelist),
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox_frame,
		  XmNbottomOffset, OFFSET,
		  NULL);
    
    XtVaSetValues(buttonbox_frame, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    return(frame);
}



static Widget create_backup_clock_widget(Widget parent, arcenv_t aenv)
{
    Widget row;
    XmString s;
    Dimension height;
    
    row = XtVaCreateManagedWidget("backup_clock_row",
				  xmRowColumnWidgetClass, parent,
				  XmNpacking, XmPACK_TIGHT,
				  XmNorientation, XmHORIZONTAL,
				  NULL);
    
    aenv->hours = XtVaCreateManagedWidget("backup_clock_hours",
					  xmTextWidgetClass, row,
					  XmNcolumns, 2,
					  XmNmaxLength, 2,
					  XmNeditable, False,
					  XmNcursorPositionVisible, False,
					  NULL);
    
    s = XmStringCreate(":", XmSTRING_DEFAULT_CHARSET);
    (void) XtVaCreateManagedWidget("backup_clock_colon",
				   xmLabelWidgetClass, row,
				   XmNlabelString, s,
				   XmNalignment, XmALIGNMENT_CENTER,
				   NULL);
    XmStringFree(s);
    
    aenv->minutes = XtVaCreateManagedWidget("backup_clock_minutes",
					    xmTextWidgetClass, row,
					    XmNcolumns, 2,
					    XmNmaxLength, 2,
					    XmNeditable, False,
					    XmNcursorPositionVisible, False,
					    NULL);
    /*
     * Size the manager widget's height prematurely because we
     * need it before realization.
     */
    XtVaGetValues(aenv->hours, XmNheight, &height, NULL);
    XtVaSetValues(row, XmNheight, height, NULL);
    
    return(row);
}



static Widget create_backup_time_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, clock_, botlabel, toplabel, up_hour, down_hour;
    Widget up_minute, down_minute, container;
    XmString s;
    Dimension aheight, cheight;
    
    frame = XtVaCreateManagedWidget("backup_time_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    container = XtVaCreateManagedWidget("backup_time_container",
					xmFormWidgetClass, frame,
					NULL);
    
    s = XmStringCreate("BACKUP TIME", XmSTRING_DEFAULT_CHARSET);
    toplabel = XtVaCreateManagedWidget("backup_time_toplabel",
				       xmLabelWidgetClass, container,
				       XmNlabelString, s,
				       XmNalignment, XmALIGNMENT_CENTER,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNtopOffset, OFFSET,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNleftOffset, OFFSET,
				       XmNrightAttachment, XmATTACH_FORM,
				       XmNrightOffset, OFFSET,
				       NULL);
    XmStringFree(s);
    
    s = XmStringCreate(" ", XmSTRING_DEFAULT_CHARSET);
    botlabel = XtVaCreateManagedWidget("backup_time_botlabel",
				       xmLabelWidgetClass, container,
				       XmNlabelString, s,
				       XmNalignment, XmALIGNMENT_CENTER,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNrightAttachment, XmATTACH_FORM,
				       NULL);
    XmStringFree(s);
    
    up_hour = XtVaCreateManagedWidget("backup_time_up_hour",
				      xmArrowButtonWidgetClass, container,
				      XmNarrowDirection, XmARROW_UP,
				      XmNleftAttachment, XmATTACH_FORM,
				      XmNleftOffset, OFFSET,
				      XmNtopAttachment, XmATTACH_WIDGET,
				      XmNtopWidget, toplabel,
				      NULL);
    XtAddCallback(up_hour, XmNarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) HOUR_ARROW_UP);
    XtAddCallback(up_hour, XmNdisarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) HOUR_ARROW_UP);
    
    down_hour = XtVaCreateManagedWidget("backup_time_down_hour",
					xmArrowButtonWidgetClass, container,
					XmNarrowDirection, XmARROW_DOWN,
					XmNleftAttachment, XmATTACH_FORM,
					XmNleftOffset, OFFSET,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, up_hour,
					XmNtopOffset, 4,
					XmNbottomAttachment, XmATTACH_WIDGET,
					XmNbottomWidget, botlabel,
					NULL);
    XtAddCallback(down_hour, XmNarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) HOUR_ARROW_DOWN);
    XtAddCallback(down_hour, XmNdisarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) HOUR_ARROW_DOWN);
    
    up_minute = XtVaCreateManagedWidget("backup_time_up_minute",
					xmArrowButtonWidgetClass, container,
					XmNarrowDirection, XmARROW_UP,
					XmNrightAttachment, XmATTACH_FORM,
					XmNrightOffset, OFFSET,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, toplabel,
					NULL);
    XtAddCallback(up_minute, XmNarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) MINUTE_ARROW_UP);
    XtAddCallback(up_minute, XmNdisarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) MINUTE_ARROW_UP);
    
    down_minute = XtVaCreateManagedWidget("backup_time_down_minute",
					  xmArrowButtonWidgetClass, container,
					  XmNarrowDirection, XmARROW_DOWN,
					  XmNrightAttachment, XmATTACH_FORM,
					  XmNrightOffset, OFFSET,
					  XmNtopAttachment, XmATTACH_WIDGET,
					  XmNtopWidget, up_minute,
					  XmNtopOffset, 4,
					  XmNbottomAttachment, XmATTACH_WIDGET,
					  XmNbottomWidget, botlabel,
					  NULL);
    XtAddCallback(down_minute, XmNarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) MINUTE_ARROW_DOWN);
    XtAddCallback(down_minute, XmNdisarmCallback, (XtCallbackProc)arrow_activate,
		  (XtPointer) MINUTE_ARROW_DOWN);
    /*
     * After creating the clock, center it relative to the arrow buttons.
     */
    clock_ = create_backup_clock_widget(container, aenv);
    XtVaGetValues(up_hour, XmNheight, &aheight, NULL);
    aheight = 2 * aheight + 4;
    XtVaGetValues(clock_, XmNheight, &cheight, NULL);
    XtVaSetValues(clock_, XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, up_hour,
		  XmNleftOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, up_minute,
		  XmNrightOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget, up_hour,
		  XmNtopOffset, (aheight - cheight) / 2,
		  NULL);
    
    return(frame);
}



static Widget create_backup_enable_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, form;
    Arg arg[13];
    int j;
    
    frame = XtVaCreateManagedWidget("backup_enable_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    form = form = XtVaCreateManagedWidget("backup_enable_form",
					  xmFormWidgetClass, frame,
					  NULL);
    
    j = 0;
    XtSetArg(arg[j], XmNuserData, (XtPointer) aenv->is_enabled); j++;
    XtSetArg(arg[j], XmNalignment, XmALIGNMENT_BEGINNING); j++;
    XtSetArg(arg[j], XmNrightAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNleftAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNleftOffset, OFFSET_LARGE); j++;
    XtSetArg(arg[j], XmNtopAttachment, XmATTACH_FORM); j++;
    XtSetArg(arg[j], XmNbottomAttachment, XmATTACH_FORM); j++;
    if (aenv->is_enabled) {
	XtSetArg(arg[j], XmNset, True); j++;
	aenv->enable_toggle = XmCreateToggleButton(form, "Job Enabled ", arg, j);
    } else {
	XtSetArg(arg[j], XmNset, False); j++;
	aenv->enable_toggle = XmCreateToggleButton(form, "Job Disabled", arg, j);
	}
    XtManageChild(aenv->enable_toggle);
    XtAddCallback(aenv->enable_toggle,
		  XmNvalueChangedCallback, (XtCallbackProc)backup_enable_callback,
		  NULL);
    
    return(frame);
}



static Widget create_backup_days_widget(Widget parent, arcenv_t aenv)
{
    XmString s;
    Widget frame, rowcol, box, label, form;
    int i;
    static char *days_of_week[] = { "Sunday", "Monday", "Tuesday",
				    "Wednesday", "Thursday", "Friday",
				    "Saturday" };
    
    frame = XtVaCreateManagedWidget("backup_days_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    form = XtVaCreateManagedWidget("backup_days_form",
				   xmFormWidgetClass, frame,
				   NULL);
    
    s = XmStringCreate("BACKUP DAYS", XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("backup_days_label",
				    xmLabelWidgetClass, form,
				    XmNlabelString, s,
				    XmNalignment, XmALIGNMENT_CENTER,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    NULL);
    XmStringFree(s);
    
    rowcol = XtVaCreateManagedWidget("backup_days_rowcol",
				     xmRowColumnWidgetClass, form,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNrightAttachment, XmATTACH_FORM,
				     XmNbottomAttachment, XmATTACH_FORM,
				     XmNtopAttachment, XmATTACH_WIDGET,
				     XmNtopWidget, label,
				     NULL);
    
    box = XtVaCreateManagedWidget("backup_days_box",
				  xmRowColumnWidgetClass, rowcol,
				  XmNpacking, XmPACK_COLUMN,
				  XmNnumColumns, 2,
				  NULL);
    
    for (i = 0; i < NUM_DAYS; i++) {
	aenv->days_toggle[i] = 
	    XtVaCreateManagedWidget(days_of_week[i],
				    xmToggleButtonGadgetClass, box,
				    XmNset,((aenv->daystr[i] == '1')?True:False),
				    NULL);
	XtAddCallback(aenv->days_toggle[i],
		      XmNvalueChangedCallback, 
		      (XtCallbackProc)backup_days_callback, (XtPointer)i);
    }
    
    return(frame);
}



Widget create_changedev_widget(Widget parent, int device_row)
{
    Widget dialog;
    XmString dev_xstr[MAX_NUM_DEVICES], s;
    Arg arg[10];
    int i, n;
    
    n = 0;
    for (i = 0; i < num_devices; i++) {
	if (i == device_row)
	    continue;
	dev_xstr[n] = XmStringLtoRCreate(dev_data[i].name,
					 XmSTRING_DEFAULT_CHARSET);
	n++;
    }
    s = XmStringCreateSimple("Cancel");
    
    i = 0;
    XtSetArg(arg[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
    XtSetArg(arg[i], XmNlistItems, dev_xstr); i++;
    XtSetArg(arg[i], XmNlistItemCount, n); i++;
    XtSetArg(arg[i], XmNautoUnmanage, False); i++;
    XtSetArg(arg[i], XmNmustMatch, True); i++;
    XtSetArg(arg[i], XmNcancelLabelString, s); i++;
    XtSetArg(arg[i], XmNtextColumns, DEVNAME_LEN); i++;
    dialog = XmCreateSelectionDialog(parent, "changedev_dialog", arg, i);
    
    XtVaSetValues(XtParent(dialog), XmNtitle, CHANGEDEV_TITLE, NULL);
    XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)changedev_bb_callback,NULL);
    XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc)changedev_bb_callback,
		  NULL);
    XtAddCallback(dialog, XmNnoMatchCallback, (XtCallbackProc)changedev_bb_callback,
		  NULL);
    XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));
    
    SET_SCROLLED_LIST_HELP(XmSelectionBoxGetChild(dialog,XmDIALOG_LIST),
			   _SelectNewDevice_WindowDescription,_Items_NewDevice);
    SET_TEXT_FIELD_HELP(XmSelectionBoxGetChild(dialog,XmDIALOG_TEXT),
			_SelectNewDevice_WindowDescription,_Selection_NewDevice);
    SET_HELP_CALLBACK(dialog);
    set_default_help_info(_SelectNewDevice_WindowDescription,_Selection_NewDevice);
    
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabExclusive);
    
    XmStringFree(s);
    for (i = 0; i < n; i++)
	XmStringFree(dev_xstr[i]);
    
    return(dialog);
}



Widget create_copyjob_widget(Widget parent, arcenv_t aenv)
{
    Widget container, textgroup, buttonbox;
    Widget dialog;
    Atom dw;
    
    dialog = XtVaCreatePopupShell("copyjob_dialog",
#ifdef VSUN
				  transientShellWidgetClass, parent,
				  XmNwidth, COPYJOB_WIDTH,
				  XmNheight, COPYJOB_HEIGHT,
#else
				  xmDialogShellWidgetClass, parent,
#endif
				  XtNmappedWhenManaged, False,
				  XmNtitle, COPYJOB_TITLE,
				  XmNdeleteResponse, XmDO_NOTHING,
				  NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(dialog, dw, (XtCallbackProc)wm_close_callback, NULL);
    
    container = XtVaCreateWidget("copyjob_container",
				 xmFormWidgetClass, dialog,
#ifndef VSUN
				 XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
#endif
				 NULL);
    
    buttonbox = create_copyjob_bb_widget(container, aenv);
    XtVaSetValues(buttonbox, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    textgroup = create_copyjob_textgroup_widget(container, aenv);
    XtVaSetValues(textgroup, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    (void) XtVaCreateManagedWidget("empty_frame",
				   xmFrameWidgetClass, container,
				   XmNshadowType, XmSHADOW_ETCHED_IN,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNrightOffset, OFFSET,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNleftOffset, OFFSET,
				   XmNbottomAttachment, XmATTACH_WIDGET,
				   XmNbottomWidget, buttonbox,
				   XmNbottomOffset, OFFSET,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, textgroup,
				   XmNtopOffset, OFFSET,
				   NULL);
    
    SET_HELP_CALLBACK(container);
    
    XtManageChild(container);
    
    return(dialog);
}



static Widget create_copyjob_bb_widget(Widget parent, arcenv_t aenv)
{
    Widget frame, buttonbox;
    
    /* done only to get rid of compiler warning about unused parameters */
    aenv=aenv;
    
    frame = XtVaCreateManagedWidget("copyjob_bb_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    buttonbox = XiCreateButtonBox(frame, "copyjob_bb",
				  (XtCallbackProc)copyjob_bb_callback, NULL,
				  "OK", "Clear", " Cancel ",
				  NULL);
    
    return(frame);
}



static Widget create_copyjob_textgroup_widget(Widget parent, arcenv_t aenv)
{
    int i;
    Widget frame, colform;
    Widget form = 0;
    static char *labels[] = { "OLD NAME:", "NEW NAME:" };
    static unsigned int copyjob_help_vectors[] = {_OldName, _NewName};
    
    frame = XtVaCreateManagedWidget("copyjob_textgroup_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    colform = XtVaCreateWidget("copyjob_textgroup_rowcol",
			       xmFormWidgetClass, frame,
			       NULL);
    
    for (i = 0; i < NUM_COPYJOB_LABELS; i++) {
	if (i == 0) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	} else if (i == (NUM_COPYJOB_LABELS - 1)) {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNbottomOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	    
	} else {
	    form = XtVaCreateWidget("form",
				    xmFormWidgetClass, colform,
				    XmNrightAttachment, XmATTACH_FORM,
				    XmNrightOffset, OFFSET,
				    XmNleftAttachment, XmATTACH_FORM,
				    XmNleftOffset, OFFSET,
				    XmNtopAttachment, XmATTACH_WIDGET,
				    XmNtopWidget, form,
				    XmNtopOffset, OFFSET_MED,
				    XmNfractionBase, BASESIZE,
				    NULL);
	}
	(void) XtVaCreateManagedWidget(labels[i],
				       xmLabelGadgetClass, form,
				       XmNtopAttachment, XmATTACH_FORM,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNrightAttachment, XmATTACH_POSITION,
				       XmNrightPosition, TEXT_LABEL_END,
				       XmNalignment, XmALIGNMENT_END,
				       NULL);
	switch (i) {
	    /* OLD NAME AND NEW NAME Cases */
	case 0:
	case 1:
	    aenv->copyjob[i] = 
		XtVaCreateManagedWidget("textfield",
					xmTextFieldWidgetClass, form,
					XmNtraversalOn, True,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_POSITION,
					XmNleftPosition, TEXT_FIELD_START,
					XmNeditable, ((i == 0)?False:True),
					XmNcursorPositionVisible,
					((i == 0)?False:True),
					NULL);
	    XtAddCallback(aenv->copyjob[i], XmNactivateCallback,
			  (XtCallbackProc)XmProcessTraversal,
			  (XtPointer)XmTRAVERSE_NEXT_TAB_GROUP);
	    
	    SET_TEXT_FIELD_HELP(aenv->copyjob[i],
				_CopyBackupJob_WindowDescription,
				copyjob_help_vectors[i]);
	    break;
	}
	XtManageChild(form);
    }
    XtManageChild(colform);
    
    return(frame);
}

