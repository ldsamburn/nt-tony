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

# Create output_prt file to matching output_types

    dbdir="/h/NTCSSS/database"
    bindir="/h/NTCSSS/bin"
    log="/h/NTCSSS/post-install.log"
    cwd=`pwd`

    touch $cwd/output_prt.out

    echo " "
    echo "Creating output_prt file"
    echo "Creating output_prt file" >> $log

    {
    while (true)
    do
        read output_type
        if [ $? -ne "0" ]
        then
            break
        fi
        read program_group
        read description
        read printer_name
        read hostname
        read blank

        echo $output_type >> $cwd/output_prt.out
        echo "$program_group" >> $cwd/output_prt.out
        echo "$printer_name" >> $cwd/output_prt.out
        echo "$printer_name" >> $cwd/output_prt.out
        echo "" >> $cwd/output_prt.out

        $bindir/dot

    done
    } < $cwd/output_type.out
    echo " "
    echo "output_prt built"
    echo "output_prt built" >> $log
    echo " "
