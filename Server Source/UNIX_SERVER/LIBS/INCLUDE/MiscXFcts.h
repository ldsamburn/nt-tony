
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * MiscXFcts.h
 */

#ifndef	_MiscXFcts_h
#define	_MiscXFcts_h

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <X11/cursorfont.h>

/** SYSTEM INCLUDES **/

#include <errno.h>


/** DEFINE DEFINITIONS **/

#define SAVECONTEXT(w, id, d)	(XSaveContext(XtDisplay((w)), XtWindow((w)), \
												(id), (caddr_t)(d)))
#define FINDCONTEXT(w, id, d)	(XFindContext(XtDisplay((w)), XtWindow((w)), \
												(id), (XtPointer)(d)))

#define	NO_PROB		0


/** TYPEDEF DEFINITIONS **/

typedef enum _DialogType { WARNING, INFO, WORKING, ERROR, QUESTION } DialogType;

typedef int Problem;

typedef struct _problem_item {
	Problem number;
	char *msg;
} ProblemItem;


/** GLOBAL DECLARATIONS **/

/** MiscXFcts.c **/

extern XPointer convertColor(Widget, char*);
extern void exitApp(int);
extern void popupDialogBox(DialogType, Boolean, char*, Boolean, Boolean,
                           Boolean, void(*)(Widget, XtPointer, 
                           XmAnyCallbackStruct*), Widget);
extern void defaultDialogCallback(Widget, XtPointer, XmAnyCallbackStruct*);
extern void defaultExitCallback(Widget, XtPointer, XmAnyCallbackStruct*);
extern void displayProblem(DialogType type, Problem num, 
                           ProblemItem* problem_array, Widget shell, ...);
extern void changeWaitMessage(char*);
extern void waitCursor(Widget, Boolean, Boolean);
extern Boolean checkForStop(Widget);
extern void stopWaitCallback(Widget, XtPointer, XmAnyCallbackStruct*);
extern void beep(Display*);
extern char *getFileMsg(char*);
extern Widget findTopmostShell(Widget);
extern void popupCenterDialog(Widget, Widget, Dimension, Dimension);

#endif /* _MiscXFcts_h */
