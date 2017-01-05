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
#=========================================================================
#
# "do_post_install.sh"
#
# FOR ATLASS
#
# Last modified: 10/12/2001
#                Copied u2n_convert & nalc_conv from bin directory to
#                tools directory to insure correct compiled version
#                is in the tools directory for the OS. djp.
# 
# 05/03/1999
#	* The "REPOSITORY_NAME" was being removed from SYS_CONF
#	  Had to ask again, and put it back..
#	  Will be in 806-01.01.11
#
#=========================================================================

initialize()
{
    echo "do_post_install.sh"

    settings="/etc/ntcss_system_settings.ini"

    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    log="$ntcssdir/post-install.log"
    echo "NTCSS Home Directory: <$ntcssdir>" >> $log
#    echo "NTCSS Home Directory: <$ntcssdir>"

    ntcss_bin_dir=`grep NTCSS_BIN_DIR "$settings" | cut -d= -f2`
    ntcss_bin_dir="$ntcssdir/$ntcss_bin_dir"
    echo "NTCSS Bin directory: <$ntcss_bin_dir>" >> $log
#    echo "NTCSS Bin directory: <$ntcss_bin_dir>"

    PATH=$PATH:${ntcss_bin_dir};export PATH

    gps="$ntcss_bin_dir/GetProfileString"
    wps="$ntcss_bin_dir/WriteProfileString"

    #---applications--
    appdir="$ntcssdir/applications"
    echo "NTCSS Application Directory: <$appdir>" >> $log
#    echo "NTCSS Application Directory: <$appdir>"

    #---install-------
    appinst=`$gps Strings APP_INST "applications/data/install" $settings`
    appinst="$ntcssdir/$appinst"
    echo "NTCSS Install Directory: <$appinst>" >> $log
#    echo "NTCSS Install Directory: <$appinst>"

    #---post_install---
    postdir="$appinst/post_install"
    echo "NTCSS PI Directory: <$postdir>" >> $log
#    echo "NTCSS PI Directory: <$postdir>"

    #---database---
    dbdir=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_DB_DIR "/h/NTCSSS/database" $settings`
    echo "NTCSS Database Directory: <$dbdir>" >> $log
#    echo "NTCSS Database Directory: <$dbdir>"

    #---users_conf---
    ucdir=`$gps NTCSS_SYSTEM_SETTINGS USER_CONFIG_DIR "applications/data/users_conf" $settings`
    ucdir=$ntcssdir/$ucdir
    echo "NTCSS User Config Directory: <$ucdir>" >> $log
#    echo "NTCSS User Config Directory: <$ucdir>"

    #---progrps.ini---
    progrps_file=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_PROGRP_ID_FILE "/h/NTCSSS/init_data/progrps.ini" $settings`
    echo "NTCSS Program Groups file: <$progrps_file>" >> $log
#    echo "NTCSS Program Groups file: <$progrps_file>"

    #---init_data---
    init_data_dir=`$gps Strings INIT_DATA "init_data" $settings`
    init_data_dir="$ntcssdir/$init_data_dir"
    echo "NTCSS Init Data Directory: <$init_data_dir>" >> $log
#    echo "NTCSS Init Data Directory: <$init_data_dir>"

    #---user_config (full path)---
    uc_prog=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_CHANGE_USER_PROGRAM "/h/NTCSSS/applications/data/users_conf/user_config" $settings`
    echo "NTCSS User Config Program:" >> $log
    echo "      <$uc_prog>" >> $log
#    echo "NTCSS User Config Program:"
#    echo "      <$uc_prog>"

    #---Unix user's directory---
    user_dir=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_UNIX_USERS_DIR "/h/USERS" $settings`
    echo "NTCSS User's directory: <$user_dir>" >> $log
#    echo "NTCSS User's directory: <$user_dir>"

    #---Server device interface directories---
    sdi_dir=`$gps NTCSS_SYSTEM_SETTINGS SDI_DIR "applications/data/srv_dev" $settings`
    sdi_dir="$ntcssdir/$sdi_dir"
    echo "NTCSS SDI Directory: <$sdi_dir>" >> $log
#    echo "NTCSS SDI Directory: <$sdi_dir>"

    #---ntcss_cmd---
    ntcsscmd=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_CMD_PROG "ntcss_cmd" $settings`
    echo "NTCSS CMD Name: <$ntcsscmd>" >> $log
#    echo "NTCSS CMD Name: <$ntcsscmd>"

    #---web directory---
    webdir="$ntcssdir/web"
    echo "Web Directory: <$webdir>" >> $log
#    echo "Web Directory: <$webdir>"

    opt="y"
    inhouse="n"
    master_server="n"
    keepnt="n"
    keepip="n"
    hostname=`hostname`

    roothd=`grep ^root /etc/passwd | cut -d: -f 6`
    echo "root home directory: <$roothd>" >> $log
    echo "root home directory: <$roothd>"


    # Get release number from the "build_date" file to allow CM
    # to change as needed.
    echo "Install date: `date`" > /tmp/post-install.log
    if [ -f $postdir/build_date ]
    then
       cat /tmp/post-install.log $postdir/build_date >> $log
       cat $postdir/build_date
    fi 

#
#  source in the getOSinfo.sh to obtain Operating System
#  information about HP-UX 10.2, 11, Linux, SunOS.
#
. $appinst/getOSinfo.sh
                 

    if [ ! -f /upgrade ]
    then
        echo " "; echo " "; echo " "
        echo "Is this a master server? [Y/N]"
        read ans
        if [ $ans = "n" -o $ans = "N" ]
        then 
            echo " "
            echo "   Enter the name of the master server:"
            echo " "
            read master_name
            echo " "
        else
            master_server="y"
            master_ip=`grep $hostname /etc/hosts | cut -d" " -f1`
            echo "This is a master server" >> $log
            echo "This is a master server"
            echo " "; echo " "
        fi 
    else
        echo "This is an UPGRADE..." >> $log
        echo "This is an UPGRADE..."
    fi

    hostname=`hostname`

    if [ -f /keepnt ]
    then
        echo "Keeping NT configuration parameters.." >> $log
        echo "Keeping NT configuration parameters.."
        keepnt="y"
    fi

    if [ -f /keepip ]
    then
        echo "Keeping IP configuration parameters.." >> $log
        echo "Keeping IP configuration parameters.."
        keepip="y"
    fi

    if [ $master_server = "y" ]
    then
        if [ ! -f /keep_pg ]
        then
            keep_pg="n"
            echo "New progrps.ini file will be loaded on ($hostname)" >> $log
            echo "New progrps.ini file will be loaded on ($hostname)"
        else
            echo "Old progrps.ini will be kept on ($hostname)." >> $log
            echo "Old progrps.ini will be kept on ($hostname)."
        fi
    fi

    if [ -f /force_tac4cm ]
    then
        echo "Force tac4cm Detected, (hostname=tac4cm)" >> $log
        echo "Force tac4cm Detected, (hostname=tac4cm)"
        hostname="tac4cm"
    fi

    if [ -f /inhouse ]
    then
        echo "Inhouse Detected" >> $log
        echo "Inhouse Detected"
        inhouse="y"
    fi
}

#-------------------------------------------------------------------

create_netrc()
{
    echo "Configuration for $master_server not done yet." >> $log
    echo "Configuration for $master_server not done yet."
    echo " "
    echo "machine $master_name" >> $roothd/.netrc
    echo "login root" >> $roothd/.netrc
    echo "password $rootpw" >> $roothd/.netrc
    chmod 700 $roothd/.netrc
    echo "$roothd/.netrc created" >> $log
    echo "$roothd/.netrc created"
    echo " "
}

verifyip()
{
        ipstat="good"
	if [ -z "$ipaddr" ]
	then
		echo "No IP"
                ipstat="bad"
		break
	fi

	echo "Processing IP = "$ipaddr
	
	chk=`echo $ipaddr | sed -n "s/[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*/Good/p"`
	if [ $chk = "Good" ]
	then
		echo "IP address format looks good"
	else
		echo "IP address format looks bad"
                ipstat="bad"
		break
	fi

	p1=`echo $ipaddr | cut -d '.' -f1`
	p2=`echo $ipaddr | cut -d '.' -f2`
	p3=`echo $ipaddr | cut -d '.' -f3`
	p4=`echo $ipaddr | cut -d '.' -f4`
	
	if [ $p1 -gt 255 -o $p2 -gt 255 -o $p3 -gt 255 -o $p4 -gt 255 ]
	then
		echo "IP address too Big"
                ipstat="bad"
		break
	fi
	
	if [ $p1 -lt 0 -o $p2 -lt 0 -o $p3 -lt 0 -o $p4 -lt 0 ]
	then
		echo "IP address too Small"
                ipstat="bad"
		break
	fi
}

#-------------------------------------------------------------------
enterip()
{
    while true
    do
	read ipaddr
	verifyip
        if [ $ipstat = "good" ]
        then
            break
        else
            if [ $ipstat = "" -o $ipstat = "q" ]
            then
                ipstat="bad"
                break
            fi
        fi
    done
}

#-------------------------------------------------------------------
sethost()
{
# Configure NTCSSS host/progrps.ini file for server names-ip addresses
    echo "Setting Master server name" >> $log
    echo "Setting Master server name"
    awk -f $postdir/sethost.awk $progrps_file
}

#-------------------------------------------------------------------
set_servers()
{
# Add slaves found in /etc/hosts file to progrps.ini
    echo "Setting Slave server name(s)" >> $log
    echo "Setting Slave server name(s)"
    cp $postdir/slave_master.ini $postdir/slaves.ini
    awk -f $postdir/setservers.awk $postdir/slaves.ini
    $postdir/select_servers.sh $master_ip
}

#-------------------------------------------------------------------
do_prime()
{
  if [ $master_server = "n" ]
  then
      echo "Trying to do prime_db on an NTCSS slave server" >> $log
      echo "Trying to do prime_db on an NTCSS slave server"
      exit -1
  fi
  if [ `hostname` = `ypwhich -m passwd.byname` ]
  then
      # Do prime_db to get groups installed
      echo "Executing prime_db to get groups installed" >> $log
      echo "Executing prime_db to get groups installed"

#      # WARNING: May need to remove this later
      ipline=`ipcs -m | grep 000001f4`
      if [ $? = "0" ]
      then
          #system settings shared memory detected
          echo "$ipline" >> $log
          echo "$ipline"
          echo "Shared memory segment detected, deleting.." >> $log
          echo "Shared memory segment detected, deleting.."
          if [ -n "$ipline" ]
          then
            ipid=`echo $ipline | sed -n "s/m[ 	]*\([^ 	]*\).*/\1/p"`
            if [ -n "$ipid" ]
            then
                echo "Deleting segment ID: <$ipid>" >> $log
                echo "Deleting segment ID: <$ipid>"
                ipcrm -m $ipid
                if [ "$?" != "0" ]
                then
                    echo "Shared memory delete failed, aborting mission.." >> $log
                    echo "Shared memory delete failed, aborting mission.."
                    exit -1
                else
                    echo "Shared memory deleted successfully.." >> $log
                    echo "Shared memory deleted successfully.."
                fi
            fi
          fi
      fi
      PATH=$ntcss_bin_dir:$PATH;export PATH
      $ntcss_bin_dir/prime_db -SSCCinriPRIME
      if [ $? -eq 0 ]
      then
          echo "Prime DB OK" >> $log
          echo "Prime DB OK"
      else
          echo "PRIME_DB FAILED! Aborting mission..." >> $log
          echo "PRIME_DB FAILED! Aborting mission..."
          exit -1
       fi 
  fi
}

#-------------------------------------------------------------------
add_ntcssii()
{
# Create ntcssii user if it doesn't exist
    echo "Creating User ntcssii" >> $log
    echo "Creating User ntcssii"
# Do ypmake to get ntcss group installed
    do_push
    ntcss_op=`grep "^ntcssii" /etc/passwd`
    if [ -z "${ntcss_op}" ]
    then
        echo "User ntcssii was not found in /etc/passwd file" >> $log
        echo "User ntcssii was not found in /etc/passwd file"
        if [ $opt = "y" ]
        then
            echo "add_ntcssii: opt=$opt, ntcssdir=$ntcssdir" >> $log
            echo "add_ntcssii: opt=$opt, ntcssdir=$ntcssdir"
            $ntcssdir/applications/data/users_conf/user_config ADDUSER ntcssii ntcssii /h/USERS "NTCSS Administrator" 
        else
            echo "add_ntcssii: opt=$opt, ntcssdir=$ntcssdir" >> $log
            echo "add_ntcssii: opt=$opt, ntcssdir=$ntcssdir"
            $ntcssdir/applications/data/users_conf/user_config ADDUSER ntcssii ntcssii /u "NTCSS Administrator" 
        fi
        echo "UserConfig status is "$? >> $log
        echo "User ntcssii has been added to /etc/passwd file" >> $log
        echo "User ntcssii has been added to /etc/passwd file"
    else
        echo "User ntcssii was already in /etc/passwd file" >> $log
        echo "User ntcssii was already in /etc/passwd file"
    fi
}

#-------------------------------------------------------------------
add_www()
{
# Create user for NTCSS Web Server
    echo "Creating User www" >> $log
    echo "Creating User www"
    ntcss_op=`grep "^www:" /etc/passwd`
    if [ -z "${ntcss_op}" ]
    then
        echo "User www was not found in /etc/passwd file" >> $log
        echo "User www was not found in /etc/passwd file"
        $uc_prog ADDUSER www u505505 $user_dir "NTCSS Web start"
        echo "User www has been added to /etc/passwd file" >> $log
        echo "User www has been added to /etc/passwd file"
    else
        echo "User www was already in /etc/passwd file" >> $log
        echo "User www was already in /etc/passwd file"
    fi
}

#-------------------------------------------------------------------
do_backups()
{
    # Save existing state of NTCSSS database
    echo "Current NTCSSS database being backed up to" >> $log
    echo " $postdir/preinstalldb.tar" >> $log
    echo "Current NTCSSS database being backed up to"
    echo " $postdir/preinstalldb.tar"
    tar cvf $postdir/preinstalldb.tar $dbdir
}

#-------------------------------------------------------------------
update_logingroup()
{
    # There must be a link here for the "groups" command to work
    echo "Modifying /etc/logingroup" >> $log
    echo "Modifying /etc/logingroup"
    rm -f /etc/logingroup
    ln -s /etc/group /etc/logingroup
}

#-------------------------------------------------------------------
addsource()
{
    # Add source of coe_cshrc to csh.login
    # This is how slave servers get their environment
    if [ $master_server = "n" ]
    then
        ckit=`grep coe_cshrc /etc/csh.login`
        if [ -z "$ckit" ]
        then
            echo "Adding source of coe_cshrc to csh.login" >> $log
            echo "Adding source of coe_cshrc to csh.login"
            cp /etc/csh.login /etc/csh.login.old
            cat /etc/csh.login.old $postdir/addsource > /etc/csh.login
        else
            echo "coe_cshrc already being sourced in csh.login" >> $log
            echo "coe_cshrc already being sourced in csh.login"
        fi
    fi
}

#-------------------------------------------------------------------
do_atlass()
{
    echo "do_atlass" >> $log
    echo "do_atlass"

    echo "Copying new atlass progrps file to" >>$log
    echo " $init_data_dir" >>$log
    echo "Copying new atlass progrps file to"
    echo " $init_data_dir"
    cp -p $postdir/progrps_atlass.ini $init_data_dir/progrps.ini
 
    # Create SDI directories if needed
    echo "Creating SDI directories" >> $log
    echo "Creating SDI directories"
    if [ ! -d /h/ATLASS/data ]
    then
        mkdir -p /h/ATLASS/data
        chmod -R 777 /h/ATLASS
    fi
#    if [ ! -d /h/ATLASS/data/reports ]
#    then
#        ln -s /h/ATLASS/data /h/ATLASS/data/reports
#    fi
    if [ ! -d /h/Sybase_Utils/data ]
    then
        mkdir -p /h/Sybase_Utils/data
        chmod -R 777 /h/Sybase_Utils
    fi
    if [ ! -d /h/NTCSS_APP/data ]
    then
        mkdir -p /h/NTCSS_APP/data ]
        chmod -R 777 /h/NTCSS_APP
    fi
    if [ ! -d /h/ADHOC/data/reports ]
    then
        mkdir -p /h/ADHOC/data/reports ]
        chmod -R 777 /h/ADHOC
    fi
# Install app_dirs for atlass class
    cp -p $postdir/app_dirs_atlass $full_sdi_dir/app_dirs
}

do_nmaster()
{
#default load
    do_atlass
}

do_tac4cm()
{
#default load for CM
    touch /save_output_types
    do_atlass
}

#-------------------------------------------------------------------
do_push()
{
#
#   following 2 lines commented out because of getOSinfo.sh
#
#   vers=`/bin/uname -r`
#   NISDIR=/var/yp
    if [ `ypwhich -m passwd.byname` = `hostname` ]
    then
        if [ $master_server = "n" ]
        then
            echo "This NTCSS slave is currently the NIS master!!" >> $log
            echo "This NTCSS slave is currently the NIS master!!"
            echo "NIS PUSH NOT PERFORMED!" >> $log
            echo "NIS PUSH NOT PERFORMED!"
        else
            curdir=`pwd`
            cd $NISDIR
	    echo "Executing NIS make" >> $log
	    echo "Executing NIS make"
	    make -f Makefile
	    cd $curdir
        fi
    fi
}

#-------------------------------------------------------------------
init_fs_archive()
{
   echo "Initializing Backup archive info" >> $log
   echo "Initializing Backup archive info"
   curdir=`pwd`
   cd $appdir/data/fs_archive
   tar xvf $postdir/fs_archive_opt.tar
   cd $curdir 
}

#-------------------------------------------------------------------
make_ntcssii_dir()
{
    # Create backups directory, there is an established fs_archive
    # job which uses this directory.
    if [ ! -d $user_dir/ntcssii/backups ]
    then
        echo "Creating ntcssii/backups directory" >> $log
        echo "Creating ntcssii/backups directory"
        mkdir -p $user_dir/ntcssii/backups
        chmod -R 777 $user_dir/ntcssii
    fi
}

#-------------------------------------------------------------------
make_ntcss_dir()
{
    if [ ! -d /h/NTCSS ]
    then
        echo "Creating /h/NTCSS directory" >> $log
        echo "Creating /h/NTCSS directory"
        mkdir -p /h/NTCSS/bin
        mkdir /h/NTCSS/scripts
        mkdir /h/NTCSS/data
        mkdir /h/NTCSS/data/reports
        chmod -R 777 /h/NTCSS
    fi
}

#-------------------------------------------------------------------
do_database_files()
{

# If not master, set MASTER file in database to point to master server 
    if [ $master_server = "n" ]
    then
        echo "Setting master server in database MASTER file to $master_name" >> $log
        echo "Setting master server in database MASTER file to $master_name"
        echo $master_name > $dbdir/MASTER
    fi
}

#-------------------------------------------------------------------
get_nt_wd()
{
    # The following code is to clean up pre-existing SYS_CONF entries..
    ntusrdir=`grep ^NTUSERDIR $dbdir/SYS_CONF`
    nttmpusr=`grep ^NTTMPUSER $dbdir/SYS_CONF`
    ntprofdir=`grep ^NTPROFDIR $dbdir/SYS_CONF`
        
    if [ -n "${ntusrdir}" ]
    then
       # Insure that an entry does not already exist in SYS_CONF for NTUSERDIR..
       ed $dbdir/SYS_CONF <<EOF
/^NTUSERDIR/d
a
NTUSERDIR: NTUSERS
.
w
q
EOF
    else
        # Not currently in SYS_CONF, put it in..
        ed $dbdir/SYS_CONF <<EOF
$
a
NTUSERDIR: NTUSERS
.
w
q
EOF
    fi

    if [ -n "${nttmpusr}" ]
    then
    # Insure that an entry does not already exist in SYS_CONF for NTTMPUSER..
        ed $dbdir/SYS_CONF <<EOF
/^NTTMPUSER/d
a
NTTMPUSER: IT-21
.
w
q
EOF
    else
        ed $dbdir/SYS_CONF <<EOF
$
a
NTTMPUSER: IT-21
.
w
EOF
    fi

    if [ -n "${ntprofdir}" ]
    then
    # Insure that an entry does not already exist in SYS_CONF for NTPROFDIR..
        ed $dbdir/SYS_CONF <<EOF
/^NTPROFDIR/d
a
NTPROFDIR: PROFILES
.
w
q
EOF
    else
        ed $dbdir/SYS_CONF <<EOF
$
a
NTPROFDIR: PROFILES
.
w
EOF
    fi
}

#-------------------------------------------------------------------
get_master_IP()
{
    echo "Enter the IP address for master server.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$wps HOSTS $hostname $ipaddr $progrps_file
    fi
    echo "      "
}

#-------------------------------------------------------------------
get_domain_name()
{
    echo "Getting domain name" >> $log
    echo "Getting domain name"
    defdom="NTCSS"
    echo "	"
    echo "Enter the NT domain name (return for default of NTCSS).."
    echo " "
    echo "    NOTE-1: This determines which domains can login to NTCSS"
    echo "    NOTE-2: You may enter more than one domain and/or "
    echo "            PC name(s) separated by commas"
    echo " "
    domname=`$ntcss_bin_dir/upper`
#    read domname
    if [ -z "$domname" ]
    then
        domname=$defdom
    fi
    echo "Setting NT Domain name to" ${domname} >> $log
    echo "Setting NT Domain name to" ${domname}
    $ntcss_bin_dir/$ntcsscmd comData set NTDOMAIN ${domname} 
}

#-------------------------------------------------------------------
fix_printcap_file()
{
    if [ -f /etc/printcap ]
    then
        echo "Updating /etc/printcap for sd.. to /usr/spool/lp/request/.." >> $log
        echo "Updating /etc/printcap for sd.. to /usr/spool/lp/request/.."
        $init_data_dir/tools/update_pcap
    else
        echo "NO /etc/printcap detected" >> $log
        echo "NO /etc/printcap detected"
    fi
}

#-------------------------------------------------------------------
killprogd()
{
        # This routine is used for multi-unix systems. 
        echo "Killing old progd" >> $log
        echo "Killing old progd"
        ncline=`ps -ef | grep progd | egrep -v grep`
        if [ -n "${ncline}" ]
        then
            ncpid=`echo $ncline | sed -n "s/[^0-9]*\([^     ]*\).*/\1/p"`
            if [ -n "${ncpid}" ]
            then
                kill -9 "$ncpid"
            fi
        fi
        chmod 755 $ntcss_bin_dir/progd
        chmod 755 $ntcss_bin_dir/remsh
        echo "Starting new progd" >> $log
        echo "Starting new progd"
        $ntcss_bin_dir/progd &
}

#-------------------------------------------------------------------
do_rem_start()
{
    # Copy new rem_start to /usr/bin
    echo "Copying rem_start to /usr/bin" >> $log
    echo "Copying rem_start to /usr/bin"
    chmod 755 $ntcss_bin_dir/rem_start
    cp -p $ntcss_bin_dir/rem_start /usr/bin
}

#-------------------------------------------------------------------
do_host_id_file()
{
    echo "[`hostname`]" > $dbdir/HOST_ID
}

#-------------------------------------------------------------------
do_slave()
{
    echo "do_slave" >> $log
    echo "do_slave"

    rm -f $init_data_dir/progrps.ini
    $ntcss_bin_dir/net_xfer get $master_name $init_data_dir/progrps.ini $init_data_dir/progrps.ini
    cp $init_data_dir/progrps.ini $dbdir/progrps.ini
    sleep 2
    touch $init_data_dir/progrps.ini
    
#    ftp $master_name < $postdir/get_progrps.cmd
    if [ -f $init_data_dir/progrps.ini ]
    then
        #Good fetch
        echo "The progrps.ini file has been retrieved from $master_name" >> $log
        echo " "
        echo "The progrps.ini file has been retrieved from $master_name"
        echo " "
    else
        #Bad fetch
        echo " " >> $log
        echo "WARNING !!" >> $log
        echo "The File transfer of the progrps.ini file from $master_name" >> $log
        echo "Failed.  The file: $init_data_dir/progrps.ini must be" >> $log
        echo "transferred to this system in the same directory." >> $log
        echo " " >> $log
        echo " "
        echo "WARNING !!"
        echo "The Automatic File transfer of the progrps.ini file from $master_name"
        echo "Failed.  The file: $init_data_dir/progrps.ini from $master_name must be"
        echo "manually transferred to this system in the same directory."
        echo "Perform this after the install completes."
        echo " "
        echo "Press any key to continue..."
        echo " "
        read wait_for_any
    fi

    if [ ! -f /h/NTCSS_APP/scripts/paxferd.scr ]
    then
        if [ ! -d /h/NTCSS_APP/scripts ]
        then
            echo "Creating /h/NTCSS_APP/scripts directory" >> $log
            echo "Creating /h/NTCSS_APP/scripts directory"
            mkdir -p /h/NTCSS_APP/scripts
            chown imabtusr /h/NTCSS_APP /h/NTCSS_APP/scripts
            chmod 755 /h/NTCSS_APP /h/NTCSS_APP/scripts
        fi
        if [ ! -f /h/NTCSS_APP/scripts/paxferd.scr ]
        then
            echo "Copying paxferd.scr to /h/NTCSS_APP/scripts" >> $log
            echo "Copying paxferd.scr to /h/NTCSS_APP/scripts"
            cp -p $postdir/paxferd.scr /h/NTCSS_APP/scripts/paxferd.scr
        fi
        if [ ! -f /h/NTCSS_APP/scripts/ntcss_app.env ]
        then
            echo "touching ntcss_app.env in /h/NTCSS_APP/scripts" >> $log
            echo "touching ntcss_app.env in /h/NTCSS_APP/scripts"
            touch /h/NTCSS_APP/scripts/ntcss_app.env
        fi
    fi
    # Create SDI directories if needed
    echo "Creating SDI directories" >> $log
    echo "Creating SDI directories"
    if [ ! -d /h/ATLASS/data ]
    then
        mkdir -p /h/ATLASS/data
        chmod -R 777 /h/ATLASS
    fi
    if [ ! -d /h/ATLASS/data/reports ]
    then
        mkdir -p /h/ATLASS/data/reports
        chmod -R 777 /h/ATLASS/data/reports
    fi
    if [ ! -d /h/Sybase_Utils/data ]
    then
        mkdir -p /h/Sybase_Utils/data
        chmod -R 777 /h/Sybase_Utils
    fi
    if [ ! -d /h/NTCSS_APP/data ]
    then
        mkdir -p /h/NTCSS_APP/data ]
        chmod -R 777 /h/NTCSS_APP
    fi
    if [ ! -d /h/ADHOC/data/reports ]
    then
        mkdir -p /h/ADHOC/data/reports ]
        chmod -R 777 /h/ADHOC
    fi
    # Set MASTER file to point to ATLASS master 
    do_database_files
    addsource
    killprogd
    do_host_id_file
}

#-------------------------------------------------------------------
start_ntcss_master()
{
    echo "Starting NTCSSS in master mode" >> $log
    echo "Starting NTCSSS in master mode"
    echo "If slave servers are down, startup will be slow.."
    echo " "
    $ntcss_bin_dir/ntcss_init -noboot
    #The next command must be done after the ntcssii gens are started
    echo "7 Second Delay to let NTCSS start....."
    sleep 4
    echo "3 Seconds left....."
    sleep 3
    if [ $keepnt = "n" ]
    then
        get_domain_name
    fi
}

    #============ Post install begins here ===============================

    # Initialize for run
    initialize

    if [ -d $ntcssdir/pcload ]
    then
        echo "Removing old pcload directory" >> $log
        echo "Removing old pcload directory"
        rm -r $ntcssdir/pcload
    fi

    if [ -d $webdir ]
    then
        if [ -f $webdir/bin/stopserver ]
        then
            # Stop web server if it is up
            ncline=`ps -ef | grep httpd | egrep -v grep`
            if [ -n "${ncline}" ]
            then
                $webdir/bin/stopserver
            fi
        fi
        if [ ! -f /keep_old_web ]
        then
            # Remove old web software
            echo "Removing web directory" >> $log
            echo "Removing web directory"
            rm -r $webdir
        fi
    fi

    if [ -d $init_data_dir/tools ]
    then
        # Remove old tools software
        rm -r $init_data_dir/tools
        echo "Removing tools directory" >> $log
        echo "Removing tools directory"
    fi

    # Install new pcload directory
    mv $postdir/pcload $ntcssdir/pcload
    echo "pcload directory has been moved" >> $log
    echo "pcload directory has been moved"

    # Install new tools directory
    cp -p $ntcss_bin_dir/u2n_convert $postdir/tools
    cp -p $ntcss_bin_dir/nalc_conv $postdir/tools
    mv $postdir/tools $ntcssdir/init_data/tools
    echo "tools directory has been moved" >> $log
    echo "tools directory has been moved"

    # Install Web Server
    if [ ! -f /keep_old_web ]
    then
        mv $postdir/web $ntcssdir
        echo "Installing new web software" >> $log
        echo "Installing new web software"
        $postdir/do_web.sh $hostname
        echo "Web installed" >> $log
        echo "Web installed"
    fi

    if [ -f /upgrade ]
    then
        # Switch some things around so that web starts and stops with
        # ntcss_init and ntcss_halt commands
        # This is an upgrade, so the script 'ntcss_init' has been overlayed
        echo "Modifying ntcss_init/ntcss_halt to bring up web server" >> $log
        echo "Modifying ntcss_init/ntcss_halt to bring up web server"
        #cp -p $postdir/ntcssinit $ntcss_bin_dir/ntcss_init
        #cp -p $postdir/ntcsshalt $ntcss_bin_dir/ntcss_halt
        chmod 700 $ntcss_bin_dir/ntcss_init
        chmod 700 $ntcss_bin_dir/ntcss_halt
        # Do repository name again..
        x=`grep REPOSITORY_NAME $dbdir/SYS_CONF`
        if [ $? != "0" ]
        then
            echo " ";echo " "
            echo "Enter the ATLASS REPOSITORY_NAME"
            echo "..... (actual host name - case sensitive)....."
            echo " "
            read rep_name
            echo " "
            ed - /h/NTCSSS/database/SYS_CONF <<EOF
$
a
REPOSITORY_NAME: $rep_name
.
1
/NTUSERDIR:/d
a
NTUSERDIR: NTUSERS
.
1
/NTPROFDIR:/d
a
NTPROFDIR: PROFILES
.
w
q
EOF
        fi

        # Upgrade init_data/progrps.ini file with data from atlass_occurance
        $ntcss_bin_dir/updateProgrps

        rm -f /upgrade
        exit 0
    fi

    # Fix /etc/group file to remove any occurences of "nt_".
    if [ $master_server = "y" ]
    then
        curdir=`pwd`
        cd $init_data_dir/tools/opt
        ./update_nt_groups
        cd $curdir
    fi

    chmod 755 $ntcss_bin_dir/net_xfer
    chmod 755 $ntcss_bin_dir/progd
    chmod 755 $ntcss_bin_dir/remsh


    # sethost and prime must be run to get ntcss group installed
    # This sethost and prime is against the generic NTCSS progrps file
    if [ $master_server = "y" ]
    then
        echo "Executing generic prime_db to get 'ntcss' group installed" >> $log
        echo "Executing generic prime_db to get 'ntcss' group installed"
        sethost

#	dbver=`cat $dbdir/VERSION`
#	if [ "$dbver" = "3.0.1" ]
#	then
ed $dbdir/ntcss_users <<EOF 2>&1 > /dev/null
1,\$s/NOHOST/${hostname}/
w
q
EOF
#	fi

        do_prime

        #Create shared memory segment
        $ntcss_bin_dir/init_ntcss_system_setting
        add_ntcssii
        add_www
    fi

    # Backup things we'll change
    do_backups

    case $master_server in
        "y")
            echo "case-master" >> $log
            echo "case-master"
            do_atlass
        ;;
        "n")
            echo "case-slave" >> $log
            echo "case-slave"
            do_slave
        ;;
    esac

    # Don't mess with inhouse systems, they could be different
    if [ $inhouse = "n" ]
    then
        echo "Installing refresh_nis_maps.sh" >> $log
        echo "Installing refresh_nis_maps.sh"
        cp -p $postdir/refresh_nis_maps.sh $ntcss_bin_dir/refresh_nis_maps.sh
        echo "Installing setup.sh, shlib and SITE_FILE" >> $log
        echo "Installing setup.sh, shlib and SITE_FILE"
        cp -p $postdir/setup.sh $init_data_dir/setup.sh
        cp -p $postdir/shlib $init_data_dir/shlib
        cp -p $postdir/SITE_FILE $init_data_dir/SITE_FILE
        chmod 700 $init_data_dir/setup.sh
        chmod 700 $init_data_dir/shlib
        chmod 700 $init_data_dir/SITE_FILE
        echo "Copying getcrc, upper, dot & fdump to post_install directory" >> $log
        echo "Copying getcrc, upper, dot & fdump to post_install directory"
        cp -p $ntcss_bin_dir/getcrc $postdir 
        cp -p $ntcss_bin_dir/fdump $postdir
        cp -p $ntcss_bin_dir/upper $postdir
        cp -p $ntcss_bin_dir/dot $postdir
    fi

    # Update the user directories, /u or /h/USERS 
    # The inhouse flag is passed to avoid modifying in-house systems..
    #$postdir/updateu.sh $inhouse | tee $ntcssdir/passwd.lst

    # Install fs_archive for the specific system being installed
    #init_fs_archive

    # Insure that a printcap file exists in any case
    if [ ! -f /etc/printcap ]
    then
        touch /etc/printcap
    fi

    # Create directories for backups, etc.
    make_ntcssii_dir
    make_ntcss_dir

    # Do sethost and prime_db and add boot users (against the specific progrps)
    if [ $master_server = "y" ]
    then
        echo "Making IP address for NTCSS hosts file" >> $log
        echo "Making IP address for NTCSS hosts file"
        if [ $keep_pg = "n" ]
        then
            set_servers
        fi
        sethost
        rm -f $ucdir/pmap.ini
        if [ $keepnt = "n" ]
        then
            get_nt_wd
        fi
        do_prime

        echo "Enabling boot user accounts" >> $log
        echo "Enabling boot user accounts"
        echo "---for ATLASS" >> $log
        echo "---for ATLASS"
        $postdir/enable_account.sh atlbtusr
    fi

#    # Install Web Server
#    if [ ! -f /keep_old_web ]
#    then
#        mv $postdir/web $ntcssdir
#        echo "Installing new web software" >> $log
#        echo "Installing new web software"
#        $postdir/do_web.sh $hostname
#        echo "Web installed" >> $log
#        echo "Web installed"
#    fi

    # Switch some things around so that web starts and stops with
    # ntcss_init and ntcss_halt commands
    echo "Modifying ntcss_init/ntcss_halt to bring up web server" >> $log
    echo "Modifying ntcss_init/ntcss_halt to bring up web server"
    #cp -p $postdir/ntcssinit $ntcss_bin_dir/ntcss_init
    #cp -p $postdir/ntcsshalt $ntcss_bin_dir/ntcss_halt
    chmod 700 $ntcss_bin_dir/ntcss_init
    chmod 700 $ntcss_bin_dir/ntcss_halt

    # Do NIS push if on NIS Master
    do_push

    # Start NTCSSS (required to execute setlinkdata)
    if [ `hostname` = `ypwhich -m passwd.byname` ]
    then
            start_ntcss_master
            echo " ";echo " "
            echo "Enter the ATLASS REPOSITORY_NAME"
            echo "..... (actual host name - case sensitive)....."
            echo " "
            read rep_name
            echo " "
            $ntcsscmd comData set REPOSITORY_NAME $rep_name 
            echo `$ntcsscmd comData get REPOSITORY_NAME`
    else

            echo "Restarting ypbind" >> $log
            echo "Restarting ypbind"

            psline=`ps -ef | grep ypbind | egrep -v grep`
            if [ "$?" = "0" ]
            then
                #ypbind running
                yppid=`echo $psline | sed -n "s/[         ]*[^     ]*[    ]*\([^  ]*\).*/\1/p"`
                if [ -n "$yppid" ]
                then
        		kill -9 $yppid
                fi
            fi
            /usr/lib/netsvc/yp/ypbind -ypset
            /usr/sbin/ypset $master_name

            echo "Starting NTCSS on non-master system" >> $log
            echo "Starting NTCSS on non-master system"
            $ntcss_bin_dir/ntcss_init -noboot
    fi

    # Set link Data if not in-house (ntcss_init must have been executed)
    if [ $inhouse = "n" ]
    then
        echo "Setting Link Data" >> $log
        echo "Setting Link Data"
        $ntcss_bin_dir/setlinkdata $hostname
    fi

    touch $dbdir/ntprint.ini

    #Delete old scheduler; may be referencing "NTCSSII"
    crontab -l > ct.out
    cstr=`grep "scheduler check" ct.out`
    if [ "$?" = "0" ]
    then
        $ntcss_bin_dir/$ntcsscmd scheduler delete
        if [ "$?" = "0" ]
        then
            echo "Old sceduler detected and deleted." >> $log
            echo "Old sceduler detected and deleted."
        else
            echo "Unable to delete the old scheduler." >> $log
            echo "Unable to delete the old scheduler."
            echo "Run $ntcss_bin_dir/$ntcsscmd and correct the problem" >> $log
            echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem"
        fi
    else
         echo "Scheduler not currently in cron." >> $log
         echo "Scheduler not currently in cron."
    fi

    #Add new scheduler
    $ntcss_bin_dir/$ntcsscmd scheduler add
    if [ "$?" = "0" ]
    then
        echo "New scheduler added." >> $log
        echo "New scheduler added."
    else
        echo "Unable to add new scheduler." >> $log
        echo "Unable to add new scheduler."
        echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem" >> $log
        echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem"
    fi

    #Add bulletin board cleanup default of 7 days
    if [ $master_server = "y" ]
    then
        #First delete the old cleanup in case of path name change..
        crontab -l > ct.out
        bb=`grep "msgbb cleanup" ct.out`
        if [ "$?" = 0 ]
        then
            $ntcss_bin_dir/$ntcsscmd msgbb delsched
            if [ "$?" = "0" ]
            then
                echo "Old bulletin board schedule detected and deleted" >> $log
                echo "Old bulletin board schedule detected and deleted"
            else
                echo "Old bulletin board schedule could not be deleted" >> $log
                echo "Old bulletin board schedule could not be deleted"
                echo "Run $ntcss_bin_dir/$ntcsscmd and correct the problem" >> $log
                echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem"
            fi
        else
            echo "Message bulletin board scheduler not currently active." >> $log
            echo "Message bulletin board scheduler not currently active."
        fi
        #Now add new schedule
        $ntcss_bin_dir/$ntcsscmd msgbb addsched
        if [ "$?" = "0" ]
        then
            echo "Message bulletin board schedule added." >> $log
            echo "Message bulletin board schedule added."
        else
            echo "Unable to add message bulletin board schedule" >> $log     
            echo "Unable to add message bulletin board schedule"     
            echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem" >> $log 
            echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem" 
        fi
        $ntcss_bin_dir/$ntcsscmd msgbb setperiod 7
        if [ "$?" = "0" ]
        then
            echo "Message bulletin board cleanup set to 7 days." >> $log
            echo "Message bulletin board cleanup set to 7 days."
        else
            echo "Unable to set period of message bulletin board cleanup" >> $log
            echo "Unable to set period of message bulletin board cleanup"
            echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem" >> $log
            echo "Run '$ntcss_bin_dir/$ntcsscmd' and correct the problem"
        fi
    fi

    echo "Halting NTCSSS" >> $log
    echo "Halting NTCSSS....ignore 'terminating messages'"
    $ntcss_bin_dir/ntcss_halt

    # Copy in various commands to bin directory

    # Remove flag files from root
    rm -f /save_output_types ct.out
    rm -f /force_tac4cm /inhouse
    rm -f /skip_updateu /keepip /keepnt /keep_old_web
    
    # Final warnings
    echo " "; echo " "
    echo "NTCSSS has not been started.."
    if [ $master_server = "y" ]
    then
        cat $postdir/final_master
    else
        cat $postdir/final_slave
    fi
    echo " "; echo " "
