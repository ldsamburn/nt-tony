/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Session.cpp
//                                         
// This is the implementation file for Csession.
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include "0_basics.h"
#include "session.h"



/////////////////////////////////////////////////////////////////////////////
// Csession constuctor
// ___________________

Csession::Csession
		(HINSTANCE hInstance, HWND hWnd, CString app_name, 
		 int ID,  BOOL need_socket )
{
	m_hWnd		= hWnd;        
	m_hInstance = hInstance;	
    m_strAppName	= app_name;     

	m_nID = ID;		
	m_bBusy = FALSE;

    m_bLocalPrinterMruFlag     = FALSE;
    m_pLocalPrinterInfo  = NULL;
    m_pRemotePrinterInfo = NULL;
	
 /*   if(need_socket) //everyone but secban
	{
		m_pSock = new CNtcssSock(this);
		if(!m_pSock->Create())
		{
			CString temp;
			temp.Format("Socket Creation failed-> %d",m_pSock->GetLastError());
			AfxMessageBox(temp);
		}
		else
			m_pSock->SetTimeoutLength(5800);
	}
	else */
//		m_pSock = NULL;
}



/////////////////////////////////////////////////////////////////////////////
// Csession destuctor
// __________________

Csession::~Csession()
{
                                       // Trash m_pLocalPrinterInfo
                                       //  if it exists
    if (m_pLocalPrinterInfo != NULL)
    {
        delete m_pLocalPrinterInfo;
    }
                                       // Trash m_pRemotePrinterInfo
                                       //  if it exists
    if (m_pRemotePrinterInfo != NULL)
    {
        delete m_pRemotePrinterInfo;
    }
}



/////////////////////////////////////////////////////////////////////////////
// DMcall_reserve
// ______________

BOOL Csession::DMcall_reserve()
{
	if (!m_bBusy)
	{
		m_bBusy = TRUE;
		return(TRUE);
	}
	else
		return(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// setLocalPrinterInfo
// ___________________

BOOL Csession::setLocalPrinterInfo(const SlocalPrinterInfo *src_ptr,
                                   BOOL  set_mru)
{
                                      // First allocate space for
                                      //  the SlocalPrinterInfo
    if (m_pLocalPrinterInfo == NULL)
    {
        m_pLocalPrinterInfo = new SlocalPrinterInfo;
        if (m_pLocalPrinterInfo == NULL)
        {
            m_ErrMan.RecordError("Memory allocation error");
            return(FALSE);
        }
    }
                                       // Here copy over the base
                                       //  SlocalPrinterInfo info
                                       //
    memcpy(m_pLocalPrinterInfo, src_ptr, sizeof(SlocalPrinterInfo));


    if (set_mru)
        m_bLocalPrinterMruFlag = TRUE;
        
    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// setRemotePrinterInfo
// ____________________

BOOL Csession::setRemotePrinterInfo(const SremotePrinterInfo *src_ptr,
                                    BOOL  set_mru)
{
                                      // First allocate space for
                                      //  the SremotePrinterInfo
    if (m_pRemotePrinterInfo == NULL)
    {
        m_pRemotePrinterInfo = new SremotePrinterInfo;
        if (m_pRemotePrinterInfo == NULL)
        {
            m_ErrMan.RecordError("Memory allocation error");
            return(FALSE);
        }
    }
                                       // Here copy over the base
                                       //  SremotePrinterInfo info
                                       //
    memcpy(m_pRemotePrinterInfo, src_ptr, sizeof(SremotePrinterInfo));


    if (set_mru)
        m_bLocalPrinterMruFlag = FALSE;
        
    return(TRUE);
}
