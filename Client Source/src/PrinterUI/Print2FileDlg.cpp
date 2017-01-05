/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Print2FileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterUI.h"
#include "Print2FileDlg.h"
#include "StringEx.h"
#define  theman
#include "ini.h"
#include "msgs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrint2FileDlg dialog

void AFXAPI DDV_CheckPath(CDataExchange* pDX, int nIDC,CString& strPath)
{
/*	HWND hWndCtrl=pDX->PrepareEditCtrl(nIDC);
	if(pDX->m_bSaveAndValidate)
		if(!strPath.IsEmpty())
		{
			CStringEx temp=strPath;
			temp.FindReplace("\\","/");	
			CmsgGETDIRLISTING msg;
			if(!(msg.Load(ini.m_hostname,(LPCTSTR)
					temp.Mid(0,temp.ReverseFind('/'))) && msg.DoItNow()))
			{
				AfxMessageBox("Bad Path Name specified");
				DDX_Text(pDX, nIDC, strPath);   
				pDX->Fail(); 
			
			}
			strPath=temp;
		} */
}


CPrint2FileDlg::CPrint2FileDlg(CWnd* pParent /*=NULL*/,CString strFileName/*=_T("")*/,
							   int nAppend/*=-1*/)
	: CDialog(CPrint2FileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrint2FileDlg)
	m_PrintFileName = _T("");
	m_nAppend = nAppend;
	//}}AFX_DATA_INIT
}


void CPrint2FileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrint2FileDlg)
	DDX_Radio(pDX, IDC_OVERWRITE, m_nAppend);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_PrintFileName);
	DDV_MaxChars(pDX, m_PrintFileName, _MAX_PATH);
	DDV_CheckPath(pDX, IDC_EDIT1,m_PrintFileName);
}


BEGIN_MESSAGE_MAP(CPrint2FileDlg, CDialog)
	//{{AFX_MSG_MAP(CPrint2FileDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrint2FileDlg message handlers
