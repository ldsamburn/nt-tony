// MyListCt.cpp

#include "stdafx.h"
#include "MyListCtrl.h"
#include "afxtempl.h"
#include "resource.h"
#include "StringEx.h" //added 7/00 for CallBack support
#define IDC_COMMAND_LINE 5012 //had to add 5/30/98 for some reason
//#include "SetupHelp.h" 5/4


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( MyListCtrl, CListCtrl )

///////////////////////////////////////////////////////////////////
// MyListCtrl

// Constructor.
MyListCtrl::MyListCtrl() : 

m_nColumnSort(0), m_bSortAscending(TRUE), m_nColumnNumber(0), m_HelpTopics(NULL), m_WindowTopic(0),m_redrawcount(0), m_nRowNumber(-1)/*m_nRowNumber(0)5/27*/


{
}

 

// Destructor.
/* virtual */ MyListCtrl::~MyListCtrl()
{
}

//ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)

BEGIN_MESSAGE_MAP(MyListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(MyListCtrl)
   ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
   ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//ON_WM_LBUTTONDOWN()
///////////////////////////////////////////////////////////////////
// MyListCtrl helper functions

// Does the sent item exist?
BOOL MyListCtrl::IsItem(int nItem) const
{
   return GetItemCount() > nItem;
}

// Does the sent column exist?
BOOL MyListCtrl::IsColumn(int nCol) const
{
   LV_COLUMN lvc;
   memset(&lvc, 0, sizeof(lvc));
   lvc.mask = LVCF_WIDTH;
   return GetColumn(nCol, &lvc);
}

// Returns the first item that is selected by default.
int MyListCtrl::GetSelectedItem(int nStartItem /* = -1 */ ) const
{
   return GetNextItem(nStartItem, LVNI_SELECTED);
}

// Selects the sent item.
BOOL MyListCtrl::SelectItem(int nItem) 
{
   return SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
}

BOOL MyListCtrl::SelectItem(CString strItem)
{
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strItem;
	int i;
	
	if((i=FindItem(&ls_findinfo,-1))!=-1 && SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED) &&
		EnsureVisible (i, FALSE))
		return TRUE;
	else 
		return FALSE;
}

BOOL MyListCtrl::SelectItemPartial(CString strItem)
{
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING|LVFI_PARTIAL;
	ls_findinfo.psz=strItem;
	int i;
	
	if((i=FindItem(&ls_findinfo,-1))!=-1 && SetItemState(i,/*LVIS_SELECTED|*/LVIS_FOCUSED,/*LVIS_SELECTED|*//*LVIS_FOCUSED*/0x0001) &&
		EnsureVisible(i,FALSE))
		return TRUE;
	else 
		return FALSE;
}

// Selects all the items in the control.
BOOL MyListCtrl::SelectAll()
{
   BOOL bReturn(TRUE);

   for (int nItem = 0; IsItem(nItem); nItem++) 
      if (! SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED)) {
         bReturn = FALSE;
         break;
      }

   return bReturn;
}


BOOL MyListCtrl::DeleteItem(CString strItem)
{
	LV_FINDINFO ls_findinfo;
	::ZeroMemory(&ls_findinfo, sizeof(ls_findinfo));
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strItem;

	return CListCtrl::DeleteItem(FindItem(&ls_findinfo,-1));
}

int MyListCtrl::GetItemNumber(CString strItem)
{
	LV_FINDINFO ls_findinfo;
	::ZeroMemory(&ls_findinfo, sizeof(ls_findinfo));
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strItem;

	return CListCtrl::FindItem(&ls_findinfo,-1);
}

// Selects all the items in the control.
BOOL MyListCtrl::DeSelectAll()

{
	 BOOL bReturn(TRUE);

   for (int nItem = 0; IsItem(nItem); nItem++) 
      if (! SetItemState(nItem, 0, LVNI_SELECTED)) {
         bReturn = FALSE;
         break;
      }

   return bReturn;

}


int MyListCtrl::GetColumnNumber(CString str_ColumnName)

{
	LV_COLUMN lvc;
	memset(&lvc, 0, sizeof(lvc));
	lvc.mask=LVCF_TEXT;
	lvc.cchTextMax=_MAX_PATH;
	CString buf;
	str_ColumnName.MakeUpper();

	for (int nCol = 0; IsColumn(nCol); nCol++) 
	{
		
		lvc.pszText=buf.GetBufferSetLength(_MAX_PATH);
		GetColumn(nCol,&lvc);
		buf.ReleaseBuffer();
		buf.MakeUpper();

		if(buf == str_ColumnName)
			return nCol;
	}
	return -1;
}


///////////////////////////////////////////////////////////////////
// MyListCtrl mode functions

// handle column clicks.
/*void MyListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;

   // If a different column is being sorted from the last time we
   // sorted, we always start off ascending.
   if (pNMListView->iSubItem != m_nColumnSort)
      m_bSortAscending = TRUE;
   else
      m_bSortAscending = ! m_bSortAscending;

   m_nColumnSort = pNMListView->iSubItem;

   // Now, the only way the CListCtrl can know how to sort is by
   // each item's LPARAM, so we give it the item number.
   for (int nItem = 0; IsItem(nItem); nItem++)
      SetItemData(nItem, nItem);

   // Call the sort routine.
   SortItems(CompareFunc, reinterpret_cast <DWORD> (this));
   *pResult = 0;
}*/


void MyListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;

   // If a different column is being sorted from the last time we
   // sorted, we always start off ascending.
   if (pNMListView->iSubItem != m_nColumnSort)
      m_bSortAscending = TRUE;
   else
      m_bSortAscending = ! m_bSortAscending;

   m_nColumnSort = pNMListView->iSubItem;

   // Now, the only way the CListCtrl can know how to sort is by
   // each item's LPARAM, so we give it the item number.

   if(!m_bLParamValid) //we make it unique
	   for (int nItem = 0; IsItem(nItem); nItem++)
		  SetItemData(nItem, nItem);

   // Call the sort routine.
   //Changed 7/00 for CallBacks
   if(!m_bUsesCallBacks) 
	SortItems(CompareFunc, reinterpret_cast <DWORD> (this));
   else
	SortItems(CompareFunc2, reinterpret_cast <DWORD> (this));

   m_headerctrl.SetSortImage( m_nColumnSort, m_bSortAscending );


#ifndef SKIP_HELP
   if(m_HelpTopics) //means there is help
   SetHelpTopic((*(m_HelpTopics+pNMListView->iSubItem)));
#endif
   

   //Help Stuff
   
 /*  if(m_HelpTopics) //means there is help
   {
	   help_object.SetHelpTopics(m_WindowTopic,(*(m_HelpTopics+pNMListView->iSubItem)));
	   help_object.SemiAutomatic();
   }*/ //5/4

	   *pResult = 0;

}

// This is the function that the base CListCtrl code calls whenever
// it needs to compare two items.
int CALLBACK MyListCtrl::CompareFunc(LPARAM lParam1,
                                             LPARAM lParam2,
                                             LPARAM lParamSort)
{
   MyListCtrl* pListCtrl =
      reinterpret_cast <MyListCtrl*> (lParamSort);

   LV_FINDINFO lvi;
   memset(&lvi, 0, sizeof(lvi));
   lvi.flags = LVFI_PARAM;

   lvi.lParam = lParam1;
   int nItem1(pListCtrl->FindItem(&lvi));

   lvi.lParam = lParam2;
   int nItem2(pListCtrl->FindItem(&lvi));

   CString s1(pListCtrl->GetItemText(nItem1,
      pListCtrl->m_nColumnSort));

   CString s2(pListCtrl->GetItemText(nItem2,
      pListCtrl->m_nColumnSort));

   int nReturn(s1.CompareNoCase(s2));

   if (! pListCtrl->m_bSortAscending)
      nReturn = -nReturn;

   return nReturn;
}

int CALLBACK MyListCtrl::CompareFunc2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{

	MyListCtrl* pListCtrl =
    reinterpret_cast <MyListCtrl*> (lParamSort);

	CStringEx* s1 = (CStringEx*) lParam1;
	CStringEx* s2 = (CStringEx*) lParam2;
	CString cs1,cs2;

	cs1=s1->GetField((char) 1,pListCtrl->m_nColumnSort);
	cs2=s2->GetField((char) 1,pListCtrl->m_nColumnSort);

	int nReturn(cs1.CompareNoCase((LPCTSTR)cs2));

	if (!pListCtrl->m_bSortAscending)
		  nReturn = -nReturn;

	return nReturn;

}

#if defined MYLISTCTRL_COLUMNWIDTHS

int MyListCtrl::SetModeColumnWidths(LV_COLUMN listColumn)
//tries to insert columns into control from the registry
//using last values for col order & width
{
    
  	HKEY hKey(NULL);

    if(ERROR_SUCCESS==::RegOpenKeyEx(HKEY_CURRENT_USER,m_sColumnWidthsKey,
        NULL, KEY_READ , &hKey))
    
	{
		// If key must there, open columns until you run out.
	   //AfxMessageBox("In here");
       LONG lnReturn(ERROR_SUCCESS);
	   char buf[_MAX_PATH];
	   DWORD keylen=_MAX_PATH;
	   DWORD dwType= REG_SZ;
	   unsigned char val[_MAX_PATH];
	   DWORD valuelen=_MAX_PATH;
	   CString strColumnOrder;
 

       for (DWORD item = 0; ERROR_SUCCESS == lnReturn; item++) 
	   {
		    keylen = _MAX_PATH; //these get modified each iteration
			valuelen = _MAX_PATH;
	
			lnReturn =RegEnumValue(hKey,item,buf,&keylen,NULL,&dwType,
	                     val,&valuelen);
		
		   if(lnReturn==ERROR_SUCCESS)
			   if(!strstr(buf,"!!CS!!"))
				   SetColumnWidth(GetColumnNumber(buf), atoi((const char*)val));
			   else
				   strColumnOrder=val;

	   }
	   ASSERT(!strColumnOrder.IsEmpty());
	  // for (int nCol = 0; IsColumn(nCol); nCol++); //get column count
	   int *pArray=new int[	m_nMaxColumn];
	   char *token = strtok((char*)(LPCTSTR) strColumnOrder, "," );
	   int i=0;	
	   while( token != NULL )
	   {
		   *(pArray + i++)=atoi(token);
		   token = strtok( NULL, "," );
	   }
	   ListView_SetColumnOrderArray(*this,m_nMaxColumn,pArray); //sort columns 
	   delete [] pArray;
	   ::RegCloseKey(hKey);
	   hKey = NULL;
	   return item;
	}
	else
		return FALSE;
 }

#endif


// When the control is about to be destroyed, we write the column
// widths and there current order to the Registry key specified by the client
// as well as the column names. Key->Column Name Value->"n_order,n_width"
void MyListCtrl::OnDestroy() 
{
   CListCtrl::OnDestroy();
   
   #if defined MYLISTCTRL_COLUMNWIDTHS

   SaveToReg();
 
   #endif
}

void MyListCtrl::SaveToReg() 
{

  if (! m_sColumnWidthsKey.IsEmpty()&&IsColumn(0)) 
   {
      HKEY hKey(NULL);
      DWORD dwDisp(0L);
	  LV_COLUMN lvc;
	  lvc.mask=LVCF_TEXT;
	  
	  char buf[_MAX_PATH];
	  lvc.pszText=buf;
	  CString str_RegValue;
	
	//this code gets the "real" column order
	//handles changes made by drag and drop
    
    //for (int nCol = 0; IsColumn(nCol); nCol++); //get column count
	
	int *pArray=new int[m_nMaxColumn];

	ListView_GetColumnOrderArray(GetSafeHwnd(),m_nMaxColumn,pArray);  //get the real column order


  ::RegCreateKeyEx(HKEY_CURRENT_USER, m_sColumnWidthsKey,
    NULL, "MyListCtrlClass", 0L, KEY_WRITE, NULL, &hKey,
    &dwDisp);

	CString ColOrder,temp;

	for (int nCol = 0; IsColumn(nCol); nCol++)
	{
		temp.Format("%d,",*(pArray+nCol));
		ColOrder+=temp;
	}

	char *ColumnOrder={"!!CS!!"};
	ColOrder=ColOrder.Left(ColOrder.GetLength()-1);
	
	::RegSetValueEx(hKey,ColumnOrder,NULL,REG_SZ,
		 (CONST BYTE*)(LPCTSTR)ColOrder,ColOrder.GetLength());

	
//	const BYTE* pData;

      for (nCol = 0; IsColumn(nCol); nCol++) 
	  {
         
		  lvc.cchTextMax=_MAX_PATH;
		  GetColumn(nCol,&lvc);
	  	  temp.Format("%d",(GetColumnWidth(nCol)));
		  //pData=reinterpret_cast<const BYTE*> (&nData);
          ::RegSetValueEx(hKey,buf,NULL,REG_SZ,(CONST BYTE*)(LPCTSTR)temp,
		  temp.GetLength());
      }
	  
	  delete [] pArray;
      ::RegCloseKey(hKey);
      hKey = NULL;
   }
}

int MyListCtrl::Write(CString str_data)
{
	

	int ret=0;
	CString temp=str_data;

	
	if(m_nColumnNumber==0)
	{
		m_listItem.pszText=str_data.GetBuffer(temp.GetLength());
		//m_listItem.iItem=0; //12/14 insert @ end
		m_nRowNumber++; //5/27 test with it last set it to -1 in constuctor
		m_listItem.iItem=m_nRowNumber; //5/27 test with it last
		m_nRowNumber=InsertItem(&m_listItem);
		ASSERT(ret!=-1);
		m_nColumnNumber++;
		str_data.ReleaseBuffer();
	
	}
	else
	{
		SetItemData(m_nRowNumber,m_nRowNumber);
		SetItemText(m_nRowNumber,m_nColumnNumber++,str_data); 
	}
	if(m_nColumnNumber==m_nMaxColumn)
		m_nColumnNumber=0;
	return m_nRowNumber;  //12/14 return row #
}






void MyListCtrl::SetUp(LV_COLUMN listColumn,LV_ITEM listItem,CString str_Columns,
					   DWORD db_NewAttributes,CString str_RegKey/*=_T("")*/,
					   long *HelpTopics/*=NULL*/,BOOL bLParamValid/*=FALSE*/,
					   BOOL bUsesCallBacks/*=FALSE*/) 
{

	//stuff for help
	m_HelpTopics=HelpTopics;

	m_bLParamValid=bLParamValid; //TRUE means you're resposible for keeping it unique
	m_bUsesCallBacks=bUsesCallBacks; //added 7/00 to handle callback LC's & sorting
    

	//Clear();
	m_listItem=listItem;
	m_sColumnWidthsKey=str_RegKey;

	//Set ext styles
	ListView_SetExtendedListViewStyle(*this,db_NewAttributes); 


	char *token = strtok((char*)(LPCTSTR) str_Columns, "," );
	int i=0;	
	while( token != NULL )
	{
		listColumn.iSubItem=i;
		listColumn.pszText=token;
		InsertColumn(i++,&listColumn);
		token = strtok( NULL, "," );
	}

	m_nMaxColumn=i;

    //detect if Ctrl has LVS_SORTASCENDING or LVS_SORTDESCENDING style
	//so we can display appropriate sort indicators
	DWORD dwStyle = GetStyle();
	if (LVS_SORTASCENDING & dwStyle)
	{
		m_bSortAscending=TRUE;
		m_headerctrl.SetSortImage( 0, m_bSortAscending );
	}
	else
		if(LVS_SORTDESCENDING & dwStyle)
		{
			m_bSortAscending=FALSE;
			m_headerctrl.SetSortImage( 0, m_bSortAscending );
		}
	


#if defined MYLISTCTRL_COLUMNWIDTHS
	
	if(m_sColumnWidthsKey.GetLength()>0)
		if(!SetModeColumnWidths(listColumn))
		//	AutoSizeColumns();
///#else
	//	AutoSizeColumns();



#endif

   
}

void MyListCtrl::Clear()
{
	DeleteAllItems();
	#if defined MYLISTCTRL_COLUMNWIDTHS
		SaveToReg() ;
	#endif
	while(DeleteColumn(0));
}

/////////////////////////////////////////////////////////////////////
/////  stuff for Editable SubItems
/////////////////////////////////////////////////////////////////////

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int MyListCtrl::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}



CEdit* MyListCtrl::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	pEdit->Create( dwStyle, rect, this, IDC_COMMAND_LINE );


	return pEdit;
}


void MyListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this) SetFocus();
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void MyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this) SetFocus();
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}



CString MyListCtrl::GetColumnName(int nCol) 
{
	LV_COLUMN lvc;
	//CString strCol;
	lvc.mask=LVCF_TEXT;
	char buf[_MAX_PATH];
	lvc.pszText=buf;
	lvc.cchTextMax=_MAX_PATH;
	GetColumn(nCol,&lvc);
	return lvc.pszText;

}


/*void MyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int index;
	CListCtrl::OnLButtonDown(nFlags, point);

	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
	
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag && colnum > 0)
		{
			// Add check for LVS_EDITLABELS
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
				EditSubLabel( index, colnum );
		}
		else
			SetItemState(index,LVIS_SELECTED|LVIS_FOCUSED,
						LVIS_SELECTED|LVIS_FOCUSED);
	}
}*/


void MyListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->SendMessage(WM_KEYDOWN,nChar,MAKELPARAM(nRepCnt,nFlags));
}


BOOL MyListCtrl::IsString(CString strData)
{
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strData;

	return(FindItem(&ls_findinfo,-1)!=-1);
}



int MyListCtrl::GetColumnCount() const
 {
	// get the header control
     CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
     // return the number of items in it - ie. the number of columns
     return pHeader->GetItemCount();
}


void MyListCtrl::AutoSizeColumns(int col /*=-1*/)
{
     // Call this after your list control is filled     
	
	 SetRedraw(false);
     int mincol = col < 0 ? 0 : col;
     int maxcol = col < 0 ? GetColumnCount()-1 : col;
     for (col = mincol; col <= maxcol; col++)
	 {
          SetColumnWidth(col,LVSCW_AUTOSIZE);
          int wc1 = GetColumnWidth(col);
          SetColumnWidth(col,LVSCW_AUTOSIZE_USEHEADER);
          int wc2 = GetColumnWidth(col);
          int wc = __max(MINCOLWIDTH,__max(wc1,wc2));
          SetColumnWidth(col,wc);
     }
     // RecalcHeaderTips(); *** uncomment this if you use my header tips method
     SetRedraw(true);
     // Invalidate(); *** uncomment this if you don't use my SetRedraw function}
}



void MyListCtrl::SetRedraw( BOOL bRedraw)
{
	if (! bRedraw)
	{
		if (m_redrawcount++ <= 0) 
			CListCtrl::SetRedraw(false);
	}
	else
	{
		if (--m_redrawcount <= 0)
		{
			CListCtrl::SetRedraw(true);
			m_redrawcount = 0;
			Invalidate();
		}
	}
}

/*void MyListCtrl::ClickSort()
{
	CHeaderCtrl* pHeader =(CHeaderCtrl*)GetDlgItem(0);
	pHeader->SendMessage(WM_LBUTTONDOWN, 0, 0);
}*/

//TODO there is a problem with this control where it clobbers the 
//LVS_SORTASCENDING or LVS_SORTDESCENDING styles & I can't figure 
//out why


// this function was written by Zafir Anjum from Code Geru.Com
// SortTextItems	- Sort the list based on column text
// Returns		- Returns true for success
// nCol			- column that contains the text to be sorted
// bAscending	- indicate sort order
// low			- row to start scanning from - default row is 0
// high			- row to end scan. -1 indicates last row
BOOL MyListCtrl::SortTextItems( int nCol, BOOL bAscending, 
					int low /*= 0*/, int high /*= -1*/)
{
	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;
		

	if( hi <= lo ) return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			//while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) ) jgj
			while( ( lo < high ) && (( midItem.CompareNoCase(GetItemText(lo, nCol))>0)))
				++lo;
		else
			//while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) ) jgj
			while( ( lo < high ) && (( midItem.CompareNoCase(GetItemText(lo, nCol))<0)))
				++lo;

		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			//while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) ) jgj
			while( ( hi > low ) && (( midItem.CompareNoCase(GetItemText(hi, nCol))<0)))
				--hi;
		else
			//while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) ) jgj
			while( ( hi > low ) && (( midItem.CompareNoCase(GetItemText(hi, nCol))>0)))
				--hi;

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = 
					((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
						LVIS_FOCUSED |  LVIS_SELECTED | 
						LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				GetItem( &lvitemlo );
				GetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));

				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

	return TRUE;
}


void MyListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	// Add initialization code
	m_headerctrl.SubclassWindow( ::GetDlgItem(m_hWnd,0) );
}



void MyListCtrl::SetSortIndicator(int nColumnSort/*=0*/,BOOL bSortAscending/*=TRUE*/)
{
	m_nColumnSort=nColumnSort;
	m_bSortAscending=bSortAscending;
	m_headerctrl.SetSortImage( m_nColumnSort, m_bSortAscending );
}


CString MyListCtrl::GetText(CString strValue,int nCol)
//returns the 1st item that matches the given value/col combo
{
	LV_FINDINFO ls_findinfo;
	int nRow;
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strValue;
	
	if((nRow=FindItem(&ls_findinfo,-1))==-1)
		return(_T(""));
	else
		return GetItemText(nRow,nCol);
}


BOOL MyListCtrl::SetText(CString strValue,int nCol,CString strSetValue)
{
	LV_FINDINFO ls_findinfo;
	int nRow;
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strValue;
	
	if((nRow=FindItem(&ls_findinfo,-1))==-1)
		return FALSE;
	else
		return SetItemText(nRow,nCol,strSetValue);
}


BOOL  MyListCtrl::Find(CString strItem)
{
	//Just a little BOOL to see if an item is in the control
	LV_FINDINFO ls_findinfo;
	ls_findinfo.flags=LVFI_STRING;
	ls_findinfo.psz=strItem;
	return FindItem(&ls_findinfo,-1)!=-1;
}

BOOL  MyListCtrl::Find(CString strItem,int nCol)
//this 1 does the find beyond the supported 1st column
//but is probably to inefficient for large amounts of data
{
	for(int i=0;i<GetItemCount();i++)
		if(GetItemText(i,2)==strItem)
			return TRUE;
	return FALSE;

}
