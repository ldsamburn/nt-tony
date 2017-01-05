/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DT.h"
#include "DTDlg.h"
#include "DTSocket.h"
#include "AFXIMPL.H"
#include "sys/stat.h"
#include "lm.h"
#include "afxconv.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CString& strVersion);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg(CString& strVersion) : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
	m_strVersion=strVersion;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDTDlg dialog

CDTDlg::CDTDlg(CWnd* pParent,CNtcssConfig* cfg,CString& strServer,LoginReplyStruct* pLRS,
			   const CString& strPassword,const BOOL bAppAdmin,HWND hLoginWnd)
	: CDialog(CDTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	TRACE(_T("In CDTDlg contructor\n"));
	m_hIcon=AfxGetApp()->LoadIcon(IDI_NTCSS);
	m_hIconDefault=AfxGetApp()->LoadIcon(IDI_EXPLODE);
	m_strRoot=cfg->m_csNtcssRootDir;
	m_strNtcssBin=m_strRoot+_T("\\bin");
	m_strHelp=m_strRoot+_T("\\help");
	m_bRunOnce=cfg->m_bRunOnce;
	m_bQueryKnownHosts=cfg->m_bQueryKnownHosts; //Vxyz
	m_bUDP=cfg->m_bUDPOn;
	m_strXEmulator=cfg->m_csXemulator; 
	m_strVersion=cfg->m_csVersion;
	m_strVCProgram=cfg->m_csVCProgram;
	m_pLRS=pLRS;
	m_pDTSocket=NULL;
	m_strExPGs=_T("");
	m_strServer=strServer;
	m_strDefaultServer=cfg->m_csMasterServer;
	m_strPassword=strPassword;
	m_bShowMessage=TRUE;
	m_bAppAdmin=bAppAdmin;
	m_strIniAppNameTag=cfg->m_csIniAppNameTag;
	m_hLoginWnd=hLoginWnd;
	m_strNtcssIniFile=cfg->m_csIniFile;
	TRACE(_T("Leaving CDTDlg contructor\n"));
}

CDTDlg::~CDTDlg()
{
	if(m_pDTSocket)
	{
		TRACE(_T("In Destructor... closing & deleting socket\n"));
		m_pDTSocket->ShutDown(2);
		m_pDTSocket->Close();
		delete m_pDTSocket;

		HWND hWnd=::FindWindow(_T("SECBAN"),NULL);
		if(hWnd!=NULL) //assume OnClose() was not closed && we need to loguser off of NTCSS
			::SendMessage(hWnd,SECBAN_KILLIT_MSG,0,0L);


	}
}

void CDTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDTDlg)
	DDX_Control(pDX, IDC_MENUBUTTON, m_btnMenu);
	DDX_Control(pDX, IDC_HELPBUTTON, m_btnHelp);
	DDX_Control(pDX, IDC_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_BACK, m_btnBack);
	DDX_Control(pDX, IDC_FORWARD, m_btnForward);
	//}}AFX_DATA_MAP

}

//	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)

BEGIN_MESSAGE_MAP(CDTDlg, CDialog)
	//{{AFX_MSG_MAP(CDTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_APC, OnApc)
	ON_UPDATE_COMMAND_UI(ID_APC, OnUpdateApc)
	ON_WM_INITMENUPOPUP()
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_FORWARD, OnForward)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_COMMAND(ID_BJQ, OnBjq)
	ON_UPDATE_COMMAND_UI(ID_BJQ, OnUpdateBjq)
	ON_BN_CLICKED(IDC_HELPBUTTON, OnHelpbutton)
	ON_COMMAND(ID_SH, OnSh)
	ON_UPDATE_COMMAND_UI(ID_SH, OnUpdateSh)
	ON_COMMAND(ID_FH, OnFh)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_RN, OnRn)
	ON_COMMAND(ID_MSGBRDS, OnMsgbrds)
	ON_UPDATE_COMMAND_UI(ID_MSGBRDS, OnUpdateMsgbrds)
	ON_COMMAND(ID_PDJ, OnPdj)
	ON_UPDATE_COMMAND_UI(ID_PDJ, OnUpdatePdj)
	ON_COMMAND(ID_PRQ, OnPrq)
	ON_UPDATE_COMMAND_UI(ID_PRQ, OnUpdatePrq)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PASSCHG, OnPasschg)
	ON_UPDATE_COMMAND_UI(ID_PASSCHG, OnUpdatePasschg)
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
	ON_BN_CLICKED(IDC_MENUBUTTON, OnMenubutton)
	ON_WM_ENDSESSION()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//ON_WM_CREATE()
///////////////////////////////////////////////////////////////////////////
// CDTDlg message handlers



///////////////////////////////////////////////////////////////////////////
// CDTDlg OnOutbarNotify
long CDTDlg::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
//the dll runremote is throwing exceptions
{
	switch (wParam)
	{
	case NM_FOLDERCHANGE:
		// cast the lParam to an integer to get the clicked folder
		return 0;
		
	case NM_OB_ITEMCLICK:
		{
			
			ProgramStruct* pProgramStruct;
			int index = (int) lParam;
			CString cs, cs1;
			cs1 = m_wndBar.GetItemText(index);
			pProgramStruct=(ProgramStruct*)m_wndBar.GetItemData(index);
			
			DWORD dwRet;
			
			if(!pProgramStruct->m_nFlags) //0 == regular client program
			{
				
				//new logic for EnvironmentVariables
				CString strPG=((GroupStruct*)m_wndBar.GetFolderData(m_wndBar.GetSelFolder()))->
					m_strGroupTitle;

				if(strPG==_T("NTCSS"))
					SetEnvironmentVariable(_T("APPNAME"),strPG);
				else
					if(m_strIniAppNameTag==_T("GROUP"))
						SetEnvironmentVariable(_T("APPNAME"),strPG);
					else
						SetEnvironmentVariable(_T("APPNAME"),pProgramStruct->m_strProgramTitle);

				//make up for pg.ini file error
				CString strWorkingDir=pProgramStruct->m_strWorkingDir;
				if(!strWorkingDir.CompareNoCase(m_strRoot))
					strWorkingDir=m_strNtcssBin+_T("\\");
				//make up for pg.ini file error

				//convert from log file name to short
				TCHAR sbuf[_MAX_PATH];
				if(::GetShortPathName(strWorkingDir,sbuf,_MAX_PATH))
				{
					TRACE2(_T("Converted %s to %s \n"),strWorkingDir,sbuf);
					strWorkingDir=sbuf;

				}
 
					
				if(STAT(strWorkingDir + CString(pProgramStruct->m_strProgramFile))) 
				{

					CString cmd;
					cmd.Format(_T("%s%s %s"),strWorkingDir,pProgramStruct->m_strProgramFile,
						                      pProgramStruct->m_strCmdLineArgs);
				
					TRACE("Running -> " + cmd);
					
					//Append to PATH
					TCHAR appPath[_MAX_PATH];
					CString oldPath;
					CString newPath;

					DWORD need=::GetEnvironmentVariable(_T("Path"),
						                   oldPath.GetBuffer(_MAX_PATH),_MAX_PATH);

					if(need>0)
					{

						::GetEnvironmentVariable(_T("Path"),
											   oldPath.GetBuffer(need),need);

						oldPath.ReleaseBuffer();

					
    					if(NtcssGetAppPath(pProgramStruct->m_strProgramFile,appPath,_MAX_PATH))
						{
							newPath.Format(_T("%s;%s"),oldPath,appPath);
							::SetEnvironmentVariable(_T("Path"),newPath);
						}

					}

						
					startProcess(cmd,CString(strWorkingDir),dwRet);

					if(!newPath.IsEmpty())
						::SetEnvironmentVariable(_T("Path"),oldPath);

					return 0;
				}
				else
				{
					CString temp;
					temp.Format(_T("Can't find Program %s\\%s"),
					strWorkingDir,
					pProgramStruct->m_strProgramFile);
					::MessageBeep(MB_ICONEXCLAMATION);
					AfxMessageBox(temp,MB_ICONEXCLAMATION);
					return 0;
				}  
			}

			
			try
			{
				TCHAR lpszTemp[_MAX_PATH];
				CString strApp=((GroupStruct*)m_wndBar.GetFolderData(m_wndBar.GetSelFolder()))->
					m_strGroupTitle;
				//no need to differentiate between 1 & 2 
			/*	if(pProgramStruct->m_nFlags==1)
				{
					_stprintf(lpszTemp,_T("%s%s"),pProgramStruct->m_strWorkingDir,
						pProgramStruct->m_strProgramFile);
					RunRemote(lpszTemp,strApp.GetBuffer(16));
				} */
				if(pProgramStruct->m_nFlags==1 || pProgramStruct->m_nFlags==2)
				{
					_stprintf(lpszTemp,_T("%s%s %s"),pProgramStruct->m_strWorkingDir,
						pProgramStruct->m_strProgramFile,pProgramStruct->m_strCmdLineArgs);
			        RunRemote(lpszTemp,strApp.GetBuffer(16));
				}
				else if(pProgramStruct->m_nFlags==3)
				{
				
					CSelHostDlg dlg(NULL,pProgramStruct->m_strCmdLineArgs);
					::ReleaseCapture(); //VERY NECCESARY
				
					if(dlg.DoModal()==IDOK)
					{
						_stprintf(lpszTemp,_T("%s%s"),pProgramStruct->m_strWorkingDir,
						pProgramStruct->m_strProgramFile);
						RunRemote(lpszTemp,strApp.GetBuffer(16),dlg.m_strHost.GetBuffer(16)); 
					}
				}
			}
			catch(LPTSTR pStr)
			{
				::MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(pStr,MB_ICONEXCLAMATION);
				
			}
			catch(...)
			{
				::MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(_T("Unknown exception caught in RunRemote"),
					MB_ICONEXCLAMATION);
			}
			
			
		}
		return 0;
		
	case NM_OB_ONLABELENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
		// return 1 to do the change and 0 to cancel it
		// MAKE PROPERTIES
		return 1;
		
	case NM_OB_ONGROUPENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
		// return 1 to do the change and 0 to cancel it
		// MAKE PROPERTIES
		return 1;
		
	case NM_OB_DRAGITEM:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
		// return 1 to do the change and 0 to cancel it
		return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
// CDTDlg OnInitMenuPopup

void CDTDlg::OnInitMenuPopup(CMenu* pMenu, UINT, BOOL bSysMenu)
//this had to be taken out of CFrameWnd to allow on command update to work in a dialog
//based project
{
	AfxCancelModes(m_hWnd);

	if (bSysMenu)
		return;     // don't support system menu

	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, TRUE);  //this line changed from MFC 
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;

	  }

}

//-------------------------------------------------------------

BOOL CDTDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	TRACE(_T("In CDTDlg OnInitDialog\n"));

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    //Needed this hack to replace the stuff in the session table the
	//login Winder Wif this one

	TRACE(_T("In CDTDlg OnInitDialog calling NtcssUpdateDTSessionValues\n"));
	
	NtcssUpdateDTSessionValues(m_hLoginWnd,m_hWnd,GetCurrentProcessId(),
							   GetCurrentThreadId());

	// Set up Buttons///////////////////////////////
	

	m_btnForward.SetIcon(IDI_BACK257,IDI_BACK256);
	m_btnSearch.SetIcon(IDI_SEARCH256);
	m_btnHelp.SetIcon(IDI_HELP256);
	m_btnMenu.SetIcon(IDI_MENU256);
    m_btnBack.SetIcon(IDI_NEXT257,IDI_NEXT256);

	CString temp=_T("Previous Group of Apps");
	m_btnForward.SetTooltipText(&temp);
	temp=_T("Next Group of Apps");
	m_btnBack.SetTooltipText(&temp);
	temp=_T("Search for specific App");
	m_btnSearch.SetTooltipText(&temp);  


   	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect RectMain;
	GetClientRect(RectMain);  // get dialogs size & size control to it

//	m_wndBar.Create(WS_CHILD | WS_VISIBLE, CRect(15, 15, 120, 200), this, IDD_DT_DIALOG); 
	m_wndBar.Create(WS_CHILD | WS_VISIBLE, CRect(RectMain.left,RectMain.top+20, 
	RectMain.right,RectMain.bottom-25), this, IDD_DT_DIALOG);



	//take out the edit & remove ability from Items & Groups
	m_wndBar.ModifyFlag(m_wndBar.fEditGroups | m_wndBar.fEditItems |
						m_wndBar.fRemoveItems | m_wndBar.fRemoveGroups,0); 

	m_wndBar.SetOwner(this); 

	//TODO grow size has to be VERY BIG pagesize*64

	m_imgLarge.Create(32, 32, ILC_MASK, 0, 64);
	m_imgSmall.Create(16, 16, ILC_MASK, 0, 64);

	m_imgLarge.Add(m_hIcon); 
	m_imgSmall.Add(m_hIcon);

	m_wndBar.SetImageList(&m_imgLarge, CGfxOutBarCtrl::fLargeIcon);
	m_wndBar.SetImageList(&m_imgSmall, CGfxOutBarCtrl::fSmallIcon); 

    m_wndBar.SetAnimationTickCount(15); 

	m_wndBar.SetAnimSelHighlight(200); 

	m_wndBar.SetSmallIconView(false); 

	SetUpTemplateList(m_pLRS);
	DoInitializations(m_pLRS);


//Add STATUSBAR													


	RectMain.top = RectMain.bottom- 25;
    	
	if(!m_StatBar.Create(WS_CHILD | WS_BORDER | WS_VISIBLE ,RectMain,this,
        				        IDC_STATUSBAR))

	TRACE0(_T("Status bar not created\n"));
  	
	m_Widths[0] =-1;

	m_StatBar.SetMinHeight(25);
	m_StatBar.SetParts(1,m_Widths); 
	temp.Format(_T("User> %s (%s)"),m_strUserName,m_strServer);
	m_StatBar.SetText(temp,0,0);  

//kick off the UDP server if the citrix flag ISNT set
	TRACE(_T("In CDTDlg OnInitDialog kicking of UDP listener\n"));

	if(m_bUDP)
	{

		m_pDTSocket=new CDTSocket(this);
		BOOL bLinger=FALSE;


		if(!m_pDTSocket->Create(NTCSS_DATAGRAM_PORT,SOCK_DGRAM))
		{
			int nError=m_pDTSocket->GetLastError();
			::MessageBeep(MB_ICONEXCLAMATION);
			CString tmp;
			tmp.Format(_T("UDP Socket Creation Failed... Error #-> %d"),nError);
			AfxMessageBox(tmp,MB_ICONEXCLAMATION);
			::ExitProcess(0);
		}

	} 


	if(isXEmulator())
	{
		DWORD dwRet;
		startProcess(m_strXEmulator,GetDir(m_strXEmulator),dwRet);
	}

	TRACE(_T("Leaving OnInitDialog()\n"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CDTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout(m_strVersion);
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDTDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




//-------------------------------------------------------------------

BOOL CDTDlg::startProcess(CString& sCmdLine,CString& sRunningDir,
						  DWORD& dwRetValue)
{
 
 STARTUPINFO stInfo;
 PROCESS_INFORMATION prInfo;
 BOOL bResult;
 ::ZeroMemory( &stInfo, sizeof(stInfo) );
 stInfo.cb = sizeof(stInfo);
 stInfo.dwFlags=STARTF_USESHOWWINDOW;
 stInfo.wShowWindow=SW_SHOW; //SW_MINIMIZE


 ::SetCurrentDirectory(sRunningDir);


 bResult = ::CreateProcess(NULL, 
                         (LPSTR)(LPCSTR)sCmdLine, 
                         NULL, 
                         NULL, 
                         TRUE,
                         CREATE_NEW_CONSOLE
                         | NORMAL_PRIORITY_CLASS ,
                         NULL,
                         NULL,
                         &stInfo, 
                         &prInfo);

 dwRetValue = ::GetLastError();
 
 // Don't write these two lines if you need
 ::CloseHandle(prInfo.hThread); 
 // to use these handles
 ::CloseHandle(prInfo.hProcess);
 
 if(!bResult)
 {
	 display_error(dwRetValue);
	 return FALSE;
 }
 else return TRUE;	

}

//-------------------------------------------------------------------

BOOL CDTDlg::startProcessAndWait(CString& sCmdLine,CString& sRunningDir,
								 DWORD& dwRetValue)
{

 //Use this 1 for the CM program
 

 DWORD dwTimeout = 1000 * 300; //300s
					
					
 STARTUPINFO stInfo;
 PROCESS_INFORMATION prInfo;
 BOOL bResult;
 ::ZeroMemory( &stInfo, sizeof(stInfo) );
 stInfo.cb = sizeof(stInfo);
 stInfo.dwFlags=STARTF_USESHOWWINDOW;
 stInfo.wShowWindow=SW_SHOW;//SW_MINIMIZE;

 ::SetCurrentDirectory(sRunningDir);


 bResult = ::CreateProcess(NULL, 
                          (LPSTR)(LPCSTR)sCmdLine, 
                          NULL, 
                          NULL, 
                          TRUE,
                          CREATE_NEW_CONSOLE 
                         | NORMAL_PRIORITY_CLASS,
                         NULL,
                         NULL,
                         &stInfo, 
                         &prInfo);

 dwRetValue = ::GetLastError();
 
 if (!bResult)
{
	 display_error(dwRetValue);
	 return FALSE;
}

DWORD StartTime = ::GetTickCount();

TRACE1("TIME -> %d\n",StartTime);

BOOL bTimeOut=FALSE;
MSG msg;

while (WaitForSingleObject(prInfo.hProcess, 0) == WAIT_TIMEOUT)
{
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if(::GetTickCount() > (StartTime + dwTimeout))
	{
		bTimeOut=TRUE;
		break;
	}
}	

  ::CloseHandle(prInfo.hThread); 
  ::CloseHandle(prInfo.hProcess); 

 TRACE("EXITING LOOP...time-> %d\n",::GetTickCount());

 if (bTimeOut) 
 { 
	 TRACE("DETECTED TIME OUT\n");
	 return FALSE;
 }
 else
 {
	TRACE("GOOD EXIT!!! \n");
	return TRUE;
 }
}


//-------------------------------------------------------

void CDTDlg::UDPSelect()
{

	CString strIP;
	TCHAR strMessage[_MAX_PATH];

	UINT uPort;

	::ZeroMemory(strMessage,sizeof(strMessage));

	m_pDTSocket->ReceiveFrom(strMessage,_MAX_PATH,strIP,uPort);


	TRACE1(_T("In UDP Message Handler... message is <%s>\n"),strMessage);

	CString strCommand(strMessage);
	strCommand.TrimRight();
	const int nDataBeginning=20;

	if(strCommand==_T("MESSAGEBOX"))
	{
		SetForegroundWindow();
		::MessageBeep(MB_ICONEXCLAMATION);
		MessageBox((const char *)&strMessage[nDataBeginning],_T("Server Message"),MB_ICONEXCLAMATION);
		return;

	}

	else if(strCommand==_T("APPMSGBOX"))
	{

		TCHAR strAppCh[16];
		::ZeroMemory(strAppCh,sizeof(strAppCh));
		memcpy(strAppCh,&strMessage[nDataBeginning],15);
		CString strApp(strAppCh);
		strApp.TrimRight();
		TCHAR strMsgCh[80];
		_tcsncpy(strMsgCh,&strMessage[36],80);
		CString strMsg(strMsgCh);

		if(!FindApp(strApp))
			return;

		SetForegroundWindow();
		::MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(strMsg,strApp + _T(" Message"),MB_ICONEXCLAMATION);
 		 return;
	}

	else if(strCommand==_T("FORCELOGOFF"))
	{
		 NtcssKillProcesses(); 
		 TRACE0(_T("FORCELOGOFF is calling OnClose()\n"));
		 SetNoMessage();
		 PostMessage(WM_CLOSE,0,0);

 		 return;
	}
		
	else if(strCommand==_T("VALIDATETOKEN"))
	{
		//echo back name
		CString command=_T("VALIDATETOKENREPLY");
		CString data(m_strUserName);
		CString message;
		message.Format("%-*.*s%-*.*s",20,20,command,8,8,data);
		m_pDTSocket->SendTo((message.GetBuffer
		(message.GetLength())),message.GetLength(),uPort,strIP);
		message.ReleaseBuffer();
		TRACE1(_T("VALIDATETOKEN is returning-> %s\n"),message);
		return;
	}

	else if(strCommand==_T("EXECUTE_WS_PROGRAM"))
	{
		TCHAR strCmdCh[_MAX_PATH];
		_tcscpy(strCmdCh,&strMessage[20]);
		CString strCmd(strCmdCh);
		DWORD dwRet;
		startProcess(strCmd,GetDir(strCmd),dwRet);
		return; 
	}

	else if(strCommand==_T("NEWBBMSGCAMEIN"))
	{
		//The OLD DESKTOP just chimed when this happened
		//Put this back in for Java where this works
		::MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	TRACE0(_T("Unknown message sent to UDP server\n"));
}

//--------------------------------------------------------

void CDTDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CONTEXT));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
	point.x, point.y,pWndPopupOwner);	
}

/////////////////////////////////////
//   Command & CommandUI
/////////////////////////////////////

//--------------------------------------------------------

void CDTDlg::OnApc() 
{
	DWORD dwReturn; 
	startProcess(m_strNtcssBin+_T("\\app_prt.exe"),m_strNtcssBin,dwReturn);
}

//--------------------------------------------------------

void CDTDlg::OnUpdateApc(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bAppAdmin && STAT(m_strNtcssBin+_T("\\app_prt.exe")));
}

//--------------------------------------------------------

void CDTDlg::OnBjq() 
{
	DWORD dwReturn; 
	startProcess(m_strNtcssBin+_T("\\Spcque.exe"),m_strNtcssBin,dwReturn);
}

//--------------------------------------------------------

void CDTDlg::OnUpdateBjq(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bAppAdmin && STAT(m_strNtcssBin+_T("\\Spcque.exe")));
}

//--------------------------------------------------------

BOOL CDTDlg::STAT(const CString& strFile)
{
	struct _tstat buf;
	return _stat(strFile,&buf)==0;
}


    
//---------------------------------------------------

BOOL CDTDlg::DoInitializations(LoginReplyStruct* lrs) 
{
	//Pass LRS TO DLL & DO ALL the initializations

	m_pUserInfoStruct=&lrs->m_stUserInfo;

	//Fix User Name... 8 chars are a problem & not Null terminated
	TCHAR lpszUser[_MAX_PATH];
	::ZeroMemory(lpszUser,sizeof(lpszUser));
	_tcsncpy(lpszUser,m_pUserInfoStruct->m_strLoginName,8);
	m_strUserName=lpszUser;

	if((lrs->m_nNumberOfGroups%PageSize)==0) //save # of pages 
		m_nPages=lrs->m_nNumberOfGroups/PageSize;     
	else
		m_nPages=lrs->m_nNumberOfGroups/PageSize+1;

	TRACE1("Number of pages-> %d\n",m_nPages);
	NewPage(lrs,0);

	return TRUE;
}

//----------------------------------------------------

void CDTDlg::NewPage(const LoginReplyStruct* lrs,const int nPage,
				 const CString& strItem/*=_T("")*/) 
{
ASSERT(nPage>=0 && nPage<=m_nPages);


	ProgramStruct* pProgramStruct;
	int nStart=PageSize*nPage,nFolders;
	
	//remove old groups
	if((nFolders=m_wndBar.GetFolderCount()))
	{
		TRACE0("Doing deletes\n");
		for(int i=nFolders-1;i>=0;i--)
			m_wndBar.RemoveFolder(i);
		
		TRACE0("Did deletes OK\n");
	}
	
	//put in new groups
	LRSTemplateStruct p;
	CLRSList::iterator x;
	Advance(nStart,x);
	int nSel=0;
	int nIcon=0; 
	CString strIconFile;
	HICON hIcon;
	int nPrograms=0;
	
	//count # of programs per page
	
	for(int i=0;i<PageSize && x!=m_listlrs.end();i++)
	{
		p=*x;
		nPrograms+=((GroupStruct*)p.dwApp)->m_nNumberOfPrograms;
		++x;
	}
	
	TRACE1("Number of programs -> %d\n",nPrograms);
	
	Advance(nStart,x);
	
	m_imgLarge.SetImageCount(nPrograms);
	m_imgSmall.SetImageCount(nPrograms);
	
	for(i=0;i<PageSize && x!=m_listlrs.end();i++)
	{
		p = *x;
		m_wndBar.AddFolder(p.strPG,p.dwApp); 
		if(p.strPG==strItem) nSel=i;
		pProgramStruct=((GroupStruct*)p.dwApp)->m_stFirstProgram;
		for(int y=0;y < ((GroupStruct*)p.dwApp)->m_nNumberOfPrograms;y++)
		{
			
			strIconFile=pProgramStruct->m_strIconFile;
			
			if(!strIconFile.IsEmpty() && STAT(strIconFile))
			{
				
				hIcon=::ExtractIcon(AfxGetInstanceHandle(),strIconFile,
					pProgramStruct->m_nIconIndex);
				if(hIcon) 
				{
					m_imgLarge.Replace(nIcon,hIcon); 
					m_imgSmall.Replace(nIcon,hIcon);
					::DestroyIcon(hIcon);
				}
				else
				{
					m_imgLarge.Replace(nIcon,m_hIconDefault); 
					m_imgSmall.Replace(nIcon,m_hIconDefault);
				}
			}
			
			else
			{
				m_imgLarge.Replace(nIcon,m_hIconDefault); 
				m_imgSmall.Replace(nIcon,m_hIconDefault);
			}
			
			m_wndBar.InsertItem(i,y,pProgramStruct->m_strProgramTitle,nIcon++,
				(DWORD)pProgramStruct);
			
			TRACE("<%d> %s -> Adding Program #%d -> %s flag-> %d\n",i,((GroupStruct*)p.dwApp)->m_strGroupTitle,
				y,pProgramStruct->m_strProgramTitle,pProgramStruct->m_nFlags);
			
			pProgramStruct=pProgramStruct->m_stNextProgram;
		}
		
		++x;
	}
	
	m_wndBar.SetImageList(&m_imgLarge, CGfxOutBarCtrl::fLargeIcon);
	m_wndBar.SetImageList(&m_imgSmall, CGfxOutBarCtrl::fSmallIcon);
	
	m_nPage=nPage;
	
	
	m_wndBar.SetSelFolder(nSel);
	
}


//---------------------------------------------------------

void CDTDlg::SetUpTemplateList(const LoginReplyStruct* lrs) 
{
	//sticks the LRS into a sorted template list 
	
	try
	{
		GroupStruct* pGroupStruct=lrs->m_stFirstGroup;
		BOOL bIsVCProgram=isVCProgram();
		
		
		for(int i=0;i<lrs->m_nNumberOfGroups;i++)
		{
			//TODO need to exclude locked & CM all & put any failures on locked
			LRSTemplateStruct p(pGroupStruct->m_strGroupTitle,(DWORD)pGroupStruct);
			
			TRACE1(_T("SetUpTemplateList() Adding LRS group <%s> to LRS template\n"),
				pGroupStruct->m_strGroupTitle);
			
			if(bIsVCProgram)
			{
				DWORD dwRet;
				if(startProcessAndWait(m_strVCProgram + _T(" ") + pGroupStruct->m_strGroupTitle,
					GetDir(m_strVCProgram),dwRet))
					//if(!dwRet)
					m_listlrs.push_back(p);
			}
			else
				m_listlrs.push_back(p);
			
			pGroupStruct=pGroupStruct->m_stNextGroup;
		} 
		
		
		m_listlrs.sort();
		
		//cache the sorted PG List in ext CString
		
		LRSTemplateStruct p;
		CLRSList::iterator x;
		for(x=m_listlrs.begin();x!=m_listlrs.end();++x)
		{
			p=*x;
			if(m_strExPGs.IsEmpty())
				m_strExPGs=p.strPG;
			else
				m_strExPGs+=_T(',') + p.strPG;
		}
		
		TRACE1(_T("Built App String-> %s\n"),m_strExPGs);
	}
	
	catch(...)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("SetUpTemplateList() Fatal Exception Caught!!!"),MB_ICONEXCLAMATION);
		::ExitProcess(1);
	}
}

//-------------------------------------------------------

void CDTDlg::OnSearch() 
{
		CLookupDlg dlg(NULL,m_strExPGs);
		if(dlg.DoModal()==IDOK)
			LookupPage(dlg.m_strApp);
}

//-------------------------------------------------------

void CDTDlg::LookupPage(const CString& strApp) 
{
	
	CString temp;
	BOOL bFound=FALSE;
	
	for(int i=0;!bFound;i++)
	{
		if((temp=m_strExPGs.GetField(',',i))==_T(""))
			break;
		if(temp==strApp)
		{
			bFound=TRUE;
			break;
		}
	}


	ASSERT(bFound);
	if(!bFound) return;

	i++;

	//need the 0 based page# here!!!!
	i = (i%PageSize)==0 ? i/PageSize-1 : i/PageSize;

	NewPage(m_pLRS,i,strApp);
		
}

//---------------------------------------------------------

void CDTDlg::OnBack() 
//which is really forward
{
	if(m_nPage+1==m_nPages)
		return;
	NewPage(m_pLRS,m_nPage+1);
//	m_wndBar.SetSelFolder(0);
}

//---------------------------------------------------------

void CDTDlg::OnForward() 
//which is really back
{
	if(m_nPage==0)
		return;
	NewPage(m_pLRS,m_nPage-1);
//	m_wndBar.SetSelFolder(0);
}

//-----------------------------------------------------------

void CDTDlg::Advance(const int nAdvance,CLRSList::iterator& x)
{
	//Advances the template iterator
	x=m_listlrs.begin();
	for(int i=0;i<nAdvance && x!=m_listlrs.end();i++)
		++x;
}


//-----------------------------------------------------------

void CDTDlg::OnHelpbutton() 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_HELP));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	CRect RectMain;
	m_btnHelp.GetWindowRect(RectMain);  

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
	RectMain.left,RectMain.top,pWndPopupOwner);	
}

//-----------------------------------------------------------

void CDTDlg::OnSh() 
{
	::WinHelp(m_hWnd, m_strHelp+_T("\\dskscrn.hlp"), HELP_INDEX,0);
}

//-----------------------------------------------------------

void CDTDlg::OnUpdateSh(CCmdUI* pCmdUI) 
{
	// TODO: AX Tony if he would rather not enable if file is missing
    // If not delete this	
}

//-----------------------------------------------------------

void CDTDlg::OnFh() 
{
	::WinHelp(m_hWnd, m_strHelp+_T("\\dskfield.hlp"), HELP_INDEX,0);
}

void CDTDlg::OnAbout() 
{
	CAboutDlg dlg(m_strVersion);
	dlg.DoModal();
}

//-----------------------------------------------------------

void CDTDlg::OnRn() 
{
	::WinHelp(m_hWnd,m_strHelp + _T("\\803notes.hlp"), HELP_INDEX,0);
}



/////////////////////////////////////////////////////////////////////////////
// CLookupDlg dialog


CLookupDlg::CLookupDlg(CWnd* pParent,CStringEx strApps)
	: CDialog(CLookupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLookupDlg)
	m_strApp = _T("");
	//}}AFX_DATA_INIT
	m_strApps=strApps;
}


void CLookupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLookupDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_PGEDITBOX, m_FindCtrl);
	DDX_Control(pDX, IDC_PGLISTBOX, m_lbControl);
	DDX_LBString(pDX, IDC_PGLISTBOX, m_strApp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLookupDlg, CDialog)
	//{{AFX_MSG_MAP(CLookupDlg)
	ON_EN_CHANGE(IDC_PGEDITBOX, OnChangePgeditbox)
	ON_LBN_DBLCLK(IDC_PGLISTBOX, OnDblclkPglistbox)
	ON_LBN_SELCHANGE(IDC_PGLISTBOX, OnSelchangePglistbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLookupDlg message handlers

BOOL CLookupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString strApp;
	for(int i=0;;i++)
	{
		if((strApp=m_strApps.GetField(',',i))==_T(""))
			break;
		m_lbControl.InsertString(-1,strApp);
	}
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------

void CLookupDlg::OnChangePgeditbox() 
{
	CString temp;

	m_FindCtrl.GetWindowText(temp);
	m_btnOK.EnableWindow(FALSE);

	if(!temp.IsEmpty())
	{
		m_lbControl.SetCurSel(-1);
		m_btnOK.EnableWindow(m_lbControl.SelectString(0,temp)!=LB_ERR);
	}
	else
		m_lbControl.SetCurSel(-1);
}

//-------------------------------------------------------

void CLookupDlg::OnDblclkPglistbox() 
{
	if(m_lbControl.GetCurSel()!=LB_ERR)
		CDialog::OnOK();
}

//-------------------------------------------------------

void CLookupDlg::OnSelchangePglistbox() 
{
	if(m_lbControl.GetCurSel()!=LB_ERR)
	{
		CString temp;
		m_lbControl.GetText(m_lbControl.GetCurSel(),temp);
		m_FindCtrl.SetWindowText(temp);
		m_btnOK.EnableWindow(TRUE);
	}

}

//-------------------------------------------------------

void CDTDlg::OnMsgbrds() 
{
	DWORD dwReturn;
	startProcess(m_strNtcssBin+_T("\\MsgBoard.exe"),m_strNtcssBin,dwReturn);
}

//-------------------------------------------------------

void CDTDlg::OnUpdateMsgbrds(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bAppAdmin && STAT(m_strNtcssBin+_T("\\MsgBoard.exe")));
}

//-------------------------------------------------------

void CDTDlg::OnPdj() 
{
	DWORD dwReturn;
	startProcess(m_strNtcssBin+_T("\\pdj_conf.exe"),m_strNtcssBin,dwReturn);
}

//-------------------------------------------------------

void CDTDlg::OnUpdatePdj(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bAppAdmin && STAT(m_strNtcssBin+_T("\\pdj_conf.exe")));
}

//-------------------------------------------------------

void CDTDlg::OnPrq() 
{
	DWORD dwReturn;
	startProcess(m_strNtcssBin+_T("\\Pqueue.exe"),m_strNtcssBin,dwReturn);
}

//-------------------------------------------------------

void CDTDlg::OnUpdatePrq(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bAppAdmin && STAT(m_strNtcssBin+_T("\\Pqueue.exe")));
}

//--------------------------------------------------------

CString CDTDlg::GetDir(const CString& strPath)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_tsplitpath(strPath,drive, dir, fname, ext);
	return dir;
}

//--------------------------------------------------------

BOOL CDTDlg::KillSecBan() 
{
	HWND hWnd=::FindWindow(_T("SECBAN"),NULL);
	return hWnd ? ::SendMessage(hWnd,SECBAN_KILLIT_MSG,0,0L) : FALSE;
}

//--------------------------------------------------------

void CDTDlg::OnClose() 
{

	if(DisplayMessage())
	{

		if(NtcssGetNumTasks()>1)
		{
				::MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(_T("Exit ALL Active Programs First"),
				MB_ICONQUESTION);
				return;
		}

		::MessageBeep(MB_ICONQUESTION);
		if((AfxMessageBox(_T("Are you sure you want to Exit the Desktop"),
			MB_YESNO|MB_ICONQUESTION))==IDNO)
			return;
	}

	if(m_bQueryKnownHosts) //Vxyz
	{
		TRACE0(_T("In OnClose() calling CreateKnownHosts()"));
		CreateKnownHosts();
	}

	TRACE0(_T("In OnClose() calling ClearLRS()\n"));

	if(m_pLRS)
		ClearLRS();

	TRACE0(_T("In OnClose() calling LogoutUser()\n"));
	LogoutUser();

	TRACE0(_T("In OnClose() calling Ntcss_ClearLRS()\n"));
	Ntcss_ClearLRS();

	TRACE0(_T("In OnClose() calling KillSecBan()\n"));

	KillSecBan();

	TRACE0(_T("In OnClose() called KillSecBan()\n"));

	if(DisplayMessage())
		CDialog::OnClose(); //Calls DestroyWindow
	else
		::ExitProcess(0);     //hit from force logon only
}

//--------------------------------------------------------

BOOL CDTDlg::FindApp(const CString& strApp) 
{
	CString temp;
	for(int i=0;;i++)
	{
		if((temp=m_strExPGs.GetField(',',i))==_T(""))
			break;
		else
			if(temp==strApp)
				return TRUE;
	}

	return FALSE;
}

//------------------------------------------
void CDTDlg::display_error(DWORD error_code)
{
	LPTSTR lpBuffer;
		
	::FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    error_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpBuffer,
    0,
    NULL
  );
	  
  AfxMessageBox(lpBuffer);
  ::LocalFree(lpBuffer);
}

//-------------------------------------------

void CDTDlg::CheckCmdLine(CString& strCmdline)
{   //used to put "" on a path with spaces"
	if(strCmdline.Find(_T(' '))!=-1)
	{
		CString temp=strCmdline;
		strCmdline.Format(_T("\"%s\""),temp);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPassChangeDlg dialog


CPassChangeDlg::CPassChangeDlg(CWnd* pParent,const CString& strUserName,
							   const CString& strOldPassword)
	: CDialog(CPassChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassChangeDlg)
	m_strUserName = strUserName;
	m_strPassword = _T("");
	m_strVerifyPassword = _T("");
	m_strOldPassword = _T("");
	//}}AFX_DATA_INIT

	m_strOldPassword2=strOldPassword;
}


void CPassChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassChangeDlg)
	DDX_Control(pDX, IDC_OLDPASS, m_OldPassCtrl);
	DDX_Control(pDX, IDC_VERIFYPASS, m_VerifyCtrl);
	DDX_Control(pDX, IDC_NEWPASSWRD, m_PasswordCtrl);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_NEWPASSWRD, m_strPassword);
	DDX_Text(pDX, IDC_VERIFYPASS, m_strVerifyPassword);
	DDX_Text(pDX, IDC_OLDPASS, m_strOldPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassChangeDlg, CDialog)
	//{{AFX_MSG_MAP(CPassChangeDlg)
	ON_EN_SETFOCUS(IDC_NEWPASSWRD, OnSetfocusNewpasswrd)
	ON_EN_SETFOCUS(IDC_VERIFYPASS, OnSetfocusVerifypass)
	ON_EN_UPDATE(IDC_VERIFYPASS, OnUpdateVerifypass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassChangeDlg message handlers


void CDTDlg::OnPasschg() 
{
	try
	{
		CPassChangeDlg dlg(NULL,m_strUserName,m_strPassword);
		if(dlg.DoModal()==IDOK)
		{

			TRACE2(_T("Calling NtcssChangePassword old pass-> <%s> new pass-> <%s>\n"),
				m_strPassword,dlg.m_strPassword);

			NtcssChangePassword(dlg.m_strUserName.GetBuffer(dlg.m_strUserName.GetLength()),
								dlg.m_strOldPassword.GetBuffer(dlg.m_strOldPassword.GetLength()),

								dlg.m_strPassword.GetBuffer(dlg.m_strPassword.GetLength()));
		

     		dlg.m_strUserName.ReleaseBuffer(); //TODO get rid of this when TONY changes to const
			dlg.m_strPassword.ReleaseBuffer();
			dlg.m_strOldPassword.ReleaseBuffer();
		}

	}
	catch(...)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Unknown exception caught Change Password"),MB_ICONEXCLAMATION);
	}

}

void CDTDlg::OnUpdatePasschg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(1); //No Conditions
	
}


/////////////////////////////////////////////////////////////////////////////
// CSelHostDlg dialog


CSelHostDlg::CSelHostDlg(CWnd* pParent,LPCTSTR strCmdLine)
	: CDialog(CSelHostDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelHostDlg)
	m_strHost = _T("");
	//}}AFX_DATA_INIT
	m_strExCommandLine=strCmdLine;
}


void CSelHostDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelHostDlg)
	DDX_Control(pDX, IDC_SELHOSTCOMBO, m_SelHostCtrl);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_CBString(pDX, IDC_SELHOSTCOMBO, m_strHost);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelHostDlg, CDialog)
	//{{AFX_MSG_MAP(CSelHostDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelHostDlg message handlers

BOOL CSelHostDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString temp;

	for(int i=0;;i++)
	{
		if((temp=m_strExCommandLine.GetField(' ',i))==_T(""))
			break;
		m_SelHostCtrl.AddString(temp);
	}

	m_SelHostCtrl.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CPropDlg dialog


CPropDlg::CPropDlg(CWnd* pParent,LPCTSTR strTitle,LPCTSTR strFile,
				   LPCTSTR strArgs,LPCTSTR strWorkingDir)
	: CDialog(CPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropDlg)
	m_strArgs = strArgs;
	m_strFile = strFile;
	m_strTitle = strTitle;
	m_strWorkingDir = strWorkingDir;
	//}}AFX_DATA_INIT
}


void CPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropDlg)
	DDX_Text(pDX, IDC_PGARGS, m_strArgs);
	DDX_Text(pDX, IDC_PGFILE, m_strFile);
	DDX_Text(pDX, IDC_PGTITLE, m_strTitle);
	DDX_Text(pDX, IDC_WORKINGDIR, m_strWorkingDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropDlg, CDialog)
	//{{AFX_MSG_MAP(CPropDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropDlg message handlers

void CDTDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(::IsWindow(m_wndBar)) 
	{
		CRect R(0,20,cx,cy-25);

		m_wndBar.SetRedraw(false);

		m_wndBar.MoveWindow(&R); 

		R.top = cy - 20;

		m_StatBar.MoveWindow(&R);

		m_wndBar.SetRedraw(true);

		m_wndBar.Invalidate();

	}
	
	// TODO: Add your message handler code here
	
}

void CDTDlg::OnKickIdle()
{

	//note: m_nPage goes from 0... m_nPages from 1...  
	//back -> button

	m_btnBack.SetRedraw(false);
	m_btnForward.SetRedraw(false); 

    if(m_nPages>0 && m_nPage + 1 != m_nPages)
	{
		m_btnBack.SetIcon(IDI_NEXT257,IDI_NEXT256);
		m_btnBack.DrawBorder(TRUE);
	}
	else
	{
		//due to a bug in button clas this is as close as we can get to disabled
	    m_btnBack.SetIcon(IDI_NEXT258);
		m_btnBack.DrawBorder(FALSE);
	}

	//forward <- button
	if(m_nPages>1 && m_nPage!= 0)
	{
		m_btnForward.SetIcon(IDI_BACK257,IDI_BACK256);
		m_btnForward.DrawBorder(TRUE);
	}
	else
	{
		//due to a bug in button clas this is as close as we can get to disabled
	    m_btnForward.SetIcon(IDI_BACK258);
		m_btnForward.DrawBorder(FALSE);
	}

	m_btnBack.SetRedraw(true);
	m_btnForward.SetRedraw(true); 
	m_btnBack.Invalidate();
	m_btnForward.Invalidate();

}


void CDTDlg::OnMenubutton() 
{

	RECT rect;

	m_btnMenu.GetWindowRect(&rect);

	CPoint point;
	point.x = rect.left; 
	point.y = rect.bottom; 

    CMenu menu;
	VERIFY(menu.LoadMenu(IDR_CONTEXT));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	CRect RectMain;
    GetClientRect(RectMain);  // get dialogs size & size control to it

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
	point.x, point.y, pWndPopupOwner);	
	
}



void CDTDlg::CreateKnownHosts()
 
//Keeps adding new [KNOWN_HOSTS] to the NTCSS.INI on this box

{

	CStringEx strHosts;
	CString strMaster=m_strDefaultServer;
	CString strIniFile=m_strNtcssIniFile;
	CString strHost;

	int nSize = (m_pLRS->m_nNumberOfGroups * SIZE_APP_NAME) 
		        + m_pLRS->m_nNumberOfGroups;

	LPTSTR pList = new char[nSize];

	TRACE1(_T("In CreateKnownHosts allocated <%d> bytes for known Hosts List\n"),nSize);
	//Replace or create [KNOWN_HOSTS] section
	if(NtcssGetHostNameList(pList,nSize))
	{
		strHosts = pList;
		if(strHosts.IsEmpty())
			return;

		for(int i=0;;i++)
		{
			if((strHost=strHosts.GetField(',',i))==_T(""))
				break;
			if(strHost!=strMaster)
				::WritePrivateProfileString(_T("KNOWN_HOSTS"),strHost,strHost,strIniFile);
		}

	}

	if (pList)
		delete [] pList;
}


BOOL CDTDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
      if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE) 
		  return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}




void CDTDlg::OnEndSession(BOOL bEnding) 
{
	LogoutUser();
	if(m_bQueryKnownHosts) //Vxyz
		CreateKnownHosts();

	CDialog::OnEndSession(bEnding);
}



void CPassChangeDlg::OnSetfocusNewpasswrd() 
{
	//On Get Focus make sure they enter something 
	//for the Old Password
	CString strPassword;
	m_OldPassCtrl.GetWindowText(strPassword);
	m_btnOk.EnableWindow(FALSE);

	if(strPassword.IsEmpty())
	{
//		MessageBeep(MB_ICONEXCLAMATION );
//		AfxMessageBox(_T("Old Password must be entered"),MB_ICONEXCLAMATION);
		TRACE("In Old PASS Setting Focus back\n");
		m_OldPassCtrl.SetFocus();
	}
	
}

void CPassChangeDlg::OnSetfocusVerifypass() 
{
	//On Get Focus make sure they enter something 
	//for the New Password
	CString strPassword;
	m_PasswordCtrl.GetWindowText(strPassword);
	m_btnOk.EnableWindow(FALSE);

	if(strPassword.IsEmpty())
	{
//		MessageBeep(MB_ICONEXCLAMATION );
//		AfxMessageBox(_T("Old Password must be entered"),MB_ICONEXCLAMATION);
		TRACE("In Old PASS Setting Focus back\n");
		m_PasswordCtrl.SetFocus();
	}
	
}

void CPassChangeDlg::OnUpdateVerifypass() 
{
	m_btnOk.EnableWindow();
}







void CPassChangeDlg::OnOK() 
{
		CString strPassword;
	m_PasswordCtrl.GetWindowText(strPassword);


	CString strNewPassword,strConfirmPassword;

	m_PasswordCtrl.GetWindowText(strNewPassword);
	m_VerifyCtrl.GetWindowText(strConfirmPassword);

	if(strNewPassword!=strConfirmPassword)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(_T("Bad Password Confirmation"),MB_ICONEXCLAMATION);
		m_PasswordCtrl.SetWindowText(_T(""));
		m_VerifyCtrl.SetWindowText(_T(""));
		m_PasswordCtrl.SetFocus();
		return;
	}
	
	CDialog::OnOK();
}
