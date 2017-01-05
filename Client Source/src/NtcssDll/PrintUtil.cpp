/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "printutil.h"
#include "winspool.h"
#include "DllUtils.h"


typedef BOOL (CALLBACK *ULPRET)(LPWSTR,DWORD,LPBYTE,LPWSTR);

BOOL CPrintUtil::DelTempPrinter() 
{
	HANDLE lh_Printer;
	PRINTER_DEFAULTS pd;

	CWaitCursor wait;	// display wait cursor
	
	::ZeroMemory(&pd, sizeof(pd));
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if(::OpenPrinter("NTCSS",(LPHANDLE)(&lh_Printer),&pd))
	{
	    
		BOOL ok=::DeletePrinter(lh_Printer);
		
		if (lh_Printer != NULL)
			::ClosePrinter(lh_Printer);


		return ok;
	}
	else
		return FALSE;
}


BOOL CPrintUtil::AddNTCSSPort() 
{
	
	PORT_INFO_1 lport;

	makeSpoolFileName(m_SpoolFileName);

	lport.pName=m_SpoolFileName;
	HINSTANCE hLib;
	ULPRET    lpfnDLLProc;

	//this block gets sys path for winspool.drv
	UINT size=::GetSystemDirectory(NULL,0);
	CString temp;
	::GetSystemDirectory(temp.GetBuffer(size),size);
	temp.ReleaseBuffer();
	temp+="\\Winspool.drv";
    
	hLib=::LoadLibrary((char*)(LPCSTR)temp);

	if(hLib)
	{

		lpfnDLLProc=(ULPRET) ::GetProcAddress(hLib,"AddPortExA");

		if(!lpfnDLLProc)
			return FALSE;
	
		BOOL ok=(*lpfnDLLProc)(NULL,(DWORD)1,(LPBYTE)&lport,(LPWSTR)"Local Port");

		::FreeLibrary(hLib);

		return ok;

	}
	else
	{
		AfxMessageBox("Couldn't load Winspool.drv!");
		return FALSE;
	}
}
	

BOOL CPrintUtil::DelNTCSSPort() 
{
	makeSpoolFileName(m_SpoolFileName);
	//doesn't do a damn thing except take it out of the registry
	//BOOL ok=::WriteProfileString("Ports",m_SpoolFileName,NULL);
	//::SendMessage(HWND_BROADCAST,WM_WININICHANGE,0L,0L); 
	::DeletePort(NULL,AfxGetMainWnd()->GetSafeHwnd(),m_SpoolFileName);
	DWORD ret=::GetLastError();
//	CString temp;
//	temp.Format("Error Code is-> %d",ret);
//	AfxMessageBox(temp);
	return TRUE;
}

BOOL CPrintUtil::IsNtcssPort() 
{

   	DWORD needed,returned;
 	PORT_INFO_2 *prts;

	makeSpoolFileName(m_SpoolFileName);

    ::EnumPorts(NULL,2,NULL,0,&needed,&returned);

	char *buff=new char[needed];

	prts=(PORT_INFO_2 *)buff;

	BOOL ok=::EnumPorts(NULL,2,(LPBYTE) prts,needed,&needed,&returned);

	if (ok) 
	{
		ok=FALSE;
		for(DWORD i=0;i<returned;i++)
		{
			if(!_stricmp(prts[i].pPortName,m_SpoolFileName))
			{
				ok=TRUE;
				break;
			}
		}
	}
	delete [] buff;
	return ok;
}
