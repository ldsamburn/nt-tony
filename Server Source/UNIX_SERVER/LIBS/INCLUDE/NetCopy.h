
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * Netcopy.h
 */

#ifndef _DEFINE_NETCOPY_H
#define _DEFINE_NETCOPY_H

#include <sys/types.h>  /* For mode_t */


#define OPENLOG(x,y,z)  openlog(x,y,z)
#define SECLOG_PRI      LOG_NOTICE|LOG_AUTH


/* BOOLS */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* General / Global defines */

#define 	HOSTNAMELEN		100
#define		HOST_PORT_NUM		1925
#define		RESP_PORT_NUM		1926
#define		PWD_PORT_NUM		1927
#define		PWD_RESP_PORT_NUM	1928

#define 	DEFAULT_NETXFER_PORT	1926
#define 	RECV_TO_VALUE		5

#define		BUF_SIZE		262144
#define		HDRSZ			sizeof(struct DBUF)
#define		CLIENT			0
#define		SERVER			1
#define		SET_TIMER		1
#define		NO_TIMER		0

/* Defines for primitive file ops */

#define		OPEN_NC     		0
#define		CLOSE_NC		1 
#define		OPEN_READ_NC		2
#define		WRITE_NC		3
#define		READ_NC			4
#define		CLOSE_READ_NC		5
#define 	UPDATE_FILE		6
#define		SET_PWENT		7
#define 	OPEN_NC_APP		8

/* Responsese */

#define 	OK_PACK			0
#define		NOK_PACK		1
#define		EOF_PACK		2

/* State defs */

#define 	INIT_WRITE		0
#define		OPEN_LOCAL		1
#define		OPEN_REMOTE		2
#define		GET_RESP		3
#define 	READ_LOCAL		4
#define		WRITE_REMOTE	5
#define		CLOSE			6
#define		DONE			7

#define 	INIT_READ		8
#define		READ_REMOTE		10
#define		WRITE_LOCAL		11
#define		CLOSE_LOCAL		12 
#define		INIT_PWD		13
#define		SEND_PWENT		14

#define		ERROR			99


/* Server control masks */

#define		OPEN_S			0x01
#define		WRITE_S			0x02
#define		CLOSE_S			0x04

#define		SECURE_PWFILE		"/.secure/etc/passwd"
#define		TMP_FILE			"/tmp/exfer"

#define 	XFER_ASCII		0
#define		XFER_BINARY		1

/* Data and command structer definition */

struct DBUF {
	char  cmd;
	char  file_name[100];
	gid_t gid;
	gid_t uid;
	mode_t file_mode;
	char  hostname[100];
	int   len;
	char  buf[BUF_SIZE];
};

struct DBUF_IN {
	char  cmd[10];
	char  file_name[100];
	char  gid[10];
	char  uid[10];
	char  file_mode[10];
	char  hostname[100];
	char  len[10];
	char  buf[BUF_SIZE];
};
#define DBUF_SIZE sizeof(struct DBUF_IN)

struct SPWD {
	char name[100];
	char pwd[100];
	char age[10];
	int  audid;
	int  audflg;
};
	

union header {
	char dummy[HDRSZ];
	struct DBUF dbuf;
};

union sheader {
	char dummy[226];
	struct SPWD sbuf;
};

typedef union sheader SHeader;
typedef union header Header;

int  DBUF_from_text(struct DBUF_IN *, struct DBUF * );
int  DBUF_to_text(struct DBUF * , struct DBUF_IN * );


#endif
