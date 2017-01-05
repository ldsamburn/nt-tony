/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// dmcdlg.cpp : implementation file
//

#include "stdafx.h"
#include "pqueue.h"
#include "dmcdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDMCDlg dialog


CDMCDlg::CDMCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDMCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDMCDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDMCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDMCDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDMCDlg, CDialog)
	//{{AFX_MSG_MAP(CDMCDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDMCDlg message handlers

BOOL CDMCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();  
	char tbuf[100];


	
	m_DlgWidthUnits=LOWORD(::GetDialogBaseUnits())/4;
	m_TabStops[0]=(m_DlgWidthUnits * 7 * 2);
	m_TabStops[1]=(m_DlgWidthUnits * 14 * 2);
	m_TabStops[2]=(m_DlgWidthUnits * 21 * 2);
	m_TabStops[3]=(m_DlgWidthUnits * 28 * 2);
	m_TabStops[4]=(m_DlgWidthUnits * 35 * 2);
	m_TabStops[5]=(m_DlgWidthUnits * 46 * 2);
	m_TabStops[6]=(m_DlgWidthUnits * 52 * 2);

 		 
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_SETTABSTOPS,6,(LONG)(LPSTR)m_TabStops);
	
	                         
	                         
    SetDlgItemText(IDC_COMMS_STATUS,"On-Line");
    SetDlgItemText(IDC_SERVER_STATUS,"On-Line");
    SetDlgItemText(IDC_RAID_STATUS,"On-Line");
    SetDlgItemText(IDC_ARCHIVE_STATUS,"Off-Line"); 
 
    memset(tbuf,0,100);   
		wsprintf((char far *)tbuf,"%s\t%s\t%s\t%s",
				 "123431","APPL1","10/10/96","On-Line");
    
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_ADDSTRING,0,(LONG)"001676             MOCAS            10/12/96            Archived");
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_ADDSTRING,0,(LONG)"001274             SAMMS            10/12/96            On-Line");
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_ADDSTRING,0,(LONG)"001678             SAMMS            10/12/96            Archived");
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_ADDSTRING,0,(LONG)"001234             SAMMS            10/12/96            Archived");
	SendDlgItemMessage(IDC_ARCHIVE_LIST,LB_ADDSTRING,0,(LONG)"001888             MOCAS            10/12/96            On-Line");
    
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
