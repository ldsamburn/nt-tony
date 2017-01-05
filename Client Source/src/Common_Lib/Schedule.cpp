/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// Schedule.cpp
//                                         
// This is the implementation file for the CScheduleEntry and
// CSchedule classes.
//
//----------------------------------------------------------------------

#include "stdafx.h"

#include "schedule.h"
#include "sizes.h"
#include "genutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//***************************************************************************
//***************************************************************************
// CScheduleEntry  
// ______________
                                       

//
// Initialize static data
//

char *CScheduleEntry::m_lpszPrettyTextStr = NULL;


/////////////////////////////////////////////////////////////////////////////
// CScheduleEntry   Construct from a raw ASCII schedule entry
// ______________

CScheduleEntry::CScheduleEntry(const char *raw_sched)
{
    m_bIsOk = FALSE;

    const char *ch_ptr = raw_sched;

    m_ttTime.SetToNow();
    switch (*ch_ptr)
    {
        case ONE_SHOT_SCHED_CODE:
            m_iType = ONE_SHOT_SCHED_ID;
            break;

        case DAILY_SCHED_CODE:
            m_iType = DAILY_SCHED_ID;
            break;

        case WEEKLY_SCHED_CODE:
            m_iType = WEEKLY_SCHED_ID;
            break;

        case MONTHLY_SCHED_CODE:
            m_iType = MONTHLY_SCHED_ID;
            break;

        case HOURLY_SCHED_CODE:
            m_iType = HOURLY_SCHED_ID;
            break;

        case YEARLY_SCHED_CODE:
            m_iType = YEARLY_SCHED_ID;
            break;

        case EXCEPT_SCHED_CODE:
            m_iType = EXCEPT_SCHED_ID;
            break;

        default:
            return;
    }

    struct tm *time_ptr = m_ttTime.tm_PtrWrt();

    char tmp_buf[5];

                                       // Minute
    ch_ptr++;
    tmp_buf[0] = *ch_ptr++;
    tmp_buf[1] = *ch_ptr++;
    tmp_buf[2] = '\0';


    time_ptr->tm_min = atoi(tmp_buf);
 
    if ( (time_ptr->tm_min < 0) || 
         (59 < time_ptr->tm_min) )
    {
        return;
    }
                                     // Hour
    tmp_buf[0] = *ch_ptr++;
    tmp_buf[1] = *ch_ptr++;
    tmp_buf[2] = '\0';
 
    time_ptr->tm_hour = atoi(tmp_buf);

    if ( (time_ptr->tm_hour < 0) || 
         (23 < time_ptr->tm_hour) )
    {
        return;
    }


                                       // Weekday
    tmp_buf[0] = *ch_ptr++;
    tmp_buf[1] = '\0';
 
    time_ptr->tm_wday = atoi(tmp_buf);

    if ( (time_ptr->tm_wday < 0) || 
         (6 < time_ptr->tm_wday) )
    {
        return;
    }
                                       // Month day
    tmp_buf[0] = *ch_ptr++;
    tmp_buf[1] = *ch_ptr++;
    tmp_buf[2] = '\0';
  
    time_ptr->tm_mday = atoi(tmp_buf);

    if ( (time_ptr->tm_mday < 0) || 
         (31 < time_ptr->tm_mday) )
    {
        return;
    }
                                       // Month number
    tmp_buf[0] = *ch_ptr++;
    tmp_buf[1] = *ch_ptr++;
    tmp_buf[2] = '\0';
  

    time_ptr->tm_mon = atoi(tmp_buf);
                                       // Subtract one as struct tm has
                                       //  months go from 0 to 11
    time_ptr->tm_mon -= 1;

    if ( (time_ptr->tm_mon < 0) || 
         (12 < time_ptr->tm_mon) )
    {
        return;
    }

    m_ttTime.UpdateWith_tm_Val();

    m_bIsOk = TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// GetRawScheduleEntry
// ___________________

char *CScheduleEntry::GetRawScheduleEntry(char *target_ptr)
{
	sprintf(target_ptr, "%c%02d%02d%01d%02d%02d",
            SCHEDULE_ID_TO_CODE_MATRIX[m_iType],
            m_ttTime.tm_Ptr()->tm_min, m_ttTime.tm_Ptr()->tm_hour,
            m_ttTime.tm_Ptr()->tm_wday, m_ttTime.tm_Ptr()->tm_mday,

                                       // Add one to tm_mon as server has
                                       //  months go from 1 to 12
            (m_ttTime.tm_Ptr()->tm_mon+1));

    return(target_ptr);
}



/////////////////////////////////////////////////////////////////////////////
// PrettyScheduleEntry
// ___________________

const char *CScheduleEntry::PrettyScheduleEntry()
{
    static char *sched_type_str;

    if (m_lpszPrettyTextStr == NULL)
    {
        m_lpszPrettyTextStr = new char[MAX_PRETTY_ENTRY_LEN];

        if (m_lpszPrettyTextStr == NULL)
            return(NULL);
    }

    switch (m_iType)
    {
        case ONE_SHOT_SCHED_ID:
            sched_type_str = "One shot";
            break;

        case DAILY_SCHED_ID:
            sched_type_str = "Daily";
            break;

        case WEEKLY_SCHED_ID:
            sched_type_str = "Weekly";
            break;

        case HOURLY_SCHED_ID:
            sched_type_str = "Hourly";
            break;

        case MONTHLY_SCHED_ID:
            sched_type_str = "Monthly";
            break;

        case YEARLY_SCHED_ID:
            sched_type_str = "Yearly";
            break;

        case EXCEPT_SCHED_ID:
            sched_type_str = "Exception";
            break;
    }

    //Y2K bug here tm_year is 100 for 2000
    
    sprintf(m_lpszPrettyTextStr,"%s @ %d/%d/%02d %02d:%02d",
            sched_type_str,
            (m_ttTime.tm_Ptr()->tm_mon+1),
             m_ttTime.tm_Ptr()->tm_mday,
             m_ttTime.tm_Ptr()->tm_year<=99?m_ttTime.tm_Ptr()->tm_year:
			 m_ttTime.tm_Ptr()->tm_year-100, //Y2K hack breaks in Y3K
             m_ttTime.tm_Ptr()->tm_hour,
             m_ttTime.tm_Ptr()->tm_min);

    return(m_lpszPrettyTextStr);
}



//***************************************************************************
//***************************************************************************
// CSchedule
// _________

/////////////////////////////////////////////////////////////////////////////
// CSchedule  - Construct from a raw ASCII schedule entry
// __________

CSchedule::CSchedule(const char *raw_sched)
{
    const char *ch_ptr = raw_sched;
    m_nEntryCnt = 0;

	char lcl_raw_sched[SIZE_SCHEDULE];
	strcpy(lcl_raw_sched, raw_sched);
	trim(lcl_raw_sched);

	if ((ch_ptr == NULL) || (*ch_ptr == NULL))
	{
        m_nErrorCode = RAW_SCHED_ERROR;
        return;
	}
									   // Construct and add to
									   //  m_EntryList for each entry
									   //  in raw_sched
									   //

    while ((*ch_ptr) && (m_nEntryCnt < MAX_SCHEDULE_ENTRIES))
    {
        m_EntryList[m_nEntryCnt] = new CScheduleEntry(ch_ptr);

        if (m_EntryList[m_nEntryCnt] == NULL)
        {
            m_nErrorCode = MEM_ALLOC_ERROR;
            return;
        }

        if (!(m_EntryList[m_nEntryCnt]->IsValid()))
        {
            m_nErrorCode = RAW_SCHED_ERROR;
            return;
        }
        m_nEntryCnt++;
        ch_ptr += SIZE_SCHEDULE_ENTRY;
    }								   
									   // If get here then raw_sched was
									   //  good, 

    SortScheduleList();

    m_nErrorCode = NO_ERRORS;
}




/////////////////////////////////////////////////////////////////////////////
// CSchedule  - copy constructor
// __________

CSchedule::CSchedule(const CSchedule& src)
{
    for (int i=0; i<src.m_nEntryCnt; i++)
    {
        m_EntryList[i] = new CScheduleEntry(*(src.m_EntryList[i]));

        if (m_EntryList[i] == NULL)
        {
            m_nErrorCode = MEM_ALLOC_ERROR;
            return;
        }
    }

    m_nEntryCnt = src.m_nEntryCnt;
    m_nErrorCode = NO_ERRORS;
}




/////////////////////////////////////////////////////////////////////////////
// CSchedule  - destructor
// __________

CSchedule::~CSchedule()
{
    for (int i=0; i < m_nEntryCnt; i++)
    {
        if (m_EntryList[i] != NULL)
            delete m_EntryList[i];
    }   
}


/////////////////////////////////////////////////////////////////////////////
// CSchedule  - assignment operator
// __________

CSchedule& CSchedule::operator=(const CSchedule& src)
{
    if (!src.IsValid())
    {
        m_nErrorCode = BAD_COPY_ERROR;
        return(*this);
    }

    for (int i=0; i<src.m_nEntryCnt; i++)
    {
        m_EntryList[i] = new CScheduleEntry(*(src.m_EntryList[i]));

        if (m_EntryList[i] == NULL)
        {
            m_nErrorCode = MEM_ALLOC_ERROR;
            return(*this);
        }
        // Otherwise assume constructed ok and IsValid
    }

    m_nEntryCnt = src.m_nEntryCnt;
    m_nErrorCode = NO_ERRORS;

    return (*this);
}



/////////////////////////////////////////////////////////////////////////////
// Clear
// _____

void CSchedule::Clear()
{
    if (!IsValid())
        return;

    for (int i = 0; i<m_nEntryCnt ; i++)
    {
        delete m_EntryList[i];
    }
    m_nEntryCnt = 0;
}




/////////////////////////////////////////////////////////////////////////////
// DeleteEntry
// ___________

int CSchedule::DeleteEntry(int pos)
{
    if (!IsValid())
        return(-1);

    delete m_EntryList[pos];

    for (int i=pos; (i < (m_nEntryCnt-1)); i++)
        m_EntryList[i] = m_EntryList[i+1];

    m_nEntryCnt--;

    return(pos);
}



/////////////////////////////////////////////////////////////////////////////
// GetRawSchedule
// ______________

char *CSchedule::GetRawSchedule(char *target_ptr)
{
    char *ch_ptr = target_ptr;

    for (int i=0; i < m_nEntryCnt; i++)
    {
        m_EntryList[i]->GetRawScheduleEntry(ch_ptr);

        ch_ptr += SIZE_SCHEDULE_ENTRY;
    }
	*ch_ptr = '\0';

    return(target_ptr);
}



/////////////////////////////////////////////////////////////////////////////
// ErrorMsg
// ________

BOOL CSchedule::GetErrorMsg(char *err_buf) const
{
    if (m_nErrorCode == NO_ERRORS)
    {
        *err_buf = '\0';
        return(FALSE);
    }

    switch(m_nErrorCode)
    {
        case MEM_ALLOC_ERROR:
            strcpy(err_buf,"Memory allocation error");
            break;

        case RAW_SCHED_ERROR:
            strcpy(err_buf,"Initialized with invalid raw schedule");
            break;

        case BAD_COPY_ERROR:
            strcpy(err_buf,"Given invalid schedule to copy");
            break;

        case BAD_INSERT_ERROR:
            strcpy(err_buf,"Given invalid schedule entry to insert");
            break;

        case IS_FULL_ERROR:
            strcpy(err_buf,"Attempt to exceed schedule capacity");
            break;

        default:
            strcpy(err_buf,"Unknown error");
            break;
    }
    return(TRUE);
}


////////////////////////////////////////////////////////////////////////////
// PutEntry
// ________

int CSchedule::PutEntry(CScheduleEntry *new_entry_src,
                        BOOL must_alloc)
{
                                       // First validate
    if (!(new_entry_src->IsValid()))
    {
        m_nErrorCode = BAD_INSERT_ERROR;
        return(-1);
    }

    if ((m_nEntryCnt+1) > MAX_SCHEDULE_ENTRIES)
    {
        m_nErrorCode = IS_FULL_ERROR;
        return(-1);
    }

    CScheduleEntry *new_entry;
                                       // If need be then construct
                                       //  a new CScheduleEntry using
                                       //  the given source, otherwise
                                       //  just set ptr (this is when
                                       //  a replace is being done)
    if (must_alloc)
    {
        new_entry = new CScheduleEntry(*new_entry_src);
    }
    else
        new_entry = new_entry_src;
 
    if (new_entry == NULL)
    {
        m_nErrorCode = MEM_ALLOC_ERROR;
        return(-1);
    }

    BOOL found_spot = FALSE;
    int idx = 0;
    int new_idx;
    CScheduleEntry *this_one;
                                       // Now find where the new entry
                                       //  will be located
                                       //
    while ((idx < m_nEntryCnt) && (!found_spot))
    {
        this_one = m_EntryList[idx];

        if (*new_entry < *this_one)
        {
            found_spot = TRUE;
            new_idx = idx;
        }
        else
            idx++;
    }
                                        // If didn't find a spot then
                                        //  will go at end
    if (!found_spot)
    {
        m_EntryList[m_nEntryCnt] = new_entry;
        m_nEntryCnt++;
        return(m_nEntryCnt-1);
    }
                                        // Now make room for the new
                                        //  entry by moving ptrs in the
                                        //  m_EntryList
                                        //
    for (idx = m_nEntryCnt; idx != new_idx; idx--)

        m_EntryList[idx] = m_EntryList[idx - 1];

                                        // Now stick the new one in the
                                        //  list and bump the cntr
    m_EntryList[new_idx] = new_entry;
    m_nEntryCnt++;

    return(new_idx);
}



/////////////////////////////////////////////////////////////////////////////
// ReplaceEntry
// _____________

int CSchedule::ReplaceEntry(int pos, CScheduleEntry *new_entry_src)
{
    if (!(new_entry_src->IsValid()))
    {
        m_nErrorCode = BAD_INSERT_ERROR;
        return(-1);
    }
                                       // First get pointer to entry
                                       //  being replaced, then use .
                                       //  assignment operator to update
                                       //  it new_entry_src data
                                       //
    CScheduleEntry *entry_space = m_EntryList[pos];

    if (entry_space != new_entry_src)
        *entry_space = *new_entry_src;
                                       // Now will take the entry from
                                       //  the m_EntryList, by
                                       //  overwriting its ptr with the
                                       //  entries following it in list
                                       //
    for (int i=pos; i<(m_nEntryCnt-1); i++)
    {
        m_EntryList[i] = m_EntryList[i+1];

    }
                                       // NULLify the last entry and
                                       //  decrement cntr, have now
                                       //  removed entry 
    m_EntryList[i] = NULL;
    m_nEntryCnt--;
                                       // Now use PutEntry to put the
                                       //  new entry back into the list
                                       //  in its proper place
                                       //
    return(PutEntry(entry_space, FALSE));
}



/////////////////////////////////////////////////////////////////////////////
// ScheduleEntryExceptCnt
// ______________________

int CSchedule::ScheduleEntryExceptCnt() const
{
    int except_cnt = 0;
    for (int i=0; i < m_nEntryCnt; i++)
    {
        if (m_EntryList[i]->Type() == EXCEPT_SCHED_ID)
            except_cnt++;
    }
    return(except_cnt);
}



/////////////////////////////////////////////////////////////////////////////
// SortScheduleList
// ________________

void CSchedule::SortScheduleList()
{
    CScheduleEntry *tmp_sched_list[MAX_SCHEDULE_ENTRIES];

                                       // First copy ptrs from 
                                       //   m_EntryList to a tmp list
    int i;
    for (i=0; i<m_nEntryCnt; i++)
        tmp_sched_list[i] = m_EntryList[i];
    
    int    low_idx;

    CScheduleEntry *low_one;
    CScheduleEntry *this_one;

                                       // Outer loop looks at each
                                       //  entry, starting at the front
                                       //  of the list, each iteration
                                       //  gets one more entry in sorted
                                       //  position
                                       //
    for (int next_idx=0; next_idx<m_nEntryCnt; next_idx++)
    {
                                       // Inner loop finds the 'next'
                                       //  entry - the low_one
        low_one = NULL;
        for (i=0; i<m_nEntryCnt; i++)
        {
                                       // If tmp_sched_list[i] is NULL
                                       //  then have already handled it
                                       //
            if (tmp_sched_list[i] != NULL)
            {
                                       // If low_one is NULL then is
                                       //  first time thru for this value
                                       //  of next_idx
                if (low_one == NULL)
                {
                    low_one = tmp_sched_list[i];
                    low_idx = i;
                }
                                       // Else already have a low_one,
                                       //  now check if this_one is
                                       //  less than low_one, if so
                                       //  then this_one becomes low_one
                else
                {
                    this_one = tmp_sched_list[i];

                    if (*this_one < *low_one)
                    {
                        low_one = tmp_sched_list[i];
                        low_idx = i;
                    }
                }
            }
        }

        m_EntryList[next_idx] = tmp_sched_list[low_idx];
        tmp_sched_list[low_idx] = NULL;
    }
}

