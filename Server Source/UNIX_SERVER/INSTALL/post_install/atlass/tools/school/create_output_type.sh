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

if [ $# -lt 2 ]; then
   echo "USAGE: $0 <App name> <printer_name> <slave_name> < 1 | 2 >"
   echo "       where 1 = a single server installation and"
   echo "             2 = a master-slave installation"
   exit 1
fi

log="/h/NTCSSS/post-install.log"
application=$1
printer=$2
slave_name=$3
suffix=$4
bindir="/h/NTCSSS/bin"
cwd=`pwd`

NUM_STUDENTS=41
FILE1=output_type.in1
FILE2=output_type.in2
OUT=output_type.out
touch $OUT

if [ ! -f "$FILE1" ]; then
   echo "File "$FILE1" does not exist."
   echo "Aborting ..."
   exit 1
fi

if [ ! -f "$FILE2" ]; then
   echo "File "$FILE2" does not exist."
   echo "Aborting ..."
   exit 1
fi

# Make report output types for master and slave

   echo "report" >> $OUT
   echo "${application}" >> $OUT
   echo "General Reports" >> $OUT
   echo "${printer}" >> $OUT
   echo "1" >> $OUT
   echo "" >> $OUT

#
# Populate class data from baseline
#
echo " "
echo "Creating output_type file"
for CLASS in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do

    sed -e "s/CLASS/${CLASS}/g" \
        -e "s/PRINTER/${printer}/g" \
        -e "s/SERVER/${application}/g" \
           < ${FILE1} >> $OUT
    $bindir/dot
done

for CLASS in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do
    sed -e "s/CLASS/${CLASS}/g" \
        -e "s/PRINTER/${printer}/g" \
        -e "s/SERVER/${application}/g" \
    	   < ${FILE2} >> $OUT
    $bindir/dot
done

if [ "$suffix" = "2" ]
then
    for CLASS in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    do

    sed -e "s/CLASS/${CLASS}/g" \
        -e "s/PRINTER/${printer}/g" \
        -e "s/SERVER/${slave_name}/g" \
           < ${FILE1} >> $OUT
    $bindir/dot
done

for CLASS in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
do
    sed -e "s/CLASS/${CLASS}/g" \
        -e "s/PRINTER/${printer}/g" \
        -e "s/SERVER/${slave_name}/g" \
           < ${FILE2} >> $OUT
    $bindir/dot
done
fi

echo " "
echo "output_type file built"
echo "output_type file built" >> $log

echo " "
$cwd/create_output_prt.sh $application $printer

exit 0

