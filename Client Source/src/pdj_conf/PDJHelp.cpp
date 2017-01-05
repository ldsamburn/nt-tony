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
#include "pdj_conf.h"
#include "PDJHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_HELP(CPDJHelp)

CPDJHelp::CPDJHelp()
{

}

CPDJHelp::~CPDJHelp()
{

}

bool CPDJHelp::SelectHelpTopics(int ctrl_id)
{
	bool found = true;


	switch (ctrl_id) 
	{
	
		//Add/Edit Output Type
		case IDC_PDJ_TITLE:		
			SetHelpTopics(/*HT_BatchJobQueue_WindowDescriptions,*/ HT_Title_PredefinedJob); 
			break;
		case IDC_PDJ_COMMAND:	
			SetHelpTopics(/*HT_BatchJobQueue_WindowDescriptions,*/ HT_Command); 
			break;
		case IDC_ROLE_LIST:	
			SetHelpTopics(/*HT_BatchJobQueue_WindowDescriptions,*/ HT_AccessRoles); 
			break;
		case IDC_JOB_TYPE:	
			SetHelpTopics(/*HT_BatchJobQueue_WindowDescriptions,*/ HT_Type_PredefinedJob); 
			break;
		case IDC_APPLICATION_LIST:	
			SetHelpTopics(/*HT_BatchJobQueue_WindowDescriptions,*/ HT_ApplicationList); 
			break;
		default: found = false;
	}

	return found;

}
