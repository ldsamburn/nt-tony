/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AddEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddEdit dialog



class CAddEdit : public CDialog
{
// Construction
public:
	CAddEdit(CWnd* pParent = NULL);   // standard constructor
// CAddEdit message handlers
	void ResetRoles();
	void AddRoleEntry(char *entry);
	void ResetTypes();
	void AddTypeEntry(char *entry);
	void GetRoleList(char *role_buf);
	int GetRoles();
	void GetJobTitle(char *title_buf); 
	void GetCommandLine(char *command_buf); 
	void GetJobType(char *type_buf); 
	void GetSchedule(char *schedule_buf);
	BOOL IsValid();
	void SetTitle(char *entry);
	void SetCommandLine(char *entry);
	void SetSchedule(char *entry);
	void SetSelectedType(char *entry); 
	void SetSelectedRoles(char *entry);


	int m_Roles;
	CString m_RoleList;
	CString m_TypeList;
	CString m_SelectedType;
	CString m_SelectedRoles;
	CString m_Schedule;
	BOOL	m_DataValid;

// Dialog Data
	//{{AFX_DATA(CAddEdit)
	enum { IDD = IDD_PDJ_EDIT };
	CString	m_JobType;
	CString	m_CommandLine;
	CString	m_JobTitle;
	CString	m_RoleListEntry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddEdit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnScheduleButton();
	virtual void OnCancel();
	afx_msg void OnSelchangeJobType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
