/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include <afxsock.h>

class CDTLibAppDlg;

class CLoginSocket : public CSocket
{
	DECLARE_DYNAMIC(CLoginSocket);

// Construction
public:
	CLoginSocket(CDTLibAppDlg* ctd);
	void CheckWnd(void);

// Operations
public:
	CDTLibAppDlg* dlgptr;

// Implementation
protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};

