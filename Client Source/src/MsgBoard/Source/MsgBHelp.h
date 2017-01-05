/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgBHelp.h: interface for the CMsgBHelp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MsgBHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)
#define AFX_MsgBHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_

#include "NtcssHelp.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMsgBHelp : public CNtcssHelp  
{
public:
	CMsgBHelp();
	virtual ~CMsgBHelp();

	virtual bool SelectHelpTopics(int ctrl_id);
};

DECLARE_HELP(CMsgBHelp)

#endif // !defined(AFX_MsgBHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)