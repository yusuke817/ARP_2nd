#!/bin/bash
pwd=$(pwd)
chmod +x ./run.sh
unzip -d $1 sources.zip
cd $1
export envvar=$1

gcc -o ./sources/unnamed/unnamed ./sources/unnamed/unnamed.c
chmod +x ./sources/unnamed/unnamed
gcc -o ./sources/named/named ./sources/named/named.c
chmod +x ./sources/named/named
gcc -o ./sources/socket/socket ./sources/socket/socket.c
chmod +x ./sources/socket/socket
gcc -pthread -o ./sources/shared/shared ./sources/shared/shared.c
chmod +x ./sources/shared/shared
#gcc -o ./sources/main/main ./sources/Main/main.c
#chmod +x ./sources/main/main

cd $pwd
