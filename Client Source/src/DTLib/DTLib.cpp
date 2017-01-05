/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

////////////////////////////////////////////////////////////////
//
// DTLib.cpp : Defines the initialization routines for the DLL.
// Desktop DLL Library
// ACA 6/11/01...............Original Coding
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DTLib.h"
#include "DTMsgs.h"
#include "lrs.h"
#include <lm.h>
#include "lmaccess.h"
#include "lmserver.h"
#include "afxconv.h"
#include "wchar.h"
#include "ntcssdef.h"
#include "bundleda.h"
#include "addacl.h"
#define __DTEXPORT
#include "dtimport.h"
#include "timeoutsocket.h"
#include "StringEx.h"
#include "IniFile.h"


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
// CDTLibApp

BEGIN_MESSAGE_MAP(CDTLibApp, CWinApp)
	//{{AFX_MSG_MAP(CDTLibApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDTLibApp construction

CDTLibApp::CDTLibApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDTLibApp object

CDTLibApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CDTLibApp initialization
/////////////////////////////////////////////////////////////////////////////
//
BOOL CDTLibApp::InitInstance()
{
char strTmpDir[256];

	memset(strTmpDir,NULL,256);
	if (!AfxSocketInit())
	{
		TRACE0(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	m_TotalPrograms=0;
	m_TotalGroups=0;
	m_AppAdminFlag=FALSE;
	m_strPAS.Empty();

	CString m_csWindowsDir;

	::GetWindowsDirectory(m_csWindowsDir.GetBuffer(_MAX_PATH),_MAX_PATH);
	m_csWindowsDir.ReleaseBuffer();

	m_csWindowsDir = m_csWindowsDir+_T("\\ntcss.ini");

	GetPrivateProfileString(_T("NTCSS"),_T("NTCSS_ROOT_DIR"),_T("c:\\ntcss2"),
		strTmpDir,256,m_csWindowsDir);
	m_NtcssDir=strTmpDir;
	m_NtcssBinDir=m_NtcssDir+_T("\\bin");
	m_NtcssHelpDir=m_NtcssDir+_T("\\help");
	m_NtcssResourceDir=m_NtcssDir+_T("\\resource");

	memset(strTmpDir,NULL,256);
	CString strIgnoreNT;
	GetPrivateProfileString(_T("NTCSS"),_T("NTCSS_NT_ADMIN"),_T("YES"),
		strTmpDir,256,m_csWindowsDir);

	strIgnoreNT=strTmpDir;
	
	m_bIgnoreNT = false;

	//if (strIgnoreNT == _T("NO"))
	if(!strIgnoreNT.CompareNoCase(_T("NO")))
		m_bIgnoreNT = true;

	TRACE1(_T("Ignore NT FLAG is -> %s\n"),m_bIgnoreNT ? _T("TRUE") : _T("FALSE"));

//	NtcssSpoolDir=GetProfileString(_T("NTCSS"),_T("NTCSS_SPOOL_DIR"),_T("c:\\ntcss2\\spool"));

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////
// change the users password in NT, NTCSS and Unix
/////////////////////////////////////////////////////////////////////////
//
__declspec(dllexport) BOOL NtcssChangePassword(char *strUserName,char *strOldPwd,
											   char *strNewPwd)
{
BOOL CPB;  
char old_crypt_password[SIZE_PASSWORD * 2];
char new_crypt_password[SIZE_PASSWORD * 2];
char crypt_key[SIZE_PASSWORD * 2 + 1];
CmsgCHANGEPASSWORD cChPWD;
BOOL bRetCode,b;
int nRetCode;
CHAR strDomainName[MAX_PATH];
char strErrMsg[81];

	CWaitCursor cr;

	// if no LRS, just fake it
	if (theApp.m_stLRS == NULL)
		return FALSE;

	// init some string vars
	bRetCode=TRUE;
    memset(old_crypt_password,NULL,SIZE_PASSWORD*2);
    memset(new_crypt_password,NULL,SIZE_PASSWORD*2);
    memset(crypt_key,NULL,SIZE_PASSWORD*2+1);
	memset(strErrMsg,NULL,81);

    // Copy the data into the message, crypting the passwords  
	sprintf(crypt_key,"%s%s",strUserName,strUserName);

    CPB=NtcssEncrypt(crypt_key,strOldPwd,old_crypt_password);
    CPB=NtcssEncrypt(crypt_key,strNewPwd,new_crypt_password);
	old_crypt_password[strlen(old_crypt_password)]=':';
	new_crypt_password[strlen(new_crypt_password)]=':';

//	try {
	    //another bug here... if username is 8 the password change fails
		nRetCode=theApp.DoChangePassword((LPCSTR)strUserName, old_crypt_password, 
			new_crypt_password);

       
		if (nRetCode < 0) {
			::MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(_T("Unix Password Change Failed"),MB_ICONEXCLAMATION);
			return FALSE;
		}


		if(theApp.m_bIgnoreNT == false)
		{
			b=theApp.IsDomainLocal(strDomainName);
			if (b)
				CPB=NtcssChangeNTPassword(strUserName,strOldPwd,strNewPwd,0,0); 
			else
				CPB=NtcssChangeNTPassword(strUserName,strOldPwd,strNewPwd,0,1); 
			
	
			
			if (CPB == 1) {  // try and change Unix back to the old pwd on failure
				//if (CPB == 1 && theApp.m_bIgnoreNT == false) {  // try and change Unix back to the old pwd on failure
				TRACE("DOING THE PASSWORD RESET\n");
				
				::MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(_T("NT Password Change Failed... Rolling Back Unix Password"),MB_ICONEXCLAMATION);
				nRetCode=theApp.DoChangePassword((LPCSTR)strUserName, 
					new_crypt_password, old_crypt_password);
				if (nRetCode < 0)
					strcpy(theApp.m_strErrMsg,"Unable To Change Unix Password Back To Original!");
				else
					strcpy(theApp.m_strErrMsg,"Unable To Change Window Password!");
				return FALSE;
				
				//throw (_T("Unable to Change NT Password"));
				
			}
		}
	// release the buffers after they are used
	return bRetCode;
}


////////////////////////////////////////////////////////////////////////
// do the hard work of sending the message here
////////////////////////////////////////////////////////////////////////
//

int CDTLibApp::DoChangePassword(const char *strUserName, char *strCryptOldPwd,
								char *strCryptNewPwd)
{
int nRetCode;
int i;
CTimeoutSocket *cDTSock;
char strMessage[PWMSGSIZE+1];
char strResponseBuffer[SIZE_MESSAGE_TYPE+80];
char strMasterName[SIZE_HOST_NAME+1];
	

	memset(strMasterName,NULL,SIZE_HOST_NAME+1);

	TRACE("Getting master name\n");
	NtcssGetMasterName(strMasterName);

	TRACE("Getting master name, done <%s>\n",strMasterName);

	nRetCode=0;
    memset(strMessage,NULL,PWMSGSIZE+1);

	cDTSock=new CTimeoutSocket();

	cDTSock->Create();
	if (cDTSock->Connect(strMasterName,NTCSS_PORT_DBPORT) == FALSE) {
		delete cDTSock;
		strcpy(m_strErrMsg,"Unable To Connect To Auth Server");
		return -1;
	}

	memcpy(strMessage,"CHANGEUSERPASSWD",strlen("CHANGEUSERPASSWD"));
	memset(strMessage+SIZE_MESSAGE_TYPE,' ',SIZE_LOGIN_NAME);
	memcpy(strMessage+SIZE_MESSAGE_TYPE,strUserName,strlen(strUserName));
	memset(strMessage+SIZE_MESSAGE_TYPE+SIZE_LOGIN_NAME,' ',SIZE_PASSWORD*2);
	memcpy(strMessage+SIZE_MESSAGE_TYPE+SIZE_LOGIN_NAME,strCryptOldPwd,
			strlen(strCryptOldPwd));
	memset(strMessage+SIZE_MESSAGE_TYPE+SIZE_LOGIN_NAME+SIZE_PASSWORD*2,
			' ',SIZE_PASSWORD*2);
	memcpy(strMessage+SIZE_MESSAGE_TYPE+SIZE_LOGIN_NAME+SIZE_PASSWORD*2,
			strCryptNewPwd,strlen(strCryptNewPwd));


	TRACE("Sending message <%s>\n",strMessage+20);

	i=cDTSock->Send(strMessage,PWMSGSIZE+1,0);
	if (i != PWMSGSIZE+1) {
		delete cDTSock;
		strcpy(m_strErrMsg,"Unable To Send Change Password Message!");
		return -1;
	}
	TRACE("Sending message,done, Rxing mesg\n");

	memset(strResponseBuffer,NULL,SIZE_MESSAGE_TYPE+80);
	i=ReadMessage(cDTSock,strResponseBuffer,SIZE_MESSAGE_TYPE+80);
	if (i <= 0) {
		delete cDTSock;
		strcpy(m_strErrMsg,"Unable To Receive Change Password Response");
		return -1;
	}

	TRACE("Got Message, <%s>\n",strResponseBuffer);
	TRACE("Got Message, <%s>\n",strResponseBuffer+20);

	if (strcmp(strResponseBuffer,"GOODPASSWDCHANGE")) {
		delete cDTSock;
		strcpy(m_strErrMsg,strResponseBuffer+25);
		return UNABLE_TO_CHANGE_PWD;
	}

	delete cDTSock;

	TRACE("All Done\n");

	return 0;

}

__declspec(dllexport) int GetErrorString(char *strErrMsg)
{
	strcpy(strErrMsg,theApp.m_strErrMsg);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// Log the user in with the given name and password. The PAS is sent
// and the data is extracted from the ntcss.svr file or the provided
// default PAS
/////////////////////////////////////////////////////////////////////////
__declspec(dllexport) int ValidateUser(const char *strUserName,const char *strPassword,
											const char *strDefaultPAS, char *strPASName, 
											BOOL &bAppAdmin)
{
char myToken[SIZE_PASSWORD+1];
int i;
char strPAS[MAX_PATH];
CTimeoutSocket *cDTSock;
int nSock;
BOOL b;
char strMessageBuf[SIZE_MESSAGE_TYPE+1];
char strUsernamePassword[SIZE_LOGIN_NAME+SIZE_PASSWORD*2+SIZE_WORKING_DIR+1];
char strCryptPassword[SIZE_PASSWORD*2+1];
char myBigBuffer[80000];
char myDomain[MAX_PATH];
char unLockedApps[4096];

CWaitCursor cr;


	memset(unLockedApps,NULL,4096);
	memset(theApp.m_UnlockedApps,NULL,SIZE_UNLOCKED_APPS);	
	memset(theApp.m_strErrMsg,NULL,100);	


	// clear up some things
	theApp.m_TotalPrograms=0;
	theApp.m_TotalGroups=0;
	theApp.m_strPAS.Empty();
	memset(theApp.m_UnlockedApps,NULL,SIZE_UNLOCKED_APPS);
	theApp.m_AppAdminFlag=FALSE;

	CString pas=theApp.getpas(strUserName,strDefaultPAS);
	if(pas.IsEmpty()) {

		// Get the PAS...make the call
		memset(strPAS,NULL,MAX_PATH);
		i=theApp.GetPAS(strPAS,strUserName,strDefaultPAS);
		if (i < 0) {
			if (strDefaultPAS == NULL || strlen(strDefaultPAS) == 0)
				throw (_T("Cannot Determine User's Auth Server"));
			else
				strcpy(strPAS,strDefaultPAS);
		}
	}
	else
		sprintf(strPAS,_T("%s"),pas);

	TRACE1("Got the AuthServer... connecting to -> %s\n",strPAS);
	TRACE1("Default Server is ... connecting to -> %s\n",strDefaultPAS);

	cDTSock=new CTimeoutSocket();
	cDTSock->Create();
	if (cDTSock->Connect(strPAS,NTCSS_PORT_DSKPORT) == FALSE) {

		TRACE2("Connect to <%s> FAILED!... trying <%s> \n",strPAS,strDefaultPAS);

		if (cDTSock->Connect(strDefaultPAS,NTCSS_PORT_DSKPORT) == FALSE) {
			DWORD temp=::GetLastError();
			delete cDTSock;
			TRACE1(_T("Connected Failed GetLastError Returned-> %u\n"),temp);
			throw (_T("Unable To Contact User's Auth Server"));
		} else
			strcpy(strPAS,strDefaultPAS);
	}

	nSock=cDTSock->m_hSocket;

	memset(strMessageBuf,NULL,SIZE_MESSAGE_TYPE+1);
	memcpy(strMessageBuf,"GETUSERINFO",strlen("GETUSERINFO"));

	i=cDTSock->Send(strMessageBuf,SIZE_MESSAGE_TYPE+1,0);
	if (i != SIZE_MESSAGE_TYPE+1) {
		delete cDTSock;
		throw (_T("Trouble Sending Login Request To Auth Server"));
	}

	memset(myBigBuffer,NULL,SIZE_PASSWORD+9);
	memset(myToken,NULL,SIZE_PASSWORD+1);

	i=theApp.ReadMessage(cDTSock,myBigBuffer,SIZE_MESSAGE_TYPE+9);
	if (i <= 0) {
		delete cDTSock;
		throw (_T("Trouble Reading Intial Login Reply From Auth Server"));
	}

	memcpy(myToken,&myBigBuffer[SIZE_MESSAGE_TYPE-1],8);
	TRACE("token - <%s>\n",myToken);

	memset(strCryptPassword,NULL,SIZE_PASSWORD*2);

	if (strPassword && strlen(strPassword) > 0)
		b=NtcssEncrypt(myToken,strPassword,strCryptPassword);
	else
		b=NtcssEncrypt(myToken,NTCSS_AUTO_PWD,strCryptPassword);
	strCryptPassword[strlen(strCryptPassword)]=':';

	TRACE("crypt pwd - <%s>\n",strCryptPassword);

	memset(strUsernamePassword,' ',SIZE_LOGIN_NAME+SIZE_PASSWORD*2+SIZE_WORKING_DIR);
	strUsernamePassword[SIZE_LOGIN_NAME+SIZE_PASSWORD*2+SIZE_WORKING_DIR+1]='\0';
	strUsernamePassword[SIZE_LOGIN_NAME+SIZE_PASSWORD*2+SIZE_WORKING_DIR]='\0';
	GetEnvironmentVariable( _T("USERDOMAIN"),myDomain,MAX_PATH); 

	memcpy(strUsernamePassword,strUserName,strlen(strUserName));
	TRACE("msg = <%s>\n",strUsernamePassword);

	memcpy(&strUsernamePassword[SIZE_LOGIN_NAME],strCryptPassword,strlen(strCryptPassword));
	TRACE("msg- <%s>\n",strUsernamePassword);

	memcpy(&strUsernamePassword[SIZE_LOGIN_NAME+SIZE_PASSWORD*2],
				myDomain,strlen(myDomain));
	TRACE("msg- <%s>\n",strUsernamePassword);

	i=cDTSock->Send(strUsernamePassword,strlen(strUsernamePassword)+1,0);
	if (i != (int)strlen(strUsernamePassword)+1) {
		delete cDTSock;
		throw (_T("Trouble Reading Login Reply From Auth Server"));
	}

	memset(myBigBuffer,NULL,80000);
	i=theApp.ReadMessage(cDTSock,myBigBuffer,80000);
	if (i <= 0) {
		delete cDTSock;
		throw (_T("Error Reading Login Reply Data From Auth Server"));
	}

	TRACE("mt=<%.20s>\n",myBigBuffer);
	
	//TRACE("Data=<%.*s>\n",i-30,myBigBuffer+20);

	//TRACE("%s.\n",myBigBuffer+20);

	if (!strcmp(myBigBuffer, "VALIDLOGIN")) {
		if (theApp.FillLRS(myBigBuffer+20))
			throw (_T("Invalid User Profile, Check progrp.ini file"));
	} else {
		delete cDTSock;
		throw (_T(myBigBuffer+25));
	}


	
	strcpy(strPASName,strPAS);
	bAppAdmin=theApp.m_AppAdminFlag;

	delete cDTSock;

	TRACE("Length of Unlocked Apps = %d\n",strlen(theApp.m_UnlockedApps));
	return strlen(theApp.m_UnlockedApps);
}

///////////////////////////////////////////////////////////
// function that returns the unlocked apps list
///////////////////////////////////////////////////////////
//
__declspec(__DECL) int GetUnlockedAppList(char *strUnlockedApps)
{
	if (strlen(theApp.m_UnlockedApps) != 0) {
		strcpy(strUnlockedApps,theApp.m_UnlockedApps);
	}

	return(0);
}

///////////////////////////////////////////////////////////
// function that only returns the LRS pointer
///////////////////////////////////////////////////////////
__declspec(dllexport) LoginReplyStruct* __stdcall GetLRS(void)
{
CString tstr;

	tstr.Format("DTLIB ptr = %d\n",theApp.m_stLRS);
	TRACE(tstr);

	return(theApp.m_stLRS);
}


//////////////////////////////////////////////////////
// expand the path statements on the way to 
// extracting message data
//////////////////////////////////////////////////////
int CDTLibApp::ExpandPath(char *strPath, int nPathLen)
{
CString *tempbuf;


	TRACE("BEFORE EXPAND = <%s>\n",strPath);
	
	tempbuf=new CString(strPath);

	memset(strPath,NULL,nPathLen);

	tempbuf->Replace(NTCSSDIRSTRING,m_NtcssDir);
	tempbuf->Replace(NTCSSBINDIRSTRING,m_NtcssBinDir);
	tempbuf->Replace(NTCSSRESDIRSTRING,m_NtcssResourceDir);
	tempbuf->Replace(NTCSSHELPDIRSTRING,m_NtcssHelpDir);

	strcpy(strPath,tempbuf->GetBuffer(tempbuf->GetLength()));
	tempbuf->ReleaseBuffer();

	TRACE("After EXPAND = <%s>\n",strPath);

	delete tempbuf;

	return 0;
}

///////////////////////////////////////////////////////
// Function to return the number of unlocked apps
///////////////////////////////////////////////////////
__declspec(dllexport) int GetUnlockedAppCount(void)
{
	return strlen(theApp.m_UnlockedApps);
;
}

///////////////////////////////////////////////////////////
// Function that sends a logoff message to the
// desktop server
///////////////////////////////////////////////////////////
__declspec(dllexport) int LogoutUser(void)
{
CmsgLogout cLO;

	TRACE("Logging out, apps = <%s>\n",theApp.m_UnlockedApps);
	cLO.Load(theApp.m_UnlockedApps);
	cLO.DoItNow();	// fire and forget
	TRACE("LogOut Message is complete\n");

	memset(theApp.m_UnlockedApps,NULL,SIZE_UNLOCKED_APPS);	
	memset(theApp.m_strErrMsg,NULL,100);
	
    TRACE("Leaving LogoutUser\n");

	return 0;
}

//////////////////////////////////////////////////////////////
// Run remote interactive program (probably x)
//////////////////////////////////////////////////////////////
__declspec(dllexport) int RunRemote(char *strCmdLineArgs,char *strAppName,
									char *strHostName/*=NULL*/)
{
CmsgRunRemote cRR;
struct GroupStruct *grPtr,*grFound;
char strPID[100];
BOOL b;
char strHN[512];

CWaitCursor cr;

	// if no LRS, just fake it
	if (theApp.m_stLRS == NULL)
		return -1;

	// find hostname by app
	memset(strPID,NULL,100);
	memset(strHN,NULL,512);
	grFound=NULL;

	//
	// if a host is specified, use it, otherwise look it up
	grPtr=theApp.m_stLRS->m_stFirstGroup;

		// for each group
	while (grPtr != NULL) {
		if (!strcmp(strAppName,grPtr->m_strGroupTitle)) {
			grFound=grPtr;
			break;
		}
		grPtr=grPtr->m_stNextGroup;
	}

	if (grFound != NULL) {
		if (strHostName == NULL) 
			strcpy(strHN,grFound->m_strHostName);
		else
			strcpy(strHN,strHostName);

		TRACE("Found group host  <%s>\n",grFound->m_strHostName);

		b=cRR.Load(strAppName,strCmdLineArgs,theApp.m_stLRS->m_stUserInfo.m_strLoginName,
			strHN);
		if (cRR.DoItNow() == FALSE)
			throw (_T("Unable To Run Remote Process"));
		cRR.UnLoadPID(strPID);
		TRACE("PID = <%s>\n",strPID);
	} else
		throw (_T("Invalid Application Name"));

	return 0;
}

/////////////////////////////////////////////////////////
// This function frees the allocated data that the LRS
// is occupying
/////////////////////////////////////////////////////////

__declspec(dllexport) BOOL ClearLRS(void)
{
struct GroupStruct *grPtr, *last_grPtr;
struct ProgramStruct *prPtr, *last_prPtr;

	theApp.m_TotalPrograms=0;
	theApp.m_TotalGroups=0;
	theApp.m_strPAS.Empty();
	theApp.m_AppAdminFlag=FALSE;

	// if no LRS, just fake it
	if (theApp.m_stLRS == NULL)
		return TRUE;

	// unpack each group
	if (theApp.m_stLRS->m_nNumberOfGroups > 0) {
		grPtr=theApp.m_stLRS->m_stFirstGroup;
		last_grPtr=NULL;

		// for each group
		while (grPtr != NULL) {
			prPtr=grPtr->m_stFirstProgram;
			last_prPtr=NULL;

			// for each program
			while (prPtr != NULL) {
				last_prPtr=prPtr;
				prPtr=prPtr->m_stNextProgram;
				delete last_prPtr;
			}
			last_grPtr=grPtr;
			grPtr=grPtr->m_stNextGroup;
			delete last_grPtr;
		}
	}
	delete theApp.m_stLRS;
	theApp.m_stLRS=NULL;

	return TRUE;
}

__declspec(dllexport) BOOL NTCSS_FindServer(void)
{

	return TRUE;
}



//////////////////////////////////////////////////////////
// Determine from the computer and userdomain if this
// WS is logged into a domain
//////////////////////////////////////////////////////////
BOOL CDTLibApp::IsDomainLocal(char *strDCName)
{   //logic was reversed here - false if local -  check GetDomainFlag()

	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwComputerNameLength = sizeof (szComputerName);
	TCHAR szDomainName[MAX_PATH + 1];
	::GetComputerName(szComputerName, &dwComputerNameLength);

	return ::GetEnvironmentVariable(_T("USERDOMAIN"),szDomainName,MAX_PATH + 1) &&
	lstrcmpi(szComputerName, szDomainName) == 0;

}


////////////////////////////////////////////////////////////////////////////
// GetPAS - Get Auth Server
//
//  Get the PAS from the users homedir. If there is no file, just return FALSE
//
////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) int GetUserPAS(char *strPAS)
{

//	int i=theApp.GetPAS(strPAS,strUserName,strDefaultPAS);
	strcpy(strPAS,(LPCSTR)theApp.m_strPAS);
	TRACE("%s\n",strPAS);
	return theApp.m_strPAS.GetLength();

}


////////////////////////////////////////////////////////
// Get the users PAS. If home file not available,
// set the PAS to the provided default PAS.  This 
// should probably be from the INI file.
////////////////////////////////////////////////////////
int  CDTLibApp::GetPAS(char *strPAS,
					  const char *strUserName, const char *strDefaultPAS)
{
LPUSER_INFO_11	  pusr;  
char homedir[500];
char tmppas[500];
TCHAR strDomainName[MAX_PATH]; //TODO find out what the MAX domain size is
LPWSTR szServerName=NULL;
BOOL b;
int nRet;
CFile *userHomeDir;
USES_CONVERSION;
CString tstr;


	memset(tmppas,NULL,500);
	memset(homedir,NULL,500);

	b=IsDomainLocal(strDomainName);

	if (b==TRUE) {
		nRet = NetUserGetInfo(NULL,T2W(strUserName),11,(LPBYTE *)&pusr);  
	} else {
 


		GetEnvironmentVariable( _T("USERDOMAIN"),strDomainName,MAX_PATH); 

		nRet=NetGetDCName(NULL,T2W(strDomainName),(LPBYTE *)&szServerName);

		nRet = NetUserGetInfo(szServerName,T2W(strUserName),
			                  11,(LPBYTE *)&pusr);  
	}


	if (nRet != NERR_Success) {
		strcpy(strPAS,strDefaultPAS);
		return(0);
	}

//AfxMessageBox("Trying file");
	try {
		strcpy(homedir,W2T(pusr->usri11_home_dir));
		strcat(homedir,"\\ntcss.svr");
//tstr.Format("homedir=<%s>",homedir);
//AfxMessageBox(tstr);

		userHomeDir=new CFile(homedir,CFile::modeRead);
	} catch (...) {
//AfxMessageBox("Caught file error");

		strcpy(strPAS,strDefaultPAS);
		TRACE("%s\n",strPAS);
		return (0);
	}

	TRACE("%s\n",homedir);
	try {
//AfxMessageBox("Reading file");

		userHomeDir->Read(tmppas,SIZE_HOST_NAME+1);
//AfxMessageBox(tmppas);

		userHomeDir->Close();
//		if (tmppas[strlen(tmppas)-1] == '\n')
			tmppas[strlen(tmppas)-2]='\0';
		strcpy(strPAS,tmppas);
//tstr.Format("strpas = <%s>",strPAS);
//AfxMessageBox(tstr);

		delete userHomeDir;
//AfxMessageBox("Cfile deleted");

		TRACE("%s\n",strPAS);

	} catch (CFileException e) {
		userHomeDir->Close();
		delete userHomeDir;
		return(0);
	}
	return 0;
}


//////////////////////////////////////////////////////////
// Simple routine to help connect a CSocket to a 
// IP and port.
//////////////////////////////////////////////////////////
int CDTLibApp::ConnectServer(CTimeoutSocket *cps, int port,char *strHostname)
{
int sock;
struct sockaddr_in socketAddr;
struct hostent *lpHe;
char tmphost[NTCSS_SIZE_HOSTNAME];
int retcode;

TRACE("In connectserver\n");
TRACE("%s\n",strHostname);
	 
	memset(tmphost,NULL,NTCSS_SIZE_HOSTNAME);

    strcpy(tmphost,strHostname);

TRACE("Getting hostbyname\n");
TRACE("%s\n",tmphost);
	lpHe=gethostbyname(tmphost);
	 
	if (!lpHe) {
    	sock=0;
TRACE("No HE\n");
    	return -1;
    }          

    socketAddr.sin_port=htons(port);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,(char far *)lpHe->h_addr_list[0],lpHe->h_length);

	cps->Create(0,SOCK_STREAM,(LPCTSTR)NULL);

	sock=cps->m_hSocket;

TRACE("Connecting\n");
     
    retcode=cps->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (cps->GetLastError() != WSAEWOULDBLOCK) {
  		  	cps->Close();
 		   	sock=0;
TRACE("Not Connected\n");

    		return -1;
    	}
    }
TRACE("Connected\n");

	return sock;
      
}

////////////////////////////////////////////////////////////
//  Read an NTCSS message including the MT
////////////////////////////////////////////////////////////
int CDTLibApp::ReadMessage(CTimeoutSocket *cps, char *lrsBuffer,int maxMessage)
{
int mycnt;
int i;

	memset(lrsBuffer,'\0',maxMessage);
	mycnt=0;

	while (mycnt < maxMessage)  {
		try {
			i=cps->Receive(&lrsBuffer[mycnt],1,0);
			if ((lrsBuffer[mycnt] == '\0' && mycnt > SIZE_MESSAGE_TYPE) || i<=0) 
				return mycnt;
			mycnt++;
		} catch (...) {
			return mycnt;
		}
	}

	return mycnt;
}


///////////////////////////////////////////////////////
// Fill and allocate LRS from the given buffer
///////////////////////////////////////////////////////
int CDTLibApp::FillLRS(char *lrsBuffer)
{
int i,j;
char *bptr;
CString tstr;
CString tstr1;

	// keep these handy	
	m_TotalPrograms=0;
	m_TotalGroups=0;
	memset(theApp.m_UnlockedApps,NULL,SIZE_UNLOCKED_APPS);
	m_stLRS=(LoginReplyStruct *)malloc(sizeof(LoginReplyStruct));

	// unpack user info
	bptr=unbundleData(lrsBuffer,"CICCCCIDCII",
		m_stLRS->m_stUserInfo.m_strToken,
		&m_stLRS->m_stUserInfo.m_nUnixID,
		m_stLRS->m_stUserInfo.m_strLoginName,
		m_stLRS->m_stUserInfo.m_strRealName,
		m_stLRS->m_stUserInfo.m_strSSN,
		m_stLRS->m_stUserInfo.m_strPhoneNumber,
		&m_stLRS->m_stUserInfo.m_nSecurityClass,
		m_stLRS->m_stUserInfo.m_strPasswordChangeTime,
		m_stLRS->m_stUserInfo.m_strShardPassword,
		&m_stLRS->m_stUserInfo.m_nUserRole,
		&m_stLRS->m_nNumberOfGroups);

	TRACE("%s\n",m_stLRS->m_stUserInfo.m_strLoginName);
	TRACE("Numb groups=%d\n",m_stLRS->m_nNumberOfGroups);
	
	// unpack # of groups
	m_TotalGroups=m_stLRS->m_nNumberOfGroups;

	// unpack each group
	if (m_stLRS->m_nNumberOfGroups > 0) {
		GroupStruct *grPtr;
		m_stLRS->m_stFirstGroup=(GroupStruct *)malloc(sizeof(GroupStruct));
		m_stLRS->m_stFirstGroup->m_stNextGroup=NULL;

		grPtr=m_stLRS->m_stFirstGroup;

		for (i=0;i<m_stLRS->m_nNumberOfGroups;i++) {
			
			if (*bptr == '\0') {
				ClearLRS();
				return 1;
			}

			bptr=unbundleData(bptr,"CCIICCCICDCCI",
				grPtr->m_strGroupTitle,
				grPtr->m_strHostName,
				&grPtr->m_nHostType,
				&grPtr->m_nAppRole,
				grPtr->m_strAppData,
				grPtr->m_strAppPassword,
				grPtr->m_strIconFile,
				&grPtr->m_nIconIndex,
				grPtr->m_strVersionNumber,
				grPtr->m_strReleaseDate,
				grPtr->m_strClientLocation,
				grPtr->m_strServerLocation,
				&grPtr->m_nNumberOfPrograms);

				ExpandPath(grPtr->m_strIconFile,SIZE_ICON_FILE);
				ExpandPath(grPtr->m_strClientLocation,SIZE_CLIENT_LOCATION);
				ExpandPath(grPtr->m_strServerLocation,SIZE_SERVER_LOCATION);

			    if (grPtr->m_nAppRole%8)
					m_AppAdminFlag=TRUE;

				TRACE("%s\n",grPtr->m_strGroupTitle);
				ProgramStruct *prPtr;
				grPtr->m_stFirstProgram=(ProgramStruct *)malloc(sizeof(ProgramStruct));

				grPtr->m_stFirstProgram->m_stNextProgram=NULL;
				prPtr=grPtr->m_stFirstProgram;

				m_TotalPrograms=m_TotalPrograms+grPtr->m_nNumberOfPrograms;

				for (j=0;j<grPtr->m_nNumberOfPrograms;j++) {
					bptr=unbundleData(bptr,"CCCICIIC",
						prPtr->m_strProgramTitle,
						prPtr->m_strProgramFile,
						prPtr->m_strIconFile,
						&prPtr->m_nIconIndex,
						prPtr->m_strCmdLineArgs,
						&prPtr->m_nSecurityClass,
						&prPtr->m_nFlags,
						prPtr->m_strWorkingDir);


					ExpandPath(prPtr->m_strProgramFile,SIZE_PROGRAM_FILE);
					ExpandPath(prPtr->m_strIconFile,SIZE_ICON_FILE);
					ExpandPath(prPtr->m_strWorkingDir,SIZE_WORKING_DIR);

					TRACE("%s\n",prPtr->m_strProgramTitle);

					if (j<grPtr->m_nNumberOfPrograms-1) {
							prPtr->m_stNextProgram=new ProgramStruct;
							prPtr=prPtr->m_stNextProgram;
							prPtr->m_stNextProgram=NULL;
					}

				}
				if (i<m_stLRS->m_nNumberOfGroups-1) {
						grPtr->m_stNextGroup=new GroupStruct;
						grPtr=grPtr->m_stNextGroup;
						grPtr->m_stNextGroup=NULL;
				}

		}
	}

	if (*bptr == '\0') {
				ClearLRS();
				return 1;
	}
	// now process the unlocked app list
	bptr=unbundleData(bptr,"C",	m_UnlockedApps);

	TRACE("Ulocked Len = %d\n",strlen(m_UnlockedApps));

	m_stLRS->m_nUserValidatedFlag=1;
	TRACE("All done with LRS\n");

	return 0;
}


/*
int CDTLibApp::bundleData(char *str, char *key,...)
{
	va_list ap;
	char *k, *char_data;
	char format[20], fmt_str[10];
	int int_data, n, leng = 0;

	k = key;
	
	va_start(ap, key);
	while (*k) {
 // Bundle character string.
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				n = strlen(char_data);
				wsprintf(format, "%5.5d", n);
				strcat(str, format);
				strcat(str, char_data);
				leng += n + 5;
				break;

// * Bundle integer string.
 
			case 'I':
				int_data = va_arg(ap, int);
				wsprintf(format, "%d", int_data);
				n = strlen(format);
				wsprintf(fmt_str, "%%1d%%%d.%dd", n, n);
				wsprintf(format, fmt_str, n, int_data);
				strcat(str, format);
				leng += n + 1;
				break;

// * Bundle date/time string.
 
			case 'D':
				char_data = va_arg(ap, char *);
				strcat(str, char_data);
				leng += 11;
				break;

			default:
				return(-1);
				break;
		}
		k++;
	}
	va_end(ap);

	return(leng);
}

*/

char *CDTLibApp::unbundleData(char *str, char *key,... )
{
	va_list ap;
	char *s, *k, *char_data;
	char format[20];
	int *int_data, i, n;

	s = str;
	k = key;

	va_start(ap,key);
	while (*k) {
/*
 * Unbundle character string.
 */
		memset(format,NULL,20);
		switch (*k) {
			case 'C':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 5; i++) {
					if (*s && isdigit((int)*s))
						format[i] = *s++;
					else
						return(NULL);
				}
				format[5] = 0;
				n = atoi(format);
				for (i = 0; i < n; i++) {
					if (*s)
						char_data[i] = *s++;
					else
						return(NULL);
				}
				char_data[n] = 0;
				break;
/*
 * Unbundle integer string.
 */
			case 'I':
				int_data = va_arg(ap, int *);
				if (*s)
					format[0] = *s++;
				else
					return(NULL);

				format[1] = 0;
				n = atoi(format);
				if (n > 20)
					return(NULL);

				for (i = 0; i < n; i++) {
					if (*s)
						format[i] = *s++;
					else
						return(NULL);
				}
				format[n] = 0;
				*int_data = atoi(format);
				break;
/*
 * Unbundle date/time string.
 */
			case 'D':
				char_data = va_arg(ap, char *);
				for (i = 0; i < 11; i++) {
					if (*s)
						char_data[i] = *s++;
					else
						return(NULL);
				}
				char_data[11] = 0;
				break;

			default:
				return(NULL);
				break;
		}
		k++;
	}
	va_end(ap);

	return(s);
}

CString CDTLibApp::getpas(const CString& UserName, const CString& strMaster)
//If they are using the Known Server Flag this guy returns a server period
//Else empty string & processing goes as usual
{
	CString m_csWindowsDir,strServer,strResponse,strUserName,strAuthServer;
	CStringEx m_csExServers;
	CTimeoutSocket* pSocket;
	CStringArray list;
	
	try
	{
		
		strUserName=UserName;
		
		::GetWindowsDirectory(m_csWindowsDir.GetBuffer(_MAX_PATH),_MAX_PATH);
		m_csWindowsDir.ReleaseBuffer();
		CIniFile ini(m_csWindowsDir+_T("\\ntcss.ini"));
		
		ini.ReadFile();
		CString temp=ini.GetValue(_T("NTCSS"),_T("QUERY_KNOWN_HOSTS"));
		TRACE1("QUERY_KNOWN_HOSTS is -> %s\n",temp);
		
		if(!temp.CompareNoCase(_T("YES")))
			m_csExServers=ini.GetValues(_T("KNOWN_HOSTS"));
		else
			return _T("");
		
		TRACE1("Using INI Server String -> %s\n",m_csExServers); 
		//now fire the message until we get authserver or fail & return master
		
		//add canidate servers to CStringArray to randomize access from 
		for(int i=0;;i++)
		{
			if((strServer=m_csExServers.GetField(_T(","),i))==_T(""))
				break;
			
			list.Add(strServer);
		}
		
		int ct=list.GetSize();

		while(1)
		{
			if(list.GetSize()==0)
				break;
			
			i=rand() % ct;
		
			strServer=list.GetAt(i);

			
			TRACE1(_T("DTLib.getpas()... Random # is <%d>\n"),i);
			
			try
			{
				
				pSocket=new CTimeoutSocket();
				pSocket->Create();
				pSocket->SetTimeoutLength(5); 
				
				TRACE2(_T("Trying to get <%s>'s AuthServer from %s\n"),
					       strUserName,strServer);
				
				if(pSocket->connect(strServer,NTCSS_PORT_DSKPORT,5) == FALSE)
				{
					TRACE1(_T("Connect Failed on %s!!!!\n"),strServer);
					delete pSocket;
					list.RemoveAt(i);
					ct--;
					continue;
				}
				
				TRACE1(_T("Past Connect on %s\n"),strServer);
				//Build the $&%&! message
				TCHAR buf[30]={_T('\0')};
				memcpy(buf,_T("GETAUTHSVR"),strlen(_T("GETAUTHSVR")));
				memset(buf+20,_T(' '),8);
				memcpy(buf+20,strUserName,strUserName.GetLength());
				memset(buf+28,_T('\0'),2); 
				
				pSocket->Send(buf,(20 + _tcslen(buf+20) + 1),0);
				if(pSocket->HadTimeout())
					throw(_T("Send timed out!!!"));
				
				TCHAR strResponseBuffer[37]={NULL};
				
				pSocket->Receive(strResponseBuffer,37,0);
				if(pSocket->HadTimeout())
					throw(_T("Receive timed out!!!"));
				
				strResponse=strResponseBuffer;
				strAuthServer=strResponseBuffer+20;
				
				if(strResponse==_T("GOODGETAUTHSVR"))
				{
					TRACE3(_T("User <%s>'s AuthServer is <%s>... Obtained from Server <%s>\n"),
						   strUserName,strAuthServer,strServer);
					pSocket->ShutDown(2);
					pSocket->Close();
					delete pSocket;
					return strAuthServer;
				}
			}
			
			
			catch(LPTSTR pStr)
			{
				TRACE1(_T("%s\n"),pStr);
				pStr=NULL; //lose the release warning
			}
			
			catch(...)
			{
				TRACE0(_T("Unknown execption caught in DTLIB.getpas() #1\n"));
			}
			
			pSocket->ShutDown(2);
			pSocket->Close();
			delete pSocket;
			
			list.RemoveAt(i);
			ct--;
			
		}
		
	}
	
	catch(...)
	{
		TRACE0(_T("Unknown execption caught in DTLIB.getpas() #2\n"));
	}
	
	TRACE("Couldn't get AuthServer returning Master-> %s \n",strMaster);
	return strMaster;
	
}
