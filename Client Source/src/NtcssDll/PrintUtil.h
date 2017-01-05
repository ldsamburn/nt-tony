/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/


class CPrintUtil
{
public:

//	BOOL StartStopSpooler(char *cmd_line);
	BOOL DelTempPrinter();
	BOOL AddNTCSSPort();
	BOOL DelNTCSSPort();
	BOOL IsNtcssPort() ;
	char m_SpoolFileName[_MAX_FNAME];
};
