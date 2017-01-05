/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class ntcsss_libs_UserLib */

#ifndef _Included_ntcsss_libs_UserLib
#define _Included_ntcsss_libs_UserLib
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addBatchUserToSystem
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addBatchUserToSystem
  (JNIEnv *, jclass, jstring, jstring, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addSystemGroup
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addSystemGroup
  (JNIEnv *, jclass, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    addBaseNtcssUserToSystem
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_addBaseNtcssUserToSystem
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    removeSystemGroup
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_removeSystemGroup
  (JNIEnv *, jclass, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    removeBatchUserFromSystem
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ntcsss_libs_UserLib_removeBatchUserFromSystem
  (JNIEnv *, jclass, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    CreateMasterSvrAccount
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lntcsss/util/IntegerBuffer;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_CreateMasterSvrAccount
  (JNIEnv *, jclass, jstring, jstring, jstring, jstring, jstring, jobject);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    CreateAuthSvrAccount
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_CreateAuthSvrAccount
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveMasterSvrAccount
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveMasterSvrAccount
  (JNIEnv *, jclass, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveAuthSvrAccount
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveAuthSvrAccount
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    ChangeSystemAccountRealName
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_ChangeSystemAccountRealName
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    ChangeSystemAccountPassword
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_ChangeSystemAccountPassword
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    RemoveUserFromOSGroup
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_RemoveUserFromOSGroup
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     ntcsss_libs_UserLib
 * Method:    AddUserToOSGroup
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_ntcsss_libs_UserLib_AddUserToOSGroup
  (JNIEnv *, jclass, jstring, jstring);

#ifdef __cplusplus
}
#endif
#endif
