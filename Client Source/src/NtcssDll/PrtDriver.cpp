/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrtDriver.cpp
//                                         
// This is the implementation file for the CprintDriver.
//
//----------------------------------------------------------------------
 
#include "stdafx.h"
#include "fstream.h"
#include "winspool.h"
#include "0_basics.h"
#include "PrtDriver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCprintDriver constructor and message mapping
// _________________
             
CprintDriver::CprintDriver(Csession *session)
{
	m_pSock = new CNtcssSock(session);
    m_pSession = session;
}

BEGIN_MESSAGE_MAP(CprintDriver, CWnd)
    //{{AFX_MSG_MAP(CprintDriver)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()   

  


/////////////////////////////////////////////////////////////////////////////
// cookFileLocally
// _______________

BOOL CprintDriver::cookFileLocally(const SlocalPrinterInfo *local_printer_ptr,
                                   const SremotePrinterInfo *remote_printer_ptr,
                                   const char *file_name,
                                   BOOL spool_to_file,
                                   const char *spool_file_name)
{

                                       // First open file, if have
                                       //  problems then bail out

    ifstream infile;

    infile.open(file_name,ios::in | ios::nocreate);


    if (!infile.is_open())
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Unable to open file %s",file_name);
        m_pSession->ssnErrorMan()->RecordError(err_buf);
        return FALSE;
    }
    char output_device[SIZE_SMALL_GENBUF];
    const char *driver_name;
    const char *device_name;

    if (local_printer_ptr)
    {
		device_name = local_printer_ptr->printer_device_name_buf;
        driver_name = local_printer_ptr->printer_driver_name_buf;
		strcpy(output_device, local_printer_ptr->output_device_buf);
	}
    else
    {
        device_name = remote_printer_ptr->printer_device_name_buf;
        driver_name = remote_printer_ptr->printer_driver_name_buf;
        output_device[0] = '\0'; 
    }


    HDC hdc = CreateDC("WINSPOOL", device_name,       
                       NULL, NULL);





    BOOL ok = (hdc != NULL);
    CDC *pDC = NULL;

    if (ok)
    {
        pDC = new CDC;
        ok = pDC->Attach (hdc);
    }


    DOCINFO doc_info_rec;
    if (ok)
    {
        doc_info_rec.cbSize = sizeof(DOCINFO);
        doc_info_rec.lpszDocName = NTCSSVAL_LPR_PRINT_DOCNAME;

        if (spool_to_file)

            doc_info_rec.lpszOutput = spool_file_name;

        else
            doc_info_rec.lpszOutput = NULL;

                                       // Not sure about these 2,
                                       //  help was sparse, are
                                       //  for Windows 95 only


        ok = (pDC->StartDoc(&doc_info_rec) > 0);
    }

    int cxPage, cyPage;
    if (ok)
    {
        ok = pDC->SetMapMode (MM_HIMETRIC) ;

        pDC->SetTextAlign (TA_LEFT | TA_TOP | TA_NOUPDATECP) ;

        cxPage = pDC->GetDeviceCaps (HORZSIZE) ;
        cyPage = pDC->GetDeviceCaps (VERTSIZE) ;
   }

//AfxMessageBox("After GetDeviceCaps");
    TEXTMETRIC tm;
    if (ok)
        ok = pDC->GetTextMetrics(&tm);

    int lines_per_page;
    int chars_per_row;
    int start_x_pos;
    int start_y_pos;
    int line_on;
    int y_pos;
    if (ok)
    {
        lines_per_page = (int) (( cyPage/((double)tm.tmHeight/100)) - 2);

        chars_per_row = (int) (( cxPage/((double)tm.tmMaxCharWidth/100)) - 2);

                                       // NEEDTODO - not happy with this
                                       //  setting but works ok for now,
                                       //  old 16-bit setting commented out
//        start_x_pos = 10; 
//        start_y_pos = 10; 
		start_x_pos = tm.tmMaxCharWidth;
//		start_y_pos = tm.tmHeight;
		start_y_pos = 0;

        line_on = 0;
        y_pos = start_y_pos;
    }

    if (ok)
        ok = (pDC->StartPage() > 0);          

    if (ok)
    {
        int len;
        char read_buffer[SIZE_STANDARD_GENBUF];

//AfxMessageBox("Going into loop");
        while ((ok) &&
               (infile.getline(read_buffer,SIZE_STANDARD_GENBUF,'\n')))
        {
            len = strlen(read_buffer);
        
            if (len > chars_per_row)

                read_buffer[chars_per_row] = '\0';    

            line_on++;
            if (line_on > lines_per_page)
            {
                line_on = 1;
                y_pos = start_y_pos;
                pDC->EndPage();           
            }
            ok = pDC->TextOut(start_x_pos, y_pos, read_buffer);

                                       // NEEDTODO - not happy with this
                                       //  setting but works ok for now
                                       //  old method commented out

            y_pos -= tm.tmHeight;
        }

    }

    infile.close();

    if (ok)
    {                                   
                                       // NEEDTODO - look into: EndPage
                                       //   returns err code but if
                                       //   ignore seems to do ok?

        pDC->EndPage();        

        ok = (pDC->EndDoc() > 0);             

    }

    if (pDC != NULL)
    {
        pDC->Detach();            

        delete pDC;
    }

    if (hdc != NULL)
        DeleteDC(hdc);

    if (ok)
        return(TRUE);

    else
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Client file printing error");
        m_pSession->ssnErrorMan()->RecordError(err_buf);

        return(FALSE);
    }
}



/////////////////////////////////////////////////////////////////////////////
// CprintDriver::sendRawLocalLPRjob
// ________________________________

BOOL CprintDriver::sendRawLocalLPRjob
                        (const SlocalPrinterInfo *local_printer,
                         const char *file_name)
{
    CFile data_file;

    if (!data_file.Open(file_name, CFile::modeRead)) 
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Unable to open file %s for printing",
                file_name);
        m_pSession->ssnErrorMan()->RecordError(err_buf);
        return(FALSE);
    }

    HANDLE hPrinter = NULL;

    char tmp_printer_name[SIZE_SMALL_GENBUF];
    strcpy(tmp_printer_name, local_printer->printer_device_name_buf);

    BOOL  ok = OpenPrinter(tmp_printer_name, 
                           &hPrinter, NULL);
    if (!ok)
    {
        ok = FALSE;
    }

    if (ok)
    {     
        DOC_INFO_1 doc_info1;

        doc_info1.pDocName = NTCSSVAL_LPR_LOCALPRT_DOC_NAME;
        doc_info1.pOutputFile = NULL;
        doc_info1.pDatatype = "RAW";

        ok = (StartDocPrinter(hPrinter,1,(unsigned char *)&doc_info1) != 0);

        if (!ok) 
        {
            ClosePrinter(hPrinter);
        }
    }

    if (ok)
    {
        ok = StartPagePrinter(hPrinter);

        if (!ok)
        {
            EndDocPrinter(hPrinter);
            ClosePrinter(hPrinter);
        }
    }

    char  data_buffer[SIZE_BIG_GENBUF];
    unsigned long   rd_cnt;
    unsigned long   wr_cnt;

    if (ok)
    {
        while ( (ok) && 
                (rd_cnt = data_file.Read(data_buffer,
                                         sizeof(data_buffer))) > 0) 
        {
            
            ok = WritePrinter(hPrinter,data_buffer,rd_cnt, &wr_cnt);

            if (ok)
                ok = (rd_cnt == wr_cnt);

        }
        if (!ok)
        {
            EndPagePrinter(hPrinter);
            EndDocPrinter(hPrinter);
            ClosePrinter(hPrinter);
        }
    }

    if (ok)
        ok = EndPagePrinter(hPrinter);

    if (ok)
        ok = EndDocPrinter(hPrinter);

    ClosePrinter(hPrinter);

    if (!ok)
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Error in output of file %s to printer %s",
                file_name,
                local_printer->printer_device_name_buf);

        m_pSession->ssnErrorMan()->RecordError(err_buf);
    }

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// localLprSendLeadControls
// ________________________

BOOL CprintDriver::localLprSendLeadControls
                        (const char *printer_name, const char *host_name, 
                         long file_len, int seq_num) 
{
    char cmd_line[NTCSSVAL_LPR_CMDFILE_LINE_LEN];

                                       // Note that on server side will
                                       //  allow printing to any UNIX
                                       //  printer if leave off the
                                       //  prefix
    sprintf(cmd_line, "\002%s%s\012",
            NTCSSVAL_LPR_PRINT_PREFIX_STR, printer_name);

    BOOL ok = localLprSendCommmandLines(cmd_line, 1);

    if (ok)
    {
        sprintf(cmd_line, "\003%ld dfA%03d%s\012\000",
                file_len, seq_num, host_name);

        ok = localLprSendCommmandLines(cmd_line, 1);

    }

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// localLprSendTrailControls
// ________________________

BOOL CprintDriver::localLprSendTrailControls 
                  (const char *host_name, const char *file_name, 
                   long file_len, int seq_num, SecurityLevel seclvl,
				   const int nCopies,const BOOL bBanner) 
{
    char multi_cmd_lines[NTCSSVAL_LPR_CMDFILE_NUM_LINES]
                        [NTCSSVAL_LPR_CMDFILE_LINE_LEN];

    int multi_cmd_line_bytes = 0;
    int multi_cmd_lines_idx = 0;
                                       // Get ptr to user info, need
                                       //  it below
    const SuserInfoRec_wFullData *user = 
                        gbl_dll_man.dllUser()->userInfo();

                                       // Now start putting cmd lines
                                       //  into buffer
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"H%s\012\000",
        host_name);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"P%.8s\012\000",
                user->login_name_buf);

    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"J%s\012\000",
                m_pSession->ssnAppName());

    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"C%s\012\000",
                LPR_SECLVL_NAMES[seclvl]);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"L%.8s\012\000",
                user->login_name_buf);
     if(bBanner)
	 {
		
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"B\012\000");
	 }
	//jj banner B or nothing whole line
    CString temp;
	temp.Format("K%d\012\000",nCopies);
	multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],/*"K3\012\000"*/(LPCTSTR)temp);
	//K# is # of copies
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"O\012\000");
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],
                "FdfA%0.3d%s\012\000", seq_num, host_name);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],
                "fdfA%0.3d%s\012\000", seq_num, host_name);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],
                "UdfA%0.3d%s\012\000", seq_num, host_name);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"N%s\012\000",
                file_name);
            
    multi_cmd_line_bytes +=     
        sprintf(multi_cmd_lines[multi_cmd_lines_idx++],"A0\012\000",
                file_name);
                                       // This single cmd line goes 1st,
                                       //  but needed total length of
                                       //  stuff in multi_cmd_lines
                                       //
    char one_cmd_line[NTCSSVAL_LPR_CMDFILE_LINE_LEN];

    sprintf(one_cmd_line,"\002%d cfA%0.3d%s\012",
            multi_cmd_line_bytes,seq_num,host_name);

                                       // Send single cmd line
    BOOL ok = localLprSendCommmandLines
                            (one_cmd_line, 1);

    if (ok)                            // Now send multi cmd lines
        ok = localLprSendCommmandLines((char *)multi_cmd_lines, 
                                       multi_cmd_lines_idx);

    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// localLprSendCommmandLines
// _________________________

// Note that if have more than one line in cmd_buf_ptr then each must
//  start on NTCSSVAL_LPR_CMDFILE_LINE_LEN boundary

BOOL CprintDriver::localLprSendCommmandLines (const char *cmd_buf_ptr, 
                                             const int num_cmd_lines)
{   
    char data_buffer[SIZE_TRANSPORT_GENBUF];

    if ((cmd_buf_ptr == NULL) || (num_cmd_lines < 1))
        return(FALSE);

    const char *send_data_src;
    int  len = 0;

    if (num_cmd_lines == 1)
    {
        len = strlen(cmd_buf_ptr);
        send_data_src = cmd_buf_ptr;
    }
    else
    {
        send_data_src = data_buffer;
        const char *src_ptr = cmd_buf_ptr;
        char *trgt_ptr = data_buffer;
        int ch_cnt; 

        for (int i=0; i < num_cmd_lines; i++) 
        {
            ch_cnt = strlen(src_ptr);

            strncpy(trgt_ptr,src_ptr,ch_cnt);
            src_ptr += NTCSSVAL_LPR_CMDFILE_LINE_LEN;
            trgt_ptr += ch_cnt;
            len += ch_cnt;

        }
        *trgt_ptr = '\0';
        len++;
    }

    BOOL ok = TRUE;
    ok = (m_pSock->Send(send_data_src, len, 0) 
          != SOCKET_ERROR);

    if (ok)
        ok = (m_pSock->Recv(data_buffer, 1, 0) 
              != SOCKET_ERROR);

    if (ok)
        ok = (data_buffer[0] == NTCSSVAL_LPR_ACK);

    if (!ok)       
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        strcpy(err_buf,"Print Queue Disabled");
        m_pSession->ssnErrorMan()->RecordError(err_buf,TRUE);
        return FALSE;
    }

   return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// sendDataFile
// ____________

BOOL CprintDriver::localLprSendDataFile(const char *file_name, long file_len)
{
    CFile data_file;

    if (!data_file.Open(file_name,CFile::modeRead)) 
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Unable to open file:\n\t%s",file_name);
        m_pSession->ssnErrorMan()->RecordError(err_buf);
        return FALSE;
    }

    char  data_buffer[SIZE_TRANSPORT_GENBUF];
    int   cnt;
                                       // Loop reads a block at a time
                                       //  and sends it out the socket
                                       //
    BOOL ok = TRUE;
    int  total_bytes_sent = 0;

    while ( (ok) && 
            (cnt=data_file.Read(data_buffer,SIZE_TRANSPORT_GENBUF)) > 0) 
    {
        if (m_pSock->Send(data_buffer, cnt, 0) 
            == SOCKET_ERROR)
        {
            ok = FALSE;
        }
        total_bytes_sent += cnt;
    }
                                       // Now send NULL to tell done &
                                       //   check that get ack ok
    data_buffer[0] = '\0';
    
    if (ok)       
        ok = (m_pSock->Send(data_buffer, 1, 0)
              != SOCKET_ERROR);

    if (ok)       
        ok = (m_pSock->Recv(data_buffer, 1, 0)
              != SOCKET_ERROR);
    if (ok)
        ok = (data_buffer[0] == NTCSSVAL_LPR_ACK);

    if (ok)
        ok = (total_bytes_sent == file_len);


    if (ok)       
        return TRUE;
    else
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        strcpy(err_buf,"Client print file socket error");
        m_pSession->ssnErrorMan()->RecordError(err_buf,TRUE);
        return FALSE;
    }
}



/////////////////////////////////////////////////////////////////////////////
// sendCookedLocalLPRjob
// _____________________

BOOL CprintDriver::sendCookedLocalLPRjob
                        (const SlocalPrinterInfo *local_printer,
                         const char *file_name)
{
    return(cookFileLocally(local_printer, NULL, 
                           file_name, FALSE, NULL));
}




/////////////////////////////////////////////////////////////////////////////
// sendCookedRemoteLPRjob
// ______________________

BOOL CprintDriver::sendCookedRemoteLPRjob 
                             (const SremotePrinterInfo *remote_printer,
                              const char *file_name,
                              SecurityLevel seclvl)
{
    char spool_file_name[SIZE_STANDARD_GENBUF];

    makeSpoolFileName(spool_file_name);

    BOOL ok = cookFileLocally(NULL, remote_printer, 
                              file_name, TRUE, spool_file_name);

    if (ok)
        sendRawRemoteLPRjob(remote_printer, spool_file_name, seclvl);
                               
    return(ok);
}



/////////////////////////////////////////////////////////////////////////////
// sendRawRemoteLPRjob
// ___________________

BOOL CprintDriver::sendRawRemoteLPRjob 
                             (const SremotePrinterInfo *remote_printer,
                              const char *file_name,
                              SecurityLevel seclvl)
{

	char host_name[SIZE_SMALL_GENBUF];

	strcpy(host_name,((CNtcssDll*)AfxGetApp())->GetLocalHostName());

    char *cptr;
    if ((cptr = strchr(host_name,'.')))
         *cptr='\0';

    int seq_num = updatedNtcssIniNumValue
                                (NTCSSINICODE_LPR_SECTION_CODE,
                                 NTCSSINICODE_SEQNO_CODE,
                                 NTCSSVAL_MAX_LPR_SEQ_NUM);



    CFileStatus status;
	
	::Sleep(2000);
	
	if (CFile::GetStatus(file_name, status ) == 0) 
	 return FALSE;

    

    long file_len = status.m_size;

    if (*host_name == '\0')
        strcpy(host_name, gbl_dll_man.dllServerName());



    
 

    if (!m_pSock->ConnectHost(remote_printer->host_name_buf, 
                                             NTCSS_PORT_LPR))
    {
        char err_buf[MAX_ERROR_MSG_LEN];
        sprintf(err_buf,"Connect host error for:\n\t%s",remote_printer->host_name_buf);
        m_pSession->ssnErrorMan()->RecordError(err_buf);
 

        return FALSE;
    }  





    BOOL ok = localLprSendLeadControls(remote_printer->printer_name_buf, 
                                       host_name, file_len, seq_num);

    if (ok)
        localLprSendDataFile(file_name, file_len);
        

    if (ok)

        ok=localLprSendTrailControls(host_name, file_name, file_len, 
                                     seq_num,  seclvl, remote_printer->nCopies, 
									 remote_printer->bBanner); 

      
                                      // Restore cursor and
                                      //ReleaseCapture
  
    return(ok);
}



	
