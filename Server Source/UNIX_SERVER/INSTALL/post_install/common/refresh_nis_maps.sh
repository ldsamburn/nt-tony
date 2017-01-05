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
#     refresh_nis_maps.sh
#
#
# Refresh the NIS maps by pulling from the NIS master
# 6/1/2000 fgr

echo "ntcss_adapters.byip"
/usr/sbin/ypxfr -f ntcss_adapters.byip
echo "ntcss_hosts.byaddr"
/usr/sbin/ypxfr -f ntcss_hosts.byaddr
echo "ntcss_printers.bynam"
/usr/sbin/ypxfr -f ntcss_printers.byname
echo "ntcss_sys_conf.bytag"
/usr/sbin/ypxfr -f ntcss_sys_conf.bytag
echo "ntcss_user_auth_server.byname"
/usr/sbin/ypxfr -f ntcss_user_auth_server.byname
echo "ntcss_hosts.byname"
/usr/sbin/ypxfr -f ntcss_hosts.byname

echo "passwd.byname"
/usr/sbin/ypxfr -f passwd.byname
echo "auto.master"
/usr/sbin/ypxfr -f auto.master
echo "vhe_list"
/usr/sbin/ypxfr -f vhe_list
echo "netid.byname"
/usr/sbin/ypxfr -f netid.byname
echo "publickey.byname"
/usr/sbin/ypxfr -f publickey.byname
echo "mail.byaddr"
/usr/sbin/ypxfr -f mail.byaddr
echo "mail.aliases"
/usr/sbin/ypxfr -f mail.aliases
echo "netgroup.byhost"
/usr/sbin/ypxfr -f netgroup.byhost
echo "netgroup"
/usr/sbin/ypxfr -f netgroup
echo "netgroup.byuser"
/usr/sbin/ypxfr -f netgroup.byuser
echo "protocols.byname"
/usr/sbin/ypxfr -f protocols.byname
echo "protocols.bynumber"
/usr/sbin/ypxfr -f protocols.bynumber
echo "rpc.byname"
/usr/sbin/ypxfr -f rpc.byname
echo "rpc.bynumber"
/usr/sbin/ypxfr -f rpc.bynumber
echo "group.byname"
/usr/sbin/ypxfr -f group.byname
echo "networks.byadd"
/usr/sbin/ypxfr -f networks.byaddr
echo "networks.byname"
/usr/sbin/ypxfr -f networks.byname
echo "passwd.byuid"
/usr/sbin/ypxfr -f passwd.byuid
echo "hosts.byadd"
/usr/sbin/ypxfr -f hosts.byaddr
echo "group.bygid"
/usr/sbin/ypxfr -f group.bygid
echo "ypservers"
/usr/sbin/ypxfr -f ypservers


#
#   end of refresh_nis_maps.sh
#
