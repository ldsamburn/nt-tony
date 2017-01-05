/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ViewSFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ntcssdll.h"
#include "ViewSFileDlg.h"
#include "dlluser.h"
#include "dll_msgs.h"
#include "DllMan.h"
#include "ViewServerFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewSFileDlg dialog


CViewSFileDlg::CViewSFileDlg(CWnd* pParent /*=NULL*/,
							 Csession *session,CString ServerName,CString Directory)
	: CDialog(CViewSFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewSFileDlg)
	m_SelectedFileName = _T("");
	m_DirName = _T("");
	//}}AFX_DATA_INIT
	m_strDirectory = Directory;
	m_DirName = Directory;
    m_strServerName = ServerName;
	m_pSession = session;
	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM;
	listItem.iSubItem=0;

}


void CViewSFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewSFileDlg)
	DDX_Control(pDX, IDC_LISTCTRL, m_ListCtrl);
	DDX_Text(pDX, IDC_FILENAME, m_SelectedFileName);
	DDX_Text(pDX, IDC_DIRNAME, m_DirName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewSFileDlg, CDialog)
	//{{AFX_MSG_MAP(CViewSFileDlg)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL, OnItemchangedListctrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkListctrl)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewSFileDlg message handlers




// CViewSFileDlg implementation specific

/////////////////////////////////////////////////////////////////////////////
// getUnixFiles
// ____________

BOOL CViewSFileDlg::getUnixFiles(MyListCtrl* list_box_ptr)
{


	SUnixDirInfo *file_list_ptr=NULL;


	try
	{

	                                       
		CmsgGETDIRLISTING msg;
		

		int dir_filelist_cnt;

		if(!(msg.Load(m_strServerName,m_strDirectory)&&msg.DoItNow()))
			throw(_T("Bad Path Specified"));

		if(!msg.getFileCount(&dir_filelist_cnt))
			throw(_T("Could not get File Info"));

		if (dir_filelist_cnt == 0)
		{
			TCHAR temp[_MAX_PATH];
			sprintf(temp,"No files for host %s directory %s",m_strServerName,m_strDirectory);
			throw(temp);
		}


                                      
                                       // Now get buffer to receive
                                       //   files, bail on mem error
                                       //
		int file_list_size = (dir_filelist_cnt * sizeof(SUnixDirInfo));
	

		file_list_ptr = new SUnixDirInfo[file_list_size];
	
     
		int num_files_got;   
		if(!msg.UnLoad(file_list_ptr,file_list_size,&num_files_got))
			throw(_T("Could not Unbundle Files"));

      
                                       //  warning but continue
                                       //
		if (num_files_got != dir_filelist_cnt)
		{
			char err_msg[MAX_ERROR_MSG_LEN];
			sprintf(err_msg,
					"List box unable to display all files for host %s directory %s",
					m_strServerName,m_strDirectory);
			m_pSession->ssnErrorMan()->RecordError(err_msg);

		}
                                       // Put files in list box and
                                       //  have 1st file as default
                                       //
	   SUnixDirInfo *ptr = file_list_ptr;
	   CString temp;
 
		for (int i=0; i < num_files_got; i++)
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
	}
	catch(LPTSTR strErrorMsg)
	{
		m_pSession->ssnErrorMan()->RecordError(strErrorMsg);
      
		if(file_list_ptr)
			delete [] file_list_ptr;

		return FALSE;

	}
                                       
    delete [] file_list_ptr;

    return TRUE;
}



BOOL CViewSFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString str_Columns="NAME,MODIFIED,SIZE,OWNER";
	
	m_ListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\DLL\\VIEWSERVERFILES");

	if (!getUnixFiles(&m_ListCtrl))
		CDialog::OnCancel();
        //return(FALSE);

	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CViewSFileDlg::CheckServerFile(CString HostName,CString ServerFileName)
{
	CmsgGETFILEINFO msg;
	int LinesRequested=1,BytesRequired;

    BOOL ok = msg.Load((LPCSTR)HostName,(LPCSTR)ServerFileName);

    if (ok)
	   ok = msg.DoItNow();

	if (ok)
	   ok = msg.UnLoad(&LinesRequested,&BytesRequired);
	if(ok)
		return LinesRequested!=0;
	else
		return FALSE;
}
	

void CViewSFileDlg::OnOK() 
{

	if(CheckServerFile(m_strServerName,m_strDirectory+"/"+m_SelectedFileName))
	{
		CViewServerFileDlg dlg(NULL,m_strServerName,m_strDirectory+"/"+m_SelectedFileName);
		dlg.DoModal();
	}

	
//	CDialog::OnOK();
}





void CViewSFileDlg::OnDelete() 
{

		if(AfxMessageBox(_T("DELETE SELECTED FILE"),MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			return;
		CmsgDELETESVRFILE msg;
		CString temp=m_strDirectory+"/"+m_SelectedFileName;
		BOOL ok=msg.Load(m_strServerName,m_pSession->ssnAppName(),temp.GetBufferSetLength(SIZE_SERVER_FILE_NAME));
		if (ok) ok=msg.DoItNow();
		if (ok)
		{
			m_ListCtrl.DeleteItem(m_ListCtrl.GetSelectedItem());
			m_SelectedFileName=_T("");
			UpdateData(FALSE);
			GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		}

		else
		{
			::Beep(500,40);
			AfxMessageBox(m_pSession->ssnErrorMan()->GetLastErrorMsgText());
		}

		

}

void CViewSFileDlg::OnItemchangedListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_SelectedFileName=m_ListCtrl.GetText();
	if(m_SelectedFileName.IsEmpty())
	{
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
        return;
	}
	else //5/11
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	UpdateData(FALSE);

	int i=gbl_dll_man.dllUser()->appGroup(m_pSession->ssnAppName())->app_role;

	if(i&1) //bit 1 is app admin & only they can delete
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
    	
	*pResult = 0;
}

void CViewSFileDlg::OnDblclkListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_ListCtrl.GetSelectedItem() != -1)
		OnOK();
	
	*pResult = 0;
}

void CViewSFileDlg::OnRefresh() 
{
	m_ListCtrl.DeleteAllItems();
	getUnixFiles(&m_ListCtrl);
}
