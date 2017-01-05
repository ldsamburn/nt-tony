/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\etc.h                    
//
// WSP, last update: 9/16/96
//                                        
// This file taken from "Visual C++ How To" CD, by Standfield, et.al.
// See original comments below, no changes made by me, WSP.
//
//----------------------------------------------------------------------
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

                                       // Make sure have afx.h
#ifndef __AFXWIN_H__
    #error include <stdafx.h> before including this file
#endif

/////////////////////////////////////////////////////////////////////////////
// Miscellaneous System/Screen metrics

class CEtc
{
public:
    CEtc();
    ~CEtc();
    void UpdateSysColors();

    // System metrics
    int     cxVScroll, cyHScroll;
    int     cxIcon, cyIcon;

    // Device metrics for screen
    int     cxPixelsPerInch, cyPixelsPerInch;
    int     cySysFont;

    // Solid brushes with convenient gray colors and system colors
    HBRUSH  hbrLtGray, hbrDkGray;
    HBRUSH  hbrBtnHilite, hbrBtnFace, hbrBtnShadow;

    // Color values of system colors used for CToolBar
    COLORREF    clrBtnFace, clrBtnShadow, clrBtnHilite;
    COLORREF    clrBtnText, clrWindowFrame;

    BOOL    bWin32s;            // TRUE if Win32s

// Implementation
protected:
    void SafeDelete(HGDIOBJ hObject);
};

extern CEtc g_etc;

