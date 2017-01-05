
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * NtcssHelp.h
 */

#define MAX_ICS_WIDGETS            10
#define MAX_COLUMNS_IN_ICS_WIDGET  20
#define MAX_HELP_TOPICS           100

#define WINDOW_VECTOR_MASK         0xFFFC0000
#define FIELD_VECTOR_MASK          0x0003FFF0
#define ICS_NUMBER_MASK            0x0000000F
#define CLEAR_ICS_MASK             0xFFFFFFF0
#define RUNTIME_WINDOW_INDICATOR   0x3FFF
#define RUNTIME_FIELD_INDICATOR_1  0x3FFF
#define RUNTIME_FIELD_INDICATOR_2  0x3FFE
#define RUNTIME_ICS_INDICATOR      0xF
#define WINDOW_VECTOR_SIZE         14
#define FIELD_VECTOR_SIZE          14
#define ICS_NUMBER_SIZE            4


#define PACK_HELP_INFO(window_help_vector, field_help_vector, ics_number) \
(XtPointer)(((window_help_vector) << (FIELD_VECTOR_SIZE + ICS_NUMBER_SIZE)) +        \
 ((field_help_vector) << ICS_NUMBER_SIZE) +                               \
  (ics_number))

#define UNPACK_WINDOW_HELP(packed_help_info) \
(((packed_help_info) & WINDOW_VECTOR_MASK) >> (FIELD_VECTOR_SIZE + ICS_NUMBER_SIZE))

#define UNPACK_FIELD_HELP(packed_help_info) \
(((help_info) & FIELD_VECTOR_MASK) >> ICS_NUMBER_SIZE)

#define UNPACK_ICS_NUMBER(packed_help_info) \
((packed_help_info)  & ICS_NUMBER_MASK)

#define SET_HELP_INFO(widget_to_set,window_vector,field_vector,ics_number)    \
XtInsertEventHandler(widget_to_set,                                           \
		     ButtonPressMask | FocusChangeMask | KeyPressMask,        \
		     False,                                                   \
		     (XtEventHandler)set_help_info,                           \
		     PACK_HELP_INFO(window_vector, field_vector, ics_number), \
		     XtListHead) 

#define UNSET_HELP_INFO(widget_to_set,window_vector,field_vector,ics_number)    \
XtRemoveEventHandler(widget_to_set,                                             \
		     ButtonPressMask | FocusChangeMask | KeyPressMask,          \
		     False,                                                     \
		     set_help_info,                                             \
		     PACK_HELP_INFO(window_vector, field_vector, ics_number))

#define SET_DEFAULT_HELP(window_widget, window_vector, field_vector)          \
XtInsertEventHandler(window_widget,                                           \
		     FocusChangeMask,                                         \
		     False,                                                   \
		     (XtEventHandler)process_window_focus_event,              \
		     PACK_HELP_INFO(window_vector, field_vector, 0),          \
		     XtListHead) 

#define SET_DEFAULT_HELP_FOR_POPUP(popup_widget, window, field)  \
XtAddCallback(popup_widget,                                      \
	      XtNpopupCallback,                                  \
	      process_popup_for_setting_help_info,               \
	      PACK_HELP_INFO(window,field,0))


#define SET_EXTENDED_LIST_HELP(ics_widget, list_widget, window_vector, ics_number) \
if ((list_widget = XtNameToWidget(ics_widget, (String) "*list")))                    \
  SET_HELP_INFO(list_widget, window_vector, 0, ics_number)

#define SET_TEXT_FIELD_HELP(text_field_widget, window_vector, field_vector) \
SET_HELP_INFO(text_field_widget, window_vector, field_vector, 0)

#define SET_SCROLLED_LIST_HELP(scrolled_list_widget, window_vector, field_vector) \
SET_HELP_INFO(scrolled_list_widget, window_vector, field_vector, 0)

#define SET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, field_vector) \
if ((text_widget = XtNameToWidget(combo_box_widget, (String) "*text")))                  \
  SET_HELP_INFO(text_widget, window_vector, field_vector, 0);                          \
if ((text_widget = XtNameToWidget(combo_box_widget, (String) "*arrow")))                 \
  SET_HELP_INFO(text_widget, window_vector, field_vector, 0);                          \
if ((text_widget = XtNameToWidget(combo_box_widget, (String) "*list")))                  \
  SET_HELP_INFO(text_widget, window_vector, field_vector, 0)

#define UNSET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, field_vector) \
if (text_widget = XtNameToWidget(combo_box_widget, (String) "*text"))                    \
  UNSET_HELP_INFO(text_widget, window_vector, field_vector, 0);                          \
if (text_widget = XtNameToWidget(combo_box_widget, (String) "*arrow"))                   \
  UNSET_HELP_INFO(text_widget, window_vector, field_vector, 0);                          \
if (text_widget = XtNameToWidget(combo_box_widget, (String) "*list"))                    \
  UNSET_HELP_INFO(text_widget, window_vector, field_vector, 0)

#define SET_EXTENDED_LIST_HELP_WITH_RUNTIME_WINDOW(ics_widget, list_widget, ics_number) \
SET_EXTENDED_LIST_HELP(ics_widget, list_widget, RUNTIME_WINDOW_INDICATOR, ics_number)

#define SET_EXTENDED_LIST_HELP_WITH_RUNTIME_ICS(ics_widget, list_widget, window_vector) \
SET_EXTENDED_LIST_HELP(ics_widget, list_widget, window_vector, RUNTIME_ICS_INDICATOR)

#define SET_TEXT_FIELD_HELP_WITH_RUNTIME_WINDOW(text_field_widget, field_vector) \
SET_TEXT_FIELD_HELP(text_field_widget, RUNTIME_WINDOW_INDICATOR, field_vector)

#define SET_SCROLLED_LIST_HELP_WITH_RUNTIME_WINDOW(scrolled_list_widget, field_vector) \
SET_SCROLLED_LIST_HELP(scrolled_list_widget, RUNTIME_WINDOW_INDICATOR, field_vector)

#define SET_COMBO_BOX_HELP_WITH_RUNTIME_WINDOW(combo_box_widget, text_widget, field_vector) \
SET_COMBO_BOX_HELP(combo_box_widget, text_widget, RUNTIME_WINDOW_INDICATOR, field_vector)

#define SET_HELP_CALLBACK(help_callback_widget) \
XtAddCallback(help_callback_widget, XmNhelpCallback, (XtCallbackProc)process_help_request, NULL)

#define SET_POPUP_DEFAULT(help_popup_widget, default_widget) \
XtAddCallback(help_popup_widget, XtNpopupCallback, set_focus_on_popup, default_widget)

#define SET_DEFAULT_WIDGET(window_widget,default_widget) \
XtInsertEventHandler(window_widget,FocusChangeMask,False,(XtEventHandler)set_focus,default_widget,XtListHead)

#define SET_TEXT_FIELD_HELP_WITH_RUNTIME_FIELD_1(text_field_widget, window_vector) \
SET_TEXT_FIELD_HELP(text_field_widget, window_vector, RUNTIME_FIELD_INDICATOR_1)

#define SET_TEXT_FIELD_HELP_WITH_RUNTIME_FIELD_2(text_field_widget, window_vector) \
SET_TEXT_FIELD_HELP(text_field_widget, window_vector, RUNTIME_FIELD_INDICATOR_2)

#define SET_COMBO_BOX_HELP_WITH_RUNTIME_FIELD_1(combo_box_widget, text_widget, window_vector) \
SET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, RUNTIME_FIELD_INDICATOR_1)

#define SET_COMBO_BOX_HELP_WITH_RUNTIME_FIELD_2(combo_box_widget, text_widget, window_vector) \
SET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, RUNTIME_FIELD_INDICATOR_2)

#define UNSET_COMBO_BOX_HELP_WITH_RUNTIME_FIELD_1(combo_box_widget, text_widget, window_vector) \
UNSET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, RUNTIME_FIELD_INDICATOR_1)

#define UNSET_COMBO_BOX_HELP_WITH_RUNTIME_FIELD_2(combo_box_widget, text_widget, window_vector) \
UNSET_COMBO_BOX_HELP(combo_box_widget, text_widget, window_vector, RUNTIME_FIELD_INDICATOR_2)

#define SET_SCROLLED_LIST_HELP_WITH_RUNTIME_FIELD_1(scrolled_list_widget, window_vector) \
SET_SCROLLED_LIST_HELP(scrolled_list_widget,window_vector, RUNTIME_FIELD_INDICATOR_1)    \

#define SET_SCROLLED_LIST_HELP_WITH_RUNTIME_FIELD_2(scrolled_list_widget, window_vector) \
SET_SCROLLED_LIST_HELP(scrolled_list_widget,window_vector, RUNTIME_FIELD_INDICATOR_2)    \


extern void save_help_info(void);
extern void restore_help_info(void);
extern void send_for_help(const char*, char*);
extern void set_help_info(Widget, XtPointer, XEvent*);
extern void process_help_request(Widget, char, XmAnyCallbackStruct*);
extern void read_help(char*, char*, char*, Widget);
extern void free_help(void);
extern void set_runtime_window_help_vector(unsigned int);
extern void set_runtime_field_help_vector1(unsigned int);
extern void set_runtime_field_help_vector2(unsigned int);
extern void set_runtime_ics_number(unsigned int);
extern void set_runtime_ics_info(Widget, unsigned int);
extern void set_default_help_info(unsigned int, unsigned int);
extern void set_focus(Widget, XtPointer, XEvent*);
extern void set_focus_on_popup(Widget, Widget, XmAnyCallbackStruct*);
extern void help_menu_callback(Widget, int);
extern Widget build_help_menu(Widget);

