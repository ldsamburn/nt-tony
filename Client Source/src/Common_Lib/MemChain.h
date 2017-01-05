/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\memchain.h                    
//
// WSP, last update: 9/16/96
//                                        
// This is the header file for the CMemChain class and related
// declarations.  This class allows for a chain of memory blocks to be
// created and linked together in a manner so that the bytes can be
// accessed sequentially (using	the TakeChainBytes() function).
//
// See implementation file for details.
//----------------------------------------------------------------------

#ifndef _INCLUDE_MEMCHAIN_H_CLASS_
#define _INCLUDE_MEMCHAIN_H_CLASS_

                                       // A single chain link is made 
                                       //  from this struct
typedef struct SMemChainLinkRec
{
	int	   validity_chk_value;
    char  *data_block_ptr;
    int    data_block_len;
    struct SMemChainLinkRec *next_ptr;

} SMemChainLinkRec;
                                       // Users of CmemChain get
                                       //  a pointer to a TMemChainLink
                                       //  to work with - here it is 
                                       //  defined as just a char
typedef char TMemChainLink;

class CMemChain // : public CObject
{
private:
	SMemChainLinkRec  *m_head_ptr;
	SMemChainLinkRec  **m_external_head_ptrptr;
	SMemChainLinkRec  *m_tail_ptr;	
	SMemChainLinkRec  *m_link_byteing_on_ptr;	
	int				   m_byteing_on_idx;

public:

	CMemChain();

	CMemChain(TMemChainLink *first_link_ptr);

	~CMemChain();

	char *LinkData(TMemChainLink *);

	TMemChainLink *NewLink(int size);

	BOOL PrepareToByteChain();

	void Reset(TMemChainLink *first_link_ptr = NULL);

	void SetExternalHead(TMemChainLink **external_head_ptr)
		{ 
			m_external_head_ptrptr  = (SMemChainLinkRec  **)
									   external_head_ptr;
			*m_external_head_ptrptr = m_head_ptr;
		};

	int  TakeChainBytes(char *recv_buf, int num_bytes);
	
	BOOL TrashChainLinks();
};


#endif  // _INCLUDE_MEMCHAIN_H_CLASS_