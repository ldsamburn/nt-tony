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

#-------------------------------------------------------------------------
#                                                                        !
# setup.sh                                                               !
#                                                                        !
# Called by /usr/local/NTCSSII/bin/ntcss_init                            !
#                                                                        !
# Detect and configure portions of progrps.ini file to enable            !
# currently deployed or future DII deployed contigurations by            !
# changing path names according to a "SITE_FILE".                       !
#                                                                        !
# Designed to work with existing progrps.ini file found at installation  !
# at the time of the patch implementing this script.                     !
#                                                                        !
# ARG-1: If non-zero, assume debug mode                                  !
#                                                                        !
#-------------------------------------------------------------------------

    # Just return if on slave processer

    if [ `hostname` != `ypwhich -m passwd.byname` ]
    then
        echo "setup.sh not executing on master server, exiting.."
        exit 0
    fi

    if [ -n "$1" ]
    then
        if [ "$1" = "debug" ]
        then
            debug="1"
        else
            debug="0"
        fi
    fi

    ntcssdir="/usr/local/NTCSSII"
    init_dir="/usr/local/NTCSSII/init_data"
    bindir="$ntcssdir/bin"
    init_data="$ntcssdir/init_data"
    ini_file_tmp="$init_data/progrps_temp.ini"
    ini_file_org="$init_data/progrps.ini"
    timefile="$init_data/progrps.time"
    database="$ntcssdir/database"
    lastgood="$init_data/progrps.lastgood"
    gps="$bindir/GetProfileString"
    wps="$bindir/WriteProfileString"
    hostname=`hostname`
    log="$ntcssdir/post-install.log"
    cwd=`pwd`

    echo " "
    echo "Starting NTCSSII configuration..."
    echo " "
    sleep 2

    isup=`ps -ef | grep "NTCSSII/bin/gen_se command_server" | egrep -v grep`
    if [ $? = "0" ]
    then
        echo " ";echo " "
        echo "NTCSSII appears to be up. Insure that all users are off"
        echo "the system and bring it down by executing"
        echo "$bindir/ntcss_halt, and then the script:"
        echo " ";echo "$cwd/setup.sh"
        echo " ";echo " "
        cd "$cwd"
        exit 1
    fi

    nalc_dii_chk_file=nalc_dii_chk_file
    omms_dii_chk_file=omms_dii_chk_file
    radm_dii_chk_file=radm_dii_chk_file

    #Make one good copy of orginal progrps.ini file
    if [ ! -f $lastgood ]
    then
        cp -p $ini_file_org $lastgood
    fi

    cp -p $ini_file_org $ini_file_tmp 

    yes="0"
    no="1"
    nalcomis=$no
    rsupply=$no
    ommsng=$no
    radm=$no

. $init_dir/shlib

get_app_parms()
{
    if [ "$debug" = "1" ]; then
    echo " "; echo "--->get_app_parms: app=($app)"; fi
    clientpath=`$gps $app PATH Fail SITE_FILE`
    clientloc=`$gps $app CLIENT_LOCATION Fail SITE_FILE`
    icon=`$gps $app ICON Fail SITE_FILE`
    envfile=`$gps $app ENV_FILE_SERVER_PATH Fail SITE_FILE`
    pspfile=`$gps $app PSP_FILE_SERVER_PATH Fail SITE_FILE`
}

print_parms()
{
    if [ "$debug" = "1" ]; then
        rm -f temp.ini
        echo " "
        echo "--->Results of SITE_FILE contents for application ($app)"
        echo "[RESULTS]" > temp.ini
        echo "ICON=ICON" >> temp.ini
        echo "PATH=PATH" >> temp.ini
        echo "CLIENT_LOCATION=CLIENT_LOCATION" >> temp.ini
        echo "ENV=ENV" >> temp.ini
        echo "PSP=PSP" >> temp.ini
        $wps RESULTS ICON $icon temp.ini
        $wps RESULTS PATH $clientpath temp.ini
        $wps RESULTS CLIENT_LOCATION $clientloc temp.ini
        $wps RESULTS ENV $envfile temp.ini
        $wps RESULTS PSP $pspfile temp.ini
        ed - temp.ini <<EOF
1,\$s/^/    /
w
q
EOF
        cat temp.ini
    fi
}

do_pdj()
{
    #app is the section name in the SITE_FILE
    if [ "$debug" = "1" ]; then
    echo "      GPS STRING: (PDJ_"$pdj_num"_"$site_type" NONE SITE_FILE)"; fi
    pdj=`$gps $app PDJ_"$pdj_num"_"$site_type" NONE SITE_FILE`
    if [ "$pdj" = "NONE" ]
    then
        if [ "$debug" = "1" ]; then
        echo "    No PDJ for $app - PDJ_ $pdj_num $site_type"; fi
        return 2
    else
        if [ "$debug" = "1" ]; then
        echo "      Executing:"
        echo "        $wps"
        echo "        $application"_PDJ_$pdj_num" COMMAND_LINE="$pdj""
        echo "        $ini_file_tmp"; fi
        $wps $application"_PDJ_"$pdj_num "COMMAND_LINE" $pdj $ini_file_tmp
        if [ $? != "0" ]
        then
            echo "    Problems writing $application"_PDJ_$pdj_num" to $ini_file_tmp"
        fi
    fi
}

update_ini_file()
{
    echo "--->Processing Application group $application"
    echo "--->Processing Application group $application" >> $log
    # Server path names, PDJ's, etc.
    if [ "$debug" = "1" ]; then
    echo "    Modifying Server path names in PDJ entries"; fi
    pdj_num=`expr 0`
    while [ $pdj_num -lt 16 ]
    do
        pdj_num=`expr $pdj_num + 1`
        if [ "$debug" = "1" ]; then
        echo "    Calling do_pdj for PDJ: ($pdj_num)"; fi
        do_pdj
        if [ $? = "2" ]
        then
            if [ "$debug" = "1" ]; then
            echo "    PDJ processing complete"; fi
            break
        fi
    done
    # Application group icon file path
    if [ "$debug" = "1" ]; then
    echo "    Writing ICON_FILE to progrps.ini file for ($application)"; fi
    $wps $application ICON_FILE $icon $ini_file_tmp
    if [ $? != "0" ]
    then
        echo "    Unable to modify the ICON_FILE variable in Section $application"
        echo "    Unable to modify the ICON_FILE variable in Section" >> $log
        echo "  $application" >> $log
        cd "$cwd"
        exit 1
    fi
    # Application program icon file path
    if [ "$debug" = "1" ]; then
    echo "    Writing PROG_1 to progrps.ini file for ($application)"; fi
    $wps $application"_PROG_1" ICONFILE $icon $ini_file_tmp
    if [ $? != "0" ]
    then
        echo "    Unable to modify the ICONFILE variable in Section"
        echo "      $application_PROG_1"
        echo "    Unable to modify the ICONFILE variable in Section" >> $log
        echo "      $application_PROG_1" >> $log
        cd "$cwd"
        exit 1
    fi
    # Application env file path
    if [ "$debug" = "1" ]; then
    echo "    Writing APP_ENV to progrps.ini file for ($application)"; fi
    $wps $application APP_ENV_FILE $envfile $ini_file_tmp
    if [ $? != "0" ]
    then
        echo "    Unable to modify the APP_ENV_FILE variable in Section"
        echo "      $application"
        echo "    Unable to modify the APP_ENV_FILE variable in Section" >> $log
        echo "      $application" >> $log
        cd "$cwd"
        exit 1
    fi
    # Application client_location file path
#    if [ "$debug" = "1" ]; then
#    echo "    Writing CLIENT_LOCATION to progrps.ini file for ($application)"; fi
#    $wps $application CLIENT_LOCATION $clientloc $ini_file_tmp
#    if [ $? != "0" ]
#    then
#        echo "    Unable to modify the CLIENT_LOCATION variable in section" 
#        echo "      $application, this is non-fatal"
#        echo "    Unable to modify the CLIENT_LOCATION variable in section" >> $log
#        echo "      $application, this is non-fatal" >> $log
#        # This entry in the progrps.ini file is not currently used
#        # cd "$cwd"
#        # exit 1
#    fi
    # Application psp file path
    if [ "$debug" = "1" ]; then
    echo "    Writing PSP to progrps.ini file for ($application)"; fi
    $wps $application PROCESS_STATUS_FLAG $pspfile $ini_file_tmp
    if [ $? != "0" ]
    then
        echo "    Unable to modify the PROCESS_STATUS_FLAG variable in Section"
        echo "      $application"
        echo "    Unable to modify the PROCESS_STATUS_FLAG variable in Section" \
             >> $log
        echo "      $application" >> $log
        cd "$cwd"
        exit 1
    fi
    # Application working directory path
    if [ "$debug" = "1" ]; then
    echo "    Writing WORKINGDIR to progrps.ini file for ($application)"; fi
    $wps $application"_PROG_1" WORKINGDIR $clientpath $ini_file_tmp
    if [ $? != "0" ]
    then
        echo "    Unable to modify the WORKINGDIR variable in Section"
        echo "      $application_PROG_1"
        echo "    Unable to modify the WORKINGDIR variable in Section" >> $log
        echo "      $application_PROG_1" >> $log
        cd "$cwd"
        exit 1
    fi
}

do_app()
{
    if [ $app_id = "$yes" ]
    then
        if [ "$debug" = "1" ]; then
        echo " ";echo "-->Starting ($application)"
        echo "   >do_app: application=($application), app_id=($app_id)"
        echo "   $app"; fi
        # Get clientpath, icon, envfile, pspfile, and scripts variables
        get_app_parms
        print_parms #debug
        update_ini_file
    fi
}

display_dii_status()
{
    echo " "
    echo "--->$application ($dii) release detected.."
}

fetch_dii_info()
{
    this_is_dii=$no
    dii_file=`$gps RELEASE $application Fail SITE_FILE`
    if [ "$dii_file" != "Fail" ]
    then
        #We know what it should be, now fetch the dii unique file
        if [ "$debug" = "1" ]; then
        echo " ";echo "   Chking $dii_file for $application on system $box"; fi
        rm -f "$box"_dii_chk_file
        if [ "$debug" = "1" ]; then
        echo "   NET_XFER: BOX($box) DII_FILE($dii_file)"
        echo "   CHK_FILE("$box"_dii_chk_file)"; fi
        ns=`$bindir/net_xfer get $box $dii_file "$box"_dii_chk_file`
        if [ $? != "254" ]
        then
            if [ -f "$box"_dii_chk_file ]
            then
                #This is a DII release
                dii="_DII"
                this_is_dii=$yes
                display_dii_status
            else
                #This is a NON-DII release
                dii="_NON_DII"
                this_is_dii=$no
                display_dii_status
            fi
        else
            # Bad read
            echo " ";echo "--->NOTE: Unable to read from the $box system"
            echo "Unable to read from the $box"  system>> $log
            echo "--->Continuing with standard $application release"
            echo "Continuing with standard $application release" >> $log
            dii="_NON_DII"
            this_is_dii=$no
            set_dii_status
            fi
    else #Bad GetProfileString read on SITE_FILE
        echo "   SITE_FILE does not have the $application unique DII file defined"
        echo "   Continuing with standard $application release"
        dii="_NON_DII"
        this_is_dii=$no
        set_dii_status
    fi
}

do_nalcomis()
{
    app_id=$nalcomis_site
    application="NALCOMIS"
    app="NALCOMIS$dii"
    do_app
    app_id=$nalcomis_adm_site
    application="NALCOMIS_Adm"
    app="NALCOMIS_Adm$dii"
    do_app
}

do_rsupply()
{
    app_id=$rsupply_site
    application="RSUPPLY"
    app="RSUPPLY$dii"
    do_app
    app_id=$rsupply_adm_site
    application="RSUPPLY_Adm"
    app="RSUPPLY_Adm$dii"
    do_app
}

do_ommsng()
{
    app_id=$ommsng_site
    application="OMMSNG"
    app="OMMSNG$dii"
    do_app
    app_id=$ommsng_adm_site
    application="OMMSNG_Adm"
    app="OMMSNG_Adm$dii"
    do_app
}

do_radm()
{
    app_id=$radm_site
    application="RADM"
    app="RADM$dii"
    do_app
    app_id=$radm_adm_site
    application="RADM_Adm"
    app="RADM_Adm$dii"
    do_app
}

initlocal()
{
    $wps LOCALAPPS NALCOMIS NO SITE_FILE
    $wps LOCALAPPS NALCOMIS_Adm NO SITE_FILE
    $wps LOCALAPPS RSUPPLY NO SITE_FILE
    $wps LOCALAPPS RSUPPLY_Adm NO SITE_FILE
    $wps LOCALAPPS RADM NO SITE_FILE
    $wps LOCALAPPS RADM_Adm NO SITE_FILE
    $wps LOCALAPPS OMMSNG NO SITE_FILE
    $wps LOCALAPPS OMMSNG_Adm NO SITE_FILE
}

#------------------------------------------------------------------------

# Start here

    #initialize LOCALAPPS section

    initlocal

    # Set variables to "yes" (0) or "no" (1) depending on their
    #  presence in the progrps.ini file.

    echo " "

    # Analyze site:
    #
    #    returns:
    #
    #        rsupply_site       (0/$yes=in progrps.ini file, 1/$no=not)
    #        rsupply_adm_site   (0/$yes=in progrps.ini file, 1/$no=not)
    #        ommsng_site        (0/$yes=in progrps.ini file, 1/$no=not)
    #        ommsng_adm_site    (0/$yes=in progrps.ini file, 1/$no=not)
    #        radm_site          (0/$yes=in progrps.ini file, 1/$no=not)
    #        radm_adm_site      (0/$yes=in progrps.ini file, 1/$no=not)
    #        nalcomis_site      (0/$yes=in progrps.ini file, 1/$no=not)
    #        nalcomis_adm_site  (0/$yes=in progrps.ini file, 1/$no=not)
    #
    #        site_type: (mals, smalldeck, dual_air, single_air, largedeck)
    #
    #     (If the file "/p8_test_mode" is present, these values are
    #      returned with the current hostname.)
    #
    #        rsupply_box   =   rsupmast, or current test hostname
    #        omms_box      =   omms, or current test hostname
    #        radm_box      =   radm, or current test hostname
    #        nalcomis_box  =   nalc, or current test hostname

    analyze_site

    if [ $site_type = "largedeck" -o $site_type = "smalldeck" \
         -o $site_type = "mals" ]
    then
        application="RSUPPLY"
        box=$rsupply_box
        fetch_dii_info
        do_rsupply
    fi

    if [ $site_type = "largedeck" -o $site_type = "smalldeck" ]
    then
        application="OMMSNG"
        box=$omms_box
        fetch_dii_info
        do_ommsng
    fi

    if [ $site_type = "largedeck" -o $site_type = "smalldeck" ]
    then
        application="RADM"
        box=$radm_box
        fetch_dii_info
        do_radm
    fi

    # Check nalc box for dii-coe release
    #    Large decks, NALCOMIS is on nalc
    #    Mals, RSUPPLY is on rsupmast and NALCOMIS is on nalc
    #    Air Stations with 1 cpu, Code and db is on nalcmast 
    #    Air Stations with 2 cpu's, Code is on nalcmast, db is on nalc 

    if [ $site_type = "largedeck" -o $site_type = "mals" -o \
         $site_type = "single_air" -o $site_type = "dual_air" ]
    then
        application="NALCOMIS"
        box=$nalcomis_box
        fetch_dii_info
        do_nalcomis
    fi

    #Check if we need to prime_db
    if [ "$debug" = "1" ]; then
    echo "--->Checking org and tmp progrps.ini files"; fi
    pr=`diff $ini_file_tmp $ini_file_org`
    diffstat=$?
    if [ $diffstat = "1" ]
    then
        if [ "$debug" = "1" ]; then
        echo "    prime_db required"; fi
        cp $ini_file_tmp $ini_file_org
        $bindir/prime_db
        if [ $? = "0" ]
        then
            echo "    prime_db OK"
            echo "    prime_db OK" >> $log
            cp -p $ini_file_org $lastgood
        else
            echo "    prime_db BAD"    
            echo "    prime_db BAD" >> $log
            echo " "
            echo "--->Contents of prime_db log file:"
            echo " "
            cat $ntcssdir/logs/prime_db.log
            cp -p $lastgood $ini_file_org
            echo " "
            echo "Previous progrps.ini file restored"
            $bindir/prime_db
            if [ $? = "0" ]
            then
                echo "    prime_db OK"
                echo "    prime_db OK" >> $log
                echo " "
            fi
        fi
    else
        echo " "
        echo "--->No changes were made to the progrps.ini file"
        echo " "
    fi

    cd "$cwd"
    exit 0


#
#   end of setup.sh
#
