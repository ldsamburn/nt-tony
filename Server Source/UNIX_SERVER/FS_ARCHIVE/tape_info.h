
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * tape_info.h
 */

#ifndef _tape_info_h
#define _tape_info_h


/** INRI INCLUDES **/

#include <Ntcss.h>


/** DEFINE DEFINITIONS **/

#define MAX_ITEM_LEN            512
#define MAX_FILELINE_LEN        512
#define MAX_FILELISTING_LEN     (MAX_FILENAME_LEN + 100)
#define MAX_DEVNAME_LEN         254
#define MAX_USER_LEN            8
#define MAX_GROUP_LEN           8

#define NO_MSG_MSGTYPE          0
#define ERR_LIST_MSGTYPE        1
#define FULL_LIST_MSGTYPE       2


/** GLOBAL DECLARATIONS **/

/** tarlist_fcts.c **/

extern Problem create_tarlist(char*, char*);
extern Problem determine_filelist_size(char*, int*);
extern void build_file_listing(char*, char*);
extern void get_owner(char*, struct stat*);


#endif /* _tape_info_h */
