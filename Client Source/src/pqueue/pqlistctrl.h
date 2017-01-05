/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_CPQListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_)
#define AFX_CPQListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CPQL.h : header file
//
#include "MyListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPQL window

class CPQListCtrl : public MyListCtrl
{
// Construction
public:
	CPQListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPQListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPQListCtrl();
	virtual void SetHelpTopic(long lTopic);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPQL)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPQListCtrl_H__1AA05432_E351_11D1_A74B_006008421BB1__INCLUDED_)
