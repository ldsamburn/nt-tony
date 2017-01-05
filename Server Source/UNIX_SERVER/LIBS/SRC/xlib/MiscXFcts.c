
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * MiscXFcts.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

/** INRI INCLUDES **/

#include <MiscXFcts.h>
#include <NtcssHelp.h>

/** LOCAL INCLUDES **/

#include "MiscXFctsP.h"


/** FUNCTION PROTOTYPES **/

XPointer convertColor(Widget, char*);
void exitApp(int);
void popupDialogBox(DialogType, Boolean, char*, Boolean, Boolean, Boolean,
                    void (*)(Widget, XtPointer, XmAnyCallbackStruct*), Widget);
void defaultDialogCallback(Widget, XtPointer, XmAnyCallbackStruct*);
void defaultExitCallback(Widget, XtPointer, XmAnyCallbackStruct*);
void displayProblem(DialogType, Problem, ProblemItem*, Widget, ...); 
static Cursor create_wait_cursor(Widget, Boolean);
void changeWaitMessage(char*);
void waitCursor(Widget, Boolean, Boolean);
Boolean checkForStop(Widget);
void stopWaitCallback(Widget, XtPointer, XmAnyCallbackStruct*);
void beep(Display*);
char *getFileMsg(char*);
Widget findTopmostShell(Widget);
void popupCenterDialog(Widget, Widget, Dimension, Dimension);

#if defined SOLARIS || defined HPUX
extern char *sys_errlist[];
#endif

/** STATIC VARIABLES **/

static Boolean stopped;
static Widget wait_dialog;

static int time16_bits[] =
{
	0x00, 0x00, 0xfe, 0x7f, 0x14, 0x28, 0x14, 0x28, 0x14, 0x28, 0x24, 0x24,
	0x44, 0x22, 0x84, 0x21, 0x84, 0x21, 0x44, 0x22, 0x24, 0x24, 0x14, 0x28,
	0x94, 0x29, 0xd4, 0x2b, 0xfe, 0x7f, 0x00, 0x00
};

static int time16m_bits[] =
{
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
	0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
	0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff
};


/** FUNCTION DEFINITIONS **/

XPointer convertColor(Widget w, char *name)
{
    XrmValue fromVal, toVal;
    XPointer pixel;
    
    fromVal.size = sizeof(char *);
    fromVal.addr = name;
    
    XtConvert(w, XmRString, &fromVal, XmRPixel, &toVal);
    pixel = toVal.addr;
    if(pixel == NULL)
    {
        fromVal.addr = (char*)XtDefaultBackground;
        XtConvert(w, XmRString, &fromVal, XmRPixel, &toVal);
        pixel = toVal.addr;
    }
    return(pixel);
}



void exitApp(int retval)
{
/*
    free_help();
*/
    exit(retval);
}



void popupDialogBox(DialogType type, Boolean exclusive, char* msg, 
                    Boolean ok_on, Boolean cancel_on, Boolean help_on,
                    void (*callback_fct)(Widget, XtPointer, 
                    XmAnyCallbackStruct*), 
                    Widget shell)
{
    Widget dialog = 0;
    XmString xstr, cancelstr = 0;
    int i = 0;
    Arg arg[5];
    
    if (cancel_on) {
	cancelstr = XmStringCreateSimple((char*)"Cancel");
	XtSetArg(arg[i], XmNcancelLabelString, cancelstr); i++;
    }
    
    xstr = XmStringCreateLtoR(msg, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(arg[i], XmNmessageString, xstr); i++;
    XtSetArg(arg[i], XmNmessageAlignment, XmALIGNMENT_CENTER); i++;
    XtSetArg(arg[i], XmNdeleteResponse, XmDO_NOTHING); i++;
    
    switch (type) {
    case WARNING:
	XtSetArg(arg[i], XmNtitle, "Warning"); i++;
	dialog = XmCreateWarningDialog(shell, (char*)"warning", arg, i);
	break;
    case INFO:
	XtSetArg(arg[i], XmNtitle, "Information"); i++;
	dialog = XmCreateInformationDialog(shell, (char*)"information", arg, i);
	break;
    case WORKING:
	XtSetArg(arg[i], XmNtitle, "Working"); i++;
	dialog = XmCreateWorkingDialog(shell, (char*)"working", arg, i);
	break;
    case ERROR:
	XtSetArg(arg[i], XmNtitle, "Error"); i++;
	dialog = XmCreateErrorDialog(shell, (char*)"error", arg, i);
	break;
    case QUESTION:
	XtSetArg(arg[i], XmNtitle, "Question"); i++;
	dialog = XmCreateQuestionDialog(shell, (char*)"question", arg, i);
	break;
    }
    XmStringFree(xstr);
    
    if (cancel_on)
	XmStringFree(cancelstr);
    
    if (exclusive)
	XtVaSetValues(dialog, XmNdialogStyle,
		      XmDIALOG_FULL_APPLICATION_MODAL, NULL);
    
    if (callback_fct != NULL) {
	if (ok_on)
	    XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)callback_fct, NULL);
	if (cancel_on)
	    XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc)callback_fct,
			  NULL);
	if (help_on)
	    XtAddCallback(dialog, XmNhelpCallback, (XtCallbackProc)callback_fct,
			  NULL);
    } else {
	if (ok_on) {
	    if (type == ERROR) 
		XtAddCallback(dialog, XmNokCallback,
			      (XtCallbackProc)defaultExitCallback, NULL);
	    else
		XtAddCallback(dialog, XmNokCallback,
			      (XtCallbackProc)defaultDialogCallback, NULL);
	}
	if (cancel_on) {
	    if (type == ERROR) 
		XtAddCallback(dialog, XmNcancelCallback,
			      (XtCallbackProc)defaultExitCallback, NULL);
	    else
		XtAddCallback(dialog, XmNcancelCallback,
			      (XtCallbackProc)defaultDialogCallback, NULL);
	}
	if (help_on) {
	    if (type == ERROR) 
		XtAddCallback(dialog, XmNhelpCallback,
			      (XtCallbackProc)defaultExitCallback, NULL);
	    else
		XtAddCallback(dialog, XmNhelpCallback,
			      (XtCallbackProc)defaultDialogCallback, NULL);
	}
    }
    
    if (!ok_on)
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
    if (!cancel_on)
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    if (!help_on)
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    
    XtManageChild(dialog);
    
    if (exclusive)
	XtPopup(XtParent(dialog), XtGrabExclusive);
    else
	XtPopup(XtParent(dialog), XtGrabNone);
}



void defaultDialogCallback(Widget w, XtPointer client_data,XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    client_data=client_data;
    call=call;
    
    XtPopdown(XtParent(w));
}



void defaultExitCallback(Widget w, XtPointer client_data, XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    client_data=client_data;
    call=call;
    
    XtPopdown(XtParent(w));
    exitApp(1);
}



void displayProblem(DialogType type, Problem num, ProblemItem* problem_array, 
                    Widget shell, ...)
{
    va_list ap;
    char msg[MAX_PROBLEM_MSG_LEN];
    
    if (num > ENOSYS) {
	va_start(ap, shell);
	vsprintf(msg, problem_array[num - ENOSYS - 1].msg, ap);
	popupDialogBox(type, True, msg, True, False, False, NULL, shell);
	va_end(ap);
    } else {
	sprintf(msg, "%s.", sys_errlist[num]);
	popupDialogBox(type, True, msg, True, False, False, NULL, shell);
    }
}



static Cursor create_wait_cursor(Widget w, Boolean interruptable)
{
    Cursor cursor;
    Pixmap pixmap, mask_pixmap;
    Display *dp = XtDisplay(w);
    Window win = XtWindow(w);
    XColor colors[2];
    
    interruptable=interruptable;
    
    pixmap = XCreateBitmapFromData(dp, win, (char*)time16_bits, 
				   time16_width,time16_height);
    mask_pixmap = XCreateBitmapFromData(dp, DefaultRootWindow(dp),
					(char*)time16m_bits,time16_width,
					time16_height);
    colors[0].pixel = BlackPixelOfScreen(DefaultScreenOfDisplay(dp));
    colors[1].pixel = WhitePixelOfScreen(DefaultScreenOfDisplay(dp));
    XQueryColors(dp, DefaultColormapOfScreen(DefaultScreenOfDisplay(dp)),
				 colors, 2);
    
    cursor = XCreatePixmapCursor(dp, pixmap, mask_pixmap, &colors[0],
				 &colors[1], time16_x_hot, time16_y_hot);
    XFreePixmap(dp, pixmap);
    XFreePixmap(dp, mask_pixmap);
    
    return(cursor);
}



void changeWaitMessage(char *str)
{
    XmString xstr;
    
    xstr = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues(wait_dialog, XmNmessageString, xstr, NULL);
    XmStringFree(xstr);
}



void waitCursor(Widget w, Boolean waiting, Boolean interruptable)
{
    XSetWindowAttributes wa;
    static Cursor cursor = 0;
    Display *dp = XtDisplay(w);
    static int locked = 0;
    XEvent event;
    XmString xstr;
    Arg arg[4];
    
    if (waiting)
	locked++;
    else
	locked--;
    
    if (((locked > 1) || (locked == 1)) && !waiting)
	return;
    
    stopped = False;
    
    if (!cursor)
	cursor = create_wait_cursor(w, True);
    wa.cursor = waiting ? cursor : None;
    XChangeWindowAttributes(dp,	XtWindow(w), CWCursor, &wa);
    XFlush(dp);
    
    if (waiting) {
	xstr = XmStringCreateLtoR((char*)"Please wait.", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(arg[0], XmNmessageString, xstr);
	XtSetArg(arg[1], XmNtitle, "Working");
	XtSetArg(arg[2], XmNmessageAlignment, XmALIGNMENT_CENTER);
	XtSetArg(arg[3], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
	wait_dialog = XmCreateWorkingDialog(w, (char*)"working", arg, 4);
	XmStringFree(xstr);
	XtUnmanageChild(XmMessageBoxGetChild(wait_dialog, XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(wait_dialog,
					     XmDIALOG_HELP_BUTTON));
	
	if (interruptable) {
	    xstr = XmStringCreateSimple((char*)"Stop");
	    XtVaSetValues(wait_dialog, XmNcancelLabelString, xstr, NULL);
	    XmStringFree(xstr);
			XtAddCallback(wait_dialog, XmNcancelCallback,
				      (XtCallbackProc)stopWaitCallback, NULL);
	} else
	    XtUnmanageChild(XmMessageBoxGetChild(wait_dialog,
												XmDIALOG_CANCEL_BUTTON));
	XtManageChild(wait_dialog);
    } else {
	while (XCheckMaskEvent(dp, ButtonPressMask | ButtonReleaseMask |
			       ButtonMotionMask | PointerMotionMask |
			       KeyPressMask, &event))
	    ;
	XtDestroyWidget(wait_dialog);
    }
}



Boolean checkForStop(Widget w)
{
    Window win = XtWindow(wait_dialog);
    Display *dp = XtDisplay(w);
    XEvent event;
    
    XFlush(dp);
    XmUpdateDisplay(w);
    while (XCheckMaskEvent(dp, ButtonPressMask | ButtonReleaseMask |
			   ButtonMotionMask | PointerMotionMask |
			   KeyPressMask | KeyReleaseMask, &event))
	if (event.xany.window == win)
	    XtDispatchEvent(&event);
    
    return(stopped);
}



void stopWaitCallback(Widget w, XtPointer client_data, XmAnyCallbackStruct *call)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    client_data=client_data;
    call=call;
    
    stopped = True;
}



void beep(Display *display)
{
    XBell(display, 10);
    XFlush(display);
}



char *getFileMsg(char *file)
{
    char *str, line[MAX_FILELINE_LEN];
    long leng = 0;
    FILE *fp;
    
    if ((fp = fopen(file, "r")) == NULL)
	return(NULL);
    
    while (fgets(line, MAX_FILELINE_LEN, fp) != NULL)
	leng += (long) strlen(line);
    
    rewind(fp);
    str = (char *) XtMalloc(sizeof(char)*leng);
    
    str[0] = 0;
    while (fgets(line, MAX_FILELINE_LEN, fp) != NULL)
	(void) strcat(str, line);
    fclose(fp);
    
    return(str);
}



Widget findTopmostShell(Widget w)
{
    Widget shell;
    
    for (shell = XtParent(w); !XtIsShell(shell); shell = XtParent(shell))
	;
    
    return(shell);
}



void popupCenterDialog(Widget top, Widget dialog, Dimension d_wdth, 
                       Dimension d_hght)
{
    Dimension x_pos, y_pos, wdth, hght;
    
    XtVaGetValues(top,
		  XmNwidth, &wdth,
		  XmNheight, &hght,
		  XmNx, &x_pos,
		  XmNy, &y_pos,
		  NULL);
    XtManageChild(dialog);
#ifdef VSUN
    XtVaGetValues(dialog,
		  XmNwidth, &d_w,
		  XmNheight, &d_h,
		  NULL);
#endif
    XtVaSetValues(dialog,
#ifdef VSUN
		  XmNx, x_pos + (wdth - d_w)/2,
		  XmNy, y_pos + (hght - d_h)/2,
#else
		  XmNx, x_pos + (wdth - d_wdth)/2,
		  XmNy, y_pos + (hght - d_hght)/2,
		  XmNwidth, d_wdth,
		  XmNheight, d_hght,
#endif
		  NULL);
    XtPopup(dialog, XtGrabExclusive);
}
