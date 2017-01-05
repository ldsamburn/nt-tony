/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "ini.h"
#include "ntcssapi.h"
#include "msgs.h"
#include "Inri_Ftp.h"
#include "StringEX.h"
#include "ini2.h"


CIni::CIni()
{
	
    m_unclass=_T("UNCLASSIFIED");
    m_topsecret=_T("TOP SECRET");
    m_secret=_T("SECRET");
    m_sensitive=_T("UNCLASSIFIED SENSITIVE");
    m_confidential=_T("CONFIDENTIAL");
}

CIni::~CIni()
{

	TRACE(_T("In CINI destructor\n"));
    AdapterList.RemoveAll();
	HostList.RemoveAll();
	PrinterList.RemoveAll();
	AppsList.RemoveAll();
	PTList.RemoveAll();
}

BOOL CIni::CheckHost(CString strHostname,CString& strIPaddress)
{	
    	
	CWaitCursor wait;	
	
	CmsgGETHOSTINFO msg;
	
	try
	{
		
		if(!msg.Load(strHostname.GetBufferSetLength(SIZE_HOST_NAME)
			,strIPaddress.GetBufferSetLength(SIZE_IP_ADDR*2)))
			throw (_T(""));
		
		if(!msg.DoItNow())
			throw (_T(""));
		
		strHostname.ReleaseBuffer();
		strIPaddress.ReleaseBuffer();
		
		
		if(!msg.UnLoad(strHostname.GetBufferSetLength(SIZE_HOST_NAME),
			strIPaddress.GetBufferSetLength(SIZE_IP_ADDR*2)))
			throw (_T(""));
		
		strHostname.ReleaseBuffer();
		strIPaddress.ReleaseBuffer();
		
		return TRUE;
	}

	catch(LPSTR pStr)
	{
		pStr=_T(""); 
		return FALSE;
	}

}


BOOL CIni::CheckIP(CString strIPaddress)
{	
	CWaitCursor wait;	// display wait cursor

	CmsgGETHOSTINFO msg;

	BOOL ok=msg.Load(_T(""),strIPaddress.GetBuffer(SIZE_IP_ADDR*2));

	if (ok)
		ok=msg.DoItNow();

	strIPaddress.ReleaseBuffer();

    return ok;
}


BOOL CIni::GetIniFile()
{
    CWaitCursor wait;	// display wait cursor

	char buf[_MAX_PATH];
	char server_file_name[_MAX_PATH];

	try
	{

		//Get NTCSS root & make filenames out of it
		::GetPrivateProfileString("NTCSS","NTCSS_SPOOL_DIR","!",buf,_MAX_PATH,"ntcss.ini");
		if (strstr(buf,"!"))
			throw(_T("There is no NTCSS ini file!"));
		
		//initialize all client side filenames using ntcss spool as dir


		m_PutFileName=m_LogFileName=m_GetFileName=buf;
		m_GetFileName+=_T("/printers.ini");
		m_PutFileName+=_T("/prtupd.ini");
		m_LogFileName+=_T("/printer.log");

	    _unlink( m_PutFileName);       //kill old log file
		   

	//	NtcssGetServerName(m_hostname,NTCSS_SIZE_HOSTNAME);

		NtcssGetMasterName(m_hostname);

		CmsgGETPRINTERINI msg;

		
		if(!msg.Load())
			throw(_T("GETPRINTERINI Load failed"));

		if(!msg.DoItNow())
			throw(_T("GETPRINTERINI failed"));

		if(!msg.UnLoad(server_file_name))
			throw(_T("GETPRINTERINI Load failed"));
			
		if(!NtcssGetFile(m_hostname,server_file_name,m_GetFileName,TRUE))
			throw(_T("FTP failed")); 

  
  //       _tcscpy(server_file_name,"/h/NTCSSS/logs/printcap.ini.jaydiv"); //debug!!!!!!!!
      

			//Uses the Unix path to initialize putfile name
		TCHAR lpszUserName[SIZE_LOGIN_NAME+1];
		if(!NtcssLoginName(lpszUserName))
			throw(_T("Could not get UserName"));
			
		*(strrchr(server_file_name,'/'))=NULL;
	
		m_ServerFileName.Format("%s%s%s",server_file_name,_T("/prtupd.ini."),lpszUserName);
				
		return TRUE;
	}

	catch(LPSTR pStr)
	{
		AfxMessageBox(pStr);
		m_PutFileName=_T("");
		m_GetFileName=_T("");
		return FALSE;

	}
}


////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::GetPrinterStatus(LPCTSTR pname,int *printer_status,int *queue_status,LPCTSTR host_name)
{


	CmsgGETPRTSTATUS msg;

	BOOL ok=msg.Load(pname,host_name);

	if (ok)
		ok=msg.DoItNow();
	
	if (ok)
		ok=msg.UnLoad(printer_status,queue_status);

	if (ok)

		return TRUE;

	else
	{
		printer_status=0;
		queue_status=0;
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::SetPrinterStatus(LPCTSTR pname,LPCTSTR host_name,
							LPCTSTR printer_status,LPCTSTR queue_status)
{


	CmsgSETPRTSTATUS msg;
	
	if(!msg.Load(pname,printer_status,queue_status,host_name))
		return FALSE;

	return msg.DoItNow();
	

}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::PutIniFile()
{
    CWaitCursor wait;	// display wait cursor

	if(!NtcssPutFile(m_hostname,m_ServerFileName,
				     m_PutFileName,TRUE)) return FALSE;

	_unlink( m_PutFileName);       //kill old log file
	
	CmsgPUTPRINTERINI msg;

	char server_file_name_buf[SIZE_SERVER_FILE_NAME];
	strcpy(server_file_name_buf,m_ServerFileName);

	if(!msg.Load(server_file_name_buf)) return FALSE;

	if(!msg.DoItNow()) return FALSE;
			
	if(!msg.UnLoad(server_file_name_buf)) return FALSE;


	if(!NtcssGetFile(m_hostname,server_file_name_buf,
				     m_LogFileName,TRUE)) return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::DoesSectionExist(CString as_section)
{
	
	int i;
	return(i=::GetPrivateProfileSection(as_section,m_scratch_buf,
	                         (DWORD) sizeof(m_scratch_buf),m_PutFileName));
}

/////////////////////////////////////////////////////////////////////////////////////////

void CIni::OutputPrinterNode(CString as_operation,printer_node *ptr_printer)
{
	char *ls_junk={NULL};
	int  revision=1;
	CString DerivedName=ptr_printer->name + _T("_1");

	TRACE0(_T("In OutputPrinterNode()\n"));

	 while (DoesSectionExist(DerivedName))   //this should increment derived name
		DerivedName.Format("%s_%d",ptr_printer->name,++revision);

	//Redid this access section 6/10/02 to detect changes when Access is removed
	CMapStringToString HostLIST;

	CStringEx strAccess=ptr_printer->access;
	CString strApp,strHost;

	//new or current hosts... map keeps things unique
	for(int i=0;;i++)
	{
		if((strApp=strAccess.GetField(',',i))==_T(""))
			break;

		strHost=GetAppHost(strApp);
		HostLIST.SetAt(strApp,strHost);
	}

	//now stick in any old hosts
	strAccess=ptr_printer->OldAccess;
	for(i=0;;i++)
	{
		if((strApp=strAccess.GetField(',',i))==_T(""))
			break;

		strHost=GetAppHost(strApp);
		HostLIST.SetAt(strApp,strHost);
	}

	POSITION pos;
	for( pos = HostLIST.GetStartPosition(); pos != NULL; )
	{

		HostLIST.GetNextAssoc( pos, strApp, strHost);

		::WritePrivateProfileString(_T("APPLY_HOSTS"),strHost,strHost,m_PutFileName);
		::WritePrivateProfileString(strHost + _T("_PRINTERS"),DerivedName,DerivedName,m_PutFileName);

	}

	HostLIST.RemoveAll();

	::WritePrivateProfileSection(DerivedName,ls_junk,m_PutFileName);
	::WritePrivateProfileString("PRINTERS",DerivedName,DerivedName,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"OPERATION",as_operation,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"PRINTERNAME",ptr_printer->name,m_PutFileName);

	::WritePrivateProfileString(DerivedName,"HOSTNAME",_T("NODATA"),m_PutFileName);
	::WritePrivateProfileString(DerivedName,"LOCATION",ptr_printer->location,m_PutFileName);
	CString temp;
	temp.Format("%d",ptr_printer->maxsize);
	::WritePrivateProfileString(DerivedName,"MAXSIZE",temp,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"CLASS",ptr_printer->classname,m_PutFileName);
	temp.Format("%d",ptr_printer->security);
	::WritePrivateProfileString(DerivedName,"SECURITY",temp,m_PutFileName);
	temp.Format("%d",ptr_printer->suspend);
	::WritePrivateProfileString(DerivedName,"SUSPEND",temp,m_PutFileName);
	temp.Format("%d",ptr_printer->queuestatus);
	::WritePrivateProfileString(DerivedName,"QUEUESTATUS",temp,m_PutFileName);
	temp.Format("%d",ptr_printer->printerstatus);
	::WritePrivateProfileString(DerivedName,"PRINTERSTATUS",temp,m_PutFileName);
	temp.Format("%d",ptr_printer->accessstatus);
	::WritePrivateProfileString(DerivedName,"ACCESSSTATUS",temp,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"PORT",ptr_printer->port,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"REDIRECTED",ptr_printer->redirected,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"ADAPTER_IP",ptr_printer->adapterIP.IsEmpty() ? "None"
		: ptr_printer->adapterIP,m_PutFileName);
	//New stuff for adding the printer type 12/29/99
	::WritePrivateProfileString(DerivedName,"TYPE",ptr_printer->Type,m_PutFileName);

	//start new 5/21/99
	CStringEx strDelimited=ptr_printer->access;
	for(i=0;;i++)
		if((temp=strDelimited.GetField(',',i))==_T(""))
				break;
		else
			::WritePrivateProfileString(DerivedName+_T("_ACCESS"),temp,temp,m_PutFileName);
	//end new 5/21/99


	//::WritePrivateProfileString(DerivedName,"ACCESS",ptr_printer->access,m_PutFileName); OLD WAY 5/21
	::WritePrivateProfileString(DerivedName,"NTDRIVER",ptr_printer->NTDriver,m_PutFileName);
	temp.Format("%d",ptr_printer->nAppend);//FILE STUFF
	//AfxMessageBox("Writing this for append -> " + temp);
	::WritePrivateProfileString(DerivedName,"APPEND",temp,m_PutFileName);//FILE STUFF
	::WritePrivateProfileString(DerivedName,"FILENAME",ptr_printer->FileName.IsEmpty() ? "None"
	: ptr_printer->FileName,m_PutFileName);//FILE STUFF
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
void CIni::OutputAdapterNode(CString as_operation,adapter_node *ptr_adapter)
{
	char *ls_junk={NULL};
	int  revision=1;
	CString DerivedName=ptr_adapter->IPaddress+"_1";

	while (DoesSectionExist(DerivedName))
		DerivedName.Format("%s_%d",ptr_adapter->IPaddress,++revision);

	::WritePrivateProfileSection(DerivedName,ls_junk,m_PutFileName);
	::WritePrivateProfileString("ADAPTERS",DerivedName,DerivedName,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"OPERATION",as_operation,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"HOSTNAME",ptr_adapter->hostname,m_PutFileName);
    ::WritePrivateProfileString(DerivedName,"IPADDRESS",ptr_adapter->IPaddress,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"MACHINE_ADDRESS",ptr_adapter->machine_address,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"LOCATION",ptr_adapter->location,m_PutFileName);
	::WritePrivateProfileString(DerivedName,"TYPE",ptr_adapter->type,m_PutFileName);
}


/////////////////////////////////////////////////////////////////////////////////////////////
char *CIni::GetSectionData(char *as_section)
{

	int i=::GetPrivateProfileSection(as_section, m_scratch_buf,
	                         (DWORD) sizeof(m_scratch_buf),m_GetFileName);
	return m_scratch_buf;
}



////////////////////////////////////////////////////////////////////////////////////////////
const CString CIni::GetClassification(const int dClass)
{

	switch(dClass)
	{
	case 0:
			return m_unclass;
	case 1:
			return m_sensitive;
	case 2:
			return m_confidential;
	case 3:
			return m_secret;
	case 4:
			return m_topsecret;
	default:
			return _T("");
	}

}

////////////////////////////////////////////////////////////////////////////////////////////

const int CIni::PutClassification(CString StrClass)
{

	if (StrClass==m_unclass) return 0;
	if (StrClass==m_sensitive) return 1;
	if (StrClass==m_confidential) return 2;
	if (StrClass==m_secret) return 3;
	if (StrClass==m_topsecret) return 4;
	return -1;

}

////////////////////////////////////////////////////////////////////////////////////////////

BOOL CIni::BuildPrinterList()
{
	
	CWaitCursor wait;	// display wait cursor
	
	PrinterList.RemoveAll();
	CIni2 ini(m_GetFileName);
	CIni2 ini2(m_GetFileName);
	CIni2 ini3(m_GetFileName); //new
	
	
	if(!ini.GetSectionData(_T("Printers")))
		return FALSE;
	
	printer_node *t;
	CString strPrinter;
	
	while(ini.GetString(strPrinter))
	{
		
		if(ini2.GetSectionData(strPrinter))
		{
			t=new printer_node;
			t->name=strPrinter;
			t->hostname=ini2.GetStrings();
			t->location=ini2.GetStrings();
			t->maxsize=atoi(ini2.GetStrings());
			t->classname=ini2.GetStrings();
			t->security=atoi(ini2.GetStrings());
			t->suspend=atoi(ini2.GetStrings());
			t->printerstatus=atoi(ini2.GetStrings());
			t->queuestatus=atoi(ini2.GetStrings());
			t->accessstatus=atoi(ini2.GetStrings());
			t->port=ini2.GetStrings();
			t->redirected=ini2.GetStrings();
			if(t->redirected==_T("")) t->redirected=_T("NONE");
			t->adapterIP=ini2.GetStrings();
			CString strApp;
			
			if(ini3.GetSectionData(strPrinter+_T("_ACCESS")))
				while(ini3.GetString(strApp))
					if(t->access.IsEmpty())
						t->access=strApp;
					else
						t->access+= (_T(",")+strApp);
					
					t->NTDriver=ini2.GetStrings();
					t->OT=ini2.GetStrings();
					t->nAppend=atoi(ini2.GetStrings());
					t->FileName=ini2.GetStrings();
					t->Type=ini2.GetStrings();
					if(t->Type.IsEmpty()) //figure out the "Printer Type... Oh Boy!!!"
					{
						TRACE0(_T("No type in INI file...deriving it\n"));
						if(IsAdapter(t->adapterIP)) //REGULAR PRINTER
							t->Type=_T("REGULAR");
						
						else
							if(t->port==_T("FILE")) //FILEPRINTER
								
								t->Type=_T("FILE");
							
							else
								if(IsHostIP(t->adapterIP)) //NTSHARE
									
									t->Type=_T("SERVER");
								
								else //SERVER PRINTER
									t->Type=_T("NTSHARE");

						TRACE2(_T("Printer <%s>is type-> %s\n"),t->name,t->Type);
					}
					
					PrinterList.AddTail(t);
		}
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////

printer_node* CIni::NewPrinterNode()
{
	printer_node *t;
	return t=new printer_node;
}

///////////////////////////////////////////////////////////////////////////////////////////

adapter_node* CIni::NewAdapterNode()
{
	adapter_node *t;
	return t=new adapter_node;
}

///////////////////////////////////////////////////////////////////////////////////////////

BOOL CIni::BuildAdapterList()
{
	AdapterList.RemoveAll();
	CIni2 ini(m_GetFileName);
	CIni2 ini2(m_GetFileName);
	
	if(!ini.GetSectionData(_T("Adapters")))
		return FALSE;
	
	
	adapter_node *t;
	CString strAdapter;
	
	while(ini.GetString(strAdapter))
	{
		
		if(ini2.GetSectionData(strAdapter))
		{
			t=new adapter_node;
			t->IPaddress=strAdapter;
			t->hostname=ini2.GetStrings();
			t->machine_address=ini2.GetStrings();
			t->location=ini2.GetStrings();
			t->type=ini2.GetStrings();
			AdapterList.AddTail(t);
		}
	}

	return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////

BOOL CIni::BuildAdapterTypeList()
{

	AdapterTypeList.RemoveAll();

	int i=::GetPrivateProfileSection(_T("ADAPTERTYPES"),m_scratch_buf,
	                         (DWORD) sizeof(m_scratch_buf),m_GetFileName);
	if(i==0)
		return FALSE;

	char *q,*p=m_scratch_buf; 
    adapter_type_node* t;
	char temp[100];

	while (strlen(p)>0)

 	{
		
		t=new adapter_type_node;
		strcpy(temp,p);
		p=strchr(p,'\0')+1;  //pointing @ next line
		q=strchr(temp,'=')+1;//pointing @ ports
		t->ports=q;
		*(q-1)=NULL;
		t->name=temp;
		AdapterTypeList.AddTail(t);
   }
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////

BOOL CIni::BuildHostList()
{
	//puts in all the hosts THAT HAVE printers!!!!!
	HostList.RemoveAll();
	CIni2 ini(m_GetFileName);
	CIni2 ini2(m_GetFileName);
	CIni2 ini3(m_GetFileName);

	if(!ini.GetSectionData(_T("HOSTS")))
		return FALSE;

    host_node* t;
	CString strHost;
	CString strPrinter;
	while(ini.GetString(strHost))
	{
		if(ini2.GetSectionData(strHost))
		{
			t=new host_node;
			t->name=strHost;
			t->PrinterList=_T("");
			t->IP=ini2.GetStrings();
			if(ini3.GetSectionData(strHost+_T("_PRINTERS")))
				while(ini3.GetString(strPrinter))
					if(t->PrinterList.IsEmpty())
						t->PrinterList=strPrinter;
					else
						t->PrinterList+="," + strPrinter;
			
			HostList.AddTail(t);
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::BuildCStringList(char* section,CStringList* list)
//Builds all CStringList types
{
	list->RemoveAll();
		
	int i=::GetPrivateProfileSection(section,m_scratch_buf,
	                         (DWORD) sizeof(m_scratch_buf),m_GetFileName);
	if(i==0)
		return FALSE;

	char *q=m_scratch_buf; 
	q=strchr(q,'=');
	
	do
	{
		q++;
		list->AddTail(q);
		q=strchr(q,'\0')+1;
		q=strchr(q,'=');
	}while (q !=NULL);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

printer_node* CIni::FindAdapterInPrintList(CString as_adapter)
//returns address of node if found else NULL
{
	printer_node *ptr_printer;
	
	POSITION pos=PrinterList.GetHeadPosition();
	while (pos != NULL)  //loop till we find printer name
	{
		  ptr_printer = PrinterList.GetNext(pos);
		  if(ptr_printer->adapterIP==as_adapter) return ptr_printer;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////

printer_node* CIni::FindPrinterInPrintList(CString as_printer)
{
	printer_node* ptr_printer;
	POSITION pos=PrinterList.GetHeadPosition();
	while (pos != NULL)  
		{
		  ptr_printer = PrinterList.GetNext(pos);
		  if(ptr_printer->name==as_printer) return ptr_printer;
		}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////

CString CIni::GetHostPrinters(CString name) //3/13/98 added this function
{
	host_node* h;
	POSITION pos=HostList.GetHeadPosition();
	while (pos != NULL)  
		{
		  h = HostList.GetNext(pos);
		  if(h->name==name) return h->PrinterList;
		}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////////////////
CString CIni::GetHostIP(CString name) //3/13/98 added this function
{
	host_node* h;
	POSITION pos=HostList.GetHeadPosition();
	while (pos != NULL)  
		{
		  h = HostList.GetNext(pos);
		  if(h->name==name) return h->IP;
		}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::IsHostIP(CString strIP) 
{
	host_node* h;
	POSITION pos=HostList.GetHeadPosition();
	while (pos != NULL)  
		{
		  h = HostList.GetNext(pos);
		  if(h->IP==strIP) return TRUE;
		}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::DelAdapterInAdapterList(CString as_adapter)
//deletes node from Adapter CLIST
//records delete in ini file 
//destroys node
{
	adapter_node *ptr_adapter;
	POSITION pos=AdapterList.GetHeadPosition();
	while (pos != NULL)  //loop till we find printer name
	{
		  ptr_adapter = AdapterList.GetNext(pos);
		  if(ptr_adapter->IPaddress==as_adapter) break;
	}
	AdapterList.RemoveAt(AdapterList.Find(ptr_adapter));
	OutputAdapterNode("DELETE",ptr_adapter);
	delete ptr_adapter;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::PortUsed(CString adapter,CString port/*,CString current_selected_ip*/)
{
	//used to see if ports in use before putting in dd for edit/add
	//
	printer_node* ptr_printer;
	
	POSITION pos=PrinterList.GetHeadPosition();
	while (pos != NULL)  
		{
		  ptr_printer = PrinterList.GetNext(pos);
		  if(ptr_printer->adapterIP==adapter&&ptr_printer->port==port/*&&
			 ptr_printer->adapterIP!=current_selected_ip*/)
			 return TRUE;
		}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CIni::AllPortsUsed(CString adapter_IP,CString adapter_name)
{
	//used to suppress adapter from edit list if all the ports are used
	//except for currently selected printer which will be included

	//find adapter type to get # of ports
	
	adapter_type_node* t;
	POSITION pos=AdapterTypeList.GetHeadPosition();
	while (pos != NULL)  
	{
		  t = AdapterTypeList.GetNext(pos);
		  if(t->name==adapter_name) break;
	}

	const char *p=(LPCTSTR)t->ports;
	
	int i=0;
    //count the commas to get # of ports
	while (*p) //count # of ports for adapter type
	{
		if (*p==',')
			i++;
		p++;
	}

	i++;     //this is final count

	
	printer_node* ptr_printer;
    //now count how many are in use
	int y=0;
	pos=PrinterList.GetHeadPosition();
	while (pos != NULL)  
	{
		ptr_printer = PrinterList.GetNext(pos);
		if(ptr_printer->adapterIP==adapter_IP)
			y++;
	}
	return y==i;
}

///////////////////////////////////////////////////////////////////
CString CIni::GetAppHost(CString strApp)
{
	TCHAR strHostname[SIZE_HOST_NAME];
	NtcssGetAppHost(strApp,strHostname,SIZE_HOST_NAME);
	ASSERT(strlen(strHostname)>0);
	return strHostname;
}

BOOL CIni::IsAdapter(CString strIP)
//returns true if IP exists
{
	adapter_node* t;
	
	POSITION pos=AdapterList.GetHeadPosition();
	while (pos != NULL)  
	{
		  t = AdapterList.GetNext(pos);
		  if(t->IPaddress==strIP) return TRUE;
	}
	return FALSE;
}

BOOL CIni::IsHost(CString strHost)
//returns true if Host exists
{
	host_node* t;
	
	POSITION pos=HostList.GetHeadPosition();
	while (pos != NULL)  
	{
		  t = HostList.GetNext(pos);
		  if(t->name==strHost) return TRUE;
	}
	return FALSE;
}
