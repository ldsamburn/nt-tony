
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
--  NAME           : ButtonBoxP.h
--
--  DESCRIPTION    : Private stuff for XiButtonBox widget.
--                   
--  AUTHOR(S)      : INRI-CH (dey)
--
--  CREATION DATE  : Mar 22, 1993
--
-------------------------------------------------------------------------------
*/

#ifndef _XiButtonBoxP_h
#define _XiButtonBoxP_h

#include <Xm/BulletinBP.h>
#include <ButtonBox.h>

typedef struct {
	int mumble;
} XiButtonBoxClassPart;

typedef struct _XiButtonBoxClassRec {
    CoreClassPart            core_class;
    CompositeClassPart       composite_class;
    ConstraintClassPart      constraint_class;
    XmManagerClassPart       manager_class;
    XmBulletinBoardClassPart bulletin_board_class;
    XiButtonBoxClassPart     button_box_class;
} XiButtonBoxClassRec;

extern XiButtonBoxClassRec xiButtonBoxClassRec;

typedef struct {
	XtCallbackList activate;
	Dimension kid_width, kid_height;
	Dimension spacing;
	Boolean uniform;
} XiButtonBoxPart;

typedef struct _XiButtonBoxRec {
	CorePart			core;
	CompositePart		composite;
	ConstraintPart      constraint;
	XmManagerPart		manager;
	XmBulletinBoardPart	bulletin_board;
	XiButtonBoxPart		button_box;
} XiButtonBoxRec;

#endif /* _XiButtonBoxP_h */

/* DON'T ADD STUFF AFTER THIS #endif */
	
