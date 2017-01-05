/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

/* 
	Trace.h
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __TRACE_H__
#define __TRACE_H__

#include <crtdbg.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#define TRACEMAXSTRING	1024

char szBuffer[TRACEMAXSTRING];
inline void Trace(const char* format,...)
{
 va_list args;
 va_start(args,format);
 int nBuf;
 nBuf = _vsnprintf(szBuffer,
                   TRACEMAXSTRING,
                   format,
                   args);
 va_end(args);

 _RPT0(_CRT_WARN,szBuffer);
}
#define TraceEx _snprintf(szBuffer,TRACEMAXSTRING,"%s(%d): ", \
                          &strrchr(__FILE__,'\\')[1],__LINE__); \
_RPT0(_CRT_WARN,szBuffer); \
Trace

#else
inline void Trace(const char* format,...) {};
inline void TraceEx(const char* format,...) {};
#endif


#endif // __TRACE_H__


