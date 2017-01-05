/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// EditPrinter.cpp : implementation file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"
#include "genutils.h"
#define  theman
#include "ini.h"
#include "PrinterUI.h"
#include "resource.h"
#include "EditPrinter.h"
#include "PrinterUIDoc.h"
#include "PrinterUIView.h"
#include "MainFrm.h"
#include "RedirectDialog.h"
#include "AddNTPrinter.h"
#include "Print2FileDlg.h"
#include "PUIHelp.h"
#include "StringEx.h"
#include "msgs.h"
#include "SelectShareDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDV_CheckPath2(CDataExchange* pDX, int nIDC,CString& strPath)
{
/*	HWND hWndCtrl=pDX->PrepareEditCtrl(nIDC);
	if(pDX->m_bSaveAndValidate)
		if(!strPath.IsEmpty())
		{
			CStringEx temp=strPath;
			temp.FindReplace("\\","/");	
			CmsgGETDIRLISTING msg;
			if(!(msg.Load(ini.m_hostname,(LPCTSTR)
					temp.Mid(0,temp.ReverseFind('/'))) && msg.DoItNow()))
			{
				AfxMessageBox("Bad Path Name specified");
				DDX_Text(pDX, nIDC, strPath);   
				pDX->Fail(); 
			
			}
			strPath=temp;
		} */
}

/////////////////////////////////////////////////////////////////////////////
// CEditPrinter dialog


CEditPrinter::CEditPrinter(CWnd* pParent /*=NULL*/,printer_node* selected_printer)
	: CDialog(CEditPrinter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditPrinter)
	m_Name = _T("");
	m_Classification = _T("");
	m_Location = _T("");
	m_PrinterClass = _T("");
	m_PrinterAccess = _T("");
	m_AdapterPort = _T("");
	m_RedirectPrinter = _T("");
	m_SuspendState = _T("");
	m_MaxRequestedSize = 0;
	m_adapterIP = _T("");
	m_NTDriver = _T("");
	m_FileName = _T("");
	m_nAppend = 0;
	m_HostName = _T("");
	//}}AFX_DATA_INIT
	m_saved_printer=selected_printer;
	m_nCurrentListViewColumn=-1;
	m_nAppend=-1; //FILE STUFF
	m_bAtlass=FALSE;
	
}


void CEditPrinter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPrinter)
	DDX_Control(pDX, IDC_FILENAME, m_FileNameCtrl);
	DDX_Control(pDX, IDC_LOCATION, m_LocationCtrl);
	DDX_Control(pDX, IDC_NAME, m_CEditCtrlName);
	DDX_Control(pDX, IDC_NTBUTTON, m_NTBUTTON);
	DDX_Control(pDX, IDC_SUSPENDSTATE, m_SuspendStateCtrl);
	DDX_Control(pDX, IDC_ADAPTERPORT, m_AdapterPortCtrl);
	DDX_Control(pDX, IDC_PRINTERCLASS, m_PrinterClassCtrl);
	DDX_Control(pDX, IDC_PrinterAccess, m_PrinterAccessCtrl);
	DDX_Control(pDX, IDC_CLASSIFICATION, m_ClassNameCtrl);
	DDX_Control(pDX, IDC_APPLICATION_LIST, m_AppList);
	DDX_Control(pDX, IDC_PrinterAdapterList, m_AdapterList);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_CBString(pDX, IDC_CLASSIFICATION, m_Classification);
	DDX_Text(pDX, IDC_LOCATION, m_Location);
	DDX_CBString(pDX, IDC_PRINTERCLASS, m_PrinterClass);
	DDX_CBString(pDX, IDC_PrinterAccess, m_PrinterAccess);
	DDX_CBString(pDX, IDC_ADAPTERPORT, m_AdapterPort);
	DDX_Text(pDX, IDC_REDIRECTPRINTER, m_RedirectPrinter);
	DDX_CBString(pDX, IDC_SUSPENDSTATE, m_SuspendState);
	DDX_Text(pDX, MAXREQUESTSIZE, m_MaxRequestedSize); 
	DDX_Text(pDX, IDC_NTDRIVER, m_NTDriver);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_Radio(pDX, IDC_OVERWRITE, m_nAppend);
	//}}AFX_DATA_MAP
	DDV_MaxChars(pDX, m_FileName, _MAX_PATH);
	if(m_FileNameCtrl.IsWindowVisible())
	DDV_CheckPath2(pDX, IDC_FILENAME,m_FileName);
}


BEGIN_MESSAGE_MAP(CEditPrinter, CDialog)
	//{{AFX_MSG_MAP(CEditPrinter)
	ON_BN_CLICKED(IDC_REDIRECT, OnRedirect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PrinterAdapterList, OnItemchangedPrinterAdapterList)
	ON_BN_CLICKED(IDC_NTBUTTON, OnNtbutton)
	ON_CBN_SELCHANGE(IDC_PrinterAccess, OnSelchangePrinterAccess)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_PrinterAdapterList, OnClickPrinterAdapterList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CEditPrinter::FillListBox(CListBox* pBox,CStringList* List) 
//use this to fill all the ListBoxes filled with CStringList lists

{
	
		CString tmpString;
		pBox->ResetContent();

		POSITION pos=List->GetHeadPosition();
		while (pos !=NULL)
		{
			tmpString=List->GetNext(pos);
			if(pBox->FindString(-1,tmpString)==LB_ERR ) //TR10258
				pBox->AddString(tmpString);
		}
}

void CEditPrinter::FillListBox(CComboBox* pBox,CStringList* List) 
//overloaded for combo boxes

{
	
		CString tmpString;
		pBox->ResetContent();

		POSITION pos=List->GetHeadPosition();
		while (pos !=NULL)
		{
			tmpString=List->GetNext(pos);
			if(pBox->FindString(-1,tmpString)==LB_ERR ) //TR10258
				pBox->AddString(tmpString);
		}
}


/////////////////////////////////////////////////////////////////////////////
// CEditPrinter message handlers

BOOL CEditPrinter::OnInitDialog() 
{
	CDialog::OnInitDialog();


	LV_COLUMN listColumn;
    LV_ITEM listItem;
	listColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	listColumn.fmt=LVCFMT_LEFT;
	listColumn.cx=100;
	listItem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;;
	listItem.iSubItem=0;
	listItem.iItem=0;

	//***********************************************************************
		
		CImageList  *pimagelist;   
		CBitmap    bitmap;
		

	   // Generate the imagelist and associate with the list control
		pimagelist = new CImageList(); 
		pimagelist->Create(16, 16, TRUE, 1, 3);
		bitmap.LoadBitmap(IDB_ADAPTER);  
		pimagelist->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
				
		m_AdapterList.SetImageList(pimagelist, LVSIL_SMALL);
		listItem.iImage   = 0;

	//***********************************************************************

	
	 ///////////////////////////////////////////////////////////////////////	
	 CString str_Columns="DEVICE ID,MACHINE ADDRESS,LOCATION,TYPE";

	 static long HelpTopics[]={ HT_IPAddress,HT_MachineAddress,HT_Location_Adapter,
							    HT_Type_Adapter};

	m_AdapterList.SetUp(listColumn,listItem,str_Columns,LVS_EX_HEADERDRAGDROP|
		             LVS_EX_FULLROWSELECT,"Software\\NTCSS\\PUI\\EDITADAPTER",
					 HelpTopics);
	////////////////////////////////////////////////////////////////////////
	

	//Put Columns in Network Printer Report Ctrl

		//Fill Adapter List Ctrl
		
		POSITION pos=AdapterList.GetHeadPosition();
		//i=0;
        struct adapter_node *t;
		while (pos != NULL)
		{
			//listItem.iItem=0;
			t = AdapterList.GetNext(pos);
			//put new func here to suppress any 100% full adapter unless it's
			//associated with currently selected printer
			/////////////////////////////////////////////////////////////////
			BOOL ok;
			if(m_saved_printer!=NULL&&m_saved_printer->adapterIP==t->IPaddress)
				ok=TRUE;
			else
				ok=TRUE;// 1/9/98 replace this with line below
		//		ok=!ini.AllPortsUsed(t->IPaddress,t->type); //jj 1/9 took out for NAVMASO put back
			/////////////////////////////////////////////////////////////////
			if(ok)
			{
				m_AdapterList.Write(t->IPaddress);
				m_AdapterList.Write(t->machine_address);
				m_AdapterList.Write(t->location); 
				m_AdapterList.Write(t->type);


			}
		

		}

		// 3/13/98 //////////////////////////////////////////////////////////
		// stick the hosts in the adapter list with the rest of the stuff N/A
		/////////////////////////////////////////////////////////////////////
		struct host_node *h;
		pos=HostList.GetHeadPosition();
	//	i=0;
		CString strHost;
        while (pos != NULL)
		{
			listItem.iItem=0;
			h = HostList.GetNext(pos);
			if(!h->PrinterList.IsEmpty())
			{
		
				m_AdapterList.Write(h->name);
				m_AdapterList.Write("N/A");
				m_AdapterList.Write("N/A");
				m_AdapterList.Write("SERVER");
				
			}
		}

	
		// 4/16/98 //////////////////////////////////////////////////////////
		// stick the FILE in the adapter list with the rest of the stuff N/A
		/////////////////////////////////////////////////////////////////////
	
		m_AdapterList.Write("FILE");
		m_AdapterList.Write("N/A");
		m_AdapterList.Write("N/A");
		m_AdapterList.Write("FILE");

		// 12/14/98 //////////////////////////////////////////////////////////
		// stick the NTSHARE in the adapter list with the rest of the stuff N/A
		/////////////////////////////////////////////////////////////////////
		m_AdapterList.Write("NTSHARE");
		m_AdapterList.Write("N/A");
		m_AdapterList.Write("N/A");
		m_AdapterList.Write("NTSHARE");

         ////////////////////////////////////////////////////
		//Fill In List Boxes from CList Objects
		if(!m_saved_printer/*add*/
		   ||!m_saved_printer->accessstatus) //10-7-97 new feature 
		   FillListBox(&m_AppList,&AppsList);      //Application List
		//FillListBox(&m_HostNameCtrl,&HostList);    //Hostname List
		FillListBox(&m_PrinterClassCtrl,&PTList);  //Printer Type List 3/13/1998


		//Classification List
		for (int i=0;i<5;i++)
			m_ClassNameCtrl.AddString(ini.GetClassification(i));

		//Printer Access List
		m_PrinterAccessCtrl.AddString("By Application");
		m_PrinterAccessCtrl.AddString("General");
		m_SuspendStateCtrl.AddString("Suspend All Requests");
		m_SuspendStateCtrl.AddString("Permit All Requests");
		//now if operation is Edit set evreything on current values
		if (m_saved_printer)
		{
			GetDlgItem(IDC_NAME)->EnableWindow(FALSE);  //disable name on edit
//			m_HostNameCtrl.EnableWindow(FALSE);  //disable hostname on edit TR10141
			SetUpForEdit();
			help_object.SetWindowHelp(HT_EditNTCSSPrinter_WindowDescription);
			if(m_saved_printer->port==_T("FILE")) //TR 10236
			GetDlgItem(IDC_REDIRECT)->EnableWindow(FALSE);
		}
		else
		{  //add - change title
			SetWindowText("Add NTCSS Printer");
			help_object.SetWindowHelp(HT_AddNTCSSPrinter_WindowDescription);
			SetUpForAdd();
		}

		
		return TRUE;  
}

void CEditPrinter::SetUpForAdd()
{
	GetDlgItem(IDC_ADAPTERPORT)->EnableWindow(FALSE);
	//m_HostNameCtrl.SelectString(-1,ini.m_hostname);
	m_ClassNameCtrl.SelectString(-1,"UNCLASSIFIED");
	m_PrinterAccessCtrl.SelectString(-1,"By Application");
	m_PrinterClassCtrl.SelectString(-1,"POST SCRIPT");
	m_SuspendStateCtrl.SelectString(-1,"Suspend All Requests");
}

void CEditPrinter::SetUpForEdit()
//Sets all list box data to match currently selected printer
//fills edit boxes with current selection data
//TODO: probably can be allot easier if I use the type which even exists for flat file now
{
	//Get pointer to view to get to Printer List Ctrl
	CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
	ASSERT(pview!=NULL);

	//Save old access so we can detect deletes
	m_saved_printer->OldAccess = m_saved_printer->access;

	printer_node* ptr_printer=ini.FindPrinterInPrintList(m_saved_printer->name);//Get node from CList
	
//	m_HostNameCtrl.SelectString(-1,ptr_printer->hostname);
	m_ClassNameCtrl.SelectString(-1,ini.GetClassification(ptr_printer->security));
	m_PrinterAccessCtrl.SelectString(-1,ptr_printer->accessstatus==1 ? "General" : "By Application");
	m_PrinterClassCtrl.SelectString(-1,ptr_printer->classname);
	m_SuspendStateCtrl.SelectString(-1,ptr_printer->suspend==1 ? "Permit All Requests" : "Suspend All Requests");
	
	
	//Select Applist as it is in current printer
	if(!ptr_printer->accessstatus) 
	{
		char save[512];
		strcpy(save,ptr_printer->access);  
		char *token = strtok((char*)(LPCTSTR) ptr_printer->access, "," );
		int i;
		while( token != NULL )
		{
			if((i=m_AppList.FindString(-1,token))>-1) //SelectString doesn't work in multibox
			m_AppList.SetSel(i,TRUE);
			token = strtok( NULL, "," );
		}
		ptr_printer->access=save;
	}


	//Atlass stuff
	if(ptr_printer->port==_T("FILE"))
	{
		m_bAtlass=TRUE;
		m_nAppend=ptr_printer->nAppend;
		m_FileName=ptr_printer->FileName;
		ShowFileInfo();
	}
	
	//Select right adapter from adapter control using view member function
	if(ptr_printer->port==_T("FILE")) //Atlas
		m_AdapterList.SelectItem(_T("FILE"));
	else
		m_AdapterList.SelectItem(ptr_printer->adapterIP); //blowing up here

	int nIndex = m_AdapterList.GetNextItem( -1, LVNI_SELECTED );

	//Set correct ports for adapter

	
	SetPorts(m_AdapterList.GetItemText(nIndex,0),m_AdapterList.GetItemText(nIndex,3));
	//9-25-97 TR added this line to put the current port in LB
	if(ptr_printer->port!="FILE") //Atlas added just this if addstring was here
		m_AdapterPortCtrl.AddString(ptr_printer->port);
	//9-25-97 TR

	 //4/6/99 detect that it's an NT SHARE & put it in LC & select it
		if(!ini.IsAdapter(ptr_printer->adapterIP)&&ptr_printer->port!=_T("FILE")) //need to exclude fileprinter
		{
			m_AdapterList.Write(ptr_printer->port);
			m_AdapterList.Write(ptr_printer->adapterIP);
			m_AdapterList.Write(ptr_printer->location);
			if(ptr_printer->Type==_T("NTSHARE"))
				m_AdapterList.SelectItem(m_AdapterList.Write(_T("NTSHARE")));
			else
				m_AdapterList.SelectItem(m_AdapterList.Write(_T("SERVER")));
			m_AdapterPortCtrl.ResetContent();
			m_AdapterPortCtrl.AddString(ptr_printer->port);
			m_AdapterPortCtrl.EnableWindow(FALSE);

		}


	m_AdapterPortCtrl.SelectString(-1,ptr_printer->port); //Select current

	//Fill in Edit Boxes
	m_Name=ptr_printer->name; 
	m_Location=ptr_printer->location; 
	m_MaxRequestedSize=ptr_printer->maxsize;
	m_RedirectPrinter=ptr_printer->redirected; 
	m_NTDriver=ptr_printer->NTDriver;
	m_adapterIP=ptr_printer->adapterIP;


	UpdateData(FALSE);
}

void CEditPrinter::SwapPrinterData(printer_node *ptr_printer)
{
//used by add & delete to put dialog data into the node passed in
	
	//AfxMessageBox("In Swap data");
	ptr_printer->name=m_Name;
	ptr_printer->hostname=m_HostName;
	ptr_printer->location=m_Location;
	ptr_printer->maxsize=m_MaxRequestedSize;
	ptr_printer->classname=m_PrinterClass;
	ptr_printer->security=ini.PutClassification(m_Classification);
  	ptr_printer->suspend=m_SuspendState==_T("Permit All Requests") ? 1 :0;
	ptr_printer->accessstatus=m_PrinterAccess==_T("General") ? 1 :0;
	ptr_printer->port=m_AdapterPort;
	ptr_printer->redirected=m_RedirectPrinter.IsEmpty() ? "None" : m_RedirectPrinter;
	ptr_printer->NTDriver=m_NTDriver;//new
	//AfxMessageBox("In Swap filename-> " + m_FileName);
	ptr_printer->nAppend=m_nAppend;  //FILE STUFF
	ptr_printer->FileName=m_FileName;//FILE STUFF

     //IP is used 4 different ways
	 //put in type for new extract

	if(ini.IsAdapter(m_AdapterList.GetText())) //REGULAR PRINTER
	{
		ptr_printer->adapterIP=m_AdapterList.GetText();
		ptr_printer->Type=_T("REGULAR");

	}
	else
		if(m_AdapterPort==_T("FILE")) //FILEPRINTER
		{
			ptr_printer->adapterIP=_T("NONE");
			ptr_printer->Type=_T("FILE");
		}
		else
			if(!ini.IsAdapter(m_AdapterList.GetText()) //NTSHARE
				&&m_AdapterList.GetText(3)==_T("NTSHARE")) 
             
			{
				ptr_printer->adapterIP=m_adapterIP;
				ptr_printer->Type=_T("NTSHARE");
			}
		
			else //SERVER PRINTER
			{
				ptr_printer->adapterIP=ini.GetHostIP(m_AdapterList.GetText());
				ptr_printer->Type=_T("SERVER");
			}

	
	//Now deal with checking for changes in applist
	
	if(m_PrinterAccess==_T("General"))      //new feature 
		ptr_printer->access.Empty();   //if changed access to Gen wack access list
	else
	{

		int nSelected=m_AppList.GetSelCount();
		if (nSelected>0)
		{
			int *pArray=new int[nSelected];
			m_AppList.GetSelItems( nSelected, (LPINT)pArray ); 
			CString temp,buf;
			m_AppList.GetText(pArray[0],temp);

			for (int i=1;i<nSelected;i++)
			{
				m_AppList.GetText(pArray[i],buf);
				temp+=","+buf;			
			}

			delete [] pArray;
			ptr_printer->access=temp;
		}
		else
			ptr_printer->access.Empty();
	}
}


BOOL CEditPrinter::CheckChanges()
//performs minimal error checking
{
	CString Message;
	BOOL ok=TRUE;

	if (!m_saved_printer) //do only on add
	{
		if (m_Name.Find(' ')>0)
		{
			Message = _T("Printer Name can't contain spaces");
			ok = FALSE;
		}
		if(ok)
		{
			char *p=(char*)(LPCTSTR)m_Name;
			//BOOL good=TRUE;
			while(*p)
			{
				if(!isalpha(*p)&&!isdigit(*p)&&*p!='_')
				{
					Message="Invalid Printer Name";
			        ok=FALSE;
					break;
				}
				p++;
			}
		}


		if(ok)
			if(m_Name.GetLength()<MIN_PRINTER_NAME||m_Name.GetLength()>MAX_PRINTER_NAME)
			{
				Message.Format("Printer Name must be between %d & %d Characters",
								MIN_PRINTER_NAME,MAX_PRINTER_NAME);
				ok=FALSE;
			}
		if(ok)
		{
			CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
			ASSERT(pview!=NULL);
			LV_FINDINFO ls_findinfo;
			ls_findinfo.flags=LVFI_STRING;
			ls_findinfo.psz=(LPCTSTR)m_Name;
			if(pview->m_PrinterList.FindItem(&ls_findinfo,-1)!=-1)
			{
				Message="Printer Name already exists";
				ok=FALSE;
			} 
		}
	}
	if (ok && m_NTDriver.IsEmpty()) //TR ##### 9-6-97
	{
		Message.Format("Must specify an NT driver file");
		ok=FALSE;
	}


	if(ok && m_Location.GetLength()<MIN_LOCATION||m_Location.GetLength()>MAX_LOCATION)
	{
		
		CString temp;
		Message.Format("Location must be between %d & %d Characters",MIN_LOCATION,
			         MAX_LOCATION);
		ok=FALSE;
	
	}
	if (ok && m_PrinterClass.IsEmpty())
	{
		Message.Format("Printer Class can't be empty");
		ok=FALSE;
	} 
	if (ok && m_Classification.IsEmpty())
	{
		Message.Format("Classification can't be empty");
		ok=FALSE;
	} 
	if (ok && m_AdapterPort.IsEmpty())
	{
		Message.Format("Port can't be empty");
		ok=FALSE;
	} 
	
	if(!ok)
	{
		MessageBeep(MB_ICONEXCLAMATION );
		AfxMessageBox(Message,MB_ICONEXCLAMATION);
	}
	return ok;
}

BOOL CEditPrinter::PrinterChanged()


{
   
 	//UpdateData(TRUE);
	
	// Check all single selection items

	if (m_Name!=m_saved_printer->name) return TRUE;
//	if (m_HostName!=m_saved_printer->hostname) return TRUE;
	if (m_Location!=m_saved_printer->location) return TRUE;
	if (m_MaxRequestedSize!=m_saved_printer->maxsize) return TRUE;
	if (m_PrinterClass.CompareNoCase (m_saved_printer->classname)!=0) return TRUE;
	if (m_Classification!=ini.GetClassification(m_saved_printer->security)) return TRUE;
    if (m_SuspendState!=(m_saved_printer->
		suspend==1 ?  "Permit All Requests" : "Suspend All Requests")) return TRUE;
	if (m_PrinterAccess!=(m_saved_printer->
		accessstatus==1 ?  "General" : "By Application")) return TRUE;
	if (m_AdapterPort!=m_saved_printer->port) return TRUE;
	if (m_RedirectPrinter!=m_saved_printer->redirected) return TRUE;
	if (m_NTDriver!=m_saved_printer->NTDriver) return TRUE;//05_13
	if (m_nAppend!=m_saved_printer->nAppend) return TRUE;//FILE STUFF
	if (m_FileName!=m_saved_printer->FileName) return TRUE;//FILE STUFF

	if (CheckOverloadChanges())
		return TRUE;

	//call new routine here that will see if printer changed
	//based on overloaded fields
	
	//Now deal with checking for changes in applist
	int nSelected=m_AppList.GetSelCount(),size=0;
	BOOL changed=FALSE;
	if (nSelected>0)
	{
		int *pArray=new int[nSelected];
		m_AppList.GetSelItems( nSelected, (LPINT)pArray ); 
		CString temp;
		for (int i=0;i<nSelected;i++)
		{
			m_AppList.GetText(pArray[i],temp);
			size+=temp.GetLength();
			if(m_saved_printer->access.Find(temp)==-1) 
			{
				changed=TRUE;
				break;
			}
						
		}

		if (!changed)//taking size of indiv app names adding comma's & comparing to applist
		{            //in other words will detect if app was deleted in edit control
			if ((size+nSelected)-1 != m_saved_printer->access.GetLength())
				changed=TRUE;
		}

		delete [] pArray;
	}
	else
	{
		if (m_saved_printer->access.GetLength()!=0)
			changed=TRUE;
	}
	return changed;
}


void CEditPrinter::OnOK() 
{

	if(!UpdateData(TRUE))
		return;
	CPrinterUIView *pview=(CPrinterUIView*) GetParentFrame()->GetActiveView();
	ASSERT(pview!=NULL);
	
	
	if (m_saved_printer)
	{
		if (PrinterChanged())
		{   
			//AfxMessageBox("Printer Changed");
			if (! CheckChanges()) return;
			SwapPrinterData(m_saved_printer);
			pview->FillPrinterListCtrl(); //Reset main view Printer List Ctrl
			pview->FillAdapterListCtrl(); //Reset main view Adapter List Ctrl
			ini.OutputPrinterNode("EDIT",m_saved_printer);//Write changes to output
			pview->ChangeEditBoxes(NULL,NULL);                    //Clear Edit Boxes
		//	pview->m_AppList.ResetContent();                      //Clear App ListBox
			pview->m_AppListCtrl.DeleteAllItems();

		//	pview->FillEditBoxes(mFrame->m_saved_printer->name);  //Reset main view edit boxes
			pview->GetDlgItem(IDC_Apply)->EnableWindow(TRUE); 
		
		}
	}
	else  //its an ADD
	{
		if (!CheckChanges()) return;//new
		printer_node* t;
		t=ini.NewPrinterNode();
		//AfxMessageBox("swapping data");
		//these 2 can't be changed through UI on add at this time
		t->queuestatus=0;
		t->printerstatus=0;   
		SwapPrinterData(t);
		//AfxMessageBox("swapped data");
		PrinterList.AddTail(t);
		pview->FillPrinterListCtrl();  //Reset main view Printer List Ctrl
		pview->FillAdapterListCtrl();  //Reset main view Adapter List Ctrl
		ini.OutputPrinterNode("ADD",t);//Write changes to output
		//pview->FillEditBoxes(t); 
		pview->GetDlgItem(IDC_Apply)->EnableWindow(TRUE); 


	}



	CDialog::OnOK();
}


void CEditPrinter::OnRedirect() 
{
	printer_node* t=new printer_node;
	UpdateData(TRUE);
	SwapPrinterData(t);    //put current dialog stuff in temp node
	CRedirectDialog redirect_dlg(NULL,t);
	BOOL ok = (redirect_dlg.DoModal() == IDOK);
	if(ok)
	{
		m_RedirectPrinter=t->redirected;
		UpdateData(FALSE);
	}
	delete t;
}

void CEditPrinter::OnItemchangedPrinterAdapterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->uNewState & LVIS_SELECTED)
	{
		
		int i=pNMListView->iItem;
		if (i>=0)
			
		{   
			
			
			if(m_AdapterList.GetItemText(i,3)==_T("FILE")&&!m_bAtlass)
		
			{
				CPrint2FileDlg dlg;
				dlg.DoModal();
				m_FileName=dlg.m_PrintFileName;
				m_FileNameCtrl.SetWindowText(m_FileName);
				m_AdapterPortCtrl.ResetContent();
				m_AdapterPortCtrl.AddString("FILE");
				m_AdapterPortCtrl.SelectString(-1,"FILE");
				m_nAppend=dlg.m_nAppend;
				m_bAtlass=TRUE;
				ShowFileInfo();
				return;
			}
			
			
			
			if(m_AdapterList.GetItemText(i,3)!=_T("FILE")&&m_bAtlass) //make it ! ATLAS
			{
								
				ShowFileInfo(FALSE);
				m_bAtlass=FALSE;
				m_FileName=_T("None");
				m_AdapterPort=_T("");
				m_FileNameCtrl.SetWindowText(m_FileName);
				//AfxMessageBox("In atlas part Adapter changed & FileName is-> " + m_FileName);
				m_nAppend=-1;
			}
			
			
			if(m_AdapterList.GetItemText(i,1)!=_T("N/A")) //3/13/98
				SetPorts(m_AdapterList.GetItemText(i,0),m_AdapterList.GetItemText(i,3));
			//9-25-97 TR added these lines to put the current port in LB
			if(m_saved_printer&&m_saved_printer->adapterIP==m_AdapterList.GetItemText(i,0)
				/*3/13/98 added this &&*/&&m_AdapterList.GetItemText(i,1)!="N/A")
			{
				m_AdapterPortCtrl.AddString(m_saved_printer->port);
				m_AdapterPortCtrl.SelectString(-1,m_saved_printer->port); 
			}
			//9-25-97 TR
			
			// 3/13/98 added this next if
			// TODO got to get the printer string
			if(m_AdapterList.GetItemText(i,1)=="N/A")
			{
                m_AdapterPortCtrl.ResetContent();
				CString Printers=ini.GetHostPrinters(m_AdapterList.GetItemText(i,0));
				LPTSTR pstr,pstrToken;
				TCHAR seps[]=_T(",");
				pstr=Printers.GetBuffer(0);
				pstrToken=_tcstok(pstr,seps);
				while(pstrToken!=NULL)
				{
					m_AdapterPortCtrl.AddString(pstrToken);
					//pLB->SetSel(pLB->FindString(-1,pstrToken));
					pstrToken=_tcstok(NULL,seps);
				}
				Printers.ReleaseBuffer(-1);
				
				//m_AdapterPortCtrl.SelectString(-1,"N/A"); 
			}
			
			GetDlgItem(IDC_ADAPTERPORT)->EnableWindow(TRUE);
		}
		
	}
	else
	{  
		m_AdapterPortCtrl.ResetContent();
		GetDlgItem(IDC_ADAPTERPORT)->EnableWindow(FALSE);
	}
	*pResult = 0;
}



void CEditPrinter::SetPorts(CString adapter_IP,CString adapter_name) 
{  
	//puts ports for currently selected adapter in ports comobox

	if(adapter_IP=="FILE")  //ATLAS
	{
		//AfxMessageBox("In atlas part of set ports");
		m_AdapterPortCtrl.ResetContent();
		m_AdapterPortCtrl.AddString("FILE");   
		return;
	}

	adapter_type_node* t;
	CString current_selected_ip;
	POSITION pos=AdapterTypeList.GetHeadPosition();

	while (pos != NULL)  
	{
		  t = AdapterTypeList.GetNext(pos);
		  if(t->name==adapter_name) break;
	}

    TCHAR save[100];
	strcpy(save,t->ports); 
    char *token = strtok((char*)(LPCTSTR) t->ports, "," );
	m_AdapterPortCtrl.ResetContent();


	while( token != NULL )
	{   //don't display ports if they are used
	//	if(!ini.PortUsed(adapter_IP,(CString)token/*,current_selected_ip*/)) //jj 1/9 took out for NAVMASO put back
			m_AdapterPortCtrl.AddString(token);      
		token = strtok( NULL, "," );
	}
	t->ports=save;
}



void CEditPrinter::OnNtbutton() 
{
	CAddNTPrinter dlg;
	UpdateData(TRUE);
	if(dlg.DoModal()==IDOK)
	{
		//m_NTDriver=dlg.m_driver; 3/1/99
		m_NTDriver=dlg.m_Printers; //3/1/99
		UpdateData(FALSE);
	}
		
	//CEdit* pEdit=(CEdit*) GetDlgItem(IDC_PrinterAccess);
	((CEdit*) GetDlgItem(IDC_PrinterAccess))->SetFocus();
}




void CEditPrinter::OnSelchangePrinterAccess() 
{
	

	if(m_saved_printer && !m_saved_printer->OT.IsEmpty()) //TR10172/73
	{
		MessageBeep(MB_ICONEXCLAMATION );
		//AfxMessageBox("Associated Output types must be removed First!");
		m_PrinterAccessCtrl.SelectString(-1,"By Application");
		return;
	}

	//new feature 10-7-97
	//show app list based if access is by type or not

	UpdateData(TRUE);

	if(m_PrinterAccess==_T("By Application"))
		FillListBox(&m_AppList,&AppsList);
	else
		m_AppList.ResetContent(); 
}


BOOL CEditPrinter::PreTranslateMessage(MSG* pMsg) 
{
	AUTOMATIC_HELP(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CEditPrinter::OnDestroy() 
{
	CDialog::OnDestroy();
	
	help_object.SetWindowHelp(HT_PrinterConfiguration_WindowDescription);	
}

void CEditPrinter::ShowFileInfo(BOOL bShow/*TRUE*/) 
{
	int cmd= bShow ? SW_SHOWNA   /*SW_SHOW*/ : SW_HIDE;
	
	GetDlgItem(IDC_FILENAMETEXT)->ShowWindow(cmd);
	GetDlgItem(IDC_APPEND)->ShowWindow(cmd);
	GetDlgItem(IDC_OVERWRITE)->ShowWindow(cmd);
	m_FileNameCtrl.ShowWindow(cmd);
	if(bShow)
	{
		m_FileNameCtrl.SetWindowText(m_FileName);

		if(!m_nAppend)
			((CButton*)GetDlgItem(IDC_OVERWRITE))->SetCheck(1);
		else
			((CButton*)GetDlgItem(IDC_APPEND))->SetCheck(1);
	}
}

void CEditPrinter::OnClickPrinterAdapterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_AdapterList.GetText()==_T("NTSHARE")) 
	{
		CSelectShareDlg dlg;
		
		if(dlg.DoModal()==IDOK)
		{
		
			m_HostName=_T("NTSHARE");//USE as flag in swapdata
			m_AdapterPort=dlg.m_strPrinter;
		    m_adapterIP=dlg.m_strHost;
			m_AdapterPortCtrl.ResetContent();
			//Put Dot Matrix & don't them change Printer class
			m_PrinterClassCtrl.ResetContent();
			m_PrinterClassCtrl.AddString(_T("Dot Matrix"));
			m_PrinterClassCtrl.SelectString(-1,_T("Dot Matrix"));
		//	m_PrinterClassCtrl.EnableWindow(FALSE);
			////////////////////////////////////////////////////
			m_AdapterPortCtrl.AddString(dlg.m_strPrinter);
			m_AdapterPortCtrl.SelectString(-1,dlg.m_strPrinter);
			m_AdapterPortCtrl.EnableWindow(FALSE);
		
					
		}

		((CPrinterUIApp*)AfxGetApp())->m_strShares=dlg.strResult;
			
	}



	*pResult = 0;
}

BOOL CEditPrinter::CheckOverloadChanges() 
//called in PrinterChanged to see if any of the Overloaded IP, Port 
//fields Changed 06/10/02
{
	CString strType;
	int nIndex = m_AdapterList.GetNextItem( -1, LVNI_SELECTED );
	ASSERT (nIndex > -1);

	if(ini.IsAdapter(m_AdapterList.GetText())) //REGULAR PRINTER
		return (m_AdapterList.GetText()==m_saved_printer->adapterIP && 
		       m_saved_printer->Type==_T("REGULAR"));

	if(m_AdapterPort==_T("FILE")) //FILEPRINTER
	{
		return (m_saved_printer->Type==_T("REGULAR") && 
			   m_saved_printer->FileName==_T("REGULAR")== m_FileName);
	}
	
	if(!ini.IsAdapter(m_AdapterList.GetText()) //NTSHARE
		&& m_AdapterList.GetText(3)==_T("NTSHARE")) 
	{
		return (m_saved_printer->Type==_T("NTSHARE") && m_AdapterList.GetText() ==
	    m_saved_printer->port && m_AdapterList.GetText(1) ==
	    m_saved_printer->adapterIP);
	}

    //Danger,Danger,Danger, but the hack requires that it HAS TO BE Server 
	//@ this point

	//ELSE SERVER!!!


//	TRACE1(_T("In CheckOverloadChanges() printer type is-> %s\n"),strType);

	return (m_saved_printer->Type==_T("SERVER") && m_AdapterList.GetText() ==
	    m_saved_printer->port && m_AdapterList.GetText(1) ==
	    m_saved_printer->adapterIP);

	return FALSE; //should never happen

	
}