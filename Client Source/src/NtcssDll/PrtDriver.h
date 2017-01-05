/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtDriver.h          
//
// This is the header file for CprintDriver class which handles the
// details of sending 'cooked' and 'raw' print jobs to both local and
// remote printers.  See code comments for details on processing.
//
//----------------------------------------------------------------------

                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_PRNTDRVR_H_CLASS_
#define _INCLUDE_PRNTDRVR_H_CLASS_

const int LPR_NUM_SECURITY_LEVELS    = 5;
const int LPR_MAX_SECLVL_NAME_LEN    = 15;

const char LPR_SECLVL_NAMES[LPR_NUM_SECURITY_LEVELS]
                           [LPR_MAX_SECLVL_NAME_LEN]
           = {"UNCLASS", "SENSITIVE", "CONFID",  "SECRET",
              "TOP SECRET"};


class CprintDriver : public CWnd
{
// Constructors
public:
    CprintDriver(Csession *session);


// Attributes


// Operations
public:
    BOOL sendCookedLocalLPRjob(const SlocalPrinterInfo *local_printer,
                               const char *file_name);

    BOOL sendCookedRemoteLPRjob(const SremotePrinterInfo *remote_printer,
                                const char *file_name,
                                SecurityLevel security);

    BOOL sendRawLocalLPRjob(const SlocalPrinterInfo *local_printer,
                            const char *file_name);

    BOOL sendRawRemoteLPRjob(const SremotePrinterInfo *remote_printer,
                             const char *file_name,
                             SecurityLevel security);

// Overrides


// Implementation - protected data
protected:
    Csession *m_pSession;
	CNtcssSock* m_pSock;

    SecurityLevel m_eSecLvl;

// Implementation - message map functions
protected:

    // Generated message map functions
    //{{AFX_MSG(clprdrv)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Implementation - public destructor and protected internal functions
public:
    ~CprintDriver() {};

protected:

    BOOL cookFileLocally(const SlocalPrinterInfo *local_printer_ptr,
                         const SremotePrinterInfo *remote_printer_ptr,
                         const char *file_name,
                         BOOL spool_to_file,
                         const char *spool_file_name);

    BOOL localLprSendLeadControls
                        (const char *printer_name, 
                         const char *host_name, 
                         long file_len, int seq_num); 

    BOOL localLprSendTrailControls
                  (const char *host_name, const char *file_name, 
                   long file_len, int seq_num, SecurityLevel seclvl,int nCopies,
							  BOOL bBanner); 

    BOOL localLprSendDataFile(const char *file_name, long file_len);


    BOOL localLprSendCommmandLines(const char *cmd_buf_ptr, 
                                   const int num_cmd_lines);



};

#endif // _INCLUDE_PRNTDRVR_H_CLASS_
