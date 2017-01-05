/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// SysLog.cpp : implementation file
//

#include "stdafx.h"
#include "pdj_conf.h"
#include "SysLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysLog dialog


CSysLog::CSysLog(CWnd* pParent /*=NULL*/)
	: CDialog(CSysLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSysLog)
	m_SyslogText = _T("");
	//}}AFX_DATA_INIT
}


void CSysLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysLog)
	DDX_Text(pDX, IDC_SYSTEM_LOG, m_SyslogText);
	DDV_MaxChars(pDX, m_SyslogText, 2046);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysLog, CDialog)
	//{{AFX_MSG_MAP(CSysLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysLog message handlers

void CSysLog::SetFile(CString logFile)
{
	m_LogFile = logFile;
	return;
}

BOOL CSysLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CEdit *ce=(CEdit *)GetDlgItem(IDC_SYSTEM_LOG);
	
	TRY 
	{
		CFile f(m_LogFile, CFile::modeRead | CFile::shareDenyNone );

		TRY
		{
			CString buf;
			int nRead=f.Read(buf.GetBuffer(f.GetLength()),f.GetLength());
			ce->SetWindowText(buf);
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
