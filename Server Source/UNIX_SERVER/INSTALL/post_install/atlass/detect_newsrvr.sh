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
#
#    detect_newsrvr.sh
#
#
#      28 Dec 2000
#
#
#------------ This subroutine determines if any new servers exist in the
#             /etc/hosts file that do not exist in the progrps.ini file
#             to help speed up post_install process
#

    echo "detect_newsrvr.sh"

. ./define_variables.sh  detect_newsrvr.sh



search_progrps()
{
#
#   hostlist.ini file created from gethst.sh to reduce the
#   amount of searching in progrps.ini file for HOST names
#
#   echo "searching for $name in hostlist.ini which was derived from progrps.ini"    

    in_progrps=`$gps HOST_NAMES $name "NONE" $postdir/hostlist.ini`    
    if [ "$in_progrps" = "NONE" ]
       then
         in_adapters=`$gps ADAPTERS $ip "NONE" adapters_tmp.ini`
         if [ "$in_adapters" = "NONE" ]
            then
               echo "$name is not in progrps.ini...candidate to be added"
               $wps "HOSTS" "$name" "$ip" $postdir/newslaves.ini
         fi
    fi
}

build_adapters()
{
#
#    build an adapters_tmp.ini file to search if ip-addresses
#    are either an adapter or a hostname
#

rm -f adapt_list
rm -f adapters_tmp.ini
touch adapters_tmp.ini

ypcat ntcss_adapters.byip >> adapt_list
if [ "$?" != 0 ]
   then
     echo "ypcat ntcss_adapters.byip failed..."
     echo "Cannot continue....Correct and restart..."
     exit 1
fi

(
adapt_stat=`expr 0`
adapt_ans="n"
adapt_first="y"

while [ $adapt_stat -eq 0 ]
do

    read adapt_file
    adapt_stat=$?

    if [ $adapt_stat -ne 0 ]
       then
         break
    fi
    if [ "$adapt_first" = "y" ]
       then
         adapt_first=$adapt_file
       else
         if [ -z "$adapt_file" ]
            then
              break
         fi
         if [ "$adapt_file" = "$adapt_first" ]
            then
              break
         fi
     fi
     adapt_ip=`echo $adapt_file | cut -d\~ -f3`
     $wps ADAPTERS $adapt_ip $adapt_ip adapters_tmp.ini
done

) < adapt_list
}


do_exit()
{
    cp $postdir/progrps.tmp $ntcssdir/init_data/progrps.ini
    cp $postdir/newslaves.ini $postdir/slaves.ini
    rm -f adapt_list adapters_tmp.ini
    exit
}

#===============Script starts here========================
#clear
echo " "

rm -f $postdir/newslaves.ini
touch $postdir/newslaves.ini

#
#   go build a printer adapters ini file to be used by
#   the adapters_search routine
#

build_adapters

this_server=`hostname`
echo "This server is $this_server"

cp -p $ntcssdir/init_data/progrps.ini $postdir/progrps.tmp
if [ -f "$postdir/progrps.bak" ]
    then
        echo " "
        echo "**********ALERT***************"
        echo " "
        echo "progrps.bak already exists....no copy made"
        echo " "
        echo "This appears to be a restart. If so, you may want"
        echo "to copy the progrps.bak file back into the original"
        echo "progrps.ini file in init_data."
        ans1="n"
        echo  "Do you want to copy the backup progrps file into progrps.ini(y/n?): "
        read ans1
        if [ "$ans1" = "y" -o "$ans1" = "Y" ]
           then
              echo "Copying..."
               cp -p $postdir/progrps.bak $ntcssdir/init_data/progrps.ini
           else
              echo "No copy selected..."
        fi
      else
        cp -p $ntcssdir/init_data/progrps.ini $postdir/progrps.bak
fi
      

host1=`$gps HOST_1 NAME "NONE" $postdir/progrps.tmp`
if [ "$host1" = "$this_server" ]
   then
     echo "Executing on master server $this_server"
     mstr_srvr="y"
   else
     echo "NOT executing on master. Executing on $this_server"
     echo "Will not modify progrps.ini here. "
     exit 1 
fi

#
#   call gethst.sh to build hostlist.ini file
#

./gethst.sh  $postdir/progrps.tmp $postdir/hostlist.ini


#
# Obtain the highest HOST_NUMBER currently used in progrps.ini file
# since this process will increment that number by 1 for the new host(s)
#

high_num=`$gps HIGH_NUMBER NUMBER "NONE" $postdir/hostlist.ini`

if [ "$high_num" = "NONE" ] 
  then
    echo "Process not correct...$postdir/hostlist.ini cannot be processed"
    echo "The $postdir/hostlist.ini requires a [HIGH_NUMBER] entry at end"
    echo "Process terminating...correct and restart..."
    exit 1
fi  

high_num=`expr $high_num + 1`

(
ans="n"
first="y"

hostnum=$high_num  #This should be the next available HOST_NUMBER for new server
echo "The next HOST_num to be used will be $hostnum"

read_stat=`expr 0`

while [ $read_stat -eq 0 ] 
do
    # Get possible ATLASS server from list
    #
    # Reads slaves.ini file which contains list of servers
    # obtained from /etc/hosts file. These may be servers
    # or printer adapters. To be determined above in search_progrps
    #
#   echo "Reading server_name"
    read server_name
    read_stat=$?  

    if [ $read_stat -ne 0 ]
    then
    	echo "Detection completed"            
        do_exit
    fi

    if [ "$first" = "y" ]
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
        search_progrps
    fi
done

) < $postdir/slaves.ini


#
#    end of detect_newsrvr.sh
#
