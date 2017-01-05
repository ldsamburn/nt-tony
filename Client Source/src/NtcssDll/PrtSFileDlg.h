/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtSFileDlg.h                    
//                                         
// This is the header file for the CPrtSvrFile class.
// 
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#pragma once

#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_PRTSVRFI_H_CLASS_
#define _INCLUDE_PRTSVRFI_H_CLASS_

#include "session.h"
#include "PrtSelectDlg.h"
#include "MyListCtrl.h"


const CString PRINT_SERVER_FILE_FACILITY_STR = "Print Server File";


class CPrtSvrFile : public CDialog
{
// Constructors
public:
    CPrtSvrFile(CWnd* pParent, Csession *session,
                const char *server_name, const char *directory,   
                const char *output_type,const char *copies,const char *options);
                
	LV_COLUMN listColumn;
	LV_ITEM listItem;
// Attributes

// Operations

// Overrides

// Implementation - desrtructor 
    ~CPrtSvrFile()
        { if (m_dlgSelPrinter) delete m_dlgSelPrinter; };

// Implementation - protected member variables
protected:
    CSelPrtDialog *m_dlgSelPrinter;

    CString m_strServerName,m_strOptions,m_strCopies;
    CString m_StrOutputType;

    Csession *m_pSession;

    BOOL m_bPrinterIsPicked;

	SUnixDirInfo *m_file_list_ptr;  //TR10166 02/12
	int m_num_files_got;    //TR10166

    // Dialog Data
    //{{AFX_DATA(CPrtSvrFile)
	enum { IDD = IDD_PSFDIALOG };
	MyListCtrl	m_ListCtrl;
	CComboBox	m_FileTypeCtrl;
    CString m_strDirectory;
    CString m_strFileName;
    CString m_strSelectedFile;
    CString m_strPrinter;
    CString m_strFileType;
    CString m_strSendPrinter;
	//}}AFX_DATA


// Implementation - protected member functions, message mapping, DDX
protected:

    BOOL getUnixFiles(MyListCtrl* list_box_ptr);

    BOOL spoolUnixPrintJob();
    
    virtual void    DoDataExchange(CDataExchange* pDX); // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CPrtSvrFile)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnSelect();
    afx_msg void OnDestroy();
	afx_msg void OnSelchangeFilecombo();
	afx_msg void OnDblclkListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // _INCLUDE_PRTSVRFI_H_CLASS_
