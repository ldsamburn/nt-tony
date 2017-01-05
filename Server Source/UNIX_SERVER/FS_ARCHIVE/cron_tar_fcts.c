
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * cron_tar_fcts.c
 */

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <ctype.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <MiscXFcts.h>
#include <ExtraProtos.h>

/** LOCAL INCLUDES **/

#include "fs_archive.h"
#include "problem_defs.h"


/** FUNCTION PROTOTYPES **/

int change_cron_state(char*, int, JobData*, int);
int return_cron_state(char*);
int convert_daystr_to_days(char*, char*);
int launch_process(char**, int*);
void reaper(int);
void read_from_process(XtPointer, int*, XtInputId*);
void check_timer(XtPointer, XtInputId *);


/** STATIC VARIABLES **/

static int proc_client;


/** GLOBAL VARIABLES **/



/** FUNCTION DEFINITIONS **/

int change_cron_state(char *cmd, int state, JobData *job, int lock_id)
{
    FILE *fp, *pp;
    char listing[MAX_CMD_LEN], days[MAX_ITEM_LEN];
    char file[MAX_FILENAME_LEN], command[MAX_CMD_LEN];
    
    sprintf(file, "%s/cronlisting", job->dir);
    if ((fp = fopen(file, "w")) == NULL)
	return(0);
    
    if ((pp = popen(CRON_LIST_PROG, "r")) != NULL) {
		while (fgets(listing, MAX_CMD_LEN, pp) != NULL) {
		    if ((strstr(listing, cmd) == NULL) ||
			(strstr(listing, job->dir) == NULL))
			fprintf(fp, "%s", listing);
		}
		pclose(pp);
    } else {
	fclose(fp);
	return(0);
    }
    /*
     * Add cron job to listing if enabled.
     */
    if (state) {
	if (!convert_daystr_to_days(job->daystr, days)) {
	    fclose(fp);
	    return(0);
	}
	fprintf(fp, "%2.2d %2.2d * * %s %s %s %s/%s/%s.%d >/dev/null 2>&1\n",
		job->minutes, job->hours,
		days, cmd, job->dir,
		resources.db_dir, LOCK_DIR,
		DEVICE_LOCK_FILE, lock_id);
    }
    fclose(fp);
    
    sprintf(command, "%s %s", CRON_ADD_PROG, file);
#ifdef VSUN
    (void) system(command);
#else
    if (system(command) != 0)
	return(0);
#endif
    
    return(1);
}



int return_cron_state(char *job)
{
    int cron_state = 0;
    FILE *pp;
    char listing[MAX_CMD_LEN];
    
    if ((pp = popen(CRON_LIST_PROG, "r")) != NULL) {
	while (fgets(listing, MAX_CMD_LEN, pp) != NULL) {
	    if (strstr(listing, job) != NULL) {
		cron_state = 1;
		break;
	    }
	}
	pclose(pp);
    }
    
    return(cron_state);
}



int convert_daystr_to_days(char *daystr, char *days)
{
    char str[5];
    int i, leng, value, start = 1;
    static int offset = 0;
    
    days[0] = 0;
    if (strstr(daystr, "0") == NULL) {
	strcpy(days, "*");
	return(1);
    }
    
    leng = strlen(daystr);
    for (i = 0; i < leng; i++) {
	if (daystr[i] == '1') {
	    value = i + offset;
	    if (start) {
		start = 0;
		sprintf(str, "%d", value);
			} else
			    sprintf(str, ",%d", value);
	    strcat(days, str);
	}
    }
    if (strlen(days) == 0)
	return(0);
    
    return(1);
}



int launch_process(char **argv, int *fd)
{
    int fds[2];
    int pid;
    char command[256];
    
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
	return(-1);
    }
    
    if ((pid = fork()) < 0) {
	close(fds[0]);
	return(-1);
    }
    
    /* CHILD */
    
    if (pid == 0) {
	close(fds[0]);
	if (dup2(fds[1], 0) != 0) {
	    printf("WARNINGFailed to dup the fail descriptor.###");
	    fflush(stdout);
	}
	if (dup2(fds[1], 1) != 1) {
	    printf("WARNINGFailed to dup the fail descriptor.###");
	    fflush(stdout);
	}
	
	strcpy(command, argv[0]);
	stripPath(argv[0]);
	
	execv(command, argv);
	exit(1);
    }
    
    /* PARENT */
    
    close(fds[1]);
    *fd = fds[0];
    
    return(pid);
}


void reaper(int signo)
{
    int status;
    /*union wait status;*/
    
    signo = signo;
    while (wait3(&status, WNOHANG, /*(struct rusage *)*/NULL) >= 0)
	;
}



void read_from_process(XtPointer client, int *fd, XtInputId *id)
{
    static char str[512];
    static int nbytes = 0;
    char buf[512], *s, *p, msg[512], text[128];
    int n, i;
    Problem prog_err;
    
    /* done to get rid of compiler warning about unused parameters */
    fd=fd;
    
    proc_client = (int) client;
    
    if ((n = read(proc_client, buf, sizeof(buf))) != -1) {
	/*
	 * Parse message.
	 */
	buf[n] = 0;
	if (nbytes == 0) {
	    strcpy(str, buf);
	    nbytes = n;
	} else {
	    strcat(str, buf);
	    nbytes += n;
	}
	i = 0;
	p = &str[0];
	while ((s = strstr(p, "###")) != NULL) {
	    i = 0;
	    while (p != s) {
		msg[i] = *p;
		p++;
		i++;
	    }
	    msg[i] = 0;
	    p += strlen("###");
	    i = 1;
	    /*
	     * Take action based on message.
	     */
	    if (strncmp(msg, "DONE", strlen("DONE")) == 0) {
		close(proc_client);
		XtRemoveInput(*id);
		XtRemoveTimeOut(timer_id);
		waitCursor(toplevel, False, True);
		sprintf(text, "%s process completed successfully.",
			&msg[strlen("DONE")]);
		if (strncmp(text, "Read header", strlen("Read header")) == 0)
		    read_header();
		else
		    popupDialogBox(INFO, True, text,
				   True, False, False, NULL, toplevel);
	    } else if (strncmp(msg, "FATAL", strlen("FATAL")) == 0) {
		close(proc_client);
		XtRemoveInput(*id);
		XtRemoveTimeOut(timer_id);
		waitCursor(toplevel, False, True);
		prog_err = atoi(&msg[strlen("FATAL")]);
		displayProblem(WARNING, prog_err, problem_array, toplevel);
	    } else if (strncmp(msg, "DEBUG", strlen("DEBUG")) == 0) {
		printf("%s\n", &msg[strlen("DEBUG")]);
	    } else if (strncmp(msg, "INFO", strlen("INFO")) == 0) {
		changeWaitMessage(&msg[strlen("INFO")]);
	    } else if (strncmp(msg, "WARNING", strlen("WARNING")) == 0) {
		close(proc_client);
		XtRemoveInput(*id);
		XtRemoveTimeOut(timer_id);
		waitCursor(toplevel, False, True);
		popupDialogBox(WARNING, True, &msg[strlen("WARNING")],
			       True, False, False,
			       NULL, toplevel);
	    }
	}
	if (i) {
	    nbytes = 0;
	    str[0] = 0;
	}
    }
}



void check_timer(XtPointer client, XtInputId *id)
{
    static int pid = 0;
    arcenv_t aenv;
    
    proc_client = (int) client;
    
    if (pid == 0) {
	FINDCONTEXT(toplevel, env, (XPointer*)&aenv);
	pid = aenv->pid;
    }
    if (checkForStop(toplevel) == True) {
	kill((pid_t) pid, SIGUSR1);
	pid = 0;
	close(proc_client);
	XtRemoveInput(input_id);
	XtRemoveTimeOut(*id);
	waitCursor(toplevel, False, True);
	popupDialogBox(WARNING, True, "Process was aborted.",
		       True, False, False, NULL, toplevel);
    } else {
	XtRemoveTimeOut(*id);
	timer_id = XtAppAddTimeOut(context, 500, check_timer, 
				   (XtPointer)proc_client);
    }
}

