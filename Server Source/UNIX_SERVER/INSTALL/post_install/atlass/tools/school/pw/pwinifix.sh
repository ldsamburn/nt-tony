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
# Run as upgrade script to encrypt passwords, and establish auth_server
#
# ARG-1 = slave server name
#

settings="/etc/ntcss_system_settings.ini"
ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
log="$ntcssdir/post-install.log"
hostname=`hostname`
rm -f newini
cwd=`pwd`

slave_server="$1"

if [ -z "$slave_server" ]
then
    slave_server="$hostname"
else
    slave_server=$1
fi

{
while (true)
do
    read line
    if [ $? -ne "0" ]
    then
        exit 0
    fi
    echo "$line" | grep "login_name" 2> /dev/null        # login_name
    if [ $? = "0" ]
    then
        pwd=`echo "$line" | cut -d= -f2` 2> /dev/null
    fi
    echo "$line" | grep "password" 2> /dev/null          # password
    if [ $? = "0" ]
    then
        if [ $pwd != "atlbtusr" ]
        then
            line="password=$pwd"                         # pw = login_id
        else                                             # except atlbtusr
            line="password=ecafretni"                    # which is different
        fi
    fi
    echo "$line" | grep "auth_serve" 2> /dev/null        # auth_serve
    if [ $? = "0" ]
    then
        line="auth_serve=$slave_server"
    fi
    echo "$line" >> newini
#    dot
done
} < $cwd/pwdfix.ini
