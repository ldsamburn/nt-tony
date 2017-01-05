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
spl=/h/NTCSSS/set_perms.log
rm -f /h/NTCSSS/set_perms.log
touch /h/NTCSSS/set_perms.log
missing_files="n"
if [ -f "/etc/ntcss_system_settings.ini" -o -d "/etc/ntcss_system_settings.ini" -o -p "/etc/ntcss_system_settings.ini" ]
then
    echo "/etc/ntcss_system_settings.ini"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /etc/ntcss_system_settings.ini
else
    echo "/etc/ntcss_system_settings.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/Release_Notes" -o -d "/h/NTCSSS/Release_Notes" -o -p "/h/NTCSSS/Release_Notes" ]
then
    echo "Release_Notes"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/Release_Notes
else
    echo "/h/NTCSSS/Release_Notes NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/alias_file" -o -d "/h/NTCSSS/alias_file" -o -p "/h/NTCSSS/alias_file" ]
then
    echo "alias_file"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/alias_file
else
    echo "/h/NTCSSS/alias_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/api/" -o -d "/h/NTCSSS/api/" -o -p "/h/NTCSSS/api/" ]
then
    echo "api/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/api/
else
    echo "/h/NTCSSS/api/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/api/custom_proc" -o -d "/h/NTCSSS/api/custom_proc" -o -p "/h/NTCSSS/api/custom_proc" ]
then
    echo "api/custom_proc"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/api/custom_proc
else
    echo "/h/NTCSSS/api/custom_proc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/api/msg_test" -o -d "/h/NTCSSS/api/msg_test" -o -p "/h/NTCSSS/api/msg_test" ]
then
    echo "api/msg_test"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/api/msg_test
else
    echo "/h/NTCSSS/api/msg_test NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/api/ws_exec" -o -d "/h/NTCSSS/api/ws_exec" -o -p "/h/NTCSSS/api/ws_exec" ]
then
    echo "api/ws_exec"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/api/ws_exec
else
    echo "/h/NTCSSS/api/ws_exec NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/api/ntcss_lp" -o -d "/h/NTCSSS/api/ntcss_lp" -o -p "/h/NTCSSS/api/ntcss_lp" ]
then
    echo "api/ntcss_lp"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/api/ntcss_lp
else
    echo "/h/NTCSSS/api/ntcss_lp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/" -o -d "/h/NTCSSS/applications/" -o -p "/h/NTCSSS/applications/" ]
then
    echo "applications/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/
else
    echo "/h/NTCSSS/applications/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/" -o -d "/h/NTCSSS/applications/help/" -o -p "/h/NTCSSS/applications/help/" ]
then
    echo "applications/help/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/help/
else
    echo "/h/NTCSSS/applications/help/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/users_conf.help" -o -d "/h/NTCSSS/applications/help/users_conf.help" -o -p "/h/NTCSSS/applications/help/users_conf.help" ]
then
    echo "applications/help/users_conf.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/users_conf.help
else
    echo "/h/NTCSSS/applications/help/users_conf.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/fs_archive.help" -o -d "/h/NTCSSS/applications/help/fs_archive.help" -o -p "/h/NTCSSS/applications/help/fs_archive.help" ]
then
    echo "applications/help/fs_archive.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/fs_archive.help
else
    echo "/h/NTCSSS/applications/help/fs_archive.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/sys_mon.help" -o -d "/h/NTCSSS/applications/help/sys_mon.help" -o -p "/h/NTCSSS/applications/help/sys_mon.help" ]
then
    echo "applications/help/sys_mon.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/sys_mon.help
else
    echo "/h/NTCSSS/applications/help/sys_mon.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/srv_dev.help" -o -d "/h/NTCSSS/applications/help/srv_dev.help" -o -p "/h/NTCSSS/applications/help/srv_dev.help" ]
then
    echo "applications/help/srv_dev.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/srv_dev.help
else
    echo "/h/NTCSSS/applications/help/srv_dev.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/conf_menu.help" -o -d "/h/NTCSSS/applications/help/conf_menu.help" -o -p "/h/NTCSSS/applications/help/conf_menu.help" ]
then
    echo "applications/help/conf_menu.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/conf_menu.help
else
    echo "/h/NTCSSS/applications/help/conf_menu.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/help/unix_uga.help" -o -d "/h/NTCSSS/applications/help/unix_uga.help" -o -p "/h/NTCSSS/applications/help/unix_uga.help" ]
then
    echo "applications/help/unix_uga.help"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/help/unix_uga.help
else
    echo "/h/NTCSSS/applications/help/unix_uga.help NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/" -o -d "/h/NTCSSS/applications/app-defaults/" -o -p "/h/NTCSSS/applications/app-defaults/" ]
then
    echo "applications/app-defaults/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/app-defaults/
else
    echo "/h/NTCSSS/applications/app-defaults/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Users_conf" -o -d "/h/NTCSSS/applications/app-defaults/Users_conf" -o -p "/h/NTCSSS/applications/app-defaults/Users_conf" ]
then
    echo "applications/app-defaults/Users_conf"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Users_conf
else
    echo "/h/NTCSSS/applications/app-defaults/Users_conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Fs_archive" -o -d "/h/NTCSSS/applications/app-defaults/Fs_archive" -o -p "/h/NTCSSS/applications/app-defaults/Fs_archive" ]
then
    echo "applications/app-defaults/Fs_archive"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Fs_archive
else
    echo "/h/NTCSSS/applications/app-defaults/Fs_archive NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Sys_mon" -o -d "/h/NTCSSS/applications/app-defaults/Sys_mon" -o -p "/h/NTCSSS/applications/app-defaults/Sys_mon" ]
then
    echo "applications/app-defaults/Sys_mon"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Sys_mon
else
    echo "/h/NTCSSS/applications/app-defaults/Sys_mon NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Srv_dev" -o -d "/h/NTCSSS/applications/app-defaults/Srv_dev" -o -p "/h/NTCSSS/applications/app-defaults/Srv_dev" ]
then
    echo "applications/app-defaults/Srv_dev"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Srv_dev
else
    echo "/h/NTCSSS/applications/app-defaults/Srv_dev NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Adm_menu" -o -d "/h/NTCSSS/applications/app-defaults/Adm_menu" -o -p "/h/NTCSSS/applications/app-defaults/Adm_menu" ]
then
    echo "applications/app-defaults/Adm_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Adm_menu
else
    echo "/h/NTCSSS/applications/app-defaults/Adm_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Conf_menu" -o -d "/h/NTCSSS/applications/app-defaults/Conf_menu" -o -p "/h/NTCSSS/applications/app-defaults/Conf_menu" ]
then
    echo "applications/app-defaults/Conf_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Conf_menu
else
    echo "/h/NTCSSS/applications/app-defaults/Conf_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults/Unix_uga" -o -d "/h/NTCSSS/applications/app-defaults/Unix_uga" -o -p "/h/NTCSSS/applications/app-defaults/Unix_uga" ]
then
    echo "applications/app-defaults/Unix_uga"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults/Unix_uga
else
    echo "/h/NTCSSS/applications/app-defaults/Unix_uga NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/" -o -d "/h/NTCSSS/applications/data/" -o -p "/h/NTCSSS/applications/data/" ]
then
    echo "applications/data/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/
else
    echo "/h/NTCSSS/applications/data/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/adm_menu/" -o -d "/h/NTCSSS/applications/data/adm_menu/" -o -p "/h/NTCSSS/applications/data/adm_menu/" ]
then
    echo "applications/data/adm_menu/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/adm_menu/
else
    echo "/h/NTCSSS/applications/data/adm_menu/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/adm_menu/warning_msg.doc" -o -d "/h/NTCSSS/applications/data/adm_menu/warning_msg.doc" -o -p "/h/NTCSSS/applications/data/adm_menu/warning_msg.doc" ]
then
    echo "applications/data/adm_menu/warning_msg.doc"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/adm_menu/warning_msg.doc
else
    echo "/h/NTCSSS/applications/data/adm_menu/warning_msg.doc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/adm_menu/Menus" -o -d "/h/NTCSSS/applications/data/adm_menu/Menus" -o -p "/h/NTCSSS/applications/data/adm_menu/Menus" ]
then
    echo "applications/data/adm_menu/Menus"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/adm_menu/Menus
else
    echo "/h/NTCSSS/applications/data/adm_menu/Menus NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/" -o -d "/h/NTCSSS/applications/data/install/" -o -p "/h/NTCSSS/applications/data/install/" ]
then
    echo "applications/data/install/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/
else
    echo "/h/NTCSSS/applications/data/install/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/VERSION" -o -d "/h/NTCSSS/applications/data/install/VERSION" -o -p "/h/NTCSSS/applications/data/install/VERSION" ]
then
    echo "applications/data/install/VERSION"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/VERSION
else
    echo "/h/NTCSSS/applications/data/install/VERSION NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/backup_list" -o -d "/h/NTCSSS/applications/data/install/backup_list" -o -p "/h/NTCSSS/applications/data/install/backup_list" ]
then
    echo "applications/data/install/backup_list"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/backup_list
else
    echo "/h/NTCSSS/applications/data/install/backup_list NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/lp_filter" -o -d "/h/NTCSSS/applications/data/install/lp_filter" -o -p "/h/NTCSSS/applications/data/install/lp_filter" ]
then
    echo "applications/data/install/lp_filter"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/lp_filter
else
    echo "/h/NTCSSS/applications/data/install/lp_filter NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/prsetup" -o -d "/h/NTCSSS/applications/data/install/prsetup" -o -p "/h/NTCSSS/applications/data/install/prsetup" ]
then
    echo "applications/data/install/prsetup"
    chmod u+r,u+w,u+x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/prsetup
else
    echo "/h/NTCSSS/applications/data/install/prsetup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rlaserjet" -o -d "/h/NTCSSS/applications/data/install/rlaserjet" -o -p "/h/NTCSSS/applications/data/install/rlaserjet" ]
then
    echo "applications/data/install/rlaserjet"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rlaserjet
else
    echo "/h/NTCSSS/applications/data/install/rlaserjet NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rmodel" -o -d "/h/NTCSSS/applications/data/install/rmodel" -o -p "/h/NTCSSS/applications/data/install/rmodel" ]
then
    echo "applications/data/install/rmodel"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rmodel
else
    echo "/h/NTCSSS/applications/data/install/rmodel NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rpostscript" -o -d "/h/NTCSSS/applications/data/install/rpostscript" -o -p "/h/NTCSSS/applications/data/install/rpostscript" ]
then
    echo "applications/data/install/rpostscript"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rpostscript
else
    echo "/h/NTCSSS/applications/data/install/rpostscript NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/NTCSS_SCRIPT" -o -d "/h/NTCSSS/applications/data/install/NTCSS_SCRIPT" -o -p "/h/NTCSSS/applications/data/install/NTCSS_SCRIPT" ]
then
    echo "applications/data/install/NTCSS_SCRIPT"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/NTCSS_SCRIPT
else
    echo "/h/NTCSSS/applications/data/install/NTCSS_SCRIPT NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/fix_progrps" -o -d "/h/NTCSSS/applications/data/install/fix_progrps" -o -p "/h/NTCSSS/applications/data/install/fix_progrps" ]
then
    echo "applications/data/install/fix_progrps"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/fix_progrps
else
    echo "/h/NTCSSS/applications/data/install/fix_progrps NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/TIMEZONE" -o -d "/h/NTCSSS/applications/data/install/TIMEZONE" -o -p "/h/NTCSSS/applications/data/install/TIMEZONE" ]
then
    echo "applications/data/install/TIMEZONE"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/TIMEZONE
else
    echo "/h/NTCSSS/applications/data/install/TIMEZONE NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/fix_progrps.sh" -o -d "/h/NTCSSS/applications/data/install/fix_progrps.sh" -o -p "/h/NTCSSS/applications/data/install/fix_progrps.sh" ]
then
    echo "applications/data/install/fix_progrps.sh"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/fix_progrps.sh
else
    echo "/h/NTCSSS/applications/data/install/fix_progrps.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rlaser_13cpi" -o -d "/h/NTCSSS/applications/data/install/rlaser_13cpi" -o -p "/h/NTCSSS/applications/data/install/rlaser_13cpi" ]
then
    echo "applications/data/install/rlaser_13cpi"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rlaser_13cpi
else
    echo "/h/NTCSSS/applications/data/install/rlaser_13cpi NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rjet_13cpi_nb" -o -d "/h/NTCSSS/applications/data/install/rjet_13cpi_nb" -o -p "/h/NTCSSS/applications/data/install/rjet_13cpi_nb" ]
then
    echo "applications/data/install/rjet_13cpi_nb"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rjet_13cpi_nb
else
    echo "/h/NTCSSS/applications/data/install/rjet_13cpi_nb NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/printer_integ" -o -d "/h/NTCSSS/applications/data/install/printer_integ" -o -p "/h/NTCSSS/applications/data/install/printer_integ" ]
then
    echo "applications/data/install/printer_integ"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/printer_integ
else
    echo "/h/NTCSSS/applications/data/install/printer_integ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rjet_13cpi_nb.102" -o -d "/h/NTCSSS/applications/data/install/rjet_13cpi_nb.102" -o -p "/h/NTCSSS/applications/data/install/rjet_13cpi_nb.102" ]
then
    echo "applications/data/install/rjet_13cpi_nb.102"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rjet_13cpi_nb.102
else
    echo "/h/NTCSSS/applications/data/install/rjet_13cpi_nb.102 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rlaser_13cpi.102" -o -d "/h/NTCSSS/applications/data/install/rlaser_13cpi.102" -o -p "/h/NTCSSS/applications/data/install/rlaser_13cpi.102" ]
then
    echo "applications/data/install/rlaser_13cpi.102"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rlaser_13cpi.102
else
    echo "/h/NTCSSS/applications/data/install/rlaser_13cpi.102 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rlaserjet.102" -o -d "/h/NTCSSS/applications/data/install/rlaserjet.102" -o -p "/h/NTCSSS/applications/data/install/rlaserjet.102" ]
then
    echo "applications/data/install/rlaserjet.102"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rlaserjet.102
else
    echo "/h/NTCSSS/applications/data/install/rlaserjet.102 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rmodel.102" -o -d "/h/NTCSSS/applications/data/install/rmodel.102" -o -p "/h/NTCSSS/applications/data/install/rmodel.102" ]
then
    echo "applications/data/install/rmodel.102"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rmodel.102
else
    echo "/h/NTCSSS/applications/data/install/rmodel.102 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rpostscript.102" -o -d "/h/NTCSSS/applications/data/install/rpostscript.102" -o -p "/h/NTCSSS/applications/data/install/rpostscript.102" ]
then
    echo "applications/data/install/rpostscript.102"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rpostscript.102
else
    echo "/h/NTCSSS/applications/data/install/rpostscript.102 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/merge_progrps" -o -d "/h/NTCSSS/applications/data/install/merge_progrps" -o -p "/h/NTCSSS/applications/data/install/merge_progrps" ]
then
    echo "applications/data/install/merge_progrps"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/merge_progrps
else
    echo "/h/NTCSSS/applications/data/install/merge_progrps NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/dmodel" -o -d "/h/NTCSSS/applications/data/install/dmodel" -o -p "/h/NTCSSS/applications/data/install/dmodel" ]
then
    echo "applications/data/install/dmodel"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/dmodel
else
    echo "/h/NTCSSS/applications/data/install/dmodel NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/jet_13cpi_nb" -o -d "/h/NTCSSS/applications/data/install/jet_13cpi_nb" -o -p "/h/NTCSSS/applications/data/install/jet_13cpi_nb" ]
then
    echo "applications/data/install/jet_13cpi_nb"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/jet_13cpi_nb
else
    echo "/h/NTCSSS/applications/data/install/jet_13cpi_nb NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/laser_13cpi" -o -d "/h/NTCSSS/applications/data/install/laser_13cpi" -o -p "/h/NTCSSS/applications/data/install/laser_13cpi" ]
then
    echo "applications/data/install/laser_13cpi"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/laser_13cpi
else
    echo "/h/NTCSSS/applications/data/install/laser_13cpi NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/laserjet" -o -d "/h/NTCSSS/applications/data/install/laserjet" -o -p "/h/NTCSSS/applications/data/install/laserjet" ]
then
    echo "applications/data/install/laserjet"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/laserjet
else
    echo "/h/NTCSSS/applications/data/install/laserjet NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/postscript" -o -d "/h/NTCSSS/applications/data/install/postscript" -o -p "/h/NTCSSS/applications/data/install/postscript" ]
then
    echo "applications/data/install/postscript"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/postscript
else
    echo "/h/NTCSSS/applications/data/install/postscript NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/ntcss_system_settings.ini" -o -d "/h/NTCSSS/applications/data/install/ntcss_system_settings.ini" -o -p "/h/NTCSSS/applications/data/install/ntcss_system_settings.ini" ]
then
    echo "applications/data/install/ntcss_system_settings.ini"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/ntcss_system_settings.ini
else
    echo "/h/NTCSSS/applications/data/install/ntcss_system_settings.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/pdiff" -o -d "/h/NTCSSS/applications/data/install/pdiff" -o -p "/h/NTCSSS/applications/data/install/pdiff" ]
then
    echo "applications/data/install/pdiff"
    chmod u+r,u-w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/pdiff
else
    echo "/h/NTCSSS/applications/data/install/pdiff NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/rmodelh" -o -d "/h/NTCSSS/applications/data/install/rmodelh" -o -p "/h/NTCSSS/applications/data/install/rmodelh" ]
then
    echo "applications/data/install/rmodelh"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/rmodelh
else
    echo "/h/NTCSSS/applications/data/install/rmodelh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/" -o -d "/h/NTCSSS/applications/data/install/post_install/" -o -p "/h/NTCSSS/applications/data/install/post_install/" ]
then
    echo "applications/data/install/post_install/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/
else
    echo "/h/NTCSSS/applications/data/install/post_install/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/getini" -o -d "/h/NTCSSS/applications/data/install/post_install/getini" -o -p "/h/NTCSSS/applications/data/install/post_install/getini" ]
then
    echo "applications/data/install/post_install/getini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/getini
else
    echo "/h/NTCSSS/applications/data/install/post_install/getini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/add_batch_users.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/add_batch_users.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/add_batch_users.sh" ]
then
    echo "applications/data/install/post_install/add_batch_users.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/add_batch_users.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/add_batch_users.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/addsource" -o -d "/h/NTCSSS/applications/data/install/post_install/addsource" -o -p "/h/NTCSSS/applications/data/install/post_install/addsource" ]
then
    echo "applications/data/install/post_install/addsource"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/addsource
else
    echo "/h/NTCSSS/applications/data/install/post_install/addsource NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/app_dirs_atlass" -o -d "/h/NTCSSS/applications/data/install/post_install/app_dirs_atlass" -o -p "/h/NTCSSS/applications/data/install/post_install/app_dirs_atlass" ]
then
    echo "applications/data/install/post_install/app_dirs_atlass"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/app_dirs_atlass
else
    echo "/h/NTCSSS/applications/data/install/post_install/app_dirs_atlass NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/atlass_occurance" -o -d "/h/NTCSSS/applications/data/install/post_install/atlass_occurance" -o -p "/h/NTCSSS/applications/data/install/post_install/atlass_occurance" ]
then
    echo "applications/data/install/post_install/atlass_occurance"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/atlass_occurance
else
    echo "/h/NTCSSS/applications/data/install/post_install/atlass_occurance NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/get_perms.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/get_perms.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/get_perms.sh" ]
then
    echo "applications/data/install/post_install/get_perms.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/get_perms.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/get_perms.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/bin_dir_lok" -o -d "/h/NTCSSS/applications/data/install/post_install/bin_dir_lok" -o -p "/h/NTCSSS/applications/data/install/post_install/bin_dir_lok" ]
then
    echo "applications/data/install/post_install/bin_dir_lok"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/bin_dir_lok
else
    echo "/h/NTCSSS/applications/data/install/post_install/bin_dir_lok NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/build_date" -o -d "/h/NTCSSS/applications/data/install/post_install/build_date" -o -p "/h/NTCSSS/applications/data/install/post_install/build_date" ]
then
    echo "applications/data/install/post_install/build_date"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/build_date
else
    echo "/h/NTCSSS/applications/data/install/post_install/build_date NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/do_post_install.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/do_post_install.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/do_post_install.sh" ]
then
    echo "applications/data/install/post_install/do_post_install.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/do_post_install.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/do_post_install.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/do_web.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/do_web.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/do_web.sh" ]
then
    echo "applications/data/install/post_install/do_web.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/do_web.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/do_web.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/enable_account.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/enable_account.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/enable_account.sh" ]
then
    echo "applications/data/install/post_install/enable_account.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/enable_account.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/enable_account.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/final_master" -o -d "/h/NTCSSS/applications/data/install/post_install/final_master" -o -p "/h/NTCSSS/applications/data/install/post_install/final_master" ]
then
    echo "applications/data/install/post_install/final_master"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/final_master
else
    echo "/h/NTCSSS/applications/data/install/post_install/final_master NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/final_slave" -o -d "/h/NTCSSS/applications/data/install/post_install/final_slave" -o -p "/h/NTCSSS/applications/data/install/post_install/final_slave" ]
then
    echo "applications/data/install/post_install/final_slave"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/final_slave
else
    echo "/h/NTCSSS/applications/data/install/post_install/final_slave NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/getans.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/getans.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/getans.sh" ]
then
    echo "applications/data/install/post_install/getans.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/getans.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/getans.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/getcrc" -o -d "/h/NTCSSS/applications/data/install/post_install/getcrc" -o -p "/h/NTCSSS/applications/data/install/post_install/getcrc" ]
then
    echo "applications/data/install/post_install/getcrc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/getcrc
else
    echo "/h/NTCSSS/applications/data/install/post_install/getcrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/hosts_file_lok" -o -d "/h/NTCSSS/applications/data/install/post_install/hosts_file_lok" -o -p "/h/NTCSSS/applications/data/install/post_install/hosts_file_lok" ]
then
    echo "applications/data/install/post_install/hosts_file_lok"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/hosts_file_lok
else
    echo "/h/NTCSSS/applications/data/install/post_install/hosts_file_lok NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/hosts_temp" -o -d "/h/NTCSSS/applications/data/install/post_install/hosts_temp" -o -p "/h/NTCSSS/applications/data/install/post_install/hosts_temp" ]
then
    echo "applications/data/install/post_install/hosts_temp"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/hosts_temp
else
    echo "/h/NTCSSS/applications/data/install/post_install/hosts_temp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/isntup" -o -d "/h/NTCSSS/applications/data/install/post_install/isntup" -o -p "/h/NTCSSS/applications/data/install/post_install/isntup" ]
then
    echo "applications/data/install/post_install/isntup"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/isntup
else
    echo "/h/NTCSSS/applications/data/install/post_install/isntup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/net_push" -o -d "/h/NTCSSS/applications/data/install/post_install/net_push" -o -p "/h/NTCSSS/applications/data/install/post_install/net_push" ]
then
    echo "applications/data/install/post_install/net_push"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/net_push
else
    echo "/h/NTCSSS/applications/data/install/post_install/net_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/net_xfer.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/net_xfer.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/net_xfer.sh" ]
then
    echo "applications/data/install/post_install/net_xfer.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/net_xfer.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/net_xfer.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/ntcss.env" -o -d "/h/NTCSSS/applications/data/install/post_install/ntcss.env" -o -p "/h/NTCSSS/applications/data/install/post_install/ntcss.env" ]
then
    echo "applications/data/install/post_install/ntcss.env"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/ntcss.env
else
    echo "/h/NTCSSS/applications/data/install/post_install/ntcss.env NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/ntcss.psp" -o -d "/h/NTCSSS/applications/data/install/post_install/ntcss.psp" -o -p "/h/NTCSSS/applications/data/install/post_install/ntcss.psp" ]
then
    echo "applications/data/install/post_install/ntcss.psp"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/ntcss.psp
else
    echo "/h/NTCSSS/applications/data/install/post_install/ntcss.psp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/ntcsshalt" -o -d "/h/NTCSSS/applications/data/install/post_install/ntcsshalt" -o -p "/h/NTCSSS/applications/data/install/post_install/ntcsshalt" ]
then
    echo "applications/data/install/post_install/ntcsshalt"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/ntcsshalt
else
    echo "/h/NTCSSS/applications/data/install/post_install/ntcsshalt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/ntcssinit" -o -d "/h/NTCSSS/applications/data/install/post_install/ntcssinit" -o -p "/h/NTCSSS/applications/data/install/post_install/ntcssinit" ]
then
    echo "applications/data/install/post_install/ntcssinit"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/ntcssinit
else
    echo "/h/NTCSSS/applications/data/install/post_install/ntcssinit NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/output_prt_atlass" -o -d "/h/NTCSSS/applications/data/install/post_install/output_prt_atlass" -o -p "/h/NTCSSS/applications/data/install/post_install/output_prt_atlass" ]
then
    echo "applications/data/install/post_install/output_prt_atlass"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/output_prt_atlass
else
    echo "/h/NTCSSS/applications/data/install/post_install/output_prt_atlass NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/output_type_atlass" -o -d "/h/NTCSSS/applications/data/install/post_install/output_type_atlass" -o -p "/h/NTCSSS/applications/data/install/post_install/output_type_atlass" ]
then
    echo "applications/data/install/post_install/output_type_atlass"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/output_type_atlass
else
    echo "/h/NTCSSS/applications/data/install/post_install/output_type_atlass NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/paxferd.scr" -o -d "/h/NTCSSS/applications/data/install/post_install/paxferd.scr" -o -p "/h/NTCSSS/applications/data/install/post_install/paxferd.scr" ]
then
    echo "applications/data/install/post_install/paxferd.scr"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/paxferd.scr
else
    echo "/h/NTCSSS/applications/data/install/post_install/paxferd.scr NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/progrps_atlass.ini" -o -d "/h/NTCSSS/applications/data/install/post_install/progrps_atlass.ini" -o -p "/h/NTCSSS/applications/data/install/post_install/progrps_atlass.ini" ]
then
    echo "applications/data/install/post_install/progrps_atlass.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/progrps_atlass.ini
else
    echo "/h/NTCSSS/applications/data/install/post_install/progrps_atlass.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/select_servers.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/select_servers.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/select_servers.sh" ]
then
    echo "applications/data/install/post_install/select_servers.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/select_servers.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/select_servers.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/sethost.awk" -o -d "/h/NTCSSS/applications/data/install/post_install/sethost.awk" -o -p "/h/NTCSSS/applications/data/install/post_install/sethost.awk" ]
then
    echo "applications/data/install/post_install/sethost.awk"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/sethost.awk
else
    echo "/h/NTCSSS/applications/data/install/post_install/sethost.awk NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/setlinkdata_atlass" -o -d "/h/NTCSSS/applications/data/install/post_install/setlinkdata_atlass" -o -p "/h/NTCSSS/applications/data/install/post_install/setlinkdata_atlass" ]
then
    echo "applications/data/install/post_install/setlinkdata_atlass"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/setlinkdata_atlass
else
    echo "/h/NTCSSS/applications/data/install/post_install/setlinkdata_atlass NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/setservers.awk" -o -d "/h/NTCSSS/applications/data/install/post_install/setservers.awk" -o -p "/h/NTCSSS/applications/data/install/post_install/setservers.awk" ]
then
    echo "applications/data/install/post_install/setservers.awk"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/setservers.awk
else
    echo "/h/NTCSSS/applications/data/install/post_install/setservers.awk NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/slave_master.ini" -o -d "/h/NTCSSS/applications/data/install/post_install/slave_master.ini" -o -p "/h/NTCSSS/applications/data/install/post_install/slave_master.ini" ]
then
    echo "applications/data/install/post_install/slave_master.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/slave_master.ini
else
    echo "/h/NTCSSS/applications/data/install/post_install/slave_master.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/occurance.tmp" -o -d "/h/NTCSSS/applications/data/install/post_install/occurance.tmp" -o -p "/h/NTCSSS/applications/data/install/post_install/occurance.tmp" ]
then
    echo "applications/data/install/post_install/occurance.tmp"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/occurance.tmp
else
    echo "/h/NTCSSS/applications/data/install/post_install/occurance.tmp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/update_appusers.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/update_appusers.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/update_appusers.sh" ]
then
    echo "applications/data/install/post_install/update_appusers.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/update_appusers.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/update_appusers.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/update_inhouse.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/update_inhouse.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/update_inhouse.sh" ]
then
    echo "applications/data/install/post_install/update_inhouse.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/update_inhouse.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/update_inhouse.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/updateu.sh" -o -d "/h/NTCSSS/applications/data/install/post_install/updateu.sh" -o -p "/h/NTCSSS/applications/data/install/post_install/updateu.sh" ]
then
    echo "applications/data/install/post_install/updateu.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/updateu.sh
else
    echo "/h/NTCSSS/applications/data/install/post_install/updateu.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/upper" -o -d "/h/NTCSSS/applications/data/install/post_install/upper" -o -p "/h/NTCSSS/applications/data/install/post_install/upper" ]
then
    echo "applications/data/install/post_install/upper"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/upper
else
    echo "/h/NTCSSS/applications/data/install/post_install/upper NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/fdump" -o -d "/h/NTCSSS/applications/data/install/post_install/fdump" -o -p "/h/NTCSSS/applications/data/install/post_install/fdump" ]
then
    echo "applications/data/install/post_install/fdump"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/fdump
else
    echo "/h/NTCSSS/applications/data/install/post_install/fdump NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/preinstalldb.tar" -o -d "/h/NTCSSS/applications/data/install/post_install/preinstalldb.tar" -o -p "/h/NTCSSS/applications/data/install/post_install/preinstalldb.tar" ]
then
    echo "applications/data/install/post_install/preinstalldb.tar"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/preinstalldb.tar
else
    echo "/h/NTCSSS/applications/data/install/post_install/preinstalldb.tar NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/slaves.ini" -o -d "/h/NTCSSS/applications/data/install/post_install/slaves.ini" -o -p "/h/NTCSSS/applications/data/install/post_install/slaves.ini" ]
then
    echo "applications/data/install/post_install/slaves.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/slaves.ini
else
    echo "/h/NTCSSS/applications/data/install/post_install/slaves.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/progrps.tmp" -o -d "/h/NTCSSS/applications/data/install/post_install/progrps.tmp" -o -p "/h/NTCSSS/applications/data/install/post_install/progrps.tmp" ]
then
    echo "applications/data/install/post_install/progrps.tmp"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/progrps.tmp
else
    echo "/h/NTCSSS/applications/data/install/post_install/progrps.tmp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/progrps.tmp1" -o -d "/h/NTCSSS/applications/data/install/post_install/progrps.tmp1" -o -p "/h/NTCSSS/applications/data/install/post_install/progrps.tmp1" ]
then
    echo "applications/data/install/post_install/progrps.tmp1"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/progrps.tmp1
else
    echo "/h/NTCSSS/applications/data/install/post_install/progrps.tmp1 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/post_install/done_file" -o -d "/h/NTCSSS/applications/data/install/post_install/done_file" -o -p "/h/NTCSSS/applications/data/install/post_install/done_file" ]
then
    echo "applications/data/install/post_install/done_file"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/install/post_install/done_file
else
    echo "/h/NTCSSS/applications/data/install/post_install/done_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/set_ntcss_status" -o -d "/h/NTCSSS/applications/data/install/set_ntcss_status" -o -p "/h/NTCSSS/applications/data/install/set_ntcss_status" ]
then
    echo "applications/data/install/set_ntcss_status"
    chmod u+r,u+w,u+s,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/set_ntcss_status
else
    echo "/h/NTCSSS/applications/data/install/set_ntcss_status NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/set_ntcss_pid" -o -d "/h/NTCSSS/applications/data/install/set_ntcss_pid" -o -p "/h/NTCSSS/applications/data/install/set_ntcss_pid" ]
then
    echo "applications/data/install/set_ntcss_pid"
    chmod u+r,u+w,u+s,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/set_ntcss_pid
else
    echo "/h/NTCSSS/applications/data/install/set_ntcss_pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/install/prelaunch" -o -d "/h/NTCSSS/applications/data/install/prelaunch" -o -p "/h/NTCSSS/applications/data/install/prelaunch" ]
then
    echo "applications/data/install/prelaunch"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/install/prelaunch
else
    echo "/h/NTCSSS/applications/data/install/prelaunch NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/fs_archive/" -o -d "/h/NTCSSS/applications/data/fs_archive/" -o -p "/h/NTCSSS/applications/data/fs_archive/" ]
then
    echo "applications/data/fs_archive/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/fs_archive/
else
    echo "/h/NTCSSS/applications/data/fs_archive/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/fs_archive/backup/" -o -d "/h/NTCSSS/applications/data/fs_archive/backup/" -o -p "/h/NTCSSS/applications/data/fs_archive/backup/" ]
then
    echo "applications/data/fs_archive/backup/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/fs_archive/backup/
else
    echo "/h/NTCSSS/applications/data/fs_archive/backup/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/fs_archive/restore/" -o -d "/h/NTCSSS/applications/data/fs_archive/restore/" -o -p "/h/NTCSSS/applications/data/fs_archive/restore/" ]
then
    echo "applications/data/fs_archive/restore/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/fs_archive/restore/
else
    echo "/h/NTCSSS/applications/data/fs_archive/restore/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/fs_archive/devices" -o -d "/h/NTCSSS/applications/data/fs_archive/devices" -o -p "/h/NTCSSS/applications/data/fs_archive/devices" ]
then
    echo "applications/data/fs_archive/devices"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/fs_archive/devices
else
    echo "/h/NTCSSS/applications/data/fs_archive/devices NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/srv_dev/" -o -d "/h/NTCSSS/applications/data/srv_dev/" -o -p "/h/NTCSSS/applications/data/srv_dev/" ]
then
    echo "applications/data/srv_dev/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/srv_dev/
else
    echo "/h/NTCSSS/applications/data/srv_dev/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/srv_dev/tape_specs" -o -d "/h/NTCSSS/applications/data/srv_dev/tape_specs" -o -p "/h/NTCSSS/applications/data/srv_dev/tape_specs" ]
then
    echo "applications/data/srv_dev/tape_specs"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/srv_dev/tape_specs
else
    echo "/h/NTCSSS/applications/data/srv_dev/tape_specs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/srv_dev/app_dirs" -o -d "/h/NTCSSS/applications/data/srv_dev/app_dirs" -o -p "/h/NTCSSS/applications/data/srv_dev/app_dirs" ]
then
    echo "applications/data/srv_dev/app_dirs"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/srv_dev/app_dirs
else
    echo "/h/NTCSSS/applications/data/srv_dev/app_dirs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/srv_dev/ctlrec" -o -d "/h/NTCSSS/applications/data/srv_dev/ctlrec" -o -p "/h/NTCSSS/applications/data/srv_dev/ctlrec" ]
then
    echo "applications/data/srv_dev/ctlrec"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/srv_dev/ctlrec
else
    echo "/h/NTCSSS/applications/data/srv_dev/ctlrec NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/sys_mon/" -o -d "/h/NTCSSS/applications/data/sys_mon/" -o -p "/h/NTCSSS/applications/data/sys_mon/" ]
then
    echo "applications/data/sys_mon/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/sys_mon/
else
    echo "/h/NTCSSS/applications/data/sys_mon/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/sys_mon/log_items" -o -d "/h/NTCSSS/applications/data/sys_mon/log_items" -o -p "/h/NTCSSS/applications/data/sys_mon/log_items" ]
then
    echo "applications/data/sys_mon/log_items"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/sys_mon/log_items
else
    echo "/h/NTCSSS/applications/data/sys_mon/log_items NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/" -o -d "/h/NTCSSS/applications/data/prt_conf/" -o -p "/h/NTCSSS/applications/data/prt_conf/" ]
then
    echo "applications/data/prt_conf/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/
else
    echo "/h/NTCSSS/applications/data/prt_conf/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/del_adapter" -o -d "/h/NTCSSS/applications/data/prt_conf/del_adapter" -o -p "/h/NTCSSS/applications/data/prt_conf/del_adapter" ]
then
    echo "applications/data/prt_conf/del_adapter"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/del_adapter
else
    echo "/h/NTCSSS/applications/data/prt_conf/del_adapter NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/lanprint_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/lanprint_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/lanprint_setup" ]
then
    echo "applications/data/prt_conf/lanprint_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/lanprint_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/lanprint_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/lantronix_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/lantronix_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/lantronix_setup" ]
then
    echo "applications/data/prt_conf/lantronix_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/lantronix_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/lantronix_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/m200print_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/m200print_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/m200print_setup" ]
then
    echo "applications/data/prt_conf/m200print_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/m200print_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/m200print_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/main_print" -o -d "/h/NTCSSS/applications/data/prt_conf/main_print" -o -p "/h/NTCSSS/applications/data/prt_conf/main_print" ]
then
    echo "applications/data/prt_conf/main_print"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/main_print
else
    echo "/h/NTCSSS/applications/data/prt_conf/main_print NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/microplex_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/microplex_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/microplex_setup" ]
then
    echo "applications/data/prt_conf/microplex_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/microplex_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/microplex_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/prt_conf_help.ini" -o -d "/h/NTCSSS/applications/data/prt_conf/prt_conf_help.ini" -o -p "/h/NTCSSS/applications/data/prt_conf/prt_conf_help.ini" ]
then
    echo "applications/data/prt_conf/prt_conf_help.ini"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/prt_conf/prt_conf_help.ini
else
    echo "/h/NTCSSS/applications/data/prt_conf/prt_conf_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/hostandpush" -o -d "/h/NTCSSS/applications/data/prt_conf/hostandpush" -o -p "/h/NTCSSS/applications/data/prt_conf/hostandpush" ]
then
    echo "applications/data/prt_conf/hostandpush"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/hostandpush
else
    echo "/h/NTCSSS/applications/data/prt_conf/hostandpush NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/printcap.changes" -o -d "/h/NTCSSS/applications/data/prt_conf/printcap.changes" -o -p "/h/NTCSSS/applications/data/prt_conf/printcap.changes" ]
then
    echo "applications/data/prt_conf/printcap.changes"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/prt_conf/printcap.changes
else
    echo "/h/NTCSSS/applications/data/prt_conf/printcap.changes NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/std_bootp_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/std_bootp_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/std_bootp_setup" ]
then
    echo "applications/data/prt_conf/std_bootp_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/std_bootp_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/std_bootp_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/jetprint_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/jetprint_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/jetprint_setup" ]
then
    echo "applications/data/prt_conf/jetprint_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/jetprint_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/jetprint_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/do_pcap" -o -d "/h/NTCSSS/applications/data/prt_conf/do_pcap" -o -p "/h/NTCSSS/applications/data/prt_conf/do_pcap" ]
then
    echo "applications/data/prt_conf/do_pcap"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/do_pcap
else
    echo "/h/NTCSSS/applications/data/prt_conf/do_pcap NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/m202print_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/m202print_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/m202print_setup" ]
then
    echo "applications/data/prt_conf/m202print_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/m202print_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/m202print_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/file_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/file_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/file_setup" ]
then
    echo "applications/data/prt_conf/file_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/file_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/file_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/unix_server_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/unix_server_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/unix_server_setup" ]
then
    echo "applications/data/prt_conf/unix_server_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/unix_server_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/unix_server_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/dmodel" -o -d "/h/NTCSSS/applications/data/prt_conf/dmodel" -o -p "/h/NTCSSS/applications/data/prt_conf/dmodel" ]
then
    echo "applications/data/prt_conf/dmodel"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/dmodel
else
    echo "/h/NTCSSS/applications/data/prt_conf/dmodel NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/prt_conf/server_setup" -o -d "/h/NTCSSS/applications/data/prt_conf/server_setup" -o -p "/h/NTCSSS/applications/data/prt_conf/server_setup" ]
then
    echo "applications/data/prt_conf/server_setup"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/prt_conf/server_setup
else
    echo "/h/NTCSSS/applications/data/prt_conf/server_setup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/" -o -d "/h/NTCSSS/applications/data/users_conf/" -o -p "/h/NTCSSS/applications/data/users_conf/" ]
then
    echo "applications/data/users_conf/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/users_conf/
else
    echo "/h/NTCSSS/applications/data/users_conf/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/user_config" -o -d "/h/NTCSSS/applications/data/users_conf/user_config" -o -p "/h/NTCSSS/applications/data/users_conf/user_config" ]
then
    echo "applications/data/users_conf/user_config"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/applications/data/users_conf/user_config
else
    echo "/h/NTCSSS/applications/data/users_conf/user_config NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/app_map.ini" -o -d "/h/NTCSSS/applications/data/users_conf/app_map.ini" -o -p "/h/NTCSSS/applications/data/users_conf/app_map.ini" ]
then
    echo "applications/data/users_conf/app_map.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/users_conf/app_map.ini
else
    echo "/h/NTCSSS/applications/data/users_conf/app_map.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/pmap.old" -o -d "/h/NTCSSS/applications/data/users_conf/pmap.old" -o -p "/h/NTCSSS/applications/data/users_conf/pmap.old" ]
then
    echo "applications/data/users_conf/pmap.old"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/users_conf/pmap.old
else
    echo "/h/NTCSSS/applications/data/users_conf/pmap.old NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/CheckGroupDup" -o -d "/h/NTCSSS/applications/data/users_conf/CheckGroupDup" -o -p "/h/NTCSSS/applications/data/users_conf/CheckGroupDup" ]
then
    echo "applications/data/users_conf/CheckGroupDup"
    chmod u+r,u-w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/applications/data/users_conf/CheckGroupDup
else
    echo "/h/NTCSSS/applications/data/users_conf/CheckGroupDup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/GetUserGroups" -o -d "/h/NTCSSS/applications/data/users_conf/GetUserGroups" -o -p "/h/NTCSSS/applications/data/users_conf/GetUserGroups" ]
then
    echo "applications/data/users_conf/GetUserGroups"
    chmod u+r,u-w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/applications/data/users_conf/GetUserGroups
else
    echo "/h/NTCSSS/applications/data/users_conf/GetUserGroups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/users_conf/GetUserProgs" -o -d "/h/NTCSSS/applications/data/users_conf/GetUserProgs" -o -p "/h/NTCSSS/applications/data/users_conf/GetUserProgs" ]
then
    echo "applications/data/users_conf/GetUserProgs"
    chmod u+r,u-w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/applications/data/users_conf/GetUserProgs
else
    echo "/h/NTCSSS/applications/data/users_conf/GetUserProgs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/" -o -d "/h/NTCSSS/applications/data/unix_uga/" -o -p "/h/NTCSSS/applications/data/unix_uga/" ]
then
    echo "applications/data/unix_uga/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/
else
    echo "/h/NTCSSS/applications/data/unix_uga/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/nis_push" -o -d "/h/NTCSSS/applications/data/unix_uga/nis_push" -o -p "/h/NTCSSS/applications/data/unix_uga/nis_push" ]
then
    echo "applications/data/unix_uga/nis_push"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/nis_push
else
    echo "/h/NTCSSS/applications/data/unix_uga/nis_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/user_config" -o -d "/h/NTCSSS/applications/data/unix_uga/user_config" -o -p "/h/NTCSSS/applications/data/unix_uga/user_config" ]
then
    echo "applications/data/unix_uga/user_config"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/user_config
else
    echo "/h/NTCSSS/applications/data/unix_uga/user_config NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/" ]
then
    echo "applications/data/unix_uga/shell/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/.cshrc" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/.cshrc" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/.cshrc" ]
then
    echo "applications/data/unix_uga/shell/.cshrc"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/.cshrc
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/.cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/coe_cshrc" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/coe_cshrc" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/coe_cshrc" ]
then
    echo "applications/data/unix_uga/shell/coe_cshrc"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/coe_cshrc
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/coe_cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/csh.login" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/csh.login" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/csh.login" ]
then
    echo "applications/data/unix_uga/shell/csh.login"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/csh.login
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/csh.login NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/.login" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/.login" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/.login" ]
then
    echo "applications/data/unix_uga/shell/.login"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/.login
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/.login NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/is_telnet" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/is_telnet" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/is_telnet" ]
then
    echo "applications/data/unix_uga/shell/is_telnet"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/is_telnet
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/is_telnet NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga/shell/coe_login" -o -d "/h/NTCSSS/applications/data/unix_uga/shell/coe_login" -o -p "/h/NTCSSS/applications/data/unix_uga/shell/coe_login" ]
then
    echo "applications/data/unix_uga/shell/coe_login"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga/shell/coe_login
else
    echo "/h/NTCSSS/applications/data/unix_uga/shell/coe_login NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/pdj_conf/" -o -d "/h/NTCSSS/applications/data/pdj_conf/" -o -p "/h/NTCSSS/applications/data/pdj_conf/" ]
then
    echo "applications/data/pdj_conf/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/pdj_conf/
else
    echo "/h/NTCSSS/applications/data/pdj_conf/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/" ]
then
    echo "applications/data/unix_uga.inri/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/nis_push" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/nis_push" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/nis_push" ]
then
    echo "applications/data/unix_uga.inri/nis_push"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/nis_push
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/nis_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/" ]
then
    echo "applications/data/unix_uga.inri/shell/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/.cshrc" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/.cshrc" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/.cshrc" ]
then
    echo "applications/data/unix_uga.inri/shell/.cshrc"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/.cshrc
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/.cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/.xsession" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/.xsession" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/.xsession" ]
then
    echo "applications/data/unix_uga.inri/shell/.xsession"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/.xsession
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/.xsession NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/Menus" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/Menus" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/Menus" ]
then
    echo "applications/data/unix_uga.inri/shell/Menus"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/data/unix_uga.inri/shell/Menus
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/Menus NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/" ]
then
    echo "applications/data/unix_uga.inri/shell/bin/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/bin/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/launch_xterm" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/launch_xterm" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/launch_xterm" ]
then
    echo "applications/data/unix_uga.inri/shell/bin/launch_xterm"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/bin/launch_xterm
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/bin/launch_xterm NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/" ]
then
    echo "applications/data/unix_uga.inri/shell/exe/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/exe/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/sun/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/sun/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/sun/" ]
then
    echo "applications/data/unix_uga.inri/shell/exe/sun/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/exe/sun/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/sun/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/hp/" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/hp/" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/hp/" ]
then
    echo "applications/data/unix_uga.inri/shell/exe/hp/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/shell/exe/hp/
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/shell/exe/hp/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/unix_uga.inri/user_config" -o -d "/h/NTCSSS/applications/data/unix_uga.inri/user_config" -o -p "/h/NTCSSS/applications/data/unix_uga.inri/user_config" ]
then
    echo "applications/data/unix_uga.inri/user_config"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/unix_uga.inri/user_config
else
    echo "/h/NTCSSS/applications/data/unix_uga.inri/user_config NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/" -o -d "/h/NTCSSS/applications/data/help_ini/" -o -p "/h/NTCSSS/applications/data/help_ini/" ]
then
    echo "applications/data/help_ini/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/data/help_ini/
else
    echo "/h/NTCSSS/applications/data/help_ini/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/app_prt_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/app_prt_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/app_prt_help.ini" ]
then
    echo "applications/data/help_ini/app_prt_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/app_prt_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/app_prt_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/fs_archive_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/fs_archive_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/fs_archive_help.ini" ]
then
    echo "applications/data/help_ini/fs_archive_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/fs_archive_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/fs_archive_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/pdj_conf_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/pdj_conf_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/pdj_conf_help.ini" ]
then
    echo "applications/data/help_ini/pdj_conf_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/pdj_conf_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/pdj_conf_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/pdj_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/pdj_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/pdj_help.ini" ]
then
    echo "applications/data/help_ini/pdj_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/pdj_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/pdj_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/prt_conf_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/prt_conf_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/prt_conf_help.ini" ]
then
    echo "applications/data/help_ini/prt_conf_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/prt_conf_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/prt_conf_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/srv_dev_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/srv_dev_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/srv_dev_help.ini" ]
then
    echo "applications/data/help_ini/srv_dev_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/srv_dev_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/srv_dev_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/sys_mon_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/sys_mon_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/sys_mon_help.ini" ]
then
    echo "applications/data/help_ini/sys_mon_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/sys_mon_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/sys_mon_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/unix_uga_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/unix_uga_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/unix_uga_help.ini" ]
then
    echo "applications/data/help_ini/unix_uga_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/unix_uga_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/unix_uga_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/data/help_ini/users_conf_help.ini" -o -d "/h/NTCSSS/applications/data/help_ini/users_conf_help.ini" -o -p "/h/NTCSSS/applications/data/help_ini/users_conf_help.ini" ]
then
    echo "applications/data/help_ini/users_conf_help.ini"
    chmod u+r,u+w,u-x,g+r,g+w,g-x,o+r,o+w,o-x /h/NTCSSS/applications/data/help_ini/users_conf_help.ini
else
    echo "/h/NTCSSS/applications/data/help_ini/users_conf_help.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/" -o -d "/h/NTCSSS/applications/app-defaults.sun/" -o -p "/h/NTCSSS/applications/app-defaults.sun/" ]
then
    echo "applications/app-defaults.sun/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/app-defaults.sun/
else
    echo "/h/NTCSSS/applications/app-defaults.sun/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Users_conf" -o -d "/h/NTCSSS/applications/app-defaults.sun/Users_conf" -o -p "/h/NTCSSS/applications/app-defaults.sun/Users_conf" ]
then
    echo "applications/app-defaults.sun/Users_conf"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Users_conf
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Users_conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Fs_archive" -o -d "/h/NTCSSS/applications/app-defaults.sun/Fs_archive" -o -p "/h/NTCSSS/applications/app-defaults.sun/Fs_archive" ]
then
    echo "applications/app-defaults.sun/Fs_archive"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Fs_archive
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Fs_archive NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Sys_mon" -o -d "/h/NTCSSS/applications/app-defaults.sun/Sys_mon" -o -p "/h/NTCSSS/applications/app-defaults.sun/Sys_mon" ]
then
    echo "applications/app-defaults.sun/Sys_mon"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Sys_mon
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Sys_mon NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Srv_dev" -o -d "/h/NTCSSS/applications/app-defaults.sun/Srv_dev" -o -p "/h/NTCSSS/applications/app-defaults.sun/Srv_dev" ]
then
    echo "applications/app-defaults.sun/Srv_dev"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Srv_dev
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Srv_dev NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Adm_menu" -o -d "/h/NTCSSS/applications/app-defaults.sun/Adm_menu" -o -p "/h/NTCSSS/applications/app-defaults.sun/Adm_menu" ]
then
    echo "applications/app-defaults.sun/Adm_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Adm_menu
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Adm_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Conf_menu" -o -d "/h/NTCSSS/applications/app-defaults.sun/Conf_menu" -o -p "/h/NTCSSS/applications/app-defaults.sun/Conf_menu" ]
then
    echo "applications/app-defaults.sun/Conf_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Conf_menu
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Conf_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.sun/Unix_uga" -o -d "/h/NTCSSS/applications/app-defaults.sun/Unix_uga" -o -p "/h/NTCSSS/applications/app-defaults.sun/Unix_uga" ]
then
    echo "applications/app-defaults.sun/Unix_uga"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.sun/Unix_uga
else
    echo "/h/NTCSSS/applications/app-defaults.sun/Unix_uga NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/" -o -d "/h/NTCSSS/applications/app-defaults.solaris/" -o -p "/h/NTCSSS/applications/app-defaults.solaris/" ]
then
    echo "applications/app-defaults.solaris/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/applications/app-defaults.solaris/
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Users_conf" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Users_conf" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Users_conf" ]
then
    echo "applications/app-defaults.solaris/Users_conf"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Users_conf
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Users_conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Fs_archive" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Fs_archive" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Fs_archive" ]
then
    echo "applications/app-defaults.solaris/Fs_archive"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Fs_archive
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Fs_archive NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Sys_mon" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Sys_mon" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Sys_mon" ]
then
    echo "applications/app-defaults.solaris/Sys_mon"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Sys_mon
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Sys_mon NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Srv_dev" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Srv_dev" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Srv_dev" ]
then
    echo "applications/app-defaults.solaris/Srv_dev"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Srv_dev
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Srv_dev NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Adm_menu" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Adm_menu" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Adm_menu" ]
then
    echo "applications/app-defaults.solaris/Adm_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Adm_menu
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Adm_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Conf_menu" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Conf_menu" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Conf_menu" ]
then
    echo "applications/app-defaults.solaris/Conf_menu"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Conf_menu
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Conf_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/applications/app-defaults.solaris/Unix_uga" -o -d "/h/NTCSSS/applications/app-defaults.solaris/Unix_uga" -o -p "/h/NTCSSS/applications/app-defaults.solaris/Unix_uga" ]
then
    echo "applications/app-defaults.solaris/Unix_uga"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/applications/app-defaults.solaris/Unix_uga
else
    echo "/h/NTCSSS/applications/app-defaults.solaris/Unix_uga NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/" -o -d "/h/NTCSSS/bin/" -o -p "/h/NTCSSS/bin/" ]
then
    echo "bin/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/
else
    echo "/h/NTCSSS/bin/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/GetProfileSection" -o -d "/h/NTCSSS/bin/GetProfileSection" -o -p "/h/NTCSSS/bin/GetProfileSection" ]
then
    echo "bin/GetProfileSection"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/GetProfileSection
else
    echo "/h/NTCSSS/bin/GetProfileSection NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/GetProfileString" -o -d "/h/NTCSSS/bin/GetProfileString" -o -p "/h/NTCSSS/bin/GetProfileString" ]
then
    echo "bin/GetProfileString"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/GetProfileString
else
    echo "/h/NTCSSS/bin/GetProfileString NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/GetSectionValues" -o -d "/h/NTCSSS/bin/GetSectionValues" -o -p "/h/NTCSSS/bin/GetSectionValues" ]
then
    echo "bin/GetSectionValues"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/GetSectionValues
else
    echo "/h/NTCSSS/bin/GetSectionValues NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/GetUserGroups" -o -d "/h/NTCSSS/bin/GetUserGroups" -o -p "/h/NTCSSS/bin/GetUserGroups" ]
then
    echo "bin/GetUserGroups"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/GetUserGroups
else
    echo "/h/NTCSSS/bin/GetUserGroups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/GetUserProgs" -o -d "/h/NTCSSS/bin/GetUserProgs" -o -p "/h/NTCSSS/bin/GetUserProgs" ]
then
    echo "bin/GetUserProgs"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/GetUserProgs
else
    echo "/h/NTCSSS/bin/GetUserProgs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/NtcssStartRemoteProcess" -o -d "/h/NTCSSS/bin/NtcssStartRemoteProcess" -o -p "/h/NTCSSS/bin/NtcssStartRemoteProcess" ]
then
    echo "bin/NtcssStartRemoteProcess"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/NtcssStartRemoteProcess
else
    echo "/h/NTCSSS/bin/NtcssStartRemoteProcess NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/RemoveProfileString" -o -d "/h/NTCSSS/bin/RemoveProfileString" -o -p "/h/NTCSSS/bin/RemoveProfileString" ]
then
    echo "bin/RemoveProfileString"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/RemoveProfileString
else
    echo "/h/NTCSSS/bin/RemoveProfileString NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/WriteProfileString" -o -d "/h/NTCSSS/bin/WriteProfileString" -o -p "/h/NTCSSS/bin/WriteProfileString" ]
then
    echo "bin/WriteProfileString"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/WriteProfileString
else
    echo "/h/NTCSSS/bin/WriteProfileString NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/adduser" -o -d "/h/NTCSSS/bin/adduser" -o -p "/h/NTCSSS/bin/adduser" ]
then
    echo "bin/adduser"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/adduser
else
    echo "/h/NTCSSS/bin/adduser NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/adm_menu" -o -d "/h/NTCSSS/bin/adm_menu" -o -p "/h/NTCSSS/bin/adm_menu" ]
then
    echo "bin/adm_menu"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/adm_menu
else
    echo "/h/NTCSSS/bin/adm_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/backup_tape" -o -d "/h/NTCSSS/bin/backup_tape" -o -p "/h/NTCSSS/bin/backup_tape" ]
then
    echo "bin/backup_tape"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/backup_tape
else
    echo "/h/NTCSSS/bin/backup_tape NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/backup_tape.gtar" -o -d "/h/NTCSSS/bin/backup_tape.gtar" -o -p "/h/NTCSSS/bin/backup_tape.gtar" ]
then
    echo "bin/backup_tape.gtar"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/backup_tape.gtar
else
    echo "/h/NTCSSS/bin/backup_tape.gtar NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/bcserver" -o -d "/h/NTCSSS/bin/bcserver" -o -p "/h/NTCSSS/bin/bcserver" ]
then
    echo "bin/bcserver"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/bcserver
else
    echo "/h/NTCSSS/bin/bcserver NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/conf_menu" -o -d "/h/NTCSSS/bin/conf_menu" -o -p "/h/NTCSSS/bin/conf_menu" ]
then
    echo "bin/conf_menu"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/conf_menu
else
    echo "/h/NTCSSS/bin/conf_menu NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/custom_proc" -o -d "/h/NTCSSS/bin/custom_proc" -o -p "/h/NTCSSS/bin/custom_proc" ]
then
    echo "bin/custom_proc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/custom_proc
else
    echo "/h/NTCSSS/bin/custom_proc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/force_logoff" -o -d "/h/NTCSSS/bin/force_logoff" -o -p "/h/NTCSSS/bin/force_logoff" ]
then
    echo "bin/force_logoff"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/force_logoff
else
    echo "/h/NTCSSS/bin/force_logoff NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/fs_archive" -o -d "/h/NTCSSS/bin/fs_archive" -o -p "/h/NTCSSS/bin/fs_archive" ]
then
    echo "bin/fs_archive"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/fs_archive
else
    echo "/h/NTCSSS/bin/fs_archive NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/gen_se" -o -d "/h/NTCSSS/bin/gen_se" -o -p "/h/NTCSSS/bin/gen_se" ]
then
    echo "bin/gen_se"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/gen_se
else
    echo "/h/NTCSSS/bin/gen_se NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/getappenv" -o -d "/h/NTCSSS/bin/getappenv" -o -p "/h/NTCSSS/bin/getappenv" ]
then
    echo "bin/getappenv"
    chmod u+r,u+w,u+s,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/getappenv
else
    echo "/h/NTCSSS/bin/getappenv NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/install" -o -d "/h/NTCSSS/bin/install" -o -p "/h/NTCSSS/bin/install" ]
then
    echo "bin/install"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/install
else
    echo "/h/NTCSSS/bin/install NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/interactive_test_api" -o -d "/h/NTCSSS/bin/interactive_test_api" -o -p "/h/NTCSSS/bin/interactive_test_api" ]
then
    echo "bin/interactive_test_api"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/interactive_test_api
else
    echo "/h/NTCSSS/bin/interactive_test_api NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/lp_users" -o -d "/h/NTCSSS/bin/lp_users" -o -p "/h/NTCSSS/bin/lp_users" ]
then
    echo "bin/lp_users"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/lp_users
else
    echo "/h/NTCSSS/bin/lp_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/msg_test" -o -d "/h/NTCSSS/bin/msg_test" -o -p "/h/NTCSSS/bin/msg_test" ]
then
    echo "bin/msg_test"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/msg_test
else
    echo "/h/NTCSSS/bin/msg_test NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/nalc_conv" -o -d "/h/NTCSSS/bin/nalc_conv" -o -p "/h/NTCSSS/bin/nalc_conv" ]
then
    echo "bin/nalc_conv"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/nalc_conv
else
    echo "/h/NTCSSS/bin/nalc_conv NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/nc_pwup" -o -d "/h/NTCSSS/bin/nc_pwup" -o -p "/h/NTCSSS/bin/nc_pwup" ]
then
    echo "bin/nc_pwup"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/nc_pwup
else
    echo "/h/NTCSSS/bin/nc_pwup NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/nc_server" -o -d "/h/NTCSSS/bin/nc_server" -o -p "/h/NTCSSS/bin/nc_server" ]
then
    echo "bin/nc_server"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/nc_server
else
    echo "/h/NTCSSS/bin/nc_server NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/net_passwd" -o -d "/h/NTCSSS/bin/net_passwd" -o -p "/h/NTCSSS/bin/net_passwd" ]
then
    echo "bin/net_passwd"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/net_passwd
else
    echo "/h/NTCSSS/bin/net_passwd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/net_xfer" -o -d "/h/NTCSSS/bin/net_xfer" -o -p "/h/NTCSSS/bin/net_xfer" ]
then
    echo "bin/net_xfer"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/net_xfer
else
    echo "/h/NTCSSS/bin/net_xfer NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/nt_lppd" -o -d "/h/NTCSSS/bin/nt_lppd" -o -p "/h/NTCSSS/bin/nt_lppd" ]
then
    echo "bin/nt_lppd"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/nt_lppd
else
    echo "/h/NTCSSS/bin/nt_lppd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_cmd" -o -d "/h/NTCSSS/bin/ntcss_cmd" -o -p "/h/NTCSSS/bin/ntcss_cmd" ]
then
    echo "bin/ntcss_cmd"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_cmd
else
    echo "/h/NTCSSS/bin/ntcss_cmd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_init" -o -d "/h/NTCSSS/bin/ntcss_init" -o -p "/h/NTCSSS/bin/ntcss_init" ]
then
    echo "bin/ntcss_init"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_init
else
    echo "/h/NTCSSS/bin/ntcss_init NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_stop" -o -d "/h/NTCSSS/bin/ntcss_stop" -o -p "/h/NTCSSS/bin/ntcss_stop" ]
then
    echo "bin/ntcss_stop"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_stop
else
    echo "/h/NTCSSS/bin/ntcss_stop NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_lp" -o -d "/h/NTCSSS/bin/ntcss_lp" -o -p "/h/NTCSSS/bin/ntcss_lp" ]
then
    echo "bin/ntcss_lp"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/ntcss_lp
else
    echo "/h/NTCSSS/bin/ntcss_lp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_msg_dist" -o -d "/h/NTCSSS/bin/ntcss_msg_dist" -o -p "/h/NTCSSS/bin/ntcss_msg_dist" ]
then
    echo "bin/ntcss_msg_dist"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_msg_dist
else
    echo "/h/NTCSSS/bin/ntcss_msg_dist NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_msg_dist_mngr" -o -d "/h/NTCSSS/bin/ntcss_msg_dist_mngr" -o -p "/h/NTCSSS/bin/ntcss_msg_dist_mngr" ]
then
    echo "bin/ntcss_msg_dist_mngr"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_msg_dist_mngr
else
    echo "/h/NTCSSS/bin/ntcss_msg_dist_mngr NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntlpd" -o -d "/h/NTCSSS/bin/ntlpd" -o -p "/h/NTCSSS/bin/ntlpd" ]
then
    echo "bin/ntlpd"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntlpd
else
    echo "/h/NTCSSS/bin/ntlpd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/plist" -o -d "/h/NTCSSS/bin/plist" -o -p "/h/NTCSSS/bin/plist" ]
then
    echo "bin/plist"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/plist
else
    echo "/h/NTCSSS/bin/plist NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/setlinkdata" -o -d "/h/NTCSSS/bin/setlinkdata" -o -p "/h/NTCSSS/bin/setlinkdata" ]
then
    echo "bin/setlinkdata"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/setlinkdata
else
    echo "/h/NTCSSS/bin/setlinkdata NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/prepsp" -o -d "/h/NTCSSS/bin/prepsp" -o -p "/h/NTCSSS/bin/prepsp" ]
then
    echo "bin/prepsp"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/prepsp
else
    echo "/h/NTCSSS/bin/prepsp NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/prime_db" -o -d "/h/NTCSSS/bin/prime_db" -o -p "/h/NTCSSS/bin/prime_db" ]
then
    echo "bin/prime_db"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/prime_db
else
    echo "/h/NTCSSS/bin/prime_db NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/print_ntcss_system_setting" -o -d "/h/NTCSSS/bin/print_ntcss_system_setting" -o -p "/h/NTCSSS/bin/print_ntcss_system_setting" ]
then
    echo "bin/print_ntcss_system_setting"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/print_ntcss_system_setting
else
    echo "/h/NTCSSS/bin/print_ntcss_system_setting NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/print_users" -o -d "/h/NTCSSS/bin/print_users" -o -p "/h/NTCSSS/bin/print_users" ]
then
    echo "bin/print_users"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/print_users
else
    echo "/h/NTCSSS/bin/print_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/progrps_convert" -o -d "/h/NTCSSS/bin/progrps_convert" -o -p "/h/NTCSSS/bin/progrps_convert" ]
then
    echo "bin/progrps_convert"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/progrps_convert
else
    echo "/h/NTCSSS/bin/progrps_convert NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/read_header" -o -d "/h/NTCSSS/bin/read_header" -o -p "/h/NTCSSS/bin/read_header" ]
then
    echo "bin/read_header"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/read_header
else
    echo "/h/NTCSSS/bin/read_header NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/release_req" -o -d "/h/NTCSSS/bin/release_req" -o -p "/h/NTCSSS/bin/release_req" ]
then
    echo "bin/release_req"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/release_req
else
    echo "/h/NTCSSS/bin/release_req NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/sdi_in" -o -d "/h/NTCSSS/bin/sdi_in" -o -p "/h/NTCSSS/bin/sdi_in" ]
then
    echo "bin/sdi_in"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/sdi_in
else
    echo "/h/NTCSSS/bin/sdi_in NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/restore_tape" -o -d "/h/NTCSSS/bin/restore_tape" -o -p "/h/NTCSSS/bin/restore_tape" ]
then
    echo "bin/restore_tape"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/restore_tape
else
    echo "/h/NTCSSS/bin/restore_tape NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/restore_tape.gtar" -o -d "/h/NTCSSS/bin/restore_tape.gtar" -o -p "/h/NTCSSS/bin/restore_tape.gtar" ]
then
    echo "bin/restore_tape.gtar"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/restore_tape.gtar
else
    echo "/h/NTCSSS/bin/restore_tape.gtar NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/sdi_out" -o -d "/h/NTCSSS/bin/sdi_out" -o -p "/h/NTCSSS/bin/sdi_out" ]
then
    echo "bin/sdi_out"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/sdi_out
else
    echo "/h/NTCSSS/bin/sdi_out NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/fdump" -o -d "/h/NTCSSS/bin/fdump" -o -p "/h/NTCSSS/bin/fdump" ]
then
    echo "bin/fdump"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/fdump
else
    echo "/h/NTCSSS/bin/fdump NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_start" -o -d "/h/NTCSSS/bin/ntcss_start" -o -p "/h/NTCSSS/bin/ntcss_start" ]
then
    echo "bin/ntcss_start"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_start
else
    echo "/h/NTCSSS/bin/ntcss_start NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/getcrc" -o -d "/h/NTCSSS/bin/getcrc" -o -p "/h/NTCSSS/bin/getcrc" ]
then
    echo "bin/getcrc"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/getcrc
else
    echo "/h/NTCSSS/bin/getcrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/showtxt" -o -d "/h/NTCSSS/bin/showtxt" -o -p "/h/NTCSSS/bin/showtxt" ]
then
    echo "bin/showtxt"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/showtxt
else
    echo "/h/NTCSSS/bin/showtxt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/srv_dev" -o -d "/h/NTCSSS/bin/srv_dev" -o -p "/h/NTCSSS/bin/srv_dev" ]
then
    echo "bin/srv_dev"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/srv_dev
else
    echo "/h/NTCSSS/bin/srv_dev NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/start_boot_jobs" -o -d "/h/NTCSSS/bin/start_boot_jobs" -o -p "/h/NTCSSS/bin/start_boot_jobs" ]
then
    echo "bin/start_boot_jobs"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/start_boot_jobs
else
    echo "/h/NTCSSS/bin/start_boot_jobs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/test_api" -o -d "/h/NTCSSS/bin/test_api" -o -p "/h/NTCSSS/bin/test_api" ]
then
    echo "bin/test_api"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/test_api
else
    echo "/h/NTCSSS/bin/test_api NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/test_sdi" -o -d "/h/NTCSSS/bin/test_sdi" -o -p "/h/NTCSSS/bin/test_sdi" ]
then
    echo "bin/test_sdi"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/test_sdi
else
    echo "/h/NTCSSS/bin/test_sdi NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/u2n_convert" -o -d "/h/NTCSSS/bin/u2n_convert" -o -p "/h/NTCSSS/bin/u2n_convert" ]
then
    echo "bin/u2n_convert"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/u2n_convert
else
    echo "/h/NTCSSS/bin/u2n_convert NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/unix_ini" -o -d "/h/NTCSSS/bin/unix_ini" -o -p "/h/NTCSSS/bin/unix_ini" ]
then
    echo "bin/unix_ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/unix_ini
else
    echo "/h/NTCSSS/bin/unix_ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/unix_uga" -o -d "/h/NTCSSS/bin/unix_uga" -o -p "/h/NTCSSS/bin/unix_uga" ]
then
    echo "bin/unix_uga"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/unix_uga
else
    echo "/h/NTCSSS/bin/unix_uga NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/users_conf" -o -d "/h/NTCSSS/bin/users_conf" -o -p "/h/NTCSSS/bin/users_conf" ]
then
    echo "bin/users_conf"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/users_conf
else
    echo "/h/NTCSSS/bin/users_conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ws_exec" -o -d "/h/NTCSSS/bin/ws_exec" -o -p "/h/NTCSSS/bin/ws_exec" ]
then
    echo "bin/ws_exec"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ws_exec
else
    echo "/h/NTCSSS/bin/ws_exec NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/xfer_test" -o -d "/h/NTCSSS/bin/xfer_test" -o -p "/h/NTCSSS/bin/xfer_test" ]
then
    echo "bin/xfer_test"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/xfer_test
else
    echo "/h/NTCSSS/bin/xfer_test NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_make_and_push_nis_maps" -o -d "/h/NTCSSS/bin/ntcss_make_and_push_nis_maps" -o -p "/h/NTCSSS/bin/ntcss_make_and_push_nis_maps" ]
then
    echo "bin/ntcss_make_and_push_nis_maps"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_make_and_push_nis_maps
else
    echo "/h/NTCSSS/bin/ntcss_make_and_push_nis_maps NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_system_settings.ini" -o -d "/h/NTCSSS/bin/ntcss_system_settings.ini" -o -p "/h/NTCSSS/bin/ntcss_system_settings.ini" ]
then
    echo "bin/ntcss_system_settings.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_system_settings.ini
else
    echo "/h/NTCSSS/bin/ntcss_system_settings.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/progd" -o -d "/h/NTCSSS/bin/progd" -o -p "/h/NTCSSS/bin/progd" ]
then
    echo "bin/progd"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/progd
else
    echo "/h/NTCSSS/bin/progd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/remsh" -o -d "/h/NTCSSS/bin/remsh" -o -p "/h/NTCSSS/bin/remsh" ]
then
    echo "bin/remsh"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/bin/remsh
else
    echo "/h/NTCSSS/bin/remsh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ed_test" -o -d "/h/NTCSSS/bin/ed_test" -o -p "/h/NTCSSS/bin/ed_test" ]
then
    echo "bin/ed_test"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ed_test
else
    echo "/h/NTCSSS/bin/ed_test NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_decrypt" -o -d "/h/NTCSSS/bin/ntcss_decrypt" -o -p "/h/NTCSSS/bin/ntcss_decrypt" ]
then
    echo "bin/ntcss_decrypt"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_decrypt
else
    echo "/h/NTCSSS/bin/ntcss_decrypt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin/ntcss_halt" -o -d "/h/NTCSSS/bin/ntcss_halt" -o -p "/h/NTCSSS/bin/ntcss_halt" ]
then
    echo "bin/ntcss_halt"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/bin/ntcss_halt
else
    echo "/h/NTCSSS/bin/ntcss_halt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/bin_dir_lok" -o -d "/h/NTCSSS/bin_dir_lok" -o -p "/h/NTCSSS/bin_dir_lok" ]
then
    echo "bin_dir_lok"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/bin_dir_lok
else
    echo "/h/NTCSSS/bin_dir_lok NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/" -o -d "/h/NTCSSS/database/" -o -p "/h/NTCSSS/database/" ]
then
    echo "database/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/database/
else
    echo "/h/NTCSSS/database/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/adapters" -o -d "/h/NTCSSS/database/adapters" -o -p "/h/NTCSSS/database/adapters" ]
then
    echo "database/adapters"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/adapters
else
    echo "/h/NTCSSS/database/adapters NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/appusers" -o -d "/h/NTCSSS/database/appusers" -o -p "/h/NTCSSS/database/appusers" ]
then
    echo "database/appusers"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/appusers
else
    echo "/h/NTCSSS/database/appusers NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/current_users" -o -d "/h/NTCSSS/database/current_users" -o -p "/h/NTCSSS/database/current_users" ]
then
    echo "database/current_users"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/current_users
else
    echo "/h/NTCSSS/database/current_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/login_history" -o -d "/h/NTCSSS/database/login_history" -o -p "/h/NTCSSS/database/login_history" ]
then
    echo "database/login_history"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/login_history
else
    echo "/h/NTCSSS/database/login_history NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/ntcss_users" -o -d "/h/NTCSSS/database/ntcss_users" -o -p "/h/NTCSSS/database/ntcss_users" ]
then
    echo "database/ntcss_users"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/ntcss_users
else
    echo "/h/NTCSSS/database/ntcss_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/output_type" -o -d "/h/NTCSSS/database/output_type" -o -p "/h/NTCSSS/database/output_type" ]
then
    echo "database/output_type"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/output_type
else
    echo "/h/NTCSSS/database/output_type NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/printer_access" -o -d "/h/NTCSSS/database/printer_access" -o -p "/h/NTCSSS/database/printer_access" ]
then
    echo "database/printer_access"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/printer_access
else
    echo "/h/NTCSSS/database/printer_access NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/printers" -o -d "/h/NTCSSS/database/printers" -o -p "/h/NTCSSS/database/printers" ]
then
    echo "database/printers"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/printers
else
    echo "/h/NTCSSS/database/printers NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/access_roles" -o -d "/h/NTCSSS/database/access_roles" -o -p "/h/NTCSSS/database/access_roles" ]
then
    echo "database/access_roles"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/access_roles
else
    echo "/h/NTCSSS/database/access_roles NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/ADAPTER_TYPES" -o -d "/h/NTCSSS/database/ADAPTER_TYPES" -o -p "/h/NTCSSS/database/ADAPTER_TYPES" ]
then
    echo "database/ADAPTER_TYPES"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/ADAPTER_TYPES
else
    echo "/h/NTCSSS/database/ADAPTER_TYPES NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/VERSION" -o -d "/h/NTCSSS/database/VERSION" -o -p "/h/NTCSSS/database/VERSION" ]
then
    echo "database/VERSION"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/VERSION
else
    echo "/h/NTCSSS/database/VERSION NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/output_prt" -o -d "/h/NTCSSS/database/output_prt" -o -p "/h/NTCSSS/database/output_prt" ]
then
    echo "database/output_prt"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/output_prt
else
    echo "/h/NTCSSS/database/output_prt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/current_apps" -o -d "/h/NTCSSS/database/current_apps" -o -p "/h/NTCSSS/database/current_apps" ]
then
    echo "database/current_apps"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/current_apps
else
    echo "/h/NTCSSS/database/current_apps NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/SYS_CONF" -o -d "/h/NTCSSS/database/SYS_CONF" -o -p "/h/NTCSSS/database/SYS_CONF" ]
then
    echo "database/SYS_CONF"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/SYS_CONF
else
    echo "/h/NTCSSS/database/SYS_CONF NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/predefined_jobs" -o -d "/h/NTCSSS/database/predefined_jobs" -o -p "/h/NTCSSS/database/predefined_jobs" ]
then
    echo "database/predefined_jobs"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/predefined_jobs
else
    echo "/h/NTCSSS/database/predefined_jobs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/APP_ID" -o -d "/h/NTCSSS/database/APP_ID" -o -p "/h/NTCSSS/database/APP_ID" ]
then
    echo "database/APP_ID"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/APP_ID
else
    echo "/h/NTCSSS/database/APP_ID NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/HOST_ID" -o -d "/h/NTCSSS/database/HOST_ID" -o -p "/h/NTCSSS/database/HOST_ID" ]
then
    echo "database/HOST_ID"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/HOST_ID
else
    echo "/h/NTCSSS/database/HOST_ID NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/MASTER" -o -d "/h/NTCSSS/database/MASTER" -o -p "/h/NTCSSS/database/MASTER" ]
then
    echo "database/MASTER"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/database/MASTER
else
    echo "/h/NTCSSS/database/MASTER NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/access_auth" -o -d "/h/NTCSSS/database/access_auth" -o -p "/h/NTCSSS/database/access_auth" ]
then
    echo "database/access_auth"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/access_auth
else
    echo "/h/NTCSSS/database/access_auth NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/hosts" -o -d "/h/NTCSSS/database/hosts" -o -p "/h/NTCSSS/database/hosts" ]
then
    echo "database/hosts"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/hosts
else
    echo "/h/NTCSSS/database/hosts NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/programs" -o -d "/h/NTCSSS/database/programs" -o -p "/h/NTCSSS/database/programs" ]
then
    echo "database/programs"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/programs
else
    echo "/h/NTCSSS/database/programs NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/progroups" -o -d "/h/NTCSSS/database/progroups" -o -p "/h/NTCSSS/database/progroups" ]
then
    echo "database/progroups"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/progroups
else
    echo "/h/NTCSSS/database/progroups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/progrps.ini" -o -d "/h/NTCSSS/database/progrps.ini" -o -p "/h/NTCSSS/database/progrps.ini" ]
then
    echo "database/progrps.ini"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/progrps.ini
else
    echo "/h/NTCSSS/database/progrps.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/prt_class" -o -d "/h/NTCSSS/database/prt_class" -o -p "/h/NTCSSS/database/prt_class" ]
then
    echo "database/prt_class"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/prt_class
else
    echo "/h/NTCSSS/database/prt_class NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/ADMINPROCS" -o -d "/h/NTCSSS/database/ADMINPROCS" -o -p "/h/NTCSSS/database/ADMINPROCS" ]
then
    echo "database/ADMINPROCS"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/database/ADMINPROCS
else
    echo "/h/NTCSSS/database/ADMINPROCS NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/database/ntprint.ini" -o -d "/h/NTCSSS/database/ntprint.ini" -o -p "/h/NTCSSS/database/ntprint.ini" ]
then
    echo "database/ntprint.ini"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/database/ntprint.ini
else
    echo "/h/NTCSSS/database/ntprint.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/" -o -d "/h/NTCSSS/env/" -o -p "/h/NTCSSS/env/" ]
then
    echo "env/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/
else
    echo "/h/NTCSSS/env/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/NTCSS/" -o -d "/h/NTCSSS/env/NTCSS/" -o -p "/h/NTCSSS/env/NTCSS/" ]
then
    echo "env/NTCSS/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/NTCSS/
else
    echo "/h/NTCSSS/env/NTCSS/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/NTCSS/ENV" -o -d "/h/NTCSSS/env/NTCSS/ENV" -o -p "/h/NTCSSS/env/NTCSS/ENV" ]
then
    echo "env/NTCSS/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/NTCSS/ENV
else
    echo "/h/NTCSSS/env/NTCSS/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/NTCSS/PSP" -o -d "/h/NTCSSS/env/NTCSS/PSP" -o -p "/h/NTCSSS/env/NTCSS/PSP" ]
then
    echo "env/NTCSS/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/NTCSS/PSP
else
    echo "/h/NTCSSS/env/NTCSS/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/NTCSS/SIG" -o -d "/h/NTCSSS/env/NTCSS/SIG" -o -p "/h/NTCSSS/env/NTCSS/SIG" ]
then
    echo "env/NTCSS/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/NTCSS/SIG
else
    echo "/h/NTCSSS/env/NTCSS/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/nmaster/" -o -d "/h/NTCSSS/env/nmaster/" -o -p "/h/NTCSSS/env/nmaster/" ]
then
    echo "env/nmaster/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/nmaster/
else
    echo "/h/NTCSSS/env/nmaster/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/nmaster/ENV" -o -d "/h/NTCSSS/env/nmaster/ENV" -o -p "/h/NTCSSS/env/nmaster/ENV" ]
then
    echo "env/nmaster/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/nmaster/ENV
else
    echo "/h/NTCSSS/env/nmaster/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/nmaster/PSP" -o -d "/h/NTCSSS/env/nmaster/PSP" -o -p "/h/NTCSSS/env/nmaster/PSP" ]
then
    echo "env/nmaster/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/nmaster/PSP
else
    echo "/h/NTCSSS/env/nmaster/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/nmaster/SIG" -o -d "/h/NTCSSS/env/nmaster/SIG" -o -p "/h/NTCSSS/env/nmaster/SIG" ]
then
    echo "env/nmaster/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/nmaster/SIG
else
    echo "/h/NTCSSS/env/nmaster/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_nmaster/" -o -d "/h/NTCSSS/env/IF_nmaster/" -o -p "/h/NTCSSS/env/IF_nmaster/" ]
then
    echo "env/IF_nmaster/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/IF_nmaster/
else
    echo "/h/NTCSSS/env/IF_nmaster/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_nmaster/ENV" -o -d "/h/NTCSSS/env/IF_nmaster/ENV" -o -p "/h/NTCSSS/env/IF_nmaster/ENV" ]
then
    echo "env/IF_nmaster/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_nmaster/ENV
else
    echo "/h/NTCSSS/env/IF_nmaster/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_nmaster/PSP" -o -d "/h/NTCSSS/env/IF_nmaster/PSP" -o -p "/h/NTCSSS/env/IF_nmaster/PSP" ]
then
    echo "env/IF_nmaster/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_nmaster/PSP
else
    echo "/h/NTCSSS/env/IF_nmaster/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_nmaster/SIG" -o -d "/h/NTCSSS/env/IF_nmaster/SIG" -o -p "/h/NTCSSS/env/IF_nmaster/SIG" ]
then
    echo "env/IF_nmaster/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_nmaster/SIG
else
    echo "/h/NTCSSS/env/IF_nmaster/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_nmaster/" -o -d "/h/NTCSSS/env/DB_nmaster/" -o -p "/h/NTCSSS/env/DB_nmaster/" ]
then
    echo "env/DB_nmaster/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/DB_nmaster/
else
    echo "/h/NTCSSS/env/DB_nmaster/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_nmaster/ENV" -o -d "/h/NTCSSS/env/DB_nmaster/ENV" -o -p "/h/NTCSSS/env/DB_nmaster/ENV" ]
then
    echo "env/DB_nmaster/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_nmaster/ENV
else
    echo "/h/NTCSSS/env/DB_nmaster/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_nmaster/PSP" -o -d "/h/NTCSSS/env/DB_nmaster/PSP" -o -p "/h/NTCSSS/env/DB_nmaster/PSP" ]
then
    echo "env/DB_nmaster/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_nmaster/PSP
else
    echo "/h/NTCSSS/env/DB_nmaster/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_nmaster/SIG" -o -d "/h/NTCSSS/env/DB_nmaster/SIG" -o -p "/h/NTCSSS/env/DB_nmaster/SIG" ]
then
    echo "env/DB_nmaster/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_nmaster/SIG
else
    echo "/h/NTCSSS/env/DB_nmaster/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/danasvr/" -o -d "/h/NTCSSS/env/danasvr/" -o -p "/h/NTCSSS/env/danasvr/" ]
then
    echo "env/danasvr/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/danasvr/
else
    echo "/h/NTCSSS/env/danasvr/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/danasvr/ENV" -o -d "/h/NTCSSS/env/danasvr/ENV" -o -p "/h/NTCSSS/env/danasvr/ENV" ]
then
    echo "env/danasvr/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/danasvr/ENV
else
    echo "/h/NTCSSS/env/danasvr/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/danasvr/PSP" -o -d "/h/NTCSSS/env/danasvr/PSP" -o -p "/h/NTCSSS/env/danasvr/PSP" ]
then
    echo "env/danasvr/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/danasvr/PSP
else
    echo "/h/NTCSSS/env/danasvr/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/danasvr/SIG" -o -d "/h/NTCSSS/env/danasvr/SIG" -o -p "/h/NTCSSS/env/danasvr/SIG" ]
then
    echo "env/danasvr/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/danasvr/SIG
else
    echo "/h/NTCSSS/env/danasvr/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_danasvr/" -o -d "/h/NTCSSS/env/IF_danasvr/" -o -p "/h/NTCSSS/env/IF_danasvr/" ]
then
    echo "env/IF_danasvr/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/IF_danasvr/
else
    echo "/h/NTCSSS/env/IF_danasvr/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_danasvr/ENV" -o -d "/h/NTCSSS/env/IF_danasvr/ENV" -o -p "/h/NTCSSS/env/IF_danasvr/ENV" ]
then
    echo "env/IF_danasvr/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_danasvr/ENV
else
    echo "/h/NTCSSS/env/IF_danasvr/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_danasvr/PSP" -o -d "/h/NTCSSS/env/IF_danasvr/PSP" -o -p "/h/NTCSSS/env/IF_danasvr/PSP" ]
then
    echo "env/IF_danasvr/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_danasvr/PSP
else
    echo "/h/NTCSSS/env/IF_danasvr/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/IF_danasvr/SIG" -o -d "/h/NTCSSS/env/IF_danasvr/SIG" -o -p "/h/NTCSSS/env/IF_danasvr/SIG" ]
then
    echo "env/IF_danasvr/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/IF_danasvr/SIG
else
    echo "/h/NTCSSS/env/IF_danasvr/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_danasvr/" -o -d "/h/NTCSSS/env/DB_danasvr/" -o -p "/h/NTCSSS/env/DB_danasvr/" ]
then
    echo "env/DB_danasvr/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/env/DB_danasvr/
else
    echo "/h/NTCSSS/env/DB_danasvr/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_danasvr/ENV" -o -d "/h/NTCSSS/env/DB_danasvr/ENV" -o -p "/h/NTCSSS/env/DB_danasvr/ENV" ]
then
    echo "env/DB_danasvr/ENV"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_danasvr/ENV
else
    echo "/h/NTCSSS/env/DB_danasvr/ENV NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_danasvr/PSP" -o -d "/h/NTCSSS/env/DB_danasvr/PSP" -o -p "/h/NTCSSS/env/DB_danasvr/PSP" ]
then
    echo "env/DB_danasvr/PSP"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_danasvr/PSP
else
    echo "/h/NTCSSS/env/DB_danasvr/PSP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/env/DB_danasvr/SIG" -o -d "/h/NTCSSS/env/DB_danasvr/SIG" -o -p "/h/NTCSSS/env/DB_danasvr/SIG" ]
then
    echo "env/DB_danasvr/SIG"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/env/DB_danasvr/SIG
else
    echo "/h/NTCSSS/env/DB_danasvr/SIG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/hosts_file_lok" -o -d "/h/NTCSSS/hosts_file_lok" -o -p "/h/NTCSSS/hosts_file_lok" ]
then
    echo "hosts_file_lok"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/hosts_file_lok
else
    echo "/h/NTCSSS/hosts_file_lok NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/" -o -d "/h/NTCSSS/init_data/" -o -p "/h/NTCSSS/init_data/" ]
then
    echo "init_data/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/init_data/
else
    echo "/h/NTCSSS/init_data/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/app_info/" -o -d "/h/NTCSSS/init_data/app_info/" -o -p "/h/NTCSSS/init_data/app_info/" ]
then
    echo "init_data/app_info/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/init_data/app_info/
else
    echo "/h/NTCSSS/init_data/app_info/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/app_info/NTCSS" -o -d "/h/NTCSSS/init_data/app_info/NTCSS" -o -p "/h/NTCSSS/init_data/app_info/NTCSS" ]
then
    echo "init_data/app_info/NTCSS"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/init_data/app_info/NTCSS
else
    echo "/h/NTCSSS/init_data/app_info/NTCSS NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/host_info/" -o -d "/h/NTCSSS/init_data/host_info/" -o -p "/h/NTCSSS/init_data/host_info/" ]
then
    echo "init_data/host_info/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/init_data/host_info/
else
    echo "/h/NTCSSS/init_data/host_info/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/prt_info/" -o -d "/h/NTCSSS/init_data/prt_info/" -o -p "/h/NTCSSS/init_data/prt_info/" ]
then
    echo "init_data/prt_info/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/init_data/prt_info/
else
    echo "/h/NTCSSS/init_data/prt_info/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/prt_info/classes" -o -d "/h/NTCSSS/init_data/prt_info/classes" -o -p "/h/NTCSSS/init_data/prt_info/classes" ]
then
    echo "init_data/prt_info/classes"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/init_data/prt_info/classes
else
    echo "/h/NTCSSS/init_data/prt_info/classes NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/progrps.ini" -o -d "/h/NTCSSS/init_data/progrps.ini" -o -p "/h/NTCSSS/init_data/progrps.ini" ]
then
    echo "init_data/progrps.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/progrps.ini
else
    echo "/h/NTCSSS/init_data/progrps.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/" -o -d "/h/NTCSSS/init_data/tools/" -o -p "/h/NTCSSS/init_data/tools/" ]
then
    echo "init_data/tools/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/
else
    echo "/h/NTCSSS/init_data/tools/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/nalc_conv" -o -d "/h/NTCSSS/init_data/tools/nalc_conv" -o -p "/h/NTCSSS/init_data/tools/nalc_conv" ]
then
    echo "init_data/tools/nalc_conv"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/nalc_conv
else
    echo "/h/NTCSSS/init_data/tools/nalc_conv NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/nalc_tmp.ini" -o -d "/h/NTCSSS/init_data/tools/nalc_tmp.ini" -o -p "/h/NTCSSS/init_data/tools/nalc_tmp.ini" ]
then
    echo "init_data/tools/nalc_tmp.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/nalc_tmp.ini
else
    echo "/h/NTCSSS/init_data/tools/nalc_tmp.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/update_users" -o -d "/h/NTCSSS/init_data/tools/update_users" -o -p "/h/NTCSSS/init_data/tools/update_users" ]
then
    echo "init_data/tools/update_users"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/update_users
else
    echo "/h/NTCSSS/init_data/tools/update_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/update_pcap" -o -d "/h/NTCSSS/init_data/tools/update_pcap" -o -p "/h/NTCSSS/init_data/tools/update_pcap" ]
then
    echo "init_data/tools/update_pcap"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/update_pcap
else
    echo "/h/NTCSSS/init_data/tools/update_pcap NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/" -o -d "/h/NTCSSS/init_data/tools/opt/" -o -p "/h/NTCSSS/init_data/tools/opt/" ]
then
    echo "init_data/tools/opt/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/
else
    echo "/h/NTCSSS/init_data/tools/opt/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/app_map.ini" -o -d "/h/NTCSSS/init_data/tools/opt/app_map.ini" -o -p "/h/NTCSSS/init_data/tools/opt/app_map.ini" ]
then
    echo "init_data/tools/opt/app_map.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/app_map.ini
else
    echo "/h/NTCSSS/init_data/tools/opt/app_map.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/getnalchosts" -o -d "/h/NTCSSS/init_data/tools/opt/getnalchosts" -o -p "/h/NTCSSS/init_data/tools/opt/getnalchosts" ]
then
    echo "init_data/tools/opt/getnalchosts"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/getnalchosts
else
    echo "/h/NTCSSS/init_data/tools/opt/getnalchosts NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/nt_push" -o -d "/h/NTCSSS/init_data/tools/opt/nt_push" -o -p "/h/NTCSSS/init_data/tools/opt/nt_push" ]
then
    echo "init_data/tools/opt/nt_push"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/nt_push
else
    echo "/h/NTCSSS/init_data/tools/opt/nt_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/u2n_convert" -o -d "/h/NTCSSS/init_data/tools/opt/u2n_convert" -o -p "/h/NTCSSS/init_data/tools/opt/u2n_convert" ]
then
    echo "init_data/tools/opt/u2n_convert"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/u2n_convert
else
    echo "/h/NTCSSS/init_data/tools/opt/u2n_convert NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/update_users" -o -d "/h/NTCSSS/init_data/tools/opt/update_users" -o -p "/h/NTCSSS/init_data/tools/opt/update_users" ]
then
    echo "init_data/tools/opt/update_users"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/update_users
else
    echo "/h/NTCSSS/init_data/tools/opt/update_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/nalc_tmp.ini" -o -d "/h/NTCSSS/init_data/tools/opt/nalc_tmp.ini" -o -p "/h/NTCSSS/init_data/tools/opt/nalc_tmp.ini" ]
then
    echo "init_data/tools/opt/nalc_tmp.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/nalc_tmp.ini
else
    echo "/h/NTCSSS/init_data/tools/opt/nalc_tmp.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/remove_old_groups" -o -d "/h/NTCSSS/init_data/tools/opt/remove_old_groups" -o -p "/h/NTCSSS/init_data/tools/opt/remove_old_groups" ]
then
    echo "init_data/tools/opt/remove_old_groups"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/remove_old_groups
else
    echo "/h/NTCSSS/init_data/tools/opt/remove_old_groups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/RecoveryDir/" -o -d "/h/NTCSSS/init_data/tools/opt/RecoveryDir/" -o -p "/h/NTCSSS/init_data/tools/opt/RecoveryDir/" ]
then
    echo "init_data/tools/opt/RecoveryDir/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/RecoveryDir/
else
    echo "/h/NTCSSS/init_data/tools/opt/RecoveryDir/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/update_nt_groups" -o -d "/h/NTCSSS/init_data/tools/opt/update_nt_groups" -o -p "/h/NTCSSS/init_data/tools/opt/update_nt_groups" ]
then
    echo "init_data/tools/opt/update_nt_groups"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/update_nt_groups
else
    echo "/h/NTCSSS/init_data/tools/opt/update_nt_groups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/update_users1" -o -d "/h/NTCSSS/init_data/tools/opt/update_users1" -o -p "/h/NTCSSS/init_data/tools/opt/update_users1" ]
then
    echo "init_data/tools/opt/update_users1"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/update_users1
else
    echo "/h/NTCSSS/init_data/tools/opt/update_users1 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/ntusers.ini" -o -d "/h/NTCSSS/init_data/tools/opt/ntusers.ini" -o -p "/h/NTCSSS/init_data/tools/opt/ntusers.ini" ]
then
    echo "init_data/tools/opt/ntusers.ini"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/ntusers.ini
else
    echo "/h/NTCSSS/init_data/tools/opt/ntusers.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/recover_nt_users" -o -d "/h/NTCSSS/init_data/tools/opt/recover_nt_users" -o -p "/h/NTCSSS/init_data/tools/opt/recover_nt_users" ]
then
    echo "init_data/tools/opt/recover_nt_users"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/recover_nt_users
else
    echo "/h/NTCSSS/init_data/tools/opt/recover_nt_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/opt/omsbtusr" -o -d "/h/NTCSSS/init_data/tools/opt/omsbtusr" -o -p "/h/NTCSSS/init_data/tools/opt/omsbtusr" ]
then
    echo "init_data/tools/opt/omsbtusr"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/opt/omsbtusr
else
    echo "/h/NTCSSS/init_data/tools/opt/omsbtusr NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/test_user_config" -o -d "/h/NTCSSS/init_data/tools/test_user_config" -o -p "/h/NTCSSS/init_data/tools/test_user_config" ]
then
    echo "init_data/tools/test_user_config"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/test_user_config
else
    echo "/h/NTCSSS/init_data/tools/test_user_config NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/test_adduser" -o -d "/h/NTCSSS/init_data/tools/test_adduser" -o -p "/h/NTCSSS/init_data/tools/test_adduser" ]
then
    echo "init_data/tools/test_adduser"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/test_adduser
else
    echo "/h/NTCSSS/init_data/tools/test_adduser NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/test_cshrc" -o -d "/h/NTCSSS/init_data/tools/test_cshrc" -o -p "/h/NTCSSS/init_data/tools/test_cshrc" ]
then
    echo "init_data/tools/test_cshrc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/test_cshrc
else
    echo "/h/NTCSSS/init_data/tools/test_cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/sync.sh" -o -d "/h/NTCSSS/init_data/tools/sync.sh" -o -p "/h/NTCSSS/init_data/tools/sync.sh" ]
then
    echo "init_data/tools/sync.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/sync.sh
else
    echo "/h/NTCSSS/init_data/tools/sync.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/sync_exceptions" -o -d "/h/NTCSSS/init_data/tools/sync_exceptions" -o -p "/h/NTCSSS/init_data/tools/sync_exceptions" ]
then
    echo "init_data/tools/sync_exceptions"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/sync_exceptions
else
    echo "/h/NTCSSS/init_data/tools/sync_exceptions NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/give_roles.sh" -o -d "/h/NTCSSS/init_data/tools/give_roles.sh" -o -p "/h/NTCSSS/init_data/tools/give_roles.sh" ]
then
    echo "init_data/tools/give_roles.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/give_roles.sh
else
    echo "/h/NTCSSS/init_data/tools/give_roles.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/nalc_printcap_sample" -o -d "/h/NTCSSS/init_data/tools/nalc_printcap_sample" -o -p "/h/NTCSSS/init_data/tools/nalc_printcap_sample" ]
then
    echo "init_data/tools/nalc_printcap_sample"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/nalc_printcap_sample
else
    echo "/h/NTCSSS/init_data/tools/nalc_printcap_sample NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/srv0_printcap_sample" -o -d "/h/NTCSSS/init_data/tools/srv0_printcap_sample" -o -p "/h/NTCSSS/init_data/tools/srv0_printcap_sample" ]
then
    echo "init_data/tools/srv0_printcap_sample"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/srv0_printcap_sample
else
    echo "/h/NTCSSS/init_data/tools/srv0_printcap_sample NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/" -o -d "/h/NTCSSS/init_data/tools/s1p/" -o -p "/h/NTCSSS/init_data/tools/s1p/" ]
then
    echo "init_data/tools/s1p/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/s1p/
else
    echo "/h/NTCSSS/init_data/tools/s1p/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/getnalchosts" -o -d "/h/NTCSSS/init_data/tools/s1p/getnalchosts" -o -p "/h/NTCSSS/init_data/tools/s1p/getnalchosts" ]
then
    echo "init_data/tools/s1p/getnalchosts"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/getnalchosts
else
    echo "/h/NTCSSS/init_data/tools/s1p/getnalchosts NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/update_users" -o -d "/h/NTCSSS/init_data/tools/s1p/update_users" -o -p "/h/NTCSSS/init_data/tools/s1p/update_users" ]
then
    echo "init_data/tools/s1p/update_users"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/update_users
else
    echo "/h/NTCSSS/init_data/tools/s1p/update_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/app_map.ini" -o -d "/h/NTCSSS/init_data/tools/s1p/app_map.ini" -o -p "/h/NTCSSS/init_data/tools/s1p/app_map.ini" ]
then
    echo "init_data/tools/s1p/app_map.ini"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/app_map.ini
else
    echo "/h/NTCSSS/init_data/tools/s1p/app_map.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/nt_push" -o -d "/h/NTCSSS/init_data/tools/s1p/nt_push" -o -p "/h/NTCSSS/init_data/tools/s1p/nt_push" ]
then
    echo "init_data/tools/s1p/nt_push"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/nt_push
else
    echo "/h/NTCSSS/init_data/tools/s1p/nt_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/u2n_convert" -o -d "/h/NTCSSS/init_data/tools/s1p/u2n_convert" -o -p "/h/NTCSSS/init_data/tools/s1p/u2n_convert" ]
then
    echo "init_data/tools/s1p/u2n_convert"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/u2n_convert
else
    echo "/h/NTCSSS/init_data/tools/s1p/u2n_convert NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/nalc_tmp.ini" -o -d "/h/NTCSSS/init_data/tools/s1p/nalc_tmp.ini" -o -p "/h/NTCSSS/init_data/tools/s1p/nalc_tmp.ini" ]
then
    echo "init_data/tools/s1p/nalc_tmp.ini"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/nalc_tmp.ini
else
    echo "/h/NTCSSS/init_data/tools/s1p/nalc_tmp.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s1p/remove_old_groups" -o -d "/h/NTCSSS/init_data/tools/s1p/remove_old_groups" -o -p "/h/NTCSSS/init_data/tools/s1p/remove_old_groups" ]
then
    echo "init_data/tools/s1p/remove_old_groups"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s1p/remove_old_groups
else
    echo "/h/NTCSSS/init_data/tools/s1p/remove_old_groups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/" -o -d "/h/NTCSSS/init_data/tools/s2p/" -o -p "/h/NTCSSS/init_data/tools/s2p/" ]
then
    echo "init_data/tools/s2p/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/s2p/
else
    echo "/h/NTCSSS/init_data/tools/s2p/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/app_map.ini" -o -d "/h/NTCSSS/init_data/tools/s2p/app_map.ini" -o -p "/h/NTCSSS/init_data/tools/s2p/app_map.ini" ]
then
    echo "init_data/tools/s2p/app_map.ini"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/app_map.ini
else
    echo "/h/NTCSSS/init_data/tools/s2p/app_map.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/getnalchosts" -o -d "/h/NTCSSS/init_data/tools/s2p/getnalchosts" -o -p "/h/NTCSSS/init_data/tools/s2p/getnalchosts" ]
then
    echo "init_data/tools/s2p/getnalchosts"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/getnalchosts
else
    echo "/h/NTCSSS/init_data/tools/s2p/getnalchosts NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/nt_push" -o -d "/h/NTCSSS/init_data/tools/s2p/nt_push" -o -p "/h/NTCSSS/init_data/tools/s2p/nt_push" ]
then
    echo "init_data/tools/s2p/nt_push"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/nt_push
else
    echo "/h/NTCSSS/init_data/tools/s2p/nt_push NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/u2n_convert" -o -d "/h/NTCSSS/init_data/tools/s2p/u2n_convert" -o -p "/h/NTCSSS/init_data/tools/s2p/u2n_convert" ]
then
    echo "init_data/tools/s2p/u2n_convert"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/u2n_convert
else
    echo "/h/NTCSSS/init_data/tools/s2p/u2n_convert NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/update_users" -o -d "/h/NTCSSS/init_data/tools/s2p/update_users" -o -p "/h/NTCSSS/init_data/tools/s2p/update_users" ]
then
    echo "init_data/tools/s2p/update_users"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/update_users
else
    echo "/h/NTCSSS/init_data/tools/s2p/update_users NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/nalc_tmp.ini" -o -d "/h/NTCSSS/init_data/tools/s2p/nalc_tmp.ini" -o -p "/h/NTCSSS/init_data/tools/s2p/nalc_tmp.ini" ]
then
    echo "init_data/tools/s2p/nalc_tmp.ini"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/nalc_tmp.ini
else
    echo "/h/NTCSSS/init_data/tools/s2p/nalc_tmp.ini NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/s2p/remove_old_groups" -o -d "/h/NTCSSS/init_data/tools/s2p/remove_old_groups" -o -p "/h/NTCSSS/init_data/tools/s2p/remove_old_groups" ]
then
    echo "init_data/tools/s2p/remove_old_groups"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/init_data/tools/s2p/remove_old_groups
else
    echo "/h/NTCSSS/init_data/tools/s2p/remove_old_groups NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/get_new_config" -o -d "/h/NTCSSS/init_data/tools/get_new_config" -o -p "/h/NTCSSS/init_data/tools/get_new_config" ]
then
    echo "init_data/tools/get_new_config"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/get_new_config
else
    echo "/h/NTCSSS/init_data/tools/get_new_config NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/getaline.sh" -o -d "/h/NTCSSS/init_data/tools/getaline.sh" -o -p "/h/NTCSSS/init_data/tools/getaline.sh" ]
then
    echo "init_data/tools/getaline.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/getaline.sh
else
    echo "/h/NTCSSS/init_data/tools/getaline.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/do_appusers.awk" -o -d "/h/NTCSSS/init_data/tools/do_appusers.awk" -o -p "/h/NTCSSS/init_data/tools/do_appusers.awk" ]
then
    echo "init_data/tools/do_appusers.awk"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/do_appusers.awk
else
    echo "/h/NTCSSS/init_data/tools/do_appusers.awk NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/remote_test.scr" -o -d "/h/NTCSSS/init_data/tools/remote_test.scr" -o -p "/h/NTCSSS/init_data/tools/remote_test.scr" ]
then
    echo "init_data/tools/remote_test.scr"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/remote_test.scr
else
    echo "/h/NTCSSS/init_data/tools/remote_test.scr NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/push_file" -o -d "/h/NTCSSS/init_data/tools/push_file" -o -p "/h/NTCSSS/init_data/tools/push_file" ]
then
    echo "init_data/tools/push_file"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/push_file
else
    echo "/h/NTCSSS/init_data/tools/push_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/push_pg" -o -d "/h/NTCSSS/init_data/tools/push_pg" -o -p "/h/NTCSSS/init_data/tools/push_pg" ]
then
    echo "init_data/tools/push_pg"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/push_pg
else
    echo "/h/NTCSSS/init_data/tools/push_pg NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/rename_cshrc" -o -d "/h/NTCSSS/init_data/tools/rename_cshrc" -o -p "/h/NTCSSS/init_data/tools/rename_cshrc" ]
then
    echo "init_data/tools/rename_cshrc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/rename_cshrc
else
    echo "/h/NTCSSS/init_data/tools/rename_cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/skip_updates_list" -o -d "/h/NTCSSS/init_data/tools/skip_updates_list" -o -p "/h/NTCSSS/init_data/tools/skip_updates_list" ]
then
    echo "init_data/tools/skip_updates_list"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/skip_updates_list
else
    echo "/h/NTCSSS/init_data/tools/skip_updates_list NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/replace_cshrc" -o -d "/h/NTCSSS/init_data/tools/replace_cshrc" -o -p "/h/NTCSSS/init_data/tools/replace_cshrc" ]
then
    echo "init_data/tools/replace_cshrc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/replace_cshrc
else
    echo "/h/NTCSSS/init_data/tools/replace_cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/chk_and_update_cshrc" -o -d "/h/NTCSSS/init_data/tools/chk_and_update_cshrc" -o -p "/h/NTCSSS/init_data/tools/chk_and_update_cshrc" ]
then
    echo "init_data/tools/chk_and_update_cshrc"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/chk_and_update_cshrc
else
    echo "/h/NTCSSS/init_data/tools/chk_and_update_cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/remote_target.scr" -o -d "/h/NTCSSS/init_data/tools/remote_target.scr" -o -p "/h/NTCSSS/init_data/tools/remote_target.scr" ]
then
    echo "init_data/tools/remote_target.scr"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/remote_target.scr
else
    echo "/h/NTCSSS/init_data/tools/remote_target.scr NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/xfer_list" -o -d "/h/NTCSSS/init_data/tools/xfer_list" -o -p "/h/NTCSSS/init_data/tools/xfer_list" ]
then
    echo "init_data/tools/xfer_list"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/xfer_list
else
    echo "/h/NTCSSS/init_data/tools/xfer_list NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/targetlist" -o -d "/h/NTCSSS/init_data/tools/targetlist" -o -p "/h/NTCSSS/init_data/tools/targetlist" ]
then
    echo "init_data/tools/targetlist"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/targetlist
else
    echo "/h/NTCSSS/init_data/tools/targetlist NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/filelist" -o -d "/h/NTCSSS/init_data/tools/filelist" -o -p "/h/NTCSSS/init_data/tools/filelist" ]
then
    echo "init_data/tools/filelist"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/filelist
else
    echo "/h/NTCSSS/init_data/tools/filelist NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/dynamic_list" -o -d "/h/NTCSSS/init_data/tools/dynamic_list" -o -p "/h/NTCSSS/init_data/tools/dynamic_list" ]
then
    echo "init_data/tools/dynamic_list"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/dynamic_list
else
    echo "/h/NTCSSS/init_data/tools/dynamic_list NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/tools/get_perms.sh" -o -d "/h/NTCSSS/init_data/tools/get_perms.sh" -o -p "/h/NTCSSS/init_data/tools/get_perms.sh" ]
then
    echo "init_data/tools/get_perms.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/init_data/tools/get_perms.sh
else
    echo "/h/NTCSSS/init_data/tools/get_perms.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/init_data/host.sql" -o -d "/h/NTCSSS/init_data/host.sql" -o -p "/h/NTCSSS/init_data/host.sql" ]
then
    echo "init_data/host.sql"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/init_data/host.sql
else
    echo "/h/NTCSSS/init_data/host.sql NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/" -o -d "/h/NTCSSS/libs/" -o -p "/h/NTCSSS/libs/" ]
then
    echo "libs/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/libs/
else
    echo "/h/NTCSSS/libs/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libDbInri.a" -o -d "/h/NTCSSS/libs/libDbInri.a" -o -p "/h/NTCSSS/libs/libDbInri.a" ]
then
    echo "libs/libDbInri.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libDbInri.a
else
    echo "/h/NTCSSS/libs/libDbInri.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libINI.a" -o -d "/h/NTCSSS/libs/libINI.a" -o -p "/h/NTCSSS/libs/libINI.a" ]
then
    echo "libs/libINI.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libINI.a
else
    echo "/h/NTCSSS/libs/libINI.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libNtcssApi.a" -o -d "/h/NTCSSS/libs/libNtcssApi.a" -o -p "/h/NTCSSS/libs/libNtcssApi.a" ]
then
    echo "libs/libNtcssApi.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libNtcssApi.a
else
    echo "/h/NTCSSS/libs/libNtcssApi.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libNtcssInri.a" -o -d "/h/NTCSSS/libs/libNtcssInri.a" -o -p "/h/NTCSSS/libs/libNtcssInri.a" ]
then
    echo "libs/libNtcssInri.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libNtcssInri.a
else
    echo "/h/NTCSSS/libs/libNtcssInri.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libSockInri.a" -o -d "/h/NTCSSS/libs/libSockInri.a" -o -p "/h/NTCSSS/libs/libSockInri.a" ]
then
    echo "libs/libSockInri.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libSockInri.a
else
    echo "/h/NTCSSS/libs/libSockInri.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libStrInri.a" -o -d "/h/NTCSSS/libs/libStrInri.a" -o -p "/h/NTCSSS/libs/libStrInri.a" ]
then
    echo "libs/libStrInri.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libStrInri.a
else
    echo "/h/NTCSSS/libs/libStrInri.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/libXInri.a" -o -d "/h/NTCSSS/libs/libXInri.a" -o -p "/h/NTCSSS/libs/libXInri.a" ]
then
    echo "libs/libXInri.a"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/libXInri.a
else
    echo "/h/NTCSSS/libs/libXInri.a NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/NtcssApi.h" -o -d "/h/NTCSSS/libs/NtcssApi.h" -o -p "/h/NTCSSS/libs/NtcssApi.h" ]
then
    echo "libs/NtcssApi.h"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/NtcssApi.h
else
    echo "/h/NTCSSS/libs/NtcssApi.h NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/libs/Ntcss.h" -o -d "/h/NTCSSS/libs/Ntcss.h" -o -p "/h/NTCSSS/libs/Ntcss.h" ]
then
    echo "libs/Ntcss.h"
    chmod u+r,u+w,u+x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/libs/Ntcss.h
else
    echo "/h/NTCSSS/libs/Ntcss.h NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/" -o -d "/h/NTCSSS/logs/" -o -p "/h/NTCSSS/logs/" ]
then
    echo "logs/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/logs/
else
    echo "/h/NTCSSS/logs/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/status" -o -d "/h/NTCSSS/logs/status" -o -p "/h/NTCSSS/logs/status" ]
then
    echo "logs/status"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/status
else
    echo "/h/NTCSSS/logs/status NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/status.daemons" -o -d "/h/NTCSSS/logs/status.daemons" -o -p "/h/NTCSSS/logs/status.daemons" ]
then
    echo "logs/status.daemons"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/status.daemons
else
    echo "/h/NTCSSS/logs/status.daemons NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/prime_db.log" -o -d "/h/NTCSSS/logs/prime_db.log" -o -p "/h/NTCSSS/logs/prime_db.log" ]
then
    echo "logs/prime_db.log"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/prime_db.log
else
    echo "/h/NTCSSS/logs/prime_db.log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/printer_admin.pid" -o -d "/h/NTCSSS/logs/printer_admin.pid" -o -p "/h/NTCSSS/logs/printer_admin.pid" ]
then
    echo "logs/printer_admin.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/printer_admin.pid
else
    echo "/h/NTCSSS/logs/printer_admin.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.1.pid" -o -d "/h/NTCSSS/logs/pdj_admin.1.pid" -o -p "/h/NTCSSS/logs/pdj_admin.1.pid" ]
then
    echo "logs/pdj_admin.1.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.1.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.1.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.2.pid" -o -d "/h/NTCSSS/logs/pdj_admin.2.pid" -o -p "/h/NTCSSS/logs/pdj_admin.2.pid" ]
then
    echo "logs/pdj_admin.2.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.2.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.2.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.3.pid" -o -d "/h/NTCSSS/logs/pdj_admin.3.pid" -o -p "/h/NTCSSS/logs/pdj_admin.3.pid" ]
then
    echo "logs/pdj_admin.3.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.3.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.3.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.4.pid" -o -d "/h/NTCSSS/logs/pdj_admin.4.pid" -o -p "/h/NTCSSS/logs/pdj_admin.4.pid" ]
then
    echo "logs/pdj_admin.4.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.4.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.4.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.5.pid" -o -d "/h/NTCSSS/logs/pdj_admin.5.pid" -o -p "/h/NTCSSS/logs/pdj_admin.5.pid" ]
then
    echo "logs/pdj_admin.5.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.5.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.5.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.6.pid" -o -d "/h/NTCSSS/logs/pdj_admin.6.pid" -o -p "/h/NTCSSS/logs/pdj_admin.6.pid" ]
then
    echo "logs/pdj_admin.6.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.6.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.6.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/logs/pdj_admin.7.pid" -o -d "/h/NTCSSS/logs/pdj_admin.7.pid" -o -p "/h/NTCSSS/logs/pdj_admin.7.pid" ]
then
    echo "logs/pdj_admin.7.pid"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/logs/pdj_admin.7.pid
else
    echo "/h/NTCSSS/logs/pdj_admin.7.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/" -o -d "/h/NTCSSS/message/" -o -p "/h/NTCSSS/message/" ]
then
    echo "message/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/message/
else
    echo "/h/NTCSSS/message/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_09" -o -d "/h/NTCSSS/message/bb_msgs_09" -o -p "/h/NTCSSS/message/bb_msgs_09" ]
then
    echo "message/bb_msgs_09"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_09
else
    echo "/h/NTCSSS/message/bb_msgs_09 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_AE" -o -d "/h/NTCSSS/message/bb_msgs_AE" -o -p "/h/NTCSSS/message/bb_msgs_AE" ]
then
    echo "message/bb_msgs_AE"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_AE
else
    echo "/h/NTCSSS/message/bb_msgs_AE NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_FJ" -o -d "/h/NTCSSS/message/bb_msgs_FJ" -o -p "/h/NTCSSS/message/bb_msgs_FJ" ]
then
    echo "message/bb_msgs_FJ"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_FJ
else
    echo "/h/NTCSSS/message/bb_msgs_FJ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_KO" -o -d "/h/NTCSSS/message/bb_msgs_KO" -o -p "/h/NTCSSS/message/bb_msgs_KO" ]
then
    echo "message/bb_msgs_KO"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_KO
else
    echo "/h/NTCSSS/message/bb_msgs_KO NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_PT" -o -d "/h/NTCSSS/message/bb_msgs_PT" -o -p "/h/NTCSSS/message/bb_msgs_PT" ]
then
    echo "message/bb_msgs_PT"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_PT
else
    echo "/h/NTCSSS/message/bb_msgs_PT NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_UZ" -o -d "/h/NTCSSS/message/bb_msgs_UZ" -o -p "/h/NTCSSS/message/bb_msgs_UZ" ]
then
    echo "message/bb_msgs_UZ"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_UZ
else
    echo "/h/NTCSSS/message/bb_msgs_UZ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_ae" -o -d "/h/NTCSSS/message/bb_msgs_ae" -o -p "/h/NTCSSS/message/bb_msgs_ae" ]
then
    echo "message/bb_msgs_ae"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_ae
else
    echo "/h/NTCSSS/message/bb_msgs_ae NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_fj" -o -d "/h/NTCSSS/message/bb_msgs_fj" -o -p "/h/NTCSSS/message/bb_msgs_fj" ]
then
    echo "message/bb_msgs_fj"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_fj
else
    echo "/h/NTCSSS/message/bb_msgs_fj NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_ko" -o -d "/h/NTCSSS/message/bb_msgs_ko" -o -p "/h/NTCSSS/message/bb_msgs_ko" ]
then
    echo "message/bb_msgs_ko"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_ko
else
    echo "/h/NTCSSS/message/bb_msgs_ko NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_pt" -o -d "/h/NTCSSS/message/bb_msgs_pt" -o -p "/h/NTCSSS/message/bb_msgs_pt" ]
then
    echo "message/bb_msgs_pt"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_pt
else
    echo "/h/NTCSSS/message/bb_msgs_pt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bb_msgs_uz" -o -d "/h/NTCSSS/message/bb_msgs_uz" -o -p "/h/NTCSSS/message/bb_msgs_uz" ]
then
    echo "message/bb_msgs_uz"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bb_msgs_uz
else
    echo "/h/NTCSSS/message/bb_msgs_uz NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/bul_brds" -o -d "/h/NTCSSS/message/bul_brds" -o -p "/h/NTCSSS/message/bul_brds" ]
then
    echo "message/bul_brds"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/bul_brds
else
    echo "/h/NTCSSS/message/bul_brds NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_09" -o -d "/h/NTCSSS/message/markers_09" -o -p "/h/NTCSSS/message/markers_09" ]
then
    echo "message/markers_09"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_09
else
    echo "/h/NTCSSS/message/markers_09 NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_ae" -o -d "/h/NTCSSS/message/markers_ae" -o -p "/h/NTCSSS/message/markers_ae" ]
then
    echo "message/markers_ae"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_ae
else
    echo "/h/NTCSSS/message/markers_ae NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_fj" -o -d "/h/NTCSSS/message/markers_fj" -o -p "/h/NTCSSS/message/markers_fj" ]
then
    echo "message/markers_fj"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_fj
else
    echo "/h/NTCSSS/message/markers_fj NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_ko" -o -d "/h/NTCSSS/message/markers_ko" -o -p "/h/NTCSSS/message/markers_ko" ]
then
    echo "message/markers_ko"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_ko
else
    echo "/h/NTCSSS/message/markers_ko NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_pt" -o -d "/h/NTCSSS/message/markers_pt" -o -p "/h/NTCSSS/message/markers_pt" ]
then
    echo "message/markers_pt"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_pt
else
    echo "/h/NTCSSS/message/markers_pt NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/markers_uz" -o -d "/h/NTCSSS/message/markers_uz" -o -p "/h/NTCSSS/message/markers_uz" ]
then
    echo "message/markers_uz"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/markers_uz
else
    echo "/h/NTCSSS/message/markers_uz NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/subscribers" -o -d "/h/NTCSSS/message/subscribers" -o -p "/h/NTCSSS/message/subscribers" ]
then
    echo "message/subscribers"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/subscribers
else
    echo "/h/NTCSSS/message/subscribers NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/ntcssii/" -o -d "/h/NTCSSS/message/ntcssii/" -o -p "/h/NTCSSS/message/ntcssii/" ]
then
    echo "message/ntcssii/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/ntcssii/
else
    echo "/h/NTCSSS/message/ntcssii/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/ntcssii/lock_file" -o -d "/h/NTCSSS/message/ntcssii/lock_file" -o -p "/h/NTCSSS/message/ntcssii/lock_file" ]
then
    echo "message/ntcssii/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/ntcssii/lock_file
else
    echo "/h/NTCSSS/message/ntcssii/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/001/" -o -d "/h/NTCSSS/message/001/" -o -p "/h/NTCSSS/message/001/" ]
then
    echo "message/001/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/001/
else
    echo "/h/NTCSSS/message/001/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/001/lock_file" -o -d "/h/NTCSSS/message/001/lock_file" -o -p "/h/NTCSSS/message/001/lock_file" ]
then
    echo "message/001/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/001/lock_file
else
    echo "/h/NTCSSS/message/001/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/002/" -o -d "/h/NTCSSS/message/002/" -o -p "/h/NTCSSS/message/002/" ]
then
    echo "message/002/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/002/
else
    echo "/h/NTCSSS/message/002/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/002/lock_file" -o -d "/h/NTCSSS/message/002/lock_file" -o -p "/h/NTCSSS/message/002/lock_file" ]
then
    echo "message/002/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/002/lock_file
else
    echo "/h/NTCSSS/message/002/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/003/" -o -d "/h/NTCSSS/message/003/" -o -p "/h/NTCSSS/message/003/" ]
then
    echo "message/003/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/003/
else
    echo "/h/NTCSSS/message/003/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/003/lock_file" -o -d "/h/NTCSSS/message/003/lock_file" -o -p "/h/NTCSSS/message/003/lock_file" ]
then
    echo "message/003/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/003/lock_file
else
    echo "/h/NTCSSS/message/003/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/004/" -o -d "/h/NTCSSS/message/004/" -o -p "/h/NTCSSS/message/004/" ]
then
    echo "message/004/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/004/
else
    echo "/h/NTCSSS/message/004/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/004/lock_file" -o -d "/h/NTCSSS/message/004/lock_file" -o -p "/h/NTCSSS/message/004/lock_file" ]
then
    echo "message/004/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/004/lock_file
else
    echo "/h/NTCSSS/message/004/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/005/" -o -d "/h/NTCSSS/message/005/" -o -p "/h/NTCSSS/message/005/" ]
then
    echo "message/005/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/005/
else
    echo "/h/NTCSSS/message/005/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/005/lock_file" -o -d "/h/NTCSSS/message/005/lock_file" -o -p "/h/NTCSSS/message/005/lock_file" ]
then
    echo "message/005/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/005/lock_file
else
    echo "/h/NTCSSS/message/005/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/006/" -o -d "/h/NTCSSS/message/006/" -o -p "/h/NTCSSS/message/006/" ]
then
    echo "message/006/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/006/
else
    echo "/h/NTCSSS/message/006/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/006/lock_file" -o -d "/h/NTCSSS/message/006/lock_file" -o -p "/h/NTCSSS/message/006/lock_file" ]
then
    echo "message/006/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/006/lock_file
else
    echo "/h/NTCSSS/message/006/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/007/" -o -d "/h/NTCSSS/message/007/" -o -p "/h/NTCSSS/message/007/" ]
then
    echo "message/007/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/007/
else
    echo "/h/NTCSSS/message/007/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/007/lock_file" -o -d "/h/NTCSSS/message/007/lock_file" -o -p "/h/NTCSSS/message/007/lock_file" ]
then
    echo "message/007/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/007/lock_file
else
    echo "/h/NTCSSS/message/007/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/008/" -o -d "/h/NTCSSS/message/008/" -o -p "/h/NTCSSS/message/008/" ]
then
    echo "message/008/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/008/
else
    echo "/h/NTCSSS/message/008/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/008/lock_file" -o -d "/h/NTCSSS/message/008/lock_file" -o -p "/h/NTCSSS/message/008/lock_file" ]
then
    echo "message/008/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/008/lock_file
else
    echo "/h/NTCSSS/message/008/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/009/" -o -d "/h/NTCSSS/message/009/" -o -p "/h/NTCSSS/message/009/" ]
then
    echo "message/009/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/009/
else
    echo "/h/NTCSSS/message/009/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/009/lock_file" -o -d "/h/NTCSSS/message/009/lock_file" -o -p "/h/NTCSSS/message/009/lock_file" ]
then
    echo "message/009/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/009/lock_file
else
    echo "/h/NTCSSS/message/009/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/010/" -o -d "/h/NTCSSS/message/010/" -o -p "/h/NTCSSS/message/010/" ]
then
    echo "message/010/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/010/
else
    echo "/h/NTCSSS/message/010/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/010/lock_file" -o -d "/h/NTCSSS/message/010/lock_file" -o -p "/h/NTCSSS/message/010/lock_file" ]
then
    echo "message/010/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/010/lock_file
else
    echo "/h/NTCSSS/message/010/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/011/" -o -d "/h/NTCSSS/message/011/" -o -p "/h/NTCSSS/message/011/" ]
then
    echo "message/011/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/011/
else
    echo "/h/NTCSSS/message/011/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/011/lock_file" -o -d "/h/NTCSSS/message/011/lock_file" -o -p "/h/NTCSSS/message/011/lock_file" ]
then
    echo "message/011/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/011/lock_file
else
    echo "/h/NTCSSS/message/011/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/012/" -o -d "/h/NTCSSS/message/012/" -o -p "/h/NTCSSS/message/012/" ]
then
    echo "message/012/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/012/
else
    echo "/h/NTCSSS/message/012/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/012/lock_file" -o -d "/h/NTCSSS/message/012/lock_file" -o -p "/h/NTCSSS/message/012/lock_file" ]
then
    echo "message/012/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/012/lock_file
else
    echo "/h/NTCSSS/message/012/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/013/" -o -d "/h/NTCSSS/message/013/" -o -p "/h/NTCSSS/message/013/" ]
then
    echo "message/013/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/013/
else
    echo "/h/NTCSSS/message/013/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/013/lock_file" -o -d "/h/NTCSSS/message/013/lock_file" -o -p "/h/NTCSSS/message/013/lock_file" ]
then
    echo "message/013/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/013/lock_file
else
    echo "/h/NTCSSS/message/013/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/014/" -o -d "/h/NTCSSS/message/014/" -o -p "/h/NTCSSS/message/014/" ]
then
    echo "message/014/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/014/
else
    echo "/h/NTCSSS/message/014/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/014/lock_file" -o -d "/h/NTCSSS/message/014/lock_file" -o -p "/h/NTCSSS/message/014/lock_file" ]
then
    echo "message/014/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/014/lock_file
else
    echo "/h/NTCSSS/message/014/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/015/" -o -d "/h/NTCSSS/message/015/" -o -p "/h/NTCSSS/message/015/" ]
then
    echo "message/015/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/015/
else
    echo "/h/NTCSSS/message/015/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/015/lock_file" -o -d "/h/NTCSSS/message/015/lock_file" -o -p "/h/NTCSSS/message/015/lock_file" ]
then
    echo "message/015/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/015/lock_file
else
    echo "/h/NTCSSS/message/015/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/016/" -o -d "/h/NTCSSS/message/016/" -o -p "/h/NTCSSS/message/016/" ]
then
    echo "message/016/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/016/
else
    echo "/h/NTCSSS/message/016/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/016/lock_file" -o -d "/h/NTCSSS/message/016/lock_file" -o -p "/h/NTCSSS/message/016/lock_file" ]
then
    echo "message/016/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/016/lock_file
else
    echo "/h/NTCSSS/message/016/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/017/" -o -d "/h/NTCSSS/message/017/" -o -p "/h/NTCSSS/message/017/" ]
then
    echo "message/017/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/017/
else
    echo "/h/NTCSSS/message/017/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/017/lock_file" -o -d "/h/NTCSSS/message/017/lock_file" -o -p "/h/NTCSSS/message/017/lock_file" ]
then
    echo "message/017/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/017/lock_file
else
    echo "/h/NTCSSS/message/017/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/018/" -o -d "/h/NTCSSS/message/018/" -o -p "/h/NTCSSS/message/018/" ]
then
    echo "message/018/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/018/
else
    echo "/h/NTCSSS/message/018/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/018/lock_file" -o -d "/h/NTCSSS/message/018/lock_file" -o -p "/h/NTCSSS/message/018/lock_file" ]
then
    echo "message/018/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/018/lock_file
else
    echo "/h/NTCSSS/message/018/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/019/" -o -d "/h/NTCSSS/message/019/" -o -p "/h/NTCSSS/message/019/" ]
then
    echo "message/019/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/019/
else
    echo "/h/NTCSSS/message/019/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/019/lock_file" -o -d "/h/NTCSSS/message/019/lock_file" -o -p "/h/NTCSSS/message/019/lock_file" ]
then
    echo "message/019/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/019/lock_file
else
    echo "/h/NTCSSS/message/019/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/020/" -o -d "/h/NTCSSS/message/020/" -o -p "/h/NTCSSS/message/020/" ]
then
    echo "message/020/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/020/
else
    echo "/h/NTCSSS/message/020/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/020/lock_file" -o -d "/h/NTCSSS/message/020/lock_file" -o -p "/h/NTCSSS/message/020/lock_file" ]
then
    echo "message/020/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/020/lock_file
else
    echo "/h/NTCSSS/message/020/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/atlbtusr/" -o -d "/h/NTCSSS/message/atlbtusr/" -o -p "/h/NTCSSS/message/atlbtusr/" ]
then
    echo "message/atlbtusr/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/atlbtusr/
else
    echo "/h/NTCSSS/message/atlbtusr/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/message/atlbtusr/lock_file" -o -d "/h/NTCSSS/message/atlbtusr/lock_file" -o -p "/h/NTCSSS/message/atlbtusr/lock_file" ]
then
    echo "message/atlbtusr/lock_file"
    chmod u+r,u+w,u-x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/message/atlbtusr/lock_file
else
    echo "/h/NTCSSS/message/atlbtusr/lock_file NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/notes/" -o -d "/h/NTCSSS/notes/" -o -p "/h/NTCSSS/notes/" ]
then
    echo "notes/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/notes/
else
    echo "/h/NTCSSS/notes/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/notes/Release_Notes" -o -d "/h/NTCSSS/notes/Release_Notes" -o -p "/h/NTCSSS/notes/Release_Notes" ]
then
    echo "notes/Release_Notes"
    chmod u+r,u-w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/notes/Release_Notes
else
    echo "/h/NTCSSS/notes/Release_Notes NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/ntcss_home_dir" -o -d "/h/NTCSSS/ntcss_home_dir" -o -p "/h/NTCSSS/ntcss_home_dir" ]
then
    echo "ntcss_home_dir"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/ntcss_home_dir
else
    echo "/h/NTCSSS/ntcss_home_dir NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/" -o -d "/h/NTCSSS/pcload/" -o -p "/h/NTCSSS/pcload/" ]
then
    echo "pcload/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/pcload/
else
    echo "/h/NTCSSS/pcload/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/setup.exe" -o -d "/h/NTCSSS/pcload/setup.exe" -o -p "/h/NTCSSS/pcload/setup.exe" ]
then
    echo "pcload/setup.exe"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/pcload/setup.exe
else
    echo "/h/NTCSSS/pcload/setup.exe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/vicware408.exe" -o -d "/h/NTCSSS/pcload/vicware408.exe" -o -p "/h/NTCSSS/pcload/vicware408.exe" ]
then
    echo "pcload/vicware408.exe"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/pcload/vicware408.exe
else
    echo "/h/NTCSSS/pcload/vicware408.exe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/tonyware.exe" -o -d "/h/NTCSSS/pcload/tonyware.exe" -o -p "/h/NTCSSS/pcload/tonyware.exe" ]
then
    echo "pcload/tonyware.exe"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/pcload/tonyware.exe
else
    echo "/h/NTCSSS/pcload/tonyware.exe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/ntcss2_115.exe" -o -d "/h/NTCSSS/pcload/ntcss2_115.exe" -o -p "/h/NTCSSS/pcload/ntcss2_115.exe" ]
then
    echo "pcload/ntcss2_115.exe"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/pcload/ntcss2_115.exe
else
    echo "/h/NTCSSS/pcload/ntcss2_115.exe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/pcload/frame_dbg.exe" -o -d "/h/NTCSSS/pcload/frame_dbg.exe" -o -p "/h/NTCSSS/pcload/frame_dbg.exe" ]
then
    echo "pcload/frame_dbg.exe"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/pcload/frame_dbg.exe
else
    echo "/h/NTCSSS/pcload/frame_dbg.exe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/post-install.log" -o -d "/h/NTCSSS/post-install.log" -o -p "/h/NTCSSS/post-install.log" ]
then
    echo "post-install.log"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/post-install.log
else
    echo "/h/NTCSSS/post-install.log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/progrpslok" -o -d "/h/NTCSSS/progrpslok" -o -p "/h/NTCSSS/progrpslok" ]
then
    echo "progrpslok"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/progrpslok
else
    echo "/h/NTCSSS/progrpslok NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/" -o -d "/h/NTCSSS/spool/" -o -p "/h/NTCSSS/spool/" ]
then
    echo "spool/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/
else
    echo "/h/NTCSSS/spool/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/" -o -d "/h/NTCSSS/spool/ntdrivers/" -o -p "/h/NTCSSS/spool/ntdrivers/" ]
then
    echo "spool/ntdrivers/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/
else
    echo "/h/NTCSSS/spool/ntdrivers/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/RASDDUI.DLL" -o -d "/h/NTCSSS/spool/ntdrivers/RASDDUI.DLL" -o -p "/h/NTCSSS/spool/ntdrivers/RASDDUI.DLL" ]
then
    echo "spool/ntdrivers/RASDDUI.DLL"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/RASDDUI.DLL
else
    echo "/h/NTCSSS/spool/ntdrivers/RASDDUI.DLL NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/RASDDUI.HLP" -o -d "/h/NTCSSS/spool/ntdrivers/RASDDUI.HLP" -o -p "/h/NTCSSS/spool/ntdrivers/RASDDUI.HLP" ]
then
    echo "spool/ntdrivers/RASDDUI.HLP"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/RASDDUI.HLP
else
    echo "/h/NTCSSS/spool/ntdrivers/RASDDUI.HLP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/PSCRIPT.HLP" -o -d "/h/NTCSSS/spool/ntdrivers/PSCRIPT.HLP" -o -p "/h/NTCSSS/spool/ntdrivers/PSCRIPT.HLP" ]
then
    echo "spool/ntdrivers/PSCRIPT.HLP"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/PSCRIPT.HLP
else
    echo "/h/NTCSSS/spool/ntdrivers/PSCRIPT.HLP NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/RASDD.DLL" -o -d "/h/NTCSSS/spool/ntdrivers/RASDD.DLL" -o -p "/h/NTCSSS/spool/ntdrivers/RASDD.DLL" ]
then
    echo "spool/ntdrivers/RASDD.DLL"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/RASDD.DLL
else
    echo "/h/NTCSSS/spool/ntdrivers/RASDD.DLL NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/PSCRIPT.DLL" -o -d "/h/NTCSSS/spool/ntdrivers/PSCRIPT.DLL" -o -p "/h/NTCSSS/spool/ntdrivers/PSCRIPT.DLL" ]
then
    echo "spool/ntdrivers/PSCRIPT.DLL"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/PSCRIPT.DLL
else
    echo "/h/NTCSSS/spool/ntdrivers/PSCRIPT.DLL NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/ntdrivers/PSCRPTUI.DLL" -o -d "/h/NTCSSS/spool/ntdrivers/PSCRPTUI.DLL" -o -p "/h/NTCSSS/spool/ntdrivers/PSCRPTUI.DLL" ]
then
    echo "spool/ntdrivers/PSCRPTUI.DLL"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/ntdrivers/PSCRPTUI.DLL
else
    echo "/h/NTCSSS/spool/ntdrivers/PSCRPTUI.DLL NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/pipes/" -o -d "/h/NTCSSS/spool/pipes/" -o -p "/h/NTCSSS/spool/pipes/" ]
then
    echo "spool/pipes/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/pipes/
else
    echo "/h/NTCSSS/spool/pipes/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/pipes/dm_input_pipe" -o -d "/h/NTCSSS/spool/pipes/dm_input_pipe" -o -p "/h/NTCSSS/spool/pipes/dm_input_pipe" ]
then
    echo "spool/pipes/dm_input_pipe"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/pipes/dm_input_pipe
else
    echo "/h/NTCSSS/spool/pipes/dm_input_pipe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/pipes/dm_ret_pipe" -o -d "/h/NTCSSS/spool/pipes/dm_ret_pipe" -o -p "/h/NTCSSS/spool/pipes/dm_ret_pipe" ]
then
    echo "spool/pipes/dm_ret_pipe"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/pipes/dm_ret_pipe
else
    echo "/h/NTCSSS/spool/pipes/dm_ret_pipe NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/lp/" -o -d "/h/NTCSSS/spool/lp/" -o -p "/h/NTCSSS/spool/lp/" ]
then
    echo "spool/lp/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/lp/
else
    echo "/h/NTCSSS/spool/lp/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/lp/PIPES/" -o -d "/h/NTCSSS/spool/lp/PIPES/" -o -p "/h/NTCSSS/spool/lp/PIPES/" ]
then
    echo "spool/lp/PIPES/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/lp/PIPES/
else
    echo "/h/NTCSSS/spool/lp/PIPES/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE" -o -d "/h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE" -o -p "/h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE" ]
then
    echo "spool/lp/NTCSS_LP_SEQUENCE_FILE"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE
else
    echo "/h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/etc/" -o -d "/h/NTCSSS/spool/etc/" -o -p "/h/NTCSSS/spool/etc/" ]
then
    echo "spool/etc/"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/etc/
else
    echo "/h/NTCSSS/spool/etc/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/etc/LPNODATA_MSG" -o -d "/h/NTCSSS/spool/etc/LPNODATA_MSG" -o -p "/h/NTCSSS/spool/etc/LPNODATA_MSG" ]
then
    echo "spool/etc/LPNODATA_MSG"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/etc/LPNODATA_MSG
else
    echo "/h/NTCSSS/spool/etc/LPNODATA_MSG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/spool/etc/LPNOFILE_MSG" -o -d "/h/NTCSSS/spool/etc/LPNOFILE_MSG" -o -p "/h/NTCSSS/spool/etc/LPNOFILE_MSG" ]
then
    echo "spool/etc/LPNOFILE_MSG"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/spool/etc/LPNOFILE_MSG
else
    echo "/h/NTCSSS/spool/etc/LPNOFILE_MSG NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/" -o -d "/h/NTCSSS/web/" -o -p "/h/NTCSSS/web/" ]
then
    echo "web/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/
else
    echo "/h/NTCSSS/web/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/" -o -d "/h/NTCSSS/web/bin/" -o -p "/h/NTCSSS/web/bin/" ]
then
    echo "web/bin/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/
else
    echo "/h/NTCSSS/web/bin/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/httpd" -o -d "/h/NTCSSS/web/bin/httpd" -o -p "/h/NTCSSS/web/bin/httpd" ]
then
    echo "web/bin/httpd"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/httpd
else
    echo "/h/NTCSSS/web/bin/httpd NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/startserver" -o -d "/h/NTCSSS/web/bin/startserver" -o -p "/h/NTCSSS/web/bin/startserver" ]
then
    echo "web/bin/startserver"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/startserver
else
    echo "/h/NTCSSS/web/bin/startserver NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/stopserver" -o -d "/h/NTCSSS/web/bin/stopserver" -o -p "/h/NTCSSS/web/bin/stopserver" ]
then
    echo "web/bin/stopserver"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/stopserver
else
    echo "/h/NTCSSS/web/bin/stopserver NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/startserver.sh" -o -d "/h/NTCSSS/web/bin/startserver.sh" -o -p "/h/NTCSSS/web/bin/startserver.sh" ]
then
    echo "web/bin/startserver.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/startserver.sh
else
    echo "/h/NTCSSS/web/bin/startserver.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/stopserver.sh" -o -d "/h/NTCSSS/web/bin/stopserver.sh" -o -p "/h/NTCSSS/web/bin/stopserver.sh" ]
then
    echo "web/bin/stopserver.sh"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/stopserver.sh
else
    echo "/h/NTCSSS/web/bin/stopserver.sh NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/bin/httpd.master" -o -d "/h/NTCSSS/web/bin/httpd.master" -o -p "/h/NTCSSS/web/bin/httpd.master" ]
then
    echo "web/bin/httpd.master"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/bin/httpd.master
else
    echo "/h/NTCSSS/web/bin/httpd.master NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/cgi-bin/" -o -d "/h/NTCSSS/web/cgi-bin/" -o -p "/h/NTCSSS/web/cgi-bin/" ]
then
    echo "web/cgi-bin/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/cgi-bin/
else
    echo "/h/NTCSSS/web/cgi-bin/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/" -o -d "/h/NTCSSS/web/conf/" -o -p "/h/NTCSSS/web/conf/" ]
then
    echo "web/conf/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/
else
    echo "/h/NTCSSS/web/conf/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/access.conf" -o -d "/h/NTCSSS/web/conf/access.conf" -o -p "/h/NTCSSS/web/conf/access.conf" ]
then
    echo "web/conf/access.conf"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/access.conf
else
    echo "/h/NTCSSS/web/conf/access.conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/httpd.conf" -o -d "/h/NTCSSS/web/conf/httpd.conf" -o -p "/h/NTCSSS/web/conf/httpd.conf" ]
then
    echo "web/conf/httpd.conf"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/httpd.conf
else
    echo "/h/NTCSSS/web/conf/httpd.conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/magic" -o -d "/h/NTCSSS/web/conf/magic" -o -p "/h/NTCSSS/web/conf/magic" ]
then
    echo "web/conf/magic"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/magic
else
    echo "/h/NTCSSS/web/conf/magic NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/mime.types" -o -d "/h/NTCSSS/web/conf/mime.types" -o -p "/h/NTCSSS/web/conf/mime.types" ]
then
    echo "web/conf/mime.types"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/mime.types
else
    echo "/h/NTCSSS/web/conf/mime.types NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/conf/srm.conf" -o -d "/h/NTCSSS/web/conf/srm.conf" -o -p "/h/NTCSSS/web/conf/srm.conf" ]
then
    echo "web/conf/srm.conf"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/conf/srm.conf
else
    echo "/h/NTCSSS/web/conf/srm.conf NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/" -o -d "/h/NTCSSS/web/icons/" -o -p "/h/NTCSSS/web/icons/" ]
then
    echo "web/icons/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/icons/
else
    echo "/h/NTCSSS/web/icons/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/README" -o -d "/h/NTCSSS/web/icons/README" -o -p "/h/NTCSSS/web/icons/README" ]
then
    echo "web/icons/README"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/README
else
    echo "/h/NTCSSS/web/icons/README NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/a.gif" -o -d "/h/NTCSSS/web/icons/a.gif" -o -p "/h/NTCSSS/web/icons/a.gif" ]
then
    echo "web/icons/a.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/a.gif
else
    echo "/h/NTCSSS/web/icons/a.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/alert.black.gif" -o -d "/h/NTCSSS/web/icons/alert.black.gif" -o -p "/h/NTCSSS/web/icons/alert.black.gif" ]
then
    echo "web/icons/alert.black.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/alert.black.gif
else
    echo "/h/NTCSSS/web/icons/alert.black.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/alert.red.gif" -o -d "/h/NTCSSS/web/icons/alert.red.gif" -o -p "/h/NTCSSS/web/icons/alert.red.gif" ]
then
    echo "web/icons/alert.red.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/alert.red.gif
else
    echo "/h/NTCSSS/web/icons/alert.red.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/apache_pb.gif" -o -d "/h/NTCSSS/web/icons/apache_pb.gif" -o -p "/h/NTCSSS/web/icons/apache_pb.gif" ]
then
    echo "web/icons/apache_pb.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/apache_pb.gif
else
    echo "/h/NTCSSS/web/icons/apache_pb.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/back.gif" -o -d "/h/NTCSSS/web/icons/back.gif" -o -p "/h/NTCSSS/web/icons/back.gif" ]
then
    echo "web/icons/back.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/back.gif
else
    echo "/h/NTCSSS/web/icons/back.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/ball.gray.gif" -o -d "/h/NTCSSS/web/icons/ball.gray.gif" -o -p "/h/NTCSSS/web/icons/ball.gray.gif" ]
then
    echo "web/icons/ball.gray.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/ball.gray.gif
else
    echo "/h/NTCSSS/web/icons/ball.gray.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/ball.red.gif" -o -d "/h/NTCSSS/web/icons/ball.red.gif" -o -p "/h/NTCSSS/web/icons/ball.red.gif" ]
then
    echo "web/icons/ball.red.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/ball.red.gif
else
    echo "/h/NTCSSS/web/icons/ball.red.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/binary.gif" -o -d "/h/NTCSSS/web/icons/binary.gif" -o -p "/h/NTCSSS/web/icons/binary.gif" ]
then
    echo "web/icons/binary.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/binary.gif
else
    echo "/h/NTCSSS/web/icons/binary.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/binhex.gif" -o -d "/h/NTCSSS/web/icons/binhex.gif" -o -p "/h/NTCSSS/web/icons/binhex.gif" ]
then
    echo "web/icons/binhex.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/binhex.gif
else
    echo "/h/NTCSSS/web/icons/binhex.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/blank.gif" -o -d "/h/NTCSSS/web/icons/blank.gif" -o -p "/h/NTCSSS/web/icons/blank.gif" ]
then
    echo "web/icons/blank.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/blank.gif
else
    echo "/h/NTCSSS/web/icons/blank.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/bomb.gif" -o -d "/h/NTCSSS/web/icons/bomb.gif" -o -p "/h/NTCSSS/web/icons/bomb.gif" ]
then
    echo "web/icons/bomb.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/bomb.gif
else
    echo "/h/NTCSSS/web/icons/bomb.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/box1.gif" -o -d "/h/NTCSSS/web/icons/box1.gif" -o -p "/h/NTCSSS/web/icons/box1.gif" ]
then
    echo "web/icons/box1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/box1.gif
else
    echo "/h/NTCSSS/web/icons/box1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/box2.gif" -o -d "/h/NTCSSS/web/icons/box2.gif" -o -p "/h/NTCSSS/web/icons/box2.gif" ]
then
    echo "web/icons/box2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/box2.gif
else
    echo "/h/NTCSSS/web/icons/box2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/broken.gif" -o -d "/h/NTCSSS/web/icons/broken.gif" -o -p "/h/NTCSSS/web/icons/broken.gif" ]
then
    echo "web/icons/broken.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/broken.gif
else
    echo "/h/NTCSSS/web/icons/broken.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/burst.gif" -o -d "/h/NTCSSS/web/icons/burst.gif" -o -p "/h/NTCSSS/web/icons/burst.gif" ]
then
    echo "web/icons/burst.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/burst.gif
else
    echo "/h/NTCSSS/web/icons/burst.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/c.gif" -o -d "/h/NTCSSS/web/icons/c.gif" -o -p "/h/NTCSSS/web/icons/c.gif" ]
then
    echo "web/icons/c.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/c.gif
else
    echo "/h/NTCSSS/web/icons/c.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/comp.blue.gif" -o -d "/h/NTCSSS/web/icons/comp.blue.gif" -o -p "/h/NTCSSS/web/icons/comp.blue.gif" ]
then
    echo "web/icons/comp.blue.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/comp.blue.gif
else
    echo "/h/NTCSSS/web/icons/comp.blue.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/comp.gray.gif" -o -d "/h/NTCSSS/web/icons/comp.gray.gif" -o -p "/h/NTCSSS/web/icons/comp.gray.gif" ]
then
    echo "web/icons/comp.gray.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/comp.gray.gif
else
    echo "/h/NTCSSS/web/icons/comp.gray.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/compressed.gif" -o -d "/h/NTCSSS/web/icons/compressed.gif" -o -p "/h/NTCSSS/web/icons/compressed.gif" ]
then
    echo "web/icons/compressed.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/compressed.gif
else
    echo "/h/NTCSSS/web/icons/compressed.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/continued.gif" -o -d "/h/NTCSSS/web/icons/continued.gif" -o -p "/h/NTCSSS/web/icons/continued.gif" ]
then
    echo "web/icons/continued.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/continued.gif
else
    echo "/h/NTCSSS/web/icons/continued.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/dir.gif" -o -d "/h/NTCSSS/web/icons/dir.gif" -o -p "/h/NTCSSS/web/icons/dir.gif" ]
then
    echo "web/icons/dir.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/dir.gif
else
    echo "/h/NTCSSS/web/icons/dir.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/down.gif" -o -d "/h/NTCSSS/web/icons/down.gif" -o -p "/h/NTCSSS/web/icons/down.gif" ]
then
    echo "web/icons/down.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/down.gif
else
    echo "/h/NTCSSS/web/icons/down.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/dvi.gif" -o -d "/h/NTCSSS/web/icons/dvi.gif" -o -p "/h/NTCSSS/web/icons/dvi.gif" ]
then
    echo "web/icons/dvi.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/dvi.gif
else
    echo "/h/NTCSSS/web/icons/dvi.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/f.gif" -o -d "/h/NTCSSS/web/icons/f.gif" -o -p "/h/NTCSSS/web/icons/f.gif" ]
then
    echo "web/icons/f.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/f.gif
else
    echo "/h/NTCSSS/web/icons/f.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/folder.gif" -o -d "/h/NTCSSS/web/icons/folder.gif" -o -p "/h/NTCSSS/web/icons/folder.gif" ]
then
    echo "web/icons/folder.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/folder.gif
else
    echo "/h/NTCSSS/web/icons/folder.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/folder.open.gif" -o -d "/h/NTCSSS/web/icons/folder.open.gif" -o -p "/h/NTCSSS/web/icons/folder.open.gif" ]
then
    echo "web/icons/folder.open.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/folder.open.gif
else
    echo "/h/NTCSSS/web/icons/folder.open.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/folder.sec.gif" -o -d "/h/NTCSSS/web/icons/folder.sec.gif" -o -p "/h/NTCSSS/web/icons/folder.sec.gif" ]
then
    echo "web/icons/folder.sec.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/folder.sec.gif
else
    echo "/h/NTCSSS/web/icons/folder.sec.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/forward.gif" -o -d "/h/NTCSSS/web/icons/forward.gif" -o -p "/h/NTCSSS/web/icons/forward.gif" ]
then
    echo "web/icons/forward.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/forward.gif
else
    echo "/h/NTCSSS/web/icons/forward.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/generic.gif" -o -d "/h/NTCSSS/web/icons/generic.gif" -o -p "/h/NTCSSS/web/icons/generic.gif" ]
then
    echo "web/icons/generic.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/generic.gif
else
    echo "/h/NTCSSS/web/icons/generic.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/generic.red.gif" -o -d "/h/NTCSSS/web/icons/generic.red.gif" -o -p "/h/NTCSSS/web/icons/generic.red.gif" ]
then
    echo "web/icons/generic.red.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/generic.red.gif
else
    echo "/h/NTCSSS/web/icons/generic.red.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/generic.sec.gif" -o -d "/h/NTCSSS/web/icons/generic.sec.gif" -o -p "/h/NTCSSS/web/icons/generic.sec.gif" ]
then
    echo "web/icons/generic.sec.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/generic.sec.gif
else
    echo "/h/NTCSSS/web/icons/generic.sec.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/hand.right.gif" -o -d "/h/NTCSSS/web/icons/hand.right.gif" -o -p "/h/NTCSSS/web/icons/hand.right.gif" ]
then
    echo "web/icons/hand.right.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/hand.right.gif
else
    echo "/h/NTCSSS/web/icons/hand.right.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/hand.up.gif" -o -d "/h/NTCSSS/web/icons/hand.up.gif" -o -p "/h/NTCSSS/web/icons/hand.up.gif" ]
then
    echo "web/icons/hand.up.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/hand.up.gif
else
    echo "/h/NTCSSS/web/icons/hand.up.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/icon.sheet.gif" -o -d "/h/NTCSSS/web/icons/icon.sheet.gif" -o -p "/h/NTCSSS/web/icons/icon.sheet.gif" ]
then
    echo "web/icons/icon.sheet.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/icon.sheet.gif
else
    echo "/h/NTCSSS/web/icons/icon.sheet.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/image1.gif" -o -d "/h/NTCSSS/web/icons/image1.gif" -o -p "/h/NTCSSS/web/icons/image1.gif" ]
then
    echo "web/icons/image1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/image1.gif
else
    echo "/h/NTCSSS/web/icons/image1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/image2.gif" -o -d "/h/NTCSSS/web/icons/image2.gif" -o -p "/h/NTCSSS/web/icons/image2.gif" ]
then
    echo "web/icons/image2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/image2.gif
else
    echo "/h/NTCSSS/web/icons/image2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/image3.gif" -o -d "/h/NTCSSS/web/icons/image3.gif" -o -p "/h/NTCSSS/web/icons/image3.gif" ]
then
    echo "web/icons/image3.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/image3.gif
else
    echo "/h/NTCSSS/web/icons/image3.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/index.gif" -o -d "/h/NTCSSS/web/icons/index.gif" -o -p "/h/NTCSSS/web/icons/index.gif" ]
then
    echo "web/icons/index.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/index.gif
else
    echo "/h/NTCSSS/web/icons/index.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/layout.gif" -o -d "/h/NTCSSS/web/icons/layout.gif" -o -p "/h/NTCSSS/web/icons/layout.gif" ]
then
    echo "web/icons/layout.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/layout.gif
else
    echo "/h/NTCSSS/web/icons/layout.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/left.gif" -o -d "/h/NTCSSS/web/icons/left.gif" -o -p "/h/NTCSSS/web/icons/left.gif" ]
then
    echo "web/icons/left.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/left.gif
else
    echo "/h/NTCSSS/web/icons/left.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/link.gif" -o -d "/h/NTCSSS/web/icons/link.gif" -o -p "/h/NTCSSS/web/icons/link.gif" ]
then
    echo "web/icons/link.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/link.gif
else
    echo "/h/NTCSSS/web/icons/link.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/movie.gif" -o -d "/h/NTCSSS/web/icons/movie.gif" -o -p "/h/NTCSSS/web/icons/movie.gif" ]
then
    echo "web/icons/movie.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/movie.gif
else
    echo "/h/NTCSSS/web/icons/movie.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/p.gif" -o -d "/h/NTCSSS/web/icons/p.gif" -o -p "/h/NTCSSS/web/icons/p.gif" ]
then
    echo "web/icons/p.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/p.gif
else
    echo "/h/NTCSSS/web/icons/p.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/patch.gif" -o -d "/h/NTCSSS/web/icons/patch.gif" -o -p "/h/NTCSSS/web/icons/patch.gif" ]
then
    echo "web/icons/patch.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/patch.gif
else
    echo "/h/NTCSSS/web/icons/patch.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pdf.gif" -o -d "/h/NTCSSS/web/icons/pdf.gif" -o -p "/h/NTCSSS/web/icons/pdf.gif" ]
then
    echo "web/icons/pdf.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pdf.gif
else
    echo "/h/NTCSSS/web/icons/pdf.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie0.gif" -o -d "/h/NTCSSS/web/icons/pie0.gif" -o -p "/h/NTCSSS/web/icons/pie0.gif" ]
then
    echo "web/icons/pie0.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie0.gif
else
    echo "/h/NTCSSS/web/icons/pie0.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie1.gif" -o -d "/h/NTCSSS/web/icons/pie1.gif" -o -p "/h/NTCSSS/web/icons/pie1.gif" ]
then
    echo "web/icons/pie1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie1.gif
else
    echo "/h/NTCSSS/web/icons/pie1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie2.gif" -o -d "/h/NTCSSS/web/icons/pie2.gif" -o -p "/h/NTCSSS/web/icons/pie2.gif" ]
then
    echo "web/icons/pie2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie2.gif
else
    echo "/h/NTCSSS/web/icons/pie2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie3.gif" -o -d "/h/NTCSSS/web/icons/pie3.gif" -o -p "/h/NTCSSS/web/icons/pie3.gif" ]
then
    echo "web/icons/pie3.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie3.gif
else
    echo "/h/NTCSSS/web/icons/pie3.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie4.gif" -o -d "/h/NTCSSS/web/icons/pie4.gif" -o -p "/h/NTCSSS/web/icons/pie4.gif" ]
then
    echo "web/icons/pie4.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie4.gif
else
    echo "/h/NTCSSS/web/icons/pie4.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie5.gif" -o -d "/h/NTCSSS/web/icons/pie5.gif" -o -p "/h/NTCSSS/web/icons/pie5.gif" ]
then
    echo "web/icons/pie5.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie5.gif
else
    echo "/h/NTCSSS/web/icons/pie5.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie6.gif" -o -d "/h/NTCSSS/web/icons/pie6.gif" -o -p "/h/NTCSSS/web/icons/pie6.gif" ]
then
    echo "web/icons/pie6.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie6.gif
else
    echo "/h/NTCSSS/web/icons/pie6.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie7.gif" -o -d "/h/NTCSSS/web/icons/pie7.gif" -o -p "/h/NTCSSS/web/icons/pie7.gif" ]
then
    echo "web/icons/pie7.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie7.gif
else
    echo "/h/NTCSSS/web/icons/pie7.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/pie8.gif" -o -d "/h/NTCSSS/web/icons/pie8.gif" -o -p "/h/NTCSSS/web/icons/pie8.gif" ]
then
    echo "web/icons/pie8.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/pie8.gif
else
    echo "/h/NTCSSS/web/icons/pie8.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/portal.gif" -o -d "/h/NTCSSS/web/icons/portal.gif" -o -p "/h/NTCSSS/web/icons/portal.gif" ]
then
    echo "web/icons/portal.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/portal.gif
else
    echo "/h/NTCSSS/web/icons/portal.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/ps.gif" -o -d "/h/NTCSSS/web/icons/ps.gif" -o -p "/h/NTCSSS/web/icons/ps.gif" ]
then
    echo "web/icons/ps.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/ps.gif
else
    echo "/h/NTCSSS/web/icons/ps.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/quill.gif" -o -d "/h/NTCSSS/web/icons/quill.gif" -o -p "/h/NTCSSS/web/icons/quill.gif" ]
then
    echo "web/icons/quill.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/quill.gif
else
    echo "/h/NTCSSS/web/icons/quill.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/right.gif" -o -d "/h/NTCSSS/web/icons/right.gif" -o -p "/h/NTCSSS/web/icons/right.gif" ]
then
    echo "web/icons/right.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/right.gif
else
    echo "/h/NTCSSS/web/icons/right.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/screw1.gif" -o -d "/h/NTCSSS/web/icons/screw1.gif" -o -p "/h/NTCSSS/web/icons/screw1.gif" ]
then
    echo "web/icons/screw1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/screw1.gif
else
    echo "/h/NTCSSS/web/icons/screw1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/screw2.gif" -o -d "/h/NTCSSS/web/icons/screw2.gif" -o -p "/h/NTCSSS/web/icons/screw2.gif" ]
then
    echo "web/icons/screw2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/screw2.gif
else
    echo "/h/NTCSSS/web/icons/screw2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/script.gif" -o -d "/h/NTCSSS/web/icons/script.gif" -o -p "/h/NTCSSS/web/icons/script.gif" ]
then
    echo "web/icons/script.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/script.gif
else
    echo "/h/NTCSSS/web/icons/script.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/sound1.gif" -o -d "/h/NTCSSS/web/icons/sound1.gif" -o -p "/h/NTCSSS/web/icons/sound1.gif" ]
then
    echo "web/icons/sound1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/sound1.gif
else
    echo "/h/NTCSSS/web/icons/sound1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/sound2.gif" -o -d "/h/NTCSSS/web/icons/sound2.gif" -o -p "/h/NTCSSS/web/icons/sound2.gif" ]
then
    echo "web/icons/sound2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/sound2.gif
else
    echo "/h/NTCSSS/web/icons/sound2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/sphere1.gif" -o -d "/h/NTCSSS/web/icons/sphere1.gif" -o -p "/h/NTCSSS/web/icons/sphere1.gif" ]
then
    echo "web/icons/sphere1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/sphere1.gif
else
    echo "/h/NTCSSS/web/icons/sphere1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/sphere2.gif" -o -d "/h/NTCSSS/web/icons/sphere2.gif" -o -p "/h/NTCSSS/web/icons/sphere2.gif" ]
then
    echo "web/icons/sphere2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/sphere2.gif
else
    echo "/h/NTCSSS/web/icons/sphere2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/tar.gif" -o -d "/h/NTCSSS/web/icons/tar.gif" -o -p "/h/NTCSSS/web/icons/tar.gif" ]
then
    echo "web/icons/tar.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/tar.gif
else
    echo "/h/NTCSSS/web/icons/tar.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/tex.gif" -o -d "/h/NTCSSS/web/icons/tex.gif" -o -p "/h/NTCSSS/web/icons/tex.gif" ]
then
    echo "web/icons/tex.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/tex.gif
else
    echo "/h/NTCSSS/web/icons/tex.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/text.gif" -o -d "/h/NTCSSS/web/icons/text.gif" -o -p "/h/NTCSSS/web/icons/text.gif" ]
then
    echo "web/icons/text.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/text.gif
else
    echo "/h/NTCSSS/web/icons/text.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/transfer.gif" -o -d "/h/NTCSSS/web/icons/transfer.gif" -o -p "/h/NTCSSS/web/icons/transfer.gif" ]
then
    echo "web/icons/transfer.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/transfer.gif
else
    echo "/h/NTCSSS/web/icons/transfer.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/unknown.gif" -o -d "/h/NTCSSS/web/icons/unknown.gif" -o -p "/h/NTCSSS/web/icons/unknown.gif" ]
then
    echo "web/icons/unknown.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/unknown.gif
else
    echo "/h/NTCSSS/web/icons/unknown.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/up.gif" -o -d "/h/NTCSSS/web/icons/up.gif" -o -p "/h/NTCSSS/web/icons/up.gif" ]
then
    echo "web/icons/up.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/up.gif
else
    echo "/h/NTCSSS/web/icons/up.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/uu.gif" -o -d "/h/NTCSSS/web/icons/uu.gif" -o -p "/h/NTCSSS/web/icons/uu.gif" ]
then
    echo "web/icons/uu.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/uu.gif
else
    echo "/h/NTCSSS/web/icons/uu.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/uuencoded.gif" -o -d "/h/NTCSSS/web/icons/uuencoded.gif" -o -p "/h/NTCSSS/web/icons/uuencoded.gif" ]
then
    echo "web/icons/uuencoded.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/uuencoded.gif
else
    echo "/h/NTCSSS/web/icons/uuencoded.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/world1.gif" -o -d "/h/NTCSSS/web/icons/world1.gif" -o -p "/h/NTCSSS/web/icons/world1.gif" ]
then
    echo "web/icons/world1.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/world1.gif
else
    echo "/h/NTCSSS/web/icons/world1.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/icons/world2.gif" -o -d "/h/NTCSSS/web/icons/world2.gif" -o -p "/h/NTCSSS/web/icons/world2.gif" ]
then
    echo "web/icons/world2.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/icons/world2.gif
else
    echo "/h/NTCSSS/web/icons/world2.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/" -o -d "/h/NTCSSS/web/logs/" -o -p "/h/NTCSSS/web/logs/" ]
then
    echo "web/logs/"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/
else
    echo "/h/NTCSSS/web/logs/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/access_log" -o -d "/h/NTCSSS/web/logs/access_log" -o -p "/h/NTCSSS/web/logs/access_log" ]
then
    echo "web/logs/access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/access_log
else
    echo "/h/NTCSSS/web/logs/access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/error_log" -o -d "/h/NTCSSS/web/logs/error_log" -o -p "/h/NTCSSS/web/logs/error_log" ]
then
    echo "web/logs/error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/error_log
else
    echo "/h/NTCSSS/web/logs/error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/httpd.pid" -o -d "/h/NTCSSS/web/logs/httpd.pid" -o -p "/h/NTCSSS/web/logs/httpd.pid" ]
then
    echo "web/logs/httpd.pid"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/httpd.pid
else
    echo "/h/NTCSSS/web/logs/httpd.pid NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ommsng-error_log" -o -d "/h/NTCSSS/web/logs/ommsng-error_log" -o -p "/h/NTCSSS/web/logs/ommsng-error_log" ]
then
    echo "web/logs/ommsng-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ommsng-error_log
else
    echo "/h/NTCSSS/web/logs/ommsng-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ommsng-access_log" -o -d "/h/NTCSSS/web/logs/ommsng-access_log" -o -p "/h/NTCSSS/web/logs/ommsng-access_log" ]
then
    echo "web/logs/ommsng-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ommsng-access_log
else
    echo "/h/NTCSSS/web/logs/ommsng-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss-error_log" -o -d "/h/NTCSSS/web/logs/ntcss-error_log" -o -p "/h/NTCSSS/web/logs/ntcss-error_log" ]
then
    echo "web/logs/ntcss-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss-error_log
else
    echo "/h/NTCSSS/web/logs/ntcss-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss-access_log" -o -d "/h/NTCSSS/web/logs/ntcss-access_log" -o -p "/h/NTCSSS/web/logs/ntcss-access_log" ]
then
    echo "web/logs/ntcss-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss-access_log
else
    echo "/h/NTCSSS/web/logs/ntcss-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_utils-error_log" -o -d "/h/NTCSSS/web/logs/ntcss_utils-error_log" -o -p "/h/NTCSSS/web/logs/ntcss_utils-error_log" ]
then
    echo "web/logs/ntcss_utils-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_utils-error_log
else
    echo "/h/NTCSSS/web/logs/ntcss_utils-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/hpatlass-error_log" -o -d "/h/NTCSSS/web/logs/hpatlass-error_log" -o -p "/h/NTCSSS/web/logs/hpatlass-error_log" ]
then
    echo "web/logs/hpatlass-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/hpatlass-error_log
else
    echo "/h/NTCSSS/web/logs/hpatlass-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_os-error_log" -o -d "/h/NTCSSS/web/logs/ntcss_os-error_log" -o -p "/h/NTCSSS/web/logs/ntcss_os-error_log" ]
then
    echo "web/logs/ntcss_os-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_os-error_log
else
    echo "/h/NTCSSS/web/logs/ntcss_os-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_app-error_log" -o -d "/h/NTCSSS/web/logs/ntcss_app-error_log" -o -p "/h/NTCSSS/web/logs/ntcss_app-error_log" ]
then
    echo "web/logs/ntcss_app-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_app-error_log
else
    echo "/h/NTCSSS/web/logs/ntcss_app-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/fleet_support-error_log" -o -d "/h/NTCSSS/web/logs/fleet_support-error_log" -o -p "/h/NTCSSS/web/logs/fleet_support-error_log" ]
then
    echo "web/logs/fleet_support-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/fleet_support-error_log
else
    echo "/h/NTCSSS/web/logs/fleet_support-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/cm-error_log" -o -d "/h/NTCSSS/web/logs/cm-error_log" -o -p "/h/NTCSSS/web/logs/cm-error_log" ]
then
    echo "web/logs/cm-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/cm-error_log
else
    echo "/h/NTCSSS/web/logs/cm-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/adhoc-error_log" -o -d "/h/NTCSSS/web/logs/adhoc-error_log" -o -p "/h/NTCSSS/web/logs/adhoc-error_log" ]
then
    echo "web/logs/adhoc-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/adhoc-error_log
else
    echo "/h/NTCSSS/web/logs/adhoc-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/rsupply-error_log" -o -d "/h/NTCSSS/web/logs/rsupply-error_log" -o -p "/h/NTCSSS/web/logs/rsupply-error_log" ]
then
    echo "web/logs/rsupply-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/rsupply-error_log
else
    echo "/h/NTCSSS/web/logs/rsupply-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/nalcomis-error_log" -o -d "/h/NTCSSS/web/logs/nalcomis-error_log" -o -p "/h/NTCSSS/web/logs/nalcomis-error_log" ]
then
    echo "web/logs/nalcomis-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/nalcomis-error_log
else
    echo "/h/NTCSSS/web/logs/nalcomis-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/radm-error_log" -o -d "/h/NTCSSS/web/logs/radm-error_log" -o -p "/h/NTCSSS/web/logs/radm-error_log" ]
then
    echo "web/logs/radm-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/radm-error_log
else
    echo "/h/NTCSSS/web/logs/radm-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcssii-error_log" -o -d "/h/NTCSSS/web/logs/ntcssii-error_log" -o -p "/h/NTCSSS/web/logs/ntcssii-error_log" ]
then
    echo "web/logs/ntcssii-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcssii-error_log
else
    echo "/h/NTCSSS/web/logs/ntcssii-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_utils-access_log" -o -d "/h/NTCSSS/web/logs/ntcss_utils-access_log" -o -p "/h/NTCSSS/web/logs/ntcss_utils-access_log" ]
then
    echo "web/logs/ntcss_utils-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_utils-access_log
else
    echo "/h/NTCSSS/web/logs/ntcss_utils-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/hpatlass-access_log" -o -d "/h/NTCSSS/web/logs/hpatlass-access_log" -o -p "/h/NTCSSS/web/logs/hpatlass-access_log" ]
then
    echo "web/logs/hpatlass-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/hpatlass-access_log
else
    echo "/h/NTCSSS/web/logs/hpatlass-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_os-access_log" -o -d "/h/NTCSSS/web/logs/ntcss_os-access_log" -o -p "/h/NTCSSS/web/logs/ntcss_os-access_log" ]
then
    echo "web/logs/ntcss_os-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_os-access_log
else
    echo "/h/NTCSSS/web/logs/ntcss_os-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcss_app-access_log" -o -d "/h/NTCSSS/web/logs/ntcss_app-access_log" -o -p "/h/NTCSSS/web/logs/ntcss_app-access_log" ]
then
    echo "web/logs/ntcss_app-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcss_app-access_log
else
    echo "/h/NTCSSS/web/logs/ntcss_app-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/fleet_support-access_log" -o -d "/h/NTCSSS/web/logs/fleet_support-access_log" -o -p "/h/NTCSSS/web/logs/fleet_support-access_log" ]
then
    echo "web/logs/fleet_support-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/fleet_support-access_log
else
    echo "/h/NTCSSS/web/logs/fleet_support-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/cm-access_log" -o -d "/h/NTCSSS/web/logs/cm-access_log" -o -p "/h/NTCSSS/web/logs/cm-access_log" ]
then
    echo "web/logs/cm-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/cm-access_log
else
    echo "/h/NTCSSS/web/logs/cm-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/adhoc-access_log" -o -d "/h/NTCSSS/web/logs/adhoc-access_log" -o -p "/h/NTCSSS/web/logs/adhoc-access_log" ]
then
    echo "web/logs/adhoc-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/adhoc-access_log
else
    echo "/h/NTCSSS/web/logs/adhoc-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/rsupply-access_log" -o -d "/h/NTCSSS/web/logs/rsupply-access_log" -o -p "/h/NTCSSS/web/logs/rsupply-access_log" ]
then
    echo "web/logs/rsupply-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/rsupply-access_log
else
    echo "/h/NTCSSS/web/logs/rsupply-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/nalcomis-access_log" -o -d "/h/NTCSSS/web/logs/nalcomis-access_log" -o -p "/h/NTCSSS/web/logs/nalcomis-access_log" ]
then
    echo "web/logs/nalcomis-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/nalcomis-access_log
else
    echo "/h/NTCSSS/web/logs/nalcomis-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/radm-access_log" -o -d "/h/NTCSSS/web/logs/radm-access_log" -o -p "/h/NTCSSS/web/logs/radm-access_log" ]
then
    echo "web/logs/radm-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/radm-access_log
else
    echo "/h/NTCSSS/web/logs/radm-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/ntcssii-access_log" -o -d "/h/NTCSSS/web/logs/ntcssii-access_log" -o -p "/h/NTCSSS/web/logs/ntcssii-access_log" ]
then
    echo "web/logs/ntcssii-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/ntcssii-access_log
else
    echo "/h/NTCSSS/web/logs/ntcssii-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/atlass-error_log" -o -d "/h/NTCSSS/web/logs/atlass-error_log" -o -p "/h/NTCSSS/web/logs/atlass-error_log" ]
then
    echo "web/logs/atlass-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/atlass-error_log
else
    echo "/h/NTCSSS/web/logs/atlass-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/atlass-access_log" -o -d "/h/NTCSSS/web/logs/atlass-access_log" -o -p "/h/NTCSSS/web/logs/atlass-access_log" ]
then
    echo "web/logs/atlass-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/atlass-access_log
else
    echo "/h/NTCSSS/web/logs/atlass-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/sybase-error_log" -o -d "/h/NTCSSS/web/logs/sybase-error_log" -o -p "/h/NTCSSS/web/logs/sybase-error_log" ]
then
    echo "web/logs/sybase-error_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/sybase-error_log
else
    echo "/h/NTCSSS/web/logs/sybase-error_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/logs/sybase-access_log" -o -d "/h/NTCSSS/web/logs/sybase-access_log" -o -p "/h/NTCSSS/web/logs/sybase-access_log" ]
then
    echo "web/logs/sybase-access_log"
    chmod u+r,u+w,u+x,g-r,g-w,g-x,o-r,o-w,o-x /h/NTCSSS/web/logs/sybase-access_log
else
    echo "/h/NTCSSS/web/logs/sybase-access_log NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/.cshrc" -o -d "/h/NTCSSS/web/.cshrc" -o -p "/h/NTCSSS/web/.cshrc" ]
then
    echo "web/.cshrc"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/web/.cshrc
else
    echo "/h/NTCSSS/web/.cshrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/.login" -o -d "/h/NTCSSS/web/.login" -o -p "/h/NTCSSS/web/.login" ]
then
    echo "web/.login"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/web/.login
else
    echo "/h/NTCSSS/web/.login NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/.mailrc" -o -d "/h/NTCSSS/web/.mailrc" -o -p "/h/NTCSSS/web/.mailrc" ]
then
    echo "web/.mailrc"
    chmod u+r,u+w,u+x,g+r,g+w,g+x,o+r,o+w,o+x /h/NTCSSS/web/.mailrc
else
    echo "/h/NTCSSS/web/.mailrc NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/perl" -o -d "/h/NTCSSS/web/perl" -o -p "/h/NTCSSS/web/perl" ]
then
    echo "web/perl"
    chmod u+r,u-w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/perl
else
    echo "/h/NTCSSS/web/perl NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/LICENSE" -o -d "/h/NTCSSS/web/LICENSE" -o -p "/h/NTCSSS/web/LICENSE" ]
then
    echo "web/LICENSE"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/LICENSE
else
    echo "/h/NTCSSS/web/LICENSE NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/" -o -d "/h/NTCSSS/web/DocumentRoot/" -o -p "/h/NTCSSS/web/DocumentRoot/" ]
then
    echo "web/DocumentRoot/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/
else
    echo "/h/NTCSSS/web/DocumentRoot/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/RADM/" -o -d "/h/NTCSSS/web/DocumentRoot/RADM/" -o -p "/h/NTCSSS/web/DocumentRoot/RADM/" ]
then
    echo "web/DocumentRoot/RADM/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/RADM/
else
    echo "/h/NTCSSS/web/DocumentRoot/RADM/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/RSUPPLY/" -o -d "/h/NTCSSS/web/DocumentRoot/RSUPPLY/" -o -p "/h/NTCSSS/web/DocumentRoot/RSUPPLY/" ]
then
    echo "web/DocumentRoot/RSUPPLY/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/RSUPPLY/
else
    echo "/h/NTCSSS/web/DocumentRoot/RSUPPLY/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/" ]
then
    echo "web/DocumentRoot/NTCSSII/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/NTCSSII/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/home.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/index.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/images/sub.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/LICENSE NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/TODO NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/bind.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/cgi_path.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/content-negotiation.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/custom-error.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/dns-caveats.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/env.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/footer.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/handler.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/header.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/API.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/FAQ.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/client_block_api.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/compat_notes.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/descriptors.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/fin_wait_2.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/footer.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/header.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/howto.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/index.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_bugs.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/known_client_problems.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/nopgp.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-bsd44.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-dec.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-hp.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf-tuning.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/perf.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/security_tips.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/vif-info.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/misc/windoz_keepalive.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/index.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/install_1_1.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/invoking.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/keepalive.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/location.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/man-template.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/core.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directive-dict.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/directives.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/footer.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/header.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/index.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_access.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_actions.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_alias.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_asis.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_anon.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_db.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_dbm.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_auth_msql.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_autoindex.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_browser.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cern_meta.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cgi.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_cookies.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_digest.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dir.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dld.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_dll.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_env.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_example.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_expires.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_headers.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_imap.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_include.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_info.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_isapi.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_agent.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_common.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_config.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_log_referer.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_mime_magic.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_proxy.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_negotiation.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_rewrite.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_setenvif.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_speling.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_status.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_unique_id.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_userdir.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/mod/mod_usertrack.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/details_1_2.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/examples.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/fd-limits.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/footer.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/header.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/host.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/index.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/ip-based.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/name-based.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/vhosts-in-depth.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/vhosts/virtual-host.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/multilogs.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_0.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_1.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_2.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/new_features_1_3.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/process-model.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sections.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/sourcereorg.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/stopping.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/suexec.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/unixware.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/upgrading_to_1_3.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/manual/windows.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/apache_pb.gif NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/index.html" -o -d "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/index.html" -o -p "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/index.html" ]
then
    echo "web/DocumentRoot/NTCSSII/reports/htdocs/index.html"
    chmod u+r,u+w,u-x,g+r,g-w,g-x,o+r,o-w,o-x /h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/index.html
else
    echo "/h/NTCSSS/web/DocumentRoot/NTCSSII/reports/htdocs/index.html NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/ATLASS/" -o -d "/h/NTCSSS/web/DocumentRoot/ATLASS/" -o -p "/h/NTCSSS/web/DocumentRoot/ATLASS/" ]
then
    echo "web/DocumentRoot/ATLASS/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/ATLASS/
else
    echo "/h/NTCSSS/web/DocumentRoot/ATLASS/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/NALCOMIS/" -o -d "/h/NTCSSS/web/DocumentRoot/NALCOMIS/" -o -p "/h/NTCSSS/web/DocumentRoot/NALCOMIS/" ]
then
    echo "web/DocumentRoot/NALCOMIS/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/NALCOMIS/
else
    echo "/h/NTCSSS/web/DocumentRoot/NALCOMIS/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/OMMSNG/" -o -d "/h/NTCSSS/web/DocumentRoot/OMMSNG/" -o -p "/h/NTCSSS/web/DocumentRoot/OMMSNG/" ]
then
    echo "web/DocumentRoot/OMMSNG/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/OMMSNG/
else
    echo "/h/NTCSSS/web/DocumentRoot/OMMSNG/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ -f "/h/NTCSSS/web/DocumentRoot/ADHOC/" -o -d "/h/NTCSSS/web/DocumentRoot/ADHOC/" -o -p "/h/NTCSSS/web/DocumentRoot/ADHOC/" ]
then
    echo "web/DocumentRoot/ADHOC/"
    chmod u+r,u+w,u+x,g+r,g-w,g+x,o+r,o-w,o+x /h/NTCSSS/web/DocumentRoot/ADHOC/
else
    echo "/h/NTCSSS/web/DocumentRoot/ADHOC/ NO LONGER EXISTS !!" >> $spl
    missing_files="y"
fi
if [ $missing_files = "y" ]
then
    echo " "
    echo " "
    echo "    There were missing files!, chk set_perms.log for details.."
    echo " "
    echo " "
else
    echo "Set permissions complete with no missing files."
    echo "Set permissions complete with no missing files." >> $spl
fi
