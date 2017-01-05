/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Schedule.h                    
//
// This is the header file for both the CScheduleEntry and CSchedule
// classes, along with related definitions.  These classes used 
// to provide functionality useful for maintaining schedules.  Currently
// the classes are limited in capability to what is provided on the
// NTCSS server side.  However, this capabilities could be extended for
// wider applicability.
//
// NEEDTODO -
//	- probably want to move this back into COMN_LIB at some point.
//
//----------------------------------------------------------------------


#ifndef __AFXWIN_H__
    #error include "stdafx.h" before including this file
#endif

#ifndef _INCLUDE_SCHEDULE_H_CLASS_
#define _INCLUDE_SCHEDULE_H_CLASS_
		
#include "tttime.h"

                                       // Limited number of schedule
                                       //  entries
#define MAX_SCHEDULE_ENTRIES 12
 
                                       // Not allowing dates prior
                                       //  to January 1971.
#define FIRST_YEAR_ALLOWED  71
                                       // Codes used to identify 
                                       //  schedule problems
#define NO_ERRORS            0
#define MEM_ALLOC_ERROR      1
#define RAW_SCHED_ERROR      2
#define BAD_COPY_ERROR       3
#define BAD_INSERT_ERROR     4
#define IS_FULL_ERROR        5
                                       // SMonthInfo used for month
                                       //  abbrevs and day counts
typedef struct SMonthInfo
{
    char name[4];
    int  num_days;
} SMonthInfo;
                                       // These are int codes for the 
                                       //  types of schedule entries
#define ONE_SHOT_SCHED_ID 0
#define DAILY_SCHED_ID    1
#define WEEKLY_SCHED_ID   2
#define MONTHLY_SCHED_ID  3
#define HOURLY_SCHED_ID   4
#define YEARLY_SCHED_ID   5
#define EXCEPT_SCHED_ID   6
                                       // These are char codes for the 
                                       //  types of schedule entries
#define ONE_SHOT_SCHED_CODE     'O'
#define DAILY_SCHED_CODE        'D'
#define WEEKLY_SCHED_CODE       'W'
#define MONTHLY_SCHED_CODE      'M'
#define HOURLY_SCHED_CODE       'H'
#define YEARLY_SCHED_CODE       'Y'
#define EXCEPT_SCHED_CODE       'E'

                                       // Used to convert from schedule
                                       //  entry type int to char
                                       //
const char SCHEDULE_ID_TO_CODE_MATRIX[] = "ODWMHYE";

#define  MAX_PRETTY_ENTRY_LEN   30


/////////////////////////////////////////////////////////////////////////////
// CScheduleEntry
// ______________

class CScheduleEntry
{
// Constructors
public:
                                       // Default constructor, sets to
                                       //  one shot and clears m_ttTime
	CScheduleEntry()
    {
        m_bIsOk = TRUE; 
        m_iType = ONE_SHOT_SCHED_ID;
        m_ttTime.Clear();
    };
                                       // Construct from a raw ASCII
                                       //  schedule entry
                                       //
	CScheduleEntry(const char *raw_sched);

                                       // Copy constructor
                                       //
    CScheduleEntry(const CScheduleEntry& src)
    {
        m_ttTime          = src.m_ttTime;
        m_iType          = src.m_iType;
    };

// Attributes
public:

    BOOL IsClear() const
        { return(m_ttTime.IsClear()); };

                                       
    BOOL IsValid() const
        { return(m_bIsOk); };


                                       // Read only pointer to m_ttTime
    const CttTime *tt() const
        { return(&m_ttTime); };

                                       // Write access pointer to m_ttTime
    CttTime *ttWrt()
        { return(&m_ttTime); };

    int Type() const
        { return(m_iType); };

                                       // Sets to given schedule type
                                       //  but if invalid then makes
                                       //  it a one shot
    void SetType(int new_val)
    {
        if ( (ONE_SHOT_SCHED_ID <= new_val) &&
             (new_val <= EXCEPT_SCHED_ID) )

            m_iType = new_val;

        else
            m_iType = ONE_SHOT_SCHED_ID;             
    }

// Operations
public:
                                       // Assignment operator
                                       //
    CScheduleEntry& operator=(const CScheduleEntry& src)
    {
        m_bIsOk	  = TRUE; 
        m_ttTime    = src.m_ttTime;
        m_iType    = src.m_iType;

        return (*this);
    };
                                       // Less than operator, orders
                                       //  first by m_iType and
                                       //  then by m_ttTime
                                       //
    BOOL operator<(const CScheduleEntry& src)
    {
        if (m_iType < src.m_iType)
            return(TRUE);

        else if ( (m_iType == src.m_iType) &&
                  (m_ttTime < src.m_ttTime) )

            return(TRUE);

        else
            return(FALSE);
    };
                                       // Will write raw ASCII schedule
                                       //  entry to given address
                                       //
    char *GetRawScheduleEntry(char *target_ptr);

                                       // Returns string containing
                                       //  formatted entry info

    const char *PrettyScheduleEntry(); //{return(NULL);}; 



// Overrides

// Implementation - Destructor
public:
	~CScheduleEntry() {};

// Implementation - protected data
protected:
    BOOL     m_bIsOk;
    CttTime  m_ttTime;
    int      m_iType;

    static char *m_lpszPrettyTextStr;

};



/////////////////////////////////////////////////////////////////////////////
// CSchedule
// _________

class CSchedule
{
// Constructors
public:
                                       // Default constructor, starts
                                       //  off with 0 entries
	CSchedule()
        { m_nEntryCnt = 0; m_nErrorCode = NO_ERRORS; };

                                       // Construct from a raw ASCII
                                       //  schedule 
                                       //
	CSchedule(const char *raw_sched);  //{}; // HERE;
                                       // Copy constructor
    CSchedule(const CSchedule& src);  //{}; // HERE;

// Attributes

// Operations

                                       // Assignment operator
                                       //
    CSchedule& operator=(const CSchedule& src);  //{return(*this);}; // HERE;

                                       // Indexing operator, if ok
                                       //  returns read pointer to
                                       //  idx'th entry, otherwise NULL
                                       //
    const CScheduleEntry *operator[](int idx) const
    {
        return ( ((0 <= idx) && (idx < m_nEntryCnt)) 
                ? (m_EntryList[idx])
                : NULL);
    }
                                       // Indexing operator, if ok
                                       //  returns write pointer to
                                       //  idx'th entry, otherwise NULL
                                       //
    CScheduleEntry *operator[](int idx)
    {
        return ( ((0 <= idx) && (idx < m_nEntryCnt)) 
                ? (m_EntryList[idx])
                : NULL);
    }
                                       // Calls PutEntry to add a
                                       //  CScheduleEntry to the
                                       //  m_EntryList
                                       //
    int AddEntry(CScheduleEntry *new_entry_src,
                 BOOL must_alloc = TRUE)
    {
        return (PutEntry(new_entry_src, TRUE));
    };

                                       // Removes all CScheduleEntry
                                       //  from m_EntryList
	void Clear(); // {}; // HERE;

                                       // Removes pos'th CScheduleEntry
                                       //  from m_EntryList
    int DeleteEntry(int pos);  // {return(0); }; // HERE;
                                       // If !IsValid then will write
                                       //  text giving problem to
                                       //  err_msg_buf
                                       //
    BOOL GetErrorMsg(char *err_msg_buf) const; // {return(TRUE);}; // HERE;

                                       // Will write raw ASCII schedule
                                       //  to given address
                                       //
    char *GetRawSchedule(char *target_ptr); // {return(NULL);}; // HERE;


    BOOL IsValid() const
        { return (m_nErrorCode == NO_ERRORS); };   

                                       // Replaces pos'th CScheduleEntry
                                       //  with new_entry_src
                                       //
	BOOL ReplaceEntry(int pos, CScheduleEntry *new_entry_src); // {return(TRUE);}; // HERE;

                                       // Current CScheduleEntry cnt
    int ScheduleEntryCnt() const
        { return(m_nEntryCnt); };

    int ScheduleEntryExceptCnt() const; // {return(0);}; // HERE;

                                       // Max CScheduleEntry's allowed
    int MaxScheduleEntries() const
        { return(MAX_SCHEDULE_ENTRIES); };

// Overrides

// Implementation - destructor
public:
    ~CSchedule(); // {};

// Implementation - protected data
protected:
    int m_nEntryCnt;
    int m_nErrorCode;

    CScheduleEntry *m_EntryList[MAX_SCHEDULE_ENTRIES];

// Implementation - internal functions
protected:
                                       // Used to put a CScheduleEntry
                                       //  into m_EntryList
                                       //
    int PutEntry(CScheduleEntry *new_entry_src,
                 BOOL must_alloc); 

                                       // Used to get CScheduleEntry's
                                       //  in m_EntryList ordered
    void SortScheduleList();

};


#endif // _INCLUDE_SCHEDULE_H_CLASS_

