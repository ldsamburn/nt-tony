/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppPrintConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "AFXIMPL.H"
#include "AppPrintConfig.h"
#include "AppPrintConfigDlg.h"
#include "LogFileDlg.h"
#include "msgs.h"
#include "sizes.h"
#include "ModifyOtDlg.h"
#include "LockedAppWarnDlg.h"
#include "APCHelp.h"
#include "StringEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LV_COLUMN listColumn;
LV_ITEM listItem;
char ini_section[MAX_INI_SECTION];

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

/////////////////////////////////////////////////////////////////////////////
// CAppPrintConfigDlg dialog

CAppPrintConfigDlg::CAppPrintConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppPrintConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppPrintConfigDlg)
	m_AppList = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIsLogFile=FALSE;
	new_node=NULL;                //current postchange
}

void CAppPrintConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppPrintConfigDlg)
	DDX_Control(pDX, IDC_PRINTERLIST, m_PrinterCtrl);
	DDX_Control(pDX, IDC_OUTPUTLIST, m_OutPutCtrl);
	DDX_Control(pDX, IDC_APPLIST, m_AppListCtrl);
	DDX_LBString(pDX, IDC_APPLIST, m_AppList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAppPrintConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CAppPrintConfigDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_APPLIST, OnSelchangeApplist)
	ON_COMMAND(ID_APPLY, OnApply)
	ON_COMMAND(ID_OTADD, OnOtadd)
	ON_COMMAND(ID_OTEDIT, OnOtedit)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_VIEWLOG, OnViewlog)
	ON_UPDATE_COMMAND_UI(ID_OTEDIT, OnUpdateOtedit)
	ON_UPDATE_COMMAND_UI(ID_OTDELETE, OnUpdateOtdelete)
	ON_UPDATE_COMMAND_UI(ID_OTADD, OnUpdateOtadd)
	ON_UPDATE_COMMAND_UI(ID_VIEWLOG, OnUpdateViewlog)
	ON_UPDATE_COMMAND_UI(ID_APPLY, OnUpdateApply)
	ON_COMMAND(ID_OTDELETE, OnOtdelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OUTPUTLIST, OnItemchangedOutputlist)
	ON_NOTIFY(NM_CLICK, IDC_PRINTERLIST, OnClickPrinterlist)
	ON_COMMAND(ID_DESKTOPHELP, OnDesktophelp)
	ON_COMMAND(ID_GETTINGHELP, OnGettinghelp)
	ON_COMMAND(ID_WINDOWHELP, OnWindowhelp)
	ON_COMMAND(ID_FIELDHELP, OnFieldhelp)
	ON_COMMAND(ID_USERGUIDE, OnUserguide)
	ON_COMMAND(ID_IDD, OnIdd)
	ON_COMMAND(ID_SYSADM, OnSysadm)
	ON_COMMAND(ID_TECHSUPPORT, OnTechsupport)
	ON_COMMAND(ID_VERSION, OnVersion)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_COMMAND(ID_RC_ADD, OnOtadd)
	ON_COMMAND(ID_RC_EDIT, OnOtedit)
	ON_COMMAND(ID_RC_VIEW, OnViewlog)
	ON_COMMAND(ID_RC_DELETE, OnOtdelete)
	ON_UPDATE_COMMAND_UI(ID_RC_EDIT, OnUpdateOtedit)
	ON_UPDATE_COMMAND_UI(ID_RC_DELETE, OnUpdateOtdelete)
	ON_UPDATE_COMMAND_UI(ID_RC_VIEW, OnUpdateViewlog)
	ON_COMMAND(ID_QUIT, OnQuit)
	//}}AFX_MSG_MAP
	 //ON_MESSAGE (WM_INITMENUPOPUP, OnInitMenuPopup)
END_MESSAGE_MAP()

//	ON_COMMAND(ID_QUIT, OnQuit)

/////////////////////////////////////////////////////////////////////////////
// CAppPrintConfigDlg message handlers

BOOL CAppPrintConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);

	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	
	 CWaitCursor wait;	// display wait cursor

	 help_object.Initialize();


	//init the DLL
	 if (!NtcssDLLInitialize
        (NTCSS_DLL_VERSION,
		NTCSS_APP_NAME,
        AfxGetInstanceHandle(),
        AfxGetMainWnd()->m_hWnd))
		
		::ExitProcess(0);

	 OTInit();
	
	 help_object.SetWindowHelp(HT_ApplicationPrinterConfiguration_WindowDescription);

	 GetDlgItem(IDAPPLY)->EnableWindow(FALSE);  //disable apply till we get something in ini
	//////////////////////////////////////////////////////////////////////////////////////
//	CImageList  *pimagelist;   
	CBitmap    bitmap;
	CBitmap    bitmap2;

   // Generate the imagelist and associate with the list control
	m_pimagelist = new CImageList(); 
	//pimagelist->Create(16, 16, TRUE, 1, 3);
	m_pimagelist->Create(16, 16, TRUE, 2, 5);
	bitmap.LoadBitmap(IDB_OTYPE);  
	m_pimagelist->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap2.LoadBitmap(IDB_PRINTERS);  
	m_pimagelist->Add(&bitmap2, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap2.DeleteObject();
	m_OutPutCtrl.SetImageList(m_pimagelist, LVSIL_SMALL);
	m_PrinterCtrl.SetImageList(m_pimagelist, LVSIL_SMALL);

	//////////////////////////////////////////////////////////////////////////////////

	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
	listItem.iSubItem=0;
	listItem.iImage   = 0;
	 
	 CString str_Columns="NAME,DESCRIPTION,DEFAULT PRINTER,BATCH PRINTER";

     static long OTHelpTopics[]={HT_Name_OutputType,HT_Description_OutputType,HT_DefaultPrinter,HT_BatchPrinter};

	 static long PrinterHelpTopics[]={HT_Name_Printer,HT_NTCSSHostName,HT_Classification_Printer,HT_Location_Printer,
							   HT_MaxRequestSize,HT_PrinterClass,HT_SuspendState};

		
	 m_OutPutCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\APC\\OT",
					 OTHelpTopics/*,HT_ApplicationPrinterConfiguration_WindowDescription*/); 


	 str_Columns="NAME,CLASSIFICATION,LOCATION,MAX REQUEST SIZE,PRINTER CLASS,SUSPEND STATE";

	 listItem.iImage   = 1;

	 m_PrinterCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\APC\\Printer",
					 PrinterHelpTopics/*,HT_ApplicationPrinterConfiguration_WindowDescription*/);

	 m_Popup.LoadMenu(IDR_RIGHTCLICK);



	FillAppListCtrl();

	
	return TRUE;  // return TRUE  unless you set the focus to a control 
}

void CAppPrintConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
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

void CAppPrintConfigDlg::OnPaint() 
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
HCURSOR CAppPrintConfigDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAppPrintConfigDlg::OnSelchangeApplist() 
{
	CString strApp,strNewHostName;
	m_AppListCtrl.GetText(m_AppListCtrl.GetCurSel(),strApp);
	strNewHostName=GetAuthServer(strApp);

	if(strNewHostName!=m_HostName)
	{
		if(GetDlgItem(IDAPPLY)->IsWindowEnabled()) //pending changes for this app
			OnApply();

		m_HostName=strNewHostName;
		GetIniFile();

        //see if this host is flat or rdms
		m_bRDMS=::GetPrivateProfileSection(_T("POSTGRES"),ini_section,
		                                MAX_INI_SECTION,m_GetFileName);

	}

	UpdateData(TRUE);
	UpdatePrinterList(m_AppList);
	UpdateOTList(m_AppList);
}

void CAppPrintConfigDlg::UpdatePrinterList(CString app)
{

	
	m_PrinterCtrl.DeleteAllItems();


	char *keys[6]={"PRINTER","SECURITY","LOCATION","MAXSIZE","CLASS","SUSPEND"};

	//fill in printer list for this app
	CString temp=app+"_PRTS";
	::GetPrivateProfileSection((LPCTSTR)temp,ini_section,MAX_INI_SECTION,m_GetFileName);
	char *p=ini_section;
	CString pname;
	if(*p==NULL) return;

	int x=0;
	do
	{
		
		p=strchr(p,'=')+1;
		pname=p;
				
		char tbuff[MAX_INI_SECTION];
		
		//get all keys for each printer
		for(int i=0;i<6;i++)
		{
			::GetPrivateProfileString((LPCTSTR)pname,keys[i],"!",tbuff,
									  MAX_INI_SECTION,m_GetFileName);
	
			m_PrinterCtrl.Write(tbuff); //5/4

		}

		p=strchr(p,'\0')+1;

	}while (*p!=NULL);
}

void CAppPrintConfigDlg::UpdateOTList(CString app)
{

	//fill in otype list for this app by looping through the output types for this
	//app in the INI file & then if they are not in the change list writing them in the 
	//control - then all adds & edits are dumped from change CList 

	m_OutPutCtrl.DeleteAllItems();
	::GetPrivateProfileSection((LPCTSTR)app+(CString)_T("_PROGROUP"),ini_section,MAX_INI_SECTION,m_GetFileName);
	char *p=ini_section;
	CString otname;
	if(*p!=NULL) 
	{

	char *otkeys[4]={"TITLE","DESCRIPTION","DEFAULTPRT","BATCH"};

	int x=0;
			
	do
	{
		
		p=strchr(p,'=')+1;
		otname=p;
	
		char tbuff[MAX_INI_SECTION];
	
		//get all keys for each output type
		for(int i=0;i<4;i++)
		{
			::GetPrivateProfileString((LPCTSTR)otname,otkeys[i],"!",tbuff,
									  MAX_INI_SECTION,m_GetFileName);
	
			if(i==0&&IsInOtypeChangeList(tbuff))
				break; //5/4

			m_OutPutCtrl.Write(tbuff);
			   
		}
	
		
		p=strchr(p,'\0')+1;

	}while (*p!=NULL);
	}

	//now dump CList changes for this app
	//walk backwards so we get most recent
	//change in control first

	POSITION pos=ChangeList.GetTailPosition(); 
	change_node* t;
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING;
	while (pos != NULL)  
	{
		t = ChangeList.GetPrev(pos);
		ls_findinfo.psz=(LPCTSTR)t->title;
		if(t->group==m_AppList && t->operation!="DELETE" && 
			m_OutPutCtrl.FindItem(&ls_findinfo,-1)==-1) //this makes sure only last
			                                             //change on CList is displayed
		{
			listItem.iItem=0;
			listItem.pszText=(char*)(LPCTSTR)t->title;
			listItem.lParam=m_OutPutCtrl.GetItemCount()+1;  
			m_OutPutCtrl.InsertItem(&listItem);
			m_OutPutCtrl.SetItemText(0,1,(LPCTSTR)t->description); 
			m_OutPutCtrl.SetItemText(0,2,(LPCTSTR)t->Default); 
			m_OutPutCtrl.SetItemText(0,3,(LPCTSTR)t->batch);
		}

	}
}

void CAppPrintConfigDlg::ExtractPrinters(CString title) 
//selects printers associated with this output type
//called by OnItemchangedOutputlist
{
	
   	char tbuff[MAX_INI_SECTION];
	CString temp= "",printers= "";
	//call function here to return printer number string from change list!!!
	//if false got to loop through otypes looking at title so we can get 
	//printer number string
//	m_PrinterCtrl.EnableWindow(TRUE);
	UnSelectPrintCtrl();  //unselect all
	printers=FindPrinterStringInChangeList(title);

	if(printers.IsEmpty()&&IsInOtypeChangeList(title))
		return;  //this catches new adds with no printers
	
	if(printers.IsEmpty()) //else get numeric string out of ini file
	{
		int i=0;
		while (1)
		{
			i++; 
			temp.Format("%s%s%d",m_AppList,"_OTYPE_",i);

			if(!::GetPrivateProfileSection((LPCTSTR)temp,tbuff,MAX_INI_SECTION,
								m_GetFileName))
				return;
		
			if((CString) (strchr(tbuff,'=')+1)==title)
				break;
		
		}

		::GetPrivateProfileString((LPCTSTR)temp,"PRTLIST","!",tbuff,
								  MAX_INI_SECTION,m_GetFileName);

	    if(!m_bRDMS) //the only difference between flat & RDMS 
			printers=NumericPrintStringToCString((CString) tbuff);// 2/7/00
		else
			printers=tbuff; //2/7/00
		
		
	}


	int nItem;
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING;

	//now just parse string and select matching printers in list ctrl
	//printer string is in pname@hostname format so we ignore @host

	char *token = strtok((char*)(LPCTSTR) printers, "," );
	while( token != NULL )
	{
		temp=token;
	//	temp=temp.Left(temp.Find('@')); 12/18/98
		ls_findinfo.psz=temp;
	//	AfxMessageBox("Extracting -> " + temp);
		if((nItem=m_PrinterCtrl.FindItem(&ls_findinfo,-1))!=-1)
		m_PrinterCtrl.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	//	else AfxMessageBox("Failed to match");
		token = strtok( NULL, "," );
	}
//	m_PrinterCtrl.EnableWindow(FALSE);
}


void CAppPrintConfigDlg::OnUpdateOtedit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);
	
}

void CAppPrintConfigDlg::OnOtedit() 
{
	new_node=new change_node;
    new_node->operation="EDIT";
	int nIndex=m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED); //save this because dlg ok clobers 
	CModifyOtDlg dlg(NULL,"EDIT",this);                      //the selected state in this ctrl
	if(dlg.DoModal()==IDOK&& !new_node->title.IsEmpty())     //if this empty then they hit ok
		                                                     //but didn't change anything
	{   
		ChangeList.AddTail(new_node);
		AddNodeToCtrl(nIndex);
		//UnSelectPrintCtrl();
		GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
		//OnSelchangeApplist();
	}
	else
		delete new_node;
	
}

void CAppPrintConfigDlg::OnApply() 
{

	if(!OutputChangeList())
		AfxMessageBox("Apply failed"); //TODO handle failure
	else
	{
		GetDlgItem(IDAPPLY)->EnableWindow(FALSE);
		m_bIsLogFile=TRUE;
		GetIniFile();
		UpdateOTList(m_AppList);
		UpdatePrinterList(m_AppList);
	}

	
}

void CAppPrintConfigDlg::OnOtadd() 
{
	new_node=new change_node;
	new_node->operation="ADD";
	CModifyOtDlg dlg(NULL,"ADD",this);
	if(dlg.DoModal()==IDOK) //dialog will fill in new node structure
	{
		ChangeList.AddTail(new_node);
		UnSelectPrintCtrl();
		AddNodeToCtrl();
		GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
	}
	else
		delete new_node;
}

void CAppPrintConfigDlg::OnUpdateOtadd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_AppListCtrl.GetCurSel()!=LB_ERR &&
	m_PrinterCtrl.GetItemCount()>0);
	
}


void CAppPrintConfigDlg::OnUpdateOtdelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);
	
}


void CAppPrintConfigDlg::OnUpdateViewlog(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bIsLogFile);
	
}

void CAppPrintConfigDlg::OnViewlog() 
{
	CLogFileDlg dlg(NULL,m_LogFileName);
	dlg.DoModal();
	
}

void CAppPrintConfigDlg::OnQuit() 
{
	OnCancel();
}


void CAppPrintConfigDlg::OnUpdateApply(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChangeList.GetCount()>0);
}


void CAppPrintConfigDlg::OnOtdelete() 
{
	
	int nIndex=m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED );

	change_node* t=new change_node;

	t->operation="DELETE";
	t->group=m_AppList; //Get from App List
	t->Default=GetDefaultPrinter();
	t->title=GetName();
	t->description=GetDescription();
	t->batch=GetBatch(); //yes no
	t->printers=GetPrinterString();

	m_OutPutCtrl.DeleteItem(nIndex);

	ChangeList.AddTail(t);
	UnSelectPrintCtrl();
	GetDlgItem(IDAPPLY)->EnableWindow(TRUE);
	
}


///////////////////////////////////////////////////////////////////////////////
void CAppPrintConfigDlg::OnInitMenuPopup(CMenu* pMenu, UINT, BOOL bSysMenu)
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
///////////////////////////////////////////////////////////////////////////////


BOOL CAppPrintConfigDlg::OTInit()
{
	CWaitCursor wait;	// display wait cursor

		
	char buf[_MAX_PATH];

	//Get NTCSS root & make filenames out of it
	::GetPrivateProfileString("NTCSS","NTCSS_SPOOL_DIR","!",buf,sizeof(buf),"ntcss.ini");

	if (strstr(buf,"!"))
	{
		AfxMessageBox("There is no NTCSS ini file!");
		m_GetFileName="";
		return FALSE;
	}

    m_GetFileName=(CString)buf+"\\otype.ini";
	m_LogFileName=(CString)buf+"\\otype.log";
	m_PutFileName=(CString)buf+"\\otypeupd.ini";

	_unlink((LPCTSTR) m_PutFileName);       //kill old log file


	return TRUE;
				
}


void CAppPrintConfigDlg::UnSelectPrintCtrl() 
//unselects everything in print control list since this list is static
//this avoids having to reread everytime something changes
{
	int nIndex=-1;
//	m_PrinterCtrl.EnableWindow(TRUE);
	while((nIndex=m_PrinterCtrl.GetNextItem( nIndex, LVNI_SELECTED))>=0 )
		m_PrinterCtrl.SetItemState(nIndex,0,LVNI_SELECTED); //this deselects
		
}

void CAppPrintConfigDlg::AddNodeToCtrl(int nIndex)
{
	if (new_node->operation=="ADD")
	{

		m_OutPutCtrl.Write(new_node->title);
		m_OutPutCtrl.Write(new_node->description);
		m_OutPutCtrl.Write(new_node->Default);
		m_OutPutCtrl.SelectItem(m_OutPutCtrl.Write(new_node->batch));
	}
	
	else //edit ='s overwrite, only change 3 displayed things + assoc printers
	{
		
		if (new_node->description!=GetDescription(nIndex))
			m_OutPutCtrl.SetItemText(nIndex,1,new_node->description);
		if (new_node->Default!=GetDefaultPrinter(nIndex))
			m_OutPutCtrl.SetItemText(nIndex,2,(LPCTSTR)new_node->Default); 
		if (new_node->batch!=GetBatch(nIndex))
			m_OutPutCtrl.SetItemText(nIndex,3,(LPCTSTR)new_node->batch);
		
	} 

	ExtractPrinters(new_node->title); //update associated printers	 

}

CString CAppPrintConfigDlg::FindPrinterStringInChangeList(CString otype)
//Called in extract printers to see if this output type has been modified
//in which case we use the values in the CList instead of the INI file
{
		
	change_node *t;
	BOOL found=FALSE;
	char temp[512]; //ugly but fixed memory leak from hell
	temp[0]='\0';
	
	POSITION pos=ChangeList.GetTailPosition(); //most recent change will be on end

	while (pos != NULL)  //loop till we find printer name
	{
		t = ChangeList.GetPrev(pos);
		
		if(t->title==otype&&t->group==m_AppList)
		{
			found=TRUE;
			strcpy(temp,(LPCTSTR)t->printers);
			break;
		}
	
	}
	
	return (CString)temp;
}

BOOL CAppPrintConfigDlg::IsInOtypeChangeList(CString otype)
//called in UpdateOTList & ExtractPrinters to see if an original (INI) output type has 
//already been changed & therefore would be inserted from change list
//and not the INI file

{

	POSITION pos=ChangeList.GetTailPosition(); 
	change_node* t;
	while (pos != NULL)  
	{
		t = ChangeList.GetPrev(pos);
		if(t->title==otype&&t->group==m_AppList)
			return TRUE;
	}

	return FALSE;
}


CString CAppPrintConfigDlg::GetName(int nIndex)
{
  return m_OutPutCtrl.GetItemText(m_OutPutCtrl.GetNextItem(nIndex,LVNI_SELECTED),0);
}

CString CAppPrintConfigDlg::GetDescription(int nIndex)
{
  return m_OutPutCtrl.GetItemText(m_OutPutCtrl.GetNextItem(nIndex,LVNI_SELECTED),1);
}

CString CAppPrintConfigDlg::GetDefaultPrinter(int nIndex)
{
  return m_OutPutCtrl.GetItemText(m_OutPutCtrl.GetNextItem(nIndex,LVNI_SELECTED),2);
}

CString CAppPrintConfigDlg::GetBatch(int nIndex)
{
  return m_OutPutCtrl.GetItemText(m_OutPutCtrl.GetNextItem(nIndex,LVNI_SELECTED),3);
}


CString CAppPrintConfigDlg::GetPrinterString()
{
//called by delete to retrieve currently selected printers


	CString temp;

	int i = m_PrinterCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	while( i != -1 )
	{
		if(temp.IsEmpty())
			temp=m_PrinterCtrl.GetItemText(i,0);
		else
			temp+=','+m_PrinterCtrl.GetItemText(i,0);
		
		i = m_PrinterCtrl.GetNextItem( i, LVNI_ALL | LVNI_SELECTED);
	 }
	


 //   AfxMessageBox(temp); //debug
		

	return temp;
}


BOOL  CAppPrintConfigDlg::OutputChangeList()
{
	char *ls_junk={NULL};
	int  revision=1;
	CString temp="OTYPE";
	CString temp2;

	::WritePrivateProfileSection("OTYPE",ls_junk,m_PutFileName);
	
	int cnt=ChangeList.GetCount();
	for (int i=1;i<=cnt;i++)
	{
		temp.Format("%s%d","CHANGE",i);
		::WritePrivateProfileString("OTYPE",temp,temp,m_PutFileName);
	}


	POSITION pos=ChangeList.GetHeadPosition();
	change_node *t;
	i=1;
	while (pos != NULL)  
	{
		t = ChangeList.GetNext(pos);
		temp.Format("%s%d","CHANGE",i);
		::WritePrivateProfileSection(temp,ls_junk,m_PutFileName);
		::WritePrivateProfileString(temp,"GROUP",t->group,m_PutFileName);
		::WritePrivateProfileString(temp,"OPERATION",t->operation,m_PutFileName);
		::WritePrivateProfileString(temp,"TITLE",t->title,m_PutFileName);
		::WritePrivateProfileString(temp,"DESCRIPTION",t->description,m_PutFileName);
		::WritePrivateProfileString(temp,"BATCH",t->batch,m_PutFileName);
		::WritePrivateProfileString(temp,"DEFAULTPRT",t->Default.GetLength()>0?
		t->Default:"",m_PutFileName);
		temp2.Format("%s%d%s","CHANGE",i++,"PRINTER");
		::WritePrivateProfileString(temp,"PRINTERS",t->printers.GetLength()>0?
		temp2:"",m_PutFileName);
		::WritePrivateProfileSection(temp2,ls_junk,m_PutFileName);
		//temp=NumericPrintStringToCString(t); 9-3
		char *token = strtok((char*)(LPCTSTR) /*9-3temp*/t->printers, "," );
		while( token != NULL )
		{
			::WritePrivateProfileString(temp2,token,token,m_PutFileName);
			token = strtok( NULL, "," );
		}

	}

	
	if(!PutIniFile()) 
		return(FALSE);


	return DeleteCList(); //delete change list
}


void CAppPrintConfigDlg::OnItemchangedOutputlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_OutPutCtrl.SetImageList(m_pimagelist, LVSIL_SMALL);
	listItem.iImage   = 0;
	if(pNMListView->uNewState&LVIS_SELECTED)
	{
	
		UnSelectPrintCtrl();
		ExtractPrinters(m_OutPutCtrl.GetItemText
		(pNMListView->iItem,0));
	}
	
	*pResult = 0;
}


CString CAppPrintConfigDlg::NumericPrintStringToCString(CString printers)
//called in extract printers to converts OT printer string to alpha so
//we can use same printer selection routine with change list and never have
//to mess with the numeric relationship again. This is only used when reading
//from an INI file & there is no entry in the change list

//only called for flat file version


{
	char *p;

	CString temp=m_AppList+"_PRTS";
	char tbuff[MAX_INI_SECTION];
	CString result="";

	::GetPrivateProfileSection((LPCTSTR)temp,tbuff,MAX_INI_SECTION,
								m_GetFileName);

	char *token = strtok((char*)(LPCTSTR) printers, "," );
	while( token != NULL )
	{
		p=tbuff;
		for(int i=1;i<=atoi(token);i++) 
		{
			p=strchr(p,'=')+1; //got the right printer now
			if (atoi(token)==i)
			{
				temp.Format("%c%s",',',p);
				result+=temp;
			}
			p=strchr(p,'\0')+1;
		}
		token = strtok( NULL, "," );
	}

	return result;
} 

BOOL CAppPrintConfigDlg::PutIniFile()
{
    CWaitCursor wait;	// display wait cursor

	
	if(!NtcssPutFile(m_HostName,m_ServerFileName,
				     m_PutFileName,TRUE)) return FALSE;


	_unlink( m_PutFileName);       //kill old log file
	
	CmsgPUTOTYPEINI msg;
	char server_file_name_buf[SIZE_SERVER_FILE_NAME];
	strcpy(server_file_name_buf,m_ServerFileName);

	if(!msg.Load(server_file_name_buf,m_HostName)) return FALSE;

	if(!msg.DoItNow()) return FALSE;
			
	if(!msg.UnLoad(server_file_name_buf)) return FALSE;

	return NtcssGetFile(m_HostName,server_file_name_buf,m_LogFileName,TRUE);

}


BOOL CAppPrintConfigDlg::DeleteCList()
{
	
	POSITION pos=ChangeList.GetHeadPosition(); 
	change_node* t;

	while (pos != NULL)  
	{
		t = ChangeList.GetNext(pos);
		delete t;
	}
	ChangeList.RemoveAll();
	return ChangeList.IsEmpty( );
}




void CAppPrintConfigDlg::OnCancel() 
{
	if(GetDlgItem(IDAPPLY)->IsWindowEnabled()) //warn if there is unapplied log file
		if(AfxMessageBox("Quit Without Saving Changes",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			return;

	
	CDialog::OnCancel();
}


void CAppPrintConfigDlg::OnClickPrinterlist(NMHDR* pNMHDR, LRESULT* pResult) 
//use this to keep them from changing selections in this listctrl while allowing them to
//sort and scroll. The currently selected here are used for the initial state on an edit
//so they can't be messed with
{
	
	m_OutPutCtrl.SetFocus();
	int nIndex=m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED);
	if (nIndex>=0)
		ExtractPrinters(m_OutPutCtrl.GetItemText(nIndex,0));
	
	*pResult = 0;
}

BOOL CAppPrintConfigDlg::GetIniFile()
{

	CWaitCursor wait;	// display wait cursor
	
	CmsgGETOTYPEINI msg;

	CString UnixIniFile;

	BOOL ok=msg.Load(m_HostName);
	ok=msg.DoItNow();
	if(ok) ok=msg.UnLoad(UnixIniFile.GetBuffer(_MAX_PATH));
	if(!ok)
	{
		CString temp;
		temp.Format(_T("Failed to get Ini File Location from %s!!!"),m_HostName);
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(temp,MB_ICONEXCLAMATION);
		TCHAR err_msg[MAX_ERROR_MSG_LEN]; //debug
		::ZeroMemory(err_msg,sizeof(err_msg));
		if(_tcslen(err_msg)>0)
		{
			NtcssGetLastError(err_msg, MAX_ERROR_MSG_LEN); //debug
			AfxMessageBox(err_msg,MB_ICONEXCLAMATION);
		}
		return FALSE;
	}

    	
	UnixIniFile.ReleaseBuffer(); 

	m_ServerFileName=UnixIniFile.Left(UnixIniFile.ReverseFind('/'))+"/prtupd.ini";

	//TODO if no data change UnixIni to make empty file or
	//something

	if(!NtcssGetFile(m_HostName,UnixIniFile,
				    m_GetFileName,TRUE))
	{
		TCHAR err_msg[MAX_ERROR_MSG_LEN]; //debug
		NtcssGetLastError(err_msg, MAX_ERROR_MSG_LEN); //debug
		AfxMessageBox(err_msg);

	//	MessageBeep(MB_ICONEXCLAMATION );
	//	AfxMessageBox(_T("FTP failed!!!"),MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CAppPrintConfigDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->wParam==VK_F5)
	{
		OnRefresh();
		return TRUE;
	}
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CAppPrintConfigDlg::OnDesktophelp() 
{
	help_object.LaunchDesktopHelpTopics();	
}

void CAppPrintConfigDlg::OnGettinghelp() 
{
	help_object.LaunchGettingHelp();	
	
}

void CAppPrintConfigDlg::OnWindowhelp() 
{
	help_object.LaunchWindowHelp();		
}

void CAppPrintConfigDlg::OnFieldhelp() 
{
	help_object.LaunchFieldHelp();		
}

void CAppPrintConfigDlg::OnUserguide() 
{
	help_object.LaunchUsersGuide();		
}

void CAppPrintConfigDlg::OnIdd() 
{
	help_object.LaunchInterfaceDesignDocument();		
}

void CAppPrintConfigDlg::OnSysadm() 
{
	help_object.LaunchSystemAdministration();		
}

void CAppPrintConfigDlg::OnTechsupport() 
{
	help_object.LaunchTechnicalSupport();		
}

void CAppPrintConfigDlg::OnVersion() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

CString CAppPrintConfigDlg::GetAuthServer(CString strApp)
{
	TCHAR host[NTCSS_SIZE_HOSTNAME];

	NtcssGetAuthenticationServer((LPCTSTR)strApp,host);
	ASSERT(strlen(host)>0);
	return host;
}

void CAppPrintConfigDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if((APCListCtrl*)pWnd!=&m_OutPutCtrl)
		return;
	CMenu* pSubMenu;
	pSubMenu=m_Popup.GetSubMenu(0);

    //do pop up
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd(),
		NULL);
}


void CAppPrintConfigDlg::OnRefresh() 
{
	m_AppListCtrl.ResetContent();
	m_PrinterCtrl.DeleteAllItems();
	m_OutPutCtrl.DeleteAllItems();
	FillAppListCtrl();

}



void CAppPrintConfigDlg::FillAppListCtrl() 
{
	//Fill in Apps List Box
	CStringEx strAppList;
	CString strApp;

	NtcssGetAppNameList(strAppList.GetBuffer(8000),8000); //from LRS 
	strAppList.ReleaseBuffer();

	for(int i=0;;i++)
	{
		if((strApp=strAppList.GetField(',',i))==_T(""))
			break;
		if((NtcssGetAppRole(strApp)&1) && (strApp.CompareNoCase(_T("NTCSS"))))
			m_AppListCtrl.AddString(strApp);
	}

	if (!m_AppListCtrl.GetCount()) 
	{
		
		MessageBeep(MB_ICONSTOP);
		AfxMessageBox(_T("Current user is not an Administrator"),MB_ICONSTOP);
		::ExitProcess(0);
	}
}




