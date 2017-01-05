
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* bcserver.h */

/** SYSTEM INCLUDES **/

#if defined(hpux)
#include <string.h>
#else
#include <strings.h>
#include <arpa/inet.h>
#endif /* hpux */
/* #if !defined(hpux) */
/* #include <arpa/inet.h> */
/* #endif  hpux  */

/* #include <stdio.h> */
/* #include <fcntl.h> */
/* #include <ctype.h> */
/* #include <syslog.h> */
/* #include <signal.h> */
/* #include <sgtty.h> */
/* #include <errno.h> */
/* #include <varargs.h> */
/* #include <pwd.h> */
/* #include <netdb.h> */
/* #include <sys/types.h> */
/* #include <sys/param.h> */
/* #include <sys/file.h> */
/* #include <sys/wait.h> */
/* #include <sys/socket.h> */
/* #include <netinet/in.h> */
/* #include <sys/stat.h> */


/** INRI INCLUDES  **/

#include <Ntcss.h>

/* TEMPORARY DBDIR */
/* #define PRINTQ_DBDIR "/usr/local/NTCSSII/database" */


/* a few machine specifics */
#if defined(ultrix) || defined(bsd4_2)
#define OPENLOG(x,y,z)	openlog(x,y)
#define SECLOG_PRI	LOG_CRIT
#else
#define OPENLOG(x,y,z)	openlog(x,y,z)
#define SECLOG_PRI	LOG_NOTICE|LOG_AUTH
#endif

/* define the truth */
#define TRUE	1
#define FALSE	0

/* define some chars to use */
#define SPACE 0x20
#define LF 0x0a

/* default ports */
#define DEFAULT_BCSERVER_PORT 5582

/* default ntcss spool directory */
/* #define DEFAULT_NTCSS_LP_SPOOL_DIR "/usr/local/NTCSSII/spool/lp" */

/* request tyep defs */
#define NTCSS_PRINT_REQUEST      1
#define NON_NTCSS_PRINT_REQUEST  2

#define NTCSS_PREFIX       "NTCSS_PRINTREQ:"
#define NTCSS_REQUEST      "NTCSS Request"
#define NON_NTCSS_REQUEST  "Non-NTCSS Request"


/** LPR command structure 
struct cmd_struct {
        unsigned char cmd;
        char *cmd_description;
        int (*func_ptr)();
        int (*action_ptr)();
        int (*subcmd_ptr)();
};
**/
/* extern char version[]; */

struct PRINTQ_MSG {
	char message_type[20];
	char printer_name[MAX_PRT_NAME_LEN];
};


/** GLOBAL PROTOTYPES **/
/*int get_line(), *malloc(); */


#ifdef hp300
extern char *tempnam();
#endif

