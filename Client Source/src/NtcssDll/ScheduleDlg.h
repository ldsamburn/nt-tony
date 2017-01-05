/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// ScheduleDlg.h                    
//
// This is the header file for both the CDayButton and the CScheduleDlg
// classes.
//
//----------------------------------------------------------------------


                                       // Make sure have stdafx.h
#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_SCHDDLG_H_CLASS_
#define _INCLUDE_SCHDDLG_H_CLASS_
		
#include "resource.h"
#include "schedule.h"
                                       // Number of CDayButton's
#define NUM_BUTTONS     42             
                                       // Codes to tell how to color
                                       //  buttons
#define PLAIN_COLOR_CODE        0
#define RED_COLOR_CODE          1
#define WHITE_COLOR_CODE        2



#ifndef _INCLUDE_DAYBTN_H_CLASS_
#define _INCLUDE_DAYBTN_H_CLASS_
		

#define BLUE_BACKGROUND_COLOR       RGB(0,0,225)
#define SHADOW_COLOR                RGB(88,100,225)
#define RED_FOREGROUND_COLOR        RGB(255,0,0)
#define WHITE_FOREGROUND_COLOR      RGB(255,255,255)




/////////////////////////////////////////////////////////////////////////////
// CDayButton
// __________

class CDayButton : public CButton
{
// Constructors
public:
	CDayButton();

// Attributes
public:
    BOOL m_bShowIt;
    BOOL m_bSelected;
    int  m_nColorCode;
    int  m_nDayVal;

// Operations

// Overrides
protected:
    void DrawOuterFrame(CDC *pDC, CRect& rect, BOOL bThick);

	//{{AFX_VIRTUAL(CDayButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation - destructor
public:
	virtual ~CDayButton() {};

// Implementation - protected data
protected:

// Implementation - message mapping

protected:

	//{{AFX_MSG(CDayButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // _INCLUDE_DAYBTN_H_CLASS_




/////////////////////////////////////////////////////////////////////////////
// CScheduleDlg
// ____________

class CScheduleDlg : public CDialog
{
// Constructors
public:
	CScheduleDlg(CSchedule *sched = NULL, BOOL read_only=FALSE,  
                 CWnd* pParent = NULL);

// Attributes

// Operations
public:
    BOOL Reset(CSchedule *sched=NULL, BOOL read_only=FALSE);

    CSchedule TheSchedule();

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation - protected data
protected:

    friend class CDayButton;

	BOOL m_bReadyForInit;
    BOOL m_bDidOk;					
    BOOL m_bEntryChanged;			
    BOOL m_bReadOnly;

    int  m_nCurEntryIdx; 

    SMonthInfo m_MonthInfo[12];

    CDayButton m_DayButtons[NUM_BUTTONS];

    CSchedule m_TheSchedule;

    CScheduleEntry m_CurScheduleEntry;

    CttTime m_ttShownMonth;


protected :
// Dialog Data
	//{{AFX_DATA(CScheduleDlg)
	enum { IDD = IDD_SCHEDULE_DLG };	   
	CListBox	m_ctlSchedEntryList;
	CComboBox	m_ctlSchedType;	  
	int		m_nHour;
	int		m_nMinute;
	//}}AFX_DATA

	//COleDateTime	m_Time;

// Implementation - internal functions
protected:
    BOOL DoingException()
        { return ( (m_CurScheduleEntry.IsValid()
                    ? (m_CurScheduleEntry.Type() 
                       == EXCEPT_SCHED_ID)
                    : FALSE) ); };

    void CalButtonPressed(int button_id);

    BOOL ChangeDayButtonSelection(int button_idx);

    void DrawCalendar(BOOL sched_month_changed    = FALSE,
                      BOOL selected_entry_changed = FALSE,
                      BOOL force_daybutton_redraw = FALSE);

    void DrawScheduleEntryList(int start_at_pos = 0,
                               int cur_sel_pos = 0,
                               BOOL first_time = FALSE);

    void ErrorExit(char *err_msg);

    BOOL SelectedDateIsAfter(int year, int month, int day)
    {
        if ( m_CurScheduleEntry.tt()->IsClear() )
            return(FALSE);

        if ( (year < m_CurScheduleEntry.tt()->tm_Ptr()->tm_year) ||
             ((year == m_CurScheduleEntry.tt()->tm_Ptr()->tm_year) &&
              (month < m_CurScheduleEntry.tt()->tm_Ptr()->tm_mon)) ||
             ((year == m_CurScheduleEntry.tt()->tm_Ptr()->tm_year) &&
              (month == m_CurScheduleEntry.tt()->tm_Ptr()->tm_mon) &&
              (day < m_CurScheduleEntry.tt()->tm_Ptr()->tm_mday)) )

            return(TRUE);
        else
            return(FALSE);
    };

    void SetCurrentEntry (int new_pos);

    void SetEntryHasChanged (BOOL changes_made = TRUE,
                             BOOL draw_on_first_set = FALSE);

    void WdayHeadButtonPressed(int button_id);

    BOOL WorkingOnNewEntry()
    {
        return (m_nCurEntryIdx == m_TheSchedule.ScheduleEntryCnt());
    };


// Implementation - message mapping
protected:
	// Generated message map functions
	//{{AFX_MSG(CScheduleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMonthDown();
	afx_msg void OnMonthUp();
	afx_msg void OnYearDown();
	afx_msg void OnYearUp();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCancel();
	afx_msg void OnDone();
	afx_msg void OnSelchangeScheduleType();
	afx_msg void OnDeleteEntry();
	afx_msg void OnDiscardEntryChanges();
	afx_msg void OnSelchangeEntryList();
	afx_msg void OnSaveEntry();
	afx_msg void OnHelp();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _INCLUDE_SCHDDLG_H_CLASS_
