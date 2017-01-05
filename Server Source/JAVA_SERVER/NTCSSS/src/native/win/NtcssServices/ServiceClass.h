/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ServiceClass.h: interface for the ServiceClass class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>

#if !defined(AFX_SERVICECLASS_H__86DC1121_F3E0_11D5_B4E7_00C04F4D4DED__INCLUDED_)
#define AFX_SERVICECLASS_H__86DC1121_F3E0_11D5_B4E7_00C04F4D4DED__INCLUDED_

#include "ntsecapi.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// MessageId: EVMSG_INSTALLED
//
// MessageText:
//
//  The %1 service was installed.
//
#define EVMSG_INSTALLED                  0x00000064L

//
// MessageId: EVMSG_REMOVED
//
// MessageText:
//
//  The %1 service was removed.
//
#define EVMSG_REMOVED                    0x00000065L

//
// MessageId: EVMSG_NOTREMOVED
//
// MessageText:
//
//  The %1 service could not be removed.
//
#define EVMSG_NOTREMOVED                 0x00000066L

//
// MessageId: EVMSG_CTRLHANDLERNOTINSTALLED
//
// MessageText:
//
//  The control handler could not be installed.
//
#define EVMSG_CTRLHANDLERNOTINSTALLED    0x00000067L

//
// MessageId: EVMSG_STOPPED
//
// MessageText:
//
//  The service was stopped.
//
#define EVMSG_STOPPED                    0x0000006CL

//
// MessageId: EVMSG_BADREQUEST
//
// MessageText:
//
//  The service received an unsupported request.
//
#define EVMSG_BADREQUEST                 0x0000006AL

//
// MessageId: EVMSG_FAILEDINIT
//
// MessageText:
//
//  The initialization process failed.
//
#define EVMSG_FAILEDINIT                 0x00000068L

//
// MessageId: EVMSG_STARTED
//
// MessageText:
//
//  The service was started.
//
#define EVMSG_STARTED                    0x00000069L

//
// MessageId: EVMSG_DEBUG
//
// MessageText:
//
//  Debug: %1
//
#define EVMSG_DEBUG                      0x0000006BL



#define SERVICE_CONTROL_USER 128

class ServiceClass  
{
public:
	ServiceClass(const char* svcName, int majorVersion, int minorVersion);
    ServiceClass(const char* svcName, int majorVersion, int minorVersion, LPCTSTR dependenciesList);
	virtual ~ServiceClass();
    void init(const char* svcName, int majorVersion, int minorVersion, LPCTSTR dependenciesList);

    BOOL ParseStandardArgs(int argc, char* argv[]);
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
    void LogEvent(WORD wType, DWORD dwID,
                  const char* pszS1 = NULL,
                  const char* pszS2 = NULL,
                  const char* pszS3 = NULL);
    BOOL StartService();
    void DebugMsg(const char* pszFormat, ...);
    BOOL Initialize();
    void SetStatus(DWORD dwState);

    virtual BOOL OnInit();
    virtual void Run();
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnInterrogate();
    virtual void OnShutdown();
    virtual BOOL OnUserControl(DWORD dwOpcode);

    // static member functions
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);



    char serviceName[64];
    int majorVersion;
    int minorVersion;
    SERVICE_STATUS status; 
    SERVICE_STATUS_HANDLE hServiceStatus; 
    BOOL isRunning;

    // static data
    static ServiceClass* pThis; // nasty hack to get object ptr  

    // These members contain the paramets specified in the "Startup Parameters" 
    //  box of the services control dialog
    DWORD numArgs;
    LPTSTR* args;

    // A list of services that must be started before this one
    LPCTSTR dependenciesList;

private:
    HANDLE hEventSource;
    LPTSTR serviceUserName;
    LPTSTR serviceUserPassword;
    LPCWSTR domainName;    
    BOOL CreateServiceUser();
    void InitLsaString(PLSA_UNICODE_STRING, LPWSTR);
};

#endif // !defined(AFX_SERVICECLASS_H__86DC1121_F3E0_11D5_B4E7_00C04F4D4DED__INCLUDED_)
