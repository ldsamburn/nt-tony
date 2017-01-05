/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_DTSOCKET_H__C5813C17_439E_4AC3_8D0C_6B8D4225FB84__INCLUDED_)
#define AFX_DTSOCKET_H__C5813C17_439E_4AC3_8D0C_6B8D4225FB84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DTSocket.h : header file
//

const int MAX_MESSAGE_SIZE=5000;
const int COMMAND_SIZE=20;
const int NTCSS_DATAGRAM_PORT=9121;
class CDTDlg;

/////////////////////////////////////////////////////////////////////////////
// CDTSocket command target

class CDTSocket : public CSocket
{
// Attributes

public:

// Operations
public:
	CDTSocket(CDTDlg* pDTDlg);
	virtual ~CDTSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDTSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
private:
	CDTDlg*	m_pDTDlg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DTSOCKET_H__C5813C17_439E_4AC3_8D0C_6B8D4225FB84__INCLUDED_)
