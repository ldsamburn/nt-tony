/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\dbg_defs.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file is used for defines that are used for debugging.
//
// To centralize debugging, please keep all of debug related defines
// in this file, along with a description of how to turn them on and
// off and brief details on how they work.  Group in sections as
// applicable.
//
// Sections for this file:
//
// SECTION 1 - Debug log files
// SECTION 2 - NTCSS Message Debugging
// SECTION 3 - NTCSS INRI FTP Tracing
//
//----------------------------------------------------------------------

                                       // A quick way to ensure no
                                       // debugging is to uncomment
                                       // out NTCSS_DEBUG_TURNED_OFF
                                       // definition
#define NTCSS_DEBUG_TURNED_OFF

#ifndef NTCSS_DEBUG_TURNED_OFF         // Don't uncomment this

/////////////////////////////////////////////////////////////////////////////
// SECTION 1 - Debug log files
// ___________________________
//
// Just defines log file names
								       // Receives _NTCSS_MSG_TRACING_
									   // related data
									   //
#define NTCSS_MSG_TRACE_FILE      "TRC_MSG.TXT"
#define NTCSS_FTP_MAN_TRACE_FILE  "TRC_FTP.TXT"


/////////////////////////////////////////////////////////////////////////////
// SECTION 2 - NTCSS Message Debugging
// ___________________________________
//
// Definition				Descripton
// ----------				----------
//
// _NTCSS_MSG_TRACING_		
//							This is prerequite for all NTCSS message 
//							tracing
//
// _NTCSS_MSG_TRACING_UNBUNDLING		
//							If having problems with unbundling of data
//							from server then turn this on

//					   
									   // If comment out then all 
									   // _NTCSS_MSG... definitions will
									   //  not apply and no tracing code 
									   // will be compiled into the source
#define  _NTCSS_MSG_TRACING_	       

									   // Now depends on setting of
									   // _NTCSS_MSG_TRACING_
#ifdef _NTCSS_MSG_TRACING_
									   // Leave	_SHOW_UNBUNDLING_
									   // commented unless having problems
									   // with unbundling

#define _NTCSS_MSG_TRACING_UNBUNDLING		   



#endif // _NTCSS_MSG_TRACING_




/////////////////////////////////////////////////////////////////////////////
// SECTION 3 - NTCSS INRI FTP Tracing
// __________________________________
//
// If want to trace CInriFtpMan then define	_NTCSS_FTP_MAN_TRACING_,
// otherwise leave commented.
//

//#define _NTCSS_FTP_MAN_TRACING_



#endif // NTCSS_DEBUG_TURNED_OFF - leave as last line!