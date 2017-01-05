/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// EditAdapter.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterUI.h"
#include "resource.h"
#include "PrinterUIDoc.h"
#include "PrinterUIView.h"
#include "EditAdapter.h"
#include "MainFrm.h"
#include "sizes.h"
#define  theman
#include "ini.h"
#include "PUIHelp.h"
#include "StringEx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditAdapter dialog


CEditAdapter::CEditAdapter(CWnd* pParent /*=NULL*/,CString mode)
	: CDialog(CEditAdapter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditAdapter)
	m_HostName = _T("");
	m_IPAddress = _T("");
	m_Location = _T("");
	m_MachineAddress = _T("");
	m_Type = _T("");
	//}}AFX_DATA_INIT
	m_mode=mode;
}


void CEditAdapter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CEditAdapter)
	DDX_Control(pDX, IDC_IPADDR, m_IP);
	DDX_Control(pDX, IDC_MachineAddress, m_MachineAddrCtrl);
	DDX_Control(pDX, IDC_HOSTNAME_ADAPTER_EDIT, m_HostEditCtrl);
	DDX_Control(pDX, IDC_TypeCombo, m_TypeCtrl);
	DDX_Text(pDX, IDC_HOSTNAME_ADAPTER_EDIT, m_HostName);
	DDV_MaxChars(pDX, m_HostName, 16);
	DDX_Text(pDX, IDC_LOCATION_ADAPTER_EDIT, m_Location);
	DDV_MaxChars(pDX, m_Location, 64);
	DDX_Text(pDX, IDC_MachineAddress, m_MachineAddress);
	DDX_CBString(pDX, IDC_TypeCombo, m_Type);
	//}}AFX_DATA_MAP
//	DDX_IPAddr(pDX, IDC_IPADDR, m_nAddr);
	//DDX_Control(pDX, IDC_IPAddress, m_IPEditCtrl);
//	DDX_Text(pDX, IDC_IPAddress, m_IPAddress);
}


BEGIN_MESSAGE_MAP(CEditAdapter, CDialog)
	//{{AFX_MSG_MAP(CEditAdapter)
	ON_CBN_SELCHANGE(IDC_TypeCombo, OnSelchangeTypeCombo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditAdapter message handlers

BOOL CEditAdapter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CStringEx strIP;
	adapter_type_node* t;

    //Fill Type Combo Box
	POSITION pos=AdapterTypeList.GetHeadPosition();
	while (pos !=NULL)
	{
		t=AdapterTypeList.GetNext(pos);
		m_TypeCtrl.AddString(t->name);
	}

    
	if(m_mode==_T("EDIT"))
	{
		CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
        int nIndex=pview->m_AdapterList.GetNextItem( -1, LVNI_SELECTED );
		//TODO Fill new IP Control

		strIP=m_IPAddress=pview->m_AdapterList.GetItemText(nIndex,0);
		m_IP.SetWindowText(strIP);

		m_HostName=pview->m_AdapterList.GetItemText(nIndex,1);       
		m_MachineAddress=pview->m_AdapterList.GetItemText(nIndex,2); 
		m_Location=pview->m_AdapterList.GetItemText(nIndex,3);		 
		m_Type=pview->m_AdapterList.GetItemText(nIndex,4);
		m_TypeCtrl.SelectString(-1,m_Type);
		UpdateData(FALSE);
		GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE); //disable IP on edit
		GetDlgItem(IDC_MachineAddress)->EnableWindow(FALSE); //disable Mac Addr too
		help_object.SetWindowHelp(HT_EditPrintAdapter_WindowDescription);
	}
	else
	{
		SetWindowText(_T("Add Print Adapter"));
		GetDlgItem(IDC_HOSTNAME_ADAPTER_EDIT)->SetWindowText(_T("NONE"));
		help_object.SetWindowHelp(HT_AddNTCSSAdapter_WindowDescription);
	}

	m_MachineAddrCtrl.SetWindowText(m_MachineAddress.IsEmpty()?_T("00:00:00:00:00:00"):m_MachineAddress); 
	m_MachineAddrCtrl.m_bisTime	       = FALSE;
	m_MachineAddrCtrl.m_isdate	       = FALSE;
	m_MachineAddrCtrl.m_bUseMask       = TRUE;
	m_MachineAddrCtrl.m_strMask        = _T("HH HH HH HH HH HH");
	m_MachineAddrCtrl.m_strLiteral     = _T("__:__:__:__:__:__");
	m_MachineAddrCtrl.m_str            = m_MachineAddress.IsEmpty()?_T("00:00:00:00:00:00"):m_MachineAddress;
	m_MachineAddrCtrl.m_strMaskLiteral = m_MachineAddrCtrl.m_str; 
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEditAdapter::AdapterChanged() 
{
	CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
    int nIndex=pview->m_AdapterList.GetNextItem( -1, LVNI_SELECTED );
	if(m_IPAddress!=pview->m_AdapterList.GetItemText(nIndex,0)) return TRUE;
	if(m_HostName!=pview->m_AdapterList.GetItemText(nIndex,1)) return TRUE;
	if(m_MachineAddress!=pview->m_AdapterList.GetItemText(nIndex,2)) return TRUE;
	if(m_Location!=pview->m_AdapterList.GetItemText(nIndex,3)) return TRUE;
	if(m_Type!=pview->m_AdapterList.GetItemText(nIndex,4)) return TRUE;
	return FALSE;
}

BOOL CEditAdapter::CheckChanges()

{
	
	CStringEx strError;
	
	try
	{
        
		
		if(m_HostName.IsEmpty()||m_HostName.Find(' ')!=-1||ini.IsHost(m_HostName))
		{
			strError.Format(_T("Invalid Hostname:%d"),IDC_HOSTNAME_ADAPTER_EDIT);
			throw(strError);
		}
		else
			m_HostName.MakeLower();
		
		if(!m_Location.IsEmpty())
			m_Location.MakeLower();
		
		if(m_MachineAddress.IsEmpty())
		{
			strError.Format(_T("Machine address can't be blank:%d"),IDC_MachineAddress);
			throw(strError);
		}

		if(m_mode!=_T("EDIT") && ((CPrinterUIView*) GetParentFrame()->GetActiveView())
			->m_AdapterList.Find(m_MachineAddress,2)) //check for dup machine address
		{
			strError.Format(_T("Machine address must be unique:%d"),IDC_MachineAddress);
			throw(strError);
		}

		else
			m_MachineAddress.MakeLower();
		
		if(m_Type.IsEmpty())
		{
			strError.Format(_T("Adapter type can't be blank:%d"),IDC_TypeCombo);
			throw(strError);
		}
		
		return TRUE;
	} //try
	
	catch(CStringEx strError)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(strError.GetField(':',0),MB_ICONEXCLAMATION);
		GetDlgItem(atoi(strError.GetField(':',1)))->SetFocus();
		return FALSE;
	}
}

void CEditAdapter::OnOK() 
{
	CString strIP;
	UpdateData(TRUE);
	if(!CheckChanges()) return;
	
    m_IP.GetWindowText(m_IPAddress);
	
	CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
	
	if(m_mode==_T("EDIT") && AdapterChanged())
	{
		adapter_node* t;
		CString temp=pview->m_AdapterList.GetText();
		POSITION pos=AdapterList.GetHeadPosition();
		while (pos!=NULL)
		{
			t=AdapterList.GetNext(pos);
			if(t->IPaddress==temp) break;
			
		}
		
		VERIFY(t->IPaddress==temp);
		
		if(t->IPaddress==temp)
		{
			if(t->hostname!=m_HostName && m_HostName!=_T("NONE")) //changed hostname
			{
				if(ini.CheckHost(m_HostName,strIP)) //returns true if host is known
				{
					if(pview->m_AdapterList.Find(strIP))
					{
						MessageBeep(MB_ICONEXCLAMATION);
						AfxMessageBox(_T("Host & IP address already exists")
							,MB_ICONEXCLAMATION);
						m_IP.SetWindowText(_T("")); //TODO make sure this works
						m_HostEditCtrl.SetWindowText(_T(""));
						m_HostEditCtrl.SetFocus();
						return;
					}
					
					MessageBeep(MB_ICONEXCLAMATION);
					
					AfxMessageBox(_T("Host name exists may be duplicate!"),
						MB_ICONEXCLAMATION);
					
					TRACE(_T("DID HOST MAY BE A DUPLICATE WARNING changing IP from <%s> to <%s>")
						,t->IPaddress,strIP);
					
					t->IPaddress=strIP; //use this IP & ignore the 1 in the edit box
				}
				else //host isn't known... check IP 
				{
					TRACE(_T("Edit Adapter... host isn't known checking IP "));
					if(ini.CheckIP(m_IPAddress))
					{ //bad news IP exists can't allow this
						TRACE(_T("Edit Adapter... Fatal error IP exists"));
						MessageBeep(MB_ICONEXCLAMATION);
						AfxMessageBox(_T("IP address already exists")
							,MB_ICONEXCLAMATION);
						// m_IP.SetWindowText(_T("")); //TODO check this
						m_IP.ClearAddress( );
						m_IPAddress.Empty();
						m_IP.SetFocus();
						return;
					}
					t->IPaddress=m_IPAddress; //else OK 
				}
				
			}

			UpdateListCtrl(t);

	/*		t->hostname=m_HostName;
			t->machine_address=m_MachineAddress;
			t->location=m_Location;
			t->type=m_Type;
			pview->FillPrinterListCtrl();    //Reset main view Printer List Ctrl
			pview->FillAdapterListCtrl();    //Reset main view Adapter List Ctrl
			ini.OutputAdapterNode("EDIT",t); //Write changes to output
			pview->ChangeEditBoxes(NULL,NULL); */
		}
	}
	else //it's an add
	{
		TRACE(_T("Adding new host -> ") + m_HostName);
		
		if(ini.CheckHost(m_HostName,strIP)) //Is host known 
		{
			if(pview->m_AdapterList.Find(strIP))
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(_T("Host & IP address already exists")
					,MB_ICONEXCLAMATION);
				m_IP.ClearAddress();
				m_HostEditCtrl.SetWindowText(_T(""));
				m_HostEditCtrl.SetFocus();
				return;
			}
			
			MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(_T("Host Name exists... may be duplicate"),
				MB_ICONEXCLAMATION);
			m_IPAddress=strIP; //use this IP & ignore the 1 in the edit box
			
		}
		else //host name didn't exist check IP
		{   
			
			if(!m_IPAddress.IsEmpty())
				if(ini.CheckIP(m_IPAddress))
					
				{ //bad news IP exists can't allow this
					MessageBeep(MB_ICONEXCLAMATION );
					AfxMessageBox(_T("IP address already exists!"),MB_ICONEXCLAMATION);
					m_IP.ClearAddress( );
					m_IPAddress.Empty();
					m_IP.SetFocus();
					return;
				}
		}
		
		adapter_node* t;
		t=ini.NewAdapterNode();
		UpdateListCtrl(t);

	} //add 
	
	CDialog::OnOK();
}


void CEditAdapter::OnSelchangeTypeCombo() 
{

	if(ini.FindAdapterInPrintList(m_IPAddress)&&m_mode==_T("EDIT"))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(_T("Delete Associated Printers before changing Type")
	    ,MB_ICONEXCLAMATION);
		m_TypeCtrl.SelectString(-1,m_Type);
		GetDlgItem(IDC_HOSTNAME_ADAPTER_EDIT)->SetFocus();
	}

}


BOOL CEditAdapter::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CEditAdapter::OnDestroy() 
{
	CDialog::OnDestroy();
	
	help_object.SetWindowHelp(HT_PrinterConfiguration_WindowDescription);	
}

void CEditAdapter::UpdateListCtrl(adapter_node* t)
//called to update parent list control
{
	CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
	ASSERT(pview);

	t->hostname=m_HostName;
	t->location=m_Location;
	t->type=m_Type;

	pview->m_AdapterList.SetRedraw(FALSE);

	if(m_mode==_T("EDIT")) //overwrite existing 
	{
		int nIndex=pview->m_AdapterList.GetSelectedItem();
		ASSERT (nIndex!=-1);
		pview->m_AdapterList.SetItemText(nIndex,1,m_HostName);
		pview->m_AdapterList.SetItemText(nIndex,2,m_MachineAddress);
		pview->m_AdapterList.SetItemText(nIndex,3,m_Location);
		pview->m_AdapterList.SetItemText(nIndex,4,m_Type);
		ini.OutputAdapterNode(_T("EDIT"),t); //Write changes to output
	}
	else
	{
		AdapterList.AddTail(t);
		t->IPaddress=m_IPAddress;
	    t->machine_address=m_MachineAddress;
		pview->m_AdapterList.Write(m_IPAddress);
		pview->m_AdapterList.Write(m_HostName);
		pview->m_AdapterList.Write(m_MachineAddress);
		pview->m_AdapterList.Write(m_Location);
		pview->m_AdapterList.Write(m_Type);
		ini.OutputAdapterNode(_T("ADD"),t);  //Write changes to output
	}

	pview->m_AdapterList.SetRedraw(TRUE);
	pview->ChangeEditBoxes(NULL,NULL);
	pview->GetDlgItem(IDC_Apply)->EnableWindow(TRUE);

}

