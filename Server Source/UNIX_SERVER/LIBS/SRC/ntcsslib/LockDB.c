
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * LockDB.c
 */

/** SYSTEM INCLUDES **/

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>


/** INRI INCLUDES **/

#include <LibNtcssFcts.h>
#include <ExtraProtos.h>

/** DEFINE DEFINITIONS **/

#define PERMS       0666


/** STATIC FUNCTION PROTOTYPES **/

static void  signal_alarm( int );


/** STATIC VARIABLES **/

static jmp_buf  env_alrm;

static struct sembuf  wait_write_lock[2] = /* 2 operations for this semaphore*/
{
  {     /* First struct.. */
    0,       /* For semaphore #0 in this set ... */
    -1,      /* ... wait until sem #0's value is 1 (resource is free),
                subtract 1 from the value (make it 0, locking the resource). */
    SEM_UNDO,/* Undo last change (add 1: frees sem #0) if process crashes. */
  },

  {     /* Second struct.. */
    1,         /* Then, for semaphore #1 in this set ... */
    0,         /* ...wait until sem #1 becomes 0 ... */
    0          /* .. and return */
  }
};

static struct sembuf clear_write_lock[1] =  /* 1 operation for this semaphore*/
{
  {
    0,         /* For semaphore #0 in this set ... */
    1,         /* ... add 1 to sem #0's value (returns this resource) */
    SEM_UNDO   /* and undo change (subtract 1) if process crashes. */
  }
};

static struct sembuf wait_read_lock[3] =   /* operations for this semaphore */
{
  {0, -1, SEM_UNDO}, /* wait till sem #0's resource is 1, subtract 1 */
  {1,  1, SEM_UNDO}, /* set sem #1 to 1 */
  {0,  1, SEM_UNDO}  /* set sem #0 to 1 */
};

static struct sembuf clear_read_lock[1] =   /* operations for this semaphore */
{
  {
    1,         /* For semaphore number 1 */
    -1,         /* if sem #1 = 1 then decrement it to 0 ...*/
    (IPC_NOWAIT | SEM_UNDO)  /* if sem #1 != 1, return immediately */
  }
};



/** FUNCTION DEFINITIONS **/

/**************************** signal_alarm ***********************************/

static void  signal_alarm(int signo)

{
    signo=signo;
    longjmp(env_alrm, 1);
}


/************************** err_sys ******************************************/

/*
static void  err_sys(str)

     char *str;

{
  syslog(LOG_WARNING, "%s", str);
}
*/


/************************* createDBLock **************************************/

int  createDBLock( int key )

{
  const char   *fnct_name = "createDBLock";
  register int  id;
  union semun
  {
    int               val;
    struct semid_ds  *buf;
    ushort           *array;
  } semctl_arg;

  id = semget((key_t) key, 2, PERMS | IPC_CREAT);
  if (id < 0)
    {
      syslog(LOG_WARNING, "%s Cannot obtain lock semaphore for key %d!",
             fnct_name, key);
      return(id);
    }

  semctl_arg.val = 1;
  if (semctl(id, 0, SETVAL, semctl_arg) < 0)
    syslog(LOG_WARNING, "%s Cannot initialize write DB lock for key %d!",
           fnct_name, key);

  semctl_arg.val = 0;
  if (semctl(id, 1, SETVAL, semctl_arg) < 0)
    syslog(LOG_WARNING, "%s Cannot initialize read DB lock for key %d!",
           fnct_name, key);
  
  return(id);
}


/************************ setReadDBLock **************************************/

int  setReadDBLock( int key,
                    int wait_time )

{
  const char   *fnct_name = "setReadDBLock():";
  int  x;
  register int  id;

  if (signal(SIGALRM, signal_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s Cannot set signal alarm for key %d!", fnct_name,
             key);
      return(-1);
    }

  x = setjmp(env_alrm);
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Database Read Lock Timeout!", fnct_name);
      return(-2);
    }
  
  if ((id = semget((key_t) key, 2, 0)) < 0)
    {
    syslog(LOG_WARNING, "%s: Cannot obtain lock semaphorefor key %d!",
           fnct_name, key);
      return(-3);
    }

  (void) alarm(wait_time);

  x = semop(id, &wait_read_lock[0], 3);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot wait for read DB lock for id %d!",
           fnct_name, id);

  (void) alarm(0);
  
  return(id);
}


/************************ nsetReadDBLock *************************************/

void  unsetReadDBLock( int id )

{
  const char   *fnct_name = "unsetReadDBLock():";
  int  x;

  x = semop(id, &clear_read_lock[0], 1);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot clear read DB lock"
           " for id %d", fnct_name, id);
}


/*********************** setWriteDBLock **************************************/

int  setWriteDBLock( int  key,
                     int  wait_time )
{
  const char   *fnct_name = "setWriteDBLock():";
  register int  id;
  int  x;

  if (signal(SIGALRM, signal_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s Cannot set signal alarm!", fnct_name);
      return(-1);
    }

  x = setjmp(env_alrm);
  if (x != 0)
    {
      /* if returning from a longjmp(), we should get a 1 for the return val */
      syslog(LOG_WARNING, "%s Timed out waiting for semaphore.", fnct_name);
      return(-2);
    }
  id = semget( (key_t) key, 2, 0);
  if (id < 0) /* grab sem ID of this key */
    {
    syslog(LOG_WARNING, "%s Cannot find the semophore ID for key %d!",
           fnct_name, key);
      return(-3);
    }

  (void) alarm(wait_time); /* breaks out of the semop if wait too long */
  x = semop(id, &wait_write_lock[0], 2);
  if (x < 0)
      syslog(LOG_WARNING, "%s Problems accessing semaphore ID %d! Setting "
             "lock failed!", fnct_name, id);
  (void) alarm(0);  /* turn off the alarm set above */

  return(id);
}


/********************* unsetWriteDBLock **************************************/

void  unsetWriteDBLock( int id )

{
  const char   *fnct_name = "unsetWriteDBLock():";
  int  x;

  x = semop(id, &clear_write_lock[0], 1);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot clear semaphore for id %d!", fnct_name, id);
}


/*********************** removeDBLock ****************************************/

void  removeDBLock( int id )

{
  const char   *fnct_name = "removeDBLock():";
  int  x;

  x = semctl(id, 0, IPC_RMID, 0);
  if (x < 0)
    syslog(LOG_WARNING, "%s Error removing read/write DB lock semaphore for "
           "ID %d", fnct_name, id);
}


/*********************** createLock ******************************************/

int  createLock( int key )

{
  const char   *fnct_name = "createLock():";
  register int  id;
  int  x;
  union semun
     {
       int              val;
       struct semid_ds *buf;
       ushort          *array;
     } semctl_arg;

     id = semget((key_t) key, 1, PERMS | IPC_CREAT);
  if (id < 0)
    {
      syslog(LOG_WARNING, "%s Cannot obtain semaphore for key %d!", fnct_name,
             key);
      return(-1);
    }
  semctl_arg.val = 1;
  x = semctl(id, 0, SETVAL, semctl_arg);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot initialize lock semaphore for key %d!",
           fnct_name, key);

  return(id);
}


/************************ setLock ********************************************/

int  setLock( int  key,
              int  wait_time )
{
  const char   *fnct_name = "setLock():";
  register int  id;
  int  x;
    
  if (signal(SIGALRM, signal_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s Cannot set signal alarm for key %d!", fnct_name,
             key);
      return(-1);
    }
  x = setjmp(env_alrm);
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Cannot set jump point!", fnct_name);
      return(-2);
    }
  id = semget((key_t) key, 1, 0);
  if (id < 0)
    {
      syslog(LOG_WARNING, "%s Cannot obtain lock semaphore for key %d!",
             fnct_name, key);
      return(-3);
    }  
  (void) alarm(wait_time);
  
  x = semop(id, &wait_write_lock[0], 1);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Cannot wait for lock for id %d!", fnct_name, id);
      return(-4);
    }

  (void) alarm(0);
  
  return(id);
}


/***************** unsetLock *************************************************/

void  unsetLock( int id )

{
  const char   *fnct_name = "unsetLock():";
  int  x;

  x = semop(id, &clear_write_lock[0], 1);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot clear the write lock (id %d)!", fnct_name,
           id);
}


/*********************** removeLock ******************************************/

void  removeLock(id)

     int    id;

{
  const char   *fnct_name = "removeLock():";
  int  x;

  x = semctl(id, 0, IPC_RMID, 0);
  if (x < 0)
    syslog(LOG_WARNING, "%s Cannot remove semaphore lock id %d!", fnct_name,
           id);
}


/********************** setFileLock ******************************************/

int  setFileLock( char *file,/* empty file */
                  int  *fd,/* assigns file descriptr for the calling process */
                  int   wait_time ) /* time for a device lock to be approved */
{
  const char   *fnct_name = "setFileLock():";
  int  x;

  *fd = open(file, O_RDWR);
  if (*fd < 0)
    {
      syslog(LOG_WARNING, "%s Cannot open file %s!", fnct_name, file);
      return(0);
    }

  if (signal(SIGALRM, signal_alarm) == SIG_ERROR)
    {
      syslog(LOG_WARNING, "%s Cannot set signal alarm for file %s!",
             fnct_name, file);
      return(0);
    }

  x = setjmp(env_alrm);
  if (x != 0)
    {
      syslog(LOG_WARNING, "%s Cannot set jump point!", fnct_name);
      return(0);
    }

  (void) alarm(wait_time);  /* set off the SIGALRM after wait_time secs */

#ifdef SYSV
  lseek(*fd, 0L, SEEK_SET);

  x = lockf(*fd, F_LOCK, 0L);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Cannot lock file %s! fd = %d", fnct_name, file,
             *fd);
      return(0);
    }
#else
  x = flock(*fd, LOCK_EX);
  if (x < 0)
    {
      syslog(LOG_WARNING, "%s Cannot lock file %s! fd = %d", fnct_name, file,
             *fd);      
      return(0);
    }
#endif

  (void) alarm(0);
  return(1);
}


/********************** unsetFileLock ****************************************/

int  unsetFileLock( int fd )

{
  const char   *fnct_name = "unsetFileLock():";
#ifdef SYSV
  lseek(fd, 0L, SEEK_SET);
  if (lockf(fd, F_ULOCK, 0L) < 0)
    {
      syslog(LOG_WARNING, "%s Cannot unlock file descriptor %d!", fnct_name,
             fd);
      return(0);
    }
  
#else
  if (flock(fd, LOCK_UN) < 0)
    {
      syslog(LOG_WARNING, "%s Cannot unlock file descriptor %d!", fnct_name,
             fd);
      return(0);
    }

#endif
  close(fd);
  return(1);
}


/*****************************************************************************/
