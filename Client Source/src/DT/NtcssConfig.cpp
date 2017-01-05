/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "ntcssconfig.h"



CNtcssConfig::CNtcssConfig()
{
	::GetWindowsDirectory(m_csWindowsDir.GetBuffer(_MAX_PATH),_MAX_PATH);
	m_csWindowsDir.ReleaseBuffer();
	SetPath(m_csWindowsDir+_T("\\ntcss.ini"));
	m_csIniFile=m_csWindowsDir+_T("\\ntcss.ini");

	ReadFile();
	m_csNtcssRootDir=GetValue(_T("NTCSS"),_T("NTCSS_ROOT_DIR")); 
	m_csNtcssSpoolDir=GetValue(_T("NTCSS"),_T("NTCSS_SPOOL_DIR"));
	m_bForceBanner=(GetValue(_T("NTCSS"),_T("FORCEBANNER"))==_T("NO")?FALSE:TRUE);
	m_csVCProgram=GetValue(_T("NTCSS"),_T("VCPROGRAM")); 
	m_csVersion=GetValue(_T("NTCSS"),_T("VERSION"));
	m_csXemulator=GetValue(_T("XEMULATOR"),_T("XPROGRAM"));
	m_csMasterServer=GetValue(_T("SERVER"),_T("NAME"));
	///////////////////////////////////////////////////////
	//Get Known Hosts Value change for Version xyz
	CString temp=GetValue(_T("NTCSS"),_T("QUERY_KNOWN_HOSTS")); 
	m_bQueryKnownHosts= temp.CompareNoCase(_T("YES"))==0;
	TRACE1(_T("QUERY_KNOWN_HOSTS is -> %s\n"),
	m_bQueryKnownHosts ? _T("TRUE") : _T("FALSE"));
	////////////  NEW STUFF FOR CITRIX  /////////////////////////////////////////
	m_bRunOnce=(GetValue(_T("NTCSS"),_T("RUNONCE"))==_T("NO")?FALSE:TRUE);
	m_bUDPOn=(GetValue(_T("NTCSS"),_T("UDP"))==_T("NO")?FALSE:TRUE);
	m_csIniAppNameTag=GetValue(_T("NTCSS"),_T("APPNAME"));
	if(m_csIniAppNameTag.IsEmpty())
		m_csIniAppNameTag=_T("GROUP");

	///////////// RIGHT TRIM EVERYTHING TO PROTECT AGAINST INSTAL.CFG BUG

	m_csNtcssRootDir.TrimRight();
	m_csNtcssSpoolDir.TrimRight();
	m_csVCProgram.TrimRight(); 
	m_csVersion.TrimRight();
	m_csXemulator.TrimRight();
	m_csMasterServer.TrimRight();
	m_csIniAppNameTag.TrimRight();

}

CNtcssConfig::~CNtcssConfig()
{
	
}

