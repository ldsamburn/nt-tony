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
# define_variables.sh 
#
#     Sourced by any shell that needs the below environment
#           variables.
#
#
#   16 Jan 2001                  
#
#=========================================================================
#
#   appdir
#   appinst
#   bindir
#   dbdir
#   idir
#   init_data_dir
#   gps  (GetProfileString)
#   gsv  (GetSectionValues)
#   log
#   ntcsscmd
#   ntcssdir
#   postdir
#   progrps_file
#   rmps (RemoveProfileString)
#   sdi_dir
#   settings
#   sld_arg
#   uc_prog
#   uc_dir
#   user_dir
#   webdir
#   wps  (WriteProfileString)
#     

def_caller=$1

    settings="/etc/ntcss_system_settings.ini"

    #---NTCSSV home directory
    ntcssdir=`grep "^HOME_DIR=" "$settings" | cut -d= -f2`
    
    #---Install log file
    log=$ntcssdir/post-install.log
    if [ "$appdir" = "$ntcssdir/applications" ]
    then
        echo " "
    else
        echo "_____INSTALL STARTED at `date`--called by $def_caller-------" >> $log
    fi

    echo "NTCSS Home Directory: <$ntcssdir>" >> $log
    #echo "NTCSS Home Directory: <$ntcssdir>"


    #---NTCSS bin directory
    bindir=`grep "NTCSS_BIN_DIR" "$settings" | cut -d= -f2`
    bindir="$ntcssdir/$bindir"
    echo "NTCSS Bin directory: <$bindir>" >> $log
    #echo "NTCSS Bin directory: <$bindir>"

    #---Put bin directory in path
    PATH=$PATH:${bindir}:/usr/local/bin;export PATH

    #---Set LD path for print_ntcss_system_settings
    LD_LIBRARY_PATH=/usr/lib:/lib:/usr/ucblib:/h/NTCSSS/libs;export LD_LIBRARY_PATH

    #---GetProfileString, WriteProfileString, GetSectionValues
    gps="$bindir/GetProfileString"
    wps="$bindir/WriteProfileString"
    gsv="$bindir/GetSectionValues"
    rmps="$bindir/RemoveProfileString"

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

    sld_arg=`hostname`

###########################################################
#
#     export the variables
#
export appdir appinst bindir dbdir gps gsv init_data_dir idir
export log ntcsscmd ntcssdir postdir progrps_file rmps sdi_dir
export settings sld_arg uc_prog ucdir user_dir webdir wps

#
#    end of define_variables.sh
#

