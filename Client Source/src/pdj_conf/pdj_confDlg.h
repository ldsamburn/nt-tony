/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pdj_confDlg.h : header file
//
#include <sizes.h>
#include <ntcssdef.h>
#include <ntcssapi.h>
#include "PDJListCtrl.h"
//#include "PDJHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CPdj_confDlg dialog
/*
#define DESKTOP_PORT	9121
#define PRINTQ_PORT		9122
#define CMD_PORT		9152 
#define SPCQ_PORT	    9162
#define DB_PORT 		9132 

#define NTCSS_MASTER_MSG 1
#define NTCSS_APP_MSG 2
#define	SIZE_MESSAGE_TYPE 20




struct GET_PDJ_INI_MSG {
    	char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
		char terminator;
	};

struct PUT_PDJ_INI_MSG {
    	char message_name[SIZE_MESSAGE_TYPE];
    	char login_name[SIZE_LOGIN_NAME];
    	char token[SIZE_TOKEN];
		char iniFile[NTCSS_SIZE_COMMAND_LINE];
		char terminator;
	};

*/

#define NTCSS_BASE_NUMBER 100

class CPdj_confDlg : public CDialog
{
// Construction
public:
	CPdj_confDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL PopulateApps(CListBox *app_list);
	void WriteProfileOutData(char *group,
		  				     char *command,
							 char *title_buf,
							 char *command_line,
							 char *role_list,
							 int role_mask,
							 char *job_type,
							 char *schedule,
							 char *inifile,
							 char *iniOutfile);
//	int SendServerMessage(char far *msg, 
//						 int msgbuf_len,
//						 int server_port,
//						 int mst_app_msg,
//						 char far *good_resp_str,
//						 char far *bad_resp_str,
//						 char far *recv_buffer,
//						 int recv_buffer_len);
	int GetINIFile(char *appname);
	int PutINIFile(char *appname);
	void DisableMenuItems(int first,...);
	void EnableMenuItems(int first,...);
	void FindSectionByTitle(char *retbuf,char *title_buf, char *group);
	void ClearAllLocks(void);
	void UpdatePDJList();


//	CPDJHelp m_HelpObject;
	CString m_iniFile;
	CString m_iniOutFile;
	CString m_Source;
	CString m_Target;
	CString m_rLogFile;
	CString m_lLogFile;
	CString m_SpoolDir;
	CString m_WindowsDir;

	CString m_LastApplication;

//	char m_SpoolDir[MAX_PATH];
//	char m_WindowsDir[MAX_PATH];
	struct {
		char AppName[SIZE_GROUP_TITLE+1];
		char LockId[LOCK_ID_LEN];
	} m_LockIds[SIZE_PRT_FILE_NAME];  // Gives about 128 items

	int m_NewIniEntry;
	int m_NumApps;

	int m_ChangeCount;
	NtcssUserInfo m_ui;
// Dialog Data
	//{{AFX_DATA(CPdj_confDlg)
	enum { IDD = IDD_PDJ_CONF_DIALOG };
	CPDJListCtrl	m_ListCtrl;
	CString	m_Application;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPdj_confDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPdj_confDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSystemApply();
	afx_msg void OnDblclkPdjList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeApplicationList();
	afx_msg void OnPredfinedjobsAdd();
	afx_msg void OnPredfinedjobsDelete();
	afx_msg void OnPredfinedjobsEdit();
	afx_msg void OnSystemCancel();
	afx_msg void OnSystemQuit();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSystemViewchangelog();
	afx_msg void OnClickPdjList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnPdjAbout();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
