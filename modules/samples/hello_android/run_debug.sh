
if [ -z "$2" ]
then
	adb shell ps | grep $1
else
	adb forward tcp:5039 localfilesystem:/data/data/$1/debug-pipe
	adb shell run-as $1 /data/data/$1/lib/gdbserver.so +debug-pipe --attach $2
fi

