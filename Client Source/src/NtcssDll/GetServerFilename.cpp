/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// GetServerFilename.cpp : implementation file
//

#include "stdafx.h"
#include "ntcssdll.h"
#include "GetServerFilename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetServerFilename dialog
// this just inherits from CViewSFileDlg & we override a few things & move some buttons


CGetServerFilename::CGetServerFilename(CWnd* pParent /*=NULL*/,
							 Csession *session,CString ServerName,CString Directory)
	: CViewSFileDlg(pParent,session,ServerName,Directory)
{
	//{{AFX_DATA_INIT(CGetServerFilename)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGetServerFilename::DoDataExchange(CDataExchange* pDX)
{
	CViewSFileDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetServerFilename)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetServerFilename,CViewSFileDlg )
	//{{AFX_MSG_MAP(CGetServerFilename)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGetServerFilename::OnOK() 
{

	m_SelectedFileName=m_strDirectory+"/"+m_SelectedFileName;
	UpdateData(FALSE);
	CDialog::OnOK();
}




