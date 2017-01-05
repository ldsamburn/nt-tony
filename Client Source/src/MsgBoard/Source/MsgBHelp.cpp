/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// MsgBHelp.cpp: implementation of the CMsgBHelp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Msgboard.h"
#include "MsgBHelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_HELP(CMsgBHelp)

CMsgBHelp::CMsgBHelp()
{

}

CMsgBHelp::~CMsgBHelp()
{

}

bool CMsgBHelp::SelectHelpTopics(int ctrl_id)
{
	bool found = true;


	switch (ctrl_id) 
	{
	
		//Add/Edit Output Type
		case IDC_POST_TO:			SetHelpTopics( HT_Postto); break;
		case IDC_TITLE_TEXT:		SetHelpTopics( HT_Title_Message ); break;
		case IDC_MESSAGE_TEXT:		SetHelpTopics( HT_MessageText); break;
		case IDC_MSGBOARD_NAME:		SetHelpTopics(  HT_EnternewMessageBoardname); break;
	


		default: found = false;
	}




	return found;

}

/*

// MessageBoard main window

// New Message Board window
(HT_NewMessageBoard_WindowDescription, HT_EnternewMessageBoardname);

// Main window - New Message
(HT_MsgboardWindowsApplication__NewMessage__WindowDescription, HT_Postto);
(HT_MsgboardWindowsApplication__NewMessage__WindowDescription, HT_Title_Message);
(HT_MsgboardWindowsApplication__NewMessage__WindowDescription, HT_MessageText);

// New Message window
(HT_NewMessage_WindowDescription, HT_Postto);
(HT_NewMessage_WindowDescription, HT_Title_Message);
(HT_NewMessage_WindowDescription, HT_MessageText);

*/
