/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ...\comn_src\ntcssdll\prtsvrfi.cpp
//                                         
//----------------------------------------------------------------------


//
// NEEDTODO
// - Need to get the "List Files of Type" box implemented
// - Need to implement using default printer if none selected
// - Need to get security level to CPrtSvrFile constructor

#include "stdafx.h"
#include "0_basics.h"

#include "iostream.h"
#include "fstream.h"
#include "PrtSFileDlg.h"
#include "PrtSelectDlg.h"
#include "dllutils.h"
#include "dll_msgs.h"
#include "StringEx.h"
#include "AtlassFileDlg.h"
#include "inri_ftp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPrtSvrFile constructor
// _______________________
//
// For now making all UNCLASSIFIED, need to fix this

CPrtSvrFile::CPrtSvrFile(CWnd* pParent, Csession *session,
                         const char *server_name, 
                         const char *directory,
                         const char *output_type,
						 const char *copies,
						 const char *options)
    : CDialog(CPrtSvrFile::IDD, pParent)
{
                                       // Inits
    //{{AFX_DATA_INIT(CPrtSvrFile)
    m_strDirectory = "";
    m_strFileName = "";
    m_strSelectedFile = "";
    m_strPrinter = "";
    m_strFileType = "*.*";
    m_strSendPrinter = "";
    //}}AFX_DATA_INIT

	m_pSession      = session;
    m_strDirectory  = directory;
    m_strServerName    = server_name;
    m_StrOutputType   = output_type;
	m_strCopies = copies;
	m_strOptions = options;

	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM;
	listItem.iSubItem=0;


    m_bPrinterIsPicked = FALSE;

    if (m_pSession->isRemotePrinterInfoSet())
    {
        m_strSendPrinter = m_pSession->remotePrinterInfo()->printer_name_buf;
        m_strSendPrinter += "@";
       // m_strSendPrinter += m_pSession->remotePrinterInfo()->host_name_buf;
		m_strSendPrinter +=gbl_dll_man.dllUser()->GetAuthServer(m_pSession->ssnAppName());
		m_strSendPrinter += " in ";
        m_strSendPrinter += m_pSession->remotePrinterInfo()->printer_loc_buf;
    }

 
}



/////////////////////////////////////////////////////////////////////////////
// DoDataExchange and message mapping
// ______________

void CPrtSvrFile::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPrtSvrFile)
	DDX_Control(pDX, IDC_LISTCTRL, m_ListCtrl);
	DDX_Control(pDX, IDC_FILECOMBO, m_FileTypeCtrl);
    DDX_Text(pDX, IDC_DIREDIT, m_strDirectory);
    DDX_Text(pDX, IDC_FILEEDIT, m_strFileName);
    DDX_Text(pDX, IDC_PRINTEREDIT, m_strPrinter);
    DDX_CBString(pDX, IDC_FILECOMBO, m_strFileType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrtSvrFile, CDialog)
    //{{AFX_MSG_MAP(CPrtSvrFile)
    ON_BN_CLICKED(IDSELECT, OnSelect)
    ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_FILECOMBO, OnSelchangeFilecombo)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkListctrl)
	ON_NOTIFY(NM_CLICK, IDC_LISTCTRL, OnClickListctrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// getUnixFiles
// ____________

BOOL CPrtSvrFile::getUnixFiles(MyListCtrl* list_box_ptr)
{

                                       // Setup and send msg to server
                                       //
    const char *server_name = (LPCTSTR)m_strServerName;

    const char *directory = (LPCTSTR)m_strDirectory;

                                       // Load and do msg
	CmsgGETDIRLISTING msg;

	
	BOOL ok = msg.Load(server_name, directory);

	if (ok)
		ok = msg.DoItNow();

    int dir_filelist_cnt;
    if (ok)
        ok = msg.getFileCount(&dir_filelist_cnt);

                                       // Bail out now if have problems
    if (!ok)
    {
		m_pSession->ssnErrorMan()->RecordError(msg.GetLastErrorMsgText());
        return(FALSE);
    }
                                       // Else if no files then give our
                                       //  own msg and bail
    else if (dir_filelist_cnt == 0)
    {
        char err_msg[MAX_ERROR_MSG_LEN];
        sprintf(err_msg,"No files for host %s directory %s",
                server_name,directory);
        m_pSession->ssnErrorMan()->RecordError(err_msg);
        return(FALSE);
    }
                                       // Now get buffer to receive
                                       //   files, bail on mem error
                                       //
    int file_list_size = (dir_filelist_cnt * sizeof(SUnixDirInfo));
    //char *file_list_ptr; TR10166 put m_ prefix on it

    m_file_list_ptr = new SUnixDirInfo[file_list_size]; //TR10166
    if (m_file_list_ptr == NULL)
    {
        m_pSession->ssnErrorMan()->RecordError("Memory allocation error");
        return(FALSE);
    }
     
      
	ok = msg.UnLoad(m_file_list_ptr,file_list_size,&m_num_files_got);

                                       // Bail if can't unload files
    if (!ok)
    {
		m_pSession->ssnErrorMan()->RecordError(msg.GetLastErrorMsgText());
        return(FALSE);
    }
                                       // If not enuf space then give
                                       //  warning but continue
                                       //
    if (m_num_files_got != dir_filelist_cnt)
    {
        char err_msg[MAX_ERROR_MSG_LEN];
        sprintf(err_msg,
                "List box unable to display all files for host %s directory %s",
                server_name,directory);
        m_pSession->ssnErrorMan()->RecordError(err_msg);
//        m_pSession->ssnErrorMan()->HandleError
//                            (PRINT_SERVER_FILE_FACILITY_STR,
//                             NULL, "Warning");
    }
                                       // Put files in list box and
                                       //  have 1st file as default
                                       //
   SUnixDirInfo *ptr = m_file_list_ptr;       //TR10166
   m_strFileName = ptr->FileName; 
   CString temp;
   

    for (int i=0; i < m_num_files_got; i++)
    {
    	m_ListCtrl.Write(ptr->FileName);
		temp=ptr->FileDate;
		temp.Format("%s/%s/%s",temp.Left(2),temp.Mid(2,2),temp.Mid(4,2));
		m_ListCtrl.Write(temp);
		temp.Format("%10d",ptr->FileSize);
		m_ListCtrl.Write(temp);
		m_ListCtrl.Write(ptr->FileOwner);
		ptr++;
    }
                                       
          
    return(TRUE);
}







/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
// ______________

BOOL CPrtSvrFile::OnInitDialog()
{
    

	CDialog::OnInitDialog();

	 //this was in the constructor which caused an assert on GetParent()

	 m_dlgSelPrinter = new CSelPrtDialog(GetParent(), m_pSession, FALSE);

     m_dlgSelPrinter->enableRemoteSelection
                              (m_StrOutputType, NTCSS_UNCLASSIFIED,
                               FALSE,  // all printers, not just default
                               FALSE); // but not general access printers 

	CString str_Columns="NAME,MODIFIED,SIZE,OWNER";
	
	m_ListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\DLL\\PRINTSVRFILE");


    
    centerScreen(m_hWnd);

                                       // No need to go on if can't get
                                       //   files
                                       //
   
    if (!getUnixFiles(&m_ListCtrl))
	{
		::MessageBeep(MB_ICONSTOP);
		AfxMessageBox("There are no files in this directory",MB_ICONSTOP);
		gbl_dll_man.CMcall_releaseSession(m_pSession); //debug for TR10243
		CDialog::EndDialog(IDCANCEL);  //debug for TR10243

        //return(FALSE); was only statement
	}



    if (m_pSession->isRemotePrinterInfoSet())
    {
        const SremotePrinterInfo* tmp_prt  = m_pSession->remotePrinterInfo();
        m_strSendPrinter = tmp_prt->printer_name_buf;
        m_strSendPrinter += "@";
        //m_strSendPrinter += tmp_prt->host_name_buf;
		 m_strSendPrinter +=gbl_dll_man.dllUser()->GetAuthServer(m_pSession->ssnAppName());
        m_strSendPrinter += " in ";
        m_strSendPrinter += tmp_prt->printer_loc_buf;
        m_strPrinter = m_strSendPrinter;
        UpdateData(FALSE);
    }

	//TR10166
	//m_FileTypeCtrl.SelectString( -1,"*.*" );
	//TR10166

   
    
    return TRUE;  
}



/////////////////////////////////////////////////////////////////////////////
// OnOK
// ____

void CPrtSvrFile::OnOK()
{
    UpdateData(TRUE);
    
    if (m_strFileName.IsEmpty())
    {
        AfxMessageBox ("Please enter file name",MB_OK);
        return;
    }
    if (m_strPrinter.IsEmpty())
    {
        AfxMessageBox ("Please select printer",MB_OK);
        return;
    }

    const SremotePrinterInfo* tmp_prt;
    BOOL ok = TRUE;

    if (m_bPrinterIsPicked)
    {
        tmp_prt = m_dlgSelPrinter->getRemotePrinterSelection();

        if ((tmp_prt == NULL) ||
            (!m_pSession->setRemotePrinterInfo(tmp_prt, TRUE)))
        {
            ok = FALSE;
            m_pSession->ssnErrorMan()->RecordError
                      ("Error, unable to save printer selection");
        }
    }


    if ((ok) && (spoolUnixPrintJob()))

        AfxMessageBox ("File successfully spooled to server",MB_OK);

    else
    {
// Need to check out if spoolUnixPrintJob is okay
//        m_pSession->ssnErrorMan()->HandleError
//                            (PRINT_SERVER_FILE_FACILITY_STR,
//                             NULL, NULL);
    }

    CDialog::OnOK();
}




/////////////////////////////////////////////////////////////////////////////
// OnSelect
// ________
//
// Get here to select printer - use m_dlgSelPrinter(CSelPrtDialog) for 
// this
void CPrtSvrFile::OnSelect()
{
    if (m_dlgSelPrinter->DoModal() == IDOK)
    {
        if (m_dlgSelPrinter->selectedRemotePrinter())
        {
            m_bPrinterIsPicked = TRUE;

            const SremotePrinterInfo *tmp_prt;
            tmp_prt = m_dlgSelPrinter->getRemotePrinterSelection();

            m_strSendPrinter = tmp_prt->printer_name_buf;
            m_strSendPrinter += "@";
            //m_strSendPrinter += tmp_prt->host_name_buf;
			m_strSendPrinter += gbl_dll_man.dllUser()->GetAuthServer(m_pSession->ssnAppName());
            m_strSendPrinter += " in ";
            m_strSendPrinter += tmp_prt->printer_loc_buf;
            m_strPrinter = m_strSendPrinter;
        }
    }


    UpdateData(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// SpoolUnixPrintJob
// _________________
//
// For now making all UNCLASSIFIED, need to fix this

BOOL CPrtSvrFile::spoolUnixPrintJob() 
{
	// Setup for CmsgPRINTSVRFILE msg
                                       //
    char  fullpathname[SIZE_STANDARD_GENBUF];

    strcpy(fullpathname,(LPCTSTR)m_strDirectory);
    strcat(fullpathname,"/");
    strcat(fullpathname,(LPCTSTR)m_strFileName);

    const SremotePrinterInfo *tmp_prt;
    tmp_prt = m_pSession->remotePrinterInfo();

    if (tmp_prt == NULL)
    {
        m_pSession->ssnErrorMan()->RecordError("Printer not selected");
        return(FALSE);
    }
///////////////////////ATLASS STUFF/////////////////////////////////////////////////////////
//got to do a get & put to do this with our ftp
////////////////////////////////////////////////////////////////////////////////////////////
	
	if(!strcmp(tmp_prt->AtlassFileName,"***"))  
	{
		BOOL ok=TRUE;
		CAtlassFileDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			try
			{
				CInriFtpMan ftp_man;
				CString strTempFile;
				makeSpoolFileName(strTempFile.GetBuffer(_MAX_PATH));
				strTempFile.ReleaseBuffer();
			
				if (!ftp_man.Initialize())
					throw _T("Ftp init failure");

				if(!ftp_man.GetFile(gbl_dll_man.dllServerName(),
								 fullpathname,
								 (LPCTSTR)strTempFile,
								 FALSE,0666))

					throw _T("PSF FTP Get failure");

				if (!ftp_man.Initialize())
					throw _T("Ftp init failure");

				ftp_man.SetUnixID(0);

				if(!ftp_man.PutFile(gbl_dll_man.dllServerName(),
								 (LPCTSTR)dlg.m_strAtlassFname,
								 (LPCTSTR)strTempFile,
								 FALSE,0666,dlg.m_nAppend)) //defaulting to binary

					throw _T("PSF FTP Put failure");

				_unlink((LPCTSTR)strTempFile);
			
				//	AfxMessageBox ("File successfully spooled to remote printer",MB_OK);
				return TRUE;
			}
									
				catch (LPTSTR pstr)
				{
					m_pSession->ssnErrorMan()->RecordError(pstr);
					ok=FALSE;
				}

		}
		else
		{
			m_pSession->ssnErrorMan()->RecordError("No Atlass File Name specified");
			ok=FALSE;
		}
		if(!ok)
			m_pSession->ssnErrorMan()->HandleError(gbl_dll_man.dllFacilityName());

		gbl_dll_man.CMcall_releaseSession(m_pSession);

		return(FALSE);
	}

/////////////////////////////////////////////////////////////////////////////////////////////

    const char *the_progroup_title = (LPCTSTR)m_pSession->ssnAppName();

                                       // Load and do msg
	CmsgPRINTSVRFILE msg;
	//TODO HostName isn't being used in message @ all message type is authserver now

	try
	{
		if(!msg.Load(tmp_prt->host_name_buf, fullpathname, 
						   tmp_prt->printer_name_buf,
						   the_progroup_title, NTCSS_UNCLASSIFIED,m_strCopies.
						   GetBufferSetLength(NTCSS_MAX_PRT_COPIES_LEN),
						   m_strOptions.GetBufferSetLength(NTCSS_MAX_PRT_OPTIONS_LEN)))
			throw;

		if (!msg.DoItNow())
			throw;

		return TRUE;
	}
	catch(...)
	{
        m_pSession->ssnErrorMan()->RecordError(msg.GetLastErrorMsgText());
		return(FALSE);
	}
}




void CPrtSvrFile::OnDestroy() //TR10166 added to save buffer
                              //so it can be reused for filter operations
							 
{
	CDialog::OnDestroy();
	
	delete [] m_file_list_ptr; 
//	delete m_dlgSelPrinter;   
}


void CPrtSvrFile::OnSelchangeFilecombo() 
//TR10166 provide very simple filtering
{
	UpdateData();
	m_ListCtrl.DeleteAllItems();
	//m_ListCtrl.Clear();

	CString str_Columns="NAME,MODIFIED,SIZE,OWNER";
	
	//m_ListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
	//	             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\DLL\\PRINTSVRFILE");


	SUnixDirInfo* ptr = m_file_list_ptr;       
    //m_strFileName = ptr->FileName; 
	m_strFileName=_T("");
	CString scratch,temp2,temp=m_strFileType.Mid(m_strFileType.Find('.'));
		
    for (int i=0; i < m_num_files_got; i++)
    {

		scratch=ptr->FileName;
		scratch=scratch.Mid(scratch.Find('.'));

		if(m_strFileType=="*.*" || ! scratch.Compare(temp))
		{
			m_ListCtrl.Write(ptr->FileName);
			temp2=ptr->FileDate;
			temp2.Format("%s/%s/%s",temp2.Left(2),temp2.Mid(2,2),temp2.Mid(4,2));
			m_ListCtrl.Write(temp2);
			temp2.Format("%10d",ptr->FileSize);
			m_ListCtrl.Write(temp2);
			m_ListCtrl.Write(ptr->FileOwner);
		}
		ptr++;          
        
    }

	
}

/////////////////////////////////////////////////////////////////////////////
// OnDblclkListctrl
// ________________

void CPrtSvrFile::OnDblclkListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_strFileName=m_ListCtrl.GetText();
	if(!m_strFileName.IsEmpty())
		UpdateData(FALSE);

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// OnClkListctrl
// ________________

void CPrtSvrFile::OnClickListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_strFileName=m_ListCtrl.GetText();
	if(!m_strFileName.IsEmpty())
		UpdateData(FALSE);
	
	*pResult = 0;
}
