/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\genutils.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file contains prototypes for a variety of general purpose
// routines.  See implementation file for details.
//
//----------------------------------------------------------------------

#ifndef _INCLUDE_GENUTILS_H_CLASS_
#define _INCLUDE_GENUTILS_H_CLASS_


//int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); //list view sort

void centerScreen(HWND hWnd);

BOOL isBlank(char*);

void getTempFileName(char *temp_file_name, int len);

void stripNewline(char*);

int  stripWord(int, char*, char*);

char *strncatWpad(char *dest_str, const char *src_str, 
                  const int len, const char pad);

char *strncpyWpad(char *dest_str, const char *src_str, 
                    const int len, const char pad);

char* trim(char*);

extern "C" BOOL WINAPI NtcssRunOnce(const LPSTR szTitleText);

#endif // _INCLUDE_GENUTILS_H_CLASS_

