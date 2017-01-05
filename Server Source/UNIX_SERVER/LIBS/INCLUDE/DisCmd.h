
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * DisCmd.h
 */

#ifndef _DisCmd_h
#define _DisCmd_h

/** GLOBAL DECLARATIONS **/

/** DisCmd.c **/

extern void XiDisplayCommand(/* XtAppContext, Widget, Widget, (char *),
								(char *), (Display *), (char*) */);
/*
  XtAppContext (app_context of application.)  
  Widget t (Highest level shell);
  Widget w (Parent of display dialog);
  char *CommandToExecute;
  char *WindowTitle;
  Display *display;
  char *regular_expression;
*/

extern void XiDisplayCommandClose(/* void */);
/* Call this function to cleanup processes launched by XiDisplayCommand() */

#endif /* _DisCmd_h */
