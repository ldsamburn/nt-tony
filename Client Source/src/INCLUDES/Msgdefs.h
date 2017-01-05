/*
	Copyright(c)2002 Northrop Grumman Corporation

		All Rights Reserved

	This material may be reproduced by or for the U.S. Government pursuant to 
	the copyright license under the clause at Defense Federal Acquisition 
	Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/
// NTCSS_ROOT_DIR\source\include\msgdefs.h                    
//
// WSP, last update: 9/16/96
//                                        
// This file contains definitions and declarations for all
// the message records sent to and received from the UNIX server in
// software that I am responsible for.  Thus far, this includes all 
// NTCSS DLL and MsgBoard application messages.
//
// CONTENTS:
// -------- 		  
//
//  >>> Sbasic_svrmsgrec				  
//  >>> ADDMSGBB stuff
//  >>> ADMINLOCKOPS
//  >>> CHANGEAPPDATA stuff
//  >>> CHANGELOGIN stuff
//  >>> CREATEMSGFILE stuff
//  >>> DELMSGBB stuff
//  >>> DELETESERVERFILE stuff
//  >>> GETALLBBS stuff
//  >>> GETAPPDATA stuff
//  >>> GETAPPLIST stuff
//  >>> GETAPPUSERINFO stuff
//  >>> GETAPPUSERLIST stuff
//  >>> GETBBMSGS stuff
//  >>> GETCOMMONDB stuff						
//  >>> GETCONFIGFILES stuff
//  >>> GETDIRLISTING stuff
//  >>> GETFILEINFO stuff
//  >>> GETHOSTINFO stuff
//  >>> GETLINKDATA stuff
//  >>> GETMASTERNAME stuff
//  >>> GETMSGFILE stuff
//  >>> GETNEWMSGFILE stuff
//  >>> GETOTYPEINI stuff
//  >>> GETOWNBBMSGS stuff
//  >>> GETPRINTERINI stuff
//  >>> GETPRTSTATUS stuff
//  >>> GETSUBSCRIBEDBBS stuff
//  >>> GETUCUSERLIST stuff
//  >>> GETPROGROUPLIST 
//  >>> GETUNSUBSCRIBEDBBS stuff
//  >>> GETUSERINI
//  >>> GETUSERPRTLIST stuff
//  >>> KILLSVRPROC stuff
//  >>> LAUNCHSVRPROCBG stuff
//  >>> MARKMSGREAD stuff
//  >>> NEWPRINTER  stuff
//  >>> NTCSS_QUERY stuff
//  >>> PRINTSVRFILE stuff
//  >>> PROCESSUSRCHANGES stuff
//  >>> PUTCONFIGFILES stuff
//  >>> PUSHNIS stuff
//  >>> REGISTERAPPUSER stuff
//  >>> REMOVESPQITEM stuff
//  >>> SCHEDSVRPROCBG stuff
//  >>> SENDBBMSG stuff
//  >>> SETCOMMONDB stuff
//  >>> SETLINKDATA stuff
//  >>> SETPRTSTATUS stuff
//  >>> SUBSCRIBEBB stuff
//  >>> SVRPROCLIST stuff
//  >>> VERIFYUSER stuff
//
//----------------------------------------------------------------------

                 
#ifndef _INCLUDE_MSGDEFS_H_CLASS_
#define _INCLUDE_MSGDEFS_H_CLASS_

#include <sizes.h>

// WSPTODO is this the best place


                                       // >>> Sbasic_svrmsgrec
                                       // Basic header info used with 
                                       //   most messages
typedef struct Sbasic_svrmsgrec
{
    char    header[SIZE_MESSAGE_TYPE];
    char    loginname[SIZE_LOGIN_NAME];
    char    token[SIZE_TOKEN];

}  Sbasic_svrmsgrec;



                                       // ***
                                       // >>> ADDMSGBB stuff
                                       //
#define  DO_ADDMSGBB_MSGSTR    "ADDMSGBB"
#define  OK_ADDMSGBB_MSGSTR    "GOODCHANGEMSGBB"
#define  BAD_ADDMSGBB_MSGSTR   "NOCHANGEMSGBB"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODCHANGEMSGBB (or
                                        //  NOCHANGEMSGBB)
                                        //
#define  SIZE_ADDMSGBB_RESPONSE     0

typedef struct Saddmsgbb_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                bb_name[SIZE_MSGBOARD_NAME];
    char                null_tail;

} Saddmsgbb_svrmsgrec;



                                       // ***
                                       // >>> ADMINLOCKOPS stuff
                                       //
#define  DO_ADMINLOCKOPS_MSGSTR    "ADMINLOCKOPS"
#define  OK_ADMINLOCKOPS_MSGSTR    "GOODADMINLOCKOPS"
#define  BAD_ADMINLOCKOPS_MSGSTR   "NOADMINLOCKOPS"

                                        
#define  SIZE_ADMINLOCKOPS_RESPONSE     512

typedef struct Sadminlockops_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_APP_NAME];
	char                code[SIZE_ADMINLOCK_OPS];
    char                action[SIZE_ADMINLOCK_OPS];
	char                pid[SIZE_ADMIN_PID];
    char                null_tail;

} Sadminlockops_svrmsgrec;


                                       // ***
                                       // >>> CHANGEAPPDATA stuff
                                       //
#define  DO_CHANGEAPPDATA_MSGSTR    "CHANGEAPPDATA"
#define  OK_CHANGEAPPDATA_MSGSTR    "GOODAPPDATACHANGE"
#define  BAD_CHANGEAPPDATA_MSGSTR   "NOAPPDATACHANGE"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODAPPDATACHANGE (or
                                        //  NOAPPDATACHANGE)
                                        //
#define  SIZE_CHANGEAPPDATA_RESPONSE     0

typedef struct Schangeappdata_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                appdata[SIZE_APP_DATA];
    char                null_tail;

} Schangeappdata_svrmsgrec;


                                       // ***
                                       // >>> CHANGELOGIN stuff
                                       //
#define  DO_CHANGELOGIN_MSGSTR    "CHANGELOGIN"
#define  OK_CHANGELOGIN_MSGSTR    "GOODCHANGELOGIN"
#define  BAD_CHANGELOGIN_MSGSTR   "NOCHANGELOGIN"

                                        
#define  SIZE_CHANGELOGIN_RESPONSE    512

typedef struct Schangelogin_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                LoginName[SIZE_LOGIN_NAME];
    char                Flag[2];
    char                null_tail;

} Schangelogin_svrmsgrec;




                                       // ***
                                       // >>> CREATEMSGFILE stuff
                                       //
#define  DO_CREATEMSGFILE_MSGSTR    "CREATEMSGFILE"
#define  OK_CREATEMSGFILE_MSGSTR    "GOODCREATEMSGFILE"
#define  BAD_CREATEMSGFILE_MSGSTR   "NOCREATEMSGFILE"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODCREATEMSGFILE (or
                                        //  NOCREATEMSGFILE)
                                        //
#define  SIZE_CREATEMSGFILE_RESPONSE     0

typedef struct Screatemsgfile_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                msg_id[SIZE_MSGBOARD_KEY];
    char                msg_title[SIZE_MSG_TITLE];
    char                null_tail;

} Screatemsgfile_svrmsgrec;



                                     // ***
                                       // >>> DELETESVRFILE stuff
                                       //
#define  DO_DELETESVRFILE_MSGSTR    "DELETESVRFILE"
#define  OK_DELETESVRFILE_MSGSTR    "GOODDELETESVRFILE"
#define  BAD_DELETESVRFILE_MSGSTR   "NODELETESVRFILE"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the DELETESVRFILE (or
                                        //  NODELETESVRFILE)
                                        //
#define  SIZE_DELETESVRFILE_RESPONSE     0

typedef struct Sdeletesvrfile_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                AppName[SIZE_GROUP_TITLE];
    char                FileName[SIZE_SERVER_FILE_NAME];
    char                null_tail;

} Sdeletesvrfile_svrmsgrec;


                                       // ***
                                       // >>> DELMSGBB stuff
                                       //
#define  DO_DELMSGBB_MSGSTR    "DELMSGBB"
#define  OK_DELMSGBB_MSGSTR    "GOODCHANGEMSGBB"
#define  BAD_DELMSGBB_MSGSTR   "NOCHANGEMSGBB"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODCHANGEMSGBB (or
                                        //  NOCHANGEMSGBB)
                                        //
#define  SIZE_DELMSGBB_RESPONSE     0

typedef struct Sdelmsgbb_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                bb_name[SIZE_MSGBOARD_NAME];
    char                null_tail;

} Sdelmsgbb_svrmsgrec;



                                       // ***
                                       // >>> GETAPPUSERLIST stuff
                                       //
#define  DO_GETAPPUSERLIST_MSGSTR    "GETAPPUSERLIST"
#define  OK_GETAPPUSERLIST_MSGSTR    "GOODAPPUSERLIST"
#define  BAD_GETAPPUSERLIST_MSGSTR   "NOAPPUSERLIST"

#define SIZE_REGISTERED_FLAG			2

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETAPPUSERLIST_RESPONSE    0

typedef struct Sgetappuserlist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                null_tail;

} Sgetappuserlist_svrmsgrec;


                                       // >>> GETALLBBS stuff
                                       //
#define  DO_GETALLBBS_MSGSTR    "GETALLBBS"
#define  OK_GETALLBBS_MSGSTR    "GOODGETALLBBS"
#define  BAD_GETALLBBS_MSGSTR   "NOGETALLBBS"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETALLBBS_RESPONSE    0

typedef struct Sgetallbbs_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetallbbs_svrmsgrec ;


//////////////////////////////////////////////////////////////////////////
                                      // ***
                                       // >>> GETAPPDATA stuff
                                       //
#define  DO_GETAPPDATA_MSGSTR    "GETAPPDATA"
#define  OK_GETAPPDATA_MSGSTR    "GOODGETAPPDATA"
#define  BAD_GETAPPDATA_MSGSTR   "NOGETAPPDATA"

                                        
#define  SIZE_GETAPPDATA_RESPONSE     512 //ITS ADV ALLOC

typedef struct Sgetappdata_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                null_tail;

} Sgetappdata_svrmsgrec;


//////////////////////////////////////////////////////////////////////////
    

                                       // ***
                                       // >>> GETAPPLIST stuff
                                       //
#define  DO_GETAPPLIST_MSGSTR    "GETAPPLIST"
#define  OK_GETAPPLIST_MSGSTR    "GOODAPPLIST"
#define  BAD_GETAPPLIST_MSGSTR   "NOAPPLIST"


                                        // Each record from server has
										//  32-bytes, for now allowing
                                        //  up to 25 apps, 
										// Jan 08, 1999 GWY.  Changed
										// SIZE_GETAPPLIST_RESPONSE from 800
										// to 16384 to allow about 500 apps
										// This was needed for ATLAS
#define  SIZE_GETAPPLIST_RESPONSE  16384


typedef struct Sgetapplist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetapplist_svrmsgrec;


                                       // ***
                                       // >>> GETAPPUSERINFO stuff
                                       //
#define  DO_GETAPPUSERINFO_MSGSTR    "GETAPPUSERINFO"
#define  OK_GETAPPUSERINFO_MSGSTR    "GOODAPPUSERINFO"
#define  BAD_GETAPPUSERINFO_MSGSTR   "NOAPPUSERINFO"

                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_GETAPPUSERINFO_RESPONSE    512

typedef struct Sgetappuserinfo_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                username[SIZE_LOGIN_NAME];
    char                appname[SIZE_GROUP_TITLE];
    char                null_tail;

} Sgetappuserinfo_svrmsgrec;


                                       // ***
                                       // >>> GETCOMMONDB stuff
                                       //
#define  DO_GETCOMMONDB_MSGSTR    "GETCOMMONDB"
#define  OK_GETCOMMONDB_MSGSTR    "GOODGETCOMMONDB"
#define  BAD_GETCOMMONDB_MSGSTR   "NOGETCOMMONDB"


                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_GETCOMMONDB_RESPONSE  150

typedef struct Sgetcommondb_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                tag[SIZE_COMMON_TAG];
    char                null_tail;

} Sgetcommondb_svrmsgrec;


                                       // ***
                                       // >>> GETBBMSGS stuff
                                       //
#define  DO_GETBBMSGS_MSGSTR    "GETBBMSGS"
#define  OK_GETBBMSGS_MSGSTR    "GOODGETBBMSGS"
#define  BAD_GETBBMSGS_MSGSTR   "NOGETBBMSGS"


                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
#define  SIZE_GETBBMSGS_RESPONSE  0

typedef struct Sgetbbmsgs_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                bb_name[SIZE_MSGBOARD_NAME];
    char                null_tail;

} Sgetbbmsgs_svrmsgrec;

                                       // ***
                                       // >>> GETCONFIGFILES stuff
                                       //

#define  DO_GETCONFIGFILES_MSGSTR    "GETCONFIGFILES"
#define  OK_GETCONFIGFILES_MSGSTR    "GOODGETCONFIGFILES"
#define  BAD_GETCONFIGFILES_MSGSTR   "NOGETCONFIGFILES"


#define  CONFIGFILES_FILE_CODE_SIZE   3

#define  CONFIGFILES_APPCONFIG_FILE_CODE    1
#define  CONFIGFILES_HOSTCONFIG_FILE_CODE   2
#define  CONFIGFILES_DEVCONFIG_FILE_CODE    4

                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_GETCONFIGFILES_RESPONSE  150

typedef struct Sgetconfigfiles_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                file_code[CONFIGFILES_FILE_CODE_SIZE];
    char                null_tail;

} Sgetconfigfiles_svrmsgrec;



                                       // ***
                                       // >>> GETDIRLISTING stuff
                                       //
#define  DO_GETDIRLISTING_MSGSTR    "GETDIRLISTING"
#define  OK_GETDIRLISTING_MSGSTR    "GOODDIRLISTING"
#define  BAD_GETDIRLISTING_MSGSTR   "NODIRLISTING"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETDIRLISTING_RESPONSE    0

typedef struct Sgetdirlisting_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                directory[SIZE_SERVER_LOCATION];
    char                null_tail;

} Sgetdirlisting_svrmsgrec ;


  
                                       // ***
                                       // >>> GETFILEINFO stuff
                                       //
#define  DO_GETFILEINFO_MSGSTR    "GETFILEINFO"
#define  OK_GETFILEINFO_MSGSTR    "GOODGETFILEINFO"
#define  BAD_GETFILEINFO_MSGSTR   "NOGETFILEINFO"

                                        // This should be plenty
                                        //
#define  SIZE_GETFILEINFO_RESPONSE    512

typedef struct Sgetfileinfo_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                fname[SIZE_SERVER_LOCATION];
	char                null_tail;

} Sgetfileinfo_svrmsgrec ;

  
                                       // ***
                                       // >>> GETFILESECTION stuff
                                       //
#define  DO_GETFILESECTION_MSGSTR    "GETFILESECTION"
#define  OK_GETFILESECTION_MSGSTR    "GOODGETFILESECTION"
#define  BAD_GETFILESECTION_MSGSTR   "NOGETFILESECTION"

                                        // This should be plenty
                                        //
#define  SIZE_GETFILESECTION_RESPONSE    0

typedef struct Sgetfilesection_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                fname[SIZE_SERVER_LOCATION];
	char                FirstLine[SIZE_CHAR_TO_INT];
	char                LastLine[SIZE_CHAR_TO_INT];
	char                null_tail;

} Sgetfilesection_svrmsgrec ;
  
                                       // ***
                                       // >>> GETHOSTINFO stuff
                                       //
#define  DO_GETHOSTINFO_MSGSTR    "GETHOSTINFO"
#define  OK_GETHOSTINFO_MSGSTR    "GOODGETHOSTINFO"
#define  BAD_GETHOSTINFO_MSGSTR   "NOGETHOSTINFO"

                                        // This should be plenty
                                        //
#define  SIZE_GETHOSTINFO_RESPONSE    512

typedef struct Sgethostinfo_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                host_name[SIZE_HOST_NAME];
	char                ip_address[SIZE_IP_ADDR*2];
    char                null_tail;

} Sgethostinfo_svrmsgrec ;

                                       // ***
                                       // >>> GETLINKDATA stuff
                                       //
#define  DO_GETLINKDATA_MSGSTR    "GETLINKDATA"
#define  OK_GETLINKDATA_MSGSTR    "GOODGETLINKDATA"
#define  BAD_GETLINKDATA_MSGSTR   "NOGETLINKDATA"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODAPPDATACHANGE (or
                                        //  NOAPPDATACHANGE)
                                        //
#define  SIZE_GETLINKDATA_RESPONSE     512

typedef struct Sgetlinkdata_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                null_tail;
} Sgetlinkdata_svrmsgrec;

                                       // ***
                                       // >>> GETMASTERNAME stuff
                                       //
#define  DO_GETMASTERNAME_MSGSTR    "GETMASTERNAME"
#define  OK_GETMASTERNAME_MSGSTR    "GOODMASTERNAME"
#define  BAD_GETMASTERNAME_MSGSTR   "NOMASTERNAME"

                                        // This should be plenty
                                        //
#define  SIZE_GETMASTERNAME_RESPONSE    100

typedef struct Sgetmastername_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetmastername_svrmsgrec ;
    

                                       // ***
                                       // >>> GETMSGFILE stuff
                                       //
#define  DO_GETMSGFILE_MSGSTR    "GETMSGFILE"
#define  OK_GETMSGFILE_MSGSTR    "GOODGETMSGFILE"
#define  BAD_GETMSGFILE_MSGSTR   "NOGETMSGFILE"

                                        // This should be plenty
                                        //
#define  SIZE_GETMSGFILE_RESPONSE    256

typedef struct Sgetmsgfile_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				msg_id[SIZE_MSGBOARD_KEY];
    char                null_tail;

} Sgetmsgfile_svrmsgrec ;
    


  
                                       // ***
                                       // >>> GETNEWMSGFILE stuff
                                       //
#define  DO_GETNEWMSGFILE_MSGSTR    "GETNEWMSGFILE"
#define  OK_GETNEWMSGFILE_MSGSTR    "GOODGETNEWMSGFILE"
#define  BAD_GETNEWMSGFILE_MSGSTR   "NOGETNEWMSGFILE"

                                        // This should be plenty
                                        //
#define  SIZE_GETNEWMSGFILE_RESPONSE    256

typedef struct Sgetnewmsgfile_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetnewmsgfile_svrmsgrec ;
    
                                       // ***
                                       // >>> GETOTYPEINI stuff
                                       //
#define  DO_GETOTYPEINI_MSGSTR    "GETOTYPEINI"
#define  OK_GETOTYPEINI_MSGSTR    "GOODOTYPEINI"
#define  BAD_GETOTYPEINI_MSGSTR   "NOOTYPEINI"

                                        // This should be plenty
                                        //
#define  SIZE_GETOTYPEINI_RESPONSE    512

typedef struct Sgetotypeini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetotypeini_svrmsgrec ;


                                       // ***
                                       // >>> GETPRINTERINI stuff
                                       //
#define  DO_GETPRINTERINI_MSGSTR    "GETPRINTERINI"
#define  OK_GETPRINTERINI_MSGSTR    "GOODGETPRINTERINI"
#define  BAD_GETPRINTERINI_MSGSTR   "NOGETPRINTERINI"

                                        // This should be plenty
                                        //
#define  SIZE_GETPRINTERINI_RESPONSE    512

typedef struct Sgetprinterini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetprinterini_svrmsgrec ;



                                       // ***
                                       // >>> GETPRTSTATUS stuff
                                       //
#define  DO_GETPRTSTATUS_MSGSTR    "GETPRTSTATUS"
#define  OK_GETPRTSTATUS_MSGSTR    "GOODGETPRTSTATUS"
#define  BAD_GETPRTSTATUS_MSGSTR   "NOGETPRTSTATUS"

                                        // This should be plenty
                                        //
#define  SIZE_GETPRTSTATUS_RESPONSE    512

typedef struct Sgetprtstatus_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char				printer_name[SIZE_PRINTER_NAME];
    char                null_tail;

} Sgetprtstatus_svrmsgrec ;


    

                                       // ***
                                       // >>> GETSUBSCRIBEDBBS stuff
                                       //
#define  DO_GETSUBSCRIBEDBBS_MSGSTR    "GETSUBSCRIBEDBBS"
#define  OK_GETSUBSCRIBEDBBS_MSGSTR    "GOODGETSUBSCRBBS"
#define  BAD_GETSUBSCRIBEDBBS_MSGSTR   "NOGETSUBSCRBBS"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETSUBSCRIBEDBBS_RESPONSE    0

typedef struct Sgetsubscribedbbs_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                null_tail;

} Sgetsubscribedbbs_svrmsgrec ;
    

                                       // >>> GETOWNBBMSGS stuff
                                       //
#define  DO_GETOWNBBMSGS_MSGSTR    "GETOWNBBMSGS"
#define  OK_GETOWNBBMSGS_MSGSTR    "GOODGETBBMSGS"
#define  BAD_GETOWNBBMSGS_MSGSTR   "NOGETBBMSGS"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETOWNBBMSGS_RESPONSE    0

typedef struct Sgetownbbmsgs_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetownbbmsgs_svrmsgrec ;
    

                                       // ***
                                       // >>> GETUNSUBSCRIBEDBBS stuff
                                       //
#define  DO_GETUNSUBSCRIBEDBBS_MSGSTR    "GETUNSUBSCRIBEDBBS"
#define  OK_GETUNSUBSCRIBEDBBS_MSGSTR    "GOODGETUNSUBSCRBBS"
#define  BAD_GETUNSUBSCRIBEDBBS_MSGSTR   "NOGETUNSUBSCRBBS"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETUNSUBSCRIBEDBBS_RESPONSE    0

typedef struct Sgetunsubscribedbbs_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetunsubscribedbbs_svrmsgrec ;
    

                                       // ***
                                       // >>> GETSYSTEMSTATUS stuff
                                       //      Depricated

//#define  DO_GETSYSTEMSTATUS_MSGSTR    "GETSYSTEMSTATUS"
//#define  OK_GETSYSTEMSTATUS_MSGSTR    "GOODGETSYSTEMSTATUS"
//#define  BAD_GETSYSTEMSTATUS_MSGSTR   "NOGETSYSTEMSTATUS"


                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
//#define  SIZE_GETSYSTEMSTATUS_RESPONSE  150

//typedef struct Sgetsystemstatus_svrmsgrec 
//{
//    Sbasic_svrmsgrec    bsc;
//    char                null_tail;

//} Sgetsystemstatus_svrmsgrec;


                                      // ***
                                       // >>> GETUSERADMINUSRSTR stuff for USR ADM
                                       //
#define  DO_GETUSRADMINUSRSTR_MSGSTR "GETUSERADMINUSRSTR"
#define  OK_GETUSRADMINUSRSTR_MSGSTR    "GOODUSRADMSTR"
#define  BAD_GETUSRADMINUSRSTR_MSGSTR   "BADUSRADMSTR"

                                        // This should be plenty
                                        //
#define  SIZE_GETUSERADMINUSRSTR_RESPONSE    512

typedef struct Sgetuseradminusrstr_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                UserName[SIZE_LOGIN_NAME];
    char                null_tail;

} Sgetuseradminusrstr_svrmsgrec ;


                                       // ***
                                       // >>> GETUSERINI stuff
                                       //
#define  DO_GETUSERINI_MSGSTR    "GETUSRADMINI"
#define  OK_GETUSERINI_MSGSTR    "GOODUSRADMINI"
#define  BAD_GETUSERINI_MSGSTR   "BADUSRADMINI "

                                        // This should be plenty
                                        //
#define  SIZE_GETUSERINI_RESPONSE    0

typedef struct Sgetusererini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Sgetuserini_svrmsgrec ;



                                     // ***
                                     // >>> GETUCUSERLIST stuff
                                     //
#define  DO_GETUCUSERLIST_MSGSTR    "GETUCUSERLIST"
#define  OK_GETUCUSERLIST_MSGSTR    "GOODUCUSERLIST"
#define  BAD_GETUCUSERLIST_MSGSTR   "NOUCUSERLIST"

                                      // ADV ALLOCATED
                                      //

#define  SIZE_GETUCUSERLIST_RESPONSE    0

typedef struct SgetUCuserlist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                action[2];
    char                null_tail;

} SgetUCuserlist_svrmsgrec;


                                    // ***
                                     // >>> GETUCPROGROUPLIST stuff
                                     //
#define  DO_GETUCPROGROUPLIST_MSGSTR    "GETUCPROGROUPLIST"
#define  OK_GETUCPROGROUPLIST_MSGSTR    "GOODUCPROGROUPLIST"
#define  BAD_GETUCPROGROUPLIST_MSGSTR   "NOUCPROGROUPLIST"

                                      // ADV ALLOCATED
                                      //

#define  SIZE_GETUCPROGROUPLIST_RESPONSE    0

typedef struct SgetUCprogrouplist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} SgetUCprogrouplist_svrmsgrec;




//  >>> GETCOMMONDB stuff
//  >>>  stuff
                                       // ***
                                       // >>> GETUSERPRTLIST stuff
                                       //
#define  DO_GETUSERPRTLIST_MSGSTR    "GETUSERPRTLIST"
#define  OK_GETUSERPRTLIST_MSGSTR    "GOODPRTLIST"
#define  BAD_GETUSERPRTLIST_MSGSTR   "NOPRTLIST"

#define  ALL_PRINTERS_FLAGVAL   '0'
#define  DEFAULT_ONLY_FLAGVAL   '1'

#define  ACCESS_BYAPP_FLAGVAL   '0'
#define  ACCESS_GEN_FLAGVAL     '1'

                                        // Can handle worse case with
                                        //  this value
                                        // 
#define  SIZE_GETUSERPRTLIST_RESPONSE   25000

typedef struct Sgetuserprtlist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                output_type[SIZE_PRT_OUTPUT_TYPE];
    char                security_class;
    char                default_only_flag;
    char                access_flag;
    char                null_tail;
} Sgetuserprtlist_svrmsgrec ;


typedef struct Sgetuserprtlist_from_svrmsgrec  
{
    char    printer_name_buf[SIZE_PRINTER_NAME];
    char    host_name_buf[SIZE_HOST_NAME];
    char    printer_loc_buf[SIZE_PRT_LOCATION];
    char    printer_device_name_buf[SIZE_PRT_DEV_NAME];
    char    printer_driver_name_buf[SIZE_PRT_DRIVER_NAME];
    char    redirect_printer_buf[SIZE_PRT_RED];
    BOOL    default_printer_flag;
    BOOL    queue_enabled_flag;
    BOOL    printer_enabled_flag;
}
Sgetuserprtlist_from_svrmsgrec;

                                       // ***
                                       // >>> KILLSVRPROC stuff
                                       //
#define HARD_KILL_SIGNAL_STR    "01"
#define SOFT_KILL_SIGNAL_STR    "00"

#define  DO_KILLSVRPROC_MSGSTR        "KILLSVRPROC"
#define  OK_KILLSVRPROC_MSGSTR        "GOODKILLSVRPROC" 
#define  BAD_KILLSVRPROC_MSGSTR       "NOKILLSVRPROC"  

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODKILLSVRPROC (or
                                        //  NOKILLSVRPROC)
#define  SIZE_KILLSVRPROC_RESPONSE   0

typedef struct Skillsvrproc_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                pid[SIZE_PROCESS_ID];
    char                signal_type[SIZE_SIGNAL_TYPE];
	char                program_group[SIZE_GROUP_TITLE];
    char                null_tail;

} Skillsvrproc_svrmsgrec ;
    
                                 
                                     // *** 
                                     // >>> LAUNCHSVRPROCBG stuff
                                     //
#define LAUNCHSVRPROCBG_NEED_APPROVAL_STR        "-1"
#define LAUNCHSVRPROCBG_NEED_DEVICE_STR          "-2"
#define LAUNCHSVRPROCBG_READY_TO_EXECUTE_STR     "-5"

#define PRINT_OUTPUT_FLAG        1
#define DEL_AFTER_PRINT_FLAG     2
#define RESTART_FLAG		     4
		 

#define  DO_LAUNCHSVRPROCBG_MSGSTR       "LAUNCHSVRPROCBG"
#define  OK_LAUNCHSVRPROCBG_MSGSTR       "GOODSVRPROC"
#define  BAD_LAUNCHSVRPROCBG_MSGSTR      "NOSVRPROC"


                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_LAUNCHSVRPROCBG_RESPONSE  350

typedef struct Slaunchsvrprocbg_svrmsgrec         
{                                                 
    Sbasic_svrmsgrec    bsc;                      
    char                appname[SIZE_GROUP_TITLE]; 
    char                state[SIZE_PROCESS_STATE];
    char                priority[SIZE_PRIORITY];
    char                cmd_line[SIZE_CMD_LINE];
    char                unix_owner[SIZE_LOGIN_NAME];
    char                job_descrip[SIZE_JOB_DESCRIP];
    char                seclvl[SIZE_SECURITY_CLASS];
    char                printer_name[SIZE_PRINTER_NAME];
    char                printer_host[SIZE_HOST_NAME];
    char                printer_seclvl[SIZE_SECURITY_CLASS];
    char                printer_file[SIZE_SERVER_LOCATION];
    char                print_flag[SIZE_PRT_FLAG];
	char                sdi_required_flag; 
    char                sdi_input_flag;
    char                sdi_control_record_id_str[NTCSS_SIZE_SDI_CONTROL_REC_ID+1];
    char                sdi_data_file_str[NTCSS_SIZE_SERVER_LOCATION];
    char                sdi_tci_file_str[NTCSS_SIZE_SERVER_LOCATION];
    char                orientation[SIZE_OF_AN_INT];  
    char                banner;
    char                copies[SIZE_OF_AN_INT];
	char                papersize[SIZE_OF_AN_INT];
    char                null_tail;

} Slaunchsvrprocbg_svrmsgrec ;
    

                                       // ***
                                       // >>> MARKMSGREAD stuff
                                       //

#define  DO_MARKMSGREAD_MSGSTR    "MARKMSGREAD"
#define  OK_MARKMSGREAD_MSGSTR    "GOODMARKMSGREAD"
#define  BAD_MARKMSGREAD_MSGSTR   "NOMARKMSGREAD"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODMARKMSGREAD (or
                                        //  NOMARKMSGREAD)
                                        //
#define  SIZE_MARKMSGREAD_RESPONSE     0

typedef struct Smarkmsgread_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                msg_id[SIZE_MSGBOARD_KEY];
    char                null_tail;

} Smarkmsgread_svrmsgrec;



                                       // ***
                                       // >>> NEWNTPRINTER stuff
                                       //
#define  DO_NEWNTPRINTER_MSGSTR    "NEWNTPRINTER"
#define  OK_NEWNTPRINTER_MSGSTR    "GOODNEWNTPRINTER"
#define  BAD_NEWNTPRINTER_MSGSTR   "NONEWNTPRINTER"

                                        // This should be plenty
                                        //
#define  SIZE_NEWNTPRINTER_RESPONSE    512

typedef struct Snewntprinter_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                drivername[SIZE_PRT_DEV_NAME];  //128
	char				driverfile[SIZE_PRT_DEV_NAME];
	char				uifile    [SIZE_PRT_DEV_NAME];
	char				datafile  [SIZE_PRT_DEV_NAME];
	char				helpfile  [SIZE_PRT_DEV_NAME];
	char                monitorfile [SIZE_PRT_DEV_NAME];
    char                monitorstring [SIZE_PRT_DEV_NAME];
	char                copyfiles [SIZE_COPY_FILES];
    char                null_tail;

} Snewntprinter_svrmsgrec ;




                                        // >>> NTCSS_QUERY stuff - just
                                        //  a single string for this msg
                                        //
#define DO_NTCSS_QUERY_MSGSTR            "NTCSS_QUERY"



                                       // ***
                                       // >>> PRINTSVRFILE stuff
                                       //
#define  DO_PRINTSVRFILE_MSGSTR    "PRINTSVRFILE"
#define  OK_PRINTSVRFILE_MSGSTR    "GOODPRINTSVRFILE"
#define  BAD_PRINTSVRFILE_MSGSTR   "NOPRINTSVRFILE"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODAPPDATACHANGE (or
                                        //  NOAPPDATACHANGE)
                                        //
#define  SIZE_PRINTSVRFILE_RESPONSE  0

typedef struct Sprintsvrfile_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                prt_file[SIZE_SERVER_LOCATION];
    char                prt_name[SIZE_PRINTER_NAME];
    char                host_name[SIZE_HOST_NAME];
    char                progroup_title[SIZE_GROUP_TITLE];
    char                security_class[SIZE_SECURITY_CLASS];
	char                copies[NTCSS_MAX_PRT_COPIES_LEN];
	char                options[NTCSS_MAX_PRT_OPTIONS_LEN];
    char                null_tail;

} Sprintsvrfile_svrmsgrec;
                                      // ***
                                       // >>> PROCESSUSRCHANGES stuff
                                       //

#define  DO_USRADMAPPLY_MSGSTR    "PROCESSUSRCHANGES"
#define  OK_USRADMAPPLY_MSGSTR    "GOODUSRADMAPPLY"
#define  BAD_USRADMAPPLY_MSGSTR   "BADUSRADMAPPLY"

                                        
#define  SIZE_USRADMAPPLY_RESPONSE     0

typedef struct Susradmapply_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Susradmapply_svrmsgrec;

                                       // ***
                                       // >>> PUTCONFIGFILES stuff
                                       //

                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_GETCONFIGFILES_RESPONSE  150

		 
#define  DO_PUTCONFIGFILES_MSGSTR    "PUTCONFIGFILES"
#define  OK_PUTCONFIGFILES_MSGSTR    "GOODPUTCONFIGFILES"
#define  BAD_PUTCONFIGFILES_MSGSTR   "NOPUTCONFIGFILES"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODPUTCONFIGFILES (or
                                        //  NOPUTCONFIGFILES)
                                        //
#define  SIZE_PUTCONFIGFILES_RESPONSE  0

										// See GETCONFIGFILES stuff
										//  for definitions of values
										//  for file_code

typedef struct Sputconfigfiles_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                file_code[CONFIGFILES_FILE_CODE_SIZE];
    char                null_tail;

} Sputconfigfiles_svrmsgrec;

                                     // ***
                                       // >>> PUTOTYPEINI stuff
                                       //
#define  DO_PUTOTYPEINI_MSGSTR    "PUTOTYPEINI"
#define  OK_PUTOTYPEINI_MSGSTR    "GOODOTYPEINI"
#define  BAD_PUTOTYPEINI_MSGSTR   "NOOTYPEINI"

                                        // This should be plenty
                                        //
#define  SIZE_PUTOTYPEINI_RESPONSE    512

typedef struct Sputotypeini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                file_name[SIZE_APP_DATA];
    char                null_tail;

} Sputotypeini_svrmsgrec ;

                                     // ***
                                       // >>> PUSHNIS stuff
                                       //
#define  DO_PUSHNIS_MSGSTR    "PUSHNIS"
#define  OK_PUSHNIS_MSGSTR    "GOODPUSHNIS"
#define  BAD_PUSHNIS_MSGSTR   "NOPUSHNIS"

                                        // This should be plenty
                                        //
#define  SIZE_PUSHNIS_RESPONSE    512

typedef struct Spushnis_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Spushnisi_svrmsgrec ;


                                       // ***
                                       // >>> PUTPRINTERINI stuff
                                       //
#define  DO_PUTPRINTERINI_MSGSTR    "PUTPRINTERINI"
#define  OK_PUTPRINTERINI_MSGSTR    "GOODPUTPRINTERINI"
#define  BAD_PUTPRINTERINI_MSGSTR   "NOPUTPRINTERINI"

                                        // This should be plenty
                                        //
#define  SIZE_PUTPRINTERINI_RESPONSE    512

typedef struct Sputprinterini_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                file_name[SIZE_APP_DATA];
    char                null_tail;

} Sputprinterini_svrmsgrec ;

                                       // ***
                                       // >>> REGISTERAPPUSER stuff
                                       //
#define  DO_REGISTERAPPUSER_MSGSTR    "REGISTERAPPUSER"
#define  OK_REGISTERAPPUSER_MSGSTR    "GOODREGAPPUSER"
#define  BAD_REGISTERAPPUSER_MSGSTR   "NOREGAPPUSER"

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODREGAPPUSER (or
                                        //  NOREGAPPUSER)
#define  SIZE_REGISTERAPPUSER_RESPONSE  0

typedef struct Sregisterappuser_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                username[SIZE_LOGIN_NAME];
    char                appname[SIZE_GROUP_TITLE];
    char                flag;
    char                null_tail;

} Sregisterappuser_svrmsgrec ;
    

                                     // *** 
                                     // >>> SCHEDSVRPROCBG stuff
                                     //
#define SCHEDSVRPROCBG_NEED_APPROVAL_STR        "-1"
#define SCHEDSVRPROCBG_NEED_DEVICE_STR          "-2"
#define SCHEDSVRPROCBG_READY_TO_EXECUTE_STR      "-7"


#define  DO_SCHEDSVRPROCBG_MSGSTR       "SCHEDSVRPROCBG"
#define  OK_SCHEDSVRPROCBG_MSGSTR       "GOODSVRPROC"
#define  BAD_SCHEDSVRPROCBG_MSGSTR      "NOSVRPROC"


                                        // Should be a bit bigger than
                                        //  amount expected
                                        //
#define  SIZE_SCHEDSVRPROCBG_RESPONSE  350

typedef struct Sschedsvrprocbg_svrmsgrec         
{                                                 
    Sbasic_svrmsgrec    bsc;                      
    char                appname[SIZE_GROUP_TITLE]; 
    char                state[SIZE_PROCESS_STATE];
    char                schedule[SIZE_SCHEDULE];
    char                cmd_line[SIZE_CMD_LINE];
    char                unix_owner[SIZE_LOGIN_NAME];
    char                job_descrip[SIZE_JOB_DESCRIP];
    char                seclvl[SIZE_SECURITY_CLASS];
    char                printer_name[SIZE_PRINTER_NAME];
    char                printer_host[SIZE_HOST_NAME];
    char                printer_seclvl[SIZE_SECURITY_CLASS];
    char                printer_file[SIZE_SERVER_LOCATION];
    char                print_flag[SIZE_PRT_FLAG];
	char                orientation[SIZE_OF_AN_INT];  
    char                banner;
    char                copies[SIZE_OF_AN_INT];
	char                papersize[SIZE_OF_AN_INT];
    char                null_tail;
} Sschedsvrprocbg_svrmsgrec ;
    

                                       // ***
                                       // >>> REMOVESPQITEM stuff
                                       //
#define PRIORITY_JOB_STR        "00"
#define SCHEDULED_JOB_STR       "01"

#define  DO_REMOVESPQITEM_MSGSTR        "REMOVESPQITEM"
#define  OK_REMOVESPQITEM_MSGSTR        "GOODREMOVESPQITEM" 
#define  BAD_REMOVESPQITEM_MSGSTR       "NOREMOVESPQITEM"  

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODREMOVESPQITEM (or
                                        //  NOREMOVESPQITEM)
#define  SIZE_REMOVESPQITEM_RESPONSE   0

typedef struct Sremovespqitem_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                pid[SIZE_PROCESS_ID];
    char                job_type[SIZE_JOB_TYPE];
	char                program_group[SIZE_GROUP_TITLE];
    char                null_tail;

} Sremovespqitem_svrmsgrec ;
    
                                 
                                       // ***
                                       // >>> SETCOMMONDB stuff
                                       //
#define  DO_SETCOMMONDB_MSGSTR    "SETCOMMONDB"
#define  OK_SETCOMMONDB_MSGSTR    "GOODSETCOMMONDB"
#define  BAD_SETCOMMONDB_MSGSTR   "NOSETCOMMONDB"


                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODSETCOMMONDB (or
                                        //  NOSETCOMMONDB)
                                        //
#define  SIZE_SETCOMMONDB_RESPONSE   0

typedef struct Ssetcommondb_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                tag[SIZE_COMMON_TAG];
    char                data[SIZE_COMMON_DATA];
    char                null_tail;

} Ssetcommondb_svrmsgrec;

                                
                                       // ***
                                       // >>> SETLINKDATA stuff
                                       //
#define  DO_SETLINKDATA_MSGSTR    "SETLINKDATA"
#define  OK_SETLINKDATA_MSGSTR    "GOODSETLINKDATA"
#define  BAD_SETLINKDATA_MSGSTR   "NOSETLINKDATA"


                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODSETLINKDATA (or
                                        //  NOSETLINKDATA)
                                        //
#define  SIZE_SETLINKDATA_RESPONSE   0

typedef struct Ssetlinkdata_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                appname[SIZE_GROUP_TITLE];
    char                link_data[SIZE_APP_PASSWORD];
    char                null_tail;

} Ssetlinkdata_svrmsgrec;

                                 
                                       // ***
                                       // >>> SETPRTSTATUS stuff
                                       //
#define  DO_SETPRTSTATUS_MSGSTR    "SETPRTSTATUS"
#define  OK_SETPRTSTATUS_MSGSTR    "GOODSETPRTSTATUS"
#define  BAD_SETPRTSTATUS_MSGSTR   "NOSETPRTSTATUS"


                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODSETPRTSTATUS (or
                                        //  NOSETPRTSTATUS)
                                        //
#define  SIZE_SETPRTSTATUS_RESPONSE   512

typedef struct Ssetprtstatus_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                printer_name[SIZE_PRINTER_NAME];
    char                printer_enable_status[SIZE_PRT_FLAG-1];
    char                queue_enable_status[SIZE_PRT_FLAG-1];
	char                null_tail;


} Ssetprtstatus_svrmsgrec;


                                       // ***
                                       // >>> SENDBBMSG stuff
                                       //
#define  DO_SENDBBMSG_MSGSTR    "SENDBBMSG"
#define  OK_SENDBBMSG_MSGSTR    "GOODSENDBBMSG"
#define  BAD_SENDBBMSG_MSGSTR   "NOSENDBBMSG"


                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODSENDBBMSG (or
                                        //  NOSENDBBMSG)
                                        //
#define  SIZE_SENDBBMSG_RESPONSE   0

typedef struct Ssendbbmsg_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                msg_id[SIZE_MSGBOARD_KEY];
    char                bb_name[SIZE_MSGBOARD_NAME];
    char                null_tail;

} Ssendbbmsg_svrmsgrec;

                                       // ***
                                       // >>> SVRPROCLIST stuff
                                       //
#define  DO_SVRPROCLIST_MSGSTR    "SVRPROCLIST"
#define  OK_SVRPROCLIST_MSGSTR    "GOODSPQUE"
#define  BAD_SVRPROCLIST_MSGSTR   "NOSPQUE"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_SVRPROCLIST_RESPONSE    0

#define  SVRPROCLIST_FILTER_VAL_CUST_PROC_STAT       1
#define  SVRPROCLIST_FILTER_VAL_PRINT_INFO           2
#define  SVRPROCLIST_FILTER_VAL_JOB_DESCR_INFO       4
#define  SVRPROCLIST_FILTER_VAL_USER_ONLY_INFO       8
#define  SVRPROCLIST_FILTER_VAL_RUN_PROCS_INFO       16
#define  SVRPROCLIST_FILTER_VAL_ONLY_USER_ACTIVE     32 //overrides all when set
#define  SVRPROCLIST_FILTER_VAL_PRIORITY             64
#define  SVRPROCLIST_FILTER_VAL_COMMAND_LINE         128
#define  SVRPROCLIST_FILTER_VAL_CUST_PROC_STATUS     256
#define  SVRPROCLIST_FILTER_VAL_LOGIN_NAME           512
#define  SVRPROCLIST_FILTER_VAL_UNIX_OWNER           1024
#define  SVRPROCLIST_FILTER_VAL_PROGROUP_TITLE       2048
#define  SVRPROCLIST_FILTER_VAL_JOB_DESCRIPTION      4096
#define  SVRPROCLIST_FILTER_VAL_ORIG_HOST            8192


#define  SIZE_SVRPROCLIST_FILTER_MASK 6



typedef struct Ssvrproclist_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                appname[SIZE_GROUP_TITLE]; 
    char                role;
    char                is_schedjobs;
    char                filter_mask[SIZE_SVRPROCLIST_FILTER_MASK];
	char                priority[SIZE_PRIORITY];
	char				command_line[NTCSS_SIZE_COMMAND_LINE];
	char				cust_proc_status[NTCSS_SIZE_CUSTOM_PROC_STAT];
    char				login_name[SIZE_LOGIN_NAME];
	char				unix_owner[SIZE_LOGIN_NAME];
	char				progroup_title[NTCSS_SIZE_APP_GROUP];
	char			    job_descrip[NTCSS_SIZE_JOB_DESCRIP];
	char				orig_host[NTCSS_SIZE_ORIG_HOST];
	char				null_tail;

} Ssvrproclist_svrmsgrec ;
    

                                       // ***
                                       // >>> SUBSCRIBEBB stuff
                                       //
#define  DO_SUBSCRIBEBB_MSGSTR    "SUBSCRIBEBB"
#define  OK_SUBSCRIBEBB_MSGSTR    "GOODSUBSCRIBEBB"
#define  BAD_SUBSCRIBEBB_MSGSTR   "NOSUBSCRIBEBB"

#define  SUBSCRIBE_FLAG         '1'
#define  UNSUBSCRIBE_FLAG       '0'

                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODSUBSCRIBEBB (or
                                        //  NOSUBSCRIBEBB)
                                        //
#define  SIZE_SUBSCRIBEBB_RESPONSE   0

typedef struct Ssubscribebb_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                bb_name[SIZE_MSGBOARD_NAME];
    char                flag;
    char                null_tail;

} Ssubscribebb_svrmsgrec;


                                
                                       // ***
                                       // >>> VALIDATEUSER stuff
                                       //
#define  DO_VALIDATEUSER_MSGSTR    "VALIDATEUSER"
#define  OK_VALIDATEUSER_MSGSTR    "GOODVALIDATEUSER"
#define  BAD_VALIDATEUSER_MSGSTR   "NOVALIDATEUSER"


                                        // Here don't worry about
                                        //  receiving anything past
                                        //  the GOODVALIDATEUSER (or
                                        //  NOVALIDATEUSER)
                                        //
#define  SIZE_VALIDATEUSER_RESPONSE   0

typedef struct Svalidateuser_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                username[SIZE_LOGIN_NAME];
    char                password[SIZE_APP_PASSWORD*2];
	char                appname[SIZE_APP_NAME]; 
    char                null_tail;

} Svalidateuser_svrmsgrec;


                                      // ***
                                       // >>> VERIFYUSER stuff
                                       //
#define  DO_VERIFYUSER_MSGSTR    "VERIFYUSER"
#define  OK_VERIFYUSER_MSGSTR    "GOODVERIFYUSER"
#define  BAD_VERIFYUSER_MSGSTR   "NOVERIFYUSER"


                                        // Here don't worry about
                                        // receiving anything past
                                        // the GOODVERIFYUSER (or
                                        // NOVERIFYUSER)
                                        //

#define  SIZE_VERIFYUSER_RESPONSE   512 //for Tony's Change

typedef struct Sverifyuser_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                UserName[SIZE_LOGIN_NAME];
	char                null_tail;

} Sverifyuser_svrmsgrec;


#define  DO_IMPORTENV_MSGSTR    "IMPORTENV"
#define  OK_IMPORTENV_MSGSTR    "GOODIMPORTENV"
#define  BAD_IMPORTENV_MSGSTR   "BADIMPORTENV"

                                        
#define  SIZE_IMPORTENV_RESPONSE     0

typedef struct Simportenv_svrmsgrec
{
    Sbasic_svrmsgrec    bsc;
    char                null_tail;

} Simportenv_svrmsgrec;
///////////////////////////////////////
//  NEW LOCK APP message
                                       // ***
                                       // >>> LOCKAPP stuff
                                       //
#define  DO_LOCKAPP_MSGSTR    "LOCKAPP"
#define  OK_LOCKAPP_MSGSTR    "GOODLOCKAPP"
#define  BAD_LOCKAPP_MSGSTR   "NOLOCKAPP"

                                        
#define  SIZE_LOCKAPP_RESPONSE 0

typedef struct Slockapp_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
    char                App[SIZE_GROUP_TITLE];
    char                Flag[2];
    char                null_tail;

} Slockapp_svrmsgrec;


                                        // ***
                                       // >>> CHECKUCSSN  stuff
                                       //
#define  DO_CHECKUCSSN_MSGSTR    "CHECKUCSSN"
#define  OK_CHECKUCSSN_MSGSTR    "GOODCHECKUCSSN"
#define  BAD_CHECKUCSSN_MSGSTR   "NOCHECKUCSSN"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_CHECKUCSSN_RESPONSE    0

typedef struct Scheckucssn_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                ssn[SIZE_SSN-1]; //11
    char                null_tail;

} Scheckucssn_svrmsgrec ;



                                      // >>> GETUCUSERINFO  stuff
                                       //
#define  DO_GETUCUSERINFO_MSGSTR    "GETUCUSERINFO"
#define  OK_GETUCUSERINFO_MSGSTR    "GOODUCUSERINFO"
#define  BAD_GETUCUSERINFO_MSGSTR   "NOUCUSERINFO"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_GETUCUSERINFO_RESPONSE    0

typedef struct Sgetucuserinfo_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	char                user[SIZE_LOGIN_NAME];
    char                null_tail;

} Sgetucuserinfo_svrmsgrec;




                                      // >>> SETPROCLIMIT  stuff
                                      //
#define  DO_SETPROCLIMIT_MSGSTR    "SETPROCLIMIT"
#define  OK_SETPROCLIMIT_MSGSTR    "GOODSETPROCLIMIT"
#define  BAD_SETPROCLIMIT_MSGSTR   "NOSETPROCLIMIT"

                                        // This will be a
                                        //   CntcssMEM_MSG_ADVALLOC_Msg
                                        //   so will get response size
                                        //   from socket
                                        //
#define  SIZE_SETPROCLIMIT_RESPONSE    0

typedef struct setproclimit_svrmsgrec 
{
    Sbasic_svrmsgrec    bsc;
	TCHAR               hostOrApp[SIZE_APP_NAME];
	TCHAR               flag[2];
	TCHAR               limit[10];
    char                null_tail;

} setproclimit_svrmsgrec;


 

#endif  // _INCLUDE_MSGDEFS_H_CLASS_