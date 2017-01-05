/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\ttTime.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the CttTime class, describe in more
// detail below.  Note that there is no corresponding ttTime.cpp, all 
// member functions defined in this header file.
//
//----------------------------------------------------------------------


#ifndef _INCLUDE_TTTIME_H_CLASS_
#define _INCLUDE_TTTIME_H_CLASS_
		

/////////////////////////////////////////////////////////////////////////////
// The CttTime
// ____________
// 
// This class contains two representations for time - a time_t value,
// m_time_t_val, and a struct tm value, m_tm_val.  Member functions are
// provided to access and change these values.  Simultaneous setting of
// m_time_t_val and m_tm_val is done at construction and by functions 
// Clear(), SetToNow(), UpdateWith_tm_Val(), and UpdateWith_time_t_Val().
// Direct write access to m_tm_val is allowed by function tm_PtrWrt(),
// but a call should be made to UpdateWith_tm_Val() after changes so that
// the m_time_t_val can be put in sync with m_tm_val.
// 
// Note that a 'clear' state is maintained for CttTime and is represented
// internally by m_time_t_val having a value of 0.
// 
// Note that the less than operator (<) uses only the m_time_t_val for
// its comparison.
// 

class CttTime
{
private:
    time_t m_time_t_val;
    struct tm m_tm_val;

    void Reset(BOOL use_current_time=FALSE, BOOL use_tm_val=TRUE,
               BOOL clear=FALSE)
    {
                                       // If use_current_time then
                                       //  call system time function
        if (use_current_time)
            time(&m_time_t_val);
                                       // If use_tm_val then use system
                                       //  mktime function to get
                                       //  m_time_t_val in sync with
                                       //  m_tm_val
        else if (use_tm_val)
		{
									   // For some screwy reason mktime
									   //  sometimes messes up the
									   //  tm_mday.  Setting these
									   //  values corrects problem and is
									   //  ok since only basing on the
									   //  year, month and month day
			m_tm_val.tm_wday = -1;
			m_tm_val.tm_sec = 0;
		//	m_tm_val.tm_min = 0;       //jgj fixed problem on 4/29
		//	m_tm_val.tm_hour = 0;
			m_tm_val.tm_wday = -1;
			m_tm_val.tm_yday = -1;
			m_tm_val.tm_isdst = -1;

            m_time_t_val = mktime(&m_tm_val);
		}

                                       // If clear then just set
                                       //  m_time_t_val to 0
        else if (clear)
            m_time_t_val = 0;
    
        memcpy(&m_tm_val,localtime(&m_time_t_val),sizeof(struct tm));
    };

public:
                                       // Constructor, depending on
                                       //  parm either clears or sets
                                       //  to current time
                                       //
    CttTime(const BOOL use_current_time = TRUE) 
    {
        if (use_current_time)
            SetToNow();
        else 
            Clear();
    };
                                       // Copy constructor
    CttTime(const CttTime& src)
    {
        m_time_t_val = src.m_time_t_val;
        memcpy(&m_tm_val,&(src.m_tm_val),sizeof(struct tm));
    };

                                       // Destructor
    ~CttTime() {};
                                       // Assignment operator
                                       //
    CttTime& operator=(const CttTime& src)
    {
        m_time_t_val = src.m_time_t_val;
        memcpy(&m_tm_val,&(src.m_tm_val),sizeof(struct tm));
        return (*this);
    };
                                       // Less than operator
                                       //
    BOOL operator<(const CttTime& src)
        { return(m_time_t_val < src.m_time_t_val); };

                                       // Clears time value
    void Clear()
        { Reset(FALSE, FALSE, TRUE); };

                                       // Returns TRUE if time value
                                       //  has been cleared
    BOOL IsClear() const
        { return(m_time_t_val == 0); };

    void SetToNow()                    // Sets to current time
        { Reset(TRUE); };
                                       // Returns m_time_t_val
    const time_t time_t_Val() const
        { return (m_time_t_val); };
                                       // Gives ptr to m_tm_val
    const struct tm *tm_Ptr() const
        { return ( &m_tm_val); };
                                       // Gives write access ptr to 
                                       //   m_tm_val
    struct tm *tm_PtrWrt()
        { return ( &m_tm_val); };
                                       // Sets m_time_t_val to given
                                       //  value
                                       //
    void UpdateWith_time_t_Val(time_t value)
    {
        m_time_t_val = value;
        Reset();
    };
                                       // Uses current setting of
                                       //  m_tm_val to update 
                                       //  m_time_t_val
    void UpdateWith_tm_Val()
        { Reset(); };
};

#endif // _INCLUDE_TTTIME_H_CLASS_

