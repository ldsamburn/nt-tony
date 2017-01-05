/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Dll_Msgs.h                    
//                                         
// This is the header file for several NTCSS DLL message
// classes derived from CntcssBaseMsg.
//
// CONTENTS:
//   - CmsgADMINLOCKOPS
//   - CmsgCHANGEAPPDATA
//   - CmsgCREATEMSGFILE  *(duplicated in MsgBoard app)	
//   - CmsgGETAPPDATA		 
//   - CmsgGETAPPLIST
//   - CmsgGETAPPUSERINFO
//   - CmsgGETAPPUSERLIST
//   - CmsgGETCOMMONDB
//   - CmsgGETDIRLISTING
//   - CmsgGETFILEINFO
//   _ CmsgGETFILESECTION
//   _ CmsgGETMASTERNAME
//   - CmsgGETNEWMSGFILE  *(duplicated in MsgBoard app)
//   - CmsgGETUSERPRTLIST
//   - CmsgKILLSVRPROC
//   - CmsgLAUNCHSVRPROCBG
//   - CmsgPRINTSVRFILE
//   - CmsgREGISTERAPPUSER
//   - CmsgREMOVESPQITEM
//   - CmsgSCHEDSVRPROCBG
//   - CmsgSENDBBMSG      *(duplicated in MsgBoard app)
//   - CmsgSETCOMMONDB
//   - CmsgSETLINKDATA
//   - CmsgSVRPROCLIST
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_DLL_MSGS_H_CLASS_
#define _INCLUDE_DLL_MSGS_H_CLASS_

#include "ntcssmsg.h"       
#include "ntcssapi.h"
#include "msgdefs.h"
#include "privapis.h"
#include "Dll_defs.h"

/////////////////////////////////////////////////////////////////////////////
// CmsgADMINLOCKOPS
// __________________

class CmsgADMINLOCKOPS : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgADMINLOCKOPS();
    
    ~CmsgADMINLOCKOPS();
    
    BOOL Load(LPCTSTR appname,LPCTSTR code,
		      LPCTSTR action,LPCTSTR pid,
			  LPCTSTR hostname);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	
	BOOL UnLoad(int *pidvalue);

private:

    Sadminlockops_svrmsgrec *svr_msg_ptr;      
   
};

/////////////////////////////////////////////////////////////////////////////
// CmsgCHANGEAPPDATA
// __________________

class CmsgCHANGEAPPDATA : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgCHANGEAPPDATA();
    
    ~CmsgCHANGEAPPDATA();
    
    BOOL Load(LPCTSTR appname, LPCTSTR appdata);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Schangeappdata_svrmsgrec *svr_msg_ptr;    
};



/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE
// _________________

class CmsgCREATEMSGFILE : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgCREATEMSGFILE();
    
    ~CmsgCREATEMSGFILE();
    
	BOOL Load(LPCTSTR msg_id, LPCTSTR msg_title);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Screatemsgfile_svrmsgrec *svr_msg_ptr; 
};

/////////////////////////////////////////////////////////////////////////////
// CmsgDELETESVRFILE
// __________________

class CmsgDELETESVRFILE : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgDELETESVRFILE();
    
    ~CmsgDELETESVRFILE();
    
    BOOL Load(LPCTSTR SvrName,LPCTSTR AppName,LPCTSTR FileName);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Sdeletesvrfile_svrmsgrec *svr_msg_ptr; 

};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPDATA
// __________________

class CmsgGETAPPDATA : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgGETAPPDATA();
    
    ~CmsgGETAPPDATA();
    
    BOOL Load(LPCTSTR strAppName);

	BOOL UnLoad(LPTSTR strAppData);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Sgetappdata_svrmsgrec *svr_msg_ptr; 
};


/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPLIST
// ______________

class CmsgGETAPPLIST : public CNtcssMEM_MSG_ALLOC_Msg
{
    int m_num_apps_cnt;

public:

    CmsgGETAPPLIST();
    
    ~CmsgGETAPPLIST();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load();

    BOOL getAppCount(int *num_apps_ptr);

    BOOL UnLoad(const int max_apps_cnt,
                int *num_apps_ptr,
                NtcssAppList *app_list_ptr);

private:

    Sgetapplist_svrmsgrec *svr_msg_ptr; 
};




/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERINFO
// __________________

class CmsgGETAPPUSERINFO : public CNtcssMEM_MSG_ALLOC_Msg
{
private:
    char m_username[NTCSS_SIZE_LOGINNAME+1];

public:

    CmsgGETAPPUSERINFO();
    
    ~CmsgGETAPPUSERINFO();
    
    BOOL Load(LPCTSTR app_name, LPCTSTR username);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL UnLoad(BOOL *registered_ptr, 
                NtcssUserInfo *user_info_ptr,
                char *access_info_ptr,const char* appname);
private:

    Sgetappuserinfo_svrmsgrec *svr_msg_ptr; 


};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETAPPUSERLIST
// __________________

class CmsgGETAPPUSERLIST : public CNtcssMEM_MSG_ADVALLOC_Msg
{
    int m_num_users_cnt;

public:

    CmsgGETAPPUSERLIST();
    
    ~CmsgGETAPPUSERLIST();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR app_name);

    BOOL getUserCount(int *num_users_ptr);

    BOOL UnLoad(const int max_user_cnt,
                int *num_users,
                NtcssAppUserList *user_list_ptr);
private:

    Sgetappuserlist_svrmsgrec *svr_msg_ptr; 
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETCOMMONDB
// __________________

class CmsgGETCOMMONDB : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgGETCOMMONDB();
    
    ~CmsgGETCOMMONDB();
    
    BOOL Load(LPCTSTR tag);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL UnLoad(char *data);

private:

    Sgetcommondb_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETDIRLISTING
// ___________________

class CmsgGETDIRLISTING : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
    int m_num_files_cnt;

public:

    CmsgGETDIRLISTING();
    
    ~CmsgGETDIRLISTING();
    
    BOOL Load(LPCTSTR server_name,
              LPCTSTR dir_name);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL getFileCount(int *num_files_ptr);

    BOOL UnLoad(SUnixDirInfo *dir_list_ptr,
                int  size_of_dir_list,
                int  *num_dirs_in_list);

private:

    Sgetdirlisting_svrmsgrec *svr_msg_ptr;
};
   
/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILEINFO
// __________________

class CmsgGETFILEINFO : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgGETFILEINFO();
    
    ~CmsgGETFILEINFO();
    
    BOOL Load(LPCTSTR server,LPCTSTR fname);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ADVALLOC_Msg::DoItNow()); };

	BOOL UnLoad(int *lines,int *bytes);

private:

    Sgetfileinfo_svrmsgrec *svr_msg_ptr;
};


/////////////////////////////////////////////////////////////////////////////
// CmsgGETFILESECTION
// __________________

class CmsgGETFILESECTION : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgGETFILESECTION();
    
    ~CmsgGETFILESECTION();
    
    BOOL Load(LPCTSTR server,LPCTSTR fname,LPCTSTR FirstLine,
		      LPCTSTR LastLine);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ADVALLOC_Msg::DoItNow()); };

	BOOL UnLoad(int *lines,char *buf);
       
private:

    Sgetfilesection_svrmsgrec *svr_msg_ptr;
};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETLINKDATA
// __________________

class CmsgGETLINKDATA : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgGETLINKDATA();
    
    ~CmsgGETLINKDATA();
    
    BOOL Load(LPCTSTR appname);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ADVALLOC_Msg::DoItNow()); };

	BOOL UnLoad(char *LinkData);
       
private:

    Sgetlinkdata_svrmsgrec *svr_msg_ptr;
};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETMASTERNAME
// __________________

class CmsgGETMASTERNAME : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgGETMASTERNAME();
    
    ~CmsgGETMASTERNAME();

	BOOL Load();
       
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL UnLoad(char *strMasterName);
               
//private:
//template <class Sgetmastername_svrmsgrec>

//	auto_ptr<Sgetmastername_svrmsgrec> svr_msg_ptr;
    Sgetmastername_svrmsgrec *svr_msg_ptr;
};

/////////////////////////////////////////////////////////////////////////////
// CmsgGETNEWMSGFILE
// _________________

class CmsgGETNEWMSGFILE : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgGETNEWMSGFILE();
    
    ~CmsgGETNEWMSGFILE();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load();

    BOOL UnLoad(char *msg_path);

private:

    Sgetnewmsgfile_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUSERPRTLIST
// __________________

class CmsgGETUSERPRTLIST : public CNtcssMEM_MSG_ADVALLOC_Msg
{
    int m_num_printers;

public:

    CmsgGETUSERPRTLIST();
    
    ~CmsgGETUSERPRTLIST();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL getPrinterCount(int *num_printers_ptr);

    BOOL Load(LPCTSTR the_server_name,
              LPCTSTR the_app_name, 
              LPCTSTR the_output_type,
              const SecurityLevel the_security_class,
              const BOOL the_default_only_flag,
              const BOOL the_access_flag);

    BOOL UnLoad(SremotePrinterInfo *printer_list_ptr,
                int max_printers);

private:

    Sgetuserprtlist_svrmsgrec *svr_msg_ptr;
};


/////////////////////////////////////////////////////////////////////////////
// CmsgKILLSVRPROC
// ___________________

class CmsgKILLSVRPROC : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgKILLSVRPROC();
    
    ~CmsgKILLSVRPROC();
    
    BOOL Load(LPCTSTR pid_buf,
              const int signal_type,
			  LPCTSTR program_group);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Skillsvrproc_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgLAUNCHSVRPROCBG
// ___________________

class CmsgLAUNCHSVRPROCBG : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgLAUNCHSVRPROCBG();
    
    ~CmsgLAUNCHSVRPROCBG();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	BOOL Load(const NtcssServerProcessDescr *descr_ptr,
			  LPCTSTR app_name_str, 
			  LPCTSTR unix_owner_str,
			  LPCTSTR prt_name_str, 
			  LPCTSTR prt_hostname_str);

    BOOL UnLoad(char *pid_buf);

private:

    Slaunchsvrprocbg_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgPRINTSVRFILE
// ___________________

class CmsgPRINTSVRFILE : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgPRINTSVRFILE();
    
    ~CmsgPRINTSVRFILE();
    
    BOOL Load(LPCTSTR the_server,
              LPCTSTR the_prt_file,
              LPCTSTR the_prt_name,
              LPCTSTR the_progroup_title,
			  const SecurityLevel the_security_class,
			  LPCTSTR copies,
			  LPCTSTR options);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

	private:

    Sprintsvrfile_svrmsgrec *svr_msg_ptr;

};


/////////////////////////////////////////////////////////////////////////////
// CmsgREGISTERAPPUSER
// ___________________

class CmsgREGISTERAPPUSER : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgREGISTERAPPUSER();
    
    ~CmsgREGISTERAPPUSER();
    
    BOOL Load(LPCTSTR the_user_name, 
              LPCTSTR the_app_name, 
              const char the_flag);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Sregisterappuser_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgREMOVESPQITEM
// _________________

class CmsgREMOVESPQITEM : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgREMOVESPQITEM();
    
    ~CmsgREMOVESPQITEM();
    
    BOOL Load(LPCTSTR pid_buf,
              const BOOL scheduled_job,
			  LPCTSTR program_group);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Sremovespqitem_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgSCHEDSVRPROCBG
// ___________________

class CmsgSCHEDSVRPROCBG : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgSCHEDSVRPROCBG();
    
    ~CmsgSCHEDSVRPROCBG();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

               

	BOOL Load(LPCTSTR schedule,
  	  		  const NtcssServerProcessDescr *descr_ptr,
			  LPCTSTR app_name_str, 
			  LPCTSTR unix_owner_str,
			  LPCTSTR prt_name_str, 
			  LPCTSTR prt_hostname_str);

    BOOL UnLoad(char *pid_buf);

private:

    Sschedsvrprocbg_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgSENDBBMSG
// ________________

class CmsgSENDBBMSG : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgSENDBBMSG();
    
    ~CmsgSENDBBMSG();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(LPCTSTR msg_id, LPCTSTR bb_name);

private:

    Ssendbbmsg_svrmsgrec *svr_msg_ptr;
};



/////////////////////////////////////////////////////////////////////////////
// CmsgSETCOMMONDB
// __________________

class CmsgSETCOMMONDB : public CNtcssMEM_MSG_ALLOC_Msg
{
public:

    CmsgSETCOMMONDB();
    
    ~CmsgSETCOMMONDB();
    
    BOOL Load(LPCTSTR key,LPCTSTR data);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

private:

    Ssetcommondb_svrmsgrec *svr_msg_ptr;
};


/////////////////////////////////////////////////////////////////////////////
// CmsgSETLINKDATA
// __________________

class CmsgSETLINKDATA : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgSETLINKDATA();
    
    ~CmsgSETLINKDATA();
    
    BOOL Load(LPCTSTR appname, LPCTSTR app_password);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };


private:

    Ssetlinkdata_svrmsgrec *svr_msg_ptr;
};

/////////////////////////////////////////////////////////////////////////////
// CmsgSVRPROCLIST
// ___________________


class CmsgSVRPROCLIST : public CNtcssMEM_MSG_ADVALLOC_Msg
{

private:
    int  m_num_processes_cnt;
    BOOL m_doing_scheduled_jobs;

public:

    CmsgSVRPROCLIST();
    
    ~CmsgSVRPROCLIST();
    
    BOOL Load(LPCTSTR appname,
              const int  role,
              const BOOL do_scheduled_jobs,
              NtcssProcessFilter *filter);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL getProcessCnt(int *num_processes_ptr);

    BOOL UnLoad(NtcssServerProcessDetails *svr_proc_list_ptr,
                int   svr_proc_list_slots,
                int  *num_put_in_svr_proc_list);

private:

    Ssvrproclist_svrmsgrec *svr_msg_ptr;
};



#endif // _INCLUDE_DLL_MSGS_H_CLASS_


/////////////////////////////////////////////////////////////////////////////
// CmsgVALIDATEUSER
// __________________

class CmsgVALIDATEUSER : public CNtcssMEM_MSG_ADVALLOC_Msg
{
public:

    CmsgVALIDATEUSER();
    
    ~CmsgVALIDATEUSER();
    
    BOOL Load(LPCTSTR username,LPCTSTR password, LPCTSTR appname);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL UnLoad(char *ssn);

private:

    Svalidateuser_svrmsgrec *svr_msg_ptr;
};


