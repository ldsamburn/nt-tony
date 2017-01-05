/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ScheduleDlg.cpp
//                                         
// This is the implementation file for the CDayButton and
// CScheduleDlg classes.
//
//----------------------------------------------------------------------

#include "stdafx.h"
#include <ntcssdef.h>
#include "dll_defs.h"
#include "ScheduleDlg.h"

#include "genutils.h"
#include "etc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//***************************************************************************
//***************************************************************************
// CDayButton
// __________

BEGIN_MESSAGE_MAP(CDayButton, CButton)
    //{{AFX_MSG_MAP(CDayButton)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CDayButton constructor
// ______________________

CDayButton::CDayButton() 
{
    m_bSelected = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// DrawItem
// ________

void CDayButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

    CRect rect(&(lpDrawItemStruct->rcItem));
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    UINT state = lpDrawItemStruct->itemState;
    UINT action = lpDrawItemStruct->itemAction;
                                      
                                       // Get buttons background color from 
                                       //   current security setting
    CBrush the_brush;

    if (m_bSelected)
        the_brush.CreateSolidBrush(BLUE_BACKGROUND_COLOR);
    else
        the_brush.CreateSolidBrush(g_etc.clrBtnFace);
    

    HBRUSH hOldBrush = (HBRUSH) ::SelectObject(pDC->m_hDC, 
                                               g_etc.hbrBtnFace);

    pDC->FillRect(rect,&the_brush);

                                       // Draw the outer border first 
                                       //   (thick if we're focused)
    BOOL bThick = state & ODS_FOCUS;
    DrawOuterFrame(pDC, rect, bThick);
                                       // Draw lines for the box 
                                       //
    CPen shadowPen(PS_SOLID, 1,SHADOW_COLOR);
    CPen *pOldPen = pDC->SelectObject(&shadowPen);

    pDC->MoveTo(rect.left, rect.bottom-1);
    pDC->LineTo(rect.left, rect.top);
    pDC->LineTo(rect.right, rect.top);      
    pDC->SelectObject(pOldPen); 

    pDC->SetBkMode(TRANSPARENT);
    if (state & ODS_SELECTED)
        rect.OffsetRect(1, 1);
    
    char day_str[3];

    sprintf(day_str,"%2d",m_nDayVal);
 
                                       // Draw the text
                                       //
    if ( (m_bSelected) && (m_nColorCode == RED_COLOR_CODE) )
        pDC->SetTextColor(RED_FOREGROUND_COLOR);

    else if ( (m_bSelected) && (m_nColorCode == WHITE_COLOR_CODE) )
        pDC->SetTextColor(WHITE_FOREGROUND_COLOR);

    else
        pDC->SetTextColor(g_etc.clrBtnText);

    
    pDC->DrawText(day_str, -1, rect, DT_SINGLELINE | DT_CENTER);

    
                                       // Draw the focus rectangle if needed
    if (state & ODS_FOCUS)
    {
        CRect focusRect(rect);
        focusRect.InflateRect(1, 1);
        pDC->DrawFocusRect(focusRect);      
    }

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(hOldBrush);
}



/////////////////////////////////////////////////////////////////////////////
// DrawOuterFrame
// ______________

void CDayButton::DrawOuterFrame(CDC *pDC, CRect& rect, BOOL bThick)
{
    CPen   blackPen(PS_SOLID, 1, RGB(0,0,0));
    CBrush brBlack(RGB(0,0,0));
    CBrush br_guess(RGB(58,200,121));
    
                                       // Draw the first outside rectangle
    CPen *pOldPen = pDC->SelectObject(&blackPen);
    
                                       // top side
    pDC->MoveTo(rect.left+1, rect.top); 
    pDC->LineTo(rect.right-1, rect.top);
    
                                       // right side
    pDC->MoveTo(rect.right-1, rect.top+1);
    pDC->LineTo(rect.right-1, rect.bottom-1);

                                       // bottom side  
    pDC->MoveTo(rect.right-2, rect.bottom-1); 
    pDC->LineTo(rect.left, rect.bottom-1);

                                       // left side    
    pDC->MoveTo(rect.left, rect.bottom-2);
    pDC->LineTo(rect.left, rect.top);

    rect.InflateRect(-1, -1);
    if (bThick)
    {
        pDC->FrameRect(rect, &brBlack);
        rect.InflateRect(-1, -1);
    }                                         

    pDC->SelectObject(pOldPen);
}



//***************************************************************************
//***************************************************************************
// CScheduleDlg
// ____________

/////////////////////////////////////////////////////////////////////////////
// CScheduleDlg constructor
// ________________________


CScheduleDlg::CScheduleDlg(CSchedule *sched_ptr,//=NULL
                           BOOL read_only,      //=FALSE
                           CWnd* pParent)       //=NULL
    : CDialog(CScheduleDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CScheduleDlg)
    m_nHour = 0;
    m_nMinute = 0;
	//}}AFX_DATA_INIT

    
    m_bReadyForInit = TRUE;
    m_bReadOnly = read_only;

                                       // A NULL sched_ptr is ok, but
                                       //  if is non-NULL then must be
                                       //  valid or dialog will not init
    if (sched_ptr != NULL)
    {
        if (!sched_ptr->IsValid())
        {
            m_bReadyForInit = FALSE;
            return;
        }
        m_TheSchedule = *sched_ptr;
    }                                  
                                       // Set up the m_MonthInfo
                                       //
    strcpy(m_MonthInfo[0].name,"JAN");
    m_MonthInfo[0].num_days = 31;
    strcpy(m_MonthInfo[1].name,"FEB");
    m_MonthInfo[1].num_days = 28;
    strcpy(m_MonthInfo[2].name,"MAR");
    m_MonthInfo[2].num_days = 31;
    strcpy(m_MonthInfo[3].name,"APR");
    m_MonthInfo[3].num_days = 30;
    strcpy(m_MonthInfo[4].name,"MAY");
    m_MonthInfo[4].num_days = 31;
    strcpy(m_MonthInfo[5].name,"JUN");
    m_MonthInfo[5].num_days = 30;
    strcpy(m_MonthInfo[6].name,"JUL");
    m_MonthInfo[6].num_days = 31;
    strcpy(m_MonthInfo[7].name,"AUG");
    m_MonthInfo[7].num_days = 31;
    strcpy(m_MonthInfo[8].name,"SEP");
    m_MonthInfo[8].num_days = 30;
    strcpy(m_MonthInfo[9].name,"OCT");
    m_MonthInfo[9].num_days = 31;
    strcpy(m_MonthInfo[10].name,"NOV");
    m_MonthInfo[10].num_days = 30;
    strcpy(m_MonthInfo[11].name,"DEC");
    m_MonthInfo[11].num_days = 31;
}



/////////////////////////////////////////////////////////////////////////////
// DoDataExchange and message map
// ______________________________

void CScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CScheduleDlg)
    DDX_Control(pDX, IDC_ENTRY_LIST, m_ctlSchedEntryList);
    DDX_Control(pDX, IDC_SCHEDULE_TYPE, m_ctlSchedType);
    DDX_Text(pDX, IDC_HOUR_VALUE, m_nHour);
    DDX_Text(pDX, IDC_MINUTE_VALUE, m_nMinute);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScheduleDlg, CDialog)
    //{{AFX_MSG_MAP(CScheduleDlg)
    ON_BN_CLICKED(IDC_MONTH_DOWN, OnMonthDown)
    ON_BN_CLICKED(IDC_MONTH_UP, OnMonthUp)
    ON_BN_CLICKED(IDC_YEAR_DOWN, OnYearDown)
    ON_BN_CLICKED(IDC_YEAR_UP, OnYearUp)
    ON_WM_VSCROLL()
    ON_BN_CLICKED(ID_DONE, OnDone)
    ON_CBN_SELCHANGE(IDC_SCHEDULE_TYPE, OnSelchangeScheduleType)
    ON_BN_CLICKED(IDC_DELETE_ENTRY, OnDeleteEntry)
    ON_BN_CLICKED(IDC_DISCARD_ENTRY_CHANGES, OnDiscardEntryChanges)
    ON_LBN_SELCHANGE(IDC_ENTRY_LIST, OnSelchangeEntryList)
    ON_BN_CLICKED(IDC_SAVE_ENTRY, OnSaveEntry)
    ON_BN_CLICKED(IDCANCEL, OnCancel)
    ON_BN_CLICKED(IDC_NEED_HELP, OnHelp)
	ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CalButtonPressed
// ________________
//
// Called when one of the CDayButton's (calendar day, 1..31) is pressed.
// Sets m_CurScheduleEntry using m_ttShownMonth and selected 
// m_DayButtons value, then redraws calendar to show selected dates.

void CScheduleDlg::CalButtonPressed(int button_id)
{
	int tbutton;

    if (m_bReadOnly)
        return;

    if (m_CurScheduleEntry.tt()->IsClear())
    {
        m_CurScheduleEntry.ttWrt()->SetToNow();
	
        UpdateData(TRUE);
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_hour = m_nHour;
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_min = m_nMinute;

        m_ttShownMonth.UpdateWith_tm_Val();
    }

    tbutton = button_id - IDC_DAY01;


    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_mon = 
                                m_ttShownMonth.tm_Ptr()->tm_mon;

    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_year = 
                                m_ttShownMonth.tm_Ptr()->tm_year;

    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_mday = 
                                m_DayButtons[tbutton].m_nDayVal;

    m_CurScheduleEntry.ttWrt()->UpdateWith_tm_Val();

    SetEntryHasChanged();
    DrawCalendar();

}                          



/////////////////////////////////////////////////////////////////////////////
// ChangeDayButtonSelection
// ________________________
//
// Called by DrawCalendar to set up a single CDayButton
// (m_DayButtons[button_idx], referred to in comments below for
// this function as 'this button') to indicate whether, according
// to the m_CurScheduleEntry.Type(), the button
// should be displayed as 'selected' or not, and if so the color
// that it should be displayed.  This function sets the CDayButton's
// m_bSelected and m_nColorCode values.  It returns TRUE if the value
// for m_bSelected was changed (either from FALSE to TRUE or vice-versa),
// or FALSE if no change takes place.

BOOL CScheduleDlg::ChangeDayButtonSelection(int button_idx)
{
                                       // If don't have a current
                                       //  entry and this button is
                                       //  set to be selected, then
                                       //  turn selection off.

    if (m_CurScheduleEntry.tt()->IsClear())
    {

        if (m_DayButtons[button_idx].m_bSelected)
        {

            m_DayButtons[button_idx].m_bSelected = FALSE;
            return(TRUE);
        }
        else                                 
        {

            return(FALSE);
        }
    }

    int wday_on;

    switch (m_CurScheduleEntry.Type())
    {
        case ONE_SHOT_SCHED_ID:
        case EXCEPT_SCHED_ID:
                                       // For one shots and excepts,
                                       //  see if mon/yr/mday matches
                                       //  exactly
                                       //

            if (   (m_CurScheduleEntry.tt()->tm_Ptr()->tm_mon 
                    == m_ttShownMonth.tm_Ptr()->tm_mon)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_year 
                    == m_ttShownMonth.tm_Ptr()->tm_year)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_mday 
                    == m_DayButtons[button_idx].m_nDayVal) )
            {
                                       // If so and already selected
                                       //  then okay, do nothing
                                       //
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    return(FALSE);
                }
                                       // Else need to color correctly
                else
                {
                    if (m_CurScheduleEntry.Type() == 
                        ONE_SHOT_SCHED_ID)
                    {
                        m_DayButtons[button_idx].m_nColorCode = 
                                                    WHITE_COLOR_CODE;
                    }
                    else
                        m_DayButtons[button_idx].m_nColorCode = 
                                                    RED_COLOR_CODE;

                    m_DayButtons[button_idx].m_bSelected = TRUE;

                    return(TRUE);
                }
            }
                                       // Else day didn't match up
            else
            {

                if (m_DayButtons[button_idx].m_bSelected)
                {

                    m_DayButtons[button_idx].m_bSelected = FALSE;
                    return(TRUE);
                }
                else            
                {

                    return(FALSE);
                }
            }
            break;

        case DAILY_SCHED_ID:
        case HOURLY_SCHED_ID:
                                       // For daily and hourly's,
                                       //  see if selected date is after
                                       //  this year/mon and current
                                       //  day value
                                       //
            if ( !SelectedDateIsAfter(m_ttShownMonth.tm_Ptr()->tm_year,
                                      m_ttShownMonth.tm_Ptr()->tm_mon,
                                      m_DayButtons[button_idx].m_nDayVal))
            {
                                       // If so and already selected
                                       //  then okay, do nothing
                                       //
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    return(FALSE);
                }
                                       // Else need to select and color
                else
                {
                    m_DayButtons[button_idx].m_bSelected = TRUE;
                    m_DayButtons[button_idx].m_nColorCode = 
                                                    WHITE_COLOR_CODE;
                    return(TRUE);
                }
            }                                             
                                       // Else day didn't match up
            else
            {
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    m_DayButtons[button_idx].m_bSelected = FALSE;
                    return(TRUE);
                }
                else
                {
                    return(FALSE);
                }
            }
            break;

        case WEEKLY_SCHED_ID:
                                       // For weeklys has to come after
                                       //  this year/mon and current
                                       //  day value and has to match
                                       //  weekday
                                       //
            wday_on = ((m_ttShownMonth.tm_Ptr()->tm_wday +
                        m_DayButtons[button_idx].m_nDayVal - 1) 
                       % 7); 

            if ( !SelectedDateIsAfter(m_ttShownMonth.tm_Ptr()->tm_year,
                                      m_ttShownMonth.tm_Ptr()->tm_mon,
                                      m_DayButtons[button_idx].m_nDayVal)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_wday == wday_on) )
            {
                                       // If so and already selected
                                       //  then okay, do nothing
                                       //
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    return(FALSE);
                }
                                       // Else need to select and color
                else
                {
                    m_DayButtons[button_idx].m_bSelected = TRUE;
                    m_DayButtons[button_idx].m_nColorCode = 
                                                    WHITE_COLOR_CODE;
                    return(TRUE);
                }
            }
                                       // Else day didn't match up
            else
            {
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    m_DayButtons[button_idx].m_bSelected = FALSE;
                    return(TRUE);
                }
                else            
                {
                    return(FALSE);
                }
            }
            break;

        case MONTHLY_SCHED_ID:
                                       // For monthly's has to come
                                       //  after current year/mon/day
                                       //  value and must match given
                                       //  mday
                                       //
            if ( !SelectedDateIsAfter(m_ttShownMonth.tm_Ptr()->tm_year,
                                      m_ttShownMonth.tm_Ptr()->tm_mon,
                                      m_DayButtons[button_idx].m_nDayVal)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_mday 
                    == m_DayButtons[button_idx].m_nDayVal) )
            {
                                       // If so and already selected
                                       //  then okay, do nothing
                                       //
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    return(FALSE);
                }
                                       // Else need to select and color
                else
                {
                    m_DayButtons[button_idx].m_bSelected = TRUE;
                    m_DayButtons[button_idx].m_nColorCode = 
                                                    WHITE_COLOR_CODE;
                    return(TRUE);
                }
            }
                                       // Else day didn't match up
            else
            {
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    m_DayButtons[button_idx].m_bSelected = FALSE;
                    return(TRUE);
                }
                else            
                {
                    return(FALSE);
                }
            }
            break;


        case YEARLY_SCHED_ID:
                                       // For yearly's has to come
                                       //  after current year/mon/day
                                       //  value and must match given
                                       //  mon and mday values
                                       //
            if ( !SelectedDateIsAfter(m_ttShownMonth.tm_Ptr()->tm_year,
                                      m_ttShownMonth.tm_Ptr()->tm_mon,
                                      m_DayButtons[button_idx].m_nDayVal)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_mon 
                    == m_ttShownMonth.tm_Ptr()->tm_mon)
                && (m_CurScheduleEntry.tt()->tm_Ptr()->tm_mday 
                    == m_DayButtons[button_idx].m_nDayVal) )
            {
                                       // If so and already selected
                                       //  then okay, do nothing
                                       //
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    return(FALSE);
                }
                                       // Else need to select and color
                else
                {
                    m_DayButtons[button_idx].m_bSelected = TRUE;
                    m_DayButtons[button_idx].m_nColorCode = 
                                                    WHITE_COLOR_CODE;
                    return(TRUE);
                }
            }
                                       // Else day didn't match up
            else
            {
                if (m_DayButtons[button_idx].m_bSelected)
                {
                    m_DayButtons[button_idx].m_bSelected = FALSE;
                    return(TRUE);
                }
                else            
                {
                    return(FALSE);
                }
            }
            break;

        default:
            return (FALSE);
    }
}



/////////////////////////////////////////////////////////////////////////////
// DrawCalendar
// ____________

void CScheduleDlg::DrawCalendar(BOOL sched_month_changed,
                                BOOL selected_entry_changed,
                                BOOL force_daybutton_redraw) 
{

    static int month_days;
                                       // If the sched_month_changed
                                       //  then write the month and
                                       //  year info
    if (sched_month_changed)
    {
        char year_str[10];

        this->SetDlgItemText
                       (IDC_MONTH_TEXT,
                        m_MonthInfo[m_ttShownMonth.tm_Ptr()->tm_mon].name);

	

        wsprintf(year_str,"%4d",(1900 + m_ttShownMonth.tm_Ptr()->tm_year));
        this->SetDlgItemText(IDC_YEAR_TEXT,year_str); //Y2K BUG??

        month_days = m_MonthInfo[m_ttShownMonth.tm_Ptr()->tm_mon].num_days;
        if ((m_ttShownMonth.tm_Ptr()->tm_mon == 1) && 
        (!(m_ttShownMonth.tm_Ptr()->tm_year % 4)))
        {
            month_days++;
        }
    }
                                       // If the selected_entry_changed
                                       //  then have to set the hour
                                       //  and min and the schedule type

    if (selected_entry_changed)
    {
        char tmp_str[3];
                                       // If haven't specified time
                                       //  or date then give default time
                                       //  of 23:59 & 1-shot sched type
                                       //

        if (m_CurScheduleEntry.tt()->IsClear())
        {
            m_nHour = 23;
            m_nMinute = 59;
			
            m_ctlSchedType.SetCurSel(ONE_SHOT_SCHED_ID);
        }
                                       // Else use set values
        else
        {
			m_nMinute = m_CurScheduleEntry.tt()->tm_Ptr()->tm_min;
            m_nHour = m_CurScheduleEntry.tt()->tm_Ptr()->tm_hour;

            m_ctlSchedType.SetCurSel(m_CurScheduleEntry.Type());
        }

        sprintf(tmp_str,"%02d",m_nHour);

        CWnd *tmpwnd;
        tmpwnd = GetDlgItem(IDC_HOUR_VALUE);
        tmpwnd->SetWindowText(tmp_str);

        sprintf(tmp_str,"%02d",m_nMinute);

        tmpwnd = GetDlgItem(IDC_MINUTE_VALUE);
        tmpwnd->SetWindowText(tmp_str);
    }

                                       // Now have to draw the calendar
                                       //  day buttons
    int day_cnt = 1;

    for (int i=0; (i<NUM_BUTTONS); i++) 
    {
                                       // This if makes sure that this
                                       //  button represents a valid
                                       //  day for the currently shown
                                       //  month (we have 42 buttons,
                                       //  only 28..31 used at any one
                                       //  time)
                                       //
        if ((i >= m_ttShownMonth.tm_Ptr()->tm_wday) &&
            (day_cnt <= month_days)) 
        {

                                       // Only once when we first change
                                       //  to a new month do we have
                                       //  to enable/show button
            if (sched_month_changed)
            {
                m_DayButtons[i].EnableWindow(TRUE);
                m_DayButtons[i].ShowWindow(SW_SHOW);
                m_DayButtons[i].m_bShowIt =  TRUE;
                m_DayButtons[i].m_nDayVal =  day_cnt;
            }
                                       // If force_daybutton_redraw need
                                       //  to turn off selection now,
                                       //  ChangeDayButtonSelection()
                                       //  will reset it
            if (force_daybutton_redraw)
                m_DayButtons[i].m_bSelected = FALSE;

                                       // Now call ChangeDayButtonSelection
                                       //  to check on selection and
                                       //  indicate whether we need to
                                       //  redraw button (if
                                       //  sched_month_changed or
                                       //  force_daybutton_redraw then 
                                       //  will redraw button regardless)
                                       //
            if ( (ChangeDayButtonSelection(i)) ||
                 (sched_month_changed) ||
                 (force_daybutton_redraw) )
            {
                                       // Does button redraw

                ::InvalidateRect(m_DayButtons[i].GetSafeHwnd(),
                                 NULL,FALSE);
            }
            day_cnt++;
        } 
                                       // Again, only once when we first
                                       //  change to a new month do we
                                       //  worry about this - here we
                                       //  disable buttons not currently
                                       //  being used
        else if (sched_month_changed)
        {
            m_DayButtons[i].m_bShowIt = FALSE;

            m_DayButtons[i].EnableWindow(FALSE);
            m_DayButtons[i].ShowWindow(SW_HIDE);
        }
    }
}



/////////////////////////////////////////////////////////////////////////////
// DrawScheduleEntryList
// _____________________

void CScheduleDlg::DrawScheduleEntryList(int start_at_pos,
                                         int cur_sel_pos,
                                         BOOL first_time)
{
    char entry_str_buf[SIZE_SMALL_GENBUF];

    for (int i=start_at_pos; i < m_TheSchedule.ScheduleEntryCnt(); i++)
    {
        if (!first_time)
            m_ctlSchedEntryList.DeleteString(i);

        m_ctlSchedEntryList.InsertString
                               (i,
                                m_TheSchedule[i]->PrettyScheduleEntry());
    }

                                       // Here get rid of any extras
                                       //
    while (m_ctlSchedEntryList.GetCount() >
           m_TheSchedule.ScheduleEntryCnt())
    {
        m_ctlSchedEntryList.DeleteString
                                    (m_TheSchedule.ScheduleEntryCnt());
    }
                                       // One 'New Entry' if haven't
                                       //  maxed out
                                       //
    if ( (!m_bReadOnly) &&
         (m_ctlSchedEntryList.GetCount() <
          m_TheSchedule.MaxScheduleEntries()) )
    {
        sprintf(entry_str_buf,"New Entry");
        m_ctlSchedEntryList.InsertString 
                                    (m_TheSchedule.ScheduleEntryCnt(),
                                     entry_str_buf);
    }

    m_ctlSchedEntryList.SetCurSel(m_nCurEntryIdx);
}



/////////////////////////////////////////////////////////////////////////////
// ErrorExit
// _________
//
// Called when irrecoverable error has occurred.
//
void CScheduleDlg::ErrorExit(char *err_msg_info) 
{
    CString msg = "Scheduler Error has occurred:\n\t(";
    msg += err_msg_info;
    msg += ")\nScheduler dialog will now cancel...";

    AfxMessageBox(msg, MB_OK);

    OnCancel();
}



/////////////////////////////////////////////////////////////////////////////
// OnCancel
// ________

void CScheduleDlg::OnCancel() 
{
    // m_bDidOk = FALSE; was done in OnInitDialog 

    CDialog::OnCancel();
}



/////////////////////////////////////////////////////////////////////////////
// OnCommand
// _________

BOOL CScheduleDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
                                       // Catch the calendar day
                                       //  buttons when pressed
                                       //
    if ( (IDC_DAY01 <= LOWORD(wParam)) && (LOWORD(wParam) <=IDC_DAY42) )
    {
        CalButtonPressed(LOWORD(wParam));
        return(TRUE);
    }
                                       // Catch the weekday buttons
                                       //  when pressed
                                       //
    else if ( (IDC_HEADING_SUN <= LOWORD(wParam)) && 
              (LOWORD(wParam) <= IDC_HEADING_SAT) )
    {
        WdayHeadButtonPressed(LOWORD(wParam));
        return(TRUE);
    }
                                       // Otherwise let pass through
    else
        return CDialog::OnCommand(wParam, lParam);
}



/////////////////////////////////////////////////////////////////////////////
// OnDeleteEntry
// _____________

void CScheduleDlg::OnDeleteEntry()               
{
    int old_idx = m_nCurEntryIdx;
    int new_idx;

    m_TheSchedule.DeleteEntry(m_nCurEntryIdx);

                                       // If old_idx is valid after 
                                       //  delete then is it will 
                                       //  become current
                                       //
    if (old_idx < m_TheSchedule.ScheduleEntryCnt())
    {
        new_idx = old_idx;
    }
                                       // If deleted the last one then
                                       //  have to back up one
                                       //
    else if ( ((old_idx-1) >= 0) &&
              ((old_idx-1) 
               < m_TheSchedule.ScheduleEntryCnt()) )
    {
        new_idx = m_nCurEntryIdx-1;
    }
                                       // Else if no more entries then
                                       //  current will be 'new entry'
    else
    {
        new_idx = 0;
    }

    SetCurrentEntry(new_idx);

    DrawScheduleEntryList(old_idx);

    DrawCalendar(TRUE, TRUE);
}




/////////////////////////////////////////////////////////////////////////////
// OnDiscardEntryChanges
// _____________________

void CScheduleDlg::OnDiscardEntryChanges() 
{
                                       // Let SetCurrentEntry() handle
                                       //
    SetCurrentEntry(m_nCurEntryIdx);
}



/////////////////////////////////////////////////////////////////////////////
// OnDone
// ______

void CScheduleDlg::OnDone() 
{
    if (!m_bReadOnly)
    {
    
        BOOL save_current = FALSE;
                                       // If any changes are outstanding
                                       //  then see if want to save 1st
        if (m_bEntryChanged)
        {

            int resp = AfxMessageBox("Save changes to current Schedule Entry?",
                                     MB_YESNOCANCEL);

            if (resp == IDCANCEL)
                return;
                
            else if (resp = IDYES)                                    
                save_current = TRUE;
        }
                                       // Now save if need be
        if (save_current)
            OnSaveEntry();
                                      // Make sure at least one entry

        if (m_TheSchedule.ScheduleEntryCnt() == 0)
        {
            AfxMessageBox("Select schedule or press Cancel to quit.", MB_OK);
            return;
        }

        if ((m_TheSchedule.ScheduleEntryCnt() - 
             m_TheSchedule.ScheduleEntryExceptCnt()) == 0)
        {
            AfxMessageBox("Schedule must have at least one entry that is not an Exception.\nPlease modify or press Cancel to quit.", MB_OK);
            return;
        }
    }

    m_bDidOk = TRUE;

    CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// OnHelp
// ______

void CScheduleDlg::OnHelp() 
{
AfxMessageBox(HELP_COMING_SOON_MSG, MB_OK);
}



/////////////////////////////////////////////////////////////////////////////
// OnInitDialog
// ____________

BOOL CScheduleDlg::OnInitDialog() 
{

    if ((m_bReadOnly) && (m_TheSchedule.ScheduleEntryCnt() == 0))
        return(FALSE);

                                       // Must do a Reset() or Clear()
                                   //  between DoModal() calls
    if (!m_bReadyForInit)
        return(FALSE);

    CDialog::OnInitDialog();
    centerScreen(m_hWnd);
                                       // Subclass the day buttons
                                       //
    for (int i=0; i < NUM_BUTTONS; i++)
        VERIFY(m_DayButtons[i].SubclassDlgItem((IDC_DAY01+i), this));

                                       // Set up the sched types
                                       //
    m_ctlSchedType.AddString("One Shot");
    m_ctlSchedType.AddString("Daily");
    m_ctlSchedType.AddString("Weekly");
    m_ctlSchedType.AddString("Monthly");
    m_ctlSchedType.AddString("Hourly");
    m_ctlSchedType.AddString("Yearly");
    m_ctlSchedType.AddString("Exception");

                                       // Setup 0th entry as currently
                                       //  selected (will be either 1st
                                       //  existing entry or 'new entry')
    SetCurrentEntry(0);
                                       // Draw any existing entries
                                       //  for m_TheSchedule
    DrawScheduleEntryList(0, 0, TRUE);

                                       // Initially mark entry as not
                                       //  changed, setting m_bEntryChanged
                                       //  so SetEntryHasChanged will take
                                       //  action (otherwise does nothing)
    m_bEntryChanged = TRUE;          
    SetEntryHasChanged(FALSE);



	struct tm *newtime;
	long ltime;
	time( &ltime );
   /* Obtain coordinated universal time: */   
	newtime = gmtime( &ltime );

	CString temp=asctime( newtime );
   	
	temp.Format("NTCSS II Schedule Selection     %s (UTC)",temp.Left
		                                       (temp.GetLength()-1));
	/******************************************/
	
	/////////////////////////////////
	//m_Time = COleDateTime::GetCurrentTime();//jj
	//CString strLine = m_Time.Format("%b %d %y %a %X");
	this->SetWindowText(temp);
//	this->SetWindowText("NTCSS II Schedule Selection      " + strLine);
	//UpdateData(FALSE);//jj
	SetTimer(1,1000,NULL);//jj

    if (m_bReadOnly)
    {
        CWnd *tmp_wnd = GetDlgItem(IDC_HOUR_SCROLL);
        tmp_wnd->EnableWindow(FALSE);
                                                    
        tmp_wnd = GetDlgItem(IDC_MIN_SCROLL);
        tmp_wnd->EnableWindow(FALSE);

        tmp_wnd = GetDlgItem(IDC_SCHEDULE_TYPE);
        tmp_wnd->EnableWindow(FALSE);

        tmp_wnd = GetDlgItem(IDC_DELETE_ENTRY);
        tmp_wnd->EnableWindow(FALSE);

        tmp_wnd = GetDlgItem(IDCANCEL);      
        tmp_wnd->EnableWindow(FALSE);
        tmp_wnd->ShowWindow(SW_HIDE);
    }
                                       // Init as NOT okay, OnDone
                                       //  resets this
    m_bDidOk = FALSE;

                                       // Must do a Clear or Reset
                                       //  before OnInitDialog is done
                                       //  again
    m_bReadyForInit = FALSE;


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}



/////////////////////////////////////////////////////////////////////////////
// OnMonthDown
// ___________

void CScheduleDlg::OnMonthDown() 
{
                                       // Don't let drop month below
                                       //  January, LOW_YEAR
                                       //
    if ( (m_ttShownMonth.tm_PtrWrt()->tm_year == FIRST_YEAR_ALLOWED) &&
         (m_ttShownMonth.tm_Ptr()->tm_mon == 0) )
    {
        ::MessageBeep(BEEP_SOUND);
        return;
    }

    if (m_ttShownMonth.tm_Ptr()->tm_mon == 0)
    {
        m_ttShownMonth.tm_PtrWrt()->tm_mon = 11;
        (m_ttShownMonth.tm_PtrWrt()->tm_year)--;
    }
    else
        (m_ttShownMonth.tm_PtrWrt()->tm_mon)--;

    m_ttShownMonth.UpdateWith_tm_Val();

    DrawCalendar(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// OnMonthUp
// _________

void CScheduleDlg::OnMonthUp() 
{
    if (m_ttShownMonth.tm_Ptr()->tm_mon == 11)
    {
        m_ttShownMonth.tm_PtrWrt()->tm_mon = 0;
        (m_ttShownMonth.tm_PtrWrt()->tm_year)++;
    }
    else
        (m_ttShownMonth.tm_PtrWrt()->tm_mon)++;

    m_ttShownMonth.UpdateWith_tm_Val();

    DrawCalendar(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// OnSaveEntry
// ___________

void CScheduleDlg::OnSaveEntry() 
{
    if (m_CurScheduleEntry.tt()->IsClear())
    {
        AfxMessageBox("Please select schedule date before saving.", MB_OK);
        return;
    }

    int new_pos;
    int set_to_pos;

    int old_pos = m_nCurEntryIdx;

                                       // First retrieve hour/min data
                                       //  and update m_CurScheduleEntry
                                       //  time value
    UpdateData(TRUE);
    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_hour = m_nHour;
    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_min = m_nMinute;


    //m_CurScheduleEntry.ttWrt()->UpdateWith_tm_Val(); jgj 04/21/97
	
	                                    // If working on 'new entry',
                                       //  do schedule AddEntry
    if (WorkingOnNewEntry())
    {
        new_pos = m_TheSchedule.AddEntry(&m_CurScheduleEntry);

                                       // Make sure did ok, if not then
                                       //  must abort
        if (new_pos < 0)
        {
            char err_buf[MAX_ERROR_MSG_LEN];

            m_TheSchedule.GetErrorMsg(err_buf);
            ErrorExit(err_buf);
        }
                                       // If not full then set entry
                                       //  selection to 'new entry',
                                       //  else go back to entry 0
                                       //
        if (m_TheSchedule.ScheduleEntryCnt() <
            m_TheSchedule.MaxScheduleEntries())

            set_to_pos = m_TheSchedule.ScheduleEntryCnt();

        else
            set_to_pos = 0;
    }
                                       // Else an existing entry has
                                       //  been changed, must do a
                                       //  ReplaceEntry
    else                               
    {
        new_pos = m_TheSchedule.ReplaceEntry(m_nCurEntryIdx,
                                           &m_CurScheduleEntry); 

                                       // Make sure did ok, if not then
                                       //  must abort
        if (new_pos < 0)
        {
            char err_buf[MAX_ERROR_MSG_LEN];

            m_TheSchedule.GetErrorMsg(err_buf);
            ErrorExit(err_buf);
        }
                                       // Here will set current entry
                                       //  to new position of modified
                                       //  entry
        set_to_pos = new_pos;
    }
                                       // Now SetCurrentEntry() and
                                       //  call SetEntryHasChanged() to 
                                       //  tell starting a fresh entry

    SetCurrentEntry(set_to_pos);

    DrawScheduleEntryList(((old_pos < new_pos) ? old_pos : new_pos),
                          set_to_pos);
}



/////////////////////////////////////////////////////////////////////////////
// OnSelchangeScheduleType
// _______________________

void CScheduleDlg::OnSelchangeScheduleType() 
{
    int old_schedule_type = m_CurScheduleEntry.Type();

    int new_schedule_type = m_ctlSchedType.GetCurSel();

    if (new_schedule_type == old_schedule_type)
        return;

    m_CurScheduleEntry.SetType(new_schedule_type);

    SetEntryHasChanged();

                                       // Since using different colors
                                       //  for normal and exceptions
                                       //  entries, if either old or
                                       //  new is exception then force
                                       //  button redraw
    BOOL force_button_redraw =
       ( (old_schedule_type == EXCEPT_SCHED_ID) ||
         (new_schedule_type == EXCEPT_SCHED_ID) );

                                       // DrawCalendar forcing buttons
                                       //  to be redrawn if need be
//AfxMessageBox("Ins selschedchange");                                       //
    DrawCalendar(FALSE, TRUE, force_button_redraw);
}




/////////////////////////////////////////////////////////////////////////////
// OnSelchangeEntryList
// ____________________

void CScheduleDlg::OnSelchangeEntryList() 
{
                                       // First, check if current entry
                                       //  has been changed and see
                                       //  about saving these changes
    if (m_bEntryChanged)
    {
        int resp = AfxMessageBox("Save changes to current Schedule Entry?",
                                 MB_YESNOCANCEL);
        if (resp == IDCANCEL)
        {
            SetCurrentEntry(m_nCurEntryIdx);
            return;
        }
                
        else if (resp == IDYES)                                    
            OnSaveEntry();
    }
                                       // Then let SetCurrentEntry()
                                       //  handle the rest
                                       //
    SetCurrentEntry(m_ctlSchedEntryList.GetCurSel());
}



/////////////////////////////////////////////////////////////////////////////
// OnVScroll
// _________
//
// Handling two scroll bars here - one for the schedule hour and one 
// for the schedule minute.
//

void CScheduleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    UpdateData(TRUE);

    BOOL change_minute = FALSE;
    BOOL change_hour = FALSE;

    if (pScrollBar->GetDlgCtrlID() == IDC_HOUR_SCROLL)
    {
        switch (nSBCode)
        {
            case SB_LINEUP:    
            case SB_PAGEUP:
                m_nHour++;
                break;
                    
            case SB_LINEDOWN:    
            case SB_PAGEDOWN:    
                m_nHour--;
                break;
        }
        change_hour = TRUE;

        if (m_nHour < 0)
            m_nHour = 23;
        else if (m_nHour > 23)
            m_nHour = 0;

    }
    else if (pScrollBar->GetDlgCtrlID() == IDC_MIN_SCROLL)
    {
        switch (nSBCode)
        {
            case SB_LINEUP:    
            case SB_PAGEUP:
                m_nMinute++;
                break;
                    
            case SB_LINEDOWN:    
            case SB_PAGEDOWN:    
                m_nMinute--;
                break;
        }
        if ((0 <= m_nMinute) && (m_nMinute <= 59))
        {
            change_minute = TRUE;
        }
        else if ((m_nMinute < 0) && (m_nHour != 0))
        {
            m_nMinute = 59;
            m_nHour--;
            change_hour = change_minute = TRUE;
        }
        else if ((m_nMinute > 59) && (m_nHour != 23))
        {
            m_nMinute = 0;
            m_nHour++;
            change_hour = change_minute = TRUE;
        }
        else if ((m_nMinute > 59) && (m_nHour == 23))
        {
            m_nMinute = 0;
            m_nHour = 0;
            change_hour = change_minute = TRUE;
        }
    }

    CWnd *tmpwnd;
    char tmp_str[3];

    if (change_hour)
    {
        sprintf(tmp_str,"%02d",m_nHour);

        tmpwnd = GetDlgItem(IDC_HOUR_VALUE);
        tmpwnd->SetWindowText(tmp_str);
    }
    if (change_minute)
    {
        sprintf(tmp_str,"%02d",m_nMinute);

        tmpwnd = GetDlgItem(IDC_MINUTE_VALUE);
        tmpwnd->SetWindowText(tmp_str);
    }

    if (m_CurScheduleEntry.tt()->IsClear())
    {
        m_CurScheduleEntry.ttWrt()->SetToNow();
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_hour = m_nHour;
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_min = m_nMinute;
        m_CurScheduleEntry.ttWrt()->UpdateWith_tm_Val();
    }

    SetEntryHasChanged(TRUE,TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// OnYearDown
// __________

void CScheduleDlg::OnYearDown() 
{
    if (m_ttShownMonth.tm_PtrWrt()->tm_year == FIRST_YEAR_ALLOWED) 
    {
        ::MessageBeep(BEEP_SOUND);
        return;
    }


    (m_ttShownMonth.tm_PtrWrt()->tm_year)--;

    m_ttShownMonth.UpdateWith_tm_Val();

    DrawCalendar(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// OnYearUp
// ________

void CScheduleDlg::OnYearUp() 
{

    (m_ttShownMonth.tm_PtrWrt()->tm_year)++;

    m_ttShownMonth.UpdateWith_tm_Val();

    DrawCalendar(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// Reset
// _____
//
// This function can be used between DoModal() calls to change the
// CSchedule that is being worked with.
//


BOOL CScheduleDlg::Reset(CSchedule *sched_ptr, BOOL read_only)
{
    m_bReadOnly = read_only;

    if ( (sched_ptr != NULL) &&
         (sched_ptr->IsValid()) )
    {
        m_TheSchedule = *sched_ptr;

        m_bReadyForInit = TRUE;

        return(TRUE);
    }
    else if (sched_ptr == NULL)
    {
        m_TheSchedule.Clear();

        m_bReadyForInit = TRUE;

        return(TRUE);
    }
    else
    {
        m_bReadyForInit = FALSE;

        return(FALSE);
    }
}



/////////////////////////////////////////////////////////////////////////////
// SetCurrentEntry
// _______________

void CScheduleDlg::SetCurrentEntry (int new_pos)
{
    m_nCurEntryIdx = new_pos;

    CWnd *tmp_wnd = GetDlgItem(IDC_DELETE_ENTRY);

    if (WorkingOnNewEntry() && 
        m_nCurEntryIdx < (m_TheSchedule.MaxScheduleEntries()))
    {
        m_CurScheduleEntry.ttWrt()->Clear();
        m_CurScheduleEntry.SetType(ONE_SHOT_SCHED_ID);

        m_ttShownMonth.SetToNow();
        m_ttShownMonth.tm_PtrWrt()->tm_mday = 1;
        m_ttShownMonth.UpdateWith_tm_Val();

        m_ctlSchedEntryList.SetCurSel(m_TheSchedule.ScheduleEntryCnt());

        tmp_wnd->EnableWindow(FALSE);
    }
    
    else
    {
        if (m_nCurEntryIdx == 
            (m_TheSchedule.MaxScheduleEntries()))
        {
            m_nCurEntryIdx = 0;
        }

        m_CurScheduleEntry = *(m_TheSchedule[m_nCurEntryIdx]);

        m_ttShownMonth = *(m_CurScheduleEntry.tt());
        m_ttShownMonth.tm_PtrWrt()->tm_mday = 1;
        m_ttShownMonth.UpdateWith_tm_Val();

        if (!m_bReadOnly)
            tmp_wnd->EnableWindow(TRUE);
    }

    SetEntryHasChanged(FALSE);

    DrawCalendar(TRUE, TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// SetEntryHasChanged
// __________________

void CScheduleDlg::SetEntryHasChanged (BOOL changes_made,
                                       BOOL draw_on_first_set)
{
                                       // No need to go on if no changes
                                       //  to m_bEntryChanged
    if (m_bEntryChanged == changes_made)
        return;

    m_bEntryChanged = changes_made;

                                        // If no day has been selected
                                        //  then we force the current
                                        //  day to be selected
                                        //
    if ( (m_bEntryChanged) && (m_CurScheduleEntry.tt()->IsClear()) )
    {
        m_ttShownMonth.SetToNow();
        m_ttShownMonth.tm_PtrWrt()->tm_mday = 1;
        m_ttShownMonth.UpdateWith_tm_Val();

        m_CurScheduleEntry.ttWrt()->SetToNow();

        UpdateData(TRUE);
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_hour = m_nHour;
        m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_min = m_nMinute;

        m_ttShownMonth.UpdateWith_tm_Val();

        if (draw_on_first_set) {
//AfxMessageBox("Set entry has changed call");
            DrawCalendar(TRUE, TRUE);
		}
    }

    if (m_bReadOnly)
        changes_made = FALSE;   
                                       // Along with setting flag also
                                       //  turn-on/off save/discard
                                       //  changes buttons
                                       //
    CWnd *tmp_wnd = GetDlgItem(IDC_SAVE_ENTRY);
    tmp_wnd->EnableWindow(changes_made);

    tmp_wnd = GetDlgItem(IDC_DISCARD_ENTRY_CHANGES);
    tmp_wnd->EnableWindow(changes_made);
}



/////////////////////////////////////////////////////////////////////////////
// TheSchedule
// ___________

CSchedule CScheduleDlg::TheSchedule()
{
    if (m_bDidOk)
    {
        return (m_TheSchedule);
    }
                                       // If didn't do okay then will
                                       //  get an empty CSchedule back
    else
        return(CSchedule());
}



/////////////////////////////////////////////////////////////////////////////
// WdayHeadButtonPressed
// _____________________
//
// This function called if one of the Weekday buttons (SUN,MON,...)
// are pressed.
//
void CScheduleDlg::WdayHeadButtonPressed(int button_id)
{
    if (m_bReadOnly)
        return;

    m_CurScheduleEntry.SetType(WEEKLY_SCHED_ID);
    if (m_ctlSchedType.GetCurSel() != WEEKLY_SCHED_ID)
        m_ctlSchedType.SetCurSel(WEEKLY_SCHED_ID);

    if (m_CurScheduleEntry.tt()->IsClear())

        m_CurScheduleEntry.ttWrt()->SetToNow();

    int day_val = button_id - IDC_HEADING_SUN;

    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_mon = 
                                m_ttShownMonth.tm_Ptr()->tm_mon;

    m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_year = 
                                m_ttShownMonth.tm_Ptr()->tm_year;

                                       // Here use m_DayButtons setting
                                       //  to determine actual first
                                       //  weekday of month (e.g. first
                                       //  WED) to use for selected date
                                       //
    for (int idx=0; idx < 3; idx++)
    {
        if (m_DayButtons[((idx*7) + day_val)].m_bShowIt)
        {
            m_CurScheduleEntry.ttWrt()->tm_PtrWrt()->tm_mday = 
                    m_DayButtons[((idx*7) + day_val)].m_nDayVal;
            break;
        }
    }


    m_CurScheduleEntry.ttWrt()->UpdateWith_tm_Val();

    SetEntryHasChanged();
    DrawCalendar();
}

void CScheduleDlg::OnTimer(UINT nIDEvent) 
{
	/******************************************/
	struct tm *newtime;
	long ltime;
	time( &ltime );
   /* Obtain coordinated universal time: */   
	newtime = gmtime( &ltime );
    
  	CString temp=asctime(newtime);
   	temp.Format("NTCSS II Schedule Selection     %s (UTC)",temp.
		                               Left(temp.GetLength()-1));
	/******************************************/
	
	/////////////////////////////////
	//m_Time = COleDateTime::GetCurrentTime();//jj
	//CString strLine = m_Time.Format("%b %d %y %a %X");
	this->SetWindowText(temp);
	
	CDialog::OnTimer(nIDEvent);
}
