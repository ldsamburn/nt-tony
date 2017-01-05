
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * install.c
 */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <search.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/utsname.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>

/** LOCAL INCLUDES **/

#include "install.h"
#include "inri_version.h"


/** DEFINE DEFINITIONS **/

#define TMP_DIR		"/tmp"


/** FUNCTION PROTOTYPES **/

void print_out(const char*);
int main(int, char**);
void cleanupSystemSettingsAndExit(int);



/** STATIC VARIABLES **/

static int net_install = 0, nResponsibleForSystemSettings = 0;
/*static FILE *logp;*/
FILE* logp;
static char machine_ext[16];


/** GLOBAL VARIABLES **/


/** FUNCTION DEFINITIONS **/


/*********************** cleanupSystemSettings **************************************************/

void cleanupSystemSettingsAndExit(int nExitCode)
{
    if (nResponsibleForSystemSettings) {
	if (freeNtcssSystemSettingsShm() != 0) {
	    syslog(LOG_WARNING, "install: cleanupSystemSettingsAndExit: freeNtcssSystemSettingsShm failed");
	} /* END if(freeNtcssSystemSettingsShm) */
    } /* END if(nResponsibleForSystemSettings) */
    
    exit(nExitCode);
    
}

/***********************************************************************************************/

void print_out(const char *msg)
{
    syslog(LOG_WARNING, msg);
    if (logp != NULL) {
	fprintf(logp, msg);
	fprintf(logp, "\n");
    }
    if (!net_install) {
	printf(msg);
	printf("\n");
    }
}



int main(int argc, char **argv)
{
    char file[MAX_FILENAME_LEN], device[MAX_FILENAME_LEN];
    char cmd[MAX_CMD_LEN];
    int i, flag, update_ntcss = 0;
    int do_gtar = 0, unix_uga_index = -1;
    int do_setup = 1;
    struct utsname machine_info;
    int device_specified = 0;
    
    /*	XtToolkitInitialize();*/
    
    openlog("NTCSS_INSTL", LOG_PID, LOG_LOCAL7);
	
    if (SystemSettingsAreInitialized() == FALSE) {
	nResponsibleForSystemSettings = 1;
	printf("Loading System Settings\n");
	if (loadNtcssSystemSettingsIntoShm() != 0) {
	    syslog(LOG_WARNING, "install: loadNtcssSystemSettingsIntoShm failed");
	    printf("loadNtcssSystemSettingsIntoShm failed. NTCSS II prime stopped\n");
	    exit(1);
	} /* END if */
    } /* END if */

    i = 1;
    while (i < argc) {
	/*
	 * Determine command-line action.
	 */
	if (strcmp(argv[i], "-network") == 0) {
	    net_install = 1;
	} else if (strcmp(argv[i], "-gtar") == 0) {
	    do_gtar = 1;
	} else if (strcmp(argv[i], "-nosetup") == 0) {
	    do_setup = 0;
	} else if (strcmp(argv[i], "-unixuga") == 0) {
	    i++;
	    if (i < argc)
		unix_uga_index = i;
        } else if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i < argc) {
                sprintf(device, "%s", argv[i]);
                device_specified = 1;
            }
	} else {
	    printf("USAGE: install {-network} {-gtar} {-unixuga <file extension>} {-nosetup} {-d <device name>}\n");
	    cleanupSystemSettingsAndExit(0);
	}
	i++;
    }
    /*
     * Open log file.
     */
    sprintf(file, "%s/install.log", TMP_DIR);
    if ((logp = fopen(file, "w")) == NULL) {
	sprintf(cmd, "Installation halted! Cannot open log file %s.", file);
	print_out(cmd);
	cleanupSystemSettingsAndExit(1);
    }
    print_out("NTCSS II installation started.");
    /*
     * Determine machine type.
     */
    if (uname(&machine_info) < 0) {
	sprintf(cmd, "Installation halted! Failed to ascertain machine information.");
	print_out(cmd);
	cleanupSystemSettingsAndExit(1);
    }
    if ((strstr(machine_info.sysname, "Sun") != NULL) &&
	(strstr(machine_info.release, "4.1.") != NULL)) {
	strcpy(machine_ext, "sun");
    } else if ((strstr(machine_info.sysname, "HP") != NULL) &&
	       (strstr(machine_info.release, "9.0") != NULL)) {
	strcpy(machine_ext, "hp90");
    } else if ((strstr(machine_info.sysname, "HP") != NULL) &&
	       (strstr(machine_info.release, "10.10") != NULL)) {
	strcpy(machine_ext, "hp1010");
    } else if ((strstr(machine_info.sysname, "HP") != NULL) &&
	       (strstr(machine_info.release, "10.20") != NULL)) {
	strcpy(machine_ext, "hp1020");
    } else if ((strstr(machine_info.sysname, "HP") != NULL) &&
	       (strstr(machine_info.release, "11.00") != NULL)) {
	strcpy(machine_ext, "hp1100");
    } else if ((strstr(machine_info.sysname, "SunOS") != NULL) &&
	       (strstr(machine_info.release, "5.5.") != NULL)) {
	strcpy(machine_ext, "sol25");
    } else if ((strstr(machine_info.sysname, "SunOS") != NULL) &&
	       (strstr(machine_info.release, "5.6") != NULL)) {
	strcpy(machine_ext, "sol26");
    } else if ((strstr(machine_info.sysname, "SunOS") != NULL) &&
	       (strstr(machine_info.release, "5.7") != NULL)) {
	strcpy(machine_ext, "sol27");
    } else if ((strstr(machine_info.sysname, "SunOS") != NULL) &&
	       (strstr(machine_info.release, "5.8") != NULL)) {
	strcpy(machine_ext, "sol27");
    } else if ((strstr(machine_info.sysname, "Linux") != NULL) &&
	       (strstr(machine_info.release, "2.2.5-15") != NULL)) {
	strcpy(machine_ext, "linux22");
    } else {
	sprintf(cmd, "Installation halted! Unable to handle this machine type and/or operating sytstem.");
	print_out(cmd);
	cleanupSystemSettingsAndExit(1);
    }
    /*
     * Check if any servers are still running.
     */
    if (are_any_servers_running(/*NTCSS_HOME_DIR*/) > 0) {
	print_out("Installation halted! NTCSS daemons are still running.");
	fclose(logp);
	cleanupSystemSettingsAndExit(1);
    }

    /*
     * Determine file source.
     */
    if (!device_specified) {
       if (!net_install) {
          do {
	    input_item("Enter tape device name:", device);
	  } while (strncmp("/", device, 1) != 0);
       } else {
	  sprintf(device, "%s/ntcss.tar", TMP_DIR);
       }
    }
    /*
     * Check for existence of NTCSS files.
     */
    if (access(NTCSS_HOME_DIR, F_OK) != -1)
	update_ntcss = 1;
    
    if (!update_ntcss) {
	sprintf(cmd, "%s %s", MKDIR_PROG, NTCSS_HOME_DIR);
	if (system(cmd) != 0) {
	    sprintf(file,
		    "Installation halted! Could not create top level directory %s.",
		    NTCSS_HOME_DIR);
	    print_out(file);
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
    }
    
    if (chdir(NTCSS_HOME_DIR) == -1) {
	sprintf(file,
		"Installation halted! Could not change to top level directory %s.",
		NTCSS_HOME_DIR);
	print_out(file);
	fclose(logp);
	cleanupSystemSettingsAndExit(1);
    }
    
    if (update_ntcss) {
	/*
	 * Remove old files before updating.
	 */
	sprintf(cmd, "%s %s bin libs api java", RM_PROG, REMOVE_FILES);
	if (system(cmd) != 0) {
	    print_out("Installation halted! Could not remove old NTCSS files.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	/*
	 * Tar off tape all needed files.
	 */
	sprintf(cmd, "%s xf \"%s\" %s bin.%s libs.%s api.%s java" , TAR_PROG,
		device, UPDATE_FILES, machine_ext, machine_ext, machine_ext);
	sprintf(file, "%s/message", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1)
	    strcat(cmd, " message");
	if (system(cmd) != 0) {
	    sprintf(file,
		    "Installation halted! Could not tar NTCSS files off device %s.",
		    device);
	    print_out(file);
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	flag = 0;
	sprintf(cmd, "%s xf \"%s\"", TAR_PROG, device);
	sprintf(file, "%s/applications/data/fs_archive", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " applications/data/fs_archive");
	    flag = 1;
	}
	sprintf(file, "%s/applications/data/sys_mon", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " applications/data/sys_mon");
	    flag = 1;
	}
	sprintf(file, "%s/applications/data/adm_menu", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " applications/data/adm_menu");
	    flag = 1;
	}
	if (unix_uga_index == -1)
	    sprintf(file, "%s/applications/data/unix_uga", NTCSS_HOME_DIR);
	else
	    sprintf(file, "%s/applications/data/unix_uga.%s",
		    NTCSS_HOME_DIR, argv[unix_uga_index]);
	if (access(file, F_OK) == -1) {
	    if (unix_uga_index == -1) {
		strcat(cmd, " applications/data/unix_uga");
	    } else {
		strcat(cmd, " applications/data/unix_uga.");
		strcat(cmd, argv[unix_uga_index]);
	    }
	    flag = 1;
	}
	if (flag && (system(cmd) != 0)) {
	    sprintf(file,
		    "Installation halted! Could not tar NTCSS files off device %s.",
		    device);
	    print_out(file);
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	flag = 0;
	sprintf(cmd, "%s xf \"%s\"", TAR_PROG, device);
	sprintf(file, "%s/applications/data/srv_dev", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " applications/data/srv_dev");
	    flag = 1;
	}
	sprintf(file, "%s/database/SYS_CONF", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " database/SYS_CONF");
	    flag = 1;
	}
	sprintf(file, "%s/database/predefined_jobs", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " database/predefined_jobs");
	    flag = 1;
	}
	sprintf(file, "%s/env", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " env");
	    flag = 1;
	}
	sprintf(file, "%s/applications/data/help_ini", NTCSS_HOME_DIR);
	if (access(file, F_OK) == -1) {
	    strcat(cmd, " applications/data/help_ini");
	    flag = 1;
	}
	if (flag && (system(cmd) != 0)) {
	    sprintf(file,
		    "Installation halted! Could not tar NTCSS files off device %s.",
		    device);
	    print_out(file);
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
    } else {
	/*
	 * Tar off tape all new files.
	 */
	sprintf(cmd, "%s xf \"%s\" %s bin.%s libs.%s api.%s java", TAR_PROG,
		device, NEW_FILES, machine_ext, machine_ext, machine_ext);
	if (system(cmd) != 0) {
	    sprintf(file,
		    "Installation halted! Could not tar NTCSS files off device %s.",
		    device);
	    print_out(file);
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	sprintf(cmd, "%s %s", TOUCH_PROG, ZERO_LENG_FILES);
	(void) system(cmd);
	sprintf(cmd, "%s %s", TOUCH_PROG, ZERO_LENG_BBMSG_FILES);
	(void) system(cmd);
    }
    /*
     * Install different user/group administration directory (if desired).
     */
    if (unix_uga_index != -1) {
	sprintf(cmd, "%s applications/data/unix_uga", RM_PROG);
	if (system(cmd) != 0) {
	    print_out("Installation halted! Could not remove old \"unix_uga\" directory.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	sprintf(cmd, "applications/data/unix_uga.%s", argv[unix_uga_index]);
	if (rename(cmd, "applications/data/unix_uga") != 0) {
	    print_out("Installation halted! Could not move \"unix_uga\" directory to its proper location.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
    }
    /*
     * Rename bin, libs, and api directories. No need to rename java dir
     */
    sprintf(cmd, "bin.%s", machine_ext);
    if (rename(cmd, "bin") != 0) {
	print_out("Installation halted! Could not move bin directory to its proper location.");
	fclose(logp);
	cleanupSystemSettingsAndExit(1);
    }
    sprintf(cmd, "%s bin.*", RM_PROG);
    (void) system(cmd);
    sprintf(cmd, "libs.%s", machine_ext);
    if (rename(cmd, "libs") != 0) {
	print_out("Installation halted! Could not move libs directory to its proper location.");
	fclose(logp);
	cleanupSystemSettingsAndExit(1);
    }
    sprintf(cmd, "%s libs.*", RM_PROG);
    (void) system(cmd);
    sprintf(cmd, "api.%s", machine_ext);
    if (rename(cmd, "api") != 0) {
	print_out("Installation halted! Could not move api directory to its proper location.");
	fclose(logp);
	cleanupSystemSettingsAndExit(1);
    }
    sprintf(cmd, "%s api.*", RM_PROG);
    (void) system(cmd);

    /*
     * Install "gtar" tape archive programs (if desired).
     */
    if (do_gtar) {
	sprintf(cmd, "%s bin/backup_tape bin/restore_tape", RM_PROG);
	if (system(cmd) != 0) {
	    print_out("Installation halted! Could not remove backup and restore program files.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	if (rename("bin/backup_tape.gtar", "bin/backup_tape") != 0) {
	    print_out("Installation halted! Could not move backup program file to its proper location.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
	if (rename("bin/restore_tape.gtar", "bin/restore_tape") != 0) {
	    print_out("Installation halted! Could not move restore program file to its proper location.");
	    fclose(logp);
	    cleanupSystemSettingsAndExit(1);
	}
    }
    /*
     * Move "set pid" program.
     */
    sprintf(cmd , "%s %s/bin/set_ntcss_pid %s/applications/data/install",
	    MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
    (void) system(cmd);
    sprintf(file, "%s/applications/data/install/set_ntcss_pid", NTCSS_HOME_DIR);
    if (access(file, F_OK) == -1) {
	sprintf(file,
		"Installation warning! Could not move \"set pid\" program file.");
	print_out(file);
    } else {
	sprintf(cmd, "%s 755 %s", CHMOD_PROG, file);
	(void) system(cmd);
	sprintf(cmd, "%s u+s %s", CHMOD_PROG, file);
	(void) system(cmd);
    }
    /*
     * Move "set status" program.
     */
    sprintf(cmd , "%s %s/bin/set_ntcss_status %s/applications/data/install",
	    MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
    (void) system(cmd);
    sprintf(file, "%s/applications/data/install/set_ntcss_status",
	    NTCSS_HOME_DIR);
    if (access(file, F_OK) == -1) {
	sprintf(file,
		"Installation warning! Could not move \"set status\" program file.");
	print_out(file);
    } else {
	sprintf(cmd, "%s 755 %s", CHMOD_PROG, file);
	(void) system(cmd);
	sprintf(cmd, "%s u+s %s", CHMOD_PROG, file);
	(void) system(cmd);
    }
    /*
     * Move "prelaunch" program.
     */
    sprintf(cmd , "%s %s/bin/prelaunch %s/applications/data/install",
	    MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
    (void) system(cmd);
    sprintf(file, "%s/applications/data/install/prelaunch", NTCSS_HOME_DIR);
    if (access(file, F_OK) == -1) {
	sprintf(file,
		"Installation warning! Could not move \"prelaunch\" program file.");
	print_out(file);
    } else {
	sprintf(cmd, "%s 755 %s", CHMOD_PROG, file);
	(void) system(cmd);
    }
    /*
     * Apply correct ownerships and permissions.
     */
    sprintf(cmd, "%s -R %s %s/api %s/applications %s/bin %s/database %s/env %s/init_data %s/java %s/libs %s/logs %s/message %s/spool", CHGRP_PROG, OWNER_GROUP, NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR,NTCSS_HOME_DIR);
    if (system(cmd) != 0) {
	sprintf(file,
		"Installation warning! Could not change UNIX group to %s on all NTCSS files under %s.",
		OWNER_GROUP, NTCSS_HOME_DIR);
	print_out(file);
    }
    
    sprintf(cmd, "%s 755 %s/database", CHMOD_PROG, NTCSS_HOME_DIR);
    if (system(cmd) != 0)
	print_out("Installation warning! Could not change permissions on NTCSS database directory.");
    
    sprintf(cmd, "%s 600 %s/database/*", CHMOD_PROG, NTCSS_HOME_DIR);
    if (system(cmd) != 0)
	print_out("Installation warning! Could not change permissions on NTCSS database files.");
    
    if (update_ntcss) {
	sprintf(cmd, "%s 644 %s/database/MASTER", CHMOD_PROG, NTCSS_HOME_DIR);
	if (system(cmd) != 0)
	    print_out("Installation warning! Could not change permissions on NTCSS database files.");
    }
    sprintf(file, "%s/applications/app-defaults", NTCSS_HOME_DIR);
    if (copy_app_default_files(file) < 0)
	print_out("Installation warning! Failed to copy the app default files.");
    
    if (do_setup) {
	sprintf(cmd, "%s/applications/data/install/prsetup", NTCSS_HOME_DIR);
	if (system(cmd) != 0)
	    print_out("Installation warning! Printer setup script failed.");
    }
    
    print_out("NTCSS II installation completed successfully.");
    fclose(logp);
    
#ifdef SOLARIS
    sprintf(cmd, "%s %s/install.log %s/logs/install.log",
	    MV_PROG, TMP_DIR, NTCSS_HOME_DIR);
    if (system(cmd) != 0) {
#else
	sprintf(cmd, "%s/install.log", TMP_DIR);
	sprintf(file, "%s/logs/install.log", NTCSS_HOME_DIR);
	if (rename(cmd, file) != 0) {
#endif
	    syslog(LOG_WARNING, "Installation warning! Cannot move log file to %s/logs.",
		   NTCSS_HOME_DIR);
	    if (!net_install)
		printf("Installation warning! Cannot move log file to %s/logs\n.",
		       NTCSS_HOME_DIR);
	}
	/*
	 * Remove unneeded files.
	 */
	sprintf(cmd, "%s %s/init_data/app_info/ntcss", RM_PROG, NTCSS_HOME_DIR);
	(void) system(cmd);
	sprintf(cmd, "%s %s/database/TIME_TEMPS", RM_PROG, NTCSS_HOME_DIR);
	(void) system(cmd);
	sprintf(cmd, "%s %s/database/*.dat", RM_PROG, NTCSS_HOME_DIR);
	(void) system(cmd);
	sprintf(cmd, "%s %s/database/*.idx", RM_PROG, NTCSS_HOME_DIR);
	(void) system(cmd);
	sprintf(cmd, "%s %s/README", RM_PROG, NTCSS_HOME_DIR);
	(void) system(cmd);
	
	if (net_install) {
	    sprintf(cmd, "%s/bin/ntcss_init -install", NTCSS_HOME_DIR);
	    if (system(cmd) != 0)
		syslog(LOG_WARNING, "Installation warning! Failed to start NTCSS.");
	}
	
	cleanupSystemSettingsAndExit(0);

	/* Set up the progrps.ini file correctly for the applicable machine. */
	if ((machine_ext == "sun") || (machine_ext == "sol25") || 
	    (machine_ext == "sol26") || (machine_ext == "sol27"))
	    sprintf(cmd, "%s %s/init_data/progrps.ini.SOL %s/init_data/progrps.ini",
		    MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
	else
	    if (machine_ext == "linux22")
		sprintf(cmd, 
			"%s %s/init_data/progrps.ini.LINUX %s/init_data/progrps.ini",
			MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
	    else
		if ((machine_ext == "hp90") || (machine_ext == "hp1010") ||
		    (machine_ext == "hp1020"))
		    sprintf(cmd,
			    "%s %s/init_data/progrps.ini.HP %s/init_data/progrps.ini",
			    MV_PROG, NTCSS_HOME_DIR, NTCSS_HOME_DIR);
	(void) system(cmd);
	
        return(0);
}

