
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * NetSvrInfo.h
 */

#ifndef _NetSvrInfo_h
#define _NetSvrInfo_h

#include <Ntcss.h> /* Needed for MAX_HOST_NAME_LEN */


/** DEFINES **/

#define DAEMON_STATUS_STR_LEN       16     /* # chars in daemon response msg.*/
#define DAEMON_STATE_IDLE           1      /* for DaemonStats overall field */
#define DAEMON_STATE_AWAKE          2      /* for DaemonStats overall field */
#define DAEMON_STATE_UNKNOWN        256    /* for DaemonStats overall field */
#define DAEMON_STATE_DEBUG_OFF      1      /* for DaemonStats debug field */
#define DAEMON_STATE_DEBUG_ON       2      /* for DaemonStats debug field */
#define DAEMON_STATE_NO_INFO        0      /* for DaemonStats logins field */
#define DAEMON_STATE_ALLOWED        1      /* for DaemonStats logins field */
#define DAEMON_STATE_BLOCKED        2      /* for DaemonStats logins field */
#define NUM_NTCSS_SERVER_DAEMONS    6


/** TYPEDEF DEFINITIONS **/

typedef struct _daemon_stats { /* Status of a single NTCSS server daemon */
  int overall;                  /* idle, awake, unknown, etc. */
  int debug;                    /* debug status is on or off */
  int logins;                   /* can be used to represent requests */
} DaemonStats;

typedef struct _host_stats {        /* Status of each NTCSS server daemon.. */
  char hostname[MAX_HOST_NAME_LEN]; /* Host these daemons are running on. */
  DaemonStats  jobq_server;
  DaemonStats  printq_server;
  DaemonStats  db_server;
  DaemonStats  desktop_server;
  DaemonStats  command_server;
  DaemonStats  message_server;
} HostStats;


/** functions in NetSvrInfo.c **/

#ifdef __cplusplus
extern "C"{
#endif

int  are_all_daemons_idled_on_host(const char*, HostStats*);
int  can_master_be_shut_down(void);
int  clear_svr_daemon_struct(DaemonStats*);
int  GetAllDaemonsStatusOnHost(const char*, HostStats*);
int  GetDaemonStatusStr(const char*, int, char*, int);
int  get_master_server(const char*, char*);
int  getNtcssPort(const char*, int);
int  GrepNumGensRunning(void);
int  IsDaemonAwakeOnHost(const char*, int );
int  IsDaemonIdledOnHost(const char*, int);
int  IsdaemonRunningOnHost(const char*, int);
int  IsDBdaemonIdledOnHost(const char*);
int  IsDBdaemonRunningOnHost(const char*);
int  is_host_valid(const char*);
int  NumDaemonsRunningOnHost(const char*);
int  NumDaemonsRunningOnThisHost(void);
int  ParseIntoDaemonStats(DaemonStats*, const char*);
int  getDebugState(char*, int);

/* The following files should be moved to some file of their own.. */
int  post_system_message(const char*, const char*, const char*, const char*,
                         int);
int  sendAdminToDm(const char*, const char*, char*, const char*);
int  sendChangePasswordToHost(char*, char*, char*, char*);
int  sendCurrentAppDataToDm(const char*, ProgroupUseStruct*, int);
int  send_msg_to_all_hosts(const char*, int, int );
int  send_msg_to_daemon(const char*, int, const char*, int, char*, int, int );
int  send_msg_to_host_daemons(const char*, const char*, int );

#ifdef __cplusplus
};
#endif

#endif  /* _NetSvrInfo_h */
