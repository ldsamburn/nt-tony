/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ControlPanelService.cpp: implementation of the CControlPanelService class.
//
//////////////////////////////////////////////////////////////////////

#include "ControlPanelService.h"

int main(int argc, char* argv[])
{	
   // The buffer below separates the strings with a
   // zero character and ends with 2 zero characters.
   LPCTSTR dependenciesList = __TEXT("NTCSS Utilities\0NTCSS Database\0");

   // Create the service object
   CControlPanelService service("NTCSS Control Panel", 1, 0, dependenciesList);    

   // Parse for standard arguments (install, uninstall, version etc.)
   if (!service.ParseStandardArgs(argc, argv)) {

      // Didn't find any standard args so start the service      
      service.StartService();
   }


   // When we get here, the service has been stopped
   return service.status.dwWin32ExitCode;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControlPanelService::CControlPanelService(const char* serviceName, int majorVersion, int minorVersion, LPCTSTR dpndList) 
  : ServiceClass(serviceName, majorVersion, minorVersion, dpndList)
{ }  


void CControlPanelService::Run()
{
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];

   DebugMsg("Entering CControlPanelService::Run()");


   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath)); 
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  
   strcat(szFilePath, "\\..\\CPserver_init.bat");


   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  

   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create control panel server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create control panel server");
      return;
   }

   DebugMsg("Created control panel");

   if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for control panel server completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for control panel server completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);

   // nothing more to do
   DebugMsg("Leaving CControlPanelService::Run()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving CControlPanelService::Run()");
}

void CControlPanelService::OnStop()
{   
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];

   DebugMsg("CControlPanelService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"CControlPanelService::OnStop()");

   SetStatus(SERVICE_STOP_PENDING);

   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath));
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath)); 
   strcat(szFilePath, "\\..\\CPserver_halt.bat");

   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  
   startInfo.lpDesktop = ""; // This is to prevent "Kernel32.dll initialization failure" message
                             //  when this service is stopped on system shutdown
 
   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create process to stop control panel server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop control panel server");
      return;
   }

   if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for process completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for process completion failed");
      return;
   }

   // Close process and thread handles. 
   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);

   // nothing more to do
   DebugMsg("Leaving CControlPanelService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving CControlPanelService::OnStop()");

   isRunning = FALSE;
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
}