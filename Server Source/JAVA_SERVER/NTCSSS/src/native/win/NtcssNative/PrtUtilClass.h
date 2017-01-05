/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "windows.h"
#include "StdString.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Exists just for physically adding & deleting printers to the server.

class CPrintUtil
{
public:
	CPrintUtil(LPCTSTR printername,LPCTSTR drivername,
		       LPCTSTR driver,LPCTSTR datafile,LPCTSTR uifile,
			   LPCTSTR helpfile,LPCTSTR monitorname,LPCTSTR monitor,
			   LPCTSTR copyfiles,LPCTSTR ntcssPrinterType,LPCTSTR port,LPCTSTR driverPath);	

	//Constructor for Deletes
    CPrintUtil(LPCTSTR printername, LPCTSTR printingmethod); 

	//Constructor for W2K adds
	CPrintUtil(LPCTSTR printername, LPCTSTR filename, LPCTSTR driver, LPCTSTR printingmethod);


    BOOL AddNTCSSPort(); 
	BOOL DelNTCSSPrinter();
	BOOL AddNTCSSPrinter();
	BOOL AddNTCSSPrinter2K();
//	BOOL DelNTCSSPort();
	

private:

	CStdString m_printername,m_drivername,m_driver,m_datafile,
	m_uifile,m_helpfile,m_monitorname,m_monitor,m_copyfiles,m_driverPath,
	m_ntcssPrinterType,m_port;
	CStdString m_strNtSpoolDir,m_strNTSysDir;
	BOOL GetDrivers();
	CStdString GetSysError(DWORD dError);
	BOOL AddNTCSSPrintDriver();
	BOOL NtcssAddMonitor(CStdString strMonitorDll);
	int GetPDriverVersion(const CStdString& csDriverFile);
	int GetOSVersion();
	int m_nOSVersion,m_nDriverVersion;
	BOOL IsPort(LPCTSTR port);
	BOOL IsPrinter();
	BOOL DeleteSharePrinter();
	BOOL AddSharePrinter();

};

