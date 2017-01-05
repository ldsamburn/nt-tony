/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

// NTCSS_ROOT_DIR\source\include\msgcore.h                    
//
// WSP, last update: 9/16/96
//                                        
// This header file contains declarations (enums, defines and a struct)
// that are used at the core of both the NTCSS DLL message handling APIs
// and by the CNtcssBaseMsg and derived classes
//----------------------------------------------------------------------

#ifndef _INCLUDE_MSGCORE_H_CLASS_
#define _INCLUDE_MSGCORE_H_CLASS_

#include <sizes.h>
#include <memchain.h>

									   // Message categories
									   //
typedef enum {MEM_MSG_ALLOC,MEM_MSG_ADVALLOC} EmsgCategory;

									   // Indicates which server is
									   //  to be used
									   //
typedef enum {NO_SERVER_CONNECTION,
			  USE_APP_SERVER, USE_CONNECTED_SERVER,
			  USE_GIVEN_SERVER,USE_MASTER_SERVER,USE_AUTH_SERVER}
              EmsgServerChoices;

									   // Three states for messages
									   //
typedef enum {RAW,READY_TO_GO,DONE_SENT} EmsgState;


                                       // Used to transmit data between 
                                       //   the message classes and the 
									   //   dll

typedef struct SDllMsgRec
{
	int						validity_check_value;
    EmsgCategory            msg_category;
    EmsgServerChoices       server_select;
    char                    server_name[SIZE_HOST_NAME];
    int                     port_num;
    char                    ok_msg[SIZE_RESPONSE_CODE];
    char                    bad_msg[SIZE_RESPONSE_CODE];
    char                   *send_data_ptr;
    int                     send_data_size;
    void                   *recv_data_chain_ptr;
    int                     recv_data_size;

} SDllMsgRec;


                                       // These defines used in 
									   //   unbundling data
#define MAX_FORMAT_SIZE     20
#define CHAR_STR_KEY        'C'
#define INT_KEY             'I'
#define DATE_KEY            'D'
#define POINTER_KEY         'P'
#define CHAR_FMTSTR_LEN      5
#define INT_FMTSTR_LEN       1
#define DATE_FMTSTR_LEN     11


#endif // _INCLUDE_MSGCORE_H_CLASS_
