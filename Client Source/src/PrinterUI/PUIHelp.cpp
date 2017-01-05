/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PUIHelp.cpp: implementation of the CPUIHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrinterUI.h"
#include "PUIHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_HELP(CPUIHelp)

CPUIHelp::CPUIHelp()
{

}

CPUIHelp::~CPUIHelp()
{

}

bool CPUIHelp::SelectHelpTopics(int ctrl_id)
{
	bool found = true;

/*	CString temp;
	temp.Format("Ctrl ID is -> %d",ctrl_id);
	AfxMessageBox(temp); */

	switch (ctrl_id) 
	{
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Main Dialog SLE's

		case IDC_EDIT1:	SetHelpTopics( HT_Location_Printer); break;
		case IDC_EDIT2:	SetHelpTopics( HT_MaxRequestSize); break;
		case IDC_EDIT3:	SetHelpTopics( HT_PrinterClass); break;
		case IDC_EDIT4:	SetHelpTopics( HT_AdapterIP); break;
		case IDC_EDIT5:	SetHelpTopics( HT_AdapterType); break;
		case IDC_EDIT6:	SetHelpTopics( HT_AdapterPort); break;
		case IDC_EDIT7:	SetHelpTopics( HT_SuspendState); break;
		case IDC_EDIT8:	SetHelpTopics( HT_RedirectPrinter); break;
		case IDC_EDIT9:	SetHelpTopics( HT_PrinterAccess); break;

		//Main Dailog LB
//		case IDC_LIST3:	SetHelpTopics( HT_Application_s_ ); break; TODO hook to LV


		//Add/Edit Printer Dialog
		case IDC_NAME:				SetHelpTopics( HT_Name_Printer); break;
		//case IDC_HOSTNAME:			SetHelpTopics( HT_Host_Printer); break;
		case IDC_CLASSIFICATION:	SetHelpTopics( HT_Classification_Printer); break;
		case IDC_NTDRIVER:			SetHelpTopics( HT_NTDriver); break;
		case IDC_PrinterAccess:		SetHelpTopics( HT_PrinterAccess); break;
		case IDC_LOCATION:			SetHelpTopics( HT_Location_Printer); break;
		case MAXREQUESTSIZE:		SetHelpTopics( HT_MaxRequestSize); break;
		case IDC_PRINTERCLASS:		SetHelpTopics( HT_PrinterClass); break;
		case IDC_ADAPTERPORT:		SetHelpTopics( HT_AdapterPort); break;
		case IDC_SUSPENDSTATE:		SetHelpTopics( HT_SuspendState); break;
		case IDC_REDIRECTPRINTER:	SetHelpTopics( HT_RedirectPrinter); break;

		//Add/Edit Printer Application LB
		case IDC_APPLICATION_LIST:	SetHelpTopics( HT_ApplicationList); break;


		//Add/Edit Adpater Dialog
		case IDC_HOSTNAME_ADAPTER_EDIT:	SetHelpTopics( HT_Hostname_Adapter); break;
	//	case IDC_IPAddress:				SetHelpTopics( HT_IPAddress); break;
		case IDC_IPADDR:				SetHelpTopics( HT_IPAddress); break;

		case IDC_MachineAddress:		SetHelpTopics( HT_MachineAddress); break;
		case IDC_LOCATION_ADAPTER_EDIT:	SetHelpTopics( HT_Location_Adapter); break;
		case IDC_TypeCombo:			SetHelpTopics( HT_Type_Adapter); break;
		
        //Select NT Printer Window
		case IDC_LIST1:	SetHelpTopics( HT_Manufacturers); break;
		case IDC_LIST2:	SetHelpTopics( HT_Printers_NTPrinter); break;
		case IDC_CURRENTINF:	SetHelpTopics( HT_CurrentINF); break;

		case IDC_DRIVES: SetHelpTopics( HT_CopyManufacturer_sfilesfrom); break;

		default: found = false;

		//AfxMessageBox(found ? "FOUND is TRUE" : "FOUND is FALSE");
	}

	
	return found;

}
