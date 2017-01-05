/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// RedirectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRedirectDialog dialog
#define   theman
#include "ini.h"
#include "PUIListCtrl.h"


class CRedirectDialog : public CDialog
{
// Construction
public:
	CRedirectDialog(CWnd* pParent = NULL,printer_node* t=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRedirectDialog)
	enum { IDD = IDD_REDIRECTDIALOG };
	PUIListCtrl	m_ListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRedirectDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	printer_node* m_PrinterNode;
protected:

	// Generated message map functions
	//{{AFX_MSG(CRedirectDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
