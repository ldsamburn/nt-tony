
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * tarlist_fcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <pwd.h>
#include <grp.h>
#include <time.h>

/** INRI INCLUDES **/

#include <StringFcts.h>

/** LOCAL INCLUDES **/

#include "problem_defs.h"
#include "tape_info.h"


/** FUNCTION PROTOTYPES **/

/* static Problem printout_dir(); */
Problem create_tarlist(char*, char*);
Problem determine_filelist_size(char*, int*);
void build_file_listing(char*, char*);
void get_owner(char*, struct stat*);
static void get_group(char*, struct stat*);


/** FUNCTION DEFINITIONS **/

/*
static Problem printout_dir(tarlist)
char *tarlist;
{
	DIR *dp;
#ifdef SOLARIS
	struct dirent *direc;
#else
	struct direct *direc;
#endif
	struct stat sbuf;
	char file[MAX_FILENAME_LEN], new_tarlist[MAX_FILENAME_LEN];
	char buf[MAX_FILENAME_LEN];
	FILE *tp, *np;
	int do_again, leng;
	char end_char;

	sprintf(new_tarlist, "%s.new", tarlist);

	do {
		if ((tp = fopen(tarlist, "r")) == NULL)
			return(CANT_READ_FILE_PROB);
	
		if ((np = fopen(new_tarlist, "w")) == NULL) {
			fclose(tp);
			return(CANT_CREATE_FILE_PROB);
		}

		do_again = 0;
		while (fgets(file, MAX_FILENAME_LEN, tp) != NULL) {
			stripNewline(file);
			leng = strlen(file);
			if (leng <= 0)
				continue;
			end_char = file[leng - 1];
			if (end_char != '/') {
				fprintf(np, "%s\n", file);
				continue;
			}
	
			if ((dp = opendir(file)) == NULL) {
				fprintf(np, "%s*\n", file);
				do_again = 1;
				break;
			}
			fprintf(np, "%s*\n", file);
	
			while ((direc = readdir(dp)) != NULL) {
				if (direc->d_ino == 0)
					continue;
				if ((strcmp(direc->d_name, ".") == 0) ||
					(strcmp(direc->d_name, "..") == 0))
					continue;
				sprintf(buf, "%s%s", file, direc->d_name);

				if (lstat(buf, &sbuf) != -1) {
					if (S_ISLNK(sbuf.st_mode)) {
						fprintf(np, "%s\n", buf);
						continue;
					}
				}

				if (stat(buf, &sbuf) == -1)
					continue;
				if (S_ISREG(sbuf.st_mode))
					fprintf(np, "%s\n", buf);
				else if (S_ISDIR(sbuf.st_mode))
					fprintf(np, "%s/\n", buf);
			}
			closedir(dp);
	
			do_again = 1;
			break;
		}
	
		if (do_again)
			while (fgets(file, MAX_FILENAME_LEN, tp) != NULL)
				fprintf(np, "%s", file);
	
		fclose(tp);
		fclose(np);
	
		(void) unlink(tarlist);
		(void) rename(new_tarlist, tarlist);
	} while (do_again);

	return(NO_PROB);
}
*/



Problem create_tarlist(char *worklist, char *tarlist)
{
    char file[MAX_FILENAME_LEN], new_tarlist[MAX_FILENAME_LEN];
    FILE *wp, *tp;
    int leng;
    /*Problem value;*/
    char /*end_char,*/ listing[MAX_FILELISTING_LEN], *s;
    
    if ((wp = fopen(worklist, "r")) == NULL)
	return(CANT_READ_FILE_PROB);
    
    if (access(tarlist, F_OK) != -1)
	(void) unlink(tarlist);
    
    /* new below */
    if ((tp = fopen(tarlist, "w")) == NULL) {
	fclose(wp);
	return(CANT_CREATE_FILE_PROB);
    } else
	fclose(tp);
    while (fgets(file, MAX_FILENAME_LEN, wp) != NULL) {
	stripNewline(file);
	sprintf(listing,
		"%s cf - %s | %s tf - >> %s",
		TAR_PROG, file, TAR_PROG, tarlist);
	if (system(listing) != 0) {
	    fclose(wp);
	    return(CANT_APPEND_FILE_PROB);
	}
    }
    fclose(wp);
    /* new above */
    
    /*	while (fgets(file, MAX_FILENAME_LEN, wp) != NULL) {
	if ((tp = fopen(tarlist, "a")) == NULL) {
	fclose(wp);
	return(CANT_APPEND_FILE_PROB);
	}
	trim(file);
	fprintf(tp, "%s", file);
	fclose(tp);
	if ((value = printout_dir(tarlist)) != NO_PROB) {
	fclose(wp);
	return(value);
	}
	}
	fclose(wp); FIX */
    
    
    
    /*
     * Strip off asterisks at end of lines.
     */
    sprintf(new_tarlist, "%s.new", tarlist);
    
    if ((tp = fopen(tarlist, "r")) == NULL)
	return(CANT_READ_FILE_PROB);
    
    if ((wp = fopen(new_tarlist, "w")) == NULL) {
	fclose(tp);
	return(CANT_CREATE_FILE_PROB);
    }
    
    while (fgets(file, MAX_FILENAME_LEN, tp) != NULL) {
	stripNewline(file);
	leng = strlen(file);
	if (leng <= 0)
	    continue;
	/*		end_char = file[leng - 1];
			if (end_char == '*')
			file[leng - 1] = 0; FIX */
	
	/* new below */
	if ((s = strstr(file, " symbolic link to ")) != NULL) {
	    leng = strlen(file) - strlen(s);
	    file[leng] = 0;
	}
	/* new above */
	
	build_file_listing(file, listing);
	fprintf(wp, "%s\n", listing);
    }
    
    fclose(tp);
    fclose(wp);
    
    (void) unlink(tarlist);
    (void) rename(new_tarlist, tarlist);
    
    return(NO_PROB);
}



Problem determine_filelist_size(char *worklist, int *total_size)
{
    char file[MAX_FILENAME_LEN], cmd[MAX_CMD_LEN];
    char buf[MAX_ITEM_LEN];
    FILE *wp, *pp;
    int size = 0;
    
    if ((wp = fopen(worklist, "r")) == NULL)
	return(CANT_READ_FILE_PROB);
    
    while (fgets(file, MAX_FILENAME_LEN, wp) != NULL) {
	trim(file);
	sprintf(cmd, "%s %s", DU_PROG, file);
	if ((pp = popen(cmd, "r")) != NULL) {
	    if (fgets(buf, MAX_ITEM_LEN, pp) != NULL)
		size += atoi(buf);
	    pclose(pp);
	}
    }
    fclose(wp);
#ifdef HPUX
    *total_size = size/2;
#else
    *total_size = size;
#endif
    
    return(NO_PROB);
}



void build_file_listing(char *file, char *listing)
{
    char type[6], perms[10], user[MAX_USER_LEN+1], group[MAX_GROUP_LEN+1];
    char time_str[18];
    struct stat sbuf;
    int i, leng;
    /*  Commented the following lines out for the ansi compile.  These 
	variables were giving warnings but are not used. */
    /*
      struct tm *localtime();
      #ifdef VSUN
      int strftime();
      #else
      size_t strftime();
      #endif
    */
    
    leng = strlen(file);
    if (file[leng - 1] == '/')
	file[leng - 1] = 0;
    
    type[0] = 0;
    if (lstat(file, &sbuf) != -1)
	if (S_ISLNK(sbuf.st_mode))
	    strcpy(type, "link ");
    
    if (type[0] == 0) {
	if (stat(file, &sbuf) == -1) {
	    sprintf(listing,
		    "N/A    N/A        N/A       N/A       N/A                %s", file);
	    return;
	}
	if (S_ISDIR(sbuf.st_mode)) {
	    strcat(file, "/");
	    strcpy(type, "dir  ");
	} else if (S_ISREG(sbuf.st_mode))
	    strcpy(type, "file ");
	else
	    strcpy(type, "other");
    }
    
    strcpy(perms, "---------");
    if (sbuf.st_mode & S_IRUSR)
	perms[0] = 'r';
    if (sbuf.st_mode & S_IWUSR)
	perms[1] = 'w';
    if (sbuf.st_mode & S_IXUSR)
	perms[2] = 'x';
    if (sbuf.st_mode & S_IRGRP)
	perms[3] = 'r';
    if (sbuf.st_mode & S_IWGRP)
	perms[4] = 'w';
    if (sbuf.st_mode & S_IXGRP)
	perms[5] = 'x';
    if (sbuf.st_mode & S_IROTH)
	perms[6] = 'r';
    if (sbuf.st_mode & S_IWOTH)
	perms[7] = 'w';
    if (sbuf.st_mode & S_IXOTH)
	perms[8] = 'x';
    
    get_owner(user, &sbuf);
    leng = strlen(user);
    for (i = leng; i < MAX_USER_LEN; i++)
	strcat(user, " ");
    
    get_group(group, &sbuf);
    leng = strlen(group);
    for (i = leng; i < MAX_GROUP_LEN; i++)
	strcat(group, " ");
    
    (void) strftime(time_str, 18, "%d %h %Y %H:%M", localtime(&sbuf.st_mtime));
    time_str[17] = 0;
    
    sprintf(listing, "%s  %s  %s  %s  %s  %s",
	    type, perms, user, group, time_str, file);
}



void get_owner(char *owner, struct stat *sbuf)
{
    struct passwd *entry;
    
    entry = getpwuid(sbuf->st_uid);
    if (entry == NULL || entry->pw_name == NULL || *entry->pw_name == 0)
	sprintf(owner, "%d", (int) sbuf->st_uid);
    else
	strcpy(owner, entry->pw_name);
}



static void get_group(char *group, struct stat *sbuf)
{
    struct group *entry;
    
    entry = getgrgid(sbuf->st_gid);
    if (entry == NULL || entry->gr_name == NULL || *entry->gr_name == 0)
	sprintf(group, "%d", (int) sbuf->st_gid);
    else
	strcpy(group, entry->gr_name);
}

