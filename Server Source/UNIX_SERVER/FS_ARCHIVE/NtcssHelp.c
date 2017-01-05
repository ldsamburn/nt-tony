
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * NtcssHelp.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <X11/Core.h>
#include <X11/keysym.h>

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

/** INRI INCLUDES **/
#include <Ntcss.h>
#include <ini_Fcts.h>
#include <Menus.h>
#include <MiscXFcts.h>
#include <SockFcts.h>
#include <ExtList.h>
#include <ComboBox.h>

/** LOCAL INCLUDES **/

#include "NtcssHelp.h"

/* GENERAL DEFINES */

#define MAXVAL(x,y) (x > y) ? x : y
#define HELP_TOPIC_LINE_SIZE      100
/*#define ICS_VECTOR_LINE_SIZE      100 */
#define MAX_HOST_NAME_SIZE        100 
#define MAX_LABEL_SIZE            20
#define MAX_MESSAGE_SIZE          100
#define MAX_HELP_PATH_SIZE        20
#define MAX_FILE_SPEC_SIZE        100
#define SECTION_BUFFER_SIZE       (10 * 40) + 1
#define VALUE_BUFFER_SIZE         MAXVAL((20 * 3),(MAX_HELP_TOPICS * 30))
#define CONTENTS_HELP_VECTOR      0
#define VERSION_HELP_VECTOR       1
#define MAX_HELP_STACK_ITEMS      20
#define APPLICATION_TITLE_SIZE    60
#define VERSION_TITLE_SIZE        40
/* #define NTCSS_TOP_LEVEL_DIRECTORY  "$NTCSSDIR" */

/* HELP MENU LABELS */

#define HELP_TOPICS_HELP   0
#define GETTING_HELP       1
#define WINDOW_HELP        2
#define FIELD_HELP         3
#define BOOKS              4
#define USER_GUIDE         5
#define DATA_ELEMENTS      6 
#define DATABASE_SPEC      7 
#define INTERFACE_DESIGN   8
#define SYSTEM_ADMIN       9
#define SECURITY_PLAN     10
#define SECURITY_CONCEPT  11
#define SITE_SECURITY     12
#define TECHNICAL_SUPPORT 13
#define VERSION_LABEL     14

/* STATIC DATA */

static unsigned int help_info_stack[MAX_HELP_STACK_ITEMS];
static int top_help_info_stack = -1;
static int master_help_switch = 0;
static unsigned int help_info = 0;
/*static unsigned int menu_help_info = 0;*/
static unsigned int runtime_window_vector = 0;
static unsigned int runtime_field_vector1 = 0;
static unsigned int runtime_field_vector2 = 0;
/*static unsigned int runtime_ics_number = 0;*/
/*static Widget ics_widget;*/
/*static int ics_help_vectors[MAX_ICS_WIDGETS][MAX_COLUMNS_IN_ICS_WIDGET];*/
static char *help_topics[MAX_HELP_TOPICS];
static int last_topic_allocated = -1;
static int allow_setting_help_on_focus_in = False;
static char application_title[APPLICATION_TITLE_SIZE]; 
static char version_title[VERSION_TITLE_SIZE];
static Widget toplevel_shell;

/* FUNCTION PROTOTYPES */

void  send_for_help(const char*, char*);
void  set_help_info(Widget, XtPointer, XEvent*);
void  set_default_help_info(unsigned int, unsigned int);
void  process_help_request(Widget, char, XmAnyCallbackStruct*);
void  read_help(char*, char*, char*, Widget);
void  free_help(void);
void  set_runtime_window_help_vector(unsigned int);
void  set_runtime_field_help_vector1(unsigned int);
void  set_runtime_field_help_vector2(unsigned int);
/*void  set_runtime_ics_number(unsigned int);*/
/*void  set_runtime_ics_info(Widget, unsigned int);*/
void  set_focus(Widget, XtPointer, XEvent*);
void  set_focus_on_popup(Widget, Widget, XmAnyCallbackStruct*);
void  help_menu_callback(Widget, int);
Widget  build_help_menu(Widget);
void  save_help_info(void);
void  restore_help_info(void);
static void debug_help_info(const char*);
static void get_help_topics(unsigned int, char**, char**, char**, char**);
static void set_menu_item(MenuItem*, int, int, char*, char, int);
static int read_help_ini_file(char*);

/****************************************************************************************/
void save_help_info()
{
    /* Push help_info */
    
    if ((top_help_info_stack + 1) < MAX_HELP_STACK_ITEMS) {
	debug_help_info("SAVE");
	help_info_stack[++top_help_info_stack] = help_info;
    } else {
	syslog(LOG_WARNING,"NTCSS help_info_stack overflow");
    }
}    

/****************************************************************************************/
void restore_help_info()
{

    /* Pop help_info */
    
    if (top_help_info_stack >= 0) {
	help_info = help_info_stack[top_help_info_stack--];
	debug_help_info("RESTORE");
    } else {
	syslog(LOG_WARNING,"NTCSS help_info_stack underflow");
    }
}

/*******************************************************************************************/
void send_for_help(const char *help_file, char *help_topic)
{
    char *host_ptr;
    char hostname[MAX_HOST_NAME_SIZE];
    char label[MAX_LABEL_SIZE];
    char message[MAX_MESSAGE_SIZE];
    
    if (!master_help_switch)
	return;
    
    if (!*help_file)
	return;
    
    host_ptr = getenv("DISPLAY");
    
    memcpy(hostname, host_ptr, MAX_HOST_NAME_SIZE);
    
    /* truncate hostname:0.0 to hostname */
    if ((host_ptr = strchr(hostname,':')))
	*host_ptr = 0;
    
    memset(label,0,MAX_LABEL_SIZE);
    strcpy(label,"EXECUTE_WS_PROGRAM");
    
    if (*help_topic)
	sprintf(message,"winhelp -i%s %s/%s",help_topic,
		NTCSS_TOP_LEVEL_DIRECTORY,help_file);
    else
	sprintf(message,"winhelp %s/%s",NTCSS_TOP_LEVEL_DIRECTORY,help_file);

#ifdef DEBUG
    syslog(LOG_WARNING,"hostname:%s message:%s label:%s",hostname,message,label);
#endif

    send_msg(hostname,message,label,MAX_LABEL_SIZE);
}  

/**********************************************************************************************/
static void get_help_topics(unsigned int packed_help_info, char **window_topic,
			    char **field_topic, char **contents_topic, 
			    char **version_topic)
{
    unsigned int window_help_vector;
    unsigned int field_help_vector;
    /*  unsigned int ics_number;*/
    /*  short selected_column;*/

    if (!master_help_switch) 
	return;
    
    window_help_vector = UNPACK_WINDOW_HELP(packed_help_info);
    field_help_vector  = UNPACK_FIELD_HELP(packed_help_info);
    /*  ics_number         = UNPACK_ICS_NUMBER(packed_help_info);*/

    if (window_help_vector == RUNTIME_WINDOW_INDICATOR)
	window_help_vector = runtime_window_vector;
    
    if (field_help_vector == RUNTIME_FIELD_INDICATOR_1)
	field_help_vector = runtime_field_vector1;
    
    if (field_help_vector == RUNTIME_FIELD_INDICATOR_2)
	field_help_vector = runtime_field_vector2;

    /*  if (ics_number == RUNTIME_ICS_INDICATOR)
	ics_number = runtime_ics_number;

	if (ics_number) {
	XtVaGetValues(ics_widget, XmNselectedColumn, &selected_column, NULL);      	
	field_help_vector = ics_help_vectors[(ics_number - 1)][selected_column];
	}
    */
    if (window_help_vector > (unsigned int)last_topic_allocated)
	window_help_vector = 0;
    
    if (field_help_vector > (unsigned int)last_topic_allocated)
	field_help_vector = 0;
    
    *window_topic   = help_topics[window_help_vector];
    *field_topic    = help_topics[field_help_vector];
    *contents_topic = help_topics[CONTENTS_HELP_VECTOR];
    *version_topic  = help_topics[VERSION_HELP_VECTOR];
}

/*******************************************************************************************/
void help_menu_callback(Widget w, int menu_selection)
{
    char *window_topic;
    char *field_topic;
    char *contents_topic;
    char *version_topic;
    char msg[1024];

    /* this is only to get rid of compiler complaining about unused parameter */
    w=w;

    if (!master_help_switch) 
	return;
    
    get_help_topics(help_info, &window_topic, &field_topic, &contents_topic,
		    &version_topic);

    switch (menu_selection) {
    case HELP_TOPICS_HELP: 
	send_for_help((char *)HELP_TOPICS_FILE_NAME,contents_topic); 
	break;
    case GETTING_HELP:
	send_for_help( (char *)GETTING_HELP_FILE_NAME,contents_topic);
	break;
    case WINDOW_HELP:
	send_for_help( (char *)WINDOW_HELP_FILE_NAME,window_topic);
	break;
    case FIELD_HELP:
	send_for_help( (char *)FIELD_HELP_FILE_NAME,field_topic);
	break;
    case BOOKS:
	send_for_help( (char *)BOOKS_FILE_NAME,contents_topic);
	break;
    case USER_GUIDE:
	send_for_help( (char *)USER_GUIDE_FILE_NAME,contents_topic);
	break;
    case DATA_ELEMENTS:
	send_for_help( (char *)DATA_ELEMENTS_FILE_NAME,contents_topic);
	break;
    case DATABASE_SPEC:
	send_for_help( (char *)DATABASE_SPEC_FILE_NAME,contents_topic);
	break;
    case INTERFACE_DESIGN:
	send_for_help( (char *)INTERFACE_DESIGN_FILE_NAME,contents_topic);
	break;
    case SYSTEM_ADMIN:
	send_for_help( (char *)SYSTEM_ADMIN_FILE_NAME,contents_topic);
	break;
    case SECURITY_PLAN:
	send_for_help( (char *)SECURITY_PLAN_FILE_NAME,contents_topic);
	break;
    case SECURITY_CONCEPT:
	send_for_help( (char *)SECURITY_CONCEPT_FILE_NAME,contents_topic);
	break;
    case SITE_SECURITY:
	send_for_help( (char *)SITE_SECURITY_FILE_NAME,contents_topic);
	break;
    case TECHNICAL_SUPPORT:
	send_for_help( (char *)TECHNICAL_SUPPORT_FILE_NAME,contents_topic);
	break;
    case VERSION_LABEL:
	sprintf(msg,"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		application_title,
		NTCSS_TITLE,
		version_title,
		NTCSS_COPYRIGHT1,
		NTCSS_COPYRIGHT2,
		NTCSS_COPYRIGHT3,
		NTCSS_COPYRIGHT4);
	popupDialogBox(INFO, True, msg, True, False, False, NULL, toplevel_shell);
	break;
    }
}


/*****************************************************************************************/
static void set_menu_item(MenuItem* menu, int item_number, 
                          int help_callback_item_number, char* label, 
                          char mnemonic, int precede_with_separator)
{
    menu[item_number].label = label;
    menu[item_number].mnemonic = mnemonic;
    menu[item_number].precede_w_separator = precede_with_separator;
    menu[item_number].class = &xmPushButtonGadgetClass;
    menu[item_number].accelerator = NTCSS_NULLCH;
    menu[item_number].accel_text = NTCSS_NULLCH;
    menu[item_number].callback = help_menu_callback;
    menu[item_number].callback_data = (XtPointer) help_callback_item_number;
    menu[item_number].subitems = (MenuItem *) NULL;
}

/***************************************************************************************/
Widget build_help_menu(Widget menubar)
{
    int i;
    static MenuItem help_menu[8];
    static MenuItem book_menu[9];
    
    char str_HELP_TOPICS[] = "Desktop Help Topics                F1      ";
    char str_GETTING_HELP[] = "Getting Help                               ";
    char str_WINDOW_HELP[] = "Window Help                        Ctrl+F1 ";
    char str_FIELD_HELP[] = "Field Help                         Shift+F1";
    char str_BOOKS[] = "Books                                      ";
    char str_TECHNICAL_SUPPORT[] = "Technical Support                          ";
    char str_VERSION_LABEL[] = "Version                                    ";
    char str_USER_GUIDE[] = "User Guide                         F1";
    char str_INTERFACE_DESIGN[] = "Interface Design Document            ";
    char str_SYSTEM_ADMIN[] = "System Administration (NTCSS)        ";
    
    i = 0;
    set_menu_item(help_menu,i++,HELP_TOPICS_HELP, str_HELP_TOPICS, 'D', False);
    set_menu_item(help_menu,i++,GETTING_HELP, str_GETTING_HELP, 'G', True);
    set_menu_item(help_menu,i++,WINDOW_HELP,  str_WINDOW_HELP, 'W', False);
    set_menu_item(help_menu,i++,FIELD_HELP,   str_FIELD_HELP, 'F', False);
    set_menu_item(help_menu,i++,BOOKS,        str_BOOKS, 'B', True);
    set_menu_item(help_menu,i++,TECHNICAL_SUPPORT, str_TECHNICAL_SUPPORT, 'T', True);
    set_menu_item(help_menu,i++,VERSION_LABEL,  str_VERSION_LABEL, 'V', False);
    
    help_menu[i].label = NTCSS_NULLCH;
    help_menu[4].subitems = book_menu;
    
    i = 0;
    set_menu_item(book_menu,i++,USER_GUIDE, str_USER_GUIDE,  'U', False);
    /*  set_menu_item(book_menu,i++,DATA_ELEMENTS,    (char*) "Data Element Dictionary              ",  'D', False); */
    /*  set_menu_item(book_menu,i++,DATABASE_SPEC,    (char*) "Database Specification               ",  'S', False); */
    set_menu_item(book_menu,i++,INTERFACE_DESIGN,  str_INTERFACE_DESIGN,  'I', 
		  False);
    set_menu_item(book_menu,i++,SYSTEM_ADMIN,   str_SYSTEM_ADMIN,  'S', False);
    /*  set_menu_item(book_menu,i++,SECURITY_PLAN,     (char*)"Security Plan                        ",  'P', False); */
    /*  set_menu_item(book_menu,i++,SECURITY_CONCEPT,  (char*)"Security Concept of Operations       ",  'C', False); */
    /*  set_menu_item(book_menu,i++,SITE_SECURITY,     (char*)"Site Security Planning Guide         ",  'G', False); */

    book_menu[i].label = NTCSS_NULLCH;
    
    return BuildPulldownMenu(menubar, "HELP", 'H', help_menu);
}

/***************************************************************************************/
void set_runtime_window_help_vector(unsigned int help_vector)
{
    runtime_window_vector = help_vector;
}

/****************************************************************************************/
void set_runtime_field_help_vector1(unsigned int help_vector)
{
    runtime_field_vector1 = help_vector;
}

/****************************************************************************************/

void set_runtime_field_help_vector2(unsigned int help_vector)
{
    runtime_field_vector2 = help_vector;
}

/***************************************************************************************/
#ifdef foo
void set_runtime_ics_number(unsigned int ics_number)
{
    runtime_ics_number = ics_number;
}
#endif
/********************************************************************************************/
#ifdef foo
void set_runtime_ics_info(Widget new_ics_widget, unsigned int window)
{
    ics_widget = new_ics_widget; 
    help_info = (unsigned int)PACK_HELP_INFO(window, 0, RUNTIME_ICS_INDICATOR);
}
#endif

/**********************************************************************************************/
static void debug_help_info(const char *title)
{
#ifdef DEBUG
    short selected_column;
    char *window_topic;
    char *field_topic;
    char *contents_topic;
    char *version_topic;

    /* done only to get rid of compiler warning about unused parameter */
    title=title;

    if (!master_help_switch) 
	return;
    
    get_help_topics(help_info,&window_topic,&field_topic,&contents_topic,
		    &version_topic);
    syslog(LOG_WARNING,"%s %s %s",title,window_topic,field_topic);
#else
    /* done only to get rid of compiler warning about unused parameter */
    title=title;
#endif

}

/*****************************************************************************************/
void process_help_request(Widget w, char not_used, XmAnyCallbackStruct* cbs)
{
    XComposeStatus compose;
    KeySym keysym;
    char buffer[2];
    int buffer_size = 2;
    char *window_topic;
    char *field_topic;
    char *contents_topic;
    char *version_topic;

    /* done only to get rid of compiler warning about unused parameter */
    not_used=not_used;
    w=w;
    
    if (!master_help_switch) 
	return;
    
    if (cbs->event->type == KeyPress) {
	(void) XLookupString((XKeyEvent*)cbs->event, buffer, buffer_size, 
			     &keysym, &compose);
	if ((keysym & XK_F1) == XK_F1) {
	    
	    get_help_topics(help_info,&window_topic,&field_topic,
			    &contents_topic,&version_topic);
	    
	    if ((cbs->event->xkey.state & XK_Shift_L) || 
		(cbs->event->xkey.state & XK_Shift_R)) {

#ifdef DEBUG
		syslog(LOG_WARNING,"Field Help - %s",field_topic);
#endif
		send_for_help((char *)FIELD_HELP_FILE_NAME,field_topic);

	    } else if ((cbs->event->xkey.state & XK_Control_L) || 
		       (cbs->event->xkey.state & XK_Control_R)) {

#ifdef DEBUG
		syslog(LOG_WARNING,"Window Help - %s",window_topic);
#endif
		send_for_help((char *)WINDOW_HELP_FILE_NAME,window_topic);

	    } else {

#ifdef DEBUG
		syslog(LOG_WARNING,"Online Books - %s %s",contents_topic,
		       version_topic);
#endif
		send_for_help((char *)USER_GUIDE_FILE_NAME,contents_topic);
	    }
	}
    }
}

/*******************************************************************************************/
void set_focus(Widget w, XtPointer client_data, XEvent *event)
{
    /* done only to get rid of compiler warning about unused parameter */
    w=w;

    if (!master_help_switch)
	return;
    
    if (event->type == FocusIn)
	XmProcessTraversal((Widget) client_data, XmTRAVERSE_CURRENT);
}

/*****************************************************************************************/
void set_focus_on_popup(Widget w, Widget default_widget, XmAnyCallbackStruct *cbs)
{
    /* done only to get rid of compiler warning about unused parameter */
    w=w;
    cbs=cbs;
    
    if (!master_help_switch)
	return;
    
    XmProcessTraversal(default_widget, XmTRAVERSE_CURRENT);
}

/****************************************************************************************/
void set_help_info(Widget w, XtPointer client_data, XEvent* event)
{
    XComposeStatus compose;
    KeySym keysym;
    char buffer[2];
    int buffer_size = 2;
    
    /* Added for compiler */
    w=w;
    if (!master_help_switch)
	return;
    
    if ((event->type == ButtonPress) || ((event->type == FocusIn) && 
					 allow_setting_help_on_focus_in)) {

	help_info = (unsigned int) client_data; 

#ifdef foo
	if (UNPACK_ICS_NUMBER(help_info))
	    ics_widget = w;
#endif

	if ((event->type == ButtonPress))
	    debug_help_info("set_help_info ButtonPress");
	else if ((event->type == FocusIn) && allow_setting_help_on_focus_in)
	    debug_help_info("set_help_info Focus In");
	
	allow_setting_help_on_focus_in = False;
	
    } else if (event->type == KeyPress) {
	
	(void) XLookupString((XKeyEvent*)event, buffer, buffer_size, &keysym, &compose);
	allow_setting_help_on_focus_in = ((keysym & XK_Tab) == XK_Tab);
	
#ifdef DEBUG
	if (allow_setting_help_on_focus_in) 
	    syslog(LOG_WARNING,"allow_setting_help_on_focus_in set");
#endif

  }
}

/****************************************************************************************/
void set_default_help_info(unsigned int window, unsigned int field)
{
    if (!master_help_switch)
	return;
    
    save_help_info();
    help_info = (unsigned int)PACK_HELP_INFO(window, field, 0);
    debug_help_info("DEFAULT");
}

/****************************************************************************************/
static int read_help_ini_file(char *filename)
{
    char values[VALUE_BUFFER_SIZE];
    char sections[SECTION_BUFFER_SIZE];
    char *section_ptr;
    char *value_ptr;
    int row;
    /*  int column;*/

    /* Initialize the ics vector matrix */

#ifdef foo
    for (row = 0; row < MAX_ICS_WIDGETS; row++)
	for (column = 0; column < MAX_COLUMNS_IN_ICS_WIDGET; column++)
	    ics_help_vectors[row][column] = 0;
#endif

    last_topic_allocated = -1;
    
    if (!filename)
	return False;
    
    if (!*filename)
	return False;
    
    if (LoadINIFile(filename) < 0)
	return False;
    
    if (GetProfileSectionNames(sections,SECTION_BUFFER_SIZE,filename) < 0)
	return False;
    
    if (GetSectionValues(sections,value_ptr=values,VALUE_BUFFER_SIZE,filename) < 0)
	return False;

    /* Read Help Topics */

    while (*value_ptr && ((last_topic_allocated + 1) < MAX_HELP_TOPICS)) {
	help_topics[++last_topic_allocated] = malloc(strlen(value_ptr));
	strcpy(help_topics[last_topic_allocated],value_ptr);
	value_ptr = strchr(value_ptr,0) + 1;
    }
    
    section_ptr = strchr(sections,0) + 1;
    row = -1;

    /* Read ICS Vectors */
#ifdef foo
    while (*section_ptr) {
	GetSectionValues(section_ptr,value_ptr=values,VALUE_BUFFER_SIZE,filename);
	if (++row >= MAX_ICS_WIDGETS) exit;
	column = -1;
	while (*value_ptr) {
	    if (++column >= MAX_COLUMNS_IN_ICS_WIDGET) exit;
	    ics_help_vectors[row][column] = (short) atoi(value_ptr);
	    value_ptr = strchr(value_ptr,0) + 1;
	}
	section_ptr = strchr(section_ptr,0) + 1;
  }
#endif

#ifdef DEBUG
    for (row=0;row<=last_topic_allocated;row++)
	syslog(LOG_WARNING,"%d) %s",row,help_topics[row]);
#endif

    return True;

}

/*********************************************************************************************/
void read_help(char *db_dir, char *filename,char *app_title,Widget shell)
{
    char filespec[MAX_FILE_SPEC_SIZE];
    char version_number_string[VERSION_TITLE_SIZE];

    /* done only to get rid of compiler warning about unused parameter */
    db_dir=db_dir;
    
    master_help_switch = 0;
    
    sprintf(filespec,"%s/%s",NTCSS_HELP_INI_DIR,filename);
    master_help_switch = read_help_ini_file(filespec);
    
    memcpy(application_title,app_title,APPLICATION_TITLE_SIZE);
    toplevel_shell = shell;
    
    if (getVersionString(version_number_string))
	sprintf(version_title,"Version %s",version_number_string);
    else
	/*sprintf(version_title,"");*/
	version_title[0]=0;

    if (master_help_switch)
	syslog(LOG_WARNING,"Context sensitive help initialized: %s",filespec);
    else
	syslog(LOG_WARNING,"Could not initialize help: %s",filespec);

}

/*******************************************************************************************/
void free_help()
{
    int i;
    
    for (i=0; i <= last_topic_allocated; i++)
	free(help_topics[i]);
}
