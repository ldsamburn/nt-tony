/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ViewServerFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dll_defs.h"
#include "dll_msgs.h"
#include "CallMan.h"
#include "ViewServerFileDlg.h"
#include "Inri_Ftp.h"
#include "ResizingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewServerFileDlg dialog


CViewServerFileDlg::CViewServerFileDlg(CWnd* pParent /*=NULL*/,
									   CString Server,CString FileName)
	: CResizingDialog(CViewServerFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewServerFileDlg)
	//}}AFX_DATA_INIT
	m_Server=Server;
	m_FileName=FileName;
	SetControlInfo(IDCANCEL,ANCHORE_BOTTOM|ANCHORE_RIGHT);
	SetControlInfo(IDC_SERVERFILEDITBOX,RESIZE_BOTH);
}


void CViewServerFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewServerFileDlg)
	DDX_Control(pDX, IDC_SERVERFILEDITBOX, m_EditCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewServerFileDlg, CResizingDialog)
	//{{AFX_MSG_MAP(CViewServerFileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewServerFileDlg message handlers

BOOL CViewServerFileDlg::OnInitDialog() 
{
	
	CResizingDialog::OnInitDialog();
	CWaitCursor wait;	// display wait cursor


	::GetPrivateProfileString(_T("NTCSS"),_T("NTCSS_ROOT_DIR"),"!",m_LocalFileName.GetBuffer(_MAX_PATH)
		,_MAX_PATH,_T("ntcss.ini"));

	m_LocalFileName.ReleaseBuffer();

	
	if (m_LocalFileName.Find("!")!=-1)
	{
		AfxMessageBox("There is no NTCSS ini file!");
		return FALSE;
	}


	m_LocalFileName+="/ntcsstmp.tmp";//TODO check for possible existance
	_unlink(m_LocalFileName);

	CInriFtpMan ftp_man;


	BOOL ok = ftp_man.Initialize();
	if(ok)
	{
		ok = ftp_man.GetFile(m_Server,
					 m_FileName,
					 m_LocalFileName,
					 TRUE,0666);        //TRUE->Ascii
	}
	else
	{
		AfxMessageBox("FTP error on get!!!");
		return FALSE;
	}

	try
	{
	
		CFile f(m_LocalFileName, CFile::modeRead | CFile::shareDenyNone );

		try
		{
			CString buf;
			int nRead=f.Read(buf.GetBuffer(f.GetLength()),f.GetLength());
			m_EditCtrl.SetWindowText(buf);
		}
		catch( CFileException e )
		{
			AfxMessageBox("File could not be read " + e.m_cause);
		}
	}
	catch( CFileException e )
	{
		AfxMessageBox("File could not be opened " + e.m_cause);
	}

/*	CString buffer;

	CmsgGETFILESECTION msg;  


	CString FL,LL;
	int lines;
	FL.Format("%d",1);
	LL.Format("%d",30);

	AfxMessageBox(m_FileName);


	BOOL ok = msg.Load((char*)(LPCTSTR)m_Server,(char*)(LPCTSTR)m_FileName,
		                FL.GetBufferSetLength(SIZE_CHAR_TO_INT)
	                    ,LL.GetBufferSetLength(SIZE_CHAR_TO_INT));

    if (ok)
	   ok = msg.DoItNow();
	

    if (ok)
	   ok = msg.UnLoad(&lines,buffer.GetBufferSetLength(_MAX_PATH));
	           
	if(lines>255)
	{
		AfxMessageBox("File excedes 255 line restriction");
		return FALSE;
	}

	if(!ok||!lines) return FALSE;


	buffer.ReleaseBuffer();
	
	m_EditCtrl.SetWindowText(FindAndReplace(buffer));*/
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CViewServerFileDlg::FindAndReplace(CString OrgString)

{
	char *token;
	CString ReturnString;

   token = strtok( (char*)(LPCTSTR)OrgString, "\n" );
   while( token != NULL )
   {
       ReturnString+=(CString)token+"\r\n";
	   token = strtok( NULL, "\n" );   
   }


	return ReturnString;
}



