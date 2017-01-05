#if !defined(AFX_INPLACEEDIT_H__2E206B52_763E_11D1_A72E_006008421BB1__INCLUDED_)
#define AFX_INPLACEEDIT_H__2E206B52_763E_11D1_A72E_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InPlaceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int m_iItem,m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEEDIT_H__2E206B52_763E_11D1_A72E_006008421BB1__INCLUDED_)
