
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
/*
 * DisCmd.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/FileSB.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>

/** SYSTEM INCLUDES **/ 

#include <unistd.h>
#include <strings.h>
#include <re_comp.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HPUX
#include <regex.h>
#endif

/** INRI INCLUDES **/

#include <ButtonBox.h>
#include <MiscXFcts.h>


/** TYPEDEF DEFINITIONS **/

typedef struct HoldWidgets {
        Widget text_w, dialog;
        int pp;
        int numLines;
        XtInputId input_id;
        int child_pid;
        char *regexp;
} HoldWidgetsStruct;

/** FUNCTION PROTOTYPES **/

static void discmd_wm_close_callback(Widget, XtPointer, XmAnyCallbackStruct*);
static void cancel_save(Widget, XtPointer, XmFileSelectionBoxCallbackStruct*);
static void ok_save(Widget, Widget, XmFileSelectionBoxCallbackStruct*);
static void save_text(Widget);
static void ButtonBoxCallback(Widget, struct HoldWidgets*, 
                              XiButtonBoxCallbackStruct*);
static void ReadCommand(struct HoldWidgets*, int*, XtInputId*);
static void ProcessCommand(XtAppContext, struct HoldWidgets*, char*);
void XiDisplayCommand(XtAppContext, Widget, Widget, char*, char*, Display*,
                      char*);
void XiDisplayCommandClose(void);
static int pipe_open(char*, int*);
static void clean_up_for_exit(struct HoldWidgets*);
static void shutdown_input(struct HoldWidgets*);

/** DEFINE DEFINITIONS **/

#define OFFSET_SMALL	3
#define MAX_ITEM_LEN	1024
#define MAX_OUTPUT_LEN	10000
#define MAX_ARGS		256


static struct HoldWidgets *fdWidgetsIndex[FD_SETSIZE];
static int fdWidgetsIndexInit = 0;

static Widget toplevel;

/** FUNCTION DEFINITIONS **/

static void shutdown_input(struct HoldWidgets *widgets)
{
    if (widgets == NULL) {
	printf("XiDisplayCommand: Error: shutdown_input was clled with widgets == NULL\n");
	return;
    }
    if (widgets->pp == -1) return;
    close(widgets->pp);
    XtRemoveInput(widgets->input_id);
    fdWidgetsIndex[widgets->pp] = NULL;
    widgets->pp = -1;
}

static void clean_up_for_exit(struct HoldWidgets *widgets)
{
    shutdown_input(widgets);
    XtDestroyWidget(widgets->dialog);
    kill(widgets->child_pid, SIGKILL);
    /*if (widgets->regexp != NULL) free(widgets->regexp);*/
    free(widgets);
}	

static void discmd_wm_close_callback(Widget w, XtPointer client, 
				     XmAnyCallbackStruct *call)
{
    struct HoldWidgets *widgets = (struct HoldWidgets *) client;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    call=call;
    
    clean_up_for_exit(widgets);
}

static void cancel_save(Widget filebox, XtPointer client, 
			XmFileSelectionBoxCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    client=client;
    call=call;
    
    XtDestroyWidget(filebox);
}



static void ok_save(Widget filebox, Widget text_w, 
		    XmFileSelectionBoxCallbackStruct *call)
{
    char *filename;
    char *string, buf[200];
    FILE *fp;
    
    waitCursor(filebox, True, True);
    
    if (!XmStringGetLtoR(call->value, XmSTRING_DEFAULT_CHARSET, &filename))
	return; /* unknown internal error */
    if (!*filename) {  /*nothing typed?*/
	XtFree(filename);
	return;
    }

    /*	printf("File selected: %s\n", filename); */
    string = XmTextGetString(text_w);
    
    if ( (fp = fopen(filename, "w")) == NULL) {
	sprintf(buf, "Unable to write to file \"%s\".", filename);
	popupDialogBox(WARNING, True, buf, True, False, False,
		       NULL, filebox);
	XtFree(string);
	XtFree(filename);
	waitCursor(filebox, False, True);
	return;
    }
    
    fputs(string, fp);
    fclose(fp);
    
    XtFree(filename);
    XtFree(string);
    
    waitCursor(filebox, False, True);
    XtDestroyWidget(filebox);
}



static void save_text(Widget text_w)
{
    Widget filebox;
    Arg arg[2];
    
    XtSetArg(arg[0], XmNtitle, "File Selection Dialog");
    XtSetArg(arg[1], XmNtextColumns, 50);
    filebox = XmCreateFileSelectionDialog(text_w, (char*)"Save", arg, 2);
    XtManageChild(filebox);
    XtUnmanageChild(XmFileSelectionBoxGetChild(filebox, XmDIALOG_HELP_BUTTON));
    XtAddCallback(filebox, XmNcancelCallback, (XtCallbackProc)cancel_save, 
		  NULL);
    XtAddCallback(filebox, XmNokCallback, (XtCallbackProc)ok_save, text_w);
}



static void ButtonBoxCallback(Widget w, struct HoldWidgets *widgets, 
			      XiButtonBoxCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    switch (call->id) {
	/* EXIT Case */
    case 1:
	clean_up_for_exit(widgets);
	break;
	
	/* SAVE Case */
    case 0:
	save_text(widgets->text_w);
	break;
    }
}


static void ReadCommand(struct HoldWidgets *widgets, int *fd, XtInputId *id)
{
    char buf[MAX_ITEM_LEN];
    int insertion_point,nread,i,totalread;
#ifdef HPUX
    regex_t re;
#else
    char *regerror;
#endif
    char *bufptr;
    
    /* done to get rid of compiler warning about unused parameters */
    fd=fd;
    id=id;
    
    bzero(buf, MAX_ITEM_LEN);
    totalread = 0;
    
    bufptr = buf;
    while (((nread = read(widgets->pp, bufptr, 1)) > 0) &&
	   (*bufptr != '\n')) {
	bufptr++;
	if (totalread++ >= MAX_ITEM_LEN) {
	    printf("XiDisplayCommand: Warning: MAX_ITEM_LEN exceeded, filter results may be incorrect.\n");
	    break;
	}
    }
    
    if (nread == 0) {
	/** End of File **/
	shutdown_input(widgets);
	return;
    }
    
    if (widgets->numLines < MAX_OUTPUT_LEN) {
	
	if (strcmp(widgets->regexp, "") != 0) {
	    /** parse input into lines and pass through reg expression **/
#ifdef HPUX
	    i = regcomp(&re, widgets->regexp, REG_NOSUB|REG_ICASE);
	    if (i!=0) {
		(void)regerror(i,&re, buf, sizeof(buf));
		shutdown_input(widgets);
	    } else {
		i = regexec(&re, buf, (size_t) 0, NULL, 0);
		regfree(&re);
		if (i!=0) {
		    (void)regerror(i,&re, buf, sizeof(buf));
		    strcpy(buf,"");
		}
	    }
#else
	    regerror = (char *)re_comp(widgets->regexp);
	    if (regerror!=NULL) {
		strcpy(buf, regerror);
		shutdown_input(widgets);
	    } else {
		i = re_exec(buf);
		if (i!=1) {
		    strcpy(buf,"");
		}
	    }
#endif
	}
	
	insertion_point = XmTextGetLastPosition(widgets->text_w);
	XmTextReplace(widgets->text_w, insertion_point, insertion_point, buf);
	widgets->numLines++;
    } else {
	if (widgets->numLines >= MAX_OUTPUT_LEN) {
	    /*char buf[200];*/
	    sprintf(buf,
		    "Output exceeds limit of %d lines.\nOutput truncated!",
		    MAX_OUTPUT_LEN);
	    popupDialogBox(WARNING, True, buf, True, False, False,
			   NULL, widgets->text_w);
	    shutdown_input(widgets);
	}
    }
}


static void ProcessCommand(XtAppContext app_context, struct HoldWidgets *widgets, 
			   char *cmd)
{
    char buf[200];
    
    
    widgets->child_pid = pipe_open(cmd, &widgets->pp);
    if (widgets->child_pid <= 0) widgets->pp = 0;
    widgets->numLines =  0;
    
    if (widgets->pp != 0) {
	widgets->input_id = XtAppAddInput(app_context, widgets->pp,
					  (XtPointer)XtInputReadMask,
					  (XtInputCallbackProc)ReadCommand, widgets);
	fdWidgetsIndex[widgets->pp] = widgets; /*Create an index to */
	/*this widget structure */
	/*based on the file descriptor*/
    } else {
	sprintf(buf, "Unable to execute command: %s", cmd);
	XmTextReplace(widgets->text_w,
		      XmTextGetInsertionPosition(widgets->text_w),
		      XmTextGetInsertionPosition(widgets->text_w),buf);
    }
}


void XiDisplayCommand(XtAppContext app_context, Widget t, Widget w, char *cmd, 
		      char *WindowTitle, Display *display, char *regexp)
{
    Widget form, buttonBox;
    Arg args[10];
    struct HoldWidgets *widgets;
    Atom dw;
    
    toplevel = t;
    
    if (!fdWidgetsIndexInit) {
	/* If this is the first time XiDisplayCommand is called, then */
	/* initialize the index from fds to the widgets               */
	bzero(fdWidgetsIndex, sizeof(fdWidgetsIndex));
	fdWidgetsIndexInit = 1;
    }
    
    widgets = (struct HoldWidgets *) malloc(sizeof(struct HoldWidgets));
    
    if (regexp == NULL) {
	widgets->regexp = (char*)"";
    } else {
	widgets->regexp = (char *)malloc(strlen(regexp)+1);
	strcpy(widgets->regexp, regexp);
    }		
    
    widgets->dialog = XtVaCreatePopupShell(WindowTitle,
					   xmDialogShellWidgetClass, w,
					   XmNdeleteResponse, XmDO_NOTHING,
					   NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(widgets->dialog, dw, 
			    (XtCallbackProc)discmd_wm_close_callback,
			    (XtPointer)widgets);
    
    form = XtVaCreateWidget("DisCmdForm",
			    xmFormWidgetClass, widgets->dialog,
			    NULL);
    
    XtSetArg(args[0], XmNeditMode, XmMULTI_LINE_EDIT);
    XtSetArg(args[1], XmNeditable, False);
    XtSetArg(args[2], XmNcursorPositionVisible, False);
    XtSetArg(args[3], XmNwordWrap, True);
    XtSetArg(args[4], XmNhighlightThickness, 0);
    
    widgets->text_w = XmCreateScrolledText(form, (char*)"DisCmdText", args, 5);
    
    buttonBox = XiCreateButtonBox(form, (char*)"DisCmdButtonBox",
				  (XtCallbackProc)ButtonBoxCallback,
				  widgets, "Save", " Close ", NULL);
    XtVaSetValues(buttonBox,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, OFFSET_SMALL,
		  XmNrightOffset, OFFSET_SMALL,
		  XmNbottomOffset, OFFSET_SMALL,
		  XmNtopOffset, OFFSET_SMALL,
		  NULL);
    
    XtVaSetValues(XtParent(widgets->text_w),
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, buttonBox,
		  XmNtopOffset, OFFSET_SMALL,
		  XmNleftOffset, OFFSET_SMALL,
		  XmNrightOffset, OFFSET_SMALL,
		  NULL);

    ProcessCommand(app_context, widgets, cmd);
    
    XtManageChild(widgets->text_w);
    XtManageChild(form);
    XtPopup(widgets->dialog, XtGrabNone);
}

void XiDisplayCommandClose()
{
    int fd;
    /* close all file descriptors, and kill associated process if they */
    /* exist */
    for (fd = FD_SETSIZE-1; fd >=3; fd--)
	if (fdWidgetsIndex[fd] != NULL) {
	    clean_up_for_exit(fdWidgetsIndex[fd]);
	}
    return;
}

static int pipe_open(char *cmd, int *fd)
{
    int fds[2];
    int pid;
    char *argv[MAX_ARGS];
    
    pipe(fds);
    
    if ((pid = fork()) < 0) {
    	close(fds[0]);
	return(-1);
    }
    
    /* CHILD */
    
    if (pid == 0) {
	char *token;
	int nargs = 0;
	
	close(1); /* close stdin*/
	dup(fds[1]);
	close (fds[0]);
	
	token = strtok(cmd, " ");
	while ((token != NULL) && (nargs < MAX_ARGS)) {
	    argv[nargs++]=token;
	    token = strtok(NULL, " ");
	}
	if (nargs >= MAX_ARGS) {
	    printf("Warning: Cammand failed.  Too man arguments.\n");
	    exit(1);
	}
	
	argv[nargs]  = NULL;
	
	execv(argv[0], argv);
	perror("execv()");
	exit(1);
	/*system(cmd);
	  exit(0);*/
    }
    
    /* PARENT */
    
    close(fds[1]);
    *fd = fds[0];
    
    return(pid);
}
