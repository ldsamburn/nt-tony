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

# wwswitch.sh
#
# This script is called with the argument for which WW program to execute..
#
# $1 = WW function:
#
#      LEVELS
#      ALT
#      FACTS
#      RPT58


    if [ -d /BUECH/COMPS/WW/$1 ]
    then
        wwpath="/BUECH/COMPS/WW/$1"
    else
        wwpath="/SPOOL/WW/$1"
    fi

    case $1 in
    "LEVELS")
        $wwpath/start_up.ec
    ;;
    "ALT")
        $wwpath/start_up.ec
    ;;
    "FACTS")
        $wwpath/start_up.ec
    ;;
    "RPT58")
        $wwpath/start_up.ec
    ;;
    esac
