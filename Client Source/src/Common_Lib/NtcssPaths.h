/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssPaths.h: interface for the CNtcssPaths class.
//
//////////////////////////////////////////////////////////////////////

#ifndef NTCSS_PATHS_H
#define NTCSS_PATHS_H

class CNtcssPaths  
{
public:
	CNtcssPaths();
	virtual ~CNtcssPaths();

	CString AddPath(CString path, CString file_name);

	CString WindowsDirectory(CString file_name = "")       {return AddPath(m_windows_directory,file_name);}
	CString NtcssRootDirectory(CString file_name = "")     {return AddPath(m_ntcss_root_directory,file_name);}
	CString NtcssSpoolDirectory(CString file_name = "")    {return AddPath(m_spool_directory,file_name);}
	CString NtcssBinDirectory(CString file_name = "")      {return AddPath(m_ntcss_bin_directory,file_name);}
	CString NtcssHelpDirectory(CString file_name = "")     {return AddPath(m_ntcss_help_directory,file_name);}
	CString NtcssResourceDirectory(CString file_name = "") {return AddPath(m_ntcss_resource_directory,file_name);}
	CString NtcssIniFile() {return WindowsDirectory("ntcss.ini");} 

	CString LoadFile(CString file_name)       { return NtcssSpoolDirectory("load_"  + file_name) + ".ini"; }
	CString StoreFile(CString file_name)      { return NtcssSpoolDirectory("store_" + file_name) + ".ini"; }
	CString ParameterFile(CString file_name)  { return NtcssSpoolDirectory("param_" + file_name) + ".ini"; }
	CString StashFile(CString file_name)      { return NtcssSpoolDirectory("stash_" + file_name) + ".ini"; }

	CString RootKey()  {return "Software";}
	CString NtcssKey() {return RootKey() + "\\NTCSS";}


protected:
	CString m_ntcss_ini_file;
	CString m_windows_directory;
	CString m_ntcss_root_directory;
	CString m_spool_directory;
	CString m_ntcss_bin_directory;
	CString m_ntcss_help_directory;
	CString m_ntcss_resource_directory;

};

#endif
