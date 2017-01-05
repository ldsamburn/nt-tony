
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
  
/**
  * ComboBoxP.h
  *
  * Private header for XiComboBox widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/11/97: Created
  * MBS:  4/09/97: Modified to enable editing
  *
  */

#ifndef _ComboBoxP_h
#define _ComboBoxP_h
 

#include <Xm/PrimitiveP.h>
#include "ComboBox.h"

#define MAX_STRING_LEN 512

/* class structures */
typedef struct _XiComboBoxClassPart {
  int ignore;
}XiComboBoxClassPart;

typedef struct _XiComboBoxClassRec {
  CoreClassPart          core_class;
  XmPrimitiveClassPart   primitive_class;
  XiComboBoxClassPart    combo_box_class;
}XiComboBoxClassRec;

extern XiComboBoxClassRec xiComboBoxClassRec;


/* instance structures */
typedef struct _XiComboBoxPart {
  /* public stuff */
  XtCallbackList   selectCallback;
  XtCallbackList   activateCallback;
  XtCallbackList   focusInCallback;
  XtCallbackList   focusOutCallback;
  XtCallbackList   modifyVerifyCallback;
  XtCallbackList   motionVerifyCallback;
  XtCallbackList   valueChangedCallback;
  int              itemCount;
  XmString        *items;
  int              visibleCount;
  XmFontList       fontList;
  Pixel            buttonBackground;
  Pixel            buttonForeground;
  Boolean          editable;
  Dimension        heightMargin;
  Dimension        widthMargin;
  
  /* private stuff */  
  Display         *dp;
  Dimension        width;
  Dimension        height;
  XRectangle       textRec;
  XPoint           textTopShadow[6];
  XPoint           textBottomShadow[6];
  XPoint           buttonTopShadow[6];
  XPoint           buttonBottomShadow[6];
  XPoint           triangleUp[3];
  XPoint           triangleDown[3];
  XSegment         cursorSegments[3];
  int              ascent;
  int              descent;
  GC               textGC;
  GC               buttonGC;
  GC               cursorGC;
  GC               cursorStippleGC;
  GC               buttonTopShadowGC;
  GC               buttonBottomShadowGC;
  char             text[MAX_STRING_LEN];
  int              len;
  int              headPos;
  int              hlStart;
  int              hllen;
  int              insertPos;
  XmString        *internalItems;
  Dimension        maxWidth;
  Dimension        maxHeight;
  Widget           popupShell;
  Widget           form;    
  Widget           list;
  Boolean          buttonUp;
  Boolean          arrowUp;
  Boolean          cursorVisible;
  XtIntervalId     timerId;
  XiComboBoxCallbackStruct cbs;
}XiComboBoxPart;

typedef struct _XiComboBoxRec {
  CorePart           core;
  XmPrimitivePart    primitive;
  XiComboBoxPart     combo_box;
}XiComboBoxRec;

#endif  /* _ComboBoxP.h */
