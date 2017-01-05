/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\kind_of.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file is used to indicate whether or not the COMN_LIB
// is being built for use by the NTCSSDLL or by applications.
// Processing is different for CNtcssBaseMsg depending on this setting.
//
// Note that with the 32-bit compile this file is ignored as the
// Development Studio makes it easy to build different build 
// configurations which automatically handle the defines.
//
//----------------------------------------------------------------------

#include <ntcssdef.h>      // Don't modify

#ifdef  NTCSS_16BIT_BUILD  // Don't modify

//#define NTCSS_DLL_BUILD	   // Comment out if not doing ntcssdll build
						   //  (only need to mess with if doing 
						   //   16-bit builds)

#endif					   // Don't modify

