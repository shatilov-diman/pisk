#!/bin/bash

pushd ${COMMON_BUILD_ROOT}

# 1. Get sources
#https://dl.google.com/android/android-sdk_r24.4.1-linux.tgz
SDK_PAGE=https://dl.google.com/android
SDK_SRC=android-sdk_r24.4.1-linux
SDK_OUTDIR_NAME=android-sdk-linux
SDK_ROOT_DIR=android-sdk

function download_and_unpack() {
    TARBALL=$1.tgz
    echo $TARBALL
    if [ ! -f $TARBALL ]
    then
	curl -O $2/$TARBALL || exit 1
	rm -rf $1
        tar -zxvf $TARBALL
        rm $3
        ln -s $4 $3
    fi

    (sleep 5 && while [ 1 ]; do sleep 1; echo y; done) | ${SDK_ROOT_DIR}/tools/android update sdk --no-ui --filter tools,platform-tools,build-tools-25.0.1,android-15,android-21
}
download_and_unpack $SDK_SRC $SDK_PAGE $SDK_ROOT_DIR $SDK_OUTDIR_NAME


popd

