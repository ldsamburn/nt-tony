#------------------------------------------------------------------#
#
#
#              Copyright(c)2002 Northrop Grumman Corporation
#
#                          All Rights Reserved
#
#
#    This material may be reproduced by or for the U.S. Government pursuant
#    to the copyright license under the clause at Defense Federal Acquisition
#    Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
#
#
#                                                                  #
# AWK program to identify applications to which users are          #
# currently assigned.                                              #
#                                                                  #
# Create rsupply_users_tmp to contain all rsupply users            #
# Create ommsng_users_tmp to contain all ommsng users              #
# Create radm_users_tmp to contain all radm users                  #
# Create navedi_users_tmp to contain all edi users                 #
#                                                                  #
# To execute:    awk -f identify_appusers.awk {appusers file}      #
#                                                                  #
#------------------------------------------------------------------#

BEGIN {
    print "identify_appusers.awk";
    ntcss_home_dir="ntcss_home_dir";
    bin_dir_lok="bin_dir_lok";
    settings="/etc/ntcss_system_settings.ini";

    cmd="grep ^HOME_DIR= "settings" | cut -d= -f2 > ntcss_home_dir";
    system(cmd);
    eof = getline < ntcss_home_dir;
    home=$0;

    cmd="grep NTCSS_BIN_DIR "settings" | cut -d= -f2 > bin_dir_lok";
    system(cmd);
    eof = getline < bin_dir_lok;
    binlok=home"/"$0;

    log_file=home"/post-install.log";

    appusers_tmp = home"/database/appusers_tmp"
    appusers = home"/database/appusers";

    # Define work files
    rsupply_users_tmp = home"/database/rsupply_users_tmp";
    ommsng_users_tmp = home"/database/ommsng_users_tmp";
    radm_users_tmp = home"/database/radm_users_tmp";
    navedi_users_tmp = home"/database/navedi_users_tmp";
    nalcomis_users_tmp = home"/database/nalcomis_users_tmp";

    # Get current working directory
    cwd_file = "/tmp/cwd.tmp";
    cmd = "pwd > /tmp/cwd.tmp";
    system( cmd );
    eof = getline < cwd_file;
    cwd = $0;

    # Remove old work files
    cmd = "rm -f "home"/database/appusers_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/rsupply_users_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/ommsng_users_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/radm_users_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/nalcomis_users_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/admin_done_tmp";
    system( cmd );

    # Create new work files
    cmd = "touch "home"/database/nalcomis_users_tmp";
    system( cmd );
    cmd = "rm -f "home"/database/navedi_users_tmp";
    system( cmd );
    cmd = "touch "home"/database/admin_done_tmp";
    system( cmd );
    cmd = "touch "home"/database/rsupply_users_tmp";
    system( cmd );
    cmd = "touch "home"/database/ommsng_users_tmp";
    system( cmd );
    cmd = "touch "home"/database/radm_users_tmp";
    system( cmd );
}

{
   eof = 1;
   while ( 1 ) {

# APPLICATION
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       application = $0;
# LOGIN ID
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       login_id = $0;
       print "...Processing: " login_id;
# LONG NAME
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       long_name = $0;
# APPLICATION DATA 
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       app_data = $0;
# APP PASSWORD
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       app_password = $0;
# APP ROLE
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       app_role = $0;
# REGISTERED FLAG
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       registered_flag = $0;
# BLANK LINE
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
#print application;
#print login_id;
#print long_name;
#print app_data;
#print app_password;
#print app_role;
#print registerd_flag;
#print $0;

      if( application ~ "RSUPPLY" ) {
          print login_id > rsupply_users_tmp;
      }
      if( application ~ "OMMSNG" ) {
          print login_id > ommsng_users_tmp;
      }
      if( application ~ "RADM" ) {
          print login_id > radm_users_tmp;
      }
      if( application ~ "NALCOMIS" ) {
          print login_id > nalcomis_users_tmp;
      }
      if( application ~ "NAVEDI" ) {
          print login_id > navedi_users_tmp;
      }

   } # end while #
}

END {
    print "identify_appusers.awk: Processing complete.";
}
