/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// DllUser.cpp                   
//                                         
// This is the implementation file for CdllUser.
// 
//----------------------------------------------------------------------


#include "stdafx.h"
#include "0_basics.h"
#include "dll_msgs.h"

#include "err_man.h"


/////////////////////////////////////////////////////////////////////////////
// CdllUser constructor
// ________________________

CdllUser::CdllUser()
{
    m_pShmemDllUserData = NULL;
	m_strMasterServer=_T("");
}


/////////////////////////////////////////////////////////////////////////////
// IsAuthServer
// ________
//
// Checks to see if given lpszGroupTitle is on an Authentication Server or Master Server


BOOL CdllUser::IsAuthServer(LPCTSTR lpszGroupTitle) //10/22/98 for new LRS stuff
{
	 const SappGroupRec_wFullData *app_group_ptr;
	 if((app_group_ptr=appGroup(lpszGroupTitle))==NULL)
		 return FALSE;
	 else                      //2=Auth Server                  0=Master Server
	 
		 return app_group_ptr->server_type==2||app_group_ptr->server_type==0;
}



/////////////////////////////////////////////////////////////////////////////
// GetAuthServer
// ________
//
// Gets Apps Authentication Server or Master Server if it's not on 1
// SO its really get dataserver

CString CdllUser::GetAuthServer(LPCTSTR lpszGroupTitle) //10/22/98 for new LRS stuff
{
	 const SappGroupRec_wFullData *app_group_ptr;
	 if((app_group_ptr=appGroup(lpszGroupTitle))==NULL)
		 return GetMasterName();
	 //2=Auth Server    0=Master Server
		 if(app_group_ptr->server_type==2)
			 return app_group_ptr->host_name_buf;
		 else
			return GetMasterName();
		
}

		
/////////////////////////////////////////////////////////////////////////////
// GetAppServer
// ________
//
// Gets Apps Server 

CString CdllUser::GetAppServer(LPCTSTR lpszGroupTitle)
{
	 const SappGroupRec_wFullData *app_group_ptr;
	 if((app_group_ptr=appGroup(lpszGroupTitle))==NULL)
		 return _T("");
	 else
		return app_group_ptr->host_name_buf;
		 
}

/////////////////////////////////////////////////////////////////////////////
// appGroup
// ________
//
// Checks to see if given lpszGroupTitle is in the 
// m_pShmemDllUserData data.

const SappGroupRec_wFullData *CdllUser::appGroup 
									(const char *lpszGroupTitle)
{
    if (!isValid())
        return(NULL);
                                       // The first group starts
                                       //  just beyond the
                                       //  SshmemLoginReplyStruct 
                                       //  at the start of the
                                       //  m_pShmemDllUserData data
                                       //
    SappGroupRec_wFullData *app_group_ptr =
                            (SappGroupRec_wFullData *)
                            ( (char *)m_pShmemDllUserData
                               + sizeof(SshmemLoginReplyStruct) );



	

    for (unsigned int i=0; 
         i < m_pShmemDllUserData->app_group_cnt; i++)
    {
	    

        if (strncmp(lpszGroupTitle,app_group_ptr->progroup_title_buf,
                    SIZE_GROUP_TITLE) == 0)
        {
			
            return(app_group_ptr);
        }
                                        // To get to next group have
                                        //  to jump over current group
                                        //  and all of its programs
                                        //
        app_group_ptr = (SappGroupRec_wFullData *)
                          ( (char *)app_group_ptr +
                            sizeof(SappGroupRec_wFullData) +
                            (app_group_ptr->program_cnt *
                             sizeof(SappProgramRec_wFullData)) );

	
    }

									   // Didn't find, return NULL
    return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// appGroupsCount
// _____________

int CdllUser::appGroupsCount () const
{
    if (!isValid())
        return(0);

    return (m_pShmemDllUserData->app_group_cnt);
}



/////////////////////////////////////////////////////////////////////////////
// appProgram
// __________
//
// Finds SappProgramRec_wFullData for given lpszGroupTitle/lpszProgramTitle 
// and returns pointer to the record.

const SappProgramRec_wFullData *CdllUser::appProgram 
                              (const char *lpszGroupTitle,
                               const char *lpszProgramTitle) const
{
    if (!isValid())
        return(NULL);
                                       // The first group starts
                                       //  just beyond the
                                       //  SshmemLoginReplyStruct 
                                       //  at the start of the
                                       //  m_pShmemDllUserData data
                                       //
    SappGroupRec_wFullData *app_group_ptr =
                            (SappGroupRec_wFullData *)
                            ( (char *)m_pShmemDllUserData
                               + sizeof(SshmemLoginReplyStruct) );

    SappProgramRec_wFullData  *app_prog_ptr;

    unsigned int i,j;
    BOOL group_does_match;

    for (i=0; i < m_pShmemDllUserData->app_group_cnt; i++)
    {
        group_does_match = (strncmp(lpszGroupTitle,
                                    app_group_ptr->progroup_title_buf,
                                    SIZE_GROUP_TITLE) == 0);

                                       // The first program starts just
                                       //  after the current 
                                       //  SappGroupRec_wFullData 
                                       //
        app_prog_ptr = (SappProgramRec_wFullData *)
                        ( (char *)app_group_ptr +
                          sizeof(SappGroupRec_wFullData) );

        if (group_does_match)
        {
            for (j=0; j < app_group_ptr->program_cnt; j++)
            {
                if ( (strncmp(lpszProgramTitle,
                              app_prog_ptr->program_title_buf,
                              SIZE_PROGRAM_TITLE) == 0) )

                return(app_prog_ptr);
            }
                                       // Can just use pointer arith.
                                       //  to bump to next program
            app_prog_ptr++;
        }
                                       // When inner loop exits the
                                       //  app_prog_ptr is really
                                       //  pointed to the next
                                       //  SappGroupRec_wFullData
                                       //  unless at end
                                       //
        app_group_ptr = (SappGroupRec_wFullData *)app_prog_ptr;
    }

    return(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// copyLrsFromDtToDllShmem 
// _______________________
//
// Takes LRS data from desktop and copies it to buffer pointed to by
// pDllLrsData.

BOOL CdllUser::copyLrsFromDtToDllShmem
                    (const SshmemLoginReplyStruct *pDesktopLrsData,
                     SshmemLoginReplyStruct *pDllLrsData)
{

// TONY Uncommented existing debug statements

/*AfxMessageBox("Now in copyLrsFromDtToDllShmem");

CString tmpstr;

tmpstr.Format("tmpstr: %s\n", pDesktopLrsData->user_rec.token_buf);
AfxMessageBox(tmpstr);
//
tmpstr.Format("unix_id: %d\n", pDesktopLrsData->user_rec.unix_id);
AfxMessageBox(tmpstr);

tmpstr.Format("login_name_buf: %s\n", pDesktopLrsData->user_rec.login_name_buf);
AfxMessageBox(tmpstr);

tmpstr.Format("real_name_buf: %s\n", pDesktopLrsData->user_rec.real_name_buf);
AfxMessageBox(tmpstr);

tmpstr.Format("ssn_buf: %s\n", pDesktopLrsData->user_rec.ssn_buf);
AfxMessageBox(tmpstr);

tmpstr.Format("phone_number_buf: %s\n", pDesktopLrsData->user_rec.phone_number_buf);
AfxMessageBox(tmpstr);

tmpstr.Format("security_class: %d\n", pDesktopLrsData->user_rec.security_class);
AfxMessageBox(tmpstr);

tmpstr.Format("password_change_time: %s\n", pDesktopLrsData->user_rec.password_change_time);
AfxMessageBox(tmpstr);

tmpstr.Format("shared_passwd_buf: %s\n", pDesktopLrsData->user_rec.shared_passwd_buf);
AfxMessageBox(tmpstr);

tmpstr.Format("user_role: %d\n", pDesktopLrsData->user_rec.user_role);
AfxMessageBox(tmpstr);

AfxMessageBox("Done with show now");*/

//	WalkLRS( (SshmemLoginReplyStruct*)pDesktopLrsData);

//	XshowShmemLrsData(pDesktopLrsData);

                                       // First set validity check value

                                       
    pDllLrsData->validity_check_value = NTCSSCODE_VALITY_CHECK_VALUE;

    pDllLrsData->app_group_cnt = 
                    pDesktopLrsData->app_group_cnt;

                                       // Since xxx_app_group_list is
                                       //  no use to us then NULLify
                                       //
    pDllLrsData->xxx_app_group_list = NULL;

    const SuserInfoRec_wFullData      *desktop_user_ptr; 
    SuserInfoRec_wFullData      *dll_user_ptr;

    SappGroupRec_wFullData      *desktop_group_ptr;
    SappGroupRec_wFullData      *dll_group_ptr;

    SappProgramRec_wFullData    *desktop_prog_ptr;
    SappProgramRec_wFullData    *dll_prog_ptr;

    unsigned int i,j;

                                       // For SuserInfoRec_wFullData
                                       //   just copy data over
                                       //
//AfxMessageBox("Before memcpying SuserInfoRec_wFullData");

    desktop_user_ptr =(&pDesktopLrsData->user_rec);

    dll_user_ptr = (&pDllLrsData->user_rec);

	


    memcpy(dll_user_ptr,desktop_user_ptr,
           sizeof(SuserInfoRec_wFullData));

                                       // The first group will start
                                       //  just beyond the
                                       //  SshmemLoginReplyStruct 
                                       //  at the start of the
                                       //  pDllLrsData data
                                       //
    dll_group_ptr =(SappGroupRec_wFullData *)
                    ( (char *)pDllLrsData
                       + sizeof(SshmemLoginReplyStruct) );

                                       // Get desktop_group_ptr set to
                                       //  the first group 
                                       //
    desktop_group_ptr = pDesktopLrsData->xxx_app_group_list;
     
                                       // Now for each group copy data
                                       //  from the desktop to the dll
                                       //  shared memory area (outer
                                       //  loop)
                                       //  

    for (i=0; i < pDesktopLrsData->app_group_cnt; i++)
    {
/*str.Format("Outer group loop - before doing memcpy #%d//%d of SappGroupRec_wFullData",
i,pDesktopLrsData->app_group_cnt);
AfxMessageBox(str);*/

/*str.Format("Group name is: %s and addr is %d",
desktop_group_ptr->progroup_title_buf, desktop_group_ptr);
AfxMessageBox(str);*/

	


        memcpy(dll_group_ptr, desktop_group_ptr,
               sizeof(SappGroupRec_wFullData));

                                       // These pointers are useless
                                       //  to dll, so make NULL
                                       //
        dll_group_ptr->xxx_program_list = NULL;
        dll_group_ptr->xxx_next_group_ptr = NULL;

                                       // The first program starts just
                                       //  after the current 
                                       //  SappGroupRec_wFullData 
                                       //
        dll_prog_ptr = (SappProgramRec_wFullData *)
                        ( (char *)dll_group_ptr +
                          sizeof(SappGroupRec_wFullData) );

                                       // Get desktop_group_ptr set to
                                       //  the first program
                                       //
        desktop_prog_ptr = desktop_group_ptr->xxx_program_list;

                                       // Now loop for each program
                                       //  and copy program data over
                                       //  (inner loop)
                                       //
        for (j=0; j < desktop_group_ptr->program_cnt; j++)
        {
/*str.Format("Inner program loop, before doing memcpy #%d//%d of SappProgramRec_wFullData",
i,desktop_group_ptr->program_cnt);
AfxMessageBox(str);*/

/*str.Format("Prog name is: %s and addr is %d",
desktop_prog_ptr->program_title_buf, desktop_prog_ptr);
AfxMessageBox(str);*/
           


	
			memcpy(dll_prog_ptr, desktop_prog_ptr,sizeof(SappProgramRec_wFullData));
	
		
                                       // This pointer useless
                                       //  to dll, so make NULL
                                       //
            dll_prog_ptr->xxx_next_program_ptr = NULL;

		//	AfxMessageBox("Set xxx to NULL");

                                       // Now have to bump up the dll
                                       //  pointer to the next program 
                                       //  (will directly follow current)
            dll_prog_ptr++;
		//	AfxMessageBox("Increment dll_prog_ptr");
                                       // And also move the desktop
                                       //  program pointer
                                       //
            desktop_prog_ptr = desktop_prog_ptr->xxx_next_program_ptr;
                                        
//AfxMessageBox("Bottom of inner loop");
        }

                                       // When exit inner loop then
                                       //  dll_program_ptr will point
                                       //  to where the next group goes
                                       //  (unless on last one)
                                       //
        dll_group_ptr = (SappGroupRec_wFullData *)dll_prog_ptr;

                                       // Bump desktop group ptr
                                       //
        desktop_group_ptr = desktop_group_ptr->xxx_next_group_ptr;

//AfxMessageBox("Bottom of outer loop");
    }

//AfxMessageBox("Done now with copyLrsFromDtToDllShmem");

    return(TRUE);
}




/////////////////////////////////////////////////////////////////////////////
// countShmemLrsDataBytesNeeded 
// ____________________________
//
// Works through given LRS data and counts number of bytes required.

int CdllUser::countShmemLrsDataBytesNeeded
                                (const SshmemLoginReplyStruct *lrs_ptr)
{
    int num_app_groups;

    num_app_groups = lrs_ptr->app_group_cnt;


    SappGroupRec_wFullData *app_group_rec_ptr;
    app_group_rec_ptr = lrs_ptr->xxx_app_group_list;

    int num_total_progs = 0;
                                       // Loop through each application
                                       //  and add up the number of 
                                       //  programs
                                       //
    
    for (int i=0; i < num_app_groups; i++)
    {
		/*temp.Format("addr of grp rec ptr-> %p",app_group_rec_ptr);

		AfxMessageBox(temp);*/
        num_total_progs += app_group_rec_ptr->program_cnt;

		 //AfxMessageBox(app_group_rec_ptr->progroup_title_buf);

        app_group_rec_ptr = app_group_rec_ptr->xxx_next_group_ptr;
    }
                                       // Need uint for validity check,
                                       //  one user rec, and one record
                                       //  for each app group and for each
                                       //  app group program
    int total_bytes_needed =
            ( (sizeof(unsigned int)) * 3 + //for the validity chk & 2 other extra things in login
			  (sizeof(SuserInfoRec_wFullData)) + //reply structure
              (sizeof(SappGroupRec_wFullData) * num_app_groups) +
              (sizeof(SappProgramRec_wFullData) *  num_total_progs) );

	

    return(total_bytes_needed);
}



/////////////////////////////////////////////////////////////////////////////
// getAppNameList 
// ______________
//
// Returns comma separated list of application names that are stored in
// the LRS data.

BOOL CdllUser::getAppNameList(char *lpszAppList, 
							  const int nSize, 
							  Csession *session)
{
                                       // The first group starts
                                       //  just beyond the
                                       //  SshmemLoginReplyStruct 
                                       //  at the start of the
                                       //  m_pShmemDllUserData data
                                       //
    SappGroupRec_wFullData *app_group_ptr =
                            (SappGroupRec_wFullData *)
                            ( (char *)m_pShmemDllUserData
                               + sizeof(SshmemLoginReplyStruct) );

    char tmpbuf[4096] = "";

    int max_len = ((nSize < 4096)
                    ? nSize
                    : 4096);
    int idx = 0;
    int j,len;


    for (unsigned int i=0; i < m_pShmemDllUserData->app_group_cnt; i++)
    {
        len = strlen(app_group_ptr->progroup_title_buf) ;
        if (len > SIZE_GROUP_TITLE)
            len = SIZE_GROUP_TITLE;

        if ( (len + idx + 1) > max_len)
        {
            session->ssnErrorMan()->RecordError
                            ("Buffer too small to receive NTCSS DLL data");
            return(FALSE);
        }

        for (j = 0; j < len; j++)
            tmpbuf[idx++] = *(app_group_ptr->progroup_title_buf+j);

                                       // To get to next group have
                                       //  to jump over the current
                                       //  SappGroupRec_wFullData plus
                                       //  one SappProgramRec_wFullData
                                       //  for each of the group programs
                                       //
        app_group_ptr = (SappGroupRec_wFullData *)
                         ( (char *)app_group_ptr +
                            sizeof(SappGroupRec_wFullData) + 
                            (app_group_ptr->program_cnt * 
                             sizeof(SappProgramRec_wFullData)) );
        tmpbuf[idx++] = ',';
    }

    tmpbuf[idx-1] = '\0';
    strcpy(lpszAppList, tmpbuf);

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// loginName 
// _________

const CString CdllUser::loginName()
{ 
	TCHAR lpszLoginName[SIZE_LOGIN_NAME+1];
	memset(lpszLoginName,NULL,SIZE_LOGIN_NAME+1);

	if (m_pShmemDllUserData != NULL)
	{
		strncpy(lpszLoginName,m_pShmemDllUserData->user_rec.login_name_buf,SIZE_LOGIN_NAME);
		return lpszLoginName;
	}
	else
		return _T(""); 
}



/////////////////////////////////////////////////////////////////////////////
// CdllUser::CMcall_initializeShmemLrsData 
// _______________________________________
//
// Called by the DeskTop application via the Ntcss_SetLRS API to
// allow the NTCSS DLL to copy the LRS data into shared memory.  

BOOL CdllUser::CMcall_initializeShmemLrsData(void *the_ptr)
{
//AfxMessageBox("Top of CMcall_initializeShmemLrsData");

    if (m_pShmemDllUserData != NULL)
    {
        gbl_dll_man.dllErrorMan()->RecordError
                ("Illegal desktop re-initialization");
        return(FALSE);
    }

    SshmemLoginReplyStruct *desk_top_data = 
                                    (SshmemLoginReplyStruct *)the_ptr;

									   // First need to count the number
									   // of bytes of shared memory that
									   // is needed
/*
AfxMessageBox("Before show");
char buf[1000];
sprintf(buf,"%s %d %s %s %s %d %s %d", 
		desk_top_data->user_rec.token_buf,
		desk_top_data->user_rec.unix_id,
		desk_top_data->user_rec.login_name_buf,
		desk_top_data->user_rec.ssn_buf,
		desk_top_data->user_rec.phone_number_buf,
		desk_top_data->user_rec.security_class,
		desk_top_data->user_rec.password_change_time,
		desk_top_data->user_rec.user_role);
AfxMessageBox(buf); 
*/

    int num_shmem_bytes_need = countShmemLrsDataBytesNeeded(desk_top_data);


   /* CString temp;
	temp.Format("total bytes needed -> %d",num_shmem_bytes_need);
	AfxMessageBox(temp);*/

    BOOL ok;

//AfxMessageBox("In dlluser doing file mapping");
                                       // Now create memory map file
    m_hMapObject =
				    
					CreateFileMapping
                         ( (HANDLE) 0xFFFFFFFF,      // Use paging file
                            NULL,                    // No security attr.
                            PAGE_READWRITE,          // Read/Write access
                            0,                       // size: high 32-bits
                            (DWORD)num_shmem_bytes_need,// size: low 32-bits
                            SHMEM_LOGINREP);         // Map name

    ok = (m_hMapObject != NULL);

    if (ok)
    {
//AfxMessageBox("In dlluser did file mapping ok");
		// Now get pointer to it
//AfxMessageBox("In dlluser doing file view mapping");
        m_pShmemDllUserData = 
                    (SshmemLoginReplyStruct *)
                    MapViewOfFile
                            (m_hMapObject, // Map to here
                             FILE_MAP_WRITE,          // Read access
                             0,                       // high offset
                             0,                       // low offset
                             0);   // map all of file

        ok = (m_pShmemDllUserData != NULL);
    }

									   // Copy LRS data into the shared
									   //  memory now
//AfxMessageBox("In dlluser doing file view mapping");

    if (ok)
{

	
        ok = copyLrsFromDtToDllShmem(desk_top_data,m_pShmemDllUserData);


}
									   // Need to tell gbl_dll_man
									   //  the size of the LRS data
    if (ok)
        ok = gbl_dll_man.UScall_setSizeOfUserLrsShmem
                                                (num_shmem_bytes_need);

//AfxMessageBox("Back from UScall_setSizeOfUserLrsShmem");

    if (ok)
        return(TRUE);

    else
    {
//AfxMessageBox("Returning false");//////////////////////debug
        gbl_dll_man.dllErrorMan()->RecordError
                ("User setup error - application still attached to prior login session");
        return(FALSE);
    }
}




/////////////////////////////////////////////////////////////////////////////
// CMcall_invalidateShmemLrsData
// _____________________________
//
// Called by the DeskTop application via the NtcssDTReset API to
// allow the NTCSS DLL release the memory shared for the LRS data.  

BOOL  CdllUser::CMcall_invalidateShmemLrsData()
{
    if (!gbl_dll_man.thisIsDeskTopDll())
        return(FALSE);

    if(!::UnmapViewOfFile(m_pShmemDllUserData))
		return(FALSE);
	
	if(!::CloseHandle(m_hMapObject))
		return(FALSE);


	m_pShmemDllUserData = NULL;

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// DMcall_linkToShmemLrsData
// _________________________
//
// Called for all instances of the NTCSS DLL except for DeskTop -
// links the LRS shared memory.

BOOL  CdllUser::DMcall_linkToShmemLrsData(unsigned int size)
{
    BOOL ok;

                                       // First link to memory map file
                                       //


    m_pShmemDllUserData = (SshmemLoginReplyStruct *)
				    
					CreateFileMapping
                         ( (HANDLE) 0xFFFFFFFF,      // Use paging file
                            NULL,                    // No security attr.
                            PAGE_READWRITE,          // Will only read it
                            0,                       // size: high 32-bits
                            size,             // size: low 32-bits
                            SHMEM_LOGINREP);         // Map name
					

	
	
   // ok = (m_pShmemDllUserData != NULL);

                                       // Must already exist or else is
                                       //   an error 
                                       //


    ok = (GetLastError() == ERROR_ALREADY_EXISTS);



    if (ok)
    {

		
                                       // Now get pointer to it
        m_pShmemDllUserData = 
                    (SshmemLoginReplyStruct *)
                    MapViewOfFile
                            (m_pShmemDllUserData, // Map to here
                             FILE_MAP_WRITE,          // Read access
                             0,                       // high offset
                             0,                       // low offset
                             0);                      // map all of file

        ok = (m_pShmemDllUserData != NULL);

    }

                                       // Make sure is valid data
    if (ok)
        ok = (m_pShmemDllUserData->validity_check_value ==
              NTCSSCODE_VALITY_CHECK_VALUE); 

    if (!ok)
    {
		gbl_dll_man.dllErrorMan()->RecordError
                ("Illegal desktop shared data initialization");

        return(FALSE);
    }

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// setAppData 
// __________
//
// NEEDTODO
//  Probably need a lock here
//

BOOL CdllUser::setAppData(const char *lpszAppName, 
					      const char *lpszAppData)
{
    if (!isValid())
        return(NULL);

    const SappGroupRec_wFullData *app_group_ptr = appGroup(lpszAppName);

    if (app_group_ptr == NULL)
        return(FALSE);      

    SappGroupRec_wFullData *nonconst_app_group_ptr =
                             (SappGroupRec_wFullData *)app_group_ptr;

    strncpy(nonconst_app_group_ptr->app_data_buf,
            lpszAppData, SIZE_APP_DATA);

    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// userInfo 
// ________

const SuserInfoRec_wFullData *CdllUser::userInfo() const
{
    if (!isValid())
        return(NULL);

    return (&m_pShmemDllUserData->user_rec);
}




void CdllUser::WalkLRS(const SshmemLoginReplyStruct *pDesktopLrsData)
{



    AfxMessageBox("In Walk LRS");

	char buf[10000];
  
    const SuserInfoRec_wFullData *desktop_user_ptr; 
    

    SappGroupRec_wFullData      *desktop_group_ptr;
    
    SappProgramRec_wFullData    *desktop_prog_ptr;

    unsigned int i,j;
                                       

    desktop_user_ptr =(&pDesktopLrsData->user_rec);

	memset(buf,NULL,10000);

    memcpy(buf,desktop_user_ptr,
          sizeof(SuserInfoRec_wFullData));

   

                                       // The first group will start
                                       //  just beyond the
                                       //  SshmemLoginReplyStruct 
                                       //  at the start of the
                                       //  pDllLrsData data
                                       //
 
                                       // Get desktop_group_ptr set to
                                       //  the first group 
                                       //
    desktop_group_ptr = pDesktopLrsData->xxx_app_group_list;
     
                                       // Now for each group copy data
                                       //  from the desktop to the dll
                                       //  shared memory area (outer
                                       //  loop)
                                       //  
   memset(buf,NULL,10000);

   memcpy(buf,desktop_group_ptr,
          sizeof(SappGroupRec_wFullData));
CString str;
    for (i=0; i < pDesktopLrsData->app_group_cnt; i++)
    {
/*str.Format("Outer group loop - before doing memcpy #%d//%d of SappGroupRec_wFullData",
i,pDesktopLrsData->app_group_cnt);
AfxMessageBox(str);*/

str.Format("Group name is: %s and address is %x next address is %x",
desktop_group_ptr->progroup_title_buf, desktop_group_ptr,desktop_group_ptr->xxx_next_group_ptr);
AfxMessageBox(str);

     
                                       //
        desktop_prog_ptr = desktop_group_ptr->xxx_program_list;

                                       // Now loop for each program
                                       //  and copy program data over
                                       //  (inner loop)
                                       //
        for (j=0; j < desktop_group_ptr->program_cnt; j++)
        {

str.Format("Prog name is: %s address is %x and next program pointer is %x",
desktop_prog_ptr->program_title_buf, desktop_prog_ptr,desktop_prog_ptr->xxx_next_program_ptr);
AfxMessageBox(str);


memset(buf,NULL,10000);


            memcpy(buf, desktop_prog_ptr,
                   sizeof(SappProgramRec_wFullData));
			
      
                                       //
            desktop_prog_ptr = desktop_prog_ptr->xxx_next_program_ptr;
                                        
//AfxMessageBox("Bottom of inner loop");
        }

                                       // When exit inner loop then
                                       //  dll_program_ptr will point
                                       //  to where the next group goes
                                       //  (unless on last one)
                                       //
        

                                       // Bump desktop group ptr
                                       //
        desktop_group_ptr = desktop_group_ptr->xxx_next_group_ptr;

//AfxMessageBox("Bottom of outer loop");
    }

//AfxMessageBox("Done now with copyLrsFromDtToDllShmem");
AfxMessageBox("Leaving Walk LRS");

}

BOOL CdllUser::SetMasterName()
//called by USE_MASTER_SERVER & USE_AUTH_SERVER (via GetMasterName if m_strMasterServer is empty)
//which ever hits it first it's only done ONCE 

{
		
		CmsgGETMASTERNAME msg;
		
		if(!msg.Load())
			return FALSE;
		
		if(!msg.DoItNow())
			return FALSE;
					
		if(!msg.UnLoad(m_strMasterServer.GetBuffer(SIZE_HOST_NAME)))
		//{
		//	AfxMessageBox("Get Master message failed"); //debug
			return FALSE;
	//	}
		
		m_strMasterServer.ReleaseBuffer();
		//AfxMessageBox("Get Master worked -> " + m_strMasterServer); //debug

		
		return TRUE;

	
}

CString CdllUser::GetMasterName()
{   //caches it after first hit
	if(m_strMasterServer.IsEmpty())
	{
		if(SetMasterName())
			return m_strMasterServer;
		else
		{
			AfxMessageBox(_T("Unable to get Server Name!!!!"));
			return _T(""); //bad things
		}
	}
	else

	return m_strMasterServer;
}
