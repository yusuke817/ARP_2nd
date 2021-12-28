#!/bin/bash
cd $envvar

PS3='Please enter your choice: '
options=("unnamed pipe" "named pipe" "socket" "shared memory" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "unnamed pipe")
            echo "you chose unnamed pipe"
            cd sources/unnamed
            ./unnamed
            ;;
        "named pipe")
            echo "you chose named pipe"
            cd sources/named
            ./named
            ;;
        "socket")
            echo "you chose socket"
            cd sources/socket
            ./socket
            ;;
        "shared memory")
            echo "you chose shared memory"
            cd sources/shared
            ./shared
            ;;
        "Quit")
            break
            ;;
        *) echo "invalid option $REPLY";;
    esac
done