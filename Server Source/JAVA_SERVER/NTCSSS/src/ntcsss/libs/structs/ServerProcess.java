/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package ntcsss.libs.structs;

/**
 * Represents information about a server process 
 */
public class ServerProcess
{
   public String  process_id;       // MAX_PROCESS_ID_LEN
   public int     pid;
   public int     priority;
   public String  command_line;     // MAX_PROC_CMD_LINE_LEN+1
   public int     proc_status;
   public String  cust_proc_status; // MAX_CUST_PROC_STAT_LEN+1
   public String  login_name;       // MAX_LOGIN_NAME_LEN+1
   public String  unix_owner;       // MAX_LOGIN_NAME_LEN+1
   public String  progroup_title;   // MAX_PROGROUP_TITLE_LEN+1
   public String  job_descrip;      // MAX_JOB_DESCRIP_LEN+1
   public String  request_time;     // DATETIME_LEN
   public String  launch_time;      // DATETIME_LEN
   public String  end_time;         // DATETIME_LEN
   public int     security_class;
   public String  orig_host;        // MAX_ORIG_HOST_LEN+1
   public String  prt_name;         // MAX_PRT_NAME_LEN+1
   public String  hostname;         // MAX_HOST_NAME_LEN+1
   public int     prt_sec_class;
   public String  prt_filename;     // MAX_PRT_FILE_LEN+1
   public int     prt_flag;
   public String  sdi_ctl_rec;      // MAX_SDI_CTL_RECORD_LEN+1
   public String  sdi_data_file;    // MAX_PRT_FILE_LEN+1
   public String  sdi_tci_file;     // MAX_PRT_FILE_LEN+1
   public String  sdi_device;       // MAX_PRT_FILE_LEN+1
   public int     copies;
   public int     banner;
   public int     orientation;
   public int     papersize;
   
   public ServerProcess()
   {
   }
   
   public ServerProcess(String _process_id, int _pid, int _priority, 
                        String _command_line, int _proc_status, String
                        _cust_proc_status, String _login_name, String _unix_owner,
                        String _progroup_title, String _job_descrip, 
                        String _request_time, String _launch_time, 
                        String _end_time, int _security_class, String _orig_host, 
                        String _prt_name, String _hostname, int _prt_sec_class,
                        String _prt_filename, int _prt_flag, String _sdi_ctl_rec,
                        String _sdi_data_file, String _sdi_tci_file, 
                        String _sdi_device,int _copies, int _banner, 
                        int _orientation, int _papersize)
   {
       
       process_id = _process_id;
       pid = _pid;
       priority = _priority;
       command_line = _command_line;
       proc_status = _proc_status;
       cust_proc_status = _cust_proc_status;
       login_name = _login_name;
       unix_owner = _unix_owner;
       progroup_title = _progroup_title;
       job_descrip = _job_descrip;
       request_time = _request_time;
       launch_time = _launch_time;
       end_time = _end_time;
       security_class = _security_class;
       orig_host = _orig_host;
       prt_name = _prt_name;
       hostname = _hostname;
       prt_sec_class = _prt_sec_class;
       prt_filename = _prt_filename;
       prt_flag = _prt_flag;
       sdi_ctl_rec = _sdi_ctl_rec;
       sdi_data_file = _sdi_data_file;
       sdi_tci_file = _sdi_tci_file;
       sdi_device = _sdi_device;
       copies = _copies;
       banner = _banner;
       orientation = _orientation;
       papersize = _papersize;
       
   }
                        
   
   
} ///:~
