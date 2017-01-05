/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PUIListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "setup2.h"
#include "PUIListCtrl.h"
#include "PUIHelp.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// PUIListCtrl

PUIListCtrl::PUIListCtrl()
{
}

PUIListCtrl::~PUIListCtrl()
{
}


BEGIN_MESSAGE_MAP(PUIListCtrl, MyListCtrl)
	//{{AFX_MSG_MAP(PUIListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PUIListCtrl::SetHelpTopic(long lTopic)
{
  if(m_HelpTopics) //means there is help
   {
	   help_object.SetHelpTopics(lTopic);
	   help_object.SemiAutomatic();
   }
}

/////////////////////////////////////////////////////////////////////////////
// PUIListCtrl message handlers
