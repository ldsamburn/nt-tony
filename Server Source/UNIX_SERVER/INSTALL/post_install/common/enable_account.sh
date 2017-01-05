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

# edit_user.sh
#	ARG-1 = username
#
# give bt users a passwd


    settings="/etc/ntcss_system_settings.ini"

    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    
    bindir=`grep "NTCSS_BIN_DIR" "$settings" | cut -d= -f2`
    bindir="$ntcssdir/$bindir"

    gps="$bindir/GetProfileString"

    appinst=`$gps Strings APP_INST "applications/data/install" $settings`
    appinst="$ntcssdir/$appinst"

    postdir="$appinst/post_install"

. $appinst/getOSinfo.sh


	PWDIR="/etc/passwd"
	username=${1}
        password="3jMK1INsgakxU"

	check_user=`grep "^${username}:" $PWDIR 2> /dev/null`
	if [ -n "$check_user" ]
	then
		check_user=`grep "^${username}:" $PWDIR 2> /dev/null`
       		if [ -n "${SECPWD}" -a -f "${SECPWD}" ]
		then
			check_user_sec=`grep "^${username}:" $SECPWD 2> /dev/null`
		fi

		userpwd=`echo $check_user | cut -d: -f2`
		userid=`echo $check_user | cut -d: -f3`
		usergid=`echo $check_user | cut -d: -f4`
		userd=`echo $check_user | cut -d: -f5`
		userdir=`echo $check_user | cut -d: -f6`
		usershl=`echo $check_user | cut -d: -f7`

       		if [ -n "${SECPWD}" -a -f "${SECPWD}" ]
		then
         		chmod 600 $SECPWD
			userpwd_str=`echo $check_user_sec | cut -d: -f2`
			usersecpwd=`echo $userpwd_str | cut -d, -f1`
			newsecpwd=${password}
			useraid=`echo $check_user_sec | cut -d: -f3`
			useraflag=`echo $check_user_sec | cut -d: -f4`
ed - $SECPWD <<EOF 2>&1 /dev/null
/${username}:/d
a
${username}:${password},443:${useraid}:${useraflag}
.
w
q
EOF
         		chmod 400 $SECPWD
			password="*"
		fi
ed - $PWDIR <<EOF 2>&1 /dev/null
/${username}:/d
a
${username}:${password}:${userid}:${usergid}:${userd}:${userdir}:${usershl}
.
w
q
EOF

		#nispush
	fi

