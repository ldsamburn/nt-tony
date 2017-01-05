/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pdj_confDlg.cpp : implementation file
//

#include "stdafx.h"
#include <NtcssApi.h>
#include <PrivApis.h>
#include <MsgCore.h>
#include <MsgDefs.h>
#include <inri_ftp.h>
#include "pdj_conf.h"
#include "pdj_confDlg.h"
#include "addedit.h"
#include "syslog.h"
#include "AppLckDlg.h"
#include "PDJHelp.h"
#include "pdjmsgs.h"

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
// CPdj_confDlg dialog

CPdj_confDlg::CPdj_confDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPdj_confDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPdj_confDlg)
	m_Application = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPdj_confDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPdj_confDlg)
	DDX_Control(pDX, IDC_PDJ_LIST, m_ListCtrl);
	DDX_LBString(pDX, IDC_APPLICATION_LIST, m_Application);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPdj_confDlg, CDialog)
	//{{AFX_MSG_MAP(CPdj_confDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_SYSTEM_APPLY, OnSystemApply)
	ON_NOTIFY(NM_DBLCLK, IDC_PDJ_LIST, OnDblclkPdjList)
	ON_LBN_SELCHANGE(IDC_APPLICATION_LIST, OnSelchangeApplicationList)
	ON_COMMAND(ID_PREDFINEDJOBS_ADD, OnPredfinedjobsAdd)
	ON_COMMAND(ID_PREDFINEDJOBS_DELETE, OnPredfinedjobsDelete)
	ON_COMMAND(ID_PREDFINEDJOBS_EDIT, OnPredfinedjobsEdit)
	ON_COMMAND(ID_SYSTEM_CANCEL, OnSystemCancel)
	ON_COMMAND(ID_SYSTEM_QUIT, OnSystemQuit)
	ON_NOTIFY(NM_CLICK, IDC_PDJ_LIST, OnClickPdjList)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_PDJ_ABOUT, OnPdjAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPdj_confDlg message handlers

BOOL CPdj_confDlg::OnInitDialog()
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
	if (!NtcssDLLInitialize((char far *)NTCSS_DLL_VERSION, (char far *)"NTCSS", 
				AfxGetInstanceHandle(),this->m_hWnd)){
		CString ErrorMsg = "Unable to load NTCSS.DLL Version "; 
		ErrorMsg += NTCSS_DLL_VERSION;
		AfxMessageBox("Initialization Error",MB_ICONSTOP | MB_OK);
		return FALSE;
	}

	CListBox *app_list = (CListBox *)GetDlgItem(IDC_APPLICATION_LIST);
	app_list->ResetContent();
	
	LV_COLUMN listColumn;
	LV_ITEM listItem;

	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM;
	listItem.iSubItem=0;
	 
	CString str_Columns="Title,Command,Type,Access Roles";

 	static long PDJHelpTopics[]={HT_Title_PredefinedJob,HT_Command,
		HT_Type_PredefinedJob,HT_AccessRoles};
		
	m_ListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PDJ\\PDJS",
					 PDJHelpTopics); 


	NtcssGetUserInfo((NtcssUserInfo far *)&m_ui);  

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	cb->EnableWindow(FALSE);

	DisableMenuItems(ID_SYSTEM_APPLY,ID_SYSTEM_VIEWCHANGELOG,
		ID_PREDFINEDJOBS_ADD,ID_PREDFINEDJOBS_EDIT,ID_PREDFINEDJOBS_DELETE,-1);
	
	m_ChangeCount = 0;

	//SetTimer(PDJ_TIMER_ID,PDJ_TIMER_VAL,NULL);

	help_object.Initialize();
	help_object.SetWindowHelp(HT_BatchJobQueue_WindowDescriptions);


	GetWindowsDirectory(m_WindowsDir.GetBuffer(MAX_PATH),MAX_PATH);

	m_WindowsDir.ReleaseBuffer();

	m_WindowsDir = m_WindowsDir + "\\ntcss.ini";

	GetPrivateProfileString("NTCSS","NTCSS_SPOOL_DIR","c:\\ntcss2\\spool",
		m_SpoolDir.GetBuffer(MAX_PATH),MAX_PATH,m_WindowsDir);

	m_SpoolDir.ReleaseBuffer();

	m_lLogFile = m_SpoolDir + "\\pdj_log.txt";


	m_NumApps = 0;


	PopulateApps(app_list);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPdj_confDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPdj_confDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	} 
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPdj_confDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPdj_confDlg::OnSystemApply() 
{
	OnOK();
}

void CPdj_confDlg::OnDblclkPdjList(NMHDR* pNMHDR, LRESULT* pResult) 
{

	OnPredfinedjobsEdit	();
	*pResult = 0;
}

BOOL CPdj_confDlg::PopulateApps(CListBox *app_list)
{
char pdj_section[MAX_INI_SECTION];
char *tptr;
char progroup[NTCSS_SIZE_APP_GROUP*2+1];
//char lock_id[LOCK_ID_LEN];
int locked_apps_found;
CAppLckDlg cad;
char *tokptr;
char hostbuf[NTCSS_SIZE_COMMAND_LINE];

	cad.ClearAppList();
	locked_apps_found = 0;

	m_LastApplication.Empty();
	
	BOOL ok=NtcssGetAppNameList(pdj_section,MAX_INI_SECTION);
	if (!ok)
		return(FALSE);

	tptr=pdj_section;
	tokptr=strtok(tptr,",");

	while (tokptr) {
		strcpy(progroup,tokptr);
		if (NtcssGetAppRole(progroup) & 0x01 && strcmp(progroup,"NTCSS")) {
			memset(hostbuf,NULL,NTCSS_SIZE_COMMAND_LINE);
			NtcssGetAuthenticationServer(progroup,hostbuf);

//			if (NtcssAdminLockOptions(progroup,PDJ_LOCK,LOCK_NEW,
//					lock_id,hostbuf)) {
				app_list->AddString(progroup);
//				strcpy(m_LockIds[m_NumApps].AppName,progroup);
//				strcpy(m_LockIds[m_NumApps++].LockId,lock_id);
//			} else {
//				locked_apps_found = 1;
//				cad.AddAppName((CString )progroup);
//			}
		}
		tokptr=strtok(NULL,",");
	}


	CListView * cbv = (CListView*)GetDlgItem(IDC_PDJ_LIST);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();

	m_NewIniEntry=NTCSS_BASE_NUMBER;

	//if (locked_apps_found) {
	//	cad.DoModal();
	//} else {
	//	if (m_NumApps == 0) {
	//		AfxMessageBox("Current User is Not An Administrator",MB_ICONSTOP);
	//		EndDialog(0);
	//	}
//	}
	return (TRUE);
}

void CPdj_confDlg::OnSelchangeApplicationList() 
{
CButton *cb=(CButton *)GetDlgItem(IDOK);
CButton *cb1=(CButton *)GetDlgItem(IDCANCEL);

	DisableMenuItems(ID_PREDFINEDJOBS_EDIT,
		ID_PREDFINEDJOBS_DELETE,-1);
	EnableMenuItems(ID_PREDFINEDJOBS_ADD,-1);

	UpdateData(TRUE);

	if (!m_LastApplication.IsEmpty() && m_ChangeCount > 0) {

		cb->EnableWindow(FALSE);
		cb1->EnableWindow(FALSE);

		CString t;
		t.Format("Save Changes for Application %s",(LPCTSTR)m_LastApplication);
		int nRet=AfxMessageBox(t,MB_YESNO);
		if (nRet == IDYES)
			PutINIFile((char *)(LPCTSTR)m_LastApplication);

		m_ChangeCount = 0;
		m_NewIniEntry=NTCSS_BASE_NUMBER;


	}

	GetINIFile((char *)(LPCTSTR)m_Application);

	m_LastApplication=m_Application;

	UpdatePDJList();

	cb1->EnableWindow(TRUE);

	return;
	
}

void CPdj_confDlg::UpdatePDJList()
{
char group_section[MAX_INI_SECTION];
char *tptr,*sptr;
char pdj_title[NTCSS_SIZE_JOB_DESCRIP*2+2];
char pdj_command[NTCSS_SIZE_COMMAND_LINE+1];
char pdj_jobclass[MAX_ROLE_LEN+1];
char pdj_roles[8*MAX_ROLE_LEN];
char iniEntry[MAX_INI_ENTRY+2];
int i;


	LV_ITEM lvi;
	CListView * cbv = (CListView*)GetDlgItem(IDC_PDJ_LIST);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();

	GetPrivateProfileSection(m_Application.GetBuffer(m_Application.GetLength()),
					group_section,MAX_INI_SECTION,
					m_iniFile.GetBuffer(m_iniFile.GetLength()));

	if (!strlen(group_section))
		return ;

	tptr=group_section;

	i=0;
	while (strlen(tptr)) {
		memset(iniEntry,NULL,MAX_INI_ENTRY+2);
		strcpy(iniEntry,tptr);
		sptr=strchr(iniEntry,'=');
		if (!sptr || !strlen(sptr+1)) { 
			tptr += strlen(tptr) + 1;
			continue;
		}

		sptr += 1;

		GetPrivateProfileString(sptr,
			"TITLE","Unknown",pdj_title,NTCSS_SIZE_COMMAND_LINE,
			m_iniFile.GetBuffer(m_iniFile.GetLength()));

		lvi.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.pszText=pdj_title;
		lvi.iImage=i;
		lvi.stateMask=LVIS_STATEIMAGEMASK;
		lvi.state=INDEXTOSTATEIMAGEMASK(1);
		
		lview.InsertItem(&lvi);

		
		GetPrivateProfileString(sptr,
			"COMMAND","Unknown",pdj_command,NTCSS_SIZE_COMMAND_LINE,
			m_iniFile.GetBuffer(m_iniFile.GetLength()));

		GetPrivateProfileString(sptr,
			"JOBCLASS","Unknown",pdj_jobclass,MAX_ROLE_LEN,
			m_iniFile.GetBuffer(m_iniFile.GetLength()));

		GetPrivateProfileString(sptr,
			"ROLES","Unknown",pdj_roles,8*MAX_ROLE_LEN,
			m_iniFile.GetBuffer(m_iniFile.GetLength()));


		lview.SetItemText(i,1,(char far *)pdj_command);
		lview.SetItemText(i,2,(char far *)pdj_jobclass);
		lview.SetItemText(i,3,(char far *)pdj_roles);
	
		tptr += strlen(tptr) + 1;
		i++;

	}

	m_ListCtrl.AutoSizeColumns();
	return;
}

void CPdj_confDlg::OnPredfinedjobsAdd() 
{
CAddEdit ae_dlg;
char pdj_roles[MAX_ROLE_LEN*8+1];
char title_buf[MAX_INI_ENTRY];
char command[MAX_INI_ENTRY];
char roles[MAX_INI_ENTRY];
char schedule[MAX_INI_ENTRY];
char jobtype[MAX_INI_ENTRY];
int role_mask;

	UpdateData(TRUE);

	ae_dlg.ResetRoles();
	ae_dlg.ResetTypes();
	
	memset(pdj_roles,NULL,MAX_ROLE_LEN*8+1);

	if (m_Application.IsEmpty()) {
		AfxMessageBox("Application Must Be Selected!");
		return;
	}	

	GetPrivateProfileString("ROLES",
		m_Application.GetBuffer(m_Application.GetLength()),
		"",pdj_roles,MAX_ROLE_LEN*8+1,
		m_iniFile.GetBuffer(m_iniFile.GetLength()));

	ae_dlg.AddRoleEntry(pdj_roles);

	GetPrivateProfileString("JOBCLASS",
		"CLASSES","",pdj_roles,MAX_ROLE_LEN*8+1,
		m_iniFile.GetBuffer(m_iniFile.GetLength()));

	ae_dlg.AddTypeEntry(pdj_roles);

	ae_dlg.DoModal();

	if (!ae_dlg.IsValid())
		return;

	memset(title_buf,NULL,MAX_INI_ENTRY);

	ae_dlg.GetJobTitle(title_buf);

	memset(command,NULL,MAX_INI_ENTRY);
	ae_dlg.GetCommandLine(command);

	memset(jobtype,NULL,MAX_INI_ENTRY);
	ae_dlg.GetJobType(jobtype);

	memset(schedule,NULL,MAX_INI_ENTRY);
	ae_dlg.GetSchedule(schedule);

	memset(roles,NULL,MAX_INI_ENTRY);
	ae_dlg.GetRoleList(roles);

	role_mask=ae_dlg.GetRoles();

	WriteProfileOutData(m_Application.GetBuffer(m_Application.GetLength()),
					"ADD",title_buf,command,roles,role_mask,
					jobtype,schedule,
					m_iniFile.GetBuffer(m_iniFile.GetLength()),
					m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));

	//OnSelchangeApplicationList();
	UpdatePDJList();

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	cb->EnableWindow(TRUE);

	EnableMenuItems(ID_SYSTEM_APPLY,-1);


}

void CPdj_confDlg::OnPredfinedjobsDelete() 
{
CListView * cbv = (CListView*)GetDlgItem(IDC_PDJ_LIST);
CListCtrl& lview = cbv->GetListCtrl();
LVITEM lvi;
char pdj_roles[MAX_ROLE_LEN*8+1];
char title_buf[MAX_INI_ENTRY];
char command[MAX_INI_ENTRY];
char roles[MAX_INI_ENTRY];
char schedule[MAX_INI_ENTRY];
char jobtype[MAX_INI_ENTRY];
char tbuf[MAX_INI_ENTRY];
int role_mask;

	UpdateData(TRUE);

	role_mask = 0;
	
	memset(pdj_roles,NULL,MAX_ROLE_LEN*8+1);

	if (m_Application.IsEmpty()) {
		AfxMessageBox("Application Must Be Selected!");
		return;
	}	

	int tmppid=lview.GetNextItem(-1,LVNI_SELECTED);

	if (tmppid == -1) {
		AfxMessageBox("Job Must Be Selected!");
		return;
	}

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=0;
	lvi.pszText=title_buf;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=1;
	lvi.pszText=command;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=2;
	lvi.pszText=jobtype;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=3;
	lvi.pszText=roles;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	memset(schedule,NULL,MAX_INI_ENTRY);

	FindSectionByTitle(tbuf,title_buf,
					m_Application.GetBuffer(m_Application.GetLength()));


	WriteProfileOutData(m_Application.GetBuffer(m_Application.GetLength()),
					"DELETE",title_buf,command,roles,role_mask,
					jobtype,schedule,
					m_iniFile.GetBuffer(m_iniFile.GetLength()),
					m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));

//	OnSelchangeApplicationList();
	UpdatePDJList();

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	cb->EnableWindow(TRUE);
	EnableMenuItems(ID_SYSTEM_APPLY,-1);

	
}

void CPdj_confDlg::OnPredfinedjobsEdit() 
{
CListView * cbv = (CListView*)GetDlgItem(IDC_PDJ_LIST);
CListCtrl& lview = cbv->GetListCtrl();
LVITEM lvi;
CAddEdit ae_dlg;
char pdj_roles[MAX_ROLE_LEN*8+1];
char title_buf[MAX_INI_ENTRY];
char selected_title_buf[MAX_INI_ENTRY];
char command[MAX_INI_ENTRY];
char roles[MAX_INI_ENTRY];
char schedule[MAX_INI_ENTRY];
char jobtype[MAX_INI_ENTRY];
int role_mask;

	UpdateData(TRUE);

	ae_dlg.ResetRoles();
	ae_dlg.ResetTypes();
	
	memset(pdj_roles,NULL,MAX_ROLE_LEN*8+1);

	if (m_Application.IsEmpty()) {
		AfxMessageBox("Application Must Be Selected!");
		return;
	}	

	int tmppid=lview.GetNextItem(-1,LVNI_SELECTED);

	if (tmppid == -1) {
		AfxMessageBox("Job Must Be Selected!");
		return;
	}

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=0;
	lvi.pszText=selected_title_buf;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=1;
	lvi.pszText=command;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=2;
	lvi.pszText=jobtype;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	lvi.mask=LVIF_TEXT;
	lvi.iItem=tmppid;
	lvi.iSubItem=3;
	lvi.pszText=roles;
	lvi.cchTextMax=MAX_INI_ENTRY;
	lview.GetItem(&lvi);

	GetPrivateProfileString("ROLES",
		m_Application.GetBuffer(m_Application.GetLength()),
		"",pdj_roles,MAX_ROLE_LEN*8+1,
		m_iniFile.GetBuffer(m_iniFile.GetLength()));

	ae_dlg.AddRoleEntry(pdj_roles);

	GetPrivateProfileString("JOBCLASS",
		"CLASSES","",pdj_roles,MAX_ROLE_LEN*8+1,
		m_iniFile.GetBuffer(m_iniFile.GetLength()));

	FindSectionByTitle(title_buf,selected_title_buf,
					m_Application.GetBuffer(m_Application.GetLength()));

	GetPrivateProfileString(title_buf,
		"SCHEDULE","",schedule,MAX_INI_ENTRY,
		m_iniFile.GetBuffer(m_iniFile.GetLength()));

	ae_dlg.AddTypeEntry(pdj_roles);

	ae_dlg.SetTitle(selected_title_buf);
	ae_dlg.SetCommandLine(command);

	ae_dlg.SetSchedule(schedule);
	ae_dlg.SetSelectedType(jobtype);
	ae_dlg.SetSelectedRoles(roles);

	ae_dlg.DoModal();

	if (!ae_dlg.IsValid())
		return;

	memset(title_buf,NULL,MAX_INI_ENTRY);

	ae_dlg.GetJobTitle(title_buf);

	memset(command,NULL,MAX_INI_ENTRY);
	ae_dlg.GetCommandLine(command);

	memset(jobtype,NULL,MAX_INI_ENTRY);
	ae_dlg.GetJobType(jobtype);

	memset(schedule,NULL,MAX_INI_ENTRY);
	ae_dlg.GetSchedule(schedule);

	memset(roles,NULL,MAX_INI_ENTRY);
	ae_dlg.GetRoleList(roles);

	role_mask=ae_dlg.GetRoles();

	if (strcmp(selected_title_buf,title_buf)) {

		WriteProfileOutData(m_Application.GetBuffer(m_Application.GetLength()),
					"DELETE",selected_title_buf,command,roles,role_mask,
					jobtype,schedule,
					m_iniFile.GetBuffer(m_iniFile.GetLength()),
					m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));

		WriteProfileOutData(m_Application.GetBuffer(m_Application.GetLength()),
					"ADD",title_buf,command,roles, role_mask,
					jobtype,schedule,
					m_iniFile.GetBuffer(m_iniFile.GetLength()),
					m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));

	} else {

		WriteProfileOutData(m_Application.GetBuffer(m_Application.GetLength()),
				"EDIT",title_buf,command,roles,role_mask,
				jobtype,schedule,
				m_iniFile.GetBuffer(m_iniFile.GetLength()),
				m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));
	}
	//OnSelchangeApplicationList();
	UpdatePDJList();

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	cb->EnableWindow(TRUE);
	EnableMenuItems(ID_SYSTEM_APPLY,-1);
	
}

void CPdj_confDlg::OnSystemCancel() 
{
	OnCancel();
}

void CPdj_confDlg::OnSystemQuit() 
{
	OnCancel();

}

void CPdj_confDlg::WriteProfileOutData(char *group,
									   char *command,
									   char *title_buf,
									   char *command_line,
									   char *role_list,
									   int  role_mask,
									   char *job_type,
									   char *schedule,
									   char *inifile,
									   char *iniOutfile)
{
char tbuf[MAX_INI_ENTRY+1];
char token_buf[MAX_INI_ENTRY+1];
char pdj_title[NTCSS_SIZE_JOB_DESCRIP*2+2];


	memset(tbuf,NULL,MAX_INI_ENTRY+1);
	memset(token_buf,NULL,MAX_INI_ENTRY+1);

	m_ChangeCount++;

	sprintf(tbuf,"PDJ_%02d",m_ChangeCount);

	WritePrivateProfileString("PDJ",tbuf,tbuf,iniOutfile);

	WritePrivateProfileString(tbuf,"OPERATION",command,
		iniOutfile);

	WritePrivateProfileString(tbuf,"TITLE",title_buf,
		iniOutfile);

	WritePrivateProfileString(tbuf,"GROUP",group,
		iniOutfile);

	WritePrivateProfileString(tbuf,"COMMAND",command_line,
		iniOutfile);

	WritePrivateProfileString(tbuf,"JOBCLASS",job_type,
		iniOutfile);

	WritePrivateProfileString(tbuf,"SCHEDULE",schedule,
		iniOutfile);

	WritePrivateProfileString(tbuf,"ROLELIST",role_list,
		iniOutfile);

	char role_mask_str[MAX_INI_ENTRY+1];


	sprintf(role_mask_str,"%04d",role_mask);

	WritePrivateProfileString(tbuf,"ROLES",role_mask_str,
		iniOutfile);

	if (!strcmp(command,"DELETE")) {

		FindSectionByTitle(pdj_title,title_buf,group);

		if (strlen(pdj_title)) {
				WritePrivateProfileString(group,pdj_title,"",inifile);
				WritePrivateProfileSection(pdj_title,"",inifile);
		}
	} else {

		FindSectionByTitle(token_buf,title_buf,group);

		if (!strlen(token_buf)) {
			wsprintf(token_buf,"%s_%03d",group,m_NewIniEntry);

			WritePrivateProfileString(group,
				token_buf,token_buf,inifile);
		}

		WritePrivateProfileString(token_buf,"TITLE",title_buf,
			inifile);

		WritePrivateProfileString(token_buf,"COMMAND",command_line,
			inifile);

		WritePrivateProfileString(token_buf,"COMMAND",command_line,
			inifile);
		WritePrivateProfileString(token_buf,"JOBCLASS",job_type,
			inifile);
		WritePrivateProfileString(token_buf,"SCHEDULE",schedule,
			inifile);
		WritePrivateProfileString(token_buf,"ROLES",role_list,
			inifile);

		m_NewIniEntry++;
	}

}



void CPdj_confDlg::OnCancel() 
{

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	if (cb->IsWindowEnabled()) {
		int ans=AfxMessageBox("Save Changes?",MB_YESNOCANCEL);
		if (ans == IDYES) 
			OnOK();
		else if (ans == IDCANCEL)
			return;
	}

	ClearAllLocks();
	EndDialog(0);
//	CDialog::OnCancel();
}

void CPdj_confDlg::OnOK() 
{

	CButton *cb=(CButton *)GetDlgItem(IDOK);
	CButton *cb1=(CButton *)GetDlgItem(IDCANCEL);

	cb->EnableWindow(FALSE);
	cb1->EnableWindow(FALSE);

	if (PutINIFile((char *)(LPCTSTR)m_Application)) {
		AfxMessageBox("Error Sending PDJ Message to Server");
		cb1->EnableWindow(TRUE);
		return;
	}

	// Retrieve the file again
//	CListBox *app_list = (CListBox *)GetDlgItem(IDC_APPLICATION_LIST);
//	app_list->ResetContent();

	cb1->EnableWindow(TRUE);
	m_ChangeCount = 0;
	m_NewIniEntry=NTCSS_BASE_NUMBER;

}

int CPdj_confDlg::GetINIFile(char *appname)
{
char tbuf[SIZE_PRT_FILE_NAME+1];
char hostbuf[NTCSS_SIZE_COMMAND_LINE];

	CmsgGETPDJINI cGetINI;

	memset(tbuf,NULL,NTCSS_SIZE_COMMAND_LINE);

	NtcssGetAuthenticationServer(appname,hostbuf);

	cGetINI.Load(hostbuf);

	BOOL nRet=cGetINI.DoItNow();

	if (nRet != TRUE) {
		return(1);
	}

	cGetINI.UnLoad(tbuf);
	m_Source=(char *)tbuf;

	m_iniFile = m_SpoolDir + "\\pdj_in.ini";
	m_iniOutFile = m_SpoolDir + "\\pdj_out.ini";
	m_Target = "/tmp/pdj_ext.ini";

	_unlink(m_iniOutFile.GetBuffer(m_iniOutFile.GetLength()));
	_unlink(m_iniFile.GetBuffer(m_iniFile.GetLength()));

	if (NtcssGetFile(hostbuf,(char *)(LPCTSTR)m_Source,
				(char *)(LPCTSTR)m_iniFile,NTCSS_ASCII_FTP)==FALSE) {
		AfxMessageBox("Error Getting PDJ File");
		return FALSE;
	}

	return(1);
}


int CPdj_confDlg::PutINIFile(char *appname)
{
char tbuf[NTCSS_SIZE_COMMAND_LINE];
char hostbuf[NTCSS_SIZE_COMMAND_LINE];

	CmsgPUTPDJINI cPutINI;


	memset(tbuf,NULL,NTCSS_SIZE_COMMAND_LINE);

	NtcssGetAuthenticationServer(appname,hostbuf);

	cPutINI.Load((char *)(LPCSTR)m_Target,hostbuf);

	if (NtcssPutFile(hostbuf,(char *)(LPCTSTR)m_Target,
				(char *)(LPCSTR)m_iniOutFile,NTCSS_ASCII_FTP)==FALSE) {
		AfxMessageBox("Error Sending PDJ File");
		return(1);
	}

	BOOL nRet=cPutINI.DoItNow();

	if (nRet != TRUE) {
		return(1);
	}

	cPutINI.UnLoad(tbuf);
	m_rLogFile=(char *)tbuf;

	return(0);
}

void CPdj_confDlg::OnSystemViewchangelog() 
{
	CSysLog cs;

	cs.SetFile(m_lLogFile);
	cs.DoModal();

}


void CPdj_confDlg::DisableMenuItems(int first,...)
{
	va_list ap;
	int menu_id;
	CMenu *cm=GetMenu();
//	HMENU hm=GetMenu();
//	cm.Attach(hm);
	
	menu_id = first;
	va_start(ap, first);

	while (menu_id != -1) {
		cm->EnableMenuItem(menu_id, MF_BYCOMMAND | MF_GRAYED);
		menu_id = va_arg(ap, int);
	}

	va_end(ap);
}


// make sure the last one is -1
void CPdj_confDlg::EnableMenuItems(int first,...)
{
	va_list ap;
	int menu_id;
	CMenu *cm=GetMenu();
	
	menu_id = first;
	va_start(ap, first);

	while (menu_id != -1) {
		cm->EnableMenuItem(menu_id, MF_BYCOMMAND | MF_ENABLED);
		menu_id = va_arg(ap, int);
	}

	va_end(ap);
}

void CPdj_confDlg::OnClickPdjList(NMHDR* pNMHDR, LRESULT* pResult) 
{

	// TODO: Add your control notification handler code here
	EnableMenuItems(ID_PREDFINEDJOBS_ADD,ID_PREDFINEDJOBS_EDIT,
					ID_PREDFINEDJOBS_DELETE,-1);

	NM_LISTVIEW *pNMListView=(NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
}


void CPdj_confDlg::FindSectionByTitle(char *retbuf,char *title_buf,
									  char *group) 
{
char *tptr,*sptr;
char group_section[MAX_INI_SECTION];
char pdj_title[MAX_INI_ENTRY+1];
char iniEntry[MAX_INI_ENTRY+2];

		GetPrivateProfileSection(group,group_section,MAX_INI_SECTION,
					m_iniFile.GetBuffer(m_iniFile.GetLength()));

		if (!strlen(group_section))
			return ;

		tptr=group_section;

		while (strlen(tptr)) {
			memset(iniEntry,NULL,MAX_INI_ENTRY+2);
			strcpy(iniEntry,tptr);
			sptr=strchr(iniEntry,'=');
			if (!sptr || !strlen(sptr+1)) { 
				tptr += strlen(tptr) + 1;
				continue;
			}

			sptr += 1;

			GetPrivateProfileString(sptr,
				"TITLE","Unknown",pdj_title,NTCSS_SIZE_COMMAND_LINE,
				m_iniFile.GetBuffer(m_iniFile.GetLength()));

			if (!strcmp(pdj_title,title_buf)) {
				strcpy(retbuf,sptr);
				break;
			}
			tptr += strlen(tptr) + 1;

		}
}

void CPdj_confDlg::OnTimer(UINT nIDEvent) 
{
char hostbuf[NTCSS_SIZE_COMMAND_LINE];


	for (int i=0;i<m_NumApps;i++) {
		memset(hostbuf,NULL,NTCSS_SIZE_COMMAND_LINE);

		NtcssGetAuthenticationServer(m_LockIds[i].AppName,hostbuf);

		NtcssAdminLockOptions(m_LockIds[i].AppName,PDJ_LOCK,
			LOCK_RENEW,m_LockIds[i].LockId,hostbuf);
	}
	CDialog::OnTimer(nIDEvent);
}


void CPdj_confDlg::ClearAllLocks(void) 
{
char hostbuf[NTCSS_SIZE_COMMAND_LINE];


	for (int i=0;i<m_NumApps;i++) {
		memset(hostbuf,NULL,NTCSS_SIZE_COMMAND_LINE);

		NtcssGetAuthenticationServer(m_LockIds[i].AppName,hostbuf);

		NtcssAdminLockOptions(m_LockIds[i].AppName,PDJ_LOCK,
			LOCK_REMOVE,m_LockIds[i].LockId,hostbuf);
	}
}

void CPdj_confDlg::OnClose() 
{
	OnCancel();
	
	CDialog::OnClose();
}
/*
void CPdj_confDlg::OnPdjHelp() 
{
	help_object.LaunchDesktopHelpTopics();
	
}
*/
void CPdj_confDlg::OnPdjAbout() 
{
	CAboutDlg ad;

	ad.DoModal();

	
}




BOOL CPdj_confDlg::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
/*
void CPdj_confDlg::OnGettingHelp() 
{
	help_object.LaunchGettingHelp();	
}

void CPdj_confDlg::OnSystemAdminGuide() 
{

	help_object.LaunchSystemAdministration();	
}

void CPdj_confDlg::OnTechSupport() 
{
	help_object.LaunchTechnicalSupport();	
}

void CPdj_confDlg::OnUsersGuide() 
{
	help_object.LaunchUsersGuide();	
}

void CPdj_confDlg::OnWindowHelp() 
{
	help_object.LaunchWindowHelp();	
}

void CPdj_confDlg::OnFieldHelp() 
{
	help_object.LaunchFieldHelp();	
}

void CPdj_confDlg::OnIdd() 
{
	help_object.LaunchInterfaceDesignDocument();	
}
*/

