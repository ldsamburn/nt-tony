# AWK program to read the host file and add the IP into the host file     #
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
# in /h/NTCSSS/database/hosts                                             #
#                                                            #
#     To execute:    awk -f setip.awk {dbdir/hosts file}       #
#

BEGIN {
    host_file= "/h/NTCSSS/database/hosts"
    host_tmp = "/h/NTCSSS/database/hosts_tmp"
    system( "grep `hostname` /etc/hosts | egrep -v '^#' > hostname.dat" );
    dat_file = "hostname.dat"
    eof = getline < dat_file;
    print $2" "$1 > host_file;
    hostname = $2;
    system("rm -f /h/NTCSSS/database/hosts_tmp");
    system("touch /h/NTCSSS/database/hosts_tmp");
}

{
   print hostname > host_tmp
   print $2 > host_tmp
   print "30" > host_tmp
   print "0" > host_tmp
   print "0" > host_tmp
}

END {
    system( "cp "host_tmp" "host_file );
}
