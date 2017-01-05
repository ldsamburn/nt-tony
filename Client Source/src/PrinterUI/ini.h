/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ini.h : interface of the CIni class
//
/////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_INI_H_
#define _INCLUDE_INI_H_


#include "afxtempl.h"
#include "ntcssapi.h"

const int MAX_INI_SECTION=65535;
const int MAX_PRINTER_NAME=14;
const int MIN_PRINTER_NAME=3;
const int MAX_LOCATION=64;
const int MIN_LOCATION=5;


struct printer_node
{
	CString name;
	CString hostname;
	CString location;
	int     maxsize;
	CString classname;
	int     security;
	int     suspend;
	int     queuestatus;
	int     printerstatus;
	int     accessstatus;
	CString port;
	CString redirected;
	CString adapterIP;
	CString access;
	CString OldAccess;
	CString NTDriver;
	CString OT; //TR10172/73
	int     nAppend; //FILE STUFF
	CString FileName;
	CString Type; // Put this in for new Extract TODO put it in create 
	              // use it in the client to simplfy type logic
};  

struct adapter_node
{
	CString hostname;
	CString IPaddress;
	CString machine_address;
	CString location;
	CString type;
};

struct adapter_type_node
{
	CString name;
	CString ports;
};


struct host_node //3/13/98
{
	CString name;
	CString IP;
	CString PrinterList;
};


class CIni
{

protected:


	char *GetSectionData(char *section);
	CString GetAppHost(CString strApp);
	char m_scratch_buf[MAX_INI_SECTION]; //used for getting section data out of INI etc
    char m_scratch_buf2[MAX_INI_SECTION];//used by getprinter & getadapter names, they need 2 bufs

	
//	char m_PutFileName[_MAX_PATH];
//	char m_ServerFileName[_MAX_PATH];
	
public: 
	
//	char m_GetFileName[_MAX_PATH];
	CIni();
	~CIni();
   	BOOL GetIniFile();
	BOOL PutIniFile();
	BOOL GetPrinterStatus(LPCTSTR pname,int *printer_status,int *queue_status,LPCTSTR host_name);
	BOOL SetPrinterStatus(LPCTSTR pname,LPCTSTR host_name,LPCTSTR printer_status,LPCTSTR queue_status);
	BOOL CheckHost(CString strHostname,CString& strIPaddress);
	BOOL CheckIP(CString strIPaddress);
	const CString GetClassification(const int dClass);
	const int PutClassification(CString StrClass);
	printer_node* NewPrinterNode();
	adapter_node* NewAdapterNode();
	BOOL DoesSectionExist(CString as_section);
	void OutputPrinterNode(CString as_operation, printer_node *ptr_printer);
	void OutputAdapterNode(CString as_operation,adapter_node *ptr_adapter);
	BOOL BuildPrinterList();
	BOOL BuildAdapterList();
	printer_node* FindAdapterInPrintList(CString as_adapter);
	printer_node* FindPrinterInPrintList(CString as_printer);
	BOOL DelAdapterInAdapterList(CString as_adapter);
	CString GetHostPrinters(CString name);
	CString GetHostIP(CString name);
	BOOL BuildHostList();
	BOOL IsAdapter(CString strIP);
	BOOL IsHost(CString strHost);
	BOOL IsHostIP(CString strIP);
	//BOOL BuildHostNameList();
	//BOOL BuildAppList();
	BOOL BuildAdapterTypeList();
	//BOOL BuildPrinterTypeList();
	BOOL BuildCStringList(char* section,CStringList* list);
	BOOL PortUsed(CString adapter,CString port/*,CString current_selected_ip*/);
	BOOL AllPortsUsed(CString adapter_IP,CString adapter_name);

	char m_hostname[NTCSS_SIZE_HOSTNAME];
//	char m_LogFileName[_MAX_PATH];
	CString m_unclass,m_topsecret,m_secret,m_sensitive,m_confidential,m_PutFileName,
			m_ServerFileName,m_GetFileName,m_LogFileName;

};


#ifndef theman

CIni ini;
CList <printer_node*,printer_node*> PrinterList;
CList <adapter_node*,adapter_node*> AdapterList;
CList <host_node*,host_node*> HostList; //3/13/98
CList <adapter_type_node*,adapter_type_node*> AdapterTypeList;
CStringList AppsList;
//CStringList HostList;  3/13/98
CStringList PTList;


#else

extern CIni ini;
extern CList <printer_node*,printer_node*> PrinterList;
extern CList <adapter_node*,adapter_node*> AdapterList;
extern CList <adapter_type_node*,adapter_type_node*> AdapterTypeList;
extern CStringList AppsList;

//extern CStringList HostList; 3/13/98
extern CList <host_node*,host_node*>HostList;
extern CStringList PTList;

#endif

#endif // _INCLUDE_INI_H_
