
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
-------------------------------------------------------------------------------
--
--  NAME           : ButtonBox.h
--
--  DESCRIPTION    : Public stuff for XiButtonBox widget.
--                   
--  AUTHOR(S)      : INRI-CH (dey)
--
--  CREATION DATE  : Mar 22, 1993
--
-------------------------------------------------------------------------------
*/

#ifndef _XiButtonBox_h
#define _XiButtonBox_h

#include <Xm/Xm.h>

#define XiNuniformButtons		"uniformButtons"
#define XiNactivateCallback		"activateCallback"
#define XiNspacing				"spacing"

#define XiCUniformButtons		"UniformButtons"
#define XiCActivateCallback		"ActivateCallback"
#define XiCSpacing				"Spacing"

extern WidgetClass xiButtonBoxWidgetClass;

typedef struct _XiButtonBoxClassRec *XiButtonBoxWidgetClass;
typedef struct _XiButtonBoxRec      *XiButtonBoxWidget;

typedef struct _XiButtonBoxCallbackStruct {
	int reason;
	XEvent *event;
	Widget button;
	int id;
	XmPushButtonCallbackStruct *callback;
} XiButtonBoxCallbackStruct;

#define XiIsButtonBox(w)	(XtIsSubclass((w), xiButtonBoxWidgetClass))

extern Widget XiCreateButtonBox(Widget, char*, XtCallbackProc, XtPointer, ...);
extern Widget XiButtonBoxGetButton(Widget, int );

#endif /* _XiButtonBox_h */

/* DON'T ADD STUFF AFTER THIS #endif */
