/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AtlassFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ntcssdll.h"
#include "AtlassFileDlg.h"
#include "StringEx.h"
#include "dll_msgs.h"
#include "dlluser.h"
#include "DllMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtlassFileDlg dialog


CAtlassFileDlg::CAtlassFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAtlassFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAtlassFileDlg)
	m_strAtlassFname = _T("");
	m_nAppend = 1;
	//}}AFX_DATA_INIT
}


void CAtlassFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtlassFileDlg)
	DDX_Text(pDX, IDC_ATLASSFNAME, m_strAtlassFname);
	DDX_Radio(pDX, IDC_RBAPPEND, m_nAppend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAtlassFileDlg, CDialog)
	//{{AFX_MSG_MAP(CAtlassFileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtlassFileDlg message handlers

void CAtlassFileDlg::OnOK() 
{
	UpdateData();
	if(!m_strAtlassFname.IsEmpty())
	{
	
		//change MS slashes to UNIX
		
		CStringEx temp=m_strAtlassFname;
		temp.FindReplace("\\","/");	
		m_strAtlassFname=temp;

	
		//make sure we can access directory right away

		CmsgGETDIRLISTING msg;

	
		if(!(msg.Load(gbl_dll_man.dllServerName(),(LPCTSTR)
			temp.Mid(0,temp.ReverseFind('/'))) && msg.DoItNow()))
		{
			MessageBeep(MB_ICONEXCLAMATION );
			AfxMessageBox("Bad printer name specified",MB_ICONEXCLAMATION);
			return;
		}
	}
	
	CDialog::OnOK();
}
