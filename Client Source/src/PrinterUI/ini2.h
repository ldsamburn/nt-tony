/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


const int MAX_INI_SECTION2=65535;
const int MAX_INI_LINE=1024;

class CIni2
{


protected:

	char m_cBuf[MAX_INI_SECTION2];
	char *m_pBuf;
	CString m_strGetFileName;
	BOOL m_bSingleVal;


public:
	CString GetStrings();
	CIni2(CString strIniFile);
	BOOL GetSectionData(CString str_section);
	BOOL GetString(CString& strBuf);
	BOOL GetKey(CString& strBuf);
	BOOL GetNxtString(CString& strBuf);
	CString GetString(CString strSection,CString strKey);
	int GetCount(CString strSection);
	BOOL FindSection(CString strSection,CString strKey);
	BOOL GetSectionName(CString &strSection,CString strKey);
	int KeyCount(CString strSection);
	BOOL FindString(CString strSection,CString strValue,int &nRow);
	BOOL FindKey(CString strSection,CString strValue,int &nRow);
	void SubstituteString(CString& strValue);
	char* ReturnBuffer(){return m_pBuf;};
	int GetSectionNumber(CString strSection,CString strKey,CString 
		                 strValue);


};
