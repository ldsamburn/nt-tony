/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock.h"
#include "encrypt.h"
#include "PL.h"
#include "StringEx.h"
#include "sys/types.h"
#include "sys/stat.h"

#include "atlbase.h"
#include "atlconv.h"
#include <ntsecapi.h>  // For LsaEnumerateTrustedDomains

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LOCK_NAME "NTCSS_PRELAUNCH_LOCK"

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

UINT the_thread(LPVOID lpParameter);
HANDLE g_hEvent = NULL; 

PSID	pLogonSid = NULL;
HWINSTA	hWinsta = NULL;
HDESK	hDesktop = NULL;

HANDLE Launch(LPTSTR lpszUser, LPTSTR lpszCommandLine,
			  LPTSTR lpszPassword,LPTSTR lpszAppname, LPTSTR lpszNtcssPID);

void Reap(LPCTSTR strID,LPCTSTR strHostname,HANDLE hJob);
BOOL allowDesktopAccess(HANDLE hToken);
BOOL disallowDesktopAccess();
BOOL allowDesktopAccess2(void);
BOOL setWinstaDesktopSecurity(HWINSTA hWinsta,HDESK hDesktop,PSID pLogonSid,
	                          BOOL bGrant,HANDLE hToken);
BOOL setSidOnAcl(PSID pSid,PACL pAclSource,PACL *pAclDestination,DWORD AccessMask,
				BOOL bAddSid,BOOL bFreeOldAcl);
BOOL getAndAllocateLogonSid(HANDLE hToken,PSID *pLogonSid);
void getlasterror(const CString& strInputString);

BOOL login(LPTSTR lpszUser, LPTSTR lpszDCP, PHANDLE phToken);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}
	else
	{


    //regular args->  cmdline,user,password,app,host,NTCSSPID argc==7
	//app_callback->  cmdline,user,password,app,host,exitstatus,statusstring=8

 
	TRACE(_T("In preLaunch with <%s> <%s> <%s> <%s> <%s> <%s>\n"),argv[1],argv[2],argv[3],
		     argv[4],argv[5], argv[6]);

	if(argc==8)
	{   TRACE("Launching PSP program\n");

	    //TODO: overload launch to take 
		HANDLE pspProg = Launch(argv[1],argv[2],argv[3],argv[4],argv[6]);
    
        if(::WaitForSingleObject(pspProg,INFINITE)==WAIT_OBJECT_0) //job ended
		{

			TRACE0(_T("PSP Ended\n"));			

			if(!disallowDesktopAccess())
               TRACE0("disallowDesktopAccess Failed!");
		}
		return 0;
	}


	//kick off keyboard listener thread
	 HANDLE hListener=(AfxBeginThread(the_thread,NULL))->m_hThread;

      g_hEvent = CreateEvent(NULL, TRUE, FALSE, "");

	
    TRACE(_T("Calling Regular Launch with <%s> <%s> <%s> \n"),argv[1],argv[2],argv[3],argv[4],argv[6]);

	HANDLE hJob = Launch(argv[1],argv[2],argv[3],argv[4],argv[6]);

	if(hJob==NULL)
	{
		TRACE0("Back From Launch... Failure you go home now!!\n");
		return 1;
	}

	TRACE0(_T("Back From Launch... Success waiting on Job!!\n"));
	while(1) //wait for either the Job to end or the kill message
	{
		if(::WaitForSingleObject(hJob,1)==WAIT_OBJECT_0) //job ended
		{

			TRACE0(_T("Job Ended killing thread\n"));

			::TerminateThread(hListener,0);
			::CloseHandle(hListener);

			TRACE0(_T("Thread is dead!!!\n"));

			TRACE0(_T("Reaping the Job\n"));
			Reap(argv[6],argv[5],hJob);
			TRACE0("Exiting Main Normally !!!\n");

			break;
		}
		
		if(::WaitForSingleObject(g_hEvent,1)==WAIT_OBJECT_0) //job killed
		{
			HANDLE ThreadHandle=::CreateRemoteThread(hJob,NULL,0,
				(LPTHREAD_START_ROUTINE)(GetProcAddress(GetModuleHandle
				(_T("KERNEL32.DLL")),_T("ExitProcess"))),
				NULL,0,NULL);
			
			if (ThreadHandle!=NULL)
				::WaitForSingleObject(ThreadHandle,INFINITE);

			TRACE0(_T("Reaping the Job...Killed by KillServerProc\n"));
			Reap(argv[6],argv[5],hJob);

			TRACE0("Exiting Main Killed by KillServerProc!!!\n");			
			
            break;
		} 
	}

    if(!disallowDesktopAccess())
       TRACE0("disallowDesktopAccess Failed!");

    _exit(0);

}
}

////////////////////////////////////////////////////////////
//

HANDLE Launch(LPTSTR lpszCommandLine, LPTSTR lpszUser, LPTSTR lpszPassword,
			  LPTSTR lpszAppname, LPTSTR lpszNtcssPID)

{
	HANDLE				hToken = NULL;
	BOOL				ok = FALSE;
	STARTUPINFO			startup_info;
	PROCESS_INFORMATION	proc_info;
    TCHAR longPath[_MAX_PATH]={NULL};
    TCHAR libPath[_MAX_PATH];
    TCHAR buffer[200];
    TCHAR path[200];
    DWORD ret;
    HANDLE retHandle = NULL;

	//Figure out where the actual path is so we can quote it to handle long filenames
	CString prog,temp;
	CStringEx prgPath(lpszCommandLine);
	struct _stat buf;


	for(int i=0;;i++)
	{
		if((temp=prgPath.GetField(_T(" "),i))==_T(""))
			break;

		if(prog.IsEmpty())
			prog=temp;
		else
			prog+=_T(" ") + temp;

		if(!_tstat( prog, &buf ))
			break;
	}

	temp.Format(_T("\"%s\" %s"),prog,prgPath.GetField(prog,1));


	/*
	**	Get the token so that we can impersonate the required user
	*/

	TRACE0(_T("Decrypting Password\n"));

	TCHAR lpszKey[75];
	TCHAR lpszDCP[35];


	_tcscpy(lpszKey,lpszUser);
	_tcscat(lpszKey,lpszUser);

   CEncrypt crypt;
   crypt.Decrypt(lpszKey,lpszPassword,lpszDCP); 

	try
	{      


	//if(!::LogonUser(lpszUser,_T("."),lpszDCP,LOGON32_LOGON_INTERACTIVE,
    //			     LOGON32_PROVIDER_DEFAULT,&hToken))
    if (login(lpszUser, lpszDCP, &hToken) == FALSE) {
        TRACE0("Logon failed");
		throw("LogonUser Failed"); 
    }
    else
        TRACE0("Logon successful");


    if(!allowDesktopAccess(hToken))
//    if(!allowDesktopAccess2())
		throw("allowDesktopAccess Failed");

	
	::ZeroMemory(&startup_info,sizeof(STARTUPINFO));

	startup_info.cb = sizeof(STARTUPINFO);

	startup_info.wShowWindow = SW_SHOW;	
	startup_info.dwFlags = STARTF_USESHOWWINDOW;  // NOTE: use of STARTF_USESTDHANDLES flag causes batch files not to execute

// 	startup_info.lpDesktop = (LPTSTR) "Winsta0\\Default";


	// if lpEnvironment Pointer to an environment block for the new process. 
	// If this parameter is NULL, the new process uses the environment of the
	// calling process... so we'll try it that way... TODO: test later
    TRACE1(_T("Setting CURRENT_NTCSS_PID -> %s\n"),lpszNtcssPID);

	::SetEnvironmentVariable(_T("CURRENT_NTCSS_PID"),lpszNtcssPID);
	::SetEnvironmentVariable(_T("CURRENT_NTCSS_GROUP"),lpszAppname);

    
    // Get the path to the ntcsss/libs directory
    //memset(libPath, '\0', _MAX_PATH);
    ::GetModuleFileName(NULL, longPath, _MAX_PATH);
    ::GetShortPathName(longPath, libPath, _MAX_PATH); 
    _tcscat(libPath, _T("\\..\\..\\libs"));

    // Get the current path and append to the end of it the library path
    ret = ::GetEnvironmentVariable(_T("PATH"), buffer, 200); // environment variable name
    _stprintf(path, _T("%s;%s"), buffer, libPath);
    ::SetEnvironmentVariable(_T("PATH"),path);

 
	if(!CreateProcessAsUser(
					hToken,
					NULL,
					(LPTSTR)(LPCTSTR)temp,
					NULL,
					NULL,
					TRUE,
                    CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
					NULL,
					NULL,
					&startup_info,
					&proc_info))

		throw("CreateProcessAsUser Failed");

	//write back the PID to JAVA using stdout
	cout << proc_info.dwProcessId << endl;

    retHandle = proc_info.hProcess;    

	}


	catch(LPTSTR pStr)
	{
		getlasterror(pStr);		
	}


	if (hToken != NULL)
		CloseHandle(hToken);

    return retHandle;	
}


////////////////////////////////////
//    Notifies Command server the Job has terminated

void Reap(LPCTSTR strID,LPCTSTR strHostname,HANDLE hJob)
{
	
	SOCKET ServerSock = INVALID_SOCKET; // Socket bound to the server
	SOCKADDR_IN destination_sin;        // Server socket address
	PHOSTENT phostent = NULL;           // Points to the HOSTENT structure
	// of the server
	WSADATA WSAData;                    // Contains details of the 
	// Winsocket implementation
	
	
	CString message;
	DWORD dwExitCode = 0;
	
	
	TRACE0("IN REAPER\n");
	
	// Initialize Winsocket. 
	if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0) 
	{
		TRACE1(_T("WSAStartup failed. Error: %d\n"),WSAGetLastError());
		return;
	}
	
	// Create a TCP/IP socket that is bound to the server.
	if ((ServerSock = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		TRACE1(_T("Allocating socket failed. Error: %d\n"),WSAGetLastError());
		return;
	}
	
	// Fill out the server socket's address information.
	destination_sin.sin_family = AF_INET;
	
	// Retrieve the host information corresponding to the host name.
	if ((phostent = gethostbyname (strHostname)) == NULL) 
	{
		closesocket(ServerSock);
		TRACE1(_T("Unable to get the host name. Error: %d"),WSAGetLastError());
		return;
	}
	
	// Assign the socket IP address.
	::CopyMemory((LPTSTR)&(destination_sin.sin_addr), 
		phostent->h_addr, 
		phostent->h_length);
	
	
	// Convert to network ordering.
	destination_sin.sin_port = htons (9152);    
	
	TRACE0("Connecting\n");
	
	// Establish a connection to the server socket.
	if (connect (ServerSock, 
		(PSOCKADDR) &destination_sin, 
		sizeof (destination_sin)) == SOCKET_ERROR) 
	{
		closesocket (ServerSock);
		TRACE1(_T("Connecting to the server failed. Error: %d\n"),WSAGetLastError());
		return;
	}
	
	//send exitcode back too
	::GetExitCodeProcess(hJob,&dwExitCode);
	::CloseHandle(hJob);
	CString strES;

	strES.Format("%d",dwExitCode);
	
    message.Format("%-*.*s%-*.*s%-*.*s",20,20,"REAPER",32,32,strID,10,10,strES);
	
	TRACE3(_T("In Reaper calling Command Server with <%s> <%s> <%s>\n"),
		strID,strHostname,strES);
	
	int nLen=message.GetLength();
	
	TRACE1(_T("In Reaper calling Command Server with <%s>\n"),
		message);
	
	// Send a string to the server.
	if (send (ServerSock, message.GetBuffer(nLen),nLen, 0)
		== SOCKET_ERROR) 
	{
		TRACE1(_T("Sending data to the server failed. Error: %d\n"),
			WSAGetLastError ());
		return;
	}

	// Disable sending on ServerSock.
	shutdown (ServerSock, 0x01);

	TRACE0("Leaving Reaper....\n"); 
}


////////////////////////////all the other functions/////////////////////////////////////
//

BOOL getAndAllocateLogonSid(HANDLE hToken,PSID *pLogonSid)
{
	PTOKEN_GROUPS	ptgGroups = NULL;
	DWORD			cbBuffer  = 0;  	/* allocation size */
	DWORD			dwSidLength;		/* required size to hold Sid */
	UINT			i;					/* Sid index counter */
	BOOL			bSuccess  = FALSE;	/* assume this function will fail */

	*pLogonSid = NULL; // invalidate pointer

	/*
	** Get necessary memory allocation
	*/
	GetTokenInformation(hToken, TokenGroups, ptgGroups, cbBuffer, &cbBuffer);

	if (cbBuffer)
		ptgGroups = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbBuffer);

	/*
	**	Get Sids for all groups the user belongs to
	*/
	bSuccess = GetTokenInformation(
					hToken,
					TokenGroups,
					ptgGroups,
					cbBuffer,
					&cbBuffer
				);
	if (bSuccess == FALSE)
		goto finish3;

	/*
	** Get the logon Sid by looping through the Sids in the token
	*/
	for(i = 0 ; i < ptgGroups->GroupCount ; i++)
	{
		if (ptgGroups->Groups[i].Attributes & SE_GROUP_LOGON_ID)
		{
			/*
			** insure we are dealing with a valid Sid
			*/
			bSuccess = IsValidSid(ptgGroups->Groups[i].Sid);
			if (bSuccess == FALSE)
				goto finish3;

			/*
			** get required allocation size to copy the Sid
			*/
			dwSidLength=GetLengthSid(ptgGroups->Groups[i].Sid);

			/*
			** allocate storage for the Logon Sid
			*/
			if(
				(*pLogonSid = (PSID)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwSidLength))
				==
				NULL
			)
			{
				bSuccess = FALSE;
				goto finish3;
			}

			/*
			** copy the Logon Sid to the storage we just allocated
			*/
			bSuccess = CopySid(
							dwSidLength,
							*pLogonSid,
							ptgGroups->Groups[i].Sid
						);

			break;
		}
	}


finish3:
	/*
	** free allocated resources
	*/
	if (bSuccess == FALSE)
	{
		if(*pLogonSid != NULL)
		{
			HeapFree(GetProcessHeap(), 0, *pLogonSid);
			*pLogonSid = NULL;
		}
	}

	if (ptgGroups != NULL)
		HeapFree(GetProcessHeap(), 0, ptgGroups);

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////
//
//
 
BOOL setSidOnAcl(PSID pSid,PACL pAclSource,PACL *pAclDestination,DWORD AccessMask,
	             BOOL bAddSid,BOOL bFreeOldAcl)
{
	ACL_SIZE_INFORMATION	AclInfo;
	DWORD					dwNewAclSize;
	LPVOID					pAce;
	DWORD					AceCounter;
	BOOL					bSuccess=FALSE;

	/*
	** If we were given a NULL Acl, just provide a NULL Acl
	*/
	if (pAclSource == NULL)
	{
		*pAclDestination = NULL;
		return TRUE;
	}

	if (!IsValidSid(pSid)) return FALSE;

	/*
	**	Get ACL's parameters
	*/
	if (
		!GetAclInformation(
			pAclSource,
			&AclInfo,
			sizeof(ACL_SIZE_INFORMATION),
			AclSizeInformation
		)
	)
		return FALSE;

	/*
	**	Compute size for new ACL, based on
	**	addition or subtraction of ACE
	*/
	if (bAddSid)
	{
		dwNewAclSize = AclInfo.AclBytesInUse  +
							sizeof(ACCESS_ALLOWED_ACE)  +
							GetLengthSid(pSid)          -
							sizeof(DWORD)               ;
	}
	else
	{
		dwNewAclSize = AclInfo.AclBytesInUse  -
							sizeof(ACCESS_ALLOWED_ACE)  -
							GetLengthSid(pSid)          +
							sizeof(DWORD)               ;
	}


    TRACE2("ACL size for %s <%d>", (bAddSid ? "allow" : "disallow"), dwNewAclSize);

	*pAclDestination = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNewAclSize);
	if(*pAclDestination == NULL)
		return FALSE;

	/*
	** initialize new Acl
	*/
	bSuccess = InitializeAcl(*pAclDestination, dwNewAclSize, ACL_REVISION);
	if (bSuccess == FALSE)
		goto finish5;

	/*
	** copy existing ACEs to new ACL
	*/
    TRACE2("%d ACEs in ACL for %s", AclInfo.AceCount, (bAddSid ? "allow" : "disallow"));

	for(AceCounter = 0 ; AceCounter < AclInfo.AceCount ; AceCounter++)
	{
		/*
		** fetch existing ace
		*/
		bSuccess = GetAce(pAclSource, AceCounter, &pAce);
		if (bSuccess == FALSE)
			goto finish5;

		/*
		** check to see if we are removing the ACE
		*/
		if (!bAddSid)
		{
			/*
			** we only care about ACCESS_ALLOWED ACEs
			*/
			if ((((PACE_HEADER)pAce)->AceType) == ACCESS_ALLOWED_ACE_TYPE)
			{
				PSID pTempSid=(PSID)&((PACCESS_ALLOWED_ACE)pAce)->SidStart;
				/*
				** if the Sid matches, skip adding this Sid
				*/
				if (EqualSid(pSid, pTempSid)) continue;
			}
		}

		/*
		** append ACE to ACL
		*/
		bSuccess = AddAce(
						*pAclDestination,
						ACL_REVISION,
						0,  // maintain Ace order
						pAce,
						((PACE_HEADER)pAce)->AceSize
					);
		if (bSuccess == FALSE)
			goto finish5;

	}

	/*
	** If appropriate, add ACE representing pSid
	*/
	if (bAddSid)
		bSuccess = AddAccessAllowedAce(
						*pAclDestination,
						ACL_REVISION,
						AccessMask,
						pSid
					);

finish5:
	/*
	** free memory if an error occurred
	*/
	if (!bSuccess)
	{
		if(*pAclDestination != NULL)
			HeapFree(GetProcessHeap(), 0, *pAclDestination);
	}
	else if (bFreeOldAcl)
		HeapFree(GetProcessHeap(), 0, pAclSource);

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////
//
//

BOOL setWinstaDesktopSecurity(HWINSTA hWinsta,HDESK hDesktop,PSID pLogonSid,BOOL bGrant,
	                          HANDLE hToken)
{
	SECURITY_INFORMATION	si = DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR	sdDesktop = NULL;
	PSECURITY_DESCRIPTOR	sdWinsta = NULL;
	SECURITY_DESCRIPTOR		sdNewDesktop;
	SECURITY_DESCRIPTOR		sdNewWinsta;
	DWORD					sdDesktopLength	= 0;	/* allocation size */
	DWORD					sdWinstaLength	= 0;	/* allocation size */
	PACL					pDesktopDacl;		/* previous Dacl on Desktop */
	PACL					pWinstaDacl;        /* previous Dacl on Winsta */
	PACL					pNewDesktopDacl	= NULL;	/* new Dacl for Desktop */
	PACL					pNewWinstaDacl	= NULL;	/* new Dacl for Winsta */
	BOOL					bDesktopDaclPresent;
	BOOL					bWinstaDaclPresent;
	BOOL					bDaclDefaultDesktop;
	BOOL					bDaclDefaultWinsta;
	BOOL					bSuccess		= FALSE;
	PSID					pUserSid = NULL;
    HANDLE                  mutex;
    BOOL                    locked = FALSE;


    // Get/SetUserObjectSecurity does not appear to be "thread-safe", and since
    //  we could potientially have multiple prelaunches running, this mutex
    //  is needed.
    mutex = CreateMutex(NULL, FALSE, LOCK_NAME);
    ::WaitForSingleObject(mutex,INFINITE);
    locked = TRUE;
		

	/*
	** Obtain security descriptor for Desktop
	*/
	GetUserObjectSecurity(
		hDesktop,
		&si,
		sdDesktop,
		sdDesktopLength,
		&sdDesktopLength
	);

	if (sdDesktopLength)
		sdDesktop = (PSECURITY_DESCRIPTOR)HeapAlloc(
						GetProcessHeap(), HEAP_ZERO_MEMORY, sdDesktopLength);

	bSuccess = GetUserObjectSecurity(
		hDesktop,
		&si,
		sdDesktop,
		sdDesktopLength,
		&sdDesktopLength
	);

	if (bSuccess == FALSE)
		goto finish4;

	/*
	** Obtain security descriptor for Window station
	*/
	GetUserObjectSecurity(
		hWinsta,
		&si,
		sdWinsta,
		sdWinstaLength,
		&sdWinstaLength
	);

	if (sdWinstaLength)
		sdWinsta = (PSECURITY_DESCRIPTOR)HeapAlloc(
							GetProcessHeap(), HEAP_ZERO_MEMORY, sdWinstaLength);

	bSuccess = GetUserObjectSecurity(
		hWinsta,
		&si,
		sdWinsta,
		sdWinstaLength,
		&sdWinstaLength
	);

	if (bSuccess == FALSE)
		goto finish4;

// Locking just the Get/SetUserObjectSecurity calls does not appear to be
//   enough to provide thread safety, so the whole function is within the
//   mutex
// 
//    locked = FALSE;
//    ReleaseMutex(mutex);


	/*
	** Obtain DACL from security descriptor for desktop
	*/
	bSuccess = GetSecurityDescriptorDacl(
					sdDesktop,
					&bDesktopDaclPresent,
					&pDesktopDacl,
					&bDaclDefaultDesktop
				);

	if (bSuccess == FALSE)
		goto finish4;

	/*
	** Obtain DACL from security descriptor for Window station
	*/
	bSuccess = GetSecurityDescriptorDacl(
					sdWinsta,
					&bWinstaDaclPresent,
					&pWinstaDacl,
					&bDaclDefaultWinsta
				);

	if (bSuccess == FALSE)
		goto finish4;

	/*
	** Create new DACL with Logon and User Sid for Desktop
	*/
	if(bDesktopDaclPresent) {
		bSuccess = setSidOnAcl(
			pLogonSid,
			pDesktopDacl,
			&pNewDesktopDacl,
			GENERIC_READ | GENERIC_WRITE | READ_CONTROL
			| DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW
			| DESKTOP_CREATEMENU | DESKTOP_SWITCHDESKTOP
			| DESKTOP_ENUMERATE,
			bGrant,
			FALSE
		);

		if (bSuccess == FALSE)
			goto finish4;
	}

	/*
	** Create new DACL with Logon and User Sid for Window station
	*/
	if(bWinstaDaclPresent)
	{
		bSuccess = setSidOnAcl(
						pLogonSid,
						pWinstaDacl,
						&pNewWinstaDacl,
						GENERIC_READ | GENERIC_WRITE | READ_CONTROL
						| WINSTA_ACCESSGLOBALATOMS
						| WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES
						| WINSTA_ACCESSCLIPBOARD | WINSTA_ENUMERATE
						| WINSTA_EXITWINDOWS,
						bGrant,
						FALSE
					);

		if (bSuccess == FALSE)
			goto finish4;
	}
 
	/*
	** Initialize the target security descriptor for Desktop
	*/
	if (bDesktopDaclPresent)
	{
		bSuccess = InitializeSecurityDescriptor(
						&sdNewDesktop,
						SECURITY_DESCRIPTOR_REVISION
					);

		if (bSuccess == FALSE)
			goto finish4;
	}

	/*
	** Initialize the target security descriptor for Window station
	*/
	if(bWinstaDaclPresent)
	{
		bSuccess = InitializeSecurityDescriptor(
						&sdNewWinsta,
						SECURITY_DESCRIPTOR_REVISION
					);

		if (bSuccess == FALSE)
			goto finish4;
	}

	/*
	** Apply new ACL to the Desktop security descriptor
	*/
	if(bDesktopDaclPresent)
	{
		bSuccess = SetSecurityDescriptorDacl(
						&sdNewDesktop,
						TRUE,
						pNewDesktopDacl,
						bDaclDefaultDesktop
					);

		if (bSuccess == FALSE)
			goto finish4;
	}

	/*
	** Apply new ACL to the Window station security descriptor
	*/
	if(bWinstaDaclPresent)
	{
		bSuccess = SetSecurityDescriptorDacl(
						&sdNewWinsta,
						TRUE,
						pNewWinstaDacl,
						bDaclDefaultWinsta
					);

		if (bSuccess == FALSE)
			goto finish4;
	}


// Locking just the Get/SetUserObjectSecurity calls does not appear to be
//   enough to provide thread safety, so the whole function is within the
//   mutex
// 
//    mutex = OpenMutex(NULL, FALSE, LOCK_NAME);
//    ::WaitForSingleObject(mutex,INFINITE);
//    locked = TRUE;

	/*
	** Apply security descriptors with new DACLs to Desktop and Window station
	*/    
    TRACE1("setting desktop security for %s...", (bGrant ? "allow" : "disallow"));
	if (bDesktopDaclPresent)
	{
		bSuccess = SetUserObjectSecurity(
									hDesktop,
									&si,
									&sdNewDesktop
					);

		if (bSuccess == FALSE)
			goto finish4;
	}

    TRACE1("set desktop security for %s", , (bGrant ? "allow" : "disallow"));
    TRACE1("setting window security for %s...", (bGrant ? "allow" : "disallow"));
	if(bWinstaDaclPresent)
		bSuccess = SetUserObjectSecurity(
									hWinsta,
									&si,
									&sdNewWinsta
					);

	if (bSuccess == FALSE)
    {
        TRACE1("error setting window security for %s", (bGrant ? "allow" : "disallow"));
		goto finish4;
    }

    TRACE1("set window security for %s", (bGrant ? "allow" : "disallow"));

    // Release the mutex
    locked = FALSE;
    ReleaseMutex(mutex);


finish4:

    // This should only be true if we got here by one of the "goto" calls
    if (locked == TRUE)
       ReleaseMutex(mutex);


	if (sdDesktop != NULL)
		HeapFree(GetProcessHeap(), 0, sdDesktop);

	if (sdWinsta != NULL)
		HeapFree(GetProcessHeap(), 0, sdWinsta);

	if (pNewDesktopDacl != NULL)
		HeapFree(GetProcessHeap(), 0, pNewDesktopDacl);

	if (pNewWinstaDacl != NULL)
		HeapFree(GetProcessHeap(), 0, pNewWinstaDacl);

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////
BOOL allowDesktopAccess(HANDLE hToken)
{	
	BOOL	ok = FALSE;

	if (!getAndAllocateLogonSid(hToken, &pLogonSid))
		return FALSE;

	hWinsta=GetProcessWindowStation();
	hDesktop=GetThreadDesktop(GetCurrentThreadId());
	 
	ok = SetHandleInformation(hDesktop,
							  HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	if (!ok)
		return FALSE;

	ok = SetHandleInformation(hWinsta,
							  HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	if (!ok)
		return FALSE;

	ok = setWinstaDesktopSecurity(hWinsta, hDesktop, pLogonSid, TRUE, hToken);	    

	return ok;
}

BOOL disallowDesktopAccess()
{		
	BOOL	ok = FALSE;

	ok = setWinstaDesktopSecurity(hWinsta, hDesktop, pLogonSid, FALSE, NULL);

	if(pLogonSid != NULL)
		HeapFree(GetProcessHeap(), 0, pLogonSid);

    CloseWindowStation(hWinsta);
    CloseDesktop(hDesktop);

	return ok;
}

BOOL allowDesktopAccess2(void)
{
   HWINSTA hWinsta;
   HDESK hDesk;

   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   SECURITY_DESCRIPTOR sd;

   hWinsta = GetProcessWindowStation();
   if (hWinsta == NULL) return FALSE;

   TRACE0("Got window station");

   hDesk = GetThreadDesktop(GetCurrentThreadId());
   if (hDesk == NULL) return FALSE;

   TRACE0("Got desktop");

   InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
   SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE);

   TRACE0("Set security descriptor");

   if (!SetUserObjectSecurity(hWinsta, &si, &sd)) return FALSE;

   TRACE0("Set window station security");

   if (SetUserObjectSecurity(hDesk, &si, &sd) == TRUE) {
      TRACE0("Set desktop security");
      return TRUE;
   }
   else
      return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
//
void getlasterror(const CString& strInputString)
{
	
	LPTSTR 	lpMsgBuf;
	DWORD	cnBytes;
	
	cnBytes = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);
	
	if (cnBytes > 0)
	{
		TRACE2(_T("%s -> %s\n"),strInputString,lpMsgBuf);
		//Any error put "!" in 1st char & the OS error
		//so Java can tell it's an error & not a PID
		cout << _T("!") << lpMsgBuf << endl;
		::LocalFree(lpMsgBuf);
	}
	else
		TRACE1(_T("%s\n"),strInputString);

}

/////////////////////////////////////////////////////////////////////////
//
//
UINT the_thread(LPVOID lpParameter)
//just watches the keyboard for the DIE command
//if launched process signals it ends
{

	TRACE0("Keyboard thread is starting\n");
	CString buf;

	while(1)
		if(cin.peek() != EOF)
		{ 
			cin.getline(buf.GetBuffer(10),10);
			buf.ReleaseBuffer();
			if(buf=="DIE")
			{
				TRACE0("CIN: GOT KILL MESSAGE\n");
				SetEvent(g_hEvent);
				break;
			}
		}

	TRACE0("Keyboard thread is exiting\n"); 

	return 0;

}


//////////////////////////////////////////////////////////////////////////////
//
//
BOOL login(LPTSTR lpszUser, LPTSTR lpszDCP, PHANDLE phToken)
{
     LSA_OBJECT_ATTRIBUTES ObjectAttributes;
     PLSA_UNICODE_STRING lsaszServer = NULL;
     NTSTATUS ntsResult;
     LSA_HANDLE lsahPolicyHandle;

     LSA_ENUMERATION_HANDLE lsaEnumHandle = 0; 
     PLSA_TRUST_INFORMATION TrustInfo;
     ULONG ulReturned; 
     ULONG ulCounter;    

     BOOL loggedIn = FALSE;
     ULONG win32status;

     USES_CONVERSION;


     // Object attributes are reserved, so initalize to zeroes.
     ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

     // Attempt to open the policy.
     ntsResult = LsaOpenPolicy(
             lsaszServer,
             &ObjectAttributes,
//             POLICY_VIEW_LOCAL_INFORMATION, 
             POLICY_VIEW_LOCAL_INFORMATION |
             GENERIC_READ | GENERIC_EXECUTE | POLICY_ALL_ACCESS,
             &lsahPolicyHandle  //recieves the policy handle
             );
     win32status = LsaNtStatusToWinError(ntsResult);

     if (win32status == ERROR_SUCCESS) {

        // Try to log the user into the primary domain 
        if (loggedIn == FALSE) {

           TRACE0("Trying primary domain...");

           POLICY_PRIMARY_DOMAIN_INFO *pppdi = NULL;

           // Get the primary domain
           ntsResult = LsaQueryInformationPolicy( lsahPolicyHandle, PolicyPrimaryDomainInformation, 
                                                  (void **) &pppdi );
           win32status = LsaNtStatusToWinError(ntsResult);

           if (win32status == ERROR_SUCCESS) {
              
              TRACE0(W2T(pppdi->Name.Buffer));

              // Attempt to log the user into that domain
              if(::LogonUser(lpszUser,W2T(pppdi->Name.Buffer),lpszDCP,LOGON32_LOGON_INTERACTIVE,
				     LOGON32_PROVIDER_DEFAULT,phToken)) 
                 loggedIn = TRUE;                               
           }

           if ( pppdi != NULL )
              LsaFreeMemory( pppdi );
           
        } 

        // Try to log the user into the account domain 
        if (loggedIn == FALSE) {
           TRACE0("Trying account domain...");

           POLICY_ACCOUNT_DOMAIN_INFO *ppadi = NULL;

           // Get the account domain
           ntsResult = LsaQueryInformationPolicy( lsahPolicyHandle, PolicyAccountDomainInformation, 
                                                  (void **) &ppadi );
           win32status = LsaNtStatusToWinError(ntsResult);

           if (win32status == ERROR_SUCCESS) {

              TRACE0(W2T(ppadi->DomainName.Buffer));

              // Attempt to log the user into that domain
              if(::LogonUser(lpszUser,W2T(ppadi->DomainName.Buffer),lpszDCP,LOGON32_LOGON_INTERACTIVE,
				     LOGON32_PROVIDER_DEFAULT,phToken)) 
                 loggedIn = TRUE;
           }

           if ( ppadi != NULL )
              LsaFreeMemory( ppadi );

        }

        // Try to log the user into trusted domains until a login attempt is successful or
        //  list of trusts runs out
        if (loggedIn == FALSE) {

           TRACE0("Enumerating trust domains...");
           
           do {
              ntsResult = LsaEnumerateTrustedDomains(lsahPolicyHandle, &lsaEnumHandle, 
                                   (void **) &TrustInfo, 32000, &ulReturned);
              win32status = LsaNtStatusToWinError(ntsResult);

              if ( (win32status != ERROR_SUCCESS)      &&
                   (win32status != ERROR_MORE_DATA) &&
                   (win32status != ERROR_NO_MORE_ITEMS) ) {           
                 ::SetLastError( ::LsaNtStatusToWinError( ntsResult ));   
                 break;
              }

              for ( ulCounter = 0 ; ulCounter < ulReturned ; ulCounter++ ) {  

                 TRACE0(W2T(TrustInfo[ulCounter].Name.Buffer));            

                 // Attempt to log the user into that domain
                 if(::LogonUser(lpszUser,W2T(TrustInfo[ulCounter].Name.Buffer),lpszDCP,LOGON32_LOGON_INTERACTIVE,
				     LOGON32_PROVIDER_DEFAULT,phToken)) {
                    loggedIn = TRUE;
                    break;
                 }
              }

              ::LsaFreeMemory( TrustInfo ); 

              if (loggedIn == TRUE)
                 break;

           } while (win32status != ERROR_NO_MORE_ITEMS);
        }                

        // As a last ditch attempt, try to log the user into the domain called "NTCSS"
        if (loggedIn == FALSE) {

           TRACE0("Last ditch logon attempt...");

           if(::LogonUser(lpszUser,_T("NTCSS"),lpszDCP,LOGON32_LOGON_INTERACTIVE,
				     LOGON32_PROVIDER_DEFAULT,phToken)) 
             loggedIn = TRUE;
        }


        // Freeing the policy object handle
        ntsResult = LsaClose(lsahPolicyHandle);
     } 
     else   
        TRACE0("Unable to get policy handler!");
     

     return loggedIn;        
}