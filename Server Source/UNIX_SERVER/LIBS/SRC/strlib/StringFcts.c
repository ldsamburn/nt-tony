
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * StringFcts.c
 */

/** SYSTEM INCLUDES **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <ctype.h>
#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#if defined SOLARIS || defined linux
#include <crypt.h>
#endif

/** INRI INCLUDES **/

#include <StringFcts.h>

/* Boolean values used in parseWord 
   NOTE: These values are also defined
         in Ntcss.h */
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif


/** CONSTANT DEFINITIONS **/

#define MAX_RAND 32767.0   /* Maximum value that can be returned from rand() */
#define FIRST_PRINTABLE_CHAR 32  /* ASCII integer value of first printable
                                    character */
#define NUM_PRINTABLE_CHARS 95   /* Number of printing ASCII characters */ 
#define delta() ((int) ((((float) rand())/((float) RAND_MAX)) * (94.0)))

#define PS_PROG     "/bin/ps -ef"


/** FUNCTION PROTOTYPES **/

static int   checkDuplicates(char* [], int*, int, int);
static void  cryptionInit(const char*);
static int   SecsToFullTimeStr(long, char*);

/** STATIC VARIABLES **/

static const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul","Aug", "Sep", "Oct", "Nov", "Dec" };
static const int month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 
                                  30, 31 };
static const char *days_of_week[] = { "Sunday", "Monday", "Tuesday",
                                      "Wednesday", "Thursday", "Friday",
                                      "Saturday" };



/** FUNCTION DEFINITIONS **/


/*****************************************************************************/
/* Parameters:  str - input string
 * Returns:     void
 * Description: Removes one newline character from the end of 'str' if it
                has one.
*/

void  stripNewline( char *str )

{
  int  leng;

  leng = strlen(str);
  if (str[leng - 1] == '\n')
    str[leng - 1] = '\0';
}


/*****************************************************************************/

char  *addPunc( char *string )

{
  char *r, *p, *q;
  int leng;

  leng = strlen(string);
  if (string[leng - 1] == '\n')
    string[leng - 1] = '\0';

  p = q = r = string;

  while ( isspace((int)*p) )
    p++;
  while (*p)
    {
      if ( !isspace((int)*p) )
        r = q + 1;
      *q++ = *p++; 
    }
  *r = 0;

  leng = strlen(string);
  if (string[leng - 1] != '.' && string[leng - 1] != '!' &&
      string[leng - 1] != '?')
    strcat(string, ".");

  return(string);
}


/****************************************************************************/
/* Allocates a new string and copies strStringToCopy into the new string.
   The calling code is responsible for freeing the memory.
*/

char  *allocAndCopyString(const char *strStringToCopy )

{
  char  *pStrReturn;
  
  pStrReturn = malloc(sizeof(char) * (strlen(strStringToCopy) + 1));
  if (!pStrReturn)
    {
      syslog(LOG_WARNING, "allocAndCopyString: malloc failed");
      return(pStrReturn);
    }

  sprintf(pStrReturn, "%s", strStringToCopy);
  return(pStrReturn);
}


/*****************************************************************************/
/* Removes leading and trailing blanks. */

char  *trim( char *string )

{
  char *r, *p, *q;

  p = q = r = string;

  while ( isspace((int)*p) )
    p++;
  while (*p)
    {
      if ( !isspace((int)*p) )
        r = q + 1;
      *q++ = *p++; 
    }
  *r = 0;

  return(string);
}


/*****************************************************************************/
/* Removes trailing blanks. */

char  *trimEnd( char *string )

{
  char *r, *p, *q;

  p = q = r = string;

  while ( isspace((int)*p) )
    {
      p++;
      q++;
    }
  while (*p)
    {
      if ( !isspace((int)*p) )
        r = q + 1;
      *q++ = *p++; 
    }
  *r = 0;

  return(string);
}


/*****************************************************************************/

void  ncpy( char       *strg1,
            const char *strg2,
            int         num )

{
  strncpy(strg1, strg2, num);
  strg1[num] = '\0';
}


/*****************************************************************************/

char  *strstrng( char *str,
                 char *patt )

{
  char  *p;
  int    leng;

  p = str;

  for (leng = strlen(patt); *p; p++)
    {
      if (*p != *patt)
        continue;
      if (strncmp(p, patt, leng) == 0)
        return(p + leng);
    }

  return(NULL);
}


/*****************************************************************************/

int  stripWord( int         i,
                const char *str,
                char       *word )
{
  int         j;
  int         n = i;
  const char *s;
  char       *w;

  if ((size_t)i >= strlen(str))
    return(-1);

  s = str;
  w = word;
  for (j = 0; j < n; j++)
    s++;
  while ( isspace((int)*s) )
    {
      s++;
      n++;
    }
  while ( !isspace((int)*s) && (*s != 0) )
    {
      *w = *s;
      s++;
      w++;
      n++;
    }
  *w = '\0';

  while ( isspace((int)*s) )
    {
      s++;
      n++;
    }

  return(n);
}


/*****************************************************************************/

int  isBlank( char *str )

{
  char *s = str;

  if (!(s && *s))
    return(1);

  while (*s)
    {
      if ( !isspace((int)*s) )
        return(0);
      s++;
    }

  return(1);
}


/*****************************************************************************/

static int  checkDuplicates( char *items[],
                             int  *i,
                             int   n,
                             int   num )
{
  int  j;
  int  start = *i;

  while (start < n && (items[start] == NULL || *items[start] == 0))
    start++;
  if (start >= n)
    return(0);
  *i = start;
  for (j = 0; j < num; j++)
    if (strcmp(items[start], items[j]) == 0)
      return(-1);

  return(1);
}


/*****************************************************************************/
/* Alter array of strings by removing all duplicates and empty lines.
   Return the size of the altered array.
*/

int  removeDuplicates( char **items,
                       int    n )
{
  int  retval;
  int  i = 0;
  int  num = 0;

  while ( (retval = checkDuplicates(items, &i, n, num)) )
    {
      if (retval != -1)
        {
          strcpy(items[num], items[i]);
          num++;
        }
      i++;
    }
  return(num);
}


/*****************************************************************************/
/* Use this function to write a string to a file for debugging purposes. */

void  debug( char *file,
             char *str )
{
  FILE  *fp;

  fp = fopen(file, "a");
  if (fp != NULL)
    {
      fprintf(fp, "%s\n", str);
      fclose(fp);
    }
}


/*****************************************************************************/
/* Strip off leading path.  Return pointer to filename. */

char  *stripPath( char *file )

{
  int   i = 0, pos = 0;
  char *f;

  if (file == NULL)
    return(NULL);

  f = file;
  while (*f)
    {
      i++;
      if (*f == '/')
        pos = i;
      f++;
    }

  return(&file[pos]);
}


/*****************************************************************************/
/* Replace all occurrences of "str1" in "line" with "str2" and return the
   altered string in "nline".
*/

void  replacePattern( const char *str1,
                      const char *str2,
                      const char *line,
                      char       *nline )
{
  const char *s = line;
  int i, j, leng1, leng2;

  leng1 = strlen(str1);
  leng2 = strlen(str2);
  i = 0;
  while (*s != 0)
    {
      if (strncmp(s, str1, leng1) == 0)
        {
          for (j = 0; j < leng1; j++)
            s++;
          for (j = 0; j < leng2; j++)
            {
              nline[i] = str2[j];
              i++;
            }
        }
      else
        {
          nline[i] = *s;
          s++; i++;
        }
    }
  nline[i] = 0;
}


/*****************************************************************************/

int  getWeekDayOfTimeStr( const char *str )

{
  char  full[FULL_TIME_STR_LEN];
  long  tme;

  sprintf(full, "%s00", str);
  tme = FullTimeStrToSecs(full);
  if (tme < 0L)
    return(-1);

  tme = tme / 86400L;
  tme = tme % 7L;
  switch ((int) tme) {
        case 0:
            return(4);
        case 1:
            return(5);
        case 2:
            return(6);
        case 3:
            return(0);
        case 4:
            return(1);
        case 5:
            return(2);
        case 6:
            return(3);
    }

    return(-1);
}


/*****************************************************************************/

int  getTimeStr( char *str )

{
  /*struct tm  *localtime();*/
  time_t      tme;
  /*time_t      time();*/

/*
#ifdef VSUN
  int         strftime();
#endif
#ifdef SYSV
  size_t      strftime();
#endif
*/

  str[0] = '\0';                                              /* Initialize. */

  tme = time((long *) 0);            /* Returns time in seconds since Epoch. */

  (void) strftime(str, TIME_STR_LEN + 1, "%y%m%d.%H%M", localtime(&tme));

  if(str[0] == '\0')                           /* Did something get written? */
    return(-1);
  else
    str[TIME_STR_LEN] = '\0';                       /* Terminate the string. */

  return(0);
}


/*****************************************************************************/

void  getFullTimeStr( char *str )

{
    time_t tme;/*, time();
    struct tm *localtime();
#ifdef VSUN
    int strftime();
#endif
#ifdef SYSV
    size_t strftime();
#endif
*/

    tme = time((long *) 0);
    (void) strftime(str, FULL_TIME_STR_LEN + 1,
            "%y%m%d.%H%M%S", localtime(&tme));
    str[FULL_TIME_STR_LEN] = 0;
}


/*****************************************************************************/
/* GWY 98-07-17 added getFullTimeStrWithMls for ATLAS work */

void  getFullTimeStrWithMiliSec( char *str )

{
    time_t tme;/*, time();
    struct tm *localtime();*/
    struct timeval timeVal;
    struct timezone timeZone;
    char   strTemp[200];

/*
#ifdef VSUN
    int strftime();
#endif
#ifdef SYSV
    size_t strftime();
#endif
*/

    tme = time((long *) 0);
    if (gettimeofday(&timeVal, &timeZone) != 0) {
      syslog(LOG_WARNING, "getFullTimeStrWithMicroSec: gettimeofday failed\n");
      return;
    } /* END if */

    (void) strftime(strTemp, FULL_TIME_STR_LEN + 1,
            "%y%m%d.%H%M%S", localtime(&tme));
    strTemp[FULL_TIME_STR_LEN] = 0;
        /* Divide tv_usec to get miliseconds from microseconds */
        sprintf(str, "%s:%ld", strTemp, (timeVal.tv_usec/1000));

    
}
/* END GWY 98-07-17 added block */


/*****************************************************************************/

int  isValidTimeStr( const char *str )

{
    const char *s;
    char        buf[3];
    int         i, t;

    if (strlen(str) != TIME_STR_LEN)
        return(0);

    for (i = 0; i < TIME_STR_LEN; i++) {
        if (i == 6) {
            if (str[i] != '.')
                return(0);
        } else {
            if (!isdigit( (int)str[i]) )
                return(0);
        }
    }

    s = str;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 99)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 01 || t > 12)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 01 || t > 31)
        return(0);
    s += 3;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 23)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 59)
        return(0);

    return(1);
}


/*****************************************************************************/

int  isValidFullTimeStr( const char *str )

{
    const char *s;
        char  buf[3];
    int i, t;

    if (strlen(str) != FULL_TIME_STR_LEN)
        return(0);

    for (i = 0; i < FULL_TIME_STR_LEN; i++) {
        if (i == 6) {
            if (str[i] != '.')
                return(0);
        } else {
            if ( !isdigit((int)str[i]) )
                return(0);
        }
    }

    s = str;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 99)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 01 || t > 12)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 01 || t > 31)
        return(0);
    s += 3;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 23)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 59)
        return(0);
    s += 2;  ncpy(buf, s, 2);  t = atoi(buf);
    if (t < 00 || t > 59)
        return(0);

    return(1);
}


/*****************************************************************************/

int  isValidReadable( const char *str,
                      int         with_secs,
                      int         with_year )

{
  int   i, j, t;
  char  time_buff[16];
  char  value[8];

  i = 0;

  i = stripWord(i, str, time_buff);
  if (i == -1)
    return(0);
  t = atoi(time_buff);
  if (t < 01 || t > 31)
    return(0);

  i = stripWord(i, str, time_buff);
  if (i == -1)
    return(0);
  t = 1;
  for (j = 0; j < 12; j++)
    if (strcmp(time_buff, months[j]) == 0)
      t = 0;
  if (t)
    return(0);

  if (with_year)
    {
      i = stripWord(i, str, time_buff);
      if (i == -1)
        return(0);
      t = atoi(time_buff);
      if (t < 00 || t > 99)
        return(0);
    }

  i = stripWord(i, str, time_buff);
  if (i == -1)
    return(0);
  if (strlen(time_buff) != (size_t)(5 + 3*with_secs))
    return(0);

  value[0] = time_buff[0];
  value[1] = time_buff[1];
  value[2] = 0;
  t = atoi(value);
  if (t < 00 || t > 23)
    return(0);

  value[0] = time_buff[3];
  value[1] = time_buff[4];
  value[2] = 0;
  t = atoi(value);
  if (t < 00 || t > 59)
    return(0);

  if (!with_secs)
    return(1);

  value[0] = time_buff[6];
  value[1] = time_buff[7];
  value[2] = 0;
  t = atoi(value);
  if (t < 00 || t > 59)
    return(0);

  return(1);
}


/*****************************************************************************/
/* Assumes ts1 and ts2 are both valid time strings. */

int  timeStrCmp( char *ts1,
                 char *ts2 )

{
    unsigned long tm1, tm2;
    char *s1, *s2;
    char buf1[3], buf2[3];

    s1 = ts1;  ncpy(buf1, s1, 2);  tm1 = atol(buf1);
    s2 = ts2;  ncpy(buf2, s2, 2);  tm2 = atol(buf2);
    if (tm1 > 51L && tm2 < 49L)
        tm2 += 100L;
    if (tm2 > 51L && tm1 < 49L)
        tm1 += 100L;
    tm1 *= 535680L;  tm2 *= 535680L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*44640L;
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*44640L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*1440L;
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*1440L;
    s1 += 3;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*60L;
    s2 += 3;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*60L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1);
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2);

    if (tm1 == tm2)
        return(0);
    else if (tm1 < tm2)
        return(1);
    else
        return(-1);
}


/*****************************************************************************/
/* Assumes ts1 and ts2 are both valid time strings. */

int  fullTimeStrCmp( char *ts1,
                     char *ts2 )

{
    unsigned long tm1, tm2;
    int sec1, sec2;
    char *s1, *s2;
    char buf1[3], buf2[3];

    s1 = ts1;  ncpy(buf1, s1, 2);  tm1 = atol(buf1);
    s2 = ts2;  ncpy(buf2, s2, 2);  tm2 = atol(buf2);
    if (tm1 > 51L && tm2 < 49L)
        tm2 += 100L;
    if (tm2 > 51L && tm1 < 49L)
        tm1 += 100L;
    tm1 *= 535680L;  tm2 *= 535680L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*44640L;
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*44640L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*1440L;
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*1440L;
    s1 += 3;  ncpy(buf1, s1, 2);  tm1 += atol(buf1)*60L;
    s2 += 3;  ncpy(buf2, s2, 2);  tm2 += atol(buf2)*60L;
    s1 += 2;  ncpy(buf1, s1, 2);  tm1 += atol(buf1);
    s2 += 2;  ncpy(buf2, s2, 2);  tm2 += atol(buf2);
    s1 += 2;  ncpy(buf1, s1, 2);  sec1 = atoi(buf1);
    s2 += 2;  ncpy(buf2, s2, 2);  sec2 = atoi(buf2);

    if (tm1 == tm2) {
        if (sec1 == sec2)
            return(0);
        else if (sec1 < sec2)
            return(1);
        else
            return(-1);
    } else if (tm1 < tm2) {
        return(1);
    } else {
        return(-1);
    }
}


/*****************************************************************************/
int  timeStrToReadable( const char *str,
                        char       *out_str )

{
  char time_str[6];
  int month, day, year;

  if (!isValidTimeStr(str))
    return(0);

  time_str[0] = str[0];
  time_str[1] = str[1];
  time_str[2] = 0;
  year = atoi(time_str);

  time_str[0] = str[2];
  time_str[1] = str[3];
  time_str[2] = 0;
  month = atoi(time_str) - 1;

  time_str[0] = str[4];
  time_str[1] = str[5];
  time_str[2] = 0;
  day = atoi(time_str);

  time_str[0] = str[7];
  time_str[1] = str[8];
  time_str[2] = ':';
  time_str[3] = str[9];
  time_str[4] = str[10];
  time_str[5] = 0;

  sprintf(out_str, "%2.2d %s %2.2d %s", day, months[month], year, time_str);

  return(1);
}


/*****************************************************************************/
int  fullTimeStrToReadable( const char *str,
                            char       *out_str )
{
  char time_str[9];
  int month, day, year;

  if (!isValidFullTimeStr(str))
    return(0);

  time_str[0] = str[0];
  time_str[1] = str[1];
  time_str[2] = 0;
  year = atoi(time_str);

  time_str[0] = str[2];
  time_str[1] = str[3];
  time_str[2] = 0;
  month = atoi(time_str) - 1;

  time_str[0] = str[4];
  time_str[1] = str[5];
  time_str[2] = 0;
  day = atoi(time_str);

  time_str[0] = str[7];
  time_str[1] = str[8];
  time_str[2] = ':';
  time_str[3] = str[9];
  time_str[4] = str[10];
  time_str[5] = ':';
  time_str[6] = str[11];
  time_str[7] = str[12];
  time_str[8] = 0;

  sprintf(out_str, "%2.2d %s %2.2d %s", day, months[month], year, time_str);

  return(1);
}


/*****************************************************************************/
int  readableToTimeStr( const char *input_str,
                        char       *out_str )
{
  int    i, day, year, mn = 0;
  char   month[10];
  char   time_buff[6];
  char   new_time[6];

  if (!isValidReadable(input_str, 0, 1))
    return(0);

  (void) sscanf(input_str, "%d%s%d%s", &day, month, &year, time_buff);
  trim(month);
  for (i = 0; i < 12; i++)
    {
      if (strcmp(month, months[i]) == 0)
        {
          mn = i + 1;
          break;
        }
    }
  new_time[0] = time_buff[0];
  new_time[1] = time_buff[1];
  new_time[2] = time_buff[3];
  new_time[3] = time_buff[4];
  new_time[4] = 0;

  sprintf(out_str, "%2.2d%2.2d%2.2d.%s", year, mn, day, new_time);

  return(1);
}


/*****************************************************************************/

int  getNextFullTimeStr( const char *str,
                         char       *new_str,
                         int         type,
                         int         num )

{
  int year, month, day, extra;
  char buf[3];
  long tme, amt = (long) num;

  if (!isValidFullTimeStr(str))
    return(0);

  switch (type) {
        case YEAR_TYPE:
            buf[0] = str[0];
            buf[1] = str[1];
            buf[2] = 0;
            year = atoi(buf);
            buf[0] = str[2];
            buf[1] = str[3];
            buf[2] = 0;
            month = atoi(buf);
            buf[0] = str[4];
            buf[1] = str[5];
            buf[2] = 0;
            day = atoi(buf);
            year += num;
            while (day == 29 && month == 2 && (year % 4) != 0)
                year += 1;
            while (year >= 100)
                year -= 100;
            sprintf(new_str, "%2.2d%2.2d%2.2d%s", year, month, day, &str[6]);
            return(1);
            break;

        case MONTH_TYPE:
            buf[0] = str[0];
            buf[1] = str[1];
            buf[2] = 0;
            year = atoi(buf);
            buf[0] = str[2];
            buf[1] = str[3];
            buf[2] = 0;
            month = atoi(buf);
            buf[0] = str[4];
            buf[1] = str[5];
            buf[2] = 0;
            day = atoi(buf);
            month += num;
            while (month >= 13) {
                month -= 12;
                year += 1;
            }
            extra = 0;
            if (month == 2 && (year % 4) == 0) extra = 1;
            while ((month_days[month - 1] + extra) < day)
                month += 1;
            while (year >= 100)
                year -= 100;
            sprintf(new_str, "%2.2d%2.2d%2.2d%s", year, month, day, &str[6]);
            return(1);
            break;

        case WEEK_TYPE:
            amt *= 7L;
        case DAY_TYPE:
            amt *= 24L;
        case HOUR_TYPE:
            amt *= 60L;
        case MINUTE_TYPE:
            amt *= 60L;
        break;

        default:
            return(0);
            break;
    }

    if ((tme = FullTimeStrToSecs(str)) < 0L)
        return(0);
    tme += amt;
    if (!SecsToFullTimeStr(tme, new_str))
        return(0);

    return(1);
}


/*****************************************************************************/

int  parseArgs( char  *str,
                char **args )
{
    char buf[2], *s;
    int i, j, n_words;

    trim(str);
    s = str; i = 0;
    while (*s)
        if (*s++ == '"')
            i++;
    if ((i % 2) == 1)
        return(-1);

    i = 0; n_words = 0;
    while((j = stripWord(i, str, args[n_words])) != -1) {
        if (args[n_words][0] == '"') {
            args[n_words][0] = 0;
            s = &str[i];
            while (*s == '"' || isspace( (int)*s )) {
                if (*s == '"') {
                    s++;
                    i++;
                    break;
                }
                s++;
                i++;
            }
            while (*s) {
                sprintf(buf, "%c", *s);
                if (*s != '"')
                    strcat(args[n_words], buf);
                i++;
                if (*s == '"') {
                    i++;
                    n_words++;
                    break;
                } else
                    s++;
            }
            if (*s == 0)
                return(-1);
            if (*(s + 1) == 0)
                return(n_words);
        } else {
            if (strchr(args[n_words], '"') != NULL)
                return(-1);
            i = j;
            n_words++;
        }
    }
    args[n_words] = NULL;

    return(n_words);
}


/*****************************************************************************/

int  charIsInList(char        chCharToCheckFor,
                  const char *strCharList )
{
  int i;

  for (i = 0; (size_t)i < strlen(strCharList); i++) {
    if (chCharToCheckFor == strCharList[i]) {
      return(TRUE);
    } /* END if */
  } /* END for */

  return(FALSE);
}


/*****************************************************************************/

int  parseWord( int          nCharIndex,
                const char  *strString,
                char        *strWordBuffer,
                const char  *strSepChars )
{
    int j, n = nCharIndex;
    const char  *s;
    char        *w;

    if ((size_t)nCharIndex >= strlen(strString))
        return(-1);

    s = strString;
    w = strWordBuffer;
    for (j = 0; j < n; j++)
        s++;
    while (charIsInList(*s, strSepChars) == TRUE) {
        s++;
        n++;
    }
    while ((charIsInList(*s, strSepChars) == FALSE) && (*s != 0)) {
        *w = *s;
        s++;
        w++;
        n++;
    }
    *w = '\0';

    while (charIsInList(*s, strSepChars) == TRUE) {
        s++;
        n++;
    }

    return(n);
}


/*****************************************************************************/


void  getRandomString( char* str,
                       int   max_leng,
                       int   nargs,
                             ... )
{
  va_list ap;
  static char salt[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK"
    "LMNOPQRSTUVWXYZ0123456789";
  char *s, salt_val[3];
  char key[] = "zzzzzzzz";
  int i, j, leng, offset = 0;

# ifdef VSUN
    static float div_val = 2147483647.0/62.0;
# else
    static float div_val = 32767.0/62.0;
# endif

    va_start(ap, nargs);
    for (j = 0; j < nargs; j++) {
        if ((s = va_arg(ap, char *)) == NULL)
            break;
        leng = strlen(s);
        if (leng > 8)
            leng = 8;
        for (i = 0; i < leng; i++) {
            offset = abs(((int) s[i] - (int) key[i])/2);
            if ((int) s[i] < (int) key[i])
                key[i] = (char) ((int) s[i] + offset);
            else
                key[i] = (char) ((int) key[i] + offset);
        }
    }
    va_end(ap);

    srand(offset);
    i = (int) (rand()/div_val);
    if (i >= 62)
        i = 61;
    j = (int) (rand()/div_val);
    if (j >= 62)
        j = 61;
    salt_val[0] = salt[i];
    salt_val[1] = salt[j];
    salt_val[2] = 0;
    ncpy(str, crypt(key, salt_val), max_leng);
}


/*****************************************************************************/

long  FullTimeStrToSecs( const char *str )

{
    const char *s;
        char buf[3];
    int year, month, date, hour, min, sec;
    int iyr, imn, idt;
    long tme = 0L;

    if (!isValidFullTimeStr(str))
        return(-1L);

    s = str;  ncpy(buf, s, 2);  year = atoi(buf);
    if (year >= 0 && year <= 59)
        year += 100;
    s += 2;  ncpy(buf, s, 2);  month = atoi(buf);
    s += 2;  ncpy(buf, s, 2);  date = atoi(buf);
    s += 3;  ncpy(buf, s, 2);  hour = atoi(buf);
    s += 2;  ncpy(buf, s, 2);  min = atoi(buf);
    s += 2;  ncpy(buf, s, 2);  sec = atoi(buf);

    for (iyr = 70; iyr <= year; iyr++) {
        if (iyr == year) {
            for (imn = 1; imn <= month; imn++) {
                if (imn == month) {
                    for (idt = 1; idt <= date; idt++) {
                        if (idt == date)
                            tme += ((long) hour)*3600L + ((long) min)*60L +
                                                                ((long) sec);
                        else
                            tme += 86400L;
                    }
                } else {
                    tme += ((long) month_days[imn - 1])*86400L;
                    if (((iyr % 4) == 0) &&
                        (imn == 2))
                        tme += 86400L;
                }
            }
        } else {
            if ((iyr % 4) == 0)
                tme += 31622400L;
            else
                tme += 31536000L;
        }
    }

    return(tme);
}


/*****************************************************************************/

static int  SecsToFullTimeStr( long  tme,
                               char *str )
{
  int i, j, year, month, date, hour, min, sec;
  long amt, nsecs = 0L;

  if (tme < 0L)
    return(0);

  month = date = 1;
  hour = min = sec = 0;
  for (i = 70; i <= 159; i++)
    {
      year = i;
      if (tme == nsecs)
        goto assemble_datetime;
      if ((year % 4) == 0)
        amt = 31622400L;
      else
        amt = 31536000L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  for (i = 1; i <= 12; i++)
    {
      month = i;
      if (tme == nsecs)
        goto assemble_datetime;
      amt = ((long) month_days[month - 1])*86400L;
      if (((year % 4) == 0) && (month == 2))
        amt += 86400L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  j = month_days[month - 1];
  if (((year % 4) == 0) && (month == 2))
    j++;
  for (i = 1; i <= j; i++)
    {
      date = i;
      if (tme == nsecs)
        goto assemble_datetime;
      amt = 86400L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  for (i = 0; i < 24; i++)
    {
      hour = i;
      if (tme == nsecs)
        goto assemble_datetime;
      amt = 3600L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  for (i = 0; i < 60; i++)
    {
      min = i;
      if (tme == nsecs)
        goto assemble_datetime;
      amt = 60L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  for (i = 0; i < 60; i++)
    {
      sec = i;
      if (tme == nsecs)
        goto assemble_datetime;
      amt = 1L;
      if ((nsecs + amt) > tme)
        break;
      nsecs += amt;
    }
  return(0);

 assemble_datetime:

  if (year > 99)
    year -= 100;
  sprintf(str, "%2.2d%2.2d%2.2d.%2.2d%2.2d%2.2d", year, month, date,
          hour, min, sec);
  return(1);
}


/*****************************************************************************/

static void  cryptionInit( const char  *key )

{ 
  unsigned int  sum;

  sum = 0; 

  while (*key)
    {
      sum += (unsigned int)*key;
      key++;
    }
  srand(sum);
}


/*****************************************************************************/

int  encryptString( const char  *key,
                    char        *data )
{
  
  if (!(stringIsPrintable(key) && stringIsPrintable(data)))
    return(0);
  
  cryptionInit(key);
  
  while (*data)
    {
      *data = ((*data - FIRST_PRINTABLE_CHAR + delta()) % NUM_PRINTABLE_CHARS) +
        FIRST_PRINTABLE_CHAR;
      data++;
    }
  return(1);
}


/*****************************************************************************/

int  decryptString( const char  *key,
                    char        *data )
{
  if (!(stringIsPrintable(key) && stringIsPrintable(data)))
    return(0);
  
  cryptionInit(key);
  
  while (*data) {
    *data = ((*data - FIRST_PRINTABLE_CHAR - delta() + NUM_PRINTABLE_CHARS) %
         NUM_PRINTABLE_CHARS) + FIRST_PRINTABLE_CHAR;
    data++;
  }
  return(1);
}


/*****************************************************************************/

int  stringIsPrintable( const char  *test_string )

{
  while (*test_string)
    {
      if (!isprint( (int)*test_string ))
        return(0);
      test_string++;
    }
  return (1);
}


/*****************************************************************************/

int  convertScheduleString( const char *instr,
                            char       *outstr )
{
  int  j;
  int  i = 0;
  char buf[64];

  if (strlen(instr) != 10)
    return(0);

  switch(instr[0])
    {
    case 'H':
      strcpy(outstr, "Hourly at");
      break;

    case 'O':
    case 'E':
    case 'Y':
      buf[0] = instr[8];
      buf[1] = instr[9];
      buf[2] = 0;
      i = atoi(buf) - 1;
      if ((i < 0) && (i > 11))
        return(0);
      buf[0] = instr[6];
      buf[1] = instr[7];
      buf[2] = 0;
      if (((j = atoi(buf)) < 1) && (j > 31))
        return(0);
      if (instr[0] == 'O')
        sprintf(outstr, "One time on %2.2d %s", j, months[i]);
      else
        if (instr[0] == 'E')
          sprintf(outstr, "Exception on %2.2d %s", j, months[i]);
        else
          sprintf(outstr, "Yearly on %2.2d %s", j, months[i]);
      break;

    case 'D':
      strcpy(outstr, "Daily");
      break;

    case 'M':
      buf[0] = instr[6];
      buf[1] = instr[7];
      buf[2] = 0;
      if (((j = atoi(buf)) < 1) && (j > 31))
        return(0);
      sprintf(outstr, "Monthly on %2.2d", j);
      break;

    case 'W':
      buf[0] = instr[5];
      buf[1] = 0;
      if (((j = atoi(buf)) < 0) && (j > 6))
        return(0);
      sprintf(outstr, "Weekly on %s", days_of_week[j]);
      break;
      
    default:
      return(0);
      break;
    }

  if (instr[0] != 'H')
    {
      buf[0] = instr[3];
      buf[1] = instr[4];
      buf[2] = 0;
      if (((i = atoi(buf)) < 0) || (i > 23))
        return(0);
    }
  buf[0] = instr[1];
  buf[1] = instr[2];
  buf[2] = 0;
  if (((j = atoi(buf)) < 0) || (j > 59))
    return(0);
  if (instr[0] == 'H')
    sprintf(buf, " %2.2d minutes after the hour", j);
  else
    sprintf(buf, " at %2.2d:%2.2d", i, j);
  strcat(outstr, buf);

  return(1);
}


/*****************************************************************************/

int  checkScheduleString( char *instr )
{
    int i;
    char buf[16];

    if (strlen(instr) != 10)
        return(0);

    switch(instr[0]) {
        case 'H':
        case 'D':
            break;

        case 'O':
        case 'E':
        case 'Y':
            buf[0] = instr[8];
            buf[1] = instr[9];
            buf[2] = 0;
            i = atoi(buf) - 1;
            if ((i < 0) && (i > 11))
                return(0);
        case 'M':
            buf[0] = instr[6];
            buf[1] = instr[7];
            buf[2] = 0;
            if (((i = atoi(buf)) < 1) && (i > 31))
                return(0);
            break;

        case 'W':
            buf[0] = instr[5];
            buf[1] = 0;
            if (((i = atoi(buf)) < 0) && (i > 6))
                return(0);
            break;

        default:
            return(0);
            break;
    }
    if (instr[0] != 'H') {
        buf[0] = instr[3];
        buf[1] = instr[4];
        buf[2] = 0;
        if (((i = atoi(buf)) < 0) || (i > 23))
            return(0);
    }
    buf[0] = instr[1];
    buf[1] = instr[2];
    buf[2] = 0;
    if (((i = atoi(buf)) < 0) || (i > 59))
        return(0);

    return(1);
}


/****************************************************************************/

char  *memGet( char *line,
               int   length,
               char *fptr)
{
    char *s, *r, *t;
    int i = 0;

    s = strchr(fptr, '\n');
    if (s == NULL)
      return(NULL);

    r = line;
    t = fptr;
    while ( (*t != '\n') && (i < (length - 1)) )
      {
        *r = *t;
        r++;
        t++;
        i++;
      }
    *r++ = '\n';
    *r = '\0';

    return(s + 1);
}


/*****************************************************************************/

char  *reallocateString( char *buffer,
                         int   new_size)
{

  char *new_buffer;
  int last_data_byte;

  new_buffer = malloc(new_size);
  if (new_buffer == NULL)
    return(NULL);

  if (buffer != NULL)
    {
      last_data_byte = (strlen(buffer) < (size_t)new_size) ? strlen(buffer) : new_size - 1;
      memcpy(new_buffer, buffer, last_data_byte);
      new_buffer[last_data_byte] = '\0';
      free(buffer);
    }

  return(new_buffer);
}


/*****************************************************************************/

int  getChildProcess( int pid )

{
  FILE *pp;
  char  user[64];
  char  line[512];
  int   cpid;
  int   ppid;
  int   retval = -1;

  pp = popen(PS_PROG, "r");
  if (pp == NULL)
    return(-1);

  if (fgets(line, 512, pp) == NULL)
    return(-1);

  while ((fgets(line, 512, pp) != NULL) &&
         (sscanf(line, "%s%d%d", user, &cpid, &ppid) == 3))
    {
      if (ppid == pid)
        {
          retval = cpid;
          break;
        }
    }
  pclose(pp);

  return(retval);
}


/*****************************************************************************/
/* Converts the chars in a string to their equivalent lower case alpha
   characters.  Non-alpha characters remain unchanged.
*/
int  StringToLowerCase( char *string,
                        int   string_size)

{
  int  i;
  int ascii_num;

  for (i = 0; i < string_size; i++)
    {
      ascii_num = (int) string[i];
      string[i] = (char) tolower(ascii_num);
    }

  string[i] = '\0';   /* terminate the string */

  return(0);
}

/*****************************************************************************/
