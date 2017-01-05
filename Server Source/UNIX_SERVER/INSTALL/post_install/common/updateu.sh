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

ntcssdir=/h/NTCSSS
bindir=$ntcssdir/bin

log="$ntcssdir/post-install.log"
echo "" > $ntcssdir/BAD_PASSWD.LST
echo "Attempting update of user directories with new .login" >> $log
echo "Attempting update of user directories with new .login"

CSHRC=$ntcssdir/applications/data/unix_uga/shell/.cshrc
CSHLOGIN=$ntcssdir/applications/data/unix_uga/shell/.login
cdur=`pwd`

if [ $1 = "y" ]
then
    echo "Inhouse detected, skipping .cshrc replacement" >> $log
    echo "Inhouse detected, skipping .cshrc replacement"
else
    echo "Not inhouse, replacing .cshrc" >> $log
    echo "Not inhouse, replacing .cshrc"
fi

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

	cd /u
	HDIR="/u"
else
	if [ ! -d "/h/USERS" ]
	then
		exit 0
	fi
	cd /h/USERS
	HDIR="/h/USERS"
fi

total=`ls -1 | wc | sed -n "s/^[     ]*\([^    ]*\).*/\1/p"`
totaldone=`expr 0`

for i in `ls`  
do
    if [ -d "${i}" ] 
	then
		totaldone=`expr $totaldone + 1`
		$bindir/display_number $i $totaldone $total
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
                      # Just in case the in-house user has messed with it
                      if [ ! -f /skip_updateu ]
		      then
                          # Check inhouse flag
                          if [ $1 = "n" ]
                          then
           	              cp -p $CSHRC $i
	    	              chown $i ${HDIR}/${i}/.cshrc
                          fi
                          # Copy new .login, it just needs to be there
           	          cp -p $CSHLOGIN $i
	    	          chown $i ${HDIR}/${i}/.login
                          if [ $HDIR = "/u" ]
                          then
                              rm -f $i/.logout
                          fi
                      fi
		  fi
        	fi
    fi
done
echo " "
cd $cdur
