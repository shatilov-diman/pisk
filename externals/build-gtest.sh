#!/bin/bash

pushd ${BUILD_ROOT}

# 1. Get sources
GIT_PAGE=https://github.com/google/googletest.git
LIB_ROOT=googletest
BUILD_TARGET=
GTEST_INCLUDE_NAME=googletest/include/gtest
GMOCK_INCLUDE_NAME=googlemock/include/gmock

if [ ! -d ${LIB_ROOT} ]
then
	git clone $GIT_PAGE $LIB_ROOT || exit 1
fi

# 2. Build sources
CPU_COUNT=2 #`sysctl -n hw.logicalcpu_max`

pushd $LIB_ROOT

CMAKE_TOOLCHAIN_FILE="${PISK_ROOT}/cmake/toolchain.android.${ANDROID_ARCH}.cmake"

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
mkdir build

cmake -E chdir build cmake .. \
	-DCMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE}" \
	-DCMAKE_FIND_ROOT_PATH="${ANDROID_SYSROOT}" \
	-DTOOLCHAIN_ROOT="${ANDROID_TOOLCHAIN_ROOT}" \
	-DTOOLCHAIN_PREFIX="${CROSS_PREFIX}" \
	-DTOOLCHAIN_SYSROOT="${ANDROID_SYSROOT}" \
	-DEXECUTABLE_OUTPUT_PATH="${INSTALL_DIR}/bin" \
	-DLIBRARY_OUTPUT_PATH="${INSTALL_DIR}/lib" \
	-DCMAKE_BUILD_TYPE=Release

cmake --build build -- ${BUILD_TARGET}
cp -f -r ${GTEST_INCLUDE_NAME} ${INSTALL_DIR}/include
cp -f -r ${GMOCK_INCLUDE_NAME} ${INSTALL_DIR}/include
popd

popd

