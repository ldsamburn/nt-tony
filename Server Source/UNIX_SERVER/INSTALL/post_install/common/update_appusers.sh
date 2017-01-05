#!/bin/csh
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

# update_appusers.sh
#
# Sweep through database/appusers, remove all entries pertaining to btusers.
# cat new appusers to old appusers to preserve current users.

	set dbdir="/h/NTCSSS/database"
	set postdir="/h/NTCSSS/applications/data/install/post_install"

	cp -p $dbdir/appusers /tmp/appusers.org

	set ckit=`grep "...btusr" $dbdir/appusers`

	foreach i ($ckit)
		echo "Updating $i"
ed - $dbdir/appusers <<EOF
/...btusr/
-1,+6d
.
w
q
EOF
	end

	cp $dbdir/appusers /tmp/appusers.old
	cat /tmp/appusers.old $postdir/appusers_enterprise > $dbdir/appusers
