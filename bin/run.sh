#!/usr/bin/env bash

killall gamma

rm -rf *.log

# set the running environment
path=$(pwd)
export LD_LIBRARY_PATH=$path

sleep 1

nohup ./gamma login.ini -print >> login.log 2>&1 &

sleep 1

#nohup ./gamma config2.ini -print >> node2.log 2>&1 &
