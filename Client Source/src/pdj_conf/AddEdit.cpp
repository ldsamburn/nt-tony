/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AddEdit.cpp : implementation file
//

#include "stdafx.h"
#include <NtcssApi.h>
#include <Math.h>
#include "pdj_conf.h"
#include "AddEdit.h"
#include "PDJHelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEdit dialog


CAddEdit::CAddEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEdit)
	m_JobType = _T("");
	m_CommandLine = _T("");
	m_JobTitle = _T("");
	m_RoleListEntry = _T("");
	//}}AFX_DATA_INIT
}


void CAddEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEdit)
	DDX_CBString(pDX, IDC_JOB_TYPE, m_JobType);
	DDX_Text(pDX, IDC_PDJ_COMMAND, m_CommandLine);
	DDX_Text(pDX, IDC_PDJ_TITLE, m_JobTitle);
	DDX_LBString(pDX, IDC_ROLE_LIST, m_RoleListEntry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEdit, CDialog)
	//{{AFX_MSG_MAP(CAddEdit)
	ON_BN_CLICKED(IDC_SCHEDULE_BUTTON, OnScheduleButton)
	ON_CBN_SELCHANGE(IDC_JOB_TYPE, OnSelchangeJobType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEdit message handlers
void CAddEdit::ResetRoles()
{
	m_RoleList = "";
}

void CAddEdit::AddRoleEntry(char *entry)
{
	m_RoleList = entry;

}

void CAddEdit::ResetTypes()
{

	m_TypeList = "";
}

void CAddEdit::AddTypeEntry(char *entry)
{
	m_TypeList = entry;
}


void CAddEdit::SetTitle(char *entry)
{
	m_JobTitle = entry;
}

void CAddEdit::SetCommandLine(char *entry)
{
	m_CommandLine = entry;
}

void CAddEdit::SetSchedule(char *entry)
{
	m_Schedule = entry;
}


BOOL CAddEdit::OnInitDialog() 
{
char *tptr;
char pdj_roles[MAX_ROLE_LEN*8+1];
char selected_roles[MAX_ROLE_LEN*8+1];
char selected_type[MAX_TYPE_LEN+1];
int i,j;
CListBox *role_list=(CListBox *)GetDlgItem(IDC_ROLE_LIST);
CComboBox *type_list=(CComboBox *)GetDlgItem(IDC_JOB_TYPE);

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here


	memset(pdj_roles,NULL,MAX_ROLE_LEN);
	strcpy(pdj_roles,m_RoleList);
	pdj_roles[strlen(pdj_roles)+1] = NULL;

	j=strlen(pdj_roles);
	for (i=0;i<j;i++) {
			if (pdj_roles[i] == ',') 
					pdj_roles[i] = '\0';
	}


	tptr=pdj_roles;
	while (strlen(tptr)) {
		role_list->AddString(tptr);
		tptr += strlen(tptr) + 1;
	}

	//  
	// set selected roles 

	memset(selected_roles,NULL,MAX_ROLE_LEN*8+1);
	strcpy(selected_roles,m_SelectedRoles);
	selected_roles[strlen(selected_roles)+1] = NULL;

	j=strlen(selected_roles);
	for (i=0;i<j;i++) {
			if (selected_roles[i] == ',') 
					selected_roles[i] = '\0';
	}

	tptr=selected_roles;
	while(strlen(tptr)) {
		j=role_list->FindString(-1,tptr);
		if (j != LB_ERR)
			role_list->SetSel(j,TRUE);
		tptr += strlen(tptr) + 1;
	}


	memset(pdj_roles,NULL,MAX_TYPE_LEN);
	strcpy(pdj_roles,m_TypeList);
	pdj_roles[strlen(pdj_roles)+1] = NULL;

	j=strlen(pdj_roles);
	for (i=0;i<j;i++) {
			if (pdj_roles[i] == ',') 
					pdj_roles[i] = '\0';
	}

	tptr=pdj_roles;
	while (strlen(tptr)) {
		type_list->AddString(tptr);
		tptr += strlen(tptr) + 1;
	}

	memset(selected_type,NULL,MAX_TYPE_LEN);
	strcpy(selected_type,m_SelectedType);

	j=type_list->FindString(-1,selected_type);
	if (j != CB_ERR)
		type_list->SetCurSel(j);

	CButton *cb=(CButton *)GetDlgItem(IDC_SCHEDULE_BUTTON);
	cb->EnableWindow(FALSE);
	if (m_SelectedType != "Regular" && !m_SelectedType.IsEmpty()) {
		cb->EnableWindow(TRUE);
		if (!m_Schedule.IsEmpty())
			cb->SetWindowText("Edit Schedule");
		else {
			cb->SetWindowText("Add Schedule");
		}
	}


	help_object.SetWindowHelp(HT_AddPredefinedJob_WindowDescription);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddEdit::OnOK() 
{
int nSelItems,i;
CListBox *role_list=(CListBox *)GetDlgItem(IDC_ROLE_LIST);
int nSelIdx[MAX_ROLE_LEN];
char nSelItemBuf[MAX_ROLE_LEN+1];

	UpdateData(TRUE);

	nSelItems=role_list->GetSelCount();

	if (m_JobTitle.IsEmpty()) {
		AfxMessageBox("Invalid Job Title");
		return;
	} else if (m_CommandLine.IsEmpty()) {
		AfxMessageBox("Invalid Command Line");
		return;
	} else if (nSelItems == 0) {
		AfxMessageBox("No Roles Selected");
		return;
	} else if (m_JobType.IsEmpty())  {
		AfxMessageBox("No Job Type Selected");
		return;
	}

	
	role_list->GetSelItems(MAX_ROLE_LEN,nSelIdx);

	m_SelectedRoles = "";
	m_Roles=0;
	for (i=0;i<nSelItems;i++) {
		/********HACKORAMA.... make sure to get rid of the +2 when roles are 
		 right justified in the future ************************/
		m_Roles += (int)pow(2,nSelIdx[i]+2);

		// Version 1.0.7 doesn't need the +2 
		//m_Roles += (int)pow(2,nSelIdx[i]);

		/********** end of hackorama ***************/

		memset(nSelItemBuf,NULL,MAX_ROLE_LEN+1);
		role_list->GetText(nSelIdx[i],nSelItemBuf);
		if (m_SelectedRoles.IsEmpty())
			m_SelectedRoles = nSelItemBuf;
		else
			m_SelectedRoles = m_SelectedRoles + "," + nSelItemBuf;
	}

	m_DataValid=TRUE;

	CButton *cb=(CButton *)GetDlgItem(IDC_SCHEDULE_BUTTON);
	cb->EnableWindow(FALSE);


	CDialog::OnOK();
}

void CAddEdit::OnScheduleButton() 
{
char tsched[NTCSS_MAX_SCHEDULE_LEN+1];
char oldsched[NTCSS_MAX_SCHEDULE_LEN+1];

	memset(tsched,NULL,NTCSS_MAX_SCHEDULE_LEN+1);
	memset(oldsched,NULL,NTCSS_MAX_SCHEDULE_LEN+1);

	if (m_Schedule.IsEmpty())
		if (NtcssCreateSchedule(tsched,NTCSS_MAX_SCHEDULE_LEN))
			m_Schedule=tsched;
		else
			m_Schedule = "";
	else {
		strcpy(oldsched,m_Schedule);
		if (NtcssEditSchedule(oldsched,tsched,NTCSS_MAX_SCHEDULE_LEN))
			m_Schedule=tsched;
		else
			m_Schedule = "";
	}
	CButton *cb=(CButton *)GetDlgItem(IDC_SCHEDULE_BUTTON);
	cb->SetWindowText("Edit Schedule");

}

void CAddEdit::GetRoleList(char *role_buf) 
{
	strcpy(role_buf,m_SelectedRoles);
}

int CAddEdit::GetRoles() 
{
	return(m_Roles);
}

void CAddEdit::GetJobTitle(char *title_buf) 
{
	strcpy(title_buf,m_JobTitle);
}

void CAddEdit::GetCommandLine(char *command_buf) 
{
	strcpy(command_buf,m_CommandLine);
}

void CAddEdit::GetJobType(char *type_buf) 
{
	strcpy(type_buf,m_JobType);
}

void CAddEdit::GetSchedule(char *schedule_buf) 
{
	strcpy(schedule_buf,m_Schedule);
}


BOOL CAddEdit::IsValid() 
{
	return(m_DataValid);
}

void CAddEdit::OnCancel() 
{
	CButton *cb=(CButton *)GetDlgItem(IDC_SCHEDULE_BUTTON);
	cb->EnableWindow(FALSE);
	m_DataValid=FALSE;
	CDialog::OnCancel();
}

void CAddEdit::SetSelectedType(char *entry) 
{
	m_SelectedType=entry;
}

void CAddEdit::SetSelectedRoles(char *entry) 
{
	m_SelectedRoles=entry;
}



BOOL CAddEdit::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);	
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CAddEdit::DestroyWindow() 
{
	help_object.SetWindowHelp(HT_BatchJobQueue_WindowDescriptions);
	
	return CDialog::DestroyWindow();
}


void CAddEdit::OnSelchangeJobType() 
{
CButton *bt=(CButton *)GetDlgItem(IDC_SCHEDULE_BUTTON);
CComboBox *cb=(CComboBox *)GetDlgItem(IDC_JOB_TYPE);
char jtype[100];

	memset(jtype,NULL,100);
	int jdx=cb->GetCurSel();

	cb->GetLBText(jdx,jtype);

	if (strcmp(jtype,"Regular") == 0 || strlen(jtype) == 0)
		bt->EnableWindow(FALSE);
	else
		bt->EnableWindow(TRUE);
	
}


