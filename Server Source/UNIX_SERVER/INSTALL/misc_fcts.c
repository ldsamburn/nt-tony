
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * misc_fcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <search.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <SockFcts.h>
#include <ExtraProtos.h>

/** LOCAL INCLUDES **/

#include "install.h"


/** FUNCTION PROTOTYPES **/

static void signal_alarm(int);
void input_item(const char*, char*);
int copy_app_default_files(char*);
int is_host_master(char*);
int are_any_servers_running(void);


/** STATIC VARIABLES **/

static jmp_buf env_alrm;


/** FUNCTION DEFINITIONS **/

static void signal_alarm(int signo)
{
    signo=signo;
    longjmp(env_alrm, 1);
}



void input_item(str, value)
const char *str;
char *value;
{
	printf("\n%s\n", str);
	gets(value);
}



int copy_app_default_files(dir)
char *dir;
{
	FILE *rp, *wp;
	DIR *dp;
#ifdef SOLARIS
	struct dirent *direc;
#else
	struct direct *direc;
#endif
	char buf[MAX_FILENAME_LEN], app[MAX_FILENAME_LEN];
	char str[MAX_APPLINE_LEN], line[MAX_APPLINE_LEN];
	struct stat sbuf;

	if ((dp = opendir(dir)) == NULL) {
		syslog(LOG_WARNING, "Cannot open directory %s.", dir);
		return(ERROR_CANT_OPEN_FILE);
	}

	while ((direc = readdir(dp)) != NULL) {
		if (direc->d_ino == 0)
			continue;
		sprintf(buf, "%s/%s", dir, direc->d_name);
		sprintf(app, "%s/%s", APP_DEFAULTS_DIR, direc->d_name);
		if (stat(buf, &sbuf) == -1) {
			syslog(LOG_WARNING, "Cannot get status of file %s.", buf);
			continue;
		}
		if ((sbuf.st_mode & S_IFMT) == S_IFREG) {
			if ((rp = fopen(buf, "r")) == NULL) {
				syslog(LOG_WARNING, "Cannot open file %s.", buf);
				continue;
			}
			if ((wp = fopen(app, "w")) == NULL) {
				fclose(rp);
				syslog(LOG_WARNING, "Cannot open file %s.", app);
				continue;
			}
			while (fgets(str, MAX_APPLINE_LEN, rp) != NULL) {
				replacePattern("TOPLEVELDIR", TOP_LEVEL_DIR, str, line);
				fprintf(wp, "%s", line);
			}
			fclose(rp);
			fclose(wp);
		}
	}
	closedir(dp);

	return(0);
}



int is_host_master(db_dir)
char *db_dir;
{
	char filename[MAX_FILENAME_LEN], hostname[MAX_HOST_NAME_LEN+1];
	FILE *fp;
        char decryptfile[MAX_FILENAME_LEN];

	sprintf(filename, "%s/%s", db_dir, "MASTER");
	if ((fp = decryptAndOpen(filename, decryptfile, "r")) == NULL) {
                remove(decryptfile);
		return(-1);
        }

	if (fgets(hostname, MAX_HOST_NAME_LEN, fp) == NULL) {
		fclose(fp);
                remove(decryptfile);
		return(-1);
	}
	stripNewline(hostname);
	fclose(fp);
        remove(decryptfile);

	if (strcmp(hostname, "I AM MASTER") == 0)
		return(1);

	return(0);
}



int are_any_servers_running()
{
	char msg[MAX_INSTALL_ITEM_LEN], host[MAX_INSTALL_ITEM_LEN];
	int sock, i, msg_label_len = 20;
	int m,n;

	memset(msg, ' ', msg_label_len);
	msg[msg_label_len] = 0;
	sprintf(host, "%1d", SERVER_STATE_CHECK);
	strcat(msg, host);
	memset(msg, '\0', msg_label_len);
	memcpy(msg, "SERVERSTATUS", strlen("SERVERSTATUS"));

	if (gethostname(host, MAX_INSTALL_ITEM_LEN) != 0)
		return(0);
	/* ADDED VSD 10/20/99 */
	n = CMD_SVR_PORT;
	m = SOCK_STREAM;

	if ((sock = sock_init(host, CMD_SVR_PORT, 0, SOCK_STREAM)) < 0)
		return(0);

	if (write(sock, msg, sizeof(msg)) < 0) {
		close(sock);
		return(0);
	}
	if (signal(SIGALRM, signal_alarm) == SIG_ERROR) {
		close(sock);
		return(0);
	}
	if (setjmp(env_alrm) != 0) {
		close(sock);
		return(0);
	}
	(void) alarm(5);

	if (read_sock(sock, msg, msg_label_len + 6, NO_TIME_OUT,
									SOCK_STREAM) < (msg_label_len + 6)) {
		close(sock);
		return(0);
	}
	(void) alarm(0);
	close(sock);

	host[0] = msg[msg_label_len + 1];
	host[1] = 0;
	i = atoi(host);
	if (i == SERVER_STATE_IDLE)
		return(1);
	else if (i == SERVER_STATE_AWAKE)
		return(2);

	return(0);
}

