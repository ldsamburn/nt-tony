/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppLckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pdj_conf.h"
#include "AppLckDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppLckDlg dialog


CAppLckDlg::CAppLckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppLckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppLckDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAppLckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppLckDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppLckDlg, CDialog)
	//{{AFX_MSG_MAP(CAppLckDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppLckDlg message handlers
int CAppLckDlg::AddAppName(CString appname)
{

	if (!m_LockedApps.IsEmpty()) 
		m_LockedApps = m_LockedApps + ",";
	m_LockedApps = m_LockedApps + appname;	
	return 0;
}

int CAppLckDlg::ClearAppList()
{

	m_LockedApps.Empty();
	return 0;
}

BOOL CAppLckDlg::OnInitDialog() 
{
CListBox *cb=(CListBox *)GetDlgItem(IDC_APPS_LOCKED);
	char bigbuf[1000];

	CDialog::OnInitDialog();

	if (m_LockedApps.IsEmpty())
			return FALSE;

	strcpy(bigbuf,(LPCTSTR)m_LockedApps);

	char *tptr=strtok(bigbuf,",");

	while (tptr) {
		cb->AddString(tptr);
		tptr=strtok(NULL,",");
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
