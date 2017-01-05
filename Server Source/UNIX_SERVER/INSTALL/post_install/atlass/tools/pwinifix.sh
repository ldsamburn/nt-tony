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

# pwinifix.sh
#
# Run as upgrade script to correct passwords when
# upgrading from 803 to 806 releases (1.0.7 -> 3.0.4)

settings="/etc/ntcss_system_settings.ini"
ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
log="$ntcssdir/post-install.log"

#master_name="$1"

#if [ -z "$master_name" ]
#then
#    echo "No master name argument supplied to pwinifix.sh" >> $log
#    echo "No master name argument supplied to pwinifix.sh"
#    exit -1
#fi

{
while (true)
do
    read line
    if [ $? -ne 0 ]
    then
        echo "Done."
        exit 0
    fi
    echo "$line" | grep "login_name"
    if [ $? = "0" ]
    then
        pwd=`echo $line | cut -d= -f2`
    fi
    echo "$line" | grep "password"
    if [ $? = "0" ]
    then
        line="password=$pwd"
    fi
#    echo "$line" | grep "auth_serve"
#    if [ $? = "0" ]
#    then
#        line="auth_serve=$master_name"
#    fi
    echo $line >> newini
done
} < ./pwdfix.ini
