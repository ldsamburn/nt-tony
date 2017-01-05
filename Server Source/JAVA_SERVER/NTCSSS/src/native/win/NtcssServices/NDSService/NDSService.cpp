/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NDSService.cpp: implementation of the NDSService class.
//
//////////////////////////////////////////////////////////////////////

#include "NDSService.h"

#define JVM_NOT_INIT   0 
#define JVM_INIT_ERROR 1
#define JVM_INIT_OK    2

int main(int argc, char* argv[])
{	
   // The buffer below separates the strings with a
   // zero character and ends with 2 zero characters.
   // This service depends on the FTP daemon because it could potentially be
   //  ftping files from itself this host is the NTCSS master. This service
   //  depends on the database engine so that it can call the 
   //  DataLib.getMasterServer routine.
   LPCTSTR dependenciesList = __TEXT("NTCSS Utilities\0NTCSS Database\0");

   // Create the service object
   NDSService service("NTCSS NDS Server", 1, 0, dependenciesList);    

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

NDSService::NDSService(const char* serviceName, int majorVersion, int minorVersion, LPCTSTR dpndList) 
  : ServiceClass(serviceName, majorVersion, minorVersion, dpndList)
{ 
   char longPath[_MAX_PATH];

   vm  = (JavaVM *)0;
   env = (JNIEnv*)0;
   jvmInitializationState = JVM_NOT_INIT;

   // Determine the NTCSSS root path  
   // Convert a path that might have spaces in it, to its "short path" form
   memset(ntcssRootPath, '\0', _MAX_PATH);
   ::GetModuleFileName(NULL, longPath, _MAX_PATH);
   ::GetShortPathName(longPath, ntcssRootPath, _MAX_PATH); 
   strcat(ntcssRootPath, "\\..\\..");

   sprintf(ntcssRootPathLong, "%s\\..\\..", longPath);

   slapdProcessInfo.hProcess = NULL;
   slurpdProcessInfo.hProcess = NULL;
} 

/*****************************************************************************/

void NDSService::Run()
{
   HANDLE processes[2]; 

   DebugMsg("Entering NDSService::Run()");  

   // Get the list of LDAP servers
   if (GetServerList() == false) {
      DebugMsg("Could not get LDAP server list");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Could not get LDAP server list");
      return;
   }   

   // If this host isn't in the list of hosts in the LDAP server list, 
   //  then the LDAP processes don't need to run on this host
   if (IsServer() == false) {

      DebugMsg("This host is not a LDAP server");
      LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"This host is not a LDAP server");

      // If database files exist, remove them. This will force a fresh download
      //  of the database files when this server is designated to come up again
      if (IsDatabaseInstalled() == true) {
         DebugMsg("Removing LDAP database");
         RemoveDatabase();            
      }

      // Pause for a few seconds to make sure the Service Control Manager
      //  thinks we're up and running. If this isn't done, an error is
      //  always reported because the SCM thinks that this service failed
      //  starting up since we turn around and stop so quickly
      //  (which of course isn't the case in this instance).
      Sleep(5000);

      OnStop();      
   }
   else {  // This host must be a LDAP server

      // Get the LDAP *.conf file from the master
      if (GetConfFile() == false) {
         DebugMsg("Could not get conf file");
         LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                  "Could not get conf file");
         return;
      }

      // If there is no LDAP database on this host, it needs to be
      //  retrieved and applied to this server. Otherwise, the slapd 
      //  process can just be started.
      if (IsDatabaseInstalled() == false) {

         // Get the database
         DebugMsg("Retreiving LDAP database...");
         if (RetrieveDatabase() == false) {
            DebugMsg("Error retreiving database");
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                     "Error retreiving database");
            return;
         }

         // Apply database
         DebugMsg("Applying LDAP database...");
         if (ApplyDatabase() == false) {
            DebugMsg("Error applying database");
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                     "Error applying database");
            return;
         }
      }
      
      // Start the slapd process
      DebugMsg("Starting slapd process...");
      if (StartSlapdProcess() == false) {
         DebugMsg("Slapd process start failed");
         LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                  "Slapd process start failed");
         return;
      }

      // If this host is the LDAP master, start the slurpd process
      if (isLDAPMaster == true) {

         // Start the slurpd process
         if (StartSlurpdProcess() == false) {
            DebugMsg("Slurpd process start failed");
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                  "Slurpd process start failed");
            StopSlapdProcess();
            return;
         }
      }      

      DebugMsg("Startup complete");
            
      // Wait for the processes to die
      if (isLDAPMaster == false) {

         if (WaitForSingleObject(slapdProcessInfo.hProcess, INFINITE) == WAIT_FAILED) { 
            DebugMsg("Waiting for slapd failed");
            LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                     "Waiting for slapd failed");            
            return;
         }      
      }
      else {
          processes[0] = slapdProcessInfo.hProcess;
          processes[1] = slurpdProcessInfo.hProcess;

          // Wait for the ldap processes to finish
          if (WaitForMultipleObjects(2, processes, TRUE, INFINITE) == WAIT_FAILED) {
             DebugMsg("Waiting for process completion failed");
             LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
                      "Waiting for process completion failed");
             return;
          }   
      }

      // Close process and thread handles. 
      if (slapdProcessInfo.hProcess != NULL)
         CloseHandle(slapdProcessInfo.hProcess);
      if (slapdProcessInfo.hThread != NULL)
         CloseHandle(slapdProcessInfo.hThread);

      if (slurpdProcessInfo.hProcess != NULL)
         CloseHandle(slurpdProcessInfo.hProcess);
      if (slurpdProcessInfo.hThread != NULL)
         CloseHandle(slurpdProcessInfo.hThread);

   }  // End "if (IsServer() == false)"


   // nothing more to do
   DebugMsg("Leaving Run()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, 
				"Leaving Run()");
}

/*****************************************************************************/
// FTPs the LDAP server list from the NTCSS master
bool NDSService::GetServerList()
{
   jclass cls;
   jmethodID mid;   
   jboolean isText = JNI_FALSE;  
   char localFile[_MAX_PATH];    
   char remoteFile[_MAX_PATH]; 
   char ntcssMasterName[100];
   char debugMsg[100];

   // Get the name of the NTCSS master
   if (GetMasterName(ntcssMasterName) == false) {
      DebugMsg("Cannot determine master name");
      return false;
   }

   if (InitializeJVM() == false) {
      DebugMsg("JVM initialization error");
      return false;
   } 
      
   cls = env->FindClass("COMMON/FTP");
   if (cls == NULL) {
      DebugMsg("Can't find class FTP");
      return false;
   }    

   mid = env->GetStaticMethodID(cls, "get", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z");
   if (mid == NULL) {
      DebugMsg("Can't find method get");
      return false;
   }

   sprintf(debugMsg, "Retreiving server list from <%s>...", ntcssMasterName);
   DebugMsg(debugMsg);   

   sprintf(localFile, "%s\\ldap\\runtime\\ldapservers", ntcssRootPath);   
   sprintf(remoteFile, "/h/NTCSSS/ldap/ldapservers");
   if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(ntcssMasterName), env->NewStringUTF(remoteFile),
                                    env->NewStringUTF(localFile), isText) == JNI_FALSE) {
      DebugMsg("Can't get server list");
      return false;
   }

   return true;
}

/*****************************************************************************/
// Attempts to get the name of the NTCSS master
bool NDSService::GetMasterName(LPTSTR master_server_name)
{
   jclass cls, clsSB;
   jmethodID mid, midSB;      
   jobject buffer;    
   jstring strValue; 
   jfieldID constantID;
   jstring constantStr;                

   master_server_name[0] = 0;

   // First try to get the NTCSS master name from the local database

   if (InitializeJVM() == false) {
      DebugMsg("JVM initialization error");
      return false;
   } 
 
   // Find the DataLib.getMasterServer method
   cls = env->FindClass("ntcsss/libs/DataLib");
   if (cls == NULL) {
      DebugMsg("Can't find class DataLib\n");
      return false;
   } 

   mid = env->GetStaticMethodID(cls, "getMasterServer", "(Ljava/lang/StringBuffer;)I");
   if (mid == NULL) {
      DebugMsg("Can't find method getMasterServer\n");
      return false;
   }

   // Create the StringBuffer object that the result will be placed into
   clsSB = env->FindClass("java/lang/StringBuffer");
   if (clsSB == NULL) {
      DebugMsg("Can't find class StringBuffer\n");
      return false;
   }

   midSB = env->GetMethodID(clsSB, "<init>", "()V");
   if (midSB == NULL) {
      DebugMsg("Can't find StringBuffer constructor\n");
      return false;
   }

   buffer = env->NewObject(clsSB, midSB);   
   if (env->CallStaticIntMethod(cls, mid, buffer) >= 0) {      

      // Call the StringBuffer.toString method to get the data
      midSB = env->GetMethodID(clsSB, "toString", "()Ljava/lang/String;");
      if (midSB == NULL) {
         DebugMsg("Can't find StringBuffer.toString()\n");
         return false;
      }
      strValue = (jstring)env->CallObjectMethod(buffer, midSB);
   
      sprintf(master_server_name, "%s", env->GetStringUTFChars(strValue, NULL)); 

      return true;
   }


   // If the above fails, just retreive the default value from the properties file
        
   cls = env->FindClass("ntcsss/libs/Constants");
   if (cls == NULL) {
      DebugMsg("Can't find class Constants\n");
      return false;
   } 
   
   constantID = env->GetStaticFieldID(cls, "NTCSS_DEFAULT_MASTER", "Ljava/lang/String;"); 
   if (constantID == NULL) {
      DebugMsg("Can't find default master constant\n");
      return false;
   }

   constantStr = (jstring)env->GetStaticObjectField(cls, constantID);
   if (constantStr != NULL) {
      sprintf(master_server_name, "%s", env->GetStringUTFChars(constantStr, NULL));
      return true;   
   }  

   DebugMsg("Cannot determine master server");

   return false; 
}

/*****************************************************************************/
// Initializes the Java VM so that calls can be made into our java code
bool NDSService::InitializeJVM()
{
   int nError = 0;
   JavaVMInitArgs vm_args;
   JavaVMOption options[4];
   char classpath[_MAX_PATH];   
   jclass cls;
   jmethodID mid;         
   char propertiesFile[_MAX_PATH];
   char jvmDLL[_MAX_PATH];   
   char debugMsg[100];
   HMODULE libraryHandle;
   jint (JNICALL *CreateJavaVM)(JavaVM **pvm, void **env, void *args);


   // Check to see if initialization has already been done. If there
   //  was an error trying to initialize before, don't try again.
   if (jvmInitializationState == JVM_INIT_ERROR)
      return false;
   else if (jvmInitializationState == JVM_INIT_OK)
      return true;


   // Load the JVM DLL. Can remove this if the jvm.dll is specified in PATH env variable.
   sprintf(jvmDLL, "%s\\jre\\bin\\classic\\jvm.dll", ntcssRootPath);
   if ((libraryHandle = LoadLibrary(jvmDLL)) == NULL) {
      DebugMsg("Can't load library");
      jvmInitializationState = JVM_INIT_ERROR;
      return false;
   }


   // Create the class path    
   sprintf(classpath, "-Djava.class.path=%s\\..\\classes;%s\\libs\\ntcsss.jar;%s\\libs\\common.jar;%s\\libs\\mkjdbc.jar", 
           ntcssRootPath, ntcssRootPath, ntcssRootPath, ntcssRootPath);          

   // Set up the JVM options 
   options[0].optionString = classpath;                   // user classes 
//   options[1].optionString = "-Djava.compiler=NONE";    // disable JIT   
//   options[2].optionString = "-Djava.library.path=d:\\jdk1.3.1\\jre\\lib\\rt.jar;d:\\jdk1.3.1\\jre\\bin;d:\\jdk1.3.1\\jre\\lib";// "-Djava.library.path=c:\\JDK1.2\\jre\\lib\\rt.jar";  // set native library path
//   options[3].optionString = "-verbose:jni,gc,class";   // print JNI-related messages 
   options[1].optionString = "-Xrs";  // This option is needed due to a known bug in the 1.3.1 JVM, which
                                      //  causes it to exit when running in the context of a windows service and
                                      //  the user logs out
                                      //  See "Bug 4323062: Any Windows NT Service embedding Java VM aborts when user logs out " at 
                                      //   http://java.sun.com/j2se/1.3/relnotes.html

   vm_args.version = JNI_VERSION_1_2;
   vm_args.options = options;
   vm_args.nOptions = 2;

   // If ignoreUnrecognized is JNI_TRUE, JNI_CreateJavaVM ignore all
   //  unrecognized option strings that begin with "-X" or "_". If
   //  ignoreUnrecognized is JNI_FALSE, JNI_CreateJavaVM returns
   //  JNI_ERR as soon as it encounters any unrecognized option strings
   vm_args.ignoreUnrecognized = JNI_TRUE; 
    

   // Try to create the JVM
// nError = JNI_CreateJavaVM(&vm, (void**)&env, &vm_args);  // Use this if jvm.dll isn't dynamically loaded
   CreateJavaVM = (jint (JNICALL *)(JavaVM **,void **, void *))GetProcAddress(libraryHandle, "JNI_CreateJavaVM");
   if (CreateJavaVM == 0)
      nError = -1;
   else
      nError = CreateJavaVM(&vm, (void**)&env, &vm_args);
   if (nError < 0)  {
      sprintf(debugMsg, "Error: %d - JNI_CreateJavaVM()", nError);  
      DebugMsg(debugMsg);
      jvmInitializationState = JVM_INIT_ERROR;         
      return false;
   }

   // Try to load the NTCSSS properties file
   cls = env->FindClass("COMMON/PropertiesLoader");
   if (cls == NULL) {
      DebugMsg("Can't find PropertiesLoader class");      
      jvmInitializationState = JVM_INIT_ERROR;
      return false;
   }
 
   mid = env->GetStaticMethodID(cls, "load", "(Ljava/lang/String;)Z");
   if (mid == NULL) {
      DebugMsg("Can't find PropertiesLoader load method");
      jvmInitializationState = JVM_INIT_ERROR;
      return false;
   }

   sprintf(propertiesFile, "%s\\Ntcss.properties", ntcssRootPath);
   if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(propertiesFile)) == JNI_FALSE) {   
      DebugMsg("Error loading properties file");
      jvmInitializationState = JVM_INIT_ERROR;
      return false;
   }

   jvmInitializationState = JVM_INIT_OK;
   return true;
}

/*****************************************************************************/

void NDSService::OnStop()
{       
   DebugMsg("OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, "OnStop()");
   SetStatus(SERVICE_STOP_PENDING);   

   // Stop the ldap processes   
   StopSlapdProcess(); 
   if (isLDAPMaster == true)  
      StopSlurpdProcess();

   // nothing more to do
   DebugMsg("Leaving OnStop()");
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_DEBUG, "Leaving OnStop()");

   isRunning = FALSE;
   LogEvent(EVENTLOG_INFORMATION_TYPE, EVMSG_STOPPED);
}

/*****************************************************************************/
// Look to see if this host is in the LDAP configuration file to see if this
//  host should be a LDAP server
bool NDSService::IsServer()
{
   FILE* file;
   char configFile[_MAX_PATH];
   const int BUF_SIZE = 100;
   char buffer[BUF_SIZE];
   int strLength;
   char localHostname[100];
   bool found = false;

   if (gethostname(localHostname, 100) != 0) {
      DebugMsg("Can't get name of this host");
      return false;
   }

   sprintf(configFile, "%s\\ldap\\runtime\\ldapservers", ntcssRootPath);
   if ((file = fopen(configFile, "r")) == NULL) {
      DebugMsg("Can't open configuration file");
      return false;
   }   

   while (fgets(buffer, BUF_SIZE, file) != NULL) {

      // Remove carriage returns and line feeds
      strLength = strlen(buffer);
      for (int i = 0; i < strLength; i++)        
         if ((buffer[i] == 10) || (buffer[i] == 13))
            buffer[i] = '\0';

      // Compare (case-insensitively) this entry with local hostname.
      //  The first character in the line is the slave/master designation
      if (_stricmp(&buffer[1], localHostname) == 0) {
         found = true;
         break;
      }
   }

   // Set the flag which specifies whether this host is the LDAP master server
   if (found == true) {
      if ((buffer[0] == 'M') || (buffer[0] == 'm'))
         isLDAPMaster = true;
      else
         isLDAPMaster = false;
   }

   fclose(file);

   return found;
}

/*****************************************************************************/
// Checks to see if the LDAP database exists
bool NDSService::IsDatabaseInstalled()
{
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;  
  char dbDir[_MAX_PATH];    
  bool fileFound = false;

  sprintf(dbDir, "%s\\ldap\\runtime\\openldap-ldbm\\*", ntcssRootPath);  
  hFind = FindFirstFile(dbDir, &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
     DebugMsg("Invalid File Handle");     
     return false;
  }
  else {

     // It appears that the first call to FindNextFile just returns what
     //  was returned from the call to FindFirstFile.
     while (FindNextFile(hFind, &FindFileData) != 0) {

        if ((strcmp(FindFileData.cFileName, ".") == 0)  ||
            (strcmp(FindFileData.cFileName, "..") == 0) ||
            (strcmp(FindFileData.cFileName, "dn2id.dbb") == 0))  // This file doesn't appear 
           continue;                                             //  to be an actual part of the DB

        fileFound = true;
        break;
     }

     FindClose(hFind);     
  }  

  return fileFound;
}

/*****************************************************************************/
// Removes the local LDAP database
void NDSService::RemoveDatabase()
{
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;  
  char dbDir[_MAX_PATH];
  char file[_MAX_PATH];
  char debugMsg[100];

  sprintf(dbDir, "%s\\ldap\\runtime\\openldap-ldbm\\", ntcssRootPath);
  sprintf(file, "%s*", dbDir);
  hFind = FindFirstFile(file, &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE)
     DebugMsg("Invalid File Handle");     

  else {

     // It appears that the first call to FindNextFile just returns what
     //  was returned from the call to FindFirstFile.
     while (FindNextFile(hFind, &FindFileData) != 0) {

        if ((strcmp(FindFileData.cFileName, ".") == 0)  ||
            (strcmp(FindFileData.cFileName, "..") == 0))
           continue;

        sprintf(file, "%s%s", dbDir, FindFileData.cFileName);
        if (DeleteFile(file) == 0) {
           sprintf(debugMsg, "Could not delete file <%s> <%d>", file, GetLastError());   
           DebugMsg(debugMsg);
        }
     }

     FindClose(hFind);     
  }  

}

/*****************************************************************************/
// Retreives the LDAP *.conf file from the NTCSS master
bool NDSService::GetConfFile()
{
   jclass cls;
   jmethodID mid;   
   jboolean isText = JNI_FALSE;  
   char localFile[_MAX_PATH];    
   char localTmpFile[_MAX_PATH]; 
   char remoteFile[_MAX_PATH]; 
   char ntcssMasterName[100];
   char debugMsg[100];
   FILE* reader;
   FILE* writer;
   const int BUF_SIZE = 200;
   char buffer[BUF_SIZE];
   char path[_MAX_PATH];
   int strLen = strlen(ntcssRootPathLong);
   char keyWithSpace[20];
   char keyWithTab[20];
   int keyLen;
   bool found = false;

   // Get the name of the NTCSS master
   if (GetMasterName(ntcssMasterName) == false) {
      DebugMsg("Cannot determine master name");
      return false;
   }

   if (InitializeJVM() == false) {
      DebugMsg("JVM initialization error");
      return false;
   } 
      
   cls = env->FindClass("COMMON/FTP");
   if (cls == NULL) {
      DebugMsg("Can't find class FTP");
      return false;
   }    

   mid = env->GetStaticMethodID(cls, "get", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z");
   if (mid == NULL) {
      DebugMsg("Can't find method get");
      return false;
   }

   if (isLDAPMaster == true) {
      sprintf(debugMsg, "Retrieving master.conf from <%s>...", ntcssMasterName);
      sprintf(remoteFile, "/h/NTCSSS/ldap/master.conf");
   }
   else {
      sprintf(debugMsg, "Retrieving slave.conf from <%s>...", ntcssMasterName);
      sprintf(remoteFile, "/h/NTCSSS/ldap/slave.conf");
   }

   // Get the *.conf file
   DebugMsg(debugMsg);   


   sprintf(localTmpFile, "%s\\ldap\\runtime\\slapd.conf_org", ntcssRootPath);      
   if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(ntcssMasterName), env->NewStringUTF(remoteFile),
                                    env->NewStringUTF(localTmpFile), isText) == JNI_FALSE) {
      DebugMsg("Can't get conf file");
      return false;
   } 


   // Modify the conf file by adding the local ldap directory 
    

   sprintf(localFile, "%s\\ldap\\runtime\\slapd.conf", ntcssRootPath);      

   reader = fopen(localTmpFile, "r");
   writer = fopen(localFile, "w+");

  
   // Change all the backward slashes in the root path to forward slashes
   memset(path, '\0', _MAX_PATH);
   for (int i = 0; i < strLen; i++) {
      if (ntcssRootPathLong[i] == '\\')
         path[i] = '/';
      else
         path[i] = ntcssRootPathLong[i];
   }

   // Copy the contents of the temporary conf file to the final conf file.
   //  Look at each line and see if this line contains the "directory" string
   sprintf(keyWithSpace, "directory ");
   sprintf(keyWithTab, "directory\t");
   keyLen = strlen(keyWithSpace);
   while (fgets(buffer, BUF_SIZE, reader) != NULL) {

      // Check to see if this line contains the "directory" string
      if ((strncmp(buffer, keyWithSpace, keyLen) == 0) ||
          (strncmp(buffer, keyWithTab, keyLen) == 0)) {
         found = true;
         
         // Write out the path to the database directory. Quotes
         //  are placed around the whole string in case the path 
         //  contains a long filename like "Program Files"
         fprintf(writer, "directory\t\"%s/ldap/runtime/openldap-ldbm\"\n", path);
      }
      else
         fprintf(writer, "%s", buffer);
   }

   fclose(reader);
   fclose(writer); 

   if (found == false) {
      DebugMsg("Could not find directory entry in conf file");
      return false;
   }   
   else
      return true;
}

/*****************************************************************************/
// Retreives the LDAP database from the NTCSS master
bool NDSService::RetrieveDatabase()
{
   jclass cls;
   jmethodID mid;   
   jboolean isText = JNI_FALSE;  
   char localFile[_MAX_PATH]; 
   char remoteFile[_MAX_PATH]; 
   char ntcssMasterName[100];
   char debugMsg[100];   

   // Get the name of the NTCSS master
   if (GetMasterName(ntcssMasterName) == false) {
      DebugMsg("Cannot determine master name");
      return false;
   }

   if (InitializeJVM() == false) {
      DebugMsg("JVM initialization error");
      return false;
   }     
   
   cls = env->FindClass("COMMON/FTP");
   if (cls == NULL) {
      DebugMsg("Can't find class FTP");
      return false;
   }    

   mid = env->GetStaticMethodID(cls, "get", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z");
   if (mid == NULL) {
      DebugMsg("Can't find method get");
      return false;
   }

   sprintf(debugMsg, "Retreiving database from <%s>...", ntcssMasterName);
   DebugMsg(debugMsg);


   sprintf(localFile, "%s\\ldap\\runtime\\save.ldif", ntcssRootPath);   
   sprintf(remoteFile, "/h/NTCSSS/ldap/save.ldif");
   if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(ntcssMasterName), env->NewStringUTF(remoteFile),
                                    env->NewStringUTF(localFile), isText) == JNI_FALSE) {
      DebugMsg("Can't get database");
      return false;
   }          
            
   return true;     
}

/*****************************************************************************/

bool NDSService::ApplyDatabase()
{   
   STARTUPINFO startInfo;
   PROCESS_INFORMATION processInfo;
   char command[_MAX_PATH];               
   char startupDirectory[_MAX_PATH];

   // Create the process that will apply the ldif bundle to the database
   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  
   sprintf(command, "%s\\ldap\\runtime\\slapadd.exe -l %s\\ldap\\runtime\\save.ldif -f %s\\ldap\\runtime\\slapd.conf -c", ntcssRootPath, ntcssRootPath, ntcssRootPath);
   sprintf(startupDirectory, "%s\\ldap\\runtime", ntcssRootPath);
   if (CreateProcess(NULL, command, NULL, NULL, FALSE, 
                     0, NULL, startupDirectory, &startInfo, &processInfo) == FALSE) {
      DebugMsg("ApplyDatabase() Failed to create slapadd process");      
      return false;
   }

   DebugMsg("Running apply...");   

   // Wait for the process to terminate
   if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      DebugMsg("Waiting for apply completion failed");      
      return false;
   }    

   DebugMsg("Apply complete"); 

   // Close process and thread handles. 
   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);                       

   return true;
}

/*****************************************************************************/

bool NDSService::StartSlapdProcess()
{
   STARTUPINFO startInfo;   
   char command[_MAX_PATH];   
   char startupDir[_MAX_PATH];   

   DebugMsg("Entering StartSlapdProcess()");

   // Start the slapd daemon
   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  

   // The "-d 0" option keeps the process from returning automatically, which causes the WaitForSingleObject
   //  to think that the process terminated
   sprintf(command, "%s\\ldap\\runtime\\slapd.exe -d 0 -f %s\\ldap\\runtime\\slapd.conf", ntcssRootPath, ntcssRootPath);   
   sprintf(startupDir, "%s\\ldap\\runtime", ntcssRootPath);
   if (CreateProcess(NULL, command, NULL, NULL, FALSE, 
                     0, NULL, startupDir, &startInfo, &slapdProcessInfo) == FALSE) {
      DebugMsg("StartSlapdProcess() Failed to create slapd process");      
      return false;
   }

   DebugMsg("StartSlapdProcess() Created slapd process");
   return true;   
}

/*****************************************************************************/

bool NDSService::StartSlurpdProcess()
{
   STARTUPINFO startInfo;   
   char command[_MAX_PATH];
   char startupDir[_MAX_PATH];   

   DebugMsg("Entering StartSlurpdProcess()");

   // Start the slurpd process
   ZeroMemory(&startInfo, sizeof(startInfo));
   startInfo.cb = sizeof(startInfo);
   startInfo.dwFlags = STARTF_USESHOWWINDOW;
   startInfo.wShowWindow = SW_HIDE;  
   
   // The "-d 0" option keeps the process from returning automatically, which causes the WaitForSingleObject
   //  to think that the process terminated
   sprintf(command, "%s\\ldap\\runtime\\slurpd.exe -d 0 -f %s\\ldap\\runtime\\slapd.conf ./repdir/slapd.replog", ntcssRootPath, ntcssRootPath);
   sprintf(startupDir, "%s\\ldap\\runtime", ntcssRootPath);
   if (CreateProcess(NULL, command, NULL, NULL, FALSE, 
                     0, NULL, startupDir, &startInfo, &slurpdProcessInfo) == FALSE) {
      DebugMsg("StartSlurpdProcess() Failed to create slurpd process");      
      return false;
   }

   DebugMsg("StartSlurpdProcess() Created slurpd process");
   return true;
}

/*****************************************************************************/
// Terminates the slapd process
bool NDSService::StopSlapdProcess()
{
   char debugMsg[100];

   // Terminate the slapd process
   if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, slapdProcessInfo.dwProcessId) != NULL) {
  
      if (TerminateProcess(slapdProcessInfo.hProcess,0) == 0) {
         sprintf(debugMsg, "StopSlapdProcess() unable to terminate process <%d>", GetLastError());
         DebugMsg(debugMsg);      
         return false;
      }   
 
   }

   return true;
}

/*****************************************************************************/
// Terminates the slurpd process
bool NDSService::StopSlurpdProcess()
{
   char debugMsg[100];

   // Terminate the slurpd process
   if (OpenProcess(PROCESS_ALL_ACCESS, FALSE, slurpdProcessInfo.dwProcessId) != NULL) { 

      if (TerminateProcess(slurpdProcessInfo.hProcess,0) == 0) {
         sprintf(debugMsg, "StopSlurpdProcess() unable to terminate process <%d>", GetLastError());
         DebugMsg(debugMsg);      
         return false;
      }
   }   

   return true;
}

/*****************************************************************************/
