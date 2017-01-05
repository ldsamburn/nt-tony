#!/bin/sh

#-----------------------------------------------------------------------
#
# "assign_ssn.sh"
#
# Modify /h/NTCSSS/database/ntcss_users, giving each user
# a unique SSN that is NOT a part of NALCOMIS.
#
# Rules:
#
#     1.  Leave any "xxxbtusr"s alone.
#     2.  Leave any NALCOMIS user alone.
#     3.  Leave any user with a non-zero SSN alone.
#     4.  If any duplication occurs, increment the middle 2 digits of ssn.
#
#-------------------------------------------------------------------------
#

initialize()
{
    ntcssdir="/h/NTCSSS"
    postdir="$ntcssdir/applications/data/install/post_install"
    cvtdir="$ntcssdir/init_data/tools/s2p2opt"
    dbdir="$ntcssdir/database"
    log="$ntcssdir/post-install.log"
    init_data="$ntcssdir/init_data"
    bindir="$ntcssdir/bin"
    appdir="$ntcssdir/applications"
    installdir="$ntcssdir/applications/data/install"
    ucdir="$ntcssdir/applications/data/users_conf"
    hostname=`hostname`
    gps="$ntcssdir/bin/GetProfileString"
    wps="$ntcssdir/bin/WriteProfileString"
    date=`date`
    cwd=`pwd`
}

make_backups()
{
    # Make backup of current database
    echo " "
    echo "Creating backup of database to:"
    echo " -> $cwd/pre_assign_ssn_db.tar"
    echo "Creating backup of pre-assign_db database to:" >> $log
    echo " -> $cwd/pre_assign_ssn_db.tar" >> $log
    sleep 1
    cd $dbdir
    tar cvf $cwd/pre_assign_ssn_db.tar * 2>&1 > /dev/null
    echo " "
}

recover()
{
    echo " "
    echo "Do you want to recover? (Y/N)"
    echo " "
    read x
    if [ $x = "y" -o $x = "Y" -o $x = "yes" -o $x = "YES" ]
    then
        cwd=`pwd`
        # Restore system to the way it was before
        echo "Restoring system because of assignment failure."
        echo "Restoring system because of assignment failure." >> $log
        echo " "
        sleep 1
        cd $dbdir
        tar xvf $cwd/pre_assign_ssn_db.tar
        echo " "
        echo "Database Restored."
        echo "Database Restored." >> $log
        echo " "
        sleep 1
    fi
}

start_ntcss()
{
    echo "Starting NTCSS II" >> $log
    echo "Starting NTCSS II"
    echo " "
    sleep 1
    $bindir/ntcss_init -noboot
    #The gens must be started to setlinkdata
    echo "4 Second Delay to let NTCSS start....."
    sleep 2
    echo "2 Seconds left....."
    echo " "
    sleep 2
    genup=`ps -ef | grep gen_se | egrep -v grep`
    if [ $? = "0" ]
    then
        ntcssup="1"
    else
        ntcssup="0"
    fi
}

#---- Conversion starts here ----

    initialize
    echo " " >> $log
    echo "---- SSN assignment started on `date` ----" >> $log
    clear
    echo " ";echo " ";echo " ";echo " "
    echo " "
    echo "assign_ssn.sh"
    echo " "
    echo "This script assigns unique SSN's to each user that:"
    echo " "
    echo "    1. IS Not a member of NALCOMIS"
    echo "    2. IS Not a boot user"
    echo "    3. HAS a non-zero SSN"
    echo " "
    echo " ...Any duplicated SSNS cause an increment of the middle 2 digits.."
    echo " "
    echo "It first backs up the existing database to:"
    echo " "
    echo "    $cwd/pre_assign_ssn_db.tar"
    echo " "
    echo "-----> Press Enter to continue"
    read x
    clear
    echo " "

# Do Backups

    make_backups

# Bring NTCSS II down if necessary (probably not)

    isup=`ps -ef | grep gen_se | egrep -v grep`
    if [ $? = "0" ]
    then
        # gens are up for some reason
        echo " "
        echo "Insure that all users are logged off the system, because NTCSSII"
        echo "will be brought down.."
        echo " "
        echo " "
        echo "    Are all users off the system? (Y/N)"
        echo " "
        read x
        if [ $x = "Y" -o $x = "y" -o $x = "YES" -o $x = "yes" ]
        then
            echo "Bringing NTCSSII down.."
            echo " "
            cp /dev/null $dbdir/current_users
            $bindir/ntcss_halt
            isup=`ps -ef | grep gen_se | egrep -v grep`
            if [ $? = "0" ]
            then
                echo "NTCSSII did not come down for some reason.."
                echo "NTCSSII did not come down for some reason.." >> $log
                echo "Aborting mission.."
                recover
                exit 1
            fi
            echo " "
            sleep 2
        else
            echo "Aborting mission.."
            recover
            exit 1
        fi
    else
        echo "NTCSSII is down, thats good.."
        echo "NTCSSII is down, thats good.." >> $log
        echo " "
    fi
    cd $cwd

# Find current list of SSN's and sort them into database/current_ssns_sorted
# This is used in the do_ssns.awk to check if a duplicate exists.

    echo "Creating list of sorted SSN's currently being used.."
    echo "Creating list of sorted SSN's currently being used.." >> $log
    echo " "
    sleep 1
    awk -f $cwd/find_current_ssns.awk $dbdir/ntcss_users 

# Find current NALCOMIS users and create file (nalcomis_users_tmp)
# Put a colon prefix and suffix to each login ID, to eliminate grep errors.

    echo "Creating list of NALCOMIS users in nalcomis_users_tmp.."
    echo "Creating list of NALCOMIS users in nalcomis_users_tmp.." >> $log
    echo " "
    sleep 1
    awk -f $cwd/find_nalcomis_users.awk $dbdir/appusers
    echo "The following users have been detected as NALCOMIS users:"
    echo "The following users have been detected as NALCOMIS users:" >> $log
    echo " "
    sleep 1
    cat $dbdir/nalcomis_users_tmp
    cat $dbdir/nalcomis_users_tmp >> $log
    echo " "

# Fix ntcss_users file to give unique social security numbers

    echo "Converting $dbdir/ntcss_users file.."
    echo "Converting $dbdir/ntcss_users file.." >> $log
    echo " "
    sleep 1
    awk -f $cwd/do_ssns.awk $dbdir/ntcss_users
    if [ $? != "0" ]
    then
        echo "Error in applying SSN's to ntcss_users file, aborting.."
        echo "Error in applying SSN's to ntcss_users file, aborting.." >> $log
        echo " "
        sleep 3
        recover
        exit 1
    fi

# Copy over the new converted files

    echo "Copying over new ntcss_users file.."
    echo "Copying over new ntcss_users file.." >> $log
    echo " "
    sleep 1
    cp $dbdir/ntcss_users_tmp $dbdir/ntcss_users
    echo " "
    sleep 1

    #bring up NTCSSII
    start_ntcss
    if [ $ntcssup = "1" ]
    then
        echo "NTCSSII started" 
        echo "NTCSSII started"  >> $log
        echo " "
        sleep 1
    else
        echo "NTCSSII failed to start"
        echo "NTCSSII failed to start" >> $log
        echo "Aborting mission"
        echo "Aborting mission" >> $log
        echo " "
        sleep 3
        recover
        exit 1
    fi

    echo " "
    echo " "
    echo "Conversion complete, NTCSSII has been left up..."
    echo "Conversion complete, NTCSSII has been left up..." >> $log
    echo " "
    echo " "

    exit 0

