/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ModifyOtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppPrintConfig.h"
#include "ModifyOtDlg.h"
#include "APCHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyOtDlg dialog


CModifyOtDlg::CModifyOtDlg(CWnd* pParent /*=NULL*/,CString mode,CAppPrintConfigDlg* dad)
	: CDialog(CModifyOtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyOtDlg)
	m_Batch = FALSE;
	m_Default = _T("");
	m_Description = _T("");
	m_App = _T("");
	m_Name = _T("");
	//}}AFX_DATA_INIT
	mMode=mode;
	m_dad=dad; //get pointer to friend class
}


void CModifyOtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyOtDlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_DesripCtrl);
	DDX_Control(pDX, IDC_BATCHPRINT, m_BatchCtrl);
	DDX_Control(pDX, IDC_DEFAULTPRINTER, m_DefaultPrinterComboBox);
	DDX_Control(pDX, IDC_NAME, m_NameCEdit);
	DDX_Control(pDX, IDC_PRINTERLIST, m_PrinterList);
	DDX_Check(pDX, IDC_BATCHPRINT, m_Batch);
	DDX_CBString(pDX, IDC_DEFAULTPRINTER, m_Default);
	DDX_Text(pDX, IDC_DESCRIPTION, m_Description);
	DDV_MaxChars(pDX, m_Description, 64);
	DDX_Text(pDX, IDC_APPLICATION, m_App);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyOtDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyOtDlg)
	ON_LBN_SELCHANGE(IDC_PRINTERLIST, OnSelchangePrinterlist)
	ON_CBN_SELCHANGE(IDC_DEFAULTPRINTER, OnSelchangeDefaultprinter)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyOtDlg message handlers

BOOL CModifyOtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateDisplay();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CModifyOtDlg::UpdateDisplay()
{
//	int nIndex=m_dad->m_OutPutCtrl.GetNextItem( -1, LVNI_SELECTED);

	//fill in printer combo box from parent's listctrl
	for(int i=0;i<m_dad->m_PrinterCtrl.GetItemCount();i++)
		m_PrinterList.InsertString(-1,m_dad->m_PrinterCtrl.GetItemText(i,0)/*+"@"+
        m_dad->m_PrinterCtrl.GetItemText(i,1) 12/18/98*/);
	
	if (mMode=="ADD") //add stuff
	{
		SetWindowText("ADD OUTPUT TYPE");
		help_object.SetWindowHelp(HT_AddOutputType_WindowDescription);
	}
	else              //edit stuff
	{
		help_object.SetWindowHelp(HT_EditOutputType_WindowDescription);
		m_Name=m_dad->GetName();
		m_Description=m_dad->GetDescription();
		m_NameCEdit.EnableWindow(FALSE);
		int x=-1;
		while((x=m_dad->m_PrinterCtrl.GetNextItem( x, LVNI_SELECTED))>=0 )
		{	
			m_DefaultPrinterComboBox.InsertString(-1,m_dad->m_PrinterCtrl.GetItemText(x,0)/*+
			"@"+m_dad->m_PrinterCtrl.GetItemText(x,1) 12/18/98*/);
			m_PrinterList.SetSel(x); //make selections match parent
		}
		m_Default=m_dad->GetDefaultPrinter();
	
	}
                      //common stuff
	if(m_PrinterList.GetSelCount()>0)
	{
		m_DefaultPrinterComboBox.EnableWindow(TRUE);
		m_BatchCtrl.EnableWindow(TRUE);
	}

	m_App=m_dad->m_AppList;
	CString temp=m_dad->GetBatch();
	temp.MakeLower();
	m_Batch=temp==_T("yes");
		
	UpdateData(FALSE);
}


BOOL CModifyOtDlg::CheckChanges() 
// error checking
{

	CString temp;
	BOOL ok=TRUE;

	if (m_Name.Find(' ')>0)
	{
		temp.Format("Output Type can't contain spaces");
		ok=FALSE;
	}
	if(ok)
		if(m_Name.GetLength()<MIN_OT_LEN||m_Name.GetLength()>MAX_OT_LEN)
		{
					
			temp.Format("Output Type must be between %d & %d Characters",MIN_OT_LEN,
						 MAX_OT_LEN);
			ok=FALSE;
			
		}

	//checks for duplicate OT on an add
	//

	if(ok&&mMode=="ADD")
	{
		LV_FINDINFO ls_findinfo;
		ls_findinfo.flags=LVFI_STRING;
		UpdateData(TRUE);
		ls_findinfo.psz=(LPCTSTR)m_Name;
		if(m_dad->m_OutPutCtrl.FindItem(&ls_findinfo,-1)!=-1)
		{
			temp.Format("Output Type Already Exists");
			ok=FALSE;
		}
	}

	if(ok)
		if(m_Description.GetLength()<MIN_DESCRIP_LEN||
		   m_Description.GetLength()>MAX_DESCRIP_LEN)
		{
			MessageBeep(MB_ICONEXCLAMATION );
			temp.Format("Description must be between %d & %d Characters!"
				,MIN_DESCRIP_LEN,MAX_DESCRIP_LEN);
			ok=FALSE;
		}
	if(!ok)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(temp,MB_ICONEXCLAMATION);
	//	m_NameCEdit.SetFocus();
	}
	return ok;
}


BOOL CModifyOtDlg::CheckForChanges()
//Checks to see if anything changed on an edit only
{
	if(m_PrinterList.GetSelCount()>0)
	{
		
		GetNewPrinterString();
		if(m_dad->GetPrinterString()!=m_strNumericPrintList)
			return TRUE;
	
	}

	if(m_dad->GetName()!=m_Name)
		return TRUE;
	if(m_dad->GetDescription()!=m_Description)
		return TRUE;
	CString temp=m_dad->GetBatch();
	temp.MakeLower();
	if(temp=="yes" && !m_Batch)
		return TRUE;
	if(temp=="no" && m_Batch)
		return TRUE;
	if(m_dad->GetDefaultPrinter()!=m_Default)
	
		return TRUE;

	return FALSE;
}

void CModifyOtDlg::GetNewPrinterString()
//updates member variable m_strNumericPrintList to reflect printers currently selected in control

{
	m_strNumericPrintList="";//using class member here so we can reuse in UpdateParent
	int ret=m_PrinterList.GetSelCount();
	int *pint = new int[ret+1];
	m_PrinterList.GetSelItems(ret,pint);
	CString temp;
	for(int i=0;i<ret;i++)
	{
		m_PrinterList.GetText(*(pint++),temp);
		if(!m_strNumericPrintList.IsEmpty())
			m_strNumericPrintList+= "," + temp;
		else 
			m_strNumericPrintList=temp;
	}

//	delete [] pint; //TODO this causes assertion in debug & doesn't seem to leak
}

void CModifyOtDlg::UpdateParent() 
//will put changes in parent control
{

	m_dad->new_node->title=m_Name;
	m_dad->new_node->group=m_dad->m_AppList;
	m_dad->new_node->Default=m_Default/*+m_Host*/;
	m_dad->new_node->description=m_Description;
	m_dad->new_node->batch= m_Batch ? "Yes" : "No";
	m_dad->new_node->printers=m_strNumericPrintList;
	m_dad->new_node->operation=mMode;
}
	

void CModifyOtDlg::OnOK() 
{
	UpdateData();
	if(!CheckChanges())
		return; 
	if(mMode=="EDIT")
		if(CheckForChanges())
			UpdateParent();
	if(mMode=="ADD")
	{
		if(m_PrinterList.GetSelCount()>0)
			GetNewPrinterString();
		UpdateParent();
	}
	CDialog::OnOK();
}



void CModifyOtDlg::OnSelchangePrinterlist() 
{
	if(m_PrinterList.GetSelCount()>0)
	{
		
		m_DefaultPrinterComboBox.EnableWindow(TRUE);
		m_BatchCtrl.EnableWindow(TRUE);
		m_DefaultPrinterComboBox.ResetContent();
		int *pint = new int[m_PrinterList.GetSelCount()];
		int ret=m_PrinterList.GetSelItems( m_PrinterList.GetSelCount(), (LPINT) pint);
		CString temp;
		for(int i=0;i<ret;i++)
		{
			m_PrinterList.GetText(*(pint++),temp);
			m_DefaultPrinterComboBox.InsertString(-1,temp);
		}
		if(m_PrinterList.FindString(-1,m_Default)==LB_ERR )
		{	//see if they deleted default
			UpdateData(TRUE);
			m_Default=_T("");      //printer & clear if so
			UpdateData(FALSE);

		}
		else
			m_DefaultPrinterComboBox.SelectString(-1,m_Default);

		//delete[] pint; this causes assertion in debug & doesn't seem to leak

	}
	else  //clear out default in case there was one
	{
		
		m_Default=_T("");      //printer & clear if so
		m_DefaultPrinterComboBox.ResetContent();
		m_DefaultPrinterComboBox.EnableWindow(FALSE);
		m_BatchCtrl.EnableWindow(FALSE);
	}
}

void CModifyOtDlg::OnSelchangeDefaultprinter() 
{
  UpdateData(TRUE);	
}

BOOL CModifyOtDlg::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CModifyOtDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	help_object.SetWindowHelp(HT_ApplicationPrinterConfiguration_WindowDescription);	
}
