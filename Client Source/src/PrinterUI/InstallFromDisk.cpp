/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// InstallFromDisk.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterUI.h"
#include "InstallFromDisk.h"
#include "sys/stat.h"
#include "WCHAR.H"
#include "io.h"
#include "shlobj.h"
#include "PUIHelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int WINAPI BrowseForFolderCallBack(
	HWND	hwnd,
	UINT	uMsg,
	LPARAM	lParam,
	LPARAM	lpData)
{
	// Notified of events by in call to SHBrowseForFolder

	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd,BFFM_ENABLEOK,0,0); 
	}
	else if (uMsg == BFFM_SELCHANGED)
	{
		char bufPath[MAX_PATH];

		SHGetPathFromIDList((LPCITEMIDLIST)lParam, bufPath);
		strcat(bufPath,(CString)(char*)lpData);
		CString bufPath2=bufPath;
		bufPath2.SetAt(bufPath2.GetLength()-1,'_');
		
		if (_access(bufPath,0) == 0 || _access(bufPath2,0) == 0)
			SendMessage(hwnd,BFFM_ENABLEOK,1,1);
		else 
			SendMessage(hwnd,BFFM_ENABLEOK,0,0);
    }
	return(0);
}


/////////////////////////////////////////////////////////////////////////////
// CInstallFromDisk dialog


CInstallFromDisk::CInstallFromDisk(CWnd* pParent /*=NULL*/,
								   CString DriverFile /*NULL*/,BOOL SysInf /*FALSE*/,
								   CMapStringToString* list /*NULL*/)
	: CDialog(CInstallFromDisk::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallFromDisk)
	m_Drive = _T("");
	//}}AFX_DATA_INIT
	m_DriverFile=DriverFile;
	m_SysInf=SysInf;
	m_strINF=m_Path=_T("");
	m_list=list;
	
}


void CInstallFromDisk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallFromDisk)
	DDX_Control(pDX, IDC_DRIVES, m_DriveCtrl);
	DDX_CBString(pDX, IDC_DRIVES, m_Drive);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInstallFromDisk, CDialog)
	//{{AFX_MSG_MAP(CInstallFromDisk)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstallFromDisk message handlers

BOOL CInstallFromDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	char DriveList[128];
	char *p=DriveList;
	::GetLogicalDriveStrings(sizeof(DriveList),DriveList);
	do
	{
		if (::GetDriveType(p)==DRIVE_REMOVABLE)
			m_DriveCtrl.AddString(p);
		else if(::GetDriveType(p)==DRIVE_CDROM)
			m_DriveCtrl.AddString((CString)p+_T("i386\\"));
		if (m_SysInf&&::GetDriveType(p)==DRIVE_CDROM)
			m_DriveCtrl.SelectString( -1,p);  //Select CD Rom if were using sys inf
		else if(!m_SysInf&&::GetDriveType(p)==DRIVE_REMOVABLE)
			m_DriveCtrl.SelectString( -1,p);  //Select Floppy if were not using sys inf*/
		p=strchr(p,'\0')+1;
	}while (*p!=NULL);

	if(m_DriverFile!=_T(""))  //need a driver file
	{
		SetWindowText(_T("Select Driver File Folder"));
		help_object.SetWindowHelp(HT_SelectDriverFileFolder_WindowDescription);
	}
	else
	{
		SetWindowText(_T("Select Installation File"));
		help_object.SetWindowHelp(HT_SelectInstallationFile_WindowDescription);
	}
	

//	AfxMessageBox("Leaving init dialog");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInstallFromDisk::OnOK() 
//only hit when they use listbox to select files
//TODO test this

{
	//returns 2 different things, if called with a driver name, it validates & 
	//returns full path else if drivername is blank, returns first inf file which
	//is used back in AddNTPrinter to refresh that dialog with that inf & then when
	//user hits ok there, they'll come back in here with a driver name
	struct _stat buf;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
   	UpdateData(TRUE);
	
	
	if(m_DriverFile!=_T(""))  //need driver files
	{
		POSITION pos;
		CString strVal,strFile,strPath;
		
		for( pos = m_list->GetStartPosition(); pos != NULL; )
		{
			m_list->GetNextAssoc(pos, strFile, strVal);
			strPath=m_Drive+strFile;
			TRACE1(_T("STATing <%s>\n"),strPath);
			if(_stat((strPath), &buf )!=0)
			{
				strPath.SetAt(strPath.GetLength()-1,'_');
				if( _stat(strPath, &buf )!=0)
				{   //if not found in either format fail
					CString Error;
					Error.Format(_T("Can't find %s in %s"),strFile,m_Drive);
					::MessageBeep(MB_ICONSTOP);
					AfxMessageBox(Error,MB_ICONSTOP);
					return;
				}
			}
	
		}
		
		m_Path=m_Drive+m_DriverFile;
	}
	
	else  //need an inf file
	{
		struct _finddata_t c_file;
		CString filespec=m_Drive+"*.inf";
		if( _findfirst( (char*)(LPCTSTR)filespec, &c_file ) != -1L )
			m_Path=m_Drive+c_file.name;
		else
		{
			::Beep(500,40);
			return;
		}
	}
	
	_splitpath((char*)(LPCTSTR)m_Path, drive, dir, fname, ext );
	if(m_DriverFile.IsEmpty())
		m_strINF=m_Path;
	m_Path.Format("%s%s",drive,dir);
	
	CDialog::OnOK();
}

void CInstallFromDisk::OnBrowse() 

//if they hit this they never go back to OK...
//OK is only used off the listbox

{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
    if(m_DriverFile!=_T("")) //need a driver file
	{
		
	
		if((m_Path=OnPickfolder("\\"+m_DriverFile))!=_T(""))
		{
			struct _stat buf;
			m_Path+="\\"+m_DriverFile;
			if( _stat(m_Path, &buf )!=0)  //make this a function!!!!!
			{ //if not found unexpanded see if it's expanded

				m_Path.SetAt(m_Path.GetLength()-1,'_');
				if( _stat(m_Path, &buf )!=0)
				{ //if not found in either format fail
					CString Error;
					Error.Format(_T("Can't find %s in %s"),m_DriverFile,m_Path);
					::MessageBeep(MB_ICONSTOP);
				    AfxMessageBox(Error,MB_ICONSTOP);
					return;
				}
			}
		
			//CDialog::OnOK(); 3/1/99
		}
		else
			return;

	}
	else  //we need an inf 
	{
		CFileDialog dlg(TRUE,_T("inf"),NULL,OFN_HIDEREADONLY,"INF Files (*.inf)|*.inf||");
		dlg.m_ofn.lpstrTitle = _T("Browse For Installation File");
		if(dlg.DoModal()==IDOK)
			m_Path=dlg.GetPathName();  
		//CDialog::OnOK(); 3/1/99
	
	}

	 _splitpath((char*)(LPCTSTR)m_Path, drive, dir, fname, ext );
	  if(m_DriverFile.IsEmpty())
		 m_strINF=m_Path;
	 m_Path.Format("%s%s",drive,dir);
	 	 
	 CDialog::OnOK();
	
}


CString CInstallFromDisk::OnPickfolder(CString driver) 
{
	
	// pick a folder starting with our last selection point
	// (well, start at the root for now...)

	CString path=_T("");
	
	BROWSEINFO bi;
	LPITEMIDLIST pidlBrowse;
	
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;	// use root
	bi.pszDisplayName = new char[_MAX_PATH];
	bi.lpszTitle = _T("Select Driver File Folder...");
	bi.ulFlags = 0;
	bi.lpfn = BrowseForFolderCallBack;
	bi.lParam = (LPARAM)(LPCSTR)driver;
	
	pidlBrowse = SHBrowseForFolder(&bi);
	char fullPath[_MAX_PATH];
	SHGetPathFromIDList(pidlBrowse, fullPath);

	if(pidlBrowse!=NULL) 
		path=fullPath;
	
	delete[] bi.pszDisplayName;

	return path;
	
}


BOOL CInstallFromDisk::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CInstallFromDisk::OnDestroy() 
{
	CDialog::OnDestroy();
	help_object.SetWindowHelp(HT_SelectNTPrinter_WindowDescription);
	
}
