
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/**************************** NTCSS I  *****************************/
/**                                                               **/
/**  Developed by Inter-National Research Institute, Inc. under   **/
/**  contract to the Department of Defense.  All U.S. Government  **/
/**  restrictions on software distributions apply.                **/
/**                                                               **/
/**  Copyright 1994, Inter-National Research Institute            **/
/**  All rights reserved.                                         **/
/**                                                               **/
/*******************************************************************/


/*********************************************************************/
/* Program: Network Copy Command, Anthony C. Amburn                  */
/* Descrip: This program initiates a file transfer across the network*/
/*          while not processing any environmental issues as does the*/
/*          rcp command. It will still check user and group access.  */
/*                                                                   */
/* Modifcation History:                                              */
/* Date      Name                 Description                        */
/*                                                                   */
/* 11/19/93  Anthony C. Amburn    Original Coding                    */
/* 02/11/98  Denton W. Byers      Added syslog error statements      */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>



#include <NetCopy.h>
#include <SockFcts.h>
#include <ExtraProtos.h>
#include <Ntcss.h>


extern int  ReadGenericPacket(int, char*, int, int);
extern int  WriteGenericPacket(int, char*, int);

int  get_pack(int, struct DBUF*, int);
int  nc_fsm_write(char*, char*, int);
int  net_copy(const char*, const char*, const char*, const char*, int);
int  pack_send(struct DBUF*, int, int);
int  send_cmd(char, char*);
void syserr(const char*);
int nc_fsm_read(char*, char*, int);

Header  hdblock;
struct stat  s;

/* FILE  *fp; */
FILE  *fp1;
int  sock;
int  sock_resp;
char  debug_flag = 0;
char  hst[HOSTNAMELEN];
char  lcl_hst[HOSTNAMELEN];
mode_t  file_mode;

/*************** syserr ******************************************************/

void syserr(err)

     const char *err;
{
  syslog(LOG_WARNING, "net_copy:%s\n", err);
}


/* invocation: net_xfer get|put hostname sfile [dfile] */

/************************ net_copy *******************************************/

int net_copy(opptr, hnptr, srcptr, fnptr, file_type)

     const char *opptr;   /* operation pointer (get or put) */
     const char *hnptr;   /* hostname pointer */
     const char *srcptr;  /* source file pointer */
     const char *fnptr;   /* destination file pointer */
     int   file_type; /* XFER_ASCII or XFER_BINARY */

{
  char  dest_file[HOSTNAMELEN];
  char  file_name[HOSTNAMELEN];
  /* char  i; */
  int i;
  char  rdwr;
  char  src_file[HOSTNAMELEN];
  int  port, fixedport;
  struct servent  *svc_ptr;


/*
  if (debug_flag)
    {
      syslog(LOG_WARNING, "ARGC=%d\n", argc);
      for (i = 0; i < argc; i++) printf("%s ", argv[i]);
      printf("\n");
    }

  if (argc < 3)
    {
      syslog(LOG_WARNING, "Format: net_xfer get|put hostname file_name\n");
      return(-1);
    }
*/
  svc_ptr = getservbyname("netxfer", "tcp");
  if (svc_ptr == NULL) 
    {
      port = DEFAULT_NETXFER_PORT;
      syslog(LOG_WARNING, "Unable to obtain netxfer port info"
             " from system. Using %d.", port);
    }
  else {
    fixedport = svc_ptr->s_port;
    port=htons(svc_ptr->s_port);
  }

  memset(hst, NTCSS_NULLCH, HOSTNAMELEN);
  strcpy(hst, hnptr);

  if (!strcmp(opptr, "put") || !strcmp(opptr, "PUT"))
    rdwr = WRITE_NC;
  else
    if (!strcmp(opptr, "get") || !strcmp(opptr, "GET"))
      rdwr = READ_NC; 
    else 
      { 
        syslog(LOG_WARNING, "net_xfer: Illegal option.\n");
        syslog(LOG_WARNING, "        Format: net_xfer get|put hostname"
               "file_name\n");
        return(-1);
      }


  if ((sock_resp = sock_init(hst, port, CLIENT, SOCK_STREAM)) < 0)
    {
      syslog(LOG_WARNING,"NETCOPY: unable to start network read\n");
      return(-2);
    }


  strcpy(src_file, srcptr);
  strcpy(dest_file, fnptr);
  strcpy(file_name, fnptr);

  gethostname(lcl_hst, HOSTNAMELEN);
  if (rdwr == WRITE_NC)
    i=nc_fsm_write(src_file, dest_file, file_type);
  else
    i=nc_fsm_read(src_file, dest_file, file_type);
  
  close(sock_resp);
  return(i);
}


/************************* nc_fsm_write **************************************/

int nc_fsm_write(srcfile, destfile, is_ascii)  /* binary for last param -db*/

     char  *srcfile;
     char  *destfile;
     int   is_ascii;

{

  FILE *fp;
  int  done;
  int  done_next;
  int  mlen;
  int  numRead = 0;
  int  rc;
  int  state;

  state = INIT_WRITE;

  rc = 0; 
  done = 0;
  done_next = 0;
  fp = NULL;

  while(!done)
    {
      switch(state)
        {
        case INIT_WRITE:
          if (!stat(srcfile, &s) && S_ISREG(s.st_mode))
            {
              state = OPEN_LOCAL;
              file_mode = (s.st_mode & 07777);
            }
          else
            {
              state = ERROR;
              syslog(LOG_WARNING,
                     "net_copy: File not found, or not a file: %s", srcfile);
            }
          break;
          
        case OPEN_LOCAL:
          if ((fp = fopen(srcfile, is_ascii == XFER_ASCII ? "r" : "rb"))
              == NULL)
            {
              syslog(LOG_WARNING, "net_copy cannot open file %s", srcfile);
              state = ERROR;
            }
          else 
            state = OPEN_REMOTE;
          break;
          
        case OPEN_REMOTE:
          if ((send_cmd(OPEN_NC, destfile)))
            {
              syslog(LOG_WARNING, "net_copy unable to open file %s on"
                     " remote host!", destfile);
              state = ERROR;
            }
          else 
            state = GET_RESP;
          break;
          
        case GET_RESP:
          if (debug_flag)
            syslog(LOG_WARNING,"State GET_RESP:");
          
          if ((mlen = get_pack(sock_resp, &hdblock.dbuf, HDRSZ)) == -1)
            {
              syserr("net_copy error reading socket data for response!");
              state = ERROR;
            }
          else
            {
              if (hdblock.dbuf.cmd == OK_PACK)
                {
                  if (debug_flag)
                    syslog(LOG_WARNING, "State GET_RESP: Got OK");
                  if (done_next)
                    state = DONE;
                  else
                    state = READ_LOCAL;
                }
              else
                {
                  syslog(LOG_WARNING, "Error opening remote file, %d\n",
                         hdblock.dbuf.len);
                  state = CLOSE;
                }
            }
          break;
          
        case READ_LOCAL:
          if (debug_flag)
            syslog(LOG_WARNING,"State READ_LOCAL:");
          
          if (feof(fp))
            state = CLOSE;
          else
            {
              if (debug_flag)
                syslog(LOG_WARNING, "State READ_LOCAL:Reading File");
              numRead = fread(hdblock.dbuf.buf, 1, BUF_SIZE, fp);
              if (numRead == 0)
                {
                  syslog(LOG_WARNING,
                         "net_copy error reading file desc. %d", fp);
                  state = CLOSE;
                }
              else 
                state = WRITE_REMOTE;
            }
          break;
          
        case WRITE_REMOTE:
          if (debug_flag)
            syslog(LOG_WARNING, "State WRITE_REMOTE:");
          hdblock.dbuf.cmd = WRITE_NC;
          hdblock.dbuf.len = numRead;
          strcpy(hdblock.dbuf.hostname, lcl_hst);
          /*if ((pack_send(hdblock.dummy, HDRSZ, HOST_PORT_NUM)))*/
          if ((pack_send(&hdblock.dbuf, HDRSZ, HOST_PORT_NUM)))
            {
              syserr("net_copy error writing remote file!");
              state = ERROR;
            }
          else
            state = GET_RESP;
          break;
          
        case CLOSE:
          if (fp)
            fclose(fp);
          fp = NULL;
          hdblock.dbuf.len = 0;
          send_cmd(CLOSE_NC, destfile);
          state = GET_RESP;
          done_next++;
          break;
          
        case DONE:
          done++;
          break;
          
        case ERROR: 
          if (fp)
            fclose(fp);
          rc = 1;
          done++;
          break;
          
        default:
          syserr("Error, I'm in an unknown state, help!\n");
          done++;
          return(-1);
          break;
        }
    }
  return(rc);
}


/***************** nc_fsm_read ***********************************************/

int nc_fsm_read(srcfile, destfile, is_ascii)

     char  *srcfile;
     char  *destfile;
     int   is_ascii;

{
  int  ctl = 2334; /* a random number just to initialize it with something */
  int  done;
  int  mlen;
  int  state;
  FILE *fp = 0;
  int  rc;

    done=0;
    state=INIT_READ;
    rc = 0;

    while (!done)
      {
        switch(state)
          {
          case INIT_READ:
            state = GET_RESP;
            strcpy(hdblock.dbuf.file_name, srcfile);
            if (send_cmd(OPEN_READ_NC, srcfile)) 
              state = ERROR;
            ctl = 0;
            break; 

          case GET_RESP: 
            if ((mlen = get_pack(sock_resp, &hdblock.dbuf, HDRSZ)) == -1)
              {
                syserr("Error reading socket data for response.\n");
                state = ERROR;
              }
            else
              {
                if (!(ctl & OPEN_S)) 
                  state = OPEN_LOCAL;
                else
                  state = WRITE_LOCAL;
              }
            break;

          case OPEN_LOCAL:
            if (hdblock.dbuf.cmd == NOK_PACK)
              {
                syslog(LOG_WARNING,"net_xfer: error opening remote file"
                       " for read, %d.\n", hdblock.dbuf.len);
                state = ERROR;
              }
            else
              {
                if (!stat(destfile,&s))
                  {
                    if (S_ISREG(s.st_mode))
                        remove(destfile);
                    else
                      {
                        syserr("net_xfer: Permissions Denied or invalide"
                               " file type.\n");
                        state = ERROR;
                      }
                  }
                setgid(hdblock.dbuf.gid);
                setuid(hdblock.dbuf.uid);
                file_mode = hdblock.dbuf.file_mode;
                if ((fp = fopen(destfile,
                                is_ascii == XFER_ASCII ? "w" : "w+b")) == NULL)
                  {
                    syslog(LOG_WARNING,"net_xfer: Error, unable to open"
                           " file for write.\n");
                    state = ERROR;
                  }
                else
                  {
                    ctl |= OPEN_S;
                    send_cmd(READ_NC, srcfile);
                    state = GET_RESP; 
                  }
              }
            break;

          case WRITE_LOCAL:
            if (!(ctl & OPEN_S))
              {
                syslog(LOG_WARNING, "Error, cannot write to an un-opend"
                       " file, %02X.\n", ctl);
                send_cmd(NOK_PACK, srcfile);
              }
            else
              {
                switch(hdblock.dbuf.cmd)
                  {
                  case OK_PACK:
                    if (hdblock.dbuf.len && 
                        (fwrite(hdblock.dbuf.buf, 1, hdblock.dbuf.len, fp)) != 
                        (size_t)hdblock.dbuf.len)
                      {
                        send_cmd(CLOSE_READ_NC,srcfile);
                        state = ERROR;
                      }
                    else
                      {
                        state = GET_RESP;
                        send_cmd(READ_NC, srcfile);
                      }
                    break;
                  case NOK_PACK:
                    syslog(LOG_WARNING, "net_xfer: remote indicates error.\n");
                    state = ERROR;
                    break;
                  case EOF_PACK:
                    if (hdblock.dbuf.len && 
                        (fwrite(hdblock.dbuf.buf,1,hdblock.dbuf.len,fp)) !=
                        (size_t)hdblock.dbuf.len)
                      state = ERROR;
                    else
                      state = CLOSE_LOCAL;
                    break;
                  default:
                    syslog(LOG_WARNING, "net_xfer: unknown command from"
                           " remote.\n");
                    state = ERROR;
                    break;
                  }
              }
            break;
    
          case CLOSE_LOCAL:
            fclose(fp);
            chmod(destfile, file_mode);
            ctl = 0;
            done++;
            break;
    
          case ERROR:
            if (ctl & OPEN_S)
                fclose(fp);
            done++;
            rc = 1;
            break;

          default:
            syslog(LOG_WARNING, "Unknown command, somebody help me!");
            rc = -1;
            done++;
            break;
          }
        
    }

    return (rc);

}


/********************* pack_send *********************************************/

int pack_send(addr, size, port)

     struct DBUF  *addr;
     int  size;
     int  port;

{
  int  rc;
  int  ttotal;
  char  *tptr;
  struct DBUF_IN  dout;

  /* gets rid of compiler warning about unused parameters */
  size=size;
  port=port;

  rc = 0;
  
  DBUF_to_text(addr, &dout);     
  if (debug_flag)
    {
      syslog(LOG_WARNING, "Sending Packet with following info...");
      syslog(LOG_WARNING, "cmd = %.9s, gid = %.9s, uid = %.9s",
         dout.cmd, dout.gid, dout.uid);
      syslog(LOG_WARNING, "filename = %s, len = %.9s",
         dout.file_name, dout.len);
      syslog(LOG_WARNING, "fmode = %.9s, host = %.30s",
         dout.file_mode, dout.hostname);
    }
  tptr = (char *)&dout;
  ttotal = WriteGenericPacket(sock_resp, tptr, 250);
  
    /*
    rc=0;
    tptr=(char *)&dout;
    ttotal=0;
    tsize=0;
    do {
        if (write(sock_resp,tptr,1) < 0) {
            syslog(LOG_WARNING,"ERROR: unable to send message- %d\n",errno);
            rc=1;
            break;
        }
        tptr++;
        ttotal++;
    } while (ttotal < 250);
    */

    if (debug_flag)
        syslog(LOG_WARNING, "Command part Done.");

    if (addr->len > 0) {
        ttotal = WriteGenericPacket(sock_resp, addr->buf, addr->len);
        if (debug_flag)
            syslog(LOG_WARNING, "Data part Done. Wrote %d",
                   ttotal);
    /*
        rc=0;
        tptr=(char *)addr->buf;
        ttotal=0;
        tsize=0;
        do {
            tsize=addr->len - ttotal;
            if ((tsize=write(sock_resp,tptr,addr,tsize)) < 0) {
                syslog(LOG_WARNING,"ERROR: unable to send message- %d\n",errno);
                rc=1;
                break;
            }
            tptr++;
            ttotal+=tsize;
         } while (ttotal < addr->len);
         */

    }
    
    return(rc);
}


/***************** send_cmd **************************************************/

int send_cmd(char cmd, char* fn)
{
  int  rc = 0;

  memset(hdblock.dummy, NTCSS_NULLCH, HDRSZ);

  /* WARNING: Make sure gid is done first. */
  strcpy(hdblock.dbuf.hostname, lcl_hst);
  hdblock.dbuf.gid = getegid();
  hdblock.dbuf.uid = geteuid();
  hdblock.dbuf.len = 0;
  hdblock.dbuf.file_mode = file_mode;
    
  strcpy(hdblock.dbuf.file_name, fn);
  hdblock.dbuf.cmd = cmd;
    
  if (debug_flag)
    {
      syslog(LOG_WARNING, "GID = %d, UID = %d, LEN = %d, HN = %s",
             hdblock.dbuf.gid, hdblock.dbuf.uid, hdblock.dbuf.len,
             hdblock.dbuf.hostname);
      syslog(LOG_WARNING, "FN = %s", hdblock.dbuf.file_name);
    }

    /*if ((pack_send(hdblock.dummy, HDRSZ, HOST_PORT_NUM)))*/
    if ((pack_send(&hdblock.dbuf, HDRSZ, HOST_PORT_NUM)))
        rc = 1;

    return(rc);
}


/*
int recv_to_alarm()
{
    syslog(LOG_WARNING,"NET_COPY:Receive TimeOut.");
    return;
}
*/


/***************** get_pack **************************************************/

int  get_pack(sptr, bptr, size)

     int  sptr;
     struct DBUF  *bptr;
     int  size;

{
  char *ptr;
  int  ttotal;
  struct DBUF_IN  din;

  /* gets rid of compiler warning about unused parameters */
  size=size;

  if (debug_flag)
      syslog(LOG_WARNING, "Reading %d.", sizeof(struct DBUF_IN));

  ptr = (char *)&din;
    /*
    ttotal=0;
    do {
        signal(SIGALRM,recv_to_alarm);
        alarm(RECV_TO_VALUE);
        if ((read(sptr,ptr,1)) != 1) {
            return(-1);
        }
        alarm(0);
        ttotal += 1;
        ptr++;
    } while (ttotal < 250);
    */
    ttotal = ReadGenericPacket(sptr, ptr, 250, 8192);

    DBUF_from_text(&din, bptr);

    if (bptr->len > 0)
      {

        ttotal = ReadGenericPacket(sptr, bptr->buf, bptr->len, 8192);
/*
        ptr=(char *)bptr->buf;
        ttotal=0;
        do {
            if (bptr->len - ttotal >= 4096)
                tsize = 4096;
            else
                tsize = bptr->len - ttotal;

            signal(SIGALRM,recv_to_alarm);
            alarm(RECV_TO_VALUE);
            if ((tsize=read(sptr,ptr,tsize)) <= 0) {
                    return(-1);
            }
            alarm(0);
            ttotal += tsize;
            ptr += tsize;
        } while (ttotal < bptr->len);
    */
      }

    if (debug_flag)
      {
        syslog(LOG_WARNING, "Recieved Packet data (%d) with following info...",
               ttotal);
        syslog(LOG_WARNING, "cmd = %d, gid = %d, uid = %d, fmode = %d",
               bptr->cmd, bptr->gid, bptr->uid, bptr->file_mode);
        syslog(LOG_WARNING, "host = %s, len = %d",
               bptr->hostname, bptr->len);
        syslog(LOG_WARNING, "filename = %s", bptr->file_name);
      }
    return(ttotal);
}


/*****************************************************************************/
