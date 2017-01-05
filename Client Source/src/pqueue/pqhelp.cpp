/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// APCHelp.cpp: implementation of the CAPCHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pqueue.h"
#include "PQHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_HELP(CPQHelp)

CPQHelp::CPQHelp()
{

}

CPQHelp::~CPQHelp()
{

}

bool CPQHelp::SelectHelpTopics(int ctrl_id)
{
	bool found = true;


	switch (ctrl_id) 
	{
	
		//Add/Edit Output Type
		case IDC_COMBO1:		
			SetHelpTopics( HT_Application_s_); 
			break;
		case IDC_COMBO2:	
			SetHelpTopics( HT_Printers_PrintQueue); 
			break;
		default: found = false;
	}

	return found;

}
