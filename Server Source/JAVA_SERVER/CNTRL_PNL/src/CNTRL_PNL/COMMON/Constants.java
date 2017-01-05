/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.COMMON;

import java.io.*;

public class Constants
{   
   private static final String SCRIPT_EXTENSION; 
   public static final boolean IS_WINDOWS_OS;
   
   static {
      String OSName = System.getProperty("os.name");
      
      // Determine the operating system 
      if (OSName.toLowerCase().indexOf("windows") != -1) { // i.e. Windows OS
         SCRIPT_EXTENSION = ".bat";     
         IS_WINDOWS_OS = true;
      }
      else {
         SCRIPT_EXTENSION = "";          
         IS_WINDOWS_OS = false;
      }
   }
   
   // Gif files with their path location inside the jar file
   public static final String LOADING_GIF =
                           "/CNTRL_PNL/GUI/ICONS/gears.gif";
   public static final String MASTER_SVR_GIF =
                           "/CNTRL_PNL/GUI/ICONS/comput0c.gif";
   public static final String AUTH_SVR_GIF =
                           "/CNTRL_PNL/GUI/ICONS/key04.gif";
   public static final String APP_SVR_GIF =
                           "/CNTRL_PNL/GUI/ICONS/laptop.gif";
   public static final String UNKNOWN_SVR_GIF =
                           "/CNTRL_PNL/GUI/ICONS/ques2.gif";
   public static final String SERVER_DOWN_GIF =
                           "/CNTRL_PNL/GUI/ICONS/noway.gif";

   // Files and Directories
   private static final String NTCSS_ROOT_DIR = 
                             System.getProperty("ntcss.root_dir", "c:/NTCSSS");
   public static final String NTCSS_LOGS_DIRECTORY =
                             NTCSS_ROOT_DIR + "/logs" + "/";   
   public static final String NTCSS_LDAP_RUNTIME_DIRECTORY =
                             NTCSS_ROOT_DIR + "/ldap/runtime";
   public static final String SVR_INI_DIRECTORY = NTCSS_LOGS_DIRECTORY; 
   
   public static final String SVR_SERVICE_START = NTCSS_ROOT_DIR + 
                               "/bin/ntcsss_svc_start" + SCRIPT_EXTENSION;
   public static final String SVR_SERVICE_STOP = NTCSS_ROOT_DIR + 
                               "/bin/ntcsss_svc_stop" + SCRIPT_EXTENSION;
   public static final String NDS_SERVICE_START = NTCSS_ROOT_DIR + 
                               "/bin/ldap_svc_start" + SCRIPT_EXTENSION;
   public static final String NDS_SERVICE_STOP = NTCSS_ROOT_DIR + 
                               "/bin/ldap_svc_stop" + SCRIPT_EXTENSION;
      
   public static final boolean LOG_TO_SCREEN = 
    ((System.getProperty("cp.echo_logging") == null) ? false :
       (System.getProperty("cp.echo_logging").equalsIgnoreCase("on") ? 
       true : false));
   
   public static final String SVR_LOG_FILE =       
                           NTCSS_LOGS_DIRECTORY + "cp_svr.log";       
   public static final String CLT_LOG_FILE =
             ((System.getProperty("ntcss.root_dir") == null) ? 
                    "cp_clt.log" : (NTCSS_LOGS_DIRECTORY + "cp_clt.log"));                                  

   // Ntcss Control Panel ports
   public static final int CP_SERVER_PORT =
            Integer.parseInt(System.getProperty("cp.server_port", "9182"));

   // Multicast information
   public static final String MULTICAST_IP =
            System.getProperty("cp.multicast_ip", "224.0.0.7");
   public static final int MULTICAST_PORT  =
            Integer.parseInt(System.getProperty("cp.multicast_port", "6789"));
   public static final int MULTICAST_PING_WAIT =
        Integer.parseInt(System.getProperty("cp.multicast_ping_wait", "2000"));

   
   // Ntcss daemon ports
   public static final int PRTQ_SVR_PORT =
            Integer.parseInt(System.getProperty("servers.prtq_svr_port", "9122"));
   public static final int DB_SVR_PORT   =
            Integer.parseInt(System.getProperty("servers.db_svr_port", "9132"));
   public static final int DSK_SVR_PORT  =
            Integer.parseInt(System.getProperty("servers.dsk_svr_port", "9142"));
   public static final int CMD_SVR_PORT  =
            Integer.parseInt(System.getProperty("servers.cmd_svr_port", "9152"));
   public static final int SPQ_SVR_PORT  =
            Integer.parseInt(System.getProperty("servers.spq_svr_port", "9162"));
   public static final int MSG_SVR_PORT  =
            Integer.parseInt(System.getProperty("servers.msg_svr_port", "9172"));
   
   // On Windows NT 4.0, this number has a very large effect on the size of 
   //  "non-paged pool" memory used by the process. Each increment of this 
   //  number increases "non-paged pool" by 16kbytes. If the amount of 
   //  "non-paged pool" memory used is too high, NTCSS batch jobs will not be
   //  able to run - the system reports a ERROR_NOT_ENOUGH_QUOTA Microsoft error.
   public static final int SERVER_BACKLOG_QUEUE_SIZE = 
            Integer.parseInt(System.getProperty("server.backlog_size", "5"));

   // Control Panel client and server versions
   public static String CNTRL_PNL_VERSION = "1.1.0";
   public static String CNTRL_PNL_SVR_VERSION = "2.0.0";

   // NTCSS server types
   public static final int MASTER_SVR       = 0;
   public static final int AUTH_SVR         = 1;
   public static final int APP_SVR          = 2;
   public static final int UNKNOWN_SVR_TYPE = -1;

   // NIS host types
   public static final int NIS_MASTER       = 0x04;
   public static final int NIS_SLAVE        = 0x02;
   public static final int NIS_CLIENT       = 0x01;
   public static final int UNKNOWN_NIS_TYPE = -1;         
}