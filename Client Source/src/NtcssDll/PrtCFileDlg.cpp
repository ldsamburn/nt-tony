/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtCFileDlg.cpp
//                                         
// This is the implementation file for CPrtClientFileDlg.
//
//----------------------------------------------------------------------


#include "stdafx.h"

#include "0_basics.h"
#include "PrtDriver.h"

#include "iostream.h"
#include "fstream.h" 

#include "PrtCFileDlg.h"
#include "PrtSelectDlg.h"
#include "genutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// NEEDTODO - 
// - Need to get enable raw printing to local printers
// - Need to get enable cooked printing to remote printers

/////////////////////////////////////////////////////////////////////////////
// CPrtClientFileDlg constructor
// ____________________

CPrtClientFileDlg::CPrtClientFileDlg(CWnd* pParent, Csession *session, 
                   const char *file_name,
                   const SecurityLevel seclvl, 
                   const char *output_type)
    : CDialog(CPrtClientFileDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CPrtClientFileDlg)
    m_strFilename = "";
    m_strPrintername = "";
    m_strHostname = "";
    //}}AFX_DATA_INIT                  // Inits

    m_pSession = session;
    m_eSeclvl = seclvl;
                                       // Set up printer selection
                                       //  dialog now
                                       //


    m_dlgSelPrinter = new CSelPrtDialog(GetParent(),m_pSession);

    m_dlgSelPrinter->enableLocalSelection();

    m_dlgSelPrinter->enableRemoteSelection
                              (output_type,
                               seclvl,
                               FALSE,  // all printers, not just default
                               FALSE); // but not general access printers
    m_bIsLocalPrinter = FALSE;

	m_bPrinterPicked = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// DoDataExchange and message mapping
// ______________

void CPrtClientFileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPrtClientFileDlg)
    DDX_Text(pDX, IDC_FILENAME, m_strFilename);
    DDX_Text(pDX, IDC_PRINTER_NAME, m_strPrintername);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrtClientFileDlg, CDialog)
    //{{AFX_MSG_MAP(CPrtClientFileDlg)
    ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
    ON_BN_CLICKED(IDC_BROWSE_PRINTER, OnBrowsePrinter)
	ON_EN_KILLFOCUS(IDC_FILENAME, OnKillfocusFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// OnBrowseFile
// ____________

void CPrtClientFileDlg::OnBrowseFile()
{
    OPENFILENAME    ofn;
    char            dir_name[SIZE_CLIENT_LOCATION];
    char            file_name[SIZE_CLIENT_LOCATION];
    char            file_title[SIZE_CLIENT_LOCATION];
    UINT            rc;
    char            replace_ch;
    char            filter_str[SIZE_STANDARD_GENBUF];

                                       // Get the system directory name
                                       //
    GetSystemDirectory(dir_name, sizeof(dir_name));
    file_name[0] = '\0';
                                       // Get filter
                                       //
    rc = LoadString(m_pSession->ssnhInstance(), 
                    IDS_FILTERSTRING, 
                    filter_str, 
                    sizeof(filter_str));

    if (rc == 0) 
    {
        sprintf(filter_str, "Text Files(*.TXT)|*.txt|");
        rc = strlen(filter_str);;
    }
                                       // retrieve wildcard
    replace_ch = filter_str[rc - 1];  
    for (int i = 0; filter_str[i] != '\0'; i++) 
    {
        if (filter_str[i] == replace_ch)
            filter_str[i] = '\0';
    }
                                        // Set all OPENFILENAME structure 
                                        //   members to zero
    memset(&ofn, 0, sizeof(OPENFILENAME));
    
    ofn.lStructSize = sizeof(OPENFILENAME);

								       // WSP - Not sure why commented?
    //ofn.hwndOwner = session->m_hWnd;    
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = filter_str;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = sizeof(file_name);
    ofn.lpstrFileTitle = file_title;
    ofn.nMaxFileTitle = sizeof(file_title);
    ofn.lpstrInitialDir = dir_name;
    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) 
    {
								       // WSP - Not sure why commented?
//        hf = _lopen(ofn.lpstrFile, OF_READ);
        m_strFilename = (const char *) ofn.lpstrFile;
        UpdateData(FALSE);
    }
}  



/////////////////////////////////////////////////////////////////////////////
// OnBrowsePrinter
// _______________

void CPrtClientFileDlg::OnBrowsePrinter()
{
    if (m_dlgSelPrinter->DoModal() == IDOK)
    {
									   // If printer was selected then
									   //  need to uppdate this dialog's
									   //  displayed printer/host names
									   //
        if (m_dlgSelPrinter->selectedLocalPrinter())
        {
            const SlocalPrinterInfo* l_tmp_prt = 
                           m_dlgSelPrinter->getLocalPrinterSelection();

            m_strPrintername = l_tmp_prt->printer_device_name_buf;
            m_strHostname = "";
            m_bPrinterPicked = TRUE;
            m_bIsLocalPrinter = TRUE;
        }
        else if (m_dlgSelPrinter->selectedRemotePrinter())
        {
            const SremotePrinterInfo* r_tmp_prt = 
                           m_dlgSelPrinter->getRemotePrinterSelection();

            m_strPrintername = r_tmp_prt->printer_name_buf;
            m_strHostname = r_tmp_prt->host_name_buf;
            m_bPrinterPicked = TRUE;
            m_bIsLocalPrinter = FALSE;
        }
    }

    UpdateData(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
// ____________

BOOL CPrtClientFileDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    centerScreen(m_hWnd);

    BOOL do_default_prt;
    BOOL do_local_default_prt;


    if ( !m_pSession->isRemotePrinterInfoSet() &&
         !m_pSession->isLocalPrinterInfoSet() )
    {
									   // No default printer info 
									   //  available
        do_default_prt = FALSE;
    }
    else
    {
        do_default_prt = TRUE;

        if ( m_pSession->isRemotePrinterInfoSet() &&
             m_pSession->isLocalPrinterInfoSet() )
        {
									   // If local and remote enabled
									   //  then local will be the
									   //  default if it was most
									   //  recently used
									   //
            do_local_default_prt = m_pSession->isLocalPrinterMru();
        }
        else if (m_pSession->isRemotePrinterInfoSet())
        {
									   // Else if remote not local is
									   //  enabled, no local default
									   //  printing
									   //
            do_local_default_prt = FALSE;
        }
        else											 
        {
									   // Else local not remote is
									   //  enabled, local default
									   //  printing	is ok
									   //
            do_local_default_prt = TRUE;
        }
    }

    if (do_default_prt)
    {								  
									   // Setup info for default
									   // printer

        if (do_local_default_prt)
        {
            const SlocalPrinterInfo* tmp_prt = 
                                        m_pSession->localPrinterInfo();

            m_strPrintername = tmp_prt->printer_device_name_buf;
            m_strHostname = "";
            m_bIsLocalPrinter = TRUE;
        }
        else
        {
            const SremotePrinterInfo* tmp_prt = 
                                        m_pSession->remotePrinterInfo();

//CString msg;
//msg.Format ("Got here in setup, printer is %s, addr is: %d",
//tmp_prt->host_name_buf,tmp_prt);
//AfxMessageBox(msg);
            m_strPrintername = tmp_prt->printer_name_buf;
            m_strHostname = tmp_prt->host_name_buf;
            m_bIsLocalPrinter = FALSE;
        }

        UpdateData(FALSE);
    }

    return TRUE; 
}



/////////////////////////////////////////////////////////////////////////////
// OnOK
// ____

void CPrtClientFileDlg::OnOK()
{
    UpdateData(TRUE);

	AfxMessageBox ("In OK");
    
    if (m_strFilename.IsEmpty())
    {
        AfxMessageBox ("Enter file name",MB_OK);
        return;
    }
    if (m_strPrintername.IsEmpty())
    {
        AfxMessageBox ("Select printer",MB_OK);
        return;
    }

    m_pSession->ssnErrorMan()->ClearError();

    ::SetCapture(m_pSession->ssnHWnd());
    HCURSOR hcurSave = ::SetCursor(LoadCursor(NULL,IDC_WAIT));

								       // Set up CprintDriver to handle
									   //  the requested printing
									   //
    CprintDriver print_driver_man(m_pSession);

    BOOL ok = TRUE;

    if (m_bIsLocalPrinter)
    {
		const char *file_name = (LPCTSTR)m_strFilename;
        const SlocalPrinterInfo* l_tmp_prt;
        l_tmp_prt = m_dlgSelPrinter->getLocalPrinterSelection();
        if(!l_tmp_prt) l_tmp_prt=m_pSession->localPrinterInfo(); //jgj bug null on 2nd call
		                                                          //get from session in that case
// NEEDTODO - here just handling cooked, need to fixup to handle raw
        
        ok = print_driver_man.sendCookedLocalLPRjob
                                                (l_tmp_prt, file_name);

	}
    else
    {


	//	AfxMessageBox ("Doing remote stuff"); //debug
		  
        const char *file_name = (LPCTSTR)m_strFilename;

        const SremotePrinterInfo* r_tmp_prt;


        if (m_bPrinterPicked)
		{
            r_tmp_prt = m_dlgSelPrinter->getRemotePrinterSelection();
		}
        else
		{
            r_tmp_prt = m_pSession->remotePrinterInfo();                                        
		}


		// NEEDTODO - here just handling raw, need to fixup to handle cooked

	//	AfxMessageBox ("Doing print driver stuff"); //debug

        ok = print_driver_man.sendRawRemoteLPRjob(r_tmp_prt,
                                                  file_name,
                                                  m_eSeclvl);
    }

    SetCursor(hcurSave);
    ::ReleaseCapture();
 

    if (ok)
    {
//		AfxMessageBox ("Doing print driver stuff"); //debug

        if (m_bIsLocalPrinter)
            AfxMessageBox ("File successfully spooled to local printer",MB_OK);
        else
            AfxMessageBox ("File successfully spooled to remote printer",MB_OK);
    }
    else
    {
        m_pSession->ssnErrorMan()->HandleError
                            (PRINT_CLIENT_FILE_FACILITY_STR,
                             /*NULL*/"", ""); //5.0
    }

    m_pSession->ssnErrorMan()->ClearError();
    ok = TRUE;

    if (m_bPrinterPicked)
    {
									   // If made a printer selection
									   //  then have then update the
									   //  session with the printer info
        if (m_bIsLocalPrinter)
        {
            const SlocalPrinterInfo* l_tmp_prt;
            l_tmp_prt = m_dlgSelPrinter->getLocalPrinterSelection();

            if ((l_tmp_prt == NULL) ||
                (!m_pSession->setLocalPrinterInfo(l_tmp_prt, TRUE)))
            {
                ok = FALSE;
                m_pSession->ssnErrorMan()->RecordError
                      ("Error, unable to save remote printer selection");
            }
        }
        else
        {
            const SremotePrinterInfo* r_tmp_prt;
            r_tmp_prt = m_dlgSelPrinter->getRemotePrinterSelection();

            if ((r_tmp_prt == NULL) ||
                (!m_pSession->setRemotePrinterInfo(r_tmp_prt, TRUE)))
            {
                ok = FALSE;
                m_pSession->ssnErrorMan()->RecordError
                      ("Error, unable to save remote printer selection");
            }
        }
    }

    if (!ok)
        m_pSession->ssnErrorMan()->HandleError
                            (PRINT_CLIENT_FILE_FACILITY_STR,
                             /*NULL*/"", "Warning"); //5.0

	

    CDialog::OnOK();
}




void CPrtClientFileDlg::OnKillfocusFilename() 
{
//AfxMessageBox("In here - OnKillfocusFilename");
UpdateData(TRUE);
}

