
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
--  NAME           : ButtonBox.c
--
--  DESCRIPTION    : The XiButtonBox widget, a simple composite that manages
--                   a group of XmPushButtons, enforcing a consistent layout
--                   policy.  Spacing is proportional to the number of buttons.
--                   Geometry management is basic, no fancy stuff.
--                   Ditto for resources.
--                   
--  AUTHOR(S)      : INRI-CH (dey)
--
--  CREATION DATE  : Mar 22, 1993
--
-------------------------------------------------------------------------------
*/

#include <stdarg.h>
#include <time.h> 

#include <X11/IntrinsicP.h>
#include <Xm/Label.h>
#include <Xm/PushBP.h>

#include "ButtonBoxP.h"

#define MAX(a, b)			((a) > (b) ? (a) : (b))
#define VALID_ID(bw, id)	((id) >= 0 && (unsigned int)(id) < (bw)->composite.num_children)
#define ONLY_PB_KIDS		"XiButtonBoxWidget Only Accepts XmPushButton Kids"
#define DEFAULT_SPACING		4

static void Initialize(XiButtonBoxWidget, XiButtonBoxWidget);
static void Resize(XiButtonBoxWidget);
static void InsertChild(Widget);
static void ChangeManaged(XiButtonBoxWidget);
static void Destroy(Widget);
static Boolean SetValues(XiButtonBoxWidget, XiButtonBoxWidget, XiButtonBoxWidget);
static XtGeometryResult GeometryHandler(Widget, XtWidgetGeometry*, XtWidgetGeometry*);
Widget XiButtonBoxGetButton(Widget , int );

static XtResource resources[] = {
  {
	  (char*)XiNactivateCallback,
	  (char*)XiCActivateCallback,
	  XmRCallback,
	  sizeof(XtCallbackList),
	  XtOffset(XiButtonBoxWidget, button_box.activate),
	  XmRCallback,
	  NULL
  },
  {
	  (char*)XiNuniformButtons,
	  (char*)XiCUniformButtons,
	  XmRBoolean,
	  sizeof(Boolean),
	  XtOffset(XiButtonBoxWidget, button_box.uniform),
	  XmRImmediate,
	  (caddr_t) True
  },
  {
	  (char*)XiNspacing,
	  (char*)XiCSpacing,
	  XmRDimension,
	  sizeof(Dimension),
	  XtOffset(XiButtonBoxWidget, button_box.spacing),
	  XmRImmediate,
	  (caddr_t) DEFAULT_SPACING
  }
};

XiButtonBoxClassRec xiButtonBoxClassRec = {
{
	/* CoreClassPart */
	
	(WidgetClass) &xmBulletinBoardClassRec,	/* superclass */
    	(char*)"XiButtonBox",			/* class_name */
   	sizeof(XiButtonBoxRec),			/* widget_size */
	NULL,					/* class_initialize */
	NULL,					/* class part initialize */
	FALSE,					/* class_inited */
	(XtInitProc)Initialize,			/* initialize */
	NULL,					/* initialize_hook */
	XtInheritRealize,			/* realize */
	NULL,					/* actions */
	0,					/* num_actions */
	resources,				/* resources */
	XtNumber(resources),			/* num_resources */
	NULLQUARK,				/* xrm_class */
	TRUE,					/* compress_motion */
	XtExposeCompressMaximal,		/* compress_exposure */
	TRUE,					/* compress_enterlv */
	TRUE,					/* visible_interest */
	Destroy,				/* destroy */
	(XtWidgetProc)Resize,			/* resize */
	XtInheritExpose,			/* expose */
	(XtSetValuesFunc)SetValues,		/* set_values */
	NULL,					/* set_values_hook */
    XtInheritSetValuesAlmost,			/* set_values_almost */
	NULL,					/* get_values_hook */
	NULL,					/* accept_focus */
	XtVersion,				/* version */
	NULL,					/* callback_private */
	NULL,					/* tm_table */
	XtInheritQueryGeometry,			/* query_geometry */
	NULL,					/* display_accelerator */
	NULL,					/* extension */
},
{
	/* CompositeClassPart */
#ifdef FOO	
	(XtGeometryHandler) _XtInherit,         /* geometry_manager   */
#endif
	GeometryHandler,			/* geometry_manager   */
	(XtWidgetProc)ChangeManaged,           	/* change_managed     */
	InsertChild,                   		/* insert_child       */
	XtInheritDeleteChild,                   /* delete_child       */
	NULL,                                   /* extension          */
},
{
	/* ConstraintClassPart */

	NULL,					/* constraint resources */
	0,					/* resource count */
	0,					/* constraint size */
	NULL,					/* initialize */
	NULL,					/* destroy */
	NULL,					/* set_values */
	NULL,					/* extension */
},
{
	/* XmManagerClassPart */

	XtInheritTranslations,                  /* translations */
	NULL,                            	/* syn_resources */
    0,                                      /* num_syn_resources */
    NULL,                                   /* syn_cont_resources */
    0,                                      /* num_syn_cont_resources */
    XmInheritParentProcess,                 /* parent_process */
    NULL,                                   /* extension */
},
{
	/* XmBulletinBoardClassPart */

	FALSE,				/* always_install_accelerators */
	NULL,				/* geo_matrix_create */
#ifdef VSUN
	XtInheritFocusMovedProc,	/* focus_moved_proc */
#endif
#ifdef SYSV
	XmInheritFocusMovedProc,	/* focus_moved_proc */
#endif
	NULL,				/* extension */
},
{
	/* XiButtonBoxClassPart */

	0,
}
};

WidgetClass xiButtonBoxWidgetClass = (WidgetClass) &xiButtonBoxClassRec;
static XmBulletinBoardWidgetClass sclass =
	(XmBulletinBoardWidgetClass) &xmBulletinBoardClassRec;

static void InternalActivate(Widget w, XtPointer client, XtPointer call)
{
    XiButtonBoxWidget bw = (XiButtonBoxWidget) client;
    
    if (bw->button_box.activate) {
	XmPushButtonCallbackStruct *cb = (XmPushButtonCallbackStruct*) call;
	XiButtonBoxCallbackStruct bcb;
	int i;
	
	bcb.reason = XmCR_ACTIVATE;
	bcb.event = cb->event;
	bcb.button = w;
	for (i = 0; (unsigned int)i < bw->composite.num_children; i++)
	    if (w == bw->composite.children[i])
		break;
	bcb.id = (unsigned int)i < bw->composite.num_children ? i : -1;
	bcb.callback = cb;
	XtCallCallbackList((Widget) bw, bw->button_box.activate, (XtPointer) &bcb);
    }
}
	
static void Initialize(XiButtonBoxWidget req, XiButtonBoxWidget new)
{
    /* done to get rid of compiler warning */
    req=req;
    
    new->button_box.kid_width = new->button_box.kid_height = 0;
}

static void DoLayout(XiButtonBoxWidget box)
{
    Dimension margin_w, margin_h, xoffset, yoffset;
    Dimension height, width, swidth;
    WidgetList wl;
    int i;
    
    if (box->composite.num_children == 0)
	return;
    
    margin_w = (2 * box->bulletin_board.margin_width) +
	(2 * box->manager.shadow_thickness);
    margin_h = (2 * box->bulletin_board.margin_height) +
	(2 * box->manager.shadow_thickness);
    width = box->core.width - margin_w;
    height = box->core.height - margin_h;
    
    swidth = width / box->composite.num_children;
    wl = box->composite.children;
    xoffset = box->bulletin_board.margin_width + box->manager.shadow_thickness;
    yoffset = box->bulletin_board.margin_height + box->manager.shadow_thickness;
    for (i = 0; (unsigned int)i < box->composite.num_children; i++)
	if (XtIsManaged(wl[i])) {
	    Dimension kwidth, kheight;
	    XmPushButtonWidget pb = (XmPushButtonWidget) wl[i];
	    
	    kwidth = pb->core.width + (2 * pb->core.border_width);
	    kheight = pb->core.height + (2 * pb->core.border_width);
	    XtMoveWidget(wl[i], ((swidth - kwidth) / 2) + (swidth * i) + xoffset,
			 ((height - kheight) / 2) + yoffset);
	}
}

static void ResizeKids(XiButtonBoxWidget box)
{
    int i;
    
    for (i = 0; (unsigned int)i < box->composite.num_children; i++) {
	XmPushButtonWidget pb = (XmPushButtonWidget) box->composite.children[i];
	if (pb->core.width != box->button_box.kid_width ||
	    pb->core.height != box->button_box.kid_height)
	    XtResizeWidget((Widget) pb, box->button_box.kid_width,
			   box->button_box.kid_height, pb->core.border_width);
    }
}

/*
 * GeometryHandler:
 *
 * Simple geometry manager, allowing kids to request width or height changes
 * only.
 */

static XtGeometryResult GeometryHandler(Widget w, XtWidgetGeometry *req,
					XtWidgetGeometry *reply)
{
    XiButtonBoxWidget bw = (XiButtonBoxWidget) ((XmPushButtonWidget) w)->core.parent;
    Dimension new_width = bw->core.width,
	new_height = bw->core.height;
    
    /* done to get rid of compiler warning */
    reply=reply;
    
    if (req->request_mode & ~(CWWidth | CWHeight))
	return (XtGeometryNo);
    
    if (req->request_mode & CWWidth)
	if (req->width > bw->button_box.kid_width)
	    new_width = bw->core.width +
		((req->width - bw->button_box.kid_width) * 
		 bw->composite.num_children);
    
    if (req->request_mode & CWHeight)
	if (req->height > bw->button_box.kid_height)
	    new_height = bw->core.height + 
		(req->height - bw->button_box.kid_height);
    
    if (new_width != bw->core.width || new_height != bw->core.height)
	switch (XtMakeResizeRequest((Widget) bw, new_width,new_height, NULL, NULL)) {
	case XtGeometryYes:
	    if (req->request_mode & CWWidth)
		bw->button_box.kid_width = req->width;
	    if (req->request_mode & CWHeight)
		bw->button_box.kid_height = req->height;
	    if (bw->button_box.uniform)
		ResizeKids(bw);
	    DoLayout(bw);
	    return (XtGeometryYes);
	    break;
	default:
	    return (XtGeometryNo);
	}
    
    return (XtGeometryNo);
    
    /* return ((*sclass->composite_class.geometry_manager)(w, req, reply)); */
}
	
static void Resize(XiButtonBoxWidget box)
{
    DoLayout(box);
    (*sclass->core_class.resize)((Widget) box);
}

static void ChangeManaged(XiButtonBoxWidget box)
{
    if (box->composite.num_children != 0) {
	WidgetList wl = box->composite.children;
	int nkids = box->composite.num_children;
	int i;
	Dimension width = 0, height = 0;
	for (i = 0; i < nkids; i++) {
	    XmPushButtonWidget pb = (XmPushButtonWidget) wl[i];
	    Dimension st = 2 * pb->primitive.shadow_thickness;
	    
	    if (box->button_box.uniform)
		if (pb->core.width != box->button_box.kid_width ||
		    pb->core.height != box->button_box.kid_height)
		    XtResizeWidget((Widget)pb, box->button_box.kid_width,
				   box->button_box.kid_height,
				   pb->core.border_width);
	    width += (pb->core.width + box->button_box.spacing + st);
	    height = MAX((wl[i])->core.height + st, height);
	}
	box->core.width = width + (2 * box->bulletin_board.margin_width);
	box->core.height = height + (2 * box->bulletin_board.margin_height);
	DoLayout(box);
    }
    /* (*sclass->composite_class.change_managed)((Widget) box); */
}

static void InsertChild(Widget w)
{
    if (!XmIsPushButton(w))
	XtWarning(ONLY_PB_KIDS);
    else {
	XmPushButtonWidget pb = (XmPushButtonWidget) w;
	XiButtonBoxWidget bw = (XiButtonBoxWidget) pb->core.parent;
	bw->button_box.kid_width = MAX(pb->core.width, bw->button_box.kid_width);
	bw->button_box.kid_height = MAX(pb->core.height, bw->button_box.kid_height);
	XtAddCallback((Widget)pb, XmNactivateCallback, InternalActivate,
		      (XtPointer) bw);
	(*sclass->composite_class.insert_child)(w);
    }
}

static void Destroy(Widget box)
{
    /* done to get rid of compiler warning */
    box=box;
    
}

static Boolean SetValues(XiButtonBoxWidget cur, XiButtonBoxWidget req,
			 XiButtonBoxWidget new)
{
    /* done to get rid of compiler warning */
    cur=cur;
    req=req;
    new=new;
    
    return (False);
}

/* ----------------- External Hooks to XiButtonBoxWidget ------------- */
Widget XiCreateButtonBox(Widget parent, char* name, XtCallbackProc callback, 
                         XtPointer client, ...)
{
    Widget box;
    va_list ap;
    char *label;
    int i = 0;
    
    box = XtVaCreateManagedWidget(name, xiButtonBoxWidgetClass,
				  parent, NULL);
    if (callback)
	XtAddCallback(box, XiNactivateCallback, callback, client);
    
    va_start(ap, client);
    while ((label = va_arg(ap, char *))) {
	XmString s = XmStringCreateSimple(label);
	char buf[64];
	sprintf(buf, "button_%d", i++);
	(void) XtVaCreateManagedWidget(buf, xmPushButtonWidgetClass,
				       box, XmNlabelString, s, NULL);
	XmStringFree(s);
    }
    va_end(ap);
    
    return (box);
}

Widget XiButtonBoxGetButton(Widget box, int id)
{
    XiButtonBoxWidget bw = (XiButtonBoxWidget) box;
    
    if (!XiIsButtonBox(box)) {
	XtWarning("XiButtonBoxGetButton() Needs An XiButtonBox Widget");
	return (NULL);
    }
    if (VALID_ID(bw, id))
	return (bw->composite.children[id]);
    
    return (NULL);
}
