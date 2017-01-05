/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// AddNTPrinter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddNTPrinter dialog
#ifndef _INCLUDE_AddNTPrinter_H_
#define _INCLUDE_AddNTPrinter_H_

#include "StringEx.h"
#include "setupapi.h"

class CAddNTPrinter : public CDialog
{
// Construction
public:
	CAddNTPrinter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddNTPrinter)
	enum { IDD = IDD_ADDNTPRINTER };
	CListBox	m_ManufacturesCtrl;
	CListBox	m_PrintersCtrl;
	CString	m_Manufactures;
	CString	m_Printers;
	CString	m_NTPrinterInf;
	//}}AFX_DATA

CString m_driver;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNTPrinter)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNTPrinter)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeList2();
	virtual void OnOK();
	afx_msg void OnNewINF();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CString	m_sysroot,m_path,m_monitorstring,m_strSysSpoolDir;
	BOOL CopyFiles(CStringEx strCopyFiles);
	BOOL AreFilesLocal();
	BOOL AreCopyFilesOnServer();
	BOOL AreFilesWithINF();
	BOOL AddCopyFilesSection(HINF hInf,const CString& section);
	BOOL SendServerMsg();
	BOOL GetPrinterFiles();
	BOOL GetFilesFromDisk();
	void GetSection(LPCTSTR section, CListBox* pLB, BOOL bGetKey=FALSE);
	static UINT CALLBACK CabinetCallback(LPVOID pCtx,UINT uNotify,
										 UINT uParam1,UINT uParam2);
	BOOL IterateCabinet();
	BOOL m_bWIN2K;
	HINF OpenInf(const CString& strInf);
	CString GetValue(HINF hInf, const CString& section, const CString& key);
	CMapStringToString* list;
	CString driverfile,configfile,datafile,helpfile,monitorfile,monitorstring;
	CStringEx strCopyFiles;
	CString m_strTempPath;
};

#endif
