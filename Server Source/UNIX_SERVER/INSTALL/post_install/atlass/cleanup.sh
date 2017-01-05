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
#    cleanup.sh
#
#      28 Dec 2000
#
#       remove selected files to begin clean test
#       of AddNewAtlassSrvr.sh
#

. ./define_variables.sh  cleanup.sh

rm -f $postdir/slaves.ini
rm -f $postdir/progrps.tmp
rm -f $postdir/newslaves.ini
rm -f $postdir/hostlist.ini
rm -f $postdir/hostname.dat
rm -f $postdir/adapt_list
rm -f $postdir/adapters_tmp.ini

#
#   end of cleanup.sh
#
