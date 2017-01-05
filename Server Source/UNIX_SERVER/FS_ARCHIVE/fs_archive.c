
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * fs_archive.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <X11/Intrinsic.h>

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <search.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <MiscXFcts.h>
#include <Pixmap.h>
#include <Ntcss.h>
#include <NtcssHelp.h>
#include <ExtraProtos.h>

/** LOCAL INCLUDES **/

#include "fs_archive.h"
#include "problem_defs.h"
#include "xpm_backres.h"
#include "fs_archive_help.h"


/** FUNCTION PROTOTYPES **/

int main(int, char**);



/** STATIC VARIABLES **/


/** GLOBAL VARIABLES **/

Widget toplevel, restore_shell, backup_shell, fsb_dialog;
Widget changedev_dialog, copyjob_dialog, joblist_tb;
XtAppContext context;
XContext env;
AppResources resources;
Display *display;
char machine_type[16];
int filelist_size = 0;
int device_lock = -1;
XtIntervalId timer_id;
XtInputId input_id;


/** FUNCTION DEFINITIONS **/

int main(argc, argv)
int argc;
char **argv;
{
    Atom dw;
    int init_error = 0;
    static char display_env[256];
    char *progname, buf[128], iconbuf[128], err_msg[MAX_ITEM_LEN];
    FILE *fp;
    arcenv aenv;
    static XtResource app_resources[] =
    {
	{ XtNdataDir, XtCDataDir, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, data_dir), XmRImmediate, NULL },
	{ XtNprogDir, XtCProgDir, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, prog_dir), XmRImmediate, NULL },
	{ XtNhelpFile, XtCHelpFile, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, help_file), XmRImmediate, NULL },
	{ XtNdevFile, XtCDevFile, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, dev_file), XmRImmediate, NULL },
	{ XtNshowText, XtCShowText, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, show_text), XmRImmediate, NULL },
	{ XtNdbDir, XtCDbDir, XmRString, sizeof(String),
	  XtOffsetOf(AppResources, db_dir), XmRImmediate, NULL }
    };
    static String fallback[] =
    {
	"Fs_archive*highlightThickness:				0",
	"Fs_archive*foreground:						black",
	"Fs_archive*background:						grey65",
	"Fs_archive.geometry:						540x300",
	"Fs_archive*XmText.background:				LightSkyBlue",
	"Fs_archive*XmTextField.background:			LightSkyBlue",
	"Fs_archive*XmList.background:				LightSkyBlue",
	"Fs_archive*XmMenuShell.background:			grey70",
	"Fs_archive*XmPushButton.background:		grey70",
	"Fs_archive*XmPushButtonGadget.background:	grey70",
	"Fs_archive*XmToggleButton.selectColor:		LightSkyBlue",
	"Fs_archive*XmToggleButtonGadget.selectColor:	LightSkyBlue",
	"Fs_archive*main_menubar*background:		grey70",
#ifndef SOLARIS
	"Fs_archive*joblist_list*background:		LightSkyBlue",
	"Fs_archive*joblist_list.background:		grey65",
	"Fs_archive*joblist_list*title.background:	grey65",
	"Fs_archive*joblist_list*vertBar.background:	LightSkyBlue",
	"Fs_archive*joblist_list*horizBar.background:	LightSkyBlue",
	"Fs_archive*joblist_list*find.background: 	grey70",
	"Fs_archive*fsb_filelist_list*background:		LightSkyBlue",
	"Fs_archive*fsb_filelist_list.background:		grey65",
	"Fs_archive*fsb_filelist_list*title.background:	grey65",
	"Fs_archive*fsb_filelist_list*vertBar.background:	LightSkyBlue",
	"Fs_archive*fsb_filelist_list*horizBar.background:	LightSkyBlue",
	"Fs_archive*fsb_filelist_list*find.background: 	grey70",
	"Fs_archive*fsb_dirlist_list*background:		LightSkyBlue",
	"Fs_archive*fsb_dirlist_list.background:		grey65",
	"Fs_archive*fsb_dirlist_list*title.background:	grey65",
	"Fs_archive*fsb_dirlist_list*vertBar.background:	LightSkyBlue",
	"Fs_archive*fsb_dirlist_list*horizBar.background:	LightSkyBlue",
	"Fs_archive*fsb_dirlist_list*find.background: 	grey70",
#else
	"Fs_archive*XiExtdList.background:			LightSkyBlue",
	"Fs_archive*XiExtdList*foreground:			black",
	"Fs_archive*XiExtdList*showColumnSeparator:	True",
	"Fs_archive*XiExtdList*shadowThickness:		2",
	"Fs_archive*XiExtdList*columnHeadingBackground:		grey65",
	"Fs_archive*XiExtdList*columnHeadingLabelAlignment:		0",
#endif
	"Fs_archive*XmPushButton*fontList:			-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive*XmPushButtonGadget*fontList:	-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive*XmCascadeButton*fontList:		-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive*XmCascadeButtonGadget*fontList:	-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive*XmToggleButton*fontList:		-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive*XmToggleButtonGadget*fontList:		-misc-fixed-bold-*-*-*-*-120*",
	"Fs_archive.dataDir:	/h/NTCSSS/applications/data/fs_archive",
	"Fs_archive.progDir:	/h/NTCSSS/bin",
	"Fs_archive.helpFile:	/h/NTCSSS/applications/help/fs_archive.help",
	"Fs_archive.devFile:	/h/NTCSSS/applications/data/fs_archive/devices",
	"Fs_archive.showText:	/h/NTCSSS/bin/showtxt",
	"Fs_archive.dbDir:		/h/NTCSSS/database",
	NULL
    };
    Problem prob;
    Pixmap icon;
    char hostname[64];
    
    (void) signal(SIGCHLD, reaper);
    
    XtToolkitInitialize();
    context = XtCreateApplicationContext();
    XtAppSetFallbackResources(context, fallback);
    
    if ((progname = strrchr(*argv, '/')))
	progname++;
    else
	progname = *argv;
    
    if (!(display = XtOpenDisplay(context, NULL, progname, "Fs_archive",
				  (XrmOptionDescRec *) NULL, 0, &argc, argv))) {
	sprintf(buf, "Can't Open Display (%s)", XDisplayName(NULL));
	XtError(buf);
    }
    sprintf(display_env, "DISPLAY=%s", DisplayString(display));
    (void) putenv(display_env);
    
    if (gethostname(hostname, 64) == 0) {
	sprintf(buf, "NTCSS File System Backup/Restore - %s", hostname);
	sprintf(iconbuf, "NTCSS File Sys. Backup/Restore - %s", hostname);
    } else {
	sprintf(buf, "NTCSS File System Backup/Restore");
	sprintf(iconbuf, "NTCSS File Sys. Backup/Restore");
    }
    
    toplevel = XtVaAppCreateShell(progname, "Fs_archive",
				  applicationShellWidgetClass, display,
				  XmNallowShellResize, False,
				  XmNtitle, buf,
				  XmNiconName, iconbuf,
				  XmNdeleteResponse, XmDO_NOTHING,
				  NULL);
    
    icon = XPixmapFromData(toplevel, xpm_backres, 1);
    XtVaSetValues(toplevel, XmNiconPixmap, icon, NULL);
    
    dw = XmInternAtom(display, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(toplevel, dw, (XtCallbackProc)wm_close_callback, NULL);
    XtVaGetApplicationResources(toplevel, (XtPointer) &resources,
				app_resources,
				XtNumber(app_resources), NULL);
    num_job_rows = -1;
    /*
     * Check admininstration processes file.
     */
    if ((checkAdminProc(progname, (int) getpid(), resources.db_dir) <= 0) &&
	((argc != 2) || (strcmp(argv[1], "debug") != 0))) {
	sprintf(err_msg, "Program must be launched through NTCSS II Desktop");
	init_error = 1;
	goto create_main;
    }
    
    if (determine_machine_type() == 0) {
	sprintf(err_msg, "Cannot determine machine type.");
	init_error = 1;
	goto create_main;
    }

 create_main:
    create_main_widget(toplevel, &aenv);
    
    XtRealizeWidget(toplevel);
    
    env = XUniqueContext();
    SAVECONTEXT(toplevel, env, &aenv);
    
    if (!init_error) {
	/*
	 * Prepare archive directories.
	 */
	prob = prepare_archive_dirs(resources.data_dir, resources.dev_file);
	if (prob != NO_PROB) {
	    if (prob == CANT_DETERMINE_DEVICES_PROB)
		displayProblem(ERROR, prob,
			       problem_array, toplevel);
	    else
		displayProblem(ERROR, prob,
			       problem_array, toplevel,
			       "archive directories");
	} else {
	    /*
	     * Read in device list.
	     */
	    if ((fp = fopen(resources.dev_file, "r")) != NULL) {
		read_help(resources.db_dir,HELP_INI_FILE,APPLICATION_TITLE,
			  toplevel);
		openlog("FS_ARCH", LOG_PID, LOG_LOCAL7);
		syslog(LOG_WARNING, "Utility started.");
		set_default_help_info(_FileSystemBackup_RestoreOverview,
				      _Name_Backup);
		waitCursor(toplevel, True, False);
		XtAppAddInput(context, fileno(fp), (XtPointer)XtInputReadMask,
			      read_devicelist_in, fp);
	    } else {
		displayProblem(ERROR, CANT_DETERMINE_DEVICES_PROB,
			       problem_array, toplevel);
	    }
	}
    } else
	displayProblem(ERROR, INIT_APP_PROB,
		       problem_array, toplevel, err_msg);
    
    XtAppMainLoop(context);
    
    /*	free_help(); Commented out as there is a call in exit_archive */
    
    return(0);
}
