/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ValidateUserDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ValidateUserDialog.h"
#include "sizes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CValidateUserDialog dialog


CValidateUserDialog::CValidateUserDialog(CWnd* pParent)
	: CDialog(CValidateUserDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CValidateUserDialog)
	m_UserName = _T("");
	m_Password = _T("");
	//}}AFX_DATA_INIT

}


void CValidateUserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValidateUserDialog)
	DDX_Text(pDX, IDC_UserName, m_UserName);
	DDV_MaxChars(pDX, m_UserName, 8);
	DDX_Text(pDX, IDC_Password, m_Password);
	DDV_MaxChars(pDX, m_Password, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValidateUserDialog, CDialog)
	//{{AFX_MSG_MAP(CValidateUserDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



