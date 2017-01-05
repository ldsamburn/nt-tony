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
#
# update the .cshrc in each directory of /u with the NTCSS II version and
# delete the .profile which brings up the old /u/admin/sysadm script
#

log="/h/NTCSSS/post-install.log"
echo "" > /h/NTCSSS/BAD_PASSWD.LST
echo "Updating user directories with new .login and .cshrc" >> $log
echo "Updating user directories with new .login and .cshrc"

CSHRC=/h/NTCSSS/applications/data/unix_uga/shell/.cshrc
CSHLOGIN=/h/NTCSSS/applications/data/unix_uga/shell/.login
cdur=`pwd`

if [ -d "/u" ]
then
	# don't want to process mount /u dir
    	mountdir=`grep "/u" /etc/mnttab`
    	if [ -n "${mountdir}" ]
        # /u found in mount table, and is on another machine
    	then
		echo "...Not processing /u, it is a mounted directory" >> $log
		echo "...Not processing /u, it is a mounted directory"
                echo "...and not on this machine." >> $log
                echo "...and not on this machine."
		exit 0
    	fi

        echo "Updating /u directory" >> $log
        echo "Updating /u directory"

	cd /u
	HDIR="/u"
else
	if [ ! -d "/h/USERS" ]
	then
		exit 0
	fi

        echo "Updating /h/USERS directory" >> $log
        echo "Updating /h/USERS directory"

	cd /h/USERS
	HDIR="/h/USERS"
fi

for i in `ls`  
do
    if [ -d "${i}" ] 
    then
      	echo "Processing Directory: "$i
	uent=`grep "^${i}:" /etc/passwd`
	gs=$?
        if [ $gs -eq 2 ]
	then
            echo "...User $i has a home directory, but is not in" >> $log
            echo ".....the /etc/passwd file" >> $log
            echo "...User $i has a home directory, but is not in"
            echo ".....the /etc/passwd file"
            echo "...Check /h/NTCSSS/BAD_PASSWD.LST"
            echo ".....for listing" >> $log
            echo "Possible bad entry for $i" >> /h/NTCSSS/BAD_PASSWD.LST 
        else
            if [ -n "$uent" ]
            then
                mv ${HDIR}/${i}/.cshrc ${HDIR}/${i}/cshrc_org
       	        cp -p $CSHRC $i
   	        chown $i ${HDIR}/${i}/.cshrc
       	        cp -p $CSHLOGIN $i
    	        chown $i ${HDIR}/${i}/.login
            fi
        fi
    fi
done
cd $cdur
