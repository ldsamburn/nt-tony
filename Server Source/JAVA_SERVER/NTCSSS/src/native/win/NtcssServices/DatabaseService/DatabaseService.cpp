/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DatabaseService.cpp: implementation of the DatabaseService class.
//
//////////////////////////////////////////////////////////////////////

#include "DatabaseService.h"

int main(int argc, char* argv[])
{	
   // Create the service object
   DatabaseService service("NTCSS Database", 1, 0);    

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

DatabaseService::DatabaseService(const char* serviceName, int majorVersion, int minorVersion) 
  : ServiceClass(serviceName, majorVersion, minorVersion)
{ }  


void DatabaseService::Run()
{
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];

   DebugMsg("Entering DatabaseService::Run()");


   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath)); 
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  
   strcat(szFilePath, "\\..\\ntcssdb_start.bat");


   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  

   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create database daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create database daemon");
      return;
   }

   DebugMsg("Created database daemon");

   if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for database daemon completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for database daemon completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);

   // nothing more to do
   DebugMsg("Leaving DatabaseService::Run()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving DatabaseService::Run()");
}

void DatabaseService::OnStop()
{   
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];

   DebugMsg("DatabaseService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"DatabaseService::OnStop()");

   SetStatus(SERVICE_STOP_PENDING);

   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath));
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath)); 
   strcat(szFilePath, "\\..\\ntcssdb_stop.bat");

   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  
   startInfo.lpDesktop = ""; // This is to prevent "Kernel32.dll initialization failure" message
                             //  when this service is stopped on system shutdown
 
   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create process to stop database daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop database daemon");
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
   DebugMsg("Leaving DatabaseService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving DatabaseService::OnStop()");

   isRunning = FALSE;
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
}
