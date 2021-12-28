#!/bin/bash
#cd $envvar

PS3='Please enter your choice: '
options=("unnamed pipe" "named pipe" "socket" "shared memory" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "unnamed pipe")
            echo "you chose unnamed pipe"
            #cd sources/unnamed
            ./exec/sources/unnamed/unnamed
            ;;
        "named pipe")
            echo "you chose named pipe"
            #cd sources/named
            ./exec/sources/named/named
            ;;
        "socket")
            echo "you chose socket"
            #cd sources/socket
            ./exec/sources/socket/socket
            ;;
        "shared memory")
            echo "you chose shared memory"
            #cd sources/shared
            ./exec/sources/shared/shared
            ;;
        "Quit")
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done
