/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DT.h"
#include "DTDlg.h"
#include "LogonBanner.h"
#include "genutils.h"
#include <sys/stat.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDTApp

BEGIN_MESSAGE_MAP(CDTApp, CWinApp)
	//{{AFX_MSG_MAP(CDTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDTApp construction

CDTApp::CDTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CDTApp::~CDTApp()
{
	if(m_NtcssConfig)
		delete m_NtcssConfig;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDTApp object

CDTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDTApp initialization

BOOL CDTApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	try
	{

	
	CString m_csWindowsDir,temp;
	::GetWindowsDirectory(m_csWindowsDir.GetBuffer(_MAX_PATH),_MAX_PATH);
	m_csWindowsDir.ReleaseBuffer();
	m_csWindowsDir+=_T("\\ntcss.ini");

	struct _stat buf;

	if(_tstat(m_csWindowsDir,&buf))
	{
		temp.Format(_T("There is no %s File ... exiting!!!"),m_csWindowsDir);
		AfxMessageBox(temp);
		return FALSE;
	}


	if(!NtcssRunOnce (_T("NTCSS TOOLBAR")))
	{
		TRACE0(_T("Run Once Kicked In\n"));
		return FALSE;
	}

	KillSecBan();  //kill any orphaned Secban... better than using task man


	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("AfxSocketInit failed!"));
		return FALSE;
	} 

	m_NtcssConfig=new CNtcssConfig;
	BOOL bForce=m_NtcssConfig->m_bForceBanner;

	if(m_NtcssConfig->m_csNtcssRootDir.IsEmpty() ||
	   m_NtcssConfig->m_csMasterServer.IsEmpty())
	{
		temp.Format(_T("Missing critical values in %s... exiting!!!"),m_csWindowsDir);
	   	AfxMessageBox(temp);
		return FALSE;
	}


	TRACE1(_T("Going into while loop...  Force banner is %d\n"),bForce);

	int nRet;

	while(1)
	{
		{

			CLogonBanner dlg2(NULL,m_NtcssConfig->m_csMasterServer,m_NtcssConfig,bForce);
			if((nRet=dlg2.DoModal())==IDOK)
			{
				TRACE("Instantiating CDTDlg\n");
				CDTDlg dlg(NULL,m_NtcssConfig,dlg2.m_strServer,dlg2.m_pLRS,
					       dlg2.m_strPassword,dlg2.m_bAppAdmin,dlg2.m_Hwnd);
				dlg.DoModal();
				if(!bForce)
				break;
			}
			else if(nRet==5) //nobanner failed so we go again this time visible
			{   TRACE0(_T("No Force Banner failed... trying again with Force=TRUE %\n"));
				bForce=TRUE;
				continue;
			}
			else
				break;
		}
	} 

	}

	catch(...)
	{
		TRACE(_T("Unknown exception caught in DTApp()\n"));
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//Put this in to kill an orphaned SecBan
void CDTApp::KillSecBan() 
{
	HWND hWnd=::FindWindow(_T("SECBAN"),NULL);
	::SendMessage(hWnd,SECBAN_KILLIT_MSG,0,0L);
}


