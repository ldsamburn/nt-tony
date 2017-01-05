/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

/***********************************************************************
	$Workfile$
	$Revision: 1.1 $	$Date: 2002/06/04 20:24:53 $
	$Created By: <various authors> $

	Description: <Description of the file and its purpose>

	$Log: STDAFX.H,v $
	Revision 1.1  2002/06/04 20:24:53  dbyers
	Initial check-in
	
	Revision 1.2  1997/09/25 19:15:30  lpm
	LPM - Added headers.


***********************************************************************/


//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions (including VB)
#include <stdio.h>
#include <windows.h>
#include <aclapi.h> 
#include <lmerr.h> 
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif 

