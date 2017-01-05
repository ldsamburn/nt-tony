
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * read_header.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/** INRI INCLUDES **/

#include <StringFcts.h>

/** LOCAL INCLUDES **/

#include "problem_defs.h"
#include "tape_info.h"
#include "inri_version.h"


/** FUNCTION PROTOTYPES **/

void stop_process(int);
void exit_proc(int);
int main(int, char*[]);


/** STATIC VARIABLES **/


/** FUNCTION DEFINITIONS **/

void stop_process(int signo)
{
    signo = signo;
    exit(1);
}



void exit_proc(int value)
{
    printf("FATAL%d###", value);
    fflush(stdout);
    exit(1);
}



int main(int argc, char *argv[])
{
    char header[MAX_FILENAME_LEN], filelist[MAX_FILENAME_LEN];
    char worklist[MAX_FILENAME_LEN];
    char cmd[MAX_CMD_LEN], tarlist[MAX_FILENAME_LEN];
    int is_drive;
    
    signal(SIGUSR1, stop_process);
    
    /*
     * Arg 1 is device directory.
     * Arg 2 is restore directory.
     * Arg 3 is DRIVE or FILE.
     * Arg 4 is machine type.
     */
    if (argc != 5)
	exit_proc(WRONG_NUM_ARGS_PROB);
    
    sprintf(header, "%s/header", argv[2]);
    sprintf(filelist, "%s/filelist", argv[2]);
    sprintf(worklist, "%s/worklist", argv[2]);
    sprintf(tarlist, "%s/tarlist", argv[2]);

    /*
     * Remove old files.
     */
    if (access(header, F_OK) != -1)
	if (unlink(header) == -1)
	    exit_proc(UNLINK_FILE_PROB);
    if (access(filelist, F_OK) != -1)
	if (unlink(filelist) == -1)
	    exit_proc(UNLINK_FILE_PROB);
    if (access(worklist, F_OK) != -1)
	if (unlink(worklist) == -1)
	    exit_proc(UNLINK_FILE_PROB);
    if (access(tarlist, F_OK) != -1)
	if (unlink(tarlist) == -1)
	    exit_proc(UNLINK_FILE_PROB);
    
    if (strncmp(argv[3], "DRIVE", 5) == 0)
	is_drive = 1;
    else
	is_drive = 0;
    
    switch (is_drive) {
    case 0:
	sprintf(cmd,
		"(cd %s; %s xpf %s%s header filelist worklist tarlist)",
		argv[2], TAR_PROG, argv[1], ".info.tar");
	printf("INFOExtracting header files.###");
	fflush(stdout);
#ifdef DEBUG
	printf("DEBUG%s###", cmd);
	fflush(stdout);
#endif
	if (system(cmd) != 0)
	    exit_proc(TAR_EXTRACT_FAILED_PROB);
	
	if (access(header, R_OK) == -1)
	    exit_proc(BAD_HEADER_FILE_PROB);
	if (access(filelist, R_OK) == -1)
	    exit_proc(BAD_FILELIST_FILE_PROB);
	if (access(worklist, R_OK) == -1)
	    exit_proc(BAD_WORKLIST_FILE_PROB);
	if (access(tarlist, R_OK) == -1)
	    exit_proc(BAD_TARLIST_FILE_PROB);
	break;
	
    case 1:
	sprintf(cmd, "%s %s %s", MT_PROG, argv[1], REWIND_CMD);
	printf("INFORewinding device.###");
	fflush(stdout);
#ifdef DEBUG
	printf("DEBUG%s###", cmd);
	fflush(stdout);
#endif
	if (system(cmd) != 0)
	    exit_proc(MT_REW_FAILED_PROB);
	
	sprintf(cmd, "(cd %s; %s xpf %s header filelist worklist tarlist)",
		argv[2], TAR_PROG, argv[1]);
	printf("INFOExtracting header files.###");
	fflush(stdout);
#ifdef DEBUG
	printf("DEBUG%s###", cmd);
	fflush(stdout);
#endif
	if (system(cmd) != 0)
	    exit_proc(TAR_EXTRACT_FAILED_PROB);
	
	if (access(header, R_OK) == -1)
	    exit_proc(BAD_HEADER_FILE_PROB);
	if (access(filelist, R_OK) == -1)
	    exit_proc(BAD_FILELIST_FILE_PROB);
	if (access(worklist, R_OK) == -1)
	    exit_proc(BAD_WORKLIST_FILE_PROB);
	if (access(tarlist, R_OK) == -1)
	    exit_proc(BAD_TARLIST_FILE_PROB);
	
#ifndef SOLARIS
	if ((strcmp(argv[4], "hp90") == 0) ||
	    (strcmp(argv[4], "sun") == 0) ||
	    (strcmp(argv[4], "linux") == 0)) {
	    sprintf(cmd, "%s %s fsf", MT_PROG, argv[1]);
	    printf("INFOFast forwarding device.###");
	    fflush(stdout);
#ifdef DEBUG
	    printf("DEBUG%s###", cmd);
	    fflush(stdout);
#endif
	    if (system(cmd) != 0)
		exit_proc(MT_FSF_FAILED_PROB);
	}
#endif
	break;
    }	
    printf("DONERead header###");
    fflush(stdout);
    exit(0);
}
