#!/bin/csh -f
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
#    gethst.sh
#
#     17 Jan 2001
#
#
#    Will build temp .ini file from input progrps.ini file
#    specified containing all server names associated with
#    HOST_x. This short .ini file used to speed up search
#    process.
#
#

if ( $1 == "" || $2 == "" ) then
    echo "gethst.sh <input progrps file> <output file>"
    exit (1)
endif

rm -f name.file tmpfile tmpfile1 tmpfile2 hostfile.temp blankline lastline1.tmp
rm -f lastline2.tmp

grep NAME $1 | cut -d= -f2 > name.file
cp name.file tmpfile
paste -d= name.file tmpfile > tmpfile1
rm -f name.file
mv tmpfile1 name.file

echo "[HOST_NAMES]" > tmpfile1
cat tmpfile1 name.file > tmpfile2
rm name.file
mv tmpfile2 name.file

grep "\[HOST_" $1 | cut -d_ -f2 | cut -d\] -f1 > hostfile.temp

set highestnum = 0
set high_num = `cat hostfile.temp`

foreach i (${high_num}) 
    if ( $i > $highestnum ) then        
        set highestnum = $i         
    endif
end

rm -f blankline lastline1.tmp lastline2.tmp $2              
echo " " > blankline
echo "[HIGH_NUMBER]" > lastline1.tmp
echo "NUMBER=$highestnum" > lastline2.tmp
cat name.file  blankline lastline1.tmp lastline2.tmp blankline > $2 

rm -f name.file tmpfile tmpfile1 tmpfile2 hostfile.temp blankline lastline1.tmp
rm -f lastline2.tmp

