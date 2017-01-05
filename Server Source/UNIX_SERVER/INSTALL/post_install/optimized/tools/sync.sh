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

#USAGE:"
#
#    ./sync.sh {REMOVE} {NP}
#
#    PURPOSE:
#
#    To remove user home directories and mail files
#    for users that have been deleted from the /etc/passwd file.
#
#    ARGUMENTS:
#
#    (1) If the 'REMOVE' argument is supplied,
#        the un-sync'd user's home directory will be removed and
#        the corresponding mail entry in 'var/spool/mail' (9x)
#        or 'var/mail' (10x) will also be removed.
#
#    (2) If argument 2 is 'NP', a prompt will not be issued
#        before removal.
#
#    OUTPUTS:
#
#    Results are written to the /h/USERS/UNSYNCED_DIR file (10.x), or
#                               /u/UNSYNCED_DIR (9.x).
#
#    EXCEPTIONS: 
#
#    If the user's name is found in the
#    /h/NTCSSS/init_data/tools/sync_exceptions file (10.x), or,
#    /usr/local/NTCSSII/init_data/tools/sync_exceptions file (9.x),
#    the home directory will not be removed.
#

usage()
{
    echo " "
    echo "USAGE:"
    echo " "
    echo "    ./sync.sh {REMOVE} {NP}"
    echo " "
    echo "    PURPOSE:"
    echo " "
    echo "    To remove user home directories and mail files"
    echo "    for users that have been deleted from the /etc/passwd file."
    echo " "
    echo "    ARGUMENTS:"
    echo " "
    echo "    (1) If the 'REMOVE' argument is supplied,"
    echo "        the un-sync'd user's home directory will be removed and"
    echo "        the corresponding mail entry in 'var/spool/mail' (9x)"
    echo "        or 'var/mail' (10x) will also be removed."
    echo " "
    echo "    (2) If argument 2 is 'NP', a prompt will not be issued"
    echo "        before removal."
    echo " "
    echo "    OUTPUTS:"
    echo " "
    echo "    Results are written to the $log file."
    echo " "
    echo "    EXCEPTIONS: "
    echo " "
    echo "    If the user's name is found in the"
    echo "    $sef file,"
    echo "    the home directory will not be removed."
    echo " "
}

initialize()
{
    curdir=`pwd`
    changes="no"
    sef=$ntcssdir/init_data/tools/sync_exceptions
    touch $sef

    if [ "$PLATFORM" = "HP-UX" -a "$VERSION_1" = "09" ]
    then
        maildir=$MAILCMD
        log="/u/UNSYNCED_DIR"
        userdir="/u"
    else
        maildir=$MAILCM 
        log="/h/USERS/UNSYNCED_DIR"
        userdir="/h/USERS"
    fi

    rm -f $log
    touch $log
}

analyze_args()
{
    REMOVE_DIR=$arg1
    if [ "$arg1" = "REMOVE" -o "$arg1" = "remove" ]
    then
        REMOVE_DIR="REMOVE"
    else
        REMOVE_DIR="NO"
    fi

    if [ "$arg2" = "NP" -o "$arg2" = "np" ]
    then
        Q="NO"
    else
        Q="YES"
    fi
}

remove_user_dir()
{
    rm -r $userdir/$i
    rm -f $maildir/$i
    echo "......Directory and Mail have been removed for $i." >> $log
    echo "......Directory and Mail have been removed for $i."
    echo " "
}

#------------------------------
# Script starts here          !
#------------------------------

arg1=${1}
arg2=${2}


    settings="/etc/ntcss_system_settings.ini"

    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    
    bindir=`grep "NTCSS_BIN_DIR" "$settings" | cut -d= -f2`
    bindir="$ntcssdir/$bindir"

    gps="$bindir/GetProfileString"

    appinst=`$gps Strings APP_INST "applications/data/install" $settings`
    appinst="$ntcssdir/$appinst"

    postdir="$appinst/post_install"

. $appinst/getOSinfo.sh

initialize

if [ -z "${1}" ]
then
    usage
    exit 0
fi

analyze_args

cd $userdir

for i in `ls`
do
    if [ -d "${i}" ]
    then
        # Check for skip
        uent=`grep "^${i}\$" $sef`
        stat=$?
        if [ "$stat" = "0" ]
        then
            echo " "
            echo "...Skipping Directory $i" >> $log
            echo "...Skipping Directory $i"
            continue
        fi
        # Check if in passwd file
        uent=`grep "^${i}:" /etc/passwd`
        stat=$?
        if [ "$stat" = "1" ]
        then
           # Not found in passwd file
           changes="yes"
           echo " "
           echo "...User $i has a home directory, but is not in the passwd file"
           echo "...User $i has a home directory, but is not in the passwd file">> $log
           if [ $REMOVE_DIR = "REMOVE" ]
           then
               if [ $Q = "YES" ]
               then
                   echo "Remove User home directory: $i (Y/N)??"
                   read prompt
                   echo " "
                   if [ $prompt = "Y" -o $prompt = "y" ]
                   then
                       remove_user_dir
                   fi
               else
                   remove_user_dir
               fi
           fi
        fi
    fi
done

echo " "
if [ $changes = "yes" ]
then
    echo "NOTE: Users were detected with no passwd entries."
    echo "NOTE: Users were detected with no passwd entries." >> $log
else
    echo "No users have been detected that are out of sync with the passwd file."
    echo "No users have been detected that are out of sync with the passwd file." >> $log
fi

cd $curdir

