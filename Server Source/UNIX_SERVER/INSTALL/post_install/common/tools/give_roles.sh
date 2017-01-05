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

# give_roles.sh

# Read the /h/NTCSSS/database/appusers file and replace any
# occurances of a "0" role (no roles) with "All Users" role.  If a role
# of "12" is encountered, change it to "All Users" role.

# Write the new file to appusers.tmp in the database directory.  It must
# be manually copied over the standard appusers file.

# A new prime_db should be executed following this procedure, to re-instate
# the "access_auth" file.

dbdir="/h/NTCSSS/database"
rm $dbdir/appusers.tmp
touch $dbdir/appusers.tmp

{
while (true)
do
        read appname
        if [ $? -ne 0 ]
        then
                echo "Done."
                exit 0
        fi
        read userid
        read longname
        read blank
        read passwd
        read roles
        read class
	read extra

	if [ "${roles}" -eq "0" ]
	then
		roles="8"
	fi

        if [ "${roles}" -eq "12" ]
        then
                roles="8"
        fi

# Give certain users "App Administrator" AND "All Users"
#	if [ $userid = "sjames" -o $userid = "cbist" -o $userid = "fasm01" -o $userid = "jprice" -o $userid = "eboykin" -o $userid = "dkauffma" -o $userid = "dedwards" -o $userid = "melms" -o $userid = "ntcssii" ]
#	then
#		roles="12"
#	fi

	echo $appname >> $dbdir/appusers.tmp
	echo $userid >> $dbdir/appusers.tmp
	echo $longname >> $dbdir/appusers.tmp
	echo $blank >> $dbdir/appusers.tmp
	echo $passwd >> $dbdir/appusers.tmp
	echo $roles >> $dbdir/appusers.tmp
	echo $class >> $dbdir/appusers.tmp
	echo $extra >> $dbdir/appusers.tmp

done } < /h/NTCSSS/database/appusers

