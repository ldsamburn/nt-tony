/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// pqdlg.cpp : implemenation file
//
                                                       
#include "stdafx.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include "ntcssapi.h"
#include "pqmsgs.h"
#include "pqueue.h"                 
#include "ntcsss.h"
#include "pqdlg.h" 
#include "ntcssdef.h"
#include "bundleda.h"
//#include "gettoken.h"   
#include "dmcdlg.h"
#include "privapis.h"
#include "pqhelp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif 

CPqdlg * CPqdlg::m_dlgptr=NULL;
//CWinsock  * CPqdlg::m_cmdsock=NULL;

/////////////////////////////////////////////////////////////////////////////
// CPqdlg dialog


CPqdlg::CPqdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPqdlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPqdlg)
	m_PrinterListEntry = "";
	m_AppListEntry = "";
	m_RequestListEntry = "";
	//}}AFX_DATA_INIT
}

void CPqdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPqdlg)
	DDX_Control(pDX, IDC_LISTVIEW, m_PQListCtrl);
	DDX_CBString(pDX, IDC_COMBO2, m_PrinterListEntry);
	DDV_MaxChars(pDX, m_PrinterListEntry, 64);
	DDX_CBString(pDX, IDC_COMBO1, m_AppListEntry);
	DDV_MaxChars(pDX, m_AppListEntry, 16);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPqdlg, CDialog)
	//{{AFX_MSG_MAP(CPqdlg)
	ON_WM_CREATE()         
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_BN_CLICKED(IDCDELETE, OnCdelete)
	ON_MESSAGE(WM_UPDATELIST,OnUpdateList)
	ON_BN_CLICKED(ID_RELEASE, OnRelease)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDCHELP, OnChelp)
	ON_NOTIFY(NM_CLICK, IDC_LISTVIEW, OnClickListview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPqdlg message handlers


void CPqdlg::OnOK()
{
struct sockaddr_in socketAddr;
struct hostent FAR *lpHe;
char outbuf[MAX_RECV_BUFFER];
int retcode;
char tmptoken[SIZE_TOKEN];
int i,arole,urole; 
//struct PRINTER_LIST *plist_ptr;
NtcssPrinterList *plist_ptr;

char errstr[100];

	UpdateData(TRUE);   
    CWnd FAR *mCwnd=AfxGetMainWnd();
    
	CWnd *tcwnd = AfxGetMainWnd();
	m_dlgptr=this;

    
    if (m_sock) {
    	m_cmdsock->CloseSocket();
		m_sock=0;
		//delete m_cmdsock;
    }

    m_cmdsock = new CWinsock(NetEventHandler,mCwnd);

	m_sock = m_cmdsock->Socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_sock<=0) {
		::MessageBox(NULL,"Error Creating Socket","NTCSS II Error",MB_OK);
		return;
	}
	 
    m_cmdsock->Assume(m_sock); 
    
    plist_ptr=m_prt_list;
    
    for (i=0;i<m_prtcnt;i++) {
    	if (!strncmp(m_PrinterListEntry.GetBuffer(NTCSS_SIZE_PRINTERNAME),
    	              plist_ptr->prt_name,plist_ptr->prt_namelen))
    		break;
    	plist_ptr++;
    }
    	 
	lpHe=m_cmdsock->GetHostByName((const char far *)plist_ptr->hostname);
	 
	if (!lpHe) { 
	    wsprintf((char far *)errstr,"Couldn't get hostname -> %s",(char far *)plist_ptr->hostname);
    	::MessageBox(NULL,errstr, "NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
    	return;
    }
    
    socketAddr.sin_port=m_cmdsock->Htons(PRINTQ_PORT);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,lpHe->h_addr_list[0],lpHe->h_length);
     
    retcode=m_cmdsock->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
    		::MessageBox(NULL,"Unable to connect to Server","NTCSS II Error",MB_OK);
    		wsprintf(outbuf,"error is %d\000",m_cmdsock->GetLastError());
    		::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
  		  	m_cmdsock->CloseSocket();
    		return;
    	}
    }
       



    memset(outbuf,0,25);
	memset(m_printer_queue_msg.message_name,NULL,SIZE_MESSAGE_TYPE);
    strcpy((char far *)m_printer_queue_msg.message_name,(char far *)"NEWPRTQ"); 
    
	memset(m_printer_queue_msg.login_name,' ',NTCSS_SIZE_LOGINNAME);
	memcpy(m_printer_queue_msg.login_name,ui.login_name,strlen(ui.login_name));
	
	memset(m_printer_queue_msg.token,' ',SIZE_TOKEN);
	NtcssGetToken(tmptoken);  
	memcpy(m_printer_queue_msg.token,tmptoken,strlen(tmptoken));

	memset(m_printer_queue_msg.prt_name,' ',SIZE_PRT_NAME);
	memcpy(m_printer_queue_msg.prt_name,m_PrinterListEntry.GetBuffer(m_PrinterListEntry.GetLength()),
	         plist_ptr->prt_namelen);

	memset(m_printer_queue_msg.progroup_title,' ',SIZE_PROGROUP_TITLE);
	memcpy(m_printer_queue_msg.progroup_title,m_AppListEntry.GetBuffer(m_AppListEntry.GetLength()),
	         m_AppListEntry.GetLength());
    
    arole=NtcssGetAppRole(m_AppListEntry.GetBuffer(m_AppListEntry.GetLength()));
    urole=NtcssGetUserRole();

	m_printer_queue_msg.role='0';

	if (urole == 1)
		m_printer_queue_msg.role='2';
	else {
	 	if (urole & 1)
			m_printer_queue_msg.role='1';
	}
	     
	m_printer_queue_msg.terminator = NULL;
    
    retcode = m_cmdsock->Send((char FAR *)&m_printer_queue_msg,sizeof(struct PRINT_QUEUE_MSG),0);
    
    if (retcode == SOCKET_ERROR) {
       if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
       		::MessageBox(NULL,"Error Sending Data","NTCSS II Error",MB_OK);
   		 	m_cmdsock->CloseSocket();
       		return;
       }
    }
    
    memset(outbuf,0,25);
	retcode=SIZE_MESSAGE_TYPE;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return;
    } 
    
    if (!strcmp(outbuf,"NOPRTQUEUE")) {
     	::MessageBox(NULL,"Unable to Receive Printer Queue!","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return;
    } 
    
 	
	memset(outbuf,0,MAX_RECV_BUFFER);
	retcode=MAX_RECV_BUFFER;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return;
    }
     

    UpdateRequestList((char far *)outbuf);
    

    if (m_cmdsock->AsyncSelect(FD_READ|FD_WRITE|FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR) {
    	::MessageBox(NULL,"Socket Async. Failed", "NTCSS II Error",MB_OK);
    	wsprintf(outbuf,"error is %d\000",m_cmdsock->GetLastError());
    	::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
    	return;
    } 
	
	EnableDisableSelect(FALSE);
//	CDialog::OnOK();
} 


void CPqdlg::UpdateRequestList(char far *outbuf)
{
int i;
char *bptr;
char tm_buf[NTCSS_SIZE_DATETIME1];

	/* get initial list */      

	bptr=unbundleData((char far *)outbuf,"I",&m_reqcnt);
	if (m_prt_requests)
		free(m_prt_requests);

/*
CString t;
t.Format("count = %d",m_reqcnt);
AfxMessageBox(t);
*/		
	m_prt_requests=(struct PRINTER_REQUESTS *)malloc(sizeof(struct PRINTER_REQUESTS)*m_reqcnt);    
	
	LV_ITEM lvi;
	CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();
//AfxMessageBox(bptr);

	for (i=0;i<m_reqcnt;i++) {
		
		bptr=unbundleData(bptr,"CCCCCDIICI",
		               m_prt_requests[i].request_id,
		               m_prt_requests[i].progroup_title,
		               m_prt_requests[i].prt_file,
		               m_prt_requests[i].login_name,
		               m_prt_requests[i].prt_name,
		               m_prt_requests[i].time_of_request,
		               &m_prt_requests[i].security_class,
		               &m_prt_requests[i].size,
		               m_prt_requests[i].orig_host,
		               &m_prt_requests[i].status);

	               
		lvi.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.pszText=m_prt_requests[i].request_id;
		lvi.iImage=i;
		lvi.stateMask=LVIS_STATEIMAGEMASK;
		lvi.state=INDEXTOSTATEIMAGEMASK(1);

		lview.InsertItem(&lvi);


		lview.SetItemText(i,1,(char far *)m_prt_requests[i].login_name);
//		lview.SetItemText(i,2,(char far *)m_prt_requests[i].status);

		memset(tm_buf,NULL,NTCSS_SIZE_DATETIME1);
		ConvertFromNTCSSDate(m_prt_requests[i].time_of_request,tm_buf);
		
		lview.SetItemText(i,2,(char far *)tm_buf);
//		lview.SetItemText(i,3,(char far *)m_prt_requests[i].time_of_request);
		lview.SetItemText(i,3,(char far *)m_prt_requests[i].orig_host);
		lview.SetItemText(i,4,(char far *)m_prt_requests[i].prt_file);
		
	}
}	


void CALLBACK CPqdlg::NetEventHandler(WPARAM wParam, LPARAM lParam)
{
int retcode=0; 
int errcode=0;
struct INPUT_QUEUE_LIST far *last_ptr;

	switch(lParam) {
		case FD_READ:
	 		if (m_dlgptr->head_list == NULL) {
      		 	m_dlgptr->head_list=
      		       (struct INPUT_QUEUE_LIST *)malloc(sizeof(struct INPUT_QUEUE_LIST));
      		    m_dlgptr->head_list->next_buffer=NULL;   
      		    m_dlgptr->current_ptr=m_dlgptr->head_list;
				last_ptr=m_dlgptr->head_list;
      		} else {
				last_ptr=m_dlgptr->current_ptr;
      			m_dlgptr->current_ptr->next_buffer=
      		       (struct INPUT_QUEUE_LIST *)malloc(sizeof(struct INPUT_QUEUE_LIST));
      		    m_dlgptr->current_ptr=m_dlgptr->current_ptr->next_buffer;
      		    m_dlgptr->current_ptr->next_buffer=NULL;
      		}
		  
		    m_dlgptr->current_ptr->buffer_length=
		            recv(wParam,(char FAR *)m_dlgptr->current_ptr->buffer,MAX_RECV_BUFFER,0);
    		if (WSAGetLastError() != WSAEWOULDBLOCK) {
      			if (m_dlgptr->current_ptr->buffer_length <= 0) {
					if (m_dlgptr->head_list == last_ptr) {
						m_dlgptr->head_list = NULL;
						m_dlgptr->current_ptr = NULL;
						free(last_ptr);
						last_ptr=NULL;
					} else {
						free(m_dlgptr->current_ptr);
						m_dlgptr->current_ptr = last_ptr;
						m_dlgptr->current_ptr->next_buffer = NULL;
					}
     //				::MessageBox(NULL,"NetEventHandler, No Message.","NTCSS II Error",MB_OK);
    ///				closesocket(wParam);
     				return ;
      			}          
      		 } 
      		 
    		 if (m_dlgptr->current_ptr->buffer_length <= 0) {
                   TRACE("Received 0 or less message, freeing data block %d\n",WSAGetLastError());
 	               if (m_dlgptr->head_list == last_ptr) {
				      m_dlgptr->head_list = NULL;
					  m_dlgptr->current_ptr = NULL;
                      free(last_ptr);
                   } else {
				      free(m_dlgptr->current_ptr);
				      m_dlgptr->current_ptr=last_ptr;
                      m_dlgptr->current_ptr->next_buffer=NULL;
				   }
	  			   return ;
      		 }          


      		 if (m_dlgptr->current_ptr->buffer[m_dlgptr->current_ptr->buffer_length-1] == NULL) {
					m_dlgptr->PostMessage(WM_UPDATELIST,wParam,(LPARAM)m_dlgptr->head_list);
//		::MessageBox(NULL,m_dlgptr->current_ptr->buffer,"",MB_OK);
					m_dlgptr->head_list=NULL;
			 }      		 		
			 break;
		default:
			 break;
	}
	return ;
}    



CPqdlg::PopulateApps(void)
{
struct sockaddr_in socketAddr;
struct hostent FAR *lpHe;
int retcode;
char outbuf[10000];                  // May need to change this later
char far *bptr;
int i; 
CPqdlg *twnd;
int token_len;
char errstr[100];
   
    CWnd FAR *mCwnd=AfxGetMainWnd();
    twnd=this;
    
   	EnableDisableSelect(FALSE);
       
    m_cmdsock = new CWinsock(NetEventHandler,mCwnd);
    //m_spoolsock = new CWinsock(NetEventHandler,mCwnd);

	m_sock = m_cmdsock->Socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_sock<=0) {
		::MessageBox(NULL,"Error Creating Socket","NTCSS II Error",MB_OK);
		return FALSE;
	}
	 
    m_cmdsock->Assume(m_sock);
    	 
	lpHe=m_cmdsock->GetHostByName((const char far *)NtcssServerName);
	 
	if (!lpHe) {  
	    wsprintf((char far *)errstr,"Couldn't get hostname -> %s",(char far *)NtcssServerName);
    	::MessageBox(NULL,errstr, "NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
    	return FALSE;
    } 
    
    socketAddr.sin_port=m_cmdsock->Htons(DB_PORT);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,lpHe->h_addr_list[0],lpHe->h_length);

    
    retcode=m_cmdsock->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
    		::MessageBox(NULL,"Unable to connect to Server","NTCSS II Error",MB_OK);
    		wsprintf(outbuf,"error is %d\000",m_cmdsock->GetLastError());
    		::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
  		  	m_cmdsock->CloseSocket();
    		return FALSE;
    	}
    } 
       
    
    memset(outbuf,0,10000);
    strcpy((char far *)m_get_app_msg.message_name,(char far *)"GETAPPLIST");
    memset(m_get_app_msg.login_name,' ',SIZE_LOGIN_NAME);
    strncpy((char far *)m_get_app_msg.login_name,(char far *)ui.login_name,
              strlen((char far *)ui.login_name)); 
    memset(m_get_app_msg.token,' ',SIZE_TOKEN);
    NtcssGetToken(m_get_app_msg.token);
    token_len = (strlen(m_get_app_msg.token)); 
     
    if (token_len < SIZE_TOKEN) {  
    	m_get_app_msg.token[token_len] = ' ';
    }
    m_get_app_msg.terminator = NULL;
	
    retcode = m_cmdsock->Send((char FAR *)&m_get_app_msg,
              SIZE_LOGIN_NAME + SIZE_TOKEN + SIZE_MESSAGE_TYPE+1,0);
    
    if (retcode == SOCKET_ERROR) {
       if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
       		::MessageBox(NULL,"Error Sending Data","NTCSS II Error",MB_OK);
   		 	m_cmdsock->CloseSocket();
       		return FALSE;
       }
    }
    
    memset(outbuf,0,10000); 
	retcode=20;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return FALSE;
    } 
    
    if (!strcmp(outbuf,"NOAPPLIST")) {  
        memset(outbuf,0,10000); 
    	retcode=80;
	    if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	   ::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	   m_cmdsock->CloseSocket();
     	   return FALSE;
     	}   
        ::MessageBox(NULL,outbuf+5,"NTCSS II Error",MB_OK);
        return FALSE;
    } 
 
    memset(outbuf,0,10000);
    
	retcode=10000;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return FALSE;
    }
    

	bptr=unbundleData((char far *)outbuf,"I",&m_appcnt);
	
	m_app_list=(struct APP_LIST *)malloc(sizeof(struct APP_LIST)*m_appcnt);    
	
	for (i=0;i<m_appcnt;i++) {
		bptr=unbundleData(bptr,"CC",m_app_list[i].progroup_title,m_app_list[i].progroup_host);
//		::MessageBox(NULL,m_app_list[i].progroup_title,"Title",MB_OK);
//		::MessageBox(NULL,m_app_list[i].progroup_host,"HOST",MB_OK);
		CWnd *tcwnd = AfxGetMainWnd();
		HWND thwnd = tcwnd->m_hWnd;
		SendDlgItemMessage(IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)m_app_list[i].progroup_title);
	}
	
   	m_cmdsock->CloseSocket();
	 
//	delete m_cmdsock;
	m_sock=0;
	//delete m_spoolsock;
    return TRUE;   
    
}
 
 
 
int CPqdlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
//	PopulateApps();
	
	return 0;
}

BOOL CPqdlg::OnInitDialog()
{
int ycenter,xcenter,dwidth,dheight; 
RECT drect;

	CDialog::OnInitDialog();
		CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();

	LV_COLUMN listColumn;
	LV_ITEM listItem;

	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=80;
	listItem.mask=LVIF_TEXT|LVIF_PARAM;
	listItem.iSubItem=0;
	 
	CString str_Columns="Request No.,User,Req. Time,Orig. Host,Filename";

 	static long PQHelpTopics[]={HT_RequestNo_,HT_User_PrintQueue,
		HT_Req_Time,HT_Orig_Host,HT_Filename_PrintQueue};

		
	m_PQListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PQUEUE\\PQUEUE",
					 PQHelpTopics); 

	m_app_list=NULL;
	m_prt_list=NULL; 
	m_prt_requests=NULL;
	m_sock=0;
	
	head_list=NULL;
	current_ptr=NULL;

    ycenter = ::GetSystemMetrics(SM_CYSCREEN)/2;
    xcenter = ::GetSystemMetrics(SM_CXSCREEN)/2;

    GetWindowRect((RECT far *)&drect); 
    
    dwidth=drect.right-drect.left;
    dheight=drect.bottom-drect.top;

    drect.left = xcenter - dwidth/2;
    drect.top = ycenter - dheight/2;      
    
    MoveWindow(drect.left,drect.top,dwidth,dheight,TRUE);

	NtcssGetUserInfo(&ui);

    NtcssGetServerName((char far *)NtcssServerName,MAX_SERVER_NAME); 
 
	if (PopulateApps() == FALSE) {
		EndDialog(FALSE);
		return FALSE;
	}
	
	EnableDisableButtons(FALSE);

	help_object.Initialize();
	help_object.SetWindowHelp(HT_BatchJobQueue_WindowDescriptions);   

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CPqdlg::OnSelchangeCombo1()
{  
//NtcssUserInfo ui;
char buf[100];
//char tmptoken[SIZE_TOKEN]; 
char hostname[SIZE_HOST_NAME];
     
    UpdateData(TRUE);
	// TODO: Add your control notification handler code here 
	GetDlgItemText(IDC_COMBO1,buf,100);       
	
/*
	NtcssGetUserInfo(&ui);
	memset(m_printer_list_msg.login_name,' ',NTCSS_SIZE_LOGINNAME);
	memcpy(m_printer_list_msg.login_name,ui.login_name,strlen(ui.login_name));
	
	memset(m_printer_list_msg.token,' ',SIZE_TOKEN);
	NtcssGetToken(tmptoken);  
	memcpy(m_printer_list_msg.token,tmptoken,strlen(tmptoken));
	
	memset(m_printer_list_msg.progroup_title,' ',SIZE_PROGROUP_TITLE);
	memcpy(m_printer_list_msg.progroup_title,m_AppListEntry.GetBuffer(m_AppListEntry.GetLength()),
	         m_AppListEntry.GetLength());
	m_printer_list_msg.printer_access='0'; 
	m_printer_list_msg.terminator = NULL;
	
	if (m_sock) {
		m_cmdsock->CloseSocket();
		m_sock=0;
	}    	
	*/

	memset(hostname,' ',SIZE_HOST_NAME);

	NtcssGetAppHost((char *)(LPCTSTR)m_AppListEntry,
			hostname,SIZE_HOST_NAME);

	CmsgGETPRTLIST cPrinters;

	cPrinters.Load(m_AppListEntry.GetBuffer(m_AppListEntry.GetLength()),
				   '0',hostname);

	BOOL nRet=cPrinters.DoItNow();

	if (nRet != TRUE) {
		AfxMessageBox("Error getting printers");
	}

	m_prtcnt=cPrinters.UnLoadCount();
	m_prt_list=(NtcssPrinterList *)malloc(sizeof(NtcssPrinterList)*m_prtcnt);
    
//CString t;
//t.Format("Printer count = %d",m_prtcnt);
//AfxMessageBox(t);
	
	nRet=cPrinters.UnLoad(&m_prtcnt,m_prt_list);
	
	EnableDisableButtons(FALSE);
	PopulatePrts();	
}

void CPqdlg::OnCancel()
{   
    if (m_app_list)
    	free(m_app_list);
    if (m_prt_list)
    	free(m_prt_list);
    	
	CDialog::OnCancel();   
}
 
CPqdlg::PopulatePrts(void)
{
//struct sockaddr_in socketAddr;
//struct hostent FAR *lpHe;
//int retcode;
//char outbuf[MAX_RECV_BUFFER];                  // May need to change this later
//char far *bptr;
char prtstring[64];
//char errmsg[100];
//char errstr[100];
int i; 
 
/*  
    CWnd FAR *mCwnd=AfxGetMainWnd();
    m_cmdsock = new CWinsock(NetEventHandler,mCwnd);

	m_sock = m_cmdsock->Socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_sock<=0) {
		::MessageBox(NULL,"Error Creating Socket","NTCSS II Error",MB_OK);
		return FALSE;
	}
	 
    m_cmdsock->Assume(m_sock);
    	 
	lpHe=m_cmdsock->GetHostByName((const char far *)NtcssServerName);
	 
	if (!lpHe) {  
	    wsprintf((char far *)errstr,"Couldn't get hostname -> %s",(char far *)NtcssServerName);
    	::MessageBox(NULL,errstr, "NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
    	return FALSE;
    } 
    
    socketAddr.sin_port=m_cmdsock->Htons(DB_PORT);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,lpHe->h_addr_list[0],lpHe->h_length);

    retcode=m_cmdsock->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
    		::MessageBox(NULL,"Unable to connect to Server","NTCSS II Error",MB_OK);
    		wsprintf(outbuf,"error is %d\000",m_cmdsock->GetLastError());
    		::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
  		  	m_cmdsock->CloseSocket();
    		return FALSE;
    	}
    } 
       
    memset(outbuf,0,MAX_PRINTER_LIST_SIZE);
    strcpy((char far *)m_printer_list_msg.message_name,(char far *)"GETPRTLIST");
	m_printer_list_msg.terminator = NULL;

    retcode = m_cmdsock->Send((char FAR *)&m_printer_list_msg,sizeof(struct PRINTER_LIST_MSG),0);
    
    if (retcode == SOCKET_ERROR) {
       if (m_cmdsock->GetLastError() != WSAEWOULDBLOCK) {
       		::MessageBox(NULL,"Error Sending Data","NTCSS II Error",MB_OK);
   		 	m_cmdsock->CloseSocket();
       		return FALSE;
       }
    }

	memset(outbuf,0,MAX_PRINTER_LIST_SIZE);
	retcode=20;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return FALSE;
    } 
    


    if (!strcmp(outbuf,"NOPRTLIST")) {
//     	::MessageBox(NULL,"Unable to Receive Printer List!","NTCSS II Error",MB_OK);
   	  	memset(errmsg,0,80);
		retcode=80;
		if (!RecvDataBuffer(m_cmdsock,errmsg,&retcode)) {
     		MessageBox("Unable to read response!","NTCSS II Error",MB_OK);
    		m_cmdsock->CloseSocket();
     		return FALSE;
    	}
     	MessageBox(errmsg+5,"NTCSS II Error",MB_OK | MB_ICONSTOP);

    	m_cmdsock->CloseSocket();
     	return FALSE;
    }

    memset(outbuf,0,MAX_RECV_BUFFER);
	retcode=MAX_RECV_BUFFER;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return FALSE;
    }
    

	bptr=unbundleData((char far *)outbuf,"I",&m_prtcnt);
	
	if (m_prt_list)
		free(m_prt_list);
*/		

	SendDlgItemMessage(IDC_COMBO2,CB_RESETCONTENT,0,0L);


//	m_prt_list=(struct PRINTER_LIST *)malloc(sizeof(struct PRINTER_LIST)*m_prtcnt);    
	
	
	for (i=0;i<m_prtcnt;i++) { 

		/*
		memset(m_prt_list[i].prt_name,' ',SIZE_PRT_NAME);
		memset(m_prt_list[i].hostname,' ',SIZE_HOST_NAME);
		memset(m_prt_list[i].location,' ',SIZE_PRT_LOCATION);
		memset(m_prt_list[i].device_name,' ',SIZE_PRT_DEV_NAME);
		memset(m_prt_list[i].redirect_printer,' ',SIZE_PRT_RED);
	
		
		bptr=unbundleData(bptr,"CCCCIICCCCCCCC",
		      m_prt_list[i].prt_name,
		      m_prt_list[i].hostname,
		      m_prt_list[i].location,
		      m_prt_list[i].device_name,
		      &m_prt_list[i].que_enabled,
		      &m_prt_list[i].printer_enabled,
		      m_prt_list[i].redirect_printer,
		      m_prt_list[i].driver_file,
		      m_prt_list[i].ui_file,
		      m_prt_list[i].data_file,
		      m_prt_list[i].help_file, 
			  m_prt_list[i].monitor_file,
			  m_prt_list[i].monitor_name,
			  m_prt_list[i].filename);


		m_prt_list[i].prt_namelen=strlen(m_prt_list[i].prt_name);
		
//		CWnd *tcwnd = AfxGetMainWnd();
//		HWND thwnd = tcwnd->m_hWnd;
*/

		memset(m_prt_list[i].hostname,' ',SIZE_HOST_NAME);

		NtcssGetAppHost((char *)(LPCTSTR)m_AppListEntry,
			m_prt_list[i].hostname,SIZE_HOST_NAME);

		memset(prtstring,NULL,64);
		wsprintf(prtstring,"%s in %.30s",m_prt_list[i].prt_name,
				 m_prt_list[i].location);


		SendDlgItemMessage(IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)prtstring);

	}

//   	m_cmdsock->CloseSocket(); 

	CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();

//	delete m_cmdsock;

	m_sock=0;
 
    return TRUE;   
    
}
 
CPqdlg::PopulateQueue(void)
{
int retcode;
char outbuf[MAX_RECV_BUFFER];                  // May need to change this later
char far *bptr;
int i; 

    memset(outbuf,0,MAX_RECV_BUFFER);
	retcode=MAX_RECV_BUFFER;
	if (!RecvDataBuffer(m_cmdsock,outbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
     	return FALSE;
    }

	bptr=unbundleData((char far *)outbuf,"I",&m_reqcnt);
	
	if (m_prt_list)
		free(m_prt_list);
		                                                              
	m_prt_requests=(struct PRINTER_REQUESTS *)malloc(sizeof(struct PRINTER_REQUESTS)*m_reqcnt);    
	
	CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();
	
	for (i=0;i<m_prtcnt;i++) {
		bptr=unbundleData(bptr,"CCCCCDIICI",
		      m_prt_requests[i].request_id,
		      m_prt_requests[i].progroup_title,
		      m_prt_requests[i].prt_file,
		      m_prt_requests[i].login_name,
		      m_prt_requests[i].prt_name,
		      m_prt_requests[i].time_of_request,
		      &m_prt_requests[i].security_class,
		      &m_prt_requests[i].size,
		      m_prt_requests[i].orig_host,
		      &m_prt_requests[i].status);

	}
	
   	m_cmdsock->CloseSocket();
 
    return TRUE;   
    
}


BOOL CPqdlg::RecvDataBuffer(CWinsock *sptr,char *buffer,int *len)
{
int tcnt,icnt;                     
char *tptr;
int retcode; 
int errcode;

    memset(buffer,' ',*len);
    
    tcnt=0;
    tptr=buffer;
    
    while (tcnt < *len) {           
        icnt=(*len-tcnt);
        memset(tptr,' ',icnt);
    	retcode=sptr->Recv((char FAR *)tptr,icnt,0);
    	errcode=sptr->GetLastError();
 //   	if (errcode != WSAEWOULDBLOCK) {
      		if (retcode <= 0) {
//     			::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
//    			sptr->CloseSocket();
     			return FALSE;
      		}          
      		tcnt += retcode;
      		if (tptr[retcode-1] == NULL)
	      		break;
      		tptr=tptr+retcode;
	      		
//	    }
    }                 
    *len=tcnt;
    return TRUE;
}

 
void CPqdlg::OnSelchangeCombo2()
{
int i; 
//struct PRINTER_LIST *plist_ptr;
NtcssPrinterList *plist_ptr;

	EnableDisableSelect(TRUE);

	UpdateData(TRUE);   
    CWnd FAR *mCwnd=AfxGetMainWnd();
    
	CWnd *tcwnd = AfxGetMainWnd();
	HWND thwnd = tcwnd->m_hWnd;
    
    plist_ptr=m_prt_list;
    
    for (i=0;i<m_prtcnt;i++) {
    	if (!strncmp(m_PrinterListEntry.GetBuffer(NTCSS_SIZE_PRINTERNAME),
    	              plist_ptr->prt_name,plist_ptr->prt_namelen))
    		break;
    	plist_ptr++;
    }
    
 //   SetDlgItemText(IDC_PRTHOST,(LPCSTR)plist_ptr->hostname); 
    SetDlgItemText(IDC_DEVICE_NAME,(LPCSTR)plist_ptr->device_name); 

    if (plist_ptr->que_enabled == 1)
    	SetDlgItemText(IDC_QUESTAT,(LPCSTR)"Enabled");
	else    	
    	SetDlgItemText(IDC_QUESTAT,(LPCSTR)"Disabled");
    		
    if (plist_ptr->printer_enabled == 1)
    	SetDlgItemText(IDC_PRTSTAT,(LPCSTR)"Enabled");
	else    	
    	SetDlgItemText(IDC_PRTSTAT,(LPCSTR)"Disabled");    	
    	
    if (strcmp(plist_ptr->redirect_printer,"***"))
    	SetDlgItemText(IDC_REDIRECTED,(LPCSTR)plist_ptr->redirect_printer);
	else    	
    	SetDlgItemText(IDC_REDIRECTED,(LPCSTR)"None"); 

	if (m_sock) {
		m_cmdsock->CloseSocket();
		m_sock=0;
	} 
   	
	CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();

	lview.DeleteAllItems();
    	

}


void CPqdlg::OnCdelete()
{ 
struct sockaddr_in socketAddr;
struct hostent FAR *lpHe;
int retcode;
char outbuf[100];
char msgbuf[20];                  // May need to change this later
char tmptoken[32];
char errstr[100];
int i;
//struct PRINTER_LIST *plist_ptr;
struct PRINTER_REQUESTS *rlist_ptr; 
NtcssPrinterList *plist_ptr;

	UpdateData(TRUE);
	
    CWnd FAR *mCwnd=AfxGetMainWnd();
    m_spoolsock = new CWinsock(NetEventHandler,mCwnd);

//	MessageBox(m_RequestListEntry,"Delete Item",MB_OK);
	if (m_RequestListEntry.GetLength() == 0) {
		MessageBox("No Print Request Selected!","NTCSS II ERROR",
		            MB_OK | MB_ICONEXCLAMATION); 	
		return;
	}  

   plist_ptr=m_prt_list;
    
    for (i=0;i<m_prtcnt;i++) {
    	if (!strncmp(m_PrinterListEntry.GetBuffer(NTCSS_SIZE_PRINTERNAME),
    	              plist_ptr->prt_name,plist_ptr->prt_namelen))
    		break;
    	plist_ptr++;
    }
	   
	   
	wsprintf(outbuf,"Delete Request %.3s for Printer %s?",m_RequestListEntry.GetBuffer(36),
	                 plist_ptr->prt_name);  
	                 
 	                 	
	i=MessageBox(outbuf,"NTCSS II",MB_ICONQUESTION | MB_YESNO); 
	
	if (i == IDNO) 
			return;
			
	if (m_sock1) {
		m_spoolsock->CloseSocket();
	}
    
	m_sock1 = m_spoolsock->Socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_sock1<=0) {
		::MessageBox(NULL,"Error Creating Socket","NTCSS II Error",MB_OK);
		return;
	}
	 
    m_spoolsock->Assume(m_sock1);


    	 
	lpHe=m_cmdsock->GetHostByName((const char far *)plist_ptr->hostname);
	 
	if (!lpHe) {                                                       
	    wsprintf((char far *)errstr,"Couldn't get hostname -> %s",(char far *)plist_ptr->hostname);
    	::MessageBox(NULL,errstr, "NTCSS II Error",MB_OK);
    	m_cmdsock->CloseSocket();
    	return;
    } 
    
    socketAddr.sin_port=m_spoolsock->Htons(PRINTQ_PORT);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,lpHe->h_addr_list[0],lpHe->h_length);

    retcode=m_spoolsock->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (m_spoolsock->GetLastError() != WSAEWOULDBLOCK) {
    		::MessageBox(NULL,"Unable to connect to Server","NTCSS II Error",MB_OK);
    		wsprintf(outbuf,"error is %d\000",m_spoolsock->GetLastError());
    		::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
  		  	m_spoolsock->CloseSocket();
    		return;
    	}
    }
     
   	memset(m_delete_request_msg.message_name,NULL,20);
    strcpy((char far *)m_delete_request_msg.message_name,(char far *)"DELPRTREQ");
    
   	memset(m_delete_request_msg.login_name,' ',NTCSS_SIZE_LOGINNAME);
	memcpy(m_delete_request_msg.login_name,ui.login_name,strlen(ui.login_name));

	memset(m_delete_request_msg.token,' ',SIZE_TOKEN);
	NtcssGetToken(tmptoken);  
	memcpy(m_delete_request_msg.token,tmptoken,strlen(tmptoken));
	
   	memset(m_delete_request_msg.prt_name,' ',NTCSS_SIZE_PRINTERNAME);
	memcpy(m_delete_request_msg.prt_name,plist_ptr->prt_name,plist_ptr->prt_namelen); 

	memset(m_delete_request_msg.request_id,' ',SIZE_PRT_REQ_ID);
	
    rlist_ptr=m_prt_requests;
    for (i=0;i<m_reqcnt;i++) {
    	if (!strncmp(m_RequestListEntry.GetBuffer(SIZE_PRT_REQ_ID),
    	              rlist_ptr->request_id,3))
    		break;
    	rlist_ptr++;
    }

	memcpy(m_delete_request_msg.request_id,rlist_ptr->request_id,
	         strlen(rlist_ptr->request_id));

	m_delete_request_msg.terminator = NULL;

	//_fmemcpy(m_delete_request_msg.request_id,m_RequestListEntry.GetBuffer(36),3);

    retcode = m_spoolsock->Send((char FAR *)&m_delete_request_msg,sizeof(struct DELETE_REQUEST_MSG),0);
    
    if (retcode == SOCKET_ERROR) {
       if (m_spoolsock->GetLastError() != WSAEWOULDBLOCK) {
       		::MessageBox(NULL,"Error Sending Data","NTCSS II Error",MB_OK);
   		 	m_spoolsock->CloseSocket();
       		return;
       }
    }
    
    memset(msgbuf,0,20);
	retcode=20;
	if (!RecvDataBuffer(m_spoolsock,msgbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_spoolsock->CloseSocket();
     	return;
    } 
    
    memset(outbuf,0,100);
	retcode=100;
	if (!RecvDataBuffer(m_spoolsock,outbuf,&retcode)) {
   		MessageBox("Did not receive message","NTCSS II Error",MB_OK);
   		m_spoolsock->CloseSocket();
   		return;
   	} 
    
    MessageBox(outbuf+5,"NTCSS II ",MB_OK | MB_ICONINFORMATION);
   	m_spoolsock->CloseSocket();
	
	delete m_spoolsock;
	m_sock1=0;

	OnOK();
	
}



LONG CPqdlg::OnUpdateList(WPARAM wParam, LPARAM lParam)
{ 
struct INPUT_QUEUE_LIST *input_list,*tptr;
int i,retcode;
char far *message_type,*bptr,*tmp_bptr;

	input_list=(struct INPUT_QUEUE_LIST *)lParam;
	
	message_type=(char far *)input_list->buffer;
	
	if (strcmp(message_type,(char far *)"NTCSSHB")) { 
	
		i=input_list->buffer_length;
		tptr=input_list->next_buffer;
		while(tptr) {
			i += tptr->buffer_length;
			tptr=tptr->next_buffer;
		}
		
		bptr=(char *)malloc(i);
		tmp_bptr=bptr;
	
		memset(bptr,NULL,i);
	
		memcpy(bptr,input_list->buffer+20,input_list->buffer_length-20);
		tmp_bptr += input_list->buffer_length-20;
	
		tptr=input_list->next_buffer;

		while(tptr) {
			memcpy(tmp_bptr,tptr->buffer,tptr->buffer_length);
			tmp_bptr += tptr->buffer_length;
			tptr=tptr->next_buffer;
		} 
		
	 	UpdateRequestList((char far *)bptr);
	 	free(bptr);
	} else {
	    retcode = send(wParam,(char FAR *)input_list->buffer,
	                              input_list->buffer_length,0);
	}	                              
			
 	
  	while(input_list->next_buffer) {
 		tptr=input_list->next_buffer;
		free(input_list);
		input_list=tptr;
	} 
	
	free(input_list);
 	
 	return 0L;
}

void CPqdlg::OnRelease()
{ 
//struct sockaddr_in socketAddr;
//struct hostent FAR *lpHe;
//int retcode;
//char outbuf[100];
//char msgbuf[20];                  // May need to change this later
//char tmptoken[32];                                                     
//char errstr[100];
int i;
//struct PRINTER_LIST *plist_ptr;
struct PRINTER_REQUESTS *rlist_ptr; 
NtcssPrinterList *plist_ptr;

	UpdateData(TRUE);
	/*
    CWnd FAR *mCwnd=AfxGetMainWnd();
    m_spoolsock = new CWinsock(NetEventHandler,mCwnd);


	if (m_RequestListEntry.GetLength() == 0) {
		MessageBox("No Print Request Selected!","NTCSS II ERROR",
		            MB_OK | MB_ICONEXCLAMATION); 	
		return;
	}
	if (m_sock1) {
		m_spoolsock->CloseSocket();
	}
    
	m_sock1 = m_spoolsock->Socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_sock1<=0) {
		::MessageBox(NULL,"Error Creating Socket","NTCSS II Error",MB_OK);
		return;
	}
	 
    m_spoolsock->Assume(m_sock1);
*/
    plist_ptr=m_prt_list;
    
    for (i=0;i<m_prtcnt;i++) {
    	if (!strncmp(m_PrinterListEntry.GetBuffer(NTCSS_SIZE_PRINTERNAME),
    	              plist_ptr->prt_name,plist_ptr->prt_namelen))
    		break;
    	plist_ptr++;
    }

    rlist_ptr=m_prt_requests;
    for (i=0;i<m_reqcnt;i++) {
    	if (!strncmp(m_RequestListEntry.GetBuffer(SIZE_PRT_REQ_ID),
    	              rlist_ptr->request_id,3))
    		break;
    	rlist_ptr++;
    }

	CmsgRelRequest cr;

	cr.Load(plist_ptr->prt_name,rlist_ptr->request_id,plist_ptr->hostname);

	cr.DoItNow();

	cr.UnLoad();


 /*   	 
	lpHe=m_spoolsock->GetHostByName((const char far *)plist_ptr->hostname);
	 
	if (!lpHe) {
	    wsprintf((char far *)errstr,"Couldn't get hostname -> %s",(char far *)plist_ptr->hostname);
    	::MessageBox(NULL,errstr, "NTCSS II Error",MB_OK);
    	m_spoolsock->CloseSocket();
    	return;
    } 
    
    socketAddr.sin_port=m_spoolsock->Htons(PRINTQ_PORT);
    socketAddr.sin_family=AF_INET;
    memcpy(&socketAddr.sin_addr,lpHe->h_addr_list[0],lpHe->h_length);

    retcode=m_spoolsock->Connect((const struct sockaddr *)&socketAddr,sizeof(socketAddr));
    if (retcode < 0) { 
        if (m_spoolsock->GetLastError() != WSAEWOULDBLOCK) {
    		::MessageBox(NULL,"Unable to connect to Server","NTCSS II Error",MB_OK);
    		wsprintf(outbuf,"error is %d\000",m_spoolsock->GetLastError());
    		::MessageBox(NULL,outbuf, "NTCSS II Error",MB_OK);
  		  	m_spoolsock->CloseSocket();
    		return;
    	}
    }
     
   	memset(m_delete_request_msg.message_name,NULL,SIZE_MESSAGE_TYPE);
    strcpy((char far *)m_delete_request_msg.message_name,(char far *)"RELPRTREQ");
    
   	memset(m_delete_request_msg.login_name,' ',NTCSS_SIZE_LOGINNAME);
	memcpy(m_delete_request_msg.login_name,ui.login_name,strlen(ui.login_name));

	memset(m_delete_request_msg.token,' ',SIZE_TOKEN);
	NtcssGetToken(tmptoken);  
	memcpy(m_delete_request_msg.token,tmptoken,strlen(tmptoken));
	
   	memset(m_delete_request_msg.prt_name,' ',NTCSS_SIZE_PRINTERNAME);
	memcpy(m_delete_request_msg.prt_name,plist_ptr->prt_name,plist_ptr->prt_namelen); 

	memset(m_delete_request_msg.request_id,' ',SIZE_PRT_REQ_ID);

    rlist_ptr=m_prt_requests;
    for (i=0;i<m_reqcnt;i++) {
    	if (!strncmp(m_RequestListEntry.GetBuffer(SIZE_PRT_REQ_ID),
    	              rlist_ptr->request_id,3))
    		break;
    	rlist_ptr++;
    }

	memcpy(m_delete_request_msg.request_id,rlist_ptr->request_id,
	         strlen(rlist_ptr->request_id));

	m_delete_request_msg.terminator = NULL;

    retcode = m_spoolsock->Send((char FAR *)&m_delete_request_msg,sizeof(struct DELETE_REQUEST_MSG),0);
    
    if (retcode == SOCKET_ERROR) {
       if (m_spoolsock->GetLastError() != WSAEWOULDBLOCK) {
       		::MessageBox(NULL,"Error Sending Data","NTCSS II Error",MB_OK);
   		 	m_cmdsock->CloseSocket();
       		return;
       }
    }
    
    memset(msgbuf,0,SIZE_MESSAGE_TYPE);
	retcode=SIZE_MESSAGE_TYPE;
	if (!RecvDataBuffer(m_spoolsock,msgbuf,&retcode)) {
     	::MessageBox(NULL,"Did not receive message","NTCSS II Error",MB_OK);
    	m_spoolsock->CloseSocket();
     	return;
    } 
    
    memset(outbuf,0,100);
	retcode=100;
	if (!RecvDataBuffer(m_spoolsock,outbuf,&retcode)) {
   		MessageBox("Did not receive message","NTCSS II Error",MB_OK);
   		m_cmdsock->CloseSocket();
   		return;
   	} 
    
    MessageBox(outbuf+5,"NTCSS II ",MB_OK | MB_ICONINFORMATION);
   	m_spoolsock->CloseSocket();
	
	delete m_spoolsock;

  */
	m_sock1=0;
	OnOK();
	
}

void CPqdlg::EnableDisableButtons(BOOL nEDInd)
{
CWnd *tCwnd;

    tCwnd=GetDlgItem(IDCDELETE);
    tCwnd->EnableWindow(nEDInd);
    tCwnd=GetDlgItem(ID_RELEASE);
    tCwnd->EnableWindow(nEDInd);
}

void CPqdlg::EnableDisableSelect(BOOL nEDInd)
{
CWnd *tCwnd;

    tCwnd=GetDlgItem(IDOK);
    tCwnd->EnableWindow(nEDInd);
}


void CPqdlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (IsIconic()) {
	    DefWindowProc(WM_ICONERASEBKGND,(WORD)dc.m_hDC,0L);
	    CRect rc;
	    GetClientRect(&rc);
	    rc.left = (rc.right - ::GetSystemMetrics(SM_CXICON)) >> 1;
	    rc.top = (rc.bottom - ::GetSystemMetrics(SM_CYICON)) >> 1;
	    HICON hIcon = AfxGetApp()->LoadIcon(IDI_PRTDLG);
	    dc.DrawIcon(rc.left,rc.top,hIcon); 
	}
	// Do not call CDialog::OnPaint() for painting messages
}
                                          
BOOL CPqdlg::OnEraseBkgnd(CDC* pDC) {
	if (IsIconic())
		return TRUE;
	else
	    return CDialog::OnEraseBkgnd(pDC);
}                                          

void CPqdlg::OnChelp() 
{
	char root_dir[100];

	memset(root_dir,NULL,100);

	GetPrivateProfileString("NTCSS","NTCSS_ROOT_DIR","C:/ntcss2",
		root_dir,100,"ntcss.ini");

	strcat(root_dir,"/help/dskscrn.hlp");
	::WinHelp(AfxGetMainWnd()->m_hWnd,root_dir,HELP_KEY,
		      (DWORD)"Print Queue Dialog");

	
}

void CPqdlg::OnClickListview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CListView * cbv = (CListView*)GetDlgItem(IDC_LISTVIEW);
	CListCtrl& lview = cbv->GetListCtrl();
	int tmppid=lview.GetNextItem(-1,LVNI_SELECTED);

	if (tmppid == -1) {
		m_RequestListEntry="";
		return;
	}

	m_RequestListEntry=lview.GetItemText( tmppid, 0 );
  

	EnableDisableSelect(FALSE);

	EnableDisableButtons(TRUE);    		

	*pResult = 0;
}

char *CPqdlg::ConvertFromNTCSSDate(char *srcbuf, char *dstbuf)
{

	sprintf(dstbuf,"%.2s/%.2s/%.2s %.2s:%.2s",
		            &srcbuf[2],&srcbuf[4],&srcbuf[0],
					&srcbuf[7],&srcbuf[9]);

	return(dstbuf);
}

BOOL CPqdlg::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}
