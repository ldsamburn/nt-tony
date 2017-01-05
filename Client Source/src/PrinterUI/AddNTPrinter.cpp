/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AddNTPrinter.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterUI.h"
#include "AddNTPrinter.h"
#include "sys/stat.h"
#include "direct.h"
#include "ntcssapi.h"
#include "Inri_Ftp.h"
#include "ntcssapi.h"
#include "msgs.h"
#include "InstallFromDisk.h"
#define  theman
#include "ini.h"
#include "ini2.h"
#include "PUIHelp.h"
#include "EditPrinter.h"
#include "lzexpand.h"
#include "WinSpool.h"
#include "setupapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// context for SetupIterateCabinet
typedef struct tagCTX {
    LPCTSTR pszDestPath;    // destination path
} CTX;

//TODO this might be better to make a Temp dir to store these files in instead of
//C:\\WINNT\\

/////////////////////////////////////////////////////////////////////////////
// CAddNTPrinter dialog


CAddNTPrinter::CAddNTPrinter(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNTPrinter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNTPrinter)
	m_Manufactures = _T("");
	m_Printers = _T("");
	m_NTPrinterInf = _T("");
	//}}AFX_DATA_INIT
	m_driver=_T("");
	m_path=_T("");
	m_monitorstring=_T("");
	m_bWIN2K=FALSE;
	driverfile=configfile=datafile=helpfile=
	monitorfile=monitorstring=strCopyFiles=_T("");
	list=new CMapStringToString;

}


void CAddNTPrinter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNTPrinter)
	DDX_Control(pDX, IDC_LIST1, m_ManufacturesCtrl);
	DDX_Control(pDX, IDC_LIST2, m_PrintersCtrl);
	DDX_LBString(pDX, IDC_LIST1, m_Manufactures);
	DDX_LBString(pDX, IDC_LIST2, m_Printers);
	DDX_Text(pDX, IDC_CURRENTINF, m_NTPrinterInf);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNTPrinter, CDialog)
	//{{AFX_MSG_MAP(CAddNTPrinter)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_BN_CLICKED(IDC_NEWINSTALL, OnNewINF)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNTPrinter message handlers

BOOL CAddNTPrinter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	try
	{
		
		help_object.SetWindowHelp(HT_SelectNTPrinter_WindowDescription);
		
		//GET OS TYPE
		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		
		// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
		// which is supported on Windows 2000.
		//
		// If that fails, try using the OSVERSIONINFO structure.
		
		::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		
		if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)))
		{
			// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
			
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				throw(_T("Couldn't obtain OS Version"));
		}
		
		if(osvi.dwPlatformId==VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 4)
			m_bWIN2K = osvi.dwMajorVersion == 5;
		else
			throw(_T("Unsupported OS Version"));
		
		
		//GET OS TYPE
		struct _stat buf;
		char sysroot[256];
		DWORD needed;


    	::GetEnvironmentVariable(_T("SystemRoot"), sysroot, sizeof(sysroot));
		m_sysroot=sysroot;  //Save system root in member variable.. this is C:\WINNT
		
		m_sysroot+=_T("\\");
		
		m_NTPrinterInf=m_sysroot+_T("INF\\NTPRINT.INF"); 

		
		//Save System Spool Directory in member variable
		
		::GetPrinterDriverDirectory(NULL,NULL,1,NULL,0,&needed);
		
		unsigned char *buffer=new unsigned char[needed];
		
		if(::GetPrinterDriverDirectory(NULL,NULL,1,buffer,needed,&needed))
			m_strSysSpoolDir=buffer;
		else
		{
			if(buffer)
				delete [] buffer;
			CDialog::OnCancel();
			throw(_T("Couldn't obtain system spool directory"));
		}

		if(buffer)
			delete [] buffer;
		
		m_strSysSpoolDir+=_T("\\");
		
		//make sure there is a Printer INF file
		if(!( _stat(m_NTPrinterInf, &buf )))
			GetSection(_T("Manufacturer"),&m_ManufacturesCtrl);
		else
		{
			AfxMessageBox(_T("NT default Print INF is missing!"));
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}

		//Get Temp Path
	    ::GetTempPath(_MAX_PATH,m_strTempPath.GetBufferSetLength(_MAX_PATH));
		m_strTempPath.ReleaseBuffer();
		//m_strTempPath+=_T("\\");

		TRACE1("TEMP DIR -> %s\n",m_strTempPath);

		UpdateData(FALSE);
		
	}
	
	catch(LPTSTR pStr)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(pStr,MB_ICONEXCLAMATION);
		CDialog::OnCancel();
	}
	
	catch(...)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Caught unknown exception"),MB_ICONEXCLAMATION);
		CDialog::OnCancel();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//=============================================================================
void CAddNTPrinter::OnSelchangeList1() 
//=============================================================================

//when they select a new manufacturer
{
	int nIndex;
	if((nIndex=m_ManufacturesCtrl.GetCurSel())!=LB_ERR)
	{
		m_ManufacturesCtrl.GetText(nIndex,m_Manufactures);
		m_PrintersCtrl.ResetContent();
	    GetSection(m_Manufactures,&m_PrintersCtrl,TRUE);
	}
}


//=============================================================================
	void CAddNTPrinter::OnSelchangeList2()
//=============================================================================
{
	GetDlgItem(IDOK)->EnableWindow(m_PrintersCtrl.GetCurSel()!=LB_ERR);
	
}

//=============================================================================
	void CAddNTPrinter::OnOK() 
//=============================================================================
{
	//Get the filenames from the NT INF FILE

	
	UpdateData(TRUE);
	
	try
	{
		
		if(!GetPrinterFiles())
			throw(_T("Couldn't get driver files from INF"));

		m_driver=driverfile;
		
		if(!AreCopyFilesOnServer()) //nothing to do if server has them ALL
			if(!AreFilesLocal()) //verifies & FTPs all files
				if(!GetFilesFromDisk())
					throw(_T("Couldn't get driver files from MFG disk"));

		if(!SendServerMsg())
				throw(_T("NEWNTPRINTER message failed"));

		//Success so add these files to the current INI FILE so we don't get them again before an
		//apply

		POSITION pos;
	    CString strKey,strVal;
		for( pos = list->GetStartPosition(); pos != NULL; )
		{
			list->GetNextAssoc( pos, strKey, strVal );
			::WritePrivateProfileString(_T("FILELIST"), strKey, strKey,
			ini.m_GetFileName);
		}

	}
	
	catch (LPTSTR pStr)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(pStr,MB_ICONEXCLAMATION);
		list->RemoveAll();
		return;
	}
	
	CDialog::OnOK();
}



//=============================================================================
BOOL CAddNTPrinter::SendServerMsg()
//=============================================================================
{
	CWaitCursor wait;	// display wait cursor

	CmsgNEWNTPRINTER msg;

	//make all files upper case because UNIX is stupid

	
	driverfile.MakeUpper();
	configfile.MakeUpper();
	datafile.MakeUpper();
	helpfile.MakeUpper();
	monitorfile.MakeUpper();
	monitorstring.MakeUpper();
	strCopyFiles.MakeUpper();

	BOOL ok=msg.Load(m_Printers,driverfile,configfile,datafile,helpfile,monitorfile,
					 monitorstring,strCopyFiles);

	if (ok)
		ok=msg.DoItNow();

	return ok;
}


//=============================================================================
	BOOL CAddNTPrinter::GetPrinterFiles()
//=============================================================================
//digs out the 5 standard inf files plus the copyFiles & PUTS THEM in member Vars
				
{

	HINF hInf=OpenInf(m_NTPrinterInf);

   
	CStringEx strValue,strSection;

    if(hInf==NULL)
		return FALSE;

 	try
	{

		if((strSection=GetValue(hInf,m_Manufactures,m_Printers))==_T(""))
			throw(_T("Invalid INF format -> installer section"));


		//WIN2k INF has extra stuff on this line so lose it
		strSection=strSection.GetField(',',0);

		//try to get everything we can from INSTALLER section
		//USING MICROSOFTS RULES 

		//RULE 1 
		//IF there is no DRIVER= section the driver isn't in THE DATASECTION
		//the section NAME IT CAN BE IN 1 OF THE OTHER SECTIONS

		//Try to assign this stuff DataSection First

		CString strDS=GetValue(hInf,strSection,_T("DataSection"));
		if(!strDS.IsEmpty())
		{
            driverfile=GetValue(hInf,strDS,_T("DriverFile"));
			datafile=GetValue(hInf,strDS,_T("DataFile"));
		    configfile=GetValue(hInf,strDS,_T("ConfigFile"));
			helpfile=GetValue(hInf,strDS,_T("HelpFile"));

		}

		if(driverfile.IsEmpty() && (driverfile=GetValue(hInf,strSection,_T("DriverFile")))==_T(""))
			driverfile=strSection;

		//got to @ least have this guy
		if(driverfile.IsEmpty())
			throw(_T("Invalid INF format -> installer section"));

		TRACE1("Driver File is <%s>\n",driverfile);

		//RULE 2 
		//IF there is no DATAFILE= section the driver is the section
		//CHECK IN THE OPTIONAL "Datasection" section DataFile key
		//ELSE it's the same as the driver section
		if(datafile.IsEmpty() && (datafile=GetValue(hInf,strSection,_T("DataFile")))==_T(""))
			datafile=strSection;


		//RULE 3
		//ConfigFile same as #2
		if(configfile.IsEmpty() && (configfile=GetValue(hInf,strSection,_T("ConfigFile")))==_T(""))
			configfile=strSection;

		//RULE 4 
		//helpfile same as #2 except if not found in datasection.. it's NULL
		if(helpfile.IsEmpty())
			helpfile=GetValue(hInf,strSection,_T("HelpFile"));

		//RULE 5
		//LanguageMonitor same as #4
		//LanguageMonitor is stored-> NAME,DLL so if it was there split it
		strValue=GetValue(hInf,strSection,_T("LanguageMonitor"));

		if(!strValue.IsEmpty())
		{
			monitorstring=strValue.GetField(',',0);
			monitorfile=strValue.GetField(',',1);
		}

		//now check the guys that MIGHT still be empty but MIGHT be in the datasection
		if(!strDS.IsEmpty() && monitorfile.IsEmpty() &&
		  (strValue=GetValue(hInf,strDS,_T("LanguageMonitor")))!=_T(""))
		{
			monitorstring=strValue.GetField(',',0);
			monitorfile=strValue.GetField(',',1);
		}

		TRACE1("Data File is <%s>\n",datafile);
		TRACE1("Config File is <%s>\n",configfile);
		TRACE1("Help File is <%s>\n",helpfile);
		TRACE1("Monitor File is <%s>\n",monitorfile);
		TRACE0("Getting Copy Files\n");

		//PUT COPYFILES in a MAP which will be used in iterate cabinent in W2K
		//FORCE THEM UPPERCASE BECAUSE WE CANT COUNT ON CASE IN CABINET FILES
		driverfile.MakeUpper();
		datafile.MakeUpper();
		helpfile.MakeUpper();
		monitorfile.MakeUpper();

		list->SetAt(driverfile,_T("f"));
		list->SetAt(datafile,_T("f"));
		list->SetAt(configfile,_T("f"));
		if(!helpfile.IsEmpty())
			list->SetAt(helpfile,_T("f"));
		if(!monitorfile.IsEmpty())
			list->SetAt(monitorfile,_T("f"));

		CStringEx strCopyFilesEx;
		CString strToken;

		if((strCopyFilesEx=GetValue(hInf,strSection,_T("CopyFiles")))==_T(""))
			throw(_T("Invalid inf format-> No CopyFiles Section"));

		for(int i=0;;i++)
		{
			if((strToken=strCopyFilesEx.GetField(',',i))==_T(""))
				break;
			if(strToken.GetAt(0)=='@')
			{
				CString temp(strToken.Mid(1));
				temp.MakeUpper();
			    list->SetAt(temp,_T("f"));
			}
			
			//another section in [section]
			//                    file1
			//                    file2
			//form, also some file names look like PJLMON.DLL,,,0x00000020
			else  
				AddCopyFilesSection(hInf,strToken);
		}

		POSITION pos;
		CString strVal;
		for( pos = list->GetStartPosition(); pos != NULL; )
		{
				list->GetNextAssoc( pos, strToken, strVal );
				//TRACE1(_T("Value is <%s>\n"),strVal);

				 if(strCopyFiles.IsEmpty())
					strCopyFiles=strToken;
		         else
					strCopyFiles+=_T(",") + strToken; 
		}

		//////////////////////////////////////////////////// debug 
		CString strKey;
		for( pos = list->GetStartPosition(); pos != NULL; )
		{
			list->GetNextAssoc( pos, strKey, strVal );
			TRACE2(_T("Debug List Key is <%s> Value is <%s>\n"),strKey, strVal);
		}

		//////////////////////////////////////////////////// debug
        TRACE1("Leaving GPF list size is  <%d>\n",list->GetCount());
		TRACE1("Copy Files is <%s>\n",strCopyFiles);
		::SetupCloseInfFile(hInf);
		return (!datafile.IsEmpty() && !configfile.IsEmpty() && !driverfile.IsEmpty());
	}
			
	catch (LPSTR pStr)
	{
		::SetupCloseInfFile(hInf);
		::MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(pStr,MB_ICONEXCLAMATION);
		return FALSE;
	}
			
}

//=============================================================================	
	void CAddNTPrinter::OnNewINF()
//=============================================================================
{
	//going in here will use default constructor so it will
	//return the path of the ini file which we use to populate
	//this dialog
	
	m_path=_T("");
	
	CInstallFromDisk InstallDialog;
	if(InstallDialog.DoModal()==IDOK)
	{
		m_NTPrinterInf=InstallDialog.m_strINF;  //change inf file
		m_path=InstallDialog.m_Path;
		m_PrintersCtrl.ResetContent();
		GetSection(_T("Manufacturer"),&m_ManufacturesCtrl);
		GetDlgItem(IDC_CURRENTINF)->SetWindowText(m_NTPrinterInf);
	}
	
}

BOOL CAddNTPrinter::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CAddNTPrinter::OnDestroy() 
{
	CDialog::OnDestroy();
	CEditPrinter *pEditPrinter=(CEditPrinter*) GetParent();
	ASSERT(pEditPrinter!=NULL);
	if (pEditPrinter->IsEdit())
		help_object.SetWindowHelp(HT_EditNTCSSPrinter_WindowDescription);
	else
		help_object.SetWindowHelp(HT_AddNTCSSPrinter_WindowDescription);

	list->RemoveAll();
	delete list;
	
	// TODO: Add your message handler code here
	
}

//=============================================================================
	BOOL CAddNTPrinter::AreCopyFilesOnServer() 
//=============================================================================

//returns true if ALL the CopyFiles are on the server
{

	CString strKey,strVal;
	CIni2 ini2(ini.m_GetFileName);
	POSITION pos;

	//TODO: test this first
	for( pos = list->GetStartPosition(); pos != NULL; )
	{
		list->GetNextAssoc( pos, strKey, strVal );
		if((ini2.GetString(_T("FILELIST"),strKey))!=_T(""))
			list->RemoveKey(strKey);
	}

	return list->IsEmpty();
}
//=============================================================================
	BOOL CAddNTPrinter::AreFilesWithINF()
//=============================================================================
 
//returns true if ALL the CopyFiles WE NEED are with the MFG INF file
//
{
	struct _stat buf;
	CString strFile,strVal;
	POSITION pos;

	for( pos = list->GetStartPosition(); pos != NULL; )
	{
		list->GetNextAssoc( pos, strFile, strVal );
		if(_stat((m_path+strFile), &buf )!=0)
		   return FALSE;
	}

	TRACE("AreFilesWithINF is TRUE \n");
	return TRUE;
}


//=============================================================================
	BOOL CAddNTPrinter::AreFilesLocal() 
//=============================================================================

//returns true if ALL the CopyFiles WE NEED are on the client
//monitor DLL is a problem it only HAS TO BE in SYSTEM32
//makes a new copystring with full paths so copyfile function can
//copy them to the server
{

	CString strVal,strFile;
	CStringEx strCopyFiles2;
	struct _stat buf;
	POSITION pos;

	UINT size=::GetSystemDirectory(NULL,0);
	CString temp;
	::GetSystemDirectory(temp.GetBuffer(size),size);
	temp.ReleaseBuffer();
	temp+=_T("\\");

	if(!monitorfile.IsEmpty() && list->Lookup(monitorfile,strVal))
	{
		if(_stat(temp+monitorfile, &buf )!=0)
			return FALSE;
		strCopyFiles2=temp+monitorfile;
	}

	for( pos = list->GetStartPosition(); pos != NULL; )
	{
		list->GetNextAssoc( pos, strFile, strVal );
		if(!strFile.CompareNoCase(monitorfile))
		   continue;
	   if(_stat(m_strSysSpoolDir+strFile, &buf )!=0)
		   return FALSE;
	   if(strCopyFiles2.IsEmpty())
		   strCopyFiles2=m_strSysSpoolDir+strFile;
	   else
		   strCopyFiles2+=_T(",") + m_strSysSpoolDir+strFile;
		
	}


    TRACE("AreFilesLocal is TRUE... copying the files to the server\n");
	return CopyFiles(strCopyFiles2);
}

//=============================================================================
	BOOL CAddNTPrinter::CopyFiles(CStringEx strCopyFiles)
//=============================================================================

//parses strCopyFiles & copies each file in it to the server
{
	CWaitCursor wait;	// display wait cursor
	CString strFile;
	CString strServerFile;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	char server_name[NTCSS_SIZE_HOSTNAME+1];
	if(!NtcssGetServerName(server_name, sizeof(server_name))) return FALSE;

	for(int i=0;;i++)
	{
	   if((strFile=strCopyFiles.GetField(',',i))==_T(""))
		   break;

	   strFile.MakeUpper();

	   _splitpath(strFile, drive, dir, fname, ext);

	   strServerFile.Format("/h/NTCSSS/spool/ntdrivers/%s%s",fname,ext);

	    if(!NtcssPutFile(server_name,strServerFile,strFile,FALSE)) return FALSE;
	}

	return TRUE;
}

//=============================================================================
	BOOL CAddNTPrinter::GetFilesFromDisk()
//=============================================================================

// Parses the CopyFiles String & copies them to the system root disk to be FTP'd 
// & deleted later... 3 possible sources
// 1) get from MFG disk if inf doesn't=NTPRINT
// 2) If NT4 & using NTPRINT.INF then call InstallDialog to get MFG disk location
// 3) If W2K & using NTPRINT.INF silently iterate the cached driver cab

//TODO... this can use SetupDecompressOrCopyFile && clean things up alot

{
	
	CStringEx strCopyFiles2;
	CString strFile,strVal;
	BOOL bDoDlg=TRUE;
	BOOL bCopyFiles=TRUE;
	POSITION pos;

	//1st find out if they have changed inf from default
	//if they did & ALL the files are in same place we done here
	
	if(m_NTPrinterInf.CompareNoCase(m_sysroot+_T("INF\\NTPRINT.INF")))
		bDoDlg=!AreFilesWithINF();
	
	else
		if(m_bWIN2K)
			bCopyFiles=bDoDlg=!IterateCabinet();
		
		if(bDoDlg)
		{
			
			CInstallFromDisk InstallDialog(NULL,m_driver,
				m_NTPrinterInf.CompareNoCase(m_sysroot+_T("ntprint.inf")) ? FALSE : TRUE,
				list);
			
			if(InstallDialog.DoModal()==IDCANCEL)
				return FALSE;
			else
				m_path=InstallDialog.m_Path;
		}
		
		//Now loop through MAP & make a copystring of guys we NEED to copy

		for( pos = list->GetStartPosition(); pos != NULL; )
		{
		   list->GetNextAssoc( pos, strFile, strVal );
		   if(strCopyFiles2.IsEmpty())
			   strCopyFiles2=m_strTempPath+strFile;
		   else
			   strCopyFiles2+=_T(",") + m_strTempPath+strFile;
		   if(bCopyFiles)
			   if(::SetupDecompressOrCopyFile(m_path+strFile,
				    m_strTempPath+strFile,NULL)!=ERROR_SUCCESS)
			   {
				   TRACE1(_T("SetupDecompressOrCopyFile Failed for <%s>\n"),m_path+strFile);
					return FALSE;
			   }
			   else
				   TRACE1(_T("SetupDecompressOrCopyFile Worked for <%s>\n"),m_path+strFile);
		}




		BOOL ret=CopyFiles(strCopyFiles2); //COPY THEM TO SERVER FROM SYSTEM TEMP
		
		for(int i=0;;i++)                  //DELETE TEMP COPY FILES
		{
			if((strFile=strCopyFiles2.GetField(',',i))==_T(""))
				break;
			_unlink(strFile);
		}
		
		return(ret);
}

//=============================================================================
	void CAddNTPrinter::GetSection(LPCTSTR section, CListBox* pLB, BOOL bGetKey/*=FALSE*/)
//=============================================================================
{

	INFCONTEXT context;
	DWORD needed;
	LPTSTR buf=NULL;
	CStringEx strManufacturer;


    HINF hInf = OpenInf(_T("NTPRINT.INF"));

	BOOL bResult = ::SetupFindFirstLine (hInf,section,NULL,&context);

	do
	{
		if(!bGetKey) //Get Data
		{
			::SetupGetLineText(&context,NULL,NULL,NULL,NULL,0,&needed);
			buf=new char[needed];
			::SetupGetLineText(&context,NULL,NULL,NULL,buf,needed,&needed);
		}
		else  //Get Key
		{
			::SetupGetStringField(&context,0,NULL,0,&needed);
			buf=new char[needed];
			::SetupGetStringField(&context,0,buf,needed,&needed);
		}

		strManufacturer = buf;
	    strManufacturer.FindReplace("\"",""); 
	    pLB->AddString(strManufacturer);
		
		delete [] buf;
		
			
	} while(::SetupFindNextLine(&context,&context));


	if(hInf)
		::SetupCloseInfFile(hInf);
}


//=============================================================================
	HINF CAddNTPrinter::OpenInf(const CString& strInf)
//=============================================================================

{
	HINF hInf = ::SetupOpenInfFile (strInf,NULL,INF_STYLE_WIN4,NULL);
	ASSERT (hInf!=INVALID_HANDLE_VALUE);

    if (hInf == INVALID_HANDLE_VALUE)  
    { 
        DWORD dwResult = GetLastError();
		CString strError;
		strError.Format(_T("OpenInf Failed Error code -> %d\n"),dwResult);
		TRACE("Couldn't open Inf file <%s> -> %d\n",strInf,dwResult);
		::MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(strError,MB_ICONEXCLAMATION);
		return NULL;
    } 
	else
		return hInf;
}

//=============================================================================
	CString CAddNTPrinter::GetValue(HINF hInf, const CString& section, const CString& key)
//=============================================================================

{
	INFCONTEXT context;
	DWORD needed;
	LPTSTR buf=NULL;
	CString val;

	if(!::SetupFindFirstLine (hInf,section,key,&context))
		return _T("");

	::SetupGetLineText(&context,NULL,NULL,NULL,NULL,0,&needed);

	buf=new char[needed];

    ::SetupGetLineText(&context,NULL,NULL,NULL,buf,needed,&needed);

	val=buf;
	if(buf)
		delete[] buf;
	return val;

}

//=============================================================================
	BOOL CAddNTPrinter::AddCopyFilesSection(HINF hInf,const CString& section)
//=============================================================================
//used for sections inside COPYFILES section with the unique format...
//put the files right in the CString List
{
	INFCONTEXT context;
	DWORD needed;
	LPTSTR buf=NULL;
	CStringEx strLine;
	CString temp;
	DWORD ct=0;
	
	if((ct=SetupGetLineCount(hInf,section))>0)
	{
		for(DWORD i=0;i<ct;i++)
		{
			if(!::SetupGetLineByIndex(hInf,section,i,&context))
				return false;
			::SetupGetLineText(&context,NULL,NULL,NULL,NULL,0,&needed);
			buf=new char[needed];
			::SetupGetLineText(&context,NULL,NULL,NULL,buf,needed,&needed);
			strLine=buf;
			if(buf)
				delete[] buf;

			temp=strLine.GetField(',',0);
			temp.MakeUpper();
			list->SetAt(temp,_T("f"));

		}
		
		return TRUE;
	}
	
	else
		return FALSE;
}

//====================================================================
	UINT CALLBACK CAddNTPrinter::CabinetCallback(LPVOID pCtx,UINT uNotify,
										       UINT uParam1,UINT uParam2)
//====================================================================
{

	CAddNTPrinter *pParent=((CAddNTPrinter*)AfxGetMainWnd()->GetActiveWindow());
	ASSERT(pParent);
	

    switch (uNotify)
    {
		// SetupIterateCabinet is going to unpack a file and we have to tell
		// it where it should place the file
		case SPFILENOTIFY_FILEINCABINET:
		{
			CString strVal,strKey;
			FILE_IN_CABINET_INFO * pInfo = (FILE_IN_CABINET_INFO *)uParam1;
			ASSERT(pInfo != NULL);
			strKey=pInfo->NameInCabinet;
			strKey.MakeUpper();
			//TRACE1(_T("Name in cabinet <%s> \n"),strKey);
			//TRACE1(_T("Size of list is <%d> \n"),pParent->list->GetCount());
	
			if(pParent->list->Lookup(strKey,strVal))
			{
				pParent->list->SetAt(strKey,_T("t"));
				TRACE2(_T("Doing -> %s%s\n"),pParent->m_strTempPath,pInfo->NameInCabinet);
				_stprintf(pInfo->FullTargetName,_T("%s%s"),pParent->m_strTempPath,pInfo->NameInCabinet);
				return FILEOP_DOIT;
			}
			else
			{
			//	TRACE1(_T("Lookup failed for <%s> \n"),strKey);
			    return FILEOP_SKIP;
			}

		}

		// a file was extracted
		case SPFILENOTIFY_FILEEXTRACTED:
		{
			FILEPATHS * pPaths = (FILEPATHS *)uParam1;
			ASSERT(pPaths != NULL);
			return pPaths->Win32Error;
		}

		// a new cabinet file is required; this notification should never
		// appear in our program, since all files have to be in a single
		// cabinet
		case SPFILENOTIFY_NEEDNEWCABINET:
		{
			ASSERT(0);
			return ERROR_INVALID_PARAMETER;
		}
    }

    return ERROR_SUCCESS;
}

//====================================================================
	BOOL CAddNTPrinter::IterateCabinet()
//====================================================================
// called on WIN2K ONLY... tries to copy all the copyfiles (which are in member map)
// to the system temp
{
	CTX ctx;
	POSITION pos;
	CString strKey,strVal;
	BOOL ok=TRUE;
	
	TRACE1(_T("Iterating Cabinet File <%s>\n"), m_sysroot + _T("Driver Cache\\i386\\driver.cab"));
	if(!::SetupIterateCabinet(m_sysroot + _T("Driver Cache\\i386\\driver.cab"), 
		0, CabinetCallback, &ctx))
		return FALSE;
	
	TRACE0(_T("Iterating Cabinet File Successful\n"));
	
	//Now LOOP through map to see if we got ALL the files
	
	for( pos = list->GetStartPosition(); pos != NULL; )
	{
		list->GetNextAssoc( pos, strKey, strVal );
		if(strVal!=_T("t"))
		{
		    TRACE2(_T("Checking <%s> Value is <%s>\n"),strKey, strVal);
			ok=FALSE;
			break;
		}
	}
	
	if(ok) 
		return TRUE;
	else //lets wack any files it did get
		
		for( pos = list->GetStartPosition(); pos != NULL; )
		{
			list->GetNextAssoc( pos, strKey, strVal );
			if(strVal==_T("t"))
				_unlink(m_strTempPath + strKey);
		}
		
		return FALSE;
}
