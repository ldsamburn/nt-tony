/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// LogFileDialog.cpp : implementation file
//

#include "stdafx.h"
#define  theman
#include "ini.h"
#include "PrinterUI.h"
#include "LogFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogFileDialog dialog


CLogFileDialog::CLogFileDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogFileDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogFileDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLogFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogFileDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogFileDialog, CDialog)
	//{{AFX_MSG_MAP(CLogFileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogFileDialog message handlers

BOOL CLogFileDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	TRY
	{
	
		CFile f(ini.m_LogFileName, CFile::modeRead | CFile::shareDenyNone );

		TRY
		{
			CString buf;
			int nRead=f.Read(buf.GetBuffer(f.GetLength()),f.GetLength());
			m_Edit_Ctrl.SetWindowText(buf);
		}
		CATCH( CFileException, e )
		{
			AfxMessageBox("File could not be read " + e->m_cause);
		}
		END_CATCH
	}
	CATCH( CFileException, e )
	{
		AfxMessageBox("File could not be opened " + e->m_cause);
	}
	END_CATCH
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
