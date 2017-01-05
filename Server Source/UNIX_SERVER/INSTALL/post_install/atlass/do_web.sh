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

# do_web.sh
#
# This script is executed after the "web" directory has been moved from
# the 'post_install' directory to /h/NTCSSS/web.
#
# It is responsible for insuring that the /etc/services file is set up for
# httpd at ports 8000 and 8080.
#
# Additionally, it sets the ownership, group membership and permissions
# for the web directory.
#
# It copies the ../conf/httpd.conf file to ../bin/httpd.master.
# When the 'startserver.sh' script is executed, the httpd.master file
# is copied to the ../conf/httpd.conf file prior to starting httpd.
#

echo " "
echo "Installing NTCSS Web services"

hostname=`hostname`
settings="/etc/ntcss_system_settings.ini"
ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
log="$ntcssdir/post-install.log"
echo "Installing NTCSS Web services" >> $log
postdir="$ntcssdir/applications/data/install/post_install"
webdir="$ntcssdir/web"
confdir="$webdir/conf"
webbindir="$webdir/bin"
docdir="$webdir/DocumentRoot"
logdir="$webdir/logs"

if [ -h $docdir/ATLASS/data ]
then
    unlink $docdir/ATLASS/data
fi
if [ -h $docdir/ADHOC/data ]
then
    unlink $docdir/ADHOC/data
fi
if [ -h $docdir/NTCSSU/data ]
then
    unlink $docdir/NTCSSU/data
fi

change_port()
{
ckit=`grep $search_port /etc/services`
if [ -z "$ckit" ]
then
    #No entries for NTCSS http 8000 in services.. add to ntcss section
ed - /etc/services <<EOF
/message_server/
a
http          $port/tcp  www            # NTCSS Web server
http          $port/udp  www            # NTCSS Web server
.
w
q
EOF
    echo "http port $port entries added to /etc/services file" >> $log
    echo "http port $port entries added to /etc/services file"
else
    echo "http port $port already in /etc/services file" >> $log
    echo "http port $port already in /etc/services file"
fi
}

set_perms()
{
# Fix ownership of files
echo " "
echo "NOTE: Any 'chown' and 'chgrp' messages here should be ignored."
echo "      They are caused by links in the directory structure and not"
echo "      harmful."
echo " "
chown -R www $webdir
chgrp -R ntcss $webdir
chown -R root $webbindir
chown -R root $confdir
chmod -R 700 $confdir
chown -R root $logdir
chmod -R 700 $logdir

# For Sybase administration
chmod -R 750 $webbindir
chmod 4710 $webbindir/suexec                            
}

echo "Adding NTCSS web service ports to /etc/services file"
echo "Adding NTCSS web service ports to /etc/services file" >> $log

# For general NTCSS use
search_port="   8000"
port="8000"
change_port

# For Sybase administration
if [ -d /h/NTCSSU/data/syb_web/cgi-bin ]
then
    search_port="   8080"
    port="8080"
    #change_port # Activate for sybase utils on the web
    change_port
fi

cp -p $confdir/httpd.conf $webbindir/httpd.master

echo "Setting Web ownership to 'www'"
echo "Setting Web group to 'ntcss'"
set_perms

echo "NTCSS Web services installed"
echo " "
