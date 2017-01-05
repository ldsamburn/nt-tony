
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * NetSvrInfo.c
 */

/* Only functions related with network Ntcss server daemon communications
   and information should be in this file. */
/* Local function & global var names (signal handling code) are prefixed
   by "NSI" */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <setjmp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <SockFcts.h>
#include <StringFcts.h>
#include <NetSvrInfo.h>          /* structures defined in here */
#include <NetCopy.h>
#include <LibNtcssFcts.h>
#include <ExtraProtos.h>

/** LOCAL INCLUDES **/

/** GLOBAL DECLARATIONS **/

/** FUNCTION PROTOTYPES **/

void  NSIsignalAlarm(int);
void  NSIPipeSignalAlarm(int);


/** STATIC GLOBAL VARIABLES **/

static jmp_buf  NSI_jump_buff;     /* used for the longjmp() stack */

/** GLOBAL VARIABLES **/

/** FUNCTION DEFINITIONS **/


/*****************************************************************************/

/* Checks specified host to see if all of it's daemons are idled.
   If provided_status_info is NULL, this procedure gathers info for itself.
   If provided_host_name is NULL, the functions implies the current host.
*/

int  are_all_daemons_idled_on_host( const char  *provided_host_name,
                                    HostStats   *provided_status_info )
    
{
  const char  *fnct_name = "are_all_daemons_idled_on_host():";
  char         local_hosts_name[MAX_HOST_NAME_LEN + 1];
  const char  *name_ptr;
  HostStats    local_servers_status;
  HostStats   *status_ptr;        /* Points to which structure is to be used.*/

  if(provided_host_name == NULL)
    {
      if (gethostname(local_hosts_name, MAX_HOST_NAME_LEN + 1) != 0)
        {
          syslog(LOG_WARNING, "%s Failed to get this host's name!\n",
                 fnct_name);
          return(-1);
        }
      name_ptr = local_hosts_name;
    }
  else
    name_ptr = provided_host_name;


  /* figure out which structure to access for our info.. */
  if (provided_status_info == NULL)   /* info was NOT provided by caller... */
    {
      GetAllDaemonsStatusOnHost(name_ptr, &local_servers_status);
      status_ptr = &local_servers_status;
    }
  else
    status_ptr = provided_status_info;

  if ((status_ptr->jobq_server.overall ==    DAEMON_STATE_IDLE) &&
      (status_ptr->printq_server.overall ==  DAEMON_STATE_IDLE) &&
      (status_ptr->db_server.overall ==      DAEMON_STATE_IDLE) &&
      (status_ptr->desktop_server.overall == DAEMON_STATE_IDLE) &&
      (status_ptr->command_server.overall == DAEMON_STATE_IDLE) &&
      (status_ptr->message_server.overall == DAEMON_STATE_IDLE))
    return(1);
  else
    return(0);
}


/*****************************************************************************/

/* given a hostname, this functions queries that host for all its daemons'
   statuses.  If the statuses are unknown, then it is assumed that the daemons
   are not running so they can respond.  NUM_NTCSS_SERVER_DAEMONS should
   respond if all are running.
 */

int  NumDaemonsRunningOnHost( const char  *host_name )

{
  HostStats  svr_stats;
  int  i; 
  int  num_daemons = 0;                    /* default is no daemons running */

  i = GetAllDaemonsStatusOnHost(host_name, &svr_stats); /* fill in svr_stats */
  if(i < 0)
    return(-1);

  if (svr_stats.jobq_server.overall !=  DAEMON_STATE_UNKNOWN)
    num_daemons++;
  if (svr_stats.printq_server.overall != DAEMON_STATE_UNKNOWN)
    num_daemons++;
  if (svr_stats.db_server.overall != DAEMON_STATE_UNKNOWN)
    num_daemons++;
  if (svr_stats.desktop_server.overall != DAEMON_STATE_UNKNOWN)
    num_daemons++;
  if (svr_stats.command_server.overall != DAEMON_STATE_UNKNOWN)
    num_daemons++;
  if (svr_stats.message_server.overall != DAEMON_STATE_UNKNOWN)
    num_daemons++;

  return(num_daemons);
}


/*****************************************************************************/
/* Returns the number of daemons contacted as running on the current host.
 */

int  NumDaemonsRunningOnThisHost( void )

{
  char  this_hosts_name[MAX_HOST_NAME_LEN + 1];
  int   num_active_daemons = 0;

  if (gethostname(this_hosts_name, MAX_HOST_NAME_LEN) != 0) 
    {
      syslog(LOG_WARNING, "areAnyDaemonsRunningOnThisHost(): Cannot get"
             " the name of this host!");
      return(-1);
    }
  num_active_daemons = NumDaemonsRunningOnHost(this_hosts_name);
  if (num_active_daemons < 0)
    return(-2);                          /* error should already be reported */
  else
    return(num_active_daemons);
}


/*****************************************************************************/
/* Checks current master server's desktop daemon to make sure it isn't on-line
   or not accepting any login requests.
   Returns 1 true if it appears safe to make another the master server.
 */

int  can_master_be_shut_down()

{
  int  i;                                                   /* temp storage */
  char  masterhostname[MAX_HOST_NAME_LEN + 1];
  HostStats  svr_stats;                  /* holds server's daemon's statuses */

  i = get_master_server(NTCSS_DB_DIR, masterhostname); 
  if (i < 0)
    return(-1);

  i = GetAllDaemonsStatusOnHost(masterhostname, &svr_stats);

  if((svr_stats.desktop_server.overall != DAEMON_STATE_AWAKE) ||
     (svr_stats.desktop_server.logins != DAEMON_STATE_ALLOWED))
    return(1);
  return(0);
}


/*****************************************************************************/

/* Clears out a DaemonStats structure */

int clear_svr_daemon_struct(daemon_struct)

     DaemonStats  *daemon_struct;

{
  daemon_struct->overall = DAEMON_STATE_UNKNOWN;
  daemon_struct->debug = DAEMON_STATE_UNKNOWN;
  daemon_struct->logins = DAEMON_STATE_NO_INFO;
  return(0);
}


/*****************************************************************************/
/* Returns 1 if current host is the master, 0 if not, negative value if error.
 */

int  get_master_server( const char  *db_dir,
                        char        *master_host_name )

{
  int   i;
  int   nHosts;
  int   nReturnCode = ERROR_CANT_DETER_MASTER;
  char   strThisHostName[MAX_HOST_NAME_LEN + 1];
  HostItem  **Hosts;

  /* done only to get rid of compiler warning about the parameter not being
     used. please remove the following if used in the future */
  db_dir=db_dir;

  gethostname(strThisHostName, MAX_HOST_NAME_LEN);
  /*sprintf(master_host_name, "");*/
  master_host_name[0] = 0;
  nReturnCode = ERROR_CANT_DETER_MASTER;
  
  Hosts = getHostDataFromNis(&nHosts);
  if (!Hosts)
    {
      return(nReturnCode);
    }
  
  for (i = 0; i < nHosts; i++)
    {
      if (Hosts[i]->type != NTCSS_MASTER)
        continue;

      sprintf(master_host_name, "%s", Hosts[i]->hostname);
      if (strcmp(master_host_name, strThisHostName) == 0)
        {
          nReturnCode = 1;
        }
      else
        {
          nReturnCode = 0;
        }

      break;
    }             /* END for */

  freeHostData(Hosts, nHosts);
  return(nReturnCode);
}


/*****************************************************************************/

/* Gets status info for each of dest_host's NTCSS daemons and populates the
   server_status structure with it. */
/* This procedure assumes the servers_status pointer is non-NULL */

int  GetAllDaemonsStatusOnHost( const char  *dest_host,
                                HostStats   *servers_status )

{
  int    daemon_port;                    /* port that daemon listens on */
  const char  *fnct_name = "GetAllDaemonsStatusOnHost():";
  int    i;                              /* temp storage */
  char   status_str[DAEMON_STATUS_STR_LEN + 1];
  int    x;                              /* function return values */
  DaemonStats  *daemon_ptr;         /* current daemon struct to fill in */

  sprintf(servers_status->hostname, dest_host);

  /* query the global servers[] array for available servers */
  for(i = 0; i < NUM_NTCSS_SERVER_DAEMONS; i++)
    {
      switch(i)
        {
        case 0:
          {
            daemon_port = SPQ_SVR_PORT;
            daemon_ptr = &servers_status->jobq_server;
            break;
          }
        case 1:
          {
            daemon_port = PRTQ_SVR_PORT;
            daemon_ptr = &servers_status->printq_server;
            break;
          }
        case 2:
          {
            daemon_port = DSK_SVR_PORT;
            daemon_ptr = &servers_status->desktop_server;
            break;
          }
        case 3:
          {
            daemon_port = CMD_SVR_PORT;
            daemon_ptr = &servers_status->command_server;
            break;
          }
        case 4:
          {
            daemon_port = MSG_SVR_PORT;
            daemon_ptr = &servers_status->message_server;
            break;
          }
        case 5:
          {
            daemon_port = DB_SVR_PORT;
            daemon_ptr = &servers_status->db_server;
            break;
          }
        default:
          {
            syslog(LOG_WARNING, "%s Extra server daemon found! Please"
                   " modify this function to recognize this new server!",
                   fnct_name);
           return(1); /* all others should have been retrieved by now. */
          }
        }       /* end of the switch */

      x = GetDaemonStatusStr(dest_host, daemon_port, status_str,
                             DAEMON_STATUS_STR_LEN + 1);
      if (x < 0)
        {
          clear_svr_daemon_struct(daemon_ptr);
          continue;
        }

      ParseIntoDaemonStats(daemon_ptr, status_str);   /* Fill in struct. */
    } /* end of the while loop */
  
  return(0);
}


/*****************************************************************************/

/* Determines if the host name specified is in the HOST_ID file */ 

int  is_host_valid( const char  *name )


{
  FILE  *fp;
  char filename[MAX_FILENAME_LEN];
  char host_entry[MAX_HOST_NAME_LEN+1];  /* entry read from the HOST_ID file */
  char decryptfile[MAX_FILENAME_LEN];

  sprintf(filename, "%s/%s", NTCSS_DB_DIR, "HOST_ID");
  fp = decryptAndOpen(filename, decryptfile, "r");
  if (fp == NULL)
    {
      syslog(LOG_WARNING, "is_host_valid(): Could not open file <%s>!",
             decryptfile);
      return(-1);
    }

  while(fgets(host_entry, MAX_HOST_NAME_LEN, fp) != NULL)
    {
      stripNewline(host_entry); /* strips off nl char on end of each name */
      if (strcmp(name, host_entry) == 0)
        {
          fclose(fp);
          if (decryptfile[0] != '\0')
            remove(decryptfile);
          return(1);
        }
    }
  return(0);
}



/*****************************************************************************/

/* Takes the received mesage from a SERVERSTATUS call and parses the fields
   of information from it, loading the global_stat_ptr structure with
   the information.

   daemon_struct;    Struct for current daemon to fill in
   daemon_status_str;  Status numbers for some daemon.
 */

int  ParseIntoDaemonStats( DaemonStats  *daemon_struct,
                           const char   *daemon_status_str)

{

  const char  *fnct_name = "ParseIntoDaemonStats():";
  int    i;
  int    status_of_debug = DAEMON_STATE_UNKNOWN;
  int    status_of_logins = DAEMON_STATE_UNKNOWN;
  int    status_of_overall = DAEMON_STATE_UNKNOWN;

  i = sscanf(daemon_status_str, "%i %i %i", &status_of_overall,
             &status_of_logins, &status_of_debug);

  if (i < 3)
    {
      syslog(LOG_WARNING, "%s Missing status value(s) for string <%s>!",
             fnct_name, daemon_status_str);
      clear_svr_daemon_struct(daemon_struct);
      return(-1);
    }

  /* Overall daemon status */
  switch(status_of_overall)
    {
    case SERVER_STATE_IDLE:   /* was 0 */
      daemon_struct->overall = DAEMON_STATE_IDLE;
      break;
    case SERVER_STATE_AWAKE:  /* 1 */
      daemon_struct->overall = DAEMON_STATE_AWAKE;
      break;
    default:
      daemon_struct->overall = DAEMON_STATE_UNKNOWN;
      break;
    }

  /* Status of debug */
  switch(status_of_debug)
    {
    case SERVER_STATE_ONDEBUG:  /* 4 */
      daemon_struct->debug = DAEMON_STATE_DEBUG_ON;
      break;
    case SERVER_STATE_OFFDEBUG:  /* 5 */
      daemon_struct->debug = DAEMON_STATE_DEBUG_OFF;
      break;
    default:
      daemon_struct->debug = DAEMON_STATE_UNKNOWN;
      break;
    }

  switch(status_of_logins)
    {
    case SERVER_STATE_ONLOCK: /* 8 */
      daemon_struct->logins = DAEMON_STATE_BLOCKED;
      break;
    case SERVER_STATE_OFFLOCK: /* 9 */
      daemon_struct->logins = DAEMON_STATE_ALLOWED;
      break;
    default:
      daemon_struct->logins = DAEMON_STATE_UNKNOWN;
      break;
    }

  return(0);
}


/*****************************************************************************/
/*
    *master;      Master host to send message to.
    *login_name,  Login_name of poster.
    *app,         App that is sending the message.
    *sys_msg,     Message string to send.
    type;         Message type.
*/

int  post_system_message( const char  *master,
                          const char  *login_name,
                          const char  *app,
                          const char  *sys_msg,
                          int    type )
{
  const char  *fnct_name = "post_system_message():";
  static char  msg[CMD_MESSAGE_LEN+1];
  char  buf[32];
  int   i;
  int   sock;
  int   msg_size;

  i = CMD_LEN + MAX_LOGIN_NAME_LEN + MAX_PROGROUP_TITLE_LEN +
    3 + MAX_SYSTEM_MSG_LEN;
  memset(msg, ' ', i);
  msg[i] = '\0';
  i = CMD_LEN;
  memcpy(&msg[i], login_name, strlen(login_name));

  i += MAX_LOGIN_NAME_LEN;
  memcpy(&msg[i], app, strlen(app));

  i += MAX_PROGROUP_TITLE_LEN;
  sprintf(buf, "%3.3d", type);
  memcpy(&msg[i], buf, 3);

  i += 3;
  memcpy(&msg[i], sys_msg, strlen(sys_msg));

  msg_size = strlen(msg) + 1;
  memset(msg, '\0', CMD_LEN);
  memcpy(msg, "SENDSYSBBMSG", strlen("SENDSYSBBMSG"));

  sock = sock_init(master, MSG_SVR_PORT, 0, SOCK_STREAM);
  if (sock < 0)
    {
      syslog(LOG_WARNING, "%s unable to open socket to the message server "
             "port (%d) on master server %s!", fnct_name, MSG_SVR_PORT,
             master);
      exit(ERROR_CANT_COMM_WITH_MASTER);
    }

  i = write(sock, msg, msg_size);
  close(sock);

  if (i < 0)
    {
      syslog(LOG_WARNING, "%s unable to write to socket on the message server "
             "port (%d) on master server %s!", fnct_name, MSG_SVR_PORT,
             master);
      exit(ERROR_CANT_COMM_WITH_MASTER);
    }
  else  
    return(0);
}


/*****************************************************************************/

/* Broadcasts out_msg to all daemons on each host in the HOST_ID file,
   UNLESS  dont_send_to_master is non-zero - which just skips sending 
   out_msg to the master server.

   dont_send_to_master;   If true, msg isn't sent to the master server.
*/

int send_msg_to_all_hosts( const char  *out_msg,
                           int          out_msg_size,
                           int          dont_send_to_master )
 {
   int  i;
   int return_val = 0;           /* number of errors trying to send messages */
   FILE  *fp;
   char  filename[MAX_FILENAME_LEN];          /* general purpose */
   char  dest_host[MAX_HOST_NAME_LEN+1];      /* destination host's name */
   char  masters_name[MAX_HOST_NAME_LEN+1];
   char  decryptfile[MAX_FILENAME_LEN];

   if (dont_send_to_master)
     get_master_server(NTCSS_DB_DIR, masters_name);
   
   sprintf(filename, "%s/%s", NTCSS_DB_DIR, "HOST_ID");
   fp = decryptAndOpen(filename, decryptfile, "r");
   if (fp == NULL)
     {
       syslog(LOG_WARNING, "send_msg_to_all_hosts(): Could not open file"
              " <%s>!", filename);
       return(-1);
     }
   
   while (fgets(dest_host, MAX_HOST_NAME_LEN, fp) != NULL)
     {
       stripNewline(dest_host);
       
       if (dont_send_to_master)                        /* skip master server */
         if (strcmp(masters_name, dest_host) == 0)
           continue;

       i = send_msg_to_host_daemons(dest_host, out_msg, out_msg_size);
       if (i > 0)
           return_val += i;           /* some probs occured in sending msg */
     }

   if (decryptfile[0] != '\0')
     remove(decryptfile);

   return(return_val);
 }


/*****************************************************************************/

/* Sends a single message to the specified daemon on the specified host */
/* If bytes2read is non-negative, a returned message of size recv_bytes
   is waited for. In the case of 0, it still waits, but the reply is
   discarded.
   It is strongly recommended that a timeout value be specified.

     char *dest_host;   * hostname on which specified daemon is running.
     int  port_id;      * requested port number to use.
     char *out_msg;     * message to be sent to the daemon.
     int  out_msg_size; * # bytes in the out_msg.
     char *recv_msg;    * message received in response to msg sent.
     int  bytes2read;   * size of bytes to read back from the socket.
     int  timeout;      * # secs to wait before giving up on a response.
*/

int  send_msg_to_daemon( const char  *dest_host,
                         int          port_id,
                         const char  *out_msg,
                         int          out_msg_size,
                         char        *recv_msg,
                         int          bytes2read,
                         int          timeout )
{
  int   sock_id;
  int   i;                  /* General purpose. */
  char  dummy_buf[8];       /* Dummy buffer. */
  const char  *fnct_name = "send_msg_to_daemon():";
  
  sock_id = sock_init(dest_host, port_id, 0, SOCK_STREAM);
  if (sock_id < 0)
    {
      syslog(LOG_WARNING, "%s Failed to open socket to %s, port %d!",
             fnct_name, dest_host, port_id);
      return(-1);
    }

  i = write(sock_id, out_msg, out_msg_size);
  if (i < 0)
    {
      close(sock_id);
      syslog(LOG_WARNING, "%s Failed to write to port %d on host %s!",
             fnct_name, port_id, dest_host);
      return(-2);
    }
  

  if (bytes2read < 0)
    { 
      close(sock_id);
      return(0);
    }

  if (bytes2read > 0) {


    if(recv_msg == NULL)  /* make sure of storage space for incoming msg*/
      {
        syslog(LOG_WARNING, "%s No return msg buffer space provided!",
               fnct_name);
        close(sock_id);
        return(-3); 
      }
  }
  
  if (timeout > 0)  /* set timeout alarm if specified for the read */
    {
      if (signal(SIGALRM, NSIsignalAlarm) == SIG_ERROR) /* set timeout handler */
        {
          close(sock_id);
          syslog(LOG_WARNING, "%s Failed to set signal alarm!", fnct_name);
          return(-4);
        }
  
      if (setjmp(NSI_jump_buff) != 0)  /* store the current stack in buffer */
        {
          close(sock_id);
          syslog(LOG_WARNING, "%s Socket may have timed out trying to read!",
                 fnct_name);
          return(-5);
        }
      (void) alarm(timeout);
    }

  if (bytes2read > 0) {
    i = read_sock(sock_id, recv_msg, bytes2read, NO_TIME_OUT, SOCK_STREAM);
  } else {                                    /* Don't care what's returned. */
    i = read_sock(sock_id, dummy_buf, 8, NO_TIME_OUT, SOCK_STREAM);
  }


  if (timeout > 0)                          /* Unset the time-out alarm. */
    (void) alarm(0);
  
  close(sock_id);

  if ((bytes2read > 0) && (i < bytes2read))
    {
      syslog(LOG_WARNING, "%s Only read %d of the required %d bytes from "
             "port %d on host %s!", fnct_name, i, bytes2read, port_id,
             dest_host);
      return(-6); /*  was retval = -1000; */
    }
  return(0);
}


/*****************************************************************************/

/* Sends out_msg to all daemons running on dest_host.  Returned value
   specifies how many problems were encountered sending the messages
   to all the daemons.
*/

int send_msg_to_host_daemons( const char  *dest_host,
                              const char  *out_msg,
                              int          out_msg_size )
{
  int  daemon_port;
  int  i;
  int  num_probs = 0;            /* # of errors encountered sending messages */
  int  x;

  for(i = 0; i < NUM_NTCSS_SERVER_DAEMONS; i++)
    {
      switch(i)
        {
        case 0:
          {
            daemon_port = SPQ_SVR_PORT;
            break;
          }
        case 1:
          {
            daemon_port = PRTQ_SVR_PORT;
            break;
          }
        case 2:
          {
            daemon_port = DSK_SVR_PORT;
            break;
          }
        case 3:
          {
            daemon_port = CMD_SVR_PORT;
            break;
          }
        case 4:
          {
            daemon_port = MSG_SVR_PORT;
            break;
          }
        case 5:
          {
            daemon_port = DB_SVR_PORT;
            break;
          }
        default:
          {
            syslog(LOG_WARNING, "send_msg_to_host_daemons(): Extra server"
                   " daemon found! Please modify this function to recognize"
                   " this new server!");
           return(1); /* all others should have been retrieved by now. */
          }
        } /* end of the switch */

      x = send_msg_to_daemon(dest_host, daemon_port, out_msg,
                             out_msg_size, NULL, -1, /*CMD_LEN + 5,*/ 35);
      if (x < 0)
        {
          num_probs++;
          syslog(LOG_WARNING, "send_msg_to_host_daemons(): Failed to contact"
                 " daemon on port %d, on host %s!", daemon_port, dest_host);
        }
    }
  return(num_probs);
}


/*****************************************************************************/

/* Restores the program stack to when setjmp() was called */

void  NSIsignalAlarm(int signo)
{
    signo=signo;
    longjmp(NSI_jump_buff, 1);
}


/*****************************************************************************/
void NSIPipeSignalAlarm(int  signo)
{
    signo=signo;
    /* do nothing */
    /* return; */
}

/*****************************************************************************/
/* Function to help remove port dependencies.  The svc_name refers to 
   the name given the services in the /etc/services file.  It also needs
   to match the name in the servers_dat.c structures
*/

int  getNtcssPort(svc_name, def_port)

     const char  *svc_name;
     int    def_port;
{
  struct servent  *se;

  if (!strlen(svc_name)) 
    return(def_port);

  se = getservbyname(svc_name, NULL);  /* Null because not tcp/udp comb */
  if (!se)
    return(def_port);
  
  return(htons(se->s_port));
}


/*****************************************************************************/

int  sendCurrentAppDataToDm( const char          *strHostList,
                             ProgroupUseStruct   *aProgroupUseStruct,
                             int                  nMaxArrayItems)
{
  int    nCharIndex;
  int    nProgroupUse;
  int    pfp_in;
  int    pfp_out;
  int    ret;
  const char  *fnct_name = "sendCurrentAppDataToDm():";
  char  *one_in;
  char   strMsg[CMD_LEN], strReply[CMD_LEN];
  char   strProgroup[MAX_PROGROUP_TITLE_LEN];
  char   strProgroupUse[50];
  char   strHostListLen[50];
  char   strResponse[MAX_NUM_PROGROUPS *
                    (MAX_PROGROUP_TITLE_LEN + 10)
                    /* 10 - Separators and number current users */];

  /* done only to get rid of compiler warning about the parameter not being
     used. please remove the following if used in the future */
  strHostList=strHostList;
  
  if (requestAndWaitForDmPipe() != 0)
    {
      syslog(LOG_WARNING, "%s requestAndWaitForDmPipe failed!", fnct_name);
      return(-1);
    }
  
  pfp_out = open(APP_TO_DM_PIPE, O_WRONLY);
  if (pfp_out < 0)
    {
      syslog(LOG_WARNING, "%s open(APP_TO_DM_PIPE) failed!", fnct_name);
      releaseDmPipe();
      return(-2);
    }

  memset(strMsg, '\0', CMD_LEN);
  sprintf(strMsg, "DPCURRENTAPPDATA");
  if (write(pfp_out, strMsg, CMD_LEN) != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s write CMD failed!", fnct_name);
      releaseDmPipe();
      return(-3);
    }

  sprintf(strHostListLen, "%05d", (int)strlen("ALLHOSTS"));
  if ((size_t)write(pfp_out, strHostListLen, strlen(strHostListLen)) !=
      strlen(strHostListLen))
    {
      syslog(LOG_WARNING, "%s write strHostListLen failed!", fnct_name);
      releaseDmPipe();
      return(-4);
    }

  if ((size_t)write(pfp_out, "ALLHOSTS", strlen("ALLHOSTS")) != strlen("ALLHOSTS"))
    {
      syslog(LOG_WARNING, "%s write strHostList failed!", fnct_name);
      releaseDmPipe();
      return(-5);
    }

  memset(strMsg, '\0', CMD_LEN);
  sprintf(strMsg, MT_CURRENTAPPDATA);
  if (write(pfp_out, strMsg, CMD_LEN) != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Write CMD failed!", fnct_name);
      releaseDmPipe();
      return(-6);
    }

  /* Write out trailing NULL */
  if (write(pfp_out, "", sizeof(char)) != 1)
    {
      syslog(LOG_WARNING, "%s Failed to write a trailing NULL!", fnct_name);
      releaseDmPipe();
      return(-7);
    }

  close(pfp_out);

  /* Need to read from dm to app pipe */
  pfp_in = open(DM_TO_APP_PIPE, O_RDONLY);
  if (pfp_in < 0) {
    syslog(LOG_WARNING,"%s Unable to open DM to APP Pipe File!", fnct_name);
    releaseDmPipe();
    return(-8);
  }
  
  ret = read(pfp_in, strReply, CMD_LEN);
  if (ret != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to read reply!", fnct_name);
      releaseDmPipe();
      return(-9);
    }

  strResponse[0] = 1;
  one_in = strResponse;
  ret = read(pfp_in, one_in, 1); 
  if (!ret)
    {
      syslog(LOG_WARNING, "%s Unable to read from in pipe! -- %s", fnct_name,
             strerror(errno));
      close(pfp_in);  
      releaseDmPipe();
      return(-10);
    }

  while(*one_in != '\0')
    {
      one_in++;
      ret = read(pfp_in, one_in, 1);
      
      if (!ret)
        {
          syslog(LOG_WARNING, "%s Unable to read from in pipe! %s", fnct_name,
                 strerror(errno));
          close(pfp_in);     
          releaseDmPipe();
          return(-11);
        }                /* END if */
    }              /* END while */

  releaseDmPipe();
  
  initProgroupUseStruct(aProgroupUseStruct, nMaxArrayItems);
  /* The data will be in the form progroup~nUsers~progroup~nUsers... */
  nCharIndex = parseWord(0,          strResponse, strProgroup,    "~");
  nCharIndex = parseWord(nCharIndex, strResponse, strProgroupUse, "~");
  nProgroupUse = atoi(strProgroupUse);
  while (nCharIndex >= 0)
    {
      addProgroupUseInfo(aProgroupUseStruct, strProgroup, nProgroupUse,
                         nMaxArrayItems);
    
      nCharIndex   = parseWord(nCharIndex, strResponse, strProgroup,    "~");
      nCharIndex   = parseWord(nCharIndex, strResponse, strProgroupUse, "~");
      nProgroupUse = atoi(strProgroupUse);
    }

  return(0);
}


/*****************************************************************************/
/*  General Admin sending and recv routine.
 */

int  sendAdminToDm( const char  *strHostList,
                    const char  *strFileName,
                    char        *strHostResponses,
                    const char  *strMessageType )

{
  char   ch;
  const char  *fnct_name = "sendAdminToDm():";
  char  *one_in;
  char   strHostListLen[50];
  char   strMsg[CMD_LEN];
  char   strReply[CMD_LEN + 1];
  int    fdToDist;
  int    pfp_in;
  int    pfp_out;
  int    ret;
  int    dmpid;

  /* if no DMs just return */
  dmpid=getDmPid();
  if (dmpid <= 0) 
      return 0;

  if (requestAndWaitForDmPipe() != 0)
    {
      syslog(LOG_WARNING, "%s Failed to get access to the DM's pipe!",
             fnct_name);
      return(-1);
    }

  /* Open a pipe to the DM .. */
  pfp_out = open(APP_TO_DM_PIPE, O_WRONLY);
  if (pfp_out < 0)
    {
      syslog(LOG_WARNING, "%s Failed to open %s!", fnct_name, APP_TO_DM_PIPE);
      releaseDmPipe();
      return(-2);
    }
  
  /* Tell the DM what kind of operation has to happen.. */
  memset(strMsg, '\0', CMD_LEN);
  memcpy(strMsg, strMessageType, strlen(strMessageType));
  if (write(pfp_out, strMsg, CMD_LEN) != (ssize_t)CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to send DM the operation type!",
             fnct_name);
      releaseDmPipe();
      return(-3);
    }

  /* Send the length of the host list string to follow next.. */
  sprintf(strHostListLen, "%05d", strlen(strHostList));
  if ((size_t)write(pfp_out, strHostListLen, strlen(strHostListLen)) !=
                                                       strlen(strHostListLen))
    {
      syslog(LOG_WARNING, "%s Failed to send DM the host string's length!",
             fnct_name);
      releaseDmPipe();
      return(-4);
    }

  /* Send the list of target hosts to distribute to .. */
  if ((size_t)write(pfp_out, strHostList,
                    strlen(strHostList)) != strlen(strHostList))
    {
      syslog(LOG_WARNING, "%s Failed to send DM the host list!", fnct_name);
      releaseDmPipe();
      return(-5);
    }

  /* Send the message header to be distributed.. */
   if ((size_t)write(pfp_out, strMsg, CMD_LEN )!= CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Failed to send DM the msg header to distribute!",
             fnct_name);
      releaseDmPipe();
      return(-6);
    }

   /* Open up the filname with the data to be distributed.. */
  fdToDist = open(strFileName, O_RDONLY);
  if (fdToDist < 0)
    {
      syslog(LOG_WARNING, "%s Failed to open %s!", fnct_name, strFileName);
      releaseDmPipe();
      return(-7);
    }

  /* Read in the file character by character, feeding it on to DM.. */
  while (read(fdToDist, &ch, sizeof(char)) == 1)
    {
      if (write(pfp_out, &ch, sizeof(char)) != 1)
        {
          syslog(LOG_WARNING, "%s Failed to finish writing file %s to the DM!",
                 fnct_name, strFileName);
          releaseDmPipe();
          return(-8);
        }
    }       /* END while */

  /* Send a NULL to signal DM that we're finished sending the data to
     distribute..
  */
  if (write(pfp_out, "", sizeof(char)) != 1)
    {
      syslog(LOG_WARNING, "%s Failed to signal the DM about end of data!",
             fnct_name);
      releaseDmPipe();
      return(-9);
    }

  close(pfp_out);

  /* This open will hang until DM opens the other end of the pipe.  If the 
   DM gets a response it does not recognize, it will never open the response
   pipe, leaving us hang.  Hense the alarm.
  */
  signal(SIGALRM, NSIPipeSignalAlarm);
  alarm(MAX_DM_PIPE_WAIT_TIME*1.5);

  /* Read what happened after the distribution..*/
  pfp_in = open(DM_TO_APP_PIPE, O_RDONLY);
  alarm(0);

  if (pfp_in < 0)
    {
      syslog(LOG_WARNING, "%s DM did not open its end of the pipe to relay "
             "status of the distributed data!", fnct_name);
      releaseDmPipe();
      return(-10);
    }

  ret = 0;                        /* Just in case read() doesn't set this.. */
  strReply[CMD_LEN] = '\0'; /* don't want to read off end in case incomplete.*/

  /* Wait for and read the DM's distribution status response msg header.. */
  signal(SIGALRM, NSIPipeSignalAlarm);
  alarm(MAX_DM_PIPE_WAIT_TIME);

  ret = read(pfp_in, strReply, CMD_LEN);

  alarm(0);

  if (ret != CMD_LEN)
    {
      syslog(LOG_WARNING, "%s Only read %i of the expected %i bytes of the "
             "DM's distribution response msg header <%s>! -- %s", fnct_name,
             ret, CMD_LEN, strReply, strerror(errno));
      releaseDmPipe();
      return(-11);
    }

  strHostResponses[0] = (char) 1;               /* set a "no response" flag. */
  one_in = strHostResponses;

  /* Read in the first character of the body of the DM distribution result
     message.. */
  signal(SIGALRM, NSIPipeSignalAlarm);
  alarm(MAX_DM_PIPE_WAIT_TIME);

  ret = read(pfp_in, one_in, 1); 

  alarm(0);

  if (!ret)
    {
      syslog(LOG_WARNING, "%s DM did not send the body after the header for"
             " the distribution response msg! -- %s", fnct_name,
             strerror(errno));
      close(pfp_in);  
      releaseDmPipe();
      return(-12);
    }

  /* Keep reading, character by character, the body of the message.. */
  signal(SIGALRM, NSIPipeSignalAlarm);

  while(*one_in != '\0')
    {
      one_in++;

      /* reset the alarm each time */
      alarm(MAX_DM_PIPE_WAIT_TIME);
      ret = read(pfp_in, one_in, 1);
      alarm(0);
  
      if (ret < 1)
        {
          syslog(LOG_WARNING, "%s Failed to read in all of the DM's response"
                 " message body! -- %s", fnct_name, strerror(errno));
          syslog(LOG_WARNING, "%s DM reports: <%s>", strHostResponses);
          close(pfp_in);     
          releaseDmPipe();
          return(-13);
        }
    }
  
  releaseDmPipe();
  return(0);
}


/*****************************************************************************/

void  nullifyDomain( char *strHostName )

{
  int   i;
  int   nBlankOut = 0;

  for (i = 0; (size_t)i < strlen(strHostName); i++)
    {
      if (strHostName[i] == '.')
        nBlankOut = 1;
      if (nBlankOut)
        strHostName[i] = '\0';
    }
}


/****************************************************************************/

int  sendChangePasswordToHost(strHost, strLoginName, strOldPassword,
                              strNewPassword)
     char  *strHost;
     char  *strLoginName;
     char  *strOldPassword;
     char  *strNewPassword;
{
  /* IMPORTANT NOTE: The reason we use  (2 * MAX_PASSWORD_LEN) instead of
     MAX_PASSWORD_LEN is because the passwords are being sent encrypted, and
     the encryption algorithm can double the length of the data it encrypts. */
#define LOCAL_MSG_LEN CMD_LEN + MAX_LOGIN_NAME_LEN + (2 * MAX_PASSWORD_LEN) + (2 * MAX_PASSWORD_LEN) + 1

  char   strMsg[LOCAL_MSG_LEN];
  char   strReply[CMD_LEN + 1];
  const char  *fnct_name = "sendChangePasswordToHost():";
  int    nReturn;

  memset(strMsg, ' ', LOCAL_MSG_LEN);
  strMsg[LOCAL_MSG_LEN - 1] = '\0';
  memset(strMsg, '\0', CMD_LEN);
  sprintf(strMsg, "CHANGEUSERPASSWD");
  strncpy(strMsg + CMD_LEN, strLoginName, strlen(strLoginName));
  strncpy(strMsg + CMD_LEN + MAX_LOGIN_NAME_LEN, strOldPassword,
          strlen(strOldPassword));
  strncpy(strMsg + CMD_LEN + MAX_LOGIN_NAME_LEN + (2 * MAX_PASSWORD_LEN),
          strNewPassword, strlen(strNewPassword));
  
  /*
    strcat(strMsg + CMD_LEN, strLoginName);
    strcat(strMsg + CMD_LEN + MAX_LOGIN_NAME_LEN, strOldPassword);
    strcat(strMsg + CMD_LEN + MAX_LOGIN_NAME_LEN + MAX_PASSWORD_LEN,
           strNewPassword);
  */
  memset(strReply, '\0', CMD_LEN + 1);
  nReturn = send_msg_to_daemon(strHost, DB_SVR_PORT, strMsg, LOCAL_MSG_LEN,
                               strReply, CMD_LEN + 1, 15);
  if (nReturn != 0)
    {
      return(-1);
    }
  
  if (strcmp(strReply, "GOODPASSWDCHANGE") != 0)
    {
      syslog(LOG_WARNING, "%s Non success reply(%s) from host %s!",
             fnct_name, strReply, strHost);
      return(-2);
    }

  return(0);

}

/*****************************************************************************/
/* Returns the number representing the particular daemon's status. See
   NetSvrInfo.h for the response interpretations.
*/

int  GetDaemonStatusStr( const char  *dest_host,
                         int          daemon_port,
                         char        *return_buff,
                         int          buff_len )

{
  const char  *fnct_name = "GetDaemonStatusStr():";
  char   out_msg[CMD_LEN + 20];  /* Outgoing msg to remote daemon. */
  char   recv_msg[CMD_MESSAGE_LEN + 20]; /* Feedback from remote daemon. */
  int    status_debug;
  int    status_logins;
  int    status_overall;
  int    x;                             /* Function return values. */

  return_buff[0] = '\0';

  memset(out_msg, '\0', CMD_LEN + 20);
  memcpy(out_msg, "SERVERSTATUS", strlen("SERVERSTATUS"));
  sprintf(&out_msg[CMD_LEN], "%1i", SERVER_STATE_CHECK);

  memset(recv_msg, '\0', CMD_MESSAGE_LEN + 1);

  /* Reading less than specified response bytes will cause a negative value
     to be returned, so we estimate the bare minimum to be 6: 3 bundled, single
     digit numbers. */
  x = send_msg_to_daemon(dest_host, daemon_port, out_msg,
                         sizeof(out_msg), recv_msg, CMD_LEN + 6,  5);

  if (x < 0) /* error talking to this daemon */
    return(-1);

  if(recv_msg[CMD_LEN] == '\0')
    {
      syslog(LOG_WARNING, "%s No status info returned in msg!", fnct_name);
      return(-2);
    }

  x = unbundleData(&recv_msg[CMD_LEN], "III", &status_overall, &status_logins,
                    &status_debug);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Could not unbundle response message <%s>!",
             fnct_name, &recv_msg[CMD_LEN]);
      return(-3);
    }

  /* Using the receive var for a temp buffer.. */
  sprintf(recv_msg, "%i %i %i", status_overall, status_logins, status_debug);

  strncpy(return_buff, recv_msg, buff_len);
  return_buff[buff_len - 1] = '\0';           /* Just in case it's too long! */
  return(0);
}


/*****************************************************************************/
/* Checks to see if the specified daemon is idled. If so, the response
   will be greater than zero.  Host can optionally be NULL, which causes the
   function to get the status of the local daemons instead of a remote host's.
*/

int  IsDaemonIdledOnHost( const char  *host,
                          int          daemon_port )

{
  const char  *fnct_name = "IsDaemonIdledOnHost():";
  int    i;
  char   local_hostname[MAX_HOST_NAME_LEN + 1];
  int    overall_daemon_status = DAEMON_STATE_UNKNOWN;
  const char  *ptr_hostname;
  char   daemon_status_str[DAEMON_STATUS_STR_LEN + 1];

  if (host == NULL)
    {
      if (gethostname(local_hostname, MAX_HOST_NAME_LEN) != 0) 
        {
          syslog(LOG_WARNING, "%s Cannot get the name of this host!",
                 fnct_name);
          return(-1);
        }
      ptr_hostname = local_hostname;
    }
  else
    ptr_hostname = host;

  i = GetDaemonStatusStr(ptr_hostname, daemon_port, daemon_status_str,
                         DAEMON_STATUS_STR_LEN + 1);
  if (i < 0)
    return(-2);

  /* The status we want should be the first number in the string.. */
  i = sscanf(daemon_status_str, "%i", &overall_daemon_status);
  if (i < 1)
    {
      syslog(LOG_WARNING, "%s Missing status value(s) for string <%s>!",
             fnct_name, daemon_status_str);
      return(-3);
    }

  if (overall_daemon_status == SERVER_STATE_IDLE)
    return(1);

  return(0);
}


/*****************************************************************************/
/* Checks to see if the specified daemon is awake, NOT IDLED. If so, the
   response
   will be greater than zero.  Host can optionally be NULL, which causes the
   function to get the status of the local daemons instead of a remote host's.
*/

int  IsDaemonAwakeOnHost( const char  *host,
                          int          daemon_port)

{

  const char  *fnct_name = "IsDaemonRunningOnHost():";
  int    i;
  char   local_hostname[MAX_HOST_NAME_LEN + 1];
  int    overall_daemon_status = DAEMON_STATE_UNKNOWN;
  const char  *ptr_hostname;
  char   daemon_status_str[DAEMON_STATUS_STR_LEN + 1];

  if (host == NULL)
    {
      if (gethostname(local_hostname, MAX_HOST_NAME_LEN) != 0) 
        {
          syslog(LOG_WARNING, "%s Cannot get the name of this host!",
                 fnct_name);
          return(-1);
        }
      ptr_hostname = local_hostname;
    }
  else
    ptr_hostname = host;

  i = GetDaemonStatusStr(ptr_hostname, daemon_port, daemon_status_str,
                         DAEMON_STATUS_STR_LEN + 1);
  if (i < 0)
    return(-2);

  /* The status we want should be the first number in the string.. */
  i = sscanf(daemon_status_str, "%i", &overall_daemon_status);
  if (i < 1)
    {
      syslog(LOG_WARNING, "%s Missing status value(s) for string <%s>!",
             fnct_name, daemon_status_str);
      return(-3);
    }

  if (overall_daemon_status == SERVER_STATE_AWAKE)
    return(1);

  return(0);
}


/*****************************************************************************/
/* Checks to see if the specified daemon is awake OR just idled. If so, the
   response
   will be greater than zero.  Host can optionally be NULL, which causes the
   function to get the status of the local daemons instead of a remote host's.
*/

int  IsdaemonRunningOnHost( const char  *host,
                            int          daemon_port )

{

  const char  *fnct_name = "IsdaemonRunningOnHost():";
  int    i;
  char   local_hostname[MAX_HOST_NAME_LEN + 1];
  int    overall_daemon_status = DAEMON_STATE_UNKNOWN;
  const char  *ptr_hostname;
  char   daemon_status_str[DAEMON_STATUS_STR_LEN + 1];

  if (host == NULL)
    {
      if (gethostname(local_hostname, MAX_HOST_NAME_LEN) != 0) 
        {
          syslog(LOG_WARNING, "%s Cannot get the name of this host!",
                 fnct_name);
          return(-1);
        }
      ptr_hostname = local_hostname;
    }
  else
    ptr_hostname = host;

  i = GetDaemonStatusStr(ptr_hostname, daemon_port, daemon_status_str,
                         DAEMON_STATUS_STR_LEN + 1);
  if (i < 0)
    return(-2);

  /* The status we want should be the first number in the string.. */
  i = sscanf(daemon_status_str, "%i", &overall_daemon_status);
  if (i < 1)
    {
      syslog(LOG_WARNING, "%s Missing status value(s) for string <%s>!",
             fnct_name, daemon_status_str);
      return(-3);
    }

  if ( (overall_daemon_status == SERVER_STATE_AWAKE) ||
       (overall_daemon_status == SERVER_STATE_IDLE) )
    return(1);

  return(0);
}

/*****************************************************************************/
/* Checks to see if the database daemon is idled. If so, the response
   will be greater than zero.
*/

int  IsDBdaemonIdledOnHost( const char  *host )

{

  int     daemon_port;
  /*char   *fnct_name = "IsDBdaemonIdledOnHost():";*/
  int     i;

  daemon_port = getNtcssPort("database_server", DB_SVR_PORT);

  i = IsDaemonIdledOnHost(host, daemon_port);
  if(i < 0)
    return(-1);

  return(i);
}

/*****************************************************************************/
/* Checks to see if the database daemon is running (either awake or idled).
   If so, the response will be greater than zero.
*/

int  IsDBdaemonRunningOnHost( const char  *host )

{

  int     daemon_port;
  /*char   *fnct_name = "IsDBdaemonRunningOnHost():";*/
  int     i;

  daemon_port = getNtcssPort("database_server", DB_SVR_PORT);

  i = IsdaemonRunningOnHost(host, daemon_port);
  if(i < 0)
    return(-1);

  return(i);
}

/*****************************************************************************/
/* Resorts to using a ps command to determine if copies of gen_se are running.
   This is useful for finding stranded/orphaned daemons.
 */

int  GrepNumGensRunning()

{
#define MAX_LINE_SIZE 750
  FILE *fp;
  char strLine[MAX_LINE_SIZE];
  int  nGens = 0;

  fp = popen("ps -ef | grep gen_se | egrep -v grep", "r");
  if (fp == NULL)
    {
      return(-1);
    }

  while (fgets(strLine, MAX_LINE_SIZE, fp) != NULL)
    {
      nGens++;
    }
  
  fclose(fp);

  if (nGens > 0)
    {
      return(1);
    }
  else
    {
      return(0);
    }
}

/*****************************************************************************/
/*  
   Returns the current debugging state for the specified daemon on the 
   specified host. If an error occurs, -1 is returned.
*/
int getDebugState(char* host, int daemon_port)
{
   int debugState = -1;
   char   status_str[DAEMON_STATUS_STR_LEN + 1];
   DaemonStats* daemon_stats;

   if (GetDaemonStatusStr(host, daemon_port, status_str,
                          DAEMON_STATUS_STR_LEN + 1) >= 0) {
      daemon_stats = (DaemonStats*)malloc(sizeof(DaemonStats));
      if (ParseIntoDaemonStats(daemon_stats, status_str) == 0)
         debugState = daemon_stats->debug;
      free(daemon_stats);
   }

   return debugState;
}

/*****************************************************************************/
/*****************************************************************************/
