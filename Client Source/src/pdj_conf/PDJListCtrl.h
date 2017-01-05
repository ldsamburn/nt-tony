/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_CPDJListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_)
#define AFX_CPDJListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CPDJL.h : header file
//
#include "MyListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPDJL window

class CPDJListCtrl : public MyListCtrl
{
// Construction
public:
	CPDJListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPDJListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPDJListCtrl();
	virtual void SetHelpTopic(long lTopic);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPDJL)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPDJListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_)
