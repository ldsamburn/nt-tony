
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


/*
 * ini_Fcts.h
 */

#ifndef _ini_Fcts_h
#define _ini_Fcts_h


/** DEFINE DEFINITIONS **/

#define MAX_INI_ENTRY			256


/** GLOBAL DECLARATIONS **/

/** ini_Fcts.c **/

#ifdef __cplusplus
 extern "C"{
#endif

int  BuildProfileString(char*, const char*, const char*);
int  BuildSectionLabel(char*, const char*);
int  DeleteINILinks( const char*);
int  FlushINIFile(const char*);
int  GetItemValues(const char*, char*, char* );
int  GetProfileInt(const char*, const char*, int, const char*);
int  GetProfileSection(const char*, char*, int, const char*);
int  GetProfileSectionNames(char*, int, const char*);
int  GetProfileString(const char*, const char*, const char*, char*, int,
                      const char*);
int  GetSectionValues(const char*, char*, int, const char*);
int  LoadINIFile(const char*);
int  RemoveProfileString(const char*, const char*, const char*);
int  WalkINIEntries(const char*);
int  WriteProfileSection(const char*, const char*, const char*);
int  WriteProfileString(const char*, const char*, const char*,
                        const char*);
int RemoveProfileSection(char *,char *);


#ifdef __cplusplus
 };
#endif

#endif /* _ini_Fcts_h */
