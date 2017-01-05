// NTCSS_ROOT_DIR\source\include\buildnum.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file defines THE_BUILD_NUM which identifies the current  
// build for the NTCSS DLL and related applications which I develop.
//
//----------------------------------------------------------------------


#ifndef _INCLUDE_0_BLDNUM_H_
#define _INCLUDE_0_BLDNUM_H_
 
#include <ntcssdef.h>

#ifdef NTCSS_32BIT_BUILD

#ifdef WINDOWS_95_BUILD

#define THE_BUILD_NUM     "Test Version" //"Build32-0009"

#else // WINDOWS_NT_BUILD

#define THE_BUILD_NUM     "Test Version" //"Build32NT-0009"

#endif // WINDOWS_95_BUILD / WINDOWS_NT_BUILD

#else  // NTCSS_32BIT_BUILD

#define THE_BUILD_NUM     "Test Version" //"No 16-bit Build16-0009!"

#endif


#endif //_INCLUDE_0_BLDNUM_H_

