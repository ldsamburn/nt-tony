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
# AWK program to read hosts file and create name entries        #
# in slaves.ini of all possible slave candidate servers         #
#                                                               #
#     To execute:    awk -f setservers.awk {slaves.ini file}     #
#

BEGIN {
    host_file = "/etc/hosts"
    dat_file = "hostname.dat"
    system( "echo [HOSTS] > /h/NTCSSS/applications/data/install/post_install/slaves.ini" );
    system( "echo `hostname` > hostname.dat" );
    eof = getline < dat_file;
    this_host = $0;
}

{
    eof = 1;
    while ( 1 ) {
       eof = getline < host_file;
       if ( eof <= 0 ) {
           break;
       }
       if ( ($1 !~ "#") && ($2 !~ "loopback") && ($2 != "localhost") && (length($1) > 2) ) {
#       if ( ($1 !~ "#") && ($2 !~ "loopback") && (length($1) > 2) ) {
           cmd="/h/NTCSSS/bin/WriteProfileString HOSTS "$2" "$1" /h/NTCSSS/applications/data/install/post_install/slaves.ini"
           system(cmd);
       }
    } # end while #
}

END {
}
