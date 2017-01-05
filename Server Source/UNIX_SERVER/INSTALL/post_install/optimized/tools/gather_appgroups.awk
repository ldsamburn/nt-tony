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
# gather_appgroups.awk                                             #
#                                                                  #
# AWK program to read the appusers file and write the NTCSS        #
# application groups to which a user is associated into the        #
# output file: home/init_data/userapps.out                         #
#                                                                  #
# Format: loginid:application                                      #
#                                                                  #
# To execute:    awk -f gather_appgroups.awk {appusers file}       #
#                                                                  #
#------------------------------------------------------------------#

BEGIN {
    print "gather_appgroups.awk";
    home="/h/NTCSSS";
    log_file=home"\/post-install.log";
    progrpslok=home"\/init_data/progrps.ini";
    hosts_file_lok="/etc/hosts";
    bin_lok=home"\/bin";
    userapps_out = home"\/init_data/tools/userapps.out"
    appusers = home"\/database/appusers";
    # Define work files
    # Get current working directory
    cwd_file = "/tmp/cwd.tmp";
    cmd = "pwd > /tmp/cwd.tmp";
    system( cmd );
    eof = getline < cwd_file;
    cwd = $0;
    # Remove old work files
    cmd = "rm -f "home"\/init_data/tools/userapps.out";
    system( cmd );
    # Create new work files
    cmd = "touch "home"\/init_data/tools/userapps.out";
    system( cmd );
    sortcmd = "sort -u "home"\/init_data/tools/userapps.out > userapps.sorted";
    appxref = home"\/init_data/tools/appxref.out";
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
       #print "...Processing: " login_id;
       printf( "." );
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

      {
          eof1 = 1;
          while ( 1 ) {
#print "IN SUB, APPLICATION: "application
              eof1 = getline < appxref;
              if ( eof1 <= 0 )
                  break;
#print "JUST READ RECORD, 0:("$0") 1:("$1") 2:("$2")" 
              if ( $1 == application ) {
#print "READING "$0" UNIXGRP: "$2
                  unixgrp = $2;
                  break;
              }
          }
      }
      close appxref;

      print login_id":"unixgrp > userapps_out;
      break;

   } # end while #
}

END {
    system ( sortcmd );
    print " ";
    print "The users apps file is located in ";
    print home"\/init_data/tools/userapps.out";
    print " ";
    print "The sorted user apps file is located in ";
    print home"\/init_data/tools/userapps.sorted";
    print " ";
}
