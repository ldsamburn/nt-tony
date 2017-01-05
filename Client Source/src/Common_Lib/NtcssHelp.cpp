/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssHelp.cpp: implementation of the CNtcssHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "NtcssPaths.h"
#include "NtcssHelp.h"
//#include "DebugMessage.h" jj

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GETTING_HELP_FILE           "Ntcsshlp.hlp"
#define FIELD_HELP_FILE             "Dskfield.hlp"
#define USERS_GUIDE_FILE            "Dskug.hlp"
#define INTERFACE_DESIGN_FILE       "Ntcssapi.hlp"
#define SYSTEM_ADMINISTRATION_FILE  "Dsksag.hlp"
#define TECHNICAL_SUPPORT_FILE      "Techsupt.hlp"

CNtcssHelp::CNtcssHelp() :
	m_accelerators(NULL),
	m_automatic(true),
	m_handle_index(0)
{
	SetHelpTopics();
}

CNtcssHelp::~CNtcssHelp()
{
}

bool CNtcssHelp::Initialize()
{
	if (!Create(NULL,"HelpObject",WS_CHILD,CRect(0,0,0,0),AfxGetMainWnd(),0,NULL)) {  
		AfxMessageBox("Error creating help object.");                                           
		return false;                                                                           
	}
	
	if (!(m_accelerators = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_HELP)))) {
		AfxMessageBox("Error loading help accelerator keys.");                                   
		return false;        
	}

	m_callers_window_handle = AfxGetMainWnd()->m_hWnd;
	
	return true;
}

void CNtcssHelp::SaveHandle(HWND hwnd)
{
	m_handle_index = (m_handle_index + 1) % MAX_HANDLES;
	m_handles[m_handle_index] = hwnd;
	m_automatic = true;
}

bool CNtcssHelp::DecrementHandleIndex(int& handle_index)
{
	handle_index--;
	if (handle_index < 0)
		handle_index = MAX_HANDLES - 1;
	return (handle_index != m_handle_index);
}

void CNtcssHelp::SetCurrentHelpInfo()
{
	if (!m_automatic) {
		m_callers_window_handle = AfxGetMainWnd()->m_hWnd;
		return;
	}

	bool done = false;
	int current_handle_index = m_handle_index;
	int control_id;

	while (!done) {
		control_id = TranslateToCtrlId(m_handles[current_handle_index]);
		if (control_id != 0) {
			if (SelectHelpTopics(control_id)) {
				m_callers_window_handle = m_handles[current_handle_index];
				done = true;
			}
		}
		if (!DecrementHandleIndex(current_handle_index))
			done = true;
	}
}

int CNtcssHelp::TranslateToCtrlId(HWND hwnd)
{
	CWnd* wnd = this->FromHandle(hwnd);
	if (wnd == NULL) return 0;
	return wnd->GetDlgCtrlID();
}

bool CNtcssHelp::CheckForHelpInfo(MSG* pMsg)
{
	if (m_accelerators != NULL)
		if (TranslateAccelerator(m_hWnd, m_accelerators, pMsg))
			return true;

	if ((pMsg->message == WM_KEYUP) || (pMsg->message == WM_LBUTTONUP))
		SaveHandle(pMsg->hwnd);

	return false;
}

void CNtcssHelp::LaunchDesktopHelpTopics() 
{
	LaunchUsersGuide();
}

void CNtcssHelp::LaunchGettingHelp()
{
	::WinHelp(AfxGetMainWnd()->m_hWnd,m_paths.NtcssHelpDirectory(GETTING_HELP_FILE),HELP_FINDER,0);
}

void CNtcssHelp::LaunchFieldHelp()
{

	::WinHelp(m_callers_window_handle,m_paths.NtcssHelpDirectory(FIELD_HELP_FILE),HELP_CONTEXT,m_field_help_topic);

	//::WinHelp(m_callers_window_handle,m_paths.NtcssHelpDirectory("Dskfield.hlp"),HELP_SETPOPUP_POS,MAKELONG(x,y));
	//::WinHelp(m_callers_window_handle,m_paths.NtcssHelpDirectory("Dskfield.hlp"),HELP_CONTEXTPOPUP,field_help);
	//::WinHelp(m_callers_window_handle,m_paths.NtcssHelpDirectory("Dskfield.hlp"),HELP_CONTEXT,m_field_help_topic);
}

void CNtcssHelp::LaunchWindowHelp()
{
	
	::WinHelp(m_callers_window_handle,m_paths.NtcssHelpDirectory(USERS_GUIDE_FILE),HELP_CONTEXT,m_window_help_topic);
}

void CNtcssHelp::LaunchUsersGuide()
{
	::WinHelp(AfxGetMainWnd()->m_hWnd,m_paths.NtcssHelpDirectory(USERS_GUIDE_FILE),HELP_FINDER,0);
}

void CNtcssHelp::LaunchInterfaceDesignDocument()
{
	::WinHelp(AfxGetMainWnd()->m_hWnd,m_paths.NtcssHelpDirectory(INTERFACE_DESIGN_FILE),HELP_FINDER,0);
}

void CNtcssHelp::LaunchSystemAdministration()
{
	::WinHelp(AfxGetMainWnd()->m_hWnd,m_paths.NtcssHelpDirectory(SYSTEM_ADMINISTRATION_FILE),HELP_FINDER,0);
}

void CNtcssHelp::LaunchTechnicalSupport()
{
	::WinHelp(AfxGetMainWnd()->m_hWnd,m_paths.NtcssHelpDirectory(TECHNICAL_SUPPORT_FILE),HELP_FINDER,0);
}

BEGIN_MESSAGE_MAP(CNtcssHelp, CWnd)
	//{{AFX_MSG_MAP(CNtcssHelp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNtcssHelp message handlers

BOOL CNtcssHelp::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (LOWORD(wParam)) {
	case IDD_USERS_GUIDE:
		LaunchUsersGuide();
		break;

	case IDD_FIELD_HELP:
		SetCurrentHelpInfo();
		LaunchFieldHelp();
		break;

	case IDD_WINDOW_HELP:
		SetCurrentHelpInfo();
		LaunchWindowHelp();
		break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}
