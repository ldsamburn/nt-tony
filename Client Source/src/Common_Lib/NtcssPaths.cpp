/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssPaths.cpp: implementation of the CNtcssPaths class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "SystemMonitor.h" jj
#include "NtcssPaths.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNtcssPaths::CNtcssPaths()
{
	::GetWindowsDirectory(m_windows_directory.GetBuffer(MAX_PATH),MAX_PATH);
	m_windows_directory.ReleaseBuffer();

	GetPrivateProfileString("NTCSS",
							"NTCSS_ROOT_DIR",
							m_windows_directory,
							m_ntcss_root_directory.GetBuffer(MAX_PATH),
							MAX_PATH,
							NtcssIniFile());

	m_ntcss_root_directory.ReleaseBuffer();

	GetPrivateProfileString("NTCSS",
							"NTCSS_SPOOL_DIR",
							m_windows_directory,
							m_spool_directory.GetBuffer(MAX_PATH),
							MAX_PATH,
							NtcssIniFile());

	m_spool_directory.ReleaseBuffer();

	m_ntcss_resource_directory = NtcssRootDirectory("Resource");
	m_ntcss_bin_directory      = NtcssRootDirectory("Bin");
	m_ntcss_help_directory     = NtcssRootDirectory("Help");

}


CNtcssPaths::~CNtcssPaths()
{

}


CString CNtcssPaths::AddPath(CString path, CString file_name)
{
	if (file_name.IsEmpty()) return path;
	CString temp;
	temp.Format("%s\\%s",path,file_name);
	return temp;
}
