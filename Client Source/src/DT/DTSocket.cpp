/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DTSocket.cpp : implementation file
//

#include "stdafx.h"
#include "DT.h"
#include "DTSocket.h"
#include "DTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDTSocket

CDTSocket::CDTSocket(CDTDlg* pDTDlg)
{
	m_pDTDlg=pDTDlg;
}

CDTSocket::~CDTSocket()
{
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CDTSocket, CSocket)
	//{{AFX_MSG_MAP(CDTSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CDTSocket member functions

void CDTSocket::OnReceive(int nErrorCode) 
{

	TRACE0(_T("UDP Listner has been hit!!!\n"));
	m_pDTDlg->UDPSelect();
	CSocket::OnReceive(nErrorCode);
}
