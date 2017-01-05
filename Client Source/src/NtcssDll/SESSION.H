/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Session.h                    
//                                         
// This is the header file for the Csession class.
//
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_SESSION_H_
#define _INCLUDE_SESSION_H_

                                       // Necessary forward declaration
class Csession;

#include "err_man.h"
#include "ntcssdef.h"
#include "ntcssSock.h"
#include "dll_defs.h"


class Csession // : public CObject     
{
    friend class CdllManager;

// Constructors
private:
                                        // Is okay that constructor
                                        //  /destructor is private as
                                        //  our friend the CdllManager
                                        //  should be the only one making
                                        //  new sessions.
                                        //
    Csession(HINSTANCE m_hInstance, HWND m_hWnd, 
             CString m_strAppName, int ID, BOOL need_socket ); 

// Attributes
public:

   BOOL ID()
        { return(m_nID); };

    const SlocalPrinterInfo* localPrinterInfo()
        { return(m_pLocalPrinterInfo); };

    const SremotePrinterInfo* remotePrinterInfo()
        { return(m_pRemotePrinterInfo); };

    BOOL isBusy()
        { return(m_bBusy); };

    BOOL isLocalPrinterInfoSet()
        {return(m_pLocalPrinterInfo != NULL); };

    BOOL isRemotePrinterInfoSet()
        {return(m_pRemotePrinterInfo != NULL); };

    const CString ssnAppName()
        { return (m_strAppName); };

    const HINSTANCE ssnhInstance()
        { return (m_hInstance); };

    HWND ssnHWnd()
        { return (m_hWnd); };

 //   CNtcssSock *ssnSocket()
     //   { return (m_pSock); };

    CErrorManager *ssnErrorMan()
        { return(&m_ErrMan); };

// Operations
public:

    BOOL isLocalPrinterMru()
        {return(m_bLocalPrinterMruFlag); };

    BOOL setLocalPrinterInfo(const SlocalPrinterInfo *info, 
                             BOOL  set_mru);

    BOOL setRemotePrinterInfo(const SremotePrinterInfo *info, 
                              BOOL  set_mru);

// Overrides

// Implementation - protected data
protected:
    

    HWND                m_hWnd;         // App's Window
    HINSTANCE           m_hInstance;    // App's Instance
    CString             m_strAppName;   // Application Name
 //   CNtcssSock          *m_pSock;        // Socket for a task 
    CErrorManager       m_ErrMan;       // Every session has own err-mgr
    int                 m_nID;          // Internal ID
    BOOL                m_bBusy;         // Currently busy?
                                        // Local or remote printer
                                        //  most recently used?
    BOOL                m_bLocalPrinterMruFlag;

                                        // Ptr to printer structs
										//
    SlocalPrinterInfo     *m_pLocalPrinterInfo;
    SremotePrinterInfo    *m_pRemotePrinterInfo;
                                        

// Implementation - destructor
public:
    virtual ~Csession();

// Implementation - CdllManager calls to reserve/release the Csession
protected:

    void DMcall_release()
        { m_bBusy = FALSE; };

    BOOL DMcall_reserve();
    

};

#endif //_INCLUDE_SESSION_H_

