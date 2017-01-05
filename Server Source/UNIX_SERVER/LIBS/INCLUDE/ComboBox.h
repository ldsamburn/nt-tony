
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/******************************  FCP  *******************************
 **                                                                **
 **  Developed by Inter-National Research Institute, Inc. under    **
 **  contract to the Department of Defense.  All U.S. Government   **
 **  restrictions on software distributions apply.                 **
 **                                                                **
 **  Copyright 1995,1996, Inter-National Research Institute        **
 **  All rights reserved.                                          **
 **                                                                **
 **  This material may be reproduced by or for the U.S. Government **
 **  pursuant to the copyright license under the clause at         **
 **  DFARS 252.227-7013 (OCT 1988).                                **
 **                                                                **
 ********************************************************************/

  
/**
  * ComboBox.h
  *
  * Public header for XiComboBox widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/11/97: Created
  * MBS:  4/09/97: Modified to enable editing
  *
  */

#ifndef _ComboBox_h
#define _ComboBox_h

#ifdef __cplusplus
extern "C" {
#endif
  
#define PRIVATE static
#define PUBLIC
  
#define XiNselectCallback "selectCallback"
#define XiNactivateCallback "activateCallback"
#define XiNfocusInCallback "focusInCallback"
#define XiNfocusOutCallback "focusOutCallback"
#define XiNmodifyVerifyCallback "modifyVerifyCallback"
#define XiNmotionVerifyCallback "motionVerifyCallback"
#define XiNvalueChangedCallback "valueChangedCallback"
#define XiNitemCount "itemCount"
#define XiNitems "items"
#define XiNvisibleCount "visibleCount"
#define XiNfontList "fontList"
#define XiNbuttonBackground "buttonBackground"
#define XiNbuttonForeground "buttonForeground"
#define XiNeditable "editable"
#define XiNheightMargin "heightMargin"
#define XiNwidthMargin "widthMargin"

#define XiCSelectCallback "SelectCallback"
#define XiCActivateCallback "ActivateCallback"
#define XiCFocusInCallback "FocusInCallback"
#define XiCFocusOutCallback "FocusOutCallback"
#define XiCModifyVerifyCallback "ModifyVerifyCallback"
#define XiCMotionVerifyCallback "MotionVerifyCallback"
#define XiCValueChangedCallback "ValueChangedCallback"
#define XiCItemCount "ItemCount"
#define XiCItems "Items"
#define XiCVisibleCount "VisibleCount"
#define XiCFontList "FontList"
#define XiCButtonBackground "ButtonBackground"
#define XiCButtonForeground "ButtonForeground"
#define XiCEditable "Editable"
#define XiCHeightMargin "HeightMargin"
#define XiCWidthMargin "WidthMargin"

extern WidgetClass xiComboBoxWidgetClass;

typedef struct _XiComboBoxClassRec *XiComboBoxWidgetClass;
typedef struct _XiComboBoxRec      *XiComboBoxWidget;

#define XiCR_SELECT             1
#define XiCR_ACTIVATE           2
#define XiCR_FOCUSIN            3
#define XiCR_FOCUSOUT           4
#define XiCR_MODIFY_VERIFY      5
#define XiCR_MOTION_VERIFY      6
#define XiCR_VALUE_CHANGED      7


typedef struct _XiComboBoxCallbackStruct {
  int          reason;
  XEvent      *event;
  char        *label;
  int          id;
  Boolean      doit;
  int          currInsert;
  int          newInsert;
  int          startPos;
  int          len;
  char        *newText;
}XiComboBoxCallbackStruct;

/* convenience functions and macros */
#define XiIsComboBox(w)     (XtIsSubclass((w), xiComboBoxWidgetClass))

#ifdef _NO_PROTO
PUBLIC Widget   XiCreateComboBox(Widget w, char*, ...);
PUBLIC XmString XiComboBoxGetSelectedString();
PUBLIC int      XiComboBoxGetSelectedPosition();
PUBLIC XmString XiComboBoxGetStringByPosition();
PUBLIC int      XiComboBoxGetPositionByString();
PUBLIC void     XiComboBoxSelectString();
PUBLIC void     XiComboBoxSelectPosition();
PUBLIC void     XiComboBoxDeleteItemByString();
PUBLIC void     XiComboBoxDeleteItemByPosition();
PUBLIC void     XiComboBoxAddItem();
PUBLIC int      XiComboBoxGetItemCount();
PUBLIC Boolean  XiComboBoxIsEditable();
PUBLIC void     XiComboBoxSetEditable();
PUBLIC char    *XiComboBoxGetTextString();
PUBLIC void     XiComboBoxSetTextString();

#else

PUBLIC Widget   XiCreateComboBox(Widget w,
			       char *resource_name,
			       ...);      /* varargs of items to be contained */
PUBLIC XmString XiComboBoxGetSelectedString(Widget w);    
PUBLIC int      XiComboBoxGetSelectedPosition(Widget w);
PUBLIC XmString XiComboBoxGetStringByPosition(Widget w,
					      int position);
PUBLIC int      XiComboBoxGetPositionByString(Widget w,
					      XmString xms);
PUBLIC void     XiComboBoxSelectString(Widget w,
				       XmString xms,
				       Boolean notify);
PUBLIC void     XiComboBoxSelectPosition(Widget w,
					 int position,
					 Boolean notify);
PUBLIC void     XiComboBoxDeleteItemByString(Widget w,
					     XmString xms);
PUBLIC void     XiComboBoxDeleteItemByPosition(Widget w,
					       int position);
PUBLIC void     XiComboBoxAddItem(Widget w,
				  XmString xms,
				  int position);
PUBLIC int      XiComboBoxGetItemCount(Widget w);
PUBLIC Boolean  XiComboBoxIsEditable(Widget w);
PUBLIC void     XiComboBoxSetEditable(Widget w, 
				      Boolean state);
PUBLIC char    *XiComboBoxGetTextString(Widget w);
PUBLIC void     XiComboBoxSetTextString(Widget w, char *txt, Boolean notify);
#endif

#ifdef __cplusplus
}
#endif

#endif



