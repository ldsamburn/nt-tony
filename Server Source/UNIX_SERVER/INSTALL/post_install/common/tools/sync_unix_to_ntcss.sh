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

# build_user_dirs.sh
#
# For each user in ntcss_users, build an appropriate directory in
# "userdir", check passwd and group file and add if necessary.

settings="/etc/ntcss_system_settings.ini"
ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
log="$ntcssdir/post-install.log"
echo "Executing build_user_dirs.sh" >> $log
userdir="/h/USERS"
uc="$ntcssdir/applications/data/users_conf/user_config"
csh="$ntcssdir/applications/data/unix_uga/shell/.cshrc"
login="$ntcssdir/applications/data/unix_uga/shell/.login"
profile="$ntcssdir/applications/data/unix_uga/shell/.profile"

if [ `hostname` != `ypwhich -m passwd.byname` ]
then
    echo "This is not the master server" > $log
    echo "This is not the master server"
    exit -1
fi

echo "Executing build_user_dirs.sh" >> $log
echo "Executing build_user_dirs.sh"

{
while (true)
do
    read uid 
    if [ $? -ne 0 ]
    then
        echo "Done."
        break
    fi
    read userid
    echo "Checking $userid"
    read longname
    read login_pw
    read ssn
    read telno
    read zero1
    read time
    read zero2
    read zero3
    read dbpwd
    read auth_server
    read spacer

    user_in_pw=`grep "^$userid:" /etc/passwd`
    if [ -z "${user_in_pw}" ]
    then
        echo "...$userid not in /etc/passwd file" >> $log
        echo "...$userid not in /etc/passwd file"
        $uc ADDUSER "$userid" "$userid" "$userdir" "$userid-added by script"
        echo "...$userid added to /etc/passwd file" >> $log
        echo "...$userid added to /etc/passwd file"
    else
        echo "...OK in Password file" >> $log
        echo "...OK in Password file"
    fi

    GROUPS=`groups $userid | grep atlass` 
    if [ $? != "0" ]
    then
        $uc ADDGROUP $userid atlass
        echo "...Added to groups for atlass" >> $log
        echo "...Added to groups for atlass"
    else
        echo "...OK in Group file" >> $log
        echo "...OK in Group file"
    fi    

    if [ ! -d "/h/USERS/$userid" ]
    then
        echo "...Building Directory" >> $log
        echo "...Building Directory"
        /bin/mkdir "$userdir/$userid"
        cp -p "$csh" "$userdir/$userid"
        cp -p "$login" "$userdir/$userid"
        cp -p "$profile" "$userdir/$userid"
        touch "$userdir/$userid/.mailrc"
        chown -R "$userid" "$userdir/$userid"
        chgrp -R ntcss "$userdir/$userid"
        chmod -R 700 "$userdir/$userid"
    else
        echo "...Directory OK" >> $log
        echo "...Directory OK"
    fi

done
} < /h/NTCSSS/database/ntcss_users

echo "Done." >> $log
echo "Done."
exit 0
