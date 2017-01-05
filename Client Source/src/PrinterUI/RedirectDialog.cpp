/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// RedirectDialog.cpp : implementation file
//

#include "stdafx.h"
#define  theman
#include "ini.h"
#include "PrinterUI.h"
#include "RedirectDialog.h"
#include "PUIHelp.h"
#include "EditPrinter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRedirectDialog dialog


CRedirectDialog::CRedirectDialog(CWnd* pParent /*=NULL*/,printer_node* t /*NULL*/)
	: CDialog(CRedirectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRedirectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_PrinterNode=t;
}


void CRedirectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRedirectDialog)
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRedirectDialog, CDialog)
	//{{AFX_MSG_MAP(CRedirectDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRedirectDialog message handlers

BOOL CRedirectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();


	help_object.SetWindowHelp(HT_SelectRedirectPrinter_WindowDescription);

	LV_COLUMN listColumn;
	LV_ITEM listItem;
	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT;
	listItem.iSubItem=0;
	
	m_ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS);



	 /////////////////////////////////////////////////////////////////////////////////////////////////	
	 CString str_Columns="NAME,HOSTNAME,CLASSIFICATION,PRINTER ACCESS,PRINTER STATUS,QUEUEING STATUS";

	 static long HelpTopics[]={ HT_Name_Printer,HT_Host_Printer,HT_Classification_Printer,
							    HT_PrinterAccess,HT_PrinterStatus,HT_Queue_ing_Status};

	m_ListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PUI\\REDIRECTPRINTER",
					 HelpTopics);
	

	//now just put in printers that match current printer by 
	//access (whole list?), class, & classification is >=
   
	POSITION pos=PrinterList.GetHeadPosition();
//	i=0;
//	listItem.iItem=i;
    printer_node *t;
	while (pos != NULL)
	{
	    t = PrinterList.GetNext(pos);

		if(t->classname==m_PrinterNode->classname&&t->security
	    >=m_PrinterNode->security&&t->name!=m_PrinterNode->name &&t->port!=_T("FILE")/*TR 10236*/)
		{
	
			m_ListCtrl.Write(t->name);
			m_ListCtrl.Write(t->hostname);
			m_ListCtrl.Write(ini.GetClassification(t->security));
			m_ListCtrl.Write(t->accessstatus==1 ? "General" : "By Application");
			m_ListCtrl.Write(t->printerstatus==1 ? "Working" : "Halted");
			m_ListCtrl.Write(t->queuestatus==1 ? "Enabled" : "Disabled");

		}
	


	}
	if (m_PrinterNode->redirected != "NONE")
		m_ListCtrl.SelectItem(m_PrinterNode->redirected);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRedirectDialog::OnOK() 
{
	m_PrinterNode->redirected=m_ListCtrl.GetText("NAME");
	if(m_PrinterNode->redirected.IsEmpty())
		m_PrinterNode->redirected = _T("None");

	CDialog::OnOK();
}

BOOL CRedirectDialog::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CRedirectDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CEditPrinter *pEditPrinter=(CEditPrinter*) GetParent();
	ASSERT(pEditPrinter!=NULL);
	if (pEditPrinter->IsEdit())
		help_object.SetWindowHelp(HT_EditNTCSSPrinter_WindowDescription);
	else
		help_object.SetWindowHelp(HT_AddNTCSSPrinter_WindowDescription);	
}


void CRedirectDialog::OnClear() 
{
	m_ListCtrl.DeSelectAll();
}
