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
#   net_xfer.sh
#
#
# $1= get | put
# $2= Server name to put put to | Server name to get from
# $3= Source file for transmission
# $4= Destination file

/etc/yp/net_xfer.exe $1 $2 $3 $4 

#
#   end of net_xfer.sh
#
