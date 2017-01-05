#if !defined(AFX_MYHEADERCTRL_H__6883C4D2_DBD0_11D2_9493_0050041C95DD__INCLUDED_)
#define AFX_MYHEADERCTRL_H__6883C4D2_DBD0_11D2_9493_0050041C95DD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MyHeaderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyHeaderCtrl window

class CMyHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CMyHeaderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyHeaderCtrl();
	int SetSortImage( int nCol, BOOL bAsc);

	// Generated message map functions
protected:
	int	m_nSortCol;
	BOOL m_bSortAsc;
	
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	//{{AFX_MSG(CMyHeaderCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYHEADERCTRL_H__6883C4D2_DBD0_11D2_9493_0050041C95DD__INCLUDED_)
