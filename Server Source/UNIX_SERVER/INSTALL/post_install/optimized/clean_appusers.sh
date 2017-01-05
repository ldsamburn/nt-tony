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
# clean_appusers.sh
#
#     
# Read the /h/NTCSSS/database/appusers file and delete any
# occurances of a user.

# Write the new file to appusers.tmp in the database directory.  It must
# be manually copied over the standard appusers file.

dbdir="/h/NTCSSS/database"
rm $dbdir/appusers.tmp
touch $dbdir/appusers.tmp

{
while (true)
do
        read "appname"
        if [ $? -ne 0 ]
        then
                echo "Done."
                exit 0
        fi
	/h/NTCSSS/bin/dot
        read "userid"
        read "longname"
        read "blank"
        read "passwd"
        read "roles"
        read "class"
	read "extra"


	if [ $userid != "$1" ]
	then
    	    echo "$appname" >> $dbdir/appusers.tmp
    	    echo "$userid" >> $dbdir/appusers.tmp
    	    echo "$longname" >> $dbdir/appusers.tmp
    	    echo "$blank" >> $dbdir/appusers.tmp
    	    echo "$passwd" >> $dbdir/appusers.tmp
    	    echo "$roles" >> $dbdir/appusers.tmp
    	    echo "$class" >> $dbdir/appusers.tmp
    	    echo "$extra" >> $dbdir/appusers.tmp
	else
	    echo " ";echo "User <$userid> deleted from appusers.tmp";echo " "
        fi

done } < /h/NTCSSS/database/appusers

#
#    end of clean_appusers.sh
#
