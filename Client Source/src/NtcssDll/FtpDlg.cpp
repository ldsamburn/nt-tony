/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// FtpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ntcssdll.h"
#include "FtpDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpDlg dialog


CFtpDlg::CFtpDlg(CWnd* pParent, /*=NULL*/CString InFile/*=_T("")*/,CString OutFile
				 /*=_T("")*/,BOOL bZip/*=TRUE*/): CDialog(CFtpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFtpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
m_strInFile=InFile;
m_strOutFile=OutFile;
m_bZip=bZip;

}


void CFtpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFtpDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFtpDlg, CDialog)
	//{{AFX_MSG_MAP(CFtpDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER+300, DOZIP)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFtpDlg message handlers

void CFtpDlg::OnCancel() 
{
	m_zf.AbortZip();	
//	CDialog::OnCancel();
}

BOOL CFtpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
  	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CFtpDlg::DOZIP(UINT wParam,LONG lParam)
{
	 m_zf.SetProgressWindow(&m_ProgressCtrl);

	if(m_bZip)
	{
		if(!m_zf.ZipFile(m_strInFile.GetBuffer(m_strInFile.GetLength()),
               m_strOutFile.GetBuffer(m_strOutFile.GetLength())))
		EndDialog(IDOK);
		else
			EndDialog(IDCANCEL);
		return 0;
	}
	else
	{
		if(!m_zf.UnZipFile(m_strInFile.GetBuffer(m_strInFile.GetLength()),
               m_strOutFile.GetBuffer(m_strOutFile.GetLength())))
		EndDialog(IDOK);
		else
			EndDialog(IDCANCEL);
		return 0;
	}
}


void CFtpDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	PostMessage(WM_USER+300);	
}
