/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "PrtUtilClass.h"
#include "winspool.h"
#include "Trace.h"
#include "atlbase.h"
#include "atlconv.h"
#include "token.h"
#include "sys/types.h"
#include "sys/stat.h"


typedef BOOL (CALLBACK *ULPRET)(LPWSTR,DWORD,LPBYTE,LPWSTR);

CPrintUtil::CPrintUtil(LPCTSTR printername,LPCTSTR drivername,
		               LPCTSTR driver,LPCTSTR datafile,LPCTSTR uifile,
			           LPCTSTR helpfile,LPCTSTR monitorname,LPCTSTR monitor,
			           LPCTSTR copyfiles,LPCTSTR ntcssPrinterType,LPCTSTR port,
					   LPCTSTR driverPath)
//Constructor for NT4 Adds
{

	m_printername=printername;
	m_drivername=drivername;
	m_driver=driver;
	m_datafile=datafile;
	m_uifile=uifile;
	m_helpfile=helpfile;
	m_monitorname=monitorname;
	m_monitor=monitor;
	m_copyfiles=copyfiles;
	m_ntcssPrinterType=ntcssPrinterType;
	m_port=port;
	m_driverPath=driverPath;
	TraceEx("In PrintUtil Constructor... native DLL\n");
	::OutputDebugString("Doing debug String");
	::OutputDebugString(m_ntcssPrinterType);
	::OutputDebugString("Did debug string");
	TraceEx("PrinterType -> %s\n",(LPCTSTR)m_ntcssPrinterType);
	Trace("Going to Print out the port\n");
	TraceEx("Port -> %s\n",(LPCTSTR)m_port);
	Trace("Printed out the port\n");
	TraceEx("PrinterName -> %s\n",(LPCTSTR)m_printername);
	m_nOSVersion=GetOSVersion();
	TraceEx("OS Version -> %d\n", m_nOSVersion);
	m_driverPath+=_T("\\");
	TraceEx("Driver Path-> %s\n",(LPCTSTR)m_driverPath);

}

CPrintUtil::CPrintUtil(LPCTSTR printername, LPCTSTR printingmethod)

//Constructor for deletes
{
	m_printername=printername;
    m_ntcssPrinterType=printingmethod;
	TraceEx("PrinterType -> %s\n",(LPCTSTR)m_ntcssPrinterType);
	TraceEx("PrinterName -> %s\n",(LPCTSTR)m_printername);
}


CPrintUtil::CPrintUtil(LPCTSTR printername, LPCTSTR filename, LPCTSTR driver,
                       LPCTSTR printingmethod)
//Constructor for W2K adds 
{
	m_printername=printername;
	m_port=filename;
	m_drivername=driver;
    m_ntcssPrinterType=printingmethod;
}


BOOL CPrintUtil::AddNTCSSPrinter()
{
	
	
	
	DEVMODE ls_devmode;
	PSECURITY_DESCRIPTOR pSecurityDescriptor;	 
	DWORD dwRevision=SECURITY_DESCRIPTOR_REVISION,dwError;
	HANDLE hPrinter;
	
	try
	{
		TraceEx(_T("In the CPrintUtil.AddPrinter Method\n"));
		
        //Called for all printer types, but we only physically add FILE printers
        if(m_ntcssPrinterType == _T("NTSHARE"))
           return AddSharePrinter();
		else
			if(m_ntcssPrinterType == _T("REGULAR") || 
			   m_ntcssPrinterType == _T("SERVER"))
			   return FALSE;

		if(IsPrinter()) //return TRUE if it's already there
			return TRUE;

    //TODO: we stopped adding the file port && used FILE for PORT in call
	//		need to investigate this

	//	TraceEx(_T("Adding Port <%s> to System\n"),(LPCTSTR)m_port);
	//	Trace("Calling AddPort\n");
	//	if(!AddNTCSSPort())
		//	throw(_T("Adding Printer Port Failed!!"));
		
	//	TraceEx(_T("Added Port <%s> Successfully\n"),(LPCTSTR)m_port);
		
		TraceEx("Calling add Print Driver\n");
		
		if(!AddNTCSSPrintDriver())
			throw(_T("Adding Printer Driver Failed!!"));
		
		
		/////////////////////////////////////////////////////
		
		::ZeroMemory( &ls_devmode, sizeof(ls_devmode) );
		
		if(!(::InitializeSecurityDescriptor(&pSecurityDescriptor,dwRevision)))
			throw (_T("Couldn't initialize security descriptor"));
		

		PRINTER_INFO_2 ls_pinfo={NULL,(LPTSTR)(LPCTSTR)m_printername,

			NULL,(LPTSTR)(LPCTSTR)"FILE:",(LPTSTR)(LPCTSTR)m_drivername,
			NULL,NULL,&ls_devmode,NULL,"winprint","RAW",NULL,pSecurityDescriptor,
			0,0,0,0,0,0,0,0};
		
		hPrinter =::AddPrinter(NULL,2,(LPBYTE) &ls_pinfo);
		dwError = ::GetLastError();
		
		if(!hPrinter) {
            Trace((LPCTSTR)GetSysError(dwError));
//			throw(_T("Failed to add printer") + GetSysError(dwError));
			throw(_T("Failed to add printer"));
        }

		::CloseHandle(hPrinter);
		
	}
	
	catch(LPTSTR pStr)
	{
		Trace(_T("%s\n"),pStr);
		return FALSE;
	}
	catch(...)
	{
		Trace(_T("Unknown exception caught... Adding Printer\n"));
		return FALSE;
	}
	return TRUE;
}


BOOL CPrintUtil::DelNTCSSPrinter() 
{
	HANDLE lh_Printer;
	PRINTER_DEFAULTS pd;
	
	try
	{

		Trace(_T("In DelNTCSSPrinter printer name is <%s> and type is <%s> \n"),
			(LPCTSTR)m_printername ,(LPCTSTR)m_ntcssPrinterType); ///debug

        // Only delete file printers
        if (m_ntcssPrinterType != _T("FILE") &&
			m_ntcssPrinterType != _T("NTSHARE"))
           return TRUE;

		Trace(_T("Calling IsPrinter\n")); ///debug

		if(!IsPrinter()) //return TRUE if it's not there
			return TRUE;

		Trace(_T("Past IsPrinter\n")); ///debug

		 if(m_ntcssPrinterType == _T("NTSHARE"))
           return DeleteSharePrinter();

		::ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		if(::OpenPrinter((LPTSTR)(LPCTSTR)m_printername,(LPHANDLE)(&lh_Printer),&pd))
		{
			
			BOOL ok=::DeletePrinter(lh_Printer);
			
			if (lh_Printer != NULL)
				::ClosePrinter(lh_Printer);
			
			return ok;
		}
		else
			return FALSE;
	}
	
	catch(...)
	{
		Trace(_T("Unknown excetion caught in DelNTCSSPrinter\n"));
		return FALSE;
	}
	
}


BOOL CPrintUtil::AddNTCSSPort() 
{
	//TODO prpbably need to check that it doesn't already exists
	try
	{
		if(IsPort(m_port))
			return TRUE;
		
		PORT_INFO_1 lport;
		
		lport.pName=(LPTSTR)(LPCTSTR)m_port;
		
		HINSTANCE hLib;
		ULPRET    lpfnDLLProc;
		
		//this block gets sys path for winspool.drv
		UINT size=::GetSystemDirectory(NULL,0);
		TCHAR temp[_MAX_PATH];
		
		::GetSystemDirectory(temp,_MAX_PATH);
		_tcscat(temp,_T("\\Winspool.drv"));
		
		hLib=::LoadLibrary(temp);
		
		if(hLib)
		{
			
			lpfnDLLProc=(ULPRET) ::GetProcAddress(hLib,"AddPortExA");
			
			if(!lpfnDLLProc)
				return FALSE;
			
			BOOL ok=(*lpfnDLLProc)(NULL,(DWORD)1,(LPBYTE)&lport,(LPWSTR)"Local Port");
			
			::FreeLibrary(hLib);

			int dog=GetLastError();
			TraceEx("In add port & the error is %d\n",dog);
			
			return ok;
			
		}
		else
		{
			Trace(_T("Couldn't load Winspool.drv!\n"));
			return FALSE;
		}
	}
	
	catch(...)
	{
		Trace(_T("Unknown exception caught in AddNTCSSPort!\n"));
		return FALSE;
	}
}


BOOL CPrintUtil::IsPort(LPCTSTR port) 
{
	
	DWORD needed,returned;
	PORT_INFO_2 *prts;
	
	try
	{
		
		::EnumPorts(NULL,2,NULL,0,&needed,&returned);
		
		LPTSTR buff=new char[needed];
		
		prts=(PORT_INFO_2 *)buff;
		
		if(!::EnumPorts(NULL,2,(LPBYTE) prts,needed,&needed,&returned))
			return FALSE;
		
		BOOL ok=FALSE;
		
		for(DWORD i=0;i<returned;i++)
			if(!_stricmp(prts[i].pPortName,port))
			{
				ok=TRUE;
				break;
			}
			
			if (buff)
				delete [] buff;
			
			return ok;
			
	}
	
	catch(...)
	{
		Trace(_T("Unknown Exception caught in IsPort\n"));
		return FALSE;
	}
}


BOOL CPrintUtil::GetDrivers()
//Copies the drivers from our directory to Proper spool directory

{
	struct _stat buf;
	//TODO get this out of the INI file & make it a member var
	CStdString src = m_driverPath;
	
	try
	{
		TraceEx(_T("In Get Drivers... stating files\n"));
		
		if (_tstat(m_strNtSpoolDir + m_driver, &buf))
			if(!CopyFile(src + m_driver, m_strNtSpoolDir + m_driver,TRUE))
				throw(_T("Could not copy the driver file\n"));
			
		TraceEx(_T("Copied the driver file\n"));
			
		if (_tstat(m_strNtSpoolDir + m_datafile, &buf))
			if(!CopyFile(src + m_datafile, m_strNtSpoolDir + m_datafile,TRUE))
				throw(_T("Could not copy the data file file\n"));
				
		TraceEx(_T("Copied the data file\n"));
		
		if (_tstat(m_strNtSpoolDir + m_uifile, &buf))
			if(!CopyFile(src + m_uifile, m_strNtSpoolDir + m_uifile,TRUE))
				throw(_T("Could not copy the UI file file\n"));
			
		TraceEx(_T("Copied the UI file\n"));
		
		if (_tstat(m_strNtSpoolDir + m_helpfile, &buf))
			if(!CopyFile(src + m_helpfile, m_strNtSpoolDir + m_helpfile,TRUE))
				throw(_T("Could not copy the help file file\n"));
			
		TraceEx(_T("Copied the help file\n"));
		
		if (_tstat(m_strNTSysDir + m_monitor, &buf))
			if(!CopyFile(src + m_helpfile, m_strNTSysDir + m_helpfile,TRUE))
				throw(_T("Could not copy the help file file\n"));
			
		TraceEx(_T("Copied the monitor file\n"));

		CToken tok(m_copyfiles);
		tok.SetToken(_T(","));
		CStdString strFile;
		while(tok.MoreTokens())
		{
			strFile = tok.GetNextToken();
			if (_tstat(m_strNTSysDir + strFile, &buf))
			if(!CopyFile(src + strFile, m_strNTSysDir + strFile,TRUE))
				throw(_T("Could not copy the help file file\n"));
			TraceEx(_T("Copied the copyfile file\n"));

		}
	}
	
	catch(LPTSTR pStr)
	{
		TraceEx(_T("GetDrivers(): %s\n"),(LPCTSTR)pStr);
		return FALSE;
	}
	
	catch(...)
	{
		TraceEx(_T("GetDrivers(): Unknown Exception caught!!!\n"));
		return FALSE;
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// AddTempPrintDriver 
// __________

BOOL CPrintUtil::AddNTCSSPrintDriver()
{
	
	DWORD needed;
	
	TraceEx(_T("In AddNTCCStdStrSPrintDriver\n"));
	
	try
	{
		
		//Save System Spool Directory in member variable
		
		::GetPrinterDriverDirectory(NULL,NULL,1,NULL,0,&needed);
		
		unsigned char *buffer=new unsigned char[needed];
		
		if(::GetPrinterDriverDirectory(NULL,NULL,1,buffer,needed,&needed))
			m_strNtSpoolDir=(LPTSTR) buffer;
		
		else
		{
			TraceEx(_T("Couldn't obtain system spool directory\n"));
			delete [] buffer;
			return FALSE;
		}
		
		delete [] buffer;
		
		m_strNtSpoolDir+=_T("\\");

		//then adjust it based on driver version
		Trace(_T("getting driver version\n"));

		int m_nDriverVersion=GetPDriverVersion(m_driverPath + m_driver);

		if(m_nDriverVersion<2 || m_nDriverVersion>3)
		{
			Trace(_T("Error... Illegal driver version!!! \n"));
			return FALSE;
		}

		if(m_nDriverVersion==3 && m_nOSVersion<5)
		{
			Trace(_T("Error can't add a W2K driver to NT4!!! \n"));
			return FALSE;
		}

		CStdString temp;

		TraceEx(_T("Got the system spool dir-> %s\n"),(LPCTSTR)m_strNtSpoolDir);
		
		//Save System Dir in member Var for addmonitor
		
		UINT size=::GetSystemDirectory(NULL,0);
		//CStdString temp;

		::GetSystemDirectory(m_strNTSysDir.GetBuffer(size),size);
		m_strNTSysDir.ReleaseBuffer();
		
		m_strNTSysDir+=_T("\\");

		TraceEx(_T("Got the system dir-> %s\n"),(LPCTSTR)m_strNTSysDir);
		
		//first see if it's already loaded
		TraceEx(_T("Checking if Driver is already loaded\n"));
		
		DWORD pcbNeeded,pcReturned;
		HGLOBAL hGlobal=NULL;
		DRIVER_INFO_1 *di1=NULL;
		BOOL ok=TRUE;
		BOOL found=FALSE;
		
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
				if(!_tcscmp(di1[i].pName,m_drivername))
				{
					TraceEx(_T("Found Driver is already loaded\n"));
					found=TRUE;
					break;
				}
			}
			
			if (di1 != NULL)
				::GlobalUnlock(hGlobal);
			
			if (hGlobal != NULL)
				::GlobalFree(hGlobal);
			
			if(found)
				return TRUE;                    

				
				
				if(!GetDrivers())
				{
					//make sure we have all the driverfiles 
					TraceEx(_T("Can't get drivers\n"));
					return FALSE;
				}
				
				
				//DEVMODE ls_devmode;
				//LPDEVMODE lprt_devmode=&ls_devmode;
							
				CStdString datafile = m_strNtSpoolDir + m_datafile;
				CStdString driver = m_strNtSpoolDir + m_driver;
				CStdString uifile = m_strNtSpoolDir + m_uifile;
				CStdString helpfile = m_strNtSpoolDir + m_helpfile;
				CStdString monitorfile=m_monitor;

				if(m_monitorname.CompareNoCase(_T("none")) || !m_monitorname.IsEmpty())
					monitorfile=m_strNtSpoolDir+monitorfile;

				CStdString monitorstring=m_monitorname;

				CStdString CopyFiles=m_copyfiles;
				
				TCHAR szBuf[2064]={NULL};
				
				CStdString strDependentFiles;
				strDependentFiles.Format("%s,%s,%s,%s",(LPCTSTR) datafile, (LPCTSTR) driver, (LPCTSTR) uifile, (LPCTSTR)helpfile);
				
				if(m_monitorname.IsEmpty()) //see if we have to add monitor
				{
					if(monitorfile.IsEmpty())
					{
						TraceEx(_T("Monitor String But no file!!"));
						return FALSE;
					}
					
					strDependentFiles+=_T(",")+monitorfile;
					
					if(!NtcssAddMonitor(monitorfile))
					{
						TraceEx(_T("Error couldn't add monitor file"));
						return FALSE;
					}
				}
				
				//need this @ top so we don't dup 5 main files &need to append path to copy files
				//so if copy files are there, use it completly
				
				if(CopyFiles.CompareNoCase(_T("none")))
				{
					CStdString strFile;
					
					CToken tok(m_copyfiles);
					tok.SetToken(_T(","));
					while(tok.MoreTokens())
					{
						strFile = tok.GetNextToken();
						strDependentFiles.Format("%s,%s",strDependentFiles,m_strNtSpoolDir+strFile);
					}
				}
				
				
				//make dependent file each Null terminated
				_tcscpy(szBuf,strDependentFiles.GetBuffer(strDependentFiles.GetLength()));
				LPTSTR p = szBuf;
				while (*p)
				{
					if (*p == ',')
						*p = '\0';
					p++;
				} 
				*p=NULL;
				
				strDependentFiles.ReleaseBuffer();
				

				DRIVER_INFO_3 ls_pinfo={m_nDriverVersion,(LPTSTR)(LPCTSTR)m_drivername, 
					_T("Windows NT x86"),
					(LPTSTR)(LPCTSTR)driver,
					(LPTSTR)(LPCTSTR)datafile,
					(LPTSTR)(LPCTSTR)uifile,
					(LPTSTR)(LPCTSTR)helpfile,
					szBuf,
					(!m_monitorname.IsEmpty()?
					(LPTSTR)(LPCTSTR)m_monitorname:NULL),NULL};
				DWORD error_code;
				BOOL ret;
				
				ret=::AddPrinterDriver(NULL,3,LPBYTE(&ls_pinfo));
				error_code=GetLastError();
				if(!ret)
					
					TraceEx(_T("Adding Printer Driver Failed-> \n") + GetSysError(error_code));
				
				return ret;
				
			
	}
	
	catch(...)
	{
		Trace(_T("Unknown Exception Caught\n"));
		return FALSE;
	}
	
}


CStdString CPrintUtil::GetSysError(DWORD dError)
{
	
	LPTSTR lpBuffer;
	CStdString strError;

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


BOOL CPrintUtil::NtcssAddMonitor(CStdString strMonitorDll)
{
	//NOTE DLL must be WINNT/SYSTEM32
	try
	{
		
		MONITOR_INFO_2 mi;
		
		::ZeroMemory(&mi, sizeof(mi));
		
		mi.pName=(LPTSTR)(LPCTSTR)m_monitorname;
		mi.pEnvironment=0;
		mi.pDLLName=(LPTSTR)(LPCTSTR)strMonitorDll;
		if((::AddMonitor(0,2,(LPBYTE)&mi)))
			return TRUE;
		else
			return (::GetLastError()==3006); //already loaded is OK
	}
	
	catch(...)
	{
		Trace(_T("Unknown Exception caught in NtcssAddMonitor\n"));
		return FALSE;
	}
}


int CPrintUtil::GetPDriverVersion(const CStdString& csDriverFile)
{
	
	CStdString csVer;
	DWORD dwVerHnd = 0 ;

	try
	{

	TraceEx(_T("In GetPDriverVersion() checking... %s\n"),(LPCTSTR) csDriverFile);

	DWORD dwVerInfoSize = ::GetFileVersionInfoSize((LPTSTR)(LPCTSTR)csDriverFile, &dwVerHnd ) ;
	if (( dwVerInfoSize > 0 ) && ( dwVerHnd == 0 ))
	{

		// allocate memory to hold it
		HGLOBAL hMem     = ::GlobalAlloc( GMEM_MOVEABLE, dwVerInfoSize ) ;
		LPVOID  pVerInfo = ::GlobalLock( hMem ) ;

		// get the block
		if ( ::GetFileVersionInfo((LPTSTR)(LPCTSTR)csDriverFile, dwVerHnd, dwVerInfoSize, pVerInfo ) )
		{
			
			char *pszInfo;
			DWORD dwError = 0L;
			unsigned int uiLen;

		
			if(::VerQueryValue(pVerInfo,"\\",(LPVOID*)&pszInfo,&uiLen))
			{
				VS_FIXEDFILEINFO *vv = (VS_FIXEDFILEINFO*)pszInfo;
				DWORD maj=HIWORD(vv->dwFileVersionMS);
				DWORD min=LOWORD(vv->dwFileVersionMS);
				csVer.Format("%u.%u",maj,min);
				
				Trace(_T("File Version for %s is %s\n"),(LPCTSTR) csDriverFile, (LPCTSTR) csVer);
			}
			
			::GlobalUnlock( hMem ) ;
			::GlobalFree( hMem ) ;
		}


			CStdString ver;
			ver.Format("%c",csVer.GetAt(2));
	
			return _ttoi(ver); 
	}

	else
		return -1;
	}

	catch(...)
	{
		Trace("Exception Caught in GetPDriverVersion() -> %d\n",::GetLastError());
		return -1;
	}

}


int CPrintUtil::GetOSVersion()
{
	//GET OS TYPE
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
		
	// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
	// which is supported on Windows 2000.
	//
	// If that fails, try using the OSVERSIONINFO structure.
	
	::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)))
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return -1;
	}
	
	if(osvi.dwPlatformId!=VER_PLATFORM_WIN32_NT)
		return -1;
	
	return osvi.dwMajorVersion;
}


BOOL CPrintUtil::AddNTCSSPrinter2K()
{

	//To handle adding 2K Printers without drivers
	//need to pass in port (Filename) printername & driver... make another constructor

    CStdString cmd;

	try
	{

		 Trace(_T("IN AddNTCSSPrinter2K(): Adding %s Printer Type is %s\n"),
			  (LPCTSTR) m_printername, (LPCTSTR) m_ntcssPrinterType);

        //Called for all printer types, but we only physically add FILE printers
        if(m_ntcssPrinterType == _T("NTSHARE"))
           return TRUE;  //we allow these in DB but no need to add them
		else
			if(m_ntcssPrinterType == _T("REGULAR") || 
			   m_ntcssPrinterType == _T("SERVER"))
			   return FALSE;

	    Trace("AddNTCSSPrinter2K(): Past Prt Type Check... Calling IsPrinter()\n");

	   //If its already added... we're done
	   if(IsPrinter())
		return TRUE;

	    Trace("AddNTCSSPrinter2K(): Is2KPrinterAdded() was false\n");


	   if(!AddNTCSSPort())
			throw(_T("Adding Printer Port Failed!!"));

	   Trace("In AddNTCSSPrinter2K() Past Adding Port... Hitting DLL\n");


	//Try to add the printer using printui.dll & the 2K INF FILE
	//will prompt if driver not found
	cmd.Format(_T("rundll32 printui.dll,PrintUIEntry /if /b \"%s\" \
		/f %%windir%%\\inf\\ntprint.inf /r \"%s\" /m \"%s\"  /u /w /q"),
		(LPCTSTR) m_printername, (LPCTSTR) m_port, (LPCTSTR) m_drivername);

	Trace("%s\n",(LPCTSTR) cmd);


	_tsystem(cmd); //return value is useless

	//If it didn't get added ... run add printer Utility 
	//problem here is it 

	 Trace("AddNTCSSPrinter2K(): Calling IsPrinter()\n");


	if(!IsPrinter())
		return FALSE;

	Trace("AddNTCSSPrinter2K(): IsPrinter() returned TRUE\n");


	return TRUE;

	}


	catch(LPTSTR pStr)
	{
		Trace(_T("%s\n"),pStr);
		return FALSE;
	}

	catch(...)
	{
        Trace(_T("AddNTCSSPrinter2K() Unknown exception caught\n"));
		return FALSE;
	}

}

BOOL CPrintUtil::IsPrinter()
{
	
	HANDLE lh_Printer;
	PRINTER_DEFAULTS pd;
	BOOL bReturn=FALSE;
	
	try
	{
		::ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		bReturn=::OpenPrinter((LPTSTR)(LPCTSTR)m_printername,(LPHANDLE)(&lh_Printer),&pd);
				
		if (lh_Printer != NULL)
			::ClosePrinter(lh_Printer);

		return bReturn;
	
	}
	
	catch(...)
	{
		Trace(_T("Exception caught in Is2KPrinterAdded()\n"));
		return FALSE;
	}
	
}


BOOL CPrintUtil::AddSharePrinter()
{
	
	
	try
	{
		
		Trace(_T("In Native AddSharePrinter(): adding-> %s\n"),(LPCTSTR) m_printername);
		
		if(!::AddPrinterConnection((LPTSTR)(LPCTSTR)m_printername))
		{
			DWORD error=::GetLastError();
			CStdString tmp;
			Trace("AddSharePrinter(): Error # is-> %d",error);
			return FALSE;
		}
		else
			return TRUE;
		
	}
	
	catch(...)
	{
		Trace(_T("Exception caught in AddNtcssSharePrinter()\n"));
		return FALSE;
	}
	
}


BOOL CPrintUtil::DeleteSharePrinter()
{

	try
	{
		
		Trace(_T("In Native DeleteSharePrinter(): deleting-> %s\n"),(LPCTSTR) m_printername);
		
		if(!::DeletePrinterConnection((LPTSTR)(LPCTSTR)m_printername))
		{
			DWORD error=::GetLastError();
			CStdString tmp;
			Trace("DeleteSharePrinter(): Error # is-> %d",error);
			return FALSE;
		}
		else
			return TRUE;
		
	}
	
	catch(...)
	{
		Trace(_T("Exception caught in DeleteSharePrinter()\n"));
		return FALSE;
	}

}
