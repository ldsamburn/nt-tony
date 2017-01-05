/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// LogonBanner.cpp : implementation file
//

#include "stdafx.h"
#include "DT.h"
#include "LogonBanner.h"
#include "Mmsystem.h"
#include "DTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogonBanner dialog


CLogonBanner::CLogonBanner(CWnd* pParent,const CString& strMasterServer,
						   CNtcssConfig* NtcssConfig,const BOOL bVisible)
	: CDialog(CLogonBanner::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogonBanner)
	m_strPassword = _T("");
	m_strUserName = _T("");
	m_strServer = _T("");
	//}}AFX_DATA_INIT
	m_strMasterServer=strMasterServer;
	m_bAppAdmin=FALSE;
	m_pLRS=NULL;
	m_NtcssConfig=NtcssConfig;
	m_bVisible=bVisible;
	TRACE0(_T("In CLogonBanner Constructor \n"));

}

CLogonBanner::~CLogonBanner()
{
	TRACE0(_T("In CLogonBanner destructor\n"));
}



void CLogonBanner::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogonBanner)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_StaticPassword);
	DDX_Control(pDX, IDC_STATIC_NAME, m_StaticUserName);
	DDX_Control(pDX, IDC_LINE3, m_StaticLine3);
	DDX_Control(pDX, IDC_LINE2, m_StaticLine2);
	DDX_Control(pDX, IDC_LINE1, m_StaticLine1);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordCtrl);
	DDX_Control(pDX, IDC_USER_NAME, m_UserNameCtrl);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 32);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogonBanner, CDialog)
	//{{AFX_MSG_MAP(CLogonBanner)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NTCSS_WARNING_RESPONSE, OnSecbanPing)
	ON_MESSAGE(WM_HIT_OK, OnHitOK)

	ON_WM_WINDOWPOSCHANGING()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogonBanner message handlers


void CLogonBanner::OnChangePassword() 
{

	if(!m_btnOK.IsWindowEnabled())
	{
		UpdateData();
		if(!m_strUserName.IsEmpty() && !m_strPassword.IsEmpty())
			m_btnOK.EnableWindow();
	}

}

void CLogonBanner::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	m_StaticLine1.SetFontBold(TRUE);
	m_StaticLine1.SetFontName("Times New Roman");
	m_StaticLine1.SetFontSize(29);

	m_StaticLine2.SetFontBold(TRUE);
	m_StaticLine2.SetFontName("Times New Roman");
	m_StaticLine2.SetFontSize(19);

	m_StaticLine3.SetFontBold(TRUE);
	m_StaticLine3.SetFontName("Times New Roman");
	m_StaticLine3.SetFontSize(19);
	m_StaticLine3.SetTextColor(RGB(150,0,0));

	m_StaticUserName.SetFontBold(TRUE);
	m_StaticUserName.SetFontName("Times New Roman");
	m_StaticUserName.SetFontSize(19);

	m_StaticPassword.SetFontBold(TRUE);
	m_StaticPassword.SetFontName("Times New Roman");
	m_StaticPassword.SetFontSize(19);

	m_TextFont.CreateFont (19, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
	CEdit *ce=(CEdit *)GetDlgItem(IDC_USER_NAME);
	ce->SetFont(&m_TextFont);

	m_btnOK.SetFont(&m_TextFont);
	m_btnCancel.SetFont(&m_TextFont);

	CEdit *cp=(CEdit *)GetDlgItem(IDC_PASSWORD);
	cp->SetFont(&m_TextFont);

	if (!m_bVisible)
		PostMessage(WM_HIT_OK,0,0);
	else
		SetForegroundWindow();

}

void CLogonBanner::OnOK() 
{

	m_btnOK.EnableWindow(FALSE); 
	m_btnCancel.EnableWindow(FALSE);



	UpdateData(); 

		if(!m_bVisible)
		{
			DWORD dwRet=_MAX_PATH;
			::GetUserName(m_strUserName.GetBuffer(_MAX_PATH),&dwRet);
			m_strUserName.ReleaseBuffer();
			m_strPassword=_T("XYZNTCSSNAVMASSOINRIzyx"); //Backdoor Password
		}

		try
		{
		   //NMCI CHANGE... check if we got a server string from INI & 
		   //Call new func if true... if it fails set server to master
		   //Having a value in this
		/*	if(!m_NtcssConfig->m_csExServers.IsEmpty())
				if((m_strServer=GetPAS(m_NtcssConfig->m_csExServers)==_T("")
					m_strServer=m_strMasterServer; */

			TRACE("Calling Vaidate User: User->%s Pass->%s Master-> %s Server-> %s\n",
			m_strUserName,m_strPassword,m_strMasterServer,m_strServer);

			ValidateUser(m_strUserName,m_strPassword,m_strMasterServer,
			m_strServer.GetBuffer(16),m_bAppAdmin);

			TRACE1("Back From Vaidate User & server is %s \n",m_strServer);
		}
		catch(LPTSTR pStr)
		{
			::MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(pStr,MB_ICONEXCLAMATION);
			m_strUserName.Empty();
			m_strPassword.Empty();
			if(!m_bVisible)
				EndDialog(5); //if we couldn't get in wiffout banner return this

			UpdateData(FALSE);
			m_btnCancel.EnableWindow();
			m_UserNameCtrl.SetFocus();
			return;
		}
	
		m_strServer.ReleaseBuffer();

		try
		{
			TRACE0("Doing 1st NtcssDTInitialize\n");
			
			if(NtcssDTInitialize(NTCSS_DLL_VERSION))
				throw(_T("NtcssDTInitialize Failed"));
			
			TRACE0("Past 1st NtcssDTInitialize\n");
			
			if(NtcssDTInitialize(NTCSS_DLL_VERSION))
				throw(_T("NtcssDTInitialize Failed")); //TODO bug here... FIX THIS 2X CALL!!
			
			TRACE0("Past 2nd NtcssDTInitialize\n");
			
			if(!NtcssDLLInitialize(NTCSS_DLL_VERSION,NTCSS_APP_NAME,AfxGetInstanceHandle(),
				/*AfxGetMainWnd()->*/m_hWnd)) 
				                          
				
				throw(_T("DLL init Failed\n"));
			
			TRACE0("Past NtcssDLLInitialize\n");
			
			m_Hwnd=/*AfxGetMainWnd()->*/m_hWnd; 
			
			if((m_pLRS=GetLRS())==NULL)
				throw(_T("Error getting LRS")); 

			////New stuff for that 0 Groups mess////

			TRACE1(_T("The Number of groups is-> %d\n")
				   ,m_pLRS->m_nNumberOfGroups);

		    if(!m_pLRS->m_nNumberOfGroups)
				throw(_T("There are no Progroups in LRS")); 

			////New stuff for that 0 Program mess////
			
			TRACE0("Past GetLRS\n");
			
			if(Ntcss_SetLRS((void*)m_pLRS))
				throw(_T("Error setting LRS")); 
			
			TRACE0("Past Ntcss_SetLRS\n");
			
			if(!NtcssSetAuthServer(m_strServer))
				throw(_T("Error setting AuthServer")); 
			
			TRACE0("Past NtcssSetAuthServer\n");
			
			if(!startSecban())
				throw(_T("Starting Secban failed"));
			
			TRACE0("Past startSecban\n");
			
			NtcssPortDelete(); 
			
			TRACE0("Past NtcssPortDelete\n");
			
			
			
		}

	catch(LPTSTR pStr)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(pStr,MB_ICONEXCLAMATION);
		::ExitProcess(0); 

	} 

	catch(...)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Unknown exception caught in Logon Dialog"),MB_ICONEXCLAMATION);
		::ExitProcess(0); 
	}
}




BOOL CLogonBanner::startSecban()
{
 
	 STARTUPINFO stInfo;
	 PROCESS_INFORMATION prInfo;
	 BOOL bResult;
	 ::ZeroMemory( &stInfo, sizeof(stInfo) );
	 stInfo.cb = sizeof(stInfo);
	 stInfo.dwFlags=STARTF_USESHOWWINDOW;
	 stInfo.wShowWindow=SW_NORMAL; //SW_MINIMIZE
	 stInfo.lpReserved=NULL;
	 stInfo.lpReserved2=NULL;
	 stInfo.cbReserved2=0;
	 stInfo.lpDesktop=NULL;
	 stInfo.lpTitle=NULL;


	 DWORD dwTimeout = 1000 * 30;

	 CString strCmd;
	 strCmd.Format(_T("%s\\bin\\secban.exe"),m_NtcssConfig->m_csNtcssRootDir);
	 
	 bResult = ::CreateProcess(NULL, 
							 strCmd.GetBuffer(strCmd.GetLength()),
							 NULL, 
							 NULL, 
							 FALSE,
							 CREATE_NEW_PROCESS_GROUP
							 | NORMAL_PRIORITY_CLASS ,
							 NULL,
							 NULL ,
							 &stInfo, 
							 &prInfo);

	 TRACE(_T("Create Process returned -> %d\n"),::GetLastError());

	strCmd.ReleaseBuffer();

	CloseHandle(prInfo.hThread); 
	CloseHandle(prInfo.hProcess); 

	return bResult;
}

LONG CLogonBanner::OnSecbanPing(UINT wParam,LONG lParam)
{
	if(wParam==WM_SECACCEPT)
		CDialog::OnOK();
	else
	{
		if(m_pLRS)
			ClearLRS();
		
		TRACE0(_T("In OnClose() calling LogoutUser()"));
		LogoutUser();
		
		TRACE0(_T("In OnClose() calling Ntcss_ClearLRS()"));
		Ntcss_ClearLRS();
		
		
		CDialog::OnCancel();
	}
	return 0;
}

LONG CLogonBanner::OnHitOK(UINT wParam,LONG lParam)
{
    OnOK();
	return 0;
}

void CLogonBanner::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (!m_bVisible)
		lpwndpos->flags &= ~SWP_SHOWWINDOW ;
	CDialog::OnWindowPosChanging(lpwndpos);
}






