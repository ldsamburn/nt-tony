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
#   add_batch_users.sh 
#
#       28 Dec 2000
#

. ./define_variables.sh  add_batch_users.sh

appstr=`$gsv APPS $progrps_file`

for i in $appstr
do
        bu=`$gps $i BATCH_USER "NONE" $progrps_file`
        grp=`$gps $i UNIX_GROUP "NONE" $progrps_file`
        if [ -z "${bu}" -o -z "${grp}" -o "${bu}" = "NONE" -o "${grp}" = "NONE" ]
        then
                :
        else
                $uc_prog ADDGROUP $bu $grp
        fi
done

#
#   end of add_batch_users.sh
#
