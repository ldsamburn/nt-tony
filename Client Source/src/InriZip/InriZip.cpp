/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// InriZip.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "InriZip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DllImport __declspec(dllimport) WINAPI

#define DllExport __declspec(dllexport) WINAPI


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CInriZipApp

BEGIN_MESSAGE_MAP(CInriZipApp, CWinApp)
	//{{AFX_MSG_MAP(CInriZipApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInriZipApp construction

CInriZipApp::CInriZipApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

int DllExport AddFile(CString FileName)
{
AFX_MANAGE_STATE(AfxGetStaticModuleState());

CString temp;

	temp.Format("Adding file <%s> to Zipfile",FileName);
	AfxMessageBox(temp);
	return(0);

}

void CInriZipApp::ReadBuffer (char *buffer, UINT *iSize, void *pParam)
{

}

void CInriZipApp::WriteBuffer(char *buffer, UINT *iSize, void *pParam)
{

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInriZipApp object

CInriZipApp theApp;
