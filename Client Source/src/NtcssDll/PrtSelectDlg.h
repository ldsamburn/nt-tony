/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtSelectDlg.h          
//
// This is the header file for the CSelPrtDialog class.
//
//----------------------------------------------------------------------
// ...\comn_src\ntcssdll\selprndi.h                    
//                                         
// 32-bit and 16-bit common header file
//

#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_SELPRNDI_H_CLASS_
#define _INCLUDE_SELPRNDI_H_CLASS_

#include <msgdefs.h>
//#include <resource.h>

const CString SELECT_PRINTER_FACILITY_STR = "Select Printer";

class CSelPrtDialog : public CDialog
{
// Constructors
public:
    CSelPrtDialog(CWnd* pParent, Csession *session, 
                  BOOL enable_setup = TRUE,int nCopies=1,BOOL bBanner=FALSE);

// Attributes

// Operations

public:
//    void disableSetup()
      //  { m_bOkToSetup = FALSE; };
        
    void enableLocalSelection()
        {m_bLocalEnabled = TRUE; };


    void enableRemoteSelection(const char *output_type_str,
                               const SecurityLevel seclvl,
                               const BOOL default_only,
                               const BOOL general_access);

    const SremotePrinterInfo  *getDefaultRemotePrinter();

    const SlocalPrinterInfo   *getLocalPrinterSelection();

    const SremotePrinterInfo  *getRemotePrinterSelection();

    BOOL selectedLocalPrinter()
        { return(m_bLocalSelected); };

    BOOL selectedRemotePrinter()
        { return(m_bRemoteSelected); };

    void setDefaultPrinter(const char *printer_name)
        { m_default_prn = printer_name; };

    HDC selectedPrinterDeviceContext();

// Overrides

// Implementation - destructor
public:
    ~CSelPrtDialog();
	

// Implementation - protected data
protected:
    //{{AFX_DATA
    enum { IDD = IDD_PSETUP };
    CString m_local_printers;
    CString m_remote_printers;
    CString m_default_prn;
    BOOL    m_banner;
	int		m_dCopies;
    //}}AFX_DATA

    Csession      *m_pSession;

    char          m_szOutputType[SIZE_PRT_OUTPUT_TYPE+1];
    SecurityLevel m_eSecLvl;
    BOOL          m_bGeneralAccess;
    BOOL          m_bDefaultOnly;

    BOOL          m_bLocalEnabled;
    BOOL          m_bRemoteEnabled;

    BOOL          m_bLocalSelected;
    BOOL          m_bRemoteSelected;

    //BOOL          m_bOkToSetup;

    int           m_nSelectionIdx;

    SlocalPrinterInfo    *m_pLocalPrinterInfoList;
    SremotePrinterInfo   *m_pRemotePrinterInfoList;

    int m_nLocalPrinterCnt;
    int m_nRemotePrinterCnt;

// Implementation - protected data
protected:
    void checkForRemoteRedirect(SremotePrinterInfo 
                                *remote_printer_info_ptr);

    BOOL  getLocalPrinters(CListBox* list_box_ptr);

    BOOL  getRemotePrinters(CListBox* list_box_ptr);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    

    // Generated message map functions
    //{{AFX_MSG(CSelPrtDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnClickedOk();
    afx_msg void OnDblclkLocalPrinters();
    afx_msg void OnDblclkRemotePrinters();
    afx_msg void OnSetfocusLocalPrinters();
    afx_msg void OnSetfocusRemotePrinters();
    afx_msg void OnOptions();
	afx_msg void OnNetwork();
	afx_msg void OnSelchangedLocalPrinters();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};                      

#endif  // _INCLUDE_SELPRNDI_H_CLASS_
