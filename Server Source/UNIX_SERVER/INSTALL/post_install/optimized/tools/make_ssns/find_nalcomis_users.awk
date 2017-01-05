#---------------------------------------------------------------------#
#                                                                     #
# AWK program to read appusers file and create a file of NALCOMIS     #
# users in the database directory named "nalcomis_users_tmp"          #
#                                                                     #
#     To execute:    awk -f find_nalcomis_users.awk {appusers file}   #
#                                                                     #
#---------------------------------------------------------------------#

BEGIN {
    print "find_nalcomis_users.awk"
    home="/h/NTCSSS";
    progrpslok=home"/h/NTCSSS/init_data/progrps.ini";
    hosts_file_lok="/etc/hosts";
    bin_lok=home"\/bin";
    nalcomis_users_tmp = home"\/database/nalcomis_users_tmp"
    appusers = home"\/database/appusers";
    cmd = "rm -f "home"\/database/nalcomis_users_tmp";
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

       if ( application ~ "NALCOMIS" )
           print ":"login_id":" >> nalcomis_users_tmp;

       break;

   } # end while #
}

END {
    print " ";
    print "The NALCOMIS users list file is located in ";
    print home"\/database/nalcomis_users_tmp";
    print " ";
}
