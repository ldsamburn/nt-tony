/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\mainfrm.cpp                   
//
// WSP, last update: TBD
//                                        
// This is the implementation file for the MsgBoard CMainFrame class.
// As seen, most calls are passed on to gbl_msgboard_man functions
// for handling.

#include "stdafx.h"
#include "msgboard.h"

#include "mainfrm.h"

#include "mb_mgr.h"
#include "mb_vw.h"
#include "MsgBHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame constructor/destructor
// _________________________________

CMainFrame::CMainFrame()
{
	
}

CMainFrame::~CMainFrame()
{
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame dynamic implementation/message mapping
// _________________________________________________

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_MSGBOARD_SUBCRIBE, OnMsgboardSubcribe)
	ON_COMMAND(ID_MESSAGES_POSTNEWMESSAGE, OnMessagesPostnewmessage)
	ON_UPDATE_COMMAND_UI(ID_MSGBOARD_MAIN_IN, OnUpdateMsgboardMainIn)
	ON_COMMAND(ID_MSGBOARDS_OPEN, OnMsgboardOpen)
	ON_COMMAND(ID_MSGBOARD_MAIN_IN, OnMsgboardMainIn)
	ON_COMMAND(ID_MSGBOARDS_REFRESH_ALL, OnMsgboardsRefreshAll)
	ON_COMMAND(ID_MSGBOARD_NEW, OnMsgboardNew)
	ON_COMMAND(ID_DESKTOPTOPICS, OnDesktoptopics)
	ON_COMMAND(ID_GETTINGHELP, OnGettinghelp)
	ON_COMMAND(ID_WINDOWHELP, OnWindowhelp)
	ON_COMMAND(ID_FIELDHELP, OnFieldhelp)
	ON_COMMAND(ID_USERGUIDE, OnUserguide)
	ON_COMMAND(ID_IDD, OnIdd)
	ON_COMMAND(ID_SYSADM, OnSysadm)
	ON_COMMAND(ID_MSGBOARD_OPEN, OnMsgboardOpen)
	ON_COMMAND(ID_TECHSUPPORT, OnTechsupport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMainFrame toolbar buttons and status line indicators
// _____________________________________________________
//
// toolbar buttons - IDs are command buttons

static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR,
	ID_FILE_PRINT,
	ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



/////////////////////////////////////////////////////////////////////////////
// OnCreate
// ________

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// File menu stuff here
// ____________________

void CMainFrame::OnFileNew()
{
// WSPTODO - need to do
}

void CMainFrame::OnFileOpen()
{
// WSPTODO - need to do
}

void CMainFrame::OnFilePrint()
{
// WSPTODO - need to do
}

void CMainFrame::OnFileSave()
{
// WSPTODO - need to do
}


/////////////////////////////////////////////////////////////////////////////
// OnMsgboardDelete
// ________________

void CMainFrame::OnMsgboardDelete()
{
	gbl_msgboard_man.DeleteMsgBoard();
}



/////////////////////////////////////////////////////////////////////////////
// OnMsgboardMainIn
// ________________

void CMainFrame::OnMsgboardMainIn()
{
	gbl_msgboard_man.OpenMsgBoard
				(OWNER_IN_MSG_BOARD,
				 (LPCTSTR)gbl_msgboard_man.UserName());
}



/////////////////////////////////////////////////////////////////////////////
// OnMsgboardMainOut
// _________________
//
// No longer any need for this

//void CMainFrame::OnMsgboardMainOut()
//{
//	gbl_msgboard_man.OpenMsgBoard
//				(OWNER_OUT_MSG_BOARD,
//				 (LPCTSTR)gbl_msgboard_man.UserName());
//}


/////////////////////////////////////////////////////////////////////////////
// OnMsgboardOpen
// ______________

void CMainFrame::OnMsgboardOpen()
{
	gbl_msgboard_man.OpenMsgBoard();
}



/////////////////////////////////////////////////////////////////////////////
// OnMessagesPostnewmessage
// ________________________

void CMainFrame::OnMessagesPostnewmessage()
{
	gbl_msgboard_man.PostNewMsg();
}



/////////////////////////////////////////////////////////////////////////////
// OnMsgboardSubcribe
// __________________

void CMainFrame::OnMsgboardsRefreshAll()
{
	gbl_msgboard_man.RefreshAllMsgBoards();
}



/////////////////////////////////////////////////////////////////////////////
// OnMsgboardSubcribe
// __________________

void CMainFrame::OnMsgboardSubcribe()
{
	gbl_msgboard_man.MsgBoardSubscribe();
}



void CMainFrame::OnMsgboardNew()
{
	gbl_msgboard_man.CreateMsgBoard();
}


/////////////////////////////////////////////////////////////////////////////
// OnUpdateMsgboardMainIn
// ______________________

void CMainFrame::OnUpdateMsgboardMainIn(CCmdUI* pCmdUI)
{
    pCmdUI->SetText((LPCTSTR)gbl_msgboard_man.UserInMsgBoardName());
}


/////////////////////////////////////////////////////////////////////////////
// DEBUG stuff
// ___________


#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG





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
	help_object.LaunchWindowHelp();	
}



void CMainFrame::OnFieldhelp() 
{
	help_object.LaunchFieldHelp();	
	
}

void CMainFrame::OnUserguide() 
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
