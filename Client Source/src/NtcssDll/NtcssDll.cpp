/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NtcssDll.h"
#include "PrintUtil.h"
#include "0_basics.h"
//#include "Winsock2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNtcssDll

BEGIN_MESSAGE_MAP(CNtcssDll, CWinApp)
	//{{AFX_MSG_MAP(CNtcssDll)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNtcssDll construction ---------  did it Whit's way

CNtcssDll::CNtcssDll()
{
	GetLocalHostANDIP();
	//if(!AfxSocketInit())
		//AfxMessageBox("Sockinit Failed");
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNtcssDll object

CNtcssDll gbl_theNtcssDLL;
//CNtcssDll  gbl_theNtcssDLL("NtcssDll.dll");

int CNtcssDll::ExitInstance() 
{
	CdllSpoolPrintManager* prt_man = gbl_dll_man.dllSpoolPrintMan();
	if(prt_man) //only returns ptr if this process has print lock
	{
		CPrintUtil PU;
		PU.DelTempPrinter(); //take out temp printer if there was a problem
		PU.DelNTCSSPort();
			                     
	}
	gbl_dll_man.APPcall_dismissThisDllInstance();	
	return CWinApp::ExitInstance();
}

BOOL CNtcssDll::InitInstance() 
{
	SetDialogBkColor(); 
	//Everythread does not hit this!!!
	//if(!AfxSocketInit())
	//	AfxMessageBox("Sockinit Failed");
	//TRACE1("Thread %ld has called AfxSocketInit\n",GetCurrentThreadId());

  //return TRUE; Why did Whit do this	
	return CWinApp::InitInstance();
}

void CNtcssDll::GetLocalHostANDIP()
{
	char buf[256];
	
	WORD wVersionRequested = MAKEWORD(1, 1);  
	WSADATA wsaData; 
	int err = WSAStartup(wVersionRequested, &wsaData); 
	
	gethostname(buf,sizeof(buf));

	m_csLocalHostName=buf;
	
	//get host information from the host name
	HOSTENT* pHostEnt = gethostbyname(buf);
	if (pHostEnt == NULL)
	{
		TRACE1(_T("Failed in call to gethostbyname, WSAGetLastError returns %d\n"), WSAGetLastError());
		return;
	}
	
	//check the length of the IP adress
	if (pHostEnt->h_length != 4)
	{
		TRACE0(_T("IP address returned is not 32 bits !!\n"));
		return;
	}
	
    in_addr address;
    CopyMemory(&address.S_un.S_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);

	m_csLocalHostIP.Format(_T("%d.%d.%d.%d"),address.S_un.S_un_b.s_b1, 
		address.S_un.S_un_b.s_b2,address.S_un.S_un_b.s_b3,address.S_un.S_un_b.s_b4);
	
	WSACleanup(); 
}
