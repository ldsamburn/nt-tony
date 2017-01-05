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

# Take a snapshot of the directory defined by arg-1 and create a
# 'set_perms.sh' in the same directory.
# The set_perms.sh can then be run to re-constitute the permissions.

    if [ "$1" = "?" -o "$1" = "help" -o "$1" = "-help" ]
    then
        echo "Usage:"
        echo " ";echo " "
        echo "get_perms.sh { help | snapshot | (no-args) }"
        echo " "
        echo "  where: help ........ this help message"
        echo "         snapshot .... force creation of new set_perms.sh script"
        echo "                       In this case, the second argument must be"
        echo "                       the directory to snapshot."
        echo "         no-args ..... use old tar file when"
        echo "                       creating set_perms.sh using pre-existing"
        echo "                       'tarlisting' file in the directory"
        echo "                       defined by arg-1"
        echo " ";echo " "
        exit 0
    fi

    perms_sh_path="$1/set_perms.sh"
    list_file="$1/listing_file"
    curdir=`pwd`

    if [ "$1" = "snapshot" ]
    then
        if [ -z "$2" ]
        then
            echo " ";echo " "
            echo "When argument 1 is the word 'snapshot', the second"
            echo "argument must be directory to use as snapshot original.."
            echo " ";echo " "
            exit 1
        fi
        if [ ! -d "$2" ]
        then
            echo " ";echo " "
            echo "The directory specified by argument 2 does not exist..."
            echo " ";echo " "
        fi
        model_dir=$2
        cd "$2"
        perms_sh_path="$2/set_perms.sh"
        list_file="$2/listing_file"
        echo " ";echo " ";echo " "
        echo "A snapshot of $2 will be created."
        echo "Strike any key to continue..."
        echo " ";echo " ";echo " "
        read ans
        if [ "$2" = "/h/NTCSSS" ]
        then
            tar cvf /tmp/perms.tar /etc/ntcss_system_settings.ini *
        else
            tar cvf /tmp/perms.tar *
        fi
        tar tvf /tmp/perms.tar > $2/tarlisting
        rm /tmp/perms.tar
        echo " ";echo " ";echo " "
    else
       #First argument is not 'snapshot, must be directory.'
       if [ -z "$1" ]
       then
           #First argument does not exist.
           echo " ";echo " "
           echo "Argument 1 must be the directory to use as a model"
           echo "for permissions -- or the word 'snapshot' which forces"
           echo "a new tar file listing to be generated.  In that case,"
           echo "the second argument must be the directory to use as"
           echo "the model."
           echo " ";echo " "
           exit 1
       else
           #First argument exists, chk for directry.
           if [ ! -d "$1" ]
           then
              echo " ";echo " "
              echo "The directory specified by argument 1 does not exist..."
              echo " ";echo " "
              exit 1
           fi
           model_dir=$1
       fi
       perms_sh_path="$model_dir/set_perms.sh"
       list_file="$model_dir/listing_file"
    fi

    if [ ! -f $model_dir/tarlisting ]
    then
        echo " ";echo " ";echo " "
        echo "A snapshot of $model_dir does not exist in $model_dir"
        echo "Do you want to create one?"
        echo " ";echo " "
        read ans
        if [ "$ans" = "Y" -o "$ans" = "y" ]
        then
            cd $model_dir
            echo " ";echo " "
            if [ $model_dir = "/h/NTCSSS" ]
            then
                tar cvf /tmp/perms.tar /etc/ntcss_system_settings.ini *
            else
                tar cvf /tmp/perms.tar *
            fi
            tar tvf /tmp/perms.tar > $model_dir/tarlisting 
        else
            echo "Mission aborted.."
            exit 0
        fi
    fi

    if [ -f "$model_dir/set_perms.sh" ]
    then
        mv $model_dir/set_perms.sh $model_dir/set_perms.last
        touch $model_dir/set_perms.sh
    fi

    echo "#!/bin/sh" > $model_dir/set_perms.sh
    echo "spl=$model_dir/set_perms.log" >> $model_dir/set_perms.sh
    echo "rm -f $model_dir/set_perms.log" >> $model_dir/set_perms.sh
    echo "touch $model_dir/set_perms.log" >> $model_dir/set_perms.sh
    echo "missing_files=\"n\"" >> $model_dir/set_perms.sh

    {
    while (true)
    do

        read pathname

        if [ "$?" -ne "0" ]
        then
            echo "DONE."
            break
        fi

        if [ -z $pathname -o $pathname = "" ]
        then
            continue
        fi

        #Extract relative pathname from tarlisting
        p=`echo $pathname | cut -d " " -f 8`

        if [ "$p" != "/etc/ntcss_system_settings.ini" ]
        then
            fullpath="$model_dir/$p"
        else
            fullpath=$p
        fi

#        #Skip 'logs' files, but do the logs directory
#        logstat=`echo $fullpath | grep "/h/NTCSSS/logs"`

#        if [ $? = "0" ]
#        then
#            dirstat=`echo $fullpath | grep "/h/NTCSSS/logs/$"`
#            if [ $? != "0" ]
#            then
#                #Skip logs files
#                echo "Skipping $fullpath"
#                continue
#            fi
#        fi

#        #Skip 'bb_msgs' files, but do the bb_msgs directory
#        msgstat=`echo $fullpath | grep "/h/NTCSSS/message/bb_msgs_../."`
#        if [ $? = "0" ]
#        then
#            #Skip bb_msgs files
#            echo "Skipping bb_msgs:"
#            echo "..$fullpath"
#            continue
#        fi

#        msgstat=`echo $fullpath | grep "/h/NTCSSS/message/0../."`
#        if [ $? = "0" ]
#        then
#            #Skip files under numerical message directories
#            echo "Skipping numerical:"
#            echo "..$fullpath"
#            continue
#        fi

#        msgstat=`echo $fullpath | grep "/h/NTCSSS/markers_../."`
#        if [ $? = "0" ]
#        then
#            #Skip files under marker directories
#            echo "Skipping marker:"
#            echo "..$fullpath"
#            continue
#        fi

#        msgstat=`echo $fullpath | grep "/lock_file"`
#        if [ $? = "0" ]
#        then
#            #Skip lock files
#            echo "Skipping lock file:"
#            echo "..$fullpath"
#            continue
#        fi

#        msgstat=`echo $fullpath | grep "/h/NTCSSS/spool/lp/PIPES/"`
#        case $msgstat in
#            "/h/NTCSSS/spool/lp/PIPES/$")
#            ;;
#            "/h/NTCSSS/spool/lp/PIPES/.")
#            echo "Skipping files under PIPES.."
#            continue
#            ;;
#            "/h/NTCSSS/spool/lp/NTCSS_LP_SEQUENCE_FILE")
#            ;;
#            "/h/NTCSSS/spool/lp/etc/$")
#            ;;
#            "/h/NTCSSS/spool/lp/etc/LPNODATA_MSG")
#            ;;
#            "/h/NTCSSS/spool/lp/etc/LPNOFILE_MSG")
#            ;;
#            "/h/NTCSSS/spool/lp/pipes/$")
#            ;;
#            "/h/NTCSSS/spool/lp/pipes/dm_input_pipe")
#            ;;
#            "/h/NTCSSS/spool/lp/pipes/dm_ret_pipe")
#            ;;
#            "/h/NTCSSS/spool/lp/ntdrivers/$")
#            ;;
#        esac

        if [ "$p" = "tarlisting" ]
        then
            continue
        fi

        if [ "$p" = "set_perms.log" ]
        then
            continue
        fi

        perms=`echo $pathname | cut -c 1-9`
        
        a1=`echo "$perms" | cut -c 1`; a2=`echo "$perms" | cut -c 2`
        a3=`echo "$perms" | cut -c 3`; a4=`echo "$perms" | cut -c 4`
        a5=`echo "$perms" | cut -c 5`; a6=`echo "$perms" | cut -c 6`
        a7=`echo "$perms" | cut -c 7`; a8=`echo "$perms" | cut -c 8`
        a9=`echo "$perms" | cut -c 9`
    
        
        if [ "$a1" != "-" ]
        then
            cmd1="u+r"
        else
            cmd1="u-r"
        fi
        if [ "$a2" != "-" ]
        then
            cmd2="u+w"
        else
            cmd2="u-w"
        fi
        if [ "$a3" != "-" ]
        then
            cmd3="u+x"
        else
            cmd3="u-x"
        fi
        if [ "$a3" = "s" ]
        then
            cmd3="u+s"
        fi
        if [ "$a4" != "-" ]
        then
            cmd4="g+r"
        else
            cmd4="g-r"
        fi
        if [ "$a5" != "-" ]
        then
            cmd5="g+w"
        else
            cmd5="g-w"
        fi
        if [ "$a6" != "-" ]
        then
            cmd6="g+x"
        else
            cmd6="g-x"
        fi
        if [ "$a7" != "-" ]
        then
            cmd7="o+r"
        else
            cmd7="o-r"
        fi
        if [ "$a8" != "-" ]
        then
            cmd8="o+w"
        else
            cmd8="o-w"
        fi
        if [ "$a9" != "-" ]
        then
            cmd9="o+x"
        else
            cmd9="o-x"
        fi
        
        accum="$cmd1,$cmd2,$cmd3,$cmd4,$cmd5,$cmd6,$cmd7,$cmd8,$cmd9"
    
        echo "CMD: chmod $accum "
        echo ".. $p"
    
        echo "if [ -f \"$fullpath\" -o -d \"$fullpath\" -o -p \"$fullpath\" ]" >> $model_dir/set_perms.sh
        echo "then" >> $model_dir/set_perms.sh
        echo "    echo \"$p\"" >> $model_dir/set_perms.sh
        echo "    chmod $accum $fullpath" >> $model_dir/set_perms.sh
        echo "else" >> $model_dir/set_perms.sh
        echo "    echo \"$fullpath NO LONGER EXISTS !!\" >> \$spl" >> $model_dir/set_perms.sh
        echo "    missing_files=\"y\"" >> $model_dir/set_perms.sh
        echo "fi" >> $model_dir/set_perms.sh
    
    done
    } < $model_dir//tarlisting

    echo "if [ \$missing_files = \"y\" ]" >> $model_dir/set_perms.sh
    echo "then" >> $model_dir/set_perms.sh
    echo "    echo \" \"" >> $model_dir/set_perms.sh
    echo "    echo \" \"" >> $model_dir/set_perms.sh
    echo "    echo \"    There were missing files!, chk set_perms.log for details..\"" >> $model_dir/set_perms.sh
    echo "    echo \" \"" >> $model_dir/set_perms.sh
    echo "    echo \" \"" >> $model_dir/set_perms.sh
    echo "else" >> $model_dir/set_perms.sh
    echo "    echo \"Set permissions complete with no missing files.\"" >> $model_dir/set_perms.sh 
    echo "    echo \"Set permissions complete with no missing files.\" >> \$spl" >> $model_dir/set_perms.sh 
    echo "fi" >> $model_dir/set_perms.sh
    
    chmod 700 $model_dir/set_perms.sh

    cd $curdir

