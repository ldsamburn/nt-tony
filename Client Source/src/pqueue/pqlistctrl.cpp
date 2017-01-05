/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// CPQListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "setup2.h"
#include "PQListCtrl.h"
#include "PQHelp.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPQListCtrl

CPQListCtrl::CPQListCtrl()
{
}

CPQListCtrl::~CPQListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPQListCtrl, MyListCtrl)
	//{{AFX_MSG_MAP(CPQListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPQListCtrl::SetHelpTopic(long lTopic)
{
  if(m_HelpTopics) //means there is help
   {
	   help_object.SetHelpTopics(/*m_WindowTopic,*/lTopic);
	   help_object.SemiAutomatic();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPQListCtrl message handlers
