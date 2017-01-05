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

# rebuild_users.sh
# 01/31/2000

#######################################################################
#
# This script reads the /h/NTCSSS/database/ntcss_users file
# and re-creates the user in Unix and NT.  It is to be used to fix
# catastrophic failure in the NT or Unix domain, where users are lost, and a
# re-synching between NTCSS and NT/Unix is required.
# To be run only on the NTCSSII master server.
#
# It does not create Unix groups except for 'ntcss'.
#
# If a 'use_last_ip" exists under the root directory, this script will
# use the last IP of the NT domain controller that was entered.
#
# It will skip users if they are listed in the file: 
# /h/NTCSSS/init_data/tools/skip_updates_list
# By default, the user 'sybase' is entered into this file.
#
# If present, ARG-1 or 2  = no_nt to avoid pushes to the NT domain.
#             ARG-1 or 2  = nodomain to add users to the local workstation
#                          for testing purposes.
#             ARG-1       = if not "no_nt" or "nodomain", can be the time
#                           in seconds for NT delay time in which in to add
#                           users. The default is 10 seconds.
#
#######################################################################

    clear
    echo " ";echo " "
    echo "rebuild_users.sh"
    echo " "

    delay1=5
    delay2=15
    ntcssdir="/h/NTCSSS"
    ntcss_bin_dir="$ntcssdir/bin"
    PATH=$PATH:${ntcss_bin_dir};export PATH
    gps="$ntcss_bin_dir/GetProfileString"
    wps="$ntcss_bin_dir/WriteProfileString"
    appdir="$ntcssdir/applications"
    appinst="$appdir/data/install"
    postdir="$appinst/post_install"
    dbdir="$ntcssdir/database"
    ucdir="$appdir/data/users_conf"
    progrps_file="$ntcssdir/init_data/progrps.ini"
    init_data_dir="$ntcssdir/init_data"
    tools_dir="$init_data_dir/tools"
    uc_prog="$appdir/data/users_conf/user_config"
    user_dir="/h/USERS"
    sdi_dir="$appdir/data/srv_dev"
    webdir="$ntcssdir/web"
    shelldir="$appdir/data/unix_uga/shell"
    push_to_nt="1"
    DOMAIN_FLAG="DOMAIN"
    auth_server=`hostname`

    #Do not process the users that are in the update_exceptions file
    skip_file=$tools_dir/skip_updates_list
    if [ -f "$skip_file" ]
    then
        skip_file_exists="y"
    else
        skip_file_exists="n"
    fi

    TELLNT="YES"
    TELLPWD="NO"

    NTCSS_GRP="ntcss"
    PWFILE="/etc/passwd"
    GRPFILE="/etc/group"
    NTSHELL="/bin/csh"
    NTDIRPRE="$user_dir"
    DEF_NTDIR="d:\ntusers"

get_ip()
{
    echo "Enter the IP address of the Workstation where the NTCSSII desktop"
    echo "is executing as the NT Domain Administrator... (nnn.nnn.nnn.nnn)"
    echo " "
    read disp < /dev/tty
    echo $disp > lastip
    echo " "
}

nispush()
{
    if [ `ypwhich -m passwd.byname` = `hostname` ]
    then
        curdir=`pwd`
        cd $NISDIR
        /bin/make -f Makefile
        cd $curdir
    fi
}

do_unix()
{
    # Insure user is in /etc/passwd file.
#    echo "Processing User -> "$username
    check_user=`grep "^${username}:" $PWFILE 2> /dev/null`
    if [ $? = "0" ]
    then
        #User found in passwd file
#        echo "USER: <$username> already in passwd file"
        userpwd=`echo $check_user | cut -d: -f2`
        userid=`echo $check_user | cut -d: -f3`
        usergid=$ntcss_gid
        userdir=$user_dir/$username
        usershl=$NTSHELL
        if [ -z "$longname" ]
        then
            longname="N/A"
        fi
        chmod 644 $PWFILE

        ed - $PWFILE <<EOF 2>&1 > /dev/null
/${username}:/d
a
${username}:${userpwd}:${userid}:${usergid}:${longname}:${userdir}:${usershl}
.
w
q
EOF
        echo "------>User $username ($longname) modified in passwd file"
    else
        #User not in passwd file
        userdir=$user_dir/$username
        $uc_prog ADDUSER $username "*" "$user_dir" "$longname" "$username" "$auth_server"
        if [ -z "$longname" ]
        then
            longname="N/A"
        fi
        echo "------>User $username ($longname) added to passwd file"
    fi

    #Create user directory if it doesn't exist
    #echo "DOING PERMISSIONS ON $userdir FOR USER $username"
    if [ ! -d "${userdir}" ]
    then
        echo "      >Creating user directory"
        mkdir -p ${userdir}
    fi

    echo "      >Setting permissions on directory ${userdir}.."
    chown ${username} ${userdir}
    chmod 755 ${userdir}
    chgrp ${NTCSS_GRP} ${userdir}

    #New .cshrc
    cp ${shelldir}/.cshrc ${userdir}
    chown ${username} ${userdir}/.cshrc 
    chgrp ${NTCSS_GRP} ${userdir}/.cshrc
    chmod 774 ${userdir}/.cshrc
    echo "      >New .cshrc created in directory ${userdir}.."

    if [ ${VERSION_1} != "10" ]
    then
        chmod 400 $SECPWD
    fi
    #Add user to application groups
    grpname=`grep "^$username:" $tools_dir/userapps.sorted`
    if [ $? = "0" ]
    then
        grpname=`echo "$grpname" | cut -d: -f2` 
        for i in `echo $grpname`
        do
            if [ $i != "ntcss" ]
            then
                groupthere=`grep "^$i:" $GRPFILE`
                if [ $? != "0" ]
                then
                    #User not in group file
                    newgroup=$i
                    add_group_to_unix
                fi
                $uc_prog ADDGROUP $username $i
                echo "      >User $username added to $i group in $GRPFILE file.."
            fi
        done
    else
        echo " "
        echo "   ...>User: $username is not a member of any application groups!"
        echo " "
    fi
}

do_nt()
{
    #Do NT registration

    #echo "disp:        <$disp>"
    #echo "username:    <$username>"
    #echo "userdesc:    <$userdesc>"
    #echo "ntdir:       <$ntdir>"
    #echo "tmpuser:     <$tmpuser>"
    #echo "ntprof:      <$ntprof>"
    #echo "DOMAIN_FLAG: <$DOMAIN_FLAG>"
    #echo "auth_server: <$auth_server>"
    #echo "      >Adding user to NT, $delay1 second delay.."
    #sleep $delay1
    #echo "      >NT adduser command:"
    #echo "      .....IP address: ${disp}"
    #echo "      .....Username:   $username"
    #echo "      .....Longname:   $longname"
    #echo "      .....Directory:  ${ntdir}\\${username}"
    #echo "      .....Template:   ${tmpuser}"
    #echo "      .....Profile:    ${ntprof}\\${username}"
    #echo "      .....domain:     $DOMAIN_FLAG"
    $ntcss_bin_dir/adduser "${disp}" /ADDEXT "$username" "$username" "$longname" ${ntdir}\\${username} "${tmpuser}" "${ntprof}\\${username}" $DOMAIN_FLAG "$auth_server"
    echo "      >Add user command sent to NT with $DOMAIN_FLAG, $delay2 second delay.."
    sleep $delay2
#    $ntcss_bin_dir/ws_exec "${disp}" "c:\ntcss2\bin\nltest.exe"
}

finish()
{
    cp -p $dbdir/ntcss_users $dbdir/ntcss_users.old
    cp $dbdir/ntcss_users.new $dbdir/ntcss_users
    echo "Processing for ntcss_users complete."
}

add_group_to_unix()
{
    grp=`grep "$newgroup:" $GRPFILE`
    if [ $? = "0" ]
    then
        echo "$newgroup group already in $GRPFILE file"
        group_gid=`grep "$newgroup:" $GRPFILE | cut -d: -f3`
    else
        echo "      >Adding $newgroup group to $GRPFILE"
        gh=`sort -t: +2n -3 $GRPFILE | tail -1 | cut -d: -f3`
        group_gid=`expr ${gh} + 1`
        ed - $GRPFILE <<EOF 2>&1 > /dev/null
$
a
$newgroup:*:${group_gid}:
.
w
q
EOF
    echo "      >$newgroup group added to /etc/group"
    fi
}

#---- MAIN -----


    settings="/etc/ntcss_system_settings.ini"

    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    
    bindir=`grep "NTCSS_BIN_DIR" "$settings" | cut -d= -f2`
    bindir="$ntcssdir/$bindir"

    gps="$bindir/GetProfileString"

    appinst=`$gps Strings APP_INST "applications/data/install" $settings`
    appinst="$ntcssdir/$appinst"

    postdir="$appinst/post_install"

. $appinst/getOSinfo.sh


    if [ -n "$1" ]
    then
        if [ "$1" = "help" -o "$1" = "-help" -o "$1" = "HELP" \
              -o "$1" = "-HELP" ]
        then
            echo "This utility uses the NTCSS database (ntcss_users and"
            echo "appusers) to rebuild:"
            echo " "
            echo "   1. Users in the Unix /etc/passwd and /etc/group files."
            echo "   2. The Unix group associated with the NTCSS applications"
            echo "      as needed."
            echo "   3. The User's home directory in Unix as needed."
            echo "   4. The User in the NT domain by default."
            echo "      This may be avoided by providing the arguments below."
            echo "      Note: The user is given a password which is the same as"
            echo "      their user login ID in NT and their Authentication"
            echo "      Server will be this host."
            echo " "
            echo "COMMAND ARGUMENTS:"
            echo " "
            echo "    ./rebuild_users.sh            (rebuild on NT and DOMAIN"
            echo "                                   normal execution)"
            echo "    ./rebuild_users.sh -no_nt     (do not rebuild in NT)"
            echo "    ./rebuild_users.sh -nodomain  (rebuild in NT but only"
            echo "                                   on local workstation)"
            echo "    ./rebuild_users.sh -help      (this help dialog)"
            echo "    ./rebuild_users.sh {NT delay time}"
            echo "                                  (The time delay allowed for"
            echo "                                   NT to register the user)"
            exit 0
        fi
    fi

    #Check for the existance of required files..
    echo "Checking for required files.."
    echo " "
    if [ ! -f $tools_dir/gather_appgroups.awk ]
    then
        echo " "
        echo "Need $tools_dir/gather_appgroups.awk file to"
        echo "execute this process.."
        echo " "
        exit 1
    fi
    if [ ! -x getapps.sh ]
    then
        echo " "
        echo "Need $tools_dir/getapps.sh file to"
        echo "execute this process.."
        echo " "
        exit 1
    fi
    echo "Required files exist.."
    echo " "

    rm -f $dbdir/ntcss_users.new
    touch $dbdir/ntcss_users.new


    #Default NT wait time
    delay2="10"
    if [ -n "$1" ]
    then
        if [ "$1" = "nodomain" -o "$1" = "-nodomain" -o "$1" = "NODOMAIN" \
             -o "$1" = "-NODOMAIN" ]
        then
            DOMAIN_FLAG="NODOMAIN"
            echo "DOMAIN FLAG set to $DOMAIN_FLAG.."
            echo " "
        fi
        if [ "$1" = "no_nt" -o "$1" = "-no_nt" -o "$1" = "NO_NT" \
             -o "$1" = "-NO_NT" ]
        then
            push_to_nt="0"
            echo "NT registration set to No registration.."
            echo " "
        fi
        if [ "$DOMAIN_FLAG" != "NODOMAIN" -a "$push_to_nt" != "0" ]
        then
            echo "NT delay set at $1 seconds.."
            echo " "
            delay2="$1"
        fi
    fi

    if [ -n "$2" ]
    then
        if [ "$2" = "no_nt" -o "$2" = "-no_nt" -o "$2" = "NO_NT" \
             -o "$2" = "-NO_NT" ]
        then
            push_to_nt="0"
        fi
        if [ "$2" = "nodomain" -o "$2" = "-nodomain" -o "$2" = "NODOMAIN" \
             -o "$2" = "-NODOMAIN" ]
        then
            DOMAIN_FLAG="NODOMAIN"
        fi
    fi

    if [ $push_to_nt = "1" ]
    then
        if [ -f /use_last_ip ]
        then
            if [ -f lastip ]
            then
                read disp < lastip
            else
                get_ip
            fi
        else
            get_ip
        fi
        echo "Testing capability to communicate with the NT domain.."
        echo " "
        $ntcss_bin_dir/ws_exec "$disp" "clock.exe"
        echo "Did a clock just appear on the NT system designated by the"
        echo "above IP address?"
        echo " "
        read x
        if [ "$x" = "y" -o "$x" = "Y" -o "$x" = "yes" -o "$x" = "YES" ]
        then
            echo " "
        else
            echo " "
            echo "Communications with the NT system is not functioning.."
            echo " "
            exit 1
        fi
    fi

    #Get user's NT home directory from SYS_CONF
    sys_line=`grep NTUSERDIR ${dbdir}/SYS_CONF`
    if [ -n "${sys_line}" ]
    then
        ntdir=`echo ${sys_line} | sed -n "s/[^:]*.[ ]*\(.*\)$/\1/p"`
    else
        ntdir="d:\\ntusers"
    fi

    #Template user
    tmpuser=""
    sys_line=`grep NTTMPUSER ${dbdir}/SYS_CONF`
    if [ -n "${sys_line}" ]
    then
        tmpuser=`echo ${sys_line} | sed -n "s/[^:]*.[ ]*\(.*\)$/\1/p"`
        if [ -z "${tmpuser}" ]
        then
            tmpuser="IT-21"
        fi
    fi

    #Profiles directory
    ntprof=""
    sys_line=`grep NTPROFDIR ${dbdir}/SYS_CONF`
    if [ -n "${sys_line}" ]
    then
        ntprof=`echo ${sys_line} | sed -n "s/[^:]*.[ ]*\(.*\)$/\1/p"`
        if [ -z "${ntprof}" ]
        then
            ntprof="/winnt/profiles"
        fi
    fi

    #Get NTCSS App groups to Unix group conversion table
    echo "Creating NTCSS App to Unix group conversion table.."
    echo " "
    $tools_dir/getapps.sh

    echo "Creating Unix group membership from NTCSS appusers file.."
    echo " "
    awk -f $tools_dir/gather_appgroups.awk $dbdir/appusers

    #Get ntcss group id
    echo "Validating ntcss group in Unix.."
    echo " "
    ntcss=`grep "^ntcss:" /etc/group`
    if [ $? = "0" ]
    then
        ntcss_gid=`grep "^ntcss:" /etc/group | cut -d: -f3`
        echo "ntcss group valid in /etc/group file with GID of $ntcss_gid.."
    else
        newgroup="ntcss"
        add_group_to_unix
    fi

    echo " "
    echo "Processing NTCSS ntcss_users file.."
    echo " "
    {
    while (true)
    do

	read uid

	if [ $? -ne 0 ]
	then
            finish
            break
	fi

	read username
	read longname
	read apppwd
	read ssn
	read phone
	read class
	read pwchange
	read dbreg
	read role
	read shpwd
        read auth_server
        read seperater

	if [ -z "$username" ] 
	then
            finish
            break
	fi

        if [ -f /TEST_ABORT ]
        then
            echo "Process gracefully aborted.."
            rm -f /TEST_ABORT
            finish
            break
        fi

        if [ $skip_file_exists = "y" ]
        then
            skip=`grep ^$username$ $skip_file`
        fi

        if [ -z "$skip" ]
        then
            do_unix
            #Write user to new ntcss_users file to update unix id
            newuid=`grep $username":" $PWFILE | cut -d: -f3`
            echo $newuid >> $dbdir/ntcss_users.new
            echo $username >> $dbdir/ntcss_users.new
            echo $longname >> $dbdir/ntcss_users.new
            echo $apppwd >> $dbdir/ntcss_users.new
            echo $ssn >> $dbdir/ntcss_users.new
            echo $phone >> $dbdir/ntcss_users.new
            echo $class >> $dbdir/ntcss_users.new
            echo $pwchange >> $dbdir/ntcss_users.new
            echo $dbreg >> $dbdir/ntcss_users.new
            echo $role >> $dbdir/ntcss_users.new
            echo $shpwd >> $dbdir/ntcss_users.new
            echo $auth_server >> $dbdir/ntcss_users.new
            echo $seperater >> $dbdir/ntcss_users.new
        else
            echo ".......> Unix processing for '$username' skipped.."
        fi

        if [ $push_to_nt = "1" ]
        then
            #Add to NT
            do_nt
        fi

    done } < $dbdir/ntcss_users

exit 0

