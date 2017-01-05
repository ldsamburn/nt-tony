/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/



#include "stdafx.h"
#include "ini2.h"
#include "StringEx.h"


CIni2::CIni2(CString strIniFile)
{
	m_pBuf=NULL;
	m_cBuf[0]=NULL;
	m_strGetFileName=strIniFile;
	m_bSingleVal=FALSE;
}


BOOL CIni2::GetSectionData(CString strSection)
{

	if(::GetPrivateProfileSection(strSection, m_cBuf,
	                sizeof(m_cBuf),m_strGetFileName))
	{
		m_pBuf=m_cBuf;
		if(!strchr(m_cBuf,'='))
			m_bSingleVal=TRUE;
		else
			m_bSingleVal=FALSE;
		return TRUE;
	}
	else
	{
		DWORD ret=::GetLastError();
		m_pBuf=NULL;
		return FALSE;
	}

}


BOOL CIni2::GetString(CString& strBuf)
//this one will deal with getting strings from the buffer 
//initialized by a call to GetSectionData stuff on the 
//right side of the equal sign  If the section just has values
//it uses m_bSingleVal to just return them

{
	strBuf=_T("");
    if(m_pBuf==NULL)
		return FALSE;
	if(!m_bSingleVal)
		m_pBuf=strchr(m_pBuf,'=')+1;
	strBuf=m_pBuf;
	SubstituteString(strBuf);
	m_pBuf=strchr(m_pBuf,'\0')+1;
	if(*m_pBuf==NULL)
		m_pBuf=NULL;
	return TRUE;
}

CString CIni2::GetStrings()
//this one will deal with getting strings from the buffer 
//initialized by a call to GetSectionData stuff on the 
//right side of the equal sign  If the section just has values
//it uses m_bSingleVal to just return them

{
	CString strBuf=_T("");
    if(m_pBuf==NULL)
		return _T("");
	if(!m_bSingleVal)
		m_pBuf=strchr(m_pBuf,'=')+1;
	strBuf=m_pBuf;
	SubstituteString(strBuf);
	m_pBuf=strchr(m_pBuf,'\0')+1;
	if(*m_pBuf==NULL)
		m_pBuf=NULL;
	return strBuf;
}


BOOL CIni2::GetKey(CString& strBuf)
//this one will deal with getting strings from the buffer 
//initialized by a call to GetSectionData stuff on the 
//left side of the equal sign. If the section just has values
//it uses m_bSingleVal to just return them
{
	strBuf=_T("");
    if(m_pBuf==NULL)
		return FALSE;
	strBuf=m_pBuf;
	if(!m_bSingleVal)
		strBuf=strBuf.Left(strBuf.Find('='));
	SubstituteString(strBuf);
	m_pBuf=strchr(m_pBuf,'\0')+1;
	if(*m_pBuf==NULL)
		m_pBuf=NULL;
	return TRUE;
}



BOOL CIni2::GetNxtString(CString& strBuf)
//gets string on both sides of = in sequential
//calls  If the section just has values
//it uses m_bSingleVal to just return them


{
	strBuf=_T("");
	if(m_pBuf==NULL)
		return FALSE;
	strBuf=m_pBuf;
	if(!m_bSingleVal&&strBuf.Find('=')!=-1)
	{
		strBuf=strBuf.Left(strBuf.Find('='));
		m_pBuf=strchr(m_pBuf,'=')+1;
	}
	else
	{
		strBuf=m_pBuf;
		m_pBuf=strchr(m_pBuf,'\0')+1;
	}
	SubstituteString(strBuf);
	if(*m_pBuf==NULL)
		m_pBuf=NULL;
	return TRUE;
}


CString CIni2::GetString(CString strSection,CString strKey)
//this one deals with getting a string directly with GPPS
{
	CString strBuf;
	::GetPrivateProfileString(strSection,strKey,"!",strBuf.
				GetBuffer(MAX_INI_LINE),MAX_INI_LINE,m_strGetFileName);
	strBuf.ReleaseBuffer();
	SubstituteString(strBuf);
	return strBuf!="!" ? strBuf : _T("");

}

int CIni2::GetCount(CString strSection)
//returns int representation of [strSection] COUNT=#
{
	return atoi(GetString(strSection,"COUNT"));
}


BOOL CIni2::FindSection(CString strSection,CString strKey)
//this one takes the prefix of a section ADM_PDJ_1 & increments it searching for the
//search string that way it sets the buf to pt to right section if it works
{
	CString temp,savedSection=strSection;;
	int i=1;
	while(1)
	{
		strSection.Format("%s%d",savedSection,i++);
		if(!GetSectionData(strSection))
			return FALSE;
		if(!GetString(temp))
			return FALSE;
		if(temp==strKey)
			return GetSectionData(strSection);  //reset this
		
	}
}


BOOL CIni2::GetSectionName(CString &strSection,CString strKey)
//Used when over writing sections tries to find section, if it does, returns true &
//strSection should be set to correct section - if it doesn't strSection should be
//incremented to the correct next section name
//strSection COMES in like-> ADM_PROG_
{
	CString temp,savedSection=strSection;
	int i=1;
	while(1)
	{
		strSection.Format("%s%d",savedSection,i++);
		if(!GetSectionData(strSection))
			return FALSE;
		if(!GetString(temp))
			return FALSE;
		if(temp==strKey)
			return TRUE;  
	}

}


int CIni2::KeyCount(CString strSection)
//Counts the number of entries in section
{
	if(!GetSectionData(strSection))
		return 0;
	int cnt=0;
	CString temp;
	while(GetNxtString(temp))
		cnt++;
	return cnt?cnt/2:cnt;
}

BOOL CIni2::FindString(CString strSection,CString strValue,int &nRow)
// Finds if a value on the rt side of =
// in a section like
// ADM_ROLE_1=ADM_ROLE_1
// ADM_ROLE_1=TEST exists
{
    nRow=0;
	if(!GetSectionData(strSection))
		return nRow;  //no section
	CString temp;
	//int cnt=0;
	while(GetString(temp))
	{
		++nRow;
		if(temp==strValue)
			return TRUE;
	}
	++nRow;
	if(nRow==0)nRow=1;
	return FALSE;
}

BOOL CIni2::FindKey(CString strSection,CString strValue,int &nRow)
// Finds if a value on the left side of =
// in a section like
// ADM_ROLE_1=ADM_ROLE_1
// ADM_ROLE_1=TEST exists
{
    nRow=0;
	if(!GetSectionData(strSection))
		return nRow;  //no section
	CString temp;
	//int cnt=0;
	while(GetKey(temp))
	{
		++nRow;
		if(temp==strValue)
			return TRUE;
	}
	++nRow;
	if(nRow==0)nRow=1;
	return FALSE;
}

int CIni2::GetSectionNumber(CString strSection,CString strKey,CString strValue) 
//walks sections formated like ADM_PDJ_ looking for key & value
//if last char isn't '_' it will return 0 or 1 if whole section is there

{
	CIni2 ini(m_strGetFileName);
	//this is only used like this on dup chk for apps
	if(strSection.ReverseFind('_')!=strSection.GetLength()-1)
		return ini.GetSectionData(strSection);


	CString savedSection=strSection;
	
	int i=1;
	strSection.Format("%s%d",savedSection,i);
	while(ini.GetSectionData(strSection))
	{
	    if(ini.GetString(strSection,strKey)==strValue)
			return i;
		strSection.Format("%s%d",savedSection,++i);
	
	}
	return 0;
}

void CIni2::SubstituteString(CString& strValue)
//needed to automatic string substitution in ini files that use it
{
	if(strValue.IsEmpty())
		return;
	if(!((strValue.GetAt(0)=='%')&&(strValue.GetAt(strValue.GetLength()-1)=='%')))
		return;
	CStringEx temp=strValue;
	temp.FindReplace("%",""); //assuming there won't be 1 anywhere else
	strValue=GetString("Strings",temp);
	return;
}

