
/****************************************************************************
  
               Copyright (c)2002 Northrop Grumman Corporation
 
                           All Rights Reserved
 
 
     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/ 


//////////////////////////////////////////////////////////////////////////////////////////////
//
// FileName:       CString.h
// Version:        2.0
// Description:    CString Class from MFC ported to UNIX
//
// File History:
//
//	9/26/96		RRR		CString ported and tested.  Vesion 2.0 includes all documented 
//						functions from MSVC v.1.52c.
//
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CSTRING_H
#define _CSTRING_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <iostream.h>


/////////////////////////////////////////////////////////////////////////////
// Basic types (from Windows)

typedef int            BOOL;   // BOOLean (0 or !=0)

// Standard constants
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0


/////////////////////////////////////////////////////////////////////////////
// Strings

class CString
{
// Construction/Destruction--Public Members

public:
	CString();
	CString(const CString& stringSrc);
	CString(char ch, int nRepeat = 1);
	CString(const char* psz);
	CString(const char* pch, int nLength);
	~CString();

// The String as an Array -- Public Members

public:
	int GetLength() const
		{ return m_nDataLength; }	
	BOOL IsEmpty() const
		{ return m_nDataLength == 0; }
	void Empty();                       // free up the data
	char GetAt(int nIndex) const
		{
		assert(nIndex >= 0);
		assert(nIndex < m_nDataLength);

		return m_pchData[nIndex];
		}
 	char operator[](int nIndex) const
		{
		// same as GetAt

		assert(nIndex >= 0);
		assert(nIndex < m_nDataLength);

		return m_pchData[nIndex];
		}
	void SetAt(int nIndex, char ch)
		{
		assert(nIndex >= 0);
		assert(nIndex < m_nDataLength);
		assert(ch != 0);

		m_pchData[nIndex] = ch;
		}
	operator const char*() const
		{ return (const char*)m_pchData; }

//  Assignment/Concatenation

public:
	// overloaded assignment
	const CString& operator=(const CString& stringSrc);
	const CString& operator=(char ch);
	const CString& operator=(const char* psz);

	// string concatenation
	friend CString operator+(const CString& string1, const CString& string2);
	friend CString operator+(const CString& string, const char* psz);
	friend CString operator+(const char* psz, const CString& string);
	friend CString operator+(const CString& string1, char ch);
	friend CString operator+(char ch, const CString& string);

	const CString& operator+=(const CString& string);
	const CString& operator+=(char ch);
	const CString& operator+=(const char* psz);

//  Comparison  --  Public Members	

	friend BOOL operator==(const CString& s1, const CString& s2);
	friend BOOL operator==(const CString& s1, const char* s2);
	friend BOOL operator==(const char* s1, const CString& s2);
	friend BOOL operator!=(const CString& s1, const CString& s2);
	friend BOOL operator!=(const CString& s1, const char* s2);
	friend BOOL operator!=(const char* s1, const CString& s2);
	friend BOOL operator<(const CString& s1, const CString& s2);
	friend BOOL operator<(const CString& s1, const char* s2);
	friend BOOL operator<(const char* s1, const CString& s2);
	friend BOOL operator>(const CString& s1, const CString& s2);
	friend BOOL operator>(const CString& s1, const char* s2);
	friend BOOL operator>(const char* s1, const CString& s2);
	friend BOOL operator<=(const CString& s1, const CString& s2);
	friend BOOL operator<=(const CString& s1, const char* s2);
	friend BOOL operator<=(const char* s1, const CString& s2);
	friend BOOL operator>=(const CString& s1, const CString& s2);
	friend BOOL operator>=(const CString& s1, const char* s2);
	friend BOOL operator>=(const char* s1, const CString& s2);

	int Compare(const char* psz) const
		{ return strcmp(m_pchData, psz); }
	int CompareNoCase(const char* psz) const;   // ignore case
	int Collate(const char* psz) const
		{ return strcoll(m_pchData, psz); }

// Extraction -- Public Members

	CString Mid(int nFirst, int nCount) const;
	CString Mid(int nFirst) const;
	CString Left(int nCount) const;
	CString Right(int nCount) const;

	CString SpanIncluding(const char* pszCharSet) const;
	CString SpanExcluding(const char* pszCharSet) const;

// Other Conversions -- Public memebers 
	void MakeUpper();
	void MakeLower();
	void MakeReverse();

// Searching -- Public members

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(char ch) const;                    // like "C" strchr
	int ReverseFind(char ch) const;
	int FindOneOf(const char* pszCharSet) const;

	// look for a specific sub-string
	int Find(const char* pszSub) const;         // like "C" strstr

// Buffer Access -- Public memebers

	// Access to string implementation buffer as "C" character array
	char* GetBuffer(int nMinBufLength);
	void ReleaseBuffer(int nNewLength = -1);
	char* GetBufferSetLength(int nNewLength);

// Implementation

protected:
	// lengths/sizes in characters
	//  (note: an extra character is always allocated)
	char* m_pchData;            // actual string (zero terminated)
	int m_nDataLength;          // does not include terminating 0
	int m_nAllocLength;         // does not include terminating 0

	// implementation helpers
	void Init();
	void AllocCopy(CString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, const char* pszSrcData);
	void ConcatCopy(int nSrc1Len, const char* pszSrc1Data, int nSrc2Len, const char* pszSrc2Data);
	void ConcatInPlace(int nSrcLen, const char* pszSrcData);
	static void SafeDelete(char* pch);
public:
	static int SafeStrlen(const char* psz)
		{ return (strlen(psz)); }

	int GetAllocLength() const
		{ return m_nAllocLength; }

	friend ostream& operator<< (ostream& os, const CString& s);


};


#endif /* _CSTRING_H */
