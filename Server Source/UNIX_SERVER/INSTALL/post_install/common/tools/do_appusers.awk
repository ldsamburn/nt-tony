#
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
#
# AWK program to read appusers file and delete entries          #
# for users that match an input string.                         #
#                                                               #
#     To execute:    awk -f do_appusers.awk {appusers file}     #
#

BEGIN {
    print "do_appusers.awk"
    progrpslok="progrpslok";
    ntcss_home_dir="ntcss_home_dir";
    hosts_file_lok="hosts_file_lok";
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

    appuserstmp = home"\/database/new_appusers"
    appusers = home"\/database/appusers";

    cmd = home"\/init_data/tools/getaline.sh";

    print " ";
    print "Enter the application group to delete:";
    print " ";
    cmd | getline;
    appgroup = $0;
    print "The applications group: "appgroup" will be deleted...";
    print " ";
}

{
   eof = 1;
   while ( 1 ) {
       eof = getline < appusers;
       if ( eof <= 0 )
           break;
       save_group = $0;
       if( $0 ~ appgroup ) {
           for ( i=0;i<7;i++ ) {
               eof = getline < appusers;
           }
           print "Application group <"save_group"> deleted";
       }
       else {
	   print $0 > appuserstmp;
           for ( i=0;i<7;i++ ) {
              eof = getline < appusers;
	      print $0 > appuserstmp;
	   }
       }
   } # end while #
}

END {
    cmd="rm -f bin_dir_lok";
    system(cmd);
    cmd="rm -f ntcss_home_dir";
    system(cmd);
    print " ";
    print "The modified appusers file is located in ";
    print home"\/database/new_appusers";
    print " ";
}
