/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.COMMON;

public class CPMessageHeaders
{
   //!!! NOTE: The number of characters in a header string cannot be greater
   //          than MessageHeader.headerSize !!!

   // Messages dealing with server being up or down
   public static final String GET_SERVER_STATUS = new String("GET_SERVER_STATUS");
   public static final String DAEMONS_UP = new String("DAEMONS_UP");
   public static final String DAEMONS_DOWN = new String("DAEMONS_DOWN");
   public static final String START_SERVER = new String("START_SERVER");
   public static final String HALT_SERVER = new String("HALT_SERVER");

   // Messages dealing with server being idle or awake
   public static final String GET_SERVER_STATE = new String("GET_SERVER_STATE");
   public static final String DAEMONS_AWAKE = new String("DAEMONS_AWAKE");
   public static final String DAEMONS_IDLE = new String("DAEMONS_IDLE");
   public static final String AWAKEN_SERVER = new String("AWAKEN_SERVER");
   public static final String IDLE_SERVER = new String("IDLE_SERVER");

   // Messages dealing with the server's debug state
   public static final String GET_SERVER_DEBUG_STATE = new String("GET_SRV_DBG_STATE");
   public static final String DEBUG_ON = new String("DEBUG_ON");
   public static final String DEBUG_OFF = new String("DEBUG_OFF");
   public static final String TURN_DEBUG_ON = new String("TURN_DEBUG_ON");
   public static final String TURN_DEBUG_OFF = new String("TURN_DEBUG_OFF");

   // Messages dealing with logins into the Ntcss server
   public static final String GET_LOGIN_STATE = new String("GET_LOGIN_STATE");
   public static final String LOGINS_ALLOWED = new String("LOGINS_ALLOWED");
   public static final String LOGINS_DISALLOWED = new String("LOGINS_DISALLOWED");
   public static final String ENABLE_LOGINS = new String("ENABLE_LOGINS");
   public static final String DISABLE_LOGINS = new String("DISABLE_LOGINS");

   // Messages dealing with the NTCSS server's type
   public static final String GET_SERVER_TYPE = new String("GET_SERVER_TYPE");
   public static final String MASTER_SVR = new String("MASTER_SVR");
   public static final String AUTH_SVR = new String("AUTH_SVR");
   public static final String APP_SVR = new String("APP_SVR");
   public static final String UNKNOWN_SVR_TYPE = new String("UNKNOWN_SVR_TYPE");
   public static final String MAKE_NTCSS_MASTER = new String("MAKE_NTCSS_MSTR");
   public static final String MAKE_NTCSS_AUTH = new String("MAKE_NTCSS_AUTH");
   public static final String MAKE_NTCSS_APP = new String("MAKE_NTCSS_APP");

   // INI creation messages
   public static final String CREATE_STATUS_INI_FILE = new String("CREATE_STAT_INI");
   public static final String CREATE_LOGIN_HISTORY_INI_FILE = new String("CREATE_LGN_HSTRY_INI");
   public static final String CREATE_SYSTEM_LOG_INI_FILE = new String("CREATE_SYS_LOG_INI");
   public static final String CREATE_COMMON_DATA_INI_FILE = new String("CREATE_CD_INI");
   public static final String GET_INI_DIRECTORY = new String("GET_INI_DIRECTORY");

   // Misc. messages
   public static final String GET_NUM_CURRENT_USERS = new String("GET_NUM_CUR_USRS");
   public static final String GET_HOST_UP_TIME = new String("GET_HOST_UP_TIME");
   public static final String GET_NTCSS_UP_TIME = new String("GET_NTCSS_UP_TIME");
   public static final String MULTICAST_PING = new String("MCPING");
   public static final String PING = new String("PING");
   public static final String KILL = new String("KILL");
   public static final String SET_COMMON_DATA = new String("SET_COMMON_DATA");
   public static final String START_NDS_SERVER = new String("START_NDS_SERVER");
   public static final String STOP_NDS_SERVER = new String("STOP_NDS_SERVER");
   public static final String RUN_NDS_PROGRAM = new String("RUN_NDS_PROG");

   // Response messages
   public static final String SUCCESSFUL = new String("SUCCESSFUL");
   public static final String ERROR = new String("ERROR");
}