/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

#include "stdafx.h"
#include "lmaccess.h"
#include "lmapibuf.h"
#include "sizes.h"
#include "tchar.h"
//#include "AddACL.h"

BOOL NtcssDupGlobalNTGroups(TCHAR *loginname,
							TCHAR *temp_user,BOOL domain_flag,TCHAR *server); 
BOOL NtcssDupLocalNTGroups(TCHAR *loginname,
							TCHAR *temp_user,BOOL domain_flag,TCHAR *server);
int SyncPDC (char *dc_name);

SECURITY_DESCRIPTOR* NtcssGetFileSecurity( const TCHAR *filename, 
										  SECURITY_INFORMATION whichParts);

BOOL CreateDirectoryAndWait(const char *dir);
BOOL DisplayError(int nError);
BOOL DisplayMessage(char *msg);


//#include <rpc.h>

#define SD_SIZE (65536 + SECURITY_DESCRIPTOR_MIN_LENGTH)

BOOL GetTextualSid (PSID pSid, LPSTR TextualSid, LPDWORD dwBufferLen)
{
	PSID_IDENTIFIER_AUTHORITY psia;
	DWORD dwSubAuthorities;
	DWORD dwSidRev=SID_REVISION;
	DWORD dwCounter;
	DWORD dwSidSize;
	if (!IsValidSid (pSid)) return (FALSE);

	psia = GetSidIdentifierAuthority (pSid);
	dwSubAuthorities = *GetSidSubAuthorityCount (pSid);
	dwSidSize = (15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof (TCHAR);

	if (*dwBufferLen < dwSidSize)
	{
		*dwBufferLen = dwSidSize;
		SetLastError (ERROR_INSUFFICIENT_BUFFER);
		return (FALSE);
	}

	dwSidSize = wsprintf (TextualSid, TEXT ("S-%lu-"), dwSidRev);
	if ((psia->Value[0] != 0) || (psia->Value[1] != 0))
	{
		dwSidSize += wsprintf (TextualSid + lstrlen (TextualSid),
				TEXT ("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
				(USHORT)psia->Value[0], (USHORT)psia->Value[1],
				(USHORT)psia->Value[2], (USHORT)psia->Value[3],
				(USHORT)psia->Value[4], (USHORT)psia->Value[5]);
	}
	else
	{
		dwSidSize += wsprintf (TextualSid + lstrlen (TextualSid),
				TEXT ("%lu"), (ULONG)(psia->Value[5]) +
				(ULONG)(psia->Value[4] << 8) +
				(ULONG)(psia->Value[3] << 16) +
				(ULONG)(psia->Value[2] << 24));
	}

	for (dwCounter = 0; dwCounter < dwSubAuthorities; dwCounter++)
	{
		dwSidSize += wsprintf (TextualSid + dwSidSize, TEXT("-%lu"),
				*GetSidSubAuthority (pSid, dwCounter));
	}
	return TRUE;
}



BOOL DumpSID (PSID pSID, LPTSTR lpSIDName, LPTSTR lpNameType)
{
	TCHAR szAccount [256];
	DWORD dwAccountSize = sizeof (szAccount);
	TCHAR szDomain [256];
	DWORD dwDomainSize = sizeof (szDomain);
	SID_NAME_USE SidNameUse;
	char sidString[1024];
	DWORD sidSize = 1024;


	if (!LookupAccountSid (NULL, pSID, szAccount, &dwAccountSize,
			szDomain, &dwDomainSize, &SidNameUse))
	{
		printf ("Error %d:LookupAccountSid\n", GetLastError ());
		return (FALSE);
	}
	
	sprintf (lpSIDName, "%s", szAccount);
	if (lpNameType != NULL)
		sprintf (lpNameType, "%s", szDomain);

//	printf ("\nSidNameUse = %d\n", SidNameUse);
	GetTextualSid (pSID, sidString, &sidSize);
	printf ("\nSID - %s\n", sidString);

	return TRUE;
}

BOOL DumpSID2 (PSID pSID, LPTSTR lpAccount)
{
	TCHAR szAccount [256];
	DWORD dwAccountSize = sizeof (szAccount);
	TCHAR szDomain [256];
	DWORD dwDomainSize = sizeof (szDomain);
	SID_NAME_USE SidNameUse;
	char sidString[1024];
	DWORD sidSize = 1024;

	if (!LookupAccountSid (NULL, pSID, szAccount, &dwAccountSize,
			szDomain, &dwDomainSize, &SidNameUse))
	{
		printf ("Error %d:LookupAccountSid\n", GetLastError ());
		return (FALSE);
	}
	
	//printf ("\nAccount = %s  Domain = %s", szAccount, szDomain);
	sprintf (lpAccount, "%s", szAccount);

//	printf ("\nSidNameUse = %d\n", SidNameUse);
	GetTextualSid (pSID, sidString, &sidSize);
	printf ("\nSID - %s\n", sidString);

	return TRUE;
}

VOID DumpACEs (PACL pACL, LPTSTR lpFileName)
{
	BYTE byBuffer [1024];
	ACL_SIZE_INFORMATION *pACLSize = (ACL_SIZE_INFORMATION *) byBuffer;
	LPVOID pACE;
	ACE_HEADER *pACEHeader;
	LPBYTE lpTemp;
	DWORD dwAccessMask;
	PSID pSID;
	LPTSTR lpVerb;
	TCHAR szSIDName [256];
	TCHAR szNameType [256];
	DWORD i;

	if (pACL == NULL)
	{
		printf ("\nDumpACEs () passed a NULL ACL pointer");
		return;
	}

	if (!GetAclInformation (pACL, byBuffer, sizeof (byBuffer),
			AclSizeInformation))
	{
		printf ("Error %d:GetAclInformation\n", GetLastError ());
		return;
	}

	for (i = 0; i < pACLSize->AceCount; ++i)
	{
		if (!GetAce (pACL, i, &pACE))
		{
			printf ("Error %d:GetAce\n", GetLastError ());
			continue;
		}
		pACEHeader = (ACE_HEADER *) pACE;
		lpTemp = (LPBYTE) pACE;

		lpTemp += sizeof (ACE_HEADER);
		dwAccessMask = *(ACCESS_MASK *) lpTemp;
		lpTemp += sizeof (ACCESS_MASK);
		pSID = (PSID) lpTemp;

		switch (pACEHeader->AceType)
		{
		case ACCESS_ALLOWED_ACE_TYPE:
			printf ("\nAccess allowed ace");
			lpVerb = "granted to";
			break;
		case ACCESS_DENIED_ACE_TYPE:
			printf ("\nAccess denied ace");
			lpVerb = "denied to";
			break;
		case SYSTEM_AUDIT_ACE_TYPE:
			printf ("\nSystem audit ace");
			lpVerb = "being audited for";
			break;
		case SYSTEM_ALARM_ACE_TYPE:
			printf ("\nSystem alarm ace");
			lpVerb = "being audited for";
			break;
		}
		if (DumpSID (pSID, szSIDName, szNameType))
		{
			printf ("Access header flags = %08X\n", pACEHeader->AceFlags);
			printf ("Access rights %08X %s %s on %s in domain %s",
					dwAccessMask, lpVerb, szSIDName, lpFileName,
					szNameType);
		}
	}
}

VOID DumpACL (LPTSTR lpFileName)
{
	PACL pACL = NULL;
	BOOL bDACLPresent;
	BOOL bDACLDefaulted;
	BYTE bySDBuffer [8192];
	PSECURITY_DESCRIPTOR pSD =
			(PSECURITY_DESCRIPTOR) bySDBuffer;
	DWORD dwSDSize;

	dwSDSize = sizeof (bySDBuffer);
	if (!GetFileSecurity (lpFileName, (SECURITY_INFORMATION)
			(DACL_SECURITY_INFORMATION), pSD, dwSDSize, &dwSDSize))
	{
		printf ("Error %d:GetFileSecurity\n", GetLastError ());
		return;
	}


	if (!GetSecurityDescriptorDacl (pSD, &bDACLPresent, &pACL,
			&bDACLDefaulted))
	{
		printf ("Error %d:GetSecurityDescriptorDacl\n", GetLastError ());
		return;
	}
	if (bDACLPresent)
		printf ("\nDACL is present\n");
	else
		printf ("\nDACL is not present\n");
	if (bDACLDefaulted)
		printf ("\nDACL provided by default mechanism\n");
	else
		printf ("\nDACL explicitly assigned\n");

	if (pACL != NULL)
		DumpACEs (pACL, lpFileName);
	else
		printf ("\n%s has a NULL DACL", lpFileName);

	printf ("\n");

}

BOOL AddDiscACL (char *pFileName, DWORD dwAcessMask,char *lpszUserName)
{
	UCHAR					NewSD [SECURITY_DESCRIPTOR_MIN_LENGTH];
	PSECURITY_DESCRIPTOR	psdNewSD = (PSECURITY_DESCRIPTOR)NewSD;
	PACL	pNewACL;
	DWORD	dwNewACLSize;
	UCHAR psnuType [2048];
	char  *lpszDomain;
	DWORD dwDomainLength = 250;
	UCHAR UserSID [1024];
	DWORD dwSIDBufSize = 1024;
//	char *lpszUserName;

//	lpszUserName = (char *) malloc (250);
//	sprintf (lpszUserName, "Everyone");

	lpszDomain = (char *) malloc (250);
	if (!LookupAccountName ((LPSTR) NULL, lpszUserName, UserSID,
			&dwSIDBufSize, lpszDomain, &dwDomainLength,
			(PSID_NAME_USE) psnuType))
	{
		printf ("Error: %d:LookupAccountName\n", GetLastError ());
		return (FALSE);
	}

	if (!InitializeSecurityDescriptor (psdNewSD,
			SECURITY_DESCRIPTOR_REVISION))
	{
		printf ("Error %d:InitializeSecurityDescriptor\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	//Compute size needed for the new ACL
	dwNewACLSize = sizeof (ACCESS_ALLOWED_ACE) + sizeof (ACCESS_ALLOWED_ACE) +
			GetLengthSid (UserSID) - sizeof (DWORD);

	//Allocate memory for new ACL
	pNewACL = (PACL) LocalAlloc (LPTR, dwNewACLSize);

	//Initialize the new ACL
	if (!InitializeAcl (pNewACL, dwNewACLSize, ACL_REVISION2))
	{
		printf ("Error %d:InitializeAcl\n", GetLastError ());
		LocalFree ((HLOCAL) pNewACL);
		return (FALSE);
	}

	//Add the access-allowed ACE to the new DACL
	if (!AddAccessAllowedAce (pNewACL, ACL_REVISION, dwAcessMask, &UserSID))
	{
		printf ("Error %d:AddAccessAllowedAce\n", GetLastError ());
		LocalFree ((HLOCAL) pNewACL);
		return (FALSE);
	}

	//Add a NULL disc. ACL to the security descriptor
	if (!SetSecurityDescriptorDacl (psdNewSD, TRUE, pNewACL, FALSE))
	{
		printf ("Error %d:SetSecurityDescriptorDacl\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	//Add the security descriptor to the file
	if (!SetFileSecurity (pFileName, DACL_SECURITY_INFORMATION, psdNewSD))
	{
		printf ("Error %d:SetFileSecurity\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	LocalFree ((HLOCAL) psdNewSD);
	return (TRUE);
}

BOOL AddAccessRights (char *pFileName, DWORD dwAcessMask, BYTE Flags, char *lpszUserName)
{
	UCHAR					NewSD [SECURITY_DESCRIPTOR_MIN_LENGTH];
	PSECURITY_DESCRIPTOR	psdNewSD = (PSECURITY_DESCRIPTOR)NewSD;
	PACL	pNewACL;
	DWORD	dwNewACLSize;
	UCHAR psnuType [2048];
	char  *lpszDomain;
	DWORD dwDomainLength = 250;
	UCHAR UserSID [1024];
	DWORD dwSIDBufSize = 1024;
//	char *lpszUserName;
	//File SID variables
	UCHAR					ucSDbuf [SD_SIZE];
	PSECURITY_DESCRIPTOR	pFileSD = (PSECURITY_DESCRIPTOR) ucSDbuf;
	DWORD					dwSDLengthNeeded;
	//ACL variables
	PACL					pACL;
	BOOL					bDaclPresent;
	BOOL					bDaclDefaulted;
	ACL_SIZE_INFORMATION	AclInfo;
	//Temporary ACE
	PVOID pTempAce;
	UINT CurrentAceIndex;

	//lpszUserName = (char *) malloc (250);
	//sprintf (lpszUserName, "Everyone");

	lpszDomain = (char *) malloc (250);
	if (!LookupAccountName ((LPSTR) NULL, lpszUserName, UserSID,
			&dwSIDBufSize, lpszDomain, &dwDomainLength,
			(PSID_NAME_USE) psnuType))
	{
		printf ("Error: %d:LookupAccountName\n", GetLastError ());
		return (FALSE);
	}

	if (!InitializeSecurityDescriptor (psdNewSD,
			SECURITY_DESCRIPTOR_REVISION))
	{
		printf ("Error %d:InitializeSecurityDescriptor\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	if (!GetFileSecurity (pFileName, (SECURITY_INFORMATION)
			(DACL_SECURITY_INFORMATION), pFileSD, SD_SIZE,
			(LPDWORD)&dwSDLengthNeeded))
	{
		printf ("Error %d:GetFileSecurity\n", GetLastError ());
		return (FALSE);
	}

	//Get DACL from SD
	if (!GetSecurityDescriptorDacl (pFileSD, &bDaclPresent, &pACL,
			&bDaclDefaulted))
	{
		printf ("Error %d:GetSecurityDescriptorDacl\n", GetLastError ());
		return (FALSE);
	}

	//Get file ACL size information
	if (!GetAclInformation (pACL, &AclInfo, sizeof (ACL_SIZE_INFORMATION),
			AclSizeInformation))
	{
		printf ("Error %d:GetAclInformation\n", GetLastError ());
		return (FALSE);
	}

	//Compute size needed for the new ACL
//	dwNewACLSize = sizeof (ACCESS_ALLOWED_ACE) + sizeof (ACCESS_ALLOWED_ACE) +
//			GetLengthSid (UserSID) - sizeof (DWORD);
	//Compute size needed for the new ACL
	dwNewACLSize = AclInfo.AclBytesInUse + sizeof (ACCESS_ALLOWED_ACE) +
			GetLengthSid (UserSID) - sizeof (DWORD);

	//Allocate memory for new ACL
	pNewACL = (PACL) LocalAlloc (LPTR, dwNewACLSize);

	//Initialize the new ACL
	if (!InitializeAcl (pNewACL, dwNewACLSize, ACL_REVISION2))
	{
		printf ("Error %d:InitializeAcl\n", GetLastError ());
		LocalFree ((HLOCAL) pNewACL);
		return (FALSE);
	}

	//If DACL is present, copy it to a new DACL
	if (bDaclPresent)
	{
		//Copy the file's ACEs to our new ACL
		if (AclInfo.AceCount)
		{
			for (CurrentAceIndex = 0; CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++)
			{
				//Get an ACE
				if (!GetAce (pACL, CurrentAceIndex, &pTempAce))
				{
					printf ("Error %d:GetAce\n", GetLastError ());
					LocalFree ((HLOCAL) pNewACL);
					return (FALSE);
				}

				//Add the ACE to the new ACL
				if (!AddAce (pNewACL, ACL_REVISION, MAXDWORD,
						pTempAce, ((PACE_HEADER)pTempAce)->AceSize))
				{
					printf ("Error %d:AddAce\n", GetLastError ());
					LocalFree ((HLOCAL) pNewACL);
					return (FALSE);
				}
			}
		}
	}

	//Add the access-allowed ACE to the new DACL
	if (!AddAccessAllowedAce (pNewACL, ACL_REVISION, dwAcessMask, &UserSID))
	{
		printf ("Error %d:AddAccessAllowedAce\n", GetLastError ());
		LocalFree ((HLOCAL) pNewACL);
		return (FALSE);
	}

	if (Flags != NULL)
	{
		if (!GetAce (pNewACL, AclInfo.AceCount, &pTempAce))
		{
			printf ("Error %d:GetAce\n", GetLastError ());
			LocalFree ((HLOCAL) pNewACL);
			return (FALSE);
		}

		ACE_HEADER ACEHeader;
		ACEHeader = *(ACE_HEADER *) pTempAce;
		ACEHeader.AceFlags = Flags;
		((ACCESS_ALLOWED_ACE *)pTempAce)->Header = ACEHeader;

	}

	//Add a NULL disc. ACL to the security descriptor
	if (!SetSecurityDescriptorDacl (psdNewSD, TRUE, pNewACL, FALSE))
	{
		printf ("Error %d:SetSecurityDescriptorDacl\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	//Add the security descriptor to the file
	if (!SetFileSecurity (pFileName, DACL_SECURITY_INFORMATION, psdNewSD))
	{
		printf ("Error %d:SetFileSecurity\n", GetLastError ());
		LocalFree ((HLOCAL) psdNewSD);
		return (FALSE);
	}

	LocalFree ((HLOCAL) psdNewSD);
	return (TRUE);
}


#define RTN_OK 0 
#define RTN_USAGE 1 
#define RTN_ERROR 13 
#define TRY_COUNT 75
 
extern "C" int __declspec(dllexport) WINAPI SetFileAccess(char *FileName,char *Operation,char *Trustee)
{ 
    DWORD AccessMask = GENERIC_ALL; 
    DWORD InheritFlag = NO_INHERITANCE; 
    ACCESS_MODE option; 
    EXPLICIT_ACCESS explicitaccess; 
 
    PACL ExistingDacl;    
    PACL NewAcl = NULL; 
    PSECURITY_DESCRIPTOR psd = NULL; 
 
    DWORD dwError; 
    BOOL bSuccess = FALSE; // assume failure 

 
    if ( (0 == stricmp(Operation, "DENY") ) || 
        (0 == stricmp(Operation, "D") ) ) 
    { 
      option = DENY_ACCESS; 
	  AccessMask=0xfffffff;
    } else if ( ( (0 == stricmp(Operation, "REVOKE") ) || 
                 (0 == stricmp(Operation, "R") ) ) ) 
    { 
      option = REVOKE_ACCESS; 
	  AccessMask=0xfffffff;
    } else if ( (0 == stricmp(Operation, "SET") ) || 
               (0 == stricmp(Operation, "S") ) ) 
    { 
      option = SET_ACCESS; 
	  AccessMask=0xfffffff;
    } else if ( (0 == stricmp(Operation, "GRANT") ) || 
               (0 == stricmp(Operation, "G") ) ) 
    { 
      option = GRANT_ACCESS; 
	  AccessMask=SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    } else { 
        printf("Invalid action specified\n"); 
        return RTN_ERROR; 
    } 
 
	/* This loop accounts for a PDC lazy write condition when
	   the account is added */
 	int trycnt=0;

	while (trycnt < TRY_COUNT && bSuccess != TRUE) {

		InheritFlag = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;

		psd=NtcssGetFileSecurity( FileName, DACL_SECURITY_INFORMATION); 
	

		BOOL bDaclPresent;
		BOOL bDaclDefaulted;

		bSuccess=::GetSecurityDescriptorDacl(psd,		// address of security descriptor
					&bDaclPresent,					// address of flag for presence of disc. ACL
					&ExistingDacl,					// address of pointer to ACL
					&bDaclDefaulted);			    // address of flag for default disc. ACL

 		if (bSuccess == FALSE)
				goto cleanup; 

		BuildExplicitAccessWithName( 
				&explicitaccess, 
				Trustee, 
				AccessMask, 
				option, 
				InheritFlag); 

		// 
		// add specified access to the object 
		// 

		if (option == REVOKE_ACCESS)
			dwError = SetEntriesInAcl(
				1,
				&explicitaccess, 
				NULL,
				&NewAcl); 
		else
			dwError = SetEntriesInAcl(
				1,
				&explicitaccess, 
				ExistingDacl, 
				&NewAcl); 


		if(dwError != ERROR_SUCCESS) { 
			trycnt++;
			goto cleanup; 
		} 
 
		// 
		// apply new security to file 
		// 
 
		dwError = SetNamedSecurityInfo( 
						FileName, 
						SE_FILE_OBJECT, // object type 
						DACL_SECURITY_INFORMATION, 
						NULL, 
						NULL, 
						NewAcl, 
						NULL); 
 
		if(dwError != ERROR_SUCCESS) { 
			trycnt++;
			goto cleanup;
		} 
 
		bSuccess = TRUE; // indicate success 
 
	cleanup: 
 
		if( NewAcl != NULL ) LocalFree( NewAcl );
		if( ExistingDacl != NULL ) LocalFree( ExistingDacl );
		if( psd != NULL) delete [] psd;

		if (bSuccess != TRUE)
				Sleep(300);
 
 
	}              /* end of try loop */

    if(!bSuccess) 
        return RTN_ERROR;
	
 
    return RTN_OK; 
} 

extern "C" BOOL __declspec(dllexport) WINAPI NtcssSavePas(const char *strHomeDir,
				  const char *strPAS)
{
char strPASFile[SIZE_CMD_LINE];

	sprintf(strPASFile,"%s\\%s",strHomeDir,"ntcss.svr");
	FILE *fp=fopen(strPASFile,"w");
	if(!fp) return(-1);
	fprintf(fp,"%s\n",strPAS);
	fclose(fp);

	return(0);
}

extern "C" BOOL __declspec(dllexport) WINAPI NtcssAddUserToNT(char *loginname,
														 char *password,
														 char *realname,
														 char *homedir,
														 char *profilepath,
														 char *temp_user, 
														 BOOL domain_flag, 
														 char *pas,
														 int NtcssNTDelay,
														 int adminFlag)
{
  USER_INFO_3 usr;
  USER_INFO_3 *pusr,*pusr1;
  TCHAR w_name[32];   
  TCHAR w_temp_user[32];
  TCHAR w_password [32];
  TCHAR w_realname[SIZE_REAL_NAME+1];  
  TCHAR w_homedir[SIZE_CMD_LINE+1]; 
  TCHAR w_profilepath[SIZE_CMD_LINE+1];  
  TCHAR w_ntgroup[64];
  char temp_admin_user[32];
  char new_login[64];  
  DWORD net_ret, net_err;    
  char dn[SIZE_CMD_LINE];
  TCHAR w_dn[SIZE_CMD_LINE];
  int dnlen;                      
  char dn_server[64];
  LPWSTR pdn_wserver;   
  LOCALGROUP_MEMBERS_INFO_3 lgm_struct;
  SID user_sid;
  ULONG user_sid_size;
  char ref_domain[SIZE_CMD_LINE];
  ULONG ref_domain_size;      
  int sid_cnt;
  int use_temp_user=0;
  //char strError[100];


 
    memset(&usr,NULL,sizeof(USER_INFO_3));
	net_ret=CreateDirectoryAndWait(homedir);

	if (net_ret == 1) {  
//		MessageBox(NULL,"Unable to create users home directory, aborting",
//						"NTCSS Error",MB_OK);
		DisplayMessage("Creating Home Dir");

		return(1);
	}

    Sleep((int)NtcssNTDelay  * 1000);

	memset(temp_admin_user,NULL,32);

	if (temp_user) {
		if (adminFlag)
			sprintf(temp_admin_user,"%s-Admin",temp_user);
		else
			strcpy(temp_admin_user,temp_user);
	}

    pdn_wserver=NULL;
    memset(dn_server,NULL,64);

    memset(dn,NULL,SIZE_CMD_LINE);   

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CMD_LINE);
    if (dnlen <= 0) {
		DisplayMessage("Getting Domain Name");
        return (1);
    }       

    if (domain_flag == 1) {

        memset(w_dn,0,SIZE_CMD_LINE);

        net_ret=MultiByteToWideChar(0,0,dn,
                 SIZE_CMD_LINE,(LPWSTR)w_dn,SIZE_CMD_LINE); 

        net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);    
        net_ret=WideCharToMultiByte(0,0,(LPWSTR)pdn_wserver,-1,
                        dn_server,64,NULL,NULL);  

	} else {
		strcpy(dn_server,dn);
	}

	///////// add user to system
    net_ret=MultiByteToWideChar(0,0,loginname,
                  -1,(LPWSTR)w_name,32);

    net_ret=MultiByteToWideChar(0,0,password,
                  -1,(LPWSTR)w_password,14);   

    net_ret=MultiByteToWideChar(0,0,realname,
                 -1,(LPWSTR)w_realname,64);   

    net_ret=MultiByteToWideChar(0,0,homedir,
                 -1,(LPWSTR)w_homedir,SIZE_CMD_LINE); 

    if (profilepath) {
      net_ret=MultiByteToWideChar(0,0,profilepath,
                 -1,(LPWSTR)w_profilepath,SIZE_CMD_LINE);   
    }


	// get user
    pusr1=NULL;
    net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_name,
                             3,(LPBYTE *)&pusr1); 

    if (net_ret != 0) {
		pusr1=NULL;  // just make sure
	}


	pusr=NULL;
	if (temp_user != NULL) {
		use_temp_user = 1;
        net_ret=MultiByteToWideChar(0,0,temp_admin_user,
                       -1,(LPWSTR)w_temp_user,32); 

        net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_temp_user,
                             3,(LPBYTE *)&pusr); 
	
        if (net_ret != 0) {
			DisplayMessage("Using Default Template");
	 	    use_temp_user = 0;
		}
	}


    if (!use_temp_user) {  

        usr.usri3_comment=NULL;     
 
        usr.usri3_priv=USER_PRIV_USER;  
        usr.usri3_flags=UF_NORMAL_ACCOUNT | UF_SCRIPT;
        usr.usri3_script_path=NULL; 

        usr.usri3_auth_flags=0; 
        usr.usri3_usr_comment=NULL; 
        usr.usri3_parms=NULL;
        usr.usri3_workstations=NULL; 
        usr.usri3_last_logon=0; 
        usr.usri3_last_logoff=0; 
        usr.usri3_acct_expires=TIMEQ_FOREVER;   
        usr.usri3_max_storage=USER_MAXSTORAGE_UNLIMITED;   
        usr.usri3_units_per_week=0;   
        usr.usri3_logon_hours=NULL;   
        usr.usri3_bad_pw_count=3;   
        usr.usri3_num_logons=0;   
        usr.usri3_logon_server=NULL;   
        usr.usri3_country_code=0;   
        usr.usri3_code_page=0; 
        usr.usri3_user_id=0;   
        usr.usri3_primary_group_id=DOMAIN_GROUP_RID_USERS;
        if (profilepath) 
          usr.usri3_profile=(LPWSTR)w_profilepath;
        else 
          usr.usri3_profile=NULL;
      
        usr.usri3_home_dir_drive=NULL;  
        usr.usri3_password_expired=0;  
    } else  {

		  WideCharToMultiByte(0,0,(LPWSTR)pusr->usri3_name,-1,
							 new_login,64,NULL,NULL); 
		  
		  usr.usri3_comment = pusr->usri3_comment;

		  usr.usri3_priv=pusr->usri3_priv;  
		  usr.usri3_flags=UF_NORMAL_ACCOUNT + UF_SCRIPT;

		  usr.usri3_script_path=pusr->usri3_script_path;

		  usr.usri3_auth_flags=pusr->usri3_auth_flags; 

		  usr.usri3_usr_comment=pusr->usri3_usr_comment;

		  usr.usri3_parms=pusr->usri3_parms;

		  usr.usri3_workstations=pusr->usri3_workstations; 
		  usr.usri3_last_logon=0; 
		  usr.usri3_last_logoff=0; 
		  usr.usri3_acct_expires=pusr->usri3_acct_expires;   
		  usr.usri3_max_storage=pusr->usri3_max_storage;   
		  usr.usri3_units_per_week=pusr->usri3_units_per_week;   
		  usr.usri3_logon_hours=pusr->usri3_logon_hours;   
		  usr.usri3_bad_pw_count=pusr->usri3_bad_pw_count;   
		  usr.usri3_num_logons=pusr->usri3_num_logons;   
		  usr.usri3_logon_server=pusr->usri3_logon_server;   
		  usr.usri3_country_code=pusr->usri3_country_code;   
		  usr.usri3_code_page=pusr->usri3_code_page; 
		  usr.usri3_user_id=0;   
		  usr.usri3_primary_group_id=DOMAIN_GROUP_RID_USERS;  
		  if (profilepath) 
			usr.usri3_profile=(LPWSTR)w_profilepath;
		  else 
			usr.usri3_profile=NULL;
    
		  usr.usri3_home_dir_drive=pusr->usri3_home_dir_drive;  
		  usr.usri3_password_expired=0;
	}

    usr.usri3_name=(LPWSTR)w_name; 
    usr.usri3_password=(LPWSTR)w_password;     
    usr.usri3_full_name=(LPWSTR)w_realname;  
    usr.usri3_home_dir=(LPWSTR)w_homedir;   
	
	if (domain_flag) {
		if (pusr1 == NULL) 
			net_ret=NetUserAdd((LPWSTR)pdn_wserver,3,(LPBYTE)&usr,&net_err); 
		else
	        net_ret=NetUserSetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_name,3,
                           (LPBYTE)&usr, &net_err); 

    } else {  
		if (pusr1 == NULL) {
	        net_ret=NetUserAdd(NULL,3,(LPBYTE)&usr,&net_err);
		} else
	        net_ret=NetUserSetInfo((LPWSTR)NULL,(LPWSTR)w_name,3,
                           (LPBYTE)&usr, &net_err); 

	}    


	////////////////////////////

    // Add Users to NTCSS Base Group
	if (net_ret == 0)  {  

		memset(w_ntgroup,NULL,64);
		net_ret=MultiByteToWideChar(0,0,"NTCSS",
                -1,(LPWSTR)w_ntgroup,64);    

		if (domain_flag == 0) {
				lgm_struct.lgrmi3_domainandname=(LPWSTR)w_name;
				net_ret=NetLocalGroupAddMembers((LPWSTR)pdn_wserver,(LPWSTR)w_ntgroup,
                    3,(LPBYTE)&lgm_struct,1);  
		} else {
				net_ret=NetGroupAddUser((LPWSTR)pdn_wserver,(LPWSTR)w_ntgroup,
					                    (LPWSTR)w_name);
		}


       // adding to all group local and global
	   if (use_temp_user && net_ret == 0) {                   
            // only do global if domain is set

				if (domain_flag == 1)  {
					net_ret = NtcssDupGlobalNTGroups(w_name,w_temp_user,
                                        domain_flag,(TCHAR *)pdn_wserver);


				}
            // do local regardless
				if (net_ret == 0) {
					net_ret = NtcssDupLocalNTGroups(w_name,w_temp_user,
                                        domain_flag,(TCHAR *)pdn_wserver);
				} 
		} 

        Sleep((int)(NtcssNTDelay * 1000));

		user_sid_size = GetSidLengthRequired( SID_MAX_SUB_AUTHORITIES );
		
        //user_sid_size=28;
        ref_domain_size=SIZE_CMD_LINE;    
		//psnu_user = SidTypeUser;            

//      loop to wait for the SID to be mapped to the users name.

		if (domain_flag == 1)  {               
           net_ret=SyncPDC(&dn_server[2]);
		}

		SID_NAME_USE  tmp_snuser;

        sid_cnt=0;
        while (sid_cnt < 50 ) {
			if (domain_flag == 1)                 
				LookupAccountName((LPCTSTR)dn_server,
                            loginname,
                            &user_sid,
                            &user_sid_size,
                            ref_domain,
                            &ref_domain_size,
                            &tmp_snuser); 
			else
				LookupAccountName((LPCTSTR)NULL,
                            loginname,
                            &user_sid,
                            &user_sid_size,
                            ref_domain,
                            &ref_domain_size,
                            &tmp_snuser); 


		   // Valid sid, break
		   if (IsValidSid(&user_sid)) {		
			   sid_cnt=49;
			   break;
		   }
           sid_cnt=sid_cnt+1;
           Sleep(300);                            // WAG delay
		}	//    end loop;

		if (domain_flag == 1)  {               
           net_ret=SyncPDC(&dn_server[2]);
		}

		if (sid_cnt < 50) {
			net_ret=SetFileAccess(homedir,"REVOKE","everyone");
			net_ret=SetFileAccess(homedir,"GRANT",loginname);
			net_ret=SetFileAccess(homedir,"GRANT","Administrators");

			if (domain_flag == 1) 
				net_ret=SetFileAccess(homedir,"GRANT","Domain Admins");

		}

	} else {
	//	sprintf(strError,"Error Adding User - %d",GetLastError());
	//	MessageBox(NULL,strError,"NTCSS Error", MB_OK);
		DisplayMessage("Error Adding User");
		return(1);

	}

	if (NtcssSavePas(homedir,pas)) {
		DisplayMessage("Creating PAS File");
		return(1);
	}

	if (pusr != NULL)
		NetApiBufferFree(pusr);
	if (pusr1 != NULL)
		NetApiBufferFree(pusr1);
	if (pdn_wserver != NULL)
		NetApiBufferFree(pdn_wserver);

    return(0);  

}



int SyncPDC (char *dc_name)
{
STARTUPINFO SI;
PROCESS_INFORMATION PI;  
BOOL ISALB;   
char cmd[_MAX_DIR];  
DWORD msgret;
                                                  

      SI.cb=68;  
      SI.wShowWindow = SW_HIDE;  
      SI.lpReserved = NULL;
      SI.lpReserved2 = NULL;      
      SI.cbReserved2 = 0;
      SI.lpDesktop = NULL;
      SI.lpTitle = NULL;
      SI.dwFlags = STARTF_USESHOWWINDOW;

      memset(cmd,NULL,_MAX_DIR);

	  // Assume the bin dir is in the path
  //    strcat(cmd,NtcssBinDir);   
      strcat(cmd,"nltest /server:");
      strcat(cmd,dc_name);
      strcat(cmd," /pdc_repl");


      ISALB=CreateProcess(NULL,cmd,
                 NULL,NULL,0,CREATE_NEW_CONSOLE + NORMAL_PRIORITY_CLASS,
                 NULL,NULL,&SI,&PI);
      if (ISALB == 0)  {
          return (1);
      }

      msgret=WaitForSingleObject((HANDLE)&PI.hProcess,15000);

      return(1);
}


BOOL NtcssDupGlobalNTGroups(TCHAR *loginname,
							TCHAR *temp_user,BOOL domain_flag,TCHAR *server) 
{
  DWORD net_ret;
  LPGROUP_USERS_INFO_0 pgg;
  LPWSTR dn_wserver;  
  char groupname[64];
  int i;
  DWORD dwFlags = LG_INCLUDE_INDIRECT ;
  DWORD dwLevel = 0;
  DWORD dwPrefMaxLen = -1;
  DWORD read_entries = 0;
  DWORD total_entries = 0;

                                                                
  if (domain_flag == 0) 
      dn_wserver=0;
  else {
      dn_wserver=(LPWSTR)server;
  }
  
  net_ret = NetUserGetGroups( (LPWSTR)dn_wserver, 
			(LPWSTR)temp_user, dwLevel, (LPBYTE *) &pgg, dwPrefMaxLen, &read_entries, 
					&total_entries );

  if (net_ret == 0) {
	    LPGROUP_USERS_INFO_0 tpgg=pgg;

        for (i=0;i < (int)read_entries;i++) {
          net_ret=WideCharToMultiByte(0,0,(LPCWSTR)tpgg->grui0_name,-1,
                          (LPSTR)groupname,64,NULL,NULL);    

          net_ret=NetGroupAddUser((LPWSTR)(dn_wserver),(LPWSTR)tpgg->grui0_name,
								(LPWSTR)loginname);
		  tpgg++;
		}
		if (pgg != NULL)
	       net_ret=NetApiBufferFree((LPVOID)pgg); 
  }       

   return 0;
}  

BOOL NtcssDupLocalNTGroups(TCHAR *loginname,
							TCHAR *temp_user,BOOL domain_flag,TCHAR *server) 
{
  DWORD net_ret;
  LPLOCALGROUP_USERS_INFO_0 plg = NULL;  
  LOCALGROUP_MEMBERS_INFO_3 lgm_struct;
  LPWSTR dn_wserver;  
  char groupname[64];
  int i;
  DWORD dwFlags = LG_INCLUDE_INDIRECT ;
  DWORD dwLevel = 0;
  DWORD dwPrefMaxLen = -1;
  DWORD read_entries = 0;
  DWORD total_entries = 0;
 
                                                                      
  if (domain_flag == 0) 
      dn_wserver=0;
  else {
      dn_wserver=(LPWSTR)server;
  }

  net_ret = NetUserGetLocalGroups((LPWSTR)dn_wserver,(LPWSTR)temp_user, 
				dwLevel, dwFlags,(LPBYTE *) &plg, dwPrefMaxLen, 
				&read_entries, &total_entries );
 
  if (net_ret == 0) {             
		LPLOCALGROUP_USERS_INFO_0 tmpplg = plg;  

        for (i=0;i < (int)read_entries;i++) {
          net_ret=WideCharToMultiByte(0,0,(LPCWSTR)tmpplg->lgrui0_name,-1,
                          (LPSTR)groupname,64,NULL,NULL);    
          lgm_struct.lgrmi3_domainandname=(LPWSTR)loginname;

          net_ret=NetLocalGroupAddMembers((LPWSTR)dn_wserver,(LPWSTR)tmpplg->lgrui0_name,
                    3,(LPBYTE)&lgm_struct,1);
  
		  tmpplg++;
		}
		if (plg != NULL)
			net_ret=NetApiBufferFree((LPVOID)plg); 
  }       

   return 0;
}  



extern "C" BOOL __declspec(dllexport) WINAPI NtcssChangeNTName(char *loginname,
					char *realname,BOOL domain_flag)
{
DWORD  net_ret,net_err;
DWORD pwdResult;
LPWSTR pdn_wserver;
USER_INFO_1011 usr0;
char dn[SIZE_CMD_LINE];
TCHAR w_dn[SIZE_CMD_LINE];
int dnlen; 
TCHAR wUserName[32];
TCHAR wRealName[SIZE_REAL_NAME];  
  

	pdn_wserver = NULL;

    memset(wUserName,NULL,SIZE_LOGIN_NAME+1);
    net_ret=MultiByteToWideChar(0,0,loginname,SIZE_LOGIN_NAME,
					(LPWSTR)wUserName,32);
    memset(wRealName,NULL,SIZE_REAL_NAME);
    net_ret=MultiByteToWideChar(0,0,realname,SIZE_REAL_NAME,
					(LPWSTR)wRealName,SIZE_REAL_NAME);


    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CLIENT_LOCATION);

    pdn_wserver = NULL;
    if (domain_flag == 1) {

        net_ret=MultiByteToWideChar(0,0,dn,
                 -1,(LPWSTR)w_dn,SIZE_CLIENT_LOCATION);   

		net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);

    }                                               


     usr0.usri1011_full_name=(LPWSTR)wRealName;
     pwdResult=NetUserSetInfo((LPWSTR)pdn_wserver,(LPWSTR)wUserName,1011,
                           (unsigned char *)&usr0, &net_err); 

	if (domain_flag == 1) {
		if (pdn_wserver != NULL)
			net_ret=NetApiBufferFree((LPVOID)pdn_wserver); 
	}

    return pwdResult;

}; 


extern "C" BOOL __declspec(dllexport) WINAPI NtcssDeleteNTUser(char *loginname,
														  int domain_flag, BOOL bRmDir)
{
DWORD  delResult,net_ret;  
TCHAR wUserName[32];  
char dn[SIZE_CLIENT_LOCATION];
TCHAR w_dn[SIZE_CLIENT_LOCATION];
char homedir[SIZE_CLIENT_LOCATION];
int dnlen; 
LPWSTR pdn_wserver;
USER_INFO_3 *pusr3;


    memset(wUserName,NULL,SIZE_LOGIN_NAME+1);
    net_ret=MultiByteToWideChar(0,0,loginname,SIZE_LOGIN_NAME,
					(LPWSTR)wUserName,32);

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CLIENT_LOCATION);

    pdn_wserver = NULL;
    if (domain_flag == 1) {


        net_ret=MultiByteToWideChar(0,0,dn,
                 -1,(LPWSTR)w_dn,SIZE_CLIENT_LOCATION);   

		net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);

    }                                               

	pusr3=NULL;
	net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)wUserName,
                             3,(LPBYTE *)&pusr3); 

	if (net_ret) {
			MessageBox(NULL,"Unable to identify NT users.","NTCSS Error",MB_OK);
	} else {
			memset(homedir,NULL,SIZE_CLIENT_LOCATION);

			net_ret=WideCharToMultiByte(0,0,(LPWSTR)pusr3->usri3_home_dir,-1,
						homedir,SIZE_CLIENT_LOCATION,NULL,NULL);
	
			if (bRmDir && strlen(homedir)) {
				net_ret=RemoveDirectory(homedir);
				if (!net_ret) {
					MessageBox(NULL,"Unable to delete users home directory.","NTCSS Error",MB_OK);
				}
			}

			if (pusr3 != NULL)
				net_ret=NetApiBufferFree(pusr3); 

			delResult=NetUserDel(pdn_wserver,(LPWSTR)wUserName);

			if (pdn_wserver != NULL && domain_flag == 1) {
				net_ret=NetApiBufferFree(pdn_wserver); 
			}
	}
    return 0;

}    


extern "C" BOOL __declspec(dllexport) WINAPI  NtcssChangeNTPassword(char *loginname,
					char *oldPassword,char *newPassword,
					int adm_flag,
                    int domain_flag) 
{
TCHAR wUserName[32];
TCHAR wOldPwd[64];  
TCHAR wNewPwd1[64];  
DWORD net_ret,net_err;
DWORD pwdResult;
LPWSTR pdn_wserver;
USER_INFO_1003 usr1003;
char dn[SIZE_CMD_LINE];
TCHAR w_dn[SIZE_CMD_LINE];
int dnlen; 
char dn_server[64];
TCHAR dn_wserver[64];  
         

    memset(wUserName,NULL,32);
    memset(wNewPwd1,NULL,64);
    memset(wOldPwd,NULL,64);
    memset(dn,NULL,SIZE_CMD_LINE);
    memset(dn_wserver,NULL,64);

    pdn_wserver = NULL;

    if (domain_flag == 1) {
	    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CLIENT_LOCATION);

        net_ret=MultiByteToWideChar(0,0,dn,
                 -1,(LPWSTR)w_dn,SIZE_CLIENT_LOCATION);   

		net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);

		net_ret=WideCharToMultiByte(0,0,(LPWSTR)pdn_wserver,-1,
						dn_server,64,NULL,NULL);

    }                                               

    MultiByteToWideChar(0,0,loginname,-1,(LPWSTR)wUserName,32);
    MultiByteToWideChar(0,0,newPassword,-1,(LPWSTR)wNewPwd1,64);

    if (adm_flag == 0) {
        MultiByteToWideChar(0,0,oldPassword,-1,(LPWSTR)wOldPwd,64);  

        pwdResult=NetUserChangePassword((LPWSTR)NULL,(LPWSTR)wUserName,
                              (LPWSTR)wOldPwd,(LPWSTR)wNewPwd1); 

    } else {

	    MultiByteToWideChar(0,0,newPassword,-1,(LPWSTR)wNewPwd1,64);

         usr1003.usri1003_password=(LPWSTR)wNewPwd1;

         pwdResult=NetUserSetInfo((LPWSTR)pdn_wserver,(LPWSTR)wUserName,1003,
                           (LPBYTE)&usr1003, &net_err); 

    }


    if (domain_flag == 1 && pdn_wserver != NULL) {
        net_ret=NetApiBufferFree(pdn_wserver); 
    }

	if (pwdResult)
		return(1);
	else
		return 0;

}    


extern "C" BOOL __declspec(dllexport) WINAPI NtcssMakeNTAdmin(char *loginname,
														 char *temp_user, 
														 BOOL domain_flag)
{
  TCHAR w_name[32];   
  DWORD net_ret;    
  char dn[SIZE_CMD_LINE];
  TCHAR w_dn[SIZE_CMD_LINE];
  int dnlen;                      
  LPWSTR pdn_wserver;   
  TCHAR w_groupname[64];

    memset(w_name,NULL,32);
    net_ret=MultiByteToWideChar(0,0,loginname,SIZE_LOGIN_NAME,
					(LPWSTR)w_name,32);

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CLIENT_LOCATION);


    pdn_wserver = NULL;
    if (domain_flag == 1) {
        net_ret=MultiByteToWideChar(0,0,dn,
                 -1,(LPWSTR)w_dn,SIZE_CLIENT_LOCATION);   

		net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);

		net_ret=MultiByteToWideChar(0,0,"Domain Admins",
                       -1,(LPWSTR)w_groupname,64); 

    } else {
		net_ret=MultiByteToWideChar(0,0,"Administrators",
                       -1,(LPWSTR)w_groupname,64); 

	}

    net_ret=NetGroupAddUser((LPWSTR)pdn_wserver,(LPWSTR)w_groupname,
							(LPWSTR)w_name);

	if (pdn_wserver != NULL)
		NetApiBufferFree(pdn_wserver);


    return(net_ret);  

}


extern "C" BOOL __declspec(dllexport) WINAPI NtcssMakeAdminNormal(char *loginname,
						  char *temp_user,BOOL domain_flag) 
{
  TCHAR w_name[32];   
  DWORD net_ret;
  LPWSTR pdn_wserver;  
  TCHAR w_groupname[64];
  char dn[SIZE_CMD_LINE];
  TCHAR w_dn[SIZE_CMD_LINE];
  int dnlen;                      
  
    memset(w_name,NULL,32);
    net_ret=MultiByteToWideChar(0,0,loginname,SIZE_LOGIN_NAME,
					(LPWSTR)w_name,32);

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CLIENT_LOCATION);


    pdn_wserver = NULL;
    if (domain_flag == 1) {
        net_ret=MultiByteToWideChar(0,0,dn,
                 -1,(LPWSTR)w_dn,SIZE_CLIENT_LOCATION);   

		net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);

		net_ret=MultiByteToWideChar(0,0,"Domain Admins",
                       -1,(LPWSTR)w_groupname,64); 

    } else {
		net_ret=MultiByteToWideChar(0,0,"Administrators",
                       -1,(LPWSTR)w_groupname,64); 

	}

    net_ret=NetGroupDelUser((LPWSTR)pdn_wserver,(LPWSTR)w_groupname,
							(LPWSTR)w_name);

   return net_ret;
}  




const int initialBufferSize = 2048;

SECURITY_DESCRIPTOR* NtcssGetFileSecurity( const TCHAR *filename, SECURITY_INFORMATION whichParts)
{
	SECURITY_DESCRIPTOR *psd;
	DWORD needed, rc;
	bool haveRetriedPrivilege = false, haveRetriedSize = false;
	bool oldSecPrivState = false;

	needed = initialBufferSize;
	psd = (SECURITY_DESCRIPTOR *) new byte[initialBufferSize];

	for ( ; ; )
	{
		if ( ::GetFileSecurity( filename, whichParts, psd, needed, &needed ) )
			rc = 0;
		else
			rc = GetLastError();

		if ( rc == ERROR_INSUFFICIENT_BUFFER && ! haveRetriedSize )
		{
			delete [] (byte *) psd;
			psd = (SECURITY_DESCRIPTOR *) new byte[needed];
			haveRetriedSize = true;
		}
		else if ( rc == ERROR_PRIVILEGE_NOT_HELD && ! haveRetriedPrivilege )
		{
			haveRetriedPrivilege = true;
		}
		else
			break; // unrecoverable or all OK
	}

	return psd;
}

extern "C" BOOL __declspec(dllexport) WINAPI NtcssGetUsersDC(LPCTSTR loginname,
 								 BOOL domain_flag, 
								 LPTSTR UserDirDomain,LPTSTR ProfileDirDomain)
{
  USER_INFO_3 *pusr;
  char dn_server[64];
  LPWSTR pdn_wserver;   
  TCHAR w_temp_user[32];
  char dn[SIZE_CMD_LINE];
  char homedir[SIZE_CMD_LINE+1]; 
  char profiledir[SIZE_CMD_LINE+1]; 
  TCHAR w_dn[SIZE_CMD_LINE];
  int dnlen,net_ret;  

    pdn_wserver=NULL;
    memset(dn_server,NULL,64);

	UserDirDomain[0]='\0';
	ProfileDirDomain[0]='\0';

    memset(dn,NULL,SIZE_CMD_LINE);   

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CMD_LINE);
    if (dnlen <= 0) {
		MessageBox(NULL,"Cannot get user domain for user","NTCSS Error",MB_OK);
        return (1);
    }       

    if (domain_flag == 1) {

        memset(w_dn,0,SIZE_CMD_LINE);

        net_ret=MultiByteToWideChar(0,0,dn,
                 SIZE_CMD_LINE,(LPWSTR)w_dn,SIZE_CMD_LINE); 

        net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);    
        net_ret=WideCharToMultiByte(0,0,(LPWSTR)pdn_wserver,-1,
                        dn_server,64,NULL,NULL);  

	} else {
		strcpy(dn_server,dn);
	}


	pusr=NULL;
    net_ret=MultiByteToWideChar(0,0,loginname,
                       -1,(LPWSTR)w_temp_user,32); 

    net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_temp_user,
                             3,(LPBYTE *)&pusr); 
	
    if (net_ret != 0) {
//			MessageBox(NULL,"Cannot get user info from NT","NTCSS Error",MB_OK);
			if (pusr != NULL)
				NetApiBufferFree(pusr);
			if (pdn_wserver != NULL)
				NetApiBufferFree(pdn_wserver);

		   return(1);
	}

    memset(homedir,'\0',SIZE_CMD_LINE+1);
    net_ret=WideCharToMultiByte(0,0,(LPWSTR)pusr->usri3_home_dir,-1,
                        homedir,SIZE_CMD_LINE,NULL,NULL);  

	char drive[255],dname[255],fname[255],ext[10];
	memset(drive,NULL,255);
	memset(dname,NULL,255);
	memset(fname,NULL,255);
	memset(ext,NULL,10);

	_splitpath(homedir,drive,dname,fname,ext);

	if (strlen(drive) == 0) {   // must be unc
		if (strlen(dname) == 0) {
			if (pusr != NULL)
				NetApiBufferFree(pusr);
			if (pdn_wserver != NULL)
				NetApiBufferFree(pdn_wserver);
			return(1);
		}
		char *ptr=strchr(dname+2,'\\');
		if (ptr)
			*ptr=NULL;
		strncpy(UserDirDomain,dname,strlen(dname));
	} 

	memset(drive,NULL,255);
	memset(dname,NULL,255);
	memset(fname,NULL,255);
	memset(ext,NULL,10);

	_stprintf(profiledir,"%ls",pusr->usri3_profile);

	_splitpath(profiledir,drive,dname,fname,ext);

	if (strlen(drive) == 0) {   // must be unc
		if (strlen(dname) == 0) {
			if (pusr != NULL)
				NetApiBufferFree(pusr);
			if (pdn_wserver != NULL)
				NetApiBufferFree(pdn_wserver);
			return(1);
		}
		char *ptr=strchr(dname+2,'\\');
		if (ptr)
			*ptr=NULL;
		strncpy(ProfileDirDomain,dname,strlen(dname));
	} 

	if (pusr != NULL)
		NetApiBufferFree(pusr);
	if (pdn_wserver != NULL)
		NetApiBufferFree(pdn_wserver);

    return(0);

}


extern "C" BOOL __declspec(dllexport) WINAPI NtcssSetUsersDC(char *loginname,char *homepath, 
													char *profilepath,BOOL domain_flag)
{
  USER_INFO_3 *pusr,usr;
  char dn_server[64];
  LPWSTR pdn_wserver;   
  TCHAR w_temp_user[32];
  TCHAR w_homedir[SIZE_CMD_LINE+1];
  TCHAR w_profilepath[SIZE_CMD_LINE+1];
  char dn[SIZE_CMD_LINE];
  char homedir[SIZE_CMD_LINE+1]; 
  TCHAR w_dn[SIZE_CMD_LINE];
  int dnlen,net_ret;  
  DWORD dwSetError;

    pdn_wserver=NULL;
    memset(dn_server,NULL,64);

    memset(dn,NULL,SIZE_CMD_LINE);   

    dnlen=GetEnvironmentVariable("USERDOMAIN",dn,SIZE_CMD_LINE);
    if (dnlen <= 0) {
		MessageBox(NULL,"Cannot get user domain for user","NTCSS Error",MB_OK);
        return (1);
    }       

    if (domain_flag == 1) {

        memset(w_dn,0,SIZE_CMD_LINE);

        net_ret=MultiByteToWideChar(0,0,dn,
                 SIZE_CMD_LINE,(LPWSTR)w_dn,SIZE_CMD_LINE); 

        net_ret=NetGetDCName(NULL, (LPWSTR)w_dn,(LPBYTE *)&pdn_wserver);    
        net_ret=WideCharToMultiByte(0,0,(LPWSTR)pdn_wserver,-1,
                        dn_server,64,NULL,NULL);  

	} else {
		strcpy(dn_server,dn);
	}


	pusr=NULL;
    net_ret=MultiByteToWideChar(0,0,loginname,
                       -1,(LPWSTR)w_temp_user,32); 

    net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_temp_user,
                             3,(LPBYTE *)&pusr); 
	
    if (net_ret != 0) {
//		MessageBox(NULL,"Cannot get user info from NT","NTCSS Error",MB_OK);
		if (pusr != NULL)
			NetApiBufferFree(pusr);
		if (pdn_wserver != NULL)
			NetApiBufferFree(pdn_wserver);
		return(1);
	}


    net_ret=MultiByteToWideChar(0,0,homepath,
                       -1,(LPWSTR)w_homedir,SIZE_CMD_LINE); 
    net_ret=MultiByteToWideChar(0,0,profilepath,
                       -1,(LPWSTR)w_profilepath,SIZE_CMD_LINE); 

	usr.usri3_comment = pusr->usri3_comment;
    usr.usri3_priv=pusr->usri3_priv;  
	usr.usri3_flags=pusr->usri3_flags;
	usr.usri3_script_path=pusr->usri3_script_path;
	usr.usri3_auth_flags=pusr->usri3_auth_flags; 
	usr.usri3_usr_comment=pusr->usri3_usr_comment;
	usr.usri3_parms=pusr->usri3_parms;
	usr.usri3_workstations=pusr->usri3_workstations; 
	usr.usri3_last_logon=pusr->usri3_last_logon; 
	usr.usri3_last_logoff=pusr->usri3_last_logoff; 
	usr.usri3_acct_expires=pusr->usri3_acct_expires;   
	usr.usri3_max_storage=pusr->usri3_max_storage;   
	usr.usri3_units_per_week=pusr->usri3_units_per_week;   
	usr.usri3_logon_hours=pusr->usri3_logon_hours;   
	usr.usri3_bad_pw_count=pusr->usri3_bad_pw_count;   
	usr.usri3_num_logons=pusr->usri3_num_logons;   
	usr.usri3_logon_server=pusr->usri3_logon_server;   
	usr.usri3_country_code=pusr->usri3_country_code;   
	usr.usri3_code_page=pusr->usri3_code_page; 
	usr.usri3_user_id=pusr->usri3_user_id;   
	usr.usri3_primary_group_id=pusr->usri3_primary_group_id;  
    usr.usri3_home_dir_drive=pusr->usri3_home_dir_drive;  
	usr.usri3_password_expired=pusr->usri3_password_expired;
    usr.usri3_name=pusr->usri3_name; 
    usr.usri3_password=pusr->usri3_password;     
    usr.usri3_full_name=pusr->usri3_full_name;  
    usr.usri3_home_dir=(LPWSTR)w_homedir;   
	usr.usri3_profile=(LPWSTR)w_profilepath;


	net_ret=CreateDirectoryAndWait(homepath);

	if (net_ret == 1) {  
		MessageBox(NULL,"Unable to create users home directory, aborting",
						"NTCSS Error",MB_OK);

		return(1);
	}


	net_ret = NetUserSetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_temp_user,
					3,(LPBYTE)&usr,&dwSetError);


    net_ret = NetUserGetInfo((LPWSTR)pdn_wserver,(LPWSTR)w_temp_user,
                             3,(LPBYTE *)&pusr); 

    memset(homedir,'\0',SIZE_CMD_LINE+1);
    net_ret=WideCharToMultiByte(0,0,(LPWSTR)pusr->usri3_home_dir,-1,
                        homedir,SIZE_CMD_LINE,NULL,NULL);  

	if (pusr != NULL)
		NetApiBufferFree(pusr);
	if (pdn_wserver != NULL)
		NetApiBufferFree(pdn_wserver);

	net_ret=SetFileAccess(homedir,"REVOKE","everyone");
	net_ret=SetFileAccess(homedir,"GRANT","Administrators");
	net_ret=SetFileAccess(homedir,"GRANT",loginname);

    return(0);
}


//
// Create a directory if it doesn't exist and wait for it to be visible
//

BOOL CreateDirectoryAndWait(const char *dir)
{
  int create_dir_cnt;
  HANDLE fp;          
  WIN32_FIND_DATA fd;     
  BOOL atontB;

    create_dir_cnt=0;

	// create 
	atontB=CreateDirectory(dir,NULL) ;

	// if create dir says it exists, just check it with FindFirstFile
	if (atontB == 0) {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			fp=FindFirstFile(dir,&fd);
			if (fp != 0) {
		        atontB=FindClose(fp);
			} else {
				DisplayError(GetLastError());
				return(1);
			}
		} else {
			DisplayError(GetLastError());
			return(1);
		}
	} else {

	    while (create_dir_cnt > 60) { 
		  Sleep(250);
		fp=FindFirstFile(dir,&fd);
		if (fp != 0) {
	        atontB=FindClose(fp);
			break;
		}
		create_dir_cnt++;
		}  

		if (create_dir_cnt > 60) {  
			DisplayError(GetLastError());
			return(1);
		}
	}

	return(0);

}


BOOL DisplayError(int nError)
{
LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				  FORMAT_MESSAGE_FROM_SYSTEM | 
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  nError,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				  (LPTSTR) &lpMsgBuf,
				  0,
				  NULL);

	MessageBox(NULL,(const char *)lpMsgBuf,"NTCSS Error",MB_OK);
	LocalFree( lpMsgBuf );

	return(0);
}

BOOL DisplayMessage(char *msg)
{
char strError[100];
LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				  FORMAT_MESSAGE_FROM_SYSTEM | 
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  GetLastError(),
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				  (LPTSTR) &lpMsgBuf,
				  0,
				  NULL);

	sprintf(strError,"%s:%s",msg,lpMsgBuf);
	MessageBox(NULL,(const char *)strError,"NTCSS Error",MB_OK);
	LocalFree( lpMsgBuf );

	return(0);
}

