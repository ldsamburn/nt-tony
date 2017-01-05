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
#
#    sethost.sh
#
#
# This script uses the current host name and IP address from the
# "ypcat" of the host file and plugs the values into the progrps.ini file.

cat /etc/hosts | sed -e "s/	/~/g" | sed -e "s/ /~/g" |
              sed -e "s/$/~/g" > /tmp/standard_hosts

ntcssdir=/h/NTCSSS
progrps_file=$ntcssdir/init_data/progrps.ini
ntcss_host_file=$ntcssdir/database/hosts
hostname=`hostname`
rm -f $ntcss_host_file
touch $ntcss_host_file

    ed - $progrps_file <<EOF                         # Do master hostname first
1,\$s/hostname/$hostname/g
w
q
EOF

process_server()                                     # Now do each ip address
{
    # Process the server only if it's in the progrps.ini file
    dosvr=`grep "SERVER=$svr" $progrps_file`         # progrps.ini file
    if [ $? = "0" ]
    then
        # This server found in progrps.ini file
        hostline=`grep "~$svr~" /tmp/standard_hosts`
        ipaddr=`echo $hostline | cut -d"~" -f1`
        echo "$svr" >> $ntcss_host_file
        echo "$ipaddr" >> $ntcss_host_file
        echo "30" >> $ntcss_host_file
        if [ $svr = $hostname ]
        then
            echo "0" >> $ntcss_host_file
            echo "0" >> $ntcss_host_file
        else
            echo "2" >> $ntcss_host_file
            echo "1" >> $ntcss_host_file
        fi
        echo "" >> $ntcss_host_file
        sethost_done=`grep "0.0.0.0" $progrps_file`
        if [ $? = "0" ]
        then
            ed - $progrps_file <<EOF
/^.HOST_$hostnumber/
+2
s/IPADDR=0.0.0.0/IPADDR=$ipaddr/
w
q
EOF
        fi
    fi
}

    # Call "process_server" for each potential system
    # the "hostnumber" relates to it's position in the baseline
    # progrps.ini file for a particular platform

    svr="$hostname"         # Do this server (the master)
    hostnumber="1"
    process_server

    svr="nalc"              # nalc is second in ported and optimized
    hostnumber="2"
    process_server

    svr="omms"              # Carrier configuration
    hostnumber="4"
    process_server

    svr="radm"
    hostnumber="3"          # Carrier configuration
    process_server

    svr="sdb"               # In-house "Oceana" nalc box (SUN - sdb inhouse)
    hostnumber="2"
    process_server

    svr="rps0"              # Force level ported
    hostnumber="3"
    process_server

    svr="rps1"              # Force level ported
    hostnumber="4"
    process_server

    svr="rps2"              # Force level ported
    hostnumber="5"
    process_server

#
#   end of sethost.sh
#
