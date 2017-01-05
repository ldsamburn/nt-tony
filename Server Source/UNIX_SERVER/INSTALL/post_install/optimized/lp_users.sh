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
#     lp_users.sh
#
#
clear
echo ""
echo ""
echo "Printer NTCSS Users Utility...."
echo ""
echo "Enter a printer name or RETURN to display:"
read ans
echo ""
echo ""

if [ -z "$ans" ]
then
	/h/NTCSSS/bin/lp_users | more 
else
	/h/NTCSSS/bin/lp_users $ans
fi
echo ""
echo "Press RETURN to continue."
read ans

exit 0

#
#    end of lp_users.sh
#
