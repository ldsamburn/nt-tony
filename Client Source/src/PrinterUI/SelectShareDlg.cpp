/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// SelectShareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterUI.h"
#include "SelectShareDlg.h"
#include "Sizes.h"
#include "winspool.h"
#include "StringEx.h"

//#include "EditPrinter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectShareDlg dialog


CSelectShareDlg::CSelectShareDlg(CWnd* pParent)
	: CDialog(CSelectShareDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectShareDlg)
	m_strPrinter = _T("");
	//}}AFX_DATA_INIT
	m_strHost=_T("");

}


void CSelectShareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectShareDlg)
	DDX_Control(pDX, IDC_TREECTRL, m_TreeCtrl);
	DDX_Text(pDX, IDC_PRINTERLB, m_strPrinter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectShareDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectShareDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREECTRL, OnSelchangedTreectrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREECTRL, OnItemexpandingTreectrl)

/////////////////////////////////////////////////////////////////////////////
// CSelectShareDlg message handlers

BOOL CSelectShareDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_REFRESH));
    ASSERT(m_hAccel);

	m_ImageList.Create( IDB_SHARETREECTRL, 16, 3, RGB(255,255,255) );
	
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );

	OnRefresh();

 	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



/*void CSelectShareDlg::OnOK() 


{
	CStringEx strUNC;
//	m_ShareLBCtlr.GetText(m_ShareLBCtlr.GetCurSel(),strUNC);

	strUNC=m_strPrinter;

	if(strUNC.IsEmpty())
		CDialog::OnCancel();
	try
	{

		m_strHost=strUNC.GetField('\\',2);
		if(m_strHost.GetLength()>SIZE_HOST_NAME)
			throw("Host Name exceeds NTCSS maximum");
		m_strPrinter=strUNC.GetField('\\',3);
		if(m_strPrinter.GetLength()>SIZE_PRINTER_NAME)
			throw("Printer Name exceeds NTCSS maximum");

	}

	catch (LPTSTR msg)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(msg,MB_ICONEXCLAMATION);
		CDialog::OnCancel();
	}

	CDialog::OnOK();
}*/


BOOL CSelectShareDlg::PreTranslateMessage(MSG* pMsg) 
{
  if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
   {
      return TRUE;
   }
   	
	return CDialog::PreTranslateMessage(pMsg);
}


 
void CSelectShareDlg::OnRefresh() 
{
	CWaitCursor csr;

	TCHAR strDomain[MAX_PATH]; 
	HTREEITEM hWorkStation;

	GetEnvironmentVariable( _T("USERDOMAIN"),strDomain,MAX_PATH); 

	#ifdef _DEBUG
	_tcscpy(strDomain,_T("DLVAB-JONESJE"));  //debug 
	#endif

	DWORD pcbNeeded,pcReturned;

	PRINTER_INFO_1 *pPI1;
	TCHAR strName[_MAX_PATH*2];

	sprintf(strName,_T("Windows NT Remote Printers!%s"),strDomain); 

	::EnumPrinters(PRINTER_ENUM_NAME ,strName,1,0,0,&pcbNeeded,&pcReturned);

	if (pcbNeeded == 0) 
	{
		CString temp;
		temp.Format(_T("There are no shares for the %s domain"),strDomain);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(temp,MB_ICONEXCLAMATION);
		PostMessage(WM_CLOSE);
	}

	LPTSTR buffer=new char[pcbNeeded];
	
	
	pPI1=(PRINTER_INFO_1*)buffer;

	::EnumPrinters(PRINTER_ENUM_NAME,strName,1,(LPBYTE)pPI1,pcbNeeded,
										&pcbNeeded,&pcReturned);

	m_TreeCtrl.DeleteAllItems();


	HTREEITEM hDomain=m_TreeCtrl.InsertItem(strDomain,0,0); 
	m_TreeCtrl.SetItemData(hDomain,1); //domain is 1

	for(DWORD i=0;i<pcReturned;i++)
	{
		hWorkStation=m_TreeCtrl.InsertItem(strstr(pPI1[i].pName,_T("\\"))+2,2,2,hDomain);
		m_TreeCtrl.SetItemData(hWorkStation,2); //workstation is 2

	}

	            
	m_TreeCtrl.Expand(hDomain,TVE_EXPAND);

	delete [] buffer;	
}

void CSelectShareDlg::OnSelchangedTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hNext=NULL;

		switch (pNMTreeView->itemNew.lParam)
		{
		case 2: //workstation got to try to expand it
				if(!m_TreeCtrl.GetChildItem( pNMTreeView->itemNew.hItem))
					EnumPrintServer(pNMTreeView->itemNew.hItem,m_TreeCtrl.
					                GetItemText(pNMTreeView->itemNew.hItem));
				m_TreeCtrl.Expand(pNMTreeView->itemNew.hItem,TVE_EXPAND);
				break;

		case 3: //printer
				m_strPrinter=m_TreeCtrl.GetItemText(pNMTreeView->itemNew.hItem);
				m_strHost=m_TreeCtrl.GetItemText(m_TreeCtrl.GetParentItem(pNMTreeView->itemNew.hItem));
	
				UpdateData(FALSE);
				break;

		}
	
	*pResult = 0;
}

void CSelectShareDlg::EnumPrintServer(HTREEITEM hParent,CString strWS) 
{
	TCHAR strDomain[_MAX_PATH]; //TODO find out what the MAX domain size is


	GetEnvironmentVariable( _T("USERDOMAIN"),strDomain,MAX_PATH); 
	#ifdef _DEBUG
	_tcscpy(strDomain,_T("DLVAB-JONESJE"));  //debug 
	#endif

	
	DWORD pcbNeeded,pcReturned;

	PRINTER_INFO_1 *pPI1;
	TCHAR strName[_MAX_PATH*2];

	sprintf(strName,_T("Windows NT Remote Printers!%s!\\\\%s"),strDomain,strWS); 

//	AfxMessageBox(strName);

	CWaitCursor cursor;

	::EnumPrinters(PRINTER_ENUM_NAME ,strName,1,0,0,&pcbNeeded,&pcReturned);

	if (pcbNeeded == 0) 
		  return;

	LPTSTR buffer=new char[pcbNeeded];
	
	
	pPI1=(PRINTER_INFO_1*)buffer;

	::EnumPrinters(PRINTER_ENUM_NAME,strName,1,(LPBYTE)pPI1,pcbNeeded,
										&pcbNeeded,&pcReturned);
	CStringEx temp;
	
	for(DWORD i=0;i<pcReturned;i++)
	{
		temp=pPI1[i].pName;
		HTREEITEM dog=m_TreeCtrl.InsertItem(temp.GetField('\\',3),2,2,hParent);

		m_TreeCtrl.SetItemData(dog,3);
		
	}

	delete [] buffer;
}
	


