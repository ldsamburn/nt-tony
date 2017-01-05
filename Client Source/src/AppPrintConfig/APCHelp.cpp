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
#include "AppPrintConfig.h"
#include "APCHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_HELP(CAPCHelp)

CAPCHelp::CAPCHelp()
{

}

CAPCHelp::~CAPCHelp()
{

}

bool CAPCHelp::SelectHelpTopics(int ctrl_id)
{
	bool found = true;


	switch (ctrl_id) 
	{
	
		//Add/Edit Output Type
		case IDC_NAME:				SetHelpTopics(/*HT_EditOutputType_WindowDescription,*/ HT_Name_OutputType); break;
		case IDC_APPLICATION:		SetHelpTopics(/*HT_EditOutputType_WindowDescription,*/ HT_Application_s_ ); break;
		case IDC_DESCRIPTION:		SetHelpTopics(/*HT_EditOutputType_WindowDescription,*/ HT_Description_OutputType); break;
		case IDC_DEFAULTPRINTER:	SetHelpTopics(/*HT_EditOutputType_WindowDescription,*/ HT_DefaultPrinter); break;
		case IDC_PRINTERLIST:		SetHelpTopics(/*HT_EditOutputType_WindowDescription,*/ HT_NetworkPrinterList); break;
		case IDC_APPLIST:			SetHelpTopics(/*HT_ApplicationPrinterConfiguration_WindowDescription,*/ HT_ApplicationList); break;

		default: found = false;
	}




	return found;

}
