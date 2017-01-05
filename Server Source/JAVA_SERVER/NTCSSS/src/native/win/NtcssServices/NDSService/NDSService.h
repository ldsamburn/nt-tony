/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NDSService.h: interface for the NDSService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NDSSERVICE_H__0C370A93_1FDD_11D6_B4F4_00C04F4D4DED__INCLUDED_)
#define AFX_NDSSERVICE_H__0C370A93_1FDD_11D6_B4F4_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ServiceClass.h"
#include "jni.h"

class NDSService : public ServiceClass 
{
public:
	NDSService(const char*, int, int, LPCTSTR);

    virtual void Run();
    virtual void OnStop();

private:

    bool InitializeJVM();
    bool GetMasterName(LPTSTR);
    bool GetServerList();         
    bool IsServer();
    bool GetConfFile();

    bool IsDatabaseInstalled();
    void RemoveDatabase();
    bool RetrieveDatabase();
    bool ApplyDatabase();

    bool StartSlapdProcess();   
    bool StopSlapdProcess(); 
  
    bool StartSlurpdProcess();
    bool StopSlurpdProcess(); 

    JavaVM *vm;
    JNIEnv *env;
    int jvmInitializationState;
    char ntcssRootPath[_MAX_PATH];
    char ntcssRootPathLong[_MAX_PATH];    
    bool isLDAPMaster; 

    PROCESS_INFORMATION slapdProcessInfo;
    PROCESS_INFORMATION slurpdProcessInfo;
};

#endif // !defined(AFX_NDSSERVICE_H__0C370A93_1FDD_11D6_B4F4_00C04F4D4DED__INCLUDED_)
