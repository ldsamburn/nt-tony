
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 

/**
  * ExtListP.h
  *
  * Private header for XiExtdList widget
  * 
  * HISTORY 
  * -------
  * MBS:  1/17/97: Created
  * MBS:  3/07/97: Added lastColW field to the private stuff
  *
  */

#ifndef _ExtListP_h
#define _ExtListP_h
 

#include <Xm/ManagerP.h>
#include "ExtList.h"


/* class structures */
typedef struct _XiExtdListClassPart {
  int ignore;
}XiExtdListClassPart;

typedef struct _XiExtdListClassRec {
  CoreClassPart          core_class;
  CompositeClassPart     composite_class;
  ConstraintClassPart    constraint_class;
  XmManagerClassPart     manager_class;
  XiExtdListClassPart     extList_class;
}XiExtdListClassRec;

extern XiExtdListClassRec xiExtdListClassRec;
  
/* instance structures */
typedef struct _XiExtdListPart {
  /* public stuff */
  XtCallbackList    browseSelectCB;
  XtCallbackList    defaultActionCB;
  XtCallbackList    extSelectionCB;
  XtCallbackList    multiSelectCB;
  XtCallbackList    singleSelectCB;
  XmFontList        fontList;
  int               colCount;
  int               rowCount;
  int               itemType;
  XtPointer         items;
  int               visibleCount;
  Pixel             background;
  Pixel             foreground;
  Pixel             chBackground;
  Pixel             chForeground;
  Boolean           showColSeparator;
  Dimension         chHeightMargin;
  Dimension         itemHeightMargin;
  Dimension         colWidthMargin;
  int               chLabelAlignment;
  int               itemLabelAlignment;
  int               selectionType;
  Boolean           allowSort;
  
  /* private stuff */  
  Display           *dp;
  Dimension         width;
  Dimension         height;
  ExtdListItemStruct *extItem;
  Dimension         *colWidth;
  Position          *colSeparator;
  XiExtdListSortProc *func;
  Widget            vscroll;
  Widget            hscroll;
  GC                gc;
  GC                stippledGC;
  GC                resizeColGC;
  Cursor            cursor;
  Pixel             topShadow;
  Pixel             bottomShadow;
  Pixel             selectColor;
  Pixmap            pixmap;
  Dimension         pixmapWidth;
  Dimension         pixmapHeight;
  int               topPosition;
  Position          clipX;
  Dimension         colTitleH;
  Dimension         itemH;
  Dimension         totalColW;
  Dimension         lastColW;
  int               selectedCol;

  int               btnDownType;

  int               resizePos;
  int               resizeIndex;
  Dimension         minColWidth;

  int               lastSelect;
  int               selectStart;

  int               *selectedPos;
  ExtdListItemStruct *selectedItems;
  int               multiClickTime;
}XiExtdListPart;

typedef struct _XiExtdListRec {
  CorePart           core;
  CompositePart      composite;
  ConstraintPart     constraint;
  XmManagerPart      manager;
  XiExtdListPart      extList;
}XiExtdListRec;

#endif  /* _ExtListP.h */
