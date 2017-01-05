/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"

#include "Ntcss.h"
#include "jni.h"

/**
 * This file represents functions that were in the NTCSS/LIBS/SRC directory 
 *  in the C server code.
 */

#define NOT_INIT   0 
#define INIT_ERROR 1
#define INIT_OK    2


static bool initialize();
void closeLogConnection();

// Global variables
static JavaVM *vm  = (JavaVM *)0;
static JNIEnv *env = (JNIEnv*)0;
static int initializationState = NOT_INIT;

/*****************************************************************************/
/**
  This function must be used by those functions that make calls into our NTCSS java code.
 */

static bool initialize()
{
   int nError = 0;
   JavaVMInitArgs vm_args;
   JavaVMOption options[4];
   char classpath[_MAX_PATH];
   char ntcssRootPath[_MAX_PATH];
   jclass cls;
   jmethodID mid;
   HMODULE hModule;
   char longPath[_MAX_PATH];
   jboolean ok;
   char propertiesFile[_MAX_PATH];
   HMODULE libraryHandle;
   jint (JNICALL *CreateJavaVM)(JavaVM **pvm, void **env, void *args);
   char jvmDLL[_MAX_PATH];


   // Check to see if initialization has already been done. If there
   //  was an error trying to initialize before, don't try again.
   if (initializationState == INIT_ERROR)
      return false;
   else if (initializationState == INIT_OK)
      return true;


   // Determine the NTCSSS root path  
   memset(ntcssRootPath, '\0', _MAX_PATH);
   if ((hModule = GetModuleHandle("NtcssApi")) == NULL) {
      initializationState = INIT_ERROR;
      return false;
   }
   GetModuleFileName(hModule, longPath, _MAX_PATH);
   GetShortPathName(longPath, ntcssRootPath, _MAX_PATH);      
   strcat(ntcssRootPath, "\\..\\..");  


   // Load the JVM DLL. Can remove this if the jvm.dll is specified in PATH env variable.
   sprintf(jvmDLL, "%s\\jre\\bin\\classic\\jvm.dll", ntcssRootPath);
   if ((libraryHandle = LoadLibrary(jvmDLL)) == NULL) {
      initializationState = INIT_ERROR;
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

   vm_args.version = JNI_VERSION_1_2;
   vm_args.options = options;
   vm_args.nOptions = 1;

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
      printf("Error: %d - JNI_CreateJavaVM()\n", nError);  
      initializationState = INIT_ERROR;         
      return false;
   }

   // Try to load the NTCSSS properties file
   cls = env->FindClass("COMMON/PropertiesLoader");
   if (cls == NULL) {
      printf("Can't find PropertiesLoader class\n");
      initializationState = INIT_ERROR;
      return false;
   }
 
   mid = env->GetStaticMethodID(cls, "load", "(Ljava/lang/String;)Z");
   if (mid == NULL) {
      printf("Can't find PropertiesLoader load method\n");
      initializationState = INIT_ERROR;
      return false;
   }

   sprintf(propertiesFile, "%s\\Ntcss.properties", ntcssRootPath);
   ok = env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(propertiesFile));
   if (ok == JNI_FALSE) {
      printf("Error loading properties file\n");
      initializationState = INIT_ERROR;
      return false;
   }

   initializationState = INIT_OK;
   return true;
}

/*****************************************************************************/

int getHostType(LPCTSTR hostname)
{
   jclass cls;
   jmethodID mid;
   jint hostType;

   if (initialize() == false) 
      return -1;

   
   cls = env->FindClass("ntcsss/libs/DataLib");
   if (cls == NULL) {
      printf("Can't find class DataLib\n");
      return -1;
   } 

   mid = env->GetStaticMethodID(cls, "getHostType", "(Ljava/lang/String;)I");
   if (mid == NULL) {
      printf("Can't find method getHostType\n");
      return -1;
   }

   hostType = env->CallStaticIntMethod(cls, mid, env->NewStringUTF(hostname));

   // For some reason, if the java method called here sends any data to
   //  the Log daemon via the Log class, that connection to the Log daemon
   //  does not get broken even when this NtcssApi.dll is no longer in use.
   //  This call just forces that connection to be broken (if one was 
   //  established). If this is not done, then the Log daemon will have
   //  handler threads that won't die, which uses up valueable memory
   //  (e.g. 16kbytes of non-paged pool memory per handler on Windows).
   closeLogConnection();

   return hostType;
}

/*****************************************************************************/

int net_copy(LPCTSTR action, LPCTSTR host, LPCTSTR local_file_name, LPCTSTR server_file_name, int xfer_mode)
{   
   jclass cls;
   jmethodID mid;   
   jboolean isText;
   jboolean append;

   if (initialize() == false) 
      return -1;

   
   cls = env->FindClass("COMMON/FTP");
   if (cls == NULL) {
      printf("Can't find class FTP\n");
      return -1;
   } 

   if (xfer_mode == XFER_ASCII)
      isText = JNI_TRUE;
   else
      isText = JNI_FALSE;



   if ((strcmp(action, "put") == 0) || (strcmp(action, "PUT") == 0)) {      

      mid = env->GetStaticMethodID(cls, "put", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ)Z");
      if (mid == NULL) {
         printf("Can't find method put\n");
         return -1;
      }
      
      append = FALSE;
      if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(host), env->NewStringUTF(server_file_name),
                                    env->NewStringUTF(local_file_name), isText, append) == JNI_FALSE)
         return -1;
      
   }
   else {

      mid = env->GetStaticMethodID(cls, "get", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z");
      if (mid == NULL) {
         printf("Can't find method get\n");
         return -1;
      }

      if (env->CallStaticBooleanMethod(cls, mid, env->NewStringUTF(host), env->NewStringUTF(server_file_name),
                                    env->NewStringUTF(local_file_name), isText) == JNI_FALSE)
         return -1;
   } 

   
   return 0;
}

/*****************************************************************************/

int getNtcssSysConfValueFromNis(LPTSTR* strSysConfData, LPCTSTR tag)
{
   jclass cls;
   jmethodID mid;
   jstring value;
   jsize strLength;

   if (initialize() == false) 
      return -1;

   
   cls = env->FindClass("ntcsss/libs/NDSLib");
   if (cls == NULL) {
      printf("Can't find class NDSLib\n");
      return -1;
   } 

   mid = env->GetStaticMethodID(cls, "getSysConfValue", "(Ljava/lang/String;)Ljava/lang/String;");
   if (mid == NULL) {
      printf("Can't find method getHostType\n");
      return -1;
   }

   value = (jstring)env->CallStaticObjectMethod(cls, mid, env->NewStringUTF(tag));

   // For some reason, if the java method called here sends any data to
   //  the Log daemon via the Log class, that connection to the Log daemon
   //  does not get broken even when this NtcssApi.dll is no longer in use.
   //  This call just forces that connection to be broken (if one was 
   //  established). If this is not done, then the Log daemon will have
   //  handler threads that won't die, which uses up valueable memory
   //  (e.g. 16kbytes of non-paged pool memory per handler on Windows).
   closeLogConnection();

   if (value == NULL) 
      return -1;

   strLength = env->GetStringLength(value);
   *strSysConfData = (LPTSTR)malloc(sizeof(char) * (strLength + 1));
   memset(*strSysConfData, '\0', (strLength + 1));
   strncpy(*strSysConfData, env->GetStringUTFChars(value, NULL), strLength);  

   return 0;
}

/*****************************************************************************/

int getNtcssAuthServerByLoginNameFromNis(LPCTSTR user, LPTSTR pas)
{
   jclass cls;
   jmethodID mid;
   jstring value;
   jsize strLength;   

   if (initialize() == false) 
      return -1;

   
   cls = env->FindClass("ntcsss/libs/NDSLib");
   if (cls == NULL) {
      printf("Can't find class NDSLib\n");
      return -1;
   } 

   mid = env->GetStaticMethodID(cls, "getAuthServer", "(Ljava/lang/String;)Ljava/lang/String;");
   if (mid == NULL) {
      printf("Can't find method getAuthServer\n");
      return -1;
   }

   value = (jstring)env->CallStaticObjectMethod(cls, mid, env->NewStringUTF(user));

   // For some reason, if the java method called here sends any data to
   //  the Log daemon via the Log class, that connection to the Log daemon
   //  does not get broken even when this NtcssApi.dll is no longer in use.
   //  This call just forces that connection to be broken (if one was 
   //  established). If this is not done, then the Log daemon will have
   //  handler threads that won't die, which uses up valueable memory
   //  (e.g. 16kbytes of non-paged pool memory per handler on Windows).
   closeLogConnection();

   if (value == NULL) 
      return -1;

   strLength = env->GetStringLength(value);
   memset(pas,  '\0', strLength + 1);
   sprintf(pas, "%s", env->GetStringUTFChars(value, NULL));  

   return 0;   
}

/*****************************************************************************/
// Renamed so  as not to conflict with NtcssServerSocket.get_master_name
int LIB_get_master_server(LPTSTR master_server_name)
{
   jclass cls, clsSB;
   jmethodID mid, midSB;
   jint value;   
   jobject buffer;    
   jstring strValue;     

   master_server_name[0] = 0;

   if (initialize() == false) 
      return ERROR_CANT_DETER_MASTER;
 
   // Find the DataLib.getMasterServer method
   cls = env->FindClass("ntcsss/libs/DataLib");
   if (cls == NULL) {
      printf("Can't find class DataLib\n");
      return ERROR_CANT_DETER_MASTER;
   } 

   mid = env->GetStaticMethodID(cls, "getMasterServer", "(Ljava/lang/StringBuffer;)I");
   if (mid == NULL) {
      printf("Can't find method getMasterServer\n");
      return ERROR_CANT_DETER_MASTER;
   }

   // Create the StringBuffer object that the result will be placed into
   clsSB = env->FindClass("java/lang/StringBuffer");
   if (clsSB == NULL) {
      printf("Can't find class StringBuffer\n");
      return ERROR_CANT_DETER_MASTER;
   }

   midSB = env->GetMethodID(clsSB, "<init>", "()V");
   if (midSB == NULL) {
      printf("Can't find StringBuffer constructor\n");
      return ERROR_CANT_DETER_MASTER;
   }

   buffer = env->NewObject(clsSB, midSB);

   value = env->CallStaticIntMethod(cls, mid, buffer);

   // For some reason, if the java method called here sends any data to
   //  the Log daemon via the Log class, that connection to the Log daemon
   //  does not get broken even when this NtcssApi.dll is no longer in use.
   //  This call just forces that connection to be broken (if one was 
   //  established). If this is not done, then the Log daemon will have
   //  handler threads that won't die, which uses up valueable memory
   //  (e.g. 16kbytes of non-paged pool memory per handler on Windows).
   closeLogConnection();

   if (value < 0) 
      return ERROR_CANT_DETER_MASTER;

   // Call the StringBuffer.toString method to get the data
   midSB = env->GetMethodID(clsSB, "toString", "()Ljava/lang/String;");
   if (midSB == NULL) {
      printf("Can't find StringBuffer.toString()\n");
      return ERROR_CANT_DETER_MASTER;
   }
   strValue = (jstring)env->CallObjectMethod(buffer, midSB);
   
   sprintf(master_server_name, "%s", env->GetStringUTFChars(strValue, NULL)); 

   return value;   
}

/*****************************************************************************/

int  plainEncryptString( const char  *key,
                         const char  *data,
                         char        **outStringPtr,
                         int           use_compression,
                         int           use_scrambling )
{
   jclass cls, clsSB;
   jmethodID mid, midSB;
   jint value;
   jobject buffer;
   jstring strValue;   

   *outStringPtr = 0;

   if (initialize() == false) 
      return -1;

   // Find CryptLib.plainEncryptString
   cls = env->FindClass("ntcsss/libs/CryptLib");
   if (cls == NULL) {
      printf("Can't find class CryptLib\n");
      return -1;
   } 

   mid = env->GetStaticMethodID(cls, "plainEncryptString", 
                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/StringBuffer;ZZ)I");
   if (mid == NULL) {
      printf("Can't find method plainEncryptString\n");
      return -1;
   }

   // Create the StringBuffer object that the result will be placed into
   clsSB = env->FindClass("java/lang/StringBuffer");
   if (clsSB == NULL) {
      printf("Can't find class StringBuffer\n");
      return -1;
   }

   midSB = env->GetMethodID(clsSB, "<init>", "()V");
   if (midSB == NULL) {
      printf("Can't find StringBuffer constructor\n");
      return -1;
   }

   buffer = env->NewObject(clsSB, midSB);

   value = env->CallStaticIntMethod(cls, mid, env->NewStringUTF(key), env->NewStringUTF(data), buffer,
                                    use_compression, use_scrambling);

   // For some reason, if the java method called here sends any data to
   //  the Log daemon via the Log class, that connection to the Log daemon
   //  does not get broken even when this NtcssApi.dll is no longer in use.
   //  This call just forces that connection to be broken (if one was 
   //  established). If this is not done, then the Log daemon will have
   //  handler threads that won't die, which uses up valueable memory
   //  (e.g. 16kbytes of non-paged pool memory per handler on Windows).
   closeLogConnection();

   if (value < 0) 
      return -1;

   // Call the StringBuffer.toString method to get the data
   midSB = env->GetMethodID(clsSB, "toString", "()Ljava/lang/String;");
   if (midSB == NULL) {
      printf("Can't find StringBuffer.toString()\n");
      return -1;
   }
   strValue = (jstring)env->CallObjectMethod(buffer, midSB);
   
   *outStringPtr = (char *)malloc((size_t)(env->GetStringLength(strValue) + 1));
   sprintf(*outStringPtr, "%s", env->GetStringUTFChars(strValue, NULL)); 

   return value;
}

/*****************************************************************************/

int  stripWord( int     i, LPCTSTR str, LPTSTR  word )
{
  int         j;
  int         n = i;
  const char *s;
  char       *w;

  if ((size_t)i >= strlen(str))
    return(-1);

  s = str;
  w = word;
  for (j = 0; j < n; j++)
    s++;
  while ( isspace((int)*s) )
    {
      s++;
      n++;
    }
  while ( !isspace((int)*s) && (*s != 0) )
    {
      *w = *s;
      s++;
      w++;
      n++;
    }
  *w = '\0';

  while ( isspace((int)*s) )
    {
      s++;
      n++;
    }

  return(n);
}

/*****************************************************************************/

int GetProfileString(LPTSTR section, LPTSTR key, LPTSTR def, LPTSTR returnBuffer,
                             int returnBuffersSize, LPTSTR filename)
{
   if (GetPrivateProfileString(section, key, def, returnBuffer, returnBuffersSize, filename) <= 0)
      return -1;
   else
      return strlen(returnBuffer);  
}

/*****************************************************************************/
/* Some of this was copied from the Bundleda.c file that is included
   in some of the NTCSS Client code. The rest of it was cut and pasted 
   from the BundleData.c file in the C server side code. 

   The code should be mimicking the code in the C server side BundleData.c file 
*/

extern "C" int  unbundleData( char *str, const char  *key, ... )
{
        va_list ap;
        char *s, *char_data;
        const char *k;
        char format[20];
        int *int_data, i, n;

        s = str;
        k = key;

        va_start(ap, key);
        while (*k) {
                switch (*k) {
/*
 * Unbundle character string.
 */
                        case 'C':
                                char_data = va_arg(ap, char *);
                                for (i = 0; i < 5; i++) {
                                        if (*s)
                                                format[i] = *s++;
                                        else
                                                return(-1);
                                }
                                format[5] = 0;
                                n = atoi(format);
                                for (i = 0; i < n; i++) {
                                        if (*s)
                                                char_data[i] = *s++;
                                        else
                                                return(-1);
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
                                        return(-1);
                                format[1] = 0;
                                n = atoi(format);
                                for (i = 0; i < n; i++) {
                                        if (*s)
                                                format[i] = *s++;
                                        else
                                                return(-1);
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
                                                return(-1);
                                }
                                char_data[11] = 0;
                                break;
                }
                k++;
        }
        va_end(ap);

        return(0);
}

/*****************************************************************************/

int  getOverallLength( const char  *key, ... )
{
        va_list ap;
        const char *k;
        char *char_data;
        char format[20];
        int int_data, leng = 0;

        k = key;

        va_start(ap, key);
        while (*k) {
                switch (*k) {
/*
 * Get length of character string.
 */
                        case 'C':
                                char_data = va_arg(ap, char *);
                                leng += strlen(char_data) + 5;
                                break;
/*
 * Get length of integer.
 */
                        case 'I':
                                int_data = va_arg(ap, int);
                                wsprintf(format, "%d", int_data);
                                leng += strlen(format) + 1;
                                break;
/*
 * Get length of date/time string.
 */
                        case 'D':
                                char_data = va_arg(ap, char *);
                                leng += strlen(char_data);
                                break;
                }
                k++;
        }
        va_end(ap);

        return(leng);
}


/*****************************************************************************/
/**
 * Closes a connection to the NTCSS Log daemon if one has been made.

 * This function should be called by all functions that call any of the 
 *  java classes within the "ntcsss" class package!!!
 */
void closeLogConnection()
{
   jclass cls;
   jmethodID mid;   

   if (initialize() == false) 
      return;

   
   cls = env->FindClass("ntcsss/log/Log");
   if (cls == NULL) {
      printf("Can't find class Log\n");
      return;
   } 

   mid = env->GetStaticMethodID(cls, "closeDaemonConnection", "()V");
   if (mid == NULL) {
      printf("Can't find method closeDaemonConnection\n");
      return;
   }

   env->CallStaticVoidMethod(cls, mid);

   return;   
}

/*****************************************************************************/