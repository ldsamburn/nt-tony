
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * file_sel_box.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <X11/StringDefs.h>
#include <X11/Core.h>

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <time.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <ButtonBox.h>
#include <MiscXFcts.h>
#include <NtcssHelp.h>
#include <ExtList.h>


/** LOCAL INCLUDES **/

#include "fs_archive.h"
#include "problem_defs.h"
#include "fs_archive_help.h"



/** FUNCTION PROTOTYPES **/

Widget create_fsb_widget(Widget, char*);
static Widget create_fsb_bb_widget(Widget);
static Widget create_selection_widget(Widget);
static Widget create_filelist_widget(Widget);
static Widget create_dirlist_widget(Widget);
static void fsb_wm_close_callback(Widget, XtPointer, XmAnyCallbackStruct*);
static void fsb_bb_callback(Widget, XtPointer, XiButtonBoxCallbackStruct*);
static void file_sel_callback(Widget, XtPointer, XtPointer);
static void dir_sel_callback(Widget, XtPointer, XtPointer);
static void get_filelist_rows(Widget);
static void get_dirlist_rows(Widget);
static void double_click_callback(Widget, XtPointer, XtPointer);
static void filelist_findbtn_callback(Widget, XtPointer, XtPointer);
static void dirlist_findbtn_callback(Widget, XtPointer, XtPointer);


/** STATIC VARIABLES **/

static int num_files;
static int num_dirs;
static char curr_dir[MAX_FILENAME_LEN];
static Widget fsb_textfield, filelist_list, dirlist_list;
static Widget filelist_tb, dirlist_tb;
static char *titles[] = { "OWNER", "NAME" };



/** FUNCTION DEFINITIONS **/

Widget create_fsb_widget(Widget parent, char *start_dir)
{
    Widget container, dirlist, filelist, selection, buttonbox;
    Widget dialog;
    Atom dw;
    
    if (start_dir != NULL &&
	*start_dir != 0 &&
	start_dir[strlen(start_dir) - 1] == '/')
	strcpy(curr_dir, start_dir);
    else
	strcpy(curr_dir, "/");
    
    (void) checkForStop(parent);
    
    dialog = XtVaCreatePopupShell("fsb_dialog",
				  transientShellWidgetClass, parent,
				  XtNmappedWhenManaged, False,
				  XmNdeleteResponse, XmDO_NOTHING,
				  XmNtitle, "File Selection",
				  XmNwidth, FILESEL_WIDTH,
				  XmNheight, FILESEL_HEIGHT,
				  NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(dialog, dw, 
			    (XtCallbackProc)fsb_wm_close_callback, NULL);
    
    (void) checkForStop(parent);
    
    container = XtVaCreateManagedWidget("fsb_container",
					xmFormWidgetClass, dialog,
					XmNfractionBase, 10,
					NULL);
    
    buttonbox = create_fsb_bb_widget(container);
    XtVaSetValues(buttonbox, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    (void) checkForStop(parent);
    
    selection = create_selection_widget(container);
    XtVaSetValues(selection, XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonbox,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  NULL);
    
    (void) checkForStop(parent);
    
    dirlist = create_dirlist_widget(container);
    XtVaSetValues(dirlist, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, selection,
		  XmNbottomOffset, OFFSET,
		  XmNrightAttachment, XmATTACH_POSITION,
		  XmNrightPosition, 4,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET,
		  NULL);
    
    (void) checkForStop(parent);
    
    filelist = create_filelist_widget(container);
    XtVaSetValues(filelist, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, OFFSET,
		  XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		  XmNbottomWidget, dirlist,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, OFFSET,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, dirlist,
		  XmNleftOffset, OFFSET,
		  NULL);
    
    (void) checkForStop(parent);
    
    SET_HELP_CALLBACK(container);
    
    return(dialog);
}



static Widget create_fsb_bb_widget(Widget parent)
{
    Widget frame, buttonbox;
    
    frame = XtVaCreateManagedWidget("fsb_bb_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    buttonbox = XiCreateButtonBox(frame, "fsb_bb", (XtCallbackProc)fsb_bb_callback, NULL,
				  " Add Selection ", "Close", NULL);
    
    return(frame);
}



static Widget create_selection_widget(Widget parent)
{
    Widget frame, rowcol;
    
    frame = XtVaCreateManagedWidget("fsb_selection_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    rowcol = XtVaCreateManagedWidget("fsb_selection_rowcol",
				     xmRowColumnWidgetClass, frame,
				     XmNpacking, XmPACK_COLUMN,
				     XmNisAligned, False,
				     XmNorientation, XmVERTICAL,
				     NULL);
    
    (void) XtVaCreateManagedWidget("Selection",
				   xmLabelGadgetClass, rowcol,
				   XmNalignment, XmALIGNMENT_BEGINNING,
				   NULL);
    
    fsb_textfield = XtVaCreateManagedWidget("fsb_selection_textfield",
					    xmTextFieldWidgetClass, rowcol,
					    XmNeditable, False,
					    XmNcursorPositionVisible, False,
					    NULL);
    
    SET_TEXT_FIELD_HELP(fsb_textfield,
			_FileSelection_WindowDescription,_Selection_File);
    
    XtManageChild(rowcol);
    
    return(frame);
}



static Widget create_filelist_widget(Widget parent)
{
    Widget frame, container, label, findbtn, form, list;
    int i;
    XmString *header, s;
    Dimension lheight, pheight;
    
    frame = XtVaCreateManagedWidget("fsb_filelist_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    container = XtVaCreateManagedWidget("fsb_filelist_container",
					xmFormWidgetClass, frame,
					NULL);
    
    s = XmStringCreate("Files", XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("fsb_filelist_label",
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
    
    /*header = (XmString *) XtMalloc(sizeof(XmString)*2);*/
    header = (XmString *) malloc(sizeof(XmString)*2);
    for (i = 0; (unsigned int)i < XtNumber(titles); i++)
	header[i] = XmStringCreateLocalized(titles[i]);
    
    filelist_list = XtVaCreateManagedWidget("fsb_filelist_list",
					    xiExtdListWidgetClass, container,
					    XiNitemType, XiEXTLIST_ITEM_TYPE_NON_STRUCTURED,
					    XiNcolumnCount, XtNumber(titles),
					    XiNrowCount, 1,
					    XiNitems, header,
					    XiNselectionType, XiEXTLIST_SINGLE_SELECT,
					    XmNtopAttachment, XmATTACH_WIDGET,
					    XmNtopWidget, label,
					    XmNtopOffset, OFFSET,
					    XmNrightAttachment, XmATTACH_FORM,
					    XmNrightOffset, OFFSET_RT_EXT,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNleftOffset, OFFSET,
					    NULL);
    XtAddCallback(filelist_list, XiNsingleSelectCallback,
		  (XtCallbackProc)file_sel_callback, NULL);
    
    SET_EXTENDED_LIST_HELP(filelist_list,
			   list,_FileSelection_WindowDescription,_ICS_FileList);
    
    for (i = 0; (unsigned int)i < XtNumber(titles); i++)
	XmStringFree(header[i]);
    XtFree((char*)header);
    
    form = XtVaCreateWidget("form",
			    xmFormWidgetClass, container,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, OFFSET,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNleftOffset, OFFSET,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, OFFSET,
			    NULL);
    
    findbtn = XtVaCreateManagedWidget("Find",
				      xmPushButtonWidgetClass, form,
				      XmNleftAttachment, XmATTACH_FORM,
				      NULL);
    
    XtAddCallback(findbtn, XmNactivateCallback,
		  (XtCallbackProc)filelist_findbtn_callback, NULL);
    
    filelist_tb = XtVaCreateManagedWidget("textfield",
					  xmTextFieldWidgetClass, form,
					  XmNtopAttachment, XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_WIDGET,
					  XmNleftWidget, findbtn,
					  XmNleftOffset, OFFSET,
					  NULL);
    /*
     * Size the push button.
     */
    XtVaGetValues(filelist_tb, XmNheight, &lheight, NULL);
    XtVaGetValues(findbtn, XmNheight, &pheight, NULL);
    XtVaSetValues(findbtn, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, (lheight - pheight)/2,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, (lheight - pheight)/2,
		  NULL);
    XtManageChild(form);
    
    XtVaSetValues(filelist_list, XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, form,
		  XmNbottomOffset, OFFSET_BT_EXT,
		  NULL);
    
    get_filelist_rows(NULL);
    
    return(frame);
}



static Widget create_dirlist_widget(Widget parent)
{
    Widget frame, container, label, findbtn, form, list;
    int i;
    XmString *header, s;
    Dimension lheight, pheight;
    
    frame = XtVaCreateManagedWidget("fsb_dirlist_frame",
				    xmFrameWidgetClass, parent,
				    XmNshadowType, XmSHADOW_ETCHED_IN,
				    NULL);
    
    container = XtVaCreateManagedWidget("fsb_dirlist_container",
					xmFormWidgetClass, frame,
					NULL);
    
    s = XmStringCreate("Directories", XmSTRING_DEFAULT_CHARSET);
    label = XtVaCreateManagedWidget("fsb_dirlist_label",
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
    
    /*header = (XmString *) XtMalloc(sizeof(XmString)*2);*/
    header = (XmString *) malloc(sizeof(XmString)*2);
    for (i = 0; (unsigned int)i < XtNumber(titles); i++)
	header[i] = XmStringCreateLocalized(titles[i]);
    
    dirlist_list = XtVaCreateManagedWidget("fsb_dirlist_list",
					   xiExtdListWidgetClass, container,
					   XiNitemType, XiEXTLIST_ITEM_TYPE_NON_STRUCTURED,
					   XiNcolumnCount, XtNumber(titles),
					   XiNrowCount, 1,
					   XiNitems, header,
					   XiNselectionType, XiEXTLIST_SINGLE_SELECT,
					   XmNtopAttachment, XmATTACH_WIDGET,
					   XmNtopWidget, label,
					   XmNtopOffset, OFFSET,
					   XmNrightAttachment, XmATTACH_FORM,
					   XmNrightOffset, OFFSET_RT_EXT,
					   XmNleftAttachment, XmATTACH_FORM,
					   XmNleftOffset, OFFSET,
					   NULL);
    XtAddCallback(dirlist_list, XiNsingleSelectCallback,
		  (XtCallbackProc)dir_sel_callback, NULL);
    XtAddCallback(dirlist_list, XiNdefaultActionCallback,
		  (XtCallbackProc)double_click_callback, NULL);
    
    SET_EXTENDED_LIST_HELP(dirlist_list,
			   list,_FileSelection_WindowDescription,_ICS_DirectoryList);
    
    for (i = 0; (unsigned int)i < XtNumber(titles); i++)
	XmStringFree(header[i]);
    XtFree((char*)header);
    
    form = XtVaCreateWidget("form",
			    xmFormWidgetClass, container,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, OFFSET,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNleftOffset, OFFSET,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, OFFSET,
			    NULL);
    
    findbtn = XtVaCreateManagedWidget("Find",
				      xmPushButtonWidgetClass, form,
				      XmNleftAttachment, XmATTACH_FORM,
				      NULL);
    
    XtAddCallback(findbtn, XmNactivateCallback,
		  dirlist_findbtn_callback, NULL);
    
    dirlist_tb = XtVaCreateManagedWidget("textfield",
					 xmTextFieldWidgetClass, form,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNleftWidget, findbtn,
					 XmNleftOffset, OFFSET,
					 NULL);
    /*
     * Size the push button.
     */
    XtVaGetValues(dirlist_tb, XmNheight, &lheight, NULL);
    XtVaGetValues(findbtn, XmNheight, &pheight, NULL);
    XtVaSetValues(findbtn, XmNtopAttachment, XmATTACH_FORM,
		  XmNtopOffset, (lheight - pheight)/2,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, (lheight - pheight)/2,
		  NULL);
    XtManageChild(form);
    
    XtVaSetValues(dirlist_list, XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, form,
		  XmNbottomOffset, OFFSET_BT_EXT,
		  NULL);
    
    get_dirlist_rows(NULL);
    
    return(frame);
}



static void fsb_wm_close_callback(Widget w, XtPointer client, 
				  XmAnyCallbackStruct *call)
{
    arcenv_t aenv;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    call=call;
    
#ifdef VSUN
    if (call->reason == XmCR_PROTOCOLS) {
#endif
	FINDCONTEXT(toplevel, env, &aenv);
	change_backup_buttons(aenv->backup_bb,
			      aenv->backup_filelist_bb, True);
	XtPopdown(fsb_dialog);
	restore_help_info();
#ifdef VSUN
    }
#endif
}



static void fsb_bb_callback(Widget w, XtPointer client, 
			    XiButtonBoxCallbackStruct *call)
{
    char *s, file[MAX_FILENAME_LEN], listing[MAX_FILELISTING_LEN];
    char buf[MAX_FILELINE_LEN];
    FILE *fp;
    arcenv_t aenv;
    XmString xstr;
    int size;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
    FINDCONTEXT(toplevel, env, &aenv);
    switch (call->id) {
	/* ADD SELECTION Case */
    case 0:
	s = XmTextFieldGetString(fsb_textfield);
	if (!(s == NULL || *s == 0)) {
	    if (s[strlen(s) - 2] == '.' && s[strlen(s) - 1] == '.') {
		displayProblem(WARNING, DONT_APPLY_PARENT_DIRS_PROB,
			       problem_array, fsb_dialog);
		return;
	    } else if (s[strlen(s) - 1] == '.')
		s[strlen(s) - 1] = 0;
	    /*
	     * Check if file already in list.
	     */
	    waitCursor(fsb_dialog, True, False);
	    sprintf(file, "%s/%s", job_data[aenv->job_index].dir,
		    "templist");
	    if ((fp = fopen(file, "r")) != NULL) {
		while (fgets(buf, MAX_FILELINE_LEN, fp) != NULL) {
		    (void) checkForStop(fsb_dialog);
		    stripNewline(buf);
		    trim(buf);
		    if (strcmp(buf, s) == 0) {
			fclose(fp);
			XtFree(s);
			waitCursor(fsb_dialog, False, False);
			displayProblem(WARNING,
				       FILE_OR_DIR_ALREADY_IN_LIST_PROB,
				       problem_array, fsb_dialog);
			return;
		    }
		}
	    } else {
		XtFree(s);
		waitCursor(fsb_dialog, False, False);
		displayProblem(WARNING, CANT_READ_THING_PROB,
			       problem_array, fsb_dialog,
			       "backup job filelist");
		return;
	    }
	    fclose(fp);
	    /*
	     * Place selection in backup filelist and update list.
	     */
	    if ((fp = fopen(file, "a")) != NULL) {
		(void) checkForStop(fsb_dialog);
		build_file_listing(s, listing);
		fprintf(fp, "%s\n", s);
		fclose(fp);
		xstr = XmStringCreateSimple(listing);
		XmListAddItem(aenv->backup_filelist, xstr, 0);
		XmStringFree(xstr);
		(void) checkForStop(fsb_dialog);
		if (aenv->show_size) {
		    size = get_file_size(s);
#ifdef HPUX
		    size = size/2;
#endif
		    filelist_size += size;
		    sprintf(buf, "%d", filelist_size);
		    XmTextFieldSetString(aenv->backup_tb[B_SIZE_FLD], buf);
		}
	    } else {
		waitCursor(fsb_dialog, False, False);
		XtFree(s);
		displayProblem(WARNING, CANT_WRITE_THING_PROB,
			       problem_array, fsb_dialog,
			       "backup job filelist");
		return;
	    }
	    waitCursor(fsb_dialog, False, False);
	    XtFree(s);
	} else {
	    displayProblem(WARNING, NO_THING_SELECTED_PROB,
			   problem_array, fsb_dialog,
			   "file or directory");
	    return;
	}
	break;
	
	
	/* CLOSE Case */
    case 1:
	change_backup_buttons(aenv->backup_bb,
			      aenv->backup_filelist_bb, True);
	XtPopdown(fsb_dialog);
	restore_help_info();
	break;
    }
}



static void file_sel_callback(Widget w, XtPointer client, XtPointer call)
{
    XiExtdListCallbackStruct *cbs = (XiExtdListCallbackStruct *) call;
    char buf[MAX_FILENAME_LEN], *s;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
#ifdef VSUN
    XmStringGetLtoR(cbs->item.item[1], XmSTRING_DEFAULT_CHARSET, &s);
#else
    XmStringGetLtoR(cbs->item.item[1], XmFONTLIST_DEFAULT_TAG, &s);
#endif
    strcpy(buf, s);
    XtFree(s);
    
    if (!isBlank(buf))
	XmTextFieldSetString(fsb_textfield, buf);
}



static void dir_sel_callback(Widget w, XtPointer client, XtPointer call)
{
    XiExtdListCallbackStruct *cbs = (XiExtdListCallbackStruct *) call;
    char buf[MAX_FILENAME_LEN], *s;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
#ifdef VSUN
    XmStringGetLtoR(cbs->item.item[1], XmSTRING_DEFAULT_CHARSET, &s);
#else
    XmStringGetLtoR(cbs->item.item[1], XmFONTLIST_DEFAULT_TAG, &s);
#endif
    strcpy(buf, s);
    XtFree(s);
    
    if (!isBlank(buf))
	XmTextFieldSetString(fsb_textfield, buf);
}



static void get_filelist_rows(Widget w)
{
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;
    char buf[MAX_FILENAME_LEN], user[MAX_USER_LEN+1];
    ExtdListItemStruct frow;
    XmString *rowitem;
    
    num_files = 0;
    
    if ((dp = opendir(curr_dir)) == NULL)
	return;
    
    if (w)
	(void) checkForStop(w);
    
    XiExtdListDeleteAllItems(filelist_list);
    
    /*rowitem = (XmString *) XtMalloc(sizeof(XmString)*2);*/
    rowitem = (XmString *) malloc(sizeof(XmString)*2);
    while ((direc = readdir(dp)) != NULL) {
	if (w)
	    (void) checkForStop(w);
	if (direc->d_ino == 0)
	    continue;
	sprintf(buf, "%s%s", curr_dir, direc->d_name);
	if (stat(buf, &sbuf) == -1)
	    continue;
	if ((sbuf.st_mode & S_IFMT) == S_IFREG) {
	    get_owner(user, &sbuf);
	    rowitem[0] = XmStringCreateLocalized(user);
	    rowitem[1] = XmStringCreateLocalized(buf);
	    frow.item = rowitem;
	    frow.selected = False;
	    frow.sensitive = True;
	    frow.foreground = 0;
	    frow.hook = NULL;
	    XiExtdListAddItem(filelist_list, frow, 0);
	    XmStringFree(rowitem[0]);
	    XmStringFree(rowitem[1]);
	    num_files++;
	}
    }
    if (w)
	(void) checkForStop(w);
    closedir(dp);
    XtFree((char*)rowitem);
}



static void get_dirlist_rows(Widget w)
{
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;
    char buf[MAX_FILENAME_LEN], user[MAX_USER_LEN+1];
    ExtdListItemStruct drow;
    XmString *rowitem;
    int count, i, leng, keep_going = 1;
    
    num_dirs = 0;
    
    if ((dp = opendir(curr_dir)) == NULL)
	keep_going = 0;
    
    XiExtdListDeleteAllItems(dirlist_list);
    /*
     * Set row for current dir.
     */
    sprintf(buf, "%s", curr_dir);
    if (stat(buf, &sbuf) != -1)
	get_owner(user, &sbuf);
    else
	strcpy(user, "");
    sprintf(buf, "%s.", curr_dir);
    /*rowitem = (XmString *) XtMalloc(sizeof(XmString)*2);*/
    rowitem = (XmString *) malloc(sizeof(XmString)*2);
    rowitem[0] = XmStringCreateLocalized(user);
    rowitem[1] = XmStringCreateLocalized(buf);
    drow.item = rowitem;
    drow.selected = False;
    drow.sensitive = True;
    drow.foreground = 0;
    drow.hook = NULL;
    XiExtdListAddItem(dirlist_list, drow, 0);
    XmStringFree(rowitem[0]);
    XmStringFree(rowitem[1]);
    num_dirs++;
    
    if (w)
	(void) checkForStop(w);
    /*
     * Set row for parent dir of current dir.
     */
    if (strcmp(curr_dir, "/") != 0) { 
	sprintf(buf, "%s", curr_dir);
	leng = strlen(buf);
	count = 0;
	i = leng - 1;
	while (i >= 0) {
	    if (buf[i] == '/' && count != 1) {
		count++;
	    } else if (buf[i] == '/' && count == 1) {
		buf[i + 1] = 0;
		break;
	    }
	    i--;
	}
	if (stat(buf, &sbuf) != -1)
	    get_owner(user, &sbuf);
	else
	    strcpy(user, "");
	sprintf(buf, "%s..", curr_dir);
	rowitem[0] = XmStringCreateLocalized(user);
	rowitem[1] = XmStringCreateLocalized(buf);
	drow.item = rowitem;
	drow.selected = False;
	drow.sensitive = True;
	drow.foreground = 0;
	drow.hook = NULL;
	XiExtdListAddItem(dirlist_list, drow, 0);
	XmStringFree(rowitem[0]);
	XmStringFree(rowitem[1]);
	num_dirs++;
    }
    if (w)
	(void) checkForStop(w);
    
    if (!keep_going) {
	XtFree((char*)rowitem);
	return;
    }
    
    while ((direc = readdir(dp)) != NULL) {
	if (w)
	    (void) checkForStop(w);
	if (direc->d_ino == 0)
	    continue;
	if ((strcmp(direc->d_name, "..") == 0) ||
	    (strcmp(direc->d_name, ".") == 0))
	    continue;
	sprintf(buf, "%s%s/", curr_dir, direc->d_name);
	if (stat(buf, &sbuf) == -1)
	    continue;
	if ((sbuf.st_mode & S_IFMT) == S_IFDIR) {
	    get_owner(user, &sbuf);
	    rowitem[0] = XmStringCreateLocalized(user);
	    rowitem[1] = XmStringCreateLocalized(buf);
	    drow.item = rowitem;
	    drow.selected = False;
	    drow.sensitive = True;
	    drow.foreground = 0;
	    drow.hook = NULL;
	    XiExtdListAddItem(dirlist_list, drow, 0);
	    XmStringFree(rowitem[0]);
	    XmStringFree(rowitem[1]);
	    num_dirs++;
	}
    }
    if (w)
	(void) checkForStop(w);
    closedir(dp);
    XtFree((char*)rowitem);
}



static void double_click_callback(Widget w, XtPointer client, XtPointer call)
{
    XiExtdListCallbackStruct *cbs = (XiExtdListCallbackStruct *) call;
    char buf[MAX_FILENAME_LEN];
    int leng;
    char *s;
    int flag = 0;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    
#ifdef VSUN
    XmStringGetLtoR(cbs->item.item[1], XmSTRING_DEFAULT_CHARSET, &s);
#else
    XmStringGetLtoR(cbs->item.item[1], XmFONTLIST_DEFAULT_TAG, &s);
#endif
    strcpy(buf, s);
    XtFree(s);
    
    leng = strlen(buf);
    if (buf[leng - 1] == '.' && buf[leng - 2] == '.') {
	/*
	 * Ascend in dir hierarchy.
	 */
	if (strcmp(curr_dir, "/") == 0)
	    return;
	if ((s = strrchr(curr_dir, '/'))) {
	    *s = 0;
	    if ((s = strrchr(curr_dir, '/'))) {
		s++;
		*s = 0;
		flag = 1;
	    }
	}
	if (!flag)
	    return;	
	waitCursor(fsb_dialog, True, False);
	XmTextFieldSetString(fsb_textfield, "");
	(void) checkForStop(fsb_dialog);
	get_filelist_rows(fsb_dialog);
	get_dirlist_rows(fsb_dialog);
	(void) checkForStop(fsb_dialog);
	waitCursor(fsb_dialog, False, False);
    } else if (buf[leng - 1] == '/') {
	/*
	 * Descend in dir hierarchy.
	 */
	waitCursor(fsb_dialog, True, False);
	strcpy(curr_dir, buf);
	XmTextFieldSetString(fsb_textfield, "");
	(void) checkForStop(fsb_dialog);
	get_filelist_rows(fsb_dialog);
	get_dirlist_rows(fsb_dialog);
	(void) checkForStop(fsb_dialog);
	waitCursor(fsb_dialog, False, False);
    }
}



static void filelist_findbtn_callback(Widget w, XtPointer client,
				      XtPointer call)
{
    char *text = XmTextFieldGetString(filelist_tb);
    XmString xms;
    int *pos, count, start, row;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    call=call;
    
    if (text == NULL)
	return;
    
    xms = XmStringCreateLocalized(text);
    count = XiExtdListGetSelectedPos(filelist_list, &pos);
    if (count > 0)
	start = pos[count -1] + 1;
    else
	start = 1;
    row = XiExtdListFindItem(filelist_list, xms, -1, start, True);
    XmStringFree(xms);
    XtFree(text);
}



static void dirlist_findbtn_callback(Widget w, XtPointer client, 
				     XtPointer call)
{
    char *text = XmTextFieldGetString(dirlist_tb);
    XmString xms;
    int *pos, count, start, row;
    
    /* done only to get rid of compiler warning about unused parameters */
    w=w;
    client=client;
    call=call;
    
    if (text == NULL)
	return;
    
    xms = XmStringCreateLocalized(text);
    count = XiExtdListGetSelectedPos(dirlist_list, &pos);
    if (count > 0)
	start = pos[count -1] + 1;
    else
	start = 1;
    row = XiExtdListFindItem(dirlist_list, xms, -1, start, True);
    XmStringFree(xms);
    XtFree(text);
}

