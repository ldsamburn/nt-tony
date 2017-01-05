/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AppLibDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AppLibDll.h"
#include "sizes.h"
//#include "NtcssAppLibMsgs.h"
#include "encrypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAppLibDllApp

BEGIN_MESSAGE_MAP(CAppLibDllApp, CWinApp)
	//{{AFX_MSG_MAP(CAppLibDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppLibDllApp construction

CAppLibDllApp::CAppLibDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAppLibDllApp object

CAppLibDllApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAppLibDllApp initialization

BOOL CAppLibDllApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}
int ConnectServer(CAsyncSocket *cps, int port, char *host);


extern "C" BOOL __declspec(dllexport) WINAPI NtcssGetDBPassword(char *loginname,
						char *password,
						char *appname,
						char *hostname,
						char *dbpassword)
{

struct {
	char mt[20];
	char logname[8];
	char password[32];
	char progroup[16];
	char termchar;
} msg_buf, *msg_buf_ptr;
int nResult;
CSocket cSock;
char inbuf[1000];
CString temp;
int nRet;
struct sockaddr_in socketAddr;
struct hostent far *lpHe;
int sock;
//char cryptkey[32];
//char cryptpwd[32];
CString strEncryptPassword;
CString strLoginname=loginname;
CString strPlainPassword=password;
CEncrypt crypt;
//char tbuf[100];
		
	strEncryptPassword=crypt.plainEncryptString(strLoginname+strLoginname, 
		strPlainPassword);
	
	if (strEncryptPassword.IsEmpty()) {
		AfxMessageBox("Unable to encrypt password, aborting!");
		return -1;
	}


	nResult = 0;

	msg_buf_ptr=&msg_buf;
	memset(msg_buf_ptr,' ',sizeof(msg_buf)-1);

	lpHe=gethostbyname((char far *)hostname);
	 
	if (!lpHe) {
    	sock=0;
    	return -1;
    }         

    socketAddr.sin_port=htons(9142);
    socketAddr.sin_family=AF_INET;

    memcpy(&socketAddr.sin_addr,(char far *)lpHe->h_addr_list[0],lpHe->h_length);

    sock=socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
                return -1;


    nResult=connect(sock,(const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (nResult < 0) { 
        if (cSock.GetLastError() != WSAEWOULDBLOCK) {
  		  	closesocket(sock);
 		   	sock=0;
    		return -1;
    	}
    }

	memset(msg_buf.mt,NULL,20);
	strcpy(msg_buf.mt,"REMOTEVALIDATE");
	memcpy(msg_buf.logname,loginname,strlen(loginname));
	memcpy(msg_buf.password,strEncryptPassword,strlen(strEncryptPassword));
	msg_buf.password[strlen(strEncryptPassword)]=':';
//	strcat(msg_buf.password,":");
//	AfxMessageBox(msg_buf.password);
	memcpy(msg_buf.progroup,appname,strlen(appname));
	msg_buf.termchar=NULL;

	nRet=send(sock,(char *)&msg_buf,sizeof(msg_buf),0);
	if (nRet < 0) {
		closesocket(sock);
		return GetLastError();
	}
	Sleep(1000);
	memset(inbuf,NULL,1000);

	nRet=recv(sock,inbuf,1000,0);
	if (nRet < 0) {
		closesocket(sock);
		return GetLastError();
	}

	if (!strcmp(inbuf,"NOREMOTEVALIDATE")) 
		AfxMessageBox(inbuf+20+5);
	else
		strcpy(dbpassword,inbuf+20+5);

	closesocket(sock);


	return nResult;
}


int ConnectServer(CAsyncSocket *cps, int port, char *host)
{
int sock;
struct sockaddr_in socketAddr;
struct hostent far *lpHe;
int retcode;

	 

	lpHe=gethostbyname((char far *)host);
	 
	if (!lpHe) {
    	sock=0;
    	return -1;
    }          

    socketAddr.sin_port=htons(port);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,(char far *)lpHe->h_addr_list[0],lpHe->h_length);

	cps->Create(0,SOCK_STREAM,NULL);

	sock=cps->m_hSocket;
     
    retcode=cps->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (cps->GetLastError() != WSAEWOULDBLOCK) {
  		  	cps->Close();
 		   	sock=0;
    		return -1;
    	}
    }
	return sock;
      
}



