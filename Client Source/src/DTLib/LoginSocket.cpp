/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "loginsocket.h"
#include <stddef.h> 

IMPLEMENT_DYNAMIC(CLoginSocket, CSocket)

CLoginSocket::CLoginSocket(CPriPage* ctd)
{
	dlgptr=ctd;

}

void CLoginSocket::OnReceive(int nErrorCode)
{

   	CSocket::OnReceive(nErrorCode);

	dlgptr->ProcessReadEvent();
	
//	CheckWnd();

}
void CLoginSocket::OnClose(int nErrorCode)
{

   	CSocket::OnClose(nErrorCode);


}

void CLoginSocket::OnConnect(int nErrorCode)
{

   	CSocket::OnConnect(nErrorCode);
}
