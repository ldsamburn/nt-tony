/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ServerService.h: interface for the ServerService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSERVICE_H__1A478D03_F565_11D5_B4E7_00C04F4D4DED__INCLUDED_)
#define AFX_SERVERSERVICE_H__1A478D03_F565_11D5_B4E7_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ServiceClass.h"

class ServerService : public ServiceClass  
{
public:	
    ServerService(const char*, int, int, LPCTSTR);
	
    virtual void Run();
    virtual void OnStop();	

};

#endif // !defined(AFX_SERVERSERVICE_H__1A478D03_F565_11D5_B4E7_00C04F4D4DED__INCLUDED_)
