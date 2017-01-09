#!/bin/bash

pushd ${BUILD_ROOT}

# 1. Get sources
GIT_PAGE=http://github.com/kcat/openal-soft.git
LIB_ROOT=openal
BUILD_TARGET=
INCLUDE_NAME=AL

if [ ! -d ${LIB_ROOT} ]
then
	git clone $GIT_PAGE $LIB_ROOT || exit 1
fi

# 2. Build sources
CPU_COUNT=2 #`sysctl -n hw.logicalcpu_max`
#CURL_EXTRA="--enable-ipv6 --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-proxy --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-sspi --disable-manual"
#LIBRARY_STATIC=lib/.libs/libcurl.a
#LIBRARY_SHARED=lib/.libs/libcurl.so

# 2.3.2 Build LIBRARY for Android
pushd $LIB_ROOT

CMAKE_TOOLCHAIN_FILE="${PISK_ROOT}/cmake/toolchain.android.${ANDROID_ARCH}.cmake"

SONAME=libopenal.so

export PATH=${ANDROID_TOOLCHAIN_ROOT}/bin:$PATH
export CROSS_PREFIX=${ANDROID_CROSS}-
export CC=${CROSS_PREFIX}gcc
export CXX=${CROSS_PREFIX}g++
export AR=${CROSS_PREFIX}ar
export RANLIB=${CROSS_PREFIX}ranlib
export CROSS_SYSROOT=${ANDROID_SYSROOT}
export CFLAGS=${ANDROID_CFLAGS}
export CPPFLAGS=${ANDROID_CPPFLAGS}
export LDFLAGS=${ANDROID_LDFLAGS}

rm build -r
mkdir build

cmake -E chdir build cmake .. \
	-DCMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE}" \
	-DCMAKE_FIND_ROOT_PATH="${ANDROID_SYSROOT}" \
	-DCMAKE_SYSROOT="${ANDROID_SYSROOT}" \
	-DTOOLCHAIN_ROOT="${ANDROID_TOOLCHAIN_ROOT}" \
	-DTOOLCHAIN_PREFIX="${CROSS_PREFIX}" \
	-DTOOLCHAIN_SYSROOT="${ANDROID_SYSROOT}" \
	-DEXECUTABLE_OUTPUT_PATH="${INSTALL_DIR}/bin" \
	-DLIBRARY_OUTPUT_PATH="${INSTALL_DIR}/lib" \
	-DCMAKE_BUILD_TYPE=Release

cmake --build build -- ${BUILD_TARGET}
cp -f -r include/${INCLUDE_NAME} ${INSTALL_DIR}/include
#cp -f ${LIBRARY_STATIC} ${INSTALL_DIR}/lib
#cp -f ${LIBRARY_SHARED} ${INSTALL_DIR}/lib
popd

popd

