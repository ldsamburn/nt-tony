/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// msgboard.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "msgboard.h"

#include "mainfrm.h"
#include "mb_doc.h"
#include "mb_vw.h"
#include "mb_mgr.h"
#include "msg_doc.h"
#include "rmsg_vw.h"
#include "pmsg_vw.h"
#include "mb_noteDoc.h"
#include "mb_noteView.h"
#include "MsgBHelp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardApp

BEGIN_MESSAGE_MAP(CMsgBoardApp, CWinApp)
	//{{AFX_MSG_MAP(CMsgBoardApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardApp construction

CMsgBoardApp::CMsgBoardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMsgBoardApp object

CMsgBoardApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CMsgBoardApp initialization

BOOL CMsgBoardApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.



	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	
	CMultiDocTemplate* pdocTemplate =
	 		new CMultiDocTemplate
				(IDR_EDITVITYPE,
				RUNTIME_CLASS(CMb_noteDoc),
				RUNTIME_CLASS(CMDIChildWnd),        
				RUNTIME_CLASS(CMb_noteView));
	AddDocTemplate(pdocTemplate);

	gbl_msgboard_man.m_msgboard_doc_tmplt =
	 		new CMultiDocTemplate
				(IDR_MSGBRDTYPE,
				RUNTIME_CLASS(CMsgBoardDoc),
				RUNTIME_CLASS(CMDIChildWnd),        
				RUNTIME_CLASS(CMsgBoardView));
	AddDocTemplate(gbl_msgboard_man.m_msgboard_doc_tmplt);

	gbl_msgboard_man.m_send_msg_doc_tmplt =
	 		new CMultiDocTemplate
				(IDR_MESSAGETYPE,
				RUNTIME_CLASS(CMsgDoc),
				RUNTIME_CLASS(CMDIChildWnd),        
				RUNTIME_CLASS(CPostMsgView));
	AddDocTemplate(gbl_msgboard_man.m_send_msg_doc_tmplt);

	gbl_msgboard_man.m_recv_msg_doc_tmplt =
	 		new CMultiDocTemplate
				(IDR_MESSAGETYPE,
				RUNTIME_CLASS(CMsgDoc),
				RUNTIME_CLASS(CMDIChildWnd),        
				RUNTIME_CLASS(CRecvMsgView));
	AddDocTemplate(gbl_msgboard_man.m_recv_msg_doc_tmplt);

	

	// create main MDI Frame window
	CMainFrame *pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;


	if (!gbl_msgboard_man.Initialize())
	{
		gbl_msgboard_man.m_errman.HandleError("MsgBoard");
		return(FALSE);
	}

	// create a new (empty) document
//	OnFileNew();

	if (m_lpCmdLine[0] != '\0')
	{
		// TODO: add command line processing here
	}

	// The main window has been initialized, so show and update it.
	help_object.Initialize();


	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
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
void CMsgBoardApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


