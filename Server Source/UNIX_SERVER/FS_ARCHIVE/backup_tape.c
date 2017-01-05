
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * backup_tape.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

/** INRI INCLUDES **/

#include <Ntcss.h>
#include <StringFcts.h>

/** LOCAL INCLUDES **/

#include "problem_defs.h"
#include "tape_info.h"
#include "inri_version.h"


/** FUNCTION PROTOTYPES **/

void stop_process(int);
void exit_proc(int);
void print_out(char*);
void print_problem(char*);
int main(int, char*[]);


/** STATIC VARIABLES **/

static int is_immed = 0;


/** FUNCTION DEFINITIONS **/

void stop_process(int signo)
{
    signo = signo;
    exit(1);
}



void exit_proc(int value)
{
    if (is_immed) {
	printf("FATAL%d###", value);
	fflush(stdout);
	exit(1);
    } else
	exit(value - ENOSYS);
}



void print_out(char *msg)
{
    if (is_immed) {
	printf("%s###", msg);
	fflush(stdout);
    }
}



void print_problem(char *msg)
{
    if (is_immed) {
#ifdef DEBUG
	printf("DEBUG%s###", msg);
	fflush(stdout);
#endif
    } else
	syslog(LOG_WARNING, "%s", msg);
}



int main(int argc, char *argv[])
{
    int is_drive, msg_type, i, j, size, lock;
    char header[MAX_FILENAME_LEN], configfile[MAX_FILENAME_LEN];
    char filelist[MAX_FILENAME_LEN], worklist[MAX_FILENAME_LEN];
    char cmd[MAX_CMD_LEN], tarlist[MAX_FILENAME_LEN], buf[MAX_ITEM_LEN];
    char archive[MAX_ITEM_LEN], email[MAX_ITEM_LEN], devdir[MAX_FILENAME_LEN];
    char message[MAX_FILENAME_LEN];
    char device[MAX_ITEM_LEN], tapelist[MAX_FILENAME_LEN];
    char report[MAX_CMD_LEN+64], hostname[MAX_ITEM_LEN];
    FILE *fp, *wp, *mp;
    time_t tme, tme2, tme3; 

    /*  Commented the following lines out for the ansi compile.  These
	variables were giving warnings but are not used. */
    /*
      time_t time();
      struct tm *localtime();
      #ifdef VSUN
      int strftime();
      #else
      size_t strftime();
      #endif
    */

    signal(SIGUSR1, stop_process);

    /*
     * Arg 1 is backup job directory.
     * Arg 2 is IMMEDIATE or device lock filename
     */
    if (strncmp(argv[2], "IMMEDIATE", 9) == 0)
	is_immed = 1;
    
    if (!is_immed) {
	openlog("FS_ARCH", LOG_PID, LOG_LOCAL7);
	syslog(LOG_WARNING, "Timed backup job started.");
    }
    
    if (argc != 3) {
	print_problem("Backup job failed due to incorrect argument list.");
	exit_proc(WRONG_NUM_ARGS_PROB);
    }
    
    sprintf(tapelist, "%s/tapelist", argv[1]);
    (void) unlink(tapelist);
    sprintf(configfile, "%s/config_file", argv[1]);
    if (access(configfile, R_OK) == -1) {
	print_problem("Backup job failed because it could not read configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    sprintf(filelist, "%s/filelist", argv[1]);
    if (access(filelist, R_OK) == -1) {
	print_problem("Backup job failed because it could not read filelist file.");
	exit_proc(BAD_FILELIST_FILE_PROB);
    }
    
    sprintf(worklist, "%s/worklist", argv[1]);
    sprintf(header, "%s/header", argv[1]);
    sprintf(tarlist, "%s/tarlist", argv[1]);
    /*
     * Read config file.
     */
    if ((fp = fopen(configfile, "r")) == NULL) {
	print_problem("Backup job failed because it could not read configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    if (fgets(buf, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    if (fgets(archive, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    stripNewline(archive);
    if (fgets(buf, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    if (fgets(buf, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    if (fgets(email, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    stripNewline(email);
    if (fgets(buf, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    msg_type = atoi(buf);
    if (fgets(buf, MAX_ITEM_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    if (strncmp(buf, "DRIVE", 5) == 0)
	is_drive = 1;
    else
	is_drive = 0;
    if (fgets(devdir, MAX_FILENAME_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    stripNewline(devdir);
    if (fgets(device, MAX_FILENAME_LEN, fp) == NULL) {
	fclose(fp);
	print_problem("Backup job failed due to improper configuration file.");
	exit_proc(BAD_CONFIG_FILE_PROB);
    }
    stripNewline(device);
    fclose(fp);
    /*
     * Acquire device lock if timed backup.
     */
    lock = -1;
    if (!is_immed) {
#ifdef USE_FILE_LOCKS
	if (!setFileLock(argv[2], &lock, LOCK_WAIT_TIME)) {
	    print_problem("Backup job failed because it could not set device lock.");
	    exit_proc(CANT_GET_DEVICE_LOCK_PROB);
	}
#else
	lock = 1;
#endif
	/*
	 * Assemble mail message.
	 */
	switch (msg_type) {
	case FULL_LIST_MSGTYPE:
	case ERR_LIST_MSGTYPE:
	    sprintf(message, "%s/mail_msg", argv[1]);
	    if ((mp = fopen(message, "w")) == NULL) {
		print_problem("Backup job failed because it could not create mail message file.");
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	    if (gethostname(hostname, MAX_ITEM_LEN) != 0) {
		fprintf(mp, "Timed file system backup on unknown host\n");
		strcpy(hostname, "unknown host");
	    } else
		fprintf(mp, "Timed file system backup on host \"%s\"\n",
			hostname);
	    tme = time((long *) 0);
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme));
	    fprintf(mp, "Started at %s\n", buf);
	    fprintf(mp,
		    "Archive named \"%s\" was backed up on device \"%s\"\n",
		    archive, device);
	    fclose(mp);
	    break;
	}
    }
    /*
     * Create worklist file.
     */
    if ((wp = fopen(worklist, "w")) == NULL) {
	print_problem("Backup job failed because it could not create worklist file.");
	exit_proc(CANT_CREATE_WORKLIST_FILE_PROB);
    }
    if ((fp = fopen(filelist, "r")) == NULL) {
	print_problem("Backup job failed because it could not read filelist file.");
	exit_proc(BAD_FILELIST_FILE_PROB);
    }
    while (fgets(configfile, MAX_FILENAME_LEN, fp) != NULL) {
	stripNewline(configfile);
	i = strlen(configfile);
	if (configfile[i - 1] == '/')
	    configfile[i - 1] = 0;
	fprintf(wp, "%s\n", configfile);
    }
    fclose(wp);
    fclose(fp);
    /*
     * Create header file.
     */
    tme = time((long *) 0);
    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M\n%d %h %y\n",
		    localtime(&tme));
    if (determine_filelist_size(worklist, &size) != NO_PROB) {
	print_problem("Backup job failed because it could not determine archive size.");
	exit_proc(CANT_DETER_FILELIST_SIZE_PROB);
    }
    tme2 = time((long *) 0);
    if (create_tarlist(worklist, tarlist) != NO_PROB) {
	print_problem("Backup job failed because it could not create tarlist file.");
	exit_proc(CANT_CREATE_TARLIST_FILE_PROB);
    }
    tme3 = time((long *) 0);
    if ((fp = fopen(header, "w")) == NULL) {
	print_problem("Backup job failed because it could not create header file.");
	exit_proc(CANT_CREATE_HEADER_FILE_PROB);
    }
    fprintf(fp, "%s\n%s%d\n", archive, buf, size);
    fclose(fp);
    /*
     * Run pre-backup script.
     */
    if (!is_immed) {
	sprintf(cmd, "%s/prebackup", argv[1]);
	if (access(cmd, X_OK) != -1) {
	    if ((i = system(cmd)) != 0) {
		sprintf(report, "Backup job failed because prebackup program failed with exit value of %d.", i);
		print_problem(report);
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	}
    }
    
    switch (is_drive) {
    case 0:
	print_out("INFOArchiving header files.");
	sprintf(cmd, "(cd %s; %s cf %s%s %s %s %s %s)",
		argv[1], TAR_PROG, devdir, ".info.tar",
		stripPath(header), stripPath(filelist),
		stripPath(worklist), stripPath(tarlist));
	if (system(cmd) != 0) {
	    sprintf(report,"Backup job failed because command \"%s\" failed.",
		    cmd);
	    print_problem(report);
	    exit_proc(TAR_CREATE_FAILED_PROB);
	}
	print_out("INFOArchiving data files.");
#ifdef USE_GTAR
	sprintf(cmd,
		"csh -f -c \'setenv GZIP -3;%s -cvpzP --file=%s%s --files-from=%s >& %s;logger `echo Backup job gtar returned $status.`\'",
		GTAR_PROG, devdir, ".data.tar", worklist, tapelist);
#else
#ifdef HPUX
	sprintf(cmd, "csh -f -c \'%s cvf %s%s `cat %s` >& %s\'", TAR_PROG,
		devdir, ".data.tar", worklist, tapelist);
#else
	sprintf(cmd, "csh -f -c \'%s cvf %s%s -I %s >& %s\'", TAR_PROG,
		devdir, ".data.tar", worklist, tapelist);
#endif
#endif
	if (system(cmd) != 0) {
	    sprintf(report,"Backup job failed because command \"%s\" failed.",
		    cmd);
	    print_problem(report);
	    exit_proc(TAR_CREATE_FAILED_PROB);
	}
	break;
	
    case 1:
	print_out("INFORewinding device.");
	sprintf(cmd, "%s %s %s", MT_PROG, devdir, REWIND_CMD);
	if (system(cmd) != 0) {
	    sprintf(report,"Backup job failed because command \"%s\" failed.",
		    cmd);
	    print_problem(report);
	    exit_proc(MT_REW_FAILED_PROB);
	}
	
	print_out("INFOArchiving header files.");
	sprintf(cmd, "(cd %s; %s cf %s %s %s %s %s)",
		argv[1], TAR_PROG, devdir,
		stripPath(header), stripPath(filelist),
		stripPath(worklist), stripPath(tarlist));
	if (system(cmd) != 0) {
	    sprintf(report,"Backup job failed because command \"%s\" failed.",
		    cmd);
	    print_problem(report);
	    exit_proc(TAR_CREATE_FAILED_PROB);
	}
#ifdef HPUX
#ifdef USE_GTAR
	sprintf(cmd,
		"csh -f -c \'setenv GZIP -3;%s -cvpzP --file=%s --files-from=%s >& %s;logger `echo Backup job gtar returned $status.`\'",
		GTAR_PROG, devdir, worklist, tapelist);
#else
	sprintf(cmd, "csh -f -c \'%s cvf %s `cat %s` >& %s\'", TAR_PROG,
		devdir, worklist, tapelist);
#endif
#else
#ifdef USE_GTAR
	sprintf(cmd,
		"csh -f -c \'setenv GZIP -3;%s -cvpzP --file=%s --files-from=%s >& %s;logger `echo Backup job gtar returned $status.`\'",
		GTAR_PROG, devdir, worklist, tapelist);
#else
	sprintf(cmd, "csh -f -c \'%s cvf %s -I %s >& %s\'", TAR_PROG,
		devdir, worklist, tapelist);
#endif
#endif
	if ((j = system(cmd)) != 0) {
	    sprintf(report,"Backup job failed because command \"%s\" failed.",
		    cmd);
	    print_problem(report);
	    exit_proc(TAR_CREATE_FAILED_PROB);
	}
	break;
    }
    if (!is_immed)
	syslog(LOG_WARNING, "Timed backup job completed.");
    (void) unlink(header);
    (void) unlink(worklist);
    (void) unlink(tarlist);
    
    if (lock != -1) {
#ifdef USE_FILE_LOCKS
	unsetFileLock(lock);
#endif
	switch (msg_type) {
	case ERR_LIST_MSGTYPE:
	    if ((mp = fopen(message, "a")) == NULL) {
		print_problem("Failed to append to mail message file.");
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme2));
	    fprintf(mp, "Started creating listing at %s\n", buf);
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme3));
	    fprintf(mp, "Started writing data at %s\n", buf);
	    tme = time((long *) 0);
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme));
	    fprintf(mp, "Completed at %s\n", buf);
	    fprintf(mp, "Total size of all archived files is %d kbytes\n",
		    size);
	    
	    fprintf(mp, "\nError listing of archived files:\n");
	    if ((fp = fopen(tapelist, "r")) != NULL) {
		i = 0;
		while (fgets(cmd, MAX_CMD_LEN, fp) != NULL) {
		    if (strncmp(cmd, "tar:", 4) == 0) {
			fprintf(mp, "%s", cmd);
			if ((strstr(cmd, "couldn\'t get uname") != 
			     NTCSS_NULLCH) ||
			    (strstr(cmd, "couldn\'t get gname") != 
			     NTCSS_NULLCH))
			    i = 1;
		    } else if (i == 1) {
			fprintf(mp, "%s", cmd);
			i = 0;
		    }
		}
	    } else
		fprintf(mp, "Failed to read list of archived files\n");
	    fclose(fp);
	    fclose(mp);
	    sprintf(cmd,
		    "%s -s \"Backup Status Report from %s\" %s < %s",
		    MAIL_PROG, hostname,
		    email, message);
	    if (system(cmd) != 0) {
		print_problem("Failed to mail backup job status report.");
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	    break;
	    
	case FULL_LIST_MSGTYPE:
	    if ((mp = fopen(message, "a")) == NULL) {
		print_problem("Failed to append to mail message file.");
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme2));
	    fprintf(mp, "Started creating listing at %s\n", buf);
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme3));
	    fprintf(mp, "Started writing data at %s\n", buf);
	    tme = time((long *) 0);
	    (void) strftime(buf, MAX_ITEM_LEN, "%H:%M %d %h %y",
			    localtime(&tme));
	    fprintf(mp, "Completed at %s\n", buf);
	    fprintf(mp, "Total size of all archived files is %d kbytes\n",
		    size);
	    
	    fprintf(mp, "\nFull listing of archived files:\n");
	    if ((fp = fopen(tapelist, "r")) != NULL) {
		while (fgets(cmd, MAX_CMD_LEN, fp) != NULL)
		    fprintf(mp, "%s", cmd);
	    } else
		fprintf(mp, "Failed to read list of archived files\n");
	    fclose(fp);
	    fclose(mp);
	    sprintf(cmd,
		    "%s -s \"Backup Status Report from %s\" %s < %s",
		    MAIL_PROG, hostname,
		    email, message);
	    if (system(cmd) != 0) {
		print_problem("Failed to mail backup job status report.");
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	    break;
	}
	/*
	 * Run post-backup script and send message.
	 */
	sprintf(cmd, "%s/postbackup", argv[1]);
	if (access(cmd, X_OK) != -1) {
	    if ((i = system(cmd)) != 0) {
		sprintf(report,
			"Postbackup program failed with exit value of %d.", i);
		print_problem(report);
		exit_proc(BACKUP_MSG_SCRIPT_PROB);
	    }
	}
    }
    
    
    /* eject tape when done */
    
    print_out("INFOEjecting Tape.");
    sprintf(cmd, "%s %s %s", MT_PROG, devdir, EJECT_CMD);
    if (system(cmd) != 0) {
	print_problem("Unable to eject tape");
	exit_proc(MT_EJECT_FAILED_PROB);
    }
    
    print_out("DONEBackup");
    exit(0);
}
