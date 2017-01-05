/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// PrinterUIView.cpp : implementation of the CPrinterUIView class

#include "stdafx.h"
#include "genutils.h"
#include "PrinterUI.h"
#include "privapis.h"
#define  theman
#include "ini.h"
#include "PrinterUIDoc.h"
#include "PrinterUIView.h"
#include "EditPrinter.h"
#include "EditAdapter.h"
#include "LogFileDialog.h"
#include "MainFrm.h"
#include "PUIHelp.h"
#include "msgs.h"
#include "PrinterUI.h"
#include "StringEx.h"
//#include "Splash.h"


//HANDLE g_hEvent = NULL; //debug multithreads


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LV_COLUMN listColumn;
LV_ITEM listItem;
/////////////////////////////////////////////////////////////////////////////
// CPrinterUIView

IMPLEMENT_DYNCREATE(CPrinterUIView, CFormView)

BEGIN_MESSAGE_MAP(CPrinterUIView, CFormView)
	//{{AFX_MSG_MAP(CPrinterUIView)
	ON_BN_CLICKED(IDC_Apply, OnApply)
	ON_COMMAND(ID_ADAPTER_DELETE, OnAdapterDelete)
	ON_UPDATE_COMMAND_UI(ID_ADAPTER_DELETE, OnUpdateAdapterDelete)
	ON_COMMAND(ID_PRINT_DELETE, OnPrintDelete)
	ON_UPDATE_COMMAND_UI(ID_PRINT_DELETE, OnUpdatePrintDelete)
	ON_COMMAND(ID_STATUS_HALTPRINTER, OnStatusHaltprinter)
	ON_UPDATE_COMMAND_UI(ID_STATUS_HALTPRINTER, OnUpdateStatusHaltprinter)
	ON_COMMAND(ID_STATUS_ENABLEPRINTER, OnStatusEnableprinter)
	ON_UPDATE_COMMAND_UI(ID_STATUS_ENABLEPRINTER, OnUpdateStatusEnableprinter)
	ON_COMMAND(ID_STATUS_ENABLEQUEUEING, OnStatusEnablequeueing)
	ON_UPDATE_COMMAND_UI(ID_STATUS_ENABLEQUEUEING, OnUpdateStatusEnablequeueing)
	ON_COMMAND(ID_STATUS_DISABLEQUEUEING, OnStatusDisablequeueing)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DISABLEQUEUEING, OnUpdateStatusDisablequeueing)
	ON_COMMAND(ID_PRINT_ADD, OnPrintAdd)
	ON_COMMAND(ID_PRINT_EDIT, OnPrintEdit)
	ON_UPDATE_COMMAND_UI(ID_PRINT_EDIT, OnUpdatePrintEdit)
	ON_COMMAND(ID_ADAPTER_ADD, OnAdapterAdd)
	ON_COMMAND(ID_ADAPTER_EDIT, OnAdapterEdit)
	ON_UPDATE_COMMAND_UI(ID_ADAPTER_EDIT, OnUpdateAdapterEdit)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, OnPUIQuit)
	ON_NOTIFY(NM_CLICK, IDC_PrinterList, OnClickPrinterList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_APPLICATIONCTRL, OnItemchangedApplicationctrl)
	ON_NOTIFY(NM_DBLCLK, IDC_PrinterList, OnDblclkPrinterList)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_AdapterList, OnDblclkAdapterList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIView construction/destruction

CPrinterUIView::CPrinterUIView()
	: CFormView(CPrinterUIView::IDD)
{
	//{{AFX_DATA_INIT(CPrinterUIView)
	m_Location = _T("");
	m_MaxRequestSize = 0;
	m_PrinterClass = _T("");
	m_AdapterIP = _T("");
	m_AdapterType = _T("");
	m_AdapterPort = _T("");
	m_SuspendState = _T("");
	m_RedirectPrinter = _T("");
	m_Access = _T("");
	//}}AFX_DATA_INIT
	m_GotLog=m_Queue_Enabled=m_Printer_Enabled=FALSE;
	m_strCurrentApp=m_strCurrentPrinter=_T("");
	m_PID=_T("");
	m_app=_T("");
	m_DoingUpdates=FALSE;
}

CPrinterUIView::~CPrinterUIView()
{

}

void CPrinterUIView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrinterUIView)
	DDX_Control(pDX, IDC_APPLICATIONCTRL, m_AppListCtrl);
	DDX_Control(pDX, IDC_AdapterList, m_AdapterList);
	DDX_Control(pDX, IDC_PrinterList, m_PrinterList);
	DDX_Text(pDX, IDC_EDIT1, m_Location);
	DDX_Text(pDX, IDC_EDIT2, m_MaxRequestSize);
	DDX_Text(pDX, IDC_EDIT3, m_PrinterClass);
	DDX_Text(pDX, IDC_EDIT4, m_AdapterIP);
	DDX_Text(pDX, IDC_EDIT5, m_AdapterType);
	DDX_Text(pDX, IDC_EDIT6, m_AdapterPort);
	DDX_Text(pDX, IDC_EDIT7, m_SuspendState);
	DDX_Text(pDX, IDC_EDIT8, m_RedirectPrinter);
	DDX_Text(pDX, IDC_EDIT9, m_Access);
	//}}AFX_DATA_MAP
}

BOOL CPrinterUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIView diagnostics

#ifdef _DEBUG
void CPrinterUIView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPrinterUIView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPrinterUIDoc* CPrinterUIView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPrinterUIDoc)));
	return (CPrinterUIDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrinterUIView message handlers
// With a form view overide this to show changes to dialog

void CPrinterUIView::OnInitialUpdate() 
{

//	CSplashWnd::ShowSplashScreen(this);


	CFormView::OnInitialUpdate();
	
	help_object.Initialize();
	
	
	GetDlgItem(IDC_Apply)->EnableWindow(FALSE);  //disable apply till we get something in ini
	
	
	//init the DLL
	
	
	if(!NtcssDLLInitialize
		(NTCSS_DLL_VERSION,
		NTCSS_APP_NAME,
		AfxGetInstanceHandle(),
		AfxGetMainWnd()->m_hWnd))
		
		::ExitProcess(0);
	
	if (NtcssGetUserRole() != 1) 
	{
		
		MessageBeep(MB_ICONSTOP);
		AfxMessageBox("Current user is not an Administrator",MB_ICONSTOP);
		::ExitProcess(0);
	} 
	
	
/*	if (!NtcssIsAdmin()) 
	{
		
		MessageBeep(MB_ICONSTOP);
		AfxMessageBox("Current user is not an NT Administrator",MB_ICONSTOP);
		::ExitProcess(0);
		
	} */
	
    //TODO go back to using the API locks 
	
	BOOL ok=NtcssAdminLockOptions(m_app.GetBufferSetLength(16),_T("00"),_T("00"),m_PID.GetBufferSetLength(8),NULL);
	
	m_app.ReleaseBuffer();
	m_PID.ReleaseBuffer();
	
	if(!ok)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox("Failed to Get Printer Lock",MB_ICONEXCLAMATION);
		::ExitProcess(0);
		
	}

		
		if(!(m_nTimer=SetTimer(1,60000,NULL))) //1 minute
		{
			AfxMessageBox(_T("Failed to Set Timer"));
			return;
		} 
		
		
		CWaitCursor cursor;
		
		
		listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		listColumn.fmt=LVCFMT_LEFT;
		listColumn.cx=100;
		listItem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;;
		
		CImageList  *pimagelist;   
		CBitmap    bitmap;
		CBitmap    bitmap2;
		CBitmap    bitmap3;
		
		// Generate the imagelist and associate with the list control
		pimagelist = new CImageList(); 
		//pimagelist->Create(16, 16, TRUE, 1, 3);
		pimagelist->Create(16, 16, TRUE, 3, 7);
		bitmap.LoadBitmap(IDB_PRINTER);  
		pimagelist->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap2.LoadBitmap(IDB_ADAPTER);  
		pimagelist->Add(&bitmap2, (COLORREF)0xFFFFFF);
		bitmap3.LoadBitmap(IDB_GROUP);  
		pimagelist->Add(&bitmap3, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
		bitmap2.DeleteObject();
		bitmap3.DeleteObject();
		
		m_PrinterList.SetImageList(pimagelist, LVSIL_SMALL);
		listItem.iImage   = 0;
		m_AdapterList.SetImageList(pimagelist, LVSIL_SMALL);
		m_AppListCtrl.SetImageList(pimagelist, LVSIL_SMALL);
		
		
		CString str_Columns="NAME,CLASSIFICATION,PRINTER ACCESS,PRINTER STATUS,QUEUEING STATUS";
		
		
		
		static long HelpTopics[]={ HT_Name_Printer,HT_Host_Printer,HT_Classification_Printer,
			HT_PrinterAccess,HT_PrinterStatus,HT_Queue_ing_Status};
		
		static long HelpTopics2[]={ HT_IPAddress,HT_Hostname_Adapter,HT_MachineAddress,
			HT_Location_Adapter,HT_Type_Adapter};
		
		
		m_PrinterList.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
			LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PUI\\MAINPRINTERLV",
			HelpTopics);
		
		str_Columns="IP ADDRESS,HOSTNAME,MACHINE ADDRESS,LOCATION,TYPE";
		listItem.iImage   = 1;
		
		m_AdapterList.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
			LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PUI\\MAINADAPTERLV",
			HelpTopics2);
		
		static long HelpTopics3[]={ HT_IPAddress,HT_Hostname_Adapter,HT_MachineAddress};
		
		//TODO get real help topics
		
		
		str_Columns="APP,PRINTER STATUS,QUEUE STATUS";
		
		listItem.iImage   = 2;
		
		m_AppListCtrl.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
			LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PUI\\MAINAPPLV",
			HelpTopics3);
		
		m_PrinterPopUp.LoadMenu(IDR_PRINTER_RC);
		m_AdapterPopUp.LoadMenu(IDR_ADAPTER_RC);

		OnRefresh();
		
		
		GetParentFrame()->RecalcLayout();
		ResizeParentToFit(FALSE);
		GetDocument()->SetTitle("NTCSS PRINTER CONFIGURATION");

	
}

void CPrinterUIView::FillAdapterListCtrl()
// called here & in edit/add dialogs to refill list ctrls from current CList Object
{
	POSITION pos=AdapterList.GetHeadPosition();
	int i=0;

	adapter_node *t2;

	m_AdapterList.DeleteAllItems();
	while (pos != NULL)
	{
	//	listItem.iItem=0;
		t2 = AdapterList.GetNext(pos);
	
		m_AdapterList.Write(t2->IPaddress);
		m_AdapterList.Write(t2->hostname);
		m_AdapterList.Write(t2->machine_address);
		m_AdapterList.Write(t2->location);
		m_AdapterList.Write(t2->type);

		
	}


		// 3/13/98 //////////////////////////////////////////////////////////
		// stick the hosts in the adapter list with the rest of the stuff N/A
		/////////////////////////////////////////////////////////////////////
		struct host_node *h;
		pos=HostList.GetHeadPosition();
	
		//i=0;
		//CString strHost;
				
        while (pos != NULL)
		{
			//listItem.iItem=0;
			h = HostList.GetNext(pos);
			if(!h->PrinterList.IsEmpty())
			{
				m_AdapterList.Write(h->IP);
				m_AdapterList.Write(h->name);
				m_AdapterList.Write("N/A");
				m_AdapterList.Write("N/A");
				m_AdapterList.Write("SERVER");

			}
		}


		if(m_AdapterList.GetItemCount()>0)
			m_AdapterList.AutoSizeColumns();

		// 3/13/98 //////////////////////////////////////////////////////////
		// stick the hosts in the adapter list with the rest of the stuff N/A
		/////////////////////////////////////////////////////////////////////
}


void CPrinterUIView::FillPrinterListCtrl()
// called here & in dialogs to refill list ctrls from current CList
{
    
	POSITION pos=PrinterList.GetHeadPosition();
	//int i=0;
//	listItem.iImage=0;
    printer_node *t;
	m_PrinterList.DeleteAllItems();
	while (pos != NULL)
	{
		//listItem.iItem=0;
		t = PrinterList.GetNext(pos);

		m_PrinterList.Write(t->name);
		//m_PrinterList.Write(t->hostname);
		m_PrinterList.Write(ini.GetClassification(t->security));
		m_PrinterList.Write(t->accessstatus==1 ? "General" : "By Application");
		m_PrinterList.Write(t->printerstatus==1 ? "Enabled" : "Disabled");
		m_PrinterList.Write(t->queuestatus==1 ? "Enabled" : "Disabled");

	}

	m_PrinterList.AutoSizeColumns();
}


BOOL CPrinterUIView::FillEditBoxes(CString as_printer_name)
{
	//5/28 changed to write bogus status in app list ctrl to start
	// and then thread off getting real status because the delay 
	// was getting bad with allot of apps
	printer_node *ptr_printer=NULL;
	adapter_node *ptr_adapter=NULL;


	POSITION pos=PrinterList.GetHeadPosition();
	while (pos != NULL)  //loop till we find printer name
	{
		  ptr_printer = PrinterList.GetNext(pos);
		  if(ptr_printer->name==as_printer_name) break;
	}

	//ASSERT(ptr_printer!=NULL);
	
	POSITION pos2=AdapterList.GetHeadPosition();
	while (pos2 != NULL)  //loop till we find adapter name
	{
		  ptr_adapter = AdapterList.GetNext(pos2);
		  if(ptr_adapter->IPaddress==ptr_printer->adapterIP) break;
	}

		
    //ASSERT(ptr_adapter!=NULL);
	
	ChangeEditBoxes(ptr_printer,ptr_adapter);

	//for now filling applist box with just apps for this printer

	m_AppListCtrl.DeleteAllItems();

	if(!ptr_printer->accessstatus)  //new feature 10-7-97
	{
		
		CStringEx strApps=ptr_printer->access;
		CString strApp;

		for(int i=0;;i++)
		{
			if((strApp=strApps.GetField(',',i))==_T(""))
				break;

			m_AppListCtrl.Write(strApp);

			//write the bogus status
			m_AppListCtrl.Write(_T("Enabled"));
			m_AppListCtrl.Write(_T("Enabled"));
		
		}

		m_AppListCtrl.AutoSizeColumns();
		
	}

	//CWinThread *thread_handle=AfxBeginThread(the_thread,this);
	SetAppPrintStatuses();

	//select first app by default this app is hooked to OnCommandUpdate
	m_AppListCtrl.SelectItem(0);
	m_strCurrentApp=m_AppListCtrl.GetText();

	//need to select appropriate adapter for default
	
	SelectAdapterInCtrl(ptr_printer->adapterIP,&m_AdapterList);

	return TRUE;
}

void CPrinterUIView::SetAppPrintStatuses()

{
	m_DoingUpdates=TRUE;
	CMapStringToString mapHosts;
	int printer_status,queue_status;
	int i=m_AppListCtrl.GetItemCount();
	CString strPrinter=m_PrinterList.GetText();
	CStringEx strAccess;
	CString strPS,strQS,strHost;
	CWaitCursor csr;
	
	for(int y=0;y<i;y++)
	{
		printer_status=queue_status=0;
		strHost=GetAppHost(m_AppListCtrl.GetItemText(y,0));

		//see if this host is already in the MAP
		if(mapHosts.Lookup(strHost,strAccess))
		{
			m_AppListCtrl.SetItemText(y,1,strAccess.GetField(',',0));
			m_AppListCtrl.SetItemText(y,2,strAccess.GetField(',',1));
		}
		else
		{
			ini.GetPrinterStatus(strPrinter,&printer_status,
			&queue_status,strHost);
			strPS=printer_status ? _T("Enabled") : _T("Disabled");
			strQS=queue_status ? _T("Enabled") : _T("Disabled");
			m_AppListCtrl.SetItemText(y,1,strPS);
			m_AppListCtrl.SetItemText(y,2,strPS);
			mapHosts.SetAt(strHost,strPS+_T(",")+strQS);
		}
	}
		m_DoingUpdates=FALSE;
}



void CPrinterUIView::SelectAdapterInCtrl(CString as_AdapterIP,PUIListCtrl* ctrl)
//this can set selection in any List Ctrl 
{
	ctrl->SelectItem(as_AdapterIP);
}


void CPrinterUIView::ChangeEditBoxes(printer_node* ptr_printer,adapter_node* ptr_adapter)
{
	//Fills in the List box values if structs aren't NULL, else fills with blanks
	
	
	m_Location=ptr_printer ? ptr_printer->location : _T(" "); 
	m_MaxRequestSize=ptr_printer ? ptr_printer->maxsize : 0;
	m_PrinterClass=ptr_printer ? ptr_printer->classname : _T(" ");
	m_AdapterIP=ptr_printer ? ptr_printer->adapterIP : _T(" ");
	m_AdapterType=ptr_adapter ? ptr_adapter->type : _T(" ");
	m_AdapterPort=ptr_printer ? ptr_printer->port : _T(" ");
	
	if (ptr_printer)
	{
		m_SuspendState=ptr_printer->suspend==1 ? _T("Permit All Requests") : _T("Suspend Requests");
		m_Access=ptr_printer->accessstatus==1 ? _T("General") : _T("By Application");
	}
	else
	{
		m_Access=_T("");
		m_SuspendState=_T("");
	}
	
	m_RedirectPrinter=ptr_printer ? ptr_printer->redirected : _T(" ");
	
	UpdateData(FALSE);
	
	if(ptr_printer) //Find out what type of printer it is cover boff baselines
	{
		CString strType;
		if(!ptr_printer->Type.IsEmpty())
			strType=ptr_printer->Type;
		else //flat files have to "figure out type"
		{
			if(!ini.IsAdapter(ptr_printer->adapterIP)) //REGULAR ADAPTER
				if(ptr_printer->port==_T("FILE")) //FILEPRINTER
					strType=_T("FILE");
				else
					if(ini.IsHostIP(ptr_printer->adapterIP)) //SERVER PRINTER
						strType=_T("SERVER");
					else
						strType=_T("NTSHARE");
		}
		
		if(strType==_T("FILE"))
		{
			GetDlgItem(IDC_EDIT5)->SetWindowText(_T("FILE"));
			GetDlgItem(IDC_EDIT6)->SetWindowText(ptr_printer->FileName);
		}
		else
			if(strType==_T("SERVER"))
				GetDlgItem(IDC_EDIT5)->SetWindowText(_T("SERVER"));
			else
				if(strType==_T("NTSHARE"))
				{
					GetDlgItem(IDC_EDIT4)->SetWindowText(ptr_printer->adapterIP);
					GetDlgItem(IDC_EDIT5)->SetWindowText(_T("NTSHARE"));
					GetDlgItem(IDC_EDIT6)->SetWindowText(ptr_printer->port);
				}
	}
}


void CPrinterUIView::OnApply() 
{
	if(ini.PutIniFile())
	{
		m_GotLog=TRUE;
		GetDlgItem(IDC_Apply)->EnableWindow(FALSE); 
		//new stuff to reread printer & adapter list after put ini
		if (ini.GetIniFile()) 
		{
			m_AppListCtrl.DeleteAllItems();
			ChangeEditBoxes(NULL,NULL);
			///////////////////////////////////////////////
			
			POSITION posCurrent;
			
			printer_node* p;
			adapter_node* a;
			
			posCurrent=PrinterList.GetHeadPosition();
			
			while(posCurrent!=NULL)
			{
				p=PrinterList.GetNext(posCurrent);
				delete p;
			}
			
			posCurrent=AdapterList.GetHeadPosition();
			
			while(posCurrent!=NULL)
			{
				a=AdapterList.GetNext(posCurrent);
				delete a;
			}
			
			AdapterList.RemoveAll(); 
			PrinterList.RemoveAll();
			///////////////////////////////////////////////
			m_AdapterList.DeleteAllItems();
			m_PrinterList.DeleteAllItems();
		
			if (ini.BuildAdapterList()) //Build adapter CList
				FillAdapterListCtrl();

			if (ini.BuildPrinterList()) //Fill in Network Printer List
				FillPrinterListCtrl();
		}
		else
			AfxMessageBox(_T("Error getting New Ini File"));
		//end of new stuff
	}
	else
	{
		AfxMessageBox(_T("Error occured during processing! Check Log File"));
		m_GotLog=FALSE;
		
	}
}


void CPrinterUIView::OnAdapterDelete() 
{

	int nIndex = m_AdapterList.GetNextItem( -1, LVNI_SELECTED );
	ASSERT(nIndex>=0);
	CString strItem=m_AdapterList.GetItemText(nIndex,0);

	m_AdapterList.DeleteItem(nIndex);          //del from ctrl
	ini.DelAdapterInAdapterList(strItem);      //del from CList

	GetDlgItem(IDC_Apply)->EnableWindow(TRUE); 
}


void CPrinterUIView::OnUpdateAdapterDelete(CCmdUI* pCmdUI) 

{
	pCmdUI->Enable(m_AdapterList.GetNextItem( -1, LVNI_SELECTED )>=0 &&
	m_AdapterList.GetText(4)!=_T("SERVER") && !ini.FindAdapterInPrintList
	(m_AdapterList.GetText()));
	
}
///////////////////////////////////////////////////////////////////////////////
CString CPrinterUIView::GetSelectedItem(CListCtrl* ctrl, int item)
//this is used to get current selection from any list ctrl
//put in ini later then pass pointers to ctrls to add & edit
//dialogs. Item chooses which field
{
	CString strItem;
	int nIndex = ctrl->GetNextItem( -1, LVNI_SELECTED );
	if (nIndex>=0)
		strItem=ctrl->GetItemText(nIndex,item);
	return strItem;
}

///////////////////////////////////////////////////////////////////////////////
void CPrinterUIView::OnPrintDelete() 
{
	printer_node* ptr_printer;

	int nIndex = m_PrinterList.GetNextItem( -1, LVNI_SELECTED );

	if (nIndex>=0)
	{   
		//Get name of selected Printer
		CString strItem=m_PrinterList.GetItemText(nIndex,0);
		CString temp;

		temp.Format(_T("Delete Printer %s"),strItem);
		if((AfxMessageBox(temp,MB_YESNO|MB_ICONQUESTION))==IDNO)
			return;

			              
		POSITION pos=PrinterList.GetHeadPosition();
	
		
		while (pos != NULL)
		{
			ptr_printer = PrinterList.GetNext(pos);
			if(ptr_printer->name==strItem)
		    break;
			 
		} 
			
        
		if(!ptr_printer->OT.IsEmpty())	   //TR10172/73
		{
			MessageBeep(MB_ICONEXCLAMATION );
			AfxMessageBox(_T("Associated Output types must be removed First!"));
			return;
		}

		
		//Delete From List
		

		PrinterList.RemoveAt(PrinterList.Find(ptr_printer)); 

		//del from ctrl

		
	
		m_PrinterList.DeleteItem(nIndex);

							
		///write in output
        ini.OutputPrinterNode(_T("DELETE"),ptr_printer);

		GetDlgItem(IDC_Apply)->EnableWindow(TRUE);
		
		//delete node
		delete ptr_printer;

		//now clear App List box
		
		//m_AppList.ResetContent();
		m_AppListCtrl.DeleteAllItems();

		ChangeEditBoxes(NULL,NULL);

	}
	
}

void CPrinterUIView::OnPrintAdd() 
{
    CEditPrinter dlg(NULL,NULL);
	if(dlg.DoModal()==IDOK)
		m_AppListCtrl.DeleteAllItems( );

}


void CPrinterUIView::OnPrintEdit() 
{   
  	CEditPrinter dlg(NULL,ini.FindPrinterInPrintList(GetSelectedItem(&m_PrinterList)));
	dlg.DoModal();
}


void CPrinterUIView::OnAdapterAdd() 
{

	CEditAdapter dlg(NULL,_T("ADD"));
	dlg.DoModal();	
	
}

void CPrinterUIView::OnAdapterEdit() 
{

	CEditAdapter dlg;
	dlg.DoModal();
	
}

void CPrinterUIView::OnUpdateAdapterEdit(CCmdUI* pCmdUI) 
//Enable Disable Print Edit Menu item based on if Pinter selected

{
	pCmdUI->Enable(m_AdapterList.GetNextItem( -1, LVNI_SELECTED )>=0 &&
	m_AdapterList.GetText(4)!=_T("SERVER"));
}


void CPrinterUIView::OnUpdatePrintEdit(CCmdUI* pCmdUI) 
//Enable Disable Print Edit Menu item based on if Pinter selected

{
	pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0);
}

void CPrinterUIView::OnUpdatePrintDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0);
}

void CPrinterUIView::OnStatusHaltprinter() 
{
	if(ini.SetPrinterStatus(GetSelectedItem(&m_PrinterList),
							GetAppHost(m_AppListCtrl.GetText()),"0",
							(m_Queue_Enabled ? "1" : "0")))
	{
		m_Printer_Enabled=FALSE;
		m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      3,_T("Disabled"));
		m_AppListCtrl.SetText(_T("Disabled"),1);
	}
}

void CPrinterUIView::OnUpdateStatusHaltprinter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0 && m_Printer_Enabled &&
			   m_AppListCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);

}

void CPrinterUIView::OnStatusEnableprinter() 
{
	if(ini.SetPrinterStatus(GetSelectedItem(&m_PrinterList),
							GetAppHost(m_AppListCtrl.GetText()),"1",
							(m_Queue_Enabled ? "1" : "0")))
	{
		m_Printer_Enabled=TRUE;
		m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      3,_T("Enabled"));
		m_AppListCtrl.SetText(_T("Enabled"),1);
	}

}

void CPrinterUIView::OnUpdateStatusEnableprinter(CCmdUI* pCmdUI) 
{
		
	pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0 && !m_Printer_Enabled &&
			   m_AppListCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);
	
}

void CPrinterUIView::OnStatusEnablequeueing() 
{
	if(ini.SetPrinterStatus(GetSelectedItem(&m_PrinterList),
							GetAppHost(m_AppListCtrl.GetText()),
							(m_Printer_Enabled ? "1" : "0"),"1"))
	{
		m_Queue_Enabled=TRUE;
   		m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      4,_T("Enabled"));
		m_AppListCtrl.SetText(_T("Enabled"),2);
	}
   
}

void CPrinterUIView::OnUpdateStatusEnablequeueing(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0 && !m_Queue_Enabled &&
			   m_AppListCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);

	
}

void CPrinterUIView::OnStatusDisablequeueing() 
{
	if(ini.SetPrinterStatus(GetSelectedItem(&m_PrinterList),
							GetAppHost(m_AppListCtrl.GetText()),
							(m_Printer_Enabled ? "1" : "0"),"0"))
	{
		m_Queue_Enabled=FALSE;
		m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      4,_T("Disabled"));
		m_AppListCtrl.SetText(_T("Disabled"),2);
	} 

}

void CPrinterUIView::OnUpdateStatusDisablequeueing(CCmdUI* pCmdUI) 
{
pCmdUI->Enable(m_PrinterList.GetNextItem( -1, LVNI_SELECTED )>=0 && m_Queue_Enabled &&
			   m_AppListCtrl.GetNextItem( -1, LVNI_SELECTED )>=0);
	
}


void CPrinterUIView::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(0);
	
}

void CPrinterUIView::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_GotLog);
	
}

void CPrinterUIView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_GotLog);
	
}

void CPrinterUIView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(0);
	
}

void CPrinterUIView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(0);
	
}

void CPrinterUIView::OnUpdateFileMruFile1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(0);
	
}


void CPrinterUIView::OnFileOpen() 
{

CLogFileDialog dlg;
int ret=dlg.DoModal();
	
}


void CPrinterUIView::OnTimer(UINT nIDEvent) 
{
	NtcssAdminLockOptions(m_app,PUI_Lock,RenewLock,m_PID.GetBufferSetLength(8),NULL);
	return;
}


void CPrinterUIView::OnPUIQuit() 
{
	if(GetDlgItem(IDC_Apply)->IsWindowEnabled()) //warn if there is unapplied log file
		if(AfxMessageBox("Quit Without Saving Changes",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			return;

/////////////////////test multithreading////////////////////////////////
/*{
	HANDLE hThreads[5];
	CWinThread *thread_handle;
	TRACE0("Creating threads\n");
	for(int i=0;i<5;i++)
	{
		hThreads[i]=thread_handle->m_hThread;
		thread_handle=AfxBeginThread(the_thread,this);
	}

	 //start the action
    SetEvent(g_hEvent);

	WaitForMultipleObjects(5,hThreads,TRUE,INFINITE);
	TRACE0("Done\n");
    TRACE0("Closing handles\n");
	for(i=0;i<5;i++)
		if(hThreads[i])
			CloseHandle(hThreads[i]); 
}*/
////////////////////////test multithreading///////////////////////////////

	POSITION posCurrent;

	printer_node* p;
	adapter_node* a;
	adapter_type_node* at;
	host_node* h;

	posCurrent=PrinterList.GetHeadPosition();

	while(posCurrent!=NULL)
	{
		p=PrinterList.GetNext(posCurrent);
		delete p;
	}

	posCurrent=AdapterList.GetHeadPosition();

	while(posCurrent!=NULL)
	{
		a=AdapterList.GetNext(posCurrent);
		delete a;
	}


	posCurrent=AdapterTypeList.GetHeadPosition();

	while(posCurrent!=NULL)
	{
		at=AdapterTypeList.GetNext(posCurrent);
		delete at;
	}


    posCurrent=HostList.GetHeadPosition();

	while(posCurrent!=NULL)
	{
		h=HostList.GetNext(posCurrent);
		delete h;
	}


	CWnd* dlg=(CWnd*) AfxGetMainWnd();
	dlg->DestroyWindow();
}

void CPrinterUIView::OnClickPrinterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  
	CString strItem=GetSelectedItem(&m_PrinterList);
	CString strAppName;

	if (!strItem.IsEmpty())
	{
		m_strCurrentPrinter=strItem;
		FillEditBoxes(strItem); 
	}
	else
		m_PrinterList.SelectItem(m_strCurrentPrinter); //stop them from deslecting all


	*pResult = 0;
}

BOOL CPrinterUIView::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CFormView::PreTranslateMessage(pMsg);
}


void CPrinterUIView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

CString CPrinterUIView::GetAppHost(CString strApp)
{
	TCHAR strHostname[SIZE_HOST_NAME];
	NtcssGetAppHost(strApp,strHostname,SIZE_HOST_NAME);
//	ASSERT(strlen(strHostname)>0);
	return strHostname;
}

void CPrinterUIView::OnDblclkPrinterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_PrinterList.GetText()==_T("")) //stop them from selecting nothing
		m_PrinterList.SelectItem(m_strCurrentPrinter); 

	OnPrintEdit();
	
	*pResult = 0;
}


void CPrinterUIView::OnItemchangedApplicationctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->uNewState & LVIS_SELECTED && !m_DoingUpdates)
		if(m_AppListCtrl.GetText()!=_T(""))
		{
		
			m_Queue_Enabled=m_AppListCtrl.GetText(1)== _T("Enabled");
			m_Printer_Enabled=m_AppListCtrl.GetText(2)== _T("Enabled");

			m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      3,m_AppListCtrl.GetText(1));
			m_PrinterList.SetItemText(m_PrinterList.GetNextItem( -1, LVNI_SELECTED ),
			                      4,m_AppListCtrl.GetText(2));

		}
		else
			m_AppListCtrl.SelectItem(m_strCurrentApp);	
	
	*pResult = 0;
}


void CPrinterUIView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if((PUIListCtrl*)pWnd!=&m_PrinterList && (PUIListCtrl*)pWnd!=&m_AdapterList)
		return;

	CMenu* pSubMenu;

	if((PUIListCtrl*)pWnd==&m_PrinterList)
		pSubMenu=m_PrinterPopUp.GetSubMenu(0);
	else
		pSubMenu=m_AdapterPopUp.GetSubMenu(0);

    //do pop up
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd(),
		NULL);
	
}

void CPrinterUIView::OnRefresh() 
{

	CWaitCursor cursor;
	if(ini.GetIniFile())
	{
		ini.BuildHostList();

		if(ini.BuildAdapterList()||HostList.GetCount()) //Build adapter CList
			FillAdapterListCtrl();

		if(ini.BuildPrinterList())
			FillPrinterListCtrl();
		
		ini.BuildAdapterTypeList();
		ini.BuildCStringList(_T("APPS"),&AppsList);
		ini.BuildCStringList(_T("PRINTCLASS"),&PTList); 
	}
	
}


/*UINT CPrinterUIView::the_thread(LPVOID param)
{
	WaitForSingleObject(g_hEvent, INFINITE);
		
	TRACE0("Thread is running\n");
	CString host=((CPrinterUIView*) param)->GetAppHost("Ntcss"); 
	TRACE1("Thread is done returned-> %s\n",host);
	return 0;
}*/


void CPrinterUIView::OnDblclkAdapterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_AdapterList.GetText()!=_T("")&&
	m_AdapterList.GetText(4)!=_T("SERVER")) 
		OnAdapterEdit();
	*pResult = 0;
}

