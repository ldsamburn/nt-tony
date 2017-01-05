
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * restore_tape.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int main(int , char*[]);



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
    char cmd[MAX_CMD_LEN], str[MAX_FILENAME_LEN];
    FILE *fp;
    int is_drive, first_time = 1;
    
    signal(SIGUSR1, stop_process);
    
    /* done only to get rid of compiler warning about unused variables.
       these may not be used depending on #defines */
    str[0]=str[0];
    fp=fp;
    first_time=first_time;
    
    /*
     * Arg 1 is device directory.
     * Arg 2 is worklist file.
     * Arg 3 is DRIVE or FILE.
     * Arg 4 is machine type.
     */
    if (argc != 5)
	exit_proc(WRONG_NUM_ARGS_PROB);
    
    if (access(argv[2], R_OK) == -1)
	exit_proc(BAD_WORKLIST_FILE_PROB);
    
    if (strncmp(argv[3], "DRIVE", 5) == 0)
	is_drive = 1;
    else
	is_drive = 0;
    
#ifdef USE_GTAR
    printf("INFOExtracting data files.###");
    fflush(stdout);
    
    if (is_drive)
	sprintf(cmd, "%s -xpzP --file=%s --files-from=%s",
		GTAR_PROG, argv[1], argv[2]);
    else
	sprintf(cmd, "%s -xpzP --file=%s%s --files-from=%s",
		GTAR_PROG, argv[1], ".data.tar", argv[2]);
#ifdef DEBUG
    printf("DEBUG%s###", cmd);
    fflush(stdout);
#endif
    if (system(cmd) != 0)
	exit_proc(TAR_EXTRACT_FAILED_PROB);
#else
#ifdef HPUX
    if ((fp = fopen(argv[2], "r")) != NULL) {
	while (fgets(str, MAX_FILENAME_LEN, fp) != NULL) {
	    stripNewline(str);
	    
	    if (is_drive && !first_time) {
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
	    
	    printf("INFOExtracting data files.###");
	    fflush(stdout);
	    
	    if (is_drive)
		sprintf(cmd, "%s xpf %s %s", TAR_PROG, argv[1], str);
	    else
		sprintf(cmd, "%s xpf %s%s %s", TAR_PROG, argv[1],
			".data.tar", str);
#ifdef DEBUG
	    printf("DEBUG%s###", cmd);
	    fflush(stdout);
#endif
	    if (system(cmd) != 0)
		exit_proc(TAR_EXTRACT_FAILED_PROB);

	    if (is_drive) {
		sprintf(cmd, "%s %s %s", MT_PROG, argv[1], REWIND_CMD);
		printf("INFORewinding device.###");
		fflush(stdout);
#ifdef DEBUG
		printf("DEBUG%s###", cmd);
		fflush(stdout);
#endif
		if (system(cmd) != 0)
		    exit_proc(MT_REW_FAILED_PROB);
		
	    }
	    first_time = 0;
	}
	fclose(fp);
    } else
	exit_proc(BAD_WORKLIST_FILE_PROB);
#else
    if (is_drive)
	sprintf(cmd, "%s xpf %s -I %s", TAR_PROG, argv[1], argv[2]);
    else
	sprintf(cmd, "%s xpf %s%s -I %s", TAR_PROG, argv[1],
		".data.tar", argv[2]);
    printf("INFOExtracting header files.###");
    fflush(stdout);
#ifdef DEBUG
    printf("DEBUG%s###", cmd);
    fflush(stdout);
#endif
    if (system(cmd) != 0)
	exit_proc(TAR_EXTRACT_FAILED_PROB);
#endif
#endif

    printf("DONERestore###");
    fflush(stdout);
    exit(0);
}
