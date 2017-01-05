/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PDJHelp.h: interface for the CPDJHelp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PDJHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)
#define AFX_PDJHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_

#include "NtcssHelp.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPDJHelp : public CNtcssHelp  
{
public:
	CPDJHelp();
	virtual ~CPDJHelp();

	virtual bool SelectHelpTopics(int ctrl_id);
};

DECLARE_HELP(CPDJHelp)

#endif // !defined(AFX_PDJHelp_H__8DB07072_BA7E_11D1_B3FC_00A024C3D7DB__INCLUDED_)
