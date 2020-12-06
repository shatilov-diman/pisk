#!/bin/bash

if [ -z "$1" ]
then
	echo "USAGE: ./run_debug.sh [package_name]"
else
	PID=`adb shell ps | grep $1 | cut -d' ' -f4`
	if [ -z "$PID" ]
	then
		echo "Process for '$1' not found"
	else
		adb forward tcp:5039 localfilesystem:/data/data/$1/debug-pipe
		adb shell run-as $1 /data/data/$1/lib/gdbserver.so +debug-pipe --attach $PID
	fi
fi

