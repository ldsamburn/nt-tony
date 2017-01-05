
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/* lp_ops.c : LP Scheduler operations */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <Ntcss.h>
#include <lp_ops.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/** INRI INCLUDES **/

#include <LibNtcssFcts.h>


/*****************************************************************************/

int  lp_shut( void )

{
  char cmd[255];
  int rc;

#ifdef SYSV
    strcpy(cmd, LP_SHUT_CMD);
#endif
#ifdef BSD
    sprintf(cmd, LP_SHUT_CMD, printer);
#endif
    rc=system(cmd);
    
    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }
}


/*****************************************************************************/

int  lp_start( void )

{
char cmd[255];
int rc;

#ifdef SYSV
    strcpy(cmd, LP_START_CMD);
#endif
#ifdef BSD
    sprintf(cmd, LP_START_CMD, current_printer.name);
#endif
    rc=system(cmd);
    
    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }
}


/*****************************************************************************/

int  lp_enable( const char  *current_printer )

{
char cmd[255];
int rc;

        /* Both SYSV and BSD CMD take current_printer */
    sprintf(cmd,"/usr/bin/enable %s > /dev/null",current_printer);
    rc=system(cmd);
    
    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }
}


/*****************************************************************************/

int  lp_disable( const char *current_printer )

{
char cmd[255];
int rc;

        
        /* Both SYSV and BSD CMD take current_printer */
    sprintf(cmd, LP_DISABLE_CMD, current_printer);

    rc=system(cmd);
    

    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }

}


/*****************************************************************************/

int  lp_accept( const char  *current_printer )

{
char cmd[255];
int rc;

        /* Both SYSV and BSD CMD take current_printer */
    sprintf(cmd, LP_ACCEPT_CMD, current_printer);
    rc=system(cmd);
    

    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }

}


/*****************************************************************************/

int  lp_reject( const char  *current_printer )

{
char cmd[255];
int rc;

        /* Both SYSV and BSD CMD take current_printer */
    sprintf(cmd, LP_REJECT_CMD, current_printer);
    rc=system(cmd);
    

    if (rc && 0xff00) {
        return(0);
    } else {
        return(1);
    }

}


/*****************************************************************************/

int  lp_isaccept( const char  *printer_name )

{
FILE *pfp;
char cmd[256];
char buf[256];
int rc=QUEUE_DISABLED;

        /* Both SYSV and BSD CMD take current_printer */
        sprintf(cmd, LP_ISACCEPT_STATUS_CMD, printer_name);


        if ((pfp=popen(cmd,"r")) == NULL) {
                printf("Unable to determine status of %s\n",printer_name);
                return(QUEUE_ENABLED);
        }

    alarm(5);

        if (!fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp)) 
        rc=QUEUE_DISABLED;
    alarm(0);

#ifdef BSD
    alarm(5);
        if (!fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp)) 
        rc=QUEUE_DISABLED;
    alarm(0);
    alarm(5);
        if (!fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp)) 
        rc=QUEUE_DISABLED;
    alarm(0);
        if (strstr(buf,"not accepting"))
                rc=QUEUE_DISABLED;
        else if (strstr(buf,"accepting"))
                rc=QUEUE_ENABLED;
#endif

#ifdef SYSV
        if (strstr(buf,"not accepting"))
                rc=QUEUE_DISABLED;
        else if (strstr(buf,"accepting"))
                rc=QUEUE_ENABLED;
    if (strstr(buf,"non-existent"))
        rc = -1;
#endif
        pclose(pfp);
        return(rc);
}


/*****************************************************************************/

int  lp_isenable( const char  *printer_name )

{
FILE *pfp;
char cmd[256];
char buf[256];
int rc=PRINTER_DISABLED;


       /* Both SYSV and BSD CMD take current_printer */
       sprintf(cmd, LP_ISENABLE_STATUS_CMD, printer_name);

    if ((pfp=popen(cmd,"r")) == NULL) {
        printf("Unable to determine status of %s\n",printer_name);
        return(PRINTER_ENABLED);
    }
    
    fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp);
#ifdef BSD
    fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp);
    fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp);
    if (strstr(buf,"disabled")) 
        rc=PRINTER_DISABLED;
    if (strstr(buf,"enabled"))
        rc=PRINTER_ENABLED;
    if (strstr(buf,"non-existent"))
        rc = -1;
#endif
        
#ifdef SYSV
    if (strstr(buf,"disabled")) 
        rc=PRINTER_DISABLED;
    if (strstr(buf,"enabled"))
        rc=PRINTER_ENABLED;
    if (strstr(buf,"non-existent"))
        rc = -1;
#endif
    pclose(pfp);
    return(rc);
}


/*****************************************************************************/

int  lp_issched( const char  *printer_name )

{
FILE *pfp;
char cmd[256];
char buf[256];
int rc=SCHEDULER_ENABLED;

#ifdef BSD
    sprintf(cmd,LP_ISSCHED_CMD,printer_name);
#endif
#ifdef SYSV
    strcpy(cmd,LP_ISSCHED_CMD);
#endif

    if ((pfp=popen(cmd,"r")) == NULL) {
        printf("Unable to determine status of %s\n",printer_name);
        return(SCHEDULER_ENABLED);
    }
    
    fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp);
#ifdef BSD
    fgets(buf,MAX_PROC_CMD_LINE_LEN,pfp);
    if (strstr(buf,"disabled")) 
        rc=SCHEDULER_DISABLED;
    if (strstr(buf,"enabled"))
        rc=SCHEDULER_ENABLED;
#endif
        
#ifdef SYSV
    if (strstr(buf,"not running")) 
        rc=SCHEDULER_DISABLED;
    if (strstr(buf,"is running"))
        rc=SCHEDULER_ENABLED;
#endif
    pclose(pfp);
    return(rc);
}


/*****************************************************************************/

int  lp_exists( const char  *prname )

{
int rc;

#ifdef BSD
        /* here we need to check the printcap */
        rc=0;
#endif

#ifdef SYSV
struct stat st;
char lppath[2048];
int interface_stat;

        rc=0;

        sprintf(lppath, LP_EXISTS_INTERFACE_CMD, prname);
        interface_stat=stat(lppath,&st);

        if (!interface_stat)
                rc=1;
#endif

        return(rc);
}


/*****************************************************************************/
