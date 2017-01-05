/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssHelp.h: interface for the CNtcssHelp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTCSSHELP_H__8DB07073_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)
#define AFX_NTCSSHELP_H__8DB07073_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "WindowHelpTopics.h"
#include "NtcssPaths.h"
#include "FieldHelpTopics.h"


#define DECLARE_HELP(help_class) extern help_class help_object;
#define DEFINE_HELP(help_class) help_class help_object;
#define AUTOMATIC_HELP(msg) if (help_object.CheckForHelpInfo(msg)) return true;

#define HT_NOT_DEFINED 1
#define MAX_HANDLES 25
#define IDR_HELP 172
#define	IDD_USERS_GUIDE 33000
#define	IDD_FIELD_HELP 33001
#define	IDD_WINDOW_HELP 33002


////////////////////////////////////////////////////////////////////////////////
class CNtcssHelp : public CWnd  
{
public:
	CNtcssHelp();
	virtual ~CNtcssHelp();

	bool Initialize();

	void SetHelpTopics(/*long window_help = HT_HelpContents1,*/
					   long field_help = HT_HelpContents1)
	{
		/*m_window_help_topic = window_help;*/
		m_field_help_topic = field_help;
	}

	void LaunchDesktopHelpTopics();
	void LaunchGettingHelp();
	void LaunchFieldHelp();
	void LaunchWindowHelp();
	void LaunchUsersGuide();
	void LaunchInterfaceDesignDocument();
	void LaunchSystemAdministration();
	void LaunchTechnicalSupport();

	bool CheckForHelpInfo(MSG* msg);

	void SemiAutomatic() { m_automatic = false;}
	void Automatic()     { m_automatic = true; }
	void SetWindowHelp(long lWinHelp) {m_window_help_topic=lWinHelp;} 

	HACCEL m_accelerators;

private:
	virtual bool SelectHelpTopics(int ctrl_id) = 0;
	void SetCurrentHelpInfo();
	int TranslateToCtrlId(HWND hwnd);
	bool DecrementHandleIndex(int& handle_index);
	void SaveHandle(HWND hwnd);

	//{{AFX_VIRTUAL(CNtcssHelp)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
	HWND m_handles[MAX_HANDLES];
	int m_handle_index;
	long m_window_help_topic;
	long m_field_help_topic;
	HWND m_callers_window_handle;
	CNtcssPaths m_paths;
	bool m_automatic;

protected:
	//{{AFX_MSG(CNtcssHelp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_NTCSSHELP_H__8DB07073_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)

