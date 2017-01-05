/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// InriZip.h : main header file for the INRIZIP DLL
//

#if !defined(AFX_INRIZIP_H__9D8BA907_35FA_11D2_B77A_006097CA0784__INCLUDED_)
#define AFX_INRIZIP_H__9D8BA907_35FA_11D2_B77A_006097CA0784__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <implode.h>


/////////////////////////////////////////////////////////////////////////////
// CInriZipApp
// See InriZip.cpp for the implementation of this class
//

struct ZIP_HEADER {
	char sig[4];
	char version[2];
	char gen_flag[2];
	char comp_type[2];
	char last_mod_time[2];
	char last_mod_date[2];
	char crc32[4];
	char comp_size[4];
	char uncomp_size[4];
	char filename_length[2];
	char extra_field_length[2];
	char *filename;
	char *extra_field;
};



class CInriZipApp : public CWinApp
{
public:
	CInriZipApp();

	int AddFile(CString FileName);
	void ReadBuffer (char *buffer, UINT *iSize, void *pParam);
	void WriteBuffer(char *buffer, UINT *iSize, void *pParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInriZipApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInriZipApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INRIZIP_H__9D8BA907_35FA_11D2_B77A_006097CA0784__INCLUDED_)
