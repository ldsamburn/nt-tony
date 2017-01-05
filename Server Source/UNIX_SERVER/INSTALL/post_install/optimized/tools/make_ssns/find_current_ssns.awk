#---------------------------------------------------------------------#
#                                                                     #
# AWK program to read ntcss_users file and create a file of currently #
# used ssns and create a file "current_ssns_tmp" in the database      #
# directory.                                                          #
#                                                                     #
#     To execute:    awk -f find_ssns.awk {ntcss_users file}          #
#                                                                     #
#---------------------------------------------------------------------#

BEGIN {
    print "find_current_ssns.awk"
    home="/h/NTCSSS";
    progrpslok=home"\/init_data/progrps.ini";
    hosts_file_lok="/etc/hosts";
    bin_lok=home"\/bin";
    current_ssns_tmp = home"\/database/current_ssns_tmp"
    current_ssns_sorted = home"\/database/current_ssns_sorted"
    ntcss_users = home"\/database/ntcss_users";
    cmd = "rm -f "home"\/database/current_ssns_tmp";
    system( cmd );
}

{
    eof = 1;
    while ( 1 ) {

# UNIX ID
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       unixid = $0;
# LOGIN ID
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       loginid = $0;
# REAL NAME
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       realname = $0;
# LOGIN PASSWORD
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       loginpwd = $0;
# SOCIAL SECURITY NUMBER
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       ssn = $0;
# TELEPHONE NUMBER
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       telno = $0;
# SECURITY CLASSIFICATION
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       securityclass = $0;
# PASSWORD CHANGED TIME
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       pwdchangetime = $0;
# USER ROLE
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       userrole = $0;
# USER LOCKED BY SYSTEM MONITOR FLAG
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       userlockedflag = $0;
# DB PASSWORD
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       dbpwd = $0;
# AUTH SERVER
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       auth_server = $0;
# BLANK LINE
       eof = getline < ntcss_users;
       if ( eof <= 0 )
           break;
       blank = $0;

       print ssn >> current_ssns_tmp;

       cmd = "sort "current_ssns_tmp" > "home"\/database/current_ssns_sorted";
       system( cmd );

       break;

   } # end while #
}

END {
    print " ";
    print "The sorted SSN list file is located in ";
    print home"\/database/current_ssns_sorted";
    print " ";
}
