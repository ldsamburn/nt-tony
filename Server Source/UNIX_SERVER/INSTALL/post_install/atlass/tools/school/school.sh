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

# school.sh

# Add users to ntcss_users and appusers using "userlist" as input file.
# Add same users to NT domain. Fix other files as necessary...
# Create output_type file, and output_prt file for master and slaves,
# but no users for slaves.

# Input file format:

#     ssn  |  last name  |  first name  | login id

    clear
    echo " ";echo " "
    echo "school.sh"
    echo " "

    log="/h/NTCSSS/post-install.log"
    ntcssdir="/h/NTCSSS"
    bindir="$ntcssdir/bin"
    dbdir="$ntcssdir/database"
    idir="$ntcssdir/init_data"
    hostname=`hostname`
    uc_prog="$ntcssdir/applications/data/users_conf/user_config"
    PATH="/h/NTCSSS/bin: $PATH"; export PATH
    master_server="n"

    echo "---- school.sh executed on `date` ----" >> $log

    cwd=`pwd`
    t=`date`
    t1=`echo $t | cut -d: -f2`
    t2=`echo $t | cut -d: -f3 | cut -d" " -f1`
    t3=$t1$t2

    if [ -n "$1" ]
    then
        if [ "$1" = "-recover" -o "$1" = "recover" ]
        then
            echo " "
            echo "Restoring NTCSSII to previous state..."
            echo "Restoring NTCSSII to previous state..." >> $log
            echo " "
            read tartime < $cwd/tartime
            tar xvf $cwd/db.$tartime
            tar xvf $cwd/group.$tartime
            tar xvf $cwd/pw.$tartime
            tar xvf $cwd/ini.$tartime
            touch $dbdir/progrps.ini # To avoid auto prime
            echo " "
            rm -f $dbdir/*.time
            if [ "$master_server" = "y" ]
            then
                /var/yp/ypmake
                echo "Pushing NTCSS NIS maps.."
                echo "Pushing NTCSS NIS maps.." >> $log
                $bindir/ntcss_make_and_push_nis_maps
                echo "NTCSS NIS maps pushed.."
                echo "NTCSS NIS maps pushed.." >> $log
                echo "NTCSSII database, passwd, group and ini files recovered.."
                echo "NTCSSII database, passwd, group and ini files recovered.." >>$log
                echo " "
            fi
            exit 0
        else
            echo "$t3" > $cwd/tartime
        fi
    fi

add_group()
{
    echo "Creating group: ($groupname)"
    echo "Creating group: ($groupname)" >> $log
    echo " "
    sleep 1
    gline=`grep "^$groupname" /etc/group`
    if [ -z "${gline}" ]
    then
        echo "Group $groupname was not found in /etc/group file"
        echo "Group $groupname was not found in /etc/group file" >> $log
        echo " "
        gid_hi=`sort -t: +2n -3 /etc/group | tail -1 | cut -d: -f3`
        gid_new=`expr ${gid_hi} + 1`
        echo "$groupname:*:$gid_new:" >> /etc/group
        echo "Group $groupname has been added to /etc/group file"
        echo "Group $groupname has been added to /etc/group file" >> $log
        echo " "
        sleep 1
    fi
}

add_user_everywhere()
{
    # Add user to Unix and build home directories

    $uc_prog ADDUSER "$login_id" "*" "/h/USERS" "$first_name $last_name" "$login_id" "$hostname"
    x=`grep "^$login_id" /etc/passwd`

    if [ $? = "1" ]
    then
        echo " "
        echo "User $login_id failed to get added to Unix.."
        echo "User $login_id failed to get added to Unix.." >> $log
        echo " "
        z=`ps -ef | grep "gen_se" | egrep -v "grep"`
        if [ $? = "1" ]
        then
            echo "Because NTCSSII is not running on $hostname!"
            echo "Because NTCSSII is not running on $hostname!" >> $log
            echo " "
            echo "Aborting Mission..."
            echo "Aborting Mission..." >> $log
            sleep 3
            echo "Press the Enter key to continue..."
            echo " "
            read y
            exit 1
        else
            echo "Unable to determine reason.."
            echo "Unable to determine reason.." >> $log
            echo " "
            echo "Aborting Mission..."
            echo "Aborting Mission..." >> $log
            echo " "
            echo "Press the Enter key to continue..."
            echo " "
            read y
            exit 1
        fi
    fi
}

    echo " "
    echo "Is this a MASTER SERVER?"
    echo " "
    read x
    if [ "$x" = "y" -o "$x" = "Y" -o "$x" = "yes" -o "$x" = "YES" ]
    then
        master_server="y"
        while (true)
        do
            echo " "
            echo "What is the name of the SLAVE SERVER? -or- 'NONE'"
            echo "if only a single system.."
            echo " "
            read slave_server
            echo " "
            case "$slave_server" in
                "NONE")
                    break
            ;;
                "none")
                    break
            ;;
                "")
                    echo "You must enter the name of the slave server -or- "
                    echo "'NONE'.."
            ;;
            esac
            real=`grep "$slave_server" /etc/hosts`
            if [ $? = "1" ]
            then
                echo "WARNING: That slave server does not exist in the /etc/hosts file..."
                echo " "
            fi
            echo "You entered: ( $slave_server ), is this correct?"
            echo " "
            read y
            if [ "$y" = "y" -o "$y" = "Y" -o "$y" = "yes" -o "$y" = "YES" ]
            then
                break
            fi
        done

        echo " "
        echo "The slave server is set to ( $slave_server )"
        echo "The slave server is set to ( $slave_server )" >> $log
        echo " "
        # Get this server's IP address
        if [ $slave_server = "NONE" -o $slave_server = "none" ]
        then
            slave_server=`hostname`
            suffix="1"
        else
            suffix="2"
        fi

        x=`grep "$slave_server" /etc/hosts 2> /dev/null`
        slave_ipaddr=`echo $x | cut -d " " -f1`
        if [ -n "$slave_ipaddr" ]
        then
            echo "The Slave server's IP address: $slave_ipaddr"
            echo "The Slave server's IP address: $slave_ipaddr" >> $log
        else
            echo "The Slave server's IP address could not be found in the"
            echo "/etc/hosts file......"
            echo " "
            echo "Enter the IP address of the slave server:"
            echo " "
            read $slave_ipaddr
        fi
    else
        echo " "
        echo "Enter the name of the Master Server:"
        echo " "
        read master_name
        echo " "
        echo "The master name is: ( $master_name )"
        echo "The master name is: ( $master_name )" >> $log
        echo " "
    fi

    if [ "$master_server" = "y" ]
    then
        if [ -n "$1" ]
        then
            infile="$cwd/$1"
            if [ ! -f "$infile" ]
            then
                echo " "
                echo "$infile does not exist,"
                echo "$infile does not exist," >> $log
                echo "unable to continue..."
                echo "unable to continue..." >> $log
                echo " "
                echo "NOTE: This is the input 'userlist' file."
                echo " "
                echo "Aborting Mission..." >> $log
                exit 1
            else
                infile="$1"
            fi
        else
            # Default
            infile="$cwd/userlist"
        fi
    fi

    read installed_ptrs < $dbdir/printers 
    if [ -n "$installed_ptrs" ]
    then
        continue
    else
        echo " "
        echo "It appears that NO printers have been built using NTCSSII."
        echo "At least ONE printer must exist to enable the creation of"
        echo "the 'output_type' and 'printer_access' tables..."
        echo " "
        echo "No installed printers on this system..." >> $log
        echo "Aborting Mission..."
        echo "Aborting Mission..." >> $log
        echo " "
        sleep 3
        exit 1
    fi

    # Get this server's IP address
    x=`grep $hostname /etc/hosts 2> /dev/null`
    ipaddr=`echo $x | cut -d " " -f1`

    echo "This server's IP address: $ipaddr"
    echo "This server's IP address: $ipaddr" >> $log
    echo " "

    echo "Backing up existing $dbdir to"
    echo "    $cwd/db.$t3"
    echo " "
    sleep 1
    tar cvf $cwd/db.$t3 $dbdir
    echo " "
    echo "$dbdir backed up.."
    echo " "
    sleep 1
    tar cvf $cwd/group.$t3 /etc/group
    tar cvf $cwd/pw.$t3 /etc/passwd
    tar cvf $cwd/ini.$t3 $ntcssdir/init_data/progrps.ini
    cp $dbdir/ntcss_users $cwd/old_ntcss_users   # current ntcss_users
    cp $dbdir/appusers $cwd/old_appusers         # current appusers
    echo " "
    echo "/etc/group, /etc/passwd, and"
    echo "/etc/group, /etc/passwd, and" >> $log
    echo "$ntcssdir/init_data/progrps.ini backed up.."
    echo "$ntcssdir/init_data/progrps.ini backed up.." >> $log
    echo " "
    sleep 1

    if [ "$master_server" = "y" ]
    then
        cp $cwd/appusers"$suffix".master $dbdir/appusers
        cp $cwd/APP_ID"$suffix".master $dbdir/APP_ID
        cp $cwd/access_roles"$suffix".master $dbdir/access_roles
        cp $cwd/current_apps"$suffix".master $dbdir/current_apps
        cp $cwd/predefined_jobs"$suffix".master $dbdir/predefined_jobs
        cp $cwd/programs"$suffix".master $dbdir/programs
        cp $cwd/progroups"$suffix".master $dbdir/progroups
        cp $cwd/progrps"$suffix".ini.master $dbdir/progrps.ini

        echo "ntcss_users, appusers, APP_ID, access_roles, current_apps," >> $log
        echo "predefined_jobs, programs, progroups and progrps.ini" >> $log
        echo "copied to database directory" >> $log

        if [ $suffix = "1" ]
        then
            ed - $dbdir/appusers <<EOF
1,\$s/SCHOOL01/$hostname/g
w
q
EOF
        else
            ed - $dbdir/appusers <<EOF
1,\$s/SCHOOL01/$hostname/g
1,\$s/SCHOOL02/$slave_server/g
w
q
EOF
        fi

        if [ $suffix = "1" ]
        then
            ed - $dbdir/APP_ID <<EOF
1,\$ s/SCHOOL01/$hostname/
w
q
EOF
        else
            ed - $dbdir/APP_ID <<EOF
1,\$ s/SCHOOL01/$hostname/
1,\$ s/SCHOOL02/$slave_server/
w
q
EOF
        fi

        if [ $suffix = "1" ]
        then
            ed - $dbdir/access_roles <<EOF
1,\$ s/SCHOOL01/$hostname/
w
q
EOF
        else
            ed - $dbdir/access_roles <<EOF
1,\$ s/SCHOOL01/$hostname/
1,\$ s/SCHOOL02/$slave_server/
w
q
EOF
        fi

        if [ $suffix = "1" ]
        then
            ed - $dbdir/current_apps <<EOF
1,\$ s/SCHOOL01/$hostname/
1,\$ s/hostname/$hostname/
w
q
EOF
        else
            ed - $dbdir/current_apps <<EOF
1,\$ s/SCHOOL01/$hostname/
1,\$ s/SCHOOL02/$slave_server/
1,\$ s/hostname/$hostname/
1,\$ s/slavename/$slave_server/
w
q
EOF
        fi

        if [ $suffix = "1" ]
        then
            ed - $dbdir/predefined_jobs <<EOF
1,\$ s/SCHOOL01/$hostname/g
w
q
EOF
        else
            ed - $dbdir/predefined_jobs <<EOF
1,\$ s/SCHOOL01/$hostname/g
1,\$ s/SCHOOL02/$slave_server/g
w
q
EOF
        fi

        if [ $suffix = "1" ]
        then
            ed - $dbdir/programs <<EOF
1,\$ s/SCHOOL01/$hostname/g
w
q
EOF
        else
            ed - $dbdir/programs <<EOF
1,\$ s/SCHOOL01/$hostname/g
1,\$ s/SCHOOL02/$slave_server/g
w
q
EOF
        fi

    # merge_users.sh will delete "ntcssii", "sybase", "atlbtusr" and "spawar"
    # from the old_ntcss_users, and old_appusers files, and then
    # concatanate the ntcss_users.master and appusers.master file with
    # the output of merge_users.sh (merged_ntcss_users_tmp and
    # merged_appusers_tmp files).

        $cwd/merge_users.sh $slave_server $suffix

        # Modify static files to new hostname, etc.

        # ntcss_users
        echo " "
        echo "Fixing hostname in ntcss_users file for of-the-shelf users.."
        ed - $dbdir/ntcss_users <<EOF
1,\$s/hostname/$slave_server/
1,\$s/mastername/$hostname/
w
q
EOF

        echo " "
        echo "ntcss_users boiler plate modified.."
        echo "ntcss_users boiler plate modified.." >> $log

        # progroups
        echo " "
        echo "Fixing hostname in progroups file.."

        if [ $suffix = "1" ]
        then
            ed - $dbdir/progroups <<EOF
1,\$s/hostname/$hostname/
1,\$s/SCHOOL01/$hostname/
w
q
EOF
        else
            ed - $dbdir/progroups <<EOF
1,\$s/hostname/$hostname/
1,\$s/slavename/$slave_server/
1,\$s/SCHOOL01/$hostname/
1,\$s/SCHOOL02/$slave_server/
w
q
EOF
        fi

        echo " "
        echo "progroups modified.."
        echo "progroups modified.." >> $log

        # progrps.ini
        echo " "
        echo "Fixing hostname in database/progrps.ini.."

        if [ $suffix = "1" ]
        then
            echo "...Single server mode.."
            sleep 2
            ed - $dbdir/progrps.ini <<EOF
1,\$s/hostname/$hostname/g
1,\$s/SCHOOL01/$hostname/g
1,\$s/master_server/$hostname/
1,\$s/IPADDR_master=/IPADDR=$ipaddr/
w
q
EOF
        else
            echo "...Dual server mode.."
            sleep 2
            ed - $dbdir/progrps.ini <<EOF
1,\$s/hostname/$hostname/g
1,\$s/SCHOOL01/$hostname/g
1,\$s/SCHOOL02/$slave_server/g
1,\$s/master_server/$hostname/
1,\$s/slave_server/$slave_server/
1,\$s/IPADDR_master=/IPADDR=$ipaddr/
1,\$s/IPADDR_slave=/IPADDR=$slave_ipaddr/
w
q
EOF
        fi

        #Unconditional edit for used for import file to NT
        #-------------------------------------------------
#        ed - $dbdir/progrps1.ini <<EOF
#1,\$s/hostname/$hostname/g
#1,\$s/SCHOOL01/$hostname/g
#1,\$s/master_server/$hostname/
#1,\$s/IPADDR_master=/IPADDR=$ipaddr/
#w
#q
#EOF

        echo " "
        echo "progrps.ini modified.."
        echo "progrps.ini modified.." >> $log
        echo " "

        cp $dbdir/progrps.ini $ntcssdir/init_data/progrps.ini
        touch $dbdir/progrps.ini # To stop the auto-prime that would occur
        echo "$ntcssdir/init_data/progrps.ini replaced"
        echo "$ntcssdir/init_data/progrps.ini replaced" >> $log
        echo " "

        if [ $suffix = "2" ]
        then
            #hosts
            echo "$slave_server" >> $dbdir/hosts
            echo "$slave_ipaddr" >> $dbdir/hosts
            echo "30" >> $dbdir/hosts
            echo "2" >> $dbdir/hosts
            echo "1" >> $dbdir/hosts
            echo "" >> $dbdir/hosts
            #HOST_ID
            echo "$slave_server" >> $dbdir/HOST_ID
        fi

        # Add "spawar" to system

        login_id="spawar"
        add_user_everywhere
        $uc_prog ADDGROUP spawar atlass
        $uc_prog ADDGROUP spawar ntcss_app
        $uc_prog ADDGROUP spawar sybase
        groupname="atlass_ms"
        add_group
        $uc_prog ADDGROUP spawar atlass_ms

        usercount=0
        linecount=`wc $infile | cut -d " " -f 1`

        # Go down the userlist and add each user to Unix, NTCSS and the NT Domain

        {
        while (true)
        do
            read user_rec
            if [ "$?" -ne "0" ]
            then
                break
            fi
            login_id=`echo $user_rec | cut -d "|" -f 4`
            # make check just in case this thing is run twice..
            in_ntcss_users=`grep $login_id $dbdir/ntcss_users`
            if [ $? = "0" ]
            then
                # Already in ntcss_users file
                echo "User: $login_id already in ntcss_users file - skipping"
                echo "User: $login_id already in ntcss_users file - skipping" >> $log
                continue
            fi
            ssn=`echo $user_rec | cut -d "|" -f 1`
            last_name=`echo $user_rec | cut -d "|" -f 2`
            first_name=`echo $user_rec | cut -d "|" -f 3`
            usercount=`expr $usercount + 1`
            echo " "
            echo "Processing: $usercount of $linecount -> $login_id"
            echo "Processing: $usercount of $linecount -> $login_id" >> $log
            echo "       ($first_name $last_name) `date`"
            echo "       ($first_name $last_name) `date`" >> $log

            # Add user to Unix and build home directories

            add_user_everywhere

            # Extract Unix ID

            unix_id=`echo $x | cut -d ":" -f 3`

            # Write to ntcss_users file

            adm=`echo "$login_id" | grep "class"`
            if [ $? = "0" ]
            then
                #Instructor detected
                echo "Instructor: $login_id detected ($login_id)"
                echo "Instructor: $login_id detected ($login_id)" >> $log
                adm="1"
                userrole="12"
                echo "---adm: $adm, userrole: $userrole"
                echo "---adm: $adm, userrole: $userrole" >> $log
            else
                #Student
                adm="0"
                userrole="8"
            fi

            echo "$unix_id" >> $dbdir/ntcss_users
            echo "$login_id" >> $dbdir/ntcss_users
            echo "$first_name $last_name" >> $dbdir/ntcss_users
            echo "$login_id" >> $dbdir/ntcss_users   # Really login password
            echo "$ssn" >> $dbdir/ntcss_users
            echo "555-1212" >> $dbdir/ntcss_users
            echo "0" >> $dbdir/ntcss_users           # Security class
            echo "990907.1143" >> $dbdir/ntcss_users # PW change time
            echo "$adm" >> $dbdir/ntcss_users        # NTCSS Admin=1
            echo "0" >> $dbdir/ntcss_users           # User locked flag
            echo "$login_id" >> $dbdir/ntcss_users   # Really DB password
            echo "$slave_server" >> $dbdir/ntcss_users   # Auth Server
            echo "" >> $dbdir/ntcss_users

            # Write to appusers file

            echo "$hostname" >> $dbdir/appusers      # Application name
            echo "$login_id" >> $dbdir/appusers      # login ID
            echo "$first_name $last_name" >> $dbdir/appusers # Real name 
            echo "" >> $dbdir/appusers               # App Data
            echo "$login_id" >> $dbdir/appusers      # Really App password
            echo "$userrole" >> $dbdir/appusers      # User role
            echo "0" >> $dbdir/appusers              # Registered flag
            echo "" >> $dbdir/appusers

            if [ $adm = "1" ]
            then
                #Make the instructor an NTCSS App Admin
                echo "Processing $login_id as NTCSS App Admin"
                echo "Processing $login_id as NTCSS App Admin" >> $log
                echo "NTCSS" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "$first_name $last_name" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "4" >> $dbdir/appusers
                echo "0" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                #
                echo "IF_$hostname" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "$first_name $last_name" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "12" >> $dbdir/appusers
                echo "0" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                #
                echo "DB_$hostname" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "$first_name $last_name" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "28" >> $dbdir/appusers
                echo "0" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                #
                echo "CS_$hostname" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "$first_name $last_name" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                echo "$login_id" >> $dbdir/appusers
                echo "12" >> $dbdir/appusers
                echo "0" >> $dbdir/appusers
                echo "" >> $dbdir/appusers
                #
                $uc_prog ADDGROUP $login_id sybase
                $uc_prog ADDGROUP $login_id ntcss_app
                $uc_prog ADDGROUP $login_id atlass_ms
                $uc_prog ADDGROUP $login_id atlass
            fi

            if [ $suffix = "2" ]
            then
                echo "$slave_server" >> $dbdir/appusers      # Application name
                echo "$login_id" >> $dbdir/appusers      # login ID
                echo "$first_name $last_name" >> $dbdir/appusers # Real name
                echo "" >> $dbdir/appusers               # App Data
                echo "$login_id" >> $dbdir/appusers      # Really App password
                echo "$userrole" >> $dbdir/appusers      # User role
                echo "0" >> $dbdir/appusers              # Registered flag
                echo "" >> $dbdir/appusers
                #
                if [ $adm = "1" ]
                then
                    echo "IF_$slave_server" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "$first_name $last_name" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "12" >> $dbdir/appusers
                    echo "0" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    #
                    echo "DB_$slave_server" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "$first_name $last_name" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "28" >> $dbdir/appusers
                    echo "0" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    #
                    echo "CS_$slave_server" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "$first_name $last_name" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    echo "$login_id" >> $dbdir/appusers
                    echo "12" >> $dbdir/appusers
                    echo "0" >> $dbdir/appusers
                    echo "" >> $dbdir/appusers
                    #
                fi
            fi

            # Add user to atlass group in /etc/group

            $uc_prog ADDGROUP $login_id atlass
       
        done

        } < $infile

    fi # End master server test

    if [ "$master_server" = "n" ]
    then
        echo "Transferring $master_name newdb.tar to $hostname"
        echo "Transferring $master_name newdb.tar to $hostname" >> $log
        echo " "
        sleep 2
        rm -f $dbdir/newdb.tar
        rm -f $idir/progrps.ini
        $bindir/net_xfer get $master_name $dbdir/newdb.tar $dbdir/newdb.tar
        if [ ! -f $dbdir/newdb.tar ]
        then
            echo "The database tar file did not xfer from $master_name..."
            echo "This must be done manually. Move the"
            echo "$dbdir/newdb.tar file to this machine and"
            echo "execute: tar xvf $dbdir/newdb.tar"
            echo " "
            exit 1
        else
            tar xvf $dbdir/newdb.tar
            cp $dbdir/progrps.ini $idir/progrps.ini
            chmod 755 $dbdir/progrps.ini
            touch $dbdir/progrps.ini
            echo "Database files updated from $master_name"
            echo "Database files updated from $master_name" >> $log
            echo " "
            sleep 2
        fi
    fi

# Set link ID
    $bindir/setlinkdata "school"

# Do output types and connect to a printer that has already been built
# prior to this script

if [ ! -f /skip_output_types ]
then

while (true)
do

    clear
    echo " "
    echo "The following procedure will build the 'output_type' file and"
    echo "the 'output_prt' file and set the relation between the output_type"
    echo "name and the associated physical printer. It will only do this"
    echo "for ONE printer.  If more are needed, use the NTCSS Application"
    echo "Printer utilities...."
    echo " "

    p=`ls /usr/spool/lp/interface`
    echo "The MASTER system currently has the following printers configured:"
    echo " "
    ls /usr/spool/lp/interface
    echo " "
    
    if [ -n "$p" ]
    then
        echo "Which printer would you like to use?"
        echo " "
        read ptr
        echo " "
        if [ -n "$ptr" ]
        then
            echo " "
            echo "You have selected: ( $ptr )"
            echo " "
            echo "Is this correct?"
            echo " "
            read x
            if [ "$x" = "Y" -o "$x" = "y" -o "$x" = "YES" -o $x = "yes" ]
            then
                echo "Creating output_types for printer $ptr" >> $log
                rm -f $cwd/output_type.out
                rm -f $cwd/output_prt.out
                $cwd/create_output_type.sh $hostname $ptr $slave_name $suffix
                if [ $? != "0" ]
                then
                    echo "Error creating output type file" >> $log
                    echo "There was an error creating the output types file or"
                    echo "the output_prt file. The output_type file and"
                    echo "output_prt files are currently located in"
                    echo "$cwd."
                    echo "They are named 'output_type.out' and 'output_prt.out."
                    echo " "
                    echo "The output type builder can be executed in the"
                    echo "future by executing the following commands:"
                    echo " "
                    echo "cd /h/NTCSSS/init_data/tools"
                    echo " "
                    echo "./create_output_type.sh $hostname 'printer name' $slave_name $suffix" 
                    echo " "
                    echo "The new output_type file must then be copied to"
                    echo "/h/NTCSSS/database/output_type"
                    echo " "
                    echo " "
                else # Good return from output_type/output_prt
                    echo " "
                    cp $cwd/output_type.out $dbdir/output_type
                    cp $cwd/output_prt.out $dbdir/output_prt
                    echo "output_type and output_prt files built.."
                    echo "output_type and output_prt files built.." >> $log
                    echo " "
                    break
                fi
            else
                continue
            fi
        fi # end of which printer question?
    fi # end of empty interface directory check

done # finally got it right             

fi # End of check for '/skip_output_types'

# Create encrypted passwords

if [ "$master_server" = "y" ]
then
    cd pw
    cp $dbdir/ntcss_users .
    rm -f $cwd/pwdfix.ini
    echo "Extracting User Information" 
    echo "Extracting User Information" >> $log
    $bindir/fixpwd > /dev/null
    echo "Done Extracting User Information" 
    echo "Done Extracting User Information" >> $log
    echo " "
    echo " "
    echo "Creating plain/encrypted passwords"
    echo "Creating plain/encrypted passwords" >> $log
    echo " "
    echo "NOTE: Login passwords will be the same as the User ID.."
    echo "      and the 'auth_server' will be $slave_server"
    echo "      except for 'ntcssii' that will be $hostname"
    echo " "
    $cwd/pw/pwinifix.sh $slave_server $hostname
    echo " "
    cp $cwd/pw/newini $cwd/pw/pwdfix.ini
    
    $bindir/newpwd $cwd/pw/pwdfix.ini

    tar cvf $dbdir/newdb.tar \
            $dbdir/APP_ID $dbdir/SYS_CONF $dbdir/access_roles \
            $dbdir/appusers $dbdir/current_apps $dbdir/hosts \
            $dbdir/ntcss_users $dbdir/output_prt $dbdir/output_type \
            $dbdir/predefined_jobs $dbdir/progroups $dbdir/programs \
            $dbdir/progrps.ini $idir/progrps.ini

    rm -f $dbdir/*.time
    echo "Doing ypmake" >> $log
    /var/yp/ypmake

    echo "Pushing NTCSS NIS maps.."
    echo "Pushing NTCSS NIS maps.." >> $log
    $bindir/ntcss_make_and_push_nis_maps
    echo "NTCSS NIS maps pushed.."
    echo "NTCSS NIS maps pushed.." >> $log

    $bindir/unix_ini create USRADMIN $cwd/pw/import_temp
    cat $dbdir/progrps.ini $cwd/pw/import_temp > /nt_import.ini

    echo " "
    echo "Remember that the original $dbdir"
    echo "is saved as $cwd/db.$t3"
    echo "The passwd, group and ini files are also saved.."
    echo " "
    echo "If you need to replace them...."
    echo " "
    echo "They may be recovered by:"
    echo " "
    echo "    cd $cwd"
    echo "    ./school.sh -recover"
    echo " "
    echo "Press the 'Enter' key to continue..."
    echo " "
    read x
    echo " "
fi
    echo " "
    if [ $master_server = "y" ]
    then
        clear
        cat $cwd/ftp_instructions
    else
        echo "(Slave) Server part of the process is complete..."
    fi

    cd $cwd

exit 0

