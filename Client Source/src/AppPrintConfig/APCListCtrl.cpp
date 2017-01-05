/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// APCListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "setup2.h"
#include "APCListCtrl.h"
#include "APCHelp.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// APCListCtrl

APCListCtrl::APCListCtrl()
{
}

APCListCtrl::~APCListCtrl()
{
}


BEGIN_MESSAGE_MAP(APCListCtrl, MyListCtrl)
	//{{AFX_MSG_MAP(APCListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void APCListCtrl::SetHelpTopic(long lTopic)
{
  if(m_HelpTopics) //means there is help
   {
	   help_object.SetHelpTopics(/*m_WindowTopic,*/lTopic);
	   help_object.SemiAutomatic();
   }
}

/////////////////////////////////////////////////////////////////////////////
// APCListCtrl message handlers
