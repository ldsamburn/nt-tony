/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ServiceClass.cpp: implementation of the ServiceClass class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "ServiceClass.h"

#include <string.h>
#include <stdio.h>

#include "atlbase.h"
#include "atlconv.h"

#include "Lm.h"
#include "ntsecapi.h"

// static variables
ServiceClass* ServiceClass::pThis = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ServiceClass::ServiceClass(const char* svcName, int mjVersion, int mnVersion) 
{
   init(svcName, mjVersion, mnVersion, NULL);   
}

ServiceClass::ServiceClass(const char* svcName, int mjVersion, int mnVersion, LPCTSTR dpList)
{
   init(svcName, mjVersion, mnVersion, dpList);   
}

void ServiceClass::init(const char* svcName, int mjVersion, int mnVersion, LPCTSTR dpList)
{
   // Copy the address of the current object so we can access it from
   // the static member callback functions (ServiceMain and Handler). 
   // WARNING: This limits the application to only one ServiceClass object. 
   pThis = this;

   // Set the default service name and version
   strncpy(serviceName, svcName, sizeof(serviceName)-1);
   majorVersion = mjVersion;
   minorVersion = mnVersion;

   status.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
   status.dwCurrentState            = SERVICE_STOPPED;
   status.dwControlsAccepted        = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
   status.dwWin32ExitCode           = 0;
   status.dwServiceSpecificExitCode = 0;
   status.dwCheckPoint              = 0;
   status.dwWaitHint                = 0;

   hServiceStatus = NULL;
   hEventSource = NULL;

   isRunning = FALSE;

   dependenciesList = dpList;
  
   serviceUserName = "NtcssSvcUsr";
   serviceUserPassword = "NtcssSvcUsrPwd";
}

ServiceClass::~ServiceClass()
{
   DebugMsg("ServiceClass::~ServiceClass()");
   if (hEventSource) {
      ::DeregisterEventSource(hEventSource);
   }
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions

// This function makes an entry into the application event log
void ServiceClass::LogEvent(WORD wType, DWORD dwID,
                          const char* pszS1,
                          const char* pszS2,
                          const char* pszS3)
{
    const char* ps[3];
    ps[0] = pszS1;
    ps[1] = pszS2;
    ps[2] = pszS3;

    int iStr = 0;
    for (int i = 0; i < 3; i++) {
        if (ps[i] != NULL) iStr++;
    }
        
    // Check the event source has been registered and if
    // not then register it now
    if (!hEventSource) {
        hEventSource = ::RegisterEventSource(NULL,  // local machine
                                               serviceName); // source name
    }

    if (hEventSource) {
        ::ReportEvent(hEventSource,
                      wType,
                      0,
                      dwID,
                      NULL, // sid
                      iStr,
                      0,
                      ps,
                      NULL);
    }
}

void ServiceClass::DebugMsg(const char* pszFormat, ...)
{
    char buf[1024];
    sprintf(buf, "[%s](%lu): ", serviceName, GetCurrentThreadId());
	va_list arglist;
	va_start(arglist, pszFormat);
    vsprintf(&buf[strlen(buf)], pszFormat, arglist);
	va_end(arglist);
    strcat(buf, "\n");
    OutputDebugString(buf);
}

////////////////////////////////////////////////////////////////////////////////////////
// Default command line argument parsing

// Returns TRUE if it found an arg it recognised, FALSE if not
// Note: processing some arguments causes output to stdout to be generated.
BOOL ServiceClass::ParseStandardArgs(int argc, char* argv[])
{
    // See if we have any command line args we recognize
    if (argc <= 1) return FALSE;

    if (_stricmp(argv[1], "-v") == 0) {

        // Display version info
        printf("%s Version %d.%d\n", serviceName, majorVersion, minorVersion);
        printf("The service is %s installed\n",
               IsInstalled() ? "currently" : "not");
        return TRUE; // say we processed the argument

    } else if (_stricmp(argv[1], "-i") == 0) {

        // Request to install.
        if (IsInstalled()) {
            printf("%s is already installed\n", serviceName);
        } else {
            // Try and install the copy that's running
            if (Install()) {
                printf("%s now installed\n", serviceName);
            } else {
                printf("%s failed to install. Error %d\n", serviceName, GetLastError());
            }
        }
        return TRUE; // say we processed the argument

    } else if (_stricmp(argv[1], "-u") == 0) {

        // Request to uninstall.
        if (!IsInstalled()) {
            printf("%s is not installed\n", serviceName);
        } else {
            // Try and remove the copy that's installed
            if (Uninstall()) {                
                printf("%s removed.\n", serviceName);
            } else {
                printf("Could not remove %s. Error %d\n", serviceName, GetLastError());
            }
        }
        return TRUE; // say we processed the argument
    
    } 
         
    // Don't recognise the args
    return FALSE; 
}

// Test if the service is currently installed
BOOL ServiceClass::IsInstalled()
{
    BOOL bResult = FALSE;

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {

        // Try to open the service
        SC_HANDLE hService = ::OpenService(hSCM,
                                           serviceName,
                                           SERVICE_QUERY_CONFIG);
        if (hService) {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }

        ::CloseServiceHandle(hSCM);
    }
    
    return bResult; 
}

BOOL ServiceClass::Install()
{
    char szFilePath[_MAX_PATH];
//    char userName[100];

//    USES_CONVERSION;

/*
    // Attempt to create the user that the service will be running as
    if (CreateServiceUser() == FALSE) {
       printf("Unable to create service user\n");
       return FALSE;
    }
*/

    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    // Get the executable file path    
    ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));     

    // Create the service
//    sprintf(userName, "%s\\%s", W2A(domainName), serviceUserName);    
    SC_HANDLE hService = ::CreateService(hSCM,
                                         serviceName,
                                         serviceName,
                                         SERVICE_ALL_ACCESS,
                                   // Creating an SERVICE_INTERACTIVE_PROCESS is important so that 
                                   //  batch jobs run successfully
                                         SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS ,
//                                       SERVICE_WIN32_OWN_PROCESS, // can't specifiy .._INTERACTIVE_PROCESS if specifing a service user
                                         SERVICE_AUTO_START,        
                                         SERVICE_ERROR_NORMAL,
                                         szFilePath,
                                         NULL,
                                         NULL,
                                         dependenciesList,
//                                         userName,
                                         NULL,
//                                         serviceUserPassword);
                                         NULL);
    if (!hService) {
        printf("Service creation failed\n");
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // make registry entries to support logging messages
    // Add the source name as a subkey under the Application
    // key in the EventLog service portion of the registry.
    char szKey[256];
    HKEY hKey = NULL;
    strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    strcat(szKey, serviceName);
    if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    // Add the Event ID message-file name to the 'EventMessageFile' subkey.
    ::RegSetValueEx(hKey,
                    "EventMessageFile",
                    0,
                    REG_EXPAND_SZ, 
                    (CONST BYTE*)szFilePath,
                    strlen(szFilePath) + 1);     

    // Set the supported types flags.
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    ::RegSetValueEx(hKey,
                    "TypesSupported",
                    0,
                    REG_DWORD,
                    (CONST BYTE*)&dwData,
                     sizeof(DWORD));
    ::RegCloseKey(hKey);

    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_INSTALLED, serviceName);

    // tidy up
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL ServiceClass::Uninstall()
{
    // Open the Service Control Manager
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    BOOL bResult = FALSE;
    SC_HANDLE hService = ::OpenService(hSCM,
                                       serviceName,
                                       DELETE);
    if (hService) {
        if (::DeleteService(hService)) {
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_REMOVED, serviceName);
            bResult = TRUE;
        } else {
            LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_NOTREMOVED, serviceName);
        }
        ::CloseServiceHandle(hService);
    }
    
    ::CloseServiceHandle(hSCM);
    return bResult;
}

// Creates a user that the service will be run as. 
// To successfully grant and revoke privileges, the caller needs to be an administrator on the target system. 
BOOL ServiceClass::CreateServiceUser()
{
   LPUSER_INFO_0 pBuf = NULL;
   NET_API_STATUS nStatus;
   USER_INFO_1 ui;
   DWORD dwError = 0;
   BOOL           fAPISuccess;
   LPVOID         pUserSID       = NULL;
   DWORD          cbUserSID      = 0;
   char *         szDomain       = NULL;
   DWORD          cbDomain       = 0;
   SID_NAME_USE   snuType;
//   HANDLE tokenHandle;
   LSA_UNICODE_STRING PrivilegeString; 
   LSA_OBJECT_ATTRIBUTES ObjectAttributes;
   PLSA_UNICODE_STRING lsaszServer = NULL;
   NTSTATUS ntsResult;
   LSA_HANDLE lsahPolicyHandle;
   POLICY_PRIMARY_DOMAIN_INFO *pppdi = NULL;    
   LPWSTR domainControllerName = NULL;
   USHORT SystemNameLength;
   LSA_UNICODE_STRING lusSystemName;
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;


#define heapalloc(x) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, x))
#define heapfree(x)  (HeapFree(GetProcessHeap(), 0, x))


   USES_CONVERSION;


   // First, determine the primary domain and domain controller name    
   

   // Attempt to open the policy.
   ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));
   ntsResult = LsaOpenPolicy(
             lsaszServer,
             &ObjectAttributes, 
             POLICY_VIEW_LOCAL_INFORMATION |
             GENERIC_READ | GENERIC_EXECUTE | POLICY_ALL_ACCESS,
             &lsahPolicyHandle  //recieves the policy handle
             );   
   if (LsaNtStatusToWinError(ntsResult) != ERROR_SUCCESS) {
      printf("Unable to open policy for getting domain name\n");
      return FALSE;
   }
      

   // Get the primary domain information
   ntsResult = LsaQueryInformationPolicy( lsahPolicyHandle, PolicyPrimaryDomainInformation, 
                                                  (void **) &pppdi );      

   if (LsaNtStatusToWinError(ntsResult) == ERROR_SUCCESS)                                                     
      domainName = pppdi->Name.Buffer;                                                           

   if ( pppdi != NULL )
      LsaFreeMemory( pppdi );

   // Freeing the policy object handle
   LsaClose(lsahPolicyHandle);

   if (LsaNtStatusToWinError(ntsResult) != ERROR_SUCCESS) {
      printf("Unable to query policy for getting domain info\n");
      return FALSE;
   }                              


   // Get the name of the primary domain controller in this domain
   if (NetGetDCName(NULL,domainName,(LPBYTE *)&domainControllerName) != NERR_Success) {         
      printf("Error getting DC for <%s>\n", W2T(domainName));         
      return FALSE; 
   }   



   // See if the user account exists
   nStatus = NetUserGetInfo(domainControllerName, A2W(serviceUserName), 0, (LPBYTE *)&pBuf);
   if ((nStatus != NERR_Success) && (nStatus != NERR_UserNotFound)) {
      printf("Unable to determine if service user already exists\n");
      return FALSE;
   }

   if (pBuf != NULL)
      NetApiBufferFree(pBuf);


   // If the user doesn't exist, try to create the account
   if (nStatus == NERR_UserNotFound) {
      
      printf("Adding service user...\n");

      // Add the user to the domain
      ZeroMemory( &ui, sizeof(ui) );
      ui.usri1_name = A2W(serviceUserName);     
      ui.usri1_password = A2W(serviceUserPassword); 
      ui.usri1_priv = USER_PRIV_USER;
      ui.usri1_home_dir = NULL;
      ui.usri1_comment = A2W(serviceUserName);
      ui.usri1_flags = UF_NORMAL_ACCOUNT | UF_SCRIPT;
      ui.usri1_script_path = NULL;  
      nStatus = NetUserAdd(domainControllerName, 1, (LPBYTE)&ui, &dwError); 
      if (nStatus != NERR_Success) {         
         printf("User add failed\n");
         return FALSE;
      } 

      // Add the user to the necessary group so that they have domain administration rights
      if (NetGroupAddUser(domainControllerName, A2W("Domain Admins"), A2W(serviceUserName)) != NERR_Success) {
         printf("Unable to add service user to domain admin group\n");
         return FALSE;
      }     

      // 
      // Get SID for the user so that certain privileges can be granted.
      // 
      fAPISuccess = LookupAccountName((LPCTSTR) W2T(domainControllerName), serviceUserName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);

      // API should have failed with insufficient buffer.
      if (fAPISuccess) {
         printf("Could not lookup user");
         return FALSE;
      }
      else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
         printf("Lookup failed x.  Error %d\n", GetLastError());         
         return FALSE;
      }

      pUserSID = heapalloc(cbUserSID);
      if (!pUserSID) {
         printf("HeapAlloc() failed.  Error %d\n", GetLastError());         
         return FALSE;
      }

      szDomain = (char *) heapalloc(cbDomain);
      if (!szDomain) {
         printf("HeapAlloc() failed.  Error %d\n", GetLastError());         
         return FALSE;
      }

      fAPISuccess = LookupAccountName((LPCTSTR) W2T(domainControllerName), serviceUserName,
            pUserSID, &cbUserSID, szDomain, &cbDomain, &snuType);
      if (!fAPISuccess) {
         printf("Lookup failed.  Error %d\n", GetLastError());         
         return FALSE;
      }

           
      // Attempt to open the policy.so that privileges can be granted
      SystemNameLength = wcslen(domainControllerName);
      lusSystemName.Buffer = domainControllerName;
      lusSystemName.Length = SystemNameLength * sizeof(WCHAR);
      lusSystemName.MaximumLength= (SystemNameLength+1) * sizeof(WCHAR);

      ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));
      ntsResult = LsaOpenPolicy(
                     &lusSystemName,
                     &ObjectAttributes,
                     POLICY_VIEW_LOCAL_INFORMATION |
                     GENERIC_READ | GENERIC_EXECUTE | POLICY_ALL_ACCESS |
                     POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES ,
                     &lsahPolicyHandle);
      if (LsaNtStatusToWinError(ntsResult) != ERROR_SUCCESS) {
         printf("Policy open unsuccessful\n");
         return FALSE;  
      }


      // Add the necessary privilege(s)
      InitLsaString(&PrivilegeString, L"SeServiceLogonRight");
      ntsResult = LsaAddAccountRights(lsahPolicyHandle,  // An open policy handle.
                                      pUserSID,          // The target SID.
                                      &PrivilegeString,  // The privilege(s).
                                      1);                // Number of privileges.  
      
      LsaClose(lsahPolicyHandle); 

      if (pUserSID)
         heapfree(pUserSID);

      if (szDomain)
         heapfree(szDomain);
  

      if (LsaNtStatusToWinError(ntsResult) != ERROR_SUCCESS) {
         printf("LsaAddAccountRights unsuccessful\n");         

         LsaClose(lsahPolicyHandle);   
         return FALSE;
      }



      ///////// Maybe a periodic call to LookupAccountName here would do instead
      /////////  of calling "net accounts /sync" 


      // Even though the user was just added, logon attempts fail until a 
      //  synchronization is done. So a process is created to synchronize
      //  the account databases
      ZeroMemory(&startInfo, sizeof(startInfo));
      startInfo.cb = sizeof(startInfo);
      startInfo.dwFlags = STARTF_USESHOWWINDOW;
      startInfo.wShowWindow = SW_HIDE;  

      printf("Synchronizing account...\n");

      // NOTE: created with CREATE_NO_WINDOW so output of "net accounts /sync" isn't displayed to
      //  the console
      if ((CreateProcess(NULL, "net accounts /sync", NULL, NULL, FALSE, 
                         CREATE_NO_WINDOW , NULL, NULL, &startInfo, &processInfo) == FALSE) ||
          (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED)) {
         printf("Synchronization failed");
         return FALSE;
      }      

      // Sleep for a period of time to make sure the synchronization completes
      Sleep(1000 * 20);

/*  This doesn't seem to be needed 
      // Privileges aren't applied to the user until they login, so force a login here
      if (LogonUser(serviceUserName,       // string that specifies the user name           
                 W2A(domainName),
                 serviceUserPassword,      // string that specifies the password
                 LOGON32_LOGON_SERVICE,    // specifies the type of logon operation              
                 LOGON32_PROVIDER_DEFAULT, // specifies the logon provider
                 &tokenHandle) == 0) {     // pointer to variable to receive token handle
         printf("Could not log user in <%d>\n", GetLastError());
         return FALSE;
      }         
      else  
         CloseHandle(tokenHandle);  
*/  
          

      printf("User successfully added\n");
      
   }  // end of "if (nStatus == NERR_UserNotFound) "

   
   return TRUE;
}

void ServiceClass::InitLsaString(PLSA_UNICODE_STRING LsaString, LPWSTR String)
{
  USHORT StringLength;

  if (String == NULL) 
  {
    LsaString->Buffer = NULL;
    LsaString->Length = 0;
    LsaString->MaximumLength = 0;
    return;
  }
  // Get the length of the string without the null terminator.
  StringLength = wcslen(String);
  // Store the string.
  LsaString->Buffer = String;
  LsaString->Length =  StringLength * sizeof(WCHAR);
  LsaString->MaximumLength= (StringLength+1) * sizeof(WCHAR);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration

BOOL ServiceClass::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
        {serviceName, ServiceMain},
        {NULL, NULL}
    };
    
    DebugMsg("Calling service ctrl dispatcher...");

    BOOL b = ::StartServiceCtrlDispatcher(st);

    DebugMsg("Called service ctrl dispatcher");

    return b;
}

// static member function (callback)
void ServiceClass::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{

    // Get a pointer to the C++ object
    ServiceClass* pService = pThis;

    pService->DebugMsg("Entering ServiceClass::ServiceMain()");

    // Save off the paramets specified in the "Startup Parameters" box of the services control dialog
    pService->numArgs = dwArgc;
    pService->args    = lpszArgv;


    // Register the control request handler
    pService->status.dwCurrentState = SERVICE_START_PENDING;
    pService->hServiceStatus = RegisterServiceCtrlHandler(pService->serviceName,
                                                           Handler);
    if (pService->hServiceStatus == NULL) {
        pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_CTRLHANDLERNOTINSTALLED);
        return;
    }

    // Start the initialisation
    if (pService->Initialize()) {

        // Do the real work. 
        // When the Run function returns, the service has stopped.
        pService->isRunning = TRUE;
        pService->status.dwWin32ExitCode = 0;
        pService->status.dwCheckPoint = 0;
        pService->status.dwWaitHint = 0;
        pService->Run();		
    }

    // Tell the service manager we are stopped
    pService->SetStatus(SERVICE_STOPPED);

    pService->DebugMsg("Leaving ServiceClass::ServiceMain()"); 
}


//////////////////////////////////////////////////////////////////////////////////////
// Control request handlers

// static member function (callback) to handle commands from the
// service control manager
void ServiceClass::Handler(DWORD dwOpcode)
{

    // Get a pointer to the object
    ServiceClass* pService = pThis;
    
    pService->DebugMsg("ServiceClass::Handler(%lu)", dwOpcode);

    switch (dwOpcode) {
    case SERVICE_CONTROL_STOP: // 1        
//    case SERVICE_CONTROL_SHUTDOWN: // 5
        pService->OnStop();
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        pService->OnPause();
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        pService->OnContinue();
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        pService->OnInterrogate();
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        pService->OnShutdown();
        break;

    default:
        if (dwOpcode >= SERVICE_CONTROL_USER) {
            if (!pService->OnUserControl(dwOpcode)) {
                pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
            }
        } else {
            pService->LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_BADREQUEST);
        }
        break;
    }

    // Report current status
    pService->DebugMsg("Updating status (%lu, %lu)",
                       pService->hServiceStatus,
                       pService->status.dwCurrentState);
    ::SetServiceStatus(pService->hServiceStatus, &pService->status);

}


///////////////////////////////////////////////////////////////////////////////////////////
// Service initialization

BOOL ServiceClass::Initialize()
{
  
    // Start the initialization
    SetStatus(SERVICE_START_PENDING);
    
    // Perform the actual initialization
    BOOL bResult = OnInit(); 
    
    // Set final state
    status.dwWin32ExitCode = GetLastError();
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    if (!bResult) {
        LogEvent(EVENTLOG_ERROR_TYPE, EVMSG_FAILEDINIT);
        SetStatus(SERVICE_STOPPED);
        return FALSE;    
    }
    
    LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STARTED);
    SetStatus(SERVICE_RUNNING);
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////
// status functions

void ServiceClass::SetStatus(DWORD dwState)
{
    DebugMsg("ServiceClass::SetStatus(%lu, %lu)", hServiceStatus, dwState);
    status.dwCurrentState = dwState;
    ::SetServiceStatus(hServiceStatus, &status);
}

// Called when the service is first initialized
BOOL ServiceClass::OnInit()
{
    DebugMsg("ServiceClass::OnInit()");
	return TRUE;
}

// Called when the service control manager wants to stop the service
void ServiceClass::OnStop()
{
   DebugMsg("ServiceClass::OnStop()");
}

// called when the service is paused
void ServiceClass::OnPause()
{
    DebugMsg("ServiceClass::OnPause()");
}

// called when the service is continued
void ServiceClass::OnContinue()
{
    DebugMsg("ServiceClass::OnContinue()");
}

// called when the service is interrogated
void ServiceClass::OnInterrogate()
{
    DebugMsg("ServiceClass::OnInterrogate()");
}

// called when the machine is shut down
// When the system shuts down, it does not shutdown a service's
//  dependent services first. So this code sends the shutdown
//  message to dependent services first (if there are any),
//  and then runs the shutdown code for this service.
void ServiceClass::OnShutdown()
{     
      DWORD i;
      DWORD dwBytesNeeded;
      DWORD dwCount;
      DWORD dwStartTime = GetTickCount();

      SERVICE_STATUS ss;
      LPENUM_SERVICE_STATUS   lpDependencies = NULL;
      ENUM_SERVICE_STATUS     ess;
      SC_HANDLE               hDepService;

      char errorMsg[100];                     
      SC_HANDLE hSCM;      
      SC_HANDLE hService;  
      DWORD dwTimeout = 20000;  // Maximum time (in milliseconds) to wait 
                                //  for the service's dependencies to stop
      BOOL failure = FALSE;


      sprintf(errorMsg, "Entering %s OnShutdown", serviceName);
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);

      // Get a handle to the service control manager
      hSCM = ::OpenSCManager(NULL, // local machine
                             NULL, // ServicesActive database
                             SC_MANAGER_ALL_ACCESS); // full access
      if (hSCM) {

         // Try to open the service
         hService = ::OpenService(hSCM, serviceName, SERVICE_QUERY_CONFIG);
         if (!hService) {
            sprintf(errorMsg, "%s OnShutdown Couldn't open service <%d>", serviceName, GetLastError());
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);
            failure = TRUE;
         }
      }
      else {
         sprintf(errorMsg, "%s OnShutdown Couldn't get service control manager handle <%d>", serviceName, 
                 GetLastError());
         LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);
         failure = TRUE;
      }      

      // If there were no 
      if (failure == FALSE) {
         
         // Pass a zero-length buffer to get the required buffer size
         if ( EnumDependentServices( hService, SERVICE_ACTIVE, 
            lpDependencies, 0, &dwBytesNeeded, &dwCount ) ) {

            // If the Enum call succeeds, then there are no dependent
            // services so do nothing

         } else {
          
            if ( GetLastError() != ERROR_MORE_DATA ) {
               sprintf(errorMsg, "%s OnShutdown Unexpected error <%d>", serviceName, GetLastError());
               LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg); 				
               return;
            }

            // Allocate a buffer for the dependencies
            lpDependencies = (LPENUM_SERVICE_STATUS) HeapAlloc( 
                  GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded );
            if ( !lpDependencies ) {
               sprintf(errorMsg, "%s OnShutdown Error allocating buffer <%d>", serviceName, GetLastError());
               LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg); 
               return;
            } 

            __try {
 
               // Enumerate the dependencies
               if ( !EnumDependentServices( hService, SERVICE_ACTIVE, 
                     lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                     &dwCount ) ) {
                  sprintf(errorMsg, "%s OnShutdown Error enumerating dependencies <%d>", serviceName, GetLastError());
                  LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg); 
                  return;
               }

               for ( i = 0; i < dwCount; i++ ) {
 
                  ess = *(lpDependencies + i);

                  // Open the service
                  hDepService = OpenService( hSCM, ess.lpServiceName, 
                        SERVICE_STOP | SERVICE_QUERY_STATUS );
                  if ( !hDepService ) {
                     sprintf(errorMsg, "%s OnShutdown Error opening service <%s> <%d>", serviceName, 
                             ess.lpServiceName, GetLastError());
                     LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg); 
                     return;
                  }

                  __try {

                     // Send a shutdown code
                     if ( !ControlService( hDepService, SERVICE_CONTROL_SHUTDOWN,
                           &ss ) ) {
                        sprintf(errorMsg, "%s OnShutdown Error sending control to <%s> <%d>", 
                                serviceName, ess.lpServiceName, GetLastError());
                        LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg); 
                        return;
                     }

                     // Wait for the service to stop
                     while ( ss.dwCurrentState != SERVICE_STOPPED ) {

                        Sleep( ss.dwWaitHint );
                        if ( !QueryServiceStatus( hDepService, &ss ) ) {
                           sprintf(errorMsg, "%s OnShutdown Error error querying status of <%s> <%d>", 
                                serviceName, ess.lpServiceName, GetLastError());
                           LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);
                           return;
                        }

                        if ( ss.dwCurrentState == SERVICE_STOPPED )
                           break;

                        if ( GetTickCount() - dwStartTime > dwTimeout ) {
                           sprintf(errorMsg, "%s OnShutdown Timed out waiting for <%s> <%d>", 
                                serviceName, ess.lpServiceName, GetLastError());
                           LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);
                           return;
                        }
                     }

                  } __finally {

                     // Always release the service handle
                     CloseServiceHandle( hDepService );

                  }

               }

            } __finally {

               // Always free the enumeration buffer
               HeapFree( GetProcessHeap(), 0, lpDependencies );

            }
         }  // end if/else

         ::CloseServiceHandle(hService);
         ::CloseServiceHandle(hSCM);

      }  // end if (failure == FALSE)      
      else {
         sprintf(errorMsg, "%s OnShutdown Unable to stop any dependencies", serviceName);
         LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);
      }


      // Call the code to shutdown the service
      OnStop();         

      sprintf(errorMsg, "Leaving %s OnShutdown", serviceName);
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, errorMsg);    
}

// called when the service gets a user control message
BOOL ServiceClass::OnUserControl(DWORD dwOpcode)
{
    DebugMsg("ServiceClass::OnUserControl(%8.8lXH)", dwOpcode);
    return FALSE; // say not handled
}


///////////////////////////////////////////////////////////////////////////////////////////////
/// This function performs the main work of the service. 
// When this function returns the service has stopped.
void ServiceClass::Run()
{
    DebugMsg("Entering ServiceClass::Run()");

    while (isRunning) {
        DebugMsg("Sleeping...");
        Sleep(5000);
    }
	
    // nothing more to do
    DebugMsg("Leaving ServiceClass::Run()");
}