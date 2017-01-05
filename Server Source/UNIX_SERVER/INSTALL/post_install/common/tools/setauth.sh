#!/bin/sh
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

# setauth.sh
#
# Run as upgrade script to set auth_server to "arg-1" when
# upgrading from 803 to 806 releases (1.0.7 -> 3.0.4)
# Must be run after 1st prime_db.

settings="/etc/ntcss_system_settings.ini"
ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
log="$ntcssdir/post-install.log"

master_name="$1"

if [ -z "$master_name" ]
then
    echo "No master name argument supplied to setauth.sh" >> $log
    echo "No master name argument supplied to setauth.sh"
    exit -1
fi

rm -f ntcss_users.tmp
touch ntcss_users.tmp

{
while (true)
do
    read "uid" 
    if [ $? -ne 0 ]
    then
        echo "Done."
        break
    fi
    echo "$uid" >> ntcss_users.tmp
    read "userid"
    echo "$userid"
    echo "$userid" >> ntcss_users.tmp
    read "longname"
    echo "$longname" >> ntcss_users.tmp
    read "login_pw"
    echo "$login_pw" >> ntcss_users.tmp
    read "ssn"
    echo "$ssn" >> ntcss_users.tmp
    read "telno"
    echo "$telno" >> ntcss_users.tmp
    read "zero1"
    echo "$zero1" >> ntcss_users.tmp
    read "time"
    echo "$time" >> ntcss_users.tmp
    read "zero2"
    echo "$zero2" >> ntcss_users.tmp
    read "zero3"
    echo "$zero3" >> ntcss_users.tmp
    read "dbpwd"
    echo "$dbpwd" >> ntcss_users.tmp
    read "blank_auth_server"
    echo "$master_name" >> ntcss_users.tmp
    read "spacer"
    echo "$spacer" >> ntcss_users.tmp
done
} < ./ntcss_users

cwd=`pwd`
cd $ntcssdir/database
cp -p ntcss_users ntcss_users.no_auth
cd $cwd
cp ntcss_users.tmp $ntcssdir/database/ntcss_users

echo "Done."
exit 0
