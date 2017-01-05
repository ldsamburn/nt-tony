/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NtcssMessage.h: interface for the NtcssMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NTCSSMESSAGE_H__9F250041_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_)
#define AFX_NTCSSMESSAGE_H__9F250041_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class NtcssMessage_class  
{
public:
	NtcssMessage_class();
	virtual ~NtcssMessage_class();

    void open(LPCTSTR, int, LPCTSTR, LPCTSTR, LPCTSTR, int, int);               
    void write(LPCTSTR, int);
    void read();
    void close();
//    void associate();
//    bool timed_out();

    LPTSTR response;

private:  
    void free_response();
    void get_server_error();
    void figure_response_size();
    void allocate_response(int);
    bool received_null();

    LPCTSTR accept_label;
    LPCTSTR reject_label;
    int response_size;    
    LPTSTR response_ptr;
    bool response_allocated;
};

extern NtcssMessage_class NtcssMessage;

#endif // !defined(AFX_NTCSSMESSAGE_H__9F250041_09C2_11D6_B4ED_00C04F4D4DED__INCLUDED_)
