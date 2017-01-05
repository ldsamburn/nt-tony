/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// dmcdlg.h : header file
//
#define NUM_TAB_STOPS 10   

/////////////////////////////////////////////////////////////////////////////
// CDMCDlg dialog

class CDMCDlg : public CDialog
{
// Construction
public:
	CDMCDlg(CWnd* pParent = NULL);	// standard constructor

    int m_TabStops[NUM_TAB_STOPS];
    WORD m_DlgWidthUnits;

// Dialog Data
	//{{AFX_DATA(CDMCDlg)
	enum { IDD = IDD_DMC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDMCDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
