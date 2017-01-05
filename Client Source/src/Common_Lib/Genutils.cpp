/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\genutils.cpp
//
// WSP, last update: 9/10/96
//                                         
// This implementation file contains the general purpose routines whose
//  prototypes are given in ..\include\genutils.h.  See routines for
//  details.
//
//----------------------------------------------------------------------


#include "ctype.h"
#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////////
// centerScreen
// _______________
//
// Moves window to center of screen.
//

void centerScreen(HWND hWnd)
{   
    RECT dlgrect;
    int  dlgheight, dlgwidth, x, y; 
    SIZE screen;
                                       // Get Screen size
                                       //
    screen.cx = GetSystemMetrics(SM_CXSCREEN); 
    screen.cy = GetSystemMetrics(SM_CYSCREEN);

                                       // Get window size
                                       //
    GetWindowRect(hWnd, (LPRECT)&dlgrect);
    dlgheight = dlgrect.bottom - dlgrect.top;
    dlgwidth  = dlgrect.right - dlgrect.left;

                                       // Calculate coords dlg needs 
                                       //    to be at.
    x = screen.cx/2 - dlgwidth/2;  
    y = screen.cy/2 - dlgheight/2;  
                                       // Move window to new position
                                       //
    MoveWindow(hWnd, x,y,dlgwidth,dlgheight,TRUE);  
} 



/////////////////////////////////////////////////////////////////////////////
// getTempFileName
// _______________
//
// Uses _tempnam to generate a temporary file name, if problems then
//  will name file \TMP.TMP
//

void getTempFileName(char *temp_file_name, int len)
{
    char *buf_ptr = _tempnam("","t_");
    BOOL got_it = FALSE;

    if (buf_ptr != NULL)
    {
        if (strlen(buf_ptr) <= (unsigned) len)
        {
            strcpy(temp_file_name, buf_ptr);
            got_it = TRUE;
        }
        free(buf_ptr);
    }
    if (!got_it)
        strcat(temp_file_name,"\\TMP.TMP");
}



/////////////////////////////////////////////////////////////////////////////
// isBlank
// _______

BOOL isBlank(char* str)
{
    char* s = str;

    if (!(s && *s))
        return(TRUE);

    while (*s) 
    {
        if (!isspace(*s))
            return(FALSE);
        s++;
    }
    
    return(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// stripNewline
// ____________

void stripNewline(char* str)
{
    int leng;
    
    leng = strlen(str);
    if (str[leng - 1] == '\n')
        str[leng - 1] = '\0';
}



/////////////////////////////////////////////////////////////////////////////
// stripNewline
// ____________
//
// See Eric on usage?, WSP

int stripWord(int i, char* str, char* word)
{
    int j, n, leng;
    char *s, *w;

    n = i;
    leng = strlen(str);
    if (i >= leng)
        return -1;

    s = str;
    w = word;
    for (j = 0; j < n; j++)
        s++;
    while (isspace(*s)) 
    {
        s++;
        n++;
    }
    while (!isspace(*s) && (*s != 0)) 
    {
        *w = *s;
        s++;
        w++;
        n++;
    }
    *w = '\0';

    while (isspace(*s)) 
    {
        s++;
        n++;
    }

    return n;
}

       

/////////////////////////////////////////////////////////////////////////////
// strncpyWpad
// ____________
//
// Copies src_str to dest_str, padding with character pad.  The dest_str
// will be NULL terminated.
//

char *strncpyWpad(char *dest_str, const char *src_str, 
                    const int len, const char pad)

{
    int i;
    
    for (i=0; ((i<len) && (*(src_str+i) != '\0')); i++)
        *(dest_str+i) = *(src_str+i);

    for (; (i<len); i++)
    {
        *(dest_str+i) = pad;
    }
    
    return(dest_str);
}



/////////////////////////////////////////////////////////////////////////////
// strncatWpad
// ____________
//
// Like strncpyWpad but does a cat.
//

char *strncatWpad(char *dest_str, const char *src_str, 
                  const int len, const char pad)

{
    int i,j;
    
    for (i=0; ((i<len) && (*(dest_str+i) != '\0')); i++)
        
        ; // NULL statement intended
  
    for (j=0; ((i<len) && (*(src_str+j) != '\0')); i++,j++)
        
        *(dest_str+i) = *(src_str+j);
        
    for (; (i<len); i++)
    
        *(dest_str+i) = pad;

    return(dest_str);
}



/////////////////////////////////////////////////////////////////////////////
// trim
// ____
//
// Takes white space of both ends of the given string
//

char* trim(char* string)
{
    char* r;
    char* p;
    char* q;

    p = q = r = string;

    while (isspace(*p))
        p++;

    while (*p) 
    {
        if (!isspace(*p))
            r = q + 1;

        *q++ = *p++;
    }
    *r = 0;
    
    return(string);
}


extern "C" BOOL __declspec(dllexport) WINAPI NtcssRunOnce(const LPSTR szTitleText)


{
	
    TRACE1("In NtcssRunOnce Title is <%s>\n",szTitleText);

	HANDLE  hMutexOneInstance = CreateMutex(NULL,TRUE,szTitleText);
	TRACE1("Got mutex Handle-> %p\n",hMutexOneInstance);

    if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		TRACE0(_T("Mutex already existed!! \n"));
		 // see if previous instance exists
		HWND hWndPrev = ::FindWindow (NULL, szTitleText);
		if (!hWndPrev)
			return FALSE; // no previous instance found

		// find any open dialogs or message boxes
		if (HWND hwndTop = ::GetLastActivePopup (hWndPrev))
			hWndPrev = hwndTop ;

		// make it visible
		
		if (::IsIconic (hWndPrev))
			::ShowWindow (hWndPrev, SW_RESTORE);

		SetForegroundWindow(hWndPrev);

	     if(hWndPrev)
			CloseHandle(hWndPrev);

		return FALSE;
	}
	
	return hMutexOneInstance!=NULL; //ONLY WAY TO RETURN TRUE
}

