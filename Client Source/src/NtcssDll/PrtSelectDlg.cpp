/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtSelectDlg.cpp
//                                         
// This is the implementation file for CSelPrtDialog.
//
//----------------------------------------------------------------------

// NEEDTODO - need to change disableSetup to really disable button,
//           currently m_bOkToSetup used to disallow manually

#include "stdafx.h"
#include "0_basics.h"
#include "dll_msgs.h"
#include "genutils.h"
#include "PrtSelectDlg.h"
#include "PrintUtil.h"
#include "StringEx.h"
#include "winspool.h"






/////////////////////////////////////////////////////////////////////////////
// CSelPrtDialog constructor
// _________________________

CSelPrtDialog::CSelPrtDialog(CWnd* pParent, Csession *session,
                             BOOL enable_setup,int nCopies,BOOL bBanner) 
							 

    : CDialog(CSelPrtDialog::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSelPrtDialog)
    m_local_printers    = "";
    m_remote_printers   = "";
    m_default_prn       = "";
    m_banner            = bBanner;
	m_dCopies = nCopies;
    //}}AFX_DATA_INIT

    m_pSession = session;

    m_pRemotePrinterInfoList = NULL;
    m_nRemotePrinterCnt = 0;

    m_pLocalPrinterInfoList  = NULL;
    m_nLocalPrinterCnt = 0;

    m_bRemoteEnabled = FALSE;
    m_bLocalEnabled =  FALSE;

    m_bLocalSelected  = FALSE;
    m_bRemoteSelected = FALSE;
	
    //m_bOkToSetup = TRUE; 4/29/99
}



/////////////////////////////////////////////////////////////////////////////
// CSelPrtDialog destructor
// ________________________

CSelPrtDialog::~CSelPrtDialog()
{
    if (m_pRemotePrinterInfoList != NULL)
    {
        delete [] m_pRemotePrinterInfoList;
    }

    if (m_pLocalPrinterInfoList != NULL)
    {
        delete [] m_pLocalPrinterInfoList;
    }
}


/////////////////////////////////////////////////////////////////////////////
// DoDataExchange and Message Map
// ______________________________

void CSelPrtDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSelPrtDialog)
    DDX_LBString(pDX, IDC_LOCAL_PRINTERS,  m_local_printers);
    DDX_LBString(pDX, IDC_REMOTE_PRINTERS, m_remote_printers);
    DDX_Text(    pDX, IDC_DEFAULT_PRINTER, m_default_prn);
    DDX_Check(   pDX, IDC_BANNER,          m_banner);
	DDX_Text(pDX, IDC_COPIES, m_dCopies);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelPrtDialog, CDialog)
    //{{AFX_MSG_MAP(CSelPrtDialog)
    ON_BN_CLICKED(IDOK, OnClickedOk)
    ON_LBN_DBLCLK(IDC_LOCAL_PRINTERS, OnDblclkLocalPrinters)
    ON_LBN_DBLCLK(IDC_REMOTE_PRINTERS, OnDblclkRemotePrinters)
    ON_LBN_SETFOCUS(IDC_LOCAL_PRINTERS, OnSetfocusLocalPrinters)
    ON_LBN_SETFOCUS(IDC_REMOTE_PRINTERS, OnSetfocusRemotePrinters)
    ON_BN_CLICKED(IDOPTIONS, OnOptions)
	ON_BN_CLICKED(IDNETWORK, OnNetwork)
	ON_LBN_SELCHANGE(IDC_LOCAL_PRINTERS, OnSelchangedLocalPrinters)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// checkForRemoteRedirect 
// ______________________

void CSelPrtDialog::checkForRemoteRedirect
                           (SremotePrinterInfo *remote_printer_info_ptr)
{
    if (strcmp(remote_printer_info_ptr->redirect_device_buf,
               NTCSSVAL_PRINTER_REDIRECT_CODE) == 0)
    {
        CString msg = "Printer is being redirected to ";
        msg += remote_printer_info_ptr->redirect_device_buf;
        msg += "@";
        msg += remote_printer_info_ptr->redirect_host_buf;
        AfxMessageBox(msg);
    }
}



/////////////////////////////////////////////////////////////////////////////
// enableRemoteSelection 
// _____________________

void CSelPrtDialog::enableRemoteSelection(const char *output_type_str,
                                          const SecurityLevel seclvl,
                                          const BOOL default_only,
                                          const BOOL general_access)
{
    m_eSecLvl = seclvl;
    m_bGeneralAccess = general_access;
    m_bDefaultOnly   = default_only;

    strcpy(m_szOutputType, output_type_str);

    m_bRemoteEnabled = TRUE;
}




/////////////////////////////////////////////////////////////////////////////
// getDefaultRemotePrinter
// _______________________
//
// This lets caller find out default remote printer - can work even
// without dialog being displayed for user input.
//
 
const SremotePrinterInfo  *CSelPrtDialog::getDefaultRemotePrinter()
{
	/*if (!m_bRemoteEnabled) AfxMessageBox("m_bRemoteEnabled is false");
	if (!getRemotePrinters(NULL)) AfxMessageBox("getRemotePrinters is false");*/


	if ((!m_bRemoteEnabled) || (!getRemotePrinters(NULL)))
	    return(NULL);

   
    SremotePrinterInfo *remote_printer_info_ptr;

	for (int idx=0; idx<m_nRemotePrinterCnt; idx++)
    {
                                       // Adjust pointer for the ith
                                       //   printer
        remote_printer_info_ptr = 
                        (m_pRemotePrinterInfoList + idx);

        if (remote_printer_info_ptr->default_printer_flag)
            break;
    }
    if (idx < m_nRemotePrinterCnt)
    {
        m_bRemoteSelected = TRUE;
        m_nSelectionIdx = idx;
		return(remote_printer_info_ptr);
    }
    else 
	  return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// getLocalPrinters
// ________________

BOOL CSelPrtDialog::getLocalPrinters(CListBox* list_box_ptr) 
{
    if (!m_bLocalEnabled)
        return(FALSE);

    const char *default_printer_name = (LPCTSTR)m_default_prn;
    BOOL  default_printer_idx = -1;

    char all_device_info_buf[/*( NTCSSVAL_AVG_INI_DEV_PROFILE_LEN
                               * NTCSSVAL_MAX_LOCAL_PRINTERS)*/
							    NTCSSVAL_MAX_INI_SECTION];

                                       // Retrieve all the entries in 
                                       //   the [devices] section.
                                       //
    ::GetProfileString(WININICODE_DEVICES_SECTION_CODE, 
                       NULL, "", all_device_info_buf, 
                       sizeof(all_device_info_buf));
	//TODO put error check for buffer over run on GPS

    char one_device_info_buf[( NTCSSVAL_AVG_INI_DEV_PROFILE_LEN * 2)];

	//new stuff - Network Printers
/*	CStringEx strNetPrintList=_T("");
	int nNetPrinters=0;
	BOOL bGotNetworkPrinters;*/
	CWaitCursor cursor;

/*	if(!doEnum(0,NULL,strNetPrintList,nNetPrinters))
	{
		bGotNetworkPrinters=FALSE;
		nNetPrinters=0;
	}*/

//	else
	//	bGotNetworkPrinters=nNetPrinters>0;
	//new stuff - Network Printers


                                       // First count number of printers
    int idx = 0;
    char *device_str_ptr;
    for ( idx = 0, device_str_ptr = all_device_info_buf;
          *device_str_ptr != '\0';
          idx++)
    {
        device_str_ptr += strlen(device_str_ptr) + 1; 
		                                              
    }
                                       // If have at least one printer
                                       //   then allocate space for 'em
    
		
    if (idx != 0)
    {
		m_nLocalPrinterCnt = idx+1; //ConnectToPrinterDlg addextra space 
									//will let them only add 1 network
		                            //printer per session for now
        
	

     /*   if (m_nLocalPrinterCnt > NTCSSVAL_MAX_LOCAL_PRINTERS)
            m_nLocalPrinterCnt = NTCSSVAL_MAX_LOCAL_PRINTERS;*/ //new took limit out

	


        m_pLocalPrinterInfoList = 
                               new SlocalPrinterInfo[m_nLocalPrinterCnt];

        if (m_pLocalPrinterInfoList == NULL)
        {
			m_pSession->ssnErrorMan()->RecordError("Memory allocation error");
			m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                             /*NULL*/"", "Warning"); //5.0
            return(FALSE);
        }
    }
                                       // Else give msg and bail
    else
    {
    /*    char err_msg[MAX_ERROR_MSG_LEN];
        sprintf(err_msg,"No client printer information available");
        m_pSession->ssnErrorMan()->RecordError(err_msg);
        m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                            "", "Warning"); //5.0*/
        return(FALSE);
    }
                                       // Now setup to receive printer
                                       //  info
    char *driver_str_ptr;
    char *output_str_ptr;
    device_str_ptr = all_device_info_buf;
    SlocalPrinterInfo *local_printer_info_ptr;
    idx = 0;
    BOOL done = (m_nLocalPrinterCnt < 1);

    while (!done) //this loop puts real local printers in DLL
    {
        local_printer_info_ptr = m_pLocalPrinterInfoList + idx;

        ::GetProfileString(WININICODE_DEVICES_SECTION_CODE, 
                           device_str_ptr, 
                           "not found",
                           one_device_info_buf, 
                           sizeof(one_device_info_buf));

        if (strcmp(one_device_info_buf, "not found") == 0)
        {
            done = TRUE;
            continue;
        }
                                       // Pull out driver and output
                                       //   strings
                                       //
        driver_str_ptr = ::strtok(one_device_info_buf, ",");
        output_str_ptr = ::strtok(NULL,  ",");

                                       // If this is default printer
                                       //  then save idx
                                       //
        if (strcmp(device_str_ptr,default_printer_name) == 0)
            default_printer_idx = idx;

                                       // Now store and put in list box
                                       //
        strncpy(local_printer_info_ptr->printer_device_name_buf,
                device_str_ptr,
                sizeof(local_printer_info_ptr->printer_device_name_buf));
        strncpy(local_printer_info_ptr->printer_driver_name_buf,
                driver_str_ptr,
                sizeof(local_printer_info_ptr->printer_driver_name_buf));
        strncpy(local_printer_info_ptr->output_device_buf,
                output_str_ptr,
                sizeof(local_printer_info_ptr->output_device_buf));

                                       // Mark other fields as unused
                                       //
        list_box_ptr->InsertString(-1, device_str_ptr);

                                       // Move ptr to next device, if
                                       //   NULL then then done
                                       //
        device_str_ptr += strlen(device_str_ptr) + 1;
        if (*device_str_ptr == '\0')
            done = TRUE;

		++idx;
                                       // If reach max printers quit
                                       //
        //if ((!done) && (++idx >= NTCSSVAL_MAX_LOCAL_PRINTERS) )
          //  done = TRUE;
    }
                                       // If found that a local printer
                                       //   was default then select it
    if (default_printer_idx != -1)
    {
        list_box_ptr->SetCurSel(default_printer_idx);
    }

	//new stuff - Network Printers
	//take printer string apart & stick it in LB
	//put Network in driver field for unique ID
/*	if (nNetPrinters!= 0)
	{
		int i=0;
		CString strNetPrinter;
		while((strNetPrinter=strNetPrintList.GetField(',',i++))!=_T(""))
		{
			local_printer_info_ptr = m_pLocalPrinterInfoList + idx++;
			strcpy(local_printer_info_ptr->printer_device_name_buf,(LPCTSTR)strNetPrinter);
			strcpy(local_printer_info_ptr->printer_driver_name_buf,"NETWORK");
			list_box_ptr->InsertString(-1, strNetPrinter);
		}
	}*/


    return TRUE;
} 



/////////////////////////////////////////////////////////////////////////////
// getLocalPrinterSelection
// ________________________

const SlocalPrinterInfo *CSelPrtDialog::getLocalPrinterSelection()
{
    if (m_bLocalSelected)

        return(&(m_pLocalPrinterInfoList[m_nSelectionIdx]));
    else
        return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// getRemotePrinters
// ________________

BOOL CSelPrtDialog::getRemotePrinters(CListBox* list_box_ptr) 
{
   if (!m_bRemoteEnabled)
        return(FALSE);

    const char *default_printer_name = (LPCTSTR)m_default_prn;
    BOOL default_printer_idx = -1;

    int num_printers;
                                       // Use msg to get list of
                                       //  remote printers from server


    CmsgGETUSERPRTLIST msg;

    //TODO can get rid of this 1st param & make message type auth_server
    BOOL ok = msg.Load(gbl_dll_man.dllUser()->GetAuthServer(m_pSession->ssnAppName()),
                       m_pSession->ssnAppName(),
                       m_szOutputType, m_eSecLvl,
                       m_bDefaultOnly, m_bGeneralAccess);


    if (ok)
	
        ok = msg.DoItNow();


    ok = msg.getPrinterCount(&num_printers);

	

	                                   // Allocate space for remote
                                       //  printer info - note have two
                                       //  types of records here
    if ((ok) && (num_printers > 0))
    {
       

       // if (num_printers > NTCSSVAL_MAX_REMOTE_PRINTERS)
          //  num_printers = NTCSSVAL_MAX_REMOTE_PRINTERS;
             
        m_pRemotePrinterInfoList = 
                            new SremotePrinterInfo[num_printers];

        m_nRemotePrinterCnt = num_printers;

		if (m_pRemotePrinterInfoList == NULL)
        {
            ok = FALSE;
        }
    }

    if (ok)
		
	
        ok = msg.UnLoad(m_pRemotePrinterInfoList,num_printers);

	
	

    if (ok)
    {
                                       // If get in here then got info
                                       //  from server ok, now have to
                                       //  process it
                                       //
                                       // Figure out spooling dir

		                                   
        char tmp_name_buf[SIZE_STANDARD_GENBUF];
          
        getTempFileName(tmp_name_buf, sizeof(tmp_name_buf)); 
		
		
	

        SremotePrinterInfo *remote_printer_info_ptr;

	

                                       // Now loop to fixup file info
                                       //  and list box
                                       //
        for (int idx=0; idx<num_printers; idx++)
        {
                                       // Adjust pointers for the ith
                                       //   printer
			
            remote_printer_info_ptr = 
                        (m_pRemotePrinterInfoList + idx);

			
		


            if (strcmp(remote_printer_info_ptr->redirect_device_buf,
                       NTCSSVAL_PRINTER_REDIRECT_CODE) == 0)
            {
				
                                       // If get in here then printer
                                       //  is redirected
                                       //
                char tmp_prt_buf[SIZE_STANDARD_GENBUF];
                char tmp_host_buf[SIZE_STANDARD_GENBUF];
                
                char *src_ptr = remote_printer_info_ptr-> //...
                                                redirect_device_buf;
                char *trgt_ptr = tmp_prt_buf;

                while (*src_ptr != '\0')
                {
                    if (*src_ptr != '@')
                    {
                        *trgt_ptr++ = *src_ptr++;
                    }
                    else
                    { 
                        src_ptr++;
                        *trgt_ptr = '\0';
                        trgt_ptr = tmp_host_buf;
                    }
                }
                *trgt_ptr = '\0';

                strcpy(remote_printer_info_ptr->redirect_device_buf,
                       tmp_prt_buf);

                strcpy(remote_printer_info_ptr->redirect_host_buf,
                       tmp_host_buf);
            }

            if (remote_printer_info_ptr->default_printer_flag)
            {
			    m_default_prn = remote_printer_info_ptr->printer_name_buf;
                //m_default_prn += "@";
                //m_default_prn += remote_printer_info_ptr->host_name_buf;
            }

		    //ATLASS 1/5/99
            //sprintf(tmp_name_buf,"%s@%s in %s",

			sprintf(tmp_name_buf,"%s in %s",
                    remote_printer_info_ptr->printer_name_buf,
                    //remote_printer_info_ptr->host_name_buf, 1/5/99
                    remote_printer_info_ptr->printer_loc_buf);

                                       // If this is default printer
                                       //  then save idx
                                       //
            if (strcmp(remote_printer_info_ptr->printer_name_buf,
                       default_printer_name) == 0)
                default_printer_idx = idx;

            if (list_box_ptr != NULL)
                list_box_ptr->InsertString(idx,tmp_name_buf);
        }
    }

    if (!ok)
    {
        char err_msg[MAX_ERROR_MSG_LEN];
        sprintf(err_msg,"Problems encountered in processing remote printer information");
        m_pSession->ssnErrorMan()->RecordError(err_msg);
        m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                             /*NULL*/"", "Warning"); //5.0
        return(FALSE);
    }
                                       // If found that a remote printer
                                       //   was default then select it
    if (default_printer_idx != -1)
    {
        list_box_ptr->SetCurSel(default_printer_idx);
    }

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// getRemotePrinterSelection
// _________________________

const SremotePrinterInfo* CSelPrtDialog::getRemotePrinterSelection()
{
    if (m_bRemoteSelected)

        return(&(m_pRemotePrinterInfoList[m_nSelectionIdx]));

    else
        return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// OnClickedOk
// ____________

void CSelPrtDialog::OnClickedOk()
{
    CListBox* list_box_ptr;
    int idx;

	UpdateData();

    if (m_bLocalEnabled)
    {

		
        list_box_ptr = (CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS);

        int idx = list_box_ptr->GetCurSel();

		#ifdef _DEBUG
			CString temp;
			TRACE1(_T("Leaving SelPrtDialog... selected Local Printer-> %s\n"),temp);
        #endif

        if (idx != LB_ERR) 
        {
            m_bLocalSelected = TRUE;
            m_nSelectionIdx = idx;
            CDialog::OnOK();
            return;
        }
    }

    if (m_bRemoteEnabled)
    {
		
        list_box_ptr = (CListBox*) GetDlgItem(IDC_REMOTE_PRINTERS);
        idx = list_box_ptr->GetCurSel();

        if (idx != LB_ERR) 
        {
            SremotePrinterInfo *remote_printer_info_ptr = 
                                 &(m_pRemotePrinterInfoList[idx]);

		    TRACE1(_T("Leaving SelPrtDialog... selected Local Printer-> %s\n"),
				   remote_printer_info_ptr->printer_name_buf);


			remote_printer_info_ptr->nCopies=m_dCopies; //12/5
			remote_printer_info_ptr->bBanner=m_banner; //12/5

				
		    checkForRemoteRedirect(remote_printer_info_ptr);

            m_bRemoteSelected = TRUE;
            m_nSelectionIdx = idx;

	        CDialog::OnOK();
            //return;
        }
    }                                  
                                       // If get here then nothing yet
                                       //   selected
                                       //
    
}



/////////////////////////////////////////////////////////////////////////////
// OnDblclkLocalPrinters
// _____________________

void CSelPrtDialog::OnDblclkLocalPrinters()
{
    if (m_bLocalEnabled)
    {
        CListBox *list_box_ptr = (CListBox*) 
                                 GetDlgItem(IDC_LOCAL_PRINTERS);
        int idx = list_box_ptr->GetCurSel();

        if (idx != LB_ERR) 
        {
            m_bLocalSelected = TRUE;
            m_nSelectionIdx = idx;

            CDialog::OnOK();
        }
    }
} 



/////////////////////////////////////////////////////////////////////////////
// OnDblclkRemotePrinters
// _____________________

void CSelPrtDialog::OnDblclkRemotePrinters()
{
	UpdateData(); //12/5
    if (m_bRemoteEnabled)
    {
        CListBox *list_box_ptr = 
                            (CListBox*) GetDlgItem(IDC_REMOTE_PRINTERS);
        int idx = list_box_ptr->GetCurSel();

        if (idx != LB_ERR) 
        {
            SremotePrinterInfo *remote_printer_info_ptr = 
                                 &(m_pRemotePrinterInfoList[idx]);
	

			remote_printer_info_ptr->nCopies=m_dCopies; //12/5
			remote_printer_info_ptr->bBanner=m_banner; //12/5

				
            checkForRemoteRedirect(remote_printer_info_ptr);

            m_bRemoteSelected = TRUE;
            m_nSelectionIdx = idx;

            CDialog::OnOK();
            return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
// ____________

BOOL CSelPrtDialog::OnInitDialog()
{
	//make the up arrow increase
	CSpinButtonCtrl* pSpin;
	pSpin=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1);
	pSpin->SetRange(0,100);
	
    if ((!m_bLocalEnabled) && (!m_bRemoteEnabled))
        return(FALSE);

    m_bLocalSelected  = m_bRemoteSelected = FALSE;

	centerScreen(m_hWnd);
        
    CListBox* local_listbox = (CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS);
    CListBox* remote_listbox = (CListBox*) GetDlgItem(IDC_REMOTE_PRINTERS);

                                       // If enabled get files and setup
                                       //  local printer list box
                                       //

	GetDlgItem(IDNETWORK)->EnableWindow(m_bRemoteEnabled);

    if (m_bLocalEnabled)
		m_bLocalEnabled = getLocalPrinters(local_listbox);
	else //3/25/99 added to take this out for everything but Select Printer
	{
		GetDlgItem(IDNETWORK)->EnableWindow(FALSE);
		GetDlgItem(IDC_LOCAL_PRINTERS)->EnableWindow(FALSE);
		
	}

	
	(GetDlgItem(IDOPTIONS))->EnableWindow(m_bLocalEnabled); //4/29/99
	//disabled by default & this only way it gets turned on
    local_listbox->EnableWindow(m_bLocalEnabled);

                                       // If enabled get files and setup
                                       //  remote printer list box
                                       //
    if (m_bRemoteEnabled)
        m_bRemoteEnabled = getRemotePrinters(remote_listbox);

    remote_listbox->EnableWindow(m_bRemoteEnabled);

                                       // Give local printers 1st crack
                                       //  at being currently selected -
                                       //  if they are enabled
    BOOL selected = FALSE;
    if (m_bLocalEnabled)
    {
        local_listbox->SetCurSel(0);
        if (local_listbox->GetCurSel() >= 0)
            selected = TRUE;
    }
                                       // Otherwise if remotes are 
                                       //  enabled then try to let the
                                       //  first one be selected
                                       //
    else if ((!selected) && (m_bRemoteEnabled))
    {
        remote_listbox->SetCurSel(0);
        if (remote_listbox->GetCurSel() >= 0)
            remote_listbox->SetCurSel(-1);
    }

  /*  if (!m_bOkToSetup)
    {
        CWnd *select_button = GetDlgItem(IDOPTIONS);

        select_button->EnableWindow(FALSE);
    } */

	return  CDialog::OnInitDialog(); 
} 


/////////////////////////////////////////////////////////////////////////////
// OnOptions
// ________________________

void CSelPrtDialog::OnOptions()
{
    CListBox* list_box_ptr;
    int idx;
    BOOL local_selected = FALSE;

    if (m_bLocalEnabled)
    {
        list_box_ptr = (CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS);
        idx = list_box_ptr->GetCurSel();
        if (idx != LB_ERR)
        {
            local_selected = TRUE;
        }
    }

    if ( (!local_selected) && (m_bRemoteEnabled) )
    {
        list_box_ptr = (CListBox*) GetDlgItem(IDC_REMOTE_PRINTERS);
        idx = list_box_ptr->GetCurSel();
    }

    if (idx == LB_ERR)
    {
        AfxMessageBox("Please select printer before choosing Setup",
                      MB_OK);
        return;
    } 

	char setup_printer_name[SIZE_SMALL_GENBUF];

    if (local_selected)
    {
        SlocalPrinterInfo *local_printer = 
                            (m_pLocalPrinterInfoList + idx);

        strcpy(setup_printer_name,
               local_printer->printer_device_name_buf);
    }
    else
    {
        SremotePrinterInfo *remote_printer = 
                            (m_pRemotePrinterInfoList + idx);

		CWaitCursor wait;	// display wait cursor

			
		//jgj - the setup problem
		{
		CdllSpoolPrintManager* prt_man = gbl_dll_man.dllSpoolPrintMan();
		CPrintUtil CU;
		CU.DelTempPrinter();
	//	makeSpoolFileName(prt_man->m_szSpoolFileName);
		prt_man->AddNTCSSPrinter(remote_printer);
	//jgj - the setup problem
		}
        strcpy(setup_printer_name,"NTCSS"); //jgj - the setup problem
		
    }                          

    HANDLE h_printer;
	PRINTER_DEFAULTS pd;
	try
	{

		CWaitCursor wait;	// display wait cursor
		
		::ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;

   		if(!::OpenPrinter(setup_printer_name, &h_printer, &pd)) //try PRINTER_ALL_ACCESS 1ST
		{
			pd.DesiredAccess = 0;
			if(!::OpenPrinter(setup_printer_name, &h_printer, &pd))//then without
				throw(_T("Printer Setup Problem"));
		}


		if(!::PrinterProperties(AfxGetMainWnd()->GetSafeHwnd(),h_printer))
		throw(_T("Printer Setup Error"));
	}

	 catch(LPTSTR pStr)
	{
		
		m_pSession->ssnErrorMan()->RecordError(pStr);
		m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                            "", "Warning");  
	}
 
    catch(...)
	{
		
		m_pSession->ssnErrorMan()->RecordError("Printer setup error");
		m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                             /*NULL*/"", "Warning");  //5.0
	}
}


/////////////////////////////////////////////////////////////////////////////
// OnSetfocusLocalPrinters
// _______________________

void CSelPrtDialog::OnSetfocusLocalPrinters()
{
    if (m_bRemoteEnabled)
    {
        CListBox* remote_listbox = 
                            (CListBox*) GetDlgItem(IDC_REMOTE_PRINTERS);

        remote_listbox->SetCurSel(-1);
    }
}  

/////////////////////////////////////////////////////////////////////////////
// OnSelchangedLocalPrinters
// _______________________

void CSelPrtDialog::OnSelchangedLocalPrinters()
{

	//do all this to see if we should enable setup button
/*	HANDLE hPrinter;
	PRINTER_DEFAULTS pd;
	CString strPrinter;
	
	
	CWaitCursor wait;	// display wait cursor

	((CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS))->GetText(((CListBox*)
	GetDlgItem(IDC_LOCAL_PRINTERS))->GetCurSel(),strPrinter);

	::ZeroMemory(&pd, sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;

	try
	{

		if(!::OpenPrinter((char*)(LPCTSTR)strPrinter, &hPrinter, &pd)) //try PRINTER_ALL_ACCESS 1ST
		{
			pd.DesiredAccess = 0;
			if(!::OpenPrinter((char*)(LPCTSTR)strPrinter, &hPrinter, &pd)) //then without
				throw _T("Couldn't open printer");
		}


		 DWORD dwNeeded;
		 PRINTER_INFO_2 *pi2 = NULL; 

		::GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
			
		if (dwNeeded == 0) 
			throw _T("Get Printer failed");

		char *buff=new char[dwNeeded];

		pi2=(PRINTER_INFO_2*)buff;
   
		if(!::GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded))
		{
			delete [] buff;
			throw("Get Printer failed");
		}

		AfxMessageBox(pi2->pDriverName);

	 	GetDlgItem(IDOPTIONS)->EnableWindow(strlen(pi2->pDriverName));

		delete [] buff;
	}

		catch(LPTSTR pStr)
		{
			AfxMessageBox(pStr);
			GetDlgItem(IDOPTIONS)->EnableWindow(FALSE);
		}*/

		      	
}



/////////////////////////////////////////////////////////////////////////////
// OnSetfocusRemotePrinters
// ________________________

void CSelPrtDialog::OnSetfocusRemotePrinters()
{
    if (m_bLocalEnabled)
    {
        CListBox* local_listbox = 
                            (CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS);

        local_listbox->SetCurSel(-1);
    }
}  

/////////////////////////////////////////////////////////////////////////////
// OnNetwork
// ________________________

void CSelPrtDialog::OnNetwork()
{
	 HANDLE hPrinter;
	 DWORD dwNeeded;
	 PRINTER_INFO_2 *pi2 = NULL; 
	 char *buff;

	 if((hPrinter=ConnectToPrinterDlg(m_hWnd,0)))
	 {
		 
		 try
		 {

			::GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
		
			if (dwNeeded == 0) 
			  throw _T("Error Adding Network Printer");

			buff=new char[dwNeeded];

			pi2=(PRINTER_INFO_2*)buff;
		   
			if(!::GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded))
				throw _T("Error Adding Network Printer");
		 }
		 	 catch (LPTSTR pstr)
		 {
			 m_pSession->ssnErrorMan()->RecordError(pstr);
			 m_pSession->ssnErrorMan()->HandleError
                            (SELECT_PRINTER_FACILITY_STR,
                            "", "Warning");
			 if (buff)
				delete [] buff;
			 return;
		 }
		 if(((CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS))->   //check for dup
			 FindStringExact(-1,pi2->pPrinterName)==LB_ERR)
		 {


			m_bLocalSelected = TRUE;

			m_nSelectionIdx =((CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS))->
			InsertString(-1,pi2->pPrinterName);

			((CListBox*) GetDlgItem(IDC_LOCAL_PRINTERS))->SetCurSel(m_nSelectionIdx);

			SlocalPrinterInfo *local_printer_info_ptr;

			if(!m_pLocalPrinterInfoList)
			{
				 m_pLocalPrinterInfoList = 
							   new SlocalPrinterInfo[1];
				 m_bLocalEnabled=TRUE;
			}

			local_printer_info_ptr = m_pLocalPrinterInfoList + m_nSelectionIdx;

			 strncpy(local_printer_info_ptr->printer_device_name_buf,
				pi2->pPrinterName,
				sizeof(local_printer_info_ptr->printer_device_name_buf));

		
			strcpy(local_printer_info_ptr->printer_driver_name_buf,
				"NETWORK");


			GetDlgItem(IDNETWORK)->EnableWindow(FALSE);
		 }

		 if(buff)
			 delete [] buff;
		 
    }


}



