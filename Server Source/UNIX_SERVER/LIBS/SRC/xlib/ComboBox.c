/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
  
/**
  * ComboBox.c
  *
  * Implementation of XiComboBox widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/11/97: Created
  * MBS:  4/09/97: Modified to enable editing
  * MBS:  5/30/97: Various bug fixes
  * WLB:  5/30/97: Integration of 5.0.2
  * MBS:  7/20/97: Added test to see if widget is realized before drawing op
  * MBS:  7/27/97: Integration of 4.2.1
  */

#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

#include <X11/keysym.h>

#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/List.h>

#include "ComboBoxP.h"

#ifndef MAX
#define MAX(a, b) ((a) >= (b) ? (a):(b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a):(b))
#endif

#define XICOMBOBOX_DEFAULT_VISIBLE_COUNT 1
#define XICOMBOBOX_DEFAULT_HEIGHT_MARGIN 5
#define XICOMBOBOX_DEFAULT_WIDTH_MARGIN  5
#define XICOMBOBOX_CURSOR_BLINK_RATE     (unsigned long)500

#define offset(field)   XtOffsetOf(XiComboBoxRec, combo_box.field)

PRIVATE XtResource resources[] = {
  {
    (char*)XiNselectCallback,
    (char*)XiCSelectCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(selectCallback),  
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNactivateCallback,
    (char*)XiCActivateCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(activateCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNfocusInCallback,
    (char*)XiCFocusInCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(focusInCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNfocusOutCallback,
    (char*)XiCFocusOutCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(focusOutCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNmodifyVerifyCallback,
    (char*)XiCModifyVerifyCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(modifyVerifyCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNmotionVerifyCallback,
    (char*)XiCMotionVerifyCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(motionVerifyCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNvalueChangedCallback,
    (char*)XiCValueChangedCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(valueChangedCallback),
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNitemCount,
    (char*)XiCItemCount,
    XmRInt,
    sizeof(int),
    offset(itemCount),
    XmRImmediate,
    (XtPointer)0, 
  },
  {
    (char*)XiNitems,
    (char*)XiCItems,
    XmRStringArray,
    sizeof(String *),
    offset(items),
    XmRImmediate,
    (XtPointer)NULL,
  },
  {
    (char*)XiNvisibleCount,
    (char*)XiCVisibleCount,
    XmRInt,
    sizeof(int),
    offset(visibleCount),
    XmRImmediate,
    (XtPointer)XICOMBOBOX_DEFAULT_VISIBLE_COUNT,
  },
  {
    (char*)XiNfontList,
    (char*)XiCFontList,
    XmRFontList,
    sizeof(XmFontList),
    offset(fontList),
    XmRString,
    (XtPointer)XmSTRING_DEFAULT_CHARSET,
  },
  {
    (char*)XiNbuttonBackground,
    (char*)XiCButtonBackground,
    XmRPixel,
    sizeof(Pixel),
    offset(buttonBackground),
    XmRImmediate,
    (XtPointer)0,
  },
  {
    (char*)XiNbuttonForeground,
    (char*)XiCButtonForeground,
    XmRPixel,
    sizeof(Pixel),
    offset(buttonForeground),
    XmRImmediate,
    (XtPointer)0,
  },
  {
    (char*)XiNeditable,
    (char*)XiCEditable,
    XmRBoolean,
    sizeof(Boolean),
    offset(editable),
    XmRImmediate,
    (XtPointer)False,
  },
  {
    (char*)XiNheightMargin,
    (char*)XiCHeightMargin,
    XmRDimension,
    sizeof(Dimension),
    offset(heightMargin),
    XmRImmediate,
    (XtPointer)XICOMBOBOX_DEFAULT_HEIGHT_MARGIN,
  },
  {
    (char*)XiNwidthMargin,
    (char*)XiCWidthMargin,
    XmRDimension,
    sizeof(Dimension),
    offset(widthMargin),
    XmRImmediate,
    (XtPointer)XICOMBOBOX_DEFAULT_WIDTH_MARGIN,
  },
};



/* widget class methods */
PRIVATE void Realize(Widget w,XtValueMask *value_mask, XSetWindowAttributes *attrib);
PRIVATE void Initialize(Widget req, Widget new, ArgList  args, Cardinal *num_args);
PRIVATE void Resize(Widget w);
PRIVATE void Destroy(Widget w);
PRIVATE Boolean SetValues(Widget current, Widget request, Widget new, ArgList  args,
			  Cardinal *num_args);
PRIVATE XtGeometryResult QueryGeometry(Widget w, XtWidgetGeometry *intended,
				       XtWidgetGeometry *reply);
PRIVATE void Redisplay(Widget w, XEvent *event, Region region);

/* internal functions */
PRIVATE void ResetCBS(XiComboBoxWidget cb);
PRIVATE int  GetSelectedListItemId(XiComboBoxWidget cb);
PRIVATE void DoCursorBlink(XtPointer clientData,  XtIntervalId *id);
PRIVATE int  InsertPosToAbsolutePos(XiComboBoxWidget cb, int x);
PRIVATE int  AbsolutePosToInsertPos(XiComboBoxWidget cb, int x);
PRIVATE void GetGeometryInfo(XiComboBoxWidget cb);
PRIVATE void DeleteString(XiComboBoxWidget cb, int start, int len);
PRIVATE void InsertString(XiComboBoxWidget cb, char *str);
PRIVATE Boolean IsInButton(XiComboBoxWidget cb, int x, int y);
PRIVATE void DrawTextShadow(XiComboBoxWidget cb);
PRIVATE void DrawButton(XiComboBoxWidget cb);
PRIVATE void DrawText(XiComboBoxWidget cb);
PRIVATE void DrawCursor(XiComboBoxWidget cb);
PRIVATE void KeyHandler(Widget w, XEvent *event, String *param, Cardinal *numParam);
PRIVATE void ButtonPressHandler(Widget w, XEvent *event, String *param, 
				Cardinal *numParam);
PRIVATE void ButtonReleaseHandler(Widget w, XEvent *event, String *param,
				  Cardinal *numParam);
PRIVATE void ButtonMotionHandler(Widget w, XEvent *event, String *param,
				 Cardinal *numParam);
PRIVATE void FocusHandler(Widget w, XEvent *event, String *param, 
			  Cardinal *numParam); 
PRIVATE void ReturnKeyHandler(Widget w, XEvent *event, String *param,
			     Cardinal *numParam);
PRIVATE void TabKeyHandler(Widget w, XEvent *event, String *param,
			   Cardinal *numParam);
PRIVATE void ListCallbackProc(Widget w, XtPointer client_data, XtPointer call_data);
PRIVATE void PopupEventHandler(Widget w, XtPointer client_data, XEvent *event,
			       Boolean *cont);
PRIVATE void Popup(XiComboBoxWidget cb);
PRIVATE void Popdown(XiComboBoxWidget cb);
PRIVATE void      FreeItems(XmString *items,  int n);
PRIVATE XmString *CopyItems(XmString *items,  int n);
PRIVATE void      AddItem(XmString **items, XmString item, int curr_count,  int pos);
PRIVATE void      DeleteItem(XmString **items, XmString item,  int curr_count);
PRIVATE Dimension GetMaxWidth(XiComboBoxWidget cb);
PRIVATE Dimension GetMaxHeight(XiComboBoxWidget cb);


PRIVATE char defaultTranslations[] = 
"<BtnDown>:       ButtonPressHandler()      \n\
<BtnUp>:          ButtonReleaseHandler()    \n\
<BtnMotion>:      ButtonMotionHandler()     \n\
<FocusIn>:        FocusHandler()            \n\
<FocusOut>:       FocusHandler()            \n\
<Key>Return:      ReturnKeyHandler()        \n\
<Key>Tab:         TabKeyHandler()           \n\
<Key>:            KeyHandler()              \n\
<EnterWindow>:    PrimitiveEnter()          \n\
<LeaveWindow>:    PrimitiveLeave()";


PRIVATE XtActionsRec actions[] = {
  {(char*)"KeyHandler",           (XtActionProc)KeyHandler},
  {(char*)"ButtonPressHandler",   (XtActionProc)ButtonPressHandler},
  {(char*)"ButtonReleaseHandler", (XtActionProc)ButtonReleaseHandler},
  {(char*)"ButtonMotionHandler",  (XtActionProc)ButtonMotionHandler},
  {(char*)"FocusHandler",         (XtActionProc)FocusHandler},
  {(char*)"ReturnKeyHandler",     (XtActionProc)ReturnKeyHandler},
  {(char*)"TabKeyHandler",        (XtActionProc)TabKeyHandler},
};

XiComboBoxClassRec xiComboBoxClassRec = { 
  {
    /* core class members */
    (WidgetClass)&xmPrimitiveClassRec,      /* super class                 */
    (char*)"XiComboBox",                    /* class name                  */
    sizeof(XiComboBoxRec),                  /* widget size                 */
    NULL,                                   /* class_initialize            */
    NULL,                                   /* class_part_init             */
    False,                                  /* class_inited                */
    Initialize,                             /* initialize method           */
    NULL,                                   /* initialize_hook             */
    Realize,                                /* realize method              */
    actions,                                /* actions                     */
    XtNumber(actions),                      /* number of actions           */
    resources,                              /* resources                   */
    XtNumber(resources),                    /* number of resources         */
    NULLQUARK,                              /* xrm_class                   */
    True,                                   /* compress_motion             */
    XtExposeCompressMaximal,                /* compress_exposure           */
    True,                                   /* compress_enterleave         */
    False,                                  /* visible_interest            */
    Destroy,                                /* destroy method              */
    Resize,                                 /* resize method               */
    Redisplay,                              /* expose method               */
    SetValues,                              /* set_values method           */
    NULL,                                   /* set_values_hook             */
    XtInheritSetValuesAlmost,               /* set_values_almost method    */
    NULL,                                   /* get_values_hook             */
    NULL,                                   /* accept_focus method         */
    XtVersion,                              /* version                     */
    NULL,                                   /* callback_private            */
    defaultTranslations,                    /* tm_table                    */
    QueryGeometry,                          /* query_geometry method       */
    NULL,                                   /* display_accelerator         */
    NULL,                                   /* extension                   */
  },
  {
    NULL,                                   /* border_highlight            */
    NULL,                                   /* border_unhightlight         */
    NULL,                                   /* translations                */
    NULL,                                   /* arm_and_activate            */
    NULL,                                   /* syn_resources               */
    0,                                      /* num_syn_resources           */
    NULL,                                   /* extension                   */
  },
  {
    /* tab_box_class member */
    0,                                      /* ignore                      */
  },
};

PUBLIC  WidgetClass xiComboBoxWidgetClass = (WidgetClass) &xiComboBoxClassRec;
PRIVATE XmPrimitiveWidgetClass sclass = (XmPrimitiveWidgetClass)&xmPrimitiveClassRec;


/*
 * Initialize()
 * method to initialize instance record
 * Called in response to Xt[Va]Create[Managed]Widget
 */
PRIVATE void Initialize(Widget   req, Widget   new, ArgList  args, 
			Cardinal *num_args)
{
    XiComboBoxWidget cb = (XiComboBoxWidget) new;
    Arg al[2];
    XmFontContext fontContext = NULL;
    XmFontListEntry flEntry = NULL;
    XmFontType retType = (XmFontType)NULL;
    XtPointer retFont = NULL;
    Font font = (Font)NULL;
    XFontStruct **fontStruct = NULL;
    char **fontName = NULL;
    XGCValues val;
    Dimension desiredHeight;
    Dimension desiredWidth;
    int scrNum;
    Screen *screen;
    Window rootWindow;
    
    /* done to get rid of compiler warning about unused parameters */
    req=req;
    args=args;
    num_args=num_args;
    
    if (cb->combo_box.itemCount < 0) {
	XtWarning("XiComboBox:Initialize:XiNitemCount must be greater than or equal to 0");
	cb->combo_box.itemCount = 0;
    }
    if ((cb->combo_box.itemCount > 0) && (cb->combo_box.items == NULL)) {
	XtWarning("XiComboBox:Initialize:XiNitemCount is greater than 0, but items is NULL");
	cb->combo_box.itemCount = 0;
    }
    if (cb->combo_box.visibleCount < 1) {
	XtWarning("XiComboBox:Initialize:XiNvisibleCount must be greater than 0");
	cb->combo_box.visibleCount = XICOMBOBOX_DEFAULT_VISIBLE_COUNT;
    }
    /* commented out since data type never allows for a value less than 0
       if (cb->combo_box.heightMargin < 0) {
       XtWarning("XiComboBox:Initialize:XiNheightMargin must be greater than or equal to 0");
       cb->combo_box.heightMargin = XICOMBOBOX_DEFAULT_HEIGHT_MARGIN;
       } */
    /* commented out since data type never allows for a value less than 0 
       if (cb->combo_box.widthMargin < 0) {
       XtWarning("XiComboBox:Initialize:XiNwidthMargin must be greater than or equal to 0");
       cb->combo_box.widthMargin = XICOMBOBOX_DEFAULT_WIDTH_MARGIN;
       } */
    if (cb->combo_box.buttonBackground <= (Pixel)0) {
	cb->combo_box.buttonBackground = (Pixel)cb->core.background_pixel;
    }
    if (cb->combo_box.buttonForeground <= (Pixel)0) {
	cb->combo_box.buttonForeground = (Pixel)cb->primitive.foreground;
    }
    
    cb->combo_box.dp = XtDisplay(new);
    scrNum = DefaultScreen(cb->combo_box.dp);
    screen = ScreenOfDisplay(cb->combo_box.dp, scrNum);
    rootWindow = RootWindow(cb->combo_box.dp, scrNum);
    
    if (cb->core.width <= 0)
	cb->core.width = 5;
    if (cb->core.height <= 0)
	cb->core.height = 5;
    
    memset(cb->combo_box.text, 0, MAX_STRING_LEN);
    
    cb->combo_box.len = 0;
    cb->combo_box.headPos = 0;
    cb->combo_box.hlStart = 0;
    cb->combo_box.hllen = 0;
    cb->combo_box.insertPos = 0;   /* index */
    cb->combo_box.internalItems = NULL;
    cb->combo_box.maxWidth = (Dimension)0;
    cb->combo_box.maxHeight = (Dimension)0;
    cb->combo_box.buttonUp = True;
    cb->combo_box.arrowUp = False;
    cb->combo_box.cursorVisible = False;
    cb->combo_box.timerId = (XtIntervalId)0;
    
    if (cb->combo_box.itemCount) {
	cb->combo_box.internalItems = CopyItems(cb->combo_box.items, 
						cb->combo_box.itemCount);
	cb->combo_box.items = cb->combo_box.internalItems;
    }
    
    /* get default font for textGC */
    if (!XmFontListInitFontContext(&fontContext, cb->combo_box.fontList))
	XtWarning("XiComboBox:Initialize:Cannot initialize fontContext");
    else if ((flEntry = XmFontListNextEntry(fontContext)) == NULL) 
	XtWarning("XiComboBox:Initialize:Cannot get FontListEntry");
    else if ((retFont = XmFontListEntryGetFont(flEntry, &retType)) == NULL)
	XtWarning("XiComboBox:Initialize:Cannot get font from the FontListEntry");
    else if (retType == XmFONT_IS_FONT) {
	font = ((XFontStruct *)retFont)->fid;
	cb->combo_box.ascent = ((XFontStruct *)retFont)->ascent;
	cb->combo_box.descent = ((XFontStruct *)retFont)->descent;
    }
    else if (XFontsOfFontSet((XFontSet)retFont, &fontStruct, &fontName) < 1)
	XtWarning("XiComboBox:Initialize:Cannot get Fonts of FontSet");
    else {
	font = fontStruct[0]->fid;
	cb->combo_box.ascent = fontStruct[0]->ascent;
	cb->combo_box.descent = fontStruct[0]->descent;
    }
    /* now free fontContext */
    if (fontContext)
	XmFontListFreeFontContext(fontContext);
    
    /* create textGC */
    val.foreground = cb->primitive.foreground;
    val.background = cb->core.background_pixel;
    cb->combo_box.textGC = XCreateGC(cb->combo_box.dp, rootWindow, GCForeground | 
				     GCBackground, &val);
    if (font)
	XSetFont(cb->combo_box.dp, cb->combo_box.textGC, font);
    
    /* create buttonGC */
    val.foreground = cb->combo_box.buttonForeground;
    val.background = cb->combo_box.buttonBackground;
    cb->combo_box.buttonGC = XCreateGC(cb->combo_box.dp, rootWindow, GCForeground |
				       GCBackground, &val);
    
    /* create buttonTopShadowGC and buttonBottomShadowGC */
    {
	Pixel fg, ts, bs, sel;
	XmGetColors(screen,
		    cb->core.colormap,
		    cb->combo_box.buttonBackground,
		    &fg, &ts, &bs, &sel);
	val.foreground = ts;
	cb->combo_box.buttonTopShadowGC = XCreateGC(cb->combo_box.dp, rootWindow,
						    GCForeground, &val);
	val.foreground = bs;
	cb->combo_box.buttonBottomShadowGC = XCreateGC(cb->combo_box.dp, rootWindow,
						       GCForeground, &val);
    }
    
    /* create cursorGC */
    val.foreground = cb->primitive.foreground;
    val.background = cb->core.background_pixel;
    val.function = GXxor;
    val.line_width = 1;
    cb->combo_box.cursorGC = XCreateGC(cb->combo_box.dp, rootWindow, GCForeground |
				       GCBackground | GCFunction | GCLineWidth,&val);
    
    /* create stippleGC */
    cb->combo_box.cursorStippleGC = NULL;
    
    cb->combo_box.maxWidth = GetMaxWidth(cb);
    cb->combo_box.maxHeight = GetMaxHeight(cb);  
    
    /* now set desired width and height */
    desiredHeight = (Dimension)(cb->combo_box.ascent  + cb->combo_box.descent) + 
	(cb->combo_box.heightMargin * 2) +
	(cb->primitive.shadow_thickness * 2) +
	(cb->primitive.highlight_thickness * 2);
    if (cb->core.height < desiredHeight)
	cb->core.height = desiredHeight;
    desiredWidth = cb->combo_box.maxWidth + 
	desiredHeight + 
	(cb->primitive.highlight_thickness * 2) +
	(cb->primitive.shadow_thickness * 2) +
	(cb->combo_box.widthMargin * 2);
    
    if (cb->core.width < desiredWidth)
	cb->core.width = desiredWidth;
    
    cb->combo_box.popupShell = XtVaCreatePopupShell("XiComboBoxPopupShell",
						    overrideShellWidgetClass,
						    new, 
						    XmNallowShellResize,    False,
						    XmNbackground,
						    cb->core.background_pixel,
						    NULL);
    cb->combo_box.form = XtVaCreateManagedWidget("XiComboBoxForm", xmFormWidgetClass,
						 cb->combo_box.popupShell,
						 XmNborderWidth,    0,
						 NULL);
    XtAddEventHandler(cb->combo_box.form, ButtonPressMask | ButtonReleaseMask | 
		      EnterWindowMask | LeaveWindowMask, 
		      False, PopupEventHandler, (XtPointer)cb);
    
    XtSetArg(al[0], XmNlistSizePolicy,         XmRESIZE_IF_POSSIBLE); 
    XtSetArg(al[1], XmNfontList,               cb->combo_box.fontList);
    cb->combo_box.list = XmCreateScrolledList(cb->combo_box.form,
					      (char*)"XiComboBoxList", al, 2);
    if (cb->combo_box.itemCount > 0)
	XtVaSetValues(cb->combo_box.list,
		      XmNitemCount,  cb->combo_box.itemCount,
		      XmNitems,      cb->combo_box.items,
		      NULL);
    XtAddCallback(cb->combo_box.list, XmNbrowseSelectionCallback, ListCallbackProc,
		  (XtPointer)cb);
    XtManageChild(cb->combo_box.list);
    XtVaSetValues(XtParent(cb->combo_box.list),
		  XmNtopAttachment,    XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment,   XmATTACH_FORM, 
		  XmNrightAttachment,  XmATTACH_FORM,
		  XmNtopOffset,        0,
		  XmNbottomOffset,     0,
		  XmNleftOffset,       0,
		  XmNrightOffset,      0,
		  NULL);
    
    
    cb->primitive.navigation_type = XmTAB_GROUP;
    
    /* now initialize callback struct */
    cb->combo_box.cbs.reason = 0;
    cb->combo_box.cbs.event = NULL;
    cb->combo_box.cbs.label = NULL;
    cb->combo_box.cbs.id = 0;
    cb->combo_box.cbs.doit = True;
    cb->combo_box.cbs.currInsert = 0;
    cb->combo_box.cbs.newInsert = 0;
    cb->combo_box.cbs.startPos = 0;
    cb->combo_box.cbs.len = 0;
    cb->combo_box.cbs.newText = NULL;
}
  
	
/*
 * Realize()
 * realize method
 */
PRIVATE void Realize(Widget w,
		     XtValueMask *value_mask,
		     XSetWindowAttributes *attrib)
{
    /*XiComboBoxWidget cb = (XiComboBoxWidget)w;*/
    
    (*sclass->core_class.realize)(w, value_mask, attrib);
}



PRIVATE void Redisplay(Widget w,
		       XEvent *event,
		       Region region)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    /* done to get rid of compiler warning about unused parameters */
    event=event;
    region=region;
    
    GetGeometryInfo(cb);
    
    DrawTextShadow(cb);
    DrawButton(cb);
    DrawText(cb);
    DrawCursor(cb);
    if (XmGetFocusWidget(w) == w)
	(*sclass->primitive_class.border_highlight)(w);
}



/*
 * Resize()
 * resize method
 * Called in response to resize event
 */
PRIVATE void Resize(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget) w;
    
    GetGeometryInfo(cb);
} 


/*
 * Destroy()
 * Called when this widget is destroyed
 * This will free up any allocated memory
 */
PRIVATE void Destroy(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    if (cb->combo_box.selectCallback)
	XtFree((char *)cb->combo_box.selectCallback);
    if (cb->combo_box.activateCallback)
	XtFree((char *)cb->combo_box.activateCallback);
    if (cb->combo_box.focusInCallback)
	XtFree((char *)cb->combo_box.focusInCallback);
    if (cb->combo_box.focusOutCallback)
	XtFree((char *)cb->combo_box.focusOutCallback);
    if (cb->combo_box.modifyVerifyCallback)
	XtFree((char *)cb->combo_box.modifyVerifyCallback);
    if (cb->combo_box.motionVerifyCallback)
	XtFree((char *)cb->combo_box.motionVerifyCallback);
    if (cb->combo_box.valueChangedCallback)
	XtFree((char *)cb->combo_box.valueChangedCallback);
    if (cb->combo_box.textGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.textGC);
    if (cb->combo_box.buttonGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.buttonGC);
    if (cb->combo_box.cursorGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.cursorGC);
    if (cb->combo_box.cursorStippleGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.cursorStippleGC);
    if (cb->combo_box.buttonTopShadowGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.buttonTopShadowGC);
    if (cb->combo_box.buttonBottomShadowGC)
	XFreeGC(cb->combo_box.dp, cb->combo_box.buttonBottomShadowGC);
    FreeItems(cb->combo_box.internalItems, cb->combo_box.itemCount);
    if (cb->combo_box.fontList)
	XmFontListFree(cb->combo_box.fontList);
    XtDestroyWidget(cb->combo_box.popupShell);
    XtDestroyWidget(cb->combo_box.form);
    XtDestroyWidget(cb->combo_box.list);
}


/*
 * SetValues()
 * Called in response to Xt[Va]SetValues()
 */
PRIVATE Boolean SetValues(Widget   current, Widget   request, Widget   new,
			  ArgList  args, Cardinal *num_args)
{
    XiComboBoxWidget curr_cb = (XiComboBoxWidget)current;
    XiComboBoxWidget new_cb = (XiComboBoxWidget)new;
    Boolean redraw = False;
    
    /* done to get rid of compiler warning about unused parameters */
    request=request;
    args=args;
    num_args=num_args;
    
    if (curr_cb->combo_box.items != new_cb->combo_box.items) {
	FreeItems(curr_cb->combo_box.internalItems, curr_cb->combo_box.itemCount);
	new_cb->combo_box.internalItems = CopyItems(new_cb->combo_box.items, 
						    new_cb->combo_box.itemCount);
	new_cb->combo_box.items = new_cb->combo_box.internalItems;
	XtVaSetValues(new_cb->combo_box.list,
		      XmNitemCount,  new_cb->combo_box.itemCount,
		  XmNitems,      new_cb->combo_box.items,
		      NULL);
	new_cb->combo_box.maxWidth = GetMaxWidth(new_cb);
	new_cb->combo_box.maxHeight = GetMaxHeight(new_cb);
	redraw = True;
    }
    if (curr_cb->combo_box.visibleCount != new_cb->combo_box.visibleCount) {
	XtVaSetValues(new_cb->combo_box.list,
		      XmNvisibleItemCount,       new_cb->combo_box.visibleCount,
		      NULL);
    }
    if (curr_cb->combo_box.fontList != new_cb->combo_box.fontList) {
	XmFontContext fontContext = NULL;
	XmFontListEntry flEntry = NULL;
	XmFontType retType = (XmFontType)NULL;
	XtPointer retFont = NULL;
	Font font = (Font)NULL;
	XFontStruct **fontStruct = NULL;
	char **fontName = NULL;
        
	if (!XmFontListInitFontContext(&fontContext, new_cb->combo_box.fontList))
	    XtWarning("XiComboBox:SetValues:Cannot initialize fontContext");
	else if ((flEntry = XmFontListNextEntry(fontContext)) == NULL) 
	    XtWarning("XiComboBox:SetValues:Cannot get FontListEntry");
	else if ((retFont = XmFontListEntryGetFont(flEntry, &retType)) == NULL)
	    XtWarning("XiComboBox:SetValues:Cannot get font from the FontListEntry");
	else if (retType == XmFONT_IS_FONT) {
	    font = ((XFontStruct *)retFont)->fid;
	    new_cb->combo_box.ascent = ((XFontStruct *)retFont)->ascent;
	    new_cb->combo_box.descent = ((XFontStruct *)retFont)->descent;
	}
	else if (XFontsOfFontSet((XFontSet)retFont, &fontStruct, &fontName) < 1)
	    XtWarning("XiComboBox:SetValues:Cannot get Fonts of FontSet");
	else {
	    font = fontStruct[0]->fid;
	    new_cb->combo_box.ascent = fontStruct[0]->ascent;
	    new_cb->combo_box.descent = fontStruct[0]->descent;
	}
	if (font)
	    XSetFont(new_cb->combo_box.dp, new_cb->combo_box.textGC, font);
	/* now free fontContext */
	if (fontContext)
	    XmFontListFreeFontContext(fontContext);
	
	new_cb->combo_box.maxWidth = GetMaxWidth(new_cb);
	new_cb->combo_box.maxHeight = GetMaxHeight(new_cb);
	XtVaSetValues(new_cb->combo_box.list,
		      XmNfontList,          new_cb->combo_box.fontList,
		      NULL);
	redraw = True;
    }
    if (curr_cb->core.background_pixel != new_cb->core.background_pixel) {
	XSetBackground(new_cb->combo_box.dp,
		       new_cb->combo_box.textGC,
		       (unsigned long)new_cb->core.background_pixel);
	XSetForeground(new_cb->combo_box.dp,
		       new_cb->combo_box.cursorGC,
		       (unsigned long)new_cb->core.background_pixel);
	XSetWindowBackground(new_cb->combo_box.dp,
			     new_cb->core.window,
			     (unsigned long)new_cb->core.background_pixel);
	XtVaSetValues(new_cb->combo_box.list,
		      XmNbackground,        new_cb->core.background_pixel,
		      NULL);
	redraw = True;
    }
    if (curr_cb->primitive.foreground != new_cb->primitive.foreground) {
	XSetForeground(new_cb->combo_box.dp,
		       new_cb->combo_box.textGC,
		       (unsigned long)new_cb->primitive.foreground);
	XSetBackground(new_cb->combo_box.dp,
		       new_cb->combo_box.cursorGC,
		       (unsigned long)new_cb->primitive.foreground);
	XtVaSetValues(new_cb->combo_box.list,
		      XmNforeground,        new_cb->primitive.foreground,
		      NULL);
	redraw = True;
    }
    if (curr_cb->combo_box.buttonBackground != new_cb->combo_box.buttonBackground) {
	XSetBackground(new_cb->combo_box.dp,
		       new_cb->combo_box.buttonGC,
		       (unsigned long)new_cb->combo_box.buttonBackground);
	redraw = True;
    }
    if (curr_cb->combo_box.buttonForeground != new_cb->combo_box.buttonForeground) {
	XSetForeground(new_cb->combo_box.dp,
		       new_cb->combo_box.buttonGC,
		       (unsigned long)new_cb->combo_box.buttonForeground);
	redraw = True;
    }
    if (curr_cb->combo_box.editable != new_cb->combo_box.editable) {
	redraw = True;
    }
    if (curr_cb->combo_box.heightMargin != new_cb->combo_box.heightMargin)
	redraw = True;
    
    if (curr_cb->combo_box.widthMargin != new_cb->combo_box.widthMargin) 
	redraw = True;
    
    if (redraw) {
	Dimension desiredHeight, desiredWidth;
	/* now set desired width and height */
	desiredHeight = (Dimension)(new_cb->combo_box.ascent  + 
				    new_cb->combo_box.descent) + 
	    (new_cb->combo_box.heightMargin * 2) +
	    (new_cb->primitive.shadow_thickness * 2) +
	    (new_cb->primitive.highlight_thickness * 2);
	if (new_cb->core.height < desiredHeight)
	    new_cb->core.height = desiredHeight;
	desiredWidth = new_cb->combo_box.maxWidth + 
	    desiredHeight + 
	    (new_cb->primitive.highlight_thickness * 2) +
	    (new_cb->primitive.shadow_thickness * 2) +
	    (new_cb->combo_box.widthMargin * 2);
	if (new_cb->core.width < desiredWidth)
	    new_cb->core.width = desiredWidth;
    
	GetGeometryInfo(new_cb);
    }
    if (XtIsRealized(current) && redraw) {
	DrawText(new_cb);
	DrawButton(new_cb);
	DrawCursor(new_cb);
	if (XmGetFocusWidget(new) == new)
	    (*sclass->primitive_class.border_highlight)(new);
    }
    return True;
}


/*
 * QueryGeometry()
 * QueryGeometry method
 * simpley returns XtGeometryYes
 */
PRIVATE XtGeometryResult QueryGeometry(Widget w,
				       XtWidgetGeometry *intended,
				       XtWidgetGeometry *reply)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    intended=intended;
    reply=reply;
    
    return XtGeometryYes;
}


/*************************************************************************************
 * internal functions
 ************************************************************************************/
/*
 *
 */
PRIVATE void ResetCBS(XiComboBoxWidget cb)
{
    cb->combo_box.cbs.reason = 0;
    cb->combo_box.cbs.event = NULL;
    cb->combo_box.cbs.label = NULL;
    cb->combo_box.cbs.id = 0;
    cb->combo_box.cbs.doit = True;
    cb->combo_box.cbs.currInsert = 0;
    cb->combo_box.cbs.newInsert = 0;
    cb->combo_box.cbs.startPos = 0;
    cb->combo_box.cbs.len = 0;
    if (cb->combo_box.cbs.newText)
	XtFree(cb->combo_box.cbs.newText);
    cb->combo_box.cbs.newText = NULL;   
}


/*
 *
 */
PRIVATE int  GetSelectedListItemId(XiComboBoxWidget cb)
{
    int *pos;
    int count;
    
    if (XmListGetSelectedPos(cb->combo_box.list, &pos, &count)) {
	count = pos[0];
	XtFree((char *)pos);
	return count;         /* variable recycling...  count is really the selected pos */
    }
    return 0;
}



/*
 *
 */
PRIVATE void DoCursorBlink(XtPointer clientData, 
			   XtIntervalId *id)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)clientData;
    Boolean visible = cb->combo_box.cursorVisible;
    
    /* done to get rid of compiler warning about unused parameters */
    id=id;
    
    if (!cb->combo_box.editable) 
	return;
    
    cb->combo_box.cursorVisible = False;
    DrawCursor(cb);
    if (visible)
	cb->combo_box.cursorVisible = False;
    cb->combo_box.timerId =XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)cb),
					   XICOMBOBOX_CURSOR_BLINK_RATE, 
					   DoCursorBlink, (XtPointer)cb);
}


/*
 *
 */
PRIVATE int  InsertPosToAbsolutePos(XiComboBoxWidget cb, int x)
{
    char buf[MAX_STRING_LEN];
    char *ptr;
    XmString xms;
    int pos;
    Dimension w;
    
    memset(buf, 0, MAX_STRING_LEN);
    ptr = cb->combo_box.text;
    ptr += cb->combo_box.headPos;
    strncpy(buf, ptr, x - cb->combo_box.headPos);
    xms = XmStringCreateLocalized(buf);
    w = XmStringWidth(cb->combo_box.fontList, xms);
    pos = (int)cb->primitive.highlight_thickness + 
	(int)cb->primitive.shadow_thickness + 
	(int)cb->combo_box.widthMargin + (int)w;
    XmStringFree(xms);
    return pos;
}



/*
 *
 */
PRIVATE int  AbsolutePosToInsertPos(XiComboBoxWidget cb, int x)
{
    int head = InsertPosToAbsolutePos(cb, cb->combo_box.headPos);
    int tail = InsertPosToAbsolutePos(cb, cb->combo_box.len);
    int pos;
    int i;
    
    if (x > tail)
	return cb->combo_box.len;
    if (x < head)
	return cb->combo_box.headPos;
    
    for (i = cb->combo_box.headPos; i <= cb->combo_box.len; i++) {
	pos = InsertPosToAbsolutePos(cb, i);
	if (x <= pos)
	    return i;
    }
    return -1;
}




/*
 *
 */
PRIVATE void GetGeometryInfo(XiComboBoxWidget cb)
{
    int cw = (int)cb->core.width;
    int ch = (int)cb->core.height;
    int ht = (int)cb->primitive.highlight_thickness;
    int st = (int)cb->primitive.shadow_thickness;
    int hm = (int)cb->combo_box.heightMargin;
    int cbw = cw - (ht * 2);
    int cbh = ch - (ht * 2);
    int tw = (cbw > cbh) ? (cbw - cbh):0;
    int bw = tw ? cbh:cbw;
    
    /* get textRec info */
    cb->combo_box.textRec.width = (unsigned short)(tw - (st * 2));
    cb->combo_box.textRec.height = (unsigned short)(cbh - (st * 2));
    cb->combo_box.textRec.x = (short)(ht + st);
    cb->combo_box.textRec.y = (short)(ht + st);
    /* commented out since data type never allows for a value less than 0
       if (cb->combo_box.textRec.width < 0)
       cb->combo_box.textRec.width = 0;
       if (cb->combo_box.textRec.height < 0)
       cb->combo_box.textRec.height = 0; */
    
    /* get textTopShadow points */
    if ((tw >= (st * 2)) && (cbh >= (st * 2))) {
	cb->combo_box.textTopShadow[0].x = (short)ht;
	cb->combo_box.textTopShadow[0].y = (short)(ch - ht);
	cb->combo_box.textTopShadow[1].x = (short)ht;
	cb->combo_box.textTopShadow[1].y = (short)ht;
	cb->combo_box.textTopShadow[2].x = (short)(ht + tw);
	cb->combo_box.textTopShadow[2].y = (short)ht;
	cb->combo_box.textTopShadow[3].x = (short)(ht + tw -st);
	cb->combo_box.textTopShadow[3].y = (short)(ht + st);
	cb->combo_box.textTopShadow[4].x = (short)(ht + st);
	cb->combo_box.textTopShadow[4].y = (short)(ht + st);
	cb->combo_box.textTopShadow[5].x = (short)(ht + st);
	cb->combo_box.textTopShadow[5].y = (short)(ch - ht - st);
    }
    else
	memset(cb->combo_box.textTopShadow, 0, sizeof(XPoint) * 6);
    
    /* get textBottomShadow points */
    if ((tw >= (st * 2)) && (cbh >= (st * 2))) {
	cb->combo_box.textBottomShadow[0].x = (short)ht;
	cb->combo_box.textBottomShadow[0].y = (short)(ch - ht);
	cb->combo_box.textBottomShadow[1].x = (short)(ht + tw);
	cb->combo_box.textBottomShadow[1].y = (short)(ch - ht);
	cb->combo_box.textBottomShadow[2].x = (short)(ht + tw);
	cb->combo_box.textBottomShadow[2].y = (short)ht;
	cb->combo_box.textBottomShadow[3].x = (short)(ht + tw - st);
	cb->combo_box.textBottomShadow[3].y = (short)(ht + st);
	cb->combo_box.textBottomShadow[4].x = (short)(ht + tw - st);
	cb->combo_box.textBottomShadow[4].y = (short)(ch - ht - st);
	cb->combo_box.textBottomShadow[5].x = (short)(ht + st);
	cb->combo_box.textBottomShadow[5].y = (short)(ch - ht - st);
    }
    else
	memset(cb->combo_box.textBottomShadow, 0, sizeof(XPoint) * 6);
    
    /* get buttonTopShadow points */
    if ((bw >= (st * 2)) && (cbh >= (st * 2))) {
	int offset = ht + tw;
	cb->combo_box.buttonTopShadow[0].x = (short)offset;
	cb->combo_box.buttonTopShadow[0].y = (short)(ch - ht);
	cb->combo_box.buttonTopShadow[1].x = (short)offset;
	cb->combo_box.buttonTopShadow[1].y = (short)ht;
	cb->combo_box.buttonTopShadow[2].x = (short)(offset + bw);
	cb->combo_box.buttonTopShadow[2].y = (short)ht;
	cb->combo_box.buttonTopShadow[3].x = (short)(offset + bw - st);
	cb->combo_box.buttonTopShadow[3].y = (short)(ht + st);
	cb->combo_box.buttonTopShadow[4].x = (short)(offset + st);
	cb->combo_box.buttonTopShadow[4].y = (short)(ht + st);
	cb->combo_box.buttonTopShadow[5].x = (short)(offset + st);
	cb->combo_box.buttonTopShadow[5].y = (short)(ch - ht - st);
    }
    else
	memset(cb->combo_box.buttonTopShadow, 0, sizeof(XPoint) * 6);
    
    /* get buttonBottomShadow points */
    if ((bw >= (st * 2)) && (cbh >= (st * 2))) {
	int offset = ht + tw;
	cb->combo_box.buttonBottomShadow[0].x = (short)offset;
	cb->combo_box.buttonBottomShadow[0].y = (short)(ch - ht);
	cb->combo_box.buttonBottomShadow[1].x = (short)(offset + bw);
	cb->combo_box.buttonBottomShadow[1].y = (short)(ch - ht);
	cb->combo_box.buttonBottomShadow[2].x = (short)(offset + bw);
	cb->combo_box.buttonBottomShadow[2].y = (short)ht;
	cb->combo_box.buttonBottomShadow[3].x = (short)(offset + bw - st);
	cb->combo_box.buttonBottomShadow[3].y = (short)(ht + st);
	cb->combo_box.buttonBottomShadow[4].x = (short)(offset + bw - st);
	cb->combo_box.buttonBottomShadow[4].y = (short)(ch - ht - st);
	cb->combo_box.buttonBottomShadow[5].x = (short)(offset + st);
	cb->combo_box.buttonBottomShadow[5].y = (short)(ch - ht - st);
    }
    else
	memset(cb->combo_box.buttonBottomShadow, 0, sizeof(XPoint) * 6);
    
    /* get triangleUp points */
    if (((bw - (st * 2)) >= 4) && ((cbh - (st * 2)) >= 4)) {
	int xoffset = ht + tw + st;
	int yoffset = ht + st;
	int width = bw - (st * 2);
	int height = cbh - (st * 2);
	int q = width / 4;
	int half = width / 2;

	cb->combo_box.triangleUp[0].x = (short)(xoffset + half);
	cb->combo_box.triangleUp[0].y = (short)(yoffset + q);
	cb->combo_box.triangleUp[1].x = (short)(xoffset + q);
	cb->combo_box.triangleUp[1].y = (short)(yoffset + height - q);
	cb->combo_box.triangleUp[2].x = (short)(xoffset + width - q);
	cb->combo_box.triangleUp[2].y = (short)(yoffset + height - q);
    }
    else
	memset(cb->combo_box.triangleUp, 0, sizeof(XPoint) * 3);
    
    /* get triangleDown points */
    if (((bw - (st * 2)) >= 4) && ((cbh - (st * 2)) >= 4)) {
	int xoffset = ht + tw + st;
	int yoffset = ht + st;
	int width = bw - (st * 2);
	int height = cbh - (st * 2);
	int q = width / 4;
	int half = width / 2;

	cb->combo_box.triangleDown[0].x = (short)(xoffset + q);
	cb->combo_box.triangleDown[0].y = (short)(yoffset + q);
	cb->combo_box.triangleDown[1].x = (short)(xoffset + width - q);
	cb->combo_box.triangleDown[1].y = (short)(yoffset + q);
	cb->combo_box.triangleDown[2].x = (short)(xoffset + half);
	cb->combo_box.triangleDown[2].y = (short)(yoffset + height - q);
    }
    else
	memset(cb->combo_box.triangleDown, 0, sizeof(XPoint) * 3);
    
    /* get cursor segments */
    {
	int mid = (int)cb->core.height / 2;
	int pos = InsertPosToAbsolutePos(cb, cb->combo_box.insertPos);
	int fontH;
	int top;
	int bot;
	
	if (cb->combo_box.ascent && cb->combo_box.descent) {
	    fontH = cb->combo_box.ascent + cb->combo_box.descent;
	    top = mid - (fontH / 2);
	    bot = mid + (fontH / 2);
	}
	else {
	    fontH = cbh - (st * 2) - (hm * 2);
	    top = mid - (fontH / 2);
	    bot = mid + (fontH / 2);
	}
	
	cb->combo_box.cursorSegments[0].x1 = (short)(pos - 2);
	cb->combo_box.cursorSegments[0].y1 = (short)top;
	cb->combo_box.cursorSegments[0].x2 = (short)(pos + 3);
	cb->combo_box.cursorSegments[0].y2 = (short)top;
	cb->combo_box.cursorSegments[1].x1 = (short)(pos - 2);
	cb->combo_box.cursorSegments[1].y1 = (short)bot;
	cb->combo_box.cursorSegments[1].x2 = (short)(pos + 3);
	cb->combo_box.cursorSegments[1].y2 = (short)bot;
	cb->combo_box.cursorSegments[2].x1 = (short)pos;
	cb->combo_box.cursorSegments[2].y1 = (short)(top + 1);
	cb->combo_box.cursorSegments[2].x2 = (short)pos;
	cb->combo_box.cursorSegments[2].y2 = (short)bot;
    } 
}



/*
 *
 */
PRIVATE void DeleteString(XiComboBoxWidget cb, int start, int len)
{
    char buf[MAX_STRING_LEN];
    
    memset(buf, 0, MAX_STRING_LEN);
    strncpy(buf, cb->combo_box.text, start);
    strcat(buf, &cb->combo_box.text[start + len]);
    strcpy(cb->combo_box.text, buf);
}


/*
 *
 */
PRIVATE void InsertString(XiComboBoxWidget cb, char *str)
{
    char buf[MAX_STRING_LEN];
    int len = strlen(str);
    
    memset(buf, 0, MAX_STRING_LEN);
    strncpy(buf, cb->combo_box.text, cb->combo_box.insertPos);
    strncat(buf, str, len);
    strcat(buf, &cb->combo_box.text[cb->combo_box.insertPos]);
    strcpy(cb->combo_box.text, buf);
}


/*
 *
 */
PRIVATE Boolean IsInButton(XiComboBoxWidget cb, int x, int y)
{
    if ((y < 0) || (y > (int)cb->core.height))
	return False;
    if ((x < 0) || (x > (int)cb->core.width))
	return False;
    if (x > (int)cb->combo_box.buttonTopShadow[1].x)
	return True;
    return False;
}


/*
 *
 */
PRIVATE void DrawTextShadow(XiComboBoxWidget cb)
{
    if ((cb->combo_box.textTopShadow[2].x) && XtIsRealized((Widget)cb)){ /* check upper right x coord */
	XFillPolygon(cb->combo_box.dp,
		     cb->core.window,
		     cb->primitive.bottom_shadow_GC,
		     cb->combo_box.textTopShadow, 6, Nonconvex, CoordModeOrigin);
	XFillPolygon(cb->combo_box.dp,
		     cb->core.window,
		     cb->primitive.top_shadow_GC,
		     cb->combo_box.textBottomShadow, 6, Nonconvex, CoordModeOrigin);
    }
}




/*
 *
 */
PRIVATE void DrawButton(XiComboBoxWidget cb)
{
     /* check upper right x coord */
    if ((cb->combo_box.buttonTopShadow[2].x) && XtIsRealized((Widget)cb)) { 
	XSetForeground(cb->combo_box.dp, cb->combo_box.buttonGC, 
		       cb->combo_box.buttonBackground);
	XFillRectangle(cb->combo_box.dp,
		       cb->core.window,
		       cb->combo_box.buttonGC,
		       (int)cb->combo_box.buttonTopShadow[1].x, /* upper left x coord */
		       (int)cb->combo_box.buttonTopShadow[1].y, /* upper left y coord */
		       (unsigned int)(cb->combo_box.buttonTopShadow[2].x - 
				      cb->combo_box.buttonTopShadow[1].x),
		       (unsigned int)(cb->combo_box.buttonTopShadow[0].y - 
				      cb->combo_box.buttonTopShadow[1].y));
	XSetForeground(cb->combo_box.dp, cb->combo_box.buttonGC, 
		       cb->combo_box.buttonForeground);
	
	/* draw top shadow */
	XFillPolygon(cb->combo_box.dp,
		     cb->core.window,
		     cb->combo_box.buttonUp ? cb->combo_box.buttonTopShadowGC:cb->combo_box.buttonBottomShadowGC,
		     cb->combo_box.buttonTopShadow, 6, Nonconvex, CoordModeOrigin);
	
	/* draw bottom shadow */
	XFillPolygon(cb->combo_box.dp,
		     cb->core.window,
		     cb->combo_box.buttonUp ? cb->combo_box.buttonBottomShadowGC:cb->combo_box.buttonTopShadowGC,
		     cb->combo_box.buttonBottomShadow, 6, Nonconvex, CoordModeOrigin);
  }
    
    /* now draw the triangle */
    if ((cb->combo_box.triangleUp[0].x) && XtIsRealized((Widget)cb)) { 
	/* make sure it has dimension */
	if (cb->combo_box.arrowUp)   /* draw pointing up */
	    XFillPolygon(cb->combo_box.dp,
			 cb->core.window,
			 cb->combo_box.buttonGC,
			 cb->combo_box.triangleUp, 3, Convex, CoordModeOrigin);
	else                         /* draw pointing down */
	    XFillPolygon(cb->combo_box.dp,
			 cb->core.window,
			 cb->combo_box.buttonGC,
			 cb->combo_box.triangleDown, 3, Convex, CoordModeOrigin);
    }
}



/*
 *
 */
PRIVATE void DrawText(XiComboBoxWidget cb)
{
    XmString xms;
    char *ptr = cb->combo_box.text;
    Dimension w;
    Dimension maxW = (Dimension)cb->combo_box.textRec.width - 
	(cb->combo_box.widthMargin * (Dimension)2);
    char buf[MAX_STRING_LEN];
    int n;
    
    
    if (cb->combo_box.text == NULL)
	return;
    if ((cb->combo_box.textRec.width - (cb->combo_box.widthMargin * 2) <= 
	 (unsigned short)0) || 
	(cb->combo_box.textRec.height - (cb->combo_box.heightMargin * 2) <=
	 (unsigned short)0))
	return;
    
    ptr += cb->combo_box.headPos;
    while(1) {
	memset(buf, 0, MAX_STRING_LEN);
	strncpy(buf, ptr, cb->combo_box.insertPos - cb->combo_box.headPos);
	xms = XmStringCreateLocalized(buf);
	w = XmStringWidth(cb->combo_box.fontList, xms);
	if (w <= maxW) {
	    XmStringFree(xms);
	    break;
	}
	XmStringFree(xms);
	cb->combo_box.headPos += 1;
	ptr++;
    }
    n = cb->combo_box.len - cb->combo_box.headPos;
    while(1) {
	memset(buf, 0, MAX_STRING_LEN);
	strncpy(buf, ptr, n);
	xms = XmStringCreateLocalized(buf);
	w = XmStringWidth(cb->combo_box.fontList, xms);
	if (w <= maxW)
      break;
	XmStringFree(xms);
	n--;
    }
    if (XtIsRealized((Widget)cb)) {
	XClearArea(cb->combo_box.dp,
		   cb->core.window,
		   (int)cb->combo_box.textRec.x,
		   (int)cb->combo_box.textRec.y,
		   (unsigned int)cb->combo_box.textRec.width,
		   (unsigned int)cb->combo_box.textRec.height,
		   False);
	XmStringDraw(cb->combo_box.dp,
		     cb->core.window,
		     cb->combo_box.fontList,
		     xms,
		     cb->combo_box.textGC,
		     (Position)cb->combo_box.textRec.x + 
		     (Position)cb->combo_box.widthMargin,
		     (Position)cb->combo_box.cursorSegments[0].y1,
		     w,
		     XmALIGNMENT_BEGINNING,
		     XmSTRING_DIRECTION_L_TO_R,
		     &(cb->combo_box.textRec));
    }
    XmStringFree(xms);
    cb->combo_box.cursorVisible = False;
}


/*
 *
 */
PRIVATE void DrawCursor(XiComboBoxWidget cb)
{
    int newPos = InsertPosToAbsolutePos(cb, cb->combo_box.insertPos);
    
    if (!XtIsRealized((Widget)cb))
	return;
    
    if (cb->combo_box.cursorVisible) {
	XDrawSegments(cb->combo_box.dp,
		      cb->core.window,
		      cb->combo_box.cursorGC,
		      cb->combo_box.cursorSegments, 3);
    }
    
    cb->combo_box.cursorSegments[0].x1 = (short)(newPos - 2);
    cb->combo_box.cursorSegments[0].x2 = (short)(newPos + 3);
    cb->combo_box.cursorSegments[1].x1 = (short)(newPos - 2);
    cb->combo_box.cursorSegments[1].x2 = (short)(newPos + 3);
    cb->combo_box.cursorSegments[2].x1 = (short)newPos;
    cb->combo_box.cursorSegments[2].x2 = (short)newPos;
    
    if (!cb->combo_box.editable)
	return;
    
    XDrawSegments(cb->combo_box.dp,
		  cb->core.window,
		  cb->combo_box.cursorGC,
		  cb->combo_box.cursorSegments, 3);
    cb->combo_box.cursorVisible = True;
}


/*
 * KeyHandler()
 *
 */
PRIVATE void KeyHandler(Widget w, XEvent *event, String *param, Cardinal *numParam)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    char buf[MAX_STRING_LEN];
    int len;
    KeySym ksym;
    Boolean insert = True;
    Dimension width;
    XmString xms;
    char *ptr;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    if (!cb->combo_box.editable)
	return;
    
    memset(buf, 0, MAX_STRING_LEN);
    len = XLookupString(&(event->xkey), buf, MAX_STRING_LEN, &ksym, NULL);
    if (ksym == XK_Left) {
	ResetCBS(cb);
	cb->combo_box.cbs.reason = XiCR_MOTION_VERIFY;
	cb->combo_box.cbs.event = event;
	cb->combo_box.cbs.label = cb->combo_box.text;
	cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
	
	cb->combo_box.hllen = 0;
	if (cb->combo_box.insertPos <= 0)
	    return;
	if (cb->combo_box.insertPos <= cb->combo_box.headPos) {
	    cb->combo_box.cbs.newInsert = cb->combo_box.insertPos - 1;
	    XtCallCallbacks(w, XiNmotionVerifyCallback, 
			    (XtPointer)&cb->combo_box.cbs);
	    if (!cb->combo_box.cbs.doit)
		return;
	    cb->combo_box.headPos--;
	    cb->combo_box.insertPos = cb->combo_box.headPos;
	    DrawText(cb);
	    DrawCursor(cb);
	    return;
	}
	cb->combo_box.cbs.newInsert = cb->combo_box.insertPos - 1;
	XtCallCallbacks(w, XiNmotionVerifyCallback, (XtPointer)&cb->combo_box.cbs);
	if (!cb->combo_box.cbs.doit)
	    return;
	cb->combo_box.insertPos--;
	DrawText(cb);
	DrawCursor(cb);
	return;
    }
    if (ksym == XK_Right) {
	int newPos;
	ResetCBS(cb);
	cb->combo_box.cbs.reason = XiCR_MOTION_VERIFY;
	cb->combo_box.cbs.event = event;
	cb->combo_box.cbs.label = cb->combo_box.text;
	cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
	
	cb->combo_box.hllen = 0;
	if (cb->combo_box.insertPos == cb->combo_box.len)
	    return;
	
	cb->combo_box.cbs.newInsert = cb->combo_box.insertPos + 1;
	XtCallCallbacks(w, XiNmotionVerifyCallback, (XtPointer)&cb->combo_box.cbs);
	if (!cb->combo_box.cbs.doit)
	    return;
	newPos = InsertPosToAbsolutePos(cb, cb->combo_box.insertPos + 1);
	if (newPos > (int)(cb->primitive.highlight_thickness + 
			   cb->primitive.shadow_thickness) + 
	    (int)cb->combo_box.textRec.width)
	    cb->combo_box.headPos++;
	cb->combo_box.insertPos++;
	DrawText(cb);
	DrawCursor(cb);
	return;
    }
    
    if (len == 1) {
	if (cb->combo_box.hllen) {
	    ResetCBS(cb);
	    cb->combo_box.cbs.reason = XiCR_MODIFY_VERIFY;
	    cb->combo_box.cbs.event = event;
	    cb->combo_box.cbs.label = cb->combo_box.text;
	    cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
	    cb->combo_box.cbs.newInsert = cb->combo_box.hlStart;
	    cb->combo_box.cbs.startPos = cb->combo_box.hlStart;
	    cb->combo_box.cbs.len = -cb->combo_box.hllen;
	    cb->combo_box.cbs.newText = XtMalloc(cb->combo_box.hllen + 1);
	    memset(cb->combo_box.cbs.newText, 0, cb->combo_box.hllen + 1);
	    strncpy(cb->combo_box.cbs.newText, 
		    &cb->combo_box.text[cb->combo_box.hlStart], cb->combo_box.hllen);
	    XtCallCallbacks(w, XiNmodifyVerifyCallback, 
			    (XtPointer)&cb->combo_box.cbs);
	    if (!cb->combo_box.cbs.doit)
		return;
	    DeleteString(cb, cb->combo_box.hlStart, cb->combo_box.hllen);
	    cb->combo_box.len -= cb->combo_box.hllen;
	    cb->combo_box.hllen = 0;
	    cb->combo_box.insertPos = cb->combo_box.hlStart;
	    if (ksym == XK_BackSpace)
		insert = False;
	}
	if (insert) {
	    if (ksym == XK_BackSpace && cb->combo_box.insertPos) {
		ResetCBS(cb);
		cb->combo_box.cbs.reason = XiCR_MODIFY_VERIFY;
		cb->combo_box.cbs.event = event;
		cb->combo_box.cbs.label = cb->combo_box.text;
		cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
		cb->combo_box.cbs.newInsert = cb->combo_box.insertPos - 1;
		cb->combo_box.cbs.startPos = cb->combo_box.insertPos - 1;
		cb->combo_box.cbs.len = -1;
		cb->combo_box.cbs.newText = XtMalloc(2);
		memset(cb->combo_box.cbs.newText, 0, 2);
		strncpy(cb->combo_box.cbs.newText, 
			&cb->combo_box.text[cb->combo_box.insertPos - 1], 1);
		XtCallCallbacks(w, XiNmodifyVerifyCallback, 
				(XtPointer)&cb->combo_box.cbs);
		if (!cb->combo_box.cbs.doit)
		    return;
		DeleteString(cb, cb->combo_box.insertPos - 1, 1);
		cb->combo_box.insertPos--;
		if (cb->combo_box.insertPos < cb->combo_box.headPos)
		    cb->combo_box.headPos--;
		cb->combo_box.len--;
	    }
	    else if ((ksym != XK_BackSpace) && (cb->combo_box.len < 
						(MAX_STRING_LEN - 1))) {
		ResetCBS(cb);
		cb->combo_box.cbs.reason = XiCR_MODIFY_VERIFY;
		cb->combo_box.cbs.event = event;
		cb->combo_box.cbs.label = cb->combo_box.text;
		cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
		cb->combo_box.cbs.newInsert = cb->combo_box.insertPos + 1;
		cb->combo_box.cbs.startPos = cb->combo_box.insertPos;
		cb->combo_box.cbs.len = 1;
		cb->combo_box.cbs.newText = XtMalloc(2);
		memset(cb->combo_box.cbs.newText, 0, 2);
		strncpy(cb->combo_box.cbs.newText, buf, 1);
		XtCallCallbacks(w, XiNmodifyVerifyCallback, 
				(XtPointer)&cb->combo_box.cbs);
		if (!cb->combo_box.cbs.doit)
		    return;
		InsertString(cb, buf);
		cb->combo_box.insertPos += len;
		cb->combo_box.len++;
	    }
	}
	while(1) {
	    memset(buf, 0, MAX_STRING_LEN);
	    ptr = cb->combo_box.text;
	    ptr += cb->combo_box.headPos;
	    strncpy(buf, ptr, cb->combo_box.insertPos - cb->combo_box.headPos);
	    xms = XmStringCreateLocalized(buf);
	    width = XmStringWidth(cb->combo_box.fontList, xms);
	    if (width >= (Dimension)cb->combo_box.textRec.width - 
		(cb->combo_box.widthMargin * (Dimension)2))
		cb->combo_box.headPos++;
	    else
		break;
	}
	DrawText(cb);
	DrawCursor(cb);
    }
}


/*
 * ButtonPressHandler()
 *
 */
PRIVATE void ButtonPressHandler(Widget w,
				XEvent *event,
				String *param,
				Cardinal *numParam)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    int newInsert;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    if (IsInButton(cb, event->xbutton.x, event->xbutton.y)) {
	cb->combo_box.buttonUp = False;
	DrawButton(cb);
	return;
    }
    if (!cb->combo_box.editable) {
	Popup(cb);
	return;
    }
    newInsert = AbsolutePosToInsertPos(cb, event->xbutton.x);
    ResetCBS(cb);
    cb->combo_box.cbs.reason = XiCR_MOTION_VERIFY;
    cb->combo_box.cbs.event = event;
    cb->combo_box.cbs.label = cb->combo_box.text;
    cb->combo_box.cbs.currInsert = cb->combo_box.insertPos;
    cb->combo_box.cbs.newInsert = newInsert;
    XtCallCallbacks(w, XiNmotionVerifyCallback, (XtPointer)&cb->combo_box.cbs);
    if (!cb->combo_box.cbs.doit)
	return;
    cb->combo_box.insertPos = cb->combo_box.hlStart = 
	AbsolutePosToInsertPos(cb, event->xbutton.x);
    DrawCursor(cb);
}


/* 
 *
 */
PRIVATE void ButtonReleaseHandler(Widget w, XEvent *event, String *param,
				  Cardinal *numParam)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    if (IsInButton(cb, event->xbutton.x, event->xbutton.y) && (!cb->combo_box.buttonUp)) 
	Popup(cb);
}



/* 
 *
 */
PRIVATE void ButtonMotionHandler(Widget w, XEvent *event, String *param,
				 Cardinal *numParam)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    if ((!cb->combo_box.buttonUp) && (!IsInButton(cb, event->xmotion.x, 
						  event->xmotion.y))) {
	cb->combo_box.buttonUp = True;
	DrawButton(cb);
    };
    
}



/* 
 *
 */
PRIVATE void FocusHandler(Widget w, XEvent *event, String *param, 
			  Cardinal *numParam)  
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    static char oldval[MAX_STRING_LEN];
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    ResetCBS(cb);
    if (event->type == FocusIn) {
	(*sclass->primitive_class.border_highlight)(w);
	if (cb->combo_box.timerId > (XtIntervalId)0)
	    XtRemoveTimeOut(cb->combo_box.timerId);
	cb->combo_box.timerId = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
						XICOMBOBOX_CURSOR_BLINK_RATE,
						DoCursorBlink, (XtPointer)cb);
	cb->combo_box.cbs.reason = XiCR_FOCUSIN;
    }
    else {
	(*sclass->primitive_class.border_unhighlight)(w);
	XtRemoveTimeOut(cb->combo_box.timerId);
	cb->combo_box.timerId = (XtIntervalId)0;
	DrawCursor(cb);
	cb->combo_box.cbs.reason = XiCR_FOCUSOUT;
    }
    
    cb->combo_box.cbs.event = event;
    if (cb->combo_box.cbs.reason == XiCR_FOCUSIN) {
	XtCallCallbacks(w, XiNfocusInCallback, (XtPointer)&cb->combo_box.cbs);
	strcpy(oldval, cb->combo_box.text);
    }
    else {
	XtCallCallbacks(w, XiNfocusOutCallback, (XtPointer)&cb->combo_box.cbs);
	if (strcmp(oldval, cb->combo_box.text)) {
	    ResetCBS(cb);
	    cb->combo_box.cbs.reason = XiCR_VALUE_CHANGED;
	    cb->combo_box.cbs.event = event;
	    cb->combo_box.cbs.label = cb->combo_box.text;
	    XtCallCallbacks(w, XiNvalueChangedCallback, 
			    (XtPointer)&cb->combo_box.cbs);
	}
    }
}


/* 
 *
 */
PRIVATE void ReturnKeyHandler(Widget w, XEvent *event, String *param,
			     Cardinal *numParam)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    event=event;
    param=param;
    numParam=numParam;
    
    printf("activate\n");
}


/* 
 *
 */
PRIVATE void TabKeyHandler(Widget w, XEvent *event, String *param,
			   Cardinal *numParam)
{
    /* done to get rid of compiler warning about unused parameters */
    event=event;
    param=param;
    numParam=numParam;
    
    XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}


/*
 * ListCallbackProc()
 * this function pops up the popup shell for the list
 */
PRIVATE void ListCallbackProc(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XiComboBoxWidget cb = (XiComboBoxWidget) client_data;
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    char *txt;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    
    XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &txt);
    ResetCBS(cb);
    cb->combo_box.cbs.reason = XiCR_SELECT;
    cb->combo_box.cbs.label = txt;
    cb->combo_box.cbs.id = GetSelectedListItemId(cb);
    XtCallCallbacks((Widget)cb, XiNselectCallback, (XtPointer)&cb->combo_box.cbs);
    if (!cb->combo_box.cbs.doit) {
	XtFree(txt);
	Popdown(cb);
	return;
    }
    
    strcpy(cb->combo_box.text, txt);
    cb->combo_box.len = strlen(txt);
    cb->combo_box.headPos = 0;
    cb->combo_box.insertPos = cb->combo_box.len;
    cb->combo_box.hlStart = 0;
    cb->combo_box.hllen = 0;
    DrawText(cb);
    DrawCursor(cb);
    XtFree(txt);
    Popdown(cb);
}




/*
 * PopupEventHandler()
 * this function monitors the pointer events while popup shell is up
 */
PRIVATE void PopupEventHandler(Widget w, XtPointer client_data, XEvent *event,
			       Boolean *cont)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)client_data;
    
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    cont=cont;
    
    if ((event->type == EnterNotify) && (event->xcrossing.mode == NotifyNormal)){
	XUngrabPointer(cb->combo_box.dp, CurrentTime);
	XGrabPointer(cb->combo_box.dp,
		     XtWindow(cb->combo_box.form),
		     True,
		     ButtonPressMask | ButtonReleaseMask | EnterWindowMask | 
		     LeaveWindowMask,
		     GrabModeAsync,
		     GrabModeAsync,
		     None,
		     None,
		     CurrentTime);
    }
    else if ((event->type == LeaveNotify) && (event->xcrossing.mode == NotifyNormal)){
	XUngrabPointer(cb->combo_box.dp, CurrentTime);
	XGrabPointer(cb->combo_box.dp,
		     XtWindow(cb->combo_box.form),
		     False,
		     ButtonPressMask | ButtonReleaseMask | EnterWindowMask | 
		     LeaveWindowMask,
		     GrabModeAsync,
		     GrabModeAsync,
		     None,
		     None,
		     CurrentTime);
    }
    else if ((event->type == ButtonPress) && !(event->xbutton.subwindow)) {
	XUngrabPointer(cb->combo_box.dp, CurrentTime);
	Popdown(cb);
    }
}


/*
 *
 */
PRIVATE void Popup(XiComboBoxWidget cb)
{
    Position x, y;
    Widget hsb;
    Dimension hsb_height;
    Dimension list_spacing;
    Dimension w, h;
    
    XtTranslateCoords((Widget)cb, 0, cb->core.height, &x, &y);
    XtMoveWidget(cb->combo_box.popupShell, x, y);
    
    XtVaGetValues(XtParent(cb->combo_box.list),
		  XmNhorizontalScrollBar,  &hsb,
		  NULL);
    XtVaGetValues(hsb,
		  XmNheight,  &hsb_height,
		  NULL);
    XtVaGetValues(cb->combo_box.list,
		  XmNlistSpacing, &list_spacing,
		  NULL);
    w = cb->core.width -  cb->core.height;
    h = ((cb->combo_box.maxHeight + list_spacing) * 
	 (Dimension)cb->combo_box.visibleCount) +
	hsb_height + cb->combo_box.maxHeight;
    XtVaSetValues(cb->combo_box.popupShell,
		  XmNwidth,  w,
		  XmNheight, h,
		  XmNx,      x,
		  XmNy,      y,
		  NULL);
    XtPopup(cb->combo_box.popupShell, XtGrabNone);
    XtVaSetValues(cb->combo_box.popupShell,
		  XmNwidth,  w,
		  XmNheight, h,
		  NULL);
    
    XGrabPointer(XtDisplay(cb->combo_box.form),
		 XtWindow(cb->combo_box.form),
		 False,
		 ButtonPressMask | ButtonReleaseMask | EnterWindowMask | 
		 LeaveWindowMask,
		 GrabModeAsync,
		 GrabModeAsync,
		 None,
		 None,
		 CurrentTime);
    cb->combo_box.buttonUp = True;
    cb->combo_box.arrowUp = True;
    DrawButton(cb);
}


/*
 *
 */
PRIVATE void Popdown(XiComboBoxWidget cb)
{
    XUngrabPointer(XtDisplay(cb->combo_box.form), CurrentTime);
    XtPopdown(cb->combo_box.popupShell);
    cb->combo_box.buttonUp = True;
    cb->combo_box.arrowUp = False;
    DrawButton(cb);
}



/*
 *
 */
PRIVATE void FreeItems(XmString *items, int n)
{
    int i;
    
    for (i = 0; i < n; i++)
	XmStringFree(items[i]);
}


/*
 *
 */
PRIVATE XmString *CopyItems(XmString *items, int n)
{
    int i;
    XmString *tmp;
    
    if (n <= 0)
	return NULL;
    
    /*tmp = (XmString *)XtMalloc(sizeof(XmString) * n);*/
    tmp = (XmString *)malloc(sizeof(XmString) * n);
    for (i = 0; i < n; i++) 
	tmp[i] = XmStringCopy(items[i]);
    return tmp;
}



/*
 *
 */
PRIVATE void      AddItem(XmString **items, XmString item, int curr_count,
			  int pos)
{
    /*XmString *new = (XmString *)XtMalloc(sizeof(XmString) * (curr_count + 1));*/
    XmString *new = (XmString *)malloc(sizeof(XmString) * (curr_count + 1));
    XmString *tmp = *items;
    int i;
    
    if ((pos == 0) || (pos > curr_count)){
	for (i = 0; i < curr_count; i++)
	    new[i] = XmStringCopy(tmp[i]);
	new[curr_count] = XmStringCopy(item);
    }
    else {
	for (i = 0; i < (pos - 1); i++) 
	    new[i] = XmStringCopy(tmp[i]);
	new[pos - 1] = XmStringCopy(item);
	for (i = pos - 1; i < curr_count; i++)
	    new[i + 1] = XmStringCopy(tmp[i]);
    }
    FreeItems(*items, curr_count);
    *items = new;    
}



/*
 *
 */
PRIVATE void      DeleteItem(XmString **items,  XmString item, int curr_count)
{
    /*XmString *new = (XmString *)XtMalloc(sizeof(XmString) * (curr_count -1));*/
    XmString *new = (XmString *)malloc(sizeof(XmString) * (curr_count -1));
    XmString *tmp = *items;
    int i;
    int index = 0;
    
    for (i = 0; i < curr_count; i++) {
	if (XmStringCompare(tmp[i], item)) {
	    index = i;
	    break;
	}
	new[i] = XmStringCopy(tmp[i]);
    }
    for (i = index; i < curr_count; i++) 
	new[i] = XmStringCopy(tmp[i + 1]);
    FreeItems(*items, curr_count);
    *items = new;
}


/*
 *
 */
PRIVATE Dimension GetMaxWidth(XiComboBoxWidget cb)
{
    Dimension max = (Dimension)0;
    Dimension w;
    int i;
    
    for (i = 0; i < cb->combo_box.itemCount; i++) {
	w = XmStringWidth(cb->combo_box.fontList, cb->combo_box.internalItems[i]);
	if (w > max)
	    max = w;
    }
    return max;
}


/*
 *
 */
PRIVATE Dimension GetMaxHeight(XiComboBoxWidget cb)
{
    Dimension max = (Dimension)0;
    Dimension h;
    int i;
    
    for (i = 0; i < cb->combo_box.itemCount; i++) {
	h = XmStringHeight(cb->combo_box.fontList, cb->combo_box.internalItems[i]);
	if (h > max)
	    max = h;
    }
    return max;
}

/**************************************************************************************
 * interface functions
 **************************************************************************************/

/*
 * XiCreateComboBox()
 * this function creates a ComboBox
 * it takes a va num of items to be included in the ComboBox
 */
PUBLIC Widget   XiCreateComboBox(Widget w, char *resource_name,
				 ...)      /* varargs of items to be contained */
{
    va_list ap;
    char *tmp;
    Widget comboBox;
    XiComboBoxWidget cb;
    XmString xms;
    
    va_start(ap, resource_name);
    comboBox = XtVaCreateWidget(resource_name, xiComboBoxWidgetClass,
				w, NULL);
    
    cb = (XiComboBoxWidget)comboBox;
    while ((tmp = va_arg(ap, char *))) {
	xms = XmStringCreateLocalized(tmp);
	XiComboBoxAddItem(comboBox, xms, 0);
	XmStringFree(xms);
    }
    va_end(ap);
    return comboBox;
}



/*
 * XiComboBoxGetSelectedString()
 * this function returns selected XmString
 * The returned XmString must not be freed
 */
PUBLIC XmString XiComboBoxGetSelectedString(Widget w)
{
    XmString *xms;
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    XtVaGetValues(cb->combo_box.list,
		  XmNselectedItems,   &xms,
		  NULL);
    return xms[0];
}



/*
 *
 */
PUBLIC int      XiComboBoxGetSelectedPosition(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    return GetSelectedListItemId(cb);
}


/*
 *  Do not free the returned XmString
 */
PUBLIC XmString XiComboBoxGetStringByPosition(Widget w, int position) 
{
    XmString *xms;
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    if ((position <= 0) || (position > cb->combo_box.itemCount))
	return NULL;
    XtVaGetValues(cb->combo_box.list,
		  XmNitems,     &xms,
		  NULL);
    return xms[position - 1];
}


/*
 *
 */
PUBLIC int      XiComboBoxGetPositionByString(Widget w, XmString xms) 
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    return XmListItemPos(cb->combo_box.list, xms);
}


/*
 *
 */
PUBLIC void XiComboBoxSelectString(Widget w, XmString xms, Boolean notify)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    char *txt;
    
    if (!XmListItemExists(cb->combo_box.list, xms))
	return;
    if (notify) 
	XmListSelectItem(cb->combo_box.list, xms, True);
    else {
	XmStringGetLtoR(xms, XmFONTLIST_DEFAULT_TAG, &txt);
	strcpy(cb->combo_box.text, txt);
	cb->combo_box.len = strlen(txt);
	cb->combo_box.headPos = 0;
	cb->combo_box.insertPos = cb->combo_box.len;
	cb->combo_box.hlStart = 0;
	cb->combo_box.hllen = 0;
	DrawText(cb);
	DrawCursor(cb);
	XtFree(txt);
	XmListSelectItem(cb->combo_box.list, xms, False);
    }
}
  

/*
 *
 */
PUBLIC void XiComboBoxSelectPosition(Widget w, int position, Boolean notify)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    XmString *xms;
    
    if ((position < 1) || (position > cb->combo_box.itemCount))
	return;
    if (notify) 
	XmListSelectPos(cb->combo_box.list, position, True);
    else {
	char *txt;
	XtVaGetValues(cb->combo_box.list,
		      XmNitems,     &xms,
		      NULL);
	XmStringGetLtoR(xms[0], XmFONTLIST_DEFAULT_TAG, &txt);
	strcpy(cb->combo_box.text, txt);
	cb->combo_box.len = strlen(txt);
	cb->combo_box.headPos = 0;
	cb->combo_box.insertPos = cb->combo_box.len;
	cb->combo_box.hlStart = 0;
	cb->combo_box.hllen = 0;
	DrawText(cb);
	DrawCursor(cb);
	XtFree(txt);
	XmListSelectPos(cb->combo_box.list, position, False);
    }
}


/*
 *
 */
PUBLIC void  XiComboBoxDeleteItemByString(Widget w, XmString xms)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    if (!XmListItemExists(cb->combo_box.list, xms)) {
	XtWarning("XiComboBoxWidget:XiComboBoxDeleteItemByString:Specified item does not exist");
	return;
    }
    
    XmListDeleteItem(cb->combo_box.list, xms);
    DeleteItem(&(cb->combo_box.internalItems), xms, cb->combo_box.itemCount);
    cb->combo_box.itemCount -= 1;
    cb->combo_box.items = cb->combo_box.internalItems;
}


/*
 *
 */
PUBLIC void  XiComboBoxDeleteItemByPosition(Widget w, int position)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    XmString xms;
    
    if ((position <= 0) || (position > cb->combo_box.itemCount)) {
	XtWarning("XiComboBoxWidget:XiComboBoxDeleteItemByPosition:Invalid item position");
	return;
    }
    xms = XiComboBoxGetStringByPosition(w, position);
    XmListDeleteItem(cb->combo_box.list, xms);
    DeleteItem(&(cb->combo_box.internalItems), xms, cb->combo_box.itemCount);
    cb->combo_box.itemCount -= 1;
    cb->combo_box.items = cb->combo_box.internalItems;
    cb->combo_box.maxHeight = GetMaxHeight(cb);
    cb->combo_box.maxWidth = GetMaxWidth(cb);
}


/*
 *
 */
PUBLIC void     XiComboBoxAddItem(Widget w, XmString xms, int position)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    if ((position < 0) || (position > cb->combo_box.itemCount)) {
	XtWarning("XiComboBoxWidget:XiComboBoxAddItem:Invalid position parameter");
	return;
    }
    AddItem(&(cb->combo_box.internalItems), xms, cb->combo_box.itemCount, position);
    XmListAddItem(cb->combo_box.list, xms, position);
    cb->combo_box.itemCount += 1;
    cb->combo_box.items = cb->combo_box.internalItems;
    cb->combo_box.maxHeight = GetMaxHeight(cb);
    cb->combo_box.maxWidth = GetMaxWidth(cb);
}


/*
 *
 */
PUBLIC int      XiComboBoxGetItemCount(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    return cb->combo_box.itemCount;
}

/*
 *
 */
PUBLIC Boolean  XiComboBoxIsEditable(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    
    return cb->combo_box.editable;
}


/*
 *
 */
PUBLIC void     XiComboBoxSetEditable(Widget w, Boolean state)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;

    cb->combo_box.editable = state;
  
}


/*
 *  Do not free the text
 */
PUBLIC char    *XiComboBoxGetTextString(Widget w)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
  
    return cb->combo_box.text;
}


/*
 *  
 */
PUBLIC void     XiComboBoxSetTextString(Widget w, char *txt, Boolean notify)
{
    XiComboBoxWidget cb = (XiComboBoxWidget)w;
    Boolean doCallValueChanged = False;
    
    if (strcmp(cb->combo_box.text, txt))
	doCallValueChanged = True;
    
    strcpy(cb->combo_box.text, txt);
    cb->combo_box.len = strlen(txt);
    cb->combo_box.headPos = 0;
    cb->combo_box.insertPos = cb->combo_box.len;
    cb->combo_box.hlStart = 0;
    cb->combo_box.hllen = 0;
    DrawText(cb);
    DrawCursor(cb);
    if (notify && doCallValueChanged) {
	ResetCBS(cb);
	cb->combo_box.cbs.reason = XiCR_VALUE_CHANGED;
	cb->combo_box.cbs.label = cb->combo_box.text;
	XtCallCallbacks(w, XiNvalueChangedCallback, (XtPointer)&cb->combo_box.cbs);
    } 
}
