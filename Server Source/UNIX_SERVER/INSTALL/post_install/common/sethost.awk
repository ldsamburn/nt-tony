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
# AWK program to read hostname file and create name entries     #
# in /h/NTCSSS/init_data/progrps.ini and               #
# change all server name entries in progrps file to current     #
# host name                                                     #
#                                                               #
#     To execute:    awk -f sethost.awk {progrps file}          #
#

BEGIN {
    print "sethost.awk"
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

    gps=""binlok"\/GetProfileString NTCSS_SYSTEM_SETTINGS ";

    cmd=gps"NTCSS_PROGRP_ID_FILE xxx /etc/ntcss_system_settings.ini > progrpslok";
    system(cmd);
    eof = getline < progrpslok;
    progrpfile = $0;

#    cmd=binlok"/GetProfileString NTCSS_SYSTEM_SETTINGS NTCSS_INIT_HOSTS_FILE xxx /etc/ntcss_system_settings.ini > hosts_file_lok";
#    system(cmd);
#    eof = getline < hosts_file_lok;
#    hostlok=$0;
#    host_file=home"/"hostlok;

    system( "grep `hostname` /etc/hosts | egrep -v '^#' > hostname.dat" );
    dat_file = "hostname.dat"

    progrpstmp = "progrps_tmp.ini"
    eof = getline < dat_file;
    hostname = $2;

    cmd=binlok"/WriteProfileString HOSTS HOST_1 HOST_1 "progrpfile;
    system(cmd);
    cmd=binlok"/WriteProfileString HOST_1 NAME "$2" "progrpfile;
    system(cmd);
    cmd=binlok"/WriteProfileString HOST_1 IPADDR "$1" "progrpfile;
    system(cmd);
    cmd=binlok"/WriteProfileString HOST_1 TYPE \"Master Server\" "progrpfile;
    system(cmd);
    cmd=binlok"/WriteProfileString HOST_1 REPL NO "progrpfile;
    system(cmd);
}

{
   eof = 1;
   while ( 1 ) {
       eof = getline < progrpfile;
       if ( eof <= 0 )
           break;
       if( $0 ~ "hostname" ) {
           print "SERVER="hostname > progrpstmp;
       } else
	  print $0 > progrpstmp;
   } # end while #
}

END {
    cmd="cp progrps_tmp.ini "progrpfile;
    system(cmd);
    #system( "cp progrps_tmp.ini /h/NTCSSS/init_data/progrps.ini" );
    system( "rm hostname.dat progrps_tmp.ini" );
}
