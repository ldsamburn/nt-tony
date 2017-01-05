
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Menus.h
 */

#ifndef	_Menus_h
#define	_Menus_h

/** TYPEDEF DEFINITIONS **/

typedef struct _menu_item {
	char *label;
	WidgetClass *class;
	char mnemonic;
	char *accelerator;
	char *accel_text;
	int precede_w_separator;
	void (*callback)(Widget, int);
	XtPointer callback_data;
	struct _menu_item *subitems;
} MenuItem;

/** GLOBAL DECLARATIONS **/

/** Menus.c **/

extern Widget BuildPulldownMenu(Widget, const char*, char, MenuItem*);

#endif /* _Menus_h */
