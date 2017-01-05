#------------------------------------------------------------------#
#                                                                  #
# AWK program to read ntcss_users file and apply unique ssn        #
# entries to an incrementing ssn to keep unique-ness               #
#                                                                  #
#     To execute:    awk -f do_ssns.awk {access_auth file}         #
#                                                                  #
#------------------------------------------------------------------#

BEGIN {
    print "do_ssns.awk"
    home="/h/NTCSSS";
    progrpslok=home"\/init_data/progrps.ini";
    hosts_file_lok="/etc/hosts";
    bin_lok=home"\/bin";
    ntcss_users_tmp = home"\/database/ntcss_users_tmp"
    ntcss_users = home"\/database/ntcss_users";
    duplicates = home"\/database/ssn_duplicates_tmp";
    dupcountfile = home"\/database/dupcountfile";
    ssn4=0000;
    dupcount=00;
    dupbump=0;
    ssn = "000-00-0000";
    cmd = "rm -f "home"\/database/ntcss_users_tmp";
    system( cmd );
    cmd = "rm -f "home"\/database/ssn_duplicates_tmp";
    system( cmd );
    eof = getline < dupcountfile;
    if ( eof <= 0 ) {
        dupcount=01;
    }
    else {
        dupcount=$0;
        dupcount++;
    }
    print dupcount > dupcountfile;
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

       if ( (loginid ~ "ntcssii") || (loginid ~ "btusr") )
           ssn2 = ssn;
       else {
           cmd = "grep :"loginid": "home"\/database/nalcomis_users_tmp 2>&1 /dev/null";
           result = system( cmd );
           if ( result !~ 0 ) {
               # Not a NALCOMIS user
               if ( ssn ~ "000-00-0000" ) {
                   # Regular user, assign ssn to user 
                   # First see if new ssn would be a duplicate
                   testssn = ssn4 +1;
                   cmd = "grep "testssn" "home"\/database/current_ssns_sorted";
                   result = system( cmd );
                   if ( result !~ 0 ) {
                       # Not a duplicate
                       ssn4++;     # regular user, increment ssn
                       ssn2 = sprintf( "000-00-%04i", ssn4 );
                   }
                   else {
                       # Duplicate, use alternate
                       print loginid": "ssn >> duplicates;
                       ssn5++;     # bump alternate ssn counter, right 4 digits
                       ssn2 = sprintf( "000-%02i-%04i", dupcount, ssn5 );
                   }
               }
               else
                   # Regular user, non-zero ssn
                   ssn2 = ssn; # regular user, non-zero ssn, carry-over
           }
           else
               ssn2 = ssn; # nalcomis user, carry over ssn
       }

#       print "UNIXID:  "unixid;
#       print "LOGINID: "loginid;
#       print "REALNAME:"realname;
#       print "LOGINPWD:"loginpwd;
#       print "SSN2:    "ssn2;
#       print "TELNO:   "telno;
#       print "SEC:     "securityclass;
#       print "PWCHANGE:"pwdchangetime;
#       print "ROLE:    "userrole;
#       print "LOCKED:  "userlockedflag;
#       print "DBPWD:   "dbpwd;
#       print "AUTH:    "auth_server;
#       print "";

       print unixid >> ntcss_users_tmp;
       print loginid >> ntcss_users_tmp;
       print realname >> ntcss_users_tmp;
       print loginpwd >> ntcss_users_tmp;
       print ssn2 >> ntcss_users_tmp;
       print telno >> ntcss_users_tmp;
       print securityclass >> ntcss_users_tmp;
       print pwdchangetime >> ntcss_users_tmp;
       print userrole >> ntcss_users_tmp;
       print userlockedflag >> ntcss_users_tmp;
       print dbpwd >> ntcss_users_tmp;
       if ( length(auth_server) < 2 )
           auth_server = hostname;
       print auth_server >> ntcss_users_tmp;
       print "" >> ntcss_users_tmp;
       break;

   } # end while #
}

END {
    print " ";
    print "The modified ntcss_users file is located in ";
    print home"\/database/ntcss_users_tmp";
    print " ";
}
