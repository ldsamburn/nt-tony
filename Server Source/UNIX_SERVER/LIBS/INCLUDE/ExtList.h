
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
  * ExtList.h
  *
  * Public header for XiExtdList widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/17/97: Created
  * MBS:  3/06/97: Added XiExtdListGetSortColumn()
  *
  */

#ifndef _ExtList_h
#define _ExtList_h

#ifdef __cplusplus
extern "C" {
#endif
  
#define PRIVATE static
#define PUBLIC
  
#define XiEXTLIST_LABEL_ALIGNMENT_BEGINNING 0  
#define XiEXTLIST_LABEL_ALIGNMENT_CENTER    1   
#define XiEXTLIST_LABEL_ALIGNMENT_END       2    

#define XiEXTLIST_BROWSE_SELECT             3
#define XiEXTLIST_EXTENDED_SELECT           4
#define XiEXTLIST_MULTIPLE_SELECT           5
#define XiEXTLIST_SINGLE_SELECT             6
  
#define XiCR_BROWSE_SELECT                  7
#define XiCR_EXTENDED_SELECT                8
#define XiCR_MULTIPLE_SELECT                9
#define XiCR_SINGLE_SELECT                  10
#define XiCR_DEFAULT_ACTION                 11

#define XiEXTLIST_ITEM_TYPE_STRUCTURED      12
#define XiEXTLIST_ITEM_TYPE_NON_STRUCTURED  13
 


  
#define XiNbrowseSelectCallback "browseSelectCallback"
#define XiNdefaultActionCallback "defaultActionCallback"
#define XiNextendedSelectCallback "extendedSelectCallback"
#define XiNmultipleSelectCallback "multipleSelectCallback"
#define XiNsingleSelectCallback "singleSelectCallback"
#define XiNfontList "fontList"
#define XiNcolumnCount "columnCount"
#define XiNrowCount "rowCount"
#define XiNitemType "itemType"
#define XiNitems "items"
#define XiNvisibleCount "visibleCount"
#define XiNbackground "background"
#define XiNforeground "foreground"
#define XiNcolumnHeadingBackground "columnHeadingBackground"
#define XiNcolumnHeadingForeground "columnHeadingForeground"
#define XiNshowColumnSeparator "showColumnSeparator"
#define XiNcolumnHeadingMarginHeight "columnHeadingMarginHeight"
#define XiNitemMarginHeight "itemMarginHeight"
#define XiNcolumnMarginWidth "columnMarginWidth"
#define XiNcolumnHeadingLabelAlignment "columnHeadingLabelAlignment"
#define XiNextListItemLabelAlignment "extListItemLabelAlignment"
#define XiNselectionType "selectionType"
#define XiNallowSort "allowSort"

#define XiCBrowseSelectCallback "BrowseSelectCallback"
#define XiCDefaultActionCallback "DefaultActionCallback"
#define XiCExtendedSelectCallback "ExtendedSelectCallback"
#define XiCMultipleSelectCallback "MultipleSelectCallback"
#define XiCSingleSelectCallback "SingleSelectCallback"
#define XiCFontList "FontList"
#define XiCColumnCount "ColumnCount"
#define XiCRowCount "RowCount"
#define XiCItemType "ItemType"
#define XiCItems "Items"
#define XiCVisibleCount "VisibleCount"
#define XiCBackground "Background"
#define XiCForeground "Foreground"
#define XiCColumnHeadingBackground "ColumnHeadingBackground"
#define XiCColumnHeadingForeground "ColumnHeadingForeground"
#define XiCShowColumnSeparator "ShowColumnSeparator"
#define XiCColumnHeadingMarginHeight "ColumnHeadingMarginHeight"
#define XiCItemMarginHeight "ItemMarginHeight"
#define XiCColumnMarginWidth "ColumnMarginWidth"
#define XiCColumnHeadingLabelAlignment "ColumnHeadingLabelAlignment"
#define XiCExtdListItemLabelAlignment "ExtdListItemLabelAlignment"
#define XiCSelectionType "SelectionType"
#define XiCAllowSort "AllowSort"

extern WidgetClass xiExtdListWidgetClass;

typedef struct _XiExtdListClassRec *XiExtdListWidgetClass;
typedef struct _XiExtdListRec      *XiExtdListWidget;

typedef struct _ExtdListItemStruct {
  XmString  *item;
  Boolean   sensitive;
  Boolean   selected;
  Pixel     foreground;
  XtPointer hook;
}ExtdListItemStruct;

typedef struct _XiExtdListCallbackStruct {
  int               reason;
  XEvent            *event;
  ExtdListItemStruct item;
  int               position;
  int               selectedItemCount;
  ExtdListItemStruct *items;
  int               *selectedPos;
}XiExtdListCallbackStruct;

#ifdef _NO_PROTO
typedef int (*XiExtdListSortProc)();
#else
typedef int (*XiExtdListSortProc)(XmString, XmString);
#endif

/* convenience functions and macros */
#define XiIsExtdList(w)     (XtIsSubclass((w), xiExtdListWidgetClass))

#ifdef _NO_PROTO
PUBLIC void    XiExtdListAddItem();
PUBLIC void    XiExtdListAddItems();
PUBLIC void    XiExtdListDeleteItem();  /* -1 is hook */
PUBLIC void    XiExtdListDeleteAllItems();
PUBLIC void    XiExtdListSelectItem();
PUBLIC void    XiExtdListSelectAllItems();
PUBLIC void    XiExtdListDeSelectItem();
PUBLIC void    XiExtdListDeSelectAllItems();
PUBLIC Boolean XiExtdListIsItemSelected();
PUBLIC int     XiExtdListGetSelectedItems();
PUBLIC int     XiExtdListGetSelectedPos();
PUBLIC Boolean XiExtdListItemExists();
PUBLIC void    XiExtdListSetItem();
PUBLIC void    XiExtdListSetPos();
PUBLIC void    XiExtdListSetItemForeground();
PUBLIC void    XiExtdListSetSortProc();
PUBLIC void    XiExtdListSortColumn();
PUBLIC int     XiExtdListGetSortColumn();
PUBLIC int     XiExtdListGetItems();
PUBLIC int     XiExtdListFindItem();
PUBLIC void    XiExtdListSetSensitive();


#else
PUBLIC void    XiExtdListAddItem(Widget w, ExtdListItemStruct item, int position);
PUBLIC void    XiExtdListAddItems(Widget w, ExtdListItemStruct *item, int count, int position);
PUBLIC void    XiExtdListDeleteItem(Widget w, XtPointer item, int col);  /* -1 is hook */
PUBLIC void    XiExtdListDeleteAllItems(Widget w);
PUBLIC void    XiExtdListSelectItem(Widget w, XtPointer item, int col);
PUBLIC void    XiExtdListSelectAllItems(Widget w);
PUBLIC void    XiExtdListDeSelectItem(Widget w, XtPointer item, int col);
PUBLIC void    XiExtdListDeSelectAllItems(Widget w);
PUBLIC Boolean XiExtdListIsItemSelected(Widget w, XtPointer item, int col);
PUBLIC int     XiExtdListGetSelectedItems(Widget w, ExtdListItemStruct **item);
PUBLIC int     XiExtdListGetSelectedPos(Widget w, int **pos);
PUBLIC Boolean XiExtdListItemExists(Widget w, XtPointer item, int col);
PUBLIC void    XiExtdListSetItem(Widget w, XtPointer item, int col);
PUBLIC void    XiExtdListSetPos(Widget w, int pos);
PUBLIC void    XiExtdListSetItemForeground(Widget w, XtPointer item, int col, char *color);
PUBLIC void    XiExtdListSetSortProc(Widget w, XiExtdListSortProc func, int col);
PUBLIC void    XiExtdListSortColumn(Widget w, int col);
PUBLIC int     XiExtdListGetSortColumn(Widget w);
PUBLIC int     XiExtdListGetItems(Widget w, ExtdListItemStruct **items);
PUBLIC int     XiExtdListFindItem(Widget w, XmString xms, int col, int from, Boolean sel);  /* col = -1 for all columns */
PUBLIC void    XiExtdListSetSensitive(Widget w, int pos, Boolean state);


#endif

#ifdef __cplusplus
}
#endif

#endif
