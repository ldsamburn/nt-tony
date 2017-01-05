/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqueue.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ntcssapi.h" 
#include "pqueue.h"
#include "mainfrm.h"
#include "pqueudoc.h"
#include "pqueuvw.h"
#include "pqmsgs.h" 
#include "pqdlg.h" 
#include "dmcdlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPqueueApp

BEGIN_MESSAGE_MAP(CPqueueApp, CWinApp)
	//{{AFX_MSG_MAP(CPqueueApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPqueueApp construction

CPqueueApp::CPqueueApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPqueueApp object

CPqueueApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CPqueueApp initialization

BOOL CPqueueApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPqueueDoc),
		RUNTIME_CLASS(CMainFrame),     // main SDI frame window
		RUNTIME_CLASS(CPqueueView));
	AddDocTemplate(pDocTemplate);   
	
	CMainFrame* pMainFrame = new CMainFrame;
	
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW))
		return(FALSE);
		
	m_pMainWnd=pMainFrame;

	// create a new (empty) document
//	OnFileNew();

	if (m_lpCmdLine[0] != '\0')
	{
		// TODO: add command line processing here
	} 
	
		
	if (!NtcssDLLInitialize(NTCSS_DLL_VERSION, "NTCSS", AfxGetInstanceHandle(), m_pMainWnd->m_hWnd)){
		CString ErrorMsg = "Unable to load NTCSS.DLL Version "; 
		ErrorMsg += NTCSS_DLL_VERSION;
		::MessageBox(m_pMainWnd->m_hWnd, "Initialization Error", "Title",MB_ICONSTOP | MB_OK);
		::DestroyWindow(m_pMainWnd->m_hWnd);
		return TRUE;
	}
      
    
    CPqdlg cpd; 
    
    cpd.DoModal();

	::DestroyWindow(m_pMainWnd->m_hWnd);    
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPqueueApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPqueueApp commands
