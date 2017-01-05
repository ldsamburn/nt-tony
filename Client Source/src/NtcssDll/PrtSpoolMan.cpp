/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtSpoolMan.cpp
//                                         
// This is the implementation file for CdllSpoolPrintManager.
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include "0_basics.h"
#include "PrtSpoolMan.h"                    
#include "PrtSelectDlg.h"
#include "PrtDriver.h"
#include "WinSpool.h"
#include "Inri_Ftp.h"
#include "sys/stat.h"
#include "PrintUtil.h"
#include "StringEx.h"
#include  "dll_msgs.h"
#include  "AtlassFileDlg.h"
#include  "inri_ftp.h"

#ifdef NTCSS_16BIT_BUILD
#include <stdio.h>
#endif // NTCSS_16BIT_BUILD


/////////////////////////////////////////////////////////////////////////////
// CdllSpoolPrintManager constructor
// _____________________

CdllSpoolPrintManager::CdllSpoolPrintManager(Csession *session)
{
    m_bIsOkay = TRUE;
    m_bCancelled = FALSE;

    m_bSelectedPrinter=FALSE;
    m_pSession = session;

    m_TheHDC = NULL;

    m_szOrigProfile[0] = '\0';
    m_szSpoolFileName[0] = '\0';
	m_bAtlassAppend=m_bAtlassPrompt=FALSE;
	
} 


/////////////////////////////////////////////////////////////////////////////
// deleteFile 
// __________

BOOL CdllSpoolPrintManager::deleteFile(const char *file_name)
{
    BOOL ok;

#ifdef NTCSS_32BIT_BUILD

    ok = DeleteFile(file_name);

#else  // NTCSS_16BIT_BUILD

    ok = (remove(m_szSpoolFileName) == 0);

#endif // NTCSS_32BIT_BUILD / NTCSS_16BIT_BUILD             

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// doHdcLocalPrintSpooling 
// _______________________

BOOL CdllSpoolPrintManager::doHdcLocalPrintSpooling
                        (const SlocalPrinterInfo  *local_printer_ptr)
{
                                       // Nothing really todo but
                                       //  the EndDoc
    CDC *pDC = new CDC;
    BOOL ok = pDC->Attach (m_TheHDC);

                                       // NEEDTODO - had problems so am
                                       //  for now not checking return here
    if (ok)
        pDC->EndDoc();             
                    
    if (!ok)
        m_pSession->ssnErrorMan()->RecordError
                            ("Local printer end document error");

    if (ok)
        ok = setupHdcForLocalPrinter(local_printer_ptr, NULL, TRUE);

    if (pDC != NULL)
    {
        pDC->Detach();
        delete pDC;
    }
    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// doHdcRemotePrintSpooling 
// _______________________

BOOL CdllSpoolPrintManager::doHdcRemotePrintSpooling
                        (const SremotePrinterInfo  *remote_printer_ptr)
{
    CDC *pDC = new CDC;
    BOOL ok = pDC->Attach (m_TheHDC);

                                       // NEEDTODO - had problems so am
                                       //  for now not checking return here
    if (ok)
        pDC->EndDoc();             
                    
    if (!ok)
        m_pSession->ssnErrorMan()->RecordError
                            ("Remote printer end document error");

                                       // Now have to use print driver 
                                       //  to take file that was printed
                                       //  to and send it to remote
                                       //  printer
    if (ok)
    {
        CprintDriver print_driver(m_pSession);

	

        ok = print_driver.sendRawRemoteLPRjob
                            (remote_printer_ptr,
                             m_szSpoolFileName,
                             m_eSeclvl);
        if (ok)
        {
            deleteFile(m_szSpoolFileName);
       }
    }

    if (ok)
        ok = setupHdcForRemotePrinter(remote_printer_ptr, NULL, TRUE);

    if (pDC != NULL)
    {
        pDC->Detach();
        delete pDC;
    }

    return(ok);
}


/////////////////////////////////////////////////////////////////////////////
// doWinIniPrintSpooling 
// _____________________

BOOL CdllSpoolPrintManager::doWinIniPrintSpooling
                      (const SlocalPrinterInfo  *local_printer_ptr,
                       const SremotePrinterInfo *remote_printer_ptr)
{
    CprintDriver print_driver(m_pSession);

    BOOL ok = TRUE;

    if (local_printer_ptr != NULL)
    {
        ; // Nothing to do in this case as dc is direct to printer
//        ok = print_driver.sendRawLocalLPRjob(local_printer_ptr,
//                                             m_szSpoolFileName);

    }
    else
    {

//		CFile spoolfile(m_szSpoolFileName,CFile::modeCreate|CFile::modeReadWrite);
//        spoolfile.Close();
		if(m_bAtlassPrompt) //Atlass prompt for file stuff
			                //Just Ftp File to Server
		{
			CInriFtpMan ftp_man;
			if (ftp_man.Initialize())
			{
				ftp_man.SetUnixID(0);

				if(ftp_man.PutFile(gbl_dll_man.dllServerName(),
							 (LPCTSTR)m_strAtlassPrintFile,
							 m_szSpoolFileName,
							 FALSE,0666,m_bAtlassAppend)) //defaulting to binary
				{
					deleteFile(m_szSpoolFileName);
					return TRUE;
				}
			}
			m_pSession->ssnErrorMan()->RecordError
             ("Cannot Ftp Print File");
			return FALSE;
		}

///////////////////////////////////////////////////////////////////////////
	
        ok = print_driver.sendRawRemoteLPRjob(remote_printer_ptr,
                                              m_szSpoolFileName,
                                              m_eSeclvl);
        if (ok)
        {
            deleteFile(m_szSpoolFileName);

        }
    }

   /* if (ok)
    {
        ok = setupWinIniPrinting(local_printer_ptr,remote_printer_ptr,
                                 FALSE);
    } jgj 5/14 */

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// restorePrinter 
// _____________

BOOL CdllSpoolPrintManager::restorePrinter()
{
    if ((!m_bIsOkay) || (m_bCancelled) || (!m_bSelectedPrinter))
    {
        m_pSession->ssnErrorMan()->RecordError
               ("Cannot restore, printer selection was not successfully completed");

        return(FALSE);
    }

	//New Network Printer Stuff
	if(m_bNetworkPrinter)
	{
		if(!::DeletePrinterConnection((char*)(LPCTSTR)m_strUNC))
		{
			m_pSession->ssnErrorMan()->RecordError
            ("Cannot remove Network Printer");
			//TODO use getlast error routine here

			return FALSE;
		}
		else
		{
			m_bNetworkPrinter=FALSE;
			::WriteProfileString(WININICODE_WINDOWS_SECTION_CODE,
                                  WININICODE_A_DEVICE_KEY_CODE, 
                                  m_szOrigProfile);
			return TRUE;
		}
	}
                                       // Just in case will try to
                                       //  delete tmp spool file

    if (m_szSpoolFileName[0] != '\0')
    {
        deleteFile(m_szSpoolFileName);
    }

    BOOL ok = TRUE;

    if (m_szOrigProfile[0] != '\0')  
    {
	
	//	AfxMessageBox("Restoring  Printer-> "+ (CString)m_szOrigProfile); //jgj

        ok = ::WriteProfileString(WININICODE_WINDOWS_SECTION_CODE,
                                  WININICODE_A_DEVICE_KEY_CODE, 
                                  m_szOrigProfile);
		
	     if (!ok)
            m_pSession->ssnErrorMan()->RecordError
               ("Unable to restore original print device setting");

		 CPrintUtil PU;
		 PU.DelTempPrinter();
	
    }  

    if (m_TheHDC != NULL)
    {    
        if (!(::DeleteDC(m_TheHDC)))
        {
                                       // Is okay if this error covers
                                       //  up WriteProfileString error
            ok = FALSE;
            m_pSession->ssnErrorMan()->RecordError
               ("Unable to free device context structure");
        }
        m_TheHDC = NULL;
    }
                                       // Set this so will have to go
                                       //  through SelectPrinter again
    m_bSelectedPrinter = FALSE;

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// selectPrinter 
// _____________

BOOL CdllSpoolPrintManager::selectPrinter 
                      (const char           *output_type_str,
                       const int             seclvl,
                       const BOOL            default_flag,
                       const BOOL            gen_access_flag,
                       const BOOL            return_DC_flag,
                       const NtcssPrintAttrib    *pr_attr_ptr,
                       const BOOL            local_enable_flag,
                       const BOOL            remote_enable_flag)
{
    if (m_bSelectedPrinter)
    {
        m_pSession->ssnErrorMan()->RecordError
            ("Printer already selected, must restore first");
        return(NULL);
    }

    BOOL ok = TRUE;
    BOOL cancelled = FALSE;

    SremotePrinterInfo *remote_printer_ptr = NULL;
    SlocalPrinterInfo  *local_printer_ptr = NULL;

    char selected_printer_name[SIZE_SMALL_GENBUF];


    CSelPrtDialog sel_prn_dlg(CWnd::FromHandle(m_pSession->ssnHWnd()),
                              m_pSession,TRUE,pr_attr_ptr->copies,
							  pr_attr_ptr->banner);

                                       // If just need to get default
                                       //   printer info
    if (default_flag)
    {
                                       // Setup and use sel_prn_dlg to
                                       //  get just default printer
                                       //

		

        sel_prn_dlg.enableRemoteSelection(output_type_str,
                                          seclvl,
                                          default_flag,
                                          gen_access_flag);

		
                                       // Get ptr to default printer info
                                       //
        if (ok)
        {
            remote_printer_ptr =
                    (SremotePrinterInfo *)
					 sel_prn_dlg.getDefaultRemotePrinter();
					 					
           ok = (remote_printer_ptr != NULL);
		  // if(remote_printer_ptr == NULL) return(FALSE); //jgj 
		}
    }
                                       // Else really need dialog for
                                       //  user interaction,
    else 
    {
        if (local_enable_flag)
            sel_prn_dlg.enableLocalSelection();

        if (remote_enable_flag)
            sel_prn_dlg.enableRemoteSelection(output_type_str,
                                              seclvl,
                                              default_flag,
                                              gen_access_flag);

        int result = sel_prn_dlg.DoModal();

                                       // If user selected ok then
                                       //  get ptr to printer selected,
                                       //  see if local or remote
        if (result == IDOK)
        {
            if (sel_prn_dlg.selectedLocalPrinter())
            {
               local_printer_ptr = (SlocalPrinterInfo *)
                            sel_prn_dlg.getLocalPrinterSelection();

               strcpy(selected_printer_name,
                      local_printer_ptr->printer_device_name_buf);
            }
            else if (sel_prn_dlg.selectedRemotePrinter())
            {
               remote_printer_ptr = (SremotePrinterInfo *)
                            sel_prn_dlg.getRemotePrinterSelection();
               strcpy(selected_printer_name,
                      remote_printer_ptr->printer_device_name_buf);
            }
        }
        else
            m_bCancelled = TRUE;
    }

                                       // Make sure have printer ptr
    if ((!m_bCancelled) &&
        (local_printer_ptr == NULL) && (remote_printer_ptr == NULL))
    {
	    m_pSession->ssnErrorMan()->RecordError
            ("Printer selection error");
        ok = FALSE;
		
    }

	//if(remote_printer_ptr) //added 12/3
	//{                      //to pass through these attributes
	//	remote_printer_ptr->nCopies=pr_attr_ptr->copies;
	//	remote_printer_ptr->bBanner=pr_attr_ptr->banner;
	//}

                                       // Here are setups if caller
                                       //  wants an HDC
                                       //
    if ((!m_bCancelled) && (return_DC_flag)&&ok) //jgj added ok 6-30-97
    {
        if (local_printer_ptr != NULL)
        {
            ok = setupHdcForLocalPrinter(local_printer_ptr, 
                                         pr_attr_ptr, FALSE);
        }
        else
        {
            ok = setupHdcForRemotePrinter(remote_printer_ptr, 
                                          pr_attr_ptr, FALSE);
        }
    }
                                       // Here are setups if caller
                                       //  wants INI file print spooling
                                       //
    else if (!m_bCancelled&&ok) // && !return_DC_flag jgj added ok 6-30-97
	    ok = setupWinIniPrinting(local_printer_ptr,remote_printer_ptr);
	                                       // Will let session keep track
                                       //  of the selected printer
    if ((ok) && (!m_bCancelled))
    {
	    if (local_printer_ptr != NULL)
        {
            ok = m_pSession->setLocalPrinterInfo(local_printer_ptr, TRUE);
        }
        else
        {
            ok = m_pSession->setRemotePrinterInfo(remote_printer_ptr, TRUE);
        }
	
    }

                                       // If ok and didn't cancel then
                                       //  return is HDC if that's what
                                       //  caller wants or TRUE if 
                                       //  doing INI file setup
    if ((ok) && (!m_bCancelled))
    {
        m_bSelectedPrinter = TRUE;
		OnSetprinter(pr_attr_ptr,local_printer_ptr != NULL);
		m_eSeclvl = seclvl;

        return(TRUE);
    }                                  
                                       // Is okay if user cancelled,
                                       //  caller has to use
                                       //  userCancelled() to find out

    else if (m_bCancelled)
    {
        m_bIsOkay = FALSE;
		//jgj printer setup problem
		//here take out the printer if they happened to 
		//have hit setup on a remote which would do an add
		CPrintUtil PU;
		PU.DelTempPrinter();
        return(TRUE);
    }                                   // Otherwise there was an error
    else
    {
        m_bIsOkay = FALSE;
        return(FALSE);
    }
}



/////////////////////////////////////////////////////////////////////////////
// setupHdcForLocalPrinter               
// _______________________

/////////////////////////////////////////////////////////////////////////////
// CdllSpoolPrintManager::setupHdcForLocalPrinter               
// ______________________________________________

BOOL CdllSpoolPrintManager::setupHdcForLocalPrinter
                        (const SlocalPrinterInfo  *local_printer_ptr,
                         const NtcssPrintAttrib   *pr_attr_ptr,
                         BOOL hdc_exists)
{
    if (!hdc_exists)
    {

        m_TheHDC = CreateDC("WINSPOOL", 
                             local_printer_ptr->printer_device_name_buf,
                             NULL, NULL);


        if (m_TheHDC == NULL)
        {
            char err_buf[NTCSS_MAX_ERROR_MSG_LEN];

            sprintf(err_buf,
                "Unable to obtain device context for printer %s",
                local_printer_ptr->printer_device_name_buf);
            m_pSession->ssnErrorMan()->RecordError(err_buf);
            return(FALSE);
        }
    }

    DOCINFO lcl_doc_info;
                
    lcl_doc_info.cbSize = sizeof(DOCINFO);
    lcl_doc_info.lpszDocName = NTCSSVAL_LPR_LOCALPRT_DOC_NAME;

                                       // NULL because going directly
                                       //  to printer
                                       //
    lcl_doc_info.lpszOutput = NULL;

                                       // NEEDTODO:
                                       // Not sure about these 2,
                                       //  help was sparse, are
                                       //  for Windows 95 only

#ifdef NTCSS_32BIT_BUILD

    lcl_doc_info.lpszDatatype = "";
    lcl_doc_info.fwType = 0;

#endif // NTCSS_32BIT_BUILD

    CDC *pDC;
    pDC = new CDC;
    BOOL ok = pDC->Attach (m_TheHDC);

    if (ok)
        ok = (pDC->StartDoc(&lcl_doc_info) > 0);

    if (!ok)
        m_pSession->ssnErrorMan()->RecordError("Local printer setup error");

    if (pDC != NULL)
    {
        pDC->Detach();
        delete pDC;
    }
    return(ok);        
}


/////////////////////////////////////////////////////////////////////////////
// setupHdcForRemotePrinter 
// _______________________

BOOL CdllSpoolPrintManager::setupHdcForRemotePrinter
                        (const SremotePrinterInfo  *remote_printer_ptr,
                         const NtcssPrintAttrib         *pr_attr_ptr,
                         BOOL hdc_exists)
{
    if (!hdc_exists)
    {

        m_TheHDC = CreateDC("WINSPOOL", 
                             remote_printer_ptr->printer_device_name_buf,
                             NULL, NULL);


        if (m_TheHDC == NULL)
        {
            char err_buf[NTCSS_MAX_ERROR_MSG_LEN];

            sprintf(err_buf,
                    "Unable to obtain device context for printer %s",
                    remote_printer_ptr->printer_device_name_buf);
            m_pSession->ssnErrorMan()->RecordError(err_buf);
            return(FALSE);
        }
    }
                                       // Since going to a 
                                       //  remote printer then will have 
                                       //  to spool the output to a temp
                                       //  file

//    char spool_file_name[SIZE_SMALL_GENBUF];

    makeSpoolFileName(m_szSpoolFileName);    

    DOCINFO rmt_doc_info;
    rmt_doc_info.cbSize = sizeof(DOCINFO);
    rmt_doc_info.lpszDocName = m_szSpoolFileName;

                                       // Here going to spool file
                                       //
    rmt_doc_info.lpszOutput = m_szSpoolFileName;

                                       // NEEDTODO:
                                       // Not sure about these 2,
                                       //  help was sparse, are
                                       //  for Windows 95 only
#ifdef NTCSS_32BIT_BUILD

    rmt_doc_info.lpszDatatype = "";
    rmt_doc_info.fwType = 0;

#endif // NTCSS_32BIT_BUILD

    CDC *pDC = new CDC;
    BOOL ok = pDC->Attach (m_TheHDC);

    if (ok)
        ok = (pDC->StartDoc(&rmt_doc_info) > 0);

    if (!ok)
    {
        char err_buf[NTCSS_MAX_ERROR_MSG_LEN];
        sprintf(err_buf,
                "Unable to set up spool file \n  %s\nfor remote printer\n  %s",
                m_szSpoolFileName, remote_printer_ptr->printer_device_name_buf);
        m_pSession->ssnErrorMan()->RecordError(err_buf);
    }

    if (pDC != NULL)
    {
        pDC->Detach();
        delete pDC;
    }

    return(ok);
}


////////////////////////////////////////////////////////////////////////////
// setupWinIniPrinting 
// _______________________

BOOL CdllSpoolPrintManager::setupWinIniPrinting
                      (const SlocalPrinterInfo  *local_printer_ptr,
                       const SremotePrinterInfo *remote_printer_ptr)

{
	CString temp;
	m_bNetworkPrinter=FALSE;

    
    ::GetProfileString(WININICODE_WINDOWS_SECTION_CODE, 
                       WININICODE_A_DEVICE_KEY_CODE, 
                       "", m_szOrigProfile, 
                       sizeof(m_szOrigProfile));

	
    if (local_printer_ptr)
	{
		if(strcmp(local_printer_ptr->printer_driver_name_buf,"NETWORK"))
			temp.Format((CString)local_printer_ptr->printer_device_name_buf+","
			+(CString)local_printer_ptr->printer_driver_name_buf+","
			+(CString)local_printer_ptr->output_device_buf);
		else 
		
			m_bNetworkPrinter=TRUE; //Network Printer Stuff
	
	}

    else  //remote ini
    {
		//ATLASS STUFF
		if(!strcmp(remote_printer_ptr->AtlassFileName,"***"))  //Atlass prompt for file
		{
			m_bAtlassPrompt=TRUE;
			CAtlassFileDlg dlg;
			if(dlg.DoModal()==IDOK)
			{
				m_strAtlassPrintFile=dlg.m_strAtlassFname;
				m_bAtlassAppend=!dlg.m_nAppend;

			}
			else
			{
				m_pSession->ssnErrorMan()->RecordError("No Atlass File Name specified");
				return(FALSE);
			}
		}
 
        makeSpoolFileName(m_szSpoolFileName);

		CFile spoolfile(m_szSpoolFileName,CFile::modeCreate|CFile::modeReadWrite);
		spoolfile.Close();

		//jgj - the setup problem
		HANDLE lh_Printer;
		PRINTER_DEFAULTS pd;
		
		::ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		if (!::OpenPrinter("NTCSS",(LPHANDLE)(&lh_Printer),&pd))
		//jgj - the setup problem
			if (!AddNTCSSPrinter(remote_printer_ptr)) //add temp printer & port
			{
				if(::WriteProfileString("Ports",m_szSpoolFileName,NULL))//this should delete 
				::SendMessage(HWND_BROADCAST,WM_WININICHANGE,0L,0L);//port from 
				m_pSession->ssnErrorMan()->RecordError              //Registry on failure
				("Error adding Temp NTCSS Printer");
				return(FALSE);
			}

		temp.Format("NTCSS,"+(CString)remote_printer_ptr-> printer_datafile_name_buf+","
	        +(CString)m_szSpoolFileName);

	}    //end remote ini

		
	if(!m_bNetworkPrinter) //this is the EASIEST way to set default
		if(::WriteProfileString("windows","device",temp))
			return(TRUE);

		else
		{
			m_pSession->ssnErrorMan()->RecordError
						("Error in setting WIN.INI device value");
			return(FALSE);
		}
	else //it's a Network Printer
	{
		//CString strUNC=local_printer_ptr->printer_device_name_buf;

        //Network Printing stuff
		//1st Add it
		if(!AddPrinterConnection((char*)local_printer_ptr->printer_device_name_buf))
		{
			DWORD error=::GetLastError();
			CString Serror;
			Serror.Format("Error code-> %d",error);
			AfxMessageBox(Serror);
			if(error==5) //Access Denied should be only 1 hit
				m_pSession->ssnErrorMan()->RecordError("Error Access to Printer Denied");
			else
				m_pSession->ssnErrorMan()->RecordError("Error Setting Printer Connection");
			m_bNetworkPrinter=FALSE;
			//TODO use getlast error routine here
			return(FALSE);
		}
		//Then set it as default 
		//1st Dig out driver this is easier then EnumPrinter

	
		CStringEx buf;
		::GetProfileString(WININICODE_DEVICES_SECTION_CODE , 
                           local_printer_ptr->printer_device_name_buf, 
                           "not found",
                           buf.GetBuffer(MAX_PATH), 
                           MAX_PATH);

		buf.ReleaseBuffer();

		if(buf==_T("not found"))
		{
			m_pSession->ssnErrorMan()->
			RecordError("Error Setting Network Printer as default");
			::DeletePrinterConnection((char*)local_printer_ptr->printer_device_name_buf);
			return FALSE;
		}

		temp.Format("%s,%s,%s",local_printer_ptr->printer_device_name_buf,buf.
			 GetField(',',0),buf.GetField(',',1));

		//now make it default
		if(!::WriteProfileString("windows","device",(char*)(LPCTSTR)temp))
		{
			m_pSession->ssnErrorMan()->
			RecordError("Error Setting Network Printer as default");
			::DeletePrinterConnection((char*)local_printer_ptr->printer_device_name_buf);
			return(FALSE);
		}

		m_strUNC=local_printer_ptr->printer_device_name_buf; //Got to save UNC here so Restore can
															//take out Printer
		return(TRUE);

	}

}


/////////////////////////////////////////////////////////////////////////////
// SpoolPrintRequest 
// _________________

BOOL CdllSpoolPrintManager::spoolPrintRequest()
{
    if ((!m_bIsOkay) || (m_bCancelled) || (!m_bSelectedPrinter))
    {
        m_pSession->ssnErrorMan()->RecordError
               ("Cannot spool, printer selection was not successfully completed");

        return(FALSE);
    }

    BOOL using_local_printer = m_pSession->isLocalPrinterMru();

    BOOL ok = TRUE;

    if (m_TheHDC != NULL)
    {
        if (using_local_printer)

            ok = doHdcLocalPrintSpooling(m_pSession->localPrinterInfo());

        else
            ok = doHdcRemotePrintSpooling(m_pSession->remotePrinterInfo());
    }
    else
    {
        if (using_local_printer)

            ok = doWinIniPrintSpooling(m_pSession->localPrinterInfo(),
                                       NULL);
        else
            ok = doWinIniPrintSpooling(NULL,
                                       m_pSession->remotePrinterInfo());
    }

    return(ok);
}

/////////////////////////////////////////////////////////////////////////////
// AddTempPrintDriver 
// __________

BOOL CdllSpoolPrintManager::AddTempPrintDriver(const SremotePrinterInfo *remote_printer_ptr)
{

	
	//Save System Spool Directory in member variable

	DWORD needed;

	::GetPrinterDriverDirectory(NULL,NULL,1,NULL,0,&needed);

	unsigned char *buffer=new unsigned char[needed];

	if(::GetPrinterDriverDirectory(NULL,NULL,1,buffer,needed,&needed))
	   m_strNtSpoolDir=buffer;
	else
	{
		AfxMessageBox(_T("Couldn't obtain system spool directory"));
		delete [] buffer;
		return FALSE;
	}

    delete [] buffer;

	m_strNtSpoolDir+=_T("\\");

	//Save System Dir in member Var for addmonitor

	UINT size=::GetSystemDirectory(NULL,0);
	CString temp;
	::GetSystemDirectory(m_strNTSysDir.GetBuffer(size),size);
	m_strNTSysDir.ReleaseBuffer();


	m_strNTSysDir+=_T("\\");

	//first see if it's already loaded

	CWaitCursor wait;	// display wait cursor

	DWORD pcbNeeded,pcReturned;
	HGLOBAL hGlobal=NULL;
	DRIVER_INFO_1 *di1=NULL;
	BOOL ok=TRUE;
	BOOL found=FALSE;

	//AfxMessageBox("in add driver");
	//TODO put this whole function in a try block & just use new & delete

	::EnumPrinterDrivers(NULL,NULL,1,0,0,&pcbNeeded,&pcReturned);
	if(pcbNeeded==0)
		ok=FALSE;

	if((hGlobal = ::GlobalAlloc(GHND, pcbNeeded))==NULL)
		ok=FALSE;

	if((di1 = (DRIVER_INFO_1 *)::GlobalLock(hGlobal))==NULL)
		ok=FALSE;

	if(!::EnumPrinterDrivers(NULL,NULL,1,(LPBYTE)di1,pcbNeeded,&pcbNeeded,&pcReturned))
		ok=FALSE;

    if(ok)
		for(DWORD i=0;i<pcReturned;i++) //try to find it in list
		{	
			if(!strcmp(di1[i].pName,(char*)remote_printer_ptr-> printer_device_name_buf))
				found=TRUE;
		}

	if (di1 != NULL)
		::GlobalUnlock(hGlobal);

	if (hGlobal != NULL)
		::GlobalFree(hGlobal);
    
	if(found)
	{
		//AfxMessageBox("found it"); //debug!!!!!
		//TODO this should return result of this function
		GetDrivers(remote_printer_ptr); //just because it's installed doesn't mean they have 
		return TRUE;                    //all the files, not sure they need them though
	}
	else
	{
	
		
		if(!GetDrivers(remote_printer_ptr))
		{
			//make sure we have all the driverfiles 
			AfxMessageBox("Can't get drivers"); //debug
			return FALSE;
		}

			
		//DEVMODE ls_devmode;
		//LPDEVMODE lprt_devmode=&ls_devmode;
				
		CString datafile=m_strNtSpoolDir+(CString)remote_printer_ptr->printer_datafile_name_buf;
		CString driver=m_strNtSpoolDir+(CString)remote_printer_ptr-> printer_NTdriver_name_buf ;
		CString uifile=m_strNtSpoolDir+(CString)remote_printer_ptr-> printer_uifile_name_buf;

		CString helpfile=m_strNtSpoolDir+(CString)remote_printer_ptr-> printer_helpfile_name_buf;
		CString monitorfile=(CString)remote_printer_ptr-> printer_monitorfile_name_buf;
		if(monitorfile.CompareNoCase(_T("none")) || !monitorfile.IsEmpty())
			monitorfile=m_strNtSpoolDir+monitorfile;
		CString monitorstring=(CString)remote_printer_ptr-> printer_monitorstring_name_buf;
		CStringEx CopyFiles=remote_printer_ptr->CopyFiles;
//		AfxMessageBox("CopyFiles 1 -> " + CopyFiles);


		TCHAR szBuf[SIZE_COPY_FILES];
		memset(szBuf,NULL,SIZE_COPY_FILES);

		CString strDependentFiles;
		strDependentFiles.Format("%s,%s,%s,%s",datafile,driver,uifile,helpfile);


		if(!monitorstring.IsEmpty()) //see if we have to add monitor
		{
			if(monitorfile.IsEmpty())
			{
				AfxMessageBox(_T("Monitor String But no file!!"));
				return FALSE;
			}

			strDependentFiles+=_T(",")+monitorfile;

			if(!NtcssAddMonitor(monitorstring,monitorfile))
			{
				AfxMessageBox(_T("Error couldn't add monitor file"));
				return FALSE;
			}
		}

	

		//need this @ top so we don't dup 5 main files &need to append path to copy files
		//so if copy files are there, use it completly
	
		if(CopyFiles.CompareNoCase(_T("none")))
		{
			CString strFile;
			for(int i=0;;i++)
			{
			   if((strFile=CopyFiles.GetField(',',i))==_T(""))
				   break;
			   strDependentFiles.Format("%s,%s",strDependentFiles,m_strNtSpoolDir+strFile);
		   		
			}
		}

				
		//make dependent file each Null terminated
		strcpy(szBuf,strDependentFiles.GetBuffer(strDependentFiles.GetLength()));
		LPTSTR p = szBuf;
		while (*p)
		{
		   if (*p == ',')
			  *p = '\0';
		   p++;
		} 
		*p=NULL;

		strDependentFiles.ReleaseBuffer();
		
	/*	AfxMessageBox("Adding Driver");
		AfxMessageBox("Driver Name-> " + (CString)remote_printer_ptr-> printer_device_name_buf);
		AfxMessageBox("Driver-> " + driver);
		AfxMessageBox("datafile-> " + datafile);
		AfxMessageBox("uifile-> " + uifile);
		AfxMessageBox("help-> " + helpfile);
		AfxMessageBox("monitorstring-> " + monitorstring);
		AfxMessageBox("monitorfile-> " + monitorfile);
		AfxMessageBox("CopyFiles are comming next");*/

	/*	p=szBuf;
		while(strlen(p))
		{
			AfxMessageBox(p);
			p+=strlen(p)+1;
		} */
		
				
		DRIVER_INFO_3 ls_pinfo={2,(char*)remote_printer_ptr-> printer_device_name_buf, 
		_T("Windows NT x86"),
		(char*)(LPCTSTR)driver,
		(char*)(LPCTSTR)datafile,
		(char*)(LPCTSTR)uifile,
		(char*)(LPCTSTR)helpfile,
		szBuf,
		(!monitorstring.IsEmpty()?
		(char*)(LPCTSTR)monitorstring:NULL),NULL};
		DWORD error_code;
		BOOL ret;

		ret=::AddPrinterDriver(NULL,3,LPBYTE(&ls_pinfo));
		error_code=GetLastError();
		if(!ret)
			AfxMessageBox(_T("Adding Printer Driver Failed-> ") + GetSysError(error_code));

		return ret;
	   
	}
	    
}


/////////////////////////////////////////////////////////////////////////////
// AddTempPrinter 
// __________

BOOL CdllSpoolPrintManager::AddTempPrinter(const SremotePrinterInfo *remote_printer_ptr/*,CString why*/)
{


CWaitCursor wait;	// display wait cursor

DEVMODE ls_devmode;
PSECURITY_DESCRIPTOR pSecurityDescriptor;	 
DWORD dwRevision=SECURITY_DESCRIPTOR_REVISION,dwError;
HANDLE hPrinter;

//AfxMessageBox("in add print");	

try
{
	::ZeroMemory( &ls_devmode, sizeof(ls_devmode) );

	if(!(::InitializeSecurityDescriptor(&pSecurityDescriptor,dwRevision)))
	  throw (_T("Couldn't initialize security descriptor"));
	//jj 9-24 new stuff spoolfile causing problems
	//////////////////////////////////////////////
	CString why;

	makeSpoolFileName(why.GetBuffer(_MAX_PATH));
	why.ReleaseBuffer();
	//AfxMessageBox("In Add Printer and m_szSpoolFileName is -> " + why);

	//jj 9-24 new stuff spoolfile causing problems
	//////////////////////////////////////////////

	//AfxMessageBox("in add printer using-> "+(CString)remote_printer_ptr-> printer_device_name_buf);

	PRINTER_INFO_2 ls_pinfo={NULL,"NTCSS",
	NULL,(char*)(LPCTSTR)why,(LPTSTR)remote_printer_ptr-> printer_device_name_buf,
	NULL,NULL,&ls_devmode,NULL,"winprint","RAW",NULL,pSecurityDescriptor,
	0,0,0,0,0,0,0,0};

	hPrinter =::AddPrinter(NULL,2,(LPBYTE) &ls_pinfo);
	dwError = ::GetLastError();

	if(!hPrinter)
		throw(_T("Failed to add printer") + GetSysError(dwError));
}

catch (LPTSTR pStr)
{
	AfxMessageBox(pStr);
	return FALSE;
}


return (hPrinter!=NULL);

}


/////////////////////////////////////////////////////////////////////////////
// AddNTCSSPrinter 
// __________

BOOL CdllSpoolPrintManager::AddNTCSSPrinter(const SremotePrinterInfo *remote_printer_ptr)
{
	CPrintUtil PU;
	PU.AddNTCSSPort();
	return AddTempPrintDriver(remote_printer_ptr) &&
		   AddTempPrinter(remote_printer_ptr/*,(CString) m_szSpoolFileName*/);
	 
}


/////////////////////////////////////////////////////////////////////////////
// GetDrivers 
// __________

BOOL CdllSpoolPrintManager::GetDrivers(const SremotePrinterInfo *remote_printer_ptr)
//this is called to see if the client has the drivers the server says it needs to 
//do the remote print job if not we need to do an ftp
//we're assuming user can't select a remote printer where the server doesn't have drivers
//if we need them, we do an implicit FTP


{
	CWaitCursor wait;	// display wait cursor


	struct _stat buf;


	
	//copy driver//////////////////////////////////////////////////////////////////////////
	CString strPutFile=m_strNtSpoolDir+
		         (CString)remote_printer_ptr-> printer_NTdriver_name_buf;

    
	if( _stat(strPutFile, &buf ))
		if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_NTdriver_name_buf)))
			return FALSE;
			

	//copy datafile/////////////////////////////////////////////////////////////////////////
   	strPutFile=m_strNtSpoolDir+
		         (CString)remote_printer_ptr-> printer_datafile_name_buf;

	if( _stat(strPutFile, &buf ))
        if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_datafile_name_buf)))
			return FALSE;
		

	//copy UIfile/////////////////////////////////////////////////////////////////////////

	strPutFile=m_strNtSpoolDir+
		         (CString)remote_printer_ptr->printer_uifile_name_buf;

	if( _stat(strPutFile, &buf ))
		if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_uifile_name_buf)))
			return FALSE;
			
		
	//copy Helpfile/////////////////////////////////////////////////////////////////////////

	strPutFile=m_strNtSpoolDir+
		         (CString)remote_printer_ptr->printer_helpfile_name_buf;

	if( _stat(strPutFile, &buf ))
		if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_helpfile_name_buf)))
			return FALSE;
			

   //copy MonitorFile/////////////////////////////////////////////////////////////////////////
	if(strlen(remote_printer_ptr-> printer_monitorstring_name_buf)>0)
	{
		//this needs to be in sys32!!! but we'll put it there & spool like everyone else
		strPutFile=m_strNTSysDir+
		         (CString)remote_printer_ptr->printer_monitorfile_name_buf;

		if( _stat(strPutFile, &buf ))
			if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_monitorfile_name_buf)))
			return FALSE;

		strPutFile=m_strNtSpoolDir+
		         (CString)remote_printer_ptr->printer_monitorfile_name_buf;

			if( _stat(strPutFile, &buf ))
				if(!(GetNTDriver(strPutFile,(CString)remote_printer_ptr->printer_monitorfile_name_buf)))
			return FALSE;
	}

	//copy Copy Files files//////
	//message returns only files in addition to the above 5!!!!!

	CStringEx strCopyFiles=remote_printer_ptr->CopyFiles;

	if (strCopyFiles.CompareNoCase(_T("none")))
	{
		CString strFile;
		for(int i=0;;i++)
		{
		   if((strFile=strCopyFiles.GetField(',',i))==_T(""))
			   break;
		   strPutFile=m_strNtSpoolDir+strFile;
		   	if( _stat(strFile, &buf ))
				if(!(GetNTDriver(strPutFile,strFile)))
			return FALSE;
		}
	}

	return TRUE;
	   
}


/////////////////////////////////////////////////////////////////////////////
// GetNTDriver 
// __________


BOOL CdllSpoolPrintManager::GetNTDriver(CString FileSpec,CString Driver)
{
	
	CWaitCursor wait;	// display wait cursor


	CInriFtpMan ftp_man;
	if(!ftp_man.Initialize()) 
		return FALSE;
	Driver.MakeUpper();
	CString ServerFileName=_T("/h/NTCSSS/spool/ntdrivers/")+Driver;

	
	if(!ftp_man.GetFile(gbl_dll_man.dllServerName(),
						ServerFileName,
						FileSpec,
						FALSE,
						0666)) 
		return FALSE;
	else 
		return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// OnSetprinter 
// __________

BOOL CdllSpoolPrintManager::OnSetprinter(const NtcssPrintAttrib *ptr_attrib,
										 BOOL local_printer) 
{
	HANDLE hPrinter = NULL; 
	DWORD dwNeeded = 0; PRINTER_INFO_2 *pi2 = NULL; 
	PRINTER_DEFAULTS pd;  
	char pname[256];

	CWaitCursor wait;	// display wait cursor

	
	if(local_printer)
	{

		//Get current printer 
		::GetProfileString(_T("windows"),_T("device"), 
                           _T(""), pname, 
                           sizeof(pname)); 

		*(strchr(pname,','))=NULL; //just want the name
	
	}
	else
		strcpy(pname,"NTCSS");

  //AfxMessageBox("pname-> " + (CString)pname);
  ::ZeroMemory(&pd, sizeof(pd));
   pd.DesiredAccess = PRINTER_ALL_ACCESS;

   if (::OpenPrinter(pname, &hPrinter, &pd))
   {
	   ::GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
	
		if (dwNeeded == 0) 
		  return FALSE;

		char *buff=new char[dwNeeded];

		pi2=(PRINTER_INFO_2*)buff;
	   
		BOOL ok=::GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded);

		if(ok)
		{
			pi2->pDevMode->dmOrientation=ptr_attrib->orient;	//set to landscape
			pi2->pDevMode->dmCopies=ptr_attrib->copies;			//set # of copies
			pi2->pDevMode->dmPaperSize=ptr_attrib->paper_size;  //set papersize
	
			ok=::SetPrinter(hPrinter, 2, (LPBYTE)pi2, 0);
		}

		delete [] buff;

		if (hPrinter != NULL)
			::ClosePrinter(hPrinter);

		return ok;
   }
   else
	   return FALSE;
	
}

BOOL CdllSpoolPrintManager::NtcssAddMonitor(CString strMonitorName,CString strMonitorDll)
{
	//NOTE DLL must be WINNT/SYSTEM32

	MONITOR_INFO_2 mi;

	::ZeroMemory(&mi, sizeof(mi));

	mi.pName=(char*)(LPCTSTR)strMonitorName;
	mi.pEnvironment=0;
	mi.pDLLName=(char*)(LPCTSTR)strMonitorDll;
	if((::AddMonitor(0,2,(LPBYTE)&mi)))
		return TRUE;
	else
	    return (::GetLastError()==3006); //already loaded is OK
}

CString CdllSpoolPrintManager::GetSysError(DWORD dError)
{
	
		
	LPTSTR lpBuffer;
	CString strError;

	::FormatMessage( 
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	dError,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	(LPTSTR) &lpBuffer,
	0,
	NULL
  );
	  
  strError=lpBuffer;
  ::LocalFree(lpBuffer);
  return (strError.IsEmpty() ? _T("Unknown error") : strError);
}
