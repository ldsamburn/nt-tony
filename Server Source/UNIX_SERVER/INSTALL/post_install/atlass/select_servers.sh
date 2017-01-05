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

# select_servers.sh
#===========================================================================

#------------ This subroutine changes group icon names of previously -------
#------------ auto-configured ATLASS machines ------------------------------


. ./define_variables.sh

do_exit()
{
#    clear
#    echo " "

   cp $postdir/progrps.tmp $ntcssdir/init_data/progrps.ini
   exit
}

#===============Script starts here========================
#clear
echo " "
echo "    select_servers.sh executing..."
echo " "

run_mode="new_install"

if [ "$1" = "adding_new_servers" ]
then
   run_mode="$1"
fi
echo " "
echo "Run mode = $run_mode" >> $log
echo "Run mode = $run_mode"
echo " "
rm -f $postdir/done_file $postdir/progrps.tmp $postdir/slaves.tmp
rm -f $postdir/progrps.tmp1 $postdir/slaves1.tmp
rm -f $postdir/hosts.tmp $postdir/occurance.tmp
rm -f $postdir/slaves1.tmp

if [ "$run_mode" = "adding_new_servers" ]
   then
      cp -p $ntcssdir/init_data/progrps.ini $postdir/progrps.tmp
   else
      cp $postdir/progrps_atlass.ini $postdir/progrps.tmp
fi

cp $postdir/slaves.ini $postdir/slaves.bak

(
ans="n"
first="y"

# hostnum=`expr 2`  # 1st hostnum is master.. created by sethost.awk

#######################################################
#
#   The below code added to begin using the
#   new AddNewAtlassSrvr.sh script which will
#   allow adding any new servers to atlass and only
#   ask necessary questions about the new server being
#   added. Eliminates questions on all the servers in the
#   atlass configuration that have already existed.
#
#   The below code only affects the starting HOST_number.
#
#   If the new script is not used, the starting HOST_number
#   will begin at 2. If the new script is used, then the
#   starting HOST_number will begin at last-number + 1.
#
#   Therefore, this change will be ok if new shell is or is not
#   utilized.
#

hostnum=`$gps HIGH_NUMBER NUMBER NONE $postdir/hostlist.ini`
if [ "$hostnum" = "NONE" ]
  then
    #first hostnum =1 is set by sethost.awk
     hostnum=`expr 2`
  else
     hostnum=`expr $hostnum + 1`
fi
echo "Next HOST_number will be $hostnum"


read_stat=`expr 0`

while [ $read_stat -eq 0 ] 
do
    # Get possible ATLASS server from list
    read server_name
    read_stat=$?  

    if [ $read_stat -ne 0 ]
    then
    	echo "Host file processing complete"
        do_exit
    fi

    if [ $first = "y" ]
    then
        first=$server_name
    else
        if [ -z "$server_name" ]
        then
            do_exit
        fi
        if [ "$server_name" = "$first" ]
        then
            do_exit
        fi
    fi
    if [ "${server_name}" != "[HOSTS]" ]
    then
        name=`echo ${server_name} | cut -d= -f1`
        ip=`echo ${server_name} | cut -d= -f2`
        rm -f /tmp/_hostfound
        echo "Processing server - $name"
        echo " "; echo " "
        echo "Is $name an ATLASS server? [y,n, RETURN=finished with server selections..]"
        echo " "
        ( ans=""
        read ans
        if [ -z "${ans}" ]
        then
            echo "DONE" > $postdir/done_file 
        fi
        if [ "${ans}" = "y" -o "${ans}" = "Y" ]
        then
            if [ "$name" != `hostname` ]
            then
                echo "hostfound" > /tmp/_hostfound
                $wps HOSTS HOST_$hostnum HOST_$hostnum $postdir/progrps.tmp
                $wps HOST_$hostnum "NAME" $name $postdir/progrps.tmp
                $wps HOST_$hostnum "IPADDR" $ip $postdir/progrps.tmp
                echo " "
                echo "Is this server an 'Authentication Server' (Y/N) ?"
                echo " "
                read auth
                if [ "${auth}" = "Y" -o "${auth}" = "y" ]
                then
                    $wps HOST_$hostnum "TYPE" "Authentication Server" $postdir/progrps.tmp
                else
                    $wps HOST_$hostnum "TYPE" "Application Server" $postdir/progrps.tmp
                fi
                echo " "
                echo "Do you want this server to be capable of becoming a 'MASTER' Server (Y/N) ?"
                echo " "
                read repl
                if [ "${repl}" = "Y" -o "${repl}" = "y" ]
                then
                    $wps HOST_$hostnum "REPL" "YES" $postdir/progrps.tmp
                else
                    $wps HOST_$hostnum "REPL" "NO" $postdir/progrps.tmp
                fi
            fi
            newname=$name
            $wps APPS $newname $newname $postdir/progrps.tmp
            newname="IF_$name"
            $wps APPS $newname $newname $postdir/progrps.tmp
            newname="DB_$name"
            $wps APPS $newname $newname $postdir/progrps.tmp
            cp $postdir/atlass_occurance $postdir/occurance.tmp
            dbname=`echo $name | $postdir/upper`
            dbbackup=`echo $name"_BACKUP" | $postdir/upper`
            pword=$name"sa1"
            ed - $postdir/occurance.tmp 2>&1 <<EOF
1,\$s/ATLASS_X/$name/g
1,\$s/DB_X/DB_$name/g
1,\$s/IF_X/IF_$name/g
1,\$s/hostname/$name/
w
q
EOF
            cat $postdir/progrps.tmp $postdir/occurance.tmp > $postdir/progrps.tmp1
            cp $postdir/progrps.tmp1 $postdir/progrps.tmp
        fi ) < /dev/tty
        if [ -f /tmp/_hostfound ]
        then
            hostnum=`expr $hostnum + 1`
        fi
        if [ -f "$postdir/done_file" ]
        then
            # Change made for non-interactiveicon naming
            # get_icon_names
            do_exit
        fi
    fi
done

rm -f $postdir/done_file
) < $postdir/slaves.ini 

mv $postdir/slaves.bak $postdir/slaves.ini

