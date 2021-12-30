#!/bin/bash
#cd $envvar

PS3='Please enter your choice: '
options=("unnamed pipe" "named pipe" "socket" "shared memory" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "unnamed pipe")
            echo "you chose unnamed pipe"
            ./exec/sources/unnamed/unnamed
            sleep 3
            ;;
        "named pipe")
            echo "you chose named pipe"
            ./exec/sources/named/named
            sleep 3
            ;;
        "socket")
            echo "you chose socket"
            ./exec/sources/socket/socket
            sleep 3
            ;;
        "shared memory")
            echo "you chose shared memory"
            ./exec/sources/shared/shared
            sleep 3
            ;;
        "Quit")
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done
