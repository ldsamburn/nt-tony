#!/bin/sh

##########################################################################
#                                                                        #
# System-specific commands used by the SNAP System Administration        #
# Scripts which were authored by Anthony C. Amburn (Paramax)             #
#                                                                        #
# This script will define system commands that may differ between        #
# platforms.  Each sysadm script should begin by sourcing this file      #
# to ensure that certain commands are correctly defined.                 #
#                                                                        #
# IMPORTANT!!!                                                           #
# The scripts know this file as /NTCSS/env/sysadmsh.cmd                   #
#                                                                        #
# Author: Emelyn Reelachart                                              #
# Date:   May 7, 1993                                                    #
#                                                                        #
##########################################################################

#

if [ "$PLATFORM" = "HP-UX" ]
then
    AWKCMD=awk
    PSCMD="ps -ef "
    PRINTCAP_DIR=/etc
    MAILCMD=/usr/bin/mailx
    TPUTCUP=/usr/local/bin/tput_cup
    ECHO_N="echo "
    NONL='\c'
else
    AWKCMD=nawk
    PSCMD="ps -aux "
    PRINTCAP_DIR=/etc
    MAILCMD=/usr/ucb/mail
    TPUTCUP="/usr/local/bin/tput_cup"
    ECHO_N="echo -n"
    NONL=""
fi

export AWKCMD PSCMD
export PRINTCAP_DIR
export MAILCMD
export TPUTCUP
export ECHO_N
export NONL

