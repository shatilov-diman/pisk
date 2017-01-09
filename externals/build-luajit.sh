#!/bin/bash

pushd ${BUILD_ROOT}

# 1. Get sources
LUAJIT_PAGE=http://luajit.org/download/
LUAJIT_SRC=LuaJIT-2.0.4
LUAJIT_ROOT=luajit

BUILD_TARGET=
INCLUDE_NAME_LUAJIT=luajit

function download_and_unpack() {
    TARBALL=$1.tar.gz
    echo $TARBALL
    if [ ! -f $TARBALL ]
    then
	curl -O $2/$TARBALL || exit 1
	rm -rf $1
        tar -xzf $TARBALL
        rm $3
        ln -s $1 $3
    fi
}
download_and_unpack $LUAJIT_SRC $LUAJIT_PAGE $LUAJIT_ROOT


# 2. Build sources
CPU_COUNT=2 #`sysctl -n hw.logicalcpu_max`

# 2.3 Build LIBRARY for Android
pushd $LUAJIT_ROOT

SONAME=libluajit.so

export PATH=${ANDROID_TOOLCHAIN_ROOT}/bin:$PATH
export HOST_CFLAGS=-m32
export HOST_LDFLAGS=-m32
export CROSS=${ANDROID_CROSS}-
export TARGET_SYS=Linux
export TARGET_CFLAGS="${ANDROID_CFLAGS} --sysroot=${ANDROID_SYSROOT}"
export TARGET_LDFLAGS="${ANDROID_LDFLAGS} -Wl,-soname,${SONAME}"
export TARGET_SHLDFLAGS="${ANDROID_LDFLAGS} -Wl,-soname,${SONAME}"

# --with-ogg does not work
make clean && make -j ${CPU_COUNT} || exit 1

cp -f src/lua.h         ${INSTALL_DIR}/include/
cp -f src/lua.hpp       ${INSTALL_DIR}/include/
cp -f src/luaconf.h     ${INSTALL_DIR}/include/
cp -f src/lualib.h      ${INSTALL_DIR}/include/
cp -f src/lauxlib.h     ${INSTALL_DIR}/include/
cp -f src/luajit.h      ${INSTALL_DIR}/include/

cp -f src/luajit        ${INSTALL_DIR}/bin/
cp -f src/libluajit.a   ${INSTALL_DIR}/lib/
cp -f src/libluajit.so  ${INSTALL_DIR}/lib/
popd

popd

