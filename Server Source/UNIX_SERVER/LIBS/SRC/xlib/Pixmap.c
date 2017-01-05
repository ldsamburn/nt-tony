
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Pixmap.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>

XPointer COLOR(Widget, char*);
static void CachePixmap(int, Pixmap);
static Pixmap FetchPixmap(int);     
Pixmap XCreatePixmapFromData(Display*, Drawable, Colormap, unsigned int, 
                             unsigned int, int, unsigned int, unsigned int,
                             char**, char**);
Pixmap XPM_PIXMAP(Widget, int, unsigned int , unsigned int, unsigned int, 
                  unsigned int, char**, char**);
Pixmap XPixmapFromData(Widget, char*[], int);

XPointer COLOR(Widget w, char *name)
{
  XrmValue	fromVal, toVal;
  XPointer pixel;

  fromVal.size = sizeof(char*);
  fromVal.addr = name;

  XtConvert(w, XmRString, &fromVal, XmRPixel, &toVal);
  pixel = toVal.addr;
  if( pixel == NULL )
    {
      fromVal.addr = (char*)XtDefaultBackground;
      XtConvert(w, XmRString, &fromVal, XmRPixel, &toVal);
      pixel = toVal.addr;
    }
  return(pixel);
}


#define XPM_FORMAT 1

/* we keep the same codes as for Bitmap management */
#ifndef _XUTIL_H_
#ifdef VMS
#include "decw$include:Xutil.h"
#else
#include <X11/Xutil.h>
#endif
#endif

#define PixmapSuccess        BitmapSuccess 
#define PixmapOpenFailed     BitmapOpenFailed
#define PixmapFileInvalid    BitmapFileInvalid
#define PixmapNoMemory       BitmapNoMemory


typedef struct _Cixel {
     char c1,c2 ;
 } Cixel ;                    /* 2 chars for one pixel value */

typedef struct _CmapEntry {
     Cixel cixel ;               
     int red,green,blue ;
 } CmapEntry ;                    

typedef struct _PixmapEntry {
	int type;
	Pixmap pmap;
	int ref;
} PixmapEntry, *PixmapCache;

#define INITIAL_CACHE_SIZE	25
#define XPM_GENERIC		   	0

static PixmapCache xpm_cache;
static int cache_size;

#define MAXPRINTABLE 93             /* number of printable ascii chars 
				       minus \ and " for string compat. */

static const char * printable = " .XoO+@#$%&*=-;:?>,<1234567890qwertyuipasdfghjklzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|" ;
           /* printable begin with a space, so in most case, due to
	      my algorythm, when the number of different colors is
	      less than MAXPRINTABLE, it will give a char follow by 
	      "nothing" (a space) in the readable xpm file */

static void CachePixmap(int type, Pixmap pmap)
{
    PixmapEntry *pe = 0;
    
    if (!xpm_cache) {
	/*xpm_cache = (PixmapCache) XtCalloc((cache_size = INITIAL_CACHE_SIZE),
	  sizeof(PixmapEntry));*/
	xpm_cache = (PixmapCache) malloc((cache_size = INITIAL_CACHE_SIZE) *
					 sizeof(PixmapEntry));
	pe = &xpm_cache[0];
    } else {
	register int i;
	for (i = 0; i < cache_size; i++)
	    if (xpm_cache[i].pmap == 0) {
		pe = &xpm_cache[i];
		break;
	    }
	if (i == cache_size) {
	    XtWarning("Pixmap Cache Full");
	    return;
	}
    }
    
    pe->pmap = pmap;
    pe->ref = 1;
    pe->type = type;
}


static Pixmap FetchPixmap(int type)     
{
    register int i;
    
    for (i = 0; i < cache_size; i++)
	if (xpm_cache[i].type == type) {
	    xpm_cache[i].ref++;
	    return (xpm_cache[i].pmap);
	}
    
    return (0);
}

/**[ XCreatePixmapFromData ]**************************************************
 *
 *  This function allows you to include in your C program (using #include)
 *  a pixmap file in XPM format that was written by XWritePixmapFile
 * (THIS VERSION SUPPORTS : '_chars_per_pixel 1', '_chars_per_pixel 2')
 *****************************************************************************/
Pixmap XCreatePixmapFromData(Display *display, Drawable d, Colormap colormap, 
			     unsigned int width, unsigned int height, int depth, 
			     unsigned int ncolors, unsigned int chars_per_pixel, 
			     char **colors, char **pixels)
{
    Pixmap pixmap;
    GC Gc = NULL;
    XGCValues xgcv;
    XImage *xim = NULL;
  
    CmapEntry * cmap = NULL;
    int * Tpixel = NULL;
    /*
     * KLUDGE - This is a memory wasteful way to lookup this information.
     *          We could convert this to use a smaller index but it would
     *          be simpler to change the datafile format.
     */
    
    char	cmap_index[65536];		/* 65536 = range of 2 bytes */
    
    char c1, c2;
    XColor xcolor ;
    int i,j;
    
    /*
      #ifdef DEBUG
      #define DEBUG fprintf 
      #else
      #define DEBUG
      #endif
    */
#define DEBUG fprintf
    
#undef RETURN
#define RETURN(code, flag) \
  { if (cmap)   free (cmap); \
      if (Tpixel) free (Tpixel); \
      if (Gc)     XFreeGC(display, Gc); \
      if (xim)    XDestroyImage(xim); \
      if (flag && pixmap) XFreePixmap(display, pixmap); \
      return (code); }
    
    if (ncolors > (MAXPRINTABLE*MAXPRINTABLE)) {
	DEBUG (stderr,"Too many different colors, version 1");
	RETURN (PixmapFileInvalid, 0);
    }
    
    if ((chars_per_pixel < 1) || (chars_per_pixel > 2)) {
	DEBUG (stderr,"version 1.1 handles only 1 or 2 chars_per_pixel");
	RETURN (PixmapFileInvalid, 0);
    }
    
    /* now we construct cmap and Tpixel from colors array parameter */
    cmap = (CmapEntry *) malloc(ncolors*sizeof(CmapEntry)) ;
    Tpixel = (int *) malloc(ncolors*sizeof(int)) ;
    if ((cmap == NULL) || (Tpixel == NULL)) {
	RETURN (PixmapNoMemory, 0);
    }
    
    if (!colors) {
	DEBUG (stderr,"colors not defined");
	RETURN (PixmapFileInvalid, 0);
    }
    
    switch (chars_per_pixel){
    case 1:
	for (i=0; (unsigned int)i<2*ncolors ; i+=2){
	    if (sscanf(colors[i],"%c", &c1) != 1) {
		DEBUG (stderr,
		       "bad colormap entry : must be '\"c\" , \"colordef\",'"); 
		RETURN (PixmapFileInvalid, 0);
	    }
	    if(strchr(printable,c1)){
		cmap[i/2].cixel.c1 = c1 ;
		if (!XParseColor(display,colormap,colors[i+1],&xcolor)) {
		    DEBUG (stderr,
			   "bad colordef specification : #RGB or colorname");
		    RETURN (PixmapFileInvalid, 0);
		}
		XAllocColor(display,colormap,&xcolor);
		cmap_index[(int)c1] = i/2;
		Tpixel[i/2] = xcolor.pixel ;
	    } else {
		DEBUG (stderr,"bad cixel value : must be printable");
		RETURN (PixmapFileInvalid, 0);
	    }
	}
	break;
    case 2:
	for(i=0; (unsigned int)i<2*ncolors ; i+=2){
	    if (sscanf(colors[i],"%c%c", &c1,&c2) != 2) {
		DEBUG (stderr,
		       "bad colormap entry : must be '\"cC\" , \"colordef\",'"); 
		RETURN (PixmapFileInvalid, 0);
	    }
	    if ((strchr(printable,c1)) && (strchr(printable,c2))) {
		cmap[i/2].cixel.c1 = c1 ;
		cmap[i/2].cixel.c2 = c2 ;
		if (!XParseColor(display,colormap,colors[i+1],&xcolor)) {
		    DEBUG (stderr,"bad colordef specification : #RGB or colorname");
		    RETURN (PixmapFileInvalid, 0);
		}
		XAllocColor(display,colormap,&xcolor);
		cmap_index[c1 | (c2 << 8)] = i/2;
		Tpixel[i/2] = xcolor.pixel ;
	    } else {
		DEBUG (stderr,"bad cixel value : must be printable");
		RETURN (PixmapFileInvalid, 0);
	    }
	}
	break;
    }
    
    pixmap = XCreatePixmap(display,d,width,height,depth);
    Gc = XCreateGC(display,pixmap,0,&xgcv);
    xim = XGetImage(display,pixmap,0,0,width,height,AllPlanes,ZPixmap);
    
    if (!pixels) {
	DEBUG (stderr,"pixels not defined");
	RETURN (PixmapFileInvalid, 1);
    }
  
    j = 0 ;
    while ((unsigned int)j < height)
	{  
	    if (strlen(pixels[j]) != (chars_per_pixel*width)) {
		DEBUG (stderr,"bad pixmap line length %d",strlen(pixels[j]));
		RETURN (PixmapFileInvalid, 1);
	    }
	    switch (chars_per_pixel)
		{
		case 1:
		    for (i=0; (unsigned int)i< (width) ; i++)
	    {
		c1 = pixels[j][i] ;
#ifdef SLOW
		for (p = 0 ; p < ncolors ; p++)
		    if (cmap[p].cixel.c1 == c1)
			break;
		if (p == ncolors) {
		    DEBUG (stderr,"cixel \"%c\" not in previous colormap",c1);
		    RETURN (PixmapFileInvalid, 1);
		}
		XPutPixel(xim,i,j,Tpixel[p]);
#else
		XPutPixel(xim, i, j, Tpixel[(int)cmap_index[(int)c1]]);
#endif
	    }
		    break;
		case 2:
		    for (i=0; (unsigned int)i< (2*width) ; i+=2)
			{
			    c1 = pixels[j][i] ;
			    c2 = pixels[j][i+1] ;
#ifdef SLOW
			    for (p = 0 ; p < ncolors ; p++)
				if ((cmap[p].cixel.c1 == c1)&&(cmap[p].cixel.c2 ==
							       c2))
				    break;
			    if (p == ncolors) {
				DEBUG (stderr,"cixel \"%c%c\" not in previous colormap",c1,c2);
				RETURN (PixmapFileInvalid, 1);
			    }
			    XPutPixel(xim,i/2,j,Tpixel[p]);
#else
			    XPutPixel(xim, i/2,j,
				      Tpixel[(int)cmap_index[(int)c1|((int)c2<<8)]]);
#endif				
			}
		    break;
		}
	    j++ ;
	}
    
    XPutImage(display,pixmap,Gc,xim,0,0,0,0,width,height);
    
    RETURN (pixmap, 0);
}

/****<eof>********************************************************************/
Pixmap XPM_PIXMAP(Widget w, int type, unsigned int width, unsigned int height,
		  unsigned int ncolors, unsigned int colorptr, char **colors, 
		  char **pixels)  
{
    Colormap	colormap;
    int		depth;
    Arg		args[10];
    int		argcnt;
    Pixmap		pixmap;
    
    if (type != XPM_GENERIC && (pixmap = FetchPixmap(type)))
	return (pixmap);
    
    argcnt = 0;	
    XtSetArg(args[argcnt], XmNdepth, &depth); argcnt++;
    XtSetArg(args[argcnt], XmNcolormap, &colormap); argcnt++; 
    XtGetValues(w, args, argcnt);
    
    pixmap = XCreatePixmapFromData(XtDisplay(w), 
				   DefaultRootWindow(XtDisplay(w)),
				   colormap,
				   width,
				   height,
				   depth,
				   ncolors,
				   colorptr,
				   colors,
				   pixels);
    
    switch(pixmap)
    {
    case BitmapOpenFailed:
    case BitmapFileInvalid:
    case BitmapNoMemory: {
	char buf[100];
	sprintf(buf, "Can't create pixmap (%ld)", pixmap);
	XtWarning(buf);
	return(XmUNSPECIFIED_PIXMAP);
    }
    default:
	if (type != XPM_GENERIC)
	    CachePixmap(type, pixmap);
	return(pixmap);
    }
}


Pixmap XPixmapFromData(Widget w, char *pm_chars[], int cache_id) 
{
    int width, height, ncolors, colorptr;
    char *colors[256];
    char **ppcolors;
    char **pixels;
    int x,y;
    Pixmap pixmap;
    
    /* For HPUX hard code values as sscanf is not working correctly on HPUX 9.0 */
#ifdef HPUX
    width=height=32;
    ncolors=18;
    colorptr=1;
#else
    if (sscanf(pm_chars[0],"%d %d %d %d", &width, &height, &ncolors, &colorptr) != 4)
	return (Pixmap)NULL; 
#endif
    
    y=0;
    for (x=0; x<ncolors; x++) {
	colors[y]=(char*)malloc(14);
	colors[y+1]= (char*)malloc(14);
	colors[y][0] = pm_chars[1+x][0];
	colors[y][1] = '\0';
	strcpy(colors[y+1], &pm_chars[1+x][4]);
	y+=2;
    }
    
    pixels = &pm_chars[ncolors+1];
    ppcolors = colors;
    
    pixmap = XPM_PIXMAP(w, cache_id, width, height, ncolors, colorptr, colors, 
			pixels);
    
    for (x=0; x<ncolors*2; x++)
	free(colors[x]);
    
    return pixmap;
}

