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
# Unique things to do for a STOUT/NALCOMIS(1&2 processors)/ENTERPRISE,
# and Ported SNAP-1 and 2 class of (initial or update) installation.
# This version post_installs the "ATLASS (806)" release of NTCSSII,
# but this is Version 803-02.00...)
#
#
#  Modified: 8/13/2002
#                 Modified to only install the apache web server on systems
#                 running OMMS-NG. Will only install on systems who's hostname
#                 is omms or who's hostname is rsupmast and OMMS is in the
#                 progrps.ini file, or if /force_omms file exists. ...DJP
#
#  Modified: 10/16/2001
#                 Modified this shell since we now only have one setlinkdata
#                 program. This shell will now pass an argument to setlinkdata.
#                 setlinkdata now in bin directory.                          djp.
#
#  Modified: 10/15/2001
#                 Copy upper, getcrc, fdump from bin directory to post_install
#                 directory.  djp.
#
#  Modified: 10/12/2001
#                 Copy u2n_convert & nalc_conv from bin directory to
#                 tools & tools/opt directories. This insures correct compiled version
#                 for OS is in the tools direrctory.   djp.
#
#=========================================================================

initialize()
{
    echo "do_post_install.sh"
    settings="/etc/ntcss_system_settings.ini"

    #---NTCSSV home directory
    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    #---Install log file
    log="$ntcssdir/post-install.log"
    echo "NTCSS Home Directory: <$ntcssdir>" >> $log
    #echo "NTCSS Home Directory: <$ntcssdir>"

    echo "-----------INSTALL START-------------------------" >> $log

    #---NTCSS bin directory
    bindir=`grep "NTCSS_BIN_DIR" "$settings" | cut -d= -f2`
    bindir="$ntcssdir/$bindir"
    echo "NTCSS Bin directory: <$bindir>" >> $log
    #echo "NTCSS Bin directory: <$bindir>"

    #---Put bin directory in path
    PATH=$PATH:${bindir}:/usr/local/bin;export PATH

    #---Set LD path for print_ntcss_system_settings
    LD_LIBRARY_PATH=/usr/lib:/lib:/usr/ucblib:/h/NTCSSS/libs;export PATH

    #---GetProfileString, WriteProfileString
    gps="$bindir/GetProfileString"
    wps="$bindir/WriteProfileString"

    #---applications--
    appdir="$ntcssdir/applications"
    echo "NTCSS Application Directory: <$appdir>" >> $log
    #echo "NTCSS Application Directory: <$appdir>"

    #---install-------
    appinst=`$gps Strings APP_INST "applications/data/install" $settings`
    appinst="$ntcssdir/$appinst"
    echo "NTCSS Install Directory: <$appinst>" >> $log
    #echo "NTCSS Install Directory: <$appinst>"

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
    idir=$init_data_dir
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
    sdi_dir="$ntcssdir/$sdi_dir"    echo "NTCSS SDI Directory: <$sdi_dir>" >> $log
#    echo "NTCSS SDI Directory: <$sdi_dir>"

    #---ntcss_cmd---
    ntcsscmd=`$gps NTCSS_SYSTEM_SETTINGS NTCSS_CMD_PROG "ntcss_cmd" $settings`
    echo "NTCSS CMD Name: <$ntcsscmd>" >> $log
#    echo "NTCSS CMD Name: <$ntcsscmd>"

    #---web directory---
    webdir="$ntcssdir/web"
    echo "Web Directory: <$webdir>" >> $log
#    echo "Web Directory: <$webdir>"

    bindir=$ntcssdir/bin
    dbdir=$ntcssdir/database
    log="$ntcssdir/post-install.log"
    appdir="$ntcssdir/applications"
    postdir="$appdir/data/install/post_install"
    ucdir="$appdir/data/users_conf"
    idir="$ntcssdir/init_data"
    progrps_file="$idir/progrps.ini"
    snap="no"
    rps="n"
    opt="y"
    inhouse="n"
    master_server="n"
    enterprise="n"
    keepnt="n"
    keepip="n"
    dualnalc="n"
    ported_nalc="n"
    opt_nalc="n"
    opt_mals="n"
    opt_mals_nalc="n"
    sethost_done="n"
    check_ots="n"
    otfile=""
    opfile=""

# Get release number from the "build_date" file to allow CM to change as needed.
    echo " "
    echo "Install date: `date`" > /tmp/post-install.log
    if [ -f $postdir/build_date ]
    then
       cat /tmp/post-install.log $postdir/build_date >> $log
       cat $postdir/build_date
    fi 

    hostname=`hostname`

    ed - $dbdir/ntcss_users <<EOF 2>&1 >> $log
1,\$s/NOHOST/$hostname/
w
q
EOF


. $appinst/getOSinfo.sh


    if [ $hostname != "omms" -a $hostname != "radm" -a $hostname != "nalc" -a $hostname != "rps0" -a $hostname != "rps1" -a $hostname != "rps2" -a $hostname != "s235_25" -a $hostname != "s010x083" -a $hostname != "s234x201" -a $hostname != "sdb" ]
    then
        master_server="y"
    else
	return
    fi

    if [ `hostname` = `ypwhich -m passwd.byname` ]
    then
        if [ $master_server = "n" ]
	then
                echo " "
		echo "This NTCSS slave is currently the NIS master!!" >> $log
		echo "This NTCSS slave is currently the NIS master!!"
                echo "NIS PUSH NOT PERFORMED!" >> $log
                echo "NIS PUSH NOT PERFORMED!"
	else
	        curdir=`pwd`
        	cd $NISDIR
	        echo " ";echo "Executing NIS make" >> $log
		echo "Executing NIS make"
                if [ `uname` = "SunOS" ]
                then
                    /usr/ccs/bin/make -f Makefile 2>&1 >> $log
                else
	            /usr/bin/make -f Makefile 2>&1 >> $log
                fi
		cd $curdir
	fi
    fi

    if [ -f /keepnt ]
    then
        echo " "
        echo "Keeping NT configuration parameters.." >> $log
        echo "Keeping NT configuration parameters.."
        keepnt="y"
    fi

    if [ -f /keepip ]
    then
        echo " "
        echo "Keeping IP configuration parameters.." >> $log
        echo "Keeping IP configuration parameters.."
        keepip="y"
        touch /upgrade
    fi

    if [ ! -f /upgrade ]
    then
        echo " "
        echo "This is an INITIALIZE install on ($hostname)" >> $log
        echo "This is an INITIALIZE install on ($hostname)"
    else
        echo " "
        echo "This is an UPDATE install on ($hostname)." >> $log
        echo "This is an UPDATE install on ($hostname)."
    fi

# Added 10-20-98 to determine force level or not.
    if [ ! -f /force_srv0 ]
    then
        if [ $hostname != "srv0" ]
        then
            echo " ";echo " "
            echo "Is this an OPTIMIZED 'FORCE LEVEL' install? (Y/N),"
            echo "           ---------"
            echo "    i.e. A carrier-type installation"
            echo " "
            echo "    Note: NOT PORTED !!"
            echo " "
            read force_level
            if [ $force_level = "y" -o $force_level = "Y" ]
            then
                enterprise="y"
                opt="y"
                snap="no"
                ported_nalc="n"
                echo "Force level configuration set" >> $log
                echo "Force level configuration set"
            else
                enterprise="n"
            fi
            echo " "
        fi
    fi
    
# Below detection code commented out.. MALS installs also have
# the LCK..SNAPI file. 10/20/98
# Requested by Code 431, 6/17/98
#    if [ -f /usr/local/lock/LCK..SNAPI ]
#    then
#        echo " "
#        echo "ENTERPRISE configuration detected" >> $log
#        echo "ENTERPRISE configuration detected"
#        enterprise="y"
#        opt="y"
#        snap="no"
#        ported_nalc="n"
#    fi

# Old path for lck file kept for compatibility
#    if [ -f /opt/sybase/lock/LCK..SNAPI ]
#    then
#        echo " "
#        echo "ENTERPRISE configuration detected" >> $log
#        echo "ENTERPRISE configuration detected"
#        enterprise="y"
#        opt="y"
#        snap="no"
#        ported_nalc="n"
#    fi

# Look for dual computer Air Station NALCOMIS-nalcmast configuration
    if [ -f /usr/local/lock/LCK..AS_MASTER ]
    then
        echo " "
        echo "Air Station NALCOMIS Master configuration detected" >> $log
        echo "Air Station NALCOMIS Master configuration detected"
        enterprise="n"
        opt="y"
        snap="no"
        dualnalc="y"
        ported_nalc="n"
    fi

# Look for dual computer Air Station NALCOMIS-nalc configuration
    if [ -f /usr/local/lock/LCK..AS_SLAVE ]
    then
        echo " "
        echo "Air Station NALCOMIS Slave configuration detected" >> $log
        echo "Air Station NALCOMIS Slave configuration detected"
        enterprise="n"
        opt="y"
        snap="no"
        dualnalc="y"
        ported_nalc="n"
    fi

    if [ -f /force_dualnalc ]
    then
        echo " "
        echo "Forced Dual NALCOMIS Master configuration set" >> $log
        echo "Forced Dual NALCOMIS Master configuration set"
        enterprise="n"
        opt="y"
        snap="no"
        dualnalc="y"
        ported_nalc="n"
    fi

    if [ -h /snapi ]
    then
        echo " "
        echo "SNAP-1 Detected" >> $log
        echo "SNAP-1 Detected"
        snap="snapi"
    fi

    if [ -h /snapii ]
    then
        if [ $enterprise = "n" ]
        then
            echo " "
            echo "SNAP-2 Detected, (snap=snapii)" >> $log
            echo "SNAP-2 Detected, (snap=snapii)"
            snap="snapii"
        fi
    fi

    if [ -f /force_snapi ]
    then
        echo " "
        echo "Force SNAP-1 Detected, (snap=snapi)" >> $log
        echo "Force SNAP-1 Detected, (snap=snapi)"
        snap="snapi"
    fi

    if [ -f /force_snapii ]
    then
        echo " "
        echo "Force SNAP-2 Detected, (snap=snapii)" >> $log
        echo "Force SNAP-2 Detected, (snap=snapii)"
        snap="snapii"
    fi

    if [ -f /force_rsupmast ]
    then
        echo " "
        echo "Force rsupmast Detected, (hostname=rsupmast)" >> $log
        echo "Force rsupmast Detected, (hostname=rsupmast)"
        hostname="rsupmast"
    fi

    if [ -f /force_rsupply ]
    then
        echo " "
        echo "Force rsupply Detected, (hostname=rsupply)" >> $log
        echo "Force rsupply Detected, (hostname=rsupply)"
        hostname="rsupply"
    fi

    if [ -f /force_nalcmast ]
    then
        echo " "
        echo "Force nalcmast Detected, (hostname=nalcmast)" >> $log
        echo "Force nalcmast Detected, (hostname=nalcmast)"
        hostname="nalcmast"
    fi

    if [ -f /force_srv0 ]
    then
        echo " "
        echo "Force srv0 Detected, (hostname=srv0)" >> $log
        echo "Force srv0 Detected, (hostname=srv0)"
        hostname="srv0"
    fi

    if [ -f /force_tac4cm ]
    then
        echo " "
        echo "Force tac4cm Detected, (hostname=tac4cm)" >> $log
        echo "Force tac4cm Detected, (hostname=tac4cm)"
        hostname="tac4cm"
    fi

    if [ -f /force_rps ]
    then
        echo " "
        echo "Force RPS Detected, (hostname=rps0)" >> $log
        echo "Force RPS Detected, (hostname=rps0)"
        rps="y"
        hostname="rps0"
    fi

    if [ -f /force_enterprise ]
    then
        echo " "
        echo "Force ENTERPRISE Detected" >> $log
        echo "Force ENTERPRISE Detected"
        opt="y"
        snap="no"
        ported_nalc="n"
        enterprise="y"
    fi

    if [ -f /force_radm ]
    then
        echo " "
        echo "Force RADM Detected, (hostname=radm)" >> $log
        echo "Force RADM Detected, (hostname=radm)"
        enterprise="y"
        opt="y"
        hostname="radm"
    fi

    if [ -f /force_omms ]
    then
        echo " "
        echo "Force OMMS Detected, (hostname=omms)" >> $log
        echo "Force OMMS Detected, (hostname=omms)"
        enterprise="y"
        opt="y"
        hostname="omms"
    fi

    if [ -f /force_nalc ]
    then
        echo " "
        echo "Force NALC Detected, (hostname=nalc)" >> $log
        echo "Force NALC Detected, (hostname=nalc)"
        if [ -f /opt/sybase/lock/LCK..SNAPI ]
        then
            enterprise="y"
            opt="y"
        fi
        hostname="nalc"
    fi

    if [ -f /force_tac4cm ]
    then
        echo " "
        echo "Force tac4cm Detected, (hostname=tac4cm)" >> $log
        echo "Force tac4cm Detected, (hostname=tac4cm)"
        hostname="tac4cm"
    fi

    if [ -f /force_nalcrte ]
    then
        echo " "
        echo "Force nalcrte Detected, (hostname=nalcrte)" >> $log
        echo "Force nalcrte Detected, (hostname=nalcrte)"
        hostname="nalcrte"
    fi

    if [ -f /force_t4naltst ]
    then
        echo " "
        echo "Force t4naltst Detected, (hostname=t4naltst)" >> $log
        echo "Force t4naltst Detected, (hostname=t4naltst)"
        hostname="t4naltst"
    fi

    if [ -f /force_hpnaldev ]
    then
        echo " "
        echo "Force hpnaldev Detected, (hostname=hpnaldev)" >> $log
        echo "Force hpnaldev Detected, (hostname=hpnaldev)"
        hostname="hpnaldev"
    fi

    if [ -f /force_t4omsdev ]
    then
        echo " "
        echo "Force t4omsdev Detected, (hostname=t4omsdev)" >> $log
        echo "Force t4omsdev Detected, (hostname=t4omsdev)"
        hostname="t4omsdev"
    fi

    if [ -f /force_omms_tst ]
    then
        echo " "
        echo "Force omms_tst Detected, (hostname=omms_test)" >> $log
        echo "Force omms_tst Detected, (hostname=omms_test)"
        hostname="omms_tst"
    fi

    if [ -f /force_hprsdev ]
    then
        echo " "
        echo "Force hprsdev Detected, (hostname=hprsdev)" >> $log
        echo "Force hprsdev Detected, (hostname=hprsdev)"
        hostname="hprsdev"
    fi

    if [ -f /force_t4rstest ]
    then
        echo " "
        echo "Force t4rstest Detected, (hostname=t4rstest)" >> $log
        echo "Force t4rstest Detected, (hostname=t4rstest)"
        hostname="t4rstest"
    fi

    if [ -f /force_t4431 ]
    then
        echo " "
        echo "Force t4431 Detected, (hostname=t4431)" >> $log
        echo "Force t4431 Detected, (hostname=t4431)"
        hostname="t4431"
    fi

    if [ -f /force_s234x83 ]
    then
        echo " "
        echo "Force s234x83 Detected, (hostname=s234x83)" >> $log
        echo "Force s234x83 Detected, (hostname=s234x83)"
        hostname="s234x83"
    fi

    if [ -f /force_s235_25 ]
    then
        echo " "
        echo "Force s235_25 Detected, (hostname=s235_25)" >> $log
        echo "Force s235_25 Detected, (hostname=s235_25)"
        hostname="s235_25"
    fi

    if [ -f /force_s010x083 ]
    then
        echo " "
        echo "Force s010x083 Detected, (hostname=s010x083)" >> $log
        echo "Force s010x083 Detected, (hostname=s010x083)"
        hostname="s010x083"
    fi

    if [ -f /force_s234x201 ]
    then
        echo " "
        echo "Force s234x201 Detected, (hostname=s234x201)" >> $log
        echo "Force s234x201 Detected, (hostname=s234x201)"
        hostname="s234x201"
    fi

    if [ $hostname = "rps0" -o $hostname = "rps1" -o $hostname = "rps2" -o $hostname = "nalc" ]
    then
        if [ $enterprise = "n" ]
        then
            echo " ";echo " "
            if [ $dualnalc = "n" ]
            then
                echo "Is this a 'MALS' configuration (Y/N)?"
                echo " "
                read opt_mals_nalc
                if [ $opt_mals_nalc = "y" -o $opt_mals_nalc = "Y" ]
                then
                    echo "Optimized MALS-nalc was answered yes" >> $log
                    echo "Optimized MALS-nalc was answered yes"
                    opt_mals="y"
                    ported_nalc="n"
                else
                    echo "SNAP-1 RPS or NALC Detected, (snap=snapi)" >> $log
                    echo "SNAP-1 RPS or NALC Detected, (snap=snapi)"
                    snap="snapi"
                    ported_nalc="y"
                fi
            fi
        fi
    fi
}

#pingum()
#{
## Ping rps0, rps1, rps2, nalc to get IP addresses and build hosts file
## The "else" section of each if may be uncommented if needed to force
## the entry of a non pingable rps/nalc in the hosts file
#    echo "Attempting to detect rps's/nalc IP addresses and build ntcssii hosts file" >> $log
#    echo "Attempting to detect rps's/nalc IP addresses and build ntcssii hosts file"
#    rm $ntcssdir/init_data/host_info/hosts
#    touch $ntcssdir/init_data/host_info/hosts
## Do test ping on srv0
#    testping=`ping srv0 -n 1 | grep "100%"`
#    if [ $? = 1 ]
#    then
#       echo srv0 `ping srv0 -n 1 | grep "bytes from" | cut -d' ' -f 4 | sed 's/://'` >> $ntcssdir/init_data/host_info/hosts
#    else
#       get_srv0_IP
#    fi
## Do test ping on rps0
#    testping=`ping rps0 -n 1 | grep "100%"`
#    if [ $? = 1 ]
#    then
#       echo rps0 `ping rps0 -n 1 | grep "bytes from" | cut -d' ' -f 4 | sed 's/://'` >> $ntcssdir/init_data/host_info/hosts
#    else
#       get_rps0_IP
#    fi
## Do test ping on rps1
#    testping=`ping rps1 -n 1 | grep "100%"`
#    if [ $? = 1 ]
#    then
#       echo rps1 `ping rps1 -n 1 | grep "bytes from" | cut -d' ' -f 4 | sed 's/://'` >> $ntcssdir/init_data/host_info/hosts
#    else
#       get_rps1_IP
#    fi
## Do test ping on rps2
#    testping=`ping rps2 -n 1 | grep "100%"`
#    if [ $? = 1 ]
#    then
#       echo rps2 `ping rps2 -n 1 | grep "bytes from" | cut -d' ' -f 4 | sed 's/://'` >> $ntcssdir/init_data/host_info/hosts
#    else
#       get_rps2_IP
#    fi
## Do test ping on nalc 
#    testping=`ping nalc -n 1 | grep "100%"`
#    if [ $? = 1 ]
#    then
#       echo nalc `ping nalc -n 1 | grep "bytes from" | cut -d' ' -f 4 | sed 's/://'` >> $ntcssdir/init_data/host_info/hosts
#    else
#       get_nalc_IP
#    fi
#}

verifyip()
{
        ipstat="good"
	if [ -z "$ipaddr" ]
	then
		echo "No IP" >> $log
		echo "No IP"
                ipstat="bad"
		break
	fi

        echo " "
	echo "Processing IP = "$ipaddr >> $log
	echo "Processing IP = "$ipaddr
	
	chk=`echo $ipaddr | sed -n "s/[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*/Good/p"`
        echo " "
	if [ $chk = "Good" ]
	then
		echo "IP address format looks good" >> $log
		echo "IP address format looks good"
	else
		echo "IP address format looks bad" >> $log
		echo "IP address format looks bad"
                ipstat="bad"
		break
	fi

	p1=`echo $ipaddr | cut -d '.' -f1`
#	echo $p1
	p2=`echo $ipaddr | cut -d '.' -f2`
#	echo $p2
	p3=`echo $ipaddr | cut -d '.' -f3`
#	echo $p3
	p4=`echo $ipaddr | cut -d '.' -f4`
#	echo $p4
	
	if [ $p1 -gt 255 -o $p2 -gt 255 -o $p3 -gt 255 -o $p4 -gt 255 ]
	then
                echo " "
		echo "IP address too Big" >> $log
		echo "IP address too Big"
                ipstat="bad"
		break
	fi
	
	if [ $p1 -lt 0 -o $p2 -lt 0 -o $p3 -lt 0 -o $p4 -lt 0 ]
	then
                echo " "
		echo "IP address too Small" >> $log
		echo "IP address too Small"
                ipstat="bad"
		break
	fi
}

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

sethost()
{
# Configure NTCSSII host file
    echo " "
    echo "Setting host name in progrps and ntcss ii hosts files" >> $log
    echo "Setting host name in progrps and ntcss ii hosts files"
    if [ ! -d $ntcssdir/init_data/host_info ]
    then
        mkdir -p $ntcssdir/init_data/host_info
    fi
    rm -f $ntcssdir/init_data/host_info/hosts
    touch $ntcssdir/init_data/host_info/hosts
    $postdir/sethost.sh
}

do_prime()
{
  if [ $master_server = "n" ]
  then
      echo " "
      echo "Trying to do prime_db on an NTCSS slave server" >> $log
      echo "Trying to do prime_db on an NTCSS slave server"
      exit -1
  fi
#  if [ `hostname` = `ypwhich` ]
  if [ `hostname` = `ypwhich -m passwd.byname` ]
  then
      # Do make and push to setup ntcss_hosts.byname
      $bindir/ntcss_make_and_push_nis_maps
      if [ $? = "0" ]
      then
          echo "make and push OK" >> $log
          echo "make and push OK"
      else
          echo "Bad make and push" >> $log
          echo "Bad make and push"
          echo "Aborting mission..."
          exit -1
      fi
      # Now do prime_db to get groups installed
      echo " "
      echo "Executing prime_db" >> $log
      echo "Executing prime_db"
      #$ntcssdir/bin/prime_db  -SSCCinriPRIME 2>&1 >> $log

      # removed i/o redirect so user can answer question ACA 10/12/2000
      $ntcssdir/bin/prime_db  -SSCCinriPRIME
      if [ $? -eq 0 ]
      then
          echo "Prime DB OK" >> $log
          echo "Prime DB OK"
      else
          echo "PRIME_DB FAILED! Aborting mission..." >> $log
          echo "PRIME_DB FAILED! Aborting mission..."
          exit -1
      fi 
      rm -f $dbdir/*.time
      echo " "
      echo "Pushing NTCSS NIS maps" >> $log
      echo "Pushing NTCSS NIS maps"
      $bindir/ntcss_make_and_push_nis_maps
      if [ $? -eq 0 ]
      then
          echo "ntcss_make_and_push OK" >> $log
          echo "ntcss_make_and_push OK"
      else
          echo "ntcss_make_and_push FAILED! Aborting mission..." >> $log
          echo "ntcss_make_and_push FAILED! Aborting mission..."
          exit -1
      fi
  fi
}

add_ntcssii()
{
# Create ntcssii user if it doesn't exist
    echo " "
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
            $ntcssdir/applications/data/users_conf/user_config ADDUSER ntcssii ntcssii /h/USERS "NTCSS Administrator" 
        else
            $ntcssdir/applications/data/users_conf/user_config ADDUSER ntcssii ntcssii /u "NTCSS Administrator" 
        fi
        echo "User ntcssii has been added to /etc/passwd file" >> $log
        echo "User ntcssii has been added to /etc/passwd file"
    fi
}

add_www()
{
# Create user for NTCSS Web Server
    echo " "
    echo "Creating User www" >> $log
    echo "Creating User www"
    ntcss_op=`grep "^www:" /etc/passwd`
    if [ -z "${ntcss_op}" ]
    then
        echo "User www was not found in /etc/passwd file" >> $log
        echo "User www was not found in /etc/passwd file"
        if [ $opt = "y" ]
        then
            $ntcssdir/applications/data/users_conf/user_config ADDUSER www u505505 /h/USERS "NTCSS Web start"
        else
            $ntcssdir/applications/data/users_conf/user_config ADDUSER www u505505 /u "NTCSS Web start"
        fi
        echo "User www has been added to /etc/passwd file" >> $log
        echo "User www has been added to /etc/passwd file"
    fi
}

# Add nalcomis group for ported snap 1
add_nalc_group()
{
# Create nalcomis group if it doesn't exist
    echo " "
    echo "Creating nalcomis group" >> $log
    echo "Creating nalcomis group"
    gline=`grep "^nalcomis" /etc/group`
    if [ -z "${gline}" ]
    then
        echo "Group nalcomis was not found in /etc/group file" >> $log
        echo "Group nalcomis was not found in /etc/group file"
        gid_hi=`sort -t: +2n -3 /etc/group | tail -1 | cut -d: -f3`
        gid_new=`expr ${gid_hi} + 1`
        echo "nalcomis:*:$gid_new:" >> /etc/group
        echo "User nalcomis has been added to /etc/group file" >> $log
        echo "User nalcomis has been added to /etc/group file"
    fi
}

do_backups()
{
# Save any uniques which may have been created, in case the installer
# has not set the "upgrade" condition by mistake (for recovery purposes)
    appdate=`date +"%m%d%y.%H%M"`
# The only files this script modifies:
    cp -p $ntcssdir/init_data/progrps.ini $postdir/progrps.$appdate
    cp -p $ntcssdir/database/output_type $postdir/output_type.$appdate
    cp -p $ntcssdir/database/output_prt $postdir/output_prt.$appdate
    cp -p /etc/services $postdir/services.$appdate
}

update_logingroup()
{
    echo " "
    echo "Modifying /etc/logingroup" >> $log
    echo "Modifying /etc/logingroup"
    rm -f /etc/logingroup
    ln -s /etc/group /etc/logingroup
}

#install_setlinkdata()
#{
#    if [ $PLATFORM = "SunOS" ]
#    then
#        cp -p $postdir/setlinkdata_sun_$config $ntcssdir/bin/setlinkdata
#    fi
#    if [ $PLATFORM = "HP-UX" ]
#    then
#        cp -p $postdir/setlinkdata_$config $ntcssdir/bin/setlinkdata
#    fi
#    if [ $PLATFORM = "Linux" ]
#    then
#        cp -p $postdir/setlinkdata_linux_$config $ntcssdir/bin/setlinkdata
#    fi
#}

do_rsupmast()
{
    echo " "
    sld_arg="rsup"
    if [ $enterprise = "y" ]
    then
        $postdir/update_appusers.sh
    fi
    if [ ! -f /upgrade ]
    then
        echo " "
        if [ $enterprise = "n" ]
        then
            echo " "
            echo "Is this install for a MALS configuration (Y/N)?"
            echo " "
            read opt_mals
            if [ $opt_mals = "y" -o $opt_mals = "Y" ]
            then
                echo "Copying new 2 computer MALS progrps.ini to $ntcssdir/init_data" >> $log
                echo "Copying new 2 computer MALS progrps.ini to $ntcssdir/init_data"
                cp -p $postdir/progrps_mals2.ini $ntcssdir/init_data/progrps.ini
            else
                echo " "
                echo "Is this install for an 'RSUPPLY/NALCOMIS' Airstation?"
                echo " "
                read air_load
                if [ $air_load = "y" -o $air_load = "Y" ]
                then
                    air_load="y"
                    echo "Copying new RSUPPLY/NALCOMIS Airstation configuration." >> $log
                    echo "Copying new RSUPPLY/NALCOMIS Airstation configuration."
                    cp -p $postdir/progrps_oceana.ini $ntcssdir/init_data/progrps.ini
                else
                    echo "Copying new STOUT progrps file to $ntcssdir/init_data" >>$log
                    echo "Copying new STOUT progrps file to $ntcssdir/init_data"
                    cp -p $postdir/progrps_stout.ini $ntcssdir/init_data/progrps.ini
                fi
            fi
        else
            echo "Copying new ENTERPRISE progrps file to $ntcssdir/init_data" >> $log
            echo "Copying new ENTERPRISE progrps file to $ntcssdir/init_data"
            cp -p $postdir/progrps_enterprise.ini $ntcssdir/init_data/progrps.ini
        fi
        if [ -f /save_output_types ]
        then
            echo "Output types preserved" >> $log
            echo "Output types preserved"
        else
#           echo "Copying new output_type/prt files to database" >> $log
#           echo "Copying new output_type/prt files to database"

	    check_ots="y"

            if [ $enterprise = "n" ]
            then
                if [ "$air_load" = "y" ]
                then
                    ####cp -p $postdir/output_type_enterprise $ntcssdir/database/output_type
			otfile="$postdir/output_type_enterprise"
			opfile=""
                else
                    if [ $opt_mals = "n" ]
                    then
                        ####cp -p $postdir/output_type_stout $ntcssdir/database/output_type
                        ####cp -p $postdir/output_prt_stout $ntcssdir/database/output_prt
			otfile="$postdir/output_type_stout"
			opfile="$postdir/output_prt_stout"
                    else
                        ####cp -p $postdir/output_type_mals $ntcssdir/database/output_type
                        ####cp -p $postdir/output_prt_mals $ntcssdir/database/output_prt
			otfile="$postdir/output_type_mals"
			opfile="$postdir/output_prt_mals"
                    fi
                fi
            else
                ####cp -p $postdir/output_type_enterprise $ntcssdir/database/output_type
#               ####cp -p $postdir/output_prt_enterprise $ntcssdir/database/output_prt
		otfile="$postdir/output_type_enterprise"
		opfile="$postdir/output_prt_enterprise"
            fi
        fi
    fi
# Add program to allow other systems to execute programs here.
    killprogd
# Install app_dirs for stout class
    if [ $enterprise = "n" ]
    then
        cp -p $postdir/app_dirs_stout $appdir/data/srv_dev/app_dirs
    else
        cp -p $postdir/app_dirs_enterprise $appdir/data/srv_dev/app_dirs
    fi
}

do_sdbdev()
{
    echo " "
    sld_arg="rsup"
    if [ ! -f /upgrade ]
    then
        echo "Copying 2 computer Airstation progrps.ini " >> $log
        echo "to $ntcssdir/init_data" >> $log
        echo "Copying 2 computer Airstation progrps.ini "
        echo "to $ntcssdir/init_data"
        cp -p $postdir/progrps_oceana_inhouse.ini \
              $ntcssdir/init_data/progrps.ini
    fi
# Add program to allow other systems to execute programs here.
    killprogd
}

do_nalcmast()
{
    echo " "
    sld_arg="nalc"
    if [ ! -f /upgrade ]
    then
        echo " "
        if [ $dualnalc = "n" ]
        then
            echo "Copying new nalc progrps file for single config" >>$log
            echo "Copying new nalc progrps file for single config"
            cp -p $postdir/progrps_nalc.ini $ntcssdir/init_data/progrps.ini
        else
            echo "Copying new nalc progrps file for dual config" >> $log
            echo "Copying new nalc progrps file for dual config"
            cp -p $postdir/progrps_nalc2.ini $ntcssdir/init_data/progrps.ini
        fi
        sethost
        sethost_done="y"
        echo " "
        if [ -f /save_output_types ]
        then
            echo "Output types preserved" >> $log
            echo "Output types preserved"
        else
            echo "Copying new nalc output_type/prt files to database" >> $log
            echo "Copying new nalc output_type/prt files to database"
            cp -p $postdir/output_type_nalc $ntcssdir/database/output_type
            cp -p $postdir/output_prt_nalc $ntcssdir/database/output_prt
        fi
    fi
# Install app_dirs for nalc class
    cp -p $postdir/app_dirs_nalc $appdir/data/srv_dev/app_dirs
}

do_nalcrte()
{
    echo " "
    sld_arg="nalc"
    if [ ! -f /upgrade ]
    then
        echo " "
        echo "Copying new nalcrte progrps file to $ntcssdir/init_data" >>$log
        echo "Copying new nalcrte progrps file to $ntcssdir/init_data"
        cp -p $postdir/progrps_nalcrte.ini $ntcssdir/init_data/progrps.ini
    fi
# Install app_dirs for nalc class
    cp -p $postdir/app_dirs_nalc $appdir/data/srv_dev/app_dirs
}


do_hpnaldev()
{
    echo " "
    sld_arg="nalc"
    if [ ! -f /upgrade ]
    then
        echo " "
        echo "Copying new nalc progrps file to $ntcssdir/init_data" >>$log
        echo "Copying new nalc progrps file to $ntcssdir/init_data"
        cp -p $postdir/progrps_hpnaldev.ini $ntcssdir/init_data/progrps.ini
    fi
# Install app_dirs for nalc class
    cp -p $postdir/app_dirs_nalc $appdir/data/srv_dev/app_dirs
}

do_t4naltst()
{
    echo " "
    sld_arg="nalc"
    if [ ! -f /upgrade ]
    then
        echo " "
        echo "Copying new nalc progrps file to $ntcssdir/init_data" >>$log
        echo "Copying new nalc progrps file to $ntcssdir/init_data"
        cp -p $postdir/progrps_t4naltst.ini $ntcssdir/init_data/progrps.ini
    fi
# Install app_dirs for nalc class
    cp -p $postdir/app_dirs_nalc $appdir/data/srv_dev/app_dirs
}

addsource()
{
# Add source of coe_cshrc to csh.login
    if [ $master_server = "n" ]
    then
        ckit=`grep "coe_cshrc" /etc/csh.login`
        if [ -z "$ckit" ]
        then
            echo " "
            echo "Adding source of coe_cshrc to csh.login for slave processors" >> $log
            echo "Adding source of coe_cshrc to csh.login for slave processors"
            cp /etc/csh.login /etc/csh.login.old
            cat /etc/csh.login.old $postdir/addsource > /etc/csh.login
        fi
    fi
}

do_omms()
{
    echo " "
    sld_arg="omms"
    echo "rsupmast" > $dbdir/MASTER
    addsource
# Add progd to allow other systems to execute programs here
    killprogd
    do_host_id_file
}

do_sdb()
{
    echo " "
    echo "Installing setlinkdata command for NALCOMIS" >> $log
    echo "Installing setlinkdata command for NALCOMIS"
    sld_arg="nalc"
    echo "sdbdev" > $dbdir/MASTER
    addsource
# Add progd to allow other systems to execute programs here
    killprogd
    do_host_id_file
}

do_t4omsdev()
{
    echo " "
    sld_arg="omms"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_t4omsdev.ini $ntcssdir/init_data/progrps.ini
    fi
    killprogd
}

do_omms_tst()
{
    echo " "
    sld_arg="omms"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_omms_tst.ini $ntcssdir/init_data/progrps.ini
    fi
    killprogd
}

do_hprsdev()
{
    echo " "
    sld_arg="rsup"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_hprsdev.ini $ntcssdir/init_data/progrps.ini
    fi
    killprogd
}

do_t4rstest()
{
    echo " "
    sld_arg="rsup"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_t4rstest.ini $ntcssdir/init_data/progrps.ini
    fi
    killprogd
}

do_t4431()
{
    echo " "
    sld_arg="rsup"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_stout.ini $ntcssdir/init_data/progrps.ini
    fi
    killprogd 
}

do_s234x83()
{
    echo " "
    sld_arg="radm"
    if [ ! -f /upgrade ]
    then
        cp $postdir/progrps_s234x83.ini /$ntcssdir/init_data/progrps.ini
    fi
    killprogd
}

do_s235_25()
{
    echo " "
    sld_arg="radm"
    echo "s234x83" > $ntcssdir/database/MASTER 
}

do_s010x083()
{
    echo " "
    sld_arg="radm"
    echo "s234x83" > $ntcssdir/database/MASTER 
}

do_s234x201()
{
    echo " "
    sld_arg="radm"
    echo "s234x83" > $ntcssdir/database/MASTER 
}

do_radm()
{
    echo " "
    sld_arg="radm"
    cp $postdir/MASTER_enterprise $ntcssdir/database/MASTER
    addsource
# Add progd to allow other systems to execute programs here
    killprogd
    do_host_id_file
}

do_nmaster()
{
#default load
    do_rsupmast
}

do_tac4cm()
{
#default load for CM
    touch /save_output_types
    do_rsupmast
}

#add_boot_users()
#{
# Add boot users to appropiate application groups in /etc/group
#   echo "Adding boot users to /etc/group file" >> $log
#   echo "Adding boot users to /etc/group file"
#   #awk -f $postdir/getbtusr.awk $ntcssdir/init_data/progrps
#   $postdir/add_batch_user.sh
#}

do_push()
{
    if [ `hostname` = `ypwhich -m passwd.byname` ]
    then
        if [ $master_server = "n" ]
	then
                echo " "
		echo "This NTCSS slave is currently the NIS master!!" >> $log
		echo "This NTCSS slave is currently the NIS master!!"
                echo "NIS PUSH NOT PERFORMED!" >> $log
                echo "NIS PUSH NOT PERFORMED!"
	else
	        curdir=`pwd`
        	cd $NISDIR
	        echo " ";echo "Executing NIS make" >> $log
		echo "Executing NIS make"
                if [ `uname` = "SunOS" ]
                then
                    /usr/ccs/bin/make -f Makefile 2>&1 >> $log
                else
                    /usr/bin/make -f Makefile 2>&1 >> $log
                fi
		cd $curdir
	fi
    fi
}

init_fs_archive()
{
   echo " "
   echo "Initializing Backup archive info" >> $log
   echo "Initializing Backup archive info"
   curdir=`pwd`
   cd $appdir/data/fs_archive
   if [ $PLATFORM = "HP-UX" ]
   then
       if [ $opt = "y" ]
       then
           tar xvf $postdir/fs_archive_opt.tar 2>&1 >> $log
           echo "Optimized fs_archive copied" >> $log
           echo "Optimized fs_archive copied"
       fi
   fi
   if [ $PLATFORM = "SunOS" ]
   then
           tar xvf $postdir/fs_archive_sun.tar 2>&1 >> $log
           echo "Optimized fs_archive copied" >> $log
           echo "Optimized fs_archive copied"
   fi
   if [ $snap = "snapi" ]
   then
       tar xvf $postdir/fs_archive_s1p.tar 2>&1 >> $log
       echo "SNAP I Ported fs_archive copied" >> $log
       echo "SNAP I Ported fs_archive copied"
   fi
   if [ $snap = "snapii" ]
   then
       tar xvf $postdir/fs_archive_s2p.tar 2>&1 >> $log
       echo "SNAP II Ported fs_archive copied" >> $log
       echo "SNAP II Ported fs_archive copied"
   fi
   cd $curdir 
}

make_ntcssii_dir()
{
    echo " "
    echo "Creating ntcssii/backups directory" >> $log
    echo "Creating ntcssii/backups directory"
        if [ ! -d /h/USERS/ntcssii/backups ]
        then
            mkdir -p /h/USERS/ntcssii/backups
            chmod -R 777 /h/USERS/ntcssii
        fi
}

make_ntcss_dir()
{
    if [ ! -d /h/NTCSS ]
    then
        echo " "
        echo "Creating /h/NTCSS directory" >> $log
        echo "Creating /h/NTCSS directory"
        mkdir -p /h/NTCSS/bin
        mkdir /h/NTCSS/scripts
        mkdir /h/NTCSS/data
        chmod -R 777 /h/NTCSS
    fi
}

do_database_files()
{
# Copy SYS_CONF to database to establish user directories
    if [ ! -f /keepnt ]
    then
        echo " "
        echo "Copying SYS_CONF to database to establish user directories" >> $log
        echo "Copying SYS_CONF to database to establish user directories"
        cp -p $postdir/SYS_CONF /h/NTCSSS/database
    fi

# If rps, copy MASTER file to database to point to srv0
    if [ $rps = "y" ]
    then
        echo " "
        echo "Copying MASTER, rps=" $rps >> $log
        echo "Copying MASTER, rps=" $rps
        cp -p $postdir/MASTER_snapi $ntcssdir/database/MASTER
    fi
}

get_nt_wd()
{
    # The following code is to clean up pre-existing SYS_CONF entries..
    ntusrdir=`grep "^NTUSERDIR" $dbdir/SYS_CONF`
    nttmpusr=`grep "^NTTMPUSER" $dbdir/SYS_CONF`
    ntprofdir=`grep "^NTPROFDIR" $dbdir/SYS_CONF`
        
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

do_s1p_host_file()
{
# Create NTCSS II hosts file
    echo " "
    echo " "
    srv0ipp=`grep "srv0" /etc/hosts`
    rps0ipp=`grep "rps0" /etc/hosts`
    rps1ipp=`grep "rps1" /etc/hosts`
    rps2ipp=`grep "rps2" /etc/hosts`
    nalcipp=`grep "nalc" /etc/hosts`
    if [ -n "$srv0ipp" -a "$rps0ipp" -a "$rps1ipp" -a "$rps2ipp" -a "$nalcipp" ]
    then
        echo "Making IP address for NTCSS hosts file via sethost" >> $log
        echo "Making IP address for NTCSS hosts file via sethost"
        sethost
        sethost_done="y"
        return
    fi
    echo "Gathering info to create NTCSS II hosts file" >> $log
    echo "Gathering info to create NTCSS II hosts file"
    echo " ...in /h/NTCSSS/init_data/host_info..."
    echo " "
    echo " "
    rm $ntcssdir/init_data/host_info/hosts
    touch $ntcssdir/init_data/host_info/hosts
    get_srv0_IP
    get_rps0_IP
    get_rps1_IP
    get_rps2_IP
    get_nalc_IP
}

do_enterprise_host_file()
{
# Create NTCSS II hosts file
    echo " "
    echo " "
    echo "Gathering info to create NTCSS II hosts file" >> $log
    echo "Gathering info to create NTCSS II hosts file"
    echo " ...in /h/NTCSSS/init_data/host_info..."
    echo " "
    echo " "
# Removed when we went to the ini file format
#    rm $ntcssdir/init_data/host_info/hosts
#    touch $ntcssdir/init_data/host_info/hosts
    get_rsupmast_IP
    get_omms_IP
    get_radm_IP
    get_nalc_IP
}

get_srv0_IP()
{
    echo "Enter the IP address for srv0.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS srv0 $ipaddr $progrps_file
    fi
    echo "	"
}

get_rps0_IP()
{
    echo "	"
    echo "Enter the IP address for rps0.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS rps0 $ipaddr $progrps_file
    fi
    echo "	"
}

get_rps1_IP()
{
    echo "	"
    echo "Enter the IP address for rps1.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS rps1 $ipaddr $progrps_file
    fi
    echo "	"
}

get_rps2_IP()
{
    echo "	"
    echo "Enter the IP address for rps2.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS rps2 $ipaddr $progrps_file
    fi
    echo "	"
}

get_nalc_IP()
{
    echo "	"
    echo "Enter the IP address for nalc.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS nalc $ipaddr $progrps_file
    fi
    echo "	"
}

get_rsupmast_IP()
{
    echo " "
    echo "Enter the IP address for rsupmast.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS rsupmast $ipaddr $progrps_file
    fi
    echo "      "
}

get_omms_IP()
{
    echo " "
    echo "Enter the IP address for omms.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS omms $ipaddr $progrps_file
    fi
    echo "      "
}

get_radm_IP()
{
    echo " "
    echo "Enter the IP address for radm.."
    echo "	"
    enterip
    if [ $ipstat = "good" ]
    then
	$ntcssdir/bin/WriteProfileString HOSTS radm $ipaddr $progrps_file
    fi
    echo "      "
}

get_domain_name()
{
    echo " "
    echo "Getting domain name" >> $log
    echo "Getting domain name"
    defdom="NTCSS"
    echo "	"
    echo "Enter the NT domain name (return for default of NTCSS).."
    echo " "
    echo "    NOTE-1: This entry must be in CAPITAL letters !!"
    echo "    NOTE-2: This determines which domains can login to NTCSS"
    echo "    NOTE-3: You may enter more than one domain separated by commas"
    echo " "
    read domname
    if [ -z "$domname" ]
    then
        domname=$defdom
    fi
    echo " "
    echo "Setting NT Domain name to" ${domname} >> $log
    echo "Setting NT Domain name to" ${domname}
    $ntcssdir/bin/ntcss_cmd comData set NTDOMAIN ${domname} 
}

do_ported_net_xfer()
{
#This version of net_xfer is used in ported systems for secure
#file transfers during ypmake, and also for updating applications files
    echo " "
    echo "Replacing net_xfer in /etc/yp" >> $log
    echo "Replacing net_xfer in /etc/yp"
    cp -p $ntcssdir/bin/net_xfer /etc/yp/net_xfer.exe
# For proper argument alignment
    cp -p $postdir/net_xfer.sh /etc/yp/net_xfer
    chmod 755 /etc/yp/net_xfer /etc/yp/net_xfer.exe
    cp -p $postdir/net_push /etc/yp/net_push
    chmod 755 /etc/yp/net_push
#This file used for launching remote programs
    echo " "
    echo "Replacing remsh in /usr/bin" >> $log
    echo "Replacing remsh in /usr/bin"
    cp -p $ntcssdir/bin/remsh /usr/bin
    chmod 755 /usr/bin/remsh
}

fix_printcap_file()
{
    if [ -f /etc/printcap ]
    then
        echo " "
        echo "Updating /etc/printcap for sd.. to /usr/spool/lp/request/.." >> $log
        echo "Updating /etc/printcap for sd.. to /usr/spool/lp/request/.."
        $ntcssdir/init_data/tools/update_pcap
    else
        echo " "
        echo "NO /etc/printcap detected" >> $log
        echo "NO /etc/printcap detected"
    fi
}

killprogd()
{
### This routine is used for SNAP I Ported, and Enterprise multi-unix systems. 
        echo " "
        echo "Killing old progd" >> $log
        echo "Killing old progd"
        ncline=`ps -ef | grep "progd" | egrep -v "grep"`
        if [ -n "${ncline}" ]
        then
                ncpid=`echo $ncline | sed -n "s/[^0-9]*\([^     ]*\).*/\1/p"`
                if [ -n "${ncpid}" ]
                then
                        kill -9 "$ncpid"
                fi
        fi
        cp -p $ntcssdir/bin/progd /etc
        chmod 755 /etc/progd
        echo "Starting new progd" >> $log
        echo "Starting new progd"
        /etc/progd &
        cp -p $ntcssdir/bin/remsh /usr/bin
        chmod 755 /usr/bin/remsh
}

do_rc_files()
{
#  For ported installs, copy chsrc's to home4root, edit NTCSSrc in /etc
    echo " "
    echo "Copying .cshrc to /home4root and /" >> $log
    echo "Copying .cshrc to /home4root and /"
    if [ `hostname` != "nmaster" ]
    then
        cp -p $appdir/data/unix_uga/shell/.cshrc /home4root  
        cp -p $appdir/data/unix_uga/shell/.cshrc /  
    fi
    echo " "
    echo "Copying .cshrc to /u/snapi" >> $log
    echo "Copying .cshrc to /u/snapi"
    cp -p $appdir/data/unix_uga/shell/.cshrc /u/snapi

    ckit=`grep "ntcss_init" /etc/NTCSSrc`
    if [ -z "$ckit" ]
    then
        echo " "
        echo "Editing NTCSSrc to start NTCSSII at boot" >> $log
        echo "Editing NTCSSrc to start NTCSSII at boot"
	cp /etc/NTCSSrc /etc/NTCSSrc.old
	cat /etc/NTCSSrc.old $postdir/NTCSSrc_s1p > /etc/NTCSSrc
    fi

ed - /etc/NTCSSrc <<EOF
/progd/
-2
.,.+3s/^/#/
a
# New Progd Invocation
.
w
q
EOF

    ckit=`grep "^/etc/progd" /etc/NTCSSrc"`
    if [ -z "$ckit" ]
    then
ed - /etc/NTCSSrc <<EOF
/New Progd Invocation/
a
/etc/progd &
.
w
q
EOF
    fi
}

do_rem_start()
{
# Copy new rem_start to /usr/bin
    echo " "
    echo "Copying rem_start to /usr/bin" >> $log
    echo "Copying rem_start to /usr/bin"
    chmod 755 $ntcssdir/bin/rem_start
    cp -p $ntcssdir/bin/rem_start /usr/bin
}

do_tput_cup()
{
# Copy tput_cup to /usr/local/bin if necessary
    if [ ! -f /usr/local/bin/tput_cup ]
    then
        echo " "
        echo "Copying tput_cup to /usr/local/bin" >> $log
        echo "Copying tput_cup to /usr/local/bin"
        cp -p $postdir/tput_cup_s1p /usr/local/bin/tput_cup
        chmod 755 /usr/local/bin/tput_cup
    fi
}

do_user_config()
{
    echo " "
    echo "Changing user_config to TELLPWD" >> $log
    echo "Changing user_config to TELLPWD"
    
ed /h/NTCSSS/applications/data/users_conf/user_config <<EOF
/TELLPWD
d
a
TELLPWD="YES"
.
w
EOF

}

do_host_id_file()
{
    echo "[`hostname`]" > $ntcssdir/database/HOST_ID
}

create_snap1_sdi()
{
# Create snapi SDI directories if needed
    echo " "
    echo "Creating SDI directories" >> $log
    echo "Creating SDI directories"
    if [ ! -d /u/SUADPS/data ]
    then
        mkdir -p /u/SUADPS/data
        chmod -R 777 /u/SUADPS
    fi
    if [ ! -d /u/OMMS/data ]
    then
        mkdir -p /u/OMMS/data
        chmod -R 777 /u/OMMS
    fi
    if [ ! -d /u/ADM/data ]
    then
        mkdir -p /u/ADM/data
        chmod -R 777 /u/ADM
    fi
# Install app_dirs for snapi ported class
    cp -p $postdir/app_dirs_s1p $appdir/data/srv_dev/app_dirs
}

do_snapi()
{
# Kill old progd, copy new progd and start it 
    killprogd

# Replace new snap env scripts
    do_snap_env

# Link /etc/logingroup to /etc/group
    update_logingroup

# Copy new net_xfer to /etc/yp
    do_ported_net_xfer

# Copy new rem_start to /usr/bin
    do_rem_start

# Edit user config with "allow password switch set"
    do_user_config
     
# Copy new progrps file to init_data
    if [ ! -f /upgrade ]
    then
        echo " "
        echo "Copying progrps file to init_data" >> $log
        echo "Copying progrps file to init_data"
        cp -p $postdir/progrps_s1p.ini $ntcssdir/init_data/progrps.ini
    fi

# Copy new csh stuff to home4root, /, /u/snapi; modify /etc/NTCSSrc
    do_rc_files    

# Copy appropiate MASTER file to database (srv0 vs rpsx vs nalc)
# and SYS_CONF file to establish user directories as /u
    do_database_files

# Copy tput_cup to /usr/local/bin if not there
    do_tput_cup

# Create SDI directories if needed
    create_snap1_sdi

# Update /etc/printcap
    fix_printcap_file

# Add nalcomis group
    add_nalc_group
   
# Remove /tmp/netxfer.log
    rm -f /tmp/netxfer.log
}

do_snap_env()
{
# Copy SNAP coe_cshrc, snap_u.csh_env, access_con, coe_login
# to /usr/local/NTCSS/env
    echo " "
    echo "Replacing SNAP coe_cshrc for xterm, snap_u.csh_env, access_con, coe_login" >> $log
    echo "Replacing SNAP coe_cshrc for xterm, snap_u.csh_env, access_con, coe_login"
    mv /usr/local/NTCSS/env/coe_cshrc /usr/local/NTCSS/env/coe_cshrc.org
    cp -p $postdir/coe_cshrc_ported /usr/local/NTCSS/env/coe_cshrc
    chmod 755 /usr/local/NTCSS/env/coe_cshrc
    cp -p $postdir/access_con_ported /usr/local/NTCSS/env/access_con
    chmod 755 /usr/local/NTCSS/env/access_con
    cp -p $postdir/coe_login_ported /usr/local/NTCSS/env/coe_login
    chmod 755 /usr/local/NTCSS/env/coe_login
    cp -p $postdir/snap_u.csh_env_ported /usr/local/NTCSS/env/snap_u.csh_env
    chmod 755 /usr/local/NTCSS/env/snap_u.csh_env
# Replace catch_warn with null version to insure no catch_warns, 7/10/98, fgr
    cp -p $postdir/catch_warn /usr/local/bin/catch_warn
    chmod 755 /usr/local/bin/catch_warn
# Replace NTCSS_Xsession, Xreset to eliminate any catch_warn stuff
    cp -p $postdir/NTCSS_Xsession /usr/local/NTCSS/xdm/NTCSS_Xsession
    cp -p $postdir/Xreset /usr/local/NTCSS/xdm/Xreset
    chmod 755 /usr/local/NTCSS/xdm/NTCSS_Xsession
    chmod 755 /usr/local/NTCSS/xdm/Xreset
}


do_snapii()
{
# Kill, replace and start new progd
    killprogd

# Update /etc/logingroup to link to /etc/group
    update_logingroup

# Edit user_config to enable Unix passwords in registrations
    do_user_config

# Copy new progrps file to init_data
    if [ ! -f /upgrade ]
    then
        echo " "
        echo "Copying progrps file to init_data" >> $log
        echo "Copying progrps file to init_data"
        cp -p $postdir/progrps_s2p.ini $ntcssdir/init_data/progrps.ini
    fi

# 7/9/98, fgr
# Copy new csh stuff to home4root, /, /u/snapi; modify /etc/NTCSSrc
    do_rc_files

# Copy SYS_CONF to database to establish user directories
    if [ ! -f /keepnt ]
    then
        echo " "
        echo "Copying SYS_CONF to database to establish user directories" >> $log
        echo "Copying SYS_CONF to database to establish user directories"
        cp -p $postdir/SYS_CONF /h/NTCSSS/database
    fi

# Replace net_xfer in /etc/yp with ntcssii version
    do_ported_net_xfer

# Copy SNAP coe_cshrc, snap_u.csh_env, access_con, coe_login
# to /usr/local/NTCSS/env
    do_snap_env

# Create SDI directories if needed
    echo " "
    echo "Creating SDI directories" >> $log
    echo "Creating SDI directories"
    if [ ! -d /u/SFM/data ]
    then
        mkdir -p /u/SFM/data
        chmod -R 777 /u/SFM
    fi
    if [ ! -d /u/MDS/data ]
    then
        mkdir -p /u/MDS/data
        chmod -R 777 /u/MDS
    fi
    if [ ! -d /u/ADM/data ]
    then
        mkdir -p /u/ADM/data
        chmod -R 777 /u/ADM
    fi
    if [ ! -d /u/SMS/data ]
    then
        mkdir -p /u/SMS/data
        chmod -R 777 /u/SMS
    fi

# Install app_dirs for snapii ported class
    cp -p $postdir/app_dirs_s2p $appdir/data/srv_dev/app_dirs

# Update /etc/printcap
    fix_printcap_file
}

do_rps0()
{
    echo " "
    echo "do_rpsX/nalc, setting opt=n, rps=y" >> $log
    echo "do_rpsX/nalc, setting opt=n, rps=y"
    opt="n"
    rps="y"
    echo "forcing snap=snapi" >> $log
    echo "forcing snap=snapi"
    snap="snapi"
    do_database_files
    killprogd
    do_ported_net_xfer
    do_rem_start
    do_rc_files
    do_tput_cup
    create_snap1_sdi
    do_snap_env
    fix_printcap_file
    do_host_id_file
}

do_rps1()
{
    echo " "
    echo "do_rps1, executing do_rps0" >> $log
    echo "do_rps1, executing do_rps0"
    do_rps0
}

do_rps2()
{
    echo " "
    echo "do_rps2, executing do_rps0" >> $log
    echo "do_rps2, executing do_rps0"
    do_rps0
}

do_nalc()
{
    if [ $ported_nalc = "y" ]
    then
        echo " "
        echo "do_nalc-snapi, executing do_rps0" >> $log
        echo "do_nalc-snapi, executing do_rps0"
        do_rps0
    else

        echo " "
    	sld_arg="nalc"
        addsource
        if [ $dualnalc = "n" ]
        then
            echo " "
            echo "do_nalc, pointing nalc to rsupmast" >> $log
            echo "do_nalc, pointing nalc to rsupmast"
            cp -p $postdir/MASTER_enterprise $ntcssdir/database/MASTER
        else
            echo " "
            echo "doing dual nalc, pointing nalc to nalcmast" >> $log
            echo "doing dual nalc, pointing nalc to nalcmast"
            cp -p $postdir/MASTER_dual_nalc $ntcssdir/database/MASTER
        fi
        do_host_id_file
    fi
    killprogd
}

do_sdb()
{
    echo " "
    sld_arg="nalc"
    cp -p $postdir/progrps_oceana_inhouse.ini \
          $ntcssdir/init_data/progrps.ini
    addsource
    killprogd
}


start_ntcss_master()
{
    echo " "
    echo "Starting NTCSS II" >> $log
    echo "Starting NTCSS II"
    $ntcssdir/bin/ntcss_init -noboot
    #The next command must be done after the ntcssii gens are started
    echo "7 Second Delay to let NTCSS start....."
    sleep 4
    echo "3 Seconds left....."
    sleep 3
    if [ $keepnt = "n" ]
    then
        if [ $master_server = "y" ]
        then
            get_domain_name
        fi
    fi
}

add_bt_users_to_ntcss()
{
    # Add btusrs to ntcss the manual way..
    bt_there=`grep "^ntcss:" /etc/group | grep "btusr"`
    if [ -z "${bt_there}" ]
    then
        if [ $hostname = "nalcmast" ]
        then
            ed - /etc/group <<EOF
/^ntcss:/
s/$/imabtusr,omabtusr,supbtusr/
w
q
EOF
        else
            ed - /etc/group <<EOF
/^ntcss:/
s/$/imabtusr,omabtusr,supbtusr,admbtusr,omsbtusr/
w
q
EOF
        fi
        do_push
    fi
}

ima2rsup()
{
    # imabtusr -> rsupply
    echo "Adding imabtusr to rsupply" >> $log
    echo "Adding imabtusr to rsupply"
    $ucdir/user_config ADDGROUP imabtusr rsupply
}
oma2rsup()
{
    # omabtusr -> rsupply
    echo "Adding omabtusr to rsupply" >> $log
    echo "Adding omabtusr to rsupply"
    $ucdir/user_config ADDGROUP omabtusr rsupply
}
oms2rsup()
{
    # omsbtusr -> rsupply
    echo "Adding omsbtusr to rsupply" >> $log
    echo "Adding omsbtusr to rsupply"
    $ucdir/user_config ADDGROUP omsbtusr rsupply
}
oms2nalc()
{
    # omsbtusr -> nalcomis
    echo "Adding omsbtusr to nalcomis" >> $log
    echo "Adding omsbtusr to nalcomis"
    $ucdir/user_config ADDGROUP omsbtusr nalcomis
}
oma2nalc()
{
    # omabtusr -> nalcomis
    echo "Adding omabtusr to nalcomis" >> $log
    echo "Adding omabtusr to nalcomis"
    $ucdir/user_config ADDGROUP omabtusr nalcomis
}
oma2appnalc()
{
    # omabtusr -> ntcss_app_nalc
    echo "Adding omabtusr to ntcss_app_nalc" >> $log
    echo "Adding omabtusr to ntcss_app_nalc"
    $ucdir/user_config ADDGROUP omabtusr ntcss_app_nalc
}
sup2nalc()
{
    # supbtusr -> nalcomis
    echo "Adding supbtusr to nalcomis" >> $log
    echo "Adding supbtusr to nalcomis"
    $ucdir/user_config ADDGROUP supbtusr nalcomis
}
ima2apprs()
{
    # imabtusr -> ntcss_app_rs
    echo "Adding imabtusr to ntcss_app_rs" >> $log
    echo "Adding imabtusr to ntcss_app_rs"
    $ucdir/user_config ADDGROUP imabtusr ntcss_app_rs
}
oma2apprs()
{
    # omabtusr -> ntcss_app_rs
    echo "Adding omabtusr to ntcss_app_rs" >> $log
    echo "Adding omabtusr to ntcss_app_rs"
    $ucdir/user_config ADDGROUP omabtusr ntcss_app_rs
}
ima2appnalc()
{
    # imabtusr -> ntcss_app_nalc
    echo "Adding imabtusr to ntcss_app_nalc" >> $log
    echo "Adding imabtusr to ntcss_app_nalc"
    $ucdir/user_config ADDGROUP imabtusr ntcss_app_nalc
}
sup2apprs()
{
    # supbtusr -> ntcss_app_rs
    echo "Adding supbtusr to ntcss_app_rs" >> $log
    echo "Adding supbtusr to ntcss_app_rs"
    $ucdir/user_config ADDGROUP supbtusr ntcss_app_rs
}
sup2appnalc()
{
    # supbtusr -> ntcss_app_nalc
    echo "Adding supbtusr to ntcss_app_nalc" >> $log
    echo "Adding supbtusr to ntcss_app_nalc"
    $ucdir/user_config ADDGROUP supbtusr ntcss_app_nalc
}
ima2ntcssapp()
{
    echo "Adding imabtusr to ntcss_app" >> $log
    echo "Adding imabtusr to ntcss_app"
    $ucdir/user_config ADDGROUP imabtusr ntcss
}
ima2oma()
{
    # imabtusr -> oma
    echo "Adding imabtusr to oma" >> $log
    echo "Adding imabtusr to oma"
    $ucdir/user_config ADDGROUP imabtusr oma
}

webgroups()
{

    echo " "
    echo "Adding user <www> to each application's unix group so they can"
    echo "use the web server for their application"
    echo "Adding user <www> to each application's unix group" >> $log
    echo " "

    {
    while (true)
    do
        read appname
        if [ $? = "0" ]
        then
            unix_group=`$gps $appname UNIX_GROUP ntcss $progrps_file`
            $ucdir/user_config ADDGROUP www $unix_group
            if [ $? != "0" ]
            then
                echo " "
                echo "Unable to add user <www> to $unix_group"
                echo "Unable to add user <www> to $unix_group" >> $log
            else
                echo "User <www> added to $unix_group"
                echo "User <www> added to $unix_group" >> $log
            fi
        else
            break
        fi
    done
    } < $dbdir/APP_ID

}


ck_for_omms()
{
#
#
#    check if this is an OMMSNG host or a host that has OMMSNG
#    on it. If so, then the apache web server should be installed.
#

doweb="n"
if [ `hostname` = "OMMS" ]
then
    doweb="y"
else
    if [ `hostname` = "omms" ]
    then
        doweb="y"
    else
        if [ `hostname` = "rsupmast" ]
        then
            grep "OMMS" $progrps_file >/dev/null
            ckstat=$?
            if [ $ckstat = 0 ]
            then
                doweb="y"
            fi
        fi
    fi
fi

if [ -f /force_omms ]
then
    doweb="y"
fi

# echo "doweb = $doweb"
#
#
#
}


#============ Post install begins here ===============================

# Initialize for run

#
#   Force remove the /upgrade file.....6.26.2002...DJP
#

rm -f /upgrade

    initialize

    if [ $hostname != "omms" -a $hostname != "radm" -a $hostname != "nalc" -a $hostname != "rps0" -a $hostname != "rps1" -a $hostname != "rps2" -a $hostname != "s235_25" -a $hostname != "s010x083" -a $hostname != "s234x201" -a $hostname != "sdb" ]
    then
        master_server="y"
    fi

    if [ -d $ntcssdir/pcload ]
    then
        echo " "
        echo "Removing pcload directory" >> $log
        echo "Removing pcload directory"
        rm -r $ntcssdir/pcload
    fi
    if [ -d $ntcssdir/web ]
    then
        if [ -f $ntcssdir/web/bin/stopserver.sh ]
        then
            ncline=`ps -ef | grep "httpd" | egrep -v "grep"`
            if [ -n "${ncline}" ]
            then
                su - www -c "$ntcssdir/web/bin/stopserver.sh"
            fi
        fi
        if [ ! -f /keep_old_web ]
        then
            echo " "
            echo "Removing web directory" >> $log
            echo "Removing web directory"
            rm -r $ntcssdir/web
        fi
    fi
    if [ -d $ntcssdir/init_data/tools ]
    then
        rm -r $ntcssdir/init_data/tools
        echo " "
        echo "Removing tools directory" >> $log
        echo "Removing tools directory"
    fi
    mv $postdir/pcload $ntcssdir/pcload
    echo " "
    echo "New pcload directory has been installed" >> $log
    echo "New pcload directory has been installed"
    cp -p $bindir/u2n_convert $postdir/tools
    cp -p $bindir/nalc_conv $postdir/tools
    cp -p $bindir/u2n_convert $postdir/tools/opt
    cp -p $bindir/nalc_conv $postdir/tools/opt
    mv $postdir/tools $ntcssdir/init_data/tools
    echo " "
    echo "New tools directory has been installed" >> $log
    echo "New tools directory has been installed"

# 7/24/98 Fix /etc/group file to remove any occurences of "nt_".
    curdir=`pwd`
    cd $ntcssdir/init_data/tools/opt
    $postdir/update_nt_groups
    cd $curdir

    chmod 755 $ntcssdir/bin/net_xfer
    chmod 755 $ntcssdir/bin/progd
    chmod 755 $ntcssdir/bin/remsh

# sethost and prime must be run to get ntcss group installed
# This sethost and prime is against the generic NTCSS progrps file
    #if [ $master_server = "y" ]
    #then
        #if [ "$enterprise" = "y" ]
	#then
		#cp $postdir/progrps_enterprise.ini $ntcssdir/init_data/progrps.ini
        #elif [ "$opt_mals" = "y" ]
	#then
		#cp -p $postdir/progrps_mals2.ini $ntcssdir/init_data/progrps.ini
	#else
		#cp -p $postdir/progrps_stout.ini $ntcssdir/init_data/progrps.ini
	#fi
        #sethost      # This sethost is against the generic progrps.ini file
        #do_prime     # Get ntcss group into the /etc/group file
        add_ntcssii  # Add user ntcssii
        #add_www      # Add user www
    #fi

# Backup things we'll change
    do_backups

echo " "
case $hostname in
    "rsupmast")
        echo "case-rsupmast" >> $log
        echo "case-rsupmast"
        opt="y"
        rps="n"
        do_rsupmast
    ;;
    "sdbdev")
        echo "case-sdbdev" >> $log
        echo "case-sdbdev"
        opt="y"
        rps="n"
        inhouse="y"
        do_sdbdev
    ;;
    "nalcmast")
        echo "case-nalcmast" >> $log
        echo "case-nalcmast"
        opt="y"
        rps="n"
        do_nalcmast
    ;;
    "nmaster")
        do_nmaster
    ;;
    "tac4cm")
        do_tac4cm
    ;;
    "srv0")
        echo "case-srv0" >> $log
        echo "case-srv0"
        opt="n"
        rps="n"
        if [ $snap = "snapi" ]
        then
            echo "case-srv0-snapi" >> $log
            echo "case-srv0-snapi"
            do_snapi
	    # Pull off C2 security for ported
      	    $postdir/desecure MASTER
	    # update exports for nalcomis
            cp $postdir/exports.nalc /etc/exports
            if [ `hostname` != "nmaster" ]
            then
               /usr/etc/exportfs -a
            fi
        fi
        if [ $snap = "snapii" ]
        then
            echo "case-srv0-snapii" >> $log
            echo "case-srv0-snapii"
            do_snapii
        fi
    ;;
    "rps0")
        echo "case-rps0" >> $log
        echo "case-rps0"
        do_rps0
	# Pull of C2 security for ported
      	$postdir/desecure SLAVE
    ;;
    "rps1")
        echo "case-rps1" >> $log
        echo "case-rps1"
        do_rps1
	# Pull of C2 security for ported
      	$postdir/desecure SLAVE
    ;;
    "rps2")
        echo "case-rps2" >> $log
        echo "case-rps2"
        do_rps2
	# Pull of C2 security for ported
      	$postdir/desecure SLAVE
    ;;
    "nalc")
        echo "case-nalc" >> $log
        echo "case-nalc"
        do_nalc
        if [ $enterprise = "n" -a $ported_nalc = "y" ]
        then
            # add /u mount to checklist and then remount
            cline=`grep "srv0" /etc/checklist`
	    if [ -z "$cline" ]
	    then
	        if [ -h /u ]
	        then
		    rm /u
		    mkdir /u
		    chmod 777 /u
		fi
		cat $postdir/checklist.nalc >> /etc/checklist
		/etc/mount -a
	    fi
	    # Pull of C2 security for ported
      	    $postdir/desecure SLAVE

	    #TONY : add the new scripts for nalcomis
            if [ ! -d /usr/hvx_scripts ]
	    then
	    	mkdir -p /usr/hvx_scripts
	    	chmod  777 /usr/hvx_scripts
	    	cp $postdir/hvx_scripts/* /usr/hvx_scripts
	    	chmod 755 /usr/hvx_scripts/*
		cp /usr/local/NTCSS/desktop/Menus /usr/local/NTCSS/desktop/Menus.prenalc
		
		# We change the file instead of copy over it just in case the
                # site has modified the file

ed - /usr/local/NTCSS/desktop/Menus <<EOF
/Backup HVX
c
Backup HVX Files System;nalcomis;xterm -T Backup_File_System -e /usr/hvx_scripts/hvx-backup; \
.
1
/Restore HVX
c
Restore HVX Files System;nalcomis;xterm -T Restore_File_System -e /usr/hvx_scripts/hvx-restore; \
.
1
/HVX Initiation
c
HVX Initiation;nalcomis;xterm -T GCOS_Console -e /usr/hvx_scripts/nalc_gen;; \
.
w
q
EOF
	    fi
        fi
    ;;
    "omms")
        echo "case-omms" >> $log
        echo "case-omms"
        do_omms
    ;;
    "radm")
        echo "case-radm" >> $log
        echo "case-radm"
        do_radm
    ;;
#    "nalcrte")
#        echo "case-nalcrte" >> $log
#        echo "case-nalcrte"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_nalcrte
#    ;;
#    "t4naltst")
#        echo "case-t4naltst" >> $log
#        echo "case-t4naltst"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_t4naltst
#    ;;
#    "hpnaldev")
#        echo "case-hpnaldev" >> $log
#        echo "case-hpnaldev"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_hpnaldev
#    ;;
#    "t4omsdev")
#        echo "case-t4omsdev" >> $log
#        echo "case-t4omsdev"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_t4omsdev
#    ;;
#    "omms_tst")
#        echo "case-omms_tst" >> $log
#        echo "case-omms_tst"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_omms_tst
#    ;;
#    "hprsdev")
#        echo "case-hprsdev" >> $log
#        echo "case-hprsdev"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_hprsdev
#    ;;
#    "t4rstest")
#        echo "case-t4rstest" >> $log
#        echo "case-t4rstest"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_t4rstest
#    ;;
#    "t4431")
#        echo "case-t4431" >> $log
#        echo "case-t4431"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_t4431
#    ;;
#    "s234x83")
#        echo "case-s234x83" >> $log
#        echo "case-s234x83"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_s234x83
#    ;;
#    "s235_25")
#        echo "case-s235_25" >> $log
#        echo "case-s235_25"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_s235_25
#    ;;
#    "s010x083")
#        echo "case-s010x083" >> $log
#        echo "case-s010x083"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_s010x083
#    ;;
#    "s234x201")
#        echo "case-s234x201" >> $log
#        echo "case-s234x201"
#        inhouse="y"
#        rps="n"
#        opt="y"
#        enterprise="n"
#        do_s234x201
#    ;;
     *)
         echo "case-inhouse" >> $log
         echo "case-inhouse"
         inhouse="y"
esac

##########TONY TESTING #############
    if [ $master_server = "y" ]
    then
        sethost      # This sethost is against the generic progrps.ini file
        do_prime     # Get ntcss group into the /etc/group file
        add_ntcssii  # Add user ntcssii
        add_www      # Add user www
    fi
##########TONY TESTING #############

    echo "end-case" >> $log
    echo "end-case"

# Update the user directories, /u or /h/USERS 
    $postdir/updateu.sh $inhouse | tee /h/NTCSSS/passwd.lst

# Install fs_archive for the specific system being installed
    init_fs_archive

# Insure that a printcap file exists in any case
    if [ ! -f /etc/printcap ]
    then
        touch /etc/printcap
    fi

# Create directories for backups, etc.
    make_ntcssii_dir
    make_ntcss_dir


# Do sethost and prime_db and add boot users (against the specific progrps)
    if [ $opt = "y" -a $master_server = "y" ]
    then
        if [ $enterprise = "n" ]
        then
            #OPTIMIZED MASTER SERVER BUT NOT ENTERPRISE
            echo " "
            echo "Making IP address for NTCSS hosts file via sethost" >> $log
            echo "Making IP address for NTCSS hosts file via sethost"
            sethost
            sethost_done="y"
        fi
        if [ $opt_mals = "y" ]
        then
            # OPTIMIZED MALS
            if [ $keepip = "n" ]
            then
                echo "Making IP address for the nalc machine" >> $log
                echo "Making IP address for the nalc machine"
                $postdir/sethost.sh
            fi
        fi 
        if [ $enterprise = "y" -a $hostname = "rsupmast" ]
        then
            #OPTIMIZED MASTER SERVER AND ENTERPRISE
            echo " "
            echo "hostname=$hostname, enterprise=$enterprise"
            if [ $keepip = "n" ]
            then
                echo " "
                #echo "Making IPs for NTCSS hosts file via manual input" >> $log
                #echo "Making IPs for NTCSS hosts file via manual input"
                #do_enterprise_host_file
                echo "Setting hosts for rsupmast" >> $log
                echo "Setting hosts for rsupmast" 
		sethost
            	sethost_done="y"
            fi
        fi
        if [ $inhouse = "y" ]
        then
            if [ $sethost_done = "n" ]
            then
                sethost
            fi
        fi
        if [ $master_server = "y" ]
        then
            echo " "
            echo "This is a master computer for optimized systems" >> $log
            echo "This is a master computer for optimized systems"
	    rm -f $ucdir/pmap.ini
            if [ $keepnt = "n" ]
            then
                get_nt_wd
            fi
            do_prime

	    if [ $enterprise = "y" -a $inhouse = "n" ]
	    then
                # ENTERPRISE
                echo " "
    	        echo "Enabling boot user accounts" >> $log
    	        echo "Enabling boot user accounts"
    	        $postdir/enable_account.sh imabtusr
    	        $postdir/enable_account.sh supbtusr
    	        $postdir/enable_account.sh omabtusr
    	        $postdir/enable_account.sh omsbtusr
    	        $postdir/enable_account.sh admbtusr
    	        $postdir/enable_account.sh sybbtusr
                # Make boot users members of each other's application
                # imabtusr -> rsupply
                ima2rsup
                # omsbtusr -> rsupply
                oms2rsup
                # omsbtusr -> nalcomis
                oms2nalc
                # omabtusr -> nalcomis
                oma2nalc
                # supbtusr -> nalcomis
                sup2nalc
                # omabtusr -> ntcss_app_nalc
                oma2appnalc
                # imabtusr -> ntcss_app_nalc
                ima2appnalc
                # supbtusr -> ntcss_app_nalc
                sup2appnalc
                # omabtusr -> ntcss_app_rs
                oma2apprs
                # imabtusr -> ntcss_app_rs
                ima2apprs
                # supbtusr -> ntcss_app_rs
                sup2apprs
                # add bt-users to ntcss
		add_bt_users_to_ntcss
	    fi

            if [ $hostname = nalcmast ]
            then
                # AIR STATION
                # omabtusr -> nalcomis
                oma2nalc
                # imabtusr -> ntcss_app
                ima2ntcssapp
                # imabtusr -> oma
                ima2oma
                oma2nalc
                # When RSUPPLY gets to an Air Station, they will need this..
                # supbtusr -> nalcomis
                sup2nalc
                # imabtusr -> rsupply
                ima2rsup
                # omabtusr -> ntcss_app_nal
                # Give boot users passwords to enable rcp to function - YUK..
                echo "Enabling ima and oma boot user accounts" >> $log
                echo "Enabling ima and oma boot user accounts"
                $postdir/enable_account.sh imabtusr
                $postdir/enable_account.sh omabtusr
                # add bt-users to ntcss
                # Don't understand why they say they need this..
                add_bt_users_to_ntcss
            fi

            if [ $opt_mals = "y" ]
            then
                # Make boot users members of each other's application
                # imabtusr -> rsupply
                ima2rsup
                # omabtusr -> rsupply
                oma2rsup
                # omabtusr -> nalcomis
                oma2nalc
                # supbtusr -> nalcomis
                sup2nalc
                # omabtusr -> ntcss_app_nalc
                oma2appnalc
                # supbtusr -> ntcss_app_nalc
                sup2appnalc
                # omabtusr -> ntcss_app_rs
                oma2apprs
                # imabtusr -> ntcss_app_rs
                ima2apprs
                echo "Enabling ima, sup, syb and oma boot user accounts" >> $log
                echo "Enabling ima, sup, syb and oma boot user accounts"
                $postdir/enable_account.sh imabtusr
                $postdir/enable_account.sh supbtusr
                $postdir/enable_account.sh omabtusr
                $postdir/enable_account.sh sybbtusr
                # add bt-users to ntcss
                add_bt_users_to_ntcss
            fi
        fi
    fi

#    # Assign passwords to bt users
#    rm -f nu_temp
#    touch nu_temp
#    {
#    skip="0"
#    while (true)
#    do
#        read "nu_record" 
#        if [ $? != "0" ]
#        then
#            cp nu_temp $dbdir/nu_temp
#            echo "btusr's assigned passwords.."
#            break
#        fi
#        btusr=`echo "$nu_record" | grep "btusr"`
#        if [ $? = "0" ]
#        then
#            echo "$nu_record" >> nu_temp
#            echo "Batch User" >> nu_temp
#            echo ".,KU2%~ExIf{Hr|>7 " >> nu_temp
#            skip="1"
#        fi
#        if [ "$skip" = "1" ]
#        then
#            if [ "$nu_record" = "000-00-0000" ]
#            then
#                echo "$nu_record" >> nu_temp
#                skip="0"
#            fi
#        else
#            echo "$nu_record" >> nu_temp
#        fi
#    done
#    } < $dbdir/ntcss_users

    if [ $hostname = "srv0" -a $snap = "snapi" ]
    then
        echo " "
        echo "hostname=srv0 and snap=snapi" >> $log
        echo "hostname=srv0 and snap=snapi"
        if [ $keepip = "n" ]
        then
            do_s1p_host_file
        fi
        if [ $keepnt = "n" ]
        then
            get_nt_wd
        fi
        do_prime
    fi

    if [ $hostname = "srv0" -a $snap = "snapii" ]
    then
        echo " "
        echo "hostname=srv0 and snap=snapii" >> $log
        echo "hostname=srv0 and snap=snapii"
        sethost
        if [ $keepnt = "n" ]
        then
	    get_nt_wd
        fi
        do_prime
    fi

# Install Web Server
    if [ $opt = "y" ]
    then
        ck_for_omms
        if [ "$doweb" = "y" ]
        then
          if [ ! -f /keep_old_web ]
          then
            if [ $PLATFORM = "HP-UX" ]
            then
                mv $postdir/web_hp $ntcssdir
                mv $ntcssdir/web_hp $ntcssdir/web
            fi
            if [ $PLATFORM = "SunOS" ]
            then
                mv $postdir/web_sun $ntcssdir
                mv $ntcssdir/web_sun $ntcssdir/web
            fi
            echo " "
            echo "Executing do_web.sh" >> $log
            echo "Executing do_web.sh"
            $postdir/do_web.sh $hostname
            echo "Web installed" >> $log
            echo "Web installed"
          fi
        fi
        if [ ! -f $ntcssdir/bin/ntcss_start ]
        then
            #cp -p $postdir/ntcssinit $ntcssdir/bin/ntcss_init
            #cp -p $postdir/ntcsshalt $ntcssdir/bin/ntcss_halt
            chmod 700 $ntcssdir/bin/ntcss_init
            chmod 700 $ntcssdir/bin/ntcss_halt
        fi
    else
        rm -r $postdir/web
    fi

# Do NIS push if on NIS Master
    do_push

# Start NTCSS II (required for optimized to setlinkdata)
    start_ntcss_master

# Set link Data (ntcss_init must have been executed, optimized only)
    if [ $opt = "y" ]
    then
        if [ $inhouse = "n" ]
        then
            echo " "
            echo "Setting Link Data" >> $log
            echo "Setting Link Data"
            $ntcssdir/bin/setlinkdata $sld_arg
        else
            echo " "
            echo "Skipping setlinkdata for inhouse systems" >> $log
            echo "Skipping setlinkdata for inhouse systems"
        fi

        if [ "$check_ots" = "y" ]
	then
		if [ -s "$otfile" ]
		then
			cp -p $otfile $ntcssdir/database/output_type
		fi
		if [ -s "$opfile" ]
		then
			cp -p $opfile $ntcssdir/database/output_prt
		fi
	fi
    fi

# Stop ntcssii
# TONY
    touch /h/NTCSSS/database/ntprint.ini
    #if [ $master_server = "y" ]
    #then
        #mascp=`crontab -l | grep "master copy"`
        #if [ -z "$mascp" ]
        #then
            #if [ $enterprise = "y" ]
            #then
            ##ENTERPRISE
                #echo " "
                #echo "NOTE: The following commands will fail if the"
                #echo "      nalc, omms, or radm servers (NTCSSII gens)"
                #echo "      are down..."
                #echo "      Please ignore any errors at this point..."
                #echo "      The install program is attempting to configure"
                #echo "      the NTCSSII Database replication service."
                #echo "      It will replicate the NTCSSII database to"
                #echo "      host: nalc..."
                #echo " "
                #$ntcssdir/bin/ntcss_cmd master copy nalc
            #fi
            #if [ $dualnalc = "y" ]
            #then
                #$ntcssdir/bin/ntcss_cmd master copy nalc
            #fi
            #if [ $hostname = "rsupmast" -a $enterprise = "n" ]
            #then
                #if [ $opt_mals = "y" ]
                #then
                    ##MALS
                    #$ntcssdir/bin/ntcss_cmd master copy nalc
                #else
                    ##STOUT
                    #$ntcssdir/bin/ntcss_cmd master copy $hostname
                #fi
            #fi
            #if [ $snap = "snapi" ]
            #then
            ##PORTED SNAP-I
                #echo " "
                #echo "NOTE: The following command will fail if the"
                #echo "      nalc server (NTCSSII gens) are down..."
                #echo "      Please ignore any errors at this point..."
                #echo "      The install program is attempting to configure"
                #echo "      the NTCSSII Database replication service."
                #echo "      It will replicate the NTCSSII database to"
                #echo "      host: nalc..."
                #echo " "
                #$ntcssdir/bin/ntcss_cmd master copy nalc
            #fi
            #if [ $snap = "snapii" ]
            #then
            ##PORTED SNAP-II
                #$ntcssdir/bin/ntcss_cmd master copy $hostname
            #fi
            ##$ntcssdir/bin/ntcss_cmd master addsched 30
        #fi
    #fi
#

  if [ "$doweb" = "y" ]
  then
    webgroups
  fi

    echo " "
    echo "Halting NTCSS II" >> $log
    echo "Halting NTCSS II....ignore 'terminating messages'"
    $ntcssdir/bin/ntcss_halt

# Copy in  "isntup" and "refresh_nis_maps.sh", etc. to bin directory
    echo " "
    echo "Copying isntup script to bin directory" >> $log
    echo "Copying isntup script to bin directory"
    cp -p $postdir/isntup $ntcssdir/bin
    chmod 744 $ntcssdir/bin/isntup 
    echo "Copying refresh_nis_maps.sh to bin directory" >> $log
    echo "Copying refresh_nis_maps.sh to bin directory"
    cp -p $postdir/refresh_nis_maps.sh $bindir
    chmod 700 $bindir/refresh_nis_maps.sh
    echo "copying getcrc, upper & fdump to post_install directory" >> $log
    echo"copying getcrc, upper & fdump to post_install directory"
    cp -p $bindir/getcrc $postdir
    cp -p $bindir/fdump $postdir
    cp -p $bindir/upper $postdir
    echo "Copying setup.sh, shlib and SITE_FILE to init_data directory" >> $log
    echo "Copying setup.sh, shlib and SITE_FILE to init_data directory"
    cp -p $postdir/setup.sh $init_data_dir/setup.sh
    cp -p $postdir/shlib $init_data_dir/shlib
    cp -p $postdir/SITE_FILE $init_data_dir/SITE_FILE
    chmod 700 $init_data_dir/setup.sh
    chmod 700 $init_data_dir/shlib
    chmod 700 $init_data_dir/SITE_FILE
    echo "Copying lp_users.sh to bin directory" >> $log
    echo "Copying lp_users.sh to bin directory"
    cp -p $postdir/lp_users.sh $bindir
    chmod 755 $bindir/lp_users.sh
#   echo "Copying print_que to bin directory" >> $log
#   echo "Copying print_que to bin directory"
#   cp -p $postdir/print_que $bindir
#   chmod 755 $bindir/print_que
#
#   commented out above print_que copy per instructions  3/15/01 djp
#
#

    if [ $PLATFORM = "HP-UX" ]
    then
        # Needed for fs_archive to work
        if [ ! -f /usr/lib/Motif1.2/libXm.1 ]
        then
            echo "Copying libXm.1 to /usr/lib/Motif1.2" >> $log
            echo "Copying libXm.1 to /usr/lib/Motif1.2"
            cp -p $postdir/libXm.1 /usr/lib/Motif1.2/libXm.1
            chmod 775 /usr/lib/Motif1.2/libXm.1
        fi
    fi

# Remove the "force Initialize file from root, so that the default is update"
    rm  -f /upgrade /save_output_types /force_rsupmast /force_nalcmast
    rm -f /force_srv0 /force_snapi /force_snapii /force_enterprise
    rm -f /force_radm /force_omms /force_nalc /force_rps
    rm -f /force_nalcrte /force_t4naltst /force_hpnaldev /force_t4omsdev
    rm -f /force_omms_tst /force_hprsdev /force_t4rstest /force_t4431
    rm -f /force_s234x83 /force_s235_25 /force_s010x083 /force_s234x201
    rm -f /force_tac4cm /force_dualnalc
    rm -f /skip_updateu /keepip /keepnt /keep_old_web
    
# Final warnings
    if [ $snap = "snapi" -a $hostname = "srv0" ]
    then
        more $postdir/final_s1p
        echo " "
        echo "NTCSSII has not been started.."
    fi

    if [ $rps = "y" ]
    then
        more $postdir/final_s1p
        echo "NTCSSII has not been started.."
        echo " "
    fi

    if [ $snap = "snapii" -a $hostname = "srv0" ]
    then
        echo "NTCSSII has not been started.."
        echo " "
    fi

    if [ $opt = "y" ]
    then
        cat $postdir/final_master
        echo " "
        echo "NTCSSII has not been started.."
        echo " "
    fi


