
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

/** MOTIF INCLUDES **/

#include <Xm/Xm.h>
#include <X11/Intrinsic.h>
#include <Xm/List.h>

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <sys/utsname.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <MiscXFcts.h>
#include <ExtList.h>

/** LOCAL INCLUDES **/

#include "fs_archive.h"
#include "problem_defs.h"


/** FUNCTION PROTOTYPES **/

int get_item(int, int, char*, char*);
int determine_machine_type(void);
void read_devicelist_in(XtPointer, int*, XtInputId*);
void read_backup_filelist_in(XtPointer, int*, XtInputId*);
void read_restore_filelist_in(XtPointer, int*, XtInputId*);
Problem prepare_archive_dirs(char*, char*);
int get_num_jobs(int);
Problem get_job_data(int);
int get_job_row_num(char*);
int get_device_row_num(char*);
void set_job_rows(void);
char *get_msg_type(int);
int get_msg_type_num(char*);
void convert_name_to_dir(char*, char*);
int get_file_size(char*);
int check_job_name(char*);
Problem create_job_config_file(JobData*, int);


/** STATIC VARIABLES **/

static char *msg_types[] = { "No Message", "Error List", "Full List", "Unknown Value" };

static char *no_yes[] = { "No", "Yes" };


/** GLOBAL VARIABLES **/

DeviceData dev_data[MAX_NUM_DEVICES];
int num_devices = 0;
JobData job_data[MAX_NUM_JOBS_PER_DEV];
int num_jobs;
int num_job_rows;




/** FUNCTION DEFINITIONS **/

int get_item(int num, int tot, char *line, char *item)
{
    int i, j, count;
    
    if (num == tot)
	count = tot - 1;
    else
	count = num;
    
    j = 0;
    for (i = 0; i < count; i++) {
	if ((j = stripWord(j, line, item)) == -1)
	    return(0);
    }
    if (num == tot) {
	strcpy(item, &line[j]);
	trim(item);
    }
    /*
     * Check if item is blank or not.
     */
    if (isBlank(item))
	return(0);
    else
	return(1);
}



int determine_machine_type()
{
    struct utsname name;
    
    if (uname(&name) == -1)
	return(0);
    
    if (strstr(name.sysname, "HP") != NULL) {
	if (strstr(name.release, "09.0") != NULL)
	    strcpy(machine_type, "hp90");
	else
	    strcpy(machine_type, "hp");
	return(1);
    } else if (strstr(name.sysname, "Sun") != NULL) {
	strcpy(machine_type, "sun");
	return(1);
    }
    else if (strstr(name.sysname, "Linux") != NULL) {
	strcpy(machine_type, "linux");
	return(1);
    }
    
    return(0);
}



void read_devicelist_in(XtPointer client, int *fd, XtInputId *id)
{
    char buf[MAX_FILELINE_LEN];
    XmString xstr;
    static Widget w = NULL;
    arcenv_t aenv;
    
    /* done only to get rid of compiler warning about unused parameters */
    fd=fd;
    
    if (w == NULL) {
	FINDCONTEXT(toplevel, env, &aenv);
	w = aenv->devicelist;
    }
    
    if (fgets(buf, MAX_FILELINE_LEN, (FILE *) client) != NULL) {
	if (buf[0] == '#') {
	    (void) checkForStop(toplevel);
	    return;
	}
	stripNewline(buf);
	if ((num_devices < MAX_NUM_DEVICES) &&
	    get_item(1, 3, buf, dev_data[num_devices].dir) &&
	    get_item(2, 3, buf, dev_data[num_devices].file) &&
	    get_item(3, 3, buf, dev_data[num_devices].name)) {
	    if (strcmp(dev_data[num_devices].dir, "DRIVE") == 0) {
		dev_data[num_devices].is_drive = 1;
	    } else if (strcmp(dev_data[num_devices].dir, "FILE") == 0) {
		dev_data[num_devices].is_drive = 0;
	    } else {
		(void) checkForStop(toplevel);
		return;
	    }
	    dev_data[num_devices].lock_id = num_devices + 1;
	    convert_name_to_dir(dev_data[num_devices].name,
				dev_data[num_devices].dir);
	    xstr = XmStringCreateSimple(dev_data[num_devices].name);
	    XmListAddItem(w, xstr, 0);
	    XmStringFree(xstr);
	    num_devices++;
	}
	(void) checkForStop(toplevel);
    } else {
	fclose((FILE *) client);
	XtRemoveInput(*id);
	waitCursor(toplevel, False, False);
	
	if (num_devices <= 0)
	    displayProblem(ERROR, CANT_DETERMINE_DEVICES_PROB,
			   problem_array, toplevel);
    }
}



void read_backup_filelist_in(XtPointer client, int *fd, XtInputId *id)
{
    char buf[MAX_FILELINE_LEN], listing[MAX_FILELISTING_LEN];
    XmString xstr;
    static Widget w = NULL;
    static Widget size_w = NULL;
    static int showsize = -1;
    arcenv_t aenv;

    /* done only to get rid of compiler warning about unused parameters */
    fd=fd;
    
    if (w == NULL || size_w == NULL) {
	FINDCONTEXT(toplevel, env, &aenv);
	w = aenv->backup_filelist;
	size_w = aenv->backup_tb[B_SIZE_FLD];
	showsize = aenv->show_size;
    }
    if (showsize == -1) {
	FINDCONTEXT(toplevel, env, &aenv);
	showsize = aenv->show_size;
    }
    
    if (fgets(buf, MAX_FILELINE_LEN, (FILE *) client) != NULL) {
	(void) checkForStop(backup_shell);
	stripNewline(buf);
	if (showsize)
	    filelist_size += get_file_size(buf);
	build_file_listing(buf, listing);
	xstr = XmStringCreateSimple(listing);
	XmListAddItem(w, xstr, 0);
	XmStringFree(xstr);
    } else {
#ifdef HPUX
	if (showsize)
	    filelist_size = filelist_size/2;
#endif
	if (showsize)
	    sprintf(buf, "%d", filelist_size);
	else
	    strcpy(buf, "Not Determined");
	showsize = -1;
	XmTextFieldSetString(size_w, buf);
	fclose((FILE *) client);
	XtRemoveInput(*id);
	waitCursor(backup_shell, False, False);
    }
}



void read_restore_filelist_in(XtPointer client, int *fd, XtInputId *id)
{
    char buf[MAX_FILELINE_LEN], listing[MAX_FILELISTING_LEN];
    XmString xstr;
    static Widget w = NULL;
    arcenv_t aenv;
    static FILE *fp;
    int flag;
    
    /* done only to get rid of compiler warning about unused parameters */
    fd=fd;
    
    if (w == NULL) {
	FINDCONTEXT(toplevel, env, &aenv);
	w = aenv->restore_filelist;
	sprintf(buf, "%s/%s/%s/%s", resources.data_dir, "restore",
		dev_data[aenv->device_row_num - 1].dir, "tarlist");
	fp = fopen(buf, "r");
    }
    
    if (fgets(buf, MAX_FILELINE_LEN, (FILE *) client) != NULL) {
	(void) checkForStop(restore_shell);
	stripNewline(buf);
	flag = 1;
	if (fp != NULL) {
	    while (fgets(listing, MAX_FILELISTING_LEN, fp) != NULL) {
		stripNewline(listing);
		if (strcmp(&listing[FILELISTING_FILE_POS], buf) == 0) {
		    flag = 0;
		    break;
		}
	    }
	    if (flag)
		rewind(fp);
	}
	if (flag || (strncmp(listing, "N/A", 3) == 0))
	    return;
	xstr = XmStringCreateSimple(listing);
		XmListAddItem(w, xstr, 0);
		XmStringFree(xstr);
    } else {
	fclose(fp);
	w = NULL;
	fclose((FILE *) client);
	XtRemoveInput(*id);
	waitCursor(restore_shell, False, False);
    }
}



Problem prepare_archive_dirs(char *dirname, char *devfile)
{
    DIR *dp;
    char buf[MAX_FILENAME_LEN];
    
    sprintf(buf, "%s/%s", dirname, "backup");
    if ((dp = opendir(buf)) == NULL)
	return(CANT_OPEN_THING_PROB);
    closedir(dp);
    
    sprintf(buf, "%s/%s", dirname, "restore");
    if ((dp = opendir(buf)) == NULL)
	return(CANT_OPEN_THING_PROB);
    closedir(dp);
    
    if (access(devfile, R_OK) == -1)
	return(CANT_DETERMINE_DEVICES_PROB);
    
    return(NO_PROB);
}



int get_num_jobs(int dev_row)
{
    char buf[MAX_FILENAME_LEN], config_file[MAX_FILENAME_LEN];
    int njobs;
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;
    
    sprintf(buf, "%s/backup/%s", resources.data_dir, dev_data[dev_row].dir);
    if ((dp = opendir(buf)) == NULL)
	return(-1);
    
    njobs = 0;
    while ((direc = readdir(dp)) != NULL) {
	if (direc->d_ino == 0)
	    continue;
	if (strcmp(direc->d_name, ".") == 0 ||
	    strcmp(direc->d_name, "..") == 0)
	    continue;
	sprintf(config_file, "%s/%s/config_file", buf, direc->d_name);
	if (stat(config_file, &sbuf) == -1)
	    continue;
	
	if ((sbuf.st_mode & S_IFMT) == S_IFREG)
	    if (access(config_file, R_OK) != -1)
		njobs++;
    }
    closedir(dp);
    
    return(njobs);
}



Problem get_job_data(int dev_row)
{
    char buf[MAX_FILENAME_LEN], config_file[MAX_FILENAME_LEN];
    char str[MAX_FILELINE_LEN], time_[3];
    FILE *fp;
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;
    
    sprintf(buf, "%s/backup/%s", resources.data_dir, dev_data[dev_row].dir);
    if ((access(buf, F_OK) == -1) &&
	(mkdir(buf,
	       S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0))
	return(CANT_CREATE_THING_PROB);
    
    if ((dp = opendir(buf)) == NULL)
	return(CANT_OPEN_THING_PROB);
    
    num_jobs = 0;
    while ((direc = readdir(dp)) != NULL) {
	(void) checkForStop(toplevel);
	
	if (direc->d_ino == 0)
	    continue;
	if (strcmp(direc->d_name, ".") == 0 ||
			strcmp(direc->d_name, "..") == 0)
	    continue;
	
	sprintf(config_file, "%s/%s/config_file", buf, direc->d_name);
		if (stat(config_file, &sbuf) == -1)
		    continue;
		
		if (((sbuf.st_mode & S_IFMT) == S_IFREG)) {
		    if (num_jobs >= MAX_NUM_JOBS_PER_DEV)
			continue;
		    
		    if ((fp = fopen(config_file, "r")) == NULL)
			continue;
		    /*
		     * Get job data from config file.
		     */
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
			continue;
		    }
		    stripNewline(str);
		    ncpy(job_data[num_jobs].name, str, MAX_ITEM_LEN);
		    
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
			continue;
		    }
		    stripNewline(str);
		    ncpy(job_data[num_jobs].archive, str, MAX_ITEM_LEN);
		    
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
			continue;
		    }
		    time_[0] = str[0];
		    time_[1] = str[1];
		    time_[2] = 0;
		    job_data[num_jobs].hours = atoi(time_);
		    if (job_data[num_jobs].hours < 0 ||
			job_data[num_jobs].hours > HIGH_HOUR) {
			fclose(fp);
			continue;
		    }
		    time_[0] = str[3];
		    time_[1] = str[4];
		    time_[2] = 0;
		    job_data[num_jobs].minutes = atoi(time_);
		    if (job_data[num_jobs].minutes < 0 ||
			job_data[num_jobs].minutes > HIGH_MINUTE) {
			fclose(fp);
			continue;
		    }
		    
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
			continue;
		    }
		    stripNewline(str);
		    ncpy(job_data[num_jobs].daystr, str, NUM_DAYS + 1);
		    
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
				continue;
		    }
		    stripNewline(str);
		    ncpy(job_data[num_jobs].email, str, MAX_ITEM_LEN);
		    
		    if (fgets(str, MAX_FILELINE_LEN, fp) == NULL) {
			fclose(fp);
			continue;
		    }
		    stripNewline(str);
		    job_data[num_jobs].msg_type = atoi(str);
		    if (job_data[num_jobs].msg_type < 0 ||
			job_data[num_jobs].msg_type >= NUM_MSG_TYPES)
			job_data[num_jobs].msg_type = 0;
		    
		    sprintf(job_data[num_jobs].dir, "%s/%s", buf, direc->d_name);
		    
		    job_data[num_jobs].is_enabled = return_cron_state(job_data[num_jobs].dir);
		    
		    num_jobs++;
		    fclose(fp);
		}
    }
    closedir(dp);
    
    return(NO_PROB);
}



int get_job_row_num(char *job_name)
{
    int i;
    
    for (i = 0; i <= num_jobs; i++)
	if (strcmp(job_data[i].name, job_name) == 0)
	    return(i);
    
    return(-1);
}



int get_device_row_num(char *dev_name)
{
    int i;
    
    for (i = 0; i <= num_devices; i++)
	if (strcmp(dev_data[i].name, dev_name) == 0)
	    return(i);
    
    return(-1);
}



void set_job_rows()
{
    static char *days[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };
    char str[MAX_ITEM_LEN];
    int i, j;
    ExtdListItemStruct jrow;
    XmString *rowitem;
    arcenv_t aenv;
    
    num_job_rows = num_jobs;
    
    FINDCONTEXT(toplevel, env, &aenv);
    XiExtdListDeleteAllItems(aenv->joblist);
    rowitem = (XmString*) XtMalloc(sizeof(XmString)*NUM_JOB_TITLES);
    
    /* Determine rows for backup jobs.   */
    for (i = 0; i < num_job_rows; i++) {
	sprintf(str, "%2.2d:%2.2d", job_data[i].hours,
		job_data[i].minutes);
	rowitem[0] = XmStringCreateLocalized(job_data[i].name);
	rowitem[1] = XmStringCreateLocalized(str);
	str[0] = NTCSS_NULLCH;
	for (j = 0; j < NUM_DAYS; j++)
	    if (job_data[i].daystr[j] == '1')
		strcat(str, days[j]);
	rowitem[2] = XmStringCreateLocalized(str);
	if (job_data[i].is_enabled)
	    j = 1;
	else
	    j = 0;
	rowitem[3] = XmStringCreateLocalized(no_yes[j]);
	jrow.item = rowitem;
	jrow.selected = False;
	jrow.sensitive = True;
	jrow.foreground = 0;
	jrow.hook = NULL;
	XiExtdListAddItem(aenv->joblist, jrow, 0);
	XmStringFree(rowitem[0]);
	XmStringFree(rowitem[1]);
	XmStringFree(rowitem[2]);
	XmStringFree(rowitem[3]);
    }
    XtFree((char *)rowitem);
}



char *get_msg_type(int value)
{
    if (value >= 0 && value < NUM_MSG_TYPES)
	return(msg_types[value]);
    else
	return(msg_types[NUM_MSG_TYPES]);
}



int get_msg_type_num(char *str)
{
    int i;
    
    for (i = 0; i < NUM_MSG_TYPES; i++)
	if (strcmp(str, msg_types[i]) == 0)
	    return(i);
    
    return(0);
}



void convert_name_to_dir(char *name, char *dir)
{
    char *n = name, *d = dir;
    
    while (*n) {
	if (isalnum((int) *n))
	    *d++ = *n;
	else
	    *d++ = '_';
	n++;
    }
    *d = 0;
}



int get_file_size(char *file)
{
    char cmd[MAX_CMD_LEN], buf[MAX_FILELINE_LEN];
    FILE *pp;
    
    sprintf(cmd, "%s %s", DU_PROG, file);
    if ((pp = popen(cmd, "r")) == NULL)
	return(0);
    
    while (fgets(buf, MAX_FILELINE_LEN, pp) != NULL) {
	if (isdigit((int)buf[0])) {
	    pclose(pp);
	    return(atoi(buf));
	}
    }
    pclose(pp);
    
    return(0);
}



int check_job_name(char *name)
{
    int leng;
    char *s = name;
    
    trim(name);
    leng = strlen(name);
    
    if (leng <= 0 || leng > MAX_JOBNAME_LEN)
	return(0);
    
    while (*s) {
	if (!(isalnum((int) *s) || *s == ' '))
	    return(0);
	s++;
    }
    
    if (strcmp(name, "tmp_tmp_tmp") == 0)
	return(0);
    
    return(1);
}



Problem create_job_config_file(JobData *job, int dev_i)
{
    char config_file[MAX_FILENAME_LEN];
    static char *dev_type[] = { "FILE", "DRIVE" };
    FILE *fp;
    
    sprintf(config_file, "%s/config_file", job->dir);
    if ((fp = fopen(config_file, "w")) == NULL)
	return(CANT_CREATE_FILE_PROB);
    
    fprintf(fp, "%s\n%s\n%2.2d:%2.2d\n%s\n%s\n%d\n%s\n%s\n%s\n",
	    job->name,
	    job->archive,
	    job->hours, job->minutes,
	    job->daystr,
	    job->email,
	    job->msg_type,
	    dev_type[dev_data[dev_i].is_drive],
	    dev_data[dev_i].file,
	    dev_data[dev_i].name);
    
    fclose(fp);
    
    return(NO_PROB);
}
