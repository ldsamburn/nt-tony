/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ServerService.cpp: implementation of the ServerService class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ServerService.h"

int main(int argc, char* argv[])
{	
   // The buffer below separates the strings with a
   // zero character and ends with 2 zero characters.
   LPCTSTR dependenciesList = __TEXT("NTCSS Utilities\0NTCSS Database\0");

   // Create the service object
   ServerService service("NTCSS Server", 1, 0, dependenciesList);    

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

ServerService::ServerService(const char* serviceName, int majorVersion, int minorVersion, LPCTSTR dpndList) 
  : ServiceClass(serviceName, majorVersion, minorVersion, dpndList)
{ }  


void ServerService::Run()
{
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   PROCESS_INFORMATION webProcessInfo;
   PROCESS_INFORMATION webStopProcessInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];
   char szKey[256];
   HKEY hKey = NULL;

   DebugMsg("Entering ServerService::Run()");


   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath)); 
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  
   strcat(szFilePath, "\\..\\ntcss_start.bat");

   // Add any parameters specified in the "Startup Parameters" dialog to the end 
   //  of the command line
   if (numArgs > 1) {
      for (DWORD i = 1; i < numArgs; i++) {
         strcat(szFilePath, " ");
         strcat(szFilePath, args[i]);
      }
   }

   // If this service was started from the script (ntcsss_svc_start), it would have placed any command line
   //  parameters it received into the registry. So look there for any parameters   
   sprintf(szKey, "SYSTEM\\CurrentControlSet\\Services\\%s\\Parameters", serviceName);   
   if (RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey) == ERROR_SUCCESS) {

      // Read the parameters from the registry
      DWORD type;
      char value[256];      
      DWORD valueLen = 256;    
      if (RegQueryValueEx(hKey, "AppParameters", NULL, &type, (LPBYTE)value, &valueLen) == ERROR_SUCCESS) {
         strcat(szFilePath, " ");
         strcat(szFilePath, value);
      }


      RegCloseKey(hKey);


      // Remove the parameters from the registry. If these aren't deleted each time,
      //  then every time the service is started, they will be passed to it which
      //  may not be desirable. The parameter registry key will be recreated every
      //  time the service start script is called.
      if (RegDeleteKey(HKEY_LOCAL_MACHINE, szKey) != ERROR_SUCCESS)
         DebugMsg("Could not remove startup parameters");  

   }



   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  

   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create server");
      return;
   }

   DebugMsg("Created server");

   // Start the web server
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  
   strcat(szFilePath, "\\..\\..\\web\\bin\\startserver.bat");

   // CREATE_NEW_PROCESS_GROUP flag needed to be able to kill startup
   //  script and the JVM it starts up
   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startInfo, 
                     &webProcessInfo) == FALSE) {
      DebugMsg("Failed to create web server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create web server");
      return;
   }

   DebugMsg("Created web server");


   // Wait for the NTCSS server to die
   if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for server completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for server completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);

   DebugMsg("Server stopped. Now stopping web server...");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Server stopped. Now stopping web server...");


   // Stop the web server
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath));  
   strcat(szFilePath, "\\..\\..\\web\\bin\\stopserver.bat");

   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, 
                     &webStopProcessInfo) == FALSE) {
      DebugMsg("Failed to create web server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create web server");
      return;
   }


   // Close process and thread handles. 
   CloseHandle(webStopProcessInfo.hProcess);
   CloseHandle(webStopProcessInfo.hThread);

   // Wait for the web server to die
   if (WaitForSingleObject(webProcessInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for web server completion failed");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Waiting for web server completion failed");
      return;
   } 

   // Close process and thread handles. 
   CloseHandle(webProcessInfo.hProcess);
   CloseHandle(webProcessInfo.hThread);

   DebugMsg("Web server stopped.");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Web server stopped.");


   // nothing more to do
   DebugMsg("Leaving ServerService::Run()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving ServerService::Run()");
}

void ServerService::OnStop()
{   
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char szFilePath[_MAX_PATH];
   char longFilePath[_MAX_PATH];
   struct stat statBuf;

   DebugMsg("ServerService::OnStop()"); 
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"ServerService::OnStop()");      

   // Convert a path that might have spaces in it, to its "short path" form
   ::GetModuleFileName(NULL, longFilePath, sizeof(longFilePath));
   ::GetShortPathName(longFilePath, szFilePath, sizeof(szFilePath)); 
   strcat(szFilePath, "\\..\\ntcss_stop.bat");

   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  
   startInfo.lpDesktop = ""; // This is to prevent "Kernel32.dll initialization failure" message
                             //  when this service is stopped on system shutdown
 
   if (CreateProcess(NULL, szFilePath, NULL, NULL, FALSE, 
                     0, NULL, NULL, &startInfo, &processInfo) == FALSE) {
      DebugMsg("Failed to create process to stop server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Failed to create process to stop server");
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

   // Check for the existence of the "stop" file
   sprintf(szFilePath, "%s\\..\\..\\logs\\STOP_FILE", szFilePath);   
   if (stat(szFilePath, &statBuf) == 0) {

      // This service can stop now
      SetStatus(SERVICE_STOP_PENDING);

      isRunning = FALSE;
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
   }
   else {
      DebugMsg("Could not find stop file");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Could not find stop file");
   }

   // nothing more to do
   DebugMsg("Leaving ServerService::OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving ServerService::OnStop()");
}

