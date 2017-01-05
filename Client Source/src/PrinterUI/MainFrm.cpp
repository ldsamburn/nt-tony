/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MainFrm.cpp : implementation of the CMainFrame class
//


#include "stdafx.h"
#include "resource.h"
#include "PrinterUIDoc.h"
#include "PrinterUIView.h"
#include "privapis.h"
#include "PrinterUI.h"
#include "MainFrm.h"
#include "PUIHelp.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_DESKTOPTOPICS, OnDesktoptopics)
	ON_COMMAND(ID_GETTINGHELP, OnGettinghelp)
	ON_COMMAND(ID_WINDOWHELP, OnWindowhelp)
	ON_COMMAND(ID_FIELDHELP, OnFieldhelp)
	ON_COMMAND(ID_USERSGUIDE, OnUsersguide)
	ON_COMMAND(ID_IDD, OnIdd)
	ON_COMMAND(ID_SYSADM, OnSysadm)
	ON_COMMAND(ID_TECHSUPPORT, OnTechsupport)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//ON_COMMAND(ID_REFRESH, OnRefresh)

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// these save orig values prior to firing edit/add dialogs
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// CG: The following line was added by the Splash Screen component.
	//CSplashWnd::ShowSplashScreen(this);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying

	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~WS_MAXIMIZEBOX;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


//this drops lock from all 3 exits in FormView
BOOL CMainFrame::DestroyWindow() 
{
	CPrinterUIView*	pview=(CPrinterUIView*)GetActiveView();
	if(pview!=NULL)
	{
	
		NtcssAdminLockOptions(pview->m_app.GetBufferSetLength(16),PUI_Lock,
							  RemoveLock,pview->m_PID.GetBufferSetLength(8),NULL);
		KillTimer(pview->m_nTimer);
	} 
	
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnAppExit() 
{
	CPrinterUIView*	pview=(CPrinterUIView*)GetActiveView();
		
	if(pview!=NULL)
	{
		if(pview->GetDlgItem(IDC_Apply)->IsWindowEnabled())
		if(AfxMessageBox("Quit Without Saving Changes",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			return;
		CWnd* dlg=(CWnd*) AfxGetMainWnd();
		dlg->DestroyWindow();

	}
}

void CMainFrame::OnDesktoptopics() 
{
help_object.LaunchDesktopHelpTopics();	
}

void CMainFrame::OnGettinghelp() 
{
help_object.LaunchGettingHelp();	
}

void CMainFrame::OnWindowhelp() 
{
help_object.LaunchGettingHelp();		
}

void CMainFrame::OnFieldhelp() 
{
help_object.LaunchFieldHelp();		
}

void CMainFrame::OnUsersguide() 
{
help_object.LaunchUsersGuide();		
}

void CMainFrame::OnIdd() 
{
help_object.LaunchInterfaceDesignDocument();		
}

void CMainFrame::OnSysadm() 
{
help_object.LaunchSystemAdministration();		
}

void CMainFrame::OnTechsupport() 
{
help_object.LaunchTechnicalSupport();		
}


void CMainFrame::OnClose() 
{
	CPrinterUIView*	pview=(CPrinterUIView*)GetActiveView();
		
	if(pview!=NULL)
	{
		if(pview->GetDlgItem(IDC_Apply)->IsWindowEnabled())
		if(AfxMessageBox("Quit Without Saving Changes",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			return;
	}
	CFrameWnd::OnClose();
}
