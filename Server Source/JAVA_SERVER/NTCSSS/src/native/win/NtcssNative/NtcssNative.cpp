/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include <windows.h>
#include <Mmsystem.h>
#include <Lm.h>
#include <fstream.h>
#include "PrtUtilClass.h"
//#include "Trace.h"


#include <ntsecapi.h>  // For LsaEnumerateTrustedDomains

// To be able to use the USE_CONVERSION statement
#include "atlbase.h"
#include "atlconv.h"

#include <jni.h>
#include "ntcsss_libs_UserLib.h"
#include "ntcsss_util_SystemOps.h"
#include "ntcsss_libs_PrintLib.h"
#include "ntcsss_tools_NtcssLp.h"

//typedef BOOL (CALLBACK *ULPRET)(LPWSTR,DWORD,LPBYTE,LPWSTR);

/*
 * Returns the NT domain to use for domain APIs. 
 *  If an error occurs, NULL is returned.
 */
LPCWSTR getDomain()
{
   LSA_OBJECT_ATTRIBUTES ObjectAttributes;
   PLSA_UNICODE_STRING lsaszServer = NULL;
   NTSTATUS ntsResult;
   LSA_HANDLE lsahPolicyHandle;
   POLICY_PRIMARY_DOMAIN_INFO *pppdi = NULL; 
   LPCWSTR domainName = NULL;     

   // Object attributes are reserved, so initialize to zeroes.
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
   if (LsaNtStatusToWinError(ntsResult) != ERROR_SUCCESS) 
      return NULL;
      

   // Get the primary domain
   ntsResult = LsaQueryInformationPolicy( lsahPolicyHandle, PolicyPrimaryDomainInformation, 
                                                  (void **) &pppdi );      

   if (LsaNtStatusToWinError(ntsResult) == ERROR_SUCCESS) 
      //domainName = W2T(pppdi->Name.Buffer);                                                           
      domainName = pppdi->Name.Buffer;                                                           

   if ( pppdi != NULL )
      LsaFreeMemory( pppdi );
                 

   // Freeing the policy object handle
   ntsResult = LsaClose(lsahPolicyHandle);    

   return domainName;
}

/*
 * User/group administration functions should only be performed if the local host is not in a domain, 
 *  or the host is a domain controller in a domain. If admin functions should be performed, true is returned.
 *  Otherwise, false is returned.
 */
bool performAdminFunction()
{
   LPCWSTR domainName;
   char localHostname[50];
   OSVERSIONINFOEX versionInfo;

   USES_CONVERSION;

   // Get the domain name 
   if ((domainName = getDomain()) == NULL)       
      return false;   

   // Get the local hostname
   gethostname(localHostname, 50);

   // If this host is not in an NT domain, then admin functions should be performed. If 
   //  the host is in an NT domain, only perform the admin operations if this is a
   //  domain controller.
   if (_stricmp(localHostname, W2A(domainName)) == 0) // We're not on a domain
      return true;
   else {  // This host must be on a domain

      // Determine what OS type we're on   
      versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
      if (GetVersionEx((OSVERSIONINFO*)&versionInfo) == 0)          
         return false;      

      if (versionInfo.wProductType == VER_NT_DOMAIN_CONTROLLER) 
         return true;
   }

   return false;
}


/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addBatchUserToSystem
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 *
 * Adds the given user to the domain if they do not exist. The user is also added to the 
 *  given global users group.
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addBatchUserToSystem
  (JNIEnv *env, jclass, jstring login, jstring passwd, jstring realName, jstring group)
{
   USER_INFO_1 ui;
   DWORD dwLevel = 1;
   DWORD dwError = 0;
   NET_API_STATUS nStatus;
   jboolean ret = true;
   LPCWSTR domainName;
//   LPWSTR domainControllerName = NULL;
   jclass cls;
   jmethodID mid;
   char errorBuffer[50];
   LPCSTR convUsername, convPassword, convRealName, convGroup;
   BOOL canLog = TRUE;
   LOCALGROUP_MEMBERS_INFO_3 lgm_struct;
   char name[50];
      
   USES_CONVERSION;      

   // See if this administration function should be performed
   if (performAdminFunction() == false)
      return true;

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;
    

   // Get the domain name 
   if ((domainName = getDomain()) == NULL) {
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting domain %lu: Unable to add user to group", GetLastError());                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBatchUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false;
   }

/*
   // Get the PDC for the domain
   if ((nStatus = NetGetDCName(NULL,domainName,(LPBYTE *)&domainControllerName)) != NERR_Success) {   
      if (canLog == TRUE) {                  
         sprintf(errorBuffer, "Error getting DC: %lu   Unable to add user to group", nStatus);                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBatchUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false; 
   }    
*/  

   convUsername = env->GetStringUTFChars(login, 0); 
   convPassword = env->GetStringUTFChars(passwd, 0); 
   convRealName = env->GetStringUTFChars(realName, 0); 
   convGroup = env->GetStringUTFChars(group, 0);             
   
   // Attempt to add the given user. If the call succeeds, add the user to the given group 
   ui.usri1_name = A2W(convUsername);     
   ui.usri1_password = A2W(convPassword); 
   ui.usri1_priv = USER_PRIV_USER;
   ui.usri1_home_dir = NULL;
   ui.usri1_comment = A2W(convRealName);
   ui.usri1_flags = UF_SCRIPT | UF_DONT_EXPIRE_PASSWD | UF_PASSWD_CANT_CHANGE;
   ui.usri1_script_path = NULL;        
   nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&ui, &dwError);   
   if ((nStatus == NERR_Success) || (nStatus == NERR_UserExists)) {
            
      // Attempt to add the user to the given group
      sprintf(name, "%s\\%s", W2A(domainName), convUsername);        
      lgm_struct.lgrmi3_domainandname=(LPWSTR)A2W(name);
      nStatus=NetLocalGroupAddMembers(NULL,(LPCWSTR)A2W(convGroup), 3,(LPBYTE)&lgm_struct,1);      
//    nStatus = NetGroupAddUser(domainControllerName, A2W(convGroup), A2W(convUsername));           
      if (nStatus != NERR_Success) {       
         sprintf(errorBuffer, "Error adding user to group: %lu. User may already be a member", nStatus);                           
         ret = false; 
      }             
   }
   else if (nStatus != NERR_NotPrimary) {  // Don't report an error just because we're on a BDC
      sprintf(errorBuffer, "Error adding user: %lu", nStatus);                           
      ret = false; 
   }

   env->ReleaseStringUTFChars(login, convUsername);
   env->ReleaseStringUTFChars(passwd, convPassword);
   env->ReleaseStringUTFChars(realName, convRealName);
   env->ReleaseStringUTFChars(group, convGroup);

   // Attempt to use our NTCSSS logger
   if ((ret == false) && (canLog == TRUE))
      env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBatchUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));

   return ret;
}


/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addSystemGroup
 * Signature: (Ljava/lang/String;)Z
 *
 * Adds the given group as a global user group to the system
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addSystemGroup
  (JNIEnv *env, jclass, jstring groupName)
{
   jclass cls;
   jmethodID mid;
   BOOL canLog = true;   
   NET_API_STATUS nRet;
//   GROUP_INFO_0 grpInfo;
   DWORD parm_err;
//   LPCWSTR domainName;   
//   LPWSTR domainControllerName = NULL;
   LPCTSTR  convGroupName; 
   char errorBuffer[50];
   LOCALGROUP_INFO_0 localGrpInfo;   

   USES_CONVERSION;

   // See if this administration function should be performed
   if (performAdminFunction() == false)
      return true;

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;  
           
/*
   // Get the domain name   
   if ((domainName = getDomain()) == NULL) {
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting domain %lu", GetLastError());                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false;
   }

   // Get the PDC for the domain
   if ((nRet = NetGetDCName(NULL,domainName,(LPBYTE *)&domainControllerName)) != NERR_Success) {   
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting DC for <%s>: %lu", W2T(domainName), nRet);
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
      }
      return false; 
   }
*/
   

   // Attempt to add the group to the domain
   convGroupName = env->GetStringUTFChars(groupName, 0);
   localGrpInfo.lgrpi0_name = A2W(convGroupName);
// grpInfo.grpi0_name = T2W(convGroupName);      
   nRet = NetLocalGroupAdd(NULL, 0, (LPBYTE)&localGrpInfo, &parm_err);   
// nRet = NetGroupAdd(domainControllerName, 0, (LPBYTE)&grpInfo,  &parm_err);
   env->ReleaseStringUTFChars(groupName, convGroupName);


   // Check the return type. Don't complain if the group already exists
   if ((nRet != NERR_Success) && (nRet != NERR_GroupExists) && (nRet != ERROR_ALIAS_EXISTS) &&
       (nRet != NERR_NotPrimary)) {  
      if (canLog == TRUE) {     
         convGroupName = env->GetStringUTFChars(groupName, 0);
         sprintf(errorBuffer, "Error adding group <%s>: %lu", convGroupName, nRet);
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
         env->ReleaseStringUTFChars(groupName, convGroupName);
      }
      return false;
   }

   return true;   
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addBaseNtcssUserToSystem
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 *
 * Adds the base NTCSS user to the system
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addBaseNtcssUserToSystem
  (JNIEnv *env, jclass, jstring loginName, jstring password, jstring realName)
{
   USER_INFO_1 ui;
   DWORD dwLevel = 1;
   DWORD dwError = 0;
   NET_API_STATUS nStatus;
   jboolean ret = true;
//   LPCWSTR domainName;
//   LPWSTR domainControllerName = NULL;
   jclass cls;
   jmethodID mid;
   char errorBuffer[50];
   LPCSTR convUsername, convPassword, convRealName;
   BOOL canLog = TRUE;

   USES_CONVERSION;
   
   
   // See if this administration function should be performed
   if (performAdminFunction() == false)
      return true;

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;

/*
   // Get the domain name 
   if ((domainName = getDomain()) == NULL) {
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting domain: %lu  Unable to add base NTCSS user", GetLastError());                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBaseNtcssUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false;
   }

   // Get the PDC for the domain   
   if ((nStatus = NetGetDCName(NULL, domainName,(LPBYTE *)&domainControllerName)) != NERR_Success) {   
      if (canLog == TRUE) {                  
         sprintf(errorBuffer, "Error getting DC: %lu   Unable to add base NTCSS user", nStatus);                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBaseNtcssUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false; 
   }   
*/   

   convUsername = env->GetStringUTFChars(loginName, 0); 
   convPassword = env->GetStringUTFChars(password, 0); 
   convRealName = env->GetStringUTFChars(realName, 0);              
   
   // Attempt to add the given user. 
   ui.usri1_name = A2W(convUsername);     
   ui.usri1_password = A2W(convPassword); 
   ui.usri1_priv = USER_PRIV_USER;
   ui.usri1_home_dir = NULL;
   ui.usri1_comment = A2W(convRealName);
   ui.usri1_flags = UF_SCRIPT | UF_DONT_EXPIRE_PASSWD | UF_PASSWD_CANT_CHANGE;
   ui.usri1_script_path = NULL;  
// nStatus = NetUserAdd(domainControllerName, dwLevel, (LPBYTE)&ui, &dwError);   
   nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&ui, &dwError);   
   if ((nStatus != NERR_Success) && (nStatus != NERR_UserExists) && (nStatus != NERR_NotPrimary)) {      
      sprintf(errorBuffer, "Error adding base NTCSS user: %lu", nStatus);                           
      ret = false; 
   }   

   env->ReleaseStringUTFChars(loginName, convUsername);
   env->ReleaseStringUTFChars(password, convPassword);
   env->ReleaseStringUTFChars(realName, convRealName);
   

   // Attempt to use our NTCSSS logger
   if ((ret == false) && (canLog == TRUE))
      env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_addBaseNtcssUserToSystem"), 
                                   env->NewStringUTF(errorBuffer));

   return ret;

}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    removeSystemGroup
 * Signature: (Ljava/lang/String;)Z
 *
 * Removes the given global user group from the system
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_removeSystemGroup
  (JNIEnv *env, jclass, jstring groupName)
{
   NET_API_STATUS nRet;
   LPCWSTR  conv2GroupName;
   LPCTSTR  convGroupName;
   jclass cls;
   jmethodID mid;
   BOOL canLog = true;   
//   LPCWSTR domainName;   
//   LPWSTR domainControllerName = NULL;
   char errorBuffer[50];

   USES_CONVERSION;

   // See if this administration function should be performed
   if (performAdminFunction() == false)
      return true;

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;  
           
/*
   // Get the domain name
   if ((domainName = getDomain()) == NULL) {
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting domain: %lu", GetLastError());                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false;
   }
 
   // Get the PDC for the domain
   if ((nRet = NetGetDCName(NULL, domainName,(LPBYTE *)&domainControllerName)) != NERR_Success) {
 
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting DC: %lu", nRet);
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
      }
      return false; 
   }
*/

   // Attempt to remove the group from the domain
   convGroupName = env->GetStringUTFChars(groupName, 0);       
   conv2GroupName = T2W(convGroupName);         
// nRet = NetGroupDel(domainControllerName, conv2GroupName);
   nRet = NetLocalGroupDel(NULL, conv2GroupName);
   env->ReleaseStringUTFChars(groupName, convGroupName);

   // Check the return type. Don't complain if the group doesn't exists
   if ((nRet != NERR_Success) && (nRet != NERR_GroupNotFound) && (nRet != NERR_NotPrimary)) {  
      if (canLog == TRUE) {     
         convGroupName = env->GetStringUTFChars(groupName, 0);
         sprintf(errorBuffer, "Error removing group <%s>: %lu", convGroupName, nRet);
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeSystemGroup"), 
                                   env->NewStringUTF(errorBuffer));
         env->ReleaseStringUTFChars(groupName, convGroupName);
      }
      return false;
   }
   

   return true;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    removeBatchUserFromSystem
 * Signature: (Ljava/lang/String;)Z
 *
 * Removes the given user from the domain
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_removeBatchUserFromSystem
  (JNIEnv *env, jclass, jstring loginName)
{
   NET_API_STATUS nStatus;
   jclass cls;
   jmethodID mid;
   BOOL canLog = TRUE;
//   LPCWSTR domainName;
//   LPWSTR domainControllerName = NULL;
   char errorBuffer[50];
   LPCSTR convLoginName;
   jboolean ret = true;

   USES_CONVERSION;

   // See if this administration function should be performed
   if (performAdminFunction() == false)
      return true;

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;

/*
   // Get the domain name
   if ((domainName = getDomain()) == NULL) {
      if (canLog == TRUE) {
         sprintf(errorBuffer, "Error getting domain: %lu     Unable to remove user", GetLastError());                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeBatchUserFromSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false;
   }

   // Get the PDC for the domain   
   if ((nStatus = NetGetDCName(NULL, domainName,(LPBYTE *)&domainControllerName)) != NERR_Success) {   
      if (canLog == TRUE) {                  
         sprintf(errorBuffer, "Error getting DC: %lu   Unable to remove user", nStatus);                     
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeBatchUserFromSystem"), 
                                   env->NewStringUTF(errorBuffer));
      }

      return false; 
   } 
*/

   convLoginName = env->GetStringUTFChars(loginName, 0); 

   // Attempt to remove the given user
// nStatus = NetUserDel(domainControllerName,  A2W(convLoginName));
   nStatus = NetUserDel(NULL,  A2W(convLoginName));
   if ((nStatus != NERR_Success) && (nStatus != NERR_UserNotFound) && (nStatus != NERR_NotPrimary)) {
      sprintf(errorBuffer, "Unable to remove user: %lu", nStatus);
      ret = false;
   }

   env->ReleaseStringUTFChars(loginName, convLoginName);

   if ((ret == false) && (canLog == TRUE))
      env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_UserLib_removeBatchUserFromSystem"), 
                                   env->NewStringUTF(errorBuffer));

   return ret;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    CreateMasterSvrAccount
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_CreateMasterSvrAccount
  (JNIEnv *, jclass, jstring, jstring, jstring, jstring, jstring, jobject)
{
   // normally calls the unix crypt library function to create the crypted
   // password, then calls the script to do the text file editing work
   return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    CreateAuthSvrAccount
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String)Z
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_CreateAuthSvrAccount
  (JNIEnv *, jclass, jstring, jstring, jstring)
{
   return 0;
}


/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveMasterSvrAccount
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveMasterSvrAccount
  (JNIEnv *, jclass, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveAuthSvrAccount
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveAuthSvrAccount
  (JNIEnv *, jclass, jstring, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    ChangeSystemAccountRealName
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_ChangeSystemAccountRealName
  (JNIEnv *, jclass, jstring, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    ChangeSystemAccountPassword
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_ChangeSystemAccountPassword
  (JNIEnv *, jclass, jstring, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveUserFromOSGroup
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveUserFromOSGroup
  (JNIEnv *, jclass, jstring, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    AddUserToOSGroup
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_AddUserToOSGroup
  (JNIEnv *, jclass, jstring, jstring)
{
	return 0;
}

/*
 * Class:     ntcsss_util_SystemOps
 * Method:    getUpTime
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ntcsss_util_SystemOps_getUpTime
  (JNIEnv *env, jclass cls)
{
   char buf[50];
   DWORD days, hrs, mins, secs;
   DWORD time;
         
   // Get the amount of time this host has been up (in milliseconds)
   //  NOTE: The value returned by the timeGetTime function is a DWORD value.
   //        The return value wraps around to 0 every 2^32 milliseconds, which 
   //        is about 49.71 days. So a host that has been up longer than
   //        that will show an incorrect time.
   time = timeGetTime();

   // Determine the number of days, hours, minutes, and seconds the
   //  host has been up
   days = time/(1000 * 60 * 60 * 24);
   time = time%(1000 * 60 * 60 * 24);

   hrs = time/(1000 * 60 * 60);
   time = time%(1000 * 60 * 60);

   mins = time/(1000 * 60);
   time = time%(1000 * 60);

   secs = time/1000;

   // Create a string to return that contains the "up time" information
   sprintf(buf, "%d days %d hrs %d mins %d secs",		
            days, hrs, mins, secs);


   return env->NewStringUTF(buf);
}

/*
 * Class:     ntcsss_util_SystemOps
 * Method:    getCurrentSystemUser
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ntcsss_util_SystemOps_getCurrentSystemUser
  (JNIEnv *env, jclass)
{

  DWORD dwLevel = 0;
  LPWKSTA_USER_INFO_0 pBuf = NULL;
  NET_API_STATUS nStatus;   
  char login_name[20];

  USES_CONVERSION;

  //
  // Call the NetWkstaUserGetInfo function;
  //  specify level 1.
  //
  nStatus = NetWkstaUserGetInfo(NULL,
                                dwLevel,
                                (LPBYTE *)&pBuf);

  if ((nStatus != NERR_Success) || (pBuf == NULL))   
     login_name[0] = '\0';        
  else {

     WCHAR buf[50];
     swprintf(buf, L"%s", pBuf->wkui0_username);    

     memset(login_name, 0, 20); 
     memcpy(login_name, W2T(buf), 20);
     login_name[20] = '\0';

  }

  //
  // Free the allocated memory.
  //
  if (pBuf != NULL)
     NetApiBufferFree(pBuf);

  return env->NewStringUTF(login_name);
}

/*
 * Class:     ntcsss_util_SystemOps
 * Method:    getSystemEnvironmentVariable
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ntcsss_util_SystemOps_getSystemEnvironmentVariable
  (JNIEnv *env, jclass, jstring environmentVariable)
{
   return env->NewStringUTF(getenv(env->GetStringUTFChars(environmentVariable, 0)));
}

/*
 * Class:     ntcsss_libs_PrintLib
 * Method:    getPrtList
 * Signature: ()Ljava/lang/String;
 *
 * Returns a list of printers
 */
JNIEXPORT jstring JNICALL Java_ntcsss_libs_PrintLib_getSystemPrinterList
  (JNIEnv *env, jclass)
{ 

   return env->NewStringUTF("NTCSSWINDOWSHOST");

/*
    DWORD pcbNeeded,pcReturned;
    PRINTER_INFO_1 *pPI1;
    LPTSTR buffer;
    jclass cls;
    jmethodID mid;
    LPTSTR printerList;
    BOOL processedFirst = FALSE;
    jstring retList;

   try {

      // Call once to see how many printers would be returned (and the space needed to store them)
      ::EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,NULL,1,0,0,&pcbNeeded,&pcReturned);

      // Check to see if any printers were returned
      if (pcbNeeded == 0) 	
         return env->NewStringUTF("");		

      // Get the printer list
      buffer = new char[pcbNeeded];		
      pPI1=(PRINTER_INFO_1*)buffer;
      ::EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,NULL,1,(LPBYTE)pPI1,pcbNeeded,
                     &pcbNeeded,&pcReturned);
	
      // Construct a list of the printers to return
      printerList = new char[pcbNeeded + (pcReturned * 2)];
      memset(printerList, '\0', strlen(printerList));   
      for(DWORD i = 0; i < pcReturned; i++) {	

         if (processedFirst == TRUE)
            strcat(printerList, ",");

         strcat(printerList, pPI1[i].pName);		
         processedFirst = TRUE;
	  }
      retList = env->NewStringUTF(printerList);   

	  delete [] buffer;
      delete [] printerList;
      

      return retList;
   }
   catch (...)
   {
      // Log the fact that an exception occured
      if (((cls = env->FindClass("ntcsss/log/Log")) != NULL) &&
          ((mid = env->GetStaticMethodID(cls, "excp", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) != NULL))      
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_libs_PrintLib_getPrtList"), 
                                   env->NewStringUTF("An exception occurred"));
		
      return NULL;
   }
*/
}

/*
 * Class:     ntcsss_tools_NtcssLp
 * Method:    printTextFilesToLocalPrinter
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;ZZILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 *
 *  Prints the files specified in the comma delimited list of files to the
 *  specified local printer. If no errors occur, true is returned.
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_tools_NtcssLp_printTextFilesToLocalPrinter
  (JNIEnv *env, jobject, jstring printerName, jstring fileListStr, jstring optionListStr, 
   jint numCopies, jstring filePtrOutputFile, jboolean printBanner, 
   jboolean portraitOrientation, jint pageSize, jstring loginName, jstring appName,
   jstring securityClassification)
{
   ifstream infile;
   HDC hDC;
   DOCINFO docInfo;
   const char* convPrinterName = env->GetStringUTFChars(printerName, NULL);
   const char* convFileListStr = env->GetStringUTFChars(fileListStr, NULL);      
   int pageWidth, pageHeight;
   TEXTMETRIC textMetric;
   int lines_per_page;
   int chars_per_row;
   int start_x_pos;
   int start_y_pos;
   int line_on;
   int y_pos;
   int len;
   const int buffSize = 256;
   char read_buffer[buffSize];
   BOOL ok;
   BOOL canLog = TRUE;
   jclass cls;
   jmethodID mid;
   int index;
   LPSTR buf;
   DEVMODE initializationData;
   char errorMsg[100];
   jboolean retVal = false;
   char* file;

   // The optionListStr is really for the unix lp -o options. It is not used here (yet)      
      

   // Attempt to use our NTCSSS logger
   if (((cls = env->FindClass("ntcsss/log/Log")) == NULL) ||
       ((mid = env->GetStaticMethodID(cls, "error", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) == NULL))
      canLog = FALSE;
   errorMsg[0] = '\0';

   // Create a device context for the printer to which text will be output. Set the
   //  number of copies to be made equal to the number passed in
   memset(&initializationData, 0, sizeof(DEVMODE));
   initializationData.dmSize = sizeof(DEVMODE);
   initializationData.dmFields = DM_COPIES | DM_ORIENTATION | DM_PAPERSIZE;
   initializationData.dmCopies = (short)numCopies;   
   if (portraitOrientation == JNI_TRUE)
      initializationData.dmOrientation = DMORIENT_PORTRAIT;
   else
      initializationData.dmOrientation = DMORIENT_LANDSCAPE;
   if (pageSize == 5)
      initializationData.dmPaperSize = DMPAPER_LEGAL;
   else
      initializationData.dmPaperSize = DMPAPER_LETTER;

   hDC = CreateDC("WINSPOOL", convPrinterName, NULL, &initializationData);
   if (hDC == NULL) {
      sprintf(errorMsg, "Cannot create device context to <%s>", convPrinterName);            
      goto close;  
   }    

   if (SetMapMode(hDC, MM_HIMETRIC) == 0) {
      sprintf(errorMsg, "Cannot set mode for <%s>", convPrinterName);
      goto close;
   }


   if (SetTextAlign(hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP) == GDI_ERROR) {
      sprintf(errorMsg, "Cannot set text alignment for <%s>", convPrinterName);
      goto close;
   }


   // Get data from the currently selected font    
   if (GetTextMetrics(hDC, &textMetric) == FALSE) {
      sprintf(errorMsg, "Cannot get text metrics for <%s>", convPrinterName);
      goto close;
   }


   // Get device-specific information about the printer
   pageWidth = GetDeviceCaps(hDC, HORZSIZE);  // Width in millimeters
   pageHeight = GetDeviceCaps(hDC, VERTSIZE); // Height in millimeters

   lines_per_page = (int) (( pageHeight/((double)textMetric.tmHeight/100)) - 2);
   chars_per_row = (int) (( pageWidth/((double)textMetric.tmMaxCharWidth/100)) - 2);
   start_x_pos = textMetric.tmMaxCharWidth;
   start_y_pos = 0;

   memset(&docInfo, 0, sizeof(DOCINFO));
   docInfo.cbSize = sizeof(DOCINFO);
   docInfo.lpszDocName = "ntcss_pt";   // The print job name that is displayed in the queue

   // If the filePtrOutputFile parameter isn't null, then this is a "file" type printer
   //  and the parameter specifies the name of the output file
   if (filePtrOutputFile != NULL)
      docInfo.lpszOutput = env->GetStringUTFChars(filePtrOutputFile, NULL);
    

   // Parse the comma delimited list of files
   file = strtok((char*)convFileListStr, ",");
   while (file != NULL) {      
      
      // Try to open the file that is to be printed
      infile.open(file,ios::in | ios::nocreate);
      if (!infile.is_open()) {   
         sprintf(errorMsg, "Cannot open file <%s>", file);
         goto next;
      }         
                        
      // Start the printing              
      if (StartDoc(hDC, &docInfo) == SP_ERROR) {
         sprintf(errorMsg, "Cannot start print job on <%s> for <%s>", convPrinterName, file);
         goto next;
      }
      
  
      if (StartPage(hDC) == SP_ERROR) {
         sprintf(errorMsg, "Cannot start page on <%s> for <%s>", convPrinterName, file);
         goto next;
      }

      // Print the banner if desired
      y_pos = start_y_pos;
      if (printBanner == JNI_TRUE) {

         sprintf(read_buffer, "User:       %s", env->GetStringUTFChars(loginName, NULL));
         TextOut(hDC, start_x_pos, y_pos, read_buffer, strlen(read_buffer));
         y_pos -= textMetric.tmHeight;

         sprintf(read_buffer, "Security:   %s", env->GetStringUTFChars(securityClassification, NULL));
         TextOut(hDC, start_x_pos, y_pos, read_buffer, strlen(read_buffer));
         y_pos -= textMetric.tmHeight;

         sprintf(read_buffer, "Application: %s", env->GetStringUTFChars(appName, NULL));
         TextOut(hDC, start_x_pos, y_pos, read_buffer, strlen(read_buffer));
         y_pos -= textMetric.tmHeight;

         sprintf(read_buffer, "File Name:   %s", file);
         TextOut(hDC, start_x_pos, y_pos, read_buffer, strlen(read_buffer));
         y_pos -= textMetric.tmHeight;

         EndPage(hDC);
      }

      line_on = 0;
      y_pos = start_y_pos;

      // Read a line from the file and write it out to the printer page
      ok = TRUE;     
      index = 0;
      while (ok) {        

         if (index != 0)
            buf = &read_buffer[index];
         else if (infile.getline(read_buffer, buffSize,'\n'))
            buf = read_buffer;
         else 
            break;

         // Check to see if the line should be line wrapped
         len = strlen(buf);        
         if (len > chars_per_row) {         
            index += chars_per_row;
            len = chars_per_row;
         }
         else
            index = 0;

         // See if the number of lines per page has been exceeded
         line_on++;
         if (line_on > lines_per_page) {            
            line_on = 1;
            y_pos = start_y_pos;
            EndPage(hDC);           
         }

         // Write the character string out to the printer file
         ok = TextOut(hDC, start_x_pos, y_pos, buf, len);

         y_pos -= textMetric.tmHeight;
      }


      // Close the input file
      infile.close();      

      if (EndPage(hDC) == SP_ERROR) {
         sprintf(errorMsg, "Cannot end page on <%s> for <%s>", convPrinterName, file);
         goto next;
      }


      if (EndDoc(hDC) == SP_ERROR) {
         sprintf(errorMsg, "Cannot end print job on <%s> for <%s>", convPrinterName, file);
         goto next;
      }
   	
      next:

      // Log an error message if an error has occurred
      if (strlen(errorMsg) > 0) {
         env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_tools_NtcssLp_printTextFilesToLocalPrinter"), 
                                   env->NewStringUTF(errorMsg));
         errorMsg[0] = '\0';  // "Clear" the error message buffer
      }

      // Close the file if it is still open
      if ((infile != NULL) && (infile.is_open())) 
         infile.close(); 
            
      // Get the next file in the list that is to be printed
      file = strtok(NULL, ",");

   }  // while loop

   if (DeleteDC(hDC) == FALSE) {
      sprintf(errorMsg, "Cannot delete device context for <%s>", convPrinterName);
      env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_tools_NtcssLp_printTextFilesToLocalPrinter"), 
                                   env->NewStringUTF(errorMsg));
      goto close;      
   }

   retVal = true;

   close:

   // Log an error message if an error has occurred
   if (strlen(errorMsg) > 0)
      env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("NtcssNative"), 
                                   env->NewStringUTF("Java_ntcsss_tools_NtcssLp_printTextFilesToLocalPrinter"), 
                                   env->NewStringUTF(errorMsg));

   env->ReleaseStringUTFChars(printerName, convPrinterName);
   env->ReleaseStringUTFChars(fileListStr, convFileListStr);   

   return retVal;      
} 





JNIEXPORT jboolean JNICALL Java_ntcsss_libs_PrintLib_addPrinter
						  (JNIEnv *env, jclass, jstring jprintername, jstring jdrivername, jstring jdriver, 
						  jstring jdatafile, jstring juifile, jstring jhelpfile, 
						  jstring jmonitorname, jstring jmonitor, jstring jcopyfiles, 
						  jstring jntcssPrinterType , jstring jport ,jstring jdriverPath)
{ 
	try
	{
	
//		::OutputDebugString("Native entrance... Initilizing varaibles Jackass\n");

	   


//	   ::OutputDebugString("Native entrance...calling the constructor Jackass\n");




	   	   CPrintUtil prtMan(env->GetStringUTFChars(jprintername, NULL),
						 env->GetStringUTFChars(jdrivername, NULL),
						 env->GetStringUTFChars(jdriver, NULL),
						 env->GetStringUTFChars(jdatafile, NULL), 
						 env->GetStringUTFChars(juifile, NULL),   
						 env->GetStringUTFChars(jhelpfile, NULL),  
						 env->GetStringUTFChars(jmonitorname, NULL),
						 env->GetStringUTFChars(jmonitor, NULL),
						 env->GetStringUTFChars(jcopyfiles, NULL),
						 env->GetStringUTFChars(jntcssPrinterType, NULL),
						 env->GetStringUTFChars(jport, NULL),
						 env->GetStringUTFChars(jdriverPath, NULL)); 

//	   ::OutputDebugString("past the constructor Jackass\n");

       return prtMan.AddNTCSSPrinter();

	}


	catch(LPTSTR pStr)
	{
		//Trace(_T("%s\n"),pStr);
		::OutputDebugString(pStr);
		return false;
	}

	catch(...)
	{
		//Trace(_T("Unknown Exception Caught\n"));
		::OutputDebugString(_T("Unknown Exception Caught\n"));
		return false;
	}
   
}


JNIEXPORT jboolean JNICALL Java_ntcsss_libs_PrintLib_deletePrinter
  (JNIEnv *env, jclass, jstring jprintername, jstring jprintingMethod)
{
	try
	{
		CPrintUtil prtMan(env->GetStringUTFChars(jprintername, NULL),
                          env->GetStringUTFChars(jprintingMethod, NULL));
		return prtMan.DelNTCSSPrinter();

	}

	catch(LPTSTR pStr)
	{
		::OutputDebugString(pStr);
		return false;
	}

	catch(...)
	{
		::OutputDebugString(_T("Unknown Exception Caught\n"));
		return false;
	}
   
 	return true;
}


JNIEXPORT jboolean JNICALL Java_ntcsss_libs_PrintLib_add2KPrinter
(JNIEnv *env, jclass, jstring jprintername, jstring jfilename, jstring jdrivername,
                      jstring jprintingmethod)
{
	try
	{
		CPrintUtil prtMan(env->GetStringUTFChars(jprintername, NULL),
		                  env->GetStringUTFChars(jfilename, NULL),
		                  env->GetStringUTFChars(jdrivername, NULL),
                          env->GetStringUTFChars(jprintingmethod, NULL));

		return prtMan.AddNTCSSPrinter2K();

	}

	catch(LPTSTR pStr)
	{
		::OutputDebugString(pStr);
		return false;
	}

	catch(...)
	{
		::OutputDebugString(_T("Unknown Exception Caught in Add2KPrinter\n"));
		return false;
	}
   
 	return true;
}
