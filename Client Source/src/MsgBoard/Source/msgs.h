/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\MsgBoard\msgs.h                    
//
// WSP, last update: TBD
//                                        
// This is the header file containing class declarations for all
// CNtcssMEM_MSG_ALLOC_Msg derived classes used in the MsgBoard 
// application. 
//
// CONTENTS:
// -------- 
// CmsgADDMSGBB			 
// CmsgCREATEMSGFILE			 
// CmsgDELMSGBB	 
// CmsgGETALLBBS
// CmsgGETBBMSGS
// CmsgGETMSGFILE
// CmsgGETNEWMSGFILE
// CmsgGETOWNBBMSGS
// CmsgGETSUBSCRIBEDBBS
// CmsgGETUNSUBSCRIBEDBBS
// CmsgMARKMSGREAD
// CmsgSENDBBMSG
// CmsgSUBSCRIBEBB
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_MSGS_H_
#define _INCLUDE_MSGS_H_

#include "ntcssmsg.h"       
#include "privapis.h"
#include "msgdefs.h"

#include "mb_mgr.h"    



/////////////////////////////////////////////////////////////////////////////
// CmsgADDMSGBB
// _________________

class CmsgADDMSGBB : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgADDMSGBB();
    
    ~CmsgADDMSGBB() {};

    BOOL Load(const char *bb_name);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL UnLoad();

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgCREATEMSGFILE
// _________________

class CmsgCREATEMSGFILE : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgCREATEMSGFILE();
    
    ~CmsgCREATEMSGFILE();
    
	BOOL Load(const char *msg_id, const char *msg_title);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

protected:

	 Screatemsgfile_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgDELMSGBB
// ____________

class CmsgDELMSGBB : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgDELMSGBB();
    
    ~CmsgDELMSGBB();

    BOOL Load(const char *bb_name);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL UnLoad();

protected:
	Sdelmsgbb_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETALLBBS
// ______________________
									
class CmsgGETALLBBS : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
	int m_num_msgboard_cnt;

public:

    CmsgGETALLBBS();
    
    ~CmsgGETALLBBS();
    
    BOOL Load();

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL GetListCount(int *msgboard_cnt_ptr);

    BOOL UnLoad(const int max_msgboard_cnt,
                int *num_msgboard_ptr,
                Sbasic_msgboard_rec *msgboard_list_ptr);

protected:
	 Sgetallbbs_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETBBMSGS
// _____________
									
class CmsgGETBBMSGS : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
	int m_num_msgs_cnt;

public:

    CmsgGETBBMSGS();
    
    ~CmsgGETBBMSGS();
    
    BOOL Load(char *bb_name);

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL GetListCount(int *msg_cnt_ptr);

    BOOL UnLoad(const int max_msgboard_cnt,
                int *num_msgboard_ptr,
                Sbasic_msglist_rec *msgboard_list_ptr);
protected:
	  Sgetbbmsgs_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETMSGFILE
// ______________

class CmsgGETMSGFILE : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgGETMSGFILE();
    
    ~CmsgGETMSGFILE();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(const char *msg_id);

    BOOL UnLoad(char *msg_path);

protected:
	Sgetmsgfile_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
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

protected:

	Sgetnewmsgfile_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETOWNBBMSGS
// ________________
									
class CmsgGETOWNBBMSGS : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
	int m_num_msgs_cnt;

public:

    CmsgGETOWNBBMSGS();
    
    ~CmsgGETOWNBBMSGS();
    
    BOOL Load();

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL GetListCount(int *msg_cnt_ptr);

    BOOL UnLoad(const int max_msgboard_cnt,
                int *num_msgboard_ptr,
                Sbasic_msglist_rec *msgboard_list_ptr);
protected:

	Sgetownbbmsgs_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS
// ______________________
									
class CmsgGETSUBSCRIBEDBBS : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
	int m_num_msgboard_cnt;

public:

    CmsgGETSUBSCRIBEDBBS();
    
    ~CmsgGETSUBSCRIBEDBBS();
    
    BOOL Load();

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL GetListCount(int *msgboard_cnt_ptr);

    BOOL UnLoad(const int max_msgboard_cnt,
                int *num_msgboard_ptr,
                Sbasic_msgboard_rec *msgboard_list_ptr);
protected:
	Sgetsubscribedbbs_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgGETUNSUBSCRIBEDBBS
// ______________________
									
class CmsgGETUNSUBSCRIBEDBBS : public CNtcssMEM_MSG_ADVALLOC_Msg
{
private:
	int m_num_msgboard_cnt;

public:

    CmsgGETUNSUBSCRIBEDBBS();
    
    ~CmsgGETUNSUBSCRIBEDBBS();
    
    BOOL Load();

    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL GetListCount(int *msgboard_cnt_ptr);

    BOOL UnLoad(const int max_msgboard_cnt,
                int *num_msgboard_ptr,
                Sbasic_msgboard_rec *msgboard_list_ptr);
protected:
	Sgetunsubscribedbbs_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgMARKMSGREAD
// ________________

class CmsgMARKMSGREAD : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgMARKMSGREAD();
    
    ~CmsgMARKMSGREAD();
    
    BOOL DoItNow()
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(const char *msg_id);

protected:
	Smarkmsgread_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
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

    BOOL Load(const char *msg_id, const char *bb_name);

protected:
	Ssendbbmsg_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};



/////////////////////////////////////////////////////////////////////////////
// CmsgSUBSCRIBEBB
// ________________

class CmsgSUBSCRIBEBB : public CNtcssMEM_MSG_ALLOC_Msg
{

public:

    CmsgSUBSCRIBEBB();
    
    ~CmsgSUBSCRIBEBB();
    
    BOOL DoItNow() 
        { return(CNtcssMEM_MSG_ALLOC_Msg::DoItNow()); };

    BOOL Load(const char *bb_name, BOOL subscribe);

protected:
	Ssubscribebb_svrmsgrec *svr_msg_ptr;

#ifdef _NTCSS_MSG_DEBUG_
    virtual void ShowMsg(CString *show_buf);
#endif
};





#endif // _INCLUDE_MSGS_H_



