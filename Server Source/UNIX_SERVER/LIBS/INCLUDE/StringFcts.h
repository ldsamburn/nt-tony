
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * StringFcts.h
 */

#ifndef	_StringFcts_h
#define	_StringFcts_h

/** DEFINE DEFINITIONS **/

#define TIME_STR_LEN		11
#define FULL_TIME_STR_LEN	13

#define YEAR_TYPE			0
#define MONTH_TYPE			1
#define WEEK_TYPE			2
#define DAY_TYPE			3
#define HOUR_TYPE			4
#define MINUTE_TYPE			5


/** GLOBAL DECLARATIONS **/

/** StringFcts.c **/

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

char  *addPunc( char * );
char  *allocAndCopyString(const char* );
int    charIsInList(char, const char*);
int    checkScheduleString( char * );
int    convertScheduleString( const char *, char * );
void   debug(char*, char* );
int    decryptString(const char*, char*);
int    encryptString(const char*, char*);
int    fullTimeStrCmp(char*, char *);
int    fullTimeStrToReadable( const char *, char * );
long   FullTimeStrToSecs( const char * );
int    getChildProcess( int );
void   getFullTimeStr( char * );
void   getFullTimeStrWithMiliSec( char * );
int    getNextFullTimeStr( const char *, char *, int, int );
void   getRandomString(char *, int, int, ...);
int    getTimeStr( char * );
int    getWeekDayOfTimeStr( const char * );
int    stripWord( int, const char *, char * );
int    isBlank( char * );
int    isValidFullTimeStr( const char * );
int    isValidReadable( const char *, int, int );
int    isValidTimeStr( const char * );
char  *memGet( char *, int, char * );
void   ncpy( char *, const char *, int );
int    parseArgs( char *, char ** );
int    parseWord( int, const char*, char*, const char* );
int    readableToTimeStr( const char *, char * );
char  *reallocateString( char *, int );
int    removeDuplicates( char **, int );
void   replacePattern( const char * ,const char *, const char *, char * );
int    stringIsPrintable(const char*);
int    StringToLowerCase( char *, int );
void   stripNewline( char * );
char  *stripPath( char * );
char  *strstrng( char *, char * );
int    timeStrCmp( char *, char * );
int    timeStrToReadable( const char *, char * );
char  *trim( char * );
char  *trimEnd( char * );

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _StringFcts_h */
