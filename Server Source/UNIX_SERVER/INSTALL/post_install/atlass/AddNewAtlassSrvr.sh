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
#    AddNewAtlassSrvr.sh
#
#
#       28 Dec 2000
#
#        controls the scripts run to add new servers to 
#        an atlass configuration
#
#cd /h/NTCSSS/applications/data/install/post_install

clear

. ./define_variables.sh  AddNewAtlassSrvr.sh

$postdir/cleanup.sh

rm -f $postdir/slaves.ini

echo "Copying slave_master.ini slaves.ini"

cp $postdir/slave_master.ini $postdir/slaves.ini

echo "Executing awk -f setservers.awk $postdir/slaves.ini"
awk -f setservers.awk $postdir/slaves.ini

echo "Executing detect_newsrvr.sh"
$postdir/detect_newsrvr.sh
if [ "$?" != 0 ]
   then
     echo "Terminating...Rerun on master server..."
     exit 1 
fi

echo "copying newslaves.ini to slaves.ini containing only new servers"

cp $postdir/newslaves.ini $postdir/slaves.ini

#
$postdir/select_servers.sh  adding_new_servers atlass

#
#   end of AddNewAtlassSrvr.sh
#
