
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * MsgFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef SOLARIS
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <syslog.h>

/** INRI INCLUDES **/

#include <StringFcts.h>
#include <Ntcss.h>
#include <LibNtcssFcts.h>

/** LOCAL INCLUDES **/

#include "TextDBP.h"


/** STATIC FUNCTION PROTOTYPES **/

static void copy_sent_msg(const char*, const char*);
static int  does_bb_exist(const char*, const char*);
static int  does_extension_match(const char*, const char*);
static int  does_msg_exist(const char*, const char*);
static void get_bb_lock_path(const char*, char*, const char*);
static int  get_msg_title(const char*, const char*, char*);
static int  get_num_unread_msgs(const char*, const char*, const char*);
static int  is_bb_name_valid( const char*);



/** FUNCTION DEFINITIONS **/

/*****************************************************************************/

static int  does_extension_match( const char  *file,
                                  const char  *extension )

{
    int i, leng_ext, leng_file;

    leng_ext = strlen(extension) + 1;
    leng_file = strlen(file);

    if (leng_file < leng_ext)
        return(0);

    for (i = 0; i < leng_ext; i++) {
        if ((i == 0) &&
            (file[leng_file - leng_ext] != '.'))
            return(0);
        else if ((i > 0) &&
            (file[leng_file - leng_ext + i] != extension[i - 1]))
            return(0);
    }

    return(1);
}


/*****************************************************************************/

int  returnLockId( int  first_char,
                   int  table_id )

{
    switch (table_id) {
        case BUL_BRDS_TABLE_ID:
            return(1);
        case SUBSCRIBERS_TABLE_ID:
            return(2);
        case BB_MSGS_TABLE_ID:
            /* 0 thru 9 */
            if (first_char >= 48 && first_char <= 57)
                return(3);
            /* A thru E */
            else if (first_char >= 65 && first_char <= 69)
                return(4);
            /* F thru J */
            else if (first_char >= 70 && first_char <= 74)
                return(5);
            /* K thru O */
            else if (first_char >= 75 && first_char <= 79)
                return(6);
            /* P thru T */
            else if (first_char >= 80 && first_char <= 84)
                return(7);
            /* U thru Z */
            else if (first_char >= 85 && first_char <= 90)
                return(8);
            /* a thru e */
            else if (first_char >= 97 && first_char <= 101)
                return(9);
            /* f thru j */
            else if (first_char >= 102 && first_char <= 106)
                return(10);
            /* k thru o */
            else if (first_char >= 107 && first_char <= 111)
                return(11);
            /* p thru t */
            else if (first_char >= 112 && first_char <= 116)
                return(12);
            /* u thru z */
            else if (first_char >= 117 && first_char <= 122)
                return(13);
        case MARKERS_TABLE_ID:
            /* 0 thru 9 */
            if (first_char >= 48 && first_char <= 57)
                return(14);
            /* a thru e */
            else if (first_char >= 97 && first_char <= 101)
                return(15);
            /* f thru j */
            else if (first_char >= 102 && first_char <= 106)
                return(16);
            /* k thru o */
            else if (first_char >= 107 && first_char <= 111)
                return(17);
            /* p thru t */
            else if (first_char >= 112 && first_char <= 116)
                return(18);
            /* u thru z */
            else if (first_char >= 117 && first_char <= 122)
                return(19);
        default:
            return(-1);
    }
}


/*****************************************************************************/

void  returnDbFilename( int          first_char,
                        int          table_id,
                        char        *table,
                        const char  *db_dir )

{
    char ending[3];

    if (first_char >= 48 && first_char <= 57)
        strcpy(ending, "09");
    else if (first_char >= 65 && first_char <= 69)
        strcpy(ending, "AE");
    else if (first_char >= 70 && first_char <= 74)
        strcpy(ending, "FJ");
    else if (first_char >= 75 && first_char <= 79)
        strcpy(ending, "KO");
    else if (first_char >= 80 && first_char <= 84)
        strcpy(ending, "PT");
    else if (first_char >= 85 && first_char <= 90)
        strcpy(ending, "UZ");
    else if (first_char >= 97 && first_char <= 101)
        strcpy(ending, "ae");
    else if (first_char >= 102 && first_char <= 106)
        strcpy(ending, "fj");
    else if (first_char >= 107 && first_char <= 111)
        strcpy(ending, "ko");
    else if (first_char >= 112 && first_char <= 116)
        strcpy(ending, "pt");
    else if (first_char >= 117 && first_char <= 122)
        strcpy(ending, "uz");

    switch (table_id) {
        case BUL_BRDS_TABLE_ID:
            sprintf(table, "%s/%s", db_dir, BUL_BRDS_TABLE);
            break;
        case SUBSCRIBERS_TABLE_ID:
            sprintf(table, "%s/%s", db_dir, SUBSCRIBERS_TABLE);
            break;
        case BB_MSGS_TABLE_ID:
            sprintf(table, "%s/%s%s", db_dir, BB_MSGS_TABLE, ending);
            break;
        case MARKERS_TABLE_ID:
            sprintf(table, "%s/%s%s", db_dir, MARKERS_TABLE, ending);
            break;
    }
}


/*****************************************************************************/

static int  does_bb_exist( const char  *bulbrd,
                           const char  *db_dir )

{
  int    i, lock_id, flag;
  FILE  *rp;
  char   datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
  char   bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];

    if ((i = returnLockId(0, BUL_BRDS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, BUL_BRDS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    flag = ERROR_BB_DOES_NOT_EXIST;
    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (strcmp(bulbrd, bb_name) == 0) {
            flag = 1;
            break;
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    return(flag);
}


/*****************************************************************************/

static int  does_msg_exist( const char  *msg_id,
                            const char  *db_dir )

{
    char msgfile[MAX_FILENAME_LEN];

    getBulBrdMsgPath(msg_id, msgfile, db_dir);

    if (access(msgfile, F_OK) != -1)
        return(1);

    return(ERROR_BB_MSG_DOES_NOT_EXIST);
}


/*****************************************************************************/

static int  get_msg_title( const char  *msg_id,
                           const char  *db_dir,
                           char        *title )

{
    char user[MAX_LOGIN_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];
    char msgdir[MAX_FILENAME_LEN];
    int lock_id;
    FILE *fp;

    get_bb_lock_path(msg_id, msgdir, db_dir);
    if (!setFileLock(msgdir, &lock_id, LOCK_WAIT_TIME))
        return(0);

    if (isdigit((int)msg_id[DATETIME_LEN - 1])) {
        ncpy(user, &msg_id[DATETIME_LEN - 1], 3);
        sprintf(msgdir, "%s/%s/%s.title", db_dir, user, msg_id);
    } else {
        ncpy(msgid, msg_id, DATETIME_LEN - 1);
        strcpy(user, &msg_id[DATETIME_LEN - 1]);
        sprintf(msgdir, "%s/%s/%s.title", db_dir, user, msgid);
    }
    if ((fp = fopen(msgdir, "r")) == NULL) {
        unsetFileLock(lock_id);
        return(0);
    }
    if (fgets(title, MAX_MSG_TITLE_LEN, fp) == NULL) {
        fclose(fp);
        unsetFileLock(lock_id);
        return(0);
    }
    fclose(fp);
    unsetFileLock(lock_id);
    stripNewline(title);

    return(1);
}


/*****************************************************************************/

static void  copy_sent_msg( const char  *msg_id,
                            const char  *db_dir )

{
  int     i, lock_id;
  char    msgdir[MAX_FILENAME_LEN];
  char    sentfile[MAX_FILENAME_LEN];

  getBulBrdMsgPath(msg_id, sentfile, db_dir);
  if (access(sentfile, F_OK) != -1)
    return;

  get_bb_lock_path(msg_id, msgdir, db_dir);
  if (!setFileLock(msgdir, &lock_id, LOCK_WAIT_TIME))
    return;

  i = strlen(sentfile);
  ncpy(msgdir, sentfile, i - strlen("sent"));
  strcat(msgdir, "unsent");
  (void) rename(msgdir, sentfile);
  unsetFileLock(lock_id);
}


/*****************************************************************************/

static int  get_num_unread_msgs( const char  *login_name,
                                 const char  *bulbrd,
                                 const char  *db_dir )

{
    int i, num_msgs_read, lock_id, num_bb_msgs;
    int max_bb_msgs = MAX_ARRAY_SIZE;
    FILE *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];
    char **msg_ids, user[MAX_LOGIN_NAME_LEN+1];

    if ((i = does_bb_exist(bulbrd, db_dir)) < 0)
        return(-1);

    if ((i = returnLockId((int) bulbrd[0], BB_MSGS_TABLE_ID)) < 0)
        return(-1);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(-1);

    returnDbFilename((int) bulbrd[0], BB_MSGS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(-1);
    }

    num_msgs_read = num_bb_msgs = 0;
    msg_ids = (char **) malloc(sizeof(char *)*max_bb_msgs);

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(bb_name, item, MAX_BB_NAME_LEN);
        trim(bb_name);
        ncpy(msgid, &item[MAX_BB_NAME_LEN + 1], MAX_MSG_ID_LEN);
        trim(msgid);
        if (strcmp(bb_name, bulbrd) == 0) {
            msg_ids[num_bb_msgs] = allocAndCopyString(msgid);
            num_bb_msgs++;
        }

        if (num_bb_msgs >= max_bb_msgs) {
            max_bb_msgs += MAX_ARRAY_SIZE;
            msg_ids = (char **) realloc(msg_ids, sizeof(char *)*max_bb_msgs);
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    if ((i = returnLockId((int) login_name[0], MARKERS_TABLE_ID)) < 0) {
        for (i = num_bb_msgs - 1; i >= 0; i--)
            free((char *) msg_ids[i]);
        free((char *) msg_ids);
        return(-1);
    }
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0) {
        for (i = num_bb_msgs - 1; i >= 0; i--)
            free((char *) msg_ids[i]);
        free((char *) msg_ids);
        return(-1);
    }

    returnDbFilename((int) login_name[0], MARKERS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        for (i = num_bb_msgs - 1; i >= 0; i--)
            free((char *) msg_ids[i]);
        free((char *) msg_ids);
        unsetReadDBLock(lock_id);
        return(-1);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(msgid, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(msgid);
        if (strcmp(user, login_name) == 0) {
            for (i = 0; i < num_bb_msgs; i++) {
                if (strcmp(msg_ids[i], msgid) == 0) {
                    num_msgs_read++;
                    break;
                }
            }
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    for (i = num_bb_msgs - 1; i >= 0; i--)
        free((char *) msg_ids[i]);
    free((char *) msg_ids);

    return(num_bb_msgs - num_msgs_read);
}


/*****************************************************************************/
/* For parameter all_users; 0 means only user sent msg, 1 means all users
   sent msg.
*/

int  sendSystemBulBrdMsg( const char  *user,
                          const char  *app,
                          int          sys_bb_id,
                          const char  *sys_msg,
                          const char  *db_dir,
                          char        *ret_extension,
                          int         *all_users )

{
  char   datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
  char   msgid[MAX_MSG_ID_LEN+1], datadir[MAX_FILENAME_LEN];
  char   title[MAX_MSG_TITLE_LEN+1];
  int    i, lock_id;
  FILE  *fp;

  switch (sys_bb_id)
    {
    case SYS_MSG_BB_PROCESS_TERMINATED:
            strcpy(item, "Server process ended");   /* titles must be <= 24 */
            strcpy(ret_extension, "all");
            *all_users = 0;
            break;
        case SYS_MSG_BB_PRT_JOB_DELETED:
            strcpy(item, "Print request deleted");
            strcpy(ret_extension, "all");
            *all_users = 0;
            break;
        case SYS_MSG_BB_PRT_JOB_RELEASED:
            strcpy(item, "Print request released");
            strcpy(ret_extension, "all");
            *all_users = 0;
            break;
        default:
            strcpy(item, "System message");
            strcpy(ret_extension, "adm");
            *all_users = 0;
            break;
    }
    sprintf(datadir, "%%-%ds%%s", MAX_LOGIN_NAME_LEN);
    if (*all_users)
        sprintf(title, datadir, "ALL USRS", item);
    else
        sprintf(title, datadir, user, item);

    sprintf(datadir, "%s/%3.3d/lock_file", db_dir, sys_bb_id);
    if (!setFileLock(datadir, &lock_id, LOCK_WAIT_TIME))
        return(ERROR_CANT_ACCESS_DB);

    i = 0;
    getFullTimeStr(msgid);
    do {
        sprintf(item, "%s%3.3d%3.3d", msgid, sys_bb_id, i);
        sprintf(datafile, "%s/%3.3d/%s.title", db_dir, sys_bb_id, item);
        i++;
    } while (access(datafile, F_OK) != -1);

    if ((fp = fopen(datafile, "w")) == NULL) {
        unsetFileLock(lock_id);
        return(ERROR_CANT_OPEN_FILE);
    }
    fprintf(fp, "%s\n", title);
    fclose(fp);

    sprintf(datadir, "%s/%3.3d/%s.sent", db_dir, sys_bb_id, item);
    if ((fp = fopen(datadir, "w")) == NULL) {
        (void) unlink(datafile);
        unsetFileLock(lock_id);
        return(ERROR_CANT_OPEN_FILE);
    }
    fprintf(fp, "%s\n", sys_msg);
    fclose(fp);
    unsetFileLock(lock_id);

    sprintf(datafile, "%s.%s", app, ret_extension);

    return(sendBulBrdMsg(item, datafile, db_dir));
}


/*****************************************************************************/

int  sendBulBrdMsg( const char  *msg_id,
                    const char  *bulbrd,
                    const char  *db_dir )

{
    int i, lock_id, flag;
    FILE *fp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];
    char title[MAX_MSG_TITLE_LEN+1];

    copy_sent_msg(msg_id, db_dir);

    if ((i = does_bb_exist(bulbrd, db_dir)) < 0)
        return(i);
    if ((i = does_msg_exist(msg_id, db_dir)) < 0)
        return(i);

    if ((i = returnLockId((int) bulbrd[0], BB_MSGS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename((int) bulbrd[0], BB_MSGS_TABLE_ID, datafile, db_dir);
    if (((fp = fopen(datafile, "r+")) == NULL) ||
        !get_msg_title(msg_id, db_dir, title)) {
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }
    flag = 1;
    while (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
        stripNewline(item);
        ncpy(bb_name, item, MAX_BB_NAME_LEN);
        trim(bb_name);
        ncpy(msgid, &item[MAX_BB_NAME_LEN + 1], MAX_MSG_ID_LEN);
        trim(msgid);
        if ((strcmp(msgid, msg_id) == 0) &&
            (strcmp(bb_name, bulbrd) == 0)) {
            flag = 0;
            break;
        }
    }
    if (flag) {
        sprintf(item, "%%-%ds %%-%ds %%s", MAX_BB_NAME_LEN, MAX_MSG_ID_LEN);
        fprintf(fp, item, bulbrd, msg_id, title);
        fprintf(fp, "\n");
    }
    fclose(fp);
    unsetWriteDBLock(lock_id);

    return(1);
}


/*****************************************************************************/
/* For parameter sub_flag; 0 is unsubscribe, 1 is subscribe.
*/

int  subscribeBulBrd( const char  *bulbrd,
                      const char  *login_name,
                      int          sub_flag,
                      const char  *db_dir )

{
    int i, lock_id;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];
    char tmp_file[MAX_FILENAME_LEN];

    if ((i = does_bb_exist(bulbrd, db_dir)) < 0)
        return(i);

    if ((i = returnLockId(0, SUBSCRIBERS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, db_dir);
    sprintf(tmp_file, "%s.tmp", datafile);

    if ((fp = fopen(tmp_file, "w")) == NULL) {
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }
    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetWriteDBLock(lock_id);
        fclose(fp);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if ((strcmp(user, login_name) != 0) ||
            (strcmp(bb_name, bulbrd) != 0))
            fprintf(fp, "%s\n", item);
    }

    if (sub_flag) {
        sprintf(item, "%%-%ds %%s", MAX_LOGIN_NAME_LEN);
        fprintf(fp, item, login_name, bulbrd);
        fprintf(fp, "\n");
    }
    fclose(rp);
    fclose(fp);
    (void) rename(tmp_file, datafile);
    (void) chmod(datafile, S_IRUSR | S_IWUSR);
    unsetWriteDBLock(lock_id);

    return(1);
}


/*****************************************************************************/

int  getSubscribedBulBrds( const char  *login_name,
                           char        *query,
                           char        *msg,
                           const char  *db_dir)

{
    int i, leng, lock_id, num_bbs = 0;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];
    char tmp_file[MAX_FILENAME_LEN];

    if ((i = returnLockId(0, SUBSCRIBERS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    if ((fp = fopen(query, "w")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_CREATE_QUERY);
    }
    fclose(fp);

    returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, db_dir);

    i = 0;
    do {
        sprintf(tmp_file, "%s.%d", datafile, i);
    } while (access(tmp_file, F_OK) != -1);

    sprintf(item, "/bin/cp %s %s", datafile, tmp_file);
    if (system(item) != 0) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }
    unsetReadDBLock(lock_id);

    if ((rp = fopen(tmp_file, "r")) == NULL) {
        (void) unlink(tmp_file);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    leng = 0;
    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (strcmp(user, login_name) == 0) {
            leng += bundleQuery(query, "CI", bb_name,
                            get_num_unread_msgs(login_name, bb_name, db_dir));
            num_bbs++;
        }
    }
    fclose(rp);
    (void) unlink(tmp_file);

    sprintf(user, "%d", num_bbs);
    sprintf(item, "%d", leng + strlen(user) + 1);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    sprintf(item, "%d", num_bbs);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    return(leng);   /* everthing ok, bulletin board data is returned */
}


/*****************************************************************************/

int  getAllBulBrds( const char  *login_name,
                    const char  *query,
                    char        *msg,
                    const char  *db_dir )

{
    int i, leng, lock_id, num_bbs = 0;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];

    if ((i = returnLockId(0, BUL_BRDS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, BUL_BRDS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    if ((fp = fopen(query, "w")) == NULL) {
        fclose(rp);
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_CREATE_QUERY);
    }

    leng = 0;
    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (strcmp(bb_name, login_name) == 0)
            continue;
        num_bbs++;
        i = strlen(bb_name);
        fprintf(fp, "%5.5d%s", i, bb_name);
        leng += i + 5;
    }
    fclose(rp);
    fclose(fp);
    unsetReadDBLock(lock_id);

    sprintf(user, "%d", num_bbs);
    sprintf(item, "%d", leng + strlen(user) + 1);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    sprintf(item, "%d", num_bbs);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    return(leng);   /* everthing ok, bulletin board data is returned */
}


/*****************************************************************************/

int  getUnsubscribedBulBrds( const char  *login_name,
                             char        *query,
                             char        *msg,
                             const char  *db_dir )

{
    int i, leng, lock_id, num_bbs, num_unsub_bbs = 0;
    int max_bbs = MAX_ARRAY_SIZE;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];
    char **bb_names;
    int *bb_markers;

    if ((i = returnLockId(0, BUL_BRDS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, BUL_BRDS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    num_bbs = 0;
    bb_names = (char **) malloc(sizeof(char *)*max_bbs);
    bb_markers = (int *) malloc(sizeof(int)*max_bbs);

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (getBulBrdType(bb_name) != MSG_BB_TYPE_GENERAL)
            continue;
        bb_names[num_bbs] = allocAndCopyString(bb_name);
        bb_markers[num_bbs] = 1;
        num_bbs++;

        if (num_bbs >= max_bbs) {
            max_bbs += MAX_ARRAY_SIZE;
            bb_names = (char **) realloc(bb_names, sizeof(char *)*max_bbs);
            bb_markers = (int *) realloc(bb_markers, sizeof(int)*max_bbs);
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    if ((i = returnLockId(0, SUBSCRIBERS_TABLE_ID)) < 0) {
        for (i = num_bbs - 1; i >= 0; i--)
            free((char *) bb_names[i]);
        free((char *) bb_names);
        free((char *) bb_markers);
        return(ERROR_CANT_ACCESS_DB);
    }
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0) {
        for (i = num_bbs - 1; i >= 0; i--)
            free((char *) bb_names[i]);
        free((char *) bb_names);
        free((char *) bb_markers);
        return(ERROR_CANT_ACCESS_DB);
    }

    if ((fp = fopen(query, "w")) == NULL) {
        for (i = num_bbs - 1; i >= 0; i--)
            free((char *) bb_names[i]);
        free((char *) bb_names);
        free((char *) bb_markers);
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_CREATE_QUERY);
    }
    fclose(fp);

    returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        for (i = num_bbs - 1; i >= 0; i--)
            free((char *) bb_names[i]);
        free((char *) bb_names);
        free((char *) bb_markers);
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if ((strcmp(user, login_name) == 0) && (strcmp(user, bb_name) != 0)) {
            for (i = 0; i < num_bbs; i++) {
                if (strcmp(bb_names[i], bb_name) == 0) {
                    bb_markers[i] = 0;
                    break;
                }
            }
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    leng = 0;
    for (i = 0; i < num_bbs; i++) {
        if (bb_markers[i]) {
            leng += bundleQuery(query, "C", bb_names[i]);
            num_unsub_bbs++;
        }
    }

    for (i = num_bbs - 1; i >= 0; i--)
        free((char *) bb_names[i]);
    free((char *) bb_names);
    free((char *) bb_markers);

    sprintf(user, "%d", num_unsub_bbs);
    sprintf(item, "%d", leng + strlen(user) + 1);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    sprintf(item, "%d", num_unsub_bbs);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    return(leng);   /* everthing ok, bulletin board data is returned */
}


/*****************************************************************************/

int  getOwnBulBrdMsgs( const char  *login_name,
                       char        *query,
                       char        *msg,
                       const char  *db_dir )

{
    int i, leng, lock_id, num_bb_msgs;
    char datafile[MAX_FILENAME_LEN], datadir[MAX_FILENAME_LEN];
    char msgid[MAX_MSG_ID_LEN+1], title[MAX_MSG_TITLE_LEN+1];
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;
    FILE *fp, *rp;

    if ((fp = fopen(query, "w")) == NULL)
        return(ERROR_CANT_CREATE_QUERY);

    sprintf(datafile, "%s/%s/lock_file", db_dir, login_name);
    if (!setFileLock(datafile, &lock_id, LOCK_WAIT_TIME)) {
        fclose(fp);
        return(ERROR_CANT_ACCESS_DB);
    }

    leng = num_bb_msgs = 0;
    sprintf(datadir, "%s/%s", db_dir, login_name);
    if ((dp = opendir(datadir)) == NULL) {
        fclose(fp);
        unsetFileLock(lock_id);
        return(ERROR_CANT_ACCESS_DB);
    }
    while ((direc = readdir(dp)) != NULL) {
        if (direc->d_ino == 0)
            continue;
        if (strcmp(direc->d_name, ".") == 0 ||
            strcmp(direc->d_name, "..") == 0)
            continue;
        sprintf(datafile, "%s/%s", datadir, direc->d_name);
        if (stat(datafile, &sbuf) == -1)
            continue;
        if ((sbuf.st_mode & S_IFMT) != S_IFREG)
            continue;

        if (does_extension_match(direc->d_name, "unsent")) {
            ncpy(msgid, direc->d_name,
                    strlen(direc->d_name) - strlen(".unsent"));
            i = 0;
        } else if (does_extension_match(direc->d_name, "sent")) {
            ncpy(msgid, direc->d_name,
                    strlen(direc->d_name) - strlen(".sent"));
            i = 1;
        } else
            continue;

        sprintf(datafile, "%s/%s.title", datadir, msgid);
        if ((rp = fopen(datafile, "r")) != NULL) {
            if (fgets(title, MAX_MSG_TITLE_LEN, rp) != NULL) {
                stripNewline(title);
                leng += bundleQuery(query, "CCI", msgid, title, i);
                num_bb_msgs++;
            }
            fclose(rp);
        }
    }
    closedir(dp);
    fclose(fp);
    unsetFileLock(lock_id);

    sprintf(datafile, "%d", num_bb_msgs);
    sprintf(datadir, "%d", leng + strlen(datafile) + 1);
    sprintf(datafile, "%1d", strlen(datadir));
    strcat(msg, datafile);
    strcat(msg, datadir);

    sprintf(datadir, "%d", num_bb_msgs);
    sprintf(datafile, "%1d", strlen(datadir));
    strcat(msg, datafile);
    strcat(msg, datadir);

    return(leng);   /* everthing ok, message data is returned */
}


/*****************************************************************************/

int  getBulBrdMsgs( const char  *login_name,
                    const char  *bulbrd,
                    char        *query,
                    char        *msg,
                    const char  *db_dir )

{
    int i, leng, lock_id, num_bb_msgs;
    int max_bb_msgs = MAX_ARRAY_SIZE;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];
    char **msg_ids, **titles, user[MAX_LOGIN_NAME_LEN+1];
    int *is_reads, is_sysbb = 0;

    if ((i = does_bb_exist(bulbrd, db_dir)) < 0)
        return(i);

    if (getBulBrdType(bulbrd) == MSG_BB_TYPE_SYSTEM)
        is_sysbb = 1;

    if ((i = returnLockId((int) bulbrd[0], BB_MSGS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename((int) bulbrd[0], BB_MSGS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    num_bb_msgs = 0;
    msg_ids = (char **) malloc(sizeof(char *)*max_bb_msgs);
    titles = (char **) malloc(sizeof(char *)*max_bb_msgs);
    is_reads = (int *) malloc(sizeof(int)*max_bb_msgs);

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(bb_name, item, MAX_BB_NAME_LEN);
        trim(bb_name);
        ncpy(msgid, &item[MAX_BB_NAME_LEN + 1], MAX_MSG_ID_LEN);
        trim(msgid);
        if (strcmp(bb_name, bulbrd) == 0) {
            if (is_sysbb) {
                if (isdigit((int)msgid[DATETIME_LEN - 1])) {
                    ncpy(user, &item[MAX_BB_NAME_LEN + MAX_MSG_ID_LEN + 2],
                                                    MAX_LOGIN_NAME_LEN);
                    trim(user);
                    if ((strcmp(user, "ALL USRS") == 0) ||
                        (strcmp(login_name, user) == 0))
                        titles[num_bb_msgs] =
                            allocAndCopyString(&item[MAX_BB_NAME_LEN +
                                    MAX_MSG_ID_LEN + 2 + MAX_LOGIN_NAME_LEN]);
                    else
                        continue;
                } else {
                    titles[num_bb_msgs] =
                allocAndCopyString(&item[MAX_BB_NAME_LEN + MAX_MSG_ID_LEN + 2]);
                }
            } else {
                titles[num_bb_msgs] =
                    allocAndCopyString(&item[MAX_BB_NAME_LEN + MAX_MSG_ID_LEN + 2]);
            }
            msg_ids[num_bb_msgs] = allocAndCopyString(msgid);
            is_reads[num_bb_msgs] = 0;
            num_bb_msgs++;
        }

        if (num_bb_msgs >= max_bb_msgs) {
            max_bb_msgs += MAX_ARRAY_SIZE;
            msg_ids = (char **) realloc(msg_ids, sizeof(char *)*max_bb_msgs);
            titles = (char **) realloc(titles, sizeof(char *)*max_bb_msgs);
            is_reads = (int *) realloc(is_reads, sizeof(int)*max_bb_msgs);
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    if ((i = returnLockId((int) login_name[0], MARKERS_TABLE_ID)) < 0) {
        for (i = num_bb_msgs - 1; i >= 0; i--) {
            free((char *) msg_ids[i]);
            free((char *) titles[i]);
        }
        free((char *) msg_ids);
        free((char *) titles);
        free((char *) is_reads);
        return(ERROR_CANT_ACCESS_DB);
    }
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0) {
        for (i = num_bb_msgs - 1; i >= 0; i--) {
            free((char *) msg_ids[i]);
            free((char *) titles[i]);
        }
        free((char *) msg_ids);
        free((char *) titles);
        free((char *) is_reads);
        return(ERROR_CANT_ACCESS_DB);
    }

    if ((fp = fopen(query, "w")) == NULL) {
        for (i = num_bb_msgs - 1; i >= 0; i--) {
            free((char *) msg_ids[i]);
            free((char *) titles[i]);
        }
        free((char *) msg_ids);
        free((char *) titles);
        free((char *) is_reads);
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_CREATE_QUERY);
    }
    fclose(fp);

    returnDbFilename((int) login_name[0], MARKERS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        for (i = num_bb_msgs - 1; i >= 0; i--) {
            free((char *) msg_ids[i]);
            free((char *) titles[i]);
        }
        free((char *) msg_ids);
        free((char *) titles);
        free((char *) is_reads);
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(msgid, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(msgid);
        if (strcmp(user, login_name) == 0) {
            for (i = 0; i < num_bb_msgs; i++) {
                if (strcmp(msg_ids[i], msgid) == 0) {
                    is_reads[i] = 1;
                    break;
                }
            }
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    leng = 0;
    for (i = 0; i < num_bb_msgs; i++)
        leng += bundleQuery(query, "CCI", msg_ids[i], titles[i], is_reads[i]); 

    for (i = num_bb_msgs - 1; i >= 0; i--) {
        free((char *) msg_ids[i]);
        free((char *) titles[i]);
    }
    free((char *) msg_ids);
    free((char *) titles);
    free((char *) is_reads);

    sprintf(user, "%d", num_bb_msgs);
    sprintf(item, "%d", leng + strlen(user) + 1);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    sprintf(item, "%d", num_bb_msgs);
    sprintf(user, "%1d", strlen(item));
    strcat(msg, user);
    strcat(msg, item);

    return(leng);   /* everthing ok, message data is returned */
}


/*****************************************************************************/

int  markMsgRead( const char  *login_name,
                  const char  *msg_id,
                  const char  *db_dir )

{
    int i, lock_id, flag;
    FILE *fp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char user[MAX_LOGIN_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];

    if ((i = does_msg_exist(msg_id, db_dir)) < 0)
        return(i);

    if ((i = returnLockId((int) login_name[0], MARKERS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename((int) login_name[0], MARKERS_TABLE_ID, datafile, db_dir);

    if ((fp = fopen(datafile, "r+")) == NULL) {
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    flag = 1;
    while (fgets(item, MAX_ITEM_LEN, fp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(msgid, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(msgid);
        if ((strcmp(msgid, msg_id) == 0) &&
            (strcmp(user, login_name) == 0)) {
            flag = 0;
            break;
        }
    }

    if (flag) {
        sprintf(item, "%%-%ds %%s", MAX_LOGIN_NAME_LEN);
        fprintf(fp, item, login_name, msg_id);
        fprintf(fp, "\n");
    }
    fclose(fp);
    unsetWriteDBLock(lock_id);

    return(1);
}


/*****************************************************************************/

int  createMsgFile( const char  *msg_id,
                    const char  *title,
                    const char  *db_dir )

{
    char user[MAX_LOGIN_NAME_LEN+1], msgid[MAX_MSG_ID_LEN+1];
    char msgdir[MAX_FILENAME_LEN], newfile[MAX_FILENAME_LEN];
    int lock_id;
    FILE *fp;

    ncpy(msgid, msg_id, DATETIME_LEN - 1);
    strcpy(user, &msg_id[DATETIME_LEN - 1]);
    sprintf(msgdir, "%s/%s/lock_file", db_dir, user);
    if (!setFileLock(msgdir, &lock_id, LOCK_WAIT_TIME))
        return(ERROR_CANT_ACCESS_DB);

    sprintf(newfile, "%s/%s/%s.new", db_dir, user, msgid);
    if (access(newfile, F_OK) == -1) {
        syslog(LOG_WARNING, "Failed to access file %s.", newfile);
        unsetFileLock(lock_id);
        return(ERROR_CANT_OPEN_FILE);
    }

    sprintf(msgdir, "%s/%s/%s.title", db_dir, user, msgid);
    if ((fp = fopen(msgdir, "w")) == NULL) {
        syslog(LOG_WARNING, "Failed to create file %s.", msgdir);
        unsetFileLock(lock_id);
        return(ERROR_CANT_OPEN_FILE);
    }
    fprintf(fp, "%s\n", title);
    fclose(fp);

    sprintf(msgdir, "%s/%s/%s.unsent", db_dir, user, msgid);
    if (rename(newfile, msgdir) < 0) {
        sprintf(msgdir, "%s/%s/%s.title", db_dir, user, msgid);
        (void) unlink(msgdir);
        unsetFileLock(lock_id);
        return(ERROR_CANT_RENAME_FILE);
    }
    unsetFileLock(lock_id);

    return(1);
}


/*****************************************************************************/

int  isUserSubscribed( const char  *login_name,
                       const char  *bulbrd,
                       const char  *db_dir )

{
    int i, lock_id;
    FILE *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];

    if ((i = does_bb_exist(bulbrd, db_dir)) < 0)
        return(i);

    if ((i = returnLockId(0, SUBSCRIBERS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setReadDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, db_dir);

    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetReadDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if ((strcmp(user, login_name) == 0) && (strcmp(bb_name, bulbrd) == 0)) {
            fclose(rp);
            unsetReadDBLock(lock_id);
            return(1);
        }
    }
    fclose(rp);
    unsetReadDBLock(lock_id);

    return(0);
}


/*****************************************************************************/

int  getBulBrdType( const char *bulbrd )

{
    int flag = 0;
    const char *s = bulbrd;

    while (*s) {
        if (isupper((int)*s))
            flag = 1;
        if (*s == '.')
            return(MSG_BB_TYPE_SYSTEM);
        s++;
    }
    if (flag)
        return(MSG_BB_TYPE_GENERAL);

    return(MSG_BB_TYPE_USER);
}


/*****************************************************************************/

void  cleanUpOldBulBrdMsgs( int          period,        /* in days */
                            const char  *msg_dir,
                            const char  *db_dir )
{
    int i, n, lock_id, num_users;
    char later_time[DATETIME_LEN];
    char current_time[DATETIME_LEN];
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char tmp_file[MAX_FILENAME_LEN], msgid[MAX_MSG_ID_LEN+1];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];
    static const char  *first_chars = "0afkpuAFKPU";
    FILE *fp, *rp;
    UserItem **Users;
    DIR *dp;
#ifdef SOLARIS
    struct dirent *direc;
#else
    struct direct *direc;
#endif
    struct stat sbuf;

    getFullTimeStr(current_time);
/*
 * Clean out bulletin board message table.
 */
    for (n = 0; n < 11; n++) {
        if ((i = returnLockId((int) first_chars[n], BB_MSGS_TABLE_ID)) < 0)
            continue;
        if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
            continue;
        returnDbFilename((int) first_chars[n],
                                    BB_MSGS_TABLE_ID, datafile, msg_dir);
        sprintf(tmp_file, "%s.tmp", datafile);

        if ((fp = fopen(tmp_file, "w")) == NULL) {
            unsetWriteDBLock(lock_id);
            continue;
        }
        if ((rp = fopen(datafile, "r")) == NULL) {
            unsetWriteDBLock(lock_id);
            fclose(fp);
            continue;
        }
        while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
            stripNewline(item);
            ncpy(bb_name, item, MAX_BB_NAME_LEN);
            trim(bb_name);
            ncpy(msgid, &item[MAX_BB_NAME_LEN + 1], MAX_MSG_ID_LEN);
            trim(msgid);
            msgid[DATETIME_LEN - 1] = 0;

            if (!getNextFullTimeStr(msgid, later_time, DAY_TYPE, period)) {
                fprintf(fp, "%s\n", item);
                syslog(LOG_WARNING,
            "Failed to determine future time for cleaning up old messages.");
                continue;
            }
            if (fullTimeStrCmp(later_time, current_time) < 0)
                fprintf(fp, "%s\n", item);
        }
        fclose(rp);
        fclose(fp);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);
        unsetWriteDBLock(lock_id);
    }
/*
 * Clean out markers table.
 */
    for (n = 0; n < 6; n++) {
        if ((i = returnLockId((int) first_chars[n], MARKERS_TABLE_ID)) < 0)
            continue;
        if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
            continue;
        returnDbFilename((int) first_chars[n],
                                    MARKERS_TABLE_ID, datafile, msg_dir);
        sprintf(tmp_file, "%s.tmp", datafile);

        if ((fp = fopen(tmp_file, "w")) == NULL) {
            unsetWriteDBLock(lock_id);
            continue;
        }
        if ((rp = fopen(datafile, "r")) == NULL) {
            unsetWriteDBLock(lock_id);
            fclose(fp);
            continue;
        }
        while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
            stripNewline(item);
            ncpy(user, item, MAX_LOGIN_NAME_LEN);
            trim(user);
            strcpy(msgid, &item[MAX_LOGIN_NAME_LEN + 1]);
            trim(msgid);
            msgid[DATETIME_LEN - 1] = 0;

            if (!getNextFullTimeStr(msgid, later_time, DAY_TYPE, period)) {
                fprintf(fp, "%s\n", item);
                syslog(LOG_WARNING,
            "Failed to determine future time for cleaning up old messages.");
                continue;
            }
            if (fullTimeStrCmp(later_time, current_time) < 0)
                fprintf(fp, "%s\n", item);
        }
        fclose(rp);
        fclose(fp);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);
        unsetWriteDBLock(lock_id);
    }
/*
 * Clean out user directories.
 */
    if ((lock_id = setReadDBLock(MAIN_DB_RW_KEY, LOCK_WAIT_TIME)) >= 0) {
        Users = getUserData(&num_users, db_dir);
        unsetReadDBLock(lock_id);

        for (i = 0; i < num_users; i++) {
            sprintf(datafile, "%s/%s/lock_file", msg_dir, Users[i]->login_name);
            if (!setFileLock(datafile, &lock_id, LOCK_WAIT_TIME))
                continue;
            sprintf(datafile, "%s/%s", msg_dir, Users[i]->login_name);
            if ((dp = opendir(datafile)) == NULL) {
                unsetFileLock(lock_id);
                continue;
            }
            while ((direc = readdir(dp)) != NULL) {
                if (direc->d_ino == 0)
                    continue;
                if (strcmp(direc->d_name, ".") == 0 ||
                    strcmp(direc->d_name, "..") == 0)
                    continue;
                sprintf(tmp_file, "%s/%s", datafile, direc->d_name);
                if (stat(tmp_file, &sbuf) == -1)
                    continue;
                if ((sbuf.st_mode & S_IFMT) != S_IFREG)
                    continue;
                if (!does_extension_match(direc->d_name, "sent"))
                    continue;
                if (strlen(direc->d_name) < (DATETIME_LEN - 1))
                    continue;
                ncpy(msgid, direc->d_name, DATETIME_LEN - 1);

                if (!getNextFullTimeStr(msgid, later_time, DAY_TYPE, period))
                    continue;
                if (fullTimeStrCmp(later_time, current_time) >= 0) {
                    (void) unlink(tmp_file);
                    ncpy(msgid, direc->d_name,
                                    strlen(direc->d_name) - strlen(".sent"));
                    sprintf(tmp_file, "%s/%s%s", datafile, msgid, ".title");
                    (void) unlink(tmp_file);
                }
            }
            closedir(dp);
            unsetFileLock(lock_id);
        }
        freeUserData(Users, num_users);
    } else
        syslog(LOG_WARNING,
    "Failed to obtain main read lock for cleaning up user directories.");
/*
 * Clean out system message directories.
 */
    for (i = 1; i <= NUM_SYSTEM_MSG_BBS; i++) {
        sprintf(datafile, "%s/%3.3d/lock_file", msg_dir, i);
        if (!setFileLock(datafile, &lock_id, LOCK_WAIT_TIME))
            continue;
        sprintf(datafile, "%s/%3.3d", msg_dir, i);
        if ((dp = opendir(datafile)) == NULL) {
            unsetFileLock(lock_id);
            continue;
        }
        while ((direc = readdir(dp)) != NULL) {
            if (direc->d_ino == 0)
                continue;
            if (strcmp(direc->d_name, ".") == 0 ||
                strcmp(direc->d_name, "..") == 0)
                continue;
            sprintf(tmp_file, "%s/%s", datafile, direc->d_name);
            if (stat(tmp_file, &sbuf) == -1)
                continue;
            if ((sbuf.st_mode & S_IFMT) != S_IFREG)
                continue;
            if (!does_extension_match(direc->d_name, "sent"))
                continue;
            if (strlen(direc->d_name) < (DATETIME_LEN - 1))
                continue;
            ncpy(msgid, direc->d_name, DATETIME_LEN - 1);

            if (!getNextFullTimeStr(msgid, later_time, DAY_TYPE, period))
                continue;
            if (fullTimeStrCmp(later_time, current_time) >= 0) {
                (void) unlink(tmp_file);
                ncpy(msgid, direc->d_name,
                                strlen(direc->d_name) - strlen(".sent"));
                sprintf(tmp_file, "%s/%s%s", datafile, msgid, ".title");
                (void) unlink(tmp_file);
            }
        }
        closedir(dp);
        unsetFileLock(lock_id);
    }
    syslog(LOG_WARNING,
    "Finished cleaning up bulletin board messages older than %d days.",
                        period);
}


/*****************************************************************************/

void  getBulBrdMsgPath( const char  *msg_id,
                        char        *path,
                        const char  *db_dir )
{
  char  user[MAX_LOGIN_NAME_LEN + 1];
  char  msgid[MAX_MSG_ID_LEN + 1];

  if (isdigit((int)msg_id[DATETIME_LEN - 1]))
    {
      ncpy(user, &msg_id[DATETIME_LEN - 1], 3);
      sprintf(path, "%s/%s/%s.sent", db_dir, user, msg_id);
    }
  else
    {
      ncpy(msgid, msg_id, DATETIME_LEN - 1);
      strcpy(user, &msg_id[DATETIME_LEN - 1]);
      sprintf(path, "%s/%s/%s.sent", db_dir, user, msgid);
    }
}


/*****************************************************************************/

static void  get_bb_lock_path( const char  *msg_id,
                               char        *path,
                               const char  *db_dir)

{
    char user[MAX_LOGIN_NAME_LEN+1];

    if (isdigit((int)msg_id[DATETIME_LEN - 1]))
        ncpy(user, &msg_id[DATETIME_LEN - 1], 3);
    else
        strcpy(user, &msg_id[DATETIME_LEN - 1]);
    sprintf(path, "%s/%s/lock_file", db_dir, user);
}


/*****************************************************************************/

static int  is_bb_name_valid( const char  *bulbrd )

{
    int   flag = 0;
    const char *s = bulbrd;

    while (*s) {
        if (isupper((int)*s))
            flag = 1;
        else if (!(isalnum((int)*s) || (*s == '_') || (*s == '-') || (*s == ' ')))
            return(0);
        s++;
    }

    return(flag);
}


/*****************************************************************************/

int  changeBulBrd( const char  *login_name,
                   const char  *bulbrd,
                   const char  *db_dir,
                   int          which )   /* 0 is add, 1 is delete */

{
    int i, lock_id, flag;
    FILE *fp, *rp;
    char datafile[MAX_FILENAME_LEN], item[MAX_ITEM_LEN];
    char bb_name[MAX_BB_NAME_LEN+1], user[MAX_LOGIN_NAME_LEN+1];
    char tmp_file[MAX_FILENAME_LEN];

    if (!is_bb_name_valid(bulbrd))
        return(ERROR_INVALID_BB_NAME);

    if ((i = returnLockId(0, BUL_BRDS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, BUL_BRDS_TABLE_ID, datafile, db_dir);
    sprintf(tmp_file, "%s.tmp", datafile);

    if ((fp = fopen(tmp_file, "w")) == NULL) {
        syslog(LOG_WARNING, "Failed to create file %s.", tmp_file);
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }
    if ((rp = fopen(datafile, "r")) == NULL) {
        syslog(LOG_WARNING, "Failed to read file %s.", datafile);
        unsetWriteDBLock(lock_id);
        fclose(fp);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    flag = 0;
    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (strcmp(bulbrd, bb_name) == 0)
            flag = 1;
        else
            fprintf(fp, "%s\n", item);
    }
    fclose(rp);

    switch (which) {
        case 0:
            if (flag) {
                fclose(fp);
                (void) unlink(tmp_file);
                flag = ERROR_BB_ALREADY_EXISTS;
            } else {
                sprintf(item, "%%-%ds %%s", MAX_LOGIN_NAME_LEN);
                fprintf(fp, item, login_name, bulbrd);
                fprintf(fp, "\n");
                flag = 1;
            }
            break;

        case 1:
            if (!flag) {
                fclose(fp);
                (void) unlink(tmp_file);
                flag = ERROR_BB_DOES_NOT_EXIST;
            }
            break;
    }
    if (flag == 1) {
        fclose(fp);
        (void) rename(tmp_file, datafile);
        (void) chmod(datafile, S_IRUSR | S_IWUSR);
    }
    unsetWriteDBLock(lock_id);

    if ((which == 0) || (flag != 1))
        return(flag);
/*
 * Remove all subscribers to deleted bulletin board.
 */
    if ((i = returnLockId(0, SUBSCRIBERS_TABLE_ID)) < 0)
        return(ERROR_CANT_ACCESS_DB);
    if ((lock_id = setWriteDBLock(i + MSG_BUL_BRD_KEY, LOCK_WAIT_TIME)) < 0)
        return(ERROR_CANT_ACCESS_DB);

    returnDbFilename(0, SUBSCRIBERS_TABLE_ID, datafile, db_dir);
    sprintf(tmp_file, "%s.tmp", datafile);

    if ((fp = fopen(tmp_file, "w")) == NULL) {
        unsetWriteDBLock(lock_id);
        return(ERROR_CANT_OPEN_DB_FILE);
    }
    if ((rp = fopen(datafile, "r")) == NULL) {
        unsetWriteDBLock(lock_id);
        fclose(fp);
        return(ERROR_CANT_OPEN_DB_FILE);
    }

    while (fgets(item, MAX_ITEM_LEN, rp) != NULL) {
        stripNewline(item);
        ncpy(user, item, MAX_LOGIN_NAME_LEN);
        trim(user);
        strcpy(bb_name, &item[MAX_LOGIN_NAME_LEN + 1]);
        trim(bb_name);
        if (strcmp(bb_name, bulbrd) != 0)
            fprintf(fp, "%s\n", item);
    }

    fclose(rp);
    fclose(fp);
    (void) rename(tmp_file, datafile);
    (void) chmod(datafile, S_IRUSR | S_IWUSR);
    unsetWriteDBLock(lock_id);

    return(flag);
}


/*****************************************************************************/
