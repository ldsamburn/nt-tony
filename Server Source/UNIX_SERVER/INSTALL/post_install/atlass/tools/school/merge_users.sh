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

#---------------------------------------------------------------------------
#
# 'merge_users.sh'
#
# Read old ntcss_users and appusers file, delete users that are ntcssii,
# sybase and spawar.  Merge old users with off-the-shelf users.
#
# ARG-1 = slave_server name
# ARG-2 = < 1 | 2 >, 1=single server, 2=master-slave configuration
#
#----------------------------------------------------------------------------

    echo "merge_users.sh"
    echo "--slave_server: $1"
    echo "--suffix: $2"
    echo " "

    log="/h/NTCSSS/post-install.log"
    echo "merge_users.sh" >> $log
    echo "--slave_server: $1" >> $log
    echo "--suffix: $2" >> $log
    ntcssdir="/h/NTCSSS"
    bindir="$ntcssdir/bin"
    dbdir="$ntcssdir/database"
    hostname=`hostname`
    uc_prog="$ntcssdir/applications/data/users_conf/user_config"
    cwd=`pwd`
    slave_server="$1"
    suffix="$2"

    rm -f $cwd/merged_ntcss_users_tmp
    rm -f $cwd/merged_appusers_tmp
    rm -f $cwd/NTCSS_users
    rm -f $cwd/$hostname"_users"
    rm -f $cwd/"DB_"$hostname"_users"
    rm -f $cwd/"IF_"$hostname"_users"
    rm -f $cwd/"CS_"$hostname"_users"
    rm -f $cwd/$slave_server"_users"
    rm -f $cwd/"DB_"$slave_server"_users"
    rm -f $cwd/"IF_"$slave_server"_users"
    rm -f $cwd/"CS_"$slave_server"_users"

    touch $cwd/NTCSS_users $cwd/$hostname"_users" $cwd/"DB_"$hostname"_users"
    touch $cwd/"IF_"$hostname"_users" $cwd/"CS_"$hostname"_users"
    touch $cwd/$slave_server"_users" $cwd/"DB_"$slave_server"_users"
    touch $cwd/"IF_"$slave_server"_users" $cwd/"CS_"$slave_server"_users"
    touch $cwd/merged_appusers_tmp
    touch $cwd/merged_ntcss_users_tmp

write_appusers_entry()
{
    x=`grep "$login_id" $application"_users"`
    if [ $? = "0" ]
    then
        echo " "
        echo "User $login_id already assigned to $application, skipping.."
    else
        echo "$application" >> $cwd/merged_appusers_tmp
        echo "$login_id" >> $cwd/merged_appusers_tmp
        echo "$real_name" >> $cwd/merged_appusers_tmp
        echo "$app_data" >> $cwd/merged_appusers_tmp
        echo "$app_passwd" >> $cwd/merged_appusers_tmp
        echo "$app_role" >> $cwd/merged_appusers_tmp
        echo "$registered_user" >> $cwd/merged_appusers_tmp
        echo "" >> $cwd/merged_appusers_tmp
        # Make a note so that users aren't assigned to an app more than once
        if [ "$application" = "NTCSS" ]; then
            echo "$login_id" >> $cwd/NTCSS_users
        fi
        if [ "$application" = "$hostname" ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = $application"_users" ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "DB_"$hostname ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "IF_"$hostname ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "CS_"$hostname ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "$slave_server" ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "DB_"$slave_server ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "IF_"$slave_server ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        if [ "$application" = "CS_"$slave_server ]; then
            echo "$login_id" >> $cwd/$application"_users"
        fi
        $bindir/dot
    fi
}

    # Remove ntcssii, sybase, atlbtusr and spawar from old_ntcss_users file
    # because they are in the ntcss_users.master file.

    echo "Purging user 'ntcssii'"             #---------------------------
    x=`grep "^ntcssii" $cwd/old_ntcss_users`
    if [ $? = "0" ]
    then
        ed - $cwd/old_ntcss_users <<EOF
/^ntcssii/
.-1,.+11 d
w
q
EOF
    else
        echo "ntcssii already removed from old_ntcss_users"
    fi

    echo "Purging user 'sybase'"              #---------------------------
    x=`grep "^sybase" $cwd/old_ntcss_users`
    if [ $? = "0" ]
    then
        ed - $cwd/old_ntcss_users <<EOF
/^sybase/
.-1,.+11 d
w
q
EOF
    else
        echo "sybase already removed from old_ntcss_users"
    fi

    echo "Purging user 'atlbtusr'"            #---------------------------
    x=`grep "^atlbtusr" $cwd/old_ntcss_users`
    if [ $? = "0" ]
    then
        ed - $cwd/old_ntcss_users <<EOF
/^atlbtusr/
.-1,.+11 d
w
q
EOF
    else
        echo "atlbtusr already removed from old_ntcss_users"
    fi

    echo "Purging user 'spawar'"              #---------------------------
    x=`grep "^spawar" $cwd/old_ntcss_users`
    if [ $? = "0" ]
    then
        ed - $cwd/old_ntcss_users <<EOF
/^spawar/
.-1,.+11 d
w
q
EOF
    else
        echo "spawar already removed from old_ntcss_users"
    fi

    echo "old_ntcss_users purged"

    ntcssii="1"                               #---------------------------
    sybase="1"
    spawar="1"
    atlbtusr="1"

    # Remove all occurances of ntcssii, sybase, atlbtusr and spawar
    # from the old_appusers file because they are in the appusers.master file.

    echo "Purging ntcssii, sybase, atlbtusr and spawar from old_appusers"

    while (true)
    do

        x=`grep "^ntcssii" $cwd/old_appusers` #.... ntcssii
        if [ $? = "0" ]
        then
            ed - $cwd/old_appusers <<EOF
/^ntcssii
.-1,.+6 d
w
q
EOF
            $bindir/dot
        else
            ntcssii="0"                       # ...All ntcssii deleted...
        fi

        x=`grep "^sybase" $cwd/old_appusers`  # ...sybase
        if [ $? = "0" ]
        then
            ed - $cwd/old_appusers <<EOF
/^sybase
.-1,.+6 d
w
q
EOF
            $bindir/dot
        else
            sybase="0"                       # ...All sybase deleted...
        fi

        x=`grep "^atlbtusr" $cwd/old_appusers` # ...atlbtusr
        if [ $? = "0" ]
        then
            ed - $cwd/old_appusers <<EOF
/^atlbtusr
.-1,.+6 d
w
q
EOF
            $bindir/dot
        else
            atlbtusr="0"                     # ...All atlbtusr deleted...
        fi

        x=`grep "^spawar" $cwd/old_appusers` # ...spawar
        if [ $? = "0" ]
        then
            ed - $cwd/old_appusers <<EOF
/^spawar
.-1,.+6 d
w
q
EOF
            $bindir/dot
        else
            spawar="0"                       # ...All spawar deleted...
        fi

        if [ "$ntcssii" = "0" -a "$sybase" = "0" -a "$spawar" = "0" -a "$atlbtusr" = "0" ]
        then
            echo "old_appusers purged"
            echo " "
            break
        fi

    done

    # There should be no one but normal users now in old_ntcss_users and
    # old_appusers.

    {
    while (true)
    do
        read "unix_id"                      # ---Copy over old ntcss_users---
        if [ "$?" -ne "0" ]                 #    to 'merged_ntcss_users_tmp'
        then                                #    and fix auth_server to the
            break                           #    slave machine.
        fi
        read "login_name"
        read "real_name"
        read "login_password"
        read "ssn"
        read "phone_number"
        read "security_class"
        read "pw_change_time"
        read "user_role"
        read "user_lock"
        read "shared_passwd"
        read "auth_server"
        read "blank"

        echo $unix_id >> $cwd/merged_ntcss_users_tmp
        echo $login_name >> $cwd/merged_ntcss_users_tmp
        echo $real_name >> $cwd/merged_ntcss_users_tmp
        echo $login_password >> $cwd/merged_ntcss_users_tmp
        echo $ssn >> $cwd/merged_ntcss_users_tmp
        echo $phone_number >> $cwd/merged_ntcss_users_tmp
        echo $security_class >> $cwd/merged_ntcss_users_tmp
        echo $pw_change_time >> $cwd/merged_ntcss_users_tmp
        echo $user_role >> $cwd/merged_ntcss_users_tmp
        echo $user_lock >> $cwd/merged_ntcss_users_tmp
        echo $shared_passwd >> $cwd/merged_ntcss_users_tmp
        echo $slave_server >> $cwd/merged_ntcss_users_tmp #...Fix Auth server...
        echo "" >> $cwd/merged_ntcss_users_tmp

        $bindir/dot

    done
    } < $cwd/old_ntcss_users

    echo " "
    echo " "
    echo "old_ntcss_users copied to merged_ntcss_users_tmp & auth_server fixed"

    # Do appusers                         #----------------------------

    {
    while (true)
    do
        read "application"
        if [ "$?" -ne "0" ]
        then
            break
        fi
        read "login_id"
        read "real_name"
        read "app_data"
        read "app_passwd"
        read "app_role"
        read "registered_user"
        read "blank"

        if [ $login_id = "spawar" -o $login_id = "sybase" ] #... Skip these
        then
            break
        fi

        # The application value in the old file will be "mml300", "mml900",
        # etc., for normal loads, they must be converted for this machine.

        i=`echo "$application" | grep "_"`
        if [ $? != "0" ]
        then                                 #..No "_" found in App name.
            if [ "$application" != "NTCSS" ] #..No "_" means ATLASS App.
            then                             # -or- NTCSS
                application=$hostname        # Assign Master app.
                write_appusers_entry
                if [ "$suffix" = "2" ]
                then
                    application=$slave_server    # Assign Slave app.
                    write_appusers_entry
                fi
            fi
        fi

        i=`echo "$application" | grep "DB_"`
        if [ $? = "0" ]
        then
             application="DB_"$hostname     # Assign Master DB_ app.
             write_appusers_entry
             if [ "$suffix" = "2" ]
             then
                 application="DB_"$slave_server # Assign Slave DB_ app.
                 write_appusers_entry
             fi
        fi

        i=`echo "$application" | grep "IF_"`
        if [ $? = "0" ]
        then
            application="IF_"$hostname      # Assign Master IF_ app.
            write_appusers_entry
            if [ "$suffix" = "2" ]
            then
                application="IF_"$slave_server  # Assign Slave IF_ app.
                write_appusers_entry
            fi
        fi

        i=`echo "$application" | grep "CS_"`
        if [ $? = "0" ]
        then
            application="CS_"$hostname      # Assign Master CS_ app.
            write_appusers_entry
            if [ "$suffix" = "2" ]
            then
                application="CS_"$slave_server  # Assign Slave CS_ app.
                write_appusers_entry
            fi
        fi

    done
    } < $cwd/old_appusers

    echo " "
    echo " "
    echo "old_appusers converted"

    cp $cwd/appusers"$suffix".master $cwd/appusers.boiler

    if [ $suffix = "1" ]
    then
        ed - $cwd/appusers.boiler <<EOF
1,\$s/SCHOOL01/$hostname/
w
q
EOF
    else
        ed - $cwd/appusers.boiler <<EOF
1,\$s/SCHOOL01/$hostname/
1,\$s/SCHOOL02/$slave_server/
w
q
EOF
    fi

   cat $cwd/ntcss_users.master $cwd/merged_ntcss_users_tmp > $dbdir/ntcss_users
   cat $cwd/appusers.boiler $cwd/merged_appusers_tmp > $dbdir/appusers

    exit 0

