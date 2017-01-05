
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Menus.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>

/** INRI INCLUDES **/

#include <Menus.h>


/** FUNCTION PROTOTYPES **/

Widget BuildPulldownMenu(Widget, const char*, char, MenuItem*);


/** FUNCTION DEFINITIONS **/

Widget BuildPulldownMenu(Widget parent, const char* menu_title, 
                         char menu_mnemonic, MenuItem* items)
{
    Widget PullDown, cascade, widget;
    int i;
    XmString str;
    
    PullDown = XmCreatePulldownMenu(parent, (char*)"_pulldown", NULL, 0);
    
    str = XmStringCreateSimple((char*)menu_title);
    cascade = XtVaCreateManagedWidget(menu_title,
				      xmCascadeButtonGadgetClass, parent,
				      XmNsubMenuId, PullDown,
				      XmNlabelString, str,
				      XmNmnemonic, menu_mnemonic,
				      NULL);
    XmStringFree(str);
    
    for (i = 0; items[i].label != NULL; i++) {
	if (items[i].precede_w_separator)
	    (void) XtVaCreateManagedWidget("separator",
					   xmSeparatorGadgetClass, 
					   PullDown, NULL);
	
	if (items[i].subitems)
	    widget = BuildPulldownMenu(PullDown, items[i].label,
				       items[i].mnemonic, items[i].subitems);
	else
	    widget = XtVaCreateManagedWidget(items[i].label, *items[i].class,
					     PullDown, NULL);
	
	if (items[i].mnemonic)
	    XtVaSetValues(widget, XmNmnemonic, items[i].mnemonic, NULL);
	
	if (items[i].accelerator) {
	    str = XmStringCreateSimple(items[i].accel_text);
	    XtVaSetValues(widget,
			  XmNaccelerator, items[i].accelerator,
			  XmNacceleratorText, str,
			  NULL);
			XmStringFree(str);
	}
	
	if (items[i].class == &xmToggleButtonGadgetClass ||
	    items[i].class == &xmToggleButtonWidgetClass) {
	    XtVaSetValues(widget,
			  XmNindicatorSize, 10,
			  XmNindicatorOn, True,
			  XmNfillOnSelect, True,
			  XmNvisibleWhenOff, True,
			  NULL);
	    if ((int) items[i].callback_data)
		XtVaSetValues(widget, XmNset, True, NULL);
	    else
		XtVaSetValues(widget, XmNset, False, NULL);
	}
	
	if (items[i].callback) {
	    if (items[i].class == &xmToggleButtonGadgetClass ||
		items[i].class == &xmToggleButtonWidgetClass)
		XtAddCallback(widget, XmNvalueChangedCallback,
			      (XtCallbackProc)items[i].callback, 
			      items[i].callback_data);
	    else
		XtAddCallback(widget, XmNactivateCallback,
			      (XtCallbackProc)items[i].callback, 
			      items[i].callback_data);
	}
    }
    
    return(cascade);
}
