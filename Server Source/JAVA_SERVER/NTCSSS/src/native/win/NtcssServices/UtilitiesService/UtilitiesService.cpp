/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// UtilitiesService.cpp: implementation of the UtilitiesService class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "UtilitiesService.h"

int main(int argc, char* argv[])
{	
   // Create the service object
   UtilitiesService service("NTCSS Utilities", 1, 0);    

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

UtilitiesService::UtilitiesService(const char* serviceName, int majorVersion, int minorVersion) 
  : ServiceClass(serviceName, majorVersion, minorVersion)
{ }  


void UtilitiesService::Run()
{
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo1, processInfo2, processInfo3;
   char longFilePath[_MAX_PATH];
   char szFilePath[_MAX_PATH];
   char commandLine[_MAX_PATH];
   HANDLE services[3];
   char pidFilename[_MAX_PATH];

   DebugMsg("Entering UtilitiesService::Run()");


   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(szFilePath));   
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  


   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  

   ::ZeroMemory(services, sizeof(services));
   

   // Startup the logging daemon
   sprintf(commandLine, "%s\\..\\ntcss_logd.bat", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo1) == FALSE) {
      DebugMsg("Failed to create log daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create log daemon");
      return;
   }

   DebugMsg("Created log daemon");  
   services[0]= processInfo1.hProcess;

   // For debugging purposes, store the PID value
   sprintf(pidFilename, "%s\\..\\..\\logs\\logd.pid", szFilePath);
   StorePID(processInfo1.dwProcessId, pidFilename);

   // Startup the printer list daemon
   sprintf(commandLine, "%s\\..\\prt_listd.bat", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo2) == FALSE) {
      DebugMsg("Failed to create printer list daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create printer list daemon");
      return;
   }

   DebugMsg("Created print list daemon");
   services[1]= processInfo2.hProcess;

   // For debugging purposes, store the PID value
   sprintf(pidFilename, "%s\\..\\..\\logs\\prtd.pid", szFilePath);
   StorePID(processInfo2.dwProcessId, pidFilename);

   // Startup the FTP daemon
   sprintf(commandLine, "%s\\..\\ntcss_ftpd.bat", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo3) == FALSE) {
      DebugMsg("Failed to create FTP daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create FTP daemon");
      return;
   }

   DebugMsg("Created FTP daemon");
   services[2]= processInfo3.hProcess;

   // For debugging purposes, store the PID value
   sprintf(pidFilename, "%s\\..\\..\\logs\\ftpd.pid", szFilePath);
   StorePID(processInfo3.dwProcessId, pidFilename);

   // Wait for the logging, printer list, and FTP daemons to finish
   if (WaitForMultipleObjects(3, services, TRUE, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for process completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for process completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(processInfo1.hProcess);
   CloseHandle(processInfo1.hThread);
   CloseHandle(processInfo2.hProcess);
   CloseHandle(processInfo2.hThread);
   CloseHandle(processInfo3.hProcess);
   CloseHandle(processInfo3.hThread);

   // nothing more to do
   DebugMsg("Leaving UtilitiesService::Run()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving UtilitiesService::Run()");
}

void UtilitiesService::OnStop()
{   
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo1, processInfo2, processInfo3;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];
   char commandLine[_MAX_PATH];
   HANDLE services[3];

   DebugMsg("UtilitiesService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"UtilitiesService::OnStop()");

   SetStatus(SERVICE_STOP_PENDING);   

   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath));  
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));   

   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE; 
   startInfo.lpDesktop = ""; // This is to prevent "Kernel32.dll initialization failure" message
                             //  when this service is stopped on system shutdown 

   ::ZeroMemory(services, sizeof(services));
      
 
   // Send the stop message to the printer list daemon
   sprintf(commandLine, "%s\\..\\prt_listd.bat -stop", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo2) == FALSE) {
      DebugMsg("Failed to create process to stop printer list daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop printer list daemon");
      return;
   }

   DebugMsg("Created process to stop print list daemon");
   services[1]= processInfo2.hProcess;

   // Send the stop message to the FTP daemon
   sprintf(commandLine, "%s\\..\\ntcss_ftpd.bat -stop", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo3) == FALSE) {
      DebugMsg("Failed to create process to stop FTP daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop FTP daemon");
      return;
   }

   DebugMsg("Created process to stop FTP daemon");
   services[2]= processInfo3.hProcess;

   // Wait for a few seconds for the FTP and printer list daemons to
   //  log that they are shutting down. It may take a few seconds for 
   //  the daemons to "wake up" and realize they are being requested to shutdown
   Sleep(5000);

   // Send the stop message to the logging daemon
   sprintf(commandLine, "%s\\..\\ntcss_logd.bat -stop", szFilePath);
   if (CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo1) == FALSE) {
      DebugMsg("Failed to create process to stop log daemon");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop log daemon");
      return;
   }

   DebugMsg("Created process to stop log daemon");
   services[0]= processInfo1.hProcess;

   // Wait for the "stop" processes to finish
   if (WaitForMultipleObjects(3, services, TRUE, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for process completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for process completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(processInfo1.hProcess);
   CloseHandle(processInfo1.hThread);
   CloseHandle(processInfo2.hProcess);
   CloseHandle(processInfo2.hThread);
   CloseHandle(processInfo3.hProcess);
   CloseHandle(processInfo3.hThread);

   // nothing more to do
   DebugMsg("Leaving UtilitiesService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving UtilitiesService::OnStop()");

   isRunning = FALSE;
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);

}

// Stores the given pidValue in the given file
void UtilitiesService::StorePID(DWORD pidValue, LPCTSTR filename)
{
   FILE* file;

   file = fopen(filename, "w");
   fprintf(file, "%d", pidValue);
   fclose(file);
}
