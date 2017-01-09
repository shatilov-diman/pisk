#!/bin/bash

pushd ${COMMON_BUILD_ROOT}

# 1. Get sources
#https://dl.crystax.net/builds/902/linux/crystax-ndk-10.3.2-b902-linux-x86_64.tar.xz
NDK_DAYNO=908
NDK_PAGE=https://dl.crystax.net/builds/${NDK_DAYNO}/linux
NDK_SRC=crystax-ndk-10.3.2-b${NDK_DAYNO}-linux-x86_64
NDK_OUTDIR_NAME=crystax-ndk-10.3.2
NDK_ROOT_DIR=crystax-ndk

function download_and_unpack() {
    TARBALL=$1.tar.xz
    echo $TARBALL
    if [ ! -f $TARBALL ]
    then
	curl -O $2/$TARBALL || exit 1
	rm -rf $1
        tar -xJf $TARBALL
        rm $3
        ln -s $4 $3
    fi
}
download_and_unpack $NDK_SRC $NDK_PAGE $NDK_ROOT_DIR $NDK_OUTDIR_NAME

popd

