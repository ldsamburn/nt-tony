/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtCFileDlg.h          
//                                         
// This is the header file for the CPrtClientFileDlg class.
// 
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_PRTFILE_H_CLASS_
#define _INCLUDE_PRTFILE_H_CLASS_

#include "session.h"
#include "PrtSelectDlg.h"

const CString PRINT_CLIENT_FILE_FACILITY_STR = "Print Client File";


class CPrtClientFileDlg : public CDialog
{
// Constructors
public:
    CPrtClientFileDlg(CWnd* pParent, Csession *session, const char *file_name,
             const SecurityLevel security, const char *output_type); 


// Attributes

// Operations

// Overrides

// Implementation - desctructor

public:
     ~CPrtClientFileDlg()
       { if (m_dlgSelPrinter) delete m_dlgSelPrinter; };

// Implementation - protected member variables 

protected:
    CSelPrtDialog *m_dlgSelPrinter;
	Csession *m_pSession;
    BOOL m_bPrinterPicked;
    BOOL m_bIsLocalPrinter;
    SecurityLevel m_eSeclvl;

	int m_test;

// Dialog Data
    //{{AFX_DATA(CPrtClientFileDlg)
    enum { IDD = IDD_PRINT_FILE };
    CString m_strFilename;
    CString m_strPrintername;
    CString m_strHostname;
    //}}AFX_DATA

// Implementation - message mapping / DDX / internal functions

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    

    // Generated message map functions
    //{{AFX_MSG(CPrtClientFileDlg)
    afx_msg void OnBrowseFile();
    afx_msg void OnBrowsePrinter();
    virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusFilename();
	virtual void OnOK();
	//}}AFX_MSG

  
    DECLARE_MESSAGE_MAP()

    void PrintClientFile();
};

#endif // _INCLUDE_PRTFILE_H_CLASS_
