/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// LockedAppWarnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppPrintConfig.h"
#include "LockedAppWarnDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLockedAppWarnDlg dialog


CLockedAppWarnDlg::CLockedAppWarnDlg(CWnd* pParent/*=NULL*/,CString strAppList/*=_T("")*/)
	: CDialog(CLockedAppWarnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLockedAppWarnDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strAppList=strAppList;
}


void CLockedAppWarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLockedAppWarnDlg)
	DDX_Control(pDX, IDC_LIST1, m_AppListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLockedAppWarnDlg, CDialog)
	//{{AFX_MSG_MAP(CLockedAppWarnDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLockedAppWarnDlg message handlers

BOOL CLockedAppWarnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char *token = strtok((char*)(LPCTSTR) m_strAppList, "," );
	while( token != NULL )
	{
		m_AppListCtrl.AddString(token);
		token = strtok( NULL, "," );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
