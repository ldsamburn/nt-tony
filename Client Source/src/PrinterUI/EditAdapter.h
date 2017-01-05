/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// EditAdapter.h : header file
//

#include "Masked.h"
//#include "ipaddr.h"
//#include "OXMaskedEdit.h"


/////////////////////////////////////////////////////////////////////////////
// CEditAdapter dialog

class CEditAdapter : public CDialog
{
// Construction
public:
	CEditAdapter(CWnd* pParent = NULL,CString Mode="EDIT");   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditAdapter)
	enum { IDD = IDD_EditAdapterDlg };
	CIPAddressCtrl	m_IP;
	CMaskEdit m_MachineAddrCtrl;
	CEdit	m_HostEditCtrl;
	CComboBox	m_TypeCtrl;
	CString	m_HostName;
	CString	m_IPAddress;
	CString	m_Location;
	CString	m_MachineAddress;
	CString	m_Type;
	//}}AFX_DATA

//	unsigned char m_nAddr[4];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditAdapter)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditAdapter)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeTypeCombo();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
//		CIPAddrCtl* m_pIPAddrCtl;

		CString m_mode;
		BOOL AdapterChanged();
		BOOL CheckChanges();
		void UpdateListCtrl(adapter_node* t);
};
