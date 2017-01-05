/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\etc.cpp
//
// WSP, last update: 9/16/96
//                                        
// See original comments below, no changes made by me, WSP.
//
//----------------------------------------------------------------------
// etc.cpp : header file
//

#include "stdafx.h"
#include "etc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// To use this class properly, trap the OnSysColorChange 
// in your main frame window and call g_etc.UpdateSysColors:
/*
void CFrameWnd::OnSysColorChange()
{
    g_etc.UpdateSysColors();
    CFrameWnd::OnSysColorChange();
}
*/

/////////////////////////////////////////////////////////////////////////////
// System metrics etc
/////////////////////////////////////////////////////////////////////////////

CEtc g_etc;     // global access to CEtc

// Initialization code
CEtc::CEtc()
{
    HDC hDCScreen = GetDC(NULL);
    ASSERT(hDCScreen != NULL);

    // System metrics
    cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
    cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
    cxIcon = GetSystemMetrics(SM_CXICON);
    cyIcon = GetSystemMetrics(SM_CYICON);

    // Device metrics for screen
    cxPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSX);
    cyPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSY);
    SIZE size;
    VERIFY(GetTextExtentPoint(hDCScreen, _T("M"), 1, &size));
    cySysFont = size.cy;
    ReleaseDC(NULL, hDCScreen);

    // Border attributes
    hbrLtGray = ::CreateSolidBrush(RGB(192, 192, 192));
    hbrDkGray = ::CreateSolidBrush(RGB(128, 128, 128));
    ASSERT(hbrLtGray != NULL);
    ASSERT(hbrDkGray != NULL);

    // Cached system values (updated in CFrameWnd::OnSysColorChange)
    hbrBtnFace = NULL;
    hbrBtnShadow = NULL;
    hbrBtnHilite = NULL;
    UpdateSysColors();

    // hi-bit of HIWORD is set if the platform is Win32s
    bWin32s = (HIWORD(::GetVersion()) & 0x8000) == 0x8000;
}

// Termination code
CEtc::~CEtc()
{
    // cleanup
    SafeDelete(hbrLtGray);
    SafeDelete(hbrDkGray);
    SafeDelete(hbrBtnFace);
    SafeDelete(hbrBtnShadow);
    SafeDelete(hbrBtnHilite);
}

void CEtc::UpdateSysColors()
{
    clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
    clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
    clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
    clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
    clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);

    if (hbrBtnFace != NULL)
        ::DeleteObject(hbrBtnFace);
    if (hbrBtnShadow != NULL)
        ::DeleteObject(hbrBtnShadow);
    if (hbrBtnHilite != NULL)
        ::DeleteObject(hbrBtnHilite);
    hbrBtnFace = ::CreateSolidBrush(clrBtnFace);
    hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
    hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
    ASSERT(hbrBtnFace != NULL);
    ASSERT(hbrBtnShadow != NULL);
    ASSERT(hbrBtnHilite != NULL);
}

void CEtc::SafeDelete(HGDIOBJ hObject)
{
    if (hObject != NULL)
        ::DeleteObject(hObject);
}

