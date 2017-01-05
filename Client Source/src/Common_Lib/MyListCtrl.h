// MyListCt.h

#pragma once
#define MINCOLWIDTH 10  
#include "InPlaceEdit.h"
#include "MyHeaderCtrl.h"


//#define MYLISTCTRL_COLUMNWIDTHS  //enables all the registry stuff

///////////////////////////////////////////////////////////////////
// MyListCtrl class
class MyListCtrl : public CListCtrl
{
   DECLARE_DYNAMIC(MyListCtrl)
public:
   MyListCtrl();
   virtual ~MyListCtrl();

public:
   void   AutoSizeColumns(int col =-1);
   BOOL   IsItem(int nItem) const;
   BOOL   IsColumn(int nCol) const;
   BOOL   IsString(CString strData);
   int    GetSelectedItem(int nStartItem = -1) const;
   BOOL   SelectItem(int nItem);
   BOOL	  SelectItem(CString strItem);
   void   ResetCol(int nColumn=0){m_nColumnNumber=0;}
   void   SetCol(int nColumn=-1){m_nColumnNumber=(nColumn==-1?m_nMaxColumn-1:nColumn);}
   BOOL	  SelectItemPartial(CString strItem);
   BOOL   SelectAll();
   BOOL   DeSelectAll();
   BOOL   Find(CString strItem);
   BOOL   Find(CString strItem,int nCol);
   void	  SetSortIndicator(int nColumnSort=0,BOOL bSortAscending=TRUE);
   void	  SetRedraw( BOOL bRedraw);
   BOOL   DeleteItem(CString strItem);
   BOOL   DeleteItem()
	{return CListCtrl::DeleteItem(GetSelectedItem());}
   BOOL   DeleteItem(int nItem)
   {return CListCtrl::DeleteItem(nItem);} //need this to get ancestor version
   int    GetItemNumber(CString strItem);
   #ifndef SKIP_HELP
   virtual void SetHelpTopic(long lTopic)=0;
   #endif
   int   Write(CString str_data);  //12/14
   int    GetColumnNumber(CString str_ColumnName);
   CString GetText(int n_Item,CString str_Column)
   {return CListCtrl::GetItemText(n_Item,GetColumnNumber(str_Column));}
   CString GetText() {return CListCtrl::GetItemText(GetSelectedItem(),0);}
   CString GetText(int nCol) {return CListCtrl::GetItemText(GetSelectedItem(),nCol);}
   CString GetText(CString str_Column) /*overloaded to use default selection*/
   {return CListCtrl::GetItemText(GetSelectedItem(),GetColumnNumber(
   str_Column));}

   CString GetText(CString strValue,int nCol); //finds the first value for col & returns it

   int	SetText(CString strItem) {return CListCtrl::SetItemText(GetSelectedItem(),0,strItem);}
   int	SetText(CString strItem,int nCol) {return CListCtrl::SetItemText(GetSelectedItem(),nCol,strItem);}

   BOOL SetText(CString strValue,int nCol,CString strSetValue); //sets the first value for col & returns it

   void SetUp(LV_COLUMN listColumn,LV_ITEM listItem,CString str_Columns,
			  DWORD db_NewAttributes,CString str_RegKey=_T(""),long *HelpTopics=NULL,
			  BOOL bLParamValid=FALSE,BOOL bUsesCallBacks=FALSE);

   void   Clear();
   BOOL SortTextItems( int nCol, BOOL bAscending,int low= 0, int high= -1);
   CString GetColumnName(int nCol);
   //////////new stuff for Editable Subitems
   int HitTestEx(CPoint &point, int *col) const;
  
   CEdit* EditSubLabel( int nItem, int nCol );

 // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(MyListCtrl)
	//}}AFX_VIRTUAL

 
// Generated message map functions
   //afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
protected:
   //{{AFX_MSG(MyListCtrl)
   afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDestroy();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

int GetColumnCount() const;


static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,
                                LPARAM lParamSort);

//added 7/00 to handle callback LC's & sorting
static int CALLBACK CompareFunc2(LPARAM lParam1, LPARAM lParam2,
                                LPARAM lParamSort);

protected:
  
   int       m_nColumnSort;
   int       m_nColumnNumber,m_nRowNumber,m_nMaxColumn;
   BOOL      m_bLParamValid; //if not it gets clobbered (default) in sort
   int		 m_redrawcount;
   BOOL		 m_bSortAscending;
   BOOL	     m_bUsesCallBacks; //added 7/00 to handle callback LC's & sorting
   CString   m_sColumnWidthsKey;
   BOOL      SetModeColumnWidths(LV_COLUMN listColumn);
   void      SaveToReg();
   
   //stuff for help
   long *m_HelpTopics;
   long m_WindowTopic;
   CMyHeaderCtrl m_headerctrl;
   void PreSubclassWindow(); 


   LV_ITEM   m_listItem;
      
  
};

