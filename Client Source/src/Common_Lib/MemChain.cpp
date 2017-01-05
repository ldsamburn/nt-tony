/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\comn_lib\memchain.cpp
//
// WSP, last update: 9/16/96
//                                         
// This is the implementation file for the CMemChain routines.  
//
// See routine comments below and also the header file for details on 
// processing.
//
//----------------------------------------------------------------------


#include <stdarg.h>

#include <stdafx.h>

#include <ntcssdef.h>
#include <memchain.h>



///////////////////////////////////////////////////////////////////////////////
// CMemChain (constructor - no parms)
// __________________________________
/*
This is constructor when the CMemChain is to start off empty.
*/

CMemChain::CMemChain()
{
    m_head_ptr = NULL;
    m_external_head_ptrptr = NULL;
    m_tail_ptr = NULL;
    m_link_byteing_on_ptr = NULL;
    m_byteing_on_idx        = -1;
}



///////////////////////////////////////////////////////////////////////////////
// CMemChain 
// _________
//
// This is the constructor when the chain already exists in memory and
// this CMemChain is to be setup to work with it.

CMemChain::CMemChain(TMemChainLink *first_link_ptr)
{
    m_head_ptr = (SMemChainLinkRec *)first_link_ptr;
    m_external_head_ptrptr = NULL;
    Reset(first_link_ptr);
}



///////////////////////////////////////////////////////////////////////////////
// ~CMemChain (destructor)
// _______________________

CMemChain::~CMemChain()
{
    TrashChainLinks();
}



///////////////////////////////////////////////////////////////////////////////
// LinkData
// ________
//
// Given a pointer to a TMemChainLink, this function validates that the
// record is valid and then returns a pointer to the TMemChainLink's
// data (data_block_ptr).

char *CMemChain::LinkData(TMemChainLink *given_link_ptr)
{
    SMemChainLinkRec *current_link_ptr = 
                            (SMemChainLinkRec *)given_link_ptr;

    if ( (current_link_ptr == NULL) ||
         (current_link_ptr->validity_chk_value != 
          NTCSSCODE_VALITY_CHECK_VALUE) )

        return(NULL);

    return((char *)current_link_ptr->data_block_ptr);
}



///////////////////////////////////////////////////////////////////////////////
// NewLink
// _______
//
// Adds a new link onto the chain.  This entails 'new'ing size bytes, for
// the data, as well as a SMemChainLinkRec to manage the new link.

char *CMemChain::NewLink(int size)
{
    if ( (m_head_ptr != NULL) && (m_tail_ptr == NULL) )
        return(NULL);
                                       // Try to get size data bytes

    char *new_buf_ptr = new char[size];

    if (new_buf_ptr == NULL)
        return(NULL);
                                       // Now get new SMemChainLinkRec
                                       //
    SMemChainLinkRec *new_link_ptr = new SMemChainLinkRec;

    if (new_link_ptr == NULL)
    {
        delete [] new_buf_ptr;
        return(NULL);
    }

                                       // If chain was empty then new
                                       //  link becomes first link
    if (m_head_ptr == NULL)
    {
        m_head_ptr = m_tail_ptr = new_link_ptr;
        if (m_external_head_ptrptr != NULL)
            *m_external_head_ptrptr =  m_head_ptr;
    }
                                       // Otherwise stick new link at
                                       //  the end of the chain
    else
    {
        m_tail_ptr->next_ptr = new_link_ptr;  //jgj this was ==
        m_tail_ptr = new_link_ptr;
    }
    new_link_ptr->validity_chk_value = NTCSSCODE_VALITY_CHECK_VALUE;
    new_link_ptr->data_block_ptr     = new_buf_ptr;
    new_link_ptr->data_block_len     = size;
    new_link_ptr->next_ptr           = NULL;

    return(new_buf_ptr);
}




///////////////////////////////////////////////////////////////////////////////
// PrepareToByteChain
// __________________
//
// Sets CMemChain member variables in preparation for calls to
// TakeChainBytes.

BOOL CMemChain::PrepareToByteChain()
{

    if ( (m_head_ptr == NULL) ||
         (m_head_ptr->validity_chk_value 
          != NTCSSCODE_VALITY_CHECK_VALUE) )
    {
        return(FALSE);
    }
    else
    {
        m_link_byteing_on_ptr    = m_head_ptr;
        m_byteing_on_idx         = 0;

        return(TRUE);
    }
}




///////////////////////////////////////////////////////////////////////////////
// Reset
// _____
// 
// Takes pointer to the first link of a chain and sets up this CMemChain
// to handle it.  Can be used to bring an existing chain into this
// CMemChain or if for some reason this CMemChain has been altered and
// the head and tail pointers need to be Reset.

void  CMemChain::Reset(TMemChainLink *first_link_ptr)
{
    m_link_byteing_on_ptr = NULL;
    m_byteing_on_idx        = -1;
                                      // If parm is NULL then could be
                                      //   a new chain with no links 
                                      //   (m_head_ptr==NULL) or
                                      //   could be an already built
                                      //   chain (m_head_ptr!=NULL)
    if (first_link_ptr == NULL)
    {
        first_link_ptr = (TMemChainLink *)m_head_ptr;
    }
                                       // If first_link_ptr is still
                                       //   NULL then have no links 
                                       //   in chain
    if (first_link_ptr == NULL)
    {
        m_head_ptr = NULL;
        m_tail_ptr = NULL;
        if (m_external_head_ptrptr != NULL)
            *m_external_head_ptrptr = m_head_ptr;
    }
                                       // Otherwise have some links, now
                                       //   validate ok and find tail
    else
    {

        SMemChainLinkRec *current_link_ptr = 
                                (SMemChainLinkRec *)first_link_ptr;

                                       // Validate first link, if not
                                       //   ok then will make an empty
                                       //   chain
                                       //
        if (current_link_ptr->validity_chk_value 
            != NTCSSCODE_VALITY_CHECK_VALUE)
        {
            m_head_ptr = NULL;
            m_tail_ptr = NULL;
            if (m_external_head_ptrptr != NULL)
                *m_external_head_ptrptr = m_head_ptr;
            return;
        }
                                       // Otherwise run through chain
                                       //  and set head/tail pointers,
                                       //  if hit a bad one then stop
                                       //  at link prior to bad link
        else
        {
            m_head_ptr = current_link_ptr;

            if (m_external_head_ptrptr != NULL)
                *m_external_head_ptrptr = m_head_ptr;

            do
            {
                m_tail_ptr = current_link_ptr;

                current_link_ptr = current_link_ptr->next_ptr;

                if ( (current_link_ptr != NULL) &&
                     (current_link_ptr->validity_chk_value 
                      != NTCSSCODE_VALITY_CHECK_VALUE) )
                {
                    current_link_ptr = NULL;
                }
            } while (current_link_ptr != NULL);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// TakeChainBytes
// ______________
// 
// Function PrepareToByteChain should be called to setup for this routine.
// After PrepareToByteChain is called, then this routine can be
// called to progressively take bytes from the beginning of the chain
// and work towards the end of the chain.  After TakeChainBytes has been
// called, then the next call to TakeChainBytes will gets its bytes from
// where the previous TakeChainBytes left off.

int CMemChain::TakeChainBytes(char *receive_buf, int num_bytes_to_get)
{
    if (  (m_link_byteing_on_ptr == NULL) 
       || (m_link_byteing_on_ptr->validity_chk_value 
           != NTCSSCODE_VALITY_CHECK_VALUE)
       || (m_byteing_on_idx < 0) )  
    {
        return(RETURN_INT_ERROR);
    }

    if (num_bytes_to_get <= 0)
        return(0);

    int  bytes_still_needed = num_bytes_to_get;
    char *recv_buf_ptr = receive_buf;

    char *src_buf_ptr; /* jgj = (m_link_byteing_on_ptr->data_block_ptr + 
                         m_byteing_on_idx); */

    int  this_link_bytes_left;
    int  cnt;

    BOOL done = FALSE;
                                       // Loop until we have all
                                       //  num_bytes_to_get or until
                                       //  we hit the end
    while (!done)
    {
                                       // How many bytes left in current
                                       //  link?
                                       //
        src_buf_ptr = (m_link_byteing_on_ptr->data_block_ptr + 
                         m_byteing_on_idx); //jgj moved down here to fix the bug*/

        this_link_bytes_left = ( m_link_byteing_on_ptr->data_block_len
                                 - // minus
                                 m_byteing_on_idx /*+ 1 jgj*/);

                                       // cnt will be either the number
                                       //  still needed or all that's
                                       //  left in the current link
                                       //

        cnt = (this_link_bytes_left >= bytes_still_needed)
              ? bytes_still_needed
              : this_link_bytes_left;
               
                                       // Move the data to the recv buf
        for (int i=0; i < cnt; i++)
             *recv_buf_ptr++ = *src_buf_ptr++;

                                       //  Adjust counters
        m_byteing_on_idx += cnt;

        bytes_still_needed -= cnt;

                                       // Done?, if not then have to
                                       //  work with next link
        if (bytes_still_needed > 0)
        {
            m_link_byteing_on_ptr = m_link_byteing_on_ptr->next_ptr;
            m_byteing_on_idx        = 0;

            if (m_link_byteing_on_ptr == NULL)
            {
                                       // Here we have reached the end
                                       //  of the chain but haven't got
                                       //  all the bytes asked for -
                                       //  too bad...
                done = TRUE;
            }
            else if ((m_link_byteing_on_ptr->validity_chk_value 
                      != NTCSSCODE_VALITY_CHECK_VALUE) )
            {
                                       // Here we have a bad link in
                                       //  the chain
                                       //
                return(RETURN_INT_ERROR);
            }

          
        }

        else
                                       // Else have taken all the bytes
                                       //   that were asked for
            done = TRUE;
    }

    return(num_bytes_to_get - bytes_still_needed);
}



///////////////////////////////////////////////////////////////////////////////
// TrashChainLinks
// _______________
/*
Deletes both the data blocks and SMemChainLinkRec's which make up
a chain.  
*/

BOOL CMemChain::TrashChainLinks()
{
    SMemChainLinkRec *current_link_ptr = m_head_ptr;
    SMemChainLinkRec *next_ptr;

//AfxMessageBox("In TrashChainLinks, not doing deletes for now");

    while (current_link_ptr != NULL)
    {
        if (current_link_ptr->validity_chk_value != 
            NTCSSCODE_VALITY_CHECK_VALUE)
        {
                                       // If get here then have a bad
                                       //   link in the chain
// WSPTODO - probably need to do something better here
//AfxMessageBox("Bad SMemChainLinkRec - Bad because null link");

            return(FALSE);
        }
        next_ptr = current_link_ptr->next_ptr;

        delete [] current_link_ptr->data_block_ptr;
        
        delete current_link_ptr;
        
        current_link_ptr = next_ptr;
    }

    m_head_ptr = NULL;
    m_tail_ptr = NULL;
    m_link_byteing_on_ptr = NULL;
    m_byteing_on_idx        = -1;

    if (m_external_head_ptrptr != NULL)
        *m_external_head_ptrptr = m_head_ptr;

    return(TRUE);
}
