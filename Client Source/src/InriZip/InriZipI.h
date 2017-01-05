/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


#define DllImport __declspec(dllimport) WINAPI
#define DllExport __declspec(dllexport) WINAPI


int DllImport AddFile(CString FileName);
int DllImport ZipFile(CString FileName);
