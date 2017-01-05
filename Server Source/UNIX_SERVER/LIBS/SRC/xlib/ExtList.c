
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 
  
/**
  * ExtList.c
  *
  * Implementation of XiExtdList widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/17/97: Created
  * MBS:  2/11/97: Removed debug and related functions
  * MBS:  3/06/97: Fixed various bugs, ie. LoadItems. etc
  * MBS:           Added XiExtdListGetSortColumn()
  * MBS:  3/07/97: Fixed the complaint about scrollbar slider size and etc
  * MBS:  3/10/97: Fixed bug associated with clicking on empty area
  *
  */

#include <stdarg.h>
#include <stdlib.h>

#include <X11/cursorfont.h>
#include <Xm/XmP.h>
#include <Xm/ScrollBar.h>


#include "ExtListP.h"

#ifndef MAX
#define MAX(a, b) ((a) >= (b) ? (a):(b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a):(b))
#endif

#define IN 0
#define OUT 1

#define XIEXTLIST_SCROLLBAR_WIDTH 15
#define XIEXTLIST_DEFAULT_BACKGROUND "white"
#define XIEXTLIST_DEFAULT_FOREGROUND "black"

#define XIEXTLIST_DEFAULT_VISIBLE_COUNT 5

#define XIEXTLIST_CH_HEIGHT_MARGIN 4
#define XIEXTLIST_ITEM_HEIGHT_MARGIN 4
#define XIEXTLIST_COLWIDTH_MARGIN 4

#define XIEXTLIST_BUTTON_UP   0 
#define XIEXTLIST_BUTTON_DOWN 1

#define BTNDOWNTYPE_TITLE    0
#define BTNDOWNTYPE_LIST     1
#define BTNDOWNTYPE_RESIZE   2

#define offset(field)   XtOffsetOf(XiExtdListRec, extList.field)

static int cursorBits[] = {
   0x00, 0x00, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x44, 0x22,
   0x42, 0x42, 0x7f, 0xfe, 0x42, 0x42, 0x44, 0x22, 0x40, 0x02, 0x40, 0x02,
   0x40, 0x02, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00};
static int cursorMaskBits[] = {
   0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xec, 0x37, 0xee, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xee, 0x77, 0xec, 0x37, 0xe0, 0x07,
   0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00};
static int stippleBits[] = {
  0xff, 0x00, 0x00, 0xff};

PRIVATE XtResource resources[] = {
  {
    (char*)XiNbrowseSelectCallback,
    (char*)XiCBrowseSelectCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(browseSelectCB),  
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNdefaultActionCallback,
    (char*)XiCDefaultActionCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(defaultActionCB),  
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNextendedSelectCallback,
    (char*)XiCExtendedSelectCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(extSelectionCB),  
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNmultipleSelectCallback,
    (char*)XiCMultipleSelectCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(multiSelectCB),  
    XmRCallback,
    NULL,
  },
  {
    (char*)XiNsingleSelectCallback,
    (char*)XiCSingleSelectCallback,
    XmRCallback,
    sizeof(XtCallbackList),
    offset(singleSelectCB),  
    XmRCallback,
    NULL,
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
    (char*)XiNcolumnCount,
    (char*)XiCColumnCount,
    XmRInt,
    sizeof(int),
    offset(colCount),
    XmRImmediate,
    (XtPointer)0,
  },
  { 
    (char*)XiNrowCount,
    (char*)XiCRowCount,
    XmRInt,
    sizeof(int),
    offset(rowCount),
    XmRImmediate,
    (XtPointer)0,
  },
  {
    (char*)XiNitemType,
    (char*)XiCItemType,
    XmRInt,
    sizeof(int),
    offset(itemType),
    XmRImmediate,
    (XtPointer)XiEXTLIST_ITEM_TYPE_NON_STRUCTURED,
  },
  {
    (char*)XiNitems,
    (char*)XiCItems,
    XmRPointer,
    sizeof(XtPointer),
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
    (XtPointer)XIEXTLIST_DEFAULT_VISIBLE_COUNT,
  },
  {
    (char*)XiNbackground,
    (char*)XiCBackground,
    XmRPixel,
    sizeof(Pixel),
    offset(background),
    XmRString,
    (XtPointer)XIEXTLIST_DEFAULT_BACKGROUND,
  },
  {
    (char*)XiNforeground,
    (char*)XiCForeground,
    XmRPixel,
    sizeof(Pixel),
    offset(foreground),
    XmRString,
    (XtPointer)XIEXTLIST_DEFAULT_FOREGROUND,
  },
  {
    (char*)XiNcolumnHeadingBackground,
    (char*)XiCColumnHeadingBackground,
    XmRPixel,
    sizeof(Pixel),
    offset(chBackground),
    XmRImmediate,
    (XtPointer)0,
  },
  {
    (char*)XiNcolumnHeadingForeground,
    (char*)XiCColumnHeadingForeground,
    XmRPixel,
    sizeof(Pixel),
    offset(chForeground),
    XmRImmediate,
    (XtPointer)0,
  },
  {
    (char*)XiNshowColumnSeparator,
    (char*)XiCShowColumnSeparator,
    XmRBoolean,
    sizeof(Boolean),
    offset(showColSeparator),
    XmRImmediate,
    (XtPointer)True,
  },
  {
    (char*)XiNcolumnHeadingMarginHeight,
    (char*)XiCColumnHeadingMarginHeight,
    XmRDimension,
    sizeof(Dimension),
    offset(chHeightMargin),
    XmRImmediate,
    (XtPointer)2,
  },
  {
    (char*)XiNitemMarginHeight,
    (char*)XiCItemMarginHeight,
    XmRDimension,
    sizeof(Dimension),
    offset(itemHeightMargin),
    XmRImmediate,
    (XtPointer)2,
  },
  {
    (char*)XiNcolumnMarginWidth,
    (char*)XiCColumnMarginWidth,
    XmRDimension,
    sizeof(Dimension),
    offset(colWidthMargin),
    XmRImmediate,
    (XtPointer)5,
  },
  {
    (char*)XiNcolumnHeadingLabelAlignment,
    (char*)XiCColumnHeadingLabelAlignment,
    XmRInt,
    sizeof(int),
    offset(chLabelAlignment),
    XmRImmediate,
    (XtPointer)XiEXTLIST_LABEL_ALIGNMENT_CENTER,
  },
  {
    (char*)XiNextListItemLabelAlignment,
    (char*)XiCExtdListItemLabelAlignment,
    XmRInt,
    sizeof(int),
    offset(itemLabelAlignment),
    XmRImmediate,
    (XtPointer)XiEXTLIST_LABEL_ALIGNMENT_BEGINNING,
  },
  {
    (char*)XiNselectionType,
    (char*)XiCSelectionType,
    XmRInt,
    sizeof(int),
    offset(selectionType),
    XmRImmediate,
    (XtPointer)XiEXTLIST_SINGLE_SELECT,
  },
  {
    (char*)XiNallowSort,
    (char*)XiCAllowSort,
    XmRBoolean,
    sizeof(Boolean),
    offset(allowSort),
    XmRImmediate,
    (XtPointer)True,
  },
};


/* widget class methods */
PRIVATE void Realize(Widget w,
		     XtValueMask *valueMask,
		     XSetWindowAttributes *attrib);
PRIVATE void Initialize(Widget   req,
			Widget   new,
			ArgList  args,
			Cardinal *numArgs);
PRIVATE void Resize(Widget w);
PRIVATE void Destroy(Widget w);
PRIVATE Boolean SetValues(Widget   current,
			  Widget   request,
			  Widget   new,
			  ArgList  args,
			  Cardinal *numArgs);
PRIVATE XtGeometryResult QueryGeometry(Widget w,
				       XtWidgetGeometry *intended,
				       XtWidgetGeometry *reply);
PRIVATE void Redisplay(Widget w,
		       XEvent *event,
		       Region region);
PRIVATE XtGeometryResult GeometryHandler(Widget w,
					 XtWidgetGeometry *request,
					 XtWidgetGeometry *reply);
PRIVATE void InsertChild(Widget w);

/* internal functions */
/*PRIVATE void    Db(char *func,
		   int inOut);*/
/*PRIVATE char    *XmsToChar(XmString xms);*/
/*PRIVATE void    ShiftButtonDownHandler(Widget w,
				       XEvent *event,
				       String *param,
				       Cardinal *numParam); */
PRIVATE void    ButtonClickHandler(Widget w,
				   XEvent *event,
				   String *param,
				   Cardinal *numParam);
PRIVATE void    ButtonReleaseHandler(Widget w,
				     XEvent *event,
				     String *param,
				     Cardinal *numParam);
PRIVATE void    MouseMotionHandler(Widget w,
				   XEvent *event,
				   String *param,
				   Cardinal *numParam);
PRIVATE void    ButtonMotionHandler(Widget w,
				    XEvent *event,
				    String *param,
				    Cardinal *numParam);
PRIVATE void    ScrollBarHandler(Widget w,
				 XtPointer clientData,
				 XtPointer callData);
PRIVATE Boolean MapXToColumn(XiExtdListWidget el, 
			    int x, 
			    int *index);
PRIVATE void    MapYToRowIndex(XiExtdListWidget el, 
			       int y, 
			       int *rowIndex);
PRIVATE int     DefaultSortFunc(XmString xms1,
				XmString xms2);
PRIVATE void    LoadItems(XiExtdListWidget el);
PRIVATE void    UnLoadItems(XiExtdListWidget el);
PRIVATE void    DrawList(XiExtdListWidget el);
PRIVATE void    DrawColumnHeading(XiExtdListWidget el, 
				  int colNum, 
				  int upDown);
PRIVATE void    RefreshAll(XiExtdListWidget el);
PRIVATE void    RefreshButton(XiExtdListWidget el,
			      int num);
PRIVATE void    GetNewPixmap(XiExtdListWidget el);
PRIVATE void    SortList(XiExtdListWidget el, 
			 int col);
PRIVATE void    QuickSort(XiExtdListWidget el,
			  int col,
			  int first,
			  int last);
PRIVATE int     QuickSortPartition(XiExtdListWidget el,
				   int col,
				   int first,
				   int last);
PRIVATE void    CallCallbacks(XiExtdListWidget el,
			      XEvent *event, 
			      int reason);
PRIVATE void    UnselectAll(XiExtdListWidget el);
PRIVATE void    RecalcGeometry(XiExtdListWidget el);
PRIVATE Boolean IsPositionVisible(XiExtdListWidget el, int pos);


PRIVATE char defaultTranslations[] = 
"<Btn1Down>:       ButtonClickHandler()      \n\
<Btn1Up>:         ButtonReleaseHandler()    \n\
<Btn1Motion>:     ButtonMotionHandler()     \n\
<Motion>:         MouseMotionHandler()";

PRIVATE XtActionsRec actions[] = {
  {(char*)"ButtonClickHandler",     (XtActionProc)ButtonClickHandler},
  {(char*)"ButtonReleaseHandler",   (XtActionProc)ButtonReleaseHandler},
  {(char*)"MouseMotionHandler",     (XtActionProc)MouseMotionHandler},
  {(char*)"ButtonMotionHandler",    (XtActionProc)ButtonMotionHandler},
};


XiExtdListClassRec xiExtdListClassRec = { 
  {
    /* core class members */
    (WidgetClass) &xmManagerClassRec,       /* super class                 */
    (char*)"XiExtdList",                    /* class name                  */
    sizeof(XiExtdListRec),                   /* widget size                 */
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
    True,                                   /* visible_interest            */
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
    /* composite_class members */
    GeometryHandler,                        /* goemetry_manager method     */
    NULL,                                   /* change_managed method       */
    InsertChild,                            /* insert_child method         */
    XtInheritDeleteChild,                   /* delete_child method         */
    NULL,                                   /* extension                   */
  },
  {
    /* constraint_class member */
    NULL,                                   /* resource list               */
    0,                                      /* number of resources         */
    0,                                      /* contraint size              */
    NULL,                                   /* init proc                   */
    NULL,                                   /* destroy proc                */
    NULL,                                   /* set values proc             */
    NULL,                                   /* extension                   */
  },
  {
    /* manager_class member */
    XtInheritTranslations,                  /* translations                */
    NULL,                                   /* syn resources               */
    0,                                      /* number of syn resources     */
    NULL,                                   /* syn_cont_resources          */
    0,                                      /* number of syn_cont_resources*/
    XmInheritParentProcess,                 /* parent_process              */
    NULL,                                   /* extension                   */
  },
  {
    /* extList_class member */
    0,                                      /* ignore                      */
  },
};

PUBLIC  WidgetClass xiExtdListWidgetClass = (WidgetClass) &xiExtdListClassRec;
PRIVATE XmManagerWidgetClass sclass = (XmManagerWidgetClass) &xmManagerClassRec;




/*
 * Initialize()
 * method to initialize instance record
 * Called in response to Xt[Va]Create[Managed]Widget
 */
PRIVATE void Initialize(Widget   req, Widget   new, ArgList  args,
			Cardinal *numArgs)
{
    XiExtdListWidget newEl = (XiExtdListWidget) new;
    int i;
    XmFontContext fontContext;
    XmFontListEntry flEntry;
    XmFontType retType;
    XtPointer retFont;
    XFontStruct **fontStruct;
    char **fontName;
    /*Font font = NULL; */
    Font font = (Font)NULL;
    XGCValues val;
    Pixel fg;
    Pixmap shape, mask;
    XColor fgColor, bgColor;
    
    /* done to get rid of compiler warning about unused parameters */
    req=req;
    args=args;
    numArgs=numArgs;
    
    newEl->extList.dp = XtDisplay(new);
    newEl->extList.vscroll = XtVaCreateManagedWidget("XiExtdListVScroll",
						     xmScrollBarWidgetClass,
						     new,
						     XmNorientation,XmVERTICAL,
						     NULL);
    newEl->extList.hscroll = XtVaCreateManagedWidget("XiExtdListHScroll",
						     xmScrollBarWidgetClass,
						     new,
						     XmNorientation,XmHORIZONTAL,
						     NULL);
    XtAddCallback(newEl->extList.vscroll, XmNincrementCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNdecrementCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNpageIncrementCallback,ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNpageDecrementCallback,ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNtoTopCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNtoBottomCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.vscroll, XmNdragCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNincrementCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNdecrementCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNpageIncrementCallback,ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNpageDecrementCallback,ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNtoTopCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNtoBottomCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    XtAddCallback(newEl->extList.hscroll, XmNdragCallback, ScrollBarHandler,
		  (XtPointer)newEl);
    
    if (newEl->extList.visibleCount < 1) {
	XtWarning("XiExtdList:Initialize:XiNvisibleCount must be greater than 0.\n");
	newEl->extList.visibleCount = XIEXTLIST_DEFAULT_VISIBLE_COUNT;
    }
    /* commented out since data types do not allow values less than 0
       if (newEl->extList.chHeightMargin < 0) {
       XtWarning("XiExtdList:Initialize:XiNcolumnHeadingMarginHeight must be greater than or equal to 0\n");
       newEl->extList.chHeightMargin = 0;
       }
       if (newEl->extList.colWidthMargin < 0) {
       XtWarning("XiExtdList:Initialize:XiNcolumnMarginWidth must be greater than or equal to 0\n");
       newEl->extList.colWidthMargin = 0;
       }
       if (newEl->extList.itemHeightMargin < 0) {
       XtWarning("XiExtdList:Initialize:XiNitemMarginHeight must be greater than or equal to 0\n");
       newEl->extList.itemHeightMargin = 0;
       } */
    if (newEl->extList.rowCount < 0) {
	XtWarning("XiExtdList:Initialize:XiNrowCount must be greater than or equal to 0\n");
	newEl->extList.rowCount = 0;
    }
    if (newEl->extList.colCount < 0) {
	XtWarning("XiExtdList:Initialize:XiNcolumnCount must be greater than or equal to 0\n");
	newEl->extList.colCount = 0;
    }
    if ((newEl->extList.chLabelAlignment < XiEXTLIST_LABEL_ALIGNMENT_BEGINNING) ||
	(newEl->extList.chLabelAlignment > XiEXTLIST_LABEL_ALIGNMENT_END)) {
	XtWarning("XiExtdList:Initialize:XiNcolumnHeadingLabelAlignment must be one of \n\
XiEXTLIST_LABEL_ALIGNMENT_BEGINNING\n\
XiEXTLIST_LABEL_ALIGNMENT_CENTER\n\
XiEXTLIST_LABEL_ALIGNMENT_END\n");
	newEl->extList.chLabelAlignment = XiEXTLIST_LABEL_ALIGNMENT_BEGINNING;
    }
    if ((newEl->extList.itemLabelAlignment < XiEXTLIST_LABEL_ALIGNMENT_BEGINNING) ||
	(newEl->extList.itemLabelAlignment > XiEXTLIST_LABEL_ALIGNMENT_END)) {
	XtWarning("XiExtdList:Initialize:XiNcolumnHeadingLabelAlignment must be one of \n\
                        XiEXTLIST_LABEL_ALIGNMENT_BEGINNING\n\
                        XiEXTLIST_LABEL_ALIGNMENT_CENTER\n\
                        XiEXTLIST_LABEL_ALIGNMENT_END\n");
	newEl->extList.itemLabelAlignment = XiEXTLIST_LABEL_ALIGNMENT_BEGINNING;
    }
    
    if (newEl->extList.chBackground == 0)
	newEl->extList.chBackground = newEl->extList.background;
    if (newEl->extList.chForeground == 0)
	newEl->extList.chForeground = newEl->extList.foreground;
    
    newEl->extList.pixmap = (unsigned long)NULL;
    newEl->extList.extItem = NULL;
    newEl->extList.colWidth = NULL;
    newEl->extList.colSeparator = NULL;
    newEl->extList.func = NULL;
    newEl->extList.lastSelect = 0;
    newEl->extList.selectedCol = -1;
    newEl->extList.minColWidth = (newEl->manager.shadow_thickness * (Dimension)2) + 
	(newEl->extList.colWidthMargin * (Dimension)2);
    newEl->extList.selectedPos = NULL;
    newEl->extList.selectedItems = NULL;
    
    if ((newEl->extList.colCount > 0) && (newEl->extList.rowCount > 0)){
	LoadItems(newEl);
    }
    
    if (newEl->core.width <= 0) 
	newEl->core.width = newEl->extList.width = newEl->extList.totalColW  + 
	    XIEXTLIST_SCROLLBAR_WIDTH;
    if (newEl->core.height <= 0) 
	newEl->core.height = newEl->extList.height = 
	    newEl->extList.colTitleH + 
	    (newEl->extList.visibleCount * newEl->extList.itemH) +
	    XIEXTLIST_SCROLLBAR_WIDTH;
    
    /*newEl->extList.func = (XiExtdListSortProc *)XtMalloc(sizeof(XiExtdListSortProc) * newEl->extList.colCount);*/
    newEl->extList.func = (XiExtdListSortProc *)malloc(sizeof(XiExtdListSortProc) *
						       newEl->extList.colCount);
    for (i = 0; i < newEl->extList.colCount; i++)
	newEl->extList.func[i] = DefaultSortFunc;
    
    newEl->extList.topPosition = 1; 
    newEl->extList.clipX = 0;  
    
    shape = XCreatePixmapFromBitmapData(newEl->extList.dp,
					DefaultRootWindow(newEl->extList.dp),
					(char*)cursorBits,
					16,16,1, 0, 1);
    mask = XCreatePixmapFromBitmapData(newEl->extList.dp,
				       DefaultRootWindow(newEl->extList.dp),
				       (char*)cursorMaskBits,
				       16,16,1, 0, 1);
    fgColor.pixel = BlackPixelOfScreen(newEl->core.screen);
    bgColor.pixel = WhitePixelOfScreen(newEl->core.screen);
    XQueryColor(newEl->extList.dp,
		newEl->core.colormap,
		&fgColor);
    XQueryColor(newEl->extList.dp,
		newEl->core.colormap,
		&bgColor);
    newEl->extList.cursor = XCreatePixmapCursor(newEl->extList.dp, 
						shape,
						mask,
						&fgColor,
						&bgColor,
						8, 8);
    XFreePixmap(newEl->extList.dp, shape);
    XFreePixmap(newEl->extList.dp, mask);
    
    /* get default font for gcs */
    if (!XmFontListInitFontContext(&fontContext, newEl->extList.fontList))
	XtWarning("XiExtdList:Realize:Cannot initialize fontContext");
    else if ((flEntry = XmFontListNextEntry(fontContext)) == NULL) 
	XtWarning("XiExtdList:Realize:Cannot get FontListEntry");
    else if ((retFont = XmFontListEntryGetFont(flEntry, &retType)) == NULL)
	XtWarning("XiExtdList:Realize:Cannot get font from the FontListEntry");
    else if (retType == XmFONT_IS_FONT)
	font = ((XFontStruct *)retFont)->fid;
    else if (XFontsOfFontSet((XFontSet)retFont, &fontStruct, &fontName) < 1)
	XtWarning("XiExtdList:Realize:Cannot get Fonts of FontSet");
    else 
	font = fontStruct[0]->fid;
    if (fontContext)
	XmFontListFreeFontContext(fontContext);
    
    /* get topShadow, bottomShadow, and selectColor for the title bar */
    XmGetColors(newEl->core.screen, newEl->core.colormap, 
		newEl->extList.chBackground, 
		&fg,                         /* discard foreground */
		&(newEl->extList.topShadow),    
		&(newEl->extList.bottomShadow), 
		&(newEl->extList.selectColor));
    
    /* create normal gc */
    val.foreground = newEl->extList.foreground;
    val.background = newEl->extList.background;
    newEl->extList.gc = XCreateGC(newEl->extList.dp, 
				  DefaultRootWindow(newEl->extList.dp),
				  GCForeground | GCBackground, &val);
    if (font)
	XSetFont(newEl->extList.dp, newEl->extList.gc, font);
    
    /* create stippled gc */
    val.stipple = XCreatePixmapFromBitmapData(newEl->extList.dp,
					      DefaultRootWindow(newEl->extList.dp),
					      (char*)stippleBits, 2, 2, 
					      newEl->extList.foreground,
					      newEl->extList.background, 1);
    val.fill_style = FillStippled;
    newEl->extList.stippledGC = XCreateGC(newEl->extList.dp, 
					  DefaultRootWindow(newEl->extList.dp),
					  GCStipple | GCFillStyle, &val);
    if (font)
	XSetFont(newEl->extList.dp, newEl->extList.stippledGC, font);
    
    /* create resizeCol gc */
    val.foreground = newEl->extList.background;
    val.background = newEl->extList.foreground;
    val.function   = GXxor;
    val.line_width = 2;
    newEl->extList.resizeColGC = XCreateGC(newEl->extList.dp, 
					   DefaultRootWindow(newEl->extList.dp),
					   GCForeground | GCBackground | GCFunction 
					   | GCLineWidth, &val);
    
    /* get multiClickTime */
    newEl->extList.multiClickTime = XtGetMultiClickTime(newEl->extList.dp);
    GetNewPixmap(newEl);
    
  for(i = 0; i < newEl->extList.colCount; i++)
      DrawColumnHeading(newEl, i, XIEXTLIST_BUTTON_UP); 
  DrawList(newEl);
}
  

/*
 * Realize()
 * realize method
 */
PRIVATE void Realize(Widget w, XtValueMask *valueMask,
		     XSetWindowAttributes *attrib)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i;
    
    (*sclass->core_class.realize)(w, valueMask, attrib);
    XSetWindowBorderWidth(el->extList.dp, el->core.window, 1);
    
    el->extList.width = el->core.width - (Dimension)XIEXTLIST_SCROLLBAR_WIDTH;
    el->extList.height = el->core.height - (Dimension)XIEXTLIST_SCROLLBAR_WIDTH;
    el->extList.visibleCount = ((int)el->extList.height - 
				(int)el->extList.colTitleH) / (int)el->extList.itemH;
    XtConfigureWidget(el->extList.vscroll,
		      (Position)el->extList.width, (Position)0,
		      XIEXTLIST_SCROLLBAR_WIDTH,
		      el->extList.height, 0);
  XtConfigureWidget(el->extList.hscroll,
		    0, (Position)el->extList.height,
		    el->extList.width + XIEXTLIST_SCROLLBAR_WIDTH, 
		    XIEXTLIST_SCROLLBAR_WIDTH, 0);
  XtVaSetValues(el->extList.vscroll,
		XmNminimum,         1,
		XmNmaximum,         MAX(el->extList.rowCount, 2),
		XmNsliderSize,      MIN(el->extList.visibleCount,
					MAX(el->extList.rowCount - 1, 1)),
		XmNvalue,           1,
		NULL);
  XtVaSetValues(el->extList.hscroll,
		XmNminimum,         0,
		XmNmaximum,         el->extList.pixmapWidth,
		NULL);
  RecalcGeometry(el);
  GetNewPixmap(el);
  for(i = 0; i < el->extList.colCount; i++)
      DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
  DrawList(el);
  RefreshAll(el);
}



PRIVATE void Redisplay(Widget w, XEvent *event, Region region)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    /* done to get rid of compiler warning about unused parameters */
    event=event;
    region=region;
    
    RefreshAll(el);
}


/*
 * Resize()
 * resize method
 * Called in response to resize event
 */
PRIVATE void Resize(Widget w)
{
    XiExtdListWidget el = (XiExtdListWidget) w;
    Dimension oldW = el->extList.width;
    int oldVisibleCount = el->extList.visibleCount;
    int i;
    
    el->extList.width = el->core.width - (Dimension)XIEXTLIST_SCROLLBAR_WIDTH;
    el->extList.height = el->core.height - (Dimension)XIEXTLIST_SCROLLBAR_WIDTH;
    el->extList.visibleCount = ((int)el->extList.height - 
				(int)el->extList.colTitleH) / (int)el->extList.itemH;
    
    if (el->extList.width > oldW) {
	el->extList.clipX = MAX((Position)0,
				el->extList.clipX - ((Position)el->extList.width - 
						     (Position)oldW));
    }
    if (el->extList.visibleCount > oldVisibleCount) {
	el->extList.topPosition = MAX(1,
				      el->extList.topPosition - 
				      (el->extList.visibleCount - oldVisibleCount));
    }
    
    GetNewPixmap(el);
    
  XtConfigureWidget(el->extList.vscroll,
		    (Position)el->extList.width, (Position)0,
		    XIEXTLIST_SCROLLBAR_WIDTH,
		    el->extList.height, 0);
  XtConfigureWidget(el->extList.hscroll,
		    0, (Position)el->extList.height,
		    el->extList.width + XIEXTLIST_SCROLLBAR_WIDTH, 
		    XIEXTLIST_SCROLLBAR_WIDTH, 0);
  
  for(i = 0; i < el->extList.colCount; i++)
      DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
  DrawList(el);
  if (XtIsRealized(w))
      RefreshAll(el);
} 


/*
 * Destroy()
 * Called when this widget is destroyed
 * This will free up any allocated memory
 */
PRIVATE void Destroy(Widget w)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    UnLoadItems(el);
    XtFree((char *)el->extList.func);
    XFreeGC(el->extList.dp, el->extList.gc);
    XFreeGC(el->extList.dp, el->extList.stippledGC);
    XFreeGC(el->extList.dp, el->extList.resizeColGC);
    XFreePixmap(el->extList.dp, el->extList.pixmap);
    XtDestroyWidget(el->extList.vscroll);
    XtDestroyWidget(el->extList.hscroll);
    XmFontListFree(el->extList.fontList);
    XtFree((char *)el->extList.browseSelectCB);
    XtFree((char *)el->extList.defaultActionCB);
    XtFree((char *)el->extList.extSelectionCB);
    XtFree((char *)el->extList.multiSelectCB);
    XtFree((char *)el->extList.singleSelectCB);
}

/*
 * SetValues()
 * Called in response to Xt[Va]SetValues()
 */
PRIVATE Boolean SetValues(Widget   current, Widget   request, Widget   new,
			  ArgList  args, Cardinal *numArgs)
{
    XiExtdListWidget currEl = (XiExtdListWidget)current;
    XiExtdListWidget newEl = (XiExtdListWidget)new;
    int i;
    Boolean recalc = False, getpixmap = False;
    
    /* done to get rid of compiler warning about unused parameters */
    request=request;
    args=args;
    numArgs=numArgs;
    
    if ((currEl->manager.shadow_thickness != newEl->manager.shadow_thickness) ||
	(currEl->extList.fontList != newEl->extList.fontList) ||
	(currEl->extList.colCount != newEl->extList.colCount) ||
	(currEl->extList.rowCount != newEl->extList.rowCount) ||
	(currEl->extList.chHeightMargin != newEl->extList.chHeightMargin) ||
	(currEl->extList.itemHeightMargin != newEl->extList.itemHeightMargin) ||
	(currEl->extList.colWidthMargin != newEl->extList.colWidthMargin)) {
	recalc = True;
	getpixmap = True;
    }
    if ((currEl->extList.background != newEl->extList.background) ||
	(currEl->extList.foreground != newEl->extList.foreground) || 
	(currEl->extList.chBackground != newEl->extList.chBackground) ||
	(currEl->extList.chForeground != newEl->extList.chForeground)) {
	XGCValues val;
	val.foreground = newEl->extList.foreground;
	val.background = newEl->extList.background;
	XChangeGC(newEl->extList.dp, newEl->extList.stippledGC, GCForeground |
		  GCBackground, &val);
	val.foreground = newEl->extList.background;
	val.background = newEl->extList.foreground;
	XChangeGC(newEl->extList.dp, newEl->extList.resizeColGC, GCForeground |
		  GCBackground, &val);
	for (i = 1; i < newEl->extList.rowCount; i++) {
	    if (newEl->extList.extItem[i].foreground == currEl->extList.foreground)
		newEl->extList.extItem[i].foreground = newEl->extList.foreground;
	}
	if (currEl->extList.chBackground != newEl->extList.chBackground) {
	    Pixel fg;
	    XmGetColors(newEl->core.screen, newEl->core.colormap, 
			newEl->extList.chBackground, 
			&fg,                         /* discard foreground */
			&(newEl->extList.topShadow),    
			&(newEl->extList.bottomShadow), 
			&(newEl->extList.selectColor));
	}
    }
    if (currEl->extList.items != newEl->extList.items) {
	UnLoadItems(currEl);
	LoadItems(newEl);
	recalc = True;
	getpixmap = True;
    }
    if (recalc)
	RecalcGeometry(newEl);
    if (getpixmap)
	GetNewPixmap(newEl);
    for (i = 0; i < newEl->extList.colCount; i++)
	DrawColumnHeading(newEl, i, (i == newEl->extList.selectedCol) ? 
			  XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
    DrawList(newEl);
  if (XtIsRealized(new))
      RefreshAll(newEl);
  return True;
}


/*
 * QueryGeometry()
 * QueryGeometry method
 * simpley returns XtGeometryYes
 */
PRIVATE XtGeometryResult QueryGeometry(Widget w, XtWidgetGeometry *intended,
				       XtWidgetGeometry *reply)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    intended=intended;
    reply=reply;
    
    return XtGeometryYes;
}


/*
 * GeometryHandler()
 * Geometry handler method.
 * simply returns XtGeometryYes
 */
PRIVATE XtGeometryResult GeometryHandler(Widget w, XtWidgetGeometry *request,
					 XtWidgetGeometry *reply)
{
    /* done to get rid of compiler warning about unused parameters */
    w=w;
    request=request;
    reply=reply;
    
    return XtGeometryYes;
}



/*
 * InsertChild()
 * InsertChild method of composite class
 * This disallows any other widget from being a child of this widget
 * from applications.
 */
PRIVATE void InsertChild(Widget w)
{
    (*sclass->composite_class.insert_child)(w);
}

/*************************************************************************************
 * internal functions
 ************************************************************************************/

PRIVATE void ButtonClickHandler(Widget w, XEvent *event, String *param,
				Cardinal *numParam)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int colIndex, rowIndex;
    static int lastTime = 0;
    int i;
    ExtdListItemStruct *tmp = NULL;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    MapYToRowIndex(el, event->xbutton.y, &rowIndex);
    if (rowIndex < 0) {
	el->extList.btnDownType = 0;
	return;
    }
    if (rowIndex == 0) {   /* clicked on the title */
	if (!MapXToColumn(el, event->xbutton.x, &colIndex)) {  /* clicked on the border */
	    if (colIndex) {   /* not clicked on the first border */
		el->extList.btnDownType = BTNDOWNTYPE_RESIZE;
		el->extList.resizePos = event->xbutton.x;
		el->extList.resizeIndex = colIndex;
		XDrawLine(el->extList.dp,
			  el->core.window,
			  el->extList.resizeColGC,
			  el->extList.resizePos, 0,
			  el->extList.resizePos, (int)el->extList.height);
	    }
	}
	else {   /* clicked on the title button */
	    el->extList.btnDownType = BTNDOWNTYPE_TITLE;
	    if (el->extList.selectedCol == colIndex)
		return;
	    if (!el->extList.allowSort) 
		return;
	    if (el->extList.selectedCol >= 0)
		DrawColumnHeading(el, el->extList.selectedCol, XIEXTLIST_BUTTON_UP);
	    DrawColumnHeading(el, colIndex, XIEXTLIST_BUTTON_DOWN);
	    if (el->extList.selectedCol >= 0)
		RefreshButton(el, el->extList.selectedCol);
	    RefreshButton(el, colIndex);
	    if (el->extList.lastSelect >= 1) {
		/*tmp = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct));*/
		tmp = (ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct));
		memcpy(tmp, &el->extList.extItem[el->extList.lastSelect], 
		       sizeof(ExtdListItemStruct));
	    }
	    SortList(el, colIndex);
	    el->extList.selectedCol = colIndex;
	    if (tmp)
		for (i = 0; i < el->extList.rowCount; i++) {
		    Boolean found = True;
		    int j;
		    for (j = 0; j < el->extList.colCount; j++)
			if (!XmStringCompare(tmp->item[j], 
					     el->extList.extItem[i].item[j])) {
			    found = False;
			    break;
			}
	  if (found && 
	      (el->extList.extItem[i].sensitive == tmp->sensitive) &&
	      (el->extList.extItem[i].selected == tmp->selected) &&
	      (el->extList.extItem[i].foreground == tmp->foreground)) {
	      el->extList.lastSelect = i;
	      break;
	  }
		}
	}
    }
    else{   /* clicked on the list */
	rowIndex = rowIndex + el->extList.topPosition - 1;
	if (rowIndex >= el->extList.rowCount)
	    rowIndex = el->extList.rowCount - 1;
	if (!el->extList.extItem[rowIndex].sensitive) {
	    el->extList.btnDownType = BTNDOWNTYPE_TITLE;
      return;
	}
	el->extList.btnDownType = BTNDOWNTYPE_LIST;
	if ((unsigned long)(lastTime + el->extList.multiClickTime) > 
	    event->xbutton.time) {   /* double click */
	    CallCallbacks(el, event, XiCR_DEFAULT_ACTION);
	}
	else {   /* single click */
	    if (el->extList.selectionType == XiEXTLIST_SINGLE_SELECT) {
		if (rowIndex == el->extList.lastSelect)
		    el->extList.extItem[rowIndex].selected = 
			el->extList.extItem[rowIndex].selected ? False:True;
		else {
		    el->extList.extItem[el->extList.lastSelect].selected = False;
		    el->extList.extItem[rowIndex].selected = True;
	}
		el->extList.lastSelect = rowIndex;
      }
	    else if (el->extList.selectionType == XiEXTLIST_BROWSE_SELECT) {
		el->extList.extItem[el->extList.lastSelect].selected = False;
		el->extList.extItem[rowIndex].selected = True;
		el->extList.lastSelect = rowIndex;
      }
	    else if (el->extList.selectionType == XiEXTLIST_MULTIPLE_SELECT) {
		el->extList.extItem[rowIndex].selected = 
		    el->extList.extItem[rowIndex].selected ? False:True;
		el->extList.lastSelect = rowIndex;
	    }
	    else if (el->extList.selectionType == XiEXTLIST_EXTENDED_SELECT) {
		if (event->xbutton.state & ShiftMask) {
		    UnselectAll(el);
		    if (rowIndex > el->extList.selectStart) {
			for ( i = el->extList.selectStart; i <= rowIndex; i++)
			    el->extList.extItem[i].selected = True;
		    }
		    else {
			for (i = rowIndex; i <= el->extList.selectStart; i++)
			    el->extList.extItem[i].selected = True;
		    }
		    el->extList.lastSelect = rowIndex;
		}
		else if (event->xbutton.state & ControlMask) {
		    el->extList.extItem[rowIndex].selected = 
			el->extList.extItem[rowIndex].selected ? False:True;
		    el->extList.lastSelect = rowIndex;
		    el->extList.selectStart = rowIndex;
		}
		else {
		    UnselectAll(el);
		    el->extList.extItem[rowIndex].selected = True;
		    el->extList.lastSelect = rowIndex;
		    el->extList.selectStart = rowIndex;
		}
	    }
	    DrawList(el);
	    RefreshAll(el);
	}
    }
    lastTime = event->xbutton.time;
}



PRIVATE void    ButtonReleaseHandler(Widget w, XEvent *event, String *param,
				     Cardinal *numParam)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int colIndex;
    int i;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    if (el->extList.btnDownType == BTNDOWNTYPE_RESIZE) {
	XDrawLine(el->extList.dp,
		  el->core.window,
		  el->extList.resizeColGC,
		  el->extList.resizePos, 0,
		  el->extList.resizePos, (int)el->extList.height);
	colIndex = el->extList.resizeIndex;
	
	el->extList.colSeparator[colIndex] = (Position)(el->extList.resizePos + 
							(int)el->extList.clipX);
	el->extList.colWidth[colIndex - 1] = 
	    (Dimension)el->extList.colSeparator[colIndex] - 
	    (Dimension)el->extList.colSeparator[colIndex - 1];
	
	for (i = colIndex + 1; i < el->extList.colCount; i++)
	    el->extList.colSeparator[i] = el->extList.colSeparator[i - 1] + 
		(Position)el->extList.colWidth[i - 1];
	el->extList.totalColW = 
	    (Dimension)el->extList.colSeparator[el->extList.colCount - 1] + 
	    el->extList.lastColW;
	GetNewPixmap(el);
	for (i = 0; i < el->extList.colCount; i++)
	    DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			      XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
	DrawList(el);
	RefreshAll(el);
    }
    else if (el->extList.btnDownType == BTNDOWNTYPE_LIST) {
	CallCallbacks(el, event, el->extList.selectionType);
    }
}


PRIVATE void    MouseMotionHandler(Widget w, XEvent *event, String *param,
				   Cardinal *numParam)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int colIndex, rowIndex;
    static int undo = False;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    MapYToRowIndex(el, event->xbutton.y, &rowIndex);
    if (rowIndex == 0) {
	if (!MapXToColumn(el, event->xbutton.x, &colIndex)) {  /* pointer on the border */
	    if (colIndex) {
		XDefineCursor(el->extList.dp, el->core.window, el->extList.cursor);
		undo = True;
	    }
	    else if (undo) {
		XUndefineCursor(el->extList.dp, el->core.window);
		undo = False;
	    }
	}
	else if (undo) {
	    XUndefineCursor(el->extList.dp, el->core.window);
	    undo = False;
	}
    }
    else if (undo) {
	XUndefineCursor(el->extList.dp, el->core.window);
	undo = False;
    }
}


PRIVATE void ButtonMotionHandler(Widget w, XEvent *event, String *param,
				 Cardinal *numParam)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int rowIndex;
    int i;
    
    /* done to get rid of compiler warning about unused parameters */
    param=param;
    numParam=numParam;
    
    if (el->extList.btnDownType == BTNDOWNTYPE_RESIZE) {
	int lowBound = MAX(0, (int)el->extList.colSeparator[el->extList.resizeIndex
							   - 1] - 
			   (int)el->extList.clipX + 
			   (int)el->extList.minColWidth);
	int upperBound = (int)el->extList.width;
	if ((event->xbutton.x > lowBound) && (event->xbutton.x < upperBound)) {
	    XDrawLine(el->extList.dp,
		      el->core.window,
		      el->extList.resizeColGC,
		      el->extList.resizePos, 0,
		      el->extList.resizePos, (int)el->extList.height);
	    el->extList.resizePos = event->xbutton.x;
	    XDrawLine(el->extList.dp,
		      el->core.window,
		      el->extList.resizeColGC,
		      el->extList.resizePos, 0,
		      el->extList.resizePos, (int)el->extList.height);
	}
    }
    else if (el->extList.btnDownType == BTNDOWNTYPE_LIST) {
	MapYToRowIndex(el, event->xbutton.y, &rowIndex);
	if (el->extList.selectionType == XiEXTLIST_BROWSE_SELECT) {
	    if ((rowIndex + el->extList.topPosition - 1) == el->extList.lastSelect)
		return;
	    if (rowIndex < 1) {   /* scroll up */
		if (el->extList.topPosition > 1) {
		    el->extList.topPosition -= 1;
		    XtVaSetValues(el->extList.vscroll,
				  XmNvalue,       MAX(el->extList.topPosition, 1),
				  NULL);
		    el->extList.extItem[el->extList.lastSelect].selected = False;
		    if (el->extList.extItem[el->extList.topPosition].sensitive) {
			el->extList.extItem[el->extList.topPosition].selected = True;
			el->extList.lastSelect = el->extList.topPosition;
		    }
		}
	    }
	    else if (rowIndex >= el->extList.visibleCount) {    /* scroll down */
		if (el->extList.topPosition < (el->extList.rowCount - 
					       el->extList.visibleCount)) {
		    el->extList.topPosition += 1;
		    XtVaSetValues(el->extList.vscroll,
				  XmNvalue,       MAX(el->extList.topPosition, 1),
				  NULL);
		    el->extList.extItem[el->extList.lastSelect].selected = False;
		    if (el->extList.extItem[el->extList.topPosition + 
					   el->extList.visibleCount - 1].sensitive) {
			el->extList.extItem[el->extList.topPosition + 
					   el->extList.visibleCount - 1].selected =
			    True;
			el->extList.lastSelect = el->extList.topPosition + 
			    el->extList.visibleCount - 1;
		    }
		}
		else if (el->extList.lastSelect != (el->extList.rowCount - 1)) {
		    el->extList.extItem[el->extList.lastSelect].selected = False;
		    if (el->extList.extItem[el->extList.rowCount - 1].sensitive) {
			el->extList.extItem[el->extList.rowCount - 1].selected =True;
			el->extList.lastSelect = el->extList.rowCount - 1;
		    }
		}
	    }
	    else {       /* no need to scroll */
		el->extList.extItem[el->extList.lastSelect].selected = False;
		if (el->extList.extItem[rowIndex+el->extList.topPosition-1].sensitive) {
		    el->extList.extItem[rowIndex+el->extList.topPosition-1].selected
			= True;
		    el->extList.lastSelect = rowIndex + el->extList.topPosition - 1;
		}
	    }
	    DrawList(el);
	    RefreshAll(el);
	}  /* browse select */
	else if (el->extList.selectionType == XiEXTLIST_EXTENDED_SELECT) {
	    if ((rowIndex + el->extList.topPosition - 1) == el->extList.lastSelect)
		return;
	    if (rowIndex < 1) {   /* scroll up */
		if (el->extList.topPosition > 1) {
		    el->extList.topPosition -= 1;
		    XtVaSetValues(el->extList.vscroll,
				  XmNvalue,       MAX(el->extList.topPosition, 1),
				  NULL);
		    UnselectAll(el);
		    if (el->extList.extItem[el->extList.topPosition].sensitive)
			el->extList.lastSelect = el->extList.topPosition;
		    if ( el->extList.lastSelect > el->extList.selectStart) {
			for (i = el->extList.selectStart; 
			     i <= el->extList.lastSelect; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
			    }
		    }
		    else {
			for (i = el->extList.lastSelect; 
			     i <= el->extList.selectStart; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
	      }
		    }
		}
	    }
	    else if (rowIndex > el->extList.visibleCount) {    /* scroll down */
		if (el->extList.topPosition < (el->extList.rowCount - 
					       el->extList.visibleCount)) {
		    el->extList.topPosition += 1;
		    XtVaSetValues(el->extList.vscroll,
				  XmNvalue,       MAX(el->extList.topPosition, 1),
				  NULL);
		    UnselectAll(el);
		    if (el->extList.extItem[el->extList.topPosition + 
					   el->extList.visibleCount - 1].sensitive)
			el->extList.lastSelect = el->extList.topPosition + 
			    el->extList.visibleCount - 1;
		    if ( el->extList.lastSelect > el->extList.selectStart) {
			for (i = el->extList.selectStart; i <= 
				 el->extList.lastSelect; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
			    }
		    }
		    else {
			for (i = el->extList.lastSelect; i <= 
				 el->extList.selectStart; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
			    }
		    }
		}
		else if (el->extList.lastSelect != (el->extList.rowCount - 1)) {
		    UnselectAll(el);
		    if (el->extList.extItem[el->extList.rowCount - 1].sensitive)
			el->extList.lastSelect = el->extList.rowCount - 1;
		    if ( el->extList.lastSelect > el->extList.selectStart) {
			for (i = el->extList.selectStart; 
			     i <= el->extList.lastSelect; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
			    }
		    }
		    else {
			for (i = el->extList.lastSelect; 
			     i <= el->extList.selectStart; i++)
			    if (el->extList.extItem[i].sensitive) {
				el->extList.extItem[i].selected = True;
			    }
		    }
		}
	    }
	    else {       /* no need to scroll */
		UnselectAll(el);
		if (el->extList.extItem[rowIndex+el->extList.topPosition-1].sensitive)
		    el->extList.lastSelect = rowIndex + el->extList.topPosition - 1;
		if ( el->extList.lastSelect > el->extList.selectStart) {
		    for (i = el->extList.selectStart; i <= 
			     el->extList.lastSelect; i++)
			if (el->extList.extItem[i].sensitive) {
			    el->extList.extItem[i].selected = True;
			}
		}
		else {
		    for (i = el->extList.lastSelect; i <= 
			     el->extList.selectStart; i++)
			if (el->extList.extItem[i].sensitive) {
			    el->extList.extItem[i].selected = True;
			}
		}
	    }
	    DrawList(el);
	    RefreshAll(el);
	}  /* extended select */
    }   /* btnDownType == list */
}


PRIVATE void ScrollBarHandler(Widget w, XtPointer clientData, XtPointer callData)
{
    XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)callData;
    XiExtdListWidget el = (XiExtdListWidget)clientData;
    
    if (w == el->extList.vscroll) {
	el->extList.topPosition = cbs->value;
	DrawList(el);
	RefreshAll(el);
    }
    else {
	el->extList.clipX = (Position)cbs->value;
	RefreshAll(el);
    }
}



PRIVATE Boolean MapXToColumn(XiExtdListWidget el,  int x,  int *index)
{
    int i;
    Position pixmapX = (Position)x + el->extList.clipX;
    
    for (i = 0; i < el->extList.colCount; i++) {
	if (pixmapX == el->extList.colSeparator[i]) {
	    *index = i;
	    return False;
	}
	if (pixmapX < el->extList.colSeparator[i]) {
	    *index = i - 1;
	    return True;
	}
    }
    *index = el->extList.colCount - 1;
    return True;
}



PRIVATE void    MapYToRowIndex(XiExtdListWidget el,  int y,  int *rowIndex)
{
    if (y < 0)
	*rowIndex = -1;
    else if (y < (int)el->extList.colTitleH)
	*rowIndex = 0;
    else {
	y -= (int)el->extList.colTitleH;
	*rowIndex = (y / (int)el->extList.itemH) + 1;
	if (*rowIndex > el->extList.rowCount)
	    *rowIndex = -1;
    }
}


PRIVATE int  DefaultSortFunc(XmString xms1, XmString xms2)
{
    char *text1, *text2;
    int returnVal;
    
    XmStringGetLtoR(xms1, XmFONTLIST_DEFAULT_TAG, &text1);
    XmStringGetLtoR(xms2, XmFONTLIST_DEFAULT_TAG, &text2);
    
    returnVal = strcmp(text1, text2);
    
    XtFree(text1);
    XtFree(text2);
    return returnVal;
}


PRIVATE void LoadItems(XiExtdListWidget el)
{
    int i,j;
    
    UnLoadItems(el);
    
    /*el->extList.extItem = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * (el->extList.rowCount));*/
    el->extList.extItem = (ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct) *
						       (el->extList.rowCount));
    
    /* load column heading */
    /*el->extList.extItem[0].item = (XmString *)XtMalloc(sizeof(XmString) * (el->extList.colCount));*/
    el->extList.extItem[0].item = (XmString *)malloc(sizeof(XmString) *
						     (el->extList.colCount));
    for (i = 0; i < el->extList.colCount; i++) {
	if (el->extList.itemType == XiEXTLIST_ITEM_TYPE_NON_STRUCTURED)
	    el->extList.extItem[0].item[i] = 
		XmStringCopy(((XmString *)el->extList.items)[i]);
	else
	    el->extList.extItem[0].item[i] = 
		XmStringCopy(((ExtdListItemStruct *)el->extList.items)[0].item[i]);
	
	if (el->extList.itemType == XiEXTLIST_ITEM_TYPE_NON_STRUCTURED) {
	    el->extList.extItem[0].sensitive = True;
	    el->extList.extItem[0].selected = False;
	    el->extList.extItem[0].foreground = el->extList.chForeground;
	    el->extList.extItem[0].hook = NULL;
	}
	else {
	    el->extList.extItem[0].sensitive = 
		((ExtdListItemStruct *)el->extList.items)[0].sensitive;
	    el->extList.extItem[0].selected = 
		((ExtdListItemStruct *)el->extList.items)[0].selected;
	    el->extList.extItem[0].foreground = 
		((ExtdListItemStruct *)el->extList.items)[0].foreground ? 
		((ExtdListItemStruct *)el->extList.items)[0].foreground:el->extList.foreground;
	    el->extList.extItem[0].hook = 
		((ExtdListItemStruct *)el->extList.items)[0].hook;
	}
    }
    
    /* load actual list items */
    for (i = 1; i < el->extList.rowCount; i++) {
	/*el->extList.extItem[i].item = (XmString *)XtMalloc(sizeof(XmString) * (el->extList.colCount));*/
	el->extList.extItem[i].item = (XmString *)malloc(sizeof(XmString) * 
							 (el->extList.colCount));
	for (j = 0; j < el->extList.colCount; j++) {
	    if (el->extList.itemType == XiEXTLIST_ITEM_TYPE_NON_STRUCTURED) 
		el->extList.extItem[i].item[j] = 
		    XmStringCopy(((XmString *)el->extList.items)[(i * el->extList.colCount) + j]);
	    else 
		el->extList.extItem[i].item[j] = 
		    XmStringCopy(((ExtdListItemStruct *)el->extList.items)[i].item[j]);
	}
	if (el->extList.itemType == XiEXTLIST_ITEM_TYPE_NON_STRUCTURED) {
	    el->extList.extItem[i].sensitive = True;
	    el->extList.extItem[i].selected = False;
	    el->extList.extItem[i].foreground = el->extList.foreground;
	    el->extList.extItem[i].hook = NULL;
	}
	else {
	    el->extList.extItem[i].sensitive = 
		((ExtdListItemStruct *)el->extList.items)[i].sensitive;
	    el->extList.extItem[i].selected = 
		((ExtdListItemStruct *)el->extList.items)[i].selected;
	    el->extList.extItem[i].foreground = 
		((ExtdListItemStruct *)el->extList.items)[i].foreground ? 
		((ExtdListItemStruct *)el->extList.items)[i].foreground:el->extList.foreground;
	    el->extList.extItem[i].hook = 
		((ExtdListItemStruct *)el->extList.items)[i].hook;
	}
    }
    if (el->extList.rowCount > 1)
	el->extList.topPosition = 1;
    else
	el->extList.topPosition = 0;
    el->extList.items = (XtPointer)el->extList.extItem;
    RecalcGeometry(el);
}
  

PRIVATE void UnLoadItems(XiExtdListWidget el)
{
    int i, j;
    
    if (el->extList.extItem) {
	for (i = 0; i < el->extList.rowCount; i++) { 
	    for (j = 0; j < el->extList.colCount; j++) {
		if (el->extList.extItem[i].item[j])
		    XmStringFree(el->extList.extItem[i].item[j]);
	    }
	}
    }
    
    if (el->extList.extItem)
	XtFree((char *)el->extList.extItem);
    if (el->extList.colSeparator)
	XtFree((char *)el->extList.colSeparator);
    if (el->extList.colWidth)
	XtFree((char *)el->extList.colWidth);
}



PRIVATE void DrawList(XiExtdListWidget el)
{
    int i, j;
    int numRow;
    Position y = (Position)el->extList.colTitleH;
    XRectangle clipRec;
    Dimension wPad = el->extList.colWidthMargin,
	hPad = el->extList.itemHeightMargin;
    GC gc;
    
    if (!el->extList.pixmap)
	return;
    
    XSetForeground(el->extList.dp, el->extList.gc, el->extList.background);
    XFillRectangle(el->extList.dp,
		   el->extList.pixmap,
		   el->extList.gc,
		   0, (int)y,
		   (unsigned int)el->extList.pixmapWidth,
		   (unsigned int)el->extList.pixmapHeight - 
		   (unsigned int)el->extList.colTitleH);
    
    if (el->extList.rowCount < 2) 
	return;
    
    numRow = MIN(el->extList.visibleCount, el->extList.rowCount - 
		 el->extList.topPosition);
    
    for (i = el->extList.topPosition; i < el->extList.topPosition + numRow; i++) {
	if (!el->extList.extItem[i].sensitive) 
	    gc = el->extList.stippledGC;
	else
	    gc = el->extList.gc;
	
	if (el->extList.extItem[i].selected && el->extList.extItem[i].sensitive) {
	    XSetForeground(el->extList.dp, gc, el->extList.extItem[i].foreground ? 
			   el->extList.extItem[i].foreground:el->extList.foreground);
	    XFillRectangle(el->extList.dp,
			   el->extList.pixmap,
			   gc,
			   0, (int)y,
			   (unsigned int)el->extList.pixmapWidth,
			   (unsigned int)el->extList.itemH);
	    XSetForeground(el->extList.dp, gc, el->extList.background);
	}
	else 
	    XSetForeground(el->extList.dp, gc, el->extList.extItem[i].foreground ? 
			   el->extList.extItem[i].foreground:el->extList.foreground);
	
	for (j = 0; j < el->extList.colCount; j++) {
	    clipRec.x = (short)el->extList.colSeparator[j] + (short)wPad;
	    clipRec.y = (short)y + (short)hPad;
	    clipRec.width = (unsigned short)el->extList.colWidth[j] - 
		(unsigned short)(wPad * 2);
	    clipRec.height = (unsigned short)el->extList.itemH - 
		(unsigned short)(hPad * 2);
	    XmStringDraw(el->extList.dp,
			 el->extList.pixmap,
		   el->extList.fontList,
			 el->extList.extItem[i].item[j],
			 gc,
			 (Position)clipRec.x,
			 (Position)clipRec.y,
			 (Dimension)clipRec.width,
			 (el->extList.itemLabelAlignment == 
			  XiEXTLIST_LABEL_ALIGNMENT_BEGINNING) ? 
			 XmALIGNMENT_BEGINNING:
			 (el->extList.itemLabelAlignment == 
			  XiEXTLIST_LABEL_ALIGNMENT_CENTER) ? 
			 XmALIGNMENT_CENTER:XmALIGNMENT_END,
			 el->manager.string_direction,
			 &clipRec);
	}
	y += (Position)el->extList.itemH;
    }
    if (el->extList.showColSeparator) {
	/*XSegment *seg = (XSegment *)XtMalloc(sizeof(XSegment) * (el->extList.colCount - 1)); */
	XSegment *seg = (XSegment *)malloc(sizeof(XSegment) * 
					   (el->extList.colCount - 1)); 
	for (i = 0; i < (el->extList.colCount - 1); i++) {
	    seg[i].x1 = seg[i].x2 = (short)el->extList.colSeparator[i + 1];
	    seg[i].y1 = (short)el->extList.colTitleH;
	    seg[i].y2 = (short)el->extList.pixmapHeight;
	}
	XSetForeground(el->extList.dp, el->extList.gc, el->extList.foreground);
	XDrawSegments(el->extList.dp, el->extList.pixmap, el->extList.gc, seg, 
		      el->extList.colCount - 1);
	XtFree((char *)seg);
    } 
}



PRIVATE void DrawColumnHeading(XiExtdListWidget el, int colNum,  int upDown)
{
    Position bigX = el->extList.colSeparator[colNum], 
	bigY = (Position)0, 
	smallX = el->extList.colSeparator[colNum] + 
	(Position)el->manager.shadow_thickness, 
	smallY = (Position)el->manager.shadow_thickness;
    Dimension bigWidth = el->extList.colWidth[colNum], 
	bigHeight = el->extList.colTitleH, 
	smallWidth = el->extList.colWidth[colNum] - 
	(el->manager.shadow_thickness * (Dimension)2), 
	smallHeight = el->extList.colTitleH - 
	(el->manager.shadow_thickness * (Dimension)2);
    XPoint topPoints[6], bottomPoints[6];
    XRectangle clipRec;
    
    if (colNum < 0) 
	return;
    
    topPoints[0].x = (short)bigX;
    topPoints[0].y = (short)bigHeight;
    topPoints[1].x = (short)bigX;
    topPoints[1].y = (short)bigY;
    topPoints[2].x = (short)bigX + (short)bigWidth;
    topPoints[2].y = (short)bigY;
    topPoints[3].x = (short)smallX + (short)smallWidth;
    topPoints[3].y = (short)smallY;
    topPoints[4].x = (short)smallX;
    topPoints[4].y = (short)smallY;
    topPoints[5].x = (short)smallX;
    topPoints[5].y = (short)smallY + (short)smallHeight;
    bottomPoints[0].x = (short)bigX;
    bottomPoints[0].y = (short)bigHeight;
    bottomPoints[1].x = (short)bigX + (short)bigWidth;
    bottomPoints[1].y = (short)bigHeight;
    bottomPoints[2].x = (short)bigX + (short)bigWidth;
    bottomPoints[2].y = (short)bigY;
    bottomPoints[3].x = (short)smallX + (short)smallWidth;
    bottomPoints[3].y = (short)smallY;
    bottomPoints[4].x = (short)smallX + (short)smallWidth;
    bottomPoints[4].y = (short)smallY + (short)smallHeight;
    bottomPoints[5].x = (short)smallX;
    bottomPoints[5].y = (short)smallY + (short)smallHeight;
    
    clipRec.x = (short)smallX + (short)el->extList.colWidthMargin;
    clipRec.y = (short)smallY + (short)el->extList.chHeightMargin;
    clipRec.width = (unsigned short)smallWidth - 
	((unsigned short)el->extList.colWidthMargin * (unsigned short)2);
    clipRec.height = (unsigned short)smallHeight - 
	((unsigned short)el->extList.chHeightMargin * (unsigned short)2);
    
    if (el->manager.shadow_thickness > 0) {
	if (upDown == XIEXTLIST_BUTTON_DOWN)
	    XSetForeground(el->extList.dp, el->extList.gc, el->extList.bottomShadow);
	else
	    XSetForeground(el->extList.dp, el->extList.gc, el->extList.topShadow);
	XFillPolygon(el->extList.dp,
		     el->extList.pixmap,
		     el->extList.gc,
		     topPoints,
		     6,
		     Nonconvex,
		     CoordModeOrigin);
	if (upDown == XIEXTLIST_BUTTON_DOWN)
	    XSetForeground(el->extList.dp, el->extList.gc, el->extList.topShadow);
	else
	    XSetForeground(el->extList.dp, el->extList.gc, el->extList.bottomShadow);
	XFillPolygon(el->extList.dp,
		     el->extList.pixmap,
		     el->extList.gc,
		     bottomPoints,
		     6,
		     Nonconvex,
		     CoordModeOrigin);
    }
    if (upDown == XIEXTLIST_BUTTON_DOWN) {
	XSetForeground(el->extList.dp, el->extList.gc, el->extList.selectColor);
	XFillRectangle(el->extList.dp,
		       el->extList.pixmap,
		       el->extList.gc,
		       (int)smallX, (int)smallY,
		       (unsigned int)smallWidth, (unsigned int)smallHeight);
    }
    else {
	XSetForeground(el->extList.dp, el->extList.gc, el->extList.chBackground);
	XFillRectangle(el->extList.dp,
		       el->extList.pixmap,
		       el->extList.gc,
		       (int)smallX, (int)smallY,
		       (unsigned int)smallWidth, (unsigned int)smallHeight);
    }
    XSetForeground(el->extList.dp, el->extList.gc, el->extList.chForeground);
    XmStringDraw(el->extList.dp,
		 el->extList.pixmap,
		 el->extList.fontList,
		 el->extList.extItem[0].item[colNum],
		 el->extList.gc,
		 smallX + (Position)el->extList.colWidthMargin,
		 smallY + (Position)el->extList.chHeightMargin,
		 (Dimension)clipRec.width,
		 (el->extList.chLabelAlignment == 
		  XiEXTLIST_LABEL_ALIGNMENT_BEGINNING) ? XmALIGNMENT_BEGINNING:
		 (el->extList.chLabelAlignment == 
		  XiEXTLIST_LABEL_ALIGNMENT_CENTER) ? 
		 XmALIGNMENT_CENTER:XmALIGNMENT_END,
		 el->manager.string_direction,
		 &clipRec);
  
}


PRIVATE void RefreshButton(XiExtdListWidget el, int num)
{
    XCopyArea(el->extList.dp,
	      el->extList.pixmap,
	      el->core.window,
	      el->extList.gc,
	      (int)el->extList.colSeparator[num], 0,
	      MIN((unsigned int)el->extList.clipX + (unsigned int)el->extList.width,
		  (unsigned int)el->extList.colSeparator[num] + 
		  (unsigned int)el->extList.colWidth[num]),
	      MIN((unsigned int)el->extList.colTitleH,
		  (unsigned int)el->extList.height),
	      (unsigned int)el->extList.colSeparator[num] - 
	      (unsigned int)el->extList.clipX,
	      0);
}
	    

PRIVATE void RefreshAll(XiExtdListWidget el)
{
    if (XtIsRealized((Widget)el))
	XCopyArea(el->extList.dp,
		  el->extList.pixmap,
		  el->core.window,
		  el->extList.gc,
		  (int)el->extList.clipX, 0,
		  (unsigned int)el->extList.width,
		  (unsigned int)el->extList.height,
		  0, 0);
}
	    


PRIVATE void GetNewPixmap(XiExtdListWidget el)
{
    int lastCol = el->extList.colCount - 1;
    
    if (lastCol < 0)
	return;
    if (el->extList.pixmap)
	XFreePixmap(el->extList.dp, el->extList.pixmap);
    
    el->extList.pixmapWidth = MAX(el->extList.width, el->extList.totalColW);
    el->extList.pixmapHeight = MAX(el->extList.colTitleH + 
				   (el->extList.itemH * 
				    (el->extList.rowCount - 1)), el->extList.height);
    el->extList.colWidth[lastCol] = el->extList.pixmapWidth - 
	(Dimension)el->extList.colSeparator[lastCol];
    el->extList.clipX = MIN(el->extList.clipX, (Position)el->extList.pixmapWidth -
			    (Position)el->extList.width);
    el->extList.pixmap = XCreatePixmap(el->extList.dp, 
				       DefaultRootWindow(el->extList.dp), 
				       (unsigned int)el->extList.pixmapWidth,
				       (unsigned int)el->extList.pixmapHeight,
				       el->core.depth);
    XSetForeground(el->extList.dp, el->extList.gc, el->extList.chBackground);
    XFillRectangle(el->extList.dp,
		   el->extList.pixmap,
		   el->extList.gc,
		   0, 0,
		   (unsigned int)el->extList.pixmapWidth,
		   (unsigned int)el->extList.colTitleH);
    XSetForeground(el->extList.dp, el->extList.gc, el->extList.background);
    XFillRectangle(el->extList.dp,
		   el->extList.pixmap,
		   el->extList.gc,
		   0, (int)el->extList.colTitleH,
		   (unsigned int)el->extList.pixmapWidth,
		   (unsigned int)el->extList.pixmapHeight - 
		   (unsigned int)el->extList.colTitleH);
    XtVaSetValues(el->extList.vscroll,
		  XmNpageIncrement,      el->extList.visibleCount,
		  XmNsliderSize,         MIN(el->extList.visibleCount,
					     MAX(el->extList.rowCount - 1, 1)),
		  XmNvalue,              MAX(el->extList.topPosition, 1),
		  NULL);
    XtVaSetValues(el->extList.hscroll,
		  XmNmaximum,            (int)el->extList.pixmapWidth,
		  XmNsliderSize,         (int)(el->extList.width <= 0 ? 
					       1:el->extList.width),
		  XmNvalue,              (int)el->extList.clipX,
		  NULL);
    
}



PRIVATE void SortList(XiExtdListWidget el, int col)
{
    QuickSort(el, col, 1, el->extList.rowCount - 1);
    DrawList(el);
    RefreshAll(el);
}



PRIVATE void QuickSort(XiExtdListWidget el, int col, int first, int last)
{
    int mid;
    
    if (first < last) {
	mid = QuickSortPartition(el, col, first, last);
	QuickSort(el, col, first, mid);
	QuickSort(el, col, mid + 1, last);
    }
}




PRIVATE int  QuickSortPartition(XiExtdListWidget el, int col, int first, int last)
{
    XmString xms = XmStringCopy(el->extList.extItem[first].item[col]);
    int pivotIndex = first;
    ExtdListItemStruct tmp;
    
    while (1) {
	while(1) {
	    if (el->extList.func[col](el->extList.extItem[last].item[col],xms) > 0) {
		last--;
		continue;
	    }
	    else if (el->extList.selectedCol >= 0) {
		if (el->extList.func[col](el->extList.extItem[last].item[col], xms)
		    == 0) {
		    if (el->extList.func[el->extList.selectedCol]
			(el->extList.extItem[last].item[el->extList.selectedCol], 
			 el->extList.extItem[pivotIndex].item[el->extList.selectedCol]) > 0) {
			last--;
			continue;
		    }
		    break;
		}
		break;
	    }
	    break;
	}
	
	while (1) {
	    if (el->extList.func[col](el->extList.extItem[first].item[col],xms) < 0){
		first++;
		continue;
	    }
	    else if (el->extList.selectedCol >= 0) {
		if (el->extList.func[col](el->extList.extItem[first].item[col], xms)
		    == 0) {
		    if (el->extList.func[el->extList.selectedCol]
			(el->extList.extItem[first].item[el->extList.selectedCol], 
			 el->extList.extItem[pivotIndex].item[el->extList.selectedCol]) < 0) {
			first++;
			continue;
		    }
		    break;
		}
		break;
	    }
	    break;
	}
	if (first < last) {
	    if (first == pivotIndex)
		pivotIndex = last;
	    else if (last == pivotIndex)
		pivotIndex = first;
	    memcpy(&tmp, &el->extList.extItem[first], sizeof(ExtdListItemStruct));
	    memcpy(&el->extList.extItem[first], &el->extList.extItem[last], 
		   sizeof(ExtdListItemStruct));
	    memcpy(&el->extList.extItem[last], &tmp, sizeof(ExtdListItemStruct));
	    first++;
	    last--;
	}
	else {
	    XmStringFree(xms);
	    return last;
	}
    }
    XmStringFree(xms);
}



PRIVATE void    CallCallbacks(XiExtdListWidget el, XEvent *event, int reason)
{
    int i;
    int count = 0;
    static XiExtdListCallbackStruct cbs;
    
    for (i = 1; i < el->extList.rowCount; i++)
	if (el->extList.extItem[i].selected)
	    count++;
    
    if (el->extList.selectedPos)
	XtFree((char *)el->extList.selectedPos);
    if (el->extList.selectedItems)
	XtFree((char *)el->extList.selectedItems);
    
    /*el->extList.selectedItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * count);*/
    el->extList.selectedItems = (ExtdListItemStruct *)
	malloc(sizeof(ExtdListItemStruct) * count);
    /*el->extList.selectedPos = (int *)XtMalloc(sizeof(int) * count);*/
    el->extList.selectedPos = (int *)malloc(sizeof(int) * count);
    count = 0;
    for (i = 1; i < el->extList.rowCount; i++) {
	if (el->extList.extItem[i].selected) {
	    memcpy(&el->extList.selectedItems[count], &el->extList.extItem[i],
		   sizeof(ExtdListItemStruct));
	    el->extList.selectedPos[count] = i;
	    count++;
	}
    }
    cbs.reason = (reason == XiEXTLIST_BROWSE_SELECT) ? XiCR_BROWSE_SELECT:
	(reason == XiEXTLIST_SINGLE_SELECT) ? XiCR_SINGLE_SELECT:
	(reason == XiEXTLIST_MULTIPLE_SELECT) ? XiCR_MULTIPLE_SELECT:
	(reason == XiEXTLIST_EXTENDED_SELECT) ? XiCR_EXTENDED_SELECT:
	XiCR_DEFAULT_ACTION;
    cbs.event = event;
    memcpy(&cbs.item, &el->extList.extItem[el->extList.lastSelect],
	   sizeof(ExtdListItemStruct));
    cbs.position = el->extList.lastSelect;
    cbs.selectedItemCount = count;
    cbs.items = el->extList.selectedItems;
    cbs.selectedPos = el->extList.selectedPos;
    
    if (cbs.reason == XiCR_BROWSE_SELECT) 
	XtCallCallbackList((Widget)el, el->extList.browseSelectCB, &cbs);
    else if (cbs.reason == XiCR_SINGLE_SELECT)
	XtCallCallbackList((Widget)el, el->extList.singleSelectCB, &cbs);
    else if (cbs.reason == XiCR_MULTIPLE_SELECT)
	XtCallCallbackList((Widget)el, el->extList.multiSelectCB, &cbs);
    else if (cbs.reason == XiCR_EXTENDED_SELECT)
	XtCallCallbackList((Widget)el, el->extList.extSelectionCB, &cbs);
    else
	XtCallCallbackList((Widget)el, el->extList.defaultActionCB, &cbs);
}



PRIVATE void    UnselectAll(XiExtdListWidget el)
{
    int i;
    
    for (i = 1; i < el->extList.rowCount; i++)
	el->extList.extItem[i].selected = False;
}


PRIVATE void    RecalcGeometry(XiExtdListWidget el)
{
    Dimension wPad = el->extList.colWidthMargin * (Dimension)2,
	chhPad = el->extList.chHeightMargin * (Dimension)2,
	itemhPad = el->extList.itemHeightMargin * (Dimension)2,
	shadowPad = el->manager.shadow_thickness * (Dimension)2;
    Dimension maxH, maxW, w, h, sum;
    int i, j;
    
    if (el->extList.colWidth == NULL) {
	/*el->extList.colWidth = (Dimension *)XtMalloc(sizeof(Dimension) * (el->extList.colCount));*/
	el->extList.colWidth = (Dimension *)malloc(sizeof(Dimension) *
						   (el->extList.colCount));
	memset(el->extList.colWidth, 0, sizeof(Dimension) * (el->extList.colCount));
    }
    
    /* deternine column title height */
    maxH = (Dimension)0;
    for (i = 0; i < el->extList.colCount; i++) {
	h = XmStringHeight(el->extList.fontList, el->extList.extItem[0].item[i]);
	if (h > maxH)
	    maxH = h;
    }
    el->extList.colTitleH = maxH + chhPad + shadowPad;
    
    /* determine widths of each column and height of the items */
    maxH = (Dimension)0;
    for (j = 0; j < el->extList.colCount; j++) {
	maxW = (Dimension)0;
	for (i = 0; i < el->extList.rowCount; i++) {
	    w = XmStringWidth(el->extList.fontList, el->extList.extItem[i].item[j])
		+ wPad;
	    h = XmStringHeight(el->extList.fontList, el->extList.extItem[i].item[j]);
	    if (w > maxW)
		maxW = w;
	    if (h > maxH)
		maxH = h;
	}
	if (el->extList.colWidth[j] == (Dimension)0)
	    el->extList.colWidth[j] = maxW + (el->manager.shadow_thickness *
					      (Dimension)2);
    }
    
    el->extList.lastColW = el->extList.colWidth[el->extList.colCount - 1];
    el->extList.itemH = maxH + itemhPad;
    
    if (el->extList.colSeparator == NULL) {
	/*el->extList.colSeparator = (Position *)XtMalloc(sizeof(Position) * el->extList.colCount);*/
	el->extList.colSeparator = (Position *)malloc(sizeof(Position) * 
						      el->extList.colCount);
	memset(el->extList.colSeparator, 0, sizeof(Position) * el->extList.colCount);
	sum = (Dimension)0;
	for (i = 0; i < el->extList.colCount; i++) {
	    el->extList.colSeparator[i] = (Position)sum;
	    sum += el->extList.colWidth[i];
	}
    }
    if (el->core.width > 
	(Dimension)el->extList.colSeparator[el->extList.colCount - 1])
	el->extList.colWidth[el->extList.colCount - 1] = el->extList.width - 
	    (Dimension)el->extList.colSeparator[el->extList.colCount - 1];
    el->extList.totalColW = 
	(Dimension)el->extList.colSeparator[el->extList.colCount - 1] + 
	el->extList.colWidth[el->extList.colCount - 1];
    el->extList.minColWidth = (shadowPad + wPad) * (Dimension)2;
}


PRIVATE Boolean IsPositionVisible(XiExtdListWidget el, int pos) 
{
    if ((pos >= el->extList.topPosition) &&
	(pos < (el->extList.topPosition + el->extList.visibleCount)))
	return True;
    else
	return False;
}


/*********************************************************************************
 *  interface functions  - convinience functions
 *********************************************************************************/
PUBLIC void    XiExtdListAddItem(Widget w, 
				ExtdListItemStruct item, 
				int position)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
/*ExtdListItemStruct *newItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * (el->extList.rowCount + 1));*/
    ExtdListItemStruct *newItems = (ExtdListItemStruct *)
	malloc(sizeof(ExtdListItemStruct) * (el->extList.rowCount + 1));
    int i;
    
    memset(newItems, 0, sizeof(ExtdListItemStruct) * (el->extList.rowCount + 1));
    if (position == 0) {
	for (i = 0; i < el->extList.rowCount; i++)
	    memcpy(&newItems[i], &el->extList.extItem[i],sizeof(ExtdListItemStruct));
/*newItems[el->extList.rowCount].item = (XmString *)XtMalloc(sizeof(XmString) * el->extList.colCount);*/
	newItems[el->extList.rowCount].item = (XmString *)
	    malloc(sizeof(XmString) * el->extList.colCount);
	for (i = 0; i < el->extList.colCount; i++)
	    newItems[el->extList.rowCount].item[i] = XmStringCopy(item.item[i]);
	newItems[el->extList.rowCount].selected = item.selected;
	newItems[el->extList.rowCount].sensitive = item.sensitive;
	newItems[el->extList.rowCount].foreground = item.foreground;
	newItems[el->extList.rowCount].hook = item.hook;
    }
    else {
	for (i = 0; i < position; i++)
	    memcpy(&newItems[i], &el->extList.extItem[i], 
		   sizeof(ExtdListItemStruct));
	/*newItems[position].item = (XmString *)XtMalloc(sizeof(XmString) * el->extList.colCount);*/
	newItems[position].item = (XmString *)malloc(sizeof(XmString) * 
						     el->extList.colCount);
	for (i = 0; i < el->extList.colCount; i++)
	    newItems[position].item[i] = XmStringCopy(item.item[i]);
	newItems[position].selected = item.selected;
	newItems[position].sensitive = item.sensitive;
	newItems[position].foreground = item.foreground;
	newItems[position].hook = item.hook;
	for (i = position; i < el->extList.rowCount; i++)
	    memcpy(&newItems[i + 1], &el->extList.extItem[i], 
		   sizeof(ExtdListItemStruct));
    }
    el->extList.rowCount += 1;
    XtFree((char *)el->extList.extItem);
    el->extList.extItem = newItems;
    el->extList.items = (XtPointer)newItems;
    RecalcGeometry(el);
    if (el->extList.selectedCol >= 0)
	SortList(el, el->extList.selectedCol);
    for(i = 0; i < el->extList.colCount; i++)
	DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			  XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
    DrawList(el);
    RefreshAll(el);
    XtVaSetValues(el->extList.vscroll,
		  XmNmaximum,         el->extList.rowCount,
		  XmNsliderSize,      MIN(el->extList.visibleCount, 
					  MAX(el->extList.rowCount - 1, 1)),
		  NULL);
}



PUBLIC void    XiExtdListAddItems(Widget w, ExtdListItemStruct *item,  int count, 
				  int position)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
/*ExtdListItemStruct *newItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * (el->extList.rowCount + count));*/
    ExtdListItemStruct *newItems = 
	(ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct) * 
				     (el->extList.rowCount + count));
    int i, j;
    
    if (position == 0) {
	for (i = 0; i < el->extList.rowCount; i++)
	    memcpy(&newItems[i], &el->extList.extItem[i],sizeof(ExtdListItemStruct));
	for (i = 0; i < count; i++) {
    /*newItems[i + el->extList.rowCount].item = (XmString *)XtMalloc(sizeof(XmString) * el->extList.colCount);*/
	    newItems[i + el->extList.rowCount].item = 
		(XmString *)malloc(sizeof(XmString) * el->extList.colCount);
	    for (j = 0; j < el->extList.colCount; j++)
		newItems[i + el->extList.rowCount].item[j] = 
		    XmStringCopy(item[i].item[j]);
	    newItems[i + el->extList.rowCount].selected = item[i].selected;
	    newItems[i + el->extList.rowCount].sensitive = item[i].sensitive;
	    newItems[i + el->extList.rowCount].foreground = item[i].foreground;
	    newItems[i + el->extList.rowCount].hook = item[i].hook;
	}
    }
    else {
	for (i = 0; i < position; i++)
	    memcpy(&newItems[i], &el->extList.extItem[i],sizeof(ExtdListItemStruct));
	for (i = 0; i < count; i++) {
	    /*newItems[i + position].item = (XmString *)XtMalloc(sizeof(XmString) * el->extList.colCount);*/
	    newItems[i + position].item = (XmString *)malloc(sizeof(XmString) * 
							     el->extList.colCount);
	    for (j = 0; j < el->extList.colCount; j++)
		newItems[i + position].item[j] = XmStringCopy(item[i].item[j]);
	    newItems[i + position].selected = item[i].selected;
	    newItems[i + position].sensitive = item[i].sensitive;
	    newItems[i + position].foreground = item[i].foreground;
	    newItems[i + position].hook = item[i].hook;
	}
	for (i = position; i < el->extList.rowCount; i++)
	    memcpy(&newItems[i + position + count], &el->extList.extItem[i], 
		   sizeof(ExtdListItemStruct));
    }
    el->extList.rowCount += count;
    XtFree((char *)el->extList.extItem);
    el->extList.extItem = newItems;
    el->extList.items = (XtPointer)newItems;
    RecalcGeometry(el);
    if (el->extList.selectedCol >= 0)
	SortList(el, el->extList.selectedCol);
    for(i = 0; i < el->extList.colCount; i++)
	DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			  XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
    DrawList(el);
    RefreshAll(el);
    XtVaSetValues(el->extList.vscroll,
		  XmNmaximum,         el->extList.rowCount,
		  XmNsliderSize,      MIN(el->extList.visibleCount, 
					  MAX(el->extList.rowCount - 1, 1)),
		  NULL);
}



PUBLIC void    XiExtdListDeleteItem(Widget w, XtPointer item,  int col)   /* -1 is hook */
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
/*ExtdListItemStruct *newItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * (el->extList.rowCount - 1));*/
    ExtdListItemStruct *newItems = 
	(ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct) * 
				     (el->extList.rowCount - 1));
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos == 0) {
	XtFree((char *)newItems);
	return;
    }
    for (i = 0; i < el->extList.colCount; i++)
	XmStringFree(el->extList.extItem[pos].item[i]);
    XtFree((char *)el->extList.extItem[pos].item);
    for (i = 0; i < pos; i++)
	memcpy(&newItems[i], &el->extList.extItem[i], sizeof(ExtdListItemStruct));
    for (i = pos + 1; i < el->extList.rowCount; i++)
	memcpy(&newItems[i - 1], &el->extList.extItem[i],sizeof(ExtdListItemStruct));
    for (i = 0; i < el->extList.colCount; i++)
	XmStringFree(el->extList.extItem[pos].item[i]);
    
    el->extList.rowCount -= 1;
    XtFree((char *)el->extList.extItem);
    el->extList.extItem = newItems;
    el->extList.items = (XtPointer)newItems;
    if (el->extList.lastSelect == pos)
	el->extList.lastSelect = 0;
    else if (el->extList.lastSelect > pos)
	el->extList.lastSelect--;
    if (el->extList.rowCount <= 1)
	el->extList.topPosition = 1;
  RecalcGeometry(el);
  if (el->extList.selectedCol >= 0)
      SortList(el, el->extList.selectedCol);
  for(i = 0; i < el->extList.colCount; i++)
      DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
  DrawList(el);
  RefreshAll(el);
  XtVaSetValues(el->extList.vscroll,
		XmNmaximum,         el->extList.rowCount,
		XmNsliderSize,      MIN(el->extList.visibleCount,
					MAX(el->extList.rowCount - 1, 1)),
		NULL);
}



PUBLIC void    XiExtdListDeleteAllItems(Widget w)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    ExtdListItemStruct *newItems;
    int i, j;
    
    if (el->extList.rowCount <= 1)
	return;
    
    /*newItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * 1);*/
    newItems = (ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct) * 1);
    memcpy(newItems, el->extList.extItem, sizeof(ExtdListItemStruct));
    for (i = 1; i < el->extList.rowCount; i++) {
	for (j = 0; j < el->extList.colCount; j++)
	    XmStringFree(el->extList.extItem[i].item[j]);
	XtFree((char *)el->extList.extItem[i].item);
    }
    el->extList.rowCount = 1;
    XtFree((char *)el->extList.extItem);
    el->extList.extItem = newItems;
    el->extList.items = (XtPointer)newItems;
    el->extList.lastSelect = 0;
    el->extList.topPosition = 1;
    RecalcGeometry(el);
    for(i = 0; i < el->extList.colCount; i++)
	DrawColumnHeading(el, i, (i == el->extList.selectedCol) ? 
			  XIEXTLIST_BUTTON_DOWN:XIEXTLIST_BUTTON_UP);
    DrawList(el);
    RefreshAll(el);
    XtVaSetValues(el->extList.vscroll,
		  XmNmaximum,         el->extList.rowCount,
		  XmNsliderSize,      MIN(el->extList.visibleCount, 
					  MAX(el->extList.rowCount - 1, 1)),
		  NULL);
}



PUBLIC void    XiExtdListSelectItem(Widget w, XtPointer item,  int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos == 0) 
	return;
    el->extList.extItem[pos].selected = el->extList.extItem[pos].sensitive ? 
	True:False;
    DrawList(el);
    RefreshAll(el);
}



PUBLIC void    XiExtdListSelectAllItems(Widget w)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	el->extList.extItem[i].selected = el->extList.extItem[i].sensitive ? 
	    True:False;
    }
    DrawList(el);
    RefreshAll(el);
}



PUBLIC void    XiExtdListDeSelectItem(Widget w, XtPointer item,  int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
	break;
	    }
	}
    }
    if (pos == 0) 
	return;
    el->extList.extItem[pos].selected = False;
    DrawList(el);
    RefreshAll(el);
}



PUBLIC void    XiExtdListDeSelectAllItems(Widget w)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	el->extList.extItem[i].selected = False;
    }
    DrawList(el);
    RefreshAll(el);
}



PUBLIC Boolean XiExtdListIsItemSelected(Widget w, XtPointer item,  int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos == 0) 
	return False;
    if (el->extList.extItem[pos].selected)
	return True;
    else
	return False;
}



PUBLIC int     XiExtdListGetSelectedItems(Widget w, ExtdListItemStruct **item)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, count = 0;
    
    for (i = 1; i < el->extList.rowCount; i++)
	if (el->extList.extItem[i].selected)
	    count++;
    
    if (el->extList.selectedItems)
	XtFree((char *)el->extList.selectedItems);
    
    /*el->extList.selectedItems = (ExtdListItemStruct *)XtMalloc(sizeof(ExtdListItemStruct) * count);*/
    el->extList.selectedItems = 
	(ExtdListItemStruct *)malloc(sizeof(ExtdListItemStruct) * count);
    count = 0;
    for (i = 1; i < el->extList.rowCount; i++) {
	if (el->extList.extItem[i].selected) {
	    memcpy(&el->extList.selectedItems[count], &el->extList.extItem[i],
		   sizeof(ExtdListItemStruct));
	    count++;
    }
    }
    *item = el->extList.selectedItems;
    return count;
}



PUBLIC int     XiExtdListGetSelectedPos(Widget w,  int **pos)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, count = 0;
    
    for (i = 1; i < el->extList.rowCount; i++)
	if (el->extList.extItem[i].selected)
	    count++;
    
    if (el->extList.selectedPos)
	XtFree((char *)el->extList.selectedPos);
    /*el->extList.selectedPos = (int *)XtMalloc(sizeof(int) * count);*/
    el->extList.selectedPos = (int *)malloc(sizeof(int) * count);
    count = 0;
    for (i = 1; i < el->extList.rowCount; i++) {
	if (el->extList.extItem[i].selected) {
	    el->extList.selectedPos[count] = i;
	    count++;
	}
  }
    *pos = el->extList.selectedPos;
    return count;
}




PUBLIC Boolean XiExtdListItemExists(Widget w,  XtPointer item,  int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos == 0) 
	return False;
    else
    return True;
}



PUBLIC void    XiExtdListSetItem(Widget w, XtPointer item, int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos <= 0) 
	return;
    pos = MIN(pos + el->extList.visibleCount - 1, el->extList.rowCount - 1);
    pos = pos - el->extList.visibleCount;
    el->extList.topPosition = pos;
    XtVaSetValues(el->extList.vscroll,
		  XmNvalue,     MAX(el->extList.topPosition, 1),
		  NULL);
    DrawList(el);
    RefreshAll(el);
}


PUBLIC void    XiExtdListSetPos(Widget w, int pos)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    if (pos <= 0) 
	return;
    el->extList.topPosition = MIN(pos, el->extList.rowCount - 
				  el->extList.visibleCount);
    XtVaSetValues(el->extList.vscroll,
		  XmNvalue,     MAX(el->extList.topPosition, 1),
		  NULL);
    DrawList(el);
    RefreshAll(el);
}



PUBLIC void XiExtdListSetItemForeground(Widget w,  XtPointer item, int col, 
					char *color)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i, pos = 0;
    XColor xcolor;
    
    for (i = 1; i < el->extList.rowCount; i++) {
	if (col >= 0) {
	    if (XmStringCompare(el->extList.extItem[i].item[col], (XmString)item)) {
		pos = i;
		break;
	    }
	}
	else {
	    if (el->extList.extItem[i].hook == item) {
		pos = i;
		break;
	    }
	}
    }
    if (pos == 0) 
	return;
    if (!XParseColor(el->extList.dp, el->core.colormap, color, &xcolor)) {
	XtWarning("XiExtdList:XiExtdListsetItemForeground:Could not parse color\n");
	return;
  }
    else if (!XAllocColor(el->extList.dp, el->core.colormap, &xcolor)) {
	XtWarning("XiExtdList:XiExtdListsetItemForeground:Could not allocate requested color\n");
	return;
    }
    el->extList.extItem[pos].foreground = xcolor.pixel;
    DrawList(el);
    RefreshAll(el);
}



PUBLIC void    XiExtdListSetSortProc(Widget w, XiExtdListSortProc func,  int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    if (col < 0)
	return;
    if (col >= el->extList.colCount)
	return;
    el->extList.func[col] = func;
}



PUBLIC void    XiExtdListSortColumn(Widget w, int col)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    if (col < 0)
	return;
    if (col >= el->extList.colCount)
	return;
    SortList(el, col);
}


PUBLIC int     XiExtdListGetSortColumn(Widget w)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    return el->extList.selectedCol;
}



PUBLIC int XiExtdListGetItems(Widget w, ExtdListItemStruct **items)
{ 
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    *items = el->extList.extItem;
    return el->extList.rowCount;
}


PUBLIC int     XiExtdListFindItem(Widget w, XmString key, 
				 int col,        /* col = -1 for all columns */
				 int from, Boolean selectval)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    int i,j;
    int pos = 0;
    
    if (col >= el->extList.colCount) {
	XtWarning("XiExtdList:XiExtdListFindItem:Invalid column index\n");
	return 0;
    };
    if (el->extList.rowCount < 2)
	return 0;
    
    if (from < 1)
	from = 1;
    
    if (from >= el->extList.rowCount)
	from = 1;
    
    if (col < 0) {  /* look in all columns */
	Boolean found = False;
	for (i = from;; i++) {
	    for (j = 0; j < el->extList.colCount; j++)
		if (XmStringHasSubstring(el->extList.extItem[i].item[j], key)) {
		    pos = i;
		    found = True;
		    break;
		}
	    if (found)
		break;
	    if (i + 1 >= el->extList.rowCount) 
		i = 0;
	    if (i + 1 == from)
		break;
	}
  }
    else {  /* look only in specified column */
	for (i = from;; i++) {
	    if (XmStringHasSubstring(el->extList.extItem[i].item[col], key)) {
		pos = i;
		break;
	    }
	    if (i + 1 >= el->extList.rowCount) 
		i = 0;
	    if (i + 1 == from)
		break;
	}
    }
    if (!pos || !selectval) {
	XiExtdListDeSelectAllItems(w);
	return pos;
    }
  
    XiExtdListDeSelectAllItems(w);
    el->extList.extItem[pos].selected = el->extList.extItem[pos].sensitive ? 
	True:False;
    el->extList.lastSelect = pos;
    if (!IsPositionVisible(el, pos)) {
	el->extList.topPosition = MIN(pos, el->extList.rowCount - 
				      el->extList.visibleCount);
	XtVaSetValues(el->extList.vscroll,
		  XmNvalue,     MAX(el->extList.topPosition, 1),
		      NULL);
    }
    DrawList(el);
    RefreshAll(el);
  return pos;
}



PUBLIC void    XiExtdListSetSensitive(Widget w, int pos, Boolean state)
{
    XiExtdListWidget el = (XiExtdListWidget)w;
    
    if ((pos < 1) || (pos >= el->extList.rowCount)) {
	XtWarning("XiExtdList:XiExtdListSetSensitive:Invalid index\n");
	return;
    }
    el->extList.extItem[pos].sensitive = state;
    if (!state)
	el->extList.extItem[pos].selected = state;
    if (IsPositionVisible(el, pos)) {
	DrawList(el);
	RefreshAll(el);
    }
}
