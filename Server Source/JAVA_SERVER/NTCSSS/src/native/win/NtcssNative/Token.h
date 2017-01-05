/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Token.h: interface for the CToken class.
//
//
//	MFC Simple CStdString Tokenizer (Version 1)
//	
//	Written by Richard Case (case@dcs.kcl.ac.uk)
//  Copyright (c) 1999
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOKEN_H__7CEAEBEE_D15D_11D2_9CA9_444553540000__INCLUDED_)
#define AFX_TOKEN_H__7CEAEBEE_D15D_11D2_9CA9_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdString.h"

class CToken  
{
public:
	virtual CStdString GetNextToken(); // Returns the next token
	virtual BOOL MoreTokens();				// Do we have any more tokens to retrieve
	void SetToken(CStdString tok) { m_strToken = tok; };
	CToken(CStdString str);
	virtual ~CToken();

protected:
	CStdString m_strToToken;	// The  original string to tokenize
	CStdString m_strLeft;		// The string we have left after taking tokens out
	CStdString m_strToken;		// What is the value to tokenize on?
	BOOL	m_bFoundAny;	// Have we found any tokens
	BOOL	m_bFinalToken;	// There might be one last token?
};

#endif // !defined(AFX_TOKEN_H__7CEAEBEE_D15D_11D2_9CA9_444553540000__INCLUDED_)
