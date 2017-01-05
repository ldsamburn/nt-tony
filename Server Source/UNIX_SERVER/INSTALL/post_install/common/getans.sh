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

        echo "Processing server - $1"
        echo "Is $1 a slave server?"
        echo " "
        ( read ans
        if [ "${ans}" = "y" -o "${ans}" = "Y" ]
        then
          /h/NTCSSS/bin/WriteProfileString HOSTS $1 $2 progrps.ini
        fi ) < /dev/tty
