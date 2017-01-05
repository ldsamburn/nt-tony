/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtSpoolMan.h                    
//                                         
// This is the header file for CdllSpoolPrintManager which handles the
// details of print spooling for local and remote printers.
// 
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_PRTSPMAN_H_CLASS_
#define _INCLUDE_PRTSPMAN_H_CLASS_
        

class CdllSpoolPrintManager // : public CObject
{
// Constructors
public:
    CdllSpoolPrintManager(Csession *session); 
	
          
// Attributes

// Operations
public:
    BOOL restorePrinter();

    BOOL selectPrinter (const char          *output_type_str,
                        const int            seclvl,
                        const BOOL           default_flag,
                        const BOOL           gen_access_flag,
                        const BOOL           return_DC_flag,
                        const NtcssPrintAttrib   *pr_attr_ptr,
                        const BOOL           local_enable_flag,
                        const BOOL           remote_enable_flag);

    BOOL spoolPrintRequest();

	BOOL AddNTCSSPrinter(const SremotePrinterInfo *remote_printer_ptr);

    HDC theHDC()
        { return(m_TheHDC); };

    BOOL userCancelled()
        { return(m_bCancelled); };

	char m_szSpoolFileName[SIZE_SMALL_GENBUF];
	BOOL m_bNetworkPrinter;
	BOOL m_bAtlassPrompt,m_bAtlassAppend;   //Atlass
	CString m_strUNC;
	CString m_strAtlassPrintFile; //Atlass



// Overrides

// Implementation - protected data
protected:
    BOOL m_bIsOkay;
    BOOL m_bCancelled;
	BOOL m_bSelectedPrinter;
    HDC  m_TheHDC;
    char m_szOrigProfile[SIZE_SMALL_GENBUF];
	CString m_strNtSpoolDir,m_strNTSysDir;
    
	SecurityLevel m_eSeclvl;

    Csession *m_pSession;

// Implementation - destructor and protected functions
protected:

    BOOL doHdcLocalPrintSpooling(const SlocalPrinterInfo  *);
    BOOL doHdcRemotePrintSpooling(const SremotePrinterInfo  *);
    BOOL doWinIniPrintSpooling(const SlocalPrinterInfo  *,
                               const SremotePrinterInfo  *);

    BOOL deleteFile(const char *file_name);

    BOOL setupHdcForLocalPrinter(const SlocalPrinterInfo  *,
                                 const NtcssPrintAttrib   *pr_attr_ptr,
                                 BOOL hdc_exists);
    BOOL setupHdcForRemotePrinter(const SremotePrinterInfo  *,
                                  const NtcssPrintAttrib   *pr_attr_ptr,
                                  BOOL hdc_exists);
    BOOL setupWinIniPrinting (const SlocalPrinterInfo  *,
                              const SremotePrinterInfo  *);


	BOOL AddTempPrintDriver(const SremotePrinterInfo *remote_printer_ptr); 

	BOOL GetDrivers(const SremotePrinterInfo *remote_printer_ptr);

	BOOL GetNTDriver(CString FileSpec,CString Driver);

	BOOL NtcssAddMonitor(CString strMonitorName,CString strMonitorDll);

	BOOL OnSetprinter(const NtcssPrintAttrib *ptr_attrib, BOOL local_printer);

	CString GetSysError(DWORD dError);

	BOOL AddTempPrinter(const SremotePrinterInfo *remote_printer_ptr/*,CString why*/);
	
};

#endif // _INCLUDE_PRTSPMAN_H_CLASS_





