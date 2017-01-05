
/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#if !defined(AFX_NTCSSCONFIG_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)
#define AFX_NTCSSCONFIG_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IniFile.h"

class CNtcssConfig: public CIniFile
{

friend class CDTDlg;
friend class CDTApp;

public:
	CNtcssConfig();
	~CNtcssConfig();
	CString m_csNtcssRootDir;
	
private:
	CString m_csWindowsDir;
	CString m_csNtcssSpoolDir;
	BOOL	m_bForceBanner;
	CString m_csVCProgram;
	int		m_nNTDelay;
	CString m_csVersion;
	CString m_csReleaseNotes;
	CString m_csXemulator;
	int		m_nLPRSeq;
	CString m_csMasterServer;
//	CStringEx m_csExServers;
	CString m_csIniAppNameTag;
	//new INI entries for Citrix problems
	BOOL	m_bUDPOn;
	BOOL    m_bRunOnce;
	BOOL    m_bUseShare;
	BOOL    m_bQueryKnownHosts;
	CString m_csIniFile;
};

#endif // !defined(AFX_NTCSSCONFIG_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)